class MMOUIFBBattleComfirm extends MMOUIBase
	native(UI);

enum EUI_FBBattleComf_Mode
{
	EUI_FBBattleComf_None,
	EUI_FBBattleComf_Normal,
	EUI_FBBattleComf_Captain,
};

const FBEndPreMSec=5000;
const FBSummaryMSec=5000;
const FBAwardMSec=5000;
const FBEndTotalMSec=120000;

var EUI_FBBattleComf_Mode UIMode;

//var GFxObject btnMcPlayAgain;
var GFxObject LeaveBtn;
//
var GFxObject BackFBSummaryBtn;
//var GFxObject BackFBAwardBtn;
//
var GFxObject CloseFBuiBtn;

var GFxObject ConfirmMsgBox;

//native function PlayFbAgain();
//native function LeaveFB();

//
native function UpdateUIMode();
//{
//	// if in a team
//	if( player != none && player.TeamManager.hasTeam ){
//		if( player.TeamManager.ImTeamLeader ){
//			UIMode = EUI_FBBattleComf_Captain;
//		}
//		else{
//			UIMode = EUI_FBBattleComf_Normal;
//		}
//	}
//	else{//single mode
//		UIMode = EUI_FBBattleComf_Captain;
//	}
//	//Update ui apparence
//	if( btnMcPlayAgain == none ){
//		return;
//	}
//	switch( UIMode )
//	{
//	case EUI_FBBattleComf_Captain:
//		btnMcPlayAgain.SetVisible( true );
//		break;
//	//
//	case EUI_FBBattleComf_Normal: 
//	case EUI_FBBattleComf_None:
//		btnMcPlayAgain.SetVisible( false );
//		break;
//	}
//}
//Open the UI during the countdown
native function OpenInTime( int iMSec );
event CallASStartCloseCountDown( int iMSec )
{
	ActionScriptVoid("StartCloseCountDown");
}
//
event CallASStopAndHideFB()
{
	ActionScriptVoid("StopAndHideFB");
}
//
native function ResetUI();
//
native function ConfirmLeaveFromTeleport( int teleportID );
//
native function AgreeLeaveFromTeleport( GFxObject pMc );
//
native function DiagreeFromTeleport( GFxObject pMc );

//===============================================================================================================
//===============================================================================================================
native function WaitFBtoSummary();

native function ShowCountDownBar();
//see the summary, after the time specified the UI will close
native function ReadyToFBEndAll();
//see the summary, after the time specified the UI will close
native function ReviewFBSummary();
//see the summary displayed last time, no time limited
native function OpenFBSummary();
native function HideFBSummary();
//ship the FBSummay UI to show the FBAward, but not to play
native function OpenFBAward();
native function HideFBAward();

native function SendPlayFBAgain();
native function WantPlayFbAgain();
native function SendLeaveFB();
native function LeaveFB();
//Close dungeon rating, dungeon lottery, dungeon exit reset windows
native function CloseAllFBEndUI();
//
native function OpenFBBattleComfirm();
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	//FriendlyName=FBBattleComfirm
	UIMode=EUI_FBBattleComf_None
	IsDynamicLoad=true
	CloseOnEsc=false
}