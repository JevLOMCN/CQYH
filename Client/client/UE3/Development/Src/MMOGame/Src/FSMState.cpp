

#include "StdAfx.h"
#include "FSMState.h"
//#include "FSMTrigger.h"
// #include "MMOGameClasses.h"
// #include "MMOGameUIClasses.h"
// #include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMONavigationManager.h"
#include "ScaleformEngine.h"
#include "UITextParser.h"
#include "FSMCtrl_TargetTips.h"
#include "DoodadManager.h"
#include "Mir2Attack.h"




extern INT32 g_SendClearAbilityCacheTimes;
extern INT32 g_RecieveClearAbilityCacheTimes;
extern FLOAT g_UpdateClearAbilityCacheTime;

namespace FSM{

	//State set--------------------------------------------------------------------
	FSMState_Input			Input;
	FSMState_Charactor		Char;
	FSMState_Target			Target;
	FSMState_Ability		Ability;
	FSMState_CDTime			CDTime;

	FSMState_Trigger		Trigger;
	FSMState_Ctrl			Ctrl;
	//----------------------------------------------------------------------------
}

void FSMStateManager::Init()
{
	for( FSMStateIterator it = IFSMStateList.begin();	it!=IFSMStateList.end();	it++)
	{
		IFSMState* pState = *it;
		if(pState){
			pState->Init();
		}
	}
}
void FSMStateManager::AddGameState(IFSMState* pState)
{
	if(pState)
	{
		//pState->Init();
		IFSMStateList.push_back(pState);
	}
}
void FSMStateManager::Update(FLOAT DelTime)
{
	CTRL_TICK("FSMStateManager");

	for( FSMStateIterator it = IFSMStateList.begin();	it!=IFSMStateList.end();	it++)
	{
		IFSMState* pState = *it;
		if(pState){
			pState->Update(DelTime);
		}
	}
}


IFSMState::IFSMState(void)
{
	FSMStateManager::getSingleton().AddGameState(this);
}

//Mouse/key series states----------------------------------------------------------------
void FSMState_Input::Init()
{
	ShiftDown		= FALSE;				
	LeftBtnDown		= FALSE;				
	RightBtnDown	= FALSE;				
	CtrlBtnDown		= FALSE;
	AltBtnDown		= FALSE;
}
void FSMState_Input::Update(FLOAT DelTime)
{
	CTRL_TICK("FSMState_Input");

	ShiftDown_Last		=	ShiftDown;				
	LeftBtnDown_Last	=	LeftBtnDown;			
	RightBtnDown_Last	=	RightBtnDown;			
	CtrlBtnDown_Last	=	CtrlBtnDown;	
	AltBtnDown_Last		=   AltBtnDown;

	AMMOPlayerController* pController =	MMOFacade::getSingleton().GetMainController();
	if(pController){
		ShiftDown	=	(MMO::GetAsyncKeyStateEx(VK_SHIFT) & 0x8000)>0; // pController->eventIsLeftShiftDown() | pController->eventIsRightShiftDown();
		static UBOOL bF = FALSE;
		ShiftDown   = ShiftDown|bF;
		LeftBtnDown	=	pController->bLeftMousePressed;
		AltBtnDown	=   (MMO::GetAsyncKeyStateEx(VK_MENU) & 0x8000)>0; //pController->eventIsLeftAltDown() | pController->eventIsRightAltDown() ;
		CtrlBtnDown =   (MMO::GetAsyncKeyStateEx(VK_CONTROL) & 0x8000)>0; //pController->eventIsLeftCtrlDown() | pController->eventIsRightCtrlDown();
	}
	else
	{
		ShiftDown		= FALSE;				
		LeftBtnDown		= FALSE;				
		RightBtnDown	= FALSE;				
		CtrlBtnDown		= FALSE;
		AltBtnDown		= FALSE;
	}
}

