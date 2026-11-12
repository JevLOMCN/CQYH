/**
 *
 * Game finite state machine (FSM) - IO interface
 * jimZhang@sdna.com 2014.5.7 
 * 
 * Doule-Level Extensible Finite State Machine
 * Two-layer extensible finite state machine
 *  
 * 1. Bottom-layer states are limited common base states; bottom-layer states can be input to upper-layer states. Same-layer states cannot be input.
 * 2. Upper-layer states are specific states and cannot be input to bottom-layer states or same-layer side branches.
 * 3. State refresh has two modes, pull type and trigger type, due to different storage (self-stored and other-stored).
 * 4. Trigger-type state refresh provides an extensible interface; state update depends only on its own stored flags.
 * 5. State decision modules can use bottom-layer and upper-layer states as input.
 * 6. State execution modules are called directly only by the state decision module.
 *
 */

#ifndef __FSMIO_H__
#define __FSMIO_H__

#include "stdafx.h"
#include "MMOSingleton.h"
#include "FSMCtrl_TargetTips.h"

class AActor;
class UMMOAbility;
class AMMOPlayerController;

class FSMIO_CDTime
{
public:
	void ResetCD_SafeZoneDelayTime();
	void ResetCD_CheckWeaponBindTime(FLOAT CheckTime);
	void ResetCD_AbilityRespondTime(FLOAT bRespondTime=0.2f);
	void ResetCD_SwitchRespondTime(FLOAT bRespondTime);
	void ResetCD_MoveDelay(FLOAT fDelayTime);
	void ResetCD_MoveDelay_Frame(INT nDelayFrame);
	void ResetCD_WalkAndRunDelay(FLOAT fWalkDelayTime,FLOAT fRunDelayTime);

};

class FSMIO_State
{
public:
	void ClearAllState();
};

class FSMIO_Ability
{
public:
	UMMOAbility* GetDefaultAbility();
	UBOOL CanUseAnyAbility(class UMMOAbility* InAbility);
	void SetCurrentAbility(class UMMOAbility* InAbility);

};

class FSMIO_Target
{
public:
	//Set target
	void SetDesirePos(FVector Pos, INT NPCTempleteID);
	void SetDesireActor(AActor* Target);
	void SetCacheAbility(UMMOAbility* Ability);
	void SetDesireAbility(UMMOAbility* Ability);
	void SetDesireFClassAbility(UMMOAbility* Ability,UBOOL bAddOrRemove);
	void SetDesireTarget(UMMOAbility* Ability,AActor* Target);
	void SetDesireActor_FromAbility();
	void SetUITargetActor(INT nUITargetMMOActorID);
	
	
	void SetInputText();
	//Cancel attack
	void ClearDesireActor();
	void ClearDesireAbility(UMMOAbility* Ability);
	void ClearDesireTarget(UMMOAbility* Ability);
	UMMOAbility* GetDesireAbility();
	//Set selected target
	void SetSelectMMOActor(AActor* Actor);
	void UpdateDesireAbility(int slotId, int abilityId);

	//Set target actor
	void SetTargetActor(AActor* TargetActor);
	void SetDropActor(AActor* TargetActor);
	//When an actor dies
	void ClearActor(AActor* Actor);

	void SetKeepRunning(UBOOL bRunning);
	void ToggleKeepRunning();

	//Get current target
	AMMOActor*	GetTargetMMOActor();
	AMMOActor*	GetDesireMMOActor();
	AMMOActor*	GetAttackMMOActor();
	AMMOActor*	GetSelectMMOActor();
	AActor*		GetDesireActor();
};

class FSMIO_Ctrl
{
public:
//	void SetMoveCtrl(UBOOL bMoveCtrl);
//	void SetActionCtrl(UBOOL bActionCtrl);
//	void SetSelectCtrl(UBOOL bSelectCtrl);
	void  SetKeyType(KEY_TYPE KeyType);
	void  SetAlwaysWalk(UBOOL bFlag);
	UBOOL GetAlwaysWalk();
	void  SetAlwaysWalkBase(UBOOL bFlag);
	UBOOL GetAlwaysWalkBase();
	void  SetGuildBattle(UBOOL bActive);
	UBOOL GetGuildBattle();
	void StartNavigation();
	UMMOAbility* GetLeftBtnBindAbility(UBOOL bRelease);
	UMMOAbility* GetRightBtnBindAbility(UBOOL bRelease);
	void AbilityKeyDown();
	void AbilityKeyUp();
	void UpdateMoving();
	void SetInputText();
private:
	void PrimaryKeyDown();
	void PrimaryKeyUp();
	void SecondaryKeyDown();
	void SecondaryKeyUp();
	void OtherKeyUp();



};

class FSMIO_Trigger
{
public:
	void  SetWaitingSendMove(UBOOL bFlag);
	UBOOL GetWaitingSendMove();

	void  SetShowTips(UBOOL bShow, UBOOL bForce=FALSE);

};



class FSMIO : public SingletonMeyers<FSMIO>
{
public:
	FSMIO_State		State;
	FSMIO_Target	Target;
	FSMIO_CDTime	CDTime;
	FSMIO_Ability	Ability;
	FSMIO_Ctrl		Ctrl;
	FSMIO_Trigger	Trigger;
};

#endif