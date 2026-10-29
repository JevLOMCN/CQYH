#include "stdafx.h"
#include "mm_message_handler.h"

#include "common_message.h"
#include "m2mm_message.h"
#include "mm2m_message.h"
#include "mg2mm_message.h"
#include "mm2mg_message.h"

#include "version.h"
#include "statis_info.h"
#include "time_ex.h"
#include "server_config.h"
#include "dump.h"

#include "mm_session_mgr.h"

using namespace InternalMessage;
CMMMesageHandler CMMMesageHandler::ms_Instance;

void CMMMesageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
	MG_SESSION* pMGSession{};
	LOG_PROCESS_ERROR(pSocketSession);
	LOG_PROCESS_ERROR(CMMSessionMgr::GetInstance().GetServerState() != sgsClosing);

	pMGSession = CMMSessionMgr::GetInstance().NewMGSession(pSocketSession);
	LOG_PROCESS_ERROR(pMGSession);

	pSocketSession->SetUserData(pMGSession);
	pMGSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;

	return;
Exit0:
	if (pSocketSession)
		pSocketSession->Close();

	return;
}

void CMMMesageHandler::OnConnected(ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	MM_BASE_SESSION* pBaseSession = NULL;

	LOG_PROCESS_ERROR(pSocketSession);

	pBaseSession = (MM_BASE_SESSION*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pBaseSession);

	pBaseSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;;

	switch (pBaseSession->dwType)
	{
	case stGD:
	{
		GD_SESSION* pGDSession = (GD_SESSION*)pBaseSession;
		pGDSession->bConnected = true;
		pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nRegisterGuardTimeout;

		if (CMMSessionMgr::GetInstance().GetServerState() == sgsSelfReady ||
			CMMSessionMgr::GetInstance().GetServerState() == sgsAllReady)
		{
			nRetCode = DoGDRegister();
			LOG_PROCESS_ERROR(nRetCode);
		}
	}
		break;
	case stMS:
	{
		MS_SESSION* pMSSession = (MS_SESSION*)pBaseSession;
		pMSSession->bConnected = true;

		nRetCode = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = DoMSRegister(nRetCode, g_ServerConfig.DB.dwPort);
		LOG_PROCESS_ERROR(nRetCode);
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

void CMMMesageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pPacketBuffer;
	LOG_PROCESS_ERROR(pHeader);

	g_ChangeStatisInfo(statInternalNetflowIncome, (INT32)dwSize);
	g_ChangeStatisInfo(statInternalPacketIncome, 1);

	LOG_PROCESS_ERROR(pSocketSession);
	LOG_PROCESS_ERROR(m_MessageFuncList[pHeader->wMsg]);

#if defined(WIN32)
	__try
	{
		(this->*m_MessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize, pSocketSession);
	}
	__except (DumpExceptionFilter(GetExceptionInformation()))
	{
		CRI("failed to process internal message id %d size %d", pHeader->wMsg, dwSize);
	}
#elif defined (__linux__)
	(this->*m_MessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize, pSocketSession);
#endif 

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

void CMMMesageHandler::OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown)
{
	MM_BASE_SESSION* pBaseSession = NULL;

	LOG_PROCESS_ERROR(pSocketSession);

	pBaseSession = (MM_BASE_SESSION*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pBaseSession);

	pBaseSession->pSocketSession = NULL;

	switch (pBaseSession->dwType)
	{
	case stGD:
	{
		GD_SESSION* pGDSession = (GD_SESSION*)pBaseSession;
		pGDSession->bConnected = false;
		pGDSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
		CRI("GD disconnected");
	}
		break;
	case stMS:
	{
		MS_SESSION* pMSSession = (MS_SESSION*)pBaseSession;
		pMSSession->bConnected = false;
		pMSSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nReconnectTime;
		m_ServerInfo.MM.byReady = FALSE;

		if (CMMSessionMgr::GetInstance().GetServerState() != sgsInitializing)
		{
			pMSSession->qwTimer = 0xFFFFFFFF;

			CRI("MS disconnected");

			CMMSessionMgr::GetInstance().SetServerState(sgsClosing);
		}
	}
		break;
	case stMG:
	{
		MG_SESSION* pMGSession = (MG_SESSION*)pBaseSession;
		pMGSession->qwTimer = 0xFFFFFFFF;

		m_ServerInfo.MG[pMGSession->dwSessionID].byReady = FALSE;

		CRI("MG %d disconnected", pMGSession->dwSessionID);
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

CMMMesageHandler::CMMMesageHandler()
{
	m_pNet = NULL;
	m_pPacketAllocator = NULL;
	memset(&m_ServerInfo, 0, sizeof(m_ServerInfo));
	memset(m_MessageFuncList, 0, sizeof(m_MessageFuncList));
}

CMMMesageHandler::~CMMMesageHandler()
{
	UnInit();
}

BOOL CMMMesageHandler::Init(INet* pNet, IPacketAllocator* pPacketAllocator)
{
	LOG_PROCESS_ERROR(pNet);
	LOG_PROCESS_ERROR(pPacketAllocator);

	m_pNet = pNet;
	m_pPacketAllocator = pPacketAllocator;

	m_MessageFuncList[a2a_ping] = &CMMMesageHandler::OnAllPing;

	m_MessageFuncList[m2mm_register_ack] = &CMMMesageHandler::OnMSRegisterAck;
	m_MessageFuncList[m2a_server_info] = &CMMMesageHandler::OnMSServerInfo;
	m_MessageFuncList[m2a_all_ready] = &CMMMesageHandler::OnMSAllReady;
	m_MessageFuncList[m2a_close] = &CMMMesageHandler::OnMSClose;

	m_MessageFuncList[mg2mm_register] = &CMMMesageHandler::OnMGRegister;
	m_MessageFuncList[mg2mm_ready] = &CMMMesageHandler::OnMGReady;

	memset(&m_ServerInfo, 0, sizeof(m_ServerInfo));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMMMesageHandler::UnInit(void)
{

	return TRUE;
}

CMMMesageHandler& CMMMesageHandler::GetInstance(void)
{
	return ms_Instance;
}

inline BOOL CMMMesageHandler::_SendGD(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;

	GD_SESSION& rGDSession = CMMSessionMgr::GetInstance().GetGDSession();
	LOG_PROCESS_ERROR(rGDSession.bConnected);
	LOG_PROCESS_ERROR(rGDSession.pSocketSession);

	nRetCode = rGDSession.pSocketSession->Send(pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
	g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL CMMMesageHandler::_SendMS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;

	MS_SESSION& rMSSession = CMMSessionMgr::GetInstance().GetMSSession();
	LOG_PROCESS_ERROR(rMSSession.bConnected);
	LOG_PROCESS_ERROR(rMSSession.pSocketSession);

	nRetCode = rMSSession.pSocketSession->Send(pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
	g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL CMMMesageHandler::_SendMG(UINT32 dwMGSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;

	MG_SESSION* pMGSession = CMMSessionMgr::GetInstance().GetMGSession(dwMGSessionID);
	LOG_PROCESS_ERROR(pMGSession);
	LOG_PROCESS_ERROR(pMGSession->pSocketSession);

	nRetCode = pMGSession->pSocketSession->Send(pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	g_ChangeStatisInfo(statInternalNetflowOutgoing, (INT32)dwSize);
	g_ChangeStatisInfo(statInternalPacketOutgoing, 1);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMMMesageHandler::_BroadcastMG(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	INT32 nRetCode = 0;
	PACKET_BUFFER* pMsg = NULL;

	for (INT32 i = 1; i < MAX_MG_SESSION_COUNT; ++i)
	{
		MG_SESSION* pMGSession = CMMSessionMgr::GetInstance().GetMGSession(i);
		LOG_PROCESS_ERROR(pMGSession);

		if (pMGSession->pSocketSession == NULL)
			continue;

		pMsg = m_pPacketAllocator->Alloc(dwSize);
		LOG_PROCESS_ERROR(pMsg);

		memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

		nRetCode = pMGSession->pSocketSession->Send(pMsg, dwSize, nOffset);
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
	if (pMsg)
		m_pPacketAllocator->Free(pMsg);
	if (pPacketBuffer)
		m_pPacketAllocator->Free(pPacketBuffer);
	return FALSE;
}

void CMMMesageHandler::_CheckAllReady(void)
{
	INT32 nRetCode = 0;

	for (INT32 nIndex = 1; nIndex <= g_ServerConfig.MM.nInitMGCount; nIndex++)
	{
		PROCESS_ERROR(m_ServerInfo.MG[nIndex].byReady);
	}
	PROCESS_ERROR(m_ServerInfo.MM.byReady);

	nRetCode = DoMSReady();
	LOG_PROCESS_ERROR(nRetCode);

	if (CMMSessionMgr::GetInstance().GetGDSession().bConnected)
	{
		nRetCode = DoGDRegister();
		LOG_PROCESS_ERROR(nRetCode);
	}

Exit0:
	return;
}

/************************************************************************/
// To All

BOOL CMMMesageHandler::DoAllPing(void)
{
	INT32 nRetCode = 0;
	A2A_PING* pMsg = NULL;

	if (CMMSessionMgr::GetInstance().GetGDSession().bConnected)
	{
		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;

		nRetCode = _SendGD((PACKET_BUFFER*)pMsg, sizeof(*pMsg));
		LOG_CHECK_ERROR(nRetCode);

		if (!nRetCode)
			m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
		pMsg = NULL;
	}

	if (CMMSessionMgr::GetInstance().GetMSSession().bConnected)
	{
		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;

		nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(*pMsg));
		LOG_CHECK_ERROR(nRetCode);

		if (!nRetCode)
			m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
		pMsg = NULL;
	}

	for (INT32 nIndex = 1; nIndex < MAX_MG_SESSION_COUNT; nIndex++)
	{
		MG_SESSION* pMGSession = CMMSessionMgr::GetInstance().GetMGSession(nIndex);
		if (pMGSession && pMGSession->pSocketSession)
		{
			pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
			LOG_PROCESS_ERROR(pMsg);

			pMsg->wMsg = a2a_ping;

			nRetCode = pMGSession->pSocketSession->Send((PACKET_BUFFER*)pMsg, sizeof(*pMsg), 0);
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
// To GD
BOOL CMMMesageHandler::DoGDRegister(void)
{
	INT32 nRetCode = 0;

	A2U_REGRISTER* pMsg = (A2U_REGRISTER*)m_pPacketAllocator->Alloc(sizeof(A2U_REGRISTER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = a2u_regrister;
	pMsg->byType = stMM;
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

BOOL CMMMesageHandler::DoMSRegister(UINT32 dwAddr, UINT16 wPort)
{
	INT32 nRetCode = 0;

	MM2M_REGISTER* pMsg = (MM2M_REGISTER*)m_pPacketAllocator->Alloc(sizeof(MM2M_REGISTER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = mm2m_register;
	pMsg->dwAddr = dwAddr;
	pMsg->wPort = wPort;

	nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(*pMsg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMMMesageHandler::DoMSReady(void)
{
	INT32 nRetCode = 0;

	MM2M_READY* pMsg = (MM2M_READY*)m_pPacketAllocator->Alloc(sizeof(MM2M_READY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = mm2m_ready;

	nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(*pMsg));
	LOG_PROCESS_ERROR(nRetCode);

	CMMSessionMgr::GetInstance().SetServerState(sgsSelfReady);
	INF("MM is self ready");

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMMMesageHandler::DoMSCloseAck(void)
{
	INT32 nRetCode = 0;

	MM2M_CLOSE_ACK* pMsg = NULL;

	LOG_PROCESS_ERROR(m_pPacketAllocator);

	pMsg = (MM2M_CLOSE_ACK*)m_pPacketAllocator->Alloc(sizeof(MM2M_CLOSE_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = mm2m_close_ack;

	nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(*pMsg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

/************************************************************************/
// To MG
BOOL CMMMesageHandler::DoMGRegisterAck(UINT32 dwMGSessionID)
{
	INT32 nRetCode = 0;

	MM2MG_REGISTER_ACK* pMsg = (MM2MG_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(MM2MG_REGISTER_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = mm2mg_register_ack;
	pMsg->dwMGSessionID = dwMGSessionID;

	nRetCode = _SendMG(dwMGSessionID, (PACKET_BUFFER*)pMsg, sizeof(MM2MG_REGISTER_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMMMesageHandler::DoMGServerInfo(UINT32 dwMGSessionID)
{
	INT32 nRetCode = 0;

	MM2MG_SERVER_INFO* pMsg = (MM2MG_SERVER_INFO*)m_pPacketAllocator->Alloc(sizeof(MM2MG_SERVER_INFO));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = mm2mg_server_info;
	pMsg->dwLSAddr = m_ServerInfo.LS.dwAddr;
	pMsg->wLSPort = m_ServerInfo.LS.wPort;

	if (dwMGSessionID == 0)
	{
		nRetCode = _BroadcastMG((PACKET_BUFFER*)pMsg, sizeof(MM2MG_SERVER_INFO));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = _SendMG(dwMGSessionID, (PACKET_BUFFER*)pMsg, sizeof(MM2MG_SERVER_INFO));
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CMMMesageHandler::DoMGAllReady(void)
{
	INT32 nRetCode = 0;

	MM2MG_ALL_READY* pMsg = (MM2MG_ALL_READY*)m_pPacketAllocator->Alloc(sizeof(MM2MG_ALL_READY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = mm2mg_all_ready;

	nRetCode = _BroadcastMG((PACKET_BUFFER*)pMsg, sizeof(MM2MG_ALL_READY));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

/************************************************************************/
// From All

void CMMMesageHandler::OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	MM_BASE_SESSION* pBaseSession = (MM_BASE_SESSION*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pBaseSession);

	if (pBaseSession->dwType == stGD)
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
void CMMMesageHandler::OnMSRegisterAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;

	M2MM_REGISTER_ACK* pMsg = (M2MM_REGISTER_ACK*)pPacketBuffer;

	m_ServerInfo.MM.byReady = TRUE;

	INF("MM registered to MS");

#if defined(WIN32)
	{
		char szTitle[1024];
#ifdef _FINAL
		snprintf(szTitle, 1024, "Mobile Server, ver %d.%d.%d final", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#else
		snprintf(szTitle, 1024, "Mobile Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#endif	// _FINAL
		SetConsoleTitle(szTitle);
		INF(szTitle);
	}
#endif  // WIN32

	_CheckAllReady();

Exit0:
	return;
}

void CMMMesageHandler::OnMSServerInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	M2A_SERVER_INFO* pMsg = (M2A_SERVER_INFO*)pPacketBuffer;

	m_ServerInfo.LS.dwAddr = pMsg->LS.dwAddr;
	m_ServerInfo.LS.wPort = pMsg->LS.wPort;

	nRetCode = DoMGServerInfo(0);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:

	return;
}

void CMMMesageHandler::OnMSAllReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;

	CMMSessionMgr::GetInstance().SetServerState(sgsAllReady);

	INF("all servers are ready");

	nRetCode = DoMGAllReady();
	LOG_CHECK_ERROR(nRetCode);

	return;
}

void CMMMesageHandler::OnMSClose(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	CMMSessionMgr::GetInstance().SetServerState(sgsClosing);
	INF("MM is closing");
}

/************************************************************************/
// From MG
void CMMMesageHandler::OnMGRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	MG2MM_REGISTER* pMsg = (MG2MM_REGISTER*)pPacketBuffer;

	MG_SESSION* pMGSession = (MG_SESSION*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pMGSession);
	LOG_PROCESS_ERROR(CMMSessionMgr::GetInstance().GetServerState() != sgsClosing);

	pMGSession->pSocketSession = pSocketSession;
	pMGSession->qwTimer = GetTickCountEx() + g_ServerConfig.Common.nServerPingTimeout;
	pSocketSession->SetUserData(pMGSession);

	nRetCode = DoMGRegisterAck(pMGSession->dwSessionID);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = DoMGServerInfo(pMGSession->dwSessionID);
	LOG_PROCESS_ERROR(nRetCode);

	m_ServerInfo.MG[pMGSession->dwSessionID].byReady = TRUE;

	INF("MG %d register to MM", pMGSession->dwSessionID);

	_CheckAllReady();

	return;
Exit0:
	if (pSocketSession)
		pSocketSession->Close();

	return;
}

void CMMMesageHandler::OnMGReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	MG2MM_READY* pMsg = (MG2MM_READY*)pPacketBuffer;

	MG_SESSION* pMGSession = (MG_SESSION*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pMGSession);
	LOG_PROCESS_ERROR(CMMSessionMgr::GetInstance().GetServerState() != sgsClosing);

	m_ServerInfo.MG[pMGSession->dwSessionID].byReady = TRUE;

	INF("MG %d is ready", pMGSession->dwSessionID);

	_CheckAllReady();

	return;
Exit0:
	if (pSocketSession)
		pSocketSession->Close();

	return;
}
