#pragma once
#include <string>
#include <memory>
#include <ctime>

enum class UserRole {
    ADMIN,
    CUSTOMER
};

class User {
private:
    int m_id;
    std::string m_email;
    std::string m_password;
    std::string m_fullName;
    std::string m_phoneNumber;
    UserRole m_role;
    std::time_t m_createdAt;
    bool m_isActive;

public:
    User();
    User(const std::string& email, const std::string& password, 
         const std::string& fullName, const std::string& phoneNumber, 
         UserRole role = UserRole::CUSTOMER);

    // Getters
    int getId() const { return m_id; }
    const std::string& getEmail() const { return m_email; }
    const std::string& getPassword() const { return m_password; }
    const std::string& getFullName() const { return m_fullName; }
    const std::string& getUsername() const { return m_email; } // Use email as username
    const std::string& getPhoneNumber() const { return m_phoneNumber; }
    UserRole getRole() const { return m_role; }
    std::time_t getCreatedAt() const { return m_createdAt; }
    bool isActive() const { return m_isActive; }

    // Setters
    void setId(int id) { m_id = id; }
    void setEmail(const std::string& email) { m_email = email; }
    void setPassword(const std::string& password) { m_password = password; }
    void setFullName(const std::string& fullName) { m_fullName = fullName; }
    void setPhoneNumber(const std::string& phoneNumber) { m_phoneNumber = phoneNumber; }
    void setRole(UserRole role) { m_role = role; }
    void setActive(bool active) { m_isActive = active; }

    // Utility methods
    bool isAdmin() const { return m_role == UserRole::ADMIN; }
    bool validatePassword(const std::string& password) const;
    std::string getRoleString() const;
};

using UserPtr = std::shared_ptr<User>;
