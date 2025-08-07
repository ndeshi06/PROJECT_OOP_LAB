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
#include <map>

wxBEGIN_EVENT_TABLE(BookingPanel, wxPanel)
    EVT_BUTTON(ID_BOOK_COURT, BookingPanel::OnBookCourt)
    EVT_BUTTON(ID_CANCEL_BOOKING, BookingPanel::OnCancelBooking)
    EVT_BUTTON(ID_MODIFY_BOOKING, BookingPanel::OnModifyBooking)
    EVT_BUTTON(ID_CHECK_AVAILABILITY, BookingPanel::OnCheckAvailability)
    EVT_BUTTON(ID_REFRESH_BOOKINGS, BookingPanel::OnRefreshBookings)
    EVT_LIST_ITEM_SELECTED(ID_BOOKING_LIST, BookingPanel::OnBookingSelected)
    EVT_LIST_ITEM_SELECTED(ID_AVAILABLE_SLOTS_LIST, BookingPanel::OnAvailableSlotSelected)
    EVT_CHOICE(ID_COURT_CHOICE, BookingPanel::OnCourtChanged)
    EVT_TIME_CHANGED(ID_START_TIME, BookingPanel::OnTimeChanged)
    EVT_TIME_CHANGED(ID_END_TIME, BookingPanel::OnTimeChanged)
    EVT_DATE_CHANGED(ID_DATE_PICKER, BookingPanel::OnDateChanged)
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
    UpdateEstimatedCost(); // Calculate initial cost
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
    
    wxFlexGridSizer* formGrid = new wxFlexGridSizer(7, 2, 10, 10);
    
    // Court selection
    formGrid->Add(new wxStaticText(this, wxID_ANY, "Select Court:"), 0, wxALIGN_CENTER_VERTICAL);
    m_courtChoice = new wxChoice(this, ID_COURT_CHOICE);
    formGrid->Add(m_courtChoice, 0, wxEXPAND);
    
    // Date
    formGrid->Add(new wxStaticText(this, wxID_ANY, "Date:"), 0, wxALIGN_CENTER_VERTICAL);
    m_datePicker = new wxDatePickerCtrl(this, ID_DATE_PICKER, wxDefaultDateTime, 
                                       wxDefaultPosition, wxDefaultSize, 
                                       wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    formGrid->Add(m_datePicker, 0, wxEXPAND);
    
    // Start time
    formGrid->Add(new wxStaticText(this, wxID_ANY, "Start Time:"), 0, wxALIGN_CENTER_VERTICAL);
    m_startTimePicker = new wxTimePickerCtrl(this, ID_START_TIME);
    formGrid->Add(m_startTimePicker, 0, wxEXPAND);
    
    // End time
    formGrid->Add(new wxStaticText(this, wxID_ANY, "End Time:"), 0, wxALIGN_CENTER_VERTICAL);
    m_endTimePicker = new wxTimePickerCtrl(this, ID_END_TIME);
    formGrid->Add(m_endTimePicker, 0, wxEXPAND);
    
    // Add help text for time selection
    formGrid->Add(new wxStaticText(this, wxID_ANY, ""), 0); // Empty cell
    wxStaticText* helpText = new wxStaticText(this, wxID_ANY, 
        "Tip: Click on available time slots below to auto-select times");
    helpText->SetForegroundColour(wxColour(100, 100, 100));
    formGrid->Add(helpText, 0, wxALIGN_LEFT);
    
    // Set default time values
    wxDateTime now = wxDateTime::Now();
    wxDateTime defaultStart = now;
    defaultStart.SetMinute(0);
    defaultStart.SetSecond(0);
    if (now.GetMinute() > 0) {
        defaultStart += wxTimeSpan::Hours(1);
    }
    wxDateTime defaultEnd = defaultStart + wxTimeSpan::Hours(2);
    
    m_startTimePicker->SetValue(defaultStart);
    m_endTimePicker->SetValue(defaultEnd);
    
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
    
    m_availableSlotsList->AppendColumn("Time Slot", wxLIST_FORMAT_LEFT, 120);
    m_availableSlotsList->AppendColumn("Price", wxLIST_FORMAT_RIGHT, 100);
    m_availableSlotsList->AppendColumn("Status & Details", wxLIST_FORMAT_LEFT, 200);
    
    m_availabilitySizer->Add(m_availableSlotsList, 1, wxEXPAND | wxALL, 5);
    
    // Add instruction text
    wxStaticText* instructionText = new wxStaticText(this, wxID_ANY, 
        "Click on an available time slot below to select it automatically:");
    instructionText->SetFont(instructionText->GetFont().Bold());
    m_availabilitySizer->Insert(0, instructionText, 0, wxALL, 5);
    
    m_mainSizer->Add(m_availabilitySizer, 0, wxEXPAND | wxALL, 10);
}

