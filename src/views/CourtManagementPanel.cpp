#include "CourtManagementPanel.h"
#include "../include/CourtController.h"
#include "../include/AuthController.h"
#include "../include/Court.h"
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/statbox.h>

wxBEGIN_EVENT_TABLE(CourtManagementPanel, wxPanel)
    EVT_BUTTON(ID_ADD_COURT, CourtManagementPanel::OnAddCourt)
    EVT_BUTTON(ID_EDIT_COURT, CourtManagementPanel::OnEditCourt)
    EVT_BUTTON(ID_DELETE_COURT, CourtManagementPanel::OnDeleteCourt)
    EVT_BUTTON(ID_REFRESH_COURTS, CourtManagementPanel::OnRefresh)
    EVT_BUTTON(ID_COURT_SAVE, CourtManagementPanel::OnSave)
    EVT_BUTTON(ID_COURT_CANCEL, CourtManagementPanel::OnCancel)
    EVT_LIST_ITEM_SELECTED(ID_COURT_LIST, CourtManagementPanel::OnCourtSelected)
    EVT_LIST_ITEM_DESELECTED(ID_COURT_LIST, CourtManagementPanel::OnCourtDeselected)
wxEND_EVENT_TABLE()

CourtManagementPanel::CourtManagementPanel(wxWindow* parent, 
                                         std::shared_ptr<CourtController> courtController,
                                         std::shared_ptr<AuthController> authController)
    : wxPanel(parent, wxID_ANY), 
      m_courtController(courtController),
      m_authController(authController),
      m_selectedCourtId(-1),
      m_isEditing(false)
{
    CreateUI();
    BindEvents();
    RefreshCourtList();
}

CourtManagementPanel::~CourtManagementPanel()
{
}

void CourtManagementPanel::CreateUI()
{
    CreateCourtList();
    CreateDetailsPanel();
    CreateButtons();
    
    // Main layout
    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
    m_leftSizer = new wxBoxSizer(wxVERTICAL);
    m_rightSizer = new wxBoxSizer(wxVERTICAL);
    
    // Left side - Court list and buttons
    wxStaticBoxSizer* listSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Court List");
    listSizer->Add(m_courtList, 1, wxEXPAND | wxALL, 5);
    
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(m_addBtn, 0, wxRIGHT, 5);
    buttonSizer->Add(m_editBtn, 0, wxRIGHT, 5);
    buttonSizer->Add(m_deleteBtn, 0, wxRIGHT, 5);
    buttonSizer->Add(m_refreshBtn, 0);
    
    listSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 5);
    m_leftSizer->Add(listSizer, 1, wxEXPAND | wxALL, 5);
    
    // Right side - Details
    m_rightSizer->Add(m_detailsSizer, 1, wxEXPAND | wxALL, 5);
    
    // Main layout
    m_mainSizer->Add(m_leftSizer, 1, wxEXPAND);
    m_mainSizer->Add(m_rightSizer, 0, wxEXPAND);
    
    SetSizer(m_mainSizer);
}

void CourtManagementPanel::CreateCourtList()
{
    m_courtList = new wxListCtrl(this, ID_COURT_LIST, wxDefaultPosition, wxSize(400, -1),
                                wxLC_REPORT | wxLC_SINGLE_SEL);
    
    m_courtList->AppendColumn("ID", wxLIST_FORMAT_RIGHT, 50);
    m_courtList->AppendColumn("Court Name", wxLIST_FORMAT_LEFT, 120);
    m_courtList->AppendColumn("Type", wxLIST_FORMAT_LEFT, 80);
    m_courtList->AppendColumn("Price", wxLIST_FORMAT_RIGHT, 80);
    m_courtList->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
}

