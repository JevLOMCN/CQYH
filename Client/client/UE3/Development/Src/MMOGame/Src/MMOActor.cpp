#include "..\stdafx.h"
#include "..\MMOFacade.h"
#include "net/message_handler.h"
#include "attribute_def.h"
#include "EngineAudioDeviceClasses.h"
#include "EnginePrivate.h"
#include "MMOGameUIClasses.h"
#include "MMOGameClasses.h"
#include "AbilityProcessor.h"
#include "MMOBuffManager.h"
#include "MMONavigationManager.h"
#include "EquipSlotManager.h"
#include "MMOSkillManager.h"
#include "DoodadManager.h"
#include "MMOQuestManager.h"
#include "MMOStats.h"
#include "FSMIO.h"
#include "CommonGameEvents.h"
#include "MMOClientVariableManager.h"
#include "MMOPerformance.h"

IMPLEMENT_CLASS(UIMMOActor)
IMPLEMENT_CLASS(UIPickableActor)
IMPLEMENT_CLASS(UIPickablePoint)

extern INT g_moveModGridSize;
extern INT g_TotalFrameNum;
extern FSystemSettings GSystemSettings; // zhangrui@snda


FString AMMOActor::GetActorName()
{
	return ActorName;
}

BYTE AMMOActor::GetActorType() const
{
	return ActorType;
}

void AMMOActor::SetActorType( BYTE newType )
{
	if( ActorType != newType ){
		MMOGameEvent_OneInt actTypeChangedEvt( GameEvent_ActorTypeChanged, ServerID );
		MMOUI::PostGameEvent( &actTypeChangedEvt );
	}
	ActorType = newType;
}

UBOOL AMMOActor::IsMainPlayer() const
{
	return ActorType == EACT_PlayerSelf;
}

FLOAT AMMOActor::GetHealthPercent()
{
	if(!ActorAttributes)
		return 0.001f;

	if( ActorAttributes->GetMaxHp() == 0 )
		return 0.001f;//如果对flash的宽度设0有点问题

	return float(ActorAttributes->GetCurHp()) / float(ActorAttributes->GetMaxHp());
}

UAvatarData* AMMOActor::GetAvatarData()
{
	return UseingAvatar;
}

void AMMOActor::SetSilhouetteEx( UBOOL show, FVector sc )
{
	if(bDead)
	{
		show = FALSE;
	}

	if(Mesh != NULL)
	{
		Mesh->UpdateSilhouetteState( show, sc );
		//Mesh->ShowSilhouette = show;
		//Mesh->SilhouetteColor = sc;
	}

	if (OverriddenMesh)
	{
		OverriddenMesh->UpdateSilhouetteState( show, sc );
		//OverriddenMesh->ShowSilhouette = show;
		//OverriddenMesh->SilhouetteColor = sc;
	}


	if( ItemManager != NULL )
		ItemManager->SetSilhouette( show, sc );

	for (INT i = 0; i < this->EquipList.Num(); ++i)
	{
		UObject* obj = EquipList(i).ComponentOrActor;
		if (!obj)
		{
			continue;
		}

		if (obj->IsA(UMeshComponent::StaticClass()))
		{
			UMeshComponent* umc = (UMeshComponent*)(obj);
			umc->UpdateSilhouetteState( show, sc );
			//umc->ShowSilhouette = show;
			//umc->SilhouetteColor = sc;
		}
		else if (obj->IsA(AArmorActor::StaticClass()))
		{
			AArmorActor* aaa = (AArmorActor*)(obj);
			if (aaa->MainMesh)
			{
				aaa->MainMesh->UpdateSilhouetteState( show, sc );
				//aaa->MainMesh->ShowSilhouette = show;
				//aaa->MainMesh->SilhouetteColor = sc;
			}
		}
	}
}

void AMMOActor::NativePostBeginPlay()
{
	SetServerLocation(Location);
	ResetLastServerLocation();
	AttackAi_LockTime=-1.0f;
	//SetAutoAttackLockTime();
}

void AMMOActor::PlayRandomSound(const TArray<class USoundCue*>& soundList)
{	
	if(soundList.Num() > 0 )
	{
		INT idx = (INT)RandRange(0, soundList.Num() - 1);

		APawn::PlaySound(soundList(idx));
	}
}

void AMMOActor::PlayEventSound(USoundCue* sound)
{
	MMOFacade::getSingleton().StopAllVoiceSound(FALSE, 0.5, 0.0);

	UAudioComponent* audioComponent = UAudioDevice::CreateComponent(sound, GWorld->Scene, this, 0, TRUE);
	if (audioComponent)
	{
		audioComponent->Location = this->Location;
		audioComponent->bUseOwnerLocation = TRUE;
		audioComponent->bStopWhenOwnerDestroyed = TRUE;
		audioComponent->bAllowSpatialization = TRUE;
		audioComponent->bIsUISound	= TRUE;
		audioComponent->bAutoDestroy = 1;
		audioComponent->SubtitlePriority = 10000;
		audioComponent->Play();
	}
}

INT AMMOActor::SpawnMirror(class UAbilityMirrorComponent* InData, FLOAT InLiftSpan)
{
	if (!InData)
	{
		return 0;
	}

	if (!Mesh->SkeletalMesh)
	{
		return 0;
	}

	if (InLiftSpan > 5.f)
	{
		InLiftSpan = 5.f;
	}

	AAbilitySkeletalMeshActor* NewActor = Cast<AAbilitySkeletalMeshActor>(GWorld->SpawnActor(AAbilitySkeletalMeshActor::StaticClass(), NAME_None, Location, Rotation));
	if (!NewActor)
	{
		return 0;
	}



	NewActor->SkeletalMeshComponent->SetSkeletalMesh(GetActiveBaseMesh()->SkeletalMesh);
	NewActor->SkeletalMeshComponent->SetParentAnimComponent(GetActiveBaseMesh());
	NewActor->SkeletalMeshComponent->ForceSkelUpdate();

	NewActor->SkeletalMeshComponent->SetLightingChannels(GetActiveBaseMesh()->LightingChannels);
	NewActor->SkeletalMeshComponent->SetLightEnvironment(GetActiveBaseMesh()->LightEnvironment);

	UMaterialInstanceTimeVarying* MaterialToUseTemplate = InData->DefaultMaterial;
	if (InData->CustomMaterial)
	{
		MaterialToUseTemplate = InData->CustomMaterial;
	}

	UMaterialInstanceTimeVarying* MaterialToUse = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass(), NewActor);
	if (!MaterialToUse)
	{
		if(!NewActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			GWorld->DestroyActor(NewActor);
		}
		return 0;
	}

	MaterialToUse->SetParent(MaterialToUseTemplate);
	//MaterialToUse->SetScalarParameterValue(FName(TEXT("AlphaBlend")), InLiftSpan);
	MaterialToUse->ResetScalarStartTime(FName(TEXT("AlphaBlend")), 0.f);
	MaterialToUse->SetLinearColorParameterValue(FName(TEXT("Color")), InData->ColorValue);

	for (INT i = 0; i < Mesh->Attachments.Num(); ++i)
	{
		if (Mesh->Attachments(i).Component)
		{
			if (Mesh->Attachments(i).Component->IsA(USkeletalMeshComponent::StaticClass()))
			{
				//todo
			}
			else if (Mesh->Attachments(i).Component->IsA(UStaticMeshComponent::StaticClass()))
			{
				UStaticMeshComponent* TempateMesh = (UStaticMeshComponent*)(Mesh->Attachments(i).Component);
				UStaticMeshComponent* NewComponent =ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), NewActor);
				if (!NewComponent)
				{
					continue;
				}

				NewComponent->SetStaticMesh(TempateMesh->StaticMesh);

				NewComponent->SetLightEnvironment(NewActor->SkeletalMeshComponent->LightEnvironment);
				NewComponent->SetLightingChannels(NewActor->SkeletalMeshComponent->LightingChannels);


				INT MatNum = NewComponent->GetNumElements();
				for (INT j = 0; j < MatNum; ++j)
				{
					NewComponent->SetMaterial(j, MaterialToUse);
				}

				NewActor->SkeletalMeshComponent->AttachComponentToSocket(NewComponent, Mesh->Attachments(i).BoneName);
			}
		}
	}

	for (INT i = 0; i < Components.Num(); ++i)
	{
		if (Components(i) == NULL)
		{
			continue;
		}

		if (Components(i) == Mesh)
		{
			continue;
		}

		USkeletalMeshComponent* TemplateMesh = Cast<USkeletalMeshComponent>(Components(i));
		if (TemplateMesh == NULL)
		{
			continue;
		}

		USkeletalMeshComponent* NewComponent = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass(), NewActor);
		if (NewComponent == NULL)
		{
			continue;
		}

		NewComponent->SetSkeletalMesh(TemplateMesh->SkeletalMesh);
		NewComponent->SetLightEnvironment(TemplateMesh->LightEnvironment);
		NewComponent->SetLightingChannels(TemplateMesh->LightingChannels);


		INT MatNum = NewComponent->GetNumElements();
		for (INT j = 0; j < MatNum; ++j)
		{
			NewComponent->SetMaterial(j, MaterialToUse);
		}
		NewComponent->SetParentAnimComponent(GetActiveBaseMesh());
		NewActor->AttachComponent(NewComponent);
	}

	if (IsA(AMMOPlayer::StaticClass()))
	{
		NewActor->SkeletalMeshComponent->SetHiddenGame(TRUE);
	}

	NewActor->SetTickIsDisabled(TRUE);

	INT idx = -1;
	for (INT i = 0; i < ActiveMirrorActors.Num(); ++i)
	{
		if (ActiveMirrorActors(i).MeshActor == NULL)
		{
			idx = i;
		}
	}

	if (idx == -1)
	{
		idx = ActiveMirrorActors.AddZeroed();
	}

	ActiveMirrorActors(idx).LifeSpan = InLiftSpan;
	ActiveMirrorActors(idx).MeshActor = NewActor;
	return NewActor->GetIndex();
}

void AMMOActor::ClearMirror(INT InObjectId)
{
	for (INT i = 0; i < ActiveMirrorActors.Num(); ++i)
	{
		AActor* actor = ActiveMirrorActors(i).MeshActor;
		if (actor && actor->GetIndex() == InObjectId)
		{
			if(!actor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
			{
				GWorld->DestroyActor(actor);
			}
			ActiveMirrorActors(i).MeshActor = NULL;
			break;
		}
	}
}

void AMMOActor::UpdateMirrorActors(FLOAT Delta)
{
	for (INT i = 0; i < ActiveMirrorActors.Num(); ++i)
	{
		if (ActiveMirrorActors(i).MeshActor)
		{
			ActiveMirrorActors(i).LifeSpan -= Delta;
			if (ActiveMirrorActors(i).LifeSpan <= 0.f)
			{
				if(!ActiveMirrorActors(i).MeshActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
				{
					GWorld->DestroyActor(ActiveMirrorActors(i).MeshActor);
				}
				ActiveMirrorActors(i).MeshActor = NULL;
			}
		}
	}
}

void AMMOActor::ClearAllMirrors()
{
	for (INT i = 0; i < ActiveMirrorActors.Num(); ++i)
	{
		AActor* actor = ActiveMirrorActors(i).MeshActor;
		if (actor && !actor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			GWorld->DestroyActor(actor);
			ActiveMirrorActors(i).MeshActor = NULL;
		}
	}
}

UBOOL AMMOActor::GetCurrentAnimName(FName& OutAnim)
{
	if (ActorAT && ActorAT->ActionLoopBlend)
	{
		if (ActorAT->ActionLoopBlend->Children(1).Weight >= 1)
		{
			if (ActorAT->ActionBlend)
			{
				INT OutActiveAnimKey;
				BYTE OutAnimLevel;
				FLOAT OutPercent;
				FName OutAnimName;
				UBOOL bRet = ActorAT->ActionBlend->GetCurrentAnimInfo(OutActiveAnimKey, OutAnimLevel, OutPercent, OutAnimName);
				if (bRet)
				{
					OutAnim = OutAnimName;
					return TRUE;
				}
			}
		}
		else
		{
			if (ActorAT->IdleLoopBlend)
			{
				INT OutActiveAnimKey;
				BYTE OutAnimLevel;
				FLOAT OutPercent;
				FName OutAnimName;
				UBOOL bRet = ActorAT->IdleLoopBlend->GetCurrentAnimInfo(OutActiveAnimKey, OutAnimLevel, OutPercent, OutAnimName);
				if (bRet)
				{
					OutAnim = OutAnimName;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


FLOAT AMMOActor::GetMoveAnimPosition()
{
	if(ActorAT)
	{
		return ActorAT->GetMoveAnimPosition();
	}
	return 0.0f;
}

void AMMOActor::SetMoveAnimPosition(FLOAT InPos)
{
	if (MeshChangeAnimController)
	{
		MeshChangeAnimController->SetMoveAnimPosition(InPos);
	}

	if(ActorAT)
	{
		ActorAT->SetMoveAnimPosition(InPos);
	}


	// weapon animation
	UMMOActorAT* WeaponAT = GetWeaponEquipMeshAnimationController(0);
	if (WeaponAT)
	{
		WeaponAT->SetMoveAnimPosition(InPos);
	}
	WeaponAT = GetWeaponEquipMeshAnimationController(1);
	if (WeaponAT)
	{
		WeaponAT->SetMoveAnimPosition(InPos);
	}

	if(MountInfo && MountInfo->ActorAT )
	{
		MountInfo->ActorAT->SetMoveAnimPosition(InPos);
	}

	for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
	{
		if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT )
		{
			AttachedDestructableParts(i)->ActorAT->SetMoveAnimPosition(InPos);
		}
	}

}

//------------------------------------------------------------------------
void AMMOActor::SelfDestroyed(INT cid)
{
	// if(MMOFacade::getSingletonPtr())
	{
		MMOFacade::getSingleton().DestroyObject(cid, FALSE, sodtLeaveScene);
	}

// 	if (MainController && MainController->MyDecalManager && MainController->MyDecalManager->SelectedActor == this)
// 	{
// 		MainController->MyDecalManager->eventSelectActor(NULL);
// 	}

	ClearEquipEffects();

	ClearAllMirrors();

	DetachAllEquip();
}
//------------------------------------------------------------------------
void AMMOActor::TickAllAbility(FLOAT DeltaTime)
{
	for (INT i = 0; i < Abilities.Num(); ++i)
	{
		Abilities(i)->Tick(DeltaTime);
	}

	if(CurrentAbilityState != EAS_End && CurrentAbilityState != EAS_Requested && Abilities.Num()==0)
	{
		AbilityStateCD = 0.0f;
		CurrentAbilityState = EAS_End;
	}
}
//------------------------------------------------------------------------
UBOOL AMMOActor::IsUsingAbility()
{
	if (CurrentAbilityState == EAS_End)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
//------------------------------------------------------------------------
BYTE AMMOActor::GetPickPriority()
{
	if(bDead)
	{
		return EPP_MAX;
	}
	return PickPriority;
}

// oscar@snda
BYTE AMMOActor::GetMouseCursor()
{
	return EMC_Arrow;
}
// oscar@snda
//------------------------------------------------------------------------
int AMMOActor::GetSex()
{
	if( ActorAttributes )
		return ActorAttributes->GenderType;
	return 0;
}
int AMMOActor::GetHeroSex()
{
	if( HeroAttributes && ServerHeroID>0 )
		return HeroAttributes->GenderType;
	return 0;
}
//------------------------------------------------------------------------
int AMMOActor::GetClassId()
{
	if( ActorAttributes )
		return ActorAttributes->ClassType;
	return 0;
}
int AMMOActor::GetHeroClassId()
{
	if( HeroAttributes && ServerHeroID>0 )
		return HeroAttributes->ClassType;
	return 0;
}


void AMMOActor::SetObstracel(FName fName , UBOOL bEnable)
{
	FNavMeshWorld::ObstacleEnableByName(fName,bEnable);
}
//------------------------------------------------------------------------
FVector AMMOActor::GetFootLocation()
{
	return Location + FVector(0, 0, - GetCollisionHeight());
}
//------------------------------------------------------------------------
FLOAT AMMOActor::GetFullBodyHeight()
{
	return GetCollisionHeight() * 2.0f;
}

void AMMOActor::ResetPlaceActorToGroundTimer()
{
	AMMOActor* defaultActor = Cast<AMMOActor>(GetClass()->GetDefaultObject());
	if(defaultActor)
	{
		PlaceToGoundTickCount = defaultActor->PlaceToGoundTickCount;
	}
}

void AMMOActor::DisablePlaceActorToGroundTimer()
{
	PlaceToGoundTickCount = 0;
}

void AMMOActor::TryPlaceActorToGround()
{
	if (MMOFacade::getSingleton().bLevelsLoaded == FALSE)
	{
		return;
	}

	FVector HitLocation = Location;
	AActor* HitActor = NULL;

	FVector Start = Location + FVector(0,0, 780);
	FVector End = Location + FVector(0, 0, -1500);


	FMemMark Mark(GMainThreadMemStack);
	FCheckResult* HitRet = GWorld->MultiLineCheck
		(
		GMainThreadMemStack,
		End,
		Start,
		FVector(0, 0, 0),
		TRACE_Blocking | TRACE_LevelGeometry | TRACE_Level | TRACE_Terrain,
		this,
		NULL
		);

	while(HitRet)
	{
		if (HitRet->Actor)
		{
			AStaticMeshActor* sma = Cast<AStaticMeshActor>(HitRet->Actor);
			if (sma && (sma->bCanHidden || sma->bBlockActors == FALSE || sma->CollisionType != COLLIDE_BlockAll))
			{
				HitRet = HitRet->GetNext();
			}
			else
			{
				HitActor = HitRet->Actor;
				HitLocation = HitRet->Location;
				break;
			}
		}
		else
		{
			break;
		}
	}

	Mark.Pop();

	if (HitActor)
	{
		HitLocation.Z = HitLocation.Z + GetCollisionHeight();
	}
	else
	{
		// 找不到地面的时候先格子数据用下
		INT64 llSrcX = 0;
		INT64 llSrcY = 0;
		INT64 llSrcZ = 0;
		ZONE_DATA* pDestZone = NULL;
		GRID_DATA* pDestGrid = NULL;

		gServerGridWorld.ConvertPosClient2Server(Location, llSrcX, llSrcY, llSrcZ);
		gServerGridWorld.GetZoneAndGrid(llSrcX, llSrcY, llSrcZ, pDestZone, pDestGrid);
		if(pDestGrid)
		{
			HitLocation.Z = pDestGrid->uHighLayer * GRID_LAYER_HEIGHT/POINT_PER_CM + GetCollisionHeight();
		}
	}

	SetLocation(HitLocation);
}

//------------------------------------------------------------------------
void AMMOActor::TickSpecial( FLOAT DeltaSeconds )
{
	if(ResetMovePhaseTime>0.0f)
	{
		ResetMovePhaseTime-=DeltaSeconds;
	}

	//if(fActorDataResetDelay>0.0f)
	//{
	//	fActorDataResetDelay-=DeltaSeconds;
	//	if(fActorDataResetDelay<=0.0f)
	//	{
	//		RefreshVisible();
	//	}
	//}

	//移动位置残影时间刷新
	if(ResetLastLocationTime>0.0f)
	{
		ResetLastLocationTime-=DeltaSeconds;
		if(ResetLastLocationTime<=0.0f)
		{
			ResetLastServerLocation();
		}
	}

	if(AttackAi_LockTime>0.0f)
	{
		AttackAi_LockTime-=DeltaSeconds;
	}

	Super::TickSpecial(DeltaSeconds);

	// 过场动画中不要强制拉
	if(PlaceToGoundTickCount > 0 && (Physics != PHYS_Interpolating))
	{
		PlaceToGoundTickTime += DeltaSeconds;
		if(PlaceToGoundTickTime > 2.0f)
		{
			PlaceToGoundTickTime -= 2.0f;
			--PlaceToGoundTickCount;
			TryPlaceActorToGround();
		}
	}

	UpdateMirrorActors(DeltaSeconds);

	LastLocation = Location;
	if (PassiveMoveType == PassiveMoveType_DirectlyMove)
	{
		FLOAT Larm = PassiveMoveCurrentTime / PassiveMoveLastTime;
		FVector NewLocation = DashStartPoint * (1 - Larm) + PassiveMoveDestination * Larm;
		FCheckResult Hit(1.0f);
		FVector Start = NewLocation + FVector(0,0, 780);
		FVector End = NewLocation + FVector(0, 0, -1000);

		AActor* HitActor = NULL;
		FVector HitLocation = Location;

		FMemMark Mark(GMainThreadMemStack);
		FCheckResult* HitRet = GWorld->MultiLineCheck
			(
			GMainThreadMemStack,
			End,
			Start,
			FVector(0, 0, 0),
			TRACE_Blocking | TRACE_LevelGeometry | TRACE_Level | TRACE_Terrain,
			this,
			NULL
			);

		while(HitRet)
		{
			if (HitRet->Actor)
			{
				AStaticMeshActor* sma = Cast<AStaticMeshActor>(HitRet->Actor);
				if (sma && (sma->bCanHidden || sma->bBlockActors == FALSE || sma->CollisionType != COLLIDE_BlockAll))
				{
					HitRet = HitRet->GetNext();
				}
				else
				{
					HitActor = HitRet->Actor;
					HitLocation = HitRet->Location;
					break;
				}
			}
			else
			{
				break;
			}
		}

		Mark.Pop();

		/*
		GWorld->SingleLineCheck( Hit, this, End, Start, TRACE_Blocking | TRACE_LevelGeometry | TRACE_Level, FVector(0,0,0));

		*/
		if (HitActor)
		{
			NewLocation.Z = HitLocation.Z + GetCollisionHeight();
		}
		//debugf(TEXT("PM NewLocation=%f,%f,%f HitActor=%s"), NewLocation.X, NewLocation.Y, NewLocation.Z, *(HitActor->GetName()));
		SetLocation(NewLocation);
		UpdatePlaneShadowPosition();

		PassiveMoveCurrentTime += DeltaSeconds;
		if (PassiveMoveCurrentTime >= PassiveMoveLastTime)
		{
			PassiveMoveType = PassiveMoveType_None;
			FRotator NewRot = Rotation;
			NewRot.Pitch = NewRot.Roll = 0;
			SetRotation(NewRot);
			SetLocation(PassiveMoveDestination);
		}
	}
	else if (PassiveMoveType == PassiveMoveType_Throw)
	{
		if (PassiveMoveStep == 0)
		{
			// do nothing
		}
		else //if (PassiveMoveStep >= 1)
		{
			FLOAT ct = PassiveMoveCurrentTime / PassiveMoveLastTime;
			ct = powf(ct, ThrowExp);
			FVector NewPoint = CubicInterp<FVector, FLOAT>(P0, T0, P1, T1, ct);
			Mesh->PhysicsWeight = 1 - ct;
			NewPoint = ThrowStartMatrix.TransformFVector(NewPoint);
			SetLocation(NewPoint);
		}

		PassiveMoveCurrentTime += DeltaSeconds;
		if (PassiveMoveCurrentTime >= PassiveMoveLastTime)
		{
			PassiveMoveType = PassiveMoveType_None;
			SetLocation(PassiveMoveDestination);
			Mesh->PhysicsWeight = 0;
			FRotator NewRot = Rotation;
			NewRot.Pitch = NewRot.Roll = 0;
			SetRotation(NewRot);
		}
	}
	else if (PassiveMoveType == PassiveMoveType_Fear)
	{
		PassiveMoveCurrentTime += DeltaSeconds;
		if (PassiveMoveCurrentTime >= PassiveMoveLastTime)
		{
			PassiveMoveType = PassiveMoveType_None;
		}
	}

	// update cd
	for (INT i = 0; i < Abilities.Num(); ++i)
	{
		if (Abilities(i)->bCoolDown)
		{
			UBOOL bGroupCoolDown = TRUE, bSpellCoolDown = TRUE;
			
			if (Abilities(i)->CoolDownInfo.SpellTimeLeft > 0)
			{
				Abilities(i)->CoolDownInfo.SpellTimeLeft -= DeltaSeconds;
				Abilities(i)->CoolDownPercent = Abilities(i)->CoolDownInfo.SpellTimeLeft / Abilities(i)->CoolDownInfo.SpellTimeMax;
				if (Abilities(i)->CoolDownPercent < 0.f)
				{
					Abilities(i)->CoolDownPercent = 0.f;
				}
			}
			else
			{
				bSpellCoolDown = FALSE;
			}

			if (Abilities(i)->CoolDownInfo.GroupTimeLeft > 0)
			{
				Abilities(i)->CoolDownInfo.GroupTimeLeft -= DeltaSeconds;
				Abilities(i)->CoolDownPercent = Abilities(i)->CoolDownInfo.GroupTimeLeft / Abilities(i)->CoolDownInfo.GroupTimeMax;
				if (Abilities(i)->CoolDownPercent < 0.f)
				{
					Abilities(i)->CoolDownPercent = 0.f;
				}
			}
			else
			{
				bGroupCoolDown = FALSE;
			}

			if (!bSpellCoolDown && !bGroupCoolDown)
			{
				Abilities(i)->bCoolDown = FALSE;
			}
		}
	}

	// 由服务器来控制对象删除
	//// to do : check whick npc id is -1
	//if (!IsPendingKill() && ServerID != -1)
	//{
	//	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainPlayer();
	//	if (mainActor)
	//	{
	//		INT MainActorX = mainActor->Location.X / 2048.f;
	//		INT MainActorY = mainActor->Location.Y / 2048.f;

	//		INT GridX = Location.X / 2048.f;
	//		INT GridY = Location.Y / 2048.f;

	//		UBOOL bKill = FALSE;
	//		if (abs(MainActorX - GridX) >= 3)
	//		{
	//			bKill = TRUE;
	//		}

	//		if (abs(MainActorY - GridY) >= 3)
	//		{
	//			bKill = TRUE;
	//		}

	//		if (bKill)
	//		{
	//			MMOFacade::getSingleton().DestroyObject(ServerID, TRUE);
	//		}
	//	}
	//}

	// scale
	if (bScaling)
	{
		FLOAT NewCollisionRadius = UseingAvatar ? UseingAvatar->CollisionRadius * UseingAvatar->AvatarDrawScale : 1.0f;
		FLOAT NewCollisionHeight = UseingAvatar ? UseingAvatar->CollisionHeight * UseingAvatar->AvatarDrawScale : 1.f;
		if (ScaleLeftTime <= DeltaSeconds)
		{
			SetDrawScale3D(ScaleTarget);
			bScaling = FALSE;
			ScaleLeftTime = 0.f;

			NewCollisionRadius = ScaleTarget.X * NewCollisionRadius;
			NewCollisionHeight = ScaleTarget.Z * NewCollisionHeight;
		}
		else
		{
			ScaleLeftTime -= DeltaSeconds;

			FLOAT alpha = ScaleLeftTime / ScaleTime;
			FVector NewScale = ScaleTarget + (DrawScale3D - ScaleTarget) * alpha;
			SetDrawScale3D(NewScale);

			// update collision size
			NewCollisionRadius = NewScale.X * NewCollisionRadius;
			NewCollisionHeight = NewScale.Z * NewCollisionHeight;
		}

		SetCollisionSize(NewCollisionRadius, NewCollisionHeight);

		UpdateLocationZ();
	}

	UpdatePositionTransformEffect(DeltaSeconds);
}
//------------------------------------------------------------------------
USkeletalMeshComponent* AMMOActor::GetMasterComponent()
{
	return Mesh;
}
//------------------------------------------------------------------------
void AMMOActor::RefreshAnimSet()
{
	EWeaponType WeaponType;

	if( AttitudeType >= EWT_Riding_1 )//坐骑姿态
	{
		WeaponType = (EWeaponType)AttitudeType;
	}
	else
	{
		if( (ItemManager != NULL)
			&& bHoldWeapon==TRUE	//可以拿武器
			&& (ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND) != NULL)
			&& (ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND)->GetEquipAppearenceTemplate() != NULL))
		{
			WeaponType = (EWeaponType)ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND)->GetEquipAppearenceTemplate()->WeaponType;
		}
		else
		{
			WeaponType = EWT_Weapon_0; //空手
		}
	}

	if(UseingAvatar != NULL)
	{
		for(INT i = 0; i < UseingAvatar->WeaponAnimSetPairs.Num(); ++i)
		{
			if(WeaponType == UseingAvatar->WeaponAnimSetPairs(i).WeaponType)
			{
				if(Mesh != NULL && UseingAvatar->WeaponAnimSetPairs(i).AnimSets.Num() > 0)
				{
					if (Mesh->AnimSets != UseingAvatar->WeaponAnimSetPairs(i).AnimSets)
					{
						Mesh->AnimSets.Remove(0, Mesh->AnimSets.Num());
						for(INT j = 0; j < UseingAvatar->WeaponAnimSetPairs(i).AnimSets.Num(); ++j)
						{	
							Mesh->AnimSets.AddItem(UseingAvatar->WeaponAnimSetPairs(i).AnimSets(j));
						}
						Mesh->UpdateAnimations();
					}
					RefreshAnim();
				}
				break;
			}
		}
	}
}

void AMMOActor::RefreshMovementSpeed()
{
	UAvatarData* ToUseAvatar = UseingAvatar;
	if (MeshChangeAvatarData)
	{
		ToUseAvatar = MeshChangeAvatarData;
	}

	// only support npc
	if (ToUseAvatar && IsA(ANPC::StaticClass()) || MeshChangeAvatarData)
	{
		if (ToUseAvatar->WalkSpeed < 0.f)
		{
			ToUseAvatar->WalkSpeed = CalculateRootBoneMoveSpeed(PawnAnimInfo.WalkAnim);
		}

		if (ToUseAvatar->RunSpeed < 0.f)
		{
			ToUseAvatar->RunSpeed = CalculateRootBoneMoveSpeed(PawnAnimInfo.RunAnim);
		}

		if (ToUseAvatar->FlySpeed < 0.f)
		{
			ToUseAvatar->FlySpeed = CalculateRootBoneMoveSpeed(PawnAnimInfo.FlyAnim);
		}
	}
}

BYTE AMMOActor::GetWeaponType()
{
	EWeaponType type = EWT_Weapon_0;    // 空手
	if( AttitudeType >= EWT_Riding_1 )
	{
		type = (EWeaponType)AttitudeType;
	}
	else if( ItemManager && ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND)
		&& (ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND)->GetEquipAppearenceTemplate() != NULL))
	{
		type = EWeaponType(ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND)->GetEquipAppearenceTemplate()->WeaponType);
	}																		
	return type;
}

BYTE AMMOActor::GetWeaponAppearenceType()
{
	EWeaponAppearenceType type = EWAT_Type0;    // 空手
	if( ItemManager && ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND)
		&& (ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND)->GetEquipAppearenceTemplate() != NULL))
	{
		type = EWeaponAppearenceType(ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND)->GetEquipAppearenceTemplate()->WeaponAppearenceType);
	}																		
	return type;
}


