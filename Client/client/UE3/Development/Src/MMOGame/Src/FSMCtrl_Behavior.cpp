


#include "StdAfx.h"
#include "FSMState.h"
#include "FSMCtrl_Behavior.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMONavigationManager.h"
#include "ScaleformEngine.h"
#include "UITextParser.h"
#include "FSMIO.h"
#include "Mir2Attack.h"

using namespace FSM;

//FSM control class registration--------------------------------------------------------------------------------------------------
ADD_FSM_CTRL(FSMCtrl_AiAbility)
ADD_FSM_CTRL(FSMCtrl_ShiftAbility)
ADD_FSM_CTRL(FSMCtrl_StayAndUseAbility)
ADD_FSM_CTRL(FSMCtrl_MoveToAndUseAbility)
ADD_FSM_CTRL(FSMCtrl_MoveToAndInteract)
ADD_FSM_CTRL(FSMCtrl_MoveToPoint)
ADD_FSM_CTRL(FSMCtrl_FocusActor)


extern UBOOL g_PreMovetoPoint;
extern INT g_TotalFrameNum;
//Shift key use ability in place---------------------------------------------------------------------------------------------
UBOOL FSMCtrl_ShiftAbility::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_ShiftAbility");

	if( FSM::Input.ShiftDown		&&
		FSM::Char.MainPlayer		&&
		FSM::Char.Controller		&&
		FSM::Ability.UsableAbility	&&
		FSM::Ability.AbilityEnable  &&
		(FSM::Char.MainPlayer->isMoving == FALSE || FSM::Ability.UsableAbility->SpellCommonAttribute.CanMoveInPrepare )&&
		FSM::Ctrl.KeyType!=KT_NONE &&
		FSM::Ctrl.KeyType!=KT_SECONDARY &&
		FSM::Char.IsDead==FALSE	)
	{
		//if(FSM::Target.DesireMMOActor && FSM::Target.DesireAttackble && FSM::Ability.PickTarget==TRUE)return FALSE;
		if(FSM::Target.DesireMMOActor && FSM::Ability.PickTarget==TRUE)return FALSE;
		if(FSM::Target.DesireMMOActor==NULL && FSM::Char.Controller->bLeftMousePressed==FALSE)return FALSE;
		return TRUE ;
	}


	////Holding shift pauses forced targeted attack
	//if(	FSM::Input.ShiftDown==FALSE	&&
	//	FSM::Input.ShiftDown_Last==TRUE&&
	//	FSM::Target.DesireMMOActor &&
	//	FSM::Target.DesireAttackble &&
	//	FSM::Char.MainPlayer		&&
	//	FSM::Char.Controller		&&
	//	FSM::Ability.PickTarget==TRUE &&
	//	FSM::Char.IsDead==FALSE)
	//{
	//	FSM::Ctrl.state_Attack = FALSE;
	//	FSM::Ctrl.state_Switching_Flag = FALSE;
	//	FSM::Char.Controller->eventClearAllCmd();
	//}

	return FALSE;
}

