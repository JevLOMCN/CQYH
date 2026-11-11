#include "stdafx.h"
#include "quest_list.h"

#include <list>
#include <time.h>

#include "id_def.h"
#include "error_def.h"
#include "npc_data.h"
#include "spell_config.h"
#include "game_world.h"
#include "item_bag.h"
#include "message_handler.h"
#include "statistic.h"
#include "script_mgr.h"
#include "game_world.h"
#include "zone_def.h"
#include "grid_def.h"
#include "variable_def.h"
#include "circle_quest_data.h"
#include "circle_quest_mgr.h"
#include "daily_quest_mgr.h"
#include "server_config.h"

CQuestList::CQuestList()
{
	m_pOwner = NULL;

	m_dwRepeatQuestScanTime = 0;

	memset(m_byNormalQuest, 0, sizeof(m_byNormalQuest));
	memset(m_byRepeatQuest, 0, sizeof(m_byRepeatQuest));

	m_GroupMutex = 0;
	memset(m_stCircleQuest, 0, sizeof(m_stCircleQuest));

	m_nRewardIndex = 0;				
	m_qwRewardQuest = 0;
	m_qwRewardAccepted = 0;							
	m_qwRewardCompleted = 0;						
	m_dwRewardBoughtTime = 0;							
	m_dwRewardQuestBrushCDTime = 0;
	m_nRewardCompleteCount = 0;
	m_nRewardQuestRemainCompleteTime = 0;
	m_nDailyRewwardBoughtTime = 0;

	m_bRecodeKillPlayer = FALSE;
	m_nItemRecycleVarIndex = 0;

	m_dwCircleIndex = 0;

	m_nCompleteEmergencyQuestCount = 0;
	m_dwLastCompleteEmergencyQuestTime = 0;
}

CQuestList::~CQuestList()
{
	UnInit();
}

BOOL CQuestList::Init(CRole* pOwner)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pOwner);

	memset(m_byNormalQuest, 0, sizeof(m_byNormalQuest));
	memset(m_byRepeatQuest, 0, sizeof(m_byRepeatQuest));
	m_AcceptedNormalQuestList.clear();
	m_AcceptedEmergencyQuestList.clear();

	m_dwRepeatQuestScanTime = 0;
	m_pOwner = pOwner;

	m_GroupMutex = 0;
	memset(m_stCircleQuest, 0, sizeof(m_stCircleQuest));

	m_nRewardIndex = 0;								
	m_qwRewardQuest = 0;
	m_qwRewardAccepted = 0;
	m_qwRewardCompleted = 0;
	m_dwRewardBoughtTime = 0;
	m_dwRewardQuestBrushCDTime = 0;
	m_nRewardCompleteCount = 0;
	m_nRewardQuestRemainCompleteTime = 0;
	m_nDailyRewwardBoughtTime = 0;

	m_nItemRecycleVarIndex = 0;

	m_dwCircleIndex = 0;

	m_nCompleteEmergencyQuestCount = 0;
	m_dwLastCompleteEmergencyQuestTime = 0;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CQuestList::UnInit(void)
{
	INT32 nRetCode = 0;

	m_pOwner = NULL;

	return TRUE;
}

/***************************** normal quest *******************************************/

INT32 CQuestList::CanAccept(INT32 nQuestID, INT32 nAcceptType, INT32 nSrcID)
{
    const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(nQuestID);
    LOG_PROCESS_ERROR(pQuestData);

    return CanAccept(pQuestData, nAcceptType, nSrcID);

Exit0:
    return errUnknownError;
}

