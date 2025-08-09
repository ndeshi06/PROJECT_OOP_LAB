#pragma once
#include <string>
#include <vector>

class Database
{
private:
    std::string m_connectionString;
    bool m_isConnected;

public:
    Database();
    Database(const std::string &connectionString);
    ~Database();

    // Connection management
    bool connect();
    bool connect(const std::string &connectionString);
    void disconnect();
    bool isConnected() const { return m_isConnected; }

    // Basic operations
    bool execute(const std::string &query);
    std::vector<std::vector<std::string>> query(const std::string &sql);

    // File-based operations (fallback)
    bool saveToFile(const std::string &filename, const std::string &data);
    std::string loadFromFile(const std::string &filename);

    // Utility methods
    std::string escapeString(const std::string &str);
    bool tableExists(const std::string &tableName);
    bool createTable(const std::string &createTableSQL);

private:
    void initialize();
};
