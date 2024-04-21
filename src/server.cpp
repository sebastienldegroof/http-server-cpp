#include "tcp_server.h"

int main(int argc, char **argv) {

  std::string directory {};
  for (int i = 0; i < argc-1; i++) {
    if ( argv[i] == "--directory")
      directory = argv[i+1];
  }
  tcp_server server;
  if (directory == "")
    server = tcp_server(4221);
  else
    server = tcp_server(4221, directory);

  server.run_server();

  return 0;
}
