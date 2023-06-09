#pragma once

#include "http.h"

#include <memory>

class CLI {
 public:
  int run();
  void parseOpts(const int argc, const char* argv[]);

 private:
  const std::shared_ptr<Config> conf;
};