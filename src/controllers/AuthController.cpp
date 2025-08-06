#include "AuthController.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>

AuthController::AuthController() : m_currentUser(nullptr) {
}

AuthController::~AuthController() {
    saveUsers();
}

bool AuthController::login(const std::string& email, const std::string& password) {
    auto user = getUserByEmail(email);
    if (user && user->validatePassword(password) && user->isActive()) {
        m_currentUser = user;
        return true;
    }
    return false;
}

bool AuthController::logout() {
    if (m_currentUser) {
        m_currentUser = nullptr;
        return true;
    }
    return false;
}

bool AuthController::registerUser(const std::string& email, const std::string& password,
                                const std::string& fullName, const std::string& phoneNumber,
                                UserRole role) {
    // Validate input
    if (!validateEmail(email) || !validatePassword(password)) {
        return false;
    }
    
    if (isEmailTaken(email)) {
        return false;
    }
    
    // Create new user
    auto newUser = std::make_shared<User>(email, hashPassword(password), fullName, phoneNumber, role);
    newUser->setId(generateUserId());
    
    m_users.push_back(newUser);
    return true;
}

std::vector<UserPtr> AuthController::getAllUsers() const {
    return m_users;
}

UserPtr AuthController::getUserById(int userId) const {
    auto it = std::find_if(m_users.begin(), m_users.end(),
        [userId](const UserPtr& user) {
            return user->getId() == userId;
        });
    
    return (it != m_users.end()) ? *it : nullptr;
}

UserPtr AuthController::getUserByEmail(const std::string& email) const {
    auto it = std::find_if(m_users.begin(), m_users.end(),
        [&email](const UserPtr& user) {
            return user->getEmail() == email;
        });
    
    return (it != m_users.end()) ? *it : nullptr;
}

bool AuthController::updateUser(int userId, const User& updatedUser) {
    auto user = getUserById(userId);
    if (!user) return false;
    
    // Check if email is changing and if new email is available
    if (user->getEmail() != updatedUser.getEmail() && isEmailTaken(updatedUser.getEmail())) {
        return false;
    }
    
    // Update user data
    user->setEmail(updatedUser.getEmail());
    user->setFullName(updatedUser.getFullName());
    user->setPhoneNumber(updatedUser.getPhoneNumber());
    user->setRole(updatedUser.getRole());
    user->setActive(updatedUser.isActive());
    
    return true;
}

bool AuthController::deleteUser(int userId) {
    auto it = std::find_if(m_users.begin(), m_users.end(),
        [userId](const UserPtr& user) {
            return user->getId() == userId;
        });
    
    if (it != m_users.end()) {
        // Don't actually delete, just deactivate
        (*it)->setActive(false);
        return true;
    }
    
    return false;
}

bool AuthController::changePassword(int userId, const std::string& oldPassword, 
                                  const std::string& newPassword) {
    auto user = getUserById(userId);
    if (!user || !user->validatePassword(oldPassword)) {
        return false;
    }
    
    if (!validatePassword(newPassword)) {
        return false;
    }
    
    user->setPassword(hashPassword(newPassword));
    return true;
}

bool AuthController::validateEmail(const std::string& email) const {
    // Simple email validation using regex
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

bool AuthController::validatePassword(const std::string& password) const {
    // Password must be at least 6 characters long
    return password.length() >= 6;
}

bool AuthController::isEmailTaken(const std::string& email) const {
    return getUserByEmail(email) != nullptr;
}

void AuthController::loadUsers() {
    // Placeholder implementation for loading users from file
    // In a real application, this would read from a database or file
    std::ifstream file("users.dat");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // Parse user data from file
            // This is a simplified implementation
        }
        file.close();
    }
}

void AuthController::saveUsers() {
    // Placeholder implementation for saving users to file
    // In a real application, this would write to a database or file
    std::ofstream file("users.dat");
    if (file.is_open()) {
        for (const auto& user : m_users) {
            // Write user data to file
            // This is a simplified implementation
        }
        file.close();
    }
}

std::string AuthController::hashPassword(const std::string& password) const {
    // In a real application, use proper password hashing like bcrypt
    // This is a simplified implementation
    return password; // DO NOT USE IN PRODUCTION
}

bool AuthController::verifyPassword(const std::string& password, const std::string& hash) const {
    // In a real application, use proper password verification
    return password == hash; // DO NOT USE IN PRODUCTION
}

int AuthController::generateUserId() {
    int maxId = 0;
    for (const auto& user : m_users) {
        if (user->getId() > maxId) {
            maxId = user->getId();
        }
    }
    return maxId + 1;
}
