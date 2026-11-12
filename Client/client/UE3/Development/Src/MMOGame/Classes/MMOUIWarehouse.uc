class MMOUIWarehouse extends MMOUIBaseSlotContainer
	native(UI)
	config(UIVisibleMask);

const MaxWarehouseSlotCount = 72;
var GFxObject BagGroup;
var GFxObject TabGroup;

var bool OpenExpand;

const OriWarehouseCap = 16;
var array<GFxObject> ItemSlotList;
var int CurTab;
var int BaseMoney1;
var int BaseMoney2;
var int IncreaseRatio1;
var int IncreaseRatio2;
var int stage;

var GFxObject ConfirmMsgBox;

native function ClearUI();



event InitOnce()
{
	local int i;
	local GFxObject ItemSlot;

	super.InitOnce();

	
	BagGroup = GetObject("BagGroup_mc");
	for( i =0 ; i< MaxWarehouseSlotCount ;i++ )
	{
		ItemSlot = BagGroup.GetObject("slot"$i$"_mc");
		ItemSlot.SetInt("itemid",i); 
		ItemSlot.SetInt( "SlotType" ,ESPType_Warehouse );
		ItemSlotList.AddItem( ItemSlot );
	}

	
	//TabGroup = GetObject("TabGroup_mc");
	//TabGroup.GetObject("tab1_mc").SetVisible(false);
	//TabGroup.GetObject("tab2_mc").SetVisible(false);
	//TabGroup.GetObject("tab3_mc").SetVisible(false);
	//TabGroup.GetObject("tab4_mc").SetVisible(false);

	//class'MMOUIManager'.static.SetOpenWarehouseDelegate(OpenWarehouse);

	class'MMOUIManager'.static.SetUpdateWarehouseCapSlotDelegate(UpdateWarehouseCapSlot);

	// TODO: remove these 2 once the feature is done
	GetObject("BtnLock_mc").SetVisible(false);
	//GetObject("BtnSort_mc").SetVisible(false);
	
}
native function ItemDelete(int ContainerID, int SlotID);


native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

native function ExpandWarehouseOk( GFxObject mc);
native function ExpandWarehouseNo( GFxObject mc);

native function UpdateWarehouseCapSlot();

event CallSetPanelText( GFxObject data )
{
	ActionScriptVoid("SetPanelText");
}

event OnSubscribeItemSlotAction()
{
}

event OnUnSubscribeItemSlotAction()
{
}

// event OnShow(bool show)
// {
// 	super.OnShow(show);
// 	if( show ){
// 		BagPage( 0 );
// 	}
// }


//function OpenWarehouse()
//{
//	NativeShow(true);
//}

event BagSlotChangeEx(int BagId,int SlotID)
{
	local GFxObject ItemSlot;
	local Item tItem;
	if( BagId != E_CONSLOT_BANK )
		return;
	ItemSlot = BagGroup.GetObject("slot"$SlotID$"_mc");
	//item = player.ItemManager.BagBankList[SlotID];
	tItem = FindItemTemplate(ECONTAINER_SLOT(BagId), SlotID);
	if( tItem == none )
	{			
		`log("Bank move item not found:" @ BagId @ ":" @ SlotID);
		GFXPlayer.SetItemSlotData( ItemSlot, none, ESPType_Warehouse, ECONTAINER_SLOT(BagId), SlotID);
	}
	else 
	{
		GFXPlayer.SetItemSlotData( ItemSlot, tItem, ESPType_Warehouse, ECONTAINER_SLOT(BagId), SlotID);
	}
}

native function BagPage( int labelId );

//native function ClearUI();

native function BagSlotChange( INT BagId,INT SlotId );

event CallUpdateSortTimer(int secondsLeft)
{
	ActionScriptVoid("UCUpdateSortTimer");
}

event CallEnableSort(bool enable, string btnLabel)
{
	ActionScriptVoid("UCEnableSort");
}

//----------------moved--------------------
native function ASSortWarehouse();
native function SetItemSlotData( GFxObject mc , Item tItem, ESlotPanelType SlotType, optional ECONTAINER_SLOT ContainerID = E_CONSLOT_MAX, optional int SlotID = -1, optional EItemSlotState inISS = ISS_Normal); // moved
native function ASWareHouseTabChange(int tabId);

event ClearItemSlot(GFxObject ItemSlot)
{
	local int slotId;
	slotId = ItemSlot.GetInt("slotID");
	//Override this function; clearing slot attributes in bag uses ESPType_Bag
	SetItemSlotData( ItemSlot, none, ESPType_Warehouse , E_CONSLOT_BANK , slotId );
}


event CallUCGraySlot( GFxObject slot , bool gray)
{
	ActionScriptVoid("UCGraySlot");
}

DefaultProperties
{
	IsDynamicLoad=true
	IsTargetOutDistanceClose = true
}
