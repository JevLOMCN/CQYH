#include "stdafx.h"
#include "statis_info.h"
#include "ms_session_mgr.h"

#include "time_ex.h"

#include "server_config.h"

#include "net.h"

#include "message_handler.h"
#include "client/user.h"
#include "role_data.h"
#include "onlinerole/online_role_data_mgr.h"
#include "scene/scenemgr.h"
#include "gs_info_mgr.h"

CMSSessionMgr CMSSessionMgr::ms_Instance;

CMSSessionMgr::CMSSessionMgr()
{
	m_pNet = NULL;
	m_eServerState = sgsInvalid;
	m_qwLastPingTick = 0;
	memset(&m_GTSession, 0, sizeof(m_GTSession));
	memset(&m_GSSession, 0, sizeof(m_GSSession));
	memset(&m_DBSession, 0, sizeof(m_DBSession));
	memset(&m_LSSession, 0, sizeof(m_LSSession));
	memset(&m_DLSession, 0, sizeof(m_DLSession));
	memset(&m_GDSession, 0, sizeof(m_GDSession));
	memset(&m_BSSession, 0, sizeof(m_BSSession));
	memset(&m_GMSession, 0, sizeof(m_GMSession));
	memset(&m_NSSession, 0, sizeof(m_NSSession));
	memset(&m_MMSession, 0, sizeof(m_MMSession));
}

CMSSessionMgr::~CMSSessionMgr()
{
	UnInit();
}

BOOL CMSSessionMgr::Init(INet* pNet)
{
	LOG_PROCESS_ERROR(pNet);

	memset(m_GTSession, 0, sizeof(m_GTSession));
	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		m_GTSession[nIndex].dwType = stGT;
		m_GTSession[nIndex].dwSessionID = nIndex;
	}

	memset(m_GSSession, 0, sizeof(m_GSSession));
	for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
	{
		m_GSSession[nIndex].dwType = stGS;
		m_GSSession[nIndex].dwSessionID = nIndex;
        m_GSSession[nIndex].bIsExiting = FALSE;
        m_GSSession[nIndex].nOnlineRoleCount = 0;
	}

	memset(&m_DBSession, 0, sizeof(m_DBSession));
	m_DBSession.dwType = stDB;

	memset(&m_LSSession, 0, sizeof(m_LSSession));
	m_LSSession.dwType = stLS;

	memset(&m_GDSession, 0, sizeof(m_GDSession));
	m_GDSession.dwType = stGD;

	memset(&m_BSSession, 0, sizeof(m_BSSession));
	m_BSSession.dwType = stBS;

	memset(&m_DLSession, 0, sizeof(m_DLSession));
	m_DLSession.dwType = stDL;

	memset(&m_GMSession, 0, sizeof(m_GMSession));
	m_GMSession.dwType = stGM;

	memset(&m_NSSession, 0, sizeof(m_NSSession));
	m_NSSession.dwType = stNS;

	memset(&m_MMSession, 0, sizeof(m_MMSession));
	m_MMSession.dwType = stMM;

	m_pNet = pNet;
	m_eServerState = sgsInitializing;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMSSessionMgr::UnInit(void)
{
	LOG_CHECK_ERROR(!CheckSession());
	return TRUE;
}

CMSSessionMgr& CMSSessionMgr::GetInstance(void)
{
	return ms_Instance;
}

