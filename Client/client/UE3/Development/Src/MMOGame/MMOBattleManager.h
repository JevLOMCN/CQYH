#ifndef _UBATTLEMANAGER_H_
#define _UBATTLEMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOGameUIClasses.h"

namespace ExternalMessage
{
    struct G2C_SYNC_COMBAT_STAT;
}

#define RANDOM_BATTLE_ID 0
static UINT MAX_SEND_QUERY_PLAYER_INFO_CNT = 15;
enum eBattlePlayerState
{
	EBPS_OFFLINE,
	EBPS_ALIVE,
	EBPS_DEAD,
};

enum eBattleState
{
	EBTLS_NONE,
	EBTLS_QUEUEING,
	EBTLS_INQUIRING_ENTER,
	EBTLS_BEGINING,
	EBTLS_FIGHTING,
	EBTLS_ENDING
};

//Battlefield personal score
struct FBattlePlayerScore
{	//Score earned in the personal battlefield, meaning varies by battlefield
	INT	_Sum;

	INT _Kills;
	INT _BeKilled;

	INT _Damages;
	INT _Heals;
};

//Battlefield personal reward
struct FBattlePlayerAwards
{
	INT _Points;
};

//Battlefield players
struct FBattlePlayer
{
	INT			_ID;

	INT			_Sex;
	INT			_ClassID;
	INT			_Lv;

	FBattlePlayerScore	_Score;
	FBattlePlayerAwards _Awards;

	FString		_ServerName;
	FString		_PlayerName;

	eBattlePlayerState	_State;
	bool				_IsLeader;
	//0 is own side
	BYTE				_BtlTeamID;
};

//class MMOBattleManager : public Singleton<MMOBattleManager>
class MMOBattleManager : public SingletonMeyers<MMOBattleManager>, public INativeGameEventHandler
{
public:
	MMOBattleManager(void);
	virtual ~MMOBattleManager(void);

	//void Init( UMMOBattleTemplate* pBattle );
	void Init();
	void Shutdown();
	void ResetRandomBattleData();
	void TestAddData();

public:
	typedef TMap< INT, FBattlePlayer > BattlePlayerMap;
	typedef BattlePlayerMap::TIterator BattlePlayerIterator;

public:
	void RandomJoinBattle();
	//void TeamJoinBattle( INT battleID );
	//void SingleJoinBattle( INT battleID );
	void JoinSingleBattle( INT battleID );
	//void QuickJoinBattle( UINT battleID );
	void LeaveWaitingBattle();
	void ConfirmJoinBattle( UBOOL isAgree );

public:
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);
	void NotifyPlayerBaseInfo( UINT32 roleID );
	void NotifyJoinedBattle();
	void NotifyLeaveBattle();
	void NotifyEnterBattle( INT battleID );
	void NotifyBattleEnd( UBOOL isDogFall, INT winTeamID );
	void ParseBattleCombatDatas( ExternalMessage::G2C_SYNC_COMBAT_STAT* serverMsg, UINT dataCnt );

