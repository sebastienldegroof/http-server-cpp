#include "http_server.h"

std::string get_path (char* rx_buffer) {
  std::string path {};
  int count { 0 };
  char buf_char;

  // increment past the method
  while (rx_buffer[count] != ' ' ) ++count;
  ++count;

  // increment until the whitespace after the path
  while ( rx_buffer[count] != ' ' ) {
    path += rx_buffer[count];
    ++count;
  }

  return path;
}

std::vector<std::string> get_vector_path (char* rx_buffer) {
  std::vector<std::string> vector_path;
  
  // Iterate through the buffer. We can use the whitespaces
  // to find the end of the method string
  int count { 0 };
  while (rx_buffer[count] != ' ' ) ++count;
  ++count;

  // iterate through the chars in the path. When another / is found
  // push the directory to the vector and start building the next dir
  std::string dir {};
  while (rx_buffer[count] != ' ' )  {
    if ( rx_buffer[count] == '/') {
      if (dir != "") vector_path.push_back(dir);
      dir = "";
    } else {
      dir += rx_buffer[count];
    }
    ++count;
  }
  vector_path.push_back(dir);

  return vector_path;
}

std::string get_method (char* rx_buffer) {
  std::string method; 

  int count {0};
  while ( rx_buffer[count] != ' ' ) {
    method += rx_buffer[count];
    ++count;
  }

  return method;
}

std::map<std::string, std::string> get_headers (char* rx_buffer) {
  std::map<std::string, std::string> headers;

  int count {1};
  // increase count until the end of the first line \r\n
  while (rx_buffer[count-1]!='\n') count += 1;
  
  std::string key {};
  std::string value {};
  // continue looping until the double return before the body
  while (rx_buffer[count-2]!='\r' || 
         rx_buffer[count-1]!='\n' || 
         rx_buffer[count]!='\r' || 
         rx_buffer[count+1]!='\n') {
    key = "";
    value = "";

    // find the colon and store the chars in the key string
    while (rx_buffer[count]!=':') {
      key += rx_buffer[count];
      count += 1;
    }
    count += 2; // skip past the ": "

    // find the end of the line and store the chars in the value string
    while (rx_buffer[count]!='\r') {
      value += rx_buffer[count];
      count += 1;
    }
    count += 2; // skip past the \r\n
    
    headers[key] = value;
  }

  return headers;
}

char* get_body (char* rx_buffer) {
  // increase count until we get past the headers
  int count {4};
  while ( rx_buffer[count-4] != '\r' ||
          rx_buffer[count-3] != '\n' ||
          rx_buffer[count-2] != '\r' || 
          rx_buffer[count-1] != '\n' ) count++;
  
  return &rx_buffer[count];
}

size_t get_file ( std::string filename, char* tx_buffer ) {
  
  std::string response { "HTTP/1.1 404 Not Found\r\n\r\n" };

  // Step 1: find out if file exists
  FILE* pfile = fopen( filename.c_str(), "rb");
  if ( pfile == NULL) {
    std::strcpy( tx_buffer, response.c_str());
    return response.length();
  }
  // Step 2: find size of file and determine if it will fit in the tx_buffer
  fseek(pfile, 0, SEEK_END);
  long file_size = ftell(pfile);
  fseek(pfile, 0, SEEK_SET);
  
  if (file_size > MAX_BUFFER_SIZE) {

    response = "Appropriate HTTP response for file too big";
    std::strcpy( tx_buffer, response.c_str());
    return response.length();
  }
  // Step 3: read the file into the tx_buffer where the body goes
  response = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: " 
           + std::to_string(file_size) + "\r\n\r\n";
  int headers_len = response.length();
  std::strcpy( tx_buffer, response.c_str()); // copy the HTTP headers into the tx buffer

  char* tx_buffer_body = &tx_buffer[headers_len]; // create pointer after headers end
  fread(tx_buffer_body, file_size, 1, pfile); // read file into new pointer
  fclose(pfile);

  return headers_len + file_size;

}

int write_file (std::string filename, char* contents) {

  std::ofstream file;
  file.open( filename );

  if ( file.is_open() ) {
    file << contents ;
    file.close();
    return 0;
  } else
    return -1;
  
}

size_t process_request (char* rx_buffer, char* tx_buffer, std::string dir_path) {

  std::string method = get_method(rx_buffer);
  std::vector<std::string> path_vector = get_vector_path(rx_buffer);
  std::string path_str = get_path(rx_buffer);
  std::map<std::string, std::string> headers = get_headers(rx_buffer);

  std::string response { };

  if ( method == "GET" ) {

    if ( path_str == "/" ) {

      response = "HTTP/1.1 200 OK\r\n\r\n";

    } else if ( path_vector[0] == "echo" ) {

      std::string echo { path_str.length() > 5 ? path_str.substr(6) : "/" };
      response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " 
      + std::to_string( echo.length() ) + "\r\n\r\n" + echo;

    }
    else if ( path_vector[0] == "user-agent" ) {

      std::string user_agent {headers["User-Agent"]};
      response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " 
      + std::to_string( user_agent.length() ) + "\r\n\r\n" + user_agent;

    } 
    else if ( path_vector[0] == "files" ) {

      return get_file( dir_path + path_str.substr(7), tx_buffer );

    } else {

      response = "HTTP/1.1 404 Not Found\r\n\r\n";

    }
  } else if ( method == "POST" ) {

    if ( path_vector[0] == "files") {

      char* file_contents = get_body(rx_buffer);

      if (write_file( dir_path + path_str.substr(7), file_contents) == 0 ) {
        response = "HTTP/1.1 201 Created\r\n\r\n";
      } else {
        response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
      }

    } else {
      response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }
  }
  else {

    response = "HTTP/1.1 404 Not Found\r\n\r\n";

  }

  std::strcpy( tx_buffer, response.c_str());
  return response.length();
}
