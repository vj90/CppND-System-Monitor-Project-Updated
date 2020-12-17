#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
/// TODO some complex calculation
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  auto pid_list = LinuxParser::Pids();
  processes_.reserve(pid_list.size());

  for (std::size_t i = 0; i < pid_list.size(); ++i) {
    processes_.push_back(Process(pid_list[i]));
    processes_.back().Update();
  }

  std::sort(processes_.begin(), processes_.end());

  return processes_;
}

// Return the system's kernel identifier (string)
const std::string& System::Kernel() const { return kernel_; }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
const std::string& System::OperatingSystem() const { return os_; }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }