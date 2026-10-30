#include "stdafx.h"
#include "server_def.h"
#include "process_option.h"
#include "gd_session_mgr.h"
#include "process_mgr.h"
#include "native_handle.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <limits>
#include <optional>
#include <string>
#include <vector>
#include "statis_info.h"
#include "time_ex.h"
#include "gd_message_handler.h"

#if defined(WIN32)
    #include "psapi.h"
#endif

#include "guard_config.h"

CProcessMgr CProcessMgr::ms_Instance;
CProcessMgr::CProcessMgr()
{
	m_qwLastStatisTick = 0;
}

CProcessMgr::~CProcessMgr()
{
    UnInit();
}

namespace
{
bool IsServerType(UINT8 type) noexcept { return type >= stMS && type <= stMG; }
bool SameCreationTime(FILETIME left, FILETIME right) noexcept
{
    return left.dwLowDateTime == right.dwLowDateTime && left.dwHighDateTime == right.dwHighDateTime;
}
bool HasCreationTime(FILETIME time) noexcept { return time.dwLowDateTime || time.dwHighDateTime; }

bool CloseTrackedProcess(const SProcessInfo& process)
{
    if (process.dwIP != 0)
    {
        ERR("Cannot terminate remote process %u locally", process.dwProcessID);
        return false;
    }
    if (!HasCreationTime(process.ftProcessCreationTime))
    {
        ERR("Cannot verify identity of process %u for termination", process.dwProcessID);
        return false;
    }
    guard::NativeHandle handle{OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE, process.dwProcessID)};
    if (!handle)
        return false;
    if (HasCreationTime(process.ftProcessCreationTime))
    {
        FILETIME creation{}, exit{}, kernel{}, user{};
        if (!GetProcessTimes(handle.get(), &creation, &exit, &kernel, &user) ||
            !SameCreationTime(creation, process.ftProcessCreationTime))
        {
            ERR("Process %u creation time no longer matches the tracked process", process.dwProcessID);
            return false;
        }
    }
    return TerminateProcess(handle.get(), 0) != FALSE;
}
}

BOOL CProcessMgr::Init()
{
    try
    {
        std::vector<DWORD> processes(1024);
        DWORD bytes = 0;
        for (;;)
        {
            if (!EnumProcesses(processes.data(), static_cast<DWORD>(processes.size() * sizeof(DWORD)), &bytes))
                return FALSE;
            if (bytes % sizeof(DWORD) != 0 || bytes > processes.size() * sizeof(DWORD))
                return FALSE;
            if (bytes < processes.size() * sizeof(DWORD))
                break;
            if (processes.size() > (std::numeric_limits<DWORD>::max)() / sizeof(DWORD) / 2)
                return FALSE;
            processes.resize(processes.size() * 2);
        }
        processes.resize(bytes / sizeof(DWORD));

        CSprocessList discovered;
        for (const auto pid : processes)
        {
            if (pid == 0 || std::ranges::any_of(discovered, [pid](const auto& info) { return info.dwProcessID == pid; }))
                continue;
            guard::NativeHandle handle{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid)};
            if (!handle)
                continue;
            HMODULE module{};
            DWORD needed = 0;
            if (!EnumProcessModules(handle.get(), &module, sizeof(module), &needed))
                continue;
            std::array<char, MAX_PATH> buffer{};
            const auto length = GetModuleBaseNameA(handle.get(), module, buffer.data(), static_cast<DWORD>(buffer.size()));
            if (length == 0 || length >= buffer.size())
                continue;
            const std::string_view executable{buffer.data(), length};
            for (UINT8 type = stMS; type <= stMG; ++type)
            {
                const auto name = GetServerName(type);
                if (executable != std::format("{}64.exe", name) && executable != std::format("{}64_d.exe", name))
                    continue;
                SProcessInfo info;
                info.byType = type;
                info.dwProcessID = pid;
                FILETIME creation{}, exit{}, kernel{}, user{};
                if (GetProcessTimes(handle.get(), &creation, &exit, &kernel, &user))
                    info.ftProcessCreationTime = creation;
                discovered.push_back(info);
                break;
            }
        }
        m_listProcess.swap(discovered);
        m_cpuSamples.clear();
        m_qwLastStatisTick = 0;
        return TRUE;
    }
    catch (const std::exception& error)
    {
        ERR("Process enumeration failed: %s", error.what());
        return FALSE;
    }
}

