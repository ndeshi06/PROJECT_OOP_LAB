#include "LoginFrame.h"
#include "BadmintonApp.h"
#include "AuthController.h"
#include "CourtController.h"
#include "BookingController.h"
#include "MainFrame.h"
#include "RegisterDialog.h"
#include "User.h"
#include <wx/msgdlg.h>
#include <wx/choice.h>
#include <wx/app.h>

// Event table
wxBEGIN_EVENT_TABLE(LoginFrame, wxFrame)
    EVT_BUTTON(wxID_OK, LoginFrame::OnLogin)
    EVT_BUTTON(wxID_NEW, LoginFrame::OnRegister)
    EVT_CLOSE(LoginFrame::OnClose)
    EVT_MENU(wxID_EXIT, LoginFrame::OnExit)
wxEND_EVENT_TABLE()

LoginFrame::LoginFrame(std::shared_ptr<AuthController> authController,
                       std::shared_ptr<CourtController> courtController,
                       std::shared_ptr<BookingController> bookingController)
    : wxFrame(nullptr, wxID_ANY, "Badminton Court Management - Login", 
              wxDefaultPosition, wxSize(400, 300)),
      m_authController(authController),
      m_courtController(courtController),
      m_bookingController(bookingController),
      m_mainPanel(nullptr),
      m_emailCtrl(nullptr),
      m_passwordCtrl(nullptr),
      m_roleChoice(nullptr),
      m_loginBtn(nullptr),
      m_registerBtn(nullptr),
      m_statusLabel(nullptr),
      m_mainSizer(nullptr),
      m_formSizer(nullptr),
      m_buttonSizer(nullptr) {
    
    CreateUI();
    CreateMenuBar();
    BindEvents();
    Center();
}

LoginFrame::~LoginFrame() {
    // Destructor - wxWidgets handles UI cleanup
}

