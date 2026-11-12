class MMOUIChat extends MMOUIBaseSlotContainer
	config( MMOGame)
	native(UI);



//struct native ChatMessage
//{
//	var string	ChatFilterChannelMsg; //channel message
//	var string	ChatPlayer; //speaking player
//	var int	        ChatTypeId; //type
//};

// struct native ChatFilterChannelMessage
//{
//	var int ChannelFilterType; // channel filter flag
//	var array<ChatMessage> ChatMessageList;
//	var bool IsDefaultChannel;
//	var string ChannelName;
//};



var config int MsgLimit;
var array<string> ChatChannelStr;
var array<string> ChatChannelColorStr;

var int CurrentChannel;
//var array<ChatFilterChannelMessage> ChatFilterChannelList;
var int CurrentFilterChannel;

var GFxObject InputArea;
var GFxObject ChatDropDown;
var GFxObject ChatNew;

var float   _DeltaTime;
var float   _ProcessTime;
var float   _ProcessTrumpetCDTime;

var float TrumpetCDTime;
var string LastTrumpet;

var bool bChatNear;
var float  _ChatNearProcessTime;

var bool bLock;

enum ChatTipsType
{
	CTT_CHAT,
	CTT_CHAT_CLEAN,
};

event InitOnce()
{
	super.InitOnce();
	ShowInputArea(false);

	//class'MMOUIManager'.static.SetChatMessageDelegate(AddChatMessage);
	class'MMOUIManager'.static.SetEnterPrivateChatDelegate(EnterPrivateChat);
	class'MMOUIManager'.static.SetSetFilterChannelNumDelegate(SetFilterChannelNum);
	class'MMOUIManager'.static.SetSetFilterChannelDelegate(SetFilterChannel);

	//Can only be used after the function is registered
	AddDefaultFilterChannel();

	Set_onShowChatInput(_onShowChatInput);
}

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	pl.AddPostAdvanceDelegate(PostAdvance);
}

native function OnFocusGained();
native function OnFocusLost();

native function NativeInitOnce();

native function EnterPrivateChat( string playerName );
native function bool OnEsc();

native function EnterScene          ();
native function ProcessChatMsgBuff  ();
native function ProcessChatTrumpet  ();
native function TickTime            ( float tickTime );
native function ShowInputArea( bool Show ); 


native final function _onShowChatInput(bool show);

native final function ASChatLockChat(bool lock);

native final function ASRequestNewChat(bool v);
delegate ASBool1Fun(bool v);
event Set_onShowChatInput(delegate<ASBool1Fun> f)
{
	ActionScriptSetFunction("_onShowChatInput");
}


//function ChatChannelChange( int idx )
//{
	//CurrentChannel = idx;
//}

native function ChatChannelChange( int idx );
native function AddDefaultFilterChannel();
native function SetFilterChannel( int pos , string ChannelName );
native function SetFilterChannelNum( int num );

native function ClearUI();

native function SendMessage( int TypeId , string Msg , bool showMsg = false);
//native function NativeReceiveMessage(int ChannelId , int TargetId , string MessageContent , string SenderName );
//native function AddChatMessage( int TypeId , string MessageContent , string SenderName );
//native function AddSystemMessage( string MessageContent );
//{
//	local array<ASValue> args;
//	local ASValue asval;
//	if( TypeId >= ECC_Num )
//		return;
//	asval.s = "["$ChatChannelStr[TypeId]$"]"$SenderName$":"$MessageContent;
//	asval.Type = AS_String;
//	args[0] = asval;
//	//GetObject("TextShowArea_mc").Invoke("NewAppendHtml",args );
//	GetObject("TextShowArea_mc").Invoke("appendHtml",args );

//	UpdateShowTextArea();
//}

