#include "stdafx.h"
#include "achievement_list.h"

#include "npc.h"
#include "item.h"
#include "role.h"
#include "spell.h"
#include "spell_config.h"
#include "friend.h"
#include "message_handler.h"
#include "achievement_data.h"
#include "game_world.h"
#include "item_bag.h"
#include "spirit.h"
#include "spirit_data.h"
#include "statistic.h"
#include "robot_message_handler.h"
#include "enum_string.h"

CAchievementsList::CAchievementsList()
{
	m_pOwner = NULL;
	m_nCompletedCount = 0;

	memset(m_wAchievement, 0, sizeof(m_wAchievement));
	memset(m_dwAchievementReward, 0, sizeof(m_dwAchievementReward));
	memset(m_nAchievementStatistics, 0, sizeof(m_nAchievementStatistics));
	memset(m_dwHandbook, 0, sizeof(m_dwHandbook));

	m_nCompletedCount = 0;
}

CAchievementsList::~CAchievementsList()
{

}

BOOL CAchievementsList::Init(CRole* poRole)
{
	m_pOwner = NULL;
	m_nCompletedCount = 0;

	memset(m_wAchievement, 0, sizeof(m_wAchievement));
	memset(m_dwAchievementReward, 0, sizeof(m_dwAchievementReward));
	memset(m_nAchievementStatistics, 0, sizeof(m_nAchievementStatistics));
	memset(m_dwHandbook, 0, sizeof(m_dwHandbook));

	m_pOwner = poRole;
	m_nCompletedCount = 0;

	return TRUE;
}

BOOL CAchievementsList::UnInit(void)
{
	m_pOwner = NULL;

	return TRUE;
}

BOOL CAchievementsList::_IsCompleted(INT32 nAchievementID)
{
	LOG_PROCESS_ERROR(nAchievementID >= 0 && nAchievementID < ACHIEVEMENT_MAX);

	return (m_wAchievement[nAchievementID] > 0) ? TRUE : FALSE;

Exit0:
	return FALSE;
}

INT32 CAchievementsList::Complete(INT32 nAchievementID)
{
	INT32 nRetCode = 0;     
	INT32 nResult = errUnknownError;
	const ACHIEVEMENT_DATA* pData = NULL;

	LOG_PROCESS_ERROR(nAchievementID >= 0 && nAchievementID < ACHIEVEMENT_MAX);
	LOG_PROCESS_ERROR(m_pOwner);

	pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(nAchievementID);
	LOG_PROCESS_ERROR_RET_CODE(pData, errUnknownError);

	m_wAchievement[nAchievementID] = CGameWorld::GetInstance().GetGameTime() / 86400;
	m_nCompletedCount++;

#ifndef _ROBOT
	if (pData->nAchievementPts > 0)
	{
		nRetCode = m_pOwner->GetItemBag()->AddMoney(MONEYTYPE_ACHIEVEMENTPTS, pData->nAchievementPts, MONEYLOG_ACHIEVEMENT_REWARD, 0);
		LOG_PROCESS_ERROR(nRetCode);
	}

	nRetCode = CCliMessageHandler::GetInstance().DoCompleteAchievement(m_pOwner, nAchievementID, m_wAchievement[nAchievementID]);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoDLCompleteAchievement(m_pOwner, nAchievementID);
	LOG_PROCESS_ERROR(nRetCode);
#else

	RobotAchievementCheckLog(FALSE, 0, 0, nAchievementID, FALSE);

#endif

	return errSuccess;

Exit0:
	return nResult;

}

void CAchievementsList::SetVariable(INT32 nIndex, INT32 nValue)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_ACHIEVEMENT_STATISTIC_COUNT);

	m_nAchievementStatistics[nIndex] = nValue;

#ifndef _ROBOT
	if (!m_pOwner->IsLoading())
	{
		nRetCode = CCliMessageHandler::GetInstance().DoUpdateAchievementVariable(m_pOwner, nIndex, m_nAchievementStatistics[nIndex]);
		LOG_PROCESS_ERROR(nRetCode);
	}
	
#endif  // _ROBOT

Exit0:
	return;
}

void CAchievementsList::AddVariable(INT32 nIndex, INT32 nCount)
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(m_pOwner);	
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_ACHIEVEMENT_STATISTIC_COUNT);

	if ((0x7FFFFFFF - m_nAchievementStatistics[nIndex]) > nCount)
	{
		m_nAchievementStatistics[nIndex] += nCount;
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

#ifndef _ROBOT

	nRetCode = CCliMessageHandler::GetInstance().DoUpdateAchievementVariable(m_pOwner, nIndex, m_nAchievementStatistics[nIndex]);
	LOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
	return;
}

INT32 CAchievementsList::CanReward(INT32 nAchievementID)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	INT32 nFreeSlotNeed = 0;
	const ACHIEVEMENT_DATA* pData = nullptr;

	LOG_PROCESS_ERROR(m_pOwner);

	pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(nAchievementID);
	LOG_PROCESS_ERROR(pData);

	LOG_PROCESS_ERROR_RET_CODE(m_wAchievement[nAchievementID] != 0, errAchievementNotCompleted);
	LOG_PROCESS_ERROR_RET_CODE((m_dwAchievementReward[nAchievementID / 32] & (1 << (nAchievementID % 32))) == 0, errAchievementRewarded);
	PROCESS_ERROR_RET_CODE(m_pOwner->GetItemBag()->GetFreeSlots(CONSLOT_DEFAULT) >= pData->nAwardSlotCount, errAchievementRewardBagFull);

	nResult = errSuccess;
Exit0:
	return nResult;
}

