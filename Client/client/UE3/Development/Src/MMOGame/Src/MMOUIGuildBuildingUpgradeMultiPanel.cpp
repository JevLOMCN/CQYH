#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include "MMOMsgBoxManager.h"
#include <time.h>

IMPLEMENT_CLASS(UMMOUIGuildBuildingUpgradeMultiPanel)

void UMMOUIGuildBuildingUpgradeMultiPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildBuildingUpgradeMultiPanel>(this, evts);
}

void UMMOUIGuildBuildingUpgradeMultiPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
	}
}

void UMMOUIGuildBuildingUpgradeMultiPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if (!IsVisible)
	{
		return;
	}

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

void UMMOUIGuildBuildingUpgradeMultiPanel::ShowGuildBuildingUpgradePanel(UINT32 buildingType)
{
	Show(TRUE);
	UpdateGuildBuildingList(buildingType);

	GuildBuildingList guildBuildingList = MMOGuildData::getSingleton().GetGuildBuildingList();
	GuildBuildingList::iterator iter = guildBuildingList.begin();
	for (; iter != guildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingType == buildingType)
		{
			UpdateGuildBuildingData(iter->BuildingId);
			break;
		}
	}
}

void UMMOUIGuildBuildingUpgradeMultiPanel::UpdateGuildBuildingList(UINT32 buildingType)
{
	if (buildingType == GUILD_B_GATE || buildingType == GUILD_B_WALL)
	{
		return;
	}

	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	INT index = 0;
	GuildBuildingList guildBuildingList = MMOGuildData::getSingleton().GetGuildBuildingList();
	GuildBuildingList::iterator iter = guildBuildingList.begin();
	for (; iter != guildBuildingList.end(); ++ iter)
	{
		if (iter->BuildingType != buildingType)
		{
			continue;
		}

		UINT32 buildingLv = iter->BuildingLv;
		GuildBuildingData buildingData;
		if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv, buildingData))
		{
			UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
			if (object)
			{
				object->SetString(TEXT("label"), buildingData.Name);
				object->SetInt(TEXT("index"), index);
				object->SetInt(TEXT("type"), 0);
				object->SetInt(TEXT("deepth"), 0);
				object->SetBool(TEXT("visible"), true);

				object->SetInt(TEXT("guild_building_id_high"), UINT32(iter->BuildingId >> 32));
				object->SetInt(TEXT("guild_building_id_low"), UINT32(iter->BuildingId));
				object->SetInt(TEXT("guild_building_type"), buildingType);
				object->SetString(TEXT("guild_building_name"), buildingData.Name);
				FString buildingIcon = FString::Printf(TEXT("img://UI_GuildTerritoryWidget.%d"), buildingData.BuildingType);
				object->SetString(TEXT("guild_building_icon"), buildingIcon);

				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}

	eventCallUpdateGuildBuildingList(objectArray);
}


void UMMOUIGuildBuildingUpgradeMultiPanel::UpdateGuildBuildingData(UINT64 buildingId)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (!object)
	{
		return;
	}

	UINT32 buildingType = MMOGuildData::getSingleton().GetGuildBuildingType(buildingId);
	UINT32 buildingLv = MMOGuildData::getSingleton().GetGuildBuildingLevel(buildingId);
	GuildBuildingData buildingData;
	if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv, buildingData))
	{
		object->SetInt(TEXT("guild_building_id_high"), UINT32(buildingId >> 32));
		object->SetInt(TEXT("guild_building_id_low"), UINT32(buildingId));
		object->SetInt(TEXT("guild_building_type"), buildingType);
		object->SetString(TEXT("guild_building_name"), buildingData.Name);
		object->SetInt(TEXT("guild_building_level"), buildingLv);

		FString buildingIcon = FString::Printf(TEXT("img://UI_GuildTerritoryWidget.%d"), buildingData.BuildingType);
		object->SetString(TEXT("guild_building_icon"), buildingIcon);
		object->SetString(TEXT("guild_building_desc"), buildingData.Description);

		object->SetInt(TEXT("guild_building_next_level"), buildingLv >= buildingData.MaxLevel ? buildingData.MaxLevel : buildingLv + 1);
		object->SetInt(TEXT("guild_building_top_level"), buildingData.MaxLevel);

		object->SetInt(TEXT("guild_building_update_time"), buildingData.UpgradeTime);

		FString keepup = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Keepup")), buildingData.RoutineReq);
		object->SetString(TEXT("guild_building_keepup"), keepup);

		FString curOutput = TEXT("");
		FString nextOutput = TEXT("");
		if (buildingData.BuildingType == GUILD_B_HERB ||
			buildingData.BuildingType == GUILD_B_WOOD || 
			buildingData.BuildingType == GUILD_B_CRYSTAL || 
			buildingData.BuildingType == GUILD_B_SILVER)
		{
			curOutput = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Keepup")), buildingData.Param1);

			GuildBuildingData nextBuildingData;
			if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv + 1, nextBuildingData))
			{
				nextOutput = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Keepup")), nextBuildingData.Param1);
			}
		}
		object->SetString(TEXT("guild_building_cur_output"), curOutput);
		object->SetString(TEXT("guild_building_next_output"), nextOutput);

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

