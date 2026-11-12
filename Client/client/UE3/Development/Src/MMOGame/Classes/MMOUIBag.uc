class MMOUIBag extends MMOUIBaseSlotContainer
	dependson(ServerEnum)
	native(UI)
	config(UIVisibleMask);

enum EBagTabType
{
	EBTT_AllItem,
	EBTT_FilterEquip,
	EBTT_FilterItem,
	EBTT_FilterMaterial,	
};

enum EBagLabelType
{
	EBLT_NormalItem,
	EBLT_Quest,
};

var int     CurLabel;
var int     LastLabel;
var int     CurTab;
var array<int>     LabelBagPage; //Bag actually only uses E_CONSLOT_BANK; for easy extension E_CONSLOT_MAX is used here
var GFxObject MoneyText[4];
var GFxObject LabelButton[4];
var GFxObject CapMc; // capacity
var GFxObject BagGroup;
//var GFxObject BagUsedSlotTf;
const OriBagCap = 32;
const MaxBagSlotCount = 64; //itemdef
const LabelBagPageCount = 1;
const MaxBagSlotTotalCount = 64;//112;
const MaxBagDepositCount = 2;//112;
var int HintSlotLast[MaxBagSlotCount];
var array<GFxObject> ItemSlotList;
var bool OpenExpand;

var int pageIdCur;
var int slotCountCur;
var int slotCountDown;
var int onShowDelay;

var int BaseMoney1;
var int BaseMoney2;
var int IncreaseRatio1;
var int IncreaseRatio2;
var int stage;
var bool bOpenResBag;


//Lock state of items in the bag
//var native Map_Mirror BagSlotState{TMap< INT, EItemSlotState >};
//var native Map_Mirror QuestBagSlotState{TMap< INT, EItemSlotState >};
//var int CurPageIdx;

var GFxObject BagInfo;
var ItemSlotActionListener_ItemSplit SplitAction;
var ItemSlotActionListener_Identify IdentifyAction;
var ItemSlotActionListener_Signature SignatureAction;
var ItemSlotActionListener_Decompose DecomposeAction;
var transient ItemSlotActionListener_SpecRepair RepairActionSpecial;

var bool HasAskToLearnSkillBook;

var GFxObject ConfirmMsgBox;

var OperationItemBagPos CurLoadedExpContainer;
var GFxObject ExpContainerSlot;
var int ConstExpContainerSlot;

var bool bOpenByNpcShop;

native function SkillBookMsgBox(int ItemSlot);
native function SkillBookMsgBoxOk(GFxObject mc);
native function SkillBookMsgBoxCancel(GFxObject mc);

native function ConfirmUseExpContainer(GFxObject mc);
native function CancelUseExpContainer( GFxObject mc );

native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
native function bool OnDoubleClickSlot(GFxObject SrcSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, bool bDefault);

native function ClearUI();
native function ClearNewHint();

native function OnDecomposeEquipOk();
native function OnDecomposeEquipCancel();

native final function NativeOnSubscribeItemSlotAction();
native final function NativeOnUnSubscribeItemSlotAction();

native function BeforeViewUpdate( float DeltaTime );
native function HideBag_Msg();
event CallSetMoney( GFxObject data )
{
	ActionScriptVoid("SetMoney");
}

event OnSubscribeItemSlotAction()
{
	NativeOnSubscribeItemSlotAction();
}

event OnUnSubscribeItemSlotAction()
{
	NativeOnUnSubscribeItemSlotAction();
}

event CallAsSetExpContainerProgress( int curVar, int maxVar )
{
	ActionScriptVoid("UCSetExpContainerProgress");
}

event CallAsShowExpContainer( bool isToShow )
{
	ActionScriptVoid("UCShowExpContainer");
}

event ClickSplit()
{
	// OSCARTODO: refactor away this function that toggles repair state
	//NativeRepair();
	local IItemSlotActionListener curLsn;
	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();
	if( curLsn!=none && curLsn.IsA('ItemSlotActionListener_ItemSplit') )
	{
		class'MMOUIManager'.static.ClearCurrentItemSlotActionListener();
		return;
	}

	if(SplitAction == none)
	{
		SplitAction = new class'ItemSlotActionListener_ItemSplit';
	}
	class'MMOUIManager'.static.SetCurrentItemSlotActionListener(SplitAction);
}

