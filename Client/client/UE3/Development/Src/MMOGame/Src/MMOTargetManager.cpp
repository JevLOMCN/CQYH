/**
 * jimZhang@sdna.com 2013.1.22 
 *
 * state trigger mechanism oriented to abstract logic objects
 * 
 *
 *
 *
 */

#include "StdAfx.h"
#include "MMOTargetManager.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMONavigationManager.h"
#include "ScaleformEngine.h"
#include "UITextParser.h"

#define EPSILON			0.0001f
#define CLICK_TIME		0.3f
#define TIME_SYNC_FAST 	0.125f
#define TIME_SYNC_SLOW 	0.6f


MMOTargetManager::MMOTargetManager(void)
{
	Clear();
}

void MMOTargetManager::Clear()
{
	stateMainPlayer =	NULL;		//main character
	stateController	=	NULL;		//main controller
	stateStallState	=	FALSE;		//stall state

	stateLeftBtnAbility =NULL;		//ability equipped to left button

	stateDesireAbility =NULL;		//ability desired to cast
	stateAbility	=	NULL;		//ability being cast
	stateKeyPressed	=	FALSE;		//key pressed
	stateShiftDown	=	FALSE;		//whether shift is pressed
	stateLeftBtnDown=	FALSE;		//whether the left mouse button is pressed
	stateLeftBtn	=	FALSE;		//left mouse button event
	stateAbilityEnable= FALSE;		//whether abilities are usable
	stateMoveEnable	=	FALSE;		//can walk
	stateMoveKeyDown=	FALSE;		//a move key is pressed
	statePathOptimize = FALSE;		//whether the pathfinding points are non-straight or optimized
	statePickTarget=	FALSE;		//


	stateTargetActor=	NULL;		//target actor
	stateTargetMMOActor=	NULL;	//target mmoactor
	stateTargetMMOPlayer=	NULL;	//target mmoplayer
	stateTargetMMOActorID		=0;		//
	stateLastTargetMMOActorID	=0;		//
	stateTargetNPC	=	NULL;		//target npc
	//stateTargetItem	=	NULL;		//target pickup item
	stateAttackble	=	FALSE;		//whether the target is attackable
	stateTeleportOrig	=	NULL;	//target teleport point
	stateTeleportNetNode=	NULL;	//target teleport net node
	stateIsDead			=	NULL;	//whether the target is dead
	stateIsShowTips		=	FALSE;

	stateAttackMMOActor = NULL;
	stateSelectMMOActor	= NULL;

	//stateTargetPos=		NULL;		//target point
	stateLastTargetActor	= NULL;
	stateLastTargetMMOActor = NULL;

	stateDesireActor=	NULL;		//initial target actor
	stateDesireMMOActor=NULL;		//initial target mmoactor
	stateLastDesireMMOActor=NULL;	//
	//stateDesireMMOPlayer=	NULL;	//initial target mmoplayer
	stateDesireNPC=		NULL;		//initial target npc
	stateDesireNPCTempleteID = 0;	
// 	stateDesireItem=	NULL;		//initial target pickup item
	stateDesireAttackble=NULL;		//whether the initial target is attackable
	//stateDesirePos=		NULL;		//initial target point
	stateDoodadActor	= NULL;

	stateFocusActor		= NULL;		//focus actor
	stateFocusMMOActor	= NULL;		//focus mmoactor
	stateFocusMMOPlayer	= NULL;		//focus mmoplayer


	stateCtrlState	=	CS_IDLE;	//character control state; two states derived from studying Diablo 3, plus an idle state
	stateDesireState=   DS_NORMAL;	//trigger state

	statePressedTime=	0.0f;		//accumulated key press time;
	stateFastTime	=	0.0f;		//fast loop timer;
	stateSlowTime	=	0.0f;		//slow loop timer;
	stateAiCdTime	=	0.0f;		//AI-controlled cd;
	stateAbilityRespondTime=0.0f;	//time waiting for ability message response;


	stateInterrupt	=	NULL;
	stateDesireMMOPlayer = NULL;
	stateSetFocusDelayTime = 0.0f;
	TimeQTE = -0.1f;
	stateWarningTime = 0.0;

	AresFlag = 0;		
	AresIndex = 0;
	AresDeltaTime = 0.0;
}

void MMOTargetManager::EnterScene()
{
	ResetSafeZoneDelayTime();
}

void MMOTargetManager::AITick(FLOAT DeltaTime)
{
	if(GIsEditor)
	{
		return;
	}

	//update all states
	UpdateState(DeltaTime);

	//branch on state---------------------------------------------------
		//cast while holding shift
		stateShiftAbility();
		//cast ability in place
		stateStayAndUseAbility();
		//chase and cast ability
		stateMoveToAndUseAbility();
		//search for enemy and cast ability
		stateSearchAndUseAbility();
		//walk close and interact
		stateMoveToAndInteract();
		//walk close and set up stall
		//stateMoveToAndStall();
		//walk close and pick up item
// 		stateMoveToPickItem();
		//walk to target point
		stateMoveToPoint();
		//walk to specified coordinate
		stateMoveToDesirePoint();
		//keyboard movement control
		stateMoveByKey();
		//show target info and portrait
		stateShowInfo();
		//selected object tips display
		stateShowTips();
		//selected object under-foot cursor display
		stateShowSelectMark();
		//auto find monster
		stateSearchEnemy();
		//auto switch key state
		stateAIContral();
		//auto detect sheathing weapon in safe zone
		stateAIInSafeZone();
		//auto decide whether to cast QTE-breaking ability
		stateAIBossQTE();
		//click handling while setting up stall
		stateClickStallState();
		//special timed prompts, e.g. irregular-interval durability warnings for the main weapon
		stateStackWarningState();
		//smartly clear states such as decompose, identify, repair,
		stateReleaseOperation();
		//gm combo test, automatically become God of War
		stateUpdateAres();

	//----------------------------------------------------

	if(	stateDesireState==DS_TRIGGER )
	{
		stateDesireState = DS_HOLDON;
	}
	if( stateDesireState==DS_RELEASE )
	{
		stateDesireState = DS_NORMAL;
	}

	stateLastDesireMMOActor = stateDesireMMOActor;

	while(stateFastTime>TIME_SYNC_FAST)stateFastTime-=TIME_SYNC_FAST;
	while(stateSlowTime>TIME_SYNC_SLOW)stateSlowTime-=TIME_SYNC_SLOW;

}