void UMMOUIGuildBuildingUpgradeMultiPanel::DeleteConfirm(UINT64 id)
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("AgreeDeleteNow");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("RefuseDeleteNow");	

	_BuildingIdHigh = UINT32(id >> 32);
	_BuildingIdLow = UINT32(id);

	FString text = TEXT("Demolish this building? Only a small amount of resources will be refunded");
	DeleteConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, 0, funYes, funNo);
}

void UMMOUIGuildBuildingUpgradeMultiPanel::AgreeDeleteNow(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	UINT64 idHigh = _BuildingIdHigh;
	UINT64 idLow = _BuildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;
	CMessageHandler::GetInstance().Send_DestroyGuildBuilding(G_B_UPGRADE_BY_PANEL, buildingId);

	_BuildingIdHigh = 0;
	_BuildingIdLow = 0;
	DeleteConfirmBox = NULL;
}

void UMMOUIGuildBuildingUpgradeMultiPanel::RefuseDeleteNow(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	_BuildingIdHigh = 0;
	_BuildingIdLow = 0;
	DeleteConfirmBox = NULL;
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIGuildBuildingUpgradeMultiPanel::ASUpdateGuildBuildingUpgradeData(INT buildingIdHigh, INT buildingIdLow)
{
	UINT64 idHigh = buildingIdHigh;
	UINT64 idLow = buildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;
	UpdateGuildBuildingData(buildingId);
}

void UMMOUIGuildBuildingUpgradeMultiPanel::ASDestroyBuildingMulti(int buildingIdHigh, int buildingIdLow)
{
	UINT64 idHigh = buildingIdHigh;
	UINT64 idLow = buildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;

	DeleteConfirm(buildingId);
}

void UMMOUIGuildBuildingUpgradeMultiPanel::ASUpgradeBuildingMulti(INT buildingIdHigh, INT buildingIdLow)
{
	// queue full
	GuildBuildingUpgradeList guildBuildingUpgradeList = MMOGuildData::getSingleton().GetGuildBuildingUpgradeList();
	if (guildBuildingUpgradeList.size() >= GUILD_MAX_UPGRADE_BUILDING_COUNT)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.BuildingUpgradeListFull")),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeListFull"));
		return;
	}

	// level maxed
	UINT64 idHigh = buildingIdHigh;
	UINT64 idLow = buildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;

	UINT32 buildingType = MMOGuildData::getSingleton().GetGuildBuildingType(buildingId);
	UINT32 buildingLv = MMOGuildData::getSingleton().GetGuildBuildingLevel(buildingId);
	GuildBuildingData buildingData;
	MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv, buildingData);
	if (buildingLv >= buildingData.MaxLevel)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.BuildingUpgradeMaxLevel")),MsgInfoType_Table,TEXT("MMOGame.Guild.BuildingUpgradeMaxLevel"));
		return;
	}

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

	CMessageHandler::GetInstance().Send_UpgradeGuildBuilding(G_B_UPGRADE_BY_PANEL, buildingId, buildingType);
}
