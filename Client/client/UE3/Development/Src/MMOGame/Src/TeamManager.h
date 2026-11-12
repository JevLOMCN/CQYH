
#ifndef _TeamManager_H_
#define _TeamManager_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include <queue>

#define MAX_ARCHIVE_INVITE_PLAYER 10

struct MMOTeamMember
{
	MMOTeamMember()
	{
		appMemzero( this , sizeof(MMOTeamMember) );
	}
	INT ActorServerId;
	INT Lv;
	FString ActorName;
	INT ClassId;
	UBOOL isLeader;
	FLOAT HPPecent;
	FLOAT MPPecent;
	INT Sex;
	UBOOL Online;
	INT UIPos;
	INT HP; // needed to determine whether dead
	FVector WorldPos;
	INT MapID;
	INT MapLineID;
};

struct MMONearbyPlayer
{
	INT ActorServerId;
	INT Lv;
	FString ActorName;
	INT ClassId;
	INT Sex;
	UBOOL InTeam;
};

enum ETeamAuctionItemState
{
	ETAIS_NoPermit, //no permission
	ETAIS_CanBid,
	ETAIS_GiveUp,
	ETAIS_Over,

};

struct MMOTeamAuctionItem
{
	ItemInfo Item;
	ETeamAuctionItemState BidState;
	INT RewardMemberCount;
	INT Index;
};

//Invited player records, stored locally
struct MMOInvitedPlayer
{
	INT ActorServerId;
	INT Job;
	INT Level;
	FString ActorName;

	MMOInvitedPlayer()
	{
		appMemzero(this, sizeof(MMOInvitedPlayer));
	}
};

struct MMOTeamInfo
{
	INT MemberCount;
	INT MaxMemberCount;
	INT LeadLv;
	FString TeamName;
	INT TeamID;
	FString TeamLeaderName;
	INT TeamLeaderServeId;
	FString Targe;
};

#define NormalTeamNum 5
#define LargeTeamNum 11

//struct MMOGameEvent_BattleTeam : public MMOGameEvent
//{
//public:
//	enum EBattleTeamEventType
//	{
//		EBET_UpdateMemberList,
//		EBET_InviteEnterTeam,
//	};
//
//	MMOGameEvent_BattleTeam(EBattleTeamEventType evtType)
//		: EvtType(evtType)
//		, roleID(0)
//		, roleName(TEXT(""))
//	{
//	}
//
//	virtual EGameEvent GetEventType() const
//	{
//		return GameEvent_BattleTeam;
//	}
//
//	EBattleTeamEventType EvtType;
//
//	INT roleID;
//	FString roleName;
//};
typedef std::queue<INT> IntQueue;
struct TeamAuctionItemColorLimitMap
{
	INT Color;
	FString ColorStringText;
};
typedef TArray< TeamAuctionItemColorLimitMap > TeamAuctionItemColorLimitList;

struct MMOGameEvent_TeamCommon : public MMOGameEvent
{
public:
	enum ETeamEventType
	{
		ETeamEventType_TargetChange,
		ETeamEventType_LeaveTeam,
		ETeamEventType_MoveLeader,
		ETeamEventType_AddEnemy,
		ETeamEventType_RemoveEnemy,
	};

	MMOGameEvent_TeamCommon(ETeamEventType eventType,INT nServerID = -1)
		: EventType(eventType),serverID(nServerID)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_TeamCommon;
	}

	ETeamEventType  EventType;
	INT  serverID;
};

struct MMOGameEvent_TeamAuctionOtherBid : public MMOGameEvent_Any
{
	MMOGameEvent_TeamAuctionOtherBid( INT roleId , INT bidMoney )
		: MMOGameEvent_Any(GameEvent_TeamAuctionOtherBid)
		, RoleId(roleId)
		, BidMoney(bidMoney)
	{
		
	}
	
	INT BidMoney;
	INT RoleId;
};

struct MMOGameEvent_TeamExpand : public MMOGameEvent_Any
{
	MMOGameEvent_TeamExpand( BOOL v )
		: MMOGameEvent_Any(GameEvent_TeamExpand)
		, show(v)
	{

	}

	BOOL show;
};

