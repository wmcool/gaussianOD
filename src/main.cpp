#include "../include/detectanomalies.h"
#include "../include/io.h"
#include <iostream>
#include <chrono>
#include "../include/mysocket.h"
#include <unistd.h>
#include <ios>
#include <vector>
#include "../include/json.hpp"
#include <sys/fcntl.h>
#include <sys/stat.h>

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
  std::cout << "开始运行滑动窗口高斯离群点检测算法..." << std::endl;
  if (argc <= 2) {
    std::cerr << "For usage, please look README file.";
    return 1;
  } else {

    // 初始化管道
    if((mkfifo("/tmp/ndc",O_CREAT|O_EXCL)<0)&&(errno!=EEXIST))
      printf("cannot create fifo\n");
    if(errno==ENXIO){
      printf("open error; no reading process\n");
      return 0;
    }
    int fifo_fd = 0;
    fifo_fd = open("/tmp/ndc",O_RDONLY, 0);
    if(fifo_fd <= 0) {
      printf("open fifo failed");
      return 0;
    }

    vector<double> window;

    std::string method = argv[1];
    Result anomalies;
//    int sock = init_socket();
    int k = 0;
    int cur = 1;
    if (method == "tsg") {
    } else if (method == "tsr") {
      if (argc <= 3) {
        std::cerr << "You must type window size.";
        return 1;
      }
      int window_size = std::stoi(argv[2]);
      for(int i=0;i<window_size;i++) {
        double data = get_incoming_data(fifo_fd, 1, ",");
        window.push_back(data);
      }
      while(true) {
        double moving_mean = mean(window);
        double moving_stddev = stddev(window);
        double score = window[k] - moving_mean;
        if(k < window_size - 1) k++;
        nlohmann::json j;
        stringstream ss;
        ss << cur;
        if (std::abs(score) > moving_stddev) {
          j["outlier"][ss.str()] = window[k];
          cout << j.dump() << endl;
          string s = j.dump() + "\n";
//          send(sock, s.c_str(), s.size(), 0);
        } else {
          j["inlier"][ss.str()] = window[k];
          cout << j.dump() << endl;
          string s = j.dump() + "\n";
//          send(sock, s.c_str(), s.size(), 0);
        }
      }
    } else if (method == "mad") {
    } else {
      std::cerr << "Please type correct algorithm.";
      return 1;
    }
  }
  return 0;
}