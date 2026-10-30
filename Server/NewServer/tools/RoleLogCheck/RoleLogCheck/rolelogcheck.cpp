#include "stdafx.h"
#include "rolelogcheck.h"
#include "tabfile.h"
#include "time_ex.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <format>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>

namespace
{
constexpr int gold = 1;

void progress(std::size_t completed, std::size_t total, int& previous)
{
    const int percent = total == 0 ? 100 : static_cast<int>(completed * 100 / total);
    if (percent != previous)
    {
        COutput::GetInstance().PrintfLoadingProgress(percent);
        previous = percent;
    }
}

std::expected<int, std::errc> integer(std::string_view text)
{
    if (text.empty())
        return std::unexpected(std::errc::invalid_argument);
    int result = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), result);
    if (error != std::errc{} || end != text.data() + text.size())
        return std::unexpected(error == std::errc{} ? std::errc::invalid_argument : error);
    return result;
}

std::optional<std::chrono::year_month_day> file_date(std::string_view text)
{
    if (text.size() != 10 || text[4] != '_' || text[7] != '_')
        return {};
    const auto year = integer(text.substr(0, 4));
    const auto month = integer(text.substr(5, 2));
    const auto day = integer(text.substr(8, 2));
    if (!year || !month || !day || *year < 1970 || *year > 2106 || *month < 1 || *month > 12 || *day < 1 || *day > 31)
        return {};
    const std::chrono::year_month_day date{std::chrono::year{*year},
        std::chrono::month{static_cast<unsigned>(*month)}, std::chrono::day{static_cast<unsigned>(*day)}};
    return date.ok() ? std::optional(date) : std::nullopt;
}

std::expected<std::uint32_t, std::string_view> row_time(CTabFile& table, int row)
{
    std::array<char, 64> text{};
    if (!table.ReadData(0, row, text.data(), static_cast<int>(text.size())))
        return std::unexpected("Missing timestamp");
    const auto time = CustomizedStr2Time(text.data());
    if (time == 0)
        return std::unexpected("Invalid timestamp");
    return static_cast<std::uint32_t>(time);
}

std::expected<role_log::MoneyRecord, std::string_view> money_row(CTabFile& table, int row)
{
    role_log::MoneyRecord result;
    const auto time = row_time(table, row);
    if (!time)
        return std::unexpected(time.error());
    result.time = *time;
    int source = 0, target = 0;
    std::array<char, 64> action{};
    if (!table.ReadData(3, row, source) || !table.ReadData(4, row, target) ||
        !table.ReadData(5, row, action.data(), static_cast<int>(action.size())) ||
        !table.ReadData(6, row, result.type) || !table.ReadData(7, row, result.amount))
        return std::unexpected("Invalid money record field");
    const std::string_view verb(action.data());
    if (verb == "Add")
    {
        result.action = role_log::Action::add;
        result.role = target;
    }
    else if (verb == "Sub")
    {
        result.action = role_log::Action::subtract;
        result.role = source;
    }
    else
        return std::unexpected("Invalid money action");
    return result;
}

std::expected<role_log::LogoutRecord, std::string_view> logout_row(CTabFile& table, int row)
{
    role_log::LogoutRecord result;
    const auto time = row_time(table, row);
    if (!time)
        return std::unexpected(time.error());
    result.time = *time;
    if (!table.ReadData(3, row, result.role) || !table.ReadData(13, row, result.balance))
        return std::unexpected("Invalid logout record field");
    return result;
}

std::int64_t add(std::int64_t left, std::int64_t right)
{
    if ((right > 0 && left > std::numeric_limits<std::int64_t>::max() - right) ||
        (right < 0 && left < std::numeric_limits<std::int64_t>::min() - right))
        throw std::overflow_error("Money accumulation exceeds 64-bit range");
    return left + right;
}

std::int64_t change(const role_log::MoneyRecord& record)
{
    const auto amount = static_cast<std::int64_t>(record.amount);
    return record.action == role_log::Action::add ? amount : -amount;
}
}

