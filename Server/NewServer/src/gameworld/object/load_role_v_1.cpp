#include "stdafx.h"
#include "role.h"

#include "server_config.h"
#include "level_data.h"
#include "mount_data.h"
#include "refine_data.h"
#include "spirit_data.h"
#include "spell_config.h"
#include "attribute_data.h"
#include "game_world.h"
#include "item_mgr.h"
#include "modifier_tree.h"
#include "message_handler.h"

// Default code
//#include "role_db_def_v_1.h"
//using namespace ROLE_DB_DEF_V_1;

#include "role_db_def_v_3.h"
using namespace ROLE_DB_DEF_V_3;

BOOL CRole::_Load_v_1(const char* pcData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	ROLE_DB_DATA* pRoleData = (ROLE_DB_DATA*)pcData;
	UINT32 dwRealDataSize = 0;

	LOG_PROCESS_ERROR(pcData);
	//LOG_PROCESS_ERROR(pRoleData->dwVersion == 1);    // Default code, temporarily changed to 3
	LOG_PROCESS_ERROR(pRoleData->dwVersion == 3);
	LOG_PROCESS_ERROR(pRoleData->BlockTable[rbtBase].dwStartOffset == 0);

	dwRealDataSize = pRoleData->dwLen - sizeof(ROLE_DB_DATA);

	// block table
	for (UINT32 i = 0; i < rbtTotal; i++)
	{
		ROLE_DB_DATA_BLOCK* pBlock = &pRoleData->BlockTable[i];

		const char* pcBlockData = &pRoleData->RoleData[pBlock->dwStartOffset];
		UINT32 dwBlockSize = pBlock->dwBlockSize;
		LOG_PROCESS_ERROR(pBlock->dwStartOffset + pBlock->dwBlockSize <= dwRealDataSize);

		if (pBlock->dwBlockSize == 0)
			continue;

		switch (pBlock->btBlockType)
		{
		case rbtBase:
			nRetCode = _LoadBaseData(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtProcessedMails:
			nRetCode = m_ProMails.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtItem:
			nRetCode = m_ItemBag.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtSpell:
			nRetCode = _LoadSpell_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtQuest:
			nRetCode = m_QuestList.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtReputation:
			nRetCode = m_Reputation.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtSpellSlot:
			nRetCode = _LoadSpellSlot_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtStall:
			nRetCode = m_Stall.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtFriend:
			nRetCode = m_Friend.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtVariable:
			nRetCode = _LoadVariable_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtTitle:
			nRetCode = m_Title.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtAchievement:
			nRetCode = m_AchievementList.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtDungeonRecord:
			nRetCode = _LoadDungeonRecord_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtTeleportPlatform:
			nRetCode = _LoadTeleportPlatform_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtActivatedMount:
			nRetCode = _LoadMount_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtSpirit:
			nRetCode = m_Spirit.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtHero:
			nRetCode = _LoadHero_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtShield:
			nRetCode = m_ShieldList.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtMaster:
			nRetCode = m_Master.OnLoadByGS_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtAura:
			nRetCode = _LoadAura_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtGuild:
			nRetCode = m_Guild.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtMisc:
			nRetCode = _LoadMisc_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtArena:
			nRetCode = m_Arena.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtPet:
			nRetCode = _LoadPet_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtKillNpcQuest:
			nRetCode = m_KillNpcQuest.Load_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtCoolDown:
			nRetCode = _LoadCoolDown_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtCheckIn:
			nRetCode = _LoadCheckIn_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtBlackMarketLimit:
			nRetCode = m_ItemBag.LoadBlackMarketLimit_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtBlackMarketCooldown:
			nRetCode = m_ItemBag.LoadBlackMarketCooldown_v_1(pcBlockData, dwBlockSize);
			LOG_PROCESS_ERROR(nRetCode);
			break;
		case rbtGamble:    // Awaiting implementation
			break;
		default:
			LOG_PROCESS_ERROR(FALSE);
			break;
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadVariable_v_1(const char* pData, UINT32 dwSize)
{
	INT32* pnCount = NULL;
	INT16* pwIndex = NULL;
	INT32* pnValue = NULL;

	LOG_PROCESS_ERROR(pData);

	memset(m_nVarList, 0, sizeof(m_nVarList));

	pnCount = (INT32*)pData;
	pData += sizeof(INT32);
	LOG_PROCESS_ERROR(dwSize == sizeof(*pnCount) + *pnCount * (sizeof(*pwIndex) + sizeof(*pnValue)) + sizeof(m_nClientVarList));

	for (INT32 nIndex = 0; nIndex < *pnCount; nIndex++)
	{
		pwIndex = (INT16*)pData;
		pData += sizeof(INT16);

		pnValue = (INT32*)pData;
		pData += sizeof(INT32);

		LOG_PROCESS_ERROR(*pwIndex >= 0 && *pwIndex < MAX_ROLE_VARIABLE_COUNT);
		m_nVarList[*pwIndex] = *pnValue;
	}

	memcpy(m_nClientVarList, pData, sizeof(m_nClientVarList));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadSpell_v_1(const char* pcData, UINT32 dwSize)
{
    INT32 nRetCode = 0;
    CSpellSet* poSet = NULL;
    UINT32 dwCount = dwSize / sizeof(ROLE_DB_SPELL_DATA);
    LOG_PROCESS_ERROR(pcData);

    for (UINT32 i = 0; i < dwCount; i++)
    {
        ROLE_DB_SPELL_DATA* pData = (ROLE_DB_SPELL_DATA*)(pcData + i*sizeof(ROLE_DB_SPELL_DATA));

        nRetCode = LearnSpell(pData->wSpellID, pData->bySpellLevel, TRUE);
        if (nRetCode != errSuccess)
            continue;

        poSet = m_SpellBook.GetSpellSet(pData->wSpellID);
        LOG_PROCESS_ERROR(poSet);

        poSet->SetPts(pData->wCurPts);
    }

Exit0:
    return TRUE;
}

BOOL CRole::_LoadAura_v_1(const char* pcData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	UINT32 dwCount = dwSize / sizeof(ROLE_DB_AURA_DATA);
	LOG_PROCESS_ERROR(pcData);

	for (UINT32 i = 0; i < dwCount; i++)
	{
		ROLE_DB_AURA_DATA* pData = (ROLE_DB_AURA_DATA*)(pcData + i*sizeof(ROLE_DB_AURA_DATA));

		CAura* poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(pData->wAuraID);
		if (poAuraInfo == NULL)
			continue;

		ExtraParam Param = { pData->nX[0], pData->nX[1], pData->nX[2], pData->nX[3] };

		nRetCode = _LoadSingleAura_v_1(poAuraInfo, pData->dwCasterID, pData->dwRemainFrm, pData->byStackCount, Param, pData->wRootSpellID, pData->byRootSpellLevel);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadSingleAura_v_1(CAura* poAuraInfo, UINT32 dwCaster, UINT32 dwRemainFrm, INT32 nStackCount, ExtraParam& Param, INT32 nRootSpellID, INT32 nRootSpellLevel)
{
	INT32 nRetCode = 0;

	CAuraInstance* poAuraInst = NULL;
	CCharacter* poCaster = NULL;

	LOG_PROCESS_ERROR(poAuraInfo);

	if (dwCaster == m_dwObjID)
		poCaster = this;
	else
	{
		poCaster = CGameWorld::GetInstance().FindValidCharacter(dwCaster);
		PROCESS_ERROR(poCaster);
	}

	if (poAuraInfo->GetAttr(aatOfflineTimeValid))
	{
		UINT32 dwDiffFrm = (CGameWorld::GetInstance().GetGameTime() - m_dwSaveTime) * GAME_FPS;
		if (dwDiffFrm >= dwRemainFrm)
			dwRemainFrm = 0;
		else
			dwRemainFrm = dwRemainFrm - dwDiffFrm;
	}

	poAuraInst = AppendAura(poAuraInfo, poCaster, dwCaster, &Param, dwRemainFrm, nStackCount, nRootSpellID, nRootSpellLevel);
	LOG_PROCESS_ERROR(poAuraInst);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadSpellSlot_v_1(const char* pcData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	UINT32 dwCount = dwSize / sizeof(ROLE_DB_SPELL_SLOT_DATA);
	LOG_PROCESS_ERROR(pcData);

	for (UINT32 i = 0; i < dwCount; i++)
	{
		ROLE_DB_SPELL_SLOT_DATA* pData = (ROLE_DB_SPELL_SLOT_DATA*)(pcData + i*sizeof(ROLE_DB_SPELL_SLOT_DATA));
		LOG_CHECK_ERROR(pData->bySlotID >= qssSlotBegin && pData->bySlotID < qssSlotCount);

		m_SpellSlot[pData->bySlotID].Lock(pData->byLocked);

		if (pData->wSpellID != 0)
		{
			nRetCode = EquipSpell(pData->bySlotID, pData->wSpellID);
			LOG_CHECK_ERROR(nRetCode);
		}

		//		m_SpellSlot[pData->bySlotID].EnableRuneSlot(pData->byRuneSlot);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadMount_v_1(const char* pcData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	UINT32 dwCount = dwSize / sizeof(ROLE_DB_ACTIVATED_MOUNT_DATA);
	LOG_PROCESS_ERROR(pcData);

	m_ActivedMountList.clear();

	for (UINT32 i = 0; i < dwCount; i++)
	{
		ROLE_DB_ACTIVATED_MOUNT_DATA* pData = (ROLE_DB_ACTIVATED_MOUNT_DATA*)(pcData + i*sizeof(ROLE_DB_ACTIVATED_MOUNT_DATA));

		const MOUNT_ENTRY* pInfo = CDataManager<MOUNT_ENTRY>::GetInstance().GetData(pData->wMountID);
		if (pInfo == NULL)
			continue;

		m_ActivedMountList.insert(pInfo->m_nID);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadHero_v_1(const char* pcData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	INT32 nType = 0;

	// Default code
    //ROLE_DB_DEF_V_1::ROLE_HERO_DATA* pData = (ROLE_DB_DEF_V_1::ROLE_HERO_DATA*)pcData;

	// Custom code
    ROLE_DB_DEF_V_3::ROLE_HERO_DATA* pData = (ROLE_DB_DEF_V_3::ROLE_HERO_DATA*)pcData;

	if (pData->dwID != 0)
	{
		nRetCode = HireHero(pData->byLevel, pData->byClass, pData->bySex, pData->dwExp, pData->byAiType, FALSE);
		LOG_PROCESS_ERROR(nRetCode);

		LOG_PROCESS_ERROR(m_poHero);

		if (pData->byNameChanged)
			m_poHero->ChangeName(pData->szName);

		m_poHero->SetGrowth(pData->nGrowth);
		//m_poHero->SetSummoned(pData->byIsSummoned);

		AddCooldown(MakeCoolDownID(COOLDOWN_MISC, COOLDOWN_MISC_HERO_REVIVE), pData->nCooldownRemainFrm);
	}
	else
		m_ItemBag.CloseHeroContainers();

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadDungeonRecord_v_1(const char* pData, UINT32 dwSize)
{
	ROLE_DB_DUNGEON_RECORD* poRecord = (ROLE_DB_DUNGEON_RECORD*)pData;
	LOG_PROCESS_ERROR(sizeof(ROLE_DB_DUNGEON_RECORD) == dwSize);

	memcpy(m_lstDungeonRecord, poRecord->byDungeonRecord, sizeof(m_lstDungeonRecord));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadTeleportPlatform_v_1(const char* pData, UINT32 dwSize)
{
	LOG_PROCESS_ERROR(dwSize == MAX_TELEPORT_PLATFORM_COUNT / 8);
	memcpy(m_nTeleportPlatform, pData, dwSize);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTitle::Load_v_1(const char* pcData, UINT32 dwSize)
{
	const ROLE_DB_TITLE_DATA* pTitleDBData = (const ROLE_DB_TITLE_DATA*)pcData;
	UINT32 buflen = 0;

	LOG_PROCESS_ERROR(pcData);
	LOG_PROCESS_ERROR(sizeof(ROLE_DB_TITLE_DATA) <= dwSize);

	clear();

	m_nEquipedTitleID = pTitleDBData->nEquipedTitleID;
	for (INT32 i = 0; i < pTitleDBData->nTitleDataNum; ++i)
	{
		TITLE  data;
		data.m_nTemplateID = pTitleDBData->stTitleList[i].m_nTemplateID;
		data.m_nExpireTime = pTitleDBData->stTitleList[i].m_nExpireTime;
		//if (data.m_nExpireTime > CGameWorld::GetInstance().GetGameTime() || NA_TITLE_EXPIRE_TIME == data.m_nExpireTime)
		{
			m_Title_Map.insert(std::make_pair(data.m_nTemplateID, data));
		}
	}

	// equip
	if (m_nEquipedTitleID != INVALID_TITLE_ID)
	{
		TITLE_MAP::iterator iter = m_Title_Map.find(m_nEquipedTitleID);
		if (iter == m_Title_Map.end())
		{
			m_nEquipedTitleID = INVALID_TITLE_ID;
			LOG_CHECK_ERROR(FALSE);
		}
		else
		{
			iter->second.OnEquip(m_pOwner);
		}
	}

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CProMails::Load_v_1(const char* pcData, UINT32 dwSize)
{
	const ROLE_DB_PROCESSED_MAILS* pMailData = (const ROLE_DB_PROCESSED_MAILS*)pcData;

	m_lstProcessedMailID.clear();

	for (UINT8 i = 0; i < pMailData->byMailCount; i++)
	{
		m_lstProcessedMailID.push_back(pMailData->dwMailID[i]);
	}

	return TRUE;
}

BOOL CItemBag::Load_v_1(const char* pcData, UINT32 dwSize)
{
	const ROLE_DB_ITEM_DATA* pItemData = (const ROLE_DB_ITEM_DATA*)pcData;
	UINT32 buflen = 0;
	ItemInfo iteminfo;
	Item* item = NULL;

	LOG_PROCESS_ERROR(pcData);
	LOG_PROCESS_ERROR(sizeof(ROLE_DB_ITEM_DATA) + pItemData->dwItemInfoBufLen == dwSize);

	Clear();

	memcpy(m_refine_levels, pItemData->wRefineLevels, sizeof(m_refine_levels));

	for (INT32 i = 0; i < MONEYTYPE_COUNT; i++)
	{
		m_moneys[i] = pItemData->llMoneys[i];
		m_moneys_daily_earned[i] = pItemData->nMoneyDailyEarned[i];
		m_moneys_daily_used[i] = pItemData->nMoneyDailyUsed[i];
		m_moneys_weekly_earned[i] = pItemData->nMoneyWeekleEarned[i];
		m_moneys_weekly_used[i] = pItemData->nMoneyWeekleUsed[i];
	}
	UpdateMoneyLimits();

	m_use_moneytype01_only = pItemData->byUseMoneytype01Only;
	m_gold_pool = pItemData->nGoldPool;

	m_con_sizes[CONSLOT_DEFAULT] = pItemData->byDefaultSlotSize;
	m_con_sizes[CONSLOT_BANK] = pItemData->byBankSlotSize;

	m_dwWeaponLevelUpStartTime = pItemData->dwWeaponLevelUpStartTime;
	m_nWeaponDur = pItemData->nWeaponDur;
	m_nWeaponPhy = pItemData->nWeaponPhy;
	m_nWeaponMag = pItemData->nWeaponMag;
	m_nWeaponTao = pItemData->nWeaponTao;
	m_byMailNotified = pItemData->byMailNotified;
	m_nExpBottleTimes = pItemData->nExpBottleTimes;

	for (UINT32 i = 0; i < pItemData->dwItemInfoCount; i++)
	{
		UINT32 this_buflen = iteminfo.Load(pItemData->acItemInfos + buflen);
		LOG_PROCESS_ERROR(this_buflen > 0);
		buflen += this_buflen;
		if (iteminfo.m_byConSlot == CONSLOT_WEAPON_LEVEL_UP)
		{
			LOG_PROCESS_ERROR(iteminfo.m_byItemSlot == 0);
		}
		else
		{
			LOG_PROCESS_ERROR(iteminfo.m_byConSlot < CONSLOT_COUNT);
			LOG_PROCESS_ERROR(iteminfo.m_byItemSlot < m_con_sizes[iteminfo.m_byConSlot]);
		}

		item = CItemMgr::GetInstance().LoadItem(iteminfo);
		if (item == NULL)
		{
			CRI("LoadItem Failed: RoleID:%u, GUID:%I64u, TemplateID:%d, Dur:%d", m_pOwner->GetObjID(), iteminfo.m_llGuid, iteminfo.m_nTemplateID, iteminfo.m_nCurDur);
			continue;
		}

		if (iteminfo.m_byConSlot == CONSLOT_WEAPON_LEVEL_UP)
		{
			LOG_PROCESS_ERROR(m_WeaponItem == NULL);
			m_WeaponItem = item;
		}
		else
		{
			m_cons[iteminfo.m_byConSlot].insert(Container::value_type(iteminfo.m_byItemSlot, item));
			if (iteminfo.m_byConSlot == CONSLOT_EQUIPPED && iteminfo.m_nCurDur > 0)
				item->OnEquip(m_pOwner, m_refine_levels[iteminfo.m_byItemSlot], m_pOwner->GetClass(), iteminfo.m_byItemSlot);
		}
	}

	LOG_PROCESS_ERROR(buflen == pItemData->dwItemInfoBufLen);

	//Calculate enhancement-activated attributes
	{
		const REFINE_ATTR_DATA* cur_data = NULL;
		UINT16 cur_min_level = MAX_REFINE_LEVEL;
		for (INT32 i = 0; i < NES_TOTAL; i++)
		{
			if (m_refine_levels[i] < cur_min_level)
				cur_min_level = m_refine_levels[i];
		}

		cur_data = CDataManager<REFINE_ATTR_DATA>::GetInstance().GetData(REFINE_ATTR_DATA::MakeKey(cur_min_level, m_pOwner->GetClass(), NES_TOTAL));
		if (cur_data)
		{
			for (INT32 i = 0; i < cur_data->m_nNodeCount; i++)
				CModifierTree::GetInstance().RollBackExtraNode(cur_data->m_pAttrData[i]->m_nNodeID, m_pOwner, NULL);
		}
	}

	CheckWeight();

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CQuestList::Load_v_1(const char* pData, UINT32 dwSize)
{
	INT32 nRetCode = 0;

	ROLE_DB_QUEST_DATA* pQuestData = (ROLE_DB_QUEST_DATA*)pData;

	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(dwSize == sizeof(ROLE_DB_QUEST_DATA) +
		sizeof(ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST) * pQuestData->byAcceptedQuestCount);

	memcpy(m_byNormalQuest, pQuestData->byNormalQuest, sizeof(m_byNormalQuest));
	memcpy(m_byRepeatQuest, pQuestData->byRepeatQuest, sizeof(m_byRepeatQuest));

	m_dwRepeatQuestScanTime = pQuestData->dwRepeatQuestScanTime;
	m_bRecodeKillPlayer = pQuestData->bRecodeKillPlayer;
	m_dwLastCompleteEmergencyQuestTime = pQuestData->dwLastCompleteEmergencyQuestTime;
	m_nCompleteEmergencyQuestCount = pQuestData->byCompleteEmergencyQuestCount;

	for (INT32 nIndex = 0; nIndex < pQuestData->byAcceptedQuestCount; nIndex++)
	{
		ACCEPTED_QUEST AcceptedQuest;

		AcceptedQuest.nQuestID = pQuestData->AcceptQuestList[nIndex].nQuestID;
		AcceptedQuest.dwAcceptTime = pQuestData->AcceptQuestList[nIndex].dwAcceptTime;
		for (INT32 nKillIndex = 0; nKillIndex < MAX_CONDITION_PER_QUEST; nKillIndex++)
		{
			AcceptedQuest.nKillNpcCount[nKillIndex] = pQuestData->AcceptQuestList[nIndex].byKillNpcCount[nKillIndex];
			AcceptedQuest.nKillDoodadCount[nKillIndex] = pQuestData->AcceptQuestList[nIndex].byOpenDoodadCount[nKillIndex];
			AcceptedQuest.nCheckItemCount[nKillIndex] = pQuestData->AcceptQuestList[nIndex].nCheckItemCount[nKillIndex];
		}

		for (INT32 nKillIndex = 0; nKillIndex < CHECK_KILL_PLAYER_CONDITION; nKillIndex++)
		{
			AcceptedQuest.nKillPlayerCount[nKillIndex] = pQuestData->AcceptQuestList[nIndex].nKillPlayerCount[nKillIndex];
			AcceptedQuest.nKillBufPlayerCount[nKillIndex] = pQuestData->AcceptQuestList[nIndex].nKillBufPlayerCount[nKillIndex];
		}

		memcpy(AcceptedQuest.nVariable, pQuestData->AcceptQuestList[nIndex].nVariable, sizeof(AcceptedQuest.nVariable));

		if (AcceptedQuest.nQuestID >= EMERGENCY_QUEST_START_ID && AcceptedQuest.nQuestID <= (EMERGENCY_QUEST_START_ID + MAX_EMERGENCY_QUEST_COUNT))
		{
			m_AcceptedEmergencyQuestList.push_back(AcceptedQuest);
		}
		else
		{
			m_AcceptedNormalQuestList.push_back(AcceptedQuest);
		}

	}

	/*************************** circle quest *************************************/
	m_dwCircleIndex = pQuestData->stCircleQuestData.byCircleIndex;

	for (INT32 nIndex = 0; nIndex < MAX_CIRCLE_COUNT; nIndex++)
	{
		m_stCircleQuest[nIndex].nCircleNumber = pQuestData->stCircleQuestData.nCircleNumber[nIndex];
		m_stCircleQuest[nIndex].nCompleteCount = pQuestData->stCircleQuestData.nCompleteCount[nIndex];
		m_stCircleQuest[nIndex].nLastCompleteTime = pQuestData->stCircleQuestData.nLastCompleteTime[nIndex];
		m_stCircleQuest[nIndex].nQuestID = pQuestData->stCircleQuestData.nQuestID[nIndex];
	}

	/*************************** reward quest *************************************/
	m_dwRewardBoughtTime = pQuestData->stDailyQuestData.dwRewardBoughtTime;
	m_dwRewardQuestBrushCDTime = pQuestData->stDailyQuestData.dwRewardQuestBrushCDTime;
	m_nRewardIndex = pQuestData->stDailyQuestData.nRewardIndex;
	m_nRewardQuestRemainCompleteTime = pQuestData->stDailyQuestData.nRewardQuestRemainCompleteTime;
	m_qwRewardAccepted = pQuestData->stDailyQuestData.qwRewardAccepted;
	m_qwRewardCompleted = pQuestData->stDailyQuestData.qwRewardCompleted;
	m_qwRewardCompleted = pQuestData->stDailyQuestData.qwRewardCompleted;
	m_qwRewardQuest = pQuestData->stDailyQuestData.qwRewardQuest;
	m_nRewardCompleteCount = pQuestData->stDailyQuestData.nCompleteCount;
	m_nDailyRewwardBoughtTime = pQuestData->stDailyQuestData.nDailyRewwardBoughtTime;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CReputation::Load_v_1(const char* pcData, UINT32 dwSize)
{
    const ROLE_DB_REPUTATION* pcReputation = nullptr;
    INT32 nCount = 0;
    LOG_PROCESS_ERROR(pcData);

    m_qwActiveState = *(UINT64*)pcData;
    pcData += sizeof(UINT64);

    m_qwAwardState = *(UINT64*)pcData;
    pcData += sizeof(UINT64);

    pcReputation = (ROLE_DB_REPUTATION*)pcData;
    nCount = (dwSize - sizeof(UINT64) - sizeof(UINT64)) / sizeof(*pcReputation);
    for (INT32 nIndex = 0; nIndex < nCount; nIndex++)
    {
        LOG_PROCESS_ERROR(pcReputation->byForceID < MAX_NPC_FORCE_ID);
        m_nValueList[pcReputation->byForceID] = pcReputation->nReputation;
        ++pcReputation;
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CStall::Load_v_1(const char* pcData, UINT32 dwSize)
{
	ROLE_DB_STALL_DATA* pData = (ROLE_DB_STALL_DATA*)pcData;

	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(dwSize == sizeof(ROLE_DB_STALL_DATA));

	Clear();

	m_dwUpgradeExpireTime = pData->dwUpgradeExpireTime;

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CFriend::Load_v_1(const char* pcData, UINT32 dwSize)
{
	const ROLE_DB_FRIEND_DATA* pFriendData = (ROLE_DB_FRIEND_DATA*)pcData;

	LOG_PROCESS_ERROR(pcData);
	LOG_PROCESS_ERROR(pFriendData->byFriendGroupCount <= MAX_FRIEND_GROUP_COUNT);
	LOG_PROCESS_ERROR(sizeof(ROLE_DB_FRIEND_DATA) + sizeof(FRIEND_GROUP_DATA) * pFriendData->byFriendGroupCount == dwSize);

	Clear();

	strxcpy(m_szSignature, pFriendData->szSignature, SIGNATURE_LEN);

	memset(m_szGroups, 0, sizeof(m_szGroups));
	for (UINT8 i = 0; i < pFriendData->byFriendGroupCount; i++)
	{
		const FRIEND_GROUP_DATA& friend_group_data = pFriendData->stFriendGroupData[i];
		UINT8 group_id = friend_group_data.byGroupID;
		LOG_PROCESS_ERROR(group_id < MAX_FRIEND_GROUP_COUNT);
		strxcpy(m_szGroups[group_id], friend_group_data.szGroupName, FRIEND_GROUP_NAME_LEN);
	}

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CAchievementsList::Load_v_1(const char* pData, UINT32 dwSize)
{
	INT32 nRetCode = 0;

	ROLE_DB_ACHIEVEMENT_DATA* pAchievementData = (ROLE_DB_ACHIEVEMENT_DATA*)pData;

	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(dwSize == sizeof(ROLE_DB_ACHIEVEMENT_DATA));

	memcpy(m_dwAchievementReward, pAchievementData->dwAchievementReward, sizeof(m_dwAchievementReward));
	memcpy(m_wAchievement, pAchievementData->wAchievement, sizeof(m_wAchievement));
	memcpy(m_nAchievementStatistics, pAchievementData->nVarList, sizeof(m_nAchievementStatistics));

	memset(m_dwHandbook, 0, sizeof(m_dwHandbook));
	memcpy(m_dwHandbook, pAchievementData->dwHandbook, sizeof(pAchievementData->dwHandbook));

	m_nCompletedCount = pAchievementData->nAchievementCount;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSpirit::Load_v_1(const char* pData, UINT32 dwSize)
{
	INT32 nRetCode = 0;

	ROLE_DB_SPIRIT_DATA* pSpiritData = NULL;
	const SPIRIT_DATA* pSpiritLevel = NULL;

	pSpiritData = (ROLE_DB_SPIRIT_DATA*)pData;
	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(pSpiritData);
	LOG_PROCESS_ERROR(dwSize == sizeof(ROLE_DB_SPIRIT_DATA));

	/********* overral *********************/

	m_nLevel = pSpiritData->byLevel;
	m_nExp = pSpiritData->nExp;
	m_dwNextExp2PtsTime = pSpiritData->dwExp2PtsRemainTime + CGameWorld::GetInstance().GetGameTime();
	m_nTotalMoneyCost = pSpiritData->nMoneyCost;
	m_dwDailyUseMoneyChangeCount = pSpiritData->dwDailyUseMoneyChangeCount;
	m_dwNextScanTimer = pSpiritData->dwNextScanTimer;

	pSpiritLevel = CDataManager<SPIRIT_DATA>::GetInstance().GetData(m_nLevel);
	LOG_PROCESS_ERROR(pSpiritLevel);

	for (INT32 nIndex = 0; nIndex < MAX_SPIRIT_NODE_PER_LEVEL; nIndex++)		// Attributes for each spirit level
	{
		if (pSpiritLevel->nAttrTemplateNodeID[nIndex])
		{
			nRetCode = CModifierTree::GetInstance().ApplyExtraNode(pSpiritLevel->nAttrNodeID[nIndex], m_pOwner, NULL);
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

	/********* overral *********************/

	m_nProductAirLevel = pSpiritData->nProductAirLevel;

	m_nBagContainerSize = pSpiritData->nBagContainerSize;
	m_nStallContainerSize = pSpiritData->nStallContainerSize;
	m_nEquipContainerSize = pSpiritData->nEquipContainerSize;

	for (INT32 nIndex = 0; nIndex < MAX_AIRSEA_BAG_CONTAINER_SIZE; nIndex++)
	{
		m_BagContainer[nIndex].nTemplateID = pSpiritData->nBagContainerTemplate[nIndex];
		m_BagContainer[nIndex].nExp = pSpiritData->nBagContainerExp[nIndex];
	}

	for (INT32 nIndex = 0; nIndex < MAX_AIRSEA_STALL_CONTAINER_SIZE; nIndex++)
	{
		m_StallContainer[nIndex].nTemplateID = pSpiritData->nStallContainerTemplate[nIndex];
		m_StallContainer[nIndex].nExp = pSpiritData->nStallContainerExp[nIndex];
	}

	for (INT32 nIndex = 0; nIndex < MAX_AIRSEA_EQUIP_CONTAINER_SIZE; nIndex++)
	{
		m_EquipContainer[nIndex].nTemplateID = pSpiritData->nEquipContainerTemplate[nIndex];
		m_EquipContainer[nIndex].nExp = pSpiritData->nEquipContainerExp[nIndex];

		if (m_EquipContainer[nIndex].nTemplateID)
		{
			const AIR_DATA* pAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_EquipContainer[nIndex].nTemplateID);
			LOG_PROCESS_ERROR(pAir);

			for (INT32 nArmIndex = 0; nArmIndex < MAX_ATTR_PER_NODE; nArmIndex++)		// Equip item
			{
				if (pAir->nNodeID[nArmIndex])
				{
					nRetCode = CModifierTree::GetInstance().ApplyExtraNode(pAir->nNodeID[nArmIndex], m_pOwner, NULL);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
		}
	}

	/**********  blood sea *************/

	m_nBloodState = pSpiritData->byBloodState;

	memcpy(m_nBloodSlot, pSpiritData->nBloodSlot, sizeof(m_nBloodSlot));

	for (INT32 nPage = 0; nPage < MAX_BLOODSEA_PAGE; nPage++)
	{
		for (INT32 nIndex = 0; nIndex < MAX_BLOODSEA_SLOT_PER_PAGE; nIndex++)
		{
			if (m_nBloodSlot[nPage][nIndex] != 0)
			{
				const BLOOD_DATA* pBlood = CDataManager<BLOOD_DATA>::GetInstance().GetData(MAKE_BLOOD_KEY(nPage + 1, nIndex + 1, m_nBloodSlot[nPage][nIndex]));
				LOG_PROCESS_ERROR(pBlood);

				for (INT32 nAttrIndex = 0; nAttrIndex < MAX_BLOODSEA_ATTR_PER_SLOT; nAttrIndex++)		// Equip item
				{
					if (pBlood->nNodeID[nAttrIndex])
					{
						nRetCode = CModifierTree::GetInstance().ApplyExtraNode(pBlood->nNodeID[nAttrIndex], m_pOwner, NULL);
						LOG_PROCESS_ERROR(nRetCode);
					}
				}
			}
		}
	}

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CShieldList::Load_v_1(const char* pcData, UINT32 dwSize)
{
	m_byShieldNum = 0;

	const UINT8   *pCountPoint = NULL;
	const UINT32  *pRolePoint = NULL;

	LOG_PROCESS_ERROR(pcData);
	LOG_PROCESS_ERROR(dwSize <= sizeof(UINT8) + sizeof(UINT32) * MAX_SHIELD_NUM);

	pCountPoint = reinterpret_cast<const UINT8*>(pcData);
	m_byShieldNum = *pCountPoint;
	++pCountPoint;

	pRolePoint = reinterpret_cast<const UINT32*>(pCountPoint);
	for (INT32 i = 0; i < m_byShieldNum; ++i)
	{
		m_arrRoleID[i] = pRolePoint[i];
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMaster::OnLoadByGS_v_1(const char* pcData, UINT32 dwSize)
{
	ROLE_DB_MASTER_DATA* pMasterData = (ROLE_DB_MASTER_DATA*)pcData;
	m_dwCD = pMasterData->dwCDTimer;
    m_dwTwitterCD = pMasterData->dwTwitterCDTimer;
	m_bTwitter = pMasterData->byTwitter;
    m_dwStudentRewardTeacherID = pMasterData->dwStudentRewardTeacherID;
	m_nStudentRewardExp = pMasterData->nStudentRewardExp;

	for (int nIndex = 0; nIndex < MAX_STUDENT_NUM; nIndex++)
	{
		m_TeacherReward[nIndex].dwRoleID = pMasterData->TeacherReward[nIndex].dwRoleID;
		m_TeacherReward[nIndex].nExp = pMasterData->TeacherReward[nIndex].nExp;
		m_TeacherReward[nIndex].nMasterExp = pMasterData->TeacherReward[nIndex].nMasterExp;
	}

	return TRUE;
}

BOOL CRoleGuildData::Load_v_1(const char *pcData, UINT32 dwSize)
{
	const ROLE_GUILD_AURA_RECORD    *pRecord = (const ROLE_GUILD_AURA_RECORD*)pcData;

	m_dwCD = pRecord->dwGuildCD;
	m_nDonateMoneyPerDay = pRecord->nDonateMoneyPerDay;
	m_nDonateMoneyDayTime = pRecord->nDonateMoneyDayTime;
	m_nDonateResPerDay = pRecord->nDonateResPerDay;
	m_nDonateResDayTime = pRecord->nDonateResDayTime;

	for (INT32 i = 0; i < 3; ++i)
		m_Achievement[i] = pRecord->byAchievement[i];

	for (INT32 i = 0; i < pRecord->byRecordNum; ++i)
	{
		GUILD_BUY_AURA_RECORD   data;
		data.nType = pRecord->stData[i].nType;
		data.nTime = pRecord->stData[i].nTime;
		data.byCount = pRecord->stData[i].byCount;
		m_RecordList.push_back(data);
	}

	return TRUE;
}

BOOL CArena::Load_v_1(const char* pcData, UINT32 dwSize)
{
	INT32       nRetCode = 0;
	const ROLE_ARENA_RECORD *pData = (const ROLE_ARENA_RECORD*)pcData;

	LOG_PROCESS_ERROR(pcData);
	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(sizeof(ROLE_ARENA_RECORD) == dwSize);

	m_nJoinTime = pData->nJoinTime;
	m_bIsInSeason = pData->byIsInSeason;
	m_nSeasonNum = pData->nSeasonNum;
	m_byCurSeasonVictoryTimes = pData->byCurSeasonVictoryTimes;
	m_byPointSeasonVictoryTimes = pData->byPointSeasonVictoryTimes;
	m_nCurSeasonEndTime = pData->nCurSeasonEndTime;
	m_nPointSeasonEndTime = pData->nPointSeasonEndTime;
	m_nHP = pData->nHp;

	// send sync data to MS
	if (m_nJoinTime == CGameWorld::GetInstance().GetGameTime() / 86400)
	{
#ifndef _ROBOT
		nRetCode = CCliMessageHandler::GetInstance().DoMSArenaSyncData(m_pOwner->GetObjID(), m_byPointSeasonVictoryTimes, m_nPointSeasonEndTime);
		LOG_PROCESS_ERROR(nRetCode);
#endif
	}
	else
	{
		m_byPointSeasonVictoryTimes = 0;
		m_nPointSeasonEndTime = 0;
		m_nCurSeasonEndTime = 0;
		m_byCurSeasonVictoryTimes = 0;
		m_nSeasonNum = 0;
		m_bIsInSeason = FALSE;
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadPet_v_1(const char* pcData, UINT32 dwSize)
{
	ROLE_PET_LIST   *pPetData = NULL;

	LOG_PROCESS_ERROR(pcData);
	LOG_PROCESS_ERROR(sizeof(ROLE_PET_LIST) == dwSize);

	pPetData = (ROLE_PET_LIST *)pcData;
	memset(m_Pet, 0, sizeof(m_Pet));
	m_nPetCount = 0;

	//if (IsInJump() == TRUE)
	{
		m_nPetState = pPetData->byPetState;
		m_dwPetSaveTime = pPetData->dwSaveTime;
		for (INT32 i = 0; i < MAX_PET_PER_ROLE; ++i)
		{
			if (pPetData->PetList[i].wPetType == 0)
				break;
			m_Pet[i].nPetType = pPetData->PetList[i].wPetType;
			m_Pet[i].nLevel = pPetData->PetList[i].byPetLevel;
			m_Pet[i].nInitLevel = pPetData->PetList[i].byPetInitLevel;
			m_Pet[i].nMaxLevel = pPetData->PetList[i].byMaxPetLevel;
			m_Pet[i].dwNpcTemplateID = pPetData->PetList[i].dwNpcTemplateID;
			m_Pet[i].dwAliveTime = pPetData->PetList[i].dwAliveTime;
			m_Pet[i].nPetExp = pPetData->PetList[i].wPetExp;
			m_Pet[i].nCurHp = pPetData->PetList[i].nCurHp;

			++m_nPetCount;
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadMisc_v_1(const char* pcData, UINT32 dwSize)
{
	const ROLE_DB_MISC_DATA* pcMiscData = (const ROLE_DB_MISC_DATA*)pcData;

	m_nExp = pcMiscData->nExp;
	m_nExtraExp = pcMiscData->nExtraExp;
	m_nExpToday = pcMiscData->nExpToday;

	m_nActivity = pcMiscData->nActivity;
	m_nActivityReward = pcMiscData->nActivityReward;

	m_dwTeamID = 0;
	if (pcMiscData->dwTeamID > 0)
	{
		CTeamInGS* pstTeam = CGameWorld::GetInstance().FindTeam(pcMiscData->dwTeamID);
		if (pstTeam && pstTeam->GetCreateTime() < m_dwSaveTime)
		{
			for (INT32 i = 0; i < pstTeam->GetMaxMemberCount(); ++i)
			{
				if (pstTeam->GetMemberTable(i).dwRoleID == m_dwObjID)
				{
					m_dwTeamID = pcMiscData->dwTeamID;
					break;
				}
			}
		}
	}

	m_nPkMode = pcMiscData->byPkMode;
	m_dwPkValue = pcMiscData->dwPkValue;
	memcpy(m_dwVIPTime, pcMiscData->dwVIPTime, sizeof(m_dwVIPTime));
	m_nCurrentHp = pcMiscData->nCurrentHp;
	m_nCurrentMp = pcMiscData->nCurrentMp;

	m_dwLastCasterID = pcMiscData->dwLastCasterID;
	m_nKilledByPlayer = pcMiscData->nKilledByPlayer;

	{
		INT32 nTimeDiff = (INT32)pcMiscData->dwNextFreeReviveTime - (INT32)CGameWorld::GetInstance().GetGameTime();
		m_nNextFreeReviveFrm = nTimeDiff * GAME_FPS + CGameWorld::GetInstance().GetGameFrame();
	}

	{
		INT32 nTimeDiff = (INT32)pcMiscData->dwNextResetKilledByPlayerTime - (INT32)CGameWorld::GetInstance().GetGameTime();
		m_nNextResetKilledTimesByPlayerFrm = nTimeDiff * GAME_FPS + CGameWorld::GetInstance().GetGameFrame();
	}

	m_nGMLevel = pcMiscData->byGMLevel;
	if (strncmp(m_szUserName, g_ServerConfig.Common.szSuperGMUserName, USER_NAME_LEN) == 0)
		m_nGMLevel = MAX_GM_LEVEL;

	m_dwLastDailyRefreshTime = pcMiscData->dwLastDailyRefreshTime;
	m_dwLastWeeklyRefreshTime = pcMiscData->dwLastWeeklyRefreshTime;
	m_dwLastLineSceneTempID = pcMiscData->dwLastLineSceneTempID;
	m_dwLastLineSceneID = pcMiscData->dwLastLineSceneID;
	m_dwTotalExerciseTime = pcMiscData->dwTotalExerciseTime;
	m_dwStartExerciseTime = pcMiscData->dwStartExerciseTime;

	if (pcMiscData->cAntiCheatLevel < MAX_ROLE_ANTI_CHEAT_LEVEL)
		m_nAntiCheatLevel = pcMiscData->cAntiCheatLevel;
	else
		m_nAntiCheatLevel = 0;

	m_nAntiCheatPunishLevel = pcMiscData->cAntiCheatPunishLevel;
	m_nJigsawRemainTime = pcMiscData->nRemainJigsawTime;
	//	m_dwJigsawTimeoutTime = pcMiscData->dwJigsawTimeoutTime;
	m_dwJigsawSn = pcMiscData->dwJigsawSn;

	if (m_nJigsawRemainTime <= 0)
		StartAntiCheat();

	return TRUE;

}

BOOL CKillNpcQuest::Load_v_1(const char* pData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	ROLE_DB_KILL_NPC_QUEST_DATA* pQuestData = (ROLE_DB_KILL_NPC_QUEST_DATA*)pData;

	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(dwSize == sizeof(ROLE_DB_KILL_NPC_QUEST_DATA));

	memcpy(m_wKillNpcCount, pQuestData->wKillCount, sizeof(m_wKillNpcCount));
	memcpy(m_byCompleteList, pQuestData->byCompleteList, sizeof(m_byCompleteList));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadCoolDown_v_1(const char* pcData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	UINT32 dwCount = dwSize / sizeof(ROLE_COOLDOWN_DATA);

	LOG_PROCESS_ERROR(pcData);

	for (UINT32 i = 0; i < dwCount; i++)
	{
		ROLE_COOLDOWN_DATA* pData = (ROLE_COOLDOWN_DATA*)(pcData + i * sizeof(ROLE_COOLDOWN_DATA));

		AddCooldown(pData->nID, pData->nRemainFrm);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_LoadCheckIn_v_1(const char* pcData, UINT32 dwSize)
{
	ROLE_CHECKIN_DATA* pData = (ROLE_CHECKIN_DATA*)pcData;

	m_LastCheckInInfo.nIndex = pData->byIndex;
	m_LastCheckInInfo.dwLastCheckInTime = pData->dwLastCheckInTime;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CItemBag::LoadBlackMarketLimit_v_1(const char* pcData, UINT32 dwSize)
{
	const ROLE_BLACK_MARKET_LIMIT_DATA* pLimitData = (const ROLE_BLACK_MARKET_LIMIT_DATA*)pcData;

	for (INT32 i = 0; i < pLimitData->nLimitCount; i++)
	{
		INT32 bmpid = pLimitData->data[i].nBMProductID;
		m_black_market_limit_map[bmpid].nCount = pLimitData->data[i].nCount;
		m_black_market_limit_map[bmpid].dwNextRefreshTime = pLimitData->data[i].dwNextRefreshTime;
	}

	return TRUE;
}

BOOL CItemBag::LoadBlackMarketCooldown_v_1(const char* pcData, UINT32 dwSize)
{
	const ROLE_BLACK_MARKET_COOLDOWN_DATA* pCooldownData = (const ROLE_BLACK_MARKET_COOLDOWN_DATA*)pcData;

	for (INT32 i = 0; i < pCooldownData->nCooldownCount; i++)
	{
		INT32 bmpid = pCooldownData->data[i].nBMProductID;
		m_black_market_cooldown_map[bmpid] = pCooldownData->data[i].dwCooldown;
	}

	return TRUE;
}