BOOL CMSSessionMgr::MainLoop(void)
{
	INT32 nRetCode = 0;
	UINT64 qwCurrentTick = GetTickCountEx();

	if (qwCurrentTick  > m_qwLastPingTick + g_ServerConfig.Common.nServerPingInterval)
	{
		CMessageHandler::GetInstance().DoAllPing();
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
	else if(m_GDSession.pSocketSession == NULL && m_GDSession.qwTimer < qwCurrentTick)
	{
#if defined(WIN32)
		INF("try to connect to GD");
		m_GDSession.pSocketSession = m_pNet->Connect(inet_addr(g_szLocalAddress), g_ServerConfig.Common.dwGuardPort);
		if (m_GDSession.pSocketSession)
			m_GDSession.pSocketSession->SetUserData(&m_GDSession);
#endif
	}

    if (g_ServerConfig.MS.bNeedConnectGMServer)
    {
        if (m_GMSession.bConnected && m_GMSession.pSocketSession)
        {
            if (m_GMSession.qwTimer < qwCurrentTick)
            {
                CRI("GM ping time out");
                m_GMSession.pSocketSession->Close();
                m_GMSession.qwTimer = 0xFFFFFFFF;
            }
        }
        else if (m_GMSession.pSocketSession == NULL && m_GMSession.qwTimer < qwCurrentTick)
        {
#if defined(WIN32)
            //INF("try to connect to GM");
            m_GMSession.pSocketSession = m_pNet->Connect(inet_addr(g_ServerConfig.MS.szRemoteGMAddress), g_ServerConfig.MS.dwRemoteGMPort);
            if (m_GMSession.pSocketSession)
                m_GMSession.pSocketSession->SetUserData(&m_GMSession);
#endif
        }
    }

	if (m_DBSession.pSocketSession)
	{
		if (m_DBSession.qwTimer < qwCurrentTick)
		{
			CRI("DB ping time out");
			m_DBSession.pSocketSession->Close();
			m_DBSession.qwTimer = (UINT64)(~0);
		}
	}

	if (m_LSSession.pSocketSession)
	{
		if (m_LSSession.qwTimer < qwCurrentTick)
		{
			CRI("LS ping time out");
			m_LSSession.pSocketSession->Close();
			m_LSSession.qwTimer = (UINT64)(~0);
		}
	}

	if (m_BSSession.pSocketSession)
	{
		if (m_BSSession.qwTimer < qwCurrentTick)
		{
			CRI("BS ping time out");
			m_BSSession.pSocketSession->Close();
			m_BSSession.qwTimer = (UINT64)(~0);
		}
	}

	if (m_DLSession.pSocketSession)
	{
		if (m_DLSession.qwTimer < qwCurrentTick)
		{
			CRI("DL ping time out");
			m_DLSession.pSocketSession->Close();
			m_DLSession.qwTimer = (UINT64)(~0);
		}
	}

	if (m_MMSession.pSocketSession)
	{
		if (m_MMSession.qwTimer < qwCurrentTick)
		{
			CRI("MM ping time out");
			m_MMSession.pSocketSession->Close();
			m_MMSession.qwTimer = (UINT64)(~0);
		}
	}

	if (m_NSSession.pSocketSession && m_NSSession.bConnected)
	{
		if (m_NSSession.qwTimer < qwCurrentTick)
		{
			CRI("NS ping time out");
			m_NSSession.pSocketSession->Close();
			m_NSSession.qwTimer = (UINT64)(~0);
		}
	}
	else if (!m_NSSession.bConnected && m_NSSession.qwTimer < qwCurrentTick && CMSSessionMgr::GetInstance().GetServerState() != sgsClosing)
	{
		INF("try to connect to NS");

		UINT32 dwAddr = inet_addr(g_ServerConfig.NS.szAddress);
		if (dwAddr == 0)
			dwAddr = inet_addr(g_szLocalAddress);

		m_NSSession.pSocketSession = m_pNet->Connect(dwAddr, g_ServerConfig.NS.dwPort);
		if (m_NSSession.pSocketSession)
			m_NSSession.pSocketSession->SetUserData(&m_NSSession);
		m_NSSession.bConnected = TRUE;
	}

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		SGTSession& rGTSession = m_GTSession[nIndex];

		if (rGTSession.pSocketSession)
		{
			if (rGTSession.qwTimer < qwCurrentTick)
			{
				CRI("GT %d ping time out", rGTSession.dwSessionID);
				rGTSession.pSocketSession->Close();
				rGTSession.qwTimer = (UINT64)(~0);
			}
		}
	}

	for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
	{
		SGSSession& rGSSession = m_GSSession[nIndex];

		if (rGSSession.pSocketSession)
		{
			if (rGSSession.qwTimer < qwCurrentTick)
			{
				CRI("GS %d ping time out", rGSSession.dwSessionID);
				rGSSession.pSocketSession->Close();
				rGSSession.qwTimer = (UINT64)(~0);
			}
		}
	}

	for (std::list<MS_USER*>::iterator it = m_DBWaitList.begin();
		it != m_DBWaitList.end(); )
	{
        BOOL bErased = FALSE;
		MS_USER* poUser = *it;
		if (poUser)
        {
            if ((poUser->eState == USER_STATE_WAIT_EXIT_DB) && 
                 poUser->qwTimer < qwCurrentTick )
            {
                nRetCode = CMessageHandler::GetInstance().DoDBLeaveDB(poUser->dwCliSessionID);
                LOG_PROCESS_ERROR(nRetCode);
                
                MS_ONLINE_ROLE_DATA* pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
                if (pOnlineRole)
                {
                    INF("user %s <%08X> leave scene, since WAIT_EXIT_DB time up", poUser->szUserName, poUser->dwCliSessionID);

					if (pOnlineRole->dwGSSessionID > 0)
					{
						nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole,
							pOnlineRole->pRoleData->nSceneTempID, pOnlineRole->pRoleData->nSceneID, FALSE);
						LOG_CHECK_ERROR(nRetCode);

						SGSSession* pGSSession = CMSSessionMgr::GetInstance().GetGSSession(pOnlineRole->dwGSSessionID);
						//LOG_CHECK_ERROR( pGSSession );
						if (pGSSession)
						{
							--pGSSession->nOnlineRoleCount;
							INF("role %d leaved GS %d, remains online role count %d", pOnlineRole->dwCliSessionID, pGSSession->dwSessionID, pGSSession->nOnlineRoleCount);
						}
						pOnlineRole->dwGSSessionID = 0;
					}

					if (pOnlineRole->dwTargetGSSessionID > 0)
					{
						nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole,
							pOnlineRole->nPreEnterSceneTempID, pOnlineRole->nPreEnterSceneID, FALSE);
						LOG_CHECK_ERROR(nRetCode);

						SGSSession* pTargetGSSession = CMSSessionMgr::GetInstance().GetGSSession(pOnlineRole->dwTargetGSSessionID);
						//LOG_CHECK_ERROR(pTargetGSSession);
						if (pTargetGSSession)
						{
							--pTargetGSSession->nOnlineRoleCount;
							INF("role %d leaved target GS %d, remains online role count %d", pOnlineRole->dwCliSessionID, pTargetGSSession->dwSessionID, pTargetGSSession->nOnlineRoleCount);
						}
						pOnlineRole->dwTargetGSSessionID = 0;
					}

                    nRetCode = COnlineRoleDataMgr::GetInstance().DelOnlineRole(pOnlineRole);
                    LOG_CHECK_ERROR( nRetCode );
                }

                m_DBWaitList.erase(it++);
                bErased = TRUE;
                poUser->eState = USER_STATE_EXIT_DB;
            }
        }

        if(!bErased)
        {
            ++it;
        }
	}

    for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
    {
        SGSSession& rGSSession = m_GSSession[nIndex];

        if ( rGSSession.bIsExiting == TRUE && rGSSession.nOnlineRoleCount == 0 )
        {
            CRI("GS %d has no role now, clean scene and delete GSInfo", rGSSession.dwSessionID);

            nRetCode = CSceneMgr::GetInstance().CleanSceneByGSsessionID(rGSSession.dwSessionID);
            LOG_CHECK_ERROR(nRetCode);
            INF("GS %d CSceneMgr::CleanSceneByGSsessionID", rGSSession.dwSessionID );

            nRetCode = CGSInfoMgr::GetInstance().DelGSInfo(rGSSession.dwSessionID);
            LOG_PROCESS_ERROR(nRetCode);
            INF("GS %d delete from CGSInfoMgr", rGSSession.dwSessionID );

            rGSSession.bIsExiting = FALSE;
            rGSSession.qwTimer = (UINT64)(~0);
        }
    }

	return TRUE;
