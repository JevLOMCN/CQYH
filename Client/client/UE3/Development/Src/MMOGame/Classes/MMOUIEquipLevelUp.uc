class MMOUIEquipLevelUp extends MMOUIBaseSlotContainer
	native(UI)
	dependson(TypeDefine)
	config(UIVisibleMask);

enum EquipLvUpPanelSlotType
{
	EEpLvUpPnTp_Invalid,
	EEpLvUpPnTp_ToOp,
	EEpLvUpPnTp_HelpItem1,
	EEpLvUpPnTp_HelpItem2,
	EEpLvUpPnTp_HelpItem3,
	EEpLvUpPnTp_HelpItem4,
	EEpLvUpPnTp_HelpItem5,
	EEpLvUpPnTp_CnsmItem1,
	EEpLvUpPnTp_CnsmItem2,
	EEpLvUpPnTp_CnsmItem3,
	EEpLvUpPnTp_CnsmItem4,
	EEpLvUpPnTp_CnsmItem5
};

//
var OperationItemBagPos _ToOpEquip;
var GFxObject _ToOpEquipSlot;

var const int TO_CONSUM_MAT_MAX;//5
var OperationItemBagPos _ToConsumItem[5];
var GFxObject _ToConsumItemSlot[5];

var const int TO_HELP_ITEM_MAX;//5
var OperationItemBagPos _ToHelpItem[5];
var GFxObject _ToHelpItemSlot[5];

//Whether conditions are met and enhance (cancel enhance) can start
var bool IsMnyOK;

//
var GFxObject _ConfirmBox;

native function NativeInitLoad();

//Monitor events to update conditions in real time
native function BagSlotChange( INT bagId,INT slotId );
native function PlayerMoneyChange(EMONEY_TYPE MoneyType, int DeltaMoney);
//
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType slotType, ECONTAINER_SLOT containerID, int slotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType slotType, ECONTAINER_SLOT containerID, int slotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType slotType, ECONTAINER_SLOT containerID, int slotID,bool bDefault);
native function bool OnDragDrop(GFxObject srcSlotMC, GFxObject destSlotMC, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int srcSlotID, ESlotPanelType destSlotType, ECONTAINER_SLOT destContainerID, int destSlotID);
event OnSubscribeItemSlotAction()
{
}

event OnUnSubscribeItemSlotAction()
{
}

event CallASSetUpgradeCostMoney(string mnyStr )
{
	ActionScriptVoid( "UCSetUpgradeCostMoney" );
}

event CallASSetPopUpBox(GFxObject data )
{
	ActionScriptVoid( "UCSetUpgradeResultData" );
}

event CallAsShowPopUpBox( bool isToShow )
{
	ActionScriptVoid( "UCShowLevelUpResult" );
}
//Update
native function UpdatePanel();
//Clear UI content
native function ResetPanel();

//Update money cost
native function UpdateFee();

//
native function ConfirmLevelUpMessageAndGo( GFxObject pMc );
native function CancelLevelUpMessage( GFxObject pMc );

//===============================================================================================================
//===============================================================================================================
final native function ASConfirmUpgrade();
final native function ASCancelUpgrade();

final native function ASShowResultConfirmBox( bool isToShow );
final native function ASConfirmMessage();
//===============================================================================================================
//===============================================================================================================
//
defaultproperties
{
	//FriendlyName=EquipLevelUp
	IsDynamicLoad=true
	TO_CONSUM_MAT_MAX=5
	TO_HELP_ITEM_MAX=5
	_ConfirmBox=none
	IsMnyOK=false
}