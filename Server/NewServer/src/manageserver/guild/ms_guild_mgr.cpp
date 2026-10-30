#include "stdafx.h"
#include "ms_guild_mgr.h"

#include "guild_config.h"
#include "server_config.h"
#include "error_def.h"
#include "time_ex.h"

#include "role_data_mgr.h"
#include "message_handler.h"
#include "onlinerole/online_role_data_mgr.h"
#include "mail/mail_mgr.h"
#include "../gameworld/item.h"
#include "manageserver.h"
#include "application_mgr.h"
#include "db/prep_stmt.h"
#include "game_world_config.h"

#include <time.h>
#include "db/ms_sql_stmt_list.h"
#include "sql_mgr.h"
#include "script_mgr.h"

using namespace std;

CMsGuildMgr CMsGuildMgr::ms_Instance_;

CMsGuildMgr::CMsGuildMgr()
{
    memset(m_pRoot, 0, sizeof(m_pRoot));
}

CMsGuildMgr::~CMsGuildMgr()
{
    memset(m_pRoot, 0, sizeof(m_pRoot));
}

CMsGuildMgr & CMsGuildMgr::GetInstance()
{
	return ms_Instance_;
}

inline UINT16 CMsGuildMgr::_GetHashValue(GUILD_NAME_KEY& rRoleNameKey)
{
	//UINT16 wHashKey;
	//UINT32 dwTempValue;
	//UINT64 qwTempValue;

	//qwTempValue = (rRoleNameKey.qwKey0 ^ rRoleNameKey.qwKey1);
	//dwTempValue = (((UINT32*)&qwTempValue)[0] << 5) ^ ((UINT32*)&qwTempValue)[1];
	//wHashKey = (((UINT16*)&dwTempValue)[0] << 5) ^ ((UINT16*)&dwTempValue)[1];

	//return wHashKey;

    UINT16 wHashKey;
    UINT32 dwTempValue;
    UINT64 qwTempValue;

    qwTempValue = (rRoleNameKey.qwKey0 ^ rRoleNameKey.qwKey1 ^ rRoleNameKey.qwKey2);
    dwTempValue = (((UINT32*)&qwTempValue)[0] << 5) ^ ((UINT32*)&qwTempValue)[1];
    wHashKey = (((UINT16*)&dwTempValue)[0] << 5) ^ ((UINT16*)&dwTempValue)[1];

    return wHashKey;
}

bool CMsGuildMgr::CDeleteAllGuild::operator()(UINT32 dwGuildID, CMsGuild* poMsGuild)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poMsGuild);

	nRetCode = poMsGuild->UnInit();
	LOG_CHECK_ERROR(nRetCode);

Exit0:
	return TRUE;
}

BOOL CMsGuildMgr::_SortLadder()
{
    BASE_LIST_NODE  *pNode = NULL;
    BASE_LIST_NODE  *pTempNode = NULL;
    BASE_LIST_NODE  *pSortedNode = NULL;
    CMsGuild        *poMsGuild = NULL;
    CMsGuild        *poTempMsGuild = NULL;
    CMsGuild        *poSortedMsGuild = NULL;

    // first sort  prosperity
    pSortedNode = &m_Ladder.Head();
    while (pSortedNode != &m_Ladder.Rear())
    {
        pNode = pSortedNode->pNext;

        while (pNode != &m_Ladder.Rear())
        {
            if (!pTempNode)
            {
                pTempNode = pNode;
                poTempMsGuild = node_cast(CMsGuild*, GetLadderNode(), pTempNode);
            }
            else
            {
                poMsGuild = node_cast(CMsGuild*, GetLadderNode(), pNode);

                if (poMsGuild->GetProsperity() > poTempMsGuild->GetProsperity())
                {
                    pTempNode = pNode;
                    poTempMsGuild = node_cast(CMsGuild*, GetLadderNode(), pTempNode);
                }
            }
            pNode = pNode->pNext;
        }

        if (pTempNode)
        {
            m_Ladder.Remove(pTempNode);
            m_Ladder.InsertAfter(pTempNode, pSortedNode);
            pSortedNode = pTempNode;
        }
        else
            break;
        pTempNode = NULL;
    }

    // second sort  create time
    pSortedNode = m_Ladder.Head().pNext;
    while (pSortedNode != &m_Ladder.Rear())
    {
        pNode = pSortedNode->pNext;

        if (pNode != &m_Ladder.Rear())
        {
            poSortedMsGuild = node_cast(CMsGuild*, GetLadderNode(), pSortedNode);
            poMsGuild = node_cast(CMsGuild*, GetLadderNode(), pNode);

            if (poSortedMsGuild->GetProsperity() == poMsGuild->GetProsperity())
            {
                if (poSortedMsGuild->GetCreateTime() > poMsGuild->GetCreateTime())
                {
                    pTempNode = pSortedNode->pPrev;
                    while (pTempNode != &m_Ladder.Head())
                    {
                        poTempMsGuild = node_cast(CMsGuild*, GetLadderNode(), pTempNode);
                        if (poMsGuild->GetProsperity() == poTempMsGuild->GetProsperity())
                        {
                            if (poMsGuild->GetCreateTime() > poTempMsGuild->GetCreateTime())
                            {
                                m_Ladder.Remove(pNode);
                                m_Ladder.InsertAfter(pNode, pTempNode);
                                break;
                            }

                            pTempNode = pTempNode->pPrev;
                        }
                        else
                        {
                            m_Ladder.Remove(pNode);
                            m_Ladder.InsertAfter(pNode, pTempNode);
                            break;
                        }
                    }

                    if (pTempNode == &m_Ladder.Head())
                    {
                        m_Ladder.Remove(pNode);
                        m_Ladder.PushHead(pNode);
                    }

                }
            }
        }
        pSortedNode = pSortedNode->pNext;
    }

    return TRUE;
}

BOOL InsertToTNDList(GUILD_RESOURCE_STAT *pList, INT32 nListNum, UINT32 dwGuildID, UINT32 dwValue)
{
    GUILD_RESOURCE_STAT        node;
    INT32       i = 0;

    LOG_PROCESS_ERROR(pList);
    LOG_PROCESS_ERROR(nListNum < GUILD_STAT_TOP_NUM);

    node.dwGuildID = dwGuildID;
    node.dwValue = dwValue;

    for ( ; i < nListNum; ++i)
    {
        if (pList[i].dwValue < dwValue)
        {
            node.dwGuildID = pList[i].dwGuildID;
            node.dwValue = pList[i].dwValue;

            pList[i].dwGuildID = dwGuildID;
            pList[i].dwValue = dwValue;

            dwGuildID = node.dwGuildID;
            dwValue = node.dwValue;
        }
    }

    pList[i].dwGuildID = dwGuildID;
    pList[i].dwValue = dwValue;

    return TRUE;
Exit0:
    return FALSE;
}