INT32 CQuestList::CanAccept(const QUEST_DATA* pQuestData, INT32 nAcceptType, INT32 nSrcID)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	time_t llCurrentTime = (time_t)CGameWorld::GetInstance().GetGameTime();
    const tm_ex& rcLocalTime = CGameWorld::GetInstance().GetLocalTime();

	LOG_PROCESS_ERROR(pQuestData);
	LOG_PROCESS_ERROR(m_pOwner);

	PROCESS_ERROR_RET_CODE(GetAcceptedQuest(pQuestData->nQuestID) == NULL, errQuestAlreadyAccepted);

	if (!_IsEmergencyQuest(pQuestData->nQuestID))
	{
		PROCESS_ERROR_RET_CODE(m_AcceptedNormalQuestList.size() < MAX_ACCEPTED_NORMAL_QUEST_COUNT, errQuestLogFull);
	}

	if (pQuestData->nQuestID >= 0 && pQuestData->nQuestID < MAX_NORMAL_QUEST_COUNT)
	{
		LOG_PROCESS_ERROR(pQuestData->nMaxCompletedCount == 1);
		PROCESS_ERROR_RET_CODE(!IsQuestCompleted(pQuestData->nQuestID), errQuestAlreadyCompleted);
	}
	else if (pQuestData->nQuestID >= REPEAT_QUEST_START_ID && pQuestData->nQuestID < REPEAT_QUEST_START_ID + MAX_REPEAT_QUEST_COUNT)
	{
		LOG_PROCESS_ERROR_RET_CODE(m_byRepeatQuest[pQuestData->nQuestID - REPEAT_QUEST_START_ID] < pQuestData->nMaxCompletedCount, errQuestAlreadyCompleted);
	}
	else if (pQuestData->nQuestID >= CIRCLE_QUEST_START_ID && pQuestData->nQuestID < CIRCLE_QUEST_START_ID + MAX_CIRCLE_QUEST_COUNT)
	{
		const CIRCLE_QUEST_DATA *pCircleQuestData = NULL; 

		LOG_PROCESS_ERROR((pQuestData->nCircleID > 0) &&(pQuestData->nCircleID < MAX_CIRCLE_QUEST_COUNT));

		pCircleQuestData =  CDataManager<CIRCLE_QUEST_DATA>::GetInstance().GetData(MAKE_CIRCLE_QUEST_KEY(pQuestData->nCircleID, 
			(m_stCircleQuest[pQuestData->nCircleID-1].nCompleteCount+1), m_pOwner->GetLevel()));
		LOG_PROCESS_ERROR_RET_CODE(pCircleQuestData, errQuestCircleDailyComplete);			// If not found, today’s cycle limit has been reached

		LOG_PROCESS_ERROR(pQuestData->nQuestID == m_stCircleQuest[pQuestData->nCircleID-1].nQuestID);
		LOG_PROCESS_ERROR(m_stCircleQuest[pQuestData->nCircleID-1].nCompleteCount < pCircleQuestData->nMaxCirclePerDay);
		LOG_PROCESS_ERROR(m_stCircleQuest[pQuestData->nCircleID-1].nCircleNumber < pCircleQuestData->nMaxQuestPerNum);
	}
	else if (pQuestData->nQuestID >= REWARD_QUEST_START_ID && pQuestData->nQuestID < REWARD_QUEST_START_ID + MAX_REWARD_QUEST_COUNT)
	{
		INT32 nIndex = CDailyQuestMgr::GetInstance().GetIndexByQuestID(m_nRewardIndex, pQuestData->nQuestID);
		LOG_PROCESS_ERROR(nIndex != -1);

		LOG_PROCESS_ERROR((m_qwRewardQuest & ((UINT64)1<<nIndex)));
		LOG_PROCESS_ERROR((m_qwRewardAccepted & ((UINT64)1<<nIndex)) == 0);
		LOG_PROCESS_ERROR((m_qwRewardCompleted & ((UINT64)1<<nIndex)) == 0);

		PROCESS_ERROR_RET_CODE((m_dwRewardBoughtTime + m_nRewardQuestRemainCompleteTime) > 0, errQuestDailyCompleteTimeOut);	
		PROCESS_ERROR_RET_CODE(_GetAcceptedRewardQuestCount() < (m_dwRewardBoughtTime + m_nRewardQuestRemainCompleteTime), errQuestDailyCompleteTimeOut);
		PROCESS_ERROR_RET_CODE(m_nRewardCompleteCount < MAX_REWARD_QUEST_DAILY_COMPLETE_TIME, errQuestRewardQuestComplete);
	}
	else if (pQuestData->nQuestID >= EMERGENCY_QUEST_START_ID && pQuestData->nQuestID < EMERGENCY_QUEST_START_ID + MAX_EMERGENCY_QUEST_COUNT)
	{
		// donothing
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

	// Any value that is not a player class (the data uses QUEST_ANY_CLASS = 8, as in the 1.05 tables) means no class restriction.
	if (pQuestData->nClassLimit >= clsPlayerBegin && pQuestData->nClassLimit < clsPlayerEnd)
	{
		PROCESS_ERROR_RET_CODE(m_pOwner->GetClass() == pQuestData->nClassLimit, errQuestClass);
	}

	if (pQuestData->nSexLimit != cstInvalid)
	{
		PROCESS_ERROR_RET_CODE(m_pOwner->GetSex() == pQuestData->nSexLimit, errQuestSex);
	}

	if (pQuestData->nRelationLimit != qrtInvalid)
	{
		switch (pQuestData->nRelationLimit)
		{
		case qrtMaster:
			{
				LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetMaster().GetTeacherID() != INVALID_OBJECT_ID, errQuestNoMaster);
			}
			break;
		case qrtDisciple:
			break;
		case qrtHusband:
			break;
		case qrtWife:
			break;
		case qrtGuildLeader:
			break;
		case qrtGuildMember:
			break;
		case qrtTeamMaster:
			{
				nRetCode = m_pOwner->HasMasterRelationshipInTeam();
				PROCESS_ERROR_RET_CODE(nRetCode, errQuestNoMaster);
			}
			break;
		case qrtTeamHusband:
			break;
		case qrtTeamGuild:
			break;
		default:
			LOG_PROCESS_ERROR(FALSE);
			break;
		}
	}

	if (pQuestData->nPreQuestID1 != INVALID_OBJECT_ID)
	{
		PROCESS_ERROR_RET_CODE(IsQuestCompleted(pQuestData->nPreQuestID1), errQuestNotCompletePreQuest);
	}

	if (pQuestData->nPreQuestID2 != INVALID_OBJECT_ID)
	{
		PROCESS_ERROR_RET_CODE(IsQuestCompleted(pQuestData->nPreQuestID2), errQuestNotCompletePreQuest);
	}

	if (nAcceptType == qatNpc)
	{
		if (pQuestData->nStartNpcID != INVALID_OBJECT_ID)
		{
			PROCESS_ERROR_RET_CODE(CheckNpc(pQuestData->nStartNpcID, nAcceptType), errQuestNpcTooFar);
		}
	}
	else if (nAcceptType == qatShare)
	{
		CRole* poSharer = CGameWorld::GetInstance().FindValidRole(nSrcID);
		PROCESS_ERROR_RET_CODE(poSharer, errQuestNoSharer);

		LOG_PROCESS_ERROR(pQuestData->bCanShare);
		PROCESS_ERROR_RET_CODE(poSharer->GetSceneID() == m_pOwner->GetSceneID(), errQuestShareNotSameScene);
		PROCESS_ERROR_RET_CODE(((m_pOwner->GetTeamID() != 0) && (m_pOwner->GetTeamID() == poSharer->GetTeamID())), errQuestNoShareTeam);
	}
	else if (nAcceptType == qatEmegency)
	{
		UINT32 dwCompleteDay = (m_dwLastCompleteEmergencyQuestTime + CGameWorld::GetInstance().GetTimeZone()) / 86400;
		UINT32 dwCurrentDay = (CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone()) / 86400;
		if (dwCompleteDay != dwCurrentDay)
		{
			m_nCompleteEmergencyQuestCount = 0;
			m_dwLastCompleteEmergencyQuestTime = CGameWorld::GetInstance().GetGameTime();
		}
		PROCESS_ERROR_RET_CODE(m_nCompleteEmergencyQuestCount < MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY, errQuestMaxCompleted);
	}
	else if (nAcceptType == qatPublish)
	{
		// Do Nothing
	}
    else if (nAcceptType == qatItem)
    {
        // Do Nothing
    }
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

	if (pQuestData->nStartItemID != INVALID_OBJECT_ID)
	{
		LOG_PROCESS_ERROR(m_pOwner->GetItemBag()->GetItemCount(pQuestData->nStartItemID) > 0);
	}

	if (pQuestData->nYearLimit != 0)
	{
		PROCESS_ERROR_RET_CODE(pQuestData->nYearLimit == rcLocalTime.tm_year + 1900, errQuestTime);
	}

	if (pQuestData->nMonthLimit != -1)
	{
		PROCESS_ERROR_RET_CODE(pQuestData->nMonthLimit == rcLocalTime.tm_mon + 1, errQuestTime);
	}

	if (pQuestData->nStartWeekday != -1)
	{
		INT32 nStartWday = pQuestData->nStartWeekday;
		INT32 nEndWday = (pQuestData->nStartWeekday + pQuestData->nWeekdayDuration) % 7;
		if (nStartWday <= nEndWday)
		{
			PROCESS_ERROR_RET_CODE(rcLocalTime.tm_wday >= nStartWday && rcLocalTime.tm_wday <= nEndWday, errQuestTime);
		}
		else
		{
			PROCESS_ERROR_RET_CODE(rcLocalTime.tm_wday >= nStartWday || rcLocalTime.tm_wday <= nEndWday, errQuestTime);
		}
	}
	else if (pQuestData->nStartMonthday != 0)
	{
		INT32 nStartMday = pQuestData->nStartMonthday;
		INT32 nEndMday = pQuestData->nStartMonthday + pQuestData->nMonthdayDuration;
		PROCESS_ERROR_RET_CODE(rcLocalTime.tm_mday >= nStartMday && rcLocalTime.tm_mday <= nEndMday, errQuestTime);
	}
	else if (pQuestData->dwAcceptStartTime != -1)
	{
		INT32 nDayTime = CGameWorld::GetInstance().GetGameTime() % 86400;
		PROCESS_ERROR_RET_CODE(CGameWorld::GetInstance().GetGameTime() >= pQuestData->dwAcceptStartTime, errQuestTime);
		PROCESS_ERROR_RET_CODE(CGameWorld::GetInstance().GetGameTime() <= pQuestData->dwAcceptEndTime, errQuestTime);
		PROCESS_ERROR_RET_CODE(nDayTime >= pQuestData->nAcceptStartDayTime, errQuestTime);
		PROCESS_ERROR_RET_CODE(nDayTime <= pQuestData->nAcceptEndDayTime, errQuestTime);
	}

	if (pQuestData->nMinLevelLimit != 0)
	{
		PROCESS_ERROR_RET_CODE(m_pOwner->GetLevel() >= pQuestData->nMinLevelLimit, errQuestLevel);
	}

	if (pQuestData->nMaxLevelLimit != 0)
	{
		PROCESS_ERROR_RET_CODE(m_pOwner->GetLevel() <= pQuestData->nMaxLevelLimit, errQuestLevel);
	}

	if (pQuestData->nBuffIDLimit != INVALID_OBJECT_ID)
	{
		CAuraInstance* pAuraInstance = m_pOwner->GetFirstAura(pQuestData->nBuffIDLimit);
		PROCESS_ERROR_RET_CODE(pAuraInstance, errQuestBuffNotFound);
		//PROCESS_ERROR_RET_CODE(pAuraInstance->GetLevel() == pQuestData->nBuffLevelLimit, errQuestBuffNotFound);
	}

	if (pQuestData->nMoneyTypeLimit != MONEYTYPE_INVALID)
	{
		INT32 nMoneyAmount = m_pOwner->GetItemBag()->GetMoney(pQuestData->nMoneyTypeLimit);
		PROCESS_ERROR_RET_CODE(nMoneyAmount >= pQuestData->nMoneyAmountLimit, errQuestNotEnoughMoney);
	}

	if (pQuestData->nReputationForceID != INVALID_FORCE_ID)
	{
		INT32 nReputation = m_pOwner->GetReputation().GetValue(pQuestData->nReputationForceID);
		if (pQuestData->nReputationMinLimit)
		{
			PROCESS_ERROR_RET_CODE(nReputation >= pQuestData->nReputationMinLimit, errQuestLowReputation);
		}
		if (pQuestData->nReputationMaxLimit)
		{
			PROCESS_ERROR_RET_CODE(nReputation <= pQuestData->nReputationMaxLimit, errQuestHighReputation);
		}
	}

	for (INT32 nItemIndex = 0; nItemIndex < MAX_CONDITION_PER_QUEST; nItemIndex++)
	{
		if (pQuestData->vecItemLimitID[nItemIndex] != 0)
		{
			INT32 nItemCount = m_pOwner->GetItemBag()->GetItemCount(pQuestData->vecItemLimitID[nItemIndex]);
			PROCESS_ERROR_RET_CODE(nItemCount >= pQuestData->vecItemLimitCnt[nItemIndex], errQuestNotEnoughItem);
		}
	}

	if (pQuestData->nLimitVarID1 != INVALID_VARIABLE_ID)
	{
		PROCESS_ERROR_RET_CODE(_CheckVariable(NULL, pQuestData->nLimitVarID1, pQuestData->nLimitVarValue1), errQuestLimitVar);
	}

	if (pQuestData->nLimitVarID2 != INVALID_VARIABLE_ID)
	{
		PROCESS_ERROR_RET_CODE(_CheckVariable(NULL, pQuestData->nLimitVarID2, pQuestData->nLimitVarValue2), errQuestLimitVar);
	}

	if (pQuestData->nOfferMoneyType != MONEYTYPE_INVALID)
	{
		INT32 nMoneyAmount = m_pOwner->GetItemBag()->GetMoney(pQuestData->nOfferMoneyType);
		if (pQuestData->nOfferMoneyAmount > 0)
		{
			PROCESS_ERROR_RET_CODE(nMoneyAmount + pQuestData->nOfferMoneyAmount <= MAX_MONEY, errQuestTooMuchMoney);
		}
		else if (pQuestData->nOfferMoneyAmount < 0)
		{
			PROCESS_ERROR_RET_CODE(nMoneyAmount + pQuestData->nOfferMoneyAmount >= 0, errQuestNotEnoughMoney);
		}
	}

	if (pQuestData->nOfferItemID != INVALID_OBJECT_ID)
	{
		const ITEM_DATA* pItem = GetItemData(pQuestData->nOfferItemID);
		LOG_PROCESS_ERROR(pItem);		
		LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetItemBag()->GetFreeSlots(CONSLOT_DEFAULT) > 1, errQuestItemBagFull);
	}

	// Call script
	if ( '\0' != pQuestData->szBeforeAcceptScriptName[0])
	{
		INT32 nRetCode = 0;
		CLuaScript* poScript = CScriptMgr::GetInstance().FindScript(QUEST_SCRIPT_NAME);
		LOG_PROCESS_ERROR_RET_CODE(poScript, errQuestCannotFindScript);

		nRetCode = poScript->CallFunction(pQuestData->szBeforeAcceptScriptName, "o:d", m_pOwner, "CRole", &nResult);
		LOG_PROCESS_ERROR(nRetCode);
		LOG_PROCESS_ERROR(nResult == errSuccess);
	}

	nResult = errSuccess;
Exit0:
	return nResult;
}

INT32 CQuestList::Accept(INT32 nQuestID)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	const QUEST_DATA * pQuestData = NULL;
	ACCEPTED_QUEST AcceptedQuest;

	LOG_PROCESS_ERROR(m_pOwner);

	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	memset(&AcceptedQuest, 0, sizeof(AcceptedQuest));
	AcceptedQuest.nQuestID = nQuestID;
	AcceptedQuest.dwAcceptTime = CGameWorld::GetInstance().GetGameTime();

	if (!_IsEmergencyQuest(nQuestID))
	{
		m_AcceptedNormalQuestList.push_back(AcceptedQuest);
	}
	else
	{
		if (m_AcceptedEmergencyQuestList.size() >= MAX_ACCEPTED_EMERGENCY_QUEST_COUNT)
		{
            nRetCode = CanAbandon(nQuestID);
            LOG_PROCESS_ERROR(nRetCode);
			nRetCode = Abandon(nQuestID);
			LOG_PROCESS_ERROR(nRetCode);
		}

		m_AcceptedEmergencyQuestList.push_back(AcceptedQuest);
	}

#ifndef _ROBOT

	if (nQuestID >= REWARD_QUEST_START_ID && nQuestID < REWARD_QUEST_START_ID + MAX_REWARD_QUEST_COUNT)
	{
		INT32 nIndex = CDailyQuestMgr::GetInstance().GetIndexByQuestID(m_nRewardIndex, pQuestData->nQuestID);
		LOG_PROCESS_ERROR(nIndex != -1);

		m_qwRewardAccepted |= (UINT64)1<<nIndex;
	}

	// offer
	if (pQuestData->nOfferMoneyType != MONEYTYPE_INVALID && pQuestData->nOfferMoneyAmount != 0)
	{
		if (pQuestData->nOfferMoneyAmount > 0)
		{
			m_pOwner->GetItemBag()->AddMoney(pQuestData->nOfferMoneyType, pQuestData->nOfferMoneyAmount, MONEYLOG_QUEST_PREPAY, 0);
			CStatistic::GetInstance().OnAddMoney(m_pOwner, avdQuestMoneyTotal, pQuestData->nOfferMoneyType, pQuestData->nOfferMoneyAmount);
		}
		else
		{
			m_pOwner->GetItemBag()->SubMoney(pQuestData->nOfferMoneyType, -pQuestData->nOfferMoneyAmount, MONEYLOG_QUEST_COST, 0);
		}
	}

	if (pQuestData->nOfferBuffID != INVALID_OBJECT_ID)
	{
		CAura* pAura;
		CAuraInstance* pAuraInstance;

		pAura = CSpellConfig::GetInstance().FindAuraTemplate(pQuestData->nOfferBuffID);
		LOG_PROCESS_ERROR(pAura);

		pAuraInstance = m_pOwner->AppendAura(pAura, m_pOwner, m_pOwner->GetObjID(), NULL, 0, 0, 0, 0);
		LOG_PROCESS_ERROR(pAuraInstance);
	}

	if (pQuestData->nOfferItemID != INVALID_OBJECT_ID)
	{
		nRetCode = m_pOwner->GetItemBag()->AddItem(pQuestData->nOfferItemID, pQuestData->nOfferItemCnt, ITEMLOG_QUEST_PREPAY, 0, FALSE);
		LOG_PROCESS_ERROR(nRetCode == errSuccess);
	}

	for (INT32 nIndex = 0; nIndex < CHECK_KILL_PLAYER_CONDITION; nIndex++)
	{
		if (pQuestData->nCheckKillPlayerCount[nIndex] != 0)
		{
			m_bRecodeKillPlayer = TRUE;
		}
	}

	for (INT32 nCheckVarIndex = 0; nCheckVarIndex < MAX_CHECK_VAR_COUNT; nCheckVarIndex++)
	{

		if (pQuestData->nCheckVarID[nCheckVarIndex] != INVALID_VARIABLE_ID)
		{
			nRetCode = _SetItemRecycleVariable(pQuestData->nCheckVarID[nCheckVarIndex]);
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

	// Call script
	if ( '\0' != pQuestData->szAfterAcceptScriptName[0])
	{
		INT32 nRetCode = 0;
		CLuaScript* poScript = CScriptMgr::GetInstance().FindScript(QUEST_SCRIPT_NAME);
		LOG_PROCESS_ERROR_RET_CODE(poScript, errQuestCannotFindScript);

		nRetCode = poScript->CallFunction(pQuestData->szAfterAcceptScriptName, "o", m_pOwner, "CRole");
		LOG_PROCESS_ERROR(nRetCode);
	}

	if (!m_pOwner->IsLoading())
	{
		nRetCode = CCliMessageHandler::GetInstance().DoAcceptQuest(m_pOwner, nQuestID);
		LOG_PROCESS_ERROR(nRetCode);
	}
	
	nRetCode = CCliMessageHandler::GetInstance().DoDLAcceptQuest(m_pOwner, nQuestID);
	LOG_CHECK_ERROR(nRetCode);

#endif

	nResult = errSuccess;
Exit0:
	return nResult;
}

INT32 CQuestList::CanComplete(INT32 nQuestID)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	UINT32 dwRewardDefaultItemCount = 0;

	const QUEST_REWARD_DATA* pReward = NULL;
	const QUEST_DATA* pQuestData = NULL; 
	const ACCEPTED_QUEST* pAcceptedQuest = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	pAcceptedQuest = GetAcceptedQuest(nQuestID);
	LOG_PROCESS_ERROR_RET_CODE(pAcceptedQuest, errQuestAlreadyCompleted);

	pReward = _GetQuestReward(pQuestData);
	LOG_PROCESS_ERROR(pReward);

	if (pQuestData->nQuestID >= 0 && pQuestData->nQuestID < MAX_NORMAL_QUEST_COUNT)
	{
		LOG_PROCESS_ERROR((m_byNormalQuest[pQuestData->nQuestID/8] & ((UINT8)1<<(pQuestData->nQuestID%8))) == 0);
	}
	else if (pQuestData->nQuestID >= REPEAT_QUEST_START_ID && pQuestData->nQuestID < REPEAT_QUEST_START_ID + MAX_REPEAT_QUEST_COUNT)
	{
		LOG_PROCESS_ERROR(pQuestData->nQuestID < REPEAT_QUEST_START_ID + MAX_REPEAT_QUEST_COUNT);

		INT32 nCompletedCount = m_byRepeatQuest[pQuestData->nQuestID - REPEAT_QUEST_START_ID];
		PROCESS_ERROR_RET_CODE(nCompletedCount < pQuestData->nMaxCompletedCount, errQuestMaxCompleted);
	}
	else if (pQuestData->nQuestID >= CIRCLE_QUEST_START_ID && pQuestData->nQuestID < CIRCLE_QUEST_START_ID + MAX_CIRCLE_QUEST_COUNT)
	{
		LOG_PROCESS_ERROR((pQuestData->nCircleID > 0) &&(pQuestData->nCircleID <= MAX_CIRCLE_QUEST_COUNT));
		const CIRCLE_QUEST_DATA *pCircleQuestData =  CDataManager<CIRCLE_QUEST_DATA>::GetInstance().GetData(MAKE_CIRCLE_QUEST_KEY(pQuestData->nCircleID, 
			(m_stCircleQuest[pQuestData->nCircleID-1].nCompleteCount+1), m_pOwner->GetLevel()));

		LOG_PROCESS_ERROR(pCircleQuestData);
		LOG_PROCESS_ERROR(pQuestData->nQuestID == m_stCircleQuest[pQuestData->nCircleID-1].nQuestID);
		LOG_PROCESS_ERROR(m_stCircleQuest[pQuestData->nCircleID-1].nCompleteCount < pCircleQuestData->nMaxCirclePerDay);
		LOG_PROCESS_ERROR(m_stCircleQuest[pQuestData->nCircleID-1].nCircleNumber < pCircleQuestData->nMaxQuestPerNum);
	}
	else if (pQuestData->nQuestID >= REWARD_QUEST_START_ID && pQuestData->nQuestID < REWARD_QUEST_START_ID + MAX_REWARD_QUEST_COUNT)
	{
		LOG_PROCESS_ERROR_RET_CODE((m_dwRewardBoughtTime + m_nRewardQuestRemainCompleteTime) > 0, errQuestDailyCompleteTimeOut);		
	}
	else if (pQuestData->nQuestID >= EMERGENCY_QUEST_START_ID && pQuestData->nQuestID < EMERGENCY_QUEST_START_ID + MAX_EMERGENCY_QUEST_COUNT)
	{

	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

	if (pQuestData->nQuestType != qtInvalid)
	{
		LOG_PROCESS_ERROR_RET_CODE(_CheckType(pQuestData->nQuestType), errQuestDailyCompleteFull);

		if (pQuestData->nQuestType == qtMaster)
		{
			CTeamInGS* pTeam = NULL;
			INT64 llShareDistance2 = g_GWConfig.nExpShareDistance * POINT_PER_CM * g_GWConfig.nExpShareDistance * POINT_PER_CM;
			BOOL bWithMaster = FALSE;

			PROCESS_ERROR(m_pOwner->GetTeamID() != INVALID_OBJECT_ID);

			pTeam = CGameWorld::GetInstance().FindTeam(m_pOwner->GetTeamID());
			LOG_PROCESS_ERROR(pTeam);

			for (INT32 nMemberIndex = 0; nMemberIndex < pTeam->GetMaxMemberCount(); nMemberIndex++)
			{
				CRole* poTeamMember = CGameWorld::GetInstance().FindValidRole(pTeam->GetMemberTable(nMemberIndex).dwRoleID);

				if (poTeamMember && poTeamMember->GetScene() == m_pOwner->GetScene() && m_pOwner->GetObjID() != poTeamMember->GetObjID())
				{
					if (m_pOwner->GetDistance3D(poTeamMember) <= llShareDistance2)
					{
						if (m_pOwner->IsMasterRelationship(poTeamMember))
						{
							bWithMaster = TRUE;
							break;
						}
					}
				}
			}
			PROCESS_ERROR(bWithMaster);
		}
	}

	if (pQuestData->nDurationLimit != 0)
	{
		LOG_PROCESS_ERROR_RET_CODE((CGameWorld::GetInstance().GetGameTime() - pAcceptedQuest->dwAcceptTime) <= pQuestData->nDurationLimit, errQuestTimeLimit); 
	}

	if (pQuestData->nCheckLevel != 0)
	{
		LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetLevel() >= pQuestData->nCheckLevel, errQuestCheckLevel); 
	}

	if (pQuestData->nTalkNpcID != INVALID_OBJECT_ID)
	{
		PROCESS_ERROR_RET_CODE(pQuestData->nTalkVarID != INVALID_VARIABLE_ID, errQuestVarIndex);
		PROCESS_ERROR_RET_CODE(pAcceptedQuest->nVariable[pQuestData->nTalkVarID] == 1, errQuestTalkNotCompleted);
	}

	if (pQuestData->nExploreVarID != -1)
	{
		PROCESS_ERROR_RET_CODE(pQuestData->nExploreVarID != INVALID_VARIABLE_ID, errQuestVarIndex);
		PROCESS_ERROR_RET_CODE(pAcceptedQuest->nVariable[pQuestData->nExploreVarID] == 1, errQuestExplore);
	}

	// Any value that is not a player class (the data uses QUEST_ANY_CLASS = 8, as in the 1.05 tables) means no class restriction.
	if (pQuestData->nClassLimit >= clsPlayerBegin && pQuestData->nClassLimit < clsPlayerEnd)
	{
		PROCESS_ERROR_RET_CODE(m_pOwner->GetClass() == pQuestData->nClassLimit, errQuestClass);
	}

	if (pQuestData->nSexLimit != cstInvalid)
	{
		PROCESS_ERROR_RET_CODE(m_pOwner->GetSex() == pQuestData->nSexLimit, errQuestSex);
	}

	if (pQuestData->nFinishNpc != INVALID_OBJECT_ID)
	{
		PROCESS_ERROR_RET_CODE(CheckNpc(pQuestData->nFinishNpc, qatNpc), errQuestNpcTooFar);
	}

	if (pQuestData->nCheckBuffID != INVALID_OBJECT_ID)
	{
		CAuraInstance* pAuraInstance = m_pOwner->GetFirstAura(pQuestData->nCheckBuffID);
		PROCESS_ERROR_RET_CODE(pAuraInstance, errQuestBuffNotFound);
	}

	if (pQuestData->nFinishItemID != INVALID_OBJECT_ID)
	{
		LOG_PROCESS_ERROR(m_pOwner->GetItemBag()->GetItemCount(pQuestData->nFinishItemID) > 0);
	}

	for (INT32 nNpcIndex = 0; nNpcIndex < MAX_CONDITION_PER_QUEST; nNpcIndex++)
	{
		if (pQuestData->nCheckKillNpcID[nNpcIndex] != INVALID_OBJECT_ID)
		{
			INT32 nKillNpcCount = pAcceptedQuest->nKillNpcCount[nNpcIndex];
			PROCESS_ERROR_RET_CODE(nKillNpcCount >= pQuestData->nCheckKillNpcCnt[nNpcIndex], errQuestNotEnoughKill);
		}
	}

	for (INT32 nDoodadIndex = 0; nDoodadIndex < MAX_CONDITION_PER_QUEST; nDoodadIndex++)
	{
		if (pQuestData->nCheckKillDoodadID[nDoodadIndex] != INVALID_OBJECT_ID)
		{
			INT32 nDoodadCount = pAcceptedQuest->nKillDoodadCount[nDoodadIndex];
			PROCESS_ERROR_RET_CODE(nDoodadCount >= pQuestData->nCheckKillDoodadCnt[nDoodadIndex], errQuestDoodad);
		}
	}

	for (INT32 nItemIndex = 0; nItemIndex < MAX_CONDITION_PER_QUEST; nItemIndex++)
	{
		if (pQuestData->nCheckKillItemNpcID[nItemIndex] != INVALID_OBJECT_ID)
		{
			PROCESS_ERROR_RET_CODE(pAcceptedQuest->nCheckItemCount[nItemIndex] >= pQuestData->nCheckKillItemCnt[nItemIndex], errQuestNotEnoughItem);
		}

		if (pQuestData->nCheckItemID[nItemIndex] != INVALID_OBJECT_ID)
		{
			PROCESS_ERROR_RET_CODE(m_pOwner->GetItemBag()->GetItemCount(pQuestData->nCheckItemID[nItemIndex]) >= pQuestData->nCheckItemCount[nItemIndex], errQuestNotEnoughItem);
		}
	}

	if (pQuestData->nCheckReputationForceID != INVALID_FORCE_ID)
	{
		INT32 nReputation = m_pOwner->GetReputation().GetValue(pQuestData->nCheckReputationForceID);

		PROCESS_ERROR_RET_CODE(nReputation >= pQuestData->nCheckReputationForceValue, errQuestLowReputation);
	}

	for (INT32 nCheckVarIndex = 0; nCheckVarIndex < MAX_CHECK_VAR_COUNT; nCheckVarIndex++)
	{
		if (pQuestData->nCheckVarID[nCheckVarIndex] != INVALID_VARIABLE_ID)
		{
			PROCESS_ERROR_RET_CODE(_CheckVariable(pAcceptedQuest, pQuestData->nCheckVarID[nCheckVarIndex], pQuestData->nCheckVarValue[nCheckVarIndex]), errQuestLimitVar);
		}
	}

	if (pReward->nAwardMoneyType1 != MONEYTYPE_INVALID && pReward->nAwardMoneyAmount1 != 0)
	{
		INT32 nMoneyAmount = m_pOwner->GetItemBag()->GetMoney(pReward->nAwardMoneyType1);
		PROCESS_ERROR_RET_CODE(nMoneyAmount + pReward->nAwardMoneyAmount1 <= MAX_MONEY, errQuestTooMuchMoney);
	}

	if (pReward->nAwardMoneyType2 != MONEYTYPE_INVALID && pReward->nAwardMoneyAmount2 != 0)
	{
		INT32 nMoneyAmount = m_pOwner->GetItemBag()->GetMoney(pReward->nAwardMoneyType2);
		PROCESS_ERROR_RET_CODE(nMoneyAmount + pReward->nAwardMoneyAmount2 <= MAX_MONEY, errQuestTooMuchMoney);
	}

	for (INT32 nItemIndex = 0; nItemIndex < MAX_REWARD_PER_QUEST; nItemIndex++)
	{
		if (pReward->vecAwardItemID[nItemIndex] != INVALID_OBJECT_ID)
		{
			const ITEM_DATA* pItem = GetItemData(pReward->vecAwardItemID[nItemIndex]);
			LOG_PROCESS_ERROR(pItem);

			dwRewardDefaultItemCount++;
		}
	}

	for(INT32 nIndex = 0; nIndex < CHECK_KILL_PLAYER_CONDITION; nIndex++)
	{
		if (pQuestData->nCheckKillPlayerCount[nIndex] != 0)
		{
			LOG_PROCESS_ERROR_RET_CODE(pAcceptedQuest->nKillPlayerCount[nIndex] >= pQuestData->nCheckKillPlayerCount[nIndex], errQuestKillPlayer);
		}

		if (pQuestData->nCheckKillBufPlayerBufID[nIndex] != 0)
		{
			PROCESS_ERROR_RET_CODE(pAcceptedQuest->nKillBufPlayerCount[nIndex] >= pQuestData->nCheckKillBufPlayerCount[nIndex], errQuestKillBufPlayer);
		}
	}

	PROCESS_ERROR_RET_CODE(dwRewardDefaultItemCount <= m_pOwner->GetItemBag()->GetFreeSlots(CONSLOT_DEFAULT), errQuestItemBagFull);

	// Call script
	if ( '\0' != pQuestData->szBeforeCompleteScriptName[0])
	{
		INT32 nRetCode = 0;
		CLuaScript* poScript = CScriptMgr::GetInstance().FindScript(QUEST_SCRIPT_NAME);
		LOG_PROCESS_ERROR_RET_CODE(poScript, errQuestCannotFindScript);

		nRetCode = poScript->CallFunction(pQuestData->szBeforeCompleteScriptName, "o:d", m_pOwner, "CRole", &nResult);
		LOG_PROCESS_ERROR(nRetCode);
		LOG_PROCESS_ERROR(nResult == errSuccess);
	}

	nResult = errSuccess;
Exit0:
	return nResult;
}

