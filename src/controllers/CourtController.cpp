#include "CourtController.h"
#include <algorithm>
#include <fstream>

CourtController::CourtController() {
}

CourtController::~CourtController() {
    saveCourts();
}

bool CourtController::addCourt(const std::string& name, const std::string& description, 
                              double hourlyRate, CourtStatus status) {
    if (!validateCourtData(name, hourlyRate)) {
        return false;
    }
    
    if (isCourtNameTaken(name)) {
        return false;
    }
    
    auto newCourt = std::make_shared<Court>(name, description, hourlyRate, status);
    newCourt->setId(generateCourtId());
    
    m_courts.push_back(newCourt);
    return true;
}

bool CourtController::updateCourt(int courtId, const Court& updatedCourt) {
    auto court = getCourt(courtId);
    if (!court) return false;
    
    if (!validateCourtData(updatedCourt.getName(), updatedCourt.getHourlyRate())) {
        return false;
    }
    
    if (court->getName() != updatedCourt.getName() && 
        isCourtNameTaken(updatedCourt.getName(), courtId)) {
        return false;
    }
    
    court->setName(updatedCourt.getName());
    court->setDescription(updatedCourt.getDescription());
    court->setHourlyRate(updatedCourt.getHourlyRate());
    court->setStatus(updatedCourt.getStatus());
    
    return true;
}

bool CourtController::deleteCourt(int courtId) {
    auto it = std::find_if(m_courts.begin(), m_courts.end(),
        [courtId](const CourtPtr& court) {
            return court->getId() == courtId;
        });
    
    if (it != m_courts.end()) {
        m_courts.erase(it);
        return true;
    }
    
    return false;
}

CourtPtr CourtController::getCourt(int courtId) const {
    auto it = std::find_if(m_courts.begin(), m_courts.end(),
        [courtId](const CourtPtr& court) {
            return court->getId() == courtId;
        });
    
    return (it != m_courts.end()) ? *it : nullptr;
}

std::vector<CourtPtr> CourtController::getAllCourts() const {
    return m_courts;
}

std::vector<CourtPtr> CourtController::getAvailableCourts() const {
    std::vector<CourtPtr> availableCourts;
    
    std::copy_if(m_courts.begin(), m_courts.end(), std::back_inserter(availableCourts),
        [](const CourtPtr& court) {
            return court->isAvailable();
        });
    
    return availableCourts;
}

bool CourtController::setCourtStatus(int courtId, CourtStatus status) {
    auto court = getCourt(courtId);
    if (court) {
        court->setStatus(status);
        return true;
    }
    return false;
}

int CourtController::getAvailableCourtCount() const {
    return static_cast<int>(getAvailableCourts().size());
}

bool CourtController::validateCourtData(const std::string& name, double hourlyRate) const {
    return !name.empty() && hourlyRate >= 0;
}

bool CourtController::isCourtNameTaken(const std::string& name, int excludeId) const {
    return std::any_of(m_courts.begin(), m_courts.end(),
        [&name, excludeId](const CourtPtr& court) {
            return court->getName() == name && court->getId() != excludeId;
        });
}

void CourtController::loadCourts() {
    // Placeholder for loading courts from file/database
}

void CourtController::saveCourts() {
    // Placeholder for saving courts to file/database
}

int CourtController::generateCourtId() {
    int maxId = 0;
    for (const auto& court : m_courts) {
        if (court->getId() > maxId) {
            maxId = court->getId();
        }
    }
    return maxId + 1;
}
