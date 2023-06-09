#include "cli.h"

#include <getopt.h>

#include <iostream>

#include "http.h"
#include "config.h"

static option longopts[] = {{"help", no_argument, NULL, 'h'},
                            {"version", no_argument, NULL, 'v'},
                            {"addr", required_argument, NULL, 'a'},
                            {"port", required_argument, NULL, 'p'},
                            {"status", required_argument, NULL, 's'},
                            {"body", required_argument, NULL, 'b'}};

static void usage() {
  std::cout << "Usage: httpsv [<options>...]" << std::endl
            << "Options:" << std::endl
            << " -a --addr    Assign specified server address" << std::endl
            << " -p --port    Assign specified server port" << std::endl
            << " -s --status  Return specified HTTP status code" << std::endl
            << " -b --body    Return specified HTTP body" << std::endl
            << " -v --version Print version information and exit" << std::endl
            << " -h --help    Print Help (this message) and exit" << std::endl;
}

static void version() { std::cout << APP_VERSION << std::endl; }

void CLI::parseOpts(const int argc, const char* argv[]) {
  int opt, longidx;
  while ((opt = getopt_long(argc, const_cast<char* const*>(argv),
                            "hva:p:s:b:", longopts, &longidx)) != -1) {
    switch (opt) {
      case 'a':
        this->conf->addr = optarg;
        break;
      case 'p':
        this->conf->port = std::stoi(optarg);
        break;
      case 's':
        this->conf->resp_status = std::stoi(optarg);
        break;
      case 'b':
        this->conf->resp_body = optarg;
        break;
      case 'v':
        version();
        exit(1);
      case 'h':
      default:
        usage();
        exit(1);
    }
  }
}

int CLI::run() {
  Server* sv = new Server(this->conf);
  sv->run();
  return 0;
}