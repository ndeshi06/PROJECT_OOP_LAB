#include "BookingManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>

std::unique_ptr<BookingManager> BookingManager::m_instance = nullptr;
std::mutex BookingManager::m_mutex;

BookingManager& BookingManager::getInstance() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_instance == nullptr) {
        m_instance = std::unique_ptr<BookingManager>(new BookingManager());
    }
    return *m_instance;
}

bool BookingManager::createBooking(const Booking& booking) {
    if (!validateBooking(booking)) {
        return false;
    }
    
    if (hasConflict(booking)) {
        return false;
    }
    
    // Create a copy and assign ID
    Booking newBooking = booking;
    generateBookingId(newBooking);
    
    // Add to bookings
    auto bookingPtr = std::make_shared<Booking>(newBooking);
    m_bookings.push_back(bookingPtr);
    
    // Sort by date
    sortBookingsByDate();
    
    // Notify observers
    notifyObservers("Booking created", newBooking);
    
    return true;
}

bool BookingManager::cancelBooking(int bookingId) {
    auto it = std::find_if(m_bookings.begin(), m_bookings.end(),
        [bookingId](const BookingPtr& booking) {
            return booking->getId() == bookingId;
        });
    
    if (it != m_bookings.end()) {
        (*it)->setStatus(BookingStatus::CANCELLED);
        notifyObservers("Booking cancelled", **it);
        return true;
    }
    
    return false;
}

bool BookingManager::modifyBooking(int bookingId, const Booking& newBooking) {
    auto it = std::find_if(m_bookings.begin(), m_bookings.end(),
        [bookingId](const BookingPtr& booking) {
            return booking->getId() == bookingId;
        });
    
    if (it != m_bookings.end()) {
        Booking oldBooking = **it;
        
        // Update booking details
        (*it)->setStartTime(newBooking.getStartTime());
        (*it)->setEndTime(newBooking.getEndTime());
        (*it)->setTotalAmount(newBooking.getTotalAmount());
        (*it)->setNotes(newBooking.getNotes());
        
        // Check for conflicts with new time
        if (hasConflict(**it)) {
            // Revert changes
            (*it)->setStartTime(oldBooking.getStartTime());
            (*it)->setEndTime(oldBooking.getEndTime());
            (*it)->setTotalAmount(oldBooking.getTotalAmount());
            (*it)->setNotes(oldBooking.getNotes());
            return false;
        }
        
        notifyObservers("Booking modified", **it);
        return true;
    }
    
    return false;
}

BookingPtr BookingManager::getBooking(int bookingId) const {
    auto it = std::find_if(m_bookings.begin(), m_bookings.end(),
        [bookingId](const BookingPtr& booking) {
            return booking->getId() == bookingId;
        });
    
    return (it != m_bookings.end()) ? *it : nullptr;
}

std::vector<BookingPtr> BookingManager::getBookingsByUser(int userId) const {
    std::vector<BookingPtr> userBookings;
    
    std::copy_if(m_bookings.begin(), m_bookings.end(), std::back_inserter(userBookings),
        [userId](const BookingPtr& booking) {
            return booking->getUserId() == userId;
        });
    
    return userBookings;
}

std::vector<BookingPtr> BookingManager::getBookingsByCourt(int courtId) const {
    std::vector<BookingPtr> courtBookings;
    
    std::copy_if(m_bookings.begin(), m_bookings.end(), std::back_inserter(courtBookings),
        [courtId](const BookingPtr& booking) {
            return booking->getCourtId() == courtId;
        });
    
    return courtBookings;
}

std::vector<BookingPtr> BookingManager::getBookingsByDate(std::time_t date) const {
    std::vector<BookingPtr> dateBookings;
    
    std::copy_if(m_bookings.begin(), m_bookings.end(), std::back_inserter(dateBookings),
        [date](const BookingPtr& booking) {
            // Compare dates (ignoring time)
            std::time_t bookingDate = booking->getBookingDate();
            struct tm* bookingTm = std::localtime(&bookingDate);
            struct tm* searchTm = std::localtime(&date);
            
            return (bookingTm->tm_year == searchTm->tm_year &&
                    bookingTm->tm_mon == searchTm->tm_mon &&
                    bookingTm->tm_mday == searchTm->tm_mday);
        });
    
    return dateBookings;
}

bool BookingManager::isCourtAvailable(int courtId, std::time_t startTime, std::time_t endTime) const {
    return std::none_of(m_bookings.begin(), m_bookings.end(),
        [courtId, startTime, endTime](const BookingPtr& booking) {
            if (booking->getCourtId() != courtId || !booking->isActive()) {
                return false;
            }
            
            // Check for time overlap
            return !(endTime <= booking->getStartTime() || startTime >= booking->getEndTime());
        });
}

