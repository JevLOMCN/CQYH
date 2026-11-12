class MMOGFXPlayer_Team extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUITeam            Team;
var MMOUITeamChangeTargetPanel Team_TargetChange;
var MMOUITeamInvitePanel Team_Invite;

function RegisterTeam( MMOUITeam mc )
{

	Team = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Team);
}

function RegisterTeamTargetPanel(MMOUITeamChangeTargetPanel mc)
{
	Team_TargetChange = mc;
	RegisterUI(mc,class'MMOUIManager'.default.Panel_TeamTarget);
}

function RegisterTeamInvitePanel(MMOUITeamInvitePanel mc)
{
	Team_Invite = mc;
	RegisterUI(mc,class'MMOUIManager'.default.Panel_TeamInvite);
}

// event UnRegister()
// {
// 	super.UnRegister();
// 
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_Team);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_Team);
// 
// 	class'MMOUIManager'.static.SetInviteAddTeamDelegate(none);
// 	class'MMOUIManager'.static.SetLeaveTeamDelegate(none);
// 	class'MMOUIManager'.static.SetMoveLeaderDelegate(none);
// 	class'MMOUIManager'.static.SetInviteTeamRequestDelegate(none);
// 	class'MMOUIManager'.static.SetAddTeamRequestDelegate(none);
// 
// }

function ASTeamContentTab(int i)//moved team
{
	Team.TeamContentTab(i);
}

//Party part responses
function ASTeamEnterNearbyTeam()//moved team
{
	Team.EnterNearbyTeam();
}
function ASTeamSelectNearbyTeam( int id)//moved team
{
	Team.SelectNearbyTeam(id);
}
function ASTeamNearbyTeamPrePage()//moved team
{
	Team.NearbyTeamPrePage();
}
function ASTeamNearbyTeamNextPage()//moved team
{
	Team.NearbyTeamNextPage();
}

function ASTeamSelectNearbyPlayer( int id)//moved team
{
	Team.SelectNearbyPlayer(id);
}
function ASTeamInviteNearbyPlayer()//moved team
{
	Team.InviteNearbyPlayer();
}
function ASTeamNearbyPlayerPrePage()//moved team
{
	Team.NearbyPlayerPrePage();
}
function ASTeamNearbyPlayerNextPage()//moved team
{
	Team.NearbyPlayerNextPage();
}

function ASTeamSaveName()//moved team
{
	Team.SaveName();
}
function ASTeamTeamExpand()//moved team
{
	Team.TeamExpand();
}
function ASTeamCreateOrLeave()//moved team
{
	Team.CreateOrLeaveTeam();
}

function ASTeamChangeAssignTypeFree()//moved team
{
	Team.ChangeAssignTypeFree();
}

function ASTeamChangeAssignTypeTurn()//moved team
{
	Team.ChangeAssignTypeTurn();
}

function ASTeamTabChange(int TabId)//moved team
{
	Team.TeamContentTab(TabId);
}

native function ASClickClearTeamTarget();//moved team
native function ASClickChangeTeamTarget();//moved team
native function ASClickShowTeamInvite();//moved team
native function ASClickSelectPlayer(int playerId);//moved team
native function ASClickUnSelectPlayer(int playerId);//moved team
native function ASShowInviteFriendList();//moved invite
native function ASShowInviteRecentList();//moved invite
native function ASShowInviteGuiderList();//moved invite
native function ASClickInviteOk();//moved invite
native function ASClickInviteCancel();//moved invite
native function ASClickChangeTargetOk(int targetType, int targetId);//moved target
native function ASShowActivityFBTargetList();//moved target
native function ASShowActivityTargetList();//moved target
native function ASShowFBTargetList(int nDifficulty);//moved target
native function ASCancelChangeTarget();//moved target

final native function ASTeamMemberRightClick( int x , int y , int index );//moved team

native function ASAutoTeam( bool sel );//moved team

native function ASTeamAuctionLimitChange( int sel );//moved team

function ASTeamNearbyPlayerRightClick( int x , int y , int index )//moved team
{
	class'MMOUIManager'.static.OpenRClickMenu( ERCMType_TeamNearby , x , y, 0 , "" , index );
	//RClickMenu.ChangeOpen( ERCMType_TeamNearby,x,y, , ,index );
}

DefaultProperties
{
	MovieInfoStr="UI_TeamWidget.TeamWidget"
	Priority=5
}
