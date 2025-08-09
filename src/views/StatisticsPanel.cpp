#include "../include/StatisticsPanel.h"
#include "../include/BookingController.h"
#include "../include/CourtController.h"
#include "../include/AuthController.h"
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/datectrl.h>
#include <wx/statbox.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <sstream>
#include <map>
#include <tuple>
#include <iomanip>
#include <ctime>

// Event table
wxBEGIN_EVENT_TABLE(StatisticsPanel, wxPanel)
    EVT_BUTTON(ID_GENERATE_STATS, StatisticsPanel::OnGenerateStats)
    EVT_BUTTON(ID_EXPORT_STATS, StatisticsPanel::OnExportStats)
wxEND_EVENT_TABLE()

StatisticsPanel::StatisticsPanel(wxWindow *parent,
                                    BookingController* bookingController,
                                    CourtController* courtController,
                                    AuthController* authController)
: wxPanel(parent, wxID_ANY),
    m_bookingController(bookingController),
    m_courtController(courtController),
    m_authController(authController)
{
    CreateUI();
    BindEvents();
    RefreshData();
}

StatisticsPanel::~StatisticsPanel() {}

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
    wxStaticText *startLabel = new wxStaticText(this, wxID_ANY, "From Date:");
    m_startDatePicker = new wxDatePickerCtrl(this, ID_START_DATE, wxDefaultDateTime,
                                             wxDefaultPosition, wxDefaultSize,
                                             wxDP_DROPDOWN | wxDP_SHOWCENTURY);

    // End date
    wxStaticText *endLabel = new wxStaticText(this, wxID_ANY, "To Date:");
    m_endDatePicker = new wxDatePickerCtrl(this, ID_END_DATE, wxDefaultDateTime,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxDP_DROPDOWN | wxDP_SHOWCENTURY);

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

    wxFlexGridSizer *summaryGrid = new wxFlexGridSizer(2, 4, 10, 20);

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

void StatisticsPanel::OnGenerateStats(wxCommandEvent &event)
{
    GenerateStatistics();
}