Exit0:
	return FALSE;
}

UINT32 CMSSessionMgr::GetSuitableGS(UINT32 dwPreferGSSessionID)
{
	UINT32 dwGSSessionID = 0xFFFFFFFF;
	//INT32 nMinWorkload = INT_MAX;
	//INT32 nMaxWorkload = 0;
	INT32 nMinOnlineRole = INT_MAX;
	INT32 nMaxOnlineRole = 0;

	LOG_PROCESS_ERROR(dwPreferGSSessionID >= 0 && dwPreferGSSessionID < MAX_GS_SESSION_COUNT);

	//for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
	//{
	//	if (m_GSSession[nIndex].pSocketSession && m_GSSession[nIndex].nWorkload < nMinWorkload)
	//	{
	//		nMinWorkload = m_GSSession[nIndex].nWorkload;
	//		dwGSSessionID = m_GSSession[nIndex].dwSessionID;
	//	}
	//	if (m_GSSession[nIndex].pSocketSession && m_GSSession[nIndex].nWorkload > nMaxWorkload)
	//	{
	//		nMaxWorkload = m_GSSession[nIndex].nWorkload;
	//	}
	//}
	//if (m_GSSession[dwPreferGSSessionID].pSocketSession &&
	//	m_GSSession[dwPreferGSSessionID].nWorkload < nMaxWorkload * 4 / 5)
	//{
	//	dwGSSessionID = dwPreferGSSessionID;
	//}

	for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
	{
		if (m_GSSession[nIndex].pSocketSession && m_GSSession[nIndex].nOnlineRoleCount < nMinOnlineRole)
		{
			nMinOnlineRole = m_GSSession[nIndex].nOnlineRoleCount;
			dwGSSessionID = m_GSSession[nIndex].dwSessionID;
		}
		if (m_GSSession[nIndex].pSocketSession && m_GSSession[nIndex].nOnlineRoleCount > nMaxOnlineRole)
		{
			nMaxOnlineRole = m_GSSession[nIndex].nOnlineRoleCount;
		}
	}

	if (m_GSSession[dwPreferGSSessionID].pSocketSession &&
		m_GSSession[dwPreferGSSessionID].nOnlineRoleCount < nMaxOnlineRole * 4 / 5)
	{
		dwGSSessionID = dwPreferGSSessionID;
	}

	DBG("GetSuitableGS prefer %d, result %d, Min OnlineRoleCount %d, Max OnlineRoleCount %d", 
		dwPreferGSSessionID, dwGSSessionID, nMinOnlineRole, nMaxOnlineRole);

	return dwGSSessionID;
Exit0:
	return 0;
}

