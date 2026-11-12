#ifndef _ACTIVITY_MANAGER_H_
#define _ACTIVITY_MANAGER_H_

#include "MMOGameEvent.h"
#include "common.h"
#include "MMOUtil.h"
#include <time.h>
#include "TeamManager.h"
#include "MMODataManager.h"

//Activity type
enum
{
	EActivityHot	 = 0,	
	EActivityOutHome = 1,
	EActivityCopy    = 2,

	ERewardType_Money    = 1,
	ERewardType_Exp      = 2,
	ERewardType_Treasure = 4,

};

enum EActivityState
{
	ENOStart,
	EWillStart,
	EStarting,
	EActivityOver,
};

// Completion display type
enum EActivityCompleteType
{
	EACPT_None = 0,				// Cannot know whether participated
	EACPT_Quest,				// Quest complete, activity complete
	EACPT_Time,					// Time ends, activity complete
	EACPT_SignIn,				// Daily sign-in, activity complete
	EACPT_Variable,				// Character variable reaches the specified value, activity complete
};

// Activity system variables
enum EActivityRoleVariable
{
	EARV_ID_1 = 193,			// Character variable ID: guild prayer count
	EARV_ID_2 = 213,			// Character variable ID: guild stronghold war state
	EARV_ID_3 = 214,			// Character variable ID: Youming Sea entry count
	EARV_ID_4 = 216,			// Character variable ID: last entry time to Youming Sea
	EARV_ID_5 = 217,			// Character variable ID: last guild prayer time
	EARV_ID_6 = 218,			// Character variable ID: last entry time to Nine-Layer Demon Tower
	EARV_ID_7 = 219,			// Character variable ID: last entry time to Royal Treasury
	EARV_ID_8 = 220,			// Character variable ID: last entry time to Experience Arena
	EARV_ID_9 = 195,			// Character variable ID: Training Ground entry count
	EARV_ID_10 = 179,           // Character variable ID: last time the character did guild trading
	EARV_ID_11 = 187,           // Character variable ID: time the character soaked in the hot spring daily
	EARV_ID_12 = 226,			// Character variable ID: Training Ground daily count time
};

//Activity time
struct ActivityTime
{
	INT				nStartTime_year;
	INT				nStartTime_month;
	INT				nStartTime_day;
	INT				nStartTime_weekDay;
	INT				nStartTime_hour;
	INT				nStartTime_minute;
	INT				nEndTime_year;
	INT				nEndTime_month;
	INT				nEndTime_day;
	INT				nEndTime_weekDay;
	INT				nEndTime_hour;
	INT				nEndTime_minute;

	FString			sTimeStr;
	EActivityState  nState;
	time_t			startT;
	EActivityState	GetState();
	bool  CompareValue(const ActivityTime& rightVal)const
	{
		return rightVal.startT < startT;
	}
};

struct RewardBarItem
{
	INT nID;
	INT nVitality;			//Accumulated activity points
	INT nRewardItemID;
	INT nRewardItemCount;
	INT nBoxLevel;
};

//Activity reward
struct ActivityReward
{
	INT				nRewardType;			//1 is gold, 2 is experience, 4 is treasure hunt; gold and experience combined is 1+2, and so on: 1+4, 2+4, 1+2+4
	INT				nRewardVitality;		//Activity points reward
	INT				nArrItemID[10];
};

//Basic activity data
struct ActivityData
{
	INT						nVipLevel;				//Whether it is a VIP activity
	INT						nActivityID;			//Activity ID
	INT						nLinkQuestType;			//Linked quest type
	INT						nLinkQuestID;			//Linked quest ID
	INT						nCheckLevelMin;			//Level limit lower bound
	INT						nCheckLevelMax;			//Level limit upper bound
	INT						nHotStarNum;			//Recommended star count
	INT						nPersonNumMin;			//Participant count lower bound
	INT						nPersonNumMax;			//Participant count upper bound
	INT						nHaveDoneTimes;			//Number of times already completed
	INT						nPerDayJoinNum;			//Number of participations per day
	INT                     nCompleteType;			//Type of activity completion
	INT                     nCompleteMap;			//Map of activity completion
	TArray<UINT32>			aTimeLimit;				//Activity limits
	INT                     nGuildActivityType;		//Guild activity type
	INT                     nRoleVariable;			//Activity character variable

