class MMOGFXPlayer_QuestTrace extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIQuestTracePanel _QuestTracePanel;

function ASRegisterQuestTracePanelNew(MMOUIQuestTracePanel mc)
{
	_QuestTracePanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_QuestTrace);
}

native final function ASUpdateAcceptedQuestTraceList();
native final function ASUpdateAvailableQuestTraceList();
// Flash has various sync problems, changed to C++ timing
//native final function ASUpdateQuestTimeout(int questId, int timeSecondsLeft);
//native final function ASOnTimeLimitQuestTimeout(int questId);
native final function ASUseQuestItem(int questId, int itemId);
native final function ASDelayComplete(int questId);

event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)
{
	if (GetlockMouseCursorForUI() == false)
	{
		if (strHoverControllerName == "quest_trace_list_mask_mc" )
		{
			return false;
		}
		else if (strHoverControllerName == "quest_trace_list_mc" )
		{
			return false;
		}
		else if (strHoverControllerName == "quest_trace_list_bg_mc" )
		{
			return false;
		}
	}

	return super.SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

DefaultProperties
{
	MovieInfoStr="UI_QuestTrace.QuestTrace"
	Priority=1
}
