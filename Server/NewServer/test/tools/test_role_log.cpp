#include "stdafx.h"
#include "rolelogcheck.h"

#include <atomic>
#include <crtdbg.h>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <new>
#include <string>
#include <thread>
#include <vector>

thread_local int role_allocation_countdown = -1;
void* operator new(std::size_t size)
{
    // Checked-STL proxy construction can be noexcept despite allocating. Target
    // application buffers/nodes in Debug; Release/ASAN still inject every size.
#if _ITERATOR_DEBUG_LEVEL != 0
    const bool inject = size >= 64;
#else
    const bool inject = true;
#endif
    if (inject && role_allocation_countdown == 0)
    {
        role_allocation_countdown = -1;
        throw std::bad_alloc();
    }
    if (inject && role_allocation_countdown > 0)
        --role_allocation_countdown;
    if (auto* memory = std::malloc(size == 0 ? 1 : size))
        return memory;
    throw std::bad_alloc();
}
void operator delete(void* memory) noexcept { std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }

namespace
{
int checks = 0;
void require(bool condition, const char* message)
{
    ++checks;
    if (!condition)
        throw std::runtime_error(message);
}

void write(const std::filesystem::path& path, std::string_view text, bool utf16 = false)
{
    std::ofstream file(path, std::ios::binary);
    if (utf16)
    {
        file.put(char(0xff)); file.put(char(0xfe));
        for (const char byte : text) { file.put(byte); file.put(0); }
    }
    else
        file.write(text.data(), static_cast<std::streamsize>(text.size()));
    file.close();
    require(bool(file), "write role log fixture");
}

const std::string money_header = "Time\tA\tB\tSource\tTarget\tAction\tType\tAmount\n";
const std::string basic_header = "Time\tA\tB\tRole\tD\tE\tF\tG\tH\tI\tJ\tK\tL\tMoney\n";
std::string money(int seconds, int role, int amount, std::string_view action = "Add", int type = 1)
{
    return std::format("2020-01-01 00:00:{:02}\t0\t0\t{}\t{}\t{}\t{}\t{}\n", seconds, role, role, action, type, amount);
}
std::string logout(int seconds, int role, int balance)
{
    return std::format("2020-01-01 00:00:{:02}\t0\t0\t{}\t0\t0\t0\t0\t0\t0\t0\t0\t0\t{}\n", seconds, role, balance);
}
void files(std::string_view transactions, std::string_view gm, std::string_view logouts, bool utf16 = false)
{
    write("Money_2020_01_01.txt", money_header + std::string(transactions), utf16);
    write("gmmoney_2020_01_01.txt", money_header + std::string(gm), utf16);
    write("RoleLogout_Basic_2020_01_01.txt", basic_header + std::string(logouts), utf16);
}
bool check(CRoleLogCheck& checker)
{
    return checker.DoCheckLog(".", "2020_01_01", nullptr, nullptr);
}

void time_cases()
{
    require(Str2Time("2020-01-01 00:00:00") == 1577836800, "frozen UTC timestamp");
    require(CustomizedStr2Time("2020-02-29 12:34:56") == 1582979696, "leap-day timestamp");
    require(!Str2Time(nullptr) && !CustomizedStr2Time(nullptr), "null timestamp");
    const std::string valid = "2020-01-01 00:00:00";
    for (std::size_t length = 0; length < valid.size(); ++length)
        require(CustomizedStr2Time(valid.substr(0, length).c_str()) == 0, "short timestamp must not read fixed offsets");
    for (const char* invalid : {"2020-02-30 00:00:00", "2019-02-29 00:00:00", "2020-13-01 00:00:00",
        "2020-01-01 24:00:00", "2020-01-01 00:60:00", "2020-01-01 00:00:60", "2020-01-01 00:00:00extra",
        "1969-01-01 00:00:00", "2107-01-01 00:00:00", "1970-01-01 00:00:00"})
        require(!role_log::ParseTime(invalid) && !CustomizedStr2Time(invalid), "invalid time and calendar");
    require(role_log::ParseTime("2106-02-07 06:28:15").value() == 0xffffffff,
        "last representable 32-bit timestamp");
    require(!role_log::ParseTime("2106-02-07 06:28:16"), "timestamp overflow");
    require(CustomizedStr2Time(valid.c_str()) == 1577836800, "invalid input does not poison cache");
    std::atomic<bool> correct{true};
    std::vector<std::jthread> threads;
    for (int i = 0; i < 8; ++i)
    {
        threads.emplace_back([&, i]
        {
            const auto text = std::format("2020-01-01 00:00:{:02}", i);
            for (int repeat = 0; repeat < 1000; ++repeat)
                if (CustomizedStr2Time(text.c_str()) != 1577836800 + i)
                    correct = false;
        });
    }
    threads.clear();
    require(correct, "timestamp caches are independent across threads");
}

bool parameters(CRoleLogCheck& checker, std::vector<std::string> args)
{
    std::vector<char*> raw;
    for (auto& argument : args)
        raw.push_back(argument.data());
    return checker.CheckParam(static_cast<int>(raw.size()), raw.data());
}

void parameter_cases(CRoleLogCheck& checker)
{
    require(parameters(checker, {"tool", ".", "2020_01_01"}), "single-day arguments");
    require(parameters(checker, {"tool", ".", "2020_01_01", ".", "2020_01_09"}), "later dates need not differ by exactly one character");
    require(!parameters(checker, {"tool", ".", "2020_02_30"}), "invalid file date");
    require(!parameters(checker, {"tool", ".", "2020_01_09", ".", "2020_01_01"}), "reversed file dates");
    require(!parameters(checker, {"tool", ".", "2020_01_01", ".", "2020_01_01"}), "duplicate file dates");
    require(!parameters(checker, {"tool", "."}), "missing arguments");
    require(!checker.DoCheckLog(".", "2020_01_01", ".", nullptr), "partial second input");
}

std::vector<std::string> messages;
void capture(INT32, const char* text, INT32 length) { messages.emplace_back(text, length); }
void detail_output(INT32, const char* message, INT32 length)
{
    std::string_view text(message, length);
    const auto start = text.find("*** log data error,");
    if (start == text.npos)
        return;
    text.remove_prefix(start);
    while (text.ends_with('\r') || text.ends_with('\n'))
        text.remove_suffix(1);
    std::printf("ROLE_DETAIL %.*s\n", static_cast<int>(text.size()), text.data());
}

void data_cases(CRoleLogCheck& checker)
{
    const auto snapshots = logout(0, 1, 100) + logout(20, 1, 120);
    for (int source = 0; source < 3; ++source)
    {
        files(source == 1 ? "" : money(10, 1, 20), source == 0 ? "" : money(15, 1, source == 1 ? 20 : 0), snapshots);
        require(check(checker) && checker.GetSummary().errors == 0 && checker.GetSummary().total == 2,
            "roles with money-only, GM-only or both sources");
    }
    files("", "", snapshots);
    require(check(checker) && checker.GetSummary().errors == 0 && checker.GetSummary().total == 2, "no transaction sources");
    files(money(10, 1, 20), "", snapshots, true);
    require(check(checker) && checker.GetSummary().errors == 0, "UTF-16LE table inputs");
    files(money(10, 1, 20, "AddWrong"), "", snapshots);
    require(!check(checker) && checker.GetSummary().total == 0, "malformed records cannot produce a partial success report");
    files(money(10, 1, 20), "", snapshots);
    auto malformed = money_header + money(10, 1, 20);
    malformed.replace(malformed.find("2020-01-01"), 19, "short");
    write("Money_2020_01_01.txt", malformed);
    require(!check(checker), "short row timestamp");
    write("Money_2020_01_01.txt", "Time\tA\n2020-01-01 00:00:00\t0\n");
    require(!check(checker), "too few columns");
    files(money(10, 1, 20), "", snapshots);
    require(check(checker) && checker.GetSummary().errors == 0, "reuse after malformed input");

    auto& logger = CLog::GetInstance();
    require(logger.Init("role-unit", lltAll, lotCustom, FALSE, FALSE, TRUE), "initialize diagnostic capture");
    logger.SetCustomOutput(capture);
    files(money(10, 1, 2147483647) + money(15, 1, 2147483647), "", logout(0, 1, 0) + logout(20, 1, -2));
    require(check(checker) && checker.GetSummary().errors == 1, "32-bit money wrap cannot hide a mismatch");
    bool wide = false;
    for (const auto& text : messages)
        wide |= text.find("check_money=4294967294") != text.npos;
    require(wide, "money difference retains its 64-bit value");
    const std::string long_message = "%s %n " + std::string(10000, 'x');
    COutput::GetInstance().Printf("%s", long_message.c_str());
    require(messages.back().find("%s %n ") != messages.back().npos, "diagnostic data is not a printf format");
    logger.SetCustomOutput(nullptr);
    require(logger.UnInit(), "close diagnostic capture");

    files(money(10, 1, 20), "", snapshots);
    std::filesystem::create_directory("100%s records");
    for (const char* name : {"Money_2020_01_01.txt", "gmmoney_2020_01_01.txt", "RoleLogout_Basic_2020_01_01.txt"})
        std::filesystem::copy_file(name, std::filesystem::path("100%s records") / name, std::filesystem::copy_options::overwrite_existing);
    require(checker.DoCheckLog("100%s records", "2020_01_01", nullptr, nullptr), "percent signs in input paths");
    require(checker.UnInit() && checker.UnInit(), "idempotent cleanup");
    require(!check(checker) && checker.Init(), "reinitialize and reject uninitialized reads");
    require(check(checker) && checker.GetSummary().errors == 0, "state cleared across reinitialization");
}

void allocation_failures(CRoleLogCheck& checker)
{
    files(money(10, 1, 20), money(15, 1, 0), logout(0, 1, 100) + logout(20, 1, 120));
    DWORD before = 0, after = 0;
    require(GetProcessHandleCount(GetCurrentProcess(), &before), "initial native handle count");
    int failures = 0, successes = 0;
    for (int point = 0; point < 150; ++point)
    {
        if (std::getenv("CQYH_TRACE_ROLE"))
            std::fprintf(stderr, "allocation point %d\n", point);
        role_allocation_countdown = point;
        const bool success = check(checker);
        role_allocation_countdown = -1;
        if (success)
            ++successes;
        else
            ++failures;
        require(check(checker) && checker.GetSummary().errors == 0, "recover after allocation failure");
    }
    require(failures > 10 && successes > 0, "allocation failure coverage includes loading and ownership publication");
    require(GetProcessHandleCount(GetCurrentProcess(), &after) && before == after, "failed loads release file handles");
    std::cout << failures << " allocation failure points, " << successes << " successful attempts\n";
}
}

