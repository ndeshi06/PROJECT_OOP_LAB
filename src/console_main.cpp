#include <iostream>
#include <memory>
#include "AuthController.h"
#include "CourtController.h"
#include "BookingController.h"
#include "BookingManager.h"
#include "NotificationObserver.h"

class BadmintonConsoleApp {
private:
    std::shared_ptr<AuthController> m_authController;
    std::shared_ptr<CourtController> m_courtController;
    std::shared_ptr<BookingController> m_bookingController;
    std::shared_ptr<EmailNotificationObserver> m_emailObserver;
    std::shared_ptr<InAppNotificationObserver> m_inAppObserver;

public:
    bool Initialize();
    void Run();
    void Cleanup();

private:
    void InitializeControllers();
    void SetupNotifications();
    void LoadInitialData();
    void ShowMainMenu();
    void HandleUserLogin();
    void HandleCourtManagement();
    void HandleBookingManagement();
    void HandleStatistics();
    void PrintSeparator();
};

int main() {
    std::cout << "=== Badminton Court Management System (Console Version) ===" << std::endl;
    std::cout << "Starting application..." << std::endl;
    
    BadmintonConsoleApp app;
    
    if (!app.Initialize()) {
        std::cout << "Failed to initialize application!" << std::endl;
        return 1;
    }
    
    app.Run();
    app.Cleanup();
    
    std::cout << "Application terminated." << std::endl;
    return 0;
}

