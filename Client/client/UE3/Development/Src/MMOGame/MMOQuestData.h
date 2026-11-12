#ifndef _MMOQUESTDATA_H_
#define _MMOQUESTDATA_H_

#include "MMOGameEvent.h"
#include "common.h"
#include "role_db_def.h"
#include "MMOUtil.h"
#include "MMODataManager.h"

// common
enum
{
	INVALID_QUEST_VAR_ID = -1,
	MAX_CIRCLE_AWARD_ITEM_COUNT = 4,
	MAX_STORY_QUEST_CHAPTER_NUM = 50,
	MAX_STORY_QUEST_STAGE_PER_CHAPTER_NUM = 50,
	REWARD_QUEST_NPC_ID = 6456,
	QUEST_GRID_QUERY_TIME = 2,
	MAX_QUEST_DOODAD_COUNT = 32,
	QUEST_CD_TIME = 2,
	QUEST_UPDATE_TIME = 1
};

enum EQestState
{
	EQS_Null = -1,
	EQS_Accept = 0,
	EQS_Available = 1,
	EQS_Completed = 2
};

// quest data
struct QuestDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("quest.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}
	static FString FormatAsString(UQuest* obj)
	{
		return "";
	}
	static FString GetTemplateIDName()
	{
		return TEXT("QuestTempID");
	}
	static UQuest* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(UQuest* obj);
	static void ParseData(UQuest* obj, class CTabFile* dataRow, int rowIdx);
	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(UQuest* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<UQuest, QuestDataProcessor> QuestDataManager;

typedef std::map<INT, class UQuest*, M_UE3_STL_ALLOC_MAP(INT, class UQuest*)> QuestDataMap;	
typedef std::list<class UQuest*, M_UE3_STL_ALLOC(class UQuest*)> QuestDataList;					

// quest Npc
struct QuestNpcInfo
{
	QuestDataMap StartQuestList;
	QuestDataMap EndQuestList;
};

// accepted quests
class AcceptedQuestData
{
public:
	UINT32 Id;
	UINT32 AcceptTime;
	UINT32 LeftTime;
	UINT8 KillNpcCount[MAX_CONDITION_PER_QUEST];
	UINT8 KillDoodadCount[MAX_CONDITION_PER_QUEST];
	UINT8 KillPlayerCount[CHECK_KILL_PLAYER_CONDITION];
	INT32 RoleVariable[MAX_ROLE_VARIABLE_COUNT];
	INT32 QuestVariable[MAX_VARIABLE_PER_QUEST];
	INT32 ClientVariable[MAX_CLIENT_VARIABLE_COUNT];
	UINT8 Level;

	UBOOL IsTraced;
	UBOOL IsExpanded;
	UBOOL IsNewQuest;
	UBOOL IsFailed;

	AcceptedQuestData();
	AcceptedQuestData(const ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST& src);

	AcceptedQuestData& operator =(const ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST& src);

	void _Init();
	void _Assign(const ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST &src);
};

// quest rewards
struct QuestAwardItemInfo
{
	UItem* ItemTemplate;
	INT Count;
	INT Index; // index in the original config
};

typedef std::map<INT, QuestNpcInfo, M_UE3_STL_ALLOC_MAP(INT, QuestNpcInfo)> QuestNpcInfoMap;					// quest NPC
typedef std::map<INT, AcceptedQuestData, M_UE3_STL_ALLOC_MAP(INT, AcceptedQuestData)> AcceptedQuestDataMap;		// accepted quests
typedef std::map<INT, class UQuest*, M_UE3_STL_ALLOC_MAP(INT, class UQuest*)> AvailableQuestDataMap;			// acceptable quests

typedef std::list<INT, M_UE3_STL_ALLOC(INT) > QuestTraceList;													// quest tracking

typedef std::vector<QuestAwardItemInfo, M_UE3_STL_ALLOC(QuestAwardItemInfo)> QuestAwardItemInfoList;			// quest rewards

// circle quest data
struct CircleQuestData
{
	INT CircleId;									// circle number
	INT	CircleGroup;								// circle group
	INT	CircleRoundMax;								// max rounds of circle quests per day
	INT	CircleSize;									// number of quests in the current circle quest package
	INT ResetTime;									// reset time
	INT Level;										// player level
	INT	CircleRound;								// round number of the circle quest
	INT RewardId[MAX_QUEST_PER_CIRCLE];				// reward group index
};

struct CircleQuestDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("circle_quest.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}
	static FString FormatAsString(CircleQuestData* obj)
	{
		return "";
	}
	static FString GetTemplateIDName()
	{
		return TEXT("CircleID");
	}
	static CircleQuestData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(CircleQuestData* obj);
	static void ParseData(CircleQuestData* obj, class CTabFile* dataRow, int rowIdx);
	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(CircleQuestData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<CircleQuestData, CircleQuestDataProcessor> CircleQuestDataManager;

// a circle quest's reward is determined by three factors: circle number, player level, and the sequence number of the circle quest the player has completed
// circle quest reward data
struct CircleQuestRewardData
{
	INT RewardId;
	INT MoneyType;
	INT Money;
	INT Exp;
	INT BuffId;
	INT BuffLevel;
	INT Soul;
	INT Title;
	INT ReputationForceId;
	INT Reputation;
	INT AwardItemId[MAX_CIRCLE_AWARD_ITEM_COUNT];
	INT AwardItemCount[MAX_CIRCLE_AWARD_ITEM_COUNT];
	UBOOL NeedSelectAwardItem;
	INT SelectableAwardItemId[MAX_SELECTABLE_REWARD_COUNT];
	INT SelectableAwardItemCount[MAX_SELECTABLE_REWARD_COUNT];
	INT GuildContribute;
	INT MoneyTypeEx;
	INT MoneyEx;
};

struct CircleQuestRewardDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("quest_reward.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}
	static FString FormatAsString(CircleQuestRewardData* obj)
	{
		return "";
	}
	static FString GetTemplateIDName()
	{
		return TEXT("GroupID");
	}
	static CircleQuestRewardData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(CircleQuestRewardData* obj);
	static void ParseData(CircleQuestRewardData* obj, class CTabFile* dataRow, int rowIdx);
	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(CircleQuestRewardData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<CircleQuestRewardData, CircleQuestRewardDataProcessor> CircleQuestRewardDataManager;

struct CircleQuestInfo
{
	INT CircleId;
	INT CircleIndex;
	INT CircleRound;
	INT QuestId;
	INT CompleteQuestTime;
		
	CircleQuestInfo () 
		: CircleId(0)
		, CircleIndex(0)
		, CircleRound(0)
		, QuestId(INVALID_QUEST_ID)
		, CompleteQuestTime(0)
	{}
};

typedef std::map<INT, CircleQuestData, M_UE3_STL_ALLOC_MAP(INT, CircleQuestData)> CircleQuestDataMap;						// circle quest data
typedef std::map<INT, CircleQuestRewardData, M_UE3_STL_ALLOC_MAP(INT, CircleQuestRewardData)> CircleQuestRewardDataMap;		// circle quest reward data
typedef std::map<INT, CircleQuestInfo, M_UE3_STL_ALLOC_MAP(INT, CircleQuestInfo)> CircleQuestInfoMap;						// circle quest completion info

// bounty quest
struct RewardQuestInfo
{
	UINT32 QuestId;
	EQestState QuestState;

	RewardQuestInfo () 
		: QuestId(INVALID_QUEST_ID)
		, QuestState(EQS_Null)
	{}
};

typedef std::list<RewardQuestInfo, M_UE3_STL_ALLOC(RewardQuestInfo) > RewardQuestList;										// bounty quest list

// bounty quest cost
struct RewardQuestCostData
{
	INT Time;
	INT Cost;

	RewardQuestCostData() 
		: Time(0)
		, Cost(0)
	{}
};

struct RewardQuestCostDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("reward_quest_money_cost.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}
	static FString FormatAsString(RewardQuestCostData* obj)
	{
		return "";
	}
	static FString GetTemplateIDName()
	{
		return TEXT("Time");
	}
	static RewardQuestCostData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(RewardQuestCostData* obj);
	static void ParseData(RewardQuestCostData* obj, class CTabFile* dataRow, int rowIdx);
	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(RewardQuestCostData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<RewardQuestCostData, RewardQuestCostDataProcessor> RewardQuestCostDataManager;
typedef std::map<INT, RewardQuestCostData, M_UE3_STL_ALLOC_MAP(INT, RewardQuestCostData)> RewardQuestCostDataMap;

// story quest
struct StoryQuestData
{
	UINT32 Chapter;
	UINT32 Stage;
	FString ChapterName;
	FString ChapterDesc;
	FString ChapterIcon;
	FString StageName;
	FString StageDesc;

	StoryQuestData () 
		: Chapter(0)
		, Stage(0)
		, ChapterName(TEXT(""))
		, ChapterDesc(TEXT(""))
		, ChapterIcon(TEXT(""))
		, StageName(TEXT(""))
		, StageDesc(TEXT(""))
	{}
};

struct StoryQuestDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("quest_story.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}
	static FString FormatAsString(StoryQuestData* obj)
	{
		return "";
	}
	static FString GetTemplateIDName()
	{
		return TEXT("Chapter");
	}
	static StoryQuestData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(StoryQuestData* obj);
	static void ParseData(StoryQuestData* obj, class CTabFile* dataRow, int rowIdx);
	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(StoryQuestData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<StoryQuestData, StoryQuestDataProcessor> StoryQuestDataManager;

#define MAKE_STORY_QUEST_KEY(chapter, stage) ((chapter << 16) | stage)
typedef std::map<UINT32, StoryQuestData, M_UE3_STL_ALLOC_MAP(UINT32, StoryQuestData)> StoryQuestDataMap;

typedef std::list<UINT32, M_UE3_STL_ALLOC(UINT32) > StoryQuestChapterList;
typedef std::list<UINT32, M_UE3_STL_ALLOC(UINT32) > StoryQuestStageList;
typedef std::map<UINT32, StoryQuestStageList, M_UE3_STL_ALLOC_MAP(UINT32, StoryQuestStageList)> StoryQuestChapterStageMap;

// quest map
typedef std::map<UINT32, QuestDataList, M_UE3_STL_ALLOC_MAP(UINT32, QuestDataList)> QuestMapDataMap;

// recovery quest
struct QuestRecycleData
{
	INT ItemLevel;
	INT ItemColor;
	FString ItemSubType;
	INT VarIndex;
};

struct QuestRecycleDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("quest_recycle.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}
	static FString FormatAsString(QuestRecycleData* obj)
	{
		return "";
	}
	static FString GetTemplateIDName()
	{
		return TEXT("ItemLevel");
	}
	static QuestRecycleData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(QuestRecycleData* obj);
	static void ParseData(QuestRecycleData* obj, class CTabFile* dataRow, int rowIdx);
	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(QuestRecycleData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<QuestRecycleData, QuestRecycleDataProcessor> QuestRecycleDataManager;
typedef std::vector<QuestRecycleData, M_UE3_STL_ALLOC(QuestRecycleData)> QuestRecycleDataList;

// kill-monster quest:
// monster -> accepted quest mapping
typedef std::map<UINT32, QuestTraceList, M_UE3_STL_ALLOC_MAP(UINT32, QuestTraceList)> MonsterAcceptedQuestsMap;

// emergency quest info: used to record the CD time of emergency quests and determine whether the current emergency quest can be accepted
struct EmergencyQuestCoolDownInfo
{
	INT QuestId;
	INT CoolDown;
};

typedef std::map<INT, EmergencyQuestCoolDownInfo, M_UE3_STL_ALLOC_MAP(INT, EmergencyQuestCoolDownInfo)> EmergencyQuestCoolDownMap;

// quest drops
typedef std::set<INT> DoodadDropSet;
struct QuestDoodadData
{
	INT QuestId;
	DoodadDropSet DoodadDrops;

	QuestDoodadData() 
	{
		QuestId = INVALID_QUEST_ID;
		DoodadDrops.clear();
	}
};

struct QuestDoodadDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("quest_doodad.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}
	static FString FormatAsString(QuestDoodadData* obj)
	{
		return "";
	}
	static FString GetTemplateIDName()
	{
		return TEXT("QuestId");
	}
	static QuestDoodadData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(QuestDoodadData* obj);
	static void ParseData(QuestDoodadData* obj, class CTabFile* dataRow, int rowIdx);
	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(QuestDoodadData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<QuestDoodadData, QuestDoodadDataProcessor> QuestDoodadDataManager;
typedef std::map<INT, QuestDoodadData, M_UE3_STL_ALLOC_MAP(INT, QuestDoodadData)> QuestDoodadDataMap;

// guide quest bitwise enum
enum GuideQuestType
{
	GQT_Invalid = 0,
	GQT_Friend,						// add a friend, complete quest
	GQT_Mail,						// send a mail, complete quest
	GQT_Master,						// take a master, complete quest
	GQT_Soul,						// convert soul to training points, complete quest
	GQT_UpgradeSkill,				// upgrade skill, complete quest
	GQT_Posy,						// rune inlay, complete quest
	GQT_Guild,						// create a guild, complete quest
	GQT_Team,						// form a team, complete quest
	GQT_EmbedEquip,					// spirit stone inlay, complete quest
	GQT_PickEquip,					// spirit stone removal, complete quest
	GQT_Strong,						// physique enhancement, complete quest
	GQT_Heaven,						// equipment Qihai, complete quest
	GQT_Appraisal,					// appraise equipment, complete quest
	GQT_Battlefield,				// enter the arena, complete quest
	GQT_Extendbag,					// expand bag, complete quest
	GQT_Mount,						// equip mount, complete quest
	GQT_Chat,						// send chat, complete quest
	GQT_UpgradeEquip,				// upgrade equipment, complete quest
	GQT_UseSkill,					// use ability, complete quest
	GQT_LearnSkill,					// learn ability, complete quest
	GQT_GetItem,					// obtain item, complete quest
	GQT_EquipItem,					// equip item, complete quest
	GQT_Compose,					// compose item, complete quest
	GQT_Decompose,					// decompose item, complete quest

	GQT_EquipRemake,				// remake equipment, complete quest
	GQT_BookRemake,					// remake skill book, complete quest
	GQT_Booster,					// mercenary, complete quest
	GQT_Exercise,					// training, complete quest
};

// build monster group -> monster table mapping
typedef std::list<INT, M_UE3_STL_ALLOC(INT) > MonsterList;
typedef std::map<INT, MonsterList, M_UE3_STL_ALLOC_MAP(INT, MonsterList)> MonsterGroupMap;

class MMOQuestData : public SingletonMeyers<MMOQuestData>
{
public:
	MMOQuestData();
	virtual ~MMOQuestData();

	void Init();
	void Shutdown();
	void Clear();

	// quest data
	void LoadQuestDataFromUpk();
	void AddQuestData(UQuest* data);
	const class UQuest* GetQuestData(INT questId) const;
	const QuestDataMap& GetQuestDataMap() const;
	void UpdateQuestList(ROLE_DB_QUEST_DATA* questData);
	EQestState GetQuestState(INT questId);

	// quest operations
	void AcceptQuest(INT questId);
	void AbandonQuest(INT questId);
	void CompleteQuest(INT questId);

	// quest NPC
	EQuestHUDStatus IsNpcHasMyQuest(INT npcId, TArray<INT>* completeList,
		TArray<INT>* uncompleteList, 
		TArray<INT>* availableList);

	// available quests
	TArray<UQuest*> GetAvailableQuestsByType(EQUEST_TYPE type);
	INT GetAvailableQuestCount();
	bool IsQuestAvailable(INT questId);

	// accepted quests
	const AcceptedQuestDataMap& GetAcceptedQuests() const;
	TArray<UQuest*> GetAcceptedQuestsByType(EQUEST_TYPE type = E_qt_MAX);
	AcceptedQuestDataMap& GetAcceptedQuests();
	UBOOL IsQuestAccepted(INT questId) const;
	const AcceptedQuestData* GetAcceptedQuestData(INT questId) const;
	AcceptedQuestData* GetAcceptedQuestData(INT questId);

	// complete quest
	TArray<UQuest*> GetCompassableQuestsByType(EQUEST_TYPE type);
	TArray<UQuest*> GetCompletedQuestsByType(EQUEST_TYPE type);
	UBOOL IsQuestCompassable(INT questId) const;
	UBOOL IsQuestCompleted(INT questId) const;
	UINT32 GetCompletedQuestCount(INT questId) const;
	UINT32 GetCompletedQuestMaxCount(INT questId) const;
	INT GetCompletedQuestCount() const;
	UINT32 GetQuestCompletedDate(INT questId, 
		INT32& Year, 
		INT32& Month, 
		INT32& Day, 
		INT32& DayOfWeek, 
		INT32& Hour, 
		INT32& Min, 
		INT32& seconds) const;

	// quest completion conditions
	void UpdateQuestReachLevel(INT curLevel);
	void UpdateQuestNpcKillCount(INT questId, UINT16 index, INT val);
	void UpdateQuestDoodadKillCount(INT questId, UINT16 index, INT val);
	void UpdateQuestPlayerKillCount(INT questId, UINT16 index, INT val);
	void UpdateQuestVariable(INT questId, UINT16 index, INT val);
	void UpdateAllClientVariable(INT32 clientVariable[MAX_CLIENT_VARIABLE_COUNT]);
	void UpdateClientVariable(UINT16 index, INT val, INT classVal = 0);
	void UpdateRoleVariable(UINT16 index, INT val);
	void UpdateQuestTime(INT timeCount);

	// quest tracking
	const QuestTraceList& GetAcceptedQuestTraceList();
	const QuestTraceList& GetAvailableQuestTraceList();
	UBOOL HasConditions(INT questId);
	void TraceAllQuest(bool traceOn);
	void TraceQuest(INT questId, bool traceOn);

	// quest rewards
	void GetAwardItemList(INT questId, QuestAwardItemInfoList& list);
	void GetSelectableAwardItemList(INT questId, QuestAwardItemInfoList& list);
	INT GetSelectableAwardItemCount(INT questId);
	INT GetDefaultSelectableAwardItemIndex(INT questId);

	// ring quest related
	UBOOL GetCircleQuestData(INT circleId, INT playerLevel, CircleQuestData& data);
	UBOOL GetCircleQuestRewardData(INT circleId, INT circleIndex, INT playerLevel, 
		CircleQuestRewardData& reward);
	UBOOL GetCircleQuestInfo(INT circleId, CircleQuestInfo& info) const;
	void SetNextCircleQuest(INT questId, INT circleId);
	UBOOL HasCircleQuest();
	void AddCircleQuestData(const CircleQuestData& data);
	void AddCircleQuestRewardData(const CircleQuestRewardData& data);

	// bounty quest
	const RewardQuestList& GetRewardQuestList() const;
	void SetRewardQuestCostCount(INT count);
	INT GetRewardQuestCostCount() { return _RewardQuestCostCount; }
	INT GetRewardQuestAvailableLimit() {return _RewardQuestFreeCount + _RewardQuestCostCount;}
	void UpdateRewardQuestList(const INT32 (&rewardQuestId)[MAX_REWARD_QUEST_COUNT_PER_ROLE]);
	INT GetAcceptedRewardQuestCount();
	INT GetTopLevelRewardQuestCount();
	INT GetRewardQuestCost(INT time);
	void AddRewardQuestCostData(const RewardQuestCostData& data);

	// story quest
	UBOOL GetStoryQuestData(UINT32 chapter, UINT32 stage, StoryQuestData& data);
	const StoryQuestChapterStageMap& GetStoryQuestChapterStageMap();
	const QuestMapDataMap& GetQuestMapDataMap();
	const StoryQuestChapterList& GetStoryQuestChapterList();
	void AddStoryQuestData(const StoryQuestData& data);
	UBOOL IsStoryQuestPast(INT chapter, INT stage);

	// item recycling
	void AddQuestRecycle(const QuestRecycleData& questRecycle);
	const QuestRecycleDataList& GetQuestRecycleList() { return _QuestRecycleDataList; }
	UBOOL IsQuestRecycle(INT questId);

	// emergency quest
	void AddEmergencyQuestCoolDown(INT questId, INT coolDown);
	void UpdateEmergencyQuestCoolDown(INT deltaTime);
	INT GetEmergencyQuestCoolDown(INT questId) const;
	INT GetEmergencyQuestCompleteCount() { return _EmergencyQuestCompleteCount; }
	UBOOL IsEmegencyQuestAllCompleteToday();
	void AbandonAllEmergencyQuest();

	// quest drops
#if !SHIPPING_PC_GAME
	void GenerateQuestDoodadDropData(const TCHAR* filePath);
#endif
	void AddQuestDoodadData(const QuestDoodadData& data);
	const QuestDoodadDataMap& GetQuestDoodadDataMap() { return _QuestDoodadDataMap; }
	UBOOL IsAcceptedQuestDoodad(INT doodadDropId);

	// monster-kill quest
	UBOOL GetNormalQuestListByMonsterBeTraced(INT monsterId, QuestTraceList& questList);
	UBOOL GetEmergencyQuestListByMonsterBeTraced(INT monsterId, QuestTraceList& questList);
	UBOOL IsSingleMonsterQuest(INT questId);
	void ClearMonsterGroupData();
	void AddMonsterGroupData(INT groupId, INT monsterId);
	UBOOL GetGroupMonsterList(INT groupId, MonsterList& list);

	// level-triggered quest
	const QuestDataList& GetLevelTriggerQuestList() const;

	// set quest failed
	void SetQuestFailed(INT questId);

	// 
	QuestTraceList& GetReAcceptQuestList() { return _ReAcceptQuestList; }

protected:
	UBOOL _CheckQuestData();
	UBOOL _LoadQuestData(const TCHAR* filePath);
	void _UnloadQuestData();

	UBOOL _LoadCircleQuestData(const TCHAR* filePath);
	void _UnloadCircleQuestData();

	UBOOL _LoadCircleQuestRewardData(const TCHAR* filePath);
	void _UnloadCircleQuestRewardData();

	UBOOL _LoadStoryQuestData(const TCHAR* filePath);
	void _UnloadStoryQuestData();

	UBOOL _LoadQuestRecyle(const TCHAR* filePath);
	void _UnloadQuestRecyle();

	UBOOL _LoadRewardQuestCostData(const TCHAR* filePath);
	void _UnloadRewardQuestCostData();

	UBOOL _LoadQuestDoodadData(const TCHAR* filePath);
	void _UnloadQuestDoodadData();

	QuestNpcInfo* _GetOrAddQuestNpcInfo(INT npcId);
	void _InitQuestUtilData();

	void _AddAcceptedQuest(const ROLE_DB_QUEST_DATA::ROLE_DB_ACCEPTED_QUEST& questData, bool isNewQuest);
	void _RemoveAcceptedQuest(INT questId);
	
	// check available-quest restrictions
	UBOOL _CheckAvailableQuestLimit_Job(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Sex(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Master(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Disciple(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Husband(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Wife(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_GuildLeader(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_GuildMember(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_PreQuest(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Level(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Reputation(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Money(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Date(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Time(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Item(const UQuest* quest, AMMOPlayer* mainPlayer) const;
	UBOOL _CheckAvailableQuestLimit_Team(const UQuest* quest, AMMOPlayer* mainPlayer) const;

	// check quest completion conditions
	UBOOL _CheckCompleteQuestCondition_Time(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_KillMonster(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_Item(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_TalkNPC(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_Explor(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_Level(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_Variable(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_Reputation(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_Buff(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_Doodad(const UQuest* quest, const AcceptedQuestData* data) const;
	UBOOL _CheckCompleteQuestCondition_KillPlayer(const UQuest* quest, const AcceptedQuestData* data) const;

	// Item reward
	bool _IsItemForMe(EPlayerClass job, EGenderType gender, const UItem* item);

	// story quest
	void _AddStoryQuestChapterStage(UINT32 chapter, UINT32 stage);
	void _AddStoryQuestPastChapter(UINT32 chapter);

	// monster-kill quest
	void _AddMonsterAcceptedQuest(INT monsterId, INT questId);
	void _RemoveMonsterAcceptedQuest(INT monsterId, INT questId);

	void _UpdateAcceptedQuestVariable();

protected:
	QuestDataMap _QuestDataMap;

	UINT8 _NormalQuest[MAX_NORMAL_QUEST_COUNT / 8];
	UINT8 _RepeatQuest[MAX_REPEAT_QUEST_COUNT];
	UINT32 _RepeatQuestScanTime;

	AcceptedQuestDataMap _AcceptedQuestDataMap;

	QuestTraceList _AcceptedQuestTraceList;
	QuestTraceList _AvailableQuestTraceList;

	QuestNpcInfoMap _QuestNpcInfoMap;

	CircleQuestDataMap _CircleQuestDataMap;
	CircleQuestRewardDataMap _CircleQuestRewardDataMap;
	CircleQuestInfoMap _CircleQuestInfoMap;

	// bounty quest
	RewardQuestList _RewardQuestList;
	INT _RewardQuestFreeCount;
	INT _RewardQuestCostCount;
	RewardQuestCostDataMap _RewardQuestCostDataMap;

	// story quest
	StoryQuestDataMap _StoryQuestDataMap;
	StoryQuestChapterStageMap _StoryQuestChapterStageMap;
	StoryQuestChapterList _StoryQuestPastChapterList;

	// quest map
	QuestMapDataMap _QuestMapDataMap;

	// quest recycling
	QuestRecycleDataList _QuestRecycleDataList;

	// emergency quest
	EmergencyQuestCoolDownMap _EmergencyQuestCoolDownMap;				// accepted emergency quest info

	// monster-kill quest
	MonsterAcceptedQuestsMap _MonsterAcceptedNormalMap;
	MonsterAcceptedQuestsMap _MonsterAcceptedEmergencyMap;

	// quest drops
	QuestDoodadDataMap _QuestDoodadDataMap;

	// quest triggered on reaching level
	QuestDataList _LevelTriggerQuestList;

	// emergency quest
	INT _EmergencyQuestCompleteCount;

	MonsterGroupMap _MonsterGroupMap;

	// main quest that needs to be re-accepted
	QuestTraceList _ReAcceptQuestList;
};

#endif
