#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "simple_list.h"

#include "scene_object.h"
#include "attribute_def.h"
#include "char_def.h"
#include "simple_timer.h"
#include "attr_list.h"
#include "attr_modifier.h"
#include "cooldown.h"
#include "target_scanner.h"
#include "threat_list.h"
#include "spell_book.h"
#include "aura_instance.h"
#include "spell_instance.h"
#include "vehicle_slot.h"

class CSpell;
class CSpellSet;
class CAura;
class CAuraInstance;
class CBullet;
class CUser;
class CAIProcessor;
class CRole;
class CNpc;
class CDoodad;
class CSpellSlot;
struct Item;
struct ItemInfo;
struct RUNE_DATA;

struct COMBAT_STAT_NODE : BASE_LIST_NODE
{
    UINT32  dwRoleID;
    UINT8   byFaction;
    INT32   m_Data[escsTotal];
};

struct SpellEventNode
{
	SimpleListNode m_StackNode;
	UINT64 qwEventKey;
	INT32 nNodeID;
	BOOL bRemoved;

	INT32 nRootSpellID;
	INT32 nRootSpellLevel;

	ExtraParam Param;
};


class CCharacter : public CSceneObject
{
public:
	CCharacter();
	virtual ~CCharacter();

    BOOL Init(UINT32 dwObjID);
    BOOL UnInit(void);
    BOOL MainLoop(void);
    BOOL MoveLoop(void);

	virtual const TCHAR* GetName(void) = 0;
	virtual INT32 GetSpecies(void) = 0;
	virtual ISpellBook& GetSpellBook(void) = 0;
	virtual INT32 GetPow(void);

	virtual BOOL JumpTo(INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection) = 0;

	virtual BOOL SetLevel(INT32 nNewLevel);
	virtual BOOL DoSyncAttrChange(INT32 eAttrType);

	virtual BOOL Die(INT32 nParam);

	virtual INT32 GetWeaponType(void);
	virtual CSpellSlot* GetSpellSlot(INT32 nIndex);
	virtual void ClearSpellForReloading(void);
	virtual BOOL Transform(BOOL bTransform);

	virtual INT32 EquipRune(const RUNE_DATA* pRuneData, Item* poItem);
	virtual BOOL UnEquipRune(const RUNE_DATA* pRuneData, Item* poItem);
	virtual INT32 CastSpell(CSpell* poSpell, CCharacter* poTarget, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ);
	virtual void OnHurt(CCharacter* poDealer, CBullet* poBullet, INT32 nEffectValue);
	virtual BOOL AddCooldown(UINT32 dwCooldownID, INT32& nCooldownFrm);        // +168
	virtual BOOL ChangeCooldown(UINT32 dwCooldownID, INT32& nCooldownFrm);
	virtual BOOL ResetCooldown(UINT32 dwCooldownID);
	virtual BOOL IsLoading(void)	{ return FALSE; }
	virtual void SetInCutScene(BOOL bInCutScene, INT32 nDurationFrm);

    inline void SetCombatStatNode(COMBAT_STAT_NODE* pNode);
    inline INT32 GetCombatStat(INT32 nField);
    inline void SetCombatStat(INT32 nField, INT32 nValue);
    inline void IncCombatStat(INT32 nField, INT32 nValue);

	inline void IncDamageStatCount(void);
	inline void IncTotalDamage(INT32 nValue);
	inline void ResetDamageStat(void);
	inline INT32 GetDamageStatCount(void);
	inline INT32 GetTotalDamage(void);

	inline CFlagBits& GetFlag(void);
	inline CFlagBits& GetExFlag(void);
	inline CFlagBits& GetCCFlag(void);
	inline CFlagBits& GetStiffFlag(void);

	inline INT32 GetStance(void);
	inline BOOL SetStance(INT32 eValue);

	BOOL CanAttack(CCharacter* poChar);

	BOOL PlayAction(UINT32 dwActionID);

	/************************************************************************/
	// attribute 

	inline INT32 GetClass(void) const;
	inline INT32 GetSex(void) const;
	inline INT32 GetLevel(void) const;

	inline void SetClass(INT32 nClass);
	inline void SetSex(INT32 nSex);

	inline INT32 GetHp(void);
	inline INT32 GetMp(void);

	inline void SetHp(INT32 nHp);
	virtual INT32 ChangeHp(INT32 nValue, INT32 nReason);    // +208

