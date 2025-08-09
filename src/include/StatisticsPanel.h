#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/grid.h>

class BookingController;
class CourtController;
class AuthController;

class StatisticsPanel : public wxPanel
{
private:
    BookingController* m_bookingController;
    CourtController* m_courtController;
    AuthController* m_authController;

    // UI components
    wxDatePickerCtrl *m_startDatePicker;
    wxDatePickerCtrl *m_endDatePicker;
    wxButton *m_generateBtn;
    wxButton *m_exportBtn;

    // Display components
    wxListCtrl *m_statsListCtrl;
    wxStaticText *m_totalRevenueLabel;
    wxStaticText *m_totalBookingsLabel;
    wxStaticText *m_totalHoursLabel;
    wxStaticText *m_averageUsageLabel;

    // Layout
    wxBoxSizer *m_mainSizer;
    wxStaticBoxSizer *m_dateSizer;
    wxStaticBoxSizer *m_summaryBoxSizer;
    wxStaticBoxSizer *m_detailsSizer;

public:
    StatisticsPanel(wxWindow *parent,
                    BookingController* bookingController,
                    CourtController* courtController,
                    AuthController* authController);
    ~StatisticsPanel();

    // Event handlers
    void OnGenerateStats(wxCommandEvent &event);
    void OnExportStats(wxCommandEvent &event);

    // Public methods
    void RefreshData();
    void UpdateStatistics();

private:
    void CreateUI();
    void CreateDateControls();
    void CreateSummaryPanel();
    void CreateDetailsPanel();
    void BindEvents();

    void GenerateStatistics();
    void PopulateStatsList();
    void CalculateSummary();

    // Helper methods
    wxString FormatCurrency(double amount);
    wxString FormatDuration(int hours);

    DECLARE_EVENT_TABLE()
};

// Event IDs
enum
{
    ID_GENERATE_STATS = 4000,
    ID_EXPORT_STATS,
    ID_START_DATE,
    ID_END_DATE
};
