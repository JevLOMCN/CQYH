class MMOUIGuildWelfarePanel extends MMOUIBase
	native(UI);

var int _CurPageIndex;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

// UC->AS
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("UCSetWindowTitle");
}

event CallUpdateGuildWelfarePurchaseList(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateGuildWelfarePurchaseList");
}

event CallSetGuildWelfarePage(int curPage, int maxPage)
{
	ActionScriptVoid("UCSetGuildWelfarePage");
}

// Turn page
native final function ASWelfarePageUp();
native final function ASWelfarePageDown();
native final function ASPurchaseWelfare(int id);

DefaultProperties
{
	IsDynamicLoad=true
	DefaultVisible=false
}
