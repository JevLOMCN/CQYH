#include "stdafx.h"

#include "MMOFacade.h"
#include "FSMIO.h"
#include "MMOSkillManager.h"
#include "Mir2Attack.h"
#include "AbilityProcessor.h"

IMPLEMENT_CLASS(UPlayerActionQueue)
IMPLEMENT_CLASS(UIPlayerActionEx)
IMPLEMENT_CLASS(UPlayerActionEx_Base)
IMPLEMENT_CLASS(UPlayerActionEx_Compositor)
IMPLEMENT_CLASS(UPlayerActionEx_UseAbility)
IMPLEMENT_CLASS(UPlayerActionEx_Waiting)
IMPLEMENT_CLASS(UPlayerActionEx_MoveTo)
IMPLEMENT_CLASS(UPlayerActionEx_MoveNearTo)
IMPLEMENT_CLASS(UPlayerActionEx_MoveNearToPoint)
IMPLEMENT_CLASS(UPlayerActionEx_Interact)
IMPLEMENT_CLASS(UPlayerActionEx_Teleport)
IMPLEMENT_CLASS(UPlayerActionEx_AutoAttack)
IMPLEMENT_CLASS(UPlayerActionEx_WaitingForClearCache)
IMPLEMENT_CLASS(UPlayerActionEx_OpenStall)
IMPLEMENT_CLASS(UPlayerActionEx_CannotInterrupt)
IMPLEMENT_CLASS(UIInteractableActor)

extern INT g_TotalFrameNum;
extern UBOOL g_PreMovetoPoint;
INT g_LastRequestAbilitySerialId = 0;
INT g_LastRequestAbilityId = 0;

FString UPlayerActionEx_Base::Dump()
{
	FString Ret = FString::Printf(L"bStart=%d bEnd=%d  bNeedReplacexxx=%d ", bStart, bEnd, bNeedReplaceTheFrontAction);
	return Ret;
}

void UPlayerActionEx_Base::Update(FLOAT DeltaTime)
{
	// lifecycle is controlled by the behavior itself, unified control is risky
//	if (!bStart)
//	{
//		PassedTime += DeltaTime;
//		if (PassedTime > LiftSpan)
//		{
//			bEnd = TRUE;
//			//debugf(L"3 UPlayerActionEx_Base Set bEnd = true");
//		}
//	}
}

void UPlayerActionEx_Base::ResetAction(UPlayerActionEx_Base* InAction)
{

}

void UPlayerActionEx_Base::OnAction(UBOOL bOn)
{
	if (bOn)
	{
		bStart = TRUE;
	}
	else
	{
		//debugf(L"4 UPlayerActionEx_Base Set bEnd = true");
		bEnd = TRUE;
	}
}

UBOOL UPlayerActionEx_Base::IsComplete()
{
	return bStart && bEnd;
}

UBOOL UPlayerActionEx_Base::IsStart()
{
	return bStart;
}

UBOOL UPlayerActionEx_Base::IsLatenAction()
{
	return FALSE;
}


FString UPlayerActionEx_MoveTo::Dump()
{
	UBOOL IsMoving = 8;
	AMMOActor* actor = (AMMOActor*)(ActionOwner->Pawn);
	if (actor)
	{
		IsMoving = actor->isMoving;
	}

	FString Ret = FString::Printf(L"ActionMoveTo Des(%f, %f), Loc(%f, %f)  IsMoving=%d", Destination.X, Destination.Y, ActionOwner->Pawn->Location.X, ActionOwner->Pawn->Location.Y, IsMoving);
	Ret += UPlayerActionEx_Base::Dump();
	return Ret;
}

void UPlayerActionEx_MoveTo::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_MoveTo");

	UPlayerActionEx_Base::Update(DeltaTime);

	//if (bEnd || !bStart)
	//{
	//	return;
	//}

	if (!ActionOwner)
	{
		bEnd = TRUE;
		return;
	}

	AMMOActor* MainPlayer = (AMMOActor*)(ActionOwner->Pawn);
	if(!MainPlayer)
	{
		bEnd = TRUE;
		return;
	}

	if(bStart==FALSE && bEnd==FALSE)//second trigger, wait for trigger
	{
		if(MainPlayer->CanMove())
		{
			//warnf(TEXT("F[%d]------------------------UPlayerActionEx_MoveTo::Update:NativeMoveToPoint "),g_TotalFrameNum);
			ActionOwner->NativeMoveToPoint(Destination,bRunOrWalk,bCrossJudge);
			FSMIO::getSingleton().Target.SetCacheAbility(NULL);
			bStart = TRUE;
		}
	}

	if(bStart==TRUE && bEnd==FALSE){
		if (!MainPlayer->isMoving || MainPlayer->Velocity.IsNearlyZero())
		{
			//debugf(L"Move_UPdate Set bEnd=true");
	#if 0 && defined(_DEBUG) // oscar@snda
			INT64 llSrcX;
			INT64 llSrcY;
			INT64 llSrcZ;
			ZONE_DATA* pDestZone = NULL;
			GRID_DATA* pDestGrid = NULL;

			gServerGridWorld.ConvertPosClient2Server(MainPlayer->Location, llSrcX, llSrcY, llSrcZ);
			gServerGridWorld.GetZoneAndGrid(llSrcX, llSrcY, llSrcZ, pDestZone, pDestGrid);

			if(pDestGrid)
			{
				if(pDestGrid->bOutOfPoly)
				{
					appDebugBreak();
				}
			}
	#endif

			bEnd = TRUE;
			return;
		}
	}

	CurrentTime += DeltaTime;
	if (CurrentTime > MaxMovedTime)
	{
		//debugf(L"2  Move_UPdate Set bEnd=true");
		bStart=TRUE;
		bEnd = TRUE;
	}
}

