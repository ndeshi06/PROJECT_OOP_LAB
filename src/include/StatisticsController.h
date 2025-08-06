#pragma once
#include "Statistics.h"
#include "BookingManager.h"
#include <vector>
#include <memory>

class StatisticsController {
private:
    std::unique_ptr<Statistics> m_statistics;

public:
    StatisticsController();
    ~StatisticsController();

    // Statistics generation
    BookingStats generateBookingStats(std::time_t startDate, std::time_t endDate);
    std::vector<DailyStats> getDailyStatistics(std::time_t startDate, std::time_t endDate);
    std::vector<CourtUsageStats> getCourtStatistics(std::time_t startDate, std::time_t endDate);

    // Revenue analysis
    double calculateTotalRevenue(std::time_t startDate, std::time_t endDate);
    double calculateAverageRevenue(std::time_t startDate, std::time_t endDate);

    // Report generation
    std::string generateReport(std::time_t startDate, std::time_t endDate);
    std::string exportToCSV(std::time_t startDate, std::time_t endDate);

    // Data refresh
    void refreshStatistics();
    void updateStatistics();

private:
    void collectDataFromBookingManager();
};