void FSMCtrl_ShiftAbility::Action()
{
	//if(FSM::Ctrl.KeyType == KT_PRIMARY)
	//{
	//	warnf(TEXT("Cast ability-----------Mir2Attack::getSingleton().GetPrimaryAbility();"));
	//	FSM::Ability.UsableAbility = Mir2Attack::getSingleton().GetPrimaryAbility();
	//	if(FSM::Ability.UsableAbility==NULL)
	//	{
	//		return;
	//	}
	//}

	UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(FSM::Ability.UsableAbility,TRUE);
	if(bRes==FALSE)
	{
		return;
	}

	//warnf(TEXT("Cast ability-----------FSMCtrl_ShiftAbility ID:%d"),FSM::Ability.UsableAbility->SpellCommonAttribute.Id );


	//Cast ability directly
	if(FSM::Char.Controller){
		//This is an odd branch, when holding shift to attack a target, toggle-type abilities must be castable
		if(FSM::Ability.UsableAbility->SpellCommonAttribute.CanMoveInPrepare)
		{
			if(FSM::Ctrl.state_Switching_Flag)
			{
				FSM::Ctrl.state_Switching_Flag = FALSE;
				INT ServerID = FSM::Target.DesireMMOActor?FSM::Target.DesireMMOActor->ServerID:0 ;
				FVector vec = FSM::Char.MainPlayer->Rotation.Vector().SafeNormal();
				//FSM::Char.Controller->RequestUseAbility(ServerID, FSM::Ability.UsableAbility->SlotHotKeyID, FSM::Ability.UsableAbility->SpellCommonAttribute.Level, FSM::Char.MainPlayer->ServerLocation+vec*512.0, FALSE );
				FSM::Char.Controller->RequestUseAbilityEx(ServerID, FSM::Ability.UsableAbility->SpellCommonAttribute.Id, FSM::Ability.UsableAbility->SpellCommonAttribute.Level, FSM::Char.MainPlayer->ServerLocation+vec*512.0, FALSE );
				//FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(0.2f);
				FSMIO::getSingleton().Ability.SetCurrentAbility(FSM::Ability.UsableAbility);
			}
		}
		else{
			FSM::Char.Controller->eventClearAllCmd();
			if(FSM::Target.CurTargetMMOActor){
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,	FSM::Target.CurTargetMMOActor,	FSM::Target.CurTargetMMOActor->Location);
			}
			else
			{
				INT DirIndex		= gServerGridWorld.OptimizeDir_Mir2(FSM::Char.MainPlayer->ServerLocation,FSM::Target.CurOptimizeTargetPos);
				FVector pos1 = gServerGridWorld.GetNearbyGridPos(FSM::Char.MainPlayer->ServerLocation,DirIndex>=0?DirIndex:DirIndex+8 ,1.0f);
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,NULL, pos1 );

				//FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,NULL, FSM::Target.CurOptimizeTargetPos );//FSM::Char.Controller->MouseHitWorldLocation);
			}
			FSMIO::getSingleton().Ability.SetCurrentAbility(FSM::Ability.UsableAbility);
		}
	}
}