void LoginFrame::CreateUI() {
    // Create main panel
    m_mainPanel = new wxPanel(this, wxID_ANY);
    
    // Create main sizer
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Create title
    wxStaticText* titleLabel = new wxStaticText(m_mainPanel, wxID_ANY, 
        "Badminton Court Management System", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    wxFont titleFont = titleLabel->GetFont();
    titleFont.SetPointSize(16);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleLabel->SetFont(titleFont);
    
    // Create form sizer
    m_formSizer = new wxFlexGridSizer(3, 2, 10, 10);
    m_formSizer->AddGrowableCol(1, 1);
    
    // Email field
    m_formSizer->Add(new wxStaticText(m_mainPanel, wxID_ANY, "Email:"), 
                     0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    m_emailCtrl = new wxTextCtrl(m_mainPanel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    m_formSizer->Add(m_emailCtrl, 1, wxEXPAND);
    
    // Password field
    m_formSizer->Add(new wxStaticText(m_mainPanel, wxID_ANY, "Password:"), 
                     0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    m_passwordCtrl = new wxTextCtrl(m_mainPanel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD);
    m_formSizer->Add(m_passwordCtrl, 1, wxEXPAND);
    
    // Role field
    m_formSizer->Add(new wxStaticText(m_mainPanel, wxID_ANY, "Role:"), 
                     0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    m_roleChoice = new wxChoice(m_mainPanel, wxID_ANY);
    m_roleChoice->Append("ADMIN");
    m_roleChoice->Append("STAFF");
    m_roleChoice->Append("CUSTOMER");
    m_roleChoice->SetSelection(2); // Default to CUSTOMER
    m_formSizer->Add(m_roleChoice, 1, wxEXPAND);
    
    // Create button sizer
    m_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Login button
    m_loginBtn = new wxButton(m_mainPanel, wxID_OK, "Login");
    m_loginBtn->SetDefault();
    
    // Register button
    m_registerBtn = new wxButton(m_mainPanel, wxID_NEW, "Register");
    
    m_buttonSizer->Add(m_loginBtn, 0, wxRIGHT, 5);
    m_buttonSizer->Add(m_registerBtn, 0, wxLEFT, 5);
    
    // Status label
    m_statusLabel = new wxStaticText(m_mainPanel, wxID_ANY, "");
    m_statusLabel->SetForegroundColour(*wxRED);
    
    // Add to main sizer
    m_mainSizer->Add(titleLabel, 0, wxALL | wxALIGN_CENTER, 20);
    m_mainSizer->Add(m_formSizer, 0, wxALL | wxEXPAND, 20);
    m_mainSizer->Add(m_buttonSizer, 0, wxALL | wxALIGN_CENTER, 10);
    m_mainSizer->Add(m_statusLabel, 0, wxALL | wxALIGN_CENTER, 5);
    
    m_mainPanel->SetSizer(m_mainSizer);
    
    // Set focus to email field
    m_emailCtrl->SetFocus();
}

void LoginFrame::CreateMenuBar() {
    wxMenuBar* menuBar = new wxMenuBar();
    
    // File menu
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_EXIT, "&Exit\tCtrl+Q", "Exit the application");
    
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);
}

void LoginFrame::BindEvents() {
    // Events are bound through event table
    Bind(wxEVT_CLOSE_WINDOW, &LoginFrame::OnClose, this);
}

bool LoginFrame::ValidateInput() {
    if (m_emailCtrl->GetValue().IsEmpty()) {
        ShowMessage("Please enter your email.", true);
        wxMessageBox("Please enter your email!", "Missing Information", wxOK | wxICON_WARNING);
        m_emailCtrl->SetFocus();
        return false;
    }
    
    if (m_passwordCtrl->GetValue().IsEmpty()) {
        ShowMessage("Please enter your password.", true);
        wxMessageBox("Please enter your password!", "Missing Information", wxOK | wxICON_WARNING);
        m_passwordCtrl->SetFocus();
        return false;
    }
    
    if (m_roleChoice->GetSelection() == wxNOT_FOUND) {
        ShowMessage("Please select a role.", true);
        wxMessageBox("Please select your role!", "Missing Information", wxOK | wxICON_WARNING);
        m_roleChoice->SetFocus();
        return false;
    }
    
    return true;
}

void LoginFrame::ShowMessage(const wxString& message, bool isError) {
    m_statusLabel->SetLabel(message);
    if (isError) {
        m_statusLabel->SetForegroundColour(*wxRED);
    } else {
        m_statusLabel->SetForegroundColour(*wxBLUE);
    }
    m_mainPanel->Layout();
}

void LoginFrame::OnLogin(wxCommandEvent& event) {
    if (!ValidateInput()) {
        return;
    }
    
    std::string email = m_emailCtrl->GetValue().ToStdString();
    std::string password = m_passwordCtrl->GetValue().ToStdString();
    
    // Get selected role
    wxString selectedRoleStr = m_roleChoice->GetStringSelection();
    UserRole selectedRole;
    if (selectedRoleStr == "ADMIN") {
        selectedRole = UserRole::ADMIN;
    } else if (selectedRoleStr == "STAFF") {
        selectedRole = UserRole::STAFF;
    } else {
        selectedRole = UserRole::CUSTOMER;
    }
    
    // Clear any previous user session first
    m_authController->logout();
    
    // Authenticate user
    bool loginSuccess = m_authController->login(email, password);
    if (loginSuccess) {
        // Get the current user after successful login
        auto user = m_authController->getCurrentUser();
        if (user) {
            // Check if the user's actual role matches the selected role
            if (user->getRole() != selectedRole) {
                m_authController->logout(); // Logout since role doesn't match
                ShowMessage("Login Failed", true);
                wxMessageBox("Incorrect role! Please select your correct role.", "Login Error", wxOK | wxICON_ERROR);
                ClearForm();
                return;
            }
            
            // Authentication successful
            ShowMessage("Login successful!", false);
            
            // Show success dialog
            wxMessageBox("Login successful!", "Success", wxOK | wxICON_INFORMATION);
            
            // Open main window
            OpenMainWindow();
        }
    } else {
        ShowMessage("Login Failed", true);
        wxMessageBox("Invalid email or password!", "Login Error", wxOK | wxICON_ERROR);
        ClearForm();
    }
}

void LoginFrame::OnRegister(wxCommandEvent& event) {
    RegisterDialog* registerDialog = new RegisterDialog(this);
    if (registerDialog->ShowModal() == wxID_OK) {
        // Get registration data
        std::string email = registerDialog->GetEmail();
        std::string password = registerDialog->GetPassword();
        std::string fullName = registerDialog->GetFullName();
        std::string phoneNumber = registerDialog->GetPhoneNumber();
        
        // Register user
        if (m_authController->registerUser(email, password, fullName, phoneNumber)) {
            ShowMessage("Registration successful! Please login.", false);
        } else {
            ShowMessage("Registration failed!", true);
        }
    }
    registerDialog->Destroy();
}

void LoginFrame::OnClose(wxCloseEvent& event) {
    // Check if this is a forced close
    if (!event.CanVeto()) {
        Destroy();
        return;
    }
    
    // Only exit app if no other main windows are available
    if (wxTopLevelWindows.GetCount() <= 1) {
        wxExit();
    } else {
        Destroy();
    }
}

void LoginFrame::OnExit(wxCommandEvent& event) {
    wxExit();
}

void LoginFrame::ClearForm() {
    m_passwordCtrl->SetValue("");
    m_statusLabel->SetLabel("");
    m_roleChoice->SetSelection(2); // Reset to CUSTOMER
    m_mainPanel->Layout();
    m_emailCtrl->SetFocus();
}

void LoginFrame::OpenMainWindow() {
    // Use app method to show main frame
    BadmintonApp* app = BadmintonApp::GetInstance();
    if (app) {
        app->ShowMainFrame();
    }
}
