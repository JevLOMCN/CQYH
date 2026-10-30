#include "stdafx.h"
#include <chrono>
#include <thread>
#include "guardserver.h"
#include "guard_deploy.h"

#include <algorithm>
#include <filesystem>
#include <format>
#include <ranges>
#include <span>

//#include "version.h"
//#include "net.h"
//#include "ifile.h"
#include "statis_info.h"
#include "dump.h"
#include "report.h"
#include "time_ex.h"
#include "process_option.h"	//GetServerShortName
//
#include "gd_session_mgr.h"

#include "process_mgr.h"
#include "cmd_mgr.h"
#include "guard_config.h"
#include "global_config.h"	//LoadGuardConfig

//#include "report.h"
//#include "statis_info.h"

const char* s_szFixHeader = 
"********       ********        ********        ********        ********\n"\
"   id            type          session           state            mode \n";

const char* s_szFixLineContent = 
"%6d\t\t%4s\t\t%2d\t%15s\t\t%6s\n";

const char* s_szFixTail = 
"********       ********        ********        ********        ********\n";

const char* s_szEmptyLine =
"                                                                     ";

CGuardServer CGuardServer::ms_Instance;
CGuardServer::CGuardServer()
{
    m_pNet = NULL;
    m_pSocketAcceptor = NULL;
	m_bRun = FALSE;
	m_qwLatestTickCheckDiskSpace = 0LL;
}

CGuardServer::~CGuardServer()
{
    UnInit();
}

CGuardServer& CGuardServer::GetInstance(void)
{
    return ms_Instance;
}