INT32 CAchievementsList::Reward(INT32 nAchievementID)
{
	INT32 nRetCode = 0;     
	INT32 nResult = errUnknownError;
	const ACHIEVEMENT_DATA* pData = NULL;

	LOG_PROCESS_ERROR(nAchievementID >= 0 && nAchievementID < ACHIEVEMENT_MAX);
	LOG_PROCESS_ERROR(m_pOwner);

	pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(nAchievementID);
	LOG_PROCESS_ERROR(pData);

	m_dwAchievementReward[nAchievementID / 32] |= 1 << (nAchievementID % 32);

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoRewardAchievement(m_pOwner, nAchievementID);
	LOG_CHECK_ERROR(nRetCode);
#endif

	for (INT32 nRewardIndex = 0; nRewardIndex < MAX_REWARD_PER_ACHIEVEMENT; nRewardIndex++)
	{
		if (pData->nRewardType[nRewardIndex] != 0)
		{
			switch (pData->nRewardType[nRewardIndex])
			{
			case adtTitle:
				{
					nRetCode = m_pOwner->GetTitle().AddTitle(pData->nRewardID[nRewardIndex]);
					LOG_PROCESS_ERROR(nRetCode);
				}
				break;
			case adtItem:
				{
				nResult = m_pOwner->GetItemBag()->AddItem(pData->nRewardID[nRewardIndex], 1, ITEMLOG_ACHIEVEMENT_REWARD, 0, FALSE);
					LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);
				}
				break;
			case adtExp:
				{
					m_pOwner->AddExp(pData->nRewardID[nRewardIndex], FALSE, TRUE);
				}
				break;
			default:
				{
					LOG_PROCESS_ERROR(FALSE);
				}
				break;
			}
		}
	}

	nResult = errSuccess;

Exit0:
	return nResult;	
}

