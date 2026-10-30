#include "stdafx.h"
#include "ms_mgr.h"
//#include "gm_svr_def.h"
//
//#include "server_config.h"
//
//CMSMgr  CMSMgr::m_Instance;
//
//
//CMSMgr &CMSMgr::GetInstance()
//{
//    return m_Instance;
//}
//
//CMSMgr::CMSMgr()
//{
//}
//
//CMSMgr::~CMSMgr()
//{
//
//}
//
//BOOL CMSMgr::Init()
//{
//    memset(&m_listMSData, 0, sizeof(m_listMSData));
//
//    for (INT32 i = 1; i < MAX_MS_SESSION_COUNT; ++i)
//    {
//        m_listMSData[i].dwMSSessionID = i;
//        m_listMSData[i].nState = MS_STATE_OFFLINE;
//        //strxcpy(m_listMSData[i].szName, g_SvrList.MSList.arrSvrList[i].szName, sizeof(m_listMSData[i].szName));
//    }
//
//    return TRUE;
//}
//
//BOOL CMSMgr::UnInit()
//{
//    memset(&m_listMSData, 0, sizeof(m_listMSData));
//
//    return TRUE;
//}
//
//
//BOOL CMSMgr::RegisterMS(UINT32 dwSessionID, const char *pcszName)
//{
//    INT32   nIndex = 0;
//    LOG_PROCESS_ERROR(pcszName);
//    LOG_PROCESS_ERROR(dwSessionID < MAX_MS_SESSION_COUNT);
//
//    //LOG_PROCESS_ERROR( 0 == strncmp(m_listMSData[dwSessionID].szName, pcszName, sizeof(m_listMSData[dwSessionID].szName)-1) );
//
//    strxcpy(m_listMSData[dwSessionID].szName, pcszName, sizeof(m_listMSData[dwSessionID].szName));
//    
//
//    if (0 == strncmp(g_ServerConfig.Common.szAreaName, pcszName, sizeof(g_ServerConfig.Common.szAreaName)))
//    {
//        m_listMSData[dwSessionID].dwSvrID = g_ServerConfig.Common.nAreaID;
//    }
//
//    //for ( nIndex = 1; nIndex < g_GMConfig.MSList.nSvrNum; ++nIndex)
//    //{
//    //    if (0 == strncmp(g_GMConfig.MSList.arrSvrList[nIndex].szName, pcszName, sizeof(g_GMConfig.MSList.arrSvrList[nIndex].szName)))
//    //    {
//    //        m_listMSData[dwSessionID].dwSvrID = g_GMConfig.MSList.arrSvrList[nIndex].nSvrID;
//    //    }
//    //}
//
//    //if (nIndex == g_GMConfig.MSList.nSvrNum)
//    //{
//    //    // todo_tq: assign a separate IP to temporarily online MS instances. Allan Tao [20.12.2013]
//    //}
//
//
//    m_listMSData[dwSessionID].nState = MS_STATE_ONLINE;
//
//    return TRUE;
//Exit0:
//    return FALSE;
//}
//
//BOOL CMSMgr::UnRegisterMS(UINT32 dwSessionID)
//{
//    LOG_PROCESS_ERROR(dwSessionID < MAX_MS_SESSION_COUNT);
//
//    // todo_tq: init and uninit. Allan Tao [20.12.2013]
//    m_listMSData[dwSessionID].nState = MS_STATE_OFFLINE;
//    m_listMSData[dwSessionID].dwSvrID = 0;
//
//    return TRUE;
//Exit0:
//    return FALSE;
//}
//
//MS_DATA* CMSMgr::FindMS(const char *pcszName)
//{
//    LOG_PROCESS_ERROR(pcszName);
//
//    for (INT32 i = 1; i < MAX_MS_SESSION_COUNT; ++i)
//    {
//        if (0 == strncmp(m_listMSData[i].szName, pcszName, sizeof(m_listMSData[i].szName)))
//        {
//            LOG_PROCESS_ERROR(m_listMSData[i].nState == MS_STATE_ONLINE);
//            return &m_listMSData[i];
//        }
//    }
//
//Exit0:
//    return NULL;
//}
//
//MS_DATA* CMSMgr::FindMS(UINT32 dwSessionID)
//{
//    LOG_PROCESS_ERROR(dwSessionID < MAX_MS_SESSION_COUNT);
//    LOG_PROCESS_ERROR(m_listMSData[dwSessionID].nState == MS_STATE_ONLINE);
//
//    return &m_listMSData[dwSessionID];
//Exit0:
//    return NULL;
//}
//
//MS_DATA* CMSMgr::FindMSByID(UINT32 dwSvrID)
//{
//    for (INT32 i = 1; i < MAX_MS_SESSION_COUNT; ++i)
//    {
//        if (m_listMSData[i].dwSvrID == dwSvrID)
//        {
//            LOG_PROCESS_ERROR(m_listMSData[i].nState == MS_STATE_ONLINE);
//            return &m_listMSData[i];
//        }
//    }
//
//Exit0:
//    return NULL;
//}
//
//
//UINT32  CMSMgr::FirstValidMS()
//{
//    m_dwCursorSessionID = 0;
//    return NextValidMS();
//}
//
//UINT32  CMSMgr::NextValidMS()
//{
//    for (++m_dwCursorSessionID; m_dwCursorSessionID < MAX_MS_SESSION_COUNT; ++m_dwCursorSessionID)
//    {
//        if (m_listMSData[m_dwCursorSessionID].nState == MS_STATE_ONLINE)
//        {
//            return m_dwCursorSessionID;
//        }
//    }
//    return MAX_MS_SESSION_COUNT;
//}
//
