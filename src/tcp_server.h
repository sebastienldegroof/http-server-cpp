#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>

class tcp_server {
  public:
    tcp_server(int port);
    int run_server();

  private:
    int server_port;
    struct sockaddr_in server_addr;
};
