#include "BookingController.h"
#include <algorithm>
#include <iterator>
#include <ctime>

BookingController::BookingController() : m_bookingManager(BookingManager::getInstance()) {}

BookingController::~BookingController() {}

bool BookingController::createBooking(int userId, int courtId, std::time_t bookingDate,
                                      std::time_t startTime, std::time_t endTime,
                                      const std::string &notes)
{

    double cost = calculateBookingCost(courtId, startTime, endTime);
    if (cost < 0)
        return false;

    Booking booking(userId, courtId, bookingDate, startTime, endTime, cost);
    booking.setNotes(notes);
    booking.setStatus(BookingStatus::CONFIRMED); // Set as confirmed when successfully created

    return m_bookingManager.createBooking(booking);
}

bool BookingController::cancelBooking(int bookingId)
{
    return m_bookingManager.cancelBooking(bookingId);
}

bool BookingController::modifyBooking(int bookingId, std::time_t newStartTime,
                                      std::time_t newEndTime, const std::string &notes)
{
    auto booking = m_bookingManager.getBooking(bookingId);
    if (!booking)
        return false;

    if (!validateBookingTime(newStartTime, newEndTime))
    {
        return false;
    }

    Booking modifiedBooking = *booking;
    modifiedBooking.setStartTime(newStartTime);
    modifiedBooking.setEndTime(newEndTime);
    modifiedBooking.setNotes(notes);

    double newCost = calculateBookingCost(booking->getCourtId(), newStartTime, newEndTime);
    if (newCost >= 0)
    {
        modifiedBooking.setTotalAmount(newCost);
    }

    return m_bookingManager.modifyBooking(bookingId, modifiedBooking);
}

BookingPtr BookingController::getBooking(int bookingId) const
{
    return m_bookingManager.getBooking(bookingId);
}

std::vector<BookingPtr> BookingController::getAllBookings() const
{
    return m_bookingManager.getAllBookings();
}

std::vector<BookingPtr> BookingController::getUserBookings(int userId) const
{
    return m_bookingManager.getBookingsByUser(userId);
}

std::vector<BookingPtr> BookingController::getCourtBookings(int courtId) const
{
    return m_bookingManager.getBookingsByCourt(courtId);
}

std::vector<BookingPtr> BookingController::getBookingsByDate(std::time_t date) const
{
    return m_bookingManager.getBookingsByDate(date);
}

std::vector<BookingPtr> BookingController::getUpcomingBookings(int userId) const
{
    auto userBookings = getUserBookings(userId);
    std::vector<BookingPtr> upcomingBookings;

    std::time_t now = std::time(nullptr);

    std::copy_if(userBookings.begin(), userBookings.end(), std::back_inserter(upcomingBookings),
                 [now](const BookingPtr &booking)
                 {
                     return booking->getStartTime() > now && booking->isActive();
                 });

    // Sort by start time
    std::sort(upcomingBookings.begin(), upcomingBookings.end(),
              [](const BookingPtr &a, const BookingPtr &b)
              {
                  return a->getStartTime() < b->getStartTime();
              });

    return upcomingBookings;
}

bool BookingController::isSlotAvailable(int courtId, std::time_t startTime, std::time_t endTime) const
{
    return m_bookingManager.isCourtAvailable(courtId, startTime, endTime);
}

bool BookingController::validateBookingTime(std::time_t startTime, std::time_t endTime) const
{
    if (startTime >= endTime)
        return false;

    // Check if start time is at least 1 hour from now
    std::time_t now = std::time(nullptr);
    std::time_t oneHourFromNow = now + 3600; // Add 1 hour (3600 seconds)
    if (startTime < oneHourFromNow)
    {
        return false; // Must book at least 1 hour in advance
    }

    return isValidBookingDuration(startTime, endTime);
}

bool BookingController::validateBookingDate(std::time_t bookingDate) const
{
    std::time_t now = std::time(nullptr);
    std::time_t today = now - (now % (24 * 3600)); // Start of today

    return bookingDate >= today;
}

double BookingController::calculateBookingCost(int courtId, std::time_t startTime, std::time_t endTime) const
{
    // This would typically get the court's hourly rate from CourtController
    // For now, using a fixed rate in VND
    double hourlyRate = 50000.0; // Default rate in VND

    double hours = static_cast<double>(endTime - startTime) / 3600.0;
    return hours * hourlyRate;
}

bool BookingController::confirmBooking(int bookingId)
{
    auto booking = getBooking(bookingId);
    if (booking && booking->getStatus() == BookingStatus::PENDING)
    {
        booking->setStatus(BookingStatus::CONFIRMED);
        return true;
    }
    return false;
}

bool BookingController::isWithinBusinessHours(std::time_t time) const
{
    struct tm *timeInfo = std::localtime(&time);
    int hour = timeInfo->tm_hour;

    // Business hours: 6 AM to 11 PM
    return hour >= 6 && hour < 23;
}

bool BookingController::isValidBookingDuration(std::time_t startTime, std::time_t endTime) const
{
    double durationHours = static_cast<double>(endTime - startTime) / 3600.0;

    // Minimum 1 hour, maximum 4 hours
    return durationHours >= 1.0 && durationHours <= 4.0;
}

std::time_t BookingController::getBusinessHoursStart() const
{
    std::time_t now = std::time(nullptr);
    struct tm *timeInfo = std::localtime(&now);
    timeInfo->tm_hour = 6;
    timeInfo->tm_min = 0;
    timeInfo->tm_sec = 0;
    return std::mktime(timeInfo);
}

std::time_t BookingController::getBusinessHoursEnd() const
{
    std::time_t now = std::time(nullptr);
    struct tm *timeInfo = std::localtime(&now);
    timeInfo->tm_hour = 23;
    timeInfo->tm_min = 0;
    timeInfo->tm_sec = 0;
    return std::mktime(timeInfo);
}
