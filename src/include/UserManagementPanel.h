#pragma once

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <memory>

class AuthController;

enum {
    ID_USER_LIST = 3000,
    ID_REFRESH_USERS,
    ID_DELETE_USER,
    ID_CHANGE_ROLE,
    ID_TOGGLE_STATUS,
    ID_ROLE_CHOICE
};

class UserManagementPanel : public wxPanel
{
public:
    UserManagementPanel(wxWindow* parent, std::shared_ptr<AuthController> authController);
    ~UserManagementPanel();

private:
    void CreateUI();
    void CreateUserList();
    void CreateUserActions();
    void BindEvents();
    
    // Event handlers
    void OnRefreshUsers(wxCommandEvent& event);
    void OnDeleteUser(wxCommandEvent& event);
    void OnChangeRole(wxCommandEvent& event);
    void OnToggleStatus(wxCommandEvent& event);
    void OnUserSelected(wxListEvent& event);
    
    // Helper methods
    void RefreshUserList();
    void UpdateButtonStates();
    void ClearSelection();
    
    // UI components
    wxBoxSizer* m_mainSizer;
    wxStaticBoxSizer* m_userListSizer;
    wxStaticBoxSizer* m_actionsSizer;
    
    wxListCtrl* m_userList;
    wxButton* m_refreshBtn;
    wxButton* m_deleteBtn;
    wxButton* m_changeRoleBtn;
    wxButton* m_toggleStatusBtn;
    wxChoice* m_roleChoice;
    
    // Controllers
    std::shared_ptr<AuthController> m_authController;
    
    // State
    int m_selectedUserId;
    
    wxDECLARE_EVENT_TABLE();
};