int main(int argc, char* argv[])
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _putenv_s("TZ", "GMT0");
    _tzset();
    try
    {
        CRoleLogCheck checker;
        require(checker.Init() && !checker.Init(), "initialize once");
        if (argc == 3 || argc == 5)
        {
            require(CLog::GetInstance().Init("role-case", lltAll, lotCustom, FALSE, FALSE, TRUE), "initialize mismatch capture");
            CLog::GetInstance().SetCustomOutput(detail_output);
            const bool success = checker.DoCheckLog(argv[1], argv[2], argc == 5 ? argv[3] : nullptr, argc == 5 ? argv[4] : nullptr);
            const auto report = checker.GetSummary();
            CLog::GetInstance().SetCustomOutput(nullptr);
            CLog::GetInstance().UnInit();
            std::cout << "ROLE_SUMMARY " << report.errors << ' ' << report.discarded << ' ' << report.total << '\n';
            return success ? 0 : 1;
        }
        require(argc == 2, "expected work directory or case arguments");
        std::filesystem::create_directories(argv[1]);
        std::filesystem::current_path(argv[1]);
        time_cases();
        parameter_cases(checker);
        data_cases(checker);
        allocation_failures(checker);
        std::cout << checks << " role log checks passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        role_allocation_countdown = -1;
        std::cerr << error.what() << '\n';
        return 1;
    }
}
