class MMOUIFriendChatHint extends MMOUIBase
	native(UI);

struct native FriendChatHintStruct
{
	var GFxObject ListItemGfxObject; //control
	var string PlayerName; //name
	var int RoleId; // 
};

var GFxObject ListMc;
var GFxObject HintButton;
var GfxObject list;

//var MMOFriendChatManager FriendChatManager;

//var native pointer FriendChatMgr{class FriendChatManager};

var array<FriendChatHintStruct> FriendChatHintList;

native function UnReadFriendChatMsgChange();

//event GFxObject CallAddNewItem()
//{
	//return ActionScriptObject("AddNewItem");
//}
event CallAddNewItem(int RoldId,string playerName,int msgNum)
{
	ActionScriptVoid("AddNewItem");
}
event CallRemoveItem(int index)
{
	ActionScriptVoid("RemoveItem");
}

event CallUpdateItemList(int RoldId , int msgNum)
{
	ActionScriptVoid("UpdateItemList");
}

//------------------moved------------------------
native function ASFriendChatList( int Index );
event InitOnce()
{

	super.InitOnce();
	//NativeInit();

	ListMc = GetObject("FriendChatList");
	HintButton = GetObject("FriendChatHintButton");
	list = GetObject("list");
	//FriendChatManager = Player.FriendChatManager;
	UpdateHintButtonInfo();

	
}

//native function NativeInit();
native function ClearUI();

native function RemoveListItem( int Index );
native function AddNewListItem( int RoleId , string RoleName , int MsgNum );

native function ClickFriendChatHint( int Index );

native function MoveUnReadMsgToRead( int RoleId , string PlayerName );

native function UpdateUnReadList();
native function UpdateHintButtonInfo();

event CallsetHeadByMinimize(int career,int sex)
{
	ActionScriptVoid("setHeadByMinimize");
}

event CallSetHead(int career,int sex)
{
	ActionScriptVoid("setHead");
}

DefaultProperties
{
	DefaultVisible=true
	CloseOnEsc=false
	CanMoveTop=false
}
