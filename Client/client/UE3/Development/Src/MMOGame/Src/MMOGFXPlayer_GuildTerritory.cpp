#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_GuildTerritory)

void UMMOGFXPlayer_GuildTerritory::PostAdvance(FLOAT dt)
{
	if(_GuildTerritoryPanel->IsVisible)
	{
		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if (!selectActor)
		{
			_GuildTerritoryPanel->Show(FALSE);
		}
	}

	if(_GuildWelfarePanel->IsVisible)
	{
		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if (!selectActor)
		{
			_GuildWelfarePanel->Show(FALSE);
		}
	}

	if(_GuildDungeonBeginPanel->IsVisible)
	{
		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if (!selectActor)
		{
			_GuildDungeonBeginPanel->Show(FALSE);
		}
	}
}

void UMMOGFXPlayer_GuildTerritory::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_GuildTerritory::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_GuildTerritory);
	MMOUI::GetUIManager()->SetShowGuildTerritoryPanelDelegate(FScriptDelegate(this, TEXT("ShowGuildTerritoryPanel")));

}

void UMMOGFXPlayer_GuildTerritory::UnInit()
{
	Super::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);
	MMOUI::GetUIManager()->SetShowGuildTerritoryPanelDelegate(nullDelgate);
}

void UMMOGFXPlayer_GuildTerritory::ShowGuildTerritoryPanel()
{
	_GuildTerritoryPanel->ShowGuildTerritoryPanel();
}

void UMMOGFXPlayer_GuildTerritory::ASUpdateGuildBuildingCreateData(INT buildingType)
{
	_GuildBuildingCreatePanel->UpdateGuildBuildingData(buildingType);
}

void UMMOGFXPlayer_GuildTerritory::ASCreateGuildBuilding(INT buildingType)
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

	if (_GuildBuildingCreatePanel->_NpcId != 0)
	{
		//unsigned __int64 nameId = 0;
		//strncpy((char*)&nameId, TCHAR_TO_UTF8(*(_GuildBuildingCreatePanel->_NpcNameId)), sizeof(nameId));
		CMessageHandler::GetInstance().Send_CreateGuildBuilding((UINT32)(_GuildBuildingCreatePanel->_NpcId), (UINT8)buildingType);
	}
}
// resource donation
void UMMOGFXPlayer_GuildTerritory::ASContribute(INT type, INT count)
{
	if (count == 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Donation amount cannot be 0"),MsgInfoType_Table,TEXT("Donation amount cannot be 0"));
		return;
	}

	// determine whether resources have reached the limit
	INT realType =  GUILD_ATTR_HERB + type;
	UINT32 resMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(realType);
	UINT32 resCur = MMOGuildData::getSingleton().GetCurGuildTerritoryRes(realType);
	if (resCur >= resMax)
	{
		FString text = TEXT("");
		FString textString = TEXT("");
		switch (realType)
		{
		case GUILD_ATTR_WOOD:
			text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.WoodStoreLimit"));
			textString =TEXT("MMOGame.Guild.WoodStoreLimit");
			break;
		case GUILD_ATTR_HERB:
			text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.FoodStoreLimit"));
			textString =TEXT("MMOGame.Guild.FoodStoreLimit");
			break;
		case GUILD_ATTR_CRYSTAL:
			text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.StoneStoreLimit"));
			textString =TEXT("MMOGame.Guild.StoneStoreLimit");
			break;
		case GUILD_ATTR_SILVER:
			text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.IronStoreLimit"));
			textString =TEXT("MMOGame.Guild.IronStoreLimit");
			break;
		}
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,text);
		return;
	}

	// #15422
	if (resCur + count > resMax)
	{
		FString text = TEXT("Donation amount too large, cannot donate resources");		
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.MoneyStoreLimit"));
		return;
	}

	CMessageHandler::GetInstance().Send_GuildContribute(realType, count);
}

void UMMOGFXPlayer_GuildTerritory::ASCancelUpgradeBuilding(INT buildingIdHigh, INT buildingIdLow)
{
	UINT64 idHigh = buildingIdHigh;
	UINT64 idLow = buildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;
	_GuildTerritoryPanel->CancelUpgradeBuildingConfirm(buildingId);
}