class TeamManager: public SingletonMeyers<TeamManager>, public INativeGameEventHandler
{
public:

	TeamManager();
	virtual ~TeamManager();

	UBOOL hasTeam;
	INT TeamLeaderServeId; //use the set function to assign this, it has processing
	UBOOL ImTeamLeader;
	UINT32 TeamID;
	INT AssignType;
	INT MemberLimit;
	UBOOL InLargeTeam;
	//FString TeamName;

	void SendOperateTeam(INT AssignType,INT TeamMemberLimit,const FString& TeamName , INT auctionItemColorLimit );

	void Init( );
	void Shutdown(){};

	void ClearTeamInfo();

	void UninitTeamManager();

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void AddTeamMember( MMOTeamMember member );
	void RemoveTeamMember( INT roleId );

	void AddInvitedPlayer(INT roleId);
	void RemoveInvitedPlayer(INT roleId);
	void ClearInvitePlayers();
	void InvitePlayers();

	void CreateMyTeam( UINT32 nTeamId  );
	void LeaveMyTeam();
	void LeaveTeam( INT roleId );
	void MoveLeader( INT roleId );
	void SendTeamInviteAnswer(INT RoleId,UBOOL agree);
	void SendTeamAddAnswer(INT RoleId,UBOOL agree);
	INT GetMemberPosByMemberId(INT MemberId);
	void SetTeamLeaderRoleId( INT RoleId );
	void GetNearbyPlayerList();

	INT GetMemberPosByUIPos( INT uiPos );
	UINT32 GetMyTeamID(){ return TeamID; }
	INT GetTeamMemberCount(){ return TeamMemberList.Num(); }
	INT GetNearbyPlayerListCount(){ return NearbyPlayerList.Num(); }
	INT GetNearbyTeamCount(){ return TeamInfoList.Num(); }

	void LoadInvitePlayerBinary();
	void SaveInvitePlayerBinary();
	UBOOL CheckRepeatInvitePlayer(INT serverId);
	TArray<MMOInvitedPlayer>& GetInvitePlayerListHistory() { return InvitedPlayerCache; }

	TArray<MMONearbyPlayer>& GetNearPlayerList(){ return NearbyPlayerList; }
	TArray<MMOTeamInfo>& GetTeamInfoList(){ return TeamInfoList; }
	TArray<MMOTeamMember>& GetTeamMemberList(){ return TeamMemberList; }

	UBOOL InMyTeam( INT roleId );
	UBOOL HasTeam(){ return hasTeam; }
	UBOOL IsTeamLeader(){ return ImTeamLeader; }

	const FString& GetTeamName(){ return _myTeamName; }
	void SetTeamName(const FString& teamName );
	INT GetAssignType(){ return AssignType; }
	INT GetMemberLimit(){ return MemberLimit; }
	INT GetTeamLeaderServeId(){ return TeamLeaderServeId; }

	void InviteTeamOk( UGFxObject* mc );
	void InviteTeamNo( UGFxObject* mc );
	void AddTeamOk( UGFxObject* mc );
	void AddTeamNo( UGFxObject* mc );

	void RefuseTeamAdd( INT roleId0 , INT roldId1 );

	MMOTeamMember* GetTeamMember( INT index );
	MMOTeamMember* GetMemberById( INT roleID );
	MMONearbyPlayer* GetNearPlayer( INT index );
	MMOTeamInfo* GetNearTeam( INT index );

	UBOOL getAutoTeam() { return _autoTeam; }
	void setAutoTeam( UBOOL autoTeam ){ _autoTeam = autoTeam; }

	void InviteTeamRequest(const FString& RoleName,INT RoleId);
	void AddTeamRequest(const FString& RoleName,INT RoleId);

	void SetTeamTargetType(UINT32 nType) { _teamTargetType = nType; }
	UINT32 GetTeamTargetType() { return _teamTargetType; }

	void SetTeamTargetId(INT nId) { _teamTargetId = nId; }
	INT	 GetTeamTargetId() { return _teamTargetId; }

