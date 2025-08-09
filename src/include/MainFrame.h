#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>

class AuthController;
class CourtController;
class BookingController;
class CourtManagementPanel;
class BookingPanel;
class StatisticsPanel;
class UserManagementPanel;
class AdminPanel;
class LoginFrame;

class MainFrame : public wxFrame
{
private:
    AuthController *m_authController;
    CourtController *m_courtController;
    BookingController *m_bookingController;

    // UI components
    wxMenuBar *m_menuBar;
    wxNotebook *m_notebook;
    wxStatusBar *m_statusBar;

    // Panels
    CourtManagementPanel *m_courtPanel;
    BookingPanel *m_bookingPanel;
    StatisticsPanel *m_statisticsPanel;
    UserManagementPanel *m_userPanel;
    AdminPanel *m_adminPanel;

    // Current user info
    wxStaticText *m_userLabel;

    // State variables
    int m_selectedCourtId;
    int m_selectedBookingId;

public:
    MainFrame(AuthController *authController,
              CourtController *courtController,
              BookingController *bookingController);
    ~MainFrame();

    // Event handlers
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnLogout(wxCommandEvent &event);
    void OnRefresh(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);

    // Panel management
    void OnPageChanged(wxBookCtrlEvent &event);
    void RefreshAllPanels();
    void UpdateUserInterface();

private:
    void CreateUI();
    void CreateMenuBar();
    void CreateNotebook();
    void CreateStatusBar();
    void SetupPanels();
    void BindEvents();
    void UpdateStatusBar();
    void ShowUserInfo();

    DECLARE_EVENT_TABLE()
};

// Event IDs
enum
{
    ID_LOGOUT = 2000,
    ID_REFRESH,
    ID_ABOUT,
    ID_NOTEBOOK
};
