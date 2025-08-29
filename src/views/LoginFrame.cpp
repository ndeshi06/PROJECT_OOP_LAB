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

LoginFrame::LoginFrame(AuthController* authController,
                        CourtController* courtController,
                        BookingController* bookingController)
: wxFrame(nullptr, wxID_ANY, "Badminton Court Management - Login",
            wxDefaultPosition, wxSize(600, 500)),
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
    m_buttonSizer(nullptr)
{

    CreateUI();
    CreateMenuBar();
    BindEvents();
    
    // Force multiple layout passes to ensure everything is properly calculated
    Layout();
    m_mainPanel->Layout();
    m_mainSizer->Layout();
    
    // Set standard login window size
    SetSize(wxSize(600, 450));
    SetMinSize(wxSize(500, 400));
    
    // Center the window
    Center();
}

LoginFrame::~LoginFrame() {}

void LoginFrame::CreateUI()
{
    // Create main panel
    m_mainPanel = new wxPanel(this, wxID_ANY);

    // Create main sizer
    m_mainSizer = new wxBoxSizer(wxVERTICAL);

    // Add image handler
    if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG)) {
        wxImage::AddHandler(new wxPNGHandler);
    }

    // Try to load images from multiple possible locations
    wxString imagePaths[] = {
        "images/normal.png",
        "../images/normal.png", 
        "../../images/normal.png",
        "src/images/normal.png"
    };
    
    bool normalImageLoaded = false;
    bool focusImageLoaded = false;
    
    for (const wxString& path : imagePaths) {
        wxString normalPath = path;
        wxString focusPath = path;
        focusPath.Replace("normal.png", "focus.png");
        
        if (wxFileExists(normalPath) && wxFileExists(focusPath)) {
            normalImageLoaded = m_imgNormalOriginal.LoadFile(normalPath, wxBITMAP_TYPE_PNG);
            focusImageLoaded = m_imgFocusOriginal.LoadFile(focusPath, wxBITMAP_TYPE_PNG);
            if (normalImageLoaded && focusImageLoaded) {
                break;
            }
        }
    }

    // Only create image controls if images loaded successfully
    if (normalImageLoaded && focusImageLoaded) {
        m_imgNormal = new wxStaticBitmap(m_mainPanel, wxID_ANY,
            wxBitmap(m_imgNormalOriginal), wxDefaultPosition, wxDefaultSize);
        m_imgFocus = new wxStaticBitmap(m_mainPanel, wxID_ANY,
            wxBitmap(m_imgFocusOriginal), wxDefaultPosition, wxDefaultSize);

        m_imgFocus->Hide();

        // Thêm ảnh vào sizer, căn giữa
        m_mainSizer->Add(m_imgNormal, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
        m_mainSizer->Add(m_imgFocus, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    } else {
        // If images can't be loaded, set pointers to null
        m_imgNormal = nullptr;
        m_imgFocus = nullptr;
    }

    // Create title
    wxStaticText *titleLabel = new wxStaticText(m_mainPanel, wxID_ANY,
                                                "Badminton Court Management System",
                                                wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    wxFont titleFont = titleLabel->GetFont();
    titleFont.SetPointSize(16);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleLabel->SetFont(titleFont);

    m_mainSizer->Add(titleLabel, 0, wxALL | wxALIGN_CENTER, 10);

    // Create form sizer (3 hàng, 2 cột)
    m_formSizer = new wxFlexGridSizer(3, 2, 10, 10);
    m_formSizer->AddGrowableCol(1, 1);

    // Email field
    m_formSizer->Add(new wxStaticText(m_mainPanel, wxID_ANY, "Email:"),
                     0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    m_emailCtrl = new wxTextCtrl(m_mainPanel, wxID_ANY, "", wxDefaultPosition, wxSize(220, -1));
    m_formSizer->Add(m_emailCtrl, 1, wxEXPAND);

    // Password field
    m_formSizer->Add(new wxStaticText(m_mainPanel, wxID_ANY, "Password:"),
                     0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    m_passwordCtrl = new wxTextCtrl(m_mainPanel, wxID_ANY, "", wxDefaultPosition, wxSize(220, -1), wxTE_PASSWORD);
    m_formSizer->Add(m_passwordCtrl, 1, wxEXPAND);

    // Role field
    m_formSizer->Add(new wxStaticText(m_mainPanel, wxID_ANY, "Role:"),
                     0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    m_roleChoice = new wxChoice(m_mainPanel, wxID_ANY);
    m_roleChoice->Append("ADMIN");
    m_roleChoice->Append("STAFF");
    m_roleChoice->Append("CUSTOMER");
    m_roleChoice->SetSelection(2); // Default CUSTOMER
    m_formSizer->Add(m_roleChoice, 1, wxEXPAND);

    m_mainSizer->Add(m_formSizer, 0, wxALL | wxEXPAND, 15);

    // Create button sizer
    m_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_loginBtn = new wxButton(m_mainPanel, wxID_OK, "Login");
    m_loginBtn->SetDefault();
    m_registerBtn = new wxButton(m_mainPanel, wxID_NEW, "Register");

    m_buttonSizer->Add(m_loginBtn, 0, wxRIGHT, 8);
    m_buttonSizer->Add(m_registerBtn, 0, wxLEFT, 8);

    m_mainSizer->Add(m_buttonSizer, 0, wxALIGN_CENTER | wxTOP, 10);

    // Status label
    m_statusLabel = new wxStaticText(m_mainPanel, wxID_ANY, "");
    m_statusLabel->SetForegroundColour(*wxRED);
    m_mainSizer->Add(m_statusLabel, 0, wxALIGN_CENTER | wxALL, 5);

    // Set the sizer for the main panel
    m_mainPanel->SetSizer(m_mainSizer);
    
    // Create a sizer for the frame itself to properly manage the panel
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(m_mainPanel, 1, wxEXPAND);
    SetSizer(frameSizer);
    
    // Force initial layout calculation
    m_mainSizer->Layout();
    m_mainPanel->Layout();
    Layout();

    // Set focus to email field
    m_emailCtrl->SetFocus();
}

void LoginFrame::CreateMenuBar()
{
    wxMenuBar *menuBar = new wxMenuBar();

    // File menu
    wxMenu *fileMenu = new wxMenu();
    fileMenu->Append(wxID_EXIT, "&Exit\tCtrl+Q", "Exit the application");

    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);
}

void LoginFrame::BindEvents()
{
    // Events are bound through event table
    Bind(wxEVT_CLOSE_WINDOW, &LoginFrame::OnClose, this);
    Bind(wxEVT_SIZE, &LoginFrame::OnResize, this);
    m_passwordCtrl->Bind(wxEVT_SET_FOCUS, &LoginFrame::OnPasswordFocus, this);
    m_passwordCtrl->Bind(wxEVT_KILL_FOCUS, &LoginFrame::OnPasswordKillFocus, this);
}

bool LoginFrame::ValidateInput()
{
    if (m_emailCtrl->GetValue().IsEmpty())
    {
        ShowMessage("Please enter your email.", true);
        wxMessageBox("Please enter your email!", "Missing Information", wxOK | wxICON_WARNING);
        m_emailCtrl->SetFocus();
        return false;
    }

    if (m_passwordCtrl->GetValue().IsEmpty())
    {
        ShowMessage("Please enter your password.", true);
        wxMessageBox("Please enter your password!", "Missing Information", wxOK | wxICON_WARNING);
        m_passwordCtrl->SetFocus();
        return false;
    }

    if (m_roleChoice->GetSelection() == wxNOT_FOUND)
    {
        ShowMessage("Please select a role.", true);
        wxMessageBox("Please select your role!", "Missing Information", wxOK | wxICON_WARNING);
        m_roleChoice->SetFocus();
        return false;
    }

    return true;
}

void LoginFrame::ShowMessage(const wxString &message, bool isError)
{
    m_statusLabel->SetLabel(message);
    if (isError)
    {
        m_statusLabel->SetForegroundColour(*wxRED);
    }
    else
    {
        m_statusLabel->SetForegroundColour(*wxBLUE);
    }
    m_mainPanel->Layout();
}

void LoginFrame::OnLogin(wxCommandEvent &event)
{
    if (!ValidateInput())
    {
        return;
    }

    std::string email = m_emailCtrl->GetValue().ToStdString();
    std::string password = m_passwordCtrl->GetValue().ToStdString();

    // Get selected role
    wxString selectedRoleStr = m_roleChoice->GetStringSelection();
    UserRole selectedRole;
    if (selectedRoleStr == "ADMIN")
    {
        selectedRole = UserRole::ADMIN;
    }
    else if (selectedRoleStr == "STAFF")
    {
        selectedRole = UserRole::STAFF;
    }
    else
    {
        selectedRole = UserRole::CUSTOMER;
    }

    // Ensure auth controller is valid
    if (!m_authController)
    {
        ShowMessage("System error: Authentication controller not available.", true);
        wxMessageBox("System error! Please restart the application.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Clear any previous user session first
    m_authController->logout();

    // Authenticate user
    bool loginSuccess = m_authController->login(email, password);
    if (loginSuccess)
    {
        // Get the current user after successful login
        auto user = m_authController->getCurrentUser();
        if (user)
        {
            // Check if the user's actual role matches the selected role
            if (user->getRole() != selectedRole)
            {
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
        else
        {
            // This shouldn't happen if login returned true, but handle it anyway
            m_authController->logout();
            ShowMessage("Login Failed", true);
            wxMessageBox("System error during login! Please try again.", "Login Error", wxOK | wxICON_ERROR);
            ClearForm();
        }
    }
    else
    {
        ShowMessage("Login Failed", true);
        wxMessageBox("Invalid email or password!", "Login Error", wxOK | wxICON_ERROR);
        ClearForm();
    }
}

void LoginFrame::OnRegister(wxCommandEvent &event)
{
    RegisterDialog *registerDialog = new RegisterDialog(this);
    if (registerDialog->ShowModal() == wxID_OK)
    {
        // Get registration data
        std::string email = registerDialog->GetEmail();
        std::string password = registerDialog->GetPassword();
        std::string fullName = registerDialog->GetFullName();
        std::string phoneNumber = registerDialog->GetPhoneNumber();

        // Register user
        if (m_authController->registerUser(email, password, fullName, phoneNumber))
        {
            ShowMessage("Registration successful! Please login.", false);
            wxMessageBox("Registration successful! Please login.", "Success", wxOK | wxICON_INFORMATION);
        }
        else
        {
            ShowMessage("Registration failed!", true);
            wxMessageBox("Registration failed! Account already exists.", "Error", wxOK | wxICON_ERROR);
        }
    }
    registerDialog->Destroy();
}

void LoginFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
    wxTheApp->ExitMainLoop();
}

void LoginFrame::OnExit(wxCommandEvent &event)
{
    wxExit();
}

void LoginFrame::ClearForm()
{
    m_passwordCtrl->SetValue("");
    m_statusLabel->SetLabel("");
    m_roleChoice->SetSelection(2); // Reset to CUSTOMER
    m_mainPanel->Layout();
    m_emailCtrl->SetFocus();
}

void LoginFrame::OpenMainWindow()
{
    // Use app method to show main frame
    BadmintonApp *app = BadmintonApp::GetInstance();
    if (app)
    {
        app->ShowMainFrame();
    }
    else
    {
        wxMessageBox("System error: Cannot access application instance!", "Error", wxOK | wxICON_ERROR);
    }
}

void LoginFrame::OnPasswordFocus(wxFocusEvent& event)
{
    if (m_imgNormal && m_imgFocus) {
        m_imgNormal->Hide();
        m_imgFocus->Show();
        m_mainPanel->Layout();
    }
    event.Skip();
}

void LoginFrame::OnPasswordKillFocus(wxFocusEvent& event)
{
    if (m_imgNormal && m_imgFocus) {
        m_imgFocus->Hide();
        m_imgNormal->Show();
        m_mainPanel->Layout();
    }
    event.Skip();
}

void LoginFrame::OnResize(wxSizeEvent& event)
{
    // Only resize images if they exist
    if (m_imgNormal && m_imgFocus && m_imgNormalOriginal.IsOk() && m_imgFocusOriginal.IsOk()) {
        wxSize size = GetClientSize();

        // Giới hạn chiều rộng ảnh = 20% chiều rộng cửa sổ
        int targetWidth = std::min(1.0 * m_imgNormalOriginal.GetWidth(),
                                   size.GetWidth() * 0.2);
        // Giữ tỉ lệ
        int targetHeight = m_imgNormalOriginal.GetHeight() *
                           targetWidth / m_imgNormalOriginal.GetWidth();

        // Scale ảnh thường
        wxImage scaledNormal = m_imgNormalOriginal.Scale(targetWidth, targetHeight, wxIMAGE_QUALITY_HIGH);
        m_imgNormal->SetBitmap(wxBitmap(scaledNormal));

        // Scale ảnh focus
        wxImage scaledFocus = m_imgFocusOriginal.Scale(targetWidth, targetHeight, wxIMAGE_QUALITY_HIGH);
        m_imgFocus->SetBitmap(wxBitmap(scaledFocus));
    }

    Layout(); // update sizer
    event.Skip(); // tiếp tục xử lý sự kiện resize
}
