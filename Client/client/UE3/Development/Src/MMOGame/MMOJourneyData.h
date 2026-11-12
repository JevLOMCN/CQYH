#ifndef _MMOJOURNEYDATA_H_
#define _MMOJOURNEYDATA_H_

#include "MMOGameEvent.h"
#include "common.h"
#include "role_db_def.h"
#include "MMOUtil.h"
#include "MMODataManager.h"
#include "MMOQuestData.h"
		
enum
{
	MAX_JOURNEY_TALK_COUNT = 5,
	MAX_JOURNEY_MONSTER_COUNT = 12,
};

enum JourneyType
{
	JT_STORY = 0,			 // story
	JT_MONSTER,				 // monster
	JT_ACHIEVEMENT,			 // achievement
	JT_EMERGENCY,            // event
	JT_BOSS,				 // BOSS
	JT_ACTIVITY,             // activity
	JT_GUIDE,                // guide
	JT_GUILD,                // guild
	JT_TOTAL,
};

enum JourneyMonsterType
{
	JMT_NORMAL = 0,			 // normal monster
	JMT_ELITE,				 // elite monster
	JMT_BOSS,				 // BOSS
};

struct MonsterMapData
{
	UINT32 MonsterId;
	FString MonsterName;
	UINT32 MonsterType;
	UINT32 MonsterMapId;
	FString MonsterMapRegion;
	FVector2D MonsterPoint;
};

// Path of the Strong data
struct JourneyData
{
	INT32 Id;
	FString Title;
	FString NpcName;
	FString Desc;
	FString Icon;
	TArray<FString> TalkList;
	TArray<FString> SoundList;
	INT32 MinLevel;
	INT32 MaxLevel;
	INT32 ForceId;
	UINT32 StoryQuestStar;
	UINT32 MonsterStar;
	UINT32 EmegencyQuestStar;
	UINT32 BossStar;
	UINT32 ActivityQuestStar;
	UINT32 GuideQuestStar;
	UINT32 AchievementStar;
	UINT32 GuildStar;
	TArray<UINT32> AwardList;
	TArray<MonsterMapData> MonsterList;
	TArray<MonsterMapData> BossList;
	UINT32 AchievementBaseClass;
	UINT32 AchievementSubClass;
};

struct JourneyDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("journey.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}
	static FString FormatAsString(JourneyData* obj)
	{
		return "";
	}
	static FString GetTemplateIDName()
	{
		return TEXT("JourneyId");
	}
	static JourneyData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(JourneyData* obj);
	static void ParseData(JourneyData* obj, class CTabFile* dataRow, int rowIdx);
	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(JourneyData* obj, TSet<FString>& outResList);
#endif
};

typedef TDataManger<JourneyData, JourneyDataProcessor> JourneyDataManager;

typedef std::map<INT32, JourneyData, M_UE3_STL_ALLOC_MAP(UINT32, JourneyData)> JourneyDataMap;
typedef std::map<JourneyType, UBOOL, M_UE3_STL_ALLOC_MAP(JourneyType, UBOOL)> JourneyTypeActivateMap;

class MMOJourneyData : public SingletonMeyers<MMOJourneyData>
{
public:
	MMOJourneyData();
	virtual ~MMOJourneyData();

	void Init();
	void Shutdown();
	void Clear();

	UBOOL LoadJourneyData(const TCHAR* filePath);
	void UnloadJourneyData();

	void AddJourneyData(const JourneyData& data);
	UBOOL GetJourneyData(INT32 journeyId, JourneyData& data);
	UBOOL GetJourneyDataByReputation(INT32 forceId, JourneyData& data);
	const JourneyDataMap& GetJourneyDataMap() {return _JourneyDataMap;}
	INT32 GetJourneyIdByLevel(INT level);
	INT32 GetMaxJourneyId();
	INT32 GetMaxActivateJourneyId();
	INT32 GetFirstRewardJourneyId();

	void ActivateJourneyType(JourneyType journeyType);
	void DeactiveJourneyType(JourneyType journeyType);
	UBOOL IsJourneyTypeActivate(JourneyType journeyType);
	void ClearJourneyTypeActivate();

	INT32 GetReputationAwardsCount(INT32 forceId);

protected:
	JourneyDataMap _JourneyDataMap;
	JourneyTypeActivateMap _JourneyTypeActivateMap;
};

#endif
