#ifndef _MMOACHIEVEMENTDATA_H_
#define _MMOACHIEVEMENTDATA_H_

#include "MMOSingleton.h"
#include "MMOUtil.h"
#include "role_db_def.h"
#include "MMODataManager.h"

#define M_DEBUG_ACHIEVEMENT 0
#define KILL_NPC_ACHIEVEMENT_MIN (1 << 16)

// Achievement id ranges:
// Normal achievement: id >= 0 && id < ACHIEVEMENT_MAX
// Guild achievement: id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM
// Monster-kill achievement: id >= KILL_NPC_ACHIEVEMENT_MIN

enum
{
	MAX_CONDITION_TYPE_COUNT = 6,
	MAX_REWARD_COUNT = 3,
	MAX_RECENT_COMPLETE_COUNT = 6,
	MAX_PRE_ACHIEVEMENT_COUNT = 2,
	MAX_KILL_NPC_ACHIEVEMENT_COUNT = 8,
	MAX_TRACE_ACHIEVEMENT_COUNT = 2,
	MAX_GUILD_ACHIEVEMENT_AWARD_COUNT = 5,
	MAX_GUILD_ACHIEVEMENT_CONDITION_COUNT = 4,
};

enum AchievementType
{
	AT_NORMAL = 0,
	AT_GUILD
};

enum AchievementClass
{
	AC_ROLE = 0,
	AC_ACCUMULATION,
	AC_FIGHT,
	AC_ADVENTURE,
	AC_DATE,
	AC_EPIC,
	AC_GUILD1,
	AC_GUILD2,
	AC_GUILD3,
	AC_TOTAL
};

// Achievement expand type
enum AchievementDetailType
{
	ADT_NULL = 0,				// no details
	ADT_CONDITION,				// condition type
	ADT_PROGRESS				// counting type shows progress
};

// Achievement data
struct AchievementData
{
	UINT16 Id;
	FString Name;
	FString Desc;
	FString Icon;
	UINT32 BaseClass;
	UINT32 SubClass;
	UINT32 DetailType;
	UINT32 Point;
	UBOOL Hide;
	UBOOL AutoTrace;
	UINT32 MinTraceLevel;
	UINT32 MaxTraceLevel;

	INT ConditionType[MAX_CONDITION_TYPE_COUNT];
	FString ConditionDesc[MAX_CONDITION_TYPE_COUNT];
	INT ConditionValue1[MAX_CONDITION_TYPE_COUNT];
	INT ConditionValue2[MAX_CONDITION_TYPE_COUNT];
	INT ConditionValue3[MAX_CONDITION_TYPE_COUNT];

	UINT32 RewardType[MAX_REWARD_COUNT];
	UINT32 RewardValue[MAX_REWARD_COUNT];
	UINT32 RewardPoint;
	UINT32 RewardTitle;

	UINT32 PreAchievement[MAX_PRE_ACHIEVEMENT_COUNT];
};

