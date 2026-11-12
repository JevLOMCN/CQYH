class MMOGFXPlayer_GuildTerritory extends MMOGFXPlayer
	native(UI);

var MMOUIGuildTerritoryPanel _GuildTerritoryPanel;
var MMOUIGuildWelfarePanel _GuildWelfarePanel;
var MMOUIGuildDungeonBeginPanel _GuildDungeonBeginPanel;
var MMOUIGuildDungeonEndPanel _GuildDungeonEndPanel;
var MMOUIGuildWelfareUpgradePanel _GuildWelfareUpgradePanel;
var MMOUIGuildBuildingCreatePanel _GuildBuildingCreatePanel;
var MMOUIGuildBuildingUpgradeMultiPanel _GuildBuildingUpgradeMultiPanel;
var MMOUIGuildBuildingUpgradeSinglePanel _GuildBuildingUpgradeSinglePanel;
var MMOUIGuildDungeonEnterPanel _GuildDungeonEnterPanel;
var MMOUIGuildDungeonExitPanel _GuildDungeonExitPanel;

function ASRegisterGuildTerritoryPanel(MMOUIGuildTerritoryPanel mc)
{ 
	_GuildTerritoryPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_Territory);
}

function ASRegisterGuildWelfarePanel(MMOUIGuildWelfarePanel mc)
{ 
	_GuildWelfarePanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_Welfare);
}

function ASRegisterGuildDungeonBeginPanel(MMOUIGuildDungeonBeginPanel mc)
{ 
	_GuildDungeonBeginPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_DungeonBegin);
}

function ASRegisterGuildDungeonEndPanel(MMOUIGuildDungeonEndPanel mc)
{ 
	_GuildDungeonEndPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_DungeonEnd);
}

function ASRegisterGuildWelfareUpgradePanel(MMOUIGuildWelfareUpgradePanel mc)
{ 
	_GuildWelfareUpgradePanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_WelfareUpgrade);
}

function ASRegisterGuildBuildingCreatePanel(MMOUIGuildBuildingCreatePanel mc)
{ 
	_GuildBuildingCreatePanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_BuildingCreate);
}

function ASRegisterGuildBuildingUpgradeMultiPanel(MMOUIGuildBuildingUpgradeMultiPanel mc)
{ 
	_GuildBuildingUpgradeMultiPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_BuildingUpgradeMulti);
}

function ASRegisterGuildBuildingUpgradeSinglePanel(MMOUIGuildBuildingUpgradeSinglePanel mc)
{ 
	_GuildBuildingUpgradeSinglePanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_BuildingUpgradeSingle);
}

function ASRegisterGuildDungeonEnterPanel(MMOUIGuildDungeonEnterPanel mc)
{ 
	_GuildDungeonEnterPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_DungeonEnter);
}

function ASRegisterGuildDungeonExitPanel(MMOUIGuildDungeonExitPanel mc)
{ 
	_GuildDungeonExitPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_DungeonExit);
}

native function PostAdvance( float dt );

native final function ShowGuildTerritoryPanel();
// Resource donation
native final function ASContribute(int type, int count);

// Guild territory
native final function ASCancelUpgradeBuilding(int buildingIdHigh, int buildingIdLow);
native final function ASCancelUpgradeWelfare(int id);

native final function ASUpdateGuildBuildingCreateData(int buildingType);
native final function ASCreateGuildBuilding(int buildingType);

native final function ASShowBuildingUpgradePanel(int buildingType);
native final function ASUpdateGuildBuildingUpgradeData(int buildingIdHigh, int buildingIdLow);
native final function ASUpgradeBuilding(int buildingIdHigh, int buildingIdLow, int npcId);
native final function ASDestroyBuilding(int buildingIdHigh, int buildingIdLow, int npcId);

native final function ASShowWelfareUpgradePanel(int id);
native final function ASUpgradeWelfare(int id);

// Turn page
native final function ASWelfarePageUp();
native final function ASWelfarePageDown();
native final function ASPurchaseWelfare(int id);
native final function ASChangeWeather(int weatherType, int durationType);

// Guild dungeon
native final function ASOpenDungeon(int dungeonId, int leaderId);
native final function ASEnterDungeon(int dungeonId);
native final function ASLeaveGuildDungeon();

DefaultProperties
{
	MovieInfoStr="UI_GuildTerritoryWidget.GuildTerritoryWidget"
	Priority = 5
}
