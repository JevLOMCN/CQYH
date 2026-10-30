#include "stdafx.h"
#include "message_handler.h"

#include "d2m_message.h"
#include "m2d_message.h"
#include "common_message.h"
#include "m2t_message.h"
#include "m2g_message.h"
#include "role_db_def.h"

#include "time_ex.h"
#include "server_config.h"

#include "client/usermgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "role_data_mgr.h"
#include "scene/scenemgr.h"
#include "scene_data.h"
#include "team/team.h"
#include "master/master_mgr.h"
#include "gm/gm_manager.h"
#include "ranklist/ranklist.h"
#include "manageserver.h"
#include "ms_session_mgr.h"
#include "auction/auction_mgr.h"
#include "exchange/exchange_mgr.h"
#include "fcm/fcm.h"

using namespace InternalMessage;

void CMessageHandler::OnDBRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SDBSession* pDBSession{};
    INT32 nRetCode = 0;
    struct in_addr addr;
    D2M_REGISTER* pMsg = (D2M_REGISTER*)pPacketBuffer;

    //check
    SDBSession& rDBSession = CMSSessionMgr::GetInstance().GetDBSession();
    LOG_PROCESS_ERROR( rDBSession.pSocketSession == pSocketSession );
    pDBSession = (SDBSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR( pDBSession );
    LOG_PROCESS_ERROR( pDBSession->pSocketSession == pSocketSession );
    LOG_PROCESS_ERROR(CMSSessionMgr::GetInstance().GetServerState() != sgsClosing);

    if ( m_ServerInfo.DB.dwAddr 
        )// && m_ServerInfo.DB.dwAddr != pMsg->dwAddr )
    {
        char szNewIP[64] = {0};
        addr.s_addr = pMsg->dwAddr;
        strxcpy( szNewIP, inet_ntoa(addr), sizeof(szNewIP) );

        char szOldIP[64] = {0};
        addr.s_addr = m_ServerInfo.DB.dwAddr;
        strxcpy( szOldIP, inet_ntoa(addr), sizeof(szOldIP) );

        CRI("db has been registered, old addr %s, new addr %s", szOldIP, szNewIP );
        LOG_PROCESS_ERROR(FALSE);
    }
    m_ServerInfo.DB.dwAddr = pMsg->dwAddr;
    m_ServerInfo.DB.wPort = pMsg->wPort;

    addr.s_addr = pMsg->dwAddr;
    INF("DB register from ip %s:%d", inet_ntoa(addr), pMsg->wPort );

    nRetCode = DoDBRegisterAck();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoServerInfo();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnDBReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    m_ServerInfo.DB.byReady = true;
    INF("DB is ready");
    _CheckAllReady();
}

void CMessageHandler::OnDBCloseAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INF("DB is closed");
}

void CMessageHandler::OnDBCreateRole(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    D2M_CREATE_ROLE* pMsg = (D2M_CREATE_ROLE*)pPacketBuffer;
    MS_ROLE_DATA* pstRoleData = NULL;
    ROLE_DB_BASE_DATA* poBaseData = NULL;

    poBaseData = (ROLE_DB_BASE_DATA*)(pMsg->BaseData);

    pstRoleData = CRoleDataMgr::GetInstance().CreateRole(poBaseData->dwRoleID, poBaseData->szRoleName);
    LOG_PROCESS_ERROR(pstRoleData);
	
    pstRoleData->byClass = poBaseData->byClass;
    pstRoleData->byLevel = poBaseData->byLevel;
    pstRoleData->bySex = poBaseData->bySex;
    pstRoleData->dwRoleID = poBaseData->dwRoleID;
	pstRoleData->byGroup = pMsg->byServerGroup;
    strxcpy(pstRoleData->szRoleName, poBaseData->szRoleName, ROLE_NAME_LEN);
    strxcpy(pstRoleData->szUserName, pMsg->szUserName, USER_NAME_LEN);

    pstRoleData->pSellingAuction = NULL;
    pstRoleData->pBuyingAuction = NULL;
    pstRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsFree;

    pstRoleData->dwGuildID = 0;
    pstRoleData->dwJoinGuildCD = 0;
    pstRoleData->byApplicationCount = 0;
    
    nRetCode = DoDBCreateRoleAck(pMsg->dwClientSessionID, pMsg->BaseData);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    KICK_USER(pMsg->dwClientSessionID, ktLogged);

    return;
}