INT32 CAchievementsList::CanComplete(INT32 nAchievementID)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	const ACHIEVEMENT_DATA* pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(nAchievementID);
	LOG_PROCESS_ERROR_RET_CODE(pData, errUnknownError);

	PROCESS_ERROR_RET_CODE(_IsCompleted(nAchievementID) == FALSE, errAchievementAlreadyCompleted);

	if (pData->nPreAchievementID1 != INVALID_ACHIEVEMENT_ID)
	{
		PROCESS_ERROR(_IsCompleted(pData->nPreAchievementID1));
	}

	if (pData->nPreAchievementID2 != INVALID_ACHIEVEMENT_ID)
	{
		PROCESS_ERROR(_IsCompleted(pData->nPreAchievementID2));
	}

	for (INT32 nIndex = 0; nIndex < MAX_CON_PER_ACHIEVEMENT; nIndex++ )
	{	
		switch(pData->vecAchievementType[nIndex])
		{	
		case adVariable:
			nRetCode = _CheckVariable(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adRoleVariable:
			nRetCode = _CheckRoleVariable(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adLevel:
			nRetCode = _CheckLevel(pData->vecAchievementValue1[nIndex]);
			break;
		case adExperience:
			nRetCode = _CheckExp(pData->vecAchievementValue1[nIndex]);
			break;
		case adPkValue:
			nRetCode = _CheckPKValue(pData->vecAchievementValue1[nIndex]);
			break;
		case adForceReputation:
			nRetCode = _CheckForceReputation(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adVariousForceCount:
			nRetCode = 	_CheckVariousForceCount(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adEquip:
			nRetCode = _CheckEquip(pData->vecAchievementValue1[nIndex]);
			break;
		case adEquipColor:
			nRetCode = _CheckEquipColor(pData->vecAchievementValue1[nIndex]);
			break;
		case adEquipBodyColor:
			nRetCode = _CheckEquipBodyColor(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adSet:
			nRetCode = _CheckSet(pData->vecAchievementValue1[nIndex]);
			break;
		case adSetCount:
			nRetCode = _CheckSetCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adBankSlotCount:
			nRetCode = _CheckBankSlotCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adBagSlotCount:
			nRetCode = _CheckTotalBagSlotCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adItemBagSlotCount:
			nRetCode = _CheckItemBagSlotCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adEquipSlotCount:
			nRetCode = _CheckEquipBagSlotCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adMaterialBagSlotCount:
			nRetCode = _CheckMaterialBagSlotCount(pData->vecAchievementValue1[nIndex]);		
			break;
		case adTotalMoneyType:
			nRetCode = _CheckTotalMoney(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adKillPlayerTotalCount:
			nRetCode = _CheckKillPlayerTotalCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adEnterScene:
			nRetCode = _CheckEnterScene(pData->vecAchievementValue1[nIndex]);
			break;
		case adCompleteQuest:
			nRetCode = _CheckCompleteQuest(pData->vecAchievementValue1[nIndex]);
			break;
		case adCompleteQuestCount:
			nRetCode = _CheckCompleteQuestCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adSpellLevelSpecified:
			nRetCode = _CheckSpellLevelSpecified(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adSpellLevel:
			nRetCode = _CheckSpellLevel(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adLearnSpell:
			nRetCode = _CheckLearnSpell(pData->vecAchievementValue1[nIndex]);
			break;
		case adSpellCount:
			nRetCode = _CheckSpellCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adActiveSpellCount:
			nRetCode = _CheckActiveSpellCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adRuneCount:
			nRetCode = _CheckRuneCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adEquipPassiveSpellIDCount:
			nRetCode = _CheckEquipPassiveSpellIDCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adEquipPassiveSpellCount:
			nRetCode = _CheckEquipPassiveSpellCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adTitle:
			nRetCode = _CheckTitle(pData->vecAchievementValue1[nIndex]);
			break;
		case adTitleCount:
			nRetCode = _CheckTitleCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adFriendCount:
			nRetCode = _CheckFriendCount(pData->vecAchievementValue1[nIndex]);
			break;
 		case adLeaveStudentCount:
 			nRetCode = _CheckLeaveStudentCount(pData->vecAchievementValue1[nIndex]);
 			break;
		case adGetItem:
			nRetCode = _CheckGetItem(pData->vecAchievementValue1[nIndex]);
			break;
		case adItemLuck:
			nRetCode = _CheckItemLuck(pData->vecAchievementValue1[nIndex]);
			break;
		case adItemRefine:
			nRetCode = _CheckItemRefine(pData->vecAchievementValue1[nIndex]);
			break;
		case adRefineLevel:
			nRetCode = _CheckRefineLevel(pData->vecAchievementValue1[nIndex]);
			break;
		case adAchievement:
			nRetCode = _CheckAchievement(pData->vecAchievementValue1[nIndex]);
			break;
		case adAchievementCount:
			nRetCode = _CheckAchievementCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adCreateTime:
			nRetCode = _CheckadCreateTime(CGameWorld::GetInstance().GetGameTime(), pData->vecAchievementValue1[nIndex]);
			break;
		case adHero:
			nRetCode = _CheckHero(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adSpiritLevel:
			nRetCode = _CheckSpiritLevel(pData->vecAchievementValue1[nIndex]);
			break;
		case adSpiritAirColorLevel:
			nRetCode = _CheckSpiritAirColorLevel(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
		case adSpiritBuyAirEquipSlotCount:
			nRetCode = _CheckSpiritBuyAirEquipSlotCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adSpiritBuyAirBagSlotCount:
			nRetCode = _CheckSpiritBuyAirBagSlotCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adSpiritBuyAirStallSlotCount:
			nRetCode = _CheckSpiritBuyAirStallSlotCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adSpiritBloodPageCount:
			nRetCode = _CheckSpiritBloodPageCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adSpiritBloodPageCompleteCount:
			nRetCode = _CheckSpiritBloodPageCompleteCount(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adSpiritBloodPageLevel:
			nRetCode = _CheckSpiritBloodPageLevel(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex], pData->vecAchievementValue3[nIndex]);
			break;
		case adKilledByNpcIDCount:
		case adKillNpcIDCount:
		case adKillNpcGroupIDCount:
		case adUseItemIDCount:
			nRetCode = _CheckRoleVariable(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adVip:
			nRetCode = _CheckVip();
			break;
		case adColorItemCompose:
			nRetCode = _CheckColorItemCompose(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adColorItemDeCompose:
			nRetCode = _CheckColorItemDeCompose(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adRuneWeaponCount:
			nRetCode = _CheckRuneWeaponCount(pData->vecAchievementValue1[nIndex]);
			break;
		case adExplore:
			nRetCode = _CheckExplore(pData->vecAchievementValue1[nIndex]);
			break;
		case adHandbook:
			nRetCode = _CheckHandbook(pData->vecAchievementValue1[nIndex], pData->vecAchievementValue2[nIndex]);
			break;
		case adRanklist:
		case adInvalid:
			nRetCode = errSuccess;
			break;
		default:
			LOG_PROCESS_ERROR(FALSE);
			break;
		}

#ifndef _ROBOT
		LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
#else
		PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
#endif

	}

	return errSuccess;
Exit0:
	return nResult;
}

void CAchievementsList::ActivateHandbook(INT32 nHandbookType, INT32 nIndex)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(nHandbookType > htInvalid && nHandbookType < htTotal);
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_HANKBOOK_COUNT_PER_TYPE);
	PROCESS_ERROR((m_dwHandbook[nHandbookType][nIndex / 32] & (1 << (nIndex % 32))) == 0);

	m_dwHandbook[nHandbookType][nIndex / 32] |= (1 << (nIndex % 32));

#ifndef _ROBOT
	if (!m_pOwner->IsLoading())
	{	
		nRetCode = CCliMessageHandler::GetInstance().DoActivateHankbook(m_pOwner, nHandbookType, nIndex);
		LOG_PROCESS_ERROR(nRetCode);
	}

#endif

Exit0:
	return;
}

/**********************************************************************************88*/

INT32 CAchievementsList::_CheckVariousForceCount(INT32 nMinReputation, INT32 nCount)
{
	INT32 nForceCount = 0;

	LOG_PROCESS_ERROR(m_pOwner);

	for (INT32 nForceID = 0; nForceID < MAX_NPC_FORCE_ID; nForceID++)
	{
		if (m_pOwner->GetReputation().GetValue(nForceID) >= nMinReputation)
		{
			nForceCount++;
		}
	}

	return ((nForceCount >= nCount) ? errSuccess : errAchievementVariousForceCount);

Exit0:
	return  errAchievementVariousForceCount;
}

INT32 CAchievementsList::_CheckEquip(INT32 nEquipID)
{
	INT32 nRetCode = 0;
	const Item* pItem = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->Begin();

	while((pItem = m_pOwner->GetItemBag()->Next()) != NULL)
	{
		if (pItem->m_pTemplate->m_nTemplateID == nEquipID)
		{
			return errSuccess;
		}
	}

Exit0:
	return errAchievementEquip;
}

INT32 CAchievementsList::_CheckGetItem(INT32 nItemID)
{
	INT32 nRetCode = 0;
	const Item* pItem = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	 m_pOwner->GetItemBag()->Begin();

	while((pItem = m_pOwner->GetItemBag()->Next()) != NULL)
	{
		if (pItem->m_pTemplate->m_nTemplateID == nItemID)
		{
			return errSuccess;
		}
	}

Exit0:
	return errAchievementGetItem;
}

INT32 CAchievementsList::_CheckItemLuck(INT32 nLuck)
{
	INT32 nRetCode = 0;
	const Item* pItem = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->Begin();

	while((pItem = m_pOwner->GetItemBag()->Next()) != NULL)
	{
		if (pItem->m_iteminfo.m_byLuck >= nLuck)
		{
			return errSuccess;
		}
	}

Exit0:
	return errAchievementItemLuck;

}

INT32 CAchievementsList::_CheckItemRefine(INT32 nRefineLevel)
{
	INT32 nRetCode = 0;
	UINT16 refine_levels[NES_TOTAL];

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->GetRefineLevels(refine_levels);

	for(INT32 i=0; i<NES_TOTAL; i++)
	{
		if (refine_levels[i] >= nRefineLevel)
		{
			return errSuccess;
		}
	}

Exit0:
	return errAchievementItemRefine;
}

INT32 CAchievementsList::_CheckRefineLevel(INT32 nLevel)
{
	INT32 nRetCode = 0;
    UINT16 awRefineLevels[NES_TOTAL];

	LOG_PROCESS_ERROR(m_pOwner);
	memset(awRefineLevels, 0, sizeof(awRefineLevels));

	m_pOwner->GetItemBag()->GetRefineLevels(awRefineLevels);

	for (INT32 nIndex = 0; nIndex < NES_TOTAL; nIndex++)
	{
		if (awRefineLevels[nIndex] < nLevel)
		{
			return errAchievementItemRefine;
		}
	}

	return errSuccess;
Exit0:
	return errAchievementItemRefine;
}

INT32 CAchievementsList::_CheckSpellLevelSpecified(INT32 nSpellID, INT32 nSpellLevel)
{
	INT32 nRetCode = 0;

	CSpellSet* pSpellSet = m_pOwner->GetSpellBook().GetSpellSet(nSpellID);
	PROCESS_ERROR(pSpellSet);

	return ((pSpellSet->GetActualLevel() >= nSpellLevel) ? errSuccess : errAchievementSpellLevelSpecified);

Exit0:
	return errAchievementSpellLevelSpecified;
}

INT32 CAchievementsList::_CheckSpellLevel(INT32 nSpellLevel, INT32 nSpellCount)
{	
	INT32 nRetCode = 0;
	INT32 nCount = 0;

	CSpellSet* pSellSet = NULL;

	m_pOwner->GetSpellBook().BeginSpellSet();
	pSellSet = m_pOwner->GetSpellBook().NextSpellSet();

	while (pSellSet)
	{
		if (pSellSet->GetActualLevel() >= nSpellLevel)
		{
			nCount++;
		}

		pSellSet = m_pOwner->GetSpellBook().NextSpellSet();
	}

	return ((nCount >= nSpellCount)? errSuccess : errAchievementSpellLevel);
}

INT32 CAchievementsList::_CheckSpellCount(INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nSpellCount = 0;
	CSpellSet* poSpellSet = NULL;

	m_pOwner->GetSpellBook().BeginSpellSet();

	while ((poSpellSet = m_pOwner->GetSpellBook().NextSpellSet()))
	{
		CSpell* poSpell = poSpellSet->GetSpell();
		if (poSpell && poSpell->CanShowOnUI())
			nSpellCount++;
	}

	return ((nSpellCount >= nCount) ? errSuccess : errAchievementSpellCount);
}

INT32 CAchievementsList::_CheckActiveSpellCount(INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nSpellCount = 0;
	CSpellSet* pSpellSet = NULL;

	m_pOwner->GetSpellBook().BeginSpellSet();

	while((pSpellSet = m_pOwner->GetSpellBook().NextSpellSet()) != NULL)
	{
		CSpellTemplate* pSpellTemplate = CSpellConfig::GetInstance().FindSpellTemplate(pSpellSet->GetID(), pSpellSet->GetActualLevel(), pSpellSet->GetActualRuneSlot());
		LOG_PROCESS_ERROR(pSpellTemplate);		

		if (pSpellTemplate->GetAttr(satCastType) == spctActive && pSpellTemplate->GetAttr(satCanEquip))
		{
			nSpellCount++;
		}
	}

	return ((nSpellCount >= nCount) ? errSuccess : errAchievementActiveSpellCount);
Exit0:
	return errAchievementActiveSpellCount;
}

INT32 CAchievementsList::_CheckRuneCount(INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nRuneCount = 0;

	m_pOwner->GetSpellBook().BeginAura();

	while((m_pOwner->GetSpellBook().NextAura()) != NULL)
	{
		nRuneCount++;
	}

	return ((nRuneCount >= nCount) ? errSuccess : errAchievementRuneCount);
}

INT32 CAchievementsList::_CheckEquipPassiveSpellCount(INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nSpellCount = 0;
	CSpellSet* pSpellSet = NULL;

	m_pOwner->GetSpellBook().BeginSpellSet();

	while((pSpellSet = m_pOwner->GetSpellBook().NextSpellSet()) != NULL)
	{
		CSpellTemplate* pSpell = CSpellConfig::GetInstance().FindSpellTemplate(pSpellSet->GetID(), 1, 0);		
		if (pSpell && pSpell->GetAttr(satCastType) == spctPassive)
		{	
			nSpellCount++;
		}
	}

	return ((nSpellCount >= nCount) ? errSuccess : errAchievementEquipPassiveSpellCount);
}

INT32 CAchievementsList::_CheckEquipPassiveSpellIDCount(INT32 nSpellID)
{	
	INT32 nRetCode = 0;
	
	CSpell* pSpell = m_pOwner->GetSpellBook().GetMainSpell(nSpellID);
	PROCESS_ERROR(pSpell);

	return ((pSpell->GetAttr(satCastType) == spctPassive) ? errSuccess : errAchievementEquipPassiveSpellIDCount);

Exit0:
	return errAchievementEquipPassiveSpellIDCount;

}

INT32 CAchievementsList::_CheckEquipColor(INT32 nColor)
{
	INT32 nRetCode = 0;
	const Item* pItem = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->Begin();

	while((pItem = m_pOwner->GetItemBag()->Next()) != NULL)
	{
		if (pItem->m_pTemplate->m_byType != ITEMTYPE_EQUIP)
		{
			continue;
		}

		if (pItem->m_pTemplate->m_byColor >= nColor)
		{
			return errSuccess;
		}
	}

Exit0:
	return errAchievementEquipColor;
}

INT32 CAchievementsList::_CheckEquipBodyColor(INT32 nColor, INT32 nBodyPartID)
{
    INT32 nRetCode = 0;
    Item* pItem = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	pItem = m_pOwner->GetItemBag()->GetItem(CONSLOT_EQUIPPED, nBodyPartID);
	LOG_PROCESS_ERROR(pItem);

	return ((pItem->m_pTemplate->m_byColor >= nColor) ? errSuccess : errAchievementEquipBodyColor);

Exit0:
	return errAchievementEquipBodyColor;
}

INT32 CAchievementsList::_CheckBankSlotCount(INT32 nCount)
{
	INT32 nRetCode = 0;
	UINT8 abySizes[CONSLOT_COUNT];

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->GetContainerSizes(abySizes);

	return ((abySizes[CONSLOT_BANK] >= nCount) ? errSuccess :  errAchievementBankSlotCount);

Exit0:
	return errAchievementBankSlotCount;
}

INT32 CAchievementsList::_CheckTotalBagSlotCount(INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nItemBagCount = 0;

	UINT8 abySizes[CONSLOT_COUNT];

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->GetContainerSizes(abySizes);
	nItemBagCount = abySizes[CONSLOT_DEFAULT];

	return ((nItemBagCount>= nCount) ? errSuccess :  errAchievementTotalSlotCount);

Exit0:
	return errAchievementTotalSlotCount;
}

INT32 CAchievementsList::_CheckItemBagSlotCount(INT32 nCount)
{
	INT32 nRetCode = 0;

	UINT8 abySizes[CONSLOT_COUNT];

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->GetContainerSizes(abySizes);

	return ((abySizes[CONSLOT_DEFAULT]>= nCount) ? errSuccess :  errAchievementItemBagSlotCount);

Exit0:
	return errAchievementItemBagSlotCount;
}

INT32 CAchievementsList::_CheckEquipBagSlotCount(INT32 nCount)
{
	INT32 nRetCode = 0;

	UINT8 abySizes[CONSLOT_COUNT];

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->GetContainerSizes(abySizes);

	//TODO: CONSLOT_EQUIPMENT no longer exists; what should happen to this achievement?
    //return ((abySizes[CONSLOT_EQUIPMENT]>= nCount) ? errSuccess :  errAchievementEquipBagSlotCount);

Exit0:
	return errAchievementEquipBagSlotCount;
}

INT32 CAchievementsList::_CheckMaterialBagSlotCount(INT32 nCount)
{
	INT32 nRetCode = 0;

	UINT8 abySizes[CONSLOT_COUNT];

	LOG_PROCESS_ERROR(m_pOwner);

	m_pOwner->GetItemBag()->GetContainerSizes(abySizes);

    //TODO: CONSLOT_MATERIAL no longer exists; what should happen to this achievement?
	//return ((abySizes[CONSLOT_MATERIAL]>= nCount) ? errSuccess :  errAchievementMaterialBagSlotCount);

Exit0:
	return errAchievementMaterialBagSlotCount;
}

INT32 CAchievementsList::_CheckSpiritAirColorLevel(INT32 nColor, INT32 nLevel)
{
	INT32 nRetCode = 0;
	CAir* pAir = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	for (INT32 nIndex = 0; nIndex < m_pOwner->GetSpirit().GetBagContainerSize(); nIndex++)
	{
		pAir = m_pOwner->GetSpirit().GetAir(sasbtBag, nIndex);
		if (pAir)
		{
			const AIR_DATA* pAirData = CDataManager<AIR_DATA>::GetInstance().GetData(pAir->nTemplateID);
			LOG_PROCESS_ERROR(pAirData);

			if (pAirData->nColor >= nColor && pAirData->nLevel >= nLevel)
			{
				return TRUE;
			}
		}
	}

Exit0:
	return FALSE;
}

INT32 CAchievementsList::_CheckSpiritBloodPageCount(INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nBloodCount = 0;

	INT32 nBloodState = m_pOwner->GetSpirit().GetBloodState();
	for (INT32 nIndex = 0; nIndex < MAX_BLOODSEA_PAGE; nIndex++)
	{
		if (nBloodState & (1<<nIndex))
		{
			nBloodCount++;
		}
	}
	
	return ((nBloodCount >= nCount) ? errSuccess : errAchievementSpiritBloodPageCount);
}

INT32 CAchievementsList::_CheckSpiritBloodPageCompleteCount(INT32 nPage, INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nTotal = 0;

	LOG_PROCESS_ERROR(nPage >= 0 && nPage < MAX_BLOODSEA_PAGE);
	LOG_PROCESS_ERROR(m_pOwner);

	for (INT32 nIndex = 0; nIndex < MAX_BLOODSEA_SLOT_PER_PAGE; nIndex++)
	{
		nTotal += m_pOwner->GetSpirit().GetBlood(nPage, nIndex);
	}

	LOG_PROCESS_ERROR(nTotal >= nCount);

	return errSuccess;
Exit0:
	return errAchievementSpiritBloodPageCompleteCount;
}

INT32 CAchievementsList::_CheckRanklist(INT32 nRankType, INT32 nRank)
{
	INT32 nRetCode = 0;

	return errSuccess;
}

INT32 CAchievementsList::_CheckVariable(INT32 nValue1, INT32 nValue2)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(nValue1 > avdInvalid && nValue1 < avdTotal);
	
	nRetCode = ((GetVariable(nValue1) >= nValue2) ? errSuccess : errUnknownError);

#ifndef _ROBOT
	LOG_PROCESS_ERROR_RET_CODE((nRetCode == errSuccess), errAchievementVariableCheckError);
#else
	PROCESS_ERROR_RET_CODE((nRetCode == errSuccess), errAchievementVariableCheckError);
#endif

	return errSuccess;
Exit0:
	return nResult;
}

INT32 CAchievementsList::_CheckRoleVariable(INT32 nValue1, INT32 nValue2)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(nValue1 >= 0 && nValue1 < MAX_ROLE_VARIABLE_COUNT);

	nRetCode = ((m_pOwner->GetVariable(nValue1) >= nValue2) ? errSuccess : errUnknownError);

#ifndef _ROBOT
	LOG_PROCESS_ERROR_RET_CODE((nRetCode == errSuccess), errAchievementVariableCheckError);
#else
	PROCESS_ERROR_RET_CODE((nRetCode == errSuccess), errAchievementVariableCheckError);
#endif

	return errSuccess;
Exit0:
	return nResult;
	
}

INT32 CAchievementsList::_CheckExp(UINT32 nExp)
{
	return ((m_pOwner->GetExp() >= nExp) ? errSuccess : errAchievementExperience);
}

INT32 CAchievementsList::_CheckPKValue(UINT32 dwPKValue)
{
	return ((m_pOwner->GetPkValue() >= dwPKValue) ? errSuccess : errAchievementPkValue);
}

INT32 CAchievementsList::_CheckForceReputation(INT32 nForceID, INT32 nReputationValue)
{
	return ((m_pOwner->GetReputation().GetValue(nForceID) >= nReputationValue) ? errSuccess : errAchievementForceReputation);
}

INT32 CAchievementsList::_CheckTotalMoney(INT32 nMoneyType, INT32 nCount)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR((nMoneyType > MONEYTYPE_INVALID)&&(nMoneyType < MONEYTYPE_COUNT));

	return ((m_pOwner->GetItemBag()->GetMoneyInfo(nMoneyType) >= nCount) ? errSuccess : errAchievementTotalMoneyType);

Exit0:
	return errAchievementTotalMoneyType;
}

INT32 CAchievementsList::_CheckKillPlayerTotalCount(INT32 nCount)
{
	return (((GetVariable(avdKillWarriorCount) + GetVariable(avdKillMageCount) + GetVariable(avdKillRogueCount) + GetVariable(avdKillArcherCount)) >= nCount) ? errSuccess : errAchievementKillPlayerTotalCount);
}

INT32 CAchievementsList::_CheckEnterScene(INT32 nSceceTmpID)
{
	return ((m_pOwner->GetSceneTempID() == nSceceTmpID) ? errSuccess : errAchievementEnterScene);
}

INT32 CAchievementsList::_CheckCompleteQuest(INT32 nQuestID)
{
	return (m_pOwner->GetQuestList().IsQuestCompleted(nQuestID) ? errSuccess : errAchievementCompleteQuest);
}

INT32 CAchievementsList::_CheckCompleteQuestCount(INT32 nCount)
{
	return ((GetVariable(avdCompleteQuestTotalCount) >=  nCount) ? errSuccess : errAchievementCompleteQuestCount);
}

INT32 CAchievementsList::_CheckLearnSpell(INT32 nSpellID)
{
	return ((m_pOwner->GetSpellBook().GetSpellSet(nSpellID) != NULL) ? errSuccess :  errAchievementLearnSpell);
}

INT32 CAchievementsList::_CheckTitle(INT32 nTitleID)
{
	return (m_pOwner->GetTitle().IsTitleExist((UINT8)nTitleID) ? errSuccess : errAchievementTitle);
}

INT32 CAchievementsList::_CheckTitleCount(INT32 nTitleCount)
{
	return ((m_pOwner->GetTitle().GetTitleCount() >= nTitleCount) ? errSuccess : errAchievementTitleCount);
}

INT32 CAchievementsList::_CheckFriendCount(INT32 nFriendCount)
{
	return ((m_pOwner->GetFriend()->GetFriendCount() >= nFriendCount) ? errSuccess :  errAchievementFriendCount);
}

INT32 CAchievementsList::_CheckAchievement(INT32 nAchievementID)
{
	return ((_IsCompleted(nAchievementID)) ? errSuccess :  errAchievementAchievement);
}

INT32 CAchievementsList::_CheckAchievementCount(INT32 nAchievementCount)
{
	return ((GetCompletedCount() >= nAchievementCount) ? errSuccess :  errAchievementAchievementCount);
}

INT32 CAchievementsList::_CheckadCreateTime(UINT32 dwCurrentTime, UINT32 dwTime)	
{
	return (((dwCurrentTime - m_pOwner->GetCreateTime()) >= dwTime) ? errSuccess : errAchievementCreateTime);
}

inline INT32 CAchievementsList::_CheckHero(INT32 nHeroClass, INT32 nHeroLevel)
{
	return (((m_pOwner->GetHero()->GetClass() == nHeroClass) &&(m_pOwner->GetHero()->GetLevel() >= nHeroLevel)) ? errSuccess : errAchievementHero);
}

INT32 CAchievementsList::_CheckSetCount(INT32 nSetCount)
{
	return ((m_pOwner->GetMountCount() >= nSetCount) ? errSuccess : errAchievementSetCount);
}

INT32 CAchievementsList::_CheckSet(INT32 nSetID)
{
	return ((m_pOwner->GetCurrentMountID() == nSetID) ? errSuccess : errAchievementSet);
}

INT32 CAchievementsList::_CheckColorItemCompose(INT32 nColor, INT32 nCount)
{
	switch (nColor)
	{
	case ITEMCOLOR_PURPLE:
		{
			LOG_PROCESS_ERROR(GetVariable(avdComposePurpleItem) >= nCount);
		}
		break;
	case ITEMCOLOR_ORANGE:
		{
			LOG_PROCESS_ERROR(GetVariable(avdComposeOrangeItem) >= nCount);			
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
	break;
	}
	return errSuccess;

Exit0:
	return errAchievementColorItemComposeCount;
}

INT32 CAchievementsList::_CheckColorItemDeCompose(INT32 nColor, INT32 nCount)
{
	switch (nColor)
	{
	case ITEMCOLOR_PURPLE:
		{
			LOG_PROCESS_ERROR(GetVariable(avdDecomposePurpleItem) >= nCount);
		}
		break;
	case ITEMCOLOR_ORANGE:
		{
			LOG_PROCESS_ERROR(GetVariable(avdDecomposeOrangeItem) >= nCount);			
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}
	return errSuccess;

Exit0:
	return errAchievementColorItemDeComposeCount;
}

INT32 CAchievementsList::_CheckRuneWeaponCount(INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nRuneWeaponCount = 0;
    const Item*   pItem = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	for (INT32 nSlotIndex = CONSLOT_EQUIPPED; nSlotIndex <= CONSLOT_DEFAULT; nSlotIndex++)
	{
		m_pOwner->GetItemBag()->Begin(nSlotIndex);
		pItem = m_pOwner->GetItemBag()->Next();	
		while(pItem)
		{
			LOG_PROCESS_ERROR(pItem->m_pTemplate);

			if (pItem->m_pTemplate->m_byType == ITEMTYPE_EQUIP && pItem->m_pTemplate->m_bySubType == ITEMSUBTYPE_EQUIP_WEAPON)
			{
				for (INT32 nIndex = 0; nIndex < MAX_RUNE_COUNT; nIndex++)
				{
					if (pItem->m_iteminfo.m_wRune[nIndex])
					{
						nRuneWeaponCount++;
						break;
					}
				}
			}
			pItem = m_pOwner->GetItemBag()->Next();
		}
	}

	LOG_PROCESS_ERROR(nRuneWeaponCount >= nCount);

	return errSuccess;
Exit0:
	return errAchievementRuneWeaponCount;
}

INT32 CAchievementsList::_CheckHandbook(INT32 nType, INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nHandbookCount = 0;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(nType >= 0 && nType < htTotal);

	for (INT32 nIndex = 0; nIndex < MAX_HANKBOOK_COUNT_PER_TYPE/32; nIndex++)
	{
		for (INT32 nHandbookIndex = 0; nHandbookIndex < 32; nHandbookIndex++)
		{
			if (m_dwHandbook[nType][nIndex]%2)
			{
				nHandbookCount++;
			}

			m_dwHandbook[nType][nIndex] /= 2;
		}
	}
	
	LOG_PROCESS_ERROR(nHandbookCount >= nCount);

	return errSuccess;
Exit0:
	return errAchievementRuneWeaponCount;
}

inline INT32 CAchievementsList::_CheckLeaveStudentCount(INT32 nCount)
{
	return ((GetVariable(avdLeaveMaster) >= nCount) ? errSuccess : errAchievementSet);
}

inline INT32 CAchievementsList::_CheckSpiritLevel(INT32 nLevel)
{
	return ((m_pOwner->GetSpirit().GetLevel() >= nLevel) ? errSuccess : errAchievementSpiritLevel);
}

inline INT32 CAchievementsList::_CheckSpiritBuyAirEquipSlotCount(INT32 nCount)
{
	return (((m_pOwner->GetSpirit().GetEquipContainerSize() - INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtEquip]) >= nCount) ? errSuccess : errAchievementSpiritBuyAirEquipSlotCount);
}

inline INT32 CAchievementsList::_CheckSpiritBuyAirBagSlotCount(INT32 nCount)
{
	return (((m_pOwner->GetSpirit().GetBagContainerSize() - INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtBag]) >= nCount) ? errSuccess : errAchievementSpiritBuyAirBagSlotCount);
}

inline INT32 CAchievementsList::_CheckSpiritBuyAirStallSlotCount(INT32 nCount)
{
	return (((m_pOwner->GetSpirit().GetStallContainerSize() - INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtStall]) >= nCount) ? errSuccess : errAchievementSpiritBuyAirStallSlotCount);
}

inline INT32 CAchievementsList::_CheckVip()
{
	return (m_pOwner->GetVIPLevel() > vlLevel0 ? errSuccess : errAchievementVip);
}

inline INT32 CAchievementsList::_CheckSpiritBloodPageLevel(INT32 nPage, INT32 nSlotIndex, INT32 nNumber)
{
	return ((m_pOwner->GetSpirit().GetBlood(nPage, nSlotIndex) >= nNumber) ? errSuccess : errAchievementSpiritBloodPageLevel);
}

inline INT32 CAchievementsList::_CheckLevel(INT32 nLevel)
{
	return ((m_pOwner->GetLevel() >= nLevel) ? errSuccess : errAchievementLevel);
}

inline INT32 CAchievementsList::_CheckExplore(INT32 nSceneTemplateID)
{
	return ((m_pOwner->GetSceneTempID() == nSceneTemplateID) ? errSuccess : errAchievementExplore);
}

#ifdef _ROBOT

void CAchievementsList::RobotSyncAchievementList(const char* pcData, UINT32 dwSize, INT32 nCompletedCount)
{
	LOG_CHECK_ERROR(Load_v_1(pcData, dwSize));
	LOG_CHECK_ERROR(m_nCompletedCount == nCompletedCount);
}

void CAchievementsList::RobotAchievementCheckLog(BOOL bCondTYpeCheck, INT16 nCondType, INT16 nSubType, INT16 nAchievementId, BOOL bAchievementCheck)
{
	const ACHIEVEMENT_DATA* pData = NULL;
	static char m_szMsg[2048] = { 0 };
	static UINT32 m_dwPrintInterval = 8 * 3600;
	static UINT32 m_dwPrintTime = CGameWorld::GetInstance().GetGameTime() + m_dwPrintInterval;

	static INT32 anAchievementDef[adTotal] = { 0 };
	static INT32 anAchievementVar[avdTotal] = { 0 };
	static INT32 anAchievementRoleVar[MAX_ROLE_VARIABLE_COUNT] = { 0 };
	static INT32 anAchievementCompleteCount[ACHIEVEMENT_MAX] = { 0 };
	static INT32 anAchievementCheckCount[ACHIEVEMENT_MAX] = { 0 };

	if (bCondTYpeCheck)
	{
		LOG_PROCESS_ERROR(nCondType > adInvalid && nCondType < adTotal);
		++anAchievementDef[nCondType];

		if (adVariable == nCondType)
		{
			LOG_PROCESS_ERROR(nSubType > avdInvalid && nSubType < avdTotal);
			++anAchievementVar[nSubType];
		}
		else if (adRoleVariable == nCondType)
		{
			LOG_PROCESS_ERROR(nSubType > 0 && nSubType < MAX_ROLE_VARIABLE_COUNT);
			anAchievementRoleVar[nSubType]++;
		}
	}
	else
	{
		LOG_PROCESS_ERROR(nAchievementId > 0 && nAchievementId < ACHIEVEMENT_MAX);
		pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(nAchievementId);
		LOG_PROCESS_ERROR(pData);

		if (bAchievementCheck)
		{
			++anAchievementCheckCount[nAchievementId];
		}
		else
		{
			++anAchievementCompleteCount[nAchievementId];
		}
	}

	if (CGameWorld::GetInstance().GetGameTime() >= m_dwPrintTime)
	{
		m_dwPrintTime = CGameWorld::GetInstance().GetGameTime() + m_dwPrintInterval;

		DBG("**************************************** achievement condition type check times print ****************************************");
		for (INT32 i = adInvalid + 1; i < adTotal; ++i)
		{
			memset(m_szMsg, 0, sizeof(m_szMsg));
			sprintf_s(m_szMsg, "(%3d) %-30s : %10d times", i, CEnumString::GetInstance().GetEnumString("ACHIENEMENT_DEF", i).c_str(), anAchievementDef[i]);
			DBG(m_szMsg);
		}

		DBG("**************************************** achievement variable check times print ****************************************");
		for (INT32 i = avdInvalid + 1; i < avdTotal; ++i)
		{
			memset(m_szMsg, 0, sizeof(m_szMsg));
			sprintf_s(m_szMsg, "(%3d) %-35s : %10d times", i, CEnumString::GetInstance().GetEnumString("ACHIEVEMENT_VARIABLE_DEF", i).c_str(), anAchievementVar[i]);
			DBG(m_szMsg);
		}

		DBG("**************************************** role variable check times print ****************************************");
		for (INT32 i = 1; i < MAX_ROLE_VARIABLE_COUNT; ++i)
		{
			memset(m_szMsg, 0, sizeof(m_szMsg));
			sprintf_s(m_szMsg, "variable index %3d : %10d times", i, anAchievementRoleVar[i]);
			DBG(m_szMsg);
		}

		DBG("**************************************** achievement completed times print ****************************************");
		for (INT32 i = 0; i < ACHIEVEMENT_MAX; ++i)
		{
			pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(i);
			if (NULL == pData){continue;}

			memset(m_szMsg, 0, sizeof(m_szMsg));
			sprintf_s(m_szMsg, "(%3d) %s\t\t: completed %5d times, check %10d times", i, pData->szName, anAchievementCompleteCount[i], anAchievementCheckCount[i]);
			DBG(m_szMsg);
		}
	}

Exit0:

	return;
}

void CAchievementsList::RobotCompletedCheck(INT16 nCondType, INT16 nSubType)
{
	UINT16 *pAchievementIds = NULL;

	LOG_PROCESS_ERROR(nCondType > adInvalid && nCondType < adTotal);

	pAchievementIds = CAchievementIndexData::GetInstance().GetAchievementIds(nCondType, nSubType);
	LOG_PROCESS_ERROR(NULL != pAchievementIds);

	//Data record
	RobotAchievementCheckLog(TRUE, nCondType, nSubType, 0, FALSE);

	LOG_PROCESS_ERROR(pAchievementIds[0] < ACHIEVEMENT_MAX);
	for (UINT16 wCount = 1; wCount <= pAchievementIds[0]; ++wCount)
	{
		//Data record
		RobotAchievementCheckLog(FALSE, 0, 0, pAchievementIds[wCount], TRUE);

		if (_IsCompleted(pAchievementIds[wCount])){ continue; }
		if (errSuccess == CanComplete(pAchievementIds[wCount]))
		{
			CRobotMessageHandler::GetInstance().DoCompleteAchievement(m_pOwner, pAchievementIds[wCount]);
		}
	}

Exit0:
	
	return;
}

#endif // _ROBOT
