#include "stdafx.h"
#include "role_data_mgr.h"

#include "role_db_def.h"
#include "server_config.h"
#include "mysql.h"
#include "time_ex.h"
#include "master_def.h"
#include "db/ms_sql_stmt_list.h"
#include "role_db_base_data_conv.h"
#include "manageserver.h"
#include <time.h>
#include "gm_svr_def.h"

CRoleDataMgr CRoleDataMgr::ms_Instance;

CRoleDataMgr::CRoleDataMgr()
{
    memset(m_pRoot, NULL, sizeof(m_pRoot));
    m_nLoadedCount = 0;
}

CRoleDataMgr::~CRoleDataMgr()
{
    UnInit();
}

BOOL CRoleDataMgr::Init(void)
{
    BOOL bRet = FALSE;
    INT32 nRetCode = 0;
    UINT64 qwStartTick = 0;

    LOG_PROCESS_ERROR(sizeof(MS_ROLE_DATA) <= 256);

    nRetCode = m_RoleDataPool.Init(g_ServerConfig.MS.nInitRoleCount);
    LOG_PROCESS_ERROR(nRetCode);

    qwStartTick = GetTickCountEx();

    INF("Loading Roles...");

    m_nLoadedCount = 0;
    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectRole, _Load, NULL, "");
    LOG_PROCESS_ERROR(bRet);

    INF("Loaded %d Role(s) in %d ms", m_nLoadedCount, (INT32)(GetTickCountEx() - qwStartTick));

    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectGM, _LoadGM, NULL, "");
    LOG_PROCESS_ERROR(bRet);

    return TRUE;
Exit0:
    return FALSE;
}

bool CRoleDataMgr::CDeleteAllRole::operator()(UINT32 dwRoleID, MS_ROLE_DATA* pRoleData)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pRoleData);

    nRetCode = CRoleDataMgr::GetInstance().DestroyRole(pRoleData);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return TRUE;
}

