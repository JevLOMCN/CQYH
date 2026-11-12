#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include <time.h>
#include "MMOMsgBoxManager.h"


IMPLEMENT_CLASS(UMMOUIGuildTerritoryPanel)

void UMMOUIGuildTerritoryPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildTerritoryPanel>(this, evts);
}

void UMMOUIGuildTerritoryPanel::ShowGuildTerritoryPanel()
{
	ChangeVisible();
}

void UMMOUIGuildTerritoryPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
		_CurPageIndex = 1;

		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TerriPanel")));
		UpdateGuildTerritoryInfo();
		UpdateGuildTerritoryUpgradeList();
		UpdateGuildBuildingList();
//		UpdateGuildWelfarePage();
	}
}

void UMMOUIGuildTerritoryPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
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
	if (guildEvent == NULL)
	{
		return;
	}

	switch(guildEvent->_EventType)
	{
	case MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo:
		UpdateGuildTerritoryInfo();
		break;

	case MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList:
		UpdateGuildTerritoryUpgradeList();
		break;

	case MMOGameEvent_Guild::EGET_UpdateGuildBuildingList:
		UpdateGuildBuildingList();
		break;

	case MMOGameEvent_Guild::EGET_UpdateGuildWelfareList:
		UpdateGuildWelfareList();
		break;
	}
}

void UMMOUIGuildTerritoryPanel::UpdateGuildTerritoryInfo()
{
	UINT32 moneyMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(GUILD_ATTR_MONEY);
	UINT32 herbMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(GUILD_ATTR_HERB);
	UINT32 woodMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(GUILD_ATTR_WOOD);
	UINT32 crystalMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(GUILD_ATTR_CRYSTAL);
	UINT32 silverMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(GUILD_ATTR_SILVER);
	
	// Upkeep cost cap
	UINT32 keepupMax = 0;
	GuildBuildingList guildBuildingList = MMOGuildData::getSingleton().GetGuildBuildingList();
	GuildBuildingList::iterator iter = guildBuildingList.begin();
	for (; iter != guildBuildingList.end(); ++ iter)
	{
		GuildBuildingData buildingData;
		if (!MMOGuildData::getSingleton().GetGuildBuildingData(iter->BuildingType, iter->BuildingLv, buildingData))
		{
			continue;
		}
		
		keepupMax += buildingData.RoutineReq;
	}

	UGFxObject* object = GFXPlayer->CreateObject("Object");
	if (object)
	{
		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		object->SetInt(TEXT("guild_id"), baseInfo.Id);
		object->SetString(TEXT("guild_name"), baseInfo.Name);
		object->SetInt(TEXT("guild_level"), baseInfo.level);
		object->SetInt(TEXT("guild_prosperity"), baseInfo.Prosperity);

		object->SetString(TEXT("guild_money_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Money")));
		object->SetString(TEXT("guild_money_icon"), TEXT("img://UI_GuildTerritoryWidget.money"));
		object->SetInt(TEXT("guild_money"), baseInfo.Money);
		object->SetInt(TEXT("guild_money_max"), moneyMax);
		object->SetString(TEXT("guild_herb_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Herb")));
		object->SetString(TEXT("guild_herb_icon"), TEXT("img://UI_GuildTerritoryWidget.food"));
		object->SetInt(TEXT("guild_herb"), baseInfo.Herb);
		object->SetInt(TEXT("guild_herb_max"), herbMax);
		object->SetString(TEXT("guild_wood_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Wood")));
		object->SetString(TEXT("guild_wood_icon"), TEXT("img://UI_GuildTerritoryWidget.wood"));
		object->SetInt(TEXT("guild_wood"), baseInfo.Wood);
		object->SetInt(TEXT("guild_wood_max"), woodMax);
		object->SetString(TEXT("guild_crystal_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Crystal")));
		object->SetString(TEXT("guild_crystal_icon"), TEXT("img://UI_GuildTerritoryWidget.crystal"));
		object->SetInt(TEXT("guild_crystal"), baseInfo.Crystal);
		object->SetInt(TEXT("guild_crystal_max"), crystalMax);
		object->SetString(TEXT("guild_silver_name"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Silver")));
		object->SetString(TEXT("guild_silver_icon"), TEXT("img://UI_GuildTerritoryWidget.iron"));
		object->SetInt(TEXT("guild_silver"), baseInfo.Silver);
		object->SetInt(TEXT("guild_silver_max"), silverMax);
		/*
		FString contribute = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Food"));
		contribute += TEXT("=");
		contribute += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Wood"));
		contribute += TEXT("=");
		contribute += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Stone"));
		contribute += TEXT("=");
		contribute += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Iron"));
		object->SetString(TEXT("guild_contribute_type"), contribute);
		*/
		FString keepup = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Keepup")), keepupMax);
		object->SetString(TEXT("guild_keepup"), keepup);
	}

	eventCallUpdateGuildTerritoryInfo(object);
}

