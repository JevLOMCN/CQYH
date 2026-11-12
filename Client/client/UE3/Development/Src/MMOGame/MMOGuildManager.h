#ifndef _MMOGUILDMANAGER_H_
#define _MMOGUILDMANAGER_H_

#include "MMOSingleton.h"
#include <map>
#include <vector>
#include "UnString.h"
#include "guild_def.h"
#include "MMOFacade.h"
#include "MMOGuildData.h"

// guild message orb parameter type
enum GuildMsgButtonType
{
	GMBT_ATTACK = 1,
	GMBT_DEFEND,
	GMBT_APPLY_ALLY,
};

// guild event
struct MMOGameEvent_Guild : public MMOGameEvent
{
public:
	enum EGuildEventType
	{
		EGET_UpdateGuildViewList,
		EGET_UpdateGuildFindList,
		EGET_UpdateGuildBaseInfo,
		EGET_UpdateGuildNotice,
		EGET_UpdateGuildEvents,
		EGET_UpdateGuildFullMemberList,
		EGET_UpdateGuildPendingMemberList,
		EGET_UpdateGuildStorageRights,
		EGET_UpdateGuildSalary,
		EGET_UpdateGuildAllyList,
		EGET_UpdateGuildEnemyList,
		EGET_UpdateGuildApplyAllyList,
		EGET_UpdateGuildTerritoryInfo,
		EGET_UpdateGuildTerritoryUpgradeList,
		EGET_UpdateGuildBuildingList,
		EGET_UpdateGuildWelfareList,
		EGET_UpdateGuildWarEndTime,
		EGET_UpdateGuildWarCD,
		EGET_UpdateGuildWarReadyTime,
		EGET_PlayerMoneyChange,
		EGET_ShowGuildView,
		EGET_BeginSearch,
		EGET_CompleteSearch,
		EGET_UpdateGuildMoneyEvents,
		EGET_UpdateGuildWarTime,
		EGET_UpdateSabacEnrollList,
		EGET_UpdateGuildActivity,
		EGET_UpdateGuildWarRecord,
		EGET_NoMoreGuildWarRecord,
	};

	struct GuildFindInfo
	{
		UINT32 Id; 
		FString Name;
		UINT32 Level; 
		UINT32 MemberCount; 
		FString LeaderName;
		FString CreatorName;
		UINT32 CreateTime;
		FString Announce;
		UINT32 ProtectionTime;

		GuildFindInfo () 
			: Id(0)
			, Name(TEXT(""))
			, Level(0)
			, MemberCount(0)
			, LeaderName(TEXT(""))
			, CreatorName(TEXT(""))
			, CreateTime(0)
			, Announce(TEXT(""))
			, ProtectionTime(0)
		{}
	};

	MMOGameEvent_Guild(EGuildEventType eventType)
		: _EventType(eventType)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Guild;
	}

	EGuildEventType _EventType;

	GuildFindInfo _FindInfo;
	UINT8 _CurPost;
	INT _BuildingId;
	INT _WelfareId;

	INT _ViewType;
	INT _ViewSize;
};

