class MMOUIGuildMainPanel extends MMOUIBase
	native(UI);

var GFxObject ExpelConfirmBox;
var GFxObject QuitConfirmBox;
var bool IsShowOnlineMember;
var string _WarTargetName;
var GFxObject ChallengeConfirmBox;
var int _ShowPage;
var int _ShowSubPage;
var GFxObject _AllyCancelConfirmBox;
var GFxObject _ContributeConfirmBox;

native function PlayerMoneyChange(EMONEY_TYPE MoneyType, INT DeltaMoney);

native function ReplyAgreeExpel(GFxObject mc);
native function ReplyRefuseExpel(GFxObject mc);

native function ReplyAgreeQuit(GFxObject mc);
native function ReplyRefuseQuit(GFxObject mc);

native function AgreeChallengeNow(GFxObject mc);
native function RefuseChallengeNow(GFxObject mc);

native function AgreeAllyCancel(GFxObject mc);
native function RefuseAllyCancel(GFxObject mc);

native function AgreeContribute(GFxObject mc);
native function RefuseContribute(GFxObject mc);

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

// UC->AS
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("UCSetWindowTitle");
}

event CallInitGuild(bool hasGuild)
{
	ActionScriptVoid("UCInitGuild");
}

event CallUpdateGuildFullMemberList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildFullMemberList");
}

event CallUpdateGuildPendingMemberList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildPendingMemberList");
}

event CallUpdateGuildBaseInfo(GFxObject object)
{
	ActionScriptVoid("UCUpdateGuildBaseInfo");
}

event CallUpdateGuildNotice(String notice)
{
	ActionScriptVoid("UCUpdateGuildNotice");
}

event CallUpdateGuildEvents(String events)
{
	ActionScriptVoid("UCUpdateGuildEvents");
}

event CallUpdateGuildMoneyEvents(String events)
{
	ActionScriptVoid("UCUpdateGuildMoneyEvents");
}

event CallUpdateGuildAllyList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildAllyList");
}

event CallUpdateGuildEnemyList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildEnemyList");
}

event CallUpdateGuildApplyAllyList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildApplyAllyList");
}

event CallUpdateGuildRights(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildRights");
}

event CallUpdateGuildStorageRights(GFxObject dataProvider, string tips)
{
	ActionScriptVoid("UCUpdateGuildStorageRights");
}

event CallUpdateGuildStorageState(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildStorageState");
}

event CallUpdateGuildSalary(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildSalary");
}

event CallUpdateContributeInfo(int maxMoney, string intro)
{
	ActionScriptVoid("UCUpdateContributeInfo");
}

event CallUpdateGuildWarInfo(GFxObject object)
{
	ActionScriptVoid("UCUpdateGuildWarInfo");
}

event CallUpdateGuildWarCD(string time)
{
	ActionScriptVoid("UCUpdateGuildWarCD");
}

event CallUpdateForeignApplyConsume(String money)
{
	ActionScriptVoid("UCUpdateForeignApplyConsume");
}

event CallUpdateGuildWarSearchState(GFxObject object)
{
	ActionScriptVoid("UCUpdateGuildWarSearchState");
}

event CallUpdateGuildWarSearchInfo(GFxObject object)
{
	ActionScriptVoid("UCUpdateGuildWarSearchInfo");
}

event CallUpdateGuildIntroduce(String intro)
{
	ActionScriptVoid("UCUpdateGuildIntroduce");
}

event CallShowGuildMainPage(int page, int subPage)
{
	ActionScriptVoid("UCShowGuildMainPage");
}

event CallInitShowOnlineMember()
{
	ActionScriptVoid("UCClear");
}

event CallUpdateViewMoreHistoryState(bool enable)
{
	ActionScriptVoid("UCSetMoreButtonState");
}

event CallUpdateGuildActiveList(GFxObject object)
{
	ActionScriptVoid("UCUpdateGuildActive");
}

event CallUpdateGuildActiveListItem(GFxObject object)
{
	ActionScriptVoid("UCGuildActiveOpenData");
}

event CallShowActiveOpenConfirm(GFxObject object)
{
	ActionScriptVoid("UCShowActiveOpenConfirm");
}

event CallHideActiveOpenConfirm()
{
	ActionScriptVoid("UCHideActiveOpenConfirm");
}

event CallEnableGuildWar(bool enableGuildWar)
{
	ActionScriptVoid("UCUpdateGuildWarButtonState");
}

// guild view
native final function ASViewGuild();
// Update application list
native final function ASUpdateGuildPendingMemberList();
// Agree to join guild
native final function ASAgreeToJoin(int id);
// Refuse to join guild
native final function ASRefuseToJoin(int id);
// Resource donation
native final function ASContribute(int type, int count);
// Change announcement
native final function ASChangeNotice(String notice);
// Change declaration
native final function ASChangeAnnounce(String announce);
// View more guild events
native final function ASViewMoreEvents();
// Recruit members
native final function ASInviteMember(String roleName);
native final function ASSortMember(int type, bool isUp);
// Expel member from guild
native final function ASExpelMember(int id);
// Change position
native final function ASChangePost(int roleId, int post);
// Whether to show online members
native final function ASShowOnlineMember(bool isShow);
// Member online notification
native final function ASOnlineMemberNotify(bool notify);
// Guild leader abdication
native final function ASLeaderDemise(String roleName);
// Guild disband
native final function ASDisbandGuild(String text);
// Leave guild
native final function ASQuitGuild();
// Update guild permission display
native final function ASUpdateGuildRights(int post);
// Update warehouse permission display
native final function ASModifyGuildStorageRights(int post, GFXObject rights);
native final function ASUpdateGuildStorageRights(int post);
// Salary distribution
native final function ASDistributeSalary(GFXObject percent);
native final function ASApplyAlly(String guildName, int timeType);
native final function ASApplyEnemy(String guildName, int timeType);
native final function ASAgreeAlly(int id);
native final function ASRefuseAlly(int id);
native final function ASFullMemberRightClick(int posX, int posY, int roleId);
native final function ASForeignApplyDayType(int foreignType, int dayType);
native final function ASAllianceRightClick(int posX, int posY, int guildId);
native final function ASGuildEnemyRightClick(int posX, int posY, int guildId);
// guild war
native final function ASChallenge(String guildName);
native final function ASSearchGuild();
native final function ASSetShowPage(int page, int subPage);
// guild activity
native final function ASGuildActivityClickOpen(int activityId);
native final function ASGuildActivitySumitOpen(int activityId);

DefaultProperties
{
	_ShowPage=1
	_ShowSubPage=1
	IsDynamicLoad=true
	DefaultVisible=false
	IsShowOnlineMember=false
}
