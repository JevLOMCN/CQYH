class MMOUITeam extends MMOUIBase
	//dependson(TeamManager)
	native(UI)
	config(UIVisibleMask);

//var MMOUIGFXPlayer _UIPlayer;
var int    NearPlayerCurPage;
var int     NearTeamCurPage;
var int    NearPlayerMaxPage;
var int     NearTeamMaxPage;
var int     TabPage;

var int     SelectPlayerIndex;
var int     SelectTeamIndex;

//Auto team
var bool    AutoTeam;

var bool bNearTeam;
var float updateTime;

var GFxObject myTeamPanel;
var GFxObject NearbyTeamPanel;
var GFxObject NearbyPlayerPanel;
var GFxObject MemberItem5;
var GFxObject MemberItem20;
var GFxObject SaveNameButton;
var GFxObject TeamNameTextField;
var GFxObject TeamNameFrame;
var GFxObject AssignTypeButtonFree;
var GFxObject AssignTypeButtonTurn;
var GfxObject AssignTypeLeader;
var GFxObject TeamExpandButton;
var GFxObject TeamCreateOrLeaveButton;
var GFxObject NearPlayerPageTextField;
var GFxObject NearTeamPageTextField;
var GFxObject EnterTeamButton;
var GFxObject InvitePlayerButton;
var GFxObject AutoTeamButton;
var GFxObject NearPage;

//var TeamManager TM;
var GFxObject TeamSetting;
var GFxObject TeamChangeTargetButton;
var GFxObject TeamClearTargetButton;
var GFxObject TeamAuctionColorLimit;

var const int MaxNearPlayerPerPage ;
var const int MaxNearTeamPerPage ;

enum EUITeamTab
{
	EUITeam_MyTeam,
	EUITeam_NearbyPlayer,
	EUITeam_NearbyTeam,
};

event InitLoad(MMOGFXPlayer pl)
{
	//local GFxObject mc;
	super.InitLoad(pl);

	myTeamPanel = GetObject( "myTeam_mc" );
	NearbyTeamPanel = GetObject("nearbyTeamContent_mc");
	NearbyPlayerPanel = GetObject( "nearbyPlayerContent_mc" );
	TeamChangeTargetButton = GetObject("change_target_btn");
	TeamClearTargetButton = GetObject("clear_target_btn");

	MemberItem5 = myTeamPanel.GetObject( "memberItem5" );
	MemberItem20 = myTeamPanel.GetObject( "memberItem20");
	
	TeamAuctionColorLimit = myTeamPanel.GetObject("TeamAuctionLimit");
	AssignTypeButtonFree = myTeamPanel.GetObject("AssignTypeFree");
	AssignTypeButtonTurn = myTeamPanel.GetObject("AssignTypeTurn");
	AssignTypeLeader = myTeamPanel.GetObject("AssignTypeLeader");
	AutoTeamButton = myTeamPanel.GetObject("AutoTeamButton");
	SaveNameButton = myTeamPanel.GetObject("SaveNameButton");
	TeamNameTextField = myTeamPanel.GetObject("TeamNameTxt");
	TeamNameFrame = myTeamPanel.GetObject("teamName");
	TeamExpandButton= myTeamPanel.GetObject("btn_teamExpand");
	TeamCreateOrLeaveButton = myTeamPanel.GetObject( "btn_leaveTeam" );
	
	InvitePlayerButton = NearbyPlayerPanel.GetObject("InvitePlayer");
	NearPlayerPageTextField = NearbyPlayerPanel.GetObject("TeamContentCurPage_txt");
	EnterTeamButton = NearbyTeamPanel.GetObject("EnterTeam");
	NearTeamPageTextField = NearbyTeamPanel.GetObject("_page");
	NearPage = NearbyPlayerPanel.GetObject("_page");

	
	TeamSetting = GFxPlayer.GetVariableObject("root.TeamSetting");
	if(TeamSetting!=none)
	{
		TeamSetting.SetVisible(false);
	}

	

	//TM = MMOPlayer(GetPC().pawn).TeamManager;
}

event InitOnce()
{

	//class'MMOUIManager'.static.SetLeaveTeamDelegate(LeaveTeam);
	//class'MMOUIManager'.static.SetMoveLeaderDelegate(MoveLeader);


	AutoTeamButton.SetBool("selected",false);

	super.InitOnce();

}
native function NativeInitOnce();
native function SendTeamList();


native function ClearUI();
native function UpdateMyTeam();

native function CenterCreateLeaveButton( bool center );

native function UpdateTeamContent();

native function UpdateNearbyPlayerContent();

native function SendTeamAdd( int RoleId, int TeamId);
native function CreateTeam( string TeamName , int AssignType );
//native function LeaveTeam( int RoleId );
//native function MoveLeader( int roleid );
native function InviteAddTeam( int RoleId );

native function CreateOrLeaveTeam();



native function SendTeamAddAnswer( int RoleId, bool agree);
native function SendTeamInviteAnswer( int RoleId, bool agree);