void UPlayerActionEx_MoveTo::ResetAction(UPlayerActionEx_Base* InAction)
{
	UPlayerActionEx_MoveTo* NewAction = Cast<UPlayerActionEx_MoveTo>(InAction);
	if (!NewAction)
	{
		return;
	}

	if (!ActionOwner)
	{
		return;
	}

	Destination = NewAction->Destination;
	CurrentTime = 0.f;
	if(MMOFacade::getSingleton().GetMainActor()->CanMove())
	{

		ActionOwner->NativeMoveToPoint(Destination,bRunOrWalk,bCrossJudge);
		FSMIO::getSingleton().Target.SetCacheAbility(NULL);
		bStart = TRUE;
		bEnd = FALSE;
	}
	else
	{
		bStart = FALSE;
		bEnd = FALSE;
	}

}

void UPlayerActionEx_MoveTo::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		return;
	}

	//UPlayerActionEx_Base::OnAction(bOn);

	if (bOn)
	{
		ActionOwner->bTryingMoveToMouse = FALSE;
		CurrentTime = 0.f;
		if(MMOFacade::getSingleton().GetMainActor()->CanMove())
		{
			//warnf(TEXT("F[%d]------------------------UPlayerActionEx_MoveTo::OnAction:NativeMoveToPoint "),g_TotalFrameNum);
			ActionOwner->NativeMoveToPoint(Destination,bRunOrWalk,bCrossJudge);
			FSMIO::getSingleton().Target.SetCacheAbility(NULL);
			bStart = TRUE;
			bEnd = FALSE;
		}
		else
		{
			bStart = FALSE;
			bEnd = FALSE;
		}
	}
	else
	{
		if (bStart)
		{
			if(bEnd)
			{
				bEnd = TRUE;
			}
			else
			{
				ActionOwner->StopMove(TRUE);
				bEnd = TRUE;
			}
		}
		else
		{
			bEnd = TRUE;
		}
	}
}

UBOOL UPlayerActionEx_MoveTo::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_MoveTo::IsLatenAction()
{
	return TRUE;
}

FString UPlayerActionEx_UseAbility::Dump()
{
	INT UsingCount = -100;
	AMMOActor* actor = Cast<AMMOActor>(ActionOwner->Pawn);
	BYTE cas = 4;
	if (actor)
	{
		cas = actor->CurrentAbilityState;
	}
	FString Ret = FString::Printf(L"ActionUseAbility AbilityId=%d CurrentAbilityState=%d", AbilityToUse->SpellCommonAttribute.Id, cas);
	Ret += UPlayerActionEx_Base::Dump();
	return Ret;
}

void UPlayerActionEx_UseAbility::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_UseAbility");

	UPlayerActionEx_Base::Update(DeltaTime);

	if (bEnd || !bStart)
	{
		return;
	}
	AMMOActor* actor = Cast<AMMOActor>(ActionOwner->Pawn);
	if (!actor)
	{
		//debugf(L"4 UPlayerActionEx_UseAbility Set bEnd = true");
		bEnd = TRUE;
		return;
	}

	//if (!actor->IsUsingAbility() && !actor->bWaitForAbilityReply)
	//{
	//	//debugf(L"5 UPlayerActionEx_UseAbility Set bEnd = true");
	//	bEnd = TRUE;
	//	return;
	//}
}

