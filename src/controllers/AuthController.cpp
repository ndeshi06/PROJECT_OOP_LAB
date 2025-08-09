#include "AuthController.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>

AuthController::AuthController() : m_currentUser(nullptr)
{
    loadUsers();

    // Clean up any duplicate admin@badminton.com entries first
    std::vector<std::shared_ptr<User>> adminDuplicates;
    for (auto it = m_users.begin(); it != m_users.end();)
    {
        if ((*it) && (*it)->getEmail() == "admin@badminton.com")
        {
            adminDuplicates.push_back(*it);
            it = m_users.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Check if we have any admin user at all after cleanup
    bool hasAnyAdmin = false;
    for (const auto &user : m_users)
    {
        if (user && user->getRole() == UserRole::ADMIN)
        {
            hasAnyAdmin = true;
            break;
        }
    }

    // Create default admin user only if NO admin exists at all
    if (!hasAnyAdmin)
    {
        // Create new admin with correct role
        auto defaultAdmin = std::make_shared<User>("admin@badminton.com",
                                                   "admin123",
                                                   "Administrator",
                                                   "0123456789",
                                                   UserRole::ADMIN);
        defaultAdmin->setId(generateUserId());
        m_users.push_back(defaultAdmin);
        saveUsers();
    }
    else if (!adminDuplicates.empty())
    {
        m_users.push_back(adminDuplicates[0]); // Keep only the first one
        saveUsers();
    }
}

AuthController::~AuthController()
{
    saveUsers();
}

bool AuthController::login(const std::string &email, const std::string &password)
{
    auto user = getUserByEmail(email);
    if (user && user->validatePassword(password) && user->isActive())
    {
        m_currentUser = user;
        return true;
    }
    return false;
}

bool AuthController::logout()
{
    if (m_currentUser)
    {
        m_currentUser = nullptr;
        return true;
    }
    return false;
}

bool AuthController::registerUser(const std::string &email, const std::string &password,
                                  const std::string &fullName, const std::string &phoneNumber,
                                  UserRole role)
{
    // Validate input
    if (!validateEmail(email) || !validatePassword(password))
    {
        return false;
    }

    if (isEmailTaken(email))
    {
        return false;
    }

    // Create new user
    auto newUser = std::make_shared<User>(email, password, fullName, phoneNumber, role);
    newUser->setId(generateUserId());

    m_users.push_back(newUser);
    saveUsers(); // Save changes immediately
    return true;
}

std::vector<UserPtr> AuthController::getAllUsers() const
{
    return m_users;
}

UserPtr AuthController::getUserById(int userId) const
{
    auto it = std::find_if(m_users.begin(), m_users.end(),
                           [userId](const UserPtr &user)
                           {
                               return user->getId() == userId;
                           });

    return (it != m_users.end()) ? *it : nullptr;
}

UserPtr AuthController::getUserByEmail(const std::string &email) const
{
    auto it = std::find_if(m_users.begin(), m_users.end(),
                           [&email](const UserPtr &user)
                           {
                               return user->getEmail() == email;
                           });

    return (it != m_users.end()) ? *it : nullptr;
}

bool AuthController::updateUser(int userId, const User &updatedUser)
{
    auto user = getUserById(userId);
    if (!user)
        return false;

    // Check if email is changing and if new email is available
    if (user->getEmail() != updatedUser.getEmail() && isEmailTaken(updatedUser.getEmail()))
    {
        return false;
    }

    // Update user data
    user->setEmail(updatedUser.getEmail());
    user->setFullName(updatedUser.getFullName());
    user->setPhoneNumber(updatedUser.getPhoneNumber());
    user->setRole(updatedUser.getRole());
    user->setActive(updatedUser.isActive());

    saveUsers(); // Save changes immediately
    return true;
}

bool AuthController::deleteUser(int userId)
{
    auto it = std::find_if(m_users.begin(), m_users.end(),
                           [userId](const UserPtr &user)
                           {
                               return user->getId() == userId;
                           });

    if (it != m_users.end())
    {
        // Actually remove the user from the list
        m_users.erase(it);
        saveUsers(); // Save changes immediately
        return true;
    }

    return false;
}

bool AuthController::changeUserRole(int userId, UserRole newRole)
{
    auto user = getUserById(userId);
    if (!user)
        return false;

    user->setRole(newRole);
    saveUsers(); // Save changes immediately
    return true;
}

bool AuthController::toggleUserStatus(int userId)
{
    auto user = getUserById(userId);
    if (!user)
        return false;

    user->setActive(!user->isActive());
    saveUsers(); // Save changes immediately
    return true;
}

bool AuthController::changePassword(int userId, const std::string &oldPassword,
                                    const std::string &newPassword)
{
    auto user = getUserById(userId);
    if (!user || !user->validatePassword(oldPassword))
    {
        return false;
    }

    if (!validatePassword(newPassword))
    {
        return false;
    }

    user->setPassword(newPassword);
    return true;
}

bool AuthController::validateEmail(const std::string &email) const
{
    // Simple email validation using regex
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

bool AuthController::validatePassword(const std::string &password) const
{
    // Password must be at least 6 characters long
    return password.length() >= 6;
}

bool AuthController::isEmailTaken(const std::string &email) const
{
    return getUserByEmail(email) != nullptr;
}

void AuthController::loadUsers()
{
    // Load users from file
    std::ifstream file("data/users.txt");
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            // Parse user data: id|email|password|fullName|phone|role|active|createdAt
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(ss, token, '|'))
            {
                tokens.push_back(token);
            }

            if (tokens.size() >= 8)
            {
                try
                {
                    auto user = std::make_shared<User>();
                    user->setId(std::stoi(tokens[0]));
                    user->setEmail(tokens[1]);
                    user->setPassword(tokens[2]);
                    user->setFullName(tokens[3]);
                    user->setPhoneNumber(tokens[4]);

                    // Parse role
                    UserRole role = UserRole::CUSTOMER; // Default
                    if (tokens[5] == "ADMIN")
                    {
                        role = UserRole::ADMIN;
                    }
                    else if (tokens[5] == "STAFF")
                    {
                        role = UserRole::STAFF;
                    }
                    else if (tokens[5] == "CUSTOMER")
                    {
                        role = UserRole::CUSTOMER;
                    }
                    user->setRole(role);

                    user->setActive(tokens[6] == "1");

                    // Set created time
                    std::time_t createdAt = std::time(nullptr);
                    if (tokens.size() > 7)
                    {
                        createdAt = std::stoll(tokens[7]);
                    }

                    m_users.push_back(user);
                }
                catch (const std::exception &e)
                {
                    // Skip invalid lines
                    continue;
                }
            }
        }
        file.close();
    }
}

void AuthController::saveUsers()
{
    // Create data directory if it doesn't exist
    std::filesystem::create_directories("data");

    // Save users to file
    std::ofstream file("data/users.txt");
    file.clear();
    if (file.is_open())
    {
        for (const auto &user : m_users)
        {
            if (!user)
                continue;

            // Write user data: id|email|password|fullName|phone|role|active|createdAt
            file << user->getId() << "|"
                 << user->getEmail() << "|"
                 << user->getPassword() << "|"
                 << user->getFullName() << "|"
                 << user->getPhoneNumber() << "|"
                 << user->getRoleString() << "|"
                 << (user->isActive() ? "1" : "0") << "|"
                 << user->getCreatedAt() << std::endl;
        }
        file.close();
    }
}

int AuthController::generateUserId()
{
    int maxId = 0;
    for (const auto &user : m_users)
    {
        if (user->getId() > maxId)
        {
            maxId = user->getId();
        }
    }
    return maxId + 1;
}
