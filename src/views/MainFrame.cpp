#include "MainFrame.h"
#include "BadmintonApp.h"
#include "CourtManagementPanel.h"
#include "BookingPanel.h"
#include "StatisticsPanel.h"
#include "UserManagementPanel.h"
#include "LoginFrame.h"
#include "../include/AuthController.h"
#include "../include/CourtController.h"
#include "../include/BookingController.h"
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/toplevel.h>

// Event table
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(ID_LOGOUT, MainFrame::OnLogout)
    EVT_CLOSE(MainFrame::OnClose)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(std::shared_ptr<AuthController> authController,
                     std::shared_ptr<CourtController> courtController,
                     std::shared_ptr<BookingController> bookingController)
    : wxFrame(nullptr, wxID_ANY, "Badminton Court Management System", 
              wxDefaultPosition, wxSize(1200, 800)),
      m_authController(authController),
      m_courtController(courtController),
      m_bookingController(bookingController),
      m_selectedCourtId(-1),
      m_selectedBookingId(-1)
{
    Center();
    
    CreateUI();
    BindEvents();
    
    // Welcome message
    SetStatusText("Welcome to the Badminton Court Management System!");
}

MainFrame::~MainFrame()
{
}

void MainFrame::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar;
    
    // File menu
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(ID_LOGOUT, "Logout\tCtrl+L", "Logout from the system");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "Exit\tCtrl+Q", "Exit the application");

    // Help menu
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "About\tF1", "Information about the application");

    menuBar->Append(fileMenu, "File");
    menuBar->Append(helpMenu, "Help");

    SetMenuBar(menuBar);
}

void MainFrame::CreateUI()
{
    CreateMenuBar();
    CreateStatusBar();
    CreateNotebook();
    
    // Set the notebook as the main window content
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_notebook, 1, wxEXPAND);
    SetSizer(sizer);
}

void MainFrame::CreateStatusBar()
{
    m_statusBar = wxFrame::CreateStatusBar(2);
    SetStatusText("Ready", 0);
    
    // Show current user info
    if (m_authController && m_authController->getCurrentUser()) {
        auto user = m_authController->getCurrentUser();
        wxString roleStr = "UNKNOWN";
        switch(user->getRole()) {
            case UserRole::ADMIN: roleStr = "ADMIN"; break;
            case UserRole::STAFF: roleStr = "STAFF"; break;
            case UserRole::CUSTOMER: roleStr = "CUSTOMER"; break;
        }
        wxString userInfo = wxString::Format("User: %s (%s)", 
                                           user->getUsername(), roleStr);
        SetStatusText(userInfo, 1);
    }
}

void MainFrame::CreateNotebook()
{
    m_notebook = new wxNotebook(this, wxID_ANY);
    
    // Get current user role
    UserRole currentRole = UserRole::CUSTOMER; // Default
    if (m_authController && m_authController->getCurrentUser()) {
        currentRole = m_authController->getCurrentUser()->getRole();
    }
    
    // For ADMIN: Show management tabs first, no booking tab needed
    if (currentRole == UserRole::ADMIN) {
        // User Management Panel - Primary tab for ADMIN
        m_userPanel = new UserManagementPanel(m_notebook, m_authController);
        m_notebook->AddPage(m_userPanel, "User Management", true); // Set as default tab
        
        // Court Management Panel
        m_courtPanel = new CourtManagementPanel(m_notebook, m_courtController, m_authController);
        m_notebook->AddPage(m_courtPanel, "Court Management");
        
        // Statistics Panel
        m_statisticsPanel = new StatisticsPanel(m_notebook, m_bookingController, m_courtController, m_authController);
        m_notebook->AddPage(m_statisticsPanel, "Statistics");
        
        // Optional: Add booking panel for admin if needed (commented out by default)
        // m_bookingPanel = new BookingPanel(m_notebook, m_bookingController, m_courtController, m_authController);
        // m_notebook->AddPage(m_bookingPanel, "Booking");
    }
    // For STAFF: Show court management and booking
    else if (currentRole == UserRole::STAFF) {
        // Court Management Panel - Primary tab for STAFF
        m_courtPanel = new CourtManagementPanel(m_notebook, m_courtController, m_authController);
        m_notebook->AddPage(m_courtPanel, "Court Management", true); // Set as default tab
        
        // Booking Panel
        m_bookingPanel = new BookingPanel(m_notebook, m_bookingController, m_courtController, m_authController);
        m_notebook->AddPage(m_bookingPanel, "Booking");
        
        // Statistics Panel
        m_statisticsPanel = new StatisticsPanel(m_notebook, m_bookingController, m_courtController, m_authController);
        m_notebook->AddPage(m_statisticsPanel, "Statistics");
    }
    // For CUSTOMER: Only booking
    else {
        // Booking Panel - Only tab for CUSTOMER
        m_bookingPanel = new BookingPanel(m_notebook, m_bookingController, m_courtController, m_authController);
        m_notebook->AddPage(m_bookingPanel, "Booking", true);
    }
}