void UPlayerActionEx_UseAbility::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		bStart = bEnd = TRUE;
		return;
	}

	//UPlayerActionEx_Base::OnAction(bOn);

	if (bOn)
	{
//		warnf(NAME_Warning, TEXT("====================================enter cast ability flow"));

		bStart = TRUE;
		bEnd = TRUE;
		if (bCancelAbility)
		{
			bStart = TRUE;
			bEnd = TRUE;
			//debugf(L"1 AbilityOnAction Set bEnd = true");
#ifdef DEBUG_SYNC_INFO
			warnf(TEXT("Cancel ability-----------ID:%d"),AbilityToUse->SpellCommonAttribute.Id);
#endif
			CMessageHandler::GetInstance().SendRequestCancelAbility(AbilityToUse->SpellCommonAttribute.Id);// >SlotHotKeyID);
			ActionOwner->WaitingForTheAbilityBuf = -1.0f;
			ActionOwner->WaitingForTheAbilityRespond = 0;
			FSMIO::getSingleton().Ability.SetCurrentAbility(AbilityToUse);
			FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(-1.0f);

		}
		else
		{
			bStart = TRUE;
			bEnd = TRUE;
			//dynamically detect replacement of main-key ability
			if(bPrimAttackDynamicAbility)
			{
				UMMOAbility* pAbility = Mir2Attack::getSingleton().GetPrimaryAbility();
				if(pAbility)
				{
					AbilityToUse = pAbility;
				}
			}

			UBOOL bAbilityEnable	= ActionOwner->CanUseAnyAbility(AbilityToUse);
			if(bAbilityEnable==FALSE)
			{
#ifdef DEBUG_SYNC_INFO
				warnf(NAME_Warning, TEXT("============================ Time to cast the ability, but it cannot be cast"));
#endif
				return;
			}

			FVector UseLocation = TargetLocation;
			if (AbilityToUse && AbilityToUse->SpellCommonAttribute.LockType == E_splt_Direction /* || AbilityToUse->SpellCommonAttribute.LockType == ESL_Type4*/)
			{
				UseLocation = FixedLocation;
			}
			//ActionOwner->DrawDebugLine(ActionOwner->Pawn->Location, UseLocation, 255, 0, 0, TRUE);
			//debugf(L"2 AbilityOnAction SendRequest");
			//((AMMOActor*)(ActionOwner->Pawn))->bWaitForAbilityReply = TRUE;

			// check break move
			if (ActionOwner->GetPlayer()->ActorIsMoving() && AbilityToUse && AbilityToUse->SpellCommonAttribute.BreakMove)
			{
				ActionOwner->StopMove(TRUE);
			}

			Target = NULL;
			if(targetActorID==0 && TemplateID>0)//no locked target but a locked template ID, so automatically find the nearest target
			{
				Target = MMOFacade::getSingleton().GetMainController()->GetClosedMMOActorByTempleteID(TemplateID);
				if(Target==NULL)
				{
					//target not found, cancel casting ability
					return;
				}
			}
			else if(targetActorID>0)//when there is a locked target
			{
				if(targetActorType==1)
				{
					Target = FSMIO::getSingleton().Target.GetDesireMMOActor();

				}
				else{
					Target = MMOFacade::getSingleton().GetCharacter(targetActorID);
					if(Target==NULL)
					{
						//target has disappeared, cancel casting ability
						return;
					}
				}
			}


#ifdef DEBUG_SYNC_INFO
			warnf(TEXT("Cast ability-----------ID:%d, TargetID:%d, CurrentAbilityState:%d, CurAbilityId:%d"),AbilityToUse->SpellCommonAttribute.Id, Target!=NULL ? Target->ServerID : 0,  ActionOwner->GetPlayer()->CurrentAbilityState,  FSM::Ability.CurrentAbility?FSM::Ability.CurrentAbility->SpellCommonAttribute.Id:0 );
#endif

			if(AbilityToUse->SpellCommonAttribute.CanMoveInPrepare==FALSE)
			{
				g_PreMovetoPoint = FALSE;
			}


//			warnf(NAME_Warning, TEXT("====================================cast ability"));
			

			INT SerialID = ActionOwner->RequestUseAbilityEx(
				Target!=NULL ? Target->ServerID : 0,
				AbilityToUse->SpellCommonAttribute.Id,
				AbilityToUse->SpellCommonAttribute.Level,
				Target!=NULL ? Target->ServerLocation : UseLocation);

			ActionOwner->WaitingForTheAbilityRespond = AbilityToUse->SpellCommonAttribute.Id;
			FSMIO::getSingleton().Ability.SetCurrentAbility(AbilityToUse);
			FSMIO::getSingleton().Target.SetCacheAbility(NULL);

			if(AbilityToUse->SpellCommonAttribute.AttackType == E_spat_Mag || AbilityToUse->SpellCommonAttribute.AttackType == E_spat_Tao)
			{
				if(Target&&Target->GetActorType()==EACT_OtherPlayer)
				{
					FSMIO::getSingleton().CDTime.ResetCD_MoveDelay(0.3f+appFrand()*0.7f);
				}
			}
			FSMIO::getSingleton().CDTime.ResetCD_WalkAndRunDelay(AbilityToUse->SpellCommonAttribute.WalkDelayTime,AbilityToUse->SpellCommonAttribute.RunDelayTime);


			if (AbilityToUse->SpellCommonAttribute.CastType == E_spct_Active)
			{
				AMMOPlayer* pMainPlayer = ActionOwner->GetPlayer();
				if (pMainPlayer)
				{
					pMainPlayer->CurrentAbilityState = EAS_Requested;
					g_LastRequestAbilitySerialId = pMainPlayer->CurrentRequestAbilitySerialId;
					g_LastRequestAbilityId = pMainPlayer->CurrentRequestAbilityId;
					pMainPlayer->CurrentRequestAbilitySerialId = SerialID;
					pMainPlayer->CurrentRequestAbilityId = AbilityToUse->SpellCommonAttribute.Id;
				}
			}

			if(AbilityToUse->bClientPlayFirst)
			{
				if(AbilityToUse->bClientRotateFirst)
				{
					Mir2Attack::getSingleton().PreRotate(Target!=NULL ? Target->ServerLocation : UseLocation);
				}
								
				MMOFacade::getSingleton().GetAbilityProcessor()->PlayAbilityBeforeServer(
					ActionOwner->GetPlayer()->ServerID,
					AbilityToUse->SpellCommonAttribute.Id,
					AbilityToUse->SpellCommonAttribute.Level,
					AbilityToUse->SpellCommonAttribute.RuneSlotID,
					Target!=NULL ? Target->ServerID : 0,
					Target!=NULL ? Target->ServerLocation : UseLocation,
					SerialID
					);

				FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(5.0f);
			}
			else
			{
				FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(0.2f);
			}

			
		}
	}
	else
	{
		//if (bStart)
		//{
		//	if(bEnd)
		//	{
		//		bEnd = TRUE;
		//		debugf(L"2 AbilityOnAction Set bEnd = true");
		//	}
		//	else
		//	{
		//		CMessageHandler::GetInstance().SendRequestCancelAbility(AbilityToUse->SlotHotKeyID);
		//		bEnd = TRUE;
		//		//debugf(L"3 AbilityOnAction Set bEnd = true");
		//	}
		//}
		//else
		//{
		//	bEnd = TRUE;
		//	//debugf(L"4 AbilityOnAction Set bEnd = true");
		//}
	}
}

UBOOL UPlayerActionEx_UseAbility::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_UseAbility::IsLatenAction()
{
	return FALSE;
}


//---------------------------------------------------------------------------------------


FString UPlayerActionEx_Waiting::Dump()
{
	FString Ret = FString::Printf(L"UPlayerActionEx_Waiting");
	Ret += UPlayerActionEx_Base::Dump();
	return Ret;
}

void UPlayerActionEx_Waiting::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_Waiting");

	UPlayerActionEx_Base::Update(DeltaTime);

	if (bEnd || !bStart)
	{
		return;
	}

	if (ActionOwner==NULL)
	{
		bEnd = TRUE;
		return;
	}

	AMMOActor* actor = Cast<AMMOActor>(ActionOwner->Pawn);
	if (!actor)
	{
		//debugf(L"4 UPlayerActionEx_UseAbility Set bEnd = true");
		bEnd = TRUE;
		return;
	}

	if (actor->IsUsingAbility() )  //  && !actor->bWaitForAbilityReply)
	{
		return;
	}

	//if (actor->ActorAttributes && actor->ActorAttributes->byCombatPoseOn==TRUE )  //  && !actor->bWaitForAbilityReply)
	//{
	//	return;
	//}

	if (actor->ActorAttributes && actor->ActorAttributes->MountID>0 )  //  && !actor->bWaitForAbilityReply)
	{
		return;
	}

	//if (actor->bHoldWeapon==FALSE )  //  && !actor->bWaitForAbilityReply)
	//{
	//	return;
	//}

	if( ActionOwner->WaitingForEquipMount>0.0 )
	{
		return;
	}

	if( ActionOwner->WaitingForTheCombatPose>0.0 )
	{
		return;
	}

	if( ActionOwner->WaitingForTheAbilityBuf>0.0 )
	{
		return;
	}

	if( ActionOwner->WaitingForTheAbilityRespond>0)
	{
		return;
	}

	if (actor->CanUseAnyAbility()==FALSE)
	{
		return;
	}

	if (actor->CurrentAbilityState == EAS_Cast)
	{
		return;
	}


	bEnd = TRUE;
}

