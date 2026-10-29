#include "stdafx.h"
#include "message_handler.h"

#include <time.h>

#include "time_ex.h"
#include "server_config.h"
#include "dump.h"

#include "t2m_message.h"
#include "m2t_message.h"
#include "g2m_message.h"
#include "l2m_message.h"
#include "d2m_message.h"
#include "common_message.h"
#include "c2m_message.h"
#include "b2m_message.h"
#include "o2m_message.h"
#include "gmsvr_message.h"
#include "ns_message.h"
#include "mm2m_message.h"
#include "m2mm_message.h"

#include "manageserver.h"
#include "ms_session_mgr.h"
#include "role_data.h"
#include "client/usermgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "scene/scenemgr.h"
#include "gs_info_mgr.h"
#include "scene_data.h"
#include "team/team.h"
#include "manageserver.h"
#include "queuematch/battlequeuemgr.h"
#include "guild/ms_guild.h"
#include "guild/ms_guild_member.h"
#include "./guild/ms_guild_mgr.h"
#include "statis_info.h"
#include "role_data_mgr.h"
#include "./gm/gm_manager.h"
#include "script_mgr.h"
#include "master/master_mgr.h"
#include "db/ms_sql_stmt_list.h"
#include "sql_mgr.h"
#include "fcm/fcm.h"
#include "gm_def.h"
#include "gm_svr_def.h"
#include "mail/mail_mgr.h"
#include "guild_config.h"
#include "exchange/exchange_mgr.h"

using namespace InternalMessage;
CMessageHandler CMessageHandler::ms_Instance;

#define WAIT_EXIT_DB_TIME   (60 * 1000)  // config like g_ServerConfig.Common.nServerPingTimeout?

static UINT32 s_dwGateSessionList[MAX_GT_SESSION_COUNT][MAX_CLIENT_SESSION_COUNT];

void CMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(pSocketSession);
    LOG_PROCESS_ERROR(CMSSessionMgr::GetInstance().GetServerState() != sgsClosing);

    if(pSocketAcceptor == m_poGTAcceptor)
    {
		SGTSession* poGTSession = CMSSessionMgr::GetInstance().NewGTSession(pSocketSession);
        LOG_PROCESS_ERROR(poGTSession);

        //poGTSession->pSocketSession = pSocketSession;
        pSocketSession->SetUserData(poGTSession);
        poGTSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
    }
    else if(pSocketAcceptor == m_poGSAcceptor)
    {
		SGSSession* poGSSession = CMSSessionMgr::GetInstance().NewGSSession(pSocketSession);
        LOG_PROCESS_ERROR(poGSSession);

        //poGSSession->pSocketSession = pSocketSession;
        pSocketSession->SetUserData(poGSSession);
        poGSSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

        INF("NewGSSession %d in OnAccepted", poGSSession->dwSessionID);
    }
    else if(pSocketAcceptor == m_poLSAcceptor)
    {
        SLSSession& rLSSession = CMSSessionMgr::GetInstance().GetLSSession();

        LOG_PROCESS_ERROR( NULL == rLSSession.pSocketSession );
        rLSSession.pSocketSession = pSocketSession;
        pSocketSession->SetUserData(&rLSSession);
        rLSSession.qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
    }
    else if (pSocketAcceptor == m_poDBAcceptor)
    {
        SDBSession& rDBSession = CMSSessionMgr::GetInstance().GetDBSession();

        LOG_PROCESS_ERROR( NULL == rDBSession.pSocketSession );
        rDBSession.pSocketSession = pSocketSession;
        pSocketSession->SetUserData(&rDBSession);
        rDBSession.qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
    }
	else if (pSocketAcceptor == m_poBSAcceptor)
	{
		SBSSession& rBSSession = CMSSessionMgr::GetInstance().GetBSSession();

        LOG_PROCESS_ERROR( NULL == rBSSession.pSocketSession );
		rBSSession.pSocketSession = pSocketSession;
		pSocketSession->SetUserData(&rBSSession);
		rBSSession.qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
	}
	else if(pSocketAcceptor == m_poDLAcceptor)
	{
		SDLSession& rDLSession = CMSSessionMgr::GetInstance().GetDLSession();

        LOG_PROCESS_ERROR( NULL == rDLSession.pSocketSession );
		rDLSession.pSocketSession = pSocketSession;
		pSocketSession->SetUserData(&rDLSession);
		rDLSession.qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
	}
	else if(pSocketAcceptor == m_poMonitorForRNAcceptor)
	{
		nRetCode = _ProcessMonitorForRoleNumber(pSocketSession);
		LOG_CHECK_ERROR(nRetCode);

        INF("close socket m_poMonitorForRNAcceptor");
		nRetCode = pSocketSession->Close();
		LOG_CHECK_ERROR(nRetCode);
	}
	else if (pSocketAcceptor == m_poMMAcceptor)
	{
		SMMSession& rMMSession = CMSSessionMgr::GetInstance().GetMMSession();

		LOG_PROCESS_ERROR(NULL == rMMSession.pSocketSession);
		rMMSession.pSocketSession = pSocketSession;
		pSocketSession->SetUserData(&rMMSession);
		rMMSession.qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
	}
    else
    {
        LOG_PROCESS_ERROR(FALSE);
    }

    return;
Exit0:
    if( pSocketSession )
    {
        pSocketSession->Close();
    }
    return;
}

