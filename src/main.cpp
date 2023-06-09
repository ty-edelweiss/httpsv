#include "cli.hpp"
#include "config.hpp"

int main(int argc, const char* argv[]) {
  CLI* cli = new CLI;
  cli->parseOpts(argc, argv);
  return cli->run();
}