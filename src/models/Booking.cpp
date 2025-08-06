#include "Booking.h"
#include <ctime>
#include <algorithm>

Booking::Booking()
    : m_id(0), m_userId(0), m_courtId(0), m_bookingDate(0), m_startTime(0), 
      m_endTime(0), m_totalAmount(0.0), m_status(BookingStatus::PENDING),
      m_createdAt(std::time(nullptr)), m_updatedAt(std::time(nullptr)) {
}

Booking::Booking(int userId, int courtId, std::time_t bookingDate,
                 std::time_t startTime, std::time_t endTime, double totalAmount)
    : m_id(0), m_userId(userId), m_courtId(courtId), m_bookingDate(bookingDate),
      m_startTime(startTime), m_endTime(endTime), m_totalAmount(totalAmount),
      m_status(BookingStatus::PENDING), m_createdAt(std::time(nullptr)), 
      m_updatedAt(std::time(nullptr)) {
}

void Booking::setStatus(BookingStatus status) {
    m_status = status;
    updateTimestamp();
}

bool Booking::isActive() const {
    return m_status == BookingStatus::PENDING || m_status == BookingStatus::CONFIRMED;
}

double Booking::getDurationHours() const {
    if (m_endTime <= m_startTime) return 0.0;
    return static_cast<double>(m_endTime - m_startTime) / 3600.0; // Convert seconds to hours
}

std::string Booking::getStatusString() const {
    switch (m_status) {
        case BookingStatus::PENDING:
            return "Pending";
        case BookingStatus::CONFIRMED:
            return "Confirmed";
        case BookingStatus::CANCELLED:
            return "Cancelled";
        case BookingStatus::COMPLETED:
            return "Completed";
        default:
            return "Unknown";
    }
}

bool Booking::conflictsWith(const Booking& other) const {
    if (m_courtId != other.m_courtId) return false;
    if (m_bookingDate != other.m_bookingDate) return false;
    
    // Check time overlap
    return !(m_endTime <= other.m_startTime || m_startTime >= other.m_endTime);
}

void Booking::updateTimestamp() {
    m_updatedAt = std::time(nullptr);
}
