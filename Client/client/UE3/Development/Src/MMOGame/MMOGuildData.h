#ifndef _MMOGUILDDATA_H_
#define _MMOGUILDDATA_H_

#include "MMOSingleton.h"
#include <map>
#include <vector>
#include "UnString.h"
#include "guild_def.h"
#include "MMOFacade.h"
#include "MMODataManager.h"
#include "m2c_message.h"
#include "g2c_message.h"
#include "ActivityManager.h"

enum
{
	GUILD_MAX_STORAGE_RIGHT_COUNT = 12,
	GUILD_SALARY_PERCENT = 70,
	GUILD_TERRITORY_MAP_ID = 127,
	GUILD_WELFARE_GROUP_PER_PAGE = 4,
	GUILD_WELFARE_COUNT_PER_GROUP = 5,
	MAX_QUERY_SEND_COUNT = 15,
	GUILD_MAX_BUILDING_TYPE_COUNT = 22,
	GUILD_MAX_MATCH_WAR_COUNT = 5,
	SABAC_MAP_ID = 152,
	SABAC_SCENE_VARIABLE_ID_START = 8,
	GUILD_ACTIVITY_VARIABLE_107 = 107,
	GUILD_ACTIVITY_VARIABLE_108 = 108,
	GUILD_ACTIVITY_VARIABLE_109 = 109,
	GUILD_ACTIVITY_VARIABLE_110 = 110,
    GUILD_INVALID_ID_TIME = 7200,

	GUILD_OCCUPY_MAP_ID_1 = 209,			// Death Valley Insect Nest
	GUILD_OCCUPY_MAP_ID_2 = 161,			// Stone Tomb Level 4
	GUILD_OCCUPY_MAP_ID_3 = 155,			// Woma Temple Level 2
	GUILD_OCCUPY_MAP_ID_4 = 169,			// Zuma Temple Level 4
	GUILD_OCCUPY_VARIABLE_STATE = 1,		// guild stronghold war state
	GUILD_OCCUPY_VARIABLE_WINNER = 2,		// occupying guild ID
};

// guild member sorting
enum FullMemberSortType
{
	FMST_LEVEL = 0,
	FMST_JOB,
	FMST_POST,
	FMST_TIME
};

// guild basic info
struct GuildBaseInfo
{
	UINT32  Id;
	FString Name;
	UINT8 level;
	UINT8 MemberCount;
	UINT32 Money;
	UINT32 Salary;
	FString LeaderName;
	FString CreatorName;
	UINT32 CreateTime;
	FString Announce;
	FString Notice;
	FString Events;
	FString MoneyEvents;
	UINT32 Prosperity;

	UINT32 Herb;				// herb reserve
	UINT32 Wood;				// nanmu wood reserve
	UINT32 Crystal;				// magic crystal reserve
	UINT32 Silver;				// mithril reserve

	GuildBaseInfo()
	{
		Id = 0;
		Name = TEXT("");
		level = 0;
		MemberCount = 0;
		Money = 0;
		Salary = 0;
		LeaderName = TEXT("");
		CreatorName = TEXT("");
		CreateTime = 0;
		Announce = TEXT("");
		Notice = TEXT("");
		Events = TEXT("");
		MoneyEvents = TEXT("");
		Prosperity = 0;

		Herb = 0;
		Wood = 0;
		Crystal = 0;
		Silver = 0;
	}
};

// guild view info
struct GuildViewInfo
{
	UINT32  Id;
	FString Name;
	UINT8 level;
	UINT8 MemberCount;
	UINT32 LeaderId;
	FString LeaderName;
	FString CreatorName;
	UINT32 CreateTime;
	FString Announce;
	UINT32 Prosperity;
	UINT8 BuildingLevel[GUILD_B_NUM];
	UINT32 ProtectionTime;

	GuildViewInfo()
	{
		Id = 0;
		Name = TEXT("");
		level = 0;
		LeaderId = 0;
		MemberCount = 0;
		LeaderName = TEXT("");
		CreatorName = TEXT("");
		CreateTime = 0;
		Announce = TEXT("");
		Prosperity = 0;
		ProtectionTime = 0;
	}
};
typedef std::list<GuildViewInfo, M_UE3_STL_ALLOC(GuildViewInfo)> GuildViewList;

