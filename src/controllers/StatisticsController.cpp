#include "StatisticsController.h"
#include "BookingManager.h"
#include "Booking.h"
#include <iostream>
#include <map>
#include <ctime>

StatisticsController::StatisticsController()
{
    m_statistics = new Statistics();
    // Auto-collect data on initialization
    collectDataFromBookingManager();
}

StatisticsController::~StatisticsController() 
{
    delete m_statistics;
}

BookingStats StatisticsController::generateBookingStats(std::time_t startDate, std::time_t endDate)
{
    return m_statistics->calculateBookingStats(startDate, endDate);
}

std::vector<DailyStats> StatisticsController::getDailyStatistics(std::time_t startDate, std::time_t endDate)
{
    return m_statistics->getDailyStats(startDate, endDate);
}

std::vector<CourtUsageStats> StatisticsController::getCourtStatistics(std::time_t startDate, std::time_t endDate)
{
    return m_statistics->getCourtUsageStats(startDate, endDate);
}

double StatisticsController::calculateTotalRevenue(std::time_t startDate, std::time_t endDate)
{
    return m_statistics->getTotalRevenue(startDate, endDate);
}

double StatisticsController::calculateAverageRevenue(std::time_t startDate, std::time_t endDate)
{
    return m_statistics->getAverageRevenuePerDay(startDate, endDate);
}

std::string StatisticsController::generateReport(std::time_t startDate, std::time_t endDate)
{
    return m_statistics->generateReport(startDate, endDate);
}

std::string StatisticsController::exportToCSV(std::time_t startDate, std::time_t endDate)
{
    return m_statistics->exportToCSV(startDate, endDate);
}

void StatisticsController::refreshStatistics()
{
    collectDataFromBookingManager();
}

void StatisticsController::updateStatistics()
{
    collectDataFromBookingManager();
}

void StatisticsController::collectDataFromBookingManager()
{
    // Get all bookings from BookingManager
    BookingManager &bookingManager = BookingManager::getInstance();
    auto allBookings = bookingManager.getAllBookings();

    // Clear existing statistics
    delete m_statistics;
    m_statistics = new Statistics();

    // Group bookings by date and court for statistics
    std::map<std::time_t, std::map<int, std::vector<Booking*>>> dailyCourtBookings;

    for (const auto &booking : allBookings)
    {
        if (!booking)
            continue;

        // Skip cancelled bookings - only count confirmed/pending/completed
        if (booking->getStatus() == BookingStatus::CANCELLED)
        {
            continue;
        }

        // Get date (midnight of booking date)
        std::time_t bookingDate = booking->getBookingDate();
        std::tm *tm = std::localtime(&bookingDate);
        tm->tm_hour = 0;
        tm->tm_min = 0;
        tm->tm_sec = 0;
        std::time_t dateKey = std::mktime(tm);

        dailyCourtBookings[dateKey][booking->getCourtId()].push_back(booking);
    }

    // Calculate daily statistics
    for (const auto &dayPair : dailyCourtBookings)
    {
        std::time_t date = dayPair.first;
        int totalBookingsForDay = 0;
        double totalRevenueForDay = 0.0;

        for (const auto &courtPair : dayPair.second)
        {
            int courtId = courtPair.first;
            const auto &courtBookings = courtPair.second;

            int courtBookingCount = courtBookings.size();
            double courtRevenue = 0.0;

            for (const auto &booking : courtBookings)
            {
                courtRevenue += booking->getTotalAmount();
            }

            totalBookingsForDay += courtBookingCount;
            totalRevenueForDay += courtRevenue;

            // Update court statistics
            std::string courtName = "Court " + std::to_string(courtId);
            double utilizationRate = std::min(100.0, (courtBookingCount * 100.0) / 12.0); // Assuming 12 slots per day

            m_statistics->updateCourtStats(courtId, courtName, courtBookingCount, courtRevenue, utilizationRate);
        }

        // Update daily statistics
        m_statistics->updateDailyStats(date, totalBookingsForDay, totalRevenueForDay);
    }

    std::cout << "Statistics data collected successfully." << std::endl;
}