void CMessageHandler::OnDBCanDeleteRole(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nResult = errSuccess;
	BOOL bRet = FALSE;
    D2M_CAN_DELETE_ROLE* pMsg = (D2M_CAN_DELETE_ROLE*)pPacketBuffer;
    MS_ROLE_DATA* pstRoleData = NULL;

    // Not found indicates a code bug
    pstRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pstRoleData != NULL);

    //if (nResult == errSuccess)   // redmine 14360
    //{
    //    if (CMasterMgr::GetInstance().IsMaster(pstRoleData->dwRoleID) || CMasterMgr::GetInstance().IsStudent(pstRoleData->dwRoleID))
    //        nResult = errLoginDeleteRoleMaster;
    //}

    if (nResult == errSuccess)
    {
        if (pstRoleData->dwGuildID)
            nResult = errLoginDeleteRoleGuild;
    }

    if (nResult == errSuccess)
    {
        if (!CAuctionMgr::GetInstance().GetBuyingAuctions(pstRoleData->dwRoleID).empty()
            || !CAuctionMgr::GetInstance().GetSellingAuctions(pstRoleData->dwRoleID).empty())
            nResult = errLoginDeleteRoleAuction;
    }

    if (nResult == errSuccess)
    {
        if (!CExchangeMgr::GetInstance().GetOrders(eotBuy, pstRoleData->dwRoleID).empty()
            || !CExchangeMgr::GetInstance().GetOrders(eotSell, pstRoleData->dwRoleID).empty())
            nResult = errLoginDeleteRoleExchange;
    }

	if (nResult == errSuccess)
	{
		bRet = CRankListMgr::GetInstance().DelRankNode(pstRoleData->byGroup, pstRoleData->dwRoleID);
		LOG_CHECK_ERROR(bRet);
	}

	// leave team
	// copy from CMessageHandler::OnClientTeamLeave()
	if (nResult == errSuccess)
	{
		CTeam* poTeam = CTeamMgr::GetInstance().Find(pstRoleData->dwTeamID);
		if (poTeam)
		{
			UINT32 dwOldLeader = poTeam->GetLeader();
			if (dwOldLeader == pMsg->dwRoleID)
			{
				bRet = poTeam->SelectLeader(TRUE);
				LOG_PROCESS_ERROR_RET_CODE(bRet, errTeamNotPermit);
			}

			bRet = poTeam->DelMember(pMsg->dwRoleID);
			LOG_PROCESS_ERROR(bRet);

			//MS_ONLINE_ROLE_DATA* pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
			//LOG_PROCESS_ERROR(pstOnlineRole);
			//bRet = DoClientTeamLeave(pstOnlineRole->dwCliSessionID, poTeam->GetTeamID());
			//LOG_CHECK_ERROR(bRet);

			bRet = DoDLLeaveTeam(CManageServer::GetInstance().GetGameTime(), pstRoleData->byGroup,
				pMsg->dwRoleID, poTeam->GetTeamID(), poTeam->GetLeader(), poTeam->GetMemberCount());
			LOG_CHECK_ERROR(bRet);

			if (poTeam->GetMemberCount() == 0)
			{
				bRet = DoDLBreakDwonTeam(CManageServer::GetInstance().GetGameTime(),
					pstRoleData->byGroup, poTeam->GetTeamID());
				LOG_CHECK_ERROR(bRet);

				if (poTeam->GetActivityType() != etaatInvalid)
				{
					bRet = CTeamMgr::GetInstance().DeleteActivityTeam(poTeam->GetActivityType(), poTeam->GetActivityID(), poTeam->GetTeamID());
					LOG_PROCESS_ERROR(bRet);
				}

				bRet = poTeam->UnInit();
				LOG_CHECK_ERROR(bRet);

				bRet = CTeamMgr::GetInstance().Delete(poTeam);
				LOG_PROCESS_ERROR(bRet);
			}
			else
			{
				bRet = DoClientRemoveMember(pMsg->dwRoleID, poTeam);
				LOG_CHECK_ERROR(bRet);

				if (dwOldLeader != poTeam->GetLeader())
				{
					bRet = DoClientTeamChangeInfo(poTeam);
					LOG_CHECK_ERROR(bRet);

					bRet = DoGSTeamChangeInfo(poTeam);
					LOG_PROCESS_ERROR(bRet);
				}
			}

			bRet = DoGSTeamRemoveMember(pMsg->dwRoleID, poTeam);
			LOG_PROCESS_ERROR(bRet);

			pstRoleData->dwTeamID = 0;
		}
	}

	bRet = DoDBCanDeleteRoleAck(pMsg->dwClientSessionID, pMsg->dwRoleID, nResult);
	LOG_PROCESS_ERROR(bRet);

    return;