INT32 CQuestList::Complete(INT32 nQuestID, INT32 nSelectedAwardIdx)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	INT32 nSelectAwardItemTemplateID = 0;
	INT32 nSelectAwardItemCount = 0;
	const QUEST_DATA* pQuestData = NULL;
	const QUEST_REWARD_DATA* pRewardData = NULL;
	ACCEPTED_QUEST_LIST::iterator it;
	CItemBag* pItemBag = m_pOwner->GetItemBag();

	LOG_PROCESS_ERROR(m_pOwner);

	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	pRewardData = _GetQuestReward(pQuestData);
	LOG_PROCESS_ERROR(pRewardData);

	LOG_PROCESS_ERROR(pRewardData);

	if (pRewardData->bNeedSelectAward)
	{
		INT32 nSelectIndex = 0;
		LOG_PROCESS_ERROR(nSelectedAwardIdx >= 0 && nSelectedAwardIdx < MAX_SELECTABLE_REWARD_COUNT);
		LOG_PROCESS_ERROR(pRewardData->vecSelectableAwardItemID[nSelectedAwardIdx] != INVALID_OBJECT_ID);

		const ITEM_DATA* pNeedSelectItemData = GetItemData(pRewardData->vecSelectableAwardItemID[nSelectedAwardIdx]);
		LOG_PROCESS_ERROR(pNeedSelectItemData);

		if (pNeedSelectItemData->m_byRequireClass & (1 << m_pOwner->GetClass()))
		{
			nSelectAwardItemTemplateID = pNeedSelectItemData->m_nTemplateID;
			LOG_PROCESS_ERROR_RET_CODE(nSelectAwardItemTemplateID != 0, errQuestSelectItemClass);

			nSelectAwardItemCount = pRewardData->vecSelectableAwardItemCnt[nSelectedAwardIdx];

			LOG_PROCESS_ERROR_RET_CODE(nSelectAwardItemCount <= m_pOwner->GetItemBag()->GetFreeSlots(CONSLOT_DEFAULT), errQuestItemBagFull);
		}


//		for (INT32 nIndex = 0; nIndex < MAX_SELECTABLE_REWARD_COUNT; nIndex++)
//		{
//			const ITEM_DATA* pNeedSelectItemData = GetItemData(pRewardData->vecSelectableAwardItemID[nIndex]);
//			LOG_PROCESS_ERROR(pNeedSelectItemData);			
//
//			if (pNeedSelectItemData->m_byRequireClass & (1 << m_pOwner->GetClass()))
//			{
//				nSelectIndex++;
//			}
//
//			if (nSelectIndex == (nSelectedAwardIdx+1))
//			{
//				nSelectAwardItemTemplateID = pNeedSelectItemData->m_nTemplateID;
//				nSelectAwardItemCount = pRewardData->vecSelectableAwardItemCnt[nIndex];
//
//				LOG_PROCESS_ERROR_RET_CODE(nSelectAwardItemCount <= m_pOwner->GetItemBag()->GetFreeSlots(CONSLOT_DEFAULT), errQuestItemBagFull);
//				break;
//			}
//		}
//
//		LOG_PROCESS_ERROR_RET_CODE(nSelectAwardItemTemplateID != 0, errQuestSelectItemClass);
	}

	it = _GetAcceptedQuest(nQuestID);
	if (_IsEmergencyQuest(nQuestID))
	{
		LOG_PROCESS_ERROR(!m_AcceptedEmergencyQuestList.empty());
		LOG_PROCESS_ERROR(it != m_AcceptedEmergencyQuestList.end());
	}
	else
	{
		LOG_PROCESS_ERROR(!m_AcceptedNormalQuestList.empty());
		LOG_PROCESS_ERROR(it != m_AcceptedNormalQuestList.end());
	}

	// Call script
	if ( '\0' != pQuestData->szAfterCompleteScriptName[0])
	{
		CLuaScript* poScript = CScriptMgr::GetInstance().FindScript(QUEST_SCRIPT_NAME);
		LOG_PROCESS_ERROR_RET_CODE(poScript, errQuestCannotFindScript);

		nRetCode = poScript->CallFunction(pQuestData->szAfterCompleteScriptName, "o", m_pOwner, "CRole");
		LOG_PROCESS_ERROR(nRetCode);
	}

	if (_IsEmergencyQuest(nQuestID))
	{
		m_AcceptedEmergencyQuestList.erase(it);
	}
	else
	{
		m_AcceptedNormalQuestList.erase(it);
	}

	if (pQuestData->nQuestID >= 0 && pQuestData->nQuestID < MAX_NORMAL_QUEST_COUNT)
	{
		m_byNormalQuest[pQuestData->nQuestID/8] |= (1 << (pQuestData->nQuestID%8));
	}
	else if (pQuestData->nQuestID >= EMERGENCY_QUEST_START_ID && pQuestData->nQuestID < EMERGENCY_QUEST_START_ID + MAX_EMERGENCY_QUEST_COUNT)
	{

	}
	else if (pQuestData->nQuestID >= REPEAT_QUEST_START_ID && pQuestData->nQuestID < REPEAT_QUEST_START_ID + MAX_REPEAT_QUEST_COUNT)
	{
		m_byRepeatQuest[pQuestData->nQuestID - REPEAT_QUEST_START_ID]++;
	}
	else if (pQuestData->nQuestID >= CIRCLE_QUEST_START_ID && pQuestData->nQuestID < CIRCLE_QUEST_START_ID + MAX_CIRCLE_QUEST_COUNT)
	{
		const CIRCLE_QUEST_DATA *pCircleQuestData =  CDataManager<CIRCLE_QUEST_DATA>::GetInstance().GetData(MAKE_CIRCLE_QUEST_KEY(pQuestData->nCircleID, 
			(m_stCircleQuest[pQuestData->nCircleID-1].nCompleteCount+1), m_pOwner->GetLevel()));

		LOG_PROCESS_ERROR(pCircleQuestData);

		if (m_stCircleQuest[pQuestData->nCircleID-1].nCircleNumber < pCircleQuestData->nMaxQuestPerNum-1)
		{
			m_stCircleQuest[pQuestData->nCircleID-1].nCircleNumber++; 
		}
		else
		{
			m_stCircleQuest[pQuestData->nCircleID-1].nCircleNumber = 0;
			m_stCircleQuest[pQuestData->nCircleID-1].nCompleteCount++;
		}

		if (m_stCircleQuest[pQuestData->nCircleID-1].nCompleteCount < pCircleQuestData->nMaxCirclePerDay)
		{
			nRetCode = GenerateCircleQuest(pQuestData->nCircleID);
			LOG_PROCESS_ERROR(nRetCode);
		}
		else
		{	
			m_stCircleQuest[pQuestData->nCircleID-1].nQuestID = 0;
		}
	}
	else if (pQuestData->nQuestID >= REWARD_QUEST_START_ID && pQuestData->nQuestID < REWARD_QUEST_START_ID + MAX_REWARD_QUEST_COUNT)
	{
		INT32 nIndex = CDailyQuestMgr::GetInstance().GetIndexByQuestID(m_nRewardIndex, pQuestData->nQuestID);

		m_qwRewardAccepted &= ~((UINT64)1<<nIndex);
		m_qwRewardCompleted |= ((UINT64)1<<nIndex);

		if (m_nRewardQuestRemainCompleteTime > 0)
		{
			m_nRewardQuestRemainCompleteTime--;
		}
		else
		{
			LOG_PROCESS_ERROR(m_dwRewardBoughtTime > 0);
			m_dwRewardBoughtTime--;		
		}

		m_nRewardCompleteCount++;
		m_pOwner->GetAchievementList().AddVariable(avdRewardQuestCompleteCount, 1);
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

	// delete
	if (pQuestData->nOfferItemID != INVALID_OBJECT_ID)
	{
		pItemBag->DeleteItemId(pQuestData->nOfferItemID, pQuestData->nOfferItemCnt, ITEMLOG_COMPLETE_QUEST, TRUE, 0);
	}

	if (pQuestData->nOfferBuffID != INVALID_OBJECT_ID)
	{
		CAuraInstance* pAuraInstance = m_pOwner->GetFirstAura(pQuestData->nOfferBuffID);
		if (pAuraInstance)
		{
			m_pOwner->ReduceAuraStack(pAuraInstance, pAuraInstance->GetStackCount());
		}
	}

	for (INT32 nIndex = 0; nIndex < CHECK_KILL_PLAYER_CONDITION; nIndex++)
	{
		if (pQuestData->nCheckKillPlayerCount[nIndex] != 0)
		{
			m_bRecodeKillPlayer = FALSE;
			
			m_pOwner->GetAchievementList().SetVariable(avdKillPlayerLevelDuarion1, 0);
			m_pOwner->GetAchievementList().SetVariable(avdKillPlayerLevelDuarion2, 0);
			m_pOwner->GetAchievementList().SetVariable(avdKillPlayerLevelDuarion3, 0);
		}
	}

	// award
	if (pRewardData->nAwardMoneyType1 != MONEYTYPE_INVALID && pRewardData->nAwardMoneyAmount1 != 0)
	{
		m_pOwner->GetItemBag()->AddMoney(pRewardData->nAwardMoneyType1, pRewardData->nAwardMoneyAmount1, MONEYLOG_QUEST_REWARD, 0);

		CStatistic::GetInstance().OnAddMoney(m_pOwner, avdQuestMoneyTotal, pRewardData->nAwardMoneyType1, pRewardData->nAwardMoneyAmount1);
	}

	if (pRewardData->nAwardMoneyType2 != MONEYTYPE_INVALID && pRewardData->nAwardMoneyAmount2 != 0)
	{
		m_pOwner->GetItemBag()->AddMoney(pRewardData->nAwardMoneyType2, pRewardData->nAwardMoneyAmount2, MONEYLOG_QUEST_REWARD, 0);

		CStatistic::GetInstance().OnAddMoney(m_pOwner, avdQuestMoneyTotal, pRewardData->nAwardMoneyType2, pRewardData->nAwardMoneyAmount2);
	}

	if (pRewardData->nAwardExp)
	{
		m_pOwner->AddExp(pRewardData->nAwardExp, FALSE, FALSE);
	}

	if (pRewardData->nAwardBuffID)
	{
		CAura* pAura;
		CAuraInstance* pAuraInstance;

		pAura = CSpellConfig::GetInstance().FindAuraTemplate(pRewardData->nAwardBuffID);
		LOG_PROCESS_ERROR(pAura);

		pAuraInstance = m_pOwner->AppendAura(pAura, m_pOwner, m_pOwner->GetObjID(), NULL, 0, 0, 0, 0);
		LOG_PROCESS_ERROR(pAuraInstance);
	}

	if (pRewardData->nAwardReputationForceID != INVALID_FORCE_ID)
	{
		m_pOwner->GetReputation().ModifyValue(pRewardData->nAwardReputationForceID, pRewardData->nAwardReputation);
	}

	if (pRewardData->nAwardTitleID != INVALID_OBJECT_ID)
	{
		nRetCode = m_pOwner->GetTitle().AddTitle(pRewardData->nAwardTitleID);
		LOG_PROCESS_ERROR(nRetCode);
	}

	if (pRewardData->nActivity)
	{
		m_pOwner->AddActivity(pRewardData->nActivity);
	}

	for (INT32 nItemIndex = 0; nItemIndex < MAX_REWARD_PER_QUEST; nItemIndex++)
	{
		if (pRewardData->vecAwardItemID[nItemIndex] != INVALID_OBJECT_ID)
		{
			nRetCode = m_pOwner->GetItemBag()->AddItem(pRewardData->vecAwardItemID[nItemIndex], 
				pRewardData->vecAwardItemCnt[nItemIndex], ITEMLOG_QUEST_REWARD, 0, FALSE);
			LOG_PROCESS_ERROR(nRetCode == errSuccess);
		}
	}

	for (INT32 nItemIndex = 0; nItemIndex < MAX_CONDITION_PER_QUEST; nItemIndex++)
	{
		if (pQuestData->nCheckItemID[nItemIndex] != INVALID_OBJECT_ID)
		{
			nRetCode = m_pOwner->GetItemBag()->DeleteItemId(
				pQuestData->nCheckItemID[nItemIndex], pQuestData->nCheckItemCount[nItemIndex], ITEMLOG_COMPLETE_QUEST, FALSE, 0);
			PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, errQuestCheckItem);
		}
	}

	if (pRewardData->bNeedSelectAward)
	{
		nRetCode = m_pOwner->GetItemBag()->AddItem(nSelectAwardItemTemplateID, nSelectAwardItemCount, ITEMLOG_QUEST_REWARD, 0, FALSE);
		LOG_PROCESS_ERROR(nRetCode == errSuccess);
	}

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoCompleteQuest(m_pOwner, nQuestID);
	LOG_PROCESS_ERROR(nRetCode);
