class MMOGFXPlayer_Quest extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIQuestPanel _QuestPanel;
var MMOUIQuestRewardPanel _QuestRewardPanel;
var MMOUIQuestRewardBuyTimesPanel _QuestRewardBuyTimesPanel;
var MMOUIQuestStoryPanel _QuestStoryPanel;
var MMOUIQuestItemCollectionPanel _QuestItemCollectionPanel;

function ASRegisterQuestPanel(MMOUIQuestPanel mc)
{
	_QuestPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_Quest);
}

function ASRegisterQuestRewardPanel(MMOUIQuestRewardPanel mc)
{
	_QuestRewardPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_QuestReward);
	//self.OnPostAdvance = _QuestRewardPanel.TickForTestInteractNPCDistance;
}

function ASRegisterQuestRewardBuyTimesPanel(MMOUIQuestRewardBuyTimesPanel mc)
{
	_QuestRewardBuyTimesPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_QuestRewardBuyTimes);
	//self.OnPostAdvance = _QuestRewardBuyTimesPanel.TickForTestInteractNPCDistance;
}

function ASRegisterQuestStoryPanel(MMOUIQuestStoryPanel mc)
{
	_QuestStoryPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_QuestStory);
}

function ASRegisterQuestItemCollectionPanel(MMOUIQuestItemCollectionPanel mc)
{
	_QuestItemCollectionPanel = mc;
	RegisterUI(mc, class'MMOUIManager'.default.Panel_QuestItemCollection);
}

native final function ASUpdateQuestList(int group);
native final function ASUpdateQuestDetail(int id);
native final function ASTraceQuest(int id, bool traced);
native final function ASTraceAllQuest(bool traced);
native final function ASFilterLowLevelQuest(bool showLowLvQuest);
native final function ASShareQuest(int id);
native final function AbortQuest(int id);
native final function ASAbortQuest(int id);
native final function ASAcceptQuestWithoutNpc(int id);
native final function ASCompleteQuestWithoutNpc(int id);
native final function ASSelectAward(GFxObject icon, int idx, int itemTemplateID);
// bounty quest
native final function ASUpdateRewardQuestDetail(int id);
native final function ASUpgradeAvailableLimit();
native final function ASRefreshRewardQuestList(bool autoRefresh);
native final function ASCompleteAllRewardQuest();
native final function ASShowCompletedRewardQuest(bool show);
native final function ASRefreshTopLevelRewardQuest(bool refresh);
native final function ASAcceptRewardQuest(int id);
native final function ASAbandonRewardQuest(int id);
native final function ASUpdateRewardQuestBuyTimes(int times);
native final function ASBuyTimes(int times);
// story quest
native final function ASUpdateCurQuestInfo();
native final function ASUpdateQuestChapterList();
native final function ASUpdateQuestStageList(int chapter);
native final function ASViewStoryQuestStageInfo(int chapter, int stage);

// Item recycling quest
native final function ASSubmitItem(int questId, int itemId, int itemIndex);

function AbortQuestOK(GFxObject mc)
{
	local int questID;
	questID = mc.GetInt("iteminfoid");
	AbortQuest(questID);
}

function AbortQuestCanel(GFxObject mc)
{
}

function RefreshTopLevelRewardQuestOK(GFxObject mc)
{
}

function RefreshTopLevelRewardQuestCancel(GFxObject mc)
{
}

DefaultProperties
{
	MovieInfoStr="UI_Quest.QuestWidgets"
	Priority = 5
}
