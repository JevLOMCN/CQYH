class MMOGFXPlayer extends GFxMoviePlayer
	native(UI)
	dependson(TypeDefine)
	config(MMOGame);

var protected native transient pointer EventAdapter{class IGameEventAdapter};
var transient array<MMOUIBase> UIObjectList;

var transient Mouse_State MouseState;

var bool        NeedInitSceneUI; 
var transient MMOPlayer   ShowAvatar;
var transient name FriendlyName;

var config string LockedSlotIconPath;

var bool IsLockedDynamicLoad;

var transient bool RegisterEndCalled; //TODOUI: TODOUI: remove this when done

enum EItemSlotState
{
	ISS_Normal,
	ISS_SlotLocked,  //Locked Slot (not unlocked)
	ISS_ItemLocked,     //Locked Item
	ISS_Unusable,   //Unusable Item
};

native function BindingWidgetsInfo();

event Init(optional LocalPlayer LocPlay) // oscar@snda
{
	BindingWidgetsInfo(); // This must be done first
	
	super.Init(LocPlay);
}

/**
 * Event triggered when focus is given to this MoviePlayer for a given LocalPlayer
 *
 * @param LocalPlayerIndex - The index of the local player that is now focusing on this MoviePlayer
 */
event OnFocusGained(int LocalPlayerIndex)
{
	local int i ;

	//`log("====================OnFocusGained: " @ self);
	
	for(i = 0; i < UIObjectList.Length; ++i)
	{
		//UIObjectList[i].SetVisible(inShow);
		//UIObjectList[i].ActionScriptVoid("OnFocusGained");
		UIObjectList[i].OnFocusGained();
	}
}

/**
 * Event triggered when focus is removed from this MoviePlayer for a given LocalPlayer
 *
 * @param LocalPlayerIndex - The index of the local player that is no longer focusing on this MoviePlayer
 */
event OnFocusLost(int LocalPlayerIndex)
{
	local int i ;

	//`log("====================OnFocusLost: " @ self);

	for(i = 0; i < UIObjectList.Length; ++i)
	{
		//UIObjectList[i].SetVisible(inShow);
		//UIObjectList[i].ActionScriptVoid("OnFocusLost");
		UIObjectList[i].OnFocusLost();
	}

	//ActionScriptVoid();
}

// added by zhaojun@snda
event bool CanFocusGained()
{
	local int i ;
	local bool bRet;

	bRet = true;

	for(i = 0; i < UIObjectList.Length; ++i)
	{
		if (!UIObjectList[i].CanFocusGained())
		{
			bRet = false;
			break;
		}
	}

	return bRet;
}

event bool CanFocusLost()
{
	local int i ;
	local bool bRet;

	bRet = true;

	for(i = 0; i < UIObjectList.Length; ++i)
	{
		if (!UIObjectList[i].CanFocusLost())
		{
			bRet = false;
			break;
		}
	}

	return bRet;
}

native function bool PanelSkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);
// added by zhaojun@snda
native function bool NativeSkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);
event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)
{
	//local int i;
	local bool caped, r;

	r = false;
	caped = false;

	//`log("====== captured key:"@ key @ eIE @ strHoverControllerName);

	if(eIE == IE_Pressed && key == "RightMouseButton" )
	{
		class'MMOUIManager'.static.ClearCurrentItemSlotActionListener();
	}
	else if(eIE == IE_Released && key == "LeftMouseButton" )
	{
		class'MMOUIManager'.static.ClearCurrentItemSlotActionListenerDelay(2);
	}

	// Mouse wheel scroll, and not inside the UI
	if ((key == "MouseScrollUp" || key == "MouseScrollDown") && (strHoverControllerName == ""))
	{
		//`log("	    ==== captured mouse wheel scroll, not inside UI key:" @ key @ eIE @ strHoverControllerName);
		return false;
	}
	else
	{
		//for(i = 0; i < UIObjectList.Length; ++i)
		//{
		//	caped = UIObjectList[i].SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
		//	//`log("	    ==== captured key test ui:" @ UIObjectList[i]  @ caped);
		//	if(caped)			
		//	{
		//		r = caped;
		//		//`log("	    ==== captured key:" @ UIObjectList[i]);
		//	}
		//}
		caped = PanelSkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
		if(caped)			
		{
			r = caped;
		}

		if(key == "PreTest")		
		{
			return true;
		}
		//
	}
	
	if(!r)
	{
		r = NativeSkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
	}
	
	//`log("	    ==== captured r:" @ r @ "key:"@ key @ eIE @ strHoverControllerName);
	return r;
}

