#include "stdafx.h"
#include "message_handler.h"

#include "server_config.h"

#include "gm_svr_def.h"
#include "gm_session_mgr.h"
#include "common_message.h"
#include "gmsvr_message.h"
#include "gmtl_message.h"
#include "time_ex.h"
#include "ms_mgr.h"
#include "user_manager.h"
#include "permission_mgr.h"
#include "kernel.h"
#include "cmd_mgr.h"

#include "gm_def.h"
#include "gm_data.h"
#include "gmserver.h"
#include "gm_role_mgr.h"
#include <time.h>
#include "gm_log.h"
#include "item_data.h"
#include "gm_version.h"
#include "dump.h"
#include "cmd_mgr.h"
#include "ld_listener.h"
#include "db_mgr.h"
#include "user_manager.h"
#include "../manageserver/mail/mail.h"
#include "item.h"
#include "game_world.h"

CMessageHandler CMessageHandler::ms_Instance;

using namespace InternalMessage;

void CMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(pSocketSession);
    LOG_PROCESS_ERROR(pSocketAcceptor);

    if (pSocketAcceptor == m_poTLAcceptor)
    {
        STLSession *poTLSession = CGMSessionMgr::GetInstance().NewTLSession();
        LOG_PROCESS_ERROR(poTLSession);

        poTLSession->pSocketSession = pSocketSession;
        pSocketSession->SetUserData(poTLSession);
        poTLSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
        poTLSession->dwTLUserID = 0;
    }
    else if (pSocketAcceptor == m_poMSAcceptor)
    {
        SMSSession  *pMSSession = CGMSessionMgr::GetInstance().GetMSSession();
        LOG_PROCESS_ERROR(pMSSession);
        LOG_PROCESS_ERROR(pMSSession->pSocketSession == NULL);

        pMSSession->pSocketSession = pSocketSession;
        pSocketSession->SetUserData(pMSSession);
        pMSSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
    }
    else
        LOG_PROCESS_ERROR(FALSE);

    return ;
Exit0:
    return ;
}