void UPlayerActionEx_Waiting::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		return;
	}

	if (bOn)
	{
		bStart = TRUE;
		bEnd = FALSE;
	}
	else
	{
		bStart = TRUE;
		bEnd = TRUE;
	}
}

UBOOL UPlayerActionEx_Waiting::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_Waiting::IsLatenAction()
{
	return FALSE;
}





//---------------------------------------------------------------------------------------
FString UPlayerActionEx_AutoAttack::Dump()
{
	FString Ret = FString::Printf(L"ActionAutoAttack");
	Ret += UPlayerActionEx_Base::Dump();
	return Ret;
}

void UPlayerActionEx_AutoAttack::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_AutoAttack");

	UPlayerActionEx_Base::Update(DeltaTime);

	if (bEnd || !bStart)
	{
		return;
	}
	AMMOActor* actor = Cast<AMMOActor>(ActionOwner->Pawn);
	if (!actor)
	{
		bEnd = TRUE;
		return;
	}

}

void UPlayerActionEx_AutoAttack::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		bStart = bEnd = TRUE;
		return;
	}

	if (bOn)
	{
		bStart = TRUE;
		bEnd = TRUE;

		AMMOActor* targetActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetClosedMMOActorByTempleteID(TemplateID));
		if(targetActor && MMOFacade::getSingleton().IsAttackable(targetActor))
		{
			UMMOAbility* pAbility = AbilityToUse;
			if (pAbility == NULL)
			{
				//pAbility = MMOTargetManager::getSingleton().GetDefaultAbility();
				//AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
				//UUIKeyMapManager* keyMapMgr = Cast<AMMOGame>(ctrl->WorldInfo->Game)->KeyMapManager;
				//if (keyMapMgr)
				//{
				//	INT abilityId = keyMapMgr->GetCmdIDBySlotID(ECK_MouseL);
				//	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
				//	if (mainPlayer)
				//	{
				//		pAbility = mainPlayer->GetEquipedAbilityById(abilityId);
				//	}
				//}
				pAbility = MMOSkillManager::getSingleton().GetEquippedSkill( ECK_Slot1 );
			}
			//MMOTargetManager::getSingleton().SetDesireTarget(pAbility,targetActor);
			FSMIO::getSingleton().Target.SetDesireTarget(pAbility,targetActor);
		}
	}
}

UBOOL UPlayerActionEx_AutoAttack::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_AutoAttack::IsLatenAction()
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------------
void UPlayerActionQueue::Add(UPlayerActionEx_Base* InAction)
{
	if (!InAction)
	{
		return;
	}

	//warnf(TEXT("____________[UPlayerActionQueue::Add] %s"), *InAction->GetName());

	if (InAction->bNeedReplaceTheFrontAction)
	{
		if (PlayerActionList.Num() > 0)
		{
			if (PlayerActionList(0)->IsLatenAction())
			{
				if (InAction->IsLatenAction())
				{
					PlayerActionList(0)->ResetAction(InAction);
					if (PlayerActionList.Num() > 1)
					{
						PlayerActionList.Remove(1, PlayerActionList.Num() - 1);
					}
					return;
				}
				else
				{
					PlayerActionList(0)->OnAction(FALSE);
					PlayerActionList.Empty();
				}
			}
			else
			{
				PlayerActionList.Empty();
			}
		}

		PlayerActionList.AddItem(InAction);
	}
	else
	{
		PlayerActionList.AddItem(InAction);
	}

	Update(0.001);
}
//------------------------------------------------------------------------
void UPlayerActionQueue::CancelAll()
{
	PlayerActionList.Empty();
}

//------------------------------------------------------------------------
INT UPlayerActionQueue::GetCmdNum(BYTE Type)
{
	INT num = 0;
	if(Type==CMD_ABILITY)
	{
		for (INT i = 0; i < PlayerActionList.Num(); ++i)
		{
			UPlayerActionEx_UseAbility* UseAbility = Cast<UPlayerActionEx_UseAbility>(PlayerActionList(i));
			if(UseAbility)
			{
				num++;
			}
		}
	}
	if(Type==CMD_Interrupt)
	{
		for (INT i = 0; i < PlayerActionList.Num(); ++i)
		{
			UPlayerActionEx_CannotInterrupt* pCannotInterrupt = Cast<UPlayerActionEx_CannotInterrupt>(PlayerActionList(i));
			if(pCannotInterrupt && pCannotInterrupt->InterruptLevelTime>0.0f)
			{
				num++;
			}
		}
	}
	return num;
}

//------------------------------------------------------------------------
void UPlayerActionQueue::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionQueue");

	if (PlayerActionList.Num() > 0)
	{
		for (INT i = 0; i < PlayerActionList.Num(); ++i)
		{
			PlayerActionList(i)->Update(DeltaTime);
		}

		if (PlayerActionList.Num() > 0)
		{
			if (!PlayerActionList(0)->IsStart())
			{
				PlayerActionList(0)->OnAction(TRUE);
			}
		}

		UBOOL bRemove = FALSE;
		for (INT i = 0; i < PlayerActionList.Num(); )
		{
			if (PlayerActionList(i)->IsComplete())
			{
				PlayerActionList.Remove(i);
				bRemove = TRUE;
			}
			else
			{
				++i;
			}
		}

		if(bRemove)
		{
			if (PlayerActionList.Num() > 0)
			{
				if (!PlayerActionList(0)->IsStart())
				{
					PlayerActionList(0)->OnAction(TRUE);
				}
			}
		}
	}

	//for (INT i = 0; i < PlayerActionList.Num(); ++i)
	//{
	//	FString dump = PlayerActionList(i)->Dump();
	//	debugf(L"AfterUpdate %d %s", i, *dump);
	//}
}

