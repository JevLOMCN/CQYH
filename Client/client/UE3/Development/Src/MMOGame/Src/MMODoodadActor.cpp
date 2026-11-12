#include "stdafx.h"
#include "MMOFacade.h"
#include "DoodadManager.h"
#include "MMOChatManager.h"
#include "LinkManager.h"

IMPLEMENT_CLASS(AMMODoodadActor)
IMPLEMENT_CLASS(USkeletalMeshComponentEx)

UBOOL AMMODoodadActor::CanInteract() const
{
	if(IsDead || (RemainOpenCount == 0))
	{
		return FALSE;
	}

	if(DoodadTemplate)
	{
		// quest-type DOODAD is only interactable when a quest is active
		if(DoodadTemplate->DoodadType == E_dt_Quest)
		{
			return IsQuestActive;
		}

		//loot corpse
		if(MMOFacade::getSingleton().GetMainController()->eventIsLeftAltDown())
		{
			if(DoodadTemplate->DoodadType == E_dt_Corpse)
			{
				return TRUE;
			}
		}

		if( DoodadTemplate->RequireItemID > 0)
		{
			return TRUE;
		}

		if(IsVarRequired())
		{
			return TRUE;
		}

		if((DoodadTemplate->DoodadType == E_dt_TreasureBox)
			|| (DoodadTemplate->DoodadType == E_dt_Resource))
		{
			return TRUE;
		}
	}
	

	return FALSE;
}

BYTE AMMODoodadActor::GetActorType() const
{
	return EACT_Doodad;
}

BYTE AMMODoodadActor::GetPickPriority()
{
	if(IsDead)
	{
		return EPP_MAX;
	}



	// quest-type DOODAD is only interactable when a quest is active
	if(DoodadTemplate->DoodadType == E_dt_Quest)
	{
		if(IsQuestActive)
		{
			return EPP_High;
		}

		return EPP_MAX;
	}

	if(CanInteract())
	{
		return EPP_High;
	}


	if(MMOFacade::getSingleton().IsAttackable(this))
	{
		return EPP_Normal;
	}

	return EPP_MAX;
}

BYTE AMMODoodadActor::GetMouseCursor()
{
	if(MMOFacade::getSingleton().IsAttackable(this))
	{
		if(!IsDead)
		{
			return EMC_Attack;
		}
	}

	if(CanInteract())
	{
		if (DoodadTemplate)
		{
			return DoodadTemplate->DoododCursorType;
		}
	}
   
	return EMC_Arrow;
}

void AMMODoodadActor::SetSilhouette( UBOOL show, FVector sc )
{
	if(Mesh)
	{
		Mesh->UpdateSilhouetteState( show, sc );
		//Mesh->ShowSilhouette = show;
		//Mesh->SilhouetteColor = sc;
	}
}


FVector AMMODoodadActor::GetInteractActorLocation()
{
	return Location;
}

FLOAT AMMODoodadActor::GetMaxInteractRadius()
{
	return 128.0f+32.0f;//MMOFacade::getSingleton().GetMainController()->MaxPickDistance;
}

void AMMODoodadActor::DoInteract()
{	
	if(CanInteract())
	{
		if( DoodadTemplate->RequireItemID > 0)
		{
			// prompt when conditions are not met
			if(MMOFacade::getSingleton().GetItemCountInBag(
				DoodadTemplate->RequireItemID) <= 0)
			{
				UItem* item = MMOFacade::getSingleton().FindItemTemplate(DoodadTemplate->RequireItemID);
				if(item)
				{
					{
						FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.RequireItemMsg"));										
						msg = FString::Printf(*msg, *item->GetDisplayName());
						MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("Doodad.RequireItemMsg"));
					}

					{
						FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.RequireItemMsgChat"));										
						msg = FString::Printf(*msg, *LinkManager::HtmlLink_Item(item));
						MMOChatManager::getSingleton().AddSystemMessage(msg);
					}
				}				

				return;
			}
		}

		if(IsVarRequired())
		{
			// prompt when conditions are not met
			if(!IsRequireVarOK())
			{
				{
					FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.RequireVarMsg"));										
					msg = FString::Printf(*msg, *DoodadTemplate->TipsText);
					MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("Doodad.RequireVarMsg"));
				}

				{
					FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("Doodad.RequireVarMsgChat"));										
					msg = FString::Printf(*msg, *DoodadTemplate->TipsText);
					MMOChatManager::getSingleton().AddSystemMessage(msg);
				}

				return;
			}
		}

		//PlayAnimOpen();
		CMessageHandler::GetInstance().Send_PickDoodadItem( ServerID );
	}
}

