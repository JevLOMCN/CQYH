/**
 *
 * game finite state machine (FSM) - state set
 * jimZhang@sdna.com 2014.5.6 
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

#ifndef __FSMSTATE_H__
#define __FSMSTATE_H__

#include "stdafx.h"
#include <list>
#include <vector>
#include <map>
#include "MMOSingleton.h"

using namespace std;

#define EPSILON			0.0001f
#define CLICK_TIME		0.3f
#define TIME_SYNC_FAST 	0.125f
#define TIME_SYNC_SLOW 	0.5f

class UMMOAbility;
class AMMOPlayerController;


enum GAME_CONTROL_TYPE
{
	GC_MIR2,					//Legend of Mir
	GC_DIABLO3,					//Diablo 3
	GC_LOL,						//League of Legends
	GC_MIRFOREVER,				//Legend Eternal
};

enum KEY_TYPE
{
	KT_NONE,					//none
	KT_PRIMARY,					//primary key
	KT_SECONDARY,				//secondary key
	KT_ELSE,					//other
};


class IFSMState
{
public:
	IFSMState();
	virtual void Init() = 0;
	virtual void Update(FLOAT DelTime) = 0;
};


class FSMStateManager : public SingletonMeyers<FSMStateManager>
{
private:
	std::vector<IFSMState*> IFSMStateList;
	typedef std::vector<IFSMState*>::iterator FSMStateIterator;
public:
	void Init();
	void AddGameState(IFSMState* pState);
	void Update(FLOAT DelTime);
};


//mouse/key input series states----------------------------------------------------------------
class FSMState_Input: public IFSMState
{
public:
	void Init();
	void Update(FLOAT DelTime);
public:
	UBOOL		ShiftDown;					//whether shift is pressed
	UBOOL		LeftBtnDown;				//whether left mouse button is pressed
	UBOOL		RightBtnDown;				//whether right mouse button is pressed
	UBOOL		CtrlBtnDown;				//whether Ctrl is pressed
	UBOOL		AltBtnDown;					//whether Alt is pressed

	UBOOL		ShiftDown_Last;				
	UBOOL		LeftBtnDown_Last;			
	UBOOL		RightBtnDown_Last;			
	UBOOL		CtrlBtnDown_Last;		
	UBOOL		AltBtnDown_Last;					
};



//Main character related series states-------------------------------------------------------------
class FSMState_Charactor: public IFSMState
{
public:
	void Init();
	void Update(FLOAT DelTime);
public:
	AMMOPlayer*				MainPlayer;		//main character
	AMMOPlayerController*	Controller;		//main controller
	UBOOL					MoveEnable;		//whether main character can move
	UBOOL					MoveEnableInAdvance;	//whether main character can issue move commands
	UBOOL					IsDead;
	INT						AbilityActionNum;//number of ability actions waiting to execute
	INT						nCannotInterruptLevel;//interrupt flag and flag level
};



//Various countdown series states----------------------------------------------------------
class FSMState_CDTime: public IFSMState
{
public:
	void Init();
	void Update(FLOAT DelTime);
public:
	FLOAT			KeyPressedTime;			//accumulated key press time;
	FLOAT			FastTime;				//fast loop timer;
	FLOAT			SlowTime;				//slow loop timer;
	FLOAT			RotationCDTime;			//rotation CD time;
	FLOAT			AiCdTime;				//AI-controlled cd;
	FLOAT			AbilityRespondTime;		//time waiting for ability message response;
	FLOAT			SwitchRespondTime;		//toggle ability lock time
	FLOAT			AiInSafeZoneTime;		//time for AI to sheathe weapon in safe zone;
	FLOAT			CheckWeaponBindTime;	//weapon bind check time;
	FLOAT			AiBossQTETime;			//boss qte detection time
	FLOAT			WarningTime;			//warning timer
	FLOAT			FocusTime;				//focus activation countdown
	FLOAT			WalkDelayTime;		//after casting a magic ability, there is a walk cd
	FLOAT			RunDelayTime;		//after casting a magic ability, there is a walk cd
	FLOAT			MoveDelayTime;		//movement restriction
	FLOAT			MoveDelayFrame;		//movement restriction frame count
	FLOAT			DoAIAbilityCD;		//AI cast interval
};



//Ability series states----------------------------------------------------------------
class FSMState_Ability: public IFSMState
{
public:
	void Init();
	void Update(FLOAT DelTime);
public:
	//UMMOAbility*	GetDefaultAbility();
	//UMMOAbility*	OptimizeAbility(UMMOAbility* Ability);
	//void SetDesireAbility(UMMOAbility*	Ability);
public:
	UMMOAbility*	DesireFClassAbility;			//ability cast by key (set F)
	UMMOAbility*	DesireAbility;			//ability cast by key (set)
	UMMOAbility*	UsableAbility;			//ability cast by key (usable)
	UMMOAbility*	CurrentAbility;			//currently cast ability (usable)
	UMMOAbility*	CacheAbility;			//cached ability
	UBOOL			PickTarget;				//cast in place flag
	UBOOL			AbilityEnable;			//whether the current ability can be cast
};



//target series states----------------------------------------------------------------
class FSMState_Target: public IFSMState
{
public:
	void Init();
	void Update(FLOAT DelTime);
public:
	void SetTargetActor(AActor* TargetActor);
	void ClearActor(AActor* Actor);
	FVector SmoothTargetPos(FVector TargetPos);
	FVector OptimizeTargetPos(FVector TargetPos);
	INT		OptimizeTargetID(FRotator Rotator,FVector Location,FLOAT Radius);

	INT OptimizeDoodadID(const FVector& Location,FLOAT Radius);


public:
	//current
	AActor*			CurTargetActor;			//target actor currently specified by mouse
	AActor*			LastTargetActor;		//target actor previously specified by mouse
	AMMOActor*		CurTargetMMOActor;		//target mmoactor currently specified by mouse
	AMMOActor*		LastTargetMMOActor;		//target mmoactor previously specified by mouse
	INT				CurTargetMMOActorID;
	INT				LastTargetMMOActorID;
	AMMOPlayer*		CurTargetMMOPlayer;		//target player currently specified by mouse
	ANPC*			CurTargetNPC;			//target npc currently specified by mouse
	FVector			CurTargetPos;			//current target point (smoothed)
	FVector			CurOptimizeTargetPos;	//current target point (optimized)
	UBOOL			TargetAttackble;		//whether the current target can be attacked
	UBOOL			TargetIsDead;			//whether the current target is dead

	//locked
	AActor*			DesireActor;			//locked target actor
	AMMOActor*		DesireMMOActor;			//locked target mmoactor
	AMMOActor*		LastDesireMMOActor;		//previously locked target mmoactor
	AMMOPlayer*		DesireMMOPlayer;		//locked target mmoplayer
	ANPC*			DesireNPC;				//locked target npc
	INT				DesireNPCTempleteID;	//locked target npc template ID
	FVector			DesirePos;				//locked target point
	UBOOL			DesireAttackble;		//whether locked target can be attacked
	TScriptInterface<IIInteractableActor>	DesireInteractableActor; //locked interactable object
	UBOOL			DesireMMOActorChangeFlag; //lock switch flag

	//ability locked target
	AMMOActor*		AbilityTargetMMOActor;	//ability locked target

	AMMOActor*		AttackMMOActor;			//attack target mmoactor
	AMMOActor*		SelectMMOActor;			//selected target mmoactor

	FVector			LastMouseHitNavMeshLocation;			//last/frame mouse pick point
	FVector			LastMainPlayerPos;						//last character position
	FVector			LastOffsetDes;			//last character facing

	//ui lock
	INT				UITargetMMOActorID;
	//tips related
	UBOOL			CaptureInput;
	UBOOL			ShowUITipsFlag;
	//
	FLOAT			UpdateSceneHintTime;
	INT				UpdateSceneHintID;

};

//trigger event flags----------------------------------------------------------
class FSMState_Trigger: public IFSMState
{
public:
	void Init();
	void Update(FLOAT DelTime);
public:
	UBOOL			IsTargetTrigger;		//current target trigger event
	UBOOL			IsDesireTrigger;		//locked target trigger event
	UBOOL			IsDesirePosTrigger;		//locked position trigger event
	UBOOL			IsWaitingSendMove;		//waiting for move message to be sent successfully
	FLOAT			DelayTimeWaitingSendMove;
};

//Control series states----------------------------------------------------------
class FSMState_Ctrl: public IFSMState
{
public:
	void Init();
	void Update(FLOAT DelTime);
public:
	UBOOL				IsBattleState;			//battle state
	UBOOL				IsGuildBattle;			//guild battle state
	//GAME_CONTROL_TYPE	GameControlType;		//game control mode
	KEY_TYPE			KeyType;				//key type
	KEY_TYPE			KeyType_Keeping;		//key type, sustained state

	UBOOL				IsSelectCtrl;			//select control
	UBOOL				IsMoveCtrl;				//move control
	UBOOL				IsActionCtrl;			//action control

	UBOOL				state_Switching_Flag;	//state switching flag
	UBOOL				state_Standby;			//standby state
	UBOOL				state_Moving;			//moving state
	UBOOL				state_Attack;			//attack state
	UBOOL				state_Interact;			//interact state
	UBOOL				state_AiDesire;			//search state

	UBOOL				state_AlwaysWalk;		//force walk state
	UBOOL				state_AlwaysWalkBase;	//force walk base state

	UBOOL				state_OpenPrivateChat;	//smart open private chat

	UBOOL				ctrlFlag_IgnoreShiftBtn;//no shift key needed
	UBOOL				ctrlFlag_DDCS;			//every-hit assassinate
	UBOOL				ctrlFlag_ZNBY;			//smart crescent slash
	UBOOL				ctrlFlag_ZDZR;			//auto sun-chasing slash
	UBOOL				ctrlFlag_ZDLH;			//auto flaming sword
	UBOOL				ctrlFlag_ZDYS;			//auto stealth
	UBOOL				ctrlFlag_ZDKD;			//auto shield
	FLOAT				ctrlAttackAi_LastDelay;	//delay time of LastServerLocation
	FLOAT				ctrlAttackAi_LockTime;	//lock time right after stopping, prevents empty swings caused by serverlocation movement

	UBOOL				IsWantToSendRequest4000Or4001Ability;

	UBOOL				ctrlKeepRunning;		//keep running forward
	FVector				vecRotation;			//keep forward direction vector
};


namespace FSM{
	extern FSMState_Input			Input;
	extern FSMState_Charactor		Char;
	extern FSMState_Ability			Ability;
	extern FSMState_CDTime			CDTime;
	extern FSMState_Target			Target;
	extern FSMState_Trigger			Trigger;
	extern FSMState_Ctrl			Ctrl;
}









#endif