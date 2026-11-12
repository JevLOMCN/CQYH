

#include "stdafx.h"
#include "EngineAudioDeviceClasses.h"
#include "UnTerrain.h"
#include "AbilityProcessor.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOBuffManager.h"
#include "MMOStats.h"
#include "MMOSkillManager.h"
#include "MMOCoolDownManager.h"
#include "MMOQuestManager.h"
#include "MMOHurtManager.h"
#include "MMOChatManager.h"
#include "MMOGuideManager.h"
#include "MMOJourneyManager.h"
#include "FSMIO.h"
#include "MMOClientVariableManager.h"
#include "MMOPerformance.h"

using namespace std;

bool g_disableBloodyContent = false; // oscar@snda
extern FSystemSettings GSystemSettings; // zhangrui@snda
extern INT g_LastRequestAbilitySerialId;
extern INT g_LastRequestAbilityId;
UBOOL BaseComponent::Init(InitData& InData)
{
	if (!InData.ComponentData)
	{
		return FALSE;
	}
	bStopped = FALSE;
	ComponentData = InData.ComponentData;
	CreatorId = InData.CreatorId;
	BaseActorId = InData.BaseActorId;
	TargetId = InData.TargetId;
	TargetPoint = InData.TargetPoint;
	Rate = InData.Rate;
	MaxDistance = InData.MaxDistance;
	ComponentId = CreateNewId();
	StopType = ComponentData->StopType;
	CurrentTime = 0.f;
	Duration = ComponentData->Duration;

	bClearWhenOwerRevive = ComponentData->bClearWhenRevive;

	return TRUE;
}

void BaseComponent::Tick(FLOAT DeltaTime)
{
	if (bStopped)
	{
		return;
	}
	if (StopType == ACBT_StopWhenConstDurationPass)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime > Duration)
		{
			OnAction(FALSE);
		}
	}
}

void BaseComponent::OnAction(UBOOL bOn)
{
	if (bOn)
	{
	}
	else
	{
		bStopped = TRUE;
	}
}

void BaseComponent::ResetLifeSpan(FLOAT InCurrentTime, FLOAT InDuration)
{
	CurrentTime = InCurrentTime;
	Duration = InDuration;
}

AActor* BaseComponent::GetBaseActor()
{
	AActor* Ret = NULL;
	if (bUseEngineObjectId)
	{
		Ret = Cast<AActor>(UObject::GetIndexedObject(BaseActorId));
	}
	else
	{
		if (BaseActorId == -1)
		{
			return MMOFacade::getSingleton().GetMainActor();
		}
		else
		{
			Ret = MMOFacade::getSingleton().GetActor(BaseActorId);
		}
	}

	if (Ret && Ret->IsPendingKill())
	{
		Ret = NULL;
	}

	return Ret;
}

AActor* BaseComponent::GetTargetActor()
{
	AActor* Ret = NULL;
	if (bUseEngineObjectId)
	{
		Ret = Cast<AActor>(UObject::GetIndexedObject(TargetId));
	}
	else
	{
		Ret = MMOFacade::getSingleton().GetActor(TargetId);
	}

	if (Ret && Ret->IsPendingKill())
	{
		Ret = NULL;
	}

	return Ret;

}

AMMOActor* BaseComponent::GetBaseCharacter()
{
	AActor* Ret = GetBaseActor();
	return Cast<AMMOActor>(Ret);
}

AMMOActor* BaseComponent::GetTargetCharacter()
{
	AActor* Ret = GetTargetActor();
	return Cast<AMMOActor>(Ret);
}

AActor* BaseComponent::GetCreatorActor()
{
	AActor* Ret = NULL;
	if (bUseEngineObjectId)
	{
		Ret = Cast<AActor>(UObject::GetIndexedObject(CreatorId));
	}
	else
	{
		Ret = MMOFacade::getSingleton().GetActor(CreatorId);
	}

	if (Ret && Ret->IsPendingKill())
	{
		Ret = NULL;
	}

	return Ret;
}

AMMOActor* BaseComponent::GetCreatorCharacter()
{
	AActor* Ret = GetCreatorActor();
	return Cast<AMMOActor>(Ret);
}

AActor* BaseComponent::GetIndexedObject(INT InId)
{
	AActor* Ret = Cast<AActor>(UObject::GetIndexedObject(InId));
	if (Ret && !Ret->bDeleteMe && !Ret->IsPendingKill())
	{
		return Ret;
	}

	return NULL;
}

INT BaseComponent::CreateNewId()
{
	return EffectProcessor::GetSingleton().CreateEffectId();
}


UBOOL AnimComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Anim;

	AnimData = Cast<UAbilityAnimationComponent>(InData.ComponentData);
	if (!AnimData)
	{
		return FALSE;
	}

	AMMOActor* actor = GetBaseCharacter();
	if (!actor)
	{
		return FALSE;
	}

	AnimIdx = -1;
	if (AnimData->Anims.Num() > 0)
	{
		for (INT i = 0; i < 3; ++i)
		{
			AnimIdx = RandHelper(AnimData->Anims.Num());
			if(actor->eventHaveAnim(AnimData->Anims(AnimIdx)))
			{
				break;
			}
		}
	}
	if (AnimIdx == -1)
	{
		return FALSE;
	}

	return TRUE;
}

void AnimComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AMMOActor* actor = GetBaseCharacter();
	if (!actor)
	{
		bStopped = TRUE;
		return;
	}

	if (bOn)
	{
		EAnimBlendType bt = EABT_FullBody;
		if (actor->ActorIsMoving())
		{
			bt = EABT_UpperBody;
		}
		if (AnimData->bForcePlay)
		{
			bt = EABT_FullBody;
		}

		AnimKey = actor->GetNewAnimKey();

		BYTE AnimLevel = AnimData->AnimLevel;
		if (actor->IsA(ANPC::StaticClass()))
		{
			ANPC* npc = (ANPC*)actor;
			if (AnimData->AnimLevel == EAL_LowWound && npc->Quality <= E_nq_Normal5)
			{
				AnimLevel = EAL_HighWound;
			}
		}

		if (AnimData->bLoop)
		{
			FLOAT ST = FLOAT(AnimData->StartFrame) / 30.f;
			AnimLastTime = AnimData->LoopTime;
			FLOAT Ret = actor->PlayAnimation(AnimData->Anims(AnimIdx), AnimKey, AnimData->Rate * Rate, AnimData->bLoop, RBA_Discard, RBA_Discard, RBA_Discard, bt, ST, AnimData->BlendInTime, AnimData->BlendOutTime, AnimLevel);
			if (Ret < 0.001)
			{
				OnAction(FALSE);
				return;
			}
		}
		else
		{
			FLOAT ST = FLOAT(AnimData->StartFrame) / 30.f;
			if (AnimData->Anims(AnimIdx).ToString() == TEXT("Death"))
			{
				actor->SetIdleAnim(actor->PawnAnimInfo.DeadAnim);
				actor->SetMoveAnim(actor->PawnAnimInfo.DeadAnim);

				FLOAT AnimBlendInTime = actor->AnimBlendInTime;
				actor->AnimBlendInTime = 0.0;
				AnimLastTime = actor->PlayAnimation(AnimData->Anims(AnimIdx), AnimKey, AnimData->Rate * Rate, AnimData->bLoop, RBA_Discard, RBA_Discard, RBA_Discard, bt, ST, 0.0f, 0.0f, AnimLevel);
				actor->AnimBlendInTime = AnimBlendInTime;
			}
			else
			{
				AnimLastTime = actor->PlayAnimation(AnimData->Anims(AnimIdx), AnimKey, AnimData->Rate * Rate, AnimData->bLoop, RBA_Discard, RBA_Discard, RBA_Discard, bt, ST, AnimData->BlendInTime, AnimData->BlendOutTime, AnimLevel);
			}
		}

		if (AnimLastTime < 0.0001)
		{
			OnAction(FALSE);
			return;
		}

		if (AnimData->bHiddenWeapon)
		{
			//actor->HideWeapon(TRUE);
			actor->SetWeaponVisibleByAbility(FALSE);
		}
		else
		{
			//actor->HideWeapon(FALSE);
			actor->SetWeaponVisibleByAbility(TRUE);
		}
	}
	else
	{
		if (AnimData->bHiddenWeapon && AnimKey == actor->GetCurrentAnimKey())
		{
			//actor->HideWeapon(FALSE);
			actor->SetWeaponVisibleByAbility(TRUE);
		}

		if (AnimData->bLoop)
		{
			actor->eventStopAnimationByKey(AnimKey);
		}
	}
}

void AnimComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("AnimComponent");

	BaseComponent::Tick(DeltaTime);
}

void AnimComponent::SetRate(FLOAT NewRate)
{
	if (Rate == NewRate)
	{
		BaseComponent::SetRate(NewRate * AnimData->Rate);
		return;
	}

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(BaseActorId);
	if (actor)
	{
		actor->SetAnimRateById(AnimKey, NewRate * AnimData->Rate);
	}
}

UBOOL ActionProcComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_ActionProc;

	ActionProcData = Cast<UAbilityActionProcComponent>(InData.ComponentData);
	if (!ActionProcData)
	{
		return FALSE;
	}

	AMMOActor* actor = GetBaseCharacter();
	if (!actor)
	{
		return FALSE;
	}

	return TRUE;
}

void ActionProcComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(BaseActorId);
	if (!actor)
	{
		bStopped = TRUE;
		return;
	}

	if (bOn)
	{
		ActionProcData->eventDoActionProc( actor );
	}
	else
	{
		//actor->eventStopAnimationBy(AnimData->Anims(AnimIdx));
		//actor->eventBlendToIdle();
	}
}



UBOOL SoundComponent::Init(InitData& InData)
{
	UBOOL bRet = FALSE;
	bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Sound;

	SoundData = Cast<UAbilitySoundCueComponent>(InData.ComponentData);
	if (!SoundData)
	{
		return FALSE;
	}

	SoundCueComponentIdx = -1;

	return TRUE;
}

void SoundComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("SoundComponent");

	BaseComponent::Tick(DeltaTime);
}


void SoundComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AActor* actor = GetBaseActor();
	AMMOActor* mmoActor = Cast<AMMOActor>(actor);
	if (!actor)
	{
		return;
	}


	FLOAT LimitedRadius = Clamp(GSystemSettings.SoundLevel, 0.f, 1.f);
	LimitedRadius = SoundData->MinRadius * (1.0 - LimitedRadius) + SoundData->MaxRadius * LimitedRadius;
	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainActor();
	if (!mainPlayer)
	{
		return;
	}

	if ((actor->Location - mainPlayer->Location).SizeSquared() > LimitedRadius * LimitedRadius)
	{
		return;
	}

	if (bOn)
	{
		bStopped = FALSE;

		if(SoundData->SoundPlayType == ESPT_WeaponUseSound)
		{
			if (mmoActor)
			{
				mmoActor->PlayWeaponUseSound(SoundData->SoundIdx);
			}
		}
		else if(SoundData->SoundPlayType == ESPT_WeaponHitSound)
		{
			if (WeaponHitType == -1)
			{
				return;
			}
			if (mmoActor)
			{
				BYTE mt = mmoActor->GetBodyMaterialType();
				mmoActor->PlayWeaponHitSound(WeaponHitType, mt);
			}
		}
		else if (SoundData->SoundPlayType == ESPT_ClothNoise)
		{
			if (mmoActor)
			{
				mmoActor->PlayClothNoise(SoundData->ActionType);
			}
		}
		else
		{
			extern UBOOL g_bMineHitSound;
			if(SoundData->SoundPlayType == ESPT_MineHitSound && g_bMineHitSound==FALSE)
			{
				return;
			}

			if (!GWorld->Scene)
			{
				warnf(L"GWorld->Scene is null, can not play sound. check!!!");
				return;
			}

			EElementaryType EleType = EETT_Elementary0;
			AMMOActor* TempCreator = GetCreatorCharacter();
			if (TempCreator)
			{
				EleType = (EElementaryType)TempCreator->ElementaryType;
			}

			TArray<USoundCue*> ToPlayList;
			SoundData->GetSoundCueByElementaryType(EleType, ToPlayList);

			for(INT i = 0; i < ToPlayList.Num(); ++i)
			{
				UAudioComponent* AudioComponent = UAudioDevice::CreateComponent( ToPlayList(i), GWorld->Scene, actor, 0 );
				if( AudioComponent )
				{
					AudioComponent->Location = actor->Location;
					AudioComponent->bUseOwnerLocation = TRUE;
					AudioComponent->bStopWhenOwnerDestroyed = TRUE;
					AudioComponent->bAllowSpatialization	&= GIsGame;
					AudioComponent->bIsUISound				= !GIsGame;
					AudioComponent->bAutoDestroy			= 1;
					//AudioComponent->SubtitlePriority		= SUBTITLE_PRIORITY_ANIMNOTIFY;
					AudioComponent->Play();
					SoundCueComponentIdxs.AddItem(AudioComponent->GetIndex());
				}
			}
		}
	}
	else
	{
		bStopped = TRUE;
		for (INT i = 0; i < SoundCueComponentIdxs.Num(); ++i)
		{
			UObject* data = UObject::GetIndexedObject(SoundCueComponentIdxs(i));
			UAudioComponent* ac = Cast<UAudioComponent>(data);
			if (ac)
			{
				UBOOL bPlaying = ac->IsPlaying();
				ac->bAutoDestroy = TRUE;
				if (bPlaying)
				{
					ac->Stop();
				}
			}
		}
		if (SoundData->EndSound)
		{
			actor->PlaySound(SoundData->EndSound);
		}
	}

	
}


AActor* EmitterComponent::GetBaseActor()
{
	if (!EmitterData)
	{
		return NULL;
	}

	if (EmitterData->SourceType == EST_TargetActor)
	{
		return BaseComponent::GetTargetActor();
	}
	else
	{
		return BaseComponent::GetBaseActor();
	}
}

AActor* EmitterComponent::GetTargetActor()
{
	if (!EmitterData)
	{
		return NULL;
	}

	if (EmitterData->SourceType == EST_TargetActor)
	{
		return BaseComponent::GetBaseActor();
	}
	else
	{
		return BaseComponent::GetTargetActor();
	}
}


UBOOL EmitterComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Emitter;

	EmitterData = Cast<UAbilityEmitterComponent>(InData.ComponentData);
	if (!EmitterData)
	{
		return FALSE;
	}

	StartTime = InData.StartTime;
	Visible = TRUE;

	return TRUE;
}
void EmitterComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("EmitterComponent");

	BaseComponent::Tick(DeltaTime);

	if (ProjecitleId > 0)
	{
		AActor* BaseActor = GetBaseActor();
		if (BaseActor && BaseActor->IsA(AMMOActor::StaticClass()))
		{
			AMMOActor* mmoActor = (AMMOActor*)BaseActor;
			if (mmoActor)
			{
				if (Visible != mmoActor->CanShowAbilityEmitter(BaseActorId))
				{
					Visible = mmoActor->CanShowAbilityEmitter(BaseActorId);

					AMMOEmitter* ProActor = Cast<AMMOEmitter>(UObject::GetIndexedObject(ProActorId));
					if (ProActor && ProActor->IsPendingKill() == FALSE)
					{
						ProActor->SetHidden(!Visible);
					}

					if (BeamTargetActorId > 0)
					{
						AMMOEmitter* BeamTargetActor = Cast<AMMOEmitter>(UObject::GetIndexedObject(BeamTargetActorId));
						if (BeamTargetActor && BeamTargetActor->IsPendingKill() == FALSE)
						{
							BeamTargetActor->SetHidden(!Visible);
						}
					}
				}
			}
		}
	}
}

void EmitterComponent::ResetLifeSpan(FLOAT InCurrentTime, FLOAT InDuration)
{
	check(InCurrentTime >= 0.f);
	check(InDuration >= 0.f);
	BaseComponent::ResetLifeSpan(InCurrentTime, InDuration);

	if (ProjecitleId > 0)
	{
		AMMOEmitter* ProActor = Cast<AMMOEmitter>(UObject::GetIndexedObject(ProActorId));
		if (ProActor && ProActor->IsPendingKill() == FALSE)
		{
			ProActor->CurrentTime = CurrentTime;
			ProActor->Duration = Duration;
		}

		if (BeamTargetActorId > 0)
		{
			AMMOEmitter* BeamTargetActor = Cast<AMMOEmitter>(UObject::GetIndexedObject(BeamTargetActorId));
			if (BeamTargetActor && BeamTargetActor->IsPendingKill() == FALSE)
			{
				BeamTargetActor->CurrentTime = CurrentTime;
				BeamTargetActor->Duration = Duration;
			}
		}
	}
}

void EmitterComponent::OnAction(UBOOL bOn)
{

	SCOPE_CYCLE_COUNTER(STAT_AbilityParticleTime);

#if SHOW_ABILITY_EMITTER_LOG
	debugf(TEXT("EmitterComponent(%s) bOn=%d"), *(EmitterData->GetFullName()), bOn);

#endif

	BaseComponent::OnAction(bOn);

	if (bOn)
	{
		// oscar@snda
		if(g_disableBloodyContent && EmitterData && EmitterData->ParticleSystemTemplate && EmitterData->ParticleSystemTemplate->IsBloodyContent)
		{
			bStopped = TRUE;
			return;
		}
		// oscar@snda

		if (!EffectProcessor::GetSingleton().CanSpawnEmitter())
		{
			if (!EmitterData->bImportant && EmitterData->EmitterType != EMIT_FlyTo && EmitterData->EmitterType != EMIT_ConnectTo)
			{
				bStopped = TRUE;
				//warnf(TEXT("too many emitters(%d), can not spawn any more"), EffectProcessor::GetSingleton().GetCurrentEmitterNumber());
				return;
			}
		}

		AActor* BaseActor = GetBaseActor();
		AActor* Target = GetTargetActor();
		if (!BaseActor)
		{
			bStopped = TRUE;
			return;
		}

		AMMOActor* mmoActor = NULL;
		if (BaseActor->IsA(AMMOActor::StaticClass()))
		{
			mmoActor = (AMMOActor*)BaseActor;
		}

		FMatrix ToUseSpawnMatrix;
		FMatrix ToUseBaseActorMatrix;
		FName ToUseOverwriteSocketName = EmitterData->AttachSocketName;

		UMeshComponent* WeaponMesh1 = NULL;
		UMeshComponent* WeaponMesh2 = NULL;
		AArmorActor* Weapon1 = NULL;
		AArmorActor* Weapon2 = NULL;

		if (mmoActor)
		{
			if (EmitterData->bDynamicFindTheNearestSocket && EmitterData->EmitterType == EMIT_Normal)
			{
				FName OutSocketName = EmitterData->FindTheNearestSocketToTarget(mmoActor, Target, EmitterData->AttachSocketName);
				if (OutSocketName != NAME_None)
				{
					 ToUseOverwriteSocketName = OutSocketName;
				}
			}

			mmoActor->GetWeaponMeshComponents(WeaponMesh1, WeaponMesh2);
			Weapon1 = Cast<AArmorActor>(mmoActor->GetEquip(EBMT_Weapon));
			Weapon2 = Cast<AArmorActor>(mmoActor->GetEquip(EBMT_Weapon2));
			ToUseSpawnMatrix = EmitterData->GetSocketMatrix(mmoActor->GetActiveBaseMesh(), Cast<USkeletalMeshComponent>(WeaponMesh1), Cast<USkeletalMeshComponent>(WeaponMesh2), TargetPoint, FALSE, ToUseOverwriteSocketName);
		}
		else
		{
			FRotationTranslationMatrix frtm(EmitterData->RelativeRotation, EmitterData->RelativeLocation);
			ToUseSpawnMatrix = frtm * FRotationTranslationMatrix(BaseActor->Rotation, BaseActor->Location);
		}

		if (EmitterData->bUseBaseRotation == FALSE)
		{
			ToUseSpawnMatrix = FScaleRotationTranslationMatrix(ToUseSpawnMatrix.GetScaleVector(), EmitterData->RelativeRotation, ToUseSpawnMatrix.GetOrigin());
		}

		if (EmitterData->SourceType == EST_TargetPoint)
		{
			ToUseBaseActorMatrix = FRotationTranslationMatrix(BaseActor->Rotation, TargetPoint);
		}
		else
		{
			ToUseBaseActorMatrix = BaseActor->LocalToWorld();
		}

		BYTE OverwriteTargetType = EmitterData->ProjectileTargetType;
		if (EmitterData->EmitterType == EMIT_FlyTo)
		{
			if (EmitterData->ProjectileTargetType == EPTT_Actor)
			{
				if (Target == NULL)
				{
					OverwriteTargetType = EPTT_UseCasterDirection;
				}
			}

			if (OverwriteTargetType == EPTT_UseCasterDirection)
			{
				ToUseSpawnMatrix = FRotationTranslationMatrix(BaseActor->Rotation, ToUseSpawnMatrix.GetOrigin());
				FRotator NewRot = EmitterData->FlyDirection + EmitterData->DirectionDelta * StartTime;
				FRotationMatrix frm(NewRot);
				ToUseSpawnMatrix = frm * ToUseSpawnMatrix;
			}
		}



		// spawn emitter
		AMMOEmitter* ProActor = NULL;
		AMMOEmitter* BeamTargetActor = NULL;

		{
			SCOPE_CYCLE_COUNTER(STAT_AbilityParticleSpawnTime);
			ProActor = Cast<AMMOEmitter>(GWorld->SpawnActor(AMMOEmitter::StaticClass(), NAME_None, ToUseSpawnMatrix.GetOrigin(), ToUseSpawnMatrix.Rotator(), NULL, 0, 0, BaseActor));

		}

		// base res

		if (!ProActor)
		{
			bStopped = TRUE;
			return;
		}
		ProActorId = ProActor->GetIndex();

		if (EmitterData->bAddToRoot && mmoActor && mmoActor->IsMainPlayer())
		{
			UBOOL bFound = FALSE;
			bAddedToRoot = TRUE;
			//debugf(L"AddedToRoot : %s", *(ProActor->GetFullName()));
			for (INT i = 0; i < mmoActor->CanNotBeGCEmitters.Num(); ++i)
			{
				if (mmoActor->CanNotBeGCEmitters(i) == NULL)
				{
					mmoActor->CanNotBeGCEmitters(i) = ProActor;
					bFound = TRUE;
					break;
				}
			}
			if (!bFound)
			{
				mmoActor->CanNotBeGCEmitters.AddItem(ProActor);
			}
		}


		//FLOAT RadiusScale = EmitterData->Scale;
		//if (EmitterData->bScaleByStandardCollision && mmoActor)
		//{
		//	RadiusScale *= mmoActor->CollisionRadiusRate;
		//}

		EElementaryType EleType = EETT_Elementary0; // No element
		AMMOActor* TempCreator = GetCreatorCharacter();
		if (TempCreator && TempCreator->ElementaryCreatorId != 0)
		{
			EleType = (EElementaryType)TempCreator->ElementaryType;
		}
		UParticleSystem* UseParticleSystem = EmitterData->GetParticleSystemByElementaryType(EleType);

		if(UseParticleSystem)
		{				
#if SHOW_ABILITY_EMITTER_LOG
			debugf(L"Emitter Particle=%s", *UseParticleSystem->GetFullName());
#endif

			SCOPE_CYCLE_COUNTER(STAT_AbilityParticleActiveTime);
			ProActor->SetTemplate(UseParticleSystem, TRUE);
		}

		ProActor->EmitterData = EmitterData;

		ProActor->OverwriteSocketName = ToUseOverwriteSocketName;

		ProActor->OverwriteTargetType = OverwriteTargetType;

		ProActor->SpawnMatrix = ToUseSpawnMatrix;
		ProActor->BaseActorMatrix = ToUseBaseActorMatrix;


		FVector BC1(0, 0, 0);
		FVector BC2(0, 0, 0);
		EmitterData->GetBezierControlPoint(BC1, BC2);
		ProActor->OverwriteBezierControl.v1 = BC1;
		ProActor->OverwriteBezierControl.v2 = BC2;

		ProActor->OverwriteEmitterType = EmitterData->EmitterType;

		ProActor->MaxDistance = MaxDistance;

		FVector DstPoint;

		if (EmitterData->EmitterType == EMIT_Surround)
		{
			if (EmitterData->SourceType == EST_BaseActor)
			{
				if (ProActor->ParticleSystemComponent && mmoActor)
				{
					UMeshComponent* BaseMesh = mmoActor->OverriddenMesh ? mmoActor->OverriddenMesh : mmoActor->Mesh;
					ProActor->ParticleSystemComponent->SetMeshComponentParameter(FName(TEXT("VertSurfaceActor")), BaseMesh);
				}
			}
			else if (EmitterData->SourceType == EST_BaseWeapon1)
			{
				if (!ProActor->ParticleSystemComponent || !mmoActor || !WeaponMesh1 || !Weapon1)
				{
					if(!ProActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
					{
						GWorld->DestroyActor(ProActor);
					}
					bStopped = TRUE;
					return;
				}

				ProActor->ParticleSystemComponent->SetMeshComponentParameter(FName(TEXT("VertSurfaceActor")), WeaponMesh1);

				ProActor->SetBase(Weapon1);
				ProActor->SetTickGroup(TG_PostUpdateWork);
			}
			else if (EmitterData->SourceType == EST_BaseWeapon2)
			{
				if (!ProActor->ParticleSystemComponent || !mmoActor || !WeaponMesh2 || !Weapon2)
				{
					if(!ProActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
					{
						GWorld->DestroyActor(ProActor);
					}
					bStopped = TRUE;
					return;
				}

				ProActor->ParticleSystemComponent->SetMeshComponentParameter(FName(TEXT("VertSurfaceActor")), WeaponMesh2);

				ProActor->SetBase(Weapon2);
				ProActor->SetTickGroup(TG_PostUpdateWork);
			}
		}



		FVector NewScale = FVector(1, 1, 1) * EmitterData->Scale;
		if (EmitterData->bScaleByStandardCollision)
		{
			NewScale = EmitterData->Scale * ProActor->SpawnMatrix.GetScaleVector();
		}
		ProActor->SetDrawScale3D(NewScale);

		// add other effect

		if(EmitterData->EmitterType == EMIT_ConnectTo)
		{
			if(EmitterData->BeamTargetParticleSystemTemplate)
			{
				BeamTargetActor = Cast<AMMOEmitter>(GWorld->SpawnActor(AMMOEmitter::StaticClass(), NAME_None));
				if (BeamTargetActor)
				{
					BeamTargetActorId = BeamTargetActor->GetIndex();
					BeamTargetActor->SetTemplate(EmitterData->BeamTargetParticleSystemTemplate, true);
					if(BeamTargetActor->ParticleSystemComponent)
					{
						BeamTargetActor->ParticleSystemComponent->Scale = NewScale.Size() / EmitterData->Scale * EmitterData->BeamTargetScale;
						BeamTargetActor->ParticleSystemComponent->BeginDeferredUpdateTransform();
					}
					ProActor->BeamTargetActor = BeamTargetActor;
					ProActor->BeamTargetActor->bSonProjectile = true;

					// target sound
					if (EmitterData->BeamTargetSoundCue)
					{
						UAudioComponent* AudioComponent = UAudioDevice::CreateComponent( EmitterData->BeamTargetSoundCue, GWorld->Scene, BeamTargetActor, 0 );
						if( AudioComponent )
						{
							AudioComponent->Location = ProActor->Location;
							AudioComponent->bUseOwnerLocation = TRUE;
							AudioComponent->bStopWhenOwnerDestroyed = TRUE;
							AudioComponent->bAllowSpatialization	&= GIsGame;
							AudioComponent->bIsUISound				= FALSE;
							AudioComponent->bAutoDestroy			= 1;
							AudioComponent->Play();
						}

					}
				}
			}
		}

		// set light data
		if (EmitterData->MyPointLightComponent)
		{
			// Automatically turn off dynamic lights when the overall effect level is below 3; 0 is custom
			if (GSystemSettings.AutoFPSLevel > M_SCALE_DOWN_ABILITY_POINT_LIGHT)
			{
				UObject* InOuter = UObject::GetTransientPackage();
				ProActor->MyPointLightComponent = Cast<UPointLightComponent>(UObject::StaticConstructObject( EmitterData->MyPointLightComponent->StaticClass(),
					InOuter,
					NAME_None, 
					RF_ScriptMask,
					EmitterData->MyPointLightComponent));
				ProActor->MyPointLightComponent->CastShadows = FALSE;	// disable shadow casting
				ProActor->LightFadeOutTime = EmitterData->LightFadeOutTime;
				ProActor->LightFadeUpTime = EmitterData->LightFadeUpTime;
				ProActor->LightLifeSpan = EmitterData->LightLifeSpan;
			}
		}

		// set up sound data
		USoundCue* SoundToPlay = EmitterData->GetSoundCueByElementaryType(EleType);
		if (SoundToPlay && GWorld->Scene)
		{
			UAudioComponent* AudioComponent = UAudioDevice::CreateComponent( SoundToPlay, GWorld->Scene, ProActor, 0 );
			if( AudioComponent )
			{
				AudioComponent->Location = ProActor->Location;
				AudioComponent->bUseOwnerLocation = TRUE;
				AudioComponent->bStopWhenOwnerDestroyed = TRUE;
				//AudioComponent->VolumeMultiplier		= 1.0;
				//AudioComponent->PitchMultiplier			= PitchMultiplier;
				AudioComponent->bAllowSpatialization	&= GIsGame;
				AudioComponent->bIsUISound				= FALSE;
				AudioComponent->bAutoDestroy			= 1;
				//AudioComponent->SubtitlePriority		= SUBTITLE_PRIORITY_ANIMNOTIFY;
				AudioComponent->Play();
			}
		}



		ProActor->BeginPlay(BaseActor,NULL, Target, TargetPoint, Rate);

		ProjecitleId = EffectProcessor::GetSingleton().AddClientProjectile(ProActor);


		// set liftspan

		if (EmitterData->StopType == ACBT_StopWhenConstDurationPass)
		{
			// fix life time
			if (EmitterData->EmitterType == EMIT_FlyTo)
			{
				if (EmitterData->TrackTimeType == ETTT_ConstTime)
				{
					ProActor->Duration = EmitterData->FlyTime;
				}
				else
				{
					// get dst point

					if (OverwriteTargetType == EPTT_Actor)
					{
						DstPoint = Target->Location;
					}
					else if (OverwriteTargetType == EPTT_TargetPoint)
					{
						DstPoint = TargetPoint;
					}
					else if (OverwriteTargetType == EPTT_UseCasterDirection)
					{
						DstPoint = ProActor->SpawnMatrix.GetOrigin() + ProActor->SpawnMatrix.Rotator().Vector().SafeNormal() * MaxDistance;
						if (EmitterData->bHaveCollisionCheck)
						{
							FVector StartPoint = ProActor->Location;
							FVector HitLocation, HitNormal;
							UBOOL bRet = MMOFacade::getSingleton().GetAbilityProcessor()->LineCheck(BaseActor, StartPoint, DstPoint, HitLocation, HitNormal);
							if (bRet)
							{
								DstPoint = HitLocation;
							}
						}
					}

					ProActor->Duration = (DstPoint - ProActor->SpawnMatrix.GetOrigin()).Size() / EmitterData->Speed;
				}
			}
			else
			{
				ProActor->Duration = EmitterData->Duration;
			}
		}
		else if (EmitterData->StopType == ACBT_StopWhenAbilityStateEnd)
		{
			ProActor->Duration = MAX_EFFECT_LIFE_SPAN;
		}
		else// if (EmitterData->StopType == ACBT_StopWhenAbilityEnd)
		{
			ProActor->Duration =  (EmitterData->Duration > MAX_EFFECT_LIFE_SPAN ? EmitterData->Duration : MAX_EFFECT_LIFE_SPAN);
		}

		//// debug code
		//// draw line track
		//if (EmitterData && EmitterData->EmitterType == EMIT_FlyTo && mmoActor)
		//{
		//	static UBOOL bDrawLine = TRUE;

		//	if (bDrawLine)
		//	{
		//		mmoActor->FlushPersistentDebugLines();
		//		if (EmitterData->TrackLineType == ETLT_Line)
		//		{
		//			mmoActor->DrawDebugLine(ProActor->SpawnMatrix.GetOrigin(), DstPoint, 255, 0, 0, TRUE);
		//		}
		//		else if(EmitterData->TrackLineType == ETLT_Bezier)
		//		{
		//			FVector P0 = FVector(0, 0, 0);
		//			FVector P1 = ProActor->BaseActorMatrix.InverseTransformFVector(TargetPoint);
		//			FVector T0 = EmitterData->BezierControlPoint1; // SpawnMatrix.TransformFVector(EmitterData->BezierControlPoint1);
		//			FVector T1 = P1 + EmitterData->BezierControlPoint2;
		//			for (FLOAT i = 0.f; i <= 1.0f; i += 0.001)
		//			{
		//				FVector NewPoint = CubicInterp<FVector, FLOAT>(P0, T0, P1, T1, i);
		//				NewPoint = ProActor->BaseActorMatrix.TransformFVector(NewPoint);
		//				mmoActor->DrawDebugPoint(NewPoint, 1, FLinearColor(1.0, 0.f, 0.f), TRUE);
		//			}
		//		}

		//	}
		//}

	}
	else
	{
		if (ProjecitleId > 0 && EmitterData->EmitterEndEffectList.Num() > 0)
		{
			AMMOEmitter* ProActor = Cast<AMMOEmitter>(UObject::GetIndexedObject(ProActorId));
			if (ProActor)
			{
				ProActor->RemoveFromRoot();

				for (INT i = 0; i < EmitterData->EmitterEndEffectList.Num(); ++i)
				{
					if (EmitterData->EmitterEndEffectList(i))
					{
						if (EmitterData->EmitterEndEffectList(i)->StopType != ACBT_StopWhenConstDurationPass)
						{
							EmitterData->EmitterEndEffectList(i)->StopType = ACBT_StopWhenConstDurationPass;
							EmitterData->EmitterEndEffectList(i)->Duration = 10.f;
						}

						if (EmitterData->EmitterEndEffectList(i)->Duration > 60.f)
						{
							EmitterData->EmitterEndEffectList(i)->Duration = 60.f;
						}

						BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(EmitterData->EmitterEndEffectList(i), FALSE,CreatorId,  ProActorId, 0, TargetPoint, 1.0);
						if (NewNode)
						{
							NewNode->SetUseEngineObjectId(TRUE);
							EffectProcessor::GetSingleton().AddActiveComponent(NewNode);
							NewNode->OnAction(TRUE);
						}
					}
				}

			}
		}
		EffectProcessor::GetSingleton().DeleteClientProjectile(ProjecitleId);
	}
}

UBOOL SkeletalMeshComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_SkeletalMesh;

	MeshData = Cast<UAbilitySkeletalMeshComponent>(InData.ComponentData);
	if (!MeshData)
	{
		return FALSE;
	}

	ActorIdx = -1;

	return TRUE;
}

AActor* SkeletalMeshComponent::GetBaseActor()
{
	if (!MeshData)
	{
		return NULL;
	}

	if (MeshData->SourceType == EST_TargetActor)
	{
		return BaseComponent::GetTargetActor();
	}
	else
	{
		return BaseComponent::GetBaseActor();
	}
}

AActor* SkeletalMeshComponent::GetTargetActor()
{
	if (!MeshData)
	{
		return NULL;
	}

	if (MeshData->SourceType == EST_TargetActor)
	{
		return BaseComponent::GetBaseActor();
	}
	else
	{
		return BaseComponent::GetTargetActor();
	}
}

void SkeletalMeshComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	if (bOn)
	{
		AActor* BaseActor = GetBaseActor();
		if (!BaseActor)
		{
			OnAction(FALSE);
			return;
		}

		if (MeshData->SkeletalMeshRes == NULL)
		{
			OnAction(FALSE);
			return;
		}

		if (MeshData->AnimSet == NULL)
		{
			OnAction(FALSE);
			return;
		}

		AAbilitySkeletalMeshActor* NewActor = Cast<AAbilitySkeletalMeshActor>(GWorld->SpawnActor(AAbilitySkeletalMeshActor::StaticClass()));
		if (!NewActor)
		{
			OnAction(FALSE);
			return;
		}

		NewActor->SetDrawScale(MeshData->Scale);

		NewActor->InstanceId = this->GetId();
		NewActor->LifeSpan = MAX_EFFECT_LIFE_SPAN;

		ActorIdx = NewActor->GetIndex();

		USkeletalMeshComponent* NewComponent = NewActor->SkeletalMeshComponent;
		if (!NewComponent)
		{
			OnAction(FALSE);
			return;
		}

		NewComponent->SetSkeletalMesh(MeshData->SkeletalMeshRes);
		NewComponent->SetAnimTreeTemplate(MeshData->AnimTreeTemplate);
		NewComponent->AnimSets.AddItem(MeshData->AnimSet);

		AMMOActor* mmoBase = GetBaseCharacter();
		if (mmoBase && mmoBase->Mesh)
		{
			NewComponent->SetLightingChannels(mmoBase->Mesh->LightingChannels);
			NewComponent->SetLightEnvironment(mmoBase->Mesh->LightEnvironment);
		}

		NewActor->AttachComponent(NewComponent);

		UAnimNodeBlend* ActionLoopBlendNode = Cast<UAnimNodeBlend>(NewComponent->FindAnimNode(TEXT("WeaponActionLoopBlendNode")));
		if (!ActionLoopBlendNode)
		{
			OnAction(FALSE);
			return;
		}

		UAnimNodeSequence* ActionNode = Cast<UAnimNodeSequence>(NewComponent->FindAnimNode(TEXT("Action")));
		if (!ActionNode)
		{
			OnAction(FALSE);
			return;
		}

		ActionLoopBlendNode->SetBlendTarget(0.f, 0.f);
		ActionNode->SetAnim(MeshData->AnimName);
		
		FLOAT StartTime = FLOAT(MeshData->StartFrame) / 30.f;
		ActionNode->PlayAnim(MeshData->bLoop, MeshData->Rate, StartTime);
		NewComponent->ForceSkelUpdate();

		for (INT i = 0; i < MeshData->Materials.Num(); ++i)
		{
			UMaterialInterface* NewMat = MeshData->Materials(i);
			if (MeshData->Materials(i) && MeshData->Materials(i)->IsA(UMaterialInstanceTimeVarying::StaticClass()))
			{
				UMaterialInstanceTimeVarying* inst = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass());
				inst->SetParent(NewMat);
				inst->SetAllParametersStartTime(0.f);
				NewMat = inst;
			}
			NewComponent->SetMaterial(i, NewMat);
		}

		FMatrix SpawnMat = BaseActor->LocalToWorld();
		if (BaseActor->IsA(AMMOActor::StaticClass()))
		{
			AMMOActor* mmoActor = (AMMOActor*)BaseActor;
			USkeletalMeshComponent* WeaponMesh1 = NULL;
			USkeletalMeshComponent* WeaponMesh2 = NULL;
			mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
			SpawnMat = MeshData->GetSpawnMatrix(mmoActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, MeshData->AttachSocketName, MeshData->SourceType, MeshData->RelativeLocation, MeshData->RelativeRotation);
		}
		else
		{
			FRotationTranslationMatrix frtm(MeshData->RelativeRotation, MeshData->RelativeLocation);
			SpawnMat = frtm * SpawnMat;
		}

		NewActor->SetLocation(SpawnMat.GetOrigin());
		NewActor->SetRotation(SpawnMat.Rotator());
	}
	else
	{
		AAbilitySkeletalMeshActor* LastActor = Cast<AAbilitySkeletalMeshActor>(UObject::GetIndexedObject(ActorIdx));
		if (LastActor && LastActor->InstanceId == GetId())
		{
			if(!LastActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
			{
				GWorld->DestroyActor(LastActor);
			}
		}
	}
}

void SkeletalMeshComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("AbilitySkeletalMeshComponent");

	BaseComponent::Tick(DeltaTime);

	if (bStopped)
	{
		return;
	}


	AAbilitySkeletalMeshActor* Actor = Cast<AAbilitySkeletalMeshActor>(UObject::GetIndexedObject(ActorIdx));
	if (Actor && Actor->InstanceId == GetId())
	{
		if (MeshData->bAttach)
		{
			AActor* BaseActor = MMOFacade::getSingleton().GetActor(BaseActorId);
			if (!BaseActor)
			{
				OnAction(FALSE);
				return;
			}

			FMatrix SpawnMat = BaseActor->LocalToWorld();
			if (BaseActor->IsA(AMMOActor::StaticClass()))
			{
				AMMOActor* mmoActor = (AMMOActor*)BaseActor;
				USkeletalMeshComponent* WeaponMesh1 = NULL;
				USkeletalMeshComponent* WeaponMesh2 = NULL;
				mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
				SpawnMat = MeshData->GetSpawnMatrix(mmoActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, MeshData->AttachSocketName, MeshData->SourceType, MeshData->RelativeLocation, MeshData->RelativeRotation);
			}

			Actor->SetLocation(SpawnMat.GetOrigin());
			Actor->SetRotation(SpawnMat.Rotator());
		}
	}
}



UBOOL StaticMeshComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_StaticMesh;

	MeshData = Cast<UAbilityStaticMeshComponent>(InData.ComponentData);
	if (!MeshData)
	{
		return FALSE;
	}

	ActorIdx = -1;

	return TRUE;
}

AActor* StaticMeshComponent::GetBaseActor()
{
	if (!MeshData)
	{
		return NULL;
	}

	if (MeshData->SourceType == EST_TargetActor)
	{
		return BaseComponent::GetTargetActor();
	}
	else
	{
		return BaseComponent::GetBaseActor();
	}
}

AActor* StaticMeshComponent::GetTargetActor()
{
	if (!MeshData)
	{
		return NULL;
	}

	if (MeshData->SourceType == EST_TargetActor)
	{
		return BaseComponent::GetBaseActor();
	}
	else
	{
		return BaseComponent::GetTargetActor();
	}
}

void StaticMeshComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	if (bOn)
	{
		AActor* BaseActor = GetBaseActor();
		if (!BaseActor)
		{
			OnAction(FALSE);
			return;
		}

		if (MeshData->MeshRes == NULL)
		{
			OnAction(FALSE);
			return;
		}

		if (MeshData->bVisibleToEveryone == FALSE && BaseActor != MMOFacade::getSingleton().GetMainActor())
		{
			OnAction(FALSE);
			return;
		}

		AAbilityStaticMeshActor* NewActor = Cast<AAbilityStaticMeshActor>(GWorld->SpawnActor(AAbilityStaticMeshActor::StaticClass()));
		if (!NewActor)
		{
			OnAction(FALSE);
			return;
		}

		NewActor->MeshData = MeshData;
		NewActor->CurrentMoveTime = 0.f;
		NewActor->BaseActor = BaseActor;
		NewActor->TargetPoint = TargetPoint;

		NewActor->SetDrawScale(MeshData->Scale);

		NewActor->InstanceId = this->GetId();
		NewActor->LifeSpan = MAX_EFFECT_LIFE_SPAN;

		ActorIdx = NewActor->GetIndex();

		UStaticMeshComponent* NewComponent = NewActor->MainStaticMeshComponent;
		if (!NewComponent)
		{
			OnAction(FALSE);
			return;
		}

		NewComponent->SetStaticMesh(MeshData->MeshRes);

		AMMOActor* mmoBase = GetBaseCharacter();
		if (mmoBase && mmoBase->Mesh)
		{
			NewComponent->SetLightingChannels(mmoBase->Mesh->LightingChannels);
			NewComponent->SetLightEnvironment(mmoBase->Mesh->LightEnvironment);
		}

		for (INT i = 0; i < MeshData->OverrideMaterials.Num(); ++i)
		{
			//UMaterialInterface* NewMat = MeshData->OverrideMaterials(i);
			//if (MeshData->OverrideMaterials(i) && MeshData->OverrideMaterials(i)->IsA(UMaterialInstanceTimeVarying::StaticClass()))
			//{
			//	UMaterialInstanceTimeVarying* inst = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass());
			//	inst->SetParent(NewMat);
			//	inst->SetAllParametersStartTime(0.f);
			//	NewMat = inst;
			//}
			//NewComponent->SetMaterial(i, NewMat);

			if (MeshData->OverrideMaterials(i))
			{
				NewComponent->SetMaterial(i, MeshData->OverrideMaterials(i));
			}
		}

		INT Num = NewComponent->GetNumElements();
		for (INT i = 0; i < Num; ++i)
		{
			UMaterialInstance* umi = Cast<UMaterialInstance>(NewComponent->GetMaterial(i));
			if (umi)
			{
				UMaterialInstance* ToUseMat = umi;
				if (MeshData->bUseForShowEffectArea)
				{
					if (umi->IsA(UMaterialInstanceTimeVarying::StaticClass()))
					{
						ToUseMat = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass());
					}
					else
					{
						ToUseMat = ConstructObject<UMaterialInstanceConstant>(UMaterialInstanceConstant::StaticClass());
					}

					ToUseMat->SetParent(umi);

					if (MeshData->Parameter1Name != NAME_None)
					{
						ToUseMat->SetScalarParameterValue(MeshData->Parameter1Name, MeshData->Parameter1Value);
					}

					FMatrix WorldToLocal = NewComponent->LocalToWorld.Inverse();
					FBoxSphereBounds OriginalBound = NewComponent->Bounds.TransformBy(WorldToLocal);
					FLOAT Radius = OriginalBound.SphereRadius * 0.70710678;
					if (Radius <= 0)
					{
						Radius = 1;
					}
					ToUseMat->SetScalarParameterValue(FName(TEXT("Radius")), Radius);

					if (MeshData->Parameter2Name != NAME_None)
					{
						FLOAT Parameter2Value = MeshData->Parameter2Value;

						FVector ScaleVector = WorldToLocal.GetScaleVector();
						Parameter2Value = Parameter2Value * abs(ScaleVector.Y);

						if (MeshData->Parameter1Value <1)
						{
							Parameter2Value = Parameter2Value / Radius;
						}
						else if (MeshData->Parameter1Value < 2)
						{

						}
						else if (MeshData->Parameter1Value < 3)
						{
							Parameter2Value = Parameter2Value / Radius;
						}
						else if (MeshData->Parameter1Value < 4)
						{
							Parameter2Value = MeshData->Parameter2Value * 3.1415926 * 2.0 / 360.0;
						}
						ToUseMat->SetScalarParameterValue(MeshData->Parameter2Name, Parameter2Value);
					}
				}
				NewComponent->SetMaterial(i, ToUseMat);
			}
		}

		FMatrix SpawnMat = BaseActor->LocalToWorld();
		if (BaseActor->IsA(AMMOActor::StaticClass()))
		{
			AMMOActor* mmoActor = (AMMOActor*)BaseActor;
			USkeletalMeshComponent* WeaponMesh1 = NULL;
			USkeletalMeshComponent* WeaponMesh2 = NULL;
			mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
			SpawnMat = MeshData->GetSpawnMatrix(mmoActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, MeshData->AttachSocketName, MeshData->SourceType, MeshData->RelativeLocation, MeshData->RelativeRotation);
		}
		else
		{
			FRotationTranslationMatrix frtm(MeshData->RelativeRotation, MeshData->RelativeLocation);
			SpawnMat = frtm * SpawnMat;
		}

		NewActor->SetLocation(SpawnMat.GetOrigin());
		NewActor->SetRotation(SpawnMat.Rotator());

		if (MeshData->MoveTime > 0)
		{
			NewActor->MoveSpeed = SpawnMat.TransformFVector(MeshData->MoveSpeed);
		}
	}
	else
	{
		AAbilityStaticMeshActor* LastActor = Cast<AAbilityStaticMeshActor>(UObject::GetIndexedObject(ActorIdx));
		if (LastActor && LastActor->InstanceId == GetId())
		{
			if(!LastActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
			{
				GWorld->DestroyActor(LastActor);
			}
		}
	}
}

void AAbilityStaticMeshActor::TickSpecial( FLOAT DeltaSeconds )
{
	AActor::TickSpecial(DeltaSeconds);

	if (MeshData->bAttach)
	{
		if (BaseActor == NULL)
		{
			LifeSpan = 0.1f;
			return;
		}

		FMatrix SpawnMat = BaseActor->LocalToWorld();
		if (BaseActor->IsA(AMMOActor::StaticClass()))
		{
			AMMOActor* mmoActor = (AMMOActor*)BaseActor;
			USkeletalMeshComponent* WeaponMesh1 = NULL;
			USkeletalMeshComponent* WeaponMesh2 = NULL;
			mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
			SpawnMat = MeshData->GetSpawnMatrix(mmoActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, MeshData->AttachSocketName, MeshData->SourceType, MeshData->RelativeLocation, MeshData->RelativeRotation);
		}
		else
		{
			FRotationTranslationMatrix frtm(MeshData->RelativeRotation, MeshData->RelativeLocation);
			SpawnMat = frtm * SpawnMat;
		}

		SetLocation(SpawnMat.GetOrigin());
		SetRotation(SpawnMat.Rotator());
	}
	else
	{
		if (CurrentMoveTime < MeshData->MoveTime)
		{
			FVector NewLocation = Location + MoveSpeed * DeltaSeconds;
			SetLocation(NewLocation);
		}
	}

	if (CurrentMoveTime < MeshData->RotatoTime)
	{
		FRotator NewRotator = Rotation + MeshData->RotatoSpeed * DeltaSeconds;
		SetRotation(NewRotator);
	}

	if (CurrentMoveTime < MeshData->ScaleTime)
	{
		FVector NewScale = DrawScale3D + MeshData->ScaleSpeed * DeltaSeconds;
		SetDrawScale3D(NewScale);
	}

	CurrentMoveTime += DeltaSeconds;
}




UBOOL DecalComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Decal;

	DecalData = Cast<UAbilityDecalComponent>(InData.ComponentData);
	if (!DecalData)
	{
		return FALSE;
	}

	return TRUE;
}

FLOAT testValue = 0;

void DecalComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	if (bOn)
	{
		AActor* BaseActor = GetBaseActor();

		if (!BaseActor || BaseActor->IsPendingKill())
		{
			return;
		}

		if (DecalData->ImpactDecalMaterials.Num() == 0)
		{
			return;
		}

		UMaterialInterface* Mat = DecalData->ImpactDecalMaterials(RandHelper(DecalData->ImpactDecalMaterials.Num()));
		if (!Mat)
		{
			return;
		}

		// oscar@snda
		if(g_disableBloodyContent && Mat->IsBloodyContents())
		{
			return;
		}
		// oscar@snda

		UMaterialInstanceTimeVarying* MatToUse = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass());
		if (!MatToUse)
		{
			return;
		}

		MatToUse->SetParent(Mat);
		MatToUse->SetLinearColorParameterCycleTime(FName(TEXT("DecalAlpha")), DecalData->Duration);


		FLOAT Yaw = appFrand() * 65536.f;
		if (!DecalData->bUseRandomRotation)
		{
			Yaw = FLOAT(INT(DecalData->Rotation) % 360) * 65536.f / 360.f;
		}

		FRotationTranslationMatrix RelativeMat(FRotator(0, Yaw, 0), DecalData->RelativeLocation);
		FMatrix WorldMat = RelativeMat * FRotationTranslationMatrix(BaseActor->Rotation, BaseActor->Location);

		FVector Point = WorldMat.GetOrigin();
		FVector StartPoint = Point + FVector(0, 0, 100);
		FVector EndPoint = Point + FVector(0, 0, -500);

		FCheckResult Hit(1.0);
		if (GWorld->SingleLineCheck(Hit, BaseActor, EndPoint, StartPoint, TRACE_World, FVector(0.f)) == TRUE)
		{
			return;
		}

		Point = Hit.Location;
		FVector HitNormal = Hit.Normal;

		FLOAT R = appFrand();
		FLOAT Size = DecalData->ImpactDecalSizeMin * R + DecalData->ImpactDecalSizeMax * (1 - R);
		FRotator Rot = FVector(0, 0, -1).Rotation();
		//FRotator Rot = (-HitNormal).Rotation();


		FLOAT Dir = 0xffff - INT(WorldMat.Rotator().Yaw) & 0xffff;
		//Dir = WorldMat.Rotator().Yaw & 0xffff;
		//FLOAT Dir = INT(BaseActor->Rotation.Yaw) & 0xffff;

		Dir = 360.f * Dir / 65536.f;
		

		FLOAT Scale = 1.0f;
		if (DecalData->bScaleByStandardCollision)
		{
			AMMOActor* mmoActor = Cast<AMMOActor>(BaseActor);
			if (mmoActor)
			{
				Scale *= mmoActor->CollisionRadiusRate;
			}
		}
		FLOAT SizeX = Size * DecalData->ScaleX * Scale;
		FLOAT SizeY = Size * DecalData->ScaleY * Scale;

		//debugf(L"Decal dir =%f", Dir);

		//BaseActor->DrawDebugLine(BaseActor->Location, FVector(BaseActor->Location.Z + 1000, BaseActor->Location.Y, BaseActor->Location.Z), 255, 0, 0, FALSE);
		DecalData->eventSpawnDecal(BaseActor, MatToUse, Point, Rot, SizeX, SizeY, DecalData->ImpactDecalDepth, TRUE, Dir, NULL);
	}
	else
	{
		//
	}
}


UBOOL CameraAnimComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_CameraAnim;

	CameraAnimData = Cast<UCameraShakeComponent>(InData.ComponentData);
	if (!CameraAnimData)
	{
		return FALSE;
	}

	return TRUE;

}
void CameraAnimComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

#if !SHIPPING_PC_GAME
	// This feature is not wanted when recording video
	if(GIsBenchmarking)
	{
		return;
	}
#endif

	if (bOn)
	{
		AActor* BaseActor = MMOFacade::getSingleton().GetActor(BaseActorId);
		AMMOActor* mmoBaseActor = Cast<AMMOActor>(BaseActor);
		AMMOActor* TargetActor = MMOFacade::getSingleton().GetCharacter(TargetId);

		if (!BaseActor)
		{
			bStopped = TRUE;
			return;
		}

		if (MMOUI::GetUIManager()->SkillEffectHideCameraShake)
		{
			bStopped = TRUE;
			return;
		}

		UBOOL bNeedPlay = FALSE;

		bPlayed = FALSE;

		if (CameraAnimData->bInHitPoint)
		{
			if (TargetActor && TargetActor->IsMainPlayer())
			{
				bNeedPlay = TRUE;
			}
		}
		else
		{
			if (mmoBaseActor && mmoBaseActor->IsMainPlayer())
			{
				bNeedPlay = TRUE;
			}
		}

		if (CameraAnimData->Space == CAPS_UserDefined)
		{
			CameraAnimData->CustomPlaySpace = BaseActor->Rotation;
		}

		FLOAT InScale = 1.0f;
		if (bNeedPlay || (!bNeedPlay && CameraAnimData->bPlayInThirdPersonView))
		{
			if (!bNeedPlay)
			{
				FVector offst = MMOFacade::getSingleton().GetMainActor() ? MMOFacade::getSingleton().GetMainActor()->Location : FVector(0, 0, 0);
				FVector Dst = MMOFacade::getSingleton().GetMainController() ? (MMOFacade::getSingleton().GetMainController()->Location +  offst) * 0.5f : FVector(0, 0, 0);
				FLOAT dis = (Dst - BaseActor->Location).Size();
				if (dis <= CameraAnimData->MaxFadeDistance)
				{
					CameraAnimData->eventShake(MMOFacade::getSingleton().GetMainController(), 1.0);
					bPlayed = TRUE;
				}
			}
			else
			{
				CameraAnimData->eventShake(MMOFacade::getSingleton().GetMainController(), 1.0);
				bPlayed = TRUE;
			}
		}

	}
	else
	{
		if (bPlayed)
		{
			//CameraAnimData->eventStopShake(MMOFacade::getSingleton().GetMainController());
		}
	}
}

UBOOL MaterialEffectComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_MaterialEffect;

	MaterialEffectData = Cast<UAbilityComp_MaterialEffect>(InData.ComponentData);
	if (!MaterialEffectData)
	{
		return FALSE;
	}

	return TRUE;

}
void MaterialEffectComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		bStopped = TRUE;
		return;
	}

	if (bOn)
	{
		EffectId = GetNewId();
	}

	TArray<UMeshComponent*> ToProcessComponents;

	AMMOActor* ToApplyTarget = BaseActor;
	if (MaterialEffectData->SourceType == EST_TargetActor)
	{
		ToApplyTarget = GetTargetCharacter();
	}

	if (MaterialEffectData->SourceType == EST_BaseActor || MaterialEffectData->SourceType == EST_TargetActor)
	{
		if (ToApplyTarget == NULL)
		{
			return;
		}

		for (INT i = 0; i < ToApplyTarget->Components.Num(); ++i)
		{
			if (ToApplyTarget->Components(i) == NULL)
			{
				continue;
			}

			if (ToApplyTarget->Components(i)->IsA(UMeshComponent::StaticClass()) == FALSE)
			{
				continue;
			}

			ToProcessComponents.AddItem((UMeshComponent*)ToApplyTarget->Components(i));

			if (ToApplyTarget->Components(i)->IsA(USkeletalMeshComponent::StaticClass()))
			{
				USkeletalMeshComponent* smc = (USkeletalMeshComponent*)(ToApplyTarget->Components(i));
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
	}
	else if (MaterialEffectData->SourceType == EST_BaseWeapon1 || MaterialEffectData->SourceType == EST_BaseWeapon2)
	{
		EBodyMeshType type = (MaterialEffectData->SourceType == EST_BaseWeapon1) ? EBMT_Weapon : EBMT_Weapon2;
		AArmorActor* aaa = (AArmorActor*)(ToApplyTarget->GetEquip(type));
		if (aaa)
		{
			ToProcessComponents.AddItem(aaa->MainMesh);
		}
	}
	else
	{
		return;
	}


	UMaterialInstanceTimeVaryingEffectNode* TimeVaryingEffectData = NULL;
	if (bOn && MaterialEffectData->EffectType == EMET_ChangeMaterialProperty)
	{
		TimeVaryingEffectData = ToApplyTarget->GpDataManager->GetMaterialEffectData(MaterialEffectData->EffectTemplateId);
	}


	for (INT i = 0; i < ToProcessComponents.Num(); ++i)
	{
		if (ToProcessComponents(i))
		{
			if (bOn)
			{
				if (MaterialEffectData->EffectType == EMET_ChangeMaterialProperty)
				{
					if (TimeVaryingEffectData)
					{
						ToProcessComponents(i)->ChangeMaterialProperties(EffectId, TimeVaryingEffectData);
					}
				}
				else if (MaterialEffectData->EffectType == EMET_LoadOpacityMaterial)
				{
					ToProcessComponents(i)->SwitchToTransparentMaterial(EffectId);
				}
				else if (MaterialEffectData->EffectType == EMET_ChangeMaterial)
				{
					ToProcessComponents(i)->ChangeMaterial(EffectId,
						MaterialEffectData->EffectMaterialInstance, 
						MaterialEffectData->CanBeDeleteWhenSetNewMaterial,
						MaterialEffectData->Priority);
				}
			}
			else
			{
				ToProcessComponents(i)->CloseEffect(EffectId);
			}
		}
	}
}
void MaterialEffectComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("MaterialEffectComponent");
	BaseComponent::Tick(DeltaTime);
}

