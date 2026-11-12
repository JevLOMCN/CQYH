class MMOUIRClickMenu extends MMOUIBase
	//dependson(TeamManager)
	native(UI);

var GFxObject _ConfirmBox;

enum ERClickMenuType
{
	ERCMType_TargetInfo,    //target
	ERCMType_TeamMember,
	ERCMType_TeamMate,
	ERCMType_ChatPlayer, //name in chat
	ERCMType_TeamNearby,
	ERCMType_Friend,            //friend
	ERCMType_Contact,       //non-friend and followed tab of the friend UI
	ERCMType_BattleMember, //battle team teammate
	ERCMType_FriendGroup, //friend group
	ERCMType_Attention, //follow list
	ERCMType_Black,
	ERCMType_GuildMember, //clan member
	ERCMType_Dungeon,//dungeon right click
	ERCMType_BattleEnter,//battlefield queue
	ERCMType_RankListPlayer, //player ranking list
	ERCMType_RankListGuild, //guild ranking list
	ERCMType_MasterStudent, //master/apprentice right click
	ERCMType_ChangeMapLine, //map line list
	ERCMType_GuildAlliance, //guild alliance
	ERCMType_AuctionPlayer,
	ERCMType_GuildEnemy, //guild enemy
};

enum ERClickMenuLevel
{
	ERCMLevel_Main, //main
	ERCMLevel_Child, //secondary
};

var array<delegate<ClickFunc> > FuncList;
var array<delegate<ClickFunc> > FuncListChild; // secondary
var const int ButtonMaxNum;
var String OwnerName; // target player the right click points to, id or name?
var MMOActor OwnerActor;
//var MMOTeamMember OwnerMember;
//var MMONearbyPlayer OwnerNearPlayer;
//var TeamManager TM;
//var FriendManager FM;
var int OwnerRoleId;
var int OwnerIndex;//index
var int OwnerGroupId;//group number
var int high;//
var int lineStep;//
var int step;
var MMOUIRClickMenuChild MenuChild; //secondary right-click menu
var GFxObject CurClickButton; //clicked button
var GFxObject RCMenuBg; //bg
var GFxObject MenuOwnerTF; //owner name textField

var int ChildStep;
var int ChildHigh;

var int curType;

var int GuildId;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	//TM = player.TeamManager;
	//FM = Player.FriendManager;

	RCMenuBg = GetObject("RCMenuBg");
	MenuOwnerTF = GetObject("tF_menuOwner");
	SetInt( "MenuLevel" , ERCMLevel_Main );

	class'MMOUIManager'.static.SetOpenRClickMenuDelegate(OpenRClickMenu);
	class'MMOUIManager'.static.SetCloseRClickMenuDelegate(CloseRClickMenu);
}

function InitWidget()
{

}

event CallResetChildMenuButton()
{
	ActionScriptVoid("ResetChildMenuButton");		
}

delegate ClickFunc( int idx );

native function MMOActor GetActorByRoleId( int RoleId );
native function AddMenuItem( delegate<ClickFunc> func , string StrName , optional bool UseStringMap = true );
//native function AddChildMenuItem( delegate<ClickFunc> func , string StrName , optional bool UseStringMap = true );//move
native function bool SkipCaptureInputKey(String key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);


//native function ChangeMenu(  int RoleId , String RoleName , int index , ERClickMenuType type );
//{
//	local GFxObject lineBase;
//	step = 0;
//	lineStep = 0;

//	high = 0;
//	lineBase = GetObject("splitLine");
	
//	if( lineBase != none )
//	{
//		high = lineBase.GetInt( "height" ) + lineBase.GetInt("y");
//	}
//	curType = type;
//	switch( type )
//	{
//	case ERCMType_TargetInfo:
//		MenuByTargetInfo(RoleId,RoleName);
//		break;
//	case ERCMType_TeamMember:
//		MenuByMyTeam( index );
//		break;
//	case ERCMType_TeamMate:
//		MenuByTeamMate( index );
//		break;
//	case ERCMType_TeamNearby:
//		MenuByNearPlayer( index );
//		break;	
//	case ERCMType_Friend:
//		MenuByFriend( RoleId , RoleName ); //here the RoleId field is GroupId
//		break;
//	case ERCMType_Attention:
//		MenuByAttention( index , RoleName ); 
//		break;
//	case ERCMType_FriendGroup:
//		MenuByFriendGroup( index , RoleName );
//		break;
//	case ERCMType_BattleMember:
//		MenuByBattleMember( index );
//		break;
//	//case ERCMType_Black:
//	//	MenuByBlack( index );
//	//	break;
//	case ERCMType_GuildMember:
//		MenuByGuildMember(RoleId);
//		break;
//	case ERCMType_ChatPlayer:
//		MenuByChatPlayer( RoleId , RoleName );
//		break;
//	}

//}

native function OpenRClickMenu( ERClickMenuType type, int x, int y , int RoleId  ,String RoleName  ,int index );
native function CloseRClickMenu( ERClickMenuType type);
native function ShowClickItem( string label  ); //move
//native function ShowChildClickItem( string label  );// move