UBOOL MMOTargetManager::UpdateState(FLOAT DeltaTime)
{

	//time update
	
	stateFastTime	+=	DeltaTime;
	stateSlowTime	+=	DeltaTime;
	if(statePressedTime)statePressedTime +=	DeltaTime;
	if(stateAiCdTime>0.0f)stateAiCdTime	+=	DeltaTime;	
	if(stateSetFocusDelayTime>0.0f)
		stateSetFocusDelayTime += DeltaTime;

	if(stateAiInSafeZoneTime>0.0f)
		stateAiInSafeZoneTime -= DeltaTime;

	if(TimeQTE>0.0f)
		TimeQTE -= DeltaTime;

	if(	AresFlag>0) 	
		AresDeltaTime -= DeltaTime;

	//warning update
	stateWarningTime+=  DeltaTime;

	//control update
	stateMainPlayer =	MMOFacade::getSingleton().GetMainPlayer();
	stateController	=	MMOFacade::getSingleton().GetMainController();
	if(stateMainPlayer==NULL)return FALSE;
	if(stateController==NULL)return FALSE;

	stateStallState = stateMainPlayer->eventIsStallState();

	//Message freeze time update
	if(stateController && stateController->WaitingForTheAbilityRespond>0)
	{
		stateAbilityRespondTime += DeltaTime;
		if(stateAbilityRespondTime>0.2f)
		{
			stateController->WaitingForTheAbilityRespond = 0;
			stateAbilityRespondTime = 0;
		}
	}

	if(stateTargetMMOActor && stateTargetMMOActor!=stateLastTargetMMOActor)
	{
		stateSetFocusDelayTime = 0.01f;
	}
	else if(stateTargetMMOActor!=stateLastTargetMMOActor)
	{
		stateSetFocusDelayTime = -1.0f;
	}


	//delayed request for focus details
	if(stateSetFocusDelayTime>1.0f)
	{
		stateSetFocusDelayTime = -1.0f;
		if(stateTargetMMOActor)
			stateController->SendRequestSetFocus(stateTargetMMOActor->ServerID);
	}

	//determine whether it can pass through the UI
	MMOFacade::UpdateCaptureInput();

	//pick up mouse click info
	LastMouseHitNavMeshLocation = stateController->MouseHitNavMeshLocation;
	// zhaojun@snda
	UBOOL bNavMeshPicked = stateController->NativeGetPickPointNavMesh();
	if (!bNavMeshPicked)
	{
		stateController->MouseHitNavMeshLocation = stateController->MouseHitWorldLocation;
	}
	// ~zhaojun@snda
	stateController->NativeGetPickPoint();

	UBOOL CaptureInput = MMOFacade::GetCaptureInput();
	if(CaptureInput==FALSE)
	{
		SetTargetActor(stateController->GetTraceActor());
	}
	else
	{
		SetTargetActor(NULL);
	}
	//Update triggered abilities
	stateAbility=OptimizeAbility(stateDesireAbility); 
	if(stateAbility&&stateAbility->ClientCommonAttribute.PickTarget)
	{
		statePickTarget = TRUE;
	}
	else
	{
		statePickTarget = FALSE;
	}
	//Whether the ability is used
	stateAbilityEnable	= stateController->CanUseAnyAbility(stateAbility);
	//Not enough mana
	if(stateAbility && stateMainPlayer && stateAbility->CanUseManaCheck(stateMainPlayer)==FALSE)
	{
		stateAbilityEnable = FALSE;
	}

	//if(stateAbility && MMOFacade::getSingleton().GetMyCurrentMP() < stateAbility->SpellCommonAttribute.InitialCostValue + stateAbility->SpellCommonAttribute.InitialCostValuePct*MMOFacade::getSingleton().GetMyMaxMP()/10000 )
	//{
	//	stateAbilityEnable = FALSE;
	//}

	//whether can move
	stateMoveEnable		= stateMainPlayer->CanMove();
	//a movement key is pressed
	stateMoveKeyDown	= (MMOUI::GetUIManager()->WSADKey[0]!=EMCK_Empty);
		
	if(	(stateDesireMMOActor && MMOFacade::getSingleton().GetCharacter(stateDesireMMOActor->ServerID)==NULL ) ||	//if it has been released
		(stateDesireMMOActor && stateDesireMMOActor->eventIsDead())		)										//if already dead
	{
		stateDesireActor	=	NULL;		
		stateDesireMMOActor	=	NULL;
		//stateDesireMMOPlayer=	NULL;
		stateDesireNPC=			NULL;
		stateDesireNPCTempleteID=0;
		//stateDesireItem=		NULL;
		stateDesireAttackble=	FALSE;
	}

	//whether the pathfinding points are non-straight or optimized
	statePathOptimize	= stateController->bLinearMovement; /*&& stateController->bOptimizeTargetPos==FALSE*///zhaojun@snda ;

	//update states such as shift, left mouse button down
	stateShiftDown	=stateController->eventIsLeftShiftDown();
	stateLeftBtnDown=stateController->bLeftMousePressed;

	if(stateTargetMMOActor==NULL)
	{
		stateTargetPos = OptimizeTargetPos(stateController->MouseHitNavMeshLocation/*MouseHitWorldLocation*/);
	}
	else
	{
		stateTargetPos = stateController->MouseHitNavMeshLocation;
		//stateTargetPos = stateController->MouseHitWorldLocation;
	}

	stateInterrupt = FALSE;
	return TRUE;
}

void MMOTargetManager::ResetSafeZoneDelayTime()
{
	stateAiInSafeZoneTime = 2.0f;
}

void MMOTargetManager::SetDesirePos(FVector Pos, INT NPCTempleteID)
{
	stateDesirePos = Pos;
	stateDesireNPCTempleteID = NPCTempleteID;
	SetDesireTarget(NULL,NULL);
}

void MMOTargetManager::SetDesireTarget(UMMOAbility* Ability,AActor* Target,UBOOL bLeftMouseBtn)
{
	stateMainPlayer =	MMOFacade::getSingleton().GetMainPlayer();
	stateController	=	MMOFacade::getSingleton().GetMainController();

	if(stateMainPlayer==NULL)return;
	if(stateController==NULL)return;

	stateLastDesireMMOActor = stateDesireMMOActor;

	stateDesireActor	=	Target;		
	stateDesireMMOActor	=	Cast<AMMOActor>(Target);
	//stateDesireMMOPlayer=	Cast<AMMOPlayer>(Target);
	stateDesireNPC=			Cast<ANPC>(Target);
	//stateDesireItem=		Cast<AMMODropActor>(Target);
	stateDesireAttackble	= MMOFacade::getSingleton().IsAttackable(stateMainPlayer,stateDesireMMOActor);
	stateDesireAbility	= Ability;


	//stateAbilityRespondTime = -1.0f;

	if(stateDesireActor)
	{
		if(stateDesireNPC && stateDesireNPC->Template)
		{
			stateDesireNPCTempleteID = stateDesireNPC->Template->GetTemplateID();
		}
		else
		{
			stateDesireNPCTempleteID = 0;
		}
	}

	stateAttackMMOActor	= stateDesireMMOActor;

	SetFocusActor(Target);
	//hide mouse click cursor
	if(stateController->TargetLocationMarker)
	{
		stateController->TargetLocationMarker->Hide();
	}
	//if(stateDesireMMOActor && stateLastDesireMMOActor!=stateDesireMMOActor)
	//{
	//	stateController->SetSelectTarget(stateDesireActor);
	//}

	UpdateState(0.0f);

	stateLeftBtnDown=	bLeftMouseBtn;
	statePressedTime=	0.0f;						//accumulated key press time;
	stateFastTime	=	TIME_SYNC_FAST+EPSILON;		//fast loop timer;
	stateSlowTime	=	TIME_SYNC_SLOW+EPSILON;		//slow loop timer;


	if(stateDesireMMOActor==NULL && stateLeftBtnDown && stateShiftDown==FALSE && stateLeftBtnDown)
	{
		stateCtrlState	=	CS_MOVE;
	}
	else{
		stateCtrlState	=	CS_BATTLEMOVE;
	}

	if(stateLeftBtnDown)//if triggered by left button
	{
		stateLeftBtnAbility =Ability;		//ability equipped on left button
	}

	stateDesireState=	DS_TRIGGER;

//	warnf(NAME_Log, TEXT("=============================[MMOTargetManager::SetDesireTarget]"));
}

