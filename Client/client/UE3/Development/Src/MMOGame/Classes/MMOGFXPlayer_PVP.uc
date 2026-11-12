class MMOGFXPlayer_PVP extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIPVP _PVP;
var MMOUIPVPBattle _PVPBattle;
var MMOUIArenaQueue _ArenaQueue;
var MMOUIArenaScore _ArenaScore;
var MMOUIArenaHint _ArenaHint;

function RegisterArenaQueue( MMOUIArenaQueue mc )
{
	_ArenaQueue = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_PVP_ArenaQueue);
}

function RegisterArena( MMOUIPVP mc )
{
	_PVP = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_PVP_Arena);
}

function RegisterPVPBattle( MMOUIPVPBattle mc )
{
	_PVPBattle = mc;
	RegisterUI( mc , class'MMOUIManager'.default.Panel_PVP_PVPBattle );
}

function RegisterArenaScore( MMOUIArenaScore mc )
{
	_ArenaScore = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_PVP_ArenaScore );
}

function RegisterArenaHint( MMOUIArenaHint mc )
{
	_ArenaHint = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_PVP_ArenaHint );
}

native function PostAdvance( float dt );

// event UnRegister()
// {
// 	super.UnRegister();
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_PVP_ArenaQueue);
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_PVP_PVP);
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_PVP_ArenaScore);
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_PVP_PVPBattle);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_PVP);
// 
// 	class'MMOUIManager'.static.SetOpenArenaQueueDelegate(none);
// 	class'MMOUIManager'.static.SetLeaveArenaQueueDelegate(none);
// 	class'MMOUIManager'.static.SetAskEnterArenaDelegate(none);
// 
// 	class'MMOUIManager'.static.SetOpenArenaScoreSingleDelegate(none);
// 	class'MMOUIManager'.static.SetCloseArenaScoreDelegate(none);
// }
//
//function TogglePVP()
//{
//	if( !_PVP.IsVisible ){
//		_PVP.NativeShow( TRUE );
//	}
//	else{
//		_PVP.NativeShow( FALSE );
//	}
//}
//
function ASQuickSelectBattle()
{
	_PVPBattle.QuickSelectBattle();
}
//
function ASSelectBattle( int idx )
{
	_PVPBattle.SelectBattle( idx );
}
//Random battlefield list checkbox
function ASToggleBattle( int id, bool isSelected )
{
	_PVPBattle.MultiSelectBattle( id, isSelected );
}
//
function ASBattlePageUp()
{
	_PVPBattle.BattlePageUp();
}
//
function ASBattlePageDown()
{
	_PVPBattle.BattlePageDown();
}
//
function ASTeamJoinBattle()
{
	//_PVP.TeamJoinBattle();
	_PVPBattle.JoinSingleBattle();
}

function ASBattleTeamLeave()
{

}
//
function ASSingleJoinBattle()
{
	//_PVP.SingleJoinBattle();
	_PVPBattle.JoinSingleBattle();
}
//
function ASLeaveBattle()
{
	_PVPBattle.LeaveBattle();
}
//

//function ASArenaQueueLeave()
//{
//	_ArenaQueue.ClickLeave();
//}

//function ASArenaQueueClickIcon()
//{
//	_ArenaQueue.ClickIcon();
//}

//function ASArenaQueueMinimize()
//{
//	_ArenaQueue.Minimize();
//}

function ASArenaEnter()
{
	_PVP.ArenaEnter();
}

//function ASArenaHintClickIcon()
//{
//	_ArenaHint.ClickIcon();
//}

//function ASArenaHintLeave()
//{
//	_ArenaHint.ClickLeave();
//}


native function ASArenaLeave();


//function ASBattleTeamCreateOk()
//{
//	_BattleTeamCreate.CreateTeam();
//}
////
//function ASBattleTeamClickFlagBg( int index )
//{
//	_BattleTeamCreate.FlagBgChange( index );
//}
////
//function ASBattleTeamClickFlagIcon( int index )
//{
//	_BattleTeamCreate.FlagIconChange( index );
//}


DefaultProperties
{
	MovieInfoStr="UI_PVP.PVP"
	Priority=5
}
