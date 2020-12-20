#include "processor.h"

// Read the current jiffies
void Processor::UpdateJiffies() { current_jiffies_ = LinuxParser::Jiffies(); }

// Return the current CPU utilization
float Processor::Utilization() {
  UpdateJiffies();
  return CalculateUtilization();
}

// Calculate current CPU utilization
float Processor::CalculateUtilization() {
  const float delta_total = current_jiffies_.total - prev_jiffies_.total;
  const float delta_active = current_jiffies_.active - prev_jiffies_.active;
  prev_jiffies_ = current_jiffies_;
  return delta_active / delta_total;
}