class MMOGuildManager : public SingletonMeyers<MMOGuildManager>, public INativeGameEventHandler
{
public:
	MMOGuildManager();
	virtual ~MMOGuildManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void NotifyCreateGuildSuccess(const FString& guildName);
	void NotifyApplyToJoinGuildSuccess();
	void NotifyJoinGuildSuccess(const FString& guildName);
	void NotifyUpdateGuildViewList(UINT8 type, GUILD_VIEW_SUMMARY* list, UINT16 size);
	void NotifyUpdateGuildFindList(const GUILD_VIEW_SUMMARY& result);
	void NotifyUpdateGuildBaseInfo(const GuildBaseInfo& baseInfo);
	void NotifyUpdateGuildNotice(const FString& notice);
	void NotifyUpdateGuildEvents(GUILD_EVENT_INFO* events, UINT8 size);
	void NotifyMoreGuildEvents(GUILD_EVENT_INFO* events, UINT8 size);
	void NotifyUpdateGuildFullMemberList(GUILD_MEMBER_INFO* list, UINT8 size);
	void NotifyUpdateGuildFullMemberData(UINT32 roleId, UINT8 level, INT32 sceneId, UINT32 lastLoginTime);
	void NotifyUpdateGuildPendingMemberList(GUILD_APPLICANT_INFO* list, UINT8 size);
	void NotifyBeInviteToJoinGuild(UINT32 roleId, const FString& roleName, const FString& guildName);
	void NotifyInviteToJoinGuild(UINT32 replyType, const FString& roleName);
	void NotifyAddGuildFullMember(const GUILD_MEMBER_INFO& info);
	void NotifyGuildMemberSalaryChange(UINT32 salary);
	void NotifyDelGuildFullMember(UINT32 id);
	void NotifyChangeGuildFullMemberPost(UINT32 id, UINT8 post);
	void NotifyUpdateGuildStorageRights(UINT8 post, UINT16 rights);
	void NotifyUpdateGuildAttr(UINT32 type, UINT32 value);
	void NotifyUpdateGuildSalaryPercent(const UINT8 (&saleryPercent)[GUILD_POSITION_COUNT]);
	void NotifyUpdateGuildApplyAllyList(APPLY_ALLIANCE_INFO* list, UINT8 size);
	void NotifyUpdateGuildAllyList(GUILD_DIPLOMAT_INFO* list, UINT8 size);
	void NotifyUpdateGuildEnemyList(GUILD_DIPLOMAT_INFO* list, UINT8 size);
	void NotifyAddGuildAlly(UINT32 id, const FString& name, UINT8 level, UINT8 memberCount, UINT32 leftTime);
	void NotifyDelGuildAlly(UINT32 id);
	void NotifyDelGuildAllySuccess(UINT32 id);
	void NotifyAddGuildEnemy(UINT32 id, const FString& name, UINT8 level, UINT8 memberCount, UINT32 leftTime);
	void NotifyDelGuildEnemy(UINT32 id);
	void NotifyGuildLeaderDemise(UINT32 oldLeaderId, UINT32 newLeaderId);
	void NotifyExitGuild();
	void NotifyBanishGuild();
	void NotifyDisbandGuild();
	void NotifyPlayerMoneyChange(EMONEY_TYPE moneyType, INT64 DeltaMoney);

	void NotifyUpdateGuildBuildingList(const G_B_DATA (&arrBuilding)[GUILD_MAX_BUILDING_NUM]);
	void NotifyUpdateGuildBuildingUpgradeList(const INT32 (&arrBuildingUpTime)[GUILD_B_NUM]);
	void NotifyUpgradeGuildBuilding(UINT64 buildingId, UINT32 buildingType, INT32 time);
	void NotifyUpdateGuildBuilding(UINT64 buildingId, UINT32 buildingType);
	void NotifyDestroyGuildBuilding(UINT64 buildingId, UINT32 buildingType);
	void NotifyCancelGuildBuildingUpgrade(UINT64 buildingId, UINT32 buildingType);
	void NotifyGuildBuildingState(UINT8 buildingType, UINT8 state);
	void NotifyInteractBuildingNpc(UINT32 npcId, UINT32 npcTempId);

	void NotifyUpdateGuildWelfareList(const INT8 (&arrWelfareLv)[GUILD_MAX_WELFARE_AURA_COUNT], 
		const INT8 (&arrWelfareUpId)[GUILD_MAX_UPGRADE_AURA_COUNT], 
		const INT32 (&arrWelfareUpTime)[GUILD_MAX_UPGRADE_AURA_COUNT]);
	void NotifyUpgradeGuildWelfare(UINT32 welfareId, INT32 time);
	void NotifyUpdateGuildWelfare(UINT32 welfareId, UINT32 welfareLv);
	void NotifyCancelGuildWelfareUpgrade(UINT32 welfareId);
	void NotifyUpdateGuildWelfareInfoList(GUILD_BUY_AURA_RECORD_INFO* welfareInfoList, INT16 count);
	void NotifyPurchaseGuildWelfare(UINT32 welfareId, UINT32 alreadyBuyTime);
	void NotifyGuildContributeSuccess(UINT32 type, UINT32 count);

	void NotifyUpdateGuildDungeonList(GUILD_DUNGEON_INFO* list, UINT32 size);
	void NotifyOpenDungeonSuccess();
	void NotifyDungeonEnd(UINT32 bossId, GUILD_DUNGEON_AWARD_INFO* list, UINT8 size);

	void NotifyGuildWarCD(UINT32 time);
	void NotifyGuildWarState(UINT8 state,
		UINT32 attackId,
		const FString& attackName,
		UINT32 defendId, 
		const FString& defendName,
		INT32 lastTime);
	void NotifyGuildWarResult(GUILD_BATTLE_ACHIEVEMENT_DATA* list, UINT8 size);
	void NotifyGuildWarRelation(UINT32 attackId, UINT32 defendId);

