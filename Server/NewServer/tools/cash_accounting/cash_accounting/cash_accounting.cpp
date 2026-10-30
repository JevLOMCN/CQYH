#include "stdafx.h"
#include "cash_accounting.h"
#include "cash_accounting_stmt.list.h"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <format>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace
{
#pragma pack(push, 1)
struct SnapshotRow
{
    std::int64_t time;
    std::int64_t role;
    std::int32_t money0;
    std::int32_t money1;
    char name[22]; // {s21} reserves 21 data bytes and one terminator.
};
struct TransactionRow
{
    std::int64_t source;
    std::int64_t target;
    std::int8_t type;
    std::int32_t amount;
};
#pragma pack(pop)
static_assert(sizeof(SnapshotRow) == 46 && sizeof(TransactionRow) == 21);
static_assert(std::is_trivially_copyable_v<SnapshotRow> && std::is_trivially_copyable_v<TransactionRow>);
constexpr auto no_next = std::numeric_limits<std::size_t>::max();

std::uint32_t identifier(std::int64_t value)
{
    if (value < 0 || static_cast<std::uint64_t>(value) > std::numeric_limits<std::uint32_t>::max())
        throw std::runtime_error("SQL role ID or timestamp is outside the 32-bit data range");
    return static_cast<std::uint32_t>(value);
}

std::int64_t add(std::int64_t left, std::int64_t right)
{
    if ((right > 0 && left > std::numeric_limits<std::int64_t>::max() - right) ||
        (right < 0 && left < std::numeric_limits<std::int64_t>::min() - right))
        throw std::overflow_error("Money accumulation exceeds 64-bit range");
    return left + right;
}

std::string time_text(std::uint32_t stamp)
{
    const time_t value = stamp;
    std::tm local{};
    if (localtime_s(&local, &value) != 0)
        throw std::runtime_error("Cannot format accounting timestamp");
    return std::format("{}-{}-{} {}:{}:{}", local.tm_year + 1900, local.tm_mon + 1,
        local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
}
}

CCashAccounting::~CCashAccounting() { UnInit(); }
CCashAccounting& CCashAccounting::GetInstance()
{
    static CCashAccounting instance;
    return instance;
}

BOOL CCashAccounting::Init()
{
    CashDatabaseSettings settings;
// Existing connection defaults; explicit settings may be passed to Init.
    settings.server = "172.30.66.24";
    settings.user = "Jiutian";
    settings.password = "JiutianSky";
    settings.database = "sh_zhangm03_d";
    settings.log_database = "jtdb_logdata";
    settings.charset = "utf8";
    return Init(settings);
}

BOOL CCashAccounting::Init(const CashDatabaseSettings& settings, const std::filesystem::path& output)
{
    if (m_initialized)
        return FALSE;
    UnInit();
    m_summary = {};
    try
    {
        if (!m_sql.Init(settings.server.c_str(), settings.user.c_str(), settings.password.c_str(),
            settings.database.c_str(), settings.charset.c_str(), settings.port, g_CASqlStmtList, caSqlTotal, 1))
            return FALSE;
        if (!m_sql.SyncExecSqlStmtOwned(caSqlSelectLogoutRole,
            [this](std::list<void*>& rows, void*) -> BOOL { return Load(rows); }, ""))
        {
            UnInit();
            return FALSE;
        }
        m_next.assign(m_snapshots.size(), no_next);
        std::unordered_map<std::uint32_t, std::size_t> following;
        for (std::size_t i = m_snapshots.size(); i-- > 0;)
        {
            const auto [entry, inserted] = following.try_emplace(m_snapshots[i].role, i);
            if (!inserted)
            {
                m_next[i] = entry->second;
                entry->second = i;
            }
        }
        m_log.clear();
        m_log.open(output, std::ios::app);
        if (!m_log)
        {
            UnInit();
            return FALSE;
        }
        m_initialized = true;
        return TRUE;
    }
    catch (const std::exception& error)
    {
        UnInit();
        ERR("Cash accounting initialization failed: %s", error.what());
        return FALSE;
    }
}

BOOL CCashAccounting::UnInit()
{
    const bool sql_closed = m_sql.UnInit();
    bool file_closed = true;
    if (m_log.is_open())
    {
        m_log.close();
        file_closed = !m_log.fail();
    }
    m_log.clear();
    m_snapshots.clear();
    m_next.clear();
    m_cursor = 0;
    m_initialized = false;
    return sql_closed && file_closed;
}

BOOL CCashAccounting::Load(std::list<void*>& rows)
{
    for (const auto* bytes : rows)
    {
        if (!bytes)
            return FALSE;
        SnapshotRow row;
        std::memcpy(&row, bytes, sizeof(row));
        Snapshot snapshot;
        snapshot.role = identifier(row.role);
        snapshot.time = identifier(row.time);
        snapshot.money = {row.money0, row.money1};
        // The legacy destination stored at most 20 bytes plus the terminator.
        const auto end = std::find(std::begin(row.name), std::end(row.name), '\0');
        snapshot.name.assign(row.name, std::min<std::size_t>(end - row.name, 20));
        m_snapshots.push_back(std::move(snapshot));
    }
    return TRUE;
}

BOOL CCashAccounting::MainLoop()
{
    if (!m_initialized)
        return FALSE;
    try
    {
        while (m_cursor < m_snapshots.size())
        {
            const auto following = m_next[m_cursor];
            if (following != no_next)
            {
                const auto& begin = m_snapshots[m_cursor];
                const auto& end = m_snapshots[following];
                auto callback = [this, begin, end](std::list<void*>& rows, void*) -> BOOL
                { return Check(rows, begin, end); };
                // Widen to signed SQL BIGINT so the shared native binding preserves
                // the entire unsigned 32-bit role/time range without sign conversion.
                if (!m_sql.SyncExecSqlStmtOwned(caSqlSelectRoleMoney, std::move(callback), "llll",
                    static_cast<INT64>(begin.role), static_cast<INT64>(begin.role),
                    static_cast<INT64>(begin.time), static_cast<INT64>(end.time)))
                    return FALSE;
            }
            ++m_cursor;
        }
        m_snapshots.clear();
        m_next.clear();
        m_cursor = 0;
        return TRUE;
    }
    catch (const std::exception& error)
    {
        ERR("Cash accounting failed: %s", error.what());
        return FALSE;
    }
}

BOOL CCashAccounting::Check(std::list<void*>& rows, const Snapshot& begin, const Snapshot& end)
{
    std::array<std::int64_t, 2> predicted{begin.money[0], begin.money[1]};
    for (const auto* bytes : rows)
    {
        if (!bytes)
            return FALSE;
        TransactionRow row;
        std::memcpy(&row, bytes, sizeof(row));
        const auto source = identifier(row.source);
        const auto target = identifier(row.target);
        if (row.type < 0 || row.type > 1)
            continue;
        if (source == begin.role)
            predicted[row.type] = add(predicted[row.type], -static_cast<std::int64_t>(row.amount));
        else if (target == begin.role)
            predicted[row.type] = add(predicted[row.type], row.amount);
    }
    std::string name;
    if (begin.name.size() < 10)
        name.append(10 - begin.name.size(), ' ');
    name += begin.name;
    const auto first_time = time_text(begin.time);
    const auto last_time = time_text(end.time);
    std::string report;
    std::size_t successes = 0;
    for (int type = 0; type < 2; ++type)
    {
        const bool success = predicted[type] == end.money[type];
        successes += success;
        // Keep the original byte padding and text labels, including its report spelling.
        report += std::format("RoleID: {}\tRoleName:{}\tBeginTime:{}\tEndTime:{}\tMoneyType:{}\t\tBeginMoney:{:10}\tEndMoney:{:10}\tMoneyChangeCount:{:10}\tAccountint Result: {}!\n",
            begin.role, name, first_time, last_time, type, begin.money[type], end.money[type],
            static_cast<std::int64_t>(end.money[type]) - begin.money[type], success ? "Success" : "Fail");
    }
    m_log.write(report.data(), static_cast<std::streamsize>(report.size()));
    m_log.flush();
    if (!m_log)
        return FALSE;
    ++m_summary.intervals;
    m_summary.successes += successes;
    m_summary.failures += 2 - successes;
    return TRUE;
}
