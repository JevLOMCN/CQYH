class MMOGFXPlayer_Bag extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIBag            _Bag;
var MMOUIMoneyExchange  _MoneyExchange;

function RegisterBag( MMOUIBag mc )
{
	_Bag = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Bag);
}

function RegisterMoneyExchange( MMOUIMoneyExchange mc )
{
	_MoneyExchange = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_MoneyExchange );
}

// event UnRegister()
// {
// 	super.UnRegister();
// 
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_Bag);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_Bag);
// 
// 	class'MMOUIManager'.static.SetDeleteItemDelegate(none);
// 	class'MMOUIManager'.static.SetGetOpenBagDelegate(none);
// 	class'MMOUIManager'.static.SetTurntoPageDelegate(none);
// 	class'MMOUIManager'.static.SetLockItemDelegate(none);
// 	class'MMOUIManager'.static.SetUpdateBagCapSlotDelegate(none);
// 	class'MMOUIManager'.static.SetHintNewItemLabelDelegate(none);
// 	class'MMOUIManager'.static.SetDefaultItemSlotActionListener(none);
// 	class'MMOUIManager'.static.SetOpenBagDelegate(none);
// 
// }

// moved
event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)
{
	// Mouse wheel scroll, and not inside the UI
	if ((key == "MouseScrollUp" || key == "MouseScrollDown") && (strHoverControllerName == ""))
		return false;
	else
		return true;
}


// moved
event ClearItemSlot(GFxObject ItemSlot)
{
	local ECONTAINER_SLOT conslotId;
	local int slotId;
	conslotId = ECONTAINER_SLOT( ItemSlot.GetInt("containerID") );
	slotId = ItemSlot.GetInt("slotID");
	//Override this function; clearing slot attributes in bag uses ESPType_Bag
	SetItemSlotData( ItemSlot, none, ESPType_Bag , conslotId , slotId );
}

// moved
function ASBagTabChange( int tabId )
{
	if( _Bag != none )
	{
		_Bag.BagTab(tabId);
	}
}

// moved
function ASBagLabelChange( int labelId )
{
	if( _Bag != none )
	{
		_Bag.BagLabel(labelId);
	}
}

native function ASMoneyExchange();// moved

native function ASMoneyExchangeOk();// moved
native function ASMoneyExchangeInputChange();// moved

// moved
function RequireOpenStall()
{
	//class'MMOUIManager'.static.ShowPanelEx(class'MMOUIManager'.default.Panel_Stall_StallSetting,TRUE, "");
	//class'MMOUIManager'.static.RequireOpenStall();
}


native function ASLockItem();// moved
native function ASSplitItem();// moved
native function ASSortBag();// moved
native function ASEvaluteItem();// moved
native function ASSignatureItem();// moved

native function ASClickMoneyChain( bool selected );// moved

//Override this
native function SetItemSlotData( GFxObject mc , Item tItem, ESlotPanelType SlotType, optional ECONTAINER_SLOT ContainerID = E_CONSLOT_MAX, optional int SlotID = -1, optional EItemSlotState inISS = ISS_Normal); // moved
native function ASStartDrag(GFxObject srcItemSlot, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int srcSlotID); // moved

DefaultProperties
{
	MovieInfoStr="UI_BagWidget.BagWidget"
	Priority=5
}
