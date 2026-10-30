#ifndef _AI_PROCESSOR_H_
#define _AI_PROCESSOR_H_

#include "math_ex.h"
#include "bt_action.h"
#include "bt_manager.h"
#include "variable_def.h"
#include "region.h"

#ifndef _ROBOT
	#define MAX_WAYPOINT_COUNT	    (16)
#else //_ROBOT
	#define MAX_WAYPOINT_COUNT	    (64)
#endif //_ROBOT

#define MAX_WANDER_POINT		(4)
class CCharacter;
struct stSignal;
class CRegion;
class CLuaScript;
class CRole;
class CAITemplate;

enum AI_MOVE_STATE
{
	amsInvalid = 0,

	amsStop,
	amsWander,
	amsChase,
    amsWithdraw,
	amsFollow,
	amsFlee,
    amsGoHome,
    amsPatrol,

	amsTotal
};

enum AI_FIGHT_STATE
{
	afsInvalid = 0,

	afsIdle,
	afsFight,
    afsGoHome,
	afsDying,

	afsTotal
};

class CAIProcessor
{
public:
	CAIProcessor();
	~CAIProcessor();

	BOOL Init(CCharacter* poOwner, INT32 nAIID);
	BOOL UnInit(void);

	BOOL MainLoop(void);

    inline INT32 GetMoveState(void);
	inline INT32 GetFightState(void);
	inline INT32 GetBossState(void);
	inline void SetBossState(INT32 nState);
	inline INT32 GetAIID(void);
	inline CCharacter * GetOwner(void);

    inline UINT32 GetInteractRoleID();

    inline BTCustomCtx& GetBTCustomCtx();
    inline BTCustomCtx& GetBTDieCtx();

	inline void SetVariable(INT32 nIndex, INT32 nValue);
	inline INT32 GetVariable(INT32 nIndex);

    inline void SetTargetID(UINT32 dwTargetID);
	inline UINT32 GetTargetID(void);

	inline void SetIgnoreInvisible(BOOL bValue);
	inline BOOL IgnoreInvisible(void);

	inline void SetCounterInvisible(BOOL bValue);
	inline BOOL CounterInvisible(void);

    inline UINT32 GetNearestEnemyID(void);
    inline UINT32 GetFarthestEnemyID(void);
    inline UINT32 GetCastingEnemyID(void);
    inline UINT32 GetNearestFriendID(void);
    inline UINT32 GetCorpseCharID(void);
    inline UINT32 GetAuraCharID(void);
	inline UINT32 GetNearestEnemyRoleID(void);

    inline INT32 GetHostOldDirection(void);

    // move state
	BOOL Stop(void);
	BOOL MoveTo(INT64 llDestX, INT64 llDestY);
	BOOL SyncMoveTo(INT64 llDestX, INT64 llDestY);

	BOOL Wander(INT32 nWanderRange, INT32 nWanderInterval);
    BOOL Patrol(void);
	BOOL Chase(INT32 nPreferDistance, INT32 nChaseThreshold, INT32 nDelayFrame);
    BOOL Withdraw(INT32 nPreferDistance, INT32 nWithdrawThreshold);
    BOOL StepAround(CCharacter* poTarget, INT32 nPreferDistance, INT32 nChaseThreshold);
	BOOL Follow(void);
    BOOL PetFollow(void);
	BOOL Flee(INT32 nFleeTime);
    BOOL NeedChase(CCharacter* poTarget, INT32 nChaseThreshold);
    BOOL NeedWithdraw(CCharacter* poTarget, INT32 nWithdrawThreshold);

    BOOL JumpToHost(void);

    inline void SetPatrolParam(INT32 nPathID, INT32 nPatrolStyle);
    inline void SetFollowParam(UINT32 dwFollowHostID, INT32 nFollowOffsetX, INT32 nFollowOffsetY, BOOL bUseHostSpeed);

	// fight state
	BOOL EnterIdle(void);
	BOOL EnterFight(void);
    BOOL GoHome(void);

