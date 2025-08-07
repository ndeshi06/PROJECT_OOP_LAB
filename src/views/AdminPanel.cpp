#include "AdminPanel.h"
#include "../include/BookingController.h"
#include "../include/CourtController.h"
#include "../include/AuthController.h"
#include "../include/Booking.h"
#include "../include/User.h"
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/statbox.h>
#include <wx/filedlg.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <fstream>

wxBEGIN_EVENT_TABLE(AdminPanel, wxPanel)
    EVT_BUTTON(ID_REFRESH_DATA, AdminPanel::OnRefreshData)
    EVT_BUTTON(ID_EXPORT_DATA, AdminPanel::OnExportData)
    EVT_BUTTON(ID_ADMIN_CANCEL_BOOKING, AdminPanel::OnCancelBooking)
    EVT_LIST_ITEM_SELECTED(ID_BOOKING_HISTORY_LIST, AdminPanel::OnBookingSelected)
    EVT_CHOICE(ID_COURT_FILTER, AdminPanel::OnFilterByCourt)
    EVT_CHOICE(ID_USER_FILTER, AdminPanel::OnFilterByUser)
    EVT_CHOICE(ID_STATUS_FILTER, AdminPanel::OnFilterByCourt)
    EVT_DATE_CHANGED(ID_START_DATE_PICKER, AdminPanel::OnFilterByDate)
    EVT_DATE_CHANGED(ID_END_DATE_PICKER, AdminPanel::OnFilterByDate)
wxEND_EVENT_TABLE()

AdminPanel::AdminPanel(wxWindow* parent,
                       std::shared_ptr<BookingController> bookingController,
                       std::shared_ptr<CourtController> courtController,
                       std::shared_ptr<AuthController> authController)
    : wxPanel(parent, wxID_ANY),
      m_bookingController(bookingController),
      m_courtController(courtController),
      m_authController(authController),
      m_selectedBookingId(-1)
{
    CreateUI();
    RefreshData();
}

AdminPanel::~AdminPanel()
{
}

void AdminPanel::CreateUI()
{
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    
    CreateFilterControls();
    CreateBookingHistoryView();
    CreateStatisticsView();
    
    SetSizer(m_mainSizer);
}

