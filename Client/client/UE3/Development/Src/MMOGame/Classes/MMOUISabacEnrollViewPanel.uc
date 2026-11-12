class MMOUISabacEnrollViewPanel extends MMOUIBase
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

event CallUpdateSabacEnrollList(GFxObject dataProvider, int pageIndex)
{
	ActionScriptVoid("UCUpdateSabacEnrollList");
}

event CallUpdateDesc(string desc)
{
	ActionScriptVoid("UCUpdateDec");
}

// Turn page
native final function ASEnrollPageUp();
native final function ASEnrollPageDown();

DefaultProperties
{
	IsDynamicLoad=true
	_CurPageIndex=1
	DefaultVisible=false
}