void CMessageHandler::OnConnected(ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	SMSSession* poSession = NULL;

	LOG_PROCESS_ERROR(pSocketSession);

	poSession = (SMSSession*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(poSession);

	if (poSession->dwType == stGD)
	{
		SGDSession* pGDSession = (SGDSession*)poSession;
		pGDSession->bConnected = TRUE;
		pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nRegisterGuardTimeout;

		if (CMSSessionMgr::GetInstance().GetServerState() == sgsAllReady)
		{
			nRetCode = DoGDRegrister();
			LOG_CHECK_ERROR(nRetCode);
		}
	}
	else if (stGM == poSession->dwType)
	{
		SGMSession* pGMSession = (SGMSession*)poSession;

		pGMSession->bConnected = TRUE;
		pGMSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nRegisterGuardTimeout;

		if (CMSSessionMgr::GetInstance().GetServerState() == sgsAllReady)
		{
			nRetCode = DoGMRegister();
			LOG_CHECK_ERROR(nRetCode);
		}
	}
	else if (poSession->dwType == stCS)
	{
		poSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
	}
	else if (poSession->dwType == stNS)
	{
		SNSSession* pNSSession = (SNSSession*)poSession;
		pNSSession->bConnected = TRUE;
		pNSSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
		INF("NS Connected.");
		DoNSMessageVersion();
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

    return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();

    return;
}

void CMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
    SMSSession* pstMSSession = NULL;

    MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pPacketBuffer;
    LOG_PROCESS_ERROR(pHeader);

    g_ChangeStatisInfo(statInternalNetflowIncome, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketIncome, 1);

    LOG_PROCESS_ERROR(pSocketSession);
    LOG_PROCESS_ERROR(m_InternalMessageFuncList[pHeader->wMsg]);

#if defined(WIN32)
    __try
    {
        (this->*m_InternalMessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize, pSocketSession);
    }
    __except(DumpExceptionFilter(GetExceptionInformation()))
    {
		CRI("failed to process internal message id %d size %d", pHeader->wMsg, dwSize);
	}
#elif defined (__linux__)
    (this->*m_InternalMessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize, pSocketSession);
#endif 

    if(m_pPacketAllocator)
    {
        if (pPacketBuffer)
            m_pPacketAllocator->Free(pPacketBuffer);
    }

    return;
Exit0:
    if (pHeader)
        CRI("unknown internal message id %d size %d", pHeader->wMsg, dwSize);
    else
        CRI("unknown internal message with null pointer");
    if ( m_pPacketAllocator )
    {
        if (pPacketBuffer)
            m_pPacketAllocator->Free(pPacketBuffer);
    }

    if (pSocketSession)
    {
        CRI("close socket session when OnRecv error");
        pSocketSession->Close();
    }
    return;
}

void CMessageHandler::OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown)
{
    INT32 nRetCode = 0;
    SMSSession* pMSSession = NULL;

    LOG_PROCESS_ERROR(pSocketSession);

    pMSSession = (SMSSession*)pSocketSession->GetUserData();
    PROCESS_ERROR(pMSSession);

    switch (pMSSession->dwType)
    {
    case stGD:
        {
            SGDSession* pGDSession = (SGDSession*)pMSSession;
            pGDSession->pSocketSession = NULL;
            pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
            pGDSession->bConnected = false;
            CRI("GD disconnected");
            PROCESS_SUCCESS(TRUE);
        }
        break;
    case stGM:
        {
            SGMSession  *pGMSession = (SGMSession*)pMSSession;
            pGMSession->pSocketSession = NULL;
            pGMSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
            pGMSession->bConnected = FALSE;
            pGMSession->bRegistered = FALSE;

            CRI("GM disconnected!");
        }
        break;
	case stNS:
	{
		INF("NS disconnected");

		SNSSession* pNSSession = (SNSSession*)pMSSession;
		pNSSession->pSocketSession = NULL;
		pNSSession->qwTimer = 0;
		pNSSession->bConnected = FALSE;
		pNSSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
	}
		break;
    case stDB:
        {
            SDBSession* pDBSession = (SDBSession*)pMSSession;
            pDBSession->pSocketSession = NULL;
            pDBSession->qwTimer = (UINT64)(~0);

            m_ServerInfo.DB.byReady = false;
            m_ServerInfo.DB.dwAddr = 0;

            CRI("DB disconnected");

            if (CMSSessionMgr::GetInstance().GetServerState() != sgsClosing)
            {
                CManageServer::GetInstance().SetForceClose();
                CMSSessionMgr::GetInstance().SetServerState(sgsClosing);
            }
        }
        break;
    case stLS:
        {
            SLSSession* pLSSession = (SLSSession*)pMSSession;
            pLSSession->pSocketSession = NULL;
            pLSSession->qwTimer = (UINT64)(~0);

            m_ServerInfo.LS.byReady = false;
            m_ServerInfo.LS.dwAddr = 0;

            CRI("LS disconnected");
        }
        break;
	case stBS:
		{
			SBSSession* pBSSession = (SBSSession*)pMSSession;
			pBSSession->pSocketSession = NULL;
			pBSSession->qwTimer = (UINT64)(~0);

			m_ServerInfo.BS.byReady = false;
			m_ServerInfo.BS.dwAddr = 0;

			CRI("BS disconnected");
		}
		break;
	case stDL:
		{
			SDLSession* pDLSession = (SDLSession*)pMSSession;
			pMSSession->pSocketSession = NULL;
			pMSSession->qwTimer = (UINT64)(~0);

			m_ServerInfo.DL.byReady = false;
			m_ServerInfo.DL.dwAddr = 0;

			CRI("DL disconnected");
		}
		break;
	case stGT:
        {
            CKickAllUserOnGT KickAllUserOnGT;

            SGTSession* pGTSession = (SGTSession*)pMSSession;
            pMSSession->pSocketSession = NULL;
            pMSSession->qwTimer = (UINT64)(~0);
            
            m_ServerInfo.GT[pGTSession->dwSessionID].byReady = false;
            m_ServerInfo.GT[pGTSession->dwSessionID].dwAddr = 0;

            CRI("GT %d disconnected", pGTSession->dwSessionID);

            KickAllUserOnGT.m_dwGTSessionID = pGTSession->dwSessionID;
            CUserMgr::GetInstance().TraverseUser(KickAllUserOnGT);

            CMSSessionMgr::GetInstance().DelGTSession( pGTSession );
        }
        break;
    case stGS:
        {
            CKickAllRoleOnGS KickAllRoleOnGS;

            SGSSession* pGSSession = (SGSSession*)pMSSession;
            pMSSession->pSocketSession = NULL;
            pMSSession->qwTimer = (UINT64)(~0);
            pGSSession->nWorkload = 0;

            m_ServerInfo.GS[pGSSession->dwSessionID].byReady = false;
            m_ServerInfo.GS[pGSSession->dwSessionID].dwAddr = 0;

            KickAllRoleOnGS.m_dwGSSessionID = pGSSession->dwSessionID;
            COnlineRoleDataMgr::GetInstance().TraverseOnlineRole(KickAllRoleOnGS);            

            pGSSession->bIsExiting = TRUE;

            CMSSessionMgr::GetInstance().DelGSSession( pGSSession );

		CRI("GS %d disconnected", pGSSession->dwSessionID);
	}
		break;
	case stCS:
	{
		INF("CS disconnected");
	}
		break;
	case stMM:
	{
		SMMSession* pMMSession = (SMMSession*)pMSSession;
		pMMSession->pSocketSession = NULL;
		pMMSession->qwTimer = (UINT64)(~0);

		m_ServerInfo.MM.byReady = false;
		m_ServerInfo.MM.dwAddr = 0;

		CRI("MM disconnected");
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

    if (CMSSessionMgr::GetInstance().GetServerState() != sgsClosing)
    {
        nRetCode = DoServerInfo();
        LOG_CHECK_ERROR(nRetCode);
    }

Exit1:
    return;
Exit0:
    return;
}

CMessageHandler::CMessageHandler()
{
    m_pNet = NULL;
    m_pPacketAllocator = NULL;
    m_poDBAcceptor = NULL;
    m_poGSAcceptor = NULL;
    m_poGTAcceptor = NULL;
    m_poLSAcceptor = NULL;
	m_poBSAcceptor = NULL;
	m_poDLAcceptor = NULL;
	m_poMMAcceptor = NULL;
    m_poMonitorForRNAcceptor = NULL;
    memset(&m_ServerInfo, 0, sizeof(m_ServerInfo));
    memset(m_InternalMessageFuncList, 0, sizeof(m_InternalMessageFuncList));
    memset(m_ExternalMessageFuncList, 0, sizeof(m_ExternalMessageFuncList));
}

CMessageHandler::~CMessageHandler()
{
    UnInit();
}

BOOL CMessageHandler::Init(INet* pNet, IPacketAllocator* pPacketAllocator, ISocketAcceptor* poGTAcceptor,
                           ISocketAcceptor* poGSAcceptor, ISocketAcceptor* poLSAcceptor, ISocketAcceptor* poDBAcceptor, 
						   ISocketAcceptor* poBSAcceptor, ISocketAcceptor* poDLAcceptor, ISocketAcceptor* poMMAcceptor,
						   ISocketAcceptor* poMonitorForRoleNumber)
{
    LOG_PROCESS_ERROR(pNet);
    LOG_PROCESS_ERROR(pPacketAllocator);
    LOG_PROCESS_ERROR(poGTAcceptor);
    LOG_PROCESS_ERROR(poGSAcceptor);
    LOG_PROCESS_ERROR(poLSAcceptor);
    LOG_PROCESS_ERROR(poDBAcceptor);
	LOG_PROCESS_ERROR(poBSAcceptor);
	LOG_PROCESS_ERROR(poDLAcceptor);
	LOG_PROCESS_ERROR(poMMAcceptor);
	LOG_PROCESS_ERROR(poMonitorForRoleNumber);

    m_pNet = pNet;
    m_pPacketAllocator = pPacketAllocator;

    m_poGTAcceptor = poGTAcceptor;
    m_poGSAcceptor = poGSAcceptor;
    m_poLSAcceptor = poLSAcceptor;
    m_poDBAcceptor = poDBAcceptor;
    m_poBSAcceptor = poBSAcceptor;
	m_poDLAcceptor = poDLAcceptor;
	m_poMMAcceptor = poMMAcceptor;
	m_poMonitorForRNAcceptor = poMonitorForRoleNumber;

    memset(&m_ServerInfo, 0, sizeof(m_ServerInfo));
    m_ServerInfo.wMsg = m2a_server_info;

    //All
    //////////////////////////////////////////////////////////////////////////////////////
    m_InternalMessageFuncList[a2a_ping] = &CMessageHandler::OnAllPing;

    //GD
	m_InternalMessageFuncList[u2m_close] = &CMessageHandler::OnGDClose;
	m_InternalMessageFuncList[u2m_command] = &CMessageHandler::OnGDCommand;

    // GM
    m_InternalMessageFuncList[gm2m_register_ack] = &CMessageHandler::OnGMRegisterAck;
    m_InternalMessageFuncList[gm2m_clear_role_cache] = &CMessageHandler::OnGMClearRoleCache;
    m_InternalMessageFuncList[gm2m_unlock_role] = &CMessageHandler::OnGMUnlockRole;
    m_InternalMessageFuncList[gm2m_query_role_data] = &CMessageHandler::OnGMQueryRoleData;
    m_InternalMessageFuncList[gm2m_forbid_chat] = &CMessageHandler::OnGMForbidChat;
    m_InternalMessageFuncList[gm2m_freeze_role] = &CMessageHandler::OnGMFreezeRole;
    m_InternalMessageFuncList[gm2m_kick_role] = &CMessageHandler::OnGMKickRole;
    m_InternalMessageFuncList[gm2m_send_mail] = &CMessageHandler::OnGMSendMail;
    m_InternalMessageFuncList[gm2m_announce] = &CMessageHandler::OnGMAnnounce;
    m_InternalMessageFuncList[gm2m_get_announce_list] = &CMessageHandler::OnGMGetAnnounceList;
    m_InternalMessageFuncList[gm2m_del_announce] = &CMessageHandler::OnGMDelAnnounce;
    m_InternalMessageFuncList[gm2m_exe_gm_str] = &CMessageHandler::OnGMExeGMStr;
    m_InternalMessageFuncList[gm2m_del_mail] = &CMessageHandler::OnGMDelMail;
    m_InternalMessageFuncList[gm2m_check_role_freeze] = &CMessageHandler::OnGMCheckRoleFreeze;
    m_InternalMessageFuncList[gm2m_get_exchange_price] = &CMessageHandler::OnGMGetExchangePrice;
    m_InternalMessageFuncList[gm2m_get_exchange_price_detail] = &CMessageHandler::OnGMGetExchangePriceDetail;
    m_InternalMessageFuncList[gm2m_add_system_order] = &CMessageHandler::OnGMAddSystemOrder;
    m_InternalMessageFuncList[gm2m_cancel_system_order] = &CMessageHandler::OnGMCancelSystemOrder;
    m_InternalMessageFuncList[gm2m_get_exchange_limit] = &CMessageHandler::OnGMGetExchangeLimit;
    m_InternalMessageFuncList[gm2m_set_exchange_limit] = &CMessageHandler::OnGMSetExchangeLimit;

    //LS
    m_InternalMessageFuncList[l2m_register] = &CMessageHandler::OnLSRegister;
    m_InternalMessageFuncList[l2m_ready] = &CMessageHandler::OnLSReady;
    m_InternalMessageFuncList[l2m_close_ack] = &CMessageHandler::OnLSCloseAck;
    m_InternalMessageFuncList[l2m_login_query_req] = &CMessageHandler::OnLSLoginQueryReq;
    
	//DL
	m_InternalMessageFuncList[o2m_register] = &CMessageHandler::OnDLRegister;
	m_InternalMessageFuncList[o2m_ready] = &CMessageHandler::OnDLReady;
	m_InternalMessageFuncList[o2m_close_ack] = &CMessageHandler::OnDLCloseAck;

    //GT
    m_InternalMessageFuncList[t2m_register] = &CMessageHandler::OnGTRegister;
    m_InternalMessageFuncList[t2m_ready] = &CMessageHandler::OnGTReady;
    m_InternalMessageFuncList[t2m_close_ack] = &CMessageHandler::OnGTCloseAck;

    m_InternalMessageFuncList[t2m_enter_gate] = &CMessageHandler::OnGTEnterGate;
    m_InternalMessageFuncList[t2m_query_scene] = &CMessageHandler::OnGTQueryScene;
    m_InternalMessageFuncList[t2m_change_gs_ack] = &CMessageHandler::OnGTChangeGSAck;
    m_InternalMessageFuncList[t2m_return_select_role] = &CMessageHandler::OnGTReturnSelectRole;
    m_InternalMessageFuncList[t2m_client_shutdown] = &CMessageHandler::OnGTClientShutDown;

    m_InternalMessageFuncList[t2m_client_msg] = &CMessageHandler::OnGTClientMsg;
    m_InternalMessageFuncList[t2m_check_new_name] = &CMessageHandler::OnGTCheckNewName;

    //DB
    m_InternalMessageFuncList[d2m_register] = &CMessageHandler::OnDBRegister;
    m_InternalMessageFuncList[d2m_ready] = &CMessageHandler::OnDBReady;
    m_InternalMessageFuncList[d2m_close_ack] = &CMessageHandler::OnDBCloseAck;

    m_InternalMessageFuncList[d2m_create_role] = &CMessageHandler::OnDBCreateRole;
    m_InternalMessageFuncList[d2m_can_delete_role] = &CMessageHandler::OnDBCanDeleteRole;
    m_InternalMessageFuncList[d2m_delete_role] = &CMessageHandler::OnDBDeleteRole;
    m_InternalMessageFuncList[d2m_leave_db_ack] = &CMessageHandler::OnDBLeaveDBAck;
    m_InternalMessageFuncList[d2m_clear_role_cache_ack] = &CMessageHandler::OnDBClearRoleCacheAck;
    
	// MM
	m_InternalMessageFuncList[mm2m_register] = &CMessageHandler::OnMMRegister;
	m_InternalMessageFuncList[mm2m_ready] = &CMessageHandler::OnMMReady;
	m_InternalMessageFuncList[mm2m_close_ack] = &CMessageHandler::OnMMCloseAck;

    //GS
    m_InternalMessageFuncList[g2m_register] = &CMessageHandler::OnGSRegister;
    m_InternalMessageFuncList[g2m_ready] = &CMessageHandler::OnGSReady;
    m_InternalMessageFuncList[g2m_close_ack] = &CMessageHandler::OnGSCloseAck;

    m_InternalMessageFuncList[g2m_enter_gs] = &CMessageHandler::OnGSEnterGS;
    m_InternalMessageFuncList[g2m_leave_gs_ack] = &CMessageHandler::OnGSLeaveGSAck;
    m_InternalMessageFuncList[g2m_create_scene_ack] = &CMessageHandler::OnGSCreateSceneAck;
    m_InternalMessageFuncList[g2m_destroy_scene_ack] = &CMessageHandler::OnGSDestroySceneAck;
	m_InternalMessageFuncList[g2m_close_scene] = &CMessageHandler::OnGSCloseScene;

    m_InternalMessageFuncList[g2m_query_scene] = &CMessageHandler::OnGSQueryScene;
	m_InternalMessageFuncList[g2m_pre_enter_gs_ack] = &CMessageHandler::OnGSPreEnterGSAck;
    m_InternalMessageFuncList[g2m_team_sync_member] = &CMessageHandler::OnGSTeamSyncMember;
	m_InternalMessageFuncList[g2m_chat_message] = &CMessageHandler::OnGSChatMessage;
    m_InternalMessageFuncList[g2m_leave_team] = &CMessageHandler::OnGSLeaveTeam;
    m_InternalMessageFuncList[g2m_clear_team_dungeon] = &CMessageHandler::OnGSClearTeamDungeon;

    m_InternalMessageFuncList[g2m_publish_quest] = &CMessageHandler::OnGSPublishQuest;
	m_InternalMessageFuncList[g2m_update_role_info] = &CMessageHandler::OnGSUpdateRoleInfo;

    m_InternalMessageFuncList[g2m_mail_cost_check] = &CMessageHandler::OnGSMailCostCheck;
    m_InternalMessageFuncList[g2m_send_system_mail] = &CMessageHandler::OnGSSendSystemMail;
    m_InternalMessageFuncList[g2m_send_system_mail_with_simple_attach] = &CMessageHandler::OnGSSendSystemMailWithSimpleAttach;
    m_InternalMessageFuncList[g2m_send_system_mail_with_buffer_attach] = &CMessageHandler::OnGSSendSystemMailWithBufferAttach;
    m_InternalMessageFuncList[g2m_mail_get_attach] = &CMessageHandler::OnGSMailGetAttach;

    m_InternalMessageFuncList[g2m_call_lua_function] = &CMessageHandler::OnGSCallLuaFunction;
	m_InternalMessageFuncList[g2m_broadcast_group_client] = &CMessageHandler::OnGSBroadcastGroupClient;

    // master 
    m_InternalMessageFuncList[g2m_mst_submit_teacher_app_ack] = &CMessageHandler::OnGSSubmitTeacherAppAck;
    m_InternalMessageFuncList[g2m_mst_agree_teacher_app_ack] = &CMessageHandler::OnGSAgreeTeacherAppAck;

    m_InternalMessageFuncList[g2m_mst_submit_stud_app_ack] = &CMessageHandler::OnGSSubmitStudAppAck;
    m_InternalMessageFuncList[g2m_mst_agree_stud_app_ack] = &CMessageHandler::OnGSAgreeStudAppAck;
    m_InternalMessageFuncList[g2m_sync_master_exp] = &CMessageHandler::OnGSSyncMasterExp;
    m_InternalMessageFuncList[g2m_master_cz] = &CMessageHandler::OnGSMasterCZ;
    m_InternalMessageFuncList[g2m_try_to_find_teacher] = &CMessageHandler::OnGSTryToFindTeacher;
    m_InternalMessageFuncList[g2m_master_twitter] = &CMessageHandler::OnGSMasterTwitter;

    // guild
    m_InternalMessageFuncList[g2m_guild_create] = &CMessageHandler::OnGSGuildCreate;
    m_InternalMessageFuncList[g2m_pre_create_guild_ack] = &CMessageHandler::OnGSPreCreateGuildAck;
    m_InternalMessageFuncList[g2m_guild_draw_salary] = &CMessageHandler::OnGSGuildDrawSalary;
    m_InternalMessageFuncList[g2m_guild_contribute] = &CMessageHandler::OnGSGuildContribute;
    m_InternalMessageFuncList[g2m_guild_view_storage] = &CMessageHandler::OnGSGuildViewStorage;
    m_InternalMessageFuncList[g2m_guild_pre_push_item] = &CMessageHandler::OnGSGuildPrePushItem;
    m_InternalMessageFuncList[g2m_guild_push_item] = &CMessageHandler::OnGSGuildPushItem;
    m_InternalMessageFuncList[g2m_guild_pre_pop_item] = &CMessageHandler::OnGSGuildPrePopItem;
    m_InternalMessageFuncList[g2m_guild_pop_item] = &CMessageHandler::OnGSGuildPopItem;
    m_InternalMessageFuncList[g2m_guild_create_building] = &CMessageHandler::OnGSGuildCreateBuilding;
    m_InternalMessageFuncList[g2m_guild_upgrade_building] = &CMessageHandler::OnGSGuildUpgradeBuilding;
    m_InternalMessageFuncList[g2m_guild_del_building] = &CMessageHandler::OnGSGuildDelBuilding;
    m_InternalMessageFuncList[g2m_guild_pre_open_dungeon] = &CMessageHandler::OnGSGuildPreOpenDungeon;
    m_InternalMessageFuncList[g2m_guild_open_dungeon] = &CMessageHandler::OnGSGuildOpenDungeon;
    m_InternalMessageFuncList[g2m_guild_enter_dungeon]  = &CMessageHandler::OnGSGuildEnterDungeon;
    m_InternalMessageFuncList[g2m_guild_close_dungeon_commission] = &CMessageHandler::OnGSGuildCloseDungeonCommission;
    m_InternalMessageFuncList[g2m_guild_change_res] = &CMessageHandler::OnGSGuildChangeRes;
    m_InternalMessageFuncList[g2m_guild_end_war] = &CMessageHandler::OnGSGuildEndWar;
    m_InternalMessageFuncList[g2m_guild_change_building_state] = &CMessageHandler::OnGSGuildChangeBuildingState;
    m_InternalMessageFuncList[g2m_guild_achievement_reward] = &CMessageHandler::OnGSGuildAchievementReward;
    m_InternalMessageFuncList[g2m_guild_leave_guild] = &CMessageHandler::OnGSGuildLeaveGuild;

    // arena
    m_InternalMessageFuncList[g2m_arena_sync_data] = &CMessageHandler::OnGSArenaSyncData;
    m_InternalMessageFuncList[g2m_arena_join_season] = &CMessageHandler::OnGSArenaJoinSeason;
    m_InternalMessageFuncList[g2m_arena_match] = &CMessageHandler::OnGSArenaMatch;
    m_InternalMessageFuncList[g2m_arena_win] = &CMessageHandler::OnGSArenaWin;
    m_InternalMessageFuncList[g2m_arena_defeated] = &CMessageHandler::OnGSArenaDefeated;
    m_InternalMessageFuncList[g2m_arena_reward] = &CMessageHandler::OnGSArenaReward;

    m_InternalMessageFuncList[g2m_add_kill_list] = &CMessageHandler::OnGSAddKillList;

    m_InternalMessageFuncList[g2m_gm_log] = &CMessageHandler::OnGSGMLog;

    // drop limit
    m_InternalMessageFuncList[g2m_drop_limit] = &CMessageHandler::OnGSDropLimit;

    // Auction exchange
    m_InternalMessageFuncList[g2m_auction_sell] = &CMessageHandler::OnGSAuctionSell;
    m_InternalMessageFuncList[g2m_auction_buy] = &CMessageHandler::OnGSAuctionBuy;
    m_InternalMessageFuncList[g2m_auction_pay] = &CMessageHandler::OnGSAuctionPay;

    // exchange
    m_InternalMessageFuncList[g2m_exchange_add_order] = &CMessageHandler::OnGSExchangeAddOrder;

	// ranklist
    m_InternalMessageFuncList[g2m_ranklist_role_data] = &CMessageHandler::OnGSUpdateRankListData;
	m_InternalMessageFuncList[g2m_update_ranklist_node] = &CMessageHandler::OnGSUpdateRankListNode;

    // adventure
    m_InternalMessageFuncList[g2m_adventure_trigger] = &CMessageHandler::OnGSAdventureTrigger;

	// award
    m_InternalMessageFuncList[g2m_send_award_to_ump] = &CMessageHandler::OnGSSendAwardToUmp;

    m_InternalMessageFuncList[g2m_query_role_location] = &CMessageHandler::OnGSQueryRoleLocation;
    m_InternalMessageFuncList[g2m_query_role_location_ack] = &CMessageHandler::OnGSQueryRoleLocationAck;
    m_InternalMessageFuncList[g2m_take_role] = &CMessageHandler::OnGSTakeRole;
	m_InternalMessageFuncList[g2m_recall_team_member] = &CMessageHandler::OnGSRecallTeamMember;

	// sabac
	m_InternalMessageFuncList[g2m_enroll_sabac] = &CMessageHandler::OnGSEnrollSabac;
	m_InternalMessageFuncList[g2m_occupy_sabac] = &CMessageHandler::OnGSOccupySabac;
	m_InternalMessageFuncList[g2m_erect_statue] = &CMessageHandler::OnGSErectStatue;

	m_InternalMessageFuncList[g2m_set_server_variable] = &CMessageHandler::OnGSSetServerVariable;

    m_InternalMessageFuncList[g2m_get_black_market_limit_info] = &CMessageHandler::OnGSGetBlackMarketLimitInfo;
    m_InternalMessageFuncList[g2m_check_black_market_product] = &CMessageHandler::OnGSCheckBlackMarketProduct;

    m_InternalMessageFuncList[g2m_add_exercise_time] = &CMessageHandler::OnGSAddExerciseTime;

    m_InternalMessageFuncList[g2m_freeze_login] = &CMessageHandler::OnGSFreezeLogin;

    m_InternalMessageFuncList[g2m_temporary_charge] = &CMessageHandler::OnGSTemporaryCharge;

    // Player queries ingots
    m_InternalMessageFuncList[g2m_query_real_money_ack] = &CMessageHandler::OnGSQueryRealMoneyAck;
    // Player purchases cash-shop items
    m_InternalMessageFuncList[g2m_role_rela_money_shop_buy_item_ack] = &CMessageHandler::OnGSRoleRealMoneyShopBuyItemAck;


	//BS
    m_InternalMessageFuncList[b2m_register] = &CMessageHandler::OnBSRegister;
    m_InternalMessageFuncList[b2m_close_ack] = &CMessageHandler::OnBSCloseAck;
	m_InternalMessageFuncList[b2m_ready] = &CMessageHandler::OnBSReady;

    // ns
    m_InternalMessageFuncList[n2x_new_name_result] = &CMessageHandler::OnNSNewNameResult;

    //external
    /////////////////////////////////////////////////////////////////////////////////////////////
    m_ExternalMessageFuncList[ExternalMessage::c2m_query_role_info] = &CMessageHandler::OnClientQueryRoleInfo;

    m_ExternalMessageFuncList[ExternalMessage::c2m_team_join] = &CMessageHandler::OnClientTeamJoin;
    m_ExternalMessageFuncList[ExternalMessage::c2m_team_leave] = &CMessageHandler::OnClientTeamLeave;
    m_ExternalMessageFuncList[ExternalMessage::c2m_team_answer] = &CMessageHandler::OnClientTeamAnswer;
    m_ExternalMessageFuncList[ExternalMessage::c2m_team_changeinfo] = &CMessageHandler::OnClientTeamChangeInfo;

    m_ExternalMessageFuncList[ExternalMessage::c2m_team_change_activity_info] = &CMessageHandler::OnClientActivityTeamChangeDungeon;
    m_ExternalMessageFuncList[ExternalMessage::c2m_team_query_activity_team] = &CMessageHandler::OnClientActivityTeamQueryTeam;
    m_ExternalMessageFuncList[ExternalMessage::c2m_team_remove_activity_team] = &CMessageHandler::OnClientActivityTeamRemove;

    m_ExternalMessageFuncList[ExternalMessage::c2m_friend_add] = &CMessageHandler::OnClientFriendAdd;
    m_ExternalMessageFuncList[ExternalMessage::c2m_friend_del] = &CMessageHandler::OnClientFriendDel;
    m_ExternalMessageFuncList[ExternalMessage::c2m_friend_change_group] = &CMessageHandler::OnClientFriendChangeGroup;
    m_ExternalMessageFuncList[ExternalMessage::c2m_change_info] = &CMessageHandler::OnClientChangeInfo;
    m_ExternalMessageFuncList[ExternalMessage::c2m_friend_chat] = &CMessageHandler::OnClientFriendChat;
    m_ExternalMessageFuncList[ExternalMessage::c2m_personal_enemy_add] = &CMessageHandler::OnClientPersonalEnemyAdd;
    m_ExternalMessageFuncList[ExternalMessage::c2m_personal_enemy_del] = &CMessageHandler::OnClientPersonalEnemyDel;

    m_ExternalMessageFuncList[ExternalMessage::c2m_chat_message] = &CMessageHandler::OnClientChatMessage;

	m_ExternalMessageFuncList[ExternalMessage::c2m_query_line_req] = &CMessageHandler::OnClientQueryLineReq;

	//m_ExternalMessageFuncList[ExternalMessage::c2m_join_battle] = &CMessageHandler::OnClientJoinBattle;
	//m_ExternalMessageFuncList[ExternalMessage::c2m_cancel_join_battle_req] = &CMessageHandler::OnClientCancelJoinBattleReq;
	//m_ExternalMessageFuncList[ExternalMessage::c2m_join_battle_confirm_ack] = &CMessageHandler::OnClientJoinBattleConfirm;

    m_ExternalMessageFuncList[ExternalMessage::c2m_master_teacher_ignore] = &CMessageHandler::OnClientMstTeacherIgnore;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_get_recruit_list] = &CMessageHandler::OnClientMstGetRecruitList;
    //m_ExternalMessageFuncList[ExternalMessage::c2m_master_apply_recruit] = &CMessageHandler::OnClientMstApplyRecruit;
    //m_ExternalMessageFuncList[ExternalMessage::c2m_master_cancel_recruit] = &CMessageHandler::OnClientMstCancelRecruit;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_get_solo_list] = &CMessageHandler::OnClientMstGetSoloList;

    m_ExternalMessageFuncList[ExternalMessage::c2m_master_stud_submit_app] = &CMessageHandler::OnClientMstStudSubmitApp;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_agree_stud_app] = &CMessageHandler::OnClientMstAgreeStudApp;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_refuse_stud_app] = &CMessageHandler::OnClientMstRefuseStudApp;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_cancel_stud_app] = &CMessageHandler::OnClientMstCancelStudApp;

    m_ExternalMessageFuncList[ExternalMessage::c2m_master_teacher_submit_app] = &CMessageHandler::OnClientMstTeacherSubmitApp;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_agree_teacher_app] = &CMessageHandler::OnClientMstAgreeTeacherApp;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_refuse_teacher_app] = &CMessageHandler::OnClientMstRefuseTeacherApp;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_cancel_teacher_app] = &CMessageHandler::OnClientMstCancelTeacherApp;

    m_ExternalMessageFuncList[ExternalMessage::c2m_master_leave_teacher] = &CMessageHandler::OnClientMstLeaveTeacher;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_fire_stud] = &CMessageHandler::OnClientMstFireStud;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_info] = &CMessageHandler::OnClientMstInfo;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_cz] = &CMessageHandler::OnClientMstCZ;
    m_ExternalMessageFuncList[ExternalMessage::c2m_master_grad] = &CMessageHandler::OnClientMstGrad;

    //m_ExternalMessageFuncList[ExternalMessage::c2m_master_get_master_info] = &CMessageHandler::OnClientMstGetMasterInfo;
    //m_ExternalMessageFuncList[ExternalMessage::c2m_master_cong_level_up] = &CMessageHandler::OnClientMstCongUpLevel;

    m_ExternalMessageFuncList[ExternalMessage::c2m_mail_write] = &CMessageHandler::OnClientMailWrite;
    m_ExternalMessageFuncList[ExternalMessage::c2m_mail_list_recv] = &CMessageHandler::OnClientMailListRecv;
    m_ExternalMessageFuncList[ExternalMessage::c2m_mail_read] = &CMessageHandler::OnClientMailRead;
    m_ExternalMessageFuncList[ExternalMessage::c2m_mail_delete] = &CMessageHandler::OnClientMailDelete;
    m_ExternalMessageFuncList[ExternalMessage::c2m_mail_get_attach] = &CMessageHandler::OnClientMailGetAttach;

    m_ExternalMessageFuncList[ExternalMessage::c2m_query_guild_name] = &CMessageHandler::OnClientGuildQueryName;
    m_ExternalMessageFuncList[ExternalMessage::c2m_view_guild_list] = &CMessageHandler::OnClientViewGuildList;
    m_ExternalMessageFuncList[ExternalMessage::c2m_more_guild_info] = &CMessageHandler::OnClientMoreGuildInfo;
    m_ExternalMessageFuncList[ExternalMessage::c2m_find_guild] = &CMessageHandler::OnClientFindGuild;
    m_ExternalMessageFuncList[ExternalMessage::c2m_apply_to_join_guild] = &CMessageHandler::OnClientApplyToJoinGuild;
    m_ExternalMessageFuncList[ExternalMessage::c2m_get_apply_guild_list] = &CMessageHandler::OnClientGetApplyGuildList;
    m_ExternalMessageFuncList[ExternalMessage::c2m_revert_guild_apply] = &CMessageHandler::OnClientRevertGuildApply;
    m_ExternalMessageFuncList[ExternalMessage::c2m_invite_to_join_guild] = &CMessageHandler::OnClientInviteToJoinGuild;
    m_ExternalMessageFuncList[ExternalMessage::c2m_revert_guild_invitation] = &CMessageHandler::OnClientRevertGuildInvitation;
    m_ExternalMessageFuncList[ExternalMessage::c2m_disband_guild] = &CMessageHandler::OnClientDisbandGuild;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_notice_change] = &CMessageHandler::OnClientGuildNoticeChange;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_announce_change] = &CMessageHandler::OnClientGuildAnnounceChange;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_forbid_chat] = &CMessageHandler::OnClientGuildForbidChat;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_change_position] = &CMessageHandler::OnClientGuildChangePosition;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_role_leave] = &CMessageHandler::OnClientGuildLeave;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_banish_role] = &CMessageHandler::OnClientGuildBanishRole;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_demise] = &CMessageHandler::OnClientGuildDemise;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_diplomat_app] = &CMessageHandler::OnClientGuildDiplomatApp;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_cancel_alliance] = &CMessageHandler::OnClientCancelAlliance;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_get_apply_union_list] = &CMessageHandler::OnClientGetApplyUnion;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_reply_union_app] = &CMessageHandler::OnClientReplyUnionApp;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_change_storage_right] = &CMessageHandler::OnClientChangeStorageRight;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_move_item] = &CMessageHandler::OnClientGuildMoveItem;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_more_history] = &CMessageHandler::OnClientGuildMoreHistory;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_more_chest] = &CMessageHandler::OnClientGuildMoreChest;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_upgrade_welfare] = &CMessageHandler::OnClientGuildUpgradeWelfare;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_cancel_welfare_upgrade] = &CMessageHandler::OnClientGuildCancelWelfareUpgrade;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_cancel_building_upgrade] = &CMessageHandler::OnClientGuildCancelBuildingUpgrade;
    //m_ExternalMessageFuncList[ExternalMessage::c2m_guild_del_building] = &CMessageHandler::OnClientGuildDelBuilding;
    //m_ExternalMessageFuncList[ExternalMessage::c2m_guild_get_dungeon_list] = &CMessageHandler::OnClientGuildGetDungeonList;
    //m_ExternalMessageFuncList[ExternalMessage::c2m_guild_declare_war] = &CMessageHandler::OnClientGuildDeclareWar;            // TODO: disable guild wars
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_achievement] = &CMessageHandler::OnClientGuildAchievement;
    m_ExternalMessageFuncList[ExternalMessage::c2m_guild_activity] = &CMessageHandler::OnClientGuildActivity;

    //m_ExternalMessageFuncList[ExternalMessage::c2m_arena_get_data] = &CMessageHandler::OnClientArenaGetData;
    m_ExternalMessageFuncList[ExternalMessage::c2m_shield] = &CMessageHandler::OnClientShield;

	m_ExternalMessageFuncList[ExternalMessage::c2m_auction_search_type] = &CMessageHandler::OnClientAuctionSearchType;
    m_ExternalMessageFuncList[ExternalMessage::c2m_auction_search] = &CMessageHandler::OnClientAuctionSearch;
    m_ExternalMessageFuncList[ExternalMessage::c2m_auction_query] = &CMessageHandler::OnClientAuctionQuery;
    m_ExternalMessageFuncList[ExternalMessage::c2m_auction_sell] = &CMessageHandler::OnClientAuctionSell;
    m_ExternalMessageFuncList[ExternalMessage::c2m_auction_cancel_sell] = &CMessageHandler::OnClientAuctionCancelSell;
    m_ExternalMessageFuncList[ExternalMessage::c2m_auction_buy] = &CMessageHandler::OnClientAuctionBuy;
    m_ExternalMessageFuncList[ExternalMessage::c2m_auction_list_my_selling] = &CMessageHandler::OnClientAuctionListMySelling;
    m_ExternalMessageFuncList[ExternalMessage::c2m_auction_list_my_buying] = &CMessageHandler::OnClientAuctionListMyBuying;
    m_ExternalMessageFuncList[ExternalMessage::c2m_auction_pay_left_money] = &CMessageHandler::OnClientAuctionPayLeftMoney;
    
	m_ExternalMessageFuncList[ExternalMessage::c2m_exchange_add_order] = &CMessageHandler::OnClientExchangeAddOrder;
    m_ExternalMessageFuncList[ExternalMessage::c2m_exchange_cancel_order] = &CMessageHandler::OnClientExchangeCancelOrder;
    m_ExternalMessageFuncList[ExternalMessage::c2m_exchange_list_prices] = &CMessageHandler::OnClientExchangeListPrices;
    m_ExternalMessageFuncList[ExternalMessage::c2m_exchange_list_my_orders] = &CMessageHandler::OnClientExchangeListMyOrders;
    m_ExternalMessageFuncList[ExternalMessage::c2m_exchange_list_my_logs] = &CMessageHandler::OnClientExchangeListMyLogs;

	m_ExternalMessageFuncList[ExternalMessage::c2m_check_real_money] = &CMessageHandler::OnClientCheckRealMoney;
    m_ExternalMessageFuncList[ExternalMessage::c2m_check_real_money_shop_data] = &CMessageHandler::OnClientCheckRealMoneyShopData;
    m_ExternalMessageFuncList[ExternalMessage::c2m_real_money_buy] = &CMessageHandler::OnClientRealMoneyBuy;
    m_ExternalMessageFuncList[ExternalMessage::c2m_real_money_shop_log] = &CMessageHandler::OnClientRealMoneyShopLog;
    m_ExternalMessageFuncList[ExternalMessage::c2m_sale_info] = &CMessageHandler::OnClientSaleInfo;

    m_ExternalMessageFuncList[ExternalMessage::c2m_gm_command] = &CMessageHandler::OnClientGMCommand;
    m_ExternalMessageFuncList[ExternalMessage::c2m_query_ranklist] = &CMessageHandler::OnClientQueryRanklist;
    m_ExternalMessageFuncList[ExternalMessage::c2m_award_use_number] = &CMessageHandler::OnClientAwardUseNumber;
    m_ExternalMessageFuncList[ExternalMessage::c2m_buy_time] = &CMessageHandler::OnClientBuyTime;

	m_ExternalMessageFuncList[ExternalMessage::c2m_change_recall_state] = &CMessageHandler::OnClientChangeRecallState;

    m_ExternalMessageFuncList[ExternalMessage::c2m_dynamic_auth] = &CMessageHandler::OnDynamicAuth;

    m_ExternalMessageFuncList[ExternalMessage::c2m_team_query] = &CMessageHandler::OnClientTeamQuery;
    m_ExternalMessageFuncList[ExternalMessage::c2m_team_invite] = &CMessageHandler::OnClientTeamInvite;

    m_ExternalMessageFuncList[ExternalMessage::c2m_arena_fund] = &CMessageHandler::OnArenaFund;


	/////////////////////////////////////////////////////////////////////////////////////////////
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::UnInit(void)
{
    m_pNet = NULL;
    m_pPacketAllocator = NULL;

    m_poGTAcceptor = NULL;
    m_poGSAcceptor = NULL;
    m_poLSAcceptor = NULL;
    m_poDBAcceptor = NULL;
    m_poBSAcceptor = NULL;
    m_poDLAcceptor = NULL;
	m_poMMAcceptor = NULL;
    m_poMonitorForRNAcceptor = NULL;

    return TRUE;
}