bool BadmintonConsoleApp::Initialize() {
    try {
        InitializeControllers();
        SetupNotifications();
        LoadInitialData();
        std::cout << "Application initialized successfully!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

void BadmintonConsoleApp::Run() {
    std::cout << "\nWelcome to Badminton Court Management System!" << std::endl;
    
    while (true) {
        ShowMainMenu();
        
        int choice;
        std::cout << "\nEnter your choice: ";
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                HandleUserLogin();
                break;
            case 2:
                HandleCourtManagement();
                break;
            case 3:
                HandleBookingManagement();
                break;
            case 4:
                HandleStatistics();
                break;
            case 5:
                std::cout << "Thank you for using our system!" << std::endl;
                return;
            default:
                std::cout << "Invalid choice! Please try again." << std::endl;
        }
    }
}

void BadmintonConsoleApp::Cleanup() {
    // Save data before exit
    if (m_authController) {
        m_authController->saveUsers();
        std::cout << "User data saved." << std::endl;
    }
    
    if (m_courtController) {
        m_courtController->saveCourts();
        std::cout << "Court data saved." << std::endl;
    }
    
    // Save bookings through BookingManager
    BookingManager::getInstance().saveBookings();
    std::cout << "Booking data saved." << std::endl;
}

void BadmintonConsoleApp::InitializeControllers() {
    m_authController = std::make_shared<AuthController>();
    m_courtController = std::make_shared<CourtController>();
    m_bookingController = std::make_shared<BookingController>();
}

void BadmintonConsoleApp::SetupNotifications() {
    // Create notification observers
    m_emailObserver = std::make_shared<EmailNotificationObserver>(
        "smtp.gmail.com", 587, "your-email@gmail.com", "your-password");
    
    m_inAppObserver = std::make_shared<InAppNotificationObserver>(100);
    
    // Add observers to BookingManager
    BookingManager& bookingManager = BookingManager::getInstance();
    bookingManager.addObserver(m_emailObserver);
    bookingManager.addObserver(m_inAppObserver);
}

void BadmintonConsoleApp::LoadInitialData() {
    // Load data from files/database
    m_authController->loadUsers();
    m_courtController->loadCourts();
    BookingManager::getInstance().loadBookings();
    
    // Create default admin user if no users exist
    if (m_authController->getAllUsers().empty()) {
        m_authController->registerUser(
            "admin@badminton.com", 
            "admin123", 
            "System Administrator", 
            "0123456789", 
            UserRole::ADMIN
        );
        std::cout << "Default admin user created: admin@badminton.com / admin123" << std::endl;
    }
    
    // Create default courts if none exist
    if (m_courtController->getAllCourts().empty()) {
        m_courtController->addCourt("Court 1", "Standard badminton court", 50000.0);
        m_courtController->addCourt("Court 2", "Premium badminton court", 75000.0);
        m_courtController->addCourt("Court 3", "VIP badminton court", 100000.0);
        std::cout << "Default courts created." << std::endl;
    }
}

void BadmintonConsoleApp::ShowMainMenu() {
    PrintSeparator();
    std::cout << "MAIN MENU" << std::endl;
    PrintSeparator();
    std::cout << "1. User Login/Registration" << std::endl;
    std::cout << "2. Court Management" << std::endl;
    std::cout << "3. Booking Management" << std::endl;
    std::cout << "4. Statistics" << std::endl;
    std::cout << "5. Exit" << std::endl;
    PrintSeparator();
}

void BadmintonConsoleApp::HandleUserLogin() {
    std::cout << "\n=== USER LOGIN/REGISTRATION ===" << std::endl;
    std::cout << "1. Login" << std::endl;
    std::cout << "2. Register" << std::endl;
    std::cout << "3. Back to main menu" << std::endl;
    
    int choice;
    std::cout << "Enter choice: ";
    std::cin >> choice;
    
    switch (choice) {
        case 1: {
            std::string email, password;
            std::cout << "Email: ";
            std::cin >> email;
            std::cout << "Password: ";
            std::cin >> password;
            
            if (m_authController->login(email, password)) {
                auto user = m_authController->getCurrentUser();
                std::cout << "Login successful! Welcome, " << user->getFullName() 
                         << " (" << user->getRoleString() << ")" << std::endl;
            } else {
                std::cout << "Login failed! Invalid credentials." << std::endl;
            }
            break;
        }
        case 2: {
            std::string email, password, fullName, phone;
            std::cout << "Email: ";
            std::cin >> email;
            std::cout << "Password: ";
            std::cin >> password;
            std::cout << "Full Name: ";
            std::cin.ignore();
            std::getline(std::cin, fullName);
            std::cout << "Phone: ";
            std::cin >> phone;
            
            if (m_authController->registerUser(email, password, fullName, phone)) {
                std::cout << "Registration successful!" << std::endl;
            } else {
                std::cout << "Registration failed!" << std::endl;
            }
            break;
        }
        case 3:
            return;
        default:
            std::cout << "Invalid choice!" << std::endl;
    }
}

void BadmintonConsoleApp::HandleCourtManagement() {
    std::cout << "\n=== COURT MANAGEMENT ===" << std::endl;
    
    if (!m_authController->isLoggedIn()) {
        std::cout << "Please login first!" << std::endl;
        return;
    }
    
    if (!m_authController->isAdmin()) {
        std::cout << "Access denied! Admin privileges required." << std::endl;
        return;
    }
    
    std::cout << "1. List all courts" << std::endl;
    std::cout << "2. Add new court" << std::endl;
    std::cout << "3. Back to main menu" << std::endl;
    
    int choice;
    std::cout << "Enter choice: ";
    std::cin >> choice;
    
    switch (choice) {
        case 1: {
            auto courts = m_courtController->getAllCourts();
            std::cout << "\n--- COURT LIST ---" << std::endl;
            for (const auto& court : courts) {
                std::cout << "ID: " << court->getId() 
                         << ", Name: " << court->getName()
                         << ", Rate: $" << court->getHourlyRate()
                         << ", Status: " << court->getStatusString() << std::endl;
            }
            break;
        }
        case 2: {
            std::string name, description;
            double rate;
            std::cout << "Court name: ";
            std::cin.ignore();
            std::getline(std::cin, name);
            std::cout << "Description: ";
            std::getline(std::cin, description);
            std::cout << "Hourly rate: ";
            std::cin >> rate;
            
            if (m_courtController->addCourt(name, description, rate)) {
                std::cout << "Court added successfully!" << std::endl;
            } else {
                std::cout << "Failed to add court!" << std::endl;
            }
            break;
        }
        case 3:
            return;
        default:
            std::cout << "Invalid choice!" << std::endl;
    }
}

void BadmintonConsoleApp::HandleBookingManagement() {
    std::cout << "\n=== BOOKING MANAGEMENT ===" << std::endl;
    
    if (!m_authController->isLoggedIn()) {
        std::cout << "Please login first!" << std::endl;
        return;
    }
    
    std::cout << "1. View my bookings" << std::endl;
    std::cout << "2. Create new booking" << std::endl;
    std::cout << "3. Back to main menu" << std::endl;
    
    int choice;
    std::cout << "Enter choice: ";
    std::cin >> choice;
    
    switch (choice) {
        case 1: {
            auto user = m_authController->getCurrentUser();
            auto bookings = m_bookingController->getUserBookings(user->getId());
            
            std::cout << "\n--- YOUR BOOKINGS ---" << std::endl;
            for (const auto& booking : bookings) {
                std::cout << "ID: " << booking->getId()
                         << ", Court: " << booking->getCourtId()
                         << ", Amount: $" << booking->getTotalAmount()
                         << ", Status: " << booking->getStatusString() << std::endl;
            }
            break;
        }
        case 2: {
            auto user = m_authController->getCurrentUser();
            auto courts = m_courtController->getAvailableCourts();
            
            if (courts.empty()) {
                std::cout << "No courts available!" << std::endl;
                return;
            }
            
            std::cout << "Available courts:" << std::endl;
            for (const auto& court : courts) {
                std::cout << court->getId() << ". " << court->getName()
                         << " ($" << court->getHourlyRate() << "/hour)" << std::endl;
            }
            
            int courtId;
            std::cout << "Select court ID: ";
            std::cin >> courtId;
            
            // Simplified booking creation (in real app, would handle date/time properly)
            std::time_t now = std::time(nullptr);
            std::time_t startTime = now + 3600; // 1 hour from now
            std::time_t endTime = startTime + 3600; // 1 hour duration
            
            if (m_bookingController->createBooking(user->getId(), courtId, now, startTime, endTime)) {
                std::cout << "Booking created successfully!" << std::endl;
            } else {
                std::cout << "Failed to create booking!" << std::endl;
            }
            break;
        }
        case 3:
            return;
        default:
            std::cout << "Invalid choice!" << std::endl;
    }
}

void BadmintonConsoleApp::HandleStatistics() {
    std::cout << "\n=== STATISTICS ===" << std::endl;
    
    if (!m_authController->isLoggedIn() || !m_authController->isAdmin()) {
        std::cout << "Access denied! Admin privileges required." << std::endl;
        return;
    }
    
    auto courts = m_courtController->getAllCourts();
    auto allBookings = BookingManager::getInstance().getAllBookings();
    
    std::cout << "Total Courts: " << courts.size() << std::endl;
    std::cout << "Total Bookings: " << allBookings.size() << std::endl;
    
    double totalRevenue = 0.0;
    for (const auto& booking : allBookings) {
        if (booking->getStatus() == BookingStatus::CONFIRMED || 
            booking->getStatus() == BookingStatus::COMPLETED) {
            totalRevenue += booking->getTotalAmount();
        }
    }
    
    std::cout << "Total Revenue: $" << totalRevenue << std::endl;
    
    // Show notifications
    auto notifications = m_inAppObserver->getNotifications();
    std::cout << "\nRecent Notifications:" << std::endl;
    for (size_t i = 0; i < std::min(notifications.size(), size_t(5)); ++i) {
        std::cout << notifications[i] << std::endl;
    }
}

void BadmintonConsoleApp::PrintSeparator() {
    std::cout << "================================================" << std::endl;
}
