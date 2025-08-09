#include "DateTimeUtils.h"
#include <sstream>
#include <iomanip>

std::string DateTimeUtils::formatDateTime(std::time_t time, const std::string &format)
{
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), format.c_str());
    return oss.str();
}

std::string DateTimeUtils::formatDate(std::time_t time, const std::string &format)
{
    return formatDateTime(time, format);
}

std::string DateTimeUtils::formatTime(std::time_t time, const std::string &format)
{
    return formatDateTime(time, format);
}

std::time_t DateTimeUtils::parseDateTime(const std::string &dateTimeStr, const std::string &format)
{
    // Placeholder implementation
    return std::time(nullptr);
}

std::time_t DateTimeUtils::parseDate(const std::string &dateStr, const std::string &format)
{
    return parseDateTime(dateStr, format);
}

std::time_t DateTimeUtils::parseTime(const std::string &timeStr, const std::string &format)
{
    return parseDateTime(timeStr, format);
}

std::time_t DateTimeUtils::getCurrentTime()
{
    return std::time(nullptr);
}

std::time_t DateTimeUtils::getStartOfDay(std::time_t time)
{
    struct tm *timeInfo = std::localtime(&time);
    timeInfo->tm_hour = 0;
    timeInfo->tm_min = 0;
    timeInfo->tm_sec = 0;
    return std::mktime(timeInfo);
}

std::time_t DateTimeUtils::getEndOfDay(std::time_t time)
{
    struct tm *timeInfo = std::localtime(&time);
    timeInfo->tm_hour = 23;
    timeInfo->tm_min = 59;
    timeInfo->tm_sec = 59;
    return std::mktime(timeInfo);
}

std::time_t DateTimeUtils::addHours(std::time_t time, int hours)
{
    return time + (hours * 3600);
}

std::time_t DateTimeUtils::addDays(std::time_t time, int days)
{
    return time + (days * 24 * 3600);
}

bool DateTimeUtils::isWithinBusinessHours(std::time_t time, int startHour, int endHour)
{
    struct tm *timeInfo = std::localtime(&time);
    int hour = timeInfo->tm_hour;
    return hour >= startHour && hour < endHour;
}

bool DateTimeUtils::isSameDay(std::time_t time1, std::time_t time2)
{
    return getStartOfDay(time1) == getStartOfDay(time2);
}

int DateTimeUtils::getDayOfWeek(std::time_t time)
{
    struct tm *timeInfo = std::localtime(&time);
    return timeInfo->tm_wday;
}

double DateTimeUtils::getDurationHours(std::time_t startTime, std::time_t endTime)
{
    if (endTime <= startTime)
        return 0.0;
    return static_cast<double>(endTime - startTime) / 3600.0;
}

bool DateTimeUtils::isValidDateTime(std::time_t time)
{
    return time > 0;
}

bool DateTimeUtils::isTimeInFuture(std::time_t time)
{
    return time > getCurrentTime();
}

bool DateTimeUtils::isTimeInPast(std::time_t time)
{
    return time < getCurrentTime();
}