void MMOTargetManager::ClearDesireTarget(UMMOAbility* Ability)
{
	//End the abilities that need to end
	if( Ability && stateController)
	{
		if(Ability->ClientCommonAttribute.bRequestCancelOnRelease)
		{
			stateController->eventAddCmdCancelAbility(Ability);
		}
	}


	stateAbility		=	NULL;
	//stateTargetMMOActor	=	NULL;
	//stateDesireMMOActor	=	NULL;
	stateCtrlState		=	CS_IDLE;
	stateDesireState	=	DS_RELEASE;

}

AActor*	MMOTargetManager::GetDesireActor()
{
	return stateDesireActor;
}

void	MMOTargetManager::SetTargetActor(AActor* TargetActor)
{
	stateLastTargetActor=	stateTargetActor;
	stateLastTargetMMOActor = stateTargetMMOActor;
	stateLastTargetMMOActorID	= stateTargetMMOActorID;

	stateTargetActor	=   TargetActor;
	stateTargetMMOActor	=	Cast<AMMOActor>(stateTargetActor);
	if(stateTargetMMOActor )
	{
		stateTargetMMOActorID = stateTargetMMOActor->ServerID;
	}
	else
	{
		stateTargetMMOActorID = 0;
	}

	stateTargetMMOPlayer=	Cast<AMMOPlayer>(stateTargetActor);	
	stateTargetNPC		=	Cast<ANPC>(stateTargetActor);
	//stateTargetItem		=	Cast<ADropItemActor>(stateTargetActor);
	stateTeleportOrig	=	Cast<ATeleportOrig>(stateTargetActor);	
	stateTeleportNetNode=	Cast<ATeleportNetNode>(stateTargetActor);	
	stateDoodadActor	=   Cast<AMMODoodadActor>(stateTargetActor);
	stateAttackble		=	MMOFacade::getSingleton().IsAttackable(stateMainPlayer,stateTargetMMOActor);
	stateIsDead			=	stateTargetMMOActor?stateTargetMMOActor->eventIsDead():FALSE;

	if(stateTeleportOrig || stateTeleportNetNode){
		stateIsShowTips = TRUE;
	}
}

AMMOActor*	MMOTargetManager::GetTargetMMOActor()
{
	//if (stateTargetMMOActorID>0)
	//{
	//	STLInt2IntMap* pCharacterMap = MMOFacade::getSingleton().GetCharacterMap();
	//	if(pCharacterMap){
	//		STLInt2IntMap::iterator pos = pCharacterMap->find(stateTargetMMOActorID);
	//		if (pos != pCharacterMap->end())
	//		{
	//			return Cast<AMMOPlayer>(UObject::GetIndexedObject( pos->second ));
	//		}
	//	}
	//}
	return NULL;
}

AMMOActor*	MMOTargetManager::GetLastTargetMMOActor()
{
	//if (stateLastTargetMMOActorID>0)
	//{
	//	STLInt2IntMap* pCharacterMap = MMOFacade::getSingleton().GetCharacterMap();
	//	if(pCharacterMap){
	//		STLInt2IntMap::iterator pos = pCharacterMap->find(stateLastTargetMMOActorID);
	//		if (pos != pCharacterMap->end())
	//		{
	//			return Cast<AMMOPlayer>(UObject::GetIndexedObject( pos->second ));
	//		}
	//	}
	//}
	return NULL;
}


AMMOActor*	MMOTargetManager::GetSelectMMOActor()
{
	return stateSelectMMOActor;
}

void	MMOTargetManager::SetSelectMMOActor(AMMOActor* SelectActor)
{
	stateSelectMMOActor = SelectActor;
}


void MMOTargetManager::SetFocusActor(AActor* FocusActor)
{
	stateFocusActor		= FocusActor;		
	stateFocusMMOActor	= Cast<AMMOActor>(stateFocusActor);
	stateFocusMMOPlayer = Cast<AMMOPlayer>(stateFocusActor);
}

AActor*	MMOTargetManager::GetFocusActor()
{
	return stateFocusActor; 
}

void	MMOTargetManager::ClearActor(AActor* Actor)
{
	if(Actor && Actor==stateFocusActor)
	{
		SetFocusActor(NULL);
	}
	if(Actor && Actor==stateTargetActor)
	{
		UpdateState(0.0f);
	}
	if(Actor && Actor==stateDesireActor)
	{
		stateDesireActor	= NULL;
		stateDesireMMOActor = NULL;
		//stateDesireMMOPlayer= NULL;
		stateDesireNPC		= NULL;
		//stateDesireItem		= NULL;
	}
	if(Actor && Actor==stateLastTargetActor)
	{
		stateLastTargetActor	= NULL;
		stateLastTargetMMOActor = NULL;
	}
	if(Actor && Actor==stateLastDesireMMOActor)
	{
		stateLastDesireMMOActor = NULL;
	}
	if(Actor && Actor==stateSelectMMOActor)
	{
		stateSelectMMOActor = NULL;
	}


}



FVector MMOTargetManager::OptimizeTargetPos(FVector TargetPos)
{
	//prevent click point being too close to character, which could cause sliding
	FVector location = stateMainPlayer->Location - FVector(0.0,0.0,stateMainPlayer->CylinderComponent->CollisionHeight);
	FLOAT dis = (TargetPos-location).Size2D();
	FLOAT minDis = stateMainPlayer->CylinderComponent->CollisionRadius*3.0;
	if( dis<minDis )
	{
		TargetPos = TargetPos + (TargetPos-location)*(minDis-dis)/dis ;
	}
	return TargetPos;
}

UMMOAbility*	MMOTargetManager::GetDefaultAbility()
{
	//default ability
	switch( MMOFacade::getSingleton().GetMainActor()->GetClassId() )
	{
	case EPCT_PlayerClassA://Warrior
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(1000, 0);
		break;
	case EPCT_PlayerClassB://Mage
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(2500, 0);
		break;
	case EPCT_PlayerClassC://Assassin
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(1500, 0);
		break;
	case EPCT_PlayerClassD://Archer
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(2000, 0);
		break;
	case EPCT_PlayerClassE://Taoist
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(3000, 0);
		break;
	case EPCT_PlayerClassG://Lancer (1.05)
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(1200, 0);
		break;
	case EPCT_PlayerClassH://Sorcerer (1.05)
		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(3200, 0);
		break;
	default:
		return NULL;
	}
}