void UPlayerActionEx_Compositor::Update(FLOAT DeltaTime)
{
	//for (INT i = 0; i < PlayerActionList.Num(); ++i)
	//{
	//	if(!PlayerActionList(i)->IsCompleted())
	//	{
	//		PlayerActionList(i)->Update(DeltaTime);
	//		return;
	//	}
	//}
}
//------------------------------------------------------------------------
UBOOL UPlayerActionEx_Compositor::IsCompleted()
{
	//for (INT i = 0; i < PlayerActionList.Num(); ++i)
	//{
	//	if(!PlayerActionList(i)->IsCompleted())
	//	{
	//		return FALSE;
	//	}
	//}

	return TRUE;
}
//------------------------------------------------------------------------
void UPlayerActionEx_Compositor::Cancel()
{
	//for (INT i = 0; i < PlayerActionList.Num(); ++i)
	//{
	//	PlayerActionList(i)->Cancel();
	//}
}
//------------------------------------------------------------------------
UBOOL UPlayerActionEx_Compositor::IsMoveAction()
{
	//for (INT i = 0; i < PlayerActionList.Num(); ++i)
	//{
	//	// not finished and currently moving, can be interrupted
	//	if(!PlayerActionList(i)->IsCompleted() 
	//		&& PlayerActionList(i)->IsMoveAction())
	//	{
	//		return TRUE;
	//	}
	//}

	return FALSE;
}



//-------------------------------------------------------------------------

FString UPlayerActionEx_MoveNearTo::Dump()
{
	UBOOL IsMoving = 8;
	AMMOActor* actor = (AMMOActor*)(ActionOwner->Pawn);
	if (actor)
	{
		IsMoving = actor->isMoving;
	}

	FString Ret = FString::Printf(L"ActionMoveNearTo Des(%f, %f), Loc(%f, %f)  IsMoving=%d", Destination.X, Destination.Y, ActionOwner->Pawn->Location.X, ActionOwner->Pawn->Location.Y, IsMoving);
	Ret += UPlayerActionEx_Base::Dump();
	return Ret;
}

void UPlayerActionEx_MoveNearTo::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_MoveNearTo");

	UPlayerActionEx_Base::Update(DeltaTime);

	if(bStart==FALSE)
	{
		return;
	}

	if (bEnd)
	{
		return;
	}

	if (!ActionOwner)
	{
		bEnd = TRUE;
		return;
	}

	AMMOActor* actor = (AMMOActor*)(ActionOwner->Pawn);
	if (!actor)
	{
		bEnd = TRUE;
		return;
	}

	if(actor->isMoving)
	{
		return;
	}

	//if(actor->IsUsingAbility())
	//{
	//	return;
	//}

	//debugf(TEXT("---------UPlayerActionEx_MoveNearTo::Update"));

	if(targetActorID==0)
	{
		FVector Dis = actor->Location-Destination;
		FLOAT   Distance = max(abs(Dis.X),abs(Dis.Y));
		if(Distance<MinDistance)
		{
			bEnd = TRUE;
			return; 
		}

		if(Distance<256.0){
			ActionOwner->WantToMove(Destination,FALSE);
		}
		else
		{
			ActionOwner->WantToMove(Destination,TRUE);
		}
	}
	else
	{
		AMMOActor* pTargetActor = NULL;
		if(targetActorType==1)
		{
			pTargetActor = FSMIO::getSingleton().Target.GetDesireMMOActor();
		}
		else{
			pTargetActor = (AMMOActor*)MMOFacade::getSingleton().GetActor(targetActorID);
		}

		if(pTargetActor==NULL)
		{
			bEnd = TRUE;
			return;
		}

		if(bPrimAttackDynamicRange)
		{
			if(pTargetActor->AttackAi_LockTime<=0.0f)
			{
				if(Mir2Attack::getSingleton().InPrimaryAbilityRange(pTargetActor->ServerLocation,pTargetActor->BodySize>=E_body_3x3))
				{
					bEnd = TRUE;
					warnf(NAME_Warning, TEXT("==================================== chase entered main ability range"));
					return; 
				}

				if(pTargetActor->isMoving==TRUE && (pTargetActor->MoveType==EMT_Run||pTargetActor->MoveType==EMT_Run_Oblique))
				{
					FLOAT dis = (actor->ServerLocation-pTargetActor->LastServerLocation).Size2D();
					if(dis<370.0f)
					{
						if(Mir2Attack::getSingleton().InPrimaryAbilityRange(pTargetActor->LastServerLocation,pTargetActor->BodySize>=E_body_3x3))
						{
							bEnd = TRUE;
							warnf(NAME_Warning, TEXT("==================================== preparing an empty swing!!!!"));
							return; 
						}
					}
				}
			}
			else
			{
				warnf(NAME_Warning, TEXT("==================================== chase attack, border lock time"));
			}
			//FVector VirtualDes = pTargetActor->ServerLocation-actor->ServerLocation;
			//VirtualDes.Z = 0.0f;
			//VirtualDes.Normalize();
			//VirtualDes = VirtualDes*128.0f;
			if(pTargetActor->isMoving){
				ActionOwner->WantToMove(pTargetActor->ServerLocation,TRUE);
			}
			else
			{
				ActionOwner->WantToMove(pTargetActor->ServerLocation,TRUE,128.0f*1.9f);
			}
		}
		else{
			FVector D = pTargetActor->ServerLocation;//  >Location;
			ConvertXYToServer(D.X,D.Y);
			FVector S = actor->ServerLocation;
			ConvertXYToServer(S.X,S.Y);
			FVector Dis = D-S;
			FLOAT   Distance = max(abs(Dis.X),abs(Dis.Y));
			if(Distance<MinDistance)
			{
				if(FollowFlag==FALSE)
				{
					bEnd = TRUE;
					warnf(NAME_Warning, TEXT("==================================== chase entered range"));
					return; 
				}
				else
				{
					return;
				}
			}
			ActionOwner->WantToMove(pTargetActor->ServerLocation,TRUE);
		}

	}
}

