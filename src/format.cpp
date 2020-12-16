#include <string>

#include "format.h"

using std::string;

// Helper function to format time
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  constexpr int seconds_in_minute = 60;
  constexpr int seconds_in_hour = 3600;
  const int hours = seconds / seconds_in_hour;
  const string hrs =
      hours < 10 ? "0" + std::to_string(hours) : std::to_string(hours);
  seconds = seconds % seconds_in_hour;
  const int minutes = seconds / seconds_in_minute;
  const string min =
      minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
  seconds = seconds % seconds_in_minute;
  const string sec =
      seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
  return hrs + ":" + min + ":" + sec;
}