BOOL CProcessMgr::UnInit(void)
{
    m_listProcess.clear();
    m_cpuSamples.clear();
    m_qwLastStatisTick = 0;
    return TRUE;
}

CProcessMgr& CProcessMgr::GetInstance(void)
{
    return ms_Instance;
}

BOOL CProcessMgr::StartProcess(UINT8 byType, char* szCmdLine, BOOL bView, BOOL bRunRelease, BOOL bAuto)
{
    if (!IsServerType(byType))
        return FALSE;
    // These services have a single local instance. Gates and game servers may have several.
    const bool single = byType == stMS || byType == stDB || byType == stLS || byType == stBS || byType == stDL;
    if (single && std::ranges::any_of(m_listProcess, [byType](const auto& info) {
        return info.byType == byType && info.dwIP == 0;
    }))
        return FALSE;
    try
    {
        namespace fs = std::filesystem;
        std::error_code error;
        const fs::path configured{g_GuardConfig.szServerRunPath};
        const auto directory = fs::absolute(configured.empty() ? fs::path{"."} : configured, error).lexically_normal();
        if (error)
            return FALSE;
        const auto name = GetServerName(byType);
        const auto executable = directory / name / std::format("{}64{}.exe", name, bRunRelease ? "" : "_d");
        if (!fs::is_regular_file(executable, error) || error)
        {
            const auto text = executable.string();
            GD_ERROR_MT("file %s doesn't exist", text.c_str());
            return FALSE;
        }
        const auto application = executable.string();
        const auto workingDirectory = directory.string();
        std::string command = std::format("\"{}\"", application);
        if (szCmdLine && *szCmdLine)
        {
            command += ' ';
            command += szCmdLine;
        }
        if (bView)
        {
            static constexpr std::array positions{
                "", "", "950 455", "240 455", "5 5", "240 5", "240 5", "950 5", "830 15", "5 5", "240 455", "5 5"};
            static_assert(positions.size() == stMG + 1);
            command += std::format(" view {}", positions[byType]);
        }
        // Allocate the tracking node before creating the child. splice cannot allocate.
        CSprocessList pending(1);
        auto& record = pending.front();
        record.byType = byType;
        record.bAuto = bAuto != FALSE;
        STARTUPINFOA startup{};
        startup.cb = sizeof(startup);
        PROCESS_INFORMATION child{};
        if (!CreateProcessA(application.c_str(), command.data(), nullptr, nullptr, FALSE,
            CREATE_NEW_CONSOLE, nullptr, workingDirectory.c_str(), &startup, &child))
        {
            ERR("CreateProcess %s failed: %lu", application.c_str(), GetLastError());
            return FALSE;
        }
        guard::NativeHandle process{child.hProcess};
        guard::NativeHandle thread{child.hThread};
        record.dwProcessID = child.dwProcessId;
        FILETIME creation{}, exit{}, kernel{}, user{};
        if (GetProcessTimes(process.get(), &creation, &exit, &kernel, &user))
            record.ftProcessCreationTime = creation;
        // A newly created child may reuse the PID of an exited, unpolled record.
        m_listProcess.remove_if([&](const auto& info) {
            return info.dwIP == 0 && info.dwProcessID == child.dwProcessId;
        });
        m_cpuSamples.erase(child.dwProcessId);
        m_listProcess.splice(m_listProcess.end(), pending);
        try
        {
            const auto& started = m_listProcess.back();
            LOG_CHECK_ERROR(CGDMessageHandler::GetInstance().DoMTUpdateProcess(InternalMessage::epsUnonnect,
                started.dwProcessID, started.byType, started.bySessionID, started.bAuto));
        }
        catch (const std::exception& error)
        {
            ERR("Created process %lu but notification failed: %s", child.dwProcessId, error.what());
        }
        catch (...)
        {
            ERR("Created process %lu but notification failed with an unknown exception", child.dwProcessId);
        }
        return TRUE;
    }
    catch (const std::exception& error)
    {
        ERR("Process launch failed: %s", error.what());
        return FALSE;
    }
}