void InsertToFullTNDList(GUILD_RESOURCE_STAT *pList, UINT32 dwGuildID, UINT32 dwValue)
{
    GUILD_RESOURCE_STAT        node;

    LOG_PROCESS_ERROR(pList);
    PROCESS_ERROR(pList[MAX_GUILD_STAT_TOP_INDEX].dwValue < dwValue);

    for (INT32 i = 0; i < GUILD_STAT_TOP_NUM; ++i)
    {
        if (pList[i].dwValue < dwValue)
        {
            node.dwGuildID = pList[i].dwGuildID;
            node.dwValue = pList[i].dwValue;

            pList[i].dwGuildID = dwGuildID;
            pList[i].dwValue = dwValue;

            dwGuildID = node.dwGuildID;
            dwValue = node.dwValue;
        }
    }

Exit0:
    return ;
}

BOOL CMsGuildMgr::_Stat()
{
    INT32           nRetCode = 0;
    BASE_LIST_NODE  *pNode = NULL;
    CMsGuild        *poMsGuild = NULL;

    GUILD_STAT      stat[MAX_SERVER_GROUP_COUNT];

    memset(stat, 0, sizeof(stat));

    // first sort
    pNode = m_GuildLvlRank.Head().pNext;
    while (pNode != &m_GuildLvlRank.Rear())
    {
        poMsGuild = node_cast(CMsGuild*, GetLvlRankNode(), pNode);

        if (stat[poMsGuild->GetSvrGroup()].nGuildNum < GUILD_STAT_TOP_NUM)
        {
            InsertToTNDList(stat[poMsGuild->GetSvrGroup()].MoneyList, stat[poMsGuild->GetSvrGroup()].nGuildNum, poMsGuild->GetID(), poMsGuild->GetMoney());
            InsertToTNDList(stat[poMsGuild->GetSvrGroup()].WoodList, stat[poMsGuild->GetSvrGroup()].nGuildNum, poMsGuild->GetID(), poMsGuild->GetWood());
            InsertToTNDList(stat[poMsGuild->GetSvrGroup()].RiceList, stat[poMsGuild->GetSvrGroup()].nGuildNum, poMsGuild->GetID(), poMsGuild->GetHerb());
            InsertToTNDList(stat[poMsGuild->GetSvrGroup()].StoneList, stat[poMsGuild->GetSvrGroup()].nGuildNum, poMsGuild->GetID(), poMsGuild->GetCryStal());
            InsertToTNDList(stat[poMsGuild->GetSvrGroup()].MetalList, stat[poMsGuild->GetSvrGroup()].nGuildNum, poMsGuild->GetID(), poMsGuild->GetSilver());
        }
        else
        {
            InsertToFullTNDList(stat[poMsGuild->GetSvrGroup()].MoneyList, poMsGuild->GetID(), poMsGuild->GetMoney());
            InsertToFullTNDList(stat[poMsGuild->GetSvrGroup()].WoodList, poMsGuild->GetID(), poMsGuild->GetWood());
            InsertToFullTNDList(stat[poMsGuild->GetSvrGroup()].RiceList, poMsGuild->GetID(), poMsGuild->GetHerb());
            InsertToFullTNDList(stat[poMsGuild->GetSvrGroup()].StoneList, poMsGuild->GetID(), poMsGuild->GetCryStal());
            InsertToFullTNDList(stat[poMsGuild->GetSvrGroup()].MetalList, poMsGuild->GetID(), poMsGuild->GetSilver());
        }

        for (INT32 i = GUILD_B_MAIN; i < GUILD_B_NUM; ++i)
        {
            if (poMsGuild->GetBuildingMaxLevelByType(i) <= GUILD_MAX_BUILDING_LEVEL)
                ++stat[poMsGuild->GetSvrGroup()].BuildingStat[i][poMsGuild->GetBuildingMaxLevelByType(i)];
            else
                LOG_CHECK_ERROR(FALSE);           
        }

        ++stat[poMsGuild->GetSvrGroup()].nGuildNum;
        pNode = pNode->pNext;
    }

    nRetCode = CMessageHandler::GetInstance().DoDLGuildStat(CManageServer::GetInstance().GetGameTime(), stat);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CMsGuildMgr::Init()
{
	INT32   nRetCode = 0;
    UINT32  dwCurrentTime = CManageServer::GetInstance().GetGameTime();
    UINT32  dwDayTime = dwCurrentTime + get_time_zone();
    dwDayTime /= ONE_DAY_SECOND;
    dwDayTime *= ONE_DAY_SECOND;
    dwDayTime -= get_time_zone();

    const   UINT32 dwMinGuildID = static_cast<UINT32>(otGuild) << 28;

    //////////////// check config: guild member count /////////////////////////
    INT32   nMaxGuildNum = 0;
    for (INT32 i = GUILD_POSITION_LEADER - 1; i < GUILD_POSITION_TOTAL; i++)
    {
        nMaxGuildNum += ARR_GUILD_POS_HUMAN_NUM[i];
    }
    LOG_PROCESS_ERROR(nMaxGuildNum == GUILD_MAX_MEMBER_COUNT);
    //////////////////////////////////////////////////////////////////////////

    m_GuildLvlRank.Clear();
    m_Ladder.Clear();
    m_WarList.Clear();
    m_GSSyncList.clear();

    memset(&m_pRoot, 0, sizeof(m_pRoot));

	nRetCode = m_GuildPool.Init(g_GuildConfig.nInitGuildCount);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_GuildAppPool.Init(g_GuildConfig.nInitGuildAppPoolCount);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_DiplomatAppPool.Init(g_GuildConfig.nInitDiplomatAppPoolCount);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_GuildEventDataPool.Init(g_GuildConfig.nInitHistoryEventPoolCount);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_UpgradeTriggerPool.Init(g_GuildConfig.nGuildUpgradeTriggerPool);
    LOG_PROCESS_ERROR(nRetCode);

    // todo_tq: guild war pool count
    nRetCode = m_GuildWarPool.Init(g_GuildConfig.nGuildGuildWarPool);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_StorageEventPool.Init(g_GuildConfig.nInitStorageEventPoolCount);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_GBRevivePool.Init(g_GuildConfig.nGuildBuildingRevivePool);
    LOG_PROCESS_ERROR(nRetCode);

    //nRetCode = _Load();
    //   LOG_PROCESS_ERROR(nRetCode);

    {
        // clear destroy guild before  **-**-**
        UINT32  dwTempTime = dwDayTime - g_GWConfig.nWashoutDeletedGuildTime;

        nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlClearGuild, NULL, NULL, "ud", dwTempTime);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlClearGMember, NULL, NULL, "ud", dwTempTime);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectGuild, ::LoadGuild, NULL, "");
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectGMember, ::LoadGMember, NULL, "");
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _AfterLoadGuild();
    LOG_PROCESS_ERROR(nRetCode);

    // guild war
    {
        UINT32  dwYesTime = dwDayTime;

        if ((dwCurrentTime - dwDayTime) < (UINT32)g_GuildConfig.nGuildWarEndTime)
            dwYesTime -= ONE_DAY_SECOND;
        nRetCode = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectGuildWar, ::LoadGuildWar, NULL, "ud", dwYesTime);
        LOG_PROCESS_ERROR(nRetCode);

        // trigger
        BASE_TRIGGER_NODE   *pNode = new BASE_TRIGGER_NODE;
        LOG_PROCESS_ERROR(pNode);

        pNode->nTriggerType = tttInvalid;
        if ((dwCurrentTime - dwDayTime) > (UINT32)g_GuildConfig.nGuildWarScriptBeginTime)
            dwDayTime += ONE_DAY_SECOND;

        pNode->dwWashoutTime = dwDayTime + g_GuildConfig.nGuildWarScriptBeginTime;
        pNode->fp = CB_GuildWarBegin;
        nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pNode);
        LOG_PROCESS_ERROR(nRetCode);
    }
    
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMsGuildMgr::Uninit()
{
	// release
	INT32 nRetCode = 0;
	CDeleteAllGuild     deleteAllGuild;

	nRetCode = m_GuildPool.Traverse(deleteAllGuild);
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_GuildPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_GuildAppPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_DiplomatAppPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_GuildEventDataPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_UpgradeTriggerPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_GuildWarPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_StorageEventPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_GBRevivePool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    m_GuildLvlRank.Clear();
    m_Ladder.Clear();
    m_WarList.Clear();
    memset(&m_pRoot, 0, sizeof(m_pRoot));

    if (m_GSSyncList.size() > 0)
    {
        CRI("sync gs data list hava [%d] data", m_GSSyncList.size());
        LOG_CHECK_ERROR(nRetCode);
    }
    m_GSSyncList.clear();

	return TRUE;
}

