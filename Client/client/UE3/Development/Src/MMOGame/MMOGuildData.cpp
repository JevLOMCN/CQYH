#include "stdafx.h"
#include "MMOGuildData.h"
#include "MMOFacade.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOChatManager.h"
#include "MMOMsgBoxManager.h"
#include "CommonGameEvents.h"
#include "UITextParser.h"
#include "MMOMsgButtonManager.h"
#include "MMOQuestManager.h"
#include "MMOGuildManager.h"
#include "MMOMapDataManager.h"

using namespace ExternalMessage;

//////////////////////////////////////////////////////////////////////////
GuildBuildingData* GuildBuildingDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void GuildBuildingDataProcessor::DestroyDataObject(GuildBuildingData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void GuildBuildingDataProcessor::ParseData(GuildBuildingData* obj, class CTabFile* dataRow, int rowIdx)
{
	GuildBuildingData data;
	data.BuildingType = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("ID"), dataRow, rowIdx);
	data.Name = TReadDataValue_FString<GuildBuildingDataManager>(TEXT("Name"), dataRow, rowIdx);
	data.Level = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("Level"), dataRow, rowIdx);
	data.MaxLevel = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("MaxLevel"), dataRow, rowIdx);
	data.MaxNum = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("MaxNum"), dataRow, rowIdx);
	data.BaseNpcId = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("CdtnBaseNpcID"), dataRow, rowIdx);
	data.NpcId = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("NpcTemplateID"), dataRow, rowIdx);
	data.GuildMoneyReq = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("MoneyConsumption"), dataRow, rowIdx);
	data.HerbReq = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("HerbConsumption"), dataRow, rowIdx);
	data.WoodReq = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("WoodConsumption"), dataRow, rowIdx);
	data.CrystalReq = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("CrystalConsumption"), dataRow, rowIdx);
	data.SilverReq = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("SilverConsumption"), dataRow, rowIdx);
	data.UpgradeTime = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("UpgradeTime"), dataRow, rowIdx);
	data.RoutineReq = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("RoutineConsumption"), dataRow, rowIdx);
	data.PreBuildingType = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("ConditionBuildingID"), dataRow, rowIdx);
	data.PreBuildingLevel = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("ConditionBuildingLvl"), dataRow, rowIdx);
	data.UpgradeSpellID = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("UpgradingSpellID"), dataRow, rowIdx);
	data.Param1 = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("Param1"), dataRow, rowIdx);
	data.Param2 = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("Param2"), dataRow, rowIdx);
	data.Param3 = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("Param3"), dataRow, rowIdx);
	data.Param4 = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("Param4"), dataRow, rowIdx);
	data.Param5 = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("Param5"), dataRow, rowIdx);
	data.Effect = TReadDataValue_Int<GuildBuildingDataManager>(TEXT("Effect"), dataRow, rowIdx);
	data.Description = TReadDataValue_FString<GuildBuildingDataManager>(TEXT("Description"), dataRow, rowIdx);

	MMOGuildData::getSingleton().AddGuildBuildingData(data);
}

void GuildBuildingDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
GuildWelfareData* GuildWelfareDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void GuildWelfareDataProcessor::DestroyDataObject(GuildWelfareData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void GuildWelfareDataProcessor::ParseData(GuildWelfareData* obj, class CTabFile* dataRow, int rowIdx)
{
	GuildWelfareData data;
	
	data.WelfareId = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("ID"), dataRow, rowIdx);
	data.Name = TReadDataValue_FString<GuildWelfareDataManager>(TEXT("Name"), dataRow, rowIdx);
	data.Level = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("Level"), dataRow, rowIdx);
	data.BuffId = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("AuraID"), dataRow, rowIdx);
	data.Type = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("Kind"), dataRow, rowIdx);
	data.MaxLevel = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("MaxLevel"), dataRow, rowIdx);
	data.UpgradeTime = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("UpgradeTime"), dataRow, rowIdx);
	data.UpgradePreBuildingType = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("ConditionBuildingID"), dataRow, rowIdx);
	data.UpgradePreBuildingLevel = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("ConditionBuildingLvl"), dataRow, rowIdx);

	data.ExchangeLevelReq = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("PurchaseLevelRequirement"), dataRow, rowIdx);
	data.ExchangeMoneyReq = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("PurchaseMoneyConsumption"), dataRow, rowIdx);
	data.ExchangeContributeReq = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("PurchaseGuildCurrencyConsumption"), dataRow, rowIdx);
	data.ExchangeMax = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("VipPurchaseLimit0"), dataRow, rowIdx);
	data.VipExchangeMax1 = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("VipPurchaseLimit1"), dataRow, rowIdx);
	data.VipExchangeMax2 = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("VipPurchaseLimit2"), dataRow, rowIdx);
	data.VipExchangeMax3 = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("VipPurchaseLimit3"), dataRow, rowIdx);

	data.UpgradeGuildMoneyReq = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("UpgradeMoneyConsumption"), dataRow, rowIdx);
	data.UpgradeHerbReq = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("UpgradeHerbConsumption"), dataRow, rowIdx);
	data.UpgradeWoodReq = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("UpgradeWoodConsumption"), dataRow, rowIdx);
	data.UpgradeCrystalReq = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("UpgradeCrystalConsumption"), dataRow, rowIdx);
	data.UpgradeSliverReq = TReadDataValue_Int<GuildWelfareDataManager>(TEXT("UpgradeSliverConsumption"), dataRow, rowIdx);

	MMOGuildData::getSingleton().AddGuildWelfareData(data);
}

void GuildWelfareDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
GuildDungeonData* GuildDungeonDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void GuildDungeonDataProcessor::DestroyDataObject(GuildDungeonData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void GuildDungeonDataProcessor::ParseData(GuildDungeonData* obj, class CTabFile* dataRow, int rowIdx)
{
	GuildDungeonData data;

	data.DungeonId = TReadDataValue_Int<GuildDungeonDataManager>(TEXT("SceneTemplateID"), dataRow, rowIdx);
	FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo(data.DungeonId);
	data.DungeonName = mapInfo.MapDisplayName;
	data.DungeonDesc = mapInfo.MapDescript;

	data.BuildingReq = TReadDataValue_Int<GuildDungeonDataManager>(TEXT("ConditionBuildingID"), dataRow, rowIdx);
	data.BuildingLvReq = TReadDataValue_Int<GuildDungeonDataManager>(TEXT("ConditionBuildingLevel"), dataRow, rowIdx);
	data.MoneyReq = TReadDataValue_Int<GuildDungeonDataManager>(TEXT("ConditionMoney"), dataRow, rowIdx);
	data.MemberCountLimit = TReadDataValue_Int<GuildDungeonDataManager>(TEXT("BestTeammateNum"), dataRow, rowIdx);

	for (UINT8 i = 0; i < GUILD_DUNGEON_AWARD_NUM; ++ i)
	{
		FString rowName = FString::Printf(TEXT("Award%d"), i + 1);
		data.Award[i] = TReadDataValue_Int<GuildDungeonDataManager>(*rowName, dataRow, rowIdx);
	}

	MMOGuildData::getSingleton().AddGuildDungeonData(data);
}

void GuildDungeonDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
GuildActivityData* GuildActivityDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void GuildActivityDataProcessor::DestroyDataObject(GuildActivityData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void GuildActivityDataProcessor::ParseData(GuildActivityData* obj, class CTabFile* dataRow, int rowIdx)
{
	GuildActivityData data;

	data.Type = TReadDataValue_Int<GuildActivityDataManager>(TEXT("Type"), dataRow, rowIdx);
	data.Level = TReadDataValue_Int<GuildActivityDataManager>(TEXT("Level"), dataRow, rowIdx);
	data.Variable = TReadDataValue_Int<GuildActivityDataManager>(TEXT("GuildVariableIndex"), dataRow, rowIdx);
	data.Name = TReadDataValue_FString<GuildActivityDataManager> (TEXT("Name"), dataRow, rowIdx);
	data.Desc = TReadDataValue_FString<GuildActivityDataManager> (TEXT("Desc"), dataRow, rowIdx);
	data.NpcId = TReadDataValue_Int<GuildActivityDataManager>(TEXT("NpcId"), dataRow, rowIdx);
	data.MaxCount = TReadDataValue_Int<GuildActivityDataManager>(TEXT("RepeatNum"), dataRow, rowIdx);
	data.Duration = TReadDataValue_Int<GuildActivityDataManager>(TEXT("LastTime"), dataRow, rowIdx);
	data.BuildingReq = TReadDataValue_Int<GuildActivityDataManager>(TEXT("BaseBuildingType"), dataRow, rowIdx);
	data.BuildingLvReq = TReadDataValue_Int<GuildActivityDataManager>(TEXT("BaseBuildingLevel"), dataRow, rowIdx);
	data.MoneyReq = TReadDataValue_Int<GuildActivityDataManager>(TEXT("ConsumeMoney"), dataRow, rowIdx);
	data.HerbReq = TReadDataValue_Int<GuildActivityDataManager>(TEXT("ConsumeHerb"), dataRow, rowIdx);
	data.WoodReq = TReadDataValue_Int<GuildActivityDataManager>(TEXT("ConsumeWood"), dataRow, rowIdx);
	data.CrystalReq = TReadDataValue_Int<GuildActivityDataManager>(TEXT("ConsumeCrystal"), dataRow, rowIdx);
	data.SilverReq = TReadDataValue_Int<GuildActivityDataManager>(TEXT("ConsumeSilver"), dataRow, rowIdx);

	MMOGuildData::getSingleton().AddGuildActivityData(data);
}

void GuildActivityDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
MMOGuildData::MMOGuildData()
{
}

MMOGuildData::~MMOGuildData()
{
}

void MMOGuildData::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOGuildData::Init"), 0);

	Clear();

	_LoadGuildBuildingData(TEXT("guild_building.txt"));
	_LoadGuildWelfareData(TEXT("guild_welfare.txt"));
	_LoadGuildDungeonData(TEXT("guild_dungeon.txt"));
	_LoadGuildActivityData(TEXT("guild_activity.txt"));

	// 排行榜用的，不能跟着本地公会信息清除
	_QueryGuildInfoByIdMap.clear();
	_QueryGuildInfoByNameMap.clear();
}

void MMOGuildData::Shutdown()
{
	_UnloadGuildDungeonData();
	_UnloadGuildBuildingData();
	_UnloadGuildWelfareData();

	Clear();

	// 排行榜用的，不能跟着本地公会信息清除
	_QueryGuildInfoByIdMap.clear();
	_QueryGuildInfoByNameMap.clear();
}

void MMOGuildData::Clear()
{
	_GuildViewInfoList.clear();

	_GuildBaseInfo.Id = 0;
	_GuildBaseInfo.Name = TEXT("");
	_GuildBaseInfo.level = 0;
	_GuildBaseInfo.MemberCount = 0;
	_GuildBaseInfo.Money = 0;
	_GuildBaseInfo.Salary = 0;
	_GuildBaseInfo.LeaderName = TEXT("");
	_GuildBaseInfo.CreatorName = TEXT("");
	_GuildBaseInfo.CreateTime = 0;
	_GuildBaseInfo.Herb = 0;
	_GuildBaseInfo.Wood = 0;
	_GuildBaseInfo.Crystal = 0;
	_GuildBaseInfo.Silver = 0;
	_GuildBaseInfo.Notice = TEXT("");
	_GuildBaseInfo.Events = TEXT("");
	_GuildBaseInfo.MoneyEvents = TEXT("");

	_GuildEventsList.clear();
	_GuildMoneyEventsList.clear();
	_GuildFullMemberInfoList.clear();
	_GuildPendingMemberInfoList.clear();

	appMemset(_GuildStorageRights, 0, sizeof(UINT16) * GUILD_POSITION_COUNT);
	appMemset(_GuildSalaryPercent, 0, sizeof(UINT8) * GUILD_POSITION_COUNT);
	appMemset(_GuildPostCount, 0, sizeof(UINT8) * GUILD_POSITION_COUNT);

	_GuildAllyInfoList.clear();
	_GuildEnemyInfoList.clear();
	_GuildApplyAllyInfoList.clear();

	_GuildBuildingUpgradeList.clear();
	_GuildBuildingList.clear();
	_GuildWelfareUpgradeList.clear();
	_GuildWelfareInfoList.clear();

	_GuildDungeonAwardList.clear();
	_GuildWarResultList.clear();
	_MyGuildWarAttackId = 0;
	_MyGuildWarDefendId = 0;
	_GuildWarRecordList.clear();

	_SabacEnrollGuildIdList.clear();
	_CurOccupySabacGuildId = 0;

	_CurSabacStatue.RoleId = 0;
	_CurSabacStatue.RoleName = TEXT("");
	_CurSabacStatue.RoleClass = 0;
	_CurSabacStatue.RoleSex = 0;
	_CurSabacStatue.HairType = 0;
	_CurSabacStatue.HairColor = 0;
	_CurSabacStatue.FaceType = 0;
	_CurSabacStatue.SkinColor = 0;
	_CurSabacStatue.TitleId = 0;
	_CurSabacStatue.RefineFx = 0;
	appMemset(_CurSabacStatue.Avatar, 0, sizeof(INT32) * NES_VISIBLE_TOTAL);

	ClearWarehouseItemInfo();
	
	ClearVariable();
}

const GuildViewList& MMOGuildData::GetGuildViewInfoList()
{
	return _GuildViewInfoList;
}

void MMOGuildData::UpdateGuildViewList(UINT8 type, GUILD_VIEW_SUMMARY* list, UINT16 size)
{
	_GuildViewInfoList.clear();
	if (type == GUILD_VIEW_UP)
	{
		for (INT i = size - 1; i >= 0; -- i)
		{
			GuildViewInfo info;
			info.Id = list[i].dwGuildID;
			info.Name = UTF8_TO_TCHAR(list[i].szGuildName);
			info.level = list[i].byGuildLevel;
			info.MemberCount = list[i].byCurrentMemberCount;
			info.LeaderName = UTF8_TO_TCHAR(list[i].szLeaderName);
			info.CreatorName = UTF8_TO_TCHAR(list[i].szCreaterName);
			info.CreateTime = list[i].dwCreateTime;
			info.Announce = UTF8_TO_TCHAR(list[i].szAnnounce);
			info.ProtectionTime = list[i].dwProtectionTime;
			for (INT j = 0; j < GUILD_B_NUM; ++ j)
			{
				info.BuildingLevel[j] = list[i].arrBuildingLevel[j];
			}
			AddGuildViewInfo(info);
		}
	} 
	else
	{
		for (INT i = 0; i < size; ++ i)
		{
			GuildViewInfo info;
			info.Id = list[i].dwGuildID;
			info.Name = UTF8_TO_TCHAR(list[i].szGuildName);
			info.level = list[i].byGuildLevel;
			info.MemberCount = list[i].byCurrentMemberCount;
			info.LeaderName = UTF8_TO_TCHAR(list[i].szLeaderName);
			info.CreatorName = UTF8_TO_TCHAR(list[i].szCreaterName);
			info.CreateTime = list[i].dwCreateTime;
			info.Announce = UTF8_TO_TCHAR(list[i].szAnnounce);
			info.ProtectionTime = list[i].dwProtectionTime;
			for (INT j = 0; j < GUILD_B_NUM; ++ j)
			{
				info.BuildingLevel[j] = list[i].arrBuildingLevel[j];
			}
			AddGuildViewInfo(info);
		}
	}
}

void MMOGuildData::AddGuildViewInfo(const GuildViewInfo& info)
{
	_GuildViewInfoList.push_back(info);
}

UBOOL MMOGuildData::GetGuildViewInfo(UINT32 guildId, GuildViewInfo& info)
{
	GuildViewList::iterator iter = _GuildViewInfoList.begin();
	for (; iter != _GuildViewInfoList.end(); ++ iter)
	{
		if (iter->Id == guildId)
		{
			info = *iter;
			return TRUE;
		}
	}

	return FALSE;
}

void MMOGuildData::UpdateGuildEvents(GUILD_EVENT_INFO* events, UINT8 size)
{
	_GuildEventsList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		AddGuildEventInfo(events[i]);
	}
}

