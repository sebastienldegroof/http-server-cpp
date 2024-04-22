#include "http_server.h"

std::string get_path (char* recv_buffer) {
  std::string path {};
  int count { 0 };
  char buf_char;

  // increment past the method
  while (recv_buffer[count] != ' ' ) ++count;
  ++count;

  // increment until the whitespace after the path
  while ( recv_buffer[count] != ' ' ) {
    path += recv_buffer[count];
    ++count;
  }

  return path;
}

std::vector<std::string> get_vector_path (char* recv_buffer) {
  std::vector<std::string> vector_path;
  
  // Iterate through the buffer. We can use the whitespaces
  // to find the end of the method string
  int count { 0 };
  while (recv_buffer[count] != ' ' ) ++count;
  ++count;

  // iterate through the chars in the path. When another / is found
  // push the directory to the vector and start building the next dir
  std::string dir {};
  while (recv_buffer[count] != ' ' )  {
    if ( recv_buffer[count] == '/') {
      if (dir != "") vector_path.push_back(dir);
      dir = "";
    } else {
      dir += recv_buffer[count];
    }
    ++count;
  }
  vector_path.push_back(dir);

  return vector_path;
}

std::string get_method (char* recv_buffer) {
  std::string method; 

  int count {0};
  while ( recv_buffer[count] != ' ' ) {
    method += recv_buffer[count];
    ++count;
  }

  return method;
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

char* get_body (char* recv_buffer) {
  // increase count until we get past the headers
  int count {4};
  while ( recv_buffer[count-4] != '\r' ||
          recv_buffer[count-3] != '\n' ||
          recv_buffer[count-2] != '\r' || 
          recv_buffer[count-1] != '\n' ) count++;
  
  return &recv_buffer[count];
}

std::string get_file (std::string dir, std::string filename) {
  std::string file_contents {};
  std::ifstream get_file(dir + filename);

  if( get_file.fail() ) return "FILE_NOT_FOUND";
  else {

    std::string line;
    while (std::getline(get_file, line)) {
      file_contents += line;
    }
  }

  return file_contents;
}

int write_file (std::string dir, std::string filename, char* contents) {
  std::ofstream file;
  file.open(dir + filename);
  if ( file.is_open() ) {
    file << contents ;
    file.close();
    return 0;
  } else
    return -1;
  
}

std::string process_request (char* recv_buffer, std::string dir_path) {

  std::string method = get_method(recv_buffer);
  std::vector<std::string> path_vector = get_vector_path(recv_buffer);
  std::string path_str = get_path(recv_buffer);
  std::map<std::string, std::string> headers = get_headers(recv_buffer);

  if ( method == "GET" ) {
    if ( path_str == "/" )
      return "HTTP/1.1 200 OK\r\n\r\n";
    else if ( path_vector[0] == "echo" ) {
      std::string echo {path_str.substr(6)};
      return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " 
      + std::to_string(echo.length()) + "\r\n\r\n" + echo;
    }
    else if ( path_vector[0] == "user-agent" ) {
      std::string user_agent {headers["User-Agent"]};
      return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " 
      + std::to_string(user_agent.length()) + "\r\n\r\n" + user_agent;
    } 
    else if ( path_vector[0] == "files" ) {
      std::string file_contents = get_file( dir_path, path_str.substr(7) );
      if ( file_contents == "FILE_NOT_FOUND" ) {
        return "HTTP/1.1 404 Not Found\r\n\r\n";
      } else {
        return "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: " 
        + std::to_string(file_contents.length()) + "\r\n\r\n" + file_contents;
      }
    } else {
      return "HTTP/1.1 404 Not Found\r\n\r\n";
    }
  } else if ( method == "POST" ) {
    if ( path_vector[0] == "files") {
      char* file_contents = get_body(recv_buffer);
      if (write_file(dir_path, path_str.substr(7), file_contents) == 0) {
        return "HTTP/1.1 201 Created\r\n\r\n";
      } else {
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
      }
    } else {
      return "HTTP/1.1 404 Not Found\r\n\r\n";
    }
  }
  else {
    return "HTTP/1.1 404 Not Found\r\n\r\n";
  }
}
