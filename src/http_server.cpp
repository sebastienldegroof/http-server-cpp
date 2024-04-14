#include "http_server.h"

#include <string>

std::string get_path (char* recv_buffer) {
  int pre_space {3};
  while (recv_buffer[pre_space-3]!='G' || recv_buffer[pre_space-2]!='E' || recv_buffer[pre_space-1]!='T') {
    ++pre_space;
  }

  int post_space {pre_space+2};
  while (recv_buffer[post_space+1]!='H' || recv_buffer[post_space+2]!='T' || recv_buffer[post_space+3]!='T' || recv_buffer[post_space+4]!='P'){
    ++post_space;
  } 
         
  std::string req_path {};
  for (int i = pre_space+1; i<post_space; ++i) {
    req_path += recv_buffer[i];
  }

  return req_path;
}

std::string process_request (char* recv_buffer) {
    std::string path = get_path(recv_buffer);
    
    if (path == "/") return "HTTP/1.1 200 OK\r\n\r\n";
    else if (path.substr(0,6) == "/echo/") {
        std::string echo {path.substr(6)};
        std::cout << "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " << 
        std::to_string(echo.length()) << "\r\n" <<
        echo << "\r\n\r\n";
        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(echo.length()) + "/r/n" + echo;
    }
    else return "HTTP/1.1 404 Not Found\r\n\r\n";
}