	INT						nActivityType;
	ActivityTime			nTimeInfo;
	ActivityReward			nRewardInfo;
		
	TArray<FString>			sNpcName;
	TArray<INT>				nAcceptNpcID;
	TArray<INT>				ndestMapID;

	FString					sLevelString;
	FString					sPersonString;
	FString					sActivityName;
	FString					sDecrible;

	ActivityData()
	{
		appMemzero(this, sizeof(ActivityData));
	}
};
//Read activity config table
struct ActivityDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("Activity.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static ActivityData* CreateDataObject(INT templateID, bool& needAddToMap);

	static void DestroyDataObject(ActivityData* obj);

	static void ParseData(ActivityData* obj, class CTabFile* dataRow, int rowIdx);


	static FString GetTemplateIDName()
	{
		return TEXT("activityID");
	}

	static FString FormatAsString(ActivityData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(ActivityData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};
typedef TDataManger< ActivityData, ActivityDataProcessor >   MMOActivityDataManager;

//Read activity points reward config
struct ActivityRewardBarDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("activity_reward.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static RewardBarItem* CreateDataObject(INT templateID, bool& needAddToMap);

	static void DestroyDataObject(RewardBarItem* obj);

	static void ParseData(RewardBarItem* obj, class CTabFile* dataRow, int rowIdx);


	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(RewardBarItem* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(RewardBarItem* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};
typedef TDataManger< RewardBarItem, ActivityRewardBarDataProcessor >   MMOActivityRewardBarDataManager;

//Basic dungeon list data
struct FBActivityData
{
	INT						nFBID;					//Activity ID
	INT						nFBType;
	INT						nCheckLevelMin;			//Level limit lower bound
	INT						nCheckLevelMax;			//Level limit upper bound
	INT						nPersonNumMin;			//Participant count lower bound
	INT						nPersonNumMax;			//Participant count upper bound
	INT						nHaveDoneTimes;			//Number of times already completed
	INT						nPerDayJoinNum;			//Number of participations per day
	INT						nAcceptNpcID;
	INT						ndestMapID;
	INT						nPosX;
	INT						nPosY;
	INT						nPosZ;
	INT						nFBCostActiveValue;		//Dungeon cost
	INT						nFBDifficulty;			//Dungeon difficulty
	ActivityTime			nTimeInfo;
	ActivityReward			nRewardInfo;
	FString					sDifficultyString;
//		FString					sPosName;
	FString					sLevelString;
	FString					sPersonString;
	FString					sFBName;
	FString					sDecrible;

	FBActivityData()
	{
		appMemzero(this, sizeof(FBActivityData));
	}
};

//Activity/dungeon target table
struct ActivityTarget
{
	FString sName;     //Name
	INT sType;         //Type
	INT sDifficulty;   //Difficulty
	INT sLevel;        //Level
	INT sBossCount;    //Boss count
	INT sConsume;      //Vitality consumed
	INT sAchieve;      //Activity points obtained
	INT sRecommend;    //Recommended player count
};

struct ActivityTeam
{
public:
	INT TeamId;
	INT LeaderId;
	FString Name;
	INT Level;
	INT PlayerCount;
	INT MaxPlayerCount;
	INT MemberId[MAX_TEAM_MEMBER_COUNT];

	ActivityTeam()
	{
		Clear();
	}

	~ActivityTeam()
	{
		Clear();
	}

	void Clear()
	{
			//memset(this, 0, sizeof(ActivityTeam));
		TeamId = 0;
		LeaderId = 0;
		Name = TEXT("");
		Level = 0;
		PlayerCount = 0;
		MaxPlayerCount = 0;
	}
};


struct ActivityTeamMember
{
	INT PlayerId;
	FString Name;
	INT Vitality;
	INT ActivityTimes;
	INT MaxActivityTimes;
	INT ClassId;
	INT Level;
};


// Activity config table
typedef std::list<ActivityData, M_UE3_STL_ALLOC(ActivityData) >		ActivityDataList;
typedef std::list<FBActivityData, M_UE3_STL_ALLOC(FBActivityData) > FBActivityDataList;
typedef std::list<RewardBarItem, M_UE3_STL_ALLOC(RewardBarItem) >	RewardBarDataList;

typedef std::list<ActivityTarget, M_UE3_STL_ALLOC(ActivityTarget) > ActivityTargetList;
typedef TArray<ActivityTeamMember> ActivityTeamMemberList;
typedef TArray<ActivityTeam> ActivityTeamList;
typedef std::map<INT, ActivityTeamMemberList, M_UE3_STL_ALLOC_MAP(INT, ActivityTeamMemberList) > TeamMemberListMap;
typedef std::map<INT, ActivityTeamList, M_UE3_STL_ALLOC_MAP(INT, ActivityTeamList) > ActivityTeamListMap;

typedef std::map<UINT16, INT32, M_UE3_STL_ALLOC_MAP(UINT16, INT32)> ActivityVariableMap;

// Activity event
struct MMOGameEvent_Activity : public MMOGameEvent
{
public:
	enum EActivityEventType
	{
		EActivity_UpdateState,
		EActivity_UpdateActivityTeamList,
		EActivity_ForceRequireActivityTeam,
	};

	MMOGameEvent_Activity(EActivityEventType eventType, INT activityId = -1)
		: EventType(eventType)
		, nActivityID(activityId)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Activity;
	}

	EActivityEventType  EventType;
	INT					nActivityID;
};

// Activity manager class
class ActivityManager : public SingletonMeyers<ActivityManager>, public INativeGameEventHandler
{
public:
	ActivityManager();
	virtual ~ActivityManager();

	enum EActivityHeadTypeSort
	{
		//Activity sorting
		EActivitySortHead_Name,
		EActivitySortHead_Time,
		EActivitySortHead_PersonNum,
		EActivitySortHead_Reward,
		EActivitySortHead_State,
		EActivitySortHead_CompleteStatus,
		EActivitySortHead_Star,

		//Dungeon sorting
		EFBSortHead_Level,
		EFBSortHead_CostActiveValue,

	};
	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	// Activity state update event
	virtual void				OnNativeGameEvent(struct MMOGameEvent* evt);
	// Activity data
	void						AddActivityData(ActivityData& data);
	void						AddActivityRewardBarData(RewardBarItem& data);
	const ActivityData*			GetActivityData(INT activityId) const;
	const FBActivityData*		GetFBActivityData(INT FBId) const;
	const ActivityDataList&		GetActivityDataList() const;
	const ActivityDataList&		GetCompleteActivityDataList() const;
	const FBActivityDataList&	GetFBActivityDataList() const;
	const RewardBarItem*		GetRewardBarCondition(INT nIndex)const;
	const ActivityTeam*			GetRandTeamCurActivity() const;
	UBOOL						IsActivity(INT questId);
	INT							GetActivityVipLevel(INT questId);

	// Attributes that need to be checked for the activity
	INT							 GetActivityAcceptMaxTimes(INT activityId);
	INT							 GetMaxActiveValue();
	void						 FilterTodayActivity();

	// Get current activity points
	INT32   GetActiveValue();
	INT 	GetRewardState(INT nIndex);
	INT		GetCurActivityId();
	INT		GetCurActivityTeamPage();
	INT		GetSelectedActivityTeam();
	INT		GetActivityFilterType();
	INT		GetActivityTipsTeamId();
	INT		GetActivityTeamLevel(INT nTeamId);
	ActivityTeam* GetActivityTeamById(INT TeamId);
	void	SetCurActivityId(INT nActivityId);
	void	SetCurActivityTeamPage(INT nPage);
	void	SetSelectedActivityTeam(INT nIndex);
	void	UpdateActivityTeamList(const char* buf);
	void	ClearActivityTeamList();

	void FilterActivityType(INT nActivityType,INT nRewardType);
	void SortActivityList(INT nType,INT nAdd);
	void SortTodayActivityList(INT nType,INT nAdd);
	void SetActivityList(UMMOGFXPlayer* gfxPlayer, UGFxObject* dataProvider);
	void SetActivityDetailInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nActivityID);
	void SetActivityTeamMemberInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nTeamID);
	void SetActivityTeamInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nActivityID, INT nPage);
	void SetActivityInviteList(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nType, UBOOL bActivity);
	void SetFBDetailInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nFBID);
	void SetQuitHintActivityList(UMMOGFXPlayer* gfxPlayer,UGFxObject* object);
	void SetRewardBarInfo(UMMOGFXPlayer* gfxPlayer,UGFxObject* object);

	void NotifyActivityVariableErvtActivity(INT32 nValue);
	void NotifyActivityVariableErvtActivityReward(INT32 nValue);
	void NotifyActivityVariableErvtQuestComNum(INT32 nValue);

	INT32 GetRewardBit() { return _RewardBit; }