void CourtManagementPanel::CreateDetailsPanel()
{
    m_detailsSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Court Details");
    
    wxFlexGridSizer* detailsGrid = new wxFlexGridSizer(4, 2, 10, 10);
    
    // Name
    detailsGrid->Add(new wxStaticText(this, wxID_ANY, "Court Name:"), 0, wxALIGN_CENTER_VERTICAL);
    m_nameCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    detailsGrid->Add(m_nameCtrl, 0, wxEXPAND);
    
    // Description
    detailsGrid->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALIGN_CENTER_VERTICAL);
    m_descriptionCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, 80), wxTE_MULTILINE);
    detailsGrid->Add(m_descriptionCtrl, 0, wxEXPAND);
    
    // Rate
    detailsGrid->Add(new wxStaticText(this, wxID_ANY, "Price/Hour:"), 0, wxALIGN_CENTER_VERTICAL);
    m_rateCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    detailsGrid->Add(m_rateCtrl, 0, wxEXPAND);
    
    // Status
    detailsGrid->Add(new wxStaticText(this, wxID_ANY, "Status:"), 0, wxALIGN_CENTER_VERTICAL);
    m_statusChoice = new wxChoice(this, wxID_ANY);
    m_statusChoice->Append("Available");
    m_statusChoice->Append("Maintenance");
    m_statusChoice->Append("Out of Service");
    detailsGrid->Add(m_statusChoice, 0, wxEXPAND);
    
    detailsGrid->AddGrowableCol(1);
    m_detailsSizer->Add(detailsGrid, 0, wxEXPAND | wxALL, 10);
    
    // Action buttons
    wxBoxSizer* actionSizer = new wxBoxSizer(wxHORIZONTAL);
    m_saveBtn = new wxButton(this, ID_COURT_SAVE, "Save");
    m_cancelBtn = new wxButton(this, ID_COURT_CANCEL, "Cancel");
    
    actionSizer->Add(m_saveBtn, 0, wxRIGHT, 5);
    actionSizer->Add(m_cancelBtn, 0);
    
    m_detailsSizer->Add(actionSizer, 0, wxALL | wxALIGN_CENTER, 10);
    
    // Initially disable editing
    EnableEditing(false);
}

void CourtManagementPanel::CreateButtons()
{
    m_addBtn = new wxButton(this, ID_ADD_COURT, "Add");
    m_editBtn = new wxButton(this, ID_EDIT_COURT, "Edit");
    m_deleteBtn = new wxButton(this, ID_DELETE_COURT, "Delete");
    m_refreshBtn = new wxButton(this, ID_REFRESH_COURTS, "Refresh");
    
    // Initially disable edit/delete buttons
    UpdateButtonStates();
}

void CourtManagementPanel::BindEvents()
{
    // Events are bound through the event table
}

void CourtManagementPanel::RefreshCourtList()
{
    PopulateCourtList();
}

void CourtManagementPanel::UpdateUI()
{
    RefreshCourtList();
    UpdateButtonStates();
}

void CourtManagementPanel::OnAddCourt(wxCommandEvent& event)
{
    ClearDetailsPanel();
    EnableEditing(true);
    m_isEditing = false;
    m_selectedCourtId = -1;
    m_nameCtrl->SetFocus();
}

void CourtManagementPanel::OnEditCourt(wxCommandEvent& event)
{
    if (m_selectedCourtId == -1) {
        wxMessageBox("Please select a court to edit!", "Warning", 
                    wxOK | wxICON_WARNING);
        return;
    }
    
    LoadCourtDetails(m_selectedCourtId);
    EnableEditing(true);
    m_isEditing = true;
    m_nameCtrl->SetFocus();
}

void CourtManagementPanel::OnDeleteCourt(wxCommandEvent& event)
{
    if (m_selectedCourtId == -1) {
        wxMessageBox("Please select a court to delete!", "Warning", 
                    wxOK | wxICON_WARNING);
        return;
    }
    
    int result = wxMessageBox("Are you sure you want to delete this court?",
                             "Confirm Delete",
                             wxYES_NO | wxICON_QUESTION);
    
    if (result == wxYES) {
        try {
            // Actually delete the court through controller
            if (m_courtController && m_courtController->deleteCourt(m_selectedCourtId)) {
                m_selectedCourtId = -1;
                RefreshCourtList();
                ClearDetailsPanel();
                UpdateButtonStates();
                wxMessageBox("Court deleted successfully!", "Success", wxOK | wxICON_INFORMATION);
            } else {
                wxMessageBox("Failed to delete court!", "Error", wxOK | wxICON_ERROR);
            }
        } catch (const std::exception& e) {
            wxMessageBox("Error deleting court: " + wxString(e.what()), "Error", 
                        wxOK | wxICON_ERROR);
        }
    }
}

