//class MMOUIEquipSlotEnhancePanel extends MMOUIBaseSlotContainer
//	native(UI)
//	dependson(TypeDefine)
//	config(UIVisibleMask);
class MMOUIEquipSlotEnhancePanel extends Object
	native(UI)
	dependson(TypeDefine);

enum SlotEnhancedPanelSlotType
{
	ESlotEhsSltTpy_EhsConsume1
};

//
var MMOGFXPlayer GFXPlayer;
var MMOPlayerController ctrl;
var MMOPlayer Player;
//var const int MAX_EnhanceConsume;// = 3;
var OperationItemBagPos _ToEnhanceConsume;

var int _PanelEnhanceClassID;
var int _PanelCurSelSlotID;

var MMOUIBase _ContainerPanel;

var GFxObject _MainPlane;
var GFxObject _ToEnhanceConsumeMc;
var GFxObject _SuccessRateTf;

native function UpdateUIViews();
native function UpdateCurSlotDetail();

native function UpdateSummary();
native function UpdateSummaryUI(GFxObject PathArray);

native function SelectEnhanceSlot( int slotID );
native function ToEnhanceCurSelectedSlot();
native function RealEnhanceCurEquipSlot();

event CallUpdateDetailPanel(GFxObject data)
{
	_ContainerPanel.ActionScriptVoid("UpdateDetailPanel");
}

event CallUpdateSummaryPanel(GFxObject data)
{
	_ContainerPanel.ActionScriptVoid("UpdateSummaryPanel");
}

event CallUpdateTips(GFxObject data)
{
	_ContainerPanel.ActionScriptVoid("UpdateTips");
}

event CallUpdateActiveTips(GFxObject data)
{
	_ContainerPanel.ActionScriptVoid("UpdateActiveTips");
}


//native  function PlayerMoneyChange(EMONEY_TYPE MoneyType, int Money , int OldMoney);
//native  function PlayerLevelChange( int Lv );
native  function BagSlotChange(int bagId,int slotId);

native	function ClearUI();

native  function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native  function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native  function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native  function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

//===============================================================================================================
//===============================================================================================================
//Physique enhancement
native function ASSelectEquipSlot(int slotId);
native function ASShowTips_EquipSlot(int id);
native function ASHideTips_EquipSlot(int id);
native function ASShowTips_ActiveIcon(GFxObject mc);
native function ASEnhanceEquipSlot();

native function EnhanceEquipSlotOK();
native function EnhanceEquipSlotCancel();

native function ASShowTips_EnhanceGuide( GFxObject mc );
native function ASHideTips_EnhanceGuide( GFxObject mc );
//===============================================================================================================
//===============================================================================================================

defaultproperties
{
	//FriendlyName=EquipSlotEnhance
	//MAX_EnhanceConsume=3
	//IsDynamicLoad = true;
}