UMMOAbility* MMOTargetManager::OptimizeAbility(UMMOAbility* Ability)
{
	if(Ability&&Ability->ClientCommonAttribute.UseDefaultAbility)
	//if(Ability)
	{
		//not enough mana	//copy from server by JimZhang
		if(stateMainPlayer &&
			Ability->CanUseManaCheck(stateMainPlayer)==FALSE)
		//if(MMOFacade::getSingleton().GetMyCurrentMP() < Ability->SpellCommonAttribute.InitialCostValue + Ability->SpellCommonAttribute.InitialCostValuePct*MMOFacade::getSingleton().GetMyMaxMP()/10000 )
		//if(MMOFacade::getSingleton().GetMyCurrentMP()<10)
		{
			return GetDefaultAbility();
		}
	}
	return Ability;
}

INT	 MMOTargetManager::OptimizeTargetID(FRotator Rotator,FVector Location,FLOAT Radius,SEARCH_TYPE SType)
{
	//STLInt2IntMap* MMOActorMap = MMOFacade::getSingleton().GetCharacterMap();
	//FLOAT MaxDistance = Radius;
	//FLOAT MinDistance = 0.0;
	//FLOAT MaxWeightDistance = Radius*10.0f;
	//FLOAT CurDistance = 0;
	//INT   ObjID		  = 0;

	//for (STLInt2IntMap::iterator Itor=MMOActorMap->begin();  Itor!=MMOActorMap->end(); Itor++ )
	//{
	//	AMMOActor* pMMOActor = Cast<AMMOActor>(UObject::GetIndexedObject(Itor->second));
	//	if(pMMOActor==NULL)continue;

	//	if(MMOFacade::getSingleton().IsAttackable( stateMainPlayer ,pMMOActor)==FALSE) continue;

	//	if(SType==ST_INNER_DAMAGEABLE ||SType==ST_CLOSED_DAMAGEABLE)
	//	{
	//		if(pMMOActor->GetClass()->ImplementsInterface( UIAbilityDamageable::StaticClass())==FALSE)
	//		{
	//			continue;
	//		}
	//	}

	//	if(pMMOActor->eventIsDead())
	//	{
	//		continue;
	//	}

	//	FVector ObjVector = pMMOActor->Location-Location;
	//	CurDistance = ObjVector.Size2D();
	//	if(CurDistance<MaxDistance && CurDistance>MinDistance )
	//	{
	//		//compute direction coefficient-------------------------------------------
	//		ObjVector.Z = 0.0f;
	//		FRotator ObjRotator = ObjVector.Rotation();
	//		INT    Weight		= abs(ObjRotator.Yaw - Rotator.Yaw);
	//		if(Weight*2>MAXWORD)
	//		{
	//			Weight =  MAXWORD-Weight;
	//		}
	//		FLOAT WeightLv = FLOAT(Weight*2000/MAXWORD) * 0.001f;
	//		FLOAT CurWeightDistance = CurDistance*(1.0f+WeightLv*3.0f);
	//		//-------------------------------------------------------------


	//		if(CurWeightDistance<MaxWeightDistance){
	//			MaxDistance = CurDistance; 
	//			MaxWeightDistance = CurWeightDistance;
	//			ObjID		= Itor->second;
	//			if(SType==ST_INNER_ALL ||SType==ST_INNER_DAMAGEABLE ||SType==ST_INNER_FRIENDLY)
	//			{
	//				//break;
	//			}
	//		}
	//	}
	//}

	//return ObjID;
	return 0;
}

FVector MMOTargetManager::OptimizeMoveOffset(BYTE* pMode )
{
	FVector vec = FVector(0.0,0.0,0.0);
	FLOAT	scale = 2.5;
	for(int i=0;i<4;i++)
	{
		switch (*(pMode+i))
		{
		case EMCK_Left://left
			vec.X += 64.0;
			vec.Y += -64.0;
			vec.Z += 0.0;
			break;
		case EMCK_Right://right
			vec.X += -64.0;
			vec.Y += 64.0;
			vec.Z += 0.0;
			break;
		case EMCK_Front://front
			vec.X += 64.0;
			vec.Y += 64.0;
			vec.Z += 0.0;
			break;
		case EMCK_Back://back
			vec.X += -64.0;
			vec.Y += -64.0;
			vec.Z += 0.0;
			break;
		default:
			vec.X += 0.0;
			vec.Y += 0.0;
			vec.Z += 0.0;
		}
	}

	return vec*scale;
}

//---------------------------------------------------------------------------------------------------------------------------------------
//if shift is held, cast ability in place
UBOOL MMOTargetManager::stateShiftAbility()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateCtrlState==CS_BATTLEMOVE && 
		stateShiftDown==TRUE && 
		stateAbility!=NULL  && 
		stateAbilityEnable	)
	{
		//Cast ability directly
		stateController->eventClearAllCmd();
		if(stateTargetMMOActor){
			stateController->eventAddCmdUseAbility(stateAbility,stateTargetMMOActor, stateTargetMMOActor->Location);
		}
		else
		{
			stateController->eventAddCmdUseAbility(stateAbility,NULL, stateController->MouseHitWorldLocation);
		}
		stateAbilityRespondTime = 0;
		stateInterrupt = TRUE;
		return TRUE;
	}
	return FALSE;
}

//cast ability in place-------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateStayAndUseAbility()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		//stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateCtrlState==CS_BATTLEMOVE && 
		stateAbility!=NULL &&
		stateController &&
		stateMainPlayer	&&
		stateAbilityEnable &&
		stateShiftDown==FALSE &&
		statePickTarget == FALSE &&
		(stateDesireMMOActor==FALSE || stateDesireAttackble)
		)
	{
		warnf(NAME_Warning, TEXT("=============================[MMOTargetManager::stateStayAndUseAbility]"));

		if(stateAbility->SpellCommonAttribute.CanMoveInPrepare)
		{
			INT ServerID = stateTargetMMOActor?stateTargetMMOActor->ServerID:0 ;
			//stateController->RequestUseAbility(ServerID, stateAbility->SlotHotKeyID, stateAbility->SpellCommonAttribute.Level, stateTargetPos );
			stateInterrupt = TRUE;
		}
		else{
			//Cast ability directly
			stateController->eventClearAllCmd();
			//stateController->eventAddCmdUseAbility(stateAbility,stateDesireMMOActor,stateTargetPos);
			stateAbilityRespondTime = 0;
			stateInterrupt = TRUE;
		}
		return TRUE;
	}
	return FALSE;
}

