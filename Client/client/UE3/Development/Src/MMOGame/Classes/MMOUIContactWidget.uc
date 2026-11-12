class MMOUIContactWidget extends MMOUIBase
	native(UI);



//enum EFriendOnlineStatus
//{
//	EFOS_Offline,
//	EFOS_Online,
//	EFOS_Afk,
//	EFOS_Busy,
//};
//var FriendManager FM;
var GFxObject       ContactListContent; //basic info is all here
var GFxObject       SearchText; //basic info is all here
var GFxObject       MyFriendStatus;
var GFxObject       SearchPanel;
var GFxObject	    TipsWidget;

var MMOUIFriendNearContactContent   NearContactContent;
var MMOUIFriendListContent	FriendListContent;
var MMOUIFriendBlackListContent BlackListContent;
var MMOUIFriendEnemyListContent EnemyListContent;

var int MyStatus;
var string MySign;
var bool NeedUpdateSearchPanel;
var bool SearchPanelWillOpen;
var float UpdateSearchTime;

var int CurLabel;

var GFxObject msgBox;

const MaxSearchPanelItem = 6;

event InitWidget()
{

	super.InitWidget();
}

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	pl.AddPostAdvanceDelegate(PostAdvance);
}

native function NativeInitOnce();


//Used for both creating a new group and renaming a group
native function CreateNewGroup( string GroupName , int GroupId  );
native function ChangeGroupName( string GroupName , int GroupId);
native function DeleteGroup(  int GroupId );

native function SetFriendNum( int GroupId , int Num , int onlineNum );

native function SetEnemyListNum( int num );
native function SetBlackListNum( int num );

native function AddNewFriend(GFxObject mc);
native function AddNewFriend_No( GFxObject mc);

//Recent contacts
event CallSetNearContactData( GFxObject data )
{
	ActionScriptVoid("SetNearContactData");
}

event CallSetBlackListData( GFxObject data )
{
	ActionScriptVoid("SetBlackListData");
}

event CallUpdateKillInfo( string killInfoText )
{
	if( ContactListContent != none )
	{
		ContactListContent.ActionScriptVoid("UpdateKillInfo");
	}
}

//event CallSetFriendItemValueObject( GFxObject data )
//{
//	ActionScriptVoid("SetFriendItemValueObject");
//}

event CallSetSearchData( GFxObject data )
{
	ActionScriptVoid("SetSearchData");
}

event CallSetEnumValue( GFxObject data )
{
	ActionScriptVoid("SetEnumValue");
}

event CallUpdateBaseInfo( GFxObject data )
{
	ActionScriptVoid("UpdateBaseInfo");
}

native function SendChangeMyInfo();
native function ClearUI();

native function FriendChangeSearchText( String text );

native function ChangeMyInfo( string Sign , int status );
//{
//	MySign = Sign;
//	MyStatus = EFriendOnlineStatus(status);
//	SendChangeMyInfo();
//}

event CallSetPanelText( GFxObject data )
{
	ActionScriptVoid("SetPanelText");
}

//native function UpdateBlackList();
//native function UpdateEnemyList();
//native function UpdateNearContactList();
//native function UpdateMyFriend();
//native function UpdateFriendGroup( int GroupId );


function AddFriendCancel(GFxObject mc)
{
	
}


//------------------moved-----------------------
native function ASFriendChangeSearchText( String s);

native function ASFriendSearchFocusChange( bool in , String s );

native function ASFriendChangeSignature(String Sign);

native function ASFriendChangeStatus( int Status);

native function ASFriendAddFriend();

native function ASClickSearchItem( int pos );

function ASFriendMsgButton()//moved
{
}
native function PostAdvance( float dt );

function ASFriendFolderRightClick( int Posx, int Posy, GFXObject Item , int ItemId , String NameText )//moved
{
	class'MMOUIManager'.static.OpenRClickMenu( ERCMType_FriendGroup, Posx , Posy , 0 , NameText , ItemId );
	//RClickMenu.ChangeOpen( ERCMType_FriendGroup , Posx , Posy ,,NameText, ItemId );
}

native function ASFriendItemDClick(GFXObject Item);

native function ASFriendItemRightClick( int Posx, int Posy, GFXObject Item , int ItemId , String NameText);

native function ASFriendOverItem( int groupId , int index );


native function ASShowTips_State(GFxObject mc);
native function ASHideTips_State(GFxObject mc);

native function ASNearContactDClick(GFXObject Item);

//native function ASContactWidgetMiniMize(string WindowName);

//---------------------------Move interface----------------------------------
event CallCreateNewGroup( String GroupName , int GroupId )
{
	ActionScriptVoid("CreateNewGroup");
}

event CallDeleteGroup( int groupId )
{
	ActionScriptVoid("DeleteGroup");
}

event CallSetFriendListData(GFxObject data ,int type)
{
	ActionScriptVoid("SetFriendListData");
}

DefaultProperties
{
	DefaultVisible=false
	MyStatus= 0
	NeedUpdateSearchPanel=false
	SearchPanelWillOpen=false
	UpdateSearchTime=0.0
}
