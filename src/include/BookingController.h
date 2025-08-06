#pragma once
#include "Booking.h"
#include "BookingManager.h"
#include <vector>
#include <memory>

class BookingController {
private:
    BookingManager& m_bookingManager;

public:
    BookingController();
    ~BookingController();

    // Booking operations
    bool createBooking(int userId, int courtId, std::time_t bookingDate,
                      std::time_t startTime, std::time_t endTime, 
                      const std::string& notes = "");
    bool cancelBooking(int bookingId);
    bool modifyBooking(int bookingId, std::time_t newStartTime, 
                      std::time_t newEndTime, const std::string& notes = "");
    
    // Booking retrieval
    BookingPtr getBooking(int bookingId) const;
    std::vector<BookingPtr> getUserBookings(int userId) const;
    std::vector<BookingPtr> getCourtBookings(int courtId) const;
    std::vector<BookingPtr> getBookingsByDate(std::time_t date) const;
    std::vector<BookingPtr> getBookingsInRange(std::time_t startDate, std::time_t endDate) const;
    std::vector<BookingPtr> getUpcomingBookings(int userId) const;
    std::vector<BookingPtr> getBookingHistory(int userId) const;

    // Availability checking
    bool isSlotAvailable(int courtId, std::time_t startTime, std::time_t endTime) const;
    std::vector<std::pair<std::time_t, std::time_t>> getAvailableSlots(
        int courtId, std::time_t date, int slotDurationMinutes = 60) const;
    
    // Booking validation
    bool validateBookingTime(std::time_t startTime, std::time_t endTime) const;
    bool validateBookingDate(std::time_t bookingDate) const;
    double calculateBookingCost(int courtId, std::time_t startTime, std::time_t endTime) const;

    // Status management
    bool confirmBooking(int bookingId);
    bool completeBooking(int bookingId);
    std::vector<BookingPtr> getPendingBookings() const;
    std::vector<BookingPtr> getConfirmedBookings() const;

    // Reminder and notification support
    std::vector<BookingPtr> getBookingsForReminder(int hoursAhead = 24) const;
    void sendBookingReminders();

    // Statistics support
    int getTotalBookingCount() const;
    double getTotalRevenue() const;
    std::vector<BookingPtr> getMostRecentBookings(int limit = 10) const;

private:
    // Helper methods
    bool isWithinBusinessHours(std::time_t time) const;
    bool isValidBookingDuration(std::time_t startTime, std::time_t endTime) const;
    std::time_t getBusinessHoursStart() const;
    std::time_t getBusinessHoursEnd() const;
};
