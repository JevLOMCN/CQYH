#include "stdafx.h"
#include "statis_info.h"
#include "ls_session_mgr.h"

//#include <winsock2.h>

#include "time_ex.h"
#include "server_config.h"
#include "net.h"
#include "ls_message_handler.h"

CLSSessionMgr CLSSessionMgr::ms_Instance;

CLSSessionMgr::CLSSessionMgr()
{
    m_pNet = NULL;
    m_eServerState = sgsInvalid;
	m_qwLastPingTick = 0;
	m_qwLastStatisTick = 0;
    memset(m_GTSession, 0, sizeof(m_GTSession));
	memset(m_MGSession, 0, sizeof(m_MGSession));
    memset(&m_MSSession, 0, sizeof(m_MSSession));
    memset(&m_GDSession, 0, sizeof(m_GDSession));
}

CLSSessionMgr::~CLSSessionMgr()
{
    UnInit();
}

BOOL CLSSessionMgr::Init(INet* pNet)
{
    LOG_PROCESS_ERROR(pNet);

    memset(m_GTSession, 0, sizeof(m_GTSession));
    for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
    {
        m_GTSession[nIndex].dwType = stGT;
        m_GTSession[nIndex].dwSessionID = nIndex;
    }

	memset(m_MGSession, 0, sizeof(m_MGSession));
	for (INT32 nIndex = 1; nIndex < MAX_MG_SESSION_COUNT; nIndex++)
	{
		m_MGSession[nIndex].dwType = stMG;
		m_MGSession[nIndex].dwSessionID = nIndex;
	}

    memset(&m_MSSession, 0, sizeof(m_MSSession));
    m_MSSession.dwType = stMS;
    
    memset(&m_GDSession, 0, sizeof(m_GDSession));
    m_GDSession.dwType = stGD;

    m_pNet = pNet;
	m_qwLastPingTick = 0;
	m_qwLastStatisTick = 0;
    m_eServerState = sgsInitializing;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLSSessionMgr::UnInit(void)
{
    LOG_CHECK_ERROR(!CheckSession());

    return TRUE;
}

CLSSessionMgr& CLSSessionMgr::GetInstance(void)
{
    return ms_Instance;
}

GT_SESSION* CLSSessionMgr::GetGTSession(UINT32 dwSessionID)
{
    LOG_PROCESS_ERROR(dwSessionID < MAX_GT_SESSION_COUNT);

    return m_GTSession + dwSessionID;
Exit0:
    return NULL;
}

MG_SESSION* CLSSessionMgr::GetMGSession(UINT32 dwSessionID)
{
	LOG_PROCESS_ERROR(dwSessionID < MAX_MG_SESSION_COUNT);

	return m_MGSession + dwSessionID;
Exit0:
	return NULL;
}

MS_SESSION& CLSSessionMgr::GetMSSession(void)
{
    return m_MSSession;
}

GD_SESSION& CLSSessionMgr::GetGDSession(void)
{
    return m_GDSession;
}

INT32 CLSSessionMgr::GetServerState(void)
{
    return m_eServerState;
}

void CLSSessionMgr::SetServerState(INT32 eServerState)
{
    m_eServerState = eServerState;
}

BOOL CLSSessionMgr::MainLoop(void)
{
    UINT64 qwCurrentTick = GetTickCountEx();

    if (qwCurrentTick > m_qwLastPingTick + g_ServerConfig.Common.nServerPingInterval)
    {
        CLSMessageHandler::GetInstance().DoAllPing();
        m_qwLastPingTick = qwCurrentTick;
	}
	if (qwCurrentTick > m_qwLastStatisTick + 1000)//g_ServerConfig.Common.nServerStatisInterval)
	{
		//g_UpdateProcessStatisInfo();
		m_qwLastStatisTick = qwCurrentTick;
	}

    if (m_GDSession.bConnected && m_GDSession.pSocketSession)
    {
        if (m_GDSession.qwTimer < qwCurrentTick)
        {
            CRI("GD ping time out");
            m_GDSession.pSocketSession->Close();
            m_GDSession.qwTimer = 0xFFFFFFFFFFFFFFFF;
        }
    }
    else if(m_GDSession.pSocketSession == NULL && m_GDSession.qwTimer < qwCurrentTick)
    {
#if defined(WIN32)
        INF("try to connect to GD");
		m_GDSession.pSocketSession = m_pNet->Connect(inet_addr(g_szLocalAddress), g_ServerConfig.Common.dwGuardPort);
        if (m_GDSession.pSocketSession)
            m_GDSession.pSocketSession->SetUserData(&m_GDSession);
#endif
    }

    if (m_MSSession.bConnected && m_MSSession.pSocketSession)
    {
        if (m_MSSession.qwTimer < qwCurrentTick)
        {
            CRI("MS ping time out");
            m_MSSession.pSocketSession->Close();
            m_MSSession.qwTimer = 0xFFFFFFFFFFFFFFFF;
        }
    }
    else if (m_MSSession.pSocketSession == NULL && 
        m_MSSession.qwTimer < qwCurrentTick &&
        CLSSessionMgr::GetInstance().GetServerState() != sgsClosing)
    {
        INF("try to connect to MS");
		
		UINT32 dwAddr = inet_addr(g_ServerConfig.MS.szAddress);
		if(dwAddr == 0)
			dwAddr = inet_addr(g_szLocalAddress);

        m_MSSession.pSocketSession = m_pNet->Connect(dwAddr, g_ServerConfig.MS.dwLSPort);
        if (m_MSSession.pSocketSession)
            m_MSSession.pSocketSession->SetUserData(&m_MSSession);
    }

    for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
    {
        GT_SESSION* pGTSession = m_GTSession + nIndex;
        if (pGTSession->pSocketSession)
        {
            if (pGTSession->qwTimer < qwCurrentTick)
            {
                CRI("GT %d ping time out", pGTSession->dwSessionID);
                pGTSession->pSocketSession->Close();
                pGTSession->qwTimer = 0xFFFFFFFFFFFFFFFF;
            }
        }
    }

	for (INT32 nIndex = 1; nIndex < MAX_MG_SESSION_COUNT; nIndex++)
	{
		MG_SESSION* pMGSession = m_MGSession + nIndex;
		if (pMGSession->pSocketSession)
		{
			if (pMGSession->qwTimer < qwCurrentTick)
			{
				CRI("MG %d ping time out", pMGSession->dwSessionID);
				pMGSession->pSocketSession->Close();
				pMGSession->qwTimer = 0xFFFFFFFFFFFFFFFF;
			}
		}
	}

    return TRUE;
}

BOOL CLSSessionMgr::CheckSession()
{
    PROCESS_SUCCESS(m_MSSession.bConnected);

    for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
    {
        GT_SESSION* pGTSession = m_GTSession + nIndex;
        PROCESS_SUCCESS(pGTSession->pSocketSession);
    }

	for (INT32 nIndex = 1; nIndex < MAX_MG_SESSION_COUNT; nIndex++)
	{
		MG_SESSION* pMGSession = m_MGSession + nIndex;
		PROCESS_SUCCESS(pMGSession->pSocketSession);
	}

    return FALSE;
Exit1:
    return TRUE;
}

BOOL CLSSessionMgr::CloseAllSession()
{
    if (m_MSSession.bConnected && m_MSSession.pSocketSession)
        m_MSSession.pSocketSession->Close();

    for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
    {
        GT_SESSION* pGTSession = m_GTSession + nIndex;
        if (pGTSession->pSocketSession)
            pGTSession->pSocketSession->Close();
    }

	for (INT32 nIndex = 1; nIndex < MAX_MG_SESSION_COUNT; nIndex++)
	{
		MG_SESSION* pMGSession = m_MGSession + nIndex;
		if (pMGSession->pSocketSession)
			pMGSession->pSocketSession->Close();
	}
	
	return TRUE;
}