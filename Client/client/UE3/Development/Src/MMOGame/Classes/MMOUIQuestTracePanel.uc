class MMOUIQuestTracePanel extends MMOUIBase
	dependson(Quest)
	native(UI);

enum EQuestTraceListType
{
	EQTLT_Accepted,
	EQTLT_Available,
};

enum EQuestTraceTargetProgressType
{
	EQTTPT_Null,
	EQTTPT_CountProgress,
	EQTTPT_HpProgress,
};

var int _QuestState;
var bool _IsExpand;

native function OnFocusGained();
native function OnFocusLost();

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

event CallSetQuestTraceType(int type)
{
	ActionScriptVoid("UCSetQuestTraceType");
}

event CallAddAcceptedQuest(GFxObject provider)
{
	ActionScriptVoid("UCAddAcceptedQuest");
}

event CallRemoveAcceptedQuest(int questId)
{
	ActionScriptVoid("UCRemoveAcceptedQuest");
}

event CallUpdateAcceptedQuest(GFxObject provider)
{
	ActionScriptVoid("UCUpdateAcceptedQuest");
}

event CallUpdateAcceptedQuestTraceList(GFxObject provider)
{
	ActionScriptVoid("UCUpdateAcceptedQuestTraceList");
}

event CallUpdateAvailableQuestTraceList(GFxObject provider)
{
	ActionScriptVoid("UCUpdateAvailableQuestTraceList");
}

native final function ASUpdateAcceptedQuestTraceList();
native final function ASUpdateAvailableQuestTraceList();
// Flash has various sync problems, changed to C++ timing
//native final function ASUpdateQuestTimeout(int questId, int timeSecondsLeft);
//native final function ASOnTimeLimitQuestTimeout(int questId);
native final function ASUseQuestItem(int questId, int itemId);
native final function ASDelayComplete(int questId);

DefaultProperties
{
	_QuestState=0
	_IsExpand=true
	DefaultVisible=false
	CloseOnEsc=false
}