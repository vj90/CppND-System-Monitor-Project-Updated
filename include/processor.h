#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  float Utilization();

 protected:
  LinuxParser::SystemJiffies prev_jiffies_;
  LinuxParser::SystemJiffies current_jiffies_;
  float CalculateUtilization();

 private:
  void UpdateJiffies();
};

#endif