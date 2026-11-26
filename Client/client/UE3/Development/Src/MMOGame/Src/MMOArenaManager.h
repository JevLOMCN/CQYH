
#ifndef _MMOARENAMANAGER_H_
#define _MMOARENAMANAGER_H_

#include "MMOSingleton.h"
//#include "net\message_handler.h"
#include "arena_def.h"

#define AREANA_AWARD_MONEY_FACTOR (10000)
#define VS_ONE_ON_ONE_DAILY_RWD_MAX (5)

//------------------------
//enum EArenaEventType
//{
//	EArean_StateChanged,
//};
//
//struct MMOGameEvent_Arena : public MMOGameEvent_Any
//{
//	MMOGameEvent_Arena( EArenaEventType arenaEvtType )
//		: MMOGameEvent_Any(GameEvent_Arena)
//		,_ArenaEvtType(arenaEvtType)
//	{
//	}
//
//	EArenaEventType _ArenaEvtType;
//	INT _intData1;
//};

struct PlayerArenaData
{
	PlayerArenaData()
	{
		_CurHighestScore = 0;
		_AllTimeHightScore = 0;
		_SpendMoney = 0;
		_Rank = 0;
		_WinLevel = 0;
		_WinMoney = 0;
		_WinItemID = 0;
	}
	INT _CurHighestScore;
	INT _AllTimeHightScore;
	UINT _SpendMoney;
	INT _Rank;
	INT _WinLevel;
	INT _WinItemID;
	UINT32 _WinMoney;
};

//------------------------
enum EArenaWinLevel
{
	EArean_WinPrizeRank1,
	EArean_WinPrizeRank2,
	EArean_WinPrizeRank3,
	EArean_WinPrizeRank4,
	EArean_WinPrizeRankMax,
	EArean_WinPrizeRankInvalid
};

enum EArenaRank
{
	EArean_Rank1,
	EArean_Rank2,
	EArean_Rank3,
	EArean_Rank4,
	EArean_RankMax,
	EArean_RankInvalid
};

struct ArenaWinPrize
{
	ArenaWinPrize()
	{
		_MoneyToAward = 0;
		_MaxPlayerGetNum = 0;
		_PerPlayerPect = 0.0f;
	}
	UINT64 _MoneyToAward;
	INT _MaxPlayerGetNum;
	FLOAT _PerPlayerPect;
};

struct ArenaRankPrize
{
	ArenaRankPrize() : _RankMoneyNeed(0), _AwardPrizeID(0) {}
	UINT _RankMoneyNeed;
	INT _AwardPrizeID;
	FString _Desc;
};

enum EArenaState
{
	EAnaSta_Invlid,
	EAnaSta_WaitApply,
	EAnaSta_Applied,
	EAnaSta_InGame,
	EAnaSta_WaitStartRound,
	EAnaSta_InRound,
	EAnaSta_WaitNextRound,
	EAnaSta_ToGetAward,
	EAnaSta_EndWithAwarded,
	EAnaSta_EndNoAward,
};

//struct MMOGameEvent_ArenaUiUpdate : public MMOGameEvent_Any
//{
//public:
//
//	MMOGameEvent_ArenaUiUpdate(EGameEvent evtType)
//		: MMOGameEvent_Any(evtType)
//	{
//
//	}
//};

struct MMOGameEvent_VSOneOnOneResult : public MMOGameEvent_Any
{
	MMOGameEvent_VSOneOnOneResult( INT result, INT damage, INT vsMoney, INT score, INT lastSec )
		:MMOGameEvent_Any(GameEvent_VSOneOnOneResult)
		,_VSResult(result)
		,_Damage(damage)
		,_vsMoneyDelta(vsMoney)
		,_ScoreDelta(score)
		,_LastSec(lastSec)
	{
	}

	INT _VSResult;
	INT _Damage;
	INT _vsMoneyDelta;
	INT _ScoreDelta;
	INT _LastSec;
};

struct MMOGameEvent_VSOneOnOneRewardInfo : public MMOGameEvent_Any
{
	MMOGameEvent_VSOneOnOneRewardInfo( INT64 totalRwd, INT64 avgRwd )
		:MMOGameEvent_Any(GameEvent_VSOneOnOneRewardInfo)
		,_TotalRwd(totalRwd)
		,_AvgRwd(avgRwd)
	{
	}

	INT64 _TotalRwd;
	INT64 _AvgRwd;
};

enum EVSNotify
{
	EVSNotify_SideUsedRecovery,
	EVSNotify_SideInfo
};

struct VSPKInfoPersonal
{
	VSPKInfoPersonal() : _ActorID(0), _UsedRecovery(0) {}
	INT _ActorID;
	INT _UsedRecovery;
};
enum EVSSideIdx
{
	EVSSide_1,
	EVSSide_2,
	EVSSide_Total
};
struct VSPKInfo
{
	VSPKInfo() : _SideIdx(0), _UsedRecovery(0) {}
	INT _SideIdx;
	FString _Name;
	INT _UsedRecovery;
	VSPKInfoPersonal _PerInfo;
};

