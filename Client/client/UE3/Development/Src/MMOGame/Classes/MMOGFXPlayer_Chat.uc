class MMOGFXPlayer_Chat extends MMOGFXPlayer
	native(UI)
	config(MMOGame);


var MMOUIChat Chat;
var MMOUIChatOption _ChatOption;

//enum ChatTipsType
//{
	//CTT_CHAT,
	//CTT_CHAT_CLEAN,
//};

function RegisterChatPanel( MMOUIChat mc )
{
	Chat = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Chat);
}

function RegisterChatOption( MMOUIChatOption mc )
{
	_ChatOption = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Chat_ChatOption );
}

native function ASOnChatEnter();//moved

event OnFocusLost(int LocalPlayerIndex)//moved
{
	super.OnFocusLost(LocalPlayerIndex);
	//ASFocusInTextInput(false);
}

native function ASFocusInTextInput(bool In);//moved
native function PostAdvance(float DeltaTime );//moved

function ASChatChannelChange( int index )//moved
{
	Chat.ChatChannelChange( index);
}

function ASChannelTab(int ChannelTabId)//moved
{
	Chat.ChannelTab( ChannelTabId );
}

native function ASChatClickChatOption();//moved
//{
//	_ChatOption.ToogleVisible();
//}

function ASChatOptionDeleteChannel()//moved
{
	_ChatOption.ClickDelChannel();
}

function ASChatOptionAddChannel()//moved
{
	_ChatOption.ClickAddChannel();
}

function ASChatOptionChannelNameChanged( string newName )//moved
{
	_ChatOption.ChannelNameChanged(newName);
}

function ASChatOptionClickChannel( int index )//moved
{
	_ChatOption.ClickChannel( index );
}

function ASChatOptionClickFilter( int id , bool sel )//moved
{
	_ChatOption.ClickFilter( id , sel );
}

function ASChatOptionReset()//moved
{
	_ChatOption.ClickReset();//moved
}

function ASChatOptionApply()//moved
{
	_ChatOption.ClickApply();
}

function ASSendChat( string TextContent)//moved
{
	if ( Chat != none)
	{
		Chat.SendMessage( 0 , TextContent );
	}
}

event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)//moved
{
	

	if (GetlockMouseCursorForUI()==false && (key=="LeftMouseButton"||key=="RightMouseButton"))
	{
		//`log("___________CaptureInputKey:" @ strHoverControllerName );
		if (strHoverControllerName == "listItem_tf" )
		{
			return false;
		}
		if (strHoverControllerName == "ChatBg_mc" )
		{
			return false;
		}
		if (strHoverControllerName == "AlphaBk_mc" )
		{
			return false;
		}
	}

	//if ( Chat != none)
	//{
	//	Chat.SkipCaptureInputKey( key, eIE, strHoverControllerName, ClickPosInMovieSpace);
	//}

	return super.SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

// On ESC give focus to Skillbar
native function ASOnPanelClose( string PanelName );//moved

native function ASonRollChatPanel(bool focus);//moved

//Clear chat log
function ASChatCleanAll()//moved
{
	Chat.CleanChnnelChat();
}

//tips
native function ASChatShowTips(GFxObject mc, ChatTipsType type);//moved
native function ASChatHideTips();//moved

DefaultProperties
{
	MovieInfoStr="UI_ChatWidget.ChatWidget"
	
	//bAllowFocus=FALSE
	Priority=2 // Same as Skillbar, managed dynamically by chat input state
}