	inline void ChangeMp(INT32 nValue);

	inline void SetHpTap(INT32 nValue);
	inline void SetMpTap(INT32 nValue);

	inline void ChangeTotalDrugCureHp(INT32 nValue);
	inline void ChangeTotalDrugCureMp(INT32 nValue);

	inline void SetHpLeech(INT32 nValue);
	inline INT32 GetHpLeech(void);

	void ChangeTotalSpellHpCure(INT32 nValue);

	void MpToHp(INT32 nThisConvValue);
	void HpToMp(INT32 nThisConvValue);

	inline void SetDamageAbsorb(INT32 nValue);
	inline INT32 GetDamageAbsorb(void);
	inline INT32 GetRemainAbsorb(void);
	inline BOOL AddDamageType(INT32 eDamageType);
	inline INT32 GetDamageTypeBits(void);

	inline INT32 GetAttr(INT32 eAttrType);

	inline CAttrList& GetAttrList(void);



	/************************************************************************/
	// chat

	INT32 Say(UINT32 dwTargetID, UINT32 dwLocation, const char* pcszMessage);
    INT32 Say(UINT32 dwTargetID, UINT32 dwLocation, UINT32 dwStringID);
    INT32 Say(UINT32 dwTargetID, UINT32 dwLocation, UINT32 dwStringID, INT32 nParam);
    BOOL SendMail(UINT32 dwRoleID, INT32 nMailTemplateID);
    BOOL SendMail(UINT32 dwRoleID, INT32 nMailTemplateID, INT32 nItemID, INT32 nItemCount);
    BOOL SendMail(UINT32 dwRoleID, INT32 nMailTemplateID, const ItemInfo& iteminfo);

	/************************************************************************/
	// move

    virtual BOOL Revive(INT32 nParam, INT32 nHpPercent);
	inline BOOL IsDead(void);
	BOOL RotateToDir8(INT32 nFaceDirection, INT32 nDurationFrm);
	BOOL RotateToMirDir8(INT32 nMirDir8, INT32 nDurationFrm);
	virtual BOOL RotateTo(INT32 nFaceDirection, INT32 nDurationFrm);

	BOOL CanActiveMove(void);
	BOOL Stand(BOOL bForce);
	BOOL WalkTo(INT64 llDestX, INT64 llDestY);
	BOOL RunTo(INT64 llDestX, INT64 llDestY);
	BOOL MoveTo(INT64 llDestX, INT64 llDestY);
	inline void SetCanRun(BOOL bCanRun);
	BOOL Mine(INT64 llDestX, INT64 llDestY);

	BOOL Dash(INT32 nDuration, INT64 llDestX, INT64 llDestY);
	BOOL KnockBack(INT32 nDuration, INT64 llDestX, INT64 llDestY);
	BOOL Fear(INT32 nDirection, INT32 nDuration);
	BOOL Freeze(BOOL bFreeze);
	BOOL ClientShowTime(BOOL bShowTime);
	BOOL Thrown(INT32 nDuration, INT64 llDestX, INT64 llDestY);


	inline INT32 GetMoveState(void);
	inline INT64 GetDestX(void);
	inline INT64 GetDestY(void);

	void SetHome(INT32 nSceneID, INT64 llHomeX, INT64 llHomeY, INT64 llHomeZ);
	inline INT32 GetHomeSceneID(void);
	inline INT64 GetHomeX(void);
	inline INT64 GetHomeY(void);
	inline INT64 GetHomeZ(void);
	inline INT32 GetMoveTimer(void);

	void IncMoveDelay(INT32 nDelayFrame);

	/************************************************************************/
	// spell
	//void DisableCast(BOOL bDisable);

	inline BOOL IsCasting(void);

	inline INT32 NextSpellSerial(void);
	inline INT32 GetSpellSerial(void);
	inline CCoolDown& GetCoolDown(void);

#ifdef _ROBOT
	inline void SetCastingSpell(CSpellInstance* poSpell, INT32 nCastingOverFrm = 0);
#endif
	inline CSpellInstance* GetCastingSpell(void);

	inline void PushSpell(CSpellInstance* poSpell);
	inline CSpellInstance* GetPendingSpell(void);
	inline void PopSpell(void);

	virtual CSpell* GetEquippingSpell(void);

