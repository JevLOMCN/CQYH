#ifndef _MMOQUESTMANAGER_H_
#define _MMOQUESTMANAGER_H_

#include "MMOGameEvent.h"
#include "common.h"
#include "MMOUtil.h"
#include "MMOQuestData.h"

#define MIN_PAGENUMBER 1
#define MAX_PAGENUMBER 8

// quest events
struct MMOGameEvent_Quest : public MMOGameEvent
{
public:
	enum EQuestEventType
	{
		EQET_AcceptQuest,
		EQET_UpdateQuest,
		EQET_UpdateQuestTime,
		EQET_AbandonQuest,
		EQET_CompleteQuest,
		EQET_UpdateQuestTraceList,
		EQET_ClearQuestDetail,
		EQET_ShareQuest,
		EQET_EnterQuestZone,
		EQET_LeaveQuestZone,
		EQET_UpdateRewardQuestList,
		EQET_UpdateRewardQuestLimit,
		EQET_UpdateRewardQuestBrushTime,
		EQET_UpdateQuestPage,
	};

	MMOGameEvent_Quest(EQuestEventType eventType, INT questId = INVALID_QUEST_ID, INT shareSrcId = 0)
		: EventType(eventType)
		, QuestId(questId)
		, ShareSrcId(shareSrcId)
		, QuestGridType(-1)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Quest;
	}

	EQuestEventType EventType;
	INT QuestId;
	INT ShareSrcId;
	INT QuestGridType;
};


class MMOQuestManager : public SingletonMeyers<MMOQuestManager>, public INativeGameEventHandler
{
public:
	MMOQuestManager();
	virtual ~MMOQuestManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	// quest events
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	// quest NPC
	void UpdateQuestNpcStatus(class ANPC* npc);
	EQuestHUDStatus IsNpcHasMyQuest(INT npcId, TArray<INT>* completeList, TArray<INT>* uncompleteList, TArray<INT>* availableList);
	void UpdateNpcName(ANPC* npc);
	void UpdateNpcHp(ANPC* npc, INT hp);

	// drop triggers accept/submit quest
	void UpdateQuestDropItemStatus(class AMMODropActor* drop);

	// available quests
	bool AcceptQuest(INT questId, UBOOL share, INT shareSrcId) const;

	// complete quest
	bool CompleteQuest(INT questId, INT selectedAwardIndex);

	// quest tracking
	void TraceQuest(INT32 questId, bool traceOn);
	void TraceAllQuest(bool traceOn);
	UBOOL IsMonsterBeTraced(INT monsterId);

	void UpdateQuestStatus(UBOOL bForceUpdate = FALSE);
	void UpdateQuestTargets(INT32 questId);
	void UpdateAllQuestTargets();

	// bounty quest
	void InitRewardQuestList();
	void RefreshRewardQuestList(UBOOL autoRefresh);
	void SetRewardQuestRefreshTime(INT time);
	INT GetRewardQuestRefreshTime() {return _RewardQuestBrushLeftTime;}

	// flying shoes related
	UBOOL FlyToDestination(INT questId, INT destIndex);
	INT GetLastFlyShoesUseTimes() {_LastFlyShoesUseTimes;}

	// UI data
	UGFxObject* GetQuestTraceData(INT questId, UMMOGFXPlayer* gfxPlayer, EQuestTraceListType type);
	UGFxObject* GetQuestDetailData(INT questId, UMMOGFXPlayer* gfxPlayer, UBOOL isDialog);

	// quest sound effects
	void PlayQuestSound(INT questId, ANPC* npc);

	// quest completion conditions
	void UpdateQuestBuff(INT buffId);
	void UpdateQuestReputation();
	void UpdateQuestNpcKillCount(INT questId, UINT16 index, INT val);
	void UpdateRoleVariable(UINT16 index, INT val);
	void UpdateQuestVariable(INT questId, UINT16 index, INT val);
	void UpdateAllClientVariable(INT32 clientVariable[MAX_CLIENT_VARIABLE_COUNT]);
	void UpdateClientVariable(UINT16 index, INT val, INT classVal = 0);
	void UpdateQuestReachLevel(INT curLevel);
	void UpdateQuestDoodadKillCount(INT questId, UINT16 index, INT val);
	void UpdateQuestPlayerKillCount(INT questId, UINT16 index, INT val);
	void UpdateQuestItem(INT32 itemId, UBOOL isAdd);
	void UpdateAllQuestItem();
	void UpdateVariable(UINT8 varType, INT32 referId, UINT16 index, INT32 val);

	// quest page-turn dialogue
	void SetQuestDialogCurrentPage(INT page);
	INT GetQuestDialogCurrentPage() {return _CurPage;}

	INT GetPageTotalTime() {return _PageTime;}
	void RestPageTime() {_PageTimeProcess = 0;}

	void SetPageQuestId(INT id) {_CurPageQuestId = id;}
	INT GetPageQuestId() {return _CurPageQuestId;}

	INT GetMaxDialogPage();
	void ClearContext();
	FString GetContextContent(INT i);
	void ResetContent();

	void ParseQuestDialog(INT questId, UBOOL isDialog, DialogContext &dialogContext);
	DialogContext* GetQuestDialongContext() {return _QuestDialongContext;}