	void NotifyQueryGuildInfo(M2C_QUERY_GUILD_NAME_ACK* msg);
	void NotifyGuildFullMemberForbitChat(UINT32 roleId, UBOOL isForbid);

	void NotifyGuildDrawSalarySuccess(INT32 salary);

	void SetOnlineMemberNotify(UBOOL notify);
	void UpdateGuildEvents();
	void UpdateGuildMoneyEvents();
	void SortGuildFullMember(INT type, UBOOL isUp);
	INT GetGuildWarEndTime() {return _GuildWarEndTime;}
	UINT32 GetGuildWarCD() {return _GuildWarCDTime;}
	INT GetGuildWarReadyTime() {return _GuildWarReadyTime;}
	INT GetGuildWarTime() {return _GuildWarTime;}

	void EnterScene(INT32 sceneTempId, INT32 sceneId);
	INT32 GetCurTerritorySceneId() { return _CurSceneGuildId; }

	void NotifyGuildWarSearchBegin(const GuildViewInfo& info);
	void NotifyGuildWarSearchEnd();
	UBOOL GetCurGuildWarViewInfo(GuildViewInfo& info);
	void NotifyUpdateGuildMoneyEvents(GUILD_EVENT_INFO* events, UINT8 size);
	void NotifyUpdateMoreGuildInfo(INT32 matchWarConsumption, UINT8 matchCount); 

	void SetOpenGuildWarStartPanelAttackDelegate(FScriptDelegate fun);
	void SetOpenGuildWarStartPanelDefendDelegate(FScriptDelegate fun);
	void SetOpenGuildMainPanelApplyAllyDelegate(FScriptDelegate fun);

	void NotifyGuildEventsHint(UINT8 type);
	void NotifyUpdateActorGuildTitle(UINT32 roleId, const FString& guildName, UINT32 guildId);
	void NotifyApplyAllianceSuccess();

	void NotifyUpdateSceneVariable(INT32 mapId, UINT16 varIndex, INT32 varValue);

	// Bichon supremacy war
	void NotifyUpdateSabacEnrollList(G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA* list, UINT16 size);
	void NotifySabacEnrollGuildId(UINT32 guildId);
	// update occupying guild ID
	void NotifyUpdateSabacOccupyGuild(UINT32 guildId, UBOOL showMarquee = TRUE);
	void NotifySabacEnd(UINT32 oldGuildId, UINT32 newGuildId);
	void NotifySabacState(UINT32 sabacEndTime);
	UINT8 GetSabacState() {return _SabacState;}
	void GetSabacOccupyGuildName(FString& name);
	UBOOL IsSabacOccupyGuild(const FString& name);
	void NotifySabacWinnerLogin(const FString& winnerName);
	void NotifyCreateStatue(const SabacStatue& statue);
	void UpdateStatueHint(UBOOL forceUpdate);

	void NotifyGuildLeaveCD(UINT32 cd);
	INT GetGuildLeaveCD();

	void UpdateGuildBuilding(ANPC* building);

	// guild info query
	// Quest #23580
	void QueryGuildInfo(UINT32 guildId);
	void NotifyGuildQueryInvalidId(UINT32 guildId);

	// guild stronghold war
	void ClearGuildOccupy();
	void UpdateOccupyState(UINT32 mapId, UINT8 state);
	UINT8 GetOccupyState(UINT32 mapId);
	void UpdateOccupyGuild(UINT32 mapId, UINT32 guildId);
	void GetOccupyGuildName(UINT32 mapId, FString& name);

	// 6.28 guild war
	void NotifyUpdateGuildWarRecords(GUILD_WAR_RECORD_INFO* records, UINT16 size);
	void NotifyNoMoreGuildWarRecords();
	void NotifyGuildJoinWar(UINT8 warCharacter, UINT32 targetGuildID);
	UBOOL IsGuildWarStart() { return _IsGuildWarStart; }

protected:
	void _ResetLocalGuildInfo();

