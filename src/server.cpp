#include <getopt.h>

#include "tcp_server.h"

int main(int argc, char **argv) {

  int opt {};
  int opt_index {};
  std::string directory {};
  struct option long_options[] = {
    {"directory", required_argument, 0, 'd'},
    {0, 0, 0, 0}
  };

  while ( (opt = getopt_long (argc, argv, "d:", long_options, &opt_index)) != -1 ){
    if (opt == 'd')
      directory = optarg;
  }

  std::cout << directory << "\n";

  if (directory == ""){
    tcp_server server = tcp_server(4221);
    server.run_server();
  } else {
    tcp_server server = tcp_server(4221, directory);
    server.run_server();
  }
  return 0;
}
