#include "stdafx.h"
#include "ls_message_handler.h"

#include "time_ex.h"

#include "error_def.h"

#include "server_config.h"

#include "l2t_message.h"
#include "t2l_message.h"
#include "l2m_message.h"
#include "m2l_message.h"
#include "l2mg_message.h"
#include "mg2l_message.h"
#include "common_message.h"
#include "login_server.h"

#include "ls_session_mgr.h"
#include "oa_auth_mgr.h"

using namespace InternalMessage;
CLSMessageHandler CLSMessageHandler::ms_Instance;

void CLSMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
    GT_SESSION* pSession = NULL;

    LOG_PROCESS_ERROR(pSocketSession);
    LOG_PROCESS_ERROR(CLSSessionMgr::GetInstance().GetServerState() != sgsClosing);

    return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();

    return;
}

void CLSMessageHandler::OnConnected(ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    LS_BASE_SESSION* pBaseSession = NULL;

    LOG_PROCESS_ERROR(pSocketSession);

    pBaseSession = (LS_BASE_SESSION*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pBaseSession);

    pBaseSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

    if(pBaseSession->dwType == stMS)
    {
        MS_SESSION* pMSSession = (MS_SESSION*)pBaseSession;
        pMSSession->bConnected = true;

		nRetCode = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = DoMSRegister(nRetCode, g_ServerConfig.LS.dwPort);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else if(pBaseSession->dwType == stGD)
    {
        GD_SESSION* pGDSession = (GD_SESSION*)pBaseSession;
        pGDSession->bConnected = true;
        pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nRegisterGuardTimeout;

        if(CLSSessionMgr::GetInstance().GetServerState() == sgsSelfReady || 
           CLSSessionMgr::GetInstance().GetServerState() == sgsAllReady)
        {
            nRetCode = DoGDRegister();
            LOG_PROCESS_ERROR(nRetCode);
        }
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

void CLSMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
    MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pPacketBuffer;

    LOG_PROCESS_ERROR(pSocketSession);
    LOG_PROCESS_ERROR(pHeader);

    LOG_PROCESS_ERROR(m_MessageFuncList[pHeader->wMsg]);

    (this->*m_MessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize, pSocketSession);

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

void CLSMessageHandler::OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown)
{
    INT32 nRetCode = 0;
    LS_BASE_SESSION* pBaseSession = NULL;

    LOG_PROCESS_ERROR(pSocketSession);

    pBaseSession = (LS_BASE_SESSION*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pBaseSession);

    pBaseSession->pSocketSession = NULL;

     switch (pBaseSession->dwType)
    {
     case stGD:
         {
             GD_SESSION* pGDSession = (GD_SESSION*)pBaseSession;
             pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
             pGDSession->bConnected = false;

             CRI("GD disconnected");
         }
         break;
    case stMS:
        {
            MS_SESSION* pMSSession = (MS_SESSION*)pBaseSession;
            pMSSession->bConnected = false;
            pMSSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;

            CRI("MS disconnected");
        }
        break;
    case stGT:
        {
            GT_SESSION* pGTSession = (GT_SESSION*)pBaseSession;
            pGTSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;

            CRI("GT %d disconnected", pGTSession->dwSessionID);
        }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
    }

    return;
Exit0:
    return;
}

CLSMessageHandler::CLSMessageHandler()
{
	m_pNet = NULL;
	m_pPacketAllocator = NULL;
	memset(m_MessageFuncList, 0 , sizeof(m_MessageFuncList));
}

CLSMessageHandler::~CLSMessageHandler()
{
    UnInit();
}

BOOL CLSMessageHandler::Init(INet* pNet, IPacketAllocator* pPacketAllocator)
{
    LOG_PROCESS_ERROR(pNet);

    m_pNet = pNet;
    m_pPacketAllocator = pPacketAllocator;

    m_MessageFuncList[a2a_ping] = &CLSMessageHandler::OnAllPing;

    m_MessageFuncList[m2l_register_ack] = &CLSMessageHandler::OnMSRegisterAck;
    m_MessageFuncList[m2l_login_query_ack] = &CLSMessageHandler::OnMSLoginQueryAck;

    m_MessageFuncList[m2a_all_ready] = &CLSMessageHandler::OnMSAllReady;
    m_MessageFuncList[m2a_server_info] = &CLSMessageHandler::OnMSServerInfo;
    m_MessageFuncList[m2a_close] = &CLSMessageHandler::OnMSClose;

    m_MessageFuncList[t2l_register] = &CLSMessageHandler::OnGTRegister;
    m_MessageFuncList[t2l_login_req] = &CLSMessageHandler::OnGTLoginReq;

	m_MessageFuncList[mg2l_register] = &CLSMessageHandler::OnMGRegister;
	m_MessageFuncList[mg2l_login_req] = &CLSMessageHandler::OnMGLoginReq;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLSMessageHandler::UnInit(void)
{

    return TRUE;
}

CLSMessageHandler& CLSMessageHandler::GetInstance(void)
{
    return ms_Instance;
}

inline BOOL CLSMessageHandler::_SendGD(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    GD_SESSION& rGDSession = CLSSessionMgr::GetInstance().GetGDSession();
    LOG_PROCESS_ERROR(rGDSession.bConnected);
    LOG_PROCESS_ERROR(rGDSession.pSocketSession);

    nRetCode = rGDSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}

inline BOOL CLSMessageHandler::_SendMS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    MS_SESSION& rMSSession = CLSSessionMgr::GetInstance().GetMSSession();
    LOG_PROCESS_ERROR(rMSSession.bConnected);
    LOG_PROCESS_ERROR(rMSSession.pSocketSession);

    nRetCode = rMSSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}

inline BOOL CLSMessageHandler::_SendGT(UINT32 dwGTSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;

    GT_SESSION* pGTSession = CLSSessionMgr::GetInstance().GetGTSession(dwGTSessionID);
    LOG_PROCESS_ERROR(pGTSession);
    LOG_PROCESS_ERROR(pGTSession->pSocketSession);

    nRetCode = pGTSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}

inline BOOL CLSMessageHandler::_SendMG(UINT32 dwMGSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;

	MG_SESSION* pMGSession = CLSSessionMgr::GetInstance().GetMGSession(dwMGSessionID);
	LOG_PROCESS_ERROR(pMGSession);
	LOG_PROCESS_ERROR(pMGSession->pSocketSession);

	nRetCode = pMGSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/
// To All
BOOL CLSMessageHandler::DoAllPing(void)
{
    INT32 nRetCode = 0;
    A2A_PING* pMsg = NULL;

    if (CLSSessionMgr::GetInstance().GetGDSession().bConnected)
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

    if (CLSSessionMgr::GetInstance().GetMSSession().bConnected)
    {
        pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
        LOG_PROCESS_ERROR(pMsg);

        pMsg->wMsg = a2a_ping;

        nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
    }

    for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
    {
        GT_SESSION* pGTSession = CLSSessionMgr::GetInstance().GetGTSession(nIndex);
        if (pGTSession && pGTSession->pSocketSession)
        {
            pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
            LOG_PROCESS_ERROR(pMsg);

            pMsg->wMsg = a2a_ping;

            nRetCode = _SendGT(pGTSession->dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
            LOG_CHECK_ERROR(nRetCode);

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
/************************************************************************/
// To Gd
BOOL CLSMessageHandler::DoGDRegister()
{
    INT32 nRetCode = 0;

    A2U_REGRISTER* pMsg = (A2U_REGRISTER*)m_pPacketAllocator->Alloc(sizeof(A2U_REGRISTER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = a2u_regrister;
    pMsg->byType = stLS;
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
// To MS
BOOL CLSMessageHandler::DoMSRegister(UINT32 dwGTAddr, UINT16 wGTPort)
{
    INT32 nRetCode = 0;

    L2M_REGISTER* pMsg = (L2M_REGISTER*)m_pPacketAllocator->Alloc(sizeof(L2M_REGISTER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = l2m_register;
    pMsg->dwAddr = dwGTAddr;
    pMsg->wPort = wGTPort;

    nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(L2M_REGISTER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CLSMessageHandler::DoMSReady(void)
{
    INT32 nRetCode = 0;

    L2M_READY* pMsg = (L2M_READY*)m_pPacketAllocator->Alloc(sizeof(L2M_READY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = l2m_ready;

    nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(L2M_READY));
    LOG_PROCESS_ERROR(nRetCode);

    CLSSessionMgr::GetInstance().SetServerState(sgsSelfReady);
    INF("LS is self ready");

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CLSMessageHandler::DoMSCloseAck(void)
{
    INT32 nRetCode = 0;

    L2M_CLOSE_ACK* pMsg = (L2M_CLOSE_ACK*)m_pPacketAllocator->Alloc(sizeof(L2M_CLOSE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = l2m_close_ack;

    nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(L2M_CLOSE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// LS asks MS to query account credentials in the database
BOOL CLSMessageHandler::DoMsLoginQueryReq(UINT32 dwClientSessionID, CHAR* szUserName, CHAR* szPassword)
{
    INT32 nRetCode = 0;

    L2M_LOGIN_QUERY_REQ* pMsg = (L2M_LOGIN_QUERY_REQ*)m_pPacketAllocator->Alloc(sizeof(L2M_LOGIN_QUERY_REQ));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = l2m_login_query_req;
    pMsg->dwClientSessionID = dwClientSessionID;
    strxcpy(pMsg->szUserName, szUserName, USER_NAME_LEN);
    strxcpy(pMsg->szPassword, szPassword, USER_PASSWORD_LEN);

    nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(L2M_LOGIN_QUERY_REQ));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

/************************************************************************/
// To GT
BOOL CLSMessageHandler::DoGTRegisterAck(UINT32 dwGTSession)
{
    INT32 nRetCode = 0;

    L2T_REGISTER_ACK* pMsg = (L2T_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(L2T_REGISTER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = l2t_register_ack;

    nRetCode = _SendGT(dwGTSession, (PACKET_BUFFER*)pMsg, sizeof(L2T_REGISTER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// Client login response
BOOL CLSMessageHandler::DoGTLoginAck(UINT32 dwClientSessionID, INT32 nErrorCode, 
                                     const CHAR* szOASessionID, const CHAR* szUserName, const CHAR* szPGT, UINT8 bAdult,
                                     UINT16 wInfoLen, const BYTE* abyInfo)
{
    INT32 nRetCode = 0;

    L2T_LOGIN_ACK* pMsg = (L2T_LOGIN_ACK*)m_pPacketAllocator->Alloc(sizeof(L2T_LOGIN_ACK) + wInfoLen);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = l2t_login_ack;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->nErrorCode = nErrorCode;
    strxcpy(pMsg->szOASessionID, szOASessionID, OA_SESSION_ID_LEN);
    strxcpy(pMsg->szUserName, szUserName, USER_NAME_LEN);
    strxcpy(pMsg->szPGT, szPGT, OA_PGT_FIELD_LEN);
    pMsg->bAdult = bAdult;
    if (abyInfo != nullptr)
    {
        pMsg->wInfoLen = wInfoLen;
        memcpy(pMsg->abyInfo, abyInfo, wInfoLen);
    }
    else
    {
        pMsg->wInfoLen = 0;
    }

    nRetCode = _SendGT(GET_GT_INDEX_FROM_SESSION_ID(dwClientSessionID), 
        (PACKET_BUFFER*)pMsg, sizeof(L2T_LOGIN_ACK) + wInfoLen);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

/************************************************************************/
// To MG
BOOL CLSMessageHandler::DoMGRegisterAck(UINT32 dwMGSession)
{
	INT32 nRetCode = 0;

	L2MG_REGISTER_ACK* pMsg = (L2MG_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(L2MG_REGISTER_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = l2mg_register_ack;

	nRetCode = _SendMG(dwMGSession, (PACKET_BUFFER*)pMsg, sizeof(L2MG_REGISTER_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CLSMessageHandler::DoMGLoginAck(UINT32 dwClientSessionID, INT32 nErrorCode,
	const CHAR* szOASessionID, const CHAR* szUserName, const CHAR* szPGT, UINT8 bAdult,
	UINT16 wInfoLen, const BYTE* abyInfo)
{
	INT32 nRetCode = 0;

	L2MG_LOGIN_ACK* pMsg = (L2MG_LOGIN_ACK*)m_pPacketAllocator->Alloc(sizeof(L2MG_LOGIN_ACK) + wInfoLen);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = l2mg_login_ack;
	pMsg->dwClientSessionID = dwClientSessionID;
	pMsg->nErrorCode = nErrorCode;
	strxcpy(pMsg->szOASessionID, szOASessionID, OA_SESSION_ID_LEN);
	strxcpy(pMsg->szUserName, szUserName, USER_NAME_LEN);
	strxcpy(pMsg->szPGT, szPGT, OA_PGT_FIELD_LEN);
	pMsg->bAdult = bAdult;
	if (abyInfo != nullptr)
	{
		pMsg->wInfoLen = wInfoLen;
		memcpy(pMsg->abyInfo, abyInfo, wInfoLen);
	}
	else
	{
		pMsg->wInfoLen = 0;
	}

	nRetCode = _SendMG(GET_GT_INDEX_FROM_SESSION_ID(dwClientSessionID),
		(PACKET_BUFFER*)pMsg, sizeof(L2T_LOGIN_ACK) + wInfoLen);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

/************************************************************************/
// From All
void CLSMessageHandler::OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    LS_BASE_SESSION* pBaseSession = (LS_BASE_SESSION*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pBaseSession);

    if(pBaseSession->dwType == stGD)
        pBaseSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nPingGuardTimeout;
    else
        pBaseSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

    return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();
    return;
}

/************************************************************************/
// From MS
void CLSMessageHandler::OnMSRegisterAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    M2L_REGISTER_ACK* pMsg = (M2L_REGISTER_ACK*)pPacketBuffer;
    
    nRetCode = DoMSReady();
    LOG_PROCESS_ERROR(nRetCode);

    if(CLSSessionMgr::GetInstance().GetGDSession().bConnected)
    {
        nRetCode = DoGDRegister();
        LOG_CHECK_ERROR(nRetCode);
    }

    INF("LS registered to MS");

Exit0:
    return;
}

void CLSMessageHandler::OnMSAllReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    CLSSessionMgr::GetInstance().SetServerState(sgsAllReady);
    INF("all servers are ready");
}

void CLSMessageHandler::OnMSServerInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
}

void CLSMessageHandler::OnMSClose(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    CLSSessionMgr::GetInstance().SetServerState(sgsClosing);
    INF("LS is closing");
}

// MS returns the account credential query result
void CLSMessageHandler::OnMSLoginQueryAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nResult = errLoginAuthFailed;
    INT32 nRetCode = 0;
    M2L_LOGIN_QUERY_ACK* pMsg = (M2L_LOGIN_QUERY_ACK*)pPacketBuffer;

    if (pMsg->dwError == 1)
    {
        nResult = errSuccess;
        DoGTLoginAck(pMsg->dwClientSessionID, nResult, "", pMsg->szUserName, "", 0, 0, NULL);
    }
    else if (pMsg->dwError == 0)
    {
        DoGTLoginAck(pMsg->dwClientSessionID, nResult, "", pMsg->szUserName, "", 0, 0, NULL);
    }
}

/************************************************************************/
// From GT
void CLSMessageHandler::OnGTRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    T2L_REGISTER* pMsg = (T2L_REGISTER*)pPacketBuffer;

    GT_SESSION* pGTSession = CLSSessionMgr::GetInstance().GetGTSession(pMsg->dwGTSessionID);
    LOG_PROCESS_ERROR(pGTSession);
    LOG_PROCESS_ERROR(pGTSession->pSocketSession == NULL);
    LOG_PROCESS_ERROR(CLSSessionMgr::GetInstance().GetServerState() != sgsClosing);

    pSocketSession->SetUserData(pGTSession);
    pGTSession->pSocketSession = pSocketSession;
    pGTSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

    nRetCode = DoGTRegisterAck(pMsg->dwGTSessionID);
    LOG_PROCESS_ERROR(nRetCode);

    INF("GT %d registered to LS", pMsg->dwGTSessionID);

Exit0:

    return;
}

void CLSMessageHandler::OnGTLoginReq(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    // Default code
    /*
    INT32 nResult = errLoginAuthFailed;
    INT32 nRetCode = 0;
    T2L_LOGIN_REQ* pMsg = (T2L_LOGIN_REQ*)pPacketBuffer;

    LOG_PROCESS_ERROR(CLSSessionMgr::GetInstance().GetServerState() == sgsAllReady);

	if (g_ServerConfig.LS.bOAAuthorizeEnable)
	{
		nRetCode = COAAuthMgr::GetInstance().DoAuthorize(pMsg->dwClientSessionID, pMsg->szOASessionID);
		LOG_PROCESS_ERROR(nRetCode);
        INF("User %s from session <%08X> DoAuthorize due to bOAAuthorizeEnable", pMsg->szUserName, pMsg->dwClientSessionID );
	}
	else
	{
		nResult = errSuccess;

		nRetCode = DoGTLoginAck(pMsg->dwClientSessionID, nResult, "", pMsg->szUserName, "", 0, 0, NULL);
		LOG_CHECK_ERROR(nResult);
	}

	return;
Exit0:	
	nRetCode = DoGTLoginAck(pMsg->dwClientSessionID, nResult, "", pMsg->szUserName, "", 0, 0, NULL);
	LOG_CHECK_ERROR(nResult);

    return;
    */

    INT32 nResult = errLoginAuthFailed;
    INT32 nRetCode = 0;
    T2L_LOGIN_REQ* pMsg = (T2L_LOGIN_REQ*)pPacketBuffer;

    // For login authentication, ask MS to query the account in the database
    nRetCode = CLSMessageHandler::GetInstance().DoMsLoginQueryReq(pMsg->dwClientSessionID, pMsg->szUserName, pMsg->szPassword);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    nRetCode = DoGTLoginAck(pMsg->dwClientSessionID, nResult, "", pMsg->szUserName, "", 0, 0, NULL);
    LOG_CHECK_ERROR(nResult);

    return;
}

/************************************************************************/
// From MG (mobile client)
void CLSMessageHandler::OnMGRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	MG2L_REGISTER* pMsg = (MG2L_REGISTER*)pPacketBuffer;

	MG_SESSION* pMGSession = CLSSessionMgr::GetInstance().GetMGSession(pMsg->dwMGSessionID);
	LOG_PROCESS_ERROR(pMGSession);
	LOG_PROCESS_ERROR(pMGSession->pSocketSession == NULL);
	LOG_PROCESS_ERROR(CLSSessionMgr::GetInstance().GetServerState() != sgsClosing);

	pSocketSession->SetUserData(pMGSession);
	pMGSession->pSocketSession = pSocketSession;
	pMGSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

	nRetCode = DoMGRegisterAck(pMsg->dwMGSessionID);
	LOG_PROCESS_ERROR(nRetCode);

	INF("MG %d registered to LS", pMsg->dwMGSessionID);

Exit0:

	return;
}

void CLSMessageHandler::OnMGLoginReq(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nResult = errLoginAuthFailed;
	INT32 nRetCode = 0;
	MG2L_LOGIN_REQ* pMsg = (MG2L_LOGIN_REQ*)pPacketBuffer;

	LOG_PROCESS_ERROR(CLSSessionMgr::GetInstance().GetServerState() == sgsAllReady);
	LOG_PROCESS_ERROR(IS_MOBILE_CLIENT(pMsg->dwClientSessionID));

	if (g_ServerConfig.LS.bOAAuthorizeEnable)
	{
		nRetCode = COAAuthMgr::GetInstance().DoAuthorize(pMsg->dwClientSessionID, pMsg->szOASessionID);
		LOG_PROCESS_ERROR(nRetCode);
		INF("User %s from session <%08X> DoAuthorize due to bOAAuthorizeEnable", pMsg->szUserName, pMsg->dwClientSessionID);
	}
	//#ifndef _FINAL
	else
	{
		nResult = errSuccess;

		nRetCode = DoMGLoginAck(pMsg->dwClientSessionID, nResult, "", pMsg->szUserName, "", 0, 0, NULL);
		LOG_CHECK_ERROR(nResult);
	}
	//#endif	// _FINAL

	return;
Exit0:
	nRetCode = DoMGLoginAck(pMsg->dwClientSessionID, nResult, "", pMsg->szUserName, "", 0, 0, NULL);
	LOG_CHECK_ERROR(nResult);

	return;
}
