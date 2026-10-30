#include "fake_guard_process_api.h"
#include "process_mgr.h"
#include "gd_session_mgr.h"
#include "gd_message_handler.h"
#include "guard_config.h"
#include "process_option.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <exception>
#include <format>
#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <new>
#include <string>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <crtdbg.h>

namespace fault
{
std::atomic<int> after{-1};
thread_local bool skipNoexceptProxies = false;
}
void* operator new(std::size_t size)
{
    bool skip = false;
#if defined(_MSC_VER) && _ITERATOR_DEBUG_LEVEL != 0
    // MSVC Debug allocates iterator proxies even in noexcept string constructors.
    // Failing those invokes termination, not a recoverable allocation error.
    skip = fault::skipNoexceptProxies && size == sizeof(std::_Container_proxy);
#endif
    if (!skip && fault::after.load() >= 0 && fault::after.fetch_sub(1) == 0)
    {
        fault::after = -1;
        throw std::bad_alloc{};
    }
    if (void* value = std::malloc(size ? size : 1)) return value;
    throw std::bad_alloc{};
}
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void* value) noexcept { std::free(value); }
void operator delete[](void* value) noexcept { std::free(value); }
void operator delete(void* value, std::size_t) noexcept { std::free(value); }
void operator delete[](void* value, std::size_t) noexcept { std::free(value); }

