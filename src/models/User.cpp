#include "User.h"
#include <ctime>
#include <algorithm>

User::User() 
    : m_id(0), m_role(UserRole::CUSTOMER), m_createdAt(std::time(nullptr)), m_isActive(true) {
}

User::User(const std::string& email, const std::string& password, 
           const std::string& fullName, const std::string& phoneNumber, UserRole role)
    : m_id(0), m_email(email), m_password(password), m_fullName(fullName), 
      m_phoneNumber(phoneNumber), m_role(role), m_createdAt(std::time(nullptr)), m_isActive(true) {
}

bool User::validatePassword(const std::string& password) const {
    return m_password == password; // In real application, use proper hashing
}

std::string User::getRoleString() const {
    switch (m_role) {
        case UserRole::ADMIN:
            return "ADMIN";
        case UserRole::STAFF:
            return "STAFF";
        case UserRole::CUSTOMER:
            return "CUSTOMER";
        default:
            return "CUSTOMER";
    }
}