UBOOL WindSourceComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_WindSource;

	WindSourceData = Cast<UAbilityWindSourceComponent>(InData.ComponentData);
	if (!WindSourceData)
	{
		return FALSE;
	}

	WindPointSourceComponent = NULL;

	ActorIdx = 0;

	return TRUE;
}

void WindSourceComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AActor* BaseActor = GetBaseActor();
	if (!BaseActor)
	{
		bStopped = TRUE;
		return;
	}

	if (WindSourceData->WindComponentTemplate == NULL)
	{
		bStopped = TRUE;
		return;
	}

	if (bOn)
	{
		AAbilityWindSourceActor* NewActor = Cast<AAbilityWindSourceActor>(GWorld->SpawnActor(AAbilityWindSourceActor::StaticClass()));
		check(NewActor);
		if (NewActor == NULL)
		{
			return ;
		}
		ActorIdx = NewActor->GetIndex();

		UWindPointSourceComponent* NewComponent = ConstructObject<UWindPointSourceComponent>(UWindPointSourceComponent::StaticClass(), NewActor, NAME_None, 0, WindSourceData->WindComponentTemplate);
		check(NewComponent);

		if (NewComponent == NULL)
		{
			return ;
		}

		NewActor->AttachComponent(NewComponent);

		FMatrix SpawnMat = BaseActor->LocalToWorld();
		if (BaseActor->IsA(AMMOActor::StaticClass()))
		{
			AMMOActor* mmoActor = (AMMOActor*)BaseActor;
			USkeletalMeshComponent* WeaponMesh1 = NULL;
			USkeletalMeshComponent* WeaponMesh2 = NULL;
			mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
			SpawnMat = WindSourceData->GetSpawnMatrix(mmoActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, WindSourceData->AttachSocketName, WindSourceData->SourceType, WindSourceData->RelativeLocation, WindSourceData->RelativeRotation);
		}
		else
		{
			FRotationTranslationMatrix frtm(WindSourceData->RelativeRotation, WindSourceData->RelativeLocation);
			SpawnMat = frtm * SpawnMat;
		}

		NewActor->SetLocation(SpawnMat.GetOrigin());
		NewActor->SetRotation(SpawnMat.Rotator());

		//BaseActor->FlushPersistentDebugLines();
		//BaseActor->DrawDebugPoint(SpawnMat.GetOrigin(), 5, FLinearColor(1.0, 0, 0), TRUE);

		if (WindSourceData->WindComponentTemplate->IsBlast)
		{
			NewComponent->TriggerBlast();
		}

		NewActor->InstanceId = GetId();
	}
	else
	{
		AAbilityWindSourceActor* LastActor = Cast<AAbilityWindSourceActor>(UObject::GetIndexedObject(ActorIdx));
		if (LastActor && LastActor->InstanceId == GetId() && !LastActor->IsPendingKill())
		{
			if(!LastActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
			{
				GWorld->DestroyActor(LastActor);
			}
		}
	}
}

void WindSourceComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("AbilityWindSourceComponent");

	BaseComponent::Tick(DeltaTime);

	if (bStopped)
	{
		return;
	}

	AAbilityWindSourceActor* Actor = Cast<AAbilityWindSourceActor>(UObject::GetIndexedObject(ActorIdx));
	if (Actor && Actor->InstanceId == GetId() && !Actor->IsPendingKill())
	{
		if (WindSourceData->bAttach)
		{
			AActor* BaseActor = GetBaseActor();
			if (!BaseActor)
			{
				OnAction(FALSE);
				return;
			}

			FMatrix SpawnMat = BaseActor->LocalToWorld();
			if (BaseActor->IsA(AMMOActor::StaticClass()))
			{
				AMMOActor* mmoActor = (AMMOActor*)BaseActor;
				USkeletalMeshComponent* WeaponMesh1 = NULL;
				USkeletalMeshComponent* WeaponMesh2 = NULL;
				mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
				SpawnMat = WindSourceData->GetSpawnMatrix(mmoActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, WindSourceData->AttachSocketName, WindSourceData->SourceType, WindSourceData->RelativeLocation, WindSourceData->RelativeRotation);
			}

			Actor->SetLocation(SpawnMat.GetOrigin());
			Actor->SetRotation(SpawnMat.Rotator());
			//BaseActor->DrawDebugPoint(SpawnMat.GetOrigin(), 5, FLinearColor(1.0, 0, 0), TRUE);
		}
	}
}

UBOOL TimeFactorComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_TimeFactor;

	TimeFactorData = Cast<UAbilityTimeFactorComponent>(InData.ComponentData);
	if (!TimeFactorData)
	{
		return FALSE;
	}

	//TimeFactorData->StopType = ACBT_StopWhenConstDurationPass; 

	return TRUE;
}

void TimeFactorComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AMMOActor* BaseActor = MMOFacade::getSingleton().GetCharacter(BaseActorId);
	if (!BaseActor)
	{
		bStopped = TRUE;
		return;
	}

	if (bOn)
	{
		if (this->TimeFactorData->Type == ETFT_SpeedDown)
		{
			if (BaseActor == MMOFacade::getSingleton().GetMainActor())
			{
				MMOFacade::getSingleton().GetMainController()->SetTimeFactor(TimeFactorData->Type, TimeFactorData->FactorValue, TimeFactorData->Duration);
			}
		}
		else if (TimeFactorData->Type == ETFT_PauseAnim)
		{
			BaseActor->eventPauseAnim(TRUE);
		}
		else
		{

		}
	}
	else
	{
		if (this->TimeFactorData->Type == ETFT_SpeedDown)
		{

		}
		else if (TimeFactorData->Type == ETFT_PauseAnim)
		{
			BaseActor->eventPauseAnim(FALSE);
		}
		else
		{

		}
	}
}

void TimeFactorComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("TimeFactorComponent");

	BaseComponent::Tick(DeltaTime);
}

UBOOL PhysicHitComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_PhysicHit;

	PhysicHitData = Cast<UAbilityHitEffectComponent>(InData.ComponentData);
	if (!PhysicHitData)
	{
		return FALSE;
	}

	return TRUE;
}

void PhysicHitComponent::OnAction(UBOOL bOn /* = TRUE */)
{
	BaseComponent::OnAction(bOn);

	if (bOn)
	{
		// TargetActor  --->>> BaseActor
		AMMOActor* BaseActor = MMOFacade::getSingleton().GetCharacter(BaseActorId);
		AActor* TargetActor = MMOFacade::getSingleton().GetActor(TargetId);

		if (!BaseActor || !BaseActor->Mesh || !BaseActor->Mesh->PhysicsAssetInstance)
		{
			//warnf(TEXT("Please add PhysicsSimulateComponent first"));

			bStopped = TRUE;
			return;
		}

		if (TargetActor == NULL)
		{
			FVector AppleyPoint = BaseActor->Location - FVector(0, 0, BaseActor->GetCollisionHeight());
			BaseActor->Mesh->AddRadialImpulse(AppleyPoint, 1000, 2000, RIF_Linear);
		}
		else
		{
			FVector ApplyPoint, Momentum;

			Momentum = PhysicHitData->eventGetMomentum(BaseActor, TargetActor);

			FLOAT height = BaseActor->GetCollisionHeight();

			ApplyPoint = BaseActor->Location;
			if (PhysicHitData->PositionType == HPT_Center)
			{
				ApplyPoint = BaseActor->Location;
			}
			else if (PhysicHitData->PositionType == HPT_Up)
			{
				ApplyPoint = FVector(ApplyPoint.X, ApplyPoint.Y, ApplyPoint.Z + height * 0.75);
			}
			else //if (PhysicHitData->PositionType == HPT_Down)
			{
				ApplyPoint = FVector(ApplyPoint.X, ApplyPoint.Y, ApplyPoint.Z - height * 0.75);
			}

			if (BaseActor->UseingAvatar)
			{
				Momentum *= (BaseActor->UseingAvatar->eventGetWeightScale());
			}

			// As long as the designer configured it as light, it is light no matter how heavy; multiply by the original mass here, i.e. ignore the effect of the original mass
			Momentum *= BaseActor->Mesh->PhysicsAssetInstance->BodiesTotalMass;
			BaseActor->Mesh->AddImpulse(Momentum, ApplyPoint);
		}
	}
	else
	{
		//noting
	}
}


UBOOL HitPointComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_HitPoint;

	HitPointData = Cast<UAbilityHitPointComponent>(InData.ComponentData);
	if (!HitPointData)
	{
		return FALSE;
	}

	return TRUE;
}

void HitPointComponent::OnAction(UBOOL bOn /* = TRUE */)
{
	BaseComponent::OnAction(bOn);

	if (bOn)
	{
		AMMOActor* BaseActor = GetBaseCharacter();
		if (!BaseActor)
		{
			return;
		}

		if (HitPointData->ClientEffectList.Num() == 0)
		{
			return;
		}

		if (HitPointData->DamageValue < 0.00001)
		{
			return;
		}

		UClass* BaseClass = AActor::StaticClass();
		UClass* InterfaceClass = UIAbilityDamageable::StaticClass();
		FMemMark Mark(GMainThreadMemStack);
		FCheckResult* Link = GWorld->Hash->ActorRadiusCheck( GMainThreadMemStack, BaseActor->Location, HitPointData->Radius, TRUE );

		DWORD TraceFlags = TRACE_World|TRACE_ProjTargets;

		AActor* OutActor = NULL;
		FCheckResult Hit(1.f);
		FDamageData DamageData;
		DamageData.AbilityData = HitPointData;
		DamageData.BaseActor = BaseActor;
		DamageData.CreatorId = CreatorId;
		DamageData.bServerDamageData = FALSE;
		DamageData.DamageValue = HitPointData->DamageValue;
		DamageData.ElementaryType = 0;

		if ( Link )
		{
			while ( Link )
			{
				if( !Link->Actor ||  
					Link->Actor == BaseActor ||
					!Link->Actor->bCollideActors ||	
					Link->Actor->bDeleteMe ||
					!Link->Actor->IsA(BaseClass) ||
					(InterfaceClass != NULL && !Link->Actor->GetClass()->ImplementsInterface(InterfaceClass))
					)
				{
					Link = Link->GetNext();
				}
				else
				{
					UBOOL bSkip = FALSE;
					if (Link->Actor->IsA(AMMOActor::StaticClass()))
					{
						AMMOActor* actor = Cast<AMMOActor>(Link->Actor);
						if(actor && !actor->bDead)
						{
							bSkip = TRUE;
						}
					}
					//Dropped items do not accept ability physics effects
					if (Link->Actor->IsA(AMMODropActor::StaticClass()))
					{
						bSkip = TRUE;
					}


					FVector A = (Link->Actor->Location - BaseActor->Location).SafeNormal();
					FVector B = BaseActor->Rotation.Vector().SafeNormal();
					FLOAT cf = (A | B) / (A.Size() * B.Size());
					FLOAT lcf = appCos(HitPointData->Degree * PI / 360.f);
					if (cf < lcf)
					{
						bSkip = TRUE;
					}


					if (!bSkip)
					{
						IIAbilityDamageable* itarget = InterfaceCast<IIAbilityDamageable>(Link->Actor);
						//debugf(L"CalculateClientEffect");
						itarget->eventProcessDamage(DamageData);
					}
					Link = Link->GetNext();
				}
			}
		}

		Mark.Pop();

	}
	else
	{
		// nothing
	}
}

UBOOL TrailComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Trail;

	TrailData = Cast<UAbilityTrailComponent>(InData.ComponentData);
	if (!TrailData)
	{
		return FALSE;
	}

	TrailNotify = NULL;

	return TRUE;
}

void TrailComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		return;
	}


	if (bOn)
	{
		Seq = BaseActor->GetActiveAnimSequence();
		if (!Seq)
		{
			return;
		}

		FLOAT StartTime = Seq->CurrentTime;
		FLOAT EndTime = StartTime + TrailData->Duration * Seq->Rate;


		UAnimNotify_Trails* TrailNotifyTemplate = TrailData->GetTrailData(Seq);
		//UAnimNotify_Trails* TrailNotifyTemplate = TrailData->AddNewTrail(Seq, StartTime, EndTime);
		
		if (TrailNotifyTemplate == NULL)
		{
#if SHOW_TRAIL_LOG
			debugf(L"Build TrailData## StartTime=%f, EndTime=%f", StartTime, EndTime);
#endif
			TrailData->AddNewTrail(Seq, StartTime, EndTime);
			TrailNotifyTemplate = TrailData->GetTrailData(Seq);
		}

		if (!TrailNotifyTemplate)
		{
			return;
		}

		//TrailNotify  =  Cast<UAnimNotify_Trails>(UObject::StaticConstructObject(UAnimNotify_Trails::StaticClass(),
		//	UObject::GetTransientPackage(), NAME_None, 0, TrailNotifyTemplate));

		TrailNotify = TrailNotifyTemplate;

		if (TrailNotify)
		{
#if SHOW_TRAIL_LOG
			debugf(L"*****Trail On %s %f", *(Seq->AnimSeqName.ToString()), Seq->CurrentTime);
			BaseActor->FlushPersistentDebugLines();
			bFirstTime = TRUE;
#endif
			//TrailNotify->LastStartTime = Seq->CurrentTime;
			TrailNotify->Notify(Seq);
		}
	}
	else
	{
		if (TrailNotify && Seq)
		{
#if SHOW_TRAIL_LOG
			debugf(L"******Trail Off %s %f", *(Seq->AnimSeqName.ToString()), Seq->CurrentTime);
#endif
			TrailNotify->NotifyEnd(Seq, Seq->CurrentTime);
		}
	}
}

void TrailComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("TrailComponent");

	if (bStopped)
	{
		return;
	}

	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		OnAction(FALSE);
		return;
	}

	Seq = BaseActor->GetActiveAnimSequence();
	if (!Seq)
	{
		OnAction(FALSE);
		return;
	}

	FLOAT DeltaSecond = DeltaTime * Seq->Rate;

	if (TrailData->StopType == ACBT_StopWhenConstDurationPass)
	{

#if SHOW_TRAIL_LOG
		debugf(L"TrailComponent CurrentTime=%f, DeltaTime=%f Rate=%f", CurrentTime, DeltaSecond, Rate);
#endif
		
		CurrentTime += DeltaSecond;
		if (CurrentTime > Duration)
		{
			OnAction(FALSE);
			return;
		}
	}

	if (TrailNotify)
	{
		if (Seq->bPlaying == FALSE)
		{
#if SHOW_TRAIL_LOG
			debugf(TEXT("Seq->bPlaying== false, close trail"));
#endif
			OnAction(FALSE); 			
			return;
		}

#if SHOW_TRAIL_LOG
		debugf(L"*******Trail Tick %s %f",*(Seq->AnimSeqName.ToString()), Seq->CurrentTime);
#endif
		
		TrailNotify->NotifyTick(Seq, Seq->CurrentTime, DeltaSecond, TrailData->Duration);


#if SHOW_TRAIL_LOG
		FVector Pos1, Pos2, Pos3;
		FRotator Rot;
		Seq->SkelComponent->GetSocketWorldLocationAndRotation(TrailData->FirstEdgeSocketName, Pos1, &Rot, 0);
		Seq->SkelComponent->GetSocketWorldLocationAndRotation(TrailData->ControlPointSocketName, Pos2, &Rot, 0);
		Seq->SkelComponent->GetSocketWorldLocationAndRotation(TrailData->SecondEdgeSocketName, Pos3, &Rot, 0);

		if (bFirstTime == FALSE)
		{
			BaseActor->DrawDebugLine(SrcPoint1, Pos1, 128, 128, 0, TRUE);
			BaseActor->DrawDebugLine(SrcPoint2, Pos2, 128, 128, 0, TRUE);
			BaseActor->DrawDebugLine(SrcPoint3, Pos3, 128, 128, 0, TRUE);
		}

		SrcPoint1 = Pos1;
		SrcPoint2 = Pos2;
		SrcPoint3 = Pos3;
		bFirstTime = FALSE;
#endif

	}
}

void UAbilityHitPointComponent::PushHitPointEffects(UAbilityComponent* Node,INT CreatorId, AActor* BaseActor, AActor* Target, FVector TargetPoint, float Rate, FLOAT InMaxDistance)
{
	if (!Node || !BaseActor)
	{
		return;
	}

	INT InBaseActorId = 0;
	INT InTargetId = 0;
	INT InCreatorId = CreatorId;
	if (BaseActor->IsA(AMMOActor::StaticClass()))
	{
		InBaseActorId = ((AMMOActor*)BaseActor)->ServerID;
	}
	else if (BaseActor->IsA(AMMOEmitter::StaticClass()))
	{
		InBaseActorId = ((AMMOEmitter*)BaseActor)->ServerID;
	}

	if (Target)
	{
		if (Target->IsA(AMMOActor::StaticClass()))
		{
			InTargetId = ((AMMOActor*)Target)->ServerID;
		}
		else if (Target->IsA(AMMOEmitter::StaticClass()))
		{
			InTargetId = ((AMMOEmitter*)Target)->ServerID;
		}
	}


	BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(Node, TRUE, InCreatorId, InBaseActorId, InTargetId, TargetPoint, Rate, InMaxDistance);
	if (!NewNode)
	{
		return;
	}

	EffectProcessor::GetSingleton().AddActiveComponent(NewNode);
	NewNode->OnAction(TRUE);
}


UBOOL MoveControlComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_MoveControl;

	MoveControlData = Cast<UAbilityMoveControlComponent>(InData.ComponentData);
	if (!MoveControlData)
	{
		return FALSE;
	}

	return TRUE;
}


void MoveControlComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		return;
	}

	if (bOn)
	{
		BaseActor->bMovingToMousePoint = TRUE;
		if (MoveControlData->OverrideIdleAnim != NAME_None || MoveControlData->OverrideMoveAnim != NAME_None)
		{
			BaseActor->ReplaceMovementAnim(TRUE, GetId(), MoveControlData->OverrideMoveAnim, MoveControlData->OverrideIdleAnim);
		}
	}
	else
	{
		BaseActor->bMovingToMousePoint = FALSE;
		if (MoveControlData->OverrideIdleAnim != NAME_None || MoveControlData->OverrideMoveAnim != NAME_None)
		{
			BaseActor->ReplaceMovementAnim(FALSE, GetId(), MoveControlData->OverrideMoveAnim, MoveControlData->OverrideIdleAnim);
		}
	}
}

void MoveControlComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("MoveControlComponent");
	BaseComponent::Tick(DeltaTime);
}

UBOOL ThrowComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Throw;

	ThrowData = Cast<UAbilityThrowComponent>(InData.ComponentData);
	if (!ThrowData)
	{
		return FALSE;
	}

	bSuccessed = FALSE;

	return TRUE;
}

void ThrowComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);
	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		return;
	}

	AMMOActor* Target = MMOFacade::getSingleton().GetCharacter(TargetId);
	if (!Target)
	{
		return;
	}

	if (bOn)
	{

		if (Target->PassiveMoveType != PassiveMoveType_Throw && Target->bShowTime == FALSE)
		{
#if SHOW_MOVEMENT_LOG
			debugf(TEXT("%d PassiveMoveType=%d bShowTime=%d"), Target->ServerID, Target->PassiveMoveType, Target->bShowTime);
#endif
			return;
		}

		if (Target->bDead)
		{
			return;
		}

		bSuccessed = TRUE;

#if SHOW_PHYSIC_LOG
		debugf(TEXT("%d ThrowComponent::OnAction bOn=TRUE"));
#endif


		FMatrix SpawnMat = BaseActor->LocalToWorld();
		USkeletalMeshComponent* WeaponMesh1 = NULL;
		USkeletalMeshComponent* WeaponMesh2 = NULL;
		BaseActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
		SpawnMat = ThrowData->GetSpawnMatrix(BaseActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, ThrowData->AttachSocketName, ThrowData->SourceType, ThrowData->RelativeLocation, ThrowData->RelativeRotation);

		Target->SetLocation(SpawnMat.GetOrigin());
		Target->SetRotation(SpawnMat.Rotator());
		Target->Mesh->PhysicsWeight = 1.0;

		if (Target->Mesh->PhysicsAssetInstance == NULL)
		{
			Target->Mesh->SetHasPhysicsAssetInstance(TRUE, TRUE);
		}

		if (Target->Mesh->PhysicsAssetInstance != NULL)
		{
			Target->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
			for (INT i = 0; i < Target->PawnAnimInfo.PhysicSimulateRootBones.Num(); ++i)
			{
				Target->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(Target->PawnAnimInfo.PhysicSimulateRootBones(i), Target->Mesh->PhysicsAsset, Target->Mesh, TRUE);
			}
		}
	}
	else
	{

		if (!bSuccessed)
		{
			return;
		}

		if (Target->Mesh && Target->Mesh->PhysicsAssetInstance)
		{
#if SHOW_PHYSIC_LOG
			debugf(TEXT("%d ThrowComponent::OnAction bOn=FALSE"));
#endif
			Target->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
			for (INT i = 0; i < Target->PawnAnimInfo.PhysicSimulateRootBones.Num(); ++i)
			{
				Target->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(Target->PawnAnimInfo.PhysicSimulateRootBones(i), Target->Mesh->PhysicsAsset, Target->Mesh, FALSE);
			}
			Target->Mesh->PhysicsWeight = 0.f;

			if (Target->bDead)
			{
				return;
			}

			FMatrix SpawnMat = BaseActor->LocalToWorld();
			USkeletalMeshComponent* WeaponMesh1 = NULL;
			USkeletalMeshComponent* WeaponMesh2 = NULL;
			BaseActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
			SpawnMat = ThrowData->GetSpawnMatrix(BaseActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, ThrowData->AttachSocketName, ThrowData->SourceType, ThrowData->RelativeLocation, ThrowData->RelativeRotation);

			Target->SetLocation(SpawnMat.GetOrigin());
			Target->SetRotation(SpawnMat.Rotator());

			Target->ThrowStartMatrix = FRotationTranslationMatrix(BaseActor->Rotation, SpawnMat.GetOrigin());

			FVector P0 = FVector(0, 0, 0);
			FVector P1 = Target->ThrowStartMatrix.InverseTransformFVector(Target->PassiveMoveDestination);
			FVector T0 = ThrowData->T0;
			FVector T1 = P1 + ThrowData->T1;

			if (Target->PassiveMoveType == PassiveMoveType_Throw)
			{
				//Target->SetLocation(P1);
				Target->DoThrowTo(P0, P1, T0, T1, Target->PassiveMoveLastTime - CurrentTime, 1);
			}
		}
	}
}
void ThrowComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("ThrowComponent");

	BaseComponent::Tick(DeltaTime);

	if (!bStopped)
	{
		AMMOActor* BaseActor = GetBaseCharacter();
		if (!BaseActor)
		{
			return;
		}

		AMMOActor* Target = GetTargetCharacter();
		if (!Target)
		{
			return;
		}

		if (Target->bDead)
		{
			return;
		}

		if (Target->PassiveMoveType != PassiveMoveType_Throw)
		{
			return;
		}

		FMatrix SpawnMat = BaseActor->LocalToWorld();
		USkeletalMeshComponent* WeaponMesh1 = NULL;
		USkeletalMeshComponent* WeaponMesh2 = NULL;
		BaseActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
		SpawnMat = ThrowData->GetSpawnMatrix(BaseActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, TargetPoint, ThrowData->AttachSocketName, ThrowData->SourceType, ThrowData->RelativeLocation, ThrowData->RelativeRotation);

		Target->SetLocation(SpawnMat.GetOrigin());
		Target->SetRotation(SpawnMat.Rotator());

		LastVelocity = (SpawnMat.GetOrigin() - Target->Location) / DeltaTime;

		//Target->SetLocation(SpawnMat.GetOrigin());
		//Target->SetRotation(SpawnMat.Rotator());
	}
}


UBOOL PostProcessComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_PostProcess;

	PostProcessData = Cast<UAbilityPostProcessComponent>(InData.ComponentData);
	if (!PostProcessData || PostProcessData->PostEffect == NULL)
	{
		return FALSE;
	}

	return TRUE;
}


void PostProcessComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);
	//AMMOActor* BaseActor = GetBaseCharacter();
	//if (!BaseActor || !BaseActor->WorldInfo || BaseActor->GetMainController() == NULL)
	//{
	//	return;
	//}

	//if (BaseActor->IsMainPlayer() == FALSE && PostProcessData->bPlayInThirdPersonView == FALSE)
	//{
	//	return;
	//}



	//ULocalPlayer* lp = Cast<ULocalPlayer>(BaseActor->GetMainController()->Player);
	//check(lp);
	//UPostProcessChain* ppc = lp->PlayerPostProcessChains.Num() > 0 ? lp->PlayerPostProcessChains(0) : NULL;
	//if (!ppc)
	//{
	//	ppc = ConstructObject<UPostProcessChain>(UPostProcessChain::StaticClass());
	//	lp->PlayerPostProcessChains.AddItem(ppc);
	//}

	if (bOn)
	{
		AMMOActor* BaseActor = GetBaseCharacter();
		if (!BaseActor || !BaseActor->WorldInfo || BaseActor->GetMainController() == NULL)
		{
			return;
		}

		if (BaseActor->IsMainPlayer() == FALSE && PostProcessData->bPlayInThirdPersonView == FALSE)
		{
			return;
		}

		ULocalPlayer* lp = Cast<ULocalPlayer>(BaseActor->GetMainController()->Player);
		if(lp)
		{
			UPostProcessChain* ppc = lp->PlayerPostProcessChains.Num() > 0 ? lp->PlayerPostProcessChains(0) : NULL;
			if (!ppc)
			{
				ppc = ConstructObject<UPostProcessChain>(UPostProcessChain::StaticClass());
				lp->PlayerPostProcessChains.AddItem(ppc);
			}

			UPostProcessEffect* NewEffectData = ConstructObject<UPostProcessEffect>(PostProcessData->PostEffect->GetClass(), INVALID_OBJECT, NAME_None, 0, PostProcessData->PostEffect);
			if (!NewEffectData)
			{
				return;
			}

			NewEffectData->ProcessEffectId = GetId();

			ppc->Effects.AddItem(NewEffectData);
			lp->RebuildPlayerPostProcessChain();
		}
	}
	else
	{
		AMMOPlayerController* Controller = MMOFacade::getSingleton().GetMainController();
		if(Controller)
		{
			ULocalPlayer* lp = Cast<ULocalPlayer>(Controller->Player);
			if(lp)
			{
				UPostProcessChain* ppc = lp->PlayerPostProcessChains.Num() > 0 ? lp->PlayerPostProcessChains(0) : NULL;
				if (ppc)
				{
					for (INT i = 0; i < ppc->Effects.Num(); ++i)
					{
						if (ppc->Effects(i) && ppc->Effects(i)->ProcessEffectId == GetId())
						{
							ppc->Effects.Remove(i);
							lp->RebuildPlayerPostProcessChain();
							break;
						}
					}
				}
			}
		}
	}
}



UBOOL ActionsComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Actions;

	ActionsData = Cast<UAbilityActionsComponent>(InData.ComponentData);
	if (!ActionsData)
	{
		return FALSE;
	}

	return TRUE;
}

void ActionsComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	AActor* BaseActor = GetBaseActor();
	if (!BaseActor)
	{
		return;
	}

	if (bOn)
	{
		for (INT i = 0; i < ActionsData->ActionsComponentList.Num(); ++i)
		{
			if (ActionsData->ActionsComponentList(i).Component)
			{
				ActionsData->ActionsComponentList(i).Component->StopType = ACBT_StopWhenConstDurationPass;
				if (ActionsData->ActionsComponentList(i).Component->Duration > 120.f)
				{
					ActionsData->ActionsComponentList(i).Component->Duration = 120.f;
				}
			}
		}

		Tick(0.001f);
	}
	else
	{
		// nothing to do
	}
}

void ActionsComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("ActionsComponent");

	if (!bStopped)
	{
		AActor* BaseActor = GetBaseActor();
		if (!BaseActor)
		{
			return;
		}

		FLOAT NewTime = CurrentTime + DeltaTime;

		for (INT i = 0; i < ActionsData->ActionsComponentList.Num(); ++i)
		{
			if (ActionsData->ActionsComponentList(i).Component)
			{
				if (CurrentTime <= ActionsData->ActionsComponentList(i).StartTime && ActionsData->ActionsComponentList(i).StartTime < NewTime)
				{
					BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(ActionsData->ActionsComponentList(i).Component, FALSE, CreatorId, BaseActorId, TargetId, BaseActor->Location, Rate, MaxDistance, 0.f);
					if (NewNode)
					{
						NewNode->SetWeaponHitType(this->WeaponHitType);
						EffectProcessor::GetSingleton().AddActiveComponent(NewNode);
						NewNode->OnAction(TRUE);
					}
				}
			}
		}
	}

	BaseComponent::Tick(DeltaTime);
}

UBOOL VisibleComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Visible;

	VisibleData = Cast<UAbilityVisibleComponent>(InData.ComponentData);
	if (!VisibleData)
	{
		return FALSE;
	}
	return TRUE;
}

void VisibleComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);
	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		return;
	}

	if (bOn)
	{
		//BaseActor->SetHidden(TRUE);
		//BaseActor->HideWeapon(TRUE);
		//BaseActor->bVisible = FALSE;
		BaseActor->SetVisibleByAbility(FALSE);
		if (BaseActor->IsA(ANPC::StaticClass()))
		{
			//if (BaseActor->bDead)
			//{
			//	BaseActor->PickPriority = EPP_HiddenBody;
			//}
		}
	}
	else
	{
		BaseActor->SetVisibleByAbility(TRUE);
		//BaseActor->SetHidden(FALSE);
		//BaseActor->HideWeapon(FALSE);
		//BaseActor->bVisible = TRUE;
	}
	BaseActor->UpdatePlaneShow();
}

UBOOL PhysicalSimulateComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_PhysicalSimulate;

	PhysicSimulateData = Cast<UAbilityPhysicSimulateComponent>(InData.ComponentData);
	if (!PhysicSimulateData)
	{
		return FALSE;
	}
	return TRUE;
}

void PhysicalSimulateComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);
	AMMOActor* BaseActor = GetBaseCharacter();
	if (BaseActor == NULL || BaseActor->Mesh == NULL || PhysicSimulateData == NULL)
	{
		return;
	}

	if (BaseActor->Mesh->PhysicsAsset == NULL)
	{
		return;
	}

#if SHOW_PHYSIC_LOG
	  debugf(TEXT("%d PhysicalSimulateComponent::OnAction bOn=%d PType=%d Weight=%f"), BaseActorId, bOn, PhysicSimulateData->Type, PhysicSimulateData->PhysicWeight);
#endif

	if (bOn)
	{
		BaseActor->Mesh->PhysicsWeight = PhysicSimulateData->PhysicWeight;

		if (PhysicSimulateData->Type == SimulateType_Ragdoll)
		{
			BaseActor->EnableRagdoll(bOn);
		}
		else if (PhysicSimulateData->Type == SimulateType_Fully)
		{
			if (BaseActor->Mesh->PhysicsAssetInstance == NULL)
			{
				BaseActor->Mesh->SetHasPhysicsAssetInstance(TRUE, TRUE);
			}

			if (BaseActor->Mesh->PhysicsAssetInstance != NULL)
			{
				BaseActor->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(FALSE);
				FName RootName = (BaseActor->Mesh->SkeletalMesh && BaseActor->Mesh->SkeletalMesh->RefSkeleton.IsValidIndex(0) ? BaseActor->Mesh->SkeletalMesh->RefSkeleton(0).Name : NAME_None);
				BaseActor->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(RootName, BaseActor->Mesh->PhysicsAsset, BaseActor->Mesh, TRUE);
			}
		}
		else if (PhysicSimulateData->Type == SimulateType_PartialSimulate)
		{
			if (BaseActor->Mesh->PhysicsAssetInstance == NULL)
			{
				BaseActor->Mesh->SetHasPhysicsAssetInstance(TRUE, TRUE);
			}

			if (BaseActor->Mesh->PhysicsAssetInstance != NULL)
			{
				BaseActor->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
				for (INT i = 0; i < BaseActor->PawnAnimInfo.PhysicSimulateRootBones.Num(); ++i)
				{
					BaseActor->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(BaseActor->PawnAnimInfo.PhysicSimulateRootBones(i), BaseActor->Mesh->PhysicsAsset, BaseActor->Mesh, TRUE);
				}
			}
		}
		else if (PhysicSimulateData->Type == SimulateType_CustomSimulate)
		{
			if (BaseActor->Mesh->PhysicsAssetInstance == NULL)
			{
				BaseActor->Mesh->SetHasPhysicsAssetInstance(TRUE, TRUE);
			}

			if (BaseActor->Mesh->PhysicsAssetInstance != NULL)
			{
				BaseActor->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
				for (INT i = 0; i < PhysicSimulateData->SimulateRootBones.Num(); ++i)
				{
					BaseActor->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(PhysicSimulateData->SimulateRootBones(i), BaseActor->Mesh->PhysicsAsset, BaseActor->Mesh, TRUE);
				}
			}
		}

	}
	else
	{
		if (BaseActor->bDead)
		{
			return;
		}

		BaseActor->Mesh->PhysicsWeight = 0.f;
		BaseActor->Mesh->SetHasPhysicsAssetInstance(FALSE);
		if (PhysicSimulateData->Type == SimulateType_Ragdoll)
		{
			BaseActor->EnableRagdoll(bOn);
		}
		else if (PhysicSimulateData->Type == SimulateType_Fully)
		{
			if (BaseActor->Mesh->PhysicsAssetInstance == NULL)
			{
				return;
			}

			if (BaseActor->Mesh->PhysicsAssetInstance != NULL)
			{
				BaseActor->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
				FName RootName = (BaseActor->Mesh->SkeletalMesh && BaseActor->Mesh->SkeletalMesh->RefSkeleton.IsValidIndex(0) ? BaseActor->Mesh->SkeletalMesh->RefSkeleton(0).Name : NAME_None);
				BaseActor->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(RootName, BaseActor->Mesh->PhysicsAsset, BaseActor->Mesh, FALSE);
			}
		}
		else if (PhysicSimulateData->Type == SimulateType_PartialSimulate)
		{
			if (BaseActor->Mesh->PhysicsAssetInstance == NULL)
			{
				return;
			}

			if (BaseActor->Mesh->PhysicsAssetInstance != NULL)
			{
				BaseActor->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
				for (INT i = 0; i < BaseActor->PawnAnimInfo.PhysicSimulateRootBones.Num(); ++i)
				{
					BaseActor->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(BaseActor->PawnAnimInfo.PhysicSimulateRootBones(i), BaseActor->Mesh->PhysicsAsset, BaseActor->Mesh, FALSE);
				}
			}
		}
		else if (PhysicSimulateData->Type == SimulateType_CustomSimulate)
		{
			if (BaseActor->Mesh->PhysicsAssetInstance == NULL)
			{
				return;
			}

			if (BaseActor->Mesh->PhysicsAssetInstance != NULL)
			{
				BaseActor->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
				for (INT i = 0; i < PhysicSimulateData->SimulateRootBones.Num(); ++i)
				{
					BaseActor->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(PhysicSimulateData->SimulateRootBones(i), BaseActor->Mesh->PhysicsAsset, BaseActor->Mesh, FALSE);
				}
			}
		}
	}
}


UBOOL ScaleComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Scale;

	ScaleData = Cast<UAbilityScaleComponent>(InData.ComponentData);
	if (!ScaleData)
	{
		return FALSE;
	}
	return TRUE;
}

void ScaleComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);
	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		return;
	}

	if (bOn)
	{
		BaseActor->ScaleTo(TRUE, GetId(), ScaleData->Scale3D, ScaleData->FadeInTime);
	}
	else
	{
		BaseActor->ScaleTo(FALSE, GetId(), ScaleData->Scale3D, ScaleData->FadeOutTime);
	}
}


UBOOL MirrorComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_Mirror;

	MirrorData = Cast<UAbilityMirrorComponent>(InData.ComponentData);
	if (!MirrorData)
	{
		return FALSE;
	}

	NextCreateTime = -0.001f;

	return TRUE;
}

void MirrorComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);

	if (bOn)
	{
		Tick(0.001f);
	}
	else
	{
		MirrorIdList.clear();
	}
}


void MirrorComponent::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("MirrorComponent");

	BaseComponent::Tick(DeltaTime);

	NextCreateTime -= DeltaTime;
	if (NextCreateTime <= 0.f)
	{
		AMMOActor* actor = GetBaseCharacter();
		if (actor)
		{
			USkeletalMeshComponent* Mesh = actor->Mesh;
			INT NewActorId = actor->SpawnMirror(MirrorData, MirrorData->MirrorLifeSpan / Rate);
			MirrorIdList.push_back(NewActorId);
			if (MirrorIdList.size() > size_t(MirrorData->MaxCount))
			{
				actor->ClearMirror(MirrorIdList.front());
				MirrorIdList.pop_front();
			}
		}

		NextCreateTime = MirrorData->Interval / Rate;
	}
}


UBOOL MeshChangeComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_MeshChange;

	MeshChangeData = Cast<UAbilityMeshChangeComponent>(InData.ComponentData);
	if (!MeshChangeData)
	{
		return FALSE;
	}
	return TRUE;
}

void MeshChangeComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);
	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		return;
	}

	if (bOn)
	{
		BaseActor->StartMeshChangeEffect(GetId(), MeshChangeData->MeshData);
	}
	else
	{
		BaseActor->StopMeshChangeEffect(GetId());
	}
}


UBOOL PositionTransformComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_PositionTransform;

	PositionTransformData = Cast<UPositionTransformComponent>(InData.ComponentData);
	if (!PositionTransformData)
	{
		return FALSE;
	}
	return TRUE;
}

void PositionTransformComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);
	AMMOActor* BaseActor = GetBaseCharacter();
	if (!BaseActor)
	{
		return;
	}

#if SHOW_POSITION_TRANSFORM
	debugf(TEXT("PositionTransformComponent::OnAction=%d"), bOn);
#endif
	if (bOn)
	{
		BaseActor->StartPositionTransformEffect(GetId(), PositionTransformData);
	}
	else
	{
		BaseActor->StopPositionTransformEffect(GetId());
	}
}


UBOOL BackgroundMusicComponent::Init(InitData& InData)
{
	UBOOL bRet = BaseComponent::Init(InData);
	if (!bRet)
	{
		return FALSE;
	}

	ComponentType = ECPT_BackgroundMusic;

	BackgroundMusicData = Cast<UAbilityBackgroundMusicComponent>(InData.ComponentData);
	if (!BackgroundMusicData)
	{
		return FALSE;
	}
	return TRUE;
}

void BackgroundMusicComponent::OnAction(UBOOL bOn)
{
	BaseComponent::OnAction(bOn);
	AActor* BaseActor = GetBaseActor();
	if (!BaseActor)
	{
		return;
	}

	if (BackgroundMusicData)
	{
		if (BackgroundMusicData->bStopActiveBackgroundMusic)
		{
			if (bOn)
			{
				BaseActor->WorldInfo->PlayTemporaryMusicTrack(BackgroundMusicData->NewBackgroundMusic, FALSE);
			}
		}
		else
		{
			BaseActor->WorldInfo->PlayTemporaryMusicTrack(BackgroundMusicData->NewBackgroundMusic, bOn);
		}
	}
}






//void UAbilityHitPointComponent::CalculateClientEffect(class AActor* BaseActor, class UMMOAbility* AbilityData, class AActor* Target,FVector TargetPoint,FLOAT InRate)
//{
//	/*
//	if (!BaseActor)
//	{
//		return;
//	}
//
//	UClass* BaseClass = AActor::StaticClass();
//	UClass* InterfaceClass = UIAbilityDamageable::StaticClass();
//	FMemMark Mark(GMainThreadMemStack);
//	FCheckResult* Link = GWorld->Hash->ActorRadiusCheck( GMainThreadMemStack, BaseActor->Location, Radius, TRUE );
//
//	DWORD TraceFlags = TRACE_World|TRACE_ProjTargets;
//
//	AActor* OutActor = NULL;
//	FCheckResult Hit(1.f);
//	FDamageData DamageData;
//	DamageData.AbilityData = this;
//	DamageData.BaseActor = BaseActor;
//	DamageData.CreatorId = iii;
//	DamageData.bServerDamageData = FALSE;
//	DamageData.DamageValue = 100;
//	DamageData.ElementaryType = AbilityData->SpellCommonAttribute.ElementaryType;
//
//	if ( Link )
//	{
//		while ( Link )
//		{
//			if( !Link->Actor ||  
//				Link->Actor == BaseActor ||
//				!Link->Actor->bCollideActors ||	
//				Link->Actor->bDeleteMe ||
//				!Link->Actor->IsA(BaseClass) ||
//				(InterfaceClass != NULL && !Link->Actor->GetClass()->ImplementsInterface(InterfaceClass))
//				)
//			{
//				Link = Link->GetNext();
//			}
//			else
//			{
//				UBOOL bSkip = FALSE;
//				if (Link->Actor->IsA(AMMOActor::StaticClass()))
//				{
//					AMMOActor* actor = Cast<AMMOActor>(Link->Actor);
//					if(actor && !actor->bDead)
//					{
//						bSkip = TRUE;
//					}
//				}
//				if (!bSkip)
//				{
//					IIAbilityDamageable* itarget = InterfaceCast<IIAbilityDamageable>(Link->Actor);
//					debugf(L"CalculateClientEffect");
//					itarget->eventProcessDamage(DamageData);
//				}
//				Link = Link->GetNext();
//			}
//		}
//	}
//	*/
//}

void AbilityNode::ClearSpellComponents()
{
	for (size_t i = 0; i < SpellComponentIds.size(); ++i)
	{
		BaseComponent* node = EffectProcessor::GetSingleton().GetActiveComponent(SpellComponentIds[i]);
		if (node)
		{
			node->OnAction(FALSE);
			EffectProcessor::GetSingleton().DeleteActiveComponent(node->GetId());
		}
	}

	SpellComponentIds.empty();
}

void AbilityNode::ClearCastComponents()
{
	for (size_t i = 0; i < CastComponentIds.size(); ++i)
	{
		BaseComponent* node = EffectProcessor::GetSingleton().GetActiveComponent(CastComponentIds[i]);
		if (node)
		{
			node->OnAction(FALSE);
			EffectProcessor::GetSingleton().DeleteActiveComponent(node->GetId());
		}
	}
	CastComponentIds.empty();
}

void AbilityNode::ClearAbilityComponents()
{
	for (size_t i = 0; i < AbilityComponentIds.size(); ++i)
	{
		BaseComponent* node = EffectProcessor::GetSingleton().GetActiveComponent(AbilityComponentIds[i]);
		if (node)
		{
			node->OnAction(FALSE);
			EffectProcessor::GetSingleton().DeleteActiveComponent(node->GetId());
		}
	}

	AbilityComponentIds.empty();

}

void AbilityNode::DeleteAllEffectData()
{
	for (INT i = 0; i < HIT_POINT_MAX_NUMBER; ++i)
	{
		if (PrepareEffectList[i])
		{
			delete PrepareEffectList[i];
			PrepareEffectList[i] = NULL;
		}
	}
}

void AbilityNode::RefreshAutoDestroyComponentsRate()
{
	for (size_t i = 0; i < AutoDestroyComponentIds.size(); ++i)
	{
		BaseComponent* node = EffectProcessor::GetSingleton().GetActiveComponent(AutoDestroyComponentIds[i]);
		if (node)
		{
			node->SetRate(Rate);
		}
	}
}

FLOAT AbilityNode::NormalizeTime(FLOAT InLastTime)
{
	INT st = InLastTime * 1000.f;
	st = st / (1000 / GAME_FPS);
	if (st == 0)
	{
		if (InLastTime < 0.0001f)
		{
			st = 0;
		}
		else
		{
			st = 1;
		}
	}
	return FLOAT(st) / FLOAT(GAME_FPS);
}

void AbilityNode::AddEffectNode(EffectNode* NewNode)
{
	check(NewNode);
	check(AbilityData);

	NewNode->BaseAbilityNode = this;

	if (State == EAS_End)
	{
		Processor->PushEffect(NewNode);
		delete NewNode;
		NewNode = NULL;
		return;
	}

	if (NewNode->Index < 0 || NewNode->Index > ON_CAST_HIT_POINT_INDEX)
	{
		Processor->PushEffect(NewNode);
		delete NewNode;
		NewNode = NULL;
		return;
	}

	if (PrepareEffectList[NewNode->Index] == NULL)
	{
		Processor->PushEffect(NewNode);
		delete NewNode;
		return;
	}

	if (NewNode->Index == ON_CAST_HIT_POINT_INDEX && NewNode->CastId != PrepareEffectList[NewNode->Index]->Id)
	{
		Processor->PushEffect(NewNode);
		delete NewNode;
		return;
	}

	if (PrepareEffectList[NewNode->Index]->bActive == FALSE)
	{
		Processor->PushEffect(NewNode);
		delete NewNode;
		return;
	}

	PrepareEffectList[NewNode->Index]->EffectList.push_back(NewNode);
}

void AbilityNode::PushAllHitEffect()
{
	for (INT i = 0; i < HIT_POINT_MAX_NUMBER; ++i)
	{
		PushHitEffect(i);
	}
}

void AbilityNode::PushHitEffect(INT Idx)
{
	if (Idx < 0 || Idx > ON_CAST_HIT_POINT_INDEX)
	{
		return;
	}

	if (PrepareEffectList[Idx] == NULL)
	{
		return;
	}

	if (PrepareEffectList[Idx]->bActive == FALSE)
	{
		return;
	}

	PrepareEffectList[Idx]->bActive = FALSE;
	for (size_t i = 0; i < PrepareEffectList[Idx]->EffectList.size(); ++i)
	{
		Processor->PushEffect(PrepareEffectList[Idx]->EffectList[i]);
	}


	for (size_t i = 0; i < PrepareEffectList[Idx]->EffectList.size(); ++i)
	{
		delete PrepareEffectList[Idx]->EffectList[i];
		PrepareEffectList[Idx]->EffectList[i] = NULL;
	}
	PrepareEffectList[Idx]->EffectList.clear();
}


void AbilityNode::GenerateHitPointInfos(BYTE InType)
{
	for (INT i = 0; i < HIT_POINT_MAX_NUMBER; ++i)
	{
		PrepareEffectList[i] = NULL;
	}

	if (SpellEffectList == NULL || CastEffectList == NULL)
	{
		return;
	}

	// build hitpoint list
	UAbilityHitPointComponent* hc = NULL;
	for (INT i = 0; i < (*SpellEffectList).Num(); ++i)
	{
		if ((*SpellEffectList)(i).Component)
		{
			hc = Cast<UAbilityHitPointComponent>((*SpellEffectList)(i).Component);
			if (hc)
			{
				if (hc->HitPointId == -1 || hc->HitPointId >= ON_CAST_HIT_POINT_INDEX)
				{
					continue;
				}

				if (PrepareEffectList[hc->HitPointId])
				{
					continue;
				}

				PrepareEffectList[hc->HitPointId] = new HitNode;
				PrepareEffectList[hc->HitPointId]->bActive = TRUE;
				PrepareEffectList[hc->HitPointId]->HitPointData = hc;
			}
		}
	}

	hc = NULL;
	FLOAT HitPointStartTime = 0.01f;
	for (INT i = 0; i < (*CastEffectList).Num(); ++i)
	{
		if ((*CastEffectList)(i).Component)
		{
			hc = Cast<UAbilityHitPointComponent>((*CastEffectList)(i).Component);
			if (hc)
			{
				if (hc->HitPointId != ON_CAST_HIT_POINT_INDEX)
				{
					hc->HitPointId = ON_CAST_HIT_POINT_INDEX;
				}

				PrepareEffectList[hc->HitPointId] = new HitNode;
				PrepareEffectList[hc->HitPointId]->bActive = TRUE;
				PrepareEffectList[hc->HitPointId]->HitPointData = hc;
				HitPointStartTime = (*CastEffectList)(i).StartTime;
				break;
			}
		}
	}

	if (hc == NULL)
	{
		PrepareEffectList[ON_CAST_HIT_POINT_INDEX] = new HitNode;
		PrepareEffectList[ON_CAST_HIT_POINT_INDEX]->bActive = FALSE;
	}
}

BaseComponent* AbilityProcessor::CreateDynamicComponent(UAbilityComponent* InComponentNode, UBOOL bInHitPoint,INT InCreatorId, INT InBaseActorId, INT InTargetId, FVector& InTargetPoint, FLOAT InRate, FLOAT InMaxDistance, FLOAT InStartTime)
{
	try
	{
		if (!InComponentNode)
		{
			return NULL;
		}

		InComponentNode->bInHitPoint = bInHitPoint;

		BaseComponent* NewNode = InComponentNode->CreateCplusplusNode();
		if (NewNode == 0)
		{
			return NULL;
		}

		InitData InData;
		InData.BaseActorId = InBaseActorId;
		InData.ComponentData = InComponentNode;
		InData.CreatorId = InCreatorId;
		InData.MaxDistance = InMaxDistance;
		InData.Rate = InRate;
		InData.StartTime = InStartTime;
		InData.TargetId = InTargetId;
		InData.TargetPoint = InTargetPoint;

		UBOOL b = NewNode->Init(InData);

		if (!b)
		{
			delete NewNode;
			return NULL;
		}
		else
		{
			return NewNode;
		}
		return NewNode;
	}
	catch (...)
	{
		warnf(TEXT("start AbilityProcessor::CreateDynamicComponent error!!!"));
		warnf(TEXT("InBaseActorId=%d, InTargetId=%d, bInHitPoint=%d"), InBaseActorId, InTargetId, bInHitPoint);
		AActor* BaseActor = MMOFacade::getSingleton().GetActor(InBaseActorId);
		if (BaseActor)
		{
			warnf(TEXT("BaseActor's name is %s"), *(BaseActor->GetName()));
		}
		else
		{
			warnf(TEXT("BaseActor's name is null"));
		}

		warnf(TEXT("Template name is"));
		warnf(TEXT("%s"), *(InComponentNode->GetName()));
		warnf(TEXT("start AbilityProcessor::CreateDynamicComponent error!!!"));
		return NULL;
	}
}

void AbilityProcessor::CreateDynamicComponent(AbilityNode* InAbilityNode, UAbilityComponent* InComponentNode, EAbilityState ArrayState, FLOAT InStartTime)
{
	if (!InAbilityNode || !InComponentNode)
	{
		return;
	}

	// check hit point component
	if (InComponentNode->IsA(UAbilityHitPointComponent::StaticClass()))
	{
		UAbilityHitPointComponent* hc = (UAbilityHitPointComponent*)InComponentNode;
		InAbilityNode->PushHitEffect(hc->HitPointId);
	}

	AActor* BaseActor = MMOFacade::getSingleton().GetActor(InAbilityNode->SelfID);
	if (!BaseActor)
	{
		return;
	}

	BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(InComponentNode, FALSE,InAbilityNode->SelfID, InAbilityNode->SelfID, InAbilityNode->TargetID, InAbilityNode->TargetPoint, InAbilityNode->Rate, InAbilityNode->AbilityData->SpellCommonAttribute.MaxDistance, InStartTime);

	if (!NewNode)
	{
		return;
	}

	EffectProcessor::GetSingleton().AddActiveComponent(NewNode);

	EAbilityComponentStopType type = NewNode->GetStopType();
	if (ArrayState == EAS_Spell)
	{
		if (type == ACBT_StopWhenAbilityStateEnd)
		{
			InAbilityNode->SpellComponentIds.push_back(NewNode->GetId());
		}
		else if (type == ACBT_StopWhenAbilityEnd)
		{
			InAbilityNode->AbilityComponentIds.push_back(NewNode->GetId());
		}
		else
		{
			InAbilityNode->AutoDestroyComponentIds.push_back(NewNode->GetId());
		}
	}
	else if (ArrayState == EAS_Cast)
	{
		if (type == ACBT_StopWhenAbilityStateEnd)
		{
			InAbilityNode->CastComponentIds.push_back(NewNode->GetId());
		}
		else if (type == ACBT_StopWhenAbilityEnd)
		{
			InAbilityNode->AbilityComponentIds.push_back(NewNode->GetId());
		}
		else
		{

		}
	}
	else
	{
		// nothing
	}

	NewNode->OnAction(TRUE);
}


void AbilityProcessor::UpdateAbility(AbilityNode* node, FLOAT DeltaTime)
{
	UBOOL bNeedUpdateSpell = FALSE;
	FLOAT LastTime = 0.f, NewTime = 0.f;

	if (node->State == EAS_Cast)
	{
		bNeedUpdateSpell = TRUE;

		LastTime = node->CastCurrentTime;
		NewTime = node->CastCurrentTime + DeltaTime;

		if (node->CastEffectList)
		{
			for (INT i = 0; i < (*(node->CastEffectList)).Num(); ++i)
			{
				FComponentNode& Comp = (*(node->CastEffectList))(i);
				if (LastTime <= Comp.StartTime && Comp.StartTime < NewTime)
				{
#if SHOW_ABILITY_COMPONENT_LOG
					debugf(L"Id=%d, CastComp=%s, StartTime=%f, LastTime=%f, NewTime=%f",node->AbilityData->SpellCommonAttribute.Id,  *Comp.Component->GetName(), Comp.StartTime, LastTime, NewTime);
#endif
					AActor* self = MMOFacade::getSingleton().GetActor(node->SelfID);
					if (self && Comp.Component)
					{
						CreateDynamicComponent(node, Comp.Component, EAS_Cast, node->AbilityCurrentTime);
					}
				}
			}
		}

	}
	else if (node->State == EAS_Spell)
	{
		bNeedUpdateSpell = TRUE;
	}


	if (bNeedUpdateSpell)
	{

		LastTime = node->SpellCurrentTime;
		NewTime = node->SpellCurrentTime + DeltaTime;

		if (node->SpellEffectList)
		{
			for (INT i = 0; i < (*(node->SpellEffectList)).Num(); ++i)
			{
				FComponentNode& Comp = (*(node->SpellEffectList))(i);

				if (LastTime <= Comp.StartTime && Comp.StartTime < NewTime)
				{
#if SHOW_ABILITY_COMPONENT_LOG
					debugf(L"Id=%d, SpellComp=%s, StartTime=%f, LastTime=%f, NewTime=%f", node->AbilityData->SpellCommonAttribute.Id,  *Comp.Component->GetName(), Comp.StartTime, LastTime, NewTime);
#endif
					AActor* self = MMOFacade::getSingleton().GetActor(node->SelfID);
					if (self && Comp.Component)
					{
						CreateDynamicComponent(node, Comp.Component, EAS_Spell, node->AbilityCurrentTime);
					}
				}
			}
		}
	}
}

void AbilityProcessor::ClearAllAbilityNode()
{
	std::vector<LONG64> ActiveList;
	for(STLLong64ToAbilityNodeMap::iterator i = AbilityList.begin(); i != AbilityList.end(); ++i)
	{
		ActiveList.push_back(i->first);
	}

	for (size_t i = 0; i < ActiveList.size(); ++i)
	{
		DeteleAbilityNode(ActiveList[i]);
	}
}

void AbilityProcessor::DeteleAbilityNode(LONG64 key)
{
	//debugf(L"DeleteAbility Node Key=%d", key);
	STLLong64ToAbilityNodeMap::iterator pos = AbilityList.find(key);
	if (pos != AbilityList.end())
	{
#if SHOW_ABILITY_LOG
		debugf(L"Delete Ability Id=%d", pos->second->AbilityData->SpellCommonAttribute.Id);
#endif
		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(pos->second->SelfID);
		if (actor && actor->IsMainPlayer())
		{
			//if (pos->second->AbilityData->SpellCommonAttribute.CastType == E_spct_Active)
			//{
			//	actor->CurrentAbilityState = EAS_End;
			//}
			
			if (pos->second->AbilityData->ClientCommonAttribute.bRotateToMousePoint)
			{
				actor->RotateToMousePointRefrence--;
			}

			//if (pos->second->AbilityData->ClientCommonAttribute.bHideWeaponInCast || pos->second->AbilityData->ClientCommonAttribute.bHideWeaponInSpell)
			//{
			//	actor->HideWeapon(FALSE);
			//}
			//debugf(L"CurrentAbilityState=%d", actor->CurrentAbilityState);
		}


		pos->second->ClearSpellComponents();
		pos->second->ClearCastComponents();
		pos->second->ClearAbilityComponents();

		pos->second->PushAllHitEffect();
		pos->second->DeleteAllEffectData();

		delete pos->second;
		pos->second = NULL;
		AbilityList.erase(key);
	}
}

