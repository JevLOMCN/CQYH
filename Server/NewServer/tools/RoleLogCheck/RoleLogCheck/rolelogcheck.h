#ifndef CQYH_ROLE_LOG_CHECK_H
#define CQYH_ROLE_LOG_CHECK_H

#include <chrono>
#include <cstdint>
#include <ctime>
#include <deque>
#include <expected>
#include <filesystem>
#include <map>
#include <string_view>
#include <system_error>

#include "std_type.h"

namespace role_log
{
enum class Action { add, subtract };
struct MoneyRecord
{
    std::int32_t role = 0;
    std::uint32_t time = 0;
    Action action = Action::add;
    std::int32_t type = 0;
    std::int32_t amount = 0;
};
struct LogoutRecord
{
    std::int32_t role = 0;
    std::uint32_t time = 0;
    std::int32_t balance = 0;
};
struct Summary
{
    std::size_t errors = 0;
    std::size_t discarded = 0;
    std::size_t total = 0;
};
// The text format is local wall time; preserve the legacy mktime/std-time convention.
std::expected<std::uint32_t, std::errc> ParseTime(std::string_view text);
}

time_t Str2Time(const char* text);
time_t CustomizedStr2Time(const char* text);

class CRoleLogCheck
{
public:
    CRoleLogCheck() = default;
    ~CRoleLogCheck();
    CRoleLogCheck(const CRoleLogCheck&) = delete;
    CRoleLogCheck& operator=(const CRoleLogCheck&) = delete;
    static CRoleLogCheck& GetInstance();
    BOOL Init();
    BOOL UnInit();
    BOOL CheckParam(INT32 argc, char** argv);
    void ShowUsage();
    BOOL DoCheckLog(const char* directory, const char* date,
        const char* second_directory, const char* second_date);
    const role_log::Summary& GetSummary() const noexcept { return m_summary; }

private:
    using MoneyRows = std::deque<role_log::MoneyRecord>;
    using LogoutRows = std::deque<role_log::LogoutRecord>;
    BOOL LoadMoney(const std::filesystem::path& path, bool gm);
    BOOL LoadBasic(const std::filesystem::path& path);
    void CheckLog();
    void ReleaseLogCache() noexcept;
    std::map<std::uint32_t, MoneyRows> m_money;
    std::map<std::uint32_t, MoneyRows> m_gm;
    std::map<std::uint32_t, LogoutRows> m_logouts;
    role_log::Summary m_summary;
    bool m_initialized = false;
};

class COutput
{
public:
    static COutput& GetInstance();
    void Cls();
    void Goto(short x, short y);
    void Printf(const char* format, ...);
    void PrintfLoadingProgress(INT32 percent);
};

#endif
