#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include <time.h>


IMPLEMENT_CLASS(UMMOUIGuildWelfareUpgradePanel)

void UMMOUIGuildWelfareUpgradePanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildWelfareUpgradePanel>(this, evts);
}

void UMMOUIGuildWelfareUpgradePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
	}
}

void UMMOUIGuildWelfareUpgradePanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
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

void UMMOUIGuildWelfareUpgradePanel::ShowGuildWelfareUpgradePanel(UINT32 welfareId)
{
	Show(TRUE);
	UpdateGuildWelfareData(welfareId);
}

void UMMOUIGuildWelfareUpgradePanel::UpdateGuildWelfareData(UINT32 welfareId)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (!object)
	{
		return;
	}

	UINT32 welfareLv = MMOGuildData::getSingleton().GetGuildWelfareLevel(welfareId);
	GuildWelfareData welfareData;
	if (!MMOGuildData::getSingleton().GetGuildWelfareData(welfareId, welfareLv, welfareData))
	{
		return;
	}

	UBuff* buff = MMOFacade::getSingleton().FindBuffTemplate(welfareData.BuffId, 0);
	if (!buff)
	{
		return;
	}

	object->SetString(TEXT("guild_welfare_name"), buff->BuffName.GetNameString());
	object->SetInt(TEXT("guild_welfare_level"), welfareLv);
	object->SetInt(TEXT("guild_welfare_id"), welfareId);

	FString icon = FString(TEXT("img://")) + buff->GetBuffIconName();
	object->SetString(TEXT("guild_welfare_icon"), icon);
	object->SetString(TEXT("guild_welfare_desc"), buff->BuffDescription);
	object->SetInt(TEXT("guild_welfare_level"), welfareLv);
	UINT32 nextLevel = welfareLv + 1 > welfareData.MaxLevel ? welfareData.MaxLevel : welfareLv + 1;
	object->SetInt(TEXT("guild_welfare_next_level"), nextLevel);
	object->SetInt(TEXT("guild_welfare_top_level"), welfareData.MaxLevel);

	object->SetInt(TEXT("guild_welfare_up_time"), welfareData.UpgradeTime);

	GuildBuildingData preBuildingData;
	MMOGuildData::getSingleton().GetGuildBuildingData(welfareData.UpgradePreBuildingType, 1, preBuildingData);
	object->SetString(TEXT("guild_welfare_pre_building"), preBuildingData.Name);
	object->SetString(TEXT("guild_welfare_pre_building_icon"), TEXT("img://UI_GuildTerritoryWidget.building4"));
	UINT32 buildingLv = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel(welfareData.UpgradePreBuildingType);
	object->SetInt(TEXT("guild_welfare_pre_building_level"), buildingLv);
	object->SetInt(TEXT("guild_welfare_pre_building_level_req"), welfareData.UpgradePreBuildingLevel);

	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	object->SetString(TEXT("guild_money_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Money")));
	object->SetString(TEXT("guild_money_icon"), TEXT("img://UI_GuildTerritoryWidget.building4"));
	object->SetInt(TEXT("guild_money"), baseInfo.Money);
	object->SetInt(TEXT("guild_welfare_money_req"), welfareData.UpgradeGuildMoneyReq);
	object->SetString(TEXT("guild_herb_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Herb")));
	object->SetString(TEXT("guild_herb_icon"), TEXT("img://UI_GuildTerritoryWidget.building4"));
	object->SetInt(TEXT("guild_herb"), baseInfo.Herb);
	object->SetInt(TEXT("guild_welfare_herb_req"), welfareData.UpgradeHerbReq);
	object->SetString(TEXT("guild_wood_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Wood")));
	object->SetString(TEXT("guild_wood_icon"), TEXT("img://UI_GuildTerritoryWidget.building4"));
	object->SetInt(TEXT("guild_wood"), baseInfo.Wood);
	object->SetInt(TEXT("guild_welfare_wood_req"), welfareData.UpgradeWoodReq);
	object->SetString(TEXT("guild_crystal_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Crystal")));
	object->SetString(TEXT("guild_crystal_icon"), TEXT("img://UI_GuildTerritoryWidget.building4"));
	object->SetInt(TEXT("guild_crystal"), baseInfo.Crystal);
	object->SetInt(TEXT("guild_welfare_crystal_req"), welfareData.UpgradeCrystalReq);
	object->SetString(TEXT("guild_silver_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Silver")));
	object->SetString(TEXT("guild_silver_icon"), TEXT("img://UI_GuildTerritoryWidget.building4"));
	object->SetInt(TEXT("guild_silver"), baseInfo.Silver);
	object->SetInt(TEXT("guild_welfare_silver_req"), welfareData.UpgradeSliverReq);

	eventCallUpdateGuildWelfareData(object);
}
//////////////////////////////////////////////////////////////////////////

void UMMOUIGuildWelfareUpgradePanel::ASUpgradeWelfare(INT id)
{
	// queue full
	GuildWelfareUpgradeList guildWelfareUpgradeList = MMOGuildData::getSingleton().GetGuildWelfareUpgradeList();
	if (guildWelfareUpgradeList.size() >= GUILD_MAX_UPGRADE_AURA_COUNT)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.WelfareUpgradeListFull")),MsgInfoType_Table,TEXT("MMOGame.Guild.WelfareUpgradeListFull"));
		return;
	}

	// level maxed
	GuildWelfareData welfareData;
	MMOGuildData::getSingleton().GetGuildWelfareData(id, 0, welfareData);
	UINT32 welfareLv = MMOGuildData::getSingleton().GetGuildWelfareLevel(id);
	if (welfareLv >= welfareData.MaxLevel)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.WelfareUpgradeMaxLevel")),MsgInfoType_Table,TEXT("MMOGame.Guild.WelfareUpgradeMaxLevel"));
		return;
	}

	CMessageHandler::GetInstance().Send_UpgradeGuildWelfare(id); 
}

