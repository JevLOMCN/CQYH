#ifndef __MMOVIPMANAGER_H__
#define __MMOVIPMANAGER_H__

enum eVipType
{
	eVIPTyp_None,
	eVIPTyp_LV1,
	eVIPTyp_LV2,
	eVIPTyp_LV3,
	eVIPTyp_LVMax
};

enum eVipShopType
{
	eVipShopType_Warehouse,
	eVipShopType_Shop,
	eVipShopType_Soul,
	eVipShopType_Inscription,
	eVipShopType_Auction,
};

enum eGameTimeHintType
{
	eGameTimeHintType_init,
	eGameTimeHintType_30,
	eGameTimeHintType_5
};

struct SVIPTimeLimit
{
	SVIPTimeLimit() : _ExpireTime(0) {}
	//UINT32 _ScenodLeft;
	//__time32_t _LocalTimeStart;
	__time32_t _ExpireTime;
};

class MMOVipManager : public SingletonMeyers<MMOVipManager>, public INativeGameEventHandler
{
public:
	MMOVipManager();
	virtual ~MMOVipManager();

	void Initialize();
	void Shutdown();
	void Clear();
	void OnNativeGameEvent(struct MMOGameEvent* evt);
public:
	void ReadVipPrivilege();

	UINT32 GetMyVipLeftSecond( INT vipType );

	//void SetMyVipLeftSecondByClientTime( INT vipType, UINT32 second );
	void SetMyVipLeftSecondByServerTime( INT vipType, __time32_t serverTime );
	//void SetMyVipLeftSecond( INT vipType, UINT32 second, __time32_t localTime );

	eVipType GetMyCurrentVipType();

	//
	const TArray<FString> &GetVipPrivilege( eVipType vipType );
	INT GetVipItemID( INT vipType );
public:
	void GetVipDescriptionString( INT vipType, FString &vipName, UINT &leftDay, UINT &leftHour );
	void ShowVipShopByType(eVipShopType type);

public:
	void SetMyLeftGameTime( UINT32 sec );
	UINT32 GetMyLeftGameTimeInSec();

	void UpdateMyLeftGameTimeUpdateTime(){ SetMyLeftGameTime( _MyLeftGameTime ); }

	void CheckGameTime();

	UBOOL IsLimitedGamePlayMode(){ return ( GetCurGameMode() != 0 ); }
	INT GetCurGameMode(){ return _MyCurGameMode; }
	void SetCurGameMode( INT modeType );

	void HintForGameLimitedMode();

	void NotifyEnterSelectRoleTime();
public:
	INT GetMyInGameDayCnt(){ return _MyInGameDayCnt; }
	void SetMyInGameDayCnt( INT dayCnt ){ _MyInGameDayCnt = dayCnt; }

	void NotifyPlayerLevelChange(INT curLevel);

private:
	SVIPTimeLimit _MyVipTime[eVIPTyp_LVMax];
	INT _VipItemID[eVIPTyp_LVMax];

	TArray<FString> _VipPrivilege[eVIPTyp_LVMax];

	INT _VipShopId;

private:
	UINT32 _MyLeftGameTimeUpdateTime;
	UINT32 _MyLeftGameTime;
	UINT32 _MySelectedRoleTotalTime;
	INT _MyCurGameMode;
	UBOOL _LastSelectedRole;
	UINT32 _LastSelectedRoleTime;
private:
	INT _MyInGameDayCnt;
};

#endif