BOOL CMSSessionMgr::CheckSession()
{
	PROCESS_SUCCESS(m_DBSession.pSocketSession);
	PROCESS_SUCCESS(m_LSSession.pSocketSession);
	PROCESS_SUCCESS(m_GMSession.pSocketSession);
    PROCESS_SUCCESS(m_GDSession.pSocketSession);
	PROCESS_SUCCESS(m_NSSession.pSocketSession);
	PROCESS_SUCCESS(m_MMSession.pSocketSession);

	if(g_ServerConfig.MS.nInitDLCount > 0)
		PROCESS_SUCCESS(m_DLSession.pSocketSession);

	if(g_ServerConfig.MS.nInitBSCount > 0)
		PROCESS_SUCCESS(m_BSSession.pSocketSession);

	for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
	{
		SGSSession* pGSSession = m_GSSession + nIndex;
		PROCESS_SUCCESS(pGSSession->pSocketSession);
	}

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		SGTSession* pGTSession = m_GTSession + nIndex;
		PROCESS_SUCCESS(pGTSession->pSocketSession);
	}

	return FALSE;
Exit1:
	return TRUE;
}

BOOL CMSSessionMgr::CloseAllSession()
{
	if (m_LSSession.pSocketSession)
		m_LSSession.pSocketSession->Close();

	if (m_DBSession.pSocketSession)
		m_DBSession.pSocketSession->Close();

	if (m_GDSession.bConnected && m_GDSession.pSocketSession)
		m_GDSession.pSocketSession->Close();

	if (m_GMSession.bConnected && m_GMSession.pSocketSession)
		m_GMSession.pSocketSession->Close();

	if (m_BSSession.pSocketSession)
		m_BSSession.pSocketSession->Close();

	if(m_DLSession.pSocketSession)
		m_DLSession.pSocketSession->Close();

	if (m_NSSession.pSocketSession)
		m_NSSession.pSocketSession->Close();

	if (m_MMSession.pSocketSession)
		m_MMSession.pSocketSession->Close();

	for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
	{
		SGSSession* pGSSession = m_GSSession + nIndex;
		if (pGSSession->pSocketSession)
			pGSSession->pSocketSession->Close();
	}

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		SGTSession* pGTSession = m_GTSession + nIndex;
		if (pGTSession->pSocketSession)
			pGTSession->pSocketSession->Close();
	}

	return TRUE;
}