void UMMOUIGuildTerritoryPanel::UpdateGuildTerritoryUpgradeList()
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	// Building upgrade
	GuildBuildingUpgradeList guildBuildingUpgradeList = MMOGuildData::getSingleton().GetGuildBuildingUpgradeList();
	GuildBuildingUpgradeList::iterator iterBuilding = guildBuildingUpgradeList.begin();
	INT index = 0;
	for (; iterBuilding != guildBuildingUpgradeList.end(); ++ iterBuilding)
	{
		UINT32 buildingType = iterBuilding->BuildingType;
		UINT32 buildingLv = iterBuilding->BuildingLv;
		GuildBuildingData buildingData;
		if (!MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv, buildingData))
		{
			continue;
		}
		
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetString(TEXT("label"), buildingData.Name);
			object->SetInt(TEXT("index"), 0);
			object->SetInt(TEXT("type"), 0);
			object->SetInt(TEXT("deepth"), 0);
			object->SetBool(TEXT("visible"), true);

			object->SetInt(TEXT("guild_upgrade_type"), GTT_BUILDING);
			object->SetString(TEXT("guild_upgrade_type_str"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeTypeBuilding")));
			object->SetInt(TEXT("guild_upgrade_id_high"), UINT32(iterBuilding->BuildingId >> 32));
			object->SetInt(TEXT("guild_upgrade_id_low"), UINT32(iterBuilding->BuildingId));
			object->SetString(TEXT("guild_upgrade_name"), buildingData.Name);
			object->SetInt(TEXT("guild_upgrade_level"), buildingLv);
			UINT32 targetLv = (buildingLv + 1) > buildingData.MaxLevel ? buildingData.MaxLevel : (buildingLv + 1);
			object->SetInt(TEXT("guild_upgrade_target_level"), targetLv);
			object->SetInt(TEXT("guild_upgrade_left_time"), iterBuilding->UpgradeLeftTime);
			FString buildingIcon = FString::Printf(TEXT("img://UI_GuildTerritoryWidget.%d"), buildingData.BuildingType);
			object->SetString(TEXT("guild_upgrade_icon"), buildingIcon);

			__time32_t curTime;
			_time32(&curTime);
			struct tm *today = _localtime32(&curTime); 

			__time32_t localTime = iterBuilding->UpgradeEndTime;
			struct tm *time = _localtime32(&localTime); 
			
			// If it is today
			FString finishTime = TEXT("");

			if (today && time)
			{
				if (time->tm_year == today->tm_year && time->tm_mon == today->tm_mon && time->tm_mday == today->tm_mday)
				{
					finishTime = FString::Printf(TEXT("%02d:%02d:%02d"), 
						time->tm_hour, 
						time->tm_min, 
						time->tm_sec);
				}
				else
				{
					finishTime = FString::Printf(TEXT("%d-%d %02d:%02d:%02d"), 
						time->tm_mon + 1, 
						time->tm_mday,
						time->tm_hour, 
						time->tm_min, 
						time->tm_sec);
				}
			}
			object->SetString(TEXT("guild_upgrade_finish_time"), finishTime);

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	// Welfare upgrade
	GuildWelfareUpgradeList guildWelfareUpgradeList = MMOGuildData::getSingleton().GetGuildWelfareUpgradeList();
	GuildWelfareUpgradeList::iterator iterWelfare = guildWelfareUpgradeList.begin();
	for (; iterWelfare != guildWelfareUpgradeList.end(); ++ iterWelfare)
	{
		UINT32 welfareId = iterWelfare->WelfareId;
		UINT32 welfareLv = iterWelfare->WelfareLv;
		GuildWelfareData welfareData;
		if (!MMOGuildData::getSingleton().GetGuildWelfareData(welfareId, welfareLv, welfareData))
		{
			continue;
		}

		UBuff* buff = MMOFacade::getSingleton().FindBuffTemplate(welfareData.BuffId, 0);
		if (!buff)
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetString(TEXT("label"), buff->BuffName.GetNameString());
			object->SetInt(TEXT("index"), 0);
			object->SetInt(TEXT("type"), 0);
			object->SetInt(TEXT("deepth"), 0);
			object->SetBool(TEXT("visible"), true);

			object->SetInt(TEXT("guild_upgrade_type"), GTT_WELFARE);
			object->SetString(TEXT("guild_upgrade_type_str"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeTypeWelfare")));
			object->SetInt(TEXT("guild_upgrade_id_high"), 0);
			object->SetInt(TEXT("guild_upgrade_id_low"), welfareData.WelfareId);
			object->SetString(TEXT("guild_upgrade_name"), buff->BuffName.GetNameString());
			object->SetInt(TEXT("guild_upgrade_level"), welfareLv);
			UINT32 targetLv = (welfareLv + 1) > welfareData.MaxLevel ? welfareData.MaxLevel : (welfareLv + 1);
			object->SetInt(TEXT("guild_upgrade_target_level"), targetLv);
			object->SetInt(TEXT("guild_upgrade_left_time"), iterWelfare->UpgradeLeftTime);
			object->SetString(TEXT("guild_upgrade_icon"), TEXT("img://UI_GuildTerritoryWidget.building4"));

			__time32_t curTime;
			_time32(&curTime);
			struct tm *today = _localtime32(&curTime); 

			__time32_t localTime = iterWelfare->UpgradeEndTime;
			struct tm *time = _localtime32(&localTime); 

			// If it is today
			FString finishTime = TEXT("");
			if (today && time)
			{
				if (time->tm_year == today->tm_year && time->tm_mon == today->tm_mon && time->tm_mday == today->tm_mday)
				{
					finishTime = FString::Printf(TEXT("%02d:%02d:%02d"), 
						time->tm_hour, 
						time->tm_min, 
						time->tm_sec);
				}
				else
				{
					finishTime = FString::Printf(TEXT("%d-%d %02d:%02d:%02d"), 
						time->tm_mon + 1, 
						time->tm_mday,
						time->tm_hour, 
						time->tm_min, 
						time->tm_sec);
				}
			}
			object->SetString(TEXT("guild_upgrade_finish_time"), finishTime);

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}
	eventCallUpdateGuildTerritoryUpgradeList(objectArray);
}

void UMMOUIGuildTerritoryPanel::UpdateGuildBuildingList()
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
		UINT32 buildingLv = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel(buildingType);
		GuildBuildingData buildingData;
		if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv > 0 ? buildingLv : 1, buildingData))
		{
			UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
			if (object)
			{
				object->SetInt(TEXT("guild_building_type"), buildingType);
				object->SetString(TEXT("guild_building_name"), buildingData.Name);
				object->SetInt(TEXT("guild_building_level"), buildingLv);
				FString buildingIcon = FString::Printf(TEXT("img://UI_GuildTerritoryWidget.%d"), i);
				object->SetString(TEXT("guild_building_icon"), buildingIcon);

				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}

	eventCallUpdateGuildBuildingList(objectArray);
}

void UMMOUIGuildTerritoryPanel::PageUp()
{
	if (_CurPageIndex > 1)
	{
		_CurPageIndex --;
		UpdateGuildWelfarePage();
	}
}

void UMMOUIGuildTerritoryPanel::PageDown()
{
	UINT32 groupCount = MMOGuildData::getSingleton().GetGuildWelfareGroupCount();
	INT maxPage = appCeil((FLOAT)groupCount / (FLOAT)GUILD_WELFARE_GROUP_PER_PAGE);
	if (_CurPageIndex < maxPage)
	{
		_CurPageIndex ++;
		UpdateGuildWelfarePage();
	}
}

void UMMOUIGuildTerritoryPanel::UpdateGuildWelfarePage()
{
	UpdateGuildWelfareList();

	UINT32 groupCount = MMOGuildData::getSingleton().GetGuildWelfareGroupCount();
	INT maxPage = appCeil((FLOAT)groupCount / (FLOAT)GUILD_WELFARE_GROUP_PER_PAGE);
//	eventCallSetGuildWelfarePage(_CurPageIndex, maxPage);
}

void UMMOUIGuildTerritoryPanel::UpdateGuildWelfareList()
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	int index = 0;
	GuildWelfareInfoList guildWelfareInfoList = MMOGuildData::getSingleton().GetGuildWelfareList();
	GuildWelfareInfoList::iterator iter = guildWelfareInfoList.begin();
	for (; iter != guildWelfareInfoList.end(); ++ iter)
	{
		UINT32 welfareId = iter->WelfareId;
		UINT32 welfareLv = iter->WelfareLv;

		GuildWelfareData welfareData;
		if (!MMOGuildData::getSingleton().GetGuildWelfareData(welfareId, welfareLv, welfareData))
		{
			continue;
		}

		UBuff* buff = MMOFacade::getSingleton().FindBuffTemplate(welfareData.BuffId, 0);
		if (!buff)
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetInt(TEXT("guild_wel_id"), welfareData.WelfareId);
			object->SetInt(TEXT("guild_wel_buff"), welfareData.BuffId);
			object->SetString(TEXT("guild_wel_name"), buff->BuffName.GetNameString());
			object->SetString(TEXT("guild_wel_icon_name"), buff->GetBuffIconName());
			object->SetInt(TEXT("guild_wel_level"), welfareLv);
			object->SetInt(TEXT("guild_wel_max_level"), welfareData.MaxLevel);

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateGuildWelfareList(objectArray);
}

void UMMOUIGuildTerritoryPanel::CancelUpgradeBuildingConfirm(UINT64 id)
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("AgreeCancelUpgradeBuilding");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("RefuseCancelUpgradeBuilding");	

	_BuildingIdHigh = UINT32(id >> 32);
	_BuildingIdLow = UINT32(id);

	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.CancelUpgradeBuilding"));
	CancelUpgradeBuildingConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, 0, funYes, funNo);
}