	inline void PushAura(CAuraInstance* poAura);
	inline CAuraInstance* GetPendingAura(void);
	inline void PopAura(void);

	inline void PushEventNode(SpellEventNode* pNode);
	inline SpellEventNode* GetPendingEventNode(void);
	inline void PopEventNode(void);

	inline CSpell* GetLearningSpell(void);

	void SetLastCasterID(UINT32 dwID);
	inline UINT32 GetLastCasterID();
    inline UINT32 GetLastCasterTimer();

    void SetLastTargetID(UINT32 dwObjID);
    inline UINT32 GetLastTargetID(void);
    inline UINT32 GetLastTargetTimer(void);

	inline void SetDropOwnerID(UINT32 dwID);
	inline UINT32 GetDropOwnerID(void);

	inline INT32 GetSpellState(void);
	inline CAuraList& GetAuraList(void);

	CCharacter* SelectFocus(UINT32 dwTargetID);
	inline UINT32 GetFocusID(void);

    inline void SetHostID(UINT32 dwCharID);
	inline UINT32 GetHostID(void);

	inline void SetExclusiveOwnerID(UINT32 dwCharID);
	inline UINT32 GetExclusiveOwnerID(void);

	inline void SetDamageShareCharID(UINT32 dwID);
	inline UINT32 GetDamageShareCharID(void);

	inline void ClearSubSpellDepth(void);

	void AdjustTargetPt(INT32 nX, INT32 nY, INT32 nZ, INT32 nDurationFrm);

	INT32 CastSpell(INT32 nSpellID, CCharacter* poTarget, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ);

	BOOL BreakCast(BOOL bIsBreak, BOOL bHard);
	void CancelCast(void);
	BOOL Hard(void);
	BOOL Stiff(BOOL bStiff, INT32 nFrm, INT32 nParam);

	void OnHurt(CCharacter* poDealer, INT32 nEffectValue);

	INT32 CastInstantSpell(CSceneObject* poFromObj, CSpell* poSpell, CCharacter* poTarget, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ);

	INT32 CheckTarget(CSpellInstance* poSpell, CCharacter* poTarget);
	INT32 CheckTarget(CCharacter* poTarget, INT32 nTargetObjTypeLimit, INT32 nRelationShipLimit, INT32 nStanceLimit, BOOL bCorpseValid, INT32 nTargetCheckNode);
	INT32 CheckTargetPos(CSceneObject* poFromObj, CSpellInstance* poSpell, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ, INT32 nTargetRadius, INT32 nTolerantGrids);

	BOOL CheckMinDist(CSpellInstance* poSpell, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ);
	BOOL CheckMaxDist(CSpellInstance* poSpell, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ, INT32 nTargetRadius);

	BOOL PushBullet(CBullet* poBullet);

	BOOL RegisterEvent(UINT64 qwKey, INT32 nModifierNodeID, INT32 nRootSpellID, INT32 nRootSpellLevel, ExtraParam* poParam);
	BOOL RegisterTimerEvent(UINT64 qwEventKey, INT32 nInterval, INT32 nModifierNodeID, INT32 nRootSpellID, INT32 nRootSpellLevel, ExtraParam* poParam);
	BOOL RemoveEvent(UINT64 qwEventKey);
	BOOL TrigEvent(INT32 nOccasionID, CSpellInstance* poPendingSpell);
	BOOL TrigBulletEvent(INT32 nOccasionID, CBullet* poBullet);

	BOOL RollBackEvent(INT32 nOccasionID);

	BOOL SaveSpellEvent(INT32 nSpellID, INT32 eOccasionID, UINT64 qwEventKey);
	BOOL UnregisterSpellEvent(INT32 nSpellID, INT32 eOccasionID);

	CAuraInstance* AppendAura(CAura* pAuraInfo, CCharacter* poCaster, UINT32 dwFromObjID, ExtraParam* pExParam, INT32 nNewDurationFrm, INT32 nNewStackCount, INT32 nRootSpellID, INT32 nRootSpellLevel);

	BOOL AppendAllPendingAura(void);

	void RemoveAura(CAuraInstance* pAura, BOOL bTrigOnRemove);
	BOOL RemoveAuraByType(INT32 nType);
	BOOL RemoveAuraBySubType(INT32 nSubType);


	BOOL DispelAura(CAuraInstance* pAura, INT32 nCount);