void CourtManagementPanel::OnRefresh(wxCommandEvent& event)
{
    RefreshCourtList();
    wxMessageBox("Data refreshed!", "Information", wxOK | wxICON_INFORMATION);
}

void CourtManagementPanel::OnSave(wxCommandEvent& event)
{
    if (!ValidateInput()) {
        return;
    }
    
    try {
        wxString name = m_nameCtrl->GetValue();
        wxString description = m_descriptionCtrl->GetValue();
        wxString rateStr = m_rateCtrl->GetValue();
        
        double rate = 0.0;
        rateStr.ToDouble(&rate);
        
        // Get selected status
        CourtStatus status = CourtStatus::AVAILABLE;
        int statusSelection = m_statusChoice->GetSelection();
        switch (statusSelection) {
            case 0:
                status = CourtStatus::AVAILABLE;
                break;
            case 1:
                status = CourtStatus::MAINTENANCE;
                break;
            case 2:
                status = CourtStatus::OUT_OF_SERVICE;
                break;
            default:
                status = CourtStatus::AVAILABLE;
                break;
        }
        
        if (m_isEditing) {
            // Update existing court
            Court updatedCourt(name.ToStdString(), description.ToStdString(), rate, status);
            if (m_courtController && m_courtController->updateCourt(m_selectedCourtId, updatedCourt)) {
                wxMessageBox("Court information updated successfully!", "Success", wxOK | wxICON_INFORMATION);
            } else {
                wxMessageBox("Failed to update court information!", "Error", wxOK | wxICON_ERROR);
                return;
            }
        } else {
            // Add new court
            if (m_courtController && m_courtController->addCourt(name.ToStdString(), description.ToStdString(), rate, status)) {
                wxMessageBox("New court added successfully!", "Success", wxOK | wxICON_INFORMATION);
            } else {
                wxMessageBox("Failed to add new court! Court name may already exist.", "Error", wxOK | wxICON_ERROR);
                return;
            }
        }
        
        RefreshCourtList();
        EnableEditing(false);
        ClearDetailsPanel();
        m_selectedCourtId = -1;
        UpdateButtonStates();
        
    } catch (const std::exception& e) {
        wxMessageBox("Error saving: " + wxString(e.what()), "Error", 
                    wxOK | wxICON_ERROR);
    }
}

void CourtManagementPanel::OnCancel(wxCommandEvent& event)
{
    EnableEditing(false);
    if (m_selectedCourtId != -1) {
        LoadCourtDetails(m_selectedCourtId);
    } else {
        ClearDetailsPanel();
    }
}

void CourtManagementPanel::OnCourtSelected(wxListEvent& event)
{
    long item = event.GetIndex();
    if (item != -1) {
        // Get the real court ID from the item data
        m_selectedCourtId = m_courtList->GetItemData(item);
        LoadCourtDetails(m_selectedCourtId);
        UpdateButtonStates();
    }
}

void CourtManagementPanel::OnCourtDeselected(wxListEvent& event)
{
    m_selectedCourtId = -1;
    ClearDetailsPanel();
    UpdateButtonStates();
}

