#include "stdafx.h"
#include "c2m_message.h"
#include "m2c_message.h"
#include "message_handler.h"

#include "manageserver.h"
#include "master/master_mgr.h"
#include "./onlinerole/online_role_data_mgr.h"
#include "role_data_mgr.h"
#include "./client/user.h"
#include "master_gift.h"
#include "mail/mail_mgr.h"
#include "game_world_config.h"

using namespace ExternalMessage;

void CMessageHandler::OnClientMstTeacherIgnore(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    C2M_MASTER_TEACHER_IGNORE  *pMsg = (C2M_MASTER_TEACHER_IGNORE*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole->dwBeDeterminedRoleID == pMsg->dwRoleID);

    pstOnlineRole->dwBeDeterminedRoleID = 0;

    PushRoleInfoToTeacher(pMsg->dwRoleID);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    return;
}

void CMessageHandler::OnClientMstGetRecruitList(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32 nResult = errUnknownError;
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
    PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->byLevel < GRADUATE_LEVEL, errMasterHighLevel);         // tq_log

    nRetCode = DoClientMstGetRecruitList(pstOnlineRole->dwCliSessionID, pstOnlineRole);
    LOG_PROCESS_ERROR(nRetCode);
    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstGetSoloList(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32 nResult = errUnknownError;
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    nRetCode = DoClientMstGetSoloList(pstOnlineRole);
    LOG_PROCESS_ERROR(nRetCode);
    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstStudSubmitApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nMessageSize = 0;
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA *pOnlineMaster = NULL;
    C2M_MASTER_STUD_SUBMIT_APP* pMsg = (C2M_MASTER_STUD_SUBMIT_APP*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);           // student
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pOnlineMaster = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTeacherID);
    PROCESS_ERROR_RET_CODE(pOnlineMaster, errMasterTeacherNotOnline);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineMaster->pUser->eState == USER_STATE_INGAME, errMasterTeacherNotOnline);
    LOG_PROCESS_ERROR(pOnlineMaster->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineMaster->pRoleData->byGroup == pstOnlineRole->pRoleData->byGroup, errMasterRoleNotExist);

    // pre condition
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && pstOnlineRole->pRoleData->byLevel < GRADUATE_LEVEL);
    PROCESS_ERROR_RET_CODE(pstOnlineRole->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterApplayMstCD);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pstOnlineRole->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pOnlineMaster->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(nRetCode, errMasterNotMaster);                               // tq_log

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pOnlineMaster);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);

    PROCESS_ERROR_RET_CODE(pstOnlineRole->byMasterSendAppNum < MAX_MASTER_SEND_APP_NUM, errMasterStudAppFull);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineMaster->byMasterRecvAppNum < MAX_MASTER_RECV_APP_NUM, errMasterTargetStudAppFull);

    for (INT32 nIndex = 0; nIndex < pOnlineMaster->byMasterRecvAppNum; ++nIndex)
        PROCESS_ERROR_RET_CODE(pOnlineMaster->arrMasterRecvApp[nIndex] != pstOnlineRole->pRoleData->dwRoleID, errMasterAlreadyApply);

    nRetCode = CMessageHandler::GetInstance().DoGSSubmitStudApp(pstOnlineRole->dwGSSessionID, pMsg->dwTeacherID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstAgreeStudApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA *pOnlineStudent = NULL;
    C2M_MASTER_AGREE_STUD_APP* pMsg = (C2M_MASTER_AGREE_STUD_APP*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);       // master
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pOnlineStudent = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentID);
    PROCESS_ERROR_RET_CODE(pOnlineStudent, errMasterStudNotOnline);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineStudent->pUser->eState == USER_STATE_INGAME, errMasterStudNotOnline);
    LOG_PROCESS_ERROR(pOnlineStudent->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->byGroup == pOnlineStudent->pRoleData->byGroup, errMasterRoleNotExist);

    // pre condition
    PROCESS_ERROR_RET_CODE(pOnlineStudent->pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && pOnlineStudent->pRoleData->byLevel < GRADUATE_LEVEL, errMasterStudHighLevel);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineStudent->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterApplayMstCD);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pOnlineStudent->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pstOnlineRole);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);

    nRetCode = CMessageHandler::GetInstance().DoGSMstAgreeStudApp(pstOnlineRole->dwGSSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwStudentID);
    LOG_CHECK_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstRefuseStudApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32 nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA     *pStudentRole = NULL;
    C2M_MASTER_REFUSE_STUD_APP* pMsg = (C2M_MASTER_REFUSE_STUD_APP*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    MasterCancelRecvApp(*pstOnlineRole, pMsg->dwStudentID);

    nRetCode = CMessageHandler::GetInstance().DoClientMstRefuseAppAck(pstOnlineRole->dwCliSessionID, pMsg->dwStudentID);
    LOG_PROCESS_ERROR(nRetCode);

    pStudentRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentID);
    PROCESS_ERROR(pStudentRole);
    LOG_PROCESS_ERROR(pStudentRole->pRoleData);

    MasterCancelSendApp(*pStudentRole, pstOnlineRole->pRoleData->dwRoleID);

    nRetCode = CMessageHandler::GetInstance().DoClientMstRefuseAppNtf(pStudentRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    return ;
}