event CallSetPanelText( GFxObject data )
{
	ActionScriptVoid("SetPanelText");
}

event UpdateTeamTargetText(string target)
{
	ActionScriptVoid("SetTeamTargetText");
}

event CallSetDropDownMenu( GFxObject arrayData )
{
	ActionScriptVoid("SetDropDownMenu");
}

event CallSetTeamAuctionColorLimit( int idx )
{
	ActionScriptVoid("SetTeamAuctionColorLimit");		
}

event CallSetAutoTeamVisible(bool v)
{
	ActionScriptVoid("UCSetAutoTeamVisible");		
}

native function AddTeamOk(GFxObject mc);
//{
//	local int MsgInfoId;
//	MsgInfoId = mc.GetInt("iteminfoid");
//	SendTeamAddAnswer(MsgInfoId,true);
//}
native function AddTeamCancel(GFxObject mc);
//{
//	local int MsgInfoId;
//	MsgInfoId = mc.GetInt("iteminfoid");
//	SendTeamAddAnswer(MsgInfoId,false);
//}

native function AddTeamRequest( string RoleName, int RoleId);


native function InviteTeamRequest( string RoleName, int RoleId);


native function InviteTeamCancel(GFxObject mc);


native function EnterNearbyTeam();

function SelectNearbyTeam( int id)
{
	SelectTeamIndex = id+ NearTeamCurPage*MaxNearTeamPerPage;
}
function NearbyTeamPrePage()
{
	NearTeamCurPage--;
	UpdateTeamContent();
}
function NearbyTeamNextPage()
{
	NearTeamCurPage++;
	UpdateTeamContent();
}
native function SelectNearbyPlayer(int id);

native function InviteNearbyPlayer();

native function NearbyPlayerPrePage();

native function NearbyPlayerNextPage();

native function SaveName();





//Team extension
native function TeamExpand();


//native function SendOperateTeam(int LeadId , int AssignType , int TeamMemberLimit , string TeamName);

native function ChangeAssignTypeFree();
native function ChangeAssignTypeTurn();


event CallUCSetOnLineState(int nIndex, int nType, bool bOnLine)
{
	ActionScriptVoid("UCSetOnLineState");		
}

native function TeamContentTab(int tabid);

native function BeforeViewUpdate(float DeltaTime);

//-----------------------moved------------------------
function ASTeamContentTab(int i)
{
	TeamContentTab(i);
}

//Party part responses
function ASTeamEnterNearbyTeam()
{
	EnterNearbyTeam();
}
function ASTeamSelectNearbyTeam( int id)
{
	SelectNearbyTeam(id);
}
function ASTeamNearbyTeamPrePage()
{
	NearbyTeamPrePage();
}
function ASTeamNearbyTeamNextPage()
{
	NearbyTeamNextPage();
}

function ASTeamSelectNearbyPlayer( int id)
{
	SelectNearbyPlayer(id);
}
function ASTeamInviteNearbyPlayer()
{
	InviteNearbyPlayer();
}
function ASTeamNearbyPlayerPrePage()
{
	NearbyPlayerPrePage();
}
function ASTeamNearbyPlayerNextPage()
{
	NearbyPlayerNextPage();
}

function ASTeamSaveName()
{
	SaveName();
}
function ASTeamTeamExpand()
{
	TeamExpand();
}
function ASTeamCreateOrLeave()
{
	CreateOrLeaveTeam();
}

function ASTeamChangeAssignTypeFree()
{
	ChangeAssignTypeFree();
}

function ASTeamChangeAssignTypeTurn()
{
	ChangeAssignTypeTurn();
}

function ASTeamTabChange(int TabId)
{
	TeamContentTab(TabId);
}

native function ASClickClearTeamTarget();
native function ASClickChangeTeamTarget();
native function ASClickShowTeamInvite();
native function ASClickSelectPlayer(int playerId);
native function ASClickUnSelectPlayer(int playerId);

final native function ASTeamMemberUIRightClick( int x , int y , int index );//moved team
final native function ASTeamMemberUILeftClick( int x , int y , int index );

native function ASAutoTeam( bool sel );//moved team

native function ASTeamAuctionLimitChange( int sel );//moved team

function ASTeamNearbyPlayerRightClick( int x , int y , int index )//moved team
{
	class'MMOUIManager'.static.OpenRClickMenu( ERCMType_TeamNearby , x , y, 0 , "" , index );
	//RClickMenu.ChangeOpen( ERCMType_TeamNearby,x,y, , ,index );
}

DefaultProperties
{
	//CurPage = 0
	IsVisible=false
	IsDynamicLoad=true

	NearPlayerCurPage=1
	NearTeamCurPage=1
	SelectPlayerIndex=0
	SelectTeamIndex=0
	TabPage=0


	//MaxShowNearMemberPerPage = 11
	MaxNearPlayerPerPage=14
	MaxNearTeamPerPage=14

	AutoTeam=false
}