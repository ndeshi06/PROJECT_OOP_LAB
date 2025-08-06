#pragma once
#include <string>
#include <ctime>

class DateTimeUtils {
public:
    // String formatting
    static std::string formatDateTime(std::time_t time, const std::string& format = "%Y-%m-%d %H:%M:%S");
    static std::string formatDate(std::time_t time, const std::string& format = "%Y-%m-%d");
    static std::string formatTime(std::time_t time, const std::string& format = "%H:%M:%S");

    // Parsing
    static std::time_t parseDateTime(const std::string& dateTimeStr, const std::string& format = "%Y-%m-%d %H:%M:%S");
    static std::time_t parseDate(const std::string& dateStr, const std::string& format = "%Y-%m-%d");
    static std::time_t parseTime(const std::string& timeStr, const std::string& format = "%H:%M:%S");

    // Utility functions
    static std::time_t getCurrentTime();
    static std::time_t getStartOfDay(std::time_t time);
    static std::time_t getEndOfDay(std::time_t time);
    static std::time_t addHours(std::time_t time, int hours);
    static std::time_t addDays(std::time_t time, int days);
    
    // Business logic helpers
    static bool isWithinBusinessHours(std::time_t time, int startHour = 6, int endHour = 23);
    static bool isSameDay(std::time_t time1, std::time_t time2);
    static int getDayOfWeek(std::time_t time); // 0 = Sunday, 1 = Monday, etc.
    static double getDurationHours(std::time_t startTime, std::time_t endTime);

    // Validation
    static bool isValidDateTime(std::time_t time);
    static bool isTimeInFuture(std::time_t time);
    static bool isTimeInPast(std::time_t time);

private:
    DateTimeUtils() = default; // Utility class, no instances
};
