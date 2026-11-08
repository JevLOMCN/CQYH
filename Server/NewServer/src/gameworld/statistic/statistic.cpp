#include "stdafx.h"
#include "statistic.h"
#include "role.h"
#include "npc.h"
#include "npc_data.h"
#include "spell.h"
#include "game_world.h"
#include "scene.h"
#include "scene_data.h"
#include "item.h"

CStatistic CStatistic::m_Instance;

CStatistic::CStatistic()
{

}

CStatistic::~CStatistic()
{

}

CStatistic& CStatistic::GetInstance(void)
{
	return m_Instance;
}

BOOL CStatistic::Init()
{
	return TRUE;
}

BOOL CStatistic::UnInit()
{
	return TRUE;
}

void CStatistic::OnAddMoney(CRole* poRole, INT32 nAddType, INT32 nMoneyType, INT32 nMoneyCount)
{
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(nAddType > avdInvalid && nAddType < avdTotal);

	switch (nMoneyType)
	{
	case MONEYTYPE_01:
		{
			poRole->GetAchievementList().AddVariable(nAddType, nMoneyCount);
		}
		break;
	default:
		break;
	}

	return;
Exit0:
	return;
}

void CStatistic::OnCompareMaxValue(CRole* poRole, INT32 nCompareType, INT32 nValue)
{
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(nCompareType >= avdInvalid && nCompareType < avdTotal);

	if (poRole->GetAchievementList().GetVariable(nCompareType) < nValue)
	{
		poRole->GetAchievementList().SetVariable(nCompareType, nValue);
	}

	return;

Exit0:
	return;
}
	
void CStatistic::OnKillNpc(CRole* poRole, CNpc* poNpc)
{
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poNpc);
	LOG_PROCESS_ERROR(poNpc->GetNpcData());

	// Count monster kills

	poRole->GetAchievementList().AddVariable(avdKillNpcTotalCount, 1);

	// Count kills of a monster with the specified ID

	if (poNpc->GetNpcData()->nBeKilledVarIndex >= 0 && poNpc->GetNpcData()->nBeKilledVarIndex < MAX_ROLE_VARIABLE_COUNT)
	{
		poRole->AddVariable(poNpc->GetNpcData()->nBeKilledVarIndex, 1);
	}

	switch (poNpc->GetQuality())
	{
	case nqNormal1:
	case nqNormal2:
	case nqNormal3:
	case nqNormal4:
	case nqNormal5:
		poRole->GetAchievementList().AddVariable(avdKillNpcNormal, 1);
		break;
	case nqElite:
		poRole->GetAchievementList().AddVariable(avdKillNpcElit, 1);
		break;
	case nqRare:
		poRole->GetAchievementList().AddVariable(avdKillNpcRare, 1);
		break;
	case nqBoss:
		poRole->GetAchievementList().AddVariable(avdKillNpcBoss, 1);
		break;
	default:
		break;
	}

	return;

Exit0:
	return;
}

void CStatistic::OnKilledByPlayer(CRole* poKiller, CRole* poVictim)
{
	LOG_PROCESS_ERROR(poKiller);
	LOG_PROCESS_ERROR(poVictim);

	// Killer statistics
	switch (poVictim->GetClass())	// Count player kills by class
	{
	case clsWarrior:
		poKiller->GetAchievementList().AddVariable(avdKillWarriorCount, 1);
		break;
	case clsMage:
		poKiller->GetAchievementList().AddVariable(avdKillMageCount, 1);
		break;
	case clsRogue:
		poKiller->GetAchievementList().AddVariable(avdKillRogueCount, 1);
		break;
	case clsArcher:
		poKiller->GetAchievementList().AddVariable(avdKillArcherCount, 1);
		break;
	case clsTaoist:
	case clsSpear:
	case clsSorcerer:
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	if (poVictim->GetPkValue() > ACHIEVEMENT_PK_VALUE)	// Count kills of players with the specified good/evil value
	{
		poKiller->GetAchievementList().AddVariable(avdKillPkValuePlayerCount, 1);
	}

	// Victim statistics
	poVictim->GetAchievementList().AddVariable(avdPVPKilledByPlayer, 1);	// Count PvP deaths caused by players

	return;
Exit0:
	return;

}

void CStatistic::OnKillWithNoGuilty(CRole* poKiller)
{
	CScene * pScene = NULL;
	LOG_PROCESS_ERROR(poKiller);

	pScene = poKiller->GetScene();
	LOG_PROCESS_ERROR(pScene);
	LOG_PROCESS_ERROR(pScene->GetSceneData());

	if (pScene->GetSceneData()->nType != estBattleField &&
		pScene->GetSceneData()->nType != estArena)
	{
		poKiller->GetAchievementList().AddVariable(avdKillWithNoGuilty, 1);		// Complete a PvP kill without gaining PK points
	}

	return;
Exit0:
	return;
}

void CStatistic::OnCompleteQuest(CRole* poRole, const QUEST_DATA* pQuest)
{
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pQuest);

	switch (pQuest->nQuestType)
	{
	case qtTrunk:
	case qtBranch:
		{
			poRole->GetAchievementList().AddVariable(avdCompleteStoryQuestCount, 1);		// Count story quests
		}
		break;
	case qtReward:
		{
			poRole->GetAchievementList().AddVariable(avdCompleteRewardQuestCount, 1);		// Count bounty quests
			poRole->GetAchievementList().AddVariable(avdCompleteNormalQuestCount, 1);		// Count normal quests
		}
		break;
	case qtCircle:  
		{
			poRole->GetAchievementList().AddVariable(avdCompleteCircleQuestCount, 1);		// Count cycle quests
			poRole->GetAchievementList().AddVariable(avdCompleteNormalQuestCount, 1);		// Count normal quests
		}
		break;
	case qtEmegency:
		{
			poRole->GetAchievementList().AddVariable(avdCompleteEmegencyQuestCount, 1);		// Count emergency quests
			poRole->GetAchievementList().AddVariable(avdCompleteNormalQuestCount, 1);		// Count normal quests
		}
		break;
	case qtDaily:
	case qtActivity: 
	case qtGuild:
		{
			poRole->GetAchievementList().AddVariable(avdCompleteGuildQuest, 1);				// Count guild quests
		}
		break;
	case qtMaster:   
	case qtWedding:  
	case qtAdventure:
		{
			poRole->GetAchievementList().AddVariable(avdCompleteAdventureQuest, 1);			// Count adventure quests
		}
		break;	
	case qtGuide:
		{
			poRole->GetAchievementList().AddVariable(avdCompleteNormalQuestCount, 1);		// Count normal quests
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	poRole->GetAchievementList().AddVariable(avdCompleteQuestTotalCount, 1);				// Total quest completions

	return;
Exit0:
	return;

}

void CStatistic::OnHurt(CRole* poVictim, CCharacter* poDealer, INT32 nEffectValue)
{
	CRole* poRole = NULL;

	LOG_PROCESS_ERROR(poVictim);
	LOG_PROCESS_ERROR(poDealer);

	// Victim statistics

	if (poVictim->GetAchievementList().GetVariable(avdMaxHPBeHit) > nEffectValue)		// Maximum damage received
	{
		poVictim->GetAchievementList().SetVariable(avdMaxHPBeHit, nEffectValue);
	}

	// Attacker statistics

	poRole = CGameWorld::GetInstance().FindValidRole(poDealer->GetObjID());
	PROCESS_ERROR(poRole);

	if (poRole->GetAchievementList().GetVariable(avdMaxHPHit) > nEffectValue)		//Maximum damage dealt
	{
		poRole->GetAchievementList().SetVariable(avdMaxHPHit, nEffectValue);
	}

	return;
Exit0:
	return;
}

void CStatistic::OnUseItem(CRole* poRole, Item* pItem)
{
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pItem);
	LOG_PROCESS_ERROR(pItem->m_pTemplate);

	// Count uses of the specified ID using a character variable
	if ((pItem->m_pTemplate->m_nVarIndex > 0) && (pItem->m_pTemplate->m_nVarIndex  < MAX_ROLE_VARIABLE_COUNT))
	{
		poRole->AddVariable(pItem->m_pTemplate->m_nVarIndex, 1);
	}

	// Count uses of the specified type
	switch (pItem->m_pTemplate->m_byType)
	{
	case ITEMTYPE_USABLE:
		{
			if (pItem->m_pTemplate->m_nVarIndex != -1)
			{
				poRole->GetAchievementList().AddVariable(pItem->m_pTemplate->m_nVarIndex, 1);
			}

			poRole->GetAchievementList().AddVariable(avdUseItemCount_Useable, 1);
		}
		break;
	case ITEMTYPE_SPELLBOOK:
		poRole->GetAchievementList().AddVariable(avdUseItemCount_SpellBoke, 1);
		break;
	default:
		break;
	}

	return;
Exit0:
	return;
}

void CStatistic::OnKilledByNpc(CRole* poRole, CNpc* poNpc)
{
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poNpc);
	LOG_PROCESS_ERROR(poNpc->GetNpcData());

	// Count PvP deaths caused by monsters
	poRole->GetAchievementList().AddVariable(avdPVPKilledByNpc, 1);

	// Count deaths caused by a monster with the specified ID
	if (poNpc->GetNpcData()->nKillPlayerVarIndex >= 0 && poNpc->GetNpcData()->nKillPlayerVarIndex < MAX_ROLE_VARIABLE_COUNT)
	{
		poRole->AddVariable(poNpc->GetNpcData()->nKillPlayerVarIndex, 1);
	}

	return;
Exit0:
	return;
}

