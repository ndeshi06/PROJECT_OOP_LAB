#pragma once
#include "Court.h"
#include <vector>
#include <memory>

class CourtController {
private:
    std::vector<CourtPtr> m_courts;

public:
    CourtController();
    ~CourtController();

    // Court CRUD operations
    bool addCourt(const std::string& name, const std::string& description, 
                  double hourlyRate, CourtStatus status = CourtStatus::AVAILABLE);
    bool updateCourt(int courtId, const Court& updatedCourt);
    bool deleteCourt(int courtId);
    CourtPtr getCourt(int courtId) const;
    std::vector<CourtPtr> getAllCourts() const;
    std::vector<CourtPtr> getAvailableCourts() const;

    // Court management
    bool setCourtStatus(int courtId, CourtStatus status);
    bool updateCourtRate(int courtId, double newRate);
    int getCourtCount() const { return static_cast<int>(m_courts.size()); }
    int getAvailableCourtCount() const;

    // Search and filter
    std::vector<CourtPtr> searchCourts(const std::string& searchTerm) const;
    std::vector<CourtPtr> getCourtsByStatus(CourtStatus status) const;
    std::vector<CourtPtr> getCourtsByPriceRange(double minRate, double maxRate) const;

    // Validation
    bool isCourtNameTaken(const std::string& name, int excludeId = -1) const;
    bool validateCourtData(const std::string& name, double hourlyRate) const;

    // Data persistence
    void loadCourts();
    void saveCourts();

private:
    // Helper methods
    int generateCourtId();
    bool courtExists(int courtId) const;
};
