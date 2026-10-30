#include "stdafx.h"
#include "statis_info.h"
#include "mg_session_mgr.h"

#include "time_ex.h"

#include "server_config.h"

#include "net.h"
#include "external_message_header.h"

#include "mg_internal_message_handler.h"
#include "mg_external_message_handler.h"

CMGSessionMgr CMGSessionMgr::ms_Instance;
static UINT64 s_qwLastTick = 0;

CMGSessionMgr::CMGSessionMgr()
{
	m_eServerState = sgsInvalid;
	m_dwMGSessionID = 0;
	m_qwLastPingTick = 0;
	m_pExternalNet = NULL;
	m_pInternalNet = NULL;
	memset(m_ClientSessionPool, 0, sizeof(m_ClientSessionPool));
	memset(&m_LSSession, 0, sizeof(m_LSSession));
	memset(&m_MMSession, 0, sizeof(m_MMSession));
	memset(&m_bOpened, 0, sizeof(m_bOpened));
}

CMGSessionMgr::~CMGSessionMgr()
{
	UnInit();
}

BOOL CMGSessionMgr::Init(INet* pExternalNet, INet* pInternalNet)
{
	LOG_PROCESS_ERROR(pExternalNet);
	LOG_PROCESS_ERROR(pInternalNet);

	memset(m_ClientSessionPool, 0, sizeof(m_ClientSessionPool));
	for (INT32 nIndex = 1; nIndex < MAX_CLIENT_SESSION_COUNT; nIndex++)
	{
		m_ClientSessionPool[nIndex].byType = stClient;
		m_ClientSessionPool[nIndex].dwSessionID = 0;
		m_FreeClientSessionList.push_back(m_ClientSessionPool + nIndex);
	}
	m_UsedClientSessionList.Clear();

	for (INT32 nIndex = 0; nIndex < MAX_SERVER_GROUP_COUNT; nIndex++)
	{
#if !defined(_FINAL) | defined(_DEBUG)
        if (IsValidGroup(nIndex))
			m_bOpened[nIndex] = TRUE;
#endif // _FINAL
	}

	memset(&m_LSSession, 0, sizeof(m_LSSession));
	m_LSSession.byType = stLS;
	memset(&m_MMSession, 0, sizeof(m_MMSession));
	m_MMSession.byType = stMM;
	memset(&m_GDSession, 0, sizeof(m_GDSession));
	m_GDSession.byType = stGD;

	m_dwMGSessionID = 0;
	m_qwLastPingTick = 0;
	m_eServerState = sgsInitializing;

	m_pExternalNet = pExternalNet;
	m_pInternalNet = pInternalNet;

	s_qwLastTick = GetTickCountEx();

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMGSessionMgr::UnInit(void)
{
	LOG_CHECK_ERROR(m_FreeClientSessionList.size() == MAX_CLIENT_SESSION_COUNT - 1);
	LOG_CHECK_ERROR(m_UsedClientSessionList.IsEmpty());

	m_FreeClientSessionList.clear();
	m_UsedClientSessionList.Clear();

	LOG_CHECK_ERROR(!CheckSession());
	return TRUE;
}

CMGSessionMgr& CMGSessionMgr::GetInstance(void)
{
	return ms_Instance;
}

MG_CLIENT_SESSION* CMGSessionMgr::NewClientSession(void)
{
	MG_CLIENT_SESSION* pResult = NULL;

	pResult = m_FreeClientSessionList.front();
	PROCESS_ERROR(pResult);
	LOG_PROCESS_ERROR(!pResult->bConnected);

	pResult->bConnected = true;
	m_FreeClientSessionList.pop_front();
	m_UsedClientSessionList.PushRear(pResult);

Exit0:
	return pResult;
}

void CMGSessionMgr::DelClientSession(MG_CLIENT_SESSION* pClientSession)
{
	LOG_PROCESS_ERROR(pClientSession);
	LOG_PROCESS_ERROR(!pClientSession->bConnected);

	SET_GROUP_INDEX_TO_SESSION_ID(pClientSession->dwSessionID, 0);
	CBaseList::Remove(pClientSession);
	m_FreeClientSessionList.push_back(pClientSession);

Exit0:
	return;
}

MG_CLIENT_SESSION* CMGSessionMgr::FindClientSession(UINT32 dwSessionID)
{
	UINT32 dwSessionIndex = dwSessionID & 0xFFFF;
	PROCESS_ERROR(m_ClientSessionPool[dwSessionIndex].bConnected);

	return m_ClientSessionPool + dwSessionIndex;
Exit0:
	return NULL;
}

INT32 CMGSessionMgr::GetClientSessionCount(void)
{
	return static_cast<INT32>(MAX_CLIENT_SESSION_COUNT - 1 - m_FreeClientSessionList.size());
}

BOOL CMGSessionMgr::CloseAllClientSession(void)
{
	MG_CLIENT_SESSION* pClientSession = (MG_CLIENT_SESSION*)m_UsedClientSessionList.Head().pNext;
	while (pClientSession)
	{
		if (pClientSession->pSocketSession)
		{
			pClientSession->pSocketSession->Close();
			pClientSession->byShutDownType = csdtGT;
		}

		pClientSession = (MG_CLIENT_SESSION*)pClientSession->pNext;
	}

	return TRUE;
}

BOOL CMGSessionMgr::IsAllClientSessionClosed(void)
{
	return m_UsedClientSessionList.IsEmpty();
}

BOOL CMGSessionMgr::MainLoop(void)
{
	UINT64 qwCurrentTick = GetTickCountEx();

	if (qwCurrentTick > m_qwLastPingTick + g_ServerConfig.Common.nServerPingInterval)
	{
		CMGInternalMessageHandler::GetInstance().DoAllPing();
		m_qwLastPingTick = qwCurrentTick;
	}

	if (m_GDSession.bConnected && m_GDSession.pSocketSession)
	{
		if (m_GDSession.qwTimer < qwCurrentTick)
		{
			CRI("GD ping time out");
			m_GDSession.pSocketSession->Close();
			m_GDSession.qwTimer = 0xFFFFFFFF;
		}
	}
	else if (m_GDSession.pSocketSession == NULL && m_GDSession.qwTimer < qwCurrentTick)
	{
#if defined(WIN32)
		INF("try to connect to GD");
		m_GDSession.pSocketSession = m_pInternalNet->Connect(inet_addr(g_szLocalAddress), g_ServerConfig.Common.dwGuardPort);
		if (m_GDSession.pSocketSession)
			m_GDSession.pSocketSession->SetUserData(&m_GDSession);
#endif
	}

	if (m_LSSession.bConnected && m_LSSession.pSocketSession)
	{
		if (m_LSSession.qwTimer < qwCurrentTick)
		{
			CRI("LS ping time out");
			m_LSSession.pSocketSession->Close();
			m_LSSession.qwTimer = 0xFFFFFFFF;
		}
	}
	else if (m_LSSession.pSocketSession == NULL &&
		m_LSSession.dwAddr && m_dwMGSessionID != 0 &&
		m_LSSession.qwTimer < qwCurrentTick &&
		CMGSessionMgr::GetInstance().GetServerState() != sgsClosing)
	{
		INF("try to connect to LS");
		m_LSSession.pSocketSession = m_pInternalNet->Connect(m_LSSession.dwAddr, m_LSSession.wPort);
		if (m_LSSession.pSocketSession)
			m_LSSession.pSocketSession->SetUserData(&m_LSSession);
	}

	if (m_MMSession.bConnected && m_MMSession.pSocketSession)
	{
		if (m_MMSession.qwTimer < qwCurrentTick)
		{
			CRI("MM ping time out");
			m_MMSession.pSocketSession->Close();
			m_MMSession.qwTimer = 0xFFFFFFFF;
		}
	}
	else if (m_MMSession.pSocketSession == NULL &&
		m_MMSession.qwTimer < qwCurrentTick &&
		CMGSessionMgr::GetInstance().GetServerState() != sgsClosing)
	{
		INF("try to connect to MM");

		UINT32 dwAddr = inet_addr(g_ServerConfig.MM.szAddress);
		if (dwAddr == 0)
			dwAddr = inet_addr(g_szLocalAddress);

		m_MMSession.pSocketSession = m_pInternalNet->Connect(dwAddr, g_ServerConfig.MM.dwMGPort);
		if (m_MMSession.pSocketSession)
			m_MMSession.pSocketSession->SetUserData(&m_MMSession);
	}

	return TRUE;
}

BOOL CMGSessionMgr::CheckSession()
{
	PROCESS_SUCCESS(m_MMSession.bConnected);
	PROCESS_SUCCESS(m_LSSession.bConnected);

	return FALSE;
Exit1:
	return TRUE;
}

BOOL CMGSessionMgr::CloseAllSession()
{
	if (m_MMSession.bConnected && m_MMSession.pSocketSession)
		m_MMSession.pSocketSession->Close();

	if (m_LSSession.bConnected && m_LSSession.pSocketSession)
		m_LSSession.pSocketSession->Close();

	return TRUE;
}