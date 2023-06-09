#pragma once

class WebServer {
 public:
  WebServer();
  WebServer(int port);
  void run();

 private:
  int port = 80;
};