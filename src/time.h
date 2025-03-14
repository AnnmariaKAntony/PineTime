#pragma once

#include <stdint>
#include <chrono>
#include <ctime>
#include <string>
#include <cstring> // for strncpy

namespace Pinetime {
  namespace Controllers {

    class DateTime {
    private:
      std::tm localTime{};  // Current local time data.
      int8_t tzOffset = 0;
      int8_t dstOffset = 0;
      uint32_t previousSystickCounter = 0;
      std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> currentDateTime;
      std::chrono::seconds uptime {0};
      bool isMidnightAlreadyNotified = false;
      bool isHourAlreadyNotified = true;
      bool isHalfHourAlreadyNotified = true;

    public:
      DateTime();

      enum class Days : uint8_t { Unknown, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday };
      enum class Months : uint8_t {
        Unknown,
        January,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
      };

      // Inline getters that use private members.
      uint16_t Year() const { return 1900 + localTime.tm_year; }
      Months Month() const { return static_cast<Months>(localTime.tm_mon + 1); }
      uint8_t Day() const { return localTime.tm_mday; }
      Days DayOfWeek() const { 
          int daysSinceSunday = localTime.tm_wday; 
          return daysSinceSunday == 0 ? Days::Sunday : static_cast<Days>(daysSinceSunday); 
      }
      int DayOfYear() const { return localTime.tm_yday + 1; }
      uint8_t Hours() const { return localTime.tm_hour; }
      uint8_t Minutes() const { return localTime.tm_min; }
      uint8_t Seconds() const { return localTime.tm_sec; }

      int8_t UtcOffset() const { return tzOffset + dstOffset; }
      int8_t TzOffset() const { return tzOffset; }
      int8_t DstOffset() const { return dstOffset; }
      std::chrono::seconds Uptime() const { return uptime; }

      const char* MonthShortToString() const;
      const char* DayOfWeekShortToString() const;
      static const char* MonthShortToStringLow(Months month);
      static const char* DayOfWeekShortToStringLow(Days day);

      std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> CurrentDateTime();
      std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> UTCDateTime() {
        return CurrentDateTime() - std::chrono::seconds((tzOffset + dstOffset) * 15 * 60);
      }

      void SetCurrentTime(std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> t);
      void SetTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
      void SetTimeZone(int8_t timezone, int8_t dst);
      std::string FormattedTime();

    private:
      // This method is used internally to update the stored time.
      void UpdateTime(uint32_t systickCounter, bool forceUpdate);
    };

    // Free functions for time management.
    void time_init();
    void rtc_init();
    void increment_time();
    void get_time_string(char *buffer, size_t buffer_size);
  }
}

// Provide C linkage (or aliasing) for the free functions.
using Pinetime::Controllers::time_init;
using Pinetime::Controllers::rtc_init;
using Pinetime::Controllers::increment_time;
using Pinetime::Controllers::get_time_string;
