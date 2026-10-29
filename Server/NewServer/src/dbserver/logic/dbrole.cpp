#include "stdafx.h"
#include "dbrole.h"

#include "time_ex.h"

#include "dbserver.h"
#include "role_db_base_data_conv.h"


CDBRole::CDBRole()
{
    BOOL bRet = m_MemList.Init(32);
    LOG_CHECK_ERROR(bRet);

    m_pCurMem = NULL;
    m_dbtask = DB_TASK_FREE;
    m_lockstate = LOCK_STATE_INVALID;
    m_dbuser = NULL;
    m_dwRoleID = 0;
    m_dwDeleteTime = 0;
    m_nResult = 0;
    m_dwGSSessionID = 0;
    memset(&m_CreateRoleParam, 0, sizeof(m_CreateRoleParam));
    m_qwLastSaveTime = 0;
}

CDBRole::~CDBRole()
{
    BOOL bRet = m_MemList.UnInit();
    LOG_CHECK_ERROR(bRet);
}

BOOL CDBRole::SaveData(const char* data, UINT32 len)
{
    BOOL bRet = FALSE;
    DB_ROLE_DATA* pData = NULL;

    LOG_PROCESS_ERROR(data);
    LOG_PROCESS_ERROR(len <= DATA_SIZE);

    pData = CDBServer::GetInstance().m_memmgr.Pop();
    LOG_PROCESS_ERROR(pData);

    memcpy(pData->RoleData, data, len);

    pData->CtrlNode.dwDataSize = len;
    pData->CtrlNode.LastModified = GetTickCountEx();
    pData->CtrlNode.LastSaved = 0;
    pData->CtrlNode.Reading = 1;

    bRet = m_MemList.Push(pData);
    LOG_PROCESS_ERROR(bRet);

    m_pCurMem->CtrlNode.Reading = 0;
    m_pCurMem = pData;

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CDBRole::LoadData()
{
    BOOL bRet = FALSE;
    CDBServer& dbserver = CDBServer::GetInstance();

    LOG_PROCESS_ERROR(m_pCurMem == NULL);

    m_pCurMem = dbserver.m_memmgr.Pop();
    LOG_PROCESS_ERROR(m_pCurMem);

    bRet = m_MemList.Push(m_pCurMem);
    LOG_PROCESS_ERROR(bRet);

    m_dbtask = CDBRole::DB_TASK_LOAD_ROLE;
    m_dbuser->m_dbstate = CDBUser::dtsLoading;
    bRet = dbserver.m_threads[m_dwRoleID%dbserver.m_dwThreadCount].AddRoleDbTask(this);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    return FALSE;
}

void CDBRole::OnCreateRole()
{
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(m_dbuser);
    LOG_PROCESS_ERROR(m_dbuser->m_dbstate == CDBUser::dtsCreating);
    m_dbuser->m_dbstate = CDBUser::dtsFree;
    LOG_PROCESS_ERROR(m_dbuser->m_pPlayingRole == NULL);
    LOG_PROCESS_ERROR(m_dbuser->m_state == CDBUser::dusAuthed || m_dbuser->m_state == CDBUser::dusLogout);

    LOG_PROCESS_ERROR(m_dbtask == DB_TASK_CREATE_ROLE);
    m_dbtask = DB_TASK_FREE;
    LOG_PROCESS_ERROR(m_lockstate == LOCK_STATE_INVALID);

    m_dwDeleteTime = 0;
    if (m_nResult == errSuccess)
    {
        CDBServer& dbserver = CDBServer::GetInstance();
        bRet = dbserver.m_rolemgr.AddDBRole(this);
        LOG_PROCESS_ERROR(bRet);

        m_lockstate = LOCK_STATE_UNLOCKED;
        bRet = dbserver.m_rolemgr.Push(this);
        LOG_PROCESS_ERROR(bRet);

        ROLE_DB_BASE_DATA* base_data = RefreshBaseData();
        LOG_PROCESS_ERROR(base_data);

        bRet = CDBMessageHandler::GetInstance().DoMSCreateRole(m_dbuser, base_data);
        LOG_PROCESS_ERROR(bRet);
    }
    else
    {
        bRet = m_dbuser->RemoveRole(this, TRUE);
        LOG_PROCESS_ERROR(bRet);

        if (m_dbuser->m_state == CDBUser::dusAuthed)
        {
            bRet = CDBMessageHandler::GetInstance().DoGTCreateRoleAck(m_dbuser->m_dwClientSessionID, m_nResult, NULL);
            LOG_PROCESS_ERROR(bRet);
        }
    }

    return;
Exit0:
    if (m_dbuser)
        CDBMessageHandler::GetInstance().DoGTCloseClientSession(m_dbuser->m_dwClientSessionID);
}

void CDBRole::OnUpdateRoleDeleteTime()
{
    BOOL bRet = FALSE;
    UINT32 dwRoleID = 0;

    LOG_PROCESS_ERROR(m_dbuser);
    LOG_PROCESS_ERROR(m_dbuser->m_dbstate == CDBUser::dtsUpdatingRoleDeleteTime);
    m_dbuser->m_dbstate = CDBUser::dtsFree;
    LOG_PROCESS_ERROR(m_dbuser->m_pPlayingRole == NULL);
    LOG_PROCESS_ERROR(m_dbuser->m_state == CDBUser::dusAuthed || m_dbuser->m_state == CDBUser::dusLogout);

    LOG_PROCESS_ERROR((m_dbtask == DB_TASK_UPDATE_ROLE_DELETE_TIME && m_lockstate == LOCK_STATE_UNLOCKED) ||
        (m_dbtask == DB_TASK_UPDATE_ROLE_DELETE_TIME_WITHOUT_DATA && m_lockstate == LOCK_STATE_INVALID));
    m_dbtask = DB_TASK_FREE;

    dwRoleID = m_dwRoleID;

    bRet = m_dbuser->RemoveRole(this, FALSE);
    LOG_PROCESS_ERROR(bRet);

    if (m_dwDeleteTime == 0)
    {
        bRet = CDBMessageHandler::GetInstance().DoGTUnDeleteRoleAck(m_dbuser->m_dwClientSessionID, m_nResult, dwRoleID);
        LOG_PROCESS_ERROR(bRet);
    }
    else
    {
        bRet = CDBMessageHandler::GetInstance().DoGTDeleteRoleAck(m_dbuser->m_dwClientSessionID, m_nResult, dwRoleID);
        LOG_PROCESS_ERROR(bRet);
    }

    return;

Exit0:
    if (m_dbuser)
        CDBMessageHandler::GetInstance().DoGTCloseClientSession(m_dbuser->m_dwClientSessionID);
}

void CDBRole::OnDeleteRole()
{
    BOOL bRet = FALSE;
    UINT32 dwRoleID = 0;

    LOG_PROCESS_ERROR(m_dbuser);
    LOG_PROCESS_ERROR(m_dbuser->m_dbstate == CDBUser::dtsDeleting);
    m_dbuser->m_dbstate = CDBUser::dtsFree;
    LOG_PROCESS_ERROR(m_dbuser->m_pPlayingRole == NULL);
    LOG_PROCESS_ERROR(m_dbuser->m_state == CDBUser::dusAuthed || m_dbuser->m_state == CDBUser::dusLogout);

    LOG_PROCESS_ERROR((m_dbtask == DB_TASK_DELETE_ROLE && m_lockstate == LOCK_STATE_UNLOCKED) ||
        (m_dbtask == DB_TASK_DELETE_ROLE_WITHOUT_DATA && m_lockstate == LOCK_STATE_INVALID));
    m_dbtask = DB_TASK_FREE;

    dwRoleID = m_dwRoleID;

    bRet = m_dbuser->RemoveRole(this, TRUE);
    LOG_PROCESS_ERROR(bRet);

    bRet = CDBMessageHandler::GetInstance().DoMSDeleteRole(m_dbuser->m_dwClientSessionID, dwRoleID);
    LOG_PROCESS_ERROR(bRet);

    return;

Exit0:
    if (m_dbuser)
        CDBMessageHandler::GetInstance().DoGTCloseClientSession(m_dbuser->m_dwClientSessionID);
}

void CDBRole::OnLoadRole()
{
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(m_dbuser);
    LOG_PROCESS_ERROR(m_dbuser->m_dbstate == CDBUser::dtsLoading);
    m_dbuser->m_dbstate = CDBUser::dtsFree;
    LOG_PROCESS_ERROR(m_dbuser->m_pPlayingRole == NULL);
    LOG_PROCESS_ERROR(m_dbuser->m_state == CDBUser::dusAuthed || m_dbuser->m_state == CDBUser::dusLogout);

    LOG_PROCESS_ERROR(m_dbtask == DB_TASK_LOAD_ROLE);
    m_dbtask = DB_TASK_FREE;
    LOG_PROCESS_ERROR(m_lockstate == LOCK_STATE_LOCKING || m_lockstate == LOCK_STATE_LOCKING_AND_UNLOCKING);

    if (m_nResult == errSuccess)
    {
        CDBServer& dbserver = CDBServer::GetInstance();
        bRet = dbserver.m_rolemgr.AddDBRole(this);
        LOG_PROCESS_ERROR(bRet);

        if (m_lockstate == LOCK_STATE_LOCKING)
        {
            bRet = dbserver.m_threads[m_dwRoleID%dbserver.m_dwThreadCount].AddRoleLockTask(this);
            LOG_PROCESS_ERROR(bRet);

            m_qwLastSaveTime = GetTickCountEx();

            m_dbuser->m_pPlayingRole = this;
            m_dbuser->m_state = CDBUser::dusPlaying;

            bRet = CDBMessageHandler::GetInstance().DoGSLoadLockRoleDataAck(m_dwGSSessionID, m_dbuser->m_dwClientSessionID, m_dwRoleID, 
                m_pCurMem->CtrlNode.dwDataSize, m_pCurMem->RoleData);
            LOG_PROCESS_ERROR(bRet);
        }
        else
        {
            m_lockstate = LOCK_STATE_UNLOCKED;
            bRet = dbserver.m_rolemgr.Push(this);
            LOG_PROCESS_ERROR(bRet);

            bRet = RefreshBaseData() != NULL;
            LOG_PROCESS_ERROR(bRet);
        }
    }
    else
    {
        //Database read failed
        m_lockstate = LOCK_STATE_INVALID;
        m_dbuser->RemoveRole(this, FALSE);
        LOG_PROCESS_ERROR(FALSE);
    }

    return;

Exit0:
    if (m_dbuser)
        CDBMessageHandler::GetInstance().DoGTCloseClientSession(m_dbuser->m_dwClientSessionID);
}

ROLE_DB_BASE_DATA* CDBRole::RefreshBaseData()
{
    ROLE_DB_DATA* pRoleData = nullptr;
    ROLE_DB_BASE_DATA* pBaseData = nullptr;
    INT32 rolepos = 0;
    LOG_PROCESS_ERROR(m_dbuser);
    LOG_PROCESS_ERROR(m_pCurMem);
    pRoleData = (ROLE_DB_DATA*)m_pCurMem->RoleData;
    pBaseData = (ROLE_DB_BASE_DATA*)(pRoleData->RoleData + pRoleData->BlockTable[rbtBase].dwStartOffset);

    pBaseData->dwDeleteTime = m_dwDeleteTime;

    for (; rolepos < MAX_ROLE_PER_USER; rolepos++)
    {
        if (m_dbuser->m_pDBRole[rolepos] == this)
        {
            break;
        }
    }
    LOG_PROCESS_ERROR(rolepos < MAX_ROLE_PER_USER);

    if (pBaseData->byVersion == ROLE_DB_BASE_DATA_CURRENT_VERSION)
    {
        LOG_PROCESS_ERROR(pRoleData->BlockTable[rbtBase].dwBlockSize == sizeof(ROLE_DB_BASE_DATA));
        memcpy(&m_dbuser->m_BaseData[rolepos], pBaseData, sizeof(ROLE_DB_BASE_DATA));
    }
    else
    {
        BOOL bRet = ConvertToBaseDataCurrentVersion(&m_dbuser->m_BaseData[rolepos],
            pRoleData->RoleData + pRoleData->BlockTable[rbtBase].dwStartOffset,
            pRoleData->BlockTable[rbtBase].dwBlockSize);
        LOG_PROCESS_ERROR(bRet);
    }

    return &m_dbuser->m_BaseData[rolepos];
Exit0:
    return NULL;
}

BOOL CDBRole::ClearCache(BOOL include_current)
{
    while (m_MemList.Check() != m_pCurMem)
    {
        void* pMem = m_MemList.Pop();
        LOG_PROCESS_ERROR(pMem);
        CDBServer::GetInstance().m_memmgr.Push((DB_ROLE_DATA*)pMem);
    }

    if (include_current)
    {
        void* pMem = m_MemList.Pop();
        LOG_PROCESS_ERROR(pMem);
        CDBServer::GetInstance().m_memmgr.Push((DB_ROLE_DATA*)pMem);

        m_pCurMem = NULL;

        LOG_PROCESS_ERROR(m_MemList.IsEmpty());
    }

    return TRUE;

Exit0:
    return FALSE;
}
