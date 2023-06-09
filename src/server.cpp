#include "server.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <csignal>
#include <cerrno>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>

Server::Server() : conf(std::make_unique<Config>()), running(false) {}

Server::Server(std::unique_ptr<Config> conf)
    : conf(std::move(conf)), running(false) {}

int Server::run() {
  this->running = true;

  int listen_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sd < 0) {
    std::perror("socket() failed");
    exit(1);
  }

  int on = 1;
  if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    std::perror("setsockopt() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(1);
  }

  // non-blocking IO
  if (ioctl(listen_sd, FIONBIO, (char*)&on) < 0) {
    std::perror("ioctl() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(1);
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->conf->port);
  inet_aton(this->conf->addr.c_str(), &addr.sin_addr);

  if (bind(listen_sd, (sockaddr*)&addr, sizeof(addr)) < 0) {
    std::perror("bind() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(1);
  }

  if (listen(listen_sd, 32) < 0) {
    std::perror("listen() failed");
    shutdown(listen_sd, SHUT_RDWR);
    exit(1);
  }

  std::cout << "Listening on " << this->conf->addr << ":" << this->conf->port
            << " ..." << std::endl;

  fd_set master_set, working_set;
  FD_ZERO(&master_set);
  FD_SET(listen_sd, &master_set);

  char buffer[BUFFER_SIZE];

  timeval timeout;
  timeout.tv_sec = 1 * 60;
  timeout.tv_usec = 0;

  int max_sd = listen_sd;

  int rc, new_sd;
  do {
    std::memcpy(&working_set, &master_set, sizeof(master_set));

    rc = select(max_sd + 1, &working_set, nullptr, nullptr, &timeout);
    if (rc < 0) {
      std::perror("select() failed");
      break;
    }

    if (rc == 0) {
      std::cerr << "timeout" << std::endl;
      break;
    }

    int desc_ready = rc;
    for (int i_sd = 0; i_sd <= max_sd && desc_ready > 0; i_sd++) {
      if (FD_ISSET(i_sd, &working_set) == false) continue;

      desc_ready -= 1;

      if (i_sd == listen_sd) {
        // file descriptor is listening socket. accecpt new connection.
        bool sv_closed = false;
        do {
          new_sd = accept(listen_sd, nullptr, nullptr);
          if (new_sd < 0) {
            if (errno != EWOULDBLOCK) {
              std::perror("accept() failed");
              sv_closed = true;
            }
            break;
          }

          FD_SET(new_sd, &master_set);

          if (new_sd > max_sd) {
            max_sd = new_sd;
          }
        } while (new_sd != -1);

        if (sv_closed) {
          this->running = false;
        }
      } else {
        // file descriptor is not listening socket. read data from the socket.
        bool conn_closed = false;
        do {
          rc = recv(i_sd, buffer, sizeof(buffer), 0);
          if (rc < 0) {
            if (errno != EWOULDBLOCK) {
              std::perror("recv() failed");
              conn_closed = true;
            }
            break;
          }

          if (rc == 0) {
            std::cout << "connection closed by client" << std::endl;
            conn_closed = true;
            break;
          }

          std::string body = this->conf->resp_body;
          rc = send(i_sd, body.c_str(), body.size(), 0);
          if (rc < 0) {
            std::perror("send() failed");
            conn_closed = true;
            break;
          }
        } while (1);

        if (conn_closed) {
          shutdown(i_sd, SHUT_RDWR);
          FD_CLR(i_sd, &master_set);
          if (i_sd == max_sd) {
            while (FD_ISSET(max_sd, &master_set) == false) {
              max_sd -= 1;
            }
          }
        }
      }
    }
  } while (this->running);

  for (int i_sd = 0; i_sd <= max_sd; i_sd++) {
    if (FD_ISSET(i_sd, &master_set)) {
      shutdown(i_sd, SHUT_RDWR);
    }
  }

  return 0;
}