#endif

	if (pQuestData->nQuestID >= REWARD_QUEST_START_ID && pQuestData->nQuestID < REWARD_QUEST_START_ID + MAX_REWARD_QUEST_COUNT)
	{
		if (_IsRewardQuestAllCompleted())
		{
			nRetCode = GenerateRewardQuest();
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

	if (pQuestData->nQuestType == qtEmegency)
	{
		m_nCompleteEmergencyQuestCount++;
		m_dwLastCompleteEmergencyQuestTime = CGameWorld::GetInstance().GetGameTime();
	}

	CStatistic::GetInstance().OnCompleteQuest(m_pOwner, pQuestData);

	nResult = errSuccess;
Exit0:
	return nResult;
}

INT32 CQuestList::CanAbandon(INT32 nQuestID)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    const QUEST_DATA* pQuestData = NULL;

    LOG_PROCESS_ERROR(m_pOwner);

    pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(nQuestID);
    LOG_PROCESS_ERROR(pQuestData);

    PROCESS_ERROR_RET_CODE(pQuestData->bCanAbandon, errQuestCanNotAbandon);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CQuestList::Abandon(INT32 nQuestID)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	const QUEST_DATA* pQuestData = NULL;
	ACCEPTED_QUEST_LIST::iterator it;
	CItemBag* pItemBag = m_pOwner->GetItemBag();

	LOG_PROCESS_ERROR(m_pOwner);
	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	// Call script
	if ( '\0' != pQuestData->szAbandonScriptName[0])
	{
		CLuaScript* poScript = CScriptMgr::GetInstance().FindScript(QUEST_SCRIPT_NAME);
		LOG_PROCESS_ERROR_RET_CODE(poScript, errQuestCannotFindScript);

		nRetCode = poScript->CallFunction(pQuestData->szAbandonScriptName, "o", m_pOwner, "CRole");
		LOG_PROCESS_ERROR(nRetCode);
	}

    it = _GetAcceptedQuest(nQuestID);
    if (_IsEmergencyQuest(nQuestID))
    {
        LOG_PROCESS_ERROR(!m_AcceptedEmergencyQuestList.empty());
        LOG_PROCESS_ERROR(it != m_AcceptedEmergencyQuestList.end());
    }
    else
    {
        LOG_PROCESS_ERROR(!m_AcceptedNormalQuestList.empty());
        LOG_PROCESS_ERROR(it != m_AcceptedNormalQuestList.end());
    }

	if (_IsEmergencyQuest(pQuestData->nQuestID))
	{
		m_AcceptedEmergencyQuestList.erase(it);
	}
	else
	{
		m_AcceptedNormalQuestList.erase(it);
	}

	if (pQuestData->nQuestID >= REWARD_QUEST_START_ID && pQuestData->nQuestID < REWARD_QUEST_START_ID + MAX_REWARD_QUEST_COUNT)
	{
		INT32 nIndex = CDailyQuestMgr::GetInstance().GetIndexByQuestID(m_nRewardIndex, pQuestData->nQuestID);
		LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_REWARD_QUEST_COUNT_PER_GROUP);

		m_qwRewardAccepted &= ~((UINT64)1<<nIndex);
	}

	if (pQuestData->nOfferBuffID != INVALID_OBJECT_ID)
	{
		CAuraInstance* pAuraInstance = m_pOwner->GetFirstAura(pQuestData->nOfferBuffID);
		if (pAuraInstance)
		{
			m_pOwner->ReduceAuraStack(pAuraInstance, pAuraInstance->GetStackCount());
		}
	}

	if (pQuestData->nOfferItemID != INVALID_OBJECT_ID)
	{
		pItemBag->DeleteItemId(pQuestData->nOfferItemID, pQuestData->nOfferItemCnt, ITEMLOG_COMPLETE_QUEST, TRUE, 0);
	}

	for (INT32 nIndex = 0; nIndex < CHECK_KILL_PLAYER_CONDITION; nIndex++)
	{
		if (pQuestData->nCheckKillPlayerCount[nIndex] != 0)
		{
			m_bRecodeKillPlayer = FALSE;

			m_pOwner->GetAchievementList().SetVariable(avdKillPlayerLevelDuarion1, 0);
			m_pOwner->GetAchievementList().SetVariable(avdKillPlayerLevelDuarion2, 0);
			m_pOwner->GetAchievementList().SetVariable(avdKillPlayerLevelDuarion3, 0);
		}
	}

	// Clear the flag for an item recycling quest

	for (INT32 nCheckVarIndex = 0; nCheckVarIndex < MAX_CHECK_VAR_COUNT; nCheckVarIndex++)
	{
		if (pQuestData->nCheckVarID[nCheckVarIndex] != INVALID_VARIABLE_ID)
		{
			nRetCode = _ClearItemRecycleVariable();
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

	m_pOwner->GetAchievementList().AddVariable(avdAbandonQuestCount, 1);

	nResult = errSuccess;
Exit0:
	return nResult;
}

BOOL CQuestList::DailyRefresh(void)
{
	INT32 nRetCode = 0;

	for(INT32 nIndex = 0; nIndex < MAX_REPEAT_QUEST_COUNT; nIndex++)
	{
		const QUEST_DATA* pQuest = CDataManager<QUEST_DATA>::GetInstance().GetData(REPEAT_QUEST_START_ID + nIndex);
		if (pQuest == NULL || pQuest->eResetType != qttDay)
			continue;

		LOG_PROCESS_ERROR(pQuest->nQuestID >= REPEAT_QUEST_START_ID);
		m_byRepeatQuest[pQuest->nQuestID - REPEAT_QUEST_START_ID] = 0;
	}
	
	m_nRewardQuestRemainCompleteTime = MAX_REWARD_QUEST_DAILY_COMPLETE_COUNT;
    m_nRewardQuestRemainCompleteTime += g_GWConfig.VIPData[m_pOwner->GetVIPLevel()].nRewardQuestCount;

	m_pOwner->GetAchievementList().SetVariable(avdRewardQuestCompleteCount, 0);
	m_dwRewardBoughtTime = 0;

	// reward quest
	if (m_pOwner->GetLevel() >= GENERATE_REWARD_QUEST_LEVEL)
	{
		GenerateRewardQuest();
		m_nRewardCompleteCount = 0;
		m_nDailyRewwardBoughtTime = 0;
	}
	
	// circle quest
	if (m_pOwner->GetLevel() >= GENERATE_CIRCLE_QUEST_LEVEL)
	{
		for (INT32 nIndex = 1; nIndex <= MAX_CIRCLE_COUNT; nIndex++)
		{
			const CIRCLE_QUEST_DATA* pCircleData = CDataManager<CIRCLE_QUEST_DATA>::GetInstance().GetData(MAKE_CIRCLE_QUEST_KEY(nIndex, 1, m_pOwner->GetLevel()));
			
 			if (pCircleData)
 			{
 				if (pCircleData->nResetTime == 1)
 				{
 					memset(&m_stCircleQuest[nIndex-1], 0, sizeof(m_stCircleQuest[nIndex-1]));
 					GenerateCircleQuest(nIndex);
 				}
 			}
		}

		m_dwCircleIndex = m_pOwner->GetLevel() / CIRCLE_QUEST_LEVEL_PER_GRADE;
	}

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoSyncQuestList(m_pOwner);
	LOG_PROCESS_ERROR(nRetCode);
#endif

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CQuestList::WeeklyRefresh(void)
{
	for (INT32 nIndex = 0; nIndex < MAX_REPEAT_QUEST_COUNT; nIndex++)
	{
		const QUEST_DATA* pQuest = CDataManager<QUEST_DATA>::GetInstance().GetData(REPEAT_QUEST_START_ID + nIndex);
		if (pQuest == NULL || pQuest->eResetType != qttWeek)
			continue;

		LOG_PROCESS_ERROR(pQuest->nQuestID >= REPEAT_QUEST_START_ID);
		m_byRepeatQuest[pQuest->nQuestID - REPEAT_QUEST_START_ID] = 0;
	}

	return TRUE;
Exit0:
	return FALSE;
}

const ACCEPTED_QUEST* CQuestList::GetAcceptedQuest(INT32 nQuestID)
{
	ACCEPTED_QUEST_LIST::iterator it = _GetAcceptedQuest(nQuestID);

	if (_IsEmergencyQuest(nQuestID))
	{
		PROCESS_ERROR(!m_AcceptedEmergencyQuestList.empty());
		PROCESS_ERROR(it != m_AcceptedEmergencyQuestList.end());
	}
	else
	{
		PROCESS_ERROR(!m_AcceptedNormalQuestList.empty());
		PROCESS_ERROR(it != m_AcceptedNormalQuestList.end());
	}

	return &(*it);
Exit0:
	return NULL;
}

BOOL CQuestList::IsQuestCompleted(INT32 nQuestID)
{
	if (nQuestID >= 0 && nQuestID < MAX_NORMAL_QUEST_COUNT)
	{
		return (m_byNormalQuest[nQuestID /  8] & (1 << (nQuestID % 8))) > 0;
	}
	else if (nQuestID >= MAX_NORMAL_QUEST_COUNT && nQuestID < MAX_NORMAL_QUEST_COUNT + MAX_REPEAT_QUEST_COUNT)
	{
		return m_byRepeatQuest[nQuestID - MAX_NORMAL_QUEST_COUNT] > 0;
	}

	return FALSE;
}

BOOL CQuestList::CheckNpc(INT32 nNpcTemplateID, INT32 nAcceptType)
{
	CNpc* pNpc = NULL;
	INT64 llInteractDistance = 0;

	if (nAcceptType == qatEmegency)
	{
		llInteractDistance = g_GWConfig.nEmergencyNpcInteractDistance* POINT_PER_CM;

		pNpc = CGameWorld::GetInstance().FindNpc(nNpcTemplateID);
		PROCESS_ERROR(pNpc);
	}
	else
	{
		llInteractDistance = g_GWConfig.nNpcInteractDistance * POINT_PER_CM;

		pNpc = CGameWorld::GetInstance().FindNpc(m_pOwner->GetInteractNpc());
		PROCESS_ERROR(pNpc);

		PROCESS_ERROR(pNpc->GetNpcData()->nTemplateID == nNpcTemplateID);
	}

	LOG_PROCESS_ERROR(m_pOwner);
	PROCESS_ERROR(pNpc->GetScene() == m_pOwner->GetScene());
	PROCESS_ERROR(pNpc->GetNpcData());
	PROCESS_ERROR(pNpc->GetDistance3D(m_pOwner) < llInteractDistance * llInteractDistance);

	return TRUE;
Exit0:
	return FALSE;
}

void CQuestList::SetQuestFailed(const ACCEPTED_QUEST* pAcceptedQuest)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pAcceptedQuest);

	((ACCEPTED_QUEST*)pAcceptedQuest)->dwAcceptTime = 0;

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoSetQuestFailed(m_pOwner, pAcceptedQuest->nQuestID);
	LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT

Exit0:
	return;
}


