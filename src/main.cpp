#include <wx/wx.h>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/intl.h>
#include "BadmintonApp.h"
#include "LoginFrame.h"
#include "MainFrame.h"
#include "AuthController.h"
#include "CourtController.h"
#include "BookingController.h"
#include "BookingManager.h"
#include "NotificationObserver.h"
#include <memory>
#include <fstream>
#include <vector>

wxIMPLEMENT_APP(BadmintonApp);

bool BadmintonApp::OnInit() {
    // Set locale for proper date format display (DD/MM/YYYY)
    wxLocale* locale = new wxLocale(wxLANGUAGE_DEFAULT);
    
    SetExitOnFrameDelete(false);
    
    // Initialize controllers
    InitializeControllers();
    
    // Setup notification system
    SetupNotifications();
    
    // Load initial data
    LoadInitialData();
    
    // Start with login frame
    ShowLoginFrame();
    
    return true;
}

int BadmintonApp::OnExit() {
    // Save data before exit
    if (m_authController) {
        m_authController->saveUsers();
    }
    
    if (m_courtController) {
        m_courtController->saveCourts();
    }
    
    // Save bookings through BookingManager
    BookingManager::getInstance().saveBookings();
    
    return wxApp::OnExit();
}

void BadmintonApp::InitializeControllers() {
    m_authController = std::make_shared<AuthController>();
    m_courtController = std::make_shared<CourtController>();
    m_bookingController = std::make_shared<BookingController>();
}

void BadmintonApp::SetupNotifications() {
    // Create notification observers
    m_emailObserver = std::make_shared<EmailNotificationObserver>(
        "smtp.gmail.com", 587, "your-email@gmail.com", "your-password");
    
    m_inAppObserver = std::make_shared<InAppNotificationObserver>(100);
    
    // Add observers to BookingManager
    BookingManager& bookingManager = BookingManager::getInstance();
    bookingManager.addObserver(m_emailObserver);
    bookingManager.addObserver(m_inAppObserver);
}

void BadmintonApp::LoadInitialData() {
    // Load data from files/database
    m_courtController->loadCourts();
    BookingManager::getInstance().loadBookings();
    
    // Note: Default admin user creation is handled in AuthController constructor
    // No need to create admin here as it would cause duplicates
    
    // Create default courts if none exist (only once)
    if (m_courtController->getAllCourts().empty()) {
        // Check if courts file exists to avoid recreating
        std::vector<std::string> possiblePaths = {
            "data/courts.txt",                    // Current directory
            "../data/courts.txt",                 // Parent directory
            "build/data/courts.txt",              // Build subdirectory
        };
        std::ifstream courtsFile;
        for (const auto& path : possiblePaths) {
            courtsFile.open(path);
            if (courtsFile) {
                break;  // Stop at the first valid path
            }
        }
        if (!courtsFile) {
            // Only create default courts if file doesn't exist
            m_courtController->addCourt("Court 1", "Standard badminton court", 50000.0);
            m_courtController->addCourt("Court 2", "Premium badminton court", 75000.0);
            m_courtController->addCourt("Court 3", "VIP badminton court", 100000.0);
        }
        courtsFile.close();
    }
}

void BadmintonApp::ShowLoginFrame() {
    // Store current window to close after creating new one
    wxTopLevelWindow* oldWindow = dynamic_cast<wxTopLevelWindow*>(GetTopWindow());
    
    // Create and show login frame
    LoginFrame* loginFrame = new LoginFrame(m_authController, m_courtController, m_bookingController);
    loginFrame->Show(true);
    SetTopWindow(loginFrame);
    
    // Close the old window after new one is shown
    if (oldWindow && oldWindow != loginFrame) {
        // Prevent the old window from vetoing the close
        oldWindow->Close(true); // Force close without veto
    }
}

void BadmintonApp::ShowMainFrame() {
    // Store current window to close after creating new one
    wxTopLevelWindow* oldWindow = dynamic_cast<wxTopLevelWindow*>(GetTopWindow());
    
    // Create and show main frame
    MainFrame* mainFrame = new MainFrame(m_authController, m_courtController, m_bookingController);
    mainFrame->Show(true);
    SetTopWindow(mainFrame);
    
    // Close the old window after new one is shown
    if (oldWindow && oldWindow != mainFrame) {
        // Prevent the old window from vetoing the close
        oldWindow->Close(true); // Force close without veto
    }
}

void BadmintonApp::OnLogout() {
    // Logout from auth controller
    m_authController->logout();
    
    // Show login frame again
    ShowLoginFrame();
}
