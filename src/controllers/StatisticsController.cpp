#include "StatisticsController.h"
#include <iostream>

StatisticsController::StatisticsController() {
    m_statistics = std::make_unique<Statistics>();
}

StatisticsController::~StatisticsController() {
}

BookingStats StatisticsController::generateBookingStats(std::time_t startDate, std::time_t endDate) {
    return m_statistics->calculateBookingStats(startDate, endDate);
}

std::vector<DailyStats> StatisticsController::getDailyStatistics(std::time_t startDate, std::time_t endDate) {
    return m_statistics->getDailyStats(startDate, endDate);
}

std::vector<CourtUsageStats> StatisticsController::getCourtStatistics(std::time_t startDate, std::time_t endDate) {
    return m_statistics->getCourtUsageStats(startDate, endDate);
}

double StatisticsController::calculateTotalRevenue(std::time_t startDate, std::time_t endDate) {
    return m_statistics->getTotalRevenue(startDate, endDate);
}

double StatisticsController::calculateAverageRevenue(std::time_t startDate, std::time_t endDate) {
    return m_statistics->getAverageRevenuePerDay(startDate, endDate);
}

std::string StatisticsController::generateReport(std::time_t startDate, std::time_t endDate) {
    return m_statistics->generateReport(startDate, endDate);
}

std::string StatisticsController::exportToCSV(std::time_t startDate, std::time_t endDate) {
    return m_statistics->exportToCSV(startDate, endDate);
}

void StatisticsController::refreshStatistics() {
    collectDataFromBookingManager();
}

void StatisticsController::updateStatistics() {
    collectDataFromBookingManager();
}

void StatisticsController::collectDataFromBookingManager() {
    // Placeholder for collecting data from BookingManager
    std::cout << "Collecting statistics data..." << std::endl;
}