void CQuestList::SetVariable(const ACCEPTED_QUEST* pAcceptedQuest, INT32 nIndex, INT32 nValue)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pAcceptedQuest);
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_VARIABLE_PER_QUEST);

	((ACCEPTED_QUEST*)pAcceptedQuest)->nVariable[nIndex] = nValue;

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoUpdateQuestVariable(m_pOwner, nIndex, nValue, pAcceptedQuest->nQuestID);
	LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT
Exit0:
	return;
}

BOOL CQuestList::GetAcceptedQuestBegin()
{
	m_AcceptedQuest.clear();

	for (ACCEPTED_QUEST_LIST::iterator Iter = m_AcceptedNormalQuestList.begin(); Iter != m_AcceptedNormalQuestList.end(); Iter++)
	{
		m_AcceptedQuest.push_back(*Iter);
	}

	for (ACCEPTED_QUEST_LIST::iterator Iter = m_AcceptedEmergencyQuestList.begin(); Iter != m_AcceptedEmergencyQuestList.end(); Iter++)
	{
		m_AcceptedQuest.push_back(*Iter);
	}

	return TRUE;
}

INT32 CQuestList::GetNextAcceptedQuest()
{
	INT32 nQuestID = 0;

	if (m_AcceptedQuest.empty())
	{
		return INVALID_QUEST_ID;
	}
	else
	{
		ACCEPTED_QUEST* pAcceptedQuest = &m_AcceptedQuest.front();
		nQuestID = pAcceptedQuest->nQuestID;

		m_AcceptedQuest.pop_front();

		return nQuestID;
	}
}

INT32 CQuestList::GetRandomAcceptedQuest()
{
	INT32 nQuestID = INVALID_QUEST_ID;

	std::list<INT32> QuestList;

	if (!m_AcceptedNormalQuestList.empty())
	{
		for (ACCEPTED_QUEST_LIST::iterator Iter = m_AcceptedNormalQuestList.begin(); Iter != m_AcceptedNormalQuestList.end(); Iter++)
		{
			QuestList.push_back(Iter->nQuestID);
		}
	}

	if (!m_AcceptedEmergencyQuestList.empty())
	{
		for (ACCEPTED_QUEST_LIST::iterator Iter = m_AcceptedEmergencyQuestList.begin(); Iter != m_AcceptedEmergencyQuestList.end(); Iter++)
		{
			QuestList.push_back(Iter->nQuestID);
		}
	}

	if (QuestList.size() > 0)
	{
		INT32 nRankNum = g_Random(QuestList.size());
		INT32 nCount = 0;

		for (std::list<INT32>::iterator Iter = QuestList.begin(); Iter != QuestList.end(); Iter++, nCount++)
		{
			if (nCount >= nRankNum)
			{
				nQuestID = *Iter;
				break;
			}
		}
	}

	return nQuestID;
}

/******************************** special ***********************************/

