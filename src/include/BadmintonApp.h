#pragma once
#include <wx/wx.h>

class AuthController;
class CourtController;
class BookingController;
class EmailNotificationObserver;
class InAppNotificationObserver;

class BadmintonApp : public wxApp
{
private:
    AuthController *m_authController;
    CourtController *m_courtController;
    BookingController *m_bookingController;
    EmailNotificationObserver *m_emailObserver;
    InAppNotificationObserver *m_inAppObserver;

public:
    virtual bool OnInit();
    virtual int OnExit();

    // Destructor for cleanup
    ~BadmintonApp();

    // Static methods to access controllers
    static BadmintonApp *GetInstance() { return dynamic_cast<BadmintonApp *>(wxTheApp); }
    AuthController *GetAuthController() { return m_authController; }
    CourtController *GetCourtController() { return m_courtController; }
    BookingController *GetBookingController() { return m_bookingController; }

    // Application flow control
    void ShowLoginFrame();
    void ShowMainFrame();
    void OnLogout();

private:
    void InitializeControllers();
    void SetupNotifications();
    void LoadInitialData();
};
