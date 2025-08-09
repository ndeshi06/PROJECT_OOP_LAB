#pragma once
#include <wx/wx.h>

class AuthController;
class CourtController;
class BookingController;

class LoginFrame : public wxFrame
{
private:
    AuthController *m_authController;
    CourtController *m_courtController;
    BookingController *m_bookingController;

    // UI components
    wxPanel *m_mainPanel;
    wxTextCtrl *m_emailCtrl;
    wxTextCtrl *m_passwordCtrl;
    wxChoice *m_roleChoice;
    wxButton *m_loginBtn;
    wxButton *m_registerBtn;
    wxStaticText *m_statusLabel;

    // Layout components
    wxBoxSizer *m_mainSizer;
    wxFlexGridSizer *m_formSizer;
    wxBoxSizer *m_buttonSizer;

public:
    LoginFrame(AuthController *authController,
               CourtController *courtController = nullptr,
               BookingController *bookingController = nullptr);
    ~LoginFrame();

    // Event handlers
    void OnLogin(wxCommandEvent &event);
    void OnRegister(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnExit(wxCommandEvent &event);

    // Public methods
    void ClearForm();

private:
    void CreateUI();
    void CreateMenuBar();
    void BindEvents();
    bool ValidateInput();
    void ShowMessage(const wxString &message, bool isError = false);
    void OpenMainWindow();

    DECLARE_EVENT_TABLE()
};

// Event IDs
enum
{
    ID_LOGIN = 1000,
    ID_REGISTER,
    ID_LOGIN_ROLE_CHOICE
};
