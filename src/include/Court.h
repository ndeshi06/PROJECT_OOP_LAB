#pragma once
#include <string>
#include <memory>
#include <ctime>

enum class CourtStatus {
    AVAILABLE,
    MAINTENANCE,
    OUT_OF_SERVICE
};

class Court {
private:
    int m_id;
    std::string m_name;
    std::string m_description;
    double m_hourlyRate;
    CourtStatus m_status;
    std::time_t m_createdAt;
    std::time_t m_updatedAt;

public:
    Court();
    Court(const std::string& name, const std::string& description, 
          double hourlyRate, CourtStatus status = CourtStatus::AVAILABLE);

    // Getters
    int getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    double getHourlyRate() const { return m_hourlyRate; }
    CourtStatus getStatus() const { return m_status; }
    std::time_t getCreatedAt() const { return m_createdAt; }
    std::time_t getUpdatedAt() const { return m_updatedAt; }

    // Setters
    void setId(int id) { m_id = id; }
    void setName(const std::string& name);
    void setDescription(const std::string& description) { m_description = description; }
    void setHourlyRate(double rate);
    void setStatus(CourtStatus status);

    // Utility methods
    bool isAvailable() const { return m_status == CourtStatus::AVAILABLE; }
    std::string getStatusString() const;
    void updateTimestamp();
};

using CourtPtr = std::shared_ptr<Court>;