//chase and cast ability-------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateMoveToAndUseAbility()
{	
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		//stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateCtrlState==CS_BATTLEMOVE && 
		stateAbility!=NULL &&
		stateController &&
		stateMainPlayer	&&
		stateAbilityEnable &&
		stateShiftDown==FALSE  &&
		stateDesireMMOActor && 
		stateDesireAttackble &&
		stateIsDead==FALSE
		)
	{
		stateController->eventClearAllCmd();
		float dis = stateAbility->SpellCommonAttribute.MaxDistance;
		if(stateAbility->SpellCommonAttribute.UseWeaponRange)
		{
			dis = stateMainPlayer->CurrentAttackRange;
		}
		if((stateMainPlayer->Location-stateDesireMMOActor->Location).Size() >= dis)
		{
			if(stateMoveEnable==FALSE)return FALSE;
			//Chase over, end when within one casting range
			stateController->eventAddCmdMoveToActor(stateDesireMMOActor,stateDesireMMOActor->Location,dis*0.9);
		}
		if(stateAbilityEnable)
		{
			//Cast ability
			stateController->eventAddCmdUseAbility(stateAbility,stateDesireMMOActor,stateDesireMMOActor->Location,FALSE);
			stateAbilityRespondTime = 0;
		}

		stateInterrupt = TRUE;
		return TRUE;

	}
	return FALSE;
}

//search for enemy and cast ability-------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateSearchAndUseAbility()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		//stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateCtrlState==CS_BATTLEMOVE && 
		stateAbility!=NULL &&
		stateController &&
		stateMainPlayer	&&
		stateDesireMMOActor==NULL && //no initial target
		stateAbilityEnable &&
		statePickTarget &&
		stateShiftDown==FALSE  )
	{

		if(	stateAttackMMOActor && 
			stateController->GetCmdNum(CMD_ABILITY)==0 &&
			MMOFacade::getSingleton().IsAttackable(stateMainPlayer,stateAttackMMOActor) )
		{
			stateController->eventClearAllCmd();
			if((stateMainPlayer->Location-stateAttackMMOActor->Location).Size() >= stateAbility->SpellCommonAttribute.MaxDistance)
			{
				if(stateMoveEnable==FALSE)return FALSE;
				//Chase over, end when within one casting range
				stateController->eventAddCmdMoveToActor(stateAttackMMOActor,stateAttackMMOActor->Location,stateAbility->SpellCommonAttribute.MaxDistance*0.9);
			}
			if(stateAbilityEnable)
			{
				//Cast ability
				stateController->eventAddCmdUseAbility(stateAbility,stateAttackMMOActor,stateAttackMMOActor->Location,FALSE);
				stateAbilityRespondTime = 0;
			}
			return TRUE;
		}
	}

	return FALSE;
}

//walk close and interact----------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateMoveToAndInteract()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateCtrlState>=CS_MOVE && 
		stateController &&
		stateMainPlayer	&&
		stateShiftDown==FALSE && 
		stateDesireActor !=NULL &&	//must have a specified target
		stateDesireAttackble == FALSE
		) 
	{

		TScriptInterface<IIInteractableActor> interactableActor = ScriptInterfaceCast<IIInteractableActor>(stateDesireActor);
		if( interactableActor || stateDesireNPCTempleteID>0 ){
			//NPC interact
			stateController->eventClearAllCmd();


			FLOAT interactDist = 0.0f;
			if(interactableActor)
			{
				interactDist = interactableActor->GetMaxInteractRadius();

				if((stateMainPlayer->Location - interactableActor->GetInteractActorLocation()).Size() >= interactDist)
				{
					//Chase over,
					stateController->eventAddCmdMoveToActor(stateDesireNPC, interactableActor->GetInteractActorLocation(), interactDist * 0.8, stateDesireNPCTempleteID);
				}
			}
			else if(stateDesireNPCTempleteID > 0)
			{
				interactDist = stateController->InteractDistance;
				//Chase over,
				stateController->eventAddCmdMoveToActor(NULL, FVector(0, 0, 0),  interactDist * 0.8, stateDesireNPCTempleteID);
			}

			//Perform interaction
			//stateController->SetSelectTarget(stateDesireActor);
			stateController->eventAddCmdInteract(interactableActor, 0, stateDesireNPCTempleteID);
			stateCtrlState = CS_IDLE;
			return TRUE;
		}
		
// 		ATeleportOrig* teleportOrig = Cast<ATeleportOrig>( stateDesireActor );
// 		if( teleportOrig != NULL ){
// 			//teleport interact
// 			stateController->eventClearAllCmd();
// 			stateController->eventAddCmdMoveToActor(teleportOrig,teleportOrig->Location, teleportOrig->TeleportClientRange*0.4,0);
// 
// 			//
// 			stateController->eventAddCmdInteract(teleportOrig,0,0);
// 			stateCtrlState=CS_IDLE;
// 			return TRUE;
// 		}
	}
	return FALSE;
}


//walk close and set up stall----------------------------------------------------------------------------------------------------------------------------------
// UBOOL MMOTargetManager::stateMoveToAndStall()
// {
// 	if(	stateInterrupt==FALSE &&
// 		stateStallState==FALSE &&
// 		stateMoveEnable &&
// 		stateMoveKeyDown == FALSE &&
// 		stateCtrlState>=CS_MOVE && 
// 		stateController &&
// 		stateMainPlayer	&&
// 		stateShiftDown==FALSE && 
// 		stateDesireMMOPlayer  &&
// 		stateDesireMMOPlayer->MoveType==EMT_InStall)   //stall interaction
// 	{
// 		stateController->eventClearAllCmd();
// 
// 		if(stateDesireMMOActor){
// 			if((stateMainPlayer->Location-stateDesireMMOActor->Location).Size() >= stateController->InteractDistance)
// 			{
// 				//chase over,
// 				stateController->eventAddCmdMoveToActor(stateDesireMMOActor,stateDesirePos, stateController->InteractDistance*0.9,stateDesireNPCTempleteID);
// 			}
// 		}
// 
// 		//perform interaction
// 		//stateController->SetSelectTarget(stateDesireActor);
// 		stateController->eventAddCmdInteract(stateDesireMMOActor,0,stateDesireNPCTempleteID);
// 		stateCtrlState=CS_IDLE;
// 		return TRUE;
// 	}
// 	return FALSE;
// }


//walk close and pick up item
// UBOOL MMOTargetManager::stateMoveToPickItem()
// {
// 	if(	stateInterrupt==FALSE &&
// 		stateStallState==FALSE &&
// 		stateCtrlState>=CS_MOVE && 
// 		stateMoveKeyDown == FALSE &&
// 		stateController &&
// 		stateMainPlayer	&&
// 		stateShiftDown==FALSE && 
// 		stateDesireItem ) 
// 	{
// 		stateController->eventClearAllCmd();
// 
// 		if((stateMainPlayer->Location-stateDesireItem->Location).Size()
// 			>= stateController->MaxPickDistance)
// 		{
// 			//chase over,
// 			stateController->eventAddCmdMoveToActor(
// 				stateDesireItem, stateDesireItem->Location,
// 				stateController->MaxPickDistance * 0.7f);
// 		}
// 		//perform pickup
// 		stateController->eventAddCmdPickUpItem(stateDesireItem);
// 		stateCtrlState=CS_IDLE;
// 		return TRUE;
// 	}
// 	return FALSE;
// }