BOOL CGuardServer::Init(INetEventHandler* poNetEventHandler)
{
    if (m_pNet || m_pSocketAcceptor || !poNetEventHandler) return FALSE;
    m_console.clear();
    m_lastScreenTick = 0;
    INT32 nRetCode = 0;
    UINT32 dwAddr;
    m_bRun = TRUE;
	m_qwLatestTickCheckDiskSpace = 0LL;
	memset(m_szProcessingVersion, 0, sizeof(m_szProcessingVersion));
	memset(m_szProcessingPatch, 0, sizeof(m_szProcessingPatch));
    INTRANET_INIT_PARAM IntranetInitParam;
    SIMPLE_INIT_PARAM SimpleInitParam;

    nRetCode = m_PacketAllocator.Init(g_GuardConfig.Net.nSmallPacketCount,
        g_GuardConfig.Net.nLargePacketCount);
    LOG_PROCESS_ERROR(nRetCode);

    // internal
#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_GuardConfig.Net.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_GuardConfig.Net.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_GuardConfig.Net.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_GuardConfig.Net.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_GuardConfig.Net.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_GuardConfig.Net.nPacketBufferSize;
	IntranetInitParam.nDisconnectTimeout = g_GuardConfig.nShareMemDisconnectTimeout;
    m_pNet = guard_runtime::NetOwner{g_CreateNetInterface(netSimple, &m_PacketAllocator, poNetEventHandler, &IntranetInitParam), {netSimple, &g_DestroyNetInterface}};
#else
    SimpleInitParam.nMaxAcceptor = g_GuardConfig.Net.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_GuardConfig.Net.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_GuardConfig.Net.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_GuardConfig.Net.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_GuardConfig.Net.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_GuardConfig.Net.nPacketBufferSize;
    m_pNet = guard_runtime::NetOwner{g_CreateNetInterface(netSimple, &m_PacketAllocator, poNetEventHandler, &SimpleInitParam), {netSimple, &g_DestroyNetInterface}};
#endif
    LOG_PROCESS_ERROR(m_pNet);

    m_pSocketAcceptor = guard_runtime::AcceptorOwner{m_pNet->CreateAcceptor(), {m_pNet.get()}};
    LOG_PROCESS_ERROR(m_pSocketAcceptor);

    dwAddr = inet_addr(g_GuardConfig.szAddress);
    LOG_PROCESS_ERROR(dwAddr != INADDR_NONE);

	nRetCode = m_pSocketAcceptor->Open(dwAddr, g_GuardConfig.dwPort);
	if (!nRetCode)
	{
		INF("failed to open listen port at %s:%d", g_GuardConfig.szAddress, g_GuardConfig.dwPort);
		LOG_PROCESS_ERROR(nRetCode);
	}

    INF("opened listen port at %s:%d", g_GuardConfig.szAddress, g_GuardConfig.dwPort);

    INF("Guard initialized successfully");

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

void CGuardServer::StopBackgroundWork()
{
    m_deployment.stop();
    m_logCleanup.stop();
    m_szProcessingVersion[0] = m_szProcessingPatch[0] = '\0';
    m_roLogFolderEntries.clear();
}

BOOL CGuardServer::UnInit(void)
{
    m_bRun = false;
    StopBackgroundWork();
    m_pSocketAcceptor.reset();
    m_pNet.reset();
    m_console.clear();
    return m_PacketAllocator.UnInit();
}

BOOL CGuardServer::Close(void)
{
    m_bRun = FALSE;
    return TRUE;
}

namespace
{
constexpr UINT64 check_disk_space_interval = 10'000;
}

BOOL CGuardServer::_AsyncEraseOldestLogSubFolders(std::string name)
try
{
    const std::filesystem::path runDirectory{g_GuardConfig.szServerRunPath};
    const auto path = runDirectory / "log" / name;
    return m_logCleanup.start(path,
        [runDirectory, name = std::move(name)](std::stop_token stop) {
            return guard_files::erase_log_folder(runDirectory, name, stop);
        });
}
catch (const std::exception& error)
{
    CRI("cannot start log cleanup: %s", error.what());
    return FALSE;
}

void CGuardServer::_PollLogCleanup()
{
    if (auto completion = m_logCleanup.poll())
    {
        const auto path = completion->path.string();
        const auto message = completion->result
            ? std::format("deleted {} log entries under {}", *completion->result, path)
            : std::format("failed to clean log folder {}: {}", path, completion->result.error().message());
        CRI("%s", message.c_str());
        REPORT(1, "%s", message.c_str());
        GD_ALERT_MT("%s", message.c_str());
        // A failure remains eligible on the next disk check, rather than being
        // silently discarded from the cache and marked as a successful cleanup.
        if (!completion->result) m_roLogFolderEntries.clear();
    }
}

BOOL CGuardServer::MainLoop(void)
{
	INT32 nRetCode = 0;
    UINT64 qwEndTick = 0;
	INT32 nLogCleanThreshold = g_GuardConfig.nLogCleanThreshold;
	nLogCleanThreshold = nLogCleanThreshold > 0 ? nLogCleanThreshold : 16; // unit: GByte

    UINT64 qwStartTick = GetTickCountEx();

    if (!m_pNet) return FALSE;
    nRetCode = m_pNet->MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

    // During graceful shutdown only deliver transport completion callbacks.
    // Do not run commands, process restarts, pings, or filesystem jobs again.
    if (!m_bRun) return TRUE;

    nRetCode = CGDSessionMgr::GetInstance().MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CProcessMgr::GetInstance().MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

	//version check
	nRetCode = 	ProcessDeployServerVersion();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = ProcessHotUpdatePatch();
	LOG_CHECK_ERROR(nRetCode);
	
    _PollLogCleanup();
    if (m_bRun && !m_logCleanup.busy() &&
        (qwStartTick < m_qwLatestTickCheckDiskSpace ||
         qwStartTick - m_qwLatestTickCheckDiskSpace > check_disk_space_interval))
    {
        m_qwLatestTickCheckDiskSpace = qwStartTick;
        std::error_code error;
        const auto space = std::filesystem::space(g_GuardConfig.szServerRunPath, error);
        if (error)
            CRI("cannot query server disk space: %s", error.message().c_str());
        else if (space.available <= std::uintmax_t{1024} * 1024 * 1024 * nLogCleanThreshold)
        {
            if (m_roLogFolderEntries.empty())
                _GetLogSubFolders(g_GuardConfig.szServerRunPath);
            if (!m_roLogFolderEntries.empty())
            {
                if (_AsyncEraseOldestLogSubFolders(m_roLogFolderEntries.front()))
                    m_roLogFolderEntries.erase(m_roLogFolderEntries.begin());
            }
        }
    }

    extern BOOL g_bStartFromCmd;
    if (g_bStartFromCmd && kbhit())
    {
        if (auto line = m_console.push(getch()))
        {
            if (line->overflow) ERR("console command exceeds %zu bytes", guard_runtime::ConsoleInput::capacity);
            else if (!line->text.empty()) LOG_CHECK_ERROR(CCmdMgr::GetInstance().ExecOPCmd(line->text.c_str()));
        }
        PrintCmdPrompt();
    }

    if (qwStartTick - m_lastScreenTick > 500)
	{
		g_MainloopStatisInfo();

        if (g_bStartFromCmd) _UpdateScreen();
        m_lastScreenTick = qwStartTick;
    }

    qwEndTick = GetTickCountEx();
    if (qwEndTick - qwStartTick < (1000 / GAME_FPS))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    else
        CRI("too slow main loop %d", (INT32)(qwEndTick - qwStartTick));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGuardServer::PrintCmdPrompt()
{
    printf("\r%s", s_szEmptyLine);
    const auto text = m_console.text();
    printf("\rcmd:%.*s", static_cast<int>(text.size()), text.data());

    return TRUE;
}

void CGuardServer::Cls()
 {
    COORD coordScreen = { 0, 0 };    /* here's where we'll home the
                                        cursor */ 
    BOOL bSuccess;
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */ 
    DWORD dwConSize;                 /* number of character cells in
                                        the current buffer */ 

    /* get the number of character cells in the current buffer */
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	LOG_PROCESS_ERROR(hConsole );
    
	bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
	LOG_PROCESS_ERROR(bSuccess);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    /* fill the entire screen with blanks */ 

    bSuccess = FillConsoleOutputCharacter( hConsole, (CHAR) ' ',
       dwConSize, coordScreen, &cCharsWritten );
	LOG_PROCESS_ERROR(bSuccess);

    /* get the current text attribute */ 

    bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
	LOG_PROCESS_ERROR(bSuccess);

    /* now set the buffer's attributes accordingly */ 

    bSuccess = FillConsoleOutputAttribute( hConsole, csbi.wAttributes,
       dwConSize, coordScreen, &cCharsWritten );
	LOG_PROCESS_ERROR(bSuccess);

    /* put the cursor at (0, 0) */ 

    bSuccess = SetConsoleCursorPosition( hConsole, coordScreen );
	LOG_PROCESS_ERROR(bSuccess);

Exit0:
    return;
}

BOOL CGuardServer::_GetLogSubFolders(const char* runDirectory)
{
    m_roLogFolderEntries.clear();
    auto names = guard_files::log_folders(runDirectory);
    if (!names)
    {
        CRI("cannot enumerate log folders: %s", names.error().message().c_str());
        return FALSE;
    }
    m_roLogFolderEntries = std::move(*names);
    return TRUE;
}

BOOL CGuardServer::_PrintProcessContent()
{
	char stSessionContent[256] = { 0 };

	const auto& rList = CProcessMgr::GetInstance().GetProcesses();

	for (auto itr = rList.begin(); itr != rList.end(); ++itr)
	{
		LOG_PROCESS_ERROR(itr->byType >= stMS && itr->byType < stClient);

		BOOL bConnected = FALSE; 
		if (itr->bySessionID > 0)
		{
			bConnected = CGDSessionMgr::GetInstance().CheckSocketConnected(itr->byType, itr->bySessionID);
		}
		const char* szState = bConnected ? "normal" : "unconnect";
		const char* szMode = itr->bAuto ? "auto" : "manul";

		snprintf(stSessionContent, sizeof(stSessionContent), s_szFixLineContent,
			itr->dwProcessID, GetServerShortName(itr->byType), itr->bySessionID, szState, szMode);
		printf("%s", stSessionContent);
	}

	return TRUE;
Exit0:
	return FALSE;
}

extern BOOL g_bStartFromCmd;
BOOL CGuardServer::_UpdateScreen()
{
    INT32 nRetCode = 0;

	if (g_bStartFromCmd)
	{
		Cls();
	}
    printf("%s", s_szFixHeader);
    
    nRetCode = _PrintProcessContent();
    LOG_PROCESS_ERROR(nRetCode);

    printf("%s", s_szFixTail);

    nRetCode = CCmdMgr::GetInstance().PrintCmdDescribe();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = PrintCmdPrompt();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGuardServer::ReloadConfig()
{
    if (DeploymentBusy()) return FALSE;
    // Lua fills the existing POD configuration. Restore it if loading or path
    // validation fails, so callers never observe half of a new configuration.
    struct Rollback
    {
        GUARD_CONFIG previous = g_GuardConfig;
        bool committed = false;
        ~Rollback() { if (!committed) g_GuardConfig = previous; }
    } rollback;
    m_logCleanup.stop();
    m_roLogFolderEntries.clear();
    try
    {
        if (!LoadGuardConfig("guard_config.lua")) return FALSE;
        namespace fs = std::filesystem;
        const auto guardDirectory = fs::current_path();
        const auto runView = std::span{g_GuardConfig.szServerRunPath};
        const auto end = std::ranges::find(runView, '\0');
        if (end == runView.end() || end == runView.begin()) return FALSE;
        const fs::path configuredRun{std::string(runView.begin(), end)};
        const auto runDirectory = fs::absolute(configuredRun).lexically_normal();
        const auto versionDirectory = guardDirectory / "version";
        const auto patchDirectory = guardDirectory / "patch";
        const auto copyPath = [](auto& output, const fs::path& path) {
            const auto value = path.string();
            if (value.size() >= std::size(output)) return false;
            std::ranges::copy(value, output);
            output[value.size()] = '\0';
            return true;
        };
        if (!copyPath(g_GuardConfig.szServerRunPath, runDirectory) ||
            !copyPath(g_GuardConfig.szServerVersionPath, versionDirectory) ||
            !copyPath(g_GuardConfig.szServerPatchPath, patchDirectory) ||
            !copyPath(g_GuardConfig.szGuardPath, guardDirectory)) return FALSE;
        const auto logDirectory = (runDirectory / "log").string();
        if (logDirectory.size() >= PATH_LEN) return FALSE;
        for (const auto& directory : {runDirectory, versionDirectory, patchDirectory})
            fs::create_directories(directory);
        SetDumpFolder(logDirectory.c_str());
        rollback.committed = true;
        INF("g_GuardConfig.szServerRunPath after reload: %s", g_GuardConfig.szServerRunPath);
        return TRUE;
    }
    catch (const std::exception& error)
    {
        CRI("cannot reload guard configuration: %s", error.what());
        return FALSE;
    }
}

BOOL CGuardServer::FetchCurrentVersion(char* output, int length)
{
    if (!output || length <= 0) return FALSE;
    output[0] = '\0';
    try
    {
        auto tag = guard_files::read_tag(std::filesystem::path(g_GuardConfig.szGuardPath) / CURRENT_VERSION_FILE_NAME);
        if (!tag || !*tag || (**tag).size() >= static_cast<std::size_t>(length)) return FALSE;
        std::ranges::copy(**tag, output);
        output[(**tag).size()] = '\0';
        return TRUE;
    }
    catch (const std::exception& error)
    {
        CRI("cannot read current version: %s", error.what());
        return FALSE;
    }
}

BOOL CGuardServer::StartDeployment(bool patch, const char* filename)
try
{
    if (!filename || DeploymentBusy() || CGDMessageHandler::GetInstance().HasIncomingFile()) return FALSE;
    const std::string name{filename};
    if (name.size() > 127 || !guard_files::leaf_name(name) ||
        std::ranges::any_of(name, [](unsigned char value) { return value <= ' '; })) return FALSE;
    if (!patch)
    {
        if (!guard_files::same_tag(std::filesystem::path(name).extension().string(), ".zip") ||
            CProcessMgr::GetInstance().CountRuningProcessByType(stAll) != 0) return FALSE;
        const auto source = std::filesystem::path(g_GuardConfig.szServerVersionPath) / (name + "-guard_area_config.lua");
        const auto destination = std::filesystem::path(g_GuardConfig.szGuardPath) / "guardconfig" / "guard_area_config.lua";
        for (const auto suffix : {"", "-area_config.lua", "-gmsecurityconfig.lua"})
            if (!guard_files::regular_file(std::filesystem::path(g_GuardConfig.szServerVersionPath) / (name + suffix))) return FALSE;
        std::error_code permissionsError;
        if (std::filesystem::exists(destination, permissionsError))
        {
            if (!guard_files::regular_file(destination)) return FALSE;
            std::filesystem::permissions(destination, std::filesystem::perms::owner_write,
                std::filesystem::perm_options::add, permissionsError);
        }
        if (permissionsError) return FALSE;
        if (auto installed = guard_files::copy_file(source, destination, [this] { return ReloadConfig() != FALSE; }); !installed)
        {
            CRI("cannot install guard deployment configuration: %s", installed.error().message().c_str());
            return FALSE;
        }
    }
    guard_deploy::Request request{g_GuardConfig.szGuardPath, g_GuardConfig.szServerVersionPath,
        g_GuardConfig.szServerPatchPath, g_GuardConfig.szServerRunPath, name};
    const auto resultPath = request.guard / (patch ? HOT_UPDATE_PATCH_RESULT_FILE_NAME : DEPLOY_RESULT_FILE_NAME);
    std::error_code error;
    if (std::filesystem::exists(resultPath, error))
    {
        if (!guard_files::regular_file(resultPath) || !std::filesystem::remove(resultPath, error)) return FALSE;
    }
    if (error) return FALSE;
    if (!m_deployment.start(name, [patch, request = std::move(request)](std::stop_token stop) mutable {
        return patch ? guard_deploy::patch(std::move(request), stop) : guard_deploy::version(std::move(request), stop);
    })) return FALSE;
    m_deploymentIsPatch = patch;
    if (patch) SetProcessingPatch(name.c_str());
    else SetProcessingVersion(name.c_str());
    return TRUE;
}
catch (const std::exception& error)
{
    CRI("cannot start deployment worker: %s", error.what());
    return FALSE;
}

BOOL CGuardServer::_ProcessOperationResult(bool patch)
{
    auto* processing = patch ? m_szProcessingPatch : m_szProcessingVersion;
    if (!*processing) return TRUE;
    const std::string requested{processing};
    const auto acknowledge = [patch](INT32 result, const std::string& info) {
        auto& handler = CGDMessageHandler::GetInstance();
        return patch ? handler.DoMTHotUpdatePatchAck(result, info.c_str())
                     : handler.DoMTDeployServerVersionAck(result, info.c_str());
    };
    if (m_deployment.busy() && m_deploymentIsPatch == patch)
    {
        auto completion = m_deployment.poll();
        if (!completion) return TRUE;
        if (!completion->result)
        {
            processing[0] = '\0';
            const auto reason = completion->result.error().message();
            CRI("deployment %s failed: %s", requested.c_str(), reason.c_str());
            GD_ERROR_MT("deployment %s failed: %s", requested.c_str(), reason.c_str());
            acknowledge(errCritical, requested);
            return FALSE;
        }
    }
    INT32 result = errCritical;
    std::string info = requested;
    try
    {
        const auto path = std::filesystem::path(g_GuardConfig.szGuardPath) /
            (patch ? HOT_UPDATE_PATCH_RESULT_FILE_NAME : DEPLOY_RESULT_FILE_NAME);
        auto tag = guard_files::consume_tag(path);
        if (tag && !*tag) return TRUE;
        // A terminal result (including a malformed result) is acknowledged once.
        // Preserve the requested name for patch reload/acknowledgement failures.
        processing[0] = '\0';
        if (!tag)
            CRI("cannot read deployment result: %s", tag.error().message().c_str());
        else
        {
            info = **tag;
            if (!guard_files::same_tag(requested, info))
                result = patch ? errCritical : errUnknownError;
            else
            {
                info = requested;
                LOG_CHECK_ERROR(CGDMessageHandler::GetInstance().DoMTSyncProcessSummary());
                result = patch && !CCmdMgr::GetInstance().ExecOPCmd("reload") ? errGMGSExecute : errSuccess;
            }
        }
    }
    catch (const std::exception& error)
    {
        processing[0] = '\0';
        CRI("cannot process deployment result: %s", error.what());
    }
    const auto sent = acknowledge(result, info);
    return sent && result == errSuccess;
}

BOOL CGuardServer::ProcessDeployServerVersion()
{
    return _ProcessOperationResult(false);
}

BOOL CGuardServer::ProcessHotUpdatePatch()
{
    return _ProcessOperationResult(true);
}

/*

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

    INF("initializing guard server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);

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

    //save globalconfig
    //nRetCode = SaveGlobalConfig("globalconfig");
    //LOG_PROCESS_ERROR(nRetCode);
    return TRUE;
Exit0:
    return FALSE;
}

BOOL _UnInitServer(void)
{
    INT32 nRetCode = 0;

    nRetCode = CCmdMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CProcessMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGDMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);
	
    nRetCode = CGDSessionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGuardServer::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLog::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

INT32 main(INT32 argc, TCHAR** argv) 
{ 
    INT32 nRetCode = 0;
    UINT64 qwCurrentTick = GetTickCountEx();
    INT32 nCmdStartIndex = 1;

#if defined(WIN32)
    nRetCode = StartDumpThread(NULL, TRUE);
    PROCESS_ERROR(nRetCode);
#endif    

    // initialize
	nRetCode = LoadGuardConfig("guard_config.lua");
	LOG_PROCESS_ERROR(nRetCode);

	g_InitStatisInfo(GetCurrentProcessId());

	nRetCode = _InitServer();
    LOG_PROCESS_ERROR(nRetCode);

	// process options
	nCmdStartIndex = ProcessOption(argc, argv);
	LOG_PROCESS_ERROR(nCmdStartIndex >= 1);

    if(argc > nCmdStartIndex)
    {
        char szCmd[COMMON_NAME_LEN] = {0};
        for(INT32 i = nCmdStartIndex; i < argc; ++i)
        {
			strxcat(szCmd, argv[i], sizeof(szCmd));
			strxcat(szCmd, " ", sizeof(szCmd));
        }

		nRetCode = CCmdMgr::GetInstance().ExecOPCmd(szCmd);
		LOG_CHECK_ERROR( nRetCode );
    }

#if (defined(WIN32) || defined(WIN64))
    {
        char szTitle[MAX_PATH];
        snprintf(szTitle, MAX_PATH, "Guard Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
        SetConsoleTitle(szTitle);
        INF(szTitle);
    }
#endif  // WIN32

    // main loop
    while (CGuardServer::GetInstance().IsRuning())
    {
        nRetCode = CGuardServer::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    CGDSessionMgr::GetInstance().CloseAllSocketSession();
    while (CGDSessionMgr::GetInstance().CheckHasSocketSession())
    {
        nRetCode = CGuardServer::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // exit
    INF("Guard is exiting");
Exit0:
	INF("Guard has exited");

	nRetCode = _UnInitServer();
    LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif

    return 0; 
}

*/
