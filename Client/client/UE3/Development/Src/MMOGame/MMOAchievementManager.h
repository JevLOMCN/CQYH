#ifndef _MMOACHIEVEMENTMANAGER_H_
#define _MMOACHIEVEMENTMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOUtil.h"
#include "MMOAchievementData.h"
#include <queue>

struct MMOGameEvent_Achievement : public MMOGameEvent
{
public:
	enum EAchievementEventType
	{
		EAET_UpdateAchievementList = adTotal + 1,
	};

	INT _EventType;

	UINT32 _Value1;
	UINT32 _Value2;
	UINT32 _Value3;

	UINT32 _Id;

	MMOGameEvent_Achievement(UINT32 eventType, UINT32 value1, UINT32 value2, UINT32 value3)
		: _EventType(eventType), 
		_Value1(value1), 
		_Value2(value2),
		_Value3(value3),
		_Id(0)
	{
	}

	MMOGameEvent_Achievement(UINT32 eventType)
		: _EventType(eventType),
		_Value1(0), 
		_Value2(0),
		_Value3(0),
		_Id(0)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Achievement;
	}
};

class MMOAchievementManager : public SingletonMeyers<MMOAchievementManager>, public INativeGameEventHandler
{
public:
	MMOAchievementManager();
	virtual ~MMOAchievementManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void SetCompare(UBOOL isCompare);
	UBOOL IsCompare() { return _IsCompare; }
	const FString& GetRoleNameCompare() { return _RoleNameCompare; }
	void CompleteCommonAchievement(UINT32 id);

	// Message
	void NotifyAchievementListUpdate(const char* list);
	void NotifyAchievementVariableUpdate(INT32 index, INT32 value);
	void NotifyAchievementCompareListUpdate(UINT32 roleId, const char* list);

	void NotifySyncKillNpcAchievementData(UINT16 killCount[MAX_KILL_NPC_QUEST_COUNT], UINT8 complete[MAX_KILL_NPC_QUEST_COUNT]);
	void NotifyUpdateKillNpcAchievementInfo(UINT16 killId, UINT16 killCount);

	void NotifySyncGuildAchievementInfo(char info[GUILD_MAX_ACHI_ARRAY_LENGTH]);
	void NotifyUpdateGuildAchievementInfo(char info[GUILD_ACHI_TYPE_NUM]);
	void NotifyPlayerLevelChange(INT lastLevel, INT curLevel);

	void NotifyAchievementComplete(UINT32 id, UINT32 date);
	void NotifyAchievementRewardObtain(UINT32 id);

protected:
	UBOOL _BeginSyncAchievementCheck;
	// Send packet cache
	AchievementIdSet _AchievementCompleteSet;
	// Compare
	UBOOL _IsCompare;
	FString _RoleNameCompare;
};

#endif