//Main character related series states-------------------------------------------------------------
void FSMState_Charactor::Init()
{
	MainPlayer		= NULL;
	Controller		= NULL;
	MoveEnable		= FALSE;
	MoveEnableInAdvance = FALSE;
	IsDead			= FALSE;
	AbilityActionNum= 0;
}
void FSMState_Charactor::Update(FLOAT DelTime)
{
	CTRL_TICK("FSMState_Charactor");

	MainPlayer  =	MMOFacade::getSingleton().GetMainPlayer();
	Controller	=	MMOFacade::getSingleton().GetMainController();
	if(MainPlayer && Controller)	{
		if(Controller->WaitingForTheMoveToRespond>0.0f)
		{
			MoveEnable=FALSE;
			MoveEnableInAdvance = FALSE;
		}
		else{
			MoveEnable=MainPlayer->CanMove();
			MoveEnableInAdvance = MainPlayer->CanMoveInAdvance();
		}
		IsDead	  =MainPlayer->eventIsDead();
		AbilityActionNum	= Controller->GetCmdNum(CMD_ABILITY);
		nCannotInterruptLevel = Controller->GetCmdNum(CMD_Interrupt);
	}
	else{
		MoveEnable=FALSE;
		IsDead	  =FALSE;
		MoveEnableInAdvance = FALSE;
		AbilityActionNum = 0;
		nCannotInterruptLevel = 0;
	}
}

//Various countdown series states----------------------------------------------------------
void FSMState_CDTime::Init()
{
	KeyPressedTime		= 0.0f;
	FastTime			= 0.0f;
	SlowTime			= 0.0f;	
	RotationCDTime		= 0.0f;
	AiCdTime			= 0.0f;
	AbilityRespondTime	= 0.0f;
	SwitchRespondTime	= 0.0f;
	AiInSafeZoneTime	= 0.0f;
	CheckWeaponBindTime = 3.0f;
	AiBossQTETime		= 0.0f;
	WarningTime			= 0.0f;
	FocusTime			= 0.0f;
	WalkDelayTime		= 0.0f;
	RunDelayTime		= 0.0f;
	MoveDelayTime		= 0.0f;
	DoAIAbilityCD		= 0.0f;
	MoveDelayFrame		= 0;

}

void FSMState_CDTime::Update(FLOAT DelTime)
{
	CTRL_TICK("FSMState_CDTime");

	AMMOPlayerController* pController =	MMOFacade::getSingleton().GetMainController();

	if(FastTime>=TIME_SYNC_FAST)FastTime-=TIME_SYNC_FAST;
	FastTime+= DelTime;

	if(SlowTime>=TIME_SYNC_SLOW)SlowTime-=TIME_SYNC_SLOW;
	SlowTime+= DelTime;	

	if(RotationCDTime>0.0f)
	{
		RotationCDTime -= DelTime;
	}

	if(DoAIAbilityCD>0.0f)
	{
		DoAIAbilityCD -= DelTime;
	}

	if(FocusTime>0)FocusTime+=DelTime;

	//Safe zone check time
	AiInSafeZoneTime -= DelTime;
	//Time to check whether weapon binding is correct
	CheckWeaponBindTime -= DelTime;
	//bossQTE check time
	AiBossQTETime	 -= DelTime;

	//Message freeze time update
	if(pController && pController->WaitingForTheAbilityRespond>0)
	{
		AbilityRespondTime -= DelTime;
		if(AbilityRespondTime<0.0f)
		{
			pController->WaitingForTheAbilityRespond = 0;
			AbilityRespondTime = -1.0f;
		}
	}
	else
	{
		AbilityRespondTime=-1.0f;
	}

	if(SwitchRespondTime>0.0f)
	{
		SwitchRespondTime-=DelTime;
	}

	if(pController && pController->WaitingForTheMoveToRespond>0.0f)
	{
		pController->WaitingForTheMoveToRespond -= DelTime;
	}

	if(WalkDelayTime>0.0f)
	{
		WalkDelayTime -= DelTime;
	}
	if(RunDelayTime>0.0f)
	{
		RunDelayTime -= DelTime;
	}
	if(MoveDelayTime>0.0f)
	{
		MoveDelayTime -= DelTime;
	}
	if(MoveDelayFrame>0)
	{
		MoveDelayFrame--;
	}


	
}

//Ability series states----------------------------------------------------------------
void FSMState_Ability::Init()
{
	DesireFClassAbility = NULL;
	DesireAbility		= NULL;
	UsableAbility		= NULL;
	CurrentAbility		= NULL;
	PickTarget			= TRUE;
	AbilityEnable		= FALSE;
	CacheAbility = NULL;
}