Exit0:
    KICK_USER(pMsg->dwClientSessionID, ktLogged);

    return;
}

void CMessageHandler::OnDBDeleteRole(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    INT32   nResult = errUnknownError;
	INT32 nServerGroupID = 0;

    D2M_DELETE_ROLE* pMsg = (D2M_DELETE_ROLE*)pPacketBuffer;
    MS_ROLE_DATA* pstRoleData = NULL;

    // Not found indicates a code bug
    pstRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pstRoleData != NULL);

    nRetCode = CRoleDataMgr::GetInstance().DestroyRole(pstRoleData);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoDBDeleteRoleAck(pMsg->dwClientSessionID, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    KICK_USER(pMsg->dwClientSessionID, ktLogged);

    return;
}

void CMessageHandler::OnDBLeaveDBAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    D2M_LEAVE_DB_ACK* pMsg = (D2M_LEAVE_DB_ACK*)pPacketBuffer;
    MS_USER* poUser = NULL;
    MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;

    // The user must exist
    poUser = CUserMgr::GetInstance().FindUserBySessionID(pMsg->dwClientSessionID);
    LOG_PROCESS_ERROR(poUser);

    switch (poUser->eState)
    {
    case USER_STATE_EXIT_DB:
        {
            nRetCode = CGMManager::GetInstance().IsUserLocked(poUser->szUserName);
            if (nRetCode)
            {   
                // todo_tq  Allan Tao[26.12.2013] 
                nRetCode = DoDBClearCache(poUser->dwCurRoleID, 0, 0);
                LOG_PROCESS_ERROR(nRetCode);
            }

            nRetCode = CUserMgr::GetInstance().DelUser(poUser);
            LOG_PROCESS_ERROR(nRetCode);

            if (g_ServerConfig.MS.bFcmEnable)
                CFcmMgr::GetInstance().SetOffline(poUser->szUserName, poUser->dwClientIp);
        }
        break;
    case USER_STATE_EXIT_DB_RETURN:
        {            
            nRetCode = DoGTReturnSelectRole(poUser->dwCliSessionID);
            LOG_PROCESS_ERROR(nRetCode);

            poUser->eState = USER_STATE_QUEUEFINISHED;
        }
        break;
    default:
        break;
    }
	
	MS_SYS_LOG_SUCCESS(poUser);
    return;
Exit0:
	MS_SYS_LOG_ERROR(poUser);

    KICK_USER(pMsg->dwClientSessionID, ktLogged);

    return;
}

