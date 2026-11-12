class MMOUIMenuBar extends MMOUIBase
	native(UI);

const MenuButtonNum = 11;
const ChildMenuButtonNum = 6;

var GFxObject MenuList;
var GFxObject MenuListChild;

var int OpenChildId;
var int RightMenuStartIdx;
var int MailCount;

var array<MenuInfo> MenuInfoList; 
var array<MenuInfo> RightMenuInfoList;
var transient GamePlayDataManager GpDataManager;

event InitOnce()
{

	super.InitOnce();
	GpDataManager = class'GamePlayDataManager'.static.Singleton();
	MenuInfoList = GpDataManager.MenuInfoList; //get the configured menu info
	RightMenuInfoList = GpDataManager.RightMenuInfoList;

	MenuList = GetObject("MenuList");
	MenuListChild = GetObject("MenuListChild");
	InitMenuSlot();
}

//native function ClickMenu( int idx );
//native function ClickMenuChild( int menuId , int childMenuId );
native function OverMenu( int idx );
native function OutMenu( int idx );

event CallUpdateMenuEffect( int idx , int childType , bool left , bool pl )
{
	ActionScriptVoid("updateMenuEffect");
}

event CallASSetNewMailCount( int idx , int count , bool right )
{
	ActionScriptVoid("UCSetNewMailCount");
}

event CallUpdateMaxMailCount( int idx , int count , bool right )
{
	ActionScriptVoid("UCUpdateMaxMailCount");
}

event CallMenuBarHintEffect( int mainMenuRealIdx, int childMenuType, bool left, bool isToPlay )
{
	ActionScriptVoid("updateMenuEffect");
}

event CallAddNewItemHint(GFxObject data)
{
	ActionScriptVoid("UCAddNewItemHint");
}

event CallAddGuideMenuEffect( int mainMenuRealIdx, int childMenuType, bool left, bool isToPlay, string guideName )
{
	ActionScriptVoid("UCGuideMenuEffect");
}

event CallClearGuideMenuEffect()
{
	ActionScriptVoid("clearGuide");
}

native function PlayerLevelChange( int lastLv , int newLv );
native final function OnNotifyNewSkillLearned( int uniqueSkillID );
native final function ShowMenuBarHint( EMenuType majorMenuType, EMenuType minorMenuType, bool isShow );

native function InitMenuSlot();
native function ClearUI();
native function bool SkipCaptureInputKey(String key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

native function UpdateMainMenuKeyDown(bool bRelease, int InCmdID, int InSlotID);

event CallSetLeftMenuData( GFxObject data )
{
	ActionScriptVoid("SetLeftMenuData");
}

event CallSetRightMenuData( GFxObject data )
{
	ActionScriptVoid("SetRightMenuData");
}

event CallShowNewSystem( GFxObject data )
{
	ActionScriptVoid("UCShowNewSystem");
}

event CallActivateNewSystem( GFxObject data )
{
	ActionScriptVoid("UCActivateNewSystem");
}

//===============================================================================================================
//===============================================================================================================
native function ASActivateNewSystem(int systemType);
native function ASShowNextNewSystem();

native function ASMenuClick( int menuId , int menuType );
native function ASMenuChildClick( int menuId , int menuType , int childMenuId , int childMenuType );

function ASMenuChildOver( int id )
{
	OverMenu(id);
}

function ASMenuChildOut( int id )
{
	OutMenu(id);
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
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	MailCount=0
	OpenChildId=-1
	CloseOnEsc=false
	PanelAllowFocus=false
}