void CMessageHandler::OnClientMstCancelStudApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA     *pTeacherRole = NULL;
    C2M_MASTER_CANCEL_STUD_APP* pMsg = (C2M_MASTER_CANCEL_STUD_APP*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    MasterCancelSendApp(*pstOnlineRole, pMsg->dwTeacherID);

    nRetCode = CMessageHandler::GetInstance().DoClientMstCancelAppAck(pstOnlineRole->dwCliSessionID, pMsg->dwTeacherID);
    LOG_PROCESS_ERROR(nRetCode);

    pTeacherRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTeacherID);
    LOG_PROCESS_ERROR(pTeacherRole);
    LOG_PROCESS_ERROR(pTeacherRole->pRoleData);

    MasterCancelRecvApp(*pTeacherRole, pstOnlineRole->pRoleData->dwRoleID);

    nRetCode = CMessageHandler::GetInstance().DoClientMstCancelAppNtf(pTeacherRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstTeacherSubmitApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nMessageSize = 0;
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA *pOnlineStudent = NULL;
    C2M_MASTER_TEACHER_SUBMIT_APP* pMsg = (C2M_MASTER_TEACHER_SUBMIT_APP*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);           // student
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pOnlineStudent = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentID);
    PROCESS_ERROR_RET_CODE(pOnlineStudent, errMasterStudentNotOnline);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineStudent->pUser->eState == USER_STATE_INGAME, errMasterStudentNotOnline);
    LOG_PROCESS_ERROR(pOnlineStudent->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineStudent->pRoleData->byGroup == pstOnlineRole->pRoleData->byGroup, errMasterRoleNotExist);

    // pre condition
    PROCESS_ERROR_RET_CODE(pOnlineStudent->pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && pOnlineStudent->pRoleData->byLevel < GRADUATE_LEVEL, errMasterStudHighLevel);
    PROCESS_ERROR_RET_CODE(pOnlineStudent->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterApplayMstCD);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pOnlineStudent->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pstOnlineRole);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);

    PROCESS_ERROR_RET_CODE(pstOnlineRole->byMasterSendAppNum < MAX_MASTER_SEND_APP_NUM, errMasterStudAppFull);
    PROCESS_ERROR_RET_CODE(pOnlineStudent->byMasterRecvAppNum < MAX_MASTER_RECV_APP_NUM, errMasterTargetStudAppFull);

    for (INT32 nIndex = 0; nIndex < pOnlineStudent->byMasterRecvAppNum; ++nIndex)
        PROCESS_ERROR_RET_CODE(pOnlineStudent->arrMasterRecvApp[nIndex] != pstOnlineRole->pRoleData->dwRoleID, errMasterAlreadyApply);

    nRetCode = CMessageHandler::GetInstance().DoGSSubmitTeacherApp(pstOnlineRole->dwGSSessionID, pMsg->bySourceType, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwStudentID);
    LOG_CHECK_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstAgreeTeacherApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA *pOnlineTeacher = NULL;
    C2M_MASTER_AGREE_TEACHER_APP* pMsg = (C2M_MASTER_AGREE_TEACHER_APP*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pOnlineTeacher = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTeacherID);
    PROCESS_ERROR_RET_CODE(pOnlineTeacher, errMasterTeacherNotOnline);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineTeacher->pUser->eState == USER_STATE_INGAME, errMasterTeacherNotOnline);
    LOG_PROCESS_ERROR(pOnlineTeacher->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->byGroup == pOnlineTeacher->pRoleData->byGroup, errMasterRoleNotExist);

    // pre condition
    PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && pstOnlineRole->pRoleData->byLevel < GRADUATE_LEVEL, errMasterStudHighLevel);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterApplayMstCD);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pstOnlineRole->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pOnlineTeacher->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pOnlineTeacher);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);

    nRetCode = CMessageHandler::GetInstance().DoGSMstAgreeTeacherApp(pstOnlineRole->dwGSSessionID, pMsg->dwTeacherID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstRefuseTeacherApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32 nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA     *pTeacherRole = NULL;
    C2M_MASTER_REFUSE_TEACHER_APP* pMsg = (C2M_MASTER_REFUSE_TEACHER_APP*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    MasterCancelRecvApp(*pstOnlineRole, pMsg->dwTeacherID);

    nRetCode = CMessageHandler::GetInstance().DoClientMstRefuseTeacherAppAck(pstOnlineRole->dwCliSessionID, pMsg->dwTeacherID);
    LOG_PROCESS_ERROR(nRetCode);

    pTeacherRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTeacherID);
    PROCESS_ERROR(pTeacherRole);
    LOG_PROCESS_ERROR(pTeacherRole->pRoleData);

    MasterCancelSendApp(*pTeacherRole, pstOnlineRole->pRoleData->dwRoleID);

    nRetCode = CMessageHandler::GetInstance().DoClientMstRefuseTeacherAppNtf(pTeacherRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    return;
}

void CMessageHandler::OnClientMstCancelTeacherApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA     *pStudentRole = NULL;
    C2M_MASTER_CANCEL_TEACHER_APP* pMsg = (C2M_MASTER_CANCEL_TEACHER_APP*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    MasterCancelSendApp(*pstOnlineRole, pMsg->dwStudentID);

    nRetCode = CMessageHandler::GetInstance().DoClientMstCancelTeacherAppAck(pstOnlineRole->dwCliSessionID, pMsg->dwStudentID);
    LOG_PROCESS_ERROR(nRetCode);

    pStudentRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentID);
    LOG_PROCESS_ERROR(pStudentRole);
    LOG_PROCESS_ERROR(pStudentRole->pRoleData);

    MasterCancelRecvApp(*pStudentRole, pstOnlineRole->pRoleData->dwRoleID);

    nRetCode = CMessageHandler::GetInstance().DoClientMstCancelTeacherAppNtf(pStudentRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstLeaveTeacher(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA     *pOnlineTeacherRole = NULL;
    MS_ROLE_DATA            *pTeacherRole = NULL;
    MS_STUDENT_DATA         *pStudentData = NULL;
    MS_TEACHER_DATA         *pTeacherData = NULL;
    C2M_MASTER_LEAVE_TEACHER* pMsg = (C2M_MASTER_LEAVE_TEACHER*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    nRetCode = CMasterMgr::GetInstance().IsStudent(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    pStudentData = CMasterMgr::GetInstance().GetStudentData(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(pStudentData);

    pOnlineTeacherRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pStudentData->dwTeacherID);
    if (pOnlineTeacherRole)
        pTeacherRole = pOnlineTeacherRole->pRoleData;
    else
        pTeacherRole = CRoleDataMgr::GetInstance().FindRole(pStudentData->dwTeacherID);
    LOG_CHECK_ERROR(pTeacherRole);

    nRetCode = CMasterMgr::GetInstance().DelMasterRelation(pStudentData->dwTeacherID, *pstOnlineRole->pRoleData, FALSE);
    LOG_PROCESS_ERROR(nRetCode);

    // update cd
    pstOnlineRole->dwMasterCD = CManageServer::GetInstance().GetGameTime() + GetMasterCD(pstOnlineRole->pRoleData->byLevel);
    nRetCode = CMessageHandler::GetInstance().DoGSMstUpdateData(pstOnlineRole->pRoleData->dwRoleID, MST_SYNC_GS_CD, pstOnlineRole->dwMasterCD);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSMstUpdateData(pstOnlineRole->pRoleData->dwRoleID, MST_SYNC_PUNISH_EXP, 0);
    LOG_CHECK_ERROR(nRetCode);

    pTeacherData = CMasterMgr::GetInstance().GetTeacherData(pStudentData->dwTeacherID);
    LOG_PROCESS_ERROR(pTeacherData);

    nRetCode = CMessageHandler::GetInstance().DoClientMstLeaveTeacherAck(pstOnlineRole->dwCliSessionID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientMstLeaveTeacherNtf(*pTeacherData, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstFireStud(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    INT32       nDecMasterExp = 0;
    MS_ONLINE_ROLE_DATA     *pOnlineStudentRole = NULL;
    MS_ROLE_DATA            *pStudentRole = NULL;
    MS_STUDENT_DATA         *pStudentData = NULL;
    MS_TEACHER_DATA         *pTeacherData = NULL;
    C2M_MASTER_FIRE_STUD* pMsg = (C2M_MASTER_FIRE_STUD*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    PROCESS_ERROR_RET_CODE(pstOnlineRole->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterFireStudentCD);

    pStudentData = CMasterMgr::GetInstance().GetStudentData(pMsg->dwStudentID);
    LOG_PROCESS_ERROR(pStudentData);
    LOG_PROCESS_ERROR(pStudentData->dwTeacherID == pstOnlineRole->pRoleData->dwRoleID);

    pOnlineStudentRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pStudentData->dwRoleID);
    if (pOnlineStudentRole)
        pStudentRole = pOnlineStudentRole->pRoleData;
    else
        pStudentRole = CRoleDataMgr::GetInstance().FindRole(pMsg->dwStudentID);
    LOG_PROCESS_ERROR(pStudentRole);

    nRetCode = CMasterMgr::GetInstance().DelMasterRelation(pstOnlineRole->pRoleData->dwRoleID, *pStudentRole, FALSE);
    LOG_PROCESS_ERROR(nRetCode);

    // teacher add cd
    pstOnlineRole->dwMasterCD = CManageServer::GetInstance().GetGameTime() + GetMasterCD(pStudentRole->byLevel);

    nRetCode = CMessageHandler::GetInstance().DoGSMstUpdateData(pstOnlineRole->pRoleData->dwRoleID, MST_SYNC_GS_CD, pstOnlineRole->dwMasterCD);
    LOG_CHECK_ERROR(nRetCode);

    nDecMasterExp = (INT64)pStudentRole->byLevel * g_GWConfig.nMasterFireStudentPercent / RATE_MAX;

    nRetCode = CMessageHandler::GetInstance().DoGSMstUpdateData(pstOnlineRole->pRoleData->dwRoleID, MST_SYNC_MASTER_EXP_DEC, nDecMasterExp);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSMstUpdateData(pstOnlineRole->pRoleData->dwRoleID, MST_SYNC_PUNISH_EXP, pMsg->dwStudentID);
    LOG_CHECK_ERROR(nRetCode);
        
    pTeacherData = CMasterMgr::GetInstance().GetTeacherData(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(pTeacherData);

    nRetCode = CMessageHandler::GetInstance().DoClientMstFireStudentAck(pstOnlineRole->dwCliSessionID, pMsg->dwStudentID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientMstFireStudentNtf(*pTeacherData, pMsg->dwStudentID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_STUDENT_DATA     *pStudentData = NULL;
    UINT32      dwTeacherID = 0;
    C2M_MASTER_INFO *pMsg = (C2M_MASTER_INFO*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pStudentData = CMasterMgr::GetInstance().GetStudentData(pstOnlineRole->pRoleData->dwRoleID);
    if (pStudentData)
        dwTeacherID = pStudentData->dwTeacherID;
    else
        dwTeacherID = pstOnlineRole->pRoleData->dwRoleID;

    pTeacherData = CMasterMgr::GetInstance().GetTeacherData(dwTeacherID);
    LOG_PROCESS_ERROR(pTeacherData);

    nRetCode = CMessageHandler::GetInstance().DoClientMasterInfo(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, *pTeacherData);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstCZ(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    INT32       nLevel = 0;
    MS_ROLE_DATA    *pTargetRole = NULL;
    const MS_MASTER_GIFT_TEMPLATE     *pTemplate = NULL;
    C2M_MASTER_CZ *pMsg = (C2M_MASTER_CZ*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR(pMsg->byCZType > MASTER_CONG_BEGIN && pMsg->byCZType < MASTER_CONG_END);

    pTargetRole = CRoleDataMgr::GetInstance().FindRole(pMsg->dwTargetID);
    LOG_PROCESS_ERROR(pTargetRole);

    LOG_PROCESS_ERROR(pMsg->byCZLevel < GRADUATE_LEVEL);
    LOG_PROCESS_ERROR(pMsg->byCZLevel <= pTargetRole->byLevel);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errMstCannotCZ);

    nLevel = CMasterMgr::GetInstance().GetCZ(pMsg->dwTargetID, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(nLevel < pMsg->byCZLevel);

    pTemplate = GetMasterGiftTemplate(pMsg->byCZLevel);
    LOG_PROCESS_ERROR_RET_CODE(pTemplate, errMasterNotCZLevel);

    nRetCode = CMessageHandler::GetInstance().DoGSMstCZ(pstOnlineRole->dwGSSessionID, pMsg->dwRoleID, pMsg->dwTargetID,
        pMsg->byCZType, pMsg->byCZLevel, pTemplate->m_arrCZMoneyConsume[pMsg->byCZType - 1], pMsg->szText);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("master cz. t[%s][%d] cz s[%s][%d]", pstOnlineRole->pRoleData->szRoleName, (INT32)pstOnlineRole->pRoleData->byLevel, 
        pTargetRole->szRoleName, (INT32)pTargetRole->byLevel);

    return ;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

void CMessageHandler::OnClientMstGrad(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_TEACHER_DATA     *pTeacherData = NULL;
    MS_STUDENT_DATA     *pStudentData = NULL;
    MS_ROLE_DATA        *pTeacherRoleData = NULL;
    C2M_MASTER_GRAD  *pMsg = (C2M_MASTER_GRAD *)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pStudentData = CMasterMgr::GetInstance().GetStudentData(pstOnlineRole->pRoleData->dwRoleID);
    PROCESS_ERROR(pStudentData);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData->byLevel >= GRADUATE_LEVEL);

    pTeacherData = CMasterMgr::GetInstance().GetTeacherData(pStudentData->dwTeacherID);
    LOG_PROCESS_ERROR(pTeacherData);

    nRetCode = CMasterMgr::GetInstance().DelMasterRelation(pStudentData->dwTeacherID, *pstOnlineRole->pRoleData, TRUE);
    LOG_PROCESS_ERROR(nRetCode);

    pTeacherRoleData = CRoleDataMgr::GetInstance().FindRole(pStudentData->dwTeacherID);
    LOG_CHECK_ERROR(pTeacherRoleData);
    if (pTeacherRoleData)
    {
        nRetCode = CMailMgr::GetInstance().SendSystemMail(pTeacherRoleData, g_GWConfig.nMasterGradGiftMailIDForT);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CMailMgr::GetInstance().SendSystemMail(pstOnlineRole->pRoleData, g_GWConfig.nMasterGradGiftMailIDForS);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientMstGrad(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientMstGradNtf(*pTeacherData, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    if(nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return ;
}

BOOL CMessageHandler::DoClinetMstTwitterAck(UINT32 dwClientSessionID, UINT8 byTwitter)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    M2C_MASTER_TWITTER_ACK *pMsg = (M2C_MASTER_TWITTER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_TWITTER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_twitter_ack;
    pMsg->byTwitter = byTwitter;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_TWITTER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstHelloTeacher(UINT32 dwClientSessionID, UINT32 dwRoleID, UINT8 byLevel)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    M2C_MASTER_HELLO_TEACHER *pMsg = (M2C_MASTER_HELLO_TEACHER*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_HELLO_TEACHER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_hello_teacher;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byLevel = byLevel;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_HELLO_TEACHER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstGetRecruitList(UINT32 dwClientSessionID, MS_ONLINE_ROLE_DATA *pstOnlineRole)
{
    INT32   nRetCode = 0;
    UINT32  dwBeginRoleID = 0;
    UINT32  dwRoleID = 0;
    M2C_MASTER_RECRUIT_LIST *pMsg = NULL; 

    LOG_PROCESS_ERROR(sizeof(M2C_MASTER_RECRUIT_LIST) + sizeof(M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO) * SEND_MASTER_INFO_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2C_MASTER_RECRUIT_LIST*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->byCount = 0;
    //for (INT32 i = 0; i < SEND_MASTER_INFO_NUM; ++i)
    while (pMsg->byCount < SEND_MASTER_INFO_NUM)
    {
        if (CMasterMgr::GetInstance().GetRecruitData(pstOnlineRole->pRoleData->byGroup, dwRoleID))
        {
            if (dwBeginRoleID == dwRoleID)
            {
                break;
            }
            if (!dwBeginRoleID)
            {
                dwBeginRoleID = dwRoleID;
            }
            MS_ONLINE_ROLE_DATA *pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
            LOG_CHECK_ERROR(pOnlineRoleData->pRoleData->byGroup == pstOnlineRole->pRoleData->byGroup);
            if (pOnlineRoleData && (pOnlineRoleData->pRoleData->byGroup == pstOnlineRole->pRoleData->byGroup))        // online
            {
                LOG_PROCESS_ERROR(pOnlineRoleData->pRoleData);

                pMsg->szListData[pMsg->byCount].dwRoleID = dwRoleID;
                pMsg->szListData[pMsg->byCount].byClass = pOnlineRoleData->pRoleData->byClass;
                pMsg->szListData[pMsg->byCount].byLevel = pOnlineRoleData->pRoleData->byLevel;
				pMsg->szListData[pMsg->byCount].llMasterExp = pOnlineRoleData->llMasterExp;
                ++pMsg->byCount;
            }
        }
        else
            break;
    }

    pMsg->wMsg = m2c_master_recruit_list;
    pMsg->wSize = sizeof(M2C_MASTER_RECRUIT_LIST) + pMsg->byCount * sizeof(M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO);

    nRetCode = _SendClient(pstOnlineRole->dwCliSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

//BOOL CMessageHandler::DoClientMstRecruitAck(UINT32 dwClientSessionID, UINT32 dwType)
//{
//    INT32 nRetCode = 0;
//
//    M2C_MASTER_RECRUIT_ACK* pMsg = (M2C_MASTER_RECRUIT_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_RECRUIT_ACK));
//    LOG_PROCESS_ERROR(pMsg);
//
//    pMsg->wMsg = m2c_master_recruit_ack;
//    pMsg->dwType = dwType;
//
//    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_RECRUIT_ACK));
//    LOG_PROCESS_ERROR(nRetCode);
//
//    return TRUE;
//Exit0:
//    if (pMsg)
//        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
//    return FALSE;
//}

BOOL CMessageHandler::DoClientMstGetSoloList(MS_ONLINE_ROLE_DATA *pstOnlineRole)
{
    INT32   nRetCode = errUnknownError;
    UINT32  dwBeginRoleID = 0;
    UINT32  dwRoleID = 0;
    M2C_MASTER_GET_SOLO_LIST_ACK *pMsg = NULL; 

    LOG_PROCESS_ERROR(sizeof(M2C_MASTER_GET_SOLO_LIST_ACK) + sizeof(M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO) * SEND_SOLO_INFO_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2C_MASTER_GET_SOLO_LIST_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->byCount = 0;
    //for (INT32 i = 0; i < SEND_MASTER_INFO_NUM; ++i)
    while (pMsg->byCount < SEND_SOLO_INFO_NUM)
    {
        if (CMasterMgr::GetInstance().GetSolo(pstOnlineRole->pRoleData->byGroup, dwRoleID))
        {
            if (dwBeginRoleID == dwRoleID)
            {
                break;
            }
            if (!dwBeginRoleID)
            {
                dwBeginRoleID = dwRoleID;
            }
            MS_ONLINE_ROLE_DATA *pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
            LOG_CHECK_ERROR(pOnlineRoleData->pRoleData->byGroup == pstOnlineRole->pRoleData->byGroup);
            if (pOnlineRoleData && (pOnlineRoleData->pRoleData->byGroup == pstOnlineRole->pRoleData->byGroup))        // online
            {
                LOG_PROCESS_ERROR(pOnlineRoleData->pRoleData);
                LOG_CHECK_ERROR(pOnlineRoleData->pRoleData->byLevel < GRADUATE_LEVEL);
                if (pOnlineRoleData->pRoleData->byLevel >= GRADUATE_LEVEL)
                    continue;
                pMsg->szListData[pMsg->byCount].dwRoleID = dwRoleID;
                pMsg->szListData[pMsg->byCount].byClass = pOnlineRoleData->pRoleData->byClass;
                pMsg->szListData[pMsg->byCount].byLevel = pOnlineRoleData->pRoleData->byLevel;
                ++pMsg->byCount;
            }
        }
        else
            break;
    }

    pMsg->wMsg = m2c_master_get_solo_list_ack;
    pMsg->wSize = sizeof(M2C_MASTER_GET_SOLO_LIST_ACK) + pMsg->byCount * sizeof(M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO);

    nRetCode = _SendClient(pstOnlineRole->dwCliSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMasterInfo(UINT32 dwClientSessionID, UINT32 dwRoleID, MS_TEACHER_DATA &data)
{
    INT32       nRetCode = 0;
    INT32       nCZ = 0;
    MS_STUDENT_DATA *pStudentData = NULL;
    MS_ROLE_DATA    *pRole = NULL;
    M2C_MASTER_INFO *pMsg = NULL;

    pMsg = (M2C_MASTER_INFO*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(sizeof(M2C_MASTER_INFO) + MAX_STUDENT_NUM * sizeof(MASTER_STUDENT_INFO) < MAX_MESSAGE_SIZE);

    pMsg->wMsg = m2c_master_info;

    pMsg->dwTeacherID = data.dwRoleID;
    pMsg->byTeacherLevel = 0;
    pRole = CRoleDataMgr::GetInstance().FindRole(data.dwRoleID);
    if (pRole)
        pMsg->byTeacherLevel = pRole->byLevel;

    pMsg->byStudentNum = 0;
    for (INT32 nIndex = 0; nIndex < data.byStudNum; ++nIndex)
    {
        if (data.arrStudentID[nIndex] == dwRoleID)
            continue;
        
        pMsg->arrInfo[pMsg->byStudentNum].byCZCount = 0;

        pRole = CRoleDataMgr::GetInstance().FindRole(data.arrStudentID[nIndex]);
        if (pRole)
        {
            pMsg->arrInfo[pMsg->byStudentNum].byLevel = pRole->byLevel;
            pMsg->arrInfo[pMsg->byStudentNum].dwStudentID = pRole->dwRoleID;

            // For a mentor, find and send the congratulation level
            if (dwRoleID == data.dwRoleID)
            {
                nCZ = CMasterMgr::GetInstance().GetCZ(pRole->dwRoleID, dwRoleID);
                pMsg->arrInfo[pMsg->byStudentNum].byCZCount = nCZ;
            }
        }
        else
        {
            pMsg->arrInfo[pMsg->byStudentNum].dwStudentID = data.arrStudentID[nIndex];
            pMsg->arrInfo[pMsg->byStudentNum].byLevel = 0;
        }
  
        ++pMsg->byStudentNum;
    }

    pMsg->wSize = sizeof(M2C_MASTER_INFO) + pMsg->byStudentNum * sizeof(MASTER_STUDENT_INFO);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMasterSyncLvl(MS_TEACHER_DATA &data, UINT32 dwRoleID, INT32 nLevel)
{
    INT32 nRetCode = 0;
    M2C_MASTER_SYNC_LVL* pMsg = (M2C_MASTER_SYNC_LVL*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_SYNC_LVL));
    LOG_PROCESS_ERROR(pMsg);

    //
    pMsg->wMsg = m2c_master_sync_lvl;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byLevel = nLevel;

    nRetCode = _BroadcastMaster(&data, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_SYNC_LVL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstStudSubmitAppAck(UINT32 dwClientSessionID, UINT32 dwMasterID)
{
    INT32 nRetCode = 0;
    M2C_MASTER_STUD_SUBMIT_APP_ACK* pMsg = (M2C_MASTER_STUD_SUBMIT_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_STUD_SUBMIT_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    //
    pMsg->wMsg = m2c_master_stud_submit_app_ack;
    pMsg->dwMasterID = dwMasterID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_STUD_SUBMIT_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstStudSubmitAppNtf(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32 nRetCode = 0;
    M2C_MASTER_STUD_SUBMIT_APP_NTF* pMsg = NULL; 

    pMsg = (M2C_MASTER_STUD_SUBMIT_APP_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_STUD_SUBMIT_APP_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_stud_submit_app_ntf;
    pMsg->dwRoleID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_STUD_SUBMIT_APP_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstAgreeStudAppAck(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_AGREE_STUD_APP_ACK* pMsg = (M2C_MASTER_AGREE_STUD_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_AGREE_STUD_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_agree_stud_app_ack;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_AGREE_STUD_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstRefuseAppAck(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_REFUSE_STUD_APP_ACK* pMsg = (M2C_MASTER_REFUSE_STUD_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_REFUSE_STUD_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    //
    pMsg->wMsg = m2c_master_refuse_stud_app_ack;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_REFUSE_STUD_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstRefuseAppNtf(UINT32 dwClientSessionID, UINT32 dwTeacherID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_REFUSE_STUD_APP_NTF* pMsg = (M2C_MASTER_REFUSE_STUD_APP_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_REFUSE_STUD_APP_NTF));
    LOG_PROCESS_ERROR(pMsg);

    //
    pMsg->wMsg = m2c_master_refuse_stud_app_ntf;
    pMsg->dwTeacherID = dwTeacherID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_REFUSE_STUD_APP_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstCancelAppAck(UINT32 dwClientSessionID, UINT32 dwTeacherID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_CANCEL_STUD_APP_ACK* pMsg = (M2C_MASTER_CANCEL_STUD_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_CANCEL_STUD_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    //
    pMsg->wMsg = m2c_master_cancel_stud_app_ack;
    pMsg->dwTeacherID = dwTeacherID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_CANCEL_STUD_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstCancelAppNtf(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_CANCEL_STUD_APP_NTF* pMsg = (M2C_MASTER_CANCEL_STUD_APP_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_CANCEL_STUD_APP_NTF));
    LOG_PROCESS_ERROR(pMsg);

    //
    pMsg->wMsg = m2c_master_cancel_stud_app_ntf;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_CANCEL_STUD_APP_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstTeacherSubmitAppAck(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_TEACHER_SUBMIT_APP_ACK* pMsg = (M2C_MASTER_TEACHER_SUBMIT_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_TEACHER_SUBMIT_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_teacher_submit_app_ack;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_TEACHER_SUBMIT_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstTeacherSubmitAppNtf(UINT32 dwClientSessionID, INT32 nAppSourceType, UINT32 dwTeacherID, INT32 nLevel, INT32 nMasterExp)
{
    INT32       nRetCode = 0;
    M2C_MASTER_TEACHER_SUBMIT_APP_NTF* pMsg = NULL; 

    pMsg = (M2C_MASTER_TEACHER_SUBMIT_APP_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_TEACHER_SUBMIT_APP_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_teacher_submit_app_ntf;
    pMsg->bySourceType = nAppSourceType;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->nMasterExp = nMasterExp;
    pMsg->byLevel = nLevel;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_TEACHER_SUBMIT_APP_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstAgreeTeacherAppAck(UINT32 dwClientSessionID, UINT32 dwTeacherID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_AGREE_TEACHER_APP_ACK* pMsg = (M2C_MASTER_AGREE_TEACHER_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_AGREE_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_agree_teacher_app_ack;
    pMsg->dwTeacherID = dwTeacherID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_AGREE_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstRefuseTeacherAppAck(UINT32 dwClientSessionID, UINT32 dwTeacherID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_REFUSE_TEACHER_APP_ACK* pMsg = (M2C_MASTER_REFUSE_TEACHER_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_REFUSE_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_refuse_teacher_app_ack;
    pMsg->dwTeacherID = dwTeacherID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_REFUSE_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstRefuseTeacherAppNtf(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_REFUSE_TEACHER_APP_NTF* pMsg = (M2C_MASTER_REFUSE_TEACHER_APP_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_REFUSE_TEACHER_APP_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_refuse_teacher_app_ntf;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_REFUSE_TEACHER_APP_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstCancelTeacherAppAck(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_CANCEL_TEACHER_APP_ACK* pMsg = (M2C_MASTER_CANCEL_TEACHER_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_CANCEL_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_cancel_teacher_app_ack;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_CANCEL_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstCancelTeacherAppNtf(UINT32 dwClientSessionID, UINT32 dwTeacherID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_CANCEL_TEACHER_APP_NTF* pMsg = (M2C_MASTER_CANCEL_TEACHER_APP_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_CANCEL_TEACHER_APP_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_cancel_teacher_app_ntf;
    pMsg->dwTeacherID = dwTeacherID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_CANCEL_TEACHER_APP_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstAddStudentNtf(MS_TEACHER_DATA &data, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_ADD_STUDENT_NTF* pMsg = NULL;

    pMsg = (M2C_MASTER_ADD_STUDENT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_ADD_STUDENT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_add_student_ntf;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _BroadcastMaster(&data, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_ADD_STUDENT_NTF), dwStudentID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstDelStudentNtf(MS_TEACHER_DATA &data, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_DEL_STUDENT_NTF* pMsg = NULL;        // todo_tq to be del...

    pMsg = (M2C_MASTER_DEL_STUDENT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_DEL_STUDENT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_del_student_ntf;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _BroadcastMaster(&data, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_DEL_STUDENT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstClearInfo(UINT32 dwClientSessionID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_CLEAR_INFO* pMsg = (M2C_MASTER_CLEAR_INFO*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_CLEAR_INFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_clear_info;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_CLEAR_INFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstCZLevel(UINT32 dwClientSessionID, UINT32 dwRoleID, INT32 nCZLevel)
{
    INT32       nRetCode = 0;
    M2C_MASTER_CZ_LEVEL* pMsg = (M2C_MASTER_CZ_LEVEL*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_CZ_LEVEL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_cz_level;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byCZLevel = nCZLevel;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_CZ_LEVEL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstCZAck(UINT32 dwClientSessionID, UINT32 dwTargetRoleID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_CZ_ACK* pMsg = (M2C_MASTER_CZ_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_CZ_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_cz_ack;
    pMsg->dwTargetRoleID = dwTargetRoleID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_CZ_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstGrad(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_GRAD* pMsg = (M2C_MASTER_GRAD*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_GRAD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_grad;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_GRAD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstGradNtf(MS_TEACHER_DATA &data, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_GRAD* pMsg = (M2C_MASTER_GRAD*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_GRAD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_grad;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _BroadcastMaster(&data, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_GRAD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstLeaveTeacherAck(UINT32 dwClientSessionID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_LEAVE_TEACHER_ACK* pMsg = (M2C_MASTER_LEAVE_TEACHER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_LEAVE_TEACHER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_leave_teacher_ack;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_LEAVE_TEACHER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstLeaveTeacherNtf(MS_TEACHER_DATA &data, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_LEAVE_TEACHER_NTF* pMsg = (M2C_MASTER_LEAVE_TEACHER_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_LEAVE_TEACHER_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_leave_teacher_ntf;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _BroadcastMaster(&data, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_LEAVE_TEACHER_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstFireStudentAck(UINT32 dwClientSessionID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_FIRE_STUDENT_ACK* pMsg = (M2C_MASTER_FIRE_STUDENT_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_FIRE_STUDENT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_fire_student_ack;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_FIRE_STUDENT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMstFireStudentNtf(MS_TEACHER_DATA &data, UINT32 dwStudentID, UINT32 dwExceptRoleID)
{
    INT32       nRetCode = 0;
    M2C_MASTER_FIRE_STUDENT_NTF* pMsg = (M2C_MASTER_FIRE_STUDENT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_MASTER_FIRE_STUDENT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_master_fire_student_ntf;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _BroadcastMaster(&data, (PACKET_BUFFER*)pMsg, sizeof(M2C_MASTER_FIRE_STUDENT_NTF), dwExceptRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
