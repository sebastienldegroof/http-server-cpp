#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>

std::string get_path (char* recv_buffer);

std::vector<std::string> get_vector_path (char* recv_buffer);

std::string get_method (char* recv_buffer);

std::map<std::string, std::string> get_headers (char* recv_buffer);

std::string get_file (std::string dir, std::string filename);

std::string process_request (char* recv_buffer, std::string directory);