void BookingPanel::CreateMyBookingsDisplay()
{
    m_myBookingsSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Booking History");
    
    m_userBookingsList = new wxListCtrl(this, ID_BOOKING_LIST,
                                       wxDefaultPosition, wxSize(-1, 250),
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
    m_refreshBookingsBtn = new wxButton(this, ID_REFRESH_BOOKINGS, "Refresh History");
    
    managementSizer->Add(m_cancelBookingBtn, 0, wxRIGHT, 5);
    managementSizer->Add(m_modifyBookingBtn, 0, wxRIGHT, 5);
    managementSizer->Add(m_refreshBookingsBtn, 0);
    
    m_myBookingsSizer->Add(managementSizer, 0, wxALL | wxALIGN_CENTER, 5);
    m_mainSizer->Add(m_myBookingsSizer, 2, wxEXPAND | wxALL, 10);
    
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
    
    if (!m_courtController) {
        return;
    }
    
    try {
        auto courts = m_courtController->getAllCourts();
        for (const auto& court : courts) {
            if (court) {
                wxString courtName = wxString::Format("%s", court->getName());
                m_courtChoice->Append(courtName, new wxStringClientData(wxString::Format("%d", court->getId())));
            }
        }
        
        if (m_courtChoice->GetCount() > 0) {
            m_courtChoice->SetSelection(0);
        }
    } catch (const std::exception& e) {
        // Fallback to default courts if loading fails
        m_courtChoice->Append("Court 1", new wxStringClientData("1"));
        m_courtChoice->Append("Court 2", new wxStringClientData("2"));
        m_courtChoice->Append("Court 3", new wxStringClientData("3"));
        if (m_courtChoice->GetCount() > 0) {
            m_courtChoice->SetSelection(0);
        }
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
    
    if (!m_bookingController) {
        // Show message if no booking controller
        long index = m_userBookingsList->InsertItem(0, "Error");
        m_userBookingsList->SetItem(index, 1, "No booking controller");
        m_userBookingsList->SetItem(index, 2, "System error");
        return;
    }
    
    if (!m_authController) {
        // Show message if no auth controller
        long index = m_userBookingsList->InsertItem(0, "Error");
        m_userBookingsList->SetItem(index, 1, "No authentication");
        m_userBookingsList->SetItem(index, 2, "Please login");
        return;
    }
    
    auto currentUser = m_authController->getCurrentUser();
    if (!currentUser) {
        // Show message if user not logged in
        long index = m_userBookingsList->InsertItem(0, "Info");
        m_userBookingsList->SetItem(index, 1, "Not logged in");
        m_userBookingsList->SetItem(index, 2, "Please login to view bookings");
        m_userBookingsList->SetItem(index, 3, "");
        m_userBookingsList->SetItem(index, 4, "");
        m_userBookingsList->SetItem(index, 5, "");
        return;
    }
    
    try {
        // Get bookings for current user
        int currentUserId = currentUser->getId();
        auto userBookings = m_bookingController->getUserBookings(currentUserId);
        
        if (userBookings.empty()) {
            // Show message if no bookings found
            long index = m_userBookingsList->InsertItem(0, "Info");
            m_userBookingsList->SetItem(index, 1, "No booking history");
            m_userBookingsList->SetItem(index, 2, "You haven't made any bookings yet");
            m_userBookingsList->SetItem(index, 3, "Click 'Book Court' to make your first booking");
            m_userBookingsList->SetItem(index, 4, "");
            m_userBookingsList->SetItem(index, 5, "");
            return;
        }
        
        // Display all bookings (including cancelled ones for history)
        for (size_t i = 0; i < userBookings.size(); ++i) {
            auto booking = userBookings[i];
            if (!booking) continue;
            
            long index = m_userBookingsList->InsertItem(i, wxString::Format("%d", booking->getId()));
            
            // Get court name from CourtController if available
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
                    // Keep default court name if error
                }
            }
            m_userBookingsList->SetItem(index, 1, courtName);
            
            // Format date - Vietnamese format: DD/MM/YYYY
            wxDateTime startTime(booking->getStartTime());
            m_userBookingsList->SetItem(index, 2, startTime.Format("%d/%m/%Y"));
            
            // Format time slot
            wxDateTime endTime(booking->getEndTime());
            wxString timeSlot = startTime.Format("%H:%M") + " - " + endTime.Format("%H:%M");
            m_userBookingsList->SetItem(index, 3, timeSlot);
            
            // Status
            m_userBookingsList->SetItem(index, 4, booking->getStatusString());
            
            // Cost
            m_userBookingsList->SetItem(index, 5, FormatCurrency(booking->getTotalAmount()));
            
            // Store booking ID in item data
            m_userBookingsList->SetItemData(index, booking->getId());
            
            // Color code by status
            if (booking->getStatus() == BookingStatus::CANCELLED) {
                m_userBookingsList->SetItemTextColour(index, wxColour(128, 128, 128)); // Gray for cancelled
            } else if (booking->getStatus() == BookingStatus::CONFIRMED) {
                m_userBookingsList->SetItemTextColour(index, wxColour(0, 128, 0)); // Green for confirmed
            } else if (booking->getStatus() == BookingStatus::PENDING) {
                m_userBookingsList->SetItemTextColour(index, wxColour(255, 140, 0)); // Orange for pending
            }
        }
        
    } catch (const std::exception& e) {
        // If error loading bookings, show detailed error message
        long index = m_userBookingsList->InsertItem(0, "Error");
        m_userBookingsList->SetItem(index, 1, "Failed to load bookings");
        m_userBookingsList->SetItem(index, 2, wxString::Format("Error: %s", e.what()));
        m_userBookingsList->SetItem(index, 3, "Try refreshing or contact support");
        m_userBookingsList->SetItem(index, 4, "");
        m_userBookingsList->SetItem(index, 5, "");
    }
}