	//team auction
	void ClearTeamAuctionItemList();
	void AddTeamAuctionItem( ItemInfo item , ETeamAuctionItemState bidState , INT index ,INT rewardMemberCount =0 );
	TArray<MMOTeamAuctionItem>& GetTeamAuctionItemList() { return TeamAuctionItemList; }
	void StartTeamAuctionItem( UINT32 itemIndex , UINT32 bidMoney,UINT32 bidCount, UBOOL canBid = TRUE );
	void OtherBid( INT roleId , INT itemIndex , INT bidMoney );
	void EndTeamAuctionItem( INT itemIndex , INT roleId );
	INT	GetCanBidAuctionItemNum();
	MMOTeamAuctionItem* GetCurAuctionItem(){ return _CurAuctionItem; }
	void RemoveCurrentAuctionItem(INT idx);
	void SetAuctionItemColorLimit( INT color ){ _AuctionItemColorLimit = color; }
	INT GetAuctionItemColorLimit(){ return _AuctionItemColorLimit; }
	INT GetCurBidMoney(){ return _CurBidMoney; }
	//Bid increment amount
	INT GetAddBidMoney(){ return _CurBidMoney*_AddBidRate/100+1 ; }
	//Minimum bid value
	INT GetMinBidMoney();
	void RequestGiveUp( INT itemTempId , INT itemIndex );
	void RequestGiveUp( INT itemIndex );
	void RequestGiveUpCurrentItem( );
	void RequestGiveUpAll();
	void NotifyGiveUpBid( INT itemIndex/* , INT itemTempId*/ );
	INT GetTeamAuctionItemColorIndex( INT colorLimit );
	INT GetCurTeamAuctionItemColorIndex();
	INT GetTeamAuctionItemColorByIdx( INT idx );
	TeamAuctionItemColorLimitList& GetLimitColorMap(){ return _ItemAuctionLimitList; }

	void NotifyQuickTeam(BOOL join);

	BOOL GetIsExpand(){return isExpand;}
	void SetExpand(BOOL v){isExpand = v;}

	void TeamExpand();

	void SelectedNearTeamMate(INT nIndex,BOOL teamMate = TRUE);

	INT GetTeamAuctionItemIdx(INT idx);

	UGFxObject* CreateTeamMate(MMOTeamMember* member,UMMOGFXPlayer* gfxPlayer);

	MMOTeamMember* GetTeamLeader();

	void NotifyCreateTeam();
	void NotifyEnterTeam();

	void SetTeamTargetRoldId(UINT32 id){targetRoleId = id;}
	UINT32 GetTeamTargetRoldId(){return targetRoleId;}

	BOOL GetTargetRoleTeam(){return bTargetRoleTeam;}
	void SetTargetRoldTeam(BOOL v){bTargetRoleTeam = v;}

	void ResetTeamTarget();

	void QueryRoleHasTeam(INT roleId);
	UBOOL CheckBeTeamQualification(INT roleId, FString &errMsgName);

	BOOL GetTeamAnswerHasTeam(){return bTeamAnswerHasTeam;}
	void SetTeamAnswerHasTeam(BOOL v){bTeamAnswerHasTeam = v;}

protected:
	TArray<MMOTeamMember> TeamMemberList;
	TArray<MMOTeamInfo> TeamInfoList;
	TArray<MMONearbyPlayer> NearbyPlayerList;
	TArray<MMOInvitedPlayer> InvitedPlayerCache;
	IntQueue	WaitingPlayerInfoQueue;

	TArray<INT> UnknownRefuseList;
	TArray<INT>	InvitePlayerList;

	UMMOUITeam* _team;
	UBOOL	_autoTeam;
	FString	_myTeamName;

	UINT32 _teamTargetType;
	INT _teamTargetId;
	INT _AuctionItemColorLimit;

	TArray<MMOTeamAuctionItem> TeamAuctionItemList;

	MMOTeamAuctionItem* _CurAuctionItem;
	INT64 _EndAuctionItemllGuid;

	INT	_CurBidMoney;
	INT _AddBidRate;
	INT _CurBidRoleId;
	TeamAuctionItemColorLimitList _ItemAuctionLimitList;

	BOOL isExpand;

	UINT32 targetRoleId;
	BOOL bTargetRoleTeam;

	BOOL bTeamAnswerHasTeam;
};

#endif