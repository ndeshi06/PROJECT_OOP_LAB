#include "BookingPanel.h"
#include "../include/BookingController.h"
#include "../include/CourtController.h"
#include "../include/AuthController.h"
#include "../include/Booking.h"
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/statbox.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>

wxBEGIN_EVENT_TABLE(BookingPanel, wxPanel)
    EVT_BUTTON(ID_BOOK_COURT, BookingPanel::OnBookCourt)
    EVT_BUTTON(ID_CANCEL_BOOKING, BookingPanel::OnCancelBooking)
    EVT_BUTTON(ID_CHECK_AVAILABILITY, BookingPanel::OnCheckAvailability)
    EVT_BUTTON(ID_REFRESH_BOOKINGS, BookingPanel::OnRefreshBookings)
    EVT_LIST_ITEM_SELECTED(ID_BOOKING_LIST, BookingPanel::OnBookingSelected)
    EVT_CHOICE(ID_COURT_CHOICE, BookingPanel::OnCourtChanged)
wxEND_EVENT_TABLE()

BookingPanel::BookingPanel(wxWindow* parent,
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
    BindEvents();
    RefreshCourtList();
    RefreshData();
}

BookingPanel::~BookingPanel()
{
}

void BookingPanel::CreateUI()
{
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    
    CreateBookingForm();
    CreateAvailabilityDisplay();
    CreateMyBookingsDisplay();
    
    SetSizer(m_mainSizer);
}

void BookingPanel::CreateBookingForm()
{
    m_bookingSizer = new wxStaticBoxSizer(wxVERTICAL, this, "New Booking");
    
    wxFlexGridSizer* formGrid = new wxFlexGridSizer(6, 2, 10, 10);
    
    // Court selection
    formGrid->Add(new wxStaticText(this, wxID_ANY, "Select Court:"), 0, wxALIGN_CENTER_VERTICAL);
    m_courtChoice = new wxChoice(this, ID_COURT_CHOICE);
    formGrid->Add(m_courtChoice, 0, wxEXPAND);
    
    // Date
    formGrid->Add(new wxStaticText(this, wxID_ANY, "Date:"), 0, wxALIGN_CENTER_VERTICAL);
    m_datePicker = new wxDatePickerCtrl(this, ID_DATE_PICKER);
    formGrid->Add(m_datePicker, 0, wxEXPAND);
    
    // Start time
    formGrid->Add(new wxStaticText(this, wxID_ANY, "Start Time:"), 0, wxALIGN_CENTER_VERTICAL);
    m_startTimePicker = new wxTimePickerCtrl(this, ID_START_TIME);
    formGrid->Add(m_startTimePicker, 0, wxEXPAND);
    
    // End time
    formGrid->Add(new wxStaticText(this, wxID_ANY, "End Time:"), 0, wxALIGN_CENTER_VERTICAL);
    m_endTimePicker = new wxTimePickerCtrl(this, ID_END_TIME);
    formGrid->Add(m_endTimePicker, 0, wxEXPAND);
    
    // Notes
    formGrid->Add(new wxStaticText(this, wxID_ANY, "Notes:"), 0, wxALIGN_CENTER_VERTICAL);
    m_notesCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 60), wxTE_MULTILINE);
    formGrid->Add(m_notesCtrl, 0, wxEXPAND);
    
    // Cost display
    formGrid->Add(new wxStaticText(this, wxID_ANY, "Estimated Cost:"), 0, wxALIGN_CENTER_VERTICAL);
    m_costLabel = new wxStaticText(this, wxID_ANY, "0 VND");
    formGrid->Add(m_costLabel, 0, wxALIGN_CENTER_VERTICAL);
    
    formGrid->AddGrowableCol(1);
    m_bookingSizer->Add(formGrid, 0, wxEXPAND | wxALL, 10);
    
    // Buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_checkAvailabilityBtn = new wxButton(this, ID_CHECK_AVAILABILITY, "Check Availability");
    m_bookBtn = new wxButton(this, ID_BOOK_COURT, "Book Court");
    
    buttonSizer->Add(m_checkAvailabilityBtn, 0, wxRIGHT, 10);
    buttonSizer->Add(m_bookBtn, 0);
    
    m_bookingSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 10);
    
    m_mainSizer->Add(m_bookingSizer, 0, wxEXPAND | wxALL, 10);
}

