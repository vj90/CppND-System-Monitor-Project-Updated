#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

void Process::Update() {
  command_ = LinuxParser::Command(pid_);
  user_ = LinuxParser::User(pid_);
  ram_ = LinuxParser::Ram(pid_);
  cpu_ = Utilization();
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Calculate this process's CPU utilization
float Process::Utilization() {
  UpdateJiffies();
  return CalculateUtilization();
}

float Process::CpuUtilization() { return cpu_; }

// Read the current jiffies
void Process::UpdateJiffies() {
  current_jiffies_ = LinuxParser::Jiffies(pid_, uptime_);
}

// Return the command that generated this process
string Process::Command() { return command_; }

// Return this process's memory utilization
string Process::Ram() { return ram_; }

// Return the user (name) that generated this process
string Process::User() { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return this->cpu_ > a.cpu_; }