BOOL CProcessMgr::CloseProcess(UINT8 byType)
{
    if (!IsServerType(byType))
        return FALSE;
    bool found = false;
    bool success = true;
    for (const auto& process : m_listProcess)
    {
        if (process.byType != byType)
            continue;
        found = true;
        const bool closed = CloseTrackedProcess(process);
        LOG_CHECK_ERROR(closed);
        success = closed && success;
    }
    // Removal and notification happen when the periodic process poll observes exit.
    return found && success;
}

BOOL CProcessMgr::StartAllProcess(char* szCmdLine, BOOL bView, BOOL bRunRelease)
{
    struct Policy { UINT8 type; INT32 requested; INT32 limit; bool single; };
    const std::array policies{
        Policy{stMS, g_GuardConfig.nLocalMSCount, 1, true},
        Policy{stDB, g_GuardConfig.nLocalDBCount, 1, true},
        Policy{stBS, g_GuardConfig.nLocalBSCount, 1, true},
        Policy{stLS, g_GuardConfig.nLocalLSCount, 1, true},
        Policy{stGT, g_GuardConfig.nLocalGTCount, MAX_GT_SESSION_COUNT - 1, false},
        Policy{stGS, g_GuardConfig.nLocalGSCount, MAX_GS_SESSION_COUNT - 1, false},
        Policy{stGM, g_GuardConfig.nLocalGMCount, 1, true},
        Policy{stDL, g_GuardConfig.nLocalDLCount, 1, true},
        Policy{stMM, g_GuardConfig.nLocalMMCount, 1, true},
        Policy{stMG, g_GuardConfig.nLocalMGCount, MAX_MG_SESSION_COUNT - 1, false}};
    for (const auto& policy : policies)
    {
        if (policy.requested < 0 || (!policy.single && policy.requested > policy.limit))
        {
            ERR("Invalid configured process count %d for %s", policy.requested, GetServerName(policy.type));
            return FALSE;
        }
    }
    for (const auto& policy : policies)
    {
        const auto current = std::ranges::count_if(m_listProcess, [&](const auto& info) {
            return info.dwIP == 0 && info.byType == policy.type;
        });
        // Preserve the original single-instance guard and launch order.
        if (policy.single && current != 0)
            return FALSE;
        const auto requested = (std::min)(policy.requested, policy.limit);
        for (auto count = current; count < requested; ++count)
            if (!StartProcess(policy.type, szCmdLine, bView, bRunRelease, TRUE))
                return FALSE;
    }
    return TRUE;
}

BOOL CProcessMgr::CloseAllProcess()
{
    bool success = true;
    for (const auto& process : m_listProcess)
    {
        const bool closed = CloseTrackedProcess(process);
        LOG_CHECK_ERROR(closed);
        success = closed && success;
    }
    return success;
}

INT32 CProcessMgr::CountRuningProcessByType(UINT8 byType)
{
    const auto count = byType == stAll ? m_listProcess.size() :
        static_cast<std::size_t>(std::ranges::count(m_listProcess, byType, &SProcessInfo::byType));
    return static_cast<INT32>((std::min)(count, static_cast<std::size_t>((std::numeric_limits<INT32>::max)())));
}