void AbilityProcessor::Tick(FLOAT DeltaTime)
{
	CTRL_TICK("MMOAbilityProcessor::Tick");

	LONG64 PushedKey = 0;
	typedef std::vector<LONG64, STLAllocator<LONG64, UE3STLAllocatorPolicy> > Long64Vector;

	Long64Vector ToPushList;
	Long64Vector ToDeleteList;
	STLLong64ToAbilityNodeMap::iterator pos;
	for (pos = AbilityList.begin(); pos != AbilityList.end(); ++pos)
	{
		AbilityNode* node = pos->second;
		UMMOAbility* abilityData = pos->second->AbilityData;
		check(abilityData);
		check(node);

		FLOAT DeltaSeconds = DeltaTime * node->Rate;

		if (node->bInterrupted == FALSE)
		{
			UpdateAbility(node, DeltaSeconds);

			node->AbilityCurrentTime += DeltaSeconds;
		}

		//if (node->State == EAS_Requested)
		//{
		//	node->RequestCurrentTime += DeltaSeconds;
		//	if (node->RequestCurrentTime > 3.f)
		//	{
		//		// timeout
		//		ToDeleteList.push_back(pos->first);
		//		warnf("Requested an ability, but no respone");
		//	}
		//}
		//else 
		if (node->State == EAS_Spell)
		{
			node->SpellCurrentTime += DeltaSeconds;

			if (node->SpellCurrentTime > node->AbilityData->GetAbilitySpellTime() * node->TimeScaleByWeaponType + 5.0f)
			{
				// dirty data, to delete
				ToDeleteList.push_back(pos->first);
				if (node->AbilityData->bClientPlayFirst)
				{
#if _DEBUG
					check(node->AbilityData->SpellCommonAttribute.CastType == E_spct_Active);
#endif
					AMMOActor* MainPlayer = MMOFacade::getSingleton().GetMainPlayer();
					if (MainPlayer && MainPlayer->ServerID==node->SelfID)
					{
						MainPlayer->CurrentAbilityState = EAS_End;
						MainPlayer->AbilityStateCD = 0.0f;
						warnf(NAME_Log, TEXT("AbilityProcessor::Tick: ability timed out, MainPlayer->CurrentAbilityState = EAS_End----%d"),abilityData->SpellCommonAttribute.Id );
					}
				}
			}

			if (node->bClientAbility)
			{
				if (node->SpellCurrentTime >= node->OverrideSpellTime)
				{
					node->State = EAS_Cast;
				}
			}
		}
		else if (node->State == EAS_Cast)
		{
			// update spell time
			node->SpellCurrentTime += DeltaSeconds;

			//FLOAT LastTime = node->CastCurrentTime;
			//FLOAT NewTime = node->CastCurrentTime + DeltaSeconds;
			//if (LastTime <= node->HitPointTime && node->HitPointTime < NewTime)
			//{
			//	//debugf(L"CastCurrentTime=%f, HitPointTime=%f", node->CastCurrentTime, DynamicTime);
			//	ToPushList.push_back(pos->first);
			//}

			node->CastCurrentTime += DeltaSeconds;
			if (node->CastCurrentTime >= node->OverrideCastTime)
			{
				if (node->bClientAbility)
				{
					node->State = EAS_End;

				}

				if (node->AbilityData->bClientPlayFirst)
				{
					node->State = EAS_End;
				}

				// Channeled ability
				if (node->AbilityData->SpellCommonAttribute.PrepareType == ES_PT_Type1)
				{
					if (node->bLeadAbilityOver == TRUE)
					{
						node->State = EAS_End;
					}
					else
					{
						if (node->CastCurrentTime >= node->OverrideCastTime + 3.0)
						{
							node->State = EAS_End;
						}
					}
				}
				else
				{
					node->State = EAS_End;
				}
			}

			if (node->State == EAS_End)
			{
				ToDeleteList.push_back(pos->first);
			}
		}
		else // if (node->State == EAS_End)
		{
			ToDeleteList.push_back(pos->first);
		}
	}

	//for (size_t i = 0; i < ToPushList.size(); ++i)
	//{
	//	PushEffect(ToPushList[i]);
	//}
	//ToPushList.empty();

	for (size_t i = 0; i < ToDeleteList.size(); ++i)
	{
		DeteleAbilityNode(ToDeleteList[i]);
	}
	ToDeleteList.empty();
}

void AbilityProcessor::OnRecvSelectTarget(UINT CasterID, UINT AbilityId, UINT AbilityLevel, UINT RuneId)
{
	//todo
#if SHOW_ABILITY_LOG
	warnf(NAME_Log, TEXT("Parse_g2c_char_precast_spell CasterID=%d  AbilityID=%d, RuneId=%d"), CasterID, AbilityId, RuneId);
#endif
}

void AbilityProcessor::OnRecvPrepareSpell(UINT CasterID, UINT AbilityID, UINT AbilityLevel, UINT AbilityRune, UINT TargetID, FVector TargetPoint, UINT BaseWeaponSpeedFactor, UINT InSpeedFactor, UINT SerialID)
{
	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_ABILITY_LOG
	warnf(NAME_Log, TEXT("Parse_g2c_char_precast_spell AbilityId=%d CasterID=%d SerialID=%d, BaseWeaponSpeedFactor=%d, InSpeedFactor=%d, Point(%f, %f, %f)"), 
		AbilityID, CasterID, SerialID, BaseWeaponSpeedFactor, InSpeedFactor, TargetPoint.X, TargetPoint.Y, TargetPoint.Z);
#endif

	AActor* actor = MMOFacade::getSingleton().GetActor(CasterID);
	if (!actor)
	{
		// no caster, no saving cast data to buffer
		#if SHOW_ABILITY_LOG
			warnf(TEXT("Caster not found"));
		#endif
		return;
	}


	UINT key = CombinateID(AbilityID, AbilityLevel, AbilityRune);
	UMMOAbility* abilityData = GetAbilityData(key);
	if (abilityData == NULL)
	{
		warnf(TEXT("can not find ability data by id(%d), Level(%d), RuneSlot(%d)"), AbilityID, AbilityLevel, AbilityRune);
		return;
	}

	AMMOActor* mmoActor = Cast<AMMOActor>(actor);
	if(mmoActor && mmoActor->IsMainPlayer())
	{
		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		check(pc);
		if (pc != NULL)
		{
#if SHOW_ABILITY_LOG
			warnf(NAME_Warning, TEXT("OnRecvPrepareSpell:pc->WaitingForTheAbilityRespond = %d, AbilityID = %d"), pc->WaitingForTheAbilityRespond, AbilityID);
#endif
			if (pc->WaitingForTheAbilityRespond == AbilityID)
			{
				pc->WaitingForTheAbilityRespond = 0;
				FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(-1.0f);
				pc->WaitingForTheAbilityBuf = -1.0f;
				FLOAT bufTime = abilityData->GetSpellBufTime(mmoActor);
				pc->WaitingForTheAbilityBuf = bufTime;
			}
		}
		else
		{
			warnf(NAME_Error, TEXT("AMMOPlayerController == NULL!!"));
		}
	}
	else
	{
#if SHOW_ABILITY_LOG
		debugf(TEXT("Caster is not the player?"));
#endif
	}

	// If it is an ability cast by the main character itself, just update the ability's current target info
	if (mmoActor && mmoActor->IsMainPlayer() && abilityData->bClientPlayFirst)
	{
		LONG64 Skey = CombinateID64(AbilityID, CasterID, SerialID);
		STLLong64ToAbilityNodeMap::iterator pos = AbilityList.find(Skey);
		if (pos == AbilityList.end())
		{
			return;
		}
		pos->second->TargetID = TargetID;
		pos->second->TargetPoint = TargetPoint;

	}
	else
	{

		AbilityNode *node = new AbilityNode;
		if (node == NULL)
		{
			warnf(NAME_Error, TEXT("AbilityNode == NULL!! , AbilityNode new failed"));
			return ;
		}
		node->SetProcessor(this);
		node->TargetID = TargetID;
		node->SelfID = CasterID;
		node->TargetPoint = TargetPoint;
		node->State = EAS_Spell;
		node->CastCurrentTime = 0.f;
		node->Rate = 1.0f;
		node->Serial = SerialID;
		node->CastList.empty();
		node->AbilityData = abilityData;
		node->TimeScaleByWeaponType = FLOAT(BaseWeaponSpeedFactor) / 10000.f;
		node->SpeedScale = FLOAT(InSpeedFactor) / 10000.f;

		FLOAT DataTime = abilityData->SpellCommonAttribute.PrepareTimeMin;

		if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type5) //Charge-up
		{
			DataTime = abilityData->SpellCommonAttribute.PrepareTimeMax; // * node->TimeScale;
		}

		node->OverrideSpellTime = DataTime * node->TimeScaleByWeaponType / node->SpeedScale;

		node->OverrideSpellTime = node->NormalizeTime(node->OverrideSpellTime);

		if (node->OverrideSpellTime < 0.0001f)
		{
			node->Rate = 1.0;
		}
		else
		{
			DataTime *= node->TimeScaleByWeaponType;
			node->Rate = DataTime / node->OverrideSpellTime;
		}

		//node->Rate = DEBUG_RATE;

		node->OverrideCastTime = 0.f;
		node->WeaponType = 0;
		if (mmoActor)
		{
			node->WeaponType = mmoActor->GetWeaponType();
		}
		UBOOL bFound = node->AbilityData->GetEffectComponentList(node->WeaponType, &(node->SpellEffectList), &(node->CastEffectList));
		if (!bFound)
		{
			debugf(TEXT("Can not found effect list by type=%d"), node->WeaponType);
		}
		node->GenerateHitPointInfos(node->WeaponType);

#if SHOW_ABILITY_LOG
		debugf(TEXT("OverrideSpellTime=%f Rate=%f"), node->OverrideSpellTime, node->Rate);
#endif

		//INT HitIdx = -1;
		//for (INT i = 0; i < abilityData->CastComponentList.Num(); ++i)
		//{
		//	if (abilityData->CastComponentList(i).Component && abilityData->CastComponentList(i).Component->IsA(UAbilityHitPointComponent::StaticClass()))
		//	{
		//		HitIdx = i;
		//		break;
		//	}
		//}
		//if (HitIdx != -1)
		//{
		//	node->HitPointTime = abilityData->CastComponentList(HitIdx).StartTime;
		//}
		//else
		//{
		//	node->HitPointTime = 0.f ;
		//}

		LONG64 Skey = CombinateID64(AbilityID, CasterID, SerialID);
		AbilityList.insert(make_pair(Skey, node));

		if (mmoActor && mmoActor->IsMainPlayer())
		{
			AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
			check(pc);

			if (abilityData->SpellCommonAttribute.CastType == E_spct_Active)
			{
				mmoActor->CurrentAbilityState = EAS_Spell;
				mmoActor->CanMoveInSpellTime = abilityData->SpellCommonAttribute.CanMoveInPrepare;
				mmoActor->CanMoveInCastTime = abilityData->SpellCommonAttribute.CanMoveInHardTime;
			}

			if (abilityData->ClientCommonAttribute.bRotateToMousePoint)
			{
				mmoActor->RotateToMousePointRefrence++;
				mmoActor->RotateSpeed = abilityData->ClientCommonAttribute.RotateSpeed;
			}

			if (pc != NULL)
			{
				if (abilityData->ClientCommonAttribute.bShowLeadUI)
				{
					if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type2) // Chanting
					{
						pc->OnAbilityTimerBarStart(abilityData, node->Rate );
					}
					else if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type5) // Charge-up
					{
						pc->OnAbilityTimerBarStart(abilityData, node->Rate);
					}
				}
			}
			else
			{
				warnf(NAME_Error, TEXT("AMMOPlayerController == NULL!!"));
			}

			if (abilityData->ClientCommonAttribute.bShowAbilityName)
			{
				MMOChatManager::getSingleton().AddSystemMessage(abilityData->ClientCommonAttribute.AbilityName);
			}

			// Use ability, complete quest
			MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_UseSkill, AbilityID);
		}
	}
}


void AbilityProcessor::PlayAbilityBeforeServer(UINT CasterID, UINT AbilityID, UINT AbilityLevel, UINT AbilityRune, UINT TargetID, FVector TargetPoint, UINT SerialID)
{

	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_ABILITY_LOG
	debugf(L"PlayAbilityBeforeServer AbilityId=%d  CasterID=%d SerialID=%d", AbilityID, CasterID, SerialID);
#endif
	AActor* actor = MMOFacade::getSingleton().GetActor(CasterID);
	if (!actor)
	{
		// no caster, no saving cast data to buffer
		return;
	}

	UINT key = CombinateID(AbilityID, AbilityLevel, AbilityRune);
	UMMOAbility* abilityData = GetAbilityData(key);
	if (abilityData == NULL)
	{
		warnf(TEXT("can not find ability data by id(%d), Level(%d), RuneSlot(%d)"), AbilityID, AbilityLevel, AbilityRune);
		return;
	}

	if (abilityData->bClientPlayFirst == FALSE)
	{
		return;
	}

	AMMOActor* mmoActor = Cast<AMMOActor>(actor);


	//if (mmoActor && mmoActor->IsA(AMMOPlayer::StaticClass()))
	//{
	//	mmoActor->DrawDebugPoint(TargetPoint, 10, FLinearColor(1.0, 0.f, 0.f), TRUE);
	//}


	AbilityNode *node = new AbilityNode;
	node->SetProcessor(this);
	node->TargetID = TargetID;
	node->SelfID = CasterID;
	node->TargetPoint = TargetPoint;
	node->State = EAS_Spell;
	node->CastCurrentTime = 0.f;
	node->Rate = 1.0f;
	node->Serial = SerialID;
	node->CastList.empty();
	node->AbilityData = abilityData;
	node->TimeScaleByWeaponType = 1.0f;
	node->SpeedScale = 1.0f;

	FLOAT DataTime = abilityData->SpellCommonAttribute.PrepareTimeMin;

	if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type5) //Charge-up
	{
		DataTime = abilityData->SpellCommonAttribute.PrepareTimeMax; // * node->TimeScale;
	}

	node->OverrideSpellTime = DataTime * node->TimeScaleByWeaponType / node->SpeedScale;

	node->OverrideSpellTime = node->NormalizeTime(node->OverrideSpellTime);

	if (node->OverrideSpellTime < 0.0001f)
	{
		node->Rate = 1.0;
	}
	else
	{
		DataTime *= node->TimeScaleByWeaponType;
		node->Rate = DataTime / node->OverrideSpellTime;
	}

	//node->Rate = DEBUG_RATE;

	node->OverrideCastTime = 0.f;
	node->WeaponType = 0;
	if (mmoActor)
	{
		node->WeaponType = mmoActor->GetWeaponType();
	}
	UBOOL bFound = node->AbilityData->GetEffectComponentList(node->WeaponType, &(node->SpellEffectList), &(node->CastEffectList));
	if (!bFound)
	{
		debugf(TEXT("Can not found effect list by type=%d"), node->WeaponType);
	}
	node->GenerateHitPointInfos(node->WeaponType);

#if SHOW_ABILITY_LOG
	debugf(TEXT("OverrideSpellTime=%f Rate=%f"), node->OverrideSpellTime, node->Rate);
#endif

	//INT HitIdx = -1;
	//for (INT i = 0; i < abilityData->CastComponentList.Num(); ++i)
	//{
	//	if (abilityData->CastComponentList(i).Component && abilityData->CastComponentList(i).Component->IsA(UAbilityHitPointComponent::StaticClass()))
	//	{
	//		HitIdx = i;
	//		break;
	//	}
	//}
	//if (HitIdx != -1)
	//{
	//	node->HitPointTime = abilityData->CastComponentList(HitIdx).StartTime;
	//}
	//else
	//{
	//	node->HitPointTime = 0.f ;
	//}

	LONG64 Skey = CombinateID64(AbilityID, CasterID, SerialID);
	AbilityList.insert(make_pair(Skey, node));

	if (mmoActor && mmoActor->IsMainPlayer())
	{
		//AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		//AMMOActor* MainActor = MMOFacade::getSingleton().GetMainActor();
		//check(pc);
		//check(MainActor);

		//FSMIO::getSingleton().CDTime.ResetCD_WalkOrRunDelay(abilityData,TargetID);

		//if(pc->WaitingForTheAbilityRespond == AbilityID)
		//{
		//	pc->WaitingForTheAbilityRespond = 0;
		//	FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime();
		//	warnf(NAME_Warning, TEXT("1--------pc->WaitingForTheAbilityRespond = 0"));

		//	pc->WaitingForTheAbilityBuf = -1.0f;
		//	FLOAT bufTime = abilityData->GetSpellBufTime(MainActor);
		//	pc->WaitingForTheAbilityBuf = bufTime;
		//}

		// check break move1
		if (/*mmoActor->ActorIsMoving() && */ abilityData->SpellCommonAttribute.BreakMove)
		{
			//pc->StopMove(FALSE);
		}

		//if (abilityData->ClientCommonAttribute.bHideWeaponInSpell)
		//{
		//	mmoActor->HideWeapon(TRUE);
		//}


		if (abilityData->SpellCommonAttribute.CastType == E_spct_Active)
		{
			//mmoActor->CurrentAbilityState = EAS_Spell;
			mmoActor->CurrentAbilityState = EAS_Requested;
			mmoActor->CurrentRequestAbilitySerialId = SerialID;
			mmoActor->CurrentRequestAbilityId = AbilityID;
			mmoActor->CanMoveInSpellTime = abilityData->SpellCommonAttribute.CanMoveInPrepare;
			mmoActor->CanMoveInCastTime = abilityData->SpellCommonAttribute.CanMoveInHardTime;
		}

		//if (abilityData->SpellCommonAttribute.CastType == E_spct_Passive)
		//{
		//	if(abilityData->GetCoolDownBufTime(mmoActor)>0.1)
		//	{
		//		MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, CasterID, AbilityID*100+9, AbilityID*10+9, 0, abilityData->GetCoolDownBufTime(mmoActor)*1000, abilityData->GetCoolDownBufTime(mmoActor)*1000, 1,TRUE);
		//	}
		//}

		//if (abilityData->ClientCommonAttribute.bRotateToMousePoint)
		//{
		//	mmoActor->RotateToMousePointRefrence++;
		//	mmoActor->RotateSpeed = abilityData->ClientCommonAttribute.RotateSpeed;
		//}

		//if (abilityData->ClientCommonAttribute.bShowLeadUI)
		//{
		//	if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type2) // Chanting
		//	{
		//		pc->OnAbilityTimerBarStart(abilityData, node->Rate );
		//	}
		//	else if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type5) // Charge-up
		//	{
		//		pc->OnAbilityTimerBarStart(abilityData, node->Rate);
		//	}
		//}

		//if (abilityData->ClientCommonAttribute.bShowAbilityName)
		//{
		//	MMOChatManager::getSingleton().AddSystemMessage(abilityData->ClientCommonAttribute.AbilityName);
		//}

		//// Use ability, complete quest
		//MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest_Client_UseSkill, AbilityID);
	}


}

void AbilityProcessor::OnMainRoleEndAbility(INT AbilityId, INT SerialId)
{
	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_ABILITY_LOG
	debugf(TEXT("OnMainRoleEndAbility AbilityId=%d, SerialId=%d"), AbilityId, SerialId);
#endif
	
	AMMOActor* MainActor = MMOFacade::getSingleton().GetMainActor();
	check(MainActor);


	if(MainActor->CurrentRequestAbilitySerialId!=0 && MainActor->CurrentRequestAbilitySerialId==SerialId)
	{
		MainActor->CurrentAbilityState = EAS_End;
		MainActor->AbilityStateCD = 0.0f;
	}
	else if (MainActor->CurrentRequestAbilityId == AbilityId || MainActor->CurrentRequestAbilitySerialId == 0)
	{
		MainActor->CurrentAbilityState = EAS_End;
		MainActor->AbilityStateCD = 0.0f;
	}
	else if (FSM::Ability.CurrentAbility&&AbilityId == FSM::Ability.CurrentAbility->SpellCommonAttribute.Id){
		MainActor->CurrentAbilityState = EAS_End;
		MainActor->AbilityStateCD = 0.0f;
	}

	INT CasterId = MainActor->ServerID;

	LONG64 Skey = CombinateID64(AbilityId, CasterId, SerialId);
	STLLong64ToAbilityNodeMap::iterator pos = AbilityList.find(Skey);
	if (pos == AbilityList.end())
	{
		//debugf(L"OnEndAbility error 1950");
		return;
	}
	else
	{
		//debugf(L"OnMainRoleEndAbility call push effect");
		//PushEffect(Skey);
	}

	////Passive abilities have cooldowns, add cooldown flag, add by JimZhang@snda.com
	//if(pos->second){
	//	UMMOAbility* abilityData = pos->second->AbilityData;
	//	if (abilityData && abilityData->SpellCommonAttribute.CastType == E_spct_Passive)//Passive ability
	//	{
	//		if(abilityData->GetCoolDownBufTime(MMOFacade::getSingleton().GetMainActor())>0.1){
	//			MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuff(CasterId, AbilityId*100+9);
	//		}
	//	}
	//}

	//debugf(L"OnMainRoleEndAbility call delete ability");
	DeteleAbilityNode(Skey);
}

void AbilityProcessor::OnRecvCastSpell(G2C_CHAR_CAST_SPELL* InData)
{
	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_ABILITY_LOG
	warnf(NAME_Log, TEXT("Parse_g2c_char_cast_spell CasterID=%d  AbilityID=%d, SerialId=%d, CastId=%d"), 
		InData->SpellCastInfo.dwCasterID, InData->SpellCastInfo.wSpellID, InData->SpellCastInfo.bySerial, InData->SpellCastInfo.byCastID);
#endif
	AActor* Actor = MMOFacade::getSingleton().GetActor(InData->SpellCastInfo.dwCasterID);
	AMMOActor* mmoActor = Cast<AMMOActor>(Actor);
	if(mmoActor&&mmoActor->IsMainPlayer())
	{
		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		if(pc && pc->WaitingForTheAbilityRespond==InData->SpellCastInfo.wSpellID)
		{
			pc->WaitingForTheAbilityRespond = 0;
			pc->WaitingForTheAbilityBuf = -1.0f;
			FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(-1.0f);
		}
	}

	UINT DataKey = CombinateID(InData->SpellCastInfo.wSpellID, InData->SpellCastInfo.bySpellLevel, InData->SpellCastInfo.byRuneSlot);
	UMMOAbility* abilityData = GetAbilityData(DataKey);
	if (!abilityData)
	{
		return;
	}

	//AActor* Actor = MMOFacade::getSingleton().GetActor(InData->SpellCastInfo.dwCasterID);
	//AMMOActor* mmoActor = Cast<AMMOActor>(Actor);

	LONG64 Skey = CombinateID64(InData->SpellCastInfo.wSpellID, InData->SpellCastInfo.dwCasterID, InData->SpellCastInfo.bySerial);
	STLLong64ToAbilityNodeMap::iterator pos = AbilityList.find(Skey);

	if (pos == AbilityList.end())
	{
		return;
	}

	//pos->second->PushCastEffect();
	pos->second->ClearSpellComponents();

	FLOAT DataTime = 0.f;
	if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type1)	  // Channeling
	{
		DataTime = abilityData->SpellCommonAttribute.Interval;
	}
	else
	{
		DataTime = abilityData->SpellCommonAttribute.HardTime;
	}

	pos->second->OverrideCastTime = DataTime * pos->second->TimeScaleByWeaponType / pos->second->SpeedScale;
	pos->second->OverrideCastTime = pos->second->NormalizeTime(pos->second->OverrideCastTime);

	FLOAT OldRate = pos->second->Rate;

	if (pos->second->OverrideCastTime <= 0.0001)
	{
		pos->second->Rate = 1.0f;
	}
	else
	{
		DataTime *= pos->second->TimeScaleByWeaponType;
		pos->second->Rate = DataTime / pos->second->OverrideCastTime;
	}

	//pos->second->Rate = DEBUG_RATE;

	if (OldRate != pos->second->Rate)
	{
		pos->second->RefreshAutoDestroyComponentsRate();
	}



#if SHOW_ABILITY_LOG
	debugf(TEXT("OverrideCastTime=%f Rate=%f"), pos->second->OverrideCastTime, pos->second->Rate);
#endif

	// fix spell time

	

	if (mmoActor && mmoActor->IsMainPlayer())
	{		
		if (abilityData->SpellCommonAttribute.CastType == E_spct_Active)
		{
			mmoActor->CurrentAbilityState = EAS_Cast;
		}

		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		check(pc);

		if(pc->WaitingForTheAbilityRespond == InData->SpellCastInfo.wSpellID)
		{
			//warnf(NAME_Warning, TEXT("2--------pc->WaitingForTheAbilityRespond = 0"));

			pc->WaitingForTheAbilityRespond = 0;
			pc->WaitingForTheAbilityBuf = -1.0f;
			FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(-1.0f);
		}

		//mmoActor->HideWeapon(abilityData->ClientCommonAttribute.bHideWeaponInCast);

		if (abilityData->ClientCommonAttribute.bShowLeadUI)
		{
			if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type1)
			{
				if (pos->second->State == EAS_Spell)
				{
					pc->OnAbilityTimerBarStart(abilityData, 1.0f, pos->second->OverrideCastTime * 1000.f );//TotalTime, 0);
				}
			}
			else if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type2)
			{
				pc->OnAbilityTimerBarEnd(abilityData,FALSE);
			}
			else if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type5)
			{
				pc->OnAbilityTimerBarEnd(abilityData,FALSE);
			}

		}
	}

	// There may be projectile effects that need to be simulated
	if (InData->byTargetCount == 0)
	{
		UAbilityHitPointComponent* hitData = NULL;
		if (pos->second->PrepareEffectList[ON_CAST_HIT_POINT_INDEX])
		{
			hitData = pos->second->PrepareEffectList[ON_CAST_HIT_POINT_INDEX]->HitPointData;
		}
		if (hitData && hitData->bShowTheFirstBullet)
		{
			PushBullet(abilityData, hitData, InData->SpellCastInfo.dwCasterID, Actor, NULL, pos->second->TargetPoint, 0);
		}
	}

	if (pos->second->PrepareEffectList[ON_CAST_HIT_POINT_INDEX])
	{
		pos->second->PrepareEffectList[ON_CAST_HIT_POINT_INDEX]->bActive = TRUE;
		pos->second->PrepareEffectList[ON_CAST_HIT_POINT_INDEX]->Id = InData->SpellCastInfo.byCastID;
	}

	pos->second->State = EAS_Cast;
	pos->second->CastCurrentTime = 0.f;

	OnProcessTargetList(&InData->SpellCastInfo, ON_CAST_HIT_POINT_INDEX, InData->byTargetCount, InData->astTargetList);
}

