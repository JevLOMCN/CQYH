class MMOUIArenaState extends MMOUIBase
	native(UI);

var int PanelWidth;

final native function UpdatePanel();
final native function TryNextRound();

//Set arena countdown
event CallASSetArenaTimeCntDown( int countDownTime )
{
	ActionScriptVoid( "UCSetBattleTimeLeftText" );
}

//Battle record
event CallASSetArenaWin( String winCntText )
{
	ActionScriptVoid( "UCSetTipsRecordText" );
}

//
event CallASSetArenaRoundTime( String title, int countDownTime )
{
	ActionScriptVoid( "UCSetProgressText" );
}

//Operation button settings
event CallASShowOperateBtn( String btnText, bool isToShow )
{
	ActionScriptVoid( "UCSetFightButton" );
}

//Set UI hint text
event CallASSetPanelHintTxt( String strHint, int roundLeftTime = -1 )
{
	ActionScriptVoid( "UCSetProgressText" );
}

////
//event CallASSetArenaState( int stateType )
//{
//	ActionScriptVoid( "UCSetArenaState" );
//}

//===============================================================================================================
//===============================================================================================================
//function RegisterArenaState( MMOUIArenaState mc )
//{
//	_ArenaState = mc;
//	RegisterUI( mc, class'MMOUIManager'.default.Panel_Arena_ArenaState);
//}

native function bool SkipCaptureInputKey( string key, EInputEvent eIE, string strHoverControllerName, Vector2D ClickPosInMovieSpace);

native function ASOnFightingButtonClick();
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	//FriendlyName=ArenaState
	PanelWidth=220
	IsDynamicLoad=true
	CloseOnEsc=false
}
