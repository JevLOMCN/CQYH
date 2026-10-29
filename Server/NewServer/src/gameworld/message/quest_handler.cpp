#include "stdafx.h"
#include "message_handler.h"

#include "imessage_notifier.h"
#include "game_world.h"
#include "npc_data.h"
#include "ai_processor.h"
#include "quest_list.h"
#include "item_bag.h"
#include "daily_quest_mgr.h"
#include "game_world_config.h"
#include "server_config.h"
#include "statistic.h"

#include "g2m_message.h"

#ifndef _ROBOT

using namespace ExternalMessage;

void CCliMessageHandler::OnAcceptQuest(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
	const QUEST_DATA* pQuestData = NULL;
	C2G_ACCEPT_QUEST* pMsg = (C2G_ACCEPT_QUEST*) pData;

	if (pMsg->nQuestID != INVALID_QUEST_ID)
	{
		LOG_PROCESS_ERROR(pMsg->nAcceptType > qatInvalid && pMsg->nAcceptType < qatPublish);

		pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pMsg->nQuestID);
		LOG_PROCESS_ERROR(pQuestData);

		nRetCode = poRole->GetQuestList().CanAccept(pQuestData, pMsg->nAcceptType, pMsg->nAcceptSrcID);
		PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

		nRetCode = poRole->GetQuestList().Accept(pMsg->nQuestID);
		PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	}
	else
	{
		INT32 nCoolDownFrm = 0;
		INT32 nQuestCount = 0;
		CScene* pScene = NULL;
		const ZONE_DATA* pcZone = NULL;
		const GRID_DATA* pcGrid = NULL;

		pScene = poRole->GetScene();
		LOG_PROCESS_ERROR(pScene);

		nRetCode = pScene->GetLowerGrid(poRole->GetX(), poRole->GetY(), poRole->GetZ() + GRID_WIDTH, pcZone, pcGrid);
		LOG_PROCESS_ERROR(nRetCode);

		LOG_PROCESS_ERROR(pcZone);
		LOG_PROCESS_ERROR(pcGrid);
		PROCESS_SUCCESS(!pcGrid->bQuest);

		for (INT32 nIndex = 0; nIndex < MAX_QUEST_PER_ZONE; nIndex++)
		{
			INT32 nCoolDownID = 0;

			pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pcZone->nQuestID[nIndex]);
			PROCESS_SUCCESS(!pQuestData);

			nCoolDownID = MakeCoolDownID(COOLDOWN_QUEST, pQuestData->nQuestID);

			nCoolDownFrm = poRole->GetCoolDown().GetRemainFrm(nCoolDownID);
			if (nCoolDownFrm <= 0)
			{
				nRetCode = poRole->GetQuestList().CanAccept(pQuestData, qatEmegency, 0);
				if (nRetCode == errSuccess)
				{
					nRetCode = poRole->GetQuestList().Accept(pQuestData->nQuestID);
					LOG_PROCESS_ERROR(nRetCode);

					nRetCode = poRole->GetCoolDown().Add(nCoolDownID, pQuestData->nEmergencyCoolDown);
					LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
					CCliMessageHandler::GetInstance().DoAddCooldown(poRole, nCoolDownID, pQuestData->nEmergencyCoolDown);
#endif
				}
			}
		}
	}

    return;
Exit1:
	return;
Exit0:
	CRI("accept quest %d error, type %d, src %d", pMsg->nQuestID, pMsg->nAcceptType, pMsg->nAcceptSrcID);
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);

    return;
}

