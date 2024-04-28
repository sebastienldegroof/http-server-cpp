#include <string>
#include <cstring>
#include <string.h>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>

const size_t MAX_BUFFER_SIZE = 1024;

std::string get_path (char* recv_buffer);

std::vector<std::string> get_vector_path (char* recv_buffer);

std::string get_method (char* recv_buffer);

std::map<std::string, std::string> get_headers (char* recv_buffer);

char* get_body (char* recv_buffer);

size_t get_file (std::string dir, std::string filename);

int write_file(std::string path, std::string filename, char* contents);

size_t process_request (char* rx_buffer, char* tx_buffer, std::string directory);
