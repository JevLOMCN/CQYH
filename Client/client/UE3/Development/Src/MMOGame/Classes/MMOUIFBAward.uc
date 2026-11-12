class MMOUIFBAward extends MMOUIBase
	native(UI);

enum EAOG_FBAward
{
	enumAOGFbAwardType1,
	enumAOGFbAwardType2,
	enumAOGFbAwardType3
};

var GFxObject MyName_mc;
var GFxObject MyPortrait_mc;
var GFxObject MyScore_mc;

var const int MAX_FB_Award;//3
var int bAwardAnimPlayed[3];
var GFxObject MyFBAward[3];
var GFxObject MyBtnAward[3];
var GFxObject MyAwardHintTf[3];

var GFxObject MemberInfoMc;
var GFxObject MemFBAwards[4];

//Whether the third prize can be drawn for free (VIP player)
var bool bCanFreeGet3;
//Whether the second prize can be drawn for free (rating S or above)
var bool bCanFreeGet2;
//Whether no draw has been made in this dungeon
var bool bPlayed;
//
var array<int> MyClickAwardIdx;
//Update
native function UpdateMemberAward();
native function UpdateMyAward();
//
native function InitFBAwardMode();
native function ResetUI();
//Start the draw (draw?)
native function OpenToPlay( int iTime );
//
native function ResetData();
//
event CallASStartPlayCountDown( int CloseTime)
{
	ActionScriptVoid( "StartPlayCountDown" );
}
//Draw button was clicked
native function ClickAward( int iIdx );
//Request prize from server
native function GetAward( int iIdx );
//Time is up, open all rewards that can be opened
native function TimeUpToShowAll();

//===============================================================================================================
//===============================================================================================================
native function ClickGetFBAward( int iIdx );
//Back to FBSummary from FBAward
native function FBAwardPlayTimeOver();
//
native function GetFBAward( int iIdx);

//===============================================================================================================
//===============================================================================================================

//
DefaultProperties
{
	//FriendlyName=FBAward
	MAX_FB_Award=3
	IsDynamicLoad=true
	CloseOnEsc=false
}