BOOL CMsGuildMgr::LoadGuild(GUILD_DB& db_data)
{
    INT32       nRetCode = 0;
    UINT16      wHashKey = 0;
    CMsGuild    *poMsGuild = NULL;

    // leader check
    MS_ROLE_DATA    *pLeaderRoleData = CRoleDataMgr::GetInstance().FindRole(db_data.dwLeaderID);
    if (!pLeaderRoleData)
    {
        WRN("Wrong Leader Role Data! %d", db_data.dwLeaderID);
        return FALSE;
    }

    poMsGuild = m_GuildPool.New(db_data.dwGuildID);
    LOG_PROCESS_ERROR(poMsGuild);
    LOG_PROCESS_ERROR(poMsGuild->Load(db_data));

    // other construct
    // Build a sorted guild list
    nRetCode = InsertGuildIntoLvlRank(poMsGuild);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = InsertGuildIntoLadder(poMsGuild);
    LOG_PROCESS_ERROR(nRetCode);

    wHashKey = _GetHashValue(poMsGuild->GetNameMapNode().ID);
    nRetCode = LM_Insert(&(poMsGuild->GetNameMapNode()), &(m_pRoot[wHashKey]));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMgr::_AfterLoadGuild()
{
    INT32       nRetCode = 0;
    CMsGuild    *poMsGuild = GetFirstGuildOnLvlRank();
    while (poMsGuild)
    {
        nRetCode = poMsGuild->InitProsperity();
        LOG_PROCESS_ERROR(nRetCode);

        poMsGuild = GetNextGuildOnLvlRank(poMsGuild);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMgr::_SqlInsertGuild(CMsGuild *poMsGuild)
{
    INT32       nRetCode = 0;
    char        *pBuffer = NULL;
    UINT32      dwSize = 0;

    LOG_PROCESS_ERROR(poMsGuild);

    pBuffer = new char[64 * 1024];
    LOG_PROCESS_ERROR(pBuffer);

    nRetCode = poMsGuild->SaveGuild(pBuffer, dwSize);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlInsertGuild, NULL, NULL, "uduhsucudssz", 
            poMsGuild->GetID(), 0, poMsGuild->GetName(), (UINT8)poMsGuild->GetSvrGroup(),
            poMsGuild->GetLeaderID(), poMsGuild->GetCreaterName(), poMsGuild->GetAnnounce(), 
            pBuffer, dwSize);
    LOG_PROCESS_ERROR(nRetCode);

    delete[] pBuffer;
    return TRUE;
Exit0:
    if (pBuffer)
        delete[] pBuffer;
    return FALSE;
}

BOOL CMsGuildMgr::MainLoop()
{
	INT32 nRetCode = 0;
    INT32   nSendCount = 0;
    //UINT64  qwTickCount = 0;
	UINT32 dwCurrentTime = CManageServer::GetInstance().GetGameTime();

    //qwTickCount = CManageServer::GetInstance().GetGameTick();
    while (m_GSSyncList.size() > 0)
    {
        SYNC_GS_GUILD_DATA  rData = m_GSSyncList.front();

        CMsGuild    *poMSGuild = CMsGuildMgr::GetInstance().FindGuildByID(rData.dwGuildID);
        LOG_CHECK_ERROR(poMSGuild);
        if (poMSGuild)
        {
            nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildDataNtf(rData.dwGSSessionID, poMSGuild);
            LOG_CHECK_ERROR(nRetCode);
        }

        ++nSendCount;
        m_GSSyncList.pop_front();
        if (nSendCount == 512)
            break;
    }
    //qwTickCount = CManageServer::GetInstance().GetGameTick() - qwTickCount;
    //DBG("MS Guild sync 512 data to gs use [%llu]", qwTickCount);

	// guild timer
    CMsGuild    *poMsGuild = GetFirstGuildOnLvlRank();
	while (poMsGuild)
	{
		nRetCode = poMsGuild->MainLoop(dwCurrentTime);
		LOG_CHECK_ERROR(nRetCode);

        poMsGuild = GetNextGuildOnLvlRank(poMsGuild);
	}

	// global timer

	// 30-second timer
	if ( 0 == dwCurrentTime % 30 )
	{

		// 1-minute timer
		if ( 0 == dwCurrentTime % 60)
		{
            // 10-minute timer. Allan Tao [8.5.2013]
			if ( 0 == dwCurrentTime % 600 )
			{
                // one hour
                if ( 0 == dwCurrentTime % 3600)
                {
                    // one day
                    if (0 == (dwCurrentTime - 10800) % 86400)
                    {
                        _Stat();
                    }
                }
			}
		}
	}

	return TRUE;
}

BOOL CMsGuildMgr::InsertGuildIntoLvlRank(CMsGuild *poMsGuild)
{
    CMsGuild        *pGuildIter = NULL;
    BASE_LIST_NODE  *pNodeIter = NULL;

    LOG_PROCESS_ERROR(poMsGuild);
    ClearNode(&poMsGuild->GetLvlRankNode());
    for (pNodeIter = m_GuildLvlRank.Rear().pPrev; pNodeIter && pNodeIter != &(m_GuildLvlRank.Head()); pNodeIter = pNodeIter->pPrev)
    {
        pGuildIter = (CMsGuild *)((char*)pNodeIter - (char*)(&((CMsGuild*)0)->GetLvlRankNode()));

        if (pGuildIter->GetLevel() >= poMsGuild->GetLevel())
        {
            m_GuildLvlRank.InsertAfter(&poMsGuild->GetLvlRankNode(), pNodeIter);
            return TRUE;
        }
    }

    m_GuildLvlRank.PushHead(&poMsGuild->GetLvlRankNode());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMgr::RemoveGuildFromLvlRank(CMsGuild *poMsGuild)
{
    BASE_LIST_NODE  *pNodeIter = NULL;

    LOG_PROCESS_ERROR(poMsGuild);
    for (pNodeIter = m_GuildLvlRank.Rear().pPrev; pNodeIter && pNodeIter != &(m_GuildLvlRank.Head()); pNodeIter = pNodeIter->pPrev)
    {
        if (pNodeIter = &poMsGuild->GetLvlRankNode())
        {
            m_GuildLvlRank.Remove(pNodeIter);
            ClearNode(&poMsGuild->GetLvlRankNode());
            return TRUE;
        }
    }

    ClearNode(&poMsGuild->GetLvlRankNode());
    return TRUE;
Exit0:
    return FALSE;
}

// diplomat
BOOL CMsGuildMgr::DelAlliance(INT32 dwGuildID, INT32 dwAllianceID)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild    *poGuild = NULL;
    CMsGuild    *poAlliance = NULL;

    poGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwGuildID);
    LOG_CHECK_ERROR(poGuild);
    poAlliance = CMsGuildMgr::GetInstance().FindGuildByID(dwAllianceID);
    LOG_CHECK_ERROR(poAlliance);

    if (poGuild)
    {
        nRetCode = poGuild->DelAlliance(dwAllianceID);
        LOG_CHECK_ERROR(nRetCode);
    }
    
    if (poAlliance)
    {
        nRetCode = poAlliance->DelAlliance(poGuild->GetID());
        LOG_CHECK_ERROR(nRetCode);
    }

    // to gs
    nRetCode = CMessageHandler::GetInstance().DoGSGuildDelDiplomat(GUILD_DIPLOMAT_ALLIANCE, dwGuildID, dwAllianceID);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CMsGuildMgr::DelEnemy(INT32 dwGuildID, INT32 dwEnemyID)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild    *poGuild = NULL;
    CMsGuild    *poEnemy = NULL;

    poGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwGuildID);
    LOG_CHECK_ERROR(poGuild);
    poEnemy = CMsGuildMgr::GetInstance().FindGuildByID(dwEnemyID);
    LOG_CHECK_ERROR(poEnemy);

    if (poGuild)
        poGuild->DelEnemy(dwEnemyID);

    if (poEnemy)
        poEnemy->DelEnemy(dwGuildID);

    // to gs
    nRetCode = CMessageHandler::GetInstance().DoGSGuildDelDiplomat(GUILD_DIPLOMAT_ENEMY, dwGuildID, dwEnemyID);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CMsGuildMgr::AddWarData(INT32 nGroupID, UINT32 dwIntruderID, UINT32 dwDefenderID, UINT32 dwTime)
{
    INT32       nRetCode = 0;
    MS_GUILD_WAR_DATA   *pWarData = NULL;

    pWarData = m_GuildWarPool.New();
    LOG_PROCESS_ERROR(pWarData);

    ClearNode(pWarData);

    pWarData->dwBeginDayTime = dwTime;
    pWarData->dwIntruderID = dwIntruderID;
    pWarData->dwDefenderID = dwDefenderID;
    pWarData->nWarResult = GUILD_WAR_DEFENDER_NONE;

    m_WarList.PushRear(pWarData);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMgr::DeclareWar(INT32 nGroupID, UINT32 dwIntruderID, UINT32 dwDefenderID)
{
    INT32       nRetCode = 0;
    UINT32      dwCurTime = CManageServer::GetInstance().GetGameTime();
    dwCurTime += get_time_zone();
    dwCurTime /= 86400;
    dwCurTime++;
    dwCurTime *= 86400;
    dwCurTime -= get_time_zone();

    nRetCode = AddWarData(nGroupID, dwIntruderID, dwDefenderID, dwCurTime);
    LOG_PROCESS_ERROR(nRetCode);

    // sql
    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlInsertGuildWar, 
        NULL, NULL, "ucududud", (UINT8)nGroupID, dwIntruderID, dwDefenderID, dwCurTime);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMgr::WarBegin()
{
    INT32       nRetCode = 0;
    UINT32      dwCurTime = CManageServer::GetInstance().GetGameTime();
    CMsGuild    *poDefender = NULL;
    BASE_LIST_NODE   *pNode = NULL;

    pNode = (MS_GUILD_WAR_DATA*)m_WarList.Head().pNext;
    while (pNode != &m_WarList.Rear())
    {
        MS_GUILD_WAR_DATA   *pWarData = (MS_GUILD_WAR_DATA*)pNode;

        if (pWarData->dwBeginDayTime > dwCurTime)
        {
            pNode = pNode->pNext;
            continue;
        }

        poDefender = CMsGuildMgr::GetInstance().FindGuildByID(pWarData->dwDefenderID);
        if (poDefender == NULL)
        {
            LOG_CHECK_ERROR(FALSE);
            continue;
        }

        poDefender->AddIntruder(pWarData->dwIntruderID);

        pNode = pNode->pNext;
    }

    return TRUE;
}

BOOL CMsGuildMgr::WarEnd(UINT32 dwDefenderID, INT32 nWarDefenderResult)
{
    INT32       nRetCode = 0;
    CMsGuild    *poGuild = NULL;
    CLuaScript* poScript = NULL;

    poGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwDefenderID);
    LOG_PROCESS_ERROR(poGuild);

    nRetCode = poGuild->SetProtectionTime(CManageServer::GetInstance().GetGameTime() + g_GuildConfig.nGuildWarProtectionTime);
    LOG_CHECK_ERROR(nRetCode);

    poScript = CScriptMgr::GetInstance().FindScript(GUILD_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    nRetCode = poScript->CallFunction("GuildWarScore", "dd", dwDefenderID, nWarDefenderResult);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMgr::IsWarToday(UINT32 dwDefender)
{
    MS_GUILD_WAR_DATA  *pWar = NULL;
    UINT32      dwCurTime = CManageServer::GetInstance().GetGameTime();
    dwCurTime += get_time_zone();
    dwCurTime /= 86400;
    dwCurTime *= 86400;
    dwCurTime -= get_time_zone();

    pWar = FirstWarData();
    while (pWar != NULL)
    {
        if (pWar->dwDefenderID == dwDefender && pWar->dwBeginDayTime == dwCurTime)
        {
            return TRUE;
        }
        pWar = NextWarData();
    }

    return FALSE;
}


BOOL CMsGuildMgr::IsWarRelation(UINT32 dwGuildID1, UINT32 dwGuildID2)
{
    MS_GUILD_WAR_DATA  *pWar = NULL;

    pWar = FirstWarData();
    while (pWar != NULL)
    {
        if ((pWar->dwIntruderID == dwGuildID1 && pWar->dwDefenderID == dwGuildID2) 
            || (pWar->dwDefenderID == dwGuildID2 && pWar->dwDefenderID == dwGuildID1))
        {
            return TRUE;
        }
        pWar = NextWarData();
    }

    return FALSE;
}

BOOL CMsGuildMgr::ClearWarData()
{
    INT32       nRetCode = 0;
    UINT32      dwCurTime = CManageServer::GetInstance().GetGameTime();
    MS_GUILD_WAR_DATA   *pWar = NULL;
    BASE_LIST_NODE  *pNode = NULL;
    BASE_LIST_NODE  *pNextNode = NULL;

    pNode = m_WarList.Head().pNext;
    while (pNode != &(m_WarList.Rear()))
    {
        pWar = (MS_GUILD_WAR_DATA *)pNode;

        if (pWar->dwBeginDayTime < dwCurTime)
        {
            pNextNode = pNode->pNext;

            m_WarList.Remove(pNode);
            m_GuildWarPool.Delete(pWar);

            pNode = pNextNode;
        }
        else
            pNode = pNode->pNext;
    }

    return TRUE;
}

BOOL CMsGuildMgr::ClearOldWarData()
{
    INT32       nRetCode = 0;
    MS_GUILD_WAR_DATA   *pWar = NULL;
    BASE_LIST_NODE  *pNode = NULL;
    BASE_LIST_NODE  *pNextNode = NULL;

    UINT32      dwCurTime = CManageServer::GetInstance().GetGameTime();
    dwCurTime += get_time_zone();
    dwCurTime /= 86400;
    dwCurTime *= 86400;
    dwCurTime -= get_time_zone();

    pNode = m_WarList.Head().pNext;
    while (pNode != &(m_WarList.Rear()))
    {
        pWar = (MS_GUILD_WAR_DATA *)pNode;

        if (pWar->dwBeginDayTime < dwCurTime)
        {
            pNextNode = pNode->pNext;

            m_WarList.Remove(pNode);
            m_GuildWarPool.Delete(pWar);

            pNode = pNextNode;
        }
        else
            pNode = pNode->pNext;
    }

    return TRUE;
}

MS_GUILD_WAR_DATA* CMsGuildMgr::FirstWarData()
{
    m_pWarListCursor = m_WarList.Head().pNext;
    PROCESS_ERROR(m_pWarListCursor != &(m_WarList.Rear()));

    return (MS_GUILD_WAR_DATA*)m_pWarListCursor;
Exit0:
    return NULL;
}

MS_GUILD_WAR_DATA* CMsGuildMgr::NextWarData()
{
    m_pWarListCursor = m_pWarListCursor->pNext;
    if (m_pWarListCursor != &(m_WarList.Rear()))
    {
        return (MS_GUILD_WAR_DATA*)m_pWarListCursor;
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////

CMsGuild *CMsGuildMgr::GetFirstGuildOnLvlRank()
{
    if (m_GuildLvlRank.Head().pNext && m_GuildLvlRank.Head().pNext != &(m_GuildLvlRank.Rear()))
    {
        return (CMsGuild *)((char*)m_GuildLvlRank.Head().pNext - (char*)(&((CMsGuild*)0)->GetLvlRankNode()));
    }
    return NULL;
}

CMsGuild *CMsGuildMgr::GetNextGuildOnLvlRank(CMsGuild *poMsGuild)
{
    LOG_PROCESS_ERROR(poMsGuild);
    if (poMsGuild->GetLvlRankNode().pNext && poMsGuild->GetLvlRankNode().pNext != &m_GuildLvlRank.Rear())
    {
        return (CMsGuild *)((char*)poMsGuild->GetLvlRankNode().pNext - (char*)(&((CMsGuild*)0)->GetLvlRankNode()));
    }
Exit0:
	return NULL;
}

CMsGuild *CMsGuildMgr::GetPrevGuildOnLvlRank(CMsGuild *poMsGuild)
{
    LOG_PROCESS_ERROR(poMsGuild);
    if (poMsGuild->GetLvlRankNode().pPrev && poMsGuild->GetLvlRankNode().pPrev != &m_GuildLvlRank.Head())
    {
        return (CMsGuild *)((char*)poMsGuild->GetLvlRankNode().pPrev - (char*)(&((CMsGuild*)0)->GetLvlRankNode()));
    }
Exit0:
    return NULL;
}

APP_NODE *CMsGuildMgr::CreateGuildApp()
{
    return m_GuildAppPool.New();
}

BOOL CMsGuildMgr::DeleteGuildApp(APP_NODE *pNode)
{
    LOG_PROCESS_ERROR(pNode);
    LOG_PROCESS_ERROR(m_GuildAppPool.Delete(pNode));

    return TRUE;
Exit0:
    return FALSE;
}

DIPLOMAT_APP_NODE *CMsGuildMgr::CreateDiplomatApp()
{
    return m_DiplomatAppPool.New();
}

BOOL CMsGuildMgr::DeleteDiplomatApp(DIPLOMAT_APP_NODE *pNode)
{
    LOG_PROCESS_ERROR(pNode);
    LOG_PROCESS_ERROR(m_DiplomatAppPool.Delete(pNode));
    return TRUE;
Exit0:
    return FALSE;
}

EVENT_DATA *CMsGuildMgr::CreateEventData()
{
    return m_GuildEventDataPool.New();
}

BOOL CMsGuildMgr::DeleteEventData(EVENT_DATA *pData)
{
    LOG_PROCESS_ERROR(pData);
    LOG_PROCESS_ERROR(m_GuildEventDataPool.Delete(pData));
    return TRUE;
Exit0:
    return FALSE;
}

STORAGE_EVENT_DATA *CMsGuildMgr::CreateStorageEventData()
{
    return m_StorageEventPool.New();
}

BOOL CMsGuildMgr::DeleteStorageEventData(STORAGE_EVENT_DATA *pData)
{
    return m_StorageEventPool.Delete(pData);
}

GUILD_UPGRADE_TRIGGER *CMsGuildMgr::CreateUpgradeTriggerData()
{
    return m_UpgradeTriggerPool.New();
}

BOOL CMsGuildMgr::DeleteUpgradeTriggerData(GUILD_UPGRADE_TRIGGER *pData)
{
    return m_UpgradeTriggerPool.Delete(pData);
}

MS_GUILD_WAR_DATA *CMsGuildMgr::CreateWarTriggerData()
{
    return m_GuildWarPool.New();
}

BOOL CMsGuildMgr::DeleteWarTriggerData(MS_GUILD_WAR_DATA* pData)
{
    return m_GuildWarPool.Delete(pData);
}

G_B_REVIVE_TRIGGER *CMsGuildMgr::NewBGReviveTrigger()
{
    return m_GBRevivePool.New();
}

BOOL CMsGuildMgr::DeleteBGReviveTrigger(G_B_REVIVE_TRIGGER *pTrigger)
{
    return m_GBRevivePool.Delete(pTrigger);
}

UINT32 CMsGuildMgr::MatchGuild(CMsGuild *poThis)
{
    BASE_LIST_NODE      *pHeader = &m_Ladder.Head();
    BASE_LIST_NODE      *pRear = &m_Ladder.Rear();
    BASE_LIST_NODE      *pNode = NULL;
    CMsGuild            *poMSTargetGuild = NULL;
    INT32               nRandom = rand() % 100;
    INT32               nIndex = 0;
    BASE_LIST_NODE      *pNodeList[100] = {0};
    INT32               nMaxCount = 0;

    LOG_PROCESS_ERROR(poThis);

    pNode = poThis->GetLadderNode().pPrev;
    while (pNode != pHeader && nIndex < 50)
    {
        pNodeList[nIndex++] = pNode;
        pNode = pNode->pPrev;
    }

    pNode = poThis->GetLadderNode().pNext;
    while (pNode != pRear && nIndex < 100)
    {
        pNodeList[nIndex++] = pNode;
        pNode = pNode->pNext;
    }

    nMaxCount = nIndex;
    nRandom = rand() % 100;

    nIndex = 0;
    for (nIndex = 0; nIndex < nMaxCount; ++nIndex)
    {
        nRandom = nRandom % nMaxCount;
        pNode = pNodeList[nRandom];

        poMSTargetGuild = node_cast(CMsGuild*, GetLadderNode(), pNode);
        LOG_PROCESS_ERROR(poMSTargetGuild);

        if (poMSTargetGuild->GetProtectionTime() < CManageServer::GetInstance().GetGameTime())
            return poMSTargetGuild->GetID();
        else
            ++nRandom;
    }

Exit0:
    return 0;
}

BOOL CMsGuildMgr::InsertGuildIntoLadder(CMsGuild *poMsGuild)
{
    LOG_PROCESS_ERROR(poMsGuild);
    m_Ladder.PushRear(&poMsGuild->GetLadderNode());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMgr::RemoveGuildFromLadder(CMsGuild *poMsGuild)
{
    m_Ladder.Remove(&poMsGuild->GetLadderNode());
    ClearNode(&poMsGuild->GetLadderNode());

    return TRUE;
//Exit0:
//    return FALSE;
}

BOOL CMsGuildMgr::AjustGuild(CMsGuild *poMSGuild)
{
    INT32 nRetCode = 0;
    nRetCode = RemoveGuildFromLadder(poMSGuild);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = InsertGuildIntoLadder(poMSGuild);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _SortLadder();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMgr::SyncAllGuildDataToGS(UINT32 dwGSSessionID)
{
    CMsGuild    *poMsGuild = NULL;
    SYNC_GS_GUILD_DATA  data;
    INT32       nRetCode = 0;
    INT32       nCount = 0;

    poMsGuild = GetFirstGuildOnLvlRank();
    while(poMsGuild)
    {
        //nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildDataNtf(dwGSSessionID, poMsGuild);
        //LOG_CHECK_ERROR(nRetCode);
        //++nCount;

        data.dwGSSessionID = dwGSSessionID;
        data.dwGuildID = poMsGuild->GetID();
        m_GSSyncList.push_back(data);

        poMsGuild = GetNextGuildOnLvlRank(poMsGuild);
    }
    return TRUE;
}

BOOL CMsGuildMgr::SyncGuildDataToClient(MS_ONLINE_ROLE_DATA* pstOnlineRole, UINT32 dwGuildID)
{
    INT32           nRetCode = 0;
	CMsGuild        *poMsGuild = NULL;
    MS_ROLE_DATA    *pLeaderRoleData = NULL;
    CMsGuildMember  *poMsGuildMember = NULL;
    char            *pLeaderName = NULL;

	LOG_PROCESS_ERROR(pstOnlineRole);
	LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

	poMsGuild = FindGuildByID(dwGuildID);
	LOG_PROCESS_ERROR(poMsGuild);

	LOG_PROCESS_ERROR(poMsGuild->GetSvrGroup() == pstOnlineRole->pRoleData->byGroup);

    poMsGuildMember = poMsGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(poMsGuildMember);

    pLeaderRoleData = CRoleDataMgr::GetInstance().FindRole(poMsGuild->GetLeaderID());
    if (pLeaderRoleData)
    {
        pLeaderName = pLeaderRoleData->szRoleName;
    }

    nRetCode = CMessageHandler::GetInstance().DoClientGetGuildDataNtf(pstOnlineRole->dwCliSessionID, poMsGuild, pLeaderName);
    LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CMsGuildMgr::PreCreateGuild(UINT32 dwNewGuildID, MS_ONLINE_ROLE_DATA &onlineRoleData, const char *pcszGuildName, const char *pcszAnnounce, INT32 nIsNeedToConsume)
{
	BOOL    bret = FALSE;
	INT32   nResult = errUnknownError;
    INT32   nRetCode = 0;
    UINT16  wHashKey = 0;
    CMsGuild *poMsGuild = NULL;

    PROCESS_ERROR_RET_CODE(0 == onlineRoleData.pRoleData->dwGuildID, errGuildAlreadyJoin);
	PROCESS_ERROR_RET_CODE(!FindGuildByName(pcszGuildName), errGuildSameName);
    LOG_PROCESS_ERROR_RET_CODE(m_GuildPool.GetSize() < MAX_GUILD_NUM_SYSTEM_LIMIT, errGuildNumLimit);

    poMsGuild = m_GuildPool.New(dwNewGuildID);
    LOG_PROCESS_ERROR(poMsGuild);

    nRetCode = poMsGuild->Create(dwNewGuildID, onlineRoleData.pRoleData->byGroup, 
        onlineRoleData.pRoleData->dwRoleID, onlineRoleData.pRoleData->szRoleName, pcszGuildName, pcszAnnounce);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildFailed);

    wHashKey = _GetHashValue(poMsGuild->GetNameMapNode().ID);
    nRetCode = LM_Insert(&(poMsGuild->GetNameMapNode()), &(m_pRoot[wHashKey]));
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildFailed);

	nRetCode = CMessageHandler::GetInstance().DoGSPreCreateGuild(onlineRoleData.dwGSSessionID, 
        onlineRoleData.pRoleData->dwRoleID, pcszGuildName, nIsNeedToConsume);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildInternalError);

	return errSuccess;
Exit0:
    if (poMsGuild && errUnknownError != nResult)
    {
        if (errGuildFailed != nResult)
        {
            LM_Erase(&poMsGuild->GetNameMapNode(), &m_pRoot[wHashKey]);
        }
        LOG_CHECK_ERROR(m_GuildPool.Delete(poMsGuild));
    }
	return nResult;
}

INT32 CMsGuildMgr::CreateGuild(MS_ONLINE_ROLE_DATA &onlineRoleData, const char *pcszGuildName)
{
	INT32   nResult = errUnknownError;
    INT32   nRetCode = 0;
    CMsGuild    *poMsGuild = NULL;
    UINT16  wHashKey = 0;

    PROCESS_ERROR_RET_CODE(0 == onlineRoleData.pRoleData->dwGuildID, errGuildAlreadyJoin);
    poMsGuild = FindGuildByName(pcszGuildName);      // to be optimized
    LOG_PROCESS_ERROR(poMsGuild);

    // Build a sorted guild list
    nRetCode = InsertGuildIntoLvlRank(poMsGuild);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildInternalError);

    nRetCode = InsertGuildIntoLadder(poMsGuild);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildInternalError);

	// add to mysql
    nRetCode = _SqlInsertGuild(poMsGuild); 
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = poMsGuild->InitProsperity();
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildCreateFailed);

    nRetCode = poMsGuild->AddGuildEvent(GUILD_HE_TYPE_CREATE, poMsGuild->GetLeaderID(), 0, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

	nRetCode = CMessageHandler::GetInstance().DoGSRoleAddGuildNtf(poMsGuild);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientCreateGuild(onlineRoleData.dwCliSessionID, poMsGuild->GetName());
    LOG_CHECK_ERROR(nRetCode);

    // add member Allan Tao[8.4.2013] 
    nRetCode = poMsGuild->AddMember(onlineRoleData.pRoleData, GUILD_POSITION_LEADER);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, errGuildAddMemberFailed);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildChangePosition(poMsGuild->GetID(), onlineRoleData.pRoleData->dwRoleID, GUILD_POSITION_LEADER);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode = CMessageHandler::GetInstance().DoDLGuildEvent(poMsGuild->GetSvrGroup(), 
        CManageServer::GetInstance().GetGameTime(), poMsGuild->GetID(), GUILD_HE_TYPE_CREATE, poMsGuild->GetLeaderID(), 0, 0, 0);
    LOG_CHECK_ERROR(nRetCode);

	return errSuccess;
Exit0:
    if (poMsGuild && errUnknownError != nResult)
    {
        wHashKey = _GetHashValue(poMsGuild->GetNameMapNode().ID);
        LM_Erase(&poMsGuild->GetNameMapNode(), &m_pRoot[wHashKey]);
        if (errGuildInternalError != nResult)
        {
            nRetCode = RemoveGuildFromLvlRank(poMsGuild);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = RemoveGuildFromLadder(poMsGuild);
            LOG_CHECK_ERROR(nRetCode);
        }
        LOG_CHECK_ERROR(m_GuildPool.Delete(poMsGuild));
    }
	return nResult;
}

BOOL CMsGuildMgr::DestroyGuild(CMsGuild *poMsGuild)
{
    INT32   nRetCode = errGuildFailed;
    UINT16  wHashKey = 0;

	LOG_PROCESS_ERROR(poMsGuild);

    // broadcast to gs
    nRetCode = CMessageHandler::GetInstance().DoGSDeleteGuildNtf(poMsGuild->GetID());
    LOG_CHECK_ERROR(nRetCode);

    // broadcast to client
    nRetCode = CMessageHandler::GetInstance().DoClientDisbandGuildNtf(poMsGuild);
    LOG_CHECK_ERROR(nRetCode);

    // delete guild sort memory
    nRetCode = RemoveGuildFromLvlRank(poMsGuild);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = RemoveGuildFromLadder(poMsGuild);
    LOG_CHECK_ERROR(nRetCode);

    wHashKey = _GetHashValue(poMsGuild->GetNameMapNode().ID);
    LM_Erase(&poMsGuild->GetNameMapNode(), &m_pRoot[wHashKey]);

    nRetCode = poMsGuild->Destory();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_GuildPool.Delete(poMsGuild);
    LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

CMsGuild * CMsGuildMgr::FindGuildByID(UINT32 dwGuildID)
{
	return m_GuildPool.Find(dwGuildID);
}

CMsGuild * CMsGuildMgr::FindGuildByName(const char *pcszGuildName)
{
	INT32   nRetCode = errGuildFailed;
	UINT16  wHashKey = 0;
	GUILD_NAME_KEY  key;
    //char            tempName[GUILD_NAME_LEN] = {0};
	LM_MAP_NODE<GUILD_NAME_KEY> *pNode;

	LOG_PROCESS_ERROR(pcszGuildName);

    //strxcpy(tempName, pcszGuildName, GUILD_NAME_LEN);
	strncpy((char*)&key, pcszGuildName, sizeof(key));
	wHashKey = _GetHashValue(key);

	pNode = LM_Search(key, m_pRoot[wHashKey]);
	PROCESS_ERROR(pNode);

	return (CMsGuild *)((char*)pNode - (char*)(&((CMsGuild*)0)->GetNameMapNode()));
Exit0:
	return NULL;
}

//BOOL CMsGuildMgr::_GuildApplicationWashOut(UINT32 dwCurrentTime, UINT32 dwCheckTime)
//{
//    INT32   nRetCode = 0;
//    BASE_LIST_NODE  *pNext = m_lruRoleAppList.Head().pNext;
//    while (pNext && pNext != &m_lruRoleAppList.Rear())
//    {
//        G_GUILD_APPLICANTION_DATA   *pAppData = NULL;
//        pAppData = (G_GUILD_APPLICANTION_DATA*)((char *)pNext - (char *)(&((G_GUILD_APPLICANTION_DATA*)0)->m_roleAppListNode));
//
//        if (pAppData->dwEndTime < dwCurrentTime + dwCheckTime)   // Delete all entries within the next dwCheckTime seconds
//        {
//            BASE_LIST_NODE *pOldNext = pNext;
//            pNext = pNext->pNext;
//
//            nRetCode = m_lruRoleAppList.Remove(pOldNext);
//            LOG_CHECK_ERROR(nRetCode);
//
//            CMsGuild    *poMsGuild = FindGuildByID(pAppData->dwGuildID);
//            LOG_CHECK_ERROR(poMsGuild);
//            if (poMsGuild)
//            {
//                MS_ROLE_DATA    *pRoleData = NULL;
//                pRoleData = CRoleDataMgr::GetInstance().FindRole(pAppData->dwRoleID);
//                LOG_CHECK_ERROR(pRoleData);
//                if (pRoleData)
//                {
//                    --pRoleData->byApplicationCount;
//                }
//
//                nRetCode = poMsGuild->DelGuildApplication(pAppData->dwRoleID);
//                LOG_CHECK_ERROR(nRetCode);
//            }
//        }
//        else    // The queue is ordered by increasing time. Allan Tao [8.5.2013]
//            break;
//    }
//
//    return TRUE;
//}

BOOL LoadGuild(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    UINT64      qwStartTick = GetTickCountEx();

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        GUILD_DB* pLoadResult = (GUILD_DB*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->bySvrGroup));
        LOG_PROCESS_ERROR(pLoadResult->bySvrGroup >= 0 && pLoadResult->bySvrGroup < MAX_SERVER_GROUP_COUNT);
        if (g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID > 0 &&
            g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID < MAX_SERVER_GROUP_COUNT)
		{
            pLoadResult->bySvrGroup = g_ServerConfig.GroupConfig[pLoadResult->bySvrGroup].nMappedGroupID;
		}

        nRetCode = CMsGuildMgr::GetInstance().LoadGuild(*pLoadResult);
        LOG_PROCESS_ERROR(nRetCode);

        nRecords++;
    }
    INF("Loading Guild Data Finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL LoadGMember(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    UINT64      qwStartTick = GetTickCountEx();
    CMsGuildMember  oMember;
    MS_ROLE_DATA    *pRoleData = NULL;
    CMsGuild        *poMsGuild = NULL;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8       bySvrGroup;
        UINT32      dwRoleID;
        UINT32      dwGuildID;
        UINT8       byPosition;
        UINT8       byFlag;
        INT32       nSalary;
        UINT32      dwDrawSalaryTime;
        INT32       nPow;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->bySvrGroup));
        pRoleData = CRoleDataMgr::GetInstance().FindRole(pLoadResult->dwRoleID);
        LOG_PROCESS_ERROR(pRoleData);

        poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pLoadResult->dwGuildID);
        LOG_PROCESS_ERROR(poMsGuild);

        nRetCode = oMember.Init(poMsGuild->GetID(), pLoadResult->dwRoleID, pLoadResult->byPosition, pLoadResult->byFlag, 
            pLoadResult->nSalary, pLoadResult->dwDrawSalaryTime, pLoadResult->nPow);
        LOG_PROCESS_ERROR(nRetCode);

        poMsGuild->LoadMember(oMember);
        pRoleData->dwGuildID = poMsGuild->GetID();

        nRecords++;
    }
    INF("Loading Guild member Data Finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL LoadGuildWar(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8       bySvrGroup;
        UINT32      dwIntruderID;
        UINT32      dwDefenderID;
        UINT32      dwTime;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->bySvrGroup));
        nRetCode = CMsGuildMgr::GetInstance().AddWarData(pLoadResult->bySvrGroup, pLoadResult->dwIntruderID, pLoadResult->dwDefenderID, pLoadResult->dwTime);
        LOG_PROCESS_ERROR(nRetCode);

        nRecords++;
    }

    INF("Loading guild war data finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CB_GuildWarBegin(BASE_TRIGGER_NODE *pNode)
{
    INT32       nRetCode = 0;
    CLuaScript* poScript = NULL;
    MS_GUILD_WAR_DATA   *pWarData = NULL;

    LOG_PROCESS_ERROR(pNode);
    
    pNode->dwWashoutTime += ONE_DAY_SECOND;
    pNode->fp = CB_GuildWarBegin;

    nRetCode = CMsGuildMgr::GetInstance().ClearOldWarData();
    LOG_CHECK_ERROR(nRetCode);

    // 
    nRetCode = CMsGuildMgr::GetInstance().WarBegin();
    LOG_CHECK_ERROR(nRetCode);

    poScript = CScriptMgr::GetInstance().FindScript(GUILD_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    nRetCode = poScript->CallFunction("lua_trigger_callback", "sd", "GuildWar", CManageServer::GetInstance().GetGameTime());
    LOG_PROCESS_ERROR(nRetCode);

    return TIME_TRIGGER_RET_CONTINUE;
Exit0:
    return TIME_TRIGGER_RET_END;
}

BOOL GuildWarEnd(UINT32 dwDefenderID, INT32 nWarDefenderResult)
{
    INT32       nRetCode = 0;

    nRetCode = CMessageHandler::GetInstance().DoGSGuildWarEnd(dwDefenderID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().WarEnd(dwDefenderID, nWarDefenderResult);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL GuildSyncWarRelation(UINT32 dwIntruderID, UINT32 dwDefenderID)
{
    return CMessageHandler::GetInstance().DoGSGuildSyncWarRelation(dwIntruderID, dwDefenderID);
}


BOOL GuildSyncResChange(UINT32 dwNpcID, UINT32 dwGuildID, BOOL bAdd, INT32 nResType, INT32 nChangeCount, INT32 nCtxType)
{
    return CMessageHandler::GetInstance().DoGSGuildChangeRes(dwNpcID, dwGuildID, bAdd, nResType, nChangeCount, nCtxType);
}