std::expected<std::uint32_t, std::errc> role_log::ParseTime(std::string_view text)
{
    if (text.size() != 19 || text[4] != '-' || text[7] != '-' || text[10] != ' ' || text[13] != ':' || text[16] != ':')
        return std::unexpected(std::errc::invalid_argument);
    const auto year = integer(text.substr(0, 4));
    const auto month = integer(text.substr(5, 2));
    const auto day = integer(text.substr(8, 2));
    const auto hour = integer(text.substr(11, 2));
    const auto minute = integer(text.substr(14, 2));
    const auto second = integer(text.substr(17, 2));
    if (!year || !month || !day || !hour || !minute || !second || *year < 1970 || *year > 2106 ||
        *month < 1 || *month > 12 || *day < 1 || *day > 31 || *hour < 0 || *hour > 23 ||
        *minute < 0 || *minute > 59 || *second < 0 || *second > 59)
        return std::unexpected(std::errc::invalid_argument);
    const std::chrono::year_month_day date{std::chrono::year{*year},
        std::chrono::month{static_cast<unsigned>(*month)}, std::chrono::day{static_cast<unsigned>(*day)}};
    if (!date.ok())
        return std::unexpected(std::errc::invalid_argument);
    std::tm local{};
    local.tm_year = *year - 1900;
    local.tm_mon = *month - 1;
    local.tm_mday = *day;
    local.tm_hour = *hour;
    local.tm_min = *minute;
    local.tm_sec = *second;
    local.tm_isdst = 0; // Preserve the original local-time convention.
    const auto value = std::mktime(&local);
    if (value <= 0 || static_cast<std::uint64_t>(value) > std::numeric_limits<std::uint32_t>::max())
        return std::unexpected(std::errc::result_out_of_range);
    return static_cast<std::uint32_t>(value);
}

time_t Str2Time(const char* text)
{
    if (!text)
        return 0;
    const auto result = role_log::ParseTime(text);
    return result ? static_cast<time_t>(*result) : 0;
}

time_t CustomizedStr2Time(const char* text)
{
    if (!text)
        return 0;
    const std::string_view source(text);
    if (source.size() != 19)
        return 0;
    thread_local std::array<char, 19> cached{};
    thread_local time_t value = 0;
    if (value != 0 && std::ranges::equal(source, cached))
        return value;
    const auto result = role_log::ParseTime(source);
    if (!result)
        return 0;
    std::ranges::copy(source, cached.begin());
    value = *result;
    return value;
}

CRoleLogCheck::~CRoleLogCheck() { UnInit(); }
CRoleLogCheck& CRoleLogCheck::GetInstance()
{
    static CRoleLogCheck instance;
    return instance;
}
BOOL CRoleLogCheck::Init()
{
    if (m_initialized)
        return FALSE;
    m_summary = {};
    m_initialized = true;
    return TRUE;
}
BOOL CRoleLogCheck::UnInit()
{
    ReleaseLogCache();
    m_initialized = false;
    return TRUE;
}
void CRoleLogCheck::ReleaseLogCache() noexcept
{
    m_money.clear();
    m_gm.clear();
    m_logouts.clear();
}

BOOL CRoleLogCheck::DoCheckLog(const char* directory, const char* date,
    const char* second_directory, const char* second_date)
{
    m_summary = {};
    ReleaseLogCache();
    if (!m_initialized || !directory || !date || (second_directory == nullptr) != (second_date == nullptr))
        return FALSE;
    const auto first = file_date(date);
    const auto second = second_date ? file_date(second_date) : std::optional<std::chrono::year_month_day>{};
    if (!first || (second_date && (!second || *first >= *second)))
        return FALSE;
    BOOL result = FALSE;
    try
    {
        auto load = [&](const char* prefix, bool basic, bool gm)
        {
            auto one = [&](const char* folder, const char* day)
            {
                const auto path = std::filesystem::path(folder) / std::format("{}_{}.txt", prefix, day);
                return basic ? LoadBasic(path) : LoadMoney(path, gm);
            };
            return (!second_directory || one(second_directory, second_date)) && one(directory, date);
        };
        if (load("Money", false, false) && load("gmmoney", false, true) && load("RoleLogout_Basic", true, false))
        {
            CheckLog();
            result = TRUE;
        }
    }
    catch (const std::exception& error)
    {
        ERR("Role log check failed: %s", error.what());
    }
    ReleaseLogCache();
    if (!result)
        m_summary = {};
    return result;
}

