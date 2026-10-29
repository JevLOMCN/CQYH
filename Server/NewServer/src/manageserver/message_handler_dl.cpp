#include "stdafx.h"

#include "ms_session_mgr.h"
#include "o2m_message.h"
#include "m2o_message.h"
#include "common_message.h"

#include "time_ex.h"
#include "message_handler.h"
#include "server_config.h"
#include "role_data.h"
#include "onlinerole/online_role_data.h"
#include "manageserver.h"

using namespace InternalMessage;

void CMessageHandler::OnDLRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SDLSession* pDLSession{};
    INT32 nRetCode = 0;
    O2M_REGISTER* pMsg = (O2M_REGISTER*)pPacketBuffer;

    //check
    SDLSession& rDLSession = CMSSessionMgr::GetInstance().GetDLSession();
    LOG_PROCESS_ERROR( rDLSession.pSocketSession == pSocketSession );
    pDLSession = (SDLSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR( pDLSession );
    LOG_PROCESS_ERROR( pDLSession->pSocketSession == pSocketSession );
    LOG_PROCESS_ERROR(CMSSessionMgr::GetInstance().GetServerState() != sgsClosing);

    m_ServerInfo.DL.dwAddr = pMsg->dwAddr;
    m_ServerInfo.DL.wPort = pMsg->wPort;

    nRetCode = DoDLRegisterAck();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoServerInfo();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnDLReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    m_ServerInfo.DL.byReady = true;

    INF("DL is ready");

    _CheckAllReady();
}

void CMessageHandler::OnDLCloseAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INF("DL is closed");

	pSocketSession->Close();
}