void UPlayerActionEx_MoveNearTo::ResetAction(UPlayerActionEx_Base* InAction)
{
	UPlayerActionEx_MoveNearTo* NewAction = Cast<UPlayerActionEx_MoveNearTo>(InAction);
	if (!NewAction)
	{
		return;
	}

	if (!ActionOwner)
	{
		return;
	}

	Destination		= NewAction->Destination;
	TargetLastLocation = Destination;

	targetActorID	= NewAction->targetActorID;
	MinDistance		= NewAction->MinDistance;
	MaxDistance		= NewAction->MaxDistance;
	CurUpdateTime	= 0.0;

	if(MMOFacade::getSingleton().GetMainActor()->CanMove())
	//if(MMOFacade::getSingleton().GetMainActor()->bFreeze == FALSE && MMOFacade::getSingleton().GetMainActor()->bStun == FALSE && MMOFacade::getSingleton().GetMainController()->bIsInBossQTE==FALSE)
	{
		if(targetActorID!=0)
		{
			AMMOActor* pActor = NULL;
			if(targetActorType==1)
			{
				pActor = FSMIO::getSingleton().Target.GetDesireMMOActor();
			}
			else{
				pActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetActor(targetActorID));
			}
			if(pActor)
			{
				Destination = pActor->ServerLocation;// >Location;
				TargetLastLocation = Destination;
			}
		}
		ActionOwner->NativeMoveToPoint(Destination,TRUE);
		CurrentTime = 0.f;
		bStart = TRUE;
		bEnd = FALSE;
	}
	else
	{
		bStart = TRUE;
		bEnd = TRUE;
	}


}

void UPlayerActionEx_MoveNearTo::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		return;
	}

	//UPlayerActionEx_Base::OnAction(bOn);

	if (bOn)
	{
		CurUpdateTime = 0.0;
		ActionOwner->bTryingMoveToMouse = FALSE;
		if(MMOFacade::getSingleton().GetMainActor()->CanMove())
		//if(MMOFacade::getSingleton().GetMainActor()->bFreeze == FALSE && MMOFacade::getSingleton().GetMainActor()->bStun == FALSE && MMOFacade::getSingleton().GetMainController()->bIsInBossQTE==FALSE)
		{
			if(targetActorID!=0)
			{
				AMMOActor* pActor=NULL;
				if(targetActorType==1)
				{
					pActor = FSMIO::getSingleton().Target.GetDesireMMOActor();
				}
				else{
					pActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetActor(targetActorID));
				}
				if(pActor)
				{
					Destination = pActor->ServerLocation;// >Location;
					TargetLastLocation = Destination;
				}
			}
			else if(TemplateID>0)
			{
				AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
				if( TemplateID > 0 && pc != NULL )
				{
					AMMOActor* pActor = NULL;
					
					if (!IsDoodad)
					{
						pActor = pc->GetClosedMMOActorByTempleteID(TemplateID);
					} 
					else
					{
						pActor = pc->GetClosedMMODoodadByTempleteID(TemplateID);
					}

					if(pActor)
					{
						Destination = pActor->ServerLocation;// ->Location;
						TargetLastLocation = Destination;
					}
				}
			}
		 	
			if(FollowFlag && targetActorID>0)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.RClickMenu.Follow")),MsgInfoType_Table,TEXT("MMOGame.RClickMenu.Follow") );	
			}

			g_PreMovetoPoint = FALSE;

			//ActionOwner->NativeMoveToPoint(Destination,TRUE);
			CurrentTime = 0.f;
			bStart = TRUE;
			bEnd = FALSE;

		}
		else
		{
			bStart = TRUE;
			bEnd = TRUE;
		}
	}
	else
	{
		if (bStart)
		{
			if(bEnd)
			{
				bEnd = TRUE;
			}
			else
			{
				ActionOwner->StopMove(TRUE);
				bEnd = TRUE;
			}
		}
		else
		{
			bEnd = TRUE;
		}
	}
}

UBOOL UPlayerActionEx_MoveNearTo::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_MoveNearTo::IsLatenAction()
{
	return TRUE;
}

//-------------------------------------------------------------------------
// 
// FString UPlayerActionEx_PickUpItem::Dump()
// {
// 	return UPlayerActionEx_Base::Dump();
// }
// 
// void UPlayerActionEx_PickUpItem::Update(FLOAT DeltaTime)
// {
// 	UPlayerActionEx_Base::Update(DeltaTime);
// 	if (bStart && !bEnd)
// 	{
// 		PassedTime += DeltaTime;
// 		const FLOAT maxPickDistance = 
// 			MMOFacade::getSingleton().GetMainController()->MaxPickDistance * 
// 			MMOFacade::getSingleton().GetMainController()->MaxPickDistance;
// 		FVector playerPos = MMOFacade::getSingleton().GetMainPlayer()->Location;
// 		
// 		FLOAT dist = (playerPos - target->Location).SizeSquared2D();
// 		if(dist < maxPickDistance)
// 		{
// 			target->Pickup();			
// 			bEnd = TRUE;
// 		}
// 	}
// }
// 
// void UPlayerActionEx_PickUpItem::ResetAction(UPlayerActionEx_Base* InAction)
// {
// 	UPlayerActionEx_PickUpItem* NewAction = Cast<UPlayerActionEx_PickUpItem>(InAction);
// 	if (!NewAction)
// 	{
// 		return;
// 	}
// 
// 	if (!ActionOwner)
// 	{
// 		return;
// 	}
// 
// 	if(MMOFacade::getSingleton().GetMainActor()->bFreeze == FALSE)
// 	{
// 		bStart = TRUE;
// 		bEnd = FALSE;
// 	}
// 	else
// 	{
// 		bStart = TRUE;
// 		bEnd = TRUE;
// 	}
// }
// 
// void UPlayerActionEx_PickUpItem::OnAction(UBOOL bOn)
// {
// 	if (!ActionOwner)
// 	{
// 		bStart = bEnd = TRUE;
// 		return;
// 	}
// 
// 	//UPlayerActionEx_Base::OnAction(bOn);
// 
// 	if (bOn)
// 	{
// 		bStart = TRUE;
// 		//bEnd = TRUE;
// 
// 	}
// 	//else
// 	//{
// 	//	//if (bStart)
// 	//	//{
// 	//	//	if(bEnd)
// 	//	//	{
// 	//	//		bEnd = TRUE;
// 	//	//		debugf(L"2 AbilityOnAction Set bEnd = true");
// 	//	//	}
// 	//	//	else
// 	//	//	{
// 	//	//		CMessageHandler::GetInstance().SendRequestCancelAbility(AbilityToUse->SlotHotKeyID);
// 	//	//		bEnd = TRUE;
// 	//	//		//debugf(L"3 AbilityOnAction Set bEnd = true");
// 	//	//	}
// 	//	//}
// 	//	//else
// 	//	//{
// 	//	//	bEnd = TRUE;
// 	//	//	//debugf(L"4 AbilityOnAction Set bEnd = true");
// 	//	//}
// 	//}
// }
// 
// UBOOL UPlayerActionEx_PickUpItem::IsComplete()
// {
// 	return bEnd;
// }
// 
// UBOOL UPlayerActionEx_PickUpItem::IsLatenAction()
// {
// 	return FALSE;
// }

