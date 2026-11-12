class MMOGFXPlayer_FBEndAll extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIFBSummary              _FBSummary;
var MMOUIFBAward                _FBAward;
var MMOUIFBBattleComfirm        _FBBattleConfirm;

var const int FBEndPreMSec;
var const int FBSummaryMSec;
var const int FBAwardMSec;
var const int FBEndTotalMSec;
// event UnRegister()
// {
// 	super.UnRegister();
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_FBEndAll_FBSummary);
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_FBEndAll_FBAward);
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_FBEndAll_FBBattleComfirm);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_FBEndAll);
// }


//Dungeon settlement
function RegisterFBSummary( MMOUIFBSummary mc )
{
	_FBSummary = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_FBEndAll_FBSummary);
}
//Dungeon lottery
function RegisterFBAward( MMOUIFBAward mc )
{
	_FBAward = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_FBEndAll_FBAward);
}
//Dungeon exit choice
function RegisterFBBattleComfirm( MMOUIFBBattleComfirm mc )
{
	_FBBattleConfirm = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_FBEndAll_FBBattleComfirm);
}
//
native function WaitFBtoSummary();
native function ShowCountDownBar();

//see the summary, after the time specified the UI will close
native function ReadyToFBEndAll();
//see the summary, after the time specified the UI will close
native function ReviewFBSummary();
//see the summary displayed last time, no time limited
native function OpenFBSummary();
native function HideFBSummary();
//called by AS to display the FBAward and begin to play
native function NextToPlayFBAward();
//ship the FBSummay UI to show the FBAward, but not to play
native function OpenFBAward();
native function HideFBAward();
//
native function ClickGetFBAward( int iIdx );
//Back to FBSummary from FBAward
native function FBAwardPlayTimeOver();
//
native function OpenFBBattleComfirm();
//
native function GetFBAward( int iIdx);
//
native function SendPlayFBAgain();
native function WantPlayFbAgain();
native function SendLeaveFB();
native function LeaveFB();
//Close dungeon rating, dungeon lottery, dungeon exit reset windows
native function CloseAllFBEndUI();
//End
DefaultProperties
{
	FBEndPreMSec=5000
	FBSummaryMSec=5000
	FBAwardMSec=5000
	FBEndTotalMSec=120000
	MovieInfoStr="UI_FBEndAll.FBEndAll"
	bAllowFocus=FALSE
	Priority=6
}