void UMMOUIGuildTerritoryPanel::CancelUpgradeWelfareConfirm(INT welfareId)
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("AgreeCancelUpgradeWelfare");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("RefuseCancelUpgradeWelfare");	

	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.CancelUpgradeWelfare"));
	CancelUpgradeWelfareConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, welfareId, funYes, funNo);
}

void UMMOUIGuildTerritoryPanel::AgreeCancelUpgradeBuilding(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	UINT64 idHigh = _BuildingIdHigh;
	UINT64 idLow = _BuildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;
	INT8 buildingType = (INT8)(MMOGuildData::getSingleton().GetGuildUpgradeBuildingType(buildingId));
	CMessageHandler::GetInstance().Send_CancelGuildBuildingUpgrade(buildingType, buildingId);

	_BuildingIdHigh = 0;
	_BuildingIdLow = 0;
	CancelUpgradeBuildingConfirmBox = NULL;
}

void UMMOUIGuildTerritoryPanel::RefuseCancelUpgradeBuilding(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	_BuildingIdHigh = 0;
	_BuildingIdLow = 0;
	CancelUpgradeBuildingConfirmBox = NULL;
}

void UMMOUIGuildTerritoryPanel::AgreeCancelUpgradeWelfare(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	INT id = mc->GetInt(TEXT("iteminfoid"));
	CMessageHandler::GetInstance().Send_CancelGuildWelfareUpgrade(id);
	CancelUpgradeWelfareConfirmBox = NULL;
}

