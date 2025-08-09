#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/timectrl.h>
#include <wx/grid.h>
#include <ctime>

class BookingController;
class CourtController;
class AuthController;

class BookingPanel : public wxPanel
{
private:
    BookingController* m_bookingController;
    CourtController* m_courtController;
    AuthController* m_authController;

    // Booking creation components
    wxChoice *m_courtChoice;
    wxDatePickerCtrl *m_datePicker;
    wxTimePickerCtrl *m_startTimePicker;
    wxTimePickerCtrl *m_endTimePicker;
    wxTextCtrl *m_notesCtrl;
    wxStaticText *m_costLabel;
    wxButton *m_bookBtn;
    wxButton *m_checkAvailabilityBtn;

    // Available slots display
    wxListCtrl *m_availableSlotsList;

    // User bookings
    wxListCtrl *m_userBookingsList;
    wxButton *m_cancelBookingBtn;
    wxButton *m_modifyBookingBtn;
    wxButton *m_refreshBookingsBtn;

    // Layout
    wxBoxSizer *m_mainSizer;
    wxStaticBoxSizer *m_bookingSizer;
    wxStaticBoxSizer *m_availabilitySizer;
    wxStaticBoxSizer *m_myBookingsSizer;

    // State
    int m_selectedBookingId;

public:
    BookingPanel(wxWindow *parent,
                 BookingController* bookingController,
                 CourtController* courtController,
                 AuthController* authController);
    ~BookingPanel();

    // Event handlers
    void OnBookCourt(wxCommandEvent &event);
    void OnCheckAvailability(wxCommandEvent &event);
    void OnCancelBooking(wxCommandEvent &event);
    void OnModifyBooking(wxCommandEvent &event);
    void OnRefreshBookings(wxCommandEvent &event);
    void OnCourtChanged(wxCommandEvent &event);
    void OnTimeChanged(wxDateEvent &event);
    void OnDateChanged(wxDateEvent &event);
    void OnBookingSelected(wxListEvent &event);
    void OnAvailableSlotSelected(wxListEvent &event);

    // Public methods
    void RefreshBookings();
    void RefreshCourts();
    void RefreshData();
    void RefreshCourtList();
    void RefreshMyBookings();
    void RefreshAvailableSlots();
    void UpdateUI();

private:
    void CreateUI();
    void CreateBookingSection();
    void CreateBookingForm();
    void CreateAvailabilitySection();
    void CreateAvailabilityDisplay();
    void CreateMyBookingsSection();
    void CreateMyBookingsDisplay();
    void BindEvents();

    void UpdateAvailableSlots();
    void UpdateBookingCost();
    void UpdateEstimatedCost();
    void SavePendingChanges();
    void LoadUserBookings();
    void PopulateCourts();

    bool ValidateBookingInput();
    void ClearBookingForm();
    void UpdateButtonStates();
    bool IsCourtFullyBooked();

    // Helper methods
    wxString FormatTimeSlot(std::time_t startTime, std::time_t endTime);
    wxString FormatCurrency(double amount);
    wxString FormatDateTime(std::time_t time);
    std::time_t CombineDateTime(const wxDateTime &date, const wxDateTime &time);

    DECLARE_EVENT_TABLE()
};

// Event IDs
enum
{
    ID_BOOK_COURT = 4000,
    ID_CHECK_AVAILABILITY,
    ID_CANCEL_BOOKING,
    ID_MODIFY_BOOKING,
    ID_REFRESH_BOOKINGS,
    ID_COURT_CHOICE,
    ID_DATE_PICKER,
    ID_START_TIME,
    ID_END_TIME,
    ID_BOOKING_LIST,
    ID_AVAILABLE_SLOTS_LIST
};