void AMMODoodadActor::Init()
{
	//warnf(NAME_Log, TEXT("____________DoodadActor::Init"));

	if(!DoodadTemplate)
	{
		warnf(NAME_Error, TEXT("[AMMODoodadActor::Init] DoodadTemplate data is NULL!"));
		return;
	}

	class UAvatarData* AvatarDataTemplate = DoodadTemplate->DoodadAvatar;

	if(!AvatarDataTemplate)
	{
		warnf(NAME_Error, TEXT("[AMMODoodadActor::Init] AvatarDataTemplate data is NULL!"));
		return;
	}

	Mesh->SetSkeletalMesh(AvatarDataTemplate->SkeletalMesh);
	Mesh->SetLightEnvironment( LightEnvironment );

	Mesh->AnimSets.Empty();
	if(AvatarDataTemplate->WeaponAnimSetPairs.Num() > 0)
	{
		if(AvatarDataTemplate->WeaponAnimSetPairs(0).AnimSets.Num() > 0)
		{
			Mesh->AnimSets.AddItem(AvatarDataTemplate->WeaponAnimSetPairs(0).AnimSets(0));
			Mesh->UpdateAnimations();
		}
	}

	Mesh->SetAnimTreeTemplate(AvatarDataTemplate->AnimationTree);
	AvatarDataTemplate->DynamicLoadPhysicAsset();
	Mesh->SetPhysicsAsset(AvatarDataTemplate->PhysicAsset);

	setPhysics(PHYS_None);

	SetDrawScale(AvatarDataTemplate->AvatarDrawScale * AvatarDataTemplate->MeshScale);

	FLOAT colRadius = AvatarDataTemplate->CollisionRadius * AvatarDataTemplate->AvatarDrawScale;
	FLOAT colHeight = AvatarDataTemplate->CollisionHeight * AvatarDataTemplate->AvatarDrawScale;
	
	SetCollisionSize(colRadius, colHeight);

	//CylinderComponent->CollisionRadius = colRadius;
	//CylinderComponent->CollisionHeight = colHeight;
	if(DoodadTemplate->EnableCollision)
	{
		SetCollisionType(COLLIDE_BlockAll);
		SetCollision(TRUE, TRUE, TRUE);
	}
	else
	{
		SetCollisionType(COLLIDE_NoCollision);
		SetCollision(FALSE, FALSE, FALSE);
	}

	UpdateLocationZ();

	PlayAnimIdle();

	if(DoodadTemplate->ActiveEffectTemplate)
	{
		AddParticleEffects(DoodadTemplate->ActiveEffectTemplate, DoodadTemplate->ActiveEffectScale);
	}

	// fix bug #24950
	UseingAvatar = AvatarDataTemplate;
	RefreshAvatar();

	RebuildMaterialControllerList();

	//warnf(NAME_Log, TEXT("____________DoodadActor::Init End"));
}
//-----------------------------------------------------------------------------------
void AMMODoodadActor::PlayAnimIdle()
{
	if(DoodadTemplate)
	{
		Mesh->PlayAnimEx(DoodadTemplate->AnimIdle, 0.0f, TRUE);
	}
}

void AMMODoodadActor::PlayAnimOpen()
{
	if(DoodadTemplate && DoodadTemplate->AnimOpen != NAME_None)
	{
        //// fill in the data needed for the animation first, start playing after the message arrives
        //DoodadManager::getSingleton().EnableRoleAnimByDoodad(this, DoodadTemplate, Location);

		Mesh->PlayAnimEx(DoodadTemplate->AnimOpen, 0.0f, FALSE);
	}
    //else
    //    DoodadManager::getSingleton().StopRoleAnimByDoodad(FALSE);
}

FLOAT AMMODoodadActor::PlayAnimation(FName Anim,INT InKey,FLOAT Rate,UBOOL bLoop,BYTE RootBoneAxisX,BYTE RootBoneAxisY,BYTE RootBoneAxisZ,BYTE BlendType,FLOAT Position,FLOAT InBlendIn,FLOAT InBlendOut,BYTE InLevel)
{
	Mesh->PlayAnimEx(Anim, 0.0f, bLoop);

	return TRUE;
}