void CQuestList::OnKillDoodad(UINT32 dwDoodadId)
{
	INT32 nRetCode = 0;

	PROCESS_SUCCESS(m_AcceptedNormalQuestList.empty() && m_AcceptedEmergencyQuestList.empty());

	for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedNormalQuestList.begin();
		it != m_AcceptedNormalQuestList.end(); ++it)
	{
		const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(it->nQuestID);
		LOG_PROCESS_ERROR(pQuestData);

		for (INT32 nIndex = 0; nIndex < MAX_CONDITION_PER_QUEST; nIndex++)
		{
			if (pQuestData->nCheckKillDoodadID[nIndex] == dwDoodadId)
			{
				if (it->nKillDoodadCount[nIndex] < pQuestData->nCheckKillDoodadCnt[nIndex])
				{
					it->nKillDoodadCount[nIndex]++;
#ifndef _ROBOT
					nRetCode = CCliMessageHandler::GetInstance().DoUpdateKillDoodadCount(
						m_pOwner, nIndex, it->nKillDoodadCount[nIndex], pQuestData->nQuestID);
					LOG_CHECK_ERROR(nRetCode);
#endif
				}
			}
		}
	}

	for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedEmergencyQuestList.begin();
		it != m_AcceptedEmergencyQuestList.end(); ++it)
	{
		const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(it->nQuestID);
		LOG_PROCESS_ERROR(pQuestData);

		for (INT32 nIndex = 0; nIndex < MAX_CONDITION_PER_QUEST; nIndex++)
		{
			if (pQuestData->nCheckKillDoodadID[nIndex] == dwDoodadId)
			{
				if (it->nKillDoodadCount[nIndex] < pQuestData->nCheckKillDoodadCnt[nIndex])
				{
					it->nKillDoodadCount[nIndex]++;
#ifndef _ROBOT
					nRetCode = CCliMessageHandler::GetInstance().DoUpdateKillDoodadCount(
						m_pOwner, nIndex, it->nKillDoodadCount[nIndex], pQuestData->nQuestID);
					LOG_CHECK_ERROR(nRetCode);
#endif
				}
			}
		}
	}

Exit1:
Exit0:
	return;
}

void CQuestList::OnKillNpc(CNpc* pNpc)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(pNpc);
	LOG_PROCESS_ERROR(pNpc->GetNpcData());

	for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedNormalQuestList.begin();
		it != m_AcceptedNormalQuestList.end(); ++it)
	{
		const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(it->nQuestID);
		LOG_PROCESS_ERROR(pQuestData);

		for (INT32 nIndex = 0; nIndex < MAX_CONDITION_PER_QUEST; nIndex++)
		{
			if (pQuestData->nCheckKillNpcID[nIndex] == pNpc->GetNpcData()->nTemplateID ||
				(pQuestData->nCheckKillNpcGroupID[nIndex] != 0 &&
				pQuestData->nCheckKillNpcGroupID[nIndex] == pNpc->GetNpcData()->nGroupID))
			{
				if (it->nKillNpcCount[nIndex] < pQuestData->nCheckKillNpcCnt[nIndex])
				{
					if ((pQuestData->nQuestType != qtMaster) || m_pOwner->HasMasterRelationshipInTeam())
					{
						it->nKillNpcCount[nIndex]++;

#ifndef _ROBOT
						nRetCode = CCliMessageHandler::GetInstance().DoUpdateKillNpcCount(
							m_pOwner, nIndex, it->nKillNpcCount[nIndex], pQuestData->nQuestID);
						LOG_CHECK_ERROR(nRetCode);
#endif
					}
				}
			}


			if (pQuestData->nCheckKillItemNpcID[nIndex] == pNpc->GetNpcData()->nTemplateID)
			{
				INT32 nRandomNum = 	g_Random(RATE_MAX);
				if (nRandomNum >= pQuestData->nCheckKillItemNpcRate[nIndex])
				{
					it->nCheckItemCount[nIndex]++;
#ifndef _ROBOT
					nRetCode = CCliMessageHandler::GetInstance().DoUpdateKillItemNpcCount(
						m_pOwner, nIndex, it->nCheckItemCount[nIndex], pQuestData->nQuestID);
					LOG_CHECK_ERROR(nRetCode);
#endif
				}
			}
		}
	}

	for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedEmergencyQuestList.begin();
		it != m_AcceptedEmergencyQuestList.end(); ++it)
	{
		const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(it->nQuestID);
		LOG_PROCESS_ERROR(pQuestData);

		for (INT32 nIndex = 0; nIndex < MAX_CONDITION_PER_QUEST; nIndex++)
		{
			if ((pQuestData->nCheckKillNpcID[nIndex] <= MAX_NPC_ID && pQuestData->nCheckKillNpcID[nIndex] == pNpc->GetNpcData()->nTemplateID) || 
                (pQuestData->nCheckKillNpcID[nIndex] > MAX_NPC_ID && pQuestData->nCheckKillNpcID[nIndex] == pNpc->GetNpcData()->nGroupID))
			{
				if (it->nKillNpcCount[nIndex] < pQuestData->nCheckKillNpcCnt[nIndex])
				{
					if ((pQuestData->nQuestType != qtMaster) || m_pOwner->HasMasterRelationshipInTeam())
					{
						it->nKillNpcCount[nIndex]++;
#ifndef _ROBOT
						nRetCode = CCliMessageHandler::GetInstance().DoUpdateKillNpcCount(
							m_pOwner, nIndex, it->nKillNpcCount[nIndex], pQuestData->nQuestID);
						LOG_CHECK_ERROR(nRetCode);
#endif
					}
				}
			}
		}
	}

Exit0:
	return;
}

void CQuestList::OnKillPlayer(INT32 nPlayerLevel, CAuraList* pAuraList)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pAuraList);

	for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedNormalQuestList.begin(); it != m_AcceptedNormalQuestList.end(); ++it)
	{
		nRetCode = _CheckKillPlayer(&(*it), nPlayerLevel, pAuraList);
		LOG_PROCESS_ERROR(nRetCode);
	}

	for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedEmergencyQuestList.begin(); it != m_AcceptedEmergencyQuestList.end(); ++it)
	{
		nRetCode = _CheckKillPlayer(&(*it), nPlayerLevel, pAuraList);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return;
Exit0:
	return;
}

INT32 CQuestList::GetRequireItemCount(INT32 nItemID)
{
	INT32 nRequireCount = 0;

// 	LOG_PROCESS_ERROR(nItemID != 0);
// 
// 	for (ACCEPTED_QUEST_LIST::const_iterator it = m_AcceptedNormalQuestList.begin(); it != m_AcceptedNormalQuestList.end(); ++it)
// 	{
// 		const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(it->nQuestID);
// 		if (pQuestData == NULL)
// 			continue;
// 
// 		for (INT32 nItemIndex = 0; nItemIndex < MAX_CONDITION_PER_QUEST; nItemIndex++)
// 		{
// 			if (pQuestData->nCheckItemID[nItemIndex] == nItemID && nRequireCount < pQuestData->nCheckItemCnt[nItemIndex])
// 				nRequireCount = pQuestData->nCheckItemCnt[nItemIndex];
// 		}
// 	}
// 
// 	for (ACCEPTED_QUEST_LIST::const_iterator it = m_AcceptedEmergencyQuestList.begin(); it != m_AcceptedEmergencyQuestList.end(); ++it)
// 	{
// 		const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(it->nQuestID);
// 		if (pQuestData == NULL)
// 			continue;
// 
// 		for (INT32 nItemIndex = 0; nItemIndex < MAX_CONDITION_PER_QUEST; nItemIndex++)
// 		{
// 			if (pQuestData->nCheckItemID[nItemIndex] == nItemID && nRequireCount < pQuestData->nCheckItemCnt[nItemIndex])
// 				nRequireCount = pQuestData->nCheckItemCnt[nItemIndex];
// 		}
// 	}
// 
// Exit0:
	return nRequireCount;
}

BOOL CQuestList::_CheckType(INT32 nQuestType)
{
	INT32 nRetCode = 0;

	switch(nQuestType)
	{
	case qtReward:
		LOG_PROCESS_ERROR(m_dwRewardBoughtTime + m_nRewardQuestRemainCompleteTime > 0);
		break;
	default:
		break;
	}

	return TRUE;
Exit0:
	return FALSE;
}

CQuestList::ACCEPTED_QUEST_LIST::iterator CQuestList::_GetAcceptedQuest(INT32 nQuestID)
{
	if (_IsEmergencyQuest(nQuestID))
	{
		for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedEmergencyQuestList.begin(); 
			it != m_AcceptedEmergencyQuestList.end(); ++it)
		{
			if (it->nQuestID == nQuestID)
			{
				return it;
			}
		}

		return m_AcceptedEmergencyQuestList.end();
	}
	else
	{
		for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedNormalQuestList.begin(); 
			it != m_AcceptedNormalQuestList.end(); ++it)
		{
			if (it->nQuestID == nQuestID)
			{
				return it;
			}
		}

		return m_AcceptedNormalQuestList.end();
	}
}

BOOL CQuestList::_CheckVariable(const ACCEPTED_QUEST* pAcceptedQuest, INT32 nVariableID, INT32 nValue)
{
	INT32 nIndex = GetVarIndexFromID(nVariableID);

	LOG_PROCESS_ERROR(m_pOwner);

	switch (GetVarTypeFromID(nVariableID))
	{
	case vtRole:
	case vtItemRecycle:
		PROCESS_ERROR(m_pOwner->GetVariable(nIndex) == nValue);
		break;
	case vtQuest:
		LOG_PROCESS_ERROR(pAcceptedQuest);
		PROCESS_ERROR(GetVariable(pAcceptedQuest, nIndex) == nValue);
		break;
	case vtScene:
		LOG_PROCESS_ERROR(m_pOwner->GetScene());
		PROCESS_ERROR(m_pOwner->GetScene()->GetVariable(nIndex) == nValue);
		break;
	case vtGuild:
		break;
	case vtClient:
		{
			// modified by zhangrui05
			// To conserve client variables, the client stores ECVT_Quest variables as bits; nValue identifies the bit, so check it bitwise
			PROCESS_ERROR((m_pOwner->GetClientVariable(nIndex) & (1 << nValue)) > 0);
		}
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return TRUE;
Exit0:
	return FALSE;
}

const QUEST_REWARD_DATA* CQuestList::_GetQuestReward(const QUEST_DATA* pQuestData)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	const QUEST_REWARD_DATA* pRewardData = NULL;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(pQuestData);	

	if (pQuestData->nQuestID >= CIRCLE_QUEST_START_ID && pQuestData->nQuestID < CIRCLE_QUEST_START_ID + MAX_CIRCLE_QUEST_COUNT)
	{
		INT32 nRewardIndex = 0;
		const CIRCLE_QUEST_DATA *pCircleQuestData = NULL;

		LOG_PROCESS_ERROR((pQuestData->nCircleID > 0) &&(pQuestData->nCircleID <= MAX_CIRCLE_QUEST_COUNT));

		pCircleQuestData =  CDataManager<CIRCLE_QUEST_DATA>::GetInstance().GetData(MAKE_CIRCLE_QUEST_KEY(pQuestData->nCircleID, 
			(m_stCircleQuest[pQuestData->nCircleID-1].nCompleteCount+1), m_pOwner->GetLevel()));
		LOG_PROCESS_ERROR(pCircleQuestData);

		nRewardIndex = m_stCircleQuest[pQuestData->nCircleID-1].nCircleNumber;
		LOG_PROCESS_ERROR(nRewardIndex >= 0 && nRewardIndex < MAX_QUEST_PER_CIRCLE);

		const CIRCLE_REWARD_DATA* pReward = CDataManager<CIRCLE_REWARD_DATA>::GetInstance().GetData(pCircleQuestData->nRewardGroupID[nRewardIndex]);
		LOG_PROCESS_ERROR(pReward);

		pRewardData = &(pReward->stQuestRewardData);
	}
	else
	{
		pRewardData = &(pQuestData->stRewardData);
	}

	return pRewardData;
Exit0:
	return NULL;
}

