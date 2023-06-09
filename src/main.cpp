#include <iostream>

#include "CLI.h"
#include "config.h"

int main(int argc, char const* argv[]) {
  CLI* cli = new CLI;
  cli->parseOpts(argc, argv);
  return cli->run();
}
