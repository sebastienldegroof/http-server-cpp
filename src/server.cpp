#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char **argv) {
  
  // Create the server socket. fd is file descriptor, because everything
  // in linux is a file
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Set the socket option to reuse same address and port.
  // Multiple sockets may bind to the same address and port combination 
  // with SO REUSEPORT enabled.
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  // Declare the address family, IP address, and port to listen on
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);
  
  // Bind to the socket to the address parameters
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }
  
  // Listen for incoming data. New connections are added to a queue (backlog)
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  
  // Define a client address variable
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  
  std::cout << "Waiting for a client to connect...\n";
  
  // accept() uses pointers to the client address variable and writes the
  // address information to it
  int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  if (client_fd < 0){
    std::cerr << "connection failed\n";
    return 1;
  }
  std::cout << "Client connected\n";

  // read data from the new client_socket
  char buffer[1024] = {0};
  recv(client_fd, buffer, sizeof(buffer), 0);
  std::cout << "Message from client: " << buffer << std::endl;
  
  // respond to the client message with 200 OK
  const char* response_200 = "HTTP/1.1 200 OK\r\n\r\n";
  send(client_fd, response_200, sizeof(response_200), 0);

  close(server_fd);

  return 0;
}