void MMOGuildData::UpdateGuildMoneyEvents(GUILD_EVENT_INFO* events, UINT8 size)
{
	_GuildMoneyEventsList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		AddGuildMoneyEventInfo(events[i]);
	}
}

void MMOGuildData::AddGuildEventInfo(const GUILD_EVENT_INFO& eventInfo)
{
	_GuildEventsList.push_back(eventInfo);
}

void MMOGuildData::AddGuildMoneyEventInfo(const GUILD_EVENT_INFO& eventInfo)
{
	_GuildMoneyEventsList.push_back(eventInfo);
}

void MMOGuildData::UpdateGuildPendingMemberList(GUILD_APPLICANT_INFO* list, UINT8 size)
{
	_GuildPendingMemberInfoList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		GuildPendingMemberInfo info;
		info.Id = list[i].dwRoleID;
		info.Name = UTF8_TO_TCHAR(list[i].szRoleName);
		info.Level = list[i].byLevel;
		info.Job = list[i].byClass;
		info.ApplyTime = list[i].dwEndTime;

		_GuildPendingMemberInfoList.push_back(info);
		MMOFacade::getSingleton().RequestPlayerBaseInfo(info.Id);
	}
}

const GuildFullMemberInfoList& MMOGuildData::GetGuildFullMemberInfoList()
{
	return _GuildFullMemberInfoList;
}

const GuildPendingMemberInfoList& MMOGuildData::GetGuildPendingMemberInfoList()
{
	return _GuildPendingMemberInfoList;
}

UINT32 MMOGuildData::GetCurListFirstId()
{
	GuildViewList::iterator iter = _GuildViewInfoList.begin();
	if (iter != _GuildViewInfoList.end())
	{
		return iter->Id;
	}
	return 0;
}

UINT32 MMOGuildData::GetCurListLastId()
{
	std::reverse_iterator<GuildViewList::iterator> iter = _GuildViewInfoList.rbegin();
	if (iter != _GuildViewInfoList.rend())
	{
		return iter->Id;
	}
	return 0;
}

void MMOGuildData::GetJobName(UINT32 type, FString& name)
{
	switch(type)
	{
	case EPCT_PlayerClassA:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName0"));
		break;
	case EPCT_PlayerClassB:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName1"));
		break;
	case EPCT_PlayerClassC:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName2"));
		break;
	case EPCT_PlayerClassD:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName3"));
		break;
	case EPCT_PlayerClassE:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.School.SchoolName4"));
		break;
	case EPCT_PlayerClassG:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EPlayerClass.E_PlayerClassG"));
		break;
	case EPCT_PlayerClassH:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EPlayerClass.E_PlayerClassH"));
		break;
	}
}

void MMOGuildData::GetGuildPostName(UINT8 type, FString& name)
{
	switch(type)
	{
	case GUILD_POSITION_LEADER:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Post0"));
		break;
	case GUILD_POSITION_SUBLEADER:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Post1"));
		break;
	case GUILD_POSITION_ELDER:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Post2"));
		break;
	case GUILD_POSITION_BISHOP:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Post3"));
		break;
	case GUILD_POSITION_DEACON:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Post4"));
		break;
	case GUILD_POSITION_SUBDEACON:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Post5"));
		break;
	case GUILD_POSITION_CIVILIAN:
		name = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Post6"));
		break;
	}
}

UINT MMOGuildData::GetGuildFullMemberOnlineCount()
{
	UINT count = 0;
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->LastLoginTime == 0)
		{
			count ++;
		}
	}

	return count;
}

void MMOGuildData::UpdateGuildFullMemberList(GUILD_MEMBER_INFO* list, UINT8 size)
{
	_GuildFullMemberInfoList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		AddGuildFullMember(list[i]);
	}
}

void MMOGuildData::AddGuildFullMember(const GUILD_MEMBER_INFO& info)
{
	GuildFullMemberInfo memberInfo;
	memberInfo.Id = info.dwRoleID;
	memberInfo.Name = UTF8_TO_TCHAR(info.szRoleName);
	memberInfo.Post = info.byPosition;
	// 统计职位人数
	if (memberInfo.Post > GUILD_POSITION_NONE && memberInfo.Post < GUILD_POSITION_TOTAL)
	{
		_GuildPostCount[memberInfo.Post - 1] ++;
	}
	memberInfo.Level = info.byLevel;
	memberInfo.Job = info.byClass;
	memberInfo.SceneID = info.nSceneID;
	memberInfo.LastLoginTime = info.nLastLoginTime;
	memberInfo.IsForbitChat = (info.byFlag & GUILD_FLAG_IS_CHAT_FORBIDDEN) > 0;
	memberInfo.Salary = info.nNextSalary;
	
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer && memberInfo.Id == mainPlayer->ServerID)
	{
		_GuildBaseInfo.Salary = memberInfo.Salary;
	}

	_GuildFullMemberInfoList.push_back(memberInfo);
	_GuildBaseInfo.MemberCount = _GuildFullMemberInfoList.size();

	// 查询成员名字
	MMOFacade::getSingleton().RequestPlayerBaseInfo(info.dwRoleID);
}

void MMOGuildData::DelGuildFullMember(UINT32 id)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == id)
		{
			// 统计职位人数
			if (iter->Post > GUILD_POSITION_NONE && iter->Post < GUILD_POSITION_TOTAL)
			{
				_GuildPostCount[iter->Post - 1] --;
			}

			iter = _GuildFullMemberInfoList.erase(iter);
			break;
		}
	}
	_GuildBaseInfo.MemberCount = _GuildFullMemberInfoList.size();

	// 查询成员名字
	MMOFacade::getSingleton().RequestPlayerBaseInfo(id);
}

void MMOGuildData::UpdateGuildFullMemberPost(UINT32 id, UINT8 post)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter)
	{
		if (iter->Id == id)
		{
			// 统计职位人数
			if (iter->Post > GUILD_POSITION_NONE && iter->Post < GUILD_POSITION_TOTAL)
			{
				_GuildPostCount[iter->Post - 1] --;
			}

			iter->Post = post;

			if (iter->Post > GUILD_POSITION_NONE && iter->Post < GUILD_POSITION_TOTAL)
			{
				_GuildPostCount[iter->Post - 1] ++;
			}

			break;
		}
	}
}

void MMOGuildData::UpdateGuildFullMemberData(UINT32 roleId, UINT8 level, INT32 sceneId, UINT32 lastLoginTime)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == roleId)
		{
			// 任务 #25207, 如果成员下线，仍旧要显示等级
			if (level > 0)
			{
				iter->Level = level;
			}
			iter->SceneID = sceneId;
			iter->LastLoginTime = lastLoginTime;
			break;
		}
	}
}

void MMOGuildData::GetGuildFullMemberName(UINT32 roleId, FString& roleName)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == roleId)
		{
			roleName = iter->Name;
			break;
		}
	}
}

UINT32 MMOGuildData::GetGuildFullMemberId(const FString& roleName)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Name == roleName)
		{
			return iter->Id;
		}
	}

	return 0;
}


UINT MMOGuildData::GetGuildFullMemberPost(UINT32 roleId)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == roleId)
		{
			return iter->Post;
		}
	}

	return GUILD_POSITION_NONE;
}

UINT32 MMOGuildData::GetGuildFullMemberSalary(UINT32 roleId)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == roleId)
		{
			return iter->Salary;
		}
	}

	return 0;
}

