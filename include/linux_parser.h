#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

std::vector<std::string> CpuUtilization();

struct SystemJiffies {
  long active{0};
  long total{0};
};
SystemJiffies Jiffies();
long ActiveJiffies();
SystemJiffies Jiffies(const int pid, long& uptime);
long IdleJiffies();

// Processes
std::string Command(const int pid);
std::string Ram(const int pid);
std::string Uid(const int pid);
std::string User(const int pid);
long int UpTime(const int pid);

enum ProcStatus {
  kUTime_ = 13,
  kSTime_ = 14,
  kCUtime_ = 15,
  kCSTime_ = 16,
  kStartTime_ = 21
};

// Parsing helper
template <typename T>
void ReadFieldValueFromFile(const std::string& file_path,
                            const std::string& field, T& value);
};  // namespace LinuxParser

#endif