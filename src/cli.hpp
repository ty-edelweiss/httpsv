#pragma once

#include <string>

struct CLIOptions {
  std::string addr;
  int port;
  int status;
  std::string body;
};

class CLI {
 public:
  CLI();
  int run();
  void parseOpts(const int argc, const char* argv[]);

 private:
  CLIOptions opts;
};