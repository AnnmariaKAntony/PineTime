#include "time.h"
#include <cstdio>
#include <ctime>
#include <chrono>
#include <cstring>

namespace {

// Arrays for short day and month strings.
constexpr const char* const DaysStringShort[]    = {"--", "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
constexpr const char* const DaysStringShortLow[] = {"--", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
constexpr const char* const MonthsString[]         = {"--", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
constexpr const char* const MonthsStringLow[]      = {"--", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// Recursive constexpr function to convert a digit string to an integer.
// C++11 constexpr functions must have a single return statement.
constexpr int compileTimeAtoi(const char* str) {
  return (*str >= '0' && *str <= '9') ? ((*str - '0') + 10 * compileTimeAtoi(str + 1)) : 0;
}

// Tick rate and maximum tick value definitions.
constexpr uint32_t TICK_RATE_HZ = 1000;     // e.g., 1000 ticks per second
constexpr uint32_t RTC_MAX_TICKS = 0xFFFFFF;  // 24-bit counter maximum

} // end anonymous namespace

namespace Pinetime {
namespace Controllers {

DateTime::DateTime() {
  // __DATE__ is of the format "MMM DD YYYY".
  // Use the year part (starting at index 7) to initialize the clock.
  SetTime(compileTimeAtoi(&__DATE__[7]), 1, 1, 0, 0, 0);
}

void DateTime::SetCurrentTime(std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> t) {
  currentDateTime = t;
  UpdateTime(previousSystickCounter, true);
}

void DateTime::SetTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
  std::tm tm = {};
  tm.tm_sec  = second;
  tm.tm_min  = minute;
  tm.tm_hour = hour;
  tm.tm_mday = day;
  tm.tm_mon  = month - 1;
  tm.tm_year = year - 1900;
  tm.tm_isdst = -1;  // Let the C library determine DST
  currentDateTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  UpdateTime(previousSystickCounter, true);
}

void DateTime::SetTimeZone(int8_t timezone, int8_t dst) {
  tzOffset = timezone;
  dstOffset = dst;
}

std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> DateTime::CurrentDateTime() {
  return currentDateTime;
}

void DateTime::UpdateTime(uint32_t systickCounter, bool forceUpdate) {
  // Calculate the difference between the new tick count and the previous count.
  uint32_t systickDelta = (systickCounter < previousSystickCounter)
      ? (RTC_MAX_TICKS - previousSystickCounter) + systickCounter + 1
      : systickCounter - previousSystickCounter;
  // Determine how many whole seconds have passed.
  auto correctedDelta = systickDelta / TICK_RATE_HZ;
  if (correctedDelta == 0 && !forceUpdate) {
    return;
  }
  auto rest = systickDelta % TICK_RATE_HZ;
  if (systickCounter >= rest) {
    previousSystickCounter = systickCounter - rest;
  } else {
    previousSystickCounter = RTC_MAX_TICKS - (rest - systickCounter - 1);
  }
  currentDateTime += std::chrono::seconds(correctedDelta);
  uptime += std::chrono::seconds(correctedDelta);
  std::time_t currentTime = std::chrono::system_clock::to_time_t(currentDateTime);
  localTime = *std::localtime(&currentTime);
  int minute = Minutes();
  int hour   = Hours();
  if (minute == 0 && !isHourAlreadyNotified) {
    isHourAlreadyNotified = true;
    // New hour notification placeholder.
  } else if (minute != 0) {
    isHourAlreadyNotified = false;
  }
  if ((minute == 0 || minute == 30) && !isHalfHourAlreadyNotified) {
    isHalfHourAlreadyNotified = true;
    // New half-hour notification placeholder.
  } else if (minute != 0 && minute != 30) {
    isHalfHourAlreadyNotified = false;
  }
  if (hour == 0 && !isMidnightAlreadyNotified) {
    isMidnightAlreadyNotified = true;
    // New day (midnight) notification placeholder.
  } else if (hour != 0) {
    isMidnightAlreadyNotified = false;
  }
}

const char* DateTime::MonthShortToString() const {
  return MonthsString[static_cast<uint8_t>(Month())];
}

const char* DateTime::DayOfWeekShortToString() const {
  return DaysStringShort[static_cast<uint8_t>(DayOfWeek())];
}

const char* DateTime::MonthShortToStringLow(Months month) {
  return MonthsStringLow[static_cast<uint8_t>(month)];
}

const char* DateTime::DayOfWeekShortToStringLow(Days day) {
  return DaysStringShortLow[static_cast<uint8_t>(day)];
}

std::string DateTime::FormattedTime() {
  auto hour = Hours();
  auto minute = Minutes();
  char buff[9];
  std::snprintf(buff, sizeof(buff), "%02d:%02d", hour, minute);
  return std::string(buff);
}

// Global instance used for timekeeping.
static DateTime g_dateTime;

// Free function implementations.
void time_init() {
  g_dateTime = DateTime();
}

void rtc_init() {
  // Initialize your hardware RTC timer here.
  // This is hardware-dependent.
}

void increment_time() {
  auto current = g_dateTime.CurrentDateTime();
  g_dateTime.SetCurrentTime(current + std::chrono::seconds(1));
}

void get_time_string(char *buffer, size_t buffer_size) {
  std::string formatted = g_dateTime.FormattedTime();
  std::strncpy(buffer, formatted.c_str(), buffer_size);
  buffer[buffer_size - 1] = '\0';
}

} // namespace Controllers
} // namespace Pinetime
