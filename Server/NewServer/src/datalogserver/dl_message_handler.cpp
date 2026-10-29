#include "stdafx.h"
#include "dl_message_handler.h"

#include "time_ex.h"
#include "dump.h"

#include "server_config.h"
#include "role_db_def.h"

#include "o2m_message.h"
#include "m2o_message.h"
#include "o2g_message.h"
#include "g2o_message.h"
#include "dl_session_mgr.h"
#include "common_message.h"

#include "gameworld/game_world.h"
#include "quest_data.h"
#include "gameworld/role.h"
#include "gameworld/npc.h"
#include "data_manager.h"
#include "achievement_data.h"

#include "dl_def.h"

#include "mysql.h"
#include "mysqld_error.h"
#include "db/dl_sql_stmt_list.h"
#include "datalogserver.h"

using namespace InternalMessage;

CDLMessageHandler CDLMessageHandler::ms_Instance;

void CDLMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
	LOG_PROCESS_ERROR(pSocketSession);
    LOG_PROCESS_ERROR(CDLSessionMgr::GetInstance().GetServerState() != sgsClosing);

	INF("accepted new gs session");
	
	return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();
	return;
}

void CDLMessageHandler::OnConnected(ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
    UINT32 dwIP = 0;
    BASE_SESSION* pBaseSession = NULL;

	LOG_PROCESS_ERROR(pSocketSession);

	pBaseSession = (BASE_SESSION*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pBaseSession);

    pBaseSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

	switch(pBaseSession->byType)
	{
	case stMS:
		{
			MS_SESSION* pMSSession = (MS_SESSION*)pBaseSession;
			pMSSession->bConnected = true;

			dwIP = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
			LOG_PROCESS_ERROR(dwIP);

			nRetCode = DoMSRegister(dwIP, g_ServerConfig.DL.dwPort);
			LOG_PROCESS_ERROR(nRetCode);
		}
		break;;
	case stGD:
		{
			GD_SESSION* pGDSession = (GD_SESSION*)pBaseSession;
			pGDSession->bConnected = true;
            pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nRegisterGuardTimeout;

			if(CDLSessionMgr::GetInstance().GetServerState() == sgsSelfReady || 
				CDLSessionMgr::GetInstance().GetServerState() == sgsAllReady)
			{
				nRetCode = DoGDRegister();
				LOG_PROCESS_ERROR(nRetCode);
			}
		}
		break;
	case stDB:
	case stGS:
	case stGT:
	case stLS:
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return;
Exit0:
	if (pSocketSession)
		pSocketSession->Close();

	return;
}

void CDLMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
    MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pPacketBuffer;

	LOG_PROCESS_ERROR(pSocketSession);

	LOG_PROCESS_ERROR(pHeader);
	LOG_PROCESS_ERROR(m_MessageFuncList[pHeader->wMsg]);
#if defined(WIN32) | defined (WIN64)
	__try
	{
		(this->*m_MessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize, pSocketSession);
	}
	__except (DumpExceptionFilter(GetExceptionInformation()))
	{
		CRI("failed to process internal message id %d size %d", pHeader->wMsg, dwSize);
	}
#else
	(this->*m_MessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize, pSocketSession);
#endif	// WIN32 | WIN64

	if (pPacketBuffer)
		m_pPacketAllocator->Free(pPacketBuffer);

	return;
Exit0:
    if (pHeader)
    	CRI("unknown internal message id %d size %d", pHeader->wMsg, dwSize);
    else
        CRI("unknown internal message with null pointer");

	if (pPacketBuffer)
		m_pPacketAllocator->Free(pPacketBuffer);

	return;
}

