
#ifndef _FriendManager_H_
#define _FriendManager_H_

#include "MMOSingleton.h"
#include "MMOGameUIClasses.h"

struct MMOFriendMember
{
	MMOFriendMember(EEventParm)
	{
		appMemzero(this, sizeof(MMOFriendMember));
	}
	MMOFriendMember()
	{
		//appMemzero(this, sizeof(MMOFriendMember));
		RoleId = 0;
		Lv = 0;
		ClassId = 0;
		Sex = 0;
		Online = 0;
		nIntimacy = 0;
		SceneTempID = -1;
	}
	INT RoleId;
	INT Lv;
	FString ActorName;
	INT ClassId;	
	INT Sex;
	INT Online;
	FString Location;
	FString Sign;
	FString AbbName;//abbreviated name
	INT32 nIntimacy; //friendship level
	UINT8 SceneTempID;
};

struct MMOFriendGroup
{
	TArray<MMOFriendMember> FriendList;
	FString GroupName;
	UBOOL IsDefault;
};

struct MMOKillInfo
{
	INT Time;
	INT RoleId;
	UBOOL Killed;
	FString ShowText;
};

#define FriendGroupNum 10
#define AttentionGroupId 10
#define DefaultFriendGroupId 0
#define BlackGroupId 2000
#define EnemyGroupId 3000
#define NearGroupId 1000

class FriendManager : public SingletonMeyers<FriendManager>, public INativeGameEventHandler
{
public:

	FriendManager();
	virtual ~FriendManager(){Shutdown();}

	void Init();
	void Shutdown();

	
	
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);
	TArray<MMOFriendMember> EnemyList;
	
	TArray<MMOFriendMember> NearContactList;

	void ClearAll();

	INT GetFriendGroupId ( INT roleId );
	MMOFriendMember GetFriendInfoByRoleId( INT roleId );	
	MMOFriendMember* GetFriendGroupMember( INT groupId , INT pos );//use a pointer to avoid not-found
	MMOFriendMember* GetFriendGroupMemberById( INT groupId , INT id );//use a pointer to avoid not-found
	MMOFriendMember* GetNearContact( INT pos );//use a pointer to avoid not-found
	MMOFriendMember* GetBlacklistMember( INT pos );
	MMOFriendMember* GetEnemylistMember( INT pos );
	MMOFriendMember* GetBlackMemberByRoleId( INT roleId );
	MMOFriendMember* GetEnemyMemberByRoleId( INT roleId );
	MMOFriendMember* GetNearMemberByRoleId( INT roleId );
	
	MMOFriendGroup* GetFriendGroup( INT groupId );
	INT GetFriendGroupNum(){ return FriendGroupList.Num(); }
	INT GetFriendNumInGroup( INT groupId );//number of friends in a group
	INT GetTotalFriendNum( );//total number of friends
	INT GetOnlineFriendNumInGroup( INT groupId );//number of online friends in a group
	INT GetEmptyFriendGroupId();

	void ChangeFriendGroup(INT RoleId,INT GroupId);
	void RequestAddAttention( INT RoleId , const FString& TargetName );
	void RequestDelAttention( INT RoleId );

	void AddNewFriend( INT roleId , FString friendName , INT groupId );
	void PushToFriendGroup( INT groupId , MMOFriendMember member );
	void AddNewEnemy(MMOFriendMember& member);

	UBOOL IsMyFriend( INT roleId );
	UBOOL IsMyAttention( INT roleId );
	UBOOL InMyBlackList( INT roleId );
	UBOOL InMyEnemyList( INT roleId );

	MMOFriendMember* GetAttentionInfo(INT roleId);
	MMOFriendMember* GetMyFriendInfo(INT roleId);
	
	void NotifyNewContact( INT roleId );
	
	void MoveGroupPlayerToGroup( INT fromGroupId  , INT toGroupId );//change the group of a group of people
	void MovePlayerToGroup( INT roleId , INT fromGroupId  , INT toGroupId ); //change group
	void DeleteAttention( INT roleId ); //unfollow

	void SaveNearContact();
	void LoadNearContact();

	void RemoveEnemy( INT roleId );
	MMOFriendMember* GetSearchList(){ return SearchList; }
	MMOFriendMember* GetSearchFriend( INT pos );
	void ClearSearchList();

	void AddedAttentionBy(INT RoleId,const FString& RoleName);
	void AddFriendOk( UGFxObject* mc );

	void AddBlackList( MMOFriendMember member );
	INT GetBlackNum();
	TArray<MMOFriendMember>& GetBlackList();

	void SortFriendGroup( INT groupId );
	void SortAllFriend();

	void DeleteFriendGroup( INT groupId );

	//add kill info
	void AddKillInfoList( UBOOL iamKiller , INT killerRoleID , INT victimRoleID );
	//add drop info
	void AddKilledDropItem( UItem* titem , FClientItemInfo* clientItem );
	TArray<MMOKillInfo>& GetKillInfoList() { return KillInfoList; }
	void SaveKillInfoData();
	void LoadKillInfoData();

	void TickTime( FLOAT deltaTime );

	//stall requirement
	void  UpdateFriendMemberInfo(MMOFriendMember &FriendInfo,INT nRoleID);

	//mall needs an interface for all friend data
	const TArray<MMOFriendGroup>& GetAllFriendList()const;

	//delete from recent contacts
	void removeFromNerContactList(int roleId);
	void UpdateNerContactListMember(MMOFriendMember* mem);

	//friendship level changed
	void UpdateFriendIntimacy(UINT32 roleId,INT32 nIntimacy);

	INT GetPlayerIntimacy(UINT32 roldId);
	INT GetNeedIntimacyValue(){return nTradeNeedIntimacyValue;}
	BOOL CheckIntimacy(UINT32 roleId);

protected:

	TArray<MMOFriendGroup> FriendGroupList;
	TArray<MMOFriendMember> BlackList;

	MMOFriendMember SearchList[UCONST_MaxSearchPanelItem];
	TArray<MMOKillInfo> KillInfoList;

	TArray<MMOKillInfo> KillInfoListBuff;

	FString _GetCurTimeString();
	void _GetCurMapString( FString& mapString , FString& mapIcon );
	
	UBOOL startTick;
	FLOAT startTime;

	FString killTime;

	INT nTradeNeedIntimacyValue;
};



#endif