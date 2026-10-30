#include "stdafx.h"

#include "version.h"
#include "dump.h"
#include "time_ex.h"
#include "report.h"
#include "statis_info.h"
#include "net.h"

#include "global_config.h"	//LoadGuardConfig
#include "guard_config.h"

#include "gd_session_mgr.h"
#include "process_mgr.h"
#include "cmd_mgr.h"
#include "process_option.h"
#include "maintain_tool_version.h"

#include "guardserver.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <format>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

BOOL InstallService();
BOOL UninstallService();
void WINAPI ServiceMain(DWORD, LPSTR*);
void WINAPI ServiceCtrlHandler(DWORD dwOpcode);
char g_szServiceName[] = "guard_cqyh";
SERVICE_STATUS_HANDLE g_hServiceStatus;
SERVICE_STATUS g_status;
BOOL g_bStartFromCmd = FALSE;
namespace {
std::atomic_bool stopRequested{false};
std::mutex statusMutex;
bool dumpStarted=false;
std::mutex completionMutex;
std::condition_variable completion;
bool runFinished=false;
bool reportService(DWORD state, DWORD error=NO_ERROR)
{
    const std::lock_guard lock{statusMutex};
    if (!g_hServiceStatus) return false;
    if (state==SERVICE_STOP_PENDING && g_status.dwCurrentState==SERVICE_STOPPED) return false;
    if (state==SERVICE_RUNNING && stopRequested) state=SERVICE_STOP_PENDING;
    // A control callback may have requested stop before START_PENDING is sent.
    if (state==SERVICE_START_PENDING && stopRequested) state=SERVICE_STOP_PENDING;
    g_status.dwCurrentState=state;
    g_status.dwControlsAccepted=state==SERVICE_RUNNING ? SERVICE_ACCEPT_STOP : 0;
    g_status.dwWin32ExitCode=error;
    g_status.dwServiceSpecificExitCode=0;
    const bool pending=state==SERVICE_START_PENDING || state==SERVICE_STOP_PENDING;
    g_status.dwCheckPoint=pending ? g_status.dwCheckPoint+1 : 0;
    g_status.dwWaitHint=pending ? 10000 : 0;
    return SetServiceStatus(g_hServiceStatus,&g_status)!=FALSE;
}
void requestStop() noexcept
{
    stopRequested=true;
    CGuardServer::GetInstance().Close();
}
BOOL WINAPI consoleControl(DWORD control)
{
    switch(control) {
    case CTRL_C_EVENT: case CTRL_BREAK_EVENT:
        requestStop();
        return TRUE;
    case CTRL_CLOSE_EVENT: case CTRL_LOGOFF_EVENT: case CTRL_SHUTDOWN_EVENT:
        requestStop();
        // Returning from a close handler lets Windows terminate the process.
        // Give the main thread a bounded opportunity to finish owned cleanup.
        {
            std::unique_lock lock{completionMutex};
            completion.wait_for(lock,std::chrono::seconds(4),[] { return runFinished; });
        }
        return TRUE;
    default: return FALSE;
    }
}
struct ServiceCloser { void operator()(SC_HANDLE h) const noexcept { CloseServiceHandle(h); } };
using ServiceOwner=std::unique_ptr<std::remove_pointer_t<SC_HANDLE>,ServiceCloser>;
std::filesystem::path executablePath()
{
    std::vector<wchar_t> buffer(256);
    for (;;) {
        const auto count=GetModuleFileNameW(nullptr,buffer.data(),static_cast<DWORD>(buffer.size()));
        if (!count) throw std::system_error(GetLastError(),std::system_category(),"GetModuleFileNameW");
        if (count<buffer.size()) return std::filesystem::path{std::wstring(buffer.data(),count)};
        if (buffer.size()>=32768) throw std::length_error("guard executable path exceeds Windows limit");
        buffer.resize(buffer.size()*2);
    }
}
template<class Work> bool cleanup(const char* name, Work&& work) noexcept
{
    try { if (work()) return true; ERR("guard cleanup failed: %s",name); }
    catch (const std::exception& e) { ERR("guard cleanup %s: %s",name,e.what()); }
    catch (...) { ERR("guard cleanup threw: %s",name); }
    return false;
}
}

void InitServiceState()
{
    const std::lock_guard lock{statusMutex};
    stopRequested=false;
    g_hServiceStatus = NULL;
    g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_status.dwCurrentState = SERVICE_STOPPED;
    g_status.dwControlsAccepted = 0;
    g_status.dwWin32ExitCode = 0;
    g_status.dwServiceSpecificExitCode = 0;
    g_status.dwCheckPoint = 0;
    g_status.dwWaitHint = 0;
}

