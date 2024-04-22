#include "tcp_server.h"
#include "http_server.h"

tcp_server::tcp_server() {
  dir_path = "./";
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);
}

tcp_server::tcp_server(int port) : server_port(port) {
  dir_path = "./";
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(server_port);
}

tcp_server::tcp_server(int port, std::string directory) : server_port(port), dir_path(directory) {
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(server_port);
}

int tcp_server::run_server() {

  // Create the server socket. fd is file descriptor, because everything
  // in linux is a file
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  // We need to set the server socket to non-blocking because we want our
  // program to be able to call on all the sockets while we iterate thru them.
  // The child client sockets inherit the server socket's configurations.
  fcntl(server_fd, F_SETFL, O_NONBLOCK);
  
  // Set the socket option to reuse same address and port.
  // Multiple sockets may bind to the same address and port combination 
  // with SO REUSEPORT enabled.
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  // Bind to the socket to the address parameters
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port\n";
    return 1;
  }
  
  // Listen for incoming data. New connections are added to a queue (backlog)
  if ( listen(server_fd, 5) != 0 ) {
    std::cerr << "listen failed\n";
    return 1;
  }
  std::cout << "Waiting for a client to connect...\n";

  // set of socket file descriptors that will be monitored with select()
  FD_ZERO(&socket_fds);

  // select() modifies the pointers of the fds so we need a working copy
  FD_ZERO(&temp_fds);

  // add the server socket to the set
  FD_SET(server_fd, &socket_fds);

  // unix file descriptors are just integer IDs for an open socket. 
  // max_fd represents the highest open socket, which is used to
  // discover any new open sockets created after the server socket
  max_fd = server_fd;

  while(true) {

    // copy the fd set to the working copy
    memcpy(&temp_fds, &socket_fds, sizeof(socket_fds));

    // select() blocks the program until activity or the timeout
    // value of -1 is an error, 0 means nothing connected.
    // select() returns the number of ready descriptors as an int.
    // select() modifies the fd_sets refernced to only contains the
    // ready fds.
    if ( select(max_fd + 1, &temp_fds, NULL, NULL, NULL) != 1 ){
      std::cerr << "Select error or no connections received\n";
      return 1;
    }

    // Now, loop over the working fd_set and check for activity
    for (int i = 0; i <= max_fd; ++i){
      if (FD_ISSET(i, &temp_fds)) {

        // If the ready descriptor is the server socket, then
        // one or more connections are being requested.
        // Iterate through them, accept, and add them to the
        // fd_set
        if (i == server_fd) {

          // accept() returns the fd of a successful connection, or -1 on error.
          client_fd = 0;
          while ( client_fd != -1 ) {

            // accept() the new connection
            socklen_t addrlen = sizeof (client_addr);
            client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addrlen);

            if ( client_fd < 0 ){
              std::cout << "No more connections to accept.\n";
              break;
            }
            std::cout << "Client connected.\n";

            // add to the master fd_set
            FD_SET(client_fd, &socket_fds);
            // keep track if this the largest fd
            if ( client_fd > max_fd ) max_fd = client_fd;
          }
        }

        // If it's not the server socket, receive the data,
        // process the request, and remove them from the fd_set
        else {

          // read data from the ready descriptor
          char buffer[1024] = {0};
          bytes_received = recv(i, buffer, sizeof(buffer), 0);
          if (bytes_received < 0){
            std::cerr << "receive failed\n"; 
            return 1;
          }
          std::cout << "Message from client: \n" << buffer << std::endl;

          // send the data to the HTTP server for processing
          std::string response = process_request(buffer, dir_path);

          // and respond with that request
          bytes_sent = send(i, response.c_str(), response.size(), 0);
          std::cout << "Message to client: \n" << response << std::endl;
          if (bytes_sent < 0) std::cerr << "send failed\n"; 

          // then close that connection and remove from the master fd_set
          std::cout << "Closing connection to client." << std::endl;
          close(i);
          FD_CLR(i, &socket_fds);
          // make sure we decrement the max_fd to the right value
          if (i == max_fd) {
            while (FD_ISSET(max_fd, &socket_fds) == false) {
              max_fd -= 1;
            }
          }
        }
      }
    }
  }
  return 0;
}
