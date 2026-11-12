class MMOUIEquipEnhance extends MMOUIBaseSlotContainer
	native(UI)
	config(UIVisibleMask);

//
var GFxObject MakeEnhancePanel;
var GFxObject MakeEnhanceSuccessAnim_mc;
var GFxObject MakeEnhanceItem_mc;
var const int ENHANCE_MAT_ICON_MAX;//3
var GFxObject MakeEnhanceMatCsm_mc[3];
var GFxObject MakeEnhanceMnyCsm_tf;
var GFxObject MakeEnhanceMnyType_mc;
var GFxObject MakeEnhanceCurDgr_tf;
var GFxObject MakeEnhanceMaxDgr_tf;
//
var const int ENHANCE_PROP_MAX;//3
var GFxObject CurEnhanceProp_tf[3];
//var GFxObject CurEnhancePropValue_tf[3];
var GFxObject MakeEnhanceProp_tf[3];
//var GFxObject MakeEnhancePropValue_tf[3];
var GFxObject MakeEnhance_btn;
//
var GFxObject TransEnhancePanel;
var GFxObject TransEnhanceFromItem_mc;
var GFxObject TransEnhanceToItem_mc;
var GFxObject TransEnhanceMnyCsm_tf;
var GFxObject TransEnhanceCurDgr_tf;
var GFxObject TransEnhance_btn;

//Position in the bag of the item currently in the enhance (transfer enhance) slot (-1 is empty)
var int CurWorkingItemBagID;
var int CurWorkingItemContainID;
//Position in the bag of the item to transfer the enhancement to in the item slot (-1 is empty)
var int CurTransEnhanceToItemBagID;
var int CurTransEnhanceToItemContainID;

//Whether conditions are met and enhance (cancel enhance) can start
var bool IsMatOK;
var bool IsMnyOK;
var bool IsDegreeOK;
var bool IsTransToItemOK;

enum enumEquipEnhanceMode
{
	EEQPEHC_MakeEnhance,
	EEQPEHC_TransEnhance
};

enum enumEquipSlotID
{
	EEHCSLT_MakeEnhanceSlot,
	EEHCSLT_TransEnhanceSlot,
	EEHCSLT_TransTargetSlot
};

var enumEquipEnhanceMode EnumCurEnhanceMode;

//Monitor events to update conditions in real time
native function BagSlotChange( INT BagId,INT SlotId );
native function PlayerMoneyChange(EMONEY_TYPE MoneyType, int DeltaMoney);
//
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
event OnSubscribeItemSlotAction()
{
}

event OnUnSubscribeItemSlotAction()
{
}

native function Equip GetBagEquip( int bagSlotCon, int bagSlotID );
//Check whether it can be enhanced (cancel enhance)
native function bool CheckCanGo();
//Check whether it is an item currently in use by this UI
native function bool CheckInUse( int ItemInBagID );
//Determine whether all materials are present by reading back the already-set UI values, avoiding polling all material cells in the bag every time
native function ReChkMatCondtionByUI();
//Check whether transfer is possible
native function bool CheckCanTransEnhance( int bagSlotCon, int bagSlotIdx );

//Put in the equipment to enhance (cancel enhance)
native function SetWorkingSlotData( int bagSlotCon, int bagSlotID, Item slotItem );
native function SetTransSlotTarget(  int bagSlotCon, int bagSlotID, Item slotItem);
native function int CurBagIdxOfTargetEquip();
native function int CurBagIdxOfTransTargetEquip();
native function int CurContainOfTargetEquip();
native function int CurContainOfTransTargetEquip();
//Update
native function UpdatePanel();
//
native function UpdateMakeEnhanceInfo();
//
native function UpdateTransEnhanceInfo();
//Update required material info
native function UpdateMaterialConsume();
//Update the count of a material
native function int UpdateMaterialCnt( GFxObject TfCurHave, GFxObject TfCnt, int itemID, int MatCsmNeed );
//Update (transfer) enhance money cost
native function UpdateFee();
//Update (transfer) enhance item's current enhance level, max enhance level, enhance attributes
native function UpdateEquipInfo();

//

//Clear the unlocked equipment that was put in
native function ResetEnhanceItems();
//Called by AS, used to restore the default UI
native function ShowDefaultContent( int mode);
//
native function ResetUIMode();
//Clear UI content
native function ResetPanelInfo();
//
native function ResetMakeEnhanceInfo();
//
native function ResetTransEnhanceInfo();
//Clear material slots
native function ResetMaterialConsume( int startIdx );
//Restore to the default tab, notify Flash
event ChangeContentTabBtn( int tabIdx )
{
	ActionScriptVoid( "UCShowContent" );
}
//
native function ResetCondition();
//Check state, set UI button hints, etc.
native function CheckOperation();

//Operation response
native function MakeEnhanceItem();
native function NotifyMakeEnhanceItemResult( bool isSucceed );
//
native function TransferEnhanceItem();
//Control operation feasibility
native function SetOkBtn();

//===============================================================================================================
//===============================================================================================================
function ASHideEquipEnhance()
{
	NativeShow( false );
}

//
function ASMakeEnhanceItem()
{
	MakeEnhanceItem();
}

//
function ASTransferEnhanceItem()
{
	TransferEnhanceItem();
}

//
function ASUpdateEquipEnhancePanel( int iTab )
{
	ShowDefaultContent( iTab );
}
//===============================================================================================================
//===============================================================================================================

defaultproperties
{
	//FriendlyName=EquipEnhance
	IsDynamicLoad=true
	ENHANCE_MAT_ICON_MAX=3
	ENHANCE_PROP_MAX=3
	IsMatOK=false
	IsMnyOK=false
	IsDegreeOK=false
	IsTransToItemOK=true
	IsTargetOutDistanceClose = true
}
////
//event bool CheckAuxlMatOK( int itemSlotID )
//{
//	local Item auxiliaryItem;
//	//auxiliaryItem = Player.ItemManager.BagMaterialList[SrcSlotID];
//	auxiliaryItem = Player.ItemManager.BagItemList[itemSlotID];
//	if( auxiliaryItem == none ){
//		return false;
//	}
//	//
//	if( UnEnhcAxulMat[0] == auxiliaryItem.ItemId ){
//		return true;
//	}
//	if( UnEnhcAxulMat[1] == auxiliaryItem.ItemId ){
//		return true;
//	}
//	return false;
//}