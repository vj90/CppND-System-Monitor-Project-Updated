#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "processor.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process : private Processor {
 public:
  Process(int pid) : pid_(pid) { Update(); }
  void Update();
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_{0};
  std::string user_{"Not set"};
  std::string command_{"Not set"};
  float cpu_{0.0};
  std::string ram_{"Not set"};
  long int uptime_{0};
  void UpdateJiffies();
  float Utilization();
};

#endif