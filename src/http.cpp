#include "http.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <csignal>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>

Config::Config(const std::string addr, const int port, const int resp_status,
               const std::string resp_body)
    : addr(addr), port(port), resp_status(resp_status), resp_body(resp_body) {}

Server::Server() : conf(std::make_shared<Config>()), running(true) {}

Server::Server(const std::shared_ptr<Config> conf)
    : conf(conf), running(true) {}

int Server::run() {
  std::signal(SIGPIPE, SIG_IGN);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Failed to open socket." << std::endl;
    exit(1);
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->conf->port);
  inet_aton(this->conf->addr.c_str(), &addr.sin_addr);

  if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
    std::cerr << "Failed to bind socket." << std::endl;
    exit(1);
  }

  if (listen(sockfd, 10) < 0) {
    std::cerr << "Failed to listen socket." << std::endl;
    exit(1);
  }

  std::cout << "Listening on " << this->conf->addr << ":" << this->conf->port
            << " ..." << std::endl;

  std::queue<std::unique_ptr<Worker>> worker_pool;

  while (this->running || !worker_pool.empty()) {
    sockaddr_in peer_addr;
    socklen_t peer_addr_size;

    int new_sockfd = accept(sockfd, (sockaddr*)&peer_addr, &peer_addr_size);
    if (new_sockfd < 0) {
      std::cerr << "Failed to accpet message." << std::endl;
      exit(1);
    }

    std::cout << "Connected from " << inet_ntoa(peer_addr.sin_addr)
              << std::endl;

    worker_pool.push(
        std::make_unique<Worker>(this->conf, new_sockfd, peer_addr));

    while (!worker_pool.empty() && (worker_pool.front())->completed) {
      worker_pool.pop();
    }
  }

  return shutdown(sockfd, SHUT_RDWR);
}

Worker::Worker(const std::shared_ptr<Config> svconf, int peer_sockfd,
               const sockaddr_in peer_addr)
    : svconf(svconf),
      peer_sockfd(peer_sockfd),
      peer_addr(peer_addr),
      completed(false),
      worker_thread(std::make_unique<std::thread>(&Worker::run, this)) {}

Worker::~Worker() { this->worker_thread->join(); }

void Worker::run() {
  this->serve();
  this->completed = true;
}

int Worker::serve() {
  char buffer[256];
  if (recv(this->peer_sockfd, buffer, 255, 0) < 0) {
    std::cerr << "Failed to receive message." << std::endl;
    exit(1);
  }

  std::cout << buffer << std::endl;

  const char* resp = this->svconf->resp_body.c_str();
  const int resp_size = this->svconf->resp_body.size();
  send(this->peer_sockfd, resp, resp_size, 0);

  return shutdown(this->peer_sockfd, SHUT_RDWR);
}