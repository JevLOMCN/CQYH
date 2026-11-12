class MMOUIGuildViewPanel extends MMOUIBase
	native(UI);

var int _CurPageIndex;
var bool _IsNpcCalled;
var GFxObject JoinConfirmBox;
var bool _LockPage;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

native function AgreeJoinNow(GFxObject mc);
native function RefuseJoinNow(GFxObject mc);

// UC->AS
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("UCSetWindowTitle");
}

event CallUpdateGuildViewList(GFxObject dataProvider, int pageIndex)
{
	ActionScriptVoid("UCUpdateGuildViewList");
}

event CallUpdateGuildViewInfo(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildViewInfo");
}

event CallUpdateGuildBuildingList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildBuildingList");
}

event CallSetGuildViewListSelectedIndex(int index)
{
	ActionScriptVoid("UCSetGuildViewListSelectedIndex");
}

event CallUpdateGuildLeaveCD(string cd)
{
	ActionScriptVoid("UCUpdateGuildLeaveCD");
}

event CallShowChallengeConfirm(GFxObject dataProvider)
{
	ActionScriptVoid("UCShowChallengeConfirm");
}

event CallEnableGuildWar(bool enableGuildWar)
{
	ActionScriptVoid("UCUpdateGuildWarButtonState");
}

// Apply to join guild
native final function ASApplyForJoint(int id);
native final function ASUpdateGuildViewInfo(int id);
// Find guild
native final function ASFindGuild(bool isId, String content);
// Turn page
native final function ASPageUp();
native final function ASPageDown();

native final function ASGuildWarSumitOpen(int id);
native final function ASChallengeConfirm(int id);

DefaultProperties
{
	IsDynamicLoad=true
	_CurPageIndex=1
	_IsNpcCalled=true
	_LockPage=false
	DefaultVisible=false
}