BOOL CRoleLogCheck::LoadMoney(const std::filesystem::path& path, bool gm)
{
    const auto name = path.string();
    COutput::GetInstance().Printf("Opening file %s", name.c_str());
    CTabFile table;
    if (!table.Init(name.c_str(), 0) || table.GetColCount() < 8)
        return FALSE;
    COutput::GetInstance().Printf("Start loading ...");
    auto& destination = gm ? m_gm : m_money;
    const int rows = table.GetRowCount();
    int previous = -1;
    for (int row = rows; row-- > 0;)
    {
        const auto record = money_row(table, row);
        if (!record)
        {
            ERR("Failed to read row %d in %s: %.*s", row + 2, name.c_str(), static_cast<int>(record.error().size()), record.error().data());
            return FALSE;
        }
        if (!gm || record->type == gold)
            destination[static_cast<std::uint32_t>(record->role)].push_front(*record);
        progress(static_cast<std::size_t>(rows - row), rows, previous);
    }
    progress(rows, rows, previous);
    return TRUE;
}

BOOL CRoleLogCheck::LoadBasic(const std::filesystem::path& path)
{
    const auto name = path.string();
    COutput::GetInstance().Printf("Opening file %s", name.c_str());
    CTabFile table;
    if (!table.Init(name.c_str(), 0) || table.GetColCount() < 14)
        return FALSE;
    COutput::GetInstance().Printf("Start loading ...");
    const int rows = table.GetRowCount();
    int previous = -1;
    for (int row = rows; row-- > 0;)
    {
        const auto record = logout_row(table, row);
        if (!record)
        {
            ERR("Failed to read row %d in %s: %.*s", row + 2, name.c_str(), static_cast<int>(record.error().size()), record.error().data());
            return FALSE;
        }
        m_logouts[static_cast<std::uint32_t>(record->role)].push_front(*record);
        progress(static_cast<std::size_t>(rows - row), rows, previous);
    }
    progress(rows, rows, previous);
    return TRUE;
}

void CRoleLogCheck::CheckLog()
{
    COutput::GetInstance().Printf("Start checking ...");
    std::size_t completed = 0;
    int previous_progress = -1;
    for (const auto& [role, logouts] : m_logouts)
    {
        m_summary.total += logouts.size();
        const auto money_found = m_money.find(role);
        const auto gm_found = m_gm.find(role);
        const auto* money = money_found == m_money.end() ? nullptr : &money_found->second;
        const auto* gm = gm_found == m_gm.end() ? nullptr : &gm_found->second;
        if (logouts.empty() || ((!money || money->empty()) && (!gm || gm->empty())))
        {
            progress(++completed, m_logouts.size(), previous_progress);
            continue;
        }
        std::size_t money_index = 0, gm_index = 0, logout_index = 1, interval_count = 0;
        auto last_money = static_cast<std::int64_t>(logouts.front().balance);
        auto last_time = logouts.front().time;
        bool at_current = false, at_last = false;
        while (logout_index < logouts.size())
        {
            ++interval_count;
            const auto* current = &logouts[logout_index];
            auto current_money = static_cast<std::int64_t>(current->balance);
            auto current_time = current->time;
            std::int64_t difference = 0;
            if (money)
            {
                while (money_index < money->size())
                {
                    const auto& event = (*money)[money_index];
                    if (event.type != gold)
                    {
                        ++money_index;
                        continue;
                    }
                    if (event.time > current_time)
                    {
                        if (!at_current)
                            break;
                        ++interval_count;
                        if (++logout_index == logouts.size())
                            break;
                        at_current = false;
                        current = &logouts[logout_index];
                        current_money = current->balance;
                        current_time = current->time;
                    }
                    if (event.time > current_time)
                        break;
                    if (last_time == event.time)
                    {
                        at_last = true;
                        if (++logout_index == logouts.size())
                            break;
                        ++m_summary.discarded;
                        at_last = false;
                        last_money = current->balance;
                        last_time = current->time;
                        current = &logouts[logout_index];
                        current_money = current->balance;
                        current_time = current->time;
                        difference = 0;
                    }
                    if (last_time < event.time && event.time <= current_time)
                    {
                        difference = add(difference, change(event));
                        if (event.time == current_time)
                            at_current = true;
                    }
                    ++money_index;
                }
            }
            // Retain the original same-second ambiguity/discard rules.
            if (at_last)
            {
                ++m_summary.discarded;
                break;
            }
            if (at_current)
            {
                m_summary.discarded += interval_count;
                break;
            }
            if (gm)
            {
                while (gm_index < gm->size())
                {
                    const auto& event = (*gm)[gm_index];
                    if (event.time > current_time)
                        break;
                    if (last_time < event.time && event.time <= current_time && event.type == gold)
                        difference = add(difference, change(event));
                    ++gm_index;
                }
            }
            const auto predicted = add(last_money, difference);
            if (predicted != current_money)
            {
                const std::string time = get_time_str(current_time);
                const std::string previous = get_time_str(last_time);
                ERR("*** log data error, role_id=%d, last_logout_time=%s, logout_time=%s, last_logout_money=%lld, logout_money=%lld, check_money=%lld, diff_money=%lld ***",
                    current->role, previous.c_str(), time.c_str(), static_cast<long long>(last_money),
                    static_cast<long long>(current_money), static_cast<long long>(predicted), static_cast<long long>(difference));
                ++m_summary.errors;
            }
            last_money = current->balance;
            last_time = current->time;
            interval_count = 0;
            ++logout_index;
        }
        progress(++completed, m_logouts.size(), previous_progress);
    }
    progress(completed, m_logouts.size(), previous_progress);
    if (m_summary.errors)
        COutput::GetInstance().Printf("*** check finish, error log count %zu ***", m_summary.errors);
    else
        COutput::GetInstance().Printf("check finish, error log count %zu.", m_summary.errors);
    COutput::GetInstance().Printf("logout data discard %zu, total %zu.", m_summary.discarded, m_summary.total);
}

