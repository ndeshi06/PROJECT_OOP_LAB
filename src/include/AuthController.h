#pragma once
#include "User.h"
#include <string>
#include <vector>
#include <memory>

class AuthController
{
private:
    UserPtr m_currentUser;
    std::vector<UserPtr> m_users;

public:
    AuthController();
    ~AuthController();

    // Authentication methods
    bool login(const std::string &email, const std::string &password);
    bool logout();
    bool registerUser(const std::string &email, const std::string &password,
                      const std::string &fullName, const std::string &phoneNumber,
                      UserRole role = UserRole::CUSTOMER);

    // User management
    UserPtr getCurrentUser() const { return m_currentUser; }
    bool isLoggedIn() const { return m_currentUser != nullptr; }
    bool isAdmin() const { return m_currentUser && m_currentUser->isAdmin(); }

    // User operations
    std::vector<UserPtr> getAllUsers() const;
    UserPtr getUserById(int userId) const;
    UserPtr getUserByEmail(const std::string &email) const;
    bool updateUser(int userId, const User &updatedUser);
    bool deleteUser(int userId);
    bool changeUserRole(int userId, UserRole newRole);
    bool toggleUserStatus(int userId);
    bool changePassword(int userId, const std::string &oldPassword,
                        const std::string &newPassword);

    // Validation
    bool validateEmail(const std::string &email) const;
    bool validatePassword(const std::string &password) const;
    bool isEmailTaken(const std::string &email) const;

    // Data persistence
    void loadUsers();
    void saveUsers();

private:
    // Helper methods
    int generateUserId();
};
