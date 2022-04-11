#include "io.h"

std::vector<double> parseCsv(const std::string &filename) {
  std::vector<double> data;
  std::ifstream file(filename);
  std::string line;
  if(file.is_open()) {
    while (std::getline(file, line)) {
      data.push_back(std::stod(line));
    }
  }
  return data;
}