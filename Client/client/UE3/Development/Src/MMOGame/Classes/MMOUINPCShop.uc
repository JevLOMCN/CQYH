class MMOUINPCShop extends MMOUIBaseSlotContainer
	dependson(TypeDefine)
	native(UI)
	config(UIVisibleMask);

var localized string name1;
var localized string name2;

var transient GFxObject ShopContent;
var transient int _Typeid;
var transient int Page;
var transient int MaxPage;
var transient int CurTab;

var transient GamePlayDataManager GpDataManager;
var const int ItemSize; //size of an item grid cell in a page
var const int TabNum;
var transient GFxObject ShopMessage;
var GFxObject ConfirmMsgBox;

var transient item MessageItem; //item displayed by message
var transient int MessageItemShopSlot; //shop slot of the item displayed by message, the server needs...
var transient bool IsFilterUseToggle;

var transient MMOPlayer           MainPlayer;
var transient MMOPlayerController PC;

//var transient init array<ShopItemTemplate> EquipShopItemList; //equipment
//var transient init array<ShopItemTemplate> NormalShopItemList; //consumables
//var transient init array<ShopItemTemplate> MaterialShopItemList; //materials

var transient ItemSlotActionListener_Repair RepairAction;

enum ENpcShopState
{
	ENSS_Normal,
	ENSS_Buy,
	ENSS_Sell,
	ENSS_Repair,
};

var transient ENpcShopState _curState;

native function ClearUI();

native function NativeInitLoad();

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	
	//MWindow = GetObject("MWindow");
	ShopContent = GetObject("ShopContent");
	GpDataManager = class'GamePlayDataManager'.static.Singleton();

	PC = MMOPlayerController( GetPC() );
	MainPlayer = MMOPlayer( PC.Pawn );
	
	//IsFilterUseToggle = ShopContent.GetObject("checkboxFilter_mc").GetBool("selected");
	pl.AddPostAdvanceDelegate(PostAdvance);
}

event GFXObject GetSlot(int idx)
{
	local GFxObject mc;
	mc = ShopContent.GetObject("NPCShopList"$idx);
	return mc;
}

event InitOnce()
{
	local int i;
	local GFxObject mc;
	i = 0;
	for( i = 0 ; i < ItemSize ; i++ )
	{
		mc = ShopContent.GetObject("NPCShopList"$i);
		if( mc != none )
		{
			mc.SetInt("SlotType", ESPType_Shop );
			//mc.SetInt("itemid", i );
		}
	}


	//ShopMessage = GFXPlayer.GetVariableObject("root.ShopMsg_mc");
	//if(ShopMessage!=none)
	//{
		//ShopMessage.SetVisible(false);
	//}
	

	class'MMOUIManager'.static.SetOpenShopDelegate(OpenShop);	
	class'MMOUIManager'.static.SetOpenBlackMarketDelegate(OpenBlackMaret);	

	super.InitOnce();

}

event CallSetPanelText(GFxObject data)
{
	ActionScriptVoid("SetPanelText");
}

event CallSetPageButton( GFxObject data )
{
	ActionScriptVoid("SetPageButton");
}

event CallUpdateTabLabel(String dat,int indax,int itemType)
{
	ActionScriptVoid("UCUpdateTabLabel");
}

event CallUpdateShopType( GFxObject data )
{
	ActionScriptVoid("UCUpdateShopType");
}
event CallUpdateText( String data )
{
	ActionScriptVoid("UCUpdateText");
}

native function PlayerMoneyChange(EMONEY_TYPE MoneyType, int DeltaMoney );
native function NativeInitOnce();
native function ClearWidget();

native function OpenShop( int ShopId );
native function OpenBlackMaret( int ShopId );

native final function NpcShopTab( int itemid);

native function SendBuyShopItem(INT ShopItemId , INT ItemCount , int ItemTempId );
native function UsebleFilterToggle( bool bToggle );

native function ClickRepairAll();

native function RepairItem( int ConSlot , int ItemSlot );
 
// Purchase handling
//function ShopMsgAck( bool agree , GFXObject mc )
//{
	//local int count;
	//local GFxObject slot;

	//if( agree && ( MessageItem != none) )
	//{
		//count = int( mc.GetObject( "tF_Number" ).GetObject("textField").GetText() );
		//`log("ShopMsgAck"@count@ MessageItem.itemid);
		//Clamp( count ,1,99);


		
		//SendBuyShopItem( MessageItemShopSlot , count , MessageItem.GetTemplateID() );
	//}
//}

