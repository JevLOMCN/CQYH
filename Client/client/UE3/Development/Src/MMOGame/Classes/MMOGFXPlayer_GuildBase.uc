class MMOGFXPlayer_GuildBase extends MMOGFXPlayer
	native(UI);

var MMOUIGuildCreatePanel _GuildCreatePanel;
var MMOUIGuildViewPanel _GuildViewPanel;
var MMOUIGuildMainPanel _GuildMainPanel;
var MMOUIGuildWarehouse _GuildWarehouse;
var MMOUIGuildInstructionPanel _GuildInstructionPanel;
var MMOUIGuildWarViewPanel _GuildWarViewPanel;
var MMOUISabacEnrollViewPanel _SabacEnrollViewPanel;

function ASRegisterGuildCreatePanel(MMOUIGuildCreatePanel mc)
{ 
	_GuildCreatePanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_Create);
}

function ASRegisterGuildViewPanel(MMOUIGuildViewPanel mc)
{ 
	_GuildViewPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_View);
}

function RegisterGuildWarehouse(MMOUIGuildWarehouse mc)
{ 
	_GuildWarehouse = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_Warehouse);
}

function ASRegisterGuildMainPanel(MMOUIGuildMainPanel mc)
{ 
	_GuildMainPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_Main);
}

function ASRegisterGuildInstructionPanel(MMOUIGuildInstructionPanel mc)
{ 
	_GuildInstructionPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_Instruction);
}

function ASRegisterGuildWarViewPanel(MMOUIGuildWarViewPanel mc)
{ 
	_GuildWarViewPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_WarView);
}

function ASRegisterSabacEnrollViewPanel(MMOUISabacEnrollViewPanel mc)
{ 
	_SabacEnrollViewPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_SabacEnroll);
}

native function PostAdvance( float dt );

native final function ShowGuildCreatePanel();
native final function ShowGuildViewPanel();
// Create guild
native final function ASCreateGuild(String guildName, String guildAnnounce);
native final function ASInputError();

// View guild
native final function ASViewGuild();
native final function ASUpdateGuildViewInfo(int id);
// Find guild
native final function ASFindGuild(bool isId, String content);
// Turn page
native final function ASPageUp();
native final function ASPageDown();
// Warehouse
native final function ASGuildWarehouseMore();
native final function ASGuildWarehouseUpdate();
native final function ASGuildWarehouseTab( int tabID );
// Apply to join guild
native final function ASApplyForJoint(int id);
// Update application list
native final function ASUpdateGuildPendingMemberList();
// Accept joining guild
native final function ASAgreeToJoin(int id);
// Reject joining guild
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
// Expel member
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

// Guild war
native final function ASChallenge(String guildName);
native final function ASMatchChallenge(int id);
native final function ASSearchGuild();
native final function ASChallengeConfirm(int guildId);

DefaultProperties
{
	MovieInfoStr="UI_GuildBaseWidget.GuildBaseWidget"
	Priority = 5
}
