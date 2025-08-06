#pragma once
#include <string>
#include <vector>
#include <memory>

class NotificationController {
private:
    std::vector<std::string> m_notifications;

public:
    NotificationController();
    ~NotificationController();

    // Notification management
    void sendNotification(const std::string& message);
    void addNotification(const std::string& message);
    std::vector<std::string> getNotifications() const;
    void clearNotifications();

    // Configuration
    void setEmailEnabled(bool enabled);
    void setSMSEnabled(bool enabled);
    void setInAppEnabled(bool enabled);

    // Email configuration
    void configureEmail(const std::string& smtpServer, int port,
                       const std::string& username, const std::string& password);

private:
    bool m_emailEnabled;
    bool m_smsEnabled;
    bool m_inAppEnabled;
};