void CRoleLogCheck::ShowUsage()
{
    COutput::GetInstance().Printf("usage: RoleLogCheck dir1 date1 [dir2 date2]");
    COutput::GetInstance().Printf("Dates use YYYY_MM_DD; date2 must be later than date1.");
}
BOOL CRoleLogCheck::CheckParam(INT32 argc, char** argv)
{
    if (!argv || (argc != 3 && argc != 5) || !argv[1] || !*argv[1] || !argv[2] || !file_date(argv[2]))
    {
        ShowUsage();
        return FALSE;
    }
    if (argc == 5)
    {
        if (!argv[3] || !*argv[3] || !argv[4])
            return FALSE;
        const auto first = file_date(argv[2]);
        const auto second = file_date(argv[4]);
        if (!second || *first >= *second)
        {
            ShowUsage();
            return FALSE;
        }
    }
    return TRUE;
}

COutput& COutput::GetInstance()
{
    static COutput instance;
    return instance;
}
void COutput::Cls()
{
    const HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (!GetConsoleScreenBufferInfo(console, &info))
        return;
    const COORD origin{};
    const DWORD size = static_cast<DWORD>(info.dwSize.X) * info.dwSize.Y;
    DWORD written = 0;
    FillConsoleOutputCharacterA(console, ' ', size, origin, &written);
    FillConsoleOutputAttribute(console, info.wAttributes, size, origin, &written);
    SetConsoleCursorPosition(console, origin);
}
void COutput::Goto(short x, short y)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{x, y});
}
void COutput::Printf(const char* format, ...)
{
    if (!format)
        return;
    va_list arguments;
    va_start(arguments, format);
    std::string text;
    try
    {
        va_list copy;
        va_copy(copy, arguments);
        const int size = std::vsnprintf(nullptr, 0, format, copy);
        va_end(copy);
        if (size < 0)
        {
            va_end(arguments);
            return;
        }
        text.resize(static_cast<std::size_t>(size));
        std::vsnprintf(text.data(), text.size() + 1, format, arguments);
    }
    catch (...)
    {
        va_end(arguments);
        throw;
    }
    va_end(arguments);
    std::printf("%s\n", text.c_str());
    ERR("%s", text.c_str());
}
void COutput::PrintfLoadingProgress(INT32 percent)
{
    std::printf("\r                                                                               \rProgressing %d%% ...%s", percent, percent == 100 ? "\n" : "");
}
