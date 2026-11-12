class MMOUIGuildCreatePanel extends MMOUIBase
	config(MMOGame)
	native(UI);

var config int CreateItemId;
var config int CreateMoney;
var config int CreateLevel;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

// UC->AS
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("UCSetWindowTitle");
}

event CallSetMoneyRequest(string money)
{
	ActionScriptVoid("UCSetMoneyRequest");
}

event CallSetItemRequest(string itemName, string itemIcon, int itemId)
{
	ActionScriptVoid("UCSetItemRequest");
}

event CallCreateSuccess()
{
	ActionScriptVoid("UCCreateSuccess");
}


// Create guild
native final function ASCreateGuild(String guildName, String guildAnnounce);
native final function ASInputError();

DefaultProperties
{
	IsDynamicLoad=true
	DefaultVisible=false
}
