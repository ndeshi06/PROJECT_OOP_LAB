#pragma once

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <string>

class RegisterDialog : public wxDialog
{
public:
    RegisterDialog(wxWindow* parent);
    
    // Get input values
    std::string GetEmail() const;
    std::string GetPassword() const;
    std::string GetFullName() const;
    std::string GetPhoneNumber() const;

private:
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    bool ValidateInput();
    
    // Controls
    wxTextCtrl* m_emailCtrl;
    wxTextCtrl* m_passwordCtrl;
    wxTextCtrl* m_fullNameCtrl;
    wxTextCtrl* m_phoneNumberCtrl;
    wxButton* m_okButton;
    wxButton* m_cancelButton;
    
    wxDECLARE_EVENT_TABLE();
};
