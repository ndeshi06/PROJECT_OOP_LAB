#include "Statistics.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

Statistics::Statistics() {
}

BookingStats Statistics::calculateBookingStats(std::time_t startDate, std::time_t endDate) const {
    BookingStats stats = {};
    
    for (const auto& pair : m_dailyStats) {
        if (pair.first >= startDate && pair.first <= endDate) {
            stats.totalBookings += pair.second.bookingCount;
            stats.totalRevenue += pair.second.revenue;
        }
    }
    
    if (stats.totalBookings > 0) {
        stats.averageBookingValue = stats.totalRevenue / stats.totalBookings;
    }
    
    return stats;
}

std::vector<DailyStats> Statistics::getDailyStats(std::time_t startDate, std::time_t endDate) const {
    std::vector<DailyStats> result;
    
    for (const auto& pair : m_dailyStats) {
        if (pair.first >= startDate && pair.first <= endDate) {
            result.push_back(pair.second);
        }
    }
    
    // Sort by date
    std::sort(result.begin(), result.end(), 
        [](const DailyStats& a, const DailyStats& b) {
            return a.date < b.date;
        });
    
    return result;
}

std::vector<CourtUsageStats> Statistics::getCourtUsageStats(std::time_t startDate, std::time_t endDate) const {
    std::vector<CourtUsageStats> result;
    
    for (const auto& pair : m_courtStats) {
        result.push_back(pair.second);
    }
    
    // Sort by booking count (descending)
    std::sort(result.begin(), result.end(),
        [](const CourtUsageStats& a, const CourtUsageStats& b) {
            return a.bookingCount > b.bookingCount;
        });
    
    return result;
}

double Statistics::getTotalRevenue(std::time_t startDate, std::time_t endDate) const {
    double totalRevenue = 0.0;
    
    for (const auto& pair : m_dailyStats) {
        if (pair.first >= startDate && pair.first <= endDate) {
            totalRevenue += pair.second.revenue;
        }
    }
    
    return totalRevenue;
}

double Statistics::getAverageRevenuePerDay(std::time_t startDate, std::time_t endDate) const {
    double totalRevenue = getTotalRevenue(startDate, endDate);
    double daysDiff = static_cast<double>(endDate - startDate) / (24 * 3600);
    
    return daysDiff > 0 ? totalRevenue / daysDiff : 0.0;
}

int Statistics::getTotalBookings(std::time_t startDate, std::time_t endDate) const {
    int totalBookings = 0;
    
    for (const auto& pair : m_dailyStats) {
        if (pair.first >= startDate && pair.first <= endDate) {
            totalBookings += pair.second.bookingCount;
        }
    }
    
    return totalBookings;
}

double Statistics::getAverageBookingsPerDay(std::time_t startDate, std::time_t endDate) const {
    int totalBookings = getTotalBookings(startDate, endDate);
    double daysDiff = static_cast<double>(endDate - startDate) / (24 * 3600);
    
    return daysDiff > 0 ? static_cast<double>(totalBookings) / daysDiff : 0.0;
}

void Statistics::updateDailyStats(std::time_t date, int bookingCount, double revenue) {
    DailyStats stats;
    stats.date = date;
    stats.bookingCount = bookingCount;
    stats.revenue = revenue;
    
    m_dailyStats[date] = stats;
}

void Statistics::updateCourtStats(int courtId, const std::string& courtName, 
                                int bookingCount, double revenue, double utilizationRate) {
    CourtUsageStats stats;
    stats.courtId = courtId;
    stats.courtName = courtName;
    stats.bookingCount = bookingCount;
    stats.revenue = revenue;
    stats.utilizationRate = utilizationRate;
    
    m_courtStats[courtId] = stats;
}

std::string Statistics::exportToCSV(std::time_t startDate, std::time_t endDate) const {
    std::ostringstream csv;
    csv << "Date,Bookings,Revenue\n";
    
    auto dailyStats = getDailyStats(startDate, endDate);
    for (const auto& stats : dailyStats) {
        csv << std::put_time(std::localtime(&stats.date), "%Y-%m-%d") << ","
            << stats.bookingCount << ","
            << std::fixed << std::setprecision(2) << stats.revenue << "\n";
    }
    
    return csv.str();
}

std::string Statistics::generateReport(std::time_t startDate, std::time_t endDate) const {
    std::ostringstream report;
    
    auto bookingStats = calculateBookingStats(startDate, endDate);
    
    report << "=== Badminton Court Statistics Report ===\n\n";
    report << "Period: " << std::put_time(std::localtime(&startDate), "%Y-%m-%d")
           << " to " << std::put_time(std::localtime(&endDate), "%Y-%m-%d") << "\n\n";
    
    report << "Total Bookings: " << bookingStats.totalBookings << "\n";
    report << "Total Revenue: $" << std::fixed << std::setprecision(2) 
           << bookingStats.totalRevenue << "\n";
    report << "Average Booking Value: $" << std::fixed << std::setprecision(2)
           << bookingStats.averageBookingValue << "\n\n";
    
    report << "=== Court Usage Statistics ===\n";
    auto courtStats = getCourtUsageStats(startDate, endDate);
    for (const auto& stats : courtStats) {
        report << "Court " << stats.courtName << ": "
               << stats.bookingCount << " bookings, $"
               << std::fixed << std::setprecision(2) << stats.revenue << " revenue\n";
    }
    
    return report.str();
}
