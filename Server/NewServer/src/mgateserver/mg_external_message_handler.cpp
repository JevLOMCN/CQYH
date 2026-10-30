#include "stdafx.h"
#include "mg_external_message_handler.h"

#include "c2mg_messsage.h"

#include "time_ex.h"
#include "dump.h"
#include "server_def.h"
#include "server_config.h"

#include "mg_session_mgr.h"
#include "mgateserver.h"

using namespace ExternalMessage;

CMGExternalMessageHandler CMGExternalMessageHandler::ms_Instance;

void CMGExternalMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
	INT32 nResult = errLoginUnknownError;
	INT32 nRetCode = 0;
	MG_CLIENT_SESSION* pSession = NULL;
	UINT32 dwAddr;
	UINT16 wPort;
	struct in_addr addr;

	PROCESS_ERROR_RET_CODE(
		CMGSessionMgr::GetInstance().GetServerState() == sgsAllReady,
		errLoginServerNotReady);

	pSession = CMGSessionMgr::GetInstance().NewClientSession();
	LOG_PROCESS_ERROR(pSession);

	pSocketSession->SetUserData(pSession);

	pSession->pSocketSession = pSocketSession;
	pSession->byShutDownType = csdtClient;
	pSession->bIpWhiteList = FALSE;
	pSession->qwTimer = GetTickCountEx() + g_ServerConfig.GT.nLoginAuthTimeout;

	nRetCode = pSocketSession->GetRemoteAddress(dwAddr, wPort);
	LOG_CHECK_ERROR(nRetCode);
	addr.s_addr = dwAddr;

	for (INT32 nIndex = 1; nIndex < sizeof(g_ServerConfig.Common.IpWhiteList) / sizeof(g_ServerConfig.Common.IpWhiteList[0]); nIndex++)
	{
		if (g_ServerConfig.Common.IpWhiteList[nIndex].szIP[0] == 0)
			break;

		UINT32 dwWhiteAddr = inet_addr(g_ServerConfig.Common.IpWhiteList[nIndex].szIP);
		if (dwWhiteAddr == INADDR_NONE)
			break;
		UINT32 dwWhiteMask = inet_addr(g_ServerConfig.Common.IpWhiteList[nIndex].szMask);

		if ((dwWhiteAddr & dwWhiteMask) == (dwAddr & dwWhiteMask))
		{
			pSession->bIpWhiteList = TRUE;
			break;
		}
	}

	INF("session ptr<%08X>, session <%08X> connected from %s:%d, total %d", (INT32)pSession, pSession->dwSessionID, inet_ntoa(addr), wPort,
		CMGSessionMgr::GetInstance().GetClientSessionCount());

	MG_SYS_LOG_SUCCESS(pSession);

	return;
Exit0:

	if (pSession)
		MG_SYS_LOG_ERROR(pSession);

	if (pSocketSession)
		pSocketSession->Close();

	return;
}

void CMGExternalMessageHandler::OnConnected(ISocketSession* pSocketSession)
{
	LOG_PROCESS_ERROR(FALSE);

Exit0:
	if (pSocketSession)
		pSocketSession->Close();

	return;
}

void CMGExternalMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	MG_CLIENT_SESSION* pSession = NULL;
	ExternalMessage::MESSAGE_HEADER* pHeader = (ExternalMessage::MESSAGE_HEADER*)pPacketBuffer;

	if (CMGSessionMgr::GetInstance().GetServerState() != sgsAllReady)
		return;

	pSession = (MG_CLIENT_SESSION*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pSession);

	LOG_PROCESS_ERROR(m_MessageFuncList[pHeader->wMsg]);

#if defined(WIN32) | defined (WIN64)
	__try
	{
		nRetCode = (this->*m_MessageFuncList[pHeader->wMsg])(pSession, pPacketBuffer, dwSize);
	}
	__except (DumpExceptionFilter(GetExceptionInformation()))
	{
		CRI("failed to process internal message %d size %d", pHeader->wMsg, dwSize);
	}