void UMMOGFXPlayer_GuildTerritory::ASCancelUpgradeWelfare(INT id)
{
	_GuildTerritoryPanel->CancelUpgradeWelfareConfirm(id);
}

void UMMOGFXPlayer_GuildTerritory::ASShowBuildingUpgradePanel(INT buildingType)
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
		_GuildBuildingUpgradeSinglePanel->ShowGuildBuildingUpgradePanel(buildingType);
	}
	else
	{
		_GuildBuildingUpgradeMultiPanel->ShowGuildBuildingUpgradePanel(buildingType);
	}
}

void UMMOGFXPlayer_GuildTerritory::ASUpdateGuildBuildingUpgradeData(INT buildingIdHigh, INT buildingIdLow)
{
	UINT64 idHigh = buildingIdHigh;
	UINT64 idLow = buildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;
	_GuildBuildingUpgradeMultiPanel->UpdateGuildBuildingData(buildingId);
}

void UMMOGFXPlayer_GuildTerritory::ASDestroyBuilding(int buildingIdHigh, int buildingIdLow, int npcId)
{
	UINT64 idHigh = buildingIdHigh;
	UINT64 idLow = buildingIdLow;
	UINT64 buildingId = (idHigh << 32) | idLow;

	if (_GuildBuildingUpgradeSinglePanel->IsVisible)
	{
		_GuildBuildingUpgradeSinglePanel->DeleteConfirm(buildingId, npcId);
	}
	else if (_GuildBuildingUpgradeMultiPanel->IsVisible)
	{
		_GuildBuildingUpgradeMultiPanel->DeleteConfirm(buildingId);
	}
}

void UMMOGFXPlayer_GuildTerritory::ASUpgradeBuilding(int buildingIdHigh, int buildingIdLow, int npcId)
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
	
	if (npcId != 0)
	{
		CMessageHandler::GetInstance().Send_UpgradeGuildBuilding(G_B_UPGRADE_BY_NPC, npcId, buildingType);
	}
	else
	{
		CMessageHandler::GetInstance().Send_UpgradeGuildBuilding(G_B_UPGRADE_BY_PANEL, buildingId, buildingType); 
	}
}

void UMMOGFXPlayer_GuildTerritory::ASWelfarePageUp()
{
	_GuildWelfarePanel->PageUp();
}

void UMMOGFXPlayer_GuildTerritory::ASWelfarePageDown()
{
	_GuildWelfarePanel->PageDown();
}

void UMMOGFXPlayer_GuildTerritory::ASShowWelfareUpgradePanel(INT id)
{
	UMMOUIGuildWelfareUpgradePanel* panel = (UMMOUIGuildWelfareUpgradePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WelfareUpgrade);
	if (panel)
	{
		panel->ShowGuildWelfareUpgradePanel(id);
	}
}

void UMMOGFXPlayer_GuildTerritory::ASUpgradeWelfare(INT id)
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

void UMMOGFXPlayer_GuildTerritory::ASPurchaseWelfare(INT id)
{
	CMessageHandler::GetInstance().Send_PurchaseGuildWelfare(id);
}

void UMMOGFXPlayer_GuildTerritory::ASChangeWeather(INT weatherType, INT durationType)
{
	CMessageHandler::GetInstance().Send_ChangeWeather(weatherType, durationType); 
}

void UMMOGFXPlayer_GuildTerritory::ASOpenDungeon(INT dungeonId, INT leaderId)
{
	CMessageHandler::GetInstance().Send_OpenGuildDungeon(dungeonId, leaderId); 
}

void UMMOGFXPlayer_GuildTerritory::ASEnterDungeon(INT dungeonId)
{
	CMessageHandler::GetInstance().Send_EnterGuildDungeon(dungeonId);
}

void UMMOGFXPlayer_GuildTerritory::ASLeaveGuildDungeon()
{
	CMessageHandler::GetInstance().Send_LeaveScene(); 
}