// guild query info
struct GuildQueryInfo
{
	UINT32 Id;
	FString Name;
	UINT32 LeaderId;
	UINT8 MemberCount;
	UINT8 Level;
	UINT8 BuildingLevel;
	UINT32 CreateTime;
	unsigned SabacState;
	UINT32 Crc;

	GuildQueryInfo()
	{
		Id = 0;
		Name = TEXT("");
		LeaderId = 0;
		MemberCount = 0;
		Level = 0;
		BuildingLevel = 0;
		CreateTime = 0;
		SabacState = 0;
		Crc = 0;
	}
};
typedef std::map<UINT32, GuildQueryInfo, M_UE3_STL_ALLOC_MAP(UINT32, GuildQueryInfo)> QueryGuildInfoByIdMap;
typedef std::map<FString, struct GuildQueryInfo, ComparFString, STLAllocator<std::pair<FString, struct GuildQueryInfo>, UE3STLAllocatorPolicy>> QueryGuildInfoByNameMap;

// guild official member info
struct GuildFullMemberInfo
{
	UINT32 Id;
	FString Name;
	UINT8 Post;
	UINT8 Level;
	UINT8 Job;
	INT SceneID;
	INT LastLoginTime;
	UBOOL IsForbitChat;
	UINT32 Salary;
};

typedef std::list<GuildFullMemberInfo, M_UE3_STL_ALLOC(GuildFullMemberInfo) > GuildFullMemberInfoList;

// guild pending member info
struct GuildPendingMemberInfo
{
	UINT32 Id;
	FString Name;
	UINT8 Level;
	UINT8 Job;
	UINT32 ApplyTime;
};

typedef std::list<GuildPendingMemberInfo, M_UE3_STL_ALLOC(GuildPendingMemberInfo) > GuildPendingMemberInfoList;

// guild relation
enum GuildRelation
{
	GR_NORMAL = 0,
	GR_FRIEND,
	GR_ENEMY
};

// guild alliance/hostility cost
enum GuildForeignMoney
{
	GFM_DAY_1 = 100000,		
	GFM_DAY_3 = 200000,
	GFM_DAY_7 = 500000,
	GFM_DAY_30 = 1000000,
};

// guild diplomacy info
struct GuildForeignInfo 
{
	UINT32 Id;
	FString Name;
	UINT8 Level;
	UINT8 MemberCount;
	UINT32 LeftTime;
};

typedef std::list<GuildForeignInfo, M_UE3_STL_ALLOC(GuildForeignInfo) > GuildAllyInfoList;
typedef std::list<GuildForeignInfo, M_UE3_STL_ALLOC(GuildForeignInfo) > GuildEnemyInfoList;

// guild alliance application info
struct GuildApplyAllyInfo 
{
	UINT32 Id;
	FString Name;
	UINT8 Level;
	UINT8 MemberCount;
	FString LeaderName;
	UINT32 LeftTime;
};

// guild warehouse record
struct GuildWareAccessInfo 
{
	UINT32 RoleId;
	INT ItemTempId;
	FString RoleName;
	INT Count;
	UINT32 Time;
	INT Type;
};

// guild territory type
enum GuildTerritoryType
{
	GTT_BUILDING = 0,
	GTT_WELFARE
};

// guild building
struct GuildBuildingData
{
	UINT32 BuildingType;			// building Id
	FString Name;					// building name
	UINT32 Level;					// building level
	UINT32 MaxLevel;				// max level
	UINT32 MaxNum;					// max count
	UINT32 BaseNpcId;				// base NPC id
	UINT32 NpcId;					// building NPC id
	UINT32 Effect;					// building effect
	FString Description;			// building description
	UINT32 GuildMoneyReq;			// guild funds requirement
	UINT32 HerbReq;					// herb requirement
	UINT32 WoodReq;					// nanmu wood requirement
	UINT32 CrystalReq;				// magic crystal requirement
	UINT32 SilverReq;				// mithril requirement
	UINT32 UpgradeTime;				// upgrade time
	UINT32 RoutineReq;
	UINT32 PreBuildingType;			// prerequisite building ID
	UINT32 PreBuildingLevel;		// prerequisite building level
	UINT32 Param1;					// parameter 1
	UINT32 Param2;					// parameter 2
	UINT32 Param3;					// parameter 3
	UINT32 Param4;					// parameter 4
	UINT32 Param5;					// parameter 5
	UINT32 UpgradeSpellID;			// corresponding NPC
};