#elif defined (__linux__)
	nRetCode = (this->*m_MessageFuncList[pHeader->wMsg])(pSession, pPacketBuffer, dwSize);
#endif 
	LOG_PROCESS_ERROR(nRetCode);

	m_pPacketAllocator->Free(pPacketBuffer);

	return;
Exit0:
	if (pSession && pHeader)
		CRI("unknown external message id %d size %d from <%08X>", pHeader->wMsg, dwSize, pSession->dwSessionID);
	else if (pHeader)
		CRI("unknown external message id %d size %d from unknown session", pHeader->wMsg, dwSize);

	if (pPacketBuffer)
		m_pPacketAllocator->Free(pPacketBuffer);

	if (pSocketSession)
		pSocketSession->Close();

	if (pSession)
		pSession->byShutDownType = csdtMG;

	return;
}

void CMGExternalMessageHandler::OnDisconnected(ISocketSession* pSocketSession, BOOL bPassiveShutdown)
{
	INT32 nRetCode = 0;
	MG_CLIENT_SESSION* pSession = NULL;

	LOG_PROCESS_ERROR(pSocketSession);

	pSession = (MG_CLIENT_SESSION*)pSocketSession->GetUserData();
	PROCESS_ERROR(pSession);        // failed to accept

	pSession->pSocketSession = NULL;
	pSession->bConnected = FALSE;

Exit0:
	if (pSession)
	{
		UINT32 dwNoGroupSessionID = pSession->dwSessionID;
		SET_GROUP_INDEX_TO_SESSION_ID(dwNoGroupSessionID, 0);
		INF("<%08x> disconnected(no group id<%08X>),disconnect type(%d), total %d", pSession->dwSessionID, dwNoGroupSessionID, pSession->byShutDownType, CMGSessionMgr::GetInstance().GetClientSessionCount() - 1);
		MG_SYS_LOG_SUCCESS(pSession);
		CMGSessionMgr::GetInstance().DelClientSession(pSession);
	}

	return;
}

CMGExternalMessageHandler::CMGExternalMessageHandler()
{
	m_pNet = NULL;
	m_pPacketAllocator = NULL;
	m_pClientSocketAcceptor = NULL;
	memset(m_MessageFuncList, 0, sizeof(m_MessageFuncList));
}

CMGExternalMessageHandler::~CMGExternalMessageHandler()
{
	UnInit();
}

BOOL CMGExternalMessageHandler::Init(INet* pNet, IPacketAllocator* pPacketAllocator)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pNet);

	m_MessageFuncList[c2mg_message] = &CMGExternalMessageHandler::OnClientMessage;

	m_pNet = pNet;
	m_pPacketAllocator = pPacketAllocator;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMGExternalMessageHandler::UnInit(void)
{
	INT32 nRetCode = 0;

	m_pNet = NULL;
	m_pPacketAllocator = NULL;

	return TRUE;
}

CMGExternalMessageHandler& CMGExternalMessageHandler::GetInstance(void)
{
	return ms_Instance;
}

BOOL CMGExternalMessageHandler::Send(UINT32 dwSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	UINT32 msgID{};
	INT32 nRetCode = 0;

	MG_CLIENT_SESSION* pSession = CMGSessionMgr::GetInstance().FindClientSession(dwSessionID);
	PROCESS_ERROR(pSession);
	LOG_PROCESS_ERROR(pSession->pSocketSession);

	msgID = ((MESSAGE_HEADER*)(pPacketBuffer + nOffset))->wMsg;

	if (msgID >= ExternalMessage::max_message_count)
	{
		CRI("MSG ID %d too big!", msgID);
		LOG_PROCESS_ERROR(FALSE);
	}

	nRetCode = pSession->pSocketSession->Send(pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CMGExternalMessageHandler::OnClientMessage(MG_CLIENT_SESSION* pSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	C2MG_MESSAGE* pMsg = (C2MG_MESSAGE*)pPacketBuffer;



	return TRUE;
Exit0:

	return FALSE;
}
