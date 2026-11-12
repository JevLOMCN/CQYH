class MMOGFXPlayer_Friend extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIFriendChatHint ChatHint;
var MMOUIContactWidget  Friend;
var MMOUIContactLevel   ContactLevel;
var MMOUIFriendNearContactContent   NearContactContent;
var MMOUIFriendListContent	FriendListContent;
var MMOUIFriendBlackListContent BlackListContent;
var MMOUIFriendEnemyListContent EnemyListContent;

function RegisterFriend( MMOUIContactWidget mc)
{
	Friend = mc;
	RegisterUI( mc , class'MMOUIManager'.default.Panel_Friend);
}

function registerContactLevel( MMOUIContactLevel mc )
{
	ContactLevel = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Friend_FriendChatLevel);
}


function RegisterFriendChatHint( MMOUIFriendChatHint mc )
{
	ChatHint = mc;
	RegisterUI( mc , class'MMOUIManager'.default.Panel_Friend_FriendChatHint);
}

//These were created with new and registered very late, so they do not exist early; handling differs slightly
function RegisterFriendNearContactContent( MMOUIFriendNearContactContent mc )
{
	NearContactContent = mc;
	if( Friend != none )
	{
		Friend.NearContactContent = NearContactContent;
		NearContactContent.InitLoadOld( class'MMOUIManager'.default.Panel_Friend_NearContactContent , self);
	}
}

function RegisterFriendListContent( MMOUIFriendListContent mc )
{
	FriendListContent = mc;
	if( Friend != none )
	{
		Friend.FriendListContent = FriendListContent;
		FriendListContent.InitLoadOld( class'MMOUIManager'.default.Panel_Friend_FriendListContent , self);
	}
}

function RegisterBlackListContent( MMOUIFriendBlackListContent mc )
{
	BlackListContent = mc;
	if( Friend != none )
	{
		Friend.BlackListContent = BlackListContent;
		BlackListContent.InitLoadOld( class'MMOUIManager'.default.Panel_Friend_BlackListContent , self);
	}
}

function RegisterEnemyListContent( MMOUIFriendEnemyListContent mc )
{
	EnemyListContent = mc;
	if( Friend != none )
	{
		Friend.EnemyListContent = EnemyListContent;
		EnemyListContent.InitLoadOld( class'MMOUIManager'.default.Panel_Friend_EnemyListContent , self);
	}
}

/*-------------------------------------------------
Reorganize friend chat
*/
// native function ASSendFriendChat( int WindowId , string ChatMsg );
// 
// native function ASContactWindowTabClose( GFxObject Window , GFxObject Tab );
function ASContactWidgetClose( string WindowName )//moved
{
	//ContactLevel.RemoveWindow( WindowName );
}

function ASContactWidgetMiniMize(string WindowName)//moved
{
	//ContactLevel.MiniMizeWindow( WindowName );
}
// 	
/*
	Reorganize friend chat
	*/
// native function ASContactWindowTab( GFxObject Window , GFxObject Tab );
//-------------------------------------



native function ASFriendChatList( int Index );//moved
native function ASFriendOverItem( int groupId , int index );//moved
//native function ASFocusInTextInput( bool In );
native function ASClickSearchItem( int pos );//moved
native function PostAdvance( float dt );//moved

native function ASFriendItemRightClick( int Posx, int Posy, GFXObject Item , int ItemId , String NameText);//moved
//{
//	local int GroupId;
//	local FriendManager FM;
//	FM = MMOPlayer(GetPC().Pawn).FriendManager;
//	GroupId = Item.GetInt( "itemdataid");
//	if( GroupId == FM.AttentionGroupId )
//	{
//		class'MMOUIManager'.static.OpenRClickMenu( ERCMType_Attention , Posx , Posy , GroupId , NameText, ItemId );
//	}
//	else
//	{
//		class'MMOUIManager'.static.OpenRClickMenu( ERCMType_Friend , Posx , Posy , GroupId , NameText, ItemId );
//	}
//}

//friend part as
native function ASFriendChangeSearchText( String SearchText);//moved

native function ASFriendSearchFocusChange( bool in , String searchText );//moved

native function ASFriendChangeSignature(String Sign);//moved

native function ASFriendChangeStatus( int Status);//moved

native function ASFriendAddFriend();//moved

function ASFriendMsgButton()//moved
{
}
native function ASFriendItemDClick(GFXObject Item);//moved

native function ASNearContactDClick(GFXObject Item);//moved


function ASFriendFolderRightClick( int Posx, int Posy, GFXObject Item , int ItemId , String NameText )//moved
{
	class'MMOUIManager'.static.OpenRClickMenu( ERCMType_FriendGroup, Posx , Posy , 0 , NameText , ItemId );
	//RClickMenu.ChangeOpen( ERCMType_FriendGroup , Posx , Posy ,,NameText, ItemId );
}



/*
	Reorganize friend chat
*/
native function ASSelectRoleChatWnd(int nRoleID);//moved
native function ASCloseRoleChatWnd(int nRoleID);//moved
native function ASSendFriendChatByRoleID( int nRoleID , string ChatMsg );//moved

DefaultProperties
{
	MovieInfoStr="UI_Friend.Friend"
	Priority=5
}
