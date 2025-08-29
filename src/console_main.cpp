#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <sstream>

// Include your existing controllers and models
#include "AuthController.h"
#include "CourtController.h"
#include "BookingController.h"
#include "BookingManager.h"
#include "User.h"
#include "Court.h"
#include "Booking.h"

class ConsoleInterface {
private:
    AuthController* m_authController;
    CourtController* m_courtController;
    BookingController* m_bookingController;
    User* m_currentUser;
    bool m_running;

public:
    ConsoleInterface() : m_currentUser(nullptr), m_running(true) {
        // Initialize controllers
        m_authController = new AuthController();
        m_courtController = new CourtController();
        m_bookingController = new BookingController();
        
        // Load initial data
        BookingManager::getInstance().loadBookings();
        
        // Create default courts if none exist
        if (m_courtController->getAllCourts().empty()) {
            m_courtController->addCourt("Court 1", "Standard badminton court", 50000.0);
            m_courtController->addCourt("Court 2", "Premium badminton court", 75000.0);
            m_courtController->addCourt("Court 3", "VIP badminton court", 100000.0);
        }
    }
    
    ~ConsoleInterface() {
        // Save data before exit
        if (m_authController) {
            m_authController->saveUsers();
            delete m_authController;
        }
        if (m_courtController) {
            m_courtController->saveCourts();
            delete m_courtController;
        }
        if (m_bookingController) {
            delete m_bookingController;
        }
        
        // Save bookings
        BookingManager::getInstance().saveBookings();
        BookingManager::cleanup();
    }
    
    void run() {
        printWelcome();
        
        while (m_running) {
            if (!m_currentUser) {
                showLoginMenu();
            } else {
                if (m_currentUser->getRole() == UserRole::ADMIN) {
                    showAdminMenu();
                } else {
                    showCustomerMenu();
                }
            }
        }
    }
    
private:
    void printWelcome() {
        std::cout << "\n";
        std::cout << "========================================\n";
        std::cout << "  BADMINTON COURT MANAGEMENT SYSTEM   \n";
        std::cout << "           Console Version             \n";
        std::cout << "========================================\n";
        std::cout << "\n";
    }
    
    private:
    std::string formatDateTime(std::time_t time) {
        char buffer[50];
        std::strftime(buffer, sizeof(buffer), "%H:%M %d/%m/%Y", std::localtime(&time));
        return std::string(buffer);
    }
    
