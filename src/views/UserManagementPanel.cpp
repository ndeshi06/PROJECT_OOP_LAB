#include "UserManagementPanel.h"
#include "../include/AuthController.h"
#include "../include/User.h"
#include <wx/msgdlg.h>
#include <wx/stattext.h>

wxBEGIN_EVENT_TABLE(UserManagementPanel, wxPanel)
    EVT_BUTTON(ID_REFRESH_USERS, UserManagementPanel::OnRefreshUsers)
    EVT_BUTTON(ID_DELETE_USER, UserManagementPanel::OnDeleteUser)
    EVT_BUTTON(ID_CHANGE_ROLE, UserManagementPanel::OnChangeRole)
    EVT_BUTTON(ID_TOGGLE_STATUS, UserManagementPanel::OnToggleStatus)
    EVT_LIST_ITEM_SELECTED(ID_USER_LIST, UserManagementPanel::OnUserSelected)
wxEND_EVENT_TABLE()

UserManagementPanel::UserManagementPanel(wxWindow *parent,
                                            AuthController* authController)
: wxPanel(parent, wxID_ANY),
    m_authController(authController),
    m_selectedUserId(-1)
{
    CreateUI();
    BindEvents();
    RefreshUserList();
}

UserManagementPanel::~UserManagementPanel() {}

void UserManagementPanel::CreateUI()
{
    m_mainSizer = new wxBoxSizer(wxVERTICAL);

    // Title
    wxStaticText *title = new wxStaticText(this, wxID_ANY, "User Management");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    m_mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);

    CreateUserList();
    CreateUserActions();

    SetSizer(m_mainSizer);
}

void UserManagementPanel::CreateUserList()
{
    m_userListSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Registered Users");

    m_userList = new wxListCtrl(this, ID_USER_LIST,
                                wxDefaultPosition, wxSize(-1, 300),
                                wxLC_REPORT | wxLC_SINGLE_SEL);

    // Add columns
    m_userList->AppendColumn("ID", wxLIST_FORMAT_RIGHT, 50);
    m_userList->AppendColumn("Email", wxLIST_FORMAT_LEFT, 200);
    m_userList->AppendColumn("Full Name", wxLIST_FORMAT_LEFT, 150);
    m_userList->AppendColumn("Phone", wxLIST_FORMAT_LEFT, 120);
    m_userList->AppendColumn("Role", wxLIST_FORMAT_LEFT, 100);
    m_userList->AppendColumn("Status", wxLIST_FORMAT_LEFT, 80);
    m_userList->AppendColumn("Created", wxLIST_FORMAT_LEFT, 150);

    m_userListSizer->Add(m_userList, 1, wxEXPAND | wxALL, 5);
    m_mainSizer->Add(m_userListSizer, 1, wxEXPAND | wxALL, 10);
}