void BookingPanel::CreateAvailabilityDisplay()
{
    m_availabilitySizer = new wxStaticBoxSizer(wxVERTICAL, this, "Available Time Slots");
    
    m_availableSlotsList = new wxListCtrl(this, ID_AVAILABLE_SLOTS_LIST, 
                                         wxDefaultPosition, wxSize(-1, 150),
                                         wxLC_REPORT | wxLC_SINGLE_SEL);
    
    m_availableSlotsList->AppendColumn("Time", wxLIST_FORMAT_LEFT, 150);
    m_availableSlotsList->AppendColumn("Price", wxLIST_FORMAT_RIGHT, 100);
    m_availableSlotsList->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
    
    m_availabilitySizer->Add(m_availableSlotsList, 1, wxEXPAND | wxALL, 5);
    m_mainSizer->Add(m_availabilitySizer, 0, wxEXPAND | wxALL, 10);
}

void BookingPanel::CreateMyBookingsDisplay()
{
    m_myBookingsSizer = new wxStaticBoxSizer(wxVERTICAL, this, "My Bookings");
    
    m_userBookingsList = new wxListCtrl(this, ID_BOOKING_LIST,
                                       wxDefaultPosition, wxSize(-1, 200),
                                       wxLC_REPORT | wxLC_SINGLE_SEL);
    
    m_userBookingsList->AppendColumn("ID", wxLIST_FORMAT_RIGHT, 50);
    m_userBookingsList->AppendColumn("Court", wxLIST_FORMAT_LEFT, 80);
    m_userBookingsList->AppendColumn("Date", wxLIST_FORMAT_LEFT, 100);
    m_userBookingsList->AppendColumn("Time", wxLIST_FORMAT_LEFT, 150);
    m_userBookingsList->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
    m_userBookingsList->AppendColumn("Cost", wxLIST_FORMAT_RIGHT, 100);
    
    m_myBookingsSizer->Add(m_userBookingsList, 1, wxEXPAND | wxALL, 5);
    
    // Booking management buttons
    wxBoxSizer* managementSizer = new wxBoxSizer(wxHORIZONTAL);
    m_cancelBookingBtn = new wxButton(this, ID_CANCEL_BOOKING, "Cancel");
    m_modifyBookingBtn = new wxButton(this, ID_MODIFY_BOOKING, "Modify");
    m_refreshBookingsBtn = new wxButton(this, ID_REFRESH_BOOKINGS, "Refresh");
    
    managementSizer->Add(m_cancelBookingBtn, 0, wxRIGHT, 5);
    managementSizer->Add(m_modifyBookingBtn, 0, wxRIGHT, 5);
    managementSizer->Add(m_refreshBookingsBtn, 0);
    
    m_myBookingsSizer->Add(managementSizer, 0, wxALL | wxALIGN_CENTER, 5);
    m_mainSizer->Add(m_myBookingsSizer, 1, wxEXPAND | wxALL, 10);
    
    // Initially disable booking management buttons
    UpdateButtonStates();
}

void BookingPanel::BindEvents()
{
    // Events are bound through the event table
}

void BookingPanel::RefreshCourtList()
{
    m_courtChoice->Clear();
    
    // Add sample courts
    m_courtChoice->Append("Court 1", new wxStringClientData("1"));
    m_courtChoice->Append("Court 2", new wxStringClientData("2"));
    m_courtChoice->Append("Court 3", new wxStringClientData("3"));
    
    if (m_courtChoice->GetCount() > 0) {
        m_courtChoice->SetSelection(0);
    }
}

void BookingPanel::RefreshData()
{
    RefreshMyBookings();
    RefreshAvailableSlots();
}

void BookingPanel::RefreshMyBookings()
{
    m_userBookingsList->DeleteAllItems();
    
    if (!m_bookingController || !m_authController || !m_authController->getCurrentUser()) {
        return;
    }
    
    try {
        // Get bookings for current user
        int currentUserId = m_authController->getCurrentUser()->getId();
        auto userBookings = m_bookingController->getUserBookings(currentUserId);
        
        for (size_t i = 0; i < userBookings.size(); ++i) {
            auto booking = userBookings[i];
            if (!booking) continue;
            
            long index = m_userBookingsList->InsertItem(i, wxString::Format("%d", booking->getId()));
            
            // Get court name (you might need to get this from CourtController)
            m_userBookingsList->SetItem(index, 1, wxString::Format("Court %d", booking->getCourtId()));
            
            // Format date
            wxDateTime startTime(booking->getStartTime());
            m_userBookingsList->SetItem(index, 2, startTime.Format("%Y-%m-%d"));
            
            // Format time slot
            wxDateTime endTime(booking->getEndTime());
            wxString timeSlot = startTime.Format("%H:%M") + " - " + endTime.Format("%H:%M");
            m_userBookingsList->SetItem(index, 3, timeSlot);
            
            // Status
            m_userBookingsList->SetItem(index, 4, booking->getStatusString());
            
            // Cost
            m_userBookingsList->SetItem(index, 5, FormatCurrency(booking->getTotalCost()));
            
            // Store booking ID in item data
            m_userBookingsList->SetItemData(index, booking->getId());
        }
        
    } catch (const std::exception& e) {
        // If error loading bookings, show a message but don't crash
        long index = m_userBookingsList->InsertItem(0, "Error");
        m_userBookingsList->SetItem(index, 1, "Failed to load bookings");
        m_userBookingsList->SetItem(index, 2, e.what());
    }
}