native function bool SetFocusPanelByName(optional const name controllerName);


function ASOnFocusChange(int focused)
{
	//`log("====================ASOnFocusChange: " @ self @ focused);
}


// Tips
native function ASRollOverItemSlot(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault );
native function ASRollOutItemSlot( GFxObject mc);

native function ASShowTips_Tab(GFxObject mc);
native function ASHideTips_Tab(GFxObject mc);

native function ASShowTips_Attribute(GFxObject mc);
native function ASHideTips_Attribute();

native function ASShowTips_MenuBar(GFxObject mc);
native function ASHideTips_MenuBar(GFxObject mc);

native function ASShowTips_AwardIcon(GFxObject mc);
native function ASHideTips_AwardIcon(GFxObject mc);

native function ASShowTips_Other(GFxObject mc);
native function ASHideTips_Other(GFxObject mc);

native function ASShowTips_BuffSlot(GFxObject mc);
native function ASHideTips_BuffSlot(GFxObject mc);

native function ASShowTips_Building(GFxObject mc, int buildingType);
native function ASHideTips_Building(GFxObject mc);

native function ASShowTips_RuneActive(GFxObject mc);
native function ASHideTips_RuneActive(GFxObject mc);

native function ASShowTips_RuneButton(GFxObject mc);
native function ASHideTips_RuneButton(GFxObject mc);

native function ASShowTips_AvartaRuneButton(GFxObject mc);
native function ASHideTips_AvartaRuneButton(GFxObject mc);

native function ASShowTips_PlainText(GFxObject mc);
native function ASHideTips_PlainText(GFxObject mc);

native function ASShowTips_Text(GFxObject mc);
native function ASHideTips_Text(GFxObject mc);

native function ASShowTips_PlainText_WithString(GFxObject mc , string TipsString);

native function ASShowTips_OrderItem(GFxObject mc);
native function ASHideTips_OrderItem();

native function ASShowFreeTips(GFxObject mc , bool bShow);
native function ASShowReputionPanel(bool bShow);

native function ASOnPanelClose( string PanelName );
native function ASOnPanelHelp( string PanelName );
native function ASOnBringWindowTop(string PanelName);
native function ASSelectTrackForceID(int ForceID);

native function ASCopyToClipboard( string txt );
native  function string ASPasteFromClipboard();

// zhaojun@snda
native function ASFocusInTextInput(bool In);    // added by zhaojun@snda. moniter all the textfield
//~ zhaojun@snda

native function bool GetlockMouseCursorForUI();
native final function ClosePanel(GFxObject Panel);

// UI sound effects zhangrui05@snda
native function ASPlayUISound(string cueName);
//~ zhangrui05@snda


event CallSetItemData( GFxObject mc, int quantity, string IconName, int ItemIndex, ESlotPanelType SlotType, string Type , optional int itemColor = 0, optional ECONTAINER_SLOT ContainerID = E_CONSLOT_MAX, optional int SlotID = -1 )
{ 
	ActionScriptVoid("root.SetItemData");
}


//Fill ItemSlot according to Item
event CallSetItemSlotByItem( GFxObject ItemSlot,Item inItem, ESlotPanelType SlotType )
{ 
	if( inItem == none )
		CallSetItemData( ItemSlot,0,"",-1, SlotType, "", -1, 0, 0 );
	else
		CallSetItemData( ItemSlot,0,inItem.GetIconName(),inItem.ItemID, SlotType, "", inItem.ItemColor + 1,0,0);
}

native function SetItemSlotData( GFxObject mc , Item tItem, ESlotPanelType SlotType, optional ECONTAINER_SLOT ContainerID = E_CONSLOT_MAX, optional int SlotID = -1, optional EItemSlotState inISS = ISS_Normal );

event ClearItemSlot(GFxObject ItemSlot)
{
	SetItemSlotData( ItemSlot, none, ESPType_Invalid, E_CONSLOT_MAX, -1 );
}