void UserManagementPanel::CreateUserActions()
{
    m_actionsSizer = new wxStaticBoxSizer(wxVERTICAL, this, "User Actions");

    // Role selection
    wxBoxSizer *roleSizer = new wxBoxSizer(wxHORIZONTAL);
    roleSizer->Add(new wxStaticText(this, wxID_ANY, "Change Role to:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    m_roleChoice = new wxChoice(this, ID_ROLE_CHOICE);
    m_roleChoice->Append("ADMIN");
    m_roleChoice->Append("STAFF");
    m_roleChoice->Append("CUSTOMER");
    m_roleChoice->SetSelection(2); // Default to CUSTOMER

    roleSizer->Add(m_roleChoice, 0, wxRIGHT, 10);
    m_changeRoleBtn = new wxButton(this, ID_CHANGE_ROLE, "Change Role");
    roleSizer->Add(m_changeRoleBtn, 0);

    m_actionsSizer->Add(roleSizer, 0, wxALL | wxEXPAND, 5);

    // Action buttons
    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_refreshBtn = new wxButton(this, ID_REFRESH_USERS, "Refresh List");
    m_toggleStatusBtn = new wxButton(this, ID_TOGGLE_STATUS, "Enable/Disable User");
    m_deleteBtn = new wxButton(this, ID_DELETE_USER, "Delete User");

    buttonSizer->Add(m_refreshBtn, 0, wxRIGHT, 10);
    buttonSizer->Add(m_toggleStatusBtn, 0, wxRIGHT, 10);
    buttonSizer->Add(m_deleteBtn, 0);

    m_actionsSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 5);
    m_mainSizer->Add(m_actionsSizer, 0, wxEXPAND | wxALL, 10);

    // Initially disable action buttons
    UpdateButtonStates();
}

void UserManagementPanel::BindEvents()
{
    // Events are handled by the event table
}

void UserManagementPanel::RefreshUserList()
{
    m_userList->DeleteAllItems();

    if (!m_authController)
    {
        return;
    }

    try
    {
        auto users = m_authController->getAllUsers();

        for (size_t i = 0; i < users.size(); ++i)
        {
            auto user = users[i];
            if (!user)
                continue;

            long index = m_userList->InsertItem(i, wxString::Format("%d", user->getId()));
            m_userList->SetItem(index, 1, user->getEmail());
            m_userList->SetItem(index, 2, user->getFullName());
            m_userList->SetItem(index, 3, user->getPhoneNumber());
            m_userList->SetItem(index, 4, user->getRoleString());
            m_userList->SetItem(index, 5, user->isActive() ? "Active" : "Inactive");

            // Format creation date
            std::time_t createdTime = user->getCreatedAt();
            wxDateTime created(createdTime);
            m_userList->SetItem(index, 6, created.Format("%Y-%m-%d %H:%M"));

            // Store user ID in item data
            m_userList->SetItemData(index, user->getId());
        }

        // Show total count
        wxString statusMsg = wxString::Format("Total Users: %zu", users.size());
        if (users.empty())
        {
            statusMsg = "No users found. Users will appear here after registration.";
        }

        // Add a status text at the bottom
        static wxStaticText *statusText = nullptr;
        if (!statusText)
        {
            statusText = new wxStaticText(this, wxID_ANY, statusMsg);
            m_mainSizer->Add(statusText, 0, wxALL | wxALIGN_CENTER, 5);
        }
        else
        {
            statusText->SetLabel(statusMsg);
        }
    }
    catch (const std::exception &e)
    {
        wxMessageBox(wxString::Format("Error loading users: %s", e.what()),
                     "Error", wxOK | wxICON_ERROR, this);
    }
}

void UserManagementPanel::OnRefreshUsers(wxCommandEvent &event)
{
    RefreshUserList();
    wxMessageBox("User list refreshed successfully!",
                 "Information", wxOK | wxICON_INFORMATION, this);
}

void UserManagementPanel::OnDeleteUser(wxCommandEvent &event)
{
    if (m_selectedUserId == -1)
    {
        wxMessageBox("Please select a user to delete!",
                     "No User Selected", wxOK | wxICON_WARNING, this);
        return;
    }

    // Check if trying to delete current user (self)
    auto currentUser = m_authController->getCurrentUser();
    if (currentUser && currentUser->getId() == m_selectedUserId)
    {
        wxMessageBox("You cannot delete your own account!\n\n"
                     "To delete your account, please ask another administrator "
                     "to do it for you.",
                     "Cannot Delete Own Account",
                     wxOK | wxICON_WARNING, this);
        return;
    }

    // Get selected user info
    long selectedItem = m_userList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selectedItem == -1)
        return;

    wxString userEmail = m_userList->GetItemText(selectedItem, 1);
    wxString userName = m_userList->GetItemText(selectedItem, 2);

    // Confirm deletion
    int result = wxMessageBox(
        wxString::Format("Are you sure you want to delete this user?\n\n"
                         "Name: %s\n"
                         "Email: %s\n\n"
                         "This action cannot be undone!",
                         userName, userEmail),
        "Confirm User Deletion",
        wxYES_NO | wxICON_QUESTION, this);

    if (result == wxYES)
    {
        try
        {
            // Actually delete the user through controller
            if (m_authController && m_authController->deleteUser(m_selectedUserId))
            {
                wxMessageBox(wxString::Format("User '%s' has been deleted successfully!", userName),
                             "User Deleted", wxOK | wxICON_INFORMATION, this);

                RefreshUserList();
                ClearSelection();
            }
            else
            {
                wxMessageBox(wxString::Format("Failed to delete user '%s'!\n\n"
                                              "The user may not exist or there was an error.\n"
                                              "Please refresh the list and try again.",
                                              userName),
                             "Deletion Failed", wxOK | wxICON_ERROR, this);
            }
        }
        catch (const std::exception &e)
        {
            wxMessageBox(wxString::Format("Failed to delete user: %s", e.what()),
                         "Deletion Error", wxOK | wxICON_ERROR, this);
        }
    }
}