    std::string getStatusString(BookingStatus status) {
        switch(status) {
            case BookingStatus::PENDING: return "Pending";
            case BookingStatus::CONFIRMED: return "Confirmed";
            case BookingStatus::COMPLETED: return "Completed";
            case BookingStatus::CANCELLED: return "Cancelled";
            default: return "Unknown";
        }
    }
    
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    void pauseScreen() {
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
    
    int getIntInput(const std::string& prompt) {
        int value;
        std::cout << prompt;
        while (!(std::cin >> value)) {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(); // Clear the newline after the number
        return value;
    }
    
    double getDoubleInput(const std::string& prompt) {
        double value;
        std::cout << prompt;
        while (!(std::cin >> value)) {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(); // Clear the newline after the number
        return value;
    }
    
    void showLoginMenu() {
        std::cout << "\n=== LOGIN MENU ===\n";
        std::cout << "1. Login\n";
        std::cout << "2. Register\n";
        std::cout << "3. Exit\n";
        
        int choice = getIntInput("Choice: ");
        
        switch (choice) {
            case 1:
                handleLogin();
                break;
            case 2:
                handleRegister();
                break;
            case 3:
                m_running = false;
                std::cout << "Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice!\n";
                pauseScreen();
                break;
        }
    }
    
    void handleLogin() {
        std::string email, password;
        
        std::cout << "\n=== LOGIN ===\n";
        std::cout << "Email: ";
        std::getline(std::cin, email);
        std::cout << "Password: ";
        std::getline(std::cin, password);
        
        if (m_authController->login(email, password)) {
            m_currentUser = m_authController->getCurrentUser();
            std::cout << "Login successful! Welcome, " << m_currentUser->getFullName() << "\n";
            pauseScreen();
        } else {
            std::cout << "Login failed! Invalid credentials.\n";
            pauseScreen();
        }
    }
    
    void handleRegister() {
        std::string name, email, password, phone;
        
        std::cout << "\n=== REGISTER ===\n";
        std::cout << "Full Name: ";
        std::getline(std::cin, name);
        std::cout << "Email: ";
        std::getline(std::cin, email);
        std::cout << "Password: ";
        std::getline(std::cin, password);
        std::cout << "Phone Number: ";
        std::getline(std::cin, phone);
        
        // Default role is CUSTOMER
        UserRole role = UserRole::CUSTOMER;
        
        // Call registerUser with correct parameter order: email, password, fullName, phoneNumber, role
        if (m_authController->registerUser(email, password, name, phone, role)) {
            std::cout << "Registration successful! You can now login with your credentials.\n";
        } else {
            std::cout << "Registration failed! Email may already exist or validation failed.\n";
        }
        pauseScreen();
    }
    
    void showAdminMenu() {
        clearScreen();
        std::cout << "\n=== ADMIN MENU ===\n";
        std::cout << "Welcome, " << m_currentUser->getFullName() << " (Admin)\n\n";
        std::cout << "1. Manage Courts\n";
        std::cout << "2. View All Bookings\n";
        std::cout << "3. View Statistics\n";
        std::cout << "4. Manage Users\n";
        std::cout << "5. Logout\n";
        
        int choice = getIntInput("Choice: ");
        
        switch (choice) {
            case 1:
                showCourtManagement();
                break;
            case 2:
                showAllBookings();
                break;
            case 3:
                showStatistics();
                break;
            case 4:
                showUserManagement();
                break;
            case 5:
                logout();
                break;
            default:
                std::cout << "Invalid choice!\n";
                pauseScreen();
                break;
        }
    }
    
    void showCustomerMenu() {
        clearScreen();
        std::cout << "\n=== CUSTOMER MENU ===\n";
        std::cout << "Welcome, " << m_currentUser->getFullName() << "\n\n";
        std::cout << "1. View Available Courts\n";
        std::cout << "2. Make Booking\n";
        std::cout << "3. My Bookings\n";
        std::cout << "4. Cancel Booking\n";
        std::cout << "5. Logout\n";
        
        int choice = getIntInput("Choice: ");
        
        switch (choice) {
            case 1:
                showAvailableCourts();
                break;
            case 2:
                makeBooking();
                break;
            case 3:
                showMyBookings();
                break;
            case 4:
                cancelBooking();
                break;
            case 5:
                logout();
                break;
            default:
                std::cout << "Invalid choice!\n";
                pauseScreen();
                break;
        }
    }
    
    void showCourtManagement() {
        std::cout << "\n=== COURT MANAGEMENT ===\n";
        std::cout << "1. View All Courts\n";
        std::cout << "2. Add Court\n";
        std::cout << "3. Update Court\n";
        std::cout << "4. Delete Court\n";
        std::cout << "5. Back\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                viewAllCourts();
                break;
            case 2:
                addCourt();
                break;
            case 3:
                updateCourt();
                break;
            case 4:
                deleteCourt();
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice!\n";
                pauseScreen();
                break;
        }
    }
    
    void viewAllCourts() {
        auto courts = m_courtController->getAllCourts();
        
        std::cout << "\n=== ALL COURTS ===\n";
        std::cout << std::left << std::setw(5) << "ID" 
                  << std::setw(15) << "Name" 
                  << std::setw(25) << "Description" 
                  << std::setw(12) << "Rate/Hour"
                  << std::setw(12) << "Status" << "\n";
        std::cout << std::string(69, '-') << "\n";
        
        for (auto court : courts) {
            std::cout << std::left << std::setw(5) << court->getId()
                      << std::setw(15) << court->getName()
                      << std::setw(25) << court->getDescription()
                      << std::setw(12) << court->getHourlyRate()
                      << std::setw(12) << (court->getStatus() == CourtStatus::AVAILABLE ? "Available" : "Maintenance") << "\n";
        }
        pauseScreen();
    }
    
    void addCourt() {
        std::string name, description;
        
        std::cout << "\n=== ADD COURT ===\n";
        std::cout << "Court Name: ";
        std::getline(std::cin, name);
        std::cout << "Description: ";
        std::getline(std::cin, description);
        
        double rate = getDoubleInput("Hourly Rate: ");
        
        if (m_courtController->addCourt(name, description, rate)) {
            std::cout << "Court added successfully!\n";
        } else {
            std::cout << "Failed to add court!\n";
        }
        pauseScreen();
    }
    
    void updateCourt() {
        viewAllCourts();
        
        int courtId;
        std::cout << "Enter Court ID to update: ";
        std::cin >> courtId;
        std::cin.ignore();
        
        Court* court = m_courtController->getCourt(courtId);
        if (!court) {
            std::cout << "Court not found!\n";
            pauseScreen();
            return;
        }
        
        std::string name, description;
        double rate;
        
        std::cout << "Current: " << court->getName() << "\n";
        std::cout << "New Name (or press Enter to keep current): ";
        std::getline(std::cin, name);
        if (name.empty()) name = court->getName();
        
        std::cout << "Current: " << court->getDescription() << "\n";
        std::cout << "New Description (or press Enter to keep current): ";
        std::getline(std::cin, description);
        if (description.empty()) description = court->getDescription();
        
        std::cout << "Current Rate: " << court->getHourlyRate() << "\n";
        std::cout << "New Rate (or 0 to keep current): ";
        std::cin >> rate;
        if (rate == 0) rate = court->getHourlyRate();
        
        // Create a new court object with updated values
        Court updatedCourt(name, description, rate, court->getStatus());
        updatedCourt.setId(courtId);  // Set the ID manually
        if (m_courtController->updateCourt(courtId, updatedCourt)) {
            std::cout << "Court updated successfully!\n";
        } else {
            std::cout << "Failed to update court!\n";
        }
        pauseScreen();
    }
    
    void deleteCourt() {
        viewAllCourts();
        
        int courtId;
        std::cout << "Enter Court ID to delete: ";
        std::cin >> courtId;
        std::cin.ignore();
        
        if (m_courtController->deleteCourt(courtId)) {
            std::cout << "Court deleted successfully!\n";
        } else {
            std::cout << "Failed to delete court!\n";
        }
        pauseScreen();
    }
    
    void showAvailableCourts() {
        auto courts = m_courtController->getAvailableCourts();
        
        std::cout << "\n=== AVAILABLE COURTS ===\n";
        std::cout << std::left << std::setw(5) << "ID" 
                  << std::setw(15) << "Name" 
                  << std::setw(25) << "Description" 
                  << std::setw(12) << "Rate/Hour" << "\n";
        std::cout << std::string(57, '-') << "\n";
        
        for (auto court : courts) {
            std::cout << std::left << std::setw(5) << court->getId()
                      << std::setw(15) << court->getName()
                      << std::setw(25) << court->getDescription()
                      << std::setw(12) << court->getHourlyRate() << "\n";
        }
        pauseScreen();
    }
    
    void makeBooking() {
        showAvailableCourts();
        
        int courtId, hour, duration;
        std::string dateInput;
        
        std::cout << "\n=== MAKE BOOKING ===\n";
        std::cout << "Enter Court ID: ";
        std::cin >> courtId;
        std::cin.ignore(); // Clear input buffer
        
        std::cout << "Enter date (dd/mm/yyyy): ";
        std::getline(std::cin, dateInput);
        
        // Parse dd/mm/yyyy format
        std::tm timeinfo = {};
        int day, month, year;
        if (sscanf(dateInput.c_str(), "%d/%d/%d", &day, &month, &year) != 3) {
            std::cout << "Invalid date format! Please use dd/mm/yyyy format.\n";
            pauseScreen();
            return;
        }
        
        // Validate date values
        if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2024 || year > 2030) {
            std::cout << "Invalid date values! Please check your input.\n";
            pauseScreen();
            return;
        }
        
        std::cout << "Enter hour (6-22, where 6=6AM, 22=10PM): ";
        std::cin >> hour;
        
        // Validate hour is within allowed range (6AM to 10PM)
        if (hour < 6 || hour > 22) {
            std::cout << "Invalid hour! Bookings are only allowed from 6AM (6) to 10PM (22).\n";
            pauseScreen();
            return;
        }
        
        std::cout << "Enter duration (hours): ";
        std::cin >> duration;
        
        // Validate that booking doesn't extend beyond 10PM
        if (hour + duration > 22) {
            std::cout << "Invalid duration! Booking cannot extend beyond 10PM (22:00).\n";
            std::cout << "Maximum duration for hour " << hour << " is " << (22 - hour) << " hours.\n";
            pauseScreen();
            return;
        }
        
        std::cin.ignore(); // Clear input buffer
        
        // Create booking for specified date and time
        timeinfo.tm_year = year - 1900;  // tm_year is years since 1900
        timeinfo.tm_mon = month - 1;     // tm_mon is 0-11
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min = 0;
        timeinfo.tm_sec = 0;
        timeinfo.tm_isdst = -1; // Let mktime determine DST
        
        std::time_t startTime = std::mktime(&timeinfo);
        if (startTime == -1) {
            std::cout << "Invalid date/time entered!\n";
            pauseScreen();
            return;
        }
        
        std::time_t endTime = startTime + (duration * 3600); // duration in seconds
        
        std::string notes;
        std::cout << "Notes (optional): ";
        std::getline(std::cin, notes);
        
        // Get court information for display
        Court* court = m_courtController->getCourt(courtId);
        double totalCost = 0;
        if (court) {
            double hours = (endTime - startTime) / 3600.0; // Convert seconds to hours
            totalCost = court->getHourlyRate() * hours;
        }
        
        // Display booking summary
        std::cout << "\n--- Booking Summary ---\n";
        std::cout << "Court: " << (court ? court->getName() : "Unknown Court") << "\n";
        std::cout << "Start: " << formatDateTime(startTime) << "\n";
        std::cout << "End: " << formatDateTime(endTime) << "\n";
        std::cout << "Total Price: " << std::fixed << std::setprecision(0) << totalCost << " VND\n";
        std::cout << "Notes: " << (notes.empty() ? "None" : notes) << "\n";
        
        std::cout << "\nConfirm booking? (y/n): ";
        char confirm;
        std::cin >> confirm;
        std::cin.ignore();
        
        if (confirm == 'y' || confirm == 'Y') {
            if (m_bookingController->createBooking(m_currentUser->getId(), courtId, startTime, startTime, endTime, notes)) {
                std::cout << "Booking created successfully!\n";
            } else {
                std::cout << "Failed to create booking! Court may not be available at that time.\n";
            }
        } else {
            std::cout << "Booking cancelled.\n";
        }
        pauseScreen();
    }
    
    void showMyBookings() {
        auto bookings = m_bookingController->getUserBookings(m_currentUser->getId());
        
        std::cout << "\n=== MY BOOKINGS ===\n";
        if (bookings.empty()) {
            std::cout << "No bookings found.\n";
            pauseScreen();
            return;
        }
        
        std::cout << std::left << std::setw(5) << "ID" 
                  << std::setw(15) << "Court Name" 
                  << std::setw(18) << "Start Time"
                  << std::setw(18) << "End Time"
                  << std::setw(12) << "Status"
                  << std::setw(10) << "Amount" << "\n";
        std::cout << std::string(78, '-') << "\n";
        
        for (auto booking : bookings) {
            Court* court = m_courtController->getCourt(booking->getCourtId());
            std::string courtName = court ? court->getName() : "Unknown";
            
            std::cout << std::left << std::setw(5) << booking->getId()
                      << std::setw(15) << courtName
                      << std::setw(18) << formatDateTime(booking->getStartTime())
                      << std::setw(18) << formatDateTime(booking->getEndTime())
                      << std::setw(12) << getStatusString(booking->getStatus())
                      << std::setw(10) << std::fixed << std::setprecision(0) << booking->getTotalAmount() << "\n";
        }
        pauseScreen();
    }
    
    void cancelBooking() {
        std::cout << "\n=== CANCEL BOOKING ===\n";
        showMyBookings();
        
        if (m_bookingController->getUserBookings(m_currentUser->getId()).empty()) {
            return; // Already shown "No bookings found" message
        }
        
        int bookingId;
        std::cout << "\nEnter Booking ID to cancel (0 to go back): ";
        std::cin >> bookingId;
        std::cin.ignore();
        
        if (bookingId == 0) {
            return;
        }
        
        // Check if booking belongs to current user
        auto userBookings = m_bookingController->getUserBookings(m_currentUser->getId());
        bool found = false;
        for (auto booking : userBookings) {
            if (booking->getId() == bookingId) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::cout << "Booking not found or doesn't belong to you!\n";
            pauseScreen();
            return;
        }
        
        std::cout << "Are you sure you want to cancel this booking? (y/n): ";
        char confirm;
        std::cin >> confirm;
        std::cin.ignore();
        
        if (confirm == 'y' || confirm == 'Y') {
            if (m_bookingController->cancelBooking(bookingId)) {
                std::cout << "Booking cancelled successfully!\n";
            } else {
                std::cout << "Failed to cancel booking!\n";
            }
        } else {
            std::cout << "Cancellation aborted.\n";
        }
        pauseScreen();
    }
    
    void showAllBookings() {
        auto bookings = m_bookingController->getAllBookings();
        
        std::cout << "\n=== ALL BOOKINGS ===\n";
        if (bookings.empty()) {
            std::cout << "No bookings found.\n";
            pauseScreen();
            return;
        }
        
        std::cout << std::left << std::setw(5) << "ID" 
                  << std::setw(8) << "User ID" 
                  << std::setw(15) << "Court Name" 
                  << std::setw(18) << "Start Time"
                  << std::setw(18) << "End Time"
                  << std::setw(12) << "Status"
                  << std::setw(10) << "Amount" << "\n";
        std::cout << std::string(86, '-') << "\n";
        
        for (auto booking : bookings) {
            Court* court = m_courtController->getCourt(booking->getCourtId());
            std::string courtName = court ? court->getName() : "Unknown";
            
            std::cout << std::left << std::setw(5) << booking->getId()
                      << std::setw(8) << booking->getUserId()
                      << std::setw(15) << courtName
                      << std::setw(18) << formatDateTime(booking->getStartTime())
                      << std::setw(18) << formatDateTime(booking->getEndTime())
                      << std::setw(12) << getStatusString(booking->getStatus())
                      << std::setw(10) << std::fixed << std::setprecision(0) << booking->getTotalAmount() << "\n";
        }
        pauseScreen();
    }
    
    void showStatistics() {
        auto courts = m_courtController->getAllCourts();
        auto bookings = m_bookingController->getAllBookings();
        
        std::cout << "\n=== STATISTICS ===\n";
        std::cout << "Total Courts: " << courts.size() << "\n";
        std::cout << "Total Bookings: " << bookings.size() << "\n";
        
        // Calculate revenue
        double totalRevenue = 0;
        int completedBookings = 0;
        int confirmedBookings = 0;
        int pendingBookings = 0;
        int cancelledBookings = 0;
        
        for (auto booking : bookings) {
            switch(booking->getStatus()) {
                case BookingStatus::COMPLETED:
                    totalRevenue += booking->getTotalAmount();
                    completedBookings++;
                    break;
                case BookingStatus::CONFIRMED:
                    totalRevenue += booking->getTotalAmount();
                    confirmedBookings++;
                    break;
                case BookingStatus::PENDING:
                    pendingBookings++;
                    break;
                case BookingStatus::CANCELLED:
                    cancelledBookings++;
                    break;
            }
        }
        
        std::cout << "Booking Status Breakdown:\n";
        std::cout << "  - Completed: " << completedBookings << "\n";
        std::cout << "  - Confirmed: " << confirmedBookings << "\n";
        std::cout << "  - Pending: " << pendingBookings << "\n";
        std::cout << "  - Cancelled: " << cancelledBookings << "\n";
        std::cout << "Total Revenue: " << std::fixed << std::setprecision(0) << totalRevenue << " VND\n";
        pauseScreen();
    }
    
    void showUserManagement() {
        std::cout << "\n=== USER MANAGEMENT ===\n";
        std::cout << "1. View All Users\n";
        std::cout << "2. Delete User\n";
        std::cout << "3. Change User Role\n";
        std::cout << "4. Back\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                viewAllUsers();
                break;
            case 2:
                deleteUser();
                break;
            case 3:
                changeUserRole();
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice!\n";
                pauseScreen();
                break;
        }
    }
    