class MMOArenaManager : public SingletonMeyers<MMOArenaManager> , public INativeGameEventHandler
{
public:
	MMOArenaManager();
	virtual ~MMOArenaManager();

	void Init();
	void Shutdown();
	void OnNativeGameEvent( struct MMOGameEvent* evt );

public:
	void SetMyCurHighestScore( INT curScore );
	void SetMyWinLevel( INT myCurWinRank );
	void SetMyAllTimeScore( INT curScore );
	void SetMySpendForArena( UINT spendMny );
	void SetMySpendForArenaByEntCnt( UINT entCnt );
	void SetMyWinMoney( UINT32 money ){ _MyArenaData._WinMoney = money; }
	void SetMyWinItem( INT itemID ){ _MyArenaData._WinItemID = itemID; }
	const PlayerArenaData &GetMyCurArenaData(){ return _MyArenaData; }
	UINT32 GetArenaEnterCnt(){ return (UINT32)( _MyArenaData._SpendMoney / GetArenaEntryFee() ); }

public:
	UINT64 GetCurArenaMoney(){ return _CurArenaAllMoney; }
	void SetCurArenaMoney( UINT64 totalMoney );

	//
	UINT32 GetArenaEntryFee(){ return 5000; }
	INT GetArenaEntryLevel(){ return 30; }
	UINT32 GetArenaRoundDefaultTimeSec(){ return 300; }
	//
	INT GetArenaHighestScore(){ return _CurArenaHighestScore; }
	void SetArenaHighestScore( INT score ){ _CurArenaHighestScore = score; }
	//
	void ReflashArenaWinMoney();
	const ArenaWinPrize &GetArenaWinPrizeData( int idx );
	FLOAT GetWinMoneyPercentage(){ return _PrizeMoneyPercent; }
	//
	const ArenaRankPrize &GetArenaDefaultPrize( int idx );
	INT GetArenaCurPrizeRank();
	//
	void SetCurArenaState( EArenaState curState );
	EArenaState GetCurArenaState(){ return _CurArenaState; }

	//
	UBOOL IsArenaDay();
	UBOOL IsArenaApplyTime();
	UBOOL IsArenaFightingTime();
	UBOOL CanApplyArena( AMMOPlayerController *playerCtrl, FString &errorKeyStr );
	//
	void ReflashArenaTime();
	//
	void SetStartTimeByServerTime( __time32_t serverTime );
	__time32_t GetStartServerTime(){ return _ArenaStartTime; }
	_SYSTEMTIME GetStartTime();

	void SetEndTimeByServerTime( __time32_t serverTime );
	__time32_t GetEndServerTime(){ return _ArenaEndTime; }
	_SYSTEMTIME GetEndTime();

	UINT32 GetArenaLeftSecond();
	INT AreanBeginHour(){ return _ArenaBeginH; }
	INT AreanEndHour(){ return _ArenaEndH; }
	//
	void SetNewArenaRoundState( UINT32 baseUTCTime, INT modifySec );
	void SetArenaCurRoundEndTime( __time32_t utcTime );
	UINT32 GetArenaCurRoundLeftSecond();
	//
	void ClearAllCurDynamicData();
	void ClearCurArenaRoundData();
public:
	EVSSideIdx FindPlayerVSSide( INT actorID );
	INT GetCurVSArenaSessionIdx(){ return _VSSessionIdx; }
	INT GetVSSideUsedRecovery( INT sideIdx );

	VSPKInfo &GetVSSidePkInfo( INT sideIdx ){ 
		return ( ( EVSSide_1 <= sideIdx && sideIdx < EVSSide_Total ) ? _VSOneOnOnePKInfo[sideIdx] : _VSOneOnOnePKInfo[EVSSide_1] ); 
	}
	VSPKInfoPersonal &GetVSPersonalPkInfo( INT actorID, EVSSideIdx &inVSSideIdx );
public:
	void SetMyVSSide( INT sideIdx ){ _MyVSSide = sideIdx; }
	INT GetMyVSSide(){ return _MyVSSide; }
public:
	INT GetCurVSState(){ return _CurVSState; }
	INT64 GetVSOneOnOneTotalRwd(){ return _VSTotalRwd; }
	INT64 GetVSOneOnOneAvgRwd(){ return _VSAvgRwd; }
	INT GetVSOneOnOneRwdCanTryCnt(){ return ( _VSRwdCanTryTimes - _VSRwdGettedTimes ); }
	INT GetVSOneOnOneRwdTryCntDelta(){ return ( _VSGettedTryTimes ); }
private:
	VSPKInfo _VSOneOnOnePKInfo[EVSSide_Total];

	INT _VSSessionIdx;
	INT _CurVSState;

	INT _MyVSSide;

	INT64 _VSTotalRwd;
	INT64 _VSAvgRwd;
	INT _VSRwdCanTryTimes;
	INT _VSGettedTryTimes;
	INT _VSRwdGettedTimes;

private:
	PlayerArenaData _MyArenaData;

	UINT64 _CurArenaAllMoney;
	INT _CurArenaHighestScore;