//walk to target point stateTargetPos; this is a mouse-converted point found in real time in Diablo 3 style, adding flexibility to mouse control; in short, movement under mouse control
UBOOL MMOTargetManager::stateMoveToPoint()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		//stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		//(stateTargetMMOActor==NULL || stateIsDead) &&
		(statePickTarget || stateLeftBtnDown) &&
		//(stateAbility!=NULL ||(stateAbility==NULL&&stateLeftBtnDown))	&&
		stateController &&
		stateMainPlayer	&&
		stateShiftDown==FALSE && 
		stateAttackMMOActor == NULL &&	//nothing attackable
		//stateController->GetCmdNum(CMD_ABILITY)==0 && //no ability command
		stateCtrlState>=CS_MOVE	)
	{
		if(statePathOptimize)//quick refresh depending on whether the target's pathfinding data is a straight line
		{
			if(stateFastTime > TIME_SYNC_FAST )
			{
				stateController->eventClearAllCmd();
				stateController->SetTargetLocationMarker(stateTargetPos,stateDesireMMOActor);
				stateController->eventAddCmdMoveToPoint( stateTargetPos,TRUE );
			}
			else //turn only
			{
				if(stateMoveEnable){//only turn in real time when movement is allowed
					//update facing in real time
					FVector Value;
					FVector Des =  stateTargetPos;
					if(stateMainPlayer->eventGetValidPoint(Des, Value))
					{
						Des = Value;
						Des.Z += stateMainPlayer->GetCollisionHeight();
					}

					Value = Des - stateMainPlayer->Location;
					Value = Value.SafeNormal2D();

					FRotator Rot = Value.Rotation();
					stateMainPlayer->LockDesiredRotation(false, true);
					stateMainPlayer->SetDesiredRotation(Rot, false, true);
				}
			}
		}
		else
		{
			if(stateSlowTime > TIME_SYNC_SLOW )
			{
				stateController->eventClearAllCmd();
				stateController->SetTargetLocationMarker(stateTargetPos,stateDesireMMOActor);
				stateController->eventAddCmdMoveToPoint( stateTargetPos,TRUE );
			}
		}

		//if(stateCtrlState==CS_BATTLEMOVE && stateLeftBtnDown==TRUE)
		//{
		//	//Diablo 3 special method: left click with no target automatically switches to displacement state
		//	stateCtrlState=CS_MOVE;
		//}

		return TRUE;
	}
	return FALSE;
}

//walk to specified coordinate stateDesirePos; this is an initially set target point, unrelated to the mouse; in short, movement toward a preset target point
UBOOL MMOTargetManager::stateMoveToDesirePoint()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateMoveEnable &&
		stateMoveKeyDown == FALSE &&
		stateDesireActor==NULL &&
		stateDesireAbility==NULL &&
		stateController &&
		stateDesireNPCTempleteID==0 &&
		stateCtrlState>=CS_MOVE	)
	{
		stateController->eventClearAllCmd();
		stateController->eventAddCmdMoveToPoint( stateDesirePos,TRUE );
		stateCtrlState=CS_IDLE;
		return TRUE;
	}
	return FALSE;
}

//keyboard movement control
UBOOL MMOTargetManager::stateMoveByKey()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateMoveEnable &&
		stateMoveKeyDown &&
		stateController &&
		stateMainPlayer	&&
		stateShiftDown==FALSE && 
		stateFastTime > TIME_SYNC_FAST &&
		stateShiftDown==FALSE  )
	{
		stateController->eventClearAllCmd();
		//walk over
		FVector Pos = stateMainPlayer->Location +OptimizeMoveOffset(MMOUI::GetUIManager()->WSADKey);
		stateController->eventAddCmdMoveToPoint(Pos,TRUE);
	}
	return FALSE;
}

//show third-party portrait on selection
UBOOL MMOTargetManager::stateShowInfo()
{
	//Current target
	if(	stateLastTargetMMOActor!=stateTargetMMOActor )
	{
		MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_CurrentTarget, stateTargetMMOActor, stateSelectMMOActor);
		MMOUI::PostGameEvent(&evt);
	}

	//select target
	if( stateDesireMMOActor!=stateLastDesireMMOActor )
	{
		//if(stateDesireAttackble){
		//	MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_DesiredTarget, stateTargetMMOActor, NULL);
		//	MMOUI::PostGameEvent(&evt);
		//}
		//else
		{
			MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_DesiredTarget, stateTargetMMOActor, stateSelectMMOActor);
			MMOUI::PostGameEvent(&evt);
		}
	}
	return FALSE;
}

//selected object tips display
UBOOL MMOTargetManager::stateShowTips()
{
	if (MMO::GetCurrentGameState() != EGameState_InGame)
	{
		//stateLastTargetActor = NULL;
		//stateTargetActor	 = NULL;
		return FALSE;
	}

	//Current target
	if(	stateLastTargetActor != stateTargetActor ){

		if(stateTeleportOrig)
		{
			//MMOUI::GetUIManager()->delegateShowFreeStyleTips(  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeleportOrig.Name") ), stateTeleportOrig->ToSceneName );
			MMOUI::GetUIManager()->delegateShowFreeStyleTips( TEXT(""), stateTeleportOrig->ToSceneName, CM_TIPS_Style2, 1 );
			//Show effect
			stateTeleportOrig->ActiveFoucsEffect( TRUE );
			stateIsShowTips = TRUE;
		}
		//else if(stateTeleportNetNode )
		//{
		//	MMOUI::GetUIManager()->delegateShowFreeStyleTips( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TeleportNet.Name") ), stateTeleportNetNode->DisplayName );
		//	stateIsShowTips = TRUE;
		//}
        else if (stateDoodadActor)
        {
			const UDoodad* doodadTemp = stateDoodadActor->DoodadTemplate;
			if(doodadTemp && doodadTemp->DoododNameDisplay)
			{
				FString doodadTips;
				//=  MMOUI::TrimNumber(doodadTemp->DoodadName.ToString());
								
				// prompt item requirement
				if(doodadTemp->RequireItemID > 0)
				{
					//doodadTips += TEXT("<br>");
					FString locReqItem;
					
					if(doodadTemp->CostItem)
					{
						locReqItem = MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.RequireItem"));
					}
					else
					{
						locReqItem = MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.CostItem"));
					}

					locReqItem = locReqItem.TrimQuotes();
					doodadTips += locReqItem + TEXT("<br>");

					UItem* item = MMOFacade::getSingleton().FindItemTemplate(doodadTemp->RequireItemID);
					if(item)
					{
						doodadTips += UITextParser::HTMLFormatItemColorString(item);
					}
					else
					{
						doodadTips += FString::Printf(TEXT("Item:%d"), doodadTemp->RequireItemID);
					}
				}

				// has an unlock restriction; whether or not it can be unlocked, i.e. whether VarValue is correct or not, always show in TIPS
				if(stateDoodadActor->IsVarRequired() && !doodadTemp->TipsText.IsEmpty())
				{
					doodadTips += MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.RequireVar"));
					doodadTips += TEXT("<br>");
					doodadTips += doodadTemp->TipsText;
				}

				//MMOUI::GetUIManager()->delegateShowFreeStyleTips( MMOUI::TrimNumber(doodadTemp->DoodadName.ToString()), doodadTips );
				MMOUI::GetUIManager()->delegateShowFreeStyleTips( TEXT(""), MMOUI::TrimNumber(doodadTemp->DoodadName.ToString()), CM_TIPS_Style3, 1 );

				stateIsShowTips = TRUE;
			}
        }
        else if (stateIsShowTips)
        {
            MMOUI::GetUIManager()->delegateHideTips();

			//handle closing the teleport point effect
			ATeleportOrig *lastTeleportOrig	= Cast<ATeleportOrig>(stateLastTargetActor);
			if( lastTeleportOrig != NULL ){
				lastTeleportOrig->ActiveFoucsEffect( FALSE );
			}

            stateIsShowTips = FALSE;
        }
	}

	return FALSE;
}