BYTE AMMOActor::GetBodyMaterialType()
{
	BYTE Ret = EIMAT_Default;

	if (ItemManager && ItemManager->EquippedItemList_Normal(E_NES_BODY))
	{
		Ret = ItemManager->EquippedItemList_Normal(E_NES_BODY)->SoundMaterialType;
	}

	if (Ret == EIMAT_Default)
	{
		Ret = SoundMaterialType;
	}

	return Ret;
}

BYTE AMMOActor::GetBodyClothNoiseType()
{
	BYTE Ret = ECNT_None;
	if (ItemManager && ItemManager->EquippedItemList_Normal(E_NES_BODY))
	{
		Ret = ItemManager->EquippedItemList_Normal(E_NES_BODY)->ClothNoiseType;
	}
	return Ret;


}

void AMMOActor::PlayClothNoise(BYTE InActionType)
{
	if (!IsMainPlayer())
	{
		return;
	}
	BYTE ClothType = GetBodyClothNoiseType();

	BYTE GenderType = ActorAttributes ? ActorAttributes->GenderType : EGT_Asexua;

	USoundCue* SoundToPlay = GpDataManager->GetClothNoise(GenderType, ClothType, InActionType);
	if (SoundToPlay)
	{
		PlaySound(SoundToPlay);
	}
}

void AMMOActor::StartMeshChangeEffect(INT InEffectId,class UAvatarData* InNewAvatar)
{
	if (!InNewAvatar)
	{
		return;
	}

	if (!UseingAvatar)
	{
		MMOFacade::getSingleton().ForceSyncObject(ServerID, TRUE);
		if (UseingAvatar == NULL)
		{
			//check(FALSE);
			return;
		}
	}

	if (Mesh == NULL)
	{
		return;
	}

	if (MeshChangeEffectId != 0)
	{
		StopMeshChangeEffect(MeshChangeEffectId);
	}

	MeshChangeEffectId = InEffectId;

	MeshChangeAvatarData = InNewAvatar;

	if (OverriddenMesh == NULL)
	{
		OverriddenMesh = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass(), this);
		
		debugf( NAME_DevGarbage, TEXT("GC StartMeshChangeEffect.ConstructObject:%x %s"),
#ifdef _WIN64
			(UINT64)OverriddenMesh
#else
			(UINT)OverriddenMesh
#endif
			, *OverriddenMesh->GetFullName()
			);  // oscar@snda
	}
	else
	{
		warnf(NAME_Error, TEXT("AMMOActor::StartMeshChangeEffect OverriddenMesh not NULL!"));
#if !SHIPPING_PC_GAME
		appMsgf(AMT_OK, TEXT("AMMOActor::StartMeshChangeEffect OverriddenMesh not NULL!"));
#endif
	}

	OverriddenMesh->SetLightingChannels(Mesh->LightingChannels);
	OverriddenMesh->SetLightEnvironment(Mesh->LightEnvironment);

	OverriddenMesh->SetSkeletalMesh(InNewAvatar->SkeletalMesh);
	OverriddenMesh->SetAnimTreeTemplate(InNewAvatar->AnimationTree);
	OverriddenMesh->SetPhysicsAsset(InNewAvatar->PhysicAsset);
	if( InNewAvatar->OverrideLightingChannels )
		OverriddenMesh->SetLightingChannels( InNewAvatar->LightingChannels );

	RefreshCollision();

	for (INT i = 0; i < InNewAvatar->WeaponAnimSetPairs.Num(); ++i)
	{
		if (InNewAvatar->WeaponAnimSetPairs(i).AnimSets.Num() > 0)
		{
			OverriddenMesh->AnimSets = InNewAvatar->WeaponAnimSetPairs(i).AnimSets;
			break;
		}
	}

	if (!MeshChangeAnimController)
	{
		MeshChangeAnimController = ConstructObject<UMMOActorAT>(UMMOActorAT::StaticClass(), this);
	}
	MeshChangeAnimController->InitAnim(OverriddenMesh, this);

	RefreshAnim();

	//SetVisibleByAbility(FALSE);

	AttachComponent(OverriddenMesh);

	RefreshVisible();

	OverriddenMesh->RebulidMeshMaterialList();

	RefreshMovementSpeed();
}

void AMMOActor::StopMeshChangeEffect(INT InEffectId)
{
	if (MeshChangeEffectId == InEffectId)
	{
		//for (INT i = 0; i < Components.Num(); ++i)
		//{
		//	UPrimitiveComponent* TestPri = Cast<UPrimitiveComponent>(Components(i));
		//	if (TestPri)
		//	{
		//		TestPri->SetHiddenGame(FALSE);
		//		USkeletalMeshComponent* TestMesh = Cast<USkeletalMeshComponent>(TestPri);
		//		if (TestMesh)
		//		{
		//			for (INT j = 0; j < TestMesh->Attachments.Num(); ++j)
		//			{
		//				UPrimitiveComponent* TestSubPri = Cast<UPrimitiveComponent>(TestMesh->Attachments(j).Component);
		//				if (TestSubPri)
		//				{
		//					TestSubPri->SetHiddenGame(FALSE);
		//				}
		//			}
		//		}
		//	}
		//}

		check(OverriddenMesh);
		DetachComponent(OverriddenMesh);
		//OverriddenMesh->MarkPendingKill();
		check(MeshChangeAvatarData);
		OverriddenMesh = NULL;
		MeshChangeAvatarData = NULL;
		MeshChangeEffectId = 0;

		RefreshCollision();
		//SetVisibleByAbility(TRUE);
		RefreshVisible();
	}
	else
	{
		// do nothing
	}
}

UBOOL AMMOActor::IsInMeshChangeState()
{								  
	if (MeshChangeEffectId != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UBOOL AMMOActor::IsBaseMeshHidden()
{
	if (Mesh)
	{
		return Mesh->HiddenGame;
	}

	return FALSE;
}


void AMMOActor::StartPositionTransformEffect(INT InEffectId,class UPositionTransformComponent* InData)
{
	if (!InData)
	{
		return;
	}

	if (PositionTransformEffectId != 0)
	{
		return;
	}

	if (PassiveMoveType != PassiveMoveType_None)
	{
		return;
	}

	if (InData->Values.Points.Num() == 0)
	{
		return;
	}

	if (InData->Duration <= 0.0001)
	{
		return;
	}


	PositionTransformEffectId = InEffectId;
	PositionTransformData = InData;

	PositionTransformCurrentTime = 0.f;
	PositionTransformBaseTransform = LocalToWorld();
}

void AMMOActor::StopPositionTransformEffect(INT InEffectId)
{
	if (PositionTransformEffectId == InEffectId)
	{
		if (PositionTransformData->EndPositionType == EndPositionType_CurrentPoint)
		{
			// do nothing
		}
		else if (PositionTransformData->EndPositionType == EndPositionType_StartPoint)
		{
			SetLocation(PositionTransformBaseTransform.GetOrigin());
		}
		else if (PositionTransformData->EndPositionType == EndPositionType_EndPoint)
		{
			FVector EndPoint = PositionTransformData->Values.Points.Last().OutVal;
			FMatrix StopMatrix = FTranslationMatrix(EndPoint) * PositionTransformBaseTransform;
			SetLocation(StopMatrix.GetOrigin());
		}
	}

	PositionTransformEffectId = 0;
}

void AMMOActor::UpdatePositionTransformEffect(FLOAT Delta)
{
	if (PositionTransformEffectId == 0)
	{
		return;
	}

	PositionTransformCurrentTime += Delta;
	FLOAT alp = 0.f;
	if (PositionTransformData->bResizePointTimeToComponentLifespan)
	{
		alp = PositionTransformCurrentTime / PositionTransformData->Duration;
		alp *= PositionTransformData->Values.Points.Last().InVal;
	}
	else
	{
		alp = PositionTransformCurrentTime;
	}

	FVector Pos = PositionTransformData->Values.Eval(alp, FVector(0, 0, 0));
	Pos = PositionTransformBaseTransform.TransformFVector(Pos);
	SetLocation(Pos);

	if (PositionTransformCurrentTime >= PositionTransformData->Duration)
	{
		StopPositionTransformEffect(PositionTransformEffectId);
	}
}


//------------------------------------------------------------------------
void AMMOActor::RefreshAttachAnim()
{
	for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
	{
		if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT )
		{

			if(bDead)
			{
				AttachedDestructableParts(i)->ActorAT->SetIdleAnim(PawnAnimInfo.DeadAnim);
				AttachedDestructableParts(i)->ActorAT->SetMoveAnim(PawnAnimInfo.DeadAnim);
			}
			else
			{
				AttachedDestructableParts(i)->ActorAT->SetIdleAnim(PawnAnimInfo.IdleAnim);
				AttachedDestructableParts(i)->ActorAT->SetMoveAnim(PawnAnimInfo.RunAnim);
			}

			AttachedDestructableParts(i)->ActorAT->SetActionLoopBlendTarget(0.f, 0.f);		
		}
	}

}

void AMMOActor::RefreshCollision()
{
	UAvatarData* CurrentAV = UseingAvatar;
	if (MeshChangeAvatarData)
	{
		CurrentAV = MeshChangeAvatarData;
	}

	if (CurrentAV)
	{
		SetDrawScale(CurrentAV->MeshScale * CurrentAV->AvatarDrawScale);
		SetCollisionSize(CurrentAV->CollisionRadius * CurrentAV->AvatarDrawScale, CurrentAV->CollisionHeight * CurrentAV->AvatarDrawScale);
		UpdateLocationZ();
	}
}

void AMMOActor::RefreshMoveAnim()
{
	if (!IsInMeshChangeState())
	{
		if(ActorAT)
		{
			ActorAT->RefreshMoveAnim();
		}

		// weapon animation
		UMMOActorAT* WeaponAT = GetWeaponEquipMeshAnimationController(0);
		if (WeaponAT)
		{
			WeaponAT->RefreshMoveAnim();
		}
		WeaponAT = GetWeaponEquipMeshAnimationController(1);
		if (WeaponAT)
		{
			WeaponAT->RefreshMoveAnim();
		}

		if(MountInfo && MountInfo->ActorAT )MountInfo->ActorAT->RefreshMoveAnim();

		for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
		{
			if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT )
			{
				AttachedDestructableParts(i)->ActorAT->RefreshMoveAnim();
			}
		}
	}
	else
	{
		if (MeshChangeAnimController)
		{
			MeshChangeAnimController->RefreshMoveAnim();
		}
	}
}

void AMMOActor::SetMoveAnimRate(FLOAT InRate)
{
	if (CurrentMoveControlComponentId != 0)
	{
		return;
	}

	MoveAnimRate = InRate;

	if (MeshChangeAnimController)
	{
		MeshChangeAnimController->SetMoveAnimRate(InRate);
	}

	if(ActorAT)
	{
		ActorAT->SetMoveAnimRate(InRate);
	}


	// weapon animation
	UMMOActorAT* WeaponAT = GetWeaponEquipMeshAnimationController(0);
	if (WeaponAT)
	{
		WeaponAT->SetMoveAnimRate(InRate);
	}
	WeaponAT = GetWeaponEquipMeshAnimationController(1);
	if (WeaponAT)
	{
		WeaponAT->SetMoveAnimRate(InRate);
	}

	if(MountInfo && MountInfo->ActorAT )
	{
		MountInfo->ActorAT->SetMoveAnimRate(InRate);
	}

	for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
	{
		if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT )
		{
			AttachedDestructableParts(i)->ActorAT->SetMoveAnimRate(InRate);
		}
	}
}

void AMMOActor::RefreshAnim()
{
	//debugf(TEXT("====RefreshAnim()==== %s"), *PawnAnimInfo.IdleAnim.ToString());

	FName ToUseIdleAnim, ToUseMoveAnim;
	if (bDead)
	{
		ToUseIdleAnim = PawnAnimInfo.DeadAnim;
		ToUseMoveAnim = PawnAnimInfo.DeadAnim;
	}
	else
	{
		if (bFightingState)
		{
			ToUseIdleAnim = PawnAnimInfo.FightIdleAnim;
		}
		else
		{
			ToUseIdleAnim = PawnAnimInfo.IdleAnim;
		}
		
		if (MoveType == EMT_Walk)
		{
			ToUseMoveAnim = PawnAnimInfo.WalkAnim;
		}
		else if (MoveType == EMT_Run)
		{
			ToUseMoveAnim = PawnAnimInfo.RunAnim;
		}
		else if (MoveType == EMT_Walk_Oblique)
		{
			ToUseMoveAnim = PawnAnimInfo.WalkAnim_Oblique;
		}
		else if (MoveType == EMT_Run_Oblique)
		{
			ToUseMoveAnim = PawnAnimInfo.RunAnim_Oblique;
		}
		else if (MoveType == EMT_Fly)
		{
			ToUseMoveAnim = PawnAnimInfo.FlyAnim;//.RunAnim_Oblique;
		}
		else if (MoveType == EMT_Fly_Oblique)
		{
			ToUseMoveAnim = PawnAnimInfo.FlyAnim_Oblique;
		}
		else
		{
			ToUseMoveAnim = PawnAnimInfo.RunAnim;
		}
	}


	SetIdleAnim(ToUseIdleAnim);
	SetMoveAnim(ToUseMoveAnim);

	if (!IsInMeshChangeState())
	{
		if(ActorAT)
		{
			ActorAT->SetActionLoopBlendTarget(0.f, 0.f);
		}


		if(MountInfo && MountInfo->ActorAT )
		{
			MountInfo->ActorAT->SetIdleAnim(ToUseIdleAnim);
			MountInfo->ActorAT->SetMoveAnim(ToUseMoveAnim);

			MountInfo->ActorAT->SetActionLoopBlendTarget(0.f, 0.f);
		}


		RefreshAttachAnim();


		// weapon animation
		UMMOActorAT* WeaponAT = GetWeaponEquipMeshAnimationController(0);
		if (WeaponAT)
		{
			WeaponAT->SetIdleAnim(ToUseIdleAnim);
			WeaponAT->SetMoveAnim(ToUseMoveAnim);
			WeaponAT->SetActionLoopBlendTarget(0.f, 0.f);
		}
		WeaponAT = GetWeaponEquipMeshAnimationController(1);
		if (WeaponAT)
		{
			WeaponAT->SetIdleAnim(ToUseIdleAnim);
			WeaponAT->SetMoveAnim(BackupIdleAnim);
			WeaponAT->SetActionLoopBlendTarget(0.f, 0.f);
		}
	}
	else
	{
		if (MeshChangeAnimController)
		{
			MeshChangeAnimController->SetActionLoopBlendTarget(0.f, 0.f);
		}
	}
}
//------------------------------------------------------------------------

void AMMOActor::SwitchFightIdle(UBOOL bFight)
{
	if (bFightingState != bFight)
	{
		ResetIdleTimer();
	}

	bFightingState = bFight;

	if (bFight)
	{
		if (CurrentMoveControlComponentId == 0)
		{
			if (Mesh->FindAnimSequence(PawnAnimInfo.FightIdleAnim))
			{
				SetIdleAnim(PawnAnimInfo.FightIdleAnim);
			}
		}
		else
		{
			if (Mesh->FindAnimSequence(PawnAnimInfo.FightIdleAnim))
			{
				BackupIdleAnim = PawnAnimInfo.FightIdleAnim;
			}
		}
	}
	else
	{
		if (CurrentMoveControlComponentId == 0)
		{
			if (Mesh->FindAnimSequence(PawnAnimInfo.IdleAnim))
			{
				SetIdleAnim(PawnAnimInfo.IdleAnim);
			}
		}
		else
		{
			if (Mesh->FindAnimSequence(PawnAnimInfo.IdleAnim))
			{
				BackupIdleAnim = PawnAnimInfo.IdleAnim;
			}
		}
	}
}