void AbilityProcessor::OnProcessTargetList(SPELL_CAST_INFO* InInfo, INT InEffectSerial, INT TargetCount, TARGET_DATA* InData)
{
	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_ABILITY_LOG
	debugf(L"OnRecvExCast CasterId=%d, TargetCount=%d", InInfo->dwCasterID, TargetCount);
#endif

	check(InEffectSerial >= 0 && InEffectSerial <= 5);

	UINT key = CombinateID(InInfo->wSpellID, InInfo->bySpellLevel, InInfo->byRuneSlot);
	UMMOAbility* abilityData = GetAbilityData(key);
	if (abilityData == NULL)
	{
		warnf(TEXT("can not find ability data by id(%d), Level(%d), RuneSlot(%d)"), InInfo->wSpellID, InInfo->bySpellLevel, InInfo->byRuneSlot);
		return;
	}

	AActor* CreatorActor = MMOFacade::getSingleton().GetActor(InInfo->dwCasterID);
	AActor* BaseActor = NULL;

	BYTE Type = EWT_Weapon_0;
	if (CreatorActor && CreatorActor->IsA(AMMOActor::StaticClass()))
	{
		Type = ((AMMOActor*)CreatorActor)->GetWeaponType();
	}
	UAbilityHitPointComponent* hitData = abilityData->GetAbilityHitPointDataByWeaponType(Type, InEffectSerial);

	for (INT i = 0; i < TargetCount; ++i)
	{
		AActor* Target = MMOFacade::getSingleton().GetActor(InData[i].dwTargetID);

		EffectNode *node = new EffectNode;
		node->AbilityID = InInfo->wSpellID;
		node->AbilityLevel = InInfo->bySpellLevel;
		node->AbilityRune = InInfo->byRuneSlot;
		node->CasterID = InInfo->dwCasterID;

		node->Param1 = 0;
		node->Param2 = 0;
		node->TargetID = InData[i].dwTargetID;
		node->BaseActorId = InInfo->dwCasterID;
		node->SerialId = InInfo->bySerial;
		INT Flag = InData[i].wFlag;
		node->Flag =  Flag & (1 << sfbBlock);
		node->Flag += Flag & (1 << sfbDodge);
		node->Flag += Flag & (1 << sfbParry);
		node->Flag += Flag & (1 << sfbImmune);
		node->CastId = InInfo->byCastID;
		node->Index = InEffectSerial;
		node->BaseAbilityNode = NULL;

		//if (node->Flag &(1 <<sfbDodge))
		//{
		//	debugf(L"OnProcessTargetList sfbDodge Flag=%d", Flag);
		//}

		LONG64 akey = CombinateID64(InInfo->wSpellID, InInfo->dwCasterID, InInfo->bySerial);
		AbilityNode* aNode = GetAbilityNode(akey);

		/*UAbilityHitPointComponent* hitData = NULL;*/
		FLOAT Duration = FLOAT(InData[i].wEffectDelayTime) / 1000.f;

		if (aNode == NULL)
		{
			PushEffect(node);
			delete node;
			node = NULL;

			if (Target && BaseActor && hitData)
			{
				if ((hitData->bShowTheFirstBullet) || i != 0)
				{
					if (hitData->bLinkFromLastTarget)
					{
						if (i > 0)
						{
							BaseActor = MMOFacade::getSingleton().GetActor(InData[i - 1].dwTargetID);
						}
					}
					else
					{
						BaseActor = CreatorActor;
					}
					PushBullet(abilityData, hitData, InInfo->dwCasterID, BaseActor, Target, Target->Location, Duration);
				}
			}

			continue;
		}

		if (aNode->PrepareEffectList[InEffectSerial])
		{
			hitData = aNode->PrepareEffectList[InEffectSerial]->HitPointData;
		}

		if (hitData && (hitData->bShowTheFirstBullet || i != 0))
		{
			if (hitData->bLinkFromLastTarget && i > 0)
			{
				BaseActor = MMOFacade::getSingleton().GetActor(InData[i - 1].dwTargetID);
			}
			else
			{
				BaseActor = CreatorActor;
			}
			PushBullet(abilityData, hitData, InInfo->dwCasterID, BaseActor, Target, aNode->TargetPoint, Duration);
		}


		// do not need to process
		if (node->Flag == 0)
		{
			delete node;
			continue;;
		}

		aNode->AddEffectNode(node);
	}

}

/*
struct TARGET_DATA
{
UINT32           dwTargetID;
UINT16           wFlag;
UINT16           wEffectDelayTime;
};

struct G2C_CHAR_EX_CAST_SPELL : FLEX_MESSAGE_HEADER
{
UINT32           dwCasterID;
UINT16           wSpellID;
UINT8            bySpellLevel;
UINT8            byRuneSlot;
UINT8            bySerial;
UINT8            byCastID;
UINT8			 byEffectSerial;
UINT8			 byTargetCount;
TARGET_DATA		 astTargetList[0];
};
*/
void AbilityProcessor::OnRecvExCast(G2C_CHAR_EX_CAST_SPELL* InData)
{
	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_ABILITY_LOG
	warnf(NAME_Log, TEXT("G2C_CHAR_EX_CAST_SPELL CasterId=%d, TargetCount=%d"), 
		InData->SpellCastInfo.dwCasterID, InData->byTargetCount);
#endif

	OnProcessTargetList(&InData->SpellCastInfo, InData->byEffectSerial, InData->byTargetCount, InData->astTargetList);

	//check(InData->byEffectSerial >= 0 && InData->byEffectSerial <= 4);
	//
	//UINT key = CombinateID(InData->wSpellID, InData->bySpellLevel, InData->byRuneSlot);
	//UMMOAbility* abilityData = GetAbilityData(key);
	//if (abilityData == NULL)
	//{
	//	warnf(TEXT("can not find ability data by id(%d), Level(%d), RuneSlot(%d)"), InData->wSpellID, InData->bySpellLevel, InData->byRuneSlot);
	//	return;
	//}
	//AActor* BaseActor = MMOFacade::getSingleton().GetActor(InData->dwCasterID);

	//for (INT i = 0; i < InData->byTargetCount; ++i)
	//{
	//	AActor* Target = MMOFacade::getSingleton().GetActor(InData->astTargetList[i].dwTargetID);

	//	EffectNode *node = new EffectNode;
	//	node->AbilityID = InData->wSpellID;
	//	node->AbilityLevel = InData->bySpellLevel;
	//	node->AbilityRune = InData->byRuneSlot;
	//	node->CasterID = InData->dwCasterID;

	//	node->Param1 = 0;
	//	node->Param2 = 0;
	//	node->TargetID = InData->astTargetList[i].dwTargetID;
	//	node->PreviousTargetID = 0;
	//	node->SerialId = InData->bySerial;
	//	INT Flag = InData->astTargetList[i].wFlag;
	//	node->Flag =  Flag & (1 << sfbBlock);
	//	node->Flag += Flag & (1 << sfbDodge);
	//	node->Flag += Flag & (1 << sfbParry);
	//	node->Flag += Flag & (1 << sfbImmune);
	//	node->CastId = InData->byCastID;
	//	node->Index = InData->byEffectSerial;
	//	node->BaseAbilityNode = NULL;

	//	LONG64 akey = CombinateID64(InData->wSpellID, InData->dwCasterID, InData->bySerial);
	//	AbilityNode* aNode = GetAbilityNode(akey);

	//	UAbilityHitPointComponent* hitData = NULL;
	//	FLOAT Duration = FLOAT(InData->astTargetList[i].wEffectDelayTime) / 1000.f;

	//	if (aNode == NULL)
	//	{
	//		PushEffect(node);
	//		delete node;
	//		node = NULL;

	//		if (Target && BaseActor && abilityData)
	//		{
	//			BYTE Type = EWT_Weapon_0;
	//			if (BaseActor->IsA(AMMOActor::StaticClass()))
	//			{
	//				Type = ((AMMOActor*)BaseActor)->GetWeaponType();
	//			}
	//			hitData = abilityData->GetAbilityHitPointDataByWeaponType(Type, InData->byEffectSerial);

	//			if ((hitData && hitData->bShowTheFirstBullet) || i != 0)
	//			{
	//				PushBullet(abilityData, hitData, BaseActor, Target, Target->Location, Duration);
	//			}
	//		}

	//		return;
	//	}

	//	if (aNode->PrepareEffectList[InData->byEffectSerial])
	//	{
	//		hitData = aNode->PrepareEffectList[InData->byEffectSerial]->HitPointData;
	//	}

	//	if (hitData && (hitData->bShowTheFirstBullet || i != 0))
	//	{
	//		PushBullet(abilityData, hitData, BaseActor, Target, aNode->TargetPoint, Duration);
	//	}


	//	// do not need to process
	//	if (node->Flag == 0)
	//	{
	//		delete node;
	//		return;
	//	}

	//	aNode->AddEffectNode(node);
	//}



	//AActor* Target = MMOFacade::getSingleton().GetActor(TargetId);

	//if (abilityData == NULL)
	//{
	//	warnf(TEXT("can not find ability data by id(%d), Level(%d), RuneSlot(%d)"), AbilityId, AbilityLevel,AbilityRune);
	//	return;
	//}

	//EffectNode *node = new EffectNode;
	//node->AbilityID = AbilityId;
	//node->AbilityLevel = AbilityLevel;
	//node->AbilityRune = AbilityRune;
	//node->CasterID = CasterId;

	//node->Param1 = 0;
	//node->Param2 = 0;
	//node->TargetID = TargetId;
	//node->PreviousTargetID = 0;
	//node->SerialId = SerialId;
	//node->Flag = Flag & (1 << sfbBlock);
	//node->Flag += Flag & (1 << sfbDodge);
	//node->Flag += Flag & (1 << sfbParry);
	//node->Flag += Flag & (1 << sfbImmune);
	//node->CastId = CastId;
	//node->Index = EffectSerial;
	//node->BaseAbilityNode = NULL;

	//LONG64 akey = CombinateID64(AbilityId, CasterId, SerialId);
	//AbilityNode* aNode = GetAbilityNode(akey);

	//UAbilityHitPointComponent* hitData = NULL;

	//if (aNode == NULL)
	//{
	//	PushEffect(node);
	//	delete node;
	//	node = NULL;

	//	if (Target && BaseActor && abilityData)
	//	{
	//		FLOAT Duration = FLOAT(LastTime) / 1000.f;
	//		BYTE Type = EWT_Weapon_0;
	//		if (BaseActor->IsA(AMMOActor::StaticClass()))
	//		{
	//			Type = ((AMMOActor*)BaseActor)->GetWeaponType();
	//		}
	//		hitData = abilityData->GetAbilityHitPointDataByWeaponType(Type, EffectSerial);

	//		if ((hitData && hitData->bShowTheFirstBullet) || InIndex != 0)
	//		{
	//			PushBullet(abilityData, hitData, BaseActor, Target, Target->Location, Duration);
	//		}
	//	}

	//	return;
	//}

	//FLOAT Duration = FLOAT(LastTime) / 1000.f;
	//
	//if (aNode->PrepareEffectList[EffectSerial])
	//{
	//	hitData = aNode->PrepareEffectList[EffectSerial]->HitPointData;
	//}

	//if (hitData && (hitData->bShowTheFirstBullet || InIndex != 0))
	//{
	//	PushBullet(abilityData, hitData, BaseActor, Target, aNode->TargetPoint, Duration);
	//}
	//

	//// do not need to process
	//if (node->Flag == 0)
	//{
	//	delete node;
	//	return;
	//}

	//aNode->AddEffectNode(node);
}

UBOOL AbilityProcessor::LineCheck(AActor* SourceActor,FVector& StartPoint, FVector& EndPoint, FVector& HitPoint, FVector& HitNormal, FVector TraceExtend)
{
	//if (SourceActor == NULL)
	//{
	//	return FALSE;
	//}

	//FMemMark Mark(GMainThreadMemStack);
	//UBOOL bFind = FALSE;
	//DWORD TraceFlags = TRACE_World | TRACE_Visible;
	//FCheckResult* Hit = GWorld->MultiLineCheck( GMainThreadMemStack, EndPoint, StartPoint,FVector(0.f), TraceFlags, SourceActor);
	//while(Hit)
	//{
	//	if (Hit->Actor == SourceActor)
	//	{
	//		bFind = FALSE;
	//	}
	//	else
	//	{
	//		AMMOActor* HitActor = Cast<AMMOActor>(Hit->Actor);
	//		if (HitActor && HitActor->bDead)
	//		{
	//			bFind = FALSE;
	//		}
	//		else
	//		{
	//			bFind = TRUE;
	//		}
	//	}

	//	if (bFind)
	//	{
	//		HitPoint = Hit->Location;
	//		HitNormal = Hit->Normal;
	//		break;
	//	}
	//	else
	//	{
	//		Hit = Hit->GetNext();
	//	}
	//}
	//Mark.Pop();

	//return bFind;

	
	if (!SourceActor)
	{
		return FALSE;
	}

	UBOOL bFind = FALSE;

	AMMOActor* mmoSource = Cast<AMMOActor>(SourceActor);
	DWORD TraceFlags = TRACE_World | TRACE_Visible | TRACE_Pawns | TRACE_Others;
	AActor* TraceActor = SourceActor;

	HitNormal = (StartPoint - EndPoint).SafeNormal();

	FMemMark Mark(GMainThreadMemStack);
	FCheckResult* Hit = GWorld->MultiLineCheck( GMainThreadMemStack, EndPoint, StartPoint, TraceExtend, TraceFlags, SourceActor);
	while(Hit)
	{
		if (Hit->Actor == SourceActor)
		{
			bFind = FALSE;
		}
		else
		{
			AMMOActor* HitActor = Cast<AMMOActor>(Hit->Actor);
			if (HitActor)
			{
				if (HitActor->bDead)
				{
					bFind = FALSE;
				}
				else
				{
					if (mmoSource && mmoSource->IsMainPlayer())
					{
						bFind = MMOFacade::getSingleton().IsAttackable(mmoSource,HitActor);
					}

				}
			}
			else
			{
				bFind = TRUE;
			}
		}

		if (bFind)
		{
			HitPoint = Hit->Location;
			//if (mmoSource)
			//{
			//	Dst.Z += mmoSource->GetCollisionHeight() * 0.5f;
			//}
			HitNormal = Hit->Normal;
			break;
		}
		else
		{
			Hit = Hit->GetNext();
		}
	}
	Mark.Pop();

	return bFind;
}

extern FServerGridWorld gServerGridWorld;
void AbilityProcessor::PushBullet(UMMOAbility* AbilityData, UAbilityHitPointComponent* HitData, INT CreatorId, AActor* BaseActor, AActor* TargetActor, FVector& TargetPoint, FLOAT Duration, FLOAT InStartTime)
{
	if (BaseActor == NULL)
	{
		warnf(L"PushBullet BaseActor is null");
		return;
	}
	UAbilityHitPointComponent* comp = HitData;

	if (comp)
	{
		for (INT i = 0; i < comp->BulletComponents.Num(); ++i)
		{
			if (comp->BulletComponents(i))
			{
				FLOAT lt = Duration;

				FVector TargetLocation = TargetPoint;
				FLOAT InMaxDistance = AbilityData->SpellCommonAttribute.MaxDistance;

				if (comp->BulletComponents(i)->EmitterType == EMIT_ConnectTo)
				{
					if (TargetActor == NULL)
					{
						continue;
					}
				}
				else if (comp->BulletComponents(i)->EmitterType == EMIT_FlyTo)
				{
					if (comp->BulletComponents(i)->ProjectileTargetType == EPTT_Actor)
					{
						if (TargetActor == NULL)
						{
							FVector EndPoint = BaseActor->Location + AbilityData->SpellCommonAttribute.MaxDistance * BaseActor->Rotation.Vector();
							if (comp->BulletComponents(i)->bHaveCollisionCheck)
							{
								FVector StartPoint = BaseActor->Location;
								FVector HitPoint, HitNormal;
								
								// fix bug #23747
								// Without a lock-on, a projectile is blocked by every Actor with Collision; if that Actor is a character that takes no damage, it looks very strange
								// The server grid cells happen to have bBlockBullet, so query the grid cell data directly instead
								/*
								UBOOL bFind = LineCheck(BaseActor, StartPoint, EndPoint, HitPoint, HitNormal);
								if (bFind)
								{
									TargetLocation = HitPoint;
								}
								else
								{
									TargetLocation = EndPoint;
								}
								*/
								TargetLocation = gServerGridWorld.GetHitPoint(StartPoint, EndPoint);
							}
							else
							{
								TargetLocation = EndPoint;
							}
	
							if (AbilityData->SpellCommonAttribute.Speed < 0.001)
							{
								AbilityData->SpellCommonAttribute.Speed = 1;
								warnf(L"Ability speed < 0.001   id=%d", AbilityData->SpellCommonAttribute.Id);
							}
							FLOAT MaxDis = (TargetLocation - BaseActor->Location).Size();
							lt = MaxDis / FLOAT(AbilityData->SpellCommonAttribute.Speed);
							InMaxDistance = MaxDis;
						}
					}
				}

				if (lt < 0.0001)
				{
					lt = 0.15;
				}
				if (comp->bLinkFromLastTarget)
				{
					lt = comp->BulletComponents(i)->Duration;
				}
				comp->BulletComponents(i)->TrackTimeType = ETTT_ConstTime;
				comp->BulletComponents(i)->Duration = lt;
				comp->BulletComponents(i)->FlyTime = lt;
				comp->BulletComponents(i)->Speed = AbilityData->SpellCommonAttribute.Speed;
				comp->PushHitPointEffects(comp->BulletComponents(i), CreatorId, BaseActor, TargetActor, TargetLocation, 1.0, InMaxDistance);
			}
		}
	}
}

void AbilityProcessor::OnRecvEffect(UINT CreatorId, UINT AbilityID, UINT AbilityLevel,  UINT AbilityRune, UINT TargetID, INT Param1, INT Param2, UINT Flag, UINT InMissileEffect, UINT SerialId, UINT CastId, UINT EffectSerial)
{
	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_ABILITY_LOG
	warnf(NAME_Log, TEXT("OnRecvEffect CreatorId%d, abilityId=%d, TargetId=%d, Flag=%d"), CreatorId, AbilityID, TargetID, Flag);
#endif

	AMMOActor* target = MMOFacade::getSingleton().GetCharacter(TargetID);

	//if (Flag &(1<<sfbDodge))
	//{
	//	 debugf(L"OnRecvEffect flag=%d", Flag);
	//}
	
	EffectNode *node = new EffectNode;
	node->AbilityID = AbilityID;
	node->AbilityLevel = AbilityLevel;
	node->AbilityRune = AbilityRune;
	node->CasterID = CreatorId;
	node->CastId = CastId;
	node->Flag = Flag;
	node->Param1 = Param1;
	node->Param2 = Param2;
	node->TargetID = TargetID;
	node->BaseActorId = CreatorId;
	node->SerialId = SerialId;
	node->Index = EffectSerial;
	node->BaseAbilityNode = NULL;



	//Update player damage value
	MMOHurtManager::getSingleton().UpdateAllHurt(CreatorId,node->Param1);



	if (InMissileEffect)
	{
		PushEffect(node);
		delete node;
		node = NULL;
		return;
	}
	else
	{
		node->Flag &= ~(1 << sfbBlock);
		node->Flag &= ~(1 << sfbDodge);
		node->Flag &= ~(1 << sfbParry);
		node->Flag &= ~(1 << sfbImmune);
	}




	UBOOL bDead = ((Flag & (1 << sfbDead)) > 0) ? TRUE : FALSE;
	if (bDead > 0)
	{
		debugf(L"npc Dead");
	}


	LONG64 Skey = CombinateID64(AbilityID, CreatorId, SerialId);
	AbilityNode* aNode = GetAbilityNode(Skey);
	if (!aNode)
	{
		PushEffect(node);
		delete node;
		node = NULL;
	}
	else
	{
		aNode->AddEffectNode(node);
	}

	// add by zhangrui05
	if (target && target->IsA(ANPC::StaticClass()))
	{
		ANPC* npc = Cast<ANPC>(target);
		if (npc)
		{
			EActorClassType actorType = (EActorClassType)npc->GetActorType();
			if (actorType == EACT_Monster || actorType == EACT_Boss)
			{
				AMMOActor* creator = MMOFacade::getSingleton().GetCharacter(CreatorId);
				AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
				if (creator && player && creator->ServerID == player->ServerID)
				{
					// If the monster was killed by a Buff, the server does not tell whether it died
					/*
					if ((Flag & (1 << sfbDead)) > 0)
					{
						MMOGameEvent_Guide evtGuide(EGUET_KillMonster, npc->GetTemplateID());
						MMOUI::PostGameEvent(&evtGuide);
					}
					*/
					MMOGameEvent_Guide evtGuide(EGUET_KillMonster, npc->GetTemplateID());
					MMOUI::PostGameEvent(&evtGuide);

					if (actorType == EACT_Monster)
					{
						MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_KillMonster, npc->GetTemplateID());
						MMOUI::PostGameEvent(&evtJourney);
					}
					else if (actorType == EACT_Boss)
					{
						MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_KillBoss, npc->GetTemplateID());
						MMOUI::PostGameEvent(&evtJourney);
					}
				}
			}
		}
	}
}

void AbilityProcessor::OnRecvBreakCast(G2C_CHAR_BREAK_CAST* InData)
{
	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_ABILITY_LOG
	warnf(NAME_Log, TEXT("OnRecvBreakCast CasterID=%d, AbilityID=%d, AbilityLevel=%d, SerialId=%d"), 
		InData->SpellCastInfo.dwCasterID, InData->SpellCastInfo.wSpellID, InData->SpellCastInfo.bySpellLevel, InData->byIsBreak);
#endif
	

	UINT key = CombinateID(InData->SpellCastInfo.wSpellID, InData->SpellCastInfo.bySpellLevel, InData->SpellCastInfo.byRuneSlot);
	UMMOAbility* abilityData = GetAbilityData(key);
	if (abilityData == NULL)
	{
		warnf(TEXT("Can not find Ability template data by %d, %d, %d"), InData->SpellCastInfo.wSpellID, InData->SpellCastInfo.bySpellLevel, InData->SpellCastInfo.byRuneSlot);
		return;
	}
	AMMOActor* mmoActor = MMOFacade::getSingleton().GetCharacter(InData->SpellCastInfo.dwCasterID);
	if (abilityData->SpellCommonAttribute.CastType == E_spct_Active && mmoActor && mmoActor->IsMainPlayer())
	{
		//if (InData->byIsBreak)
		//{
		//	mmoActor->CurrentAbilityState = EAS_End;
		//}
		//else
		//{
		//	mmoActor->CurrentAbilityState = EAS_Cast;
		//}
	}

	LONG64 Skey = CombinateID64(InData->SpellCastInfo.wSpellID, InData->SpellCastInfo.dwCasterID, InData->SpellCastInfo.bySerial);
	STLLong64ToAbilityNodeMap::iterator pos = AbilityList.find(Skey);
	if (pos == AbilityList.end())
	{
		return;
	}

	FName LoopAnim = pos->second->LoopAnimToStop;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(pos->second->SelfID);
	if (actor && actor->IsMainPlayer())
	{
		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		check(pc);
		if (pos->second->AbilityData->SpellCommonAttribute.PrepareType == ES_PT_Type1)	// Channeling
		{
			pc->OnAbilityTimerBarEnd(pos->second->AbilityData, InData->byIsBreak);
		}
		else if (pos->second->AbilityData->SpellCommonAttribute.PrepareType == ES_PT_Type2)
		{
			pc->OnAbilityTimerBarEnd(pos->second->AbilityData,TRUE);
		}
		else if (pos->second->AbilityData->SpellCommonAttribute.PrepareType == ES_PT_Type5)
		{
			pc->OnAbilityTimerBarEnd(pos->second->AbilityData,TRUE);
		}

		//if (pos->second->AbilityData->ClientCommonAttribute.bHideWeaponInCast)
		//{
		//	actor->HideWeapon(FALSE);
		//}
		
	}

	if (InData->byIsBreak)
	{
		pos->second->bInterrupted = TRUE;
		pos->second->State = EAS_End;
		
	}
	else
	{
		pos->second->bLeadAbilityOver = TRUE;
		pos->second->bInterrupted = FALSE;
		pos->second->State = EAS_Cast;
		FLOAT ft = pos->second->AbilityData->SpellCommonAttribute.HardTime * pos->second->TimeScaleByWeaponType / pos->second->SpeedScale;
		pos->second->OverrideCastTime = pos->second->NormalizeTime(ft);
	}

	if (actor && LoopAnim != NAME_None)
	{
		actor->eventStopLoopAnim(LoopAnim);
	}

}

EDamageResultType AbilityProcessor::FlagConvertToClientType(UINT Flag)
{
	/*
	sfbCrit = sfbModifyDamageBegin,	// Critical hit
	sfbBlock,						// Block
	sfbDodge,						// Dodge
	sfbParry,						// Parry
	sfbMiss,						// Miss
	sfbNormal,						// Normal
	*/
	EDamageResultType Ret = DRT_Result0;
	Ret = (Flag & (1<<sfbCrit) ? DRT_Result7 : Ret);
	Ret = (Flag & (1<<sfbBlock) ? DRT_Result3 : Ret);
	Ret = (Flag & (1<<sfbDodge) ? DRT_Result5 : Ret);
	Ret = (Flag & (1<<sfbParry) ? DRT_Result4 : Ret);
	Ret = (Flag & (1<<sfbMiss) ? DRT_Result1 : Ret);
	Ret = (Flag & (1<<sfbNormal) ? DRT_Result6 : Ret);
	Ret = (Flag & (1<<sfbImmune) ? DRT_Result2 : Ret);
	Ret = (Flag & (1<<sfbEmptyEffect) ? DRT_Result8 : Ret);
	return Ret;
}

void AbilityProcessor::OnRecvError(INT ErrCode, INT Param1, INT Param2)
{
	AMMOActor* MainActor = MMOFacade::getSingleton().GetMainActor();
	if (MainActor)
	{
		if (MainActor->CurrentRequestAbilityId == Param1 && MainActor->CurrentRequestAbilitySerialId == Param2 && MainActor->CurrentAbilityState != EAS_End)
		{
			LONG64 Skey = CombinateID64(Param1, MainActor->ServerID, Param2);
			this->DeteleAbilityNode(Skey);
		}


		UINT key = CombinateID(Param1, 0, 0);
		UMMOAbility* pAbility = GetAbilityData(key);
		if(pAbility && pAbility->SpellCommonAttribute.CastType==E_spct_Active)
		{
			if (ErrCode == errSpellCasting)
			{
				MainActor->CurrentRequestAbilitySerialId = g_LastRequestAbilitySerialId;
				MainActor->CurrentRequestAbilityId = g_LastRequestAbilityId;
				g_LastRequestAbilitySerialId = 0;
				g_LastRequestAbilityId = 0;
			}
			else
			{
				MainActor->CurrentAbilityState = EAS_End;

				AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
				if (pc->WaitingForTheAbilityRespond == Param1)
				{
					pc->WaitingForTheAbilityRespond = 0;
					FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(-1.0f);
					pc->WaitingForTheAbilityBuf = -1.0f;
				}
			}
		}
		else
		{
			warnf(NAME_Log, TEXT("OnRecvError:received ability error message, not an active ability ErrCode=%d, Param1=%d, Param2=%d"), ErrCode, Param1, Param2);
		}
	}
}

UMMOAbility* AbilityProcessor::GetAbilityData(UINT Id)
{
	return MMOSkillManager::getSingleton().FindAbilityTemplate(Id);
}

AbilityNode* AbilityProcessor::GetAbilityNode(LONG64 Key)
{
	STLLong64ToAbilityNodeMap::iterator pos = AbilityList.find(Key);
	if (pos == AbilityList.end())
	{
		return NULL;
	}
	else
	{
		return pos->second;
	}
}

