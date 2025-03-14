#pragma once

#include <stdint>
#include <chrono>
#include <ctime>
#include <string>

namespace Pinetime {
  namespace Controllers {
    class DateTime {
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

      // Set the time using year, month, day, hour, minute, and second.
      void SetTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

      // Set the time zone. 'timezone' and 'dst' are in quarters of an hour.
      void SetTimeZone(int8_t timezone, int8_t dst);

      uint16_t Year() const { return 1900 + localTime.tm_year; }
      Months Month() const { return static_cast<Months>(localTime.tm_mon + 1); }
      uint8_t Day() const { return localTime.tm_mday; }
      Days DayOfWeek() const { int daysSinceSunday = localTime.tm_wday; return daysSinceSunday == 0 ? Days::Sunday : static_cast<Days>(daysSinceSunday); }
      int DayOfYear() const { return localTime.tm_yday + 1; }
      uint8_t Hours() const { return localTime.tm_hour; }
      uint8_t Minutes() const { return localTime.tm_min; }
      uint8_t Seconds() const { return localTime.tm_sec; }

      // Returns the offset (in quarters of an hour) between local time and UTC.
      int8_t UtcOffset() const { return tzOffset + dstOffset; }
      // Returns the base timezone offset (in quarters of an hour).
      int8_t TzOffset() const { return tzOffset; }
      // Returns the daylight saving offset (in quarters of an hour).
      int8_t DstOffset() const { return dstOffset; }

      const char* MonthShortToString() const;
      const char* DayOfWeekShortToString() const;
      static const char* MonthShortToStringLow(Months month);
      static const char* DayOfWeekShortToStringLow(Days day);

      // Returns the current date/time as stored internally.
      std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> CurrentDateTime();

      // Returns the UTC time computed from the stored local time.
      std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> UTCDateTime() {
        return CurrentDateTime() - std::chrono::seconds((tzOffset + dstOffset) * 15 * 60);
      }

      // Returns the system uptime (in seconds).
      std::chrono::seconds Uptime() const { return uptime; }

      // Sets the current date/time.
      void SetCurrentTime(std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> t);

      // Returns the time formatted as a string (24‑hour format is used in this bare‑metal version).
      std::string FormattedTime();

      // In this bare‑metal version the UpdateTime method is meant to be called with a tick count
      // (for example, from your main loop or timer interrupt) to update the stored time.
      // The systickCounter parameter should be provided by your hardware timer.
    private:
      void UpdateTime(uint32_t systickCounter, bool forceUpdate);

      std::tm localTime{};
      int8_t tzOffset = 0;
      int8_t dstOffset = 0;

      // These variables help simulate a continuously increasing clock.
      uint32_t previousSystickCounter = 0;
      std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> currentDateTime;
      std::chrono::seconds uptime {0};

      // Flags to avoid multiple notifications. In this bare‑metal version the notifications are only commented.
      bool isMidnightAlreadyNotified = false;
      bool isHourAlreadyNotified = true;
      bool isHalfHourAlreadyNotified = true;
    };
  }
}