//Cast ability in place----------------------------------------------------------------------------------------------------
UBOOL FSMCtrl_StayAndUseAbility::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_StayAndUseAbility");

	if( FSM::Char.MainPlayer	&&
		FSM::Char.Controller	&&
		FSM::Ability.AbilityEnable			&&
		FSM::Ability.UsableAbility			&&
		FSM::Ability.PickTarget == FALSE	&&
		FSM::Char.nCannotInterruptLevel==0  &&
		//FSM::Input.ShiftDown == FALSE		&&
		(FSM::Ability.UsableAbility->SpellCommonAttribute.CanMoveInPrepare==TRUE || FSM::Char.MainPlayer->isMoving==FALSE)
		//(FSM::Target.DesireMMOActor==FALSE || FSM::Target.DesireAttackble)
		)
	{

		UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(FSM::Ability.UsableAbility,TRUE);
		if(bRes)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void FSMCtrl_StayAndUseAbility::Action()
{
	if(FSM::Ability.UsableAbility->SpellCommonAttribute.CanMoveInPrepare)
	{
		UBOOL bRes = FALSE;
		if(FSM::Char.MainPlayer->ActorAttributes){
			if(FSM::Char.MainPlayer->bHoldWeapon==FALSE)
			{
				if(FSM::Char.MainPlayer->ActorAttributes->byCombatPoseOn == TRUE ){
					bRes = TRUE;
				}
			}
			if(FSM::Char.MainPlayer->ActorAttributes->MountID>0)
			{
				bRes = TRUE;
			}
		}

		if(bRes)
		{
			FSM::Char.Controller->eventClearAllCmd();
			FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,		NULL,	FSM::Target.CurTargetPos);
		}
		else
		{
			INT ServerID = FSM::Target.CurTargetMMOActor?FSM::Target.CurTargetMMOActor->ServerID:0 ;
			if(FSM::Ability.UsableAbility->bClientRotateFirst==FALSE)
			{
				FVector vec = FSM::Char.MainPlayer->Rotation.Vector().SafeNormal();
				FSM::Char.Controller->RequestUseAbilityEx(0, FSM::Ability.UsableAbility->SpellCommonAttribute.Id, FSM::Ability.UsableAbility->SpellCommonAttribute.Level, FSM::Char.MainPlayer->ServerLocation+vec*512.0, FALSE );
			}
			else if(FSM::Char.MainPlayer->isMoving)
			{
				FVector vec = FSM::Char.MainPlayer->Rotation.Vector().SafeNormal();
				FSM::Char.Controller->RequestUseAbilityEx(ServerID, FSM::Ability.UsableAbility->SpellCommonAttribute.Id, FSM::Ability.UsableAbility->SpellCommonAttribute.Level, FSM::Char.MainPlayer->ServerLocation+vec*512.0, FALSE );
			}
			else
			{
				FSM::Char.Controller->RequestUseAbilityEx(ServerID, FSM::Ability.UsableAbility->SpellCommonAttribute.Id, FSM::Ability.UsableAbility->SpellCommonAttribute.Level, FSM::Target.CurTargetPos,FALSE);
			}

			FSM::Char.Controller->WaitingForTheAbilityRespond = FSM::Ability.UsableAbility->SpellCommonAttribute.Id;
			//FSMIO::getSingleton().Ability.SetCurrentAbility(FSM::Ability.UsableAbility);
			FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(0.2f);
			FSMIO::getSingleton().CDTime.ResetCD_WalkAndRunDelay(FSM::Ability.UsableAbility->SpellCommonAttribute.WalkDelayTime,FSM::Ability.UsableAbility->SpellCommonAttribute.RunDelayTime);


		}
	}
	else{

		//Cast ability directly
		FSM::Char.Controller->eventClearAllCmd();
		//FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,		NULL,	FSM::Target.CurTargetPos);
		//stateAbilityRespondTime = 0;
		if(FSM::Ability.UsableAbility->ClientCommonAttribute.TargetSelectActor)
		{
			if(FSM::Target.CurTargetMMOActor)
			{
				FSM::Target.AbilityTargetMMOActor = FSM::Target.CurTargetMMOActor;
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,FSM::Target.CurTargetMMOActor,FSM::Target.CurTargetMMOActor->ServerLocation,FALSE);
			}
			else if(FSM::Target.AbilityTargetMMOActor){
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,FSM::Target.AbilityTargetMMOActor,FSM::Target.AbilityTargetMMOActor->ServerLocation,FALSE);
			}
			else if(FSM::Target.DesireMMOActor){
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,FSM::Target.DesireMMOActor,FSM::Target.DesireMMOActor->ServerLocation,FALSE);
			}
			//else if(FSM::Target.CurTargetMMOActor)
			//{
			//	FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,FSM::Target.CurTargetMMOActor,FSM::Target.CurTargetMMOActor->ServerLocation,FALSE);
			//}
			else
			{
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,		NULL,	FSM::Target.CurTargetPos);
			}
		}
		else
		{
			//Replacement when attacking via UI click 2015.12.16
			AMMOActor* pCurTargetMMOActor = FSM::Target.CurTargetMMOActor;
			if(FSM::Target.UITargetMMOActorID&&FSM::Ability.UsableAbility->bCanUITarget)
			{
				pCurTargetMMOActor = MMOFacade::getSingleton().GetCharacter(FSM::Target.UITargetMMOActorID);
				if(pCurTargetMMOActor==NULL)
				{
					pCurTargetMMOActor = FSM::Target.CurTargetMMOActor;
				}
			}


			if(FSM::Ability.UsableAbility->bClientRotateFirst==FALSE)
			{
				FVector vec = FSM::Char.MainPlayer->Rotation.Vector().SafeNormal();
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,		NULL,	FSM::Char.MainPlayer->ServerLocation+vec*512.0);
			}
			else if(pCurTargetMMOActor)
			{
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility, pCurTargetMMOActor, pCurTargetMMOActor->ServerLocation,FALSE);
			}
			else
			{
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,		NULL,	FSM::Target.CurTargetPos);
			}
		}
		
	}		
}