	void _UpdateGuildTerritroyUpgrade(FLOAT deltaTime);
	void _UpdateGuildForeignTime(FLOAT deltaTime);
	void _UpdateGuildWarViewTime(FLOAT deltaTime);
	void _UpdateGuildWarReadyTime(FLOAT deltaTime);
	void _UpdateGuildWarTime(FLOAT deltaTime);
	void _UpdateGuildWarEndTime(FLOAT deltaTime);
	void _UpdateGuildWarCD(FLOAT deltaTime);
	void _UpdateGuildBuildingBasePos(FLOAT deltaTime);
	void _UpdateGuildQueryInvalidIdTime(FLOAT deltaTime);
	void _UpdateGuildViewEventsTime(FLOAT deltaTime);
	void _UpdateGuildWarRecordsTime(FLOAT deltaTime);

	void _UpdateMainActorGuildTitle(const FString& guildName);
	void _UpdateActorGuildTitle(UINT32 id, const FString& name);
	void _UpdateAllGuildTitle();
	void _UpdateAllGuildInfo();

	// Sabak statue
	void _CreateStatue(const SabacStatue& statue, ANPC* statueBase);
	void _DestroyStatue();

	// guild arrow tower base
	void _CreateBuildingBase(ANPC* building, USkeletalMeshComponent* baseMesh);
	void _DestroyBuildingBase(ANPC* building);

protected:
	UBOOL _OnlineMemberNotify;

	FLOAT _GuildAllyTimeProgress;
	FLOAT _GuildEnemyTimeProgress;
	FLOAT _GuildApplyAllyTimeProgress;

	FLOAT _GuildBuildingUpgradeTimeProgress;
	FLOAT _GuildWelfareUpgradeTimeProgress;

	FLOAT _GuildWarCDProgress;
	INT _GuildWarCDTime;

	FLOAT _GuildWarEndProgress;
	INT _GuildWarEndTime;

	FLOAT _GuildWarReadyProgress;
	INT _GuildWarReadyTime;
	
	FLOAT _GuildWarProgress;
	INT _GuildWarTime;

	INT32 _CurSceneGuildId;

	typedef std::map<UINT32, UINT32, M_UE3_STL_ALLOC_MAP(UINT32, UINT32)> NeedUpdateGuildNameRoleMap;
	NeedUpdateGuildNameRoleMap _NeedUpdateGuildNameRoleMap;

	// guild war query
	GuildViewInfo _CurGuildWarViewInfo;
	FLOAT _GuildWarViewProgress;
	INT _GuildWarViewDelayTime;
	
	FScriptDelegate _OpenGuildWarStartPanelAttackDelegate;
	FScriptDelegate _OpenGuildWarStartPanelDefendDelegate;
	FScriptDelegate _OpenGuildMainPanelApplyAllyDelegate;

	UINT8 _CurGuildWarState;
	FString _CurAttackGuildName;
	FString _CurDefendGuildName;

	// Bichon supremacy war
	UINT8 _SabacState;
	FLOAT _SabacProgress;
	AMMOPlayer* _StatueActor;
	UGFxObject* _StatueHeadName;
	AMMONavitationEmitter* _StatueEffect;
	INT _StatueNpcId;

	UINT32 _GuildLeaveEndTime;

	// guild turret base
	typedef std::map<INT, INT, M_UE3_STL_ALLOC_MAP(INT, INT)> GuildBuildingBaseActorMap;
	GuildBuildingBaseActorMap _GuildBuildingBaseActorMap;
	FLOAT _GuildBuildingBaseProgress;

	// Quest #23580
	typedef std::map<UINT32, INT, M_UE3_STL_ALLOC_MAP(UINT32, INT)> GuildQueryInvalidIdTimeMap;
	GuildQueryInvalidIdTimeMap _GuildQueryInvalidIdTimeMap;
	FLOAT _GuildQueryInvalidIdTimeProgress;

	// 
	FLOAT _GuildViewEventsProgress;
	UBOOL _GuildViewEventsDelay;

	// guild stronghold war
	typedef std::map<UINT32, UINT8, M_UE3_STL_ALLOC_MAP(UINT32, UINT8)> GuildOccupyStateMap;
	GuildOccupyStateMap _GuildOccupyStateMap;

	typedef std::map<UINT32, UINT32, M_UE3_STL_ALLOC_MAP(UINT32, UINT32)> GuildOccupyIdMap;
	GuildOccupyIdMap _GuildOccupyIdMap;

	// 6.28 version guild war
	UBOOL _IsGuildWarStart;
	FLOAT _GuildWarRecordsProgress;
	UBOOL _GuildWarRecordsDelay;
};

#endif