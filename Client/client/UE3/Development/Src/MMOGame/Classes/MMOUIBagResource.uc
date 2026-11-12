class MMOUIBagResource extends MMOUIBaseSlotContainer
	native(UI)
	config(UIVisibleMask);

const MaxBagResourceSlotCount = 72;
const MaxBagResourceSlotTotalCount = 216;
var GFxObject BagGroup;
var GFxObject TabGroup;

var int     CurLabel;
var int     LastLabel;
var int     CurTab;

var int pageIdCur;
var int slotCountCur;
var int slotCountDown;
var int onShowDelay;

var int ExpandMoney1;
var int ExpandMoney2;
var int ExpandMoney3;

var bool OpenExpand;

var array<GFxObject> ItemSlotList;

var GFxObject ConfirmMsgBox;

var bool bOpenByNpcShop;

var GFxObject bagResourceTab_0;
var GFxObject bagResourceTab_1;
var GFxObject bagResourceTab_2;
var GFxObject BtnSell_mc;

var int HintSlotLast[MaxBagResourceSlotTotalCount];
var int lastPage;

var bool bChangePlayer;

var array<name> showPanelList;

native function ItemDelete(int ContainerID, int SlotID);



native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
native function BagPage( int labelId );
native function BeforeViewUpdate( float DeltaTime );

native function BagSlotChange( INT BagId,INT SlotId );

native function ExpandBagResourceOk(GFxObject mc);
native function ExpandBagResourceNo(GFxObject mc);

native function  SellBagResourceAllOk(GFxObject mc);
native function  SellBagResourceAllNo(GFxObject mc);

native function ASMaterialTypeTabChange(int id);
native function ASMaterialSellAll();
native function ASSortMaterialBag();

native function ASOnMaterialPanelClose();

event InitOnce()
{
	local int i;
	local GFxObject ItemSlot;

	super.InitOnce();
	
	BagGroup = GetObject("BagGroup_mc");

	if( BagGroup != none )
	{
		for( i =0 ; i < MaxBagResourceSlotCount ; i++ )
		{
			ItemSlot = BagGroup.GetObject("slot"$i$"_mc");
			if( ItemSlot != none )
			{
				ItemSlot.SetFloat("itemid",i); 
				//Strange effect here
				ItemSlotList.AddItem( ItemSlot );
			}
		}
	}
}

//----------------moved--------------------
native function SetItemSlotData( GFxObject mc , Item tItem, ESlotPanelType SlotType, optional ECONTAINER_SLOT ContainerID = E_CONSLOT_MAX, optional int SlotID = -1, optional EItemSlotState inISS = ISS_Normal); // moved



event CallUCGraySlot( GFxObject slot , bool gray)
{
	ActionScriptVoid("UCGraySlot");
}

event CallUpdateSortTimer(int secondsLeft)
{
	ActionScriptVoid("UCUpdateSortTimer");
}

event CallEnableSort(bool enable, string btnLabel)
{
	ActionScriptVoid("UCEnableSort");
}

event CallUCHintSlot( int slotID, bool isHint , bool isNew )
{
	ActionScriptVoid("UCHintSlot");
}

DefaultProperties
{
	IsDynamicLoad=true
	//IsTargetOutDistanceClose = true
}