//Walk to target and cast ability----------------------------------------------------------------------------------------------
UBOOL FSMCtrl_MoveToAndUseAbility::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_MoveToAndUseAbility");

	//warnf(NAME_Warning, TEXT("------------------------------------------------------------------------------FSMCtrl_MoveToAndUseAbility::Update"));

	//Normal attack entry
	if( //(FSM::Ctrl.state_Switching_Flag ||(FSM::Ctrl.state_Attack&&FSM::Char.AbilityActionNum==0) ) &&
		(FSM::Ctrl.state_Attack&&FSM::Char.AbilityActionNum==0)	&&	
		FSM::Input.ShiftDown==FALSE&&
		FSM::Input.CtrlBtnDown==FALSE&&
		FSM::Char.MainPlayer		&&
		FSM::Char.MainPlayer->isMoving==FALSE &&
		FSM::Char.Controller		&&
		FSM::Ability.UsableAbility	&&
		//FSM::Ability.AbilityEnable  &&
		FSM::Char.MoveEnable		&&
		FSM::CDTime.RunDelayTime<=0.0f &&
		FSM::CDTime.MoveDelayTime<=0.0f &&
		FSM::Char.IsDead==FALSE		&&
		FSM::Ability.PickTarget == TRUE &&
		(
		 (FSM::Ability.UsableAbility->ClientCommonAttribute.TargetSelectActor==TRUE  && FSM::Target.DesireMMOActor && FSM::Target.DesireAttackble && FSM::Target.DesireMMOActor->GetActorType()!=EACT_NPC) ||
		 (FSM::Ability.UsableAbility->ClientCommonAttribute.TargetSelectActor==FALSE && FSM::Target.CurTargetMMOActor && FSM::Target.TargetAttackble )
		) 
	)
	{
		
		//if(FSM::Char.AbilityActionNum!=0&&FSM::Target.DesireMMOActorChangeFlag){
		//	warnf(NAME_Warning, TEXT("------------------------FSMCtrl_MoveToAndUseAbility----------state_Switching_Flag[%d]  state_Attack[%d]  IsUsingAbility[%d] WaitingForTheAbilityRespond[%d]"),FSM::Ctrl.state_Switching_Flag,FSM::Ctrl.state_Attack,FSM::Char.MainPlayer->IsUsingAbility(),FSM::Char.Controller->WaitingForTheAbilityRespond);
		//}

		FSM::Ctrl.state_Switching_Flag = FALSE;
		FSM::Target.DesireMMOActorChangeFlag = FALSE;
		return TRUE;
	}

	//Lock after running for normal attack
	if( FSM::Ctrl.state_Attack==FALSE &&
		FSM::Char.AbilityActionNum==0 &&
		FSM::Ctrl.KeyType != KT_SECONDARY &&
		FSM::Input.ShiftDown==FALSE	&&
		FSM::Char.MainPlayer		&&
		FSM::Char.MainPlayer->isMoving==FALSE &&
		FSM::Char.Controller		&&
		FSM::Ability.UsableAbility	&&
		FSM::Ability.AbilityEnable  &&
		FSM::Char.MoveEnable		&&
		FSM::Char.IsDead==FALSE		&&
		FSM::Ability.PickTarget == TRUE &&
		FSM::Ability.UsableAbility->ClientCommonAttribute.TargetSelectActor==TRUE &&
		FSM::Target.DesireNPC &&
		FSM::Target.DesireAttackble 
		)
	{
		FSM::Ctrl.state_Attack = TRUE;
		FSM::Ctrl.state_Standby = FALSE;
		FSM::Ctrl.state_Moving  = FALSE;
	}

	//Cancel shift, cancel the current locked attack
	if( FSM::Input.ShiftDown==FALSE	&&
		FSM::Input.ShiftDown_Last==TRUE &&
		FSM::Char.AbilityActionNum>0 &&
		FSM::Input.CtrlBtnDown==FALSE&&
		FSM::Char.Controller		&&
		FSM::Char.MainPlayer		&&
		FSM::Target.DesireMMOPlayer &&
		FSM::Ability.UsableAbility	&&
		FSM::Ability.AbilityEnable  &&
		FSM::Char.MoveEnable		&&
		FSM::Char.IsDead==FALSE		&&
		FSM::Ability.PickTarget == TRUE
		)
	{
		FSM::Char.Controller->eventClearAllCmd();
	}


	//Attack entry while holding shift
	if( (FSM::Input.ShiftDown==TRUE||FSM::Ctrl.ctrlFlag_IgnoreShiftBtn)	&&
		FSM::Char.AbilityActionNum==0 &&
		FSM::Input.CtrlBtnDown==FALSE&&
		FSM::Char.MainPlayer		&&
		FSM::Char.MainPlayer->isMoving==FALSE &&
		FSM::Char.Controller		&&
		FSM::Ability.UsableAbility	&&
		//FSM::Ability.AbilityEnable  &&
		FSM::Char.MoveEnable		&&
		FSM::CDTime.RunDelayTime<=0.0f &&
		FSM::CDTime.MoveDelayTime<=0.0f &&
		FSM::Char.IsDead==FALSE		&&
		FSM::Ability.PickTarget == TRUE &&
		FSM::Target.DesireMMOActor &&
		FSM::Ctrl.KeyType != KT_SECONDARY &&
		FSM::Target.DesireMMOActor->GetActorType()!=EACT_Doodad &&
		//FSM::Ctrl.KeyType != KT_NONE &&
		FSM::Ability.UsableAbility->ClientCommonAttribute.TargetSelectActor==TRUE  &&
			( FSM::Target.DesireMMOActor->GetActorType()!=EACT_NPC||
			  FSM::Target.DesireAttackble)
		) 
	{

		//warnf(NAME_Warning, TEXT("----------------------------------FSMCtrl_MoveToAndUseAbility FSM::Ability.AbilityEnable=%d"),FSM::Ability.AbilityEnable);

		//if(FSM::Char.AbilityActionNum!=0&&FSM::Target.DesireMMOActorChangeFlag){
		//	warnf(NAME_Warning, TEXT("----------------------------------state_Switching_Flag[%d]  state_Attack[%d]  IsUsingAbility[%d] WaitingForTheAbilityRespond[%d]"),FSM::Ctrl.state_Switching_Flag,FSM::Ctrl.state_Attack,FSM::Char.MainPlayer->IsUsingAbility(),FSM::Char.Controller->WaitingForTheAbilityRespond);
		//}

		FSM::Ctrl.state_Switching_Flag = FALSE;
		FSM::Target.DesireMMOActorChangeFlag = FALSE;

		return TRUE;
	}

	return FALSE;

}

