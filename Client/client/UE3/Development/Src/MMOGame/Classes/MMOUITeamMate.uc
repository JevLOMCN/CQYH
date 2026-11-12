class MMOUITeamMate extends MMOUIBase
	native(UI);

var const int LineNum;
var const int LargeLineNum;

var const float UpdateInterval;
var float CurTime;
//var TeamManager TM;
var int HPFullWidth;
var int MPFullWidth;
var int LargeHpWidth;

var GFxObject NormalTeamMatePanel;
var GFxObject LargeTeamMatePanel;

var GFxObject confirmLeaveTeam;
var GFxObject confirmMoveLeader;
var bool IsShow;

//function UnitWidget()
//{
//	local int i;
//	super.UnitWidget();

//	for( i = 0 ; i < LineNum ; i++)
//	{
//		GetObject("tag_teamleader"$i).SetVisible(false);
		
//	}
//}

native function ClearUI();

native function UpdateTeamMateNum( int TeamMateNum );


event InitOnce()
{	
	super.InitOnce();

	LargeHpWidth = 110; 
}

native function UpdateAllTeamMate(  );
native function UpdateTeamMate( int UIPos , int Pos );

/*
	Popup confirm
*/
native function ClickMoveLeaderConfirmOK();
native function ClickMoveLeaderConfirmCancel();
native function ClickLeaveTeamConfirmOK();
native function ClickLeaveTeamConfirmCancel();

final native function ASTeamMemberRightClick( int x , int y , int index );
final native function ASTeamMemberLeftClick( int x , int y , int index );
final native function ASShowTeammateHint( GFXObject mc, int index );
final native function ASHideTeammateHint();

event UCUpdateTeamMenber(GfxObject dat)
{
	ActionScriptVoid("UCUpdateTeamMenber");		
}
DefaultProperties
{
	LineNum=4
	LargeLineNum=10
	UpdateInterval=0.5
	CurTime=0
	DefaultVisible=true
	CloseOnEsc=false
	IsShow=true
}