BOOL CRoleDataMgr::UnInit(void)
{
    INT32 nRetCode = 0;
    CDeleteAllRole DeleteAllRole;

    nRetCode = m_RoleDataPool.Traverse(DeleteAllRole);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_RoleDataPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

CRoleDataMgr& CRoleDataMgr::GetInstance(void)
{
    return ms_Instance;
}

BOOL CRoleDataMgr::_Load(std::list<void*>& rResult, void* pUserData)
{
    BOOL bRet = FALSE;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32 dwRoleID;
        char szRoleName[ROLE_NAME_LEN];
        char szUserName[USER_NAME_LEN];
        UINT32 dwLoginTime;
        UINT8 byServerGroup;
        UINT32 dwBaseDataBufferLen;
        char BaseDataBuffer[512];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        if (!IsValidGroup(pLoadResult->byServerGroup))
        {
            CRI("detected role data in invalid group %d", pLoadResult->byServerGroup);
            return FALSE;
        }

        bRet = CRoleDataMgr::GetInstance()._LoadRole(
            pLoadResult->dwRoleID,
            pLoadResult->szRoleName,
            pLoadResult->szUserName,
            pLoadResult->dwLoginTime,
            pLoadResult->byServerGroup,
            pLoadResult->dwBaseDataBufferLen,
            pLoadResult->BaseDataBuffer);
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CRoleDataMgr::_LoadGM(std::list<void*>& rResult, void* pUserData)
{
    BOOL        bRet = FALSE;
    UINT32      dwRoleID = 0;
    INT32       nType = 0;
    UINT32      dwTime = 0;
    MS_ROLE_DATA* pRoleData = NULL;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8   bySvrGroup;
        UINT64  qwID;
        UINT32  dwTime;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->bySvrGroup));

        // Default code
        //nType = pLoadResult->qwID;
        //dwRoleID = pLoadResult->qwID >> 32;
        //dwTime = pLoadResult->dwTime;

        // Custom code
        nType = GM_RECORD_FREEZE_ROLE;  // Always freeze the character
        dwRoleID = pLoadResult->qwID;   // Write the character ID directly to the ID field of the database gm table
        dwTime = pLoadResult->dwTime;   // Ban duration

        //dwTime = (dwTime == 0 ? 0xFFFFFFFF : dwTime);

        pRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
        LOG_CHECK_ERROR(pRoleData);

        if (pRoleData && time(NULL) < dwTime)
        {
            if (nType == GM_RECORD_FORBID_CHAT)
            {
                pRoleData->dwForbidChatTime = dwTime;
            }
            else if (nType == GM_RECORD_FREEZE_ROLE)
            {
                pRoleData->dwLoginFrozenTime = dwTime;
            }
            else if (nType == GM_RECORD_FREEZE_CHEAT_ROLE)
            {
                pRoleData->dwLoginFrozenTime = dwTime;
            }

        }
        else
        {
            CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteGM, NULL, NULL, "ul", pLoadResult->qwID);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRoleDataMgr::_LoadRole(UINT32 dwRoleID, char* szRoleName, char* szUserName,
    UINT32 dwLoginTime, UINT8 byServerGroup, UINT32 dwBaseDataBufferLen, char* BaseDataBuffer)
{
    BOOL bRet = FALSE;
    ROLE_DB_BASE_DATA base_data;

    MS_ROLE_DATA* pRoleData = CreateRole(dwRoleID, szRoleName);
    LOG_PROCESS_ERROR(pRoleData);

    bRet = ConvertToBaseDataCurrentVersion(&base_data, BaseDataBuffer, dwBaseDataBufferLen);
    LOG_PROCESS_ERROR(bRet);

    //to be deleted
    base_data.dwRoleID = dwRoleID;

    bRet = pRoleData->LoadBaseData(&base_data);
    LOG_PROCESS_ERROR(bRet);

    pRoleData->nLastLoginTime = dwLoginTime;
    pRoleData->byGroup = byServerGroup;
    pRoleData->dwForbidChatTime = 0;
    pRoleData->dwLoginFrozenTime = 0;

    m_nLoadedCount++;

    return TRUE;

Exit0:
    return FALSE;
}

MS_ROLE_DATA* CRoleDataMgr::CreateRole(UINT32 dwRoleID, const char* pcszRoleName)
{
    INT32 nRetCode = 0;
    UINT16 wHashKey;

    MS_ROLE_DATA* pResult = m_RoleDataPool.New(dwRoleID);
    LOG_PROCESS_ERROR(pResult);

    memset(pResult, 0, sizeof(*pResult));

    strxcpy(pResult->szRoleName, pcszRoleName, sizeof(ROLE_NAME_KEY));
    wHashKey = _GetHashValue(pResult->RoleNameNode.ID);

    nRetCode = LM_Insert(&(pResult->RoleNameNode), &(m_pRoot[wHashKey]));
    LOG_PROCESS_ERROR(nRetCode);

    pResult->lstMails.Clear();

    return pResult;
Exit0:
    if (pResult)
        m_RoleDataPool.Delete(pResult);
    return NULL;
}

MS_ROLE_DATA* CRoleDataMgr::FindRole(UINT32 dwRoleID)
{
    return m_RoleDataPool.Find(dwRoleID);
}

bool CRoleDataMgr::CTestRoleName::operator()(UINT32 dwRoleID, MS_ROLE_DATA* pRoleData)
{
    MS_ROLE_DATA* pResult = NULL;

    pResult = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
    LOG_CHECK_ERROR(pResult == pRoleData);

    pResult = CRoleDataMgr::GetInstance().FindRoleByName(pRoleData->szRoleName);
    LOG_CHECK_ERROR(pResult == pRoleData);

    return true;
}

MS_ROLE_DATA* CRoleDataMgr::FindRoleByName(const char* pcszRoleName)
{
    UINT16 wHashKey;
    ROLE_NAME_KEY RoleNameKey;
    LM_MAP_NODE<ROLE_NAME_KEY>* pNode;

    strncpy((char*)&RoleNameKey, pcszRoleName, sizeof(RoleNameKey));
    wHashKey = _GetHashValue(RoleNameKey);

    pNode = LM_Search(RoleNameKey, m_pRoot[wHashKey]);
    LOG_PROCESS_ERROR(pNode);

    return (MS_ROLE_DATA*)(((char*)pNode) - (char*)&(((MS_ROLE_DATA*)0)->RoleNameNode));
Exit0:

    return NULL;
}

BOOL CRoleDataMgr::DestroyRole(MS_ROLE_DATA* pRoleData)
{
    INT32 nRetCode = 0;

    UINT16 wHashKey = _GetHashValue(pRoleData->RoleNameNode.ID);
    LM_Erase(&pRoleData->RoleNameNode, &m_pRoot[wHashKey]);

    nRetCode = m_RoleDataPool.Delete(pRoleData);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

inline UINT16  CRoleDataMgr::_GetHashValue(ROLE_NAME_KEY& rRoleNameKey)
{
    UINT16 wHashKey;
    UINT32 dwTempValue;
    UINT64 qwTempValue;

    qwTempValue = (rRoleNameKey.qwKey0 ^ rRoleNameKey.qwKey1 ^ rRoleNameKey.qwKey2);
    dwTempValue = (((UINT32*)&qwTempValue)[0] << 5) ^ ((UINT32*)&qwTempValue)[1];
    wHashKey = (((UINT16*)&dwTempValue)[0] << 5) ^ ((UINT16*)&dwTempValue)[1];

    return wHashKey;
}