void AbilityProcessor::PushEffect(EffectNode* node)
{
#if SHOW_PUSH_EFFECT
	debugf(TEXT("PushEffect ability id=%d, caster=%d, target=%d, index=%d"), node->AbilityID, node->CasterID, node->TargetID,node->Index);

#endif
	AActor* target = MMOFacade::getSingleton().GetActor(node->TargetID);
	AMMOActor* mmoTarget = Cast<AMMOActor>(target);
	if (!mmoTarget)
	{
		return;
	}

	//if (mmoTarget && mmoTarget->IsA(ANPC::StaticClass()))
	//{
	//	debugf(L"catch");
	//}

	// fix bug #22491, for trap-type abilities (e.g. fire wall), the caster may have walked out of the 9-screen range and may be NULL here, but the damage effect is still needed, so handle it in later logic
	AActor* BaseActor = MMOFacade::getSingleton().GetActor(node->CasterID);

	// get the hit point data
	UINT key = CombinateID(node->AbilityID, node->AbilityLevel, node->AbilityRune);
	UMMOAbility* abilityData = GetAbilityData(key);
	if (abilityData == NULL)
	{
		warnf(TEXT("can not find ability data by id(%d), Level(%d)"), node->AbilityID, node->AbilityLevel);
		return;
	}

	EDamageResultType ResultType = FlagConvertToClientType(node->Flag);
	UBOOL bCauseDead = (node->Flag & (1<<sfbDead) ? TRUE : FALSE);
	FDamageEffectPair* dep = NULL;
	FDamageEffectPair* dep2 = NULL;
	FHitGroup HitGroup;
	HitGroup.WeaponType = HitGroup.DirectionType = HitGroup.WeaponHitType = 0;
	EWeaponType wType = EWT_Weapon_0;
	EElementaryType EleType = (EElementaryType)abilityData->SpellCommonAttribute.ElementaryType;
	if (BaseActor && BaseActor->IsA(AMMOActor::StaticClass()))
	{
		AMMOActor* mmoBaseActor = (AMMOActor*)BaseActor;
		wType = EWeaponType(mmoBaseActor->GetWeaponType());

		if ((EElementaryType)mmoBaseActor->ElementaryType != 0)
		{
			EleType = (EElementaryType)mmoBaseActor->ElementaryType;
		}
	}

	for (INT i = 0; i < abilityData->ClientCommonAttribute.HitGroupInfo.Num(); ++i)
	{
		if (wType == abilityData->ClientCommonAttribute.HitGroupInfo(i).WeaponType)
		{
			HitGroup = abilityData->ClientCommonAttribute.HitGroupInfo(i);
			break;
		}
	}

	if (ResultType == DRT_Result2 || ResultType == DRT_Result3 || ResultType == DRT_Result4 || ResultType == DRT_Result5)
	{
		dep = MMOFacade::getSingleton().GetDamgeTemplateData(ResultType, 0, 0, 0);
	}
	else if (ResultType == DRT_Result6)	// Normal attack
	{
		if (HitGroup.WeaponHitType == EWHT_HitType1
			|| HitGroup.WeaponHitType == EWHT_HitType2
			|| HitGroup.WeaponHitType == EWHT_HitType4
			|| HitGroup.WeaponHitType == EWHT_HitType5)
		{
			dep = MMOFacade::getSingleton().GetDamgeTemplateData(ResultType, HitGroup.WeaponHitType, HitGroup.DirectionType, 0);
		}
		else
		{
			dep = MMOFacade::getSingleton().GetDamgeTemplateData(ResultType, HitGroup.WeaponHitType, 0, 0);
		}

		dep2 = MMOFacade::getSingleton().GetDamgeTemplateData(ResultType, 0, 0, EleType);
	}
	else if (ResultType == DRT_Result7)	 // Explosive crit
	{
		dep = MMOFacade::getSingleton().GetDamgeTemplateData(ResultType, HitGroup.WeaponHitType, 0, 0);

		dep2 = MMOFacade::getSingleton().GetDamgeTemplateData(ResultType, 0, 0, EleType);
	}


	INT InBaseActorId = 0;
	INT InTargetId = 0;
	if (BaseActor)
	{
		if (BaseActor->IsA(AMMOActor::StaticClass()))
		{
			InBaseActorId = ((AMMOActor*)BaseActor)->ServerID;
		}
		else if (BaseActor->IsA(AMMOEmitter::StaticClass()))
		{
			InBaseActorId = ((AMMOEmitter*)BaseActor)->ServerID;
		}
	}

	if (target->IsA(AMMOActor::StaticClass()))
	{
		InTargetId = ((AMMOActor*)target)->ServerID;
	}
	else if (target->IsA(AMMOEmitter::StaticClass()))
	{
		InTargetId = ((AMMOEmitter*)target)->ServerID;
	}

	if (dep && dep->EffectList.Num() > 0)
	{
		for (INT i = 0; i < dep->EffectList.Num(); ++i)
		{
			BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(dep->EffectList(i), FALSE,InBaseActorId, InTargetId, InBaseActorId, target->Location, 1.0);
			if (NewNode)
			{
				NewNode->SetWeaponHitType(HitGroup.WeaponHitType);
				EffectProcessor::GetSingleton().AddActiveComponent(NewNode);
				NewNode->OnAction(TRUE);
			}
		}
	}

	if (dep2 && dep2->EffectList.Num() > 0)
	{
		for (INT i = 0; i < dep2->EffectList.Num(); ++i)
		{
			BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(dep2->EffectList(i), FALSE, InBaseActorId, InTargetId, InBaseActorId, target->Location, 1.0);
			if (NewNode)
			{
				NewNode->SetWeaponHitType(HitGroup.WeaponHitType);
				EffectProcessor::GetSingleton().AddActiveComponent(NewNode);
				NewNode->OnAction(TRUE);
			}
		}
	}

	// hit point effect

	UAbilityHitPointComponent* Comp = NULL;
	if (node->BaseAbilityNode)
	{
		if (node->Index >= 0 && node->Index < HIT_POINT_MAX_NUMBER)
		{
			if (node->BaseAbilityNode->PrepareEffectList[node->Index] && node->BaseAbilityNode->PrepareEffectList[node->Index]->HitPointData)
			{
				Comp = node->BaseAbilityNode->PrepareEffectList[node->Index]->HitPointData;
			}
		}
	}
	else
	{
		Comp = abilityData->GetAbilityHitPointDataByWeaponType(wType, node->Index);
	}


	if(ResultType == DRT_Result3 ||ResultType == DRT_Result4 || ResultType == DRT_Result6 || ResultType == DRT_Result7 || ResultType == DRT_Result8)
	{
		if (Comp && Comp->ServerEffectList.Num() > 0)
		{
			for (INT i = 0; i < Comp->ServerEffectList.Num(); ++i)
			{
				BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(Comp->ServerEffectList(i), TRUE, InBaseActorId, InTargetId, InBaseActorId, target->Location, 1.0);
				if (NewNode)
				{
					NewNode->SetWeaponHitType(HitGroup.WeaponHitType);
					EffectProcessor::GetSingleton().AddActiveComponent(NewNode);
					NewNode->OnAction(TRUE);
				}
			}
		}
	}


	FDamageData DamageData;
	DamageData.AbilityData = Comp;
	//DamageData.Attacker = MMOFacade::getSingleton().GetActor(node->CasterID);
	DamageData.CreatorId = node->CasterID;
	DamageData.BaseActor = MMOFacade::getSingleton().GetActor(node->BaseActorId);
	DamageData.bCauseDead = ((node->Flag) & (1 << sfbDead)) > 0 ? TRUE : FALSE;
	//if (DamageData.bCauseDead)
	//{
	//	debugf(L"NpcId%d Dead", mmoTarget->ServerID);
	//}
	
	DamageData.DamageValue = node->Param1;
	DamageData.bServerDamageData = TRUE;
	DamageData.DamageType = ResultType;
	DamageData.ElementaryType = EleType;
	mmoTarget->eventProcessDamage(DamageData);
	

	// blood effect
	if (mmoTarget)
	{
		mmoTarget->PlayBloodEffect(ResultType, bCauseDead);
	}

}

void AbilityProcessor::PushEffect(LONG64 Key)
{
	STLLong64ToAbilityNodeMap::iterator pos = AbilityList.find(Key);
	if (pos != AbilityList.end())
	{
		//for (size_t i = 0; i < pos->second->CastList.size(); ++i)
		//{
		//	for (size_t j = 0; j < pos->second->CastList[i]->EffectList.size(); ++j)
		//	{
		//		PushEffect(pos->second->CastList[i]->EffectList[j]);
		//	}
		//}

		//for (size_t i = 0; i < pos->second->CastList.size(); ++i)
		//{
		//	pos->second->CastList[i]->EffectList.clear();
		//}
		//pos->second->CastList.clear();
		//AbilityList.erase(Key);

		for (INT i = 0; i < HIT_POINT_MAX_NUMBER; ++i)
		{
			pos->second->PushHitEffect(i);
		}
	}
}


void AbilityProcessor::OnMissileAppear(INT Id, INT InMissileID, INT CreatorId, FLOAT InSpeed, FVector& Src, FRotator& InRot, FLOAT InLifeSpan)
{
	SCOPE_CYCLE_COUNTER(STAT_AbilitySystemTime);

#if SHOW_MIssile_LOG
	debugf(TEXT("MissileAppear Id=%d, TemplateId=%d InSpeed=%f"), Id, InMissileID, InSpeed);
#endif

	AMMOEmitter* proActor = MMOFacade::getSingleton().CreateEmitter(Id, InMissileID);
	if (!proActor)
	{
		return;
	}

	proActor->OverwriteEmitterType = EMIT_FlyTo;

	proActor->DelayToKill = Clamp(proActor->DelayToKill, 0.f, 5.f);


	

	proActor->Duration = InLifeSpan;
	proActor->MissileDst = InRot.Vector().SafeNormal() * proActor->Speed * InLifeSpan + Src;
	//proActor->MissileDst = InRot.Vector().SafeNormal() * InSpeed * InLifeSpan + Src;
	AMMOActor* CreatorActor = MMOFacade::getSingleton().GetCharacter(CreatorId);
	proActor->Creator = CreatorActor;

	FVector ToUseSrc = Src;
	FRotator ToUseRot = InRot;

	if (CreatorActor && proActor->MissileData && proActor->MissileData->SocketName != NAME_None)
	{
		UBOOL bFound = FALSE;
		USkeletalMeshComponent* Mesh = CreatorActor->OverriddenMesh ? CreatorActor->OverriddenMesh : CreatorActor->Mesh;
		FVector OutLocation;
		FRotator OutRotation;
		if (Mesh)
		{
			bFound = Mesh->GetSocketWorldLocationAndRotation(proActor->MissileData->SocketName, OutLocation, &OutRotation, 0);
			if (bFound)
			{
				ToUseSrc = OutLocation;
			}
		}

		if (bFound && proActor->MissileData->MoveType == E_mmt_AccordingToTerrain)
		{
			proActor->DistanceToLand = ToUseSrc.Z - CreatorActor->Location.Z + CreatorActor->GetCollisionHeight();
		}

		if (bFound)
		{
			ToUseRot = (proActor->MissileDst - ToUseSrc).Rotation();
		}
	}


	FLOAT gridWidth = FLOAT(GRID_WIDTH / POINT_PER_CM);
	FLOAT sp = 204.8;
	if (proActor->MissileData->Speed > 0)
	{
		sp = InSpeed * gridWidth / (FLOAT(proActor->MissileData->Speed) / 1000.0f);
	}

	proActor->Speed = sp;   //InSpeed;

	proActor->SetLocation(ToUseSrc);
	proActor->SetRotation(ToUseRot);
	proActor->SpawnMatrix = FRotationTranslationMatrix(InRot, Src);
	proActor->BaseActorMatrix = FMatrix::Identity;
	proActor->CurrentTime = 0.f;
	proActor->BeginPlay(proActor->Creator,NULL,NULL, proActor->MissileDst, 1.0, 0);


	//proActor->BuildNavMeshObstacle(proActor->MissileData);
	proActor->SetupCollision(proActor->MissileData);
}


void AbilityProcessor::OnMissileChangeMovement(INT Id, FVector& Dst, FRotator& InRot, FLOAT CostTime)
{
	AMMOEmitter* Emitter = MMOFacade::getSingleton().GetEmitter(Id);
	if (Emitter && Emitter->IsPendingKill() == FALSE)
	{
		//Emitter->LifeSpan = CostTime;
		Emitter->Speed = (Dst - Emitter->Location).Size() / CostTime;
		Emitter->MissileDst = Dst;
		Emitter->TargetPoint = Dst;
		Emitter->SetRotation((Dst - Emitter->Location).Rotation());
	}


}

//void AbilityProcessor::AddNewBuff(INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, INT StackCount)
//{
//	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(ObjId);
//	if (!actor)
//	{
//		 return;
//	}
//
//	UBuff* buff = MMOFacade::getSingleton().FindBuffTemplate(TemplateId, Level);
//	if (!buff)
//	{
//		warnf(L"can not find buff template data by id=%d and level=%d", TemplateId, Level);
//		return;
//	}
//
//	BuffData* newBuff = new BuffData;
//	if (!newBuff)
//	{
//		return;
//	}
//	newBuff->Id = Id;
//	newBuff->CreatorId = CreatorId;
//	newBuff->ObjId = ObjId;
//	newBuff->Buff = buff;
//	newBuff->CurrentTime = 0.f;
//	newBuff->Duration = Duration / 1000.f;
//	newBuff->StackCount = StackCount;
//
//	for (INT i = 0; i < buff->SingleEffectList.Num(); ++i)
//	{
//		if (buff->SingleEffectList(i) == NULL)
//		{
//			continue;
//		}
//		buff->SingleEffectList(i)->StopType = ACBT_StopWhenConstDurationPass;
//		buff->SingleEffectList(i)->Duration = Duration / 1000.f;
//		BaseComponent* NewComponent = AbilityProcessor::CreateDynamicComponent(buff->SingleEffectList(i), FALSE, ObjId, ObjId, actor->Location, 1.0);
//		if (!NewComponent)
//		{
//			continue;
//		}
//
//		EffectProcessor::GetSingleton().AddActiveComponent(NewComponent);
//		newBuff->SingleEffectComponentIds.push_back(NewComponent->ComponentId);
//
//		NewComponent->OnAction(TRUE);
//	}
//
//	for (INT i = 0; i < buff->OverlapEffectList.Num(); ++i)
//	{
//		if (buff->OverlapEffectList(i) == NULL)
//		{
//			continue;
//		}
//		buff->OverlapEffectList(i)->StopType = ACBT_StopWhenConstDurationPass;
//		buff->OverlapEffectList(i)->Duration = Duration / 1000.f;
//
//		newBuff->OverlapEffectComponentIds.push_back(IdNode());
//		for (INT j = 0; j < StackCount; ++j)
//		{
//			BaseComponent* NewComponent = AbilityProcessor::CreateDynamicComponent(buff->OverlapEffectList(i), FALSE, ObjId, ObjId, actor->Location, 1.0);
//			if (!NewComponent)
//			{
//				continue;
//			}
//
//			EffectProcessor::GetSingleton().AddActiveComponent(NewComponent);
//			newBuff->OverlapEffectComponentIds[i].Ids.push_back(NewComponent->ComponentId);
//			NewComponent->OnAction(TRUE);
//		}
//	}
//
//	EffectProcessor::GetSingleton().AddBuffNode(newBuff);
//
//	debugf(TEXT( "------------------------------------------------------------------AddNewBuff buffid = %d" ), TemplateId  );
//
//
//	//add by jimzhang 2012.7.18, add buff start and end state trigger behavior
//	buff->eventDoStart(actor);
//
//
//
//
//
//	if (buff->bQTEBuff)
//	{
//		AMMOPlayerController* controller = MMOFacade::getSingleton().GetMainController();
//		check(controller);
//		INT selfID = controller->GetPlayer()->ServerID;
//		if(newBuff->ObjId == selfID )
//		{
//			controller->OnQTEStart(newBuff->Buff,newBuff->CreatorId ,newBuff->Duration);
//		}
//	}
//
//
//}
//
//void AbilityProcessor::ChangeBuff(INT ObjId, INT Id, FLOAT Duration, INT StackCount)
//{
//	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(ObjId);
//	if (!actor)
//	{
//		return;
//	}
//
//	BuffData* buff = EffectProcessor::GetSingleton().GetActiveBuff(Id);
//	if (!buff)
//	{
//		return;
//	}
//
//	if (buff->OverlapEffectComponentIds.size() <= 0)
//	{
//		return;
//	}
//
//	if (StackCount < buff->StackCount)
//	{
//		for (size_t i = 0; i < buff->OverlapEffectComponentIds.size(); ++i)
//		{
//			for (INT j = 0; j < buff->StackCount - StackCount; ++j)
//			{
//				if (buff->OverlapEffectComponentIds[i].Ids.size() > 0)
//				{
//					INT IdCom = buff->OverlapEffectComponentIds[i].Ids.size();
//					IdCom = buff->OverlapEffectComponentIds[i].Ids[IdCom -1];
//					EffectProcessor::GetSingleton().DeleteActiveComponent(IdCom);
//				}
//			}
//		}
//	}
//
//	//for (INT i = 0; i < actor->BuffList.Num(); ++i)
//	//{
//	//	if (actor->BuffList(i).Id == Id)
//	//	{
//	//		FLOAT delta = Duration / 1000.f - actor->BuffList(i).MaxTime;
//	//		for (INT j = 0; j < actor->BuffList(i).BuffProjectile.Num(); ++j)
//	//		{
//	//			if (actor->BuffList(i).BuffProjectile(j) != NULL)
//	//			{
//	//				actor->BuffList(i).BuffProjectile(j)->Duration = actor->BuffList(i).BuffProjectile(j)->Duration += delta;
//	//				if (actor->BuffList(i).BuffProjectile(j)->Duration < actor->BuffList(i).CurrentTime)
//	//				{
//	//					actor->BuffList(i).BuffProjectile(j)->Duration = actor->BuffList(i).CurrentTime;
//	//				}
//	//			}
//	//		}
//
//	//		INT del = StackCount - actor->BuffList(i).StackCount;
//	//		if (del < 0)
//	//		{
//	//			if (actor->BuffList(i).BuffData->bParticleTheSameNumerAsStackCount)
//	//			{
//	//				for (INT k = 0; k < -del; ++k)
//	//				{
//	//					if (actor->BuffList(i).BuffProjectile.Num() > 0 && actor->BuffList(i).BuffProjectile(actor->BuffList(i).BuffProjectile.Num() - 1) != NULL)
//	//					{
//	//						GWorld->DestroyActor(actor->BuffList(i).BuffProjectile(actor->BuffList(i).BuffProjectile.Num() - 1));
//	//					}
//	//					if (actor->BuffList(i).BuffProjectile.Num() > 0)
//	//					{
//	//						actor->BuffList(i).BuffProjectile.Pop();
//	//					}
//	//				}
//	//			}
//	//			else
//	//			{
//	//				if (StackCount == 0)
//	//				{
//	//					if (actor->BuffList(i).BuffProjectile.Num() > 0)
//	//					{
//	//						for (INT k = 0; k < actor->BuffList(i).BuffProjectile.Num(); ++k)
//	//						{
//	//							if (actor->BuffList(i).BuffProjectile(k))
//	//							{
//	//								//GWorld->DestroyActor(actor->BuffList(i).BuffProjectile(k));
//	//								actor->BuffList(i).BuffProjectile(k)->eventKill(0.05);
//	//							}
//	//						}
//	//						actor->BuffList(i).BuffProjectile.Empty();
//	//					}
//	//				}
//	//			}
//
//	//			actor->BuffList(i).StackCount = StackCount;
//	//		}
//	//	}
//	//}
//}
//
//void AbilityProcessor::DeleteBuff(INT Id)
//{
//
//	BuffData* data = EffectProcessor::GetSingleton().GetActiveBuff(Id);
//	if (!data)
//	{
//		return;
//	}
//
//	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(data->ObjId);
//
//	if (data->Buff)
//	{
//		data->Buff->eventDoEnd(actor);
//		if (data->Buff->bQTEBuff)
//		{
//			AMMOPlayerController* controller = MMOFacade::getSingleton().GetMainController();
//			check(controller);
//			AMMOActor* creator = MMOFacade::getSingleton().GetCharacter(data->CreatorId);
//			controller->OnQTEEnd(data->Buff, creator);
//		}
//	}
//
//	EffectProcessor::GetSingleton().DeleteActiveBuff(Id);
//}

void AbilityProcessor::AddCoolDown(INT InType, INT32 InId, INT32 InValue)
{
	MMOCoolDownManager::getSingleton().AddNewCD( InType , InId , InValue );

	UMMOUISkillBar *pSkillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar );

	if (InType == COOLDOWN_GROUP)
	{
		AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
		if (!actor)
		{
			return;
		}

		for (INT i = 0; i < actor->Abilities.Num(); ++i)
		{
			UMMOAbility* TestAbility = actor->Abilities(i);
			if (!TestAbility)
			{
				continue;
			}

			for (INT j = 0; j < TestAbility->GroupIds.Num(); ++j)
			{
				if (TestAbility->GroupIds(j) == InId)
				{

					if(TestAbility->bCoolDown || InValue>0)
					{
						TestAbility->bCoolDown = TRUE;
					}
					TestAbility->CoolDownPercent = 1.0f;
					TestAbility->CoolDownInfo.GroupTimeMax = FLOAT(InValue) / 1000.f;
					TestAbility->CoolDownInfo.GroupTimeLeft = TestAbility->CoolDownInfo.GroupTimeMax;

					if( TestAbility->SlotHotKeyID >= 0 ){
						if( pSkillBarPanel != NULL ){
							//pSkillBar->SetAbilityCD(ability->SpellCommonAttribute.Id, ability->CoolDownInfo.SpellTimeMax ,TRUE );
							float leftTime = TestAbility->CoolDownInfo.SpellTimeLeft;
							leftTime = ( leftTime < TestAbility->CoolDownInfo.GroupTimeLeft ? TestAbility->CoolDownInfo.GroupTimeLeft : leftTime );

							float totalTime = TestAbility->CoolDownInfo.SpellTimeMax;
							totalTime = ( totalTime < TestAbility->CoolDownInfo.GroupTimeMax ? TestAbility->CoolDownInfo.GroupTimeMax : totalTime );

							pSkillBarPanel->SetAbilityCDInSlot( TestAbility->SlotHotKeyID, TestAbility->eventGetAbilityId(), leftTime*1000, totalTime*1000 );
						}
					}
				}
			}
		}
	}
	else if (InType == COOLDOWN_SPELL)
	{
		AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
		if (!actor)
		{
			return;
		}
		UMMOAbility* ability = NULL;
		for (INT i = 0; i < actor->Abilities.Num(); ++i)
		{
			if (actor->Abilities(i)->SpellCommonAttribute.Id == InId)
			{
				ability = actor->Abilities(i);
				break;
			}
		}

		if (!ability)
		{
			return;
		}

		if(ability->bCoolDown || InValue>0)
		{
			ability->bCoolDown = TRUE;
		}
		//ability->bCoolDown = TRUE;
		ability->CoolDownPercent = 1.0f;
		ability->CoolDownInfo.SpellTimeMax = ability->CoolDownInfo.SpellTimeLeft = FLOAT(InValue) / 1000.f;

		if( ability->SlotHotKeyID >= 0 ){
			if( pSkillBarPanel != NULL ){
				//pSkillBar->SetAbilityCD(ability->SpellCommonAttribute.Id, ability->CoolDownInfo.SpellTimeMax ,TRUE );
				float leftTime = ability->CoolDownInfo.SpellTimeLeft;
				leftTime = ( leftTime < ability->CoolDownInfo.GroupTimeLeft ? ability->CoolDownInfo.GroupTimeLeft : leftTime );

				float totalTime = ability->CoolDownInfo.SpellTimeMax;
				totalTime = ( totalTime < ability->CoolDownInfo.GroupTimeMax ? ability->CoolDownInfo.GroupTimeMax : totalTime );

				pSkillBarPanel->SetAbilityCDInSlot( ability->SlotHotKeyID, ability->eventGetAbilityId(), leftTime*1000, totalTime*1000 );
			}
		}

	}
	else if(InType == COOLDOWN_SPELL_EVENT_TRIGGER)
	{
		AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
		if (!actor)
		{
			return;
		}

		//INT32 AbilityID = InId;
		//UBuff* buff = MMOFacade::getSingleton().FindBuffTemplate(AbilityID*10+9, 0);
		//if (buff)
		//{
		//	MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, actor->ServerID, AbilityID*100+9, AbilityID*10+9, 0, FLOAT(InValue), FLOAT(InValue), 1,TRUE);
		//}
		//else
		//{
		//	MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, actor->ServerID, AbilityID*100+9, BUFFID_DEFAULTSKILL, 0, FLOAT(InValue), FLOAT(InValue), 1,TRUE);
		//}
	}
	else
	{
		//todo
	}
}

void AbilityProcessor::PlayClientAbility(INT CasterID, UMMOAbility* ToUseAbility)
{
	AActor* actor = MMOFacade::getSingleton().GetActor(CasterID);
	if (!actor)
	{
		// no caster, no saving cast data to buffer
		return;
	}

	static INT SerialID = 0;
	SerialID++;

	INT AbilityID = ToUseAbility->SpellCommonAttribute.Id;
	INT AbilityLevel = 1;
	INT AbilityRune = 0;

	//UINT key = CombinateID(AbilityID, AbilityLevel, AbilityRune);
	UMMOAbility* abilityData = ToUseAbility;
	if (abilityData == NULL)
	{
		warnf(TEXT("PlayClientAbility : can not find ability data by id(%d), Level(%d), RuneSlot(%d)"), AbilityID, AbilityLevel, AbilityRune);
		return;
	}

	AMMOActor* mmoActor = Cast<AMMOActor>(actor);


	//if (mmoActor && mmoActor->IsA(AMMOPlayer::StaticClass()))
	//{
	//	mmoActor->DrawDebugPoint(TargetPoint, 10, FLinearColor(1.0, 0.f, 0.f), TRUE);
	//}


	AbilityNode *node = new AbilityNode;
	node->SetProcessor(this);
	node->bClientAbility = TRUE;
	node->TargetID = CasterID;
	node->SelfID = CasterID;
	node->TargetPoint = actor->Location;
	node->State = EAS_Spell;
	node->CastCurrentTime = 0.f;
	node->Rate = 1.0f;
	node->Serial = SerialID;
	node->CastList.empty();
	node->AbilityData = abilityData;
	node->TimeScaleByWeaponType = 1.f;
	node->SpeedScale = 1.f;

	FLOAT DataTime = abilityData->SpellCommonAttribute.PrepareTimeMin;

	if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type5) //Charge-up
	{
		DataTime = abilityData->SpellCommonAttribute.PrepareTimeMax; // * node->TimeScale;
	}

	node->OverrideSpellTime = DataTime * node->TimeScaleByWeaponType / node->SpeedScale;

	node->OverrideSpellTime = node->NormalizeTime(node->OverrideSpellTime);

	if (abilityData->SpellCommonAttribute.PrepareType == ES_PT_Type1)	  // Channeling
	{
		DataTime = abilityData->SpellCommonAttribute.Interval;
	}
	else
	{
		DataTime = abilityData->SpellCommonAttribute.HardTime;
	}

	node->OverrideCastTime = DataTime;
	node->OverrideCastTime = node->NormalizeTime(node->OverrideCastTime);

	node->WeaponType = 0;
	if (mmoActor)
	{
		node->WeaponType = mmoActor->GetWeaponType();
	}
	UBOOL bFound = node->AbilityData->GetEffectComponentList(node->WeaponType, &(node->SpellEffectList), &(node->CastEffectList));
	if (!bFound)
	{
		debugf(TEXT("Can not found effect list by type=%d"), node->WeaponType);
	}
	node->GenerateHitPointInfos(node->WeaponType);

#if SHOW_ABILITY_LOG
	debugf(TEXT("OverrideSpellTime=%f Rate=%f"), node->OverrideSpellTime, node->Rate);
#endif

	LONG64 Skey = CombinateID64(AbilityID, CasterID, SerialID);
	AbilityList.insert(make_pair(Skey, node));

}

UINT AbilityProcessor::CombinateID( UINT AbilityID, UINT LevelID,UINT RuneID )
{
	UINT ret = AbilityID;
	ret = ret<<8;
	ret += LevelID;
	ret = ret<<8;
	ret += RuneID;
	return ret;
}


void AMMOEmitter::SelfDestroyed()
{
	//MMOFacade::getSingleton().DestroyObject(ServerID, FALSE);
	//debugf(L"Delete Projectile by self, ServerID=%d", ServerID);
	MMOFacade::getSingleton().RemoveFromCharacterMap(ServerID);
	if (!GIsEditor)
	{
		EffectProcessor::GetSingleton().DeleteClientProjectile(ClientId);
	}
	
	EffectProcessor::GetSingleton().DeleteServerProjectile(ServerID);

	for (INT i = 0; i < ToKillEffectsWhenEnd.Num(); ++i)
	{
		EffectProcessor::GetSingleton().DeleteActiveComponent(ToKillEffectsWhenEnd(i));
	}
}

void AMMOEmitter::BuildNavMeshObstacle(class UMissile* InMissile)
{
	if (!InMissile)
	{
		return;
	}

	DynamicObstacle = Cast<ADynamicNavMeshObstacle>(GWorld->SpawnActor(ADynamicNavMeshObstacle::StaticClass()));
	check(DynamicObstacle);
	DynamicObstacle->UnRegisterObstacle();
	FVector NewLocation = Location - FVector(0, 0, 100);
	DynamicObstacle->SetLocation(NewLocation);
	DynamicObstacle->SetRotation(Rotation);
	DynamicObstacle->eventSetAsCycle(100, 8);
	DynamicObstacle->RegisterObstacle();
}

void AMMOEmitter::SetupCollision(class UMissile* InMissileData)
{
	if (!InMissileData)
	{
		return;
	}

	this->SetTickGroup(TG_PostAsyncWork);

	if (CollisionComponent)
	{
		DetachComponent(CollisionComponent);
	}


	UStaticMesh* CollisionMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("EngineMeshes.Cube"), NULL, LOAD_None, NULL));
	if (!CollisionMesh)
	{
		debugf(TEXT("Can not load engine default mesh EngineMeshes.Cube, so can not setup collision component for the missile"));
		return;
	}



	UStaticMeshComponent* NewCollisionComponent = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), this);
	if (!NewCollisionComponent)
	{
		return;
	}

	NewCollisionComponent->SetStaticMesh(CollisionMesh);
	CollisionComponent = NewCollisionComponent;
	CollisionComponent->CollideActors = TRUE;
	CollisionComponent->AlwaysCheckCollision = TRUE;
	CollisionComponent->BlockActors = TRUE;
	CollisionComponent->BlockZeroExtent = TRUE;

	FVector NewScale(1.f, 1.f, 2.f);

	//if (InMissileData->ObstacleShape == E_oos_Line)
	//{
	//	NewScale.X = 25.f / 256.f;
	//	NewScale.Y = InMissileData->ObstacleParam / 256.f;
	//}
	//else if (InMissileData->ObstacleShape == E_oos_Square)
	//{
	//	NewScale.X = InMissileData->ObstacleParam / 256.f;
	//	NewScale.Y = InMissileData->ObstacleParam / 256.f;
	//}

	NewCollisionComponent->Scale3D = NewScale;
	NewCollisionComponent->SetHiddenGame(TRUE);
	AttachComponent(NewCollisionComponent);


	bCollideActors = TRUE;
	bBlockActors = TRUE;
	SetCollision(TRUE, TRUE, FALSE);
	SetCollisionType(COLLIDE_BlockAll);
}

