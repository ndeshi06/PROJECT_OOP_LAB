#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/grid.h>
#include <memory>

class CourtController;
class AuthController;

class CourtManagementPanel : public wxPanel {
private:
    std::shared_ptr<CourtController> m_courtController;
    std::shared_ptr<AuthController> m_authController;
    
    // UI components
    wxListCtrl* m_courtList;
    wxButton* m_addBtn;
    wxButton* m_editBtn;
    wxButton* m_deleteBtn;
    wxButton* m_refreshBtn;
    
    // Court details
    wxTextCtrl* m_nameCtrl;
    wxTextCtrl* m_descriptionCtrl;
    wxTextCtrl* m_rateCtrl;
    wxChoice* m_statusChoice;
    wxButton* m_saveBtn;
    wxButton* m_cancelBtn;
    
    // Layout
    wxBoxSizer* m_mainSizer;
    wxBoxSizer* m_leftSizer;
    wxBoxSizer* m_rightSizer;
    wxStaticBoxSizer* m_detailsSizer;
    
    // State
    int m_selectedCourtId;
    bool m_isEditing;

public:
    CourtManagementPanel(wxWindow* parent, 
                        std::shared_ptr<CourtController> courtController,
                        std::shared_ptr<AuthController> authController);
    ~CourtManagementPanel();

    // Event handlers
    void OnAddCourt(wxCommandEvent& event);
    void OnEditCourt(wxCommandEvent& event);
    void OnDeleteCourt(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnCourtSelected(wxListEvent& event);
    void OnCourtDeselected(wxListEvent& event);

    // Public methods
    void RefreshCourtList();
    void UpdateUI();

private:
    void CreateUI();
    void CreateCourtList();
    void CreateDetailsPanel();
    void CreateButtons();
    void BindEvents();
    
    void LoadCourtDetails(int courtId);
    void ClearDetailsPanel();
    bool ValidateInput();
    void EnableEditing(bool enable);
    void UpdateButtonStates();
    
    // Helper methods
    void PopulateCourtList();
    wxString FormatCurrency(double amount);

    DECLARE_EVENT_TABLE()
};

// Event IDs
enum {
    ID_ADD_COURT = 3000,
    ID_EDIT_COURT,
    ID_DELETE_COURT,
    ID_REFRESH_COURTS,
    ID_COURT_SAVE,
    ID_COURT_CANCEL,
    ID_COURT_LIST
};