void AdminPanel::CreateFilterControls()
{
    wxStaticBoxSizer* filterSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Filters & Controls");
    
    // Date filters
    filterSizer->Add(new wxStaticText(this, wxID_ANY, "From:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_startDatePicker = new wxDatePickerCtrl(this, ID_START_DATE_PICKER);
    m_startDatePicker->SetValue(wxDateTime::Now() - wxTimeSpan::Days(30));
    filterSizer->Add(m_startDatePicker, 0, wxRIGHT, 10);
    
    filterSizer->Add(new wxStaticText(this, wxID_ANY, "To:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_endDatePicker = new wxDatePickerCtrl(this, ID_END_DATE_PICKER);
    m_endDatePicker->SetValue(wxDateTime::Now());
    filterSizer->Add(m_endDatePicker, 0, wxRIGHT, 10);
    
    // Court filter
    filterSizer->Add(new wxStaticText(this, wxID_ANY, "Court:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_courtFilter = new wxChoice(this, ID_COURT_FILTER);
    m_courtFilter->Append("All Courts");
    m_courtFilter->SetSelection(0);
    filterSizer->Add(m_courtFilter, 0, wxRIGHT, 10);
    
    // User filter
    filterSizer->Add(new wxStaticText(this, wxID_ANY, "User:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_userFilter = new wxChoice(this, ID_USER_FILTER);
    m_userFilter->Append("All Users");
    m_userFilter->SetSelection(0);
    filterSizer->Add(m_userFilter, 0, wxRIGHT, 10);
    
    // Status filter
    filterSizer->Add(new wxStaticText(this, wxID_ANY, "Status:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_statusFilter = new wxChoice(this, ID_STATUS_FILTER);
    m_statusFilter->Append("All Status");
    m_statusFilter->Append("Pending");
    m_statusFilter->Append("Confirmed");
    m_statusFilter->Append("Cancelled");
    m_statusFilter->Append("Completed");
    m_statusFilter->SetSelection(0);
    filterSizer->Add(m_statusFilter, 0, wxRIGHT, 10);
    
    // Buttons
    m_refreshBtn = new wxButton(this, ID_REFRESH_DATA, "Refresh");
    filterSizer->Add(m_refreshBtn, 0, wxRIGHT, 5);
    
    m_exportBtn = new wxButton(this, ID_EXPORT_DATA, "Export");
    filterSizer->Add(m_exportBtn, 0);
    
    m_mainSizer->Add(filterSizer, 0, wxEXPAND | wxALL, 10);
}

void AdminPanel::CreateBookingHistoryView()
{
    wxStaticBoxSizer* historySizer = new wxStaticBoxSizer(wxVERTICAL, this, "Booking History (All Users)");
    
    m_bookingHistoryList = new wxListCtrl(this, ID_BOOKING_HISTORY_LIST,
                                         wxDefaultPosition, wxSize(-1, 300),
                                         wxLC_REPORT | wxLC_SINGLE_SEL);
    
    m_bookingHistoryList->AppendColumn("ID", wxLIST_FORMAT_RIGHT, 50);
    m_bookingHistoryList->AppendColumn("Customer", wxLIST_FORMAT_LEFT, 120);
    m_bookingHistoryList->AppendColumn("Court", wxLIST_FORMAT_LEFT, 80);
    m_bookingHistoryList->AppendColumn("Date", wxLIST_FORMAT_LEFT, 100);
    m_bookingHistoryList->AppendColumn("Time", wxLIST_FORMAT_LEFT, 120);
    m_bookingHistoryList->AppendColumn("Status", wxLIST_FORMAT_LEFT, 80);
    m_bookingHistoryList->AppendColumn("Amount", wxLIST_FORMAT_RIGHT, 100);
    m_bookingHistoryList->AppendColumn("Notes", wxLIST_FORMAT_LEFT, 150);
    
    historySizer->Add(m_bookingHistoryList, 1, wxEXPAND | wxALL, 5);
    
    // Management buttons
    wxBoxSizer* mgmtSizer = new wxBoxSizer(wxHORIZONTAL);
    m_cancelBookingBtn = new wxButton(this, ID_ADMIN_CANCEL_BOOKING, "Cancel Selected Booking");
    m_cancelBookingBtn->Enable(false);
    mgmtSizer->Add(m_cancelBookingBtn, 0);
    
    historySizer->Add(mgmtSizer, 0, wxALL | wxALIGN_CENTER, 5);
    
    m_mainSizer->Add(historySizer, 1, wxEXPAND | wxALL, 10);
}

void AdminPanel::CreateStatisticsView()
{
    wxStaticBoxSizer* statsSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Statistics");
    
    // Create statistics labels
    m_totalBookingsLabel = new wxStaticText(this, wxID_ANY, "Total Bookings: 0");
    m_totalRevenueLabel = new wxStaticText(this, wxID_ANY, "Total Revenue: 0 VND");
    m_activeBookingsLabel = new wxStaticText(this, wxID_ANY, "Active Bookings: 0");
    
    statsSizer->Add(m_totalBookingsLabel, 1, wxALL, 10);
    statsSizer->Add(m_totalRevenueLabel, 1, wxALL, 10);
    statsSizer->Add(m_activeBookingsLabel, 1, wxALL, 10);
    
    m_mainSizer->Add(statsSizer, 0, wxEXPAND | wxALL, 10);
}

void AdminPanel::RefreshData()
{
    // Populate court filter
    m_courtFilter->Clear();
    m_courtFilter->Append("All Courts");
    if (m_courtController) {
        try {
            auto courts = m_courtController->getAllCourts();
            for (const auto& court : courts) {
                if (court) {
                    m_courtFilter->Append(court->getName(), 
                                        new wxStringClientData(wxString::Format("%d", court->getId())));
                }
            }
        } catch (const std::exception& e) {
            // Handle error silently
        }
    }
    m_courtFilter->SetSelection(0);
    
    // Populate user filter
    m_userFilter->Clear();
    m_userFilter->Append("All Users");
    if (m_authController) {
        try {
            auto users = m_authController->getAllUsers();
            for (const auto& user : users) {
                if (user) {
                    m_userFilter->Append(wxString::Format("%s (%s)", 
                                                        user->getFullName(),
                                                        user->getEmail()),
                                       new wxStringClientData(wxString::Format("%d", user->getId())));
                }
            }
        } catch (const std::exception& e) {
            // Handle error silently
        }
    }
    m_userFilter->SetSelection(0);
    
    RefreshBookingHistory();
    RefreshStatistics();
}

void AdminPanel::RefreshBookingHistory()
{
    m_bookingHistoryList->DeleteAllItems();
    
    if (!m_bookingController) {
        return;
    }
    
    try {
        auto allBookings = m_bookingController->getAllBookings();
        
        for (size_t i = 0; i < allBookings.size(); ++i) {
            auto booking = allBookings[i];
            if (!booking) continue;
            
            long index = m_bookingHistoryList->InsertItem(i, wxString::Format("%d", booking->getId()));
            
            // Customer name
            wxString customerName = GetUserNameById(booking->getUserId());
            m_bookingHistoryList->SetItem(index, 1, customerName);
            
            // Court name
            wxString courtName = wxString::Format("Court %d", booking->getCourtId());
            if (m_courtController) {
                try {
                    auto courts = m_courtController->getAllCourts();
                    for (const auto& court : courts) {
                        if (court && court->getId() == booking->getCourtId()) {
                            courtName = court->getName();
                            break;
                        }
                    }
                } catch (const std::exception& e) {
                    // Keep default court name
                }
            }
            m_bookingHistoryList->SetItem(index, 2, courtName);
            
            // Date
            wxDateTime bookingDate(booking->getBookingDate());
            m_bookingHistoryList->SetItem(index, 3, bookingDate.Format("%d/%m/%Y"));
            
            // Time
            wxDateTime startTime(booking->getStartTime());
            wxDateTime endTime(booking->getEndTime());
            wxString timeSlot = startTime.Format("%H:%M") + " - " + endTime.Format("%H:%M");
            m_bookingHistoryList->SetItem(index, 4, timeSlot);
            
            // Status
            m_bookingHistoryList->SetItem(index, 5, booking->getStatusString());
            
            // Amount
            m_bookingHistoryList->SetItem(index, 6, FormatCurrency(booking->getTotalAmount()));
            
            // Notes
            m_bookingHistoryList->SetItem(index, 7, booking->getNotes());
            
            // Store booking ID
            m_bookingHistoryList->SetItemData(index, booking->getId());
            
            // Color coding
            if (booking->getStatus() == BookingStatus::CANCELLED) {
                m_bookingHistoryList->SetItemTextColour(index, wxColour(128, 128, 128));
            } else if (booking->getStatus() == BookingStatus::CONFIRMED) {
                m_bookingHistoryList->SetItemTextColour(index, wxColour(0, 128, 0));
            } else if (booking->getStatus() == BookingStatus::PENDING) {
                m_bookingHistoryList->SetItemTextColour(index, wxColour(255, 140, 0));
            }
        }
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error loading booking history: %s", e.what()),
                    "Error", wxOK | wxICON_ERROR, this);
    }
}

void AdminPanel::RefreshStatistics()
{
    if (!m_bookingController) return;
    
    try {
        auto allBookings = m_bookingController->getAllBookings();
        
        int totalBookings = 0;
        int activeBookings = 0;
        int cancelledBookings = 0;
        double totalRevenue = 0.0;
        
        for (const auto& booking : allBookings) {
            if (!booking) continue;
            
            // Only count non-cancelled bookings in total
            if (booking->getStatus() != BookingStatus::CANCELLED) {
                totalBookings++;
                
                if (booking->isActive()) {
                    activeBookings++;
                }
                
                if (booking->getStatus() == BookingStatus::CONFIRMED || 
                    booking->getStatus() == BookingStatus::COMPLETED) {
                    totalRevenue += booking->getTotalAmount();
                }
            } else {
                cancelledBookings++;
            }
        }
        
        m_totalBookingsLabel->SetLabel(wxString::Format("Total Bookings: %d (Cancelled: %d)", totalBookings, cancelledBookings));
        m_totalRevenueLabel->SetLabel(wxString::Format("Total Revenue: %s", FormatCurrency(totalRevenue)));
        m_activeBookingsLabel->SetLabel(wxString::Format("Active Bookings: %d", activeBookings));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error calculating statistics: %s", e.what()),
                    "Error", wxOK | wxICON_ERROR, this);
    }
}

wxString AdminPanel::GetUserNameById(int userId)
{
    if (!m_authController) {
        return wxString::Format("User %d", userId);
    }
    
    try {
        auto users = m_authController->getAllUsers();
        for (const auto& user : users) {
            if (user && user->getId() == userId) {
                return wxString::Format("%s (%s)", user->getFullName(), user->getEmail());
            }
        }
    } catch (const std::exception& e) {
        // Handle error silently
    }
    
    return wxString::Format("User %d", userId);
}

wxString AdminPanel::FormatCurrency(double amount)
{
    wxString amountStr = wxString::Format("%.0f", amount);
    
    wxString result = "";
    int len = amountStr.length();
    for (int i = 0; i < len; i++) {
        if (i > 0 && (len - i) % 3 == 0) {
            result += ",";
        }
        result += amountStr[i];
    }
    
    return result + " VND";
}

void AdminPanel::OnRefreshData(wxCommandEvent& event)
{
    RefreshData();
    wxMessageBox("Data refreshed successfully!", "Information", wxOK | wxICON_INFORMATION, this);
}

void AdminPanel::OnFilterByDate(wxDateEvent& event)
{
    ApplyFilters();
}

void AdminPanel::OnFilterByCourt(wxCommandEvent& event)
{
    ApplyFilters();
}

void AdminPanel::OnFilterByUser(wxCommandEvent& event)
{
    ApplyFilters();
}

void AdminPanel::OnExportData(wxCommandEvent& event)
{
    wxFileDialog dialog(this, "Export Booking Data", "", "booking_history.csv",
                       "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString path = dialog.GetPath();
        
        std::ofstream file(path.ToStdString());
        if (file.is_open()) {
            // Write header
            file << "ID,Customer,Court,Date,Time,Status,Amount,Notes\n";
            
            // Write data
            long itemCount = m_bookingHistoryList->GetItemCount();
            for (long i = 0; i < itemCount; i++) {
                file << m_bookingHistoryList->GetItemText(i, 0) << ","  // ID
                     << m_bookingHistoryList->GetItemText(i, 1) << ","  // Customer
                     << m_bookingHistoryList->GetItemText(i, 2) << ","  // Court
                     << m_bookingHistoryList->GetItemText(i, 3) << ","  // Date
                     << m_bookingHistoryList->GetItemText(i, 4) << ","  // Time
                     << m_bookingHistoryList->GetItemText(i, 5) << ","  // Status
                     << m_bookingHistoryList->GetItemText(i, 6) << ","  // Amount
                     << m_bookingHistoryList->GetItemText(i, 7) << "\n"; // Notes
            }
            
            file.close();
            wxMessageBox("Data exported successfully!", "Export Complete", wxOK | wxICON_INFORMATION, this);
        } else {
            wxMessageBox("Failed to create export file!", "Export Error", wxOK | wxICON_ERROR, this);
        }
    }
}

void AdminPanel::OnBookingSelected(wxListEvent& event)
{
    long item = event.GetIndex();
    if (item != -1) {
        m_selectedBookingId = m_bookingHistoryList->GetItemData(item);
        m_cancelBookingBtn->Enable(true);
    }
}

void AdminPanel::OnCancelBooking(wxCommandEvent& event)
{
    if (m_selectedBookingId == -1) {
        wxMessageBox("Please select a booking to cancel!", "No Selection", wxOK | wxICON_WARNING, this);
        return;
    }
    
    int result = wxMessageBox("Are you sure you want to cancel this booking?\n\n"
                             "This action cannot be undone.",
                             "Confirm Cancellation",
                             wxYES_NO | wxICON_QUESTION, this);
    
    if (result == wxYES) {
        try {
            if (m_bookingController && m_bookingController->cancelBooking(m_selectedBookingId)) {
                wxMessageBox("Booking cancelled successfully!", "Success", wxOK | wxICON_INFORMATION, this);
                RefreshData();
                m_selectedBookingId = -1;
                m_cancelBookingBtn->Enable(false);
            } else {
                wxMessageBox("Failed to cancel booking!", "Error", wxOK | wxICON_ERROR, this);
            }
        } catch (const std::exception& e) {
            wxMessageBox(wxString::Format("Error cancelling booking: %s", e.what()),
                        "Error", wxOK | wxICON_ERROR, this);
        }
    }
}

void AdminPanel::ApplyFilters()
{
    // This method would apply the selected filters to the booking list
    // For now, we'll just refresh the data
    RefreshBookingHistory();
}
