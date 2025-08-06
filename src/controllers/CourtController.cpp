#include "CourtController.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>

CourtController::CourtController() {
    loadCourts(); // Load existing courts on initialization
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
    saveCourts(); // Save changes immediately
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
    
    saveCourts(); // Save changes immediately
    return true;
}

bool CourtController::deleteCourt(int courtId) {
    auto it = std::find_if(m_courts.begin(), m_courts.end(),
        [courtId](const CourtPtr& court) {
            return court->getId() == courtId;
        });
    
    if (it != m_courts.end()) {
        m_courts.erase(it);
        saveCourts(); // Save changes immediately
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
        saveCourts(); // Save changes immediately
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
    std::ifstream file("data/courts.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            // Parse court data: id|name|description|hourlyRate|status
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, '|')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 5) {
                try {
                    auto court = std::make_shared<Court>();
                    court->setId(std::stoi(tokens[0]));
                    court->setName(tokens[1]);
                    court->setDescription(tokens[2]);
                    court->setHourlyRate(std::stod(tokens[3]));
                    
                    // Parse status
                    CourtStatus status = CourtStatus::AVAILABLE;
                    if (tokens[4] == "MAINTENANCE") {
                        status = CourtStatus::MAINTENANCE;
                    } else if (tokens[4] == "OUT_OF_SERVICE") {
                        status = CourtStatus::OUT_OF_SERVICE;
                    }
                    court->setStatus(status);
                    
                    m_courts.push_back(court);
                } catch (const std::exception& e) {
                    // Skip invalid lines
                    continue;
                }
            }
        }
        file.close();
    }
}

void CourtController::saveCourts() {
    // Create data directory if it doesn't exist
    std::filesystem::create_directories("data");
    
    std::ofstream file("data/courts.txt");
    if (file.is_open()) {
        for (const auto& court : m_courts) {
            if (!court) continue;
            
            // Write court data: id|name|description|hourlyRate|status
            std::string statusStr = "AVAILABLE";
            switch (court->getStatus()) {
                case CourtStatus::MAINTENANCE:
                    statusStr = "MAINTENANCE";
                    break;
                case CourtStatus::OUT_OF_SERVICE:
                    statusStr = "OUT_OF_SERVICE";
                    break;
                default:
                    statusStr = "AVAILABLE";
                    break;
            }
            
            file << court->getId() << "|"
                 << court->getName() << "|"
                 << court->getDescription() << "|"
                 << court->getHourlyRate() << "|"
                 << statusStr << std::endl;
        }
        file.close();
    }
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
