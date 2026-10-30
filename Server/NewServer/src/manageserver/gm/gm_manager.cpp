#include "stdafx.h"
#include "gm_manager.h"
#include "script_mgr.h"
#include "chat/ms_chat.h"
#include "server_def.h"
#include <time.h>
#include "chat_def.h"

#include "server_config.h"

CGMManager  CGMManager::m_Instance;

CGMManager & CGMManager::GetInstance()
{
    return m_Instance;
}

CGMManager::CGMManager()
{
}

CGMManager::~CGMManager()
{
}

BOOL CGMManager::Init()
{
    dwGMSerialID = 1;
    memset(m_listLockData, 0, sizeof(m_listLockData));
    m_AnnounceMap.clear();

    return TRUE;
//Exit0:
//    return FALSE;
}

BOOL CGMManager::UnInit()
{
    dwGMSerialID = 1;
    memset(m_listLockData, 0, sizeof(m_listLockData));
    m_AnnounceMap.clear();

    return TRUE;
//Exit0:
//    return FALSE;
}

BOOL CGMManager::AddLockUser(UINT32 dwTLUserID, const char *pcszName)
{
    LOG_PROCESS_ERROR(pcszName);

    for (INT32 i = 0; i < MAX_LOCK_ROLE; ++i)
    {
        if (FALSE == m_listLockData[i].bIsBlank)
        {
            m_listLockData[i].dwTLUserID = dwTLUserID;
            strxcpy(m_listLockData[i].szUserName, pcszName, sizeof(m_listLockData[i].szUserName));
            m_listLockData[i].bIsBlank = TRUE;
            return TRUE;
        }
    }

Exit0:
    return FALSE;
}

BOOL CGMManager::DelLockUser(const char *pcszName)
{
    LOG_PROCESS_ERROR(pcszName);

    for (INT32 i = 0; i < MAX_LOCK_ROLE; ++i)
    {
        if (TRUE == m_listLockData[i].bIsBlank)
        {
            if ( 0 == strncmp(m_listLockData[i].szUserName, pcszName, sizeof(m_listLockData[i].szUserName)) )
            {
                m_listLockData[i].bIsBlank = FALSE;
                return TRUE;
            }
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMManager::IsUserLocked(const char *pcszName)
{
    LOG_PROCESS_ERROR(pcszName);

    for (INT32 i = 0; i < MAX_LOCK_ROLE; ++i)
    {
        if (TRUE == m_listLockData[i].bIsBlank)
        {
            if ( 0 == strncmp(m_listLockData[i].szUserName, pcszName, sizeof(m_listLockData[i].szUserName)) )
            {
                return TRUE;
            }
        }
    }

    return FALSE;
Exit0:
    return TRUE;
}

UINT32 CGMManager::GetTLUserID(const char *pcszName)
{
    LOG_PROCESS_ERROR(pcszName);

    for (INT32 i = 0; i < MAX_LOCK_ROLE; ++i)
    {
        if (TRUE == m_listLockData[i].bIsBlank)
        {
            if ( 0 == strncmp(m_listLockData[i].szUserName, pcszName, sizeof(m_listLockData[i].szUserName)) )
            {
                return m_listLockData[i].dwTLUserID;
            }
        }
    }

Exit0:
    return 0;
}

void CGMManager::BeginAnnounce()
{
    m_AnnounceMapCursor = m_AnnounceMap.begin();
}

GM_ANNOUNCE_DATA* CGMManager::NextAnnounce()
{
    ANNOUNCE_MAP::iterator iter = m_AnnounceMapCursor;
    PROCESS_ERROR(iter != m_AnnounceMap.end());

    ++m_AnnounceMapCursor;
    return &(iter->second);
Exit0:
    return NULL;
}

BOOL CGMManager::AddAnnounce(INT32 nGroupID, UINT32 dwBeginTime, INT32 nRepeatNum, INT32 nIntervalTime, const char *pcszText)
{
    INT32       nRetCode = 0;
    GM_ANNOUNCE_DATA    data;
    GM_ANNOUNCE_TRIGGER     *pTrigger = NULL;

    LOG_PROCESS_ERROR(pcszText);

    pTrigger = new GM_ANNOUNCE_TRIGGER;
    LOG_PROCESS_ERROR(pTrigger);

    pTrigger->nGroupID = nGroupID;
    pTrigger->nIntervalTime = nIntervalTime;
    pTrigger->nRepeatNum = nRepeatNum;
    pTrigger->dwWashoutTime = dwBeginTime;
	pTrigger->nTriggerType = tttInvalid;
    pTrigger->fp = CB_BroadcastGMAnnounce;
    strxcpy(pTrigger->szText, pcszText, sizeof(pTrigger->szText));

    data.dwSerial = GetGMSerialID();
    data.pTrigger = pTrigger;
    pTrigger->dwSerial = data.dwSerial;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);

    m_AnnounceMap.insert(std::make_pair(data.dwSerial, data));

    return TRUE;
Exit0:
    if (pTrigger)
        delete pTrigger;
    return FALSE;
}

BOOL CGMManager::DelAnnounceTrigger(INT32 nGroupID, UINT32 dwSerialID)
{
    INT32       nRetCode = 0;
    ANNOUNCE_MAP::iterator iter = m_AnnounceMap.find(dwSerialID);
    PROCESS_ERROR(iter != m_AnnounceMap.end());

    LOG_CHECK_ERROR(iter->second.pTrigger);
    LOG_CHECK_ERROR(iter->second.pTrigger->nGroupID == nGroupID);
    if (iter->second.pTrigger)
    {
        nRetCode = CTimeTrigger::GetInstance().DeleteTrigger(iter->second.pTrigger);
        LOG_CHECK_ERROR(nRetCode);
    }

    m_AnnounceMap.erase(iter);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMManager::DelAnnounceRecord(INT32 nGroupID, UINT32 dwSerialID)
{
    m_AnnounceMap.erase(dwSerialID);
    return TRUE;
}

INT32 CB_BroadcastGMAnnounce(BASE_TRIGGER_NODE *pTriggerNode)
{
    INT32       nRetCode = 0;
    GM_ANNOUNCE_TRIGGER     *pTrigger = NULL;
    pTrigger = (GM_ANNOUNCE_TRIGGER *)pTriggerNode;

    LOG_PROCESS_ERROR(pTriggerNode);

    for (INT32 nGroupID = 0; nGroupID < MAX_SERVER_GROUP_COUNT; ++nGroupID)
    {
        if ((pTrigger->nGroupID == 0) || (pTrigger->nGroupID == nGroupID))
        {
            nRetCode = CMSChatMgr::GetInstance().Say(0, cctGroup, cltLighting, pTrigger->szText, nGroupID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    if (pTrigger->nRepeatNum > 1)
    {
        pTrigger->dwWashoutTime = time(NULL) + pTrigger->nIntervalTime;
        --pTrigger->nRepeatNum;
    }
    else
    {
        nRetCode = CGMManager::GetInstance().DelAnnounceRecord(pTrigger->nGroupID, pTrigger->dwSerial);
        LOG_PROCESS_ERROR(nRetCode);
        PROCESS_ERROR(FALSE);
    }

    return TIME_TRIGGER_RET_CONTINUE;
Exit0:
    if (pTrigger)
        delete pTrigger;
    return TIME_TRIGGER_RET_END;

}

BOOL g_GM_CheckRoleName(CHAR* RoleName)
{
    for (const auto tmpName : g_GmRoleName)
    {
        if (strcmp(tmpName, RoleName) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}
