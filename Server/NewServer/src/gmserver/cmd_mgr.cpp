#include "stdafx.h"
#include "cmd_mgr.h"
#include "tl_user.h"
#include "user_manager.h"
#include "ms_mgr.h"
#include "gm_svr_def.h"
#include "db_mgr.h"
#include "message_handler.h"
#include "kernel.h"

#include "gm_def.h"
#include "gmserver.h"
#include "gm_query_thread.h"
#include "gm_role_mgr.h"
#include "gm_log.h"
#include "permission_mgr.h"

CCmdMgr  CCmdMgr::m_Instance;

CCmdMgr& CCmdMgr::GetInstance()
{
    return m_Instance;
}

CCmdMgr::CCmdMgr() :
m_SerialID(0)
{
}

CCmdMgr::~CCmdMgr()
{

}

BOOL CCmdMgr::Init()
{
    m_SerialID = 1;
    
    return FALSE;
}

BOOL CCmdMgr::UnInit()
{

    return FALSE;
}


CMD_DATA_BASE* CCmdMgr::NewCmd(GM_COMMAND eCmd)
{
    CMD_DATA_BASE   *pCmd = NULL;
    switch (eCmd)
    {
    case GM_CMD_PRE_QUERY:
    {
        pCmd = new CMD_PRE_QUERY;
    }
        break;
    case GM_CMD_QUERY_ROLE_ID:
    {
        pCmd = new CMD_QUERY_ROLE_ID;
    }
        break;
    case GM_CMD_QUERY_ROLE_DATA:
    {
        pCmd = new CMD_QUERY_ROLE_DATA;
    }
        break;
    case GM_CMD_FORBID_CHAT:
    {
        pCmd = new CMD_FORBIDDEN_CHAT;
    }
        break;
    case GM_CMD_FREEZE_ROLE:
    {
        pCmd = new CMD_FREEZE_ROLE;
    }
        break;
    case GM_CMD_KICK_ROLE:
    {
        pCmd = new CMD_KICK_ROLE;
    }
        break;
    case GM_CMD_ANNOUNCE:
    {
        pCmd = new CMD_ANNOUNCE;
    }
        break;
    case GM_CMD_GET_ANNOUNCE_LIST:
    {
        pCmd = new CMD_GET_ANNOUNCE_LIST;
    }
        break;
    case GM_CMD_DEL_ANNOUNCE:
    {
        pCmd = new CMD_DEL_ANNOUNCE;
    }
        break;
    case GM_CMD_SEND_MAIL:
    {
        pCmd = new CMD_SEND_MAIL;
    }
        break;
    case GM_CMD_LOG:
    {
        pCmd = new CMD_LOG;
    }
        break;
    case GM_CMD_EXE_GM_STR:
    {
        pCmd = new CMD_EXE_GM_STR;
    }
        break;
    case GM_CMD_LOAD_ROLE:
    {
        pCmd = new CMD_LOAD_ROLE;
    }
        break;
    case GM_CMD_MODIFY_ROLE:
    {
        pCmd = new CMD_MODIFY_ROLE;
    }
        break;
    case GM_CMD_QUERY_NAME_TO_ID:
    {
        pCmd = new CMD_QUERY_NAME_TO_ID;
    }
        break;
    case GM_CMD_ROLE_EXE_GM_CMD:
    {
        pCmd = new CMD_ROLE_EXE_GM_CMD;
    }
        break;
    case GM_CMD_SYS_EXCHANGE_INFO:
    {
        pCmd = new QUERY_SYSTEM_EXCHANGE_INFO;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    pCmd->dwSerialID = m_SerialID++;

    return pCmd;
Exit0:
    return NULL;
}

void CCmdMgr::DeleteCmd(CMD_DATA_BASE *pCmd)
{
    LOG_PROCESS_ERROR(pCmd);

    switch (pCmd->eCmdType)
    {
    case GM_CMD_PRE_QUERY:
        delete ((CMD_PRE_QUERY*)pCmd);
        break;
    case GM_CMD_QUERY_ROLE_ID:
        delete ((CMD_QUERY_ROLE_ID*)pCmd);
        break;
    case GM_CMD_QUERY_ROLE_DATA:
        delete ((CMD_QUERY_ROLE_DATA*)pCmd);
        break;
    case GM_CMD_FORBID_CHAT:
        delete ((CMD_FORBIDDEN_CHAT*)pCmd);
        break;
    case GM_CMD_FREEZE_ROLE:
        delete ((CMD_FREEZE_ROLE*)pCmd);
        break;
    case GM_CMD_KICK_ROLE:
        delete ((CMD_KICK_ROLE*)pCmd);
        break;
    case GM_CMD_ANNOUNCE:
        delete ((CMD_ANNOUNCE*)pCmd);
        break;
    case GM_CMD_GET_ANNOUNCE_LIST:
        delete ((CMD_GET_ANNOUNCE_LIST*)pCmd);
        break;
    case GM_CMD_DEL_ANNOUNCE:
        delete ((CMD_DEL_ANNOUNCE*)pCmd);
        break;
    case GM_CMD_SEND_MAIL:
        delete ((CMD_SEND_MAIL*)pCmd);
        break;
    case GM_CMD_LOG:
        delete ((CMD_LOG*)pCmd);
        break;
    case GM_CMD_EXE_GM_STR:
        delete ((CMD_EXE_GM_STR*)pCmd);
        break;
    case GM_CMD_LOAD_ROLE:
        delete ((CMD_LOAD_ROLE*)pCmd);
        break;
    case GM_CMD_MODIFY_ROLE:
        delete ((CMD_MODIFY_ROLE*)pCmd);
        break;
    case GM_CMD_QUERY_NAME_TO_ID:
        delete ((CMD_QUERY_NAME_TO_ID*)pCmd);
        break;
    case GM_CMD_ROLE_EXE_GM_CMD:
        delete ((CMD_ROLE_EXE_GM_CMD*)pCmd);
        break;
    case GM_CMD_SYS_EXCHANGE_INFO:
        delete ((QUERY_SYSTEM_EXCHANGE_INFO*)pCmd);
        break;
    default:
        break;
    }

Exit0:
    return;
}

CRoleNameParse::CRoleNameParse()
{

}

CRoleNameParse::~CRoleNameParse()
{

}

BOOL CRoleNameParse::Init()
{
    m_dwCursor = 0;
    m_dwCount = 0;
    memset(m_szRoleName, 0, sizeof(m_szRoleName));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRoleNameParse::Uninit()
{
    m_dwCursor = 0;
    m_dwCount = 0;
    memset(m_szRoleName, 0, sizeof(m_szRoleName));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRoleNameParse::AddNameList(const char *pBuffer, UINT32 dwLength)
{
    char    *pTempBuffer = NULL;
    char    *pName = NULL;
    char    ch = ',';
    LOG_PROCESS_ERROR(pBuffer);

    pTempBuffer = new char[dwLength + 1];
    LOG_PROCESS_ERROR(pTempBuffer);

    strxcpy(pTempBuffer, pBuffer, dwLength+1);

    pName = strtok(pTempBuffer, &ch);
    for (INT32 nIndex = 0; pName && nIndex < MAX_GET_ROLE_LIST_NAME_NUM; ++nIndex)
    {
        sprintf(m_szRoleName[nIndex], pName, ROLE_NAME_LEN);
        pName = strtok(NULL, &ch);
        ++m_dwCount;
    }

    return TRUE;
Exit0:
    return FALSE;
}

const char *CRoleNameParse::FirstName()
{
    m_dwCursor = 0;
    LOG_PROCESS_ERROR(m_dwCount > m_dwCursor);
    LOG_PROCESS_ERROR(m_dwCount > 0);

    return m_szRoleName[m_dwCursor];
Exit0:
    return NULL;
}

const char *CRoleNameParse::NextName()
{
    ++m_dwCursor;
    PROCESS_ERROR(m_dwCursor < m_dwCount);

    return m_szRoleName[m_dwCursor];
Exit0:
    return NULL;
}

char    g_szTLMessageBuffer[2048];

BOOL EchoTLError(UINT32 dwTLSessionID, const char *pcszErrorStr, ...)
{
    INT32       nRetCode = 0;
    
    va_list     valist;

    LOG_PROCESS_ERROR(pcszErrorStr);

    va_start(valist, pcszErrorStr);

    memset(g_szTLMessageBuffer, 0, 2048);
    vsprintf_s(g_szTLMessageBuffer, 2048, pcszErrorStr, valist);
    g_szTLMessageBuffer[2047] = '\0';

    nRetCode = CMessageHandler::GetInstance().DoTLErrorStr(dwTLSessionID, g_szTLMessageBuffer);
    LOG_PROCESS_ERROR(nRetCode);

    va_end(valist);

    return TRUE;
Exit0:
    if (pcszErrorStr)
        va_end(valist);
    return FALSE;
}
