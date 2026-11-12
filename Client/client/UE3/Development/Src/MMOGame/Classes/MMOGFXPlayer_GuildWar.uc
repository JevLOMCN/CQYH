class MMOGFXPlayer_GuildWar extends MMOGFXPlayer
	native(UI);

var MMOUIGuildWarEndPanel _GuildWarEndPanel;
var MMOUIGuildWarHintPanel _GuildWarHintPanel;
var MMOUIGuildWarStartPanel _GuildWarStartPanel;

function ASRegisterGuildWarEndPanel(MMOUIGuildWarEndPanel mc)
{ 
	_GuildWarEndPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_WarEnd);
}

function ASRegisterGuildWarHintPanel(MMOUIGuildWarHintPanel mc)
{ 
	_GuildWarHintPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_WarHint);
}

function ASRegisterGuildWarStartPanel(MMOUIGuildWarStartPanel mc)
{ 
	_GuildWarStartPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Guild_WarStart);
}

native function PostAdvance( float dt );

// Join battle
native final function ASJoinWar();

DefaultProperties
{
	MovieInfoStr="UI_GuildWarWidget.GuildWarWidget"
	Priority = 5
}