void CourtManagementPanel::LoadCourtDetails(int courtId)
{
    if (!m_courtController) {
        ClearDetailsPanel();
        return;
    }
    
    // Get real court data from controller
    auto court = m_courtController->getCourt(courtId);
    if (court) {
        m_nameCtrl->SetValue(court->getName());
        m_descriptionCtrl->SetValue(court->getDescription());
        m_rateCtrl->SetValue(wxString::Format("%.0f", court->getHourlyRate()));
        
        // Set status selection
        switch (court->getStatus()) {
            case CourtStatus::AVAILABLE:
                m_statusChoice->SetSelection(0);
                break;
            case CourtStatus::MAINTENANCE:
                m_statusChoice->SetSelection(1);
                break;
            case CourtStatus::OUT_OF_SERVICE:
                m_statusChoice->SetSelection(2);
                break;
            default:
                m_statusChoice->SetSelection(0);
                break;
        }
    } else {
        ClearDetailsPanel();
    }
}

void CourtManagementPanel::ClearDetailsPanel()
{
    m_nameCtrl->Clear();
    m_descriptionCtrl->Clear();
    m_rateCtrl->Clear();
    m_statusChoice->SetSelection(-1);
}

bool CourtManagementPanel::ValidateInput()
{
    if (m_nameCtrl->GetValue().IsEmpty()) {
        wxMessageBox("Please enter court name!", "Warning", wxOK | wxICON_WARNING);
        m_nameCtrl->SetFocus();
        return false;
    }
    
    if (m_rateCtrl->GetValue().IsEmpty()) {
        wxMessageBox("Please enter court price!", "Warning", wxOK | wxICON_WARNING);
        m_rateCtrl->SetFocus();
        return false;
    }
    
    double rate;
    if (!m_rateCtrl->GetValue().ToDouble(&rate) || rate <= 0) {
        wxMessageBox("Court price must be a positive number!", "Warning", wxOK | wxICON_WARNING);
        m_rateCtrl->SetFocus();
        return false;
    }
    
    return true;
}

void CourtManagementPanel::EnableEditing(bool enable)
{
    m_nameCtrl->Enable(enable);
    m_descriptionCtrl->Enable(enable);
    m_rateCtrl->Enable(enable);
    m_statusChoice->Enable(enable);
    m_saveBtn->Enable(enable);
    m_cancelBtn->Enable(enable);
}

void CourtManagementPanel::UpdateButtonStates()
{
    bool hasSelection = (m_selectedCourtId != -1);
    m_editBtn->Enable(hasSelection);
    m_deleteBtn->Enable(hasSelection);
}

void CourtManagementPanel::PopulateCourtList()
{
    m_courtList->DeleteAllItems();
    
    if (!m_courtController) {
        return;
    }
    
    // Get real courts from controller
    auto courts = m_courtController->getAllCourts();
    
    int itemIndex = 0;
    for (const auto& court : courts) {
        if (!court) continue;
        
        long index = m_courtList->InsertItem(itemIndex, wxString::Format("%d", court->getId()));
        m_courtList->SetItem(index, 1, court->getName());
        m_courtList->SetItem(index, 2, court->getDescription());
        m_courtList->SetItem(index, 3, FormatCurrency(court->getHourlyRate()));
        
        // Convert status to string
        wxString statusStr = "Available";
        switch (court->getStatus()) {
            case CourtStatus::AVAILABLE:
                statusStr = "Available";
                break;
            case CourtStatus::MAINTENANCE:
                statusStr = "Maintenance";
                break;
            case CourtStatus::OUT_OF_SERVICE:
                statusStr = "Out of Service";
                break;
        }
        m_courtList->SetItem(index, 4, statusStr);
        
        // Store the real court ID in the item data
        m_courtList->SetItemData(index, court->getId());
        itemIndex++;
    }
    
    // If no courts available, show message
    if (courts.empty()) {
        long index = m_courtList->InsertItem(0, "No courts");
        m_courtList->SetItem(index, 1, "No courts available");
        m_courtList->SetItem(index, 2, "Add courts using the Add button");
        m_courtList->SetItem(index, 3, "");
        m_courtList->SetItem(index, 4, "");
    }
}

wxString CourtManagementPanel::FormatCurrency(double amount)
{
    return wxString::Format("%.0f VND", amount);
}
