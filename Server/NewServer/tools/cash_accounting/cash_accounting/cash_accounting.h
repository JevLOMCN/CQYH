#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <list>
#include <string>
#include <vector>

#include "std_type.h"
#include "process_error.h"
#include "sql_mgr.h"

struct CashDatabaseSettings
{
    std::string server;
    std::string user;
    std::string password;
    std::string database;
    std::string charset;
    std::string log_database;
    std::uint32_t port = 3306;
};

struct CashAccountingSummary
{
    std::size_t intervals = 0;
    std::size_t successes = 0;
    std::size_t failures = 0;
};

class CCashAccounting
{
public:
    CCashAccounting() = default;
    ~CCashAccounting();
    CCashAccounting(const CCashAccounting&) = delete;
    CCashAccounting& operator=(const CCashAccounting&) = delete;
    static CCashAccounting& GetInstance();
    BOOL Init();
    BOOL Init(const CashDatabaseSettings& settings, const std::filesystem::path& output = "money_accounting");
    BOOL UnInit();
    BOOL MainLoop();
    const CashAccountingSummary& GetSummary() const noexcept { return m_summary; }

private:
    struct Snapshot
    {
        std::uint32_t role = 0;
        std::uint32_t time = 0;
        std::array<std::int32_t, 2> money{};
        std::string name;
    };
    BOOL Load(std::list<void*>& rows);
    BOOL Check(std::list<void*>& rows, const Snapshot& begin, const Snapshot& end);
    std::vector<Snapshot> m_snapshots;
    std::vector<std::size_t> m_next;
    std::size_t m_cursor = 0;
    std::ofstream m_log;
    CashAccountingSummary m_summary;
    bool m_initialized = false;
    // Close SQL callbacks/workers before the data and output they can reference.
    CSqlMgr m_sql;
};