void BookingPanel::RefreshAvailableSlots()
{
    m_availableSlotsList->DeleteAllItems();
    
    // Get selected court
    int courtSelection = m_courtChoice->GetSelection();
    if (courtSelection == wxNOT_FOUND) {
        return; // No court selected
    }
    
    // Get court ID
    wxStringClientData* clientData = dynamic_cast<wxStringClientData*>(m_courtChoice->GetClientObject(courtSelection));
    if (!clientData) {
        return;
    }
    
    long courtId;
    if (!clientData->GetData().ToLong(&courtId)) {
        return;
    }
    
    // Get selected date
    wxDateTime selectedDate = m_datePicker->GetValue();
    
    // Get court's hourly rate
    double hourlyRate = 50000.0; // Default rate
    if (m_courtController) {
        try {
            auto courts = m_courtController->getAllCourts();
            for (const auto& court : courts) {
                if (court && court->getId() == (int)courtId) {
                    hourlyRate = court->getHourlyRate();
                    break;
                }
            }
        } catch (const std::exception& e) {
            // Use default rate if error
            hourlyRate = 50000.0;
        }
    }
    
    // Get all bookings for this court on this date
    auto allBookings = m_bookingController->getAllBookings();
    std::vector<std::pair<int, int>> bookedSlots; // start minutes, end minutes
    std::map<std::pair<int, int>, std::shared_ptr<Booking>> slotBookingMap; // map slot to booking details
    
    for (const auto& booking : allBookings) {
        if (booking && booking->getCourtId() == courtId && 
            booking->getStatus() != BookingStatus::CANCELLED) {
            
            // Check if booking is on the selected date
            wxDateTime bookingDate(booking->getBookingDate());
            if (bookingDate.IsSameDate(selectedDate)) {
                wxDateTime startTime(booking->getStartTime());
                wxDateTime endTime(booking->getEndTime());
                
                int startMinutes = startTime.GetHour() * 60 + startTime.GetMinute();
                int endMinutes = endTime.GetHour() * 60 + endTime.GetMinute();
                
                std::pair<int, int> timeSlot = {startMinutes, endMinutes};
                bookedSlots.push_back(timeSlot);
                slotBookingMap[timeSlot] = booking; // Store booking details for this slot
            }
        }
    }
    
    // Generate time slots from 6:00 to 22:00 (2-hour slots)
    wxDateTime now = wxDateTime::Now();
    
    for (int hour = 6; hour < 22; hour += 2) {
        int startMinutes = hour * 60;
        int endMinutes = (hour + 2) * 60;
        
        // Check if this time slot is in the past or too close to current time
        wxDateTime slotStartTime = selectedDate;
        slotStartTime.SetHour(hour);
        slotStartTime.SetMinute(0);
        slotStartTime.SetSecond(0);
        
        wxDateTime oneHourFromNow = now;
        oneHourFromNow.Add(wxTimeSpan::Hours(1));
        
        bool isPastTime = slotStartTime <= oneHourFromNow; // Must be at least 1 hour from now
        
        // Check if this slot conflicts with any booking
        bool isAvailable = !isPastTime; // Not available if in the past
        std::shared_ptr<Booking> conflictingBooking = nullptr;
        
        if (!isPastTime) {
            for (const auto& bookedSlot : bookedSlots) {
                // Check for time overlap
                if (!(endMinutes <= bookedSlot.first || startMinutes >= bookedSlot.second)) {
                    isAvailable = false;
                    // Find the booking details for this conflicting slot
                    auto it = slotBookingMap.find(bookedSlot);
                    if (it != slotBookingMap.end()) {
                        conflictingBooking = it->second;
                    }
                    break;
                }
            }
        }
        
        // Calculate cost per slot (2 hours * court's hourly rate)
        double cost = 2.0 * hourlyRate;
        
        wxString timeSlot = wxString::Format("%02d:00 - %02d:00", hour, hour + 2);
        wxString status;
        
        if (isAvailable) {
            status = "Available";
        } else {
            // Distinguish between past time and actually booked
            if (isPastTime) {
                status = "Not Available";
            } else if (conflictingBooking) {
                wxDateTime bookingStart(conflictingBooking->getStartTime());
                wxDateTime bookingEnd(conflictingBooking->getEndTime());
                status = wxString::Format("Booked (%s - %s) - ID: %d", 
                    bookingStart.Format("%H:%M"),
                    bookingEnd.Format("%H:%M"),
                    conflictingBooking->getId());
            } else {
                status = "Not Available";
            }
        }
        
        long index = m_availableSlotsList->InsertItem(m_availableSlotsList->GetItemCount(), timeSlot);
        m_availableSlotsList->SetItem(index, 1, wxString::Format("%.0f VND", cost));
        m_availableSlotsList->SetItem(index, 2, status);
        
        // Change text color and background for different statuses
        if (isAvailable) {
            m_availableSlotsList->SetItemTextColour(index, wxColour(0, 128, 0)); // Green text for available
            m_availableSlotsList->SetItemBackgroundColour(index, wxColour(240, 255, 240)); // Light green background
        } else if (isPastTime) {
            m_availableSlotsList->SetItemTextColour(index, wxColour(160, 160, 160)); // Light gray text for past time
            m_availableSlotsList->SetItemBackgroundColour(index, wxColour(250, 250, 250)); // Very light gray background
        } else if (conflictingBooking) {
            m_availableSlotsList->SetItemTextColour(index, wxColour(180, 0, 0)); // Dark red text for booked
            m_availableSlotsList->SetItemBackgroundColour(index, wxColour(255, 240, 240)); // Light red background
        } else {
            m_availableSlotsList->SetItemTextColour(index, wxColour(128, 128, 128)); // Gray text for other unavailable
            m_availableSlotsList->SetItemBackgroundColour(index, wxColour(248, 248, 248)); // Light gray background
        }
    }
}