//selected object under-foot cursor display
UBOOL	MMOTargetManager::stateShowSelectMark()
{
	if(	stateController &&
		stateMainPlayer	&&
		stateSelectMMOActor!=stateDesireMMOActor &&
		stateLastDesireMMOActor!=stateDesireMMOActor &&
		stateSelectMMOActor!= stateDesireMMOActor )
	{
		//if(stateDesireMMOActor && stateSelectMMOActor!= stateDesireMMOActor){
			stateSelectMMOActor = stateDesireMMOActor;
			stateController->SetSelectTarget(stateSelectMMOActor);
			MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_DesiredTarget, stateTargetMMOActor, stateSelectMMOActor);
			MMOUI::PostGameEvent(&evt);
		//}
	}


	if( stateSelectMMOActor &&
		stateMainPlayer	&&
		stateController &&
		(stateSelectMMOActor->Location-stateMainPlayer->Location).Size2D() > 1000 )
	{
		stateSelectMMOActor = NULL;
		stateDesireMMOActor = NULL;
		stateController->SetSelectTarget(stateSelectMMOActor);
		MMOGameEvent_TargetChange evt(MMOGameEvent_TargetChange::ETCT_DesiredTarget, stateTargetMMOActor, stateSelectMMOActor);
		MMOUI::PostGameEvent(&evt);
	}

	return FALSE;
}


//auto find monster. (in combat displacement, after killing a monster, automatically find the nearby monster with the highest attack weight)
UBOOL	MMOTargetManager::stateSearchEnemy()
{
	//cancel enemy target on death
	if( stateAttackMMOActor && (stateAttackMMOActor->eventIsDead() || stateAttackMMOActor->IsPendingKill()))
	{
		stateAttackMMOActor = NULL;
	}

	//cancel enemy target when out of range
	if( stateAttackMMOActor &&
		stateAttackMMOActor!=stateDesireMMOActor &&
		stateMainPlayer &&
		stateAbility &&
		(stateMainPlayer->Location - stateAttackMMOActor->Location).Size2D() > stateAbility->SpellCommonAttribute.SearchDistance )
	{
		stateAttackMMOActor = NULL;
	}

	//only auto find monsters in combat displacement
	if(	stateAttackMMOActor==NULL &&
		stateDesireMMOActor==NULL &&
		stateMainPlayer &&
		stateAbility &&
		stateCtrlState==CS_BATTLEMOVE)
	{

		//search
		INT AttackMMOActorIndex = OptimizeTargetID(stateMainPlayer->Rotation,stateMainPlayer->Location,stateAbility->SpellCommonAttribute.SearchDistance,ST_CLOSED_DAMAGEABLE);
		warnf(NAME_Warning, TEXT("=============================[MMOTargetManager::stateSearchEnemy]"));

		if(AttackMMOActorIndex>0){

			stateAttackMMOActor	= NULL;//Cast<AMMOActor>(UObject::GetIndexedObject(AttackMMOActorIndex));
		}
	}

	return FALSE;
}

//auto switch key state
UBOOL	MMOTargetManager::stateAIContral()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateCtrlState==CS_IDLE &&
		stateMoveEnable &&
		stateLeftBtnAbility &&
		stateLeftBtnAbility!=stateDesireAbility &&
		stateLeftBtnDown )
	{

		warnf( NAME_Error,  TEXT("stateAIContral!") );
		SetDesireTarget(stateLeftBtnAbility,stateTargetActor,TRUE);
		return TRUE;
	}
	return FALSE;
}


//auto detect sheathing weapon in safe zone
UBOOL	MMOTargetManager::stateAIInSafeZone()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateMainPlayer )
		//&& stateMainPlayer->IsInSafeZone &&
		//stateMainPlayer->ActorAttributes->byCombatPoseOn==FALSE )
	{
		//if(stateAbility && stateCtrlState==CS_BATTLEMOVE){
		//	ResetSafeZoneDelayTime();
		//}
		//else if(stateCtrlState<=CS_MOVE)
		//{

		//	if(	stateMainPlayer->IsInSafeZone && stateMainPlayer->ActorAttributes->byCombatPoseOn==FALSE )
		//	{
		//		if(stateAiInSafeZoneTime<=0.0f){
		//			warnf( NAME_Error,  TEXT("stateAIInSafeZone 1!") );
		//			ResetSafeZoneDelayTime();
		//			
		//			if(MMO::GetCurrentGameState() == EGameState_InGame){
		//				CMessageHandler::GetInstance().Send_CombatPoseOn(TRUE,1);
		//			}
		//			return TRUE;
		//		}
		//	}
		//	else if(stateMainPlayer->IsInSafeZone==FALSE && stateMainPlayer->ActorAttributes->byCombatPoseOn)
		//	{
		//		//when not moving, check once every 10 seconds
		//		if(stateAiInSafeZoneTime<=0.0f){
		//			warnf( NAME_Error,  TEXT("stateAIInSafeZone 2!") );
		//			ResetSafeZoneDelayTime();

		//			if(MMO::GetCurrentGameState() == EGameState_InGame){
		//				CMessageHandler::GetInstance().Send_CombatPoseOn(FALSE,0);
		//			}

		//			return TRUE;
		//		}
		//	}

		//}
	}

	return FALSE;
}



