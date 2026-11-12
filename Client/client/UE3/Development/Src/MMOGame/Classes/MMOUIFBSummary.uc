class MMOUIFBSummary extends MMOUIBase
	native(UI);

enum E_FB_RankType{
	E_FB_SUM,
	E_FB_TIME,
	E_FB_SKILL,
	E_FB_ACHIVEMENT,
};

//
var bool bShowCloseBtn;

//var GFxObject Skip_btn;//skip button
var GFxObject ClearanceTimePt_mc;//score
var GFxObject ClearanceTime_mc;//seconds
//Kill technique
var GFxObject KillSkillPt_mc;//score
var GFxObject Killed_mc;
var GFxObject Destroyed_mc;
var GFxObject Strategy_mc;
//Achievement
var GFxObject AccomplishPt_mc;//score
var GFxObject AccomplishPanel_mc;
//Overall rating
var GFxObject GradedExp_mc;
//Extra bonus
var GFxObject VIPExp_mc;
var GFxObject GuildExp_mc;
var GFxObject ActivityExp_mc;
var GFxObject FirstTimeExp_mc;
var GFxObject CouplesExp_mc;
var GFxObject SumUpExp_mc;
//
var GFxObject LevelUp_mc;
var GFxObject ExpBar_mc;
var GFxObject ExpBarTitle_mc;
//
var int ExpBarMaxWidth;
var bool bShowLevelUp;
//var int iLastLevel;

native function PlayerLevelUp(int NewLv);
native function PlayerExpChange( int CurrentExp, int MaxExp,INT ChangeExp );
native function UpdateSummaryPoints();

//
final native function UpdateData();
event CallASSetRank( int iRankType, int iRank )
{
	ActionScriptVoid( "SetFBRank" );
}
event CallASStartExpBar()
{
	ActionScriptVoid( "StartShowExpBar" );
}
event CallASSetCurExpPercent( int iPercent )
{
	ActionScriptVoid( "SetCurExpPercent" );
}
native function TimeUpToClose();
//
native function PlayAnimInTime( int time );
//
native function OpenInTime( int time );
//
event CallASStartCloseCountDown( int CloseTime)
{
	ActionScriptVoid( "StartCloseCountDown" );
}
function LevelUpHintShow()
{
	LevelUp_mc.SetBool( "visible", bShowLevelUp );
	bShowLevelUp = false;
}
//
native function UpdateFBAchievement();
event UCCallSetFBAchievement( int idx, string achievementName, string desc, int achievementType )
{
	ActionScriptVoid( "SetFBAchievement" );
}

//===============================================================================================================
//===============================================================================================================
//called by AS to display the FBAward and begin to play
native function NextToPlayFBAward();
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	//FriendlyName=FBSummary
	IsDynamicLoad=true
	bShowLevelUp=false
	bShowCloseBtn=false
	CloseOnEsc=false
}