native function SetNoninteractiveIconSlot( GFxObject mc, int itemID, string iconPath, string desc, int panelType );
native function SetNoninteractiveIconSlotByItem( GFxObject mc, Item item, string desc, int panelType );
native function SetIconShowByItem( GFxObject Slot_mc, Item SlotItem );
event ASSetSlotShowData( GFxObject mc, int quantity, string IconName,int ItemID, optional int itemColor = 0,optional ECONTAINER_SLOT ContainerID = E_CONSLOT_MAX, optional int SlotID = -1  )
{ 
	ActionScriptVoid("root.SetItemShowData");
}

event CallRemoveChildWidget( GFxObject ParentMc , GFxObject ChildMc )
{
	ActionScriptVoid("root.RemoveChildWidget");
}

function CallSetCombo( int ComboCount)
{
	ActionScriptVoid("root.SetCombo");
}

event CallWidgetDispatchEvent( GFxObject mc , string mouseevent)
{
	ActionScriptVoid("root.WidgetDispatchEvent");
}

function CallShotText( string sText ,int colorFont,int colorfilter,int Posx,int Posy,int  TextSize,int speedx,int speedy,int  Lenthx,int Lenthy )
{
	
	ActionScriptVoid("ShotText");

	//`log(sText);
}

function CallActiveChatBox( bool active )
{
	ActionScriptVoid("ActiveChatBox");
}

function CallShowAttack( int count, int Posx,int Posy,bool bBlock,int blockcount)
{
	ActionScriptVoid("root.ShowAttack");
}
function CallShowMiss( int Posx,int Posy)
{
	ActionScriptVoid("root.ShowMiss");
}

function CallShowCritical( int count, int Posx,int Posy)
{
	ActionScriptVoid("root.ShowCritical");
}

native function SetGfxFocus(GFxObject mc);
event CallSetStageFocus( GFxObject mc )
{
	ActionScriptVoid("root.SetStageFocus");
}

event CallGetMouseTopMostEntity()
{ 
	ActionScriptVoid("root.GetMouseTopMostEntity");
}

// final function native MMOUI::RegisterMoviePlayer(name inFriendlyName);
//final function native NativeGlobalRegisterUI(MMOUIBase UIMenu, name inFriendlyName);

native final function RegisterUI(MMOUIBase UIMenu, name inFriendlyName);
// {   
// 	`log("==================== RegisterUI:" @ UIMenu@inFriendlyName );
// 	NativeGlobalRegisterUI(UIMenu, inFriendlyName);
// 	if( UIMenu != none ){
// 		UIMenu.InitLoad(inFriendlyName, self);
// 		UIMenu.SetString("PanelName", string(inFriendlyName));
// 		UIObjectList.AddItem(UIMenu);
// 	}
// }

// event UnRegister()
// {
// 	ShowAllUI(false);
// }

//event bool IsAllHide()
//{
//	local int i ;
//	for(i = 0; i < UIObjectList.Length; ++i)
//	{
//		if(UIObjectList[i].IsVisible==true)
//		{
//			return false;
//		}
//	}
//	return true;
//}

final native function MMOUIBase FindUIPanel(name inFriendlyName);

native final function ShowUI(name inFriendlyName, optional bool inShow=true);
// {
// 	local MMOUIBase ui;
// 	ui = class'MMOUIManager'.static.FindUIPanel(inFriendlyName);
// 	ui.NativeShow(inShow);
// }

// event ShowAllUI(bool inShow=true)
// {
// 	local int i ;
// 	for(i = 0; i < UIObjectList.Length; ++i)
// 	{
// 		UIObjectList[i].SetVisible(inShow);
// 	}
// }

//After Login->Game, restore all UI default display states
native final function SetAllChildren2DefaultVisible();
// {
// 	local int i ;
// 	for(i = 0; i < UIObjectList.Length; ++i)
// 	{
// 		UIObjectList[i].NativeShow( UIObjectList[i].DefaultVisible );
// 	}
// }

function ToggleShowUI(name inFriendlyName)
{
	local MMOUIBase ui;
	ui = class'MMOUIManager'.static.FindUIPanel(inFriendlyName);
	ui.ToogleVisible();
}


