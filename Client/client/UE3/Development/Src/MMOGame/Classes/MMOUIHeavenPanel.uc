class MMOUIHeavenPanel extends GFxObject
	native(UI)
	config(UIVisibleMask);

var GFxObject ConverterConfirmBox;
var GFxObject pkSoulSlot;

native function ConfirmConverter(GFxObject mc);
native function CancelConverter(GFxObject mc);
//AS - > UC
//Update soul value
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

event CallUpdateSoulValue(String soulValue)
{
	ActionScriptVoid("UCUpdateSoulValue");
}

event CallUpdateXiulianValue(String soulValue)
{
	ActionScriptVoid("UCUpdateXiulianValue");
}

event CallUpdateExpProBar(int curValue, int maxValue)
{
	ActionScriptVoid("UCUpdateExpProBar");
}

event CallUpdateStateLevel(int nHeavenLevel,String sHeavenText)
{
	ActionScriptVoid("UCUpdateStateLevel");
}

event CallUpdateStateLevelStarNum(int nStarNum,String sStarText)
{
	ActionScriptVoid("UCUpdateStateLevelStarNum");
}

event CallUpdateNextConvertInfo(GFxObject nextData)
{
	ActionScriptVoid("UCUpdateNextConverInfo");
}
event CallUpdateHeavenLog(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateHeavenLog");
}
event CallUCPlayMainAnimation(int nType)
{
	ActionScriptVoid("UCPlayMainAnimation");
}
event CallUCSetConvertTimes(string times)
{
	ActionScriptVoid("UCSetConvertTimes");
}

event CallUCUpdateStateAll(string sLevel,string sPluseValue, string sReductionValue)
{
	ActionScriptVoid("UCUpdateStateAll");
}
event CallUCShowMaxLevelWarnText(bool bShow)
{
	ActionScriptVoid("UCShowMaxLevelWarnText");
}
event CallUCSetSoulBottleProgress(int nCur,int nMax)
{
	ActionScriptVoid("UCSetSoulBottleProgress");
}
event CallUCSetSoulBottleHintText(string hint)
{
	ActionScriptVoid("UCSetSoulBottleHintText");
}