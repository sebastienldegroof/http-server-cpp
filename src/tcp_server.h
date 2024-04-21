#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

class tcp_server {
  public:
    tcp_server(int port);
    int run_server();

  private:
    int server_port;
    int server_fd;
    int client_fd;
    int max_fd;
    int bytes_received;
    int bytes_sent;
    struct sockaddr_in server_addr;
    struct sockaddr_storage client_addr;
    fd_set socket_fds;
    fd_set temp_fds;
};