void AMMOActor::SetIdleAnim(FName NewIdleLoopAnim)
{	
	CurrentMoveControlComponentId = 0;

	if(ActorAT)
	{
		ActorAT->SetIdleAnim(NewIdleLoopAnim);
	}

	if(MountInfo && MountInfo->ActorAT )
	{
		MountInfo->ActorAT->SetIdleAnim(NewIdleLoopAnim);
	}

	AArmorActor* weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon));
	if (weapon)
	{
		weapon->SetIdleAnim(NewIdleLoopAnim);
	}

	weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon2));
	if (weapon)
	{
		weapon->SetIdleAnim(NewIdleLoopAnim);
	}


	//class CPlayMoveAnimAction
	//{
	//public:
	//	static void PalyAnim( UMMOActorAT* &pAnimCtr, FName NewMoveLoopAnim )
	//{
	//		pAnimCtr->SetIdleAnim( NewMoveLoopAnim );
	//}
	//};
	//if ( ItemManager )
	//{
	//	ItemManager->PalyAnimOnSkeletalEquipMesh<CPlayMoveAnimAction>( NewIdleLoopAnim );
	//}

	// weapon animation
// 	UMMOActorAT* WeaponAT = GetWeaponEquipMeshAnimationController(0);
// 	if (WeaponAT)
// 	{
// 		WeaponAT->SetIdleAnim(NewIdleLoopAnim);
// 	}
// 	WeaponAT = GetWeaponEquipMeshAnimationController(1);
// 	if (WeaponAT)
// 	{
// 		WeaponAT->SetIdleAnim(NewIdleLoopAnim);
// 	}

	for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
	{
		if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT )
		{
			AttachedDestructableParts(i)->ActorAT->SetIdleAnim(NewIdleLoopAnim);
		}
	}

	if (MeshChangeAnimController)
	{
		MeshChangeAnimController->SetIdleAnim(NewIdleLoopAnim);
	}
}
//------------------------------------------------------------------------
void AMMOActor::SetMoveAnim(FName NewMoveLoopAnim)
{
	CurrentMoveControlComponentId = 0;

	if(ActorAT)ActorAT->SetMoveAnim(NewMoveLoopAnim);

	if(MountInfo && MountInfo->ActorAT )MountInfo->ActorAT->SetMoveAnim(NewMoveLoopAnim);

	AArmorActor* weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon));
	if (weapon)
	{
		weapon->SetMoveAnim(NewMoveLoopAnim);
	}

	weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon2));
	if (weapon)
	{
		weapon->SetMoveAnim(NewMoveLoopAnim);
	}



	//class CPlayMoveAnimAction
	//{
	//public:
	//	static void PalyAnim( UMMOActorAT* &pAnimCtr, FName NewMoveLoopAnim )
	//	{
	//		pAnimCtr->SetMoveAnim( NewMoveLoopAnim );
	//}
	//};
	//if ( ItemManager )
	//{
	//	ItemManager->PalyAnimOnSkeletalEquipMesh<CPlayMoveAnimAction>( NewMoveLoopAnim );
	//}

	// weapon animation
// 	UMMOActorAT* WeaponAT = GetWeaponEquipMeshAnimationController(0);
// 	if (WeaponAT)
// 	{
// 		WeaponAT->SetMoveAnim(NewMoveLoopAnim);
// 	}
// 	WeaponAT = GetWeaponEquipMeshAnimationController(1);
// 	if (WeaponAT)
// 	{
// 		WeaponAT->SetMoveAnim(NewMoveLoopAnim);
// 	}

	for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
	{
		if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT )
		{
			AttachedDestructableParts(i)->ActorAT->SetMoveAnim(NewMoveLoopAnim);
		}
	}

	if (MeshChangeAnimController)
	{
		MeshChangeAnimController->SetMoveAnim(NewMoveLoopAnim);
	}
}
//------------------------------------------------------------------------


FName AMMOActor::GetIdleAnim()
{
	FName Ret = NAME_None;
	if (!IsInMeshChangeState())
	{
		if (ActorAT)
		{
			Ret = ActorAT->GetIdleAnim();
		}
	}
	else
	{
		if (MeshChangeAnimController)
		{
			Ret = MeshChangeAnimController->GetIdleAnim();
		}
	}


	return Ret;
}


FName AMMOActor::GetMoveAnim()
{
	FName Ret = NAME_None;
	if (!IsInMeshChangeState())
	{
		if (ActorAT)
		{
			Ret = ActorAT->GetMoveAnim();
		}
	}
	else
	{
		if (MeshChangeAnimController)
		{
			Ret = MeshChangeAnimController->GetMoveAnim();
		}
	}


	return Ret;
}


void AMMOActor::SetAnimRateById(INT InId, FLOAT InRate)
{
	if (!IsInMeshChangeState())
	{
		if(ActorAT)ActorAT->SetAnimRateById(InId, InRate);

		if(MountInfo && MountInfo->ActorAT )MountInfo->ActorAT->SetAnimRateById(InId, InRate);

		// weapon animation
		UMMOActorAT* WeaponAT = GetWeaponEquipMeshAnimationController(0);
		if (WeaponAT)
		{
			WeaponAT->SetAnimRateById(InId, InRate);
		}
		WeaponAT = GetWeaponEquipMeshAnimationController(1);
		if (WeaponAT)
		{
			WeaponAT->SetAnimRateById(InId, InRate);
		}

		for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
		{
			if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT )
			{
				AttachedDestructableParts(i)->ActorAT->SetAnimRateById(InId, InRate);
			}
		}
	}
	else
	{
		if (MeshChangeAnimController)
		{
			MeshChangeAnimController->SetAnimRateById(InId, InRate);
		}
	}

}



class UEquip* AMMOActor::GetWeaponEquip()
{
	if(ItemManager && ItemManager->EquippedItemList_Normal.Num() > 0)
	{
		return ItemManager->EquippedItemList_Normal(E_NES_MAIN_HAND);
	}
	else
	{
		return NULL;
	}
}
//
//class UEquipMesh* AMMOActor::GetWeaponEquipMesh(INT InIndex)
//{
//	UEquip* WeaponEquip = GetWeaponEquip();
//	if (WeaponEquip)
//	{
//		if (InIndex >= 0 && InIndex < WeaponEquip->MeshList.Num())
//		{
//			return WeaponEquip->MeshList(InIndex);
//		}
//	}
//	return NULL;
//}

UBOOL AMMOActor::GetWeaponSkeletalMeshComponents(class USkeletalMeshComponent*& Mesh1,class USkeletalMeshComponent*& Mesh2)
{
	UBOOL bFind = FALSE;
	AArmorActor* weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon));
	if (weapon)
	{
		if (weapon->MainMesh->IsA(USkeletalMeshComponent::StaticClass()))
		{
			Mesh1 = (USkeletalMeshComponent*)weapon->MainMesh;
			bFind = TRUE;
		}
	}

	weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon2));
	if (weapon)
	{
		if (weapon->MainMesh->IsA(USkeletalMeshComponent::StaticClass()))
		{
			Mesh2 = (USkeletalMeshComponent*)weapon->MainMesh;
			bFind = TRUE;
		}
	}

	return bFind;
}

UBOOL AMMOActor::GetWeaponMeshComponents(class UMeshComponent*& Mesh1,class UMeshComponent*& Mesh2)
{
	AArmorActor* weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon));
	if (weapon)
	{
		Mesh1 = weapon->SubMesh ? weapon->SubMesh : weapon->MainMesh;
		}

	weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon2));
	if (weapon)
		{
		Mesh2 = weapon->SubMesh ? weapon->SubMesh : weapon->MainMesh;
				}

	if (Mesh1 || Mesh2)
				{
		return TRUE;
			}
	else
	{
		return FALSE;
		}
	}

class UMMOActorAT* AMMOActor::GetWeaponEquipMeshAnimationController(INT InIndex)
{
	BYTE t = EBMT_Weapon;
	if (InIndex == 1)
	{
		t = EBMT_Weapon2;
	}

	AArmorActor* weapon = Cast<AArmorActor>(GetEquip(t));
	if (!weapon)
	{
		return NULL;
	}

	return weapon->AnimationController;
}

FLOAT AMMOActor::PlayAnimation(FName Anim,INT InKey,FLOAT Rate,UBOOL bLoop,BYTE RootBoneAxisX,BYTE RootBoneAxisY,BYTE RootBoneAxisZ,BYTE BlendType,FLOAT Position,FLOAT InBlendIn,FLOAT InBlendOut,BYTE InLevel)
{
#if SHOW_ABILITY_ANIMATION_LOG
	{
		debugf(TEXT("##Id=%d Tid=%s PlayAnimation Name=%s Key=%d Level=%d Rate=%f"),ServerID, (UseingAvatar ? *UseingAvatar->GetName() : L"none"), *(Anim.ToString()), InKey, InLevel, Rate);
	}
#endif

	if(!ActorAT)
	{
		ActorAT = CastChecked<UMMOActorAT>( UObject::StaticConstructObject(UMMOActorAT::StaticClass(), this) );
	}

	//if (IsA(ANPC::StaticClass()))
	//{
	//	debugf(TEXT("##Id=%d PlayAnimation Name=%s Key=%d Level=%d Rate=%f"),ServerID, *(Anim.ToString()), InKey, InLevel, Rate);
	//	if (Anim == FName(TEXT("Run")))
	//	{
	//		debugf(L"play run!!!!!");
	//	}
	//}

	UMMOActorAT* CurrentAT = NULL;
	if (!IsInMeshChangeState())
	{
		CurrentAT = ActorAT;
	}
	else
	{
		CurrentAT = MeshChangeAnimController;
	}

	if(!CurrentAT)
	{
		warnf(NAME_Error, TEXT("AMMOActor::PlayAnimation NULL MMOActorAT"));
		return 0.0f;
	}


	UBOOL bNeedPlay = FALSE;
	INT ActiveKey = 0;
	BYTE ActiveLevel = 0;
	FLOAT ActivePercent = 0.f;
	FName ActiveAnimName;
	UBOOL bHasAnim = CurrentAT->GetCurrentAnimInfo(ActiveKey, ActiveLevel, ActivePercent, ActiveAnimName);
	if (bHasAnim == FALSE)
	{
		bNeedPlay = TRUE;
	}
	else
	{
		/*
		if (InLevel > ActiveLevel)
		{
			bNeedPlay = TRUE;
		}
		else if (InLevel == ActiveLevel)
		{
			if (InLevel == EAL_HighWound || InLevel == EAL_LowWound)
			{
				bNeedPlay = ActivePercent >= GetWoundAnimThresholdValue();
			}
			else
			{
				bNeedPlay = TRUE;
			}
		}
		else
		{
			bNeedPlay = FALSE;
		}
		*/

		// fix bug #24012
		if (ActiveLevel == EAL_HighWound)
		{
			if (InLevel == EAL_HighWound)
			{
				bNeedPlay = ActivePercent >= GetWoundAnimThresholdValue();
			}
			else
			{
				bNeedPlay = TRUE;
			}
		}
		else
		{
			if (InLevel == EAL_HighWound)
			{
				if (InLevel > ActiveLevel && ActivePercent > 0.3)
				{
					bNeedPlay = TRUE;
				}
				else
				{
					bNeedPlay = FALSE;
				}
			}
			else
			{
				if (InLevel > ActiveLevel)
				{
					bNeedPlay = TRUE;
				}
				else if (InLevel == ActiveLevel)
				{
					if (InLevel == EAL_LowWound)
					{
						bNeedPlay = ActivePercent >= GetWoundAnimThresholdValue();
					}
					else
					{
						bNeedPlay = TRUE;
					}
				}
				else
				{
					bNeedPlay = FALSE;
				}
			}
		}
	}

	if (!bNeedPlay)
	{
		return 0;
	}



	FLOAT len = CurrentAT->PlayAnimation(Anim, InKey, Rate, bLoop, RootBoneAxisX, RootBoneAxisY, RootBoneAxisZ, BlendType, Position, InBlendIn, InBlendOut, InLevel);

#if SHOW_ABILITY_ANIMATION_LOG
	debugf(TEXT("Played Anim=%s, len=%f"), *(Anim.ToString()), len);
#endif


	if (!IsInMeshChangeState())
	{
		// weapon animation
		AArmorActor* Weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon));
		if (Weapon)
		{
			Weapon->PlayAnimation(Anim, InKey, Rate, bLoop, RootBoneAxisX, RootBoneAxisY, RootBoneAxisZ, BlendType, Position, InBlendIn, InBlendOut, InLevel);
		}
		Weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon2));
		if (Weapon)
		{
			Weapon->PlayAnimation(Anim, InKey, Rate, bLoop, RootBoneAxisX, RootBoneAxisY, RootBoneAxisZ, BlendType, Position, InBlendIn, InBlendOut, InLevel);
		}

		for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
		{
			if (AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT)
			{
				AttachedDestructableParts(i)->ActorAT->PlayAnimation(Anim, InKey, Rate, bLoop, RootBoneAxisX, RootBoneAxisY, RootBoneAxisZ, BlendType, Position, InBlendIn, InBlendOut, InLevel);
			}
		}


		if (MountInfo && MountInfo->ActorAT)
		{
			MountInfo->ActorAT->PlayAnimation(Anim, InKey, Rate, bLoop, RootBoneAxisX, RootBoneAxisY, RootBoneAxisZ, BlendType, Position, InBlendIn, InBlendOut, InLevel);
		}
	}

	return len;
}



INT AMMOActor::GetNewAnimKey()
{
	static INT i = 0;
	++i;
	if (i == 0)
	{
		++i;
	}
	return i;
}

INT AMMOActor::GetCurrentAnimKey()
{
	if (!IsInMeshChangeState())
	{
		if (ActorAT && ActorAT->ActionBlend)
		{
			return ActorAT->ActionBlend->ActiveAnimKey;
		}
	}
	else
	{
		if (MeshChangeAnimController && MeshChangeAnimController->ActionBlend)
		{
			return MeshChangeAnimController->ActionBlend->ActiveAnimKey;
		}
	}

	return 0;
}

class USkeletalMeshComponent* AMMOActor::GetMainMesh()
{
	if (OverriddenMesh && !OverriddenMesh->IsPendingKill())
	{
		return OverriddenMesh;
	}
	else
	{
		return Mesh;
	}
}

void AMMOActor::SetFreeze(UBOOL InFreeze,FVector FreezeLocation)
{
	bFreeze = InFreeze;
	if (InFreeze)
	{
		if (IsMainPlayer())
		{
			check(MainController);
			MainController->StopMove(FALSE);
		}
		StopMovement();

		const FLOAT GridSize = 64.f;
		if ((FreezeLocation - Location).Size2D() >= 64.f)
		{
			FreezeLocation.Z = Location.Z;
			SetLocation(FreezeLocation);
		}
	}
}

void AMMOActor::SetShowTime(UBOOL bOn,FVector EndShowLocation)
{
	bShowTime = bOn;
	if (bOn)
	{

	}
	else
	{
		if ((EndShowLocation - Location).IsNearlyZero() == FALSE)
		{
			SetLocation(EndShowLocation);
		}
	}
}

void AMMOActor::RefreshCollisonRate()
{
	if (CylinderComponent && WorldInfo && WorldInfo->Game && WorldInfo->Game->IsA(AMMOGame::StaticClass()))
	{
		CollisionHeightRate = CylinderComponent->CollisionHeight / ((AMMOGame*)(WorldInfo->Game))->StandardCollisionHeight;
		CollisionRadiusRate = CylinderComponent->CollisionRadius / ((AMMOGame*)(WorldInfo->Game))->StandardCollisionRadius;
	}
	else
	{
		CollisionHeightRate = CollisionRadiusRate = 1.0;
	}
}


void AMMOActor::NativeSetMesh(BYTE ClassType, BYTE GenderType)
{
	INT i;

	//ClassType = 0; 
	//GenderType = 1;
	//`log("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@SetMesh ClassType="$ClassType$"   GenderType="$GenderType);
	// TEMP-DIAG(1.05 backport): remove once class models are verified
	debugf(NAME_Log, TEXT("[SetMeshDiag] actor=%s class=%d gender=%d playerList=%d"), *GetName(), (INT)ClassType, (INT)GenderType, GpDataManager ? GpDataManager->PlayerList.Num() : -1);
	if(GpDataManager != NULL)
	{
		for(INT d = 0; d < GpDataManager->PlayerList.Num(); ++d)
		{
			debugf(NAME_Log, TEXT("[SetMeshDiag]   entry %d: class=%d gender=%d avatar=%s skel=%s"), d, (INT)GpDataManager->PlayerList(d).ClassType, (INT)GpDataManager->PlayerList(d).GenderType, GpDataManager->PlayerList(d).AvatarDataLink ? *GpDataManager->PlayerList(d).AvatarDataLink->GetName() : TEXT("NULL"), (GpDataManager->PlayerList(d).AvatarDataLink && GpDataManager->PlayerList(d).AvatarDataLink->SkeletalMesh) ? *GpDataManager->PlayerList(d).AvatarDataLink->SkeletalMesh->GetName() : TEXT("NULL"));
		}
	}
	if(GpDataManager == NULL)
		return;
	if( GpDataManager->PlayerList.Num() == 0)
	{
		//`warn("GamePlayDataManager.PlayerList is none!");
		return;
	}

	for(i = 0; i < GpDataManager->PlayerList.Num(); ++i)
	{	 
		if(GpDataManager->PlayerList(i).ClassType == ClassType && GpDataManager->PlayerList(i).GenderType == GenderType)
		{
			if(GpDataManager->PlayerList(i).AvatarDataLink != NULL && GpDataManager->PlayerList(i).AvatarDataLink->SkeletalMesh != NULL)
			{
				//用一个avatar变量
				UseingAvatar = GpDataManager->PlayerList(i).AvatarDataLink;
				bMainSkelChanged = true;

				if(ActorAttributes != NULL)
				{
					ActorAttributes->ClassType = ClassType;
					ActorAttributes->GenderType = GenderType;
				}

				RefreshAvatar();

				
				CurrentModelTypeIdx = i;


				eventInitAnimNodes();
				RefreshAnimSet();
				//RefreshCollision();
			}
			break;
		}
	}

	if(ActorAttributes&&ActorAttributes->MountID>0)
	{
		EquipMount(ActorAttributes->MountID);
	}
	else
	{
		UnEquipMount();
	}
}


void AMMOActor::SetActorLevel( INT level )
{
	if(ActorAttributes)
	{
		ActorAttributes->LevelAttributes.Level = level;
	}
	

}

INT AMMOActor::GetActorLevel( )
{
	if(ActorAttributes)
	{
		return ActorAttributes->LevelAttributes.Level;
	}
	return 0;
}
FLOAT AMMOActor::GetGridRadius()
{
	if(BodySize<=E_body_1x1)
	{
		return 128.0f;
	}
	else if(BodySize>=E_body_3x3)
	{
		return 128.0f*2.0f;
	}
	else
	{
		return 128.0f;
	}
}

void AMMOActor::RefreshAvatar()
{
	//add by jimzhang 2012.7.25
	if(ItemManager)
	{
		ItemManager->Reset();
	}
	else
	{
		warnf(NAME_Error, TEXT("[AMMOActor::RefreshAvatar] ItemManager is NULL:%s"), *GetClass()->GetName());
	}
	if(UseingAvatar )
	{
		Mesh->SetSkeletalMesh(UseingAvatar->SkeletalMesh);
		bMainSkelChanged = true;
		if( Mesh->AnimTreeTemplate != UseingAvatar->AnimationTree )
			Mesh->SetAnimTreeTemplate(UseingAvatar->AnimationTree);
		UseingAvatar->DynamicLoadPhysicAsset();
		Mesh->SetPhysicsAsset(UseingAvatar->PhysicAsset);
		if( UseingAvatar->OverrideLightingChannels )
			Mesh->SetLightingChannels( UseingAvatar->LightingChannels );
		Mesh->SetLightEnvironment( LightEnvironment );
		if( UseingAvatar->UseSHLighting )
			LightEnvironment->bSynthesizeSHLight = TRUE;	// for NPCs in rooms
		//BodyType = UseingAvatar->BodyType;
		if(ItemManager)
		{
			ItemManager->SetDefaultMeshes(UseingAvatar, ServerID);
			ItemManager->UpdateOwnerData();
		}

		this->BodyType = UseingAvatar->BodyType;
		this->SoundMaterialType = UseingAvatar->TargetArmorMatType;

		RefreshCollision();

		if (IsMainPlayer())
		{
			EnableSkeletalControlFootPlacement(TRUE);
		}
		else
		{
			EnableSkeletalControlFootPlacement(FALSE);
		}
		

		UseingAvatar->eventOnExecute(this);
	}

	// replace materials if there is any
	for( INT i = 0; i < Mesh->Materials.Num(); ++i )
	{	// clear to null		
		Mesh->SetMaterial( i, NULL );
	}

	if( UseingAvatar )
	{
		for( INT i = 0; i < UseingAvatar->Materials.Num(); ++i )
		{	// replace materials
			if(UseingAvatar->Materials(i))
			{
				Mesh->SetMaterial( i, UseingAvatar->Materials(i) );
			}
		}
	}
}
//------------------------------------------------------------------------