BOOL CProcessMgr::IsProcessRuningByID(UINT32 dwProcessID)
{
    return std::ranges::any_of(m_listProcess, [dwProcessID](const auto& process) {
        return process.dwIP == 0 && process.dwProcessID == dwProcessID;
    });
}

namespace
{
UINT64 FileTicks(FILETIME time) noexcept
{
    return (static_cast<UINT64>(time.dwHighDateTime) << 32) | time.dwLowDateTime;
}
}

BOOL CProcessMgr::UpdateServersStatis(const SProcessInfo& tracked)
{
    const auto dwProcessID = tracked.dwProcessID;
    const auto index = g_GetProcessStatisIndex(dwProcessID);
    if (index < 0 || index >= MAX_STATIS_PROCESS_COUNT)
        return FALSE;
    guard::NativeHandle process{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID)};
    if (!process)
        return FALSE;
    FILETIME idle{}, systemKernel{}, systemUser{}, creation{}, exit{}, kernel{}, user{};
    PROCESS_MEMORY_COUNTERS memory{};
    memory.cb = sizeof(memory);
    if (!GetSystemTimes(&idle, &systemKernel, &systemUser) ||
        !GetProcessTimes(process.get(), &creation, &exit, &kernel, &user) ||
        !GetProcessMemoryInfo(process.get(), &memory, sizeof(memory)))
        return FALSE;
    if ((HasCreationTime(tracked.ftProcessCreationTime) &&
        !SameCreationTime(creation, tracked.ftProcessCreationTime)) ||
        memory.PagefileUsage > static_cast<SIZE_T>((std::numeric_limits<INT64>::max)()))
        return FALSE;
    try
    {
        const CpuSample next{creation, FileTicks(systemKernel), FileTicks(systemUser), FileTicks(kernel), FileTicks(user)};
        auto& previous = m_cpuSamples[dwProcessID];
        // A reused PID or reset native counter starts a new sampling interval.
        if (!SameCreationTime(previous.creation, creation) || next.systemKernel < previous.systemKernel ||
            next.systemUser < previous.systemUser || next.processKernel < previous.processKernel ||
            next.processUser < previous.processUser)
            previous = {};
        const long double system = static_cast<long double>(next.systemKernel - previous.systemKernel) +
            (next.systemUser - previous.systemUser);
        const long double used = static_cast<long double>(next.processKernel - previous.processKernel) +
            (next.processUser - previous.processUser);
        const auto cpu = system > 0 ? static_cast<INT64>(std::clamp(100.0L * used / system, 0.0L, 100.0L)) : 0;
        previous = next;
        const auto cpuWritten = g_SetProcessStatisInfo(index, statCpuUsage, cpu);
        const auto memoryWritten = g_SetProcessStatisInfo(index, statMemUsage, static_cast<INT64>(memory.PagefileUsage));
        return cpuWritten && memoryWritten;
    }
    catch (const std::exception& error)
    {
        ERR("Process statistics failed: %s", error.what());
        return FALSE;
    }
}

BOOL CProcessMgr::MainLoop()
{
    if (m_updating)
        return TRUE;
    const auto now = GetTickCountEx();
    if (now < m_qwLastStatisTick)
    {
        m_qwLastStatisTick = now;
        return TRUE;
    }
    if (now - m_qwLastStatisTick <= 1000)
        return TRUE;
    m_qwLastStatisTick = now;
    m_updating = true;
    struct ResetPolling { bool& active; ~ResetPolling() { active = false; } } reset{m_updating};
    try
    {
        // A transient query failure must not stop the guard's main loop.
        LOG_CHECK_ERROR(UpdateServersInfo());
        if (CGDSessionMgr::GetInstance().GetMTSession().pSocketSession)
            LOG_CHECK_ERROR(CGDMessageHandler::GetInstance().DoMTUpdateStatisInfo());
        g_ResetStatisInfo();
    }
    catch (const std::exception& error)
    {
        ERR("Process poll failed: %s", error.what());
    }
    catch (...)
    {
        ERR("Process poll failed with an unknown exception");
    }
    return TRUE;
}

