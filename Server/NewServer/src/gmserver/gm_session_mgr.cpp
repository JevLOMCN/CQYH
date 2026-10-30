#include "stdafx.h"
#include "statis_info.h"
#include "gm_session_mgr.h"
#include "net.h"
#include "time_ex.h"
#include "server_config.h"
#include "message_handler.h"
#include "ms_mgr.h"
#include "user_manager.h"
#include "gm_log.h"

CGMSessionMgr CGMSessionMgr::ms_Instance;

CGMSessionMgr::CGMSessionMgr()
{
    m_pNet = NULL;
    m_eSvrState = sgsInvalid;
    m_qwLastPingTick = 0;
	m_qwLastStatisTick = 0;
    m_TLSessionIndex = 0;

    memset(&m_TLSession, 0, sizeof(m_TLSession));
    memset(&m_MSSession, 0, sizeof(m_MSSession));
    memset(&m_GDSession, 0, sizeof(m_GDSession));
}

CGMSessionMgr::~CGMSessionMgr()
{

}

BOOL CGMSessionMgr::Init(INet *pNet)
{
    LOG_PROCESS_ERROR(pNet);

    m_eSvrState = sgsInvalid;
	m_qwLastPingTick = 0;
	m_qwLastStatisTick = 0;
    m_TLSessionIndex = 0;

    memset(&m_TLSession, 0, sizeof(m_TLSession));
    memset(&m_MSSession, 0, sizeof(m_MSSession));
    memset(&m_GDSession, 0, sizeof(m_GDSession));
    m_GDSession.dwType = stGD;

    //for (INT32 nIndex = 1; nIndex < MAX_MS_SESSION_COUNT; ++nIndex)
    {
        m_MSSession.dwType = stMS;
        m_MSSession.dwSessionID = 1;
    }

    for (INT32 nIndex = 1; nIndex < MAX_GM_TL_SESSION_COUNT; ++nIndex)
    {
        m_TLSession[nIndex].dwType = stTL;
        m_TLSession[nIndex].dwSessionID = nIndex;
    }

    m_pNet = pNet;
    m_eSvrState = sgsInitializing;
    m_TLSessionIndex = 1;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMSessionMgr::UnInit()
{
    LOG_CHECK_ERROR(!CheckSession());
    return TRUE;
}

CGMSessionMgr& CGMSessionMgr::GetInstance()
{
    return ms_Instance;
}

STLSession *CGMSessionMgr::NewTLSession()
{
    for (INT32 i = m_TLSessionIndex; i < MAX_GM_TL_SESSION_COUNT + m_TLSessionIndex; ++i)
    {
        INT32 nIndex = i % MAX_GM_TL_SESSION_COUNT;

        if(nIndex == 0)
            ++nIndex;

        if (m_TLSession[nIndex].pSocketSession == NULL)
        {
            m_TLSessionIndex = nIndex + 1;

            return m_TLSession + nIndex;
        }
    }

    return NULL;
}

//SMSSession* CGMSessionMgr::GetMSSession()
//{
//    for (INT32 i = m_MSSessionIndex; i < MAX_MS_SESSION_COUNT + m_MSSessionIndex; ++i)
//    {
//        INT32 nIndex = i % MAX_MS_SESSION_COUNT;
//
//        if (nIndex == 0)
//            ++nIndex;
//
//        if (m_MSSession[nIndex].pSocketSession == NULL)
//        {
//            m_MSSessionIndex = nIndex + 1;
//            return m_MSSession + nIndex;
//        }
//    }
//    return NULL;
//}

BOOL CGMSessionMgr::MainLoop()
{
    INT32       nRetCode = 0;
    UINT64      qwCurrentTick = GetTickCountEx();

    if (qwCurrentTick > m_qwLastPingTick + g_ServerConfig.Common.nServerPingInterval
        && m_eSvrState != sgsClosing)
    {
        CMessageHandler::GetInstance().DoAllPing();
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
            m_GDSession.qwTimer = 0xFFFFFFFF;
        }
    }
    else if (m_GDSession.pSocketSession == NULL && m_GDSession.qwTimer < qwCurrentTick)
    {
#if defined(WIN32)
        INF("try to connect to GD");
        m_GDSession.pSocketSession = m_pNet->Connect(inet_addr(g_szLocalAddress), g_ServerConfig.Common.dwGuardPort);
        if (m_GDSession.pSocketSession)
            m_GDSession.pSocketSession->SetUserData(&m_GDSession);
#endif
    }

    //for (INT32 nIndex = 1; nIndex < MAX_MS_SESSION_COUNT; ++nIndex)
    {
        //SMSSession  &rMSSession = m_MSSession[nIndex];

        if (m_MSSession.pSocketSession)
        {
            if (m_MSSession.qwTimer < qwCurrentTick)
            {
                CRI("MS %d ping time out", m_MSSession.dwSessionID);

                //nRetCode = CMSMgr::GetInstance().UnRegisterMS(rMSSession.dwSessionID);
                //LOG_CHECK_ERROR(nRetCode);
                m_MSSession.eState = egmssDisconnected;
                
                m_MSSession.pSocketSession->Close();
                m_MSSession.qwTimer = 0xFFFFFFFF;
            }
        }
    }

    for (INT32 nIndex = 1; nIndex < MAX_GM_TL_SESSION_COUNT; ++nIndex)
    {
        STLSession  &rTLSession = m_TLSession[nIndex];

        if (rTLSession.pSocketSession)
        {
            if (rTLSession.qwTimer < qwCurrentTick)
            {
                CTLUser *pTLUser = NULL;
                CRI("TL %d ping time out", rTLSession.dwSessionID);

                pTLUser = CUserManager::GetInstance().FindUserByID(rTLSession.dwTLUserID);
                if (pTLUser)
                {
                    nRetCode = CUserManager::GetInstance().LogoutUser(rTLSession.dwTLUserID);

                    CRI("Ping time out %d\t%d\t%s", pTLUser->GetIP(), pTLUser->GetID(), pTLUser->GetUserName());
                }
                rTLSession.pSocketSession->Close();
                rTLSession.qwTimer = 0xFFFFFFFF;
            }
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMSessionMgr::CheckSession()
{
    for (INT32 nIndex = 1; nIndex < MAX_MS_SESSION_COUNT; ++nIndex)
    {
        //SMSSession  *pMSSession = m_MSSession + nIndex;
        PROCESS_SUCCESS(m_MSSession.pSocketSession);
    }

    for (INT32 nIndex = 1; nIndex < MAX_GM_TL_SESSION_COUNT; ++nIndex)
    {
        STLSession  *pTLSession = m_TLSession + nIndex;
        PROCESS_SUCCESS(pTLSession->pSocketSession);
    }

    return FALSE;
Exit1:
    return TRUE;    
}

BOOL CGMSessionMgr::CloseAllSession()
{
    for (INT32 nIndex = 1; nIndex < MAX_MS_SESSION_COUNT; ++nIndex)
    {
        //SMSSession  *pMSSession = m_MSSession + nIndex;
        if (m_MSSession.pSocketSession)
            m_MSSession.pSocketSession->Close();
    }

    for (INT32 nIndex = 1; nIndex < MAX_GM_TL_SESSION_COUNT; ++nIndex)
    {
        STLSession  *pTLSession = m_TLSession + nIndex;
        if (pTLSession->pSocketSession)
            pTLSession->pSocketSession->Close();
    }

    return TRUE;
}
