#include "stdafx.h"
#include "sol_bindings.h"
#include "message_handler.h"

#include "imessage_notifier.h"
#include "g2c_message.h"


#include "server_def.h"

#include "scene.h"
#include "role.h"
#include "teleport_def.h"
#include "game_world.h"
#include "team.h"
#include "scene_data.h"
#include "data_manager.h"
#include "drop_generator.h"
#include "score_def.h"
#include "scene_file_mgr.h"
#include "mail_template_data.h"
#include "item_mgr.h"
#include "drop_group_data.h"

#ifndef _ROBOT

using namespace ExternalMessage;

BOOL CCliMessageHandler::DoSceneFinish(CScene* poScene)
{
    INT32 nRetCode = 0;
    G2C_SCENE_FINISH* pMsg = NULL;

    LOG_PROCESS_ERROR(poScene);

    pMsg = (G2C_SCENE_FINISH*)m_poPacketAllocator->Alloc(sizeof(G2C_SCENE_FINISH));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_scene_finish;

    nRetCode = _BroadcastScene(poScene, (PACKET_BUFFER*)pMsg, sizeof(G2C_SCENE_FINISH));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CCliMessageHandler::DoDungeonFinishData(CRole* poRole, INT32 nTotalTime, INT32 nTimeScore, INT32 nKillNpcCount, 
    INT32 nDestroyDoodadCount, INT32 nAttackScore, UINT32 dwGoalResult, INT32 nGoalScore, INT32 nTotalScore, 
    INT32 nNormalExp, INT32 nTeamExp, INT32 nGuildExp, INT32 nMasterExp, INT32 nVipExp, INT32 nFirstPassExp, INT32 nActivityExp, INT32 nTotalExp)
{
    INT32 nRetCode = 0;
    G2C_DUNGEON_FINISH_DATA* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_DUNGEON_FINISH_DATA*)m_poPacketAllocator->Alloc(sizeof(G2C_DUNGEON_FINISH_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_dungeon_finish_data;

    pMsg->nTotalTime = nTotalTime;
    pMsg->byTimeScore = (UINT8)nTimeScore;
    pMsg->nKillNpcCount = nKillNpcCount;
    pMsg->nDestroyDoodadCount = nDestroyDoodadCount;
    pMsg->byAttackScore = (UINT8)nAttackScore;
    pMsg->byGoalResult = (UINT8)dwGoalResult;
    pMsg->byGoalScore = (UINT8)nGoalScore;
    pMsg->byTotalScore = (UINT8)nTotalScore;

    pMsg->nNormalExp = nNormalExp;
    pMsg->nTeamExp = nTeamExp;
    pMsg->nGuildExp = nGuildExp;
    pMsg->nMasterExp = nMasterExp;
    pMsg->nVipExp = nVipExp;
    pMsg->nFirstPassExp = nFirstPassExp;
    pMsg->nActivityExp = nActivityExp;
    pMsg->nTotalExp = nTotalExp;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_DUNGEON_FINISH_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

struct CTraverseRoleForDungeonScore
{
    G2C_DUNGEON_FINISH_SCORE* pMsg;

    BOOL operator()(CRole* poRole)
    {
        LOG_PROCESS_ERROR(pMsg);

        pMsg->stScore[pMsg->byCount].dwRoleID = poRole->GetObjID();
        pMsg->stScore[pMsg->byCount].dwTotalScore = poRole->GetCombatStat(escsTotalScore);
        pMsg->byCount++;

        return TRUE;
Exit0:
        return FALSE;
    }
};

BOOL CCliMessageHandler::DoSyncDungeonScore(CScene* poScene)
{
    INT32 nRetCode = 0;
    G2C_DUNGEON_FINISH_SCORE* pMsg = NULL;

    LOG_PROCESS_ERROR(poScene);

    pMsg = (G2C_DUNGEON_FINISH_SCORE*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->byCount = 0;
    CTraverseRoleForDungeonScore stTraverseRoleForDungeonScore;
    stTraverseRoleForDungeonScore.pMsg = pMsg;

    poScene->TraverseRole(stTraverseRoleForDungeonScore);
    LOG_PROCESS_ERROR(pMsg->byCount > 0);

    pMsg->wMsg = g2c_dungeon_finish_score;
    pMsg->wSize = sizeof(G2C_DUNGEON_FINISH_SCORE) + (pMsg->byCount * sizeof(SCENE_SCORE));
    nRetCode = _BroadcastScene(poScene, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoDungeonReward(CRole*poRole, INT32 nType, UINT32 dwRewardID, UINT32 dwCount)
{
    INT32 nRetCode = 0;
    G2C_DUNGEON_REWARD* pMsg = NULL;
    CScene* poScene = NULL;

    LOG_PROCESS_ERROR(poRole);

    poScene = poRole->GetScene();
    LOG_PROCESS_ERROR(poScene);

    pMsg = (G2C_DUNGEON_REWARD*)m_poPacketAllocator->Alloc(sizeof(G2C_DUNGEON_REWARD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_dungeon_reward;
    pMsg->dwRoleID = poRole->GetObjID();
    pMsg->byRewardType = nType;    
    pMsg->dwRewardID = dwRewardID;
    pMsg->dwRewardCount = dwCount;

    nRetCode = _BroadcastScene(poScene, (PACKET_BUFFER*)pMsg, sizeof(G2C_DUNGEON_REWARD));
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncDungeonGoal(CRole* poRole)
{
    INT32 nRetCode = 0;
    G2C_SYNC_DUNGEON_GOAL* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_SYNC_DUNGEON_GOAL*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_DUNGEON_GOAL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_dungeon_goal;
    pMsg->nBossKillCount = poRole->GetCombatStat(escsBossKillCount);
    pMsg->nEliteNpcKillCount = poRole->GetCombatStat(escsEliteNpcKillCount);

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_DUNGEON_GOAL));
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

void CCliMessageHandler::OnDungeonSelectReward(CRole* pRole, const char* pData)
{
    const DROP_GROUP_DATA* data = nullptr;
    INT32 nRetCode = 0;
    C2G_DUNGEON_SELECT_REWARD* pMsg = (C2G_DUNGEON_SELECT_REWARD*)pData;
    CScene* poScene = NULL;
    const DUNGEON_DATA* poDungeonData = NULL;
    CItemBag* poItemBag = NULL;
	DropContent stRewardContent = { 0 };
	INT32 nDungeonID = 0;
    
    ACHIEVEMENT_VARIABLE_DEF eVarReward;

    LOG_PROCESS_ERROR(pRole);

    poScene = pRole->GetScene();
    LOG_PROCESS_ERROR(poScene);
    LOG_PROCESS_ERROR(poScene->IsCanReward());

    poItemBag = pRole->GetItemBag();
    LOG_PROCESS_ERROR(poItemBag);
	
	LOG_PROCESS_ERROR(poScene->GetSceneData());
	LOG_PROCESS_ERROR(poScene->GetSceneData()->nType == estDungeon || poScene->GetSceneData()->nType == estSoloDungeon);

	poDungeonData = CDataManager<DUNGEON_DATA>::GetInstance().GetData(poScene->GetSceneTempID());
    LOG_PROCESS_ERROR(poDungeonData);

    switch(pMsg->byRewardType)
    {
    case edrFirst:
        {
            eVarReward = avdReward_1;
            LOG_PROCESS_ERROR(pRole->GetVariable(eVarReward) == 0);
        }
        break;
    case edrSecond:
        {
            eVarReward = avdReward_2;
            LOG_PROCESS_ERROR( pRole->GetVariable(eVarReward) == 0);

            if(pRole->GetCombatStat(escsTotalScore) < poDungeonData->nRewardFree[edrSecond])
            {
				nRetCode = poItemBag->SubMoney(MONEYTYPE_01, poDungeonData->nRewardCost[edrSecond], MONEYLOG_DUNGEON_SELECT_REWARD, 0);
                LOG_PROCESS_ERROR(nRetCode);
            }
        }
        break;
    case edrThird:
        {
            eVarReward = avdReward_3;
            LOG_PROCESS_ERROR(pRole->GetVariable(eVarReward) == 0);
			
			if (pRole->GetVIPLevel() == vlLevel0)
			{
				nRetCode = poItemBag->SubMoney(MONEYTYPE_02, poDungeonData->nRewardCost[edrThird], MONEYLOG_DUNGEON_SELECT_REWARD, 0);
				LOG_PROCESS_ERROR(nRetCode);
			}
        }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    data = CDropManager::GetInstance().GetData(poDungeonData->nRewardGroup[pMsg->byRewardType]);
    LOG_PROCESS_ERROR(data);

    nRetCode = CDropGenerator::GetInstance().GenerateDropContentFromDropGroupData(pRole->GetServerGroup(), data, stRewardContent);
    LOG_PROCESS_ERROR(nRetCode);

    if(stRewardContent.m_type == DropContent::DROP_MONEY)
    {        
		nRetCode = poItemBag->AddMoney(stRewardContent.m_money.moneytype, stRewardContent.m_money.count, MONEYLOG_DUNGEON_REWARD, 0);
        LOG_PROCESS_ERROR(nRetCode == errSuccess);

        pRole->SetVariable(eVarReward, stRewardContent.m_money.moneytype);
        
        nRetCode = CCliMessageHandler::GetInstance().DoDungeonReward(pRole, pMsg->byRewardType, stRewardContent.m_money.moneytype, stRewardContent.m_money.count);
        LOG_PROCESS_ERROR(nRetCode);

    }
    else if(stRewardContent.m_type == DropContent::DROP_ITEM)
    {
        pRole->SetVariable(eVarReward, stRewardContent.m_item->m_iteminfo.m_nTemplateID);

        nRetCode = CCliMessageHandler::GetInstance().DoDungeonReward(pRole, pMsg->byRewardType,
            stRewardContent.m_item->m_iteminfo.m_nTemplateID, stRewardContent.m_item->m_iteminfo.m_nCurDur);
        LOG_PROCESS_ERROR(nRetCode);
        
		nRetCode = poItemBag->AddItem(stRewardContent.m_item, ITEMLOG_DUNGEON_REWARD, 0);
        if(nRetCode != errSuccess)
        {
            nRetCode = pRole->SendMail(pRole->GetObjID(), mtiDungeonFinshReward, stRewardContent.m_item->m_iteminfo);
			LOG_CHECK_ERROR(nRetCode);

            BOOL bItemRet = CItemMgr::GetInstance().DestroyItem(stRewardContent.m_item);
            LOG_CHECK_ERROR(bItemRet);
        }
    }
    
Exit0:
    return;
}

void CCliMessageHandler::OnDungeonReplay(CRole* pRole, const char* pData)
{
    INT32 nRetCode = 0;
    C2G_DUNGEON_REPLAY* pMsg = (C2G_DUNGEON_REPLAY*)pData;
    CScene* poScene = NULL;

    LOG_PROCESS_ERROR(pRole);
    
    poScene = pRole->GetScene();
    LOG_PROCESS_ERROR(poScene);

    LOG_PROCESS_ERROR(poScene->GetState() == edstClosing);

    if(pRole->GetTeamID() == 0)
    {
        nRetCode = CCliMessageHandler::GetInstance().DoQuerySceneDifficulty(pRole, poScene->GetSceneTempID());
        LOG_PROCESS_ERROR(nRetCode);

        pRole->SetTeleportID(INVALID_TELEPORT_ID);
    }
    else
    {
        CTeamInGS* poTeam = CGameWorld::GetInstance().FindTeam(pRole->GetTeamID());
        LOG_PROCESS_ERROR(poTeam);
        LOG_PROCESS_ERROR(poTeam->GetLeaderID() == pRole->GetObjID());

        nRetCode = CCliMessageHandler::GetInstance().DoQuerySceneDifficulty(pRole, poScene->GetSceneTempID());
        LOG_PROCESS_ERROR(nRetCode);

        for (INT32 nIndex =0; nIndex < poTeam->GetMaxMemberCount(); nIndex++)
        {
            CRole* poMemeber = CGameWorld::GetInstance().FindRole(poTeam->GetMemberTable(nIndex).dwRoleID);
            if (poMemeber)
                poMemeber->SetTeleportID(INVALID_TELEPORT_ID);
        }

        nRetCode = CCliMessageHandler::GetInstance().DoMSClearTeamDungeon(pRole);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return;
Exit0:
    return;
}

void CCliMessageHandler::OnEnterBattle(CRole* pRole)
{
	INT32 nRetCode = 0;
	CSceneStaticData* pStaticData = NULL;
	const SERVER_POINT* pServerPoint = NULL;
	
	CHAR szServerPointName[COMMON_NAME_LEN];
	LOG_PROCESS_ERROR(pRole);

    { // for linux error: jump to label 'Exit0'
    const ENTER_POS& stPos = pRole->GetBattleEnterPos();

    pStaticData = CSceneFileMgr::GetInstance().GetStaticData(stPos.nSceneTempID);
    LOG_PROCESS_ERROR(pStaticData);

    snprintf(szServerPointName, COMMON_NAME_LEN, "%s%d", ENTER_POS_SIDE_NAME, stPos.bySide);
    pServerPoint = pStaticData->GetServerPoint(szServerPointName);

    LOG_PROCESS_ERROR(pServerPoint);

	if(pRole->GetMoveState() == CMS_DIE)
	{
		nRetCode = pRole->Revive(crtAtCurrentPos, RATE_MAX/2);
		LOG_PROCESS_ERROR(nRetCode);
	}
	
    nRetCode = CGameWorld::GetInstance().JumpTo(pRole, stPos.nSceneTempID, stPos.nSceneID,  
	    pServerPoint->nX, pServerPoint->nY, pServerPoint->nZ, 0);
    LOG_PROCESS_ERROR(nRetCode);
    }
Exit0:
	return;
}

//BOOL CCliMessageHandler::DoArenaFinishScore(CScene* poScene, INT32 nScore)
//{
//	INT32 nRetCode = 0;
//	G2C_ARENA_FINISH_SCORE* pMsg = NULL;
//
//	LOG_PROCESS_ERROR(poScene);
//
//	pMsg = (G2C_ARENA_FINISH_SCORE*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
//	LOG_PROCESS_ERROR(pMsg);
//
//	pMsg->nCurScore = nScore;
//	pMsg->byCount = 0;
//
//	CTraverseRoleForArenaScore stTraverseRoleForArenaScore;
//	stTraverseRoleForArenaScore.pMsg = pMsg;
//
//	poScene->TraverseRole(stTraverseRoleForArenaScore);
//	LOG_PROCESS_ERROR(pMsg->byCount > 0);
//
//	pMsg->wMsg = g2c_arena_finish_score;
//	pMsg->wSize = sizeof(G2C_ARENA_FINISH_SCORE) + (pMsg->byCount * sizeof(SCENE_SCORE));
//	nRetCode = _BroadcastScene(poScene, (PACKET_BUFFER*)pMsg, pMsg->wSize);
//	LOG_PROCESS_ERROR(nRetCode);
//
//	return TRUE;
//Exit0:
//	if (pMsg)
//		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
//	return FALSE;
//}

#endif  // _ROBOT