void CDLMessageHandler::OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown)
{
    INT32 nRetCode = 0;
    BASE_SESSION* pBaseSession = NULL;

	LOG_PROCESS_ERROR(pSocketSession);

	pBaseSession = (BASE_SESSION*)pSocketSession->GetUserData();
	PROCESS_ERROR(pBaseSession);

	pBaseSession->pSocketSession = NULL;

	switch (pBaseSession->byType)
	{
	case stMS:
		{
			MS_SESSION* pMSSession = (MS_SESSION*)pBaseSession;

			pMSSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
			pMSSession->bConnected = false;
			
			CRI("MS disconnected");
		}
		break;
	case  stGD:
		{
			GD_SESSION* pGDSession = (GD_SESSION*)pBaseSession;
			
			pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
			pGDSession->bConnected = false;
			
			CRI("GD disconnected");
		}
		break;
	case stGS:
		{
			GS_SESSION* pGSSession = (GS_SESSION*)pBaseSession;
			pGSSession->pSocketSession = NULL;
			pGSSession->qwTimer = 0xFFFFFFFF;

			CRI("GS disconnected");
		}
		break;
	case stGT:
	case stDB:
	case stLS:
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return;
Exit0:
	return;
}

CDLMessageHandler::CDLMessageHandler()
{
    m_pNet = NULL;
    m_pPacketAllocator = NULL;
    m_pGSSocketAcceptor = NULL;
}

CDLMessageHandler::~CDLMessageHandler()
{
    UnInit();
}

BOOL CDLMessageHandler::Init(INet* pNet, IPacketAllocator* pPacketAllocator, ISocketAcceptor* pGSSocketAcceptor)
{
	INT32 nRetCode = 0;
    UINT32 dwCurGameTime = (UINT32)time(NULL);

	LOG_PROCESS_ERROR(pNet);

	m_pNet = pNet;
	m_pPacketAllocator = pPacketAllocator;
	m_pGSSocketAcceptor = pGSSocketAcceptor;

	nRetCode = m_LogData.Init();
	LOG_PROCESS_ERROR(nRetCode);

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; i++)
    {
        m_dwNextSaveMonitorDataTime[i] = dwCurGameTime / 3600 * 3600 + 3600 / MAX_SERVER_GROUP_COUNT * i;
        if (m_dwNextSaveMonitorDataTime[i] < dwCurGameTime)
            m_dwNextSaveMonitorDataTime[i] += 3600;
    }

	memset(m_llNpcTotalDieCount, 0, sizeof(m_llNpcTotalDieCount));
    memset(m_llNpcTotalMoneyCount, 0, sizeof(m_llNpcTotalMoneyCount));
	memset(m_llMoneyProduce, 0, sizeof(m_llMoneyProduce));
	memset(m_llMoneyDestory, 0, sizeof(m_llMoneyDestory));
	memset(m_llQuestCompleteCount, 0, sizeof(m_llQuestCompleteCount));
	memset(m_llRealMoneyToMoney02, 0, sizeof(m_llRealMoneyToMoney02));
	memset(m_llRealMoneyToItem, 0, sizeof(m_llRealMoneyToItem));

	for (INT32 nServerGroup = 0; nServerGroup < MAX_SERVER_GROUP_COUNT; nServerGroup++)
	{
		m_MapNpcIDDieStatistic[nServerGroup].clear();
        m_MapNpcIDMoneyStatistic[nServerGroup].clear();
	}

	// From All
	m_MessageFuncList[a2a_ping]   =  &CDLMessageHandler::OnAllPing;
	m_MessageFuncList[m2a_all_ready] = &CDLMessageHandler::OnMSAllReady;

	// From MS
	m_MessageFuncList[m2o_register_ack] = &CDLMessageHandler::OnMSRegisterAck;
	m_MessageFuncList[m2a_server_info] = &CDLMessageHandler::OnMSServerInfo;
    m_MessageFuncList[m2a_close] = &CDLMessageHandler::OnMSClose;
	m_MessageFuncList[m2o_chat_message] = &CDLMessageHandler::OnMSChatMessage;

    m_MessageFuncList[m2o_log_item] = &CDLMessageHandler::OnMSLogItem;
    m_MessageFuncList[m2o_stat_item] = &CDLMessageHandler::OnMSStatItem;

	m_MessageFuncList[m2o_login] = &CDLMessageHandler::OnMSLogin;
	m_MessageFuncList[m2o_logout] = &CDLMessageHandler::OnMSLogout;	
 
	m_MessageFuncList[m2o_create_team] = &CDLMessageHandler::OnMSCreateTeam;	
	m_MessageFuncList[m2o_join_team] = &CDLMessageHandler::OnMSJoinTeam;
	m_MessageFuncList[m2o_leave_team] = &CDLMessageHandler::OnMSLeaveTeam;
	m_MessageFuncList[m2o_breakdown_team] = &CDLMessageHandler::OnMSBreakDownTeam;

	m_MessageFuncList[m2o_enter_scene] = &CDLMessageHandler::OnMSEnterScene;
	m_MessageFuncList[m2o_leave_scene] = &CDLMessageHandler::OnMSLeaveScene;

    m_MessageFuncList[m2o_gm_command] = &CDLMessageHandler::OnMSGMCommand;

    m_MessageFuncList[m2o_realmoney_money02] = &CDLMessageHandler::OnMSRealMoneyToMoney02;
    m_MessageFuncList[m2o_realmoney_item] = &CDLMessageHandler::OnMSRealMoneyToItem;

    m_MessageFuncList[m2o_guild_event] = &CDLMessageHandler::OnMSGuildEvent;
    m_MessageFuncList[m2o_guild_chest] = &CDLMessageHandler::OnMSGuildChest;
    m_MessageFuncList[m2o_guild_stat] = &CDLMessageHandler::OnMSGuildStat;

    m_MessageFuncList[m2o_billing_request] = &CDLMessageHandler::OnMSBillingRequest;
    m_MessageFuncList[m2o_billing_result] = &CDLMessageHandler::OnMSBillingResult;

	// From GS
	m_MessageFuncList[g2o_register] = &CDLMessageHandler::OnGSRegister;

 	m_MessageFuncList[g2o_chat_message] = &CDLMessageHandler::OnGSChatMessage;
 
 	m_MessageFuncList[g2o_accept_quest] = &CDLMessageHandler::OnGSAcceptQuest;
 	m_MessageFuncList[g2o_complete_quest] = &CDLMessageHandler::OnGSCompleteQuest;
 	m_MessageFuncList[g2o_abandon_quest] = &CDLMessageHandler::OnGSAbandonQuest;
	m_MessageFuncList[g2o_complete_achievement] = &CDLMessageHandler::OnGSCompleteAchievement;
 
 	m_MessageFuncList[g2o_level_up] = &CDLMessageHandler::OnGSLevelUp;
 	
	m_MessageFuncList[g2o_die] = &CDLMessageHandler::OnGSDie;

	m_MessageFuncList[g2o_create_scene] = &CDLMessageHandler::OnGSCreateScene;
	m_MessageFuncList[g2o_destory_scene] = &CDLMessageHandler::OnGSDestoryScene;

    m_MessageFuncList[g2o_log_item] = &CDLMessageHandler::OnGSLogItem;
    m_MessageFuncList[g2o_stat_item] = &CDLMessageHandler::OnGSStatItem;
    m_MessageFuncList[g2o_rune_item] = &CDLMessageHandler::OnGSRuneItem;
    m_MessageFuncList[g2o_change_item] = &CDLMessageHandler::OnGSChangeItem;

	m_MessageFuncList[g2o_buy_conslot] = &CDLMessageHandler::OnGSBuyConSlot;

	m_MessageFuncList[g2o_role_logout] = &CDLMessageHandler::OnGSRoleLogout;

	m_MessageFuncList[g2o_gm_command] = &CDLMessageHandler::OnGSGMCommand;

	m_MessageFuncList[g2o_add_money] = &CDLMessageHandler::OnGSAddMoney;
	m_MessageFuncList[g2o_sub_money] = &CDLMessageHandler::OnGSSubMoney;

	m_MessageFuncList[g2o_role_position] = &CDLMessageHandler::OnGSRolePosition;

	m_MessageFuncList[g2o_npc_die] = &CDLMessageHandler::OnGSNpcDie;
    m_MessageFuncList[g2o_npc_money] = &CDLMessageHandler::OnGSNpcMoney;
	m_MessageFuncList[g2o_consecutive_kill_npc_count] = &CDLMessageHandler::OnGSConsecutiveKillNpcCount;
	m_MessageFuncList[g2o_momentary_kill_npc_count] = &CDLMessageHandler::OnGSMomentaryKillNpcCount;

	m_MessageFuncList[g2o_client_hardware_info] = &CDLMessageHandler::OnGSClientHardwareInfo;
	m_MessageFuncList[g2o_client_fps_info] = &CDLMessageHandler::OnGSClientFpsInfo;

	m_MessageFuncList[g2o_spell_effect] = &CDLMessageHandler::OnGSSpellEffect;
	m_MessageFuncList[g2o_cast_spell] = &CDLMessageHandler::OnGSCastSpell;
	m_MessageFuncList[g2o_learn_spell] = &CDLMessageHandler::OnGSLearnSpell;
	m_MessageFuncList[g2o_role_anti_cheat_info] = &CDLMessageHandler::OnGSAntiCheat;
	
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CDLMessageHandler::UnInit(void)
{
	m_LogData.UnInit();

	return TRUE;
}

CDLMessageHandler& CDLMessageHandler::GetInstance(void)
{
	return ms_Instance;
}

BOOL CDLMessageHandler::MainLoop()
{
	INT32 nRetCode = 0;
	
	for (INT32 nServerGroup = 0; nServerGroup < MAX_SERVER_GROUP_COUNT; nServerGroup++)
	{
        if (IsValidGroup(nServerGroup))
		{
			if (CGameWorld::GetInstance().GetGameTime() >= m_dwNextSaveMonitorDataTime[nServerGroup])
			{
				nRetCode = _SaveStatData(CGameWorld::GetInstance().GetGameTime(), nServerGroup);
				LOG_CHECK_ERROR(nRetCode);

				m_dwNextSaveMonitorDataTime[nServerGroup] += 3600;
			}
		}
	}

	return TRUE;
}

BOOL CDLMessageHandler::_LoadPosInfo(const char* pcData, CRole* pRole)
{
	INT32 nRetCode = 0;

	UINT32 dwBlockSize = 0;
	UINT32 dwRealDataSize = 0;

	ROLE_DB_DATA* pRoleData = NULL;
	ROLE_DB_DATA_BLOCK* pBlock = NULL;
	ROLE_DB_BASE_DATA* pBaseData = NULL;
	const char* pcBlockData = NULL;

	LOG_PROCESS_ERROR(pcData);

	pRoleData = (ROLE_DB_DATA*)pcData;

	pBlock = &pRoleData->BlockTable[rbtBase];
	LOG_PROCESS_ERROR(pBlock);

	pcBlockData = &pRoleData->RoleData[pBlock->dwStartOffset];
	LOG_PROCESS_ERROR(pcBlockData);

	pBaseData = (ROLE_DB_BASE_DATA*)pcBlockData;
	LOG_PROCESS_ERROR(pBaseData);

	pRole->SetSceneTempID(pBaseData->nSceneTempID);
	pRole->SetSceneID(pBaseData->nSceneID);
	pRole->SetX(pBaseData->nX);
	pRole->SetY(pBaseData->nY);
	pRole->SetZ(pBaseData->nZ);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CDLMessageHandler::_SaveStatData(UINT32 dwTime, INT32 nGroupID)
{
	INT32 nRetCode = 0;
	INT32 nAreaID = g_ServerConfig.Common.nAreaID;
    const char* szTime = get_time_str(dwTime);

	LOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_SERVER_GROUP_COUNT);

    nRetCode = _SaveStatConsecutiveKill(szTime, nAreaID, nGroupID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _SaveStatMomentaryKill(szTime, nAreaID, nGroupID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _SaveStatNpcDie(szTime, nAreaID, nGroupID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _SaveStatNpcMoney(szTime, nAreaID, nGroupID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _SaveStatQuestComplete(szTime, nAreaID, nGroupID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _SaveStatMoney(szTime, nAreaID, nGroupID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _SaveStatRealMoney(szTime, nAreaID, nGroupID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _SaveStatItem(szTime, nAreaID, nGroupID);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CDLMessageHandler::_SaveStatConsecutiveKill(const char* szTime, INT32 nAreaID, INT32 nGroupID)
{
    INT32 nRetCode = 0;
    char* pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\r\n",
        szTime, nAreaID, nGroupID,
        m_llConsecutiveKill[nGroupID][1], m_llConsecutiveKill[nGroupID][2], m_llConsecutiveKill[nGroupID][3],
        m_llConsecutiveKill[nGroupID][4], m_llConsecutiveKill[nGroupID][5], m_llConsecutiveKill[nGroupID][6],
        m_llConsecutiveKill[nGroupID][7], m_llConsecutiveKill[nGroupID][8], m_llConsecutiveKill[nGroupID][9],
        m_llConsecutiveKill[nGroupID][10], m_llConsecutiveKill[nGroupID][11], m_llConsecutiveKill[nGroupID][12],
        m_llConsecutiveKill[nGroupID][13], m_llConsecutiveKill[nGroupID][14], m_llConsecutiveKill[nGroupID][15],
        m_llConsecutiveKill[nGroupID][16], m_llConsecutiveKill[nGroupID][17], m_llConsecutiveKill[nGroupID][18],
        m_llConsecutiveKill[nGroupID][19]);

    nRetCode = m_LogData.AsyncWriteLog(dlmtStatConsecutiveKillNpc, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    memset(&m_llConsecutiveKill[nGroupID], 0, sizeof(m_llConsecutiveKill[nGroupID]));

    return TRUE;

Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

BOOL CDLMessageHandler::_SaveStatMomentaryKill(const char* szTime, INT32 nAreaID, INT32 nGroupID)
{
    INT32 nRetCode = 0;
    char* pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\r\n",
        szTime, nAreaID, nGroupID, m_llMomentaryKill[nGroupID][0], m_llMomentaryKill[nGroupID][1],
        m_llMomentaryKill[nGroupID][2], m_llMomentaryKill[nGroupID][3], m_llMomentaryKill[nGroupID][4],
        m_llMomentaryKill[nGroupID][5], m_llMomentaryKill[nGroupID][6], m_llMomentaryKill[nGroupID][7],
        m_llMomentaryKill[nGroupID][8], m_llMomentaryKill[nGroupID][9], m_llMomentaryKill[nGroupID][10],
        m_llMomentaryKill[nGroupID][11], m_llMomentaryKill[nGroupID][12], m_llMomentaryKill[nGroupID][13]);

    nRetCode = m_LogData.AsyncWriteLog(dlmtStatMomentaryKillNpc, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    memset(&m_llMomentaryKill[nGroupID], 0, sizeof(m_llMomentaryKill[nGroupID]));

    return TRUE;

Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

BOOL CDLMessageHandler::_SaveStatNpcDie(const char* szTime, INT32 nAreaID, INT32 nGroupID)
{
    INT32 nRetCode = 0;
    char* pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%I64d\r\n",
        szTime, nAreaID, nGroupID, 0, m_llNpcTotalDieCount[nGroupID]);

    nRetCode = m_LogData.AsyncWriteLog(dlmtStatNpcDie, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    m_llNpcTotalDieCount[nGroupID] = 0;

    for (MapIter Iter = m_MapNpcIDDieStatistic[nGroupID].begin();
        Iter != m_MapNpcIDDieStatistic[nGroupID].end(); ++Iter)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);

        snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%I64d\r\n",
            szTime, nAreaID, nGroupID, Iter->first, Iter->second);

        nRetCode = m_LogData.AsyncWriteLog(dlmtStatNpcDie, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }

    m_MapNpcIDDieStatistic[nGroupID].clear();

    return TRUE;

Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

BOOL CDLMessageHandler::_SaveStatNpcMoney(const char* szTime, INT32 nAreaID, INT32 nGroupID)
{
    INT32 nRetCode = 0;
    char* pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%I64d\r\n",
        szTime, nAreaID, nGroupID, 0, m_llNpcTotalMoneyCount[nGroupID]);

    nRetCode = m_LogData.AsyncWriteLog(dlmtStatNpcMoney, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    m_llNpcTotalMoneyCount[nGroupID] = 0;

    for (MapIter Iter = m_MapNpcIDMoneyStatistic[nGroupID].begin();
        Iter != m_MapNpcIDMoneyStatistic[nGroupID].end(); ++Iter)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);

        snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%I64d\r\n",
            szTime, nAreaID, nGroupID, Iter->first, Iter->second);

        nRetCode = m_LogData.AsyncWriteLog(dlmtStatNpcMoney, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }

    m_MapNpcIDMoneyStatistic[nGroupID].clear();

    return TRUE;

Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

BOOL CDLMessageHandler::_SaveStatQuestComplete(const char* szTime, INT32 nAreaID, INT32 nGroupID)
{
    INT32 nRetCode = 0;
    char* pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\r\n",
        szTime, nAreaID, nGroupID, m_llQuestCompleteCount[nGroupID]);

    nRetCode = m_LogData.AsyncWriteLog(dlmtStatQuestComplete, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    m_llQuestCompleteCount[nGroupID] = 0;

    return TRUE;

Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

BOOL CDLMessageHandler::_SaveStatMoney(const char* szTime, INT32 nAreaID, INT32 nGroupID)
{
    INT32 nRetCode = 0;
    char* pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%I64d\t%I64d\t%I64d\t%I64d\r\n",
        szTime, nAreaID, nGroupID,
        m_llMoneyProduce[nGroupID][0], m_llMoneyProduce[nGroupID][1], m_llMoneyDestory[nGroupID][0], m_llMoneyDestory[nGroupID][1]);

    nRetCode = m_LogData.AsyncWriteLog(dlmtStatMoney, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    m_llMoneyProduce[nGroupID][0] = 0;
    m_llMoneyProduce[nGroupID][1] = 0;
    m_llMoneyDestory[nGroupID][0] = 0;
    m_llMoneyDestory[nGroupID][1] = 0;

    return TRUE;

Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

BOOL CDLMessageHandler::_SaveStatRealMoney(const char* szTime, INT32 nAreaID, INT32 nGroupID)
{
    INT32 nRetCode = 0;
    char* pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%I64d\t%I64d\r\n",
        szTime, nAreaID, nGroupID, m_llRealMoneyToMoney02[nGroupID], m_llRealMoneyToItem[nGroupID]);

    nRetCode = m_LogData.AsyncWriteLog(dlmtStatRealMoney, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    m_llRealMoneyToMoney02[nGroupID] = 0;
    m_llRealMoneyToItem[nGroupID] = 0;

    return TRUE;

Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

BOOL CDLMessageHandler::_SaveStatItem(const char* szTime, INT32 nAreaID, INT32 nGroupID)
{
    INT32 nRetCode = 0;
    char* pLogContent = NULL;

    for (std::map<INT32, ItemStat>::const_iterator it = m_MapItemStat[nGroupID].begin(); it != m_MapItemStat[nGroupID].end(); ++it)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);

        snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%I64d\t%I64d\r\n",
            szTime, nAreaID, nGroupID, it->first, it->second.llProduce, it->second.llDestroy);

        nRetCode = m_LogData.AsyncWriteLog(dlmtStatItem, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }
    m_MapItemStat[nGroupID].clear();

    return TRUE;

Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

inline BOOL CDLMessageHandler::_SendGD(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;

	GD_SESSION& rGDSession = CDLSessionMgr::GetInstance().GetGDSession();
	LOG_PROCESS_ERROR(rGDSession.bConnected);
	LOG_PROCESS_ERROR(rGDSession.pSocketSession);

	nRetCode = rGDSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL CDLMessageHandler::_SendMS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;

	MS_SESSION& rMSSession = CDLSessionMgr::GetInstance().GetMSSession();
	LOG_PROCESS_ERROR(rMSSession.bConnected);
	LOG_PROCESS_ERROR(rMSSession.pSocketSession);

	nRetCode = rMSSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CDLMessageHandler::_SendGS(UINT32 dwGSSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;

	GS_SESSION* pGSSession = CDLSessionMgr::GetInstance().GetGSSession(dwGSSessionID);
	LOG_PROCESS_ERROR(pGSSession);
	LOG_PROCESS_ERROR(pGSSession->pSocketSession);

	nRetCode = pGSSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CDLMessageHandler::DoAllPing(void)
{
	INT32 nRetCode = 0;
	A2A_PING* pMsg = NULL;
	CDLSessionMgr &rSessionMgr = CDLSessionMgr::GetInstance(); 

	if (rSessionMgr.GetGDSession().bConnected)
	{
		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;
		
		nRetCode = _SendGD((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
		LOG_CHECK_ERROR(nRetCode);
		if (!nRetCode)
		{ 
			m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
			pMsg = NULL;
		}
	}

	if(rSessionMgr.GetMSSession().bConnected)
	{
		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;
		
		nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
		LOG_CHECK_ERROR(nRetCode);
		if (!nRetCode)
		{
			m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
			pMsg = NULL;
		}
	}

	for(INT32 i = 1; i < MAX_GS_SESSION_COUNT; ++i)
	{
		GS_SESSION* pGSSession = CDLSessionMgr::GetInstance().GetGSSession(i);
		if (pGSSession == NULL || pGSSession->pSocketSession == NULL)
			continue;

		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;

		nRetCode = _SendGS(pGSSession->dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
		LOG_CHECK_ERROR(nRetCode);
		if (!nRetCode)
		{
			m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
			pMsg = NULL;
		}
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

/************************************************************************/
// To MS

BOOL CDLMessageHandler::DoMSRegister(UINT32 dwAddr, UINT16 wPort)
{
	INT32 nRetCode = 0;

	O2M_REGISTER* pMsg = (O2M_REGISTER*)m_pPacketAllocator->Alloc(sizeof(O2M_REGISTER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = o2m_register;
	pMsg->dwAddr = dwAddr;
	pMsg->wPort = wPort;

	nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(O2M_REGISTER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CDLMessageHandler::DoMSReady(void)
{
	INT32 nRetCode = 0;

	O2M_READY* pMsg = (O2M_READY*)m_pPacketAllocator->Alloc(sizeof(O2M_READY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = o2m_ready;

	nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(O2M_READY));
	LOG_PROCESS_ERROR(nRetCode);

	CDLSessionMgr::GetInstance().SetServerState(sgsSelfReady);
	INF("DL is self ready");

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CDLMessageHandler::DoMSCloseAck(void)
{
	INT32 nRetCode = 0;

	O2M_CLOSE_ACK* pMsg = (O2M_CLOSE_ACK*)m_pPacketAllocator->Alloc(sizeof(O2M_CLOSE_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = o2m_close_ack;

	nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(O2M_CLOSE_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CDLMessageHandler::DoGSRegisterAck(UINT32 dwGSSessionID)
{
	INT32 nRetCode = 0;

	O2G_REGISTER_ACK* pMsg = (O2G_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(O2G_REGISTER_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = o2g_register_ack;

	nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(O2G_REGISTER_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

/************************************************************************/
// To GD

BOOL CDLMessageHandler::DoGDRegister(void)
{
	INT32 nRetCode = 0;

	A2U_REGRISTER* pMsg = (A2U_REGRISTER*)m_pPacketAllocator->Alloc(sizeof(A2U_REGRISTER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = a2u_regrister;
	pMsg->byType = stDL;
	pMsg->bySessionID = 1;
	pMsg->dwProcessID = GetCurrentProcessId();

	nRetCode = _SendGD((PACKET_BUFFER*)pMsg, sizeof(A2U_REGRISTER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

/************************************************************************/
// From ALL

void CDLMessageHandler::OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	BASE_SESSION* pBaseSession = (BASE_SESSION*)pSocketSession->GetUserData();

	if(pBaseSession)
		pBaseSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
	
	return;
}

void CDLMessageHandler::OnMSRegisterAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	
	nRetCode = DoMSReady();
	LOG_PROCESS_ERROR(nRetCode);
    
    { // for linux compiling
        GD_SESSION& rGDSession = CDLSessionMgr::GetInstance().GetGDSession();
        if (rGDSession.bConnected && rGDSession.pSocketSession)
        {
            nRetCode = DoGDRegister();
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

	INF("register to MS successfully");
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CDLMessageHandler::OnMSServerInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;   
    M2A_SERVER_INFO* pMsg = (M2A_SERVER_INFO*)pPacketBuffer;

    return;
}

void CDLMessageHandler::OnMSAllReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;

	CDLSessionMgr::GetInstance().SetServerState(sgsAllReady);
	INF("all servers are ready");

	return;

}

void CDLMessageHandler::OnMSClose(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	CDLSessionMgr::GetInstance().SetServerState(sgsClosing);
	INF("DL is closing");
}

void CDLMessageHandler::OnMSChatMessage(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_CHAT_MESSAGE* pMsg = (M2O_CHAT_MESSAGE*)pPacketBuffer;
	char* pLogContent = NULL;

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);
	
	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%s\t%s\r\r\n", get_time_str(pMsg->dwTime), 
		g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSrcID, pMsg->dwTargetID, 
		GetDLChatTypeFromID(pMsg->dwTargetID), pMsg->chMessage);

	nRetCode = m_LogData.AsyncWriteLog(dlmtChat, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSLogItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    M2O_LOG_ITEM* pMsg = (M2O_LOG_ITEM*)pPacketBuffer;

    nRetCode = LogItem(pMsg->dwTime, pMsg->nGroupID, pMsg->qwItemGuid, pMsg->dwRoleID, pMsg->nItemTemplateID,
        pMsg->nItemCount, pMsg->nReason, pMsg->nAction, pMsg->qwUserDefinedID);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CDLMessageHandler::OnMSStatItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    M2O_STAT_ITEM   *pMsg = (M2O_STAT_ITEM*)pPacketBuffer;

    nRetCode = _StatItem(pMsg->nGroupID, pMsg->nItemTemplateID, pMsg->nItemCount, pMsg->nAction);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CDLMessageHandler::OnMSLogin(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_LOGIN* pMsg = (M2O_LOGIN*)pPacketBuffer;
	char* pLogContent = NULL;
	in_addr stAddr;

	stAddr.s_addr = pMsg->dwIP;

	nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertLogin, NULL, NULL, "dhcsss", pMsg->dwTime,
		g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->szUserName, GetLoginStringByType(dlLogin), inet_ntoa(stAddr));
	LOG_CHECK_ERROR(nRetCode);

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%s\t%s\t%s\r\n", get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID,
		pMsg->nGroupID, pMsg->szUserName, GetLoginStringByType(dlLogin), inet_ntoa(stAddr));

	nRetCode = m_LogData.AsyncWriteLog(dlmtLogin, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSLogout(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_LOGOUT* pMsg = (M2O_LOGOUT*)pPacketBuffer;
	char* pLogContent = NULL;
	in_addr stAddr;

	stAddr.s_addr = pMsg->dwIP;

	nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertLogin, NULL, NULL, "dhcsss", pMsg->dwTime,
		g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->szUserName, GetLoginStringByType(dlLogout), inet_ntoa(stAddr));
	LOG_CHECK_ERROR(nRetCode);

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%s\t%s\t%s\r\n", get_time_str(pMsg->dwTime),
		g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->szUserName, GetLoginStringByType(dlLogout), inet_ntoa(stAddr));

	nRetCode = m_LogData.AsyncWriteLog(dlmtLogin, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSCreateTeam(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_CREATE_TEAM* pMsg = (M2O_CREATE_TEAM*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%u\t%u\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, 
		pMsg->dwTeamID, pMsg->dwRoleID, pMsg->dwRoleID, 1, GetTeamActionByType(dtatCreateTeam));

	nRetCode = m_LogData.AsyncWriteLog(dlmtTeam, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSJoinTeam(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_JOIN_TEAM* pMsg = (M2O_JOIN_TEAM*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%u\t%d\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwTeamID, 
		pMsg->dwRoleID, pMsg->dwLeaderID, pMsg->nMemberCount, GetTeamActionByType(dtatJoinTeam));

	nRetCode = m_LogData.AsyncWriteLog(dlmtTeam, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSLeaveTeam(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_LEAVE_TEAM* pMsg = (M2O_LEAVE_TEAM*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%u\t%u\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwTeamID, 
		pMsg->dwRoleID, pMsg->dwLeaderID, pMsg->nMemberCount, GetTeamActionByType(dtatLeaveTeam));

	nRetCode = m_LogData.AsyncWriteLog(dlmtTeam, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSBreakDownTeam(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	char* pLogContent = NULL;
	M2O_BREAKDOWN_TEAM* pMsg = (M2O_BREAKDOWN_TEAM*)pPacketBuffer;

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%u\t%u\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwTeamID, 
		INVALID_OBJECT_ID, 0, 0, GetTeamActionByType(dtatBreakDownTeam));

	nRetCode = m_LogData.AsyncWriteLog(dlmtTeam, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSGMCommand(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	char* pLogContent = NULL;
	M2O_GM_COMMAND* pMsg = (M2O_GM_COMMAND*)pPacketBuffer;

	//nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertGm, NULL, NULL, "dhcdssc",
	//	pMsg->dwTime, g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, pMsg->szName, pMsg->szCommand, pMsg->bResult);
	//LOG_CHECK_ERROR(nRetCode);

	//pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	//LOG_PROCESS_ERROR(pLogContent);

	//snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%s\t%d\t%s\r\n", 
	//	get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, 
 //       pMsg->szName, pMsg->bResult, pMsg->szCommand);

	//nRetCode = m_LogData.AsyncWriteLog(dlmtGmCommand, pLogContent);
	//LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

    return;
}

void CDLMessageHandler::OnMSRealMoneyToItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_REALMONEY_ITEM* pMsg = (M2O_REALMONEY_ITEM*)pPacketBuffer;

	LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
	m_llRealMoneyToItem[pMsg->nGroupID] += pMsg->nCount;

	return;
Exit0:

	return;
}

void CDLMessageHandler::OnMSRealMoneyToMoney02(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{	
	INT32 nRetCode = 0;
	M2O_REALMONEY_MONEY02* pMsg = (M2O_REALMONEY_MONEY02*)pPacketBuffer;

	LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
	m_llRealMoneyToMoney02[pMsg->nGroupID] += pMsg->nCount;

	return;
Exit0:

	return;
}

void CDLMessageHandler::OnMSGuildEvent(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    char* pLogContent = NULL;
    M2O_GUILD_EVENT* pMsg = (M2O_GUILD_EVENT*)pPacketBuffer;

    pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%s\t%u\t%u\t%d\r\n",
        get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwGuildID,
        CDatalogServer::GetInstance().GetEnumString("GUILD_HISTORY_EVENT_TYPE", pMsg->nActionType).c_str(), pMsg->dwValue1, pMsg->dwValue2, (INT32)pMsg->bGM);

    nRetCode = m_LogData.AsyncWriteLog(dlmtGuildEvent, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return;
}

void CDLMessageHandler::OnMSGuildChest(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    char* pLogContent = NULL;
    M2O_GUILD_CHEST* pMsg = (M2O_GUILD_CHEST*)pPacketBuffer;

    pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%d\t%s\r\n",
        get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwGuildID,
        pMsg->nValue, CDatalogServer::GetInstance().GetEnumString("GUILD_HISTORY_EVENT_TYPE", pMsg->nReason).c_str());

    nRetCode = m_LogData.AsyncWriteLog(dlmtGuildChest, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return;
}

void CDLMessageHandler::OnMSGuildStat(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    char* pLogContent = NULL;
    M2O_GUILD_STAT* pMsg = (M2O_GUILD_STAT*)pPacketBuffer;

    pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);
    memset(pLogContent, 0, MAX_DL_RECORD_MESSAGE_LEN);
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "%s\t%d\t%d\t%d\r\n",
            get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, i, pMsg->stat[i].nGuildNum);
    }
    nRetCode = m_LogData.AsyncWriteLog(dlmtGuildNumStat, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);    

    // money stat
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);
        memset(pLogContent, 0, MAX_DL_RECORD_MESSAGE_LEN);

        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "%s\t%d\t%d",
            get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, i);
        for (INT32 j = 0; j < GUILD_STAT_TOP_NUM; ++j)
        {
            snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\t%u\t%u",
                pMsg->stat[i].MoneyList[j].dwGuildID, pMsg->stat[i].MoneyList[j].dwValue);
        }
        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\r\n");

        nRetCode = m_LogData.AsyncWriteLog(dlmtGuildMoneyStat, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }


    // rice stat
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);
        memset(pLogContent, 0, MAX_DL_RECORD_MESSAGE_LEN);

        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "%s\t%d\t%d",
            get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, i);
        for (INT32 j = 0; j < GUILD_STAT_TOP_NUM; ++j)
        {
            snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\t%u\t%u",
                pMsg->stat[i].RiceList[j].dwGuildID, pMsg->stat[i].RiceList[j].dwValue);
        }
        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\r\n");

        nRetCode = m_LogData.AsyncWriteLog(dlmtGuildRiceStat, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }

    // wood stat
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);
        memset(pLogContent, 0, MAX_DL_RECORD_MESSAGE_LEN);

        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "%s\t%d\t%d",
            get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, i);
        for (INT32 j = 0; j < GUILD_STAT_TOP_NUM; ++j)
        {
            snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\t%u\t%u",
                pMsg->stat[i].WoodList[j].dwGuildID, pMsg->stat[i].WoodList[j].dwValue);
        }
        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\r\n");

        nRetCode = m_LogData.AsyncWriteLog(dlmtGuildWoodStat, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }

    // stone stat
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);
        memset(pLogContent, 0, MAX_DL_RECORD_MESSAGE_LEN);

        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "%s\t%d\t%d",
            get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, i);
        for (INT32 j = 0; j < GUILD_STAT_TOP_NUM; ++j)
        {
            snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\t%u\t%u",
                pMsg->stat[i].StoneList[j].dwGuildID, pMsg->stat[i].StoneList[j].dwValue);
        }
        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\r\n");

        nRetCode = m_LogData.AsyncWriteLog(dlmtGuildStoneStat, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }

    // metal stat
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);
        memset(pLogContent, 0, MAX_DL_RECORD_MESSAGE_LEN);

        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "%s\t%d\t%d",
            get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, i);
        for (INT32 j = 0; j < GUILD_STAT_TOP_NUM; ++j)
        {
            snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\t%u\t%u",
                pMsg->stat[i].MetalList[j].dwGuildID, pMsg->stat[i].MetalList[j].dwValue);
        }
        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\r\n");

        nRetCode = m_LogData.AsyncWriteLog(dlmtGuildMetalStat, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }


    // building stat
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
        LOG_PROCESS_ERROR(pLogContent);
        memset(pLogContent, 0, MAX_DL_RECORD_MESSAGE_LEN);

        for (INT32 k = 1; k < GUILD_MAX_BUILDING_LEVEL + 1; ++k)
        {
            snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "%s\t%d\t%d\t%d", 
                get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, i, k);
            for (INT32 j = GUILD_B_MAIN; j < GUILD_B_NUM; ++j)
            {
                snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\t%d\t%d", j, pMsg->stat[i].BuildingStat[j][k]);
            }
            snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\r\n");
        }
        snprintf(pLogContent + strlen(pLogContent), MAX_DL_RECORD_MESSAGE_LEN - strlen(pLogContent), "\r\n");

        nRetCode = m_LogData.AsyncWriteLog(dlmtGuildBuildingStat, pLogContent);
        LOG_PROCESS_ERROR(nRetCode);
    }



    return;
Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return;
}

void CDLMessageHandler::OnMSBillingRequest(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    char* pLogContent = NULL;
    M2O_BILLING_REQUEST* pMsg = (M2O_BILLING_REQUEST*)pPacketBuffer;

    pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%u\t%s\t%s\t%s\t%s\t%u\t%u\t%I64u\t%I64u\t%d\t%d\r\n",
        get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID,
        pMsg->nAction, pMsg->dwRpcID,
        pMsg->szUserName, pMsg->szUserName2,
        pMsg->szUniqueID, pMsg->szUniqueID2,
        pMsg->dwRoleID, pMsg->dwRoleID2,
        pMsg->qwOrderID, pMsg->qwOrderID2,
        pMsg->nAmount, pMsg->nParam);

    nRetCode = m_LogData.AsyncWriteLog(dlmtBillingRequest, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return;
}

void CDLMessageHandler::OnMSBillingResult(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    char* pLogContent = NULL;
    M2O_BILLING_RESULT* pMsg = (M2O_BILLING_RESULT*)pPacketBuffer;

    pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%u\t%s\t%d\r\n",
        get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->nAction, pMsg->dwRpcID, pMsg->szUniqueID, pMsg->nResult);

    nRetCode = m_LogData.AsyncWriteLog(dlmtBillingResult, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return;
}

/************************************************************************/
// From GS
void CDLMessageHandler::OnGSRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_REGISTER* pMsg = (G2O_REGISTER*)pPacketBuffer;

	GS_SESSION* pGSSession = CDLSessionMgr::GetInstance().GetGSSession(pMsg->dwGSSessionID);
    LOG_PROCESS_ERROR(pGSSession);
    LOG_PROCESS_ERROR(CDLSessionMgr::GetInstance().GetServerState() != sgsClosing);

	pGSSession->pSocketSession = pSocketSession;
	pGSSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
	pSocketSession->SetUserData(pGSSession);

	nRetCode = DoGSRegisterAck(pGSSession->dwSessionID);
	LOG_PROCESS_ERROR(nRetCode);

	INF("GS %d register to DL", pMsg->dwGSSessionID);

	return;
Exit0:
	if (pSocketSession)
		pSocketSession->Close();

	return;
}

void CDLMessageHandler::OnGSChatMessage(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_CHAT_MESSAGE* pMsg = (G2O_CHAT_MESSAGE*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%s\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSourceID, pMsg->dwTargetID, 
		GetDLChatTypeFromID(pMsg->dwTargetID), pMsg->szMessage);

	nRetCode = m_LogData.AsyncWriteLog(dlmtChat, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSCompleteQuest(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_COMPLETE_QUEST* pMsg = (G2O_COMPLETE_QUEST*)pPacketBuffer;
	char* pLogContent = NULL;
	const QUEST_DATA* pQuest = NULL;
	
	LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
	m_llQuestCompleteCount[pMsg->nGroupID]++;

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	pQuest = CDataManager<QUEST_DATA>::GetInstance().GetData(pMsg->nQuestID);
	LOG_PROCESS_ERROR(pQuest);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%d\t%s\t%s\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSrcID, 
        pMsg->nQuestID, GetQuestActionByType(dqtComplete), CDatalogServer::GetInstance().GetEnumString("QUEST_TYPE", pQuest->nQuestType).c_str(), pQuest->szName);

	nRetCode = m_LogData.AsyncWriteLog(dlmtQuest, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSAbandonQuest(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_COMPLETE_QUEST* pMsg = (G2O_COMPLETE_QUEST*)pPacketBuffer;
	char* pLogContent = NULL;
	const QUEST_DATA* pQuest = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	pQuest = CDataManager<QUEST_DATA>::GetInstance().GetData(pMsg->nQuestID);
	LOG_PROCESS_ERROR(pQuest);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%d\t%s\t%s\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSrcID, pMsg->nQuestID, 
        GetQuestActionByType(dqtAbandon), CDatalogServer::GetInstance().GetEnumString("QUEST_TYPE", pQuest->nQuestType).c_str(), pQuest->szName);

	nRetCode = m_LogData.AsyncWriteLog(dlmtQuest, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSCompleteAchievement(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	const ACHIEVEMENT_DATA* pAchievementData{};
	INT32 nRetCode = 0;
	G2O_COMPLETE_ACHIEVEMENT* pMsg = (G2O_COMPLETE_ACHIEVEMENT*)pPacketBuffer;
	char* pLogContent = NULL;

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	pAchievementData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(pMsg->nAchievementID);
	LOG_PROCESS_ERROR(pAchievementData);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%d\t%s\r\n",
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, 
		pMsg->nAchievementID, pAchievementData->szName);

	nRetCode = m_LogData.AsyncWriteLog(dlmtAchievement, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSAcceptQuest(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_ACCEPT_QUEST* pMsg = (G2O_ACCEPT_QUEST*)pPacketBuffer;
	char* pLogContent = NULL;
	const QUEST_DATA* pQuest = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	pQuest = CDataManager<QUEST_DATA>::GetInstance().GetData(pMsg->nQuestID);
	LOG_PROCESS_ERROR(pQuest);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%d\t%s\t%s\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSrcID, 
        pMsg->nQuestID, GetQuestActionByType(dqtAccept), CDatalogServer::GetInstance().GetEnumString("QUEST_TYPE", pQuest->nQuestType).c_str(), pQuest->szName);

	nRetCode = m_LogData.AsyncWriteLog(dlmtQuest, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSLevelUp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_LEVEL_UP* pMsg = (G2O_LEVEL_UP*)pPacketBuffer;
	char* pLogContent = NULL;

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%d\t%d\t%d\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, pMsg->nLevel, 
		pMsg->nLevelOnlineTime, pMsg->nTotalOnlineTime);

	nRetCode = m_LogData.AsyncWriteLog(dlmtLevelUp, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSDie(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_DIE* pMsg = (G2O_DIE*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

 	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%d\t%llu\t%llu\t%llu\t%u\t%s\t%s\t%d\t%s\t%u\t%d\t%s\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->nSceneTemplateID, 
        pMsg->nSceneID, pMsg->llX, pMsg->llY, pMsg->llZ, pMsg->dwKillerID, CDatalogServer::GetInstance().GetEnumString("OBJECT_TYPE", pMsg->nKillerType).c_str(),
        pMsg->szKillerName, pMsg->nKillLevel, CDatalogServer::GetInstance().GetEnumString("CHAR_CLASS_TYPE", pMsg->nKillClass).c_str(), pMsg->dwVicID, pMsg->nVicLevel,
        CDatalogServer::GetInstance().GetEnumString("CHAR_CLASS_TYPE", pMsg->nVicClass).c_str(), pMsg->szVicName);

	nRetCode = m_LogData.AsyncWriteLog(dlmtDie, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSCreateScene(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_CREATE_SCENE* pMsg = (G2O_CREATE_SCENE*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%d\t%u\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->nSceneID, pMsg->nTemplateID, 
		0, GetSceneTypeTypeByType(dsstCreate));

	nRetCode = m_LogData.AsyncWriteLog(dlmtScene, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSDestoryScene(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_DESTORY_SCENE* pMsg = (G2O_DESTORY_SCENE*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%d\t%u\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->nSceneID, 
		pMsg->nTemplateID, 0, GetSceneTypeTypeByType(dsstDestory));

	nRetCode = m_LogData.AsyncWriteLog(dlmtScene, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSEnterScene(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_ENTER_SCENE* pMsg = (M2O_ENTER_SCENE*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%d\t%u\t%s\r\n", get_time_str(pMsg->dwTime), 
		g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->nSceneID, pMsg->nTemplateID, pMsg->dwRoleID, GetSceneTypeTypeByType(dsstEnter));

	nRetCode = m_LogData.AsyncWriteLog(dlmtScene, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnMSLeaveScene(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2O_LEAVE_SCENE* pMsg = (M2O_LEAVE_SCENE*)pPacketBuffer;
	char* pLogContent = NULL;

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%d\t%u\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, 
		pMsg->nGroupID, pMsg->nSceneID, pMsg->nTemplateID, 
		pMsg->dwRoleID, GetSceneTypeTypeByType(dsstLeave));

	nRetCode = m_LogData.AsyncWriteLog(dlmtScene, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSBuyConSlot(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_BUY_CONSLOT* pMsg = (G2O_BUY_CONSLOT*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%s\t%d\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, 
        CDatalogServer::GetInstance().GetEnumString("CONTAINER_SLOT", pMsg->nConType).c_str(), pMsg->nConSize);

	nRetCode = m_LogData.AsyncWriteLog(dlmtConSlot, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

BOOL LogItem(UINT32 dwTime, INT32 nGroupID, UINT64 qwItemGuid, UINT32 dwRoleID, INT32 nItemTemplateID, 
    INT32 nItemCount, INT32 nReason, INT32 nAction, UINT64 qwUserDefinedID)
{
    INT32 nRetCode = 0;

    char* pLogContent = NULL;

    nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(
        dsqlInsertItem, NULL, NULL, "dhcudsulddsul",
        dwTime, g_ServerConfig.Common.nAreaID, nGroupID,
        dwRoleID,
        CDatalogServer::GetInstance().GetEnumString("EItemActionType", nAction).c_str(),
        qwItemGuid, nItemTemplateID, nItemCount,
        CDatalogServer::GetInstance().GetEnumString("EItemLog", nReason).c_str(),
        qwUserDefinedID);
    LOG_CHECK_ERROR(nRetCode);

    pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%s\t%I64u\t%d\t%d\t%s\t%I64u\r\n",
        get_time_str(dwTime), g_ServerConfig.Common.nAreaID, nGroupID,
        dwRoleID,
        CDatalogServer::GetInstance().GetEnumString("EItemActionType", nAction).c_str(),
        qwItemGuid, nItemTemplateID, nItemCount,
        CDatalogServer::GetInstance().GetEnumString("EItemLog", nReason).c_str(),
        qwUserDefinedID);

    nRetCode = CDLMessageHandler::GetInstance().GetLogData().AsyncWriteLog(dlmtItem, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return FALSE;
}

void CDLMessageHandler::OnGSLogItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2O_LOG_ITEM* pMsg = (G2O_LOG_ITEM*)pPacketBuffer;

    nRetCode = LogItem(pMsg->dwTime, pMsg->nGroupID, pMsg->qwItemGuid, pMsg->dwRoleID, pMsg->nItemTemplateID, 
        pMsg->nItemCount, pMsg->nReason, pMsg->nAction, pMsg->qwUserDefinedID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

BOOL CDLMessageHandler::_StatItem(INT32 nGroupID, INT32 nItemTemplateID, INT32 nItemCount, INT32 nAction)
{
    LOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_SERVER_GROUP_COUNT);

    if (nAction)
        m_MapItemStat[nGroupID][nItemTemplateID].llProduce += nItemCount;
    else
        m_MapItemStat[nGroupID][nItemTemplateID].llDestroy += nItemCount;

    return TRUE;
Exit0:
    return FALSE;
}

void CDLMessageHandler::OnGSStatItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2O_STAT_ITEM* pMsg = (G2O_STAT_ITEM*)pPacketBuffer;

    nRetCode = _StatItem(pMsg->nGroupID, pMsg->nItemTemplateID, pMsg->nItemCount, pMsg->nAction);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CDLMessageHandler::OnGSRuneItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2O_RUNE_ITEM* pMsg = (G2O_RUNE_ITEM*)pPacketBuffer;
    char* pLogContent = NULL;

    pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%d\t%d\t%d\t%d\r\n",
        get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID,
        pMsg->dwRoleID, pMsg->nRoleLevel, pMsg->nEquipTemplateID, pMsg->nRuneItemID, pMsg->nRuneID);

    nRetCode = m_LogData.AsyncWriteLog(dlmtRuneItem, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return;
}

void CDLMessageHandler::OnGSChangeItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2O_CHANGE_ITEM* pMsg = (G2O_CHANGE_ITEM*)pPacketBuffer;
    char* pLogContent = NULL;

    pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
    LOG_PROCESS_ERROR(pLogContent);

    snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%s\t%I64u\t%d\t%s\r\n",
        get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID,
        pMsg->dwRoleID, CDatalogServer::GetInstance().GetEnumString("EItemLog", pMsg->nReason).c_str(),
        pMsg->qwItemGuid, pMsg->nItemTemplateID, pMsg->szItem);

    nRetCode = m_LogData.AsyncWriteLog(dlmtChangeItem, pLogContent);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    SAFE_DELETE_ARRAY(pLogContent);

    return;
}

BOOL CDLMessageHandler::_LogBase(CRole* pRole, InternalMessage::G2O_ROLE_LOGOUT* pMsg)
{
	INT32 nRetCode = 0;
	char* pLogContent = NULL;

	nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertRoleLogoutBasic, NULL, NULL, "dhcdsddsssddddddd",
		pMsg->dwTime, g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pRole->GetObjID(), pRole->GetUserName(), pRole->GetSceneID(),
        pRole->GetSceneTempID(), pRole->GetName(), CDatalogServer::GetInstance().GetEnumString("CHAR_CLASS_TYPE", pRole->GetClass()).c_str(),
        CDatalogServer::GetInstance().GetEnumString("CHAR_SEX_TYPE", pRole->GetSex()).c_str(),
		pRole->GetLevel(), pRole->GetExp(), pRole->GetItemBag()->GetMoneyInfo(MONEYTYPE_01),
		pRole->GetItemBag()->GetMoneyInfo(MONEYTYPE_02), (INT32)pRole->GetX(), (INT32)pRole->GetY(), (INT32)pRole->GetZ());
	LOG_CHECK_ERROR(nRetCode);

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%ld\t%ld\t%lu\t%s\t%lu\t%lu\t%s\t%s\t%s\t%ld\t%ld\t%ld\t%ld\t%d\t%d\t%d\t%s\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pRole->GetObjID(), pRole->GetUserName(), 
        pRole->GetSceneID(), pRole->GetSceneTempID(), pRole->GetName(), CDatalogServer::GetInstance().GetEnumString("CHAR_CLASS_TYPE", pRole->GetClass()).c_str(),
        CDatalogServer::GetInstance().GetEnumString("CHAR_SEX_TYPE", pRole->GetSex()).c_str(), pRole->GetLevel(), pRole->GetExp(), pRole->GetItemBag()->GetMoneyInfo(MONEYTYPE_01),
		pRole->GetItemBag()->GetMoneyInfo(MONEYTYPE_02), (INT32)pRole->GetX(), (INT32)pRole->GetY(), (INT32)pRole->GetZ(), pMsg->szGuildName);

	nRetCode = m_LogData.AsyncWriteLog(dlmtRoleLogout_basic, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return FALSE;
}

//BOOL CDLMessageHandler::_LogFriend(CRole* pRole, G2O_ROLE_LOGOUT* pMsg)
//{
//    INT32 nRetCode = 0;
//    char* pLogContent = NULL;
//
//    for (UINT32 dwFriendID = pRole->GetFriend()->Begin(); dwFriendID != INVALID_OBJECT_ID; dwFriendID = pRole->GetFriend()->Next())
//    {
//        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
//        LOG_PROCESS_ERROR(pLogContent);
//
//        snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%s\t%s\t%u\r\n",
//            get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID,
//            pRole->GetObjID(), pRole->GetUserName(), pRole->GetName(), dwFriendID);
//
//        nRetCode = m_LogData.AsyncWriteLog(dlmtRoleLogout_friend, pLogContent);
//        LOG_PROCESS_ERROR(nRetCode);
//    }
//
//    return TRUE;
//
//Exit0:
//	SAFE_DELETE_ARRAY(pLogContent);
//
//	return FALSE;
//}

BOOL CDLMessageHandler::_LogItem(CRole* pRole, G2O_ROLE_LOGOUT* pMsg)
{
	INT32 nRetCode = 0;
	char* pLogContent_item_default = NULL;
	char* pLogContent_item_equipped = NULL;
	UINT64	qwGuid[MAX_DL_RECORD_ITEM_COUNT];
	INT32	nTemplageID[MAX_DL_RECORD_ITEM_COUNT];
	INT32	nColor[MAX_DL_RECORD_ITEM_COUNT];

	pLogContent_item_default = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent_item_default);
	memset(pLogContent_item_default, 0, MAX_DL_RECORD_MESSAGE_LEN);

	pLogContent_item_equipped = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent_item_equipped);
	memset(pLogContent_item_equipped, 0, MAX_DL_RECORD_MESSAGE_LEN);

	for (INT32 nBagIndex = CONSLOT_EQUIPPED; nBagIndex <= CONSLOT_DEFAULT; nBagIndex++)
	{
		pRole->GetItemBag()->Begin(nBagIndex);

		memset(qwGuid, 0, sizeof(qwGuid));
		memset(nTemplageID, 0, sizeof(nTemplageID));
		memset(nColor, 0, sizeof(nColor));

		for (INT32 nIndex = 0; nIndex < MAX_CONTAINER_SIZE[nBagIndex]; nIndex++)
		{
			const Item* pItem = pRole->GetItemBag()->Next(nBagIndex);

			if (pItem)
			{
				qwGuid[nIndex] = pItem->m_iteminfo.m_llGuid;
				nTemplageID[nIndex] = pItem->m_pTemplate->m_nTemplateID;
				nColor[nIndex] = pItem->m_pTemplate->m_byColor;
			}
			else
			{
				qwGuid[nIndex] = 0;
				nTemplageID[nIndex] = 0;
				nColor[nIndex] = 0;
			}
		}

		switch (nBagIndex)
		{
		case CONSLOT_EQUIPPED:
		{
			snprintf(pLogContent_item_equipped, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%ld\t%ld\t%lu\t%s\t%s\t"	\
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\r\n",
				get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, 
				pRole->GetObjID(), pRole->GetUserName(), pRole->GetName(),
				qwGuid[0], nTemplageID[0], nColor[0], qwGuid[1], nTemplageID[1], nColor[1],
				qwGuid[2], nTemplageID[2], nColor[2], qwGuid[3], nTemplageID[3], nColor[3],
				qwGuid[4], nTemplageID[4], nColor[4], qwGuid[5], nTemplageID[5], nColor[5],
				qwGuid[6], nTemplageID[6], nColor[6], qwGuid[7], nTemplageID[7], nColor[7],
				qwGuid[8], nTemplageID[8], nColor[8], qwGuid[9], nTemplageID[9], nColor[9],
				qwGuid[10], nTemplageID[10], nColor[10], qwGuid[11], nTemplageID[11], nColor[11],
				qwGuid[12], nTemplageID[12], nColor[12], qwGuid[13], nTemplageID[13], nColor[13],
				qwGuid[14], nTemplageID[14], nColor[14]);

			nRetCode = m_LogData.AsyncWriteLog(dlmtRoleLogout_itembag_equipped, pLogContent_item_equipped);
			LOG_PROCESS_ERROR(nRetCode);

			pLogContent_item_equipped = NULL;

			break;
		}
		case CONSLOT_DEFAULT:
		{
			snprintf(pLogContent_item_default, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%ld\t%ld\t%lu\t%s\t%s\t"	\
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t" \
				"0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\t0x%016I64x\t%ld\t%ld\r\n", 
				get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, 
				pRole->GetObjID(), pRole->GetUserName(), pRole->GetName(), 
				qwGuid[0], nTemplageID[0], nColor[0], qwGuid[1], nTemplageID[1], nColor[1], 
				qwGuid[2], nTemplageID[2], nColor[2], qwGuid[3], nTemplageID[3], nColor[3], 
				qwGuid[4], nTemplageID[4], nColor[4], qwGuid[5], nTemplageID[5], nColor[5], 
				qwGuid[6], nTemplageID[6], nColor[6], qwGuid[7], nTemplageID[7], nColor[7], 
				qwGuid[8], nTemplageID[8], nColor[8], qwGuid[9], nTemplageID[9], nColor[9], 
				qwGuid[10], nTemplageID[10], nColor[10], qwGuid[11], nTemplageID[11], nColor[11], 
				qwGuid[12], nTemplageID[12], nColor[12], qwGuid[13], nTemplageID[13], nColor[13], 
				qwGuid[14], nTemplageID[14], nColor[14], qwGuid[15], nTemplageID[15], nColor[15], 
				qwGuid[16], nTemplageID[16], nColor[16], qwGuid[17], nTemplageID[17], nColor[17], 
				qwGuid[18], nTemplageID[18], nColor[18], qwGuid[19], nTemplageID[19], nColor[19], 
				qwGuid[20], nTemplageID[20], nColor[20], qwGuid[21], nTemplageID[21], nColor[21], 
				qwGuid[22], nTemplageID[22], nColor[22], qwGuid[23], nTemplageID[23], nColor[23], 
				qwGuid[24], nTemplageID[24], nColor[24], qwGuid[25], nTemplageID[25], nColor[25], 
				qwGuid[26], nTemplageID[26], nColor[26], qwGuid[27], nTemplageID[27], nColor[27], 
				qwGuid[28], nTemplageID[28], nColor[28], qwGuid[29], nTemplageID[29], nColor[29], 
				qwGuid[30], nTemplageID[30], nColor[30], qwGuid[31], nTemplageID[31], nColor[31], 
				qwGuid[32], nTemplageID[32], nColor[32], qwGuid[33], nTemplageID[33], nColor[33], 
				qwGuid[34], nTemplageID[34], nColor[34], qwGuid[35], nTemplageID[35], nColor[35], 
				qwGuid[36], nTemplageID[36], nColor[36], qwGuid[37], nTemplageID[37], nColor[37], 
				qwGuid[38], nTemplageID[38], nColor[38], qwGuid[39], nTemplageID[39], nColor[39], 
				qwGuid[40], nTemplageID[40], nColor[40], qwGuid[41], nTemplageID[41], nColor[41], 
				qwGuid[42], nTemplageID[42], nColor[42], qwGuid[43], nTemplageID[43], nColor[43], 
				qwGuid[44], nTemplageID[44], nColor[44], qwGuid[45], nTemplageID[45], nColor[45], 
				qwGuid[46], nTemplageID[46], nColor[46], qwGuid[47], nTemplageID[47], nColor[47], 
				qwGuid[48], nTemplageID[48], nColor[48], qwGuid[49], nTemplageID[49], nColor[49], 
				qwGuid[50], nTemplageID[50], nColor[50], qwGuid[51], nTemplageID[51], nColor[51], 
				qwGuid[52], nTemplageID[52], nColor[52], qwGuid[53], nTemplageID[53], nColor[53], 
				qwGuid[54], nTemplageID[54], nColor[54], qwGuid[55], nTemplageID[55], nColor[55], 
				qwGuid[56], nTemplageID[56], nColor[56], qwGuid[57], nTemplageID[57], nColor[57], 
				qwGuid[58], nTemplageID[58], nColor[58], qwGuid[59], nTemplageID[59], nColor[59], 
				qwGuid[60], nTemplageID[60], nColor[60], qwGuid[61], nTemplageID[61], nColor[61], 
				qwGuid[62], nTemplageID[62], nColor[62], qwGuid[63], nTemplageID[63], nColor[63]);

			nRetCode = m_LogData.AsyncWriteLog(dlmtRoleLogout_itembag_default, pLogContent_item_default);
			LOG_PROCESS_ERROR(nRetCode);

			pLogContent_item_default = NULL;

			break;
		}
		default:
		{
			LOG_PROCESS_ERROR(FALSE);
			break;

		}
		}
	}

	return TRUE;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent_item_default);
	SAFE_DELETE_ARRAY(pLogContent_item_equipped);

	return FALSE;
}

//BOOL CDLMessageHandler::_LogMaster(CRole* pRole, G2O_ROLE_LOGOUT* pMsg)
//{
//    INT32 nRetCode = 0;
//    char* pLogContent = NULL;
//
//    for (INT32 nIndex = 0; nIndex < pRole->GetMaster().GetStudentCount(); nIndex++)
//    {
//        pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
//        LOG_PROCESS_ERROR(pLogContent);
//
//        snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%s\t%s\t%u\r\n", \
//            get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pRole->GetObjID(),
//            pRole->GetUserName(), pRole->GetName(), pRole->GetMaster().GetStudentByIndex(nIndex));
//    }
//
//    nRetCode = m_LogData.AsyncWriteLog(dlmtRoleLogout_master, pLogContent);
//    LOG_PROCESS_ERROR(nRetCode);
//
//    return TRUE;
//
//Exit0:
//    SAFE_DELETE_ARRAY(pLogContent);
//
//    return FALSE;
//}

void CDLMessageHandler::OnGSRoleLogout(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_ROLE_LOGOUT* pMsg = (G2O_ROLE_LOGOUT*)pPacketBuffer;
	CRole* pRole = NULL;

	pRole = CGameWorld::GetInstance().NewRole(pMsg->dwRoleID);
	LOG_PROCESS_ERROR(pRole);

 	nRetCode = pRole->Init(pMsg->dwRoleID, pMsg->dwSessionID);
 	LOG_PROCESS_ERROR(pRole);

	nRetCode = pRole->Load(pMsg->szData, pMsg->dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _LoadPosInfo(pMsg->szData, pRole);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _LogBase(pRole, pMsg);
	LOG_CHECK_ERROR(nRetCode);

	//nRetCode = _LogFriend(pRole, pMsg);
	//LOG_CHECK_ERROR(nRetCode);

	nRetCode = _LogItem(pRole, pMsg);
	LOG_CHECK_ERROR(nRetCode);

	//nRetCode = _LogMaster(pRole, pMsg);
	//LOG_CHECK_ERROR(nRetCode);

Exit0:
	if (pRole)
	{
		nRetCode = pRole->UnInit();
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = CGameWorld::GetInstance().DelRole(pRole);
		LOG_CHECK_ERROR(nRetCode);
	}

	return;
}

void CDLMessageHandler::OnGSGMCommand(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_GM_COMMAND* pMsg = (G2O_GM_COMMAND*)pPacketBuffer;
	char* pLogContent = NULL;

	//nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertGm, NULL, NULL, "dhcdssc", 
	//	pMsg->dwTime, g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, pMsg->szName, pMsg->szCommand, pMsg->bResult);
	//LOG_CHECK_ERROR(nRetCode);

	//pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	//LOG_PROCESS_ERROR(pLogContent);

	//snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%s\t%d\t%s\r\n", 
	//	get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, 
 //       pMsg->szName, pMsg->bResult, pMsg->szCommand);

	//nRetCode = m_LogData.AsyncWriteLog(dlmtGmCommand, pLogContent);
	//LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSAddMoney(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	char* pLogContent = NULL;
	G2O_ADD_MONEY* pMsg = (G2O_ADD_MONEY*)pPacketBuffer;

	LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
	if (pMsg->dwSrcRoleID == INVALID_OBJECT_ID && pMsg->nMoneyType <= MONEYTYPE_02 && pMsg->nMoneyType >= MONEYTYPE_01)
	{
		m_llMoneyProduce[pMsg->nGroupID][pMsg->nMoneyType] += pMsg->nMoneyCount;
	}

	nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertMoney, NULL, NULL, "dhcddsddsd",
		pMsg->dwTime, g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSrcRoleID, pMsg->dwTarRoleID,
        "Add", pMsg->nMoneyType, pMsg->nMoneyCount, CDatalogServer::GetInstance().GetEnumString("EMoneyLog", pMsg->nReason).c_str(), pMsg->nParam);
	LOG_CHECK_ERROR(nRetCode);

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%s\t%d\t%d\t%s\t%d\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSrcRoleID, 
        pMsg->dwTarRoleID, "Add", pMsg->nMoneyType, pMsg->nMoneyCount, CDatalogServer::GetInstance().GetEnumString("EMoneyLog", pMsg->nReason).c_str(), pMsg->nParam);

	nRetCode = m_LogData.AsyncWriteLog(dlmtSubMoney, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSSubMoney(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
    char* pLogContent = NULL;
    G2O_SUB_MONEY* pMsg = (G2O_SUB_MONEY*)pPacketBuffer;

    LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
    if (pMsg->dwTarRoleID == INVALID_OBJECT_ID && pMsg->nMoneyType <= MONEYTYPE_02 && pMsg->nMoneyType >= MONEYTYPE_01)
    {
        m_llMoneyDestory[pMsg->nGroupID][pMsg->nMoneyType] += pMsg->nMoneyCount;
    }

	nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertMoney, NULL, NULL, "dhcddsddsd", 
		pMsg->dwTime, g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSrcRoleID, pMsg->dwTarRoleID,
        "Sub", pMsg->nMoneyType, pMsg->nMoneyCount, CDatalogServer::GetInstance().GetEnumString("EMoneyLog", pMsg->nReason).c_str(), pMsg->nParam);
	LOG_CHECK_ERROR(nRetCode);

	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	LOG_PROCESS_ERROR(pMsg->nReason > MONEYLOG_INVALID);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%s\t%d\t%d\t%s\t%d\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwSrcRoleID,  
        pMsg->dwTarRoleID, "Sub", pMsg->nMoneyType, pMsg->nMoneyCount, CDatalogServer::GetInstance().GetEnumString("EMoneyLog", pMsg->nReason).c_str(), pMsg->nParam);

	nRetCode = m_LogData.AsyncWriteLog(dlmtSubMoney, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSRolePosition(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32	nRetCode = 0;
	G2O_ROLE_POSITION* pMsg = (G2O_ROLE_POSITION*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%d\t%d\t%d\t%d\t%d\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, 
		pMsg->nSceneTemplateID, pMsg->nSceneID, pMsg->nX, pMsg->nY, pMsg->nZ);

	nRetCode = m_LogData.AsyncWriteLog(dlmtRolePosition, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSNpcDie(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_NPC_DIE* pMsg = (G2O_NPC_DIE*)pPacketBuffer;

	LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
	m_llNpcTotalDieCount[pMsg->nGroupID]++;

	if (pMsg->bDieIDMonitor)
	{
		m_MapNpcIDDieStatistic[pMsg->nGroupID][pMsg->dwNpcTemplateID]++;
	}

	return;
Exit0:
	return;
}

void CDLMessageHandler::OnGSNpcMoney(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2O_NPC_MONEY* pMsg = (G2O_NPC_MONEY*)pPacketBuffer;

    LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
    m_llNpcTotalMoneyCount[pMsg->nGroupID] += pMsg->nMoneyCount;

    if (pMsg->bMoneyStat)
    {
        m_MapNpcIDMoneyStatistic[pMsg->nGroupID][pMsg->dwNpcTemplateID] += pMsg->nMoneyCount;
    }

    return;
Exit0:
    return;
}

void CDLMessageHandler::OnGSConsecutiveKillNpcCount(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_CONSECUTIVE_KILL_NPC_COUNT* pMsg = (G2O_CONSECUTIVE_KILL_NPC_COUNT*)pPacketBuffer;

	LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
	LOG_PROCESS_ERROR(pMsg->nConsecutiveKillNpcCount >= MIN_CONSECUTIVE_KILL_NPC_COUNT);
    LOG_PROCESS_ERROR(pMsg->nConsecutiveKillNpcCount <= MAX_CONSECUTIVE_KILL_COUNT);

	if (pMsg->nConsecutiveKillNpcCount <= MAX_CONSECUTIVE_KILL_COUNT)
	{
		m_llConsecutiveKill[pMsg->nGroupID][pMsg->nConsecutiveKillNpcCount/5]++;
	}

	return;
Exit0:
	return;
}

void CDLMessageHandler::OnGSMomentaryKillNpcCount(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_MOMENTARY_KILL_NPC_COUNT* pMsg = (G2O_MOMENTARY_KILL_NPC_COUNT*)pPacketBuffer;

	LOG_PROCESS_ERROR(pMsg->nGroupID >= 0 && pMsg->nGroupID < MAX_SERVER_GROUP_COUNT);
	LOG_PROCESS_ERROR(pMsg->nMomentaryKillNpcCount >= MIN_MOMENTARY_KILL_NPC_COUNT);	

	if (pMsg->nMomentaryKillNpcCount <= MAX_MOMENTARY_KILL_COUNT)
	{
		m_llMomentaryKill[pMsg->nGroupID][pMsg->nMomentaryKillNpcCount-MIN_MOMENTARY_KILL_NPC_COUNT]++;
	}

	return;
Exit0:
	return;
}

void CDLMessageHandler::OnGSClientHardwareInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_CLIENT_HARDWARE_INFO* pMsg = (G2O_CLIENT_HARDWARE_INFO*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, 
		"%s\t%d\t%d\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%s\t%u\t%u\t%u\t%s\t%s\t%s\t%s\t%lu\t%lu\t%u\t%u\t%s\t%s\t%s\t%u\t%u\t%u\t%u\t%u\t%s\t%u\t%s\r\n",		
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, 
		pMsg->stcpuinfo.AddressWidth, pMsg->stcpuinfo.Family, pMsg->stcpuinfo.L2CacheSize, pMsg->stcpuinfo.L3CacheSize,
		pMsg->stcpuinfo.Level, pMsg->stcpuinfo.MaxClockSpeed, pMsg->stcpuinfo.Name, pMsg->stcpuinfo.NumberOfCores,
		pMsg->stcpuinfo.NumberOfLogicalProcessors, 
		pMsg->stcpuinfo.Revision, pMsg->stcpuinfo.Stepping, pMsg->stcpuinfo.Version,
		pMsg->stdiskinfo.MediaType, pMsg->stdiskinfo.Model, pMsg->stdiskinfo.Size, 
		pMsg->stmemoryinfo.Capacity, pMsg->stmemoryinfo.MemoryType, pMsg->stmemoryinfo.Speed,
		pMsg->stnetworkadapterinfo.AdapterType, pMsg->stnetworkadapterinfo.Name,pMsg->stnetworkadapterinfo.NetworkAdapterInfo,
		pMsg->stvideoinfo.AdapterRAM, pMsg->stvideoinfo.CurrentBitsPerPixel, pMsg->stvideoinfo.CurrentHorizontalResolution,
		pMsg->stvideoinfo.CurrentRefreshRate, pMsg->stvideoinfo.CurrentVerticalResolution, pMsg->stvideoinfo.Name,
		pMsg->stvideoinfo.VideoMemoryType, pMsg->stvideoinfo.VideoProcessor);

	nRetCode = m_LogData.AsyncWriteLog(dlmtClientHardwareInfo, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSClientFpsInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_CLIENT_FPS_INFO* pMsg = (G2O_CLIENT_FPS_INFO*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%u\t%u\t%u\t%u\r\n", 
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, 
		pMsg->byGameFps, pMsg->byCpuFps, pMsg->byGpuFps, pMsg->wGateDelay, pMsg->wGameDelay);

	nRetCode = m_LogData.AsyncWriteLog(dlmtClientFpsInfo, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSSpellEffect(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_SPELL_EFFECT* pMsg = (G2O_SPELL_EFFECT*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%u\t%u\t%u\t%u\t%d\t%d\t%u\r\n",
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->byRuneSlot, pMsg->bySpellLevel, pMsg->bySpellSerial,
		pMsg->dwCasterID, pMsg->dwSpellID, pMsg->dwTargetID, pMsg->nParam1, pMsg->nParam2, pMsg->wFlag);

	nRetCode = m_LogData.AsyncWriteLog(dlmtSpellEffect, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSCastSpell(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_CAST_SPELL* pMsg = (G2O_CAST_SPELL*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%u\t%d\r\n",
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->byCasterLevel, pMsg->byRuneSlot, pMsg->bySpellLevel,
		pMsg->bySpellSerial, pMsg->dwCasterID, pMsg->dwSpellID, pMsg->dwTargetID,
		pMsg->nCdRemain, pMsg->nMp, pMsg->nTargetX, pMsg->nTargetY, pMsg->nTargetZ, pMsg->nErrCode, pMsg->byFromServer, pMsg->dwClientTime, pMsg->byShiftPressed);

	nRetCode = m_LogData.AsyncWriteLog(dlmtCastSpell, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

void CDLMessageHandler::OnGSLearnSpell(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_LEARN_SPELL* pMsg = (G2O_LEARN_SPELL*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%u\t%u\t%u\t%u\t%u\r\n",
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, pMsg->byRoleLevel, pMsg->nRoleExp, pMsg->nSpellID, pMsg->bySpellLevel);

	nRetCode = m_LogData.AsyncWriteLog(dlmtLearnSpell, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;

}

void CDLMessageHandler::OnGSAntiCheat(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	G2O_ROLE_ANTI_CHEAT_INFO* pMsg = (G2O_ROLE_ANTI_CHEAT_INFO*)pPacketBuffer;
	char* pLogContent = NULL;
	
	pLogContent = new char[MAX_DL_RECORD_MESSAGE_LEN];
	LOG_PROCESS_ERROR(pLogContent);

	// "Time\tAreaID\tGroupID\tRoleID\tJigsawSn\tAction\tErrCode\tImageIndex\tRealAnswer\tBlockX\tBlockY\tRoleAnswer\r\n"

	snprintf(pLogContent, MAX_DL_RECORD_MESSAGE_LEN, "%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n",
		get_time_str(pMsg->dwTime), g_ServerConfig.Common.nAreaID, pMsg->nGroupID, pMsg->dwRoleID, pMsg->dwJigsawSn, pMsg->byAction, pMsg->nErrCode, pMsg->nImageIndex, pMsg->nRealAnswer, pMsg->nRealAnswerBlockX, pMsg->nRealAnswerBlockY, pMsg->nRoleAnswer);

	nRetCode = m_LogData.AsyncWriteLog(dlmtAntiCheat, pLogContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	SAFE_DELETE_ARRAY(pLogContent);

	return;
}

