#include "../include/StatisticsPanel.h"
#include "../include/BookingCo    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Total Revenue:"));
    m_totalRevenueLabel = new wxStaticText(this, wxID_ANY, "0 VND");
    summaryGrid->Add(m_totalRevenueLabel);
    
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Total Bookings:"));
    m_totalBookingsLabel = new wxStaticText(this, wxID_ANY, "0");
    summaryGrid->Add(m_totalBookingsLabel);
    
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Total Usage Hours:"));
    m_totalHoursLabel = new wxStaticText(this, wxID_ANY, "0");
    summaryGrid->Add(m_totalHoursLabel);
    
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Average Usage Rate:"));#include "../include/CourtController.h"
#include "../include/AuthController.h"
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/datectrl.h>
#include <wx/statbox.h>

// Event table
wxBEGIN_EVENT_TABLE(StatisticsPanel, wxPanel)
    EVT_BUTTON(ID_GENERATE_STATS, StatisticsPanel::OnGenerateStats)
    EVT_BUTTON(ID_EXPORT_STATS, StatisticsPanel::OnExportStats)
wxEND_EVENT_TABLE()

StatisticsPanel::StatisticsPanel(wxWindow* parent,
                               std::shared_ptr<BookingController> bookingController,
                               std::shared_ptr<CourtController> courtController,
                               std::shared_ptr<AuthController> authController)
    : wxPanel(parent, wxID_ANY),
      m_bookingController(bookingController),
      m_courtController(courtController),
      m_authController(authController)
{
    CreateUI();
    BindEvents();
    RefreshData();
}

StatisticsPanel::~StatisticsPanel()
{
}

void StatisticsPanel::CreateUI()
{
    CreateDateControls();
    CreateSummaryPanel();
    CreateDetailsPanel();
}

