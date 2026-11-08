#include "stdafx.h"
#include "ranklist.h"

#include "manageserver.h"
#include "server_def.h"
#include "role_data.h"
#include "time_ex.h"
#include <time.h>
#include "guild/ms_guild.h"
#include "role_data_mgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "db/ms_sql_stmt_list.h"
#include "server_config.h"

#define AVERAGE_LEVEL_COUNT	(500)

CRankListMgr CRankListMgr::ms_Instance;

CRankListMgr::CRankListMgr()
{
    m_bLoading = FALSE;
	memset(m_nAverageLevel, 0, sizeof(m_nAverageLevel));
}

CRankListMgr::~CRankListMgr()
{
    UnInit();
}

CRankListMgr& CRankListMgr::GetInstance(void)
{
	return ms_Instance;
}

BOOL CRankListMgr::Init()
{
	INT32 nRetCode = 0;
    UINT64 qwStartTick = GetTickCountEx();

	nRetCode = m_RankPool.Init(MAX_SERVER_GROUP_COUNT * rlTotal * MAX_RANK_LIST_NODE_COUNT);
	LOG_PROCESS_ERROR(nRetCode);

	for (INT32 nServerGroupID = 0; nServerGroupID < MAX_SERVER_GROUP_COUNT; nServerGroupID++)
	{
		for (INT32 nRanklist = 0; nRanklist < rlTotal; nRanklist++)
		{
			m_RankList[nServerGroupID][nRanklist].Clear();
		}
	}

    INF("loading rank list...");
    m_bLoading = TRUE;
    m_nLoadedCount = 0;
	memset(m_nAverageLevel, 0, sizeof(m_nAverageLevel));

    nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(msqlSelectRankList, _Load, NULL, "");
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateAverageLevel();
	LOG_PROCESS_ERROR(nRetCode);

    INF("loaded %d rank node from ranklist (%d ms)", m_nLoadedCount, (INT32)(GetTickCountEx() - qwStartTick));
    m_bLoading = FALSE;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRankListMgr::UnInit()
{
	INT32 nRetCode = 0;

	nRetCode = m_RankPool.UnInit();
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CRankListMgr::GetRankNodeCount(INT32 nServerGroupID, INT32 nRankListType)
{
	INT32 nRetCode = 0;
	RANK_NODE* pRank = NULL;

    LOG_PROCESS_ERROR((nServerGroupID >= 0) && (nServerGroupID < MAX_SERVER_GROUP_COUNT));
	LOG_PROCESS_ERROR(IsValidGroup(nServerGroupID));
	LOG_PROCESS_ERROR((nRankListType >= 0) && (nRankListType < rlTotal));

	return (INT32)m_Map_Key_2_Rank[nServerGroupID][nRankListType].size();
Exit0:
	return 0;
}

RANK_NODE* CRankListMgr::GetSelfRank(INT32 nServerGroupID, INT32 nRankListType, INT64 llKey)
{
	Iter_MapKey2RankNode Iter;

    LOG_PROCESS_ERROR((nServerGroupID >= 0) && (nServerGroupID < MAX_SERVER_GROUP_COUNT));
	LOG_PROCESS_ERROR(IsValidGroup(nServerGroupID));
	LOG_PROCESS_ERROR((nRankListType >= 0) && (nRankListType < rlTotal));

	Iter = m_Map_Key_2_Rank[nServerGroupID][nRankListType].find(llKey);
	PROCESS_ERROR(Iter != m_Map_Key_2_Rank[nServerGroupID][nRankListType].end());
	LOG_PROCESS_ERROR(Iter->second);

	return Iter->second;

Exit0:
	return 0;
}

BOOL CRankListMgr::DelRankNode(INT32 nServerGroupID, INT64 llKey)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR((nServerGroupID >= 0) && (nServerGroupID < MAX_SERVER_GROUP_COUNT));
	LOG_PROCESS_ERROR(IsValidGroup(nServerGroupID));

	for (INT32 nRankListType = rlBegin; nRankListType < rlTotal; nRankListType++)
	{
        RANK_NODE* pRankNode = NULL;

        Iter_MapKey2RankNode Iter = m_Map_Key_2_Rank[nServerGroupID][nRankListType].find(llKey);
        if (Iter == m_Map_Key_2_Rank[nServerGroupID][nRankListType].end())
            continue;

		pRankNode = Iter->second;
        LOG_CHECK_ERROR(pRankNode);
        if (pRankNode == NULL)
            continue;

		for (RANK_NODE* pNode = (RANK_NODE*)pRankNode->pNext;
			pNode != &m_RankList[nServerGroupID][nRankListType].Rear();
			pNode = (RANK_NODE*)pNode->pNext)
		{
			pNode->nSelfRank--;
		}

		nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlDeleteRankList, NULL, NULL, "ucdl", nServerGroupID, nRankListType, llKey);
		LOG_CHECK_ERROR(nRetCode);

        nRetCode = CBaseList::Remove(pRankNode);
        LOG_CHECK_ERROR(nRetCode);

        m_Map_Key_2_Rank[nServerGroupID][nRankListType].erase(Iter);

        nRetCode = m_RankPool.Delete(pRankNode);
        LOG_CHECK_ERROR(nRetCode);
	}
	
	return TRUE;
Exit0:
	return FALSE;
}

