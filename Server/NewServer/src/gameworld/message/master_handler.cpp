#include "stdafx.h"
#include "message_handler.h"
#include "imessage_notifier.h"
#include "master.h"
#include "role.h"
#include "c2g_message.h"
#include "g2m_message.h"

#ifndef _ROBOT

using namespace ExternalMessage;

BOOL CCliMessageHandler::DoMSMstSubmitTeacherAppAck(UINT8 bySourceType, UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_MST_SUBMIT_TEACHER_APP_ACK* pMsg = NULL;

    pMsg = (InternalMessage::G2M_MST_SUBMIT_TEACHER_APP_ACK*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_MST_SUBMIT_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_mst_submit_teacher_app_ack;
    pMsg->bySourceType = bySourceType;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_MST_SUBMIT_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSMstAgreeTeacherAppAck(UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_MST_AGREE_TEACHER_APP_ACK* pMsg = NULL;

    pMsg = (InternalMessage::G2M_MST_AGREE_TEACHER_APP_ACK*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_MST_AGREE_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_mst_agree_teacher_app_ack;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_MST_AGREE_TEACHER_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSMstSubmitStudAppAck(UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_MST_SUBMIT_STUD_APP_ACK* pMsg = NULL;

    pMsg = (InternalMessage::G2M_MST_SUBMIT_STUD_APP_ACK*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_MST_SUBMIT_STUD_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_mst_submit_stud_app_ack;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_MST_SUBMIT_STUD_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSMstAgreeStudAppAck(UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_MST_AGREE_STUD_APP_ACK* pMsg = NULL;

    pMsg = (InternalMessage::G2M_MST_AGREE_STUD_APP_ACK*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_MST_AGREE_STUD_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_mst_agree_stud_app_ack;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_MST_AGREE_STUD_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSSyncMasterExp(UINT32 dwRoleID, INT64 llMasterExp)
{
    INT32       nRetCode = 0;
	InternalMessage::G2M_SYNC_MASTER_EXP* pMsg = NULL;

	pMsg = (InternalMessage::G2M_SYNC_MASTER_EXP*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_SYNC_MASTER_EXP));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_sync_master_exp;
    pMsg->dwRoleID = dwRoleID;
	pMsg->llMasterExp = llMasterExp;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_SYNC_MASTER_EXP));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSMasterCZ(UINT32 dwRoleID, UINT32 dwTargetID, INT32 nCZType, INT32 nCZLevel, const char *pcszText)
{
    INT32       nRetCode = 0;
    INT32       nSize = 0;
    InternalMessage::G2M_MASTER_CZ    *pMsg = NULL;

    LOG_PROCESS_ERROR(pcszText);
    nSize = strlen(pcszText) + 1;

    LOG_PROCESS_ERROR(sizeof(InternalMessage::G2M_MASTER_CZ) + nSize < MAX_MESSAGE_SIZE);

    pMsg = (InternalMessage::G2M_MASTER_CZ*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_master_cz;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwTargetID = dwTargetID;
    pMsg->byCZType = nCZType;
    pMsg->byCZLevel = nCZLevel;
    strxcpy(pMsg->szText, pcszText, nSize);

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_MASTER_CZ) + nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSTryToFindTeacher(UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_TRY_TO_FIND_TEACHER *pMsg = (InternalMessage::G2M_TRY_TO_FIND_TEACHER*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_TRY_TO_FIND_TEACHER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_try_to_find_teacher;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_TRY_TO_FIND_TEACHER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSMasterTwitter(UINT32 dwRoleID, BOOL bTwitter)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_MASTER_TWITTER *pMsg = (InternalMessage::G2M_MASTER_TWITTER*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_MASTER_TWITTER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_master_twitter;
    pMsg->dwRoleID = dwRoleID;
    pMsg->bTwitter = bTwitter;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_MASTER_TWITTER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CCliMessageHandler::DoSyncMaster(CRole *poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_MASTER* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_MASTER*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_MASTER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_master;
	pMsg->dwCD = poRole->GetMaster().GetCD();
	pMsg->byState = poRole->GetMaster().GetState();
    pMsg->byTwitter = poRole->GetMaster().GetTwitter();

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_MASTER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncMasterRewardExp(CRole *poRole)
{
	const TEACHER_REWARD* pTeacherRewardExpList = nullptr;
	INT32 nRetCode = 0;
	G2C_SYNC_MASTER_REWARD_EXP* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_MASTER_REWARD_EXP*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_MASTER_REWARD_EXP));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_master_reward_exp;
    pMsg->dwMasterID = poRole->GetMaster().GetStudentRewardMasterID();
	pMsg->nStudentRewardExp = poRole->GetMaster().GetStudentRewardExp();
	pMsg->nStudentRewardMoney = poRole->GetMaster().GetStudentRewardExp() * g_GWConfig.nStudentExp2MoneyRate / RATE_MAX;

	pTeacherRewardExpList = poRole->GetMaster().GetTeacherRewardList();
	LOG_PROCESS_ERROR(pTeacherRewardExpList);

	for (INT32 nIndex = 0; nIndex < MAX_STUDENT_NUM; nIndex++)
	{
		pMsg->TeacherRewardExp[nIndex].dwRoleID = pTeacherRewardExpList[nIndex].dwRoleID;
		pMsg->TeacherRewardExp[nIndex].nExp = pTeacherRewardExpList[nIndex].nExp;
		pMsg->TeacherRewardExp[nIndex].nMasterExp = pTeacherRewardExpList[nIndex].nMasterExp;
		pMsg->TeacherRewardExp[nIndex].nMoney = pTeacherRewardExpList[nIndex].nExp * g_GWConfig.nTeacherExp2MoneyRate / RATE_MAX;
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_MASTER_REWARD_EXP));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoReceiveMasterRewardExpAck(CRole *poRole, BOOL bSuccess)
{
	INT32 nRetCode = 0;
	G2C_RECEIVE_MASTER_REWARD_EXP_ACK* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_RECEIVE_MASTER_REWARD_EXP_ACK*)m_poPacketAllocator->Alloc(sizeof(G2C_RECEIVE_MASTER_REWARD_EXP_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_receive_master_reward_exp_ack;
	pMsg->bySuccess = bSuccess;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_RECEIVE_MASTER_REWARD_EXP_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

void CCliMessageHandler::OnMasterTwitter(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;

    C2G_MASTER_TWITTER  *pMsg = (C2G_MASTER_TWITTER*)pData;

    if (poRole->GetMaster().IsTeacher())
    {
        poRole->GetMaster().SetTwitter(pMsg->byTwitter);
    }
    else if (poRole->GetMaster().IsStudent())
    {
        LOG_PROCESS_ERROR(FALSE);
    }
    else
    {
        LOG_PROCESS_ERROR(pMsg->byTwitter == FALSE);
        poRole->GetMaster().SetTwitter(pMsg->byTwitter);
    }

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    return;
}

void CCliMessageHandler::OnReceiveMasterRewardExp(CRole *poRole, const char *pData)
{
	INT32 nRetCode = 0;
	C2G_RECEIVE_MASTER_REWARD_EXP* pMsg = (C2G_RECEIVE_MASTER_REWARD_EXP*)pData;

	if (pMsg->dwRoleID == poRole->GetObjID())
	{
		LOG_PROCESS_ERROR(!poRole->GetMaster().IsStudent());

		INT32 nRewardExp = poRole->GetMaster().GetStudentRewardExp();
		LOG_PROCESS_ERROR(nRewardExp > 0);

		poRole->AddExp(nRewardExp, FALSE, TRUE);
		poRole->GetItemBag()->AddMoney(MONEYTYPE_02, nRewardExp * g_GWConfig.nStudentExp2MoneyRate / RATE_MAX,	MONEYLOG_STUDENT_REWARD, 0);
		poRole->GetMaster().ClearStudentRewardExp();
	}
	else
	{
		LOG_PROCESS_ERROR(!poRole->GetMaster().IsMyStudent(pMsg->dwRoleID));

		INT32 nRewardExp = poRole->GetMaster().GetTeacherRewardExp(pMsg->dwRoleID);
		LOG_PROCESS_ERROR(nRewardExp > 0);

		INT32 nMasterExp = poRole->GetMaster().GetMasterExp(pMsg->dwRoleID);
		LOG_PROCESS_ERROR(nMasterExp > 0);

		poRole->AddExp(nRewardExp, FALSE, TRUE);
		poRole->GetItemBag()->AddMoney(MONEYTYPE_02, nRewardExp * g_GWConfig.nTeacherExp2MoneyRate / RATE_MAX, MONEYLOG_TEACHER_REWARD, 0);
		poRole->GetItemBag()->AddMoney(MONEYTYPE_MASTER_EXP, nMasterExp, MONEYLOG_TEACHER_REWARD, 0);
		poRole->GetMaster().ClearTeacherReward(pMsg->dwRoleID);
	}

	nRetCode = DoReceiveMasterRewardExpAck(poRole, TRUE);
	LOG_CHECK_ERROR(nRetCode);

	return;
Exit0:
	nRetCode = DoReceiveMasterRewardExpAck(poRole, FALSE);
	LOG_CHECK_ERROR(nRetCode);

	KICK_ROLE(poRole, ktLogged);
	return;
}

void CCliMessageHandler::OnQueryMasterRewardExp(CRole *poRole, const char *pData)
{
	INT32 nRetCode = 0;

	nRetCode = DoSyncMasterRewardExp(poRole);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}


#endif //_ROBOT
