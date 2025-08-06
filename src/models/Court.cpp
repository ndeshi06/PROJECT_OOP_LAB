#include "Court.h"
#include <ctime>

Court::Court() 
    : m_id(0), m_hourlyRate(0.0), m_status(CourtStatus::AVAILABLE), 
      m_createdAt(std::time(nullptr)), m_updatedAt(std::time(nullptr)) {
}

Court::Court(const std::string& name, const std::string& description, 
             double hourlyRate, CourtStatus status)
    : m_id(0), m_name(name), m_description(description), m_hourlyRate(hourlyRate), 
      m_status(status), m_createdAt(std::time(nullptr)), m_updatedAt(std::time(nullptr)) {
}

void Court::setName(const std::string& name) {
    m_name = name;
    updateTimestamp();
}

void Court::setHourlyRate(double rate) {
    if (rate >= 0) {
        m_hourlyRate = rate;
        updateTimestamp();
    }
}

void Court::setStatus(CourtStatus status) {
    m_status = status;
    updateTimestamp();
}

std::string Court::getStatusString() const {
    switch (m_status) {
        case CourtStatus::AVAILABLE:
            return "Available";
        case CourtStatus::MAINTENANCE:
            return "Under Maintenance";
        case CourtStatus::OUT_OF_SERVICE:
            return "Out of Service";
        default:
            return "Unknown";
    }
}

void Court::updateTimestamp() {
    m_updatedAt = std::time(nullptr);
}
