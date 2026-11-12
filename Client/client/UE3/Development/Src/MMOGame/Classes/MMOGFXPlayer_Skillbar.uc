class MMOGFXPlayer_Skillbar extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUISkillbar            Skillbar;
var MMOUIMenubar            _Menubar;
//var MMOUIMsgButtonBar       _MsgButtonBar;
var MMOUIBuffPanel          _Buff;

function RegisterSkillbar( MMOUISkillbar mc )
{	
	Skillbar = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Skillbar);
}

//function RegisterMenuBar( MMOUIMenubar mc )
//{
//	_Menubar = mc;
//	RegisterUI( mc, class'MMOUIManager'.default.Panel_Skillbar_Menubar);
//}

//function RegisterMsgButtonBar( MMOUIMsgButtonBar mc )
//{
//	_MsgButtonBar = mc;
//	RegisterUI( mc, class'MMOUIManager'.default.Panel_Skillbar_MsgButtonBar );
//}


function RegisterBuff( MMOUIBuffPanel mc )
{
	_Buff = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Skillbar_Buff);
}


native function OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
native function ASStartDrag(GFxObject srcItemSlot, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int srcSlotID);
native function ASSlot_StartDrag( GFxObject mcSlot);

native function UpdateSkillBarKeyDown(bool bRelease, int inCmdID, int slotCmdKey );

native function ASClickShoppingMallBtn();
native function ASClickActivityBtn();
native final function ASClickMailIcon();

event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)
{
	local bool res;

	res = _Buff.SkipCaptureInputKey( key,  eIE,  strHoverControllerName, ClickPosInMovieSpace);
	if(res==true)return true;

	res = SkillBar.SkipCaptureInputKey( key,  eIE, strHoverControllerName, ClickPosInMovieSpace);
	if(res==true)return true;

	res = _Menubar.SkipCaptureInputKey( key,  eIE, strHoverControllerName, ClickPosInMovieSpace);
	if(res==true)return true;

	return super.SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

native function ASOnPanelClose( string PanelName );

event AddBuffPanel(int buffSlotID,string buffIcon,int buffMaxTime,int buffCurTime, int stackCount, int ownerID, int buffID,int showTimeUp,bool debuff)
{
	_Buff.Open( buffSlotID, buffIcon,  buffMaxTime, buffCurTime,stackCount,ownerID,buffID,showTimeUp,debuff);
}
event DeleteBuffPanel(int buffSlotID)
{
	_Buff.Close( buffSlotID );
}

native function ASOnClickNewSkillLearnedBtn();

native function FlashHPBar();

//Ability settings
native function ASShowTips_SkillSettingActive( GFxObject mc );
native function ASHideTips_SkillSettingActive( GFxObject mc );
native function ASSetSkill( int skillKeyType, int skillID );
native function ASCloseSkillList();

native function ASShowPanel( int idx );
native function ASHideShortcutButtons();

//New system opened, add to menu bar
native function ASActivateNewSystem(int systemType);
native function ASShowNextNewSystem();

//native function ASMenuClick( int menuId , int menuType );
//native function ASMenuChildClick( int menuId , int menuType , int childMenuId , int childMenuType );
native function UpdateMainMenuKeyDown(bool bRelease, int InCmdID, int InSlotID);

function ASMenuChildOver( int id )
{
	_Menubar.OverMenu(id);
}

function ASMenuChildOut( int id )
{
	_Menubar.OutMenu(id);
}

function ASChangeMenu(GFxObject SrcMc ,GFxObject DesMc)
{
	`log( "~~~~~~~~~!!!!!!!!!!!!!!~~~~~~~~~~~No MMOGFXPlayer_Skillbar::ASChangeMenu function" );
	//SkillBar.ChangeMenu( SrcMc , DesMc );
}

function  ASThrowMenu(GFxObject mc)
{
	`log( "~~~~~~~~~!!!!!!!!!!!!!!~~~~~~~~~~~No MMOGFXPlayer_Skillbar::ASThrowMenu function" );
	//SkillBar.ThrowMenu( mc );
}

function ASSwitchMenuBar()
{
	`log( "~~~~~~~~~!!!!!!!!!!!!!!~~~~~~~~~~~No MMOGFXPlayer_Skillbar::ASSwitchMenuBar function" );
	//SkillBar.SwitchMenuBar();
}

event CallUpdateMenuEffect( int idx , int childType , bool left , bool pl )
{
	_Menubar.CallUpdateMenuEffect( idx,  childType, left, pl );
}

DefaultProperties
{
	//bAllowFocus=false
	MovieInfoStr="UI_SkillbarWidget.SkillbarWidget"
	bCloseOnLevelChange=FALSE
	Priority=2 // Keep this at the bottom to receive keyboard and mouse events
}
