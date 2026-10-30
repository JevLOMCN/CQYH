#include "stdafx.h"
#include "gmserver.h"

#include "global_config.h"
#include "server_config.h"

#include "message_handler.h"
#include "gm_session_mgr.h"
#include "version.h"
#include "script_mgr.h"
#include "time_ex.h"
#include "dump.h"
#include "report.h"
#include "process_option.h"
#include "gm_script_func.h"
#include "user_manager.h"
#include "permission_mgr.h"
#include "ms_mgr.h"
#include "db_mgr.h"
#include "gm_query_thread.h"

#include "kernel.h"
#include "cmd_mgr.h"
#include "gm_role_mgr.h"
#include "game_world.h"
#include "../gameworld/script/script_func.h"
#include "gm_log.h"
#include "statis_info.h"
#include "gm_log_note.h"
#include "order_mgr.h"
#include "cer_message.h"
#include "zlib.h"

#include "ld_listener.h"
#include "order_mgr.h"
#include "gm_version.h"

static char    s_szLuaScript[] =
"cache_list = {};\n" \
"for k, v in pairs(_G) do \n" \
"	if type(v) == 'table' and not getmetatable(v) then \n" \
"		cache_list[k] = {}; \n" \
"		for kk,vv in pairs(v) do \n" \
"			if type(vv) == 'number' and not string.find(string.lower(kk), 'begin', 1, true) and not string.find(string.lower(kk), 'end', 1, true) then \n" \
"				cache_list[k][vv] = kk; \n" \
"			end \n" \
"		end \n" \
"	end \n" \
"end\n" \
"function GetEnumStr(tablename, indexkey) \n" \
"	return cache_list[tablename][indexkey]; \n" \
"end \n" \
"function GetEnumValue(tablename, indexname)\n" \
"	return _G[tablename][indexname];\n" \
"end\n";

class CCertMessageHandler : public INetEventHandler
{
public:
    CCertMessageHandler() { m_pMathTable = NULL; }
    ~CCertMessageHandler() { SAFE_DELETE_ARRAY(m_pMathTable) }

    virtual void OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession) {}
    virtual void OnConnected(ISocketSession* pSocketSession) {}
    virtual void OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
    {
        INT32 nRetCode = 0;
        UINT32 dwDataSize = sizeof(g_MathTable);
        InternalMessage::CER_SYNC_CERTIFICATION* pMsg = (InternalMessage::CER_SYNC_CERTIFICATION*)pPacketBuffer;

        m_pMathTable = new char[dwDataSize];
        LOG_PROCESS_ERROR(m_pMathTable);

        nRetCode = uncompress((Bytef*)m_pMathTable, (uLongf*)&dwDataSize, (const Bytef*)pMsg->szMathTable, dwSize - sizeof(InternalMessage::CER_SYNC_CERTIFICATION));
        LOG_PROCESS_ERROR(nRetCode == Z_OK);

        CGmServer::GetInstance().GetPacketAllocator()->Free(pPacketBuffer);
        return;
    Exit0:
        SAFE_DELETE_ARRAY(m_pMathTable);

        CGmServer::GetInstance().GetPacketAllocator()->Free(pPacketBuffer);
        return;

    }
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bPassiveShutdown) {}

    char*   m_pMathTable;
};

CGmServer   CGmServer::ms_Instance;

//extern int sol_common_open(lua_State* L);
extern int sol_gm_def_open(lua_State* L);
extern int sol_gm_security_open(lua_State* L);
extern int sol_enum_str_open(lua_State* L);

CGmServer::CGmServer()
{
    m_qwGameFrame = 0;
    m_qwStartTick = 0;
    m_dwCurrentTime = 0;
    m_qwCurrentTick = 0;
}

CGmServer::~CGmServer()
{

}

CGmServer &CGmServer::GetInstance()
{
    return ms_Instance;
}

BOOL CGmServer::Init(INetEventHandler *poNetEventHandler)
{
    INT32   nRetCode = 0;
    UINT32  dwAddr;
    char    *pIP = NULL;
    INTRANET_INIT_PARAM IntranetInitParam;

    m_qwGameFrame = 0;
    m_dwCurrentTime = (UINT32)time(NULL);
    m_qwCurrentTick = GetTickCountEx();

    nRetCode = m_PacketAllocator.Init(g_ServerConfig.GM.nSmallPacketCount, g_ServerConfig.GM.nLargePacketCount);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _InitMathTable();
    LOG_PROCESS_ERROR(nRetCode);

#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_ServerConfig.GM.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_ServerConfig.GM.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_ServerConfig.GM.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_ServerConfig.GM.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_ServerConfig.GM.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_ServerConfig.GM.nPacketBufferSize;
    IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nShareMemDisconnectTimeout;
    m_poNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, poNetEventHandler, &IntranetInitParam);
