#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include <memory>
#include <thread>
#include <string>

#define BUFFER_SIZE 1024

struct Config {
  std::string addr = "0.0.0.0";
  int port = 80;
  int resp_status = 200;
  std::string resp_body = "Ok";
};

class Server {
 public:
  Server();
  Server(const std::unique_ptr<Config> conf);
  int run();

  bool running;

 private:
  const std::unique_ptr<Config> conf;
};