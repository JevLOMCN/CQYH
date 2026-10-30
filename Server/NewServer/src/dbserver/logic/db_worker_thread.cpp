#include "stdafx.h"
#include "db_worker_thread.h"
#include "worker_queue.h"

#include <time.h>

#include "mysql.h"
#include "mysqld_error.h"
#include "errmsg.h"
#include "zlib.h"

#include "time_ex.h"
#include "mem_mgr.h"
#include "new_role_config.h"
#include "game_world_config.h"
#include "scene_def.h"
#include "char_def.h"
#include "dbserver.h"
#include "report.h"

static UINT64 TASK_TIME_LIMIT_TOTAL = 500;
static UINT64 TASK_TIME_LIMIT_INDIVIDUAL = 250;

CDBWorkerThread::CDBWorkerThread()
{
    m_mysql = cqyh::MysqlPtr{};
    m_bServerOK = FALSE;
}

CDBWorkerThread::~CDBWorkerThread()
{
    UnInit();
}

BOOL CDBWorkerThread::Init(UINT32 dwRoleCount)
{
    if (m_mysql || m_bstart)
    {
        LOG_CHECK_ERROR(!m_mysql && !m_bstart);
        return FALSE;
    }
    const DBSetting* dbsetting;
    BOOL bRet = FALSE;
    MYSQL* res_mysql = NULL;

    bRet = m_RoleDbTaskList.Init(dwRoleCount);
    LOG_PROCESS_ERROR(bRet);

    bRet = m_FinishedRoleDbTaskList.Init(dwRoleCount);
    LOG_PROCESS_ERROR(bRet);

    bRet = m_RoleLockTaskList.Init(dwRoleCount);
    LOG_PROCESS_ERROR(bRet);

    bRet = m_FinishedRoleLockTaskList.Init(dwRoleCount);
    LOG_PROCESS_ERROR(bRet);

    bRet = m_UserTaskList.Init(dwRoleCount);
    LOG_PROCESS_ERROR(bRet);

    bRet = m_FinishedUserTaskList.Init(dwRoleCount);
    LOG_PROCESS_ERROR(bRet);

    m_mysql = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(m_mysql.get());

    dbsetting = CDBServer::GetInstance().GetDBSetting();

    res_mysql = mysql_real_connect(
        m_mysql.get(),
        dbsetting->szServerAddr,
        dbsetting->szLoginName,
        dbsetting->szPassword,
        dbsetting->szDBName,
        dbsetting->dwServerPort,
        NULL,
        0);
    LOG_PROCESS_ERROR(res_mysql);

    if (strlen(dbsetting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(m_mysql.get(), dbsetting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    bRet = m_createrole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_deleterole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_updateroledeletetime.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_loadrole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_saverole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_listrole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    m_bServerOK = TRUE;

//test begin
    //{
    //    char buf[DATA_SIZE];
    //    UINT32 filesize = 0;
    //    FILE* f = fopen("480742.dat", "rb");
    //    LOG_PROCESS_ERROR(f);

    //    filesize = fread(buf, 1, DATA_SIZE, f);
    //    LOG_PROCESS_ERROR(filesize);

    //    char user_name[USER_NAME_LEN];
    //    UINT32 roleid;
    //    ROLE_DB_DATA* pRoleData = (ROLE_DB_DATA*)buf;
    //    ROLE_DB_BASE_DATA* pBaseData = (ROLE_DB_BASE_DATA*)(pRoleData->RoleData + pRoleData->BlockTable[rbtBase].dwStartOffset);
    //    


    //    for (UINT32 i=1; i<=8000000; i++)
    //    {
    //        pRoleData->dwRoleID = i;
    //        pBaseData->dwRoleID = i;
    //        sprintf(user_name, "dbtest%d", i);
    //        strxcpy(pRoleData->szRoleName, user_name, ROLE_NAME_LEN);
    //        strxcpy(pBaseData->szRoleName, user_name, ROLE_NAME_LEN);

    //        pRoleData->dwCRC = SGDP::SDCRC(&pRoleData->dwVersion, filesize - sizeof(pRoleData->dwLen) - sizeof(pRoleData->dwCRC));
    //        
    //        bRet = m_createrole.DoCreateRole(1,
    //            user_name,
    //            user_name,
    //            "pt",
    //            roleid,
    //            (char*)pBaseData,
    //            sizeof(ROLE_DB_BASE_DATA),
    //            (char*)pRoleData,
    //            filesize);

    //        if (i%1000 ==0)
    //            DBG("%d roles created", i);
    //    }
    //}
//test end



    return TRUE;

Exit0:
    if (m_mysql.get())
        CRI("mysql error number: %d, %s", mysql_errno(m_mysql.get()), mysql_error(m_mysql.get()));

    UnInit();
    return FALSE;
}

BOOL CDBWorkerThread::UnInit()
{
    Wait();
    BOOL bRet = FALSE;

    bRet = m_RoleDbTaskList.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_FinishedRoleDbTaskList.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_RoleLockTaskList.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_FinishedRoleLockTaskList.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_UserTaskList.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_FinishedUserTaskList.UnInit();
    LOG_CHECK_ERROR(bRet);

    CloseStatements();
    m_mysql = cqyh::MysqlPtr{};

    m_bServerOK = FALSE;
    return TRUE;
}

void CDBWorkerThread::CloseStatements()
{
    LOG_CHECK_ERROR(m_createrole.UnInit());
    LOG_CHECK_ERROR(m_deleterole.UnInit());
    LOG_CHECK_ERROR(m_updateroledeletetime.UnInit());
    LOG_CHECK_ERROR(m_loadrole.UnInit());
    LOG_CHECK_ERROR(m_saverole.UnInit());
    LOG_CHECK_ERROR(m_listrole.UnInit());
}

BOOL CDBWorkerThread::ReInit()
{
    BOOL bRet = FALSE;
    bool replaced = false;
    const DBSetting* dbsetting = CDBServer::GetInstance().GetDBSetting();

    INF("DB Failure, Reconnecting...");

    auto new_mysql = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(new_mysql.get());

    {
        MYSQL* res_mysql = mysql_real_connect(
            new_mysql.get(),
            dbsetting->szServerAddr,
            dbsetting->szLoginName,
            dbsetting->szPassword,
            dbsetting->szDBName,
            dbsetting->dwServerPort,
            NULL,
            0);
        LOG_PROCESS_ERROR(res_mysql);
    }

    if (strlen(dbsetting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(new_mysql.get(), dbsetting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    CloseStatements();
    m_mysql = std::move(new_mysql);
    replaced = true;

    bRet = m_createrole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_deleterole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_updateroledeletetime.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_loadrole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_saverole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    bRet = m_listrole.Init(m_mysql.get());
    LOG_PROCESS_ERROR(bRet);

    INF("DB Reconnection Succeeded!!!");
    m_bServerOK = TRUE;
    return TRUE;

Exit0:
    if (replaced)
    {
        CloseStatements();
        m_mysql = cqyh::MysqlPtr{};
        m_bServerOK = FALSE;
    }
    INF("DB Reconnection Failed.");
    return FALSE;
}

BOOL CDBWorkerThread::AddRoleDbTask(CDBRole* dbrole)
{
    if (!m_bstart.load() || !IsRunning())
        return FALSE;
    BOOL bRet = m_RoleDbTaskList.Push(dbrole);
    LOG_CHECK_ERROR(bRet);

    return bRet;
}

CDBRole* CDBWorkerThread::GetFinishedRoleDbTask()
{
    return (CDBRole*)m_FinishedRoleDbTaskList.Pop();
}

BOOL CDBWorkerThread::AddRoleLockTask(CDBRole* dbrole)
{
    if (!m_bstart.load() || !IsRunning())
        return FALSE;
    BOOL bRet = m_RoleLockTaskList.Push(dbrole);
    LOG_CHECK_ERROR(bRet);

    return bRet;
}

CDBRole* CDBWorkerThread::GetFinishedRoleLockTask()
{
    return (CDBRole*)m_FinishedRoleLockTaskList.Pop();
}

BOOL CDBWorkerThread::AddUserTask(CDBUser* dbuser)
{
    if (!m_bstart.load() || !IsRunning())
        return FALSE;
    BOOL bRet = m_UserTaskList.Push(dbuser);
    LOG_CHECK_ERROR(bRet);

    return bRet;
}

CDBUser* CDBWorkerThread::GetFinishedUserTask()
{
    return (CDBUser*)m_FinishedUserTaskList.Pop();
}

bool CmpRole(const CDBRole* r1, const CDBRole* r2)
{
    if (r1->m_qwLastSaveTime == r2->m_qwLastSaveTime)
        return r1->m_dwRoleID < r2->m_dwRoleID;
    else
        return r1->m_qwLastSaveTime < r2->m_qwLastSaveTime;
}

VOID CDBWorkerThread::ThrdProc()
{
    cqyh::MysqlThreadContext clientThread;
    if (!clientThread)
    {
        LOG_CHECK_ERROR(clientThread);
        return;
    }
    INT32 nRet = errSuccess;
    BOOL bRet = FALSE;
    UINT32 roles_saved;
    std::list<CDBRole*> roles_to_save;
    UINT64 starttime, listtime, loadtime, locktime, traversetime, sorttime, savetime;
    UINT64 this_task_start_time, this_task_finish_time;
    UINT32 listroles, loadroles, lockroles;

    //test begin
    //{
    //    static CSDRandom sdrandom;
    //    char buf[DATA_SIZE];
    //    UINT32 outsize=0;
    //    UINT32 testroleid[1000];
    //    for (int i=0; i<1000; i++)
    //    {
    //        testroleid[i] = sdrandom.Random(1,8000000);
    //    }
    //    UINT64 qwStarttime = GetTickCountEx();
    //    for (int i=0; i<1000; i++)
    //    {
    //        m_loadrole.m_dwRoleID = testroleid[i];
    //        bRet = m_loadrole.DoLoadRole(buf, DATA_SIZE, outsize);
    //        LOG_CHECK_ERROR(bRet);
    //        m_saverole.m_dwRoleID = testroleid[i];
    //        bRet = m_saverole.DoSaveRole(buf, outsize);
    //        LOG_CHECK_ERROR(bRet);
    //    }
    //    DBG("test complete in %d ms", GetTickCountEx()-qwStarttime);
    //}
    //test end

    for (; m_bstart; )
    {
        if (!m_bServerOK)
            ReInit();

        starttime = GetTickCountEx();

        listroles = 0;
        for (;;)
        {
            CDBUser* dbuser = (CDBUser*)m_UserTaskList.Check();

            if (dbuser == NULL)
                break;

            this_task_start_time = GetTickCountEx();

            nRet = ListRole(dbuser);
            LOG_CHECK_ERROR(nRet == errSuccess);
            dbuser->m_nResult = nRet;
            if (!cqyh::PublishUntilStopped(m_FinishedUserTaskList, dbuser, *this))
                goto FinishWorker;
            m_UserTaskList.Pop();
            listroles++;

            this_task_finish_time = GetTickCountEx();

            if (this_task_finish_time - this_task_start_time > TASK_TIME_LIMIT_INDIVIDUAL)
                REPORT(1, "List Cost %I64ums", this_task_finish_time - this_task_start_time);

            if (this_task_finish_time - starttime >= TASK_TIME_LIMIT_TOTAL)
                break;
        }
        listtime = GetTickCountEx();

        loadroles = 0;
        for (;;)
        {
            CDBRole* dbrole = (CDBRole*)m_RoleDbTaskList.Check();

            if (dbrole == NULL)
                break;

            this_task_start_time = GetTickCountEx();

            switch (dbrole->m_dbtask)
            {
            case CDBRole::DB_TASK_LOAD_ROLE:
                nRet = LoadRole(dbrole->m_dwRoleID, dbrole->m_pCurMem);
                LOG_CHECK_ERROR(nRet == errSuccess);
                dbrole->m_nResult = nRet;
                if (!cqyh::PublishUntilStopped(m_FinishedRoleDbTaskList, dbrole, *this))
                    goto FinishWorker;
                m_RoleDbTaskList.Pop();
                break;
            case CDBRole::DB_TASK_CREATE_ROLE:    // Create character
                nRet = CreateRole(dbrole);
                LOG_CHECK_ERROR(nRet == errSuccess);
                dbrole->m_nResult = nRet;
                if (!cqyh::PublishUntilStopped(m_FinishedRoleDbTaskList, dbrole, *this))
                    goto FinishWorker;
                m_RoleDbTaskList.Pop();
                break;
            case CDBRole::DB_TASK_UPDATE_ROLE_DELETE_TIME:
            case CDBRole::DB_TASK_UPDATE_ROLE_DELETE_TIME_WITHOUT_DATA:
                nRet = UpdateRoleDeleteTime(dbrole);
                LOG_CHECK_ERROR(nRet == errSuccess);
                dbrole->m_nResult = nRet;
                if (!cqyh::PublishUntilStopped(m_FinishedRoleDbTaskList, dbrole, *this))
                    goto FinishWorker;
                m_RoleDbTaskList.Pop();
                break;
            case CDBRole::DB_TASK_DELETE_ROLE:
            case CDBRole::DB_TASK_DELETE_ROLE_WITHOUT_DATA:
                nRet = DeleteRole(dbrole);
                LOG_CHECK_ERROR(nRet == errSuccess);
                dbrole->m_nResult = nRet;
                if (!cqyh::PublishUntilStopped(m_FinishedRoleDbTaskList, dbrole, *this))
                    goto FinishWorker;
                m_RoleDbTaskList.Pop();
                break;
            default:
                LOG_CHECK_ERROR(FALSE);
                m_RoleDbTaskList.Pop();
            }
            loadroles++;

            this_task_finish_time = GetTickCountEx();

            if (this_task_finish_time - this_task_start_time > TASK_TIME_LIMIT_INDIVIDUAL)
                REPORT(1, "Load/Create/Delete Cost %I64ums", this_task_finish_time - this_task_start_time);

            if (this_task_finish_time - listtime >= TASK_TIME_LIMIT_TOTAL)
                break;
        }
        loadtime = GetTickCountEx();

        lockroles = 0;
        for (;;)
        {
            CDBRole* dbrole = (CDBRole*)m_RoleLockTaskList.Check();

            if (dbrole == NULL)
                break;

            this_task_start_time = GetTickCountEx();

            switch (dbrole->m_lockstate)
            {
            case CDBRole::LOCK_STATE_LOCKING:
            case CDBRole::LOCK_STATE_LOCKING_AND_UNLOCKING:
                bRet = m_roles.insert(dbrole).second;
                LOG_CHECK_ERROR(bRet);
                if (!cqyh::PublishUntilStopped(m_FinishedRoleLockTaskList, dbrole, *this))
                    goto FinishWorker;
                m_RoleLockTaskList.Pop();
                break;
            case CDBRole::LOCK_STATE_UNLOCKING:
            case CDBRole::LOCK_STATE_UNLOCKING_AND_LOCKING:
                bRet = SaveIfNeeded(dbrole);
                LOG_CHECK_ERROR(bRet);
                bRet = m_roles.erase(dbrole) > 0;
                LOG_CHECK_ERROR(bRet);
                if (!cqyh::PublishUntilStopped(m_FinishedRoleLockTaskList, dbrole, *this))
                    goto FinishWorker;
                m_RoleLockTaskList.Pop();
                break;
            default:
                LOG_CHECK_ERROR(FALSE);
                m_RoleLockTaskList.Pop();
            }
            lockroles++;

            this_task_finish_time = GetTickCountEx();

            if (this_task_finish_time - this_task_start_time > TASK_TIME_LIMIT_INDIVIDUAL)
                REPORT(1, "Lock/Unlock Cost %I64ums", this_task_finish_time - this_task_start_time);

            if (this_task_finish_time - loadtime >= TASK_TIME_LIMIT_TOTAL)
                break;
        }
        locktime = GetTickCountEx();

        roles_saved = 0;
        roles_to_save.clear();
        CDBRole* dbrole;
        for (std::set<CDBRole*>::iterator it=m_roles.begin(); it!=m_roles.end(); ++it)
        {
            dbrole = *it;
            dbrole->ClearCache(FALSE);
            if (dbrole->m_pCurMem->CtrlNode.LastModified > dbrole->m_pCurMem->CtrlNode.LastSaved)
            {
                roles_to_save.push_back(dbrole);
            }
        }
        traversetime = GetTickCountEx();

        sorttime = traversetime;
        if (!roles_to_save.empty())
        {
            roles_to_save.sort(CmpRole);
            sorttime = GetTickCountEx();
            for (std::list<CDBRole*>::iterator it=roles_to_save.begin(); it!=roles_to_save.end(); ++it)
            {
                this_task_start_time = GetTickCountEx();

                bRet = SaveIfNeeded(*it);
                LOG_CHECK_ERROR(bRet);

                roles_saved++;

                this_task_finish_time = GetTickCountEx();

                if (this_task_finish_time - this_task_start_time > TASK_TIME_LIMIT_INDIVIDUAL)
                    REPORT(1, "Save Cost %I64ums", this_task_finish_time - this_task_start_time);

                if (this_task_finish_time - sorttime >= TASK_TIME_LIMIT_TOTAL)
                    break;
            }
        }
        savetime = GetTickCountEx();

        if (savetime-starttime>500)
        {
            INF("List: %d/%dms, Load/Create/Delete %d/%dms, Lock/Unlock %d/%dms, Traverse %d/%dms, Sort %d/%dms, Save %d/%dms, total %dms",
                listroles, listtime-starttime,
                loadroles, loadtime-listtime,
                lockroles, locktime-loadtime,
                m_roles.size(), traversetime-locktime,
                roles_to_save.size(), sorttime-traversetime,
                roles_saved, savetime-sorttime,
                savetime-starttime);
        }

        WaitForStop(std::chrono::milliseconds(16));
    }

FinishWorker:
    for (std::set<CDBRole*>::iterator it=m_roles.begin(); it!=m_roles.end(); ++it)
    {
        bRet = SaveIfNeeded(*it);
        LOG_CHECK_ERROR(bRet);
    }
}

INT32 CDBWorkerThread::CreateRole(CDBRole* dbrole)
{
	if (!m_bServerOK)
        return errDatabase;

    BOOL bRet = FALSE;
    ROLE_DB_DATA* role_data = NULL;
    ROLE_DB_BASE_DATA* role_base_data = NULL;
    UINT32 data_len = 0;
    UINT32 base_data_len = 0;
    NEW_ROLE_CONFIG::NEW_EQUIP* equips = NULL;

    LOG_PROCESS_ERROR(dbrole);
    LOG_PROCESS_ERROR(dbrole->m_pCurMem);

    memset(dbrole->m_pCurMem->RoleData, 0, DATA_SIZE);
    role_data = (ROLE_DB_DATA*)dbrole->m_pCurMem->RoleData;
    role_base_data = (ROLE_DB_BASE_DATA*)&role_data->RoleData[0];
    data_len = sizeof(ROLE_DB_DATA) + sizeof(ROLE_DB_BASE_DATA);
    base_data_len = sizeof(ROLE_DB_BASE_DATA);

    role_data->dwLen = data_len;
    role_data->dwVersion = ROLE_DB_DATA_CURRENT_VERSION;
    role_data->dwRoleID = dbrole->m_dwRoleID;
    role_data->BlockTable[rbtBase].btBlockType = rbtBase;
    role_data->BlockTable[rbtBase].dwBlockSize = base_data_len;

    role_base_data->byVersion = ROLE_DB_BASE_DATA_CURRENT_VERSION;
    role_base_data->dwRoleID = dbrole->m_dwRoleID;

	role_base_data->dwCreateTime = role_base_data->dwSaveTime = (UINT32)time(NULL);
    role_base_data->dwDeleteTime = 0;

    strxcpy(role_base_data->szRoleName, dbrole->m_CreateRoleParam.szRoleName, ROLE_NAME_LEN);
	strxcpy(role_base_data->szUserName, dbrole->m_CreateRoleParam.szUserName, USER_NAME_LEN);
    role_base_data->byClass = dbrole->m_CreateRoleParam.byClass;
    role_base_data->bySex = dbrole->m_CreateRoleParam.bySex;
    role_base_data->byHairType = dbrole->m_CreateRoleParam.byHairType;
    role_base_data->byHairColor = dbrole->m_CreateRoleParam.byHairColor;
    role_base_data->byFaceType = dbrole->m_CreateRoleParam.byFaceType;
    role_base_data->bySkinColor = dbrole->m_CreateRoleParam.bySkinColor;

    role_base_data->nSceneTempID = g_NewRoleConfig.nSceneTempID;
	role_base_data->nSceneID = 0;
    role_base_data->nX = g_NewRoleConfig.nX * POINT_PER_CM;
    role_base_data->nY = g_NewRoleConfig.nY * POINT_PER_CM;
    role_base_data->nZ = g_NewRoleConfig.nZ * POINT_PER_CM;
    role_base_data->wFaceDirection = g_NewRoleConfig.wFaceDirection;

    LOG_PROCESS_ERROR(dbrole->m_CreateRoleParam.bySex == cstMale || dbrole->m_CreateRoleParam.bySex == cstFemale);
    LOG_PROCESS_ERROR(dbrole->m_CreateRoleParam.byClass < clsPlayerEnd);

    if (dbrole->m_CreateRoleParam.bySex == cstMale)
        equips = &g_NewRoleConfig.MaleNewEquip[dbrole->m_CreateRoleParam.byClass];
    else
        equips = &g_NewRoleConfig.FemaleNewEquip[dbrole->m_CreateRoleParam.byClass];

    role_base_data->nAvatar[NES_MAIN_HAND] = equips->nMainHand;
    role_base_data->nAvatar[NES_BODY] = equips->nBody;
    role_base_data->nAvatar[NES_BACK] = equips->nBack;

    role_base_data->byRefineFx = 0;

    role_data->dwCRC = crc32(0, (const Bytef*)&role_data->dwVersion, data_len - sizeof(role_data->dwLen) - sizeof(role_data->dwCRC));

    bRet = m_createrole.DoCreateRole((UINT8)dbrole->m_dbuser->m_nServerGroup,
        dbrole->m_dwRoleID,
        dbrole->m_CreateRoleParam.szRoleName,
        dbrole->m_CreateRoleParam.szUserName,
        (char*)role_base_data,
        base_data_len,
        (char*)role_data,
        data_len);

    dbrole->m_pCurMem->CtrlNode.dwDataSize = data_len;
    dbrole->m_pCurMem->CtrlNode.LastModified = 0;
    dbrole->m_pCurMem->CtrlNode.LastSaved = 0;
    dbrole->m_pCurMem->CtrlNode.Reading = false;

    if (bRet)
        return errSuccess;
    else
    {
        if (mysql_errno(m_mysql.get()) == ER_DUP_ENTRY)
            return errLoginCreateRoleDupeName;
        else
        {
            if (mysql_stmt_errno(m_createrole.m_stmt.get()) == CR_SERVER_LOST)
            {
                m_bServerOK = FALSE;
            }
            return errDatabase;
        }
    }

Exit0:
    return errUnknownError;
}

INT32 CDBWorkerThread::DeleteRole(CDBRole* dbrole)
{
    if (!m_bServerOK)
        return errDatabase;

    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(dbrole);

    m_deleterole.m_dwRoleID = dbrole->m_dwRoleID;
    bRet = m_deleterole.DoDeleteRole();

    if (bRet)
        return errSuccess;
    else
    {
        if (mysql_stmt_errno(m_deleterole.m_stmt.get()) == CR_SERVER_LOST)
        {
            m_bServerOK = FALSE;
        }
        return errDatabase;
    }

Exit0:
    return errUnknownError;
}

INT32 CDBWorkerThread::UpdateRoleDeleteTime(CDBRole* dbrole)
{
    if (!m_bServerOK)
        return errDatabase;

    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(dbrole);

    m_updateroledeletetime.m_dwRoleID = dbrole->m_dwRoleID;
    m_updateroledeletetime.m_dwDeleteTime = dbrole->m_dwDeleteTime;
    bRet = m_updateroledeletetime.DoUpdate();

    if (bRet)
        return errSuccess;
    else
    {
        if (mysql_stmt_errno(m_deleterole.m_stmt.get()) == CR_SERVER_LOST)
        {
            m_bServerOK = FALSE;
        }
        return errDatabase;
    }

Exit0:
    return errUnknownError;
}

INT32 CDBWorkerThread::SaveRole(UINT32 dwRoleID, DB_ROLE_DATA* data)
{
    if (!m_bServerOK)
        return errDatabase;

    BOOL bRet = FALSE;

    m_saverole.m_dwRoleID = dwRoleID;
    bRet = m_saverole.DoSaveRole(data->RoleData, data->CtrlNode.dwDataSize);

    if (bRet)
    {
        data->CtrlNode.LastSaved = GetTickCountEx();
        return errSuccess;
    }
    else
    {
        if (mysql_stmt_errno(m_saverole.m_stmt.get()) == CR_SERVER_LOST)
        {
            m_bServerOK = FALSE;
        }
        return errDatabase;
    }
}

INT32 CDBWorkerThread::LoadRole(UINT32 dwRoleID, DB_ROLE_DATA* data)
{
    if (!m_bServerOK)
        return errDatabase;

    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(data);

    m_loadrole.m_dwRoleID = dwRoleID;
    bRet = m_loadrole.DoLoadRole(data->RoleData, DATA_SIZE, data->CtrlNode.dwDataSize);

    if (bRet)
    {
        data->CtrlNode.LastModified = GetTickCountEx();
        data->CtrlNode.LastSaved = data->CtrlNode.LastModified;
        return errSuccess;
    }
    else
    {
        if (mysql_stmt_errno(m_loadrole.m_stmt.get()) == CR_SERVER_LOST)
        {
            m_bServerOK = FALSE;
        }
        return errDatabase;
    }

Exit0:
    return errUnknownError;
}

INT32 CDBWorkerThread::ListRole(CDBUser* dbuser)
{
    if (!m_bServerOK)
        return errDatabase;

    BOOL bRet = m_listrole.DoListRole(dbuser);
    if (bRet)
        return errSuccess;
    else
    {
        if (mysql_stmt_errno(m_listrole.m_stmt.get()) == CR_SERVER_LOST)
        {
            m_bServerOK = FALSE;
        }
        return errDatabase;
    }
}

BOOL CDBWorkerThread::SaveIfNeeded(CDBRole* dbrole)
{
    LOG_PROCESS_ERROR(dbrole);
    LOG_PROCESS_ERROR(dbrole->m_pCurMem);

    if (dbrole->m_pCurMem->CtrlNode.LastModified > dbrole->m_pCurMem->CtrlNode.LastSaved)
    {
        INT32 nRetCode = SaveRole(dbrole->m_dwRoleID, dbrole->m_pCurMem);
        LOG_PROCESS_ERROR(nRetCode == errSuccess);

        dbrole->ClearCache(FALSE);
        dbrole->m_qwLastSaveTime = GetTickCountEx();
    }

    return TRUE;

Exit0:
    return FALSE;
}