event SetMouseState( Mouse_State newMouseState )
{
	if( MouseState != newMouseState )
	{
		switch( newMouseState )
		{
		case EMS_Normal:
			GetVariableObject("root.mouse_mc").GotoAndStop( "normal" );
			break;
		case EMS_Attack:	
			//SetHint(true,Texture2D'MobileResources.HUD.MobileHUDButton3',200,200);
			GetVariableObject("root.mouse_mc").GotoAndStop("attack" );
			break;
		case EMS_Click:
			GetVariableObject("root.mouse_mc").GotoAndStop( "click" );
			break;
		case EMS_Talk:
			GetVariableObject("root.mouse_mc").GotoAndStop( "talk" );
			break;
		case EMS_Pick:
			GetVariableObject("root.mouse_mc").GotoAndStop( "get" );
			break;
		case EMS_Buy:
			GetVariableObject("root.mouse_mc").GotoAndStop( "buy" );
			break;
		case EMS_Sell:
			GetVariableObject("root.mouse_mc").GotoAndStop( "click" );
			break;
		default:
			GetVariableObject("root.mouse_mc").GotoAndStop( "normal" );
			break;
		}
		MouseState = newMouseState;
	}
}

native final function RegisterEnd();

// native function NativeInitMoviePlayer();
// function InitMoviePlayer()
// {	
// 	local int i;
// 
// 	for(i = 0;i< UIObjectList.Length;i++)
// 	{
// 		//if( UIObjectList[i].NeedFirstInit )
// 		//{
// 			UIObjectList[i].InitOnce();
// 		//	UIObjectList[i].NeedFirstInit = false;
// 		//}
// 		UIObjectList[i].InitWidget();
// 	}	
// 
// // 	NativeInitMoviePlayer();
// }

native final function ASTrace(string v);

native function ASDebugBreak(GFxObject sender);
native final function ASLinkCallBack( GFxObject paraArr , int buttonIdx );
native final function ASLinkOverCallBack(GFxObject paraArr);
native final function ASLinkOutCallBack(GFxObject paraArr);
	
////////////////////////////////////////////Common interfaces add by JimZhang@snda.com//////////////////////////////////////////////////////
native function DebugString(string str);

//Set text by text ID
native function ASGetStringByID(GfxObject mc, string menberID,string textID);
native function SetTextByID(GfxObject mc, string textID);
native function SetLabelByID(GfxObject mc, string textID);

//Show time
native function TimeDisplay(GfxObject mc, int seconds);

//Get death countdown time, in seconds
native function GetReviveCountDown(GfxObject mc, string resName);

//Get map type.
native function GetCurrentMapType(GfxObject mc, string resName);

//Get the count of items with a given ID (in bag)
native function GetItemCountByID(GfxObject mc, string resName,int itemID );

//Get the icon name of the item with a given ID
//native function GetItemIconByID(GfxObject mc, string resName,int itemID );

//Get the icon name of the ability with a given ID
native function GetSkillIconByID(GfxObject mc, string resName,int skillID );

//Get gold class 1
native function GetPlayerMoney(GfxObject mc, string resName);

//Get gold class 2
native function GetPlayerGold(GfxObject mc, string resName);

//
native function String GetASHeadIconFrame(EPlayerClass playerClass, bool isFemale );

function LeaveGame(GFxObject mc)
{
	LeaveGameWithConfirm(mc);
}

// Exit the game directly without a dialog
function LeaveGameWithoutConfirm(GFxObject mc)
{
	GetPC().ConsoleCommand("exit");
	//LeaveGame();
}

//Head bubble display time ended
function BubbleShowedOver( GFxObject mc )
{
}

// A dialog pops up, exit the game after confirmation
native final function LeaveGameWithConfirm(GFxObject mc);

function ASHideTips()
{
	class'MMOUIManager'.static.HideTips();
}

event McGotoAndPlay(GFxObject mc,string FrameName)
{
	mc.GotoAndPlay(FrameName);
}

//function MouseLeftPressItem(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault)
//{
//	local IItemSlotActionListener dftLsn, curLsn;

//	`log("--------------------------====================SlotType:  " @ SlotType @";ContainerID:"@ContainerID@";SlotID"@SlotID  );

//	dftLsn = class'MMOUIManager'.static.GetDefaultItemSlotActionListener();
//	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();

//	if(curLsn != none && curLsn.OnLeftMousePress(mc, SlotType, ContainerID, SlotID))	
//	{
//		return;
//	}

//	if(dftLsn != none)
//	{
//		dftLsn.OnLeftMousePress(mc, SlotType, ContainerID, SlotID);
//	}
//}

