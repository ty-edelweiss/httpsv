#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include <memory>
#include <thread>
#include <string>

class Config {
 public:
  Config(const std::string addr, const int port, const int resp_status,
         const std::string resp_body);

  std::string addr = "0.0.0.0";
  int port = 80;
  int resp_status = 200;
  std::string resp_body = "Ok";
};

class Server {
 public:
  Server();
  Server(const std::shared_ptr<Config> conf);
  int run();

  bool running;

 private:
  const std::shared_ptr<Config> conf;
};

class Worker {
 public:
  Worker(const std::shared_ptr<Config> svconf, int sockfd,
         const sockaddr_in peer_addr);
  ~Worker();
  void run();

  bool completed;

 private:
  int serve();

  const std::shared_ptr<Config> svconf;
  const std::unique_ptr<std::thread> worker_thread;
  const int peer_sockfd;
  const sockaddr_in peer_addr;
};