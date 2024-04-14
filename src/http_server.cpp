#include "http_server.h"

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

std::map<std::string, std::string> get_headers (char* recv_buffer) {
  std::map<std::string, std::string> headers;

  int count {1};
  // increase count until the end of the first line \r\n
  while (recv_buffer[count-1]!='\n') count += 1;
  
  std::string key {};
  std::string value {};
  // continue looping until the double return before the body
  while (recv_buffer[count-2]!='\r' || 
         recv_buffer[count-1]!='\n' || 
         recv_buffer[count]!='\r' || 
         recv_buffer[count+1]!='\n') {
    key = "";
    value = "";

    // find the colon and store the chars in the key string
    while (recv_buffer[count]!=':') {
      key += recv_buffer[count];
      count += 1;
    }
    count += 2; // skip past the ": "

    // find the end of the line and store the chars in the value string
    while (recv_buffer[count]!='\r') {
      value += recv_buffer[count];
      count += 1;
    }
    count += 2; // skip past the \r\n
    
    headers[key] = value;
  }

  return headers;
}

std::string process_request (char* recv_buffer) {
  std::string path = get_path(recv_buffer);
    
  if (path == "/") return "HTTP/1.1 200 OK\r\n\r\n";
    else if (path.substr(0,6) == "/echo/") {
      std::string echo {path.substr(6)};
      return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " 
      + std::to_string(echo.length()) + "\r\n\r\n" + echo;
    }
    else if (path == "/user-agent") {
      std::map<std::string, std::string> headers = get_headers(recv_buffer);
      std:: string user_agent {headers["User-Agent"]};
      return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " 
      + std::to_string(user_agent.length()) + "\r\n\r\n" + user_agent;
    }
    else return "HTTP/1.1 404 Not Found\r\n\r\n";
}
