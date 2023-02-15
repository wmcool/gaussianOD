#include "../include/io.h"
#include "vector"
#include <sys/unistd.h>
std::string pipe_line;

std::vector<double> parseCsv(const std::string &filename) {
  std::vector<double> data;
  std::ifstream file(filename);
  std::string line;
  if (file.is_open()) {
    while (std::getline(file, line)) {
      data.push_back(std::stod(line));
    }
  }
  return data;
}

double get_incoming_data(int pipe_fd, int length, std::string delimiter) {
  double res;
  //    string line;
  int count = 0;
  while (true) {
    size_t pos = 0;
    std::string token;
    char buffer[71];
    int valread = read(pipe_fd, buffer, 71);
    if (valread > 0) {
      for (int i = 0; i < valread; i++) {
        if (buffer[i] == '\n') {
          bool flag = true;
          bool flag2 = true;
          long timestamp = 0;
          while ((pos = pipe_line.find(delimiter)) != std::string::npos) {
            token = pipe_line.substr(0, pos);
            if (!flag) {
              if (flag2) {
                res = atof(token.c_str());
                flag2 = false;
              }
            } else {
              timestamp = atol(token.c_str());
              flag = false;
            }
            pipe_line.erase(0, pos + delimiter.length());
          }
          count++;
          pipe_line = "";
        } else {
          pipe_line += buffer[i];
        }
      }
    }
    if(count == length) break;
  }
  return res;
}