//-------------------------------------------------------------------------

FString UPlayerActionEx_Interact::Dump()
{
	return UPlayerActionEx_Base::Dump();
}

void UPlayerActionEx_Interact::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_Interact");

	//UPlayerActionEx_Base::Update(DeltaTime);
	if (bStart  &&  bEnd==FALSE )
	{
		if(target && Cast<AActor>(target->GetUObjectInterfaceIInteractableActor()))		//   target)
		{

			//debugf(L"Interact");
			target->DoInteract();
			bEnd = TRUE;
			return;
			/*
			ANPC* npc = Cast<ANPC>(target);
			if( npc && !npc->IsMonster())
			{
				CMessageHandler::GetInstance().Send_InteractNpc(npc);
				bEnd = TRUE;
				return;
			}	

			AMMOPlayer* player = Cast<AMMOPlayer>(target); 
			if( player && EMT_InStall == player->MoveType)
			{
				CMessageHandler::GetInstance().Send_ViewStallItem( player->ServerID );
				bEnd = TRUE;
				return;
			}

			ATeleportOrig* teleportOrig = Cast<ATeleportOrig>(target); 
			if( teleportOrig )
			{
				teleportOrig->HandleTeleport();
				bEnd = TRUE;
				return;
			}*/
			
		}
		else
		{
			AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
			if( TemplateID > 0 && pc != NULL )
			{
				if (IsDoodad)
				{
					target = ScriptInterfaceCast<IIInteractableActor>(pc->GetMMODoodadByTempleteID(TemplateID));
				} 
				else
				{
					target = ScriptInterfaceCast<IIInteractableActor>(pc->GetMMOActorByTempleteID(TemplateID));
				}
			}

			//bool isMMOActor = ( Cast<AMMOActor>(target) != NULL );
			//if( isMMOActor && pc != NULL )
			if(target)
			{
				//SetFocusActor(target);
				pc->SetSelectTarget(Cast<AActor>(target->GetUObjectInterfaceIInteractableActor()));
				target->DoInteract();
				
				//MMOTargetManager::getSingleton().SetDesireTarget(NULL, Cast<AActor>(target->GetUObjectInterfaceIInteractableActor()));
				//MMOTargetManager::getSingleton().SetSelectMMOActor(Cast<AMMOActor>(target->GetUObjectInterfaceIInteractableActor()));
			}
			bEnd = TRUE;
			return;
		}
	}
}

void UPlayerActionEx_Interact::ResetAction(UPlayerActionEx_Base* InAction)
{
	UPlayerActionEx_Interact* NewAction = Cast<UPlayerActionEx_Interact>(InAction);
	if (!NewAction)
	{
		return;
	}

	if (!ActionOwner)
	{
		return;
	}

	if(MMOFacade::getSingleton().GetMainActor()->CanMove())
	//if(MMOFacade::getSingleton().GetMainActor()->bFreeze == FALSE && MMOFacade::getSingleton().GetMainActor()->bStun == FALSE && MMOFacade::getSingleton().GetMainController()->bIsInBossQTE==FALSE)
	{
		bStart = TRUE;
		bEnd = FALSE;
	}
	else
	{
		bStart = TRUE;
		bEnd = TRUE;
	}
}

void UPlayerActionEx_Interact::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		bStart = bEnd = TRUE;
		return;
	}
	if (bOn)
	{
		bStart = TRUE;
	}
}

UBOOL UPlayerActionEx_Interact::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_Interact::IsLatenAction()
{
	return FALSE;
}

// add by zhangrui05@snda.com
FString UPlayerActionEx_Teleport::Dump()
{
	FString Ret = UPlayerActionEx_Base::Dump();
	return Ret;
}

void UPlayerActionEx_Teleport::Update(FLOAT DeltaTime)
{
	//UPlayerActionEx_Base::Update(DeltaTime);
}

void UPlayerActionEx_Teleport::ResetAction(UPlayerActionEx_Base* InAction)
{
	UPlayerActionEx_Teleport* NewAction = Cast<UPlayerActionEx_Teleport>(InAction);
	if (!NewAction)
	{
		return;
	}

	if (!ActionOwner)
	{
		return;
	}
}

void UPlayerActionEx_Teleport::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		return;
	}

	if (bOn)
	{
		CMessageHandler::GetInstance().Send_Teleport(LinkId);
		bEnd = TRUE;
	}
}

UBOOL UPlayerActionEx_Teleport::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_Teleport::IsLatenAction()
{
	return TRUE;
}


// add by zhangrui05@snda.com
FString UPlayerActionEx_OpenStall::Dump()
{
	FString Ret = UPlayerActionEx_Base::Dump();
	return Ret;
}

void UPlayerActionEx_OpenStall::Update(FLOAT DeltaTime)
{
	//UPlayerActionEx_Base::Update(DeltaTime);
}