void FSMCtrl_MoveToAndUseAbility::Action()
{
	if(FSM::Char.Controller)
	{

		MMONavigationManager::getSingleton().StopNavigate(FALSE);
		FSM::Char.Controller->eventClearAllCmd();
		FSM::Trigger.IsDesireTrigger = FALSE;
		gServerGridWorld.SetDynamicBLock(FALSE);


		FLOAT dis = max(FLOAT(FSM::Ability.UsableAbility->SpellCommonAttribute.MaxDistance),128.0f+64.0f);
		if(FSM::Target.DesireMMOActor)
		{
			dis = max(dis,FSM::Target.DesireMMOActor->GetGridRadius()+64.0f);
		}

		UBOOL bRes = Mir2Attack::getSingleton().GetAbilityItemCostCondition(FSM::Ability.UsableAbility,TRUE);
		if(bRes==FALSE){
			return;
		}



		if(FSM::Ability.UsableAbility->ClientCommonAttribute.TargetSelectActor && FSM::Target.DesireMMOActor){
			//if((FSM::Char.MainPlayer->Location-FSM::Target.DesireMMOActor->Location).Size() >= dis)
			FVector D = FSM::Char.MainPlayer->ServerLocation;// >Location;
			ConvertXYToServer(D.X,D.Y);
			FVector S = FSM::Target.DesireMMOActor->ServerLocation;// >Location;
			ConvertXYToServer(S.X,S.Y);
			FVector disVector = D-S;

			UBOOL   bInRange = (abs(disVector.X)<=dis+32.0f)&&(abs(disVector.Y)<=dis+32.0f); 
			if(bInRange==FALSE)
			{
				//Chase over, end when within one casting range
				FSM::Char.Controller->eventAddCmdMoveToActor(FSM::Target.DesireMMOActor,FSM::Target.DesireMMOActor->ServerLocation,dis,0,TRUE,FALSE,FALSE,1,TRUE);
			}

			if(FSM::Target.AbilityTargetMMOActor && FSM::Ctrl.KeyType_Keeping!=KT_PRIMARY )
			{
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,FSM::Target.AbilityTargetMMOActor,FSM::Target.AbilityTargetMMOActor->ServerLocation,FALSE,-1,1);
			}
			else
			{
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,FSM::Target.DesireMMOActor,FSM::Target.DesireMMOActor->ServerLocation,FALSE,-1,1,TRUE);
			}
		}
		else if(FSM::Target.CurTargetMMOActor)
		{
			FVector D = FSM::Char.MainPlayer->ServerLocation;// >Location;
			ConvertXYToServer(D.X,D.Y);
			FVector S = FSM::Target.CurTargetMMOActor->ServerLocation;// >Location;
			ConvertXYToServer(S.X,S.Y);
			FVector disVector = D-S;

			UBOOL   bInRange = (abs(disVector.X)<=dis+32.0f)&&(abs(disVector.Y)<=dis+32.0f); 
			if(bInRange==FALSE)
			{
				//Chase over, end when within one casting range
				FSM::Char.Controller->eventAddCmdMoveToActor(FSM::Target.CurTargetMMOActor,FSM::Target.CurTargetMMOActor->ServerLocation,dis );
			}
			//if(FSM::Ability.AbilityEnable)
			{
				//Cast ability
				//warnf(NAME_Warning, TEXT("======FSMCtrl_MoveToAndUseAbility2 AbilityId[%d]"),		FSM::Ability.UsableAbility->SpellCommonAttribute.Id );
				FSM::Char.Controller->eventAddCmdUseAbility(FSM::Ability.UsableAbility,FSM::Target.CurTargetMMOActor,FSM::Target.CurTargetMMOActor->ServerLocation,FALSE);
			}
		}

	}
	
}

