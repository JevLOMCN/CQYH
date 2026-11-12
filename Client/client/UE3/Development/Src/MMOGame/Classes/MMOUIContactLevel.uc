class MMOUIContactLevel extends MMOUIBase
	native(UI);


/*
Friend chat rework
*/
 var int nCurRoleID;
// var int CurWindowId;
// var const int WindowChatNum; //number of controls per window: 4
// 
// 
// var native pointer FriendChatMgr{class FriendChatManager};
// 
//  struct native ContactWindowStruct
// {
// 	var string WindowName;
// 	var GFxObject Widget;
// 	var string PlayerName[4];
// 	var int RoleId[4];
// 	var int ActiveId;
// 	var int WindowId;
// };
// 
// 
// 
// 
// 
// //Window list
// var array<ContactWindowStruct> ContactWindowList;
// 
// //Add to unread list, addLog: whether to add log content
// native function AddtoUnReadContactLog( int RoleId , string PlayerName , string Content , optional bool addLog = true );
// native function AddtoReadContactLog( int RoleId , string PlayerName , string Content , optional bool fromSelf = false );
// //Save each friend's chat into the loglist by RoleId
// native function AddtoContactLog( int RoleId , string PlayerName , string Content , optional bool fromSelf = false );
// 
// 
// native function FriendChatMsg( int RoleId , string Content );
// 
// native function NativeInit();
// native function ClearUI();
/*
Friend chat rework
*/

event InitOnce()
{
	super.InitOnce();
	
// 	class'MMOUIManager'.static.SetSelectFriendChatDelegate(SelectFriendChat);
// 	class'MMOUIManager'.static.SetMoveUnReadMsgToReadDelegate(MoveUnReadMsgToRead);

	class'MMOUIManager'.static.SetSelectFriendChatDelegate(SelectNewRoleChat);
	class'MMOUIManager'.static.SetMoveUnReadMsgToReadDelegate(OpenUnReadChatContent);
	//NativeInit();
}

native function OpenUnReadChatContent(int RoleId , string PlayerName );
native function SelectNewRoleChat(int RoleId , string PlayerName);
native function FriendChatMsg( int RoleId , string Content );
// 
// native function AddContactMessage( GFxObject Window , string PlayerName /*, string Content*/ );
// 
// native function ContactWindowTab(GFxObject Window , int Tab);
// 
// native function CreateNewWindow( string PlayerName , int RoleId );
// 
// native function ActiveChildWindow( GFxObject Window , int ChildId );
// //Select friend chat, may also be activation
// native function SelectFriendChat( int RoleId , string RoleName );
// 
// native function MoveUnReadMsgToRead( int RoleId , string PlayerName );
// 
// 
// 
// native function MiniMizeWindow( string WindowName);
// 
// 
// //native function SetChildWindow( GFxObject Window , int ChildId , bool Visible , bool CloseVisible , optional string PlayerName = "" ); 
// 
// native function SendFriendChatMessage( int WindowId , string MsgContent );
// 
// native function WindowTabClose( GFxObject Window , GFxObject Tab );
// 
// native function int GetWindowOpenChildNum( int WindowId );
// 
// native function UpdateWindowTab( int WindowId );
// 
// native function RemoveWindow( string WindowName );



/*
	Reorganize friend chat
*/
event CallUCDataFriendChatList(GFxObject data)
{
	ActionScriptVoid("UCDataFriendChatList");
}

event CallUCShowRoleItemEffect(int index)
{
	ActionScriptVoid("UCShowRoleItemEffec");
}

native function ASContactWidgetClose( string WindowName );//moved

//function ASContactWidgetMiniMize(string WindowName)//moved
//{
	//ContactLevel.MiniMizeWindow( WindowName );
//}

native function ASSendFriendChatByRoleID( int nRoleID , string ChatMsg );

native function ASSelectRoleChatWnd(int nRoleID);

native function ASCloseRoleChatWnd(int nRoleID);

native function ASContactWidgetMiniMize(string WindowName);

native function ASShowFriendChatList();

native function ASOnPanelClose( string PanelName );

DefaultProperties
{
	IsDynamicLoad=true
	CloseOnEsc=false
	DefaultVisible=false
	//CurWindowId=0
	//WindowChatNum=4
	CanMoveTop=false
}