void AMMOEmitter::ShowMissileCollision()
{
	if (MMOFacade::getSingleton().GetMainController()->bShowMissileCollision && MissileData)
	{
		FMatrix OriginMat = LocalToWorld();
		INT CheckDelta = 400;
		FLinearColor Color(1.0, 0, 0, 1);
		// offset matrix
		FRotator OffsetRot(0, 0, 0);
		FVector OffsetVector;
		FVector Pos1, Pos2;

		//FlushPersistentDebugLines();


		if (MissileData->RangeType == E_sprt_Single)	// Target object
		{

		}
		else if (MissileData->RangeType == E_sprt_Circle)	// cycle
		{
			FLOAT Radius1 = MissileData->RangeParam1;
			FLOAT Radius2 = MissileData->RangeParam2;
			for (INT i = 0; i < 65536; i += CheckDelta)
			{
				Pos1.X = Radius1 * GMath.CosTab(i);
				Pos1.Y = Radius1 * GMath.SinTab(i);
				Pos1.Z = 0;
				Pos2.X = Radius2 * GMath.CosTab(i);
				Pos2.Y = Radius2 * GMath.SinTab(i);
				Pos2.Z = 0;
				Pos1 = OriginMat.TransformFVector(Pos1);
				Pos2 = OriginMat.TransformFVector(Pos2);
				DrawDebugLine(Pos1, Pos2, 255, 0, 0, TRUE);
			}
		}
		//else if (MissileData->RangeType == ESR_Type3)	// cycle1 - cycle2
		//{
		//	FLOAT Radius1 = AbilityObject->SpellCommonAttribute.RangeParam1;
		//	FLOAT Radius2 = AbilityObject->SpellCommonAttribute.RangeParam2;
		//	for (INT i = 0; i < 65536; i += CheckDelta)
		//	{
		//		Pos1.X = Radius1 * GMath.CosTab(i);
		//		Pos1.Y = Radius1 * GMath.SinTab(i);
		//		Pos1.Z = 0;

		//		Pos2.X = Radius2 * GMath.CosTab(i);
		//		Pos2.Y = Radius2 * GMath.SinTab(i);
		//		Pos2.Z = 0;

		//		Pos1 = OriginMat.TransformFVector(Pos1);
		//		Pos2 = OriginMat.TransformFVector(Pos2);

		//		Lines.AddItem(ULineBatchComponent::FLine(Pos1, Pos2, Color, LineBatch->DefaultLifeTime, 1, SDPG_World));
		//	}
		//}
		else if (MissileData->RangeType == E_sprt_Sector)
		{
			FLOAT Radius = MissileData->RangeParam1;
			INT Yaw1 =  - MissileData->RangeParam3 * 65536 / 360 / 2;
			INT Yaw2 =  + MissileData->RangeParam3 * 65536 / 360 / 2;
			for (INT i = Yaw1; i < Yaw2; i += CheckDelta)
			{
				Pos1.X = Radius * GMath.CosTab(i);
				Pos1.Y = Radius * GMath.SinTab(i);
				Pos1.Z = 0;

				Pos2.X = 0;
				Pos2.Y = 0;
				Pos2.Z = 0;

				Pos1 = OriginMat.TransformFVector(Pos1);
				Pos2 = OriginMat.TransformFVector(Pos2);

				DrawDebugLine(Pos1, Pos2, 255, 0, 0, TRUE);
			}
		}
		else if (MissileData->RangeType == E_sprt_Arc)
		{
			FLOAT Radius1 = MissileData->RangeParam1;
			FLOAT Radius2 = MissileData->RangeParam2;
			INT Yaw1 = - MissileData->RangeParam3 * 65536 / 360 / 2;
			INT Yaw2 = MissileData->RangeParam3 * 65536 / 360 / 2;
			for (INT i = Yaw1; i < Yaw2; i += CheckDelta)
			{
				Pos1.X = Radius1 * GMath.CosTab(i);
				Pos1.Y = Radius1 * GMath.SinTab(i);
				Pos1.Z = 0;

				Pos2.X = Radius2 * GMath.CosTab(i);
				Pos2.Y = Radius2 * GMath.SinTab(i);
				Pos2.Z = 0;

				Pos1 = OriginMat.TransformFVector(Pos1);
				Pos2 = OriginMat.TransformFVector(Pos2);

				DrawDebugLine(Pos1, Pos2, 255, 0, 0, TRUE);
			}
		}
		else if (MissileData->RangeType == E_sprt_Rectangle)
		{
			FLOAT Radius1 = MissileData->RangeParam1;
			FLOAT Radius2 = MissileData->RangeParam2;
			for (FLOAT i = -Radius2 / 2.0; i < Radius2 / 2.0; i += 10)
			{
				Pos1.X = 0;
				Pos1.Y = i;
				Pos1.Z = 0;

				Pos2.X = Radius1;
				Pos2.Y = i;
				Pos2.Z = 0;

				Pos1 = OriginMat.TransformFVector(Pos1);
				Pos2 = OriginMat.TransformFVector(Pos2);

				DrawDebugLine(Pos1, Pos2, 255, 0, 0, TRUE);
			}
		}
	}

}

void AMMOEmitter::BeginPlay(class AActor* BaseActor,class UMMOAbility* InAbilityData,class AActor* InTarget,FVector InTargetPoint,FLOAT InRate,INT SerialId)
{
	bRun = true;
	Creator = BaseActor;
	Target = InTarget;
	TargetPoint = InTargetPoint;
	Rate = InRate;

	INT CreatorId = ServerID;
	if (Creator && Creator->IsA(AMMOActor::StaticClass()))
	{
		AMMOActor* mmoActor = (AMMOActor*)Creator;
		if (mmoActor)
		{
			CreatorId = mmoActor->ServerID;
		}
	}

	if (MyPointLightComponent)
	{
		AttachComponent(MyPointLightComponent);
		OriginalBrightness = MyPointLightComponent->Brightness;

		if (LightFadeUpTime <= 0.00001)
		{
			LightFadeUpTime = 0.01;
		}
		if (LightFadeOutTime < 0.00001)
		{
			LightFadeOutTime = 0.01;
		}

		if (LightLifeSpan < LightFadeOutTime + LightFadeUpTime + 0.01)
		{
			LightLifeSpan = LightFadeOutTime + LightFadeUpTime + 0.01;
		}

		if (OriginalBrightness < 0)
		{
			OriginalBrightness = 0.1;
		}

		FadeSpeed = OriginalBrightness / LightFadeUpTime;
	}

	if (MissileData)
	{
		for (INT i = 0; i < MissileData->EffectComponents.Num(); ++i)
		{
			UAbilityComponent* TemplateComponent = MissileData->EffectComponents(i);
			if (!TemplateComponent)
			{
				continue;
			}

			//if (TemplateComponent->StopType != ACBT_StopWhenConstDurationPass)
			//{
			//	InComponent->StopType = ACBT_StopWhenConstDurationPass;
			//	InComponent->Duration = 10.f;
			//}

			BaseComponent* NewNode = AbilityProcessor::CreateDynamicComponent(TemplateComponent, FALSE, CreatorId, ServerID, 0, Location, 1.0);
			if (NewNode)
			{
				EffectProcessor::GetSingleton().AddActiveComponent(NewNode);
				NewNode->OnAction(TRUE);
				if (TemplateComponent->StopType != ACBT_StopWhenConstDurationPass)
				{
					ToKillEffectsWhenEnd.AddItem(NewNode->GetId());
				}
			}
		}
	}

}

void AMMOEmitter::Kill()
{
	bToAutoDestroy = TRUE;
	bDestroyOnSystemFinish = TRUE;
	FLOAT MaxLife = ParticleSystemComponent->GetMaxLifespanWithoutLoop();
	ParticleSystemComponent->SetStopSpawning(-1, TRUE);
	if (MaxLife <= 0.01)
	{
		MaxLife = 0.01;
	}

	if (MaxLife > 1.0)
	{
		MaxLife = 1.0;
	}

	LifeSpan = MaxLife;

	if (MyPointLightComponent)
	{
		if (LightFadeOutTime > MaxLife)
		{
			LightFadeOutTime = MaxLife;
		}

		if (LightFadeOutTime < 0.01)
		{
			LightFadeOutTime = 0.01f;
		}

		FadeSpeed = -OriginalBrightness / LightFadeOutTime;
	}

	if (BeamTargetActor)
	{
		BeamTargetActor->LifeSpan = LifeSpan;
	}

	if (DynamicObstacle)
	{
		DynamicObstacle->UnRegisterObstacle();
		if(!DynamicObstacle->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			GWorld->DestroyActor(DynamicObstacle);
		}

	}
}

void AMMOEmitter::ResetInstanceLocations( FVector loc )
{
	if( ParticleSystemComponent )
	{
		for (INT InstanceIndex = 0; 
			InstanceIndex < ParticleSystemComponent->EmitterInstances.Num(); 
			InstanceIndex++)
		{
			FParticleEmitterInstance* EmitterInstance = ParticleSystemComponent->EmitterInstances(InstanceIndex);
			if (EmitterInstance != NULL)
			{
				EmitterInstance->Location = loc;
				EmitterInstance->OldLocation = loc;
			}
		}
	}
}

void AMMOEmitter::TickSpecial( FLOAT DeltaSeconds )
{
	SCOPE_CYCLE_COUNTER(STAT_AbilityEmitterTickTime);

	CurrentTime += DeltaSeconds;

	EffectProcessor::GetSingleton().AddCurrentEmitterNumber();


	// update light
	if (MyPointLightComponent)
	{
		FLOAT ori = MyPointLightComponent->Brightness;
		if (FadeSpeed > 0)
		{
			if (CurrentTime < LightFadeUpTime)
			{
				ori += FadeSpeed * DeltaSeconds;
				if (ori <= OriginalBrightness)
				{
					MyPointLightComponent->SetLightProperties(ori, MyPointLightComponent->LightColor, MyPointLightComponent->Function);
				}
			}
			else
			{
				if (LightLifeSpan >= 0.0001)
				{
					if (CurrentTime >= LightLifeSpan - LightFadeOutTime)
					{
						FadeSpeed = -OriginalBrightness / LightFadeOutTime;
					}
				}
			}
		}
		else
		{
			ori += FadeSpeed*DeltaSeconds;
			if (ori >= 0)
			{
				MyPointLightComponent->SetLightProperties(ori, MyPointLightComponent->LightColor, MyPointLightComponent->Function);
			}
			else
			{
				MyPointLightComponent->SetEnabled(FALSE);
				this->DetachComponent(MyPointLightComponent);
				MyPointLightComponent = NULL;
			}
		}
	}

	if (bToAutoDestroy || bSonProjectile)
	{
		return;
	}

	if (StopType == ACBT_StopWhenConstDurationPass)
	{
		if (CurrentTime > Duration)
		{
			// if is a missle
			if (MissileData)
			{
				if (bHidden)
				{
					Kill();
					return;
				}
				else
				{
					Duration = DelayToKill + CurrentTime;
					SetHidden(TRUE);
					return;
				}
			}
			else
			{
				Kill();
				return;
			}
		}
	}

	UBOOL bAttachToSocket =  FALSE;
	if (EmitterData)
	{
		bAttachToSocket = EmitterData->bAttach;
	}


	// show missile collision
	if (MissileData && MMOFacade::getSingleton().GetMainController()->bShowMissileCollision)
	{
		ShowMissileCollision();
	}
		


	if (OverwriteEmitterType == EMIT_Normal)
	{
		if (bAttachToSocket)
		{
			if (Creator == NULL || Creator->IsPendingKill())
			{
				Kill();
				return;
			}

			if (Creator->IsA(AMMOEmitter::StaticClass()))
			{
				AMMOEmitter* EmitterCreator = (AMMOEmitter*)Creator;
				if (EmitterCreator->bToAutoDestroy)
				{
					Kill();
					return;
				}
			}

			FMatrix NewMat = EmitterData->GetSocketMatrix(Creator, TargetPoint, FALSE, OverwriteSocketName);

			if (EmitterData->bUseBaseRotation == FALSE)
			{
				NewMat = FScaleRotationTranslationMatrix(NewMat.GetScaleVector(), EmitterData->RelativeRotation, NewMat.GetScaleVector());
			}
			
			SetLocation(NewMat.GetOrigin());
			SetRotation(NewMat.Rotator());
			

			//debugf(TEXT("TICK %s Location:%f, %f, %f"), *(ParticleSystemComponent->Template->GetPathName()), Location.X, Location.Y, Location.Z);
		}
	}
	else if (OverwriteEmitterType == EMIT_FlyTo)
	{
		FVector NewPoint;
		FRotator NewRotation;
		// if it is a missile
		if (EmitterData == FALSE)
		{
			check(MissileData);

			if (MissileData->MoveType == E_mmt_Static)
			{
				return;
			}

			NewPoint = Location + DeltaSeconds * Speed * Rotation.Vector().SafeNormal();
			NewPoint.Z = SpawnMatrix.GetOrigin().Z;

			if (MissileData->MoveType == E_mmt_AccordingToTerrain && DistanceToLand > 0.0)
			{
				FLOAT NewZ = MMOFacade::getSingleton().GetMainActor()->GetValidZValue(FVector(NewPoint.X, NewPoint.Y, NewPoint.Z - DistanceToLand), 250, -600);
				if (NewZ - NewPoint.Z + DistanceToLand < 1)
				{
					NewZ = Location.Z;
				}
				else
				{
					NewZ += DistanceToLand;
				}

				NewPoint.Z = NewZ;
			}

			NewRotation = (NewPoint - Location).Rotation();

			FLOAT dot = (TargetPoint - Location).Dot2d(Rotation.Vector());
			if (dot <= 0.f)
			{
				NewPoint = Location;
			}
		}
		else	// if it is a emitter
		{
			FLOAT ct = CurrentTime / Duration;
			FVector DstPoint;
			FVector SpawnPoint = SpawnMatrix.GetOrigin();
			if (OverwriteTargetType == EPTT_Actor)
			{
				if (Target)
				{
					FMatrix DstMat = EmitterData->GetSocketMatrix(Target, TargetPoint, TRUE);
					DstPoint = DstMat.GetOrigin();
				}
				else
				{
					DstPoint = Location;
				}
			}
			else if (OverwriteTargetType == EPTT_TargetPoint)
			{
				DstPoint = TargetPoint;
			}
			else if (OverwriteTargetType == EPTT_UseCasterDirection)
			{
				DstPoint = SpawnPoint + SpawnMatrix.Rotator().Vector().SafeNormal() * MaxDistance;
			}

			if (EmitterData->TrackLineType == ETLT_Line)
			{
				NewPoint = (DstPoint - SpawnPoint) * ct + SpawnPoint;
				NewRotation = (NewPoint - Location).Rotation();
			}
			else if (EmitterData->TrackLineType == ETLT_Parabola)
			{
				NewPoint = (DstPoint - SpawnPoint) * ct + SpawnPoint;
				// calculate height
				//y = -k*(x - t*l/2/c)*(x - t*l/2/c) + h; (x < t/c*l)

				FLOAT t = EmitterData->Duration;
				FLOAT c = EmitterData->LoopCount;
				FLOAT h = EmitterData->MaxHeight;
				FLOAT l = EmitterData->TimeToFinishFixRotation;
				FLOAT k = (t * l) / 2.0f / c;
				k = h / k / k;

				FLOAT CycleTime = t / c;
				INT PassedCount = CurrentTime / CycleTime;
				FLOAT x = CurrentTime - PassedCount * CycleTime;
				FLOAT z = 0.f;
				if (x <= t * l / c)
				{
					z = x - t * l / 2.f / c;
					z = -k * z * z + h;
				}
				else
				{
					z = 0.f;
				}
				NewPoint.Z += z;

				NewRotation = (NewPoint - Location).Rotation();
			}
			else if (EmitterData->TrackLineType == ETLT_Bezier)
			{
				FVector P0 = FVector(0, 0, 0);
				FVector P1 = BaseActorMatrix.InverseTransformFVector(DstPoint);
				FVector T0 = OverwriteBezierControl.v1;
				FVector T1 = P1 + OverwriteBezierControl.v2;


				NewPoint = CubicInterp<FVector, FLOAT>(P0, T0, P1, T1, ct);
				NewPoint = BaseActorMatrix.TransformFVector(NewPoint);
				NewRotation = (NewPoint - Location).Rotation();
			}
		}

		SetLocation(NewPoint);
		SetRotation(NewRotation);

	}
	else if (OverwriteEmitterType == EMIT_ConnectTo)
	{
		if (Creator == NULL || Creator->IsPendingKill())
		{
			Kill();
			return;
		}

		if (EmitterData->bAttach)
		{
			FMatrix NewMat = EmitterData->GetSocketMatrix(Creator, TargetPoint, FALSE);
			SetLocation(NewMat.GetOrigin());
		}

		FVector Dst(0, 0, 0);
		FVector HitNormal;

		if (EmitterData->ProjectileTargetType == EPTT_Actor)
		{
			if (Target)
			{
				FMatrix DstMat = EmitterData->GetSocketMatrix(Target, TargetPoint, TRUE);
				Dst = DstMat.GetOrigin();
				HitNormal = (Location - Dst).SafeNormal();
			}

		}
		else if (EmitterData->ProjectileTargetType == EPTT_TargetPoint)
		{
			Dst = TargetPoint;
			HitNormal = (Location - Dst).SafeNormal();
		}
		else if (EmitterData->ProjectileTargetType == EPTT_UseCasterDirection || EmitterData->ProjectileTargetType == EPTT_UseCasterDirectionAndCollision)
		{
			//if (bAttachToSocket)
			//{
			//	FMatrix NewMat = EmitterData->GetSocketMatrix(Creator, TargetPoint, FALSE);
			//	SetLocation(NewMat.GetOrigin());
			//}


			Dst = Creator->Rotation.Vector();
			//SetRotation(Creator->Rotation);
			Dst = Dst.SafeNormal() * MaxDistance + Location;
			HitNormal = (Location - Dst).SafeNormal();

			if (EmitterData->bHaveCollisionCheck)
			{
				AMMOActor* mmoCreator = Cast<AMMOActor>(Creator);
				DWORD TraceFlags = TRACE_World | TRACE_Visible | TRACE_Pawns | TRACE_Others;
				AActor* TraceActor = this;

				FVector StartPoint = Location;
				FVector EndPoint = Dst;

				if (mmoCreator)
				{
					StartPoint.Z -= mmoCreator->GetCollisionHeight() * 0.5f;
					EndPoint.Z -= mmoCreator->GetCollisionHeight() * 0.5f;
				}

				//AMMOActor* mmoActor = Cast<AMMOActor>(Creator);
				//if (mmoActor && mmoActor->CylinderComponent)
				//{
				//	//Dst.Z = Dst.Z - mmoActor->CylinderComponent->CollisionHeight;
				//	EndPoint = Dst; // - FVector(0, 0, mmoActor->CylinderComponent->CollisionHeight);
				//	StartPoint.Z -= mmoActor->CylinderComponent->CollisionHeight * 0.5;
				//}

				HitNormal = (Location - Dst).SafeNormal();

				FMemMark Mark(GMainThreadMemStack);
				FCheckResult* Hit = GWorld->MultiLineCheck( GMainThreadMemStack, EndPoint, StartPoint, EmitterData->TraceExtend, TraceFlags, Creator);
				while(Hit)
				{
					UBOOL bFind = FALSE;
					if (Hit->Actor == Creator)
					{
						bFind = FALSE;
					}
					else
					{
						AMMOActor* HitActor = Cast<AMMOActor>(Hit->Actor);
						if (HitActor)
						{
							if (HitActor->bDead)
							{
								bFind = FALSE;
							}
							else
							{
								if (mmoCreator /*&& mmoCreator->IsMainPlayer()*/)
								{
									bFind = MMOFacade::getSingleton().IsAttackable(mmoCreator,HitActor);
								}
							}
						}
						else
						{
							bFind = TRUE;
						}
					}

					if (bFind)
					{
						Dst = Hit->Location;
						if (mmoCreator)
						{
							Dst.Z += mmoCreator->GetCollisionHeight() * 0.5f;
						}
						HitNormal = Hit->Normal;
						break;
					}
					else
					{
						Hit = Hit->GetNext();
					}
				}
				Mark.Pop();
			}
		}

		SetRotation((Dst - Location).Rotation());

		if (this->ParticleSystemComponent)
		{
			this->ParticleSystemComponent->SetVectorParameter(FName(L"LinkBeamEnd"), Dst);
		}

		if(this->BeamTargetActor)
		{
			this->BeamTargetActor->SetLocation(Dst);
			this->BeamTargetActor->SetRotation(HitNormal.Rotation());
		}

	}
	else if (OverwriteEmitterType == EMIT_Rotate)
	{
		FMatrix NewMat;
		if (EmitterData->bAttach)
		{
			NewMat = EmitterData->GetSocketMatrix(Creator, TargetPoint, FALSE);
		}

		FVector Org = NewMat.GetOrigin();

		FMatrix BMat = LocalToWorld();
		FMatrix BToAMat = BMat * NewMat.Inverse();
		FRotationMatrix frm(FRotator(EmitterData->RotateVelocity * DeltaSeconds));
		BMat = BToAMat * frm;
		NewMat = BMat * NewMat;

		FRotator delta = NewMat.Rotator();
		FVector dir = NewMat.GetOrigin();
		dir = Org + (dir - Org).SafeNormal() * EmitterData->RotateRadius;
		if (Creator == NULL || Creator->IsPendingKill())
		{
			Kill();
		}
		SetLocation(dir);
		SetRotation(delta);
	}
}

void EffectProcessor::Tick( FLOAT DeltaTime )
{
	CTRL_TICK("MOEffectProcessor::Tick");

	STLIntVector ToDeleteComponentIds;
	STLInt2BaseComponentMap::iterator pos = ActiveComponents.begin();
	for (; pos != ActiveComponents.end(); ++pos)
	{
		if (pos->second->bStopped)
		{
			delete pos->second;
			pos->second = NULL;
			ToDeleteComponentIds.push_back(pos->first);
		}
		else
		{
			pos->second->Tick(DeltaTime);
		}
	}

	for (size_t i = 0; i < ToDeleteComponentIds.size(); ++i)
	{
		ActiveComponents.erase(ToDeleteComponentIds[i]);
	}

#if SHOW_ABILITY_MEMORY
	debugf(TEXT("### Active Effect Component Number Is : %d"), ActiveComponents.size());

#endif
}

INT EffectProcessor::GetProjectileId()
{
	static INT Id = 0;
	return ++Id;
}

INT EffectProcessor::CreateEffectId()
{
	static INT Id = 0;
	++Id;
	if (Id == 0)
	{
		++Id;
	}
	return Id;
}

EffectProcessor::EffectProcessor()
{
	MaxEmitterNumber = DEFAULT_EMITTER_MAX_NUMBER;
	CurrentEmitterNumber = 0;
}

void EffectProcessor::SetMaxEmitterNumber(INT InMax)
{
	check(InMax >= 0);
	MaxEmitterNumber = InMax;
}

void EffectProcessor::ClearCurrentEmitterNumber()
{
	CurrentEmitterNumber = 0;

}

void EffectProcessor::AddCurrentEmitterNumber()
{
	CurrentEmitterNumber++;
}

UBOOL EffectProcessor::CanSpawnEmitter()
{
	//if (GIsEditor)
	//{
	//	return TRUE;
	//}

	//FLOAT Rate = GSystemSettings.ParticleLevel / 5;
	//return (CurrentEmitterNumber < MaxEmitterNumber * Rate);

	return (CurrentEmitterNumber < MaxEmitterNumber);
}

void EffectProcessor::AddActiveComponent( BaseComponent* InBaseComponent )
{
	if (!InBaseComponent || InBaseComponent->ComponentId == 0)
	{
		return;
	}

	STLInt2BaseComponentMap::iterator pos = ActiveComponents.find(InBaseComponent->ComponentId);
	if (pos != ActiveComponents.end())
	{
		warnf(L"add the same BaseComponent!!!");
		return;
	}
	else
	{
		ActiveComponents.insert(make_pair(InBaseComponent->ComponentId, InBaseComponent));
	}
}

BaseComponent* EffectProcessor::GetActiveComponent( INT Id )
{
	STLInt2BaseComponentMap::iterator pos = ActiveComponents.find(Id);
	if (pos == ActiveComponents.end())
	{
		return NULL;
	}
	else
	{
		return pos->second;
	}
}

void EffectProcessor::DeleteActiveComponent( INT Id )
{
	STLInt2BaseComponentMap::iterator pos = ActiveComponents.find(Id);
	if (pos != ActiveComponents.end())
	{
		if (pos->second->bStopped == FALSE)
		{
			pos->second->OnAction(FALSE);
		}
		delete pos->second;
		pos->second = NULL;
		ActiveComponents.erase(pos);
	}
}

void EffectProcessor::DeleteAllComponent(UBOOL bButMainPlayer)
{
	std::vector<INT> AllComponentList;
	for (STLInt2BaseComponentMap::iterator i = ActiveComponents.begin(); i != ActiveComponents.end(); ++i)
	{
		if (bButMainPlayer && (i->second->GetBaseCharacter() == MMOFacade::getSingleton().GetMainActor() || i->second->IsAddedToRoot()))
		{
			continue;
		}

		AllComponentList.push_back(i->first);
	}

	for (size_t i = 0; i < AllComponentList.size(); ++i)
	{
		DeleteActiveComponent(AllComponentList[i]);
	}
	

}


void EffectProcessor::ClearAllComponentByBaseActorId(INT ObjId)
{
	std::vector<INT> AllComponentList;
	for (STLInt2BaseComponentMap::iterator i = ActiveComponents.begin(); i != ActiveComponents.end(); ++i)
	{
		if (i->second->BaseActorId == ObjId)
		{
			AllComponentList.push_back(i->first);
		}
	}

	for (size_t i = 0; i < AllComponentList.size(); ++i)
	{
		DeleteActiveComponent(AllComponentList[i]);
	}

}

void EffectProcessor::ClearAllComponentByBaseActorIdWhenRevive(INT ObjId)
{
	std::vector<INT> AllComponentList;
	for (STLInt2BaseComponentMap::iterator i = ActiveComponents.begin(); i != ActiveComponents.end(); ++i)
	{
		if (i->second->BaseActorId == ObjId && i->second->bClearWhenOwerRevive)
		{
			AllComponentList.push_back(i->first);
		}
	}

	for (size_t i = 0; i < AllComponentList.size(); ++i)
	{
		DeleteActiveComponent(AllComponentList[i]);
	}

}

void EffectProcessor::AddServerProjectile( INT ObjId, AMMOEmitter* ProActor )
{
	if (ProActor)
	{
		ProActor->ClientId = 0;
		ProActor->ServerID = ObjId;
		ServerProjectiles.insert(make_pair(ObjId, ProActor->GetIndex()));
	}
}

void EffectProcessor::DeleteServerProjectile( INT ObjId )
{
	STLInt2IntMap::iterator pos = ServerProjectiles.find(ObjId);
	if (pos != ServerProjectiles.end())
	{
		AMMOEmitter* ProActor = Cast<AMMOEmitter>(UObject::GetIndexedObject(pos->second));
		if (ProActor)
		{
			ProActor->Kill();
		}
		ServerProjectiles.erase(ObjId);
	}
}

INT EffectProcessor::AddClientProjectile( AMMOEmitter* ProActor )
{
	INT Ret = 0;
	if (ProActor)
	{
		ProActor->ServerID = 0;
		Ret = ProActor->ClientId = GetProjectileId();
		ClientProjectiles.insert(make_pair(ProActor->ClientId, ProActor->GetIndex()));
	}
	return Ret;
}

void EffectProcessor::DeleteClientProjectile( INT ClientId )
{
	STLInt2IntMap::iterator pos = ClientProjectiles.find(ClientId);
	if (pos != ClientProjectiles.end())
	{
		AMMOEmitter* ProActor = Cast<AMMOEmitter>(UObject::GetIndexedObject(pos->second));
		if (ProActor)
		{
			if (GIsEditor)
			{
				GWorld->DestroyActor(ProActor);
			}
			else
			{
				ProActor->Kill();
			}
			
		}
		ClientProjectiles.erase(ClientId);
	}
}

void EffectProcessor::SetClientProjectileVisible(INT InBaseId, UBOOL bVisible)
{
	for (STLInt2IntMap::iterator pos = ClientProjectiles.begin(); pos != ClientProjectiles.end(); ++pos)
	{
		AMMOEmitter* ProActor = Cast<AMMOEmitter>(UObject::GetIndexedObject(pos->second));
		if (ProActor && ProActor->bDeleteMe == FALSE && ProActor->Creator)
		{
			AMMOActor* mmoActor = Cast<AMMOActor>(ProActor->Creator);
			if (mmoActor && mmoActor->ServerID == InBaseId)
			{
				ProActor->SetHidden(!bVisible);
			}
		}
	}

}