void StatisticsPanel::CreateDateControls()
{
    m_dateSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Select Time Period");
    
    // Start date
    wxStaticText* startLabel = new wxStaticText(this, wxID_ANY, "From Date:");
    m_startDatePicker = new wxDatePickerCtrl(this, ID_START_DATE);
    
    // End date
    wxStaticText* endLabel = new wxStaticText(this, wxID_ANY, "To Date:");
    m_endDatePicker = new wxDatePickerCtrl(this, ID_END_DATE);
    
    // Buttons
    m_generateBtn = new wxButton(this, ID_GENERATE_STATS, "Generate Report");
    m_exportBtn = new wxButton(this, ID_EXPORT_STATS, "Export");
    
    m_dateSizer->Add(startLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_dateSizer->Add(m_startDatePicker, 0, wxRIGHT, 15);
    m_dateSizer->Add(endLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_dateSizer->Add(m_endDatePicker, 0, wxRIGHT, 15);
    m_dateSizer->Add(m_generateBtn, 0, wxRIGHT, 5);
    m_dateSizer->Add(m_exportBtn, 0);
}

void StatisticsPanel::CreateSummaryPanel()
{
    m_summaryBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Overview");
    
    wxFlexGridSizer* summaryGrid = new wxFlexGridSizer(2, 4, 10, 20);
    
    // Labels
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Tổng doanh thu:"));
    m_totalRevenueLabel = new wxStaticText(this, wxID_ANY, "0 VND");
    summaryGrid->Add(m_totalRevenueLabel);
    
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Tổng số đặt sân:"));
    m_totalBookingsLabel = new wxStaticText(this, wxID_ANY, "0");
    summaryGrid->Add(m_totalBookingsLabel);
    
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Tổng giờ sử dụng:"));
    m_totalHoursLabel = new wxStaticText(this, wxID_ANY, "0 giờ");
    summaryGrid->Add(m_totalHoursLabel);
    
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Average Usage Rate:"));
    m_averageUsageLabel = new wxStaticText(this, wxID_ANY, "0%");
    summaryGrid->Add(m_averageUsageLabel);
    
    m_summaryBoxSizer->Add(summaryGrid, 1, wxEXPAND | wxALL, 10);
}

void StatisticsPanel::CreateDetailsPanel()
{
    m_detailsSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Details");
    
    m_statsListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                    wxLC_REPORT | wxLC_SINGLE_SEL);
    
    m_statsListCtrl->AppendColumn("Sân", wxLIST_FORMAT_LEFT, 100);
    m_statsListCtrl->AppendColumn("Số lần đặt", wxLIST_FORMAT_RIGHT, 100);
    m_statsListCtrl->AppendColumn("Giờ sử dụng", wxLIST_FORMAT_RIGHT, 100);
    m_statsListCtrl->AppendColumn("Doanh thu", wxLIST_FORMAT_RIGHT, 120);
    m_statsListCtrl->AppendColumn("Tỷ lệ sử dụng", wxLIST_FORMAT_RIGHT, 100);
    
    m_detailsSizer->Add(m_statsListCtrl, 1, wxEXPAND | wxALL, 5);
    
    // Setup main layout
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    m_mainSizer->Add(m_dateSizer, 0, wxEXPAND | wxALL, 10);
    m_mainSizer->Add(m_summaryBoxSizer, 0, wxEXPAND | wxALL, 10);
    m_mainSizer->Add(m_detailsSizer, 1, wxEXPAND | wxALL, 10);
    
    SetSizer(m_mainSizer);
}

void StatisticsPanel::BindEvents()
{
    // Events are bound through the event table
}

void StatisticsPanel::OnGenerateStats(wxCommandEvent& event)
{
    GenerateStatistics();
}

void StatisticsPanel::OnExportStats(wxCommandEvent& event)
{
    wxMessageBox("Export feature will be implemented later!", "Information", 
                wxOK | wxICON_INFORMATION);
}

void StatisticsPanel::RefreshData()
{
    GenerateStatistics();
}

void StatisticsPanel::UpdateStatistics()
{
    GenerateStatistics();
}

void StatisticsPanel::GenerateStatistics()
{
    // For now, just show placeholder data
    m_totalRevenueLabel->SetLabel("2,500,000 VND");
    m_totalBookingsLabel->SetLabel("45");
    m_totalHoursLabel->SetLabel("180 giờ");
    m_averageUsageLabel->SetLabel("75%");
    
    PopulateStatsList();
}

void StatisticsPanel::PopulateStatsList()
{
    m_statsListCtrl->DeleteAllItems();
    
    // Add sample data
    long index = m_statsListCtrl->InsertItem(0, "Sân 1");
    m_statsListCtrl->SetItem(index, 1, "15");
    m_statsListCtrl->SetItem(index, 2, "60 giờ");
    m_statsListCtrl->SetItem(index, 3, "900,000 VND");
    m_statsListCtrl->SetItem(index, 4, "80%");
    
    index = m_statsListCtrl->InsertItem(1, "Sân 2");
    m_statsListCtrl->SetItem(index, 1, "12");
    m_statsListCtrl->SetItem(index, 2, "48 giờ");
    m_statsListCtrl->SetItem(index, 3, "720,000 VND");
    m_statsListCtrl->SetItem(index, 4, "65%");
    
    index = m_statsListCtrl->InsertItem(2, "Sân 3");
    m_statsListCtrl->SetItem(index, 1, "18");
    m_statsListCtrl->SetItem(index, 2, "72 giờ");
    m_statsListCtrl->SetItem(index, 3, "880,000 VND");
    m_statsListCtrl->SetItem(index, 4, "85%");
}

void StatisticsPanel::CalculateSummary()
{
    // This would calculate real statistics from booking data
    // For now, using placeholder implementation
}

wxString StatisticsPanel::FormatCurrency(double amount)
{
    return wxString::Format("%.0f VND", amount);
}

wxString StatisticsPanel::FormatDuration(int hours)
{
    return wxString::Format("%d giờ", hours);
}