//Walk close and interact------------------------------------------------------------------------------------------------------
UBOOL FSMCtrl_MoveToAndInteract::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_MoveToAndInteract");

	if( FSM::Ctrl.state_Switching_Flag &&
		FSM::Ctrl.state_Interact  &&
		(FSM::Target.DesireInteractableActor || FSM::Target.DesireNPCTempleteID) &&
		(FSM::Target.CurTargetMMOPlayer==FALSE || FSM::Target.CurTargetMMOPlayer->eventIsStallState() )&&
		FSM::Char.MainPlayer->isMoving==FALSE &&
		FSM::Input.ShiftDown==FALSE	&&
		//FSM::Input.AltBtnDown==FALSE&&
		FSM::Char.MoveEnable		&&
		FSM::Char.Controller		&&
		FSM::Char.MainPlayer		&&
		FSM::Char.IsDead==FALSE		)
	{
		FSM::Ctrl.state_Switching_Flag = FALSE;
		FSM::Ctrl.state_Interact = FALSE;	// done interaction, prevent a second call when keys up
		return TRUE;
	}
	return FALSE;

}

void FSMCtrl_MoveToAndInteract::Action()
{
	//TScriptInterface<IIInteractableActor> interactableActor = ScriptInterfaceCast<IIInteractableActor>(FSM::Target.DesireActor);
	//if( FSM::Target.DesireInteractableActor || FSM::Target.DesireNPCTempleteID>0 )
	//{
		MMONavigationManager::getSingleton().StopNavigate(FALSE);
		FSMIO::getSingleton().Target.SetKeepRunning(FALSE);
		FSM::Char.Controller->eventClearAllCmd();


		FLOAT interactDist = 0.0f;
		if(FSM::Target.DesireInteractableActor)
		{
			interactDist = FSM::Target.DesireInteractableActor->GetMaxInteractRadius();
			if(FSM::Target.DesireNPC && FSM::Target.DesireAttackble==FALSE)
			{
				interactDist = 2048.0f;
			}

			FVector D = FSM::Char.MainPlayer->Location;
			ConvertXYToServer(D.X,D.Y);
			FVector S = FSM::Target.DesireInteractableActor->GetInteractActorLocation();
			ConvertXYToServer(S.X,S.Y);
			FVector Dis = D-S;

			//FVector Dis = FSM::Char.MainPlayer->Location - FSM::Target.DesireInteractableActor->GetInteractActorLocation();
			//ConvertCoordN2C(Dis);
			if(abs(Dis.X)>=interactDist||abs(Dis.Y)>=interactDist)
			{
				//Chase over,
				FSM::Char.Controller->eventAddCmdMoveToActor(FSM::Target.DesireNPC, FSM::Target.DesireInteractableActor->GetInteractActorLocation(), interactDist , FSM::Target.DesireNPCTempleteID,TRUE,FALSE,FALSE,1);
			}
		}
		else if(FSM::Target.DesireNPCTempleteID > 0)
		{
			interactDist = FSM::Char.Controller->InteractDistance;
			//Chase over,
			FSM::Char.Controller->eventAddCmdMoveToActor(NULL, FVector(0, 0, 0),  interactDist , FSM::Target.DesireNPCTempleteID);
		}

		//Perform interaction
		//stateController->SetSelectTarget(stateDesireActor);
		FSM::Char.Controller->eventAddCmdInteract(FSM::Target.DesireInteractableActor, 0, FSM::Target.DesireNPCTempleteID);
	//}
}

