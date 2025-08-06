#include "LoginFrame.h"
#include "../include/AuthController.h"
#include "../include/MainFrame.h"
#include "../include/CourtController.h"
#include "../include/BookingController.h"
#include <wx/msgdlg.h>

wxBEGIN_EVENT_TABLE(LoginFrame, wxFrame)
    EVT_BUTTON(ID_LOGIN, LoginFrame::OnLogin)
    EVT_BUTTON(ID_REGISTER, LoginFrame::OnRegister)
    EVT_CLOSE(LoginFrame::OnClose)
    EVT_MENU(wxID_EXIT, LoginFrame::OnExit)
wxEND_EVENT_TABLE()

LoginFrame::LoginFrame(std::shared_ptr<AuthController> authController)
    : wxFrame(nullptr, wxID_ANY, "Batminton Court Management System - Login", 
              wxDefaultPosition, wxSize(400, 300)),
      m_authController(authController)
{
    CreateUI();
    CreateMenuBar();
    BindEvents();
    
    Center();
    m_emailCtrl->SetFocus();
}

LoginFrame::~LoginFrame()
{
}

void LoginFrame::CreateUI()
{
    // Main panel
    m_mainPanel = new wxPanel(this, wxID_ANY);
    m_mainPanel->SetBackgroundColour(wxColour(240, 240, 240));
    
    // Title
    wxStaticText* titleLabel = new wxStaticText(m_mainPanel, wxID_ANY, 
                                               "BADMINTON COURT MANAGEMENT SYSTEM",
                                               wxDefaultPosition, wxDefaultSize, 
                                               wxALIGN_CENTER);
    wxFont titleFont = titleLabel->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleLabel->SetFont(titleFont);
    
    // Form controls
    wxStaticText* emailLabel = new wxStaticText(m_mainPanel, wxID_ANY, "Email:");
    m_emailCtrl = new wxTextCtrl(m_mainPanel, wxID_ANY, "", 
                                wxDefaultPosition, wxSize(250, -1));
    
    wxStaticText* passwordLabel = new wxStaticText(m_mainPanel, wxID_ANY, "Password:");
    m_passwordCtrl = new wxTextCtrl(m_mainPanel, wxID_ANY, "", 
                                   wxDefaultPosition, wxSize(250, -1), 
                                   wxTE_PASSWORD);
    
    wxStaticText* roleLabel = new wxStaticText(m_mainPanel, wxID_ANY, "Role:");
    m_roleChoice = new wxChoice(m_mainPanel, ID_ROLE_CHOICE);
    m_roleChoice->Append("Admin");
    m_roleChoice->Append("Staff");
    m_roleChoice->Append("Customer");
    m_roleChoice->SetSelection(2); // Default to Customer
    
    // Buttons
    m_loginBtn = new wxButton(m_mainPanel, ID_LOGIN, "Login");
    m_registerBtn = new wxButton(m_mainPanel, ID_REGISTER, "Register");
    
    // Status label
    m_statusLabel = new wxStaticText(m_mainPanel, wxID_ANY, "", 
                                    wxDefaultPosition, wxDefaultSize, 
                                    wxALIGN_CENTER);
    m_statusLabel->SetForegroundColour(wxColour(255, 0, 0));
    
    // Layout
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    m_formSizer = new wxFlexGridSizer(3, 2, 10, 10);
    m_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Add to form sizer
    m_formSizer->Add(emailLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    m_formSizer->Add(m_emailCtrl, 0, wxEXPAND);
    m_formSizer->Add(passwordLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    m_formSizer->Add(m_passwordCtrl, 0, wxEXPAND);
    m_formSizer->Add(roleLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    m_formSizer->Add(m_roleChoice, 0, wxEXPAND);
    
    m_formSizer->AddGrowableCol(1);
    
    // Add to button sizer
    m_buttonSizer->Add(m_loginBtn, 0, wxRIGHT, 10);
    m_buttonSizer->Add(m_registerBtn, 0);
    
    // Add to main sizer
    m_mainSizer->Add(titleLabel, 0, wxALL | wxALIGN_CENTER, 20);
    m_mainSizer->Add(m_formSizer, 0, wxALL | wxALIGN_CENTER, 20);
    m_mainSizer->Add(m_buttonSizer, 0, wxALL | wxALIGN_CENTER, 10);
    m_mainSizer->Add(m_statusLabel, 0, wxALL | wxALIGN_CENTER, 10);
    
    m_mainPanel->SetSizer(m_mainSizer);
}

void LoginFrame::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar;
    
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "Exit\tCtrl+Q", "Exit the application");
    
    menuBar->Append(fileMenu, "Files");
    
    SetMenuBar(menuBar);
}

void LoginFrame::BindEvents()
{
    // Events are bound through the event table
}

void LoginFrame::OnLogin(wxCommandEvent& event)
{
    if (!ValidateInput()) {
        return;
    }
    
    try {
        std::string email = m_emailCtrl->GetValue().ToStdString();
        std::string password = m_passwordCtrl->GetValue().ToStdString();
        
        if (m_authController->login(email, password)) {
            ShowMessage("Login successful!");

            // Hide login window and open main window
            Hide();
            OpenMainWindow();
        } else {
            ShowMessage("Invalid email or password!", true);
        }
    } catch (const std::exception& e) {
        wxString errorMsg = wxString::Format("Login error: %s", e.what());
        ShowMessage(errorMsg, true);
    }
}

void LoginFrame::OnRegister(wxCommandEvent& event)
{
    if (!ValidateInput()) {
        return;
    }
    
    try {
        std::string email = m_emailCtrl->GetValue().ToStdString();
        std::string password = m_passwordCtrl->GetValue().ToStdString();
        std::string roleStr;
        
        switch (m_roleChoice->GetSelection()) {
            case 0: roleStr = "ADMIN"; break;
            case 1: roleStr = "STAFF"; break;
            case 2: roleStr = "CUSTOMER"; break;
            default: roleStr = "CUSTOMER"; break;
        }
        
        if (m_authController->registerUser(email, password, email, roleStr)) {
            ShowMessage("Registration successful! Please log in.");
            ClearForm();
        } else {
            ShowMessage("Email already exists!", true);
        }
    } catch (const std::exception& e) {
        wxString errorMsg = wxString::Format("Registration error: %s", e.what());
        ShowMessage(errorMsg, true);
    }
}

void LoginFrame::OnClose(wxCloseEvent& event)
{
    Destroy();
}

void LoginFrame::OnExit(wxCommandEvent& event)
{
    Close();
}

void LoginFrame::ClearForm()
{
    m_emailCtrl->Clear();
    m_passwordCtrl->Clear();
    m_roleChoice->SetSelection(2);
    m_statusLabel->SetLabel("");
    m_emailCtrl->SetFocus();
}

bool LoginFrame::ValidateInput()
{
    wxString email = m_emailCtrl->GetValue();
    wxString password = m_passwordCtrl->GetValue();
    
    if (email.IsEmpty()) {
        ShowMessage("Please enter your email!", true);
        m_emailCtrl->SetFocus();
        return false;
    }
    
    if (!email.Contains("@")) {
        ShowMessage("Invalid Email!", true);
        m_emailCtrl->SetFocus();
        return false;
    }
    
    if (password.IsEmpty()) {
        ShowMessage("Please enter your password!", true);
        m_passwordCtrl->SetFocus();
        return false;
    }
    
    if (password.Length() < 6) {
        ShowMessage("Password must be at least 6 characters long!", true);
        m_passwordCtrl->SetFocus();
        return false;
    }
    
    return true;
}

void LoginFrame::ShowMessage(const wxString& message, bool isError)
{
    m_statusLabel->SetLabel(message);
    if (isError) {
        m_statusLabel->SetForegroundColour(wxColour(255, 0, 0));
    } else {
        m_statusLabel->SetForegroundColour(wxColour(0, 128, 0));
    }
    m_statusLabel->Refresh();
}

void LoginFrame::OpenMainWindow()
{
    try {
        // Create controllers
        auto courtController = std::make_shared<CourtController>();
        auto bookingController = std::make_shared<BookingController>();
        
        // Create and show main window
        MainFrame* mainFrame = new MainFrame(m_authController, courtController, bookingController);
        mainFrame->Show();
        
        // Close login window
        Close();
    } catch (const std::exception& e) {
        wxString errorMsg = wxString::Format("Error opening main window: %s", e.what());
        ShowMessage(errorMsg, true);
        Show(); // Show login window again if error
    }
} 
