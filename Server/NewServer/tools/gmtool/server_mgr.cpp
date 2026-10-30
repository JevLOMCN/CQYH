#include "stdafx.h"
#include "server_mgr.h"

#include "predefine.h"
#include "session_mgr.h"
#include "server_list.h"
#include "gmtool.h"



CServerMgr  CServerMgr::m_Instance;

CServerMgr& CServerMgr::GetInstance()
{
    return m_Instance;
}

BOOL CServerMgr::Init()
{
    ResetSelectedServer();

    return TRUE;
}

BOOL CServerMgr::UnInit()
{
    ResetSelectedServer();

    return TRUE;
}

void CServerMgr::ResetSelectedServer()
{
    m_nSelectedSessionNum = 0;
    memset(m_SessionIDList, 0, sizeof(m_SessionIDList));
}

BOOL CServerMgr::UpdateSelectServer()
{
    INT32       nRetCode = 0;
    GM_SESSION  *pGMSession = NULL;

    ResetSelectedServer();
    if (!gm::current_main_window()) return TRUE;

    for (INT32 nIndex = 0; nIndex < MAX_GM_SESSION_COUNT; ++nIndex)
    {
        nRetCode = gm::main_window().IsServerSelected(nIndex);
        if (nRetCode == FALSE)
            continue;

        pGMSession = CSessionMgr::GetInstance().GetGMSession(nIndex);
        if (pGMSession && pGMSession->eState == GM_SVR_STATE_LOGIN)
            m_SessionIDList[m_nSelectedSessionNum++] = pGMSession->dwSessionID;
    }

    return TRUE;
}

UINT32 CServerMgr::GetSessionIDByIndex(INT32 nIndex)
{
    LOG_PROCESS_ERROR(nIndex < m_nSelectedSessionNum);

    return m_SessionIDList[nIndex];
Exit0:
    return 0xFFFFFFFF;
}


