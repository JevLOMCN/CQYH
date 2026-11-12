class MMOUIGuildDungeonBeginPanel extends MMOUIBase
	native(UI);

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

// UC->AS
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("UCSetWindowTitle");
}

event CallUpdateGuildDungeonList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildDungeonList");
}

event CallUpdateGuildMemberList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildMemberList");
}

// Guild dungeon
native final function ASOpenDungeon(int dungeonId, int leaderId);

DefaultProperties
{
	IsDynamicLoad=true
	DefaultVisible=false
}