protected:
	void _UnloadActivityData();
	void _LoadFBData();

	void SetActivityDataList(UMMOGFXPlayer* gfxPlayer, UGFxObject* dataProvider);
	void SetFBActivityDataList(UMMOGFXPlayer* gfxPlayer, UGFxObject* dataProvider);

	void ClearActivityDetailInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void ClearFBDetailInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object);
	void SortActivityDataList(INT nType,INT nAdd);
	void SortFBActivityDataList(INT nType,INT nAdd);
	static bool CompareAdd_StartTime(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareAdd_Reward(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareAdd_PersonNum(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareDec_StartTime(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareDec_Reward(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareDec_PersonNum(const ActivityData& leftData, const ActivityData& rightData);

	static bool CompareAdd_State(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareDec_State(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareAdd_CompleteState(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareDec_CompleteState(const ActivityData& leftData, const ActivityData& rightData);

	static bool CompareAdd_StarNum(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareDec_StarNum(const ActivityData& leftData, const ActivityData& rightData);

	static bool CompareAdd_Level(const ActivityData& leftData, const ActivityData& rightData);
	static bool CompareDec_Level(const ActivityData& leftData, const ActivityData& rightData);

	static bool FBCompareAdd_CostActiveValue(const FBActivityData& leftData, const FBActivityData& rightData);
	static bool FBCompareDec_CostActiveValue(const FBActivityData& leftData, const FBActivityData& rightData);
	
	static bool FBCompareAdd_Dificulty(const FBActivityData& leftData, const FBActivityData& rightData);
	static bool FBCompareDec_Dificulty(const FBActivityData& leftData, const FBActivityData& rightData);

	static bool FBCompareAdd_Level(const FBActivityData& leftData, const FBActivityData& rightData);
	static bool FBCompareDec_Level(const FBActivityData& leftData, const FBActivityData& rightData);

	static bool FBCompareAdd_PersonNum(const FBActivityData& leftData, const FBActivityData& rightData);
	static bool FBCompareDec_PersonNum(const FBActivityData& leftData, const FBActivityData& rightData);

	void _GetActivityStateString(EActivityState eState, FString& stateString);
	void _GetGuildActivityTimeString(UINT16 activityType, FString& timeString);
	void _GetGuildActivityStateString(UINT16 activityType, FString& stateString);
	void _ClearVariable();
	void _UpdateVariable(UINT8 varType, INT32 referId, UINT16 index, INT32 val);
	INT32 _GetVariableValue(UINT8 varType, UINT16 index);
	INT32 _GetVariableCompleteCount(UINT8 varType, UINT16 index);

protected:
	RewardBarDataList _RewardBarDataList;
	//whole activity datalist
	ActivityDataList	_ActivityDataList;
	FBActivityDataList  _FBDataList;
	//showing datalist
	ActivityDataList _ActivityCurShowDataList;
	//today quit hint activity
	ActivityDataList _QuitTodayActivityList;

	TeamMemberListMap  _TeamMemberListCache;
	//ActivityTeamListMap _ActivityTeamListCache;
	ActivityTeamList	_ActivityTeamListData;

	ActivityVariableMap _ActivityVariableMap;

	//Activity points
	INT32			 _ActiveValue;
	INT32			 _RewardQuestComNum;
	INT32			 _RewardBit;
	INT				 _CurShowType;
	INT				 _MaxActiveValue;

	INT				_CurActivityId;
	INT				_CurActivityTeamPage;
	INT				_CurActivityTeamIndex;
	INT				_CurActivityTipsTeamId;
};

#endif
