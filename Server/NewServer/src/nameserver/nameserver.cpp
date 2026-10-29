#include "stdafx.h"
#include "nameserver.h"
#include "worker_queue.h"
#include "ns_message_handler.h"

#include "dump.h"
#include "ifile.h"

#include "global_config.h"
#include "server_config.h"
#include "process_option.h"
#include "version.h"
#include "statis_info.h"

#include "role_db_def.h"
#include "mysql_owner.h"

const char* g_szTableName[ntTotal] =
{
    "role",
    "guild",
};

// Length (ROLE_NAME_LEN) (GUILD_NAME_LEN)
const char* g_szCreateTable[ntTotal] =
{
    "CREATE TABLE IF NOT EXISTS `role` ( `RoleID` int(4) unsigned NOT NULL AUTO_INCREMENT, `RoleName` varchar(25) NOT NULL, PRIMARY KEY(`RoleID`) ) ENGINE = MyISAM DEFAULT CHARSET = utf8 ",
    "CREATE TABLE IF NOT EXISTS `guild` ( `GuildID` int(4) unsigned NOT NULL AUTO_INCREMENT, `GuildName` varchar(25) NOT NULL, PRIMARY KEY(`GuildID`) ) ENGINE = MyISAM DEFAULT CHARSET = utf8 ",
};

CNameServer CNameServer::ms_Instance;

CNameServer& CNameServer::GetInstance(void)
{
    return ms_Instance;
}

CNameServer::CNameServer(void)
{
    m_sql = cqyh::MysqlPtr{};
    m_bDBReady = FALSE;
}

CNameServer::~CNameServer(void)
{
    UnInit();
}

void CNameServer::ThrdProc(void)
{
    cqyh::MysqlThreadContext clientThread;
    if (!clientThread)
    {
        LOG_CHECK_ERROR(clientThread);
        return;
    }
    struct ClearConnection
    {
        cqyh::MysqlPtr& connection;
        ~ClearConnection() { connection = cqyh::MysqlPtr{}; }
    } clear{m_sql};
    INT32 nRetCode = 0;
    UINT32 dwLastPingTime = (UINT32)time(NULL);

    WRN("ThrdProc 111");

    while (m_bstart)
    {
        UINT32 dwNow = 0;
        //WRN("ThrdProc 222");

        if (!m_bDBReady)
        {
            WRN("ThrdProc 333");

            nRetCode = _ThrdInitDB();
            if (!nRetCode)
                goto LOOP_ERROR;

            WRN("ThrdProc 444");
            m_bDBReady.store(true);
            WRN("ThrdProc 555");
        }

        //WRN("ThrdProc 666");
        dwNow = (UINT32)time(NULL);

        if (dwNow - dwLastPingTime >= 10)
        {
            nRetCode = mysql_ping(m_sql.get());
            LOG_CHECK_ERROR(nRetCode == 0);

            dwLastPingTime = dwNow;
        }

        _ThrdWork();


        WaitForStop(std::chrono::milliseconds(1));
        continue;

    LOOP_ERROR:
        WaitForStop(std::chrono::seconds(1));
    }

    return;
}