void AMMODoodadActor::PlayAnimClose()
{
	if(DoodadTemplate)
	{
		Mesh->PlayAnimEx(DoodadTemplate->AnimClose, 0.0f, FALSE);
	}

    //// only one gathering point can be open at a time, close it directly
    //DoodadManager::getSingleton().StopRoleAnimByDoodad(FALSE);
}

void AMMODoodadActor::RemoveParticleEffects()
{
	if(ParticleEffect)
	{
		ParticleEffect->DeactivateSystem();
	}
}

void AMMODoodadActor::AddParticleEffects(class UParticleSystem* parTmp, FLOAT effScale)
{
	if(ParticleEffect)
	{
		ParticleEffect->SetTemplate(parTmp);
		if(ParticleEffect->Scale != effScale)
		{
			ParticleEffect->Scale = effScale;
			ParticleEffect->BeginDeferredUpdateTransform();
		}

		ParticleEffect->ActivateSystem();
	}
}
    
void AMMODoodadActor::SetRemainOpenCount(INT newCnt, AMMOActor* instigateActor)
{
	if(newCnt == RemainOpenCount)
	{
		return;
	}

	// if it decreased, assume damage was taken
	if(RemainOpenCount > newCnt)
	{
		if(DoodadTemplate && (DoodadTemplate->DoodadType == E_dt_Destrucible))
		{
			PlayActions(DoodadTemplate->HitEffectList);

			if(DoodadTemplate->DoodadAvatar)
			{
				if(newCnt == 0)
				{
					PlayDestroyAnim(instigateActor);
					UGamePlayDataManager::GetSingleton()->PlayDoodadDestructSound(
						this, DoodadTemplate->DoodadAvatar->TargetArmorMatType);
				}
				else
				{
					UGamePlayDataManager::GetSingleton()->PlayDoodadHitSound(
						this, DoodadTemplate->DoodadAvatar->TargetArmorMatType);
				}
			}
		}
	}

	RemainOpenCount = newCnt;
}

INT AMMODoodadActor::GetRemainOpenCount() const
{
	return RemainOpenCount;
}

void AMMODoodadActor::PlayDestroyAnim(AMMOActor* instigateActor)
{
	if(IsDead)
	{
		return;
	}

	IsDead = TRUE;

	if (MMOFacade::getSingleton().GetMainController()->GetSelectTarget() == this)
	{
		MMOFacade::getSingleton().GetMainController()->SetSelectTarget(NULL);
	}
	
	
	RemoveParticleEffects();

	if(Mesh)
	{
		//UAvatarData::ToogleBoneBreak(Mesh, TRUE);

		if(DoodadTemplate)
		{
			PlayActions(DoodadTemplate->DeathEffectList);
			if((DoodadTemplate->DoodadType == E_dt_Destrucible) && Mesh->PhysicsAsset)
			{
				Mesh->SetRBChannel(RBCC_DeadPawn);
				Mesh->SetRBCollidesWithChannel(RBCC_Default,TRUE);
				Mesh->SetRBCollidesWithChannel(RBCC_Pawn,FALSE);
				Mesh->SetRBCollidesWithChannel(RBCC_Vehicle,FALSE);
				Mesh->SetRBCollidesWithChannel(RBCC_Untitled1,FALSE);
				Mesh->SetRBCollidesWithChannel(RBCC_Untitled2,FALSE);
				Mesh->SetRBCollidesWithChannel(RBCC_Untitled3,FALSE);
				Mesh->SetRBCollidesWithChannel(RBCC_Untitled4,FALSE);
				Mesh->SetRBCollidesWithChannel(RBCC_BlockingVolume,TRUE);
				Mesh->SetRBCollidesWithChannel(RBCC_DeadPawn,TRUE);
				Mesh->SetTickGroup(TG_PostAsyncWork);
				bCollideWorld = TRUE;

				//if (Physics == PHYS_RigidBody)
				//{
				//	setPhysics(PHYS_Falling);
				//}

				CollisionComponent = Mesh;
				Mesh->PhysicsWeight = 1.0f;
				Mesh->SetHasPhysicsAssetInstance(TRUE);


				//setPhysics(PHYS_RigidBody);

				//if(Mesh->PhysicsAssetInstance)
				//{
				//	Mesh->PhysicsAssetInstance->SetAllBodiesFixed(FALSE);
				//}

				Mesh->SetBlockRigidBody(TRUE);
				Mesh->InitComponentRBPhys(FALSE);
				Mesh->WakeRigidBody();

				static FLOAT DestructFoceRadius = 0.0f, DestructFoceStrength = 0.0f;
				if(DestructFoceRadius < 1.0f)
				{
					GConfig->GetFloat(TEXT("MMODoodadActor"), TEXT("DestructFoceRadius"), DestructFoceRadius, GGameIni);
					GConfig->GetFloat(TEXT("MMODoodadActor"), TEXT("DestructFoceStrength"), DestructFoceStrength, GGameIni);
				}

				FLOAT destDestructFoceStrength = DestructFoceStrength * DoodadTemplate->DestructForceScale;
				
				if(instigateActor)
				{
					Mesh->AddRadialImpulse(instigateActor->Location, DestructFoceRadius , destDestructFoceStrength, RIF_Linear, TRUE);
				}
				else 
				{
					Mesh->AddRadialImpulse(Location, DestructFoceRadius, destDestructFoceStrength, RIF_Linear);
				}
				Mesh->PlayAnimEx(DoodadTemplate->AnimDie, 0.0f, FALSE);
				
				DelayDestroyTime = DoodadTemplate->DurationBeforeDestroy;
			}
		}
	}
}