void UPlayerActionEx_OpenStall::ResetAction(UPlayerActionEx_Base* InAction)
{
	UPlayerActionEx_OpenStall* NewAction = Cast<UPlayerActionEx_OpenStall>(InAction);
	if (!NewAction)
	{
		return;
	}

	if (!ActionOwner)
	{
		return;
	}
}

void UPlayerActionEx_OpenStall::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		return;
	}

	if (bOn)
	{
		ActionOwner->StopMove(TRUE);
		CMessageHandler::GetInstance().Send_OpenStall();
		bEnd = TRUE;
		bStart = TRUE;
	}
}

UBOOL UPlayerActionEx_OpenStall::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_OpenStall::IsLatenAction()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

FString UPlayerActionEx_MoveNearToPoint::Dump()
{
	UBOOL IsMoving = 8;
	AMMOActor* actor = (AMMOActor*)(ActionOwner->Pawn);
	if (actor)
	{
		IsMoving = actor->isMoving;
	}

	FString Ret = FString::Printf(L"ActionMoveTo Des(%f, %f), Loc(%f, %f)  IsMoving=%d", Destination.X, Destination.Y, ActionOwner->Pawn->Location.X, ActionOwner->Pawn->Location.Y, IsMoving);
	Ret += UPlayerActionEx_Base::Dump();
	return Ret;
}

void UPlayerActionEx_MoveNearToPoint::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_MoveNearToPoint");

	UPlayerActionEx_Base::Update(DeltaTime);

	if (bEnd || !bStart)
	{
		return;
	}

	if (!ActionOwner)
	{
		bEnd = TRUE;
		return;
	}


	AMMOActor* actor = (AMMOActor*)(ActionOwner->Pawn);
	if (!actor)
	{
		bEnd = TRUE;
		return;
	}

	if (bStart && !bEnd)
	{
		CheckTime-=DeltaTime;
		if(FSMIO::getSingleton().Trigger.GetWaitingSendMove())
		{
			//resend
			if(CheckTime<0.0 && MMOFacade::getSingleton().GetMainActor()->CanMove()){
				ActionOwner->NativeMoveToPoint(Destination,TRUE);
				CheckTime = 0.125f;
			}
		}
		else
		{
			bEnd = TRUE;
			return;
		}
	}

	CurrentTime += DeltaTime;
	if (CurrentTime > MaxMovedTime)
	{
		bEnd = TRUE;
		return;
	}
}

void UPlayerActionEx_MoveNearToPoint::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		return;
	}

	if (bOn)
	{
		ActionOwner->bTryingMoveToMouse = FALSE;

		if(MMOFacade::getSingleton().GetMainActor()->CanMove())
		//if(MMOFacade::getSingleton().GetMainActor()->bFreeze == FALSE && MMOFacade::getSingleton().GetMainActor()->bStun == FALSE && MMOFacade::getSingleton().GetMainController()->bIsInBossQTE==FALSE )
		{
			if(MMOFacade::getSingleton().GetMainActor()->CanMove()){
				ActionOwner->NativeMoveToPoint(Destination,TRUE);
			}
			FSMIO::getSingleton().Trigger.SetWaitingSendMove(TRUE);
			CurrentTime = 0.f;
			CheckTime = 0.125f;
			bStart = TRUE;
			bEnd = FALSE;

		}
		else
		{
			bStart = TRUE;
			bEnd = TRUE;
		}
	}

}

UBOOL UPlayerActionEx_MoveNearToPoint::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_MoveNearToPoint::IsLatenAction()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
extern INT32 g_SendClearAbilityCacheTimes;
extern INT32 g_RecieveClearAbilityCacheTimes;
extern FLOAT g_UpdateClearAbilityCacheTime;
extern UBOOL g_PreMovetoPoint;
FString UPlayerActionEx_WaitingForClearCache::Dump()
{
	return UPlayerActionEx_Base::Dump();
}

void UPlayerActionEx_WaitingForClearCache::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_WaitingForClearCache");

	UPlayerActionEx_Base::Update(DeltaTime);

	if (bEnd || !bStart)
	{
		return;
	}

	if(DelayTime>=0.0f)
	{
		DelayTime-=DeltaTime;
		if(DelayTime<0.0f)
		{
			g_PreMovetoPoint = FALSE;
			bEnd = TRUE;
#ifdef DEBUG_SYNC_INFO
			warnf(TEXT("Clear ability cache response-------------------------") );
#endif

			return;
		}
	}

	if(g_SendClearAbilityCacheTimes==g_RecieveClearAbilityCacheTimes)
	{
		if(DelayTime<0.0){
			DelayTime = 0.1f;
		}
		//bEnd = TRUE;
		//return;
	}


}

void UPlayerActionEx_WaitingForClearCache::OnAction(UBOOL bOn)
{
	if (!ActionOwner)
	{
		return;
	}

	if (bOn)
	{
		DelayTime = -1.0f;
		bStart = TRUE;
		bEnd = FALSE;
	}

}

UBOOL UPlayerActionEx_WaitingForClearCache::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_WaitingForClearCache::IsLatenAction()
{
	return TRUE;
}



//-------------------------------------------------------------------------

FString UPlayerActionEx_CannotInterrupt::Dump()
{
	return UPlayerActionEx_Base::Dump();
}

void UPlayerActionEx_CannotInterrupt::Update(FLOAT DeltaTime)
{
	CTRL_TICK("UPlayerActionEx_CannotInterrupt");

	if(InterruptLevelTime>0.0f)
	{
		InterruptLevelTime-=DeltaTime;
		if(InterruptLevelTime<=0.0f)
		{
			
		}
	}

	if (bStart  &&  bEnd==FALSE )
	{
		bEnd = TRUE;
		return;			
	}
}

void UPlayerActionEx_CannotInterrupt::ResetAction(UPlayerActionEx_Base* InAction)
{
	bStart = bEnd = TRUE;
}

void UPlayerActionEx_CannotInterrupt::OnAction(UBOOL bOn)
{
	bStart = bEnd = TRUE;
}

UBOOL UPlayerActionEx_CannotInterrupt::IsComplete()
{
	return bEnd;
}

UBOOL UPlayerActionEx_CannotInterrupt::IsLatenAction()
{
	return FALSE;
}