//Walk to target point, with mouse held, fast control of the target point to walk to-------------------------------------------------------------------------
UBOOL FSMCtrl_MoveToPoint::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_MoveToPoint");

	//Walk while in move state

	if( FSM::Char.Controller &&
		(FSM::Ctrl.state_Moving || FSM::Ctrl.state_Attack )&&
		(
		(FSM::Input.ShiftDown == FALSE && (FSM::Ctrl.KeyType == KT_PRIMARY || FSM::Char.Controller->bLeftMousePressed) && FSM::Ctrl.state_Moving) ||
			(FSM::Ctrl.KeyType == KT_SECONDARY || FSM::Char.Controller->bRightMousePressed) ||
			(FSM::Input.ShiftDown==FALSE && FSM::Ctrl.KeyType==KT_ELSE && FSM::Ability.UsableAbility && FSM::Ability.UsableAbility->SpellCommonAttribute.CanMoveInPrepare==TRUE)
		)&&
		//(FSM::Input.ShiftDown==FALSE || FSM::Ctrl.KeyType == KT_SECONDARY)&&
		//FSM::Input.ShiftDown==FALSE	&&
		FSM::Input.CtrlBtnDown==FALSE	&&
		FSM::Input.AltBtnDown==FALSE	&&
		//FSM::Char.Controller			&&
		FSM::Char.MainPlayer			&&
		FSM::Char.MainPlayer->isMoving==FALSE &&
		FSM::Char.nCannotInterruptLevel==0  &&
		//FSM::Char.MoveEnable 			&&
		FSM::Char.MoveEnableInAdvance	&&
		FSM::CDTime.MoveDelayTime<=0.0f  &&
		FSM::CDTime.MoveDelayFrame<=0 &&
		FSM::Char.IsDead==FALSE			)
	{
		return TRUE;
	}

	return FALSE;

}

