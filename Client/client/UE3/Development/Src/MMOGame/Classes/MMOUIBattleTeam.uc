class MMOUIBattleTeam extends MMOUIBase
	native(UI)
	config(UIVisibleMask);

enum BattleTeamPage
{
	BattleTeamPage_1v1,
	BattleTeamPage_5v5,
};

var const int TeamMemberNum;

//var GFxObject BattleTeam;
var GFxObject TeamNameTxt;
var GFxObject TotalScore;
var GFxObject BattleTable[3];

var BattleTeamPage PageType;

event InitOnce()
{
		TeamNameTxt = GetObject( "TeamNameTxt" );
		BattleTable[0] = GetObject("BattleTable0");
		BattleTable[1] = GetObject("BattleTable1");
		BattleTable[2] = GetObject("BattleTable2");
		TotalScore = GetObject("TotalScore");
		super.InitOnce();
}

native function NativeInitOnce();
//Battle team
native function UpdateBattleMemberList();
native function UpdateBattleTeamInfo();
native function AcceptJoinBattleTeam( GFxObject mc );
native function InviteEnterTeam( int roleID , string roleName );

event CallSetPanelText(GFxObject data)
{
	ActionScriptVoid("SetPanelText");
}

event CallUpdateTeamList(GFxObject data)
{
	ActionScriptVoid("UpdateTeamList");
}

event CallUCTurnOneToOne(GFxObject data)
{
	ActionScriptVoid("UCTurnOneToOne");
}

event CallUCTurnFiveToFive(GFxObject data)
{
	ActionScriptVoid("UCTurnFiveToFive");
}

//===============================================================================================================
//===============================================================================================================
native function ASBattleTeamTurnOneToOne();
native function ASBattleTeamTurnFiveToFive();
native function ASBattleTeamRightClickMember( int roleId , int x , int y );
native function ASBattleTeamCheckName( string teamName );
native function ASBattleTeamAddMember();
native function ASBattleTeamLeave();
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	//FriendlyName=BattleTeam
	IsDynamicLoad=true
	TeamMemberNum=7
}
