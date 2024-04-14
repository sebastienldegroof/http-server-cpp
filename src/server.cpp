#include "tcp_server.h"

int main(int argc, char **argv) {
  
  tcp_server server = tcp_server(4221);
  server.run_server();

  return 0;
}