void CNameServer::_ThrdWork(void)
{
    cqyh::MysqlResultPtr result;
    MYSQL_ROW row = nullptr;
    if (m_DBTaskList.IsEmpty())
        return;

    DBQueryData* pQuery = NULL;
    char szSql[1024] = { 0 };
    UINT32 dwRetID = 0;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(m_sql.get());

    pQuery = (DBQueryData*)m_DBTaskList.Check();
    LOG_PROCESS_ERROR(pQuery);
    pQuery->nErrCode = errNsNameDBSysErr;
    LOG_PROCESS_ERROR(pQuery->eType > ntInvalid && pQuery->eType < ntTotal);

    // Default code
    //sprintf(szSql, "insert into %s (name) values ('%s');", g_szTableName[pQuery->eType], pQuery->szName);
    //nRetCode = mysql_query(m_sql.get(), szSql);
    //LOG_PROCESS_ERROR(nRetCode == 0);
    //dwRetID = mysql_insert_id(m_sql.get());
    //LOG_PROCESS_ERROR(dwRetID > 0);

    // Custom added code
    
    // Prepare the SQL statement for the type [character name/guild name]
    switch (pQuery->eType)
    {
        case 0:
            //sprintf(szSql, "insert into %s (RoleName) values ('%s');", g_szTableName[pQuery->eType], pQuery->szName);
            sprintf(szSql, "SELECT MAX(RoleID) AS MaxRoleID FROM %s;", g_szTableName[pQuery->eType]);
            break;
        case 1:
            //sprintf(szSql, "insert into %s (GuildName) values ('%s');", g_szTableName[pQuery->eType], pQuery->szName);
            sprintf(szSql, "SELECT MAX(GuildID) AS MaxGuildID FROM %s;", g_szTableName[pQuery->eType]);
            break;
        default:
            LOG_PROCESS_ERROR(false);
    }

    // Execute the SQL statement
    nRetCode = mysql_query(m_sql.get(), szSql);
    LOG_PROCESS_ERROR(nRetCode == 0);

    // Get the result set
    result.reset(mysql_store_result(m_sql.get()));
    LOG_PROCESS_ERROR(result);

    // Read the result
    row = mysql_fetch_row(result.get());
    LOG_PROCESS_ERROR(row);

    // If the table already contains data, get the value directly
    if (*row)
    {
        // Current maximum ID + 1
        dwRetID = (UINT32)atoi(row[0]) + 1;
    }
    // Reading row[0] on an empty table would crash, so check first
    else
    {
        // Prepare an SQL statement to count table rows
        char szSqlTotalRows[1024] = { 0 };
        sprintf(szSqlTotalRows, "SELECT COUNT(*) AS TotalRows FROM %s;", g_szTableName[pQuery->eType]);

        // Execute the SQL statement
        nRetCode = mysql_query(m_sql.get(), szSqlTotalRows);
        LOG_PROCESS_ERROR(nRetCode == 0);

        // Get the result set
        cqyh::MysqlResultPtr resultTotalRows(mysql_store_result(m_sql.get()));
        LOG_PROCESS_ERROR(resultTotalRows);

        // Read the result
        MYSQL_ROW rowTotalRows = mysql_fetch_row(resultTotalRows.get());
        LOG_PROCESS_ERROR(rowTotalRows);

        // If the table is empty, this is the first record
        if (atoi(rowTotalRows[0]) == 0)
        {
            dwRetID = 1;
        }

        // Release the result set
        resultTotalRows.reset();
    }

    // Release the result set
    result.reset();

    pQuery->nErrCode = errNsSuccess;

    if (pQuery->eType == ntRoleName)
        //pQuery->dwNewID = MakeObjectID(otRole, dwRetID);
        pQuery->dwNewID = MakeObjectID(otRole, dwRetID);
    else if (pQuery->eType == ntGuildName)
        //pQuery->dwNewID = MakeObjectID(otGuild, dwRetID);
        pQuery->dwNewID = MakeObjectID(otGuild, dwRetID);

Exit0:
    result.reset();
    if (!pQuery)
    {
        LOG_CHECK_ERROR(FALSE);
        return;
    }

    if (pQuery->nErrCode == errNsNameDBSysErr)
    {
        if (pQuery->eType > ntInvalid && pQuery->eType < ntTotal)
            WRN("DB Execute Error, table: %s, name: %s", g_szTableName[pQuery->eType], pQuery->szName);
        else
            WRN("DB Execute Error, invalid name type %d", pQuery->eType);

        if (m_sql.get())
        {
            WRN("mysql error info: %s", mysql_error(m_sql.get()));
        }

        m_sql = cqyh::MysqlPtr{};

        m_bDBReady.store(false);
    }
    // No query access after publication: the main thread may recycle it immediately.
    if (cqyh::PublishUntilStopped(m_ResutList, pQuery, *this))
        m_DBTaskList.Pop();

}


