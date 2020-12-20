#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// TODO write a common function for reading data from files?

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float total_memory{0.0}, free_memory{0.0};
  std::ifstream memory_file(kProcDirectory + kMeminfoFilename);
  if (memory_file.is_open()) {
    string line;
    bool data_read{false};
    while (std::getline(memory_file, line) && !data_read) {
      // std::replace(line.begin(),line.end(),":"," ");
      string key;
      float value;
      bool total_read{false}, free_read{false};
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total_memory = value;
          total_read = true;
        }
        if (key == "MemFree:") {
          free_memory = value;
          free_read = true;
        }
      }
      data_read = total_read && free_read;
    }
  }
  float memory_utilization{0.0};
  if (total_memory > 0.0) {
    memory_utilization = (total_memory - free_memory) / total_memory;
  }
  return memory_utilization;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime{0};
  std::ifstream uptime_file(kProcDirectory + kUptimeFilename);
  if (uptime_file.is_open()) {
    string line;
    if (std::getline(uptime_file, line)) {
      std::istringstream linestream(line);
      linestream >> uptime;
    }
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
LinuxParser::SystemJiffies LinuxParser::Jiffies() {
  SystemJiffies jiffies;
  std::ifstream stat_file(kProcDirectory + kStatFilename);
  string field;
  if (stat_file.is_open()) {
    string line;
    while (std::getline(stat_file, line)) {
      std::istringstream linestream(line);
      if (linestream >> field && field == "cpu") {
        for (int i = 0; i <= CPUStates::kSteal_; ++i) {
          long current_jiffies;
          if (linestream >> current_jiffies) {
            jiffies.total += current_jiffies;
            switch (i) {
              case CPUStates::kUser_:
              case CPUStates::kNice_:
              case CPUStates::kIRQ_:
              case CPUStates::kSoftIRQ_:
              case CPUStates::kSteal_: {
                jiffies.active += current_jiffies;
              }
            }
          }
        }
        break;
      }
    }
  }
  return jiffies;
}

// Read and return the number of jiffies for a PID
LinuxParser::SystemJiffies LinuxParser::Jiffies(const int pid, long& uptime) {
  LinuxParser::SystemJiffies current_jiffies;
  long start_time{0};
  string PID = to_string(pid);
  std::ifstream stat_file(kProcDirectory + PID + kStatFilename);
  if (stat_file.is_open()) {
    string line;
    if (std::getline(stat_file, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i <= ProcStatus::kStartTime_; ++i) {
        string value;
        if (linestream >> value) {
          switch (i) {
            case (ProcStatus::kUTime_):
            case (ProcStatus::kSTime_):
            case (ProcStatus::kCUtime_):
            case (ProcStatus::kCSTime_): {
              current_jiffies.active += std::stol(value);
              break;
            }
            case (ProcStatus::kStartTime_): {
              start_time = std::stol(value);
              break;
            }
          }
        }
      }
    }
  }
  const long system_uptime = LinuxParser::UpTime();
  const long system_uptime_ticks = system_uptime * sysconf(_SC_CLK_TCK);
  uptime = system_uptime - (start_time / sysconf(_SC_CLK_TCK));
  current_jiffies.total = system_uptime_ticks - start_time;
  return current_jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int total_processes(0);
  LinuxParser::ReadFieldValueFromFile(kProcDirectory + kStatFilename,
                                      "processes", total_processes);
  return total_processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int running_processes(0);
  LinuxParser::ReadFieldValueFromFile(kProcDirectory + kStatFilename,
                                      "procs_running", running_processes);
  return running_processes;
}

// Read and return the command of running processes
string LinuxParser::Command(const int pid) {
  string command;
  string PID = to_string(pid);
  std::ifstream cmd_file(kProcDirectory + PID + kCmdlineFilename);
  if (cmd_file.is_open()) {
    std::getline(cmd_file, command);
  }
  return command;
}

// Read and return the ram of a PID
string LinuxParser::Ram(const int pid) {
  long ram{0};
  string PID = to_string(pid);
  LinuxParser::ReadFieldValueFromFile(kProcDirectory + PID + kStatusFilename,
                                      "VmSize:", ram);
  ram = ram / 1000;  // convert to MB
  return to_string(ram);
}

// Read and return the UID of a process
string LinuxParser::Uid(const int pid) {
  string PID = to_string(pid);
  string uid;
  LinuxParser::ReadFieldValueFromFile(kProcDirectory + PID + kStatusFilename,
                                      "uid:", uid);
  return uid;
}

// Read and return the user of a process
string LinuxParser::User(const int pid) {
  string user, x;
  const auto uid = Uid(pid);
  std::ifstream pswd_file(kPasswordPath);
  if (pswd_file.is_open()) {
    string line;
    while (std::getline(pswd_file, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string current_uid;
      if (linestream >> user >> x >> current_uid) {
        if (current_uid == uid) {
          break;
        }
      }
    }
  }
  return user.substr(0,6);
}

// Read and return the uptime of a process
long LinuxParser::UpTime(const int pid[[maybe_unused]]) { return 0; }

// Helper function for parsing a value from a file
template <typename T>
void LinuxParser::ReadFieldValueFromFile(const std::string& file_path,
                                         const std::string& field, T& value) {
  // T value;
  std::ifstream file(file_path);
  if (file.is_open()) {
    string line;
    while (std::getline(file, line)) {
      std::istringstream linestream(line);
      string current_field;
      if (linestream >> current_field >> value) {
        if (current_field == field) {
          return;
        }
      }
    }
  }
}