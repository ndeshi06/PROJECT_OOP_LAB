#include "NotificationObserver.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

// EmailNotificationObserver implementation
EmailNotificationObserver::EmailNotificationObserver(const std::string& smtpServer, int smtpPort,
                                                   const std::string& username, const std::string& password)
    : m_smtpServer(smtpServer), m_smtpPort(smtpPort), m_username(username), 
      m_password(password), m_isEnabled(true) {
}

void EmailNotificationObserver::onBookingCreated(const Booking& booking) {
    if (!m_isEnabled) return;
    
    std::string subject = "Booking Confirmation - Badminton Court";
    std::string body = "Dear Customer,\n\n";
    body += "Your booking has been successfully created.\n\n";
    body += formatBookingDetails(booking);
    body += "\n\nThank you for choosing our badminton courts!\n";
    body += "Best regards,\nBadminton Court Management System";
    
    // In a real implementation, this would send an actual email
    std::cout << "Email sent - Booking Created:\n" << body << std::endl;
}

void EmailNotificationObserver::onBookingCancelled(const Booking& booking) {
    if (!m_isEnabled) return;
    
    std::string subject = "Booking Cancellation - Badminton Court";
    std::string body = "Dear Customer,\n\n";
    body += "Your booking has been cancelled.\n\n";
    body += formatBookingDetails(booking);
    body += "\n\nIf you have any questions, please contact us.\n";
    body += "Best regards,\nBadminton Court Management System";
    
    std::cout << "Email sent - Booking Cancelled:\n" << body << std::endl;
}

void EmailNotificationObserver::onBookingModified(const Booking& oldBooking, const Booking& newBooking) {
    if (!m_isEnabled) return;
    
    std::string subject = "Booking Modified - Badminton Court";
    std::string body = "Dear Customer,\n\n";
    body += "Your booking has been modified.\n\n";
    body += "New booking details:\n";
    body += formatBookingDetails(newBooking);
    body += "\n\nThank you for using our service!\n";
    body += "Best regards,\nBadminton Court Management System";
    
    std::cout << "Email sent - Booking Modified:\n" << body << std::endl;
}

void EmailNotificationObserver::onBookingReminder(const Booking& booking) {
    if (!m_isEnabled) return;
    
    std::string subject = "Booking Reminder - Badminton Court";
    std::string body = "Dear Customer,\n\n";
    body += "This is a reminder for your upcoming booking.\n\n";
    body += formatBookingDetails(booking);
    body += "\n\nPlease arrive 15 minutes before your booking time.\n";
    body += "Best regards,\nBadminton Court Management System";
    
    std::cout << "Email sent - Booking Reminder:\n" << body << std::endl;
}

void EmailNotificationObserver::configure(const std::string& smtpServer, int smtpPort,
                                        const std::string& username, const std::string& password) {
    m_smtpServer = smtpServer;
    m_smtpPort = smtpPort;
    m_username = username;
    m_password = password;
}

bool EmailNotificationObserver::sendEmail(const std::string& to, const std::string& subject, 
                                        const std::string& body) const {
    // This is a placeholder implementation
    // In a real application, you would use a proper email library like libcurl or similar
    std::cout << "Sending email to: " << to << std::endl;
    std::cout << "Subject: " << subject << std::endl;
    std::cout << "Body: " << body << std::endl;
    return true;
}

std::string EmailNotificationObserver::formatBookingDetails(const Booking& booking) const {
    std::ostringstream oss;
    oss << "Booking ID: " << booking.getId() << "\n";
    oss << "Court ID: " << booking.getCourtId() << "\n";
    
    // Format dates and times
    std::time_t startTime = booking.getStartTime();
    std::time_t endTime = booking.getEndTime();
    
    oss << "Date: " << std::put_time(std::localtime(&startTime), "%Y-%m-%d") << "\n";
    oss << "Start Time: " << std::put_time(std::localtime(&startTime), "%H:%M") << "\n";
    oss << "End Time: " << std::put_time(std::localtime(&endTime), "%H:%M") << "\n";
    oss << "Total Amount: $" << std::fixed << std::setprecision(2) << booking.getTotalAmount() << "\n";
    oss << "Status: " << booking.getStatusString();
    
    if (!booking.getNotes().empty()) {
        oss << "\nNotes: " << booking.getNotes();
    }
    
    return oss.str();
}

// SMSNotificationObserver implementation
SMSNotificationObserver::SMSNotificationObserver(const std::string& apiKey, const std::string& apiUrl)
    : m_apiKey(apiKey), m_apiUrl(apiUrl), m_isEnabled(true) {
}

void SMSNotificationObserver::onBookingCreated(const Booking& booking) {
    if (!m_isEnabled) return;
    
    std::string message = "Booking confirmed for Court " + std::to_string(booking.getCourtId()) + 
                         ". Booking ID: " + std::to_string(booking.getId());
    
    std::cout << "SMS sent - Booking Created: " << message << std::endl;
}

void SMSNotificationObserver::onBookingCancelled(const Booking& booking) {
    if (!m_isEnabled) return;
    
    std::string message = "Booking " + std::to_string(booking.getId()) + " has been cancelled.";
    
    std::cout << "SMS sent - Booking Cancelled: " << message << std::endl;
}

void SMSNotificationObserver::onBookingModified(const Booking& oldBooking, const Booking& newBooking) {
    if (!m_isEnabled) return;
    
    std::string message = "Booking " + std::to_string(newBooking.getId()) + " has been modified.";
    
    std::cout << "SMS sent - Booking Modified: " << message << std::endl;
}

void SMSNotificationObserver::onBookingReminder(const Booking& booking) {
    if (!m_isEnabled) return;
    
    std::string message = "Reminder: Your booking " + std::to_string(booking.getId()) + 
                         " is coming up soon.";
    
    std::cout << "SMS sent - Booking Reminder: " << message << std::endl;
}

bool SMSNotificationObserver::sendSMS(const std::string& phoneNumber, const std::string& message) const {
    // Placeholder implementation for SMS sending
    std::cout << "Sending SMS to: " << phoneNumber << std::endl;
    std::cout << "Message: " << message << std::endl;
    return true;
}

// InAppNotificationObserver implementation
InAppNotificationObserver::InAppNotificationObserver(size_t maxNotifications)
    : m_maxNotifications(maxNotifications) {
}

void InAppNotificationObserver::onBookingCreated(const Booking& booking) {
    std::string message = "Booking created successfully for Court " + 
                         std::to_string(booking.getCourtId()) + 
                         " (ID: " + std::to_string(booking.getId()) + ")";
    addNotification(message);
}

void InAppNotificationObserver::onBookingCancelled(const Booking& booking) {
    std::string message = "Booking " + std::to_string(booking.getId()) + " has been cancelled";
    addNotification(message);
}

void InAppNotificationObserver::onBookingModified(const Booking& oldBooking, const Booking& newBooking) {
    std::string message = "Booking " + std::to_string(newBooking.getId()) + " has been modified";
    addNotification(message);
}

void InAppNotificationObserver::onBookingReminder(const Booking& booking) {
    std::string message = "Reminder: Your booking " + std::to_string(booking.getId()) + 
                         " is starting soon";
    addNotification(message);
}

void InAppNotificationObserver::addNotification(const std::string& message) {
    // Add timestamp to message
    std::time_t now = std::time(nullptr);
    std::ostringstream oss;
    oss << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] " << message;
    
    m_notifications.push_back(oss.str());
    
    // Remove old notifications if we exceed the limit
    if (m_notifications.size() > m_maxNotifications) {
        m_notifications.erase(m_notifications.begin());
    }
}
