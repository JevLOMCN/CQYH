class MMOUIGuildTerritoryPanel extends MMOUIBase
	config(MMOGame)
	native(UI);

var int _CurPageIndex;

var int _BuildingIdHigh;
var int _BuildingIdLow;

var GFxObject CancelUpgradeBuildingConfirmBox;
var GFxObject CancelUpgradeWelfareConfirmBox;

native function AgreeCancelUpgradeBuilding(GFxObject mc);
native function RefuseCancelUpgradeBuilding(GFxObject mc);

native function AgreeCancelUpgradeWelfare(GFxObject mc);
native function RefuseCancelUpgradeWelfare(GFxObject mc);

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

// UC->AS
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("UCSetWindowTitle");
}

// Guild territory
native final function ASCancelUpgradeBuilding(int buildingIdHigh, int buildingIdLow);
native final function ASCancelUpgradeWelfare(int id);
native final function ASShowBuildingUpgradePanel(int buildingType);
native final function ASShowWelfareUpgradePanel(int id);

event CallUpdateGuildTerritoryInfo(GFxObject object)
{
	ActionScriptVoid("UCUpdateGuildTerritoryInfo");
}

event CallUpdateGuildTerritoryUpgradeList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildTerritoryUpgradeList");
}

event CallUpdateGuildBuildingList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildBuildingList");
}

event CallUpdateGuildWelfareList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildWelfareList");
}

DefaultProperties
{
	IsDynamicLoad=true
	DefaultVisible=false
}