void CMessageHandler::OnConnected(ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    SGMSession* pSession = NULL;

    LOG_PROCESS_ERROR(pSocketSession);

    pSession = (SGMSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pSession);

    pSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

    switch (pSession->dwType)
    {
    case stGD:
    {
        SGDSession* pGDSession = (SGDSession*)pSession;
        pGDSession->bConnected = true;
        pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nRegisterGuardTimeout;
        INF("GD connected");

        if (CGMSessionMgr::GetInstance().GetServerState() == sgsSelfReady ||
            CGMSessionMgr::GetInstance().GetServerState() == sgsAllReady)
        {
            nRetCode = DoGDRegister();
            LOG_PROCESS_ERROR(nRetCode);
        }
    }
        break;
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

void CMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
    SGMSession  *pMSSession = NULL;

    MESSAGE_HEADER  *pHeader = (MESSAGE_HEADER*)pPacketBuffer;
    LOG_PROCESS_ERROR(pHeader);
    //LOG_PROCESS_ERROR(m_InternalMessageFuncList[pHeader->wMsg]);

    LOG_PROCESS_ERROR(pSocketSession);

    if (pHeader->wMsg > tl2gm_logout && pHeader->wMsg < tl_end)
    {
        STLSession          *pTLSession = NULL;
        CTLUser             *pTLUser = NULL;

        LOG_PROCESS_ERROR(m_GmtlMessageFuncList[pHeader->wMsg]);

        pTLSession = (STLSession *)pSocketSession->GetUserData();
        LOG_PROCESS_ERROR(pTLSession);
        LOG_PROCESS_ERROR(pTLSession->pSocketSession);
        LOG_PROCESS_ERROR(pTLSession->eState == egmssNormal);

        pTLUser = CUserManager::GetInstance().FindUserByID(pTLSession->dwTLUserID);
        LOG_PROCESS_ERROR(pTLUser);
        __try
        {
            (this->*m_GmtlMessageFuncList[pHeader->wMsg])(pTLUser, (char *)pPacketBuffer);
        }
        __except (DumpExceptionFilter(GetExceptionInformation()))
        {
            CRI("failed to process internal message id %d size %d", pHeader->wMsg, dwSize);
        }
        
    }
    else
    {
        LOG_PROCESS_ERROR(m_InternalMessageFuncList[pHeader->wMsg]);
        __try
        {
            (this->*m_InternalMessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize, pSocketSession);
        }
        __except (DumpExceptionFilter(GetExceptionInformation()))
        {
            CRI("failed to process internal message id %d size %d", pHeader->wMsg, dwSize);
        }
    }

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
    if (pSocketSession)
        pSocketSession->Close();

    return;
}

void CMessageHandler::OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown)
{
    INT32       nRetCode = 0;
    SGMSession  *pGMSession = NULL;

    LOG_PROCESS_ERROR(pSocketSession);

    pGMSession = (SGMSession*)pSocketSession->GetUserData();
    PROCESS_ERROR(pGMSession);

    pGMSession->pSocketSession = NULL;

    switch(pGMSession->dwType)
    {
    case stGD:
    {
        SGDSession* pGDSession = (SGDSession*)pGMSession;
        pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
        pGDSession->bConnected = false;
        CRI("GD disconnected");
    }
        break;
    case stMS:
        {
            SMSSession  *pMSSession = (SMSSession*)pGMSession;
            pMSSession->pSocketSession = NULL;
            pMSSession->qwTimer = 0xFFFFFFFF;
            pMSSession->eState = egmssDisconnected;

            CRI("MS disconnected");
        }
        break;
    case stTL:
        {
            STLSession  *pTLSession = (STLSession*)pGMSession;
            pTLSession->pSocketSession = NULL;
            pTLSession->qwTimer = 0xFFFFFFFF;

            if (pTLSession->dwTLUserID != 0)
            {
                INF("Disconnect UserID:[%u]", pTLSession->dwTLUserID);

                nRetCode = CUserManager::GetInstance().LogoutUser(pTLSession->dwTLUserID);
                LOG_CHECK_ERROR(nRetCode);
            }

            CRI("TL disconnected");
        }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return;
Exit0:
    return;
}

CMessageHandler::CMessageHandler()
{
    m_pNet = NULL;
    m_pPacketAllocator = NULL;

    m_poTLAcceptor = NULL;
    m_poMSAcceptor = NULL;

    memset(m_InternalMessageFuncList, 0, sizeof(m_InternalMessageFuncList));
}

CMessageHandler::~CMessageHandler()
{
    m_pNet = NULL;
    m_pPacketAllocator = NULL;

    m_poTLAcceptor = NULL;
    m_poMSAcceptor = NULL;

    memset(m_InternalMessageFuncList, 0, sizeof(m_InternalMessageFuncList));
}

BOOL CMessageHandler::Init(INet *pNet, IPacketAllocator *pPacketAllocator, ISocketAcceptor *pMSAcceptor, ISocketAcceptor *pTLAcceptor)
{
    LOG_PROCESS_ERROR(pNet);
    LOG_PROCESS_ERROR(pPacketAllocator);
    LOG_PROCESS_ERROR(pMSAcceptor);
    LOG_PROCESS_ERROR(pTLAcceptor);

    m_pNet = pNet;
    m_pPacketAllocator = pPacketAllocator;
    m_poTLAcceptor = pTLAcceptor;
    m_poMSAcceptor = pMSAcceptor;

    //all
    m_InternalMessageFuncList[a2a_ping] = &CMessageHandler::OnAllPing;

    // ms
    m_InternalMessageFuncList[m2gm_register] = &CMessageHandler::OnMSRegister;
    m_InternalMessageFuncList[m2gm_clear_role_cache_ack] = &CMessageHandler::OnMSClearRoleCacheAck;
    m_InternalMessageFuncList[m2gm_unlock_role_ack] = &CMessageHandler::OnMSUnlockRoleAck;
    m_InternalMessageFuncList[m2gm_query_role_data_ack] = &CMessageHandler::OnMSQueryRoleDataAck;
    m_InternalMessageFuncList[m2gm_forbid_chat_ack] = &CMessageHandler::OnMSForbidChatAck;
    m_InternalMessageFuncList[m2gm_freeze_role_ack] = &CMessageHandler::OnMSFreezeRoleAck;
    m_InternalMessageFuncList[m2gm_kick_role_ack] = &CMessageHandler::OnMSKickRoleAck;
    m_InternalMessageFuncList[m2gm_send_mail_ack] = &CMessageHandler::OnMSSendMailAck;
    m_InternalMessageFuncList[m2gm_announce_ack] = &CMessageHandler::OnMSAnnounceAck;
    m_InternalMessageFuncList[m2gm_get_announce_list_ack] = &CMessageHandler::OnMSGetAnnounceAck;
    m_InternalMessageFuncList[m2gm_del_announce_ack] = &CMessageHandler::OnMSDelAnnounceAck;
    m_InternalMessageFuncList[m2gm_exe_gm_str_ack] = &CMessageHandler::OnMSExeGMStrAck;
    m_InternalMessageFuncList[m2gm_gm_str_cmd_log] = &CMessageHandler::OnMSGMStrCmdLog;
    m_InternalMessageFuncList[m2gm_del_mail_ack] = &CMessageHandler::OnMSDelMail;
    m_InternalMessageFuncList[m2gm_check_role_freeze_ack] = &CMessageHandler::OnMSCheckRoleFreezeAck;

    m_InternalMessageFuncList[m2gm_get_exchange_price_ack] = &CMessageHandler::OnMSGetExchangePriceAck;
    m_InternalMessageFuncList[m2gm_get_exchange_price_detail_ack] = &CMessageHandler::OnMSQueryOrderDetailAck;
    m_InternalMessageFuncList[m2gm_add_system_order_ack] = &CMessageHandler::OnMSAddSystemOrderAck;
    m_InternalMessageFuncList[m2gm_cancel_system_order_ack] = &CMessageHandler::OnMSDelSystemOrderAck;
    m_InternalMessageFuncList[m2gm_get_exchange_limit_ack] = &CMessageHandler::OnMSGetExchangeLimitAck;
    m_InternalMessageFuncList[m2gm_set_exchange_limit_ack] = &CMessageHandler::OnMSSetExchangeLimitAck;

    // tl
    m_InternalMessageFuncList[tl2gm_register] = &CMessageHandler::OnTLRegister;
    m_InternalMessageFuncList[tl2gm_verify] = &CMessageHandler::OnTLVerify;
    m_InternalMessageFuncList[tl2gm_login] = &CMessageHandler::OnTLLogin;
    m_InternalMessageFuncList[tl2gm_logout] = &CMessageHandler::OnTLLogout;

    m_GmtlMessageFuncList[tl2gm_pre_query] = &CMessageHandler::OnTLPreQuery;
    m_GmtlMessageFuncList[tl2gm_query_role_id] = &CMessageHandler::OnTLQueryRoleID;
    m_GmtlMessageFuncList[tl2gm_query_role_data] = &CMessageHandler::OnTLQueryRoleData;


    m_GmtlMessageFuncList[tl2gm_forbid_chat] = &CMessageHandler::OnTLForbidChat;
    m_GmtlMessageFuncList[tl2gm_freeze_role] = &CMessageHandler::OnTLFreezeRole;
    m_GmtlMessageFuncList[tl2gm_kick_role] = &CMessageHandler::OnTLKickRole;
    m_GmtlMessageFuncList[tl2gm_send_mail] = &CMessageHandler::OnTLSendMail;
    m_GmtlMessageFuncList[tl2gm_announce] = &CMessageHandler::OnTLAnnounce;
    m_GmtlMessageFuncList[tl2gm_get_announce_list] = &CMessageHandler::OnTLGetAnnounceList;
    m_GmtlMessageFuncList[tl2gm_del_announce] = &CMessageHandler::OnTLDelAnnouce;

    m_GmtlMessageFuncList[tl2gm_log] = &CMessageHandler::OnTLLog;
    m_GmtlMessageFuncList[tl2gm_exe_gm_str] = &CMessageHandler::OnTLExeGMStr;
    m_GmtlMessageFuncList[tl2gm_load_role_data] = &CMessageHandler::OnTLLoadRole;
    m_GmtlMessageFuncList[tl2gm_modify_role_data] = &CMessageHandler::OnTLModifyRole;
    m_GmtlMessageFuncList[tl2gm_release_role] = &CMessageHandler::OnTLReleaseRole;
    m_GmtlMessageFuncList[tl2gm_query_name_to_id] = &CMessageHandler::OnTLQueryNameToID;
    m_GmtlMessageFuncList[tl2gm_role_exe_cmd] = &CMessageHandler::OnTLRoleExeCmd;

    m_GmtlMessageFuncList[tl2gm_select_group] = &CMessageHandler::OnTLSelectGroup;
    m_GmtlMessageFuncList[tl2gm_exchange_price] = &CMessageHandler::OnTLQueryExchangePriceLog;
    m_GmtlMessageFuncList[tl2gm_exchange_history] = &CMessageHandler::OnTLQueryExchangeHistory;
    m_GmtlMessageFuncList[tl2gm_order_detail] = &CMessageHandler::OnTLQueryOrderDetail;
    m_GmtlMessageFuncList[tl2gm_add_system_order] = &CMessageHandler::OnTLAddSystemOrder;
    m_GmtlMessageFuncList[tl2gm_del_system_order] = &CMessageHandler::OnTLDelSystemOrder;
    m_GmtlMessageFuncList[tl2gm_sys_exchange_info] = &CMessageHandler::OnTLSystemExchangeInfo;
    m_GmtlMessageFuncList[tl2gm_get_exchange_limit] = &CMessageHandler::OnTLQueryExchangeLimit;
    m_GmtlMessageFuncList[tl2gm_set_exchange_limit] = &CMessageHandler::OnTLSetExchangeLimit;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::UnInit()
{
    return TRUE;
}

CMessageHandler &CMessageHandler::GetInstance()
{
    return ms_Instance;
}

BOOL CMessageHandler::SendMS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32       nRetCode = 0;

    SMSSession  *pMSSession  = CGMSessionMgr::GetInstance().GetMSSession();
    LOG_PROCESS_ERROR(pMSSession);
    LOG_PROCESS_ERROR(pMSSession->pSocketSession);
    LOG_PROCESS_ERROR(pMSSession->eState == egmssNormal);

    nRetCode = pMSSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::SendTL(UINT32 dwTLSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32       nRetCode = 0;
    STLSession  *pTLSession = NULL;

    pTLSession = CGMSessionMgr::GetInstance().GetTLSession(dwTLSessionID);
    LOG_PROCESS_ERROR(pTLSession);
    LOG_PROCESS_ERROR(pTLSession->pSocketSession);
    LOG_PROCESS_ERROR(pTLSession->eState == egmssNormal);

    nRetCode = pTLSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::SendGD(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32       nRetCode = 0;
    SGDSession  *pGDSession = NULL;

    pGDSession = CGMSessionMgr::GetInstance().GetGDSession();
    LOG_PROCESS_ERROR(pGDSession);
    LOG_PROCESS_ERROR(pGDSession->pSocketSession);
    LOG_PROCESS_ERROR(pGDSession->bConnected == true);

    nRetCode = pGDSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::BroadcastLD(INT32 nGroupID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32       nRetCode = 0;
    STLSession  *pTLSession = NULL;
    PACKET_BUFFER* pMsg = NULL;

    for (INT32 i = 0; i < CLDListener::GetInstance().GetLDRegNum(); ++i)
    {
        LD_REG_DATA *pLDData = CLDListener::GetInstance().GetLDRegData(i);
        LOG_CHECK_ERROR(pLDData);
        if (pLDData == NULL || pLDData->nGroupID != nGroupID)
            continue;
        CTLUser *pTLUser = CUserManager::GetInstance().FindUserByID(pLDData->dwUserID);
        LOG_CHECK_ERROR(pTLUser);
        if (pTLUser)
        {
            pTLSession = CGMSessionMgr::GetInstance().GetTLSession(pTLUser->GetSessionID());
            LOG_CHECK_ERROR(pTLSession);
            if (pTLSession == NULL)
                continue;
            LOG_CHECK_ERROR(pTLSession->pSocketSession);
            if (pTLSession->pSocketSession == NULL)
                continue;
            LOG_CHECK_ERROR(pTLSession->eState == egmssNormal);
            if (pTLSession->eState != egmssNormal)
                continue;

            pMsg = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pMsg);

            memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = pTLSession->pSocketSession->Send(pMsg, dwSize, nOffset);
            if (nRetCode != dwSize)
            {
                LOG_CHECK_ERROR(nRetCode == dwSize);
                m_pPacketAllocator->Free(pMsg);
            }

            pMsg = NULL;
        }
    }

    if (pPacketBuffer)
        m_pPacketAllocator->Free(pPacketBuffer);
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::BroadcastLD(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32       nRetCode = 0;
    STLSession  *pTLSession = NULL;
    PACKET_BUFFER* pMsg = NULL;

    for (INT32 i = 0; i < CLDListener::GetInstance().GetLDRegNum(); ++i)
    {
        LD_REG_DATA *pLDData = CLDListener::GetInstance().GetLDRegData(i);
        LOG_CHECK_ERROR(pLDData);
        if (pLDData == NULL)
            continue;
        CTLUser *pTLUser = CUserManager::GetInstance().FindUserByID(pLDData->dwUserID);
        LOG_CHECK_ERROR(pTLUser);
        if (pTLUser)
        {
            pTLSession = CGMSessionMgr::GetInstance().GetTLSession(pTLUser->GetSessionID());
            LOG_CHECK_ERROR(pTLSession);
            if (pTLSession == NULL)
                continue;
            LOG_CHECK_ERROR(pTLSession->pSocketSession);
            if (pTLSession->pSocketSession == NULL)
                continue;
            LOG_CHECK_ERROR(pTLSession->eState == egmssNormal);
            if (pTLSession->eState != egmssNormal)
                continue;

            pMsg = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pMsg);

            memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = pTLSession->pSocketSession->Send(pMsg, dwSize, nOffset);
            if (nRetCode != dwSize)
            {
                LOG_CHECK_ERROR(nRetCode == dwSize);
                m_pPacketAllocator->Free(pMsg);
            }

            pMsg = NULL;
        }
    }

    if (pPacketBuffer)
        m_pPacketAllocator->Free(pPacketBuffer);
    return TRUE;
Exit0:
    return FALSE;
}

// To GD

BOOL CMessageHandler::DoGDRegister()
{
    INT32 nRetCode = 0;

    A2U_REGRISTER* pMsg = (A2U_REGRISTER*)m_pPacketAllocator->Alloc(sizeof(A2U_REGRISTER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = a2u_regrister;
    pMsg->byType = stGM;
    pMsg->bySessionID = 1;
    pMsg->dwProcessID = GetCurrentProcessId();

    nRetCode = SendGD((PACKET_BUFFER*)pMsg, sizeof(A2U_REGRISTER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoAllPing()
{
    INT32   nRetCode = 0;
    A2A_PING    *pMsg = NULL;

    LOG_PROCESS_ERROR(CGMSessionMgr::GetInstance().GetGDSession());
    if (CGMSessionMgr::GetInstance().GetGDSession()->bConnected)
    {
        pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
        LOG_PROCESS_ERROR(pMsg);

        pMsg->wMsg = a2a_ping;

        nRetCode = SendGD((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
    }

    //for (INT32 i = 1; i < MAX_MS_SESSION_COUNT; ++i)
    {
        SMSSession  *pMSSession  = CGMSessionMgr::GetInstance().GetMSSession();
        LOG_PROCESS_ERROR(pMSSession);
        if (pMSSession->pSocketSession)
        {
            pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
            LOG_PROCESS_ERROR(pMsg);

            pMsg->wMsg = a2a_ping;

            nRetCode = pMSSession->pSocketSession->Send((PACKET_BUFFER*)pMsg, sizeof(A2A_PING), 0);
            LOG_PROCESS_ERROR(nRetCode == sizeof(A2A_PING));

            pMsg = NULL;
        }
    }

    for (INT32 i = 1; i < MAX_GM_TL_SESSION_COUNT; ++i)
    {
        STLSession *pTLSession = CGMSessionMgr::GetInstance().GetTLSession(i);
        LOG_PROCESS_ERROR(pTLSession);

        if (pTLSession->pSocketSession == NULL)
            continue;

        pMsg = (A2A_PING *)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
        LOG_PROCESS_ERROR(pMsg);
        pMsg->wMsg = a2a_ping;

        nRetCode = pTLSession->pSocketSession->Send((PACKET_BUFFER*)pMsg, sizeof(A2A_PING), 0);
        LOG_PROCESS_ERROR(nRetCode == sizeof(A2A_PING));

        pMsg = NULL;
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

void CMessageHandler::OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SGMSession  *pGMSession = (SGMSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGMSession);

    pGMSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

    return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();
    return ;
}

// From MS
void CMessageHandler::OnMSRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode  = 0;
    M2GM_REGISTER   *pMsg = (M2GM_REGISTER*)pPacketBuffer;
    
    SMSSession  *pMSSession = (SMSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pMSSession);

    pMSSession->eState = egmssNormal;


    // MS should provide its database information when coming online so gmserver can find the database after a temporary MS goes offline. Allan Tao [20.12.2013]

    //nRetCode = CMSMgr::GetInstance().RegisterMS(pMSSession->dwSessionID, pMsg->szName);
    //LOG_PROCESS_ERROR(nRetCode);

    // todo_tq 12 broadcast to all tl Allan Tao[3.12.2013] 

    nRetCode = DoMSRegisterAck();
    LOG_PROCESS_ERROR(nRetCode);

    INF("MS registered");

Exit0:
    return;
}

void CMessageHandler::OnMSClearRoleCacheAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_DATA_BASE  *pCmd = NULL;

    M2GM_CLEAR_ROLE_CACHE_ACK  *pMsg = (M2GM_CLEAR_ROLE_CACHE_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_MODIFY_ROLE*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_MS_DEL_ROLE_CACHE_ACK;

    INF("[%u] clear role cache return result[%d]", pTLUser->GetID(), pMsg->nResult);
    
    return ;
Exit0:
    return;    
}

void CMessageHandler::OnMSUnlockRoleAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    //MS_DATA     *pMSData = NULL;
    SMSSession  *pMSSession = NULL;
    M2GM_UNLOCK_ROLE_ACK    *pMsg = (M2GM_UNLOCK_ROLE_ACK*)pPacketBuffer;

    pMSSession = (SMSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pMSSession);

    //pMSData = CMSMgr::GetInstance().FindMS(pMSSession->dwSessionID);
    //LOG_PROCESS_ERROR(pMSData);

    if (errSuccess == pMsg->nResult)
    {
        nRetCode = CGMRoleMgr::GetInstance().DelRole(g_ServerConfig.Common.nAreaID, pMsg->dwRoleID);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        // todo_tq log Allan Tao[24.12.2013] 
    }

Exit0:
    return; 
}

void CMessageHandler::OnMSQueryRoleDataAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_QUERY_ROLE_DATA  *pCmd = NULL;

    M2GM_QUERY_ROLE_DATA_ACK  *pMsg = (M2GM_QUERY_ROLE_DATA_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_QUERY_ROLE_DATA*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_ECHO_ERROR(pCmd, pTLUser->GetSessionID(), "从MS返回未找到CMD");

    pCmd->nResult = pMsg->nResult;
    pCmd->dwLogoutTime = pMsg->dwLogoutTime;
    if (pMsg->nResult == errSuccess && pMsg->dwLogoutTime == 0)
        pCmd->bIsOnline = TRUE;
    pCmd->eCmdState = GM_LS_MS_QUERY_ROLE_INFO_ACK;

    DBG("TLUserID[%u] CmdID[%d] query role data ms return result[%d]", pTLUser->GetID(), pCmd->dwSerialID, pMsg->nResult);
    return;
Exit0:
    return;
}

void CMessageHandler::OnMSForbidChatAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_FORBIDDEN_CHAT  *pCmd = NULL;

    M2GM_FORBID_CHAT_ACK  *pMsg = (M2GM_FORBID_CHAT_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_FORBIDDEN_CHAT*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_FORBID_CHAT_ACK;

    DBG("TLUserID[%u] CmdID[%d] forbid chat ms return result[%d]", pTLUser->GetID(), pCmd->dwSerialID, pMsg->nResult);
    return;
Exit0:
    return;   
}

void CMessageHandler::OnMSFreezeRoleAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_FREEZE_ROLE  *pCmd = NULL;

    M2GM_FREEZE_ROLE_ACK  *pMsg = (M2GM_FREEZE_ROLE_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_FREEZE_ROLE*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_FREEZE_ROLE_ACK;

    DBG("TLUserID[%u] CmdID[%d] freeze role ms return result[%d]", pTLUser->GetID(), pCmd->dwSerialID, pMsg->nResult);
    return;
Exit0:
    return;
}

void CMessageHandler::OnMSKickRoleAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_KICK_ROLE  *pCmd = NULL;

    M2GM_KICK_ROLE_ACK  *pMsg = (M2GM_KICK_ROLE_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_KICK_ROLE*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_KICK_ROLE_ACK;

    DBG("TLUserID[%u] CmdID[%u] kick role ms return result[%d]", pTLUser->GetID(), pCmd->dwSerialID, pMsg->nResult);

    return;
Exit0:
    if (pTLUser)
    {
        //nRetCode = DoTLKickRoleAck(pTLUser->GetSessionID(), nResult, pMsg->dwRoleID);
        //LOG_CHECK_ERROR(nRetCode);
    }
    return;
}

void CMessageHandler::OnMSSendMailAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errGMUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_SEND_MAIL   *pCmd = NULL;

    M2GM_SEND_MAIL_ACK  *pMsg = (M2GM_SEND_MAIL_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_SEND_MAIL*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_SEND_MAIL_ACK;

    DBG("TLUserID[%u] CmdID[%u] send mail ms return result[%d]", pTLUser->GetID(), pCmd->dwSerialID, pMsg->nResult);
    return;
Exit0:
    if (pTLUser)
    {
        //nRetCode = DoTLKickRoleAck(pTLUser->GetSessionID(), nResult, pMsg->dwRoleID);
        //LOG_CHECK_ERROR(nRetCode);
    }
    return;
}

void CMessageHandler::OnMSAnnounceAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_ANNOUNCE  *pCmd = NULL;

    M2GM_ANNOUNCE_ACK  *pMsg = (M2GM_ANNOUNCE_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_ANNOUNCE*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_ANNOUNCE_ACK;

    DBG("TLUserID[%u] CmdID[%u] announce ms return result[%d]", pTLUser->GetID(), pCmd->dwSerialID, pCmd->nResult);
    return;
Exit0:
    if (pTLUser)
    {
        //nRetCode = DoTLKickRoleAck(pTLUser->GetSessionID(), nResult, pMsg->dwRoleID);
        //LOG_CHECK_ERROR(nRetCode);
    }
    return;
}

void CMessageHandler::OnMSGetAnnounceAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_GET_ANNOUNCE_LIST  *pCmd = NULL;

    M2GM_GET_ANNOUNCE_LIST_ACK  *pMsg = (M2GM_GET_ANNOUNCE_LIST_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_GET_ANNOUNCE_LIST*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_MS_QUERY_ANNOUNCE_ACK;

    pCmd->nCount = pMsg->nCount;
    for (INT32 i = 0; i < pMsg->nCount && i < 100; ++i)
    {
        pCmd->data[i].nGroupID = pMsg->data[i].nGroupID;
        pCmd->data[i].dwSerial = pMsg->data[i].dwSerial;
        pCmd->data[i].dwIntervalTime = pMsg->data[i].dwIntervalTime;
        pCmd->data[i].dwRepeatNum = pMsg->data[i].dwRepeatNum;
        strxcpy(pCmd->data[i].szText, pMsg->data[i].szText, sizeof(pCmd->data[i].szText));
    }

    DBG("TLUserID[%u] CmdID[%u] announce ms return result[%d]", pTLUser->GetID(), pCmd->dwSerialID, pMsg->nResult);
    return;
Exit0:
    if (pTLUser)
    {
        //nRetCode = DoTLKickRoleAck(pTLUser->GetSessionID(), nResult, pMsg->dwRoleID);
        //LOG_CHECK_ERROR(nRetCode);
    }
    return;
}

void CMessageHandler::OnMSDelAnnounceAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_DEL_ANNOUNCE  *pCmd = NULL;

    M2GM_DEL_ANNOUNCE_ACK  *pMsg = (M2GM_DEL_ANNOUNCE_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_DEL_ANNOUNCE*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_DEL_ANNOUNCE_ACK;
    pCmd->dwAnnounceSerial = pMsg->dwAnnounceSerial;

    DBG("[%u] announce ms return result[%d]", pTLUser->GetID(), pMsg->nResult);
    return;
Exit0:
    if (pTLUser)
    {
        //nRetCode = DoTLKickRoleAck(pTLUser->GetSessionID(), nResult, pMsg->dwRoleID);
        //LOG_CHECK_ERROR(nRetCode);
    }
    return;
}

void CMessageHandler::OnMSExeGMStrAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_EXE_GM_STR  *pCmd = NULL;

    M2GM_EXE_GM_STR_ACK  *pMsg = (M2GM_EXE_GM_STR_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_EXE_GM_STR*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_EXE_SYS_CMD_ACK;

    DBG("TLUserID[%u] CmdID[%u] exe gm str ms return result[%d]", pTLUser->GetID(), pCmd->dwSerialID, pMsg->nResult);
    return;
Exit0:
    return;
}

void CMessageHandler::OnMSGMStrCmdLog(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;

    INT32   nIndex = 0;

    M2GM_GM_STR_CMD_LOG *pMsg = (M2GM_GM_STR_CMD_LOG*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    //while (pMsg->szGMCommand[nIndex] != ' ' && pMsg->szGMCommand[nIndex] != '\0')
    //    ++nIndex;

    //if (pMsg->szGMCommand[nIndex] == ' ' && nIndex < 512)
    //{
    //    char    szCommand[512] = { 0 };
    //    strxcpy(szCommand, pMsg->szGMCommand, nIndex + 1);

    //    nRetCode = CGMLog::GetInstance().WriteLogWithTime(pMsg->dwTime, pMsg->szRoleName, "game str cmd", "%s", pMsg->szGMCommand);
    //    LOG_PROCESS_ERROR(nRetCode);
    //}
    //else
    {
        nRetCode = CGMLog::GetInstance().WriteLogWithTime(pMsg->dwTime, pMsg->szRoleName, "game str cmd", "%s", pMsg->szGMCommand);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return;
Exit0:
    return;
}

void CMessageHandler::OnMSDelMail(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    MailInfo    *pMailInfo = NULL;
    CMD_MODIFY_ROLE  *pCmd = NULL;

    M2GM_DEL_MAIL_ACK  *pMsg = (M2GM_DEL_MAIL_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_MODIFY_ROLE*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);
    
    --pCmd->nSlotCount;
    --pCmd->nMailCount;
    snprintf(pCmd->szLog, MAX_GM_LOG_LENGTH, "%s Del Mail[%u] ||", pCmd->szLog, pMsg->dwMailID);
    pCmd->eCmdState = GM_LS_MS_DEL_MAIL_ACK;

    LOG_ECHO_ERROR(pCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to delete mail.");

    pMailInfo = (MailInfo*)pMsg->szBuffer;
    if (pMailInfo->ma.byAttachType == ATTACHTYPE_SIMPLE)
    {
        INF("DATALOG:TLUser[%s] delete mail mail id[%u], mail time[%u], SenderID[%u], SenderName[%s], ReceiverID[%u], Subject[%s], Content[%s], islocked[%d], attach item id[%u] count[%d]",
            pTLUser->GetUserName(), pMsg->dwMailID, pMailInfo->dwMailTime, pMailInfo->dwSenderRoleID, pMailInfo->szSenderRoleName,
            pMailInfo->dwReceiverRoleID, pMailInfo->szMailSubject, pMailInfo->szMailContent, (INT32)pMailInfo->ma.byAttachLocked,
            pMailInfo->ma.sa.nItemID, pMailInfo->ma.sa.nItemCount);
    }
    else if (pMailInfo->ma.byAttachType == ATTACHTYPE_BUFFER)
    {
        LOG_PROCESS_ERROR(FALSE);       // TODO: mail no longer uses direct buffers; the auction house functionality is gone
        ItemInfo    itemInfo;
        INT32 nLength = itemInfo.Load(pMailInfo->ma.buffer);
        INT32   nDumpLength = 0;
        char        szBuff[1024] = { 0 };

        for (INT32 i = 0; i < nLength; ++i)
        {
            nDumpLength += snprintf(szBuff + nDumpLength, 1024 - nDumpLength, "%s%x%x", szBuff, pMailInfo->ma.buffer[i] >> 4, pMailInfo->ma.buffer[i]);
        }

        INF("DATALOG:TLUser[%s] delete mail mail id[%u], mail time[%u], SenderID[%u], SenderName[%s], ReceiverID[%u], Subject[%s], Content[%s], islocked[%d], attach item buffer[%s]",
            pTLUser->GetUserName(), pMsg->dwMailID, pMailInfo->dwMailTime, pMailInfo->dwSenderRoleID, pMailInfo->szSenderRoleName,
            pMailInfo->dwReceiverRoleID, pMailInfo->szMailSubject, pMailInfo->szMailContent, (INT32)pMailInfo->ma.byAttachLocked, szBuff);
    }
    else
    {
        INF("DATALOG:TLUser[%s] delete mail mail id[%u], mail time[%u], SenderID[%u], SenderName[%s], ReceiverID[%u], Subject[%s], Content[%s], islocked[%d], no attach",
            pTLUser->GetUserName(), pMsg->dwMailID, pMailInfo->dwMailTime, pMailInfo->dwSenderRoleID, pMailInfo->szSenderRoleName,
            pMailInfo->dwReceiverRoleID, pMailInfo->szMailSubject, pMailInfo->szMailContent, (INT32)pMailInfo->ma.byAttachLocked);
    }

    return;
Exit0:
    return;
}

void CMessageHandler::OnMSCheckRoleFreezeAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    CMD_LOAD_ROLE  *pCmd = NULL;

    M2GM_CHECK_ROLE_FREEZE_ACK  *pMsg = (M2GM_CHECK_ROLE_FREEZE_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(pTLUser, errGMUserNotFind);

    pCmd = (CMD_LOAD_ROLE*)CKernel::GetInstance().FindCmd(pMsg->dwCmdSerialID);
    LOG_PROCESS_ERROR_RET_CODE(pCmd, errGMLostCmd);

    pCmd->nResult = pMsg->nResult;
    pCmd->eCmdState = GM_LS_MS_CHECK_ROLE_FREEZE_ACK;

    DBG("[%u] load role[%u] return result[%d]", pTLUser->GetID(), pCmd->dwRoleID, pMsg->nResult);

    return;
Exit0:
    return;
}

void CMessageHandler::OnMSGetExchangePriceAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    M2GM_GET_EXCHANGE_PRICE_ACK *pMsg = (M2GM_GET_EXCHANGE_PRICE_ACK*)pPacketBuffer;

    nRetCode = DoTLExchangePrice(pMsg->byGroupID, pMsg->bySellPriceCount, pMsg->byBuyPriceCount, (const char*)(pMsg->stPrices));
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnMSQueryOrderDetailAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;
    M2GM_GET_EXCHANGE_PRICE_DETAIL_ACK *pMsg = (M2GM_GET_EXCHANGE_PRICE_DETAIL_ACK*)pPacketBuffer;

    pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_PROCESS_ERROR(pTLUser);

    nRetCode = DoTLQueryOrderDetailAck(pTLUser->GetSessionID(), pMsg->byGroupID, pMsg->byOrderType, pMsg->nPrice, pMsg->byCountCount, (const char *)pMsg->stCounts);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnMSAddSystemOrderAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    M2GM_ADD_SYSTEM_ORDER_ACK   *pMsg = (M2GM_ADD_SYSTEM_ORDER_ACK*)pPacketBuffer;

    CTLUser *pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_CHECK_ERROR(pTLUser);

    if (pMsg->nResult == errSuccess)
    {
        if (pTLUser)
        {
            nRetCode = CGMLog::GetInstance().WriteLog(pTLUser->GetUserName(), "add system order", "OrderID[%llu] Price[%d] Count[%d]",
                pMsg->qwOrderID, pMsg->nPrice, pMsg->nCount);
            LOG_CHECK_ERROR(nRetCode);
        }
        else
        {
            nRetCode = CGMLog::GetInstance().WriteLog("User Not Find!", "add system order", "UserID[%u], OrderID[%llu] Price[%d] Count[%d]",
                pMsg->dwTLUserID, pMsg->qwOrderID, pMsg->nPrice, pMsg->nCount);
            LOG_CHECK_ERROR(nRetCode);
        }
        INF("TLUser[%u] group[%d] add system order from ms success! orderID[%llu]", pMsg->dwTLUserID, (INT32)pMsg->byGroupID, pMsg->qwOrderID);
    }
    else
        INF("TLUser[%u] group[%d] add system order from ms failed, result[%d]", pMsg->dwTLUserID, (INT32)pMsg->byGroupID, pMsg->nResult);

    nRetCode = DoTLAddSystemOrderAck(pMsg->byGroupID, pMsg->qwOrderID, pMsg->nResult, pMsg->nPrice, pMsg->nCount);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnMSDelSystemOrderAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    M2GM_CANCEL_SYSTEM_ORDER_ACK    *pMsg = (M2GM_CANCEL_SYSTEM_ORDER_ACK*)pPacketBuffer;

    CTLUser *pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_CHECK_ERROR(pTLUser);

    if (pMsg->nResult == errSuccess)
    {
        if (pTLUser)
        {
            nRetCode = CGMLog::GetInstance().WriteLog(pTLUser->GetUserName(), "del system order", "OrderID[%llu] Price[%d] Count[%d]",
                pMsg->qwOrderID, pMsg->nPrice, pMsg->nCount);
            LOG_CHECK_ERROR(nRetCode);
        }
        else
        {
            nRetCode = CGMLog::GetInstance().WriteLog("User Not Find!", "del system order", "UserID[%u], OrderID[%llu] Price[%d] Count[%d]",
                pMsg->dwTLUserID, pMsg->qwOrderID, pMsg->nPrice, pMsg->nCount);
            LOG_CHECK_ERROR(nRetCode);
        }
        INF("TLUser[%u] group[%d] del system order from ms success! orderID[%llu]", pMsg->dwTLUserID, (INT32)pMsg->byGroupID, pMsg->qwOrderID);
    }
    else
        INF("TLUser[%u] group[%d] del system order from ms failed, result[%d]", pMsg->dwTLUserID, (INT32)pMsg->byGroupID, pMsg->nResult);

    nRetCode = DoTLDelSystemOrderAck(pMsg->byGroupID, pMsg->qwOrderID, pMsg->nResult, pMsg->nPrice, pMsg->nCount);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnMSGetExchangeLimitAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;

    M2GM_GET_EXCHANGE_LIMIT_ACK *pMsg = (M2GM_GET_EXCHANGE_LIMIT_ACK*)pPacketBuffer;

    CTLUser *pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_CHECK_ERROR(pTLUser);

    nRetCode = DoTLGetExchangeLimitAck(pTLUser->GetSessionID(), pMsg->byGroupID, pMsg->nPriceHigh, pMsg->nPriceLow, pMsg->nSingleOrderLimit, pMsg->nDailyOrderLimit);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnMSSetExchangeLimitAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;

    M2GM_SET_EXCHANGE_LIMIT_ACK *pMsg = (M2GM_SET_EXCHANGE_LIMIT_ACK*)pPacketBuffer;

    CTLUser *pTLUser = CUserManager::GetInstance().FindUserByID(pMsg->dwTLUserID);
    LOG_CHECK_ERROR(pTLUser);

    nRetCode = DoTLSetExchangeLimitAck(pTLUser->GetSessionID(), pMsg->byGroupID, pMsg->nPriceHigh, pMsg->nPriceLow, pMsg->nSingleOrderLimit, pMsg->nDailyOrderLimit);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnTLRegister(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    UINT32      dwIP = 0;
    UINT16      wPort = 0;
    char        *pIP = NULL;
    in_addr    addr;
    TL2GM_REGISTER *pMsg = (TL2GM_REGISTER*)pPacketBuffer;

    STLSession  *pTLSession = (STLSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pTLSession);
    LOG_PROCESS_ERROR(pTLSession->pSocketSession);

    pTLSession->eState = egmssNormal;

    nRetCode = pTLSession->pSocketSession->GetRemoteAddress(dwIP, wPort);
    LOG_PROCESS_ERROR(nRetCode);

    addr.s_addr = dwIP;
    pIP = inet_ntoa(addr);
    LOG_PROCESS_ERROR(pIP);

    nRetCode = CPermissionMgr::GetInstance().CheckIP(dwIP);
    LOG_PROCESS_ERROR(nRetCode);

    INF("TL Register Success! IP:[%s][%d]", pIP, dwIP);

    nRetCode = DoTLRegisterAck(pTLSession->dwSessionID, errSuccess);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (pIP)
        INF("TL Register failed! IP:[%s][%d]", pIP, dwIP);

    nRetCode = DoTLRegisterAck(pTLSession->dwSessionID, errUnknownError);
    LOG_CHECK_ERROR(nRetCode);

    if (pTLSession)
        pTLSession->pSocketSession->Close();
    return;
}

void CMessageHandler::OnTLVerify(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32           nResult = errUnknownError;
    INT32           nRetCode = 0;
    STLSession      *pTLSession = NULL;
    CTLUser         *pUserData = NULL;
    UINT32          dwTLUserID = 0;
    char            *pIP = NULL;
    in_addr         addr;
    UINT32          dwIP = 0;
    UINT16          wPort = 0;
    BOOL            bLD = FALSE;

    TL2GM_VERIFY     *pMsg = (TL2GM_VERIFY*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLSession = (STLSession *)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pTLSession);
    LOG_PROCESS_ERROR(pTLSession->pSocketSession);
    LOG_PROCESS_ERROR(pTLSession->eState == egmssNormal);

    nRetCode = pTLSession->pSocketSession->GetRemoteAddress(dwIP, wPort);
    LOG_PROCESS_ERROR(nRetCode);

    addr.s_addr = dwIP;
    pIP = inet_ntoa(addr);
    LOG_PROCESS_ERROR(pIP);

    nRetCode = CPermissionMgr::GetInstance().CheckIP(dwIP);
    LOG_ECHO_ERROR(nRetCode, pTLSession->dwSessionID, "IP restriction.");

    nRetCode = CPermissionMgr::GetInstance().GetUserID(pMsg->szName, pMsg->szPwd);
    LOG_ECHO_ERROR(nRetCode != 0, pTLSession->dwSessionID, "Incorrect username or password.");

    bLD = CPermissionMgr::GetInstance().IsLD(pMsg->szName, pMsg->szPwd);
    
    //pUserData = CUserManager::GetInstance().FindUserByID(nRetCode);
    //LOG_ECHO_ERROR(pUserData == NULL, pTLSession->dwSessionID, "重复登陆");

    INF("GMTOOL Verify password Success IP:[%s][%d]\tAccount:[%s]\t", pIP, dwIP, pMsg->szName);

    // ack 
    nRetCode = DoTLVerifyAck(pTLSession->dwSessionID, errSuccess, bLD);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (pIP)
        INF("GMTOOL Loaded failed! IP IP:[%s][%d]\tAccount:[%s]\t", pIP, dwIP, pMsg->szName);
    if (pTLSession)
    {
        nRetCode = DoTLVerifyAck(pTLSession->dwSessionID, nResult, FALSE);
        LOG_PROCESS_ERROR(nRetCode);
    }
    if (errGMRelogin == nResult)
    {
        pTLSession->pSocketSession->Close();

        if (pUserData)
        {
            pTLSession = CGMSessionMgr::GetInstance().GetTLSession(pUserData->GetSessionID());
            if (pTLSession)
                pTLSession->pSocketSession->Close();
        }
    }
    return;
}

void CMessageHandler::OnTLLogin(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32           nResult = errUnknownError;
    INT32           nRetCode = 0;
    STLSession      *pTLSession = NULL;
    CTLUser         *pUserData = NULL;
    UINT32          dwTLUserID = 0;
    char            *pIP = NULL;
    in_addr         addr;
    UINT32          dwIP = 0;
    UINT16          wPort = 0;
    
    TL2GM_LOGIN     *pMsg = (TL2GM_LOGIN*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLSession = (STLSession *)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pTLSession);
    LOG_PROCESS_ERROR(pTLSession->pSocketSession);
    LOG_PROCESS_ERROR(pTLSession->eState == egmssNormal);

    LOG_ECHO_ERROR(pMsg->nVersion == GM_MAJOR_VERSION, pTLSession->dwSessionID, "Incorrect tool version");
    LOG_ECHO_ERROR(pMsg->nDependVersion == GM_DEPEND_VERSION, pTLSession->dwSessionID, "Incorrect tool database version");

    nRetCode = pTLSession->pSocketSession->GetRemoteAddress(dwIP, wPort);
    LOG_PROCESS_ERROR(nRetCode);

    addr.s_addr = dwIP;
    pIP = inet_ntoa(addr);
    LOG_PROCESS_ERROR(pIP);

    nRetCode = CPermissionMgr::GetInstance().CheckIP(dwIP);
    LOG_ECHO_ERROR(nRetCode, pTLSession->dwSessionID, "IP not in whitelist");

    dwTLUserID = CPermissionMgr::GetInstance().GetUserID(pMsg->szName, pMsg->szPwd);
    LOG_ECHO_ERROR(dwTLUserID, pTLSession->dwSessionID, "Incorrect username or password");

    pUserData = CUserManager::GetInstance().FindUserByID(dwTLUserID);
    LOG_ECHO_ERROR(pUserData == NULL, pTLSession->dwSessionID, "Account is logged in from another location");

    nRetCode = CUserManager::GetInstance().LoginUser(pTLSession->dwSessionID, dwIP, pMsg->szName, pMsg->szPwd);
    LOG_ECHO_ERROR(nRetCode, pTLSession->dwSessionID, "Login failed");

    pTLSession->dwTLUserID = dwTLUserID;

    // ack
    nRetCode = DoTLLoginAck(pTLSession->dwSessionID, errSuccess);
    LOG_PROCESS_ERROR(nRetCode);

    INF("TL Login success.SessionID:[%u]UserName:[%s]UserID[%u],IP:[%s][%u]", pTLSession->dwSessionID, pMsg->szName, dwTLUserID, pIP, dwIP);

    /// send item list data
    //nRetCode = DoTLItemListNtf(pTLSession->dwSessionID);
    //LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (pMsg && pIP)
        INF("TL Login failed.Result:[%d],UserName:[%s],IP:[%s][%u]", nResult, pMsg->szName, pIP, dwIP);

    if (pTLSession)
    {
        nRetCode = DoTLLoginAck(pTLSession->dwSessionID, nResult);
        LOG_CHECK_ERROR(nRetCode);
    }
    if (errGMRelogin == nResult )
    {
        pTLSession->pSocketSession->Close();
        if (pUserData)
        {
            pTLSession = CGMSessionMgr::GetInstance().GetTLSession(pUserData->GetSessionID());
            if (pTLSession)
                pTLSession->pSocketSession->Close();
        }
    }
    return;
}

void CMessageHandler::OnTLLogout(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    STLSession      *pTLSession = NULL;
    CTLUser         *pUserData = NULL;
    UINT32          dwTLUserID = 0;
    INT32           nPermission = 0;
    TL2GM_LOGIN     *pMsg = (TL2GM_LOGIN*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pTLSession = (STLSession *)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pTLSession);
    LOG_PROCESS_ERROR(pTLSession->pSocketSession);
    LOG_PROCESS_ERROR(pTLSession->eState == egmssNormal);

    nRetCode = CUserManager::GetInstance().LogoutUser(pTLSession->dwTLUserID);
    LOG_PROCESS_ERROR(nRetCode);

    // ack
    nRetCode = DoTLLogoutAck(pTLSession->dwSessionID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLPreQuery(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_PRE_QUERY   *pCmd = NULL;

    TL2GM_PRE_QUERY   *pMsg = (TL2GM_PRE_QUERY*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_PRE_QUERY*)CCmdMgr::GetInstance().NewCmd(GM_CMD_PRE_QUERY);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_PRE_QUERY;
    pCmd->eCmdState = GM_LS_PRE_QUERY_ROLE_ID;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->pfunc = CB_PreQueryRoleID;
    pCmd->dwRoleID = 0;
    strxcpy(pCmd->szRoleName, pMsg->szRoleName, sizeof(pCmd->szRoleName));

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLQueryRoleID(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_QUERY_ROLE_ID   *pCmd = NULL;

    TL2GM_QUERY_ROLE_ID   *pMsg = (TL2GM_QUERY_ROLE_ID*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_QUERY_ROLE_ID*)CCmdMgr::GetInstance().NewCmd(GM_CMD_QUERY_ROLE_ID);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_QUERY_ROLE_ID;
    pCmd->eCmdState = GM_LS_QUERY_ROLE_NAME;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->nQueryType = pMsg->nQueryType;
    pCmd->nCount = 0;
    pCmd->pfunc = CB_QueryRoleID;
    strxcpy(pCmd->szName, pMsg->szName, sizeof(pCmd->szName));
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);
    memset(pCmd->RoleKey, 0, sizeof(pCmd->RoleKey));
    
    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);
    //pTLUser->SetCmd(pCmd);

    DBG("TLUserID[%u] Query Role ID[%s] Begin, CmdID[%d]", pTLUser->GetID(), pMsg->szName, pCmd->dwSerialID);
    return;
Exit0: 
    return;
}

void CMessageHandler::OnTLQueryRoleData(CTLUser *pTLUser, const char *pcData)
{
    INT32           nRetCode = 0;
    INT32           nResult = errGMUnknownError;
    CMD_QUERY_ROLE_DATA     *pCmd = NULL;
    CRole           *poRole = NULL;

    TL2GM_QUERY_ROLE_DATA   *pMsg = (TL2GM_QUERY_ROLE_DATA*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    poRole = CGameWorld::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_ECHO_ERROR(poRole == NULL, pTLUser->GetSessionID(), "Role is currently in use, please try again later");

    pCmd = (CMD_QUERY_ROLE_DATA*)CCmdMgr::GetInstance().NewCmd(GM_CMD_QUERY_ROLE_DATA);
    LOG_ECHO_ERROR(pCmd, pTLUser->GetSessionID(), "Memory exhausted");

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_QUERY_ROLE_DATA;
    pCmd->eCmdState = GM_LS_MS_QUERY_ROLE_INFO;
    pCmd->nTimeout = time(NULL) + GM_SVR_LONG_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->dwLogoutTime = 0;
    pCmd->bIsOnline = FALSE;
    pCmd->dwRoleID = pMsg->dwRoleID;
    strxcpy(pCmd->szRoleName, pMsg->szRoleName, ROLE_NAME_LEN);
    pCmd->pfunc = CB_QueryRoleData;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);
    memset(&pCmd->roleData, 0, sizeof(pCmd->roleData));
    
    pCmd->poRole = CGameWorld::GetInstance().NewRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pCmd->poRole);

    pCmd->nLength = 0;
    memset(pCmd->szData, 0, sizeof(pCmd->szData));

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] query role[%u] data begin, cmdID[%d]", pTLUser->GetID(), pMsg->dwRoleID, pCmd->dwSerialID);
    return;
Exit0:
    if (pCmd && pCmd->poRole)
    {
        CGameWorld::GetInstance().DelRole(pCmd->poRole);
        pCmd->poRole = NULL;
    }
        
    if (pCmd)
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return;
}

void CMessageHandler::OnTLForbidChat(CTLUser *pTLUser, const char *pcData)
{
    INT32           nResult = errGMUnknownError;
    INT32           nRetCode = 0;
    CMD_FORBIDDEN_CHAT      *pCmd = NULL;
    TL2GM_FORBID_CHAT       *pMsg = (TL2GM_FORBID_CHAT*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_FORBIDDEN_CHAT*)CCmdMgr::GetInstance().NewCmd(GM_CMD_FORBID_CHAT);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_FORBID_CHAT;
    pCmd->eCmdState = GM_LS_FORBID_CHAT;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->dwRoleID = pMsg->dwRoleID;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);
    if (pMsg->dwTime != 0 && pMsg->dwTime != FOREVER_TIME)
        pCmd->dwTime = pMsg->dwTime + time(NULL);
    else
        pCmd->dwTime = pMsg->dwTime;

    pCmd->pfunc = CB_ForbiddenChat;

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);
    //pTLUser->SetCmd(pCmd);

    DBG("TLUserID[%u] CmdID[%d] Forbid Role[%u] Chat Time[%u] Begin", pTLUser->GetID(), pCmd->dwSerialID, pCmd->dwRoleID, pCmd->dwTime);
    return;
Exit0:
    return;
}

void CMessageHandler::OnTLFreezeRole(CTLUser *pTLUser, const char *pcData)
{
    INT32           nResult = errGMUnknownError;
    INT32           nRetCode = 0;
    CMD_FREEZE_ROLE      *pCmd = NULL;
    TL2GM_FREEZE_ROLE       *pMsg = (TL2GM_FREEZE_ROLE*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_FREEZE_ROLE*)CCmdMgr::GetInstance().NewCmd(GM_CMD_FREEZE_ROLE);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_FREEZE_ROLE;
    pCmd->eCmdState = GM_LS_FREEZE_ROLE;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->dwRoleID = pMsg->dwRoleID;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);

    if (pMsg->dwTime != 0 && pMsg->dwTime != FOREVER_TIME)
        pCmd->dwTime = pMsg->dwTime + time(NULL);
    else
        pCmd->dwTime = pMsg->dwTime;

    pCmd->pfunc = CB_FreezeRole;

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] CmdID[%d] Freeze Role[%u] Time[%u] Begin", pTLUser->GetID(), pCmd->dwSerialID, pCmd->dwRoleID, pCmd->dwTime);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLKickRole(CTLUser *pTLUser, const char *pcData)
{
    INT32           nResult = errGMUnknownError;
    INT32           nRetCode = 0;
    CMD_KICK_ROLE      *pCmd = NULL;
    TL2GM_KICK_ROLE       *pMsg = (TL2GM_KICK_ROLE*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_KICK_ROLE*)CCmdMgr::GetInstance().NewCmd(GM_CMD_KICK_ROLE);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_KICK_ROLE;
    pCmd->eCmdState = GM_LS_KICK_ROLE;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->dwRoleID = pMsg->dwRoleID;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);
    pCmd->pfunc = CB_KickRole;

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] Kick Role[%u] Begin, CmdID[%d]", pTLUser->GetID(), pCmd->dwRoleID, pCmd->dwSerialID);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLSendMail(CTLUser *pTLUser, const char *pcData)
{
    INT32           nResult = errGMUnknownError;
    INT32           nRetCode = 0;
    CMD_SEND_MAIL   *pCmd = NULL;
    TL2GM_SEND_MAIL *pMsg = (TL2GM_SEND_MAIL*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_SEND_MAIL *)CCmdMgr::GetInstance().NewCmd(GM_CMD_SEND_MAIL);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_SEND_MAIL;
    pCmd->eCmdState = GM_LS_SEND_MAIL;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->dwRoleID = pMsg->dwRoleID;
    pCmd->dwItemID = pMsg->dwItemID;
    pCmd->nItemNum = pMsg->nItemNum;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);
    strxcpy(pCmd->szTitle, pMsg->szSubject, sizeof(pCmd->szTitle));
    strxcpy(pCmd->szContent, pMsg->szContent, sizeof(pCmd->szContent));
    strxcpy(pCmd->szReason, pMsg->szReason, sizeof(pCmd->szReason));
    pCmd->pfunc = CB_SendMail;

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);
    
    DBG("TLUserID[%u] CmdID[%d] Send Mail to Role[%u] ItemID[%u] ItemNum[%d] Title[%s] Content[%s] Reason[%s] Begin", 
        pTLUser->GetID(), pCmd->dwSerialID, pCmd->dwRoleID, pCmd->dwItemID, pCmd->nItemNum, pCmd->szTitle, pCmd->szContent, pCmd->szReason);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLAnnounce(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_ANNOUNCE        *pCmd = NULL;
    TL2GM_ANNOUNCE      *pMsg = (TL2GM_ANNOUNCE*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_ANNOUNCE*)CCmdMgr::GetInstance().NewCmd(GM_CMD_ANNOUNCE);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_ANNOUNCE;
    pCmd->eCmdState = GM_LS_ANNOUNCE;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->pfunc = CB_Announce;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);
    pCmd->nGroupID = pMsg->nGroupID;
    pCmd->dwBeginTime = pMsg->dwBeginTime;
    pCmd->dwIntervalTime = pMsg->dwIntervalTime;
    pCmd->nRepeatNum = pMsg->nRepeatNum;
    strxcpy(pCmd->szText, pMsg->szText, sizeof(pCmd->szText));

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);
    
    DBG("TLUserID[%u] CmdID[%d] Announce[%s] BeginTime[%u] IntervalTime[%u] RepeatNum[%d] Begin", 
        pTLUser->GetID(), pCmd->dwSerialID, pCmd->szText, pCmd->dwBeginTime, pCmd->dwIntervalTime, pCmd->nRepeatNum);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLGetAnnounceList(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_GET_ANNOUNCE_LIST   *pCmd = NULL;
    TL2GM_GET_ANNOUNCE_LIST *pMsg = (TL2GM_GET_ANNOUNCE_LIST*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_GET_ANNOUNCE_LIST*)CCmdMgr::GetInstance().NewCmd(GM_CMD_GET_ANNOUNCE_LIST);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_GET_ANNOUNCE_LIST;
    pCmd->eCmdState = GM_LS_MS_QUERY_ANNOUNCE;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->pfunc = CB_GetAnnounceList;
    pCmd->nCount = 0;
    pCmd->nGroupID = pMsg->nGroupID;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] CmdID[%d] Get Announce List Begin", pTLUser->GetID(), pCmd->dwSerialID);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLDelAnnouce(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_DEL_ANNOUNCE   *pCmd = NULL;
    TL2GM_DEL_ANNOUNCE *pMsg = (TL2GM_DEL_ANNOUNCE*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_DEL_ANNOUNCE*)CCmdMgr::GetInstance().NewCmd(GM_CMD_DEL_ANNOUNCE);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_DEL_ANNOUNCE;
    pCmd->eCmdState = GM_LS_DEL_ANNOUNCE;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->pfunc = CB_DelAnnounce;
    pCmd->nGroupID = pMsg->nGroupID;
    pCmd->dwAnnounceSerial = pMsg->dwAnnounceSerial;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] CmdID[%u] Del AnnounceID[%u] Begin", pTLUser->GetID(), pCmd->dwSerialID, pCmd->dwAnnounceSerial);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLLog(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_LOG             *pCmd = NULL;
    TL2GM_LOG           *pMsg = (TL2GM_LOG*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (CMD_LOG*)CCmdMgr::GetInstance().NewCmd(GM_CMD_LOG);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_LOG;
    pCmd->eCmdState = GM_LS_QUERY_LOG;
    pCmd->nTimeout = time(NULL) + GM_SVR_LONG_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->pfunc = CB_Log;
    pCmd->bTruncated = FALSE;
    pCmd->nLogType = pMsg->nLogType;
    pCmd->dwBeginTime = pMsg->dwBeginTime;
    pCmd->dwEndTime = pMsg->dwEndTime;
    pCmd->nLength = 0;
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);
    memset(pCmd->szQueryStr, 0, sizeof(pCmd->szQueryStr));
    strxcpy(pCmd->szQueryStr, pMsg->szQueryStr, sizeof(pCmd->szQueryStr));
    memset(pCmd->szBuffer, 0, sizeof(pCmd->szBuffer));

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] CmdID[%u] Query Log LogType[%d] BeginTime[%u] EndTime[%u] Begin", 
        pTLUser->GetID(), pCmd->dwSerialID, pCmd->nLogType, pCmd->dwBeginTime, pCmd->dwEndTime);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLExeGMStr(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_EXE_GM_STR      *pCmd = NULL;
    TL2GM_EXE_GM_STR    *pMsg = (TL2GM_EXE_GM_STR*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pcData);

    pCmd = (CMD_EXE_GM_STR*)CCmdMgr::GetInstance().NewCmd(GM_CMD_EXE_GM_STR);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_EXE_GM_STR;
    pCmd->eCmdState = GM_LS_EXE_SYS_CMD;
    pCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->pfunc = CB_ExeGMStr;
    strxcpy(pCmd->szGMStr, pMsg->szStr, strlen(pMsg->szStr) + 1);
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] CmdID[%u] Exe GM Str CMD[%s] Begin", pTLUser->GetID(), pCmd->dwSerialID, pCmd->szGMStr);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLLoadRole(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_LOAD_ROLE       *pCmd = NULL;
    INT32               nStep = 0;
    CRole               *poRole = NULL;
    TL2GM_LOAD_ROLE_DATA    *pMsg = (TL2GM_LOAD_ROLE_DATA*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    poRole = CGameWorld::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_ECHO_ERROR(poRole == NULL, pTLUser->GetSessionID(), "Role is currently in use, please try again later");

    pCmd = (CMD_LOAD_ROLE*)CCmdMgr::GetInstance().NewCmd(GM_CMD_LOAD_ROLE);
    LOG_ECHO_ERROR(pCmd, pTLUser->GetSessionID(), "Memory exhausted");

    nRetCode = CGMRoleMgr::GetInstance().CheckRoleExist(pMsg->dwRoleID);
    LOG_ECHO_ERROR(nRetCode == FALSE, pTLUser->GetSessionID(), "Role data is already in use");

    nRetCode = CGMRoleMgr::GetInstance().AddRole(pMsg->dwRoleID, pTLUser->GetID());
    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to lock role");

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_LOAD_ROLE;
    pCmd->eCmdState = GM_LS_MS_CHECK_ROLE_FREEZE;
    pCmd->nTimeout = time(NULL) + GM_SVR_LONG_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->pfunc = CB_LoadRoleData;
    pCmd->dwRoleID = pMsg->dwRoleID;
    pCmd->nLength = 0;
    memset(pCmd->szData, 0, sizeof(pCmd->szData));
    strxcpy(pCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);
    
    pCmd->poRole = CGameWorld::GetInstance().NewRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pCmd->poRole);

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to insert into queue");

    INF("TLUserID[%u] cmdID[%u] load role[%u] data begin", pTLUser->GetID(), pCmd->dwSerialID, pCmd->dwRoleID);

    return;
Exit0:
    if (pCmd && pCmd->poRole)
    {
        CGameWorld::GetInstance().DelRole(pCmd->poRole);
        pCmd->poRole = NULL;
    }
    if (pTLUser)
    {
        nRetCode = CGMRoleMgr::GetInstance().DelRole(pMsg->dwRoleID, pTLUser->GetID());
        LOG_CHECK_ERROR(nRetCode);
    }

    if (pCmd)
        CCmdMgr::GetInstance().DeleteCmd(pCmd);

    return;
}

void CMessageHandler::OnTLModifyRole(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_MODIFY_ROLE     *pModifyCmd = NULL;
    CRole               *poRole = NULL;
    TL2GM_MODIFY_ROLE_DATA    *pMsg = (TL2GM_MODIFY_ROLE_DATA*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    poRole = CGameWorld::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_ECHO_ERROR(poRole == NULL, pTLUser->GetSessionID(), "Role is currently in use, please try again later");

    if (pMsg->dwTargetRoleID)
    {
        poRole = CGameWorld::GetInstance().FindRole(pMsg->dwTargetRoleID);
        LOG_ECHO_ERROR(poRole == NULL, pTLUser->GetSessionID(), "Target role for item transfer is currently in use, please try again later");
    }

    nRetCode = CGMRoleMgr::GetInstance().CheckRoleLock(pMsg->dwRoleID, pTLUser->GetID());
    LOG_ECHO_ERROR(nRetCode == TRUE, pTLUser->GetSessionID(), "Role data is not locked");

    if (pMsg->dwTargetRoleID != 0)
    {
        nRetCode = CGMRoleMgr::GetInstance().AddRole(pMsg->dwTargetRoleID, pTLUser->GetID());
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to lock target role, target role is being operated by another user");
    }

    pModifyCmd = (CMD_MODIFY_ROLE*)CCmdMgr::GetInstance().NewCmd(GM_CMD_MODIFY_ROLE);
    LOG_PROCESS_ERROR(pModifyCmd);

    pModifyCmd->dwTLUserID = pTLUser->GetID();
    pModifyCmd->eCmdType = GM_CMD_MODIFY_ROLE;
    pModifyCmd->eCmdState = GM_LS_MS_DEL_ROLE_CACHE;
    pModifyCmd->nTimeout = time(NULL) + GM_SVR_LONG_TIMEOUT;
    pModifyCmd->nResult = errGMUnknownError;
    pModifyCmd->pfunc = CB_ModifyRoleData;
    pModifyCmd->bClearTargetRoleCache = FALSE;
    pModifyCmd->poRole = NULL;
    pModifyCmd->poSelfRole = NULL;
    pModifyCmd->poTargetRole = NULL;
    pModifyCmd->dwRoleID = pMsg->dwRoleID;
    pModifyCmd->dwTargetRoleID = pMsg->dwTargetRoleID;
    pModifyCmd->nSlotCount = 0;
    pModifyCmd->nMailCount = 0;
    pModifyCmd->nLength = pMsg->nLength;
    pModifyCmd->nLogLength = 0;
    memset(pModifyCmd->szLog, 0, sizeof(pModifyCmd->szLog));
    memset(pModifyCmd->szData, 0, sizeof(pModifyCmd->szData));
    memcpy(pModifyCmd->szData, pMsg->szData, pMsg->nLength);
    strxcpy(pModifyCmd->szReason, pMsg->szReason, sizeof(pModifyCmd->szReason));
    strxcpy(pModifyCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);

    pModifyCmd->poSelfRole = CGameWorld::GetInstance().NewRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pModifyCmd->poSelfRole);

    if (pMsg->dwTargetRoleID > 0)
    {
        pModifyCmd->poTargetRole = CGameWorld::GetInstance().NewRole(pMsg->dwTargetRoleID);
        LOG_PROCESS_ERROR(pModifyCmd->poTargetRole);
    }

    nRetCode = CKernel::GetInstance().PushCmd(pModifyCmd);
    LOG_PROCESS_ERROR(nRetCode);

    INF("TLUserID[%u] MODIFY Role[%u] Data Begin", pTLUser->GetID(), pMsg->dwRoleID);

    return;
Exit0:
    if (pModifyCmd && pModifyCmd->poSelfRole)
    {
        CGameWorld::GetInstance().DelRole(pModifyCmd->poSelfRole);
        pModifyCmd->poSelfRole = NULL;
    }

    if (pModifyCmd && pModifyCmd->poTargetRole)
    {
        CGameWorld::GetInstance().DelRole(pModifyCmd->poTargetRole);
        pModifyCmd->poTargetRole = NULL;
    }

    if (pTLUser)
        INF("TLUserID[%u] MODIFY Role[%u] Data failed", pTLUser->GetID(), pMsg->dwRoleID);

    return;
}

void CMessageHandler::OnTLReleaseRole(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    TL2GM_RELEASE_ROLE    *pMsg = (TL2GM_RELEASE_ROLE*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    nRetCode = CGMRoleMgr::GetInstance().CheckRoleLock(pMsg->dwRoleID, pTLUser->GetID());
    LOG_ECHO_ERROR(nRetCode == TRUE, pTLUser->GetSessionID(), "Role data is not locked");

    nRetCode = CGMRoleMgr::GetInstance().DelRole(pMsg->dwRoleID, pTLUser->GetID());
    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to unlock role data");

    INF("TL:[%u] Role[%u] Release Role Data Success", pTLUser->GetID(), pMsg->dwRoleID);

    return;
Exit0:
    if (pTLUser && pMsg)
    {
        INF("TL:[%u] Role[%u] Release Role Data Failed", pTLUser->GetID(), pMsg->dwRoleID);
    }
    return;
}

void CMessageHandler::OnTLQueryNameToID(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CMD_QUERY_NAME_TO_ID    *pQueryCmd = NULL;
    TL2GM_QUERY_NAME_TO_ID  *pMsg = (TL2GM_QUERY_NAME_TO_ID*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pQueryCmd = (CMD_QUERY_NAME_TO_ID*)CCmdMgr::GetInstance().NewCmd(GM_CMD_QUERY_NAME_TO_ID);
    LOG_PROCESS_ERROR(pQueryCmd);

    pQueryCmd->dwTLUserID = pTLUser->GetID();
    pQueryCmd->eCmdType = GM_CMD_QUERY_NAME_TO_ID;
    pQueryCmd->eCmdState = GM_LS_QUERY_NAME_LIST;
    pQueryCmd->nTimeout = time(NULL) + GM_SVR_TIMEOUT;
    pQueryCmd->nResult = errGMUnknownError;
    pQueryCmd->pfunc = CB_QueryNameToID;
    pQueryCmd->dwRoleID = 0;
    pQueryCmd->nIndex = pMsg->nIndex;
    strxcpy(pQueryCmd->szRoleName, pMsg->szRoleName, sizeof(pQueryCmd->szRoleName));
    strxcpy(pQueryCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);

    nRetCode = CKernel::GetInstance().PushCmd(pQueryCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] CmdID[%u] QUERY ROLE[%s] NAME TO ID Begin", pTLUser->GetID(), pQueryCmd->dwSerialID, pMsg->szRoleName);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLRoleExeCmd(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    CRole               *poRole = NULL;
    CMD_ROLE_EXE_GM_CMD    *pRoleExeCmd = NULL;
    TL2GM_ROLE_EXE_CMD  *pMsg = (TL2GM_ROLE_EXE_CMD*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    poRole = CGameWorld::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_ECHO_ERROR(poRole == NULL, pTLUser->GetSessionID(), "Role is currently in use, please try again later");

    nRetCode = CGMRoleMgr::GetInstance().CheckRoleExist(pMsg->dwRoleID);
    LOG_ECHO_ERROR(nRetCode == FALSE, pTLUser->GetSessionID(), "Role data has been used");

    nRetCode = CGMRoleMgr::GetInstance().AddRole(pMsg->dwRoleID, pTLUser->GetID());
    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to lock role");

    pRoleExeCmd = (CMD_ROLE_EXE_GM_CMD*)CCmdMgr::GetInstance().NewCmd(GM_CMD_ROLE_EXE_GM_CMD);
    LOG_PROCESS_ERROR(pRoleExeCmd);

    pRoleExeCmd->dwTLUserID = pTLUser->GetID();
    pRoleExeCmd->eCmdType = GM_CMD_ROLE_EXE_GM_CMD;
    pRoleExeCmd->eCmdState = GM_LS_MS_CHECK_ROLE_FREEZE;
    pRoleExeCmd->nTimeout = time(NULL) + GM_SVR_LONG_TIMEOUT;
    pRoleExeCmd->nResult = errGMUnknownError;
    pRoleExeCmd->pfunc = CB_RoleExeCmd;
    pRoleExeCmd->dwRoleID = pMsg->dwRoleID;
    strxcpy(pRoleExeCmd->szGM, pMsg->szGM, sizeof(pRoleExeCmd->szGM));
    strxcpy(pRoleExeCmd->szReason, pMsg->szReason, sizeof(pRoleExeCmd->szReason));
    strxcpy(pRoleExeCmd->szUserName, pTLUser->GetUserName(), ROLE_NAME_LEN);

    pRoleExeCmd->poRole = CGameWorld::GetInstance().NewRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pRoleExeCmd->poRole);

    nRetCode = CKernel::GetInstance().PushCmd(pRoleExeCmd);
    LOG_PROCESS_ERROR(nRetCode);

    INF("TLUserID[%u] ROLE[%u] EXE GM CMD[%s] Reason[%s] Begin", pTLUser->GetID(), pRoleExeCmd->dwRoleID, pMsg->szGM, pMsg->szReason);

    return;
Exit0:
    if (pRoleExeCmd && pRoleExeCmd->poRole)
    {
        CGameWorld::GetInstance().DelRole(pRoleExeCmd->poRole);
        pRoleExeCmd->poRole = NULL;
    }
    return;
}

void CMessageHandler::OnTLSelectGroup(CTLUser *pTLUser, const char *pcData)
{
    INT32       nRetCode = 0;
    TL2GM_SELECT_GROUP  *pMsg = (TL2GM_SELECT_GROUP*)pcData;

    CLDListener::GetInstance().ChangeGroupID(pTLUser->GetID(), pMsg->byGroupID);

    INF("TLUser[%u] select group [%d]", pTLUser->GetID(), (INT32)pMsg->byGroupID);

    nRetCode = DoTLSelectGroupAck(pTLUser->GetSessionID(), pMsg->byGroupID);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnTLQueryExchangePriceLog(CTLUser *pTLUser, const char *pcData)
{
    INT32       nRetCode = 0;
    TL2GM_EXCHANGE_PRICE    *pMsg = (TL2GM_EXCHANGE_PRICE*)pcData;

    DBG("TLUser[%u] query group[%d] exchange log", pTLUser->GetID(), (INT32)pMsg->byGroupID);

    nRetCode = CDBMgr::GetInstance().QueryExchangeLog(pTLUser->GetSessionID(), pMsg->byGroupID);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnTLQueryExchangeHistory(CTLUser *pTLUser, const char *pcData)
{
    INT32       nRetCode = 0;
    TL2GM_EXCHANGE_PRICE    *pMsg = (TL2GM_EXCHANGE_PRICE*)pcData;

    DBG("TLUser[%u] query group[%d] exchange history", pTLUser->GetID(), (INT32)pMsg->byGroupID);

    nRetCode = CDBMgr::GetInstance().QueryExchangeHistory(pTLUser->GetSessionID(), pMsg->byGroupID);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnTLQueryOrderDetail(CTLUser *pTLUser, const char *pcData)
{
    INT32       nRetCode = 0;
    TL2GM_ORDER_DETAIL  *pMsg = (TL2GM_ORDER_DETAIL*)pcData;

    INF("TLUser[%u] group[%d] query order price[%d], order type[%d] exchange log", 
        pTLUser->GetID(), (INT32)pMsg->byGroupID, pMsg->nPrice, (INT32)pMsg->byOrderType);

    nRetCode = DoMSQueryOrderDetail(pMsg->byGroupID, pMsg->nPrice, pMsg->byOrderType, pTLUser->GetID());
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnTLAddSystemOrder(CTLUser *pTLUser, const char *pcData)
{
    INT32       nRetCode = 0;
    TL2GM_ADD_SYSTEM_ORDER  *pMsg = (TL2GM_ADD_SYSTEM_ORDER*)pcData;

    LOG_PROCESS_ERROR(pTLUser);

    INF("TLUser[%u] group[%d] add system order to ms price[%d] count[%d]", pTLUser->GetID(), (INT32)pMsg->byGroupID, pMsg->nPrice, pMsg->nCount);

    nRetCode = DoMSAddSystemOrder(pTLUser->GetID(), pMsg->byGroupID, pMsg->nPrice, pMsg->nCount);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnTLDelSystemOrder(CTLUser *pTLUser, const char *pcData)
{
    INT32       nRetCode = 0;
    TL2GM_DEL_SYSTEM_ORDER  *pMsg = (TL2GM_DEL_SYSTEM_ORDER*)pcData;

    LOG_PROCESS_ERROR(pTLUser);

    INF("TLUser[%u] group[%d] del system order to ms orderID[%llu]", pTLUser->GetID(), (INT32)pMsg->byGroupID, pMsg->qwOrderID);

    nRetCode = DoMSDelSystemOrder(pTLUser->GetID(), pMsg->byGroupID, pMsg->qwOrderID);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnTLSystemExchangeInfo(CTLUser *pTLUser, const char *pcData)
{
    INT32               nResult = errGMUnknownError;
    INT32               nRetCode = 0;
    QUERY_SYSTEM_EXCHANGE_INFO  *pCmd = NULL;
    TL2GM_SYS_EXCHANGE_INFO  *pMsg = (TL2GM_SYS_EXCHANGE_INFO*)pcData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTLUser);

    pCmd = (QUERY_SYSTEM_EXCHANGE_INFO*)CCmdMgr::GetInstance().NewCmd(GM_CMD_SYS_EXCHANGE_INFO);
    LOG_PROCESS_ERROR(pCmd);

    pCmd->dwTLUserID = pTLUser->GetID();
    pCmd->eCmdType = GM_CMD_SYS_EXCHANGE_INFO;
    pCmd->eCmdState = GM_LS_SYS_EXCHANGE_ORDER;
    pCmd->nTimeout = time(NULL) + GM_SVR_LONG_TIMEOUT;
    pCmd->nResult = errGMUnknownError;
    pCmd->pfunc = CB_QuerySysExchangeInfo;
    pCmd->byGroupID = pMsg->byGroupID;
    pCmd->byFlag = pMsg->byFlag;
    pCmd->dwBeginDayTime = pMsg->dwBeginDayTime;
    pCmd->dwEndDayTime = pMsg->dwEndDayTime;
    pCmd->dwBeginHourTime = pMsg->dwBeginHourTime;
    pCmd->dwEndHourTime = pMsg->dwEndHourTime;
    pCmd->llSysTotalMoney = 0;
    pCmd->llSysTotalExchangeMoney = 0;
    pCmd->llSysExchangeRMB = 0;
    pCmd->llTotalMoney = 0;
    pCmd->llTotalExchangeMoney = 0;
    pCmd->llExchangeRMB = 0;
    pCmd->nAveragePriceToday = 0;
    pCmd->nAveragePriceLastHour = 0;

    nRetCode = CKernel::GetInstance().PushCmd(pCmd);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("TLUserID[%u] group[%d] query sys exchange order, begin day time[%u], end day time[%u], begin hour time[%u], end hour time[%u]",
        pTLUser->GetID(), (INT32)pMsg->byGroupID, pMsg->dwBeginDayTime, pMsg->dwEndDayTime, pMsg->dwBeginHourTime, pMsg->dwEndHourTime);

    return;
Exit0:
    return;
}

void CMessageHandler::OnTLQueryExchangeLimit(CTLUser *pTLUser, const char *pcData)
{
    INT32   nRetCode = 0;
    TL2GM_GET_EXCHANGE_LIMIT    *pMsg = (TL2GM_GET_EXCHANGE_LIMIT*)pcData;

    // check privilege
    LOG_PROCESS_ERROR(pTLUser->GetPermission() == GM_PERMISSION_A);

    // send to ms
    nRetCode = CMessageHandler::GetInstance().DoMSQueryExchangeLimit(pTLUser->GetID(), pMsg->byGroupID);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnTLSetExchangeLimit(CTLUser *pTLUser, const char *pcData)
{
    INT32       nRetCode = 0;
    TL2GM_SET_EXCHANGE_LIMIT *pMsg = (TL2GM_SET_EXCHANGE_LIMIT*)pcData;

    // check privilege
    LOG_PROCESS_ERROR(pTLUser->GetPermission() == GM_PERMISSION_A);

    // send to ms
    nRetCode = CMessageHandler::GetInstance().DoMSSetExchangeLimit(pTLUser->GetID(), pMsg->byGroupID, 
        pMsg->nPriceHigh, pMsg->nPriceLow, pMsg->nSingleOrderLimit, pMsg->nDailyOrderLimit);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

BOOL CMessageHandler::DoMSRegisterAck( )
{
    GM2M_REGISTER_ACK   * pMsg{};
    INT32       nRetCode = 0;

    SMSSession  *pMSSession  = CGMSessionMgr::GetInstance().GetMSSession();
    LOG_PROCESS_ERROR(pMSSession);
    LOG_PROCESS_ERROR(pMSSession->pSocketSession);
    LOG_PROCESS_ERROR(pMSSession->eState == egmssNormal);

    pMsg = (GM2M_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2M_REGISTER_ACK));
    LOG_PROCESS_ERROR(pMsg);
    
    pMsg->wMsg = gm2m_register_ack;

    nRetCode = pMSSession->pSocketSession->Send((PACKET_BUFFER*)pMsg, sizeof(GM2M_REGISTER_ACK), 0);
    LOG_PROCESS_ERROR(nRetCode == sizeof(GM2M_REGISTER_ACK));

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSClearRoleCache(UINT32 dwTLUserID, UINT32 dwRoleID, UINT32 dwCmdSerialID)
{
    INT32   nRetCode = 0;
    GM2M_CLEAR_ROLE_CACHE      *pMsg = NULL;

    pMsg = (GM2M_CLEAR_ROLE_CACHE*)m_pPacketAllocator->Alloc(sizeof(GM2M_CLEAR_ROLE_CACHE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_clear_role_cache;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_CLEAR_ROLE_CACHE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSUnlockRole(UINT32 dwRoleID, const char *pcszUserName)
{
    INT32       nRetCode = 0;
    GM2M_UNLOCK_ROLE    *pMsg = NULL;
    
    LOG_PROCESS_ERROR(pcszUserName);
    
    pMsg = (GM2M_UNLOCK_ROLE*)m_pPacketAllocator->Alloc(sizeof(GM2M_UNLOCK_ROLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_unlock_role;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szUserName, pcszUserName, sizeof(pMsg->szUserName));


    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_UNLOCK_ROLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSQueryRoleData(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2M_QUERY_ROLE_DATA    *pMsg = NULL;

    pMsg = (GM2M_QUERY_ROLE_DATA*)m_pPacketAllocator->Alloc(sizeof(GM2M_QUERY_ROLE_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_query_role_data;
    pMsg->dwTLUser = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_QUERY_ROLE_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSForbidChat(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwTime, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    GM2M_FORBID_CHAT        *pMsg = NULL;

    pMsg = (GM2M_FORBID_CHAT *)m_pPacketAllocator->Alloc(sizeof(GM2M_FORBID_CHAT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_forbid_chat;
    pMsg->dwTLUser = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwTime = dwTime;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_FORBID_CHAT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSFreezeRole(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwTime, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    GM2M_FREEZE_ROLE        *pMsg = NULL;

    pMsg = (GM2M_FREEZE_ROLE *)m_pPacketAllocator->Alloc(sizeof(GM2M_FREEZE_ROLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_freeze_role;
    pMsg->dwTLUser = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwTime = dwTime;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_FREEZE_ROLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSKickRole(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    GM2M_KICK_ROLE        *pMsg = NULL;

    pMsg = (GM2M_KICK_ROLE *)m_pPacketAllocator->Alloc(sizeof(GM2M_KICK_ROLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_kick_role;
    pMsg->dwTLUser = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_KICK_ROLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSSendMail(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID, const char *pcszSubject, const char *pcszContent, UINT32 dwItemID, INT32 nItemNum)
{
    INT32       nRetCode = 0;
    GM2M_SEND_MAIL        *pMsg = NULL;

    LOG_PROCESS_ERROR(pcszSubject);
    LOG_PROCESS_ERROR(pcszContent);

    pMsg = (GM2M_SEND_MAIL *)m_pPacketAllocator->Alloc(sizeof(GM2M_SEND_MAIL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_send_mail;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwItemID = dwItemID;
    pMsg->nItemNum = nItemNum;
    strxcpy(pMsg->szSubject, pcszSubject, sizeof(pMsg->szSubject));
    strxcpy(pMsg->szContent, pcszContent, sizeof(pMsg->szContent));

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_SEND_MAIL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSAnnounce(INT32 nGroupID, UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwBeginTime, INT32 nRepeatNum, UINT32 dwIntervalTime, const char *pcszBuffer)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    GM2M_ANNOUNCE *pMsg = NULL;

    LOG_PROCESS_ERROR(pcszBuffer);
    nLength = strlen(pcszBuffer) + 1;
    LOG_PROCESS_ERROR(nLength + sizeof(GM2M_ANNOUNCE) * 2 < MAX_MESSAGE_SIZE);

    pMsg = (GM2M_ANNOUNCE *)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_announce;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->nGroupID = nGroupID;
    pMsg->dwBeginTime = dwBeginTime;
    pMsg->nRepeatNum = nRepeatNum;
    pMsg->dwIntervalTime = dwIntervalTime;
    strxcpy(pMsg->szText, pcszBuffer, nLength);

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_ANNOUNCE) + nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSGetAnnounceList(INT32 nGroupID, UINT32 dwTLUserID, UINT32 dwCmdSerialID)
{
    INT32       nRetCode = 0;
    GM2M_GET_ANNOUNCE_LIST  *pMsg = (GM2M_GET_ANNOUNCE_LIST*)m_pPacketAllocator->Alloc(sizeof(GM2M_GET_ANNOUNCE_LIST));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_get_announce_list;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->nGroupID = nGroupID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_GET_ANNOUNCE_LIST));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSDelAnnounce(INT32 nGroupID, UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwAnnounceSerial)
{
    INT32       nRetCode = 0;
    GM2M_DEL_ANNOUNCE   *pMsg = (GM2M_DEL_ANNOUNCE*)m_pPacketAllocator->Alloc(sizeof(GM2M_DEL_ANNOUNCE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_del_announce;
    pMsg->nGroupID = nGroupID;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwAnnounceSerial = dwAnnounceSerial;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_DEL_ANNOUNCE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSExeGMStrCmd(UINT32 dwTLUserID, UINT32 dwCmdSerialID, const char *pcszGMCmd)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    GM2M_EXE_GM_STR *pMsg = NULL;

    LOG_PROCESS_ERROR(pcszGMCmd);
    nLength = strlen(pcszGMCmd) + 1;
    LOG_PROCESS_ERROR(nLength + sizeof(GM2M_EXE_GM_STR) * 2 < MAX_MESSAGE_SIZE);

    pMsg = (GM2M_EXE_GM_STR *)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_exe_gm_str;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    strxcpy(pMsg->szGMStr, pcszGMCmd, nLength);

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_EXE_GM_STR) + nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSDelMail(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID, UINT32 dwMailID)
{
    INT32       nRetCode = 0;
    GM2M_DEL_MAIL        *pMsg = NULL;
    pMsg = (GM2M_DEL_MAIL *)m_pPacketAllocator->Alloc(sizeof(GM2M_DEL_MAIL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_del_mail;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwMailID = dwMailID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_DEL_MAIL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoCheckRoleFreeze(UINT32 dwTLUserID, UINT32 dwCmdSerialID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2M_CHECK_ROLE_FREEZE        *pMsg = NULL;
    pMsg = (GM2M_CHECK_ROLE_FREEZE *)m_pPacketAllocator->Alloc(sizeof(GM2M_CHECK_ROLE_FREEZE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_check_role_freeze;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->dwCmdSerialID = dwCmdSerialID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_CHECK_ROLE_FREEZE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSGetExchangePrice(INT32 nGroupID)
{
    INT32   nRetCode = 0;

    GM2M_GET_EXCHANGE_PRICE *pMsg = (GM2M_GET_EXCHANGE_PRICE*)m_pPacketAllocator->Alloc(sizeof(GM2M_GET_EXCHANGE_PRICE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_get_exchange_price;
    pMsg->byGroupID = nGroupID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_GET_EXCHANGE_PRICE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSQueryOrderDetail(UINT8 byGroupID, INT32 nPrice, UINT8 byOrderType, UINT32 dwTLUserID)
{
    INT32       nRetCode = 0;

    GM2M_GET_EXCHANGE_PRICE_DETAIL  *pMsg = (GM2M_GET_EXCHANGE_PRICE_DETAIL*)m_pPacketAllocator->Alloc(sizeof(GM2M_GET_EXCHANGE_PRICE_DETAIL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_get_exchange_price_detail;

    pMsg->byGroupID = byGroupID;
    pMsg->byOrderType = byOrderType;
    pMsg->nPrice = nPrice;
    pMsg->dwTLUserID = dwTLUserID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_GET_EXCHANGE_PRICE_DETAIL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSAddSystemOrder(UINT32 dwTLUserID, INT32 nGroupID, INT32 nPrice, INT32 nCount)
{
    INT32   nRetCode = 0;

    GM2M_ADD_SYSTEM_ORDER *pMsg = (GM2M_ADD_SYSTEM_ORDER*)m_pPacketAllocator->Alloc(sizeof(GM2M_ADD_SYSTEM_ORDER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_add_system_order;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byGroupID = nGroupID;
    pMsg->nPrice = nPrice;
    pMsg->nCount = nCount;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_ADD_SYSTEM_ORDER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSDelSystemOrder(UINT32 dwTLUserID, INT32 nGroupID, UINT64 qwOrderID)
{
    INT32   nRetCode = 0;

    GM2M_CANCEL_SYSTEM_ORDER *pMsg = (GM2M_CANCEL_SYSTEM_ORDER*)m_pPacketAllocator->Alloc(sizeof(GM2M_CANCEL_SYSTEM_ORDER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_cancel_system_order;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byGroupID = nGroupID;
    pMsg->qwOrderID = qwOrderID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_CANCEL_SYSTEM_ORDER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSQueryExchangeLimit(UINT32 dwTLUserID, INT32 nGroupID)
{
    INT32   nRetCode = 0;

    GM2M_GET_EXCHANGE_LIMIT *pMsg = (GM2M_GET_EXCHANGE_LIMIT*)m_pPacketAllocator->Alloc(sizeof(GM2M_GET_EXCHANGE_LIMIT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_get_exchange_limit;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byGroupID = nGroupID;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_GET_EXCHANGE_LIMIT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoMSSetExchangeLimit(UINT32 dwTLUserID, INT32 nGroupID, INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit)
{
    INT32   nRetCode = 0;

    GM2M_SET_EXCHANGE_LIMIT *pMsg = (GM2M_SET_EXCHANGE_LIMIT*)m_pPacketAllocator->Alloc(sizeof(GM2M_SET_EXCHANGE_LIMIT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2m_set_exchange_limit;
    pMsg->dwTLUserID = dwTLUserID;
    pMsg->byGroupID = nGroupID;
    pMsg->nPriceHigh = nPriceHigh;
    pMsg->nPriceLow = nPriceLow;
    pMsg->nSingleOrderLimit = nSingleOrderLimit;
    pMsg->nDailyOrderLimit = nDailyOrderLimit;

    nRetCode = SendMS((PACKET_BUFFER*)pMsg, sizeof(GM2M_SET_EXCHANGE_LIMIT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLRegisterAck(UINT32 dwTLSessionID, INT32 nResult)
{
    INT32       nRetCode = 0;
    GM2TL_REGISTER_ACK  *pMsg = (GM2TL_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_REGISTER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_register_ack;
    pMsg->nResult = nResult;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_REGISTER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLVerifyAck(UINT32 dwTLSessionID, INT32 nResult, BOOL bLD)
{
    INT32       nRetCode = 0;
    GM2TL_VERIFY_ACK     *pMsg = (GM2TL_VERIFY_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_VERIFY_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_verify_ack;
    pMsg->nResult = nResult;
    pMsg->bLD = bLD;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_VERIFY_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLLoginAck(UINT32 dwTLSessionID, INT32 nResult)
{
    INT32       nRetCode = 0;
    GM2TL_LOGIN_ACK     *pMsg = (GM2TL_LOGIN_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_LOGIN_ACK));
    LOG_PROCESS_ERROR(pMsg);


    pMsg->wMsg = gm2tl_login_ack;
    pMsg->nResult = nResult;

    pMsg->nMSCount = 0;
    memset(pMsg->stMSInfo, 0, sizeof(pMsg->stMSInfo));
    if (errSuccess == nResult)
    {
        pMsg->stMSInfo[pMsg->nMSCount].dwIndex = 1;
        strxcpy(pMsg->stMSInfo[pMsg->nMSCount].szMSName, g_ServerConfig.Common.szAreaName, sizeof(pMsg->stMSInfo[pMsg->nMSCount].szMSName));
        ++pMsg->nMSCount;
        //for (INT32 i = 1; i <= g_GMConfig.MSList.nSvrNum; ++i)
        //{
        //    pMsg->stMSInfo[pMsg->nMSCount].dwIndex = g_GMConfig.MSList.arrSvrList[i].nSvrID;
        //    strxcpy(pMsg->stMSInfo[pMsg->nMSCount].szMSName, g_GMConfig.MSList.arrSvrList[i].szName, 
        //        sizeof(pMsg->stMSInfo[pMsg->nMSCount].szMSName));
        //    ++pMsg->nMSCount;
        //}
    }

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_LOGIN_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLLogoutAck(UINT32 dwTLSessionID)
{
    INT32       nRetCode = 0;
    GM2TL_LOGOUT_ACK * pMsg = (GM2TL_LOGOUT_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_LOGOUT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_logout_ack;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_LOGOUT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLItemListNtf(UINT32 dwTLSessionID)
{
    INT32       nRetCode = 0;
    const   ITEM_DATA   *pData = NULL;
    GM2TL_ITEM_LIST_NTF *pMsg = (GM2TL_ITEM_LIST_NTF*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_item_list_ntf;
    pMsg->bCLS = TRUE;
    pMsg->nCount = 0;

    pData = CDataManager<ITEM_DATA>::GetInstance().GetFirstData();
    while (pData)
    {
        if (pData->m_nTemplateID > 0)
        {
            pMsg->data[pMsg->nCount].dwTempID = pData->m_nTemplateID;
            strxcpy(pMsg->data[pMsg->nCount].szName, pData->m_szName, sizeof(pMsg->data[pMsg->nCount].szName));
            ++pMsg->nCount;

            if (pMsg->nCount == 200)
            {
                nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_ITEM_LIST_NTF) + pMsg->nCount * sizeof(pMsg->data[0]));
                LOG_PROCESS_ERROR(nRetCode);

                Sleep(1);

                pMsg = (GM2TL_ITEM_LIST_NTF*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
                LOG_PROCESS_ERROR(pMsg);

                pMsg->wMsg = gm2tl_item_list_ntf;
                pMsg->bCLS = FALSE;
                pMsg->nCount = 0;
            }
        }
        pData = CDataManager<ITEM_DATA>::GetInstance().GetNextData();
    }

    pData = CDataManager<EQUIP_DATA>::GetInstance().GetFirstData();
    while (pData)
    {
        if (pData->m_nTemplateID > 0)
        {
            pMsg->data[pMsg->nCount].dwTempID = pData->m_nTemplateID;
            strxcpy(pMsg->data[pMsg->nCount].szName, pData->m_szName, sizeof(pMsg->data[pMsg->nCount].szName));
            ++pMsg->nCount;

            if (pMsg->nCount == 200)
            {
                nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_ITEM_LIST_NTF) + pMsg->nCount * sizeof(pMsg->data[0]));
                LOG_PROCESS_ERROR(nRetCode);

                pMsg = (GM2TL_ITEM_LIST_NTF*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
                LOG_PROCESS_ERROR(pMsg);

                pMsg->wMsg = gm2tl_item_list_ntf;
                pMsg->bCLS = FALSE;
                pMsg->nCount = 0;
            }
        }
        pData = CDataManager<EQUIP_DATA>::GetInstance().GetNextData();
    }

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_ITEM_LIST_NTF) + pMsg->nCount * sizeof(pMsg->data[0]));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLPreQueryAck(UINT32 dwTLSessionID, const char *pcszRoleName, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2TL_PRE_QUERY_ACK *pMsg = NULL;

    LOG_PROCESS_ERROR(pcszRoleName);

    pMsg = (GM2TL_PRE_QUERY_ACK *)m_pPacketAllocator->Alloc(sizeof(GM2TL_PRE_QUERY_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_pre_query_ack;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szRoleName, pcszRoleName, sizeof(pMsg->szRoleName));

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_PRE_QUERY_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLQueryRoleIDAck(UINT32 dwTLSessionID, INT32 nCount, GM_ROLE_KEY *pRoleKey)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    INT32       nSendCount = 0;
    GM2TL_QUERY_ROLE_ID_ACK *pMsg = NULL;
    
    pMsg = (GM2TL_QUERY_ROLE_ID_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_query_role_id_ack;
    pMsg->nAreaID = g_ServerConfig.Common.nAreaID;

    if (pRoleKey)
    {
        for (INT32 nIndex = 0; nIndex < nCount; ++nIndex)
        {
            pMsg->RoleKey[nSendCount].dwRoleID = pRoleKey[nIndex].dwRoleID;
            strxcpy(pMsg->RoleKey[nSendCount].szRoleName, pRoleKey[nIndex].szRoleName, sizeof(pMsg->RoleKey[nIndex].szRoleName));
            strxcpy(pMsg->RoleKey[nSendCount].szUserName, pRoleKey[nIndex].szUserName, sizeof(pMsg->RoleKey[nIndex].szUserName));
            ++nSendCount;
            if (nSendCount == MAX_GM_ROLE_KEY_NUM)
            {
                pMsg->byIsEnd = 1;
                break;
            }
        }
    }

    pMsg->nCount = nSendCount;
    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_QUERY_ROLE_ID_ACK) + sizeof(GM_ROLE_KEY_INFO) * nSendCount);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLQueryRoleDataAck(UINT32 dwTLSessionID, UINT32 dwRoleID, BOOL bIsOnline, INT32 nLength, const char *pData)
{
    INT32           nRetCode = 0;

    GM2TL_QUERY_ROLE_DATA_ACK *pMsg = NULL;
    
    pMsg = (GM2TL_QUERY_ROLE_DATA_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_query_role_data_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nAreaID = g_ServerConfig.Common.nAreaID;
    pMsg->bIsOnline = bIsOnline;
    pMsg->nLength = nLength;
    if (pData != NULL)
        memcpy(pMsg->szData, pData, nLength);

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_QUERY_ROLE_DATA_ACK) + nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoTLForbidChatAck(UINT32 dwTLSessionID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    GM2TL_FORBID_CHAT_ACK   *pMsg = (GM2TL_FORBID_CHAT_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_FORBID_CHAT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_forbid_chat_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nResult = errSuccess;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_FORBID_CHAT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLFreezeRoleAck(UINT32 dwTLSessionID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2TL_FREEZE_ROLE_ACK   *pMsg = (GM2TL_FREEZE_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_FREEZE_ROLE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_freeze_role_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nResult = errSuccess;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_FREEZE_ROLE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoTLKickRoleAck(UINT32 dwTLSessionID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2TL_KICK_ROLE_ACK   *pMsg = (GM2TL_KICK_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_KICK_ROLE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_kick_role_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nResult = errSuccess;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_KICK_ROLE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLSendMailAck(UINT32 dwTLSessionID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2TL_SEND_MAIL_ACK   *pMsg = (GM2TL_SEND_MAIL_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_SEND_MAIL_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_send_mail_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nResult = errSuccess;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_SEND_MAIL_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLAnnounceAck(INT32 nGroupID, UINT32 dwTLSessionID, INT32 nResult)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    GM2TL_ANNOUNCE_ACK   *pMsg = (GM2TL_ANNOUNCE_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_ANNOUNCE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_announce_ack;
    pMsg->nGroupID = nGroupID;
    pMsg->nAreaID = g_ServerConfig.Common.nAreaID;
    pMsg->nResult = nResult;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_ANNOUNCE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLGetAnnounceListAck(INT32 nGroupID, UINT32 dwTLSessionID, CMD_DATA_BASE *pCmd)
{
    INT32   nRetCode = 0;
    INT32   nLength = 0;
    CMD_GET_ANNOUNCE_LIST   *pAnnounceCmd = NULL;
    GM2TL_GET_ANNOUNCE_LIST_ACK *pMsg = (GM2TL_GET_ANNOUNCE_LIST_ACK *)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pAnnounceCmd = (CMD_GET_ANNOUNCE_LIST*)pCmd;

    pMsg->wMsg = gm2tl_get_announce_list_ack;
    pMsg->nCount = 0;
    pMsg->nGroupID = nGroupID;

    if (pAnnounceCmd)
    {
        pMsg->nCount = pAnnounceCmd->nCount;
        for (INT32 i = 0; i < pMsg->nCount; ++i)
        {
            pMsg->data[i].dwIntervalTime = pAnnounceCmd->data[i].dwIntervalTime;
            pMsg->data[i].dwRepeatNum = pAnnounceCmd->data[i].dwRepeatNum;
            pMsg->data[i].dwSerial = pAnnounceCmd->data[i].dwSerial;
            pMsg->data[i].nGroupID = pAnnounceCmd->data[i].nGroupID;
            strxcpy(pMsg->data[i].szText, pAnnounceCmd->data[i].szText, sizeof(pMsg->data[i].szText));

            DBG("TLUserID[%u] Announce List[%d], SerialID[%u] RepeatNum[%d] IntervalTime[%u] Content[%s]",
                pAnnounceCmd->dwTLUserID, i, pMsg->data[i].dwSerial, pMsg->data[i].dwRepeatNum, pMsg->data[i].dwIntervalTime, pMsg->data[i].szText);
        }
    }

    nLength = sizeof(GM2TL_GET_ANNOUNCE_LIST_ACK) + pMsg->nCount * sizeof(pMsg->data[0]);

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLDelAnnounceAck(INT32 nGroupID, UINT32 dwTLSessionID, UINT32 dwAnnounceSerial)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    GM2TL_DEL_ANNOUNCE_ACK   *pMsg = (GM2TL_DEL_ANNOUNCE_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_DEL_ANNOUNCE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_del_announce_ack;
    pMsg->nGroupID = nGroupID;
    pMsg->dwAnnounceSerial = dwAnnounceSerial;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_DEL_ANNOUNCE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLLogAck(UINT32 dwTLSessionID, INT32 nLogType, BOOL bTruncated, INT32 nLength, const char *pcszBuffer)
{
    INT32       nRetCode = 0;
    GM2TL_LOG_ACK       *pMsg = NULL;

    LOG_PROCESS_ERROR(pcszBuffer);
    
    pMsg = (GM2TL_LOG_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_log_ack;
    pMsg->nLogType = nLogType;
    pMsg->bTruncated = bTruncated;
    if (pcszBuffer)
    {
        strxcpy(pMsg->szBuffer, pcszBuffer, nLength + 1);
    }
    pMsg->nLength = nLength + 1;
    
    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_LOG_ACK) + nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLExeGMStrAck(UINT32 dwTLSessionID)
{
    INT32       nRetCode = 0;
    GM2TL_EXE_GM_STR_ACK       *pMsg = NULL;

    pMsg = (GM2TL_EXE_GM_STR_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_EXE_GM_STR_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_exe_gm_str_ack;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_EXE_GM_STR_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLLoadRoleDataAck(UINT32 dwTLSessionID, UINT32 dwRoleID, INT32 nLength, const char *pData)
{
    INT32           nRetCode = 0;
    GM2TL_LOAD_ROLE_DATA_ACK *pMsg = NULL;
    
    pMsg = (GM2TL_LOAD_ROLE_DATA_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_load_role_data_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nAreaID = g_ServerConfig.Common.nAreaID;
    pMsg->nLength = nLength;
    if (pData != NULL)
        memcpy(pMsg->szData, pData, nLength);

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_LOAD_ROLE_DATA_ACK) + nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLModifyRoleDataAck(UINT32 dwTLSessionID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2TL_MODIFY_ROLE_DATA_ACK       *pMsg = NULL;

    pMsg = (GM2TL_MODIFY_ROLE_DATA_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_MODIFY_ROLE_DATA_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_modify_role_data_ack;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_MODIFY_ROLE_DATA_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLQueryNameToIDAck(UINT32 dwTLSessionID, INT32 nIndex, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2TL_QUERY_NAME_TO_ID_ACK       *pMsg = NULL;

    pMsg = (GM2TL_QUERY_NAME_TO_ID_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_QUERY_NAME_TO_ID_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_query_name_to_id_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nIndex = nIndex;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_QUERY_NAME_TO_ID_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLErrorStr(UINT32 dwTLSessionID, const char *pcszErrStr)
{
    INT32           nRetCode = 0;
    INT32           nLength = 0;
    GM2TL_ERROR_STR *pMsg = NULL;
    
    LOG_PROCESS_ERROR(pcszErrStr);

    pMsg = (GM2TL_ERROR_STR*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_error_str;
    nLength = strlen(pcszErrStr) + 1;
    strxcpy(pMsg->szErr, pcszErrStr, nLength);

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_ERROR_STR) + nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLRoleExeGMCmdAck(UINT32 dwTLSessionID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    GM2TL_ROLE_EXE_GM_CMD       *pMsg = NULL;

    pMsg = (GM2TL_ROLE_EXE_GM_CMD*)m_pPacketAllocator->Alloc(sizeof(GM2TL_ROLE_EXE_GM_CMD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_role_exe_gm_cmd;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_ROLE_EXE_GM_CMD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLSelectGroupAck(UINT32 dwTLSessionID, UINT8 byGroupID)
{
    INT32       nRetCode = 0;
    GM2TL_SELECT_GROUP_ACK       *pMsg = NULL;

    pMsg = (GM2TL_SELECT_GROUP_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_SELECT_GROUP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_select_group_ack;
    pMsg->byGroupID = byGroupID;

    nRetCode = SendTL(dwTLSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_SELECT_GROUP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLQueryExchangeLogAck(UINT32 dwSessionID, UINT8 byGroupID, BOOL bClear, INT32 nCount, INT32 nLength, const char *pcData)
{
    INT32       nRetCode = 0;
    INT32       nMsgLength = 0;
    GM2TL_QUERY_EXCHANGE_LOG_ACK    *pMsg = NULL;

    LOG_PROCESS_ERROR(pcData);

    pMsg = (GM2TL_QUERY_EXCHANGE_LOG_ACK *)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_query_exchange_log_ack;
    pMsg->byGroupID = byGroupID;
    pMsg->byIsClear = bClear;
    pMsg->nCount = nCount;
    memcpy(pMsg->pBuffer, pcData, nLength);

    nMsgLength = sizeof(GM2TL_QUERY_EXCHANGE_LOG_ACK) + nLength;

    nRetCode = SendTL(dwSessionID, (PACKET_BUFFER*)pMsg, nMsgLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLQueryExchangeHistoryAck(UINT32 dwSessionID, UINT8 byGroupID, BOOL bClear, INT32 nCount, INT32 nLength, const char *pcData)
{
    INT32       nRetCode = 0;
    INT32       nMsgLength = 0;
    GM2TL_QUERY_EXCHANGE_HISTORY_ACK    *pMsg = NULL;

    LOG_PROCESS_ERROR(pcData);

    pMsg = (GM2TL_QUERY_EXCHANGE_HISTORY_ACK *)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_query_exchange_history_ack;
    pMsg->byGroupID = byGroupID;
    pMsg->byIsClear = bClear;
    pMsg->nCount = nCount;
    memcpy(pMsg->pBuffer, pcData, nLength);

    nMsgLength = sizeof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK) + nLength;

    nRetCode = SendTL(dwSessionID, (PACKET_BUFFER*)pMsg, nMsgLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLExchangePrice(INT32 nGroupID, INT32 nSellNum, INT32 nBuyNum, const char *pcData)
{
    INT32           nRetCode = 0;
    INT32           nLength = 0;
    INT32           i = 0;
    GM2TL_EXCHANGE_PRICE    *pMsg = NULL;
    EXCHANGE_PRICE_INFO *pData = (EXCHANGE_PRICE_INFO*)pcData;

    LOG_PROCESS_ERROR(pcData);

    pMsg = (GM2TL_EXCHANGE_PRICE*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_exchange_price;
    pMsg->byGroupID = nGroupID;
    pMsg->byBuyPriceCount = nBuyNum;
    pMsg->bySellPriceCount = nSellNum;

    for (; i < pMsg->bySellPriceCount; ++i)
    {
        pMsg->stPrices[i].nPrice = pData[i].nPrice;
        pMsg->stPrices[i].nCount = pData[i].nCount;
    }

    for (; i < pMsg->byBuyPriceCount + pMsg->bySellPriceCount; ++i)
    {
        pMsg->stPrices[i].nPrice = pData[i].nPrice;
        pMsg->stPrices[i].nCount = pData[i].nCount;
    }

    nLength = sizeof(GM2TL_EXCHANGE_PRICE) + (pMsg->byBuyPriceCount + pMsg->bySellPriceCount) * sizeof(pMsg->stPrices[0]);

    nRetCode = BroadcastLD(nGroupID, (PACKET_BUFFER*)pMsg, nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLQueryOrderDetailAck(UINT32 dwSessionID, UINT8 byGroupID, UINT8 byOrderType, INT32 nPrice, INT32 nCount, const char *pcData)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    InternalMessage::EXCHANGE_COUNT_INFO    *pInfo = NULL;
    GM2TL_QUERY_ORDER_DETAIL_ACK    *pMsg = (GM2TL_QUERY_ORDER_DETAIL_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pcData);

    pMsg->wMsg = gm2tl_query_order_detail_ack;
    pMsg->byGroupID = byGroupID;
    pMsg->byOrderType = byOrderType;
    pMsg->nPrice = nPrice;
    pMsg->byCountCount = nCount;

    pInfo = (InternalMessage::EXCHANGE_COUNT_INFO *)pcData;

    for (INT32 i = 0; i < nCount; i++)
    {
        pMsg->stCounts[i].dwRoleID = pInfo[i].dwRoleID;
        strxcpy(pMsg->stCounts[i].szRoleName, pInfo[i].szRoleName, ROLE_NAME_LEN);
        pMsg->stCounts[i].qwOrderID = pInfo[i].qwOrderID;
        pMsg->stCounts[i].nCount = pInfo[i].nCount;
    }

    nLength = sizeof(GM2TL_QUERY_ORDER_DETAIL_ACK) + sizeof(InternalMessage::EXCHANGE_COUNT_INFO) * pMsg->byCountCount;

    nRetCode = SendTL(dwSessionID, (PACKET_BUFFER*)pMsg, nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLAddSystemOrderAck(INT32 nGroupID, UINT64 qwOrderID, INT32 nResult, INT32 nPrice, INT32 nCount)
{
    INT32       nRetCode = 0;

    GM2TL_ADD_SYSTEM_ORDER_ACK  *pMsg = (GM2TL_ADD_SYSTEM_ORDER_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_ADD_SYSTEM_ORDER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_add_system_order_ack;
    pMsg->byGroupID = nGroupID;
    pMsg->qwOrderID = qwOrderID;
    pMsg->nResult = nResult;
    pMsg->nPrice = nPrice;
    pMsg->nCount = nCount;

    nRetCode = BroadcastLD((PACKET_BUFFER*)pMsg, sizeof(GM2TL_ADD_SYSTEM_ORDER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLDelSystemOrderAck(INT32 nGroupID, UINT64 qwOrderID, INT32 nResult, INT32 nPrice, INT32 nCount)
{
    INT32       nRetCode = 0;
    GM2TL_DEL_SYSTEM_ORDER_ACK  *pMsg = (GM2TL_DEL_SYSTEM_ORDER_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_DEL_SYSTEM_ORDER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_del_system_order_ack;
    pMsg->byGroupID = nGroupID;
    pMsg->qwOrderID = qwOrderID;
    pMsg->nResult = nResult;
    pMsg->nPrice = nPrice;
    pMsg->nCount = nCount;

    nRetCode = BroadcastLD((PACKET_BUFFER*)pMsg, sizeof(GM2TL_DEL_SYSTEM_ORDER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLQuerySystemExchangeInfo(UINT32 dwSessionID, 
    UINT32 dwBeginTime, UINT32 dwEndTime,
    UINT8 byGroupID, UINT8 byFlag, 
    INT64 llSysTotalMoney, INT64 llTotalMoney,
    INT64 llSysTotalExchangeMoney, INT64 llTotalExchangeMoney,
    INT64 llSysExchangeRMB, INT64 llExchangeRMB, 
    INT32 nAveragePriceToday, INT32 nAveragePriceLastHour)
{
    INT32       nRetCode = 0;
    GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK    *pMsg = (GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_query_sys_exchange_info_ack;
    pMsg->dwBeginTime = dwBeginTime;
    pMsg->dwEndTime = dwEndTime;
    pMsg->byGroupID = byGroupID;
    pMsg->byFlag = byFlag;
    pMsg->llSysTotalMoney = llSysTotalMoney;
    pMsg->llTotalMoney = llTotalMoney;

    pMsg->llSysTotalExchangeMoney = llSysTotalExchangeMoney;
    pMsg->llTotalExchangeMoney = llTotalExchangeMoney;

    pMsg->llSysExchangeRMB = llSysExchangeRMB;
    pMsg->llExchangeRMB = llExchangeRMB;

    pMsg->nAveragePriceLastHour = nAveragePriceLastHour;
    pMsg->nAveragePriceToday = nAveragePriceToday;

    nRetCode = SendTL(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLGetExchangeLimitAck(UINT32 dwSessionID, INT32 nGroup, INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit)
{
    INT32       nRetCode = 0;
    GM2TL_GET_EXCHANGE_LIMIT_ACK  *pMsg = (GM2TL_GET_EXCHANGE_LIMIT_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_GET_EXCHANGE_LIMIT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_get_exchange_limit_ack;
    pMsg->byGroupID = nGroup;
    pMsg->nPriceHigh = nPriceHigh;
    pMsg->nPriceLow = nPriceLow;
    pMsg->nSingleOrderLimit = nSingleOrderLimit;
    pMsg->nDailyOrderLimit = nDailyOrderLimit;

    nRetCode = SendTL(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_GET_EXCHANGE_LIMIT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoTLSetExchangeLimitAck(UINT32 dwSessionID, INT32 nGroup, INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit)
{
    INT32       nRetCode = 0;
    GM2TL_SET_EXCHANGE_LIMIT_ACK  *pMsg = (GM2TL_SET_EXCHANGE_LIMIT_ACK*)m_pPacketAllocator->Alloc(sizeof(GM2TL_SET_EXCHANGE_LIMIT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = gm2tl_set_exchange_limit_ack;
    pMsg->byGroupID = nGroup;
    pMsg->nPriceHigh = nPriceHigh;
    pMsg->nPriceLow = nPriceLow;
    pMsg->nSingleOrderLimit = nSingleOrderLimit;
    pMsg->nDailyOrderLimit = nDailyOrderLimit;

    nRetCode = SendTL(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(GM2TL_SET_EXCHANGE_LIMIT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