BOOL InstallService()
{
    const auto binary=std::format(L"\"{}\"",executablePath().wstring());
    const ServiceOwner scm{OpenSCManagerW(nullptr,nullptr,SC_MANAGER_CREATE_SERVICE)};
    if (!scm) return FALSE;
    const ServiceOwner service{CreateServiceW(scm.get(),L"guard_cqyh",L"guard_cqyh",
        SERVICE_QUERY_STATUS,SERVICE_WIN32_OWN_PROCESS,SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,binary.c_str(),nullptr,nullptr,L"",nullptr,nullptr)};
    return service!=nullptr;
}
BOOL UninstallService()
{
    const ServiceOwner scm{OpenSCManagerW(nullptr,nullptr,SC_MANAGER_CONNECT)};
    if (!scm) return FALSE;
    const ServiceOwner service{OpenServiceW(scm.get(),L"guard_cqyh",SERVICE_STOP|DELETE)};
    if (!service) return FALSE;
    SERVICE_STATUS status{};
    if (!ControlService(service.get(),SERVICE_CONTROL_STOP,&status)
        && GetLastError()!=ERROR_SERVICE_NOT_ACTIVE) return FALSE;
    return DeleteService(service.get());
}

void WINAPI ServiceCtrlHandler(DWORD control)
{
    if (control==SERVICE_CONTROL_STOP) { requestStop(); reportService(SERVICE_STOP_PENDING); }
    else if (control==SERVICE_CONTROL_INTERROGATE) {
        const std::lock_guard lock{statusMutex};
        if (g_hServiceStatus) SetServiceStatus(g_hServiceStatus,&g_status);
    }
}