void MainFrame::BindEvents()
{
    // Events are handled by the event table
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxExit();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("Badminton Management System\n"
                 "Version 1.0\n"
                 "Developed with C++ and wxWidgets\n\n"
                 "Features:\n"
                 "- Badminton court management\n"
                 "- Online court booking\n"
                 "- Revenue statistics\n"
                 "- User management",
                 "About",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MainFrame::OnLogout(wxCommandEvent& event)
{
    int result = wxMessageBox("Do you want to logout?",
                             "Confirm Logout",
                             wxYES_NO | wxICON_QUESTION,
                             this);
    
    if (result == wxYES) {
        // Use app method to handle logout
        BadmintonApp* app = BadmintonApp::GetInstance();
        if (app) {
            app->OnLogout();
        }
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    // Only exit if user explicitly wants to close the app
    if (event.CanVeto()) {
        int result = wxMessageBox("Do you want to exit the application?",
                                 "Confirm Exit",
                                 wxYES_NO | wxICON_QUESTION,
                                 this);
        if (result == wxNO) {
            event.Veto();
            return;
        }
    }
    
    // Exit the application safely
    Destroy();
    // Only exit main loop if this is the last window
    if (wxTopLevelWindows.GetCount() <= 1) {
        wxTheApp->ExitMainLoop();
    }
}

void MainFrame::RefreshAllPanels()
{
    if (m_courtPanel) {
        m_courtPanel->RefreshCourtList();
    }
    
    if (m_bookingPanel) {
        m_bookingPanel->RefreshData();
    }
    
    if (m_statisticsPanel) {
        m_statisticsPanel->RefreshData();
    }
    
    SetStatusText("Data has been updated", 0);
}

void MainFrame::UpdateUserInterface()
{
    if (m_authController && m_authController->getCurrentUser()) {
        auto user = m_authController->getCurrentUser();
        wxString roleStr = "UNKNOWN";
        switch(user->getRole()) {
            case UserRole::ADMIN: roleStr = "ADMIN"; break;
            case UserRole::STAFF: roleStr = "STAFF"; break;
            case UserRole::CUSTOMER: roleStr = "CUSTOMER"; break;
        }
        wxString userInfo = wxString::Format("User: %s (%s)", 
                                           user->getUsername(), roleStr);
        SetStatusText(userInfo, 1);
    }
}

void MainFrame::UpdateStatusBar()
{
    SetStatusText("Ready", 0);
}

void MainFrame::ShowUserInfo()
{
    UpdateUserInterface();
}

void MainFrame::OnPageChanged(wxBookCtrlEvent& event)
{
    // Handle notebook page changes if needed
    event.Skip();
}

void MainFrame::OnRefresh(wxCommandEvent& event)
{
    RefreshAllPanels();
    wxMessageBox("All data refreshed!", "Information", wxOK | wxICON_INFORMATION);
}