	// Net message
	void NotifyUpdateQuestList(ROLE_DB_QUEST_DATA* questData);
	void NotifyAcceptQuest(INT questId);
	void NotifyAbandonQuest(INT questId);
	void NotifyCompleteQuest(INT questId);
	void NotifyQuestFailed(INT questId);
	void NotifyShareQuest(INT questId, INT shareSrcId);
	void NotifyNextCircleQuest(INT questId, INT circleId);
	void NotifyUpgradeRewardQuestLimit(INT limit);
	void NotifyRefreshRewardQuestList(const INT32 (&rewardQuestId)[MAX_REWARD_QUEST_COUNT_PER_ROLE], UBOOL isClear);
	void NotifyPlayerLevelChange(INT curLevel);
	void NotifyFlyShoesUseTime(INT times);

	void GetQuestStateIcon(INT type, EQuestHUDStatus state, UBOOL isSmall, FString& icon);

	UBOOL IsEmergencyQuestBeTraced(INT questId);
	INT GetCurQuestGridQuestId(INT index);

	void SetCurTrunkQuestId(INT questId);
	void SetCurOtherQuestId(INT questId);
	INT GetCurTrunkQuestId() { return _CurTrunkQuestId; }
	INT GetCurOtherQuestId() { return _CurOtherQuestId; }

	void AddQuestCoolDown(INT questId, UINT32 coolDown);

protected:
	// UI data
	// accepted quest list
	void _SetAcceptedQuestTraceListItemTargetCheckTalkNpc(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetAcceptedQuestTraceListItemTargetCheckLevel(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetAcceptedQuestTraceListItemTargetCheckVariable(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetAcceptedQuestTraceListItemTargetCheckItem(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetAcceptedQuestTraceListItemTargetCheckKillNpc(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetAcceptedQuestTraceListItemTargetCheckKillDoodad(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetAcceptedQuestTraceListItemTargetCheckKillPlayer(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetAcceptedQuestTraceListItemTargetCheckReputation(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);

	void _SetAcceptedQuestTraceListItemCaption(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetAcceptedQuestTraceListItemDesc(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetAcceptedQuestTraceListItemTarget(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetAcceptedQuestTraceListItemTime(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetAcceptedQuestTraceListItemAward(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);

	// available quest list
	void _SetAvailableQuestTraceListItemCaption(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetAvailableQuestTraceListItemDesc(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);

	// quest details
	void _SetQuestDetailTargetCheckTalkNpc(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetQuestDetailTargetCheckLevel(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetQuestDetailTargetCheckVariable(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetQuestDetailTargetCheckItem(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetQuestDetailTargetCheckKillNpc(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetQuestDetailTargetCheckKillDoodad(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetQuestDetailTargetCheckKillPlayer(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);
	void _SetQuestDetailTargetCheckReputation(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index);

	void _SetQuestDetailBaseInfo(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetQuestDetailCaption(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetQuestDetailDesc(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object, UBOOL isDialog);
	void _SetQuestDetailCondition(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetQuestDetailTarget(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetQuestDetailSubmit(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void _SetQuestDetailAward(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object);

	// transit NPC check
	void _CheckTransitQuestNpc(const UQuest* quest);

	// quest NPC
	void _OnNPCInteract(INT serverId, INT npcId);
	void _GetMonsterHp(INT npcId, INT& curHp, INT& maxHp);
	void _SetNpcIcon(class ANPC* npc, const FString& iconName);

	// timed quest
	void _UpdateQuestTime(FLOAT deltaTime);

	// bounty quest
	void _UpdateRewardQuest(FLOAT deltaTime);

	// area quest
	void _ExploreZoneFromGrid(ZONE_DATA* zoneData, GRID_DATA* gridData);
	void _AcceptQuestFromGrid(ZONE_DATA* zoneData, GRID_DATA* gridData);
	void _CompleteQuestFromGrid(ZONE_DATA* zoneData, GRID_DATA* gridData);
	void _UpdateQuestFromGrid(FLOAT deltaTime);
	void _CheckQuestFromGrid();
	void _UpdateQuestCoolDown(FLOAT deltaTime);
	void _UpdateQuestStatusTime(FLOAT deltaTime);

	// quest dialogue page turn
	void _UpdateQuestPage(float deltaTime);

	void _UpdateQuestStatus();
	void _LevelLoaded();

	void _GetQuestAwardMoneyInfo(INT moneyType, FString& moneyName, FString& moneyIcon);

	void _ProcessFirstEnterScene();

protected:
	FString _CondCompleteColor;
	FString _CondNotCompleteColor;
	FString _CondComplete;

	// flying shoes
	INT _CurFlyQuestId;
	INT _CurFlyDestId;
	// remaining flying shoes uses
	INT _LastFlyShoesUseTimes;	

	// bounty quest
	UBOOL _IsAutoRefresh;
	UBOOL _HasRewardQuest;
	INT _RewardQuestBrushLeftTime;
	FLOAT _RewardQuestBrushLeftTimeProcess;

	// area quest
	INT _CurQuestGridType;
	INT _CurQuestGridQuestId[MAX_QUEST_PER_ZONE];
	FLOAT _QuestGridQueryTimeProcess;

	// quest dialogue pagination
	INT _CurPage;
	INT _PageTime;
	FLOAT _PageTimeProcess;
	DialogContext* _QuestDialongContext;
	INT _CurPageQuestId;

	// timed quest
	FLOAT _LimitedTimeQuestProcess;

	// trigger cutscene on first entering the game
	UBOOL _FirstMatinee;

	// record current main/non-main quest ID, used for quest panel display
	INT _CurTrunkQuestId;
	INT _CurOtherQuestId;

	// emergency quest CD
	FLOAT _QuestCoolDownTimeProcess;

	// quest state update flag
	UBOOL _NeedUpdateQuestStatus;
	FLOAT _QuestStatusTimeProcess;
	UBOOL _FirstEnterScene;
};

#endif
