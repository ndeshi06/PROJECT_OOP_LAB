#pragma once
#include <vector>
#include <map>
#include <string>
#include <ctime>

struct BookingStats {
    int totalBookings;
    int confirmedBookings;
    int cancelledBookings;
    double totalRevenue;
    double averageBookingValue;
};

struct DailyStats {
    std::time_t date;
    int bookingCount;
    double revenue;
};

struct CourtUsageStats {
    int courtId;
    std::string courtName;
    int bookingCount;
    double revenue;
    double utilizationRate;
};

class Statistics {
private:
    std::map<std::time_t, DailyStats> m_dailyStats;
    std::map<int, CourtUsageStats> m_courtStats;

public:
    Statistics();

    // Statistics calculation methods
    BookingStats calculateBookingStats(std::time_t startDate, std::time_t endDate) const;
    std::vector<DailyStats> getDailyStats(std::time_t startDate, std::time_t endDate) const;
    std::vector<CourtUsageStats> getCourtUsageStats(std::time_t startDate, std::time_t endDate) const;
    
    // Revenue analysis
    double getTotalRevenue(std::time_t startDate, std::time_t endDate) const;
    double getAverageRevenuePerDay(std::time_t startDate, std::time_t endDate) const;
    std::map<int, double> getRevenueByMonth(int year) const;
    std::map<int, double> getRevenueByWeek(int year, int month) const;

    // Booking analysis
    int getTotalBookings(std::time_t startDate, std::time_t endDate) const;
    double getAverageBookingsPerDay(std::time_t startDate, std::time_t endDate) const;
    std::map<int, int> getBookingsByHour() const;
    std::map<int, int> getBookingsByDayOfWeek() const;

    // Court utilization
    double getCourtUtilizationRate(int courtId, std::time_t startDate, std::time_t endDate) const;
    std::vector<int> getMostPopularCourts(int limit = 5) const;

    // Update methods
    void updateDailyStats(std::time_t date, int bookingCount, double revenue);
    void updateCourtStats(int courtId, const std::string& courtName, 
                         int bookingCount, double revenue, double utilizationRate);

    // Data export
    std::string exportToCSV(std::time_t startDate, std::time_t endDate) const;
    std::string generateReport(std::time_t startDate, std::time_t endDate) const;
};