	void ReduceAuraStack(CAuraInstance* pAura, INT32 nCount);
	BOOL ReduceAuraStackByType(INT32 nCount, INT32 nType);
	BOOL ReduceAuraStackBySubType(INT32 nCount, INT32 nSubType);

	CAuraInstance* GetFirstAura(INT32 nAuraID);
	CAuraInstance* GetFirstAuraByType(INT32 nType);
	CAuraInstance* GetFirstAuraBySubType(INT32 nSubType);

	CAuraInstance* GetAura(UINT32 nInstanceID);

	BOOL UpdateSingleAura(CAuraInstance* pAura, BOOL bForce);

	BOOL ApplyModifierTree(INT32 nNodeID, CSpellInstance* poPendingSpell);
	BOOL RollbackModifierTree(INT32 nNodeID, CSpellInstance* poPendingSpell);

	inline BOOL NewEventKey(INT32 nOccasionID, INT32 eSource, INT32 nRegisterID, UINT64& qwEventKey);

	inline BOOL IncControlCount(INT32 nControlType);
	inline BOOL DecControlCount(INT32 nControlType);
	inline INT32 GetControlCount(INT32 nControlType);

	inline void SetPendingItem(Item* poItem);
	inline Item* GetPendingItem(void);

	inline void SetPendingItemSlot(INT32 nSlot);
	inline INT32 GetPendingItemSlot(void);

	BOOL DirectlyHpChange(INT32 nValue);

	inline void SetSeducerID(UINT32 dwID);
	inline UINT32 GetSeducerID(void);

	void CheckMissileTag(void);

	/************************************************************************/
	// mount

	inline INT32 GetCurrentMountID(void);

	// wing
	inline INT32 GetCurrentWingID(void);

	/************************************************************************/

	/************************************************************************/
	// ai 
	inline CThreatList& GetThreatList(void);

    BOOL SetAIProcessor(INT32 nAIID);
	inline CAIProcessor* GetAIProcessor(void);
	inline INT32 GetOriginalAiID(void);
	inline void SetNewAiID(INT32 nID);
	inline void SetGlobalCDDelayFrame(INT32 nFrame);
	inline INT32 GetGlobalCDDelayFrame(void);

	/************************************************************************/
	// doodad
	INT32 OperateDoodad(UINT32 dwDoodadID);
	BOOL OpenDoodad(CDoodad* pDoodad, INT32 nDelayFrame);

	/************************************************************************/
#ifdef _ROBOT
	inline void SetMoveState(INT32 eMoveState);
#endif	// _ROBOT

protected:
	/************************************************************************/
	// move
	BOOL _StepMove(void);

	virtual BOOL _ComputeAttribute(INT32 nNewLevel) = 0;

	virtual void _Recovery(void);

	BOOL _InitMp(void);

	BOOL _CanPassiveMove(void);
	void _OnStand(void);
	void _OnSit(void);
	void _OnWalk(void);
	void _OnRun(void);
	void _OnMove(void);
	void _OnMine(void);
	void _OnDash(BOOL& bBroken);
	void _OnKnockBack(void);
	void _OnThrown(void);

	void _OnFreeze(void);
	void _OnFear(void);

	BOOL _OnStiffFreeze(INT32 nParam);
	BOOL _OnStiffKnockBack(INT32 nParam);
	BOOL _OnStiffSilent(INT32 nParam);

	/************************************************************************/
	// spell

	void _AdjustTargetPos(CSpellInstance* poSpell, INT64& nTargetX, INT64& nTargetY, INT64& nTargetZ);

	INT32 _PrepareSpell(CSpellInstance* poSpell, CCharacter* poTarget);
	INT32 _PrepareSpell(CSpellInstance* poSpell, INT64& nTargetX, INT64& nTargetY, INT64& nTargetZ);
	void _CastSpell(CSpellInstance* poSpell, INT32& nCastTime);
    virtual BOOL _OnCastOver(void);

	void _UpdateSpell(void);
	void _UpdateBullet(void);
	void _UpdateEvent(void);
//	void _UpdateAura(void);
	void _CheckCombat(void);

	void _EnterCombat(void);
	void _LeaveCombat(void);

	INT32 _CheckCastCondition(CSpellInstance* poSpell);
	BOOL _CheckAuraAppend(CAura* pAura);
	BOOL _CheckSpellCoolDown(CSpell* poSpell);