UBOOL AMMOActor::ActorIsMoving()
{
	if (Physics == PHYS_Walking && Velocity.Size2D() > 0.0001)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void AMMOActor::EnableSkeletalControlFootPlacement(UBOOL bOn)
{
	//bUseSkeletalControlFootPlacement = bOn;

	if (Mesh && Mesh->Animations)
	{
		UAnimTree* Tree = Cast<UAnimTree>(Mesh->Animations);
		if (Tree)
		{
			for (INT i = 0; i < Tree->SkelControlLists.Num(); ++i)
			{
				USkelControlBase* Control = Tree->SkelControlLists(i).ControlHead;
				if (Control && Control->IsA(USkelControlFootPlacement::StaticClass()))
				{
					Control->SetSkelControlStrength((bOn? 1.0 : 0.f), 0.f);
				}
			}
		}
	}
}


void AMMOActor::RotateToMosuePoint()
{
	FVector NewVec = MainController->GetFixedMouseHitWorldLocation() - Location;
	NewVec.Z = 0;
	NewVec = NewVec.SafeNormal();
	FRotator NewRot = NewVec.Rotation();
	NewRot.Clamp();
	FLOAT deltaf = Abs(Abs(NewRot.Yaw) - Abs(Rotation.Yaw)) / RotateSpeed;
	LockDesiredRotation(false, true);
	SetDesiredRotation(NewRot, false, false, deltaf, true);

}
//------------------------------------------------------------------------
INT AMMOActor::GetActorAttributeValue(INT AttrId)
{
	if( ActorAttributes && AttrId > 0 && AttrId < ActorAttributes->AttributeList.Num() )
	{
		return ActorAttributes->AttributeList(AttrId) ;
	}
	return 0;
}
//------------------------------------------------------------------------
INT AMMOActor::GetHeroAttributeValue(INT AttrId)
{
	if( HeroAttributes && AttrId > 0 && AttrId < HeroAttributes->AttributeList.Num() )
	{
		return HeroAttributes->AttributeList(AttrId) ;
	}
	return 0;
}

UBOOL AMMOActor::CanUseAnyAbility()
{
	//if (ActorAttributes && ActorAttributes->MountID>0)
	//{
	//	return FALSE;
	//}
	if (eventIsDead())
	{
		return FALSE;
	}
	if (bStun)
	{
		return FALSE;
	}
	if(eventIsStallState()==TRUE)
	{
		return FALSE;
	}

	return TRUE;
}

UBOOL AMMOActor::CanMoveInAdvance()
{
	UBOOL bMove = TRUE;

	if (IsMainPlayer() && ((AMMOPlayer*)this)->IsInMatinee)
	{
		return FALSE;
	}

	if(eventIsStallState()==TRUE)
	{
		return FALSE;
	}

	if (bDead)
	{
		bMove = FALSE;
	}

	if (bFreeze)
	{
		bMove = FALSE;
	}

	if (bStun)
	{
		bMove = FALSE;
	}

	if (PassiveMoveType != PassiveMoveType_None)
	{
		bMove = FALSE;
	}

	return bMove;
}

UBOOL AMMOActor::CanMove()
{
	UBOOL bMove = FALSE;

	if (IsMainPlayer() && ((AMMOPlayer*)this)->IsInMatinee)
	{
		return FALSE;
	}

	if(eventIsStallState()==TRUE)
	{
		return FALSE;
	}


	if( CurrentAbilityState == EAS_Spell && CurrentAbilityState == EAS_Requested)
	{
		bMove = CanMoveInSpellTime;
	}
	else if(CurrentAbilityState == EAS_Cast)
	{
		bMove = CanMoveInCastTime;
	}
	else
	{
		bMove = TRUE;
	}

	if (bDead)
	{
		bMove = FALSE;
	}

	if (bFreeze)
	{
		bMove = FALSE;
	}

	if (bStun)
	{
		bMove = FALSE;
	}

	if (PassiveMoveType != PassiveMoveType_None)
	{
		bMove = FALSE;
	}

	return bMove;
}

//=====================================================================================
//Slot位置上装配技能
void AMMOActor::EquipSpell( int hotKeySlotId, UMMOAbility* newAbility )
{
	if( newAbility == NULL ){
		return;
	}
	newAbility->SlotHotKeyID = -1;

	INT newAbilityMainID = newAbility->eventGetAbilityId();
	INT newRuneSlotID = newAbility->GetRuneSlot();
	INT newAbilityLevel = newAbility->SpellCommonAttribute.Level;

	INT foundAbilityCnt = 0;
	INT maxAbilityCnt = Abilities.Num();
	INT maxFoundAbilityCnt = UCONST_MaxSubRuneNum + 1;//maxAbilityCnt;
	UMMOAbility* tAbility = NULL;
	UBOOL bEquippedOK = FALSE;
	for( INT j = 0; j < maxAbilityCnt; ++j ){
		tAbility = Abilities( j );
		if( tAbility == NULL ){
			continue;
		}

		INT curAbilityRuneSlotID = tAbility->GetRuneSlot();
		if ( newAbilityMainID == tAbility->SpellCommonAttribute.Id ){
			tAbility->SlotHotKeyID = -1;

			////如果是主技能，设置当前符文(0为自己)
			//if( tAbility->MainSkill != NULL ){
			//	//tAbility->MainSkill->SetRuneSlot( newRuneSlotID );
			//	maxFoundAbilityCnt = tAbility->MainSkill->SubRuneList.Num() + 1;//主技能算一个
			//}

			//如果找到这个符文技能
			if( newRuneSlotID == curAbilityRuneSlotID ){
				INT curAbilityLevel = tAbility->SpellCommonAttribute.Level;
				if( newAbilityLevel != curAbilityLevel ){
					//不是同一个技能(等级不同)，替换
					Abilities(j) = newAbility;
					tAbility = newAbility;
				}

				//
				tAbility->SlotHotKeyID = hotKeySlotId;

				bEquippedOK = TRUE;
			}

			//技能包括主技能一共那么多个
			++foundAbilityCnt;
			if( foundAbilityCnt >= maxFoundAbilityCnt ){
				break;
			}
		}
	}

	//子技能可能没加入，尝试载入
	if( bEquippedOK == FALSE && ( newRuneSlotID > 0 && foundAbilityCnt > 0 ) ){
		//UINT key = MMOSkillManager::CombinateAbilityID( newAbilityMainID, newAbilityLevel, newRuneSlotID );
		//UMMOAbility* loadedAbility = MMOSkillManager::getSingleton().FindAbilityTemplate(key);
		newAbility->SlotHotKeyID = hotKeySlotId;
		AddAbility( newAbility );
	}
}

//====================================================================
UMMOAbility* AMMOActor::GetEquipedAbilityById( INT mainAbilityID )
{
	INT maxAbilityCnt = Abilities.Num();
	UMMOAbility* tAbility = NULL;

	for( INT j = 0; j < maxAbilityCnt; ++j ){
		tAbility = Abilities( j );
		if( tAbility == NULL ){
			continue;
		}

		if ( mainAbilityID == tAbility->SpellCommonAttribute.Id ){
			if( tAbility->SlotHotKeyID >= 0 ){
				return tAbility;
			}
		}
	}

	return NULL;
}
//====================================================================
void AMMOActor::AddAbility(UMMOAbility *newAbility)
{
	if( newAbility == NULL ){
		return;
	}

	//
	UBOOL added = FALSE;
	INT maxAbilityCnt = Abilities.Num();
	UMMOAbility* tAbility = NULL;

	for( INT j = 0; j < maxAbilityCnt; ++j ){
		tAbility = Abilities( j );
		if( tAbility == NULL ){
			continue;
		}

		if( newAbility->SpellCommonAttribute.Id == tAbility->SpellCommonAttribute.Id && tAbility->GetRuneSlot() == 0 ){
			Abilities.InsertItem( newAbility, j );
			added = TRUE;
			break;
		}
	}

	if( added == FALSE ){
		Abilities.AddItem( newAbility );
	}

	if( newAbility->ClientCommonAttribute.bNormalAttack ){
		NormalAttackAbilities.AddItem( newAbility );
	}
}

//-------------------------------------------------------------------------------
void AMMOActor::DelAbility( INT mainAbilityID, INT runeSlotID )
{
	INT maxAbilityCnt = Abilities.Num();
	UMMOAbility* tAbility = NULL;

	//
	UBOOL isDelNormalAttcAbility = FALSE;
	for( INT i = 0; i < maxAbilityCnt; ++i ){
		tAbility = Abilities( i );
		if( tAbility == NULL ){
			continue;
		}

		if( mainAbilityID == tAbility->SpellCommonAttribute.Id && runeSlotID == tAbility->SpellCommonAttribute.RuneSlotID ){
			if( tAbility->ClientCommonAttribute.bNormalAttack ){
				isDelNormalAttcAbility = TRUE;
			}

			//清理技能动态数据
			tAbility->SlotHotKeyID = -1;

			//删除
			Abilities.Remove( i );
			break;
		}
	}

	//
	if( isDelNormalAttcAbility ){
		INT maxNormalAttackAbilityCnt = NormalAttackAbilities.Num();
		for( INT j = 0; j < maxNormalAttackAbilityCnt; ++j ){
			tAbility = NormalAttackAbilities( j );
			if( tAbility == NULL ){
				continue;
			}
			if( mainAbilityID == tAbility->SpellCommonAttribute.Id && runeSlotID == tAbility->SpellCommonAttribute.RuneSlotID ){
				NormalAttackAbilities.Remove( j );
				break;
			}
		}
	}//end of if
}
//--------------------------------------------------------------------
void AMMOActor::ResetAbilities()
{
	INT maxAbilityCnt = Abilities.Num();
	for( INT i = 0; i < maxAbilityCnt; ++i ){
		UMMOAbility* tAbility = Abilities( i );
		if( tAbility == NULL ){
			continue;
		}

		tAbility->SlotHotKeyID = -1;
	}

	Abilities.Empty();
	NormalAttackAbilities.Empty();
}

//====================================================================
UMMOAbility* AMMOActor::GetMainAbilityById( INT mainAbilityID )
{
	return GetAbilityById( mainAbilityID, 0 );
}

//====================================================================
UMMOAbility* AMMOActor::GetAbilityById( INT mainAbilityID, INT abilityRuneID )
{
	INT maxAbilityCnt = Abilities.Num();
	UMMOAbility* tAbility = NULL;

	for( INT j = 0; j < maxAbilityCnt; ++j ){
		tAbility = Abilities( j );
		if( tAbility == NULL ){
			continue;
		}

		INT curAbilityRuneSlotID = tAbility->GetRuneSlot();
		if ( mainAbilityID == tAbility->SpellCommonAttribute.Id && abilityRuneID == curAbilityRuneSlotID ){
			return tAbility;
		}
	}

	return NULL;
}

//====================================================================
void AMMOActor::UpdateAbility( INT mainAbilityID, INT abilityRuneID, INT level )
{
	INT maxAbilityCnt = Abilities.Num();
	UMMOAbility* tAbility = NULL;

	for( INT j = 0; j < maxAbilityCnt; ++j ){
		tAbility = Abilities( j );
		if( tAbility == NULL ){
			continue;
		}

		INT curAbilityRuneSlotID = tAbility->GetRuneSlot();
		if( mainAbilityID == tAbility->SpellCommonAttribute.Id && abilityRuneID == curAbilityRuneSlotID ){
			UINT Key = MMOSkillManager::CombinateAbilityID( mainAbilityID, level, abilityRuneID );
			UMMOAbility* newAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );

			if( NULL != newAbility && ( tAbility != newAbility ) ){
				//传递是否装备此技能
				newAbility->SlotHotKeyID = tAbility->SlotHotKeyID;
				//据说技能升级CD不消
				FCoolDown &newAbilityCD = newAbility->CoolDownInfo;
				FCoolDown &oldAbilityCD = tAbility->CoolDownInfo;

				newAbility->bCoolDown = tAbility->bCoolDown;
				newAbilityCD.GroupTimeLeft = oldAbilityCD.GroupTimeLeft;
				newAbilityCD.GroupTimeMax = oldAbilityCD.GroupTimeMax;
				newAbilityCD.SpellTimeLeft = oldAbilityCD.SpellTimeLeft;
				newAbilityCD.SpellTimeMax = oldAbilityCD.SpellTimeMax;
				//
				Abilities(j) = newAbility;

				//clear old
				tAbility->SlotHotKeyID = -1;

				//不会又装配低级技能吧，那这里就不对了
				tAbility->bCoolDown = FALSE;
				oldAbilityCD.GroupTimeLeft = 0;
				oldAbilityCD.GroupTimeMax = 0;
				oldAbilityCD.SpellTimeLeft = 0;
				oldAbilityCD.SpellTimeMax = 0;
			}

			return;
		}
	}
}

INT AMMOActor::PlayAction(class UAbilityComponent* InComponent)
{
	if (!InComponent)
	{
		return 0;
	}

	if (InComponent->StopType != ACBT_StopWhenConstDurationPass)
	{
		InComponent->StopType = ACBT_StopWhenConstDurationPass;
		InComponent->Duration = 10.f;
	}

	BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(InComponent, FALSE, ServerID, ServerID, 0, Location, 1.0);
	if (NewNode)
	{
		EffectProcessor::GetSingleton().AddActiveComponent(NewNode);
		NewNode->OnAction(TRUE);
		return NewNode->GetId();
	}

	return 0;
}

TArray<INT> AMMOActor::PlayActions(const TArray<class UAbilityComponent*>& InComponents)
{
	TArray<INT> Ret;
	for (INT i = 0; i < InComponents.Num(); ++i)
	{
		INT Value = PlayAction(InComponents(i));
		if (Value != 0)
		{
			Ret.AddItem(Value);
		}
	}

	return Ret;
}

void AMMOActor::RebuildAllMeshMaterialList()
{
	TArray<UMeshComponent*> ToProcessComponents;

	for (INT i = 0; i < Components.Num(); ++i)
	{
		if (Components(i) == NULL)
		{
			continue;
		}

		if (Components(i)->IsA(UMeshComponent::StaticClass()) == FALSE)
		{
			continue;
		}

		ToProcessComponents.AddItem((UMeshComponent*)Components(i));

		if (Components(i)->IsA(USkeletalMeshComponent::StaticClass()))
		{
			USkeletalMeshComponent* smc = (USkeletalMeshComponent*)(Components(i));
			for (INT j = 0; j < smc->Attachments.Num(); ++j)
			{
				if (smc->Attachments(j).Component == NULL || smc->Attachments(j).Component->IsA(UMeshComponent::StaticClass()) == FALSE)
				{
					continue;
				}

				ToProcessComponents.AddItem((UMeshComponent*)smc->Attachments(j).Component);
			}
		}
	}

	AArmorActor* Weapon1 = Cast<AArmorActor>(GetEquip(EBMT_Weapon));
	if (Weapon1)
	{
		ToProcessComponents.AddItem(Weapon1->MainMesh);
	}
	AArmorActor* Weapon2 = Cast<AArmorActor>(GetEquip(EBMT_Weapon2));
	if (Weapon2)
	{
		ToProcessComponents.AddItem(Weapon2->MainMesh);
	}

	for (INT i = 0; i < ToProcessComponents.Num(); ++i)
	{
		if (ToProcessComponents(i))
		{
			ToProcessComponents(i)->RebulidMeshMaterialList();
		}
	}
}

void AMMOActor::NotifyRefreshEquipLevel(BYTE InSlot)
{
	if (!GpDataManager)
	{
		return;
	}

	INT NewLevel = EquipSlotManager::getSingleton().GetMyEquipSlotLevel(InSlot);

	if (InSlot == E_NES_MAIN_HAND)
	{
		ClearEquipEffects();

		BYTE CurrentWeaponType = GetWeaponType();
		for (INT i = 0; i < GpDataManager->WeaponLevelUpEffects.Num(); ++i)
		{
			if (CurrentWeaponType == GpDataManager->WeaponLevelUpEffects(i).WeaponType)
			{
				for (INT j = 0; j < GpDataManager->WeaponLevelUpEffects(i).WeaponEffects.Num(); ++j)
				{
					if (GpDataManager->WeaponLevelUpEffects(i).WeaponEffects(j).Level <= NewLevel)
					{
						if ((GpDataManager->WeaponLevelUpEffects(i).WeaponEffects.Num() == j + 1) || GpDataManager->WeaponLevelUpEffects(i).WeaponEffects(j + 1).Level > NewLevel)
						{
							WeaponLevelUpEffectIds = PlayActions(GpDataManager->WeaponLevelUpEffects(i).WeaponEffects(j).Effects);
							return;
						}
					}
				}
			}
		}
	}
	else
	{
		for (INT i = 0; i < GpDataManager->EquipLevelUpEffects.Num(); ++i)
		{
			if (GpDataManager->EquipLevelUpEffects(i).Level <= NewLevel)
			{
				if ((GpDataManager->EquipLevelUpEffects.Num() == i + 1) || GpDataManager->EquipLevelUpEffects(i + 1).Level > NewLevel)
				{
					PlayActions(GpDataManager->EquipLevelUpEffects(i).TheOtherEquipLevelUpEffects);
					return;
				}
			}
		}

	}
}

void AMMOActor::NotifyRefreshEquipLevelByIndex(INT InIndex)
{					 
	InIndex -= 1;
	ClearEquipEffects();

	BYTE CurrentWeaponType = GetWeaponType();

	for (INT i = 0; i < GpDataManager->WeaponLevelUpEffects.Num(); ++i)
	{
		if (CurrentWeaponType == GpDataManager->WeaponLevelUpEffects(i).WeaponType)
		{
			if (InIndex > 0 && InIndex < GpDataManager->WeaponLevelUpEffects(i).WeaponEffects.Num())
			{
				WeaponLevelUpEffectIds = PlayActions(GpDataManager->WeaponLevelUpEffects(i).WeaponEffects(InIndex).Effects);
			}
			else
			{
				debugf(TEXT("can not find weapon level up effect by weaponType=%d index=%d"), CurrentWeaponType, InIndex);
			}
		}
	}
}

void AMMOActor::ClearEquipEffects()
{
	//// first clear old effects
	//for (INT i = 0; i < WeaponLevelUpEffectIds.Num(); ++i)
	//{
	//	EffectProcessor::GetSingleton().DeleteActiveComponent(WeaponLevelUpEffectIds(i));
	//}
	//WeaponLevelUpEffectIds.Empty();

	//for (INT i = 0; i < CanNotBeGCEmitters.Num(); ++i)
	//{
	//	if (CanNotBeGCEmitters(i))
	//	{
	//		GWorld->DestroyActor(CanNotBeGCEmitters(i));
	//	}
	//}

	//CanNotBeGCEmitters.Empty();
}



void AMMOActor::PlayActionById(INT ActionId)
{
}

void AMMOActor::PlayBloodEffect(BYTE InResultType,UBOOL bCauseDead)
{
	if (InResultType != DRT_Result6 && InResultType != DRT_Result7)
	{
		return;
	}

	if (!GpDataManager || !GpDataManager->DamageTemplateData)
	{
		return;
	}

	BYTE NormalBET = BET_Type2; // default is red blood
	BYTE DeadBET = BET_Type2; // default is red blood

	ANPC* npc = Cast<ANPC>(this);
	if (npc && npc->Template)
	{
		NormalBET = npc->Template->NormalHitBloodEffectType;
		DeadBET = npc->Template->DeadHitBloodEffectType;
	}

	if (bCauseDead)
	{
		if (DeadBET != BET_Type0)
		{
			for (INT i = 0; i < GpDataManager->DamageTemplateData->HitBloodEffect.DeadHitEffects.Num(); ++i)
			{
				if (DeadBET == GpDataManager->DamageTemplateData->HitBloodEffect.DeadHitEffects(i).BloodEffectType)
				{
					PlayActions(GpDataManager->DamageTemplateData->HitBloodEffect.DeadHitEffects(i).HitEffects);
					break;
				}
			}
		}
	}
	else
	{
		if (NormalBET != BET_Type0)
		{
			if (InResultType == DRT_Result6)
			{
				FLOAT Randf = FLOAT(RandHelper(100)) / 100.f;
				if (Randf < GpDataManager->DamageTemplateData->HitBloodEffect.NormalBloodPlayRate)
				{
					for (INT i = 0; i < GpDataManager->DamageTemplateData->HitBloodEffect.NormalHitEffects.Num(); ++i)
					{
						if (NormalBET == GpDataManager->DamageTemplateData->HitBloodEffect.NormalHitEffects(i).BloodEffectType)
						{
							PlayActions(GpDataManager->DamageTemplateData->HitBloodEffect.NormalHitEffects(i).HitEffects);
							break;
						}
					}
				}
			}
			else //if (InResultType == DRT_Result7)
			{
				for (INT i = 0; i < GpDataManager->DamageTemplateData->HitBloodEffect.CritialHitEffects.Num(); ++i)
				{
					if (NormalBET == GpDataManager->DamageTemplateData->HitBloodEffect.CritialHitEffects(i).BloodEffectType)
					{
						PlayActions(GpDataManager->DamageTemplateData->HitBloodEffect.CritialHitEffects(i).HitEffects);
						break;
					}
				}

			}
		}
	}

}

void AMMOActor::ProcessActionsDelay(TArray<class UAbilityComponent*>& OutComponents, FLOAT Delay)
{
	return;

	for (INT i = 0; i < OutComponents.Num(); ++ i)
	{
		UAbilityComponent* Component = OutComponents(i);
		if (Component && Component->IsA(UAbilityActionsComponent::StaticClass()))
		{
			UAbilityActionsComponent* ActionsComponent = Cast<UAbilityActionsComponent>(Component);
			for (INT j = 0; j < ActionsComponent->ActionsComponentList.Num(); ++ j)
			{
				FComponentNode* ComponentNode = &(ActionsComponent->ActionsComponentList(j));
				if (ComponentNode)
				{
					ComponentNode->StartTime += Delay;
				}
			}
		}
	}
}