CBaseList* CRankListMgr::GetRankList(INT32 nServerGroupID, INT32 nRankListType)
{
	LOG_PROCESS_ERROR((nRankListType >= 0) && (nRankListType < rlTotal));
	LOG_PROCESS_ERROR((nServerGroupID >= 0) && (nServerGroupID < MAX_SERVER_GROUP_COUNT));
	LOG_PROCESS_ERROR(IsValidGroup(nServerGroupID));

	return &m_RankList[nServerGroupID][nRankListType];
Exit0:
	return NULL;
}

INT32 CRankListMgr::UpdateGuildRank(CMsGuild* pGuild)
{
	INT32 nRetCode = 0;
	INT32 nGuildID = 0;
	INT32 nServerGroupID = 0;

	LOG_PROCESS_ERROR(pGuild);

	nServerGroupID = pGuild->GetSvrGroup();
	LOG_PROCESS_ERROR((nServerGroupID >= 0)&&(nServerGroupID < MAX_SERVER_GROUP_COUNT));
	LOG_PROCESS_ERROR(IsValidGroup(nServerGroupID));

	nGuildID = pGuild->GetID();

	nRetCode = UpdateRankNode(nServerGroupID, rlGuild_Level, nGuildID, pGuild->GetLevel());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlGuild_Money, nGuildID, pGuild->GetMoney());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlGuild_Resource_Herb, nGuildID, pGuild->GetHerb());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlGuild_Resource_Wood, nGuildID, pGuild->GetWood());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlGuild_Resource_Crystal, nGuildID, pGuild->GetCryStal());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlGuild_Resource_Silver, nGuildID, pGuild->GetSilver());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlGuild_Pow, nGuildID, pGuild->GetPow());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CRankListMgr::UpdateRoleRank(UINT32 dwClientSessionID, UINT32 dwRoleID, INT32 nLevel, INT32 nArmerPower, INT32 nSpiritPower, 
                                   INT32 nTotalPower, INT32 nMoney, INT32 nAchievement, INT32 nMaster, INT32 nPkValue, INT32 nTotalKillCount, 
                                   INT32 nClass, INT32 nExp, INT32 nItemPower)
{
	INT32 nRetCode = 0;
	INT32 nServerGroupID = 0;

	nServerGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwClientSessionID);
	LOG_PROCESS_ERROR((nServerGroupID >= 0)&&(nServerGroupID < MAX_SERVER_GROUP_COUNT));
	LOG_PROCESS_ERROR(IsValidGroup(nServerGroupID));

	nRetCode = UpdateRankNode(nServerGroupID, rlLevel, dwRoleID, nLevel);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlPower, dwRoleID, nTotalPower);
	LOG_PROCESS_ERROR(nRetCode);

	switch (nClass)
	{
	case clsWarrior:
		nRetCode = UpdateRankNode(nServerGroupID, rlLevelWarrior, dwRoleID, nLevel);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = UpdateRankNode(nServerGroupID, rlPowerWarrior, dwRoleID, nTotalPower);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	case clsMage:
		nRetCode = UpdateRankNode(nServerGroupID, rlLevelMage, dwRoleID, nLevel);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = UpdateRankNode(nServerGroupID, rlPowerMage, dwRoleID, nTotalPower);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	case clsRogue:
		nRetCode = UpdateRankNode(nServerGroupID, rlLevelRogue, dwRoleID, nLevel);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = UpdateRankNode(nServerGroupID, rlPowerRogue, dwRoleID, nTotalPower);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	case clsArcher:
		nRetCode = UpdateRankNode(nServerGroupID, rlLevelArcher, dwRoleID, nLevel);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = UpdateRankNode(nServerGroupID, rlPowerArcher, dwRoleID, nTotalPower);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	case clsTaoist:
		nRetCode = UpdateRankNode(nServerGroupID, rlLevelTaoist, dwRoleID, nLevel);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = UpdateRankNode(nServerGroupID, rlPowerTaoist, dwRoleID, nTotalPower);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	case clsSpear:		// No per-class rank lists for the 1.05 classes (only the overall level/power lists above)
	case clsSorcerer:
		break;
	default:
		LOG_CHECK_ERROR(FALSE);
	}

	nRetCode = UpdateRankNode(nServerGroupID, rlMoney, dwRoleID, nMoney);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlPkValue, dwRoleID, nPkValue);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlMaster, dwRoleID, nMaster);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlAchievement, dwRoleID, nAchievement);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateRankNode(nServerGroupID, rlTotalKillCount, dwRoleID, nTotalKillCount);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRankListMgr::UpdateLastRank(INT32 nServerGroupID, INT32 nRankListType)
{
	INT32 nRetCode = 0;

	for (Iter_MapKey2RankNode it = m_Map_Key_2_Rank[nServerGroupID][nRankListType].begin();
		it != m_Map_Key_2_Rank[nServerGroupID][nRankListType].end(); ++it)
	{
		RANK_NODE* pRankNode = it->second;
		if (pRankNode == NULL)
			continue;

		if (pRankNode->nLastRank == pRankNode->nSelfRank)
			continue;

		pRankNode->nLastRank = pRankNode->nSelfRank;

		nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(
			msqlUpdateLastRank, NULL, NULL, "ducdl", pRankNode->nLastRank, nServerGroupID, nRankListType, pRankNode->llKey);
		LOG_CHECK_ERROR(nRetCode);
	}

	INF("update last rank server %d type %d", nServerGroupID, nRankListType);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRankListMgr::_Load(std::list<void*>& rResult, void* pUserData)
{
	INT32 nRetCode = 0;
#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8   byServerGroupID;
        INT32   nRankListType;
        INT64   llKey;
        INT64   llValue;
		INT32	nLastRank;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

		if (!IsValidGroup(pLoadResult->byServerGroupID))
		{
			CRI("detected rank list data in invalid group %d", pLoadResult->byServerGroupID);
			return FALSE;
		}

        if (g_ServerConfig.GroupConfig[pLoadResult->byServerGroupID].nMappedGroupID > 0 &&
            g_ServerConfig.GroupConfig[pLoadResult->byServerGroupID].nMappedGroupID < MAX_SERVER_GROUP_COUNT)
		{
            pLoadResult->byServerGroupID = g_ServerConfig.GroupConfig[pLoadResult->byServerGroupID].nMappedGroupID;
		}

        nRetCode = CRankListMgr::ms_Instance.UpdateRankNode(pLoadResult->byServerGroupID,
            pLoadResult->nRankListType, pLoadResult->llKey, pLoadResult->llValue);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CRankListMgr::ms_Instance._SetLastRank(pLoadResult->byServerGroupID,
			pLoadResult->nRankListType, pLoadResult->llKey, pLoadResult->nLastRank);
		LOG_CHECK_ERROR(nRetCode);

        CRankListMgr::ms_Instance.m_nLoadedCount++;
    }

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CRankListMgr::UpdateRankNode(INT32 nServerGroupID,INT32 nRankListType, INT64 llKey, INT64 llValue)
{
	INT32 nRetCode = 0;
	RANK_NODE* pRankNode = NULL;
    RANK_NODE* pInsertPos = NULL;
    Iter_MapKey2RankNode it;
    std::pair<Iter_MapKey2RankNode, bool> InsRet;
    INT32 nThreadIndex = (nRankListType + llKey) % CManageServer::GetInstance().m_dwThreadCount;

    LOG_PROCESS_ERROR((nServerGroupID >= 0) && (nServerGroupID < MAX_SERVER_GROUP_COUNT));
	LOG_PROCESS_ERROR(IsValidGroup(nServerGroupID));
	LOG_PROCESS_ERROR((nRankListType > rlInvalid) && (nRankListType < rlTotal));

    it = m_Map_Key_2_Rank[nServerGroupID][nRankListType].find(llKey);	
    if (it != m_Map_Key_2_Rank[nServerGroupID][nRankListType].end())
        pRankNode = it->second;

	if (pRankNode == NULL)
	{
        if (!m_RankList[nServerGroupID][nRankListType].IsEmpty())
        {
            RANK_NODE* pRearRankNode = NULL;

            pRearRankNode = (RANK_NODE*)m_RankList[nServerGroupID][nRankListType].Rear().pPrev;
			if (pRearRankNode && pRearRankNode->nSelfRank >= MAX_RANK_LIST_NODE_COUNT)
		    {
				LOG_CHECK_ERROR(pRearRankNode->nSelfRank == m_Map_Key_2_Rank[nServerGroupID][nRankListType].size());
				
				if (llValue <= pRearRankNode->llValue && m_bLoading)
				{
					nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(
						msqlDeleteRankList, NULL, NULL, "ucdl", nServerGroupID, nRankListType, llKey);
					LOG_CHECK_ERROR(nRetCode);
					CRI("deleted small rank list node %d %d %I64d %I64d", nServerGroupID, nRankListType, llKey, llValue);
				}
				PROCESS_SUCCESS(llValue <= pRearRankNode->llValue);

				nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(
					msqlDeleteRankList, NULL, NULL, "ucdl", nServerGroupID, nRankListType, pRearRankNode->llKey);
				LOG_CHECK_ERROR(nRetCode);
				if (m_bLoading)
					CRI("deleted large rank list node %d %d %I64d %I64d", 
					nServerGroupID, nRankListType, pRearRankNode->llKey, pRearRankNode->llValue);

				nRetCode = CBaseList::Remove(pRearRankNode);
                LOG_CHECK_ERROR(nRetCode);

                m_Map_Key_2_Rank[nServerGroupID][nRankListType].erase(pRearRankNode->llKey);
                
                nRetCode = m_RankPool.Delete(pRearRankNode);
                LOG_CHECK_ERROR(nRetCode);
            }
        }

        pRankNode = m_RankPool.New();
        LOG_PROCESS_ERROR(pRankNode);

        pRankNode->llKey = llKey;
        pRankNode->llValue = llValue;
        pRankNode->nSelfRank = (INT32)m_Map_Key_2_Rank[nServerGroupID][nRankListType].size() + 1;
		pRankNode->nLastRank = 0;
        pRankNode->pPrev = pRankNode->pNext = NULL;

        InsRet = m_Map_Key_2_Rank[nServerGroupID][nRankListType].insert(std::make_pair(llKey, pRankNode));
        LOG_PROCESS_ERROR(InsRet.second);

        nRetCode = m_RankList[nServerGroupID][nRankListType].PushRear(pRankNode);
        LOG_PROCESS_ERROR(nRetCode);

        if (!m_bLoading)
        {
            nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(
				msqlInsertRankList, NULL, NULL, "ucdll", nServerGroupID, nRankListType, llKey, llValue);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
	else
	{
        PROCESS_SUCCESS(pRankNode->llValue == llValue);
		pRankNode->llValue = llValue;

        if (!m_bLoading)
        {
            nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlUpdateRankList, NULL, NULL, "lucdl", llValue, nServerGroupID, nRankListType, llKey);
            LOG_CHECK_ERROR(nRetCode);
        }
	}

    pInsertPos = (RANK_NODE*)pRankNode->pPrev;
    while (pInsertPos != &m_RankList[nServerGroupID][nRankListType].Head() &&
        pRankNode->llValue > pInsertPos->llValue)
    {
        pInsertPos->nSelfRank++;
        pRankNode->nSelfRank--;
        pInsertPos = (RANK_NODE*)pInsertPos->pPrev;
    }

    if (pInsertPos != pRankNode->pPrev)
    {
        nRetCode = CBaseList::Remove(pRankNode);
		LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_RankList[nServerGroupID][nRankListType].InsertAfter(pRankNode, pInsertPos);
		LOG_CHECK_ERROR(nRetCode);
    }
    else
    {
        pInsertPos = (RANK_NODE*)pRankNode->pNext;
        while (pInsertPos != &m_RankList[nServerGroupID][nRankListType].Rear() &&
            pRankNode->llValue < pInsertPos->llValue)
        {
            pInsertPos->nSelfRank--;
            pRankNode->nSelfRank++;
            pInsertPos = (RANK_NODE*)pInsertPos->pNext;
        }
        
        if (pInsertPos != pRankNode->pNext)
        {
            nRetCode = CBaseList::Remove(pRankNode);
			LOG_CHECK_ERROR(nRetCode);

            nRetCode = m_RankList[nServerGroupID][nRankListType].InsertBefore(pRankNode, pInsertPos);
			LOG_CHECK_ERROR(nRetCode);
        }
    }

Exit1:
	return TRUE;

Exit0:
	return FALSE;
}

BOOL CRankListMgr::_SetLastRank(INT32 nServerGroupID, INT32 nRankListType, INT64 llKey, INT32 nLastRank)
{
	INT32 nRetCode = 0;
	RANK_NODE* pRankNode = NULL;
	Iter_MapKey2RankNode it;

	LOG_PROCESS_ERROR((nServerGroupID >= 0) && (nServerGroupID < MAX_SERVER_GROUP_COUNT));
	LOG_PROCESS_ERROR(IsValidGroup(nServerGroupID));
	LOG_PROCESS_ERROR((nRankListType > rlInvalid) && (nRankListType < rlTotal));

	it = m_Map_Key_2_Rank[nServerGroupID][nRankListType].find(llKey);
	LOG_PROCESS_ERROR(it != m_Map_Key_2_Rank[nServerGroupID][nRankListType].end());

	pRankNode = it->second;
	LOG_PROCESS_ERROR(pRankNode);

	pRankNode->nLastRank = nLastRank;

	return TRUE;
Exit0:
	return FALSE;
}

void CRankListMgr::AddRankAchievementID(INT32 nRankAchID)
{
	m_AchievementList.insert(nRankAchID);
}

void CRankListMgr::DelRankAchievementID(INT32 nRankAchID)
{
	m_AchievementList.erase(nRankAchID);
}

void CRankListMgr::BeginRankAchievementID(void)
{
	m_AchItr = m_AchievementList.begin();
}

INT32 CRankListMgr::NextRankAchievementID(void)
{
	INT32 nID = 0;
	PROCESS_ERROR(m_AchItr != m_AchievementList.end());

	nID = *m_AchItr;
	++m_AchItr;

	return nID;
Exit0:
	return 0;
}

BOOL CRankListMgr::UpdateAverageLevel(void)
{
	INT32 nRetCode = 0;

	for (INT32 nServerGroupID = 0; nServerGroupID < MAX_SERVER_GROUP_COUNT; nServerGroupID++)
	{
		INT64 llTotalLevel = 0;
		INT32 nCount = 0;

		if (!IsValidGroup(nServerGroupID))
			continue;

		RANK_NODE* pRankNode = (RANK_NODE*)&m_RankList[nServerGroupID][rlLevel].Head();
		for (nCount = 0;
			nCount < AVERAGE_LEVEL_COUNT && pRankNode != (RANK_NODE*)&m_RankList[nServerGroupID][rlLevel].Rear();
			nCount++)
		{
			llTotalLevel += pRankNode->llValue;
			pRankNode = (RANK_NODE*)pRankNode->pNext;
		}

		m_nAverageLevel[nServerGroupID] = llTotalLevel / nCount;
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRankListMgr::DelGuildRankNode(INT32 nServerGroupID, UINT32 dwGuildID)
{
    return DelRankNode(nServerGroupID, dwGuildID);
}