event ClickIdentify()
{
	local IItemSlotActionListener curLsn;
	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();
	if( curLsn!=none && curLsn.IsA('ItemSlotActionListener_Identify') )
	{
		class'MMOUIManager'.static.ClearCurrentItemSlotActionListener();
		return;
	}

	if( IdentifyAction == none )
	{
		IdentifyAction = new class'ItemSlotActionListener_Identify';
	}

	class'MMOUIManager'.static.SetCurrentItemSlotActionListener(IdentifyAction);
}

event ClickSignature()
{
	local IItemSlotActionListener curLsn;
	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();
	if( curLsn!=none && curLsn.IsA('ItemSlotActionListener_Signature') )
	{
		class'MMOUIManager'.static.ClearCurrentItemSlotActionListener();
		return;
	}



	if( SignatureAction == none )
	{
		SignatureAction = new class'ItemSlotActionListener_Signature';
	}

	class'MMOUIManager'.static.SetCurrentItemSlotActionListener(SignatureAction);
}

event ClickDecompose()
{
	local IItemSlotActionListener curLsn;
	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();
	if( curLsn!=none && curLsn.IsA('ItemSlotActionListener_Decompose') )
	{
		class'MMOUIManager'.static.ClearCurrentItemSlotActionListener();
		return;
	}

	if( DecomposeAction == none )
	{
		DecomposeAction = new class'ItemSlotActionListener_Decompose';
	}

	class'MMOUIManager'.static.SetCurrentItemSlotActionListener(DecomposeAction);
}

final native function PrepareForRepair();
event ClickSpecRepair()
{
	local IItemSlotActionListener curLsn;
	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();
	if( curLsn!=none && curLsn.IsA('ItemSlotActionListener_SpecRepair') )
	{
		class'MMOUIManager'.static.ClearCurrentItemSlotActionListener();
		return;
	}

	if( RepairActionSpecial == none )
	{
		RepairActionSpecial = new class'ItemSlotActionListener_SpecRepair';
	}

	class'MMOUIManager'.static.SetCurrentItemSlotActionListener(RepairActionSpecial);

	//
	PrepareForRepair();
}

event OnShow(bool show)
{
	// bag is the default ItemSlotActionListener, so no need to set it to current
	
}

native function OnFocusGained();
native function OnFocusLost();
native function OnShift(bool bRelease);

event InitLoad(MMOGFXPlayer pl)
{
	
	super.InitLoad(pl);
	
	
	class'MMOUIManager'.static.SetDeleteItemDelegate(DelegateDeleteItem);
	class'MMOUIManager'.static.SetGetOpenBagDelegate(GetBagEnum);
	class'MMOUIManager'.static.SetTurntoPageDelegate(TurntoPage);
	class'MMOUIManager'.static.SetLockItemDelegate(LockItem);
	class'MMOUIManager'.static.SetUpdateBagCapSlotDelegate(UpdateBagCapSlot);
	class'MMOUIManager'.static.SetHintNewItemLabelDelegate(HintNewItemLabel);

	//BagUsedSlotTf = GetObject("bagUsed");	

}

native function BagSlotChangeEx(ECONTAINER_SLOT ContainerID, int SlotID);


native function LockItem(ECONTAINER_SLOT ContainerID, int SlotID, int isToLockIn);

//native function ClearItemSlot(ECONTAINER_SLOT ContainerID, int SlotID);
//{
//	local GFxObject ItemSlot;
//	local int pageSlotId;

//	pageSlotId = SlotID - LabelBagPage[CurLabel] * MaxBagSlotCount;
//	ItemSlot = BagGroup.GetObject("slot"$pageSlotId$"_mc");	

//	GFXPlayer.ClearItemSlot(ItemSlot);
//}


event InitOnce()
{
	//local int i;
	//local GFxObject ItemSlot;

	super.InitOnce();
	
	
	
	//class'MMOUIManager'.static.SetDefaultItemSlotActionListener(self);
	class'MMOUIManager'.static.SetOpenBagDelegate(OpenBagEx);

	LabelButton[0] = GetObject("tab0_mc");
	LabelButton[1] = GetObject("tab1_mc");
	LabelButton[2] = GetObject("tab2_mc");
	LabelButton[3] = GetObject("tab3_mc");

	//Tab new is not needed for now
	//for(i = 0; i < 4; ++i)	
	//{
	//	LabelButton[i] = GetObject("tab"$i$"_mc");
	//	if( LabelButton[i] != none )
	//	{
	//		LabelButton[i].GetObject("newHint").SetVisible(false);
	//	}
	//}
}

native function NativeInitOnce();
native function NativeOnShow(bool v);

event OpenBagEx(ECONTAINER_SLOT ContainerType)
{
	// OSCARTODO: open the corresponding bag
	OpenBag(CurLabel);
}