CMessageHandler& CMessageHandler::GetInstance(void)
{
    return ms_Instance;
}

BOOL CMessageHandler::_SendGD(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    SGDSession& rGDSession = CMSSessionMgr::GetInstance().GetGDSession();
    LOG_PROCESS_ERROR(rGDSession.pSocketSession);

    nRetCode = rGDSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

    return TRUE;
Exit0:
    return FALSE;

}

BOOL CMessageHandler::_SendNS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    SNSSession& rNSSession = CMSSessionMgr::GetInstance().GetNSSession();
    LOG_PROCESS_ERROR(rNSSession.pSocketSession);

    nRetCode = rNSSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

    return TRUE;
Exit0:
    return FALSE;

}


BOOL CMessageHandler::_SendGT(UINT32 dwGTSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    SGTSession* pGTSession = CMSSessionMgr::GetInstance().GetGTSession(dwGTSessionID);
    LOG_PROCESS_ERROR(pGTSession);
    LOG_PROCESS_ERROR(pGTSession->pSocketSession);

    nRetCode = pGTSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_SendGS(UINT32 dwGSSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    SGSSession* pGSSession = CMSSessionMgr::GetInstance().GetGSSession(dwGSSessionID);
    LOG_PROCESS_ERROR(pGSSession);
    LOG_PROCESS_ERROR(pGSSession->pSocketSession);

    nRetCode = pGSSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_SendLS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;

	SLSSession& rLSSession = CMSSessionMgr::GetInstance().GetLSSession();
	LOG_PROCESS_ERROR(rLSSession.pSocketSession);

	nRetCode = rLSSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMessageHandler::_SendDL(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

	if(g_ServerConfig.MS.nInitDLCount > 0)
	{
		SDLSession& rDLSession = CMSSessionMgr::GetInstance().GetDLSession();
		LOG_PROCESS_ERROR(rDLSession.pSocketSession);

		nRetCode = rDLSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
		LOG_PROCESS_ERROR(nRetCode == dwSize);

        g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
        g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
    }
	else
	{
		if(pPacketBuffer)
			m_pPacketAllocator->Free(pPacketBuffer);
	}

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_SendDB(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    SDBSession& rDBSession = CMSSessionMgr::GetInstance().GetDBSession();
    LOG_PROCESS_ERROR(rDBSession.pSocketSession);

    nRetCode = rDBSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_SendBS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;
	SBSSession& rBSSession = CMSSessionMgr::GetInstance().GetBSSession();
	LOG_PROCESS_ERROR(rBSSession.pSocketSession);

	nRetCode = rBSSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMessageHandler::_SendClient(UINT32 dwClientSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    M2T_CLIENT_MSG* pMsg = (M2T_CLIENT_MSG*)((char*)pPacketBuffer - sizeof(M2T_CLIENT_MSG));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2t_client_msg;
    pMsg->dwSessionID = dwClientSessionID;

    nRetCode = _SendGT(GET_GT_INDEX_FROM_SESSION_ID(dwClientSessionID), pPacketBuffer, dwSize + sizeof(M2T_CLIENT_MSG), -(INT32)sizeof(M2T_CLIENT_MSG));
    LOG_PROCESS_ERROR(nRetCode);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_SendGM(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    SGMSession& rGMSession = CMSSessionMgr::GetInstance().GetGMSession();
    LOG_PROCESS_ERROR(rGMSession.pSocketSession);

    nRetCode = rGMSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
    g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_SendMM(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;
	SMMSession& rMMSession = CMSSessionMgr::GetInstance().GetMMSession();
	LOG_PROCESS_ERROR(rMMSession.pSocketSession);

	nRetCode = rMMSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
	g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMessageHandler::_ProcessMonitorForRoleNumber(ISocketSession* pSocketSession)
{
	INT32 nSize = 0;
	INT32 nRetCode = 0;
	PACKET_BUFFER* poBuffer = m_pPacketAllocator->Alloc(SMALL_PACKET_SIZE);
	LOG_PROCESS_ERROR(poBuffer);
	memset(poBuffer, 0, SMALL_PACKET_SIZE);

	for(INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
	{
        if (IsValidGroup(i))
		{
			char szTemp[32]={0};
			INT32 nUserCount = CUserMgr::GetInstance().GetCountPerGroup(i);
			snprintf(szTemp, 32, "area_%d\\group_%d\\%d;", g_ServerConfig.Common.nAreaID, i, nUserCount);
			strxcat((char*)poBuffer, szTemp, SMALL_PACKET_SIZE);
		}
	}

	nSize = strlen((char*)poBuffer) + 1;
	LOG_PROCESS_ERROR(nSize > 0);

	nRetCode = pSocketSession->SendNoPackageHead(poBuffer, nSize);
	LOG_PROCESS_ERROR(nRetCode);
	
	return TRUE;

Exit0:

	if(poBuffer)
		m_pPacketAllocator->Free(poBuffer);

	return FALSE;
}

BOOL CMessageHandler::_BroadcastGT(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;
    PACKET_BUFFER* pMsg = NULL;

    for(INT32 i = 1; i < MAX_GT_SESSION_COUNT; ++i)
    {
        SGTSession* pGTSession = CMSSessionMgr::GetInstance().GetGTSession(i);
        LOG_PROCESS_ERROR(pGTSession);

		if (pGTSession->pSocketSession == NULL)
			continue;

        pMsg = m_pPacketAllocator->Alloc(dwSize);
        LOG_PROCESS_ERROR(pMsg);

        memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

        nRetCode = pGTSession->pSocketSession->Send(pMsg, dwSize, nOffset);
		if (nRetCode == dwSize)
		{
			g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
			g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
		}
		else
		{
			LOG_CHECK_ERROR(nRetCode == dwSize);
			m_pPacketAllocator->Free(pMsg);
		}

        pMsg = NULL;
    }

    m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    if(pMsg)
        m_pPacketAllocator->Free(pMsg);
    if(pPacketBuffer)
        m_pPacketAllocator->Free(pPacketBuffer);
    return FALSE;
}

BOOL CMessageHandler::_BroadcastGS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;
    PACKET_BUFFER* pMsg = NULL;

    for(INT32 i = 1; i < MAX_GS_SESSION_COUNT; ++i)
    {
        SGSSession* pGSSession = CMSSessionMgr::GetInstance().GetGSSession(i);
        LOG_PROCESS_ERROR(pGSSession);

        if(!pGSSession->pSocketSession)
            continue;

        pMsg = m_pPacketAllocator->Alloc(dwSize);
        LOG_PROCESS_ERROR(pMsg);

        memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

        nRetCode = pGSSession->pSocketSession->Send(pMsg, dwSize, nOffset);
		if (nRetCode == dwSize)
		{
			g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
			g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
		}
		else
		{
			LOG_CHECK_ERROR(nRetCode == dwSize);
			m_pPacketAllocator->Free(pMsg);
		}

        pMsg = NULL;
    }

    m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    if(pMsg)
        m_pPacketAllocator->Free(pMsg);
    if(pPacketBuffer)
        m_pPacketAllocator->Free(pPacketBuffer);
    return FALSE;
}

BOOL CMessageHandler::_BroadcastAll(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;
    PACKET_BUFFER* pMsg = NULL;

    for(INT32 i = 1; i < MAX_GT_SESSION_COUNT; ++i)
    {
        SGTSession* pGTSession = CMSSessionMgr::GetInstance().GetGTSession(i);
        LOG_PROCESS_ERROR(pGTSession);

        if (pGTSession->pSocketSession == NULL)
            continue;

        pMsg = m_pPacketAllocator->Alloc(dwSize);
        LOG_PROCESS_ERROR(pMsg);

        memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

        nRetCode = pGTSession->pSocketSession->Send(pMsg, dwSize, nOffset);
		if (nRetCode == dwSize)
		{
			g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
			g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
		}
		else
		{
			LOG_CHECK_ERROR(nRetCode == dwSize);
			m_pPacketAllocator->Free(pMsg);
		}

        pMsg = NULL;
    }

    for(INT32 i = 1; i < MAX_GS_SESSION_COUNT; ++i)
    {
        SGSSession* pGSSession = CMSSessionMgr::GetInstance().GetGSSession(i);
        LOG_PROCESS_ERROR(pGSSession);
        
        if (pGSSession->pSocketSession == NULL)
            continue;

        pMsg = m_pPacketAllocator->Alloc(dwSize);
        LOG_PROCESS_ERROR(pMsg);

        memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

        nRetCode = pGSSession->pSocketSession->Send(pMsg, dwSize, nOffset);
		if (nRetCode == dwSize)
		{
			g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
			g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
		}
		else
		{
			LOG_CHECK_ERROR(nRetCode == dwSize);
			m_pPacketAllocator->Free(pMsg);
		}

        pMsg = NULL;
    }

    {
        SDBSession& rDBSession = CMSSessionMgr::GetInstance().GetDBSession();
        if (rDBSession.pSocketSession)
        {
            pMsg = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pMsg);

            memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = rDBSession.pSocketSession->Send(pMsg, dwSize, nOffset);
			if (nRetCode == dwSize)
			{
				g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
				g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
			}
			else
			{
				LOG_CHECK_ERROR(nRetCode == dwSize);
				m_pPacketAllocator->Free(pMsg);
			}

            pMsg = NULL;
        }
    }

    {
        SLSSession& rLSSession = CMSSessionMgr::GetInstance().GetLSSession();
        if (rLSSession.pSocketSession)
        {
            pMsg = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pMsg);

            memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = rLSSession.pSocketSession->Send(pMsg, dwSize, nOffset);
			if (nRetCode == dwSize)
			{
				g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
				g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
			}
			else
			{
				LOG_CHECK_ERROR(nRetCode == dwSize);
				m_pPacketAllocator->Free(pMsg);
			}

            pMsg = NULL;
        }
    }

	{
		SBSSession& rBSSession = CMSSessionMgr::GetInstance().GetBSSession();
		if (rBSSession.pSocketSession)
		{
			pMsg = m_pPacketAllocator->Alloc(dwSize);
			LOG_PROCESS_ERROR(pMsg);

			memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

			nRetCode = rBSSession.pSocketSession->Send(pMsg, dwSize, nOffset);
			if (nRetCode == dwSize)
			{
				g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
				g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
			}
			else
			{
				LOG_CHECK_ERROR(nRetCode == dwSize);
				m_pPacketAllocator->Free(pMsg);
			}

            pMsg = NULL;
		}
	}

	{
		SDLSession& rDLSession = CMSSessionMgr::GetInstance().GetDLSession();
		if (rDLSession.pSocketSession)
		{
			pMsg = m_pPacketAllocator->Alloc(dwSize);
			LOG_PROCESS_ERROR(pMsg);

			memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

			nRetCode = rDLSession.pSocketSession->Send(pMsg, dwSize, nOffset);
			if (nRetCode == dwSize)
			{
				g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
				g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
			}
			else
			{
				LOG_CHECK_ERROR(nRetCode == dwSize);
				m_pPacketAllocator->Free(pMsg);
			}

            pMsg = NULL;
		}
	}

	{
		SMMSession& rMMSession = CMSSessionMgr::GetInstance().GetMMSession();
		if (rMMSession.pSocketSession)
		{
			pMsg = m_pPacketAllocator->Alloc(dwSize);
			LOG_PROCESS_ERROR(pMsg);

			memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

			nRetCode = rMMSession.pSocketSession->Send(pMsg, dwSize, nOffset);
			if (nRetCode == dwSize)
			{
				g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
				g_ChangeStatisInfo(statInternalPacketOutgoing, 1);
			}
			else
			{
				LOG_CHECK_ERROR(nRetCode == dwSize);
				m_pPacketAllocator->Free(pMsg);
			}

			pMsg = NULL;
		}
	}

Exit0:
    if (pPacketBuffer)
        m_pPacketAllocator->Free(pPacketBuffer);

    if (pMsg)
        m_pPacketAllocator->Free(pMsg);

    return TRUE;
}

BOOL CMessageHandler::_BroadcastScene(SCENE_INFO *poMsScene, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;
    UINT32 dwGTSessionID;
    PACKET_BUFFER* pGuildBroadBuffer = NULL;
    UINT32 dwGateClientSessionCount[MAX_GT_SESSION_COUNT] = {0};
    BASE_LIST_NODE          *pNode = NULL;
    MS_ONLINE_ROLE_DATA     *pOnlineWorkerRole = NULL;

    LOG_PROCESS_ERROR(poMsScene);

    pNode = poMsScene->lstOnlineRole.Head().pNext;
    while (pNode != &poMsScene->lstOnlineRole.Rear())
    {
        pOnlineWorkerRole = (MS_ONLINE_ROLE_DATA*)pNode;
        if (!pOnlineWorkerRole->pRoleData)
        {
            ERR("_BroadcastScene catch an error! The pointer pRoleData is NULL, but online role exist");
            pNode = pNode->pNext;
            continue;
        }

        dwGTSessionID = GET_GT_INDEX_FROM_SESSION_ID(pOnlineWorkerRole->dwCliSessionID);
        s_dwGateSessionList[dwGTSessionID][dwGateClientSessionCount[dwGTSessionID]++] = pOnlineWorkerRole->dwCliSessionID;

        pNode = pNode->pNext;
    }

    for(INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; ++nIndex)
    {
        if(dwGateClientSessionCount[nIndex] == 1)
        {
            pGuildBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pGuildBroadBuffer);

            memcpy(pGuildBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _SendClient(s_dwGateSessionList[nIndex][0], pGuildBroadBuffer, dwSize);
			LOG_CHECK_ERROR(nRetCode);
			if (!nRetCode)
                m_pPacketAllocator->Free(pGuildBroadBuffer);

            pGuildBroadBuffer = NULL;
        }
        else if(dwGateClientSessionCount[nIndex] > 1)
        {
            pGuildBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pGuildBroadBuffer);

            memcpy(pGuildBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _BroadcastGTClient(nIndex, (UINT32*)s_dwGateSessionList[nIndex], dwGateClientSessionCount[nIndex], pGuildBroadBuffer, dwSize);
            LOG_CHECK_ERROR(nRetCode);

            if (!nRetCode)
                m_pPacketAllocator->Free(pGuildBroadBuffer);

            pGuildBroadBuffer = NULL;
        }
    }

    m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_BroadcastTeam(CTeam* poTeam, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode;
    UINT32 dwGTSessionID;
    PACKET_BUFFER* pTeamBroadBuffer = NULL;
    UINT32 dwGateClientSessionCount[MAX_GT_SESSION_COUNT] = {0};
    LOG_PROCESS_ERROR(poTeam);
    
    for(INT32 nIndex = 0; nIndex < poTeam->GetMemberSizeLimit(); nIndex++)
    {
        UINT32 dwRoleID = poTeam->GetMemberID(nIndex);
        MS_ONLINE_ROLE_DATA* pstOnlienRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
        if (pstOnlienRoleData == NULL)
            continue;
        
        dwGTSessionID = GET_GT_INDEX_FROM_SESSION_ID(pstOnlienRoleData->dwCliSessionID);
        s_dwGateSessionList[dwGTSessionID][dwGateClientSessionCount[dwGTSessionID]++] = pstOnlienRoleData->dwCliSessionID;
    }

    for(INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; ++nIndex)
    {
        if(dwGateClientSessionCount[nIndex] == 1)
        {
            pTeamBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pTeamBroadBuffer);

            memcpy(pTeamBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _SendClient(s_dwGateSessionList[nIndex][0], pTeamBroadBuffer, dwSize);
			LOG_CHECK_ERROR(nRetCode);
			if (!nRetCode)
                m_pPacketAllocator->Free(pTeamBroadBuffer);

			pTeamBroadBuffer = NULL;
        }
        else if(dwGateClientSessionCount[nIndex] > 1)
        {
            pTeamBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pTeamBroadBuffer);

            memcpy(pTeamBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _BroadcastGTClient(nIndex, (UINT32*)s_dwGateSessionList[nIndex], dwGateClientSessionCount[nIndex], pTeamBroadBuffer, dwSize);
            LOG_CHECK_ERROR(nRetCode);

            if (!nRetCode)
                m_pPacketAllocator->Free(pTeamBroadBuffer);

			pTeamBroadBuffer = NULL;
        }
    }

    m_pPacketAllocator->Free(pPacketBuffer);
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_BroadcastGuild(CMsGuild *poMsGuild, PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, UINT32 exceptRoleID, INT32 nOffset )
{
    INT32 nRetCode = 0;
    UINT32 dwGTSessionID;
    PACKET_BUFFER* pGuildBroadBuffer = NULL;
    UINT32 dwGateClientSessionCount[MAX_GT_SESSION_COUNT] = {0};
    CMsGuildMember      *poMsGuildMember = NULL;
    LOG_PROCESS_ERROR(poMsGuild);

    poMsGuildMember = poMsGuild->GetFirstGuildMember();
    while (poMsGuildMember)
    {
        MS_ONLINE_ROLE_DATA* pstOnlienRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poMsGuildMember->GetRoleID());
        if (pstOnlienRoleData == NULL)
        {
            poMsGuildMember = poMsGuild->GetNextGuildMember();
            continue;
        }

        dwGTSessionID = GET_GT_INDEX_FROM_SESSION_ID(pstOnlienRoleData->dwCliSessionID);
        s_dwGateSessionList[dwGTSessionID][dwGateClientSessionCount[dwGTSessionID]++] = pstOnlienRoleData->dwCliSessionID;
        poMsGuildMember = poMsGuild->GetNextGuildMember();
    }

    for(INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; ++nIndex)
    {
        if(dwGateClientSessionCount[nIndex] == 1)
        {
            pGuildBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pGuildBroadBuffer);

            memcpy(pGuildBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _SendClient(s_dwGateSessionList[nIndex][0], pGuildBroadBuffer, dwSize);
			LOG_CHECK_ERROR(nRetCode);
			if (!nRetCode)
                m_pPacketAllocator->Free(pGuildBroadBuffer);

			pGuildBroadBuffer = NULL;
        }
        else if(dwGateClientSessionCount[nIndex] > 1)
        {
            pGuildBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pGuildBroadBuffer);

            memcpy(pGuildBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _BroadcastGTClient(nIndex, (UINT32*)s_dwGateSessionList[nIndex], dwGateClientSessionCount[nIndex], pGuildBroadBuffer, dwSize);
            LOG_CHECK_ERROR(nRetCode);

            if(!nRetCode)
                m_pPacketAllocator->Free(pGuildBroadBuffer);

			pGuildBroadBuffer = NULL;
        }
    }

    m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_BroadcastGuildCheckRight(CMsGuild *poMsGuild, PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, INT32 nRight, UINT32 exceptRoleID, INT32 nOffset)
{
    INT32 nRetCode = 0;
    UINT32 dwGTSessionID;
    PACKET_BUFFER* pGuildBroadBuffer = NULL;
    UINT32 dwGateClientSessionCount[MAX_GT_SESSION_COUNT] = { 0 };
    CMsGuildMember      *poMsGuildMember = NULL;
    LOG_PROCESS_ERROR(poMsGuild);

    poMsGuildMember = poMsGuild->GetFirstGuildMember();
    while (poMsGuildMember)
    {
        MS_ONLINE_ROLE_DATA* pstOnlienRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poMsGuildMember->GetRoleID());
        if (pstOnlienRoleData == NULL)
        {
            poMsGuildMember = poMsGuild->GetNextGuildMember();
            continue;
        }
        if ((nRight != G_POS_RIGHT_NONE) && !(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & nRight))
        {
            poMsGuildMember = poMsGuild->GetNextGuildMember();
            continue;
        }

        dwGTSessionID = GET_GT_INDEX_FROM_SESSION_ID(pstOnlienRoleData->dwCliSessionID);
        s_dwGateSessionList[dwGTSessionID][dwGateClientSessionCount[dwGTSessionID]++] = pstOnlienRoleData->dwCliSessionID;
        poMsGuildMember = poMsGuild->GetNextGuildMember();
    }

    for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; ++nIndex)
    {
        if (dwGateClientSessionCount[nIndex] == 1)
        {
            pGuildBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pGuildBroadBuffer);

            memcpy(pGuildBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _SendClient(s_dwGateSessionList[nIndex][0], pGuildBroadBuffer, dwSize);
            LOG_CHECK_ERROR(nRetCode);
            if (!nRetCode)
                m_pPacketAllocator->Free(pGuildBroadBuffer);

            pGuildBroadBuffer = NULL;
        }
        else if (dwGateClientSessionCount[nIndex] > 1)
        {
            pGuildBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pGuildBroadBuffer);

            memcpy(pGuildBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _BroadcastGTClient(nIndex, (UINT32*)s_dwGateSessionList[nIndex], dwGateClientSessionCount[nIndex], pGuildBroadBuffer, dwSize);
            LOG_CHECK_ERROR(nRetCode);

            if (!nRetCode)
                m_pPacketAllocator->Free(pGuildBroadBuffer);

            pGuildBroadBuffer = NULL;
        }
    }

    m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::_BroadcastMaster(MS_TEACHER_DATA *pTeacherData, PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, UINT32 exceptRoleID, INT32 nOffset)
{
    INT32 nRetCode = 0;
    UINT32 dwGTSessionID;
    PACKET_BUFFER* pGuildBroadBuffer = NULL;
    MS_ONLINE_ROLE_DATA* pstOnlienRoleData = NULL;
    UINT32 dwGateClientSessionCount[MAX_GT_SESSION_COUNT] = {0};
    LOG_PROCESS_ERROR(pTeacherData);

    if (exceptRoleID != pTeacherData->dwRoleID)
    {
        pstOnlienRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pTeacherData->dwRoleID);
        if (pstOnlienRoleData)
        {
            dwGTSessionID = GET_GT_INDEX_FROM_SESSION_ID(pstOnlienRoleData->dwCliSessionID);
            s_dwGateSessionList[dwGTSessionID][dwGateClientSessionCount[dwGTSessionID]++] = pstOnlienRoleData->dwCliSessionID;
        }
    }

    for (INT32 nIndex = 0; nIndex < pTeacherData->byStudNum; ++nIndex)
    {
        if (exceptRoleID == pTeacherData->arrStudentID[nIndex])
            continue;
        pstOnlienRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pTeacherData->arrStudentID[nIndex]);
        if (pstOnlienRoleData)
        {
            dwGTSessionID = GET_GT_INDEX_FROM_SESSION_ID(pstOnlienRoleData->dwCliSessionID);
            s_dwGateSessionList[dwGTSessionID][dwGateClientSessionCount[dwGTSessionID]++] = pstOnlienRoleData->dwCliSessionID;
        }
    }

    for(INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; ++nIndex)
    {
        if(dwGateClientSessionCount[nIndex] == 1)
        {
            pGuildBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pGuildBroadBuffer);

            memcpy(pGuildBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _SendClient(s_dwGateSessionList[nIndex][0], pGuildBroadBuffer, dwSize);
			LOG_CHECK_ERROR(nRetCode);
			if (!nRetCode)
                m_pPacketAllocator->Free(pGuildBroadBuffer);

            pGuildBroadBuffer = NULL;
        }
        else if(dwGateClientSessionCount[nIndex] > 1)
        {
            pGuildBroadBuffer = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pGuildBroadBuffer);

            memcpy(pGuildBroadBuffer + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = _BroadcastGTClient(nIndex, (UINT32*)s_dwGateSessionList[nIndex], dwGateClientSessionCount[nIndex], pGuildBroadBuffer, dwSize);
            LOG_CHECK_ERROR(nRetCode);

            if (!nRetCode)
                m_pPacketAllocator->Free(pGuildBroadBuffer);

            pGuildBroadBuffer = NULL;
        }
    }

    m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    return FALSE;
}

// Announcement to one player
BOOL CMessageHandler::_BroadcastGTClient(UINT32 dwGTSessionID, UINT32 dwClientSessionList[], INT32 nSessionCount, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;
    INT32 nSize = dwSize + sizeof(M2T_BROADCAST_CLIENT_MSG) + sizeof(UINT32) * nSessionCount;
    M2T_BROADCAST_CLIENT_MSG* pMsg = (M2T_BROADCAST_CLIENT_MSG*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2t_broadcast_client_msg;
    pMsg->wSessionCount = nSessionCount;

    for (INT32 nIndex = 0; nIndex < nSessionCount; nIndex++)
        pMsg->dwSessionIDList[nIndex] = dwClientSessionList[nIndex];
    
    memcpy(pMsg->dwSessionIDList + nSessionCount, pPacketBuffer, dwSize);

    nRetCode = _SendGT(dwGTSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);
	
	if (pPacketBuffer)
		m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// Region-wide announcement
BOOL CMessageHandler::_BroadcastGroupClient(UINT8 byGroupID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode;
    UINT32 dwClientSessionCount[MAX_GT_SESSION_COUNT] = {0};

    m_TraverseOnlineRole.dwGateSessionList = s_dwGateSessionList;
    m_TraverseOnlineRole.dwClientSessionCount = dwClientSessionCount;

    nRetCode = COnlineRoleDataMgr::GetInstance().TraverseOnlineRoleInGroup(m_TraverseOnlineRole, byGroupID);
    LOG_PROCESS_ERROR(nRetCode);

    for(INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; ++nIndex)
    {
        if(dwClientSessionCount[nIndex] >= 1)
        {
			PACKET_BUFFER* pBuffer = m_pPacketAllocator->Alloc(dwSize);
			if(pBuffer)
			{
				memcpy(pBuffer + nOffset, pPacketBuffer + nOffset, dwSize);
				
				nRetCode = _BroadcastGTClient(nIndex, (UINT32*)s_dwGateSessionList[nIndex], dwClientSessionCount[nIndex], pBuffer, dwSize);
				LOG_CHECK_ERROR(nRetCode);

				if (!nRetCode)
					m_pPacketAllocator->Free(pBuffer);
			}
        }
    }

    m_pPacketAllocator->Free(pPacketBuffer);
    return TRUE;
Exit0:
    return FALSE;
}

// All servers and regions
BOOL CMessageHandler::_BroadcastAllClient(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode;
    UINT32 dwClientSessionCount[MAX_GT_SESSION_COUNT] = {0};

    m_TraverseOnlineRole.dwGateSessionList = s_dwGateSessionList;
    m_TraverseOnlineRole.dwClientSessionCount = dwClientSessionCount;

    nRetCode = COnlineRoleDataMgr::GetInstance().TraverseOnlineRole(m_TraverseOnlineRole);
    LOG_PROCESS_ERROR(nRetCode);

    for(INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; ++nIndex)
    {
        if(dwClientSessionCount[nIndex] >= 1)
        {
			PACKET_BUFFER* pBuffer = m_pPacketAllocator->Alloc(dwSize);
			if(pBuffer)
			{
				memcpy(pBuffer + nOffset, pPacketBuffer + nOffset, dwSize);
				
				nRetCode = _BroadcastGTClient(nIndex, (UINT32*)s_dwGateSessionList[nIndex], dwClientSessionCount[nIndex], pBuffer, dwSize);
				LOG_CHECK_ERROR(nRetCode);

				if (!nRetCode)
					m_pPacketAllocator->Free(pBuffer);
			}
        }
    }
    
	m_pPacketAllocator->Free(pPacketBuffer);
	return TRUE;
Exit0:
    return FALSE;
}

void CMessageHandler::_CheckAllReady(void)
{
    INT32 nRetCode = 0;

    for (INT32 nIndex = 1; nIndex <= g_ServerConfig.MS.nInitGTCount; nIndex++)
    {
        PROCESS_ERROR(m_ServerInfo.GT[nIndex].byReady);
    }

    for (INT32 nIndex = 1; nIndex <= g_ServerConfig.MS.nInitGSCount; nIndex++)
    {
        PROCESS_ERROR(m_ServerInfo.GS[nIndex].byReady);
    }

    PROCESS_ERROR(m_ServerInfo.DB.byReady);

    PROCESS_ERROR(m_ServerInfo.LS.byReady);

	if (g_ServerConfig.MS.nInitBSCount > 0)
		PROCESS_ERROR(m_ServerInfo.BS.byReady);

	if (g_ServerConfig.MS.nInitDLCount > 0)
		PROCESS_ERROR(m_ServerInfo.DL.byReady);

	if (g_ServerConfig.MS.nInitMMCount > 0)
		PROCESS_ERROR(m_ServerInfo.MM.byReady);

    nRetCode = DoAllReady();
    LOG_PROCESS_ERROR(nRetCode);

    CMSSessionMgr::GetInstance().SetServerState(sgsAllReady);

    {
        SGDSession& oGDSession = CMSSessionMgr::GetInstance().GetGDSession();
        if(oGDSession.bConnected)
        {
            nRetCode = DoGDRegrister();
            LOG_CHECK_ERROR(nRetCode);
        }

        SGMSession& rGMSession = CMSSessionMgr::GetInstance().GetGMSession();
        if (rGMSession.bConnected)
        {
            nRetCode = DoGMRegister();
            LOG_CHECK_ERROR(nRetCode);
        }
    }
Exit0:
    return;
}

BOOL CMessageHandler::CheckAllClose(void)
{
    INT32 nRetCode = 0;

    for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
    {
        PROCESS_ERROR(!m_ServerInfo.GT[nIndex].byReady);
    }

    for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
    {
        PROCESS_ERROR(!m_ServerInfo.GS[nIndex].byReady);
    }

    PROCESS_ERROR(!m_ServerInfo.DB.byReady);

	if(g_ServerConfig.MS.nInitBSCount > 0)
		PROCESS_ERROR(!m_ServerInfo.BS.byReady);

	if(g_ServerConfig.MS.nInitDLCount > 0)
		PROCESS_ERROR(!m_ServerInfo.DL.byReady);

    PROCESS_ERROR(!m_ServerInfo.LS.byReady);
    
    return TRUE;
Exit0:
    return FALSE;
}

///************************************************************************/
//// To All Server
BOOL CMessageHandler::DoServerInfo(void)
{
    INT32 nRetCode = 0;

    M2A_SERVER_INFO* pMsg = (M2A_SERVER_INFO*)(m_pPacketAllocator->Alloc(sizeof(M2A_SERVER_INFO)));
    LOG_PROCESS_ERROR(pMsg);
    
    memcpy(pMsg, &m_ServerInfo, sizeof(M2A_SERVER_INFO));

    nRetCode = _BroadcastAll((PACKET_BUFFER*)pMsg, sizeof(M2A_SERVER_INFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoAllReady(void)
{
    INT32 nRetCode = 0;

    M2A_ALL_READY* pMsg = (M2A_ALL_READY*)(m_pPacketAllocator->Alloc(sizeof(M2A_ALL_READY)));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2a_all_ready;

    nRetCode = _BroadcastAll((PACKET_BUFFER*)pMsg, sizeof(M2A_ALL_READY));
    LOG_PROCESS_ERROR(nRetCode);

    INF("all servers are ready");

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoAllPing(void)
{
    INT32 nRetCode = 0;

    A2A_PING* pMsg = (A2A_PING*)(m_pPacketAllocator->Alloc(sizeof(A2A_PING)));
    LOG_PROCESS_ERROR(pMsg);
    pMsg->wMsg = a2a_ping;

    nRetCode = _BroadcastAll((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
    LOG_CHECK_ERROR(nRetCode);

    if (!nRetCode)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    pMsg = NULL;

    {
        SGDSession& rGDSession = CMSSessionMgr::GetInstance().GetGDSession();
        if (rGDSession.bConnected && rGDSession.pSocketSession)
        {
            pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
            LOG_PROCESS_ERROR(pMsg);

            pMsg->wMsg = a2a_ping;

            nRetCode = _SendGD((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
            LOG_CHECK_ERROR(nRetCode);

            if (!nRetCode)
                m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
            pMsg = NULL;
        }

    }   

    {
        SNSSession& rNSSession = CMSSessionMgr::GetInstance().GetNSSession();
        if (rNSSession.bConnected && rNSSession.pSocketSession)
        {
            pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
            LOG_PROCESS_ERROR(pMsg);

            pMsg->wMsg = a2a_ping;

            nRetCode = _SendNS((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
            LOG_CHECK_ERROR(nRetCode);

            if (!nRetCode)
                m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
            pMsg = NULL;
        }

    }   

    {
        SGMSession &rGMSession = CMSSessionMgr::GetInstance().GetGMSession();
        if (rGMSession.bConnected && rGMSession.pSocketSession )
        {
            pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
            LOG_PROCESS_ERROR(pMsg);
            pMsg->wMsg = a2a_ping;

            nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
            LOG_CHECK_ERROR(nRetCode);

            //DBG("ping gm");

            if (!nRetCode)
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

BOOL CMessageHandler::DoClose(void)
{
    INT32 nRetCode = 0;

    M2A_CLOSE* pMsg = (M2A_CLOSE*)(m_pPacketAllocator->Alloc(sizeof(M2A_CLOSE)));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2a_close;

    nRetCode = _BroadcastAll((PACKET_BUFFER*)pMsg, sizeof(M2A_CLOSE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

void CMessageHandler::OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SMSSession* pSession = (SMSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pSession);

    if(pSession->dwType == stGD || pSession->dwType == stGM)
        pSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nPingGuardTimeout;
    else
        pSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

    return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();
    return;
}

void CMessageHandler::OnGDClose(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    
    CMSSessionMgr::GetInstance().SetServerState(sgsClosing);
	INF("MS is closed by guard");
	INF("MS is closing");
}

void CMessageHandler::OnGDCommand(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	U2M_COMMAND * pMsg = (U2M_COMMAND*)pPacketBuffer;

	if (pMsg->szCommand[0] == '.')
	{
#ifdef _FINAL
		nRetCode = _CallFinalGMCommand(NULL, pMsg->szCommand + 1, stGD);
		LOG_PROCESS_ERROR(nRetCode);

		//nRetCode = DoGMCientCmd(pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pUser->dwClientIp, pstOnlineRole->pRoleData->szRoleName, pMsg->szGMCommand);
		//LOG_PROCESS_ERROR(nRetCode);
#else // _FINAL
		//nRetCode = _CallDebugGMCommand(NULL, pMsg->szCommand + 1);	// allow only final version
		//LOG_PROCESS_ERROR(nRetCode);
#endif	// _FINAL
	}

Exit0:
	return;
}

BOOL CMessageHandler::DoGDRegrister()
{
    INT32 nRetCode = 0;

    A2U_REGRISTER* pMsg = (A2U_REGRISTER*)m_pPacketAllocator->Alloc(sizeof(A2U_REGRISTER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = a2u_regrister;
    pMsg->byType = stMS;
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

BOOL CMessageHandler::DoGDNotify(UINT8 byLevel, const char* szContent, ...)
{
	INT32 nRetCode = 0;
	char szInfo[4096] = { 0 };
	va_list valist;
	va_start(valist, szContent);
	vsnprintf(szInfo, sizeof(szInfo), szContent, valist);
	va_end(valist);

	INT32 nSize = sizeof(A2U_NOTIFY) + strlen(szInfo) + 1;

	A2U_NOTIFY* pMsg = (A2U_NOTIFY*)m_pPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = a2u_notify;
	pMsg->bySvrType = stMS;
	pMsg->bySvrID = 1;
	pMsg->byLevel = byLevel;
	pMsg->nErrorCode = ( byLevel == 0 ) ? errSuccess : errCritical;
	strxcpy(pMsg->szContent, szInfo, strlen(szInfo) + 1);


	nRetCode = _SendGD((PACKET_BUFFER*)pMsg, nSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CMessageHandler::DoGDCommandAck( const char* szResult )
{
	INT32 nRetCode = 0;
	INT32 nSize = sizeof(M2U_COMMAND_RESPONSE) + strlen(szResult) + 1;

	M2U_COMMAND_RESPONSE* pMsg = (M2U_COMMAND_RESPONSE*)m_pPacketAllocator->Alloc( nSize );
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2u_command_ack;
	strxcpy(pMsg->szResult, szResult, strlen(szResult) + 1);

	nRetCode = _SendGD((PACKET_BUFFER*)pMsg, nSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CMessageHandler::DoGMRegister()
{
    INT32       nRetCode = 0;
    M2GM_REGISTER   *pMsg = (M2GM_REGISTER*)m_pPacketAllocator->Alloc(sizeof(M2GM_REGISTER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_register;
    strxcpy(pMsg->szName, g_ServerConfig.Common.szAreaName, sizeof(pMsg->szName));

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_REGISTER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMClearRoleCacheAck(INT32 nResult, UINT32 dwTLUserID, UINT32 dwCmdSerialID)
{
    INT32       nRetCode = 0;

    M2GM_CLEAR_ROLE_CACHE_ACK  *pMsg = (M2GM_CLEAR_ROLE_CACHE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_CLEAR_ROLE_CACHE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_clear_role_cache_ack;
    pMsg->nResult = nResult;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_CLEAR_ROLE_CACHE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMUnlockRoleAck(INT32 nResult, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;

    M2GM_UNLOCK_ROLE_ACK    *pMsg = (M2GM_UNLOCK_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_UNLOCK_ROLE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_unlock_role_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nResult = nResult;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_UNLOCK_ROLE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMQueryRoleDataAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID, INT32 nResult, UINT32 dwLogoutTime)
{
    INT32       nRetCode = 0;
    M2GM_QUERY_ROLE_DATA_ACK    *pMsg = (M2GM_QUERY_ROLE_DATA_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_QUERY_ROLE_DATA_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_query_role_data_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nResult = nResult;
    pMsg->dwLogoutTime = dwLogoutTime;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_QUERY_ROLE_DATA_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMForbidChatAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID, INT32 nResult)
{
    INT32       nRetCode = 0;
    M2GM_FORBID_CHAT_ACK    *pMsg = (M2GM_FORBID_CHAT_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_FORBID_CHAT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_forbid_chat_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nResult = nResult;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_FORBID_CHAT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMFreezeRoleAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID, INT32 nResult)
{
    INT32       nRetCode = 0;
    M2GM_FREEZE_ROLE_ACK    *pMsg = (M2GM_FREEZE_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_FREEZE_ROLE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_freeze_role_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_FREEZE_ROLE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMKickRoleAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID, INT32 nResult)
{
    INT32       nRetCode = 0;
    M2GM_KICK_ROLE_ACK    *pMsg = (M2GM_KICK_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_KICK_ROLE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_kick_role_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_KICK_ROLE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMSendMailAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID, INT32 nResult)
{
    INT32       nRetCode = 0;
    M2GM_SEND_MAIL_ACK    *pMsg = (M2GM_SEND_MAIL_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_SEND_MAIL_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_send_mail_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_SEND_MAIL_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMAnnounceAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, INT32 nResult)
{
    INT32       nRetCode = 0;
    M2GM_ANNOUNCE_ACK    *pMsg = (M2GM_ANNOUNCE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_ANNOUNCE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_announce_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_ANNOUNCE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMGetAnnounceListAck(INT32 nGroupID, UINT32 dwTLUserID, UINT32 dwCmdSerialID, INT32 nResult)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    GM_ANNOUNCE_DATA    *pData = NULL;
    M2GM_GET_ANNOUNCE_LIST_ACK  *pMsg = (M2GM_GET_ANNOUNCE_LIST_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_get_announce_list_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->nCount = 0;

    CGMManager::GetInstance().BeginAnnounce();
    pData = CGMManager::GetInstance().NextAnnounce();
    while (pData)
    {
        LOG_CHECK_ERROR(pData->pTrigger);
        if (pData->pTrigger && ((nGroupID == 0) || (pData->pTrigger->nGroupID == 0) || (nGroupID == pData->pTrigger->nGroupID)))
        {
            pMsg->data[pMsg->nCount].nGroupID = pData->pTrigger->nGroupID;
            pMsg->data[pMsg->nCount].dwSerial = pData->dwSerial;
            pMsg->data[pMsg->nCount].dwRepeatNum = pData->pTrigger->nRepeatNum;
            pMsg->data[pMsg->nCount].dwIntervalTime = pData->pTrigger->nIntervalTime;
            strxcpy(pMsg->data[pMsg->nCount].szText, pData->pTrigger->szText, sizeof(pMsg->data[pMsg->nCount].szText));
            ++pMsg->nCount;
        }

        pData = CGMManager::GetInstance().NextAnnounce();
    }

    nLength = sizeof(M2GM_GET_ANNOUNCE_LIST_ACK) + sizeof(ANNOUNCE_DATA) * pMsg->nCount;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMDelAnnounceAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, INT32 nResult, UINT32 dwAnnounceSerial)
{
    INT32       nRetCode = 0;
    M2GM_DEL_ANNOUNCE_ACK   *pMsg = (M2GM_DEL_ANNOUNCE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_DEL_ANNOUNCE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_del_announce_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwAnnounceSerial = dwAnnounceSerial;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_DEL_ANNOUNCE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMExeGMStrAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, INT32 nResult)
{
    INT32       nRetCode = 0;
    M2GM_EXE_GM_STR_ACK    *pMsg = (M2GM_EXE_GM_STR_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_EXE_GM_STR_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_exe_gm_str_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_EXE_GM_STR_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMStrCmdLog(UINT32 dwTime, INT32 nGroupID, UINT32 dwIP, UINT32 dwRoleID, const char *pcszRoleName, const char *pcszCmd, INT32 nResult)
{
    INT32       nRetCode = 0;
    INT32 nSize = 0;
    M2GM_GM_STR_CMD_LOG *pMsg = NULL;

    LOG_PROCESS_ERROR(pcszRoleName);
    LOG_PROCESS_ERROR(pcszCmd);

    if (!g_ServerConfig.MS.bNeedConnectGMServer)
    {
        // write log to local disk todo_tq

        return TRUE;
    }

    nSize = strlen(pcszCmd) + 1;

    pMsg = (M2GM_GM_STR_CMD_LOG*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_gm_str_cmd_log;
    pMsg->dwTime = dwTime;
    pMsg->byGroupID = nGroupID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwIP = dwIP;
    pMsg->nResult = nResult;
    strxcpy(pMsg->szRoleName, pcszRoleName, ROLE_NAME_LEN);
    strxcpy(pMsg->szGMCommand, pcszCmd, nSize);

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_GM_STR_CMD_LOG) + nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pcszCmd && pMsg)
        INF("role %d failed to send gm command to gmserver:[%s]", pMsg->dwRoleID, pcszCmd);
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMDelMailAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwMailID, INT32 nResult, MailInfo *pMailInfo)
{
    INT32       nRetCode = 0;
    M2GM_DEL_MAIL_ACK    *pMsg = (M2GM_DEL_MAIL_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_DEL_MAIL_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_del_mail_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwMailID = dwMailID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    if (pMailInfo)
        memcpy(pMsg->szBuffer, (char*)pMailInfo, sizeof(MailInfo));

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_DEL_MAIL_ACK) + sizeof(MailInfo));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMCheckRoleFreezeAck(UINT32 dwTLUserID, UINT32 dwCmdSerialID, INT32 nResult)
{
    INT32       nRetCode = 0;
    M2GM_CHECK_ROLE_FREEZE_ACK    *pMsg = (M2GM_CHECK_ROLE_FREEZE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_CHECK_ROLE_FREEZE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_check_role_freeze_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nResult = nResult;
    pMsg->dwCmdSerialID = dwCmdSerialID;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_CHECK_ROLE_FREEZE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMGetExchangePriceAck(UINT8 byGroupID)
{
    INT32 nRetCode = 0;
    M2GM_GET_EXCHANGE_PRICE_ACK* pMsg = NULL;
    INT32 buy_price[10];
    INT32 buy_handcount[10];
    UINT8 buy_count = 0;
    INT32 sell_price[10];
    INT32 sell_handcount[10];
    UINT8 sell_count = 0;
    UINT32 size = 0;

    CExchangeMgr::GetInstance().GetPrices(byGroupID, eotSell, 10, sell_count, sell_price, sell_handcount);
    CExchangeMgr::GetInstance().GetPrices(byGroupID, eotBuy, 10, buy_count, buy_price, buy_handcount);
    size = sizeof(M2GM_GET_EXCHANGE_PRICE_ACK) + sizeof(EXCHANGE_PRICE_INFO) * (buy_count + sell_count);

    pMsg = (M2GM_GET_EXCHANGE_PRICE_ACK*)m_pPacketAllocator->Alloc(size);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_get_exchange_price_ack;
    pMsg->byGroupID = byGroupID;
    pMsg->bySellPriceCount = sell_count;
    for (UINT8 i = 0; i < sell_count; i++)
    {
        pMsg->stPrices[i].nPrice = sell_price[i];
        pMsg->stPrices[i].nCount = sell_handcount[i];
    }

    pMsg->byBuyPriceCount = buy_count;
    for (UINT8 i = 0; i < buy_count; i++)
    {
        pMsg->stPrices[sell_count + i].nPrice = buy_price[i];
        pMsg->stPrices[sell_count + i].nCount = buy_handcount[i];
    }

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, size);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMGetExchangePriceDetailAck(UINT8 byGroupID, UINT8 byOrderType, INT32 nPrice, UINT32 dwTLUserID)
{
    INT32 nRetCode = 0;
    M2GM_GET_EXCHANGE_PRICE_DETAIL_ACK* pMsg = NULL;
    UINT8 count = 0;
    UINT32 roleid[100];
    INT32 handcount[100];
    UINT64 orderid[100];
    UINT32 size = 0;
    MS_ROLE_DATA    *pRoleData = NULL;

    CExchangeMgr::GetInstance().GetCounts(byGroupID, byOrderType, nPrice, 100, count, roleid, handcount, orderid);
    size = sizeof(M2GM_GET_EXCHANGE_PRICE_DETAIL_ACK) + sizeof(EXCHANGE_COUNT_INFO) * count;

    pMsg = (M2GM_GET_EXCHANGE_PRICE_DETAIL_ACK*)m_pPacketAllocator->Alloc(size);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_get_exchange_price_detail_ack;
    pMsg->byGroupID = byGroupID;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byOrderType = byOrderType;
    pMsg->nPrice = nPrice;
    pMsg->byCountCount = count;

    for (UINT8 i = 0; i < count; i++)
    {
        pMsg->stCounts[i].dwRoleID = roleid[i];
        memset(pMsg->stCounts[i].szRoleName, 0, ROLE_NAME_LEN);
        if (roleid[i] != 0)
        {
            pRoleData = CRoleDataMgr::GetInstance().FindRole(roleid[i]);
            LOG_CHECK_ERROR(pRoleData);
            if (pRoleData)
                strxcpy(pMsg->stCounts[i].szRoleName, pRoleData->szRoleName, ROLE_NAME_LEN);
            else
                strxcpy(pMsg->stCounts[i].szRoleName, "none", ROLE_NAME_LEN);
        }
        else
            strxcpy(pMsg->stCounts[i].szRoleName, "system", ROLE_NAME_LEN);

        pMsg->stCounts[i].nCount = handcount[i];
        pMsg->stCounts[i].qwOrderID = orderid[i];
    }

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, size);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMAddSystemOrderAck(UINT32 dwTLUserID, UINT8 byGroupID, UINT64 qwOrderID, INT32 nResult, INT32 nPrice, INT32 nCount)
{
    INT32 nRetCode = 0;
    M2GM_ADD_SYSTEM_ORDER_ACK* pMsg = (M2GM_ADD_SYSTEM_ORDER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_ADD_SYSTEM_ORDER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_add_system_order_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byGroupID = byGroupID;
    pMsg->qwOrderID = qwOrderID;
    pMsg->nResult = nResult;
    pMsg->nPrice = nPrice;
    pMsg->nCount = nCount;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_ADD_SYSTEM_ORDER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMCancelSystemOrderAck(UINT32 dwTLUserID, UINT8 byGroupID, UINT64 qwOrderID, INT32 nResult, INT32 nPrice, INT32 nCount)
{
    INT32 nRetCode = 0;
    M2GM_CANCEL_SYSTEM_ORDER_ACK* pMsg = (M2GM_CANCEL_SYSTEM_ORDER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_CANCEL_SYSTEM_ORDER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_cancel_system_order_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byGroupID = byGroupID;
    pMsg->qwOrderID = qwOrderID;
    pMsg->nResult = nResult;
    pMsg->nPrice = nPrice;
    pMsg->nCount = nCount;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_CANCEL_SYSTEM_ORDER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMGetExchangeLimitAck(UINT32 dwTLUserID, UINT8 byGroupID, INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit)
{
    INT32 nRetCode = 0;
    M2GM_GET_EXCHANGE_LIMIT_ACK* pMsg = (M2GM_GET_EXCHANGE_LIMIT_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_GET_EXCHANGE_LIMIT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_get_exchange_limit_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byGroupID = byGroupID;
    pMsg->nPriceHigh = nPriceHigh;
    pMsg->nPriceLow = nPriceLow;
    pMsg->nSingleOrderLimit = nSingleOrderLimit;
    pMsg->nDailyOrderLimit = nDailyOrderLimit;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_GET_EXCHANGE_LIMIT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGMSetExchangeLimitAck(UINT32 dwTLUserID, UINT8 byGroupID, INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit)
{
    INT32 nRetCode = 0;
    M2GM_SET_EXCHANGE_LIMIT_ACK* pMsg = (M2GM_SET_EXCHANGE_LIMIT_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_SET_EXCHANGE_LIMIT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2gm_set_exchange_limit_ack;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byGroupID = byGroupID;
    pMsg->nPriceHigh = nPriceHigh;
    pMsg->nPriceLow = nPriceLow;
    pMsg->nSingleOrderLimit = nSingleOrderLimit;
    pMsg->nDailyOrderLimit = nDailyOrderLimit;

    nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(M2GM_SET_EXCHANGE_LIMIT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

void CMessageHandler::OnGMRegisterAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SGMSession  * pSession{};
    INT32       nRetCode = 0;
    GM2M_REGISTER_ACK   *pMsg = (GM2M_REGISTER_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pSession = (SGMSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pSession);

    pSession->bRegistered = TRUE;

    INF("GM registered");

    //pSession->eState = egssNormal;

    return;
Exit0:
    return;
}

void CMessageHandler::OnGMClearRoleCache(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32                   nRetCode = 0;
    GM2M_CLEAR_ROLE_CACHE  *pMsg = (GM2M_CLEAR_ROLE_CACHE*)pPacketBuffer;

    nRetCode = DoDBClearCache(pMsg->dwRoleID, pMsg->dwTLUserID, pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    nRetCode = DoGMClearRoleCacheAck(errUnknownError, pMsg->dwTLUserID, pMsg->dwCmdSerialID);
    LOG_CHECK_ERROR(nRetCode);
}

void CMessageHandler::OnGMUnlockRole(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32   nRetCode = 0;
    GM2M_UNLOCK_ROLE    *pMsg = (GM2M_UNLOCK_ROLE*)pPacketBuffer;

    nRetCode = CGMManager::GetInstance().DelLockUser(pMsg->szUserName);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoGMUnlockRoleAck(errSuccess, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return ;
Exit0:
    nRetCode = DoGMUnlockRoleAck(errUnknownError, pMsg->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);
}

void CMessageHandler::OnGMQueryRoleData(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32   nRetCode = 0;
    MS_ROLE_DATA        *pRoleData = NULL;
    GM2M_QUERY_ROLE_DATA *pMsg = (GM2M_QUERY_ROLE_DATA*)pPacketBuffer;

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pRoleData);

    nRetCode = CMessageHandler::GetInstance().DoGMQueryRoleDataAck(pMsg->dwTLUser, pMsg->dwCmdSerialID, pMsg->dwRoleID, errSuccess, pRoleData->nLastLoginTime);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    nRetCode = CMessageHandler::GetInstance().DoGMQueryRoleDataAck(pMsg->dwTLUser, pMsg->dwCmdSerialID, pMsg->dwRoleID, errLoginDeleteRoleNotFound, 0);
    LOG_CHECK_ERROR(nRetCode);
    
    return;
}

void CMessageHandler::OnGMForbidChat(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    GM2M_FORBID_CHAT    *pMsg = (GM2M_FORBID_CHAT*)pPacketBuffer;
    MS_ROLE_DATA    *pRoleData = NULL;
    MS_ONLINE_ROLE_DATA *pOnlineRoleData = NULL;

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(pRoleData, errGMCannotFindRole);

    if (pMsg->dwTime > 0)
    {
        if (pMsg->dwTime - time(NULL) > 600)
        {
            pMsg->dwTime = (pMsg->dwTime == 0xFFFFFFFF ? 0x7FFFFFF0 : pMsg->dwTime);
            nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlReplaceGM, NULL, NULL, "uculud", pRoleData->byGroup, (((UINT64)pMsg->dwRoleID) << 32) + GM_RECORD_FORBID_CHAT, pMsg->dwTime);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMDBError);
        }
    }
    else
    {
        nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlDeleteGM, NULL, NULL, "ul", (((UINT64)pMsg->dwRoleID) << 32) + GM_RECORD_FORBID_CHAT);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMDBError);
    }

    pRoleData->dwForbidChatTime = pMsg->dwTime;

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pRoleData->dwRoleID);
    if (pOnlineRoleData)
    {
        nRetCode = DoGSSyncGMData(pOnlineRoleData->dwGSSessionID, pRoleData->dwRoleID, pMsg->dwTime);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = DoGMForbidChatAck(pMsg->dwTLUser, pMsg->dwCmdSerialID, pMsg->dwRoleID, errSuccess);
    LOG_CHECK_ERROR(nRetCode);

    return ;
Exit0:
    nRetCode = DoGMForbidChatAck(pMsg->dwTLUser, pMsg->dwCmdSerialID, pMsg->dwRoleID, nResult);
    LOG_CHECK_ERROR(nRetCode);
}

void CMessageHandler::OnGMFreezeRole(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    GM2M_FREEZE_ROLE    *pMsg = (GM2M_FREEZE_ROLE*)pPacketBuffer;
    MS_ROLE_DATA        *pRoleData = NULL;

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(pRoleData, errGMCannotFindRole);

    if (pMsg->dwTime > 0)
    {
        if (pMsg->dwTime - time(NULL) > 600)
        {
            pMsg->dwTime = (pMsg->dwTime == 0xFFFFFFFF ? 0 : pMsg->dwTime);
            nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlReplaceGM, NULL, NULL, "uculud", pRoleData->byGroup, (((UINT64)pMsg->dwRoleID) << 32) + GM_RECORD_FREEZE_ROLE, pMsg->dwTime);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMDBError);
        }
    }
    else
    {
        nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteGM, NULL, NULL, "ul", (((UINT64)pMsg->dwRoleID) << 32) + GM_RECORD_FREEZE_ROLE);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMDBError);
    }

    pRoleData->dwLoginFrozenTime = pMsg->dwTime;

    nRetCode = DoGMFreezeRoleAck(pMsg->dwTLUser, pMsg->dwCmdSerialID, pMsg->dwRoleID, errSuccess);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    nRetCode = DoGMFreezeRoleAck(pMsg->dwTLUser, pMsg->dwCmdSerialID, pMsg->dwRoleID, nResult);
    LOG_CHECK_ERROR(nRetCode);
}

void CMessageHandler::OnGMKickRole(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    GM2M_KICK_ROLE    *pMsg = (GM2M_KICK_ROLE*)pPacketBuffer;
    MS_ONLINE_ROLE_DATA *pOnlineRoleData = NULL;

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    if (pOnlineRoleData)
    {
        DoErrorNotify(pOnlineRoleData->dwCliSessionID, errGMKickYou, 0, 0);
        KICK_USER(pOnlineRoleData->dwCliSessionID, ktImmediately);
    }

    nRetCode = DoGMKickRoleAck(pMsg->dwTLUser, pMsg->dwCmdSerialID, pMsg->dwRoleID, errSuccess);
    LOG_CHECK_ERROR(nRetCode);

    return ;
Exit0:
    nRetCode = DoGMKickRoleAck(pMsg->dwTLUser, pMsg->dwCmdSerialID, pMsg->dwRoleID, nResult);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGMSendMail(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32           nRetCode = 0;
    INT32           nResult = errGMUnknownError;
    GM2M_SEND_MAIL  *pMsg = (GM2M_SEND_MAIL*)pPacketBuffer;
    MS_ROLE_DATA    *pRoleData = NULL;

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(pRoleData, errGMCannotFindRole);

    if (pMsg->nItemNum == 0)
    {
        nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, pMsg->szSubject, pMsg->szContent);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMMSExecuteCmdFailed);
    }
    else
    {
        nRetCode = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(pRoleData, pMsg->szSubject, pMsg->szContent, pMsg->dwItemID, pMsg->nItemNum);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMMSExecuteCmdFailed);
    }

    nRetCode = DoGMSendMailAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, pMsg->dwRoleID, errSuccess);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    nRetCode = DoGMSendMailAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, pMsg->dwRoleID, nResult);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGMAnnounce(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    GM2M_ANNOUNCE    *pMsg = (GM2M_ANNOUNCE*)pPacketBuffer;

    nRetCode = CGMManager::GetInstance().AddAnnounce(pMsg->nGroupID, pMsg->dwBeginTime, pMsg->nRepeatNum, pMsg->dwIntervalTime, pMsg->szText);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMMSExecuteCmdFailed);

    nRetCode = DoGMAnnounceAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, errSuccess);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    nRetCode = DoGMAnnounceAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, nResult);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGMGetAnnounceList(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    GM2M_GET_ANNOUNCE_LIST  *pMsg = (GM2M_GET_ANNOUNCE_LIST*)pPacketBuffer;

    nRetCode = DoGMGetAnnounceListAck(pMsg->nGroupID, pMsg->dwTLUserID, pMsg->dwCmdSerialID, errSuccess);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGMDelAnnounce(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    GM2M_DEL_ANNOUNCE       *pMsg = (GM2M_DEL_ANNOUNCE*)pPacketBuffer;

    nRetCode = CGMManager::GetInstance().DelAnnounceTrigger(pMsg->nGroupID, pMsg->dwAnnounceSerial);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMAnnounceNotExist);

    nRetCode = DoGMDelAnnounceAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, errSuccess, pMsg->dwAnnounceSerial);
    LOG_PROCESS_ERROR(nRetCode);
    return;
Exit0:
    nRetCode = DoGMDelAnnounceAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, nResult, pMsg->dwAnnounceSerial);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGMExeGMStr(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    GM2M_EXE_GM_STR    *pMsg = (GM2M_EXE_GM_STR*)pPacketBuffer;

    nRetCode = _CallFinalGMCommand(NULL, pMsg->szGMStr, stGM);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoGMExeGMStrAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, errSuccess);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    nRetCode = DoGMExeGMStrAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, nResult);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGMDelMail(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    MS_ROLE_DATA        *pRoleData = NULL;
    MailInfo    tempMail;
    MailInfo    *pMailInfo = NULL;
    GM2M_DEL_MAIL   *pMsg = (GM2M_DEL_MAIL*)pPacketBuffer;

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(pRoleData, errGMCannotFindRole);

    pMailInfo = CMailMgr::GetInstance().GetMailInfo(pMsg->dwMailID);
    LOG_PROCESS_ERROR_RET_CODE(pMailInfo, errMailNotFound);

    tempMail.byMailType = pMailInfo->byMailType;
    tempMail.dwMailID = pMailInfo->dwMailID;
    tempMail.dwMailTime = pMailInfo->dwMailTime;
    tempMail.dwSenderRoleID = pMailInfo->dwSenderRoleID;
    tempMail.dwReceiverRoleID = pMailInfo->dwReceiverRoleID;
    tempMail.byNew = pMailInfo->byNew;
    strxcpy(tempMail.szSenderRoleName, pMailInfo->szSenderRoleName, ROLE_NAME_LEN);
    strxcpy(tempMail.szMailSubject, pMailInfo->szMailSubject, MAIL_SUBJECT_LEN);
    strxcpy(tempMail.szMailContent, pMailInfo->szMailContent, MAIL_CONTENT_LEN);

    tempMail.ma.byAttachLocked = pMailInfo->ma.byAttachLocked;
    tempMail.ma.byAttachType = pMailInfo->ma.byAttachType;
    
    if (tempMail.ma.byAttachType == ATTACHTYPE_SIMPLE)
    {
        tempMail.ma.byAttachLen = pMailInfo->ma.byAttachLen;
        tempMail.ma.sa.nItemID = pMailInfo->ma.sa.nItemID;
        tempMail.ma.sa.nItemCount = pMailInfo->ma.sa.nItemCount;
    }
    else if (tempMail.ma.byAttachType == ATTACHTYPE_BUFFER)
    {
        tempMail.ma.byAttachLen = pMailInfo->ma.byAttachLen;
        memcpy(tempMail.ma.buffer, pMailInfo->ma.buffer, pMailInfo->ma.byAttachLen);
    }
    else
    {
        tempMail.ma.byAttachLen = 0;
    }

    nRetCode = CMailMgr::GetInstance().DeleteMailByGM(pRoleData, pMsg->dwMailID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);


    nRetCode = DoGMDelMailAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, pMsg->dwMailID, errSuccess, &tempMail);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    nRetCode = DoGMDelMailAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, pMsg->dwMailID, nResult, NULL);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGMCheckRoleFreeze(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    MS_ROLE_DATA        *pRoleData = NULL;
    MS_ONLINE_ROLE_DATA *pOnlineRole = NULL;
    GM2M_CHECK_ROLE_FREEZE   *pMsg = (GM2M_CHECK_ROLE_FREEZE*)pPacketBuffer;

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(pRoleData, errGMCannotFindRole);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    if (pOnlineRole)
    {
        nResult = errGMRoleOnline;
    }
    else
    {
        if (pRoleData->dwLoginFrozenTime > (CManageServer::GetInstance().GetGameTime() + 3600))
        {
            nResult = errSuccess;
        }
        else if (pRoleData->dwLoginFrozenTime > CManageServer::GetInstance().GetGameTime())
        {
            nResult = errGMRoleFreezeLessOneHour;
        }
        else
        {
            nResult = errGMRoleNotFreeze;
        }
    }

    nRetCode = DoGMCheckRoleFreezeAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, nResult);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    nRetCode = DoGMCheckRoleFreezeAck(pMsg->dwTLUserID, pMsg->dwCmdSerialID, nResult);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGMGetExchangePrice(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    GM2M_GET_EXCHANGE_PRICE* pMsg = (GM2M_GET_EXCHANGE_PRICE*)pPacketBuffer;
    DoGMGetExchangePriceAck(pMsg->byGroupID);
}

void CMessageHandler::OnGMGetExchangePriceDetail(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    GM2M_GET_EXCHANGE_PRICE_DETAIL* pMsg = (GM2M_GET_EXCHANGE_PRICE_DETAIL*)pPacketBuffer;
    DoGMGetExchangePriceDetailAck(pMsg->byGroupID, pMsg->byOrderType, pMsg->nPrice, pMsg->dwTLUserID);
}

void CMessageHandler::OnGMAddSystemOrder(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    GM2M_ADD_SYSTEM_ORDER* pMsg = (GM2M_ADD_SYSTEM_ORDER*)pPacketBuffer;
    CExchangeMgr::GetInstance().AddSystemOrder(pMsg->dwTLUserID, pMsg->byGroupID, pMsg->nPrice, pMsg->nCount);
}

void CMessageHandler::OnGMCancelSystemOrder(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    GM2M_CANCEL_SYSTEM_ORDER* pMsg = (GM2M_CANCEL_SYSTEM_ORDER*)pPacketBuffer;
    CExchangeMgr::GetInstance().CancelSystemOrder(pMsg->dwTLUserID, pMsg->byGroupID, pMsg->qwOrderID);
}

void CMessageHandler::OnGMGetExchangeLimit(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    GM2M_GET_EXCHANGE_LIMIT* pMsg = (GM2M_GET_EXCHANGE_LIMIT*)pPacketBuffer;
    CExchangeMgr::GetInstance().GetSystemLimit(pMsg->dwTLUserID, pMsg->byGroupID);
}

void CMessageHandler::OnGMSetExchangeLimit(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    GM2M_SET_EXCHANGE_LIMIT* pMsg = (GM2M_SET_EXCHANGE_LIMIT*)pPacketBuffer;
    CExchangeMgr::GetInstance().SetSystemLimit(pMsg->dwTLUserID, pMsg->byGroupID, pMsg->nPriceHigh, pMsg->nPriceLow, pMsg->nSingleOrderLimit, pMsg->nDailyOrderLimit);
}

BOOL CMessageHandler::_TryLoginQueryScene(MS_ONLINE_ROLE_DATA* pstOnlineRole, UINT32 dwRoleID, 
	const SCENE_DATA* pcSceneData)
{
	BOOL bResult = FALSE;
	INT32 nRetCode = 0;
	INT32 nServerGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID);

	switch (pcSceneData->nType)
	{
	case estStatic:
		{
			if (pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID > 0)
				LOG_CHECK_ERROR(pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID == ((nServerGroupID << 24) | 1));
			pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID = (nServerGroupID << 24) | 1;

			SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID);

			LOG_PROCESS_ERROR(pSceneInfo);
			LOG_PROCESS_ERROR(pSceneInfo->eState == ESCENESTATE_NORMAL);
			LOG_PROCESS_ERROR(pstOnlineRole->pUser);

			// temp fix
			if (pcSceneData->nInitCount == 0)
				PROCESS_ERROR(pSceneInfo->dwCreateTime < pstOnlineRole->pRoleData->nLastLoginTime);

			nRetCode = DoGSPreEnterGS(
				pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
			LOG_PROCESS_ERROR(nRetCode);

			nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
			LOG_PROCESS_ERROR(nRetCode);

			pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
			pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;
			bResult = TRUE;
		}
		break;
	case estUnique:
		{
			pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID = 1;

			SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID);

			LOG_PROCESS_ERROR(pSceneInfo);
			LOG_PROCESS_ERROR(pSceneInfo->eState == ESCENESTATE_NORMAL);
			LOG_PROCESS_ERROR(pstOnlineRole->pUser);

			nRetCode = DoGSPreEnterGS(
				pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
			LOG_PROCESS_ERROR(nRetCode);

			nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
			LOG_PROCESS_ERROR(nRetCode);

			pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
			pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;
			bResult = TRUE;
		}
		break;
	case estDungeon:
	case estSoloDungeon:
		{
			SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID);

			if (pSceneInfo && pSceneInfo->dwCreateTime < pstOnlineRole->dwSaveTime && pSceneInfo->eState == ESCENESTATE_NORMAL)
			{
				LOG_CHECK_ERROR(pSceneInfo->nServerGroupID == nServerGroupID);

				switch (pSceneInfo->eState)
				{
				case ESCENESTATE_NORMAL:
					{
						LOG_PROCESS_ERROR(pstOnlineRole->pUser);

						nRetCode = DoGSPreEnterGS(
							pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
						LOG_PROCESS_ERROR(nRetCode);

						nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
						LOG_PROCESS_ERROR(nRetCode);

						pSceneInfo->bDelete = FALSE;
						pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
						pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;

						bResult = TRUE;
					}
					break;
				case ESCENESTATE_CREATING:
				case ESCENESTATE_CLOSING:
				case ESCENESTATE_DESTROYING:
				default:
					LOG_PROCESS_ERROR(FALSE);
				}
			}
			else
			{
				if (pstOnlineRole->pRoleData->dwTeamID > 0 && pcSceneData->nType != estSoloDungeon)
				{
					CTeam* poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
					LOG_CHECK_ERROR(poTeam);

					if (poTeam && poTeam->GetLeader() == dwRoleID)
					{
						poTeam->SetDungeonTempID(0);
						poTeam->SetDungeonID(0);
					}
				}
			}
		}
		break;
	case estBattleField:
		{
			SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID);

			if (pSceneInfo && pSceneInfo->dwCreateTime < pstOnlineRole->dwSaveTime && pSceneInfo->eState == ESCENESTATE_NORMAL)
			{
				LOG_CHECK_ERROR(pSceneInfo->nServerGroupID == nServerGroupID);

				nRetCode = CBattleQueueMgr::GetInstance().CheckBattleTempRole(dwRoleID, pSceneInfo->nSceneTempID, pSceneInfo->nSceneID);
				if (nRetCode && pSceneInfo->eState == ESCENESTATE_NORMAL)
				{
					LOG_PROCESS_ERROR(pstOnlineRole->pUser);

					nRetCode = DoGSPreEnterGS(
						pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
					LOG_PROCESS_ERROR(nRetCode);

					nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
					LOG_PROCESS_ERROR(nRetCode);

					pSceneInfo->bDelete = FALSE;
					pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
					pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;

					bResult = TRUE;
				}
			}
		}
		break;
	case estArena:
		{
			SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID);

			if (pSceneInfo && pSceneInfo->dwCreateTime < pstOnlineRole->dwSaveTime && pSceneInfo->eState == ESCENESTATE_NORMAL)
			{
				LOG_CHECK_ERROR(pSceneInfo->nServerGroupID == nServerGroupID);

				//nRetCode = CArenaQueueMgr::GetInstance().CheckArenaTempRole(dwRoleID, pSceneInfo->nSceneTempID, pSceneInfo->nSceneID, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].bySide);
				if (/*nRetCode && */pSceneInfo->eState == ESCENESTATE_NORMAL)
				{
					LOG_PROCESS_ERROR(pstOnlineRole->pUser);

					nRetCode = DoGSPreEnterGS(
						pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
					LOG_PROCESS_ERROR(nRetCode);

					nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
					LOG_PROCESS_ERROR(nRetCode);

					pSceneInfo->bDelete = FALSE;
					pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
					pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;

					bResult = TRUE;
				}
			}
		}
		break;
	case estLine:
		{
			if (pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID > 0)
			{
				SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID);

				if (pSceneInfo && pSceneInfo->eState == ESCENESTATE_NORMAL && pSceneInfo->nRoleCount < pcSceneData->nMaxRoleCount)
				{
					LOG_CHECK_ERROR(pSceneInfo->nServerGroupID == nServerGroupID);
					LOG_PROCESS_ERROR(pstOnlineRole->pUser);

					nRetCode = DoGSPreEnterGS(
						pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
					LOG_PROCESS_ERROR(nRetCode);

					nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
					LOG_PROCESS_ERROR(nRetCode);

					pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
					pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;
					bResult = TRUE;
				}
				else if (pSceneInfo && pSceneInfo->eState == ESCENESTATE_CREATING && pSceneInfo->WaitUserList.size() < pcSceneData->nMaxRoleCount)
				{
					pSceneInfo->WaitUserList.push_back(pstOnlineRole->dwCliSessionID);
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID = pSceneInfo->nSceneID;
					pstOnlineRole->pUser->eState = USER_STATE_LOGIN_WAIT_CREATE_SCENE;
					bResult = TRUE;
				}
			}

			if (!bResult)
			{
				UINT32 dwLineIDlst[MAX_LINE_SCENE_NUMBER];
				UINT32 nLineIDCount = 0;
				UINT32 dwGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID);

				nLineIDCount = CSceneMgr::GetInstance().GetLineInfo(dwGroupID,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, MAX_LINE_SCENE_NUMBER, dwLineIDlst);

				for (INT32 i = 0; i < nLineIDCount; ++i)
				{
					SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, dwLineIDlst[i]);

					if (pSceneInfo && pSceneInfo->eState == ESCENESTATE_NORMAL && pSceneInfo->nRoleCount < (pcSceneData->nMaxRoleCount * 4 / 5))
					{
						LOG_CHECK_ERROR(pSceneInfo->nServerGroupID == nServerGroupID);

						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID = pSceneInfo->nSceneID;

						LOG_PROCESS_ERROR(pstOnlineRole->pUser);

						nRetCode = DoGSPreEnterGS(
							pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
							pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
						LOG_PROCESS_ERROR(nRetCode);

						nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
						LOG_PROCESS_ERROR(nRetCode);

						pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
						pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;
						bResult = TRUE;
						break;
					}
					else if (pSceneInfo && pSceneInfo->eState == ESCENESTATE_CREATING && pSceneInfo->WaitUserList.size() < (pcSceneData->nMaxRoleCount * 4 / 5))
					{
						pSceneInfo->WaitUserList.push_back(pstOnlineRole->dwCliSessionID);
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID = pSceneInfo->nSceneID;
						pstOnlineRole->pUser->eState = USER_STATE_LOGIN_WAIT_CREATE_SCENE;
						bResult = TRUE;
					}
				}
			}

			if (!bResult)
			{

				nRetCode = _CreateNewScene(pstOnlineRole, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection,
					USER_STATE_LOGIN_WAIT_CREATE_SCENE, 0);

				if (nRetCode)
					bResult = TRUE;
			}
		}
		break;
	case estGuild:
		{
			if (pstOnlineRole->pRoleData->dwGuildID > 0)
			{
				SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, pstOnlineRole->pRoleData->dwGuildID);

				if (pSceneInfo && pSceneInfo->eState == ESCENESTATE_NORMAL && pSceneInfo->nRoleCount < pcSceneData->nMaxRoleCount)
				{
					LOG_CHECK_ERROR(pSceneInfo->nServerGroupID == nServerGroupID);
					LOG_PROCESS_ERROR(pstOnlineRole->pUser);

					nRetCode = DoGSPreEnterGS(
						pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
						pstOnlineRole->pRoleData->dwGuildID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
					LOG_PROCESS_ERROR(nRetCode);

					nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
					LOG_PROCESS_ERROR(nRetCode);

					pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
					pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;
					bResult = TRUE;
				}
				else if (pSceneInfo && pSceneInfo->eState == ESCENESTATE_CREATING && pSceneInfo->WaitUserList.size() < pcSceneData->nMaxRoleCount)
				{
					pSceneInfo->WaitUserList.push_back(pstOnlineRole->dwCliSessionID);
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID = pSceneInfo->nSceneID;
					pstOnlineRole->pUser->eState = USER_STATE_LOGIN_WAIT_CREATE_SCENE;
					bResult = TRUE;
				}

				if (!bResult)
				{
					nRetCode = _CreateNewScene(pstOnlineRole, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection,
						USER_STATE_LOGIN_WAIT_CREATE_SCENE, pstOnlineRole->pRoleData->dwGuildID);

					if (nRetCode)
						bResult = TRUE;
				}
			}
		}
		break;
	case estBirth:
		{
			SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, pstOnlineRole->pRoleData->dwRoleID);

			if (pSceneInfo && pSceneInfo->eState == ESCENESTATE_NORMAL)
			{
				LOG_CHECK_ERROR(pSceneInfo->nServerGroupID == nServerGroupID);
				LOG_PROCESS_ERROR(pstOnlineRole->pUser);

				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID = pstOnlineRole->pRoleData->dwRoleID;

				nRetCode = DoGSPreEnterGS(
					pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
				LOG_PROCESS_ERROR(nRetCode);

				nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
				LOG_PROCESS_ERROR(nRetCode);

				pSceneInfo->bDelete = FALSE;
				pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
				pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;

				bResult = TRUE;
			}
			else
			{
				nRetCode = _CreateNewScene(pstOnlineRole, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection,
					USER_STATE_LOGIN_WAIT_CREATE_SCENE, pstOnlineRole->pRoleData->dwRoleID);
				LOG_CHECK_ERROR(nRetCode == errSuccess);
				if (nRetCode == errSuccess)
				{
					bResult = TRUE;
				}
			}
		}
		break;
	case estGuildDungeon:
		{
			SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(
				pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID, pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID);

			if (pSceneInfo && pSceneInfo->dwCreateTime < pstOnlineRole->dwSaveTime && pSceneInfo->eState == ESCENESTATE_NORMAL)
			{
				LOG_CHECK_ERROR(pSceneInfo->nServerGroupID == nServerGroupID);

				switch (pSceneInfo->eState)
				{
				case ESCENESTATE_NORMAL:
				{
					LOG_PROCESS_ERROR(pstOnlineRole->pUser);

					nRetCode = DoGSPreEnterGS(
						pSceneInfo->dwGSSessionID, pstOnlineRole->pUser, dwRoleID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ,
						pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection);
					LOG_PROCESS_ERROR(nRetCode);

					nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, pSceneInfo);
					LOG_PROCESS_ERROR(nRetCode);

					pSceneInfo->bDelete = FALSE;
					pstOnlineRole->pUser->eState = USER_STATE_LOGIN_PRE_ENTER_GS;
					pstOnlineRole->dwGSSessionID = pSceneInfo->dwGSSessionID;

					bResult = TRUE;
				}
					break;
				case ESCENESTATE_CREATING:
				case ESCENESTATE_CLOSING:
				case ESCENESTATE_DESTROYING:
				default:
					LOG_PROCESS_ERROR(FALSE);
				}
			}
		}
		break;
	default:
		break;
	}

Exit0:
	return bResult;
}

BOOL CMessageHandler::_LoginQuerySceneProc(MS_ONLINE_ROLE_DATA* pstOnlineRole, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pstOnlineRole);
	LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    while (pstOnlineRole->nPosIndex < pstOnlineRole->nPosCount)
    {
        const SCENE_DATA* pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(
            pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID);
		if (pstSceneData)
		{
			nRetCode = _TryLoginQueryScene(pstOnlineRole, dwRoleID, pstSceneData);
			if (nRetCode)
			{
				pstOnlineRole->nPosIndex++;
				break;
			}
			else
			{
				CRI("failed to login scene %d-%d (%d, %d, %d)", 
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY,
					pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ);
			}
		}

        pstOnlineRole->nPosIndex++;
    }

    return TRUE;
Exit0:

    return FALSE;
}

void CMessageHandler::_KickUser(UINT32 dwClientSessionID, INT32 eKickType, 
                                const char* pcszFileName, const char* pcszFuncName, INT32 nLineNum)
{
    INT32 nRetCode = 0;
    MS_USER* poUser = NULL;

    // A missing user is an expected exception
    poUser = CUserMgr::GetInstance().FindUserBySessionID(dwClientSessionID);
    LOG_PROCESS_ERROR(poUser);
    PROCESS_ERROR(!poUser->bKicked);
	PROCESS_ERROR(	poUser->eState != USER_STATE_EXIT_GS_PREENTER &&
					poUser->eState != USER_STATE_JUMP_EXIT_TGT &&
					poUser->eState != USER_STATE_EXIT_GS && 
					poUser->eState != USER_STATE_EXIT_2GS &&
					poUser->eState != USER_STATE_EXIT_DB &&
					poUser->eState != USER_STATE_WAIT_EXIT_DB);

    if (eKickType == ktImmediately)
    {
        CRI("kick user %s session <%08X> immediately at line %d in function %s() file \"%s\"", 
            poUser->szUserName, dwClientSessionID, nLineNum, pcszFuncName, pcszFileName);

        nRetCode = CMessageHandler::GetInstance().DoGTCloseClientSession(dwClientSessionID);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else if (eKickType == ktLogged)
    {
        CRI("kick user %s session <%08X> logged at line %d in function %s() file \"%s\"", 
            poUser->szUserName, dwClientSessionID, nLineNum, pcszFuncName, pcszFileName);
    }
    else
    {
        CRI("kick user %s session <%08X> (unknown kick type) at line %d in function %s() file \"%s\"", 
            poUser->szUserName, dwClientSessionID, nLineNum, pcszFuncName, pcszFileName);
    }
	return;
	
Exit0:
	if (poUser)
	{
		CRI("kick user %s session <%08X> immediately at line %d in function %s() file \"%s\" failed! state = %d",
			poUser->szUserName, dwClientSessionID, nLineNum, pcszFuncName, pcszFileName, poUser->eState);
	}

    return;
}

INT32 CMessageHandler::_CreateNewScene(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, 
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection, INT32 nTargetState, INT32 nSceneID)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    UINT32 dwSuitableGS = 0;
    SCENE_INFO* poSceneInfo = NULL;

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);

    dwSuitableGS = CMSSessionMgr::GetInstance().GetSuitableGS(pstOnlineRole->dwGSSessionID);
    LOG_PROCESS_ERROR(dwSuitableGS > 0);

    poSceneInfo = CSceneMgr::GetInstance().CreateScene(nSceneTempID, GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID), nSceneID);
    LOG_PROCESS_ERROR(poSceneInfo);

    poSceneInfo->dwGSSessionID = dwSuitableGS;
    poSceneInfo->eState = ESCENESTATE_CREATING;
    poSceneInfo->dwCreateTime = CManageServer::GetInstance().GetGameTime();
    poSceneInfo->WaitUserList.empty();

	nRetCode = DoGSCreateScene(dwSuitableGS, poSceneInfo->nServerGroupID, nSceneTempID, poSceneInfo->nSceneID);
    LOG_PROCESS_ERROR(nRetCode);

    poSceneInfo->WaitUserList.push_back(pstOnlineRole->dwCliSessionID);

    if(nTargetState == USER_STATE_JUMP_WAIT_CREATE_SCENE)
	{
		pstOnlineRole->PosList[0].nSceneTempID = nSceneTempID;
		pstOnlineRole->PosList[0].nSceneID = poSceneInfo->nSceneID;
		pstOnlineRole->PosList[0].nX = nDestX;
		pstOnlineRole->PosList[0].nY = nDestY;
		pstOnlineRole->PosList[0].nZ = nDestZ;
		pstOnlineRole->PosList[0].nFaceDirection = nFaceDirection;
		pstOnlineRole->pUser->eState = USER_STATE_JUMP_WAIT_CREATE_SCENE;
		pstOnlineRole->nPosIndex = 1;
		pstOnlineRole->nPosCount = 1;
	}
	else if(nTargetState == USER_STATE_LOGIN_WAIT_CREATE_SCENE)
	{
		pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneTempID = nSceneTempID;
		pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nSceneID = poSceneInfo->nSceneID;
		pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nX = nDestX;
		pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nY = nDestY;
		pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nZ = nDestZ;
		pstOnlineRole->PosList[pstOnlineRole->nPosIndex].nFaceDirection = nFaceDirection;
		pstOnlineRole->pUser->eState = USER_STATE_LOGIN_WAIT_CREATE_SCENE;
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

    nResult = errSuccess;
Exit0:
    return nResult;
}

INT32 CMessageHandler::_ChangeScene(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, INT32 nSceneID, 
                                   INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    SCENE_INFO* poDestSceneInfo = NULL;
    SCENE_INFO* poSrcSceneInfo = NULL;
	const SCENE_DATA* poDestSceneData = NULL;

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);

    poSrcSceneInfo = CSceneMgr::GetInstance().FindScene(pstOnlineRole->pRoleData->nSceneTempID, pstOnlineRole->pRoleData->nSceneID);
    LOG_PROCESS_ERROR_RET_CODE(poSrcSceneInfo, errCritical);

    poDestSceneInfo = CSceneMgr::GetInstance().FindScene(nSceneTempID, nSceneID);
    PROCESS_ERROR_RET_CODE(poDestSceneInfo, errSceneNotExist);
	PROCESS_ERROR_RET_CODE(poDestSceneInfo->nSceneType != estBirth, errCritical);

    pstOnlineRole->PosList[0].nSceneTempID = nSceneTempID;
    pstOnlineRole->PosList[0].nSceneID = poDestSceneInfo->nSceneID;
    pstOnlineRole->PosList[0].nX = nDestX;
    pstOnlineRole->PosList[0].nY = nDestY;
    pstOnlineRole->PosList[0].nZ = nDestZ;
    pstOnlineRole->PosList[0].nFaceDirection = nFaceDirection;
    pstOnlineRole->nPosIndex = 1;
    pstOnlineRole->nPosCount = 1;
        
    //leave estDungeon need leave team DungeonID
    if (poSrcSceneInfo->nSceneType == estDungeon && 
		(poDestSceneInfo->nSceneType == estStatic || poDestSceneInfo->nSceneType == estLine || poDestSceneInfo->nSceneType == estUnique) && 
		pstOnlineRole->pRoleData->dwTeamID > 0)
    {
        CTeam* poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
        LOG_PROCESS_ERROR_RET_CODE(poTeam, errCritical);

        if (pstOnlineRole->pRoleData->dwRoleID == poTeam->GetLeader())
        {
            poTeam->SetDungeonTempID(0);
            poTeam->SetDungeonID(0);
        }
    }

	poDestSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(poDestSceneInfo->nSceneTempID);
	LOG_PROCESS_ERROR(poDestSceneData);

    switch (poDestSceneInfo->eState)
    {
    case ESCENESTATE_CREATING:
        {
            LOG_PROCESS_ERROR_RET_CODE(poDestSceneInfo->WaitUserList.size() < poDestSceneData->nMaxRoleCount, errSceneNoEnterMaxRole);
			poDestSceneInfo->WaitUserList.push_back(pstOnlineRole->dwCliSessionID);
            pstOnlineRole->pUser->eState = USER_STATE_JUMP_WAIT_CREATE_SCENE;
        }
        break;;
    case ESCENESTATE_NORMAL:
        {
            LOG_PROCESS_ERROR_RET_CODE(poDestSceneInfo->nRoleCount < poDestSceneData->nMaxRoleCount, errSceneNoEnterMaxRole);
			if (pstOnlineRole->dwGSSessionID == poDestSceneInfo->dwGSSessionID)
            {
                nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, poDestSceneInfo);
                LOG_PROCESS_ERROR(nRetCode);

                nRetCode = DoGSQuerySceneAck(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, errSuccess, pstOnlineRole->pRoleData->dwRoleID, 
                    nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
                LOG_PROCESS_ERROR(nRetCode);

                nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pstOnlineRole, 
                    pstOnlineRole->pRoleData->nSceneTempID, pstOnlineRole->pRoleData->nSceneID, TRUE);
                LOG_PROCESS_ERROR(nRetCode);

                nRetCode = COnlineRoleDataMgr::GetInstance().EnterScene(pstOnlineRole, poDestSceneInfo);
                LOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pstOnlineRole, poDestSceneInfo);
                LOG_PROCESS_ERROR(nRetCode);
				LOG_PROCESS_ERROR(pstOnlineRole->pUser);

                nRetCode = DoGSPreEnterGS(poDestSceneInfo->dwGSSessionID, pstOnlineRole->pUser, pstOnlineRole->pRoleData->dwRoleID, 
                    nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
                LOG_PROCESS_ERROR(nRetCode);

                pstOnlineRole->dwTargetGSSessionID = poDestSceneInfo->dwGSSessionID;
                pstOnlineRole->pUser->eState = USER_STATE_JUMP_PRE_ENTER_GS;
            }
        }
        break;
	case ESCENESTATE_CLOSING:
    case ESCENESTATE_DESTROYING:
        {
            PROCESS_ERROR_RET_CODE(FALSE, errSceneNotExist);                
        }
        break;
    default:
        {
            LOG_PROCESS_ERROR_RET_CODE(FALSE, errCritical);                
        }
        break;;
    }

    nResult = errSuccess;
Exit0:
    return nResult;
}

INT32 CMessageHandler::_HandleStaticScene(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, INT32 nSceneID,
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(nSceneTempID != 0, errCritical);

    if (nSceneID == 0)
	{
		INT32 nServerGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID);
		nSceneID = (1 << nServerGroupID) | 1;
	}
    
	LOG_PROCESS_ERROR_RET_CODE((nSceneID & 0x00FFFFFF) == 1, errCritical);

    nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
    PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    nResult = errSuccess;
Exit0:
    return nResult;
}

INT32 CMessageHandler::_HandleDungeon(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, INT32 nSceneID,
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(nSceneTempID != 0, errCritical);

    if (pstOnlineRole->pRoleData->dwTeamID > 0)
    {
        // team
        CTeam* poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
        LOG_PROCESS_ERROR_RET_CODE(poTeam, errCritical);

        if (poTeam->GetDungeonTempID() == 0 && poTeam->GetDungeonID() == 0)
        {
            LOG_PROCESS_ERROR(nSceneID == 0);

            // create new scene
            nRetCode = _CreateNewScene(pstOnlineRole, nSceneTempID, nDestX, nDestY, nDestZ, nFaceDirection, USER_STATE_JUMP_WAIT_CREATE_SCENE, 0);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

            poTeam->SetDungeonTempID(pstOnlineRole->PosList[0].nSceneTempID);
            poTeam->SetDungeonID(pstOnlineRole->PosList[0].nSceneID);
        }
        else
        {
            PROCESS_ERROR_RET_CODE(nSceneTempID == poTeam->GetDungeonTempID(), errSceneNoEnterRight);
            if (nSceneID == 0)
                nSceneID = poTeam->GetDungeonID();
            PROCESS_ERROR_RET_CODE(nSceneID == poTeam->GetDungeonID(), errSceneNoEnterRight);

            // enter existed scene
            nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
        }
    }
    else if (nSceneID > 0)
    {
        // enter existed scene
        nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
        PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
    }
    else
    {
        LOG_PROCESS_ERROR(nSceneID == 0);

        // create new scene
        nRetCode = _CreateNewScene(pstOnlineRole, nSceneTempID, nDestX, nDestY, nDestZ, nFaceDirection, USER_STATE_JUMP_WAIT_CREATE_SCENE, 0);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
    }

    nResult = errSuccess;
Exit0:
    return nResult;
}

INT32 CMessageHandler::_HandleSoloDungeon(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, INT32 nSceneID,
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(nSceneTempID != 0, errCritical);

	if (nSceneID > 0)
	{
		// enter existed scene
		nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
		PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	}
	else
	{
		LOG_PROCESS_ERROR(nSceneID == 0);

		// create new scene
		nRetCode = _CreateNewScene(pstOnlineRole, nSceneTempID, nDestX, nDestY, nDestZ, nFaceDirection, USER_STATE_JUMP_WAIT_CREATE_SCENE, 0);
		LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	}

	nResult = errSuccess;
Exit0:
	return nResult;
}

INT32 CMessageHandler::_HandleLineScene(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, INT32 nSceneID, 
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
    INT32 nRetCode = 0;
    INT32 nResult = 0;
	const SCENE_DATA* pstSceneData = NULL;

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(nSceneTempID != 0, errCritical);

	pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTempID);
	LOG_PROCESS_ERROR(pstSceneData);

    if (nSceneID > 0)
    {
        SCENE_INFO* poDestSceneInfo = CSceneMgr::GetInstance().FindScene(nSceneTempID, nSceneID);
		LOG_PROCESS_ERROR_RET_CODE(poDestSceneInfo, errSceneNotExist);
		LOG_PROCESS_ERROR_RET_CODE(poDestSceneInfo->nRoleCount + poDestSceneInfo->WaitUserList.size() < pstSceneData->nMaxRoleCount, errSceneNoEnterMaxRole);

        nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
        PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
    }
    else
    {
        BOOL bHandle = FALSE;

		UINT32 dwLineIDlst[MAX_LINE_SCENE_NUMBER];
		UINT32 nLineIDCount = 0;
		INT32 nServerGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID);

		CTeam* poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
		if (poTeam)
		{
			MS_ONLINE_ROLE_DATA* pLeaderOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleIDInGroup(
				poTeam->GetLeader(), poTeam->GetGroupID());
            if (pLeaderOnlineRole && pLeaderOnlineRole->pRoleData->nSceneTempID == nSceneTempID)
			{
				SCENE_INFO* poDestSceneInfo = CSceneMgr::GetInstance().FindScene(nSceneTempID, pLeaderOnlineRole->pRoleData->nSceneID);
				if (poDestSceneInfo &&
					poDestSceneInfo->nRoleCount + poDestSceneInfo->WaitUserList.size() < pstSceneData->nMaxRoleCount, errSceneNoEnterMaxRole)
				{
					nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, 
						pLeaderOnlineRole->pRoleData->nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
					if (nRetCode)
					{
						bHandle = TRUE;
					}
				}
			}
		}

		nLineIDCount = CSceneMgr::GetInstance().GetLineInfo(nServerGroupID, nSceneTempID, MAX_LINE_SCENE_NUMBER, dwLineIDlst);

        for (INT32 i = 0; i < nLineIDCount && !bHandle; ++i)
        {
            UINT32 dwGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID);
			SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(nSceneTempID, dwLineIDlst[i]);

            if(pSceneInfo && pSceneInfo->eState == ESCENESTATE_NORMAL && pSceneInfo->nRoleCount < (pstSceneData->nMaxRoleCount * 9 / 10))
            {
                nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, pSceneInfo->nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
                PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
                
                bHandle = TRUE;
                break;
            }
			else if(pSceneInfo && pSceneInfo->eState == ESCENESTATE_CREATING && pSceneInfo->WaitUserList.size() < (pstSceneData->nMaxRoleCount * 9 / 10))
			{
				pSceneInfo->WaitUserList.push_back(pstOnlineRole->dwCliSessionID);

				pstOnlineRole->PosList[0].nSceneTempID = nSceneTempID;
				pstOnlineRole->PosList[0].nSceneID = pSceneInfo->nSceneID;
				pstOnlineRole->PosList[0].nX = nDestX;
				pstOnlineRole->PosList[0].nY = nDestY;
				pstOnlineRole->PosList[0].nZ = nDestZ;
				pstOnlineRole->PosList[0].nFaceDirection = nFaceDirection;
				pstOnlineRole->nPosIndex = 1;
				pstOnlineRole->nPosCount = 1;
				pstOnlineRole->pUser->eState = USER_STATE_JUMP_WAIT_CREATE_SCENE;

				bHandle = TRUE;
				break;
			}
        }

        if(!bHandle)
        {
            nRetCode = _CreateNewScene(pstOnlineRole, nSceneTempID, nDestX, nDestY, nDestZ, nFaceDirection, USER_STATE_JUMP_WAIT_CREATE_SCENE, 0);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
        }
    }

    nResult = errSuccess;
Exit0:
    return nResult;
    return errUnknownError;
}

INT32 CMessageHandler::_HandleGuildScene(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, INT32 nSceneID, 
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
	SCENE_INFO* poGuildSceneInfo = NULL;

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(nSceneTempID != 0, errCritical);

	if(nSceneID == 0)
		nSceneID = pstOnlineRole->pRoleData->dwGuildID;

	LOG_PROCESS_ERROR_RET_CODE(nSceneID > 0, errCritical);

	poGuildSceneInfo = CSceneMgr::GetInstance().FindScene(nSceneTempID, nSceneID);
	if(poGuildSceneInfo)
	{
		nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
		PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	}
	else
	{
		nRetCode = _CreateNewScene(pstOnlineRole, nSceneTempID, nDestX, nDestY, nDestZ, nFaceDirection, USER_STATE_JUMP_WAIT_CREATE_SCENE, nSceneID);
		LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	}
	
    nResult = errSuccess;
Exit0:
    return nResult;    
}

INT32 CMessageHandler::_HandleBattlefield(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, INT32 nSceneID, 
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(nSceneTempID != 0, errCritical);
	LOG_PROCESS_ERROR(nSceneID > 0);

	nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
	PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	nResult = errSuccess;

Exit0:
	return nResult;
}

INT32 CMessageHandler::_HandleArena(MS_ONLINE_ROLE_DATA* poOnlineRole, INT32 nSceneTempID, INT32 nSceneID,
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR_RET_CODE(poOnlineRole, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(poOnlineRole->pRoleData, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(nSceneTempID != 0, errCritical);
	LOG_PROCESS_ERROR(nSceneID > 0);

	nRetCode = _ChangeScene(poOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
	PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	nResult = errSuccess;

Exit0:
	return nResult;
}

INT32 CMessageHandler::_HandleGuildDungeon(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nSceneTempID, INT32 nSceneID, 
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	SCENE_INFO* poSceneInfo = NULL;

	LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(nSceneTempID != 0, errCritical);
	LOG_PROCESS_ERROR_RET_CODE(nSceneID > 0, errCritical);

	poSceneInfo = CSceneMgr::GetInstance().FindScene(nSceneTempID, nSceneID);
	if(poSceneInfo)
	{
		nRetCode = _ChangeScene(pstOnlineRole, nSceneTempID, nSceneID, nDestX, nDestY, nDestZ, nFaceDirection);
		PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	}
	else
	{
		nRetCode = _CreateNewScene(pstOnlineRole, nSceneTempID, nDestX, nDestY, nDestZ, nFaceDirection, USER_STATE_JUMP_WAIT_CREATE_SCENE, nSceneID);
		LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	}

	nResult = errSuccess;
Exit0:
	return nResult;
}

BOOL CMessageHandler::STraverseOnlineRole::operator()(UINT32 dwRoleID, MS_ONLINE_ROLE_DATA* poOnlineRole)
{
    INT32 dwGateIndex; 
    LOG_CHECK_ERROR(poOnlineRole);
    if(poOnlineRole)
    {
        dwGateIndex = GET_GT_INDEX_FROM_SESSION_ID(poOnlineRole->dwCliSessionID);
        dwGateSessionList[dwGateIndex][dwClientSessionCount[dwGateIndex]++] = poOnlineRole->dwCliSessionID;
    }
    return TRUE;
}

BOOL CMessageHandler::_DoLoadOrSyncInfo(MS_ONLINE_ROLE_DATA* pstRoleData)
{
	INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pstRoleData);

    nRetCode = DoGSSyncRoleSocialData(pstRoleData);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = SyncMasterDataToGS(*pstRoleData);
    LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMessageHandler::_DoLoginSyncInfo(MS_ONLINE_ROLE_DATA* pstRoleData)
{
    INT32       nRetCode = 0;
    //SArenaTeam* poArenaTeam = NULL;
    CTeam* poTeam = NULL;
    CMsGuild    *poMsGuild = NULL;

    LOG_PROCESS_ERROR(pstRoleData->pRoleData);

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGMData(pstRoleData->dwGSSessionID, pstRoleData->pRoleData->dwRoleID, pstRoleData->pRoleData->dwForbidChatTime);
    LOG_CHECK_ERROR(nRetCode);

    //poArenaTeam = CArenaTeamMgr::GetInstance().FindArenaTeam(pstRoleData->pRoleData->dwArenaTeamID);

    //nRetCode = DoGSSyncRoleArena(pstRoleData->dwGSSessionID, pstRoleData->dwCliSessionID, pstRoleData->pRoleData, poArenaTeam);
    //LOG_CHECK_ERROR(nRetCode);

	poTeam = CTeamMgr::GetInstance().Find(pstRoleData->pRoleData->dwTeamID);
    if(poTeam)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientTeamJoin(pstRoleData->dwCliSessionID, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CMessageHandler::GetInstance().DoClientTeamSyncMemberState(pstRoleData->pRoleData->dwRoleID, etsOnline, poTeam);
        LOG_CHECK_ERROR(nRetCode);
    }
    
    nRetCode = DoClientSelfInfo(pstRoleData);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = DoClientMailNewCount(pstRoleData->dwCliSessionID, pstRoleData->pRoleData->lstMails);
    LOG_CHECK_ERROR(nRetCode);

    //load friends info
    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmtOwned(mysqlSelectFriend,
        [roleID = pstRoleData->pRoleData->dwRoleID, sessionID = pstRoleData->dwCliSessionID]
        (std::list<void*>& rows, void*) { return LoadFriends(rows, roleID, sessionID); },
        "udud", pstRoleData->pRoleData->dwRoleID, pstRoleData->pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    //// load master
    nRetCode = MasterRoleLogin(*pstRoleData);
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = DoClientOccupySabac(pstRoleData->dwCliSessionID, -1, 0,
		CManageServer::GetInstance().GetSabacGuildID(pstRoleData->pRoleData->byGroup));
	LOG_CHECK_ERROR(nRetCode);

    if (pstRoleData->pRoleData->dwGuildID != 0)
    {
        // Synchronize guild information and announcements
        nRetCode = CMsGuildMgr::GetInstance().SyncGuildDataToClient(pstRoleData, pstRoleData->pRoleData->dwGuildID);
        LOG_CHECK_ERROR(nRetCode);

        // sync to member
        poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstRoleData->pRoleData->dwGuildID);
        LOG_CHECK_ERROR(poMsGuild);
        if (poMsGuild)
        {
            nRetCode = DoClientSyncSocialAttrNtf(poMsGuild, pstRoleData->pRoleData->dwRoleID, 
                pstRoleData->pRoleData->byLevel, pstRoleData->pRoleData->nSceneTempID);
            LOG_CHECK_ERROR(nRetCode);
        }

		if (poMsGuild->IsOccupiedSabac() && pstRoleData->pRoleData->dwRoleID == poMsGuild->GetLeaderID())
		{
			nRetCode = DoClientSabacWinnerLogin(pstRoleData);
			LOG_CHECK_ERROR(nRetCode);
		}
    }

	nRetCode = DoClientErectStatue(pstRoleData->pRoleData->byGroup, pstRoleData->dwCliSessionID);
	LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
//
//BOOL CMessageHandler::_PreEnterScene(MS_ONLINE_ROLE_DATA* pstRoleData, SCENE_INFO* pstSceneInfo, UINT8 bySide)
//{
//    LOG_PROCESS_ERROR(pstRoleData);
//    LOG_PROCESS_ERROR(pstSceneInfo);
//
//    pstRoleData->nPreEnterSceneTempID = pstSceneInfo->nSceneTempID;
//    pstRoleData->nPreEnterSceneID = pstSceneInfo->nSceneID;
//
//    pstSceneInfo->nRoleCount++;
//
//	if(pstSceneInfo->bySideCount > 0)
//	{
//		LOG_PROCESS_ERROR(bySide <= pstSceneInfo->bySideCount);
//		pstRoleData->byPreEnterSide = bySide;
//	}
//	else
//	{
//		pstRoleData->byPreEnterSide = 0;
//	}
//
//    return TRUE;
//Exit0:
//    return FALSE;
//}
//
//BOOL CMessageHandler::_EnterScene(MS_ONLINE_ROLE_DATA* pstRoleData, SCENE_INFO* pstSceneInfo)
//{
//	INT32 nRetCode = 0;
//
//    LOG_PROCESS_ERROR(pstRoleData);
//    LOG_PROCESS_ERROR(pstSceneInfo);
//
//    LOG_PROCESS_ERROR(pstRoleData->nPreEnterSceneTempID == pstSceneInfo->nSceneTempID);
//    LOG_PROCESS_ERROR(pstRoleData->nPreEnterSceneID == pstSceneInfo->nSceneID);
//
//    pstRoleData->pRoleData->nSceneTempID = pstSceneInfo->nSceneTempID;
//    pstRoleData->pRoleData->nSceneID = pstSceneInfo->nSceneID;
//	pstRoleData->pRoleData->bySide = pstRoleData->byPreEnterSide;
//
//    pstRoleData->nPreEnterSceneTempID = pstRoleData->nPreEnterSceneID = 0;
//	pstRoleData->byPreEnterSide = 0;
//
//	LOG_PROCESS_ERROR(pstRoleData->pPrev == NULL);
//	LOG_PROCESS_ERROR(pstRoleData->pNext == NULL);
//	pstSceneInfo->lstOnlineRole.PushRear(pstRoleData);
//	
//
//	nRetCode = DoDLEnterScene(CManageServer::GetInstance().GetGameTime(), g_ServerConfig.Common.nAreaID, 
//        pstRoleData->pRoleData->byGroup, pstSceneInfo->nSceneID, pstSceneInfo->nSceneTempID, 
//        pstSceneInfo->nSceneLevel, pstSceneInfo->nSceneDifficulty, pstRoleData->pRoleData->dwRoleID);
//	LOG_CHECK_ERROR(nRetCode);
//
//    return TRUE;
//Exit0:
//    return FALSE;
//}
//
//BOOL CMessageHandler::_LeaveScene(MS_ONLINE_ROLE_DATA* pstRoleData, SCENE_INFO* pstSceneInfo, BOOL bNormalLeave)
//{
//	INT32 nRetCode = 0;
//	LOG_PROCESS_ERROR(pstRoleData);
//    LOG_PROCESS_ERROR(pstSceneInfo);
//
//	if(pstRoleData->pRoleData->nSceneTempID == pstSceneInfo->nSceneTempID && pstRoleData->pRoleData->nSceneID == pstSceneInfo->nSceneID)
//	{
//		if(pstSceneInfo->bySceneType == estArena)
//		{
//			nRetCode = CArenaQueueMgr::GetInstance().LeaveArena(pstRoleData->pRoleData->dwRoleID, pstSceneInfo->nSceneTempID, pstSceneInfo->nSceneID, 
//				pstRoleData->byPreEnterSide, bNormalLeave);
//			LOG_CHECK_ERROR(nRetCode);
//		}
//		else if(pstSceneInfo->bySceneType == estBattleField)
//		{
//			nRetCode = CBattleQueueMgr::GetInstance().LeaveBattle(pstRoleData->pRoleData->dwRoleID, pstSceneInfo->nSceneTempID, pstSceneInfo->nSceneID, 
//				pstRoleData->pRoleData->bySide, bNormalLeave);
//			LOG_CHECK_ERROR(nRetCode);
//		}
//
//		pstRoleData->pRoleData->bySide = 0;
//		pstRoleData->pRoleData->nSceneTempID = pstRoleData->pRoleData->nSceneID = 0;	
//	}
//	else if(pstRoleData->nPreEnterSceneTempID == pstSceneInfo->nSceneTempID && pstRoleData->nPreEnterSceneID == pstSceneInfo->nSceneID)
//	{
//		if(pstSceneInfo->bySceneType == estArena)
//		{
//			nRetCode = CArenaQueueMgr::GetInstance().LeaveArena(pstRoleData->pRoleData->dwRoleID, pstSceneInfo->nSceneTempID, pstSceneInfo->nSceneID, 
//				pstRoleData->byPreEnterSide, bNormalLeave);
//			LOG_CHECK_ERROR(nRetCode);
//		}
//		else if(pstSceneInfo->bySceneType == estBattleField)
//		{
//			nRetCode = CBattleQueueMgr::GetInstance().LeaveBattle(pstRoleData->pRoleData->dwRoleID, pstSceneInfo->nSceneTempID, pstSceneInfo->nSceneID, 
//				pstRoleData->byPreEnterSide, bNormalLeave);
//			LOG_CHECK_ERROR(nRetCode);
//		}
//
//		pstRoleData->byPreEnterSide = 0;
//		pstRoleData->nPreEnterSceneTempID = pstRoleData->nPreEnterSceneID = 0;
//	}
//    else
//        LOG_PROCESS_ERROR(FALSE);
//
//    pstSceneInfo->nRoleCount--;
//	if(pstRoleData->pPrev != NULL || pstRoleData->pNext != NULL)
//		pstSceneInfo->lstOnlineRole.Remove(pstRoleData);
//
//	nRetCode = DoDLLeaveScene(CManageServer::GetInstance().GetGameTime(), g_ServerConfig.Common.nAreaID, 
//        pstRoleData->pRoleData->byGroup, pstSceneInfo->nSceneID, pstSceneInfo->nSceneTempID, 
//        pstSceneInfo->nSceneLevel, pstSceneInfo->nSceneDifficulty, pstRoleData->pRoleData->dwRoleID);
//	LOG_CHECK_ERROR(nRetCode);
//
//    return TRUE;
//Exit0:
//    return FALSE;
//
//}   

BOOL CMessageHandler::_ShutdownClient(MS_USER* poUser)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poUser);

    switch(poUser->eState)
    {
    case USER_STATE_AUTHED:
        {
            LOG_CHECK_ERROR(poUser->dwCurRoleID == 0);

			CUserMgr::GetInstance().RemoveWaitUser(poUser);

            nRetCode = CUserMgr::GetInstance().DelUser(poUser);
            LOG_CHECK_ERROR(nRetCode);
        }
        break;
    case USER_STATE_QUEUEFINISHED:
	case USER_STATE_LOGIN_WAIT_CREATE_SCENE:
        {
			MS_ONLINE_ROLE_DATA* pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
			if (pstOnlineRole)
			{
				nRetCode = COnlineRoleDataMgr::GetInstance().DelOnlineRole(pstOnlineRole);
				LOG_CHECK_ERROR(nRetCode);
			}

            nRetCode = CMessageHandler::GetInstance().DoDBLeaveDB(poUser->dwCliSessionID);
            LOG_PROCESS_ERROR(nRetCode);

            poUser->eState = USER_STATE_EXIT_DB;
        }
        break;
    case USER_STATE_LOGIN_PRE_ENTER_GS:
    case USER_STATE_LOGIN_GSREADY:
    case USER_STATE_JUMP_GSREADY:
        {
            MS_ONLINE_ROLE_DATA* pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
            LOG_PROCESS_ERROR(pstOnlineRole);
            LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

            nRetCode = CMessageHandler::GetInstance().DoGSLeaveGS(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);

            if (nRetCode)
                poUser->eState = USER_STATE_EXIT_GS_PREENTER;
            else
            {
                poUser->eState = USER_STATE_WAIT_EXIT_DB;
				poUser->qwTimer = CManageServer::GetInstance().GetGameTick() + WAIT_EXIT_DB_TIME;
                CMSSessionMgr::GetInstance().AddToDBWaitList(poUser);
            }
        }
		break;
    case USER_STATE_INGAME:
    case USER_STATE_JUMP_WAIT_CREATE_SCENE:
        {
            MS_ONLINE_ROLE_DATA* pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
            LOG_PROCESS_ERROR(pstOnlineRole);
            LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

            nRetCode = CMessageHandler::GetInstance().DoGSLeaveGS(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);

            if (nRetCode)
                poUser->eState = USER_STATE_EXIT_GS;
            else
            {
                poUser->eState = USER_STATE_WAIT_EXIT_DB;
				poUser->qwTimer = CManageServer::GetInstance().GetGameTick() + WAIT_EXIT_DB_TIME;
                CMSSessionMgr::GetInstance().AddToDBWaitList(poUser);
            }
        }
        break;
    case USER_STATE_JUMP_PRE_ENTER_GS:
        {
            MS_ONLINE_ROLE_DATA* pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
            LOG_PROCESS_ERROR(pstOnlineRole);
            LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

            nRetCode = CMessageHandler::GetInstance().DoGSLeaveGS(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);

            if (nRetCode)
            {
                nRetCode = CMessageHandler::GetInstance().DoGSLeaveGS(pstOnlineRole->dwTargetGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
                LOG_CHECK_ERROR(nRetCode);

                if (nRetCode)
                    poUser->eState = USER_STATE_EXIT_2GS;
				else
					poUser->eState = USER_STATE_EXIT_GS;
            }
            else
            {
                nRetCode = CMessageHandler::GetInstance().DoGSLeaveGS(pstOnlineRole->dwTargetGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
                LOG_CHECK_ERROR(nRetCode);

				if (nRetCode)
					poUser->eState = USER_STATE_JUMP_EXIT_TGT;
                else
                {
                    poUser->eState = USER_STATE_WAIT_EXIT_DB;
					poUser->qwTimer = CManageServer::GetInstance().GetGameTick() + WAIT_EXIT_DB_TIME;
                    CMSSessionMgr::GetInstance().AddToDBWaitList(poUser);
                }
            }
        }
        break;
    case USER_STATE_JUMP_EXIT_SRC:
        {
            MS_ONLINE_ROLE_DATA* pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
            LOG_PROCESS_ERROR(pstOnlineRole);
            LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

            nRetCode = CMessageHandler::GetInstance().DoGSLeaveGS(
                pstOnlineRole->dwTargetGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);

            if (nRetCode)
                poUser->eState = USER_STATE_EXIT_2GS;
			else
			{
				//pstOnlineRole->dwTargetGSSessionID = 0;
				poUser->eState = USER_STATE_EXIT_GS;
			}
        }
        break;
    case USER_STATE_JUMP_ENTER_TARGET:
        {
            MS_ONLINE_ROLE_DATA* pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
            LOG_PROCESS_ERROR(pstOnlineRole);
            LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

            nRetCode = CMessageHandler::GetInstance().DoGSLeaveGS(
                pstOnlineRole->dwTargetGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);

            if (nRetCode)
                poUser->eState = USER_STATE_EXIT_GS_PREENTER;
            else
            {
                poUser->eState = USER_STATE_WAIT_EXIT_DB;
				poUser->qwTimer = CManageServer::GetInstance().GetGameTick() + WAIT_EXIT_DB_TIME;
                CMSSessionMgr::GetInstance().AddToDBWaitList(poUser);
            }
        }
        break;
    case USER_STATE_EXIT_GS_RETURN:
        {
            poUser->eState = USER_STATE_EXIT_GS;
        }
        break;
    case USER_STATE_EXIT_DB_RETURN:
        {
            poUser->eState = USER_STATE_EXIT_DB;
        }
        break;
    case USER_STATE_WAIT_EXIT_DB:
    case USER_STATE_EXIT_GS_PREENTER:
    case USER_STATE_EXIT_GS:
    case USER_STATE_JUMP_EXIT_TGT:
    case USER_STATE_EXIT_2GS:
    case USER_STATE_EXIT_DB:
        break;
    default:
        LOG_PROCESS_ERROR(0);
        break;
    }

    return TRUE;
Exit0:
	MS_SYS_LOG_ERROR(poUser);
    return FALSE;
}

BOOL CMessageHandler::CKickAllUserOnGT::operator()(UINT32 dwClientSessionID, MS_USER* poUser)
{
    INT32 nRetCode = 0;

    if (GET_GT_INDEX_FROM_SESSION_ID(dwClientSessionID) == m_dwGTSessionID)
    {
        INF("CKickAllUserOnGT user %s <%08X> on GT %d", poUser->szUserName, poUser->dwCliSessionID, m_dwGTSessionID);

        nRetCode = CMessageHandler::GetInstance()._ShutdownClient(poUser);
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return TRUE;
}

BOOL CMessageHandler::CKickAllRoleOnGS::operator()(UINT32 dwRoleID, MS_ONLINE_ROLE_DATA* pOnlineRole)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pOnlineRole);

    //if (pOnlineRole->dwGSSessionID == m_dwGSSessionID)
    {
        MS_USER* poUser = pOnlineRole->pUser;
        LOG_PROCESS_ERROR( poUser );

        switch(poUser->eState) 
        {
        case USER_STATE_AUTHED:
        case USER_STATE_QUEUEFINISHED:
		case USER_STATE_LOGIN_WAIT_CREATE_SCENE:
			LOG_PROCESS_ERROR(pOnlineRole->dwGSSessionID == 0);
			break;

        case USER_STATE_LOGIN_PRE_ENTER_GS:
        case USER_STATE_LOGIN_GSREADY:
		case USER_STATE_INGAME:
        case USER_STATE_JUMP_WAIT_CREATE_SCENE: 
		case USER_STATE_JUMP_GSREADY:		// cur has, target no		// cur gs = target gs; target gs = 0
		case USER_STATE_EXIT_GS_PREENTER:	// cur has, target no
		case USER_STATE_EXIT_GS:			// cur has, target no
		case USER_STATE_EXIT_GS_RETURN:		// cur has, target no
			if (pOnlineRole->dwGSSessionID == m_dwGSSessionID)
			{
				LOG_CHECK_ERROR(pOnlineRole->dwTargetGSSessionID == 0);
				pOnlineRole->dwTargetGSSessionID = 0;

				INF("CKickAllRoleOnGS user %s <%08X> eState=%d on GS %d", 
					poUser->szUserName, poUser->dwCliSessionID, poUser->eState, pOnlineRole->dwGSSessionID);

				nRetCode = CMessageHandler::GetInstance().DoGTCloseClientSession(pOnlineRole->dwCliSessionID);
				LOG_CHECK_ERROR(nRetCode);

				poUser->eState = USER_STATE_WAIT_EXIT_DB;
				poUser->qwTimer = GetTickCountEx() + WAIT_EXIT_DB_TIME;
				CMSSessionMgr::GetInstance().AddToDBWaitList(poUser);
			}
			break;

		case USER_STATE_JUMP_PRE_ENTER_GS:	// cur has, target has		// target gs = x
		case USER_STATE_JUMP_EXIT_SRC:		// cur has, target has
		case USER_STATE_EXIT_2GS:			// cur has, target has
			if (pOnlineRole->dwGSSessionID == m_dwGSSessionID ||
				pOnlineRole->dwTargetGSSessionID == m_dwGSSessionID)
			{
				INF("CKickAllRoleOnGS user %s <%08X> eState=%d on GS %d and Target GS", 
					poUser->szUserName, poUser->dwCliSessionID, poUser->eState, pOnlineRole->dwGSSessionID, pOnlineRole->dwTargetGSSessionID);

				nRetCode = CMessageHandler::GetInstance().DoGTCloseClientSession(pOnlineRole->dwCliSessionID);
				LOG_CHECK_ERROR(nRetCode);

				poUser->eState = USER_STATE_WAIT_EXIT_DB;
				poUser->qwTimer = GetTickCountEx() + WAIT_EXIT_DB_TIME;
				CMSSessionMgr::GetInstance().AddToDBWaitList(poUser);
			}
			break;

		case USER_STATE_JUMP_EXIT_TGT:		// cur no or disconnect, target has
		case USER_STATE_JUMP_ENTER_TARGET:	// cur no, target has
			if (pOnlineRole->dwTargetGSSessionID == m_dwGSSessionID)
			{
				LOG_CHECK_ERROR(pOnlineRole->dwGSSessionID == 0);
				pOnlineRole->dwGSSessionID = 0;

				INF("CKickAllRoleOnGS user %s <%08X> eState=%d on Target GS %d", 
					poUser->szUserName, poUser->dwCliSessionID, poUser->eState, pOnlineRole->dwTargetGSSessionID);

				nRetCode = CMessageHandler::GetInstance().DoGTCloseClientSession(pOnlineRole->dwCliSessionID);
				LOG_CHECK_ERROR(nRetCode);

				poUser->eState = USER_STATE_WAIT_EXIT_DB;
				poUser->qwTimer = GetTickCountEx() + WAIT_EXIT_DB_TIME;
				CMSSessionMgr::GetInstance().AddToDBWaitList(poUser);
			}
			break;

        case USER_STATE_EXIT_DB_RETURN:
        case USER_STATE_WAIT_EXIT_DB:
        case USER_STATE_EXIT_DB:
            {
                // do nothing
            }
            break;
        default:
            LOG_PROCESS_ERROR(FALSE);
        }
    }

Exit0:
    return TRUE;
}

BOOL CMessageHandler::CDeleteAllSceneOnGS::operator()(UINT64 dwSceneID, SCENE_INFO* pSceneInfo)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pSceneInfo);

    if (pSceneInfo->dwGSSessionID == m_dwGSSessionID)
    {
		nRetCode = CSceneMgr::GetInstance().DestroyScene(pSceneInfo->nSceneTempID, pSceneInfo->nSceneID,
			pSceneInfo->nServerGroupID);
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit0:

    return TRUE;
}

BOOL CMessageHandler::_CallDebugGMCommand(MS_ONLINE_ROLE_DATA* pstOnlineRole, const char* pcszCommand)
{
	INT32       nRetCode = 0;
	CLuaScript* poScript = NULL;
	BOOL        bGMRet = FALSE;
    UINT32      dwTime = CManageServer::GetInstance().GetGameTime();

    poScript = CScriptMgr::GetInstance().FindScript(MS_GM_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    if (strnicmp(pcszCommand, "reload", sizeof("reload") - 1) == 0)
    {
        if (strlen(pcszCommand) > sizeof("reload"))
            CManageServer::GetInstance().Reload(pcszCommand + sizeof("reload"));
        else
            CManageServer::GetInstance().Reload("all");
    }

    lua_pushnumber(poScript->GetLuaState(), pstOnlineRole->pRoleData->dwRoleID);
    lua_setglobal(poScript->GetLuaState(), "SelfRoleID");

    nRetCode = poScript->CallFunction("OnMSGMCommand", "sds:b", pcszCommand,
        pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->szRoleName, &bGMRet);
    LOG_PROCESS_ERROR(nRetCode);
    LOG_PROCESS_ERROR(bGMRet);

    if (bGMRet)
    {
        nRetCode = DoDLGmCommand(pstOnlineRole->pRoleData->byGroup,
            pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->szRoleName,
            CManageServer::GetInstance().GetGameTime(), pcszCommand, TRUE);
        LOG_CHECK_ERROR(nRetCode);
    }

	INF("role %s executed gm command %s", pstOnlineRole->pRoleData->szRoleName, pcszCommand);

    nRetCode = DoGMStrCmdLog(dwTime, pstOnlineRole->pRoleData->byGroup,
        pstOnlineRole->pUser->dwClientIp, pstOnlineRole->pRoleData->dwRoleID,
        pstOnlineRole->pRoleData->szRoleName, pcszCommand, errSuccess);
    LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	INF("role %s failed to execute gm command %s", pstOnlineRole->pRoleData->szRoleName, pcszCommand);

    nRetCode = DoGMStrCmdLog(dwTime, pstOnlineRole->pRoleData->byGroup,
        pstOnlineRole->pUser->dwClientIp, pstOnlineRole->pRoleData->dwRoleID,
        pstOnlineRole->pRoleData->szRoleName, pcszCommand, errGMStrCommandFailed);
    LOG_CHECK_ERROR(nRetCode);

	return FALSE;
}

BOOL CMessageHandler::_CallFinalGMCommand(MS_ONLINE_ROLE_DATA* pstOnlineRole, const char* pcszCommand, INT32 nGMFrom)
{
	INT32   nRetCode = 0;
	char    szInfo[256] = { 0 };
	CLuaScript* poScript = NULL;
	BOOL    bGMRet = FALSE;
	INT32   nGMLevel = 100;
	UINT32  dwRoleID = 0;
    UINT32  dwTime = CManageServer::GetInstance().GetGameTime();
	const char* pcszRoleName = "guard";

	LOG_PROCESS_ERROR( nGMFrom == stGD || nGMFrom == stGM || nGMFrom == stClient);

	if (pstOnlineRole)
	{
		LOG_PROCESS_ERROR(nGMFrom == stClient);
		LOG_PROCESS_ERROR(pstOnlineRole->pUser->bUserWhiteList);
		LOG_PROCESS_ERROR(pstOnlineRole->pUser->bIpWhiteList);
		nGMLevel = pstOnlineRole->nGMLevel;
		dwRoleID = pstOnlineRole->pRoleData->dwRoleID;
		pcszRoleName = pstOnlineRole->pRoleData->szRoleName;
	}

	LOG_PROCESS_ERROR(nGMLevel > 0);

	poScript = CScriptMgr::GetInstance().FindScript(MS_CS_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	if (strnicmp(pcszCommand, "reload", sizeof("reload") - 1) == 0)
	{
		LOG_PROCESS_ERROR(nGMLevel == MAX_GM_LEVEL);

		if (strlen(pcszCommand) > sizeof("reload"))
			CManageServer::GetInstance().Reload(pcszCommand + sizeof("reload"));
		else
			CManageServer::GetInstance().Reload("all");

		nRetCode = DoGSCallGMCommand(0, pcszCommand);
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = DoGTGMCommand(pcszCommand);
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = DoDBGMCommand(pcszCommand);
		LOG_CHECK_ERROR(nRetCode);
	}
	else if (strnicmp(pcszCommand, "run", sizeof("run") - 1) == 0)	// must from guard, which transfer from maintain tool
	{
		LOG_PROCESS_ERROR(nGMLevel == MAX_GM_LEVEL);

		if (strlen(pcszCommand) > sizeof("run"))
		{
			nRetCode = poScript->CallFunction("OnRunScript", "sd:b", pcszCommand + sizeof("run"), nGMFrom, &bGMRet);
			LOG_CHECK_ERROR(nRetCode && bGMRet);
		}

		nRetCode = DoGSCallGMCommand(0, pcszCommand);
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = DoGTGMCommand(pcszCommand);
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = DoDBGMCommand(pcszCommand);
		LOG_CHECK_ERROR(nRetCode);
	}
	else
	{
		nRetCode = poScript->CallFunction("OnMSCommand", "sddds:b", pcszCommand, nGMLevel, nGMFrom,
			dwRoleID, pcszRoleName, &bGMRet);
		LOG_PROCESS_ERROR(nRetCode && bGMRet);
	}

	INF("role %s:%d executed gm command %s", pcszRoleName, dwRoleID, pcszCommand);

    if (pstOnlineRole)
    {
        nRetCode = DoGMStrCmdLog(dwTime, pstOnlineRole->pRoleData->byGroup,
            pstOnlineRole->pUser->dwClientIp, pstOnlineRole->pRoleData->dwRoleID,
            pstOnlineRole->pRoleData->szRoleName, pcszCommand, errSuccess);
        LOG_CHECK_ERROR(nRetCode);
    }

	_snprintf(szInfo, sizeof(szInfo), "role %s:%d success to execute gm command %s", pcszRoleName, dwRoleID, pcszCommand);
	DoGDCommandAck(szInfo);
	INF(szInfo);
	
	return TRUE;
Exit0:
    if (pstOnlineRole)
    {
        nRetCode = DoGMStrCmdLog(dwTime, pstOnlineRole->pRoleData->byGroup,
            pstOnlineRole->pUser->dwClientIp, pstOnlineRole->pRoleData->dwRoleID,
            pstOnlineRole->pRoleData->szRoleName, pcszCommand, errGMStrCommandFailed);
        LOG_CHECK_ERROR(nRetCode);
    }

	_snprintf(szInfo, sizeof(szInfo), "role %s:%d failed to execute gm command %s", pcszRoleName, dwRoleID, pcszCommand);
	DoGDCommandAck(szInfo);
	INF(szInfo);

	return FALSE;
}

BOOL CMessageHandler::DoNSMessageVersion(void)
{
    INT32 nRetCode = 0;

    X2N_MESSAGE_VERSION* pMsg = (X2N_MESSAGE_VERSION*)m_pPacketAllocator->Alloc(sizeof(X2N_MESSAGE_VERSION));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = x2n_message_version;
	pMsg->byVersion = NS_MESSAGE_VERSION;

    nRetCode = _SendNS((PACKET_BUFFER*)pMsg, sizeof(X2N_MESSAGE_VERSION));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoNSNewName(INT32 nNameType, UINT32 dwSessionID, const char* szName, const char* UserData, UINT32 dwUserDataLen)
{
    INT32 nRetCode = 0;

    X2N_NEW_NAME* pMsg = (X2N_NEW_NAME*)m_pPacketAllocator->Alloc(sizeof(X2N_NEW_NAME));
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(dwUserDataLen <= USER_DATA_LEN);

    pMsg->wMsg = x2n_new_name;
	pMsg->dwSessionID = dwSessionID;
    pMsg->byType = nNameType;
    strncpy(pMsg->szName, szName, sizeof(pMsg->szName) - 1);
    memcpy(pMsg->UserData, UserData, dwUserDataLen);

    nRetCode = _SendNS((PACKET_BUFFER*)pMsg, sizeof(X2N_NEW_NAME));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

/************************************************************************/
// From NS
void CMessageHandler::OnNSNewNameResult(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    N2X_NEW_NAME_RESULT* pMsg = (N2X_NEW_NAME_RESULT*)pPacketBuffer;

	if (pMsg->byType == ntGuildName)
	{
		CreateGuildParam* pCgp = (CreateGuildParam*)pMsg->UserData;
		LOG_PROCESS_ERROR(pCgp);

		MS_ONLINE_ROLE_DATA *pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pCgp->dwRoleID);
		LOG_PROCESS_ERROR(pOnlineRole);

		DBG("OnNSNewNameResult Name:%s, ErrCode:%d, ID:%u", pMsg->szName, pMsg->nErrorCode, pMsg->dwID);

		if (pMsg->nErrorCode != errNsSuccess)
		{
            nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, errGuildSameName, 0, 0);
            LOG_CHECK_ERROR(nResult);
			PROCESS_ERROR(FALSE);
		}

		nResult = CMsGuildMgr::GetInstance().PreCreateGuild(pMsg->dwID, *pOnlineRole, pMsg->szName, pCgp->szAnnounce, pCgp->bConsumeItem);
		if (nResult != errUnknownError && nResult != errSuccess)
		{
			nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
			LOG_CHECK_ERROR(nResult);
		}
	}
	else if (pMsg->byType == ntRoleName)
	{
		nRetCode = DoGTCheckNewName(pMsg->dwSessionID, pMsg->nErrorCode, pMsg->dwID, pMsg->szName, pMsg->UserData, sizeof(pMsg->UserData));
		LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);
	}

    return;
Exit0:
    return;
}

void CMessageHandler::OnMMRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	SMMSession* pMMSession{};
	INT32 nRetCode = 0;
	MM2M_REGISTER* pMsg = (MM2M_REGISTER*)pPacketBuffer;

	//check
	SMMSession& rMMSession = CMSSessionMgr::GetInstance().GetMMSession();
	LOG_PROCESS_ERROR(rMMSession.pSocketSession == pSocketSession);

	pMMSession = (SMMSession*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pMMSession);
	LOG_PROCESS_ERROR(pMMSession->pSocketSession == pSocketSession);
	LOG_PROCESS_ERROR(CMSSessionMgr::GetInstance().GetServerState() != sgsClosing);

	m_ServerInfo.MM.dwAddr = pMsg->dwAddr;
	m_ServerInfo.MM.wPort = pMsg->wPort;

	nRetCode = DoMMRegisterAck();
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CMessageHandler::OnMMCloseAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	SMMSession* pMMSession = (SMMSession*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pMMSession);

	return;
Exit0:
	if (pSocketSession)
		pSocketSession->Close();
	return;
}

void CMessageHandler::OnMMReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;

	m_ServerInfo.MM.byReady = true;
	INF("MM is ready");

	nRetCode = DoServerInfo();
	LOG_PROCESS_ERROR(nRetCode);

	_CheckAllReady();
Exit0:
	return;
}

BOOL CMessageHandler::DoMMRegisterAck(void)
{
	INT32 nRetCode = 0;

	M2MM_REGISTER_ACK* pMsg = (M2MM_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2MM_REGISTER_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2mm_register_ack;

	nRetCode = _SendMM((PACKET_BUFFER*)pMsg, sizeof(M2MM_REGISTER_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}
