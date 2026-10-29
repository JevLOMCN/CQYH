#include "stdafx.h"
#include "role.h"

#include "zlib.h"
#include "role_db_def.h"
#include "game_world.h"
#include "init_reputation_data.h"
#include "spirit_data.h"
#include "guild_mgr.h"

BOOL CRole::CheckDBDataLength(void)
{
	ROLE_DB_BASE_DATA BaseData;
	ROLE_DB_ITEM_DATA Itemdata;
	ROLE_DB_AURA_DATA AuraData;
	FRIEND_GROUP_DATA GroupData;
	ROLE_DB_FRIEND_DATA FriendData;
	ROLE_DB_CIRCLE_QUEST_DATA CircleQuestData;
	ROLE_DB_QUEST_DATA QuestData;
	ROLE_DB_TITLE_DATA TitleData;
	ROLE_DB_ACHIEVEMENT_DATA AchievementData;
	ROLE_DB_DUNGEON_RECORD DungeonData;
	ROLE_DB_SPIRIT_DATA SpiritData;
	ROLE_HERO_DATA HeroData;
    ROLE_PET_LIST  RolePetList;
	ROLE_DB_KILL_NPC_QUEST_DATA	KillNpcQuestData;
    ROLE_GUILD_AURA_RECORD  RoleGuildAuraRecord;
	ROLE_DB_MISC_DATA MiscData;
	ROLE_DB_MASTER_DATA MasterData;
	ROLE_DB_GAMBLE_DATA GambleData;    // Added from copied code

	LOG_PROCESS_ERROR(sizeof(BaseData.szRoleName) == sizeof(BaseData.szRoleName[0]) * ROLE_NAME_LEN);
	LOG_PROCESS_ERROR(sizeof(BaseData.szUserName) == sizeof(BaseData.szUserName[0]) * USER_NAME_LEN);
	LOG_PROCESS_ERROR(sizeof(BaseData.nAvatar) == sizeof(BaseData.nAvatar[0]) * NES_VISIBLE_TOTAL);

	LOG_PROCESS_ERROR(sizeof(Itemdata.wRefineLevels) == sizeof(Itemdata.wRefineLevels[0]) * NES_TOTAL);
	LOG_PROCESS_ERROR(sizeof(Itemdata.llMoneys) == sizeof(Itemdata.llMoneys[0]) * MONEYTYPE_COUNT);
	LOG_PROCESS_ERROR(sizeof(Itemdata.nMoneyDailyEarned) == sizeof(Itemdata.nMoneyDailyEarned[0]) * MONEYTYPE_COUNT);
	LOG_PROCESS_ERROR(sizeof(Itemdata.nMoneyDailyUsed) == sizeof(Itemdata.nMoneyDailyUsed[0]) * MONEYTYPE_COUNT);
	LOG_PROCESS_ERROR(sizeof(Itemdata.nMoneyWeekleEarned) == sizeof(Itemdata.nMoneyWeekleEarned[0]) * MONEYTYPE_COUNT);
	LOG_PROCESS_ERROR(sizeof(Itemdata.nMoneyWeekleUsed) == sizeof(Itemdata.nMoneyWeekleUsed[0]) * MONEYTYPE_COUNT);

	LOG_PROCESS_ERROR(sizeof(AuraData.nX) == sizeof(AuraData.nX[0]) * MAX_EX_PARAM_COUNT);

	LOG_PROCESS_ERROR(sizeof(GroupData.szGroupName) == sizeof(GroupData.szGroupName[0]) * FRIEND_GROUP_NAME_LEN);

	LOG_PROCESS_ERROR(sizeof(FriendData.szSignature) == sizeof(FriendData.szSignature[0]) * SIGNATURE_LEN);

	LOG_PROCESS_ERROR(sizeof(CircleQuestData.nQuestID) == sizeof(CircleQuestData.nQuestID[0]) * MAX_CIRCLE_COUNT);
	LOG_PROCESS_ERROR(sizeof(CircleQuestData.nCompleteCount) == sizeof(CircleQuestData.nCompleteCount[0]) * MAX_CIRCLE_COUNT);
	LOG_PROCESS_ERROR(sizeof(CircleQuestData.nCircleNumber) == sizeof(CircleQuestData.nCircleNumber[0]) * MAX_CIRCLE_COUNT);
	LOG_PROCESS_ERROR(sizeof(CircleQuestData.nLastCompleteTime) == sizeof(CircleQuestData.nLastCompleteTime[0]) * MAX_CIRCLE_COUNT);

	LOG_PROCESS_ERROR(sizeof(QuestData.byNormalQuest) == sizeof(QuestData.byNormalQuest[0]) * MAX_NORMAL_QUEST_COUNT / 8);
	LOG_PROCESS_ERROR(sizeof(QuestData.byRepeatQuest) == sizeof(QuestData.byRepeatQuest[0]) * MAX_REPEAT_QUEST_COUNT);
	LOG_PROCESS_ERROR(sizeof(QuestData.AcceptQuestList[0].byOpenDoodadCount) ==
		sizeof(QuestData.AcceptQuestList[0].byOpenDoodadCount[0]) * MAX_CONDITION_PER_QUEST);
	LOG_PROCESS_ERROR(sizeof(QuestData.AcceptQuestList[0].byKillNpcCount) ==
		sizeof(QuestData.AcceptQuestList[0].byKillNpcCount[0]) * MAX_CONDITION_PER_QUEST);
	LOG_PROCESS_ERROR(sizeof(QuestData.AcceptQuestList[0].nVariable) ==
		sizeof(QuestData.AcceptQuestList[0].nVariable[0]) * MAX_VARIABLE_PER_QUEST);
	LOG_PROCESS_ERROR(sizeof(QuestData.AcceptQuestList[0].nKillPlayerCount) ==
		sizeof(QuestData.AcceptQuestList[0].nKillPlayerCount[0]) * CHECK_KILL_PLAYER_CONDITION);
	LOG_PROCESS_ERROR(sizeof(QuestData.AcceptQuestList[0].nKillBufPlayerCount) ==
		sizeof(QuestData.AcceptQuestList[0].nKillBufPlayerCount[0]) * CHECK_KILL_PLAYER_CONDITION);
	LOG_PROCESS_ERROR(sizeof(QuestData.AcceptQuestList[0].nCheckItemCount) ==
		sizeof(QuestData.AcceptQuestList[0].nCheckItemCount[0]) * MAX_CONDITION_PER_QUEST);

	LOG_PROCESS_ERROR(sizeof(AchievementData.wAchievement) == sizeof(AchievementData.wAchievement[0]) * ACHIEVEMENT_MAX);
	LOG_PROCESS_ERROR(sizeof(AchievementData.dwAchievementReward) == sizeof(AchievementData.dwAchievementReward[0]) * ACHIEVEMENT_MAX / 32);
	LOG_PROCESS_ERROR(sizeof(AchievementData.nVarList) == sizeof(AchievementData.nVarList[0]) * MAX_ACHIEVEMENT_STATISTIC_COUNT);
	LOG_PROCESS_ERROR(avdTotal <= MAX_ACHIEVEMENT_STATISTIC_COUNT);
	LOG_PROCESS_ERROR(sizeof(AchievementData.dwHandbook) == sizeof(AchievementData.dwHandbook[0]) * (htTotal));
	LOG_PROCESS_ERROR(sizeof(AchievementData.dwHandbook[0]) == sizeof(AchievementData.dwHandbook[0][0]) * MAX_HANKBOOK_COUNT_PER_TYPE / 32);

	LOG_PROCESS_ERROR(sizeof(DungeonData.byDungeonRecord) == sizeof(DungeonData.byDungeonRecord[0]) * MAX_SCENE_TEMPLATE_COUNT);

	LOG_PROCESS_ERROR(sizeof(SpiritData.nBagContainerTemplate) == sizeof(SpiritData.nBagContainerTemplate[0]) * MAX_AIRSEA_BAG_CONTAINER_SIZE);
	LOG_PROCESS_ERROR(sizeof(SpiritData.nBagContainerExp) == sizeof(SpiritData.nBagContainerExp[0]) * MAX_AIRSEA_BAG_CONTAINER_SIZE);
	LOG_PROCESS_ERROR(sizeof(SpiritData.nStallContainerTemplate) == sizeof(SpiritData.nStallContainerTemplate[0]) * MAX_AIRSEA_STALL_CONTAINER_SIZE);
	LOG_PROCESS_ERROR(sizeof(SpiritData.nStallContainerExp) == sizeof(SpiritData.nStallContainerExp[0]) * MAX_AIRSEA_STALL_CONTAINER_SIZE);
	LOG_PROCESS_ERROR(sizeof(SpiritData.nEquipContainerTemplate) == sizeof(SpiritData.nEquipContainerTemplate[0]) * MAX_AIRSEA_EQUIP_CONTAINER_SIZE);
	LOG_PROCESS_ERROR(sizeof(SpiritData.nEquipContainerExp) == sizeof(SpiritData.nEquipContainerExp[0]) * MAX_AIRSEA_EQUIP_CONTAINER_SIZE);
	LOG_PROCESS_ERROR(sizeof(SpiritData.nBloodSlot) == sizeof(SpiritData.nBloodSlot[0]) * MAX_BLOODSEA_PAGE);
	LOG_PROCESS_ERROR(sizeof(SpiritData.nBloodSlot[0]) == sizeof(SpiritData.nBloodSlot[0][0]) * MAX_BLOODSEA_SLOT_PER_PAGE);

	LOG_PROCESS_ERROR(sizeof(HeroData.szName) == sizeof(HeroData.szName[0]) * ROLE_NAME_LEN);

    LOG_PROCESS_ERROR(sizeof(RolePetList.PetList) == sizeof(RolePetList.PetList[0]) * MAX_PET_PER_ROLE);

	LOG_PROCESS_ERROR(sizeof(KillNpcQuestData.wKillCount) == sizeof(KillNpcQuestData.wKillCount[0]) * MAX_KILL_NPC_QUEST_COUNT);
    LOG_PROCESS_ERROR(sizeof(KillNpcQuestData.byCompleteList) == sizeof(KillNpcQuestData.byCompleteList[0]) * MAX_KILL_NPC_QUEST_COUNT);

    LOG_PROCESS_ERROR(sizeof(RoleGuildAuraRecord.byAchievement) == sizeof(RoleGuildAuraRecord.byAchievement[0]) * GUILD_MAX_ACHI_ARRAY_LENGTH);

	LOG_PROCESS_ERROR(sizeof(MiscData.dwVIPTime) == sizeof(MiscData.dwVIPTime[0]) * vlTotal);

	LOG_PROCESS_ERROR(sizeof(MasterData.TeacherReward) == sizeof(MasterData.TeacherReward[0]) * MAX_STUDENT_NUM);

	// Added from copied code
	LOG_PROCESS_ERROR(sizeof(GambleData.dwValue[0]) == sizeof(GambleData.dwValue[0][0]) * GAMBLE_LEVEL_COUNT);
	LOG_PROCESS_ERROR(sizeof(GambleData.dwValue) == sizeof(GambleData.dwValue[0]) * egtTotal);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::Save(char* pData, UINT32& rdwSize)
{
	INT32 nRetCode = 0;
	ROLE_DB_DATA* pRoleData = NULL;
	UINT32 dwBlockOffSet = 0;
	UINT32 dwBlockSize = 0;

	LOG_PROCESS_ERROR(pData);

	pRoleData = (ROLE_DB_DATA*)pData;
	pRoleData->dwLen = sizeof(ROLE_DB_DATA);
	pRoleData->dwVersion = ROLE_DB_DATA_CURRENT_VERSION;
	pRoleData->dwRoleID = GetObjID();

	for (UINT32 i = 0; i < rbtTotal; i++)
	{
		switch (i)
		{
		case rbtBase:
			nRetCode = _SaveBaseData(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtSpell:
			nRetCode = _SaveSpell(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtAura:
			nRetCode = _SaveAura(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtProcessedMails:
			nRetCode = m_ProMails.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtItem:
			nRetCode = m_ItemBag.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtQuest:
			nRetCode = m_QuestList.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtReputation:
			nRetCode = m_Reputation.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtSpellSlot:
			nRetCode = _SaveSpellSlot(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtStall:
			nRetCode = m_Stall.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtFriend:
			nRetCode = m_Friend.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtVariable:
			nRetCode = _SaveVariable(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtTitle:
			nRetCode = m_Title.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtAchievement:
			nRetCode = m_AchievementList.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtDungeonRecord:
			nRetCode = _SaveDungeonRecord(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtTeleportPlatform:
			nRetCode = _SaveTeleportPlatform(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtActivatedMount:
			nRetCode = _SaveMount(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtSpirit:
			nRetCode = m_Spirit.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtHero:
			nRetCode = _SaveHero(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtShield:
			nRetCode = m_ShieldList.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtMaster:
			nRetCode = m_Master.SaveByGS(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtGuild:
			nRetCode = m_Guild.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtMisc:
			nRetCode = _SaveMisc(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtArena:
			nRetCode = m_Arena.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
        case rbtPet:
            nRetCode = _SavePet(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
            break;
		case rbtKillNpcQuest:
			nRetCode = m_KillNpcQuest.Save(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtCoolDown:
			nRetCode = _SaveCoolDown(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
		case rbtCheckIn:
			nRetCode = _SaveCheckIn(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
			break;
        case rbtBlackMarketLimit:
            nRetCode = m_ItemBag.SaveBlackMarketLimit(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
            break;
        case rbtBlackMarketCooldown:
            nRetCode = m_ItemBag.SaveBlackMarketCooldown(&pRoleData->RoleData[dwBlockOffSet], dwBlockSize);
            break;
		case rbtGamble:    // Code still to be added
			break;
		default:
			LOG_PROCESS_ERROR(FALSE);
		}

		LOG_PROCESS_ERROR(nRetCode);

		pRoleData->BlockTable[i].btBlockType = i;
		pRoleData->BlockTable[i].dwStartOffset = dwBlockOffSet;
		pRoleData->BlockTable[i].dwBlockSize = dwBlockSize;

		dwBlockOffSet += dwBlockSize;
		pRoleData->dwLen += dwBlockSize;
		dwBlockSize = 0;
	}

	pRoleData->dwCRC = crc32(0, (const Bytef*)&(pRoleData->dwVersion), pRoleData->dwLen - sizeof(pRoleData->dwLen) - sizeof(pRoleData->dwCRC));

	rdwSize = pRoleData->dwLen;
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_SaveBaseData(const char* pcData, UINT32& rdwSize)
{
    CGSGuild* guild = nullptr;
	ROLE_DB_BASE_DATA* pBaseData = (ROLE_DB_BASE_DATA*)pcData;

	LOG_PROCESS_ERROR(pcData);

	pBaseData->byVersion = ROLE_DB_BASE_DATA_CURRENT_VERSION;
	pBaseData->dwRoleID = m_dwObjID;

	pBaseData->dwCreateTime = m_dwCreateTime;
	pBaseData->dwSaveTime = m_dwSaveTime = CGameWorld::GetInstance().GetGameTime();
	pBaseData->dwDeleteTime = 0;
	strxcpy(pBaseData->szRoleName, m_szName, ROLE_NAME_LEN);
	strxcpy(pBaseData->szUserName, m_szUserName, USER_NAME_LEN);

	pBaseData->byClass = (UINT8)GetClass();
	pBaseData->bySex = (UINT8)GetSex();
    pBaseData->byHairType = m_byHairType;
    pBaseData->byHairColor = m_byHairColor;
    pBaseData->byFaceType = m_byFaceType;
    pBaseData->bySkinColor = m_bySkinColor;
	pBaseData->byLevel = (UINT8)GetLevel();

	pBaseData->nSceneTempID = m_nSceneTempID;
	pBaseData->nSceneID = m_nSceneID;

	pBaseData->nX = (INT32)m_llX;
	pBaseData->nY = (INT32)m_llY;
	pBaseData->nZ = (INT32)m_llZ;
	pBaseData->wFaceDirection = (UINT16)m_nFaceDirection;

	pBaseData->wMountID = m_nCurrentMountID;

	pBaseData->byStaticReturnSceneTempID = m_StaticReturnPos.nSceneTempID;
	pBaseData->nStaticReturnSceneID = m_StaticReturnPos.nSceneID;
	pBaseData->nStaticReturnX = m_StaticReturnPos.nX;
	pBaseData->nStaticReturnY = m_StaticReturnPos.nY;
	pBaseData->nStaticReturnZ = m_StaticReturnPos.nZ;
	pBaseData->wStaticReturnFaceDirection = m_StaticReturnPos.nFaceDirection;

	pBaseData->byReviveSceneTempID = m_RevivePos.nSceneTempID;
	pBaseData->nReviveSceneID = m_RevivePos.nSceneID;
	pBaseData->nRevivePtX = m_RevivePos.nX;
	pBaseData->nRevivePtY = m_RevivePos.nY;
	pBaseData->nRevivePtZ = m_RevivePos.nZ;
	pBaseData->wReviveFaceDirection = m_RevivePos.nFaceDirection;

	GetItemBag()->GetAppearance(pBaseData->nAvatar);
	pBaseData->byRefineFx = GetItemBag()->GetRefineFx();

    guild = CGuildMgr::GetInstance().FindGuild(m_dwGuildID);
    if (guild)
        strxcpy(pBaseData->szGuildName, guild->GetName(), GUILD_NAME_LEN);
    else
        pBaseData->szGuildName[0] = 0;

    rdwSize = sizeof(ROLE_DB_BASE_DATA);

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CRole::_SaveVariable(char* pData, UINT32& rdwSize)
{
	INT32* pnCount = NULL;
	INT16* pwIndex = NULL;
	INT32* pnValue = NULL;

	LOG_PROCESS_ERROR(pData);

	pnCount = (INT32*)pData;
	pData += sizeof(INT32);

	*pnCount = 0;

	for (INT32 nIndex = 0; nIndex < MAX_ROLE_VARIABLE_COUNT; nIndex++)
	{
		if (m_nVarList[nIndex] != 0)
		{
			pwIndex = (INT16*)pData;
			pData += sizeof(INT16);

			pnValue = (INT32*)pData;
			pData += sizeof(INT32);

			*pwIndex = nIndex;
			*pnValue = m_nVarList[nIndex];
			(*pnCount)++;
		}
	}

	memcpy(pData, m_nClientVarList, sizeof(m_nClientVarList));

	rdwSize = sizeof(*pnCount) + *pnCount * (sizeof(INT16) + sizeof(INT32)) + sizeof(m_nClientVarList);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_SaveSpell(char* pData, UINT32& rdwSize)
{
	INT32 nRetCode = 0;
	INT32 i = 0;
	rdwSize = 0;

	m_SpellBook.BeginSpellSet();

	CSpellSet* e = NULL;

	while ((e = m_SpellBook.NextSpellSet()) != NULL)
	{
		if (!e->NeedSave())
			continue;

		ROLE_DB_SPELL_DATA* pDBData = (ROLE_DB_SPELL_DATA*)(pData + i*sizeof(ROLE_DB_SPELL_DATA));

		pDBData->wSpellID = e->GetID();
		pDBData->bySpellLevel = e->GetActualLevel();
		pDBData->wCurPts = e->GetPts();

		rdwSize += sizeof(ROLE_DB_SPELL_DATA);
		i++;
	}

	return TRUE;
}

BOOL CRole::_SaveAura(char* pDbData, UINT32& rdwSize)
{
	INT32 nRetCode = 0;
	INT32 nCount = 0;
	rdwSize = 0;
	CAuraInstance* poAura = NULL;

	for (INT32 i = auetNone; i < auetTotal; i++)
	{
		poAura = m_AuraList.BeginAura(i);

		while ((poAura = m_AuraList.NextAura(i, poAura)) != NULL)
		{
			if (!poAura->GetAttr(aatSaveToDB))
				continue;

			ROLE_DB_AURA_DATA* pData = (ROLE_DB_AURA_DATA*)(pDbData + nCount*sizeof(ROLE_DB_AURA_DATA));

			pData->byStackCount = poAura->GetStackCount();
			pData->dwCasterID = poAura->GetCasterID();
			pData->dwRemainFrm = poAura->GetRemainFrm();
			pData->wAuraID = poAura->GetID();
			pData->wRootSpellID = poAura->GetSpellID();
			pData->byRootSpellLevel = poAura->GetSpellLevel();

			for (INT32 j = 0; j < MAX_EX_PARAM_COUNT; j++)
				pData->nX[j] = poAura->GetExtraParam()->m_nX[j];

			nCount++;
			rdwSize += sizeof(ROLE_DB_AURA_DATA);
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_SaveSpellSlot(char* pData, UINT32& rdwSize)
{
	for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
	{
		ROLE_DB_SPELL_SLOT_DATA* pDBData = (ROLE_DB_SPELL_SLOT_DATA*)(pData + i*sizeof(ROLE_DB_SPELL_SLOT_DATA));
		pDBData->bySlotID = i;
		pDBData->wSpellID = m_SpellSlot[i].GetMainSpellID();
		pDBData->byLocked = m_SpellSlot[i].IsLocked();
		rdwSize += sizeof(ROLE_DB_SPELL_SLOT_DATA);
	}

	return TRUE;
}

BOOL CRole::_SaveMount(char* pMountData, UINT32& rdwSize)
{
	INT32 i = 0;
	rdwSize = 0;

	for (TActivatedMountList::iterator itr = m_ActivedMountList.begin(); itr != m_ActivedMountList.end(); ++itr)
	{
		ROLE_DB_ACTIVATED_MOUNT_DATA* pData = (ROLE_DB_ACTIVATED_MOUNT_DATA*)(pMountData + i*sizeof(ROLE_DB_ACTIVATED_MOUNT_DATA));

		pData->wMountID = *itr;
		rdwSize += sizeof(ROLE_DB_ACTIVATED_MOUNT_DATA);
		i++;
	}

	return TRUE;
}

BOOL CRole::_SaveHero(char* pData, UINT32& rdwSize)
{
	INT32 nRetCode = 0;
	rdwSize = sizeof(ROLE_HERO_DATA);

	ROLE_HERO_DATA* pHeroData = (ROLE_HERO_DATA*)pData;
	memset(pHeroData, 0, sizeof(ROLE_HERO_DATA));

	if (m_poHero != NULL)
	{
		pHeroData->dwID = m_poHero->GetObjID();
		pHeroData->byClass = m_poHero->GetClass();
		pHeroData->bySex = m_poHero->GetSex();
		pHeroData->byLevel = m_poHero->GetLevel();
		pHeroData->byAiType = m_poHero->GetAiType();
		pHeroData->dwExp = m_poHero->GetExp();
		pHeroData->nGrowth = m_poHero->GetGrowth();
		pHeroData->nCooldownRemainFrm = m_CoolDown.GetRemainFrm(MakeCoolDownID(COOLDOWN_MISC, COOLDOWN_MISC_HERO_REVIVE));
		pHeroData->byNameChanged = m_poHero->IsNameChanged();
		pHeroData->byIsSummoned = m_poHero->IsSummoned();
		strxcpy(pHeroData->szName, m_poHero->GetName(), sizeof(pHeroData->szName));
	}

	return TRUE;
}

BOOL CRole::_SaveDungeonRecord(char* pData, UINT32& rdwSize)
{
	ROLE_DB_DUNGEON_RECORD* poRecord = (ROLE_DB_DUNGEON_RECORD*)pData;

	memcpy(poRecord->byDungeonRecord, m_lstDungeonRecord, sizeof(ROLE_DB_DUNGEON_RECORD));

	rdwSize = sizeof(ROLE_DB_DUNGEON_RECORD);
	return TRUE;
}

BOOL CRole::_SaveTeleportPlatform(char* pData, UINT32& rdwSize)
{
	memcpy(pData, m_nTeleportPlatform, MAX_TELEPORT_PLATFORM_COUNT / 8);
	rdwSize = MAX_TELEPORT_PLATFORM_COUNT / 8;

	return TRUE;
}

BOOL CRole::_SaveMisc(char* pData, UINT32& rdwSize)
{
	ROLE_DB_MISC_DATA* pMiscData = (ROLE_DB_MISC_DATA*)pData;

	memset(pMiscData, 0, sizeof(ROLE_DB_MISC_DATA));

	pMiscData->nExp = GetExp();
	pMiscData->nExtraExp = m_nExtraExp;
	pMiscData->nExpToday = m_nExpToday;
	pMiscData->nActivity = m_nActivity;
	pMiscData->nActivityReward = m_nActivityReward;
	pMiscData->dwTeamID = GetTeamID();
	
	pMiscData->nCurrentHp = m_nCurrentHp;
	pMiscData->nCurrentMp = m_nCurrentMp;
	pMiscData->byPkMode = m_nPkMode;
	pMiscData->dwPkValue = m_dwPkValue;
	memcpy(pMiscData->dwVIPTime, m_dwVIPTime, sizeof(pMiscData->dwVIPTime));

	pMiscData->dwLastCasterID = m_dwLastCasterID;
	pMiscData->nKilledByPlayer = m_nKilledByPlayer;
	pMiscData->dwNextFreeReviveTime = CGameWorld::GetInstance().GetGameTime() + 
		(m_nNextFreeReviveFrm - CGameWorld::GetInstance().GetGameFrame()) / GAME_FPS;
	pMiscData->dwNextResetKilledByPlayerTime = CGameWorld::GetInstance().GetGameTime() + 
		(m_nNextResetKilledTimesByPlayerFrm - CGameWorld::GetInstance().GetGameFrame()) / GAME_FPS;

	pMiscData->byGMLevel = GetGMLevel();

	pMiscData->dwLastDailyRefreshTime = m_dwLastDailyRefreshTime;
	pMiscData->dwLastWeeklyRefreshTime = m_dwLastWeeklyRefreshTime;
	pMiscData->dwLastLineSceneTempID = m_dwLastLineSceneTempID;
	pMiscData->dwLastLineSceneID = m_dwLastLineSceneID;

    pMiscData->dwTotalExerciseTime = m_dwTotalExerciseTime;
    pMiscData->dwStartExerciseTime = m_dwStartExerciseTime;

	pMiscData->cAntiCheatLevel = m_nAntiCheatLevel;
	pMiscData->cAntiCheatPunishLevel = m_nAntiCheatPunishLevel;

//	pMiscData->dwNextJigsawTime = m_dwNextJigsawTime;
	pMiscData->nRemainJigsawTime = m_nJigsawRemainTime;
//	pMiscData->dwJigsawTimeoutTime = m_dwJigsawTimeoutTime;
	pMiscData->dwJigsawSn = m_dwJigsawSn;

	// Additional code
	pMiscData->dwGameTimeCard = m_dwGameTimeCard;
	pMiscData->nRuneNormal_Warrior   = m_nRuneNormal_Warrior  ;
	pMiscData->nRuneNormal_Mage      = m_nRuneNormal_Mage     ;
	pMiscData->nRuneNormal_Taoist    = m_nRuneNormal_Taoist   ;
	pMiscData->nRuneAdvanced_Warrior = m_nRuneAdvanced_Warrior;
	pMiscData->nRuneAdvanced_Mage    = m_nRuneAdvanced_Mage   ;
	pMiscData->nRuneAdvanced_Taoist  = m_nRuneAdvanced_Taoist ;
	pMiscData->nLuckRootCount        = m_nLuckRootCount;


	rdwSize += sizeof(ROLE_DB_MISC_DATA);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_SavePet(char* pData, UINT32& rdwSize)
{
    ROLE_PET_LIST   *pPetData = (ROLE_PET_LIST*)pData;

    memset(pPetData, 0, sizeof(ROLE_PET_LIST));
    pPetData->byPetState = GetPetState();
    pPetData->dwSaveTime = CGameWorld::GetInstance().GetGameTime() + g_GWConfig.nPetDisappearTime;
    for (INT32 i = 0; i < m_nPetCount && i < MAX_PET_PER_ROLE; ++i)
    {
        pPetData->PetList[i].wPetType = (UINT16)m_Pet[i].nPetType;
        pPetData->PetList[i].byPetLevel = (UINT8)m_Pet[i].nLevel;
        pPetData->PetList[i].byPetInitLevel = (UINT8)m_Pet[i].nInitLevel;
        pPetData->PetList[i].byMaxPetLevel = (UINT8)m_Pet[i].nMaxLevel;
        pPetData->PetList[i].dwNpcTemplateID = m_Pet[i].dwNpcTemplateID;
        pPetData->PetList[i].dwAliveTime = m_Pet[i].dwAliveTime;
        pPetData->PetList[i].wPetExp = (UINT16)m_Pet[i].nPetExp;
        pPetData->PetList[i].nCurHp = m_Pet[i].nCurHp;
    }
    rdwSize += sizeof(ROLE_PET_LIST);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CAchievementsList::Save(const char* pData, UINT32& rdwSize)
{
	ROLE_DB_ACHIEVEMENT_DATA* pAchievementData = (ROLE_DB_ACHIEVEMENT_DATA*)pData;
	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(pAchievementData);

	memcpy(pAchievementData->wAchievement, m_wAchievement, sizeof(pAchievementData->wAchievement));
	memcpy(pAchievementData->dwAchievementReward, m_dwAchievementReward, sizeof(pAchievementData->dwAchievementReward));
	memcpy(pAchievementData->nVarList, m_nAchievementStatistics, sizeof(pAchievementData->nVarList));
	memcpy(pAchievementData->dwHandbook, m_dwHandbook, sizeof(pAchievementData->dwHandbook));

	pAchievementData->nAchievementCount = m_nCompletedCount;

	rdwSize = sizeof(ROLE_DB_ACHIEVEMENT_DATA);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CFriend::Save(const char* pcData, UINT32& rdwSize)
{
	ROLE_DB_FRIEND_DATA* pFriendData = (ROLE_DB_FRIEND_DATA*)pcData;

	LOG_PROCESS_ERROR(pcData);
	LOG_PROCESS_ERROR(m_friends.size() <= MAX_FRIEND_COUNT);

	strxcpy(pFriendData->szSignature, m_szSignature, SIGNATURE_LEN);
	pFriendData->byFriendGroupCount = 0;

	for (UINT8 i = 0; i < MAX_FRIEND_GROUP_COUNT; i++)
	{
		if (strlen(m_szGroups[i]) > 0)
		{
			pFriendData->stFriendGroupData[pFriendData->byFriendGroupCount].byGroupID = i;
			strxcpy(pFriendData->stFriendGroupData[pFriendData->byFriendGroupCount].szGroupName, m_szGroups[i], FRIEND_GROUP_NAME_LEN);
			pFriendData->byFriendGroupCount++;
		}
	}

	rdwSize = sizeof(ROLE_DB_FRIEND_DATA) + sizeof(FRIEND_GROUP_DATA) * pFriendData->byFriendGroupCount;

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CArena::Save(const char* pcData, UINT32& rdwSize)
{
	ROLE_ARENA_RECORD *pData = (ROLE_ARENA_RECORD*)pcData;

	LOG_PROCESS_ERROR(pcData);
	LOG_PROCESS_ERROR(m_pOwner);

	pData->nJoinTime = m_nJoinTime;
	pData->byIsInSeason = m_bIsInSeason;
	pData->nSeasonNum = m_nSeasonNum;
	pData->byCurSeasonVictoryTimes = m_byCurSeasonVictoryTimes;
	pData->byPointSeasonVictoryTimes = m_byPointSeasonVictoryTimes;
	pData->nCurSeasonEndTime = m_nCurSeasonEndTime;
	pData->nPointSeasonEndTime = m_nPointSeasonEndTime;
	pData->nHp = m_nHP;

	rdwSize = sizeof(ROLE_ARENA_RECORD);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRoleGuildData::Save(char *pData, UINT32 &rdwSize)
{
    UINT32  nCurrentDay = CGameWorld::GetInstance().GetGameTime() + get_time_zone();
    nCurrentDay /= ONE_DAY_SECOND;

	ROLE_GUILD_AURA_RECORD *pRecord = (ROLE_GUILD_AURA_RECORD*)pData;

    pRecord->dwGuildCD = m_dwCD;
    pRecord->nDonateMoneyPerDay = m_nDonateMoneyPerDay;
    pRecord->nDonateMoneyDayTime = m_nDonateMoneyDayTime;
    pRecord->nDonateResPerDay = m_nDonateResPerDay;
    pRecord->nDonateResDayTime = m_nDonateResDayTime;

    for (INT32 i = 0; i < GUILD_MAX_ACHI_ARRAY_LENGTH; ++i)
        pRecord->byAchievement[i] = m_Achievement[i];

    pRecord->byRecordNum = 0;
	for (GUILD_AURA__RECORD_LIST::iterator iter = m_RecordList.begin(); iter != m_RecordList.end(); ++iter)
	{
        if (nCurrentDay == iter->nTime)
		{
			pRecord->stData[pRecord->byRecordNum].nType = iter->nType;
			pRecord->stData[pRecord->byRecordNum].nTime = iter->nTime;
			pRecord->stData[pRecord->byRecordNum].byCount = iter->byCount;
			++pRecord->byRecordNum;
		}
	}

	rdwSize = sizeof(ROLE_GUILD_AURA_RECORD) + pRecord->byRecordNum * sizeof(DB_GUILD_BUY_AURA_RECORD);

	return TRUE;
}

BOOL CProMails::Save(const char* pcData, UINT32& rdwSize)
{
	ROLE_DB_PROCESSED_MAILS* pMailData = (ROLE_DB_PROCESSED_MAILS*)pcData;

	while (m_lstProcessedMailID.size() >= 10)
		m_lstProcessedMailID.pop_front();

	pMailData->byMailCount = 0;
	for (std::list<UINT32>::iterator it = m_lstProcessedMailID.begin(); it != m_lstProcessedMailID.end(); ++it)
	{
		pMailData->dwMailID[pMailData->byMailCount] = *it;
		pMailData->byMailCount++;
	}

	rdwSize = sizeof(ROLE_DB_PROCESSED_MAILS) + sizeof(UINT32) * pMailData->byMailCount;

	return TRUE;
}

BOOL CStall::Save(const char* pcData, UINT32& rdwSize)
{
	ROLE_DB_STALL_DATA* pData = (ROLE_DB_STALL_DATA*)pcData;

	LOG_PROCESS_ERROR(pData);
    pData->dwUpgradeExpireTime = m_dwUpgradeExpireTime;

	rdwSize = sizeof(ROLE_DB_STALL_DATA);

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CReputation::Save(char* pData, UINT32& rdwSize)
{
	ROLE_DB_REPUTATION* pReputation = nullptr;
	LOG_PROCESS_ERROR(pData);

	rdwSize = 0;

	*(UINT64*)pData = m_qwActiveState;
	pData += sizeof(UINT64);
	rdwSize += sizeof(UINT64);

	*(UINT64*)pData = m_qwAwardState;
	pData += sizeof(UINT64);
	rdwSize += sizeof(UINT64);

	pReputation = (ROLE_DB_REPUTATION*)pData;
	for (INT32 nForceID = 0; nForceID < MAX_NPC_FORCE_ID; nForceID++)
	{
		const INIT_REPUTATION_DATA* pcData = CDataManager<INIT_REPUTATION_DATA>::GetInstance().GetData(nForceID);
		LOG_PROCESS_ERROR(pcData);

		if (m_nValueList[nForceID] != pcData->nInitValue)
		{
			pReputation->byForceID = (UINT8)nForceID;
			pReputation->nReputation = m_nValueList[nForceID];

			pReputation++;
			rdwSize += sizeof(*pReputation);
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTitle::Save(const char* pcData, UINT32& rdwSize)
{
	using namespace std;
	int i = 0;
	ROLE_DB_TITLE_DATA* pTitleDBData = (ROLE_DB_TITLE_DATA*)pcData;

	pTitleDBData->nEquipedTitleID = m_nEquipedTitleID;
	pTitleDBData->nTitleDataNum = m_Title_Map.size();

	for (TITLE_MAP::iterator iter = m_Title_Map.begin(); iter != m_Title_Map.end(); ++iter, ++i)
	{
		pTitleDBData->stTitleList[i].m_nTemplateID = iter->second.m_nTemplateID;
		pTitleDBData->stTitleList[i].m_nExpireTime = iter->second.m_nExpireTime;
	}

	rdwSize = sizeof(ROLE_DB_TITLE_DATA) + pTitleDBData->nTitleDataNum * sizeof(TITLE_DB_DATA);

	return TRUE;
}

BOOL CItemBag::Save(const char* pcData, UINT32& rdwSize)
{
	ROLE_DB_ITEM_DATA* pItemData = (ROLE_DB_ITEM_DATA*)pcData;

	LOG_PROCESS_ERROR(pcData);

	memcpy(pItemData->wRefineLevels, m_refine_levels, sizeof(m_refine_levels));

	for (INT32 i = 0; i < MONEYTYPE_COUNT; i++)
	{
		pItemData->llMoneys[i] = m_moneys[i];
		pItemData->nMoneyDailyEarned[i] = m_moneys_daily_earned[i];
		pItemData->nMoneyDailyUsed[i] = m_moneys_daily_used[i];
		pItemData->nMoneyWeekleEarned[i] = m_moneys_weekly_earned[i];
		pItemData->nMoneyWeekleUsed[i] = m_moneys_weekly_used[i];
	}
	pItemData->byUseMoneytype01Only = m_use_moneytype01_only;
    pItemData->nGoldPool = m_gold_pool;

	pItemData->byDefaultSlotSize = m_con_sizes[CONSLOT_DEFAULT];
	pItemData->byBankSlotSize = m_con_sizes[CONSLOT_BANK];
	pItemData->byResourceSlotSize = m_con_sizes[CONSLOT_RESOURCE];

    pItemData->dwWeaponLevelUpStartTime = m_dwWeaponLevelUpStartTime;
    pItemData->nWeaponDur = m_nWeaponDur;
    pItemData->nWeaponPhy = m_nWeaponPhy;
    pItemData->nWeaponMag = m_nWeaponMag;
    pItemData->nWeaponTao = m_nWeaponTao;
    pItemData->byMailNotified = m_byMailNotified;
    pItemData->nExpBottleTimes = m_nExpBottleTimes;

	pItemData->dwItemInfoCount = 0;
	pItemData->dwItemInfoBufLen = 0;

	for (int i = 0; i < CONSLOT_COUNT; i++)
	{
		Container& con = m_cons[i];
		for (ConIt it = con.begin(); it != con.end(); ++it)
		{
			Item* item = it->second;
			pItemData->dwItemInfoBufLen += item->m_iteminfo.Save(pItemData->acItemInfos + pItemData->dwItemInfoBufLen);
			pItemData->dwItemInfoCount++;
		}
	}

    if (m_WeaponItem)
    {
        pItemData->dwItemInfoBufLen += m_WeaponItem->m_iteminfo.Save(pItemData->acItemInfos + pItemData->dwItemInfoBufLen);
        pItemData->dwItemInfoCount++;
    }
	rdwSize = sizeof(ROLE_DB_ITEM_DATA) + pItemData->dwItemInfoBufLen;

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CItemBag::SaveBlackMarketLimit(const char* pcData, UINT32& rdwSize)
{
    ROLE_BLACK_MARKET_LIMIT_DATA* pLimitData = (ROLE_BLACK_MARKET_LIMIT_DATA*)pcData;

    LOG_PROCESS_ERROR(pcData);

    UpdateBlackMarketLimit();

    pLimitData->nLimitCount = 0;

    for (std::map<INT32, BMRoleLimitInfo>::const_iterator it = m_black_market_limit_map.begin(); it != m_black_market_limit_map.end(); ++it)
    {
        pLimitData->data[pLimitData->nLimitCount].nBMProductID = it->first;
        pLimitData->data[pLimitData->nLimitCount].nCount = it->second.nCount;
        pLimitData->data[pLimitData->nLimitCount].dwNextRefreshTime = it->second.dwNextRefreshTime;
        pLimitData->nLimitCount++;
    }

    rdwSize = sizeof(ROLE_BLACK_MARKET_LIMIT_DATA) + sizeof(ROLE_BLACK_MARKET_LIMIT_DATA::BMLimitData) * pLimitData->nLimitCount;

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CItemBag::SaveBlackMarketCooldown(const char* pcData, UINT32& rdwSize)
{
    ROLE_BLACK_MARKET_COOLDOWN_DATA* pCooldownData = (ROLE_BLACK_MARKET_COOLDOWN_DATA*)pcData;

    LOG_PROCESS_ERROR(pcData);

    UpdateBlackMarketCooldown();

    pCooldownData->nCooldownCount = 0;

    for (std::map<INT32, UINT32>::const_iterator it = m_black_market_cooldown_map.begin(); it != m_black_market_cooldown_map.end(); ++it)
    {
        pCooldownData->data[pCooldownData->nCooldownCount].nBMProductID = it->first;
        pCooldownData->data[pCooldownData->nCooldownCount].dwCooldown = it->second;
        pCooldownData->nCooldownCount++;
    }

    rdwSize = sizeof(ROLE_BLACK_MARKET_COOLDOWN_DATA) + sizeof(ROLE_BLACK_MARKET_COOLDOWN_DATA::BMCooldownData) * pCooldownData->nCooldownCount;

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CQuestList::Save(const char * pData, UINT32& rdwSize)
{
	UINT32 dwCompleteDay = 0;
	UINT32 dwCurrentDay = 0;
	INT32 nRetCode = 0;

	ROLE_DB_QUEST_DATA* pQuestData = (ROLE_DB_QUEST_DATA*)pData;

	LOG_PROCESS_ERROR(pData);

	memcpy(pQuestData->byNormalQuest, m_byNormalQuest, sizeof(pQuestData->byNormalQuest));
	memcpy(pQuestData->byRepeatQuest, m_byRepeatQuest, sizeof(pQuestData->byRepeatQuest));
	rdwSize += sizeof(ROLE_DB_QUEST_DATA);

	pQuestData->dwRepeatQuestScanTime = m_dwRepeatQuestScanTime;
	pQuestData->bRecodeKillPlayer = m_bRecodeKillPlayer;

	dwCompleteDay = (m_dwLastCompleteEmergencyQuestTime + CGameWorld::GetInstance().GetTimeZone()) / 86400;
	dwCurrentDay = (CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone()) / 86400;
	if (dwCompleteDay != dwCurrentDay)
	{
		m_nCompleteEmergencyQuestCount = 0;
		m_dwLastCompleteEmergencyQuestTime = CGameWorld::GetInstance().GetGameTime();
	}

	pQuestData->byCompleteEmergencyQuestCount = m_nCompleteEmergencyQuestCount;
	pQuestData->dwLastCompleteEmergencyQuestTime = m_dwLastCompleteEmergencyQuestTime;

	pQuestData->byAcceptedQuestCount = 0;
	for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedNormalQuestList.begin();
		it != m_AcceptedNormalQuestList.end(); ++it)
	{
		ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST& rAcceptedQuest =
			pQuestData->AcceptQuestList[pQuestData->byAcceptedQuestCount++];

		rAcceptedQuest.nQuestID = it->nQuestID;
		rAcceptedQuest.dwAcceptTime = it->dwAcceptTime;
		for (INT32 nKillIndex = 0; nKillIndex < MAX_CONDITION_PER_QUEST; nKillIndex++)
		{
			rAcceptedQuest.byKillNpcCount[nKillIndex] = (UINT8)it->nKillNpcCount[nKillIndex];
			rAcceptedQuest.byOpenDoodadCount[nKillIndex] = (UINT8)it->nKillDoodadCount[nKillIndex];
			rAcceptedQuest.nCheckItemCount[nKillIndex] = (UINT8)it->nCheckItemCount[nKillIndex];
		}

		for (INT32 nKillIndex = 0; nKillIndex < CHECK_KILL_PLAYER_CONDITION; nKillIndex++)
		{
			rAcceptedQuest.nKillPlayerCount[nKillIndex] = (UINT8)it->nKillPlayerCount[nKillIndex];
			rAcceptedQuest.nKillBufPlayerCount[nKillIndex] = (UINT8)it->nKillBufPlayerCount[nKillIndex];
		}

		memcpy(rAcceptedQuest.nVariable, it->nVariable, sizeof(rAcceptedQuest.nVariable));

		rdwSize += sizeof(ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST);
	}

	for (ACCEPTED_QUEST_LIST::iterator it = m_AcceptedEmergencyQuestList.begin();
		it != m_AcceptedEmergencyQuestList.end(); ++it)
	{
		ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST& rAcceptedQuest =
			pQuestData->AcceptQuestList[pQuestData->byAcceptedQuestCount++];

		rAcceptedQuest.nQuestID = it->nQuestID;
		rAcceptedQuest.dwAcceptTime = it->dwAcceptTime;
		for (INT32 nKillIndex = 0; nKillIndex < MAX_CONDITION_PER_QUEST; nKillIndex++)
		{
			rAcceptedQuest.byKillNpcCount[nKillIndex] = (UINT8)it->nKillNpcCount[nKillIndex];
			rAcceptedQuest.byOpenDoodadCount[nKillIndex] = (UINT8)it->nKillDoodadCount[nKillIndex];
			rAcceptedQuest.nCheckItemCount[nKillIndex] = (UINT8)it->nCheckItemCount[nKillIndex];
		}

		for (INT32 nKillIndex = 0; nKillIndex < CHECK_KILL_PLAYER_CONDITION; nKillIndex++)
		{
			rAcceptedQuest.nKillPlayerCount[nKillIndex] = (UINT8)it->nKillPlayerCount[nKillIndex];
			rAcceptedQuest.nKillBufPlayerCount[nKillIndex] = (UINT8)it->nKillBufPlayerCount[nKillIndex];
		}

		memcpy(rAcceptedQuest.nVariable, it->nVariable, sizeof(rAcceptedQuest.nVariable));

		rdwSize += sizeof(ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST);
	}

	/*************************** circle quest *************************************/
	pQuestData->stCircleQuestData.byCircleIndex = m_dwCircleIndex;

	for (INT32 nIndex = 0; nIndex < MAX_CIRCLE_COUNT; nIndex++)
	{
		pQuestData->stCircleQuestData.nCircleNumber[nIndex] = m_stCircleQuest[nIndex].nCircleNumber;
		pQuestData->stCircleQuestData.nCompleteCount[nIndex] = m_stCircleQuest[nIndex].nCompleteCount;
		pQuestData->stCircleQuestData.nLastCompleteTime[nIndex] = m_stCircleQuest[nIndex].nLastCompleteTime;
		pQuestData->stCircleQuestData.nQuestID[nIndex] = m_stCircleQuest[nIndex].nQuestID;
	}

	/*************************** reward quest *************************************/
	pQuestData->stDailyQuestData.dwRewardBoughtTime = m_dwRewardBoughtTime;
	pQuestData->stDailyQuestData.dwRewardQuestBrushCDTime = m_dwRewardQuestBrushCDTime;
	pQuestData->stDailyQuestData.nRewardIndex = m_nRewardIndex;
	pQuestData->stDailyQuestData.nRewardQuestRemainCompleteTime = m_nRewardQuestRemainCompleteTime;
	pQuestData->stDailyQuestData.qwRewardAccepted = m_qwRewardAccepted;
	pQuestData->stDailyQuestData.qwRewardCompleted = m_qwRewardCompleted;
	pQuestData->stDailyQuestData.qwRewardCompleted = m_qwRewardCompleted;
	pQuestData->stDailyQuestData.qwRewardQuest = m_qwRewardQuest;
	pQuestData->stDailyQuestData.nCompleteCount = m_nRewardCompleteCount;
	pQuestData->stDailyQuestData.nDailyRewwardBoughtTime = m_nDailyRewwardBoughtTime;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSpirit::Save(const char* pData, UINT32& rdwSize)
{
	INT32 nRetCode = 0;

	ROLE_DB_SPIRIT_DATA* pSpiritRoleDBData = (ROLE_DB_SPIRIT_DATA*)pData;
	const SPIRIT_DATA* pSpiritData = NULL;
	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(pSpiritRoleDBData);

	/********* overral *********************/

	pSpiritRoleDBData->byLevel = m_nLevel;
	pSpiritRoleDBData->nExp = m_nExp;
	pSpiritRoleDBData->nMoneyCost = m_nTotalMoneyCost;

	pSpiritRoleDBData->dwDailyUseMoneyChangeCount = m_dwDailyUseMoneyChangeCount;
	pSpiritRoleDBData->dwNextScanTimer = m_dwNextScanTimer;

	if (m_dwNextExp2PtsTime > CGameWorld::GetInstance().GetGameTime())
	{
		pSpiritRoleDBData->dwExp2PtsRemainTime = m_dwNextExp2PtsTime - CGameWorld::GetInstance().GetGameTime();
	}
	else
	{
		pSpiritRoleDBData->dwExp2PtsRemainTime = 0;
	}

	/********* airsearal *********************/

	pSpiritRoleDBData->nProductAirLevel = m_nProductAirLevel;

	pSpiritRoleDBData->nBagContainerSize = m_nBagContainerSize;
	pSpiritRoleDBData->nStallContainerSize = m_nStallContainerSize;
	pSpiritRoleDBData->nEquipContainerSize = m_nEquipContainerSize;

	for (INT32 nIndex = 0; nIndex < MAX_AIRSEA_BAG_CONTAINER_SIZE; nIndex++)
	{
		pSpiritRoleDBData->nBagContainerTemplate[nIndex] = m_BagContainer[nIndex].nTemplateID;
		pSpiritRoleDBData->nBagContainerExp[nIndex] = m_BagContainer[nIndex].nExp;
	}

	for (INT32 nIndex = 0; nIndex < MAX_AIRSEA_STALL_CONTAINER_SIZE; nIndex++)
	{
		pSpiritRoleDBData->nStallContainerTemplate[nIndex] = m_StallContainer[nIndex].nTemplateID;
		pSpiritRoleDBData->nStallContainerExp[nIndex] = m_StallContainer[nIndex].nExp;
	}

	for (INT32 nIndex = 0; nIndex < MAX_AIRSEA_EQUIP_CONTAINER_SIZE; nIndex++)
	{
		pSpiritRoleDBData->nEquipContainerTemplate[nIndex] = m_EquipContainer[nIndex].nTemplateID;
		pSpiritRoleDBData->nEquipContainerExp[nIndex] = m_EquipContainer[nIndex].nExp;
	}

	/**********  blood sea *************/
	memcpy(pSpiritRoleDBData->nBloodSlot, m_nBloodSlot, sizeof(pSpiritRoleDBData->nBloodSlot));

	pSpiritRoleDBData->byBloodState = m_nBloodState;

	rdwSize += sizeof(ROLE_DB_SPIRIT_DATA);

	return TRUE;

Exit0:
	return FALSE;
}

BOOL CKillNpcQuest::Save(const char* pData, UINT32& rdwSize)
{
	ROLE_DB_KILL_NPC_QUEST_DATA* pKillNpcQuestData = (ROLE_DB_KILL_NPC_QUEST_DATA*)pData;

	LOG_PROCESS_ERROR(pData);

	memcpy(pKillNpcQuestData->wKillCount, m_wKillNpcCount, sizeof(pKillNpcQuestData->wKillCount));
	memcpy(pKillNpcQuestData->byCompleteList, m_byCompleteList, sizeof(pKillNpcQuestData->byCompleteList));

	rdwSize = sizeof(ROLE_DB_KILL_NPC_QUEST_DATA);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_SaveCoolDown(char* pData, UINT32& rdwSize)
{
	INT32 nRetCode = 0;
	INT32 nKey = 0;
	UINT32 nTimeupFrm = 0;

	ROLE_COOLDOWN_DATA* pCD = (ROLE_COOLDOWN_DATA*)pData;

	m_CoolDown.Begin();

	while (m_CoolDown.Next(nKey, nTimeupFrm))
	{
		pCD->nID = nKey;
		pCD->nRemainFrm = nTimeupFrm - CGameWorld::GetInstance().GetGameFrame();

		if (pCD->nRemainFrm <= 0)
			continue;

		pCD++;
		rdwSize += sizeof(ROLE_COOLDOWN_DATA);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRole::_SaveCheckIn(char* pData, UINT32& rdwSize)
{
	ROLE_CHECKIN_DATA* pCheckInData = (ROLE_CHECKIN_DATA*)pData;
	LOG_PROCESS_ERROR(pData);

	pCheckInData->byIndex = m_LastCheckInInfo.nIndex;
	pCheckInData->dwLastCheckInTime = m_LastCheckInInfo.dwLastCheckInTime;

	rdwSize = sizeof(ROLE_CHECKIN_DATA);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMaster::SaveByGS(char* pData, UINT32& rdwSize)
{
	ROLE_DB_MASTER_DATA* pMasterData = (ROLE_DB_MASTER_DATA*)pData;
    pMasterData->dwTwitterCDTimer = m_dwTwitterCD;
	pMasterData->dwCDTimer = m_dwCD;
	pMasterData->byTwitter = m_bTwitter;
    pMasterData->dwStudentRewardTeacherID = m_dwStudentRewardTeacherID;
	pMasterData->nStudentRewardExp = m_nStudentRewardExp;

	for (int nIndex = 0; nIndex < MAX_STUDENT_NUM; nIndex++)
	{
		pMasterData->TeacherReward[nIndex].dwRoleID = m_TeacherReward[nIndex].dwRoleID;
		pMasterData->TeacherReward[nIndex].nExp = m_TeacherReward[nIndex].nExp;
		pMasterData->TeacherReward[nIndex].nMasterExp = m_TeacherReward[nIndex].nMasterExp;
	}

	rdwSize += sizeof(ROLE_DB_MASTER_DATA);

	return TRUE;
}

BOOL CGamble::Save(const char* pcData, UINT32& rdwSize)
{
	ROLE_DB_GAMBLE_DATA* pGambleData = (ROLE_DB_GAMBLE_DATA*)pcData;
	for (INT32 i = 0; i < egtTotal; i++)
	{
		for (INT32 j = 0; j < GAMBLE_LEVEL_COUNT; j++)
		{
			pGambleData->dwValue[i][j] = dwValue[i][j];
			pGambleData->dwTotalValue[i][j] = dwTotalValue[i][j];
		}
	}

	rdwSize += sizeof(ROLE_DB_GAMBLE_DATA);

	return TRUE;
}

