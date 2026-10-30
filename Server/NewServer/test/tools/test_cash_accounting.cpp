#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "std_type.h"
#include "cash_accounting.h"
#include "fake_mysql_client.h"

namespace
{
using fake_mysql::Cell;
using fake_mysql::Row;
using fake_mysql::Rows;
namespace fs = std::filesystem;
int checks = 0;
int failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
template<class T> Cell number(T value)
{
    Cell bytes(sizeof(value));
    std::memcpy(bytes.data(), &value, sizeof(value));
    return bytes;
}
Row snapshot(std::int64_t role, std::int64_t time, std::int32_t first,
             std::int32_t second, std::string_view name = "Alice")
{
    return {number(time), number(role), number(first), number(second), Cell(name.begin(), name.end())};
}
Row transaction(std::int64_t source, std::int64_t target, std::int8_t type, std::int32_t amount)
{
    return {number(source), number(target), number(type), number(amount)};
}
struct Query
{
    std::array<std::int64_t, 4> parameters;
    Rows rows;
};
struct Scenario
{
    Rows snapshots;
    std::vector<Query> queries;
    std::size_t snapshotCalls = 0;
    std::size_t nextQuery = 0;
} scenario;
bool schema(MYSQL_BIND* bindings, std::span<const enum_field_types> types,
            std::span<const unsigned long> sizes)
{
    if (!bindings)
        return false;
    for (std::size_t index = 0; index < types.size(); ++index)
        if (bindings[index].buffer_type != types[index] || bindings[index].buffer_length != sizes[index]
            || !bindings[index].buffer || bindings[index].is_unsigned)
            return false;
    return true;
}
Rows query(std::string_view sql, std::span<const MYSQL_BIND> parameters, MYSQL_BIND* results)
{
    if (sql == "SELECT UNIX_TIMESTAMP(LogTime), RoleID, Money1, Money2, Name FROM logdata_rolelogout_basic")
    {
        constexpr std::array types{MYSQL_TYPE_LONGLONG, MYSQL_TYPE_LONGLONG, MYSQL_TYPE_LONG, MYSQL_TYPE_LONG, MYSQL_TYPE_STRING};
        constexpr std::array<unsigned long, 5> sizes{8, 8, 4, 4, 22};
        check(parameters.empty() && schema(results, types, sizes), "snapshot query uses its exact five-column native schema");
        ++scenario.snapshotCalls;
        return scenario.snapshots;
    }
    check(sql == "SELECT SourceID, TargetID, MoneyType, MoneyCount FROM logdata_money WHERE (SourceID = ? or TargetID = ?) AND (LogTime >= FROM_UNIXTIME(?) AND LogTime <= FROM_UNIXTIME(?))",
          "money query retains the original role filter and inclusive time bounds");
    constexpr std::array types{MYSQL_TYPE_LONGLONG, MYSQL_TYPE_LONGLONG, MYSQL_TYPE_TINY, MYSQL_TYPE_LONG};
    constexpr std::array<unsigned long, 4> sizes{8, 8, 1, 4};
    check(schema(results, types, sizes), "transaction query uses its exact four-column native schema");
    if (parameters.size() != 4 || scenario.nextQuery >= scenario.queries.size())
    {
        check(false, "each interval executes exactly one expected query");
        return {};
    }
    const auto& expected = scenario.queries[scenario.nextQuery++];
    for (std::size_t index = 0; index < parameters.size(); ++index)
    {
        const auto& binding = parameters[index];
        std::int64_t actual = 0;
        const bool valid = binding.buffer_type == MYSQL_TYPE_LONGLONG && binding.buffer_length == 8
            && !binding.is_unsigned && binding.buffer;
        if (valid)
            std::memcpy(&actual, binding.buffer, sizeof(actual));
        check(valid && actual == expected.parameters[index], "signed BIGINT binding preserves the complete role/time value and query order");
    }
    return expected.rows;
}
Query interval(std::int64_t role, std::int64_t begin, std::int64_t end, Rows rows = {})
{
    return {{role, role, begin, end}, std::move(rows)};
}
const CashDatabaseSettings settings{"fake", "", "", "fixture", "utf8mb4", "", 0};
void closed()
{
    check(fake_mysql::connections == 0 && fake_mysql::statements == 0 && fake_mysql::threads == 0
        && fake_mysql::results == 0 && !fake_mysql::ready, "shutdown releases client handles, workers, result sets and library lease");
    check(fake_mysql::orderErrors == 0 && fake_mysql::scriptErrors == 0, "native resources close in dependency order without script errors");
}
std::string read(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), {}};
}
std::vector<std::string> lines(const std::string& contents)
{
    std::vector<std::string> result;
    std::size_t position = 0;
    while (position < contents.size())
    {
        const auto end = contents.find('\n', position);
        if (end == std::string::npos)
        {
            result.push_back(contents.substr(position));
            break;
        }
        result.push_back(contents.substr(position, end + 1 - position));
        position = end + 1;
    }
    return result;
}
CashAccountingSummary run(const fs::path& output)
{
    CCashAccounting accounting;
    check(!accounting.MainLoop(), "uninitialized work is rejected");
    if (!accounting.Init(settings, output))
    {
        check(false, "initialize accounting against scripted client");
        return {};
    }
    check(!accounting.Init(settings, output), "repeated initialization preserves active data and output");
    check(accounting.MainLoop(), "accounting consumes complete snapshot/transaction results");
    check(scenario.snapshotCalls == 1 && scenario.nextQuery == scenario.queries.size(), "all and only adjacent per-role intervals are queried");
    const auto summary = accounting.GetSummary();
    const auto bytes = read(output);
    check(accounting.MainLoop() && read(output) == bytes, "a second loop does not repeat completed reports");
    check(accounting.UnInit() && accounting.UnInit(), "explicit shutdown is repeatable");
    check(accounting.GetSummary().intervals == summary.intervals, "summary remains available after shutdown");
    closed();
    return summary;
}
void report_compatibility(const fs::path& directory, const fs::path& fixture)
{
    const auto expected = lines(read(fixture));
    check(expected.size() == 4, "read four frozen original report lines");
    scenario = {{snapshot(7, 1577836800, 100, 10), snapshot(7, 1577836860, 120, 8)},
        {interval(7, 1577836800, 1577836860, {transaction(99, 7, 0, 20)})}};
    const auto path = directory / "original.txt";
    { std::ofstream output(path, std::ios::binary); output << "previous report\r\n"; }
    const auto summary = run(path);
    const auto actual = lines(read(path));
    check(summary.intervals == 1 && summary.successes == 1 && summary.failures == 1, "both currency outcomes are counted");
    check(actual.size() == 3 && actual[0] == "previous report\r\n", "existing reports are retained in append mode");
    if (expected.size() == 4 && actual.size() == 3)
        check(actual[1] == expected[0] && actual[2] == expected[1], "success/failure reports match the original bytes including CRLF and padding");
    scenario = {{snapshot(42, 1577882096, -20, 0, "100%s"), snapshot(42, 1577968496, 30, 0)},
        {interval(42, 1577882096, 1577968496, {transaction(1, 42, 0, 50)})}};
    const auto percent = directory / "percent.txt";
    run(percent);
    const auto percent_lines = lines(read(percent));
    if (expected.size() == 4 && percent_lines.size() == 2)
        check(percent_lines[0] == expected[2], "percent-bearing names are data and preserve original formatting");
    else
        check(false, "percent report has two complete currency lines");
    const std::string chinese = "\xe5\x90\x8d\xe5\xad\x97";
    scenario = {{snapshot(42, 1577882096, 0, 0, chinese), snapshot(42, 1577968496, 0, 0)},
        {interval(42, 1577882096, 1577968496)}};
    const auto unicode = directory / L"\u540d\u5b57.txt";
    run(unicode);
    const auto unicode_lines = lines(read(unicode));
    if (expected.size() == 4 && unicode_lines.size() == 2)
        check(unicode_lines[1] == expected[3], "UTF-8 database names retain byte-based legacy padding through a native Unicode file path");
    else
        check(false, "Unicode report has two complete currency lines");
}
void reconciliation(const fs::path& directory)
{
    // Interleave roles and use repeated timestamps; global input order and the
    // first following snapshot of the same role determine the original intervals.
    scenario = {{snapshot(7, 100, 10, 20, "first"), snapshot(8, 100, 50, 60, "second"),
        snapshot(7, 100, 7, 25, "renamed"), snapshot(99, 110, 1, 2, "lonely"),
        snapshot(8, 120, 40, 61), snapshot(7, 130, 9, 20)},
        {interval(7, 100, 100, {transaction(7, 7, 0, 3), transaction(9, 7, 1, 5),
            transaction(7, 9, -1, 999), transaction(9, 7, 2, 999), transaction(9, 10, 0, 999)}),
         interval(8, 100, 120, {transaction(8, 1, 0, 10), transaction(1, 8, 1, 1)}),
         interval(7, 100, 130, {transaction(7, 1, 0, -2), transaction(7, 1, 1, 5)})}};
    const auto path = directory / "ordered.txt";
    const auto summary = run(path);
    const auto report = lines(read(path));
    check(summary.intervals == 3 && summary.successes == 6 && summary.failures == 0,
          "self-transfers keep source precedence, unknown currencies/unrelated roles are ignored, and signed amounts retain meaning");
    check(report.size() == 6 && report[0].starts_with("RoleID: 7\tRoleName:     first\t")
        && report[2].starts_with("RoleID: 8\tRoleName:    second\t")
        && report[4].starts_with("RoleID: 7\tRoleName:   renamed\t"), "reports follow global snapshot order and each interval keeps its starting name");
    constexpr auto high = std::int64_t{4294967295};
    constexpr auto large = std::numeric_limits<std::int32_t>::max();
    scenario = {{snapshot(high, 2147483648, large, 0, std::string(22, 'X')),
                 snapshot(high, high, large - 2, 0)},
        {interval(high, 2147483648, high, {transaction(1, high, 0, large),
            transaction(high, 1, 0, large), transaction(high, 1, 0, 2)})}};
    const auto wide = directory / "wide.txt";
    const auto wide_summary = run(wide);
    const auto text = read(wide);
    check(wide_summary.successes == 2 && wide_summary.failures == 0, "64-bit accumulation survives a temporary signed-32-bit overflow");
    check(text.starts_with("RoleID: 4294967295\tRoleName:" + std::string(20, 'X') + "\t")
        && text.find("2038-1-19 3:14:8") != text.npos && text.find("2106-2-7 6:28:15") != text.npos,
          "full unsigned-32-bit role/time values and bounded nonterminated names are preserved");
    scenario = {{snapshot(7, 1, large, 0), snapshot(7, 2, -2, 0)},
        {interval(7, 1, 2, {transaction(1, 7, 0, large)})}};
    const auto overflow = directory / "overflow.txt";
    const auto overflow_summary = run(overflow);
    check(overflow_summary.successes == 1 && overflow_summary.failures == 1
        && read(overflow).find("MoneyChangeCount:-2147483649\tAccountint Result: Fail!") != std::string::npos,
          "32-bit wrap cannot produce a false success or truncate the displayed balance difference");
    scenario = {};
    const auto empty = directory / "empty.txt";
    check(run(empty).intervals == 0 && read(empty).empty(), "empty snapshots produce an empty report without transaction queries");
    scenario = {{snapshot(7, 1, 0, 0)}, {}};
    const auto single = directory / "single.txt";
    check(run(single).intervals == 0 && read(single).empty(), "unpaired snapshots do not produce reports");
}
void failure_paths(const fs::path& directory)
{
    CCashAccounting accounting;
    for (const auto invalid : {std::int64_t{-1}, std::int64_t{4294967296}})
    {
        scenario = {{snapshot(invalid, 0, 0, 0)}, {}};
        check(!accounting.Init(settings, directory / "invalid.txt"), "out-of-range database identifiers reject initialization");
        closed();
    }
    scenario = {{snapshot(7, -1, 0, 0)}, {}};
    check(!accounting.Init(settings, directory / "invalid-time.txt"), "invalid database timestamps reject initialization");
    closed();
    scenario = {};
    check(!accounting.Init(settings, directory), "opening a directory as an output file fails cleanly");
    closed();
    fake_mysql::failLibrary = true;
    check(!accounting.Init(settings, directory / "library.txt"), "client library initialization failure reaches the caller");
    closed();
    scenario = {{snapshot(7, 1, 0, 0), snapshot(7, 2, 0, 0)}, {interval(7, 1, 2)}};
    check(accounting.Init(settings, directory / "reused.txt") && accounting.MainLoop(), "the same object recovers after failed initialization");
    check(accounting.UnInit(), "recovered accounting shuts down cleanly");
    closed();
    scenario = {{snapshot(7, 1, 0, 0), snapshot(7, 2, 0, 0)},
        {interval(7, 1, 2, {transaction(-1, 7, 0, 1)})}};
    check(accounting.Init(settings, directory / "bad-transaction.txt") && !accounting.MainLoop(), "invalid transaction identifiers fail the interval without publishing a partial report");
    check(read(directory / "bad-transaction.txt").empty() && accounting.GetSummary().intervals == 0, "failed intervals do not count or emit currency results");
    check(accounting.UnInit(), "failed callback releases all borrowed SQL rows");
    closed();
    scenario = {{snapshot(7, 1, 0, 0), snapshot(7, 2, 0, 0)}, {interval(7, 1, 2)}};
    // Make an existing output unavailable after the snapshot query has succeeded.
    const auto locked = directory / "locked.txt";
    { std::ofstream output(locked); }
    const auto handle = CreateFileW(locked.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    check(handle != INVALID_HANDLE_VALUE, "reserve an output sharing failure fixture");
    if (handle != INVALID_HANDLE_VALUE)
    {
        check(!accounting.Init(settings, locked), "output sharing violations release the already-loaded database state");
        CloseHandle(handle);
    }
    closed();
    scenario = {};
    check(accounting.Init(settings, directory / "destructor.txt"), "initialize one final empty run for destructor cleanup");
    // Destructor is deliberately responsible for this final shutdown.
}
void retry_paths(const fs::path& directory)
{
    CCashAccounting accounting;
    scenario = {{snapshot(7, 1, 0, 0), snapshot(7, 2, 5, 0)},
        {interval(7, 1, 2, {transaction(1, 7, 0, 5)})}};
    fake_mysql::failFetchAfterRow = true;
    const auto snapshots = directory / "snapshot-retry.txt";
    check(accounting.Init(settings, snapshots) && scenario.snapshotCalls == 2,
          "a failed snapshot fetch retries without passing partial rows to the loader");
    check(accounting.MainLoop() && accounting.GetSummary().intervals == 1
        && accounting.GetSummary().successes == 2 && scenario.nextQuery == 1,
          "retried snapshot results are loaded once and produce one interval");
    check(accounting.UnInit() && lines(read(snapshots)).size() == 2, "snapshot retries do not duplicate report lines");
    closed();
    const auto money = interval(7, 1, 2, {transaction(1, 7, 0, 5)});
    scenario = {{snapshot(7, 1, 0, 0), snapshot(7, 2, 5, 0)}, {money, money}};
    const auto transactions = directory / "transaction-retry.txt";
    check(accounting.Init(settings, transactions), "initialize transaction fetch retry");
    fake_mysql::failFetchAfterRow = true;
    check(accounting.MainLoop() && scenario.nextQuery == 2 && accounting.GetSummary().successes == 2,
          "transaction fetch retry discards the partial result before reconciliation");
    check(accounting.UnInit() && lines(read(transactions)).size() == 2, "only a complete transaction result publishes currency reports");
    closed();
    scenario = {{snapshot(7, 1, 0, 0), snapshot(7, 2, 5, 0)}, {money}};
    const auto exhausted = directory / "execution-retry.txt";
    check(accounting.Init(settings, exhausted), "initialize execution retry exhaustion");
    fake_mysql::failExecutions = 2;
    check(!accounting.MainLoop() && accounting.GetSummary().intervals == 0 && read(exhausted).empty(),
          "exhausted SQL retries return failure without a partial audit");
    check(accounting.MainLoop() && accounting.GetSummary().successes == 2 && scenario.nextQuery == 1,
          "an explicitly retried loop resumes at the failed interval");
    check(accounting.UnInit() && lines(read(exhausted)).size() == 2, "resuming after a SQL failure publishes exactly one interval");
    closed();
}
}

int main(int argc, char** argv)
{
    if (argc != 3)
        return EXIT_FAILURE;
    try
    {
        _putenv_s("TZ", "GMT0");
        _tzset();
        const fs::path directory = fs::path(argv[1]) /
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        fs::create_directories(directory);
        fake_mysql::preparedHandler = query;
        report_compatibility(directory, fs::path(argv[2]));
        reconciliation(directory);
        failure_paths(directory);
        retry_paths(directory);
        closed();
        fake_mysql::preparedHandler = nullptr;
        if (failures)
            return EXIT_FAILURE;
        std::printf("%d cash accounting checks passed\n", checks);
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error)
    {
        std::fprintf(stderr, "FAIL: %s\n", error.what());
        return EXIT_FAILURE;
    }
}