    BOOL SetScanEnemyParam(INT32 nScanEnemyRange);
    BOOL SetScanFriendParam(INT32 nScanFriendRange, INT32 nFriendTempalteID);
    BOOL SetScanCorpseParam(INT32 nCorpseTemplateID, BOOL bFriendCorpse);
    BOOL SetScanAuraParam(INT32 nAuraID);

    BOOL ScanEnemy(void);
    BOOL ScanNearby(void);
	BOOL ScanRedName(BOOL bOnlyRedName);
	BOOL ScanSabacWinner(void);
	BOOL ScanSabacEnemy(void);
    BOOL PetScanEnemy(INT32 nLevel, INT64 llDistance, INT32 nspellEffectGroup);
    BOOL PetWander(INT32 nWanderRange, INT32 nWanderInterval);

    stSignal* PickSignal(INT32 nScanRange, INT32 nSignalType);
    BOOL IssueSignal(INT32 nSignalType, INT32 nSignalLivingFrame, INT32 nSignalProcessTimes, INT32 nSignalParam );
    void DecreaseSignalNum(void);
    inline UINT32 GetSignalIssuerID(void);

	BOOL Reload(void);
    void CallInitBT(void);
	BOOL CallDeadBT(UINT32 dwKillerRoleID);
    void CallReviveBT(void);
	BTNod* GetInteractBT(void);
    BOOL ProcessInteractSyncRun(BTCustomCtx& rCtx);
    void SetInteractState(BOOL bEnabled);

    void PrintAllBT(void);
	inline void ClearWanderPoint(void);

#ifdef _ROBOT
	BOOL RegisterMsg(INT32 nMsgID);
	void CheckMsgID(INT32 nMsgID);
#endif	// _ROBOT

private:
	INT32 _ProcessSyncRun(BTCustomCtx& rCtx);

	BOOL _MoveTo(INT64 llDestX, INT64 llDestY, INT64 llDestZ);
	BOOL _MoveToTarget(CCharacter* poTarget, INT32 nMinDistance, INT32 nMaxDistance);
	BOOL _ProcessMove(void);

	BOOL _ProcessFightState(void);
	BOOL _OnIdle(void);
	BOOL _OnFight(void);
	BOOL _OnGoHome(void);
	BOOL _OnFightFlee(void);
	BOOL _OnDying(void);

	BTNod* _GetExecBT(INT32 nEventType);
	inline BOOL _RunBT(INT32 nEventType, UINT32 dwRoleID, BOOL bForce);

	template<class TFunc>
	inline BOOL _Scan9Region(CRegion* poCenterRegion, TFunc& rFunc);

	template<class TFunc>
	inline BOOL _ScanRegion(CRegion* poRegion, TFunc& rFunc);

    struct CScanEnemy
    {
        CAIProcessor*   m_poAIProcessor;
        INT64           m_llNearestEnemyDistance;
        BOOL operator()(CCharacter* poChar);
    };

	struct CScanNearby
	{
		CAIProcessor*   m_poAIProcessor;
        INT64           m_llScanEnemyRange;
        INT64           m_llScanFriendRange;
        INT32           m_nAuraID; 
        INT32           m_nCorpseTemplateID;
        BOOL            m_bFriendCorpse;
        INT32           m_nFriendTempalteID;

        INT64           m_llNearestEnemyDistance;
        INT64           m_llFarthestEnemyDistance;
        INT64           m_llNearestFriendDistance;
        INT64           m_llNearestEnemyRoleDistance;

        BOOL operator()(CCharacter* poChar);
	}                   m_ScanNearby;

	struct CScanRedName
	{
		CAIProcessor*   m_poAIProcessor;
		INT64           m_llNearestEnemyDistance;
		BOOL			m_bOnlyRedName;
		BOOL operator()(CCharacter* poChar);
	};

	struct CScanSabacWinner
	{
		CAIProcessor*   m_poAIProcessor;
		INT64           m_llNearestFriendDistance;
		BOOL operator()(CCharacter* poChar);
	};

	struct CScanSabacEnemy
	{
		CAIProcessor*   m_poAIProcessor;
		INT64           m_llNearestEnemyDistance;
		BOOL operator()(CCharacter* poChar);
	};

