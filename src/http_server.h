#include <string>
#include <iostream>
#include <map>

std::string get_path (char* recv_buffer);

std::map<std::string, std::string> get_headers (char* recv_buffer);

std::string process_request (char* recv_buffer);