struct AchievementDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("achievement.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static AchievementData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(AchievementData* obj);
	static void ParseData(AchievementData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(AchievementData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(AchievementData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<AchievementData, AchievementDataProcessor> AchievementDataManager;
typedef std::map<UINT16, struct AchievementData, M_UE3_STL_ALLOC_MAP(UINT16, AchievementData)> AchievementDataMap;

//////////////////////////////////////////////////////////////////////////
// Monster-kill achievement data
struct KillNpcAchievementData
{
	UINT16 KillId;
	UINT32 NpcTemplateId;
	UINT32 NpcGroupId;
	FString Desc;
	FString Icon;
	UINT32 BaseClass;
	UINT32 SubClass;

	FString Name[MAX_KILL_NPC_ACHIEVEMENT_COUNT];
	INT KillCount[MAX_KILL_NPC_ACHIEVEMENT_COUNT];
	UINT32 Point[MAX_KILL_NPC_ACHIEVEMENT_COUNT];
	UINT32 RewardExp[MAX_KILL_NPC_ACHIEVEMENT_COUNT];
	UBOOL Hide[MAX_KILL_NPC_ACHIEVEMENT_COUNT];
	UBOOL AutoTrace[MAX_KILL_NPC_ACHIEVEMENT_COUNT];
	UINT32 MinTraceLevel[MAX_KILL_NPC_ACHIEVEMENT_COUNT];
	UINT32 MaxTraceLevel[MAX_KILL_NPC_ACHIEVEMENT_COUNT];
};

struct KillNpcAchievementDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("kill_npc_quest.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static KillNpcAchievementData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(KillNpcAchievementData* obj);
	static void ParseData(KillNpcAchievementData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("QuestID");
	}

	static FString FormatAsString(KillNpcAchievementData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(KillNpcAchievementData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<KillNpcAchievementData, KillNpcAchievementDataProcessor> KillNpcAchievementDataManager;
typedef std::map<UINT16, struct KillNpcAchievementData, M_UE3_STL_ALLOC_MAP(UINT16, KillNpcAchievementData)> KillNpcAchievementDataMap;

//////////////////////////////////////////////////////////////////////////
// Guild achievement data
struct GuildAchievementData
{
	UINT16 Id;
	FString Name;
	FString Desc;
	FString Icon;
	UINT32 BaseClass;
	UINT32 SubClass;
	UBOOL Hide;
	
	UINT16 ConditionType[MAX_GUILD_ACHIEVEMENT_CONDITION_COUNT];
	UINT16 ConditionParam[MAX_GUILD_ACHIEVEMENT_CONDITION_COUNT];

	UINT32 RewardType[MAX_GUILD_ACHIEVEMENT_AWARD_COUNT];
	UINT32 RewardTemp[MAX_GUILD_ACHIEVEMENT_AWARD_COUNT];
	UINT32 RewardCount[MAX_GUILD_ACHIEVEMENT_AWARD_COUNT];
};

struct GuildAchievementDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("guild_achievement.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static GuildAchievementData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(GuildAchievementData* obj);
	static void ParseData(GuildAchievementData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(GuildAchievementData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(GuildAchievementData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<GuildAchievementData, GuildAchievementDataProcessor> GuildAchievementDataManager;
typedef std::map<UINT16, struct GuildAchievementData, M_UE3_STL_ALLOC_MAP(UINT16, GuildAchievementData)> GuildAchievementDataMap;

// Guild condition data
struct GuildConditionData
{
	UINT16 ConditionType;
	UINT16 ConditionParam;
	UINT16 ConditionPos;
};

struct GuildConditionDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("guild_achi_cdtn.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static GuildConditionData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(GuildConditionData* obj);
	static void ParseData(GuildConditionData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("CdtnType");
	}

	static FString FormatAsString(GuildConditionData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(GuildConditionData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<GuildConditionData, GuildConditionDataProcessor> GuildConditionDataManager;
typedef std::map<UINT32, struct GuildConditionData, M_UE3_STL_ALLOC_MAP(UINT32, GuildConditionData)> GuildConditionDataMap;

//////////////////////////////////////////////////////////////////////////
// Achievement completion info
struct AchievementInfo
{
	// Basic info
	UINT32 Id;					
	UINT32 Date;				// obtain time
	UBOOL IsTraced;				// whether tracked
	UBOOL IsCompassable;		// whether achievement is completed
	UBOOL IsObtain;				// whether achievement is obtained
	UBOOL IsGetReward;			// whether reward is obtained

	// Completion info
	INT CurProgress;			// counting type (may be negative, e.g. reputation)
	INT MaxProgress;								
	UINT8 CurComplete;		    // condition type

	// Comparison info
	UBOOL IsObtainCompare;
	UINT32 DateCompare;

	AchievementInfo()
	{
		Id = 0;
		Date = 0;
		IsTraced = FALSE;
		IsCompassable = FALSE;
		IsObtain = FALSE;
		IsGetReward = FALSE;

		CurProgress = 0;
		MaxProgress = 1;
		CurComplete = 0;

		IsObtainCompare = FALSE;
		DateCompare = 0;
	}
};

typedef std::map<UINT32, struct AchievementInfo, M_UE3_STL_ALLOC_MAP(UINT32, AchievementInfo)> AchievementInfoMap;

typedef std::list<UINT32, M_UE3_STL_ALLOC(UINT32)> AchievementIdList;
typedef std::set<UINT32> AchievementIdSet;
// Achievement category list, the high bits of UIN32 here are BaseClass, the low bits are SubClass
typedef std::map<UINT32, AchievementIdList, M_UE3_STL_ALLOC_MAP(UINT32, AchievementIdList)> AchievementClassMap;

class MMOAchievementData : public SingletonMeyers<MMOAchievementData>
{
public:
	MMOAchievementData();
	virtual ~MMOAchievementData();

	void Init();
	void Shutdown();
	void Clear();

	static const FString S_AchievementClassName[AC_TOTAL];

	INT GetEquipPartId(const FString& enumStr);
	INT GetGuildConditionType(const FString& enumStr);
	INT GetGuildAwardType(const FString& enumStr);

	void AddCommonAchievementData(const AchievementData& data);
	UBOOL GetCommonAchievementData(UINT16 id, AchievementData& data);
	const AchievementDataMap& GetCommonAchievementDataMap();

	void AddKillNpcAchievementData(const KillNpcAchievementData& data);
	UBOOL GetKillNpcAchievementData(UINT16 killId, KillNpcAchievementData& data);
	const KillNpcAchievementDataMap& GetKillNpcAchievementDataMap();
	UINT32 GetKillNpcAchievementId(UINT16 killId, UINT16 killIndex);

	void AddGuildAchievementData(const GuildAchievementData& data);
	UBOOL GetGuildAchievementData(UINT16 id, GuildAchievementData& data);
	const GuildAchievementDataMap& GetGuildAchievementDataMap();
	UINT16 GetGuildAchievementDataId(UINT32 infoId);

	void AddGuildConditionData(const GuildConditionData& data);
	UBOOL GetGuildConditionPos(UINT16 condType, UINT16 condParam, UINT16& condPos);

	UBOOL GetAchievementInfo(UINT32 id, AchievementInfo& info);
	UINT32 GetAchievementClassKey(UINT32 baseClass, UINT32 subClass);
	const AchievementClassMap& GetAchievementClassMap();

	// Data update
	void UpdateCommonAchievementList(ROLE_DB_ACHIEVEMENT_DATA* list);
	void UpdateCommonAchievementInfo(INT type, INT value1, INT value2, INT value3);

	void UpdateKillNpcAchievementList(UINT16 killCount[MAX_KILL_NPC_QUEST_COUNT], UINT8 complete[MAX_KILL_NPC_QUEST_COUNT]);
	void UpdateKillNpcAchievementInfo(UINT16 killId, UINT16 killCount);

	void UpdateGuildAchievementList(char info[GUILD_MAX_ACHI_ARRAY_LENGTH]);
	void UpdateGuildAchievementInfo(char info[GUILD_ACHI_TYPE_NUM]);
	void ClearGuildAchievementInfo();

	void UpdateAchievementVariable(INT32 index, INT32 value);
	void UpdateAchievementCompareList(ROLE_DB_ACHIEVEMENT_DATA* list);

	void UpdateAchievementCompleteInfo(UINT32 id, UINT32 date);
	void UpdateAchievementRewardObtain(UINT32 id);

	// Data access
	UBOOL IsAchievementObtain(UINT32 id);
	UBOOL IsAchievementHasReward(UINT32 id);
	UBOOL IsAchievementRewardObtain(UINT32 id);
	UINT32 GetAchievementPoint();
	UINT32 GetBaseClassAchievementCompleteCount(UINT32 baseClass);
	UINT32 GetBaseClassAchievementTotalCount(UINT32 baseClass);
	UINT32 GetSubClassAchievementCompleteCount(UINT32 baseClass, UINT32 subClass);
	UINT32 GetSubClassAchievementTotalCount(UINT32 baseClass, UINT32 subClass);
	UINT32 GetAchievementCompleteCount();
	void GetAchievementRecentComplete(INT (&id)[MAX_RECENT_COMPLETE_COUNT], INT (&date)[MAX_RECENT_COMPLETE_COUNT]);
	UINT32 GetAchievementLatestComplete();
	UINT32 GetAchievementPointCompare();
	UINT32 GetSubClassAchievementCompleteCountCompare(UINT32 baseClass, UINT32 subClass);
	UINT32 GetSubClassAchievementTotalCountCompare(UINT32 baseClass, UINT32 subClass);

	const AchievementIdSet& GetAchievementReward();

	void AddTraceAchievement(UINT32 id);
	void RemoveTraceAchievement(UINT32 id);
	void UpdateTraceAchievement(UINT32 level);

	const AchievementIdSet& GetPersonalAchievementBeTraced();
	const AchievementIdSet& GetGuildAchievementBeTraced();

protected:
	UBOOL _LoadAchievementData(const TCHAR* filePath);
	void _UnloadAchievementData();

	UBOOL _LoadKillNpcAchievementData(const TCHAR* filePath);
	void _UnloadKillNpcAchievementData();

	UBOOL _LoadGuildAchievementData(const TCHAR* filePath);
	void _UnloadGuildAchievementData();

	UBOOL _LoadGuildConditionData(const TCHAR* filePath);
	void _UnloadGuildConditionData();

	void _AddAchievementClass(UINT32 baseClass, UINT32 subClass, UINT32 id);

	void _InitCommonAchievementInfo();
	void _InitKillNpcAchievementInfo();
	void _InitGuildAchievementInfo();

	void _RefreshAchievementCompleteList();

protected:
	AchievementDataMap _AchievementDataMap;
	KillNpcAchievementDataMap _KillNpcAchievementDataMap;
	GuildAchievementDataMap _GuildAchievementDataMap;
	GuildConditionDataMap _GuildConditionDataMap;

	AchievementInfoMap _AchievementInfoMap;
	AchievementClassMap _AchievementClassMap;

	INT32 _AchievementVariable[MAX_ACHIEVEMENT_STATISTIC_COUNT];
	UINT32 _BaseClassCompleteCount[AC_TOTAL];
	UINT32 _BaseClassTotalCount[AC_TOTAL];

	AchievementIdList _AchievementCompleteList;		// completed achievements
	AchievementIdSet _AchievementRewardSet;			// achievements whose reward can be claimed

	AchievementIdSet _PersonalAchievementTraceSet;  // tracked personal achievements
	AchievementIdSet _GuildAchievementTraceSet;  // tracked guild achievements
};

#endif