void CCliMessageHandler::OnCompleteQuest(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
	C2G_COMPLETE_QUEST* pMsg = (C2G_COMPLETE_QUEST*) pData;

	nRetCode = poRole->GetQuestList().CanComplete(pMsg->nQuestID);
	LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	nRetCode = poRole->GetQuestList().Complete(pMsg->nQuestID, pMsg->nSelectedAwardIdx);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoDLCompleteQuest(poRole, pMsg->nQuestID);
	LOG_CHECK_ERROR(nRetCode);

	return;
Exit0:
	CRI("complete quest %d error, select %d", pMsg->nQuestID, pMsg->nSelectedAwardIdx);
	if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnAbandonQuest(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_ABANDON_QUEST* pMsg = (C2G_ABANDON_QUEST*) pData;
	INT32 nCompleteCnt = 0;

    nRetCode = poRole->GetQuestList().CanAbandon(pMsg->nQuestID);
    PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	nRetCode = poRole->GetQuestList().Abandon(pMsg->nQuestID);
    PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoAbandonQuest(poRole, pMsg->nQuestID);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoDLAbandonQuest(poRole, pMsg->nQuestID);
	LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
	CRI("abandon quest %d error", pMsg->nQuestID);
	if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnInteractWithNpc(CRole* poRole, const char* pData)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    C2G_INTERACT_WITH_NPC* pMsg = (C2G_INTERACT_WITH_NPC*) pData;
    CNpc* poNpc = NULL;

    LOG_PROCESS_ERROR_RET_CODE(!poRole->IsFreeMode(), errFreeMode);

    poNpc = CGameWorld::GetInstance().FindNpc(pMsg->dwNpcID);
    PROCESS_ERROR_RET_CODE(poNpc, errNpcNotFound);
    LOG_PROCESS_ERROR(poNpc->GetNpcData());

    nRetCode = poRole->CanInteract(poNpc);
    PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    nRetCode = poRole->Interact(poNpc);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnStopInteractWithNpc(CRole* poRole, const char* pData)
{
    INT32 nRetCode = 0;

    nRetCode = poRole->StopInteract();
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:

    return;
}

void CCliMessageHandler::OnSelectDialogOption(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    C2G_SELECT_DIALOG_OPTION * pMsg = (C2G_SELECT_DIALOG_OPTION *) pData;
    CNpc* poNpc = NULL;

    poNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    PROCESS_ERROR_RET_CODE(poNpc, errNpcNotFound);

    nRetCode = poRole->CanInteract(poNpc);
    PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    nRetCode = poRole->SelectOption(pMsg->nParam);
    PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnShareQuest(CRole* poRole, const char* pData)
{
	const QUEST_DATA* pQuestData = nullptr;
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;

	C2G_SHARE_QUEST * pMsg = (C2G_SHARE_QUEST*) pData;

	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(pMsg);

	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pMsg->nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	if (pMsg->nShareType == qatShare)
	{
		CTeamInGS* pTeam = NULL;
		const ACCEPTED_QUEST * pQuest = poRole->GetQuestList().GetAcceptedQuest(pMsg->nQuestID);
		LOG_PROCESS_ERROR_RET_CODE(pQuest, errQuestAlreadyCompleted);

		LOG_PROCESS_ERROR_RET_CODE(pQuestData->bCanShare, errQuestCannotShare);
		pTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
		PROCESS_ERROR_RET_CODE(pTeam,errQuestNoShareTeam);
		PROCESS_ERROR_RET_CODE(pTeam->GetMemberCount() > 1, errQuestShareUnavailable);

		for (UINT32 i = 0; i < pTeam->GetMemberCount(); i++)
		{	
			CRole * poTeamer = CGameWorld::GetInstance().FindValidRole(pTeam->GetMemberTable(i).dwRoleID);
			if (poTeamer == NULL || poTeamer->GetObjID() == poRole->GetObjID() || poTeamer->GetSceneID() != poRole->GetSceneID())
				continue;

			nResult = poTeamer->GetQuestList().CanAccept(pQuestData, qatShare, poRole->GetObjID());
			if (nResult == errSuccess)
				nRetCode = CCliMessageHandler::GetInstance().DoShareQuest(poTeamer, pMsg->nQuestID, poRole->GetObjID());
			else
				nRetCode = poRole->ErrorCodeNotify(errQuestShareUnavailable, pMsg->nQuestID, poRole->GetObjID());

			LOG_PROCESS_ERROR_RET_CODE(nRetCode, errQuestShareUnavailable);
		}
	}
	else if(pMsg->nShareType == qatPublish)
	{
		LOG_PROCESS_ERROR(pQuestData->bCanPublish);

		LOG_PROCESS_ERROR(poRole->GetMaster().IsTeacher());
		LOG_PROCESS_ERROR_RET_CODE((poRole->GetMaster().GetStudentCount() > 0), errMstHaveNoStu);

		nRetCode = CCliMessageHandler::GetInstance().DoMSPublishQuest(poRole, pMsg->nQuestID);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

	return;

Exit0:
	CRI("share quest %d error, type %d", pMsg->nQuestID, pMsg->nShareType);
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);

	return;
}

void CCliMessageHandler::OnConfirmPublishQuest(CRole* pRole, const char* pData)
{
	INT32 nRetCode = 0;
	const QUEST_DATA* pQuestData = NULL;

	C2G_CONFIRM_PUBLISH_QUEST * pMsg = (C2G_CONFIRM_PUBLISH_QUEST*) pData;
	LOG_PROCESS_ERROR(pMsg);
	LOG_PROCESS_ERROR(pMsg->dwMasterID == pRole->GetMaster().GetTeacherID());

	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pMsg->nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	nRetCode = pRole->GetQuestList().CanAccept(pQuestData, qatPublish, 0);
	LOG_PROCESS_ERROR(nRetCode == errSuccess);

	nRetCode = pRole->GetQuestList().Accept(pMsg->nQuestID);
	LOG_PROCESS_ERROR(nRetCode == errSuccess);

	nRetCode = CCliMessageHandler::GetInstance().DoDLAcceptQuest(pRole, pMsg->nQuestID);
	LOG_CHECK_ERROR(nRetCode);

	return;
Exit0:
	return;
}

void CCliMessageHandler::OnTalkToNpc(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	CNpc* poNpc = NULL;
	const QUEST_DATA* pQuestData = NULL;
	const ACCEPTED_QUEST* pAcceptedQuest = NULL;

	C2G_TALK_TO_NPC * pMsg = (C2G_TALK_TO_NPC*) pData;
	LOG_PROCESS_ERROR(pMsg);

	poNpc = CGameWorld::GetInstance().FindNpc(pMsg->dwTalkNpcID);
	LOG_PROCESS_ERROR(poNpc);
	LOG_PROCESS_ERROR(poNpc->GetNpcData());
	LOG_PROCESS_ERROR(poRole->GetInteractNpc() == pMsg->dwTalkNpcID);

	pAcceptedQuest = poRole->GetQuestList().GetAcceptedQuest(pMsg->nQuestID);;
	LOG_PROCESS_ERROR(pAcceptedQuest);

	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pMsg->nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	LOG_PROCESS_ERROR(pQuestData->nTalkNpcID == poNpc->GetNpcData()->nTemplateID);
	LOG_PROCESS_ERROR(pQuestData->nTalkVarID <= MAX_VARIABLE_PER_QUEST);

	nRetCode = poRole->GetQuestList().CheckNpc(poNpc->GetNpcData()->nTemplateID, qatNpc);
	LOG_PROCESS_ERROR(nRetCode);

	poRole->GetQuestList().SetVariable(pAcceptedQuest, pQuestData->nTalkVarID, 1);

	return;
Exit0:
	return;
}

void CCliMessageHandler::OnExploreZone(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;

	CScene* pScene = NULL;
	const ZONE_DATA* pcZone = NULL;
	const GRID_DATA* pcGrid = NULL;
	const QUEST_DATA* pQuestData = NULL;
	const ACCEPTED_QUEST* pAcceptedQuest = NULL;

	C2G_EXPLORE_ZONE* pMsg = (C2G_EXPLORE_ZONE*) pData;
	LOG_PROCESS_ERROR(pMsg);
	LOG_PROCESS_ERROR(poRole);

	pScene = poRole->GetScene();
	LOG_PROCESS_ERROR(pScene);

	nRetCode = pScene->GetLowerGrid(poRole->GetX(), poRole->GetY(), poRole->GetZ() + GRID_WIDTH, pcZone, pcGrid);
	LOG_PROCESS_ERROR(nRetCode);

	LOG_PROCESS_ERROR(pcZone);
	LOG_PROCESS_ERROR(pcGrid);
	LOG_PROCESS_ERROR(pcGrid->bQuest);

	for (INT32 nIndex = 0; (nIndex < MAX_QUEST_PER_ZONE) && (pcZone->nQuestID[nIndex] != INVALID_QUEST_ID); nIndex++)
	{
		pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pcZone->nQuestID[nIndex]);
		LOG_PROCESS_ERROR(pQuestData);

		pAcceptedQuest = poRole->GetQuestList().GetAcceptedQuest(pQuestData->nQuestID);

		if (pAcceptedQuest)
		{
			poRole->GetQuestList().SetVariable(pAcceptedQuest, pQuestData->nExploreVarID, 1);
			break;
		}
	}

	return;
Exit0:
	return;
}

void CCliMessageHandler::OnFlyToQuestZone(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	const ACCEPTED_QUEST* pQuest = NULL;
	const QUEST_DATA* pQuestData = NULL;

	C2G_FLY_TO_QUEST_ZONE* pMsg = (C2G_FLY_TO_QUEST_ZONE*)pData;

    LOG_PROCESS_ERROR(pMsg);
	LOG_PROCESS_ERROR((pMsg->nZoneIndex >= 0)&&(pMsg->nZoneIndex < MAX_FLY_SPOT_PER_QUEST));

    PROCESS_ERROR_RET_CODE(poRole->GetState() == rsPlaying, errJumpWrongState);
    PROCESS_ERROR_RET_CODE(!poRole->IsCasting(), errJumpCasting);
    PROCESS_ERROR_RET_CODE(poRole->GetStall()->GetStallState() == essNotOpen, errJumpStallOpen);
    PROCESS_ERROR_RET_CODE(!poRole->GetTrade()->IsTrading(), errJumpTrading);

	pQuest = poRole->GetQuestList().GetAcceptedQuest(pMsg->nQuestID);
	LOG_PROCESS_ERROR(pQuest);

	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pMsg->nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	if (poRole->GetAchievementList().GetVariable(avdQuestFlytoZone) > 0)
	{
		poRole->GetAchievementList().SetVariable(avdQuestFlytoZone, poRole->GetAchievementList().GetVariable(avdQuestFlytoZone)-1);
	}
	else
	{
		LOG_PROCESS_ERROR(poRole->GetItemBag()->GetMoney(MONEYTYPE_02) >= FLY_TO_QUEST_ZONE_COST);

		nRetCode = poRole->GetItemBag()->SubMoney(MONEYTYPE_02, FLY_TO_QUEST_ZONE_COST, MONEYLOG_QUEST_FLY_COST, 0);
		LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	}

	if (poRole->GetSceneTempID() == pQuestData->nSceneID[pMsg->nZoneIndex])
	{
		CScene* poScene = poRole->GetScene();		
		LOG_PROCESS_ERROR(poScene);

		nRetCode = poRole->JumpTo(pQuestData->nX[pMsg->nZoneIndex], pQuestData->nY[pMsg->nZoneIndex], 
			pQuestData->nZ[pMsg->nZoneIndex], poRole->GetFaceDirection());
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = CGameWorld::GetInstance().JumpTo(poRole, pQuestData->nSceneID[pMsg->nZoneIndex], 0, 
			pQuestData->nX[pMsg->nZoneIndex], pQuestData->nY[pMsg->nZoneIndex], pQuestData->nZ[pMsg->nZoneIndex], poRole->GetFaceDirection());
		LOG_PROCESS_ERROR(nRetCode);
	}

	return;
Exit0:
	if (nResult != errUnknownError)
	{
		poRole->ErrorCodeNotify(nResult, 0, 0);
	}

	return;
}

void CCliMessageHandler::OnBuyDailyQuestCompleteTime(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	C2G_BUY_DAILY_QUEST_COMPLETE_TIME* pMsg = (C2G_BUY_DAILY_QUEST_COMPLETE_TIME*)pData;
	LOG_PROCESS_ERROR(pMsg);
	LOG_PROCESS_ERROR(poRole);

	nRetCode = poRole->GetQuestList().BuyRewardBoughtTime(pMsg->nTime);
	LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	return;
Exit0:
	if (nResult != errUnknownError)
	{
		poRole->ErrorCodeNotify(nResult, 0, 0);
	}

	return;
}

void CCliMessageHandler::OnDailyQuestCompleteShotCut(CRole* poRole, const char* pData)
{
// 	INT32 nRetCode = 0;
// 	INT32 nResult = errUnknownError;
// 
// 	INT32 nQuestID = 0;
// 	INT32 nUnCompletedCost = 0;
// 	INT32 nCompleteQuestCount = 0;
// 	UINT64 qwRewardQuest = 0;
// 	ACCEPTED_QUEST* pAcceptedQuest = NULL;
// 
// 	LOG_PROCESS_ERROR(poRole);
// 
// 	qwRewardQuest = poRole->GetQuestList().GetRewardQuest();
// 
// 	for (INT32 nIndex = 0; nIndex < MAX_REWARD_QUEST_COUNT_PER_GROUP && nCompleteQuestCount < MAX_REWARD_QUEST_COUNT_PER_ROLE; nIndex++)
// 	{
// 		if ((qwRewardQuest & ((UINT64)1<<nIndex)) && !(poRole->GetQuestList().GetRewardCompleted() & ((UINT64)1<<nIndex)))
// 		{
//  			INT32 nQuestID = CDailyQuestMgr::GetInstance().GetQuestIDByIndex(poRole->GetQuestList().GetRewardIndex(), nIndex);
// 			LOG_PROCESS_ERROR(nQuestID != INVALID_QUEST_ID);
// 			
// 			PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(MONEYTYPE_02) >= REWARD_QUEST_BUY_COMPLETE_ONSHOT_COST, errNotEnoughMoney);
// 			PROCESS_ERROR_RET_CODE((poRole->GetQuestList().GetRewardBoughtTime() + poRole->GetQuestList().GetRewardQuestRemainCompleteTime()) > 0, errQuestDailyCompleteTimeOut);
// 
// 			if (!(poRole->GetQuestList().GetRewardAccepted() & ((UINT64)1<<nIndex)))
// 			{
// 				nRetCode = poRole->GetQuestList().Accept(nQuestID);
// 				LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
// 			}
// 
// 			nRetCode = poRole->GetQuestList().Complete(nQuestID);
// 			LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
// 
// 			nResult= poRole->GetItemBag()->SubMoney(MONEYTYPE_02, REWARD_QUEST_BUY_COMPLETE_ONSHOT_COST, MONEYLOG_DAILY_QUEST_BUY_ONESHOT);
// 			PROCESS_ERROR_RET_CODE((nResult == errSuccess), nResult);
// 
// 			nCompleteQuestCount++;
// 		}
// 	}
// 
// 	nRetCode = poRole->GetQuestList().GetAcceptedQuestBegin();
// 	LOG_PROCESS_ERROR(nRetCode);
// 
// 	nQuestID = poRole->GetQuestList().GetNextAcceptedQuest();
// 
// 	while (nQuestID != INVALID_QUEST_ID)
// 	{	
// 		const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(nQuestID);
// 		LOG_PROCESS_ERROR(pQuestData);
// 
// 		if (pQuestData->nQuestType == qtReward)
// 		{
// 			PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(MONEYTYPE_02) >= REWARD_QUEST_BUY_COMPLETE_ONSHOT_COST, errNotEnoughMoney);
// 
// 			nResult= poRole->GetItemBag()->SubMoney(MONEYTYPE_02, REWARD_QUEST_BUY_COMPLETE_ONSHOT_COST, MONEYLOG_DAILY_QUEST_BUY_ONESHOT);
// 			PROCESS_ERROR_RET_CODE((nResult == errSuccess), nResult);
// 
// 			nRetCode = poRole->GetQuestList().Complete(pQuestData->nQuestID);
// 			LOG_PROCESS_ERROR(nRetCode);
// 		}
// 
// 		nQuestID = poRole->GetQuestList().GetNextAcceptedQuest();
// 	}
// 
// 	return;
// Exit0:
// 	if (nResult != errUnknownError)
// 	{
// 		poRole->ErrorCodeNotify(nResult, 0, 0);
// 	}

	return;
}

void CCliMessageHandler::OnBrushDailyQuest(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	INT32 nDurationTime = 0;

	C2G_BRUSH_DAILY_QUEST* pMsg = (C2G_BRUSH_DAILY_QUEST*)pData;
	LOG_PROCESS_ERROR(pMsg);
	LOG_PROCESS_ERROR(poRole);

	LOG_PROCESS_ERROR(poRole->GetLevel() >= GENERATE_REWARD_QUEST_LEVEL);
	LOG_PROCESS_ERROR((poRole->GetQuestList().GetRewardBoughtTime() + poRole->GetQuestList().GetRewardQuestRemainCompleteTime()) > 0) ;	

	if (pMsg->bUseMoney)
	{
		LOG_PROCESS_ERROR_RET_CODE((poRole->GetItemBag()->GetMoney(MONEYTYPE_02) >= REWARD_QUEST_BRUSH_COST), errNotEnoughMoney);

		nRetCode = poRole->GetQuestList().GenerateRewardQuest();
		LOG_PROCESS_ERROR(nRetCode);

		nResult = poRole->GetItemBag()->SubMoney(MONEYTYPE_02, REWARD_QUEST_BRUSH_COST, MONEYLOG_DAILY_QUEST_BUY_ONESHOT, 0);
		LOG_PROCESS_ERROR_RET_CODE((nResult == errSuccess), nResult);

		poRole->GetAchievementList().AddVariable(avdBrushRewardQuestCount, 1);
	}
	else
	{
		nDurationTime = poRole->GetQuestList().GetRewardBrushCDTime() - CGameWorld::GetInstance().GetGameTime();
		LOG_PROCESS_ERROR(nDurationTime <= 0);

		nRetCode = poRole->GetQuestList().GenerateRewardQuest();
		LOG_PROCESS_ERROR(nRetCode);
	}

	poRole->GetQuestList().SetRewardBrushCDTime(CGameWorld::GetInstance().GetGameTime() + REWARD_QUEST_BRUSH_CD_INTERVAL);
	return;

Exit0:
	if (nResult != errUnknownError)
	{
		poRole->ErrorCodeNotify(nResult, nDurationTime, 0);
	}
	return;
}

void CCliMessageHandler::OnGenerateCircleQuest(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poRole->GetLevel() >= GENERATE_CIRCLE_QUEST_LEVEL);

	if (poRole->GetQuestList().GetCircleIndex() != (poRole->GetLevel()/CIRCLE_QUEST_LEVEL_PER_GRADE))
	{
		for (INT32 nIndex = 1; nIndex <= MAX_CIRCLE_COUNT; nIndex++)
		{
			nRetCode = poRole->GetQuestList().GenerateCircleQuest(nIndex);
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

	return;
Exit0:
	KICK_ROLE(poRole, ktLogged);
	
	return;
}

void CCliMessageHandler::OnQuestRecycle(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	const QUEST_RECYCLE_DATA* pRecycleData = NULL;
	Item* pItem = NULL;

	C2G_QUEST_RECYCLE* pMsg = (C2G_QUEST_RECYCLE*)pData;
	LOG_PROCESS_ERROR(pMsg);
	LOG_PROCESS_ERROR(poRole);

	pItem = poRole->GetItemBag()->GetItem(CONSLOT_DEFAULT, pMsg->nIndex);
	LOG_PROCESS_ERROR(pItem);
	LOG_PROCESS_ERROR(pItem->m_pTemplate);
	LOG_PROCESS_ERROR(pItem->m_pTemplate->m_nTemplateID == pMsg->nTemplateID);

	pRecycleData = CDataManager<QUEST_RECYCLE_DATA>::GetInstance().GetData(MAKE_QUEST_CYCLE_KEY(pItem->m_pTemplate->m_byItemLevel, pItem->m_pTemplate->m_byColor, pItem->m_pTemplate->m_bySubType));
	LOG_PROCESS_ERROR(pRecycleData);
	LOG_PROCESS_ERROR_RET_CODE(pRecycleData->nVarIndex == poRole->GetQuestList().GetItemRecycleVarIndex(), errQuestRecycle);

	nRetCode = poRole->GetItemBag()->DeleteItemPos(CONSLOT_DEFAULT, pMsg->nIndex, 1, ITEMLOG_QUEST_RECYCLE, 0);
	LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	poRole->SetVariable(pRecycleData->nVarIndex, 1);

	nRetCode = CCliMessageHandler::GetInstance().DoQuestRecycle(poRole, pMsg->nIndex, pMsg->nTemplateID);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (nResult != errUnknownError)
	{
		poRole->ErrorCodeNotify(nResult, 0, 0);
	}
	return;
}

void CCliMessageHandler::OnCompleteKillNpcQuest(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_COMPLETE_KILL_NPC_QUEST* pMsg = (C2G_COMPLETE_KILL_NPC_QUEST*)pData;

	nRetCode = poRole->GetKillNpcQuest().CanComplete(pMsg->wQuestID, pMsg->byIndex);
	PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	nRetCode = poRole->GetKillNpcQuest().Complete(pMsg->wQuestID, pMsg->byIndex);
	LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
	
	nRetCode = DoCompleteKillNpcQuest(poRole, pMsg->wQuestID, pMsg->byIndex);
	LOG_CHECK_ERROR(nRetCode);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);

	return;
}

/************************************************************************/

BOOL CCliMessageHandler::DoAcceptQuest(CRole * poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
    G2C_ACCEPT_QUEST* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_ACCEPT_QUEST*)m_poPacketAllocator->Alloc(sizeof(G2C_ACCEPT_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_accept_quest;
	pMsg->nQuestID = nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ACCEPT_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}
BOOL CCliMessageHandler::DoAbandonQuest(CRole * poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
    G2C_ABANDON_QUEST* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_ABANDON_QUEST*)m_poPacketAllocator->Alloc(sizeof(G2C_ABANDON_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_abandon_quest;
	pMsg->nQuestID = nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ABANDON_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCompleteQuest(CRole * poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;

    G2C_COMPLETE_QUEST* pMsg = (G2C_COMPLETE_QUEST*)m_poPacketAllocator->Alloc(sizeof(G2C_COMPLETE_QUEST));
	LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_complete_quest;
	pMsg->nQuestID = nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_COMPLETE_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoNotifyNextCircleQuest(CRole* poRole, INT32 nCircleID, INT32 nQuestID)
{
	INT32 nRetCode = 0;

	G2C_NOTIFY_NEXT_CIRCLE_QUEST* pMsg = (G2C_NOTIFY_NEXT_CIRCLE_QUEST*)m_poPacketAllocator->Alloc(sizeof(G2C_NOTIFY_NEXT_CIRCLE_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_notify_next_circle_quest;
	pMsg->nCircleID = nCircleID;
	pMsg->nQuestID = nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_NOTIFY_NEXT_CIRCLE_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDailyQuestCompleteTimeChange(CRole* poRole, INT32 nRemainTime)
{
	INT32 nRetCode = 0;

	G2C_BUY_DAILY_QUEST_COMPLETE_TIME* pMsg = (G2C_BUY_DAILY_QUEST_COMPLETE_TIME*)m_poPacketAllocator->Alloc(sizeof(G2C_BUY_DAILY_QUEST_COMPLETE_TIME));
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_buy_daily_quest_complete_time;
	pMsg->nCurrentRemainTime = nRemainTime;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_BUY_DAILY_QUEST_COMPLETE_TIME));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncDailyQuest(CRole* poRole, BOOL bClearCDTime)
{
	INT32 nRetCode = 0;
	INT32 nQuestIndex = 0;

	G2C_BRUSH_DAILY_QUEST* pMsg = (G2C_BRUSH_DAILY_QUEST*)m_poPacketAllocator->Alloc(sizeof(G2C_BRUSH_DAILY_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_brush_daily_quest;
    pMsg->bClearCDTime = bClearCDTime;

	memset(pMsg->nQuestID, 0, sizeof(pMsg->nQuestID));

	for (INT32 nIndex = 0; nIndex < MAX_REWARD_QUEST_COUNT_PER_GROUP; nIndex++)
	{
		if ((poRole->GetQuestList().GetRewardQuest() & ((UINT64)1<<nIndex)) && !(poRole->GetQuestList().GetRewardCompleted() & ((UINT64)1<<nIndex)))
		{
			pMsg->nQuestID[nQuestIndex] = CDailyQuestMgr::GetInstance().GetQuestIDByIndex(poRole->GetQuestList().GetRewardIndex(), nIndex);
			LOG_PROCESS_ERROR(nQuestIndex < MAX_REWARD_QUEST_COUNT_PER_ROLE);

			nQuestIndex++;
		}		
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_BRUSH_DAILY_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoQuestRecycle(CRole* poRole, INT32 nSlotIndex, INT32 nTemplateID)
{
	INT32 nRetCode = 0;
	G2C_QUEST_RECYCLE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_QUEST_RECYCLE*)m_poPacketAllocator->Alloc(sizeof(G2C_QUEST_RECYCLE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_quest_recycle;

	pMsg->nIndex = nSlotIndex;
	pMsg->nTemplateID = nTemplateID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_QUEST_RECYCLE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSetQuestFailed(CRole* poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	G2C_SET_QUEST_FAILED* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SET_QUEST_FAILED*)m_poPacketAllocator->Alloc(sizeof(G2C_SET_QUEST_FAILED));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_set_quest_failed;

	pMsg->nQuestID = nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_SET_QUEST_FAILED));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoOpenDialog(CRole * poRole, UINT32 dwNpcID, const char* pcszContent)
{
	INT32 nRetCode = 0;
    G2C_INTERACT_RESULT* pMsg = NULL;
    INT32 nStringLen = (INT32)strnlen(pcszContent, MAX_DIALOG_CONTENT_LEN);
    INT32 nSize = sizeof(G2C_INTERACT_RESULT) + nStringLen + 1;

    LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(nStringLen <= MAX_CHAT_MESSAGE_LEN);

	pMsg = (G2C_INTERACT_RESULT*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_interact_result;

	pMsg->dwNpcID = dwNpcID;
	pMsg->wSize = nSize;
	strxcpy(pMsg->szDialogContent, pcszContent, nStringLen + 1);

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncQuestList(CRole* poRole)
{
	INT32 nRetCode = 0;
    G2C_SYNC_QUEST_LIST* pMsg = NULL;
    CQuestList& rQuestList = poRole->GetQuestList();
    UINT32 dwSize = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_QUEST_LIST*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_quest_list;

    nRetCode = rQuestList.Save(pMsg->szData, dwSize);
    LOG_PROCESS_ERROR(nRetCode);

    pMsg->wSize = (UINT16)dwSize + sizeof(G2C_SYNC_QUEST_LIST);

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


BOOL CCliMessageHandler::DoSyncSceneVariables(CRole* poRole, CScene* poScene)
{
    INT32 nRetCode = 0;
    G2C_SYNC_SCENE_VARIABLES* pMsg = NULL;
    INT32 nVarCount = 0;

    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(poScene);

    pMsg = (G2C_SYNC_SCENE_VARIABLES*)m_poPacketAllocator->Alloc(
        sizeof(G2C_SYNC_SCENE_VARIABLES) + sizeof(G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA) * MAX_SCENE_VARIABLE_COUNT);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_scene_variables;

    for(INT32 nIndex = 0; nIndex < MAX_SCENE_VARIABLE_COUNT; ++nIndex)
    {
        INT32 nValue = poScene->GetVariable(nIndex);
        if (nValue != 0)
        {
            pMsg->VarList[nVarCount].wVarIndex = nIndex;
            pMsg->VarList[nVarCount].nValue = nValue;
            nVarCount++;
        }
    }
    pMsg->wSize = (UINT16)(sizeof(G2C_SYNC_SCENE_VARIABLES) + sizeof(G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA) * nVarCount);

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncRoleVariables(CRole* poRole)
{
    INT32 nRetCode = 0;
    G2C_SYNC_ROLE_VARIABLES* pMsg = NULL;
    INT32 nVarCount = 0;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_SYNC_ROLE_VARIABLES*)m_poPacketAllocator->Alloc(
        sizeof(G2C_SYNC_ROLE_VARIABLES) + sizeof(G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA) * MAX_ROLE_VARIABLE_COUNT);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_role_variables;

    for(INT32 nIndex = 0; nIndex < MAX_ROLE_VARIABLE_COUNT; ++nIndex)
    {
        INT32 nValue = poRole->GetVariable(nIndex);
        if (nValue != 0)
        {
            pMsg->VarList[nVarCount].wVarIndex = nIndex;
            pMsg->VarList[nVarCount].nValue = nValue;
            nVarCount++;
        }
    }
    pMsg->wSize = (UINT16)(sizeof(G2C_SYNC_ROLE_VARIABLES) + sizeof(G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA) * nVarCount);

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncClientVariables(CRole* poRole)
{
    INT32 nRetCode = 0;
    G2C_SYNC_CLIENT_VARIABLES* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_SYNC_CLIENT_VARIABLES*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_CLIENT_VARIABLES));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_client_variables;
    for (INT32 nIndex = 0; nIndex < MAX_CLIENT_VARIABLE_COUNT; nIndex++)
    {
        pMsg->nVariableList[nIndex] = poRole->GetClientVariable(nIndex);
    }

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_CLIENT_VARIABLES));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoUpdateQuestVariable(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID)
{
    INT32 nRetCode = 0;
    G2C_UPDATE_VARIABLE* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_UPDATE_VARIABLE*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_VARIABLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_update_variable;
    pMsg->byType = vtQuest;
    pMsg->wIndex = nVarIndex;
    pMsg->nValue = nValue;
    pMsg->nReferID = nQuestID;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_UPDATE_VARIABLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCliMessageHandler::DoUpdateRoleVariable(CRole* poRole, INT32 nVarIndex)
{
    INT32 nRetCode = 0;
    G2C_UPDATE_VARIABLE* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_UPDATE_VARIABLE*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_VARIABLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_update_variable;
    pMsg->byType = vtRole;
    pMsg->wIndex = nVarIndex;
    pMsg->nValue = poRole->GetVariable(nVarIndex);
    pMsg->nReferID = poRole->GetObjID();

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_UPDATE_VARIABLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCliMessageHandler::DoUpdateKillNpcCount(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID)
{
    INT32 nRetCode = 0;
    G2C_UPDATE_VARIABLE* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_UPDATE_VARIABLE*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_VARIABLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_update_variable;
    pMsg->byType = vtKillNpcCount;
    pMsg->wIndex = nVarIndex;
    pMsg->nValue = nValue;
    pMsg->nReferID = nQuestID;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_UPDATE_VARIABLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCliMessageHandler::DoUpdateKillPlayerCount(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_VARIABLE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UPDATE_VARIABLE*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_VARIABLE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_variable;
	pMsg->byType = vtKillPlayerCount;
	pMsg->wIndex = nVarIndex;
	pMsg->nValue = nValue;
	pMsg->nReferID = nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_UPDATE_VARIABLE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoUpdateKillItemNpcCount(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_VARIABLE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UPDATE_VARIABLE*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_VARIABLE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_variable;
	pMsg->byType = vtKillItemNpcCount;
	pMsg->wIndex = nVarIndex;
	pMsg->nValue = nValue;
	pMsg->nReferID = nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_UPDATE_VARIABLE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoUpdateKillBufPlayerCount(CRole* poRole, INT16 nQuestID, INT32 nCount)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_KILL_BUF_PLAYER_COUNT* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UPDATE_KILL_BUF_PLAYER_COUNT*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_KILL_BUF_PLAYER_COUNT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_kill_buf_player_count;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_UPDATE_KILL_BUF_PLAYER_COUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoUpdateKillDoodadCount(CRole* poRole, INT32 nVarIndex, INT32 nValue, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_VARIABLE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UPDATE_VARIABLE*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_VARIABLE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_variable;
	pMsg->byType = vtKillDoodad;
	pMsg->wIndex = nVarIndex;
	pMsg->nValue = nValue;
	pMsg->nReferID = nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_UPDATE_VARIABLE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoUpdateSceneVariable(CScene* poScene, INT32 nVarIndex)
{
    INT32 nRetCode = 0;
    G2C_UPDATE_VARIABLE* pMsg = NULL;

    pMsg = (G2C_UPDATE_VARIABLE*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_VARIABLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_update_variable;
    pMsg->byType = (UINT8)vtScene;
    pMsg->wIndex = (UINT16)nVarIndex;
    pMsg->nReferID = poScene->GetSceneTempID();
    pMsg->nValue = poScene->GetVariable(nVarIndex);

    nRetCode = _BroadcastScene(poScene, (PACKET_BUFFER*)pMsg, sizeof(G2C_UPDATE_VARIABLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoShareQuest(CRole* poRole, INT32 nQuestID, INT32 nSrcID)
{
	INT32 nRetCode = 0;
	G2C_SHARE_QUEST * pMsg = NULL;

	pMsg = (G2C_SHARE_QUEST*)m_poPacketAllocator->Alloc(sizeof(G2C_SHARE_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_share_quest;
	pMsg->nQuestID = nQuestID;
	pMsg->nSrcID = nSrcID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_SHARE_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoPublishQuest(CRole* poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	G2C_PUBLISH_QUEST* pMsg = NULL;

	pMsg = (G2C_PUBLISH_QUEST*)m_poPacketAllocator->Alloc(sizeof(G2C_PUBLISH_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_publish_quest;
	pMsg->nQuestID = nQuestID;
	
	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_SHARE_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncKillNpcQuest(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_KILL_NPC_QUEST* pMsg = NULL;
	
	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_KILL_NPC_QUEST*)m_poPacketAllocator->Alloc(
		sizeof(G2C_SYNC_KILL_NPC_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_kill_npc_quest;
	for (INT32 nQuestID = 0; nQuestID < MAX_KILL_NPC_QUEST_COUNT; nQuestID++)
	{
		pMsg->wKillCount[nQuestID] = poRole->GetKillNpcQuest().GetKillCount(nQuestID);
		pMsg->byCompleteList[nQuestID] = poRole->GetKillNpcQuest().GetCompleteState(nQuestID);
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_SYNC_KILL_NPC_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CCliMessageHandler::DoSyncKillNpcCount(CRole* poRole, INT32 nQuestID, INT32 nKillCount)
{
	INT32 nRetCode = 0;
	G2C_SYNC_KILL_NPC_COUNT* pMsg = (G2C_SYNC_KILL_NPC_COUNT*)m_poPacketAllocator->Alloc(
		sizeof(G2C_SYNC_KILL_NPC_COUNT));

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_sync_kill_npc_count;
	pMsg->wQuestID = (UINT16)nQuestID;
	pMsg->wKillCount = (UINT16)nKillCount;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_SYNC_KILL_NPC_COUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CCliMessageHandler::DoCompleteKillNpcQuest(CRole* poRole, INT32 nQuestID, INT32 nIndex)
{
	INT32 nRetCode = 0;
	G2C_COMPLETE_KILL_NPC_QUEST* pMsg = (G2C_COMPLETE_KILL_NPC_QUEST*)m_poPacketAllocator->Alloc(
		sizeof(G2C_COMPLETE_KILL_NPC_QUEST));

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_complete_kill_npc_quest;
	pMsg->wQuestID = (UINT16)nQuestID;
	pMsg->byIndex = (UINT8)nIndex;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_COMPLETE_KILL_NPC_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CCliMessageHandler::DoResetKillNpcCount(CRole* poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	G2C_RESET_KILL_NPC_QUEST* pMsg = (G2C_RESET_KILL_NPC_QUEST*)m_poPacketAllocator->Alloc(
		sizeof(G2C_RESET_KILL_NPC_QUEST));

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_reset_kill_npc_quest;
	pMsg->wQuestID = (UINT16)nQuestID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_RESET_KILL_NPC_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CCliMessageHandler::DoMSPublishQuest(CRole* poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	InternalMessage::G2M_PUBLISH_QUEST* pMsg = NULL;

	pMsg = (InternalMessage::G2M_PUBLISH_QUEST*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_PUBLISH_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_publish_quest;
	pMsg->nQuestID = nQuestID;
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->nNum = poRole->GetMaster().GetStudentCount();
	LOG_PROCESS_ERROR(pMsg->nNum>=0 && pMsg->nNum<MAX_STUDENT_NUM);

	for (INT32 nIndex =0; nIndex < pMsg->nNum; nIndex++)
	{
		pMsg->dwStudentList[nIndex] = poRole->GetMaster().GetStudentByIndex(nIndex);
	}

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_PUBLISH_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSAdventureTrigger(UINT32 dwRoleID, INT32 nAdventureID, INT32 nVarID, INT32 nMaxCount, INT32 nChance)
{
    INT32 nRetCode = 0;
    InternalMessage::G2M_ADVENTURE_TRIGGER* pMsg = 
        (InternalMessage::G2M_ADVENTURE_TRIGGER*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ADVENTURE_TRIGGER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_adventure_trigger;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nAdventureID = nAdventureID;
    pMsg->nVarID = nVarID;
    pMsg->nMaxCount = nMaxCount;
    pMsg->nChance = nChance;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ADVENTURE_TRIGGER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CCliMessageHandler::DoMSSendAward2Ump(UINT32 dwRoleID, INT32 nAwardType, char* pAwardNumber)
{
	INT32   nRetCode = 0;

	InternalMessage::G2M_SEND_AWARD_TO_UMP *pMsg = (InternalMessage::G2M_SEND_AWARD_TO_UMP*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_SEND_AWARD_TO_UMP));
	LOG_PROCESS_ERROR(pMsg);
	LOG_PROCESS_ERROR(pAwardNumber);

	pMsg->wMsg = InternalMessage::g2m_send_award_to_ump;
	pMsg->dwRoleID = dwRoleID;
	pMsg->nAwardType = nAwardType;
	strxcpy(pMsg->chAwardNumber, pAwardNumber, 32);

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_SEND_AWARD_TO_UMP));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSQueryRoleLocation(CRole* poRole, const char* pcszRoleName, BOOL bFromItem)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_QUERY_ROLE_LOCATION* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(pcszRoleName);

    pMsg = (InternalMessage::G2M_QUERY_ROLE_LOCATION*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_QUERY_ROLE_LOCATION));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_query_role_location;
    pMsg->dwRoleID = poRole->GetObjID();
    pMsg->dwClientSessionID = poRole->GetSessionID();
    strxcpy(pMsg->szRoleName, pcszRoleName, ROLE_NAME_LEN);
	pMsg->bFromItem = bFromItem;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_QUERY_ROLE_LOCATION));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSTakeRole(CRole* poRole, const char* pcszRoleName)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_TAKE_ROLE* pMsg = NULL;
    CScene* poScene = NULL;

    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(pcszRoleName);

    poScene = poRole->GetScene();
    LOG_PROCESS_ERROR(poScene);

    pMsg = (InternalMessage::G2M_TAKE_ROLE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_TAKE_ROLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_take_role;
    strxcpy(pMsg->szRoleName, pcszRoleName, ROLE_NAME_LEN);
    pMsg->nSceneTempID = poScene->GetSceneTempID();
    pMsg->nSceneID = poScene->GetSceneID();
    pMsg->nX = poRole->GetX();
    pMsg->nY = poRole->GetY();
    pMsg->nZ = poRole->GetZ();

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_TAKE_ROLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

#endif  // _ROBOT
