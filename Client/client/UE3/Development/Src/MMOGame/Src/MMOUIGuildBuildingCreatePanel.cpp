#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include <time.h>


IMPLEMENT_CLASS(UMMOUIGuildBuildingCreatePanel)

void UMMOUIGuildBuildingCreatePanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildBuildingCreatePanel>(this, evts);
}

void UMMOUIGuildBuildingCreatePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
	}
	else
	{
		_NpcId = 0;
	}
}

void UMMOUIGuildBuildingCreatePanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if (gameEvent->GetEventType() != GameEvent_Guild)
	{
		return;
	}

	MMOGameEvent_Guild* guildEvent = (MMOGameEvent_Guild*)gameEvent;
	if (!guildEvent)
	{
		return;
	}
}

void UMMOUIGuildBuildingCreatePanel::ShowGuildBuildingCreatePanel(UINT32 npcId, UINT32 npcTempId)
{
	Show(TRUE);
	UpdateGuildBuildingList(npcTempId);
	eventCallUpdateGuildBuildingData(NULL);
	_NpcId = npcId;
}

void UMMOUIGuildBuildingCreatePanel::UpdateGuildBuildingList(UINT32 npcTempId)
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	INT index = 0;
	for (INT i = GUILD_B_MAIN; i < GUILD_B_NUM; ++ i)
	{
		if (i == GUILD_B_GATE ||
			i == GUILD_B_WALL)
		{
			continue;
		}

		UINT32 buildingType = i;
		UINT32 buildingLv = 1;
		GuildBuildingData buildingData;
		if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv, buildingData))
		{
			if (buildingData.MaxNum == 1 && 
				MMOGuildData::getSingleton().GetGuildBuildingCount(buildingType) == buildingData.MaxNum)
			{
				continue;
			}

			if (npcTempId == buildingData.BaseNpcId)
			{
				UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
				if (object)
				{
					object->SetString(TEXT("label"), buildingData.Name);
					object->SetInt(TEXT("index"), index);
					object->SetInt(TEXT("type"), 0);
					object->SetInt(TEXT("deepth"), 0);
					object->SetBool(TEXT("visible"), true);

					object->SetInt(TEXT("guild_building_type"), buildingType);
					object->SetString(TEXT("guild_building_name"), buildingData.Name);
					FString buildingIcon = FString::Printf(TEXT("img://UI_GuildTerritoryWidget.%d"), buildingData.BuildingType);
					object->SetString(TEXT("guild_building_icon"), buildingIcon);
				
					objectArray->SetElementObject(index, object);
					index ++;
				}
			}
		}
	}
	eventCallUpdateGuildBuildingList(objectArray);
}

