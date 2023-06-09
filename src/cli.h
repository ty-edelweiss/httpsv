#pragma once

#include <string>

class CLI {
 public:
  int run();
  void parseOpts(int argc, char const* argv[]);
};