UBOOL AMMOActor::PlaySpecialDying(FLOAT Delay)
{
	if (SpecialDyingActions.Num() == 0)
	{
		return FALSE;
	}
	else
	{
		ProcessActionsDelay(SpecialDyingActions, Delay);
		PlayActions(SpecialDyingActions);
		return TRUE;
	}
}
UBOOL AMMOActor::PlayStateDying(FLOAT Delay)
{
	if (IsA(AMMOPlayer::StaticClass()))
	{
		return FALSE;
	}
	else
	{
		for (INT i = 0; i < GpDataManager->DamageTemplateData->StateDeaths.Num(); ++i)
		{
			BuffDataMap* BuffMap = MMOFacade::getSingleton().GetMMOBuffManager()->GetBuffMap(ServerID);
			if (BuffMap)
			{
				for (STLInt2BuffDataMap::iterator pos = BuffMap->Buffs.begin(); pos != BuffMap->Buffs.end(); ++pos)
				{
					if (pos->second && pos->second->Buff && pos->second->Buff->Type == GpDataManager->DamageTemplateData->StateDeaths(i).BuffType)
					{
						ProcessActionsDelay(GpDataManager->DamageTemplateData->StateDeaths(i).EffectList, Delay);
						PlayActions(GpDataManager->DamageTemplateData->StateDeaths(i).EffectList);
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}
UBOOL AMMOActor::PlayCommonDying(BYTE InStyle, BYTE InElementaryType, FLOAT Delay)
{
	if (IsA(AMMOPlayer::StaticClass()))
	{
		InStyle = DS_Anim;
	}
	else if (IsA(ANPC::StaticClass()))
	{
		// 整体效果等级在3级以下自动关闭物理动画，0是自定义
		if (GSystemSettings.AutoFPSLevel <= M_SCALE_DOWN_ABILITY_POINT_LIGHT)
		{
			if (InStyle == DS_Ragdoll)
			{
				InStyle = DS_Anim;
			}
		}
	}

	TArray<UAbilityComponent*> Actions;
	UBOOL bFindSpecial = FALSE;
	UBOOL bFind = FALSE;
	if (InStyle == DS_Explode && UseingAvatar && UseingAvatar->OverrideExplosionEffects.Num() > 0)
	{
		for (INT i = 0; i < UseingAvatar->OverrideExplosionEffects.Num(); ++i)
		{
			if (UseingAvatar->OverrideExplosionEffects(i).Type == InElementaryType)
			{
				bFindSpecial = TRUE;
				bFind = TRUE;
				Actions = UseingAvatar->OverrideExplosionEffects(i).ExplosionComponents;
				break;
			}
		}
	}

	if (!bFindSpecial)
	{
		bFind = GpDataManager->GetCommonDeath(InStyle, InElementaryType, Actions);
	}

	if (bFind)
	{
		ProcessActionsDelay(Actions, Delay);
		PlayActions(Actions);
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------
void AMMOActor::SetActorAttributeValue(INT AttrID,INT Value)
{
	if( ActorAttributes && AttrID > 0 && AttrID < ActorAttributes->AttributeList.Num() )
	{
		ActorAttributes->AttributeList(AttrID) = Value;

		//switch (AttrID)
		//{
		//case catMoveSpeed:
		//	GroundSpeed = ConvertToClientSpeed(Value);
		//	UpdateMoveSpeed(GroundSpeed);
		//	break;	
		//}		
	}
}

//------------------------------------------------------------------------
void AMMOActor::SetHeroAttributeValue(INT AttrID,INT Value)
{
	if( HeroAttributes && AttrID > 0 && AttrID < HeroAttributes->AttributeList.Num() )
	{
		HeroAttributes->AttributeList(AttrID) = Value;
	}
}


FLOAT AMMOActor::GetMp()
{
	if (ActorAttributes)
	{
		return ActorAttributes->MP;
	}
	else
	{
		return 0.f;
	}
}

FLOAT AMMOActor::GetWoundAnimThresholdValue()
{
	if (GpDataManager)
	{
		return GpDataManager->WoundAnimCanBeResetThreshold;
	}
	return 0.5f;
}

//add by jim 
void AMMOActor::TouchNative(class AActor* Other,class UPrimitiveComponent* OtherComp,FVector HitLocation,FVector HitNormal)
{
	if( !GIsEditor && IsMainPlayer() )
	{
		if(!MainController)
		{
			MainController = MMOFacade::getSingleton().GetMainController();
		}

		if(Other && Other->IsA( AMMOActor::StaticClass() ) )
		{
			UBOOL bHave = FALSE;
			for (TArrayNoInit<class AMMOActor*>::TIterator i(onTouchActor); i; ++i)
			{
				AMMOActor *p = (*i);
				if(p==Other){ bHave=TRUE; break; }
			}

			if(bHave==FALSE)
			{
				if(MainController->nPlayerColisionType == 1)
				{
					FVector v1 = this->Velocity;
					FVector v2 = Other->Location - this->Location;
					v1.Normalize();
					v2.Normalize();
					FLOAT buf = v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z;
					if(buf > MainController->fPlayerColisionRatio)//阻挡范围之内
					{
						MainController->StopMove(true);
						onTouchActor.AddItem((AMMOActor *)Other);
					}
				}
			}
		}
		else if( Other && Other->IsA( AMMOApexDestructibleActor::StaticClass() ) )
		{
			if( Other->CollisionType == COLLIDE_BlockAll )
			{
				//Other->IsEncroacher()
				SetLocation( LastLocation );
				MainController->StopMove(true);
			}
		}
	}
}

//add by jim 
void AMMOActor::UnTouchNative(class AActor* Other)
{

	//if( IsMainPlayer() )
	//{
	//	if( Other->IsA( AMMOActor::StaticClass() ) )
	//	{

	//	}
	//}
}


void AMMOActor::PlayWeaponUseSound(INT SoundIdx)
{
	EWeaponAppearenceType type = EWeaponAppearenceType(GetWeaponAppearenceType());
	USoundCue* ToPlay = GpDataManager->GetWeaponUseSound(type, SoundIdx);
	if (ToPlay)
	{
		PlaySound(ToPlay);
	}
}


void AMMOActor::PlayWeaponHitSound(BYTE HitType,BYTE MaterialType)
{
	USoundCue* ToPlay = GpDataManager->FindHitSound(HitType, MaterialType);
	if (ToPlay)
	{
		PlaySound(ToPlay);
	}
}

void AMMOActor::HideWeapon(UBOOL bHide)
{
	AArmorActor* weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon));
	if (weapon)
	{
		weapon->SetVisible(!bHide);
	}

	weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon2));
	if (weapon)
	{
		weapon->SetVisible(!bHide);
	}																		
}

void AMMOActor::SetBodyPartVisible(BYTE InType,UBOOL bVisible)
{
	AArmorActor* aaa = Cast<AArmorActor>(GetEquip(InType));
	if (aaa)
	{
		aaa->SetVisible(bVisible);
	}
}


//------------------------------------------------------------------------
void AMMOActor::InitItems()
{
	if( ItemManager != NULL )
	{

	}
	else
	{
		ItemManager = Cast<UItemManager>(StaticConstructObject(UItemManager::StaticClass(),
			GetTransientPackage(), NAME_None, 0));

		ItemManager->Init(this);
	}
}
//------------------------------------------------------------------------
void AMMOActor::CloneFrom( const AMMOActor* BaseActor )
{
	if( BaseActor == NULL )
		return;

	UseingAvatar = BaseActor->UseingAvatar;

	if( ItemManager )
		ItemManager->Reset();

	Mesh->Materials.Empty();

	if( BaseActor->UseingAvatar )
	{
		Mesh->SetSkeletalMesh( BaseActor->UseingAvatar->SkeletalMesh );
		bMainSkelChanged = true;
		Mesh->SetAnimTreeTemplate( BaseActor->UseingAvatar->AnimationTree );
		Mesh->SetPhysicsAsset( BaseActor->UseingAvatar->PhysicAsset );
		Mesh->AnimSets.Empty();
		if( ItemManager )
		{
			ItemManager->UseMergedMesh = BaseActor->UseingAvatar->UseMergedMesh;
			ItemManager->SetDefaultMeshes( BaseActor->UseingAvatar, ServerID);
			ItemManager->UpdateOwnerData( );
		}
		UseingAvatar = BaseActor->UseingAvatar;

		RefreshCollision();

		for( INT i = 0; i < BaseActor->UseingAvatar->Materials.Num(); ++i )
		{	// replace materials
			if( BaseActor->UseingAvatar->Materials(i) )
			{
				Mesh->SetMaterial( i, BaseActor->UseingAvatar->Materials(i) );
			}
		}
	}
	else if( BaseActor->ActorAttributes )
	{
		eventSetMesh( BaseActor->ActorAttributes->ClassType, BaseActor->ActorAttributes->GenderType );
		// take on equips
	}
	else if( BaseActor->Mesh )
	{
		Mesh->SetSkeletalMesh( BaseActor->Mesh->SkeletalMesh );
		bMainSkelChanged = true;
		Mesh->SetAnimTreeTemplate( BaseActor->Mesh->AnimTreeTemplate );
		Mesh->SetPhysicsAsset( BaseActor->Mesh->PhysicsAsset );
	}

	//if( RootMaterialController )
	//{
	//	RootMaterialController->eventInit( Mesh );
	//}
	eventInitAnimNodes();
	RefreshAnimSet();
	RefreshCollision();
	SetIdleAnim(PawnAnimInfo.IdleAnim);

	SetServerLocation(BaseActor->ServerLocation);	// zhaojun@snda, 2015-3-3
	ReCalGridBlockRadius();
}




void AMMOActor::SetNewElementary(INT InCreatorId,BYTE InType)
{
	ElementaryType = InType;
	ElementaryCreatorId = InCreatorId;
}

void AMMOActor::RemoveElementary(INT InCreatorId)
{
	if (InCreatorId == ElementaryCreatorId)
	{
		ElementaryType = 0;
	}
}


//------------------------------------------------------------------------
FString AMMOActor::GetDebugDescription()
{
	FString resName;
	if(Mesh)
	{
		if(Mesh->SkeletalMesh)
		{
			if(Mesh->SkeletalMesh->GetOuter())
			{
				resName = Mesh->SkeletalMesh->GetOuter()->GetName();
			}

			resName += FString(TEXT(".")) + Mesh->SkeletalMesh->GetName();
		}
	}
	return FString::Printf(TEXT("%s[%s]"), *GetName(),  *resName);
}

void AMMOActor::ResetMoveTo(BYTE InMoveState, FLOAT speed, FVector Dst, FVector Src, FRotator Dir)
{
	if (speed < 0.0001)
	{
		warnf(TEXT("%d ResetMoveTo : speed can not be nearly to zero"), ServerID);
		return;
	}

	//if (InMoveState == CMS_STAND || InMoveState == CMS_DEST_MOVE)
	{
		SetDesiredRotation(Dir);
		FVector ToDst = Dst - Location;
		if (ToDst.SizeSquared2D() <= 32.f * 32.f)
		{
			//debugf(TEXT("ResetMoveTo, but no so far(%f), skip"), ToDst.Size2D());
			return;
		}

		MoveTo(Dst, speed, TRUE);
	}
	//else
	//{
	//	FVector ToDst = Dst - Location;
	//	if (ToDst.SizeSquared2D() <= 32.f * 32.f)
	//	{
	//		//debugf(TEXT("ResetMoveTo, but no so far(%f), skip"), ToDst.Size2D());
	//		return;
	//	}
	//	FLOAT Duration = ToDst.Size2D() / speed;
	//	Dst.Z = Location.Z;
	//	Dst.Z = GetValidZValue(Dst);
	//	Dst.Z += GetCollisionHeight();
	//	DashTo(Dst, Duration);
	//}
}

void AMMOActor::UpdateMoveSpeed(FLOAT NewSpeed)
{
	if (isMoving && Velocity.IsNearlyZero() == FALSE)
	{
		FVector Dir = Destination - Location;
		Dir.Z = 0;
		FLOAT fValueOffset = Dir.Size2D();

		if (fValueOffset <= 1.f)
		{
			return;
		}

		Dir = Dir.SafeNormal2D();

		//FLOAT MoveRate = GetMoveAnimRate(NewSpeed);

		//SetMoveAnimRate(MoveRate);

		Velocity = Dir * NewSpeed;
	}
}


FLOAT AMMOActor::GetMoveAnimRate(INT NewSpeed)
{
	FLOAT Ret = 1.0;
	UAvatarData* ToUseAvatar = UseingAvatar;
	if (MeshChangeAvatarData)
	{
		ToUseAvatar = MeshChangeAvatarData;
	}
	if (ToUseAvatar)
	{
		Ret = NewSpeed / ToUseAvatar->GetMoveSpeed(MoveType);
	}

	return Ret;
}

void AMMOActor::MoveTo(FVector Des,FLOAT Speed,UBOOL rotationToDes,FLOAT inMoveRateScale)
{
	FRotator Rot;
	FVector fValue;

	CurrentMoveSpeed = Speed;

	if(Physics != PHYS_Walking)
	{
		setPhysics(PHYS_Walking);
	}

	if(eventGetValidPoint(Des, fValue))
	{
		Des = fValue;
		Des.Z += GetCollisionHeight();
	}

	fValue = Des - Location;
	fValue.Z = 0;
	FLOAT fValueOffset = fValue.Size2D();

	if (fValueOffset <= 1.f)
	{
		Velocity = FVector::ZeroVector;
		Location.X = Des.X; // oscar@snda 停下来的话要确保是在正确的点
		Location.Y = Des.Y;
		return;
	}

	static FLOAT BlendTime = 0.05f;
	//BlendToLoops();
	if (!bMovingToMousePoint)
	{
		if (!IsInMeshChangeState())
		{
			if(ActorAT)ActorAT->SetActionLoopBlendTarget(0.f, BlendTime);

			if(MountInfo && MountInfo->ActorAT )MountInfo->ActorAT->SetActionLoopBlendTarget(0.f, BlendTime);


			// weapon animation
			UMMOActorAT* WeaponAT = GetWeaponEquipMeshAnimationController(0);
			if (WeaponAT)
			{
				WeaponAT->SetActionLoopBlendTarget(0.f, BlendTime);
			}
			WeaponAT = GetWeaponEquipMeshAnimationController(1);
			if (WeaponAT)
			{
				WeaponAT->SetActionLoopBlendTarget(0.f, BlendTime);
			}


			for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
			{
				if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT )
				{
					AttachedDestructableParts(i)->ActorAT->SetActionLoopBlendTarget(0.f, BlendTime);
				}
			}
		}
		else
		{
			if(MeshChangeAnimController)
			{
				MeshChangeAnimController->SetActionLoopBlendTarget(0.f, BlendTime);
			}
		}

	}


	isMoving = true;
	//warnf(TEXT("F[%d]------------------------AMMOActor::MoveTo: isMoving = true"),g_TotalFrameNum);


	fValue = fValue.SafeNormal2D();

	if(rotationToDes)
	{
		//if(fValueOffset>64.0f)
		{
			Rot = fValue.Rotation();//FRotator();
			LockDesiredRotation(false, true);
			SetDesiredRotation(Rot, false, true);
		}
	}
	Destination = Des;

	// maybe need to change move anim
	UpdateMoveType(Speed);

	FLOAT MoveRate = inMoveRateScale;// GetMoveAnimRate(Speed) * inMoveRateScale;

	SetMoveAnimRate(MoveRate);

	Velocity = fValue * Speed;

	FLOAT MoveTime = (Des - Location).Size2D() / Velocity.Size2D();
	//debugf(L"#char move time:%f", MoveTime);
	////用于防震荡，当目标点和出发点处于同一点的时候，需要一个震荡量
	//if(Velocity.IsNearlyZero() && MainController->Pawn == this  )
	//{
	//	Velocity = FVector(0.1,0.1,0.0);

	//}

}

void AMMOActor::RotatoTo(FRotator InRot,FLOAT InDuration)
{
	FLOAT RotateTime = InDuration;

	if (abs(RotateTime) < 1E-5)
		bInstantRotate = true;
	else
		bInstantRotate = false;
	//if (RotateTime < 0.64f)
	//	RotateTime = 0.64f;



	if (IsA(AMMOPlayer::StaticClass()))
	{
		//SetDesiredRotation(InRot, FALSE, TRUE, -1.0, TRUE);
		//SetRotation(InRot);

		//LockDesiredRotation(FALSE, TRUE);
		//SetDesiredRotation(InRot, FALSE, TRUE, 0.2, TRUE);
		//return;
	}

	FRotator Rot = InRot.Clamp() - Rotation.Clamp();
	Rot.MakeShortestRoute();
	//if (abs(Rot.Yaw) < 1820)
	//{
	//	SetDesiredRotation(InRot, FALSE, TRUE, -1.0, TRUE);
	//	SetRotation(InRot);
	//	return;

	//}

	if (UseingAvatar && RotateTime <= 0.000001f)
	{
		RotateTime = abs(Rot.Yaw) * 360.f / 65536.f;
		if (UseingAvatar->RotatoDegreePerSecond <= 0.f)
		{
			UseingAvatar->RotatoDegreePerSecond = 360;
		}
		RotateTime /= UseingAvatar->RotatoDegreePerSecond;
	}

	//if (IsA(AMMOPlayer::StaticClass()))
	//{
	//	LockDesiredRotation(FALSE, TRUE);
	//	SetDesiredRotation(InRot, FALSE, TRUE, 0.3f, TRUE);
	//}
	//else
	{
		LockDesiredRotation(FALSE, TRUE);
		SetDesiredRotation(InRot, FALSE, TRUE, RotateTime, TRUE);
	}
	// play turn anim
	//FRotator Rot = InRot.Clamp() - Rotation.Clamp();
	//Rot.MakeShortestRoute();
	UBOOL bTurnRight = FALSE;
	if (Rot.Yaw >= 0)
	{
		bTurnRight = TRUE;
	}

	FName ToPlayAnim = NAME_None;
	if (bTurnRight)
	{
		ToPlayAnim = PawnAnimInfo.TurnRightAnim;
	}
	else
	{
		ToPlayAnim = PawnAnimInfo.TurnLeftAnim;
	}

	if (UseingAvatar && Mesh)
	{
		UAnimSequence* ans = Mesh->FindAnimSequence(ToPlayAnim);
		if (!ans)
		{
			return;
		}

		UAbilityAnimationComponent* ToPlayAction = NULL;
		if (bTurnRight)
		{
			ToPlayAction = UseingAvatar->TurnRightAction;
		}
		else
		{
			ToPlayAction = UseingAvatar->TurnLeftAction;
		}

		check(ToPlayAction);

		AnimComponent* AC =  (AnimComponent*)(EffectProcessor::GetSingleton().GetActiveComponent(TurnActionId));
		if (AC && AC->bStopped == FALSE && AC->AnimData->Anims(0) == ToPlayAnim)
		{
			//debugf(TEXT("ChangeTime CurrentTime=%f NewDuration=%f"), AC->CurrentTime, RotateTime);
			AC->AnimData->Duration = RotateTime;
			AC->CurrentTime = 0.f;
		}
		else
		{
			ToPlayAction->Anims.Empty();
			ToPlayAction->Anims.AddItem(ToPlayAnim);
			ToPlayAction->Duration = RotateTime;
			ToPlayAction->bLoop = TRUE;
			TurnActionId = PlayAction(ToPlayAction);
			//debugf(TEXT("AddNewAction : %d"), TurnActionId);
		}
	}
}

#if !SHIPPING_PC_GAME
bool g_DebugMove = false;
#endif

bool g_EanbleSlopeCheck = false;
bool g_EnableMoveStepHeightCheck = true;

bool AMMOActor::ValidateMoveDest(const FVector& /*dest*/) const
{
	return true;
}

#if !SHIPPING_PC_GAME
//extern DWORD g_moveCycles;
#endif

void AMMOActor::moveDelta(FVector Delta)
{
	SCOPE_CYCLE_COUNTER(STAT_MoveDeltaTime);

	if (Delta.IsNearlyZero())
	{
		return;
	}

	// do the move
	AngularVelocity = FVector(0.f);

	FVector NewLocation = Location + Delta;
	if(!ValidateMoveDest(NewLocation))
	{
		StopMovement();
		eventStopped();
		return;
	}

	FLOAT MaxGridSize = FLOAT(GRID_WIDTH) / FLOAT(POINT_PER_CM);

	// 成功地移动了delta
	NewLocation.Z -= GetCollisionHeight();
	FVector StartPoint = NewLocation + FVector(0, 0, MaxGridSize * 1.5f); // 用1.5格子高去检查
	FVector EndPoint = NewLocation - FVector(0, 0, 768.0f); // 超过768就不可以走、不可飞
	//FCheckResult Hit(1.f);
	//GWorld->SingleLineCheck(Hit, this, EndPoint, StartPoint, TRACE_Blocking | TRACE_LevelGeometry | TRACE_Level | TRACE_Terrain, FVector(10, 10, 0));

	FVector HitLocation = NewLocation;
	FVector HitNormal = FVector(0, 0, 1);

	FMemMark Mark(GMainThreadMemStack);
	FCheckResult* Hit = GWorld->MultiLineCheck( GMainThreadMemStack, EndPoint, StartPoint, FVector(10, 10, 0), TRACE_Blocking | TRACE_LevelGeometry | TRACE_Level | TRACE_Terrain, this);
	while(Hit)
	{
		if (Hit->Actor && Hit->Actor->IsA(AStaticMeshActor::StaticClass()))
		{
			AStaticMeshActor* sma = (AStaticMeshActor*)(Hit->Actor);
			if (sma->bCanHidden)
			{
				Hit = Hit->GetNext();
			}
			else
			{
				HitLocation = Hit->Location;
				HitNormal = Hit->Normal;
				break;
			}
		}
		else
		{
			HitLocation = Hit->Location;
			HitNormal = Hit->Normal;
			break;
		}
	}

	//warnf(NAME_Log, TEXT("Hit:%s"), Hit ? *Hit->Actor->GetFullName() : TEXT(""));
	Mark.Pop();

	INT64 llSrcX = 0;
	INT64 llSrcY = 0;
	INT64 llSrcZ = 0;

	ZONE_DATA* pSrcZone = NULL;
	GRID_DATA* pSrcGrid = NULL;

	ZONE_DATA* pDestZone = NULL;
	GRID_DATA* pDestGrid = NULL;

	gServerGridWorld.ConvertPosClient2Server(Location + FVector(0, 0, 128), llSrcX, llSrcY, llSrcZ);
	gServerGridWorld.GetZoneAndGrid(llSrcX, llSrcY, llSrcZ, pSrcZone, pSrcGrid);

	gServerGridWorld.ConvertPosClient2Server(NewLocation + FVector(0, 0, 128), llSrcX, llSrcY, llSrcZ);
	gServerGridWorld.GetZoneAndGrid(llSrcX, llSrcY, llSrcZ, pDestZone, pDestGrid);


	FLOAT srcGridHeight = Location.Z;
	if(pSrcGrid)
	{
		srcGridHeight = pSrcGrid->uHighLayer * GRID_LAYER_HEIGHT/POINT_PER_CM;
	}

	FLOAT destGridHeight = NewLocation.Z;
	if(pDestGrid)
	{
		destGridHeight = pDestGrid->uHighLayer * GRID_LAYER_HEIGHT/POINT_PER_CM;
	}

	FLOAT gridDelta = abs(destGridHeight - srcGridHeight);

	//warnf(NAME_Log, TEXT("Grid Delta:%f"), gridDelta);

// 	if (abs(HitLocation.Z - srcGridHeight) > MaxGridSize)
// 	{
// 		HitLocation.Z = srcGridHeight;
// 	}
	

	//DrawDebugLine(StartPoint, EndPoint, 255, 0, 0, TRUE);

#if 1

	// 只有自己才判定，其它的由服务器控制
	if(IsMainPlayer() && (g_EanbleSlopeCheck || g_EnableMoveStepHeightCheck))
	{
		//if(Hit.Normal.Z > 0.0f) // 只限定往上走时的NORMAL
		{
			if(g_EanbleSlopeCheck)
			{
				// 判定斜率, 超过45度的不能上了
				FVector GravDir = FVector(0,0,-1);

				FLOAT slope = -1.f * (HitNormal | GravDir);

				if(slope < WalkableFloorZ) // 0.7 ~= 45度
				{
#if !SHIPPING_PC_GAME
					if(g_DebugMove)
					{
						//DrawDebugLine(Hit.Location, Hit.Location + (Hit.Normal * g_moveModGridSize),
						//	slope >= WalkableFloorZ ? 0 : 255, slope >= WalkableFloorZ ? 255 : 0, 0, TRUE);
						DrawDebugLine(HitLocation, HitLocation + (HitNormal * 128.0f),
							slope >= WalkableFloorZ ? 0 : 255, slope >= WalkableFloorZ ? 255 : 0, 0, TRUE);
					}
#endif 				Velocity = FVector::ZeroVector;
					eventStopped();
					return;
				}
			}
		}

		if(g_EnableMoveStepHeightCheck)
		{
			// 超过格子高度也不能走,用脚底坐标算,只管往上走，往下不管
			FLOAT zDiff = (HitLocation.Z - NewLocation.Z);

			//debugf(TEXT("---------zDiff:%f Hit.Normal.Z:%f"), zDiff, Hit.Normal.Z);

			//if((zDiff - 64.0f) > 0.005f)  // 有误差
			//if(gridDelta > 128.0f)
			if(pDestGrid && pDestGrid->bOutOfPoly)
			{

#if !SHIPPING_PC_GAME
				if(g_DebugMove)
				{
					DrawDebugLine(StartPoint, EndPoint, 0, 0, 255, TRUE);
					DrawDebugSphere(HitLocation, 5, 5, 0, 0, 255, TRUE);

					DrawDebugBox(Location, FVector(5, 5, 5), 0, 255, 0, TRUE);
					DrawDebugBox(Location, FVector(GetCollisionRadius(), GetCollisionRadius(), GetCollisionHeight()), 0, 255, 0, TRUE);

					DrawDebugSphere(NewLocation, 5, 5, 255, 0, 0, TRUE);


					//DrawDebugLine(Hit.Location, NewLocation, 255, zDiff > g_moveModGridSize ? 0 : 255, 0, TRUE);
					DrawDebugLine(HitLocation, NewLocation, 255, zDiff > 128.0f ? 0 : 255, 0, TRUE);
				}
#endif				
				Velocity = FVector::ZeroVector;
				eventStopped();

				// TODO:检查是不是正确的同步服务器的格子坐标方式
				// SetServerLocation(Location);
				return;
			}
		}
	} // IsMainPlayer() 

#endif
	//if(IsMainPlayer())
	{
		//DWORD moveCycles = appCycles();
// 		FCheckResult fHit(1.f);
// 		GWorld->MoveActor( this, Delta, Rotation, MOVE_NoFail, fHit );
		//g_moveCycles = Max(g_moveCycles, appCycles() - moveCycles);
// 		if(g_moveCycles > 10000)
// 		{
// 			appDebugBreak();
// 		}
	}
	
#if 1
	//if (Hit.Actor)
	//{
		//DrawDebugLine(Hit.Location, Hit.Location + Hit.Normal * 100, 255, 255, 0, TRUE);
		//DrawDebugSphere(Hit.Location, 5, 16, 255, 0, 0, TRUE);
		NewLocation = HitLocation + FVector(0, 0, GetCollisionHeight());
		SetLocation(NewLocation);

		// 暂时不需要这个功能
//  		if (Hit.Actor != Base && Physics == PHYS_Walking)
//  		{
//  			SetBase(Hit.Actor, Hit.Normal);
//  		}
	//}
	//else
	//{
	//	eventFalling();
	//}
#else
	//INT64 llSrcX = 0;
	//INT64 llSrcY = 0;
	//INT64 llSrcZ = 0;
	//ZONE_DATA* pDestZone = NULL;
	//GRID_DATA* pDestGrid = NULL;

	//gServerGridWorld.ConvertPosClient2Server(Location, llSrcX, llSrcY, llSrcZ);
	//gServerGridWorld.GetZoneAndGrid(llSrcX, llSrcY, llSrcZ, pDestZone, pDestGrid);
	//if(pDestGrid)
	//{																									   代码无效(需要使用SetLocation)
	//Location.Z = pDestGrid->uHighLayer * GRID_LAYER_HEIGHT/POINT_PER_CM + GetCollisionHeight();
	//}
#endif
	UpdatePlaneShadowPosition();
}

void AMMOActor::physWalking(FLOAT deltaTime, INT Iterations)
{
	CTRL_TICK("_MMOActor::physWalking");

	if (bUsingUnrealWalking)
	{
		return APawn::physWalking(deltaTime, Iterations);
	}

	// no z-axis movement while walking
	Acceleration.Z = 0;
	Velocity.Z = 0;

	if(isMoving==false)
	{
		Velocity		= FVector(0.f);
		Acceleration	= FVector(0.f);
		return;
	}

	// 如果基本没有速度就不要移动了
	if(Velocity.IsNearlyZero())
	{
		isMoving = false;
		ResetLastServerLocation();
		SetAutoAttackLockTime();
		return;
	}

	FVector Delta = Velocity * deltaTime;
	if (!Velocity.IsNearlyZero() && !Destination.IsNearlyZero())
	{
		FVector Des		= Destination - Location;
		//if((Des.X*Velocity.X + Des.Y*Velocity.Y <= 0.0) || (DesNext.X*Velocity.X + DesNext.Y*Velocity.Y <= 0.0) )

		FLOAT DeltaLength = Delta.Size2D();
		FLOAT DesLength	  = Des.Size2D();
		FLOAT JudgeLength = DeltaLength*0.5f;
		JudgeLength = JudgeLength<16.0f?JudgeLength:16.0f;

		if(DeltaLength+JudgeLength>DesLength)
		{

			Delta.X = Des.X;
			Delta.Y = Des.Y;

			moveDelta(Delta);
			eventStopped();
			ResetLastServerLocation();
			SetAutoAttackLockTime();

			return;
		}

		//if(Delta.SizeSquared2D() >= Des.SizeSquared2D() )
		//{
		//	if(IsMainPlayer()){
		//		warnf(TEXT("-----------------------------移动到达终点 physWalking Delta=%f Step=%f"),Des.Size2D(), Delta.Size2D());
		//	}

		//	Delta.X = Des.X;
		//	Delta.Y = Des.Y;

		//	moveDelta(Delta);
		//	eventStopped();
		//	ResetLastServerLocation();
		//	SetAutoAttackLockTime();
		//	
		//	return;
		//}

		if(isMoving==true && Des.SizeSquared2D()<256.0f)
		{
			AttackAi_LockTime=0.1f;
		}
	}
	else
	{
		if (Velocity.IsNearlyZero())
		{
			//warnf(TEXT("F[%d]------------------------physWalking:isMoving==false"),g_TotalFrameNum);
			isMoving = false;
			ResetLastServerLocation();
			SetAutoAttackLockTime();
			//ServerLocation = Location;	//commented by zhaojun@snda
		}
	}

	moveDelta(Delta);
}

INT32 AMMOActor::CalGridBlockRadius() // zhaojun@snda
{
	if (bDead)
	{
		return -1;
	}
	else
	{
		return 0;//(GetActorType() == EACT_NPC);
	}
}

void AMMOActor::ResetLastServerLocation(FLOAT ResetTime)
{
	LastServerLocation = ServerLocation;
	ResetLastLocationTime = ResetTime;
}

void AMMOActor::SetAutoAttackLockTime()
{
	AttackAi_LockTime = FSM::Ctrl.ctrlAttackAi_LockTime;
}

void AMMOActor::SetServerLocation(FVector NewLoc)
{
	ResetLastServerLocation(FSM::Ctrl.ctrlAttackAi_LastDelay * appFrand());
	AttackAi_LockTime=-1.0f;
	// zhaojun@snda
	if (ServerLocation != NewLoc)
	{
#if defined(CLIENT_GRID_EXT)
		gServerGridWorld.OnActorPosChanged(this, ServerLocation, NewLoc, CurBlockRadius);
#endif
		ServerLocation = NewLoc;
		gServerGridWorld.GetZoneAndGridXY( ServerLocation, GridX, GridY );
	}
}

void AMMOActor::ReCalGridBlockRadius(void)
{
	INT32 nNew = CalGridBlockRadius();

	if (CurBlockRadius != nNew)
	{
#if defined(CLIENT_GRID_EXT)
		gServerGridWorld.OnActorBlockRadiusChanged(this, this->ServerLocation, CurBlockRadius, nNew);
#endif
		CurBlockRadius = nNew;
	}
}

void AMMOActor::OnZoneOrGridChanged(void* pOldZonePter, void* pOldGridPter, void* pNewZonePter, void* pNewGridPter)
{
	ZONE_DATA* pOldZone = (ZONE_DATA*) pOldZonePter;
	GRID_DATA* pOldGrid = (GRID_DATA*) pOldGridPter;
	ZONE_DATA* pNewZone = (ZONE_DATA*) pNewZonePter;
	GRID_DATA* pNewGrid = (GRID_DATA*) pNewGridPter;

	//--------------------------------------------------------------------------
	//if(pNewGrid){
	//	bIsActorInSafeZone= (EPKType(pNewGrid->uPkType) == EPKT_Safe);
	//}
	//else{
	//	bIsActorInSafeZone=FALSE;
	//}

	if(IsMainPlayer()==FALSE)
	{
		return;
	}

	if(pNewGrid)
	{
		AMMOPlayer* mainPlayer = (AMMOPlayer*)this;
		UBOOL _IsInSafeZone = (EPKType(pNewGrid->uPkType) == EPKT_Safe);
		if(mainPlayer->IsInSafeZone != _IsInSafeZone)
		{
			mainPlayer->IsInSafeZone = _IsInSafeZone;
			if(_IsInSafeZone){
				MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, ServerID, SERVERID_SAFEZONE, BUFFID_SAFEZONE, 0, 0.0, 10000.0, 1,TRUE);
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage2")),MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage2") );

				MMOGameEvent_OneInt enterZoneEvt( GameEvent_EnterSceneZone, EPKT_Safe );
				MMOUI::PostGameEvent( &enterZoneEvt );
			}
			else
			{
				MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuff(ServerID, SERVERID_SAFEZONE);;
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage4")),MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage4") );

				MMOGameEvent_OneInt leaveZoneEvt( GameEvent_LeaveSceneZone, EPKT_Safe );
				MMOUI::PostGameEvent( &leaveZoneEvt );
			}
		}


		UBOOL _IsInFreePkZone = (EPKType(pNewGrid->uPkType) == EPKT_Free);
		if(mainPlayer->IsInFreePkZone != _IsInFreePkZone)
		{
			mainPlayer->IsInFreePkZone = _IsInFreePkZone;
			if(_IsInFreePkZone){
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage1")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage1"));

				MMOGameEvent_OneInt enterZoneEvt( GameEvent_EnterSceneZone, EPKT_Free );
				MMOUI::PostGameEvent( &enterZoneEvt );
			}
			else
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage5")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage5"));

				MMOGameEvent_OneInt leaveZoneEvt( GameEvent_LeaveSceneZone, EPKT_Free );
				MMOUI::PostGameEvent( &leaveZoneEvt );
			}

		}

	}




	////if(IsMainPlayer() && pNewGrid )
	////{
	////	if(pOldGrid==NULL || pNewGrid->uPkType!=pOldGrid->uPkType)
	////	{
	////		AMMOPlayer* mainPlayer = (AMMOPlayer*)this;
	////		//判定进入
	////		{
	////			if(EPKType(pNewGrid->uPkType)==EPKT_Safe){
	////				mainPlayer->IsInSafeZone = TRUE;
	////				MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, ServerID, SERVERID_SAFEZONE, BUFFID_SAFEZONE, 0, 0.0, 10000.0, 1,TRUE);
	////				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage2")),MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage2") );
	////			}
	////			else if(EPKType(pNewGrid->uPkType)==EPKT_Free)
	////			{
	////				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage1")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage1"));
	////			}
	////		}
	////	}
	////}

	////if(IsMainPlayer() && pOldGrid )
	////{
	////	if(pNewGrid==NULL || pNewGrid->uPkType!=pOldGrid->uPkType)
	////	{
	////		AMMOPlayer* mainPlayer = (AMMOPlayer*)this;
	////		//判定离开
	////		{
	////			if(EPKType(pOldGrid->uPkType)==EPKT_Safe){
	////				mainPlayer->IsInSafeZone = FALSE;
	////				MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuff(ServerID, SERVERID_SAFEZONE);;
	////				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage4")),MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage4") );
	////			}
	////			else if(EPKType(pOldGrid->uPkType)==EPKT_Free)
	////			{
	////				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage5")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage5"));
	////			}
	////		}
	////	}
	////}


	//if(IsMainPlayer() && pNewGrid && pOldGrid)
	//{
	//	if(pNewGrid->uPkType!=pOldGrid->uPkType)
	//	{
	//		AMMOPlayer* mainPlayer = (AMMOPlayer*)this;
	//		//判定进入
	//		{
	//			if(EPKType(pNewGrid->uPkType)==EPKT_Safe){
	//				mainPlayer->IsInSafeZone = TRUE;
	//				MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, ServerID, SERVERID_SAFEZONE, BUFFID_SAFEZONE, 0, 0.0, 10000.0, 1,TRUE);
	//				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage2")),MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage2") );
	//			}
	//			else if(EPKType(pNewGrid->uPkType)==EPKT_Free)
	//			{
	//				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage1")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage1"));
	//			}
	//		}
	//		//判定离开
	//		{
	//			if(EPKType(pOldGrid->uPkType)==EPKT_Safe){
	//				mainPlayer->IsInSafeZone = FALSE;
	//				MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuff(ServerID, SERVERID_SAFEZONE);;
	//				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage4")),MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage4") );
	//			}
	//			else if(EPKType(pOldGrid->uPkType)==EPKT_Free)
	//			{
	//				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage5")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage5"));
	//			}
	//		}		
	//	}
	//}
}