void AMMODoodadActor::DestroyDoodadActor(INT disappearType)
{
	if(HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		return;
	}

	// mark here to avoid interaction
	IsDead = TRUE;

	if(DoodadTemplate && DoodadTemplate->OperateDuration > 0.0f)
	{
		if (disappearType == sodtLeaveScene)
		{
			PlayAnimOpen();
		}
		debugf(TEXT("DestroyDoodadActor[%d] OpDur:%d"), this->ServerID, DoodadTemplate->OperateDuration);
		LifeSpan = DoodadTemplate->OperateDuration / 1000.0f;
		//SetTimer(DoodadTemplate->OperateDuration / 1000.0f, FALSE, TEXT("FinalDestroy"), this);
	}
	else
	{
		SetRemainOpenCount(0, NULL); // on death, try to play the death animation

		if(DelayDestroyTime > 0.0f)
		{
			LifeSpan = DelayDestroyTime;
			//SetTimer(DelayDestroyTime, FALSE, TEXT("FinalDestroy"), this);
		}
		else
		{
			FinalDestroy();
		}
	}
}

void AMMODoodadActor::TickSpecial(FLOAT Delta)
{
	 AMMOActor::TickSpecial(Delta);

	 CurrentTime += Delta;
	if (OperatorStateIndex == 0)
	{
		if (CurrentTime >= FirstStateLastTime)
		{
			OperatorStateIndex = 1;
			CurrentTime = 0.f;
			PlayEffect(OperatorStateIndex);
		}
	}
}

void AMMODoodadActor::BeginOperate(INT InOperatorId, FLOAT InDuration)
{
	OperatorId = InOperatorId;
	AMMOActor* OperatorActor = MMOFacade::getSingleton().GetCharacter(OperatorId);
	if (OperatorActor && OperatorActor->Mesh)
	{
		UAnimSequence* uas = OperatorActor->Mesh->FindAnimSequence(DoodadTemplate->BeginAnimOpener);
		OperatorStateIndex = 0;
		CurrentTime = 0.f;
		if (uas && uas->SequenceLength > 0.01f)
		{
			FirstStateLastTime = uas->SequenceLength;
		}
		else
		{
			FirstStateLastTime = 0.f;
		}

		if (FirstStateLastTime > InDuration)
		{
			FirstStateLastTime = InDuration;
		}

		SecondStateLastTime = InDuration - FirstStateLastTime;
		PlayEffect(OperatorStateIndex);
	}
}

void AMMODoodadActor::EndOperate(UBOOL bBreak)
{
	OperatorStateIndex = 2;
	if (!bBreak)
	{
		PlayEffect(2);
	}
	else
	{
		AMMOActor* OperatorActor = MMOFacade::getSingleton().GetCharacter(OperatorId);
		if (OperatorActor)
		{
			OperatorActor->SetWeaponVisibleByAbility(TRUE);
		}
	}
}

