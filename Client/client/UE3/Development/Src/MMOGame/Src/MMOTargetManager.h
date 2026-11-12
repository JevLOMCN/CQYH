/**
 * jimZhang@sdna.com 2013.1.22 
 *
 * Diablo 3 is here; ability casting and character control must be refined, user-friendly and smart
 * Overview of abilities 1-4, left/right mouse button abilities; left mouse button also doubles as walk control
 * Tap 1-4 or the right mouse ability key; if it is a typical targeted ability, it checks the position under the mouse,
 * if there is an enemy, walk over and cast the ability on the enemy
 * if there is no enemy, it is equivalent to walking over
 * Hold 1-4 or the right mouse ability key; if it is a typical targeted ability, it checks the position under the mouse,
 * equivalent to attack-moving all the way there,
 */

#ifndef __MMOTARGETMANAGER_H__
#define __MMOTARGETMANAGER_H__

#include "stdafx.h"
#include <list>
#include <vector>
#include <map>
#include "MMOSingleton.h"

using namespace std;

class UMMOAbility;
class AMMOPlayerController;

struct MMOGameEvent_TargetChange : public MMOGameEvent
{
public:

	enum ETargetChangeType
	{
		ETCT_DesiredTarget,
		ETCT_CurrentTarget
	};

	MMOGameEvent_TargetChange(ETargetChangeType evtType, AMMOActor* currentTarget, AMMOActor* desiredTarget = 0)
		: EventType(evtType)
		, CurrentTarget(currentTarget)
		, DesiredTarget(desiredTarget)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_TargetChange;
	}

	ETargetChangeType EventType;
	class AMMOActor* CurrentTarget;
	class AMMOActor* DesiredTarget;

};



class MMOTargetManager : public SingletonMeyers<MMOTargetManager>
{

	enum CTRL_STATE
	{
		CS_IDLE,					//idle state
		CS_MOVE,					//move state
		CS_BATTLEMOVE,				//battle state
	};

	enum DESIRE_STATE				//state trigger
	{
		DS_NORMAL,					//low level
		DS_TRIGGER,					//rising edge trigger
		DS_HOLDON,					//high level
		DS_RELEASE,					//falling edge trigger
	};

	enum SEARCH_TYPE
	{
		ST_CLOSED_ALL,				//closest obj of all types
		ST_CLOSED_DAMAGEABLE,		//closest attackable type
		ST_CLOSED_FRIENDLY,			//closest friendly type
		ST_INNER_ALL,				//first detected within range, all types
		ST_INNER_DAMAGEABLE,		//first detected within range, attackable type
		ST_INNER_FRIENDLY,			//first detected within range, friendly type
	};

public:
	MMOTargetManager();
	virtual ~MMOTargetManager(){};

private://various states
	UBOOL			stateInterrupt;			//sequential interrupt, used among identical judgment states to judge by priority order; this is the only order-dependent one and is serial, all other conditions are parallel and independent of execution order
	
	AMMOPlayer*		stateMainPlayer;		//main character
	AMMOPlayerController*	stateController;//main controller
	UBOOL			stateStallState	;		//stall state

	UMMOAbility*	stateLeftBtnAbility;	//ability equipped on left button
	UMMOAbility*	stateDesireAbility;		//ability cast by key (initial)
	UMMOAbility*	stateAbility;			//ability cast by key (usable)
	UBOOL			stateKeyPressed;		//key pressed
	UBOOL			stateShiftDown;			//whether shift is pressed
	UBOOL			stateLeftBtnDown;		//whether left mouse button is pressed
	UBOOL			stateLeftBtn;			//ability triggered by left mouse button
	UBOOL			stateAbilityEnable;		//whether abilities can be used
	UBOOL			stateMoveEnable;		//can walk
	UBOOL			stateMoveKeyDown;		//a movement key is pressed
	UBOOL			statePathOptimize;		//whether the pathfinding points are non-straight or optimized
	UBOOL			stateIsShowTips;		//show object tips
	UBOOL			statePickTarget;		//whether current ability is pickTarget

	AActor*			stateLastTargetActor;	//previous frame target actor
	AActor*			stateTargetActor;		//target actor
	AMMOActor*		stateLastTargetMMOActor;//previous frame target mmoactor
	AMMOActor*		stateTargetMMOActor;	//target mmoactor
	INT				stateTargetMMOActorID;		//
	INT				stateLastTargetMMOActorID;	//
	AMMOPlayer*		stateTargetMMOPlayer;	//target mmoplayer
	ANPC*			stateTargetNPC;			//target npc
// 	ADropItemActor* stateTargetItem;		//target pickup item
	ATeleportOrig*	stateTeleportOrig;		//target teleport point
	ATeleportNetNode* stateTeleportNetNode;	//target teleport network node
	AMMODoodadActor*  stateDoodadActor;	//doodad pointer
	UBOOL			stateAttackble;			//whether target is attackable
	UBOOL			stateIsDead;			//whether target is dead
	FVector			stateTargetPos;			//target point

