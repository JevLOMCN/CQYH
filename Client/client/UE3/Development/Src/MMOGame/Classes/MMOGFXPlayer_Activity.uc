class MMOGFXPlayer_Activity extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIActivityViewPanel _ActivityPanel;
var MMOUIActivityQuitPanel _ActivityQuitPanel;

function ASRegisterActivityPanel( MMOUIActivityViewPanel mc )
{
	_ActivityPanel = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Activity);
}

function ASRegisterActivityQuitPanel(MMOUIActivityQuitPanel mc)
{
	_ActivityQuitPanel = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_ActivityQuit);
}

native  function ASSortActivity(int type);
native  function ASShowActivityFilterType(int nType);
native  function ASShowActivityFilterRewardType(int nType);

native function ASClickGetReward();
native function ASClickEnterTeamHome();
native function ASClickActivityList(int nActivityID);
native function ASClickActivityFBList(int nFBID);
native function ASClickFastJoinTeam(int nID);
native function ASClickRandJoinTeam(int nID);

native function  ASShowTips_ActivityRewardItem(GFxObject mc);
native function  ASHideTips_ActivityRewardItem();

native function  ASShowTips_ActivityRewardItemBar(GFxObject mc,int nIndex);
native function  ASHideTips_ActivityRewardItemBar();



native function ASClickQuitListHeadSort(int type);
native function ASClickOpenActivityPanel();
native function ASClickBackGame();
native function ASClickBackLogin();
native function ASClickQuitGame();
native function ASClickQuitGetReward(int nIndex);

//Dungeon lobby related
native function ASShowActivityFBView();
native function ASShowActivityTeamView();
native function ASClickActivityTeamList(int nTeamId);
native function ASRollOverActivityTeamList(int nTeamId);
native function ASRollOutActivityTeamList(int nTeamId);

native function ASClickTeamManager();
native function ASClickCreateTeam();
native function ASClickEnterTeam();
native function ASClickQuickEnterLimitted();
native function ASClickQuickEnter();
native function ASClickInvite();
native function ASClickChangeTarget();
native function ASClickLeaveTeam();
native function ASPrevTeamPageClick();
native function ASNextTeamPageClick();

DefaultProperties
{
	MovieInfoStr="UI_ActivityWidget.Activity"
	Priority = 5
}
