#pragma once
#include <wx/wx.h>
#include <memory>

class AuthController;

class LoginFrame : public wxFrame {
private:
    std::shared_ptr<AuthController> m_authController;
    
    // UI components
    wxPanel* m_mainPanel;
    wxTextCtrl* m_emailCtrl;
    wxTextCtrl* m_passwordCtrl;
    wxChoice* m_roleChoice;
    wxButton* m_loginBtn;
    wxButton* m_registerBtn;
    wxStaticText* m_statusLabel;
    
    // Layout components
    wxBoxSizer* m_mainSizer;
    wxFlexGridSizer* m_formSizer;
    wxBoxSizer* m_buttonSizer;

public:
    LoginFrame(std::shared_ptr<AuthController> authController);
    ~LoginFrame();

    // Event handlers
    void OnLogin(wxCommandEvent& event);
    void OnRegister(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);

private:
    void CreateUI();
    void CreateMenuBar();
    void BindEvents();
    void ClearForm();
    bool ValidateInput();
    void ShowMessage(const wxString& message, bool isError = false);
    void OpenMainWindow();

    DECLARE_EVENT_TABLE()
};

// Event IDs
enum {
    ID_LOGIN = 1000,
    ID_REGISTER,
    ID_ROLE_CHOICE
};