void AMMODoodadActor::PlayEffect(INT InIndex)
{
	AMMOActor* OperatorActor = MMOFacade::getSingleton().GetCharacter(OperatorId);
	if (OperatorActor)
	{
		if (InIndex == 0)
		{
			if (OperatorActor && OperatorActor->Mesh)
			{
//FLOAT PlayAnimation(FName Anim,INT InKey=0,FLOAT Rate=1.000000,UBOOL bLoop=FALSE,BYTE RootBoneAxisX=1,BYTE RootBoneAxisY=1,BYTE RootBoneAxisZ=1,BYTE BlendType=0,FLOAT Position=0.000000,FLOAT InBlendIn=0.100000,FLOAT InBlendOut=0.100000,BYTE InLevel=1)
				OperatorActor->PlayAnimation(DoodadTemplate->BeginAnimOpener);
				if (DoodadTemplate && DoodadTemplate->DoodadAvatar)
				{
					UGamePlayDataManager::GetSingleton()->PlayDoodadBeginSound(this, DoodadTemplate->DoodadAvatar->TargetArmorMatType);
				}

				OperatorActor->SetWeaponVisibleByAbility(FALSE);
			}
		}
		else if (InIndex == 1)
		{
			if (OperatorActor && OperatorActor->Mesh)
			{
				OperatorActor->PlayAnimation(DoodadTemplate->LoopAnimOpener, 0, 1.0, TRUE);
			}

			//if (DoodadTemplate->)
			//{
			//}

			//UGamePlayDataManager::GetSingleton()->PlayDoodadLoopSound(
			//	_doodadActorWorking,
			//	_doodadWroking->DoodadAvatar->TargetArmorMatType);

		}
		else if (InIndex == 2)
		{
			if (OperatorActor && OperatorActor->Mesh)
			{
				OperatorActor->PlayAnimation(DoodadTemplate->EndAnimOpener);
				if (DoodadTemplate && DoodadTemplate->DoodadAvatar)
				{
					UGamePlayDataManager::GetSingleton()->PlayDoodadEndSound(this, DoodadTemplate->DoodadAvatar->TargetArmorMatType);
				}

				OperatorActor->SetWeaponVisibleByAbility(TRUE);
			}

			if (DoodadTemplate && DoodadTemplate->DoodadEndEffect)
			{
				AActor* actor = GWorld->SpawnActor(AMMOEmitter::StaticClass(), NAME_None, Location);
				AMMOEmitter* emit = Cast<AMMOEmitter>(actor);
				if (emit)
					emit->eventSetTemplate(DoodadTemplate->DoodadEndEffect, TRUE);
			}
		}
	}

}

void AMMODoodadActor::FinalDestroy()
{
	SET_WARN_COLOR(COLOR_CYAN);
	debugf(TEXT("@@@@@@@@@@@ AMMODoodadActor::FinalDestroy %s"), *GetDebugDescription());
	//LifeSpan = 0.001f;
	if(!HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		GWorld->DestroyActor( this );
	}
	CLEAR_WARN_COLOR();
}

bool AMMODoodadActor::IsRequireVarOK() const
{
	if(IsVarRequired())
	{
		// TODO: fix after config reading is unified
		//return MMO::IsRequiredVarOK(DoodadTemplate->RequireVarID, DoodadTemplate->RequireVarValue);
		return TRUE;
	}

	return false;
}

bool AMMODoodadActor::IsVarRequired() const
{
	if(DoodadTemplate && (!DoodadTemplate->RequireVarID.IsEmpty()))
	{
		return true;
	}

	return false;
}

FString AMMODoodadActor::GetDebugDescription()
{
	return FString::Printf(TEXT("MMODoodadActor this[%x] ID[%d] TID[%d] [%s] %s"),
#if defined(_WIN64)
		(__int64)
#elif defined(_WIN32)
		(INT)
#endif
		this, ServerID,
		(DoodadTemplate ? DoodadTemplate->GetTemplateID() : -1), 
		(DoodadTemplate ? *DoodadTemplate->DoodadName.ToString() : TEXT("N/A")), *Location.ToString());
}

INT32 AMMODoodadActor::CalGridBlockRadius() 	// zhaojun@snda
{
	if (DoodadTemplate)
	{
		const FString& bt = DoodadTemplate->BodyType;

		if (bt.Len() > 0)
		{
			check(*((*bt) + 4) >= TEXT('0') && *((*bt) + 4) <= TEXT('9'));

			CurBlockType = *((*bt) + 4) - TEXT('0');

			return (CurBlockType == 0 ? (-1) : (CurBlockType / 2));
		}
	}
	
	return -1;
}
