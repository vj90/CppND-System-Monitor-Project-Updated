#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  const auto current_jiffies = LinuxParser::Jiffies();
  const float delta_total = (current_jiffies.active + current_jiffies.inactive) -
                           (prev_jiffies_.active + prev_jiffies_.inactive);
  const float delta_active = current_jiffies.active - prev_jiffies_.active;
  prev_jiffies_ = current_jiffies;
  return delta_active / delta_total;
}