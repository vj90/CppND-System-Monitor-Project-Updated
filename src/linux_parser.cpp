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
        for (int i = 0; i < CPUStates::kCount_; ++i) {
          long current_jiffies;
          if (linestream >> current_jiffies) {
            switch (i) {
              case CPUStates::kUser_:
              case CPUStates::kNice_:
              case CPUStates::kIRQ_:
              case CPUStates::kSoftIRQ_:
              case CPUStates::kSteal_:
                jiffies.active += current_jiffies;
                break;
              default:
                jiffies.inactive += current_jiffies;
            }
          }
        }
        break;
      }
    }
  }
  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID

long LinuxParser::ActiveJiffies(int pid) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stat_file(kProcDirectory + kStatFilename);
  string field;
  int total_processes;
  if (stat_file.is_open()) {
    string line;
    while (std::getline(stat_file, line)) {
      std::istringstream linestream(line);
      if (linestream >> field >> total_processes) {
        if (field == "processes") {
          break;
        }
      }
    }
  }
  return total_processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stat_file(kProcDirectory + kStatFilename);
  string field;
  int running_processes;
  if (stat_file.is_open()) {
    string line;
    while (std::getline(stat_file, line)) {
      std::istringstream linestream(line);
      if (linestream >> field >> running_processes) {
        if (field == "procs_running") {
          break;
        }
      }
    }
  }
  return running_processes;
}

string LinuxParser::Command(int pid) {
  string command;
  string PID = to_string(pid);
  std::ifstream cmd_file(kProcDirectory + PID + kCmdlineFilename);
  if (cmd_file.is_open()) {
    std::getline(cmd_file, command);
  }
  return command;
}

string LinuxParser::Ram(int pid) {
  string field;
  float ram{0.0};
  string PID = to_string(pid);
  std::ifstream status_file(kProcDirectory + PID + kStatusFilename);
  if (status_file.is_open()) {
    string line;
    while (std::getline(status_file, line)) {
      std::istringstream linestream(line);
      if (linestream >> field >> ram) {
        if (field == "VmSize:") {
          ram = ram / 1000;  // convert to MB
          break;
        }
      }
    }
  }
  std::ostringstream os;
  os << ram;
  return os.str();
}

string LinuxParser::Uid(int pid) {
  string uid, field;
  string PID = to_string(pid);
  std::ifstream status_file(kProcDirectory + PID + kStatusFilename);
  if (status_file.is_open()) {
    string line;
    while (std::getline(status_file, line)) {
      std::istringstream linestream(line);
      if (linestream >> field >> uid) {
        if (field == "uid:") {
          break;
        }
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid) {
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
  return user;
}

// TODO: Read and return the uptime of a process

long LinuxParser::UpTime(int pid) { return 0; }