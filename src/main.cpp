#include "detectanomalies.h"
#include "io.h"
#include <iostream>
#include <chrono>
#include "mysocket.h"
#include <unistd.h>
#include <ios>
#include <vector>
#include "json.hpp"

using namespace std;

void process_mem_usage(double& vm_usage, double& resident_set)
{
  using std::ios_base;
  using std::ifstream;
  using std::string;

  vm_usage     = 0.0;
  resident_set = 0.0;

  // 'file' stat seems to give the most reliable results
  //
  ifstream stat_stream("/proc/self/stat",ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;

  // the two fields we want
  //
  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
      >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
      >> utime >> stime >> cutime >> cstime >> priority >> nice
      >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

  stat_stream.close();

  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
  vm_usage     = vsize / 1024.0;
  resident_set = rss * page_size_kb;
}

int main(int argc, char *argv[]) {
  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::duration;
  using std::chrono::milliseconds;
  if (argc <= 2) {
    std::cerr << "For usage, please look README file.";
    return 1;
  } else {
    std::vector<double> data = parseCsv(argv[1]);
    std::string method = argv[2];
    Result anomalies;
//    int sock = init_socket();
    if (method == "tsg") {
      anomalies = detect_anomalies_global(data);
      std::cout << "Detected anomalies with global averages: " << std::endl;
      for (decltype(anomalies.indexes.size()) i = 0;
           i < anomalies.indexes.size(); i++) {
      }
    } else if (method == "tsr") {
      if (argc <= 3) {
        std::cerr << "You must type window size.";
        return 1;
      }
      int window_size = std::stoi(argv[3]);
      for (decltype(data.size()) i = window_size - 1; i < data.size(); i++) {
        auto t1 = high_resolution_clock::now();
        std::vector<double> window = std::vector<double>(
            data.begin() + i - window_size + 1, data.begin() + i + 1);
        double moving_mean = mean(window);
        double moving_stddev = stddev(window);
        double score = data[i] - moving_mean;
        auto t2 = high_resolution_clock::now();
        if (std::abs(score) > 3 * moving_stddev) {
          nlohmann::json j;
          stringstream ss;
          ss << i;
          j["outlier"][ss.str()] = data[i];
          cout << j.dump() << endl;
          duration<double, std::milli> ms_double = t2 - t1;
          std::cout << ms_double.count() << "ms cost\n";
          double vm, rss;
          process_mem_usage(vm, rss);
          cout << "VM: " << vm << "KB" << endl;
        }
      }
    } else if (method == "mad") {
      anomalies = detect_anomalies_mad(data);
      std::cout << "Detected anomalies with median absolute deviation: "
                << std::endl;
      for (decltype(anomalies.indexes.size()) i = 0;
           i < anomalies.indexes.size(); i++) {
      }
    } else {
      std::cerr << "Please type correct algorithm.";
      return 1;
    }
  }
  return 0;
}