void CStatistic::OnRevive(CRole* poRole, INT32 nMoneyType, INT32 nMoneyCount)
{
	LOG_PROCESS_ERROR(poRole);

	switch(nMoneyType)
	{
	case MONEYTYPE_01:
		poRole->GetAchievementList().AddVariable(avdRelieveMoneyTotal, nMoneyCount);	
		break;
	case MONEYTYPE_02:
		poRole->GetAchievementList().AddVariable(avdReliveRMBTotal, nMoneyCount);
		break;
	default:
		break;
	}

	return;
Exit0:
	return;
}

void CStatistic::OnComposeItem(CRole* poRole, INT32 nColor, INT32 nCount)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poRole);

	switch(nColor)
	{
	case ITEMCOLOR_PURPLE:
		{
			poRole->GetAchievementList().AddVariable(avdComposePurpleItem, nCount);
		}
		break;
	case ITEMCOLOR_ORANGE:
		{
			poRole->GetAchievementList().AddVariable(avdComposeOrangeItem, nCount);
		}
		break;
	default:
		break;
	}

	poRole->GetAchievementList().AddVariable(avdComposeItemCount, nCount);

	return;
Exit0:
	return;
}

void CStatistic::OnDecomposeItem(CRole* poRole, INT32 nColor, INT32 nCount)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poRole);

	switch(nColor)
	{
	case ITEMCOLOR_PURPLE:
		{
			poRole->GetAchievementList().AddVariable(avdDecomposePurpleItem, nCount);
		}
		break;
	case ITEMCOLOR_ORANGE:
		{
			poRole->GetAchievementList().AddVariable(avdDecomposeOrangeItem, nCount);
		}
		break;
	default:
		break;
	}

	poRole->GetAchievementList().AddVariable(avdDeComposeItemCount, nCount);

	return;
Exit0:
	return;

}

void CStatistic::OnRealShopMoney(CRole* poRole, INT32 nItemID, INT32 nStackCount, INT32 nItemCount, INT32 nOrginalPrice)
{
	INT32 nRetCode = 0;
	INT32 nMoneyCost = 0;

	LOG_PROCESS_ERROR(poRole);

	nMoneyCost = nOrginalPrice * nItemCount;

	if (nItemID < MONEYTYPE_COUNT)
	{
		poRole->GetAchievementList().AddVariable(avdRealMoneyToMoney02Count, nMoneyCost);
	}
	else
	{
		poRole->GetAchievementList().AddVariable(avdRealMoneyToItemCount, nMoneyCost);
	}

	return;
Exit0:
	return;
}