event ClickItem( int MenuLevel , int itemid )
{
	local delegate<ClickFunc> tClickFunc;
		
	/*if ( MenuLevel == ERCMLevel_Child  )
	{
		CurClickButton = MenuChild.GetObject("RClicklistItem"$itemid);
		if( FuncListChild.Length >itemid  && FuncListChild[itemid]!= none)
		{
			tClickFunc = FuncListChild[itemid];
			tClickFunc( itemid );
		}
	}*/
	/*else*/ if( MenuLevel ==  ERCMLevel_Main  )
	{
		CurClickButton = GetObject("RClicklistItem"$itemid);
			if( FuncList.Length >itemid  && FuncList[itemid]!= none)
		{
			tClickFunc = FuncList[itemid];
			tClickFunc( itemid );
		}
	}
}

native function ClearUI();
function AddChangeNameOk(GFxObject mc)
{
	local int GroupId;
	local string textInput;
	GroupId = mc.GetInt("iteminfoid");
	textInput = mc.GetObject( "textInput" ).GetString("text");
	class'MMOUIManager'.static.ChangeGroupName( textInput , GroupId );
}

native function AddNewGroupOk(GFxObject mc);
native function DelFriendOk( GFxObject mc );

function MsgBoxNo( GFxObject mc ) //no handling
{
//
}

native function ClickDelGroup( int inMenuIdx );
native function DelGroupOk( GFxObject mc );
native function MoveBlackOk( GFxObject mc );

final native function ClickOpenContactDialog( int inMenuIdx );
final native function ClickInvitePlayerIntoTeam( int inMenuIdx );
final native function ClickApplyDoTrade( int inMenuIdx );
final native function ClickLeaveFB( int inMenuIdx );
final native function ClickLeaveBattleEnterQueue( int inMenuIdx );
final native function ClickChangeMapLine( int inMenuIdx );
final native function ClickAddFriend( int inMenuIdx );
final native function ClickAddEnemy( int inMenuIdx );
final native function ClickRemoveEnemy( int inMenuIdx );
final native function ClickAchievementCompare( int inMenuIdx );
final native function ClickLeaveTeam( int inMenuIdx );
final native function ClickMoveLeader( int inMenuIdx );
final native function ClickChangeTeamAuctionLimit( int inMenuIdx );
//final native function ClickChangeTeamAuctionLimitColor( int inMenuIdx );//move
final native function ClickEnterPrivateChat( int inMenuIdx );
final native function ClickMoveToBlackList( int inMenuIdx );
final native function ClickRemoveFromBlackList( int inMenuIdx );
//final native function ClickMoveFriendToGroup( int inMenuIdx ); //move
final native function ClickSendMail( int inMenuIdx );
final native function ClickMoveFriend( int inMenuIdx ); //click move friend
final native function ClickInviteBattleTeam( int inMenuIdx );//invite to battle team
final native function ClickRemoveBattleTeam( int inMenuIdx );//kick battle team member
final native function ClickNewGroup( int inMenuIdx );
final native function ClickChangeGroupName( int inMenuIdx );

//General
	native function ClickDeleteFriend( int inMenuIdx );
	//view profile
	native function ClickViewProfile( int inMenuIdx );
	//View brief profile
	native function ClickSimpleProfile( int inMenuIdx );
	//click follow/unfollow
	native function ClickFollow( int inMenuIdx );
	native function ClickNullFollow( int inMenuIdx );

//guild category
	//Invite to join guild
native function ClickInviteToJoinGuild( int inMenuIdx );
	//Recommend to join guild
native function ClickRecommendJoinGuild( int inMenuIdx );
	//Apply to join guild
native function ClickApplyToJoinGuild( int inMenuIdx );
	//Kick from guild
native function ClickKickOutGuild( int inMenuIdx );
	//Guild mute
native function ClickForbidChatOn( int inMenuIdx );
native function ClickForbidChatOff( int inMenuIdx );
	//Guild alliance
native function ClickCancelAlliance( int inMenuIdx );
	//Copy guild name (via player)
native function ClickCopyGuildName(int inMenuIdx);
	//Copy guild name (via UI)
native function ClickCopyGuildNameByPanel(int inMenuIdx);
	//Guild declare war
native function ClickGuildWar(int inMenuIdx);

//Open guild info page
native function ClickOpenGuild( int inMenuIdx );

//Master/apprentice
	//become apprentice
native function ClickApplyMaster( int inMenuIdx );
native function ClickApplyStudent( int inMenuIdx );
	//Take apprentice
native function ClickReadmastermsg( int inMenuIdx );
	//Expel from master's school
native function ClickExpell( int inMenuIdx );
//Betray master
native function ClickBetryMaster( int inMenuIdx );

//Marry
	//Propose
native function ClickMerry( int inMenuIdx );
	//Divorce
native function ClickDivorce( int inMenuIdx );

//general settings
	//block chat
native function ClickCommon1( int inMenuIdx );
	//unblock
native function ClickCommon2( int inMenuIdx );
	//Report player
native function ClickCommon3( int inMenuIdx );
	//Track position
native function ClickCommon4( int inMenuIdx );
//copy name
	//copy name
native function ClickCopyName( int inMenuIdx );

//View character info
native function ClickViewProfileOnRank( int inMenuIdx );

native function FireStudent(GFxObject m);
native function OnFireStudentCancel();

//Betray master
native function LeaveMaster(GFxObject m);
native function LeaveMasterCancel();

//-------------------moved----------------------
native function ASClickRightMenuItem( int MenuLevel, int ItemId );

DefaultProperties
{
	ButtonMaxNum = 20
	OwnerActor=none
	DefaultVisible=false
	CloseOnEsc=false
}