std::vector<std::pair<std::time_t, std::time_t>> BookingManager::getAvailableSlots(
    int courtId, std::time_t date, int slotDurationMinutes) const {
    
    std::vector<std::pair<std::time_t, std::time_t>> availableSlots;
    
    // Business hours: 6 AM to 11 PM
    struct tm* dateTm = std::localtime(&date);
    dateTm->tm_hour = 6;
    dateTm->tm_min = 0;
    dateTm->tm_sec = 0;
    std::time_t startOfDay = std::mktime(dateTm);
    
    dateTm->tm_hour = 23;
    std::time_t endOfDay = std::mktime(dateTm);
    
    // Generate time slots
    std::time_t currentSlot = startOfDay;
    int slotDurationSeconds = slotDurationMinutes * 60;
    
    while (currentSlot + slotDurationSeconds <= endOfDay) {
        std::time_t slotEnd = currentSlot + slotDurationSeconds;
        
        if (isCourtAvailable(courtId, currentSlot, slotEnd)) {
            availableSlots.push_back(std::make_pair(currentSlot, slotEnd));
        }
        
        currentSlot += slotDurationSeconds;
    }
    
    return availableSlots;
}

bool BookingManager::hasConflict(const Booking& booking) const {
    return std::any_of(m_bookings.begin(), m_bookings.end(),
        [&booking](const BookingPtr& existingBooking) {
            return existingBooking->getId() != booking.getId() &&
                   existingBooking->isActive() &&
                   existingBooking->conflictsWith(booking);
        });
}

void BookingManager::addObserver(std::shared_ptr<NotificationObserver> observer) {
    m_observers.push_back(observer);
}

void BookingManager::removeObserver(std::shared_ptr<NotificationObserver> observer) {
    m_observers.erase(
        std::remove(m_observers.begin(), m_observers.end(), observer),
        m_observers.end());
}

void BookingManager::notifyObservers(const std::string& message, const Booking& booking) {
    for (auto& observer : m_observers) {
        if (message == "Booking created") {
            observer->onBookingCreated(booking);
        } else if (message == "Booking cancelled") {
            observer->onBookingCancelled(booking);
        } else if (message == "Booking modified") {
            // For simplicity, using the same booking for old and new
            observer->onBookingModified(booking, booking);
        }
    }
}

std::vector<BookingPtr> BookingManager::getAllBookings() const {
    return m_bookings;
}

std::vector<BookingPtr> BookingManager::getBookingsInDateRange(std::time_t startDate, std::time_t endDate) const {
    std::vector<BookingPtr> rangeBookings;
    
    std::copy_if(m_bookings.begin(), m_bookings.end(), std::back_inserter(rangeBookings),
        [startDate, endDate](const BookingPtr& booking) {
            std::time_t bookingDate = booking->getBookingDate();
            return bookingDate >= startDate && bookingDate <= endDate;
        });
    
    return rangeBookings;
}

double BookingManager::getTotalRevenue(std::time_t startDate, std::time_t endDate) const {
    auto bookings = getBookingsInDateRange(startDate, endDate);
    double totalRevenue = 0.0;
    
    for (const auto& booking : bookings) {
        if (booking->getStatus() == BookingStatus::CONFIRMED || 
            booking->getStatus() == BookingStatus::COMPLETED) {
            totalRevenue += booking->getTotalAmount();
        }
    }
    
    return totalRevenue;
}

int BookingManager::getBookingCount(std::time_t startDate, std::time_t endDate) const {
    auto bookings = getBookingsInDateRange(startDate, endDate);
    return static_cast<int>(bookings.size());
}

void BookingManager::loadBookings() {
    // Implementation for loading bookings from file/database
    // For now, this is a placeholder
}

void BookingManager::saveBookings() {
    // Implementation for saving bookings to file/database
    // For now, this is a placeholder
}

bool BookingManager::validateBooking(const Booking& booking) const {
    // Basic validation
    if (booking.getUserId() <= 0 || booking.getCourtId() <= 0) {
        return false;
    }
    
    if (booking.getStartTime() >= booking.getEndTime()) {
        return false;
    }
    
    if (booking.getTotalAmount() < 0) {
        return false;
    }
    
    return true;
}

void BookingManager::generateBookingId(Booking& booking) {
    int maxId = 0;
    for (const auto& b : m_bookings) {
        if (b->getId() > maxId) {
            maxId = b->getId();
        }
    }
    booking.setId(maxId + 1);
}

void BookingManager::sortBookingsByDate() {
    std::sort(m_bookings.begin(), m_bookings.end(),
        [](const BookingPtr& a, const BookingPtr& b) {
            return a->getStartTime() < b->getStartTime();
        });
}