	CAuraInstance* _AuraNew(CAura* pNewAuraInfo, CCharacter* poCaster, UINT32 dwFromObjID, ExtraParam* pExParam, INT32 nNewDuration, INT32 nNewStackCount, INT32 nRootSpellID, INT32 nRootSpellLevel);
	CAuraInstance* _AuraStack(CAuraInstance* pExistAura, CAura* pNewAuraInfo, CCharacter* poCaster, ExtraParam* pExParam, INT32 nNewDuration, INT32 nNewStackCount);
	CAuraInstance* _AuraSubstitute(CAuraInstance* pExistAura, CAura* pNewAuraInfo, CCharacter* poCaster, UINT32 dwFromObjID, ExtraParam* pExParam, INT32 nNewDuration, INT32 nNewStackCount, INT32 nRootSpellID, INT32 nRootSpellLevel);

	CAuraInstance* _NewAuraInstance(CAura* pAuraInfo, CCharacter* poCaster, UINT32 dwFromObjID, ExtraParam* pExParam, INT32 nNewDurationFrm, INT32 nNewStackCount, INT32 nRootSpellID, INT32 nRootSpellLevel);
	BOOL _AuraNew(CAuraInstance* poAuraInst, CCharacter* poCaster);


	BOOL _OnIdle(void);
	BOOL _OnPreCast(void);
	BOOL _OnChanneling(void);
	BOOL _OnPreparing(void);
	BOOL _OnPreAccumulating(void);
	BOOL _OnAccumulating(void);
	BOOL _OnOpening(void);
	BOOL _OnHard(void);
	BOOL _OnBreak(void);

	BOOL _DefaultCancelProc(void);
	BOOL _OnCancelChanneling(void);
	BOOL _OnCancelPreparing(void);
	BOOL _OnCancelPreAccumulating(void);
	BOOL _OnCancelAccumulating(void);
	BOOL _CancelOpening(void);

	void _ClearAfterDie(void);

	INT32 _MpRecoveryWarrior(INT32 nPeriod);
	INT32 _MpRecoveryMage(INT32 nPeriod);
	INT32 _MpRecoveryAssassin(INT32 nPeriod);
	INT32 _MpRecoveryArcher(INT32 nPeriod);

    /************************************************************************/

    void _On1SecMainLoop(void);

protected:
	/************************************************************************/
	// attribute
	INT32	m_nClass;    // +128
	INT32	m_nSex;		 // +
	INT32	m_nLevel;	 // +
    INT32	m_nFaction;	 // +

	CFlagBits m_oFlag;		   // +
	CFlagBits m_oExFlag;	   // +
	CFlagBits m_oCCFlag;	   // +
	CFlagBits m_oStiffFlag;    // +

	INT32	m_nCurrentHp;			// +
	INT32	m_nCurrentMp;			// +
	INT32	m_nExHp;				// +
	INT32	m_nHpTap;				// +
	INT32	m_nMpTap;				// +
	INT32	m_nTotalSpellCureHp;    // +180
	INT32	m_nTotalDrugCureHp;		// +184
	INT32	m_nTotalDrugCureMp;		// +188
	INT32	m_nCurrentHpLeech;		// +196
	INT64	m_llNextCureFrm;		// +

	INT32	m_nLastRecoveryFrm;
	INT32	m_nRemainDamageAbsorb;
	INT32	m_eStance;
	BOOL	m_bHpChanged;
	BOOL	m_bMpChanged;

	CAttrList m_AttrList;

	BOOL	m_bInCutScene;
	INT32	m_nCutSceneEndFrm;

	/************************************************************************/
	// move
	INT64	m_llDestX, m_llDestY;
	INT32	m_eMoveState;
	INT32	m_nMoveSpeed;
	BOOL	m_bCanRun;
	INT32	m_nMoveTimer;         // Movement interval
	INT32	m_nWalkTimer;         // Walking interval
	INT32	m_nRunTimer;          // Running interval
	INT32   m_nPassiveMoveTimer;  // Passive movement interval
	INT32	m_nMineTimer;         // Mining interval
	INT32	m_nFearDirection;
	INT32   m_nVolumeID;

	INT32	m_nFindState;
	INT32	m_nFindTimer;
	INT32	m_nFindTimes;
	INT32	m_nPathSide;