void StatisticsPanel::OnExportStats(wxCommandEvent &event)
{
    // Get date range
    wxDateTime startDate = m_startDatePicker->GetValue();
    wxDateTime endDate = m_endDatePicker->GetValue();

    // Convert wxDateTime to time_t
    std::time_t startTime = startDate.GetTicks();
    std::time_t endTime = endDate.GetTicks();

    if (startTime > endTime)
    {
        wxMessageBox("Start date cannot be later than end date!", "Error",
                     wxOK | wxICON_ERROR);
        return;
    }

    // Create file dialog to save CSV
    wxFileDialog saveDialog(this, "Export Statistics to CSV", "", "",
                            "CSV files (*.csv)|*.csv",
                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    wxString filePath = saveDialog.GetPath();

    try
    {
        // Generate CSV data using statistics controller
        if (!m_bookingController)
        {
            wxMessageBox("Booking controller not available!", "Error",
                         wxOK | wxICON_ERROR);
            return;
        }

        // Create enhanced CSV with court statistics
        std::ostringstream csvContent;

        // Header information
        csvContent << "# Badminton Court Management System - Statistics Export\n";
        csvContent << "# Export Date: " << wxDateTime::Now().FormatISODate() << "\n";
        csvContent << "# Period: " << startDate.FormatISODate() << " to " << endDate.FormatISODate() << "\n";
        csvContent << "\n";

        // Summary section
        csvContent << "SUMMARY\n";
        csvContent << "Metric,Value\n";

        // Calculate summary data
        auto allBookings = m_bookingController->getAllBookings();
        int totalBookings = 0;
        double totalRevenue = 0.0;
        double totalHours = 0.0;

        for (const auto &booking : allBookings)
        {
            if (!booking || booking->getStatus() == BookingStatus::CANCELLED)
            {
                continue;
            }

            std::time_t bookingTime = booking->getBookingDate();
            if (bookingTime >= startTime && bookingTime <= endTime)
            {
                totalBookings++;
                totalRevenue += booking->getTotalAmount();
                totalHours += booking->getDurationHours();
            }
        }

        csvContent << "Total Bookings," << totalBookings << "\n";
        csvContent << "Total Revenue," << std::fixed << std::setprecision(2) << totalRevenue << " VND\n";
        csvContent << "Total Hours," << std::fixed << std::setprecision(1) << totalHours << "\n";

        if (totalBookings > 0)
        {
            csvContent << "Average Revenue per Booking," << std::fixed << std::setprecision(2)
                       << (totalRevenue / totalBookings) << " VND\n";
        }

        csvContent << "\n";

        // Court usage section
        csvContent << "COURT USAGE STATISTICS\n";
        csvContent << "Court,Bookings,Revenue (VND),Usage Hours,Usage Rate (%)\n";

        auto courts = m_courtController->getAllCourts();
        for (const auto &court : courts)
        {
            if (!court)
                continue;

            int courtBookings = 0;
            double courtRevenue = 0.0;
            double courtHours = 0.0;

            for (const auto &booking : allBookings)
            {
                if (!booking || booking->getStatus() == BookingStatus::CANCELLED)
                {
                    continue;
                }

                std::time_t bookingTime = booking->getBookingDate();
                if (bookingTime >= startTime && bookingTime <= endTime &&
                    booking->getCourtId() == court->getId())
                {
                    courtBookings++;
                    courtRevenue += booking->getTotalAmount();
                    courtHours += booking->getDurationHours();
                }
            }

            // Calculate usage rate (assuming 12 hours available per day)
            int daysDiff = static_cast<int>((endTime - startTime) / (24 * 3600)) + 1;
            double maxPossibleHours = daysDiff * 12.0;
            double usageRate = maxPossibleHours > 0 ? (courtHours / maxPossibleHours) * 100.0 : 0.0;

            csvContent << court->getName() << "," << courtBookings << ","
                       << std::fixed << std::setprecision(2) << courtRevenue << ","
                       << std::fixed << std::setprecision(1) << courtHours << ","
                       << std::fixed << std::setprecision(1) << usageRate << "\n";
        }

        csvContent << "\n";

        // Daily breakdown section
        csvContent << "DAILY BREAKDOWN\n";
        csvContent << "Date,Bookings,Revenue (VND),Hours\n";

        // Group bookings by date
        std::map<std::string, std::tuple<int, double, double>> dailyData;

        for (const auto &booking : allBookings)
        {
            if (!booking || booking->getStatus() == BookingStatus::CANCELLED)
            {
                continue;
            }

            std::time_t bookingTime = booking->getBookingDate();
            if (bookingTime >= startTime && bookingTime <= endTime)
            {
                // Format date as YYYY-MM-DD
                std::tm *tm = std::localtime(&bookingTime);
                char dateStr[11];
                std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", tm);

                auto &dayData = dailyData[dateStr];
                std::get<0>(dayData)++;                              // bookings
                std::get<1>(dayData) += booking->getTotalAmount();   // revenue
                std::get<2>(dayData) += booking->getDurationHours(); // hours
            }
        }

        for (const auto &dayPair : dailyData)
        {
            csvContent << dayPair.first << ","
                       << std::get<0>(dayPair.second) << ","
                       << std::fixed << std::setprecision(2) << std::get<1>(dayPair.second) << ","
                       << std::fixed << std::setprecision(1) << std::get<2>(dayPair.second) << "\n";
        }

        // Write to file
        wxFile file(filePath, wxFile::write);
        if (!file.IsOpened())
        {
            wxMessageBox("Failed to create export file!", "Error",
                         wxOK | wxICON_ERROR);
            return;
        }

        file.Write(csvContent.str());
        file.Close();

        wxMessageBox(wxString::Format("Statistics exported successfully to:\n%s", filePath),
                     "Export Complete", wxOK | wxICON_INFORMATION);
    }
    catch (const std::exception &e)
    {
        wxMessageBox(wxString::Format("Export failed: %s", e.what()), "Error",
                     wxOK | wxICON_ERROR);
    }
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
    if (!m_bookingController || !m_courtController)
    {
        // No controllers available, show zero data
        m_totalRevenueLabel->SetLabel("0 VND");
        m_totalBookingsLabel->SetLabel("0");
        m_totalHoursLabel->SetLabel("0 hours");
        m_averageUsageLabel->SetLabel("0%");
        return;
    }

    // Get actual data from controllers
    auto allBookings = m_bookingController->getAllBookings();
    auto courts = m_courtController->getAllCourts();

    // Filter out cancelled bookings
    std::vector<Booking*> activeBookings;
    for (const auto &booking : allBookings)
    {
        if (booking && booking->getStatus() != BookingStatus::CANCELLED)
        {
            activeBookings.push_back(booking);
        }
    }

    // Calculate real statistics from active bookings only
    double totalRevenue = 0.0;
    int totalBookings = activeBookings.size();
    double totalHours = 0.0;

    for (const auto &booking : activeBookings)
    {
        if (booking)
        {
            totalRevenue += booking->getTotalAmount();
            // Calculate duration in hours (simplified)
            double duration = (booking->getEndTime() - booking->getStartTime()) / 3600.0; // seconds to hours
            totalHours += duration;
        }
    }

    // Calculate average usage (simplified)
    double averageUsage = 0.0;
    if (!courts.empty() && totalHours > 0)
    {
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

    if (!m_courtController || !m_bookingController)
    {
        return;
    }

    // Get real court and booking data
    auto courts = m_courtController->getAllCourts();
    auto allBookings = m_bookingController->getAllBookings();

    // Filter out cancelled bookings
    std::vector<Booking*> activeBookings;
    for (const auto &booking : allBookings)
    {
        if (booking && booking->getStatus() != BookingStatus::CANCELLED)
        {
            activeBookings.push_back(booking);
        }
    }

    // Calculate statistics for each court
    for (const auto &court : courts)
    {
        if (!court)
            continue;

        int courtBookings = 0;
        double courtRevenue = 0.0;
        double courtHours = 0.0;

        // Calculate this court's statistics from active bookings only
        for (const auto &booking : activeBookings)
        {
            if (booking && booking->getCourtId() == court->getId())
            {
                courtBookings++;
                courtRevenue += booking->getTotalAmount();
                double duration = (booking->getEndTime() - booking->getStartTime()) / 3600.0;
                courtHours += duration;
            }
        }

        // Calculate usage percentage and availability
        double usage = 0.0;
        wxString availabilityStatus = "Available";

        if (courtHours > 0)
        {
            double maxHours = 24 * 30; // hours per day * days per month
            usage = (courtHours / maxHours) * 100;

            // Check if court is fully booked (>80% usage means heavily booked)
            if (usage >= 80.0)
            {
                availabilityStatus = "Fully Booked";
            }
            else if (usage >= 50.0)
            {
                availabilityStatus = "Busy";
            }
        }

        // Add to list with availability status
        wxString courtName = court->getName();
        if (availabilityStatus != "Available")
        {
            courtName += " (" + availabilityStatus + ")";
        }

        long index = m_statsListCtrl->InsertItem(m_statsListCtrl->GetItemCount(), courtName);
        m_statsListCtrl->SetItem(index, 1, wxString::Format("%d", courtBookings));
        m_statsListCtrl->SetItem(index, 2, wxString::Format("%.1f hours", courtHours));
        m_statsListCtrl->SetItem(index, 3, wxString::Format("%.0f VND", courtRevenue));
        m_statsListCtrl->SetItem(index, 4, wxString::Format("%.1f%%", usage));
    }

    // If no courts, show empty message
    if (courts.empty())
    {
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
    // Convert to string with no decimals
    wxString amountStr = wxString::Format("%.0f", amount);

    // Add thousand separators manually
    wxString result = "";
    int len = amountStr.length();
    for (int i = 0; i < len; i++)
    {
        if (i > 0 && (len - i) % 3 == 0)
        {
            result += ",";
        }
        result += amountStr[i];
    }

    return result + " VND";
}

wxString StatisticsPanel::FormatDuration(int hours)
{
    return wxString::Format("%d hours", hours);
}