    struct CScanSignal
    {
        stSignal*       pFoundSignal;
        CCharacter*     pSignalChecker;
        INT32           nFindSignalType;
        INT64           llFindDistanceSquare;
        BOOL operator()(stSignal* pSignal);
    };

    struct CPetScanEnemy 
    {
        CAIProcessor*   m_poAIProcessor;
        INT64           m_llScanEnemyRange;
        INT32           m_nLimitLevel;
        INT64           m_llNearestEnemyDistance;
        UINT32          m_dwTargetID;
        INT32           m_nspellEffectGroup;

        BOOL operator()(CCharacter* poChar);
    };

private:
	CCharacter*			m_poOwner;
	INT32				m_nAIID;
	INT32				m_nAIFightState;
    UINT32              m_dwTargetID;

    UINT32              m_dwNearestEnemyID;
    UINT32              m_dwFarthestEnemyID;
    UINT32              m_dwCastingEnemyID;
    UINT32              m_dwNearestFriendID;
    UINT32              m_dwCorpseCharID;
    UINT32              m_dwAuraCharID;
	UINT32				m_dwNearestEnemyRoleID;

    INT32               m_nSignalType;
    UINT32              m_dwSignalIssuerID;
    INT32               m_nIssuedSignalNum;

	INT32				m_nAIMoveState;
	INT32				m_nMoveTimer;
    INT32               m_nInteractTimer;
	BOOL				m_bIgnoreInvisible;
	BOOL				m_bCounterInvisible;

	BTCustomCtx			m_BTCtx;
	BTCustomCtx			m_BTOnDieCtx;
	INT32				m_nAIBossState;
	INT32				m_nLatestHp;

    union
    {
        struct 
        {
            INT32               m_nPatrolPathID;
            INT32               m_nPatrolStyle;
            INT32               m_nPatrolIndex;
        };
        struct  
        {
            UINT32              m_dwFollowHostID;
            INT32               m_nFollowOffsetX;
            INT32               m_nFollowOffsetY;
            BOOL                m_bUseHostSpeed;
        };
    };

	INT32				m_nWaypointCount;
	INT32				m_nWaypointList[MAX_WAYPOINT_COUNT][3];
	INT32				m_nWaypointIndex;

	INT32				m_nWanderPointCount;
	INT32				m_nWanderPointList[MAX_WANDER_POINT][3];

	INT64				m_llTargetX;
	INT64				m_llTargetY;
    INT32               m_nTargetDirection;

	// AI Variables.
	INT32				m_nVarList[MAX_AI_VARIABLE_COUNT];
#ifdef _ROBOT
	INT32				m_nRegisteredMsgID;
#endif	// _ROBOT
};

inline UINT32 CAIProcessor::GetInteractRoleID()
{
    return m_BTCtx.dwRoleID;
}

inline INT32 CAIProcessor::GetMoveState(void)
{
    return m_nAIMoveState;
}

inline INT32 CAIProcessor::GetFightState(void)
{
	return m_nAIFightState;
}

inline INT32 CAIProcessor::GetBossState(void)
{
	return m_nAIBossState;
}

inline void CAIProcessor::SetBossState(INT32 nState)
{
	m_nAIBossState = nState;
}

inline INT32 CAIProcessor::GetAIID(void)
{
	return m_nAIID;
}

inline CCharacter* CAIProcessor::GetOwner(void)
{
	return m_poOwner;
}

inline BTCustomCtx& CAIProcessor::GetBTCustomCtx()
{
    return m_BTCtx;
}

inline BTCustomCtx& CAIProcessor::GetBTDieCtx()
{
    return m_BTOnDieCtx;
}

inline void CAIProcessor::SetVariable(INT32 nIndex, INT32 nValue)
{
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_AI_VARIABLE_COUNT);

	m_nVarList[nIndex] = nValue;

Exit0:
	return;
}

inline INT32 CAIProcessor::GetVariable(INT32 nIndex)
{
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_AI_VARIABLE_COUNT);

	return m_nVarList[nIndex];

Exit0:
	return 0;
}