BOOL CMessageHandler::DoDLRegisterAck(void)
{
    INT32 nRetCode = 0;
    M2O_REGISTER_ACK* pMsg = (M2O_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2O_REGISTER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2o_register_ack;

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_REGISTER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDLReload(void)
{
	INT32 nRetCode = 0;
	M2O_REGISTER_ACK* pMsg = (M2O_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2O_REGISTER_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_register_ack;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_REGISTER_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLChatMessage(UINT32 dwTime, INT32 nGroupID, UINT32 dwSrcID, UINT32 dwTargetID, const char* pcszMessage, INT32 nMessageSize)
{
	INT32 nRetCode = 0;
	INT32 nSize = sizeof(M2O_CHAT_MESSAGE) + nMessageSize; 

	M2O_CHAT_MESSAGE* pMsg = (M2O_CHAT_MESSAGE*)m_pPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_chat_message;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->dwSrcID = dwSrcID;
	pMsg->dwTargetID = dwTargetID;
	strxcpy(pMsg->chMessage, pcszMessage, nMessageSize);
	pMsg->nMessageSize = nMessageSize;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, nSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLLogItem(UINT32 dwRoleID, INT32 nGroupID, INT64 Guid,
    INT32 nTemplateID, INT32 nItemCount, INT32 nReason, INT32 nAction, UINT64 qwUserDefinedID)
{
    INT32 nRetCode = 0;
    INT32 nSize = 0;
    M2O_LOG_ITEM* pMsg = NULL;

    pMsg = (M2O_LOG_ITEM*)m_pPacketAllocator->Alloc(sizeof(M2O_LOG_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2o_log_item;

    pMsg->dwTime = CManageServer::GetInstance().GetGameTime();
    pMsg->nGroupID = nGroupID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->qwItemGuid = Guid;
    pMsg->nItemTemplateID = nTemplateID;
    pMsg->nItemCount = nItemCount;
    pMsg->nReason = nReason;
    pMsg->nAction = nAction;
    pMsg->qwUserDefinedID = qwUserDefinedID;

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_LOG_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDLStatItem(INT32 nGroupID, INT32 nItemTemplateID, INT32 nItemCount, INT32 nAction)
{
    INT32       nRetCode = 0;
    M2O_STAT_ITEM   *pMsg = NULL;

    pMsg = (M2O_STAT_ITEM *)m_pPacketAllocator->Alloc(sizeof(M2O_STAT_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2o_stat_item;
    pMsg->nGroupID = nGroupID;
    pMsg->nItemTemplateID = nItemTemplateID;
    pMsg->nItemCount = nItemCount;
    pMsg->nAction = nAction;

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_STAT_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDLCreateTeam(UINT32 dwTime, INT32 nGroupID, UINT32 dwSrcID, UINT32 dwTeamID)
{
	INT32 nRetCode = 0;

	M2O_CREATE_TEAM* pMsg = (M2O_CREATE_TEAM*)m_pPacketAllocator->Alloc(sizeof(M2O_CREATE_TEAM));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_create_team;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->dwTeamID = dwTeamID;
	pMsg->dwRoleID = dwSrcID;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_CREATE_TEAM));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLJoinTeam(UINT32 dwTime, INT32 nGroupID, UINT32 dwSrcID, UINT32 dwTeamID, UINT32 dwLeaderID, INT32 nMemberCount)
{
	INT32 nRetCode = 0;

	M2O_JOIN_TEAM* pMsg = (M2O_JOIN_TEAM*)m_pPacketAllocator->Alloc(sizeof(M2O_JOIN_TEAM));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_join_team;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->dwTeamID = dwTeamID;
	pMsg->dwRoleID = dwSrcID;
	pMsg->dwLeaderID = dwLeaderID;
	pMsg->nMemberCount = nMemberCount;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_JOIN_TEAM));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLLeaveTeam(UINT32 dwTime, INT32 nGroupID, UINT32 dwSrcID, UINT32 dwTeamID, UINT32 dwLeaderID, INT32 nMemberCount)
{
	INT32 nRetCode = 0;

	M2O_LEAVE_TEAM* pMsg = (M2O_LEAVE_TEAM*)m_pPacketAllocator->Alloc(sizeof(M2O_LEAVE_TEAM));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_leave_team;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->dwTeamID = dwTeamID;
	pMsg->dwRoleID = dwSrcID;
	pMsg->dwLeaderID = dwLeaderID;
	pMsg->nMemberCount = nMemberCount;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_LEAVE_TEAM));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLBreakDwonTeam(UINT32 dwTime, INT32 nGroupID, UINT32 dwTeamID)
{
	INT32 nRetCode = 0;

	M2O_BREAKDOWN_TEAM* pMsg = (M2O_BREAKDOWN_TEAM*)m_pPacketAllocator->Alloc(sizeof(M2O_BREAKDOWN_TEAM));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_breakdown_team;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->dwTeamID = dwTeamID;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_BREAKDOWN_TEAM));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLLogin(UINT32 dwTime, INT32 nGroupID, const char* szUserName, UINT32 dwIP)
{
	INT32 nRetCode = 0;

	M2O_LOGIN* pMsg = (M2O_LOGIN*)m_pPacketAllocator->Alloc(sizeof(M2O_LOGIN));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_login;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->dwIP = dwIP;

	strxcpy(pMsg->szUserName, szUserName, USER_NAME_LEN);

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_LOGIN));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLLogout(UINT32 dwTime, INT32 nGroupID, const char* szUserName, UINT32 dwIP)
{
	INT32 nRetCode = 0;

	M2O_LOGOUT* pMsg = (M2O_LOGOUT*)m_pPacketAllocator->Alloc(sizeof(M2O_LOGOUT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_logout;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->dwIP = dwIP;

	strxcpy(pMsg->szUserName, szUserName, USER_NAME_LEN);

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_LOGOUT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLEnterScene(UINT32 dwTime, INT32 nGroupID, INT32 nSceneID, INT32 nSceneTemplateID, UINT32 dwRoleID)
{
	INT32 nRetCode = 0;
	M2O_ENTER_SCENE* pMsg = NULL;

	pMsg = (M2O_ENTER_SCENE*)m_pPacketAllocator->Alloc(sizeof(M2O_ENTER_SCENE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_enter_scene;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->nSceneID = nSceneID;
	pMsg->nTemplateID = nSceneTemplateID;
	pMsg->dwRoleID = dwRoleID;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_ENTER_SCENE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLLeaveScene(UINT32 dwTime, INT32 nGroupID, INT32 nSceneID, INT32 nSceneTemplateID, UINT32 dwRoleID)
{
	INT32 nRetCode = 0;
	M2O_LEAVE_SCENE* pMsg = NULL;

	pMsg = (M2O_LEAVE_SCENE*)m_pPacketAllocator->Alloc(sizeof(M2O_LEAVE_SCENE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_leave_scene;

	pMsg->dwTime = dwTime;
	pMsg->nGroupID = nGroupID;
	pMsg->nSceneID = nSceneID;
	pMsg->nTemplateID = nSceneTemplateID;
	pMsg->dwRoleID = dwRoleID;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_LEAVE_SCENE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


BOOL CMessageHandler::DoDLGmCommand(INT32 nGroup, UINT32 dwRoleID, const char * pcszRoleName, UINT32 dwGameTime, const char* szCommand, BOOL bResult)
{
    INT32 nRetCode = 0;
    M2O_GM_COMMAND* pMsg = NULL;
    INT32 nSize = 0;
    INT32 nCommandSize = 0;

    LOG_PROCESS_ERROR(pcszRoleName);
    LOG_PROCESS_ERROR(szCommand);

    nCommandSize = strlen(szCommand) + 1;
    nSize = nCommandSize + sizeof(M2O_GM_COMMAND);

    pMsg = (M2O_GM_COMMAND*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2o_gm_command;
    pMsg->dwTime = dwGameTime;
    pMsg->nGroupID = nGroup;
    pMsg->dwRoleID = dwRoleID;
    pMsg->bResult = bResult;
    strxcpy(pMsg->szName, pcszRoleName, ROLE_NAME_LEN);
    strxcpy(pMsg->szCommand, szCommand, nCommandSize);

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDLRealMoney2Money02(UINT32 dwGameTime, UINT8 byGroup, INT32 nMoneyCount)
{
	INT32 nRetCode = 0;
	M2O_REALMONEY_MONEY02* pMsg = NULL;

	pMsg = (M2O_REALMONEY_MONEY02*)m_pPacketAllocator->Alloc(sizeof(M2O_REALMONEY_MONEY02));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_realmoney_money02;

	pMsg->dwTime = dwGameTime;
	pMsg->nGroupID = byGroup;
	pMsg->nCount = nMoneyCount;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_REALMONEY_MONEY02));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMessageHandler::DoDLRealMoney2Item(UINT32 dwGameTime, UINT8 byGroup, INT32 nMoneyCount)
{
	INT32 nRetCode = 0;
	M2O_REALMONEY_ITEM* pMsg = NULL;

	pMsg = (M2O_REALMONEY_ITEM*)m_pPacketAllocator->Alloc(sizeof(M2O_REALMONEY_ITEM));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2o_realmoney_money02;

	pMsg->dwTime = dwGameTime;
	pMsg->nGroupID = byGroup;
	pMsg->nCount = nMoneyCount;

	nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_REALMONEY_ITEM));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


BOOL CMessageHandler::DoDLGuildEvent(INT32 nGroupID, UINT32 dwGameTime, INT32 dwGuildID, INT32 nActionType, UINT32 dwValue1, UINT32 dwValue2, UINT32 dwValue3, UINT32 dwValue4, BOOL bGM)
{
    INT32 nRetCode = 0;
    M2O_GUILD_EVENT* pMsg = NULL;

    pMsg = (M2O_GUILD_EVENT*)m_pPacketAllocator->Alloc(sizeof(M2O_GUILD_EVENT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2o_guild_event;

    pMsg->nGroupID = nGroupID;
    pMsg->dwTime = dwGameTime;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nActionType = nActionType;
    pMsg->dwValue1 = dwValue1;
    pMsg->dwValue2 = dwValue2;
    pMsg->dwValue3 = dwValue3;
    pMsg->dwValue4 = dwValue4;
    pMsg->bGM = (UINT8)bGM;

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_GUILD_EVENT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDLGuildChest(INT32 nGroupID, UINT32 dwGameTime, INT32 dwGuildID, INT32 nValue, INT32 nReason)
{
    INT32 nRetCode = 0;
    M2O_GUILD_CHEST* pMsg = NULL;

    pMsg = (M2O_GUILD_CHEST*)m_pPacketAllocator->Alloc(sizeof(M2O_GUILD_CHEST));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2o_guild_chest;

    pMsg->nGroupID = nGroupID;
    pMsg->dwTime = dwGameTime;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nValue = nValue;
    pMsg->nReason = nReason;

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_GUILD_CHEST));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDLGuildStat(UINT32 dwGameTime, GUILD_STAT *pStatList)
{
    INT32       nRetCode = 0;
    M2O_GUILD_STAT  *pMsg = NULL;

    pMsg = (M2O_GUILD_STAT *)m_pPacketAllocator->Alloc(sizeof(M2O_GUILD_STAT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2o_guild_stat;

    memset(pMsg->stat, 0, sizeof(pMsg->stat));
    pMsg->dwTime = dwGameTime;
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pMsg->stat[i].nGuildNum = pStatList[i].nGuildNum;
        if (pMsg->stat[i].nGuildNum  == 0)
            continue;

        for (INT32 j = 0; j < GUILD_STAT_TOP_NUM; ++j)
        {
            pMsg->stat[i].MoneyList[j].dwGuildID = pStatList[i].MoneyList[j].dwGuildID;
            pMsg->stat[i].MoneyList[j].dwValue = pStatList[i].MoneyList[j].dwValue;

            pMsg->stat[i].RiceList[j].dwGuildID = pStatList[i].RiceList[j].dwGuildID;
            pMsg->stat[i].RiceList[j].dwValue = pStatList[i].RiceList[j].dwValue;

            pMsg->stat[i].WoodList[j].dwGuildID = pStatList[i].WoodList[j].dwGuildID;
            pMsg->stat[i].WoodList[j].dwValue = pStatList[i].WoodList[j].dwValue;

            pMsg->stat[i].StoneList[j].dwGuildID = pStatList[i].StoneList[j].dwGuildID;
            pMsg->stat[i].StoneList[j].dwValue = pStatList[i].StoneList[j].dwValue;

            pMsg->stat[i].MetalList[j].dwGuildID = pStatList[i].MetalList[j].dwGuildID;
            pMsg->stat[i].MetalList[j].dwValue = pStatList[i].MetalList[j].dwValue;
        }
        
        for (INT32 j = GUILD_B_MAIN; j < GUILD_B_NUM; ++j)
        {
            for (INT32 k = 0; k < GUILD_MAX_BUILDING_LEVEL + 1; ++k)
            {
                pMsg->stat[i].BuildingStat[j][k] = pStatList[i].BuildingStat[j][k];
            }
        }
    }

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(M2O_GUILD_STAT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDLBillingRequest(
    INT32 nGroupID, INT32 nAction, UINT32 dwRpcID,
    const char* szUserName, const char* szUserName2,
    const char* szUniqueID, const char* szUniqueID2,
    UINT32 dwRoleID, UINT32 dwRoleID2,
    UINT64 qwOrderID, UINT64 qwOrderID2,
    INT32 nAmount, INT32 nParam)
{
    INT32 nRetCode = 0;
    InternalMessage::M2O_BILLING_REQUEST* pMsg = NULL;

    pMsg = (InternalMessage::M2O_BILLING_REQUEST*)m_pPacketAllocator->Alloc(sizeof(InternalMessage::M2O_BILLING_REQUEST));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::m2o_billing_request;

    pMsg->dwTime = CManageServer::GetInstance().GetGameTime();
    pMsg->nGroupID = nGroupID;

    pMsg->nAction = nAction;
    pMsg->dwRpcID = dwRpcID;
    strxcpy(pMsg->szUserName, szUserName, USER_NAME_LEN);
    strxcpy(pMsg->szUserName2, szUserName2, USER_NAME_LEN);
    strxcpy(pMsg->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    strxcpy(pMsg->szUniqueID2, szUniqueID2, OA_BILLING_UNIQUE_ID_LEN);
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwRoleID2 = dwRoleID2;
    pMsg->qwOrderID = qwOrderID;
    pMsg->qwOrderID2 = qwOrderID2;
    pMsg->nAmount = nAmount;
    pMsg->nParam = nParam;

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::M2O_BILLING_REQUEST));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDLBillingResult(INT32 nGroupID, INT32 nAction, UINT32 dwRpcID, const char* szUniqueID, INT32 nResult)
{
    INT32 nRetCode = 0;
    InternalMessage::M2O_BILLING_RESULT* pMsg = NULL;

    pMsg = (InternalMessage::M2O_BILLING_RESULT*)m_pPacketAllocator->Alloc(sizeof(InternalMessage::M2O_BILLING_RESULT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::m2o_billing_result;

    pMsg->dwTime = CManageServer::GetInstance().GetGameTime();
    pMsg->nGroupID = nGroupID;

    pMsg->nAction = nAction;
    pMsg->dwRpcID = dwRpcID;
    strxcpy(pMsg->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    pMsg->nResult = nResult;

    nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::M2O_BILLING_RESULT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