	ArenaWinPrize _PrizeForWin[EArean_WinPrizeRankMax];
	//FLOAT _PrizeRankMnyPercent[EArean_WinPrizeRankMax];
	FLOAT _PrizeMoneyPercent;

	ArenaRankPrize _PrizeForRank[EArean_RankMax];

	//
	INT _ArenaBeginH;
	INT _ArenaEndH;

	__time32_t _ArenaStartTime;
	__time32_t _ArenaEndTime;
	__time32_t _ArenaCurRoundEndTime;
	EArenaState _CurArenaState;
};

#if 0
struct ArenaPlayerData
{
public:
	ArenaPlayerData()
	{
		ZeroMemory( this , sizeof(ArenaPlayerData) );
		//Reset();
	}

	INT roleID;
	FString roleName;
	INT sex;
	INT level;
	INT classId;
	INT kill;
	INT damage;
	INT currentScore;
	INT addScore;
};

enum ArenaStep
{
	ArenaStep_None,
	ArenaStep_Queuing,
	ArenaStep_WaitEnter,
	ArenaStep_Arening,
};

enum ArenaSize
{
	ArenaSize_1vs1 = 1,
	ArenaSize_2vs2 = 2,
	ArenaSize_3vs3 = 3,
	ArenaSize_5vs5 = 5,
};

typedef std::vector<ARENA_STAT_DATA, M_UE3_STL_ALLOC(ARENA_STAT_DATA)> ArenaStatusDataVec;
typedef std::vector<ARENA_TEAM_MEMBER_BASEINFO, M_UE3_STL_ALLOC(ARENA_TEAM_MEMBER_BASEINFO)> ArenaTeamMemBaseInfoVec;
typedef struct tagArenaTeamInfo
{
	UINT32 AreanTeamID;
	UINT16 IconID;
	FString AreanTeamName;
	FString Captain;
	ArenaStatusDataVec ArenaStatDataVec;
	UINT8 MemCount;
	ArenaTeamMemBaseInfoVec ArenaMemBaseInfoVec;
}ArenaTeamInfo;
typedef std::map<UINT32, ArenaTeamInfo, M_UE3_STL_ALLOC_MAP(UINT32, ArenaTeamInfo)> AllArenaTeamMap;


class MMOArenaManager : public SingletonMeyers<MMOArenaManager> , public INativeGameEventHandler
{
public:

	MMOArenaManager();
	virtual ~MMOArenaManager();

	void Init();
	void ClearArena();
	void Shutdown();


	const ArenaTeamInfo* FindAreanTeam(UINT32 Id);
	void AddOrUpdateArenaTeamInfo(M2C_ARENA_TEAM_INFO* msg);
	const AllArenaTeamMap& GetAllArenaTeamMap() const;

	INT GetMode(){ return _ArenaMode; }
	ArenaSize GetSize(){ return _ArenaSize; }

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);
	
	void PrepareArenaEnter( INT mode , INT size );
	void OpenArenaQueue( INT aveTime , UBOOL newQueue = TRUE );
	void StartArenaTick( UINT32 serveTime );
	void StartArena(  );
	void LeaveArena();
	void TickArena( FLOAT deltaTime );

	void SetSingleScoreData( UBOOL win , INT roleId , INT curScore , INT scoreGain );
	void SetTeamScoreData( UBOOL win , INT teamId , INT curTeamScore , INT scoreGain );
	void UpdateArenaSummary( G2C_SYNC_COMBAT_STAT::ROLE_STAT_DATA* data , INT count );

	ArenaPlayerData* GetWinnerPlayer(  ){ return &_Winner; };
	ArenaPlayerData* GetLoserPlayer(  ){ return &_Loser; };

	void SetArenaStep( ArenaStep step );
	ArenaStep GetArenaStep(){ return _ArenaStep; };

	INT GetQueueTime(){ return _QueueTime;}
	INT GetLastQueueTime(){ return _LastQueueTime;}
	INT GetWaitEnter(){ return _WaitEnterTime;}
	INT GetLastWaitEnter(){ return _LastWaitEnterTime; }

protected:

	ArenaStep _ArenaStep;

	ArenaSize MapArenaSize( INT size );
	INT MapArenaModeByIdx( INT index );
	ArenaPlayerData* GetArenaPlayer( INT roleId );
	void CheckAllDataUpate();

	UBOOL _StartArenaTick;
	TArray<INT> _CDHintTime;
	TArray<UBOOL> _CDHintActived;
	INT _CDMovieTime;
	UBOOL _CDMoviePlayed;
	INT _ArenaWaitTime;
	INT _ArenaFightTime;

	FLOAT _SpareTime;
	FLOAT _LastSpareTime;

	INT _ArenaMode;
	ArenaSize _ArenaSize;

	ArenaPlayerData _Winner;
	ArenaPlayerData _Loser;
	UBOOL _HasUpdateArenaSummary;
	UBOOL _HasUpdatePlayerInfo;

	AllArenaTeamMap	_AllArenaTeamMap;

	FLOAT _QueueTime;
	FLOAT _LastQueueTime;
	FLOAT _WaitEnterTime;
	FLOAT _LastWaitEnterTime;

};
#endif

#endif