	INT32   m_nHomeSceneID;
	INT64   m_llHomeX;
	INT64   m_llHomeY;
	INT64   m_llHomeZ;

	/************************************************************************/
	// spell
	typedef BOOL (CCharacter::*_SpellStateFunc)(void);

	static _SpellStateFunc m_RunSpellStateFunc[cspsTotal];
	static _SpellStateFunc m_CancelSpellStateFunc[cspsTotal];

	typedef INT32 (CCharacter::*_MpRecoveryFunc)(INT32 nPeriod);
	static _MpRecoveryFunc m_MpRecoveryFunc[clsTotal];

	typedef INT32 (CCharacter::*_StiffFunc)(INT32 nParam);
	static _StiffFunc m_RunStiffFunc[cstTotal];


	INT32			m_eSpellState;
	UINT32			m_dwFocusID;
	UINT32			m_dwHostID;
	UINT32			m_dwExclusiveOwnerID;
	UINT32			m_dwSeducerID;
	UINT32			m_dwDamageShareCharID;
	UINT32          m_dwOperateDoodadID;
	CSpellInstance* m_poCastingSpell;

#ifdef _ROBOT
	INT32			m_nCastingOverFrm;
#endif

	INT32			m_nSubSpellDepth;
	INT32			m_nSpellSerial;
	CCoolDown		m_CoolDown;
	INT32			m_eHitTag;
	INT32			m_nDamagTypeBits;

	INT32			m_nCounterStiffRate;
	INT32			m_nStiffEndFrm;
	INT32			m_nNextStiffDecayFrm;

	INT32			m_nCounterControlRate;
	INT32			m_nNextControlDecayFrm;
	INT32			m_nTotalControlCount;

	UINT32			m_dwDropOwnerID;
	UINT32			m_dwLastCasterID;		// +1676
    UINT32          m_dwLastCasterTimer;    // +1680
    UINT32          m_dwLastTargetID;
    UINT32          m_dwLastTargetTimer;
	UINT32			m_dwAuraGenerator;
	UINT32			m_dwEventGenerator;

	INT32			m_nLastHurtFrm;

	INT32			m_nDamageCount;
	INT32			m_nTotalDamage;

	INT8			m_btControlCount[cclTotal];

	CSimpleList	m_PendingSpellStack;
	CSimpleList	m_PendingAuraStack;
	CSimpleList	m_PendingEventNodeStack;

	CSimpleTimer	m_CastTimer;
	CSimpleTimer	m_PreparingTimer;
	CSimpleTimer	m_AccumulatingTimer;
	CSimpleTimer	m_HardTimer;

	CBaseList		m_BulletList;

	typedef std::map<UINT64, SpellEventNode> TEventMap;
	TEventMap		m_RegEventMap[meoTotal];

	typedef std::map<UINT64, CSimpleTimer> TTimerMap;
	TTimerMap		m_RegTimerMap;

	typedef std::list<UINT64> TRemoveEventList;
	TRemoveEventList m_RemoveEventList;

	CAuraList		m_AuraList;

	typedef std::map<INT32, INT32> TDamageAbsorbMap;
	TDamageAbsorbMap m_DamageAbsorbMap;

	typedef std::list<CAuraInstance*> TPendingAuraList;
	TPendingAuraList m_PendingAuraList;

	Item*			m_poPendingItem;
	INT32			m_nPendingItemSlot;

    COMBAT_STAT_NODE* m_pStatNode;

	INT32				m_nMineValueCount;
	INT32				m_nMineCDTimer;
	INT32				m_nLastMinePos;

	/************************************************************************/
	// Mount
	INT32				m_nCurrentMountID;

	// Wing
	INT32				m_nCurrentWingID;

	/************************************************************************/

	/************************************************************************/
	// AI
	INT32 m_nOriginAiID;
	INT32 m_nNewAiID;
	CAIProcessor* m_poAIProcessor;
	INT32 m_nGlobalCDDelayFrame;

	CThreatList m_ThreatList;
	/************************************************************************/
	// Robot
#ifdef _ROBOT
public:
	inline INT32 GetReferCount()	{ return m_nReferCount; }
	inline void SetReferCount(INT32 nReferCount) { m_nReferCount = nReferCount;}
private:
	INT32 m_nReferCount;
#endif  // _ROBOT
};

#include "character_inl.h"

#endif	// _CHARACTER_H_
