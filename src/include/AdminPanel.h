#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/choice.h>
// Forward declarations
class BookingController;
class CourtController;
class AuthController;
class Booking;

class AdminPanel : public wxPanel
{
public:
    AdminPanel(wxWindow *parent,
               BookingController *bookingController,
               CourtController *courtController,
               AuthController *authController);
    ~AdminPanel();

    void RefreshData();

private:
    // UI Creation
    void CreateUI();
    void CreateBookingHistoryView();
    void CreateFilterControls();
    void CreateStatisticsView();

    // Event handlers
    void OnRefreshData(wxCommandEvent &event);
    void OnFilterByDate(wxDateEvent &event);
    void OnFilterByCourt(wxCommandEvent &event);
    void OnFilterByUser(wxCommandEvent &event);
    void OnExportData(wxCommandEvent &event);
    void OnBookingSelected(wxListEvent &event);
    void OnCancelBooking(wxCommandEvent &event);

    // Data methods
    void RefreshBookingHistory();
    void RefreshStatistics();
    void ApplyFilters();
    wxString GetUserNameById(int userId);
    wxString FormatCurrency(double amount);

    // Member variables
    BookingController* m_bookingController;
    CourtController* m_courtController;
    AuthController* m_authController;

    // UI Controls
    wxBoxSizer *m_mainSizer;
    wxListCtrl *m_bookingHistoryList;
    wxDatePickerCtrl *m_startDatePicker;
    wxDatePickerCtrl *m_endDatePicker;
    wxChoice *m_courtFilter;
    wxChoice *m_userFilter;
    wxChoice *m_statusFilter;

    // Buttons
    wxButton *m_refreshBtn;
    wxButton *m_exportBtn;
    wxButton *m_cancelBookingBtn;

    // Statistics
    wxStaticText *m_totalBookingsLabel;
    wxStaticText *m_totalRevenueLabel;
    wxStaticText *m_activeBookingsLabel;

    int m_selectedBookingId;

    // Event IDs
    enum
    {
        ID_REFRESH_DATA = 7000,
        ID_FILTER_DATE,
        ID_FILTER_COURT,
        ID_FILTER_USER,
        ID_EXPORT_DATA,
        ID_BOOKING_HISTORY_LIST,
        ID_ADMIN_CANCEL_BOOKING,
        ID_START_DATE_PICKER,
        ID_END_DATE_PICKER,
        ID_COURT_FILTER,
        ID_USER_FILTER,
        ID_STATUS_FILTER
    };

    wxDECLARE_EVENT_TABLE();
};
