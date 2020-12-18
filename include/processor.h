#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  float Utilization();

 private:
  LinuxParser::SystemJiffies prev_jiffies_;
};

#endif