UBOOL MMOGuildData::GetGuildFullMemberInfo(UINT32 roleId, GuildFullMemberInfo& info)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == roleId)
		{
			info = *iter;
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMOGuildData::IsGuildFullMember(UINT32 roleId)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter)
	{
		if (iter->Id == roleId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMOGuildData::IsGuildFullMember(const FString& roleName)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter)
	{
		if (iter->Name == roleName)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void MMOGuildData::SetGuildFullMemberForbitChat(UINT32 roleId, UBOOL isForbid)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == roleId)
		{
			iter->IsForbitChat = isForbid;
			break;
		}
	}
}

UBOOL MMOGuildData::IsGuildFullMemberForbitChat(UINT32 roleId)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == roleId)
		{
			return iter->IsForbitChat;
		}
	}

	return FALSE;
}

void MMOGuildData::SetWarehouseItem( INT pageId , INT idx , FClientItemInfo* item )
{
	if (pageId < 0 || pageId >= E_GUILD_STORAGE_TAG_MAX)
	{
		return;
	}

	if (idx < 0 || idx >= GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG)
	{
		return;
	}

	if(item)
	{
		_GuildWarehouseItemTable[pageId][idx] = *item;
	}
	else
	{
		//标0即为空格子
		_GuildWarehouseItemTable[pageId][idx].m_nTemplateID = 0;
	}
}

void MMOGuildData::ClearWarehouseItemInfo()
{
	appMemIsZero(_GuildWarehouseItemTable , sizeof(_GuildWarehouseItemTable)); 
	for (INT i = 0; i < E_GUILD_STORAGE_TAG_MAX; ++ i)
	{
		for (INT j = 0; j < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG; ++ j)
		{
			_GuildWarehouseItemTable[i][j].m_nTemplateID = 0;
		}
	}
}

void MMOGuildData::SetWarehouseItemInfo(INT pageId, INT idx, ItemInfo* item)
{
	if (pageId < 0 || pageId >= E_GUILD_STORAGE_TAG_MAX)
	{
		return;
	}

	if (idx < 0 || idx >= GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG)
	{
		return;
	}

	if(item)
	{
		MMOFacade::getSingleton().UpdateItemInfo( item , &_GuildWarehouseItemTable[pageId][idx] );
	}
	else
	{
		
		_GuildWarehouseItemTable[pageId][idx].m_nTemplateID = 0;
	}
}

void MMOGuildData::SwapItem(INT srcPageId, INT srcSlotId , INT desPageId , INT desSlotId )
{
	if (srcPageId < 0 || srcPageId >= E_GUILD_STORAGE_TAG_MAX)
	{
		return;
	}

	if (srcSlotId < 0 || srcSlotId >= GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG)
	{
		return;
	}

	if (desPageId < 0 || desPageId >= E_GUILD_STORAGE_TAG_MAX)
	{
		return;
	}

	if (desSlotId < 0 || desSlotId >= GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG)
	{
		return;
	}

	FClientItemInfo temp = _GuildWarehouseItemTable[srcPageId][srcSlotId];
	_GuildWarehouseItemTable[srcPageId][srcSlotId] = _GuildWarehouseItemTable[desPageId][desSlotId];
	_GuildWarehouseItemTable[desPageId][desSlotId] = temp;
}

INT MMOGuildData::GetEmptyGuildWarehouseSlot( INT pageId )
{
	if (pageId < 0 || pageId >= E_GUILD_STORAGE_TAG_MAX)
	{
		return -1;
	}

	for ( INT i = 0 ; i < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG ; i++ )
	{
		if( _GuildWarehouseItemTable[pageId][i].m_nTemplateID <= 0 )
		{
			return i;
		}
	}
	return -1;
}

void MMOGuildData::ClearAccessInfo( )
{
	_AccessInfoList.clear();
}

void MMOGuildData::AddAccessInfo( INT playerId , const FString& roleName , INT itemTempId , INT count , INT time , INT type )
{
	GuildWareAccessInfo item;
	item.RoleId = playerId;
	item.Count = count;
	item.Time = time;
	item.Type = type;
	item.RoleName = roleName;
	item.ItemTempId = itemTempId;
	_AccessInfoList.push_back(item);
}

FClientItemInfo* MMOGuildData::GetWarehouseItem( INT pageId , INT idx )
{
	if (pageId < 0 || pageId >= E_GUILD_STORAGE_TAG_MAX)
	{
		return NULL;
	}

	if (idx < 0 || idx >= GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG)
	{
		return NULL;
	}

	if( _GuildWarehouseItemTable[pageId][idx].m_nTemplateID == 0 )
		return NULL;

	return &(_GuildWarehouseItemTable[pageId][idx]);
}

void MMOGuildData::UpdateGuildStorageRights(UINT8 post, UINT16 rights)
{
	if (post > GUILD_POSITION_NONE && post < GUILD_POSITION_TOTAL)
	{
		_GuildStorageRights[post - 1] = rights;
	}
}

void MMOGuildData::UpdateMyGuildSalary(UINT32 salary)
{
	_GuildBaseInfo.Salary = salary;
	
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
		for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
		{
		
			if (iter->Id == mainPlayer->ServerID)
			{
				iter->Salary = salary; 
				break;
			}
		}
	}
}

void MMOGuildData::UpdateGuildSalaryPercent(const UINT8 (&saleryPercent)[GUILD_POSITION_COUNT])
{
	appMemcpy(_GuildSalaryPercent, saleryPercent, sizeof(UINT8) * GUILD_POSITION_COUNT);
}

void MMOGuildData::UpdateGuildBaseInfo(const GuildBaseInfo& baseInfo)
{
	_GuildBaseInfo = baseInfo;
}

UINT16 MMOGuildData::GetGuildRights(UINT8 post)
{
	// 这里修改必须配合服务器NewServer\bin\globalconfig\guildconfig.lua同步修改
	UINT16 rights = 0;
	switch (post)
	{
	case GUILD_POSITION_LEADER:
		rights = 4095;
		break;
	case GUILD_POSITION_SUBLEADER:
		rights = 4095;
		break;
	case GUILD_POSITION_ELDER:
		rights = 495;
		break;
	case GUILD_POSITION_BISHOP:
		rights = 15;
		break;
	case GUILD_POSITION_DEACON:
		rights = 3;
		break;
	case GUILD_POSITION_SUBDEACON:
		rights = 1;
		break;
	case GUILD_POSITION_CIVILIAN:
		rights = 0;
		break;
	}

	return rights;
}

UINT16 MMOGuildData::GetGuildStorageRights(UINT8 post)
{
	UINT16 rights = 0;

	if (post > GUILD_POSITION_NONE && post < GUILD_POSITION_TOTAL)
	{
		rights = _GuildStorageRights[post - 1];
	}

	return rights;
}

UINT8 MMOGuildData::GetGuildSalaryPercent(UINT8 post)
{
	if (post > GUILD_POSITION_NONE && post < GUILD_POSITION_TOTAL)
	{
		return _GuildSalaryPercent[post - 1];
	}

	return 0;
}

UINT8 MMOGuildData::GetGuildPostCount(UINT8 post)
{
	UINT8 count = 0;

	if (post > GUILD_POSITION_NONE && post < GUILD_POSITION_TOTAL)
	{
		count = _GuildPostCount[post - 1];
	}

	return count;
}

bool less_level(const GuildFullMemberInfo& info1, const GuildFullMemberInfo& info2)
{
	return info1.Level > info2.Level;
}

bool less_job(const GuildFullMemberInfo& info1, const GuildFullMemberInfo& info2)
{
	return info1.Job < info2.Job;
}

bool less_post(const GuildFullMemberInfo& info1, const GuildFullMemberInfo& info2)
{
	return info1.Post < info2.Post;
}

bool less_time(const GuildFullMemberInfo& info1, const GuildFullMemberInfo& info2)
{
	INT time1 = info1.LastLoginTime;
	INT time2 = info2.LastLoginTime;

	__time32_t localTime;
	_time32(&localTime);
	_localtime32(&localTime);

	if (time1 == 0)
	{
		time1 = localTime;
	}

	if (time2 == 0)
	{
		time2 = localTime;
	}

	return time1 < time2;
}


bool great_level(const GuildFullMemberInfo& info1, const GuildFullMemberInfo& info2)
{
	return info1.Level <= info2.Level;
}

bool great_job(const GuildFullMemberInfo& info1, const GuildFullMemberInfo& info2)
{
	return info1.Job >= info2.Job;
}

bool great_post(const GuildFullMemberInfo& info1, const GuildFullMemberInfo& info2)
{
	return info1.Post >= info2.Post;
}

bool great_time(const GuildFullMemberInfo& info1, const GuildFullMemberInfo& info2)
{
	INT time1 = info1.LastLoginTime;
	INT time2 = info2.LastLoginTime;

	__time32_t localTime;
	_time32(&localTime);
	_localtime32(&localTime);

	if (time1 == 0)
	{
		time1 = localTime;
	}

	if (time2 == 0)
	{
		time2 = localTime;
	}

	return time1 >= time2;
}

void MMOGuildData::SortGuildFullMember(INT type, UBOOL isUp)
{
	switch (type)
	{
	case FMST_LEVEL:
		_GuildFullMemberInfoList.sort(isUp ? great_level : less_level);
		break;
	case FMST_JOB:
		_GuildFullMemberInfoList.sort(isUp ? great_job : less_job);
		break;
	case FMST_POST:
		_GuildFullMemberInfoList.sort(isUp ? great_post : less_post);
		break;
	case FMST_TIME:
		_GuildFullMemberInfoList.sort(isUp ? great_time : less_time);
		break;
	}
}

const GuildAllyInfoList& MMOGuildData::GetGuildAllyInfoList()
{
	return _GuildAllyInfoList;
}

const GuildEnemyInfoList& MMOGuildData::GetGuildEnemyInfoList()
{
	return _GuildEnemyInfoList;
}

const GuildApplyAllyInfoList& MMOGuildData::GetGuildApplyAllyInfoList()
{
	return _GuildApplyAllyInfoList;
}