BOOL CProcessMgr::UpdateServersInfo()
{
    bool success = true;
    // Notifications may reenter the manager and alter the live collection.
    const std::vector<SProcessInfo> snapshot(m_listProcess.begin(), m_listProcess.end());
    for (const auto& saved : snapshot)
    {
        auto current = std::ranges::find_if(m_listProcess, [&](const auto& info) {
            return info.dwProcessID == saved.dwProcessID && info.dwIP == saved.dwIP &&
                SameCreationTime(info.ftProcessCreationTime, saved.ftProcessCreationTime);
        });
        if (current == m_listProcess.end() || saved.dwIP != 0)
            continue;
        guard::NativeHandle process{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
            FALSE, saved.dwProcessID)};
        bool removed = false;
        const char* reason = "not found";
        std::optional<DWORD> exitCode;
        if (!process)
        {
            if (GetLastError() != ERROR_INVALID_PARAMETER)
            {
                success = false; // Access denied is not evidence of exit.
                continue;
            }
            removed = true;
        }
        else
        {
            const auto state = WaitForSingleObject(process.get(), 0);
            if (state == WAIT_FAILED || (state != WAIT_TIMEOUT && state != WAIT_OBJECT_0))
            {
                success = false;
                continue;
            }
            removed = state == WAIT_OBJECT_0;
            if (removed)
            {
                reason = "exited";
                DWORD code = 0;
                if (GetExitCodeProcess(process.get(), &code))
                    exitCode = code;
            }
            if (!removed && HasCreationTime(saved.ftProcessCreationTime))
            {
                FILETIME creation{}, exit{}, kernel{}, user{};
                if (!GetProcessTimes(process.get(), &creation, &exit, &kernel, &user))
                {
                    success = false;
                    continue;
                }
                removed = !SameCreationTime(creation, saved.ftProcessCreationTime);
                if (removed) reason = "PID reused";
            }
        }
        if (removed)
        {
            m_listProcess.erase(current);
            m_cpuSamples.erase(saved.dwProcessID);
            if (exitCode)
                INF("process %u [%s:%u] exited with code %lu", saved.dwProcessID,
                    GetServerShortName(saved.byType), saved.bySessionID, *exitCode);
            else
                INF("process %u [%s:%u] disappeared: %s", saved.dwProcessID,
                    GetServerShortName(saved.byType), saved.bySessionID, reason);
            LOG_CHECK_ERROR(CGDMessageHandler::GetInstance().DoMTUpdateProcess(
                InternalMessage::epsDestroy, saved.dwProcessID, 0, 0, FALSE));
            GD_ALERT_MT("process %u [%s:%u] disappeared!", saved.dwProcessID,
                GetServerShortName(saved.byType), saved.bySessionID);
        }
        else
            success = UpdateServersStatis(saved) && success;
    }
    return success;
}

BOOL CProcessMgr::SetProcessInfo(UINT32 dwProcessID, UINT8 byType, UINT8 bySessionID)
{
    if (!IsServerType(byType))
        return FALSE;
    switch (byType)
    {
    case stGS: if (bySessionID == 0 || bySessionID >= MAX_GS_SESSION_COUNT) return FALSE; break;
    case stGT: if (bySessionID == 0 || bySessionID >= MAX_GT_SESSION_COUNT) return FALSE; break;
    case stMG: if (bySessionID == 0 || bySessionID >= MAX_MG_SESSION_COUNT) return FALSE; break;
    default:
        LOG_CHECK_ERROR(bySessionID == 1);
        bySessionID = 1;
        break;
    }
    const auto process = std::ranges::find_if(m_listProcess, [=](const auto& info) {
        return info.dwIP == 0 && info.dwProcessID == dwProcessID;
    });
    if (process == m_listProcess.end())
        return FALSE;
    // A fresh registration may arrive before the poll retires an old PID owner.
    guard::NativeHandle handle{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessID)};
    FILETIME creation{}, exit{}, kernel{}, user{};
    const bool verified = handle && GetProcessTimes(handle.get(), &creation, &exit, &kernel, &user);
    if (process->byType != byType && (!verified || (HasCreationTime(process->ftProcessCreationTime) &&
        SameCreationTime(process->ftProcessCreationTime, creation))))
        return FALSE;
    if (verified)
    {
        if (!SameCreationTime(process->ftProcessCreationTime, creation))
            m_cpuSamples.erase(dwProcessID);
        process->ftProcessCreationTime = creation;
    }
    process->byType = byType;
    process->bySessionID = bySessionID;
    return TRUE;
}

