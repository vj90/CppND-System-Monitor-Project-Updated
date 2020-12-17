#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

class System {
 public:
  System()
      : kernel_(LinuxParser::Kernel()), os_(LinuxParser::OperatingSystem()) {}
  Processor& Cpu();
  std::vector<Process>& Processes();
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();
  int RunningProcesses();
  const std::string& Kernel() const;
  const std::string& OperatingSystem() const;

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string kernel_{"Not set"};
  std::string os_{"Not set"};
};

#endif