namespace
{
int checks = 0;
int failures = 0;
void Check(bool value, const char* message)
{
    ++checks;
    if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", message); }
}
FILETIME Time(UINT64 ticks) { return {static_cast<DWORD>(ticks), static_cast<DWORD>(ticks >> 32)}; }
struct Process
{
    std::string name = "gameserver64.exe";
    UINT64 creation = 100;
    UINT64 kernel = 60;
    UINT64 user = 40;
    SIZE_T memory = 123456;
    bool denied = false;
    bool modules = true;
    bool image = true;
    bool times = true;
    bool memoryOk = true;
    bool terminate = true;
    DWORD wait = WAIT_TIMEOUT;
    DWORD exit = STILL_ACTIVE;
};
struct Handle { DWORD pid; bool thread = false; };
struct Launch { std::string application, command, directory; DWORD flags; BOOL inherit; DWORD startupSize; };
struct Notice { INT32 state; UINT32 pid; UINT8 type; UINT32 session; BOOL automatic; };
struct State
{
    std::map<DWORD, Process> processes;
    std::map<HANDLE, Handle> handles;
    std::vector<DWORD> enumeration;
    std::vector<DWORD> enumCapacity;
    std::vector<std::pair<DWORD, DWORD>> opens;
    std::vector<DWORD> terminations;
    std::vector<Launch> launches;
    std::vector<Notice> notices;
    std::vector<std::tuple<INT32, INT32, INT64>> writes;
    std::function<void()> notify;
    UINT64 tick = 0;
    UINT64 systemKernel = 600;
    UINT64 systemUser = 400;
    std::uintptr_t nextHandle = 1000;
    DWORD nextPid = 5000;
    DWORD failLaunch = 0;
    int enumFailure = 0;
    int badEnumBytes = 0;
    int closed = 0;
    int alerts = 0;
    int resets = 0;
    INT32 statsIndex = 0;
    bool systemOk = true;
    bool writesOk = true;
    bool noticesOk = true;
} state;
HANDLE MakeHandle(DWORD pid, bool thread = false)
{
    const auto value = reinterpret_cast<HANDLE>(++state.nextHandle);
    state.handles.emplace(value, Handle{pid, thread});
    return value;
}
Process* Lookup(HANDLE handle)
{
    const auto found = state.handles.find(handle);
    if (found == state.handles.end() || found->second.thread) return nullptr;
    const auto process = state.processes.find(found->second.pid);
    return process == state.processes.end() ? nullptr : &process->second;
}
void Reset()
{
    fault::after = -1;
    state = {};
    g_GuardConfig = {};
}
void Poll(CProcessMgr& manager)
{
    state.tick += 1001;
    Check(manager.MainLoop(), "process query failures do not stop guard main loop");
}
void Discovery()
{
    Reset();
    state.processes[1].name = "gameserver64.exe";
    state.processes[2].name = "gameserver64.exe.backup";
    state.processes[3].image = false;
    state.processes[4].modules = false;
    state.processes[5].denied = true;
    state.processes[6].name = "mobileserver64_d.exe";
    state.processes[7].name = "mgateserver64.exe";
    state.processes[8].name = "other.exe";
    state.enumeration = {0,1,2,3,4,5,6,7,8,1};
    CProcessMgr manager;
    Check(manager.Init(), "enumerate processes");
    Check(manager.CountRuningProcessByType(stGS) == 1, "exact executable name and duplicate PID filtering");
    Check(manager.CountRuningProcessByType(stMM) == 1 && manager.CountRuningProcessByType(stMG) == 1,
        "discovery includes mobile and manage-gate servers");
    Check(manager.CountRuningProcessByType(stAll) == 3, "only complete server names adopted");
    Check(state.handles.empty(), "all enumeration handles close including image query failure");
    Check(!manager.SetProcessInfo(999, stGS, 1), "missing process update fails");
    Check(manager.SetProcessInfo(1, stGS, 1), "known process session update");
    Check(!manager.SetProcessInfo(1, stGS, 0) && !manager.SetProcessInfo(1, stGS, MAX_GS_SESSION_COUNT),
        "game session bounds enforced");
    Check(!manager.SetProcessInfo(1, stMS, 1), "registration cannot change discovered process type");
    Check(manager.SetProcessInfo(6, stMM, 99) &&
        std::ranges::find(manager.GetProcesses(), DWORD{6}, &SProcessInfo::dwProcessID)->bySessionID == 1,
        "single server session normalization retained");
    state.enumFailure = 1;
    Check(!manager.Init() && manager.CountRuningProcessByType(stAll) == 3,
        "failed enumeration preserves previous registry transactionally");
    state.enumFailure = 0;
    state.badEnumBytes = 3;
    Check(!manager.Init(), "malformed enumeration byte count rejected");
    state.badEnumBytes = 5000;
    const auto beforeMalformed = state.enumCapacity.size();
    Check(!manager.Init() && state.enumCapacity.size() == beforeMalformed + 1,
        "impossible enumeration length fails without allocating a larger buffer");
    Check(manager.UnInit() && manager.UnInit(), "registry cleanup is repeatable");
    Check(manager.GetProcesses().empty() && state.handles.empty(), "cleanup does not own process lifetime");
}
void LargeDiscovery()
{
    Reset();
    for (DWORD pid = 1; pid <= 1100; ++pid)
    {
        state.processes[pid].name = "gameserver64_d.exe";
        state.enumeration.push_back(pid);
    }
    CProcessMgr manager;
    Check(manager.Init() && manager.CountRuningProcessByType(stAll) == 1100,
        "enumeration grows beyond original 1024 PID buffer");
    Check(state.enumCapacity.size() == 2 && state.enumCapacity[0] == 1024 && state.enumCapacity[1] == 2048,
        "full enumeration buffer is retried with larger storage");
    Check(state.handles.empty(), "large enumeration closes every handle");
}
void Closing()
{
    Reset();
    for (DWORD pid : {1,2,3}) state.processes[pid] = {};
    CProcessMgr manager;
    for (DWORD pid : {1,2,3}) Check(manager.AddProcessInfo(stGS, pid, 0, TRUE), "register local process");
    Check(manager.CloseProcess(stGS), "successful selected close returns success");
    Check(state.terminations.size() == 3 && state.handles.empty(), "selected close releases all handles");
    Check(manager.GetProcesses().size() == 3, "close keeps records until exit polling");
    Check(!manager.CloseProcess(stMS) && !manager.CloseProcess(stInvalid), "missing/invalid type close fails");
    state.terminations.clear();
    state.processes[2].denied = true;
    Check(!manager.CloseAllProcess(), "close-all reports partial failure");
    Check(state.terminations == std::vector<DWORD>{1,3}, "close-all continues after one open failure");
    Check(state.handles.empty(), "open failure leaves no leaked handles");
    state.processes[2].denied = false;
    state.processes[1].terminate = false;
    state.terminations.clear();
    Check(!manager.CloseAllProcess() && state.terminations.size() == 3,
        "termination failure does not skip remaining processes");
    Check(state.handles.empty(), "termination failure still closes native handle");
    state.processes[1].terminate = true;
    state.processes[1].creation = 200;
    state.terminations.clear();
    Check(!manager.CloseAllProcess(), "reused PID cannot terminate replacement process");
    Check(std::ranges::find(state.terminations, DWORD{1}) == state.terminations.end(), "identity mismatch never terminates");
    state.processes[2].times = false;
    state.terminations.clear();
    Check(!manager.CloseAllProcess() && state.terminations == std::vector<DWORD>{3},
        "creation-time query failure never terminates an unverified process");
    Check(manager.AddProcessInfo(stGS, 999, 0x1234, FALSE), "register remote process metadata");
    state.opens.clear();
    Check(!manager.CloseAllProcess(), "remote shutdown reports unsupported operation");
    Check(std::ranges::none_of(state.opens, [](auto open) { return open.first == 999; }),
        "remote PID is never opened as a local process");
    Check(state.handles.empty(), "all close error paths release native handles");
    manager.UnInit();
    Check(manager.CloseAllProcess(), "empty close-all succeeds");
}
void Registration()
{
    Reset();
    state.processes[1] = {};
    CProcessMgr manager;
    Check(!manager.AddProcessInfo(stInvalid, 1, 0, FALSE) && !manager.AddProcessInfo(stGS, 0, 0, FALSE),
        "invalid process registration rejected");
    Check(manager.AddProcessInfo(stGS, 1, 0, TRUE) && manager.AddProcessInfo(stGS, 1, 0, TRUE),
        "duplicate process registration is idempotent");
    Check(manager.GetProcesses().size() == 1 && !manager.AddProcessInfo(stMS, 1, 0, FALSE),
        "duplicate PID cannot acquire conflicting type");
    Check(manager.GetProcesses().front().bySessionID == 0 && manager.GetProcesses().front().bAuto,
        "registration initializes session and mode fields");
    state.processes[1].creation += 100;
    Check(manager.SetProcessInfo(1, stGS, 1), "fresh registration refreshes a reused PID identity");
    Poll(manager);
    Check(manager.IsProcessRuningByID(1) && manager.GetProcesses().size() == 1,
        "a freshly registered replacement is not retired as the old process");
    state.processes[1].creation += 100;
    state.processes[1].name = "mobileserver64.exe";
    Check(manager.SetProcessInfo(1, stMM, 1), "verified PID reuse may register a different service type");
    Poll(manager);
    Check(manager.CountRuningProcessByType(stMM) == 1 && manager.CountRuningProcessByType(stGS) == 0,
        "replacement registration carries its new service identity");
    state.notices.clear();
    Check(manager.SyncProcessInfo(InternalMessage::epsNormal, 1), "one process notification succeeds");
    Check(state.notices.size() == 1 && state.notices.front().pid == 1 && state.notices.front().automatic,
        "process notification fields retained");
    Check(!manager.SyncProcessInfo(InternalMessage::epsNormal, 999), "missing notification fails");
    state.noticesOk = false;
    Check(!manager.SyncAllProcessInfo(), "notification failure reaches caller");
    state.noticesOk = true;
    state.notify = [] { throw std::runtime_error("notification fixture"); };
    Check(!manager.SyncProcessInfo(InternalMessage::epsNormal, 1), "single notification exception becomes failure");
    state.notify = [] { throw 7; };
    Check(!manager.SyncAllProcessInfo(), "unknown batch notification exception becomes failure");
    state.notify = {};
    state.processes[2] = {};
    Check(manager.AddProcessInfo(stGS, 2, 0, FALSE), "second registry entry");
    state.notices.clear();
    state.notify = [&] { manager.UnInit(); };
    Check(manager.SyncAllProcessInfo(), "notification may reenter cleanup");
    Check(state.notices.size() == 1 && manager.GetProcesses().empty(),
        "reentrant cleanup never visits erased future records");
    state.notify = {};
    state.processes[3].times = false;
    Check(manager.AddProcessInfo(stGS, 3, 0, FALSE), "unverified process remains observable");
    Check(!manager.CloseProcess(stGS) && state.terminations.empty(), "unverified creation identity cannot be killed");
    state.processes[3].times = true;
    state.processes[3].name = "mobileserver64.exe";
    Check(manager.SetProcessInfo(3, stMM, 1), "fresh verified registration recovers an unknown identity");
    Check(manager.GetProcesses().front().byType == stMM &&
        manager.GetProcesses().front().ftProcessCreationTime.dwLowDateTime == 100,
        "recovered registration binds the current native process identity");
}
void Polling()
{
    Reset();
    state.processes[1] = {};
    CProcessMgr manager;
    Check(manager.AddProcessInfo(stGS, 1, 0, FALSE), "poll fixture registration");
    state.opens.clear();
    state.tick = 1000;
    Check(manager.MainLoop() && state.opens.empty(), "poll retains strict one-second boundary");
    Poll(manager);
    Check(state.writes.size() == 2 && std::get<2>(state.writes[0]) == 10 &&
        std::get<2>(state.writes[1]) == 123456, "first CPU lifetime ratio and pagefile bytes retained");
    state.writes.clear();
    state.systemKernel = 660; state.systemUser = 440;
    state.processes[1].kernel = 72; state.processes[1].user = 48;
    Poll(manager);
    Check(state.writes.size() == 2 && std::get<2>(state.writes[0]) == 20, "subsequent CPU sample uses deltas");
    state.processes[1].denied = true;
    state.writes.clear();
    Poll(manager);
    Check(manager.IsProcessRuningByID(1) && state.writes.empty() && state.notices.empty(),
        "access denial does not invent process exit or publish stale metrics");
    state.processes[1].denied = false;
    state.processes[1].wait = WAIT_FAILED;
    Poll(manager);
    Check(manager.IsProcessRuningByID(1), "failed wait retains live record");
    state.processes[1].wait = WAIT_TIMEOUT;
    state.processes[1].memoryOk = false;
    Poll(manager);
    Check(state.writes.empty(), "failed memory query publishes neither metric");
    state.processes[1].memoryOk = true;
    state.systemOk = false;
    Poll(manager);
    Check(state.writes.empty(), "failed system clock query publishes neither metric");
    state.systemOk = true;
    state.statsIndex = MAX_STATIS_PROCESS_COUNT;
    Poll(manager);
    Check(state.writes.empty(), "statistics slot bound enforced");
    state.statsIndex = 0;
    state.processes[1].wait = WAIT_OBJECT_0;
    state.processes[1].exit = STILL_ACTIVE;
    Poll(manager);
    Check(!manager.IsProcessRuningByID(1) && state.notices.size() == 1 &&
        state.notices.back().state == InternalMessage::epsDestroy,
        "signaled process with exit code 259 is still removed");
    Check(state.alerts == 1 && state.handles.empty(), "exit notification and all query handles close");
    state.processes[2] = {};
    Check(manager.AddProcessInfo(stGS, 2, 0, FALSE), "reuse polling fixture");
    state.processes[2].creation += 1;
    Poll(manager);
    Check(!manager.IsProcessRuningByID(2), "PID reuse retires old record");
    Check(manager.AddProcessInfo(stGS, 2, 0x1234, FALSE), "remote record for poll");
    state.opens.clear();
    Poll(manager);
    Check(state.opens.empty() && manager.GetProcesses().size() == 1 && !manager.IsProcessRuningByID(2),
        "remote records are not queried or identified as local PIDs");
    manager.UnInit();
    state.processes[3] = {};
    Check(manager.AddProcessInfo(stGS, 3, 0, FALSE), "disappearance fixture");
    state.processes.erase(3);
    state.notify = [&] { manager.UnInit(); };
    Poll(manager);
    Check(manager.GetProcesses().empty() && state.handles.empty(), "exit callback can clear live registry");
    state.notify = {};
    state.processes[4] = {};
    Check(manager.AddProcessInfo(stGS, 4, 0, FALSE), "allocation failure poll fixture");
    fault::after = 0;
    Poll(manager);
    Check(manager.IsProcessRuningByID(4) && state.handles.empty(), "poll allocation failure keeps registry valid");
    Poll(manager);
    Check(state.handles.empty(), "poll retries after allocation failure");
    state.tick = 0;
    state.opens.clear();
    Check(manager.MainLoop() && state.opens.empty(), "clock rollback does not underflow interval");
}
void Launching(const std::filesystem::path& root)
{
    Reset();
    const auto directory = root.string();
    Check(directory.size() < sizeof(g_GuardConfig.szServerRunPath), "fixture path fits configuration ABI");
    std::memcpy(g_GuardConfig.szServerRunPath, directory.c_str(), directory.size() + 1);
    CProcessMgr manager;
    std::string arguments = "config \"file with spaces.lua\" " + std::string(1024, 'x');
    Check(manager.StartProcess(stGS, arguments.data(), TRUE, TRUE, TRUE), "launch request succeeds through fake API");
    const auto& launch = state.launches.back();
    const auto expected = (std::filesystem::absolute(root) / "gameserver/gameserver64.exe").lexically_normal().string();
    Check(launch.application == expected && launch.command == std::format("\"{}\" {} view 950 5", expected, arguments),
        "explicit executable path, quoting, complete arguments and view position retained");
    Check(launch.directory == std::filesystem::absolute(root).lexically_normal().string() &&
        launch.flags == CREATE_NEW_CONSOLE && !launch.inherit && launch.startupSize == sizeof(STARTUPINFOA),
        "working directory and native launch options retained");
    Check(state.handles.empty() && state.closed == 2, "new process and thread handles release immediately");
    Check(manager.GetProcesses().size() == 1 && manager.GetProcesses().front().bAuto &&
        manager.GetProcesses().front().bySessionID == 0, "launched child is tracked before notification");
    const auto calls = state.launches.size();
    Check(!manager.StartProcess(stInvalid, nullptr) && !manager.StartProcess(stClient, nullptr), "invalid launch types rejected");
    Check(state.launches.size() == calls, "invalid launch never reaches native API");
    state.failLaunch = 2;
    Check(!manager.StartProcess(stGS, nullptr) && manager.GetProcesses().size() == 1,
        "failed CreateProcess leaves no tracking node");
    Check(state.handles.empty(), "failed launch has no native handle leak");
    state.failLaunch = 0;
    Check(manager.StartProcess(stMS, nullptr), "single-instance service starts once");
    Check(!manager.StartProcess(stMS, nullptr), "single-instance guard retained");
    Check(!manager.StartAllProcess(nullptr), "start-all retains existing-singleton guard");
    manager.UnInit();
    g_GuardConfig.nLocalGSCount = 2;
    g_GuardConfig.nLocalGTCount = 1;
    Check(manager.StartAllProcess(nullptr, FALSE, TRUE), "configured multiple service launch");
    Check(manager.CountRuningProcessByType(stGS) == 2 && manager.CountRuningProcessByType(stGT) == 1,
        "configured multi-instance counts respected");
    Check(manager.StartAllProcess(nullptr, FALSE, TRUE) && manager.GetProcesses().size() == 3,
        "existing multi-instance targets are not duplicated");
    state.noticesOk = false;
    Check(manager.StartProcess(stGS, nullptr, FALSE, TRUE) && manager.GetProcesses().size() == 4,
        "failed launch notification does not lose ownership of a created child");
    state.noticesOk = true;
    state.notify = [] { throw std::runtime_error("notification fixture"); };
    Check(manager.StartProcess(stGS, nullptr, FALSE, TRUE) && manager.GetProcesses().size() == 5,
        "throwing launch notification cannot turn a created process into a failed launch");
    Check(state.handles.empty(), "throwing launch notification releases native handles");
    bool trackedDuringNotice = false;
    state.notify = [&] {
        trackedDuringNotice = std::ranges::any_of(manager.GetProcesses(), [](const auto& process) {
            return process.dwProcessID == state.notices.back().pid;
        });
        manager.UnInit();
        throw 7;
    };
    Check(manager.StartProcess(stGS, nullptr, FALSE, TRUE) && trackedDuringNotice && manager.GetProcesses().empty(),
        "launch notification sees tracked child and may reenter cleanup");
    Check(state.handles.empty(), "reentrant launch cleanup releases both native handles");
    state.notify = {};
    manager.UnInit();
    state.processes[6001] = {};
    state.processes[6001].creation = 50;
    Check(manager.AddProcessInfo(stGS, 6001, 0, FALSE), "stale PID before a new launch");
    state.nextPid = 6000;
    Check(manager.StartProcess(stGS, nullptr, FALSE, TRUE) && manager.GetProcesses().size() == 1,
        "newly created child replaces an unpolled stale PID record");
    Check(manager.GetProcesses().front().ftProcessCreationTime.dwLowDateTime == 100,
        "replacement child retains its own creation identity");
    manager.UnInit();
    g_GuardConfig.nLocalGSCount = -1;
    const auto beforeInvalid = state.launches.size();
    Check(!manager.StartAllProcess(nullptr) && state.launches.size() == beforeInvalid,
        "invalid counts fail before any partial launch");
    g_GuardConfig.nLocalGSCount = MAX_GS_SESSION_COUNT;
    Check(!manager.StartAllProcess(nullptr), "configured count must fit session slots");
    g_GuardConfig.nLocalGSCount = 0;
    std::memset(g_GuardConfig.szServerRunPath, 0, sizeof(g_GuardConfig.szServerRunPath));
    const auto beforeMissing = state.launches.size();
    Check(!manager.StartProcess(stGS, nullptr) && state.launches.size() == beforeMissing,
        "missing executable never starts process");
    std::memcpy(g_GuardConfig.szServerRunPath, directory.c_str(), directory.size() + 1);
    bool reachedSuccess = false;
    fault::skipNoexceptProxies = true;
    for (int allocation = 0; allocation < 64; ++allocation)
    {
        manager.UnInit();
        const auto before = state.launches.size();
        fault::after = allocation;
        const auto result = manager.StartProcess(stGS, arguments.data(), FALSE, TRUE);
        fault::after = -1;
        Check(state.handles.empty(), "allocation failure releases any acquired handles");
        if (result)
        {
            Check(state.launches.size() == before + 1 && manager.GetProcesses().size() == 1,
                "successful child always has a tracking record");
            reachedSuccess = true;
            break;
        }
        Check(state.launches.size() == before && manager.GetProcesses().empty(),
            "allocation failure happens before native process creation");
    }
    fault::skipNoexceptProxies = false;
    Check(reachedSuccess, "fault injection reaches successful launch after recoverable allocation sites");
}
void MetricIsolation()
{
    Reset();
    state.processes[1].kernel = 10; state.processes[1].user = 0;
    state.processes[2].kernel = 20; state.processes[2].user = 0;
    CProcessMgr manager;
    Check(manager.AddProcessInfo(stGS, 1, 0, FALSE) && manager.AddProcessInfo(stGS, 2, 0, FALSE),
        "independent sampling fixtures register");
    Poll(manager);
    Check(state.writes.size() == 4 && std::get<2>(state.writes[0]) == 1 && std::get<2>(state.writes[2]) == 2,
        "two PIDs sharing a native statistics slot retain independent samples");
    state.writes.clear();
    state.systemKernel += 60; state.systemUser += 40;
    state.processes[1].kernel += 10; state.processes[2].kernel += 20;
    Poll(manager);
    Check(state.writes.size() == 4 && std::get<2>(state.writes[0]) == 10 && std::get<2>(state.writes[2]) == 20,
        "each PID computes its own sample delta");
    CProcessMgr independent;
    Check(independent.AddProcessInfo(stGS, 1, 0, FALSE), "independent manager fixture");
    state.writes.clear();
    Poll(independent);
    Check(state.writes.size() == 2 && std::get<2>(state.writes[0]) == 1,
        "manager instances do not share static sampling history");
    state.writes.clear();
    state.systemKernel = 30; state.systemUser = 20;
    state.processes[1].kernel = 40;
    Poll(independent);
    Check(state.writes.size() == 2 && std::get<2>(state.writes[0]) == 80,
        "native counter rollback restarts sampling without negative CPU");
    state.writes.clear();
    state.systemKernel = 31;
    state.processes[1].kernel = 1000;
    Poll(independent);
    Check(state.writes.size() == 2 && std::get<2>(state.writes[0]) == 100,
        "inconsistent counter reads cannot publish CPU above one hundred percent");
    state.writes.clear();
    state.processes[1].memory = (std::numeric_limits<SIZE_T>::max)();
    Poll(independent);
    Check(state.writes.empty(), "memory value must fit native signed statistics representation");
    state.processes[1].memory = 123;
    state.writesOk = false;
    Poll(independent);
    Check(state.writes.size() == 2, "both metric writes are attempted on publication failure");
    Check(state.handles.empty(), "metric failure and rollback paths release query handles");
}
}