void UMMOUIGuildBuildingCreatePanel::UpdateGuildBuildingData(UINT32 buildingType)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (!object)
	{
		return;
	}

	UINT32 buildingLv = 0;
	GuildBuildingData buildingData;
	if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv, buildingData))
	{
		object->SetInt(TEXT("guild_building_type"), buildingType);
		object->SetString(TEXT("guild_building_name"), buildingData.Name);
		object->SetInt(TEXT("guild_building_level"), 0);

		FString buildingIcon = FString::Printf(TEXT("img://UI_GuildTerritoryWidget.%d"), buildingData.BuildingType);
		object->SetString(TEXT("guild_building_icon"), buildingIcon);
		object->SetString(TEXT("guild_building_desc"), buildingData.Description);

		object->SetInt(TEXT("guild_building_cur_count"), MMOGuildData::getSingleton().GetGuildBuildingCount(buildingType));
		object->SetInt(TEXT("guild_building_max_count"), buildingData.MaxNum);

		FString keepup = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Keepup")), buildingData.RoutineReq);
		object->SetString(TEXT("guild_building_keepup"), keepup);

		FString output = TEXT("");
		if (buildingData.BuildingType == GUILD_B_HERB ||
			buildingData.BuildingType == GUILD_B_WOOD || 
			buildingData.BuildingType == GUILD_B_CRYSTAL || 
			buildingData.BuildingType == GUILD_B_SILVER)
		{
			output = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Keepup")), buildingData.Param1);
		}
		object->SetString(TEXT("guild_building_output"), output);

		GuildBuildingData preBuildingData;
		MMOGuildData::getSingleton().GetGuildBuildingData(buildingData.PreBuildingType, 1, preBuildingData);
		UINT32 preBuildingLv = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel(buildingData.PreBuildingType);

		object->SetString(TEXT("guild_building_pre_building"), preBuildingData.Name);
		buildingIcon = FString::Printf(TEXT("img://UI_GuildTerritoryWidget.%d"), preBuildingData.BuildingType);
		object->SetString(TEXT("guild_building_pre_building_icon"), buildingIcon);
		object->SetInt(TEXT("guild_building_pre_building_level"), preBuildingLv);
		object->SetInt(TEXT("guild_building_pre_building_level_req"), buildingData.PreBuildingLevel);

		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		object->SetString(TEXT("guild_money_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Money")));
		object->SetString(TEXT("guild_money_icon"), TEXT("img://UI_GuildTerritoryWidget.money"));
		object->SetInt(TEXT("guild_money"), baseInfo.Money);
		object->SetInt(TEXT("guild_building_money_req"), buildingData.GuildMoneyReq);
		object->SetString(TEXT("guild_herb_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Herb")));
		object->SetString(TEXT("guild_herb_icon"), TEXT("img://UI_GuildTerritoryWidget.food"));
		object->SetInt(TEXT("guild_herb"), baseInfo.Herb);
		object->SetInt(TEXT("guild_building_herb_req"), buildingData.HerbReq);
		object->SetString(TEXT("guild_wood_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Wood")));
		object->SetString(TEXT("guild_wood_icon"), TEXT("img://UI_GuildTerritoryWidget.wood"));
		object->SetInt(TEXT("guild_wood"), baseInfo.Wood);
		object->SetInt(TEXT("guild_building_wood_req"), buildingData.WoodReq);
		object->SetString(TEXT("guild_crystal_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Crystal")));
		object->SetString(TEXT("guild_crystal_icon"), TEXT("img://UI_GuildTerritoryWidget.crystal"));
		object->SetInt(TEXT("guild_crystal"), baseInfo.Crystal);
		object->SetInt(TEXT("guild_building_crystal_req"), buildingData.CrystalReq);
		object->SetString(TEXT("guild_silver_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Silver")));
		object->SetString(TEXT("guild_silver_icon"), TEXT("img://UI_GuildTerritoryWidget.iron"));
		object->SetInt(TEXT("guild_silver"), baseInfo.Silver);
		object->SetInt(TEXT("guild_building_silver_req"), buildingData.SilverReq);

		eventCallUpdateGuildBuildingData(object);
	}
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIGuildBuildingCreatePanel::ASUpdateGuildBuildingCreateData(INT buildingType)
{
	UpdateGuildBuildingData(buildingType);
}

void UMMOUIGuildBuildingCreatePanel::ASCreateGuildBuilding(INT buildingType)
{
	// queue full
	GuildBuildingUpgradeList guildBuildingUpgradeList = MMOGuildData::getSingleton().GetGuildBuildingUpgradeList();
	if (guildBuildingUpgradeList.size() >= GUILD_MAX_UPGRADE_BUILDING_COUNT)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.BuildingUpgradeListFull")),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeListFull"));
		return;
	}

	GuildBuildingData buildingData;
	MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, 0, buildingData);
	// insufficient resources
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Money < buildingData.GuildMoneyReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Insufficient guild funds"),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeMaxLevel"));
		return;
	}
	if (baseInfo.Herb < buildingData.HerbReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Insufficient grain"),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeMaxLevel"));
		return;
	}
	if (baseInfo.Wood < buildingData.WoodReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Insufficient wood"),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeMaxLevel"));
		return;
	}
	if (baseInfo.Crystal < buildingData.CrystalReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Insufficient stone"),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeMaxLevel"));
		return;
	}
	if (baseInfo.Silver < buildingData.SilverReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Insufficient iron ore"),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeMaxLevel"));
		return;
	}

	// prerequisite building
	UINT32 preBuildingLv = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel(buildingData.PreBuildingType);
	if (preBuildingLv < buildingData.PreBuildingLevel)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Prerequisite building level too low"),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeMaxLevel"));
		return;
	}

	if (_NpcId != 0)
	{
		//unsigned __int64 nameId = 0;
		//strncpy((char*)&nameId, TCHAR_TO_UTF8(*(_GuildBuildingCreatePanel->_NpcNameId)), sizeof(nameId));
		CMessageHandler::GetInstance().Send_CreateGuildBuilding((UINT32)(_NpcId), (UINT8)buildingType);
	}
}