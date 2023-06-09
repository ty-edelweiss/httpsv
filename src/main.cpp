#include "cli.h"
#include "config.h"

int main(int argc, const char* argv[]) {
  CLI* cli = new CLI;
  cli->parseOpts(argc, argv);
  return cli->run();
}