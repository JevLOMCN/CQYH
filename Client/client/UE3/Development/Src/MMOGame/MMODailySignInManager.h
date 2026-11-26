#ifndef __MMODAILYSIGNINMANAGER_H__
#define __MMODAILYSIGNINMANAGER_H__

struct SSignInReward
{
	SSignInReward() : _ItemID(0), _ItemCnt(0), _LevelRequire(0), _DayRequire(0) {}
	INT _ItemID;
	INT _ItemCnt;
	INT _LevelRequire;
	INT _DayRequire;
};

struct SSignInRewardState
{
	SSignInRewardState() : _CurSignInDay(0), _SignInTimestamp(0), _CanSignIn(FALSE), _IsValid(FALSE) {}
	INT _CurSignInDay;
	UINT32 _SignInTimestamp;
	UBOOL _CanSignIn;
	UBOOL _IsValid;
};

class MMODailySignInManager : public SingletonMeyers<MMODailySignInManager>, public INativeGameEventHandler
{
public:
	const static INT SIGN_IN_LV_REQUIRE = 7;
	const static INT SIGN_IN_TOTAL = 28;
public:
	MMODailySignInManager();
	virtual ~MMODailySignInManager();

	void Initialize();
	void Shutdown();
	void Clear();
	void OnNativeGameEvent(struct MMOGameEvent* evt);

	void CheckHintForSignIn();
	void RemoveHintForSignInIf();
	UBOOL IsCurShowingHintForSignIn(){ return ( _HintSignInMsgButtonIdx >= 0 ); }
	void SetOpenDailySignInPanelDelegate( FScriptDelegate fun );

	void AutoRefreshSignIn();

	void ClearData();
public:
	const SSignInReward *GetSignInRewardAtDay( INT dayIdx ){ return _SignInRewardsCfg.Find( dayIdx ); }

	void SetMyCurSignInState( INT dayIdx, UBOOL isSignedIn, UBOOL isToValid = TRUE );
	const SSignInRewardState &GetMyCurSignInState() const { return _MyCurSignInState; }

private:
	UBOOL ReadSignInRewardConfig();

private:
	TMap< INT, SSignInReward > _SignInRewardsCfg;

	SSignInRewardState _MyCurSignInState;

	INT _HintSignInMsgButtonIdx;
	FScriptDelegate _OpenDailySignInPanelDelegate;

	UBOOL _IsLastInRoleSelecting;

	INT _RequipedSignInRoleLevel;
};

#endif