    void viewAllUsers() {
        auto users = m_authController->getAllUsers();
        
        std::cout << "\n=== ALL USERS ===\n";
        if (users.empty()) {
            std::cout << "No users found.\n";
            pauseScreen();
            return;
        }
        
        std::cout << std::left << std::setw(5) << "ID" 
                  << std::setw(20) << "Full Name" 
                  << std::setw(25) << "Email"
                  << std::setw(10) << "Role" << "\n";
        std::cout << std::string(60, '-') << "\n";
        
        for (auto user : users) {
            std::string role = (user->getRole() == UserRole::ADMIN) ? "Admin" : "Customer";
            std::cout << std::left << std::setw(5) << user->getId()
                      << std::setw(20) << user->getFullName()
                      << std::setw(25) << user->getEmail()
                      << std::setw(10) << role << "\n";
        }
        pauseScreen();
    }
    
    void deleteUser() {
        std::cout << "\n=== DELETE USER ===\n";
        viewAllUsers();
        
        int userId = 0;
        std::cout << "Enter User ID to delete (0 to cancel): ";
        std::cin >> userId;
        std::cin.ignore();
        
        if (userId == 0) {
            std::cout << "Operation cancelled.\n";
            pauseScreen();
            return;
        }
        
        if (userId == m_currentUser->getId()) {
            std::cout << "Cannot delete your own account!\n";
            pauseScreen();
            return;
        }
        
        auto users = m_authController->getAllUsers();
        bool userFound = false;
        std::string userName;
        
        for (auto user : users) {
            if (user->getId() == userId) {
                userFound = true;
                userName = user->getFullName();
                break;
            }
        }
        
        if (!userFound) {
            std::cout << "User not found!\n";
            pauseScreen();
            return;
        }
        
        std::cout << "Are you sure you want to delete user '" << userName << "'? (y/n): ";
        char confirm;
        std::cin >> confirm;
        std::cin.ignore();
        
        if (confirm == 'y' || confirm == 'Y') {
            if (m_authController->deleteUser(userId)) {
                std::cout << "User deleted successfully!\n";
            } else {
                std::cout << "Failed to delete user!\n";
            }
        } else {
            std::cout << "Deletion cancelled.\n";
        }
        pauseScreen();
    }
    
