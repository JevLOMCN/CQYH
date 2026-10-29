#include "stdafx.h"
#include "dbuser_mgr.h"
#include "dbuser.h"

CDBUserMgr::CDBUserMgr()
{
    m_freeuserlist.Clear();
}

CDBUserMgr::~CDBUserMgr()
{

}

BOOL CDBUserMgr::Init(UINT32 dwUserCount)
{
    CDBUser* dbuser;
    for (UINT32 i=0; i<dwUserCount; i++)
    {
        dbuser = new CDBUser;
        LOG_PROCESS_ERROR(dbuser);
        ClearNode(dbuser);
        m_freeuserlist.PushRear(dbuser);
    }
    return TRUE;

Exit0:
    return FALSE;
}

BOOL CDBUserMgr::UnInit()
{
    CDBUser* dbuser = NULL;
    while(dbuser = (CDBUser*)m_freeuserlist.PopHead())
    {
        if (dbuser->m_state == CDBUser::dusInvalid)
            delete dbuser;
    }

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; i++)
    {
        for (USERNAMEMAP::iterator it = m_usernamemap[i].begin(); it != m_usernamemap[i].end(); ++it)
        {
            delete it->second;
        }
        m_usernamemap[i].clear();
    }

    return TRUE;
}

BOOL CDBUserMgr::AddToUserNameMap(CDBUser* dbuser)
{
    INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(dbuser->m_szUserName);

	LOG_PROCESS_ERROR(dbuser->m_nServerGroup >= 0 && dbuser->m_nServerGroup < MAX_SERVER_GROUP_COUNT);
    
	nRetCode = m_usernamemap[dbuser->m_nServerGroup].insert(std::make_pair(dbuser->m_szUserName, dbuser)).second;
    LOG_PROCESS_ERROR(nRetCode);
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBUserMgr::DelFromUserNameMap(CDBUser* dbuser)
{
    INT32 nGroupID = 0;
    USERNAMEMAP_ITR itr;

    LOG_PROCESS_ERROR(dbuser->m_szUserName);

    nGroupID = dbuser->m_nServerGroup;
    LOG_PROCESS_ERROR(nGroupID < MAX_SERVER_GROUP_COUNT);

    itr = m_usernamemap[nGroupID].find(dbuser->m_szUserName);
    LOG_PROCESS_ERROR(itr != m_usernamemap[nGroupID].end());

    m_usernamemap[nGroupID].erase(itr);

    return TRUE;

Exit0:
    return FALSE;
}

CDBUser* CDBUserMgr::GetDBUserByName(const char* pcszUserName, INT32 nGroupID)
{
    USERNAMEMAP_ITR itr;

    LOG_PROCESS_ERROR(nGroupID < MAX_SERVER_GROUP_COUNT);
    
    itr = m_usernamemap[nGroupID].find(pcszUserName);
    PROCESS_ERROR(itr != m_usernamemap[nGroupID].end());

    return itr->second;
Exit0:
    return NULL;
}

BOOL CDBUserMgr::AddToSessionIDMap(CDBUser* dbuser)
{
    BOOL bRet = m_sessionidmap.insert(SESSIONIDMAP::value_type(dbuser->m_dwClientSessionID, dbuser)).second;
    LOG_CHECK_ERROR(bRet);
    return bRet;
}

BOOL CDBUserMgr::DelFromSessionIDMap(CDBUser* dbuser)
{
    SESSIONIDMAP::iterator it = m_sessionidmap.find(dbuser->m_dwClientSessionID);
    LOG_PROCESS_ERROR(it != m_sessionidmap.end());

    m_sessionidmap.erase(it);

    return TRUE;

Exit0:
    return FALSE;
}

CDBUser* CDBUserMgr::GetDBUserBySessionID(UINT32 dwSessionID)
{
    SESSIONIDMAP::iterator it = m_sessionidmap.find(dwSessionID);
    if (it == m_sessionidmap.end())
    {
        return NULL;
    }
    else
    {
        return it->second;
    }
}

CDBUser* CDBUserMgr::Pop()
{
    CDBUser* dbuser = (CDBUser*)m_freeuserlist.PopHead();

    LOG_PROCESS_ERROR(dbuser);
    LOG_PROCESS_ERROR(dbuser->m_state == CDBUser::dusInvalid || dbuser->m_state == CDBUser::dusLogout);
    LOG_PROCESS_ERROR(dbuser->m_dbstate == CDBUser::dtsFree);

    if (dbuser->m_state == CDBUser::dusLogout)
    {
        BOOL bRet = dbuser->RemoveAllRoles();
        LOG_PROCESS_ERROR(bRet);

        DelFromUserNameMap(dbuser);
        dbuser->m_state = CDBUser::dusInvalid;
    }
    return dbuser;

Exit0:
    if (dbuser)
    {
        BOOL bRet = m_freeuserlist.PushRear(dbuser);
        LOG_CHECK_ERROR(bRet);
        CRI("Added A Failed CDBUser To The Rear Of List");
    }
    return NULL;
}

BOOL CDBUserMgr::Pop(CDBUser* dbuser)
{
    BOOL bRet = FALSE;
    LOG_PROCESS_ERROR(dbuser);
    LOG_PROCESS_ERROR(dbuser->m_state == CDBUser::dusLogout);

    bRet = CBaseList::Remove(dbuser);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    return FALSE;
}

void CDBUserMgr::Push(CDBUser* dbuser)
{
    BOOL bRet = FALSE;
    LOG_PROCESS_ERROR(dbuser);
    LOG_PROCESS_ERROR(dbuser->m_state == CDBUser::dusInvalid || dbuser->m_state == CDBUser::dusLogout);

    bRet = m_freeuserlist.PushRear(dbuser);
    LOG_PROCESS_ERROR(bRet);

Exit0:
    return;
}
