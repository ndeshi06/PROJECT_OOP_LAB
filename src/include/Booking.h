#pragma once
#include <string>
#include <ctime>

enum class BookingStatus
{
    PENDING,
    CONFIRMED,
    CANCELLED,
    COMPLETED
};

class Booking
{
private:
    int m_id;
    int m_userId;
    int m_courtId;
    std::time_t m_bookingDate;
    std::time_t m_startTime;
    std::time_t m_endTime;
    double m_totalAmount;
    BookingStatus m_status;
    std::string m_notes;
    std::time_t m_createdAt;
    std::time_t m_updatedAt;

public:
    Booking();
    Booking(int userId, int courtId, std::time_t bookingDate,
            std::time_t startTime, std::time_t endTime, double totalAmount);

    // Getters
    int getId() const { return m_id; }
    int getUserId() const { return m_userId; }
    int getCourtId() const { return m_courtId; }
    std::time_t getBookingDate() const { return m_bookingDate; }
    std::time_t getStartTime() const { return m_startTime; }
    std::time_t getEndTime() const { return m_endTime; }
    double getTotalAmount() const { return m_totalAmount; }
    BookingStatus getStatus() const { return m_status; }
    const std::string &getNotes() const { return m_notes; }
    std::time_t getCreatedAt() const { return m_createdAt; }
    std::time_t getUpdatedAt() const { return m_updatedAt; }

    // Setters
    void setId(int id) { m_id = id; }
    void setUserId(int userId) { m_userId = userId; }
    void setCourtId(int courtId) { m_courtId = courtId; }
    void setBookingDate(std::time_t date) { m_bookingDate = date; }
    void setStartTime(std::time_t startTime) { m_startTime = startTime; }
    void setEndTime(std::time_t endTime) { m_endTime = endTime; }
    void setTotalAmount(double amount) { m_totalAmount = amount; }
    void setStatus(BookingStatus status);
    void setNotes(const std::string &notes) { m_notes = notes; }

    // Utility methods
    bool isActive() const;
    double getDurationHours() const;
    std::string getStatusString() const;
    bool conflictsWith(const Booking &other) const;
    void updateTimestamp();
};