void FSMState_Ability::Update(FLOAT DelTime)
{
	CTRL_TICK("FSMState_Ability");

	if(g_UpdateClearAbilityCacheTime>=0.0f)
	{
		g_UpdateClearAbilityCacheTime -= DelTime;
		if(g_UpdateClearAbilityCacheTime<0.0f)
		{
			g_RecieveClearAbilityCacheTimes = g_SendClearAbilityCacheTimes;
		}
	}


	//Update triggered abilities
	if(CacheAbility){
		UsableAbility= Mir2Attack::getSingleton().OptimizeAbility(CacheAbility,UsableAbility); 
	}
	else{
		UsableAbility= Mir2Attack::getSingleton().OptimizeAbility(DesireAbility,UsableAbility); 
	}
	//UsableAbility=DesireAbility; 
	//if(UsableAbility)
	//{
	//	warnf(TEXT("Refresh ability----------%d"),UsableAbility->SpellCommonAttribute.Id);

	//	//static INT lastId = 0;
	//	//if(lastId!=UsableAbility->SpellCommonAttribute.Id)
	//	//{
	//	//	lastId=UsableAbility->SpellCommonAttribute.Id;
	//	//	warnf(TEXT("Refresh ability----------%d"),lastId);
	//	//}
	//}

	if(UsableAbility&&UsableAbility->ClientCommonAttribute.PickTarget)
	{
		PickTarget = TRUE;
	}
	else
	{
		PickTarget = FALSE;
	}
	//Whether the ability is used
	AMMOPlayerController* pController =	MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* pMainPlayer  =	MMOFacade::getSingleton().GetMainPlayer();

	AbilityEnable	= pController->CanUseAnyAbility(UsableAbility);
	//Not enough mana
	if(UsableAbility && pMainPlayer && UsableAbility->CanUseManaCheck(pMainPlayer)==FALSE)
	{
		AbilityEnable = FALSE;
	}
}

//UMMOAbility*	FSMState_Ability::GetDefaultAbility()
//{
//	//Default ability
//	switch( MMOFacade::getSingleton().GetMainActor()->GetClassId() )
//	{
//	case EPCT_PlayerClassA://Warrior
//		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(1000, 0);
//		break;
//	case EPCT_PlayerClassB://Mage
//		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(2500, 0);
//		break;
//	case EPCT_PlayerClassC://Assassin
//		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(1500, 0);
//		break;
//	case EPCT_PlayerClassD://Archer
//		return MMOFacade::getSingleton().GetMainActor()->GetAbilityById(2000, 0);
//		break;
//	default:
//		return NULL;
//	}
//}
//
//
//UMMOAbility* FSMState_Ability::OptimizeAbility(UMMOAbility* Ability)
//{
//	if(Ability&&Ability->ClientCommonAttribute.UseDefaultAbility)
//	{
//		//Not enough mana	//copy from server by JimZhang
//		AMMOPlayerController* pController =	MMOFacade::getSingleton().GetMainController();
//		AMMOPlayer* pMainPlayer  =	MMOFacade::getSingleton().GetMainPlayer();
//		if(pController && pMainPlayer && Ability->CanUseManaCheck(pMainPlayer)==FALSE)
//		{
//			return GetDefaultAbility();
//		}
//	}
//	return Ability;
//}

//Control series states----------------------------------------------------------
void FSMState_Ctrl::Init()
{
	IsBattleState	= FALSE;
	IsGuildBattle	= FALSE;			//Guild battle state
	//GameControlType = GC_DIABLO3;
	IsSelectCtrl	= FALSE;
	IsMoveCtrl		= FALSE;
	IsActionCtrl	= FALSE;

	state_Switching_Flag = FALSE;		//State switching flag
	state_Standby	= TRUE;				//standby state
	state_Moving	= FALSE;			//moving state
	state_Attack	= FALSE;			//attack state
	state_Interact	= FALSE;			//interact state
	state_AiDesire	= FALSE;			//search state

	state_AlwaysWalk = FALSE;			//force walk
	state_AlwaysWalkBase = FALSE;		//force walk base state

	state_OpenPrivateChat = FALSE;
	KeyType_Keeping = KT_NONE;

//	ctrlFlag_IgnoreShiftBtn=FALSE;	//no shift key needed
//	ctrlFlag_DDCS=FALSE;			//every-hit assassinate
//	ctrlFlag_ZNBY=FALSE;			//smart crescent slash
//	ctrlFlag_ZDZR=FALSE;			//auto sun-chasing slash
//	ctrlFlag_ZDLH=FALSE;			//auto flaming sword
//	ctrlFlag_ZDYS=FALSE;			//auto stealth
//	ctrlFlag_ZDKD=FALSE;			//auto shield

	ctrlAttackAi_LastDelay	= 0.25f;
	ctrlAttackAi_LockTime	= 0.05f;

	IsWantToSendRequest4000Or4001Ability = FALSE;
	ctrlKeepRunning = FALSE;
}