//auto decide whether to cast QTE-breaking ability
UBOOL	MMOTargetManager::stateAIBossQTE()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateCtrlState==CS_IDLE &&
		stateMainPlayer &&
		stateController &&
		TimeQTE < 0.0f )
	{
		UMMOUIBase* pQTEPanel = MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_HintCombat_BossQTE);
		if(pQTEPanel && pQTEPanel->IsVisible)
		{
			if( (LastMouseHitNavMeshLocation - stateController->MouseHitNavMeshLocation).Size2D() > 256 )
			{
				//CMessageHandler::GetInstance().Send_RequestUseAbility(stateMainPlayer->ServerID ,4010,stateMainPlayer->ServerID,stateMainPlayer->Location);
				//CMessageHandler::GetInstance().Send_RequestUseAbility(stateMainPlayer->ServerID ,qssBossQTE,stateMainPlayer->ServerID,stateMainPlayer->Location);
				
				TimeQTE = 0.3f;
			}
		}
		return TRUE;
	}
	return FALSE;
}


//stall click model pop-up UI display--------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateClickStallState()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==TRUE &&
		stateController &&
		stateMainPlayer	&&
		stateDesireMMOActor &&
		stateLeftBtnDown==TRUE && 
		stateDesireMMOActor->IsMainPlayer()	//self is the target
		) 
	{
			if(stateMainPlayer->MoveType == EMT_InStall)
			{
				UMMOUIBase *stallSettingPanel = MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Stall_StallSetting);
				if(stallSettingPanel==NULL || !stallSettingPanel->IsVisible)
				{
					MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallSetting,TRUE,"");
				}

				//UMMOUIBase *stallSettingPanel = MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallSetting);
				//if(stallSettingPanel && !stallSettingPanel->IsVisible)
				//{
				//	stallSettingPanel->Show(TRUE);
				//	return TRUE;
				//}
			}
			
	}
	return FALSE;
}

//special timed prompts, e.g. irregular-interval durability warnings for the main weapon--------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateStackWarningState()
{
	if(	stateInterrupt==FALSE &&
		stateStallState==FALSE &&
		stateController &&
		stateMainPlayer &&
		stateMainPlayer->ItemManager &&
		stateWarningTime>=10.0f
		) 
	{

		//check while in game
		if(MMO::GetCurrentGameState() == EGameState_InGame)
		{
			//for(int i=EET_Normal_MainHand;i<EET_Total_Count;i++)
			//{
			//	FClientItemInfo* item = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED , EEquipType(i) );
			//	if(item )
			//	{
			//		UItem* pEquip = MMOFacade::getSingleton().FindItemTemplate(item->m_nTemplateID);
			//		if(pEquip){
			//			//less than 5%
			//			if(item->m_nCurDur*20<=item->m_nMaxDur)
			//			{
			//				stateWarningTime = 0.0f;
			//				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.ItemSubTypes.") ) + pEquip->ItemSubType )
			//																+ MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Equip.DurWarning1")) );
			//				//return TRUE;
			//			}
			//			else if(item->m_nCurDur*5<=item->m_nMaxDur)
			//			{
			//				if(stateWarningTime>=30.0f)
			//				{
			//					stateWarningTime = 0.0f;
			//					MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.ItemSubTypes.") ) + pEquip->ItemSubType )
			//						+ MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Equip.DurWarning3")) );
			//					//return TRUE;
			//				}
			//			}
			//		}
			//	}
			//}
		}
	}
	return FALSE;
}


//smartly clear states such as decompose, identify, repair,--------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOTargetManager::stateReleaseOperation()
{
	if(	stateInterrupt==FALSE &&
		stateController &&
		stateMainPlayer 
		) 
	{
		if(EGameState_InGame == MMO::GetCurrentGameState() && MMOUI::GetUIManager()->IsItemSlotActionListener() )
		{
			MMOUI::GetUIManager()->ClearCurrentItemSlotActionListenerUpdate();
		}
	}
	return FALSE;
}

//gm combo test, automatically become God of War------------------------------------------------------------------------------------------------------------------------------------
UBOOL	MMOTargetManager::stateUpdateAres()
{
	if(	AresFlag>0 && 	
		stateMainPlayer &&
		stateController &&
		AresDeltaTime <= 0.0 )
	{
		//basic boost
		if(AresFlag==1)
		{
			AresIndex = 0;

		}
		//train a weapon
		else if(AresFlag==2)
		{
			stateController->NativeDeleteBagItem(0,64);
		}
		else if(AresFlag==3)
		{
			if(AresIndex==0){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11210251,1) end") );//long sword
			}
			else if(AresIndex==1){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211151,1) end") );//helmet
			}
			else if(AresIndex==2){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211251,1) end") );//shoulder guard
			}
			else if(AresIndex==3){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211351,1) end") );//armor
			}
			else if(AresIndex==4){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211451,1) end") );//leg armor
			}
			else if(AresIndex==5){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211551,1) end") );//waist guard
			}
			else if(AresIndex==6){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211651,1) end") );//wrist guard
			}
			else if(AresIndex==7){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211751,1) end") );//boots
			}
			else if(AresIndex==8){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11211851,1) end") );//cloak
			}
			else if(AresIndex==9){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11252154,1) end") );//bracelet
			}
			else if(AresIndex==10){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11252354,1) end") );//ring
			}
			else if(AresIndex==11){
				CMessageHandler::GetInstance().Send_GMCommand( TEXT("for i=0,64,1 do AddItem(11252254,1) end") );//necklace
			}
			else
			{
				//end
				AresFlag = 0;
				return FALSE;
			}
		}
		else if(AresFlag==4)
		{
			for(INT i=0;i<64;i++){			
				FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEFAULT, i);
				if(info && info->m_byIdentified==IDENTIFY_TYPE_NO)
				{
					CMessageHandler::GetInstance().Send_IdentifyItem( E_CONSLOT_DEFAULT, i );
				}
			}
		}
		else if(AresFlag==5)
		{
			INT MaxPotential = 0;
			INT MaxPow = 0;
			INT MaxSlot = 0;
			FClientItemInfo* MaxInfo = NULL;
			for(INT i=0;i<64;i++){			
				FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEFAULT, i);
				if(info)
				{
					INT CurPotential = info->GetPotential(0);
					if(CurPotential>MaxPotential)
					{
						MaxPow	= info->Pow;
						MaxPotential = CurPotential;
						MaxSlot = i;
						MaxInfo = info;
					}
					else if(CurPotential==MaxPotential)
					{
						if(info->Pow>MaxPow)
						{
							MaxPow  =info->Pow;
							MaxSlot	= i;
							MaxInfo = info;
						}
					}
				}
			}

			if(MaxInfo)
			{
				INT EmptySlot = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_BANK);
				if(EmptySlot>=0){
					CMessageHandler::GetInstance().Send_MoveItem(E_CONSLOT_BANK, EmptySlot, E_CONSLOT_DEFAULT, MaxSlot);
				}
			}
		}
		else if(AresFlag==6)
		{
			CMessageHandler::GetInstance().Send_SortBag(EBLT_NormalItem);
		}
		else if(AresFlag==8)
		{
			//CMessageHandler::GetInstance().Send_SortBag(E_CONSLOT_BANK);
			AresIndex++;
			//jump to step two, repeat
			AresFlag = 2;
			return FALSE;
		}
		AresFlag++;
		AresDeltaTime=1.0;
	}
	return FALSE;
}