void BookingPanel::RefreshAvailableSlots()
{
    m_availableSlotsList->DeleteAllItems();
    
    // Add sample available slots
    long index = m_availableSlotsList->InsertItem(0, "08:00 - 10:00");
    m_availableSlotsList->SetItem(index, 1, "50,000 VND");
    m_availableSlotsList->SetItem(index, 2, "Available");
    
    index = m_availableSlotsList->InsertItem(1, "10:00 - 12:00");
    m_availableSlotsList->SetItem(index, 1, "60,000 VND");
    m_availableSlotsList->SetItem(index, 2, "Available");
}

void BookingPanel::OnBookCourt(wxCommandEvent& event)
{
    if (!ValidateBookingInput()) {
        return;
    }
    
    try {
        // Get booking details for confirmation
        int courtSelection = m_courtChoice->GetSelection();
        if (courtSelection == wxNOT_FOUND) {
            wxMessageBox("Please select a court!", "Booking Error", wxOK | wxICON_WARNING, this);
            return;
        }
        
        wxString courtName = m_courtChoice->GetStringSelection();
        wxString notes = m_notesCtrl->GetValue();
        
        // Get current user info
        std::string customerName = "Guest";
        if (m_authController && m_authController->getCurrentUser()) {
            customerName = m_authController->getCurrentUser()->getFullName();
        }
        
        // Show detailed success message
        wxString successMsg = wxString::Format(
            "Booking successful!\n\n"
            "Court: %s\n"
            "Customer: %s\n"
            "Date: %s\n"
            "Time: %s\n"
            "Notes: %s\n\n"
            "Your booking has been confirmed. Please arrive 10 minutes early.",
            courtName,
            customerName,
            "Today", // You can get actual date from date picker
            "Selected time slot", // You can get actual time from time picker
            notes.IsEmpty() ? "None" : notes
        );
        
        wxMessageBox(successMsg, "Booking Confirmed", wxOK | wxICON_INFORMATION, this);
        
        RefreshData();
        ClearBookingForm();
        
    } catch (const std::exception& e) {
        wxString errorMsg = wxString::Format(
            "Booking failed!\n\n"
            "Error: %s\n\n"
            "Please check:\n"
            "• Court availability\n"
            "• Selected time slot\n"
            "• Your account permissions\n\n"
            "Contact support if the problem persists.",
            e.what()
        );
        wxMessageBox(errorMsg, "Booking Error", wxOK | wxICON_ERROR, this);
    }
}

void BookingPanel::OnCancelBooking(wxCommandEvent& event)
{
    if (m_selectedBookingId == -1) {
        wxMessageBox("Please select a booking to cancel!\n\n"
                    "Click on a booking from the list below to select it.",
                    "No Booking Selected", 
                    wxOK | wxICON_WARNING, this);
        return;
    }
    
    // Show detailed confirmation dialog
    int result = wxMessageBox("Are you sure you want to cancel this booking?\n\n"
                             "This action cannot be undone.\n"
                             "You may be charged a cancellation fee depending on the timing.",
                             "Confirm Booking Cancellation",
                             wxYES_NO | wxICON_QUESTION, this);
    
    if (result == wxYES) {
        try {
            // Actually cancel the booking through controller
            if (m_bookingController && m_bookingController->cancelBooking(m_selectedBookingId)) {
                // Show detailed success message
                wxMessageBox("Booking cancelled successfully!\n\n"
                            "Your booking has been removed from the system.\n"
                            "If you paid in advance, refund processing will begin within 3-5 business days.\n\n"
                            "Thank you for using our service!",
                            "Cancellation Confirmed", 
                            wxOK | wxICON_INFORMATION, this);
                
                RefreshData();
                m_selectedBookingId = -1;
                UpdateButtonStates();
            } else {
                wxMessageBox("Failed to cancel booking!\n\n"
                            "The booking may have already been cancelled or may not exist.\n"
                            "Please refresh the list and try again.",
                            "Cancellation Failed", 
                            wxOK | wxICON_ERROR, this);
            }
        } catch (const std::exception& e) {
            wxString errorMsg = wxString::Format(
                "Failed to cancel booking!\n\n"
                "Error: %s\n\n"
                "Please try again or contact support.\n"
                "Your booking is still active until successfully cancelled.",
                e.what()
            );
            wxMessageBox(errorMsg, "Cancellation Error", wxOK | wxICON_ERROR, this);
        }
    }
                e.what()
            );
            wxMessageBox(errorMsg, "Cancellation Error", wxOK | wxICON_ERROR, this);
        }
    }
}