void FSMState_Ctrl::Update(FLOAT DelTime)
{

}


//target series states----------------------------------------------------------------
void FSMState_Target::Init()
{
	CurTargetActor			= NULL;
	LastTargetActor			= NULL;
	CurTargetMMOActor		= NULL;		
	LastTargetMMOActor		= NULL;	
	CurTargetMMOActorID		= 0;
	LastTargetMMOActorID	= 0;
	AbilityTargetMMOActor	= NULL;
	CurTargetMMOPlayer		= NULL;	
	CurTargetNPC			= NULL;	
	//CurTargetPos			= NULL;	
	TargetAttackble			= FALSE;
	TargetIsDead			= FALSE;

	DesireActor				= NULL;
	DesireMMOActor			= NULL;		
	LastDesireMMOActor		= NULL;
	DesireMMOPlayer			= NULL;		
	DesireNPC				= NULL;				
	DesireNPCTempleteID		= NULL;	
	DesireInteractableActor = ScriptInterfaceCast<IIInteractableActor>(NULL);
	//DesirePos				= NULL;				
	DesireAttackble			= NULL;		
	AttackMMOActor			= NULL;			
	SelectMMOActor			= NULL;			
	DesireMMOActorChangeFlag = FALSE;
	UITargetMMOActorID		= 0;
	UpdateSceneHintTime		= -1.0f;
	UpdateSceneHintID		= 0;
}

//smooth position,
FVector FSMState_Target::SmoothTargetPos(FVector TargetPos)
{
	AMMOPlayer*			  pMainPlayer	=	MMOFacade::getSingleton().GetMainPlayer();
	AMMOPlayerController* pController	=	MMOFacade::getSingleton().GetMainController();
	//this is a magical filter, given the inaccuracy of grid pathfinding and the need for smooth control
	if(pMainPlayer && pController)
	{
		FVector Location = pMainPlayer->Location - FVector(0.0,0.0,pMainPlayer->CylinderComponent->CollisionHeight);

		//correct the target point, eliminating horizontal pathfinding error
		FLOAT HorizontalDelta = abs(Location.Z - TargetPos.Z);
		//if(HorizontalDelta<256.0)
		//if(HorizontalDelta>256.0)
		{
			FLOAT deltaZ = Location.Z - pController->MouseProjectionOrigin.Z;
			FLOAT DirectionZ = pController->MouseProjectionDirection.Z;
			TargetPos = pController->MouseProjectionOrigin + pController->MouseProjectionDirection*(deltaZ/DirectionZ);

			//pController->DrawDebugSphere(TargetPos, 10, 16, 255, 0, 255, TRUE);
			return TargetPos;
		}
	}

	//pController->DrawDebugSphere(TargetPos, 10, 16, 255, 255, 255, TRUE);

	return TargetPos;
}

//find a suitable nearby position,
FVector FSMState_Target::OptimizeTargetPos(FVector TargetPos)
{
	AMMOPlayer*			  pMainPlayer	=	MMOFacade::getSingleton().GetMainPlayer();
	AMMOPlayerController* pController	=	MMOFacade::getSingleton().GetMainController();
	
	if(pMainPlayer && pController){
		FVector Location = pMainPlayer->Location - FVector(0.0,0.0,pMainPlayer->CylinderComponent->CollisionHeight);
	
		//the click point is too close, extend a certain distance
		FLOAT dis = (TargetPos-Location).Size2D();
		FLOAT minDis = pMainPlayer->CylinderComponent->CollisionRadius*3.0;
		if( dis<minDis && dis>0.001)
		{
			TargetPos = Location + (TargetPos - Location)*(minDis/dis);
		}
		
	}

	return TargetPos;
}

INT FSMState_Target::OptimizeDoodadID(const FVector& Location, FLOAT Radius)
{
	return DoodadManager::getSingleton().SearchBody( Location, Radius ); 
}

