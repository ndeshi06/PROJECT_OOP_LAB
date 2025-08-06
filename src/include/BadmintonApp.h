#pragma once
#include <wx/wx.h>
#include <memory>

class AuthController;
class CourtController;
class BookingController;
class EmailNotificationObserver;
class InAppNotificationObserver;

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
    
    // Static methods to access controllers
    static BadmintonApp* GetInstance() { return dynamic_cast<BadmintonApp*>(wxTheApp); }
    std::shared_ptr<AuthController> GetAuthController() { return m_authController; }
    std::shared_ptr<CourtController> GetCourtController() { return m_courtController; }
    std::shared_ptr<BookingController> GetBookingController() { return m_bookingController; }
    
    // Application flow control
    void ShowLoginFrame();
    void ShowMainFrame();
    void OnLogout();

private:
    void InitializeControllers();
    void SetupNotifications();
    void LoadInitialData();
};
