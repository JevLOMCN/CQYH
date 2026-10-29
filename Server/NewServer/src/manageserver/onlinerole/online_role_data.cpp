#include "stdafx.h"
#include "online_role_data.h"
#include "online_role_data_mgr.h"
#include "role_data.h"
#include "client/user.h"
#include "db/db_operation.h"
#include "error_def.h"
#include "message_handler.h"
#include <algorithm>
#include "master/master_mgr.h"
#include "manageserver.h"
#include "db/ms_sql_stmt_list.h"


BOOL LoadFriends(std::list<void*>& rResult, UINT32 roleID, UINT32 clientSessionID)
{
    INT32       nRetCode = 0;
    MS_ONLINE_ROLE_DATA     *pOnlineRoleData = NULL;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32      dwRoleID;
        UINT32      dwFriendID;
        UINT8       byFriendGroup;
    };
#pragma pack(pop)

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(roleID);
    // Ignore a result for a session that ended while its query was in flight.
    if (!pOnlineRoleData || pOnlineRoleData->dwCliSessionID != clientSessionID)
        return TRUE;

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        if (pLoadResult->dwRoleID == pOnlineRoleData->pRoleData->dwRoleID)
        {
            // friend
            if (pLoadResult->byFriendGroup < MAX_FRIEND_GROUP_COUNT)
                pOnlineRoleData->m_friends.insert(std::make_pair(pLoadResult->dwFriendID, pLoadResult->byFriendGroup));
            else if (FRIEND_GROUP_ENEMY == pLoadResult->byFriendGroup)
                pOnlineRoleData->m_personalEnemy.push_back(pLoadResult->dwFriendID);
            else
                LOG_CHECK_ERROR(FALSE);

            // friends and personal enemy Allan Tao[22.5.2013] 
            CMessageHandler::GetInstance().DoGSAddFriend(pOnlineRoleData->dwGSSessionID, pOnlineRoleData->dwCliSessionID,
                pOnlineRoleData->pRoleData->dwRoleID, pLoadResult->dwFriendID, pLoadResult->byFriendGroup);
        }
        else if (pLoadResult->dwFriendID == pOnlineRoleData->pRoleData->dwRoleID)
        {
            // fans
            if (pLoadResult->byFriendGroup < MAX_FRIEND_GROUP_COUNT)
            {
                pOnlineRoleData->m_fans.insert(pLoadResult->dwRoleID);
            }
            else if (FRIEND_GROUP_ENEMY == pLoadResult->byFriendGroup)
            {
                pOnlineRoleData->m_KillList.insert(pLoadResult->dwRoleID);
            }
        }
    }

   nRetCode = CMessageHandler::GetInstance().DoClientFriendList(pOnlineRoleData);
   LOG_PROCESS_ERROR(nRetCode);

    pOnlineRoleData->BroadCastInfoChange();

    return TRUE;
Exit0:
    return FALSE;
}

void MS_ONLINE_ROLE_DATA::BroadCastInfoChange()
{
    CMessageHandler& mh = CMessageHandler::GetInstance();
    COnlineRoleDataMgr& ordm = COnlineRoleDataMgr::GetInstance();
    MS_ONLINE_ROLE_DATA* roledata = NULL;
    PE_KILL_LIST::iterator   pe_end_iter;

    for (FanMap::iterator it=m_fans.begin(); it!=m_fans.end(); ++it)
    {
        roledata = ordm.FindOnlineRoleByRoleID(*it);
        if (roledata)
            mh.DoClientFriendInfoChange(roledata->pUser->dwCliSessionID, this);
    }

    pe_end_iter = m_KillList.end();
    for (PE_KILL_LIST::iterator iter = m_KillList.begin(); iter != pe_end_iter; ++iter)
    {
        roledata = ordm.FindOnlineRoleByRoleID(*iter);
        if (roledata)
            mh.DoClientFriendInfoChange(roledata->pUser->dwCliSessionID, this);
    }
}

BOOL OnSelectShopLog(std::list<void*>& rResult, UINT32 roleID)
{
#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        INT32 nItemID;
        INT32 nItemCount;
        INT32 nCost;
        UINT32 dwShopTime;
    };
#pragma pack(pop)

    MS_ONLINE_ROLE_DATA* online_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(roleID);
    if (online_role)
    {
        MS_ONLINE_ROLE_DATA::ShopLog shoplog;

        online_role->m_byShopLogLoaded = 1;
        online_role->m_lstShopLog.clear();
        for (std::list<void*>::iterator it=rResult.begin(); it!=rResult.end(); ++it)
        {
            LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
            shoplog.nItemID = pLoadResult->nItemID;
            shoplog.nItemCount = pLoadResult->nItemCount;
            shoplog.nCost = pLoadResult->nCost;
            shoplog.dwShopTime = pLoadResult->dwShopTime;
            online_role->m_lstShopLog.push_back(shoplog);
        }

        CMessageHandler::GetInstance().DoClientRealMoneyShopLog(online_role);
    }

    return TRUE;
}

void MS_ONLINE_ROLE_DATA::SendShopLog()
{
    if (m_byShopLogLoaded)
        CMessageHandler::GetInstance().DoClientRealMoneyShopLog(this);
    else
    {
        INT32 nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmtOwned(msqlSelectShopLog,
            [roleID = pRoleData->dwRoleID](std::list<void*>& rows, void*) { return OnSelectShopLog(rows, roleID); },
            "d", pRoleData->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);
    }
}

void MS_ONLINE_ROLE_DATA::AddShopLog(const ShopLog& shoplog)
{
    if (m_byShopLogLoaded)
    {
        while (m_lstShopLog.size() >= 100)
            m_lstShopLog.pop_front();

        m_lstShopLog.push_back(shoplog);
    }
}