BOOL CNameServer::_TryCreateDB(void)
{
    INT32 nRetCode = 0;
    cqyh::MysqlPtr sql;
    MYSQL* conn_sql = NULL;

    sql = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(sql);

    conn_sql = mysql_real_connect(sql.get(), g_NSConfig.db_config.ip, g_NSConfig.db_config.account,
        g_NSConfig.db_config.password, NULL, g_NSConfig.db_config.port, NULL, 0);

    WRN("_TryCreateDB - g_NSConfig.db_config.db_name %s", g_NSConfig.db_config.db_name);
    WRN("_TryCreateDB - g_NSConfig.db_config.ip %s", g_NSConfig.db_config.ip);
    WRN("_TryCreateDB - g_NSConfig.db_config.account %s", g_NSConfig.db_config.account);
    WRN("_TryCreateDB - g_NSConfig.db_config.password %s", g_NSConfig.db_config.password);
    WRN("_TryCreateDB - g_NSConfig.db_config.port %d", g_NSConfig.db_config.port);

    WRN("_TryCreateDB - 000");
    LOG_PROCESS_ERROR(conn_sql);
    if (!_ExistDB(sql.get(), g_NSConfig.db_config.db_name))
    {
        WRN("_TryCreateDB - 222");
        char db_sql[128] = { 0 };

        snprintf(db_sql, sizeof(db_sql) - 1, "CREATE DATABASE %s;", g_NSConfig.db_config.db_name);

        nRetCode = mysql_query(sql.get(), db_sql);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    sql.reset();

    WRN("_TryCreateDB - 333");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNameServer::Init(void)
{
    if (m_sql || m_bstart)
    {
        LOG_CHECK_ERROR(!m_sql && !m_bstart);
        return FALSE;
    }
    INT32 nRetCode = 0;
    MYSQL* ret_sql = NULL;

    m_bDBReady = FALSE;

    nRetCode = m_DBQueryPool.Init(g_NSConfig.db_config.pool_size);
    LOG_PROCESS_ERROR(nRetCode);

    //	nRetCode = m_DBResultPool.Init(g_NSConfig.db_config.pool_size);
    //	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_DBTaskList.Init(g_NSConfig.db_config.pool_size);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_ResutList.Init(g_NSConfig.db_config.pool_size);
    LOG_PROCESS_ERROR(nRetCode);

    WRN("111 CNameServer::Init - nRetCode %d", nRetCode);
    nRetCode = _TryCreateDB();
    LOG_PROCESS_ERROR(nRetCode);
    WRN("222 CNameServer::Init - nRetCode %d", nRetCode);

    WRN("333 CNameServer::Init - nRetCode %d", nRetCode);
    nRetCode = Start();
    LOG_PROCESS_ERROR(nRetCode);
    WRN("444 CNameServer::Init - nRetCode %d", nRetCode);

    return TRUE;
Exit0:
    WRN("555 CNameServer::Init - nRetCode %d", nRetCode);
    if (m_sql)
        CRI("mysql error : %s", mysql_error(m_sql.get()));
    UnInit();
    return FALSE;
}

void CNameServer::UnInit(void)
{
    Wait();

    while (auto* query = static_cast<DBQueryData*>(m_DBTaskList.Pop()))
        m_DBQueryPool.Delete(query);
    while (auto* query = static_cast<DBQueryData*>(m_ResutList.Pop()))
        m_DBQueryPool.Delete(query);

    m_DBTaskList.UnInit();
    m_ResutList.UnInit();

    m_DBQueryPool.UnInit();
    //	m_DBResultPool.UnInit();

    m_sql = cqyh::MysqlPtr{};
}

BOOL CNameServer::_LoadData(void)
{
    INT32 nRetCode = 0;

    cqyh::MysqlResultPtr result;
    MYSQL_ROW row = NULL;

    LOG_PROCESS_ERROR(m_sql.get());

    for (INT32 i = ntBegin; i < ntTotal; i++)
    {
        m_NameMap[i].clear();
        m_LockNameSet[i].clear();
    }

    for (INT32 i = ntBegin; i < ntTotal; i++)
    {
        nRetCode = mysql_query(m_sql.get(), g_szCreateTable[i]);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    for (INT32 i = ntBegin; i < ntTotal; i++)
    {
        char szSql[1024] = { 0 };

        // Default code
        //sprintf(szSql, "select id, name from %s;", g_szTableName[i]);

        // Query the character or guild table; field names differ
        switch (i)
        {
            case ntRoleName:
                sprintf(szSql, "select RoleID, RoleName from %s;", g_szTableName[i]);
                break;
            case ntGuildName:
                sprintf(szSql, "select GuildID, GuildName from %s;", g_szTableName[i]);
                break;
        }

        nRetCode = mysql_query(m_sql.get(), szSql);
        LOG_PROCESS_ERROR(nRetCode == 0);

        result.reset(mysql_store_result(m_sql.get()));
        LOG_PROCESS_ERROR(result);

        while (row = mysql_fetch_row(result.get()))
        {
            UINT32 id = (UINT32)atoi(row[0]);
            char* name = row[1];

            nRetCode = m_NameMap[i].insert(std::make_pair(name, id)).second;
            LOG_PROCESS_ERROR(nRetCode);
        }

        DBG("----- CNameServer::_LoadData ----- LoadDataCount %d", m_NameMap->size());

        result.reset();
    }

    return TRUE;
Exit0:
    return FALSE;
}

// Add new data and check for duplicates
INT32 CNameServer::AddNewData(INT32 eType, INT32 nSessionID, UINT32 dwCliSessionID, const char* szName, const char* UserData)
{
    INT32 nResult = errNsNameUnknown;
    INT32 nRetCode = 0;
    DBQueryData* pNewQuery = NULL;

    LOG_PROCESS_ERROR_RET_CODE(m_bDBReady && m_bstart.load() && IsRunning(), errNsNameDBSysErr);

    LOG_PROCESS_ERROR(eType > ntInvalid && eType < ntTotal);
    LOG_PROCESS_ERROR(strlen(szName) > 0);

    // Check existing names for duplicates
    {
        TNameMap::iterator itr = m_NameMap[eType].find(szName);
        PROCESS_ERROR_RET_CODE(itr == m_NameMap[eType].end(), errNsNameDuplicated);
    }

    // Check locked names for duplicates
    {
        TLockNameSet::iterator itr = m_LockNameSet[eType].find(szName);
        PROCESS_ERROR_RET_CODE(itr == m_LockNameSet[eType].end(), errNsNameDuplicated);
    }

    pNewQuery = m_DBQueryPool.New();
    LOG_PROCESS_ERROR(pNewQuery);

    memset(pNewQuery, 0, sizeof(DBQueryData));

    pNewQuery->eType = eType;
    pNewQuery->nSessionID = nSessionID;
    pNewQuery->dwCliSessionID = dwCliSessionID;
    strncpy(pNewQuery->szName, szName, COMMON_NAME_LEN - 1);
    memcpy(pNewQuery->UserData, UserData, USER_DATA_LEN);

    nRetCode = m_DBTaskList.Push(pNewQuery);
    LOG_PROCESS_ERROR(nRetCode);

    m_LockNameSet[eType].insert(szName);

    return errNsSuccess;
Exit0:
    if (pNewQuery)
        m_DBQueryPool.Delete(pNewQuery);

    return nResult;
}

BOOL CNameServer::MainLoop(void)
{
    INT32 nRetCode = 0;
    DBQueryData* pResult = NULL;

    while (!m_ResutList.IsEmpty())
    {
        pResult = (DBQueryData*)m_ResutList.Pop();
        if (!pResult || !(pResult->eType > ntInvalid && pResult->eType < ntTotal))
            goto LOOP_ERROR;

        nRetCode = m_NameMap[pResult->eType].insert(std::make_pair(pResult->szName, pResult->dwNewID)).second;
        if (!nRetCode)
            goto LOOP_ERROR;

        m_LockNameSet[pResult->eType].erase(pResult->szName);

        INF("--- create name success: [%s], type: [%s]", pResult->szName, g_szTableName[pResult->eType]);

    LOOP_ERROR:
        if (pResult)
        {
            CNSMessageHandler::GetInstance().DoNewNameResult(pResult->nSessionID, pResult->dwCliSessionID, pResult->nErrCode, pResult->eType, pResult->dwNewID, pResult->szName, pResult->UserData);
            m_DBQueryPool.Delete(pResult);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNameServer::_ExistDB(MYSQL* mysql, const char* dbname)
{
    WRN("_ExistDB - 111");
    BOOL found = FALSE;
    cqyh::MysqlResultPtr mysql_result(mysql_list_dbs(mysql, dbname));

    LOG_PROCESS_ERROR(mysql_result);

    while (mysql_fetch_row(mysql_result.get()))
    {
        found = TRUE;
    }

Exit0:
    return found;
}

BOOL CNameServer::_ThrdInitDB(void)
{
    INT32 nRetCode = 0;
    MYSQL* ret_sql = NULL;

    m_sql = cqyh::MysqlPtr{};

    m_sql = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(m_sql.get());

    ret_sql = mysql_real_connect(m_sql.get(), g_NSConfig.db_config.ip, g_NSConfig.db_config.account,
        g_NSConfig.db_config.password, g_NSConfig.db_config.db_name, g_NSConfig.db_config.port, NULL, 0);

    WRN("_ThrdInitDB - g_NSConfig.db_config.db_name %s", g_NSConfig.db_config.db_name);
    WRN("_ThrdInitDB - g_NSConfig.db_config.ip %s", g_NSConfig.db_config.ip);
    WRN("_ThrdInitDB - g_NSConfig.db_config.account %s", g_NSConfig.db_config.account);
    WRN("_ThrdInitDB - g_NSConfig.db_config.password %s", g_NSConfig.db_config.password);
    WRN("_ThrdInitDB - g_NSConfig.db_config.port %d", g_NSConfig.db_config.port);

    LOG_PROCESS_ERROR(ret_sql);

    WRN("_ThrdInitDB - 000");
    if (strlen(g_NSConfig.db_config.charset) > 0)
    {
        WRN("_ThrdInitDB - 111");
        nRetCode = mysql_set_character_set(m_sql.get(), g_NSConfig.db_config.charset);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    WRN("_ThrdInitDB - 222");
    nRetCode = _LoadData();
    LOG_PROCESS_ERROR(nRetCode);
    WRN("_ThrdInitDB - 333, nRetCode [%d]", nRetCode);

    INF("DB Load Finished");

    return TRUE;
Exit0:

    m_sql = cqyh::MysqlPtr{};

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BOOL Install(void);
BOOL Uninstall(void);
void WINAPI ServiceMain(void);
void WINAPI ServiceCtrlHandler(DWORD dwOpcode);
char g_szServiceName[] = "name server";
BOOL g_bStop = FALSE;
SERVICE_STATUS_HANDLE g_hServiceStatus;
SERVICE_STATUS g_status;
NS_CONFIG g_NSConfig;
char g_szConfigName[] = { 0 };    // Path to nameserver.ini

// Initialize the configuration path
void InitConfigPath() 
{
    char szExePath[MAX_PATH] = { 0 };

    // Get the executable’s absolute path (e.g. E:\cqyh-new2\NewServer\bin\nameserver64_d.exe)
    GetModuleFileName(NULL, szExePath, MAX_PATH);

    // Extract the executable directory (E:\cqyh-new2\NewServer\bin\)
    char* pLastSlash = strrchr(szExePath, '\\');
    if (pLastSlash != NULL) {
        *pLastSlash = '\0'; // Remove the file name, retaining the directory
    }

    // Construct the full .ini file path
    snprintf(g_szConfigName, MAX_PATH, "%s\\nameserver.ini", szExePath);
}

// The path is correct but data cannot be read here, so a default value is set
void ReadConfig(void)
{
    ::GetPrivateProfileString("net", "ip", "127.0.0.1", g_NSConfig.net_config.ip, COMMON_NAME_LEN, g_szConfigName);
    g_NSConfig.net_config.port = ::GetPrivateProfileInt("net", "port", 8909, g_szConfigName);
    g_NSConfig.net_config.max_acceptor = ::GetPrivateProfileInt("net", "max_acceptor", 32, g_szConfigName);
    g_NSConfig.net_config.max_connector = ::GetPrivateProfileInt("net", "max_connector", 32, g_szConfigName);
    g_NSConfig.net_config.max_session = ::GetPrivateProfileInt("net", "max_session", 64, g_szConfigName);
    g_NSConfig.net_config.send_buf_size = ::GetPrivateProfileInt("net", "send_buf_size", 1048576, g_szConfigName);
    g_NSConfig.net_config.recv_buf_size = ::GetPrivateProfileInt("net", "recv_buf_size", 1048576, g_szConfigName);
    g_NSConfig.net_config.packet_buf_size = ::GetPrivateProfileInt("net", "packet_buf_size", 1024, g_szConfigName);
    g_NSConfig.net_config.small_packet_count = ::GetPrivateProfileInt("net", "small_packet_count", 2048, g_szConfigName);
    g_NSConfig.net_config.large_packet_count = ::GetPrivateProfileInt("net", "large_packet_count", 128, g_szConfigName);
    g_NSConfig.net_config.sharemem_disconnect_timeout = ::GetPrivateProfileInt("net", "sharemem_disconnect_timeout", 5000, g_szConfigName);
    g_NSConfig.net_config.ping_interval = ::GetPrivateProfileInt("net", "ping_interval", 1000, g_szConfigName);
    g_NSConfig.net_config.ping_timeout = ::GetPrivateProfileInt("net", "ping_timeout", 5000, g_szConfigName);

    ::GetPrivateProfileString("db", "ip", "127.0.0.1", g_NSConfig.db_config.ip, COMMON_NAME_LEN, g_szConfigName);
    ::GetPrivateProfileString("db", "account", "root", g_NSConfig.db_config.account, COMMON_NAME_LEN, g_szConfigName);
    ::GetPrivateProfileString("db", "password", "jzh53222197", g_NSConfig.db_config.password, COMMON_NAME_LEN, g_szConfigName);
    ::GetPrivateProfileString("db", "db_name", "cqyh_name", g_NSConfig.db_config.db_name, COMMON_NAME_LEN, g_szConfigName);
    ::GetPrivateProfileString("db", "charset", "utf8", g_NSConfig.db_config.charset, COMMON_NAME_LEN, g_szConfigName);

    g_NSConfig.db_config.port = ::GetPrivateProfileInt("db", "port", 3306, g_szConfigName);
    g_NSConfig.db_config.pool_size = ::GetPrivateProfileInt("db", "pool_size", 1024, g_szConfigName);
}

void Init()
{
    g_hServiceStatus = NULL;
    g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_status.dwCurrentState = SERVICE_STOPPED;
    g_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_status.dwWin32ExitCode = 0;
    g_status.dwServiceSpecificExitCode = 0;
    g_status.dwCheckPoint = 0;
    g_status.dwWaitHint = 0;
}

BOOL Install(void)
{
    INT32 nRetCode = 0;
    SC_HANDLE hSCM = NULL;
    SC_HANDLE hService = NULL;
    char szFilePath[MAX_PATH];

    nRetCode = GetModuleFileName(NULL, szFilePath, MAX_PATH);
    LOG_PROCESS_ERROR(nRetCode);

    hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    LOG_PROCESS_ERROR(hSCM);

    hService = CreateService(hSCM, g_szServiceName, g_szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, "", NULL, NULL);
    LOG_PROCESS_ERROR(hService);

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);

    return TRUE;
Exit0:
    if (hService)
        CloseServiceHandle(hService);
    if (hSCM)
        CloseServiceHandle(hSCM);
    return FALSE;
}

BOOL Uninstall()
{
    INT32 nRetCode = 0;
    SC_HANDLE hSCM = NULL;
    SC_HANDLE hService = NULL;

    hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    LOG_PROCESS_ERROR(hSCM);

    hService = OpenService(hSCM, g_szServiceName, SERVICE_STOP | DELETE);
    LOG_PROCESS_ERROR(hService);

    nRetCode = ControlService(hService, SERVICE_CONTROL_STOP, &g_status);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DeleteService(hService);
    LOG_PROCESS_ERROR(nRetCode);

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);

    return TRUE;
Exit0:
    if (hService)
        CloseServiceHandle(hService);
    if (hSCM)
        CloseServiceHandle(hSCM);

    return FALSE;
}

void WINAPI ServiceCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl)
    {
        case SERVICE_CONTROL_STOP:
            g_bStop = TRUE;
            break;
        default:
            break;
    }
}


BOOL _InitServer(void)
{
    INT32 nRetCode = 0;

    nRetCode = CNameServer::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);


    nRetCode = CNSMessageHandler::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL _UnInitServer(void)
{
    INT32 nRetCode = 0;

    CNSMessageHandler::GetInstance().UnInit();

    CNameServer::GetInstance().UnInit();

    return TRUE;
}

//#ifndef _DEBUG
//void ServiceMain(void)
//{
//	INT32 nRetCode = 0;
//
//	Sleep(10000);
//
//	nRetCode = CLog::GetInstance().Init("name_server",
//		g_ServerConfig.NS.Log.nLogLevelMask,
//		g_ServerConfig.NS.Log.nLogOutputMask,
//		g_ServerConfig.NS.Log.bEnableLogThread,
//		g_ServerConfig.NS.Log.bPrintThreadID,
//		g_ServerConfig.NS.Log.bFlushFile);
//	LOG_PROCESS_ERROR(nRetCode);
//
//	{
//		char dir[MAX_PATH] = { 0 };
//		char new_dir[MAX_PATH] = { 0 };
//
//		::GetModuleFileName(NULL, dir, 1024);
//
//		for (INT32 i = strlen(dir) - 1; i > 0; i--)
//		{
//			if (dir[i] == '\\' || dir[i] == '/')
//			{
//				strncpy(new_dir, dir, i);
//				break;
//			}
//		}
//
//		nRetCode = ::SetCurrentDirectory(new_dir);
//		if (!nRetCode)
//		{
//			INT32 nError = ::GetLastError();
//		}
//	}
//
//#if defined(WIN32) | defined(WIN64)
//	nRetCode = StartDumpThread(NULL, FALSE);
//	PROCESS_ERROR(nRetCode);
//
//	//	SetWindowPos(GetConsoleWindow(), HWND_TOP, 1940, 455, 0, 0, SWP_NOSIZE);
//#endif
//	nRetCode = g_OpenPakFolder(DEFAULT_PAK_PATH);
//	if (nRetCode)
//		INF("open pak folder %s successfully", DEFAULT_PAK_PATH);
//	else
//		INF("cannot open pak folder %s", DEFAULT_PAK_PATH);
//
//	g_hServiceStatus = RegisterServiceCtrlHandler(g_szServiceName, ServiceCtrlHandler);
//	LOG_PROCESS_ERROR(g_hServiceStatus);
//
//	g_status.dwCurrentState = SERVICE_START_PENDING;
//	g_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
//
//	SetServiceStatus(g_hServiceStatus, &g_status);
//
//	nRetCode = LoadGlobalConfig(g_szGlobalConfigFileName);
//	PROCESS_ERROR(nRetCode);
//
//	//	g_InitStatisInfo(GetCurrentProcessId());
//
//	/************************************************************************/
//	// initialize
//	nRetCode = _InitServer();
//	LOG_PROCESS_ERROR(nRetCode);
//
//	//#if (defined(WIN32) || defined(WIN64))
//	//	{
//	//		char szTitle[MAX_PATH];
//	//		snprintf(szTitle, MAX_PATH, "Name Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
//	//		SetConsoleTitle(szTitle);
//	//		INF(szTitle);
//	//	}
//	//#endif  // WIN32
//
//	g_status.dwWin32ExitCode = S_OK;
//	g_status.dwCheckPoint = 0;
//	g_status.dwWaitHint = 0;
//	g_status.dwCurrentState = SERVICE_RUNNING;
//	SetServiceStatus(g_hServiceStatus, &g_status);
//
//
//	/************************************************************************/
//	// main loop
//	while (!g_bStop)
//	{
//		nRetCode = CNSMessageHandler::GetInstance().MainLoop();
//		LOG_PROCESS_ERROR(nRetCode);
//
//		nRetCode = CNameServer::GetInstance().MainLoop();
//		LOG_PROCESS_ERROR(nRetCode);
//
//		Sleep(1);
//	}
//
//	/************************************************************************/
//	// exit
//
//	CNameServer::GetInstance().Terminate();
//	INF("NS is exiting");
//
//	//	nRetCode = CNameServer::GetInstance().Close();
//	//	LOG_PROCESS_ERROR(nRetCode);
//
//Exit0:
//
//	nRetCode = GetLastError();
//
//	g_status.dwCurrentState = SERVICE_STOPPED;
//	SetServiceStatus(g_hServiceStatus, &g_status);
//
//	INF("NS has exited");
//
//	nRetCode = _UnInitServer();
//	LOG_CHECK_ERROR(nRetCode);
//
//#if defined(WIN32)
//	StopDumpThread();
//#endif
//}
//
//int APIENTRY WinMain(HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR     lpCmdLine,
//	int       nCmdShow)
//{
//	Init();
//
//	SERVICE_TABLE_ENTRY st[] =
//	{
//		{ g_szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
//		{ NULL, NULL }
//	};
//
//	if (_stricmp(lpCmdLine, "/install") == 0)
//	{
//		Install();
//	}
//	else if (_stricmp(lpCmdLine, "/uninstall") == 0)
//	{
//		Uninstall();
//	}
//	else
//	{
//		if(!StartServiceCtrlDispatcher(st))
//		{
//			INT32 error = GetLastError();
//
//			return -1;
//		}
//	}
//
//	return 0;
//}
//#else
INT32 main(INT32 argc, TCHAR** argv)
{
    INT32 nRetCode = 0;

    //	nRetCode = LoadGlobalConfig(g_szGlobalConfigFileName);
    //	LOG_PROCESS_ERROR(nRetCode);

    InitConfigPath();
    ReadConfig();

    nRetCode = CLog::GetInstance().Init("name_server", lltAll, lotConsole | lotFile | lotTrace, FALSE, TRUE, TRUE);
    LOG_PROCESS_ERROR(nRetCode);

#if defined(WIN32) | defined(WIN64)
    nRetCode = StartDumpThread(NULL, FALSE, "log");
    PROCESS_ERROR(nRetCode);

    //	SetWindowPos(GetConsoleWindow(), HWND_TOP, 1940, 455, 0, 0, SWP_NOSIZE);
#endif
//	nRetCode = g_OpenPakFolder(DEFAULT_PAK_PATH);
//	if (nRetCode)
//		INF("open pak folder %s successfully", DEFAULT_PAK_PATH);
//	else
//		INF("cannot open pak folder %s", DEFAULT_PAK_PATH);

    nRetCode = ProcessOption(argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    g_InitStatisInfo(GetCurrentProcessId());

    /************************************************************************/
    // initialize
    nRetCode = _InitServer();
    LOG_PROCESS_ERROR(nRetCode);

    INF("NS Initialized");

#if (defined(WIN32) || defined(WIN64))
    {
        char szTitle[MAX_PATH];
        snprintf(szTitle, MAX_PATH, "Name Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
        SetConsoleTitle(szTitle);
        INF(szTitle);
    }
#endif  // WIN32

    /************************************************************************/
    // main loop
    while (1)
    {
        nRetCode = CNSMessageHandler::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CNameServer::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        Sleep(1);
    }

    /************************************************************************/
    // exit
    INF("NS is exiting");

    //	nRetCode = CNameServer::GetInstance().Close();
    //	LOG_PROCESS_ERROR(nRetCode);

Exit0:
    INF("NS has exited");

    nRetCode = _UnInitServer();
    LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif

    return 0;
}
//#endif