	AActor*			stateDesireActor;		//initial target actor
	AMMOActor*		stateDesireMMOActor;	//initial target mmoactor
	AMMOActor*		stateLastDesireMMOActor;//previous frame initial target mmoactor
	AMMOPlayer*		stateDesireMMOPlayer;	//initial target mmoplayer
	ANPC*			stateDesireNPC;			//initial target npc
	INT				stateDesireNPCTempleteID;//initial target npc templateID
	//AMMODropActor* stateDesireItem;		//initial target pickup item
	UBOOL			stateDesireAttackble;	//whether initial target is attackable
	FVector			stateDesirePos;			//initial target point
	FLOAT			stateSetFocusDelayTime; //delay for select message request

	AMMOActor*		stateAttackMMOActor;	//attack target mmoactor
	AMMOActor*		stateSelectMMOActor;	//selected mmoactor target

	AActor*			stateFocusActor;		//focus actor
	AMMOActor*		stateFocusMMOActor;		//focus mmoactor
	AMMOPlayer*		stateFocusMMOPlayer;	//focus mmoplayer


	CTRL_STATE		stateCtrlState;			//character control state; two states derived from studying Diablo 3, plus an idle state
	DESIRE_STATE	stateDesireState;		//key state

	FLOAT			statePressedTime;		//accumulated key press time;
	FLOAT			stateFastTime;			//fast loop timer;
	FLOAT			stateSlowTime;			//slow loop timer;
	FLOAT			stateAiCdTime;			//AI-controlled cd;
	FLOAT			stateAbilityRespondTime;//time waiting for ability message response;
	FLOAT			stateAiInSafeZoneTime;	//AI-controlled time to sheathe weapon in safe zone;
	FLOAT			stateWarningTime;		//warning timer


private:
	FVector			LastMouseHitNavMeshLocation;			//last/frame mouse pick point
	FLOAT			TimeQTE;								//QTE timer;
	INT				AresFlag;								//gm auto-evolve God of War mode
	INT				AresIndex;								//gm auto-evolve God of War mode index
	FLOAT			AresDeltaTime;							//interval between steps

private://various states
	//cast while holding shift
	UBOOL	stateShiftAbility();
	//cast ability in place
	UBOOL	stateStayAndUseAbility();
	//chase and cast ability
	UBOOL	stateMoveToAndUseAbility();
	//search for enemy and cast ability
	UBOOL   stateSearchAndUseAbility();
	//walk close and interact
	UBOOL	stateMoveToAndInteract();
// 	//walk close and set up stall
// 	UBOOL	stateMoveToAndStall();
// 	//walk close and pick up item
// 	UBOOL	stateMoveToPickItem();
	//walk to target point
	UBOOL	stateMoveToPoint();
	//walk to specified coordinate
	UBOOL	stateMoveToDesirePoint();
	//show third-party portrait on selection
	UBOOL	stateShowInfo();
	//selected object tips display
	UBOOL	stateShowTips();
	//selected object under-foot cursor display
	UBOOL	stateShowSelectMark();
	//keyboard movement control
	UBOOL	stateMoveByKey();
	//auto find monster
	UBOOL	stateSearchEnemy();
	//auto switch key state
	UBOOL	stateAIContral();
	//auto detect sheathing weapon in safe zone
	UBOOL	stateAIInSafeZone();
	//auto decide whether to cast QTE-breaking ability
	UBOOL	stateAIBossQTE();
	//click handling while setting up stall
	UBOOL	stateClickStallState();
	//special timed prompts, e.g. irregular-interval durability warnings for the main weapon
	UBOOL	stateStackWarningState();
	//smartly clear states such as decompose, identify, repair,
	UBOOL	stateReleaseOperation();
	//gm combo test, automatically become God of War
	UBOOL	stateUpdateAres();

private:
	FVector			OptimizeTargetPos(FVector TargetPos );	//get optimized point
	UMMOAbility*	OptimizeAbility(UMMOAbility* Ability);	//get optimized ability
	INT				OptimizeTargetID(FRotator Rotator, FVector Location,FLOAT Radius, SEARCH_TYPE SType);	//get optimized target
	FVector			OptimizeMoveOffset(BYTE* pMode );

public:
	UMMOAbility*	GetDefaultAbility();	//get default ability of each class

public://various methods
	void	Clear();
	void	AITick(FLOAT DeltaTime);
	UBOOL	UpdateState(FLOAT DeltaTime);
	void	SetDesirePos(FVector Pos,INT NPCTempleteID=0);
	void	SetDesireTarget(UMMOAbility* Ability,AActor* Target,UBOOL bLeftMouseBtn=FALSE);
	void	ClearDesireTarget(UMMOAbility* Ability = NULL);
	
	void	ClearActor(AActor* Actor);

	AActor*	GetDesireActor();

	void	SetTargetActor(AActor* TargetActor);
	AMMOActor*	GetTargetMMOActor();
	AMMOActor*	GetLastTargetMMOActor();

	void	SetFocusActor(AActor* FocusActor);
	AActor*	GetFocusActor();

	AMMOActor*	GetSelectMMOActor();
	void	SetSelectMMOActor(AMMOActor* SelectActor);

	void	EnterScene();
	void	ResetSafeZoneDelayTime();
	void	StartAres(){if(AresFlag<=0)AresFlag=1;};

};







#endif