INT	 FSMState_Target::OptimizeTargetID(FRotator Rotator,FVector Location,FLOAT Radius)
{
	STLInt2IntMap* MMOActorMap = MMOFacade::getSingleton().GetCharacterMap();
	FLOAT MaxDistance = Radius;
	FLOAT MinDistance = 0.0;
	FLOAT MaxWeightDistance = Radius*10.0f;
	FLOAT CurDistance = 0;
	INT   ObjID		  = 0;

	AMMOPlayer*			  pMainPlayer	=	MMOFacade::getSingleton().GetMainPlayer();

	for (STLInt2IntMap::iterator Itor=MMOActorMap->begin();  Itor!=MMOActorMap->end(); Itor++ )
	{
		AMMOActor* pMMOActor = Cast<AMMOActor>(UObject::GetIndexedObject(Itor->second));
		if(pMMOActor==NULL)continue;

		if(MMOFacade::getSingleton().IsAttackable( pMainPlayer ,pMMOActor)==FALSE) continue;

		if(pMMOActor->GetClass()->ImplementsInterface( UIAbilityDamageable::StaticClass())==FALSE)
		{
			continue;
		}

		if(pMMOActor->eventIsDead())
		{
			continue;
		}

		FVector ObjVector = pMMOActor->Location-Location;
		CurDistance = ObjVector.Size2D();
		if(CurDistance<MaxDistance && CurDistance>MinDistance )
		{
			//calculate direction coefficient-------------------------------------------
			ObjVector.Z = 0.0f;
			FRotator ObjRotator = ObjVector.Rotation();
			INT    Weight		= abs(ObjRotator.Yaw - Rotator.Yaw);
			if(Weight*2>MAXWORD)
			{
				Weight =  MAXWORD-Weight;
			}
			FLOAT WeightLv = FLOAT(Weight*2000/MAXWORD) * 0.001f;
			FLOAT CurWeightDistance = CurDistance*(1.0f+WeightLv*3.0f);
			//-------------------------------------------------------------


			if(CurWeightDistance<MaxWeightDistance){
				MaxDistance = CurDistance; 
				MaxWeightDistance = CurWeightDistance;
				ObjID		= Itor->second;
			}
		}
	}

	return ObjID;
}

void FSMState_Target::SetTargetActor(AActor* TargetActor)
{
	AMMOPlayer* MainPlayer  =	MMOFacade::getSingleton().GetMainPlayer();
	if(MainPlayer==NULL)return;
	
	UBOOL bDiff = (CurTargetActor!=TargetActor);

	//if(CurTargetMMOPlayer!=TargetActor)
	//{
	//	if(CurTargetMMOPlayer && CurTargetMMOPlayer->ActorAttributes && CurTargetMMOPlayer->ActorAttributes->TitleID>0)
	//	{
	//		MMOUI::GetUIManager()->delegateUpdateHintText(CurTargetMMOPlayer);
	//	}
	//}

	LastTargetActor		= CurTargetActor;
	LastTargetMMOActor	= CurTargetMMOActor;
	LastTargetMMOActorID= CurTargetMMOActorID;

	CurTargetActor		=   TargetActor;
	CurTargetMMOActor	=	Cast<AMMOActor>(TargetActor);

	if(CurTargetMMOActor )
	{
		LastTargetMMOActorID = CurTargetMMOActor->ServerID;
	}
	else
	{
		LastTargetMMOActorID = 0;
	}

	CurTargetMMOPlayer	=	Cast<AMMOPlayer>(CurTargetMMOActor);	
	CurTargetNPC		=	Cast<ANPC>(CurTargetMMOActor);

	if(CurTargetMMOPlayer && CurTargetMMOPlayer->ActorAttributes && CurTargetMMOPlayer->ActorAttributes->TitleID>0)
	{
		if(bDiff)
		{
			UpdateSceneHintTime = 1.0f;
		}
	}
	else
	{
		UpdateSceneHintTime = -1.0f;
	}

	TargetAttackble   =	MMOFacade::getSingleton().IsAttackable(MainPlayer,CurTargetMMOActor);
	TargetIsDead	  =	CurTargetMMOActor?CurTargetMMOActor->eventIsDead():FALSE;

	//if(CurTargetMMOPlayer!=LastTargetMMOActor)
	//{
	//	if(CurTargetMMOPlayer && CurTargetMMOPlayer->ActorAttributes && CurTargetMMOPlayer->ActorAttributes->TitleID>0)
	//	{
	//		MMOUI::GetUIManager()->delegateUpdateHintText(CurTargetMMOPlayer);
	//	}
	//}
}