//function MouseLeftClickItem(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault)
//{
//	local IItemSlotActionListener dftLsn, curLsn;
//	dftLsn = class'MMOUIManager'.static.GetDefaultItemSlotActionListener();
//	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();

//	if(curLsn != none && curLsn.OnLeftMouseClick(mc, SlotType, ContainerID, SlotID))	
//	{
//		return;
//	}

//	if(dftLsn != none)
//	{
//		dftLsn.OnLeftMouseClick(mc, SlotType, ContainerID, SlotID);
//	}
//}

//New slot control interface
native final function ASSlot_MouseLeftPress(GFxObject mcSlot);
native function ASSlot_StartDrag(GFxObject mcSlot);
native final function ASSlot_ReleaseDrag(GFxObject mcSlotFrom,GFxObject mcSlotTo);

native final function ASSlot_MouseLeftClick(GFxObject mcSlot);
native final function ASSlot_MouseRightClick(GFxObject mcSlot);

native final function ASDoubleClickSlot(GFxObject SrcSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID);

//native final function ASSlot_DragDrop(GFxObject mcSlot);
//native final function ASSlot_StartDrag(GFxObject mcSlot);
//native final function ASSlot_DragRelease(GFxObject mcSlot);




native final function MouseLeftPressItem(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native final function MouseLeftClickItem(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);

native final function MouseRightClickItem(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native final function MouseRightClickBuff(GFxObject mc, int curBuffID, int curBuffSlotID);

//{
//	local IItemSlotActionListener dftLsn, curLsn;
//	dftLsn = class'MMOUIManager'.static.GetDefaultItemSlotActionListener();
//	curLsn = class'MMOUIManager'.static.GetCurrentItemSlotActionListener();

//	if(curLsn != none && curLsn.OnRightMouseClick(mc, SlotType, ContainerID, SlotID))	
//	{
//		return;
//	}

//	if(dftLsn != none)
//	{
//		dftLsn.OnRightMouseClick(mc, SlotType, ContainerID, SlotID);
//	}
//}

//Movie loads earlier than myHUD is set, so responses that need the HUD are done here
native virtual function OnHUDLoaded();

native function OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
native final function GFxObject GetDragIcon();

native function ResetDragState();
native function ASStartDrag(GFxObject SrcItemSlot, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID);

native function ASDragOver(GFxObject SrcItemSlot, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID);
//{
//}

native function ASDragOut(GFxObject SrcItemSlot, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID);
//{
//}

native function NativeASDragRelease(GFxObject SrcItemSlot, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID);

native function ASDragRelease(GFxObject SrcItemSlot, ESlotPanelType SrcSlotType,ECONTAINER_SLOT SrcContainerID,int SrcSlotID);
//native function ASDragRelease(GFxObject SrcItemSlot, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, GFxObject TopmostItemSlot, ESlotPanelType TopmostSlotType, ECONTAINER_SLOT TopmostContainerID, int TopmostSlotID);
//native function ASDragRelease_ToTopMost(GFxObject SrcItemSlot, ESlotPanelType SrcSlotType,ECONTAINER_SLOT SrcContainerID,int SrcSlotID);
native function ASGetMouseTopMostEntity(GFxObject TopMostObject);


event LevelStreamingUIShow GetShowLevel( const String LevelName )
{
	local MMOGame game;
	game = MMOGame(GetPC().WorldInfo.Game);
	if( game != none )
	{
		return game.GetUILevel( LevelName );
	}
	return none;
}

event LevelStreamingUIShow LoadShowLevel( const String LevelName, const Vector Offset )
{
	return GetPC().WorldInfo.LoadUILevel( LevelName, Offset );
}

function RemoveShowLevel( LevelStreamingUIShow Level )
{
	GetPC().WorldInfo.RemoveUILevel( Level );
}


native final function CaptureAllKeys();
native final function CaptureKeyboardKeys();

// add by zhangrui05@snda
native final function ASCloseGuidePanel(int id);

DefaultProperties
{
	SoundThemes(0)=(ThemeName=default, Theme=UISoundTheme'MMO_UISoundTheme.Default')
	SoundThemes(1)=(ThemeName=Advanced, Theme=UISoundTheme'MMO_UISoundTheme.Advanced')
	

	IsLockedDynamicLoad=false
	bAutoPlay=true	
	NeedInitSceneUI=false	
	bCloseOnLevelChange=false
	Priority=5
}
