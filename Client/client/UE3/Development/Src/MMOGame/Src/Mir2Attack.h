/**
 *
 *传奇攻击判定Ai
 *
 */

#ifndef __MIR2ATTACK_H__
#define __MIR2ATTACK_H__

#include "stdafx.h"
#include "MMOSingleton.h"


class AActor;
class UMMOAbility;
#define ONE_GRID_RADIUS 144.0f
#define TWO_GRID_RADIUS 372.0f
#define ONE_GRID_WIDTH  128.0f
class AttackClassX
{
public:
	AttackClassX();
	void ClearAbilityFlag();
	void SetAbilityFlag(INT FlagType);
	INT GetGridAttackbleActor(FVector Des);
	UBOOL GetBanYueCondition(FVector Des,INT nRange=1);
	UBOOL GetCiShaCondition(FVector Des,UBOOL bEx);
	INT GetBlockActorNumBetweenDesireMMOActor();

	void  UpdateDynamicPos(FVector DesPos, UBOOL b3x3);
	virtual UMMOAbility* OptimizeAbility(UMMOAbility* desireAbility,UMMOAbility* useAbility){return desireAbility;};
	virtual void SendUseAbility(INT AbilityId){};

	virtual void  Init(){};
	virtual UBOOL InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3) = 0;
	virtual UMMOAbility* GetPrimaryAbility() = 0;
	virtual UBOOL DoAIAbility() = 0;
	virtual	void Update(FLOAT fDeltaTime) = 0;

	
protected:
	UBOOL AbilityFlag[EAT_FLAG_MAX];
	AMMOPlayer* _pMainPlayer;
	FLOAT		_fDistance;

	FVector		_DesPos;
	FLOAT		_fDistance_GridX;
	FLOAT		_fDistance_GridY;
	UBOOL		_in8Dir;
};

class AttackClassA: public AttackClassX
{
public:
	UBOOL InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3);
	UMMOAbility* GetPrimaryAbility();	
	UBOOL DoAIAbility();
	void Update(FLOAT fDeltaTime);

private:
	UMMOAbility* _CanUseZhuRi();//逐日剑法
	UMMOAbility* _CanUseLieHuo();//烈火剑法
	UMMOAbility* _CanUseGongSha();//攻杀剑法
	UMMOAbility* _CanUseBanYue();//半月剑法
	UMMOAbility* _CanUseCiSha(UBOOL bEx);//刺杀剑法
	UMMOAbility* _CanUsePuGong();//普攻剑法

};

class AttackClassB: public AttackClassX
{
public:
	UBOOL InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3);
	UMMOAbility* GetPrimaryAbility();	
	UBOOL DoAIAbility();
	void Update(FLOAT fDeltaTime);

};

class AttackClassE: public AttackClassX
{
public:
	void  Init();
	UBOOL InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3);
	UMMOAbility* GetPrimaryAbility();	
	UMMOAbility* OptimizeAbility(UMMOAbility* desireAbility,UMMOAbility* useAbility);
	void SendUseAbility(INT AbilityId);
	UBOOL DoAIAbility();
	void Update(FLOAT fDeltaTime);

private:
	INT _Last_ShiDu_Ability;
};

// 1.05 backport: generic handler for the classes without a dedicated one (Rogue, Archer, Lancer, Sorcerer).
// It just uses the class's normal attack spell, with the same range rule as the Mage/Taoist handlers.
class AttackClassGeneric: public AttackClassX
{
public:
	AttackClassGeneric(INT normalAttackId) : _NormalAttackId(normalAttackId) {}
	UBOOL InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3);
	UMMOAbility* GetPrimaryAbility();
	UBOOL DoAIAbility();
	void Update(FLOAT fDeltaTime);

private:
	INT _NormalAttackId;
};



class Mir2Attack : public SingletonMeyers<Mir2Attack>
{
public:
	Mir2Attack();
	virtual ~Mir2Attack();

	void InitPlayerClassX();
	void Update(FLOAT fDeltaTime);
	void ClearAbilityFlag();
	void SetAbilityFlag(INT FlagType);
	UMMOAbility* GetPrimaryAbility();
	UMMOAbility* GetMineAbility(INT nMineLevel);
	UMMOAbility* OptimizeAbility(UMMOAbility* desireAbility,UMMOAbility* useAbility);
	void		 SendUseAbility(INT AbilityId);
	UBOOL InPrimaryAbilityRange(FVector DesPos, UBOOL b3x3);
	UBOOL InPrimaryAbility(INT AbilityID);


	UBOOL GetAbilityItemCostCondition(UMMOAbility* pUseAbility,UBOOL bSHowMsg=FALSE);
	void UpdateItemSubType(FClientItemInfo* newInfo,UItem* item);
	UBOOL DoAIAbility();
	void PreRotate(FVector DesPos);
	FLOAT GetNpcMoveSpeedTime(AMMOActor* actor,UBOOL bRun,FLOAT defaultTime); 

	//进入游戏
	void OnEnterGame(UBOOL bEnter);

	// 内挂练功
	void AutoPractice(UINT16 AbilityID, UINT8 deltaTime, UBOOL bOpen);
	UINT16 GetAutoPracticeAbilityID() { return m_PracticeAbilityID; }
	UINT8 GetAutoPracticeTime() { return m_PracticeDeltaTime; }
	UBOOL IsAutoPractice() { return m_bPracticeOpen; }

	void TryToMine(const FVector& Des);
private:	
	void _ShowAbilityScreenMsg(INT enumType);
private:
	AttackClassX* _AttackClassX;
	FLOAT		  _ShowScreenMsgCountDown;
	INT			  _ShowScreenMsgEnum;

public:
	UINT16		m_PracticeAbilityID;
	UINT8		m_PracticeDeltaTime;		// 限制16位，方便存取ClientVariable
	FLOAT		m_PracticeTime;
	UBOOL		m_bPracticeOpen;
	UBOOL		m_TryToMine;
	FVector		m_TryToMinePos;

};




#endif