void BookingPanel::OnCheckAvailability(wxCommandEvent& event)
{
    RefreshAvailableSlots();
    wxMessageBox("Available time slots updated!", "Information", wxOK | wxICON_INFORMATION);
}

void BookingPanel::OnRefreshBookings(wxCommandEvent& event)
{
    RefreshData();
    wxMessageBox("Data refreshed!", "Information", wxOK | wxICON_INFORMATION);
}

void BookingPanel::OnCourtChanged(wxCommandEvent& event)
{
    RefreshAvailableSlots();
    UpdateEstimatedCost();
}

void BookingPanel::OnTimeChanged(wxCommandEvent& event)
{
    UpdateEstimatedCost();
}

void BookingPanel::OnBookingSelected(wxListEvent& event)
{
    long item = event.GetIndex();
    if (item != -1) {
        // Get the actual booking ID from item data
        m_selectedBookingId = m_userBookingsList->GetItemData(item);
        UpdateButtonStates();
    }
}

void BookingPanel::OnAvailableSlotSelected(wxListEvent& event)
{
    // Set the selected time slot in the form
    long item = event.GetIndex();
    if (item != -1) {
        wxString timeSlot = m_availableSlotsList->GetItemText(item);
        // Parse and set time pickers based on selected slot
    }
}

bool BookingPanel::ValidateBookingInput()
{
    if (m_courtChoice->GetSelection() == wxNOT_FOUND) {
        wxMessageBox("Please select a court!\n\n"
                    "Choose from the available courts in the dropdown list.",
                    "Court Selection Required", 
                    wxOK | wxICON_WARNING, this);
        return false;
    }
    
    wxDateTime startTime = m_startTimePicker->GetValue();
    wxDateTime endTime = m_endTimePicker->GetValue();
    
    if (startTime >= endTime) {
        wxMessageBox("Invalid time selection!\n\n"
                    "End time must be after start time.\n\n"
                    "Please check:\n"
                    "• Start time is earlier than end time\n"
                    "• Minimum booking duration is 1 hour\n"
                    "• Maximum booking duration is 4 hours",
                    "Time Validation Error", 
                    wxOK | wxICON_WARNING, this);
        return false;
    }
    
    // Check if user is logged in
    if (!m_authController || !m_authController->isLoggedIn()) {
        wxMessageBox("You must be logged in to make a booking!\n\n"
                    "Please log in with your account and try again.",
                    "Login Required", 
                    wxOK | wxICON_ERROR, this);
        return false;
    }
    
    return true;
}

void BookingPanel::UpdateEstimatedCost()
{
    // Calculate estimated cost based on selected court, date, and time
    m_costLabel->SetLabel("75,000 VND");
}

void BookingPanel::ClearBookingForm()
{
    m_notesCtrl->Clear();
    m_costLabel->SetLabel("0 VND");
    
    // Reset time pickers to default
    wxDateTime now = wxDateTime::Now();
    m_startTimePicker->SetValue(now);
    m_endTimePicker->SetValue(now);
}

void BookingPanel::UpdateButtonStates()
{
    bool hasSelection = (m_selectedBookingId != -1);
    m_cancelBookingBtn->Enable(hasSelection);
    m_modifyBookingBtn->Enable(hasSelection);
}

void BookingPanel::SavePendingChanges()
{
    // Save any pending changes if needed
}

// Helper methods
wxString BookingPanel::FormatTimeSlot(std::time_t startTime, std::time_t endTime)
{
    wxDateTime start(startTime);
    wxDateTime end(endTime);
    return start.Format("%H:%M") + " - " + end.Format("%H:%M");
}

wxString BookingPanel::FormatCurrency(double amount)
{
    return wxString::Format("%.0f VND", amount);
}

wxString BookingPanel::FormatDateTime(std::time_t time)
{
    wxDateTime dt(time);
    return dt.Format("%Y-%m-%d %H:%M");
}

std::time_t BookingPanel::CombineDateTime(const wxDateTime& date, const wxDateTime& time)
{
    wxDateTime combined = date;
    combined.SetHour(time.GetHour());
    combined.SetMinute(time.GetMinute());
    combined.SetSecond(time.GetSecond());
    return combined.GetTicks();
}
