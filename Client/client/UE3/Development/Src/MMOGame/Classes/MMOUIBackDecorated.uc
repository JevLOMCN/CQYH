class MMOUIBackDecorated extends MMOUIBaseSlotContainer
	native(UI)
	dependson(TypeDefine)
	config(UIVisibleMask);

enum BackDecoratePanelSlotType
{
	EBackDecorateSlot_Invalid,
	EBackDecorateSlot_MakeUp,
	EBackDecorateSlot_MakeUpCnpt,
	EBackDecorateSlot_Upgrade,
	EBackDecorateSlot_UpgradeCnpt,
	EBackDecorateSlot_UpgradedItem
};

enum BackDecoratePanelType
{
	EBackDecorate_Invalid,
	EBackDecorate_MakeUp,
	EBackDecorate_Upgrade,
};

var BackDecoratePanelType _CurPanelType;

var int _ConsumptionIDForMakeUp;

var OperationItemBagPos _ItemForUpgrade;

//
var GFxObject _ItemForMakeUpSlot;
var GFxObject _MakeUpMoneyTf;
var GFxObject _MakeUpItemConsumpution;
//
var GFxObject _ItemForUpgradeSlot;
var GFxObject _UpgradeMoneyTf;
var GFxObject _UpgradeItemConsumpution;
var GFxObject _UpgradedItemSlot;

//
native function BagSlotChange( int bagId, int slotId );
//native function OnItemDeleted( int containerID, int slotID );
//native function PlayerMoneyChange( EMONEY_TYPE MoneyType, int Money, int OldMoney);
//
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject srcSlotMC, GFxObject destSlotMC, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int SrcSlotID, ESlotPanelType destSlotType, ECONTAINER_SLOT destContainerID, int destSlotID);
//event OnSubscribeItemSlotAction()
//{
//}

//event OnUnSubscribeItemSlotAction()
//{
//}

native function NativeInitLoad();

//
event int GetCurPanelType()
{
	return ActionScriptInt( "UCGetCurrentPanelType" );
}

//Craft
event CallAsSetMakeUpConsumptionInfo( string iconName, int itemID, int colorIdx, int curConsumptionCnt, int needConsumptionCnt )
{
	ActionScriptVoid( "UCSetMakeUpConsumptionInfo" );
}
event CallAsSetMakeUpResultItem( string iconName, int itemID, int colorIdx )
{
	ActionScriptVoid( "UCSetMakeUpResultItem" );
}
event CallAsSetMakeUpMoney( string moneyStr )
{
	ActionScriptVoid( "UCSetMakeUpMoney" );
}
//Upgrade
event CallAsSetUpgradeConsumptionItem( string iconName, int itemID, int colorIdx, int curConsumptionCnt, int needConsumptionCnt )
{
	ActionScriptVoid( "UCUpgradeConsumptionItem" );
}
event CallAsSetUpgradedResultItem( string iconName, int itemID, int colorIdx )
{
	ActionScriptVoid( "UCSetUpgradedResultItem" );
}
event CallAsSetUpgradeMoney( string moneyStr )
{
	ActionScriptVoid( "UCSetUpgradeMoney" );
}
event CallAsSetUpgradeSuccessRate( int percent )
{
	ActionScriptVoid( "UCSetUpgradeSuccessRat" );
}
//===============================================================================================================
//===============================================================================================================
final native function ASOnTabClick( int idx );
final native function ASMakeUpConfirm();
final native function ASUpgradeConfirm();
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{

}