BOOL WINAPI GuardTestEnumProcesses(DWORD* values, DWORD bytes, DWORD* written)
{
    state.enumCapacity.push_back(bytes / sizeof(DWORD));
    if (state.enumFailure) return FALSE;
    const auto count = (std::min)(state.enumeration.size(), static_cast<std::size_t>(bytes / sizeof(DWORD)));
    std::copy_n(state.enumeration.data(), count, values);
    *written = state.badEnumBytes ? state.badEnumBytes : static_cast<DWORD>(count * sizeof(DWORD));
    return TRUE;
}
HANDLE WINAPI GuardTestOpenProcess(DWORD access, BOOL inherit, DWORD pid)
{
    Check(!inherit, "process query handles are not inherited");
    Check(access != PROCESS_ALL_ACCESS, "process queries use required access rights");
    state.opens.emplace_back(pid, access);
    const auto found = state.processes.find(pid);
    if (found == state.processes.end()) { SetLastError(ERROR_INVALID_PARAMETER); return nullptr; }
    if (found->second.denied) { SetLastError(ERROR_ACCESS_DENIED); return nullptr; }
    return MakeHandle(pid);
}
BOOL WINAPI GuardTestEnumProcessModules(HANDLE handle, HMODULE* module, DWORD, DWORD* needed)
{
    auto* process = Lookup(handle);
    if (!process || !process->modules) return FALSE;
    *module = reinterpret_cast<HMODULE>(1); *needed = sizeof(HMODULE); return TRUE;
}
DWORD WINAPI GuardTestGetModuleBaseNameA(HANDLE handle, HMODULE, char* destination, DWORD size)
{
    auto* process = Lookup(handle);
    if (!process || !process->image) return 0;
    const auto count = (std::min)(process->name.size(), static_cast<std::size_t>(size - 1));
    std::memcpy(destination, process->name.data(), count); destination[count] = '\0';
    return static_cast<DWORD>(count);
}
BOOL WINAPI GuardTestCloseHandle(HANDLE handle)
{
    Check(state.handles.erase(handle) == 1, "native handle is closed exactly once");
    ++state.closed;
    return TRUE;
}
BOOL WINAPI GuardTestCreateProcessA(const char* application, char* command, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
    BOOL inherit, DWORD flags, void*, const char* directory, LPSTARTUPINFOA startup, LPPROCESS_INFORMATION child)
{
    fault::after = -1; // The native API does not throw C++ allocation failures.
    state.launches.push_back({application ? application : "", command, directory ? directory : "", flags, inherit, startup->cb});
    if (state.failLaunch && state.launches.size() == state.failLaunch) { SetLastError(ERROR_FILE_NOT_FOUND); return FALSE; }
    const auto pid = ++state.nextPid;
    state.processes[pid] = {};
    child->dwProcessId = pid;
    child->hProcess = MakeHandle(pid);
    child->hThread = MakeHandle(pid, true);
    return TRUE;
}
BOOL WINAPI GuardTestTerminateProcess(HANDLE handle, UINT)
{
    auto* process = Lookup(handle);
    if (!process) return FALSE;
    state.terminations.push_back(state.handles.at(handle).pid);
    if (!process->terminate) return FALSE;
    process->wait = WAIT_OBJECT_0;
    return TRUE;
}
BOOL WINAPI GuardTestGetSystemTimes(FILETIME* idle, FILETIME* kernel, FILETIME* user)
{
    if (!state.systemOk) return FALSE;
    *idle = {}; *kernel = Time(state.systemKernel); *user = Time(state.systemUser); return TRUE;
}
BOOL WINAPI GuardTestGetProcessTimes(HANDLE handle, FILETIME* creation, FILETIME* exit, FILETIME* kernel, FILETIME* user)
{
    auto* process = Lookup(handle);
    if (!process || !process->times) return FALSE;
    *creation = Time(process->creation); *exit = {}; *kernel = Time(process->kernel); *user = Time(process->user); return TRUE;
}
BOOL WINAPI GuardTestGetProcessMemoryInfo(HANDLE handle, PPROCESS_MEMORY_COUNTERS counters, DWORD)
{
    auto* process = Lookup(handle);
    if (!process || !process->memoryOk) return FALSE;
    Check(counters->cb == sizeof(*counters), "memory counter structure size initialized");
    counters->PagefileUsage = process->memory; return TRUE;
}
BOOL WINAPI GuardTestGetExitCodeProcess(HANDLE handle, DWORD* exit)
{
    auto* process = Lookup(handle);
    if (!process) return FALSE;
    *exit = process->exit; return TRUE;
}
DWORD WINAPI GuardTestWaitForSingleObject(HANDLE handle, DWORD timeout)
{
    Check(timeout == 0, "process polling never blocks guard loop");
    auto* process = Lookup(handle);
    return process ? process->wait : WAIT_FAILED;
}
UINT64 GuardTestTick() { return state.tick; }
INT32 GuardTestStatisIndex(UINT32) { return state.statsIndex; }
BOOL GuardTestSetStatis(INT32 index, INT32 field, INT64 value)
{
    state.writes.emplace_back(index, field, value); return state.writesOk;
}
void GuardTestResetStatis() { ++state.resets; }

CGDMessageHandler::CGDMessageHandler() = default;
CGDMessageHandler::~CGDMessageHandler() = default;
CGDMessageHandler& CGDMessageHandler::GetInstance() { static CGDMessageHandler instance; return instance; }
void CGDMessageHandler::OnAccepted(ISocketAcceptor*, ISocketSession*) {}
void CGDMessageHandler::OnConnected(ISocketSession*) {}
void CGDMessageHandler::OnRecv(ISocketSession*, PACKET_BUFFER*, UINT32) {}
void CGDMessageHandler::OnDisconnected(ISocketSession*, BOOL) {}
BOOL CGDMessageHandler::DoAllPing() { return TRUE; }
BOOL CGDMessageHandler::DoMTUpdateProcess(INT32 status, UINT32 pid, UINT8 type, UINT32 session, BOOL automatic)
{
    state.notices.push_back({status, pid, type, session, automatic});
    if (state.notify) state.notify();
    return state.noticesOk;
}
BOOL CGDMessageHandler::DoMTUpdateStatisInfo() { return TRUE; }
BOOL CGDMessageHandler::DoMTError(INT32, const char*, ...) { return TRUE; }
void CGDMessageHandler::GDAlertMT(const char*, ...) { ++state.alerts; }

int main(int argc, char** argv)
{
    _set_error_mode(_OUT_TO_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    std::set_terminate([] { std::fprintf(stderr, "fixture: terminate invoked\n"); std::_Exit(2); });
    std::fprintf(stderr, "process fixture: discovery\n");
    Discovery();
    std::fprintf(stderr, "process fixture: closing\n");
    Closing();
    if (argc == 2 && std::string_view{argv[1]} == "--baseline")
    {
        std::printf("guard process baseline: %d checks, %d failures\n", checks, failures);
        return failures != 0;
    }
    std::fprintf(stderr, "process fixture: large discovery\n");
    LargeDiscovery();
    std::fprintf(stderr, "process fixture: registration\n");
    Registration();
    std::fprintf(stderr, "process fixture: polling\n");
    Polling();
    std::fprintf(stderr, "process fixture: metric isolation\n");
    MetricIsolation();
    const auto temporary = std::filesystem::canonical(std::filesystem::temp_directory_path());
    const auto root = temporary / std::format("cqyh guard processes {} {}", GetCurrentProcessId(),
        std::chrono::steady_clock::now().time_since_epoch().count());
    if (!std::filesystem::create_directory(root)) return 1;
    for (UINT8 type = stMS; type <= stMG; ++type)
    {
        const auto directory = root / GetServerName(type);
        std::filesystem::create_directories(directory);
        for (const auto suffix : {"64.exe", "64_d.exe"})
            std::ofstream(directory / (std::string{GetServerName(type)} + suffix)).put('\0');
    }
    std::fprintf(stderr, "process fixture: launching\n");
    Launching(root);
    const auto resolved = std::filesystem::canonical(root);
    if (resolved.parent_path() != temporary || resolved.filename() != root.filename()) return 1;
    std::filesystem::remove_all(resolved);
    Check(state.handles.empty(), "fixture finishes with no native handles");
    std::printf("guard processes: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
