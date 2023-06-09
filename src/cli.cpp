#include "cli.h"

#include <getopt.h>

#include <iostream>

#include "http.h"
#include "config.h"

static struct option longopts[] = {{"help", no_argument, NULL, 'h'},
                                   {"version", no_argument, NULL, 'v'},
                                   {"port", required_argument, NULL, 'p'},
                                   {"code", required_argument, NULL, 'c'},
                                   {"body", required_argument, NULL, 'b'}};

static void usage() {
  std::cout << "Usage: httpsv [<options>...]" << std::endl
            << "Options:" << std::endl
            << " -p --port    Assign specified server port" << std::endl
            << " -c --code    Return specified HTTP status code" << std::endl
            << " -b --body    Return specified HTTP body" << std::endl
            << " -v --version Print version information and exit" << std::endl
            << " -h --help    Print Help (this message) and exit" << std::endl;
}

void CLI::parseOpts(int argc, char const* argv[]) {
  int opt, longidx;
  while ((opt = getopt_long(argc, const_cast<char* const*>(argv),
                            "hvp:c:b:", longopts, &longidx)) != -1) {
    switch (opt) {
      case 'p':
        std::cout << optarg << std::endl;
        break;
      case 'c':
        std::cout << optarg << std::endl;
        break;
      case 'b':
        std::cout << optarg << std::endl;
        break;
      case 'v':
        std::cout << APP_VERSION << std::endl;
        break;
      case 'h':
      default:
        usage();
        exit(1);
    }
  }
}

int CLI::run() {
  Server* sv = new Server;
  sv->run();
  return 0;
}