native function bool SkipCaptureInputKey(String key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

native function PlayerMoneyChange(EMONEY_TYPE MoneyType, int DeltaMoney );

native function ChannelTab(int id);
//{
//	CurrentFilterChannel = id;
//	UpdateShowTextArea();
//}

native function UpdateShowTextArea();

native function CleanChnnelChat();

event CallAppendChatMsg(string msg)
{
	ActionScriptVoid("UCAppendChatMsg");
}

event CallAppendInput(string msg)
{
	ActionScriptVoid("UCAppendInput");
}

event CallAddChatMsg(GFxObject Provider)
{
	ActionScriptVoid("UCAddChatMsg");
}

event CallClearChatMsg()
{
	ActionScriptVoid("UCClearChatMsg");
}

native function int  GetSlotType();
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);

event CallUpdateChannelTab(int index)
{
	ActionScriptVoid("UCUpdateChannelTabClicked");
}

event CallShowChatBackBg(bool v)
{
	ActionScriptVoid("UCShowChatBackBg");
}

event CallShowTrumpet(bool v)
{
	ActionScriptVoid("UCShowTrumpet");
}

event CallUpdateTrumpetMsg(string msg,bool show,bool bGM)
{
	ActionScriptVoid("UCUpdateTrumpetMsg");
}

event callSetDropDownList(GFxObject Provider)
{
	ActionScriptVoid("UCSetDropDownList");
}

event callSetDropDownListData(GFxObject Provider)
{
	ActionScriptVoid("UCSetDropDownListData");
}

event CallChangeChannelByTab()
{
	ActionScriptVoid("UCChangeChnnelByTab");
}
event CallSetPrivateChatName(string playerName)
{
	ActionScriptVoid("UCSetPrivateChatName");
}

event CallSetDropDownListIndex(int idx)
{
	ActionScriptVoid("UCSetDropDownListIndex");
}

event CallShowPrivateTabEffect(int idx)
{
	ActionScriptVoid("UCShowPrivateTabEffect");
}

event CallShowPrivateName()
{
	ActionScriptVoid("UCShowPrivateName");
}

//-------------------Moved---------------------------------
native function ASOnChatEnter();

//event OnFocusLost(int LocalPlayerIndex)
//{
	//super.OnFocusLost(LocalPlayerIndex);
	//ASFocusInTextInput(false);
//}
native function ASFocusInTextInput(bool In);
native function PostAdvance(float DeltaTime );

function ASChatChannelChange( int index )
{
	ChatChannelChange( index);
}

function ASChannelTab(int ChannelTabId)
{
	ChannelTab( ChannelTabId );
}

native function ASSendChat( string TextContent,bool showMsg = false);


//event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)//moved
//{
	

	//if (GetlockMouseCursorForUI()==false && (key=="LeftMouseButton"||key=="RightMouseButton"))
	//{
		//`log("___________CaptureInputKey:" @ strHoverControllerName );
		//if (strHoverControllerName == "listItem_tf" )
		//{
			//return false;
		//}
		//if (strHoverControllerName == "ChatBg_mc" )
		//{
			//return false;
		//}
		//if (strHoverControllerName == "AlphaBk_mc" )
		//{
			//return false;
		//}
	//}
	//return super.SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
//}

native function ASOnPanelClose( string PanelName );

native function ASonRollChatPanel(bool focus);

//Clear chat log
function ASChatCleanAll()
{
	CleanChnnelChat();
}

//tips
native function ASChatShowTips(GFxObject mc, ChatTipsType type);
native function ASChatHideTips();

//
event CallASSetTimeEvent( int eventType, int secTotal, int secInteval )
{
	ActionScriptVoid("UCSetTimeEvent");
}
native function ASTimeEvent( int eventType );

native function ASChatOptionClickFilter( int id , bool sel );

native function ASChatItemShow( bool showStr );

DefaultProperties
{
	DefaultVisible = true
	CurrentChannel=0
	CurrentFilterChannel=0
	//MsgLimit=100
	CloseOnEsc=false
	_DeltaTime=0.0
	_ProcessTime=0.0
	LastTrumpet = ""
	_ProcessTrumpetCDTime = 0;
	bChatNear = true
	_ChatNearProcessTime=0.0
}
