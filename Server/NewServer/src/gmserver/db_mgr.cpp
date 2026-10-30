/********************************************************************
	created:	20:12:2013   14:01
	file:		db_mgr.cpp
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "db_mgr.h"
#include "server_config.h"
#include "prep_stmt.h"
#include "role_db_def.h"
#include "gm_sql_stmt_list.h"
#include "tl_user.h"
#include "user_manager.h"
#include "role.h"
#include "zlib.h"
#include "mail_def.h"
#include "../manageserver/mail/mail.h"
#include "gm_def.h"
#include "gm_log.h"

CDBMgr   CDBMgr::m_Instance;


CDBMgr&  CDBMgr::GetInstance()
{
    return m_Instance;
}

CDBMgr::CDBMgr()
{

}

CDBMgr::~CDBMgr()
{

}


BOOL CDBMgr::Init()
{
    INT32       nRetCode = 0;

    strxcpy(m_DBData.szServerAddr, g_ServerConfig.DB.szDBIPAddress, DB_SETTING_STRING_LEN);
    strxcpy(m_DBData.szLoginName, g_ServerConfig.DB.szDBAccount, DB_SETTING_STRING_LEN);
    strxcpy(m_DBData.szPassword, g_ServerConfig.DB.szDBPassword, DB_SETTING_STRING_LEN);
    strxcpy(m_DBData.szDBName, g_ServerConfig.DB.szDBName, DB_SETTING_STRING_LEN);
    strxcpy(m_DBData.szCharSet, g_ServerConfig.DB.szDBCharset, DB_SETTING_STRING_LEN);
    m_DBData.dwServerPort = g_ServerConfig.DB.dwDBPort;

    strxcpy(m_DBLogData.szServerAddr, g_ServerConfig.DB.szLogDBIPAddress, DB_SETTING_STRING_LEN);
    strxcpy(m_DBLogData.szLoginName, g_ServerConfig.DB.szLogDBAccount, DB_SETTING_STRING_LEN);
    strxcpy(m_DBLogData.szPassword, g_ServerConfig.DB.szLogDBPassword, DB_SETTING_STRING_LEN);
    strxcpy(m_DBLogData.szDBName, g_ServerConfig.DB.szLogDBName, DB_SETTING_STRING_LEN);
    strxcpy(m_DBLogData.szCharSet, g_ServerConfig.DB.szLogDBCharset, DB_SETTING_STRING_LEN);
    m_DBLogData.dwServerPort = g_ServerConfig.DB.dwLogDBPort;

    strxcpy(m_SlaverDBData.szServerAddr, g_ServerConfig.DB.szSlaverDBIPAddress, DB_SETTING_STRING_LEN);
    strxcpy(m_SlaverDBData.szLoginName, g_ServerConfig.DB.szSlaverDBAccount, DB_SETTING_STRING_LEN);
    strxcpy(m_SlaverDBData.szPassword, g_ServerConfig.DB.szSlaverDBPassword, DB_SETTING_STRING_LEN);
    strxcpy(m_SlaverDBData.szDBName, g_ServerConfig.DB.szSlaverDBName, DB_SETTING_STRING_LEN);
    strxcpy(m_SlaverDBData.szCharSet, g_ServerConfig.DB.szSlaverDBCharset, DB_SETTING_STRING_LEN);
    m_SlaverDBData.dwServerPort = g_ServerConfig.DB.dwSlaverDBPort;

    //m_DBMap.insert(std::make_pair(tempData.dwSvrID, tempData));

    // 
    if (g_ServerConfig.DB.bUseIndependentDB)
    {
        char szHostName[256];
        char* psz = szHostName;
        gethostname(szHostName, 256);

        while (*psz)
        {
            if (*psz == '-')
                *psz = '_';
            psz++;
        }

        strxcpy(m_DBData.szDBName, szHostName, DB_SETTING_STRING_LEN);
        strxcpy(m_SlaverDBData.szDBName, szHostName, DB_SETTING_STRING_LEN);
        snprintf(m_DBLogData.szDBName, DB_SETTING_STRING_LEN, "%s_log", szHostName);
    }

    nRetCode = GetMasterSqlMgr().Init(
        m_DBData.szServerAddr,
        m_DBData.szLoginName,
        m_DBData.szPassword,
        m_DBData.szDBName,
        m_DBData.szCharSet,
        m_DBData.dwServerPort,
        g_GMMasterSqlStmtList, msqlTotal, 2);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetSlaverSqlMgr().Init(
        m_SlaverDBData.szServerAddr,
        m_SlaverDBData.szLoginName,
        m_SlaverDBData.szPassword,
        m_SlaverDBData.szDBName,
        m_SlaverDBData.szCharSet,
        m_SlaverDBData.dwServerPort,
        g_GMSlaverStmtList, sSqlTotal, 2);
    LOG_PROCESS_ERROR(nRetCode);

    // Default code temporarily disabled
    //nRetCode = GetLogDataSqlMgr().Init(
    //    m_DBLogData.szServerAddr,
    //    m_DBLogData.szLoginName,
    //    m_DBLogData.szPassword,
    //    m_DBLogData.szDBName,
    //    m_DBLogData.szCharSet,
    //    m_DBLogData.dwServerPort,
    //    g_GMLogDataStmtList, lSqlTotal, 2);
    //LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::UnInit()
{
    //m_DBMap.clear();
    memset(&m_DBData, 0, sizeof(m_DBData));
    memset(&m_SlaverDBData, 0, sizeof(m_SlaverDBData));


    GetMasterSqlMgr().UnInit();
    GetSlaverSqlMgr().UnInit();

    return TRUE;
}

DBSetting* CDBMgr::GetDBSetting()
{
    return &m_DBData;
Exit0:
    return NULL;
}

DBSetting* CDBMgr::GetSlaverDBSetting()
{
    return &m_SlaverDBData;
Exit0:
    return NULL;
}

cqyh::MysqlPtr CDBMgr::GetMysql( )
{
    cqyh::MysqlPtr connection;
    DBSetting   *pSetting = NULL;
    MYSQL       *res_mysql = NULL;

    connection = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(connection.get());

    pSetting = GetDBSetting();
    LOG_PROCESS_ERROR(pSetting);

    res_mysql = mysql_real_connect(
        connection.get(),
        pSetting->szServerAddr,
        pSetting->szLoginName,
        pSetting->szPassword,
        pSetting->szDBName,
        pSetting->dwServerPort,
        NULL,
        0);
    LOG_PROCESS_ERROR(res_mysql);

    if (strlen(pSetting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(connection.get(), pSetting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    return connection;
Exit0:
    return {};
}

cqyh::MysqlPtr CDBMgr::GetSlaverMysql()
{
    cqyh::MysqlPtr connection;
    DBSetting   *pSetting = NULL;
    MYSQL       *res_mysql = NULL;

    connection = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(connection.get());

    pSetting = GetSlaverDBSetting();
    LOG_PROCESS_ERROR(pSetting);

    res_mysql = mysql_real_connect(
        connection.get(),
        pSetting->szServerAddr,
        pSetting->szLoginName,
        pSetting->szPassword,
        pSetting->szDBName,
        pSetting->dwServerPort,
        NULL,
        0);
    LOG_PROCESS_ERROR(res_mysql);

    if (strlen(pSetting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(connection.get(), pSetting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    return connection;
Exit0:
    return {};
}


cqyh::MysqlPtr CDBMgr::GetLogMysql( )
{
    cqyh::MysqlPtr connection;
    DBSetting   *pSetting = NULL;
    MYSQL       *res_mysql = NULL;

    connection = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(connection.get());

    pSetting = GetDBSetting();
    LOG_PROCESS_ERROR(pSetting);

    res_mysql = mysql_real_connect(
        connection.get(),
        pSetting->szServerAddr,
        pSetting->szLoginName,
        pSetting->szPassword,
        pSetting->szDBName,//TODO: taoqun
        pSetting->dwServerPort,
        NULL,
        0);
    LOG_PROCESS_ERROR(res_mysql);

    if (strlen(pSetting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(connection.get(), pSetting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    return connection;
Exit0:
    return {};
}

cqyh::MysqlPtr CDBMgr::GetSlaverLogMysql()
{
    cqyh::MysqlPtr connection;
    DBSetting   *pSetting = NULL;
    MYSQL       *res_mysql = NULL;

    connection = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(connection.get());

    pSetting = GetSlaverDBSetting();
    LOG_PROCESS_ERROR(pSetting);

    res_mysql = mysql_real_connect(
        connection.get(),
        pSetting->szServerAddr,
        pSetting->szLoginName,
        pSetting->szPassword,
        pSetting->szDBName,//TODO: taoqun
        pSetting->dwServerPort,
        NULL,
        0);
    LOG_PROCESS_ERROR(res_mysql);

    if (strlen(pSetting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(connection.get(), pSetting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    return connection;
Exit0:
    return {};
}


BOOL CDBMgr::MainLoop()
{
    INT32       nRetCode = 0;

    nRetCode = m_MasterSqlMgr.MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_SlaverSqlMgr.MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_LogDataSqlMgr.MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}


BOOL CDBMgr::PreQueryRoleID(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32       nRetCode = 0;
    CMD_PRE_QUERY   *pQueryCmd = NULL;

    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (CMD_PRE_QUERY *)pCmd;
    LOG_PROCESS_ERROR(pQueryCmd);

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);

    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetSlaverSqlMgr().SyncExecSqlStmtOwned(GMMysqlSelectRoleID, MakeOwnedSqlCallback(CB_DBPreQueryRoleID, std::move(pCmdID)), "s", pQueryCmd->szRoleName);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryRoleID(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32   nRetCode = 0;
    CMD_QUERY_ROLE_ID   *pQueryCmd = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (CMD_QUERY_ROLE_ID *)pCmd;
    LOG_PROCESS_ERROR(pQueryCmd);

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);

    *pCmdID = pCmd->dwSerialID;

    if (pQueryCmd->nQueryType == GM_QUERY_BY_ROLE_NAME)
    {
        nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectRoleIDByRoleName, MakeOwnedSqlCallback(CB_DBQueryRoleID, std::move(pCmdID)), "s", pQueryCmd->szName);
        LOG_PROCESS_ERROR(nRetCode);

        //nRetCode = CGMLog::GetInstance().WriteLog(pQueryCmd->szUserName, "query role id", "Role Name[%s]", pQueryCmd->szName);
        //LOG_CHECK_ERROR(nRetCode);
    }
    else if (pQueryCmd->nQueryType == GM_QUERY_BY_USER_NAME)
    {
        nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectRoleIDByUserName, MakeOwnedSqlCallback(CB_DBQueryRoleID, std::move(pCmdID)), "s", pQueryCmd->szName);
        LOG_PROCESS_ERROR(nRetCode);

        //nRetCode = CGMLog::GetInstance().WriteLog(pQueryCmd->szUserName, "query role id", "Account Name[%s]", pQueryCmd->szName);
        //LOG_CHECK_ERROR(nRetCode);
    }


    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryRoleData(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32               nRetCode = 0;
    CMD_QUERY_ROLE_DATA *pQueryData = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pQueryData = (CMD_QUERY_ROLE_DATA *)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectRoleData, MakeOwnedSqlCallback(CB_DBLoadRole, std::move(pCmdID)), "ud", pQueryData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryGuildID(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32               nRetCode = 0;
    CMD_QUERY_ROLE_DATA *pQueryData = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pQueryData = (CMD_QUERY_ROLE_DATA *)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectGuildID, MakeOwnedSqlCallback(CB_DBQueryGuildID, std::move(pCmdID)), "ud", pQueryData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryGMForbid(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32               nRetCode = 0;
    CMD_QUERY_ROLE_DATA *pQueryData = NULL;
    UINT64              qwIDChat = 0;
    UINT64              qwIDLogin = 0;
    LOG_PROCESS_ERROR(pCmd);

    pQueryData = (CMD_QUERY_ROLE_DATA *)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    qwIDChat = (((UINT64)pQueryData->dwRoleID) << 32) + GM_RECORD_FORBID_CHAT;
    qwIDLogin = (((UINT64)pQueryData->dwRoleID) << 32) + GM_RECORD_FREEZE_ROLE;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectGMForbid, MakeOwnedSqlCallback(CB_DBQueryGMForbid, std::move(pCmdID)), "ulul", qwIDChat, qwIDLogin);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryMail(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32               nRetCode = 0;
    CMD_QUERY_ROLE_DATA *pQueryData = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pQueryData = (CMD_QUERY_ROLE_DATA *)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectMail, MakeOwnedSqlCallback(CB_DBLoadMail, std::move(pCmdID)), "ud", pQueryData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQueryRoleGroup(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8   byGroupID;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        *(INT32*)pUserData = pLoadResult->byGroupID;
        break;
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::ForbiddenChat(CMD_DATA_BASE *pCmd)
{
    CMD_FORBIDDEN_CHAT   * pForbidChat{};
    INT32   nRetCode = 0;
    INT32   nGroupID = -1;
    LOG_PROCESS_ERROR(pCmd);

    pForbidChat = (CMD_FORBIDDEN_CHAT*)pCmd;


    nRetCode = GetSlaverSqlMgr().SyncExecSqlStmt(GMMysqlSelectGroup, CB_DBQueryRoleGroup, &nGroupID, "ud", pForbidChat->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);
    LOG_PROCESS_ERROR(nGroupID != -1);

    // master db
    if (pForbidChat->dwTime != 0)
    {
        nRetCode = GetMasterSqlMgr().SyncExecSqlStmt(GMMasterMysqlReplaceGM, NULL, NULL, "uculud", nGroupID, (((UINT64)pForbidChat->dwRoleID) << 32) + GM_RECORD_FORBID_CHAT, pForbidChat->dwTime);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = GetMasterSqlMgr().SyncExecSqlStmt(GMMasterMysqlDeleteGM, NULL, NULL, "ul", (((UINT64)pForbidChat->dwRoleID) << 32) + GM_RECORD_FORBID_CHAT);
        LOG_PROCESS_ERROR(nRetCode);
    }
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::FreezeRole(CMD_DATA_BASE *pCmd)
{
    CMD_FREEZE_ROLE   * pFreezeRole{};
    INT32   nRetCode = 0;
    INT32   nGroupID = -1;
    LOG_PROCESS_ERROR(pCmd);

    pFreezeRole = (CMD_FREEZE_ROLE*)pCmd;

    nRetCode = GetSlaverSqlMgr().SyncExecSqlStmt(GMMysqlSelectGroup, CB_DBQueryRoleGroup, &nGroupID, "ud", pFreezeRole->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);
    LOG_PROCESS_ERROR(nGroupID != -1);

    // master db
    if (pFreezeRole->dwTime != 0)
    {
        nRetCode = GetMasterSqlMgr().SyncExecSqlStmt(GMMasterMysqlReplaceGM, NULL, NULL, "uculud", nGroupID, (((UINT64)pFreezeRole->dwRoleID) << 32) + GM_RECORD_FREEZE_ROLE, pFreezeRole->dwTime);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = GetMasterSqlMgr().SyncExecSqlStmt(GMMasterMysqlDeleteGM, NULL, NULL, "ul", (((UINT64)pFreezeRole->dwRoleID) << 32) + GM_RECORD_FREEZE_ROLE);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::SendMail(CMD_DATA_BASE *pCmd)
{
    CMD_SEND_MAIL   * pSendMail{};
    std::unique_ptr<UINT32> pCmdID;
    INT32   nRetCode = 0;
    struct SimpleAttach     sa;

    LOG_PROCESS_ERROR(pCmd);
    pSendMail = (CMD_SEND_MAIL*)pCmd;
    sa.nItemID = pSendMail->dwItemID;
    sa.nItemCount = pSendMail->nItemNum;

    // get mail id

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetMasterSqlMgr().SyncExecSqlStmtOwned(GMMasterMysqlSelectMaxMailID, MakeOwnedSqlCallback(CB_DBSelectMailID, std::move(pCmdID)), "");
    LOG_PROCESS_ERROR(nRetCode);

    pCmdID = NULL;

    if (pSendMail->nItemNum == 0)
    {
        // master db
        nRetCode = GetMasterSqlMgr().SyncExecSqlStmt(GMMasterMysqlInsertMail, NULL, NULL, "udududucssucb",
            pSendMail->dwMailID, 0, pSendMail->dwRoleID, MAILTYPE_SYSTEM, pSendMail->szTitle, pSendMail->szContent,
            ATTACHTYPE_NONE, &sa, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        // master db
        nRetCode = GetMasterSqlMgr().SyncExecSqlStmt(GMMasterMysqlInsertMail, NULL, NULL, "udududucssucb",
            pSendMail->dwMailID, 0, pSendMail->dwRoleID, MAILTYPE_SYSTEM, pSendMail->szTitle, pSendMail->szContent,
            ATTACHTYPE_SIMPLE, &sa, sizeof(SimpleAttach));
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryLog(CMD_DATA_BASE *pCmd)
{
    CMD_LOG   * pCmdLog{};
    std::unique_ptr<UINT32> pCmdID;
    INT32       nRetCode = 0;
    char        szSndaID[65] = { 0 };
    UINT32      dwRoleID = 0;
    UINT32      dwTargetRoleID = 0;
    UINT32      dwItemID = 0;
    UINT64      qwGuid = 0;
    INT32       nMoneyType = 0;
    INT32       nQuestID = 0;

    LOG_PROCESS_ERROR(pCmd);
    pCmdLog = (CMD_LOG*)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    // Default code temporarily disabled
    /*
    switch (pCmdLog->nLogType)
    {
    case GM_LOG_QUERY_ROLE_LOGIN:
    {
        sscanf(pCmdLog->szQueryStr, "%s", &szSndaID);

        nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectRoleLogin, MakeOwnedSqlCallback(CB_DBQueryLogRoleLogin, std::move(pCmdID)), "sudud", szSndaID, pCmdLog->dwBeginTime, pCmdLog->dwEndTime);
        LOG_PROCESS_ERROR(nRetCode);
    }
        break;
    case GM_LOG_QUERY_ROLE_LOGOUT:
    {
        sscanf(pCmdLog->szQueryStr, "%u", &dwRoleID);

        nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectRoleLogout, MakeOwnedSqlCallback(CB_DBQueryLogRoleLogout, std::move(pCmdID)), "ududud", dwRoleID, pCmdLog->dwBeginTime, pCmdLog->dwEndTime);
        LOG_PROCESS_ERROR(nRetCode);
    }
        break;
    case GM_LOG_QUERY_ITEM:
    {
        sscanf(pCmdLog->szQueryStr, "%u %u %llu", &dwRoleID, &dwItemID, &qwGuid);

        if (qwGuid != 0)
        {
            nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectItemByGuid, MakeOwnedSqlCallback(CB_DBQueryLogItem, std::move(pCmdID)), "uludud", qwGuid, pCmdLog->dwBeginTime, pCmdLog->dwEndTime);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else if (dwItemID != 0)
        {
            nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectItemByItemID, MakeOwnedSqlCallback(CB_DBQueryLogItem, std::move(pCmdID)), "ududud", dwItemID, pCmdLog->dwBeginTime, pCmdLog->dwEndTime);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else if (dwRoleID != 0)
        {
            nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectItemByRoleID, MakeOwnedSqlCallback(CB_DBQueryLogItem, std::move(pCmdID)), "ududud", dwRoleID, pCmdLog->dwBeginTime, pCmdLog->dwEndTime);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
            LOG_PROCESS_ERROR(FALSE);
    }
        break;
    case GM_LOG_QUERY_MONEY:
    {
        sscanf(pCmdLog->szQueryStr, "%u %u %d", &dwRoleID, &dwTargetRoleID, &nMoneyType);

        if (dwRoleID != 0)
        {
            nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectMoneyBySourceRole, MakeOwnedSqlCallback(CB_DBQueryLogMoney, std::move(pCmdID)), "ududud", dwRoleID, pCmdLog->dwBeginTime, pCmdLog->dwEndTime);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else if (dwTargetRoleID != 0)
        {
            nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectMoneyByTargetRole, MakeOwnedSqlCallback(CB_DBQueryLogMoney, std::move(pCmdID)), "ududud", dwTargetRoleID, pCmdLog->dwBeginTime, pCmdLog->dwEndTime);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
            LOG_PROCESS_ERROR(FALSE);
    }
        break;
    case GM_LOG_QUERY_QUEST:
    {
        sscanf(pCmdLog->szQueryStr, "%u %d", &dwRoleID, &nQuestID);

        if (dwRoleID != 0)
        {
            nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectQuestByRole, MakeOwnedSqlCallback(CB_DBQueryLogQuest, std::move(pCmdID)), "ud", dwRoleID);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else if (nQuestID != 0)
        {
            nRetCode = GetLogDataSqlMgr().AsyncExecSqlStmtOwned(GMLogSelectQuestByQuest, MakeOwnedSqlCallback(CB_DBQueryLogQuest, std::move(pCmdID)), "ud", dwRoleID);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
            LOG_PROCESS_ERROR(FALSE);
    }
        break;
    default:
        break;
    }
    */

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::LoadRole(UINT32 dwSerialID, UINT32 dwRoleID)
{
    INT32               nRetCode = 0;

    auto pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = dwSerialID;

    nRetCode = GetMasterSqlMgr().AsyncExecSqlStmtOwned(GMMasterMysqlSelectRoleData, MakeOwnedSqlCallback(CB_DBLoadRole, std::move(pCmdID)), "ud", dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::LoadMail(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32               nRetCode = 0;
    CMD_LOAD_ROLE *pLoadRole = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pLoadRole = (CMD_LOAD_ROLE *)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetMasterSqlMgr().AsyncExecSqlStmtOwned(GMMasterMysqlSelectMail, MakeOwnedSqlCallback(CB_DBLoadMail, std::move(pCmdID)), "ud", pLoadRole->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}


BOOL CDBMgr::RoleExeGMCmd(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32               nRetCode = 0;
    CMD_ROLE_EXE_GM_CMD *pRoleExeCmd = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pRoleExeCmd = (CMD_ROLE_EXE_GM_CMD *)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetMasterSqlMgr().AsyncExecSqlStmtOwned(GMMasterMysqlSelectRoleData, MakeOwnedSqlCallback(CB_DBRoleExeCmd, std::move(pCmdID)), "ud", pRoleExeCmd->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::DelMail(UINT32 dwMailID)
{
    INT32               nRetCode = 0;

    nRetCode = GetMasterSqlMgr().SyncExecSqlStmt(GMMasterMysqlDeleteMail, NULL, NULL, "ud", dwMailID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::CheckRoleFreeze(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32               nRetCode = 0;
    UINT64              qwIDLogin = 0;
    UINT32              dwRoleID = 0;
    LOG_PROCESS_ERROR(pCmd);

    if (pCmd->eCmdType == GM_CMD_LOAD_ROLE)
    {
        dwRoleID = ((CMD_LOAD_ROLE *)pCmd)->dwRoleID;
    }
    else if (pCmd->eCmdType == GM_CMD_ROLE_EXE_GM_CMD)
    {
        dwRoleID = ((CMD_ROLE_EXE_GM_CMD *)pCmd)->dwRoleID;
    }

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    qwIDLogin = (((UINT64)dwRoleID) << 32) + GM_RECORD_FREEZE_ROLE;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectFreezeTime, MakeOwnedSqlCallback(CB_DBCheckRoleFreeze, std::move(pCmdID)), "ul", qwIDLogin);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::SaveRoleData(CRole *poRole)
{
    INT32               nRetCode = 0;
    static UINT8        s_szRoleData[65536];
    UINT32              dwRoleDataSize = 65536;
    ROLE_DB_BASE_DATA*  pBaseData = NULL;
    ROLE_DB_DATA*       pRoleData = (ROLE_DB_DATA*)s_szRoleData;
    UINT32              dwRoleDataLength = 0;
    UINT32              dwRoleBaseDataLength = sizeof(ROLE_DB_BASE_DATA);
    unsigned char       zipped_role_data[65536];
    unsigned long       zipped_role_data_len = 65536;

    LOG_PROCESS_ERROR(poRole);

    nRetCode = poRole->Save((char *)s_szRoleData, dwRoleDataSize);
    LOG_PROCESS_ERROR(nRetCode);
    dwRoleDataLength = dwRoleDataSize;
    
    pBaseData = (ROLE_DB_BASE_DATA*)(pRoleData->RoleData + pRoleData->BlockTable[rbtBase].dwStartOffset);

    nRetCode = compress(zipped_role_data, &zipped_role_data_len, (unsigned char*)pRoleData, dwRoleDataLength);
    //debug
    DBG("save roleid:%u, original data len:%u, zipped data len:%u, zipped data crc:%X", poRole->GetObjID(), dwRoleDataLength, zipped_role_data_len,
        crc32(0, (const Bytef*)zipped_role_data, zipped_role_data_len));

    // master db
    nRetCode = GetMasterSqlMgr().AsyncExecSqlStmt(GMMasterMysqlUpdateRoleData, NULL, NULL, "bbud", pBaseData, dwRoleBaseDataLength, zipped_role_data, zipped_role_data_len, poRole->GetObjID());
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryNameToID(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32   nRetCode = 0;
    CMD_QUERY_NAME_TO_ID   *pQueryCmd = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (CMD_QUERY_NAME_TO_ID *)pCmd;
    LOG_PROCESS_ERROR(pQueryCmd);

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);

    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectRoleIDByName, MakeOwnedSqlCallback(CB_DBQueryNameToID, std::move(pCmdID)), "s", pQueryCmd->szRoleName);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryExchangeLog(UINT32 dwSessionID, INT32 nGroupID)
{
    INT32       nRetCode = 0;
    
    auto pSessionID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pSessionID);

    *pSessionID = dwSessionID;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectExchangeLog, MakeOwnedSqlCallback(CB_DBQueryExchangeLog, std::move(pSessionID)), "uc", nGroupID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QueryExchangeHistory(UINT32 dwSessionID, INT32 nGroupID)
{
    INT32       nRetCode = 0;
    auto pSessionID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pSessionID);

    *pSessionID = dwSessionID;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectExchangeHistory, MakeOwnedSqlCallback(CB_DBQueryExchangeHistory, std::move(pSessionID)), "uc", nGroupID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QuerySysExchangeOrder(CMD_DATA_BASE *pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32               nRetCode = 0;
    QUERY_SYSTEM_EXCHANGE_INFO *pQueryCmd = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (QUERY_SYSTEM_EXCHANGE_INFO *)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectSysExchangeOrder, MakeOwnedSqlCallback(CB_DBQuerySysExchangeOrderInfo, std::move(pCmdID)),
        "ucudud", pQueryCmd->byGroupID, pQueryCmd->dwBeginDayTime, pQueryCmd->dwEndDayTime);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBMgr::QuerySysExchangeLog(CMD_DATA_BASE* pCmd)
{
    std::unique_ptr<UINT32> pCmdID;
    INT32       nRetCode = 0;
    QUERY_SYSTEM_EXCHANGE_INFO *pQueryCmd = NULL;
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (QUERY_SYSTEM_EXCHANGE_INFO *)pCmd;

    pCmdID = std::make_unique<UINT32>();
    LOG_PROCESS_ERROR(pCmdID);
    *pCmdID = pCmd->dwSerialID;

    nRetCode = GetSlaverSqlMgr().AsyncExecSqlStmtOwned(GMMysqlSelectSysExchangeLog, MakeOwnedSqlCallback(CB_DBQuerySysExchangeLogInfo, std::move(pCmdID)),
        "ucudud", pQueryCmd->byGroupID, pQueryCmd->dwBeginDayTime, pQueryCmd->dwEndDayTime);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}