bool MMOGuildData::IsGuildAlly(UINT32 id)
{
	GuildAllyInfoList::iterator iter = _GuildAllyInfoList.begin();
	for (; iter != _GuildAllyInfoList.end(); ++iter)
	{
		if (iter->Id == id)
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool MMOGuildData::IsGuildEnemy(UINT32 id)
{
	GuildEnemyInfoList::iterator iter = _GuildEnemyInfoList.begin();
	for (; iter != _GuildEnemyInfoList.end(); ++iter)
	{
		if (iter->Id == id)
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool MMOGuildData::IsGuildEnemy(const FString& guildName)
{
	GuildEnemyInfoList::iterator iter = _GuildEnemyInfoList.begin();
	for (; iter != _GuildEnemyInfoList.end(); ++iter)
	{
		if (iter->Name == guildName)
		{
			return TRUE;
		}
	}

	return FALSE;
}

GuildRelation MMOGuildData::GetGuildRelation(const FString& guildName)
{
	GuildAllyInfoList::iterator iter1 = _GuildAllyInfoList.begin();
	for (; iter1 != _GuildAllyInfoList.end(); ++ iter1 )
	{
		if (iter1->Name == guildName)
		{
			return GR_FRIEND;
		}
	}

	GuildEnemyInfoList::iterator iter2 = _GuildEnemyInfoList.begin();
	for (; iter2 != _GuildEnemyInfoList.end(); ++ iter2 )
	{
		if (iter2->Name == guildName)
		{
			return GR_ENEMY;
		}
	}

	if (_MyGuildWarAttackId != 0)
	{
		GuildQueryInfo info;
		if (GetGuildInfoById(_MyGuildWarAttackId, info))
		{
			if (info.Name == guildName)
			{
				return GR_ENEMY;
			}
		}
	}

	if (_MyGuildWarDefendId != 0)
	{
		GuildQueryInfo info;
		if (GetGuildInfoById(_MyGuildWarDefendId, info))
		{
			if (info.Name == guildName)
			{
				return GR_ENEMY;
			}
		}
	}

	// 2016.6.23公会临时做法
	if (MMOGuildManager::getSingleton().IsGuildWarStart())
	{
		INT mapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
		if (mapId == GUILD_TERRITORY_MAP_ID)
		{
			GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			if (baseInfo.Id > 0)
			{
				return (baseInfo.Name == guildName) ? GR_FRIEND : GR_ENEMY;
			}
		}
	}

	return GR_NORMAL;
}

void MMOGuildData::UpdateGuildAllyList(GUILD_DIPLOMAT_INFO* list, UINT8 size)
{
	_GuildAllyInfoList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		// 这里服务器发的绝对时间，换算一下
		__time32_t lastTime = list[i].dwLastTime;
		_localtime32(&lastTime);

		__time32_t localTime;
		_time32(&localTime);
		_localtime32(&localTime);

		INT time = lastTime - localTime;
		if (time < 0)
		{
			time = 0;
		}
		AddGuildAlly(list[i].dwGuildID, UTF8_TO_TCHAR(list[i].szGuildName), list[i].byLevel, list[i].byMemberCount, time);
		MMOGuildManager::getSingleton().QueryGuildInfo(list[i].dwGuildID);
	}
}

void MMOGuildData::UpdateGuildEnemyList(GUILD_DIPLOMAT_INFO* list, UINT8 size)
{
	_GuildEnemyInfoList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		// 这里服务器发的绝对时间，换算一下
		__time32_t lastTime = list[i].dwLastTime;
		_localtime32(&lastTime);

		__time32_t localTime;
		_time32(&localTime);
		_localtime32(&localTime);

		INT time = lastTime - localTime;
		if (time < 0)
		{
			time = 0;
		}
		AddGuildEnemy(list[i].dwGuildID, UTF8_TO_TCHAR(list[i].szGuildName), list[i].byLevel, list[i].byMemberCount, time);
		MMOGuildManager::getSingleton().QueryGuildInfo(list[i].dwGuildID);
	}
}

void MMOGuildData::AddGuildAlly(UINT32 id, const FString& name, UINT8 level, UINT8 memberCount, UINT32 leftTime)
{
	GuildForeignInfo foreignInfo;
	foreignInfo.Id = id;
	foreignInfo.Name = name;
	foreignInfo.Level = level;
	foreignInfo.MemberCount = memberCount;
	foreignInfo.LeftTime = leftTime;
	_GuildAllyInfoList.push_back(foreignInfo);
}

void MMOGuildData::DelGuildAlly(UINT32 id)
{
	GuildAllyInfoList::iterator iter = _GuildAllyInfoList.begin();
	for (; iter != _GuildAllyInfoList.end(); ++ iter )
	{
		if (iter->Id == id)
		{
			iter = _GuildAllyInfoList.erase(iter);
			break;
		}
	}
}

void MMOGuildData::AddGuildEnemy(UINT32 id, const FString& name, UINT8 level, UINT8 memberCount, UINT32 leftTime)
{
	GuildForeignInfo foreignInfo;
	foreignInfo.Id = id;
	foreignInfo.Name = name;
	foreignInfo.Level = level;
	foreignInfo.MemberCount = memberCount;
	foreignInfo.LeftTime = leftTime;
	_GuildEnemyInfoList.push_back(foreignInfo);
}

void MMOGuildData::DelGuildEnemy(UINT32 id)
{
	GuildEnemyInfoList::iterator iter = _GuildEnemyInfoList.begin();
	for (; iter != _GuildEnemyInfoList.end(); ++ iter )
	{
		if (iter->Id == id)
		{
			iter = _GuildEnemyInfoList.erase(iter);
			break;
		}
	}
}

void MMOGuildData::UpdateGuildApplyAllyList(APPLY_ALLIANCE_INFO* list, UINT8 size)
{
	_GuildApplyAllyInfoList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		GuildApplyAllyInfo info;
		info.Id = list[i].dwGuildID;
		info.Name = UTF8_TO_TCHAR(list[i].szGuildName);
		info.Level = list[i].byLevel;
		info.MemberCount = list[i].byMemberCount;
		info.LeaderName = UTF8_TO_TCHAR(list[i].szLeaderName);
		/*
		switch (list[i].byTimeType)
		{
		case G_DIPLOMATIC_TIME_1:
			info.LeftTime = 86400;
			break;
		case G_DIPLOMATIC_TIME_2:
			info.LeftTime = 3 * 86400;
			break;
		case G_DIPLOMATIC_TIME_3:
			info.LeftTime = 7 * 86400;
			break;
		case G_DIPLOMATIC_TIME_4:
			info.LeftTime = 30 * 86400;
			break;
		}
		*/

		// 这里服务器发的绝对时间，换算一下
		__time32_t lastTime = list[i].dwWashoutTime;
		_localtime32(&lastTime);

		__time32_t localTime;
		_time32(&localTime);
		_localtime32(&localTime);

		INT time = lastTime - localTime;
		if (time < 0)
		{
			time = 0;
		}

		info.LeftTime = time;
		
		AddGuildApplyAllyInfo(info);
		MMOGuildManager::getSingleton().QueryGuildInfo(list[i].dwGuildID);
	}
}

void MMOGuildData::AddGuildApplyAllyInfo(const GuildApplyAllyInfo& info)
{
	_GuildApplyAllyInfoList.push_back(info);
}

void MMOGuildData::UpdateGuildAllyTime(UINT32 count)
{
	if (_GuildAllyInfoList.size() > 0)
	{
		GuildAllyInfoList::iterator iter = _GuildAllyInfoList.begin();
		for (; iter != _GuildAllyInfoList.end(); ++ iter )
		{
			if (iter->LeftTime > 0)
			{
				iter->LeftTime -= count;
			}
		}
	}
}

void MMOGuildData::UpdateGuildEnemyTime(UINT32 count)
{
	if (_GuildEnemyInfoList.size() > 0)
	{
		GuildEnemyInfoList::iterator iter = _GuildEnemyInfoList.begin();
		for (; iter != _GuildEnemyInfoList.end(); ++iter)
		{
			if (iter->LeftTime > 0)
			{
				iter->LeftTime -= count;
			}
		}
	}
}

void MMOGuildData::UpdateGuildApplyAllyTime(UINT32 count)
{
	if (_GuildApplyAllyInfoList.size() > 0)
	{
		GuildApplyAllyInfoList::iterator iter = _GuildApplyAllyInfoList.begin();
		for (; iter != _GuildApplyAllyInfoList.end(); ++iter)
		{
			if (iter->LeftTime > 0)
			{
				iter->LeftTime -= count;
			}
		}
	}
}

void MMOGuildData::_GetGuildEventsRoleName(UINT32 roleId, FString& roleName)
{
	roleName = TEXT("");
	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo(roleId);
	if (playerInfo)
	{
		roleName = playerInfo->_RoleName;
	}
	else
	{
		// Bug:12998
		roleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
		MMOFacade::getSingleton().RequestPlayerBaseInfo(roleId);
	}
}

void MMOGuildData::_GetGuildEventsGuildName(UINT32 guildId, FString& guildName)
{
	guildName = TEXT("");
	GuildQueryInfo info;
	if (GetGuildInfoById(guildId, info))
	{
		guildName = info.Name;
	}
	else
	{
		MMOGuildManager::getSingleton().QueryGuildInfo(guildId);
	}
}

bool GreatTime(const GUILD_EVENT_INFO& info1, const GUILD_EVENT_INFO& info2)
{
	return info1.dwTime > info2.dwTime;
}

void MMOGuildData::UpdateGuildEvents()
{
	FString events = TEXT("");
	std::sort(_GuildEventsList.begin(), _GuildEventsList.end(), GreatTime);
	for (int i = 0; i < (int)(_GuildEventsList.size()); ++ i)
	{
		GUILD_EVENT_INFO eventInfo = _GuildEventsList[i];

		FString text1 = TEXT("");
		FString text2 = TEXT("");
		FString text3 = TEXT("");
		FString text4 = TEXT("");

		if (eventInfo.byType != 0)
		{
			__time32_t localTime = eventInfo.dwTime;
			struct tm *time = _localtime32(&localTime); 
			FString date = TEXT("");
			if (time)
			{
				date = eventInfo.dwTime != 0 ? FString::Printf(TEXT("%d-%d-%d %02d:%02d:%02d"), 
					time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec) : MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowTime"));
			}
			events += date;
			events += TEXT("\n");
		}

		switch (eventInfo.byType)
		{
		case GUILD_HE_TYPE_CREATE:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice1"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_JOIN:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice3"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_LEAVE:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice5"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_BANISH:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				_GetGuildEventsRoleName(eventInfo.param2, text2);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice7"))), *text1, *text2);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_POSITION_CHANGE:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				_GetGuildEventsRoleName(eventInfo.param2, text2);
				GetGuildPostName(eventInfo.param3, text3);
				GetGuildPostName(eventInfo.param4, text4);

				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice9"))), *text1, *text2, *text3, *text4);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_BUILDING_LVLUP:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				
				GuildBuildingData data;
				GetGuildBuildingData(eventInfo.param2, eventInfo.param3, data);
				events += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeBuilding")), *text1, *(data.Name), eventInfo.param3);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_SKILL_LVLUP:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);

				GuildWelfareData data;
				GetGuildWelfareData(eventInfo.param2, 0, data);
				events += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeWelfare")), *text1, *(data.Name), eventInfo.param3);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_DEMISE:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				_GetGuildEventsRoleName(eventInfo.param2, text2);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice11"))), *text1, *text2);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_UNION:
			{
				_GetGuildEventsGuildName(eventInfo.param2, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice13"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_CANCEL_ALLIANCE:
			{
				_GetGuildEventsGuildName(eventInfo.param2, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice25"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_ENEMY:
			{
				_GetGuildEventsGuildName(eventInfo.param2, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice15"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
			/*
		case GUILD_HE_TYPE_DECLARE_WAR:
			{
				_GetGuildEventsGuildName(eventInfo.param1, text1);
				_GetGuildEventsGuildName(eventInfo.param2, text2);
				FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice17"))), *text1, *text2);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
			*/
		case GUILD_HE_TYPE_BUILDING_CREATE:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);

				GuildBuildingData data;
				GetGuildBuildingData(eventInfo.param2, 0, data);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice17"))), *text1, *(data.Name));
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_BUILDING_DESTORY:
			{
				GuildBuildingData data;
				GetGuildBuildingData(eventInfo.param1, 0, data);
				_GetGuildEventsGuildName(eventInfo.param2, text1);

				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice18"))), *(data.Name), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_BUILDING_DEL:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);

				GuildBuildingData data;
				GetGuildBuildingData(eventInfo.param2, 0, data);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice19"))), *text1, *(data.Name));
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_BUILDING_REVIVE:
			{
				GuildBuildingData data;
				GetGuildBuildingData(eventInfo.param1, 0, data);

				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice20"))), *(data.Name));
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_ATTACK_SUCC:
			{
				_GetGuildEventsGuildName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice21"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_ATTACK_FAILED:
			{
				_GetGuildEventsGuildName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice22"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_DEFEND_SUCC:
			{
				_GetGuildEventsGuildName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice23"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		case GUILD_HE_TYPE_DEFEND_FAILED:
			{
				_GetGuildEventsGuildName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Notice24"))), *text1);
				events += TEXT("\n");
				events += TEXT("\n");
			}
			break;
		}
	}

	_GuildBaseInfo.Events = events;
}

void MMOGuildData::UpdateGuildMoneyEvents()
{
	FString events = TEXT("");
	std::sort(_GuildMoneyEventsList.begin(), _GuildMoneyEventsList.end(), GreatTime);
	for (int i = 0; i < (int)(_GuildMoneyEventsList.size()); ++ i)
	{
		GUILD_EVENT_INFO eventInfo = _GuildMoneyEventsList[i];

		FString text1 = TEXT("");
		FString text2 = TEXT("");
		FString text3 = TEXT("");
		FString text4 = TEXT("");

		if (eventInfo.byType != 0)
		{
			__time32_t localTime = eventInfo.dwTime;
			struct tm *time = _localtime32(&localTime); 
			FString date = TEXT("");
			if (time)
			{
				date = eventInfo.dwTime != 0 ? FString::Printf(TEXT("%d-%d-%d %02d:%02d:%02d"), 
					time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec) : MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowTime"));
			}
			events += date;
			events += TEXT(" ");
		}

		switch (eventInfo.byType)
		{
			/*
		case GUILD_CHEST_TASK:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice0"))), *text1, eventInfo.param2);
				events += TEXT("\n");
			}
			break;
			*/
		case GUILD_CHEST_ACTIVITY:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);

				GuildActivityData data;
				MMOGuildData::getSingleton().GetGuildActivityData(eventInfo.param2, data);

				FString resName = TEXT("");
				GetGuildTerritoryResName(eventInfo.param3, resName);

				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice1"))), *text1, *(data.Name), eventInfo.param4, *resName);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_CONTRIBUTE:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice2"))), *text1, eventInfo.param2);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_CREATE_BUILDING:
		{
			_GetGuildEventsRoleName(eventInfo.param1, text1);

			GuildBuildingData data;
			GetGuildBuildingData(eventInfo.param2, 1, data);
			
			FString resName = TEXT("");
			GetGuildTerritoryResName(eventInfo.param3, resName);

			events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice13"))), *text1, *(data.Name), eventInfo.param4, *resName);
			events += TEXT("\n");
		}
			break;
		case GUILD_CHEST_B_UPGRADE:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);

				GuildBuildingData data;
				GetGuildBuildingData(eventInfo.param2, 1, data);

				FString resName = TEXT("");
				GetGuildTerritoryResName(eventInfo.param3, resName);

				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice3"))), *text1, *(data.Name), eventInfo.param4, *resName);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_W_UPGRADE:
			{
				_GetGuildEventsRoleName(eventInfo.param1, text1);

				GuildWelfareData data;
				GetGuildWelfareData(eventInfo.param2, 0, data);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice4"))), *text1, *(data.Name), eventInfo.param3);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_ALLIANCE:
			{
				_GetGuildEventsGuildName(eventInfo.param1, text1);
				//_GetGuildEventsRoleName(eventInfo.param2, text2);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice5"))), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MyGuild")), *text1, eventInfo.param2);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_ARMY:
			{
				_GetGuildEventsGuildName(eventInfo.param1, text1);
				//_GetGuildEventsRoleName(eventInfo.param2, text2);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice6"))), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MyGuild")), *text1, eventInfo.param2);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_WAR_SACK_A_1:
			{
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice7"))), eventInfo.param1);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_WAR_SACK_A_2:
			{
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice8"))), eventInfo.param1, eventInfo.param2, eventInfo.param3, eventInfo.param4);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_WAR_SACK_D_1:
			{
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice9"))), eventInfo.param1);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_WAR_SACK_D_2:
			{
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice10"))), eventInfo.param1, eventInfo.param2, eventInfo.param3, eventInfo.param4);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_WAR_WIN:
			{
				_GetGuildEventsGuildName(eventInfo.param2, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice11"))), *text1, eventInfo.param2);
				events += TEXT("\n");
			}
			break;
		case GUILD_CHEST_WAR_LOSE:
			{
				_GetGuildEventsGuildName(eventInfo.param2, text1);
				events += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyNotice12"))), *text1, eventInfo.param2);
				events += TEXT("\n");
			}
			break;
		}
	}

	_GuildBaseInfo.MoneyEvents = events;
}

void MMOGuildData::GuildLeaderDemise(UINT32 newLeaderId)
{
	GuildFullMemberInfoList::iterator iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Post == GUILD_POSITION_LEADER)
		{
			iter->Post = GUILD_POSITION_CIVILIAN;
			break;
		}
	}

	iter = _GuildFullMemberInfoList.begin();
	for (; iter != _GuildFullMemberInfoList.end(); ++ iter )
	{
		if (iter->Id == newLeaderId)
		{
			iter->Post = GUILD_POSITION_LEADER;
			_GuildBaseInfo.LeaderName = iter->Name;
			break;
		}
	}
}

UBOOL MMOGuildData::_LoadGuildBuildingData(const TCHAR* filePath)
{
	_GuildBuildingDataMap.clear();
	GuildBuildingDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOGuildData::_UnloadGuildBuildingData()
{
	_GuildBuildingDataMap.clear();
}

UINT32 MMOGuildData::_GetGuildBuildingRealId(UINT32 buildingId, UINT32 buildingLv)
{
	return  buildingId << 16 | buildingLv;
}

UBOOL MMOGuildData::_LoadGuildWelfareData(const TCHAR* filePath)
{
	_GuildWelfareDataMap.clear();
	GuildWelfareDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOGuildData::_UnloadGuildWelfareData()
{
	_GuildWelfareDataMap.clear();
}

UBOOL MMOGuildData::_LoadGuildDungeonData(const TCHAR* filePath)
{
	_GuildDungeonDataMap.clear();
	GuildDungeonDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOGuildData::_UnloadGuildDungeonData()
{
	_GuildDungeonDataMap.clear();
}

UBOOL MMOGuildData::_LoadGuildActivityData(const TCHAR* filePath)
{
	_GuildActivityDataMap.clear();
	_GuildActivityVariableMap.clear();
	GuildActivityDataManager::getSingleton().InitFromFile(filePath, true);
	
	GenerateGuildActivityInfoList();
	return TRUE;
}

void MMOGuildData::_UnloadGuildActivityData()
{
	_GuildActivityDataMap.clear();
	_GuildActivityVariableMap.clear();
}

void MMOGuildData::AddGuildActivityData(const GuildActivityData& data)
{
	UINT32 id = MAKELONG(data.Level, data.Type);
	_GuildActivityDataMap.insert(std::make_pair(id, data));
	_GuildActivityVariableMap.insert(std::make_pair(data.Variable, data.Type));
}

UBOOL MMOGuildData::GetGuildActivityData(UINT32 activityId, GuildActivityData& data)
{
	GuildActivityDataMap::iterator iter = _GuildActivityDataMap.find(activityId);
	if (iter != _GuildActivityDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

void MMOGuildData::GenerateGuildActivityInfoList()
{
	_GuildActivityInfoMap.clear();
	GuildActivityDataMap::iterator iter = _GuildActivityDataMap.begin();
	for (; iter != _GuildActivityDataMap.end(); ++ iter)
	{
		GuildActivityInfo info;
		info.Type = iter->second.Type;
		info.Complete = false;
		info.StartTime = 0;
		info.State = 0;
		_GuildActivityInfoMap.insert(std::make_pair(info.Type, info));
	}
}

UINT8 MMOGuildData::GetGuildActivityComplete(UINT16 activityType)
{
	GuildBaseInfo baseInfo = GetGuildBaseInfo();
	UINT16 activityLevel = baseInfo.level;
	UINT32 activityId = MAKELONG(activityLevel, activityType);

	GuildActivityData data;
	if (!GetGuildActivityData(activityId, data))
	{
		return 0;
	}

	GuildActivityInfoMap::iterator iter = _GuildActivityInfoMap.find(activityType);
	if (iter != _GuildActivityInfoMap.end())
	{
		UINT16 varIndex = 0;
		switch (data.Variable)
		{
		case GUILD_ACTIVITY_VARIABLE_107:
			varIndex = EGARV_ID_3;
			break;

		case GUILD_ACTIVITY_VARIABLE_108:
			varIndex = EGARV_ID_5;
			break;

		case GUILD_ACTIVITY_VARIABLE_109:
			varIndex = EGARV_ID_2;
			break;

		case GUILD_ACTIVITY_VARIABLE_110:
			varIndex = EGARV_ID_4;
			break;
		}

		int lastDay = 0;
		int lastHour = 0;
		__time32_t lastTime = GetVariableValue(vtRole, varIndex);
		struct tm *lastDate = _localtime32(&lastTime);
		if (lastTime > 0 && lastDate)
		{
			lastDay = lastDate->tm_mday;
			lastHour = lastDate->tm_hour;
		}

		int localDay = 0;
		int localHour = 0;
		__time32_t localTime;
		_time32(&localTime);
		struct tm *localDate = _localtime32(&localTime);
		if (localTime > 0 && localDate)
		{
			localDay = localDate->tm_mday;
			localHour = localDate->tm_hour;
		}

		// 凌晨0:00重置版本
		if (lastDay < localDay)
		{
			iter->second.Complete = 0;
		}
		else if (lastDay == localDay)
		{
			if (varIndex == EGARV_ID_2)
			{
				iter->second.Complete = GetVariableValue(vtRole, EGARV_ID_1) > 0 ? 1 : 0;
			}
			else
			{
				iter->second.Complete = 1;
			}
		}
		else
		{
			iter->second.Complete = 0;
		}

		return iter->second.Complete;
	}

	return 0;
}

void MMOGuildData::SetGuildActivityTime(UINT16 activityType, UINT32 time)
{
	GuildBaseInfo baseInfo = GetGuildBaseInfo();
	UINT16 activityLevel = baseInfo.level;
	UINT32 activityId = MAKELONG(activityLevel, activityType);

	GuildActivityData data;
	if (!GetGuildActivityData(activityId, data))
	{
		return;
	}

	GuildActivityInfoMap::iterator iter = _GuildActivityInfoMap.find(activityType);
	if (iter != _GuildActivityInfoMap.end())
	{
		// 服务器发过来的是结束时间，这里计算下开启时间
		// 用开启时间判断状态，可以规避跨天活动状态不对的问题
		if (time == 0)
		{
			iter->second.StartTime = 0;
		}
		else
		{
			iter->second.StartTime = time >= data.Duration ? (time - data.Duration) : 0;
		}

		int startDay = 0;
		int startHour = 0;
		__time32_t startTime = iter->second.StartTime;
		struct tm *startDate = _localtime32(&startTime);
		if (startTime > 0 && startDate)
		{
			startDay = startDate->tm_mday;
			startHour = startDate->tm_hour;
		}

		int localDay = 0;
		int localHour = 0;
		__time32_t localTime;
		_time32(&localTime);
		struct tm *localDate = _localtime32(&localTime);
		if (localTime > 0 && localDate)
		{
			localDay = localDate->tm_mday;
			localHour = localDate->tm_hour;
		}

		if (startTime == 0)
		{
			iter->second.State = GAS_NONE;
		} 
		else
		{
			/*
			// 凌晨4:00重置版本
			// 任务 #25369
			if (startDay < localDay - 1)
			{
				iter->second.State = GAS_NONE;
			}
			else if (startDay == localDay - 1)
			{// 开启时间是昨天
				__time32_t destTime = startTime + data.Duration;
				if (startHour < 4)
				{
					iter->second.State = GAS_NONE;
				}
				else
				{
					if (localHour < 4)
					{// 开启时间在昨天的,没过今天凌晨4:00都还是前一天的状态
						iter->second.State = destTime > localTime ? GAS_OPEN : GAS_OVER;
					}
					else
					{// 开启时间在昨天的,过了今天凌晨4:00都变为可开启
						iter->second.State = GAS_NONE;
					}
				}
			}
			else if (startDay == localDay)
			{// 开启时间是今天
				__time32_t destTime = startTime + data.Duration;
				if (startHour < 4)
				{
					if (localHour < 4)
					{// 开启时间在今天凌晨4:00前的,没过今天凌晨4:00都还是前一天的状态
						iter->second.State = destTime > localTime ? GAS_OPEN : GAS_OVER;
					}
					else
					{// 开启时间在今天凌晨4:00前的,过了今天凌晨4:00都变为可开启
						iter->second.State = GAS_NONE;
					}
				}
				else
				{
					iter->second.State = destTime > localTime ? GAS_OPEN : GAS_OVER;
				}
			}
			else
			{
				iter->second.State = GAS_NONE;
			}
			*/

			// 凌晨0:00重置版本
			if (startDay < localDay)
			{// 开启时间是昨天
				__time32_t destTime = startTime + data.Duration;
				iter->second.State = destTime > localTime ? GAS_OPEN : GAS_NONE;
			}
			else if (startDay == localDay)
			{// 开启时间是今天
				__time32_t destTime = startTime + data.Duration;
				iter->second.State = destTime > localTime ? GAS_OPEN : GAS_OVER;
			}
			else
			{
				iter->second.State = GAS_NONE;
			}
		}
	}
}

void MMOGuildData::UpdateGuildActivityState()
{
	GuildActivityInfoMap::iterator iter = _GuildActivityInfoMap.begin();
	for (; iter != _GuildActivityInfoMap.end(); ++ iter)
	{
		GuildBaseInfo baseInfo = GetGuildBaseInfo();
		UINT16 activityType = iter->first;
		UINT16 activityLevel = baseInfo.level;
		UINT32 activityId = MAKELONG(activityLevel, activityType);

		GuildActivityData data;
		if (!GetGuildActivityData(activityId, data))
		{
			continue;
		}

		int startDay = 0;
		int startHour = 0;
		__time32_t startTime = iter->second.StartTime;
		struct tm *startDate = _localtime32(&startTime);
		if (startTime > 0 && startDate)
		{
			startDay = startDate->tm_mday;
			startHour = startDate->tm_hour;
		}

		int localDay = 0;
		int localHour = 0;
		__time32_t localTime;
		_time32(&localTime);
		struct tm *localDate = _localtime32(&localTime);
		if (localTime > 0 && localDate)
		{
			localDay = localDate->tm_mday;
			localHour = localDate->tm_hour;
		}

		if (startTime == 0)
		{
			iter->second.State = GAS_NONE;
		}
		else
		{
			/*
			// 凌晨4:00重置版本
			// 任务 #25369
			if (startDay < localDay - 1)
			{
				iter->second.State = GAS_NONE;
			}
			else if (startDay == localDay - 1)
			{// 开启时间是昨天
				__time32_t destTime = startTime + data.Duration;
				if (startHour < 4)
				{
					iter->second.State = GAS_NONE;
				}
				else
				{
					if (localHour < 4)
					{// 开启时间在昨天的, 没过今天凌晨4:00都还是前一天的状态
						iter->second.State = destTime > localTime ? GAS_OPEN : GAS_OVER;
					}
					else
					{// 开启时间在昨天的,过了今天凌晨4:00都变为可开启
						iter->second.State = GAS_NONE;
					}
				}
			}
			else if (startDay == localDay)
			{// 开启时间是今天
				__time32_t destTime = startTime + data.Duration;
				if (startHour < 4)
				{
					if (localHour < 4)
					{// 开启时间在今天凌晨4:00前的,没过今天凌晨4:00都还是前一天的状态
						iter->second.State = destTime > localTime ? GAS_OPEN : GAS_OVER;
					}
					else
					{// 开启时间在今天凌晨4:00前的,过了今天凌晨4:00都变为可开启
						iter->second.State = GAS_NONE;
					}
				}
				else
				{
					iter->second.State = destTime > localTime ? GAS_OPEN : GAS_OVER;
				}
			}
			else
			{
				iter->second.State = GAS_NONE;
			}
			*/

			// 凌晨0:00重置版本
			if (startDay < localDay)
			{// 开启时间是昨天
				__time32_t destTime = startTime + data.Duration;
				iter->second.State = destTime > localTime ? GAS_OPEN : GAS_NONE;
			}
			else if (startDay == localDay)
			{// 开启时间是今天
				__time32_t destTime = startTime + data.Duration;
				iter->second.State = destTime > localTime ? GAS_OPEN : GAS_OVER;
			}
			else
			{
				iter->second.State = GAS_NONE;
			}
		}
	}
}

UINT16 MMOGuildData::GetGuildActivityTypeByVariable(UINT32 variable)
{
	GuildActivityVariableMap::iterator iter = _GuildActivityVariableMap.find(variable);
	if (iter != _GuildActivityVariableMap.end())
	{
		return iter->second;
	}

	return 0;
}

const GuildActivityInfoMap& MMOGuildData::GetGuildActivityInfoMap()
{
	return _GuildActivityInfoMap;
}

UINT32 MMOGuildData::_GetGuildWelfareRealId(UINT32 welfareId, UINT32 welfareLv)
{
	return  welfareId << 16 | welfareLv;
}

void MMOGuildData::AddGuildBuildingData(const GuildBuildingData& data)
{
	UINT32 index = _GetGuildBuildingRealId(data.BuildingType, data.Level);
	_GuildBuildingDataMap.insert(std::make_pair(index, data));
}

UBOOL MMOGuildData::GetGuildBuildingData(UINT32 buildingType, UINT32 buildingLv, GuildBuildingData& data)
{
	UINT32 index = _GetGuildBuildingRealId(buildingType, buildingLv);
	GuildBuildingDataMap::iterator iter = _GuildBuildingDataMap.find(index);
	if (iter != _GuildBuildingDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

void MMOGuildData::AddGuildWelfareData(const GuildWelfareData& data)
{
	UINT32 index = _GetGuildWelfareRealId(data.WelfareId, data.Level);
	_GuildWelfareDataMap.insert(std::make_pair(index, data));
}

UBOOL MMOGuildData::GetGuildWelfareData(UINT32 welfareId, UINT32 welfareLv, GuildWelfareData& data)
{
	UINT32 index = _GetGuildWelfareRealId(welfareId, welfareLv);
	GuildWelfareDataMap::iterator iter = _GuildWelfareDataMap.find(index);
	if (iter != _GuildWelfareDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

const GuildBuildingUpgradeList& MMOGuildData::GetGuildBuildingUpgradeList()
{
	return _GuildBuildingUpgradeList;
}

void MMOGuildData::UpdateGuildBuildingList(const G_B_DATA (&arrBuilding)[GUILD_MAX_BUILDING_NUM])
{
	_GuildBuildingList.clear();
	for (INT i = 0; i < GUILD_MAX_BUILDING_NUM; ++ i)
	{
		GuildBuildingInfo info;
		info.BuildingId = arrBuilding[i].qwNameID;
		info.BuildingType = arrBuilding[i].nType;
		info.BuildingLv = arrBuilding[i].nLevel;
		info.UpgradeLeftTime = 0;
		info.UpgradeEndTime = 0;
		AddGuildBuilding(info);
	}
}

void MMOGuildData::AddGuildBuilding(const GuildBuildingInfo& info)
{
	_GuildBuildingList.push_back(info);
}

void MMOGuildData::DelGuildBuilding(UINT64 buildingId)
{
	GuildBuildingList::iterator iter = _GuildBuildingList.begin();
	for (; iter != _GuildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingId == buildingId)
		{
			_GuildBuildingList.erase(iter);
			return;
		}
	}
}

void MMOGuildData::UpgradeGuildBuilding(UINT64 buildingId)
{
	GuildBuildingList::iterator iter = _GuildBuildingList.begin();
	for (; iter != _GuildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingId == buildingId)
		{
			iter->BuildingLv += 1;
			return;
		}
	}
}

UBOOL MMOGuildData::IsGuildBuildingExist(UINT64 buildingId)
{
	GuildBuildingList::iterator iter = _GuildBuildingList.begin();
	for (; iter!= _GuildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingId == buildingId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void MMOGuildData::AddGuildBuildingUpgradeInfo(const GuildBuildingInfo& info)
{
	_GuildBuildingUpgradeList.push_back(info);
}

void MMOGuildData::DelGuildBuildingUpgradeInfo(UINT64 buildingId)
{
	GuildBuildingUpgradeList::iterator iter = _GuildBuildingUpgradeList.begin();
	for (; iter!= _GuildBuildingUpgradeList.end(); ++ iter)
	{
		if (iter->BuildingId == buildingId)
		{
			_GuildBuildingUpgradeList.erase(iter);
			break;
		}
	}
}

const GuildBuildingList& MMOGuildData::GetGuildBuildingList()
{
	return _GuildBuildingList;
}

UINT32 MMOGuildData::GetGuildBuildingLevel(UINT64 buildingId)
{
	GuildBuildingList::iterator iter = _GuildBuildingList.begin();
	for (; iter != _GuildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingId == buildingId)
		{
			return iter->BuildingLv;
		}
	}

	return 0;
}

UINT32 MMOGuildData::GetGuildBuildingType(UINT64 buildingId)
{
	GuildBuildingList::iterator iter = _GuildBuildingList.begin();
	for (; iter != _GuildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingId == buildingId)
		{
			return iter->BuildingType;
		}
	}

	return GUILD_B_NONE;
}

UINT32 MMOGuildData::GetGuildUpgradeBuildingType(UINT64 buildingId)
{
	GuildBuildingUpgradeList::iterator iter = _GuildBuildingUpgradeList.begin();
	for (; iter!= _GuildBuildingUpgradeList.end(); ++ iter)
	{
		if (iter->BuildingId == buildingId)
		{
			return iter->BuildingType;
		}
	}

	return GUILD_B_NONE;
}

UBOOL MMOGuildData::IsGuildBuildingBase(UINT32 npcTempId)
{
	GuildBuildingDataMap::iterator iter = _GuildBuildingDataMap.begin();
	for (; iter != _GuildBuildingDataMap.end(); ++ iter)
	{
		if (npcTempId == iter->second.BaseNpcId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMOGuildData::IsGuildBuilding(UINT32 npcTempId)
{
	GuildBuildingDataMap::iterator iter = _GuildBuildingDataMap.begin();
	for (; iter != _GuildBuildingDataMap.end(); ++ iter)
	{
		if (npcTempId == iter->second.NpcId)
		{
			return TRUE;
		}
	}
	return FALSE;
}

UINT64 MMOGuildData::GetGuildBuildingId(UINT32 npcTempId)
{
	GuildBuildingDataMap::iterator iterMap = _GuildBuildingDataMap.begin();
	for (; iterMap != _GuildBuildingDataMap.end(); ++ iterMap)
	{
		if (npcTempId == iterMap->second.NpcId)
		{
			GuildBuildingList::iterator iterList = _GuildBuildingList.begin();
			for (; iterList != _GuildBuildingList.end(); ++ iterList)
			{
				if (iterList->BuildingType == iterMap->second.BuildingType && iterList->BuildingLv == iterMap->second.Level)
				{
					return iterList->BuildingId;
				}
			}
			break;
		}
	}

	return 0;
}

UINT32 MMOGuildData::GetGuildBuildingCount(UINT32 buildingType)
{
	UINT32 count = 0;
	GuildBuildingList::iterator iter = _GuildBuildingList.begin();
	for (; iter != _GuildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingType == buildingType)
		{
			count ++;
		}
	}

	return count;
}

UINT32 MMOGuildData::GetGuildBuildingMaxLevel(UINT32 buildingType)
{
	UINT32 maxLevel = 0;
	GuildBuildingList::iterator iter = _GuildBuildingList.begin();
	for (; iter != _GuildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingType == buildingType)
		{
			if (iter->BuildingLv > maxLevel)
			{
				maxLevel = iter->BuildingLv;
			}
		}
	}

	return maxLevel;
}

UINT32 MMOGuildData::GetGuildTerritoryResMax(UINT32 type)
{
	UINT32 moneyMax = 0;
	UINT32 herbMax = 0;
	UINT32 woodMax = 0;
	UINT32 crystalMax = 0;
	UINT32 silverMax = 0;

	// 初始化
	GuildBuildingData buildingData;
	if (GetGuildBuildingData(GUILD_B_STORAGE, GetGuildBuildingMaxLevel(GUILD_B_STORAGE), buildingData))
	{
		moneyMax = buildingData.Param1;
	}
	if (GetGuildBuildingData(GUILD_B_HERB_STORAGE, GetGuildBuildingMaxLevel(GUILD_B_HERB_STORAGE), buildingData))
	{
		herbMax = buildingData.Param1;
	}
	if (GetGuildBuildingData(GUILD_B_WOOD_STORAGE, GetGuildBuildingMaxLevel(GUILD_B_WOOD_STORAGE), buildingData))
	{
		woodMax = buildingData.Param1;
	}
	if (GetGuildBuildingData(GUILD_B_CRYSTAL_STORAGE, GetGuildBuildingMaxLevel(GUILD_B_CRYSTAL_STORAGE), buildingData))
	{
		crystalMax = buildingData.Param1;
	}
	if (GetGuildBuildingData(GUILD_B_SILVER_STORAGE, GetGuildBuildingMaxLevel(GUILD_B_SILVER_STORAGE), buildingData))
	{
		silverMax = buildingData.Param1;
	}

	switch (type)
	{
	case GUILD_ATTR_MONEY:
		return moneyMax;
		break;
	case GUILD_ATTR_WOOD:
		return woodMax;
		break;
	case GUILD_ATTR_HERB:
		return herbMax;
		break;
	case GUILD_ATTR_CRYSTAL:
		return crystalMax;
		break;
	case GUILD_ATTR_SILVER:
		return silverMax;
		break;
	}

	return 0;
}

UINT32 MMOGuildData::GetCurGuildMemberMax()
{
	UINT32 memberMax = 0;
	GuildBuildingData buildingData;
	if (GetGuildBuildingData(GUILD_B_HOUSE, GetGuildBuildingMaxLevel(GUILD_B_HOUSE), buildingData))
	{
		memberMax = buildingData.Param1;
	}

	return memberMax;
}

UINT32 MMOGuildData::GetCurGuildTerritoryRes(UINT32 type)
{
	GuildBaseInfo baseInfo = GetGuildBaseInfo();
	switch (type)
	{
	case GUILD_ATTR_MONEY:
		return baseInfo.Money;
		break;
	case GUILD_ATTR_WOOD:
		return baseInfo.Wood;
		break;
	case GUILD_ATTR_HERB:
		return baseInfo.Herb;
		break;
	case GUILD_ATTR_CRYSTAL:
		return baseInfo.Crystal;
		break;
	case GUILD_ATTR_SILVER:
		return baseInfo.Silver;
		break;
	}

	return 0;
}

void MMOGuildData::GetGuildTerritoryResName(UINT32 type, FString& name)
{
	name = TEXT("错误资源类型");
	switch (type)
	{
	case GUILD_ATTR_MONEY:
		name = TEXT("公会资金");
		break;
	case GUILD_ATTR_WOOD:
		name = TEXT("木材");
		break;
	case GUILD_ATTR_HERB:
		name = TEXT("粮食");
		break;
	case GUILD_ATTR_CRYSTAL:
		name = TEXT("石料");
		break;
	case GUILD_ATTR_SILVER:
		name = TEXT("铁矿");
		break;
	}
}

const GuildWelfareUpgradeList& MMOGuildData::GetGuildWelfareUpgradeList()
{
	return _GuildWelfareUpgradeList;
}

const GuildWelfareInfoList& MMOGuildData::GetGuildWelfareList()
{
	return _GuildWelfareInfoList;
}

UINT32 MMOGuildData::GetGuildWelfareLevel(UINT32 welfareId)
{
	GuildWelfareInfoList::iterator iter = _GuildWelfareInfoList.begin();
	for (; iter != _GuildWelfareInfoList.end(); ++ iter)
	{
		if (iter->WelfareId == welfareId)
		{
			return iter->WelfareLv;
		}
	}

	return 0;
}

UBOOL MMOGuildData::IsGuildWelfarePurchase(UINT32 welfareId)
{
	return TRUE;
}

void MMOGuildData::UpdateGuildWelfareList(const INT8 (&arrWelfareLv)[GUILD_MAX_WELFARE_AURA_COUNT],
	const INT8 (&arrWelfareUpId)[GUILD_MAX_UPGRADE_AURA_COUNT], 
	const INT32 (&arrWelfareUpTime)[GUILD_MAX_UPGRADE_AURA_COUNT])
{
	_GuildWelfareInfoList.clear();
	for (INT i = 1; i < GUILD_MAX_WELFARE_AURA_COUNT; ++ i)
	{
		GuildWelfareInfo info;
		info.WelfareId = i;
		info.WelfareLv = arrWelfareLv[i];
		info.AlreadyBuyTime = 0;

		_GuildWelfareInfoList.push_back(info);
	}

	_GuildWelfareUpgradeList.clear(); 
	for (INT i = 0; i < GUILD_MAX_UPGRADE_AURA_COUNT; ++ i)
	{
		if (arrWelfareUpTime[i] > 0)
		{
			GuildWelfareUpgradeInfo upInfo;
			upInfo.WelfareId = arrWelfareUpId[i];
			upInfo.WelfareLv = GetGuildWelfareLevel(arrWelfareUpId[i]);
			upInfo.UpgradeLeftTime = arrWelfareUpTime[i];

			__time32_t localTime;
			_time32(&localTime);
			_localtime32(&localTime);
			upInfo.UpgradeEndTime = localTime + arrWelfareUpTime[i];

			AddGuildWelfareUpgradeInfo(upInfo);
		}
	}
}

void MMOGuildData::UpgradeGuildWelfare(UINT32 welfareId, UINT32 welfareLv)
{
	GuildWelfareInfoList::iterator iter = _GuildWelfareInfoList.begin();
	for (; iter != _GuildWelfareInfoList.end(); ++ iter)
	{
		if (iter->WelfareId == welfareId)
		{
			iter->WelfareLv = welfareLv;
			break;
		}
	}
}

void MMOGuildData::AddGuildWelfareUpgradeInfo(const GuildWelfareUpgradeInfo& info)
{
	_GuildWelfareUpgradeList.push_back(info);
}

void MMOGuildData::DelGuildWelfareUpgradeInfo(UINT32 welfareId)
{
	GuildWelfareUpgradeList::iterator iterUp = _GuildWelfareUpgradeList.begin();
	for (; iterUp!= _GuildWelfareUpgradeList.end(); ++ iterUp)
	{
		if (iterUp->WelfareId == welfareId)
		{
			_GuildWelfareUpgradeList.erase(iterUp);
			break;
		}
	}
}

UBOOL MMOGuildData::IsGuildBuildingUpgrading(UINT64 buildingId)
{
	GuildBuildingUpgradeList::iterator iter = _GuildBuildingUpgradeList.begin();
	for (; iter!= _GuildBuildingUpgradeList.end(); ++ iter)
	{
		if (iter->BuildingId == buildingId)
		{
			return TRUE;
		}
	}

	return FALSE;
}


UBOOL MMOGuildData::IsGuildWelfareUpgrading(UINT32 welfareId)
{
	GuildWelfareUpgradeList::iterator iter = _GuildWelfareUpgradeList.begin();
	for (; iter!= _GuildWelfareUpgradeList.end(); ++ iter)
	{
		if (iter->WelfareId == welfareId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void MMOGuildData::UpdateGuildWelfareAlreadyBoughtTime(UINT32 welfareId, UINT8 times)
{
	GuildWelfareInfoList::iterator iter = _GuildWelfareInfoList.begin();
	for (; iter != _GuildWelfareInfoList.end(); ++ iter)
	{
		if (iter->WelfareId == welfareId)
		{
			iter->AlreadyBuyTime = times;
			break;
		}
	}
}

void MMOGuildData::GetGuildWelfareListByGroup(UINT32 groupId, GuildWelfareInfoList& list)
{
	GuildWelfareInfoList::iterator iter = _GuildWelfareInfoList.begin();
	for (; iter != _GuildWelfareInfoList.end(); ++ iter)
	{
		UINT32 welfareId = iter->WelfareId;
		UINT32 welfareLv = iter->WelfareLv;

		GuildWelfareData welfareData;
		if (!GetGuildWelfareData(welfareId, welfareLv, welfareData))
		{
			continue;
		}

		if (welfareData.Type == groupId)
		{
			list.push_back(*iter);
		}
	}
}

UINT32 MMOGuildData::GetGuildWelfareGroupCount()
{
	std::set<UINT32> typeSet;
	typeSet.clear();

	GuildWelfareInfoList::iterator iter = _GuildWelfareInfoList.begin();
	for (; iter != _GuildWelfareInfoList.end(); ++ iter)
	{
		UINT32 welfareId = iter->WelfareId;
		UINT32 welfareLv = iter->WelfareLv;

		GuildWelfareData welfareData;
		if (!GetGuildWelfareData(welfareId, welfareLv, welfareData))
		{
			continue;
		}

		typeSet.insert(welfareData.Type);
	}

	return typeSet.size();
}

void MMOGuildData::UpdateGuildBuildingUpgradeTime(UINT32 count)
{
	if (_GuildBuildingUpgradeList.size() > 0)
	{
		GuildBuildingUpgradeList::iterator iter = _GuildBuildingUpgradeList.begin();
		for (; iter != _GuildBuildingUpgradeList.end(); ++ iter)
		{
			if (iter->UpgradeLeftTime > 0)
			{
				iter->UpgradeLeftTime -= count;
			}
		}
	}
}

void MMOGuildData::UpdateGuildWelfareUpgradeTime(UINT32 count)
{
	if (_GuildWelfareUpgradeList.size() > 0)
	{
		GuildWelfareUpgradeList::iterator iter = _GuildWelfareUpgradeList.begin();
		for (; iter != _GuildWelfareUpgradeList.end(); ++iter)
		{
			if (iter->UpgradeLeftTime > 0)
			{
				iter->UpgradeLeftTime -= count;
			}
		}
	}
}

void MMOGuildData::AddGuildDungeonData(const GuildDungeonData& data)
{
	_GuildDungeonDataMap.insert(std::make_pair(data.DungeonId, data));
}

const GuildDungeonDataMap& MMOGuildData::GetGuildDungeonDataMap()
{
	return _GuildDungeonDataMap;
}

const GuildDungeonAwardList& MMOGuildData::GetGuildDungeonAwardList()
{
	return _GuildDungeonAwardList;
}

void MMOGuildData::UpdateGuildDungeonAwardList(GUILD_DUNGEON_AWARD_INFO* list, UINT8 size)
{
	_GuildDungeonAwardList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		if (i < GUILD_DUNGEON_AWARD_NUM)
		{
			GUILD_DUNGEON_AWARD_INFO info;
			info.dwItemID = list[i].dwItemID;
			info.byCount = list[i].byCount;

			_GuildDungeonAwardList.push_back(info);
		}
	}
}

const GuildWarResultList& MMOGuildData::GetGuildWarResultList()
{
	return _GuildWarResultList;
}

void MMOGuildData::UpdateGuildWarResultList(GUILD_BATTLE_ACHIEVEMENT_DATA* list, UINT8 size)
{
	_GuildWarResultList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		GUILD_BATTLE_ACHIEVEMENT_DATA info;
		info.dwRoleID = list[i].dwRoleID;
		info.nKillNum = list[i].nKillNum;
		info.nDealCount = list[i].nDealCount;
		info.nSalary = list[i].nSalary;

		_GuildWarResultList.push_back(info);
	}
}

void MMOGuildData::UpdateGuildWarRecords(GUILD_WAR_RECORD_INFO* records, UINT8 size)
{
	_GuildWarRecordList.clear();
	for (INT i = 0; i < size; ++i)
	{
		AddGuildWarRecord(records[i]);

		MMOGuildManager::getSingleton().QueryGuildInfo(records[i].dwDefenderID);
		MMOGuildManager::getSingleton().QueryGuildInfo(records[i].dwIntruderID);
	}
}

void MMOGuildData::AddGuildWarRecord(const GUILD_WAR_RECORD_INFO& recordInfo)
{
	_GuildWarRecordList.push_back(recordInfo);
}

const GuildWarRecordList& MMOGuildData::GetGuildWarRecordList()
{
	return _GuildWarRecordList;
}

void MMOGuildData::SetMyGuildWarAttackId(UINT32 id)
{
	_MyGuildWarAttackId = id;
}

void MMOGuildData::SetMyGuildWarDefendId(UINT32 id)
{
	_MyGuildWarDefendId = id;
}

UBOOL MMOGuildData::GetGuildInfoById(UINT32 guildId, GuildQueryInfo& info)
{
	QueryGuildInfoByIdMap::iterator iter = _QueryGuildInfoByIdMap.find(guildId);
	if (iter != _QueryGuildInfoByIdMap.end())
	{
		info = iter->second;
		return TRUE;
	}

	return FALSE;
}

const QueryGuildInfoByIdMap& MMOGuildData::GetQueryGuildInfoByIdMap()
{
	return _QueryGuildInfoByIdMap;
}

UBOOL MMOGuildData::GetGuildInfoByName(const FString& guildName, GuildQueryInfo& info)
{
	QueryGuildInfoByNameMap::iterator iter = _QueryGuildInfoByNameMap.find(guildName);
	if (iter != _QueryGuildInfoByNameMap.end())
	{
		info = iter->second;
		return TRUE;
	}

	return FALSE;
}

void MMOGuildData::AddOrUpdateGuildInfo(M2C_QUERY_GUILD_NAME_ACK* msg)
{
	if (!msg)
	{
		return;
	}

	QueryGuildInfoByIdMap::iterator iterId = _QueryGuildInfoByIdMap.find(msg->dwGuildID);
	if (iterId == _QueryGuildInfoByIdMap.end())
	{ 
		GuildQueryInfo info;
		info.Id = msg->dwGuildID;
		info.Name = UTF8_TO_TCHAR(msg->szGuildName);
		info.LeaderId = msg->dwLeaderID;
		info.MemberCount = msg->byMemberCount;
		info.Level = msg->byMainLevel;
		info.BuildingLevel = msg->byHouseLevel;
		info.SabacState = msg->bySabacState;
		info.CreateTime = msg->dwCreateTime;
		//info.Crc = appMemCrc(msg, sizeof(M2C_QUERY_GUILD_NAME_ACK));
		info.Crc = crc32(0, (const Bytef*)msg, sizeof(M2C_QUERY_GUILD_NAME_ACK));

		_QueryGuildInfoByIdMap.insert(std::make_pair(info.Id, info));
	}
	else
	{
		iterId->second.Id = msg->dwGuildID;
		iterId->second.Name = UTF8_TO_TCHAR(msg->szGuildName);
		iterId->second.LeaderId = msg->dwLeaderID;
		iterId->second.MemberCount = msg->byMemberCount;
		iterId->second.Level = msg->byMainLevel;
		iterId->second.BuildingLevel = msg->byHouseLevel;
		iterId->second.SabacState = msg->bySabacState;
		iterId->second.CreateTime = msg->dwCreateTime;
		//iterId->second.Crc = appMemCrc(msg, sizeof(M2C_QUERY_GUILD_NAME_ACK));
		iterId->second.Crc = crc32(0, (const Bytef*)msg, sizeof(M2C_QUERY_GUILD_NAME_ACK));
	}

	QueryGuildInfoByNameMap::iterator iterName = _QueryGuildInfoByNameMap.find(FString(UTF8_TO_TCHAR(msg->szGuildName)));
	if (iterName == _QueryGuildInfoByNameMap.end())
	{ 
		GuildQueryInfo info;
		info.Id = msg->dwGuildID;
		info.Name = UTF8_TO_TCHAR(msg->szGuildName);
		info.LeaderId = msg->dwLeaderID;
		info.MemberCount = msg->byMemberCount;
		info.Level = msg->byMainLevel;
		info.BuildingLevel = msg->byHouseLevel;
		info.SabacState = msg->bySabacState;
		info.CreateTime = msg->dwCreateTime;
		//info.Crc = appMemCrc(msg, sizeof(M2C_QUERY_GUILD_NAME_ACK));
		info.Crc = crc32(0, (const Bytef*)msg, sizeof(M2C_QUERY_GUILD_NAME_ACK));

		_QueryGuildInfoByNameMap.insert(std::make_pair(FString(UTF8_TO_TCHAR(msg->szGuildName)), info));
	}
	else
	{
		iterName->second.Id = msg->dwGuildID;
		iterName->second.Name = UTF8_TO_TCHAR(msg->szGuildName);
		iterName->second.LeaderId = msg->dwLeaderID;
		iterName->second.MemberCount = msg->byMemberCount;
		iterName->second.Level = msg->byMainLevel;
		iterName->second.BuildingLevel = msg->byHouseLevel;
		iterName->second.SabacState = msg->bySabacState;
		iterName->second.CreateTime = msg->dwCreateTime;
		//iterName->second.Crc = appMemCrc(msg, sizeof(M2C_QUERY_GUILD_NAME_ACK));
		iterName->second.Crc = crc32(0, (const Bytef*)msg, sizeof(M2C_QUERY_GUILD_NAME_ACK));
	}

	// 查一下会长的信息
	MMOFacade::getSingleton().RequestPlayerBaseInfo(msg->dwLeaderID);
}

void MMOGuildData::UpdateSabacEnrollGuildList(G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA* list, UINT16 size)
{
	_SabacEnrollGuildIdList.clear();
	for (INT i = 0; i < size; ++ i)
	{
		_SabacEnrollGuildIdList.push_back(list[i]);
		MMOGuildManager::getSingleton().QueryGuildInfo(list[i].dwGuildID);
	}
}

void MMOGuildData::UpdateSabacOccupyGuild(UINT32 guildId)
{
	_CurOccupySabacGuildId = guildId;
}

GuildSabacState MMOGuildData::GetGuildSabacState(AMMOPlayer* player)
{
	if (!player)
	{
		return GSS_NULL;
	}

	// 还没开战
	if (MMOGuildManager::getSingleton().GetSabacState() == 0)
	{
		return GSS_NULL;
	}

	// 看自己始终蓝色
	if (player->IsMainPlayer())
	{
		return GSS_BLUE;
	}

	// 看没有公会的人始终绿色
	if (player->GuildName == TEXT(""))
	{
		return GSS_GREEN;
	}

	// 自己没有公会，看别人都是绿色
	if (_GuildBaseInfo.Id == 0)
	{
		return GSS_GREEN;
	}

	// 和我一个公会
	if (_GuildBaseInfo.Name == player->GuildName)
	{
		return GSS_BLUE;
	}	

	// 和我是联盟
	GuildRelation relation = GetGuildRelation(player->GuildName);
	if (relation == GR_FRIEND)
	{
		return GSS_BLUE;
	}

	// 如果即有公会关系，又有sabac关系，sabac关系优先级高
	UINT32 myGuildId = 0;
	int mySabacState = 0;
	UINT32 otherGuildId = 0;
	int otherSabacState = 0;
	QueryGuildInfoByNameMap::iterator iter = _QueryGuildInfoByNameMap.find(_GuildBaseInfo.Name);
	if (iter != _QueryGuildInfoByNameMap.end())
	{ 
		myGuildId = iter->second.Id;
		mySabacState = iter->second.SabacState;
	}
	iter = _QueryGuildInfoByNameMap.find(player->GuildName);
	if (iter != _QueryGuildInfoByNameMap.end())
	{ 
		otherGuildId = iter->second.Id;
		otherSabacState = iter->second.SabacState;
	}

	if (mySabacState == 0)
	{// 我是第三方公会，看别人都是绿的
		return GSS_GREEN;
	}
	
	if (otherSabacState == 0)
	{// 我是参战公会，看第三方都是绿的
		return GSS_GREEN;
	}
	
	if (myGuildId == _CurOccupySabacGuildId)
	{// 我是占领公会，看占领公会是蓝的，看进攻公会是橙色的
		return otherGuildId == _CurOccupySabacGuildId ? GSS_BLUE : GSS_ORANGE;
	}
	else
	{// 我是进攻公会，看其他公会是橙的(任务 #20918)
		return GSS_ORANGE;
	}
	
	return GSS_NULL;
}

void MMOGuildData::UpdateStatueData(const SabacStatue& statue)
{
	_CurSabacStatue.RoleId = statue.RoleId;
	_CurSabacStatue.RoleName = statue.RoleName;
	_CurSabacStatue.RoleClass = statue.RoleClass;
	_CurSabacStatue.RoleSex = statue.RoleSex;
	_CurSabacStatue.HairType = statue.HairType;
	_CurSabacStatue.HairColor = statue.HairColor;
	_CurSabacStatue.FaceType = statue.FaceType;
	_CurSabacStatue.SkinColor = statue.SkinColor;
	_CurSabacStatue.TitleId = statue.TitleId;
	_CurSabacStatue.RefineFx = statue.RefineFx;
	appMemcpy(_CurSabacStatue.Avatar, statue.Avatar, sizeof(INT32) * NES_VISIBLE_TOTAL);
}

UBOOL MMOGuildData::GetStatueData(SabacStatue& statue)
{
	statue.RoleId = _CurSabacStatue.RoleId;
	statue.RoleName = _CurSabacStatue.RoleName;
	statue.RoleClass = _CurSabacStatue.RoleClass;
	statue.RoleSex = _CurSabacStatue.RoleSex;
	statue.HairType = _CurSabacStatue.HairType;
	statue.HairColor = _CurSabacStatue.HairColor;
	statue.FaceType = _CurSabacStatue.FaceType;
	statue.SkinColor = _CurSabacStatue.SkinColor;
	statue.TitleId = _CurSabacStatue.TitleId;
	statue.RefineFx = _CurSabacStatue.RefineFx;
	appMemcpy(statue.Avatar, _CurSabacStatue.Avatar, sizeof(INT32) * NES_VISIBLE_TOTAL);

	return statue.RoleId > 0;
}

void MMOGuildData::ClearVariable()
{
	_GuildVariableMap.clear();
	_GuildVariableMap.insert(std::make_pair(EGARV_ID_1, 0));
	_GuildVariableMap.insert(std::make_pair(EGARV_ID_2, 0));
	_GuildVariableMap.insert(std::make_pair(EGARV_ID_3, 0));
	_GuildVariableMap.insert(std::make_pair(EGARV_ID_4, 0));
	_GuildVariableMap.insert(std::make_pair(EGARV_ID_5, 0));
}

void MMOGuildData::UpdateVariable(UINT8 varType, INT32 referId, UINT16 index, INT32 val)
{
	if (varType == vtRole)
	{
		GuildVariableMap::iterator iter = _GuildVariableMap.find(index);
		if (iter != _GuildVariableMap.end())
		{
			iter->second = val;
		}
	}
}

INT32 MMOGuildData::GetVariableValue(UINT8 varType, UINT16 index)
{
	if (varType == vtRole)
	{
		GuildVariableMap::iterator iter = _GuildVariableMap.find(index);
		if (iter != _GuildVariableMap.end())
		{
			return iter->second;
		}
	}

	return 0;
}
