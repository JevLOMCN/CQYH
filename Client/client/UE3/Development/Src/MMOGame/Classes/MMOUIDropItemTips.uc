/********************************************************************
	Created:	2013/06
	Filename: 	UMMOGFXPlayer_PickWidget
	file ext:	uc
	Author:		hemu@snda
	Purpose:	Drop pickup Panel-MMOUIDropItemTips
*********************************************************************/

class MMOUIDropItemTips extends MMOUIBase
	config(MMOGame)
	native(UI);

// struct native TipCtrl
// {
// 	var bool          IsUsed;
// 	var int           Id;
// 	var GFxObject     Tip;
// };

// var config bool IsAutoHide;
// var array<TipCtrl> TipCtrlPool;
// var array<bool> TipsGridUsed;
//Grid
// var const int GridHeight;
// //var const int GridWidth;
// //Closest to the main character
// // var PickableItem BaseItem;
// //Number of grid cell rows
// var int MaxRow;
// //Number of grid cell columns
// var int MaxColumn;


// var MMOPlayer           MainPlayer;
// var MMOPlayerController MainPlayerController;
// var config float        TipAutoHideTime;
// var config bool         EnableAutoGridTip;
// var config int          MaxTipCtrlPoolSize;
// var int                 TipCtrlPoolSize;
// var config int          DefaultTipCtrlPoolIncrement;
// var float               TipRemainedShowTime;

// native function Toggle( );
// native function EnterScene();
// native function Update(  Canvas cvs ,float deltaTime );
// native function OnShutDown();

// native final function bool Validate();
// native final function SelectScreenPos( out PickableItem item,Canvas cvs);
// native final function float CheckNoOverlap ( PickableItem DrawnItem, PickableItem Item );
// native final function TipsPickItem( int ItemID );
// native final function NativeIni( );

// native final function DisableDropItemTip( int id );
// native final function DisableAll( );

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad( pl );

// 	MaxColumn = 800/GridHeight;
// 	TipsGridUsed.Add( MaxRow * MaxColumn );
// 	MainPlayerController = MMOPlayerController( GetPC() );
// 	MainPlayer = MMOPlayer( MainPlayerController.Pawn );
// 	class'MMOUIManager'.static.SetToggleDropItemTipsDelegate( Toggle );
// 	NativeIni();
}

native final function Pickup( int ItemID );

event SetHpInfo( GFxObject target, int id, String str, int txColor, String strIcon, int quality ) //VisibleType, 0 hidden, 1 shown, 2 show only the HP bar without numbers
{
	target.ActionScriptVoid("SetTipContent");
}

native function bool SkipCaptureInputKey(String key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);


DefaultProperties
{
// 	TipCtrlPoolSize=0
// 	GridHeight=20
 	CloseOnEsc=false
// 	DefaultVisible=true
}