native function ShopMsgAck( bool agree , int count);

native function NativeRepair();


event ClickRepair()
{
	//_curState = ENSS_Repair;

	//// OSCARTODO: refactor away this function that toggles repair state
	//NativeRepair();

	//if(RepairAction == none)
	//{
	//	RepairAction = new class'ItemSlotActionListener_Repair';
	//}

	local IItemSlotActionListener curLsn;
	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();
	if( curLsn!=none && curLsn.IsA('ItemSlotActionListener_Repair') )
	{
		class'MMOUIManager'.static.ClearCurrentItemSlotActionListener();
		return;
	}

	if( RepairAction == none )
	{
		RepairAction = new class'ItemSlotActionListener_Repair';
	}

	class'MMOUIManager'.static.SetCurrentItemSlotActionListener(RepairAction);

	//class'MMOUIManager'.static.SetCurrentItemSlotActionListener(RepairAction);
}

//event TryClearSpecRepairListener()
//{
//	local IItemSlotActionListener curLsn;
//	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();
//	if( curLsn!=none && curLsn.IsA('ItemSlotActionListener_SpecRepair') )
//	{
//		class'MMOUIManager'.static.ClearCurrentItemSlotActionListener();
//		return;
//	}
//}

native final function PagePrev();
native final function PageNext();


native function ShowShopPurchaseDialog(int SlotID);
//{
//	local int ItemTemplateID;
//	local Item ItemTemplate;
//	local GFxObject mc;
//	mc = GetSlot(SlotID);
//	ItemTemplateID = mc.GetInt("iteminfoid");

// 	ItemTemplate = FindItemTemplateByID(ItemTemplateID);

//	if( ItemTemplate == none)
// 		return ;



//	ShopMessage.SetVisible( true);
//	ShopMessage.GetObject("tF_Number").SetText( "1" );
//	ShopMessage.GetObject("buycounttext").GetObject("textField").SetText( ItemTemplate.ItemName );
//	MessageItem = ItemTemplate;
//	MessageItemShopSlot = SlotID;
//	NumChange();
//}

native function UpdateShopItemList();

native final function NumChange(int num);
// {
// 	local int count;
// 	count = int( ShopMessage.GetObject( "tF_Number" ).GetString("text") );
// 	ShopMessage.GetObject("MsgText_mc").SetText( string(CurShop.ItemList[MessageItemShopSlot].CostValue * count));
// }

native final function UpdateTabButton();
native final function UpdateListItem();

native final function SetupTab(int idx, EITEM_TYPE itemType, string tabIconFrame, string tabTips);
native final function UpdateSoldItemView();


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

native function SoldItemChange();

native final function SellItem(int containerID, int slotID);
native final function DoSellItem(int containerID, int slotID);
native final function OnSellItem(GFxObject mc);
native final function OnCancelSellItem(GFxObject mc);

event CallSelectFilter(int shopId)
{
	ActionScriptVoid("UCSelectFilter");
}

native function PostAdvance(float DeltaTime );

//-------------------moved----------------------------------
native function bool SkipCaptureInputKey(String key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

// event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)
// {
// 	// Mouse scroll, and not inside the UI
// 	if ((key == "MouseScrollUp" || key == "MouseScrollDown") && (strHoverControllerName == ""))
// 		return false;
// 	else
// 		return true;
// }

native function ASNPCShopRepair();

native function ASNPCShopRepairAll();
native function ASNPCShopClose();

function NPCShopUsebleFilterToggle(bool bToggle )
{
	UsebleFilterToggle( bToggle );
}

native function ASShowTips_RepairAll( GFxObject butt );


native function ASHideTips_RepairAll( GFxObject butt );

native function ASShowRepairTips(GFxObject butt);
native function ASHideRepairTips(GFxObject butt);


native final function NpcShopMsg_OK(GFxObject mc);
native final function NpcShopMsg_Cancel(GFxObject mc);

function ASNPCShopPagePrev()
{
	PagePrev();
}

function ASNPCShopPageNext()
{
	PageNext();
}

function ASNpcShopTab(int itemid)
{
	NpcShopTab(itemid);
}

event UCGraySlot( string slotName  , bool gray)
{
	ActionScriptVoid("UCGraySlot");
}

DefaultProperties
{
	IsTargetOutDistanceClose=true
	IsDynamicLoad=true
	_Typeid=0
	Page=0
	MaxPage=0
	ShopMessage=none
	_curState=ENSS_Normal
	ItemSize=10
	TabNum=4
	CurTab=0
}