    void changeUserRole() {
        std::cout << "\n=== CHANGE USER ROLE ===\n";
        viewAllUsers();
        
        int userId = 0;
        std::cout << "Enter User ID to change role (0 to cancel): ";
        std::cin >> userId;
        std::cin.ignore();
        
        if (userId == 0) {
            std::cout << "Operation cancelled.\n";
            pauseScreen();
            return;
        }
        
        if (userId == m_currentUser->getId()) {
            std::cout << "Cannot change your own role!\n";
            pauseScreen();
            return;
        }
        
        auto users = m_authController->getAllUsers();
        User* targetUser = nullptr;
        
        for (auto user : users) {
            if (user->getId() == userId) {
                targetUser = user;
                break;
            }
        }
        
        if (!targetUser) {
            std::cout << "User not found!\n";
            pauseScreen();
            return;
        }
        
        std::string currentRole = (targetUser->getRole() == UserRole::ADMIN) ? "Admin" : "Customer";
        std::cout << "Current role: " << currentRole << "\n";
        std::cout << "1. Customer\n";
        std::cout << "2. Admin\n";
        std::cout << "Choose new role: ";
        
        int roleChoice;
        std::cin >> roleChoice;
        std::cin.ignore();
        UserRole newRole = (roleChoice == 2) ? UserRole::ADMIN : UserRole::CUSTOMER;
        
        if (newRole == targetUser->getRole()) {
            std::cout << "User already has this role!\n";
            pauseScreen();
            return;
        }
        
        targetUser->setRole(newRole);
        std::string newRoleStr = (newRole == UserRole::ADMIN) ? "Admin" : "Customer";
        std::cout << "User role changed to " << newRoleStr << " successfully!\n";
        pauseScreen();
    }
    
    void logout() {
        m_authController->logout();
        m_currentUser = nullptr;
        std::cout << "Logged out successfully!\n";
        pauseScreen();
    }
};

int main() {
    try {
        ConsoleInterface app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