BOOL CQuestList::_CheckKillPlayer(ACCEPTED_QUEST* pAcceptQuest, INT32 nPlayerLevel, CAuraList* pAuraList)
{
	INT32 nRetCode = 0;

	const QUEST_DATA* pQuestData = NULL;

	LOG_PROCESS_ERROR(pAcceptQuest);
		
	pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pAcceptQuest->nQuestID);
	LOG_PROCESS_ERROR(pQuestData);

	for(INT32 nIndex = 0; nIndex < CHECK_KILL_PLAYER_CONDITION; nIndex++)
	{
		if ((pQuestData->nCheckKillPlayerCount[nIndex] != 0) && (pQuestData->nCheckKillPlayerLevlStart[nIndex] <= nPlayerLevel) && (pQuestData->nCheckKillPlayerLevlEnd[nIndex] >= nPlayerLevel))
		{
			pAcceptQuest->nKillPlayerCount[nIndex]++;

#ifndef _ROBOT
			nRetCode = CCliMessageHandler::GetInstance().DoUpdateKillPlayerCount(
				m_pOwner, nIndex, pAcceptQuest->nKillPlayerCount[nIndex], pAcceptQuest->nQuestID);
			LOG_CHECK_ERROR(nRetCode);
#endif
		}

		if (pQuestData->nCheckKillBufPlayerBufID[nIndex] != 0)
		{
			LOG_PROCESS_ERROR(pAuraList);

			for (INT32 nEffect = auetNone; nEffect < auetTotal; nEffect++)
			{
				CAuraInstance* pAuraInstance = pAuraList->BeginAura(nEffect);
				while(pAuraInstance)
				{
					if (pAuraInstance->GetID() == pQuestData->nCheckKillBufPlayerBufID[nIndex])
					{
						pAcceptQuest->nKillBufPlayerCount[nIndex]++;
#ifndef _ROBOT
						nRetCode = CCliMessageHandler::GetInstance().DoUpdateKillBufPlayerCount(m_pOwner, pAcceptQuest->nQuestID, pAcceptQuest->nKillBufPlayerCount[nIndex]);
						LOG_PROCESS_ERROR(nRetCode);
#endif

						break;
					}

					pAuraInstance = pAuraList->NextAura(nEffect, pAuraInstance);
				}
			}
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CQuestList::_SetItemRecycleVariable(INT32 nVariableID)
{
	INT32 nIndex = GetVarIndexFromID(nVariableID);

	switch (GetVarTypeFromID(nVariableID))
	{
	case vtRole:
	case vtQuest:
	case vtScene:
	case vtGuild:
	case vtClient:
		break;
	case vtItemRecycle:
		m_nItemRecycleVarIndex = nIndex;
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CQuestList::_ClearItemRecycleVariable()
{
	m_nItemRecycleVarIndex = 0;
	return TRUE;
}

ACCEPTED_QUEST* CQuestList::Begin()
{
	m_AcceptIteratQuestList.clear();

	for (ACCEPTED_QUEST_LIST::iterator iter = m_AcceptedNormalQuestList.begin(); iter != m_AcceptedNormalQuestList.end(); iter++)
	{
		m_AcceptIteratQuestList.push_back(*iter);
	}

	for (ACCEPTED_QUEST_LIST::iterator iter = m_AcceptedEmergencyQuestList.begin(); iter != m_AcceptedEmergencyQuestList.end(); iter++)
	{
		m_AcceptIteratQuestList.push_back(*iter);
	}


    m_AcceptedQuestCursor = m_AcceptIteratQuestList.begin();
    LOG_PROCESS_ERROR(m_AcceptedQuestCursor != m_AcceptIteratQuestList.end());

    return &(*m_AcceptedQuestCursor);
Exit0:
    return NULL;
}

const ACCEPTED_QUEST* CQuestList::Next()
{
    ++m_AcceptedQuestCursor;
    PROCESS_ERROR(m_AcceptedQuestCursor != m_AcceptIteratQuestList.end());

    return &(*m_AcceptedQuestCursor);
Exit0:
    return NULL;
}

/*********************** circle quest *****************************************/

INT32 CQuestList::GenerateCircleQuest(INT32 nCircleID)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(nCircleID > 0 && nCircleID <= MAX_CIRCLE_QUEST_COUNT);

	m_stCircleQuest[nCircleID-1].nQuestID = CCIRCLE_QUEST_MGR::GetInstance().GetRandomCircleQuest(nCircleID, m_pOwner->GetLevel()/5);

#ifndef _ROBOT
		nRetCode = CCliMessageHandler::GetInstance().DoNotifyNextCircleQuest(m_pOwner, nCircleID, m_stCircleQuest[nCircleID-1].nQuestID);
		LOG_PROCESS_ERROR(nRetCode);
#endif // _ROBOT

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CQuestList::GetCircleQuestID(INT32 nCircleID)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(nCircleID > 0 && nCircleID <= MAX_CIRCLE_QUEST_COUNT);

	return m_stCircleQuest[nCircleID-1].nQuestID;
Exit0:
	return INVALID_QUEST_ID;
}

/*********************** reward quest *****************************************/

INT32 CQuestList::GenerateRewardQuest(void)
{
	INT32 nRetCode = 0;
	INT32 nQuestCount = 0;
	
	ACCEPTED_QUEST_LIST::iterator Iter;	

	LOG_PROCESS_ERROR(m_pOwner);

	m_qwRewardQuest = 0;
	m_qwRewardAccepted = 0;
	m_qwRewardCompleted = 0;

	// Account for level-up changing the index by counting accepted but incomplete quests
	for (Iter = m_AcceptedNormalQuestList.begin(); Iter != m_AcceptedNormalQuestList.end(); Iter++)
	{
		const QUEST_DATA* pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(Iter->nQuestID);
		LOG_PROCESS_ERROR(pQuestData);

		if (pQuestData->nQuestType == qtReward)
		{
			INT32 nIndex = CDailyQuestMgr::GetInstance().GetIndexByQuestID(m_nRewardIndex, pQuestData->nQuestID);
			if (nIndex>=0 && nIndex<MAX_REWARD_QUEST_COUNT_PER_GROUP)
			{
				m_qwRewardQuest |= ((UINT64)1<<nIndex);
				m_qwRewardAccepted |= ((UINT64)1<<nIndex);
				nQuestCount++;
			}
		}
	}
	
	if (m_nRewardIndex != m_pOwner->GetLevel()/5) // Update index
	{
		memset(&m_qwRewardAccepted, 0, sizeof(m_qwRewardAccepted));
		m_nRewardIndex = m_pOwner->GetLevel()/5;
	}

	while (nQuestCount < MAX_REWARD_QUEST_COUNT_PER_ROLE)
	{
		INT32 nIndex = CDailyQuestMgr::GetInstance().GetRandomQuestIndex(m_nRewardIndex);		//An error means no bounty quests remain for this level
		PROCESS_ERROR(nIndex>=0 && nIndex<MAX_REWARD_QUEST_COUNT_PER_GROUP);

		if (!(m_qwRewardQuest & ((UINT64)1<<nIndex)) && !(m_qwRewardAccepted & ((UINT64)1<<nIndex)))
		{
			m_qwRewardQuest |= ((UINT64)1<<nIndex);

			nQuestCount++;
		}
	}

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoSyncDailyQuest(m_pOwner, TRUE);
	LOG_PROCESS_ERROR(nRetCode);
#endif // _ROBOT

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CQuestList::BuyRewardBoughtTime(INT32 nRewardBoughtTime)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	INT32 nTotalCost = 0;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR_RET_CODE((m_nDailyRewwardBoughtTime + nRewardBoughtTime) <= MAX_REWARD_QUEST_DAILY_COMPLETE_TIME, errQuestRewardBoughtTimeUp);
	
	for (INT32 nIndex = (m_nDailyRewwardBoughtTime +1); nIndex <= (m_nDailyRewwardBoughtTime + nRewardBoughtTime); nIndex++)
	{
		const REWARD_QUEST_MONEY_COST_DATA* pMoneyCostData = CDataManager<REWARD_QUEST_MONEY_COST_DATA>::GetInstance().GetData(nIndex);
		LOG_PROCESS_ERROR(pMoneyCostData);

		nTotalCost += pMoneyCostData->nMoneyCost;
	}

	LOG_PROCESS_ERROR_RET_CODE((m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_02) >= nTotalCost), errNotEnoughMoney);

	nRetCode = m_pOwner->GetItemBag()->SubMoney(MONEYTYPE_02, nTotalCost, MONEYLOG_DAILY_QUEST_BUY_COMPLETE_TIME, 0);
	LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	m_dwRewardBoughtTime += nRewardBoughtTime;
	m_nDailyRewwardBoughtTime += nRewardBoughtTime;

	m_pOwner->GetAchievementList().AddVariable(avdBuyRewardQuestCompleteCount, nRewardBoughtTime);

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoDailyQuestCompleteTimeChange(m_pOwner, m_dwRewardBoughtTime );
	LOG_PROCESS_ERROR(nRetCode);
#endif

	return errSuccess;
Exit0:
	return nResult;
}

BOOL CQuestList::_IsRewardQuestAllCompleted(void)
{
	INT32 nRetCode = 0;

	for (INT32 nIndex = 0; nIndex < MAX_REWARD_QUEST_COUNT_PER_GROUP; nIndex++)
	{
		if (m_qwRewardQuest & ((UINT64)1<<nIndex))
		{
			 PROCESS_ERROR((m_qwRewardCompleted & ((UINT64)1<<nIndex)));
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CQuestList::_GetAcceptedRewardQuestCount(void)
{
	INT32 nCount = 0;

	for (INT32 nIndex = 0; nIndex < MAX_REWARD_QUEST_COUNT_PER_GROUP; nIndex++)
	{
		if (m_qwRewardAccepted & ((UINT64)1<<nIndex))
		{
			nCount++;
		}
	}

	return nCount;
}