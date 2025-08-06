#include <wx/wx.h>
#include "LoginFrame.h"
#include "AuthController.h"
#include "CourtController.h"
#include "BookingController.h"
#include "BookingManager.h"
#include "NotificationObserver.h"
#include <memory>

class BadmintonApp : public wxApp {
private:
    std::shared_ptr<AuthController> m_authController;
    std::shared_ptr<CourtController> m_courtController;
    std::shared_ptr<BookingController> m_bookingController;
    std::shared_ptr<EmailNotificationObserver> m_emailObserver;
    std::shared_ptr<InAppNotificationObserver> m_inAppObserver;

public:
    virtual bool OnInit();
    virtual int OnExit();

private:
    void InitializeControllers();
    void SetupNotifications();
    void LoadInitialData();
};

wxIMPLEMENT_APP(BadmintonApp);

bool BadmintonApp::OnInit() {
    // Set UTF-8 encoding for proper Vietnamese character display
    wxLocale::GetSystemLanguage();
    
    // Initialize controllers
    InitializeControllers();
    
    // Setup notification system
    SetupNotifications();
    
    // Load initial data
    LoadInitialData();
    
    // Create and show login frame
    LoginFrame* loginFrame = new LoginFrame(m_authController);
    loginFrame->Show(true);
    
    return true;
}

int BadmintonApp::OnExit() {
    // Save data before exit
    if (m_authController) {
        m_authController->saveUsers();
    }
    
    if (m_courtController) {
        m_courtController->saveCourts();
    }
    
    // Save bookings through BookingManager
    BookingManager::getInstance().saveBookings();
    
    return wxApp::OnExit();
}

void BadmintonApp::InitializeControllers() {
    m_authController = std::make_shared<AuthController>();
    m_courtController = std::make_shared<CourtController>();
    m_bookingController = std::make_shared<BookingController>();
}

void BadmintonApp::SetupNotifications() {
    // Create notification observers
    m_emailObserver = std::make_shared<EmailNotificationObserver>(
        "smtp.gmail.com", 587, "your-email@gmail.com", "your-password");
    
    m_inAppObserver = std::make_shared<InAppNotificationObserver>(100);
    
    // Add observers to BookingManager
    BookingManager& bookingManager = BookingManager::getInstance();
    bookingManager.addObserver(m_emailObserver);
    bookingManager.addObserver(m_inAppObserver);
}

void BadmintonApp::LoadInitialData() {
    // Load data from files/database
    m_authController->loadUsers();
    m_courtController->loadCourts();
    BookingManager::getInstance().loadBookings();
    
    // Create default admin user if no users exist
    if (m_authController->getAllUsers().empty()) {
        m_authController->registerUser(
            "admin@badminton.com", 
            "admin123", 
            "System Administrator", 
            "0123456789", 
            UserRole::ADMIN
        );
    }
    
    // Create default courts if none exist
    if (m_courtController->getAllCourts().empty()) {
        m_courtController->addCourt("Court 1", "Standard badminton court", 50000.0);
        m_courtController->addCourt("Court 2", "Premium badminton court", 75000.0);
        m_courtController->addCourt("Court 3", "VIP badminton court", 100000.0);
    }
}