#else
    SimpleInitParam.nMaxAcceptor = g_ServerConfig.GM.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_ServerConfig.GM.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_ServerConfig.GM.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_ServerConfig.GM.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_ServerConfig.GM.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_ServerConfig.GM.nPacketBufferSize;
    m_poNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, poNetEventHandler, &SimpleInitParam);
#endif
    LOG_PROCESS_ERROR(m_poNet);

    //strxcpy(m_dbsetting.szServerAddr, g_ServerConfig.DB.szDBIPAddress, DB_SETTING_STRING_LEN);
    //strxcpy(m_dbsetting.szLoginName, g_ServerConfig.DB.szDBAccount, DB_SETTING_STRING_LEN);
    //strxcpy(m_dbsetting.szPassword, g_ServerConfig.DB.szDBPassword, DB_SETTING_STRING_LEN);
    //strxcpy(m_dbsetting.szDBName, g_ServerConfig.DB.szDBName, DB_SETTING_STRING_LEN);
    //strxcpy(m_dbsetting.szDBNameLogData, g_ServerConfig.DB.szDBName_LogData, DB_SETTING_STRING_LEN);
    //strxcpy(m_dbsetting.szCharSet, g_ServerConfig.DB.szDBCharset, DB_SETTING_STRING_LEN);
    //m_dbsetting.dwServerPort = g_ServerConfig.DB.dwDBPort;

    // todo_tq init db Allan Tao[21.11.2013] 

    //if (g_ServerConfig.DB.bUseIndependentDB)
    //{
    //    char szHostName[256];
    //    char* psz = szHostName;
    //    gethostname(szHostName, 256);

    //    struct hostent *pHost = gethostbyname(szHostName);
    //    LOG_PROCESS_ERROR(pHost);

    //    UINT32  *pInt = (UINT32*)pHost->h_addr_list[0];
    //    LOG_PROCESS_ERROR(pInt);
    //    in_addr tempAddr;
    //    tempAddr.s_addr = *pInt;
    //    pIP = inet_ntoa(tempAddr);
    //}
    //else
    //{
    //    pIP = g_ServerConfig.GM.szAddress;
    //}

    //LOG_PROCESS_ERROR(pIP);
    dwAddr = inet_addr(g_ServerConfig.GM.szAddress);
    LOG_PROCESS_ERROR(dwAddr != INADDR_NONE);

    m_pMSSocketAcceptor = m_poNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pMSSocketAcceptor);

    nRetCode = m_pMSSocketAcceptor->Open(dwAddr, g_ServerConfig.GM.dwMSPort);
    LOG_PROCESS_ERROR(nRetCode);

    INF("opened MS listen port at %s:%d", g_ServerConfig.GM.szAddress, g_ServerConfig.GM.dwMSPort);

    m_pTLSocketAcceptor = m_poNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pTLSocketAcceptor);

    nRetCode = m_pTLSocketAcceptor->Open(dwAddr, g_ServerConfig.GM.dwTLPort);
    LOG_PROCESS_ERROR(nRetCode);

    INF("opened TL listen port at %s:%d", g_ServerConfig.GM.szAddress, g_ServerConfig.GM.dwTLPort);

    nRetCode = m_DBThread.Init(32);
    LOG_PROCESS_ERROR(nRetCode);
    nRetCode = m_DBThread.Start();
    LOG_PROCESS_ERROR(nRetCode);

    m_qwStartTick = GetTickCountEx();

    return TRUE;
Exit0:
    UnInit();
    return FALSE;
}