void BookingPanel::OnBookCourt(wxCommandEvent& event)
{
    if (!ValidateBookingInput()) {
        return;
    }
    
    try {
        // Get booking details
        int courtSelection = m_courtChoice->GetSelection();
        if (courtSelection == wxNOT_FOUND) {
            wxMessageBox("Please select a court!", "Booking Error", wxOK | wxICON_WARNING, this);
            return;
        }
        
        // Get court ID from selection
        // Get court ID from client data
        wxStringClientData* clientData = dynamic_cast<wxStringClientData*>(m_courtChoice->GetClientObject(courtSelection));
        if (!clientData) {
            wxMessageBox("Invalid court selection!", "Booking Error", wxOK | wxICON_WARNING, this);
            return;
        }
        
        long courtId;
        if (!clientData->GetData().ToLong(&courtId)) {
            wxMessageBox("Invalid court ID!", "Booking Error", wxOK | wxICON_WARNING, this);
            return;
        }
        wxString courtName = m_courtChoice->GetStringSelection();
        wxString notes = m_notesCtrl->GetValue();
        
        // Get current user
        auto currentUser = m_authController->getCurrentUser();
        if (!currentUser) {
            wxMessageBox("Please login to make a booking!", "Booking Error", wxOK | wxICON_WARNING, this);
            return;
        }
        
        // Get booking date and time from pickers
        wxDateTime bookingDate = m_datePicker->GetValue();
        wxDateTime startTime = m_startTimePicker->GetValue();
        wxDateTime endTime = m_endTimePicker->GetValue();
        
        // Combine date with times
        std::time_t bookingDateTime = CombineDateTime(bookingDate, bookingDate);
        std::time_t startDateTime = CombineDateTime(bookingDate, startTime);
        std::time_t endDateTime = CombineDateTime(bookingDate, endTime);
        
        // Create booking through controller
        bool bookingSuccess = m_bookingController->createBooking(
            currentUser->getId(),
            (int)courtId,
            bookingDateTime,
            startDateTime,
            endDateTime,
            notes.ToStdString()
        );
        
        if (bookingSuccess) {
            // Show detailed success message
            wxString successMsg = wxString::Format(
                "Booking successful!\n\n"
                "Court: %s\n"
                "Customer: %s (ID: %d)\n"
                "Date: %s\n"
                "Time: %s - %s\n"
                "Notes: %s\n\n"
                "Your booking has been confirmed. Please arrive 10 minutes early.",
                courtName,
                currentUser->getFullName(),
                currentUser->getId(),  // Add user ID for debugging
                bookingDate.Format("%d/%m/%Y"),
                startTime.Format("%H:%M"),
                endTime.Format("%H:%M"),
                notes.IsEmpty() ? "None" : notes
            );
            
            wxMessageBox(successMsg, "Booking Confirmed", wxOK | wxICON_INFORMATION, this);
            
            // Force refresh all data
            RefreshData();
            RefreshMyBookings(); // Force refresh my bookings specifically
            RefreshAvailableSlots(); // Also refresh slots to show the new booking
            ClearBookingForm();
        } else {
            wxMessageBox("Failed to create booking. Please try again.", "Booking Error", wxOK | wxICON_ERROR, this);
        }
        
    } catch (const std::exception& e) {
        wxString errorMsg = wxString::Format(
            "Booking failed!\n\n"
            "Error: %s\n\n"
            "Please check:\n"
            "- Court availability\n"
            "- Selected time slot\n"
            "- Your account permissions\n\n"
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
                    "Click on a booking from the history list below to select it.",
                    "No Booking Selected", 
                    wxOK | wxICON_WARNING, this);
        return;
    }
    
    // Check if booking can be cancelled
    if (!m_bookingController || !m_authController) {
        wxMessageBox("System error: Controllers not available!", 
                    "Error", wxOK | wxICON_ERROR, this);
        return;
    }
    
    try {
        auto userBookings = m_bookingController->getUserBookings(
            m_authController->getCurrentUser()->getId()
        );
        
        std::shared_ptr<Booking> selectedBooking = nullptr;
        for (const auto& booking : userBookings) {
            if (booking && booking->getId() == m_selectedBookingId) {
                selectedBooking = booking;
                break;
            }
        }
        
        if (!selectedBooking) {
            wxMessageBox("Selected booking not found!", 
                        "Error", wxOK | wxICON_ERROR, this);
            return;
        }
        
        // Check if booking can be cancelled
        if (selectedBooking->getStatus() == BookingStatus::CANCELLED) {
            wxMessageBox("This booking has already been cancelled!", 
                        "Already Cancelled", wxOK | wxICON_WARNING, this);
            return;
        }
        
        if (selectedBooking->getStatus() == BookingStatus::COMPLETED) {
            wxMessageBox("Cannot cancel a completed booking!", 
                        "Cannot Cancel", wxOK | wxICON_WARNING, this);
            return;
        }
        
        // Show detailed confirmation dialog
        wxString confirmMsg = wxString::Format(
            "Are you sure you want to cancel this booking?\n\n"
            "Booking ID: %d\n"
            "Court: %s\n"
            "Date: %s\n"
            "Status: %s\n\n"
            "This action cannot be undone.\n"
            "You may be charged a cancellation fee depending on the timing.",
            selectedBooking->getId(),
            wxString::Format("Court %d", selectedBooking->getCourtId()),
            wxDateTime(selectedBooking->getBookingDate()).Format("%d/%m/%Y"),
            selectedBooking->getStatusString()
        );
        
        int result = wxMessageBox(confirmMsg, "Confirm Booking Cancellation",
                                 wxYES_NO | wxICON_QUESTION, this);
        
        if (result == wxYES) {
            if (m_bookingController->cancelBooking(m_selectedBookingId)) {
                wxMessageBox("Booking cancelled successfully!\n\n"
                            "Your booking has been cancelled and moved to history.\n"
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
                            "Please refresh the history and try again.",
                            "Cancellation Failed", 
                            wxOK | wxICON_ERROR, this);
            }
        }
        
    } catch (const std::exception& e) {
        wxString errorMsg = wxString::Format(
            "Failed to cancel booking!\n\n"
            "Error: %s\n\n"
            "Please try again or contact support.\n"
            "Your booking status remains unchanged.",
            e.what()
        );
        wxMessageBox(errorMsg, "Cancellation Error", wxOK | wxICON_ERROR, this);
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
    wxMessageBox("Booking history refreshed!", "Information", wxOK | wxICON_INFORMATION);
}

void BookingPanel::OnModifyBooking(wxCommandEvent& event)
{
    if (m_selectedBookingId == -1) {
        wxMessageBox("Please select a booking to modify!\n\n"
                    "Click on a booking from the list below to select it.",
                    "No Booking Selected", 
                    wxOK | wxICON_WARNING, this);
        return;
    }
    
    wxMessageBox("Modify booking functionality will be implemented in the next update.\n\n"
                "Currently you can:\n"
                "- Cancel existing bookings\n"
                "- Create new bookings\n"
                "- View all your bookings",
                "Feature Coming Soon", 
                wxOK | wxICON_INFORMATION, this);
}

void BookingPanel::OnCourtChanged(wxCommandEvent& event)
{
    RefreshAvailableSlots();
    UpdateEstimatedCost();
}

void BookingPanel::OnTimeChanged(wxDateEvent& event)
{
    UpdateEstimatedCost();
}

void BookingPanel::OnDateChanged(wxDateEvent& event)
{
    RefreshAvailableSlots();
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
        wxString status = m_availableSlotsList->GetItemText(item, 2); // Status column
        
        // Only allow selection of available slots
        if (status != "Available") {
            wxMessageBox("This time slot is not available!\n\n"
                        "Please select an available time slot (green text).",
                        "Slot Not Available", 
                        wxOK | wxICON_WARNING, this);
            return;
        }
        
        // Parse time slot (format: "HH:MM - HH:MM")
        wxString startTimeStr, endTimeStr;
        if (timeSlot.Contains(" - ")) {
            startTimeStr = timeSlot.BeforeFirst(' ');
            endTimeStr = timeSlot.AfterLast(' ');
            
            // Parse hours and minutes
            long startHour, startMin, endHour, endMin;
            if (startTimeStr.BeforeFirst(':').ToLong(&startHour) && 
                startTimeStr.AfterFirst(':').ToLong(&startMin) &&
                endTimeStr.BeforeFirst(':').ToLong(&endHour) && 
                endTimeStr.AfterFirst(':').ToLong(&endMin)) {
                
                // Get current date from date picker
                wxDateTime selectedDate = m_datePicker->GetValue();
                
                // Create start and end times
                wxDateTime startTime = selectedDate;
                startTime.SetHour(startHour);
                startTime.SetMinute(startMin);
                startTime.SetSecond(0);
                
                wxDateTime endTime = selectedDate;
                endTime.SetHour(endHour);
                endTime.SetMinute(endMin);
                endTime.SetSecond(0);
                
                // Set the time pickers
                m_startTimePicker->SetValue(startTime);
                m_endTimePicker->SetValue(endTime);
                
                // Update cost calculation
                UpdateEstimatedCost();
                
                // Show confirmation message
                wxString confirmMsg = wxString::Format(
                    "Time slot selected:\n\n"
                    "Start Time: %s\n"
                    "End Time: %s\n\n"
                    "You can now proceed to book this time slot.",
                    startTime.Format("%H:%M"),
                    endTime.Format("%H:%M")
                );
                
                wxMessageBox(confirmMsg, "Time Slot Selected", 
                            wxOK | wxICON_INFORMATION, this);
            } else {
                wxMessageBox("Failed to parse time slot format!", 
                            "Format Error", wxOK | wxICON_ERROR, this);
            }
        } else {
            wxMessageBox("Invalid time slot format!", 
                        "Format Error", wxOK | wxICON_ERROR, this);
        }
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
    
    // Compare just the time parts
    int startMinutes = startTime.GetHour() * 60 + startTime.GetMinute();
    int endMinutes = endTime.GetHour() * 60 + endTime.GetMinute();
    
    if (startMinutes >= endMinutes) {
        wxMessageBox("Invalid time selection!\n\n"
                    "End time must be after start time.\n\n"
                    "Please check:\n"
                    "- Start time is earlier than end time\n"
                    "- Minimum booking duration is 1 hour\n"
                    "- Maximum booking duration is 4 hours",
                    "Time Validation Error", 
                    wxOK | wxICON_WARNING, this);
        return false;
    }
    
    // Check minimum duration (1 hour)
    if ((endMinutes - startMinutes) < 60) {
        wxMessageBox("Minimum booking duration is 1 hour!\n\n"
                    "Please select a longer time slot.",
                    "Duration Too Short", 
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
    
    // Check court availability (if court is heavily booked)
    if (IsCourtFullyBooked()) {
        wxMessageBox("Selected court is currently fully booked!\n\n"
                    "This court has reached maximum capacity.\n"
                    "Please try:\n"
                    "- Selecting a different court\n"
                    "- Choosing a different time slot\n"
                    "- Booking for another date",
                    "Court Not Available", 
                    wxOK | wxICON_WARNING, this);
        return false;
    }
    
    return true;
}

void BookingPanel::UpdateEstimatedCost()
{
    // Calculate estimated cost based on selected court, date, and time
    wxDateTime startTime = m_startTimePicker->GetValue();
    wxDateTime endTime = m_endTimePicker->GetValue();
    
    // Calculate duration in hours
    int startMinutes = startTime.GetHour() * 60 + startTime.GetMinute();
    int endMinutes = endTime.GetHour() * 60 + endTime.GetMinute();
    
    if (endMinutes <= startMinutes) {
        m_costLabel->SetLabel("0 VND");
        return;
    }
    
    double durationHours = (endMinutes - startMinutes) / 60.0;
    
    // Get selected court's price
    double hourlyRate = 50000.0; // Default price
    
    int courtSelection = m_courtChoice->GetSelection();
    if (courtSelection != wxNOT_FOUND && m_courtController) {
        wxStringClientData* clientData = dynamic_cast<wxStringClientData*>(m_courtChoice->GetClientObject(courtSelection));
        if (clientData) {
            long courtId;
            if (clientData->GetData().ToLong(&courtId)) {
                try {
                    auto courts = m_courtController->getAllCourts();
                    for (const auto& court : courts) {
                        if (court && court->getId() == (int)courtId) {
                            hourlyRate = court->getHourlyRate();
                            break;
                        }
                    }
                } catch (const std::exception& e) {
                    // Use default rate if error
                    hourlyRate = 50000.0;
                }
            }
        }
    }
    
    double totalCost = durationHours * hourlyRate;
    
    m_costLabel->SetLabel(wxString::Format("%.0f VND", totalCost));
}

void BookingPanel::ClearBookingForm()
{
    m_notesCtrl->Clear();
    m_costLabel->SetLabel("0 VND");
    
    // Reset time pickers to reasonable defaults
    wxDateTime now = wxDateTime::Now();
    
    // Set default start time to next hour
    wxDateTime startTime = now;
    startTime.SetMinute(0);
    startTime.SetSecond(0);
    if (now.GetMinute() > 0) {
        startTime += wxTimeSpan::Hours(1);
    }
    
    // Set default end time to 2 hours later
    wxDateTime endTime = startTime + wxTimeSpan::Hours(2);
    
    m_startTimePicker->SetValue(startTime);
    m_endTimePicker->SetValue(endTime);
}

void BookingPanel::UpdateButtonStates()
{
    bool hasSelection = (m_selectedBookingId != -1);
    bool canCancel = false;
    bool canModify = false;
    
    if (hasSelection && m_bookingController) {
        try {
            // Get the selected booking to check its status
            auto userBookings = m_bookingController->getUserBookings(
                m_authController->getCurrentUser()->getId()
            );
            
            for (const auto& booking : userBookings) {
                if (booking && booking->getId() == m_selectedBookingId) {
                    // Can only cancel if booking is PENDING or CONFIRMED
                    canCancel = (booking->getStatus() == BookingStatus::PENDING || 
                               booking->getStatus() == BookingStatus::CONFIRMED);
                    
                    // Can modify if booking is PENDING
                    canModify = (booking->getStatus() == BookingStatus::PENDING);
                    break;
                }
            }
        } catch (const std::exception& e) {
            // If error, disable buttons
            canCancel = false;
            canModify = false;
        }
    }
    
    m_cancelBookingBtn->Enable(canCancel);
    m_modifyBookingBtn->Enable(canModify);
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
    // Convert to string with no decimals
    wxString amountStr = wxString::Format("%.0f", amount);
    
    // Add thousand separators manually
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

wxString BookingPanel::FormatDateTime(std::time_t time)
{
    wxDateTime dt(time);
    return dt.Format("%d/%m/%Y %H:%M");
}

bool BookingPanel::IsCourtFullyBooked()
{
    // Get selected court
    int courtSelection = m_courtChoice->GetSelection();
    if (courtSelection == wxNOT_FOUND) {
        return false; // No court selected
    }
    
    // Get court ID
    wxStringClientData* clientData = dynamic_cast<wxStringClientData*>(m_courtChoice->GetClientObject(courtSelection));
    if (!clientData) {
        return false;
    }
    
    long courtId;
    if (!clientData->GetData().ToLong(&courtId)) {
        return false;
    }
    
    // Get all active bookings for this court
    auto allBookings = m_bookingController->getAllBookings();
    double courtHours = 0.0;
    
    for (const auto& booking : allBookings) {
        if (booking && booking->getCourtId() == courtId && 
            booking->getStatus() != BookingStatus::CANCELLED) {
            double duration = (booking->getEndTime() - booking->getStartTime()) / 3600.0;
            courtHours += duration;
        }
    }
    
    // Consider court fully booked if usage > 80% (about 19.2 hours per day)
    double maxHours = 24 * 30; // hours per day * days per month  
    double usage = (courtHours / maxHours) * 100;
    
    return usage >= 80.0;
}

std::time_t BookingPanel::CombineDateTime(const wxDateTime& date, const wxDateTime& time)
{
    wxDateTime combined = date;
    combined.SetHour(time.GetHour());
    combined.SetMinute(time.GetMinute());
    combined.SetSecond(time.GetSecond());
    return combined.GetTicks();
}
