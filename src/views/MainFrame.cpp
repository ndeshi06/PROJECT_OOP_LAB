#include "MainFrame.h"
#include "CourtManagementPanel.h"
#include "BookingPanel.h"
#include "StatisticsPanel.h"
#include "../include/AuthController.h"
#include "../include/CourtController.h"
#include "../include/BookingController.h"
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>

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
    : wxFrame(nullptr, wxID_ANY, "Quản lý sân cầu lông", 
              wxDefaultPosition, wxSize(1200, 800)),
      m_authController(authController),
      m_courtController(courtController),
      m_bookingController(bookingController),
      m_selectedCourtId(-1),
      m_selectedBookingId(-1)
{
    SetIcon(wxIcon(wxIconLocation()));
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
    fileMenu->Append(wxID_EXIT, "Thoát\tCtrl+Q", "Thoát khỏi ứng dụng");
    
    // Help menu
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "About\tF1", "Information about the application");
    
    menuBar->Append(fileMenu, "Tệp");
    menuBar->Append(helpMenu, "Trợ giúp");
    
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
    
    // Show current user
    if (m_authController && m_authController->getCurrentUser()) {
        wxString userInfo = "User: " + m_authController->getCurrentUser()->getUsername();
        SetStatusText(userInfo, 1);
    }
}

void MainFrame::CreateNotebook()
{
    m_notebook = new wxNotebook(this, wxID_ANY);
    
    // Court Management Panel  
    m_courtPanel = new CourtManagementPanel(m_notebook, m_courtController, m_authController);
    m_notebook->AddPage(m_courtPanel, "Court Management", true);
    
    // Booking Panel
    m_bookingPanel = new BookingPanel(m_notebook, m_bookingController, m_courtController, m_authController);
    m_notebook->AddPage(m_bookingPanel, "Booking");
    
    // Statistics Panel
    m_statisticsPanel = new StatisticsPanel(m_notebook, m_bookingController, m_courtController, m_authController);
    m_notebook->AddPage(m_statisticsPanel, "Statistics");
}

void MainFrame::BindEvents()
{
    // Events are handled by the event table
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("Hệ thống quản lý sân cầu lông\n"
                 "Phiên bản 1.0\n"
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
        if (m_authController) {
            m_authController->logout();
        }
        Close(true);
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    // Save any pending changes before closing
    event.Skip();
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
        wxString userInfo = "User: " + m_authController->getCurrentUser()->getUsername();
        SetStatusText(userInfo, 1);
    }
}

void MainFrame::UpdateStatusBar()
{
    SetStatusText("Sẵn sàng", 0);
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