void FSMState_Target::ClearActor(AActor* Actor)
{
	if(Actor && Actor==CurTargetActor)
	{
		CurTargetActor		= NULL;
		CurTargetMMOActor	= NULL;
		CurTargetMMOPlayer	= NULL;
		CurTargetNPC		= NULL;
	}
	if(Actor && Actor==LastTargetActor)
	{
		LastTargetActor		= NULL;
		LastTargetMMOActor	= NULL;
	}
	if(Actor && Actor==DesireActor)
	{
		DesireActor			= NULL;
		DesireMMOActor		= NULL;
		DesireMMOPlayer		= NULL;
		DesireNPC			= NULL;
		DesireInteractableActor = ScriptInterfaceCast<IIInteractableActor>(NULL);
	}

	if(Actor && Actor==SelectMMOActor)
	{
		SelectMMOActor			= NULL;
		AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
		if(Controller)
		{
			Controller->SetSelectTarget(SelectMMOActor);
		}
	}

	if(Actor && Actor==AbilityTargetMMOActor)
	{
		AbilityTargetMMOActor=NULL;
	}
}


void FSMState_Target::Update(FLOAT DelTime)
{
	CTRL_TICK("FSMState_Target");

	AMMOPlayerController* pController	=	MMOFacade::getSingleton().GetMainController();
	
	if(pController){
		//determine whether it can pass through the UI
		MMOFacade::UpdateCaptureInput();

		LastMouseHitNavMeshLocation = pController->MouseHitNavMeshLocation;

		// zhaojun@snda
		UBOOL bNavMeshPicked = pController->NativeGetPickPointNavMesh();
		if (!bNavMeshPicked)
		{
			pController->MouseHitNavMeshLocation = pController->MouseHitWorldLocation;
		}
		// ~zhaojun@snda
		pController->NativeGetPickPointActorPreferred();// Point();

		//set the picked actor
		CaptureInput = MMOFacade::GetCaptureInput();
		//if(CaptureInput==FALSE)
		{
			SetTargetActor(pController->GetTraceActor());
			if(ShowUITipsFlag && CaptureInput==FALSE)
			{
				ShowUITipsFlag = FALSE;
				//MMOUI::GetUIManager()->delegateHideTips();
			}
		}
		//else
		//{
		//	SetTargetActor(NULL);
		//}

		//mouse point update
		if(CurTargetMMOActor==NULL)
		{
			CurTargetPos =SmoothTargetPos(pController->MouseHitNavMeshLocation);
			CurOptimizeTargetPos = CurTargetPos;  // OptimizeTargetPos(CurTargetPos); no longer need to move away from the point under the feet
		}
		else
		{
			CurTargetPos = SmoothTargetPos(pController->MouseHitNavMeshLocation);
			CurOptimizeTargetPos = CurTargetPos;
		}	

		//ability attack target is dead
		if(AbilityTargetMMOActor && AbilityTargetMMOActor->eventIsDead())
		{
			AbilityTargetMMOActor = NULL;
		}

		//attack target is dead
		if( AttackMMOActor && (AttackMMOActor->eventIsDead() || AttackMMOActor->IsPendingKill()))
		{
			AttackMMOActor = NULL;
			DesireMMOActor = NULL;
			SelectMMOActor =  DesireMMOActor;
			pController->SetSelectTarget(SelectMMOActor);

			FSM::Ability.DesireAbility = NULL;
			FSM::Ability.UsableAbility = NULL;

			MMOGameEvent_TargetEvent evt(MMOGameEvent_TargetEvent::ETCT_DesiredTarget, CurTargetMMOActor, DesireMMOActor);
			MMOUI::PostGameEvent(&evt);

		}
		//attack target became unselectable
		else if( AttackMMOActor && !AttackMMOActor->IsPickable)
		{
			AttackMMOActor = NULL;
			DesireMMOActor = NULL;
			SelectMMOActor =  DesireMMOActor;
			pController->SetSelectTarget(SelectMMOActor);

			MMOGameEvent_TargetEvent evt(MMOGameEvent_TargetEvent::ETCT_DesiredTarget, CurTargetMMOActor, DesireMMOActor);
			MMOUI::PostGameEvent(&evt);
		}


		////attack target has left
		//AMMOPlayer* MainPlayer  =	MMOFacade::getSingleton().GetMainPlayer();
		//if( AttackMMOActor && MainPlayer)
		//{
		//	if((MainPlayer->Location - AttackMMOActor->Location).Size2D() > 1280)
		//	{
		//		AttackMMOActor = NULL;
		//		DesireMMOActor = NULL;
		//		MMOGameEvent_TargetEvent evt(MMOGameEvent_TargetEvent::ETCT_DesiredTarget, CurTargetMMOActor, DesireMMOActor);
		//		MMOUI::PostGameEvent(&evt);
		//	}
		//}

		//locked target moved away
		AMMOPlayer* MainPlayer  =	MMOFacade::getSingleton().GetMainPlayer();
		if(MainPlayer && DesireMMOActor)
		{
			FVector dis = MainPlayer->ServerLocation - DesireMMOActor->ServerLocation;
			//if(dis.X>1408||dis.X<-1408||dis.Y>1408||dis.Y<-1408 )
			if(abs(dis.X)+abs(dis.Y)>(2353.0f+64.0f))
			{
				DesireMMOActor = NULL;
				SelectMMOActor =  DesireMMOActor;
				pController->SetSelectTarget(SelectMMOActor);

				MMOGameEvent_TargetEvent evt(MMOGameEvent_TargetEvent::ETCT_DesiredTarget, CurTargetMMOActor, DesireMMOActor);
				MMOUI::PostGameEvent(&evt);
				MMOFacade::getSingleton().GetMainController()->SetSelectTarget(DesireMMOActor);

			}			
		}

		//ability locked target moved away
		if(MainPlayer && AbilityTargetMMOActor)
		{
			FVector dis = MainPlayer->ServerLocation - AbilityTargetMMOActor->ServerLocation;
			//if(dis.X>1408||dis.X<-1408||dis.Y>1408||dis.Y<-1408 )
			if(abs(dis.X)+abs(dis.Y)>(2534.0f+64.0f))
			{
				AbilityTargetMMOActor = NULL;
			}			
		}

		//if( DesireNPC && MainPlayer)
		//{
		//	if((MainPlayer->Location - DesireNPC->Location).Size2D() > 1024 &&  )
		//	{
		//		LastDesireMMOActor = DesireMMOActor;				
		//		DesireActor		= NULL;
		//		DesireMMOActor	= NULL;
		//		DesireMMOPlayer	= NULL;
		//		DesireNPC		= NULL;
		//		DesireInteractableActor = ScriptInterfaceCast<IIInteractableActor>(DesireActor);

		//		MMOGameEvent_TargetEvent evt(MMOGameEvent_TargetEvent::ETCT_DesiredTarget, CurTargetMMOActor, DesireMMOActor);
		//		MMOUI::PostGameEvent(&evt);
		//	}
		//}

		if(MainPlayer){
			LastMainPlayerPos = MainPlayer->Location;
		}
	}


	if(UpdateSceneHintID>0 && CurTargetMMOPlayer==NULL)
	{
		MMOUI::GetUIManager()->delegateUpdateHintTextByID(UpdateSceneHintID);
		UpdateSceneHintID = 0;
	}

	if(UpdateSceneHintTime>0.0f)
	{
		UpdateSceneHintTime-=DelTime;
		if(UpdateSceneHintTime<=0.0f)
		{
			if(CurTargetMMOPlayer && CurTargetMMOPlayer->ActorAttributes && CurTargetMMOPlayer->ActorAttributes->TitleID>0)
			{
				if(UpdateSceneHintID>0)
				{
					MMOUI::GetUIManager()->delegateUpdateHintTextByID(UpdateSceneHintID);
				}
				UpdateSceneHintID = CurTargetMMOPlayer->ServerID;
				MMOUI::GetUIManager()->delegateUpdateHintTextByID(UpdateSceneHintID);
			}
		}
	}

}

//trigger event flags----------------------------------------------------------------
void FSMState_Trigger::Init()
{
	IsTargetTrigger = FALSE;		//target change trigger event
	IsDesireTrigger	= FALSE;
	IsDesirePosTrigger=FALSE;
	IsWaitingSendMove =FALSE;
	DelayTimeWaitingSendMove = -1.0;
}
void FSMState_Trigger::Update(FLOAT DelTime)
{
	CTRL_TICK("FSMState_Trigger");

	if(DelayTimeWaitingSendMove>=0.0)
	{
		DelayTimeWaitingSendMove -= DelTime;
		if(DelayTimeWaitingSendMove<0.0)
		{
			IsWaitingSendMove =FALSE;
		}
	}
}

