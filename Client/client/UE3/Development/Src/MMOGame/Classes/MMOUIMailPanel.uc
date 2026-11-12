class MMOUIMailPanel extends MMOUIBase
	native(UI)
	config(UIVisibleMask);

var GFxObject SendPage;
var float _currentTime;
var GFxObject msgBox;
var float _delayTime;
var bool bSendMail;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}



native function NativeInitOnce();

native final function UpdateMailList();
native final function UpdateRecentPlayers();

static final function ReplyMail(int mailID);

native function OnMailWriteSuccess();
native function OnSelectAll();

event InitOnce()
{
	// Note: initialize here first before calling NativeInit() below
	super.InitOnce();

	SendPage = GetObject("send_mail_page_mc");
}


event CallUpdateItemsData(GFxObject Provider)
{
	ActionScriptVoid("UCUpdateMailData");
}

event CallReplyMail(string toPlayer, string mailTitle)
{
	ActionScriptVoid("UCReplyMail");
}

event CallClearSendContent()
{
	ActionScriptVoid("UCClearSendContent");
}

event CallShowTabPage(int idx)
{
	ActionScriptVoid("UCShowTabPage");
}

event CallAddRecentPlayers(GFxObject Provider)
{
	ActionScriptVoid("UCAddRecentPlayers");
}

event CallSetPlayerName(string v)
{
	ActionScriptVoid("UCSetPlayerName");
}

event CallUpdateSelectAllState(bool v)
{
	ActionScriptVoid("UCUpdateSelectAllState");
}

native function ASSetCurrentPageNum(int currentPage,int num);
native function ASMailPageChange(int currentPage);

native function TurnSendMail( string playerName );

//----------------moved--------------------
native final function ASReadMail(int mailID);//moved
native final function ASSendMail(string toPlayer, string title, string content);//moved


native final function ASReadAll();//moved
native final function ASSelectAll();//moved
native final function ASDeleteSelected();//moved
native final function ASSelectMail(int mailID, bool select);//moved
native final function ASMessageMailContentTooLong();//moved
native final function OnDeleteMail(GFxObject mc);//moved
native final function ASSelectMailType(int nType, int page);//moved

native final function ASSelectCurrentPageMail(bool selected,int currentPage);

native function BeforeViewUpdate(float DeltaTime);

DefaultProperties
{
	IsDynamicLoad=true
}
