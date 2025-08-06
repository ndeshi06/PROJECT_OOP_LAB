#pragma once
#include "Booking.h"
#include <string>
#include <memory>
#include <vector>

// Observer pattern for notifications
class NotificationObserver {
public:
    virtual ~NotificationObserver() = default;
    virtual void onBookingCreated(const Booking& booking) = 0;
    virtual void onBookingCancelled(const Booking& booking) = 0;
    virtual void onBookingModified(const Booking& oldBooking, const Booking& newBooking) = 0;
    virtual void onBookingReminder(const Booking& booking) = 0;
};

// Email notification observer
class EmailNotificationObserver : public NotificationObserver {
private:
    std::string m_smtpServer;
    int m_smtpPort;
    std::string m_username;
    std::string m_password;
    bool m_isEnabled;

public:
    EmailNotificationObserver(const std::string& smtpServer, int smtpPort,
                             const std::string& username, const std::string& password);

    void onBookingCreated(const Booking& booking) override;
    void onBookingCancelled(const Booking& booking) override;
    void onBookingModified(const Booking& oldBooking, const Booking& newBooking) override;
    void onBookingReminder(const Booking& booking) override;

    // Configuration
    void setEnabled(bool enabled) { m_isEnabled = enabled; }
    bool isEnabled() const { return m_isEnabled; }
    void configure(const std::string& smtpServer, int smtpPort,
                  const std::string& username, const std::string& password);

private:
    bool sendEmail(const std::string& to, const std::string& subject, 
                   const std::string& body) const;
    std::string formatBookingDetails(const Booking& booking) const;
};

// SMS notification observer (future extension)
class SMSNotificationObserver : public NotificationObserver {
private:
    std::string m_apiKey;
    std::string m_apiUrl;
    bool m_isEnabled;

public:
    SMSNotificationObserver(const std::string& apiKey, const std::string& apiUrl);

    void onBookingCreated(const Booking& booking) override;
    void onBookingCancelled(const Booking& booking) override;
    void onBookingModified(const Booking& oldBooking, const Booking& newBooking) override;
    void onBookingReminder(const Booking& booking) override;

    void setEnabled(bool enabled) { m_isEnabled = enabled; }
    bool isEnabled() const { return m_isEnabled; }

private:
    bool sendSMS(const std::string& phoneNumber, const std::string& message) const;
};

// In-app notification observer
class InAppNotificationObserver : public NotificationObserver {
private:
    std::vector<std::string> m_notifications;
    size_t m_maxNotifications;

public:
    InAppNotificationObserver(size_t maxNotifications = 100);

    void onBookingCreated(const Booking& booking) override;
    void onBookingCancelled(const Booking& booking) override;
    void onBookingModified(const Booking& oldBooking, const Booking& newBooking) override;
    void onBookingReminder(const Booking& booking) override;

    // Notification management
    std::vector<std::string> getNotifications() const { return m_notifications; }
    void clearNotifications() { m_notifications.clear(); }
    size_t getNotificationCount() const { return m_notifications.size(); }

private:
    void addNotification(const std::string& message);
};