BOOL CProcessMgr::SyncProcessInfo(UINT32 nState, UINT32 dwProcessID)
try
{
    const auto process = std::ranges::find_if(m_listProcess, [=](const auto& info) {
        return info.dwIP == 0 && info.dwProcessID == dwProcessID;
    });
    if (process == m_listProcess.end())
        return FALSE;
    const auto copy = *process;
    return CGDMessageHandler::GetInstance().DoMTUpdateProcess(nState, copy.dwProcessID,
        copy.byType, copy.bySessionID, copy.bAuto);
}
catch (const std::exception& error)
{
    ERR("Process synchronization failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("Process synchronization failed with an unknown exception");
    return FALSE;
}

BOOL CProcessMgr::SyncAllProcessInfo()
{
    try
    {
        bool success = true;
        const std::vector<SProcessInfo> snapshot(m_listProcess.begin(), m_listProcess.end());
        for (const auto& saved : snapshot)
        {
            const auto current = std::ranges::find_if(m_listProcess, [&](const auto& info) {
                return info.dwProcessID == saved.dwProcessID && info.dwIP == saved.dwIP &&
                    SameCreationTime(info.ftProcessCreationTime, saved.ftProcessCreationTime);
            });
            if (current == m_listProcess.end())
                continue;
            const auto process = *current;
            const BOOL connected = process.dwIP == 0 && process.bySessionID > 0 &&
                CGDSessionMgr::GetInstance().CheckSocketConnected(process.byType, process.bySessionID);
            const auto state = connected ? InternalMessage::epsNormal : InternalMessage::epsUnonnect;
            const auto sent = CGDMessageHandler::GetInstance().DoMTUpdateProcess(state,
                process.dwProcessID, process.byType, process.bySessionID, process.bAuto);
            success = sent && success;
        }
        return success;
    }
    catch (const std::exception& error)
    {
        ERR("Process synchronization failed: %s", error.what());
        return FALSE;
    }
    catch (...)
    {
        ERR("Process synchronization failed with an unknown exception");
        return FALSE;
    }
}

BOOL CProcessMgr::AddProcessInfo(UINT8 byType, UINT32 dwProcessID, UINT32 dwIP, BOOL bAuto)
{
    if (!IsServerType(byType) || dwProcessID == 0)
        return FALSE;
    const auto duplicate = std::ranges::find_if(m_listProcess, [=](const auto& info) {
        return info.dwProcessID == dwProcessID && info.dwIP == dwIP;
    });
    if (duplicate != m_listProcess.end())
        return duplicate->byType == byType;
    try
    {
        SProcessInfo info;
        info.byType = byType;
        info.dwProcessID = dwProcessID;
        info.dwIP = dwIP;
        info.bAuto = bAuto != FALSE;
        if (dwIP == 0)
        {
            guard::NativeHandle process{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessID)};
            FILETIME creation{}, exit{}, kernel{}, user{};
            if (process && GetProcessTimes(process.get(), &creation, &exit, &kernel, &user))
                info.ftProcessCreationTime = creation;
        }
        m_listProcess.push_back(info);
        return TRUE;
    }
    catch (const std::exception& error)
    {
        ERR("Process registration failed: %s", error.what());
        return FALSE;
    }
}