public:
	void ResetJoinedBattleData();
	void ResetEnteredBattle();

	void ClearBattlePlayer(){ _BattlePlayerList.Empty(); }
	void DelPlayer( INT playerID );
	FBattlePlayer *GetOrAddPlayer( INT playerID ){
		return &_BattlePlayerList.FindOrAdd( playerID );
	}
	const FBattlePlayer *GetPlayer( INT playerID );
	BattlePlayerIterator BeginIterateBattlePlayer();

	void SortedPlayersByPointsInTeam();
	void SortedPlayersByScore();
	void SortedPlayersByKills();
	void SortedPlayersByBeKilled();

	TMap< UINT, UBOOL > &GetRoleIncompleteList(){
		return _RoleInfoIncompleteList;
	}
	//Battlefield info
	//void AddBattleTemplate( FBattleTemplate* battle );
	//void ClearBattleTemplates();
	INT GetBattleTemplateCnt();
	FBattleTemplate* FindBattleTemplateBy( INT battleID );
	FBattleTemplate* GetBattleTemplateIn( INT idx );
	INT GetBattleIdxByID( INT battleID );
	//FBattleTemplate* CurBattleTemplate( INT battleID );

	FBattleTemplate* GetRandomBattle(){ return &_BattlesInfo->RandomBattle; }
	INT GetRandomCur(){ return _RandomBattlesList.Num(); }
	INT GetRandomMin(){ return _BattlesInfo->RandomBattleMinSlt; }
	INT GetRandomMax(){ return _BattlesInfo->RandomBattleMaxSlt; }
	INT GetRandomMinLv(){ return _BattlesInfo->RandomBattleMinLv; }

	eBattlePlayerState CvtToBattleState( INT iState );
	
	INT GetCurBattleSidesCnt();
	//Battlefield state
	eBattleState GetCurBattleState(){ return _BattleState; }
	UBOOL IsWaiting(){ return (_BattleState == EBTLS_QUEUEING) ? TRUE : FALSE; }
	void SetWaiting( UBOOL isW ){ _BattleState = ( (isW == TRUE) ? EBTLS_QUEUEING : _BattleState ); }
	UBOOL IsInBattle();
	UBOOL IsBattleEnding(){ return (_BattleState == EBTLS_ENDING) ? TRUE : FALSE; }
	void SetBattleState( eBattleState state ){ _BattleState = state; }
	//Battlefield score
	void SetBattleTeamScore( INT btlTeamID, INT score ){
			_BattleTeamScore.FindOrAdd( btlTeamID ) = score;
	}
	INT GetBattleTeamScore( INT btlTeamID ){
		if( _BattleTeamScore.Find( btlTeamID ) != NULL )
			return _BattleTeamScore.FindOrAdd( btlTeamID );
		return -1;
	}
	//
	const FBattlePlayer *GetMyBattleScoreInfo();
	//
	INT GetWinTeamID();

	//Battlefield currently being joined
	INT GetJoinedBattleID(){ return _JoinedBattleID; }
	void SetJoinedBattleID( INT id ){ _JoinedBattleID = id; }
	//Battlefield already entered
	FExportMap *GetEnteredBattleMapData();
	FBattleTemplate* GetEnteredBattle();
	void SetEnteredBattleID( INT id ){ _EnteredBattleID = id; }

	void ToggleRandomBattles( INT id, UBOOL isChoosed );
	//Join time
	_SYSTEMTIME GetJoinedBattleTime(){
		return _JoinLocalTime;
	}
	//Start time
	INT GetTimeSinceBattleBegin();
	UINT32 GetBattleStartTime(){ return _BattleStartUtcTime; }
	void SetBattleStartTime( UINT32 startTime, int modifySec ){ _BattleStartUtcTime = startTime + modifySec; }
	//
	UBOOL IsBattleBegin(){ return _IsBattleBegin; }
	void SetBattleBegin( UBOOL is ){ _IsBattleBegin = is; }
private:
	//IDs of players whose info is incomplete
	TMap< UINT, UBOOL > _RoleInfoIncompleteList;
	TMap< UINT, UBOOL > _RoleInfoIncompleteListUnSended;
	//Battlefield members
	//std::map< INT, FBattlePlayer > BattlePlayerList;
	BattlePlayerMap _BattlePlayerList;

	//Battlefield info
	UMMOBattleTemplate *_BattlesInfo;
	//Usable battlefields
	TArray< UINT > _ActivedBattles;

	//Battlefield ID applied to join, 0 means random battlefield when applying
	INT _PrepareJoinedBtlID;
	//List of all random battlefields
	typedef TMap< INT, INT > RandomBattleMap;
	RandomBattleMap  _RandomBattlesList;
	//Current battlefield ID (applied)
	INT _JoinedBattleID;
	//Current battlefield ID (already in)
	INT _EnteredBattleID;
	//Whether waiting to join a battlefield
	eBattleState _BattleState;
	//Time of joining the battlefield queue
	_SYSTEMTIME _JoinLocalTime;
	//Battlefield start time
	UINT32 _BattleStartUtcTime;
	UBOOL _IsBattleBegin;
	//Battlefield score statistics
	//static const INT MAX_BATTLE_TEAM = 3;
	TMap<INT,INT> _BattleTeamScore;
	//Winning team
	INT _WinTeamID;
	//Battlefield result
	UBOOL _IsDogfall;
};


#endif