inline void CAIProcessor::SetTargetID(UINT32 dwTargetID)
{
    m_dwTargetID = dwTargetID;
}

inline UINT32 CAIProcessor::GetTargetID(void)
{
	return m_dwTargetID;
}

inline UINT32 CAIProcessor::GetNearestEnemyID(void)
{
    return m_dwNearestEnemyID;
}

inline UINT32 CAIProcessor::GetNearestEnemyRoleID(void)
{
	return m_dwNearestEnemyRoleID;
}

inline INT32 CAIProcessor::GetHostOldDirection(void)
{
    return m_nTargetDirection;
}

inline UINT32 CAIProcessor::GetFarthestEnemyID(void)
{
    return m_dwFarthestEnemyID;
}

inline UINT32 CAIProcessor::GetCastingEnemyID(void)
{
    return m_dwCastingEnemyID;
}

inline UINT32 CAIProcessor::GetNearestFriendID(void)
{
    return m_dwNearestFriendID;
}

inline UINT32 CAIProcessor::GetAuraCharID(void)
{
    return m_dwAuraCharID;
}

inline UINT32 CAIProcessor::GetCorpseCharID(void)
{
    return m_dwCorpseCharID;
}

inline UINT32 CAIProcessor::GetSignalIssuerID(void)
{
    return m_dwSignalIssuerID;
}

template<class TFunc>
inline BOOL CAIProcessor::_Scan9Region(CRegion* poCenterRegion, TFunc& rFunc)
{
    INT32 nRetCode = 0;
    INT32 RandStartPos = 0;
    CRegion* poRegion = NULL;

	PROCESS_ERROR(poCenterRegion);

	nRetCode = _ScanRegion(poCenterRegion, rFunc);
	PROCESS_ERROR(nRetCode);

    // check region in random order
    //RandStartPos = ((INT32)((void *)poRegion)) % DT_DIRECTION_8_COUNT;   
    //    //g_Random(0, DT_DIRECTION_8_COUNT - 1 );
    RandStartPos = rand() % DT_DIRECTION_8_COUNT;

	for (INT32 nIndex = 0; nIndex < DT_DIRECTION_8_COUNT; nIndex++)
	{
        poRegion = poCenterRegion->GetConnectedRegion( (nIndex + RandStartPos) % DT_DIRECTION_8_COUNT );
        if(poRegion )
		{
			nRetCode = _ScanRegion( poRegion, rFunc);
			PROCESS_ERROR(nRetCode);
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

template<class TFunc>
inline BOOL CAIProcessor::_ScanRegion(CRegion* poRegion, TFunc& rFunc)
{
	INT32 nRetCode = 0;
	PROCESS_ERROR(poRegion);

	nRetCode = poRegion->TraverseCharacter(rFunc);
	PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

inline void CAIProcessor::SetPatrolParam(INT32 nPathID, INT32 nPatrolStyle)
{
    m_nPatrolPathID = nPathID;
    m_nPatrolStyle = nPatrolStyle;
    m_nPatrolIndex = 0;
}

inline void CAIProcessor::SetFollowParam(UINT32 dwFollowHostID, INT32 nFollowOffsetX, INT32 nFollowOffsetY, BOOL bUseHostSpeed)
{
    m_dwFollowHostID = dwFollowHostID;
    m_nFollowOffsetX = nFollowOffsetX;
    m_nFollowOffsetY = nFollowOffsetY;
    m_bUseHostSpeed = bUseHostSpeed;
}

inline void CAIProcessor::ClearWanderPoint(void)
{
	m_nWanderPointCount = 0;
}

inline void CAIProcessor::SetIgnoreInvisible(BOOL bValue)
{
	m_bIgnoreInvisible = bValue;
}

inline BOOL CAIProcessor::IgnoreInvisible(void)
{
	return m_bIgnoreInvisible;
}

inline void CAIProcessor::SetCounterInvisible(BOOL bValue)
{
	m_bCounterInvisible = bValue;
}

inline BOOL CAIProcessor::CounterInvisible(void)
{
	return m_bCounterInvisible;
}



#endif	// _AI_PROCESSOR_H_
