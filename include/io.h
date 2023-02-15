#ifndef ANOMALYDETECTIONCPP_IO_H
#define ANOMALYDETECTIONCPP_IO_H

#include <fstream>
#include <string>
#include <vector>

std::vector<double> parseCsv(const std::string &filename);

double get_incoming_data(int pipe_fd, int length, std::string delimiter);

#endif
