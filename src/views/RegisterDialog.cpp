#include "RegisterDialog.h"
#include <wx/valgen.h>
#include <wx/regex.h>

enum
{
    ID_OK = wxID_HIGHEST + 1,
    ID_CANCEL
};

wxBEGIN_EVENT_TABLE(RegisterDialog, wxDialog)
    EVT_BUTTON(ID_OK, RegisterDialog::OnOK)
    EVT_BUTTON(ID_CANCEL, RegisterDialog::OnCancel)
wxEND_EVENT_TABLE()

    RegisterDialog::RegisterDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, "Register New Account", wxDefaultPosition, wxSize(400, 350))
{
    // Create main sizer
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    // Title
    wxStaticText *titleLabel = new wxStaticText(this, wxID_ANY, "Create New Account");
    wxFont titleFont = titleLabel->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleLabel->SetFont(titleFont);
    mainSizer->Add(titleLabel, 0, wxALIGN_CENTER | wxALL, 10);

    // Form sizer
    wxFlexGridSizer *formSizer = new wxFlexGridSizer(4, 2, 10, 10);
    formSizer->AddGrowableCol(1, 1);

    // Email
    formSizer->Add(new wxStaticText(this, wxID_ANY, "Email:"), 0, wxALIGN_CENTER_VERTICAL);
    m_emailCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    formSizer->Add(m_emailCtrl, 1, wxEXPAND);

    // Password
    formSizer->Add(new wxStaticText(this, wxID_ANY, "Password:"), 0, wxALIGN_CENTER_VERTICAL);
    m_passwordCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD);
    formSizer->Add(m_passwordCtrl, 1, wxEXPAND);

    // Full Name
    formSizer->Add(new wxStaticText(this, wxID_ANY, "Full Name:"), 0, wxALIGN_CENTER_VERTICAL);
    m_fullNameCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    formSizer->Add(m_fullNameCtrl, 1, wxEXPAND);

    // Phone Number
    formSizer->Add(new wxStaticText(this, wxID_ANY, "Phone Number:"), 0, wxALIGN_CENTER_VERTICAL);
    m_phoneNumberCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    formSizer->Add(m_phoneNumberCtrl, 1, wxEXPAND);

    mainSizer->Add(formSizer, 1, wxEXPAND | wxALL, 20);

    // Buttons
    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_okButton = new wxButton(this, ID_OK, "Register");
    m_cancelButton = new wxButton(this, ID_CANCEL, "Cancel");

    buttonSizer->Add(m_okButton, 0, wxRIGHT, 10);
    buttonSizer->Add(m_cancelButton, 0);

    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 20);

    SetSizer(mainSizer);

    // Set focus to email field
    m_emailCtrl->SetFocus();
}

std::string RegisterDialog::GetEmail() const
{
    return m_emailCtrl->GetValue().ToStdString();
}

std::string RegisterDialog::GetPassword() const
{
    return m_passwordCtrl->GetValue().ToStdString();
}

std::string RegisterDialog::GetFullName() const
{
    return m_fullNameCtrl->GetValue().ToStdString();
}

std::string RegisterDialog::GetPhoneNumber() const
{
    return m_phoneNumberCtrl->GetValue().ToStdString();
}

void RegisterDialog::OnOK(wxCommandEvent &event)
{
    if (ValidateInput())
    {
        EndModal(wxID_OK);
    }
}

void RegisterDialog::OnCancel(wxCommandEvent &event)
{
    EndModal(wxID_CANCEL);
}

bool RegisterDialog::ValidateInput()
{
    // Check if all fields are filled
    if (GetEmail().empty())
    {
        wxMessageBox("Please enter an email address.", "Validation Error", wxOK | wxICON_WARNING, this);
        m_emailCtrl->SetFocus();
        return false;
    }

    if (GetPassword().empty())
    {
        wxMessageBox("Please enter a password.", "Validation Error", wxOK | wxICON_WARNING, this);
        m_passwordCtrl->SetFocus();
        return false;
    }

    if (GetFullName().empty())
    {
        wxMessageBox("Please enter your full name.", "Validation Error", wxOK | wxICON_WARNING, this);
        m_fullNameCtrl->SetFocus();
        return false;
    }

    if (GetPhoneNumber().empty())
    {
        wxMessageBox("Please enter your phone number.", "Validation Error", wxOK | wxICON_WARNING, this);
        m_phoneNumberCtrl->SetFocus();
        return false;
    }

    // Basic email validation
    wxRegEx emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.Matches(wxString(GetEmail())))
    {
        wxMessageBox("Please enter a valid email address.", "Validation Error", wxOK | wxICON_WARNING, this);
        m_emailCtrl->SetFocus();
        return false;
    }

    // Password length check
    if (GetPassword().length() < 6)
    {
        wxMessageBox("Password must be at least 6 characters long.", "Validation Error", wxOK | wxICON_WARNING, this);
        m_passwordCtrl->SetFocus();
        return false;
    }

    // Phone number validation (basic check for digits)
    wxRegEx phoneRegex("^[0-9+\\-\\s()]+$");
    if (!phoneRegex.Matches(wxString(GetPhoneNumber())))
    {
        wxMessageBox("Please enter a valid phone number.", "Validation Error", wxOK | wxICON_WARNING, this);
        m_phoneNumberCtrl->SetFocus();
        return false;
    }

    return true;
}
