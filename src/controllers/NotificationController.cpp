#include "NotificationController.h"
#include <iostream>

NotificationController::NotificationController() 
    : m_emailEnabled(false), m_smsEnabled(false), m_inAppEnabled(true) {
}

NotificationController::~NotificationController() {
}

void NotificationController::sendNotification(const std::string& message) {
    if (m_inAppEnabled) {
        addNotification(message);
    }
    
    if (m_emailEnabled) {
        std::cout << "Email notification: " << message << std::endl;
    }
    
    if (m_smsEnabled) {
        std::cout << "SMS notification: " << message << std::endl;
    }
}

void NotificationController::addNotification(const std::string& message) {
    m_notifications.push_back(message);
    
    // Keep only last 100 notifications
    if (m_notifications.size() > 100) {
        m_notifications.erase(m_notifications.begin());
    }
}

std::vector<std::string> NotificationController::getNotifications() const {
    return m_notifications;
}

void NotificationController::clearNotifications() {
    m_notifications.clear();
}

void NotificationController::setEmailEnabled(bool enabled) {
    m_emailEnabled = enabled;
}

void NotificationController::setSMSEnabled(bool enabled) {
    m_smsEnabled = enabled;
}

void NotificationController::setInAppEnabled(bool enabled) {
    m_inAppEnabled = enabled;
}

void NotificationController::configureEmail(const std::string& smtpServer, int port,
                                          const std::string& username, const std::string& password) {
    // Placeholder for email configuration
    std::cout << "Email configured: " << smtpServer << ":" << port << std::endl;
}
