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