BOOL _InitServer(void)
{
	INT32 nRetCode = 0;
	CGDMessageHandler& rMessageHandler = CGDMessageHandler::GetInstance();

	CGuardServer& rGuardServer = CGuardServer::GetInstance();

	nRetCode = CLog::GetInstance().Init("guard_server",
		g_GuardConfig.Log.nLogLevelMask,
		g_GuardConfig.Log.nLogOutputMask,
		g_GuardConfig.Log.bEnableLogThread,
		g_GuardConfig.Log.bPrintThreadID,
		g_GuardConfig.Log.bFlushFile);
	LOG_PROCESS_ERROR(nRetCode);
	g_InitReportSetting("GD", 1, g_GuardConfig.szReportAbsolutePath);

	INF("initializing guard server msg %d ver %s", MAINTAIN_TOOLS_MSG_VERSION, MAINTAIN_TOOLS_BUILD_VERSION);

	nRetCode = rGuardServer.Init(&rMessageHandler);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CGDSessionMgr::GetInstance().Init(rGuardServer.GetNetInterface());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = rMessageHandler.Init(rGuardServer.GetNetInterface(), rGuardServer.GetPacketAllocator());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CProcessMgr::GetInstance().Init();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CCmdMgr::GetInstance().Init();
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _UnInitServer() noexcept
{
    bool success=true;
    auto& guard=CGuardServer::GetInstance();
    guard.Close();
    success &= cleanup("workers",[&] { guard.StopBackgroundWork(); return true; });
    CGDMessageHandler::GetInstance().BeginShutdown();
    // Detach borrowed sockets before destroying the transport. Keep the packet
    // allocator and message handler alive until the final callbacks finish.
    success &= cleanup("sessions",[] { return CGDSessionMgr::GetInstance().UnInit(); });
    success &= cleanup("transport",[&] { return guard.UnInit(); });
    success &= cleanup("messages",[] { return CGDMessageHandler::GetInstance().UnInit(); });
    success &= cleanup("process registry",[] { return CProcessMgr::GetInstance().UnInit(); });
    success &= cleanup("commands",[] { return CCmdMgr::GetInstance().UnInit(); });
    if (std::exchange(dumpStarted,false)) success &= cleanup("dump thread",[] { return StopDumpThread(); });
    success &= cleanup("logger",[] { return CLog::GetInstance().UnInit(); });
    return success;
}

BOOL _PreRunServer(int argc, char** argv)
{
    if (!CGuardServer::GetInstance().ReloadConfig())
    {
        ERR("guard configuration initialization failed");
        return FALSE;
    }
    const auto folder = (std::filesystem::path{g_GuardConfig.szServerRunPath} / "log").string();
    dumpStarted = true;
    if (!StartDumpThread(nullptr, TRUE, folder.c_str()))
    {
        ERR("guard dump initialization failed");
        return FALSE;
    }
    const auto title = std::format("guard msg {} ver {}", MAINTAIN_TOOLS_MSG_VERSION, MAINTAIN_TOOLS_BUILD_VERSION);
    SetConsoleTitleA(title.c_str());
    if (!g_bStartFromCmd) g_GuardConfig.Log.nLogOutputMask = lotFile;
    if (!g_InitStatisInfo(GetCurrentProcessId()) || !_InitServer()) return FALSE;
    if (argc > 1 && !stopRequested)
    {
        const int first = ProcessOption(argc, argv);
        if (first < 1)
        {
            ERR("guard command-line options are invalid");
            return FALSE;
        }
        if (argc > first)
        {
            // Preserve the legacy byte limit and space-separated argument format.
            std::string command;
            for (int i = first; i < argc; ++i)
            {
                command.append(argv[i], std::min(std::char_traits<char>::length(argv[i]),
                    COMMON_NAME_LEN - 1 - command.size()));
                if (command.size() < COMMON_NAME_LEN - 1) command += ' ';
            }
            LOG_CHECK_ERROR(CCmdMgr::GetInstance().ExecOPCmd(command.c_str()));
        }
    }
    if (stopRequested) CGuardServer::GetInstance().Close();
    INF("Guard PreRunServer success!");
    return TRUE;
}

INT32 _RunServer()
{
    using namespace std::chrono_literals;
    auto& guard=CGuardServer::GetInstance();
    bool success=true;
    while (!stopRequested && guard.IsRuning()) {
        if (!guard.MainLoop()) { success=false; break; }
        std::this_thread::sleep_for(1ms);
    }
    guard.Close();
    guard.StopBackgroundWork();
    CGDMessageHandler::GetInstance().BeginShutdown();
    success &= CGDSessionMgr::GetInstance().CloseAllSocketSession()!=FALSE;
    // Bound graceful draining; final transport teardown closes unresponsive peers.
    const auto deadline=std::chrono::steady_clock::now()+5s;
    while (CGDSessionMgr::GetInstance().CheckHasSocketSession()) {
        if (std::chrono::steady_clock::now()>=deadline || !guard.MainLoop()) { success=false; break; }
        std::this_thread::sleep_for(1ms);
    }
    return success;
}

namespace {
bool runServer(int argc,char** argv,bool service)
{
    bool success=false;
    try {
        if (_PreRunServer(argc,argv) && (!service || reportService(SERVICE_RUNNING)))
            success=_RunServer()!=FALSE;
    }
    catch (const std::exception& e) { ERR("guard runtime failed: %s",e.what()); }
    catch (...) { ERR("guard runtime threw an exception"); }
    // Do not overwrite an earlier failure with a successful cleanup result.
    const bool cleaned=_UnInitServer()!=FALSE;
    {
        const std::lock_guard lock{completionMutex};
        runFinished=true;
    }
    completion.notify_all();
    return cleaned && success;
}
}

void WINAPI ServiceMain(DWORD,LPSTR*)
{
    const auto handle=RegisterServiceCtrlHandlerA(g_szServiceName,ServiceCtrlHandler);
    if (!handle) return;
    { const std::lock_guard lock{statusMutex}; g_hServiceStatus=handle; }
    bool success=false;
    try {
        if (reportService(SERVICE_START_PENDING)) {
            std::filesystem::current_path(executablePath().parent_path());
            success=runServer(0,nullptr,true);
        }
    }
    catch (const std::exception& e) { ERR("guard service failed: %s",e.what()); }
    catch (...) { ERR("guard service threw an exception"); }
    reportService(SERVICE_STOPPED,success ? NO_ERROR : ERROR_PROCESS_ABORTED);
    const std::lock_guard lock{statusMutex};
    g_hServiceStatus=nullptr;
}

int main(int argc,char** argv)
{
    InitServiceState();
    { const std::lock_guard lock{completionMutex}; runFinished=false; }
    try {
        if (argc>1) {
            if (_stricmp(argv[1],"/install")==0) return InstallService() ? 0 : 1;
            if (_stricmp(argv[1],"/uninstall")==0) return UninstallService() ? 0 : 1;
            g_bStartFromCmd=TRUE;
            if (!SetConsoleCtrlHandler(consoleControl,TRUE)) return 1;
            const bool success=runServer(argc,argv,false);
            SetConsoleCtrlHandler(consoleControl,FALSE);
            return success ? 0 : 1;
        }
        g_bStartFromCmd=FALSE;
        SERVICE_TABLE_ENTRYA table[]{{g_szServiceName,ServiceMain},{nullptr,nullptr}};
        return StartServiceCtrlDispatcherA(table) ? 0 : 1;
    }
    catch (const std::exception& e) { ERR("guard entry failed: %s",e.what()); }
    catch (...) { ERR("guard entry threw an exception"); }
    return 1;
}
