#include "stdafx.h"
#include "master_mgr.h"

#include <algorithm>
#include <time.h>
#include "time_ex.h"
#include "game_world_config.h"
#include "server_config.h"

#include "variable_def.h"

#include "manageserver.h"
#include "role_data.h"
#include "message_handler.h"
#include "./onlinerole/online_role_data_mgr.h"
#include "role_data_mgr.h"
#include "mail/mail_mgr.h"
#include "master_gift.h"
#include "client/user.h"
#include "mail_template_data.h"
#include "../db/prep_stmt.h"
#include "obj_pool_config.h"

#include "./db/ms_sql_stmt_list.h"
#include "sql_mgr.h"

CMasterMgr CMasterMgr::ms_Instance_;

CMasterMgr::CMasterMgr()
{

}

CMasterMgr::~CMasterMgr()
{

}

CMasterMgr& CMasterMgr::GetInstance(void)
{
    return ms_Instance_;
}

BOOL CMasterMgr::Init()
{
    INT32 nRetCode = 0;

    for (INT32 nIndex = 0; nIndex < MAX_SERVER_GROUP_COUNT; ++nIndex)
    {
        m_RecruitList[nIndex].clear();
        m_CursorRoleIter[nIndex] = m_RecruitList[nIndex].end();
    }

    for (INT32 nIndex = 0; nIndex < MAX_SERVER_GROUP_COUNT; ++nIndex)
    {
        m_SoloList[nIndex].clear();
        m_SoloCursorIter[nIndex] = m_SoloList[nIndex].end();
    }

    for (INT32 nIndex = 0; nIndex < MAX_SERVER_GROUP_COUNT; ++nIndex)
    {
        m_TwitterList[nIndex].Clear();
    }

    nRetCode = m_TeacherPool.Init(g_ObjPoolConfig.nMasterTeacherPoolCount);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_StudentPool.Init(g_ObjPoolConfig.nMasterStudentPoolCount);
    LOG_PROCESS_ERROR(nRetCode);

    m_mapCZ.clear();
    m_mapTeacher.clear();
    m_mapStudent.clear();

    //nRetCode = _LoadMasterRelation();
    //LOG_PROCESS_ERROR(nRetCode);

    {
        // clear destroy guild before  **-**-**
        UINT32  dwCurrentTime = CManageServer::GetInstance().GetGameTime();
        dwCurrentTime -= g_GWConfig.nWashoutDeletedMasterTime;
        dwCurrentTime /= 86400;
        dwCurrentTime *= 86400;

        nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlClearMaster, NULL, NULL, "ud", dwCurrentTime);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlClearMasterCZ, NULL, NULL, "ud", dwCurrentTime);
        LOG_PROCESS_ERROR(nRetCode);
    }


    nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectMaster, LoadMaster, NULL, "");
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectMasterCZ, LoadMasterCZ, NULL, "");
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::Uninit()
{
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// master
//////////////////////////////////////////////////////////////////////////

BOOL CMasterMgr::BeMaster(MS_ROLE_DATA *pRole)
{
    INT32       nRetCode = 0;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_ONLINE_ROLE_DATA *pOnlineRoleData = NULL;

    LOG_PROCESS_ERROR(pRole);

    nRetCode = CMasterMgr::GetInstance().IsStudent(pRole->dwRoleID);
    LOG_PROCESS_ERROR(!nRetCode);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pRole->dwRoleID);
    LOG_PROCESS_ERROR(!nRetCode);

    LOG_PROCESS_ERROR(pRole->byLevel >= GRADUATE_LEVEL);

    pTeacherData = m_TeacherPool.New();
    LOG_PROCESS_ERROR(pTeacherData);

    nRetCode = DelSolo(pRole->byGroup, pRole->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);
    
    nRetCode = _AddMasterRelationToMysql(pRole->byGroup, pRole->dwRoleID, pRole->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    memset(pTeacherData, 0, sizeof(*pTeacherData));

    pTeacherData->dwRoleID = pRole->dwRoleID;

    m_mapTeacher.insert(std::make_pair(pRole->dwRoleID, pTeacherData));

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pRole->dwRoleID);
    if (pOnlineRoleData)
    {
        pOnlineRoleData->dwMasterCD = 0;
        pOnlineRoleData->bTwitter = FALSE;
        nRetCode = CMessageHandler::GetInstance().DoGSMstUpdateData(pOnlineRoleData->pRoleData->dwRoleID, MST_SYNC_GS_CD, pOnlineRoleData->dwMasterCD);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = AddRecruit(*pOnlineRoleData);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CMessageHandler::GetInstance().DoGSMstSyncState(pOnlineRoleData->dwGSSessionID, pOnlineRoleData->pRoleData->dwRoleID, MASTER_BECOME_TEACHER);
        LOG_CHECK_ERROR(nRetCode);
        nRetCode = CMessageHandler::GetInstance().DoClientMasterInfo(pOnlineRoleData->dwCliSessionID, pRole->dwRoleID, *pTeacherData);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    if (pTeacherData)
        m_TeacherPool.Delete(pTeacherData);
    return FALSE;
}

INT32 CMasterMgr::CreateMasterRelation(MS_ONLINE_ROLE_DATA &teacher, MS_ONLINE_ROLE_DATA &student)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    BOOL        bRet = FALSE;
    MS_ROLE_DATA    *pClassmate = NULL;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_STUDENT_DATA     *pStudentData = NULL;
    INT32       nLevel[MAX_STUDENT_NUM] = { 0 };

    LOG_PROCESS_ERROR(teacher.pRoleData);
    LOG_PROCESS_ERROR(student.pRoleData);

    nRetCode = IsStudent(student.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(!nRetCode);

    pTeacherData = GetTeacherData(teacher.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(pTeacherData);

    nRetCode = IsStudentFull(teacher);
    LOG_PROCESS_ERROR(!nRetCode);

    LOG_PROCESS_ERROR(pTeacherData->byStudNum < MAX_STUDENT_NUM);

    // check student role data
    for (INT32 nIndex = 0; nIndex < pTeacherData->byStudNum; ++nIndex)
    {
        pClassmate = CRoleDataMgr::GetInstance().FindRole(pTeacherData->arrStudentID[nIndex]);
        LOG_PROCESS_ERROR(pClassmate);
        nLevel[nIndex] = pClassmate->byLevel;
    }

    pStudentData = m_StudentPool.New();
    LOG_PROCESS_ERROR(pStudentData);

    memset(pStudentData, 0, sizeof(MS_STUDENT_DATA));
    pStudentData->dwRoleID = student.pRoleData->dwRoleID;
    pStudentData->dwTeacherID = teacher.pRoleData->dwRoleID;

    nRetCode = CMasterMgr::GetInstance().DelSolo(student.pRoleData->byGroup, student.pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    // Remove congratulations between fellow students
    //for (INT32 nIndex = 0; nIndex < pTeacherData->byStudNum; ++nIndex)
    //{
    //    if (pTeacherData->arrStudentID[nIndex] != student.pRoleData->dwRoleID)
    //    {
    //        AddCZ(teacher.pRoleData->byGroup, pTeacherData->arrStudentID[nIndex], student.pRoleData->dwRoleID, nLevel[nIndex]);
    //        AddCZ(teacher.pRoleData->byGroup, student.pRoleData->dwRoleID, pTeacherData->arrStudentID[nIndex], student.pRoleData->byLevel);
    //    }
    //}
    m_mapStudent.insert(std::make_pair(pStudentData->dwRoleID, pStudentData));

    pTeacherData->arrStudentID[pTeacherData->byStudNum++] = student.pRoleData->dwRoleID;

    // insert data to mysql
    bRet = _AddMasterRelationToMysql(teacher.pRoleData->byGroup, teacher.pRoleData->dwRoleID, student.pRoleData->dwRoleID);
    LOG_CHECK_ERROR(bRet);

    // construct memory
    // Student can no longer congratulate the mentor
    //AddCZ(teacher.pRoleData->byGroup, teacher.pRoleData->dwRoleID, student.pRoleData->dwRoleID, teacher.pRoleData->byLevel);
    AddCZ(teacher.pRoleData->byGroup, student.pRoleData->dwRoleID, teacher.pRoleData->dwRoleID, student.pRoleData->byLevel);
    
    // delete student all app
    MasterStudentCancelAllApp(student);

    INF("Master: create relation. Teacher[%u], Student[%u]", teacher.pRoleData->dwRoleID, student.pRoleData->dwRoleID);

    // cancel master recruit
    nRetCode = CMasterMgr::GetInstance().IsStudentFull(teacher);
    if (nRetCode)
    {
        nRetCode = CMasterMgr::GetInstance().DelRecruit(teacher);
        LOG_CHECK_ERROR(nRetCode);
    }

    // notify to gs
    nRetCode = CMessageHandler::GetInstance().DoGSMstSyncState(student.dwGSSessionID, student.pRoleData->dwRoleID, MASTER_STUDENT);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSMstSyncData(student.dwGSSessionID, student.dwCliSessionID, student.pRoleData->dwRoleID, *pTeacherData);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSMstCreateRelationNtf(teacher.pRoleData->dwRoleID, student.pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientMstAddStudentNtf(*pTeacherData, student.pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientMasterInfo(student.dwCliSessionID, student.pRoleData->dwRoleID, *pTeacherData);
    LOG_CHECK_ERROR(nRetCode);

    return errSuccess;
Exit0:
    if (pStudentData)
        m_StudentPool.Delete(pStudentData);
        
    return nResult;
}

INT32 CMasterMgr::DelMasterRelation(UINT32 dwTeacherID, MS_ROLE_DATA &student, BOOL bIsGrad)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA     *pOnlineStudRole = NULL;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_STUDENT_DATA     *pStudentData = NULL;
    STUD_MAP::iterator  iter;
    INT32       nIndex = 0;


    nRetCode = CMasterMgr::GetInstance().IsMaster(dwTeacherID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMasterMgr::GetInstance().IsStudent(student.dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    pTeacherData = CMasterMgr::GetInstance().GetTeacherData(dwTeacherID);
    LOG_PROCESS_ERROR(pTeacherData);

    LOG_PROCESS_ERROR(pTeacherData->byStudNum > 0);
    // check is teacher's student
    for ( ;nIndex < pTeacherData->byStudNum; ++nIndex)
    {
        if (pTeacherData->arrStudentID[nIndex] == student.dwRoleID)
            break;
    }
    LOG_PROCESS_ERROR(nIndex < pTeacherData->byStudNum);

    pStudentData = CMasterMgr::GetInstance().GetStudentData(student.dwRoleID);
    LOG_PROCESS_ERROR(pStudentData);

    // delete mysql data
    _DelMasterRelationToMysql(dwTeacherID, student.dwRoleID);

    pTeacherData->arrStudentID[nIndex] = pTeacherData->arrStudentID[pTeacherData->byStudNum - 1];
    --pTeacherData->byStudNum;

    // Remove student-to-mentor congratulations
    //nRetCode = DelCZ(dwTeacherID, student.dwRoleID);
    //LOG_CHECK_ERROR(nRetCode);
    nRetCode = DelCZ(student.dwRoleID, dwTeacherID);
    LOG_CHECK_ERROR(nRetCode);

    // Remove congratulations between fellow students
    //for (INT32 i = 0; i < pTeacherData->byStudNum; ++i)
    //{
    //    if (pTeacherData->arrStudentID[i] != student.dwRoleID)
    //    {
    //        nRetCode = DelCZ(pTeacherData->arrStudentID[i], student.dwRoleID);
    //        LOG_CHECK_ERROR(nRetCode);
    //        nRetCode = DelCZ(student.dwRoleID, pTeacherData->arrStudentID[i]);
    //        LOG_CHECK_ERROR(nRetCode);
    //    }
    //}

    m_mapStudent.erase(student.dwRoleID);
    nRetCode = m_StudentPool.Delete(pStudentData);
    LOG_CHECK_ERROR(nRetCode);

    INF("Master: delete relation. Teacher[%u], Student[%u]", dwTeacherID, student.dwRoleID);

    // notify
    nRetCode = CMessageHandler::GetInstance().DoGSMstDelRelationNtf(dwTeacherID, student.dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    //nRetCode = CMessageHandler::GetInstance().DoClientMstDelStudentNtf(*pTeacherData, student.dwRoleID);
    //LOG_CHECK_ERROR(nRetCode);

    pOnlineStudRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(student.dwRoleID);
    if (pOnlineStudRole)
    {
        nRetCode = CMessageHandler::GetInstance().DoGSMstSyncState(pOnlineStudRole->dwGSSessionID, pOnlineStudRole->pRoleData->dwRoleID, MASTER_NONE);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CMessageHandler::GetInstance().DoClientMstClearInfo(pOnlineStudRole->dwCliSessionID);
        LOG_CHECK_ERROR(nRetCode);
    }

    if (pOnlineStudRole && pOnlineStudRole->pRoleData->byLevel >= GRADUATE_LEVEL)
    {
        nRetCode = CMasterMgr::GetInstance().BeMaster(pOnlineStudRole->pRoleData);
        LOG_CHECK_ERROR(nRetCode);
    }

    return errSuccess;
Exit0:
    return nResult;
}

MS_TEACHER_DATA* CMasterMgr::GetTeacherData(UINT32 dwTeacherID)
{
    TEACHER_MAP::iterator   iter = m_mapTeacher.find(dwTeacherID);
    PROCESS_ERROR(iter != m_mapTeacher.end());

    return &(*iter->second);
Exit0:
    return NULL;
}

MS_STUDENT_DATA* CMasterMgr::GetStudentData(UINT32 dwStudentID)
{
    STUD_MAP::iterator  iter = m_mapStudent.find(dwStudentID);
    PROCESS_ERROR(iter != m_mapStudent.end());

    return &(*iter->second);
Exit0:
    return NULL;
}

BOOL CMasterMgr::IsStudent(UINT32 dwRoleID)
{
    STUD_MAP::iterator  iter = m_mapStudent.find(dwRoleID);
    PROCESS_ERROR(iter != m_mapStudent.end());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::IsMaster(UINT32 dwRoleID)
{
    TEACHER_MAP::iterator   iter = m_mapTeacher.find(dwRoleID);
    PROCESS_ERROR(iter != m_mapTeacher.end());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::IsStudentFull(MS_ONLINE_ROLE_DATA &onlineRole)
{
    TEACHER_MAP::iterator   iter;
    
    LOG_PROCESS_ERROR(onlineRole.pRoleData);

    iter = m_mapTeacher.find(onlineRole.pRoleData->dwRoleID);
    PROCESS_ERROR(iter != m_mapTeacher.end());

	if (onlineRole.llMasterExp < g_GWConfig.nMasterLvlTwoExp)    //  < 300
    {
        PROCESS_SUCCESS(iter->second->byStudNum < g_GWConfig.nMasterLvlOneStudentNum);
    }
	else if (onlineRole.llMasterExp < g_GWConfig.nMasterLvlThreeExp)   // 300 - 500
    {
        PROCESS_SUCCESS(iter->second->byStudNum < g_GWConfig.nMasterLevelTwoStudentNum);
    }
    else                                                                    // > 500
    {
        PROCESS_SUCCESS(iter->second->byStudNum < g_GWConfig.nMasterLevelThreeStudentNum);
    }

    return TRUE;
Exit1:
    return FALSE;
Exit0:
    return TRUE;
}

INT32 CMasterMgr::GetStudentNum(UINT32 dwTeacherID)
{
    TEACHER_MAP::iterator   iter = m_mapTeacher.find(dwTeacherID);
    PROCESS_ERROR(iter != m_mapTeacher.end());

    return iter->second->byStudNum;
Exit0:
    return -1;
}

void CMasterMgr::AddCZ_i(INT32 nSvrGroup, UINT64 qwID, INT32 nLevel, BOOL isSave)
{
    INT32       nRetCode = 0;

    if (isSave)
    {
        nRetCode = _AddCZToMysql(nSvrGroup, qwID, nLevel);
        LOG_CHECK_ERROR(nRetCode);
    }

    m_mapCZ.insert(std::make_pair(qwID, nLevel));

    return ;
}

void CMasterMgr::AddCZ(INT32 nSvrGroup, UINT32 dwRole1, UINT32 dwRole2, INT32 nLevel)
{
    INT32       nRetCode = 0;
    CZ_MAP::iterator        iter;
    UINT64      qwID = dwRole1;
    qwID = (qwID << 32) + dwRole2;

    iter = m_mapCZ.find(qwID);
    LOG_CHECK_ERROR(iter == m_mapCZ.end());
    if (iter != m_mapCZ.end())
        DelCZ(dwRole1, dwRole2);

    AddCZ_i(nSvrGroup, qwID, nLevel, TRUE);
}

BOOL CMasterMgr::DelCZ(UINT32 dwRole1, UINT32 dwRole2)
{
    INT32       nRetCode = 0;
    UINT64      qwID = dwRole1;
    qwID = (qwID << 32) + dwRole2;

    nRetCode = _DelCZToMysql(qwID);
    LOG_PROCESS_ERROR(nRetCode);

    m_mapCZ.erase(qwID);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CMasterMgr::GetCZ(UINT32 dwRole1, UINT32 dwRole2)
{
    CZ_MAP::iterator        iter;
    UINT64      qwID = dwRole1;
    qwID = (qwID << 32) + dwRole2;

    iter = m_mapCZ.find(qwID);
    LOG_PROCESS_ERROR(iter != m_mapCZ.end());

    return iter->second;
Exit0:
    return 0;
}

BOOL CMasterMgr::UpdateCZ(UINT32 dwRole1, MS_ONLINE_ROLE_DATA &onlineRole, INT32 nLevel)
{
    INT32       nRetCode = 0;
    MS_ROLE_DATA        *pTargetRole = NULL;
    CZ_MAP::iterator    iter;
    UINT64      qwID = dwRole1;

    LOG_PROCESS_ERROR(onlineRole.pRoleData);

    qwID = (qwID << 32) + onlineRole.pRoleData->dwRoleID;

    pTargetRole = CRoleDataMgr::GetInstance().FindRole(dwRole1);
    LOG_PROCESS_ERROR(pTargetRole);

    iter = m_mapCZ.find(qwID);
    LOG_PROCESS_ERROR(iter != m_mapCZ.end());

    iter->second = nLevel;
    nRetCode = _UpdateCZToMysql(qwID, iter->second);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientMstCZLevel(onlineRole.dwCliSessionID, dwRole1, nLevel);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::LoadTeacherData(UINT32 dwTeacherID)
{
    MS_TEACHER_DATA     *pTeacherData = NULL;

    TEACHER_MAP::iterator   iter = m_mapTeacher.find(dwTeacherID);
    LOG_PROCESS_ERROR(iter == m_mapTeacher.end());

    pTeacherData = m_TeacherPool.New();
    LOG_PROCESS_ERROR(pTeacherData);

    memset(pTeacherData, 0, sizeof(MS_TEACHER_DATA));

    pTeacherData->dwRoleID = dwTeacherID;

    m_mapTeacher.insert(std::make_pair(dwTeacherID, pTeacherData));

    return TRUE;
Exit0:
    if (pTeacherData)
        m_TeacherPool.Delete(pTeacherData);
    return FALSE;
}

BOOL CMasterMgr::LoadStudentData(UINT32 dwTeacherID, UINT32 dwStudentID)
{
    MS_STUDENT_DATA     *pStudentData = NULL;

    pStudentData = m_StudentPool.New();
    LOG_PROCESS_ERROR(pStudentData);

    memset(pStudentData, 0, sizeof(MS_STUDENT_DATA));

    pStudentData->dwRoleID = dwStudentID;
    pStudentData->dwTeacherID = dwTeacherID;

    m_mapStudent.insert(std::make_pair(dwStudentID, pStudentData));

    return TRUE;
Exit0:
    if (pStudentData)
        m_StudentPool.Delete(pStudentData);
    return FALSE;
}

BOOL CMasterMgr::_AddMasterRelationToMysql( INT32 nSvrGroup, UINT32 dwTeacherID, UINT32 dwStudentID ) 
{
    CManageServer   &ms = CManageServer::GetInstance();
    //BOOL bRet = ms.m_threads[dwTeacherID % ms.m_dwThreadCount].AddMaster(dwTeacherID, dwStudentID);
    //LOG_PROCESS_ERROR(bRet);

    BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlInsertMaster, NULL, NULL, "ucudud", (UINT8)nSvrGroup, dwTeacherID, dwStudentID);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::_DelMasterRelationToMysql(UINT32 dwTeacherID, UINT32 dwStudentID)
{
    CManageServer   &ms = CManageServer::GetInstance();
    //BOOL bRet = ms.m_threads[dwTeacherID % ms.m_dwThreadCount].DelMaster(dwStudentID, dwTeacherID);
    //LOG_PROCESS_ERROR(bRet);

    BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteMaster, NULL, NULL, "udud", dwTeacherID, dwStudentID);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::_AddCZToMysql(INT32 nSvrGroup, UINT64 qwID, INT32 nCZ)
{
    CManageServer   &ms = CManageServer::GetInstance();
    //BOOL bRet = ms.m_threads[(UINT32)qwID % ms.m_dwThreadCount].AddMstCZ(qwID, nCZ);
    //LOG_PROCESS_ERROR(bRet);

    BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlInsertMasterCZ, NULL, NULL, "uculuc", (UINT8)nSvrGroup, qwID, (UINT8)nCZ);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::_DelCZToMysql(UINT64 qwID)
{
    CManageServer   &ms = CManageServer::GetInstance();
    //BOOL bRet = ms.m_threads[(UINT32)qwID % ms.m_dwThreadCount].DelMstCZ(qwID);
    //LOG_PROCESS_ERROR(bRet);

    BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteMasterCZ, NULL, NULL, "ul", qwID);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;
Exit0:
    return FALSE;
}
BOOL CMasterMgr::_UpdateCZToMysql(UINT64 qwID, INT32 nCZ)
{
    CManageServer   &ms = CManageServer::GetInstance();
    //BOOL bRet = ms.m_threads[(UINT32)qwID % ms.m_dwThreadCount].UpdateMstCZ(qwID, nCZ);
    //LOG_PROCESS_ERROR(bRet);

    BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateMasterCZ, NULL, NULL, "ucul", (UINT8)nCZ, qwID);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;
Exit0:
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// recruit
//////////////////////////////////////////////////////////////////////////
// insert & update
INT32 CMasterMgr::AddRecruit(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    RECRUIT_LISTIter iter;

    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);
    LOG_PROCESS_ERROR(onlineRoleData.pRoleData->byGroup < MAX_SERVER_GROUP_COUNT);

    if (onlineRoleData.nGMLevel != 0)
        return errSuccess;

    nRetCode = CMasterMgr::GetInstance().IsMaster(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(onlineRoleData);
    LOG_PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);

    iter = find(m_RecruitList[onlineRoleData.pRoleData->byGroup].begin(), m_RecruitList[onlineRoleData.pRoleData->byGroup].end(), onlineRoleData.pRoleData->dwRoleID);
    if (iter == m_RecruitList[onlineRoleData.pRoleData->byGroup].end())
    {
        m_RecruitList[onlineRoleData.pRoleData->byGroup].push_back(onlineRoleData.pRoleData->dwRoleID);
    }

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMasterMgr::DelRecruit(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    RECRUIT_LISTIter    iter;

    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);
    LOG_PROCESS_ERROR(onlineRoleData.pRoleData->byGroup < MAX_SERVER_GROUP_COUNT);

    iter = find(m_RecruitList[onlineRoleData.pRoleData->byGroup].begin(), m_RecruitList[onlineRoleData.pRoleData->byGroup].end(), onlineRoleData.pRoleData->dwRoleID);
    if (iter != m_RecruitList[onlineRoleData.pRoleData->byGroup].end())
    {
        if (iter == m_CursorRoleIter[onlineRoleData.pRoleData->byGroup])
        {
            ++m_CursorRoleIter[onlineRoleData.pRoleData->byGroup];
        }
        m_RecruitList[onlineRoleData.pRoleData->byGroup].erase(iter);
    }

    return errSuccess;
Exit0:
    return nResult;
}


BOOL CMasterMgr::GetRecruitData(INT32 nGroup, UINT32 &dwRoleID)
{
    LOG_PROCESS_ERROR(nGroup < MAX_SERVER_GROUP_COUNT && nGroup >= 0);
    PROCESS_ERROR(!m_RecruitList[nGroup].empty());

    if (m_CursorRoleIter[nGroup] == m_RecruitList[nGroup].end())
    {
        m_CursorRoleIter[nGroup] = m_RecruitList[nGroup].begin();
    }

    dwRoleID = *m_CursorRoleIter[nGroup];
    ++m_CursorRoleIter[nGroup];

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::AddSolo(INT32 nGroup, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    SOLO_LISTIter iter;

    LOG_PROCESS_ERROR(nGroup < MAX_SERVER_GROUP_COUNT && nGroup >= 0);

    nRetCode = CMasterMgr::GetInstance().IsMaster(dwRoleID);
    PROCESS_ERROR(!nRetCode);

    nRetCode = CMasterMgr::GetInstance().IsStudent(dwRoleID);
    PROCESS_SUCCESS(nRetCode);

    iter = find(m_SoloList[nGroup].begin(), m_SoloList[nGroup].end(), dwRoleID);
    if (iter == m_SoloList[nGroup].end())
    {
        m_SoloList[nGroup].push_back(dwRoleID);
    }

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::DelSolo(INT32 nGroup, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    SOLO_LISTIter    iter;

    LOG_PROCESS_ERROR(nGroup < MAX_SERVER_GROUP_COUNT && nGroup >= 0);

    iter = find(m_SoloList[nGroup].begin(), m_SoloList[nGroup].end(), dwRoleID);
    if (iter != m_SoloList[nGroup].end())
    {
        if (iter == m_SoloCursorIter[nGroup])
        {
            ++m_SoloCursorIter[nGroup];
        }
        m_SoloList[nGroup].erase(iter);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::GetSolo(INT32 nGroup, UINT32 &dwRoleID)
{
    LOG_PROCESS_ERROR(nGroup < MAX_SERVER_GROUP_COUNT && nGroup >= 0);
    PROCESS_ERROR(!m_SoloList[nGroup].empty());

    if (m_SoloCursorIter[nGroup] == m_SoloList[nGroup].end())
    {
        m_SoloCursorIter[nGroup] = m_SoloList[nGroup].begin();
    }

    dwRoleID = *m_SoloCursorIter[nGroup];
    ++m_SoloCursorIter[nGroup];

    return TRUE;
Exit0:
    return FALSE;
}

MS_ONLINE_ROLE_DATA* CMasterMgr::GetOneFromTwitter(INT32 nGroup)
{
    INT32   nRetCode = 0;
    MS_ONLINE_ROLE_DATA *pOnlineRole = NULL;
    BASE_LIST_NODE  *pNode = NULL;

    pNode = m_TwitterList[nGroup].Head().pNext;
    while (pNode != &(m_TwitterList[nGroup].Rear()))
    {
        MS_ONLINE_ROLE_DATA *pOnlineRole = node_cast(MS_ONLINE_ROLE_DATA*, stMasterTwitterNode, pNode);

        if (pOnlineRole->dwMasterTwitterCD < CManageServer::GetInstance().GetGameTime())
        {
            pOnlineRole->dwMasterTwitterCD = CManageServer::GetInstance().GetGameTime() + g_GWConfig.nMasterTwitterCD;
            //nRetCode = CMessageHandler::GetInstance().DoGSMstUpdateData(pOnlineRole->pRoleData->dwRoleID, MST_SYNC_GS_TWITTER_CD, pOnlineRole->dwMasterTwitterCD);
            //LOG_CHECK_ERROR(nRetCode);
            return pOnlineRole;
        }

        pNode = pNode->pNext;
    }

Exit0:
    return NULL;
}

BOOL CMasterMgr::JoinTwitter(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;
    BASE_LIST_NODE  *pNode = NULL;

    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);
    LOG_PROCESS_ERROR(onlineRoleData.stMasterTwitterNode.pNext == NULL);
    LOG_PROCESS_ERROR(onlineRoleData.stMasterTwitterNode.pPrev == NULL);

    pNode = m_TwitterList[onlineRoleData.pRoleData->byGroup].Head().pNext;
    while (pNode != &(m_TwitterList[onlineRoleData.pRoleData->byGroup].Rear()))
    {
        MS_ONLINE_ROLE_DATA *pOnlineRole = node_cast(MS_ONLINE_ROLE_DATA*, stMasterTwitterNode, pNode);

		if (pOnlineRole->llMasterExp < onlineRoleData.llMasterExp)
            break;

        pNode = pNode->pNext;
    }

    nRetCode = m_TwitterList[onlineRoleData.pRoleData->byGroup].InsertBefore(&onlineRoleData.stMasterTwitterNode, pNode);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("master [%s][%d] join twitter", onlineRoleData.pRoleData->szRoleName, (INT32)onlineRoleData.pRoleData->byLevel);
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::QuitTwitter(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);
    PROCESS_ERROR(onlineRoleData.stMasterTwitterNode.pNext != NULL);
    PROCESS_ERROR(onlineRoleData.stMasterTwitterNode.pPrev != NULL);

    m_TwitterList[onlineRoleData.pRoleData->byGroup].Remove(&onlineRoleData.stMasterTwitterNode);

    DBG("master [%s][%d] quit twitter", onlineRoleData.pRoleData->szRoleName, (INT32)onlineRoleData.pRoleData->byLevel);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::AjustRankingsOnTwitter(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;
    INT32       nGroup = onlineRoleData.pRoleData->byGroup;
    BASE_LIST_NODE  *pNode = NULL;
    MS_ONLINE_ROLE_DATA *pNextOnlineRole = NULL;
    MS_ONLINE_ROLE_DATA *pPrevOnlineRole = NULL;

    DBG("try master [%s][%d] ajust twitter", onlineRoleData.pRoleData->szRoleName, (INT32)onlineRoleData.pRoleData->byLevel);

    LOG_PROCESS_ERROR(onlineRoleData.stMasterTwitterNode.pNext != NULL);
    LOG_PROCESS_ERROR(onlineRoleData.stMasterTwitterNode.pPrev != NULL);

    DBG("master [%s][%d] ajust twitter", onlineRoleData.pRoleData->szRoleName, (INT32)onlineRoleData.pRoleData->byLevel);

    pNode = onlineRoleData.stMasterTwitterNode.pNext;
    if (pNode != &m_TwitterList[nGroup].Rear())
    {
        pNextOnlineRole = node_cast(MS_ONLINE_ROLE_DATA*, stMasterTwitterNode, pNode);
    }
    pNode = onlineRoleData.stMasterTwitterNode.pPrev;
    if (pNode != &m_TwitterList[nGroup].Head())
    {
        pPrevOnlineRole = node_cast(MS_ONLINE_ROLE_DATA*, stMasterTwitterNode, pNode);
    }
    
    if (pNextOnlineRole && pNextOnlineRole->llMasterExp > onlineRoleData.llMasterExp)
    {
        m_TwitterList[nGroup].Remove(&onlineRoleData.stMasterTwitterNode);

        while (pNode != &m_TwitterList[nGroup].Rear())
        {
            LOG_PROCESS_ERROR(pNode);
            pNextOnlineRole = node_cast(MS_ONLINE_ROLE_DATA*, stMasterTwitterNode, pNode);
            
            if (pNextOnlineRole->llMasterExp <= onlineRoleData.llMasterExp)
            {
                m_TwitterList[nGroup].InsertBefore(&onlineRoleData.stMasterTwitterNode, pNode);
                break;
            }

            pNode = pNode->pNext;
        }

    }
    else if (pPrevOnlineRole && pPrevOnlineRole->llMasterExp < onlineRoleData.llMasterExp)
    {
        LOG_PROCESS_ERROR(pNode);
        m_TwitterList[nGroup].Remove(&onlineRoleData.stMasterTwitterNode);

        while (pNode != &m_TwitterList[nGroup].Head())
        {
            LOG_PROCESS_ERROR(pNode);
            pPrevOnlineRole = node_cast(MS_ONLINE_ROLE_DATA*, stMasterTwitterNode, pNode);

            if (pPrevOnlineRole->llMasterExp >= onlineRoleData.llMasterExp)
            {
                m_TwitterList[nGroup].InsertAfter(&onlineRoleData.stMasterTwitterNode, pNode);
                break;
            }

            pNode = pNode->pPrev;
        }
    }


    //// to be optimize...
    //nRetCode = QuitTwitter(onlineRoleData);
    //LOG_PROCESS_ERROR(nRetCode);

    //nRetCode = JoinTwitter(onlineRoleData);
    //LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMasterMgr::AjustRankingsOnTwitter(MS_ONLINE_ROLE_DATA &onlineRoleData, BOOL bMasterExpAdd)
{
    INT32       nRetCode = 0;
    INT32       nGroup = onlineRoleData.pRoleData->byGroup;
    BASE_LIST_NODE  *pNode = NULL;
    MS_ONLINE_ROLE_DATA *pOnlineRole = NULL;

    DBG("master [%s][%d] ajust twitter two param", onlineRoleData.pRoleData->szRoleName, (INT32)onlineRoleData.pRoleData->byLevel);

    if (bMasterExpAdd)
    {
        pNode = onlineRoleData.stMasterTwitterNode.pPrev;
        while (pNode != &m_TwitterList[nGroup].Head())
        {
            LOG_PROCESS_ERROR(pNode);
            pOnlineRole = node_cast(MS_ONLINE_ROLE_DATA*, stMasterTwitterNode, pNode);

            if (pOnlineRole->llMasterExp >= onlineRoleData.llMasterExp)
            {
                m_TwitterList[nGroup].InsertAfter(&onlineRoleData.stMasterTwitterNode, pNode);
                break;
            }

            pNode = pNode->pPrev;
        }
    }
    else
    {
        pNode = onlineRoleData.stMasterTwitterNode.pNext;
        while (pNode != &m_TwitterList[nGroup].Rear())
        {
            LOG_PROCESS_ERROR(pNode);
            pOnlineRole = node_cast(MS_ONLINE_ROLE_DATA*, stMasterTwitterNode, pNode);

            if (pOnlineRole->llMasterExp <= onlineRoleData.llMasterExp)
            {
                m_TwitterList[nGroup].InsertBefore(&onlineRoleData.stMasterTwitterNode, pNode);
                break;
            }

            pNode = pNode->pNext;
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL LoadMaster(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_STUDENT_DATA     *pStudentData = NULL;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8       bySvrGroup;
        UINT32      dwTeacherID;
        UINT32      dwStudentID;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->bySvrGroup));
        pTeacherData = CMasterMgr::GetInstance().GetTeacherData(pLoadResult->dwTeacherID);
        if (!pTeacherData)
        {
            nRetCode = CMasterMgr::GetInstance().LoadTeacherData(pLoadResult->dwTeacherID);
            LOG_PROCESS_ERROR(nRetCode);
        }

        if (pLoadResult->dwTeacherID == pLoadResult->dwStudentID)
            continue;

        pTeacherData = CMasterMgr::GetInstance().GetTeacherData(pLoadResult->dwTeacherID);
        LOG_PROCESS_ERROR(pTeacherData);
        LOG_PROCESS_ERROR(pTeacherData->byStudNum < MAX_STUDENT_NUM);

        // check code
        for (INT32 nIndex = 0; nIndex < pTeacherData->byStudNum; ++nIndex)
            LOG_PROCESS_ERROR(pTeacherData->arrStudentID[nIndex] != pLoadResult->dwStudentID);

        pTeacherData->arrStudentID[pTeacherData->byStudNum] = pLoadResult->dwStudentID;
        ++pTeacherData->byStudNum;

        pStudentData = CMasterMgr::GetInstance().GetStudentData(pLoadResult->dwStudentID);
        LOG_PROCESS_ERROR(!pStudentData);

        nRetCode = CMasterMgr::GetInstance().LoadStudentData(pLoadResult->dwTeacherID, pLoadResult->dwStudentID);
        LOG_PROCESS_ERROR(nRetCode);

        ++nRecords;
    }
    INF("Loading Master Data Finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL LoadMasterCZ(std::list<void*>& rResult, void* pUserData)
{
    INT32 nRetCode = 0;
    INT32       nRecords = 0;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8       bySvrGroup;
        UINT64      qwID;
        UINT8       byCz;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->bySvrGroup));
        LOG_PROCESS_ERROR(pLoadResult->bySvrGroup >= 0 && pLoadResult->bySvrGroup < MAX_SERVER_GROUP_COUNT);
        if (g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID > 0 &&
            g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID < MAX_SERVER_GROUP_COUNT)
		{
            pLoadResult->bySvrGroup = g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID;
		}

        CMasterMgr::GetInstance().AddCZ_i(pLoadResult->bySvrGroup, pLoadResult->qwID, pLoadResult->byCz, FALSE);
        ++nRecords;
    }
    INF("Loading Master CZ Data Finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL MasterCancelSendApp(MS_ONLINE_ROLE_DATA &onlineRoleData, UINT32 dwRoleID)
{
    for (INT32 nIndex = 0; nIndex < onlineRoleData.byMasterSendAppNum; ++nIndex)
    {
        if (onlineRoleData.arrMasterSendApp[nIndex].dwRoleID == dwRoleID)
        {
            onlineRoleData.arrMasterSendApp[nIndex].dwRoleID = onlineRoleData.arrMasterSendApp[onlineRoleData.byMasterSendAppNum].dwRoleID;
            onlineRoleData.arrMasterSendApp[nIndex].nWashoutTime = onlineRoleData.arrMasterSendApp[onlineRoleData.byMasterSendAppNum].nWashoutTime;
            --onlineRoleData.byMasterSendAppNum;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL MasterCancelRecvApp(MS_ONLINE_ROLE_DATA &onlineRoleData, UINT32 dwRoleID)
{
    for (INT32 nIndex = 0; nIndex < onlineRoleData.byMasterRecvAppNum; ++nIndex)
    {
        if (onlineRoleData.arrMasterRecvApp[nIndex] == dwRoleID)
        {
            onlineRoleData.arrMasterRecvApp[nIndex] = onlineRoleData.arrMasterRecvApp[onlineRoleData.byMasterRecvAppNum];
            --onlineRoleData.byMasterRecvAppNum;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL MasterTeacherCancelAllApp(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;
    MS_ONLINE_ROLE_DATA     *pOnlineRole = NULL;

    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);

    for (INT32 nIndex = 0; nIndex < onlineRoleData.byMasterSendAppNum; ++nIndex)
    {
        pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(onlineRoleData.arrMasterSendApp[nIndex].dwRoleID);
        if (pOnlineRole)
        {
            MasterCancelRecvApp(*pOnlineRole, onlineRoleData.pRoleData->dwRoleID);

            nRetCode = CMessageHandler::GetInstance().DoClientMstCancelTeacherAppNtf(pOnlineRole->dwCliSessionID, onlineRoleData.pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
    onlineRoleData.byMasterSendAppNum = 0;
    memset(onlineRoleData.arrMasterSendApp, 0, sizeof(onlineRoleData.arrMasterSendApp));


    for (INT32 nIndex = 0; nIndex < onlineRoleData.byMasterRecvAppNum; ++nIndex)
    {
        pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(onlineRoleData.arrMasterRecvApp[nIndex]);
        if (pOnlineRole)
        {
            MasterCancelSendApp(*pOnlineRole, onlineRoleData.pRoleData->dwRoleID);

            nRetCode = CMessageHandler::GetInstance().DoClientMstRefuseAppNtf(pOnlineRole->dwCliSessionID, onlineRoleData.pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
    onlineRoleData.byMasterRecvAppNum = 0;
    memset(onlineRoleData.arrMasterRecvApp, 0, sizeof(onlineRoleData.arrMasterRecvApp));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL MasterStudentCancelAllApp(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;
    MS_ONLINE_ROLE_DATA     *pOnlineRole = NULL;

    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);

    for (INT32 nIndex = 0; nIndex < onlineRoleData.byMasterSendAppNum; ++nIndex)
    {
        pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(onlineRoleData.arrMasterSendApp[nIndex].dwRoleID);
        if (pOnlineRole)
        {
            MasterCancelRecvApp(*pOnlineRole, onlineRoleData.pRoleData->dwRoleID);

            nRetCode = CMessageHandler::GetInstance().DoClientMstCancelAppNtf(pOnlineRole->dwCliSessionID, onlineRoleData.pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
    onlineRoleData.byMasterSendAppNum = 0;
    memset(onlineRoleData.arrMasterSendApp, 0, sizeof(onlineRoleData.arrMasterSendApp));


    for (INT32 nIndex = 0; nIndex < onlineRoleData.byMasterRecvAppNum; ++nIndex)
    {
        pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(onlineRoleData.arrMasterRecvApp[nIndex]);
        if (pOnlineRole)
        {
            MasterCancelSendApp(*pOnlineRole, onlineRoleData.pRoleData->dwRoleID);

            nRetCode = CMessageHandler::GetInstance().DoClientMstRefuseTeacherAppNtf(pOnlineRole->dwCliSessionID, onlineRoleData.pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
    onlineRoleData.byMasterRecvAppNum = 0;
    memset(onlineRoleData.arrMasterRecvApp, 0, sizeof(onlineRoleData.arrMasterRecvApp));

    return TRUE;
Exit0:
    return FALSE;
}


BOOL PushRoleInfoToTeacher(UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    MS_ONLINE_ROLE_DATA *pOnlineRole = NULL;
    MS_ONLINE_ROLE_DATA *pMasterRole = NULL;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
    PROCESS_ERROR(pOnlineRole);
    PROCESS_ERROR(pOnlineRole->pRoleData);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pOnlineRole->pRoleData->dwRoleID);
    PROCESS_ERROR(nRetCode == FALSE);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pOnlineRole->pRoleData->dwRoleID);
    PROCESS_ERROR(nRetCode == FALSE);
    PROCESS_ERROR(pOnlineRole->pRoleData->byLevel <= g_GWConfig.nMasterFindTeacherLevelLimit);
    PROCESS_ERROR(pOnlineRole->dwMasterTwitterCD < CManageServer::GetInstance().GetGameTime());

    pMasterRole = CMasterMgr::GetInstance().GetOneFromTwitter(pOnlineRole->pRoleData->byGroup);
    PROCESS_ERROR(pMasterRole);

    pMasterRole->dwBeDeterminedRoleID = dwRoleID;

    // send msg
    nRetCode = CMessageHandler::GetInstance().DoClientMstHelloTeacher(pMasterRole->dwCliSessionID, pOnlineRole->pRoleData->dwRoleID, pOnlineRole->pRoleData->byLevel);
    LOG_CHECK_ERROR(nRetCode);

    DBG("Push role[%s][%d] to teacher[%s][%d]", pOnlineRole->pRoleData->szRoleName, (INT32)pOnlineRole->pRoleData->byLevel, 
        pMasterRole->pRoleData->szRoleName, (INT32)pMasterRole->pRoleData->byLevel);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL MasterLevelUp(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32               nRetCode = 0;
    UINT32              dwRoleID = 0;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_STUDENT_DATA     *pStudentData = NULL;
    const MS_MASTER_GIFT_TEMPLATE     *pTemplate = NULL;
    MS_ROLE_DATA        *pRole = NULL;

    if (onlineRoleData.nGMLevel != 0)
        return TRUE;

    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);

    if (onlineRoleData.pRoleData->byLevel >= GRADUATE_LEVEL)
    {
        nRetCode = CMasterMgr::GetInstance().DelSolo(onlineRoleData.pRoleData->byGroup, onlineRoleData.pRoleData->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        if (onlineRoleData.pRoleData->byLevel >= GRADUATE_LEVEL)
        {
            if ((!CMasterMgr::GetInstance().IsStudent(onlineRoleData.pRoleData->dwRoleID))
                && (!CMasterMgr::GetInstance().IsMaster(onlineRoleData.pRoleData->dwRoleID)))
            {
                nRetCode = CMasterMgr::GetInstance().BeMaster(onlineRoleData.pRoleData);
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }
    else if (onlineRoleData.pRoleData->byLevel < GRADUATE_LEVEL && onlineRoleData.pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT)
    {
        nRetCode = CMasterMgr::GetInstance().AddSolo(onlineRoleData.pRoleData->byGroup, onlineRoleData.pRoleData->dwRoleID);
        //LOG_CHECK_ERROR(nRetCode);
    }

    pStudentData = CMasterMgr::GetInstance().GetStudentData(onlineRoleData.pRoleData->dwRoleID);
    if (pStudentData)
    {
        pRole = CRoleDataMgr::GetInstance().FindRole(pStudentData->dwTeacherID);
        LOG_PROCESS_ERROR(pRole);

        dwRoleID = pStudentData->dwTeacherID;
    }
    else
        dwRoleID = onlineRoleData.pRoleData->dwRoleID;

    pTeacherData = CMasterMgr::GetInstance().GetTeacherData(dwRoleID);
    PROCESS_ERROR(pTeacherData);

    nRetCode = CMessageHandler::GetInstance().DoClientMasterSyncLvl(*pTeacherData, onlineRoleData.pRoleData->dwRoleID, onlineRoleData.pRoleData->byLevel);
    LOG_CHECK_ERROR(nRetCode);

    if (pRole && onlineRoleData.pRoleData->byLevel <= GRADUATE_LEVEL)
    {
        pTemplate = GetMasterGiftTemplate(onlineRoleData.pRoleData->byLevel);
        LOG_PROCESS_ERROR(pTemplate);

        if (pTemplate->m_dwLvlUpStudMailID > 0)
        {
            nRetCode = CMailMgr::GetInstance().SendSystemMail(onlineRoleData.pRoleData, pTemplate->m_dwLvlUpStudMailID);
            LOG_CHECK_ERROR(nRetCode);
        }

        if (pTemplate->m_dwLvlUpTeacherMailID > 0)
        {
            nRetCode = CMailMgr::GetInstance().SendSystemMail(pRole, pTemplate->m_dwLvlUpTeacherMailID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL MasterRoleLogin(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_STUDENT_DATA     *pStudentData = NULL;

    if (onlineRoleData.nGMLevel != 0)
        return TRUE;

    if (onlineRoleData.pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && onlineRoleData.pRoleData->byLevel < GRADUATE_LEVEL)
    {
        nRetCode = CMasterMgr::GetInstance().AddSolo(onlineRoleData.pRoleData->byGroup, onlineRoleData.pRoleData->dwRoleID);
        //LOG_CHECK_ERROR(nRetCode);  The player may already be a student; the internal check returns as normal
    }

    nRetCode = CMasterMgr::GetInstance().IsMaster(onlineRoleData.pRoleData->dwRoleID);
    if (nRetCode)
    {
        nRetCode = CMasterMgr::GetInstance().IsStudentFull(onlineRoleData);
        if (!nRetCode)
        {
            nRetCode = CMasterMgr::GetInstance().AddRecruit(onlineRoleData);
            LOG_CHECK_ERROR(nRetCode == errSuccess);
        }

        if (onlineRoleData.bTwitter)
        {
            nRetCode = CMasterMgr::GetInstance().JoinTwitter(onlineRoleData);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL MasterRoleLogout(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;

    nRetCode = CMasterMgr::GetInstance().DelSolo(onlineRoleData.pRoleData->byGroup, onlineRoleData.pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMasterMgr::GetInstance().DelRecruit(onlineRoleData);
    LOG_CHECK_ERROR(nRetCode == errSuccess);

    nRetCode = CMasterMgr::GetInstance().IsMaster(onlineRoleData.pRoleData->dwRoleID);
    if (onlineRoleData.bTwitter && nRetCode)
    {
        nRetCode = CMasterMgr::GetInstance().QuitTwitter(onlineRoleData);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL SyncMasterDataToGS(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32 nRetCode = 0;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_STUDENT_DATA     *pStudentData = NULL;

    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);

    nRetCode = CMasterMgr::GetInstance().IsMaster(onlineRoleData.pRoleData->dwRoleID);
    if (nRetCode)
    {
        pTeacherData = CMasterMgr::GetInstance().GetTeacherData(onlineRoleData.pRoleData->dwRoleID);
        LOG_PROCESS_ERROR(pTeacherData);

        nRetCode = CMessageHandler::GetInstance().DoGSMstSyncState(onlineRoleData.dwGSSessionID, onlineRoleData.pRoleData->dwRoleID, MASTER_TEACHER);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CMessageHandler::GetInstance().DoGSMstSyncData(onlineRoleData.dwGSSessionID, onlineRoleData.dwCliSessionID, onlineRoleData.pRoleData->dwRoleID, *pTeacherData);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CMasterMgr::GetInstance().IsStudent(onlineRoleData.pRoleData->dwRoleID);
    if (nRetCode)
    {
        pStudentData = CMasterMgr::GetInstance().GetStudentData(onlineRoleData.pRoleData->dwRoleID);
        LOG_PROCESS_ERROR(pStudentData);

        pTeacherData = CMasterMgr::GetInstance().GetTeacherData(pStudentData->dwTeacherID);
        LOG_PROCESS_ERROR(pTeacherData);

        nRetCode = CMessageHandler::GetInstance().DoGSMstSyncState(onlineRoleData.dwGSSessionID, onlineRoleData.pRoleData->dwRoleID, MASTER_STUDENT);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CMessageHandler::GetInstance().DoGSMstSyncData(onlineRoleData.dwGSSessionID, onlineRoleData.dwCliSessionID, onlineRoleData.pRoleData->dwRoleID, *pTeacherData);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL RoleDel(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    return FALSE;
}

INT32   GetMasterCD(INT32 nLevel)
{
    INT32       nCD = 0;

    LOG_PROCESS_ERROR(nLevel > 0);

    if (nLevel < 10)
        return 1800;
    else
        return 2 * nLevel*nLevel + 6 * nLevel - 200;
Exit0:
    return 0;
}