struct GuildBuildingDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("guild_building.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static GuildBuildingData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(GuildBuildingData* obj);
	static void ParseData(GuildBuildingData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(GuildBuildingData* obj)
	{
		return "";
	}

	static void PostInitData();
#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(GuildBuildingData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<GuildBuildingData, GuildBuildingDataProcessor> GuildBuildingDataManager;

struct GuildBuildingInfo
{
	UINT64 BuildingId;
	UINT32 BuildingType;	
	UINT32 BuildingLv;	
	UINT32 UpgradeLeftTime;
	UINT32 UpgradeEndTime;
};

// guild welfare
struct GuildWelfareData
{
	UINT32 WelfareId;				// welfare Id
	FString Name;					// welfare name
	UINT32 BuffId;					// corresponding BuffId
	UINT32 Type;					// type
	UINT32 Level;					// level
	UINT32 MaxLevel;				// max level

	UINT8 ExchangeLevelReq;			// exchange level requirement
	UINT32 ExchangeMoneyReq;		// exchange money requirement
	UINT32 ExchangeContributeReq;	// exchange guild contribution requirement
	UINT8 ExchangeMax;				// max exchange count
	UINT8 VipExchangeMax1;          // vip max exchange count
	UINT8 VipExchangeMax2;          // vip max exchange count
	UINT8 VipExchangeMax3;          // vip max exchange count

	UINT32 UpgradeTime;				// upgrade time
	UINT32 UpgradePreBuildingType;	// upgrade building requirement
	UINT32 UpgradePreBuildingLevel;	// upgrade building level
	UINT32 UpgradeGuildMoneyReq;	// upgrade guild funds requirement
	UINT32 UpgradeHerbReq;			// upgrade herb requirement
	UINT32 UpgradeWoodReq;			// upgrade lumber requirement
	UINT32 UpgradeCrystalReq;		// upgrade magic crystal requirement
	UINT32 UpgradeSliverReq;		// upgrade mithril requirement
};

struct GuildWelfareDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("guild_welfare.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static GuildWelfareData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(GuildWelfareData* obj);
	static void ParseData(GuildWelfareData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(GuildWelfareData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(GuildWelfareData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<GuildWelfareData, GuildWelfareDataProcessor> GuildWelfareDataManager;

struct GuildWelfareUpgradeInfo
{
	UINT32 WelfareId;	
	UINT32 WelfareLv;	
	UINT32 UpgradeLeftTime;
	UINT32 UpgradeEndTime;
};

struct GuildWelfareInfo
{
	UINT32 WelfareId;	
	UINT32 WelfareLv;	
	UINT32 AlreadyBuyTime;			// times already exchanged
};

typedef std::list<GuildApplyAllyInfo, M_UE3_STL_ALLOC(GuildApplyAllyInfo) > GuildApplyAllyInfoList;
typedef std::vector<GuildWareAccessInfo, M_UE3_STL_ALLOC(GuildWareAccessInfo) > AccessInfoList;
typedef std::map<INT, const FString, M_UE3_STL_ALLOC_MAP(INT, const FString) > GuildErrorMap;
typedef std::vector<GUILD_EVENT_INFO, M_UE3_STL_ALLOC(GUILD_EVENT_INFO) > GuildEventsList;

typedef std::map<INT, GuildBuildingData, M_UE3_STL_ALLOC_MAP(INT, GuildBuildingData) > GuildBuildingDataMap;
typedef std::list<GuildBuildingInfo, M_UE3_STL_ALLOC(GuildBuildingInfo) > GuildBuildingList;
typedef std::list<GuildBuildingInfo, M_UE3_STL_ALLOC(GuildBuildingInfo) > GuildBuildingUpgradeList;

typedef std::map<INT, GuildWelfareData, M_UE3_STL_ALLOC_MAP(INT, GuildWelfareData) > GuildWelfareDataMap;
typedef std::list<GuildWelfareInfo, M_UE3_STL_ALLOC(GuildWelfareInfo) > GuildWelfareInfoList;
typedef std::list<GuildWelfareUpgradeInfo, M_UE3_STL_ALLOC(GuildWelfareUpgradeInfo) > GuildWelfareUpgradeList;

// guild dungeon
struct GuildDungeonData
{
	UINT32 DungeonId;
	FString DungeonName;
	FString DungeonDesc;
	UINT32 BuildingReq;
	UINT32 BuildingLvReq;
	UINT32 MoneyReq;
	UINT32 MemberCountLimit;
	UINT32 Award[GUILD_DUNGEON_AWARD_NUM];
};

struct GuildDungeonDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("guild_dungeon.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static GuildDungeonData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(GuildDungeonData* obj);
	static void ParseData(GuildDungeonData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("SceneTemplateID");
	}

	static FString FormatAsString(GuildDungeonData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(GuildDungeonData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<GuildDungeonData, GuildDungeonDataProcessor> GuildDungeonDataManager;
typedef std::map<UINT32, GuildDungeonData, M_UE3_STL_ALLOC_MAP(INT, GuildDungeonData)> GuildDungeonDataMap;
typedef std::list<GUILD_DUNGEON_AWARD_INFO, M_UE3_STL_ALLOC(GUILD_DUNGEON_AWARD_INFO)> GuildDungeonAwardList;

// guild war
typedef std::list<GUILD_BATTLE_ACHIEVEMENT_DATA, M_UE3_STL_ALLOC(GUILD_BATTLE_ACHIEVEMENT_DATA)> GuildWarResultList;
typedef std::vector<GUILD_WAR_RECORD_INFO, M_UE3_STL_ALLOC(GUILD_WAR_RECORD_INFO) > GuildWarRecordList;

enum GuildWarSearchState
{
	GWSS_BEGIN = 0,
	GWSS_COMPLETE,
};

// Bichon supremacy war
enum GuildSabacState
{
	GSS_NULL = 0,
	GSS_ORANGE,
	GSS_BLUE,
	GSS_GREEN,
};

// registered guild list
typedef std::list<G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA, M_UE3_STL_ALLOC(G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA)> GuildSabacEnrollList;

// 
struct SabacStatue
{
	UINT32 RoleId;
	FString RoleName;
	UINT8 RoleClass;
	UINT8 RoleSex;
	UINT8 HairType;
	UINT8 HairColor;
	UINT8 FaceType;
	UINT8 SkinColor;
	UINT8 TitleId;
	UINT8 RefineFx;
	INT32 Avatar[NES_VISIBLE_TOTAL];

	SabacStatue()
	{
		RoleId = 0;
		RoleName = TEXT("");
		RoleClass = 0;
		RoleSex = 0;
		HairType = 0;
		HairColor = 0;
		FaceType = 0;
		SkinColor = 0;
		TitleId = 0;
		RefineFx = 0;
		appMemset(Avatar, 0, sizeof(INT32) * NES_VISIBLE_TOTAL);
	}
};

// guild activity
struct GuildActivityData
{
	UINT16 Type;					// activity type
	UINT16 Level;					// activity level
	UINT32 Variable;				// activity variable
	FString Name;					// activity name
	FString Desc;					// activity description
	UINT32 NpcId;					// activity NPC
	UINT16 MaxCount;				// max completion count of the activity
	UINT32 Duration;				// activity duration

	UINT8 BuildingReq;				// prerequisite building requirement
	UINT8 BuildingLvReq;			// prerequisite building level requirement
	UINT32 MoneyReq;				// funds requirement
	UINT32 HerbReq;					// grain requirement
	UINT32 WoodReq;					// lumber requirement
	UINT32 CrystalReq;				// stone requirement
	UINT32 SilverReq;				// iron ore requirement
};

struct GuildActivityDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("guild_activity.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static GuildActivityData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(GuildActivityData* obj);
	static void ParseData(GuildActivityData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("Type");
	}

	static FString FormatAsString(GuildActivityData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(GuildWelfareData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<GuildActivityData, GuildActivityDataProcessor> GuildActivityDataManager;
typedef std::map<UINT32, GuildActivityData, M_UE3_STL_ALLOC_MAP(UINT32, GuildActivityData)> GuildActivityDataMap;
typedef std::map<UINT32, UINT16, M_UE3_STL_ALLOC_MAP(UINT32, UINT32)> GuildActivityVariableMap;

enum GuildActivityState
{
	GAS_NONE = 0,
	GAS_OPEN,
	GAS_OVER,
	GAS_NORIGHT,
};

struct GuildActivityInfo
{
	UINT16 Type;					// activity type
	UINT8 Complete;					// completion status
	UINT32 StartTime;				// start time
	UINT8 State;					// state: not started, started, ended
};

typedef std::map<UINT16, GuildActivityInfo, M_UE3_STL_ALLOC_MAP(UINT16, GuildActivityInfo)> GuildActivityInfoMap;

// guild activity character variables
enum EGuildActivityRoleVariable
{
	EGARV_ID_1 = 193,			// character variable ID: times the character did guild prayer today
	EGARV_ID_2 = 217,			// character variable ID: last time the character did guild prayer
	EGARV_ID_3 = 179,           // character variable ID: last time the character did guild trading
	EGARV_ID_4 = 187,           // character variable ID: last time the character soaked in the hot spring
	EGARV_ID_5 = 189,           // character variable ID: last time the character killed a demon beast
};
typedef std::map<UINT16, INT32, M_UE3_STL_ALLOC_MAP(UINT16, INT32)> GuildVariableMap;


class MMOGuildData : public SingletonMeyers<MMOGuildData>
{
public:
	MMOGuildData();
	virtual ~MMOGuildData();

	void Init();
	void Shutdown();
	void Clear();

	// guild view
	const GuildViewList& GetGuildViewInfoList();
	UINT32 GetCurListFirstId();
	UINT32 GetCurListLastId();
	void UpdateGuildViewList(UINT8 type, GUILD_VIEW_SUMMARY* list, UINT16 size);
	void AddGuildViewInfo(const GuildViewInfo& info);
	UBOOL GetGuildViewInfo(UINT32 guildId, GuildViewInfo& info);
	
	// my guild info
	const GuildBaseInfo& GetGuildBaseInfo() { return _GuildBaseInfo; }
	void UpdateGuildBaseInfo(const GuildBaseInfo& baseInfo);

	void UpdateGuildEvents(GUILD_EVENT_INFO* events, UINT8 size);
	void UpdateGuildMoneyEvents(GUILD_EVENT_INFO* events, UINT8 size);
	void AddGuildEventInfo(const GUILD_EVENT_INFO& eventInfo);
	void AddGuildMoneyEventInfo(const GUILD_EVENT_INFO& eventInfo);
	void UpdateGuildEvents();
	void UpdateGuildMoneyEvents();

	// guild members
	void UpdateGuildFullMemberList(GUILD_MEMBER_INFO* list, UINT8 size);
	void AddGuildFullMember(const GUILD_MEMBER_INFO& info);
	void DelGuildFullMember(UINT32 id);
	void UpdateGuildFullMemberPost(UINT32 id, UINT8 post);

	void UpdateGuildPendingMemberList(GUILD_APPLICANT_INFO* list, UINT8 size);

	const GuildFullMemberInfoList& GetGuildFullMemberInfoList();
	const GuildPendingMemberInfoList& GetGuildPendingMemberInfoList();
	void GetGuildPostName(UINT8 type, FString& name);
	void GetJobName(UINT32 type, FString& name);
	UINT GetGuildFullMemberOnlineCount();
	UINT32 GetGuildFullMemberId(const FString& roleName);
	void GetGuildFullMemberName(UINT32 roleId, FString& roleName);
	UINT GetGuildFullMemberPost(UINT32 roleId);
	UINT32 GetGuildFullMemberSalary(UINT32 roleId);
	UBOOL GetGuildFullMemberInfo(UINT32 roleId, GuildFullMemberInfo& info);
	UBOOL IsGuildFullMember(UINT32 roleId);
	UBOOL IsGuildFullMember(const FString& roleName);
	void SetGuildFullMemberForbitChat(UINT32 roleId, UBOOL isForbid);
	UBOOL IsGuildFullMemberForbitChat(UINT32 roleId);
	void SortGuildFullMember(INT type, UBOOL isUp);
	void UpdateGuildFullMemberData(UINT32 roleId, UINT8 level, INT32 sceneId, UINT32 lastLoginTime);
	void GuildLeaderDemise(UINT32 newLeaderId);
	UINT8 GetGuildPostCount(UINT8 post);

	// guild warehouse
	void SetWarehouseItem(INT pageId, INT idx, FClientItemInfo* item);
	void ClearWarehouseItemInfo();
	void SetWarehouseItemInfo(INT pageId, INT idx, ItemInfo* item);
	void SwapItem(INT srcPageId, INT srcSlotId , INT desPageId , INT desSlotId);
	INT GetEmptyGuildWarehouseSlot( INT pageId );
	FClientItemInfo* GetWarehouseItem(INT pageId, INT idx);

	void ClearAccessInfo();
	void AddAccessInfo(INT playerId, const FString& roleName, INT itemTempId, INT count, INT time, INT type);
	AccessInfoList& GetAccessInfoList(){ return _AccessInfoList; }

	// guild management
	UINT16 GetGuildRights(UINT8 post);
	UINT16 GetGuildStorageRights(UINT8 post);
	UINT8 GetGuildSalaryPercent(UINT8 post);
	void UpdateGuildStorageRights(UINT8 post, UINT16 rights);
	void UpdateMyGuildSalary(UINT32 salary);
	void UpdateGuildSalaryPercent(const UINT8 (&saleryPercent)[GUILD_POSITION_COUNT]);

	// guild diplomacy
	const GuildAllyInfoList& GetGuildAllyInfoList();
	const GuildEnemyInfoList& GetGuildEnemyInfoList();
	const GuildApplyAllyInfoList& GetGuildApplyAllyInfoList();
	GuildRelation GetGuildRelation(const FString& guildName);
	bool IsGuildAlly(UINT32 id);
	bool IsGuildEnemy(UINT32 id);
	bool IsGuildEnemy(const FString& guildName);

	void UpdateGuildAllyList(GUILD_DIPLOMAT_INFO* list, UINT8 size);
	void AddGuildAlly(UINT32 id, const FString& name, UINT8 level, UINT8 memberCount, UINT32 leftTime);
	void DelGuildAlly(UINT32 id);
	void UpdateGuildEnemyList(GUILD_DIPLOMAT_INFO* list, UINT8 size);
	void AddGuildEnemy(UINT32 id, const FString& name, UINT8 level, UINT8 memberCount, UINT32 leftTime);
	void DelGuildEnemy(UINT32 id);
	void UpdateGuildApplyAllyList(APPLY_ALLIANCE_INFO* list, UINT8 size);
	void AddGuildApplyAllyInfo(const GuildApplyAllyInfo& info);

	void UpdateGuildAllyTime(UINT32 count);
	void UpdateGuildEnemyTime(UINT32 count);
	void UpdateGuildApplyAllyTime(UINT32 count);

	// guild building
	void AddGuildBuildingData(const GuildBuildingData& data);
	const GuildBuildingDataMap& GetGuildBuildingDataMap() { return _GuildBuildingDataMap; }
	UBOOL GetGuildBuildingData(UINT32 buildingType, UINT32 buildingLv, GuildBuildingData& data);
	const GuildBuildingUpgradeList& GetGuildBuildingUpgradeList();
	const GuildBuildingList& GetGuildBuildingList();
	UINT32 GetGuildBuildingLevel(UINT64 buildingId);
	UINT32 GetGuildBuildingType(UINT64 buildingId);
	UINT32 GetGuildBuildingCount(UINT32 buildingType);
	UINT32 GetGuildBuildingMaxLevel(UINT32 buildingType);
	UINT32 GetGuildTerritoryResMax(UINT32 type);
	UINT32 GetCurGuildTerritoryRes(UINT32 type);
	void GetGuildTerritoryResName(UINT32 type, FString& name);
	UINT32 GetCurGuildMemberMax();
	UBOOL IsGuildBuildingBase(UINT32 npcTempId);
	UBOOL IsGuildBuilding(UINT32 npcTempId);
	UINT64 GetGuildBuildingId(UINT32 npcTempId);
	UINT32 GetGuildUpgradeBuildingType(UINT64 buildingId);

	void UpdateGuildBuildingList(const G_B_DATA (&arrBuilding)[GUILD_MAX_BUILDING_NUM]);
	void AddGuildBuilding(const GuildBuildingInfo& info);
	void DelGuildBuilding(UINT64 buildingId);
	void UpgradeGuildBuilding(UINT64 buildingId);
	UBOOL IsGuildBuildingExist(UINT64 buildingId);

	void AddGuildBuildingUpgradeInfo(const GuildBuildingInfo& info);
	void DelGuildBuildingUpgradeInfo(UINT64 buildingId);
	UBOOL IsGuildBuildingUpgrading(UINT64 buildingId);

	// guild welfare
	void AddGuildWelfareData(const GuildWelfareData& data);
	UBOOL GetGuildWelfareData(UINT32 welfareId, UINT32 welfareLv, GuildWelfareData& data);
	const GuildWelfareUpgradeList& GetGuildWelfareUpgradeList();
	const GuildWelfareInfoList& GetGuildWelfareList();
	UINT32 GetGuildWelfareLevel(UINT32 welfareId);
	UBOOL IsGuildWelfarePurchase(UINT32 welfareId);
	UBOOL IsGuildWelfareUpgrading(UINT32 welfareId);
	void UpdateGuildWelfareAlreadyBoughtTime(UINT32 welfareId, UINT8 times);

	void UpdateGuildWelfareList(const INT8 (&arrWelfareLv)[GUILD_MAX_WELFARE_AURA_COUNT],  
		const INT8 (&arrWelfareUpId)[GUILD_MAX_UPGRADE_AURA_COUNT], 
		const INT32 (&arrWelfareUpTime)[GUILD_MAX_UPGRADE_AURA_COUNT]);
	void UpgradeGuildWelfare(UINT32 welfareId, UINT32 welfareLv);
	void AddGuildWelfareUpgradeInfo(const GuildWelfareUpgradeInfo& info);
	void DelGuildWelfareUpgradeInfo(UINT32 welfareId);

	UINT32 GetGuildWelfareGroupCount();
	void GetGuildWelfareListByGroup(UINT32 groupId, GuildWelfareInfoList& list);

	void UpdateGuildBuildingUpgradeTime(UINT32 count);
	void UpdateGuildWelfareUpgradeTime(UINT32 count);

	// guild dungeon
	void AddGuildDungeonData(const GuildDungeonData& data);
	const GuildDungeonDataMap& GetGuildDungeonDataMap();
	const GuildDungeonAwardList& GetGuildDungeonAwardList();
	void UpdateGuildDungeonAwardList(GUILD_DUNGEON_AWARD_INFO* list, UINT8 size);
	void AddGuildDungeonAwardInfo(const GUILD_DUNGEON_AWARD_INFO& info);

	void SetMyGuildWarAttackId(UINT32 id);
	UINT32 GetMyGuildWarAttackId() {return _MyGuildWarAttackId;}
	void SetMyGuildWarDefendId(UINT32 id);
	UINT32 GetMyGuildWarDefendId() {return _MyGuildWarDefendId;}

	// guild query
	UBOOL GetGuildInfoById(UINT32 guildId, GuildQueryInfo& info);
	UBOOL GetGuildInfoByName(const FString& guildName, GuildQueryInfo& info);
	void AddOrUpdateGuildInfo(M2C_QUERY_GUILD_NAME_ACK* msg);
	const QueryGuildInfoByIdMap& GetQueryGuildInfoByIdMap();

	// Bichon supremacy war
	void UpdateSabacEnrollGuildList(G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA* list, UINT16 size);
	void UpdateSabacOccupyGuild(UINT32 guildId);
	UINT32 GetSabacOccupyGuildId() { return _CurOccupySabacGuildId; }
	const GuildSabacEnrollList& GetSabacEnrollList() { return _SabacEnrollGuildIdList; }
	GuildSabacState GetGuildSabacState(AMMOPlayer* player);

	void UpdateStatueData(const SabacStatue& statue);
	UBOOL GetStatueData(SabacStatue& statue);

	// guild activity
	void AddGuildActivityData(const GuildActivityData& data);
	UBOOL GetGuildActivityData(UINT32 activityId, GuildActivityData& data);
	void GenerateGuildActivityInfoList();
	const GuildActivityInfoMap& GetGuildActivityInfoMap();
	UINT16 GetGuildActivityTypeByVariable(UINT32 variable);
	void SetGuildActivityTime(UINT16 activityType, UINT32 time);
	void UpdateGuildActivityState();
	UINT8 GetGuildActivityComplete(UINT16 activityType);

	// guild war
	const GuildWarResultList& GetGuildWarResultList();
	void UpdateGuildWarResultList(GUILD_BATTLE_ACHIEVEMENT_DATA* list, UINT8 size);
	void AddGuildWarResult(const GUILD_BATTLE_ACHIEVEMENT_DATA& info);
	// guild war notice board
	void UpdateGuildWarRecords(GUILD_WAR_RECORD_INFO* records, UINT8 size);
	void AddGuildWarRecord(const GUILD_WAR_RECORD_INFO& recordInfo);
	const GuildWarRecordList& GetGuildWarRecordList();
	void ClearVariable();
	void UpdateVariable(UINT8 varType, INT32 referId, UINT16 index, INT32 val);
	INT32 GetVariableValue(UINT8 varType, UINT16 index);

protected:
	UBOOL _LoadGuildBuildingData(const TCHAR* filePath);
	void _UnloadGuildBuildingData();

	UBOOL _LoadGuildWelfareData(const TCHAR* filePath);
	void _UnloadGuildWelfareData();

	UBOOL _LoadGuildDungeonData(const TCHAR* filePath);
	void _UnloadGuildDungeonData();

	UBOOL _LoadGuildActivityData(const TCHAR* filePath);
	void _UnloadGuildActivityData();

	// get names of various guild members (online, offline, left, expelled)
	void _GetGuildEventsRoleName(UINT32 roleId, FString& roleName);
	void _GetGuildEventsGuildName(UINT32 guildId, FString& guildName);

	UINT32 _GetGuildBuildingRealId(UINT32 buildingId, UINT32 buildingLv);
	UINT32 _GetGuildWelfareRealId(UINT32 welfareId, UINT32 welfareLv);

protected:
	FClientItemInfo _GuildWarehouseItemTable[E_GUILD_STORAGE_TAG_MAX][GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG];

	// guild view
	GuildViewList _GuildViewInfoList;

	// my guild info
	GuildBaseInfo _GuildBaseInfo;
	GuildEventsList _GuildEventsList;
	GuildEventsList _GuildMoneyEventsList;

	// guild members
	GuildFullMemberInfoList _GuildFullMemberInfoList;
	GuildPendingMemberInfoList _GuildPendingMemberInfoList;

	// guild warehouse
	AccessInfoList _AccessInfoList;

	// guild management
	UINT16 _GuildStorageRights[GUILD_POSITION_COUNT];
	UINT8 _GuildSalaryPercent[GUILD_POSITION_COUNT];
	UINT8 _GuildPostCount[GUILD_POSITION_COUNT];

	// guild diplomacy
	GuildAllyInfoList _GuildAllyInfoList;
	GuildEnemyInfoList _GuildEnemyInfoList;
	GuildApplyAllyInfoList _GuildApplyAllyInfoList;

	// guild territory
	GuildBuildingDataMap _GuildBuildingDataMap;
	GuildBuildingList _GuildBuildingList;
	GuildBuildingUpgradeList _GuildBuildingUpgradeList;

	// guild welfare
	GuildWelfareDataMap _GuildWelfareDataMap;
	GuildWelfareInfoList _GuildWelfareInfoList;
	GuildWelfareUpgradeList _GuildWelfareUpgradeList;

	// guild dungeon
	GuildDungeonDataMap _GuildDungeonDataMap;
	GuildDungeonAwardList _GuildDungeonAwardList;

	// guild war
	GuildWarResultList _GuildWarResultList;
	UINT32 _MyGuildWarAttackId;				// ID of the guild I declared war on
	UINT32 _MyGuildWarDefendId;				// ID of the guild that declared war on me
	GuildWarRecordList _GuildWarRecordList;

	// guild query
	QueryGuildInfoByIdMap _QueryGuildInfoByIdMap;
	QueryGuildInfoByNameMap _QueryGuildInfoByNameMap;

	// Bichon supremacy war
	GuildSabacEnrollList _SabacEnrollGuildIdList;
	UINT32 _CurOccupySabacGuildId;
	SabacStatue _CurSabacStatue;

	// guild activity
	GuildActivityDataMap _GuildActivityDataMap;
	GuildActivityInfoMap _GuildActivityInfoMap;
	GuildActivityVariableMap _GuildActivityVariableMap;
	GuildVariableMap _GuildVariableMap;

};

#endif