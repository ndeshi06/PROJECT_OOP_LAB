#pragma once
#include "Booking.h"
#include "Court.h"
#include "User.h"
#include "NotificationObserver.h"
#include <vector>
#include <mutex>

// Forward declarations
class Booking;
class NotificationObserver;

// Singleton pattern for managing all booking operations
class BookingManager
{
private:
    static BookingManager *m_instance;
    static std::mutex m_mutex;

    std::vector<Booking *> m_bookings;
    std::vector<NotificationObserver *> m_observers;

    // Private constructor for Singleton
    BookingManager() = default;

public:
    // Destructor to clean up memory
    ~BookingManager();

    // Singleton access
    static BookingManager &getInstance();
    static void cleanup(); // Method to clean up singleton instance

    // Delete copy constructor and assignment operator
    BookingManager(const BookingManager &) = delete;
    BookingManager &operator=(const BookingManager &) = delete;

    // Booking operations
    bool createBooking(const Booking &booking);
    bool cancelBooking(int bookingId);
    bool modifyBooking(int bookingId, const Booking &newBooking);
    Booking *getBooking(int bookingId) const;
    std::vector<Booking *> getBookingsByUser(int userId) const;
    std::vector<Booking *> getBookingsByCourt(int courtId) const;
    std::vector<Booking *> getBookingsByDate(std::time_t date) const;
    std::vector<Booking *> getAllBookings() const;

    // Availability checking
    bool isCourtAvailable(int courtId, std::time_t startTime, std::time_t endTime) const;
    std::vector<std::pair<std::time_t, std::time_t>> getAvailableSlots(
        int courtId, std::time_t date, int slotDurationMinutes = 60) const;
    bool hasConflict(const Booking &booking) const;

    // Observer pattern for notifications
    void addObserver(NotificationObserver *observer);
    void removeObserver(NotificationObserver *observer);
    void notifyObservers(const std::string &message, const Booking &booking);

    // Statistics support
    std::vector<Booking *> getBookingsInDateRange(std::time_t startDate, std::time_t endDate) const;
    double getTotalRevenue(std::time_t startDate, std::time_t endDate) const;
    int getBookingCount(std::time_t startDate, std::time_t endDate) const;

    // Data management
    void loadBookings();
    void saveBookings();
    void clearAllBookings();

private:
    // Helper methods
    bool validateBooking(const Booking &booking) const;
    void generateBookingId(Booking &booking);
    void sortBookingsByDate();
};
