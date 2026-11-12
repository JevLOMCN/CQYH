class MMOUITradeP2P extends MMOUIBaseSlotContainer
	native(UI)
	config(UIVisibleMask);

const TRADE_ITEM_MAX = 6;

var GFXObject TradeP2PPanelmc;
var GFxObject MyTradeMc;
var GFxObject TgTradeMc;
		
var GFxObject TradeTexTf;
var GFxObject TradeTexRateTf;
		
var GFxObject ConfirmTradeBtn;
var GFxObject BtnCancelTradeBtn;
var GFxObject BtnLockTradeBtn;

var GFxObject MyNameTf;
var GFxObject MyInputMnyBtn;
var GFxObject MyTradeLockMc;

var GFxObject TgNameTf;
var GFxObject TgInputMnyBtn;
var GFxObject TgTradeLockMc;

var GFxObject ConfirmTradeBox;
var GFxObject InputMnyBox;
var GFxObject ConfirmUnlockBox;
//
var int MaxTradeDistanceSqu;

var int MyTradeState;
var int TgPlayerID;
var int MyTradeMny;
var int TgTradeMny;
var float TradeTexRate;
var bool IsMyLocked;
var bool IsTgLocked;
var bool IsInputingMny;

var bool IsBeApplyed;
var bool IsSelfCanelTrade;
var int TradeNotificationMsgButtonIdx;
var bool AlreadyCanceled;
//var int PendingDelContainID;
//var int PendingDelSlotID;

struct native P2PTradeItemData
{
	var int ItemID;
	var int FromContainID;
	var int FromSlotID;
};

struct native P2PTradeItemObj
{
	var GFxObject ParentUI;
	var GFxObject Item;
	var GFxObject Name;
	var GFxObject Mask;
};

struct native P2PTradeItem
{
	var P2PTradeItemData Data;
	var P2PTradeItemObj Ui;
	var bool hasData;
};

var P2PTradeItem MyTradeItems[TRADE_ITEM_MAX];
var P2PTradeItem TgTradeItems[TRADE_ITEM_MAX];

//
native function NativeInitLoad();

native function UpdateCondition();

native function ApplyDoTrade( int roleID );
native function NotifyApplyDoTrade( int roleID );
native function ReplyAgreeTrade( GFxObject mc );
native function ReplyRefuseTrade( GFxObject mc );
native function DoTrade( int roleID, int roleLv );
//
native function bool NotifyAddTradeItem( int putInSlotID, int fromContainID, int fromSlotID );
native function bool RealAddTradeItem( int containID, int slotID, int itemID );
//native function PendingDelTradeItem( int containID, INT int );//only used when replacing trade items
native function bool RealDelTradeItem( bool isMy, int putOutSlotID );
//native function bool DelTradeItem( int containID, int slotID );
//native function ClearAllTradeItems();
native function NotifyTradeMoneyChanged( bool isMy, int newMoney );
native function OnMyTradeMoneyChanged( GFxObject mc );
native function OnMyTradeMoneyInputCanceled( GFxObject mc );
native function NotifyLockP2PTrade( bool isMy, bool isLocked );
native function NotifyTargetTradeLockState( bool isLocked );
native function NotifyTradeResult( bool isSucceed );
//
native function InputP2PTradeMoney();
native function ConfirmP2PTrade();
native function CancelP2PTrade();
native function LockP2PTrade();
//
native function ReplyUnlockTradeContinue( GFxObject mc );
native function ReplyUnlockTradeCancel( GFxObject mc );
//
//native function TickForTestInteractNPCDistance(float deltaTime);

native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
//
native function ResetData();
native function ResetShow();
native function bool CheckError( int roleID, bool needDisplayError );
native function bool CheckInTrade( int containID, int slotID );
native function bool CheckCanConfirm();
native function byte MyContainerType();
native function byte TgContainerType();

//
event OnSubscribeItemSlotAction()
{
}
//
event OnUnSubscribeItemSlotAction()
{
}
//
event Open()
{
	NativeShow(true);
}

//===============================================================================================================
//===============================================================================================================
native function PostAdvance(float DeltaTime);
//
function ASConfirmP2PTrade()
{
	ConfirmP2PTrade();
}
//function RegisterTradeP2P( MMOUITradeP2P mc )
//{
//	_TradeP2P = mc;
//	RegisterUI( mc, class'MMOUIManager'.default.Panel_TradeP2P );
//}
//
function ASInputP2PTradeMoney()
{
	InputP2PTradeMoney();
}
//
function ASLockP2PTrade()
{
	LockP2PTrade();
}
//
function ASCancelP2PTrade()
{
	NativeShow( false );
}
//
//Name, class, gender, level, friendship display
event CallAsSetTradeInfo( string targetName, string targetClass, int targetSex, int targetLv, int targetFriendship )
{
	ActionScriptVoid( "UCSetTradeInfo" );
}
//===============================================================================================================
//===============================================================================================================

//
defaultproperties
{
	//FriendlyName=TradeP2P
	MaxTradeDistanceSqu=2890000//1700
	TradeTexRate=0.05
	IsMyLocked=false
	IsSelfCanelTrade=false
	IsBeApplyed=false;
	IsTargetOutDistanceClose = true
}