native function BagTab(int tabId);
native function BagLabel(int labelId);
native function BagPage( int pageId);

native function PlayerMoneyChange(EMONEY_TYPE MoneyType, int DeltaMoney);
native function PlayerAttributeChange( int AtrrId, int inValue );


native function Item GetItemInCurBag( int index );

native final function UpdateMoneyDisplay();
native function ItemChange(int ContainerID, int SlotID);
native function BagSlotChange( INT BagId,INT SlotId );
native function ItemDelete(int ContainerID, int SlotID);
native function OnStallStateChange(EEStallState state);

native function ExpandBagOk( GFxObject mc );
native function ExpandBagNo();
native function UpdateBagCapSlot( int ContainerID );

native function HintNewItemLabel( int containerId );

function OpenBag( int label )
{
	local GFxObject mc;
	NativeShow( true );
	GotoAndPlayI(1);
	CurLabel = label;
	mc = GetObject( "label"$label$"_mc" );
	GFXPlayer.CallWidgetDispatchEvent( mc , "click" );
	BagLabel(CurLabel);

	UpdateMoneyDisplay();
}

event RefreshBag()
{
	BagLabel(CurLabel);
	UpdateMoneyDisplay();
}

native function ECONTAINER_SLOT GetBagEnum();

native function TurntoPage( ECONTAINER_SLOT BagType );

//function bool CurIsEquipBag()
//{
//	return GetBagEnum() == E_CONSLOT_EQUIPMENT;
//}



native function OnThrowItemOk(GFxObject mc);


native function OnInputThrowItemOk(GFxObject mc);


native function DeleteItem( int ConId , int SlotId , int TemplateId);
native final function DelegateDeleteItem(ECONTAINER_SLOT ContainerID, int SlotID);

native final function OnEquipItemOK(GFxObject mc);

native final function ASShowBagWeightTips(GFxObject mc);
native final function ASHideBagWeightTips(GFxObject mc);


event CallUCGraySlot( GFxObject slot , bool gray)
{
	ActionScriptVoid("UCGraySlot");
}

event CallUCHintSlot( int slotID, bool isHint , bool isNew )
{
	`log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CallUCHintSlot at Slot:"@slotID@"To:"@isHint);
	ActionScriptVoid("UCHintSlot");
}

event CallUpdateSortTimer(int secondsLeft)
{
	ActionScriptVoid("UCUpdateSortTimer");
}

event CallEnableSort(bool enable, string btnLabel)
{
	ActionScriptVoid("UCEnableSort");
}

native function bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);
//{
//	// Mouse scroll, and not inside the UI
//	if ((key == "MouseScrollUp" || key == "MouseScrollDown") && (strHoverControllerName == ""))
//		return false;
//	else
//		return true;
//}

//event ClearItemSlot(GFxObject ItemSlot)
//{
//	local ECONTAINER_SLOT conslotId;
//	local int slotId;
//	conslotId = ECONTAINER_SLOT( ItemSlot.GetInt("containerID") );
//	slotId = ItemSlot.GetInt("slotID");
//	//Override this function; clearing grid attributes in bag is ESPType_Bag
//	SetItemSlotData( ItemSlot, none, ESPType_Bag , conslotId , slotId );
//}

event SetBagWeight(int currentWeight, int totalWeight)
{
	ActionScriptVoid("UCSetBagWeight");
}
function ASBagTabChange( int tabId )
{
	BagTab(tabId);
}

function ASBagLabelChange( int labelId )
{
	BagLabel(labelId);
}

event SetBagTab(int tab)
{
	ActionScriptVoid("UCSetBagTab");
}

native function RequireOpenStall();

native function ASLockItem();
native function ASSplitItem();
native function ASSortBag();
native function ASEvaluteItem();
native function ASSignatureItem();

native function ASClickMoneyChain( bool selected );

//native function SetItemSlotData( GFxObject mc , Item tItem, ESlotPanelType SlotType, optional ECONTAINER_SLOT ContainerID = E_CONSLOT_MAX, optional int SlotID = -1, optional EItemSlotState inISS = ISS_Normal); // moved
native function ASStartDrag(GFxObject srcItemSlot, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int srcSlotID); // moved

native function ASMoneyExchange();
native function NativeInitLoad();

native function ASOpenMaterialPanel();

DefaultProperties
{
	//BagLabel=0
	ConstExpContainerSlot=0
	DefaultVisible=false
	HasAskToLearnSkillBook=false
	pageIdCur=-1
	slotCountCur=-1;
	slotCountDown=0;
	onShowDelay=-1;
	FriendlyName=Bag
}