void CMessageHandler::OnDBClearRoleCacheAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    D2M_CLEAR_ROLE_CACHE_ACK    *pMsg = (D2M_CLEAR_ROLE_CACHE_ACK*)pPacketBuffer;

    if (pMsg->dwTLUserID == 0)
        return;

    if (pMsg->byResult == TRUE)
    {
        INF("DB clear role cache success");
        nRetCode = DoGMClearRoleCacheAck(errSuccess, pMsg->dwTLUserID, pMsg->dwCmdSerialID);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = DoGMClearRoleCacheAck(errUnknownError, pMsg->dwTLUserID, pMsg->dwCmdSerialID);
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

BOOL CMessageHandler::DoDBRegisterAck(void)
{
    INT32 nRetCode = 0;
    const DBSetting* dbsetting = CManageServer::GetInstance().GetDBSetting();

    M2D_REGISTER_ACK* pMsg = (M2D_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2D_REGISTER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2d_register_ack;
    strxcpy(pMsg->szServerAddr, dbsetting->szServerAddr, 64);
    strxcpy(pMsg->szLoginName, dbsetting->szLoginName, 64);
    strxcpy(pMsg->szPassword, dbsetting->szPassword, 64);
    strxcpy(pMsg->szDBName, dbsetting->szDBName, 64);
    strxcpy(pMsg->szCharSet, dbsetting->szCharSet, 64);
    pMsg->dwServerPort = dbsetting->dwServerPort;

    nRetCode = _SendDB((PACKET_BUFFER*)pMsg, sizeof(M2D_REGISTER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDBCreateRoleAck(INT32 dwClientSessionID, CHAR szBaseData[])
{
    INT32 nRetCode = 0;

    M2D_CREATE_ROLE_ACK* pMsg = (M2D_CREATE_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2D_CREATE_ROLE_ACK) + sizeof(ROLE_DB_BASE_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2d_create_role_ack;
    pMsg->dwClientSessionID = dwClientSessionID;
    memcpy(pMsg->BaseData, szBaseData, sizeof(ROLE_DB_BASE_DATA));

    nRetCode = _SendDB((PACKET_BUFFER*)pMsg, sizeof(M2D_CREATE_ROLE_ACK) + sizeof(ROLE_DB_BASE_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDBCanDeleteRoleAck(INT32 dwClientSessionID, UINT32 dwRoleID, INT32 nResult)
{
    INT32 nRetCode = 0;

    M2D_CAN_DELETE_ROLE_ACK* pMsg = (M2D_CAN_DELETE_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2D_CAN_DELETE_ROLE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2d_can_delete_role_ack;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nResult = nResult;

    nRetCode = _SendDB((PACKET_BUFFER*)pMsg, sizeof(M2D_CAN_DELETE_ROLE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDBDeleteRoleAck(INT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;

    M2D_DELETE_ROLE_ACK* pMsg = (M2D_DELETE_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2D_DELETE_ROLE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2d_delete_role_ack;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendDB((PACKET_BUFFER*)pMsg, sizeof(M2D_DELETE_ROLE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDBLeaveDB(INT32 dwClientSessionID)
{
    INT32 nRetCode = 0;

    M2D_LEAVE_DB* pMsg = (M2D_LEAVE_DB*)m_pPacketAllocator->Alloc(sizeof(M2D_LEAVE_DB));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2d_leave_db;
    pMsg->dwClientSessionID = dwClientSessionID;

    nRetCode = _SendDB((PACKET_BUFFER*)pMsg, sizeof(M2D_LEAVE_DB));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDBClearCache(UINT32 dwRoleID, UINT32 dwTLUserID, UINT32 dwCmdSerialID)
{
    INT32       nRetCode = 0;
    M2D_CLEAR_ROLE_CACHE    *pMsg = (M2D_CLEAR_ROLE_CACHE*)m_pPacketAllocator->Alloc(sizeof(M2D_CLEAR_ROLE_CACHE));
    LOG_PROCESS_ERROR(pMsg);
    
    pMsg->wMsg = m2d_clear_role_cache;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;

    nRetCode = _SendDB((PACKET_BUFFER*)pMsg, sizeof(M2D_CLEAR_ROLE_CACHE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoDBGMCommand(const char* pcszGMCommand)
{
	M2D_GM_COMMAND* pMsg{};
	INT32 nRetCode = 0;
	INT32 nCommandLen = 0;

	LOG_PROCESS_ERROR(pcszGMCommand);
		
	nCommandLen = (INT32)strlen(pcszGMCommand) + 1;

	pMsg = (M2D_GM_COMMAND*)m_pPacketAllocator->Alloc(sizeof(M2D_GM_COMMAND) + nCommandLen);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2d_gm_command;
	strxcpy(pMsg->szGMCommand, pcszGMCommand, nCommandLen);

	nRetCode = _SendDB((PACKET_BUFFER*)pMsg, sizeof(M2D_CLEAR_ROLE_CACHE) + nCommandLen);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}