UAnimNodeSequence* AMMOActor::GetActiveAnimSequence()
{
	UMMOActorAT* CurrentAT = NULL;
	if (!IsInMeshChangeState())
	{
		CurrentAT = ActorAT;
	}
	else
	{
		CurrentAT = MeshChangeAnimController;
	}

	if (CurrentAT&&CurrentAT->ActionBlend)
	{
		if (CurrentAT->ActionBlend->ActiveChildIdx >= 0 && CurrentAT->ActionBlend->ActiveChildIdx < CurrentAT->ActionBlend->Children.Num())
		{
			UAnimNodeSequence* seq = Cast<UAnimNodeSequence>(CurrentAT->ActionBlend->Children(CurrentAT->ActionBlend->ActiveChildIdx).Anim);
			if (seq && seq->bPlaying)
			{
				return seq;
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}

	}
	else
	{
		return NULL;
	}
}

void AMMOActor::SetMoveType(BYTE InType)
{
	if (MoveType == InType)
	{
		return;
	}

	MoveType = InType;
	if (MoveType == EMT_Walk)
	{
		FLOAT MoveAnimP = GetMoveAnimPosition();
		SetMoveAnim(PawnAnimInfo.WalkAnim);
		SetMoveAnimPosition(MoveAnimP);//在快跑和慢跑切换的时候，保持相位的延续性
	}
	else if (MoveType == EMT_Run)
	{
		FLOAT MoveAnimP = GetMoveAnimPosition();
		SetMoveAnim(PawnAnimInfo.RunAnim);
		SetMoveAnimPosition(MoveAnimP);
	}
	else if (MoveType == EMT_Walk_Oblique)
	{
		FLOAT MoveAnimP = GetMoveAnimPosition();
		SetMoveAnim(PawnAnimInfo.WalkAnim_Oblique);
		SetMoveAnimPosition(MoveAnimP);
	}
	else if (MoveType == EMT_Run_Oblique)
	{
		FLOAT MoveAnimP = GetMoveAnimPosition();
		SetMoveAnim(PawnAnimInfo.RunAnim_Oblique);
		SetMoveAnimPosition(MoveAnimP);
	}
	else if (MoveType == EMT_Fly)
	{
		FLOAT MoveAnimP = GetMoveAnimPosition();
		SetMoveAnim(PawnAnimInfo.FlyAnim);
		SetMoveAnimPosition(MoveAnimP);
	}
	else if (MoveType == EMT_Fly_Oblique)
	{
		FLOAT MoveAnimP = GetMoveAnimPosition();
		SetMoveAnim(PawnAnimInfo.FlyAnim_Oblique);
		SetMoveAnimPosition(MoveAnimP);
	}

	
	else
	{
		// undefine
	}
}

BYTE AMMOActor::GetMoveType()
{
	return MoveType;
}

void AMMOActor::UpdateMoveType(FLOAT NewSpeed)
{
	CurrentMoveSpeed = NewSpeed;

	if (UseingAvatar)
	{
		//FLOAT SpeedCriticalValue = UseingAvatar->GetToggleMoveAnimCriticalSpeed();
		//if (SpeedCriticalValue > 0.f)
		//{
		//	if (MoveType == EMT_Walk)
		//	{
		//		if (NewSpeed > SpeedCriticalValue)
		//		{
		//			SetMoveType(EMT_Run);
		//		}
		//	}
		//	else if (MoveType == EMT_Run)
		//	{
		//		if (NewSpeed < SpeedCriticalValue)
		//		{
		//			SetMoveType(EMT_Walk);
		//		}
		//	}
		//}
		//else
		//{
		//	//warnf(TEXT("Avatar:%s can not get the right ToggleMoveAnimCriticalSpeed"), *UseingAvatar->GetPathName());
		//}
	}
}


//-----------------------------------------------------------------------
void AMMOActor::ShowDamageText( BYTE InType,UBOOL bCauseDead,BYTE PropertyType,INT DamageValue )
{
    //if (IsMainPlayer())
    //{
    //    if (MMOUI::GetUIManager()->HurtHideNameSelf)
    //        return;
    //}
    //else
    //{
    //    if (ActorType == EACT_OtherPlayer && MMOUI::GetUIManager()->HurtHideNameOther)
    //        return;
    //}

	FDamageResult dmg(EC_EventParm);
	dmg.HPChange = DamageValue;
	dmg.Type = InType;
	dmg.MainRole = IsMainPlayer();
	AMMOHud* Hud = MMOFacade::getSingleton().GetMyHud();
	if( Hud)
	{
		//if(dmg.MainRole==FALSE && DamageValue!=0 && ActorAttributes)
		//{
		//	INT NewHP = Max((ActorAttributes->HP + DamageValue),0);
		//	if(IsA(ANPC::StaticClass()))
		//	{
		//		MMOFacade::getSingleton().SetNPCHP( (ANPC*)this , NewHP );
		//	}
		//	else if(IsA(AMMOPlayer::StaticClass()))
		//	{
		//		MMOFacade::getSingleton().SetPlayerHP( (AMMOPlayer*)this , NewHP );
		//	}
		//}

		Hud->AddHitStats( dmg , Location );
	}
}

class AMMOPlayer* AMMOActor::GetBasePlayer(class AActor* InActor)
{
	if (InActor == NULL)
	{
		return NULL;
	}

	if (InActor->IsA(AMMOPlayer::StaticClass()))
	{
		return Cast<AMMOPlayer>(InActor);
	}
	else if (InActor->IsA(ANPC::StaticClass()))
	{
		ANPC* npc = (ANPC*)InActor;
		if (npc->Master)
		{
			return GetBasePlayer(npc->Master);
		}
	}
	else if (InActor->IsA(AMMOEmitter::StaticClass()))
	{
		AMMOEmitter* e = (AMMOEmitter*)InActor;
		if (e->Creator)
		{
			return GetBasePlayer(e->Creator);
		}
	}

	return NULL;
}

UBOOL AMMOActor::GetRelevantInfo(class AActor* InA,class AActor* InB,class AActor*& RelevantActor)
{
	AMMOPlayer* PlayerA = InA ? GetBasePlayer(InA) : NULL;
	AMMOPlayer* PlayerB = InB ? GetBasePlayer(InB) : NULL;

	if (MainController && MainController->Pawn)
	{
		if (MainController->Pawn == PlayerA)
		{
			RelevantActor = PlayerB;

			return TRUE;
		}
		else if (MainController->Pawn == PlayerB)
		{
			RelevantActor = PlayerA;
			return TRUE;
		}
	}

	return FALSE;
}

void AMMOActor::NativeProcessDamage(struct FDamageData InDamage)
{
	if (InDamage.bServerDamageData)
	{

		UBOOL bNeedShowDamageText = FALSE;
		AActor* RelevantActor = NULL;
		AMMOActor* TargetMMOActor = Cast<AMMOActor>(InDamage.BaseActor);

		UBOOL bRelevant = GetRelevantInfo(this, InDamage.BaseActor, RelevantActor);

		if (bRelevant && MainController)
		{
			AMMOPlayer* RelevantPlayer = Cast<AMMOPlayer>(RelevantActor);
			if (RelevantPlayer)
			{
				RelevantPlayer->bRelevant = TRUE;
			}


			bNeedShowDamageText = TRUE;
		}

		//新增召唤物的战斗飘字
		AMMOPlayer* pMainPlayer = Cast<AMMOPlayer>(MainController->Pawn);
		INT MainServerID = pMainPlayer ? pMainPlayer->ServerID:-1 ;
		if(this->dwHostID==MainServerID)
		{
			bNeedShowDamageText = TRUE;	
		}
		else if(TargetMMOActor && TargetMMOActor->dwHostID==MainServerID)
		{
			bNeedShowDamageText = TRUE;
		}


		if (InDamage.bCauseDead)
		{
			BYTE DS = 1; // DS_Anim
			FLOAT Delay = 0.0f;
			if (InDamage.AbilityData)
			{
				DS = InDamage.AbilityData->AbilityDeath;
				Delay = InDamage.AbilityData->Duration;
			}
			ProcessAbilityCauseDie(DS, InDamage.ElementaryType, Delay);
			if (InDamage.AbilityData)
			{
				InDamage.AbilityData->eventClientPlay(InDamage.CreatorId, InDamage.BaseActor, NULL, this, Location, 1.0f);
			}
		}

		//UBOOL bNeedShowDamageText = FALSE;
		//if (CreatorActor)
		//{
		//	if (CreatorActor->IsA(AMMOPlayer::StaticClass()))
		//	{
		//		if (((AMMOPlayer*)CreatorActor)->IsMainPlayer())
		//		{
		//			bNeedShowDamageText = TRUE;
		//		}	
		//		if (IsMainPlayer())
		//		{
		//			MMOUI::GetUIManager()->delegateFlashHPBar();
		//		}
		//	}
		//	else if (CreatorActor->IsA(AMMOEmitter::StaticClass()))
		//	{
		//		AMMOActor* mmoAttacker = Cast<AMMOActor>(((AMMOEmitter*)CreatorActor)->Creator);
		//		if (mmoAttacker && mmoAttacker->IsMainPlayer())
		//		{
		//			bNeedShowDamageText = TRUE;
		//		}
		//	}
		//	else if (CreatorActor->IsA(ANPC::StaticClass()))
		//	{
		//		AMMOActor* MasterActor = ((ANPC*)(CreatorActor))->Master;
		//		if (MasterActor && MasterActor->IsMainPlayer())
		//		{
		//			bNeedShowDamageText = TRUE;
		//		}
		//	}
		//}

		//if (!bNeedShowDamageText)
		//{
		//	if (IsMainPlayer())
		//	{
		//		bNeedShowDamageText = TRUE;
		//	}
		//	else if (IsA(ANPC::StaticClass()))
		//	{
		//		AMMOActor* MasterActor = ((ANPC*)this)->Master;
		//		if (MasterActor && MasterActor->IsMainPlayer())
		//		{
		//			bNeedShowDamageText = TRUE;
		//		}
		//	}
		//}

		if (InDamage.BaseActor)
		{
			if (InDamage.BaseActor->IsA(AMMOPlayer::StaticClass()))
			{	
				if (IsMainPlayer() && InDamage.DamageValue<0)
				{
					MMOUI::GetUIManager()->delegateFlashHPBar();
				}
			}
		}

		if (bNeedShowDamageText)
		{
			if(MMOUI::GetUIManager()->HurtHideNameSelf){
				if(InDamage.BaseActor == MMOFacade::getSingleton().GetMainPlayer())
				{
					return;
				}
				if( IsMainPlayer() )
				{
					return;
				}
				if(dwHostID==MainServerID)
				{
					return;
				}
				else if(TargetMMOActor && TargetMMOActor->dwHostID==MainServerID)
				{
					return;
				}
			}

			if(MMOUI::GetUIManager()->HurtHideNameOther)
			{
				if(ActorType == EACT_OtherPlayer && InDamage.BaseActor != MMOFacade::getSingleton().GetMainPlayer())
				{
					return;
				}
			}

			ShowDamageText(InDamage.DamageType, InDamage.bCauseDead, EDPT_Hp, InDamage.DamageValue);

			//if( this == FSMIO::getSingleton().Target.GetDesireMMOActor() )
			//{
			//	if(IsA(ANPC::StaticClass() ) )
			//	{
			//		ANPC* tnpc = Cast<ANPC>(this);
			//		MMOFacade::getSingleton().SetNPCHP( tnpc ,  ActorAttributes->GetCurHp()+InDamage.DamageValue, ActorAttributes->GetMaxHp());
			//	}
			//	else if( IsA(AMMOPlayer::StaticClass() ))
			//	{
			//		AMMOPlayer* tplayer = Cast<AMMOPlayer>(this);
			//		MMOFacade::getSingleton().SetPlayerHP( tplayer,ActorAttributes->GetCurHp()+InDamage.DamageValue, ActorAttributes->GetMaxHp());
			//	}
			//}
		}
	}
	else
	{
		if (InDamage.AbilityData)
		{
			InDamage.AbilityData->eventClientPlay(InDamage.CreatorId, InDamage.BaseActor, NULL, this, Location, 1.0f);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------
//void AMMOActor::physWalking(FLOAT deltaTime, INT Iterations)
//{
//	if (bUsingUnrealWalking)
//	{
//		return APawn::physWalking(deltaTime, Iterations);
//	}
//
//	// no z-axis movement while walking
//	Acceleration.Z = 0;
//	Velocity.Z = 0;
//
//	if (!Velocity.IsNearlyZero() && !Destination.IsNearlyZero())
//	{
//		FVector Des = Destination - Location;
//		if(Des.X*Velocity.X + Des.Y*Velocity.Y <= 0)
//		{
//			// clear velocity/accel, otherwise it's possible to continue drifting
//			Velocity		= FVector(0.f);
//			Acceleration	= FVector(0.f);
//			Des = Destination;
//			Des.Z = Location.Z;
//			SetLocation(Des);
//			eventStopped();
//		}
//		else
//		{
//			FVector Des = Destination - Location - Velocity*deltaTime;
//			if (Des.X * Velocity.X + Des.Y * Velocity.Y <= 0)
//			{
//				Velocity = Velocity.SafeNormal() * (Destination - Location).Size2D()/deltaTime;
//				Acceleration = FVector(0.f);
//			}
//		}
//	}
//	else
//	{
//		if (Velocity.IsNearlyZero())
//		{
//			isMoving = false;
//		}
//	}
//
//	// do the move
//	Velocity += Acceleration * deltaTime;
//	AngularVelocity = FVector(0.f);
//	FVector Delta = Velocity * deltaTime;
//
//	FVector NewLocation = Location + Delta;
//
//	FCheckResult fHit(1.f);
//	GWorld->MoveActor( this, Delta, Rotation, 0, fHit );
//
//	//debugf(L"HitTime=%f", fHit.Time);
//
//	// 成功地移动了delta
//	NewLocation.Z -= GetCollisionHeight();
//	FVector StartPoint = NewLocation + FVector(0, 0, g_moveModGridSize);
//	FVector EndPoint = NewLocation - FVector(0, 0, g_moveModGridSize * 10);
//	FCheckResult Hit(1.f);
//	GWorld->SingleLineCheck(Hit, this, EndPoint, StartPoint, TRACE_AllBlocking);
//	if (Hit.Actor)
//	{
//		NewLocation = Hit.Location + FVector(0, 0, GetCollisionHeight());
//		SetLocation(NewLocation);
//		if (Hit.Actor != Base && Physics == PHYS_Walking)
//		{
//			SetBase(Hit.Actor, Hit.Normal);
//		}
//	}
//	else
//	{
//		eventFalling();
//		if (Physics == PHYS_Walking)
//		{
//			//setPhysics(PHYS_Falling); // default if script didn't change physics
//		}
//	}
//
//
//	//// check floor
//	//FVector GravDir = (GetGravityZ() > 0.f) ? FVector(0.f, 0.f, 1.f) : FVector(0.f, 0.f, -1.f);
//	//FVector Down = GravDir * (UCONST_ACTORMAXSTEPHEIGHT + MAXSTEPHEIGHTFUDGE);
//	//FCheckResult Hit(1.f);
//	//FVector ColLocation = (CollisionComponent != NULL) ? Location + CollisionComponent->Translation : Location;
//	//GWorld->SingleLineCheck(Hit, this, ColLocation + Down, ColLocation, TRACE_AllBlocking, GetCylinderExtent());
//	//FLOAT FloorDist = Hit.Time * (UCONST_ACTORMAXSTEPHEIGHT + MAXSTEPHEIGHTFUDGE);
//	//if (Hit.Normal.Z < UCONST_MINFLOORZ && !Delta.IsNearlyZero() && (Delta | Hit.Normal) < 0)
//	//{
//	//	// slide down slope
//	//	FVector Slide = FVector(0.f, 0.f, UCONST_ACTORMAXSTEPHEIGHT) - Hit.Normal * (FVector(0.f, 0.f, UCONST_ACTORMAXSTEPHEIGHT) | Hit.Normal);
//	//	GWorld->MoveActor(this, -1 * Slide, Rotation, 0, Hit); 
//	//	if (Hit.Actor != Base && Physics == PHYS_Walking)
//	//	{
//	//		SetBase(Hit.Actor, Hit.Normal);
//	//	}
//	//}
//	//else if (Hit.Time < 1.f && (Hit.Actor != Base || FloorDist > MAXFLOORDIST))
//	//{
//	//	// move down to correct position 
//	//	GWorld->MoveActor(this, Down, Rotation, 0, Hit);
//	//	if (Hit.Actor != Base && Physics == PHYS_Walking)
//	//	{
//	//		SetBase(Hit.Actor, Hit.Normal);
//	//	}
//	//}
//	//else if (FloorDist < MINFLOORDIST)
//	//{
//	//	// move up to correct position (average of MAXFLOORDIST and MINFLOORDIST above floor)
//	//	GWorld->MoveActor(this, FVector(0.f, 0.f, 0.5f * (MINFLOORDIST + MAXFLOORDIST) - FloorDist), Rotation, 0, Hit);
//	//}
//	//else if (Hit.Time >= 1.f || Hit.Normal.Z < UCONST_MINFLOORZ)
//	//{
//	//	// fall
//	//	eventFalling();
//	//	if (Physics == PHYS_Walking)
//	//	{
//	//		setPhysics(PHYS_Falling); // default if script didn't change physics
//	//	}
//	//}
//
//}

TArray<USkeletalMeshComponent*> AMMOActor::GetClientWeaponSkMesh()
{
	TArray<USkeletalMeshComponent*> wpMesh;
	
	//for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
	//{
	//	if(AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->IsWeapon )
	//	{
	//		wpMesh.AddItem(AttachedDestructableParts(i)->SkelMeshComponent);
	//	}
	//}

	return wpMesh;
}


void AMMOActor::EnterGame()
{
	setPhysics(PHYS_Walking);
	bWaitForAbilityReply = false;
	CurrentAbilityState = EAS_End;
	AbilityStateCD = 0.0f;
	RotateToMousePointRefrence = 0;
	bMovingToMousePoint = false;
}

void AMMOActor::LeaveGame()
{
	setPhysics(PHYS_None);

	// only be called by main-player
	INT iTemp = 0;
	for ( INT i=0; i<this->Touching.Num(); i++ )
	{
		if ( Touching(i) && Touching(i)->Touching.FindItem(this, iTemp) )
		{
			EndTouch( Touching(i), 1 );
			--i;
		}
	}
}

void AMMOActor::RebuildMaterialControllerList()
{
	for (INT i = 0; i < Components.Num(); ++i)
	{
		if (Components(i) && Components(i)->IsA(USkeletalMeshComponent::StaticClass()))
		{
			USkeletalMeshComponent* Comp = Cast<USkeletalMeshComponent>(Components(i));
			if (Comp)
			{
				Comp->RebulidMeshMaterialList();

				for (INT j = 0; j < Comp->Attachments.Num(); ++j)
				{
					UMeshComponent* MeshComp = Cast<UMeshComponent>(Comp->Attachments(j).Component);
					if (MeshComp)
					{
						MeshComp->RebulidMeshMaterialList();
					}
				}
			}
		}
	}

	FNpcLevelColorData ColorData;
	if (GpDataManager && UseingAvatar && IsA(ANPC::StaticClass()))
	{
		ANPC* npc = (ANPC*)this;
		if (GpDataManager->GetNpcLevelColorData(npc->Quality, ColorData))
		{
			SetInitColor(ColorData.ParameterName, ColorData.ColorValue);
		}
	}
}

void AMMOActor::SetToNewMaterialEffect(BYTE InType,INT InId,UBOOL bOn,class UMaterialInstanceTimeVaryingEffectNode* InEffectData,class UMaterialInstance* InNewMaterial)
{
	for (INT i = 0; i < Components.Num(); ++i)
	{
		if (Components(i) && Components(i)->IsA(USkeletalMeshComponent::StaticClass()))
		{
			USkeletalMeshComponent* Comp = Cast<USkeletalMeshComponent>(Components(i));
			if (Comp)
			{
				if (bOn)
				{
					if (InType == EMET_ChangeMaterialProperty)
					{
						Comp->ChangeMaterialProperties(InId, InEffectData);
					}
					else if (InType == EMET_LoadOpacityMaterial)
					{
						Comp->SwitchToTransparentMaterial(InId);
					}
					else if (InType == EMET_ChangeMaterial)
					{
						Comp->ChangeMaterial(InId, InNewMaterial, TRUE);
					}

					for (INT j = 0; j < Comp->Attachments.Num(); ++j)
					{
						UMeshComponent* MeshComp = Cast<UMeshComponent>(Comp->Attachments(j).Component);
						if (MeshComp)
						{
							if (InType == EMET_ChangeMaterialProperty)
							{
								MeshComp->ChangeMaterialProperties(InId, InEffectData);
							}
							else if (InType == EMET_LoadOpacityMaterial)
							{
								MeshComp->SwitchToTransparentMaterial(InId);
							}
							else if (InType == EMET_ChangeMaterial)
							{
								MeshComp->ChangeMaterial(InId, InNewMaterial, TRUE);
							}
						}
					}
				}
				else
				{
					Comp->CloseEffect(InId);

					for (INT j = 0; j < Comp->Attachments.Num(); ++j)
					{
						UMeshComponent* MeshComp = Cast<UMeshComponent>(Comp->Attachments(j).Component);
						if (MeshComp)
						{
							MeshComp->CloseEffect(InId);
						}
					}
				}
			}
		}
	}
}

void AMMOActor::SetInitColor(FName InColorName, FVector InColor)
{
	for (INT i = 0; i < Components.Num(); ++i)
	{
		USkeletalMeshComponent* Component = Cast<USkeletalMeshComponent>(Components(i));
		if (Component)
		{
			Component->SetInitColor(InColorName, InColor);


			for (INT j = 0; j < Component->Attachments.Num(); ++j)
			{
				UMeshComponent* MeshComp = Cast<UMeshComponent>(Component->Attachments(j).Component);
				if (MeshComp)
				{
					MeshComp->SetInitColor(InColorName, InColor);
				}
			}
		}
	}
}



void AMMOActor::SetOpacity(FName InOpacityName, FLOAT OpacityValue)
{
	for (INT i = 0; i < Components.Num(); ++i)
	{
		USkeletalMeshComponent* Component = Cast<USkeletalMeshComponent>(Components(i));
		if (Component)
		{
			Component->SetOpacity(InOpacityName, OpacityValue);
		}
	}
}

class AMMOPlayerController* AMMOActor::GetMainController() const
{
	return MainController;
}

void AMMOActor::SetMainController(class AMMOPlayerController* ctrl)
{
#ifdef _DEBUG
	if(ctrl == NULL)
	{
		warnf(TEXT("[AMMOActor::SetMainController] player controller is NULL"));
		appDebugBreak();
	}
#endif

	MainController = ctrl;
}


void AMMOActor::SetThrowTo(FVector Des, FLOAT LastTime)
{
	if (LastTime <= 0.001f)
	{
		SetLocation(Des);
		return;
	}

	PassiveMoveType = PassiveMoveType_Throw;
	PassiveMoveDestination = Des;
	PassiveMoveLastTime = LastTime;
	PassiveMoveCurrentTime = 0.f;
	PassiveMoveStep = 0;
	ThrowExp = 1.0f;

	FCheckResult Hit(1.0f);
	FVector Start = Des + FVector(0,0, 780);
	FVector End = Des + FVector(0, 0, -2000);
	GWorld->SingleLineCheck( Hit, this, End, Start, TRACE_Blocking | TRACE_LevelGeometry | TRACE_Level | TRACE_Terrain, FVector(0,0,0));
	if (Hit.Actor)
	{
		PassiveMoveDestination.Z = Hit.Location.Z + GetCollisionHeight();
	}
}

void AMMOActor::DoThrowTo(FVector InP0, FVector InP1, FVector InT0, FVector InT1, FLOAT InLastTime, FLOAT InExp)
{
	if (PassiveMoveType != PassiveMoveType_Throw)
	{
		warnf(TEXT("Call DoThrowTo, but Not in the state PassiveMoveType_Throw, Current State is %d"), PassiveMoveType);
		return;
	}


	P0 = InP0;
	P1 = InP1;
	T0 = InT0;
	T1 = InT1;
	PassiveMoveLastTime = InLastTime;
	PassiveMoveCurrentTime = 0.f;
	PassiveMoveStep = 1;
	ThrowExp = InExp;


	if (Mesh)
	{
		// play anim
		UAnimSequence* Anim1 = Mesh->FindAnimSequence(PawnAnimInfo.BeatFlyAwayAnims[0]);
		UAnimSequence* Anim2 = Mesh->FindAnimSequence(PawnAnimInfo.BeatFlyAwayAnims[1]);
		if (Anim1 == NULL || Anim2 == NULL)
		{
			return;
		}

		FLOAT Anim1Length = Anim1->SequenceLength;
		FLOAT Anim2Length = Anim2->SequenceLength;
		FLOAT PlayRate = 1.0;
		if (Anim1Length > InLastTime)
		{
			PlayRate = Anim1Length / InLastTime;
		}
		ThrowAnimRate = 1.0;
		this->PlayAnimation(PawnAnimInfo.BeatFlyAwayAnims[0], 0, PlayRate);

		FLOAT MoveTime = Anim1Length / PlayRate;
		SetTimer(MoveTime, FALSE, FName(TEXT("PlayStandUpAnimFromBeatFlyAnim")), this);

		PassiveMoveLastTime = MoveTime;
	}

}

void AMMOActor::DashTo(FVector Des, FLOAT InLastTime)
{
	if (InLastTime <= 0.001f)
	{
		SetLocation(Des);
		UpdatePlaneShadowPosition();
		return;
	}

	PassiveMoveType = PassiveMoveType_DirectlyMove;
	PassiveMoveDestination = Des;
	PassiveMoveLastTime = InLastTime;
	PassiveMoveCurrentTime = 0.f;
	DashStartPoint = Location;
}

void AMMOActor::FearTo(FVector Des,FLOAT InLastTime)
{
	if (InLastTime <= 0.001f)
	{
		SetLocation(Des);
		return;
	}

	PassiveMoveType = PassiveMoveType_Fear;
	PassiveMoveLastTime = InLastTime;
	PassiveMoveCurrentTime = 0.f;
	FRotator NewRotation = (Des - Location).Rotation();
	NewRotation.Pitch = 0;
	NewRotation.Roll = 0;

	LockDesiredRotation(FALSE, TRUE);
	SetDesiredRotation(NewRotation, FALSE, TRUE, -1.f, TRUE);

	eventScriptMoveTo(Des, (Des - Location).Size2D() / InLastTime);
}

void AMMOActor::SetAttitudeType(BYTE type)
{
	//状态的改变
	EWeaponType t = (EWeaponType)GetWeaponType();
	if(t<EWT_Riding_1)
	{
		AttitudeTypeLast = t;
	}
	AttitudeType = (EWeaponType)type;
	RefreshAnimSet();
}

void AMMOActor::RevertAttitudeType()
{
	AttitudeType = AttitudeTypeLast;
	RefreshAnimSet();
}

void AMMOActor::EquipMount(INT MountID)
{
	if(MountInfo==NULL)return;

	for(INT i = 0; i < GpDataManager->MountList.Num() ; ++i)
	{	
		INT			 mID    = GpDataManager->MountList(i).MountID;
		if(mID == MountID)
		{
			if(ActorAttributes)ActorAttributes->MountID = MountID;
			UAvatarData* pMount = GpDataManager->MountList(i).AvatarDataLink ;

			SetAttitudeType(EWT_Riding_1);

			MountInfo->eventInitMount(pMount);
			MountInfo->eventAttachActor(this);

			MountInfo->SkelMeshComponent->SetLightingChannels(Mesh->LightingChannels);
			MountInfo->SkelMeshComponent->SetLightEnvironment(Mesh->LightEnvironment);

			AttachComponent( MountInfo->SkelMeshComponent );
			MountInfo->SkelMeshComponent->AttachComponentToSocket( Mesh, FName(TEXT("FX_Waist")));

			//状态的改变
			SetAttitudeType(EWT_Riding_1);


			RefreshMoveAnim();

			HideWeapon(TRUE);

			if (IsMainPlayer())
			{
				// 引导任务
				MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Mount);

				MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(MountID, ServerID, SERVERID_MOUNT, BUFFID_MOUNT, 0, 0.0,10000.0,0);//  pData->dwRemainDuration, pData->dwMaxDuration, pData->byStackCount);

			}
			break;
		}
	}
	RefreshAnim();
}


void AMMOActor::UnEquipMount()
{
	if(MountInfo && MountInfo->SkelMeshComponent )
	{
		MountInfo->SkelMeshComponent->DetachComponent(Mesh);
		DetachComponent(MountInfo->SkelMeshComponent);
		AttachComponent( Mesh );
	}
	if(ActorAttributes)ActorAttributes->MountID = 0;

	EWeaponType t = (EWeaponType)GetWeaponType();
	if(t>=EWT_Riding_1)
	{
		RevertAttitudeType();
		HideWeapon(FALSE);
		if (IsMainPlayer())
		{
			MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuff(ServerID, SERVERID_MOUNT); //  pData->dwRemainDuration, pData->dwMaxDuration, pData->byStackCount);
		}
	}
	RefreshAnim();
}


void AMMOActor::EnableRagdoll(UBOOL bOn)
{
	if (Mesh == NULL || Mesh->PhysicsAsset == NULL)
	{
		return;
	}

	if (bOn)
	{
		Mesh->SetRBChannel(RBCC_Untitled4);
		Mesh->SetRBCollidesWithChannel(RBCC_Default,TRUE);
		Mesh->SetRBCollidesWithChannel(RBCC_Pawn,TRUE);
		Mesh->SetRBCollidesWithChannel(RBCC_Vehicle,TRUE);
		Mesh->SetRBCollidesWithChannel(RBCC_Untitled1,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Untitled2,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Untitled3,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Untitled4,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_BlockingVolume,TRUE);
		Mesh->SetTickGroup(TG_PostAsyncWork);
		InitRagdoll();
		bCollideWorld = TRUE;
		Mesh->SetBlockRigidBody(TRUE);
		Mesh->bPauseAnims = TRUE;
	}
	else
	{
		Mesh->SetRBChannel(RBCC_Untitled3);
		Mesh->SetRBCollidesWithChannel(RBCC_Default,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Pawn,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Vehicle,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Untitled1,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Untitled2,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Untitled3,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_Untitled4,FALSE);
		Mesh->SetRBCollidesWithChannel(RBCC_BlockingVolume,FALSE);
		Mesh->SetTickGroup(TG_PreAsyncWork);
		TermRagdoll();
		Mesh->SetBlockRigidBody(FALSE);
		Mesh->bPauseAnims = FALSE;
	}
}

FLOAT AMMOActor::CalculateRootBoneMoveSpeed(FName InAnimSeq)
{
	USkeletalMeshComponent* CMesh = GetActiveBaseMesh();
	if (CMesh == NULL)
	{
		return -1.f;
	}

	UAnimSequence* MoveSeq = CMesh->FindAnimSequence(InAnimSeq);
	if (MoveSeq)
	{
		FBoneAtom fba;
		MoveSeq->GetBoneAtom(fba, 0, 0.f, FALSE, FALSE, NULL);
		FMatrix StartMat = fba.ToMatrix();
		FVector StartPoint = StartMat.GetOrigin();

		MoveSeq->GetBoneAtom(fba, 0, MoveSeq->SequenceLength - 0.0001, FALSE, FALSE, NULL);
		FMatrix EndMat = fba.ToMatrix();
		FVector EndPoint = EndMat.GetOrigin();

		FLOAT Speed = (EndPoint - StartPoint).Size2D() / (MoveSeq->SequenceLength - 0.0001f);

		FLOAT TempScale = 1.0f;
		if (UseingAvatar)
		{
			TempScale = UseingAvatar->AvatarDrawScale * UseingAvatar->MeshScale;
			if (TempScale <= 0.000001)
			{
				TempScale = 1.0f;
			}
		}

		Speed *= TempScale;

		return Speed;
	}

	return -1.f;
}

void AMMOActor::ReplaceMovementAnim(UBOOL bOn,INT ProcessId, FName InMoveAnim,FName InIdleAnim)
{
#if SHOW_ABILITY_MOVE_CONTROL_LOG
	debugf(TEXT("ReplaceMovementAnim bOn[%d] ProcessId[%d] InMoveAnim[%s] InIdleAnim[%s]"), bOn, ProcessId, *InMoveAnim.ToString(), *InIdleAnim.ToString());
#endif
	
	if (bOn)
	{
		if (ProcessId == CurrentMoveControlComponentId)
		{
			warnf(TEXT("The same move control component id, error, check!!!"));
			return;
		}

		if (InMoveAnim != NAME_None)
		{
			if (CurrentMoveControlComponentId == 0)
			{
				BackupMoveAnim = PawnAnimInfo.RunAnim;
			}
			SetMoveAnim(InMoveAnim);
			SetMoveAnimRate(1.0);
		}

		if (InIdleAnim != NAME_None)
		{
			if (CurrentMoveControlComponentId == 0)
			{
				BackupIdleAnim = PawnAnimInfo.IdleAnim;
			}
			//BackupIdleAnim = InIdleAnim;
			SetIdleAnim(InIdleAnim);
			SetMoveAnimRate(1.0);
		}

		CurrentMoveControlComponentId = ProcessId;
	}
	else
	{
		if (ProcessId != CurrentMoveControlComponentId)
		{
			// no need to process
			return;
		}

		CurrentMoveControlComponentId = 0;

		SetMoveAnimRate(1.0);

		if (BackupMoveAnim != NAME_None)
		{
			SetMoveAnim(BackupMoveAnim);
			BackupMoveAnim = NAME_None;
		}

		if (BackupIdleAnim != NAME_None)
		{
			SetIdleAnim(BackupIdleAnim);
			BackupIdleAnim = NAME_None;
		}
	}
}

void AMMOActor::RotateTo(FRotator NewRotation,FLOAT DeltaTime)
{
	UBOOL bHasRotateAnim = (Mesh->FindAnimSequence(PawnAnimInfo.TurnLeftAnim) && Mesh->FindAnimSequence(PawnAnimInfo.TurnRightAnim));
	if (DeltaTime <= 0 || !bHasRotateAnim)
	{
		DeltaTime = 0.f;
	}

	if (Physics != PHYS_Walking)
	{
		setPhysics(PHYS_Walking);
	}

	LockDesiredRotation(FALSE, TRUE);
	SetDesiredRotation(NewRotation, FALSE, TRUE, 0.f, TRUE);

	INT animKey = EffectProcessor::GetSingleton().CreateEffectId();
	INT CurrYaw = Rotation.Yaw & 0xFFFF;
	INT NewYaw = NewRotation.Yaw & 0xFFFF;
	UBOOL bTurnLeft = TRUE;
	if (NewYaw == CurrYaw)
	{
		return;
	}
	else if (NewYaw < CurrYaw)
	{
		NewYaw += 0xffff;
	}

	bTurnLeft = (NewYaw - CurrYaw >= 32768) ? FALSE : TRUE;

	FName AnimToPlay = bTurnLeft ? PawnAnimInfo.TurnLeftAnim : PawnAnimInfo.TurnRightAnim;
	PlayAnimation(AnimToPlay, animKey, 1.0, TRUE);
}

void AMMOActor::ScaleTo(UBOOL bOn,INT InEffectKey,FVector InValue,FLOAT InLastTime)
{
	if (bOn)
	{
		ScaleEffectId = InEffectKey;

		//if ((InValue - DrawScale3D).IsNearlyZero())
		//{
		//	return;
		//}

		if (InValue.X < 0.f)
		{
			InValue.X = 1.f;
		}

		if (InValue.Y < 0.f)
		{
			InValue.Y = 1.f;
		}

		if (InValue.Z < 0.f)
		{
			InValue.Z = 1.f;
		}

		if (InLastTime < 0.f)
		{
			InLastTime = 0;
		}

		if (InLastTime <= 0.01)
		{
			SetDrawScale3D(InValue);
			return;
		}

		bScaling = TRUE;
		ScaleTarget = InValue;
		ScaleLeftTime = InLastTime;
		ScaleTime = InLastTime;
	}
	else
	{
		if (ScaleEffectId == InEffectKey)
		{
			if (InLastTime <= 0.01)
			{
				SetDrawScale3D(FVector(1.0, 1.0, 1.0));
				ScaleEffectId = 0;
				return;
			}

			bScaling = TRUE;
			ScaleTarget = FVector(1.0, 1.0, 1.0);
			ScaleLeftTime = InLastTime;
			ScaleTime = InLastTime;
			ScaleEffectId = 0;
		}
	}

}

void AMMOActor::UpdateLocationZ()
{
	if (MMOFacade::getSingleton().bLevelsLoaded)
	{
		FCheckResult Hit(1.0f);
		FLOAT MaxHeight = 600.000000;
		FLOAT MinHeight = -600.000000;
		FVector Start = Location + FVector(0, 0, MaxHeight);
		FVector End = Location + FVector(0, 0, MinHeight);
		GWorld->SingleLineCheck(Hit, this, End, Start, TRACE_World, FVector(0, 0, 0));
		if (Hit.Actor)
		{
			FVector NewLocation = Location;
			NewLocation.Z = Hit.Location.Z + GetCollisionHeight();
			if ((NewLocation - Location).IsNearlyZero() == FALSE)
			{
				SetLocation(NewLocation);
			}
		}
	}
}

FLOAT AMMOActor::GetCollisionHeight()
{
	return APawn::GetCollisionHeight();
}

FLOAT AMMOActor::GetCollisionRadius()
{
	return APawn::GetCollisionRadius();
}


void AMMOActor::PlayRandomAnim()
{
	// nothing todo
}

void AMMOActor::ResetIdleTimer()
{
	// nothing todo
}

class USkeletalMeshComponent* AMMOActor::GetActiveBaseMesh()
{
	if (OverriddenMesh)
	{
		return OverriddenMesh;
	}

	return Mesh;
}

void AMMOActor::SetVisible(UBOOL InVisible)
{
	if (InVisible)
	{
		bVisibleByAbility = TRUE;
		bVisibleByKismet = TRUE;
		bVisibleByMergeMesh = TRUE;
		bVisibleByHotKey = TRUE;
		RefreshVisible();
	}
	else
	{
		warnf(TEXT("error called : SetVisible[FALSE]"));
	}
}

void AMMOActor::SetEmitterAttachedVisible(UBOOL InVisible)
{
	EffectProcessor::GetSingleton().SetClientProjectileVisible(ServerID, InVisible);
}

void AMMOActor::SetVisibleByAbility(UBOOL InVisible)
{
	if (bVisibleByAbility != InVisible)
	{
		bVisibleByAbility = InVisible;
		RefreshVisible();
	}
}
void AMMOActor::SetVisibleByMergeMesh(UBOOL InVisible)
{
	if (bVisibleByMergeMesh != InVisible)
	{
		bVisibleByMergeMesh = InVisible;
		RefreshVisible();
	}
}
void AMMOActor::SetVisibleByKismet(UBOOL InVisible)
{
	if (bVisibleByKismet != InVisible)
	{
		bVisibleByKismet = InVisible;
		RefreshVisible();
	}
}

void AMMOActor::SetVisibleByHotKey(UBOOL InVisible)
{
	if (bVisibleByHotKey != InVisible)
	{
		bVisibleByHotKey = InVisible;
		RefreshVisible();
	}
}

void AMMOActor::SetVisibleByCharacterCountLimit(UBOOL InVisible)
{
	if (bVisibleByCharacterCountLimit != InVisible)
	{
		bVisibleByCharacterCountLimit = InVisible;
		RefreshVisible();
	}
}

void AMMOActor::AttachEquip(BYTE InType,class UObject* InObj,UBOOL bVisible)
{
	if (InObj == NULL)
	{
		return;
	}

	INT idx = EquipList.AddZeroed();
	EquipList(idx).BodyMeshType = InType;
	EquipList(idx).ComponentOrActor = InObj;
	EquipList(idx).bVisible	= bVisible;

	if (InObj->IsA(UActorComponent::StaticClass()))
	{
		AttachComponent((UActorComponent*)InObj);

	}
	else if ( InObj->IsA( AArmorActor::StaticClass() ) )
	{
		(( AArmorActor * )InObj)->SetVisible(bVisible);
	}
}
void AMMOActor::DetachEquip(BYTE InType)
{
	for (INT i = 0; i < EquipList.Num(); ++i)
	{
		if (EquipList(i).BodyMeshType == InType)
		{
			if (EquipList(i).ComponentOrActor)
			{
				if (EquipList(i).ComponentOrActor->IsA(UActorComponent::StaticClass()))
				{
					DetachComponent((UActorComponent*)EquipList(i).ComponentOrActor);
				}
				else if (EquipList(i).ComponentOrActor->IsA(AArmorActor::StaticClass()))
				{
					((AArmorActor*)(EquipList(i).ComponentOrActor))->OnTakeOff();
				}
			}
			EquipList.Remove(i);
			break;
		}
	}
}

void AMMOActor::DetachAllEquip()
{
	for (INT i = 0; i < EquipList.Num(); ++i)
	{
		if (EquipList(i).ComponentOrActor && EquipList(i).ComponentOrActor->IsA(AArmorActor::StaticClass()))
		{
			if(!EquipList(i).ComponentOrActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
			{
				GWorld->DestroyActor((AArmorActor*)(EquipList(i).ComponentOrActor));
			}			
		}
	}
}

void AMMOActor::SetEquipVisibleFlag(BYTE InType,UBOOL bVisible)
{
	for (INT i = 0; i < EquipList.Num(); ++i)
	{
		if (EquipList(i).BodyMeshType == InType)
		{
			EquipList(i).bVisible = bVisible;
			break;
		}
	}

}


UBOOL AMMOActor::GetEquipVisibleFlag(class UObject* InObj)
{
	for (INT i = 0; i < EquipList.Num(); ++i)
{
		if (EquipList(i).ComponentOrActor == InObj)
	{
			return EquipList(i).bVisible;
		}
	}
	return TRUE;
}

class UObject* AMMOActor::GetEquip(BYTE InType)
{
	for (INT i = 0; i < EquipList.Num(); ++i)
	{
		if (EquipList(i).BodyMeshType == InType)
		{
			return EquipList(i).ComponentOrActor;
		}
	}

	return NULL;
}


UBOOL AMMOActor::HoldWeapon(UBOOL bHold)
{
	AArmorActor* Weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon));
	if (Weapon)
	{
		Weapon->SwitchAttachSocket(bHold ? 0 : 1);
	}

	Weapon = Cast<AArmorActor>(GetEquip(EBMT_Weapon2));
	if (Weapon)
	{
		Weapon->SwitchAttachSocket(bHold ? 0 : 1);
	}

	return TRUE;
}

void AMMOActor::RefreshVisible()
{
	UBOOL bVisible = (bVisibleByAbility && bVisibleByKismet && bVisibleByMergeMesh && bVisibleByHotKey && bVisibleByCharacterCountLimit && (MoveType != EMT_PrepareStall && MoveType != EMT_InStall));// && fActorDataResetDelay<=0.0f);

	UBOOL bHasOverriddenMesh = (OverriddenMesh != NULL);

	SetHidden(!bVisible);

	for (INT i = 0; i < Components.Num(); ++i)
	{
		UPrimitiveComponent* TestPri = Cast<UPrimitiveComponent>(Components(i));
		if (TestPri && TestPri != OverriddenMesh)
		{
			UBOOL tempVisible = bVisible;
			if ( bVisible )
				tempVisible = GetEquipVisibleFlag( Components(i) );
			TestPri->SetHiddenGame(!tempVisible || bHasOverriddenMesh);
			USkeletalMeshComponent* TestMesh = Cast<USkeletalMeshComponent>(TestPri);
			if (TestMesh)
			{
				for (INT j = 0; j < TestMesh->Attachments.Num(); ++j)
				{
					UPrimitiveComponent* TestSubPri = Cast<UPrimitiveComponent>(TestMesh->Attachments(j).Component);
					if (TestSubPri)
					{
						TestSubPri->SetHiddenGame(!tempVisible || bHasOverriddenMesh);
					}
				}
			}
		}
	}

	if (OverriddenMesh)
	{
		OverriddenMesh->SetHiddenGame(!bVisible);
	}
	HideWeapon(!(bVisible && bVisibleWeaponByAbility) || bHasOverriddenMesh);

	SetBodyPartVisible(EBMT_BackOrnamental, bVisible);

	SetBodyPartVisible(EBMT_Back, bVisible);

	SetEmitterAttachedVisible(bVisible);

	// Kismet下关闭升级，复活，等特效
	if (WorldInfo && WorldInfo->MyEmitterPool)
	{
		WorldInfo->MyEmitterPool->SetHidden(!bVisibleByKismet);
	}
}

void AMMOActor::SetWeaponVisibleByAbility(UBOOL InVisible)
{
	if (InVisible != bVisibleWeaponByAbility)
	{
		bVisibleWeaponByAbility = InVisible;
		RefreshVisible();
	}
}

UBOOL AMMOActor::CanShowAbilityEmitter(INT CreatorId)
{
	if (GIsEditor)
	{
		return TRUE;
	}

	if (!bVisibleByKismet || !bVisibleByMergeMesh || !bVisibleByHotKey || !bVisibleByCharacterCountLimit)
	{
		return FALSE;
	}

	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
	if (mainActor && CreatorId == mainActor->ServerID)
	{
		if (MMOUI::GetUIManager()->SkillEffectHideSelf)
		{
			return FALSE;
		}
	}
	else
	{
		if (MMOUI::GetUIManager()->SkillEffectHideOther)
		{
			return FALSE;
		}
	}

	return TRUE;
}


UBOOL AMMOActor::IsPlayingAnyAnimation()
{
	if (!IsInMeshChangeState())
	{
		if (ActorAT)
		{
			return ActorAT->IsPlayingAnyAnimation();
		}
	}
	else
	{
		if (MeshChangeAnimController)
		{
			return MeshChangeAnimController->IsPlayingAnyAnimation();
		}
	}

	return TRUE;
}


void AMMOActor::StopAnimation()
{
	if (IsInMeshChangeState())
	{
		if (MeshChangeAnimController)
		{
			MeshChangeAnimController->StopAnimation();
		}
	}
	else
	{
		if (ActorAT)
		{
			ActorAT->StopAnimation();
		}

		for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
		{
			if (AttachedDestructableParts(i) && AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT)
			{
				AttachedDestructableParts(i)->ActorAT->StopAnimation();
			}
		}

		if (MountInfo && MountInfo->ActorAT)
		{
			MountInfo->ActorAT->StopAnimation();
		}
	}
}

FLOAT AMMOActor::GetAnimSeqLength(FName InName)
{
	USkeletalMeshComponent* CurrentMesh = this->GetActiveBaseMesh();
	if (CurrentMesh)
	{
		UAnimSequence* as = CurrentMesh->FindAnimSequence(InName);
		if (as)
		{
			return as->SequenceLength;
		}
	}

	return 0.f;
}



void AMMOActor::OnAnimEndNative(class UAnimNodeSequence* SeqNode,FLOAT PlayedTime,FLOAT ExcessTime)
{
	//local DestructablePartAttachInfo info;
	//local MMOActorAT AT;

	//ActorAT.OnAnimEnd(SeqNode,  PlayedTime,  ExcessTime);
	if (ActorAT)
	{
		ActorAT->OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	}

	if (MeshChangeAnimController)
	{
		MeshChangeAnimController->OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	}

	//if(MeshChangeAnimController != none)
	//{
	//	MeshChangeAnimController.OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	//}

	////WeaponEquip = GetWeaponEquip();
	////if(WeaponEquip != none)
	////{
	////	WeaponEquip.OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	////}
	//AT = GetWeaponEquipMeshAnimationController(0);
	//if(AT != none)
	//{
	//	AT.OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	//}
	//AT = GetWeaponEquipMeshAnimationController(1);
	//if(AT != none)
	//{
	//	AT.OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	//}

	//UMMOActorAT* AT = GetWeaponEquipMeshAnimationController(0);
	//if (AT)
	//{
	//	AT->OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	//}
	//AT = GetWeaponEquipMeshAnimationController(1);
	//if (AT)
	//{
	//	AT->OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	//}


	for (INT i = 0; i < AttachedDestructableParts.Num(); ++i)
	{
		if (AttachedDestructableParts(i) && AttachedDestructableParts(i)->SkelMeshComponent && AttachedDestructableParts(i)->ActorAT)
		{
			AttachedDestructableParts(i)->ActorAT->OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
		}
	}




	//foreach AttachedDestructableParts(info)
	//{
	//	if(info.SkelMeshComponent != none &&   info.ActorAT !=none )
	//	{
	//		info.ActorAT.OnAnimEnd(SeqNode,  PlayedTime,  ExcessTime);
	//	}
	//}

	//if(MountInfo!=none && MountInfo.ActorAT != none)
	//{
	//	MountInfo.ActorAT.OnAnimEnd(SeqNode,  PlayedTime,  ExcessTime);
	//}

	if (MountInfo && MountInfo->ActorAT)
	{
		MountInfo->ActorAT->OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
	}

}

void AMMOActor::DoRevive()
{
	bDead = FALSE;
	bFreeze = FALSE;
	bStun = FALSE;
	bWaitForAbilityReply = FALSE;
	bCollideWorld = FALSE;
	setPhysics(PHYS_Walking);
	RefreshAnim();

	// play dying animation
	PlayAnimation(PawnAnimInfo.IdleAnim);

	EffectProcessor::GetSingleton().ClearAllComponentByBaseActorIdWhenRevive(ServerID);

}

void AMMOActor::SetDead()
{
	if (bDead == FALSE)
	{
		bDead = TRUE;
		//ReleasePlanShow();
		RefreshAnim();
	}
}

void AMMOActor::ProcessAbilityCauseDie(BYTE InStyle, BYTE InElementaryType, FLOAT Delay)
{
	if (IsPendingKill() || (WorldInfo && WorldInfo->Game && WorldInfo->Game->bLevelChange))
	{
		return;
	}

	//if (bDead == FALSE)
	//{
	//	warnf(NAME_Warning, TEXT("ProcessAbilityCauseDie  bDead = TRUE") );
	//	bDead = TRUE;
	//}

	RefreshAnim();

	StopMovement();

	if (!PlaySpecialDying(Delay))
	{
		if (!PlayStateDying(Delay))
		{
			if (!PlayCommonDying(InStyle, InElementaryType, Delay))
			{
				PlayAnimation(PawnAnimInfo.DieAnim, 0, 1.000000, FALSE, 1, 1, 1, 0, 0.000000, 0.100000, 0.100000, EAL_State);
			}
		}
	}
}

void AMMOActor::ProcessDefaultDie(BYTE DieCause)
{
	StopMovement();

	//SetDead();
	//ReleasePlanShow();
	bDead = TRUE;

	if (DieCause == cdrSilent)
	{
		SetHidden(TRUE);
		warnf(NAME_Warning, TEXT("ProcessDefaultDie0  xxxxxxxxxxxxxxxxxxxxxxxxxxxxx") );
		return;
	}

	UBOOL bNeedPlayDieAction = UBOOL(DieCause != cdrSpell);

	if (bNeedPlayDieAction)
	{
		RefreshAnim();

		if (!PlaySpecialDying())
		{
			if (!PlayStateDying())
			{
				if(ActorAT)
				{
					FLOAT AnimBlendInTime = ActorAT->AnimBlendInTime;
					ActorAT->AnimBlendInTime = 0.0;
					PlayAnimation(PawnAnimInfo.DieAnim, 0, 1.000000, FALSE, 1, 1, 1, 0, 0.000000, 0.00000, 0.00000, EAL_State);
					ActorAT->AnimBlendInTime = AnimBlendInTime;
				}
			}
		}
	}
}

void AMMOActor::ClearOldEffects()
{
	//EffectProcessor::GetSingleton().ClearAllComponentByBaseActorId(ServerID);

	// check physics
	if (Mesh)
	{
		Mesh->PhysicsWeight = 0.f;
		Mesh->SetHasPhysicsAssetInstance(FALSE);
		EnableRagdoll(FALSE);
	}
}

void AMMOActor::StopMovement()
{
	Destination = Location;
	Velocity = FVector(0, 0 , 0);
	Acceleration = FVector(0, 0, 0);
	isMoving = FALSE;
	ResetLastServerLocation();
	SetAutoAttackLockTime();
	SetMoveAnimRate(1.0f);
}

FLOAT AMMOActor::GetValidZValue(FVector InLocation, FLOAT MaxHeight, FLOAT MinHeight)
{
	FCheckResult Hit(1.0f);
	FVector Start = InLocation + FVector(0,0, MaxHeight);
	FVector End = InLocation + FVector(0, 0, MinHeight);
	GWorld->SingleLineCheck( Hit, this, End, Start, TRACE_World, FVector(0,0,0));
	if (Hit.Actor)
	{
		return Hit.Location.Z;
	}
	else
	{
		return InLocation.Z;
	}
}

void AMMOActor::AddInteractiveWind( FLOAT Strength )
{
	//AWindPointSource* wps = Cast<AWindPointSource>(GWorld->SpawnActor(AWindPointSource::StaticClass(), NAME_None, Location));
	UWindPointSourceComponent* wpsc = 
		Cast<UWindPointSourceComponent>(StaticConstructObject(UWindPointSourceComponent::StaticClass(), this, NAME_None, RF_ScriptMask));
	if( wpsc )
	{
		wpsc->Strength = Strength;
		wpsc->SetInteractive( TRUE );
		AttachComponent( wpsc );
	}
}

void AMMOActor::UpdateStaticLightEnvironment()
{
	if( LightEnvironment )
		LightEnvironment->SetNeedsStaticUpdate();
}

void AMMOActor::InitPlaneShow()
{
	if ( planeShadowIndex == -1 && !GSystemSettings.GetAllowDynamicShadows() && UsePlaneShadow )
		planeShadowIndex = AInstancedShadowPlaneActor::GetInstancedShadowPlaneActor( TRUE )->AddInstance( FVector( Location.X, Location.Y, Location.Z -  CylinderComponent->CollisionHeight ), CylinderComponent->CollisionRadius / 100.f );
}

void AMMOActor::UpdatePlaneShow()
{
	if(ShowHeadName==FALSE || bVisibleByAbility==FALSE )//|| IsPickable==FALSE)
	{
		ReleasePlanShow();
	}
	else
	{
		InitPlaneShow();
	}
}

void AMMOActor::SetEnvironmentComponentEnable()
{
	if ( LightEnvironment && GWorld->GetWorldInfo()->EmissiveSilhouetteFactor < 0.001f 
		 && !IsMainPlayer() )
	{
		LightEnvironment->SetEnabled( FALSE );
	}
}
void AMMOActor::UpdatePlaneShadowPosition()
{
	if ( planeShadowIndex < 0 )
		return;
	if ( AInstancedShadowPlaneActor::GetInstancedShadowPlaneActor( FALSE ) )
	{
		FVector position( Location.X, Location.Y, Location.Z -  CylinderComponent->CollisionHeight );// = GetShadowLocation();//( Location.X, Location.Y, Location.Z -  CylinderComponent->CollisionHeight );
// 		if( IsMainPlayer() )
// 		{
// 			debugf(TEXT("ShadowPosition: x = %f, y = %f, z= %f\n"), position.X, position.Y, position.Z );
// 		}
		AInstancedShadowPlaneActor::GetInstancedShadowPlaneActor( FALSE )->Refresh( planeShadowIndex, position );
	}
}
void AMMOActor::ReleasePlanShow()
{
	if ( planeShadowIndex < 0 )
		return;
	if ( AInstancedShadowPlaneActor::GetInstancedShadowPlaneActor( FALSE ) )
		AInstancedShadowPlaneActor::GetInstancedShadowPlaneActor( FALSE )->ReleaseInstance( planeShadowIndex );
	planeShadowIndex = -1;
}

void UAnimNotify_ClothNoise::Notify( class UAnimNodeSequence* NodeSeq )
{
	if (GIsEditor)
	{
		if (!PreviewSound || !NodeSeq->SkelComponent)
		{
			return;
		}

		UAudioComponent* AudioComponent = UAudioDevice::CreateComponent( PreviewSound, NodeSeq->SkelComponent->GetScene(), NodeSeq->SkelComponent->GetOwner(), 0 );
		if( AudioComponent )
		{

			AudioComponent->bUseOwnerLocation	= 0;
			AudioComponent->Location			= NodeSeq->SkelComponent->LocalToWorld.GetOrigin();

			AudioComponent->bAllowSpatialization	&= GIsGame;
			AudioComponent->bIsUISound				= !GIsGame;
			AudioComponent->bAutoDestroy			= 1;
			//AudioComponent->SubtitlePriority		= SUBTITLE_PRIORITY_ANIMNOTIFY;
			AudioComponent->Play();
		}
	}
	else
	{
		if (NodeSeq && NodeSeq->SkelComponent)
		{
			AMMOActor* OwnerActor = Cast<AMMOActor>(NodeSeq->SkelComponent->GetOwner());
			if (OwnerActor)
			{
				OwnerActor->PlayClothNoise(ActionType);
			}
		}
	}

}

void UAnimNotify_DoodadLoopSound::Notify(class UAnimNodeSequence* NodeSeq)
{
    //DoodadManager::getSingleton().TryPlayDoodadLoopSound();
}

FVector AMMOActor::GetShadowLocation()
{
	if ( !APawn::Mesh )
		return FVector( Location.X, Location.Y, Location.Z -  CylinderComponent->CollisionHeight );
	FVector boonBip01 = APawn::Mesh->GetBoneLocation( TEXT("Bip01") );
	if ( boonBip01.IsNearlyZero() )
		return FVector( Location.X, Location.Y, Location.Z -  CylinderComponent->CollisionHeight );
	FVector rootPos   = APawn::Mesh->GetBoneLocation( TEXT("root") );
	FVector endPos    = boonBip01 + FVector( 0, 0, -1 ) * CylinderComponent->CollisionHeight * 10 ;

	FCheckResult Hit;
	if( !GWorld->SingleLineCheck(Hit, NULL, endPos, boonBip01, TRACE_World | TRACE_Level, FVector(0.f,0.f,0.f), NULL) )
	{
		if(Hit.Actor || Hit.Component)
		{
			//OutZ = Hit.Location.Z;
			return Hit.Location /*- ( boonBip01 + FVector( 0, 0, rootPos.Z - boonBip01.Z ) )*/;
		}
	}

	return endPos;
}