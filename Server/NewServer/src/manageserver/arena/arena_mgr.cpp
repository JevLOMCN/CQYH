#include "stdafx.h"
#include "arena_mgr.h"
#include "scene_data.h"
#include "scene/scenemgr.h"
#include "ms_session_mgr.h"
#include "message_handler.h"
#include "game_world_config.h"
#include "server_config.h"
#include "manageserver.h"

#include "db/ms_sql_stmt_list.h"
#include "sql_mgr.h"

#include "onlinerole/online_role_data_mgr.h"

CArenaMgr CArenaMgr::m_Instace;

CArenaMgr& CArenaMgr::GetInstance()
{
    return m_Instace;
}

CArenaMgr::CArenaMgr()
{

}

CArenaMgr::~CArenaMgr()
{

}

BOOL CArenaMgr::Init()
{
    INT32           nRetCode = 0;
    COMPETITOR_DATA     *pRankData = NULL;
    ARENA_MATCH_DATA    *pMatchData = NULL;

    memset(&m_qwFund, 0, sizeof(m_qwFund));
    memset(&m_nMatchingNum, 0, sizeof(m_nMatchingNum));

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        m_RankList[i].Clear();
        for (INT32 j = 0; j < MAX_ARENA_RANK_NUM; ++j)
        {
            pRankData = new COMPETITOR_DATA();
            ClearNode((BASE_LIST_NODE*)pRankData);
            m_RankList[i].PushRear((BASE_LIST_NODE*)pRankData);
        }

        m_MatchList[i].Clear();

        m_FreeMatchDataList[i].Clear();
        for (INT32 j = 0; j < MAX_ARENA_MATCH_NUM; ++j)
        {
            pMatchData = new ARENA_MATCH_DATA();
            ClearNode((BASE_LIST_NODE*)pMatchData);
            m_FreeMatchDataList[i].PushRear((BASE_LIST_NODE*)pMatchData);
        }
    }

    // select sql
    nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectArena, LoadArena, NULL, "");
    LOG_PROCESS_ERROR(nRetCode);

    // init object pool
    nRetCode = m_settleTriggerPool.Init(8);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _InitTimer();
    LOG_PROCESS_ERROR(nRetCode);


    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArenaMgr::UnInit()
{
    INT32       nRetCode  = 0;
    ARENA_MATCH_DATA *pData = NULL;
    COMPETITOR_DATA     *pRankData = NULL;
    BASE_LIST_NODE  *pNode = NULL;

    memset(&m_qwFund, 0, sizeof(m_qwFund));
    memset(&m_nMatchingNum, 0, sizeof(m_nMatchingNum));

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pNode = m_RankList[i].PopHead();
        while(pNode)
        {
            pRankData = (COMPETITOR_DATA *)pNode;
            delete pRankData;
            pRankData = NULL;
            pNode = m_RankList[i].PopHead();
        }
        m_RankList[i].Clear();

        pNode = m_FreeMatchDataList[i].PopHead();
        while(pNode)
        {
            pData = (ARENA_MATCH_DATA *)pNode;
            delete pData;
            pData = NULL;
            pNode = m_FreeMatchDataList[i].PopHead();
        }
        m_FreeMatchDataList[i].Clear();

        pNode = m_MatchList[i].PopHead();
        while(pNode)
        {
            pData = (ARENA_MATCH_DATA *)pNode;
            delete pData;
            pData = NULL;
            pNode = m_MatchList[i].PopHead();
        }
        m_MatchList[i].Clear();
    }

    nRetCode = m_settleTriggerPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
//Exit0:
//    return FALSE;
}

BOOL CArenaMgr::ReInit()
{
    INT32   nRetCode = 0;
    BASE_LIST_NODE      *pNode = NULL;
    COMPETITOR_DATA     *pRankData = NULL;

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pNode = m_RankList[i].Head().pNext;
        while(pNode != &m_RankList[i].Rear())
        {
            pRankData = (COMPETITOR_DATA *)pNode;

            pRankData->dwRoleID = 0;
            pRankData->byPointSeasonVictoryTimes = 0;
            pRankData->nPointSeasonEndTime = 0; 

            pNode = pNode->pNext;
        }
    }


    memset(&m_qwFund, 0, sizeof(m_qwFund));

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        nRetCode = _UpdateFund(i);
        LOG_CHECK_ERROR(nRetCode);
    }

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        nRetCode = _UpdateRank(i);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
}


BOOL CArenaMgr::Load(UINT8 byGroup, UINT64 qwFund, ARENA_DB_DATA *pData)
{
    BASE_LIST_NODE      *pNode = NULL;
    COMPETITOR_DATA     *pRankData = NULL;
    INT32       nIndex = 0;

    LOG_PROCESS_ERROR(pData);
    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    m_qwFund[byGroup] += qwFund;

    pNode = m_RankList[byGroup].Head().pNext;
    while (pNode != &m_RankList[byGroup].Rear())
    {
        pRankData = (COMPETITOR_DATA *)pNode;

        pRankData->dwRoleID = pData->rankData[nIndex].dwRoleID;
        pRankData->byPointSeasonVictoryTimes = pData->rankData[nIndex].byPointSeasonVictoryTimes;
        pRankData->nPointSeasonEndTime = pData->rankData[nIndex].nPointSeasonEndTime;
        ++nIndex;

        pNode = pNode->pNext;
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArenaMgr::JoinArena(UINT8 byGroup, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    m_qwFund[byGroup] += ARENA_JOIN_SEASON_MONEY_CONSUMPTION;

    // update sql
    nRetCode = _UpdateFund(byGroup);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

UINT64 CArenaMgr::GetFund(UINT8 byGroup)
{ 
    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);
    return m_qwFund[byGroup];
Exit0:
    return 0;
}

BOOL CArenaMgr::Match(UINT8 byGroup, UINT32 dwRoleID)
{
    INT32               nRetCode = 0;
    BOOL                bMatch = FALSE;
    UINT32              dwTempRoleID = 0;
    BASE_LIST_NODE      *pNode = NULL;
    ARENA_MATCH_DATA    *pMatchData = NULL;

    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    // check repeat
    pNode = m_MatchList[byGroup].Head().pNext;
    while (pNode != &m_MatchList[byGroup].Rear())
    {
        pMatchData = (ARENA_MATCH_DATA *)pNode;
        LOG_PROCESS_ERROR(pMatchData->dwRoleID != dwRoleID);
        pNode = pNode->pNext;
    }

    // check immediately match ( first person washout or queue full )
    pNode = m_MatchList[byGroup].Head().pNext;
    if (pNode != &m_MatchList[byGroup].Rear())
    {
        pMatchData = (ARENA_MATCH_DATA *)pNode;
        if ((CManageServer::GetInstance().GetGameTime() - pMatchData->nMatchTime >= ARENA_MATCH_WAIT_TIME) || (MAX_ARENA_MATCH_NUM == m_nMatchingNum[byGroup]))
        {
            bMatch = TRUE;
            --m_nMatchingNum[byGroup];
            m_MatchList[byGroup].Remove(pNode);
        }
    }

    if (bMatch)
    {
        // if immediately match, create scene and so on.
        pMatchData = (ARENA_MATCH_DATA *)pNode;
        LOG_PROCESS_ERROR(pMatchData);

        dwTempRoleID = pMatchData->dwRoleID;

        m_FreeMatchDataList[byGroup].PushRear((BASE_LIST_NODE*)pMatchData);

        nRetCode = _CreateArena(dwTempRoleID, dwRoleID);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        // join the matching list
        pNode = m_FreeMatchDataList[byGroup].PopHead();
        LOG_PROCESS_ERROR(pNode);
        
        ClearNode(pNode);
        pMatchData = (ARENA_MATCH_DATA *)pNode;
        pMatchData->dwRoleID = dwRoleID;
        pMatchData->nMatchTime = CManageServer::GetInstance().GetGameTime();
        
        m_MatchList[byGroup].PushRear((BASE_LIST_NODE*)pMatchData);
    }    

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArenaMgr::UpdateSeasonInfo(UINT8 byGroup, UINT32 dwRoleID, UINT8 byPointSeasonVictoryTimes, INT32 nPointSeasonEndTime)
{
    INT32       nRetCode = 0;
    BOOL        bUpdate = FALSE;
    BASE_LIST_NODE      *pPrev = NULL;
    BASE_LIST_NODE      *pNext = NULL;
    COMPETITOR_DATA     *pCur = NULL;
    COMPETITOR_DATA     *pInsert = NULL;

    COMPETITOR_DATA     temp(dwRoleID, byPointSeasonVictoryTimes, nPointSeasonEndTime);

    PROCESS_SUCCESS(0 == byPointSeasonVictoryTimes);
    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    // scan and try to find 
    pNext = m_RankList[byGroup].Head().pNext;
    while (pNext != &m_RankList[byGroup].Rear())
    {
        pCur = (COMPETITOR_DATA *)pNext;
        if (pCur->dwRoleID == dwRoleID)
            break;
        else
            pNext = pNext->pNext;
    }

    if (pNext != &m_RankList[byGroup].Rear())
    {
        pCur->dwRoleID = dwRoleID;
        pCur->byPointSeasonVictoryTimes = byPointSeasonVictoryTimes;
        pCur->nPointSeasonEndTime = nPointSeasonEndTime;
    }
    else
    {
        pPrev = m_RankList[byGroup].Rear().pPrev;
        while (pPrev != &m_RankList[byGroup].Head())
        {
            pCur = (COMPETITOR_DATA*)pPrev;
            if ( (pCur->dwRoleID != 0) && (*pCur > temp) )
            {
                if (pPrev->pNext != &m_RankList[byGroup].Rear())
                {
                    pInsert = (COMPETITOR_DATA*)pPrev->pNext;
                    pInsert->dwRoleID = dwRoleID;
                    pInsert->byPointSeasonVictoryTimes = byPointSeasonVictoryTimes;
                    pInsert->nPointSeasonEndTime = nPointSeasonEndTime;
                    break;
                }
            }
            pPrev = pPrev->pPrev;
        }

        if (pPrev == &m_RankList[byGroup].Head())
        {
            pInsert = (COMPETITOR_DATA*)(m_RankList[byGroup].Head().pNext);
            pInsert->dwRoleID = dwRoleID;
            pInsert->byPointSeasonVictoryTimes = byPointSeasonVictoryTimes;
            pInsert->nPointSeasonEndTime = nPointSeasonEndTime;
        }
    }

    // update sql
    nRetCode = _UpdateRank(byGroup);
    LOG_PROCESS_ERROR(nRetCode);

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

INT32 CArenaMgr::GetRank(UINT8 byGroup, UINT32 dwRoleID)
{
    INT32       nRank = 0;
    BASE_LIST_NODE      *pNext = NULL;
    COMPETITOR_DATA     *pCur = NULL;

    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    // scan and try to find 
    pNext = m_RankList[byGroup].Head().pNext;
    LOG_PROCESS_ERROR(pNext != &m_RankList[byGroup].Rear());
    while (pNext != &m_RankList[byGroup].Rear())
    {
        ++nRank;
        pCur = (COMPETITOR_DATA *)pNext;
        if (pCur->dwRoleID == dwRoleID)
            return nRank;
        else
            pNext = pNext->pNext;
    }

    ++nRank;
    return nRank;
Exit0:
    return MAX_ARENA_RANK_NUM;
}

COMPETITOR_DATA* CArenaMgr::GetStrongest(UINT8 byGroup)
{
    BASE_LIST_NODE      *pNext = NULL;
    COMPETITOR_DATA     *pCur = NULL;

    pNext = m_RankList[byGroup].Head().pNext;
    LOG_PROCESS_ERROR(pNext != &m_RankList[byGroup].Rear());
    pCur = (COMPETITOR_DATA *)pNext;
    PROCESS_ERROR(pCur->dwRoleID != 0);

    return pCur;
Exit0:
    return NULL;
}

UINT32 CArenaMgr::GetReward(UINT8 byGroup)
{
    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    if (m_qwFund[byGroup] < g_GWConfig.ArenaReward[0].nFundLimit)
    {
        return g_GWConfig.ArenaReward[0].dwItemID;
    }
    else if (m_qwFund[byGroup] < g_GWConfig.ArenaReward[1].nFundLimit)
    {
        return g_GWConfig.ArenaReward[1].dwItemID;
    }
    else if (m_qwFund[byGroup] < g_GWConfig.ArenaReward[2].nFundLimit)
    {
        return g_GWConfig.ArenaReward[2].dwItemID;
    }
    else
    {
        return g_GWConfig.ArenaReward[3].dwItemID;
    }

Exit0:
    return g_GWConfig.ArenaReward[0].dwItemID;
}

INT32 CArenaMgr::GetRewardMoney(UINT8 byGroup, INT32 nRank)
{
    INT64       qwMoney = 0;

    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    if (nRank <= ARENA_SPECIAL_REWARD_RANK_LIMIT)
        qwMoney = (UINT64)m_qwFund[byGroup] * ARENA_SPECIAL_REWARD_PERCENT / 100;
    else if (nRank <= ARENA_FIRST_REWARD_RANK_LIMIT)
        qwMoney = (UINT64)m_qwFund[byGroup] * ARENA_FIRST_REWARD_PERCENT / 100;
    else if (nRank <= ARENA_SECOND_REWARD_RANK_LIMIT)
        qwMoney = (UINT64)m_qwFund[byGroup] * ARENA_SECOND_REWARD_PERCENT / 100;
    else if (nRank <= ARENA_THIRD_REWARD_RANK_LIMIT)
        qwMoney = (UINT64)m_qwFund[byGroup] * ARENA_THIRD_REWARD_PERCENT / 100;
    else 
        qwMoney = 0;

    if (qwMoney > MAX_MONEY)
    {
        qwMoney = MAX_MONEY;
    }
    else if (qwMoney < 0)
    {
        qwMoney = 0;
    }

    return qwMoney;
Exit0:
    return 0;
}

//void CArenaMgr::Settlement()
//{
//    INT32           nRetCode = 0;
//    INT32           nIndex = 0;
//    COMPETITOR_DATA    *pCompotitor = NULL;
//    BASE_LIST_NODE      *pNode = NULL;
//
//    pNode = m_RankList.Head().pNext;
//    while (pNode != &m_RankList.Rear())
//    {
//        pCompotitor = (COMPETITOR_DATA *)pNode;
//
//    }
//}

void CArenaMgr::End()
{
    INT32   nRetCode = 0;
    BASE_TRIGGER_NODE       *pTrigger = NULL;
    BASE_LIST_NODE          *pNode = NULL;
    ARENA_MATCH_DATA         *pMatch = NULL;
    MS_ONLINE_ROLE_DATA     *pOnlineRole = NULL;

    // cancel match and notify user
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        pNode = m_MatchList[i].PopHead();
        while(pNode)
        {
            pMatch = (ARENA_MATCH_DATA *)pNode;

            pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMatch->dwRoleID);
            LOG_CHECK_ERROR(pOnlineRole);
            if (pOnlineRole)
            {
                nRetCode = CMessageHandler::GetInstance().DoClientArenaEnd(pOnlineRole->dwCliSessionID);
                LOG_CHECK_ERROR(nRetCode);
            }

            m_FreeMatchDataList[i].PushRear(pNode);
            pNode = m_MatchList[i].PopHead();
        }
    }

    // register clear trigger
    pTrigger = m_settleTriggerPool.New();
    LOG_PROCESS_ERROR(pTrigger);

    pTrigger->fp = CB_ClearArena;
	pTrigger->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + 10800;        // 21 clock, add 3 hours, new day , clear reward
	pTrigger->nTriggerType = tttInvalid;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CArenaMgr::Clear()
{
    ReInit();
}

BOOL CArenaMgr::DeleteTriggerData(BASE_TRIGGER_NODE *pNode)
{
    LOG_PROCESS_ERROR(pNode);

    m_settleTriggerPool.Delete(pNode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArenaMgr::LuaBroadcastArenaBegin()
{
    return CMessageHandler::GetInstance().DoClientArenaMatchBeginNtf();
}

BOOL CArenaMgr::MainLoop()
{
    INT32       nRetCode = 0;

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        nRetCode = _Match(i);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
//Exit0:
//    return FALSE;
}

BOOL CArenaMgr::_Match(UINT8 byGroup)
{
    INT32               nRetCode = 0;
    BASE_LIST_NODE      *pNode = NULL;
    BASE_LIST_NODE      *pNode2 = NULL;
    ARENA_MATCH_DATA    *pMatchData1 = NULL;
    ARENA_MATCH_DATA    *pMatchData2 = NULL;

    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    pNode = m_MatchList[byGroup].Head().pNext;
    pNode2 = m_MatchList[byGroup].Rear().pPrev;
    if (pNode != &m_MatchList[byGroup].Rear() && pNode2 != pNode)
    {
        pMatchData1 = (ARENA_MATCH_DATA *)pNode;
        if ((CManageServer::GetInstance().GetGameTime() - pMatchData1->nMatchTime >= ARENA_MATCH_WAIT_TIME))
        {
            --m_nMatchingNum[byGroup];
            m_MatchList[byGroup].Remove(pNode);
            --m_nMatchingNum[byGroup];
            m_MatchList[byGroup].Remove(pNode2);

            pMatchData2 = (ARENA_MATCH_DATA *)pNode2;

            nRetCode = _CreateArena(pMatchData1->dwRoleID, pMatchData2->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);

            m_FreeMatchDataList[byGroup].PushRear(pNode);
            m_FreeMatchDataList[byGroup].PushRear(pNode2);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArenaMgr::_CreateArena(UINT32 dwRoleID1, UINT32 dwRoleID2)
{
    INT32 nRetCode = 0;
    UINT32 dwSuitableGS = 0;
    SCENE_INFO* poSceneInfo = NULL;
    const SCENE_DATA* poSceneData = NULL;
    MS_ONLINE_ROLE_DATA     *pOnlineRole1 = NULL;
    MS_ONLINE_ROLE_DATA     *pOnlineRole2 = NULL;

    LOG_PROCESS_ERROR(g_GWConfig.dwArenaSceneTemplateID < MAX_SCENE_TEMPLATE_COUNT);

    poSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(g_GWConfig.dwArenaSceneTemplateID);
    LOG_PROCESS_ERROR(poSceneData);

    dwSuitableGS = CMSSessionMgr::GetInstance().GetSuitableGS(0);
    LOG_PROCESS_ERROR(dwSuitableGS > 0);

    poSceneInfo = CSceneMgr::GetInstance().CreateScene(g_GWConfig.dwArenaSceneTemplateID, 0, 0);
    LOG_PROCESS_ERROR(poSceneInfo);

    pOnlineRole1 = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID1);
    LOG_PROCESS_ERROR(pOnlineRole1);

    pOnlineRole2 = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID2);
    LOG_PROCESS_ERROR(pOnlineRole2);

    poSceneInfo->dwGSSessionID = dwSuitableGS;
    poSceneInfo->eState = ESCENESTATE_CREATING;
    poSceneInfo->dwCreateTime = CManageServer::GetInstance().GetGameTime();
    poSceneInfo->WaitUserList.clear();
    //poSceneInfo->WaitUserList.push_back(pOnlineRole1->dwCliSessionID);
    //poSceneInfo->WaitUserList.push_back(pOnlineRole2->dwCliSessionID);

	nRetCode = CMessageHandler::GetInstance().DoGSCreateScene(dwSuitableGS, poSceneInfo->nServerGroupID,
		g_GWConfig.dwArenaSceneTemplateID, poSceneInfo->nSceneID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSJumpArenaScene(pOnlineRole1->dwGSSessionID, dwRoleID1,
        poSceneInfo->nSceneTempID, poSceneInfo->nSceneID, 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSJumpArenaScene(pOnlineRole2->dwGSSessionID, dwRoleID2,
        poSceneInfo->nSceneTempID, poSceneInfo->nSceneID, 1);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArenaMgr::_InitTimer()
{
    INT32       nRetCode = 0;
    UINT32      dwInterval = 0;
    UINT32      dwTempValue = 0;
    BASE_TRIGGER_NODE   *pTrigger = NULL;

    UINT32      dwCurrentTime = CManageServer::GetInstance().GetGameTime();
    struct tm StartTM;

    dwTempValue = dwCurrentTime % ARENA_WEEK_SECOND;

    //////////////////////////////////////////////////////////////////////////
    // Tuesday
    if (dwTempValue <= ARENA_TUESDAY_SETTLE_TIME)
        dwInterval = ARENA_TUESDAY_SETTLE_TIME - dwTempValue;
    else
        dwInterval = ARENA_WEEK_SECOND - dwTempValue + ARENA_TUESDAY_SETTLE_TIME;

    pTrigger = m_settleTriggerPool.New();
    LOG_PROCESS_ERROR(pTrigger);

    ClearNode((BASE_LIST_NODE*)pTrigger);
    pTrigger->fp = CB_TuesdayBegin;
    pTrigger->dwWashoutTime = dwInterval + dwCurrentTime - 3600;
	pTrigger->nTriggerType = tttInvalid;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);
    pTrigger = NULL;

    //
    pTrigger = m_settleTriggerPool.New();
    LOG_PROCESS_ERROR(pTrigger);

    ClearNode((BASE_LIST_NODE*)pTrigger);
    pTrigger->fp = CB_TuesdaySettlement;
    pTrigger->dwWashoutTime = dwInterval + dwCurrentTime;
	pTrigger->nTriggerType = tttInvalid;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);
    pTrigger = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Thursday
    if (dwTempValue <= ARENA_THURSDAY_SETTLE_TIME)
        dwInterval = ARENA_THURSDAY_SETTLE_TIME - dwTempValue;
    else
        dwInterval = ARENA_WEEK_SECOND - dwTempValue + ARENA_THURSDAY_SETTLE_TIME;

    pTrigger = m_settleTriggerPool.New();
    LOG_PROCESS_ERROR(pTrigger);

    ClearNode((BASE_LIST_NODE*)pTrigger);
    pTrigger->fp = CB_ThursdayBegin;
    pTrigger->dwWashoutTime = dwInterval + dwCurrentTime - 3600;
	pTrigger->nTriggerType = tttInvalid;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);
    pTrigger = NULL;

    //
    pTrigger = m_settleTriggerPool.New();
    LOG_PROCESS_ERROR(pTrigger);

    ClearNode((BASE_LIST_NODE*)pTrigger);
    pTrigger->fp = CB_ThursdaySettlement;
    pTrigger->dwWashoutTime = dwInterval + dwCurrentTime;
	pTrigger->nTriggerType = tttInvalid;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);
    pTrigger = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Saturday
    if (dwTempValue <= ARENA_SATURDAY_SETTLE_TIME)
        dwInterval = ARENA_SATURDAY_SETTLE_TIME - dwTempValue;
    else
        dwInterval = ARENA_WEEK_SECOND - dwTempValue + ARENA_SATURDAY_SETTLE_TIME;

    pTrigger = m_settleTriggerPool.New();
    LOG_PROCESS_ERROR(pTrigger);

    ClearNode((BASE_LIST_NODE*)pTrigger);
    pTrigger->fp = CB_SaturdayBegin;
    pTrigger->dwWashoutTime = dwInterval + dwCurrentTime - 3600;
	pTrigger->nTriggerType = tttInvalid;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);
    pTrigger = NULL;

    //
    pTrigger = m_settleTriggerPool.New();
    LOG_PROCESS_ERROR(pTrigger);

    ClearNode((BASE_LIST_NODE*)pTrigger);
    pTrigger->fp = CB_SaturdaySettlement;
    pTrigger->dwWashoutTime = dwInterval + dwCurrentTime;
	pTrigger->nTriggerType = tttInvalid;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);
    pTrigger = NULL;

    return TRUE;
Exit0:
    if (pTrigger)
        m_settleTriggerPool.Delete(pTrigger);
    return FALSE;
}


BOOL CArenaMgr::_UpdateFund(UINT8 byGroup)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);

    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateArenaFund, NULL, NULL, "uluc", m_qwFund[byGroup], byGroup);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArenaMgr::_UpdateRank(UINT8 byGroup)
{
    INT32       nRetCode = 0;
    BASE_LIST_NODE  *pNode = NULL;
    COMPETITOR_DATA    *pCompotitor = NULL;
    INT32       nIndex = 0;
    ARENA_DB_DATA      rankDBData;

    LOG_PROCESS_ERROR(byGroup < MAX_SERVER_GROUP_COUNT);
    memset(&rankDBData, 0, sizeof(rankDBData));
    
    pNode = m_RankList[byGroup].Head().pNext;
    while (pNode != &m_RankList[byGroup].Rear())
    {
        pCompotitor = (COMPETITOR_DATA *)pNode;
        LOG_PROCESS_ERROR(pCompotitor);

        rankDBData.rankData[nIndex].dwRoleID = pCompotitor->dwRoleID;
        rankDBData.rankData[nIndex].byPointSeasonVictoryTimes = pCompotitor->byPointSeasonVictoryTimes;
        rankDBData.rankData[nIndex].nPointSeasonEndTime = pCompotitor->nPointSeasonEndTime;
        ++nIndex;

        pNode = pNode->pNext;
    }

    if (nIndex)
    {
        nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateArenaRank, NULL, NULL, "buc", (char*)&rankDBData, sizeof(rankDBData), byGroup);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CB_TuesdayBegin(BASE_TRIGGER_NODE *pNode)
{
    CMessageHandler::GetInstance().DoClientArenaMatchBeginNtf();

    pNode->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + ARENA_WEEK_SECOND;

    return TIME_TRIGGER_RET_CONTINUE;
//Exit0:
//    return TIME_TRIGGER_RET_CONTINUE;
}

INT32 CB_ThursdayBegin(BASE_TRIGGER_NODE *pNode)
{
    CMessageHandler::GetInstance().DoClientArenaMatchBeginNtf();

    pNode->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + ARENA_WEEK_SECOND;

    return TIME_TRIGGER_RET_CONTINUE;
//Exit0:
//    return TIME_TRIGGER_RET_CONTINUE;
}

INT32 CB_SaturdayBegin(BASE_TRIGGER_NODE *pNode)
{
    CMessageHandler::GetInstance().DoClientArenaMatchBeginNtf();

    pNode->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + ARENA_WEEK_SECOND;

    return TIME_TRIGGER_RET_CONTINUE;
//Exit0:
//    return TIME_TRIGGER_RET_CONTINUE;
}

INT32 CB_TuesdaySettlement(BASE_TRIGGER_NODE *pNode)
{
    CArenaMgr::GetInstance().End();

    pNode->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + ARENA_WEEK_SECOND;

    return TIME_TRIGGER_RET_CONTINUE;
//Exit0:
//    return TIME_TRIGGER_RET_CONTINUE;
}

INT32 CB_ThursdaySettlement(BASE_TRIGGER_NODE *pNode)
{
    CArenaMgr::GetInstance().End();

    pNode->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + ARENA_WEEK_SECOND;

    return TIME_TRIGGER_RET_CONTINUE;
//Exit0:
//    return TIME_TRIGGER_RET_CONTINUE;
}

INT32 CB_SaturdaySettlement(BASE_TRIGGER_NODE *pNode)
{
    CArenaMgr::GetInstance().End();

    pNode->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + ARENA_WEEK_SECOND;

    return TIME_TRIGGER_RET_CONTINUE;
//Exit0:
//    return TIME_TRIGGER_RET_CONTINUE;
}

INT32 CB_ClearArena(BASE_TRIGGER_NODE *pNode)
{
    INT32       nRetCode = 0;

    CArenaMgr::GetInstance().Clear();

    nRetCode = CArenaMgr::GetInstance().DeleteTriggerData(pNode);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return TIME_TRIGGER_RET_END;
}

BOOL LoadArena(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    UINT8           arrSvrGroup[MAX_SERVER_GROUP_COUNT] = {0};
    ARENA_DB_DATA   tempRankData;

    memset(&tempRankData, 0, sizeof(tempRankData));

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8           bySvrGroup;
        UINT64          qwFund;
        INT32           nBlobLength;
        ARENA_DB_DATA   rankData;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->bySvrGroup));
		LOG_PROCESS_ERROR(pLoadResult->bySvrGroup < MAX_SERVER_GROUP_COUNT);
		if (g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID > 0 &&
			g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID < MAX_SERVER_GROUP_COUNT)
		{
			pLoadResult->bySvrGroup = g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID;
		}

        arrSvrGroup[pLoadResult->bySvrGroup] = 1;

        nRetCode = CArenaMgr::GetInstance().Load(pLoadResult->bySvrGroup, pLoadResult->qwFund, &pLoadResult->rankData);
        LOG_PROCESS_ERROR(nRetCode);

    }
    INF("Loading Arena Data Finished.");

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        if (arrSvrGroup[i] == 0 && IsValidGroup(i))
        {
            nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlInsertArena, NULL, NULL, "uculb", i, 0, &tempRankData, sizeof(tempRankData));
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}
