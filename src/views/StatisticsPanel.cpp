#include "../include/StatisticsPanel.h"
#include "../include/BookingController.h"
#include "../include/CourtController.h"
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
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Total Revenue:"));
    m_totalRevenueLabel = new wxStaticText(this, wxID_ANY, "0 VND");
    summaryGrid->Add(m_totalRevenueLabel);
    
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Total Bookings:"));
    m_totalBookingsLabel = new wxStaticText(this, wxID_ANY, "0");
    summaryGrid->Add(m_totalBookingsLabel);
    
    summaryGrid->Add(new wxStaticText(this, wxID_ANY, "Total Usage Hours:"));
    m_totalHoursLabel = new wxStaticText(this, wxID_ANY, "0 hours");
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
    
    m_statsListCtrl->AppendColumn("Court", wxLIST_FORMAT_LEFT, 100);
    m_statsListCtrl->AppendColumn("Bookings", wxLIST_FORMAT_RIGHT, 100);
    m_statsListCtrl->AppendColumn("Usage Hours", wxLIST_FORMAT_RIGHT, 100);
    m_statsListCtrl->AppendColumn("Revenue", wxLIST_FORMAT_RIGHT, 120);
    m_statsListCtrl->AppendColumn("Usage Rate", wxLIST_FORMAT_RIGHT, 100);
    
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
    if (!m_bookingController || !m_courtController) {
        // No controllers available, show zero data
        m_totalRevenueLabel->SetLabel("0 VND");
        m_totalBookingsLabel->SetLabel("0");
        m_totalHoursLabel->SetLabel("0 hours");
        m_averageUsageLabel->SetLabel("0%");
        return;
    }
    
    // Get actual data from controllers
    auto bookings = m_bookingController->getAllBookings();
    auto courts = m_courtController->getAllCourts();
    
    // Calculate real statistics
    double totalRevenue = 0.0;
    int totalBookings = bookings.size();
    double totalHours = 0.0;
    
    for (const auto& booking : bookings) {
        if (booking) {
            totalRevenue += booking->getTotalAmount();
            // Calculate duration in hours (simplified)
            double duration = (booking->getEndTime() - booking->getStartTime()) / 3600.0; // seconds to hours
            totalHours += duration;
        }
    }
    
    // Calculate average usage (simplified)
    double averageUsage = 0.0;
    if (!courts.empty() && totalHours > 0) {
        double totalPossibleHours = courts.size() * 24 * 30; // Courts * hours per day * days per month
        averageUsage = (totalHours / totalPossibleHours) * 100;
    }
    
    // Display real data
    m_totalRevenueLabel->SetLabel(wxString::Format("%.0f VND", totalRevenue));
    m_totalBookingsLabel->SetLabel(wxString::Format("%d", totalBookings));
    m_totalHoursLabel->SetLabel(wxString::Format("%.1f hours", totalHours));
    m_averageUsageLabel->SetLabel(wxString::Format("%.1f%%", averageUsage));
    
    PopulateStatsList();
}

void StatisticsPanel::PopulateStatsList()
{
    m_statsListCtrl->DeleteAllItems();
    
    if (!m_courtController || !m_bookingController) {
        return;
    }
    
    // Get real court and booking data
    auto courts = m_courtController->getAllCourts();
    auto bookings = m_bookingController->getAllBookings();
    
    // Calculate statistics for each court
    for (const auto& court : courts) {
        if (!court) continue;
        
        int courtBookings = 0;
        double courtRevenue = 0.0;
        double courtHours = 0.0;
        
        // Calculate this court's statistics
        for (const auto& booking : bookings) {
            if (booking && booking->getCourtId() == court->getId()) {
                courtBookings++;
                courtRevenue += booking->getTotalAmount();
                double duration = (booking->getEndTime() - booking->getStartTime()) / 3600.0;
                courtHours += duration;
            }
        }
        
        // Calculate usage percentage (simplified)
        double usage = 0.0;
        if (courtHours > 0) {
            double maxHours = 24 * 30; // hours per day * days per month
            usage = (courtHours / maxHours) * 100;
        }
        
        // Add to list
        long index = m_statsListCtrl->InsertItem(m_statsListCtrl->GetItemCount(), court->getName());
        m_statsListCtrl->SetItem(index, 1, wxString::Format("%d", courtBookings));
        m_statsListCtrl->SetItem(index, 2, wxString::Format("%.1f hours", courtHours));
        m_statsListCtrl->SetItem(index, 3, wxString::Format("%.0f VND", courtRevenue));
        m_statsListCtrl->SetItem(index, 4, wxString::Format("%.1f%%", usage));
    }
    
    // If no courts, show empty message
    if (courts.empty()) {
        long index = m_statsListCtrl->InsertItem(0, "No courts available");
        m_statsListCtrl->SetItem(index, 1, "0");
        m_statsListCtrl->SetItem(index, 2, "0 hours");
        m_statsListCtrl->SetItem(index, 3, "0 VND");
        m_statsListCtrl->SetItem(index, 4, "0%");
    }
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
    return wxString::Format("%d hours", hours);
}