void FSMCtrl_MoveToPoint::Action()
{
	UBOOL bWaitingSendMove = FSMIO::getSingleton().Trigger.GetWaitingSendMove();
	if(bWaitingSendMove==FALSE)
	{
		
		//FSM::Char.Controller->SetTargetLocationMarker(FSM::Target.CurOptimizeTargetPos, FSM::Target.CurTargetMMOActor);
		//if(FSM::Ctrl.KeyType == KT_PRIMARY && FSM::CDTime.WalkDelayTime<=0.0f)
		//{
		//	//warnf(NAME_Warning, TEXT("----------------------------------FSMCtrl_MoveToPoint::Walk"));
		//	gServerGridWorld.SetDynamicBLock(TRUE);
		//	FSM::Char.Controller->eventClearAllCmd();
		//	FSM::Char.Controller->eventAddCmdMoveToPoint(FSM::Target.CurOptimizeTargetPos,FALSE);
		//}
		if(FSM::Char.Controller->bRightMousePressed && FSM::CDTime.RunDelayTime<=0.0f)
		{	
			//warnf(NAME_Warning, TEXT("----------------------------------FSMCtrl_MoveToPoint::Run"));
			gServerGridWorld.SetDynamicBLock(TRUE);
			FSM::Char.Controller->eventClearAllCmd();
			FVector dest = FSM::Target.CurOptimizeTargetPos;
			FVector dis = dest-FSM::Char.MainPlayer->Location;
			dis.Z = 0.0;
			if(dis.X>-64.0f && dis.X<64.0f && dis.Y>-64.0f && dis.Y<64.0f)
			{
				FSM::Char.Controller->eventAddCmdMoveToPoint(dest,TRUE,FALSE);
			}
			else if(dis.X>-256.0f && dis.X<256.0f && dis.Y>-256.0f && dis.Y<256.0f)
			{
				dis.Normalize();
				dest = FSM::Char.MainPlayer->Location + dis*342.0f;
				FSM::Char.Controller->eventAddCmdMoveToPoint(dest,TRUE,FALSE);
			}
			else
			{

				FSM::Char.Controller->eventAddCmdMoveToPoint(dest,TRUE,FALSE);
			}
		}
		else if(FSM::Ctrl.KeyType == KT_PRIMARY && FSM::CDTime.WalkDelayTime<=0.0f)
		{
			//warnf(NAME_Warning, TEXT("----------------------------------FSMCtrl_MoveToPoint::Walk"));
			gServerGridWorld.SetDynamicBLock(TRUE);
			FSM::Char.Controller->eventClearAllCmd();
			FSM::Char.Controller->eventAddCmdMoveToPoint(FSM::Target.CurOptimizeTargetPos,FALSE);
		}
	}

	return;

}


//Focus activation------------------------------
UBOOL	FSMCtrl_FocusActor::Update(FLOAT DelTime)
{
	////Walk while in move state
	//if( FSM::CDTime.FocusTime>0.0 )
	//{
	//	return TRUE;
	//}
	return FALSE;
}
void	FSMCtrl_FocusActor::Action()
{
	//if( FSM::CDTime.FocusTime>1.0 &&
	//	FSM::Target.CurTargetMMOActor &&
	//	FSM::Char.Controller)
	//{
	//	FSM::CDTime.FocusTime = -1.0;
	//	FSM::Char.Controller->SendRequestSetFocus(FSM::Target.CurTargetMMOActor->ServerID);
	//}
}

//Test------------------------------
UBOOL FSMCtrl_Test::Update(FLOAT DelTime)
{
	if(	FSM::Char.MoveEnable &&
		FSM::Char.Controller &&
		FSM::Char.MainPlayer &&
		FSM::Char.MainPlayer->isMoving )
	{
		m_DelTime = DelTime;
		return TRUE;
	}
	return FALSE;
}

void FSMCtrl_Test::Action()
{
	static FVector data[100]; 
	static INT	   dataIndex=0;

	INT index = dataIndex%100; 
	data[index] = FSM::Char.MainPlayer->Location;
	
	if(index == 99)
	{
		FString out = TEXT("");
		for(INT i=99;i>0;i--){
			out = out + FString::Printf(TEXT("%f:%f; "),(data[i]-data[i-1]).Size2D(),m_DelTime );
		}
		warnf(NAME_Log, *out);
	}

	dataIndex++;

}

//Auto cast----------------------------------------------------
UBOOL FSMCtrl_AiAbility::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_AiAbility");

	if(	FSM::Char.MainPlayer		&&
		FSM::Char.Controller		&&
		FSM::Char.nCannotInterruptLevel==0 &&
		FSM::Char.IsDead==FALSE	)
	{
		return TRUE;
	}
	return FALSE;
}

void FSMCtrl_AiAbility::Action()
{
	
	UBOOL bRes = Mir2Attack::getSingleton().DoAIAbility();
	if(bRes)
	{
		warnf(NAME_Warning, TEXT("============================ DoAIAbility"));
		FSM::Char.AbilityActionNum	= FSM::Char.Controller->GetCmdNum(CMD_ABILITY);
	}
}