BOOL CGmServer::UnInit()
{
    INT32 nRetCode = 0;

    if (m_pMSSocketAcceptor)
    {
        nRetCode = m_pMSSocketAcceptor->Close();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_poNet->DestroyAcceptor(m_pMSSocketAcceptor);
        LOG_CHECK_ERROR(nRetCode);
        m_pMSSocketAcceptor = NULL;
    }

    if (m_pTLSocketAcceptor)
    {
        nRetCode = m_pTLSocketAcceptor->Close();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_poNet->DestroyAcceptor(m_pTLSocketAcceptor);
        LOG_CHECK_ERROR(nRetCode);

        m_pTLSocketAcceptor = NULL;
    }

    if (m_poNet)
    {
#if defined(WIN32)
        nRetCode = g_DestroyNetInterface(netIntranet, m_poNet);
#elif defined(__linux__)
        nRetCode = g_DestroyNetInterface(netSimple, m_poNet);
#endif
        LOG_CHECK_ERROR(nRetCode);
        m_poNet = NULL;
    }

    nRetCode = m_PacketAllocator.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CGmServer::MainLoop()
{
    UINT64 qwEndTick{};
    INT32   nRetCode = 0;

    m_dwCurrentTime = (UINT32)time(NULL);
    m_qwCurrentTick = GetTickCountEx();

    if ((m_qwGameFrame * 1000) < (m_qwCurrentTick - m_qwStartTick) * GAME_FPS)
    {
        LOG_PROCESS_ERROR(m_poNet);
        nRetCode = m_poNet->MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CKernel::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);


        nRetCode = COrderMgr::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CDBMgr::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CGMSessionMgr::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        ++m_qwGameFrame;
    }

    qwEndTick = GetTickCountEx();
    if (qwEndTick - m_qwCurrentTick < 100)
        Sleep(1);
    else /*if (qwEndTick - m_qwCurrentTick > 100)*/
        CRI("too slow main loop %lld", qwEndTick - m_qwCurrentTick);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGmServer::_InitMathTable(void)
{
    INT32 nRetCode = 0;
    UINT32 dwAddr;
    INet* pNet = NULL;
    ISocketSession* pCSSession = NULL;
    SIMPLE_INIT_PARAM SimpleInitParam;
    CCertMessageHandler CertMessageHandler;

    SimpleInitParam.nMaxAcceptor = 1;
    SimpleInitParam.nMaxConnector = 1;
    SimpleInitParam.nMaxSessionCount = 1;
    SimpleInitParam.nSendBufSize = 1024 * 1024 * 1;
    SimpleInitParam.nRecvBufSize = 1024 * 1024 * 1;
    SimpleInitParam.nPacketBufferSize = 4;

    pNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, &CertMessageHandler, &SimpleInitParam);
    LOG_PROCESS_ERROR(pNet);

    dwAddr = inet_addr(g_ServerConfig.Common.szCertServerAddr);
    LOG_PROCESS_ERROR(dwAddr != INADDR_NONE);

    pCSSession = pNet->Connect(dwAddr, g_ServerConfig.Common.dwCSPort);
    LOG_PROCESS_ERROR(pCSSession);

    while (CertMessageHandler.m_pMathTable == NULL)
    {
        nRetCode = pNet->MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = InitMathEx(CertMessageHandler.m_pMathTable);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = pCSSession->Close();
    LOG_CHECK_ERROR(nRetCode);

    g_DestroyNetInterface(netSimple, pNet);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGmServer::Close()
{
    INT32       nRetCode = 0;

    nRetCode = CGMSessionMgr::GetInstance().CloseAllSession();
    LOG_CHECK_ERROR(nRetCode);

    while (CGMSessionMgr::GetInstance().CheckSession())
    {
        nRetCode = MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }

    CGMSessionMgr::GetInstance().SetServerState(sgsClosed);
    INF("GM is closed");


    return TRUE;
Exit0:
    return FALSE;
}

#ifdef _WIN32
void _signal_handler(int sig)
{
    signal(SIGINT, _signal_handler);
    signal(SIGBREAK, _signal_handler);
}
#endif  // _WIN32

BOOL _InitGMScript()
{
    INT32       nRetCode = 0;
    //CLuaScript  *poScript = NULL;
    lua_State*  L;

    nRetCode = CScriptMgr::GetInstance().RegisterFuncList(g_GMLuaScriptFuncTable);
    LOG_PROCESS_ERROR(nRetCode);

    //nRetCode = CScriptMgr::GetInstance().RegisterFuncList(g_LuaScriptFuncTable);
    //LOG_PROCESS_ERROR(nRetCode);

    //poScript = CScriptMgr::GetInstance().NewScript(GM_PERMISSION_NAME);
    //LOG_PROCESS_ERROR(poScript);

    //nRetCode = CScriptMgr::GetInstance().RegisterPackage("common.pkg", sol_common_open);
    //LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("gm_def.pkg", sol_gm_def_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CGmServer::GetInstance().GetLuaScript()->Init();
    LOG_PROCESS_ERROR(nRetCode);

    L = CGmServer::GetInstance().GetLuaScript()->GetLuaState();
    LOG_PROCESS_ERROR(L);

    nRetCode = sol_enum_str_open(L);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CGmServer::GetInstance().GetLuaScript()->LoadFromBuffer("EnumStr", s_szLuaScript, sizeof(s_szLuaScript) - 1);;
    LOG_PROCESS_ERROR(nRetCode);
    
    INF("initialized all scripts successfully");

    return TRUE;
Exit0:
    return FALSE;
}

const char *CGmServer::GetErrString(INT32 nErr)
{
    return GetEnumString("ERROR_CODE_DEF", nErr);
}

const char * CGmServer::GetEnumString(const char *pcszEnumName, INT32 nEnumKeyValue)
{
    const INT32 NAME_LENGTH = 256;
    static char szName[NAME_LENGTH];
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pcszEnumName);
    memset(szName, 0, NAME_LENGTH);

    nRetCode = m_LuaScript.CallFunction("GetEnumStr", "sd:s", pcszEnumName, nEnumKeyValue, szName, NAME_LENGTH);
    LOG_PROCESS_ERROR(nRetCode);

    return szName;
Exit0:
    if (pcszEnumName)
        ERR("GetEnumString failed, enum name[%s]", pcszEnumName);
    return szName;
}

BOOL _InitServer()
{
    INT32       nRetCode = 0;
    char szFilePath[PATH_LEN];
    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", "commonconfig", SETTING_FOLDER_NAME);

    CMessageHandler     &rMessageHandler = CMessageHandler::GetInstance();
    CGmServer           &rGMServer = CGmServer::GetInstance();

    nRetCode = CLog::GetInstance().Init("gm_server", 
        g_ServerConfig.GM.nLogLevelMask,
        g_ServerConfig.GM.nLogOutputMask,
        g_ServerConfig.GM.bEnableLogThread,
        g_ServerConfig.GM.bPrintThreadID,
        g_ServerConfig.GM.bFlushFile);
	LOG_PROCESS_ERROR(nRetCode);

	g_InitReportSetting(GetServerShortName(stGM), 1, g_ServerConfig.Common.szReportAbsolutePath);

    nRetCode = CGMLog::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    INF("initializing gm server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
    INF("gm version %d.%d.%d", GM_DEPEND_VERSION, GM_MAJOR_VERSION, GM_MINOR_VERSION);

    nRetCode = rGMServer.Init(&rMessageHandler);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CGMSessionMgr::GetInstance().Init(rGMServer.GetNetInterface());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = rMessageHandler.Init(rGMServer.GetNetInterface(), rGMServer.GetPacketAllocator(),
        rGMServer.GetMSSocketAcceptor(), rGMServer.GetTLSocketAcceptor());
    LOG_PROCESS_ERROR(nRetCode);

    //nRetCode = CScriptMgr::GetInstance().Init(g_ServerConfig.GM.szConfigPath);
    //LOG_PROCESS_ERROR(nRetCode);

    //nRetCode =  g_InitGameWorldLibConfig();
    //LOG_PROCESS_ERROR(nRetCode);

    nRetCode = g_InitGameWorldLib(0, rGMServer.GetNotifier(), rGMServer.GetPacketAllocator());
    LOG_PROCESS_ERROR(nRetCode);

    g_GMLogMap.clear();

    nRetCode = CDataManager<GM_LOG_NOTE>::GetInstance().Init(GM_LOG_NOTE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    {
        const GM_LOG_NOTE *pGMNote = CDataManager<GM_LOG_NOTE>::GetInstance().GetFirstData();
        while (pGMNote)
        {
            g_GMLogMap.insert(std::make_pair(pGMNote->szLog, pGMNote->szNote));

            pGMNote = CDataManager<GM_LOG_NOTE>::GetInstance().GetNextData();
        }
    }

    nRetCode = _InitGMScript();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CPermissionMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CUserManager::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDBMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CGMRoleMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CLDListener::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = COrderMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

#ifdef _WIN32
    signal(SIGINT, _signal_handler);
    signal(SIGBREAK, _signal_handler);
#endif  // _WIN32

    CGMSessionMgr::GetInstance().SetServerState(sgsSelfReady);

	for (INT32 nIndex = 1; nIndex < sizeof(g_ServerConfig.Common.IpWhiteList) / sizeof(g_ServerConfig.Common.IpWhiteList[0]); ++nIndex)
    {
		if (g_ServerConfig.Common.IpWhiteList[nIndex].szIP[0] == 0)
            break;

		UINT32 dwWhiteAddr = inet_addr(g_ServerConfig.Common.IpWhiteList[nIndex].szIP);
        if (dwWhiteAddr == INADDR_NONE)
            break;
		UINT32 dwWhiteMask = inet_addr(g_ServerConfig.Common.IpWhiteList[nIndex].szMask);
        
		INF("WHITE LIST CHECK! IP:[%s][%d], mask:[%s][%d]", g_ServerConfig.Common.IpWhiteList[nIndex].szIP,
			dwWhiteAddr, g_ServerConfig.Common.IpWhiteList[nIndex].szMask, dwWhiteMask);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL _UnInitServer()
{
    INT32       nRetCode = 0;

    nRetCode = CLDListener::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = COrderMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CUserManager::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CPermissionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGMSessionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGmServer::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDBMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGMRoleMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GM_LOG_NOTE>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGMLog::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = g_ClosePakFolder();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLog::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);
    
    return TRUE;
Exit0:
    return FALSE;
}

INT32 main(INT32 argc, TCHAR** argv)
{
    INT32       nRetCode = 0;

#if defined(WIN32)
	nRetCode = StartDumpThread(NULL, FALSE, "log");
    PROCESS_ERROR(nRetCode);

    SetWindowPos(GetConsoleWindow(), HWND_TOP, 800, 0, 0, 0, SWP_NOSIZE);
#endif

    nRetCode = g_OpenPakFolder(DEFAULT_PAK_PATH);
    if (nRetCode)
        INF("open pak folder %s successfully", DEFAULT_PAK_PATH);
    else
        INF("cannot open pak folder %s", DEFAULT_PAK_PATH);

    nRetCode = LoadGlobalConfig(g_szGlobalConfigFileName);
    LOG_PROCESS_ERROR(nRetCode);

#ifdef _FINAL
    // decrypt
    for (INT32 i = 0; i < MAX_GM_USER; ++i)
    {
        if (g_ServerConfig.GM_PWD.gmUserList[i].dwUserID == 0)
            continue;

        char    *pszPwd = g_ServerConfig.GM_PWD.gmUserList[i].szPwd;

        nRetCode = SimpleDecrypt(pszPwd, strlen(pszPwd));
        LOG_PROCESS_ERROR(nRetCode);
    }
    
    for (INT32 i = 0; i < MAX_GM_USER; ++i)
    {
        if (g_ServerConfig.GM_PWD.LDUserList[i].dwUserID == 0)
            continue;
        char    *pszPwd = g_ServerConfig.GM_PWD.LDUserList[i].szPwd;
        nRetCode = SimpleDecrypt(pszPwd, strlen(pszPwd));
        LOG_PROCESS_ERROR(nRetCode);
    }

#endif

    //Sleep(1000 * 10);

	nRetCode = g_InitStatisInfo(GetCurrentProcessId());
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _InitServer();
    LOG_PROCESS_ERROR(nRetCode);

#if (defined(WIN32) || defined(WIN64))
    {
        char szTitle[MAX_PATH];
        snprintf(szTitle, MAX_PATH, "Gm Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
        SetConsoleTitle(szTitle);
        INF(szTitle);
    }
#endif  // WIN32

    while (CGMSessionMgr::GetInstance().GetServerState() != sgsClosing)
    {
        __try
        {
            nRetCode = CGmServer::GetInstance().MainLoop();
            LOG_PROCESS_ERROR(nRetCode);
        }
        __except (DumpExceptionFilter(GetExceptionInformation()))
        {
        }

        Sleep(1);
    }

    INF("GM has exiting");

    nRetCode = CGmServer::GetInstance().Close();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    INF("GM has exited");

    nRetCode = _UnInitServer();
    LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif 

    return 0;
}