void UMMOUIGuildTerritoryPanel::RefuseCancelUpgradeWelfare(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	CancelUpgradeWelfareConfirmBox = NULL;
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIGuildTerritoryPanel::ASCancelUpgradeBuilding(INT buildingIdHigh, INT buildingIdLow)
{
	UINT64 idHigh = buildingIdHigh;
	UINT64 idLow = buildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;
	CancelUpgradeBuildingConfirm(buildingId);
}

void UMMOUIGuildTerritoryPanel::ASCancelUpgradeWelfare(INT id)
{
	CancelUpgradeWelfareConfirm(id);
}

void UMMOUIGuildTerritoryPanel::ASShowBuildingUpgradePanel(INT buildingType)
{
	UINT32 buildingCount = MMOGuildData::getSingleton().GetGuildBuildingCount(buildingType);
	if (buildingCount == 0)
	{
		FString text = TEXT("Build this building on a foundation first before upgrading");
		FString textString = TEXT("MMOGame.Guild.IronStoreLimit");
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, textString);
		return;
	}
	else if (buildingCount == 1)
	{
		UMMOUIGuildBuildingUpgradeSinglePanel* panel = (UMMOUIGuildBuildingUpgradeSinglePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_BuildingUpgradeSingle);
		if (panel)
		{
			panel->ShowGuildBuildingUpgradePanel(buildingType);
		}
	}
	else
	{
		UMMOUIGuildBuildingUpgradeMultiPanel* panel = (UMMOUIGuildBuildingUpgradeMultiPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_BuildingUpgradeMulti);
		if (panel)
		{
			panel->ShowGuildBuildingUpgradePanel(buildingType);
		}
	}
}

void UMMOUIGuildTerritoryPanel::ASShowWelfareUpgradePanel(INT id)
{
	UMMOUIGuildWelfareUpgradePanel* panel = (UMMOUIGuildWelfareUpgradePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WelfareUpgrade);
	if (panel)
	{
		panel->ShowGuildWelfareUpgradePanel(id);
	}
}
