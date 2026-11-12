class MMOUIArenaEntrance extends MMOUIBase
	native(UI);

//
enum ArenaEntrPanelSlotType
{
	EArenaSlot_Invalid,
	EArenaSlot_ViewSlot1,
	EArenaSlot_ViewSlot2,
	EArenaSlot_ViewSlot3,
	EArenaSlot_ViewSlot4,
	EArenaSlot_ResultSlot1,
};
//
final native function ApplyForArena();
final native function CanelApplyForArena();
final native function ToGetArenaReward();
//
final native function UpdatePanelBaseInfo();

final native function UpdateArenaData();

//Title area
event CallASSetPanelTopTitleText( String text )
{
	ActionScriptVoid( "UCSetTopTipsText" );
}

event CallASSetPanelTopText( String text, bool isToShowThis )
{
	ActionScriptVoid( "UCSetTopTipAdditionalText" );
}

//Final reward
final native function UpdateArenaWinResult();
event CallASSetArenaProcess( GFxObject data )
{
	ActionScriptVoid( "UCSetBattleResult" );
}

event CallASInitResultRewardItemSlot( int idx, int slotType, int inPanelIdx )
{
	ActionScriptVoid( "InitResultRewardItemSlot" );
}

//
final native function InitArenaAwards();

event CallASInitArenaAwards( GFxObject data )
{
	ActionScriptVoid( "UCInitArenaAwards" );
}

event CallASSetArenaAwardLevel( int level, float thisLevelPecent )
{
	ActionScriptVoid( "UCSetEntryRewardsRank" );
}

//
final native function UpdateArenaMnyReward();

event CallASUpdateCurWinMoney( GFxObject data )
{
	ActionScriptVoid( "UCSetBattleRewardsRankInfo" );
}

//Treasure chest reward
event CallASInitRewardViewItemSlot( int idx, int slotType, int inPanelIdx )
{
	ActionScriptVoid( "InitRewardViewItemSlot" );
}

event CallASSetAllArenaDefaultRewardInPreview( GFxObject data )
{
	ActionScriptVoid( "UCSetRewardItemListData" );
}

//
event CallASUpdateArenaTotalMoney( String mnyStr )
{
	ActionScriptVoid( "UCSetTotalBonusText" );
}

//
event CallASSetPlayerName( String playerName )
{
	ActionScriptVoid( "UCSetPlayerNameText" );
}

event CallASUpdateMyArenaCost( String mnyStr )
{
	ActionScriptVoid( "UCSetTotalCostText" );
}

//Entry button settings
event CallASSetArenaEnterBtn( String btnStr, bool isToShowThis )
{
	ActionScriptVoid( "UCSetActionButton" );
}

//Text at the bottom of the UI (entry costs, registration succeeded)
event CallASSetPanelBottomText( String titleStr )
{
	ActionScriptVoid( "UCSetBottomTipsText" );
}

//Entry fee
event CallASSetArenaEntryFee( String mnyStr, bool isToShowThis )
{
	ActionScriptVoid( "UCSetTicketCostText" );
}

//===============================================================================================================
//===============================================================================================================
//var MMOUIArenaEntrance _ArenaEntrance;
//function RegisterArenaEntrance( MMOUIArenaEntrance mc )
//{
//	_ArenaEntrance = mc;
//	RegisterUI( mc, class'MMOUIManager'.default.Panel_Arena_ArenaEntrance);
//}

native function bool SkipCaptureInputKey( string key, EInputEvent eIE, string strHoverControllerName, Vector2D ClickPosInMovieSpace);

native function ASOnActionButtonClick();
native function ASToggleShowBonusPanel( bool isToShow );
//===============================================================================================================
//===============================================================================================================

//
DefaultProperties
{
	//FriendlyName=ArenaEntrance
	IsDynamicLoad=true
}