void UserManagementPanel::OnChangeRole(wxCommandEvent &event)
{
    if (m_selectedUserId == -1)
    {
        wxMessageBox("Please select a user to change role!",
                     "No User Selected", wxOK | wxICON_WARNING, this);
        return;
    }

    // Check if trying to change role of current user (self)
    auto currentUser = m_authController->getCurrentUser();
    if (currentUser && currentUser->getId() == m_selectedUserId)
    {
        wxMessageBox("You cannot change your own role!\n\n"
                     "To change your role, please ask another administrator "
                     "to do it for you.",
                     "Cannot Change Own Role",
                     wxOK | wxICON_WARNING, this);
        return;
    }

    long selectedItem = m_userList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selectedItem == -1)
        return;

    wxString userEmail = m_userList->GetItemText(selectedItem, 1);
    wxString userName = m_userList->GetItemText(selectedItem, 2);
    wxString currentRole = m_userList->GetItemText(selectedItem, 4);
    wxString newRole = m_roleChoice->GetStringSelection();

    if (currentRole == newRole)
    {
        wxMessageBox(wxString::Format("User '%s' already has the role '%s'!", userName, newRole),
                     "No Change Needed", wxOK | wxICON_INFORMATION, this);
        return;
    }

    int result = wxMessageBox(
        wxString::Format("Change user role?\n\n"
                         "User: %s (%s)\n"
                         "Current Role: %s\n"
                         "New Role: %s",
                         userName, userEmail, currentRole, newRole),
        "Confirm Role Change",
        wxYES_NO | wxICON_QUESTION, this);

    if (result == wxYES)
    {
        try
        {
            // Convert string role to UserRole enum
            UserRole newRoleEnum = UserRole::CUSTOMER;
            if (newRole == "ADMIN")
                newRoleEnum = UserRole::ADMIN;
            else if (newRole == "STAFF")
                newRoleEnum = UserRole::STAFF;

            // Actually change the user role through controller
            if (m_authController && m_authController->changeUserRole(m_selectedUserId, newRoleEnum))
            {
                wxMessageBox(wxString::Format("User '%s' role changed from '%s' to '%s' successfully!",
                                              userName, currentRole, newRole),
                             "Role Changed", wxOK | wxICON_INFORMATION, this);

                RefreshUserList();
            }
            else
            {
                wxMessageBox(wxString::Format("Failed to change role for user '%s'!\n\n"
                                              "The user may not exist or there was an error.\n"
                                              "Please refresh the list and try again.",
                                              userName),
                             "Role Change Failed", wxOK | wxICON_ERROR, this);
            }
        }
        catch (const std::exception &e)
        {
            wxMessageBox(wxString::Format("Failed to change user role: %s", e.what()),
                         "Role Change Error", wxOK | wxICON_ERROR, this);
        }
    }
}

void UserManagementPanel::OnToggleStatus(wxCommandEvent &event)
{
    if (m_selectedUserId == -1)
    {
        wxMessageBox("Please select a user to enable/disable!",
                     "No User Selected", wxOK | wxICON_WARNING, this);
        return;
    }

    // Check if trying to disable current user (self)
    auto currentUser = m_authController->getCurrentUser();
    if (currentUser && currentUser->getId() == m_selectedUserId)
    {
        wxMessageBox("You cannot disable your own account!\n\n"
                     "To disable your account, please ask another administrator "
                     "to do it for you.",
                     "Cannot Disable Own Account",
                     wxOK | wxICON_WARNING, this);
        return;
    }

    long selectedItem = m_userList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selectedItem == -1)
        return;

    wxString userEmail = m_userList->GetItemText(selectedItem, 1);
    wxString userName = m_userList->GetItemText(selectedItem, 2);
    wxString currentStatus = m_userList->GetItemText(selectedItem, 5);
    wxString newStatus = (currentStatus == "Active") ? "Inactive" : "Active";
    wxString action = (currentStatus == "Active") ? "disable" : "enable";

    int result = wxMessageBox(
        wxString::Format("Are you sure you want to %s this user?\n\n"
                         "User: %s (%s)\n"
                         "Current Status: %s\n"
                         "New Status: %s",
                         action, userName, userEmail, currentStatus, newStatus),
        wxString::Format("Confirm User %s", action.Capitalize()),
        wxYES_NO | wxICON_QUESTION, this);

    if (result == wxYES)
    {
        try
        {
            // Actually toggle the user status through controller
            if (m_authController && m_authController->toggleUserStatus(m_selectedUserId))
            {
                wxMessageBox(wxString::Format("User '%s' has been %sd successfully!",
                                              userName, action),
                             "Status Changed", wxOK | wxICON_INFORMATION, this);

                RefreshUserList();
            }
            else
            {
                wxMessageBox(wxString::Format("Failed to %s user '%s'!\n\n"
                                              "The user may not exist or there was an error.\n"
                                              "Please refresh the list and try again.",
                                              action, userName),
                             "Status Change Failed", wxOK | wxICON_ERROR, this);
            }
        }
        catch (const std::exception &e)
        {
            wxMessageBox(wxString::Format("Failed to %s user: %s", action, e.what()),
                         "Status Change Error", wxOK | wxICON_ERROR, this);
        }
    }
}

void UserManagementPanel::OnUserSelected(wxListEvent &event)
{
    long item = event.GetIndex();
    if (item != -1)
    {
        m_selectedUserId = m_userList->GetItemData(item);
        UpdateButtonStates();
    }
}

void UserManagementPanel::UpdateButtonStates()
{
    bool hasSelection = (m_selectedUserId != -1);
    m_deleteBtn->Enable(hasSelection);
    m_changeRoleBtn->Enable(hasSelection);
    m_toggleStatusBtn->Enable(hasSelection);
}

void UserManagementPanel::ClearSelection()
{
    m_selectedUserId = -1;
    m_userList->SetItemState(-1, 0, wxLIST_STATE_SELECTED);
    UpdateButtonStates();
}
