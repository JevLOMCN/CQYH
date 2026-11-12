#include "stdafx.h"
#include "AbilityProcessor.h"
#include "MMOFacade.h"
#include "MMOPerformance.h"

IMPLEMENT_CLASS(UAbilityAnimationComponent)
IMPLEMENT_CLASS(UAbilityActionProcComponent)
IMPLEMENT_CLASS(UAbilityComp_MaterialEffect)
IMPLEMENT_CLASS(UAbilityEmitterComponent)
IMPLEMENT_CLASS(UAbilitySoundCueComponent)
IMPLEMENT_CLASS(UAbilityTimeFactorComponent)
IMPLEMENT_CLASS(UAbilityDecalComponent)
IMPLEMENT_CLASS(UCameraShakeComponent)
IMPLEMENT_CLASS(UAbilityHitPointComponent)
IMPLEMENT_CLASS(UAbilityHitEffectComponent)
IMPLEMENT_CLASS(AMMOEmitter)
IMPLEMENT_CLASS(UAbilityEnabledCollsion)
IMPLEMENT_CLASS(UAbilityTrailComponent)
IMPLEMENT_CLASS(UAnimNotify_AbilityTrails)
IMPLEMENT_CLASS(UAbilityMoveControlComponent)
IMPLEMENT_CLASS(UGhostSpawner)
IMPLEMENT_CLASS(UAbilitySkeletalMeshComponent)
IMPLEMENT_CLASS(AAbilitySkeletalMeshActor)
IMPLEMENT_CLASS(UAbilityStaticMeshComponent)
IMPLEMENT_CLASS(AAbilityStaticMeshActor)
IMPLEMENT_CLASS(UAbilityThrowComponent)
IMPLEMENT_CLASS(UAbilityPostProcessComponent)
IMPLEMENT_CLASS(UAbilityActionsComponent)
IMPLEMENT_CLASS(UAbilityVisibleComponent)
IMPLEMENT_CLASS(UAbilityPhysicSimulateComponent)
IMPLEMENT_CLASS(UAbilityScaleComponent)
IMPLEMENT_CLASS(UAbilityMirrorComponent)
IMPLEMENT_CLASS(UAbilityMeshChangeComponent)
IMPLEMENT_CLASS(UPositionTransformComponent)
IMPLEMENT_CLASS(UAbilityBackgroundMusicComponent)
IMPLEMENT_CLASS(AAbilityWindSourceActor)

extern FSystemSettings GSystemSettings; // zhangrui@snda

void UMMOAbility::GetDynamicListValues(const DynamicListInfo& ListInfo, TArray<FString>& Values)
{
	//song@snda.com
	if(ListInfo.ListName == TEXT("AbilityModifier"))
	{
		TArray<FString> ModifierArray;
		if ( GConfig->GetArray( TEXT("AbilityModifier"), TEXT("AbilityModifier"), ModifierArray, GGameIni ) > 0 )
		{
			for (INT i = 0; i < ModifierArray.Num(); ++i)
			{
				Values.AddItem(ModifierArray(i));
			}		
		}
	}
	else
	{
		Super::GetDynamicListValues(ListInfo, Values);
	}
	//song@snda.com
}

void UAnimNotify_AbilityTrails::Notify(class UAnimNodeSequence* NodeSeq)
{
	if (bEnable)
	{
		UAnimNotify_Trails::Notify(NodeSeq);
	}
}
void UAnimNotify_AbilityTrails::NotifyTick(class UAnimNodeSequence* NodeSeq, FLOAT AnimCurrentTime, FLOAT AnimTimeStep, FLOAT InTotalDuration)
{
	if (bEnable)
	{
		UAnimNotify_Trails::NotifyTick(NodeSeq, AnimCurrentTime, AnimTimeStep, InTotalDuration);
	}

}
void UAnimNotify_AbilityTrails::NotifyEnd(class UAnimNodeSequence* NodeSeq, FLOAT AnimCurrentTime)
{
	if (bEnable)
	{
		UAnimNotify_Trails::NotifyEnd(NodeSeq, AnimCurrentTime);
	}
}

void UAnimNotify_AbilityTrails::RebuildTrailData(class UAnimNodeSequence* Seq)
{
	if (!Seq)
	{
		return;
	}
	UObject* obj = Seq->GetOuter();
	if (!obj)
	{
		return;
	}

	if (obj == AnimSetData)
	{
		// do not have to rebuild
		return;
	}
	else
	{
		StoreAnimationData(Seq);
	}

}


//------------------------------------------------------------------------
// UMMOAbility
//------------------------------------------------------------------------
void UMMOAbility::Tick(FLOAT DeltaTime)
{
}
//----------------------------------------------------------------------___--
void UMMOAbility::UpdateCd(FLOAT DeltaTime)
{
	//CommonAttribute.LeftTime -= DeltaTime;
	//if(CommonAttribute.LeftTime <= 0.f)
	//{
	//	CommonAttribute.LeftTime = -0.00001f;
	//}
}


FString UAbilityComponent::GetResName()
{
	return TitleString;

}
FLOAT UAbilityComponent::GetResLength(USkeletalMeshComponent* InSM)
{
	if (StopType == ACBT_StopWhenConstDurationPass)
	{
		return Duration;
	}
	
	return 0.f;
}

void UAbilityComponent::SetResLength(USkeletalMeshComponent* InSM, FLOAT InTime)
{
	if (StopType == ACBT_StopWhenConstDurationPass)
	{
		Duration = InTime;
	}
}

BaseComponent* UAbilityComponent::CreateCplusplusNode()
{
	return 0;
}

UBOOL UAbilityComponent::IsEffectValid()
{
	return TRUE;
}


FMatrix UAbilityComponent::GetSpawnMatrix(USkeletalMeshComponent* InBaseComponent, USkeletalMeshComponent* InWeaponComponent1, USkeletalMeshComponent* InWeaponComponent2, FVector InTargetPoint, FName InSocketName, BYTE InSourceType, FVector InRelativeLocation, FRotator InRelativeRotation, INT bLocal)
{
	FMatrix Ret = FMatrix::Identity;
	UBOOL bFound = FALSE;

	if (!InBaseComponent)
	{
		return Ret;
	}

	if (InSourceType == EST_TargetPoint)
	{
		Ret = FTranslationMatrix(InTargetPoint);
		bFound = TRUE;
	}
	else if (InSourceType == EST_BaseActor || InSourceType == EST_TargetActor)
	{
		bFound = InBaseComponent->GetSocketWorldTransformMatrix(InSocketName, Ret, bLocal);
	}
	else if (InSourceType == EST_BaseWeapon1)
	{
		if (InWeaponComponent1)
		{
			bFound = InWeaponComponent1->GetSocketWorldTransformMatrix(InSocketName, Ret, bLocal);
		}
	}
	else if (InSourceType == EST_BaseWeapon2)
	{
		if (InWeaponComponent2)
		{
			bFound = InWeaponComponent2->GetSocketWorldTransformMatrix(InSocketName, Ret, bLocal);
		}
	}

	if (!bFound)
	{
		Ret = InBaseComponent->CachedParentToWorld;
	}

	FRotationTranslationMatrix frtm(InRelativeRotation, InRelativeLocation);
	Ret = frtm * Ret;
	return Ret;
}


FString UAbilityAnimationComponent::GetResName()
{
	if (Anims.Num() == 0)
	{
		return L"NoData";
	}
	else
	{
		return Anims(0).ToString();
	}
}

FLOAT UAbilityAnimationComponent::GetResLength(USkeletalMeshComponent* InSM)
{
	if (StopType == ACBT_StopWhenConstDurationPass)
	{
		if (bLoop)
		{
			return LoopTime;
		}
		else
		{
			if (InSM)
			{
				UAnimSequence* Seq = NULL;
				for (INT i = 0; i < Anims.Num(); ++i)
				{
					Seq = InSM->FindAnimSequence(Anims(i));
					if (Seq)
					{
						break;
					}
				}
				if (Seq)
				{
					return (Seq->SequenceLength - FLOAT(StartFrame) / 30.f) / Rate;
				}
				else
				{
					return Duration;
				}
			}
		}
	}

	return 0.f;
}

void UAbilityAnimationComponent::SetResLength(USkeletalMeshComponent* InSM, FLOAT InTime)
{
	UAbilityComponent::SetResLength(InSM, InTime);
	if (InTime <= 0)
	{
		InTime = 0;
	}
	if (StopType == ACBT_StopWhenConstDurationPass)
	{
		if (bLoop)
		{
			LoopTime = InTime;
		}
		else
		{
			if (InSM)
			{
				UAnimSequence* Seq = NULL;
				for (INT i = 0; i < Anims.Num(); ++i)
				{
					Seq = InSM->FindAnimSequence(Anims(i));
					if (Seq)
					{
						break;
					}
				}
				if (Seq)
				{
					Rate = (Seq->SequenceLength - FLOAT(StartFrame) / 30.f)  / InTime;
				}
			}
		}
	}
}

BaseComponent* UAbilityAnimationComponent::CreateCplusplusNode(void)
{
	AnimComponent* Ret = new AnimComponent;
	return Ret;
}

BaseComponent* UAbilityComp_MaterialEffect::CreateCplusplusNode(void)
{
	MaterialEffectComponent* Ret = new MaterialEffectComponent;
	return Ret;
}

FString UAbilityComp_MaterialEffect::GetResName()
{
	FString ResName;
	if (EffectType == EMET_None)
	{
		ResName = FString(TEXT("Material_None"));
	}
	else if (EffectType == EMET_LoadOpacityMaterial)
	{
		ResName = FString(TEXT("Material_LoadOpacityMaterial"));
	}
	else if (EffectType == EMET_ChangeMaterialProperty)
	{
		ResName = FString::Printf(TEXT("Material_SetEffect:%d"), EffectTemplateId);
	}
	else if (EffectType == EMET_ChangeMaterial)
	{
		ResName = FString(TEXT("Material_ChangeMat:")) + (EffectMaterialInstance ? EffectMaterialInstance->GetName() : FString(TEXT("No")));
	}
	return ResName;
}

BaseComponent* UAbilityEmitterComponent::CreateCplusplusNode(void)
{
	EmitterComponent* Ret = new EmitterComponent;
	return Ret;
}

BaseComponent* UAbilitySoundCueComponent::CreateCplusplusNode(void)
{
   SoundComponent* Ret = new SoundComponent;
   return Ret;
}

UBOOL UAbilitySoundCueComponent::GetSoundCueByElementaryType(BYTE InType, TArray<USoundCue*>& SoundList)
{
	for (INT i = 0; i < SoundsByElementaryType.Num(); ++i)
	{
		if (SoundsByElementaryType(i).Type == InType)
		{
			SoundList.Append(SoundsByElementaryType(i).Sounds);
			break;
		}
	}

	if (SoundList.Num() == 0)
	{
		SoundList.Append(Sounds);
	}

	if (SoundList.Num() > 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BaseComponent* UAbilityTimeFactorComponent::CreateCplusplusNode(void)
{
	TimeFactorComponent* Ret = new TimeFactorComponent;
	return Ret;
}

BaseComponent* UAbilityDecalComponent::CreateCplusplusNode(void)
{
   DecalComponent* Ret = new DecalComponent;
   return Ret;
}

BaseComponent* UCameraShakeComponent::CreateCplusplusNode(void)
{
	CameraAnimComponent* Ret = new CameraAnimComponent;
	return Ret;
}

BaseComponent* UAbilityHitPointComponent::CreateCplusplusNode(void)
{
   HitPointComponent* Ret = new HitPointComponent;
   return Ret;
}

BaseComponent* UAbilityHitEffectComponent::CreateCplusplusNode(void)
{
	PhysicHitComponent* Ret = new PhysicHitComponent;
	return Ret;
}

BaseComponent* UAbilityTrailComponent::CreateCplusplusNode(void)
{
   TrailComponent* Ret = new TrailComponent;
   return Ret;
}

UAnimNotify_Trails* UAbilityTrailComponent::GetTrailData(UAnimNodeSequence* Seq)
{
	if (Seq == NULL)
	{
		return NULL;
	}

	FString InStr = Seq->AnimSeq->GetFullName();
	//debugf(L"FullName=====%s", *InStr);
	INT idx = -1;
	for (INT i = 0; i < TrailList.Num(); ++i)
	{
		if (TrailList(i).KeyString == InStr)
		{
			idx = i;
			break;
		}
	}
	if (idx != -1)
	{
		return TrailList(idx).TrailData;
	}
	else
	{
		return NULL;
	}
}
UAnimNotify_Trails* UAbilityTrailComponent::AddNewTrail(UAnimNodeSequence* Seq, FLOAT StartTime, FLOAT EndTime)
{
	INT idx = TrailList.AddZeroed();
	if (!Seq)
	{
		return NULL;
	}

	FLOAT CurrentPos = Seq->CurrentTime;

	TrailList(idx).KeyString = Seq->AnimSeq->GetFullName();
	TrailList(idx).TrailData = ConstructObject<UAnimNotify_Trails>(UAnimNotify_Trails::StaticClass(), this);
	TrailList(idx).TrailData->PSTemplate = PSTemplate;
	TrailList(idx).TrailData->bPreview = TRUE;
	TrailList(idx).TrailData->FirstEdgeSocketName = FirstEdgeSocketName;
	TrailList(idx).TrailData->ControlPointSocketName = ControlPointSocketName;
	TrailList(idx).TrailData->SecondEdgeSocketName = SecondEdgeSocketName;
	TrailList(idx).TrailData->LastStartTime = StartTime;
	TrailList(idx).TrailData->EndTime = EndTime;
	TrailList(idx).TrailData->StoreAnimationData(Seq);

	Seq->SetPosition(CurrentPos, FALSE);
	if (Seq->SkelComponent)
	{
		Seq->SkelComponent->ForceSkelUpdate();
	}

	return TrailList(idx).TrailData;
}

void UAbilityTrailComponent::RebuildTrail(UAnimNodeSequence* Seq, FLOAT StartTime, FLOAT EndTime)
{
	UAnimNotify_Trails* notify = GetTrailData(Seq);
	if (notify)
	{
		FLOAT CurrentPos = Seq->CurrentTime;

		notify->PSTemplate = PSTemplate;
		notify->FirstEdgeSocketName = FirstEdgeSocketName;
		notify->FirstEdgeSocketName = FirstEdgeSocketName;
		notify->ControlPointSocketName = ControlPointSocketName;
		notify->SecondEdgeSocketName = SecondEdgeSocketName;
		notify->LastStartTime = StartTime;
		notify->EndTime = EndTime;
		notify->StoreAnimationData(Seq);

		Seq->SetPosition(CurrentPos, FALSE);
		if (Seq->SkelComponent)
		{
			Seq->SkelComponent->ForceSkelUpdate();
		}
	}
}

BaseComponent* UAbilityWindSourceComponent::CreateCplusplusNode(void)
{
	WindSourceComponent* Ret = new WindSourceComponent;
	return Ret;
}

BaseComponent* UAbilityActionProcComponent::CreateCplusplusNode(void)
{
	ActionProcComponent* Ret = new ActionProcComponent;
	return Ret;
}

BaseComponent* UAbilityScaleComponent::CreateCplusplusNode()
{
	ScaleComponent* Ret = new ScaleComponent;
	return Ret;
}

BaseComponent* UAbilityMirrorComponent::CreateCplusplusNode()
{
	MirrorComponent* Ret = new MirrorComponent;
	return Ret;
}







FString UCameraShakeComponent::GetResName()
{
	return AnimToPlay->GetName();
}


FString UAbilitySoundCueComponent::GetResName()
{
	if (SoundPlayType == ESPT_WeaponUseSound)
	{
		return L"UseSound";
	}
	else if (SoundPlayType == ESPT_WeaponHitSound)
	{
		return L"HitSound";
	}
	else
	{
		if (Sounds.Num() > 0 && Sounds(0))
		{
			return Sounds(0)->GetName();
		}
		else
		{
			return L"NoData";
		}
	}
}

FString UAbilityHitPointComponent::GetResName()
{
	return L"HitPoint";
}

void UAbilityHitPointComponent::CopyObjectProperties(UAbilityComponent* DstComponent)
{
	UAbilityHitPointComponent* Dst = Cast<UAbilityHitPointComponent>(DstComponent);
	if (!Dst)
	{
		return;
	}

	Dst->ClientEffectList.Empty();
	for (INT i = 0; i < ClientEffectList.Num(); ++i)
	{
		if (ClientEffectList(i))
		{
			INT j = Dst->ClientEffectList.AddZeroed();
			Dst->ClientEffectList(j) = ConstructObject<UAbilityComponent>(ClientEffectList(i)->GetClass(), Dst, NAME_None, 0, ClientEffectList(i));
		}
	}

	Dst->ServerEffectList.Empty();
	for (INT i = 0; i < ServerEffectList.Num(); ++i)
	{
		if (ServerEffectList(i))
		{
			INT j = Dst->ServerEffectList.AddZeroed();
			Dst->ServerEffectList(j) = ConstructObject<UAbilityComponent>(ServerEffectList(i)->GetClass(), Dst, NAME_None, 0, ServerEffectList(i));
		}
	}

	Dst->BulletComponents.Empty();
	for (INT i = 0; i < BulletComponents.Num(); ++i)
	{
		if (BulletComponents(i))
		{
			INT j = Dst->BulletComponents.AddZeroed();
			Dst->BulletComponents(j) = ConstructObject<UAbilityEmitterComponent>(BulletComponents(i)->GetClass(), Dst, NAME_None, 0, BulletComponents(i));
		}
	}
}

FString UAbilityEmitterComponent::GetResName()
{
	if (ParticleSystemTemplate)
	{
		return ParticleSystemTemplate->GetName();
	}
	else
	{
		return L"NoData";
	}
}

FLOAT UAbilityEmitterComponent::GetResLength(USkeletalMeshComponent* InSM)
{
	FLOAT Ret = 0.f;
	if (StopType == ACBT_StopWhenConstDurationPass)
	{
		if (ParticleSystemTemplate)
		{
			Ret = ParticleSystemTemplate->GetMaxLifespan(0.05);
			if (Ret == 0.0f) //
			{
				Ret = Duration;
			}
		}
	}
	return Ret;
}

FMatrix UAbilityEmitterComponent::GetSocketMatrix(USkeletalMeshComponent* InComponent, FName InSocketName, UBOOL bUseRelativePostion)
{
	FMatrix Ret = FMatrix::Identity;
	if (InComponent == NULL)
	{
		return Ret;
	}

	UBOOL bFind = InComponent->GetSocketWorldTransformMatrix(InSocketName, Ret, 0);
	if (bFind == FALSE)
	{
		Ret = InComponent->CachedParentToWorld;
	}

	if (!bUseSocketRotation)
	{
		Ret = FTranslationMatrix(Ret.GetOrigin());
	}

	if(bUseRelativePostion)
	{
		FRotationTranslationMatrix rtm(RelativeRotation, RelativeLocation);
		Ret = rtm * Ret;
	}

	return Ret;
}

FName UAbilityEmitterComponent::FindTheNearestSocketToTarget(AActor* InBaseActor, AActor* InTargetActor, FName InSocketName)
{
	FName Ret = NAME_None;

	// check
	if (!InBaseActor || !InTargetActor)
	{
		return Ret;
	}

	if (InSocketName == NAME_None)
	{
		return Ret;
	}

	AMMOActor* mmoBaseActor = Cast<AMMOActor>(InBaseActor);
	if (!mmoBaseActor || !mmoBaseActor->Mesh || !mmoBaseActor->Mesh->SkeletalMesh)
	{
		return Ret;
	}

	const INT HitSocketMaxNumber = 8;
	TArray<FName> SocketList;
	for (INT i = 1; i <= HitSocketMaxNumber; ++i)
	{
		FString SocketString = InSocketName.ToString();
		FString NewSocket = SocketString.Left(SocketString.Len() - 1) + FString::Printf(TEXT("%d"), i);
		FName SocketName = FName(*NewSocket);

		USkeletalMeshSocket* MeshSocket = mmoBaseActor->Mesh->SkeletalMesh->FindSocket(SocketName);
		if (MeshSocket)
		{
			SocketList.AddItem(SocketName);
		}
		else
		{
			break;
		}
	}

	if (SocketList.Num() == 0)
	{
		return Ret;
	}

	FRotator Dir = (InTargetActor->Location - InBaseActor->Location).SafeNormal().Rotation().GetDenormalized();
	FLOAT roll1 = 32768.f / SocketList.Num();

	INT DR = (Dir.Yaw - InBaseActor->Rotation.Yaw) & 0xFFFF;
	INT count = FLOAT(DR) / roll1;
	count = (count + 1) / 2 + 1;
	if (count > SocketList.Num())
	{
		count %= SocketList.Num();
	}
	check(count >= 1);
	return SocketList(count - 1);
}

FMatrix UAbilityEmitterComponent::GetSocketMatrix(AActor* InActor, FVector& InTargetPoint, UBOOL bTarget, FName OverwriteSocketName)
{
	if (InActor->IsA(AMMOActor::StaticClass()))
	{
		USkeletalMeshComponent* BaseMesh = NULL;
		USkeletalMeshComponent* WeaponMesh1 = NULL;
		USkeletalMeshComponent* WeaponMesh2 = NULL;

		AMMOActor* mmoActor = (AMMOActor*)InActor;
		mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);

		return GetSocketMatrix(mmoActor->GetActiveBaseMesh(), WeaponMesh1, WeaponMesh2, InTargetPoint, bTarget, OverwriteSocketName);
	}
	else
	{
		return InActor->LocalToWorld();
	}
}

FMatrix UAbilityEmitterComponent::GetSocketMatrix(USkeletalMeshComponent* InBaseComponent, USkeletalMeshComponent* InWeaponComponent1, USkeletalMeshComponent* InWeaponComponent2, FVector InTargetPoint, UBOOL bTarget, FName OverwriteSocketName)
{
	FMatrix Ret = FMatrix::Identity;
	UBOOL bFound = FALSE;
	if (bTarget)
	{
		if (InBaseComponent)
		{
			bFound = InBaseComponent->GetSocketWorldTransformMatrix(TargetSocketName, Ret, 0);
			if (!bFound)
			{
				Ret = InBaseComponent->CachedParentToWorld;
			}
		}
	}
	else
	{
		FName ToUseName = AttachSocketName;
		if (OverwriteSocketName != NAME_None)
		{
			ToUseName = OverwriteSocketName;
		}
		Ret = GetSpawnMatrix(InBaseComponent, InWeaponComponent1, InWeaponComponent2, InTargetPoint, ToUseName, SourceType, RelativeLocation, RelativeRotation);
	}

	return Ret;
}



FLOAT UAbilityEmitterComponent::GetLifeSpan(FVector InRelativeTargetPoint)
{
	FLOAT Ret = 0.f;
	if (StopType == ACBT_StopWhenConstDurationPass)
	{
		FLOAT ResTime = 0.f;
		if (ParticleSystemTemplate)
		{
			ResTime = ParticleSystemTemplate->GetMaxLifespan(0.05);
		}

		if (EmitterType == EMIT_FlyTo)
		{
			if (TrackTimeType == ETTT_ConstSpeed)
			{
				ResTime = InRelativeTargetPoint.Size() / Speed;
			}
			else
			{
				ResTime = FlyTime;
			}
		}
		else
		{
			// ResTime = ResTime;
		}

		Ret = Max(ResTime, Duration);
	}
	return Ret;
}

UBOOL UAbilityEmitterComponent::GetBezierControlPoint(FVector& OutV1, FVector& OutV2)
{
	if (BezierControlPointList.Num() == 0)
	{
		return FALSE;
	}

	INT idx = RandHelper(BezierControlPointList.Num());
	OutV1 = BezierControlPointList(idx).v1;
	OutV2 = BezierControlPointList(idx).v2;
	return TRUE;
}

UParticleSystem* UAbilityEmitterComponent::GetParticleSystemByElementaryType(EElementaryType InType)
{
	UParticleSystem* Ret = NULL;
	for (INT i = 0; i < ParticlesByElementTypeDataList.Num(); ++i)
	{
		if (ParticlesByElementTypeDataList(i).Type == InType && ParticlesByElementTypeDataList(i).ParticleSystems.Num() > 0)
		{
			Ret = ParticlesByElementTypeDataList(i).ParticleSystems(RandHelper(ParticlesByElementTypeDataList(i).ParticleSystems.Num()));
			break;
		}
	}

	if (Ret == NULL)
	{
		if (RandParticleSystemTemplates.Num() > 0)
		{
			Ret = RandParticleSystemTemplates(RandHelper(RandParticleSystemTemplates.Num()));
		}
	}

	if (Ret == NULL)
	{
		Ret = ParticleSystemTemplate;
	}

	return Ret;
}


USoundCue* UAbilityEmitterComponent::GetSoundCueByElementaryType(EElementaryType InType)
{
	USoundCue* Ret = SoundCueToPlay;
	for (INT i = 0; i < SoundCueToPlayByElementaryType.Num(); ++i)
	{
		if (InType == SoundCueToPlayByElementaryType(i).Type)
		{
			if (SoundCueToPlayByElementaryType(i).Sounds.Num() > 0 && SoundCueToPlayByElementaryType(i).Sounds(0))
			{
				Ret = SoundCueToPlayByElementaryType(i).Sounds(0);
			}
			break;
		}
	}
	return Ret;
}

FString UAbilitySkeletalMeshComponent::GetResName()
{
	return L"Mesh";
}

struct BaseComponent* UAbilitySkeletalMeshComponent::CreateCplusplusNode()
{
	return new SkeletalMeshComponent;
}

FMatrix UAbilitySkeletalMeshComponent::GetSocketMatrix(AActor* InActor, FVector& InTargetPoint, UBOOL bTarget)
{
	if (InActor->IsA(AMMOActor::StaticClass()))
	{
		USkeletalMeshComponent* BaseMesh = NULL;
		USkeletalMeshComponent* WeaponMesh1 = NULL;
		USkeletalMeshComponent* WeaponMesh2 = NULL;

		AMMOActor* mmoActor = (AMMOActor*)InActor;
		mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
		return GetSpawnMatrix(mmoActor->Mesh, WeaponMesh1, WeaponMesh2, InTargetPoint, AttachSocketName, SourceType, RelativeLocation, RelativeRotation);
	}
	else
	{
		FRotationTranslationMatrix frtm(RelativeRotation, RelativeLocation);
		return frtm * InActor->LocalToWorld();
	}
}

struct BaseComponent* UAbilityStaticMeshComponent::CreateCplusplusNode()
{
	return new StaticMeshComponent;
}

FMatrix UAbilityStaticMeshComponent::GetSocketMatrix(AActor* InActor, FVector& InTargetPoint, UBOOL bTarget)
{
	if (InActor->IsA(AMMOActor::StaticClass()))
	{
		USkeletalMeshComponent* BaseMesh = NULL;
		USkeletalMeshComponent* WeaponMesh1 = NULL;
		USkeletalMeshComponent* WeaponMesh2 = NULL;

		AMMOActor* mmoActor = (AMMOActor*)InActor;
		mmoActor->GetWeaponSkeletalMeshComponents(WeaponMesh1, WeaponMesh2);
		return GetSpawnMatrix(mmoActor->Mesh, WeaponMesh1, WeaponMesh2, InTargetPoint, AttachSocketName, SourceType, RelativeLocation, RelativeRotation);
	}
	else
	{
		FRotationTranslationMatrix frtm(RelativeRotation, RelativeLocation);
		return frtm * InActor->LocalToWorld();
	}
}


FString UAbilityPostProcessComponent::GetResName()
{
	return FString(TEXT("PostProcess"));
}

struct BaseComponent* UAbilityPostProcessComponent::CreateCplusplusNode()
{
	return new PostProcessComponent;
}


FString UAbilityDecalComponent::GetResName()
{
	if (ImpactDecalMaterials.Num() > 0 && ImpactDecalMaterials(0))
	{
		return ImpactDecalMaterials(0)->GetName();
	}
	else
	{
		return L"NoData";
	}
}

FString UAbilityWindSourceComponent::GetResName()
{
	return FString(TEXT("WinSource"));
}

FString UAbilityTrailComponent::GetResName()
{
	return FString(TEXT("Trail"));
}


FString UAbilityMoveControlComponent::GetResName()
{
	return L"MoveControl";
}


struct BaseComponent* UAbilityMoveControlComponent::CreateCplusplusNode()
{
	MoveControlComponent* Ret = new MoveControlComponent;
	return Ret;
}

FString UAbilityThrowComponent::GetResName()
{
	return FString(TEXT("Throw"));
}

struct BaseComponent* UAbilityThrowComponent::CreateCplusplusNode()
{
	return new ThrowComponent;
}

struct BaseComponent* UAbilityActionsComponent::CreateCplusplusNode()
{
	return new ActionsComponent;
}

struct BaseComponent* UAbilityVisibleComponent::CreateCplusplusNode()
{
	return new VisibleComponent;
}

struct BaseComponent* UAbilityPhysicSimulateComponent::CreateCplusplusNode()
{
	return new PhysicalSimulateComponent;
}


FLOAT UMMOAbility::GetAbilitySpellTime()
{
	if (SpellCommonAttribute.PrepareType == ES_PT_Type1)
	{
		return SpellCommonAttribute.PrepareTimeMin;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type2)
	{
		return SpellCommonAttribute.PrepareTimeMin;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type3)
	{
		return SpellCommonAttribute.PrepareTimeMin;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type4)
	{
		return 0;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type5)
	{
		return SpellCommonAttribute.PrepareTimeMax;
	}
	else
	{
		return 0;
	}
}

FLOAT UMMOAbility::GetAbilityCastTime()
{
	if (SpellCommonAttribute.PrepareType == ES_PT_Type1)
	{
		return SpellCommonAttribute.Interval;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type2)
	{
		return SpellCommonAttribute.HardTime;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type3)
	{
		return SpellCommonAttribute.HardTime;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type4)
	{
		return SpellCommonAttribute.HardTime;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type5)
	{
		return SpellCommonAttribute.HardTime;
	}
	else
	{
		return 0;
	}

}

void UMMOAbility::GetSpellTime(FLOAT& Time)
{
	FLOAT Ret = 0.f;
	if (SpellCommonAttribute.PrepareType == ES_PT_Type4)
	{
		Ret = 0;
	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type5)
	{
		Ret = SpellCommonAttribute.PrepareTimeMax;
	}
	else
	{
		Ret = SpellCommonAttribute.PrepareTimeMin;
	}

	Time = Ret;
}

void UMMOAbility::GetCastTime(FLOAT& Time,INT& CycleNumber,FLOAT& FixTime)
{
	if (SpellCommonAttribute.PrepareType == ES_PT_Type1)
	{
		Time = SpellCommonAttribute.Interval;
		CycleNumber = SpellCommonAttribute.ChannelCount;
		FixTime = (SpellCommonAttribute.HardTime - SpellCommonAttribute.Interval);
	}
	else
	{
		Time = SpellCommonAttribute.HardTime;
		CycleNumber = 1;
		FixTime = 0.f;
	}
}


FString UMMOAbility::GetAbilityDetailDescribe()
{
	return SkillDetailDescribe;
}

void UMMOAbility::ParseAbilityDetailDescribe()
{
	SkillDetailDescribeArray.Empty();

	FString copyDetailDescribe = SkillDetailDescribe;
	INT pos = copyDetailDescribe.InStr( TEXT("<br>") , FALSE , TRUE );
	while ( pos >= 0 )
	{
		SkillDetailDescribeArray.AddItem( copyDetailDescribe.Left(pos) );
		copyDetailDescribe = copyDetailDescribe.RightChop( pos+FString(TEXT("<br>")).Len() );

		pos = copyDetailDescribe.InStr( TEXT("<br>") , FALSE , TRUE );
		
	}

	if( !SkillDetailDescribe.IsEmpty() )
	{
		SkillDetailDescribeArray.AddItem(copyDetailDescribe);
	}
	
}

void UMMOAbility::SetSpellTime(FLOAT InNewTime)
{
	if (SpellCommonAttribute.PrepareType == ES_PT_Type4)
	{

	}
	else if (SpellCommonAttribute.PrepareType == ES_PT_Type5)
	{
		SpellCommonAttribute.PrepareTimeMax = InNewTime;
	}
	else
	{
		SpellCommonAttribute.PrepareTimeMin = InNewTime;
	}
}

void UMMOAbility::SetCastTime(FLOAT InCastTime)
{
	if (SpellCommonAttribute.PrepareType == ES_PT_Type1)
	{
		SpellCommonAttribute.Interval = InCastTime;
	}
	else
	{
		SpellCommonAttribute.HardTime = InCastTime;
	}
}

float UMMOAbility::GetCoolDownBufTime(AMMOActor* player)
{
	float ModifyValue = 1.0;
	float offsetTime = 0.25;
	float res = -1.0;
	if(player)
	{
		if(SpellCommonAttribute.AffectedBySpeedParam)
		{
			ModifyValue = player->GpDataManager->GetWeaponSpeedFactor(player->GetWeaponType());
		}

		if(ClientCommonAttribute.ModifyCD>0.001)
		{
			res=ClientCommonAttribute.ModifyCD*ModifyValue;
		}
		else if(SpellCommonAttribute.CancelHardTime)
		{
			res=(GetAbilitySpellTime())*ModifyValue;
		}
		else
		{
			res=(GetAbilitySpellTime()+GetAbilityCastTime())*ModifyValue;
		}
	}
	return res;

}


float UMMOAbility::GetSpellBufTime(AMMOActor* player)
{
	float ModifyValue = 1.0;
	float offsetTime = 0.25;
	float res = -1.0;
	if(player)
	{
		if(SpellCommonAttribute.AffectedBySpeedParam)
		{
			ModifyValue = player->GpDataManager->GetWeaponSpeedFactor(player->GetWeaponType());
		}

		if(ClientCommonAttribute.ModifyCD>0.001)
		{
			//res=ClientCommonAttribute.ModifyCD*ModifyValue - offsetTime;
			res=ClientCommonAttribute.ModifyCD*ModifyValue;
		}
		else
		{
			//res=(GetAbilitySpellTime()+GetAbilityCastTime())*ModifyValue - offsetTime;
			res=(GetAbilitySpellTime())*ModifyValue;
		}
	}
	return res;
}

//song@snda.com
UBOOL UMMOAbility::IsNeedArchetype()
{
	return false;
}



void UMMOAbility::DataToExcel(TArray<FString>& Row)
{

}

void UMMOAbility::DataToExcel2(TArray<FString>& Row)
{
	int nCasterCheckNodeID	= 0;
	int nTargetCheckNodeID	= 0;
	int nOnLearnID			= 0;
	int nOnForgetID			= 0;
	int nOnEquipID			= 0;
	int nOnUnEquipID		= 0;
	int nOnPrepareID	= 0;
	int nOnCastID		= 0;
	int nOnBreakID		= 0;
	int nOnCreateID		= 0;
	int nBeforeRollID	= 0;
	int nAfterRollID	= 0;
	int nBeforeEffectID	= 0;
	int nAfterEffectID	= 0;

	int nOnExCast1		= 0;
	int nOnExCast2		= 0;
	int nOnExCast3		= 0;
	int nOnExCast4		= 0;
	int nOnExCast5		= 0;

	if(SpellCommonAttribute.CasterCheckNode)
		nCasterCheckNodeID = SpellCommonAttribute.CasterCheckNode->Id;
	if(SpellCommonAttribute.TargetCheckNode)
		nTargetCheckNodeID = SpellCommonAttribute.TargetCheckNode->Id;
	if(SpellCommonAttribute.OnLearn)
		nOnLearnID = SpellCommonAttribute.OnLearn->Id;
	if(SpellCommonAttribute.OnForget)
		nOnForgetID = SpellCommonAttribute.OnForget->Id;

	if(SpellCommonAttribute.OnEquip)
		nOnEquipID = SpellCommonAttribute.OnEquip->Id;
	if(SpellCommonAttribute.OnUnEquip)
		nOnUnEquipID = SpellCommonAttribute.OnUnEquip->Id;
	if(SpellCommonAttribute.OnPrepare)
		nOnPrepareID = SpellCommonAttribute.OnPrepare->Id;
	if(SpellCommonAttribute.OnCast)
		nOnCastID = SpellCommonAttribute.OnCast->Id;
	if(SpellCommonAttribute.OnBreak)
		nOnBreakID = SpellCommonAttribute.OnBreak->Id;
	if(SpellCommonAttribute.OnBulletCreate)
		nOnCreateID	= SpellCommonAttribute.OnBulletCreate->Id;
	if(SpellCommonAttribute.BeforeRoll)
		nBeforeRollID = SpellCommonAttribute.BeforeRoll->Id;
	if(SpellCommonAttribute.AfterRoll)
		nAfterRollID = SpellCommonAttribute.AfterRoll->Id;
	if(SpellCommonAttribute.BeforeEffect)
		nBeforeEffectID = SpellCommonAttribute.BeforeEffect->Id;
	if(SpellCommonAttribute.AfterEffect)
		nAfterEffectID = SpellCommonAttribute.AfterEffect->Id;

	if(SpellCommonAttribute.OnExCast1)
		nOnExCast1 = SpellCommonAttribute.OnExCast1->Id;
	if(SpellCommonAttribute.OnExCast2)
		nOnExCast2 = SpellCommonAttribute.OnExCast2->Id;
	if(SpellCommonAttribute.OnExCast3)
		nOnExCast3 = SpellCommonAttribute.OnExCast3->Id;
	if(SpellCommonAttribute.OnExCast4)
		nOnExCast4 = SpellCommonAttribute.OnExCast4->Id;
	if(SpellCommonAttribute.OnExCast5)
		nOnExCast5 = SpellCommonAttribute.OnExCast5->Id;

	Row.AddItem(FString::Printf( TEXT("%d"), SpellCommonAttribute.Id));
	//Row.AddItem((TEXT("_IMPORT_")));
	//Row.AddItem(FString::Printf( TEXT("%d"), SpellCommonAttribute.Level));
	Row.AddItem(FString::Printf( TEXT("%d"), SpellCommonAttribute.RuneSlotID));
	Row.AddItem(FString::Printf( TEXT("%d"), nCasterCheckNodeID));
	Row.AddItem(FString::Printf( TEXT("%d"), nTargetCheckNodeID));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnLearnID));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnForgetID));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnEquipID));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnUnEquipID));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnPrepareID));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnCastID));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnBreakID));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnCreateID));
	Row.AddItem(FString::Printf( TEXT("%d"), nBeforeRollID));
	Row.AddItem(FString::Printf( TEXT("%d"), nAfterRollID));
	Row.AddItem(FString::Printf( TEXT("%d"), nBeforeEffectID));
	Row.AddItem(FString::Printf( TEXT("%d"), nAfterEffectID));

	Row.AddItem(FString::Printf( TEXT("%d"), nOnExCast1));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnExCast2));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnExCast3));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnExCast4));
	Row.AddItem(FString::Printf( TEXT("%d"), nOnExCast5));
	//Row.AddItem(FString::Printf( TEXT("%d"), AfterDamageForCaster));
	//Row.AddItem(FString::Printf( TEXT("%d"), AfterDamageForTarget));
}

void UMMOAbility::ExportExcelHeaders(TArray<FString>& Header,TArray<FString>& DefaultValues)
{
}

void UMMOAbility::ExportExcelHeaders2(TArray<FString>& Header,TArray<FString>& DefaultValues)
{
	//Item.AddItem("Index"); // Handled uniformly by the lower layer
	Header.AddItem("SpellID");
	DefaultValues.AddItem("0");

	//Header.AddItem("SpellLevel");
	//DefaultValues.AddItem("0");

	Header.AddItem("RuneSlotID");
	DefaultValues.AddItem("0");

	Header.AddItem("CasterCheckNode");
	DefaultValues.AddItem("0");

	Header.AddItem("TargetCheckNode");
	DefaultValues.AddItem("0");

	Header.AddItem("OnLearn");
	DefaultValues.AddItem("0");

	Header.AddItem("OnForget");
	DefaultValues.AddItem("0");

	Header.AddItem("OnEquip");
	DefaultValues.AddItem("0");

	Header.AddItem("OnUnEquip");
	DefaultValues.AddItem("0");

	Header.AddItem("OnPrepare");
	DefaultValues.AddItem("0");

	Header.AddItem("OnCast");
	DefaultValues.AddItem("0");

	Header.AddItem("OnBreak");
	DefaultValues.AddItem("0");

	Header.AddItem("OnBulletCreate");
	DefaultValues.AddItem("0");

	Header.AddItem("BeforeRoll");
	DefaultValues.AddItem("0");

	Header.AddItem("AfterRoll");
	DefaultValues.AddItem("0");

	Header.AddItem("BeforeEffect");
	DefaultValues.AddItem("0");

	Header.AddItem("AfterEffect");
	DefaultValues.AddItem("0");

	Header.AddItem("OnExCast1");
	DefaultValues.AddItem("0");

	Header.AddItem("OnExCast2");
	DefaultValues.AddItem("0");

	Header.AddItem("OnExCast3");
	DefaultValues.AddItem("0");

	Header.AddItem("OnExCast4");
	DefaultValues.AddItem("0");

	Header.AddItem("OnExCast5");
	DefaultValues.AddItem("0");

}
void UMMOAbility::InitDataToExcel()
{
}
//------------------------------------------------------------------------
void UMMOAbility::ImportFromExcel(const TArray<FString>& Values)
{	
}

FString UMMOAbility::GetAbilityIconName()
{
	FString FullName;

	UObject* Obj = ClientCommonAttribute.DisplayIcon;
	if( Obj )
	{
		FullName = FString( Obj->GetPureName().ToString() );
		while( Obj && Obj->GetOuter() )
		{
			FullName = FString(Obj->GetOuter()->GetPureName().ToString()) + TEXT(".") + FullName;
			Obj= Obj->GetOuter();
		}
	}
	//int i = 0;
	//FString str = FString( DisplayIcon->GetOutermost() ->GetOut());// ->TextureFileCacheName.ToString() );
	return FullName;
}

int UMMOAbility::GetTemplateID() const
{
	return SpellCommonAttribute.Id;
}

void UMMOAbility::ParseGroupIds()
{
	FString str = SpellCommonAttribute.GroupCoolDown;

	INT Idx = 0, NextIdx = 0;

	FString IdStr, ValueStr;

	UBOOL bFindMid = FALSE;

	for (INT i = 0; i < str.Len(); ++i)
	{
		if (!bFindMid)
		{
			if (str[i] == TCHAR('-'))
			{
				bFindMid = TRUE;
				continue;
			}
			else if (str[i] == TCHAR(';'))
			{
				IdStr.Empty();
				ValueStr.Empty();
				continue;
			}
			else
			{
				IdStr += str[i];
			}
		}
		else
		{
			if (str[i] == TCHAR('-'))
			{
				IdStr.Empty();
				ValueStr.Empty();
				continue;
			}
			else if (str[i] == TCHAR(';'))
			{
				INT GroupId = appAtoi(*IdStr);
				GroupIds.Empty();
				GroupIds.AddItem(GroupId);
				bFindMid = FALSE;
				IdStr.Empty();
				ValueStr.Empty();
			}
			else
			{
				ValueStr += str[i];
			}
		}
	}
}

UBOOL UMMOAbility::CanUseSelfCheck(class AMMOActor* InActor, UBOOL bCheckCD)
{
	if (!InActor)
	{
		return FALSE;
	}

	if (InActor->bDead)
	{
		return FALSE;
	}

	if (bCheckCD)
	{
		if (bCoolDown)
		{
			return FALSE;
		}
	}

	if (InActor->eventIsStallState())
	{
		return FALSE;
	}

	AMMOPlayerController* MainController = Cast<AMMOPlayerController>(InActor->Controller);
	if (!MainController)
	{
		return FALSE;
	}

	if (MainController->CanUseAnyAbility(this) == FALSE)
	{
		return FALSE;
	}

	if (!CanUseManaCheck(InActor))
	{
		return FALSE;
	}


	return TRUE;
}

UBOOL UMMOAbility::CanUseManaCheck(AMMOActor* InActor)
{
	if (!InActor || !InActor->IsMainPlayer())
	{
		return FALSE;
	}

	FLOAT InitResCost = FLOAT(SpellCommonAttribute.InitialCostValue) + FLOAT(SpellCommonAttribute.InitialCostValuePct) * FLOAT(InActor->GetActorAttributeValue(catMaxMp)) / 10000.f;
	FLOAT ExtraResCost = FLOAT(SpellCommonAttribute.ModifierCostValue) + FLOAT(SpellCommonAttribute.ModifierCostValuePct) * FLOAT(InActor->GetActorAttributeValue(catMaxMp)) / 10000.f;

	// JO*(WJOSIJERg
	//InitResCost *= InActor->GetActorAttributeValue(catEnergyCostModificationRate) / 10000.f;
	//ExtraResCost *= InActor->GetActorAttributeValue(catEnergyCostModificationRate) / 10000.f;
	// JO*(WJOSIJERg end

	//if (SpellCommonAttribute.CostType != ES_COST_Type4)
	//{
		if (InitResCost + ExtraResCost > InActor->GetMp())
		{
			return FALSE;
		}
	//}
	//else
	//{
	//	if (InitResCost + ExtraResCost >= 0)
	//	{
	//		if (InitResCost + ExtraResCost > InActor->GetMp())
	//		{
	//			return FALSE;
	//		}
	//	}
	//	else
	//	{
	//		if (InActor->GetMp() - InitResCost - ExtraResCost > InActor->GetActorAttributeValue(catMaxMp))
	//		{
	//			return FALSE;
	//		}
	//	}
	//}


	return TRUE;
}

//Get the rune Icon at the specified sub-rune Slot position
FString  UMMOAbility::GetSubRuneIconName(int RuneSlot )
{
	if( RuneSlot <0 || RuneSlot >= SubRuneList.Num() )
		return "Texture2D'MMO_UIIcon.Blank'"; 
	return SubRuneList( RuneSlot-1 )->RuneIconName;
}
//Get rune Icon
FString  UMMOAbility::GetRuneIconName( )
{
	//FString str = GetAbilityIconName();
	return RuneIconName;
}

//Get SkillGuiRuneID
INT  UMMOAbility::GetSkillGuiRuneID( )
{
    //@Temp no art assets for now, directly use an existing set
    //return SkillGuiRuneID;
    return GetRuneSlot();
}

//Add associated child rune
void UMMOAbility::AddSubRune(int RuneSlot,UMMOAbility *SubRune )
{
	if( RuneSlot < 1 ){
		return;
	}
	//Rune count is not fixed
	//Reallocation is also unpleasant, but given preprocessing, time is sacrificed here
	if( SubRuneList.Num() < RuneSlot ){
		SubRuneList.AddZeroed( RuneSlot - SubRuneList.Num() );
	}
	SubRune->MainSkill = this;
	//Texture2D'MMO_UIIcon.Rune.2003_Rune2_1'
	//0 means none, Slot1~5
	//@Temp no art assets for now, directly use an existing set
	SubRune->RuneIconName = FString::Printf(TEXT("img://MMO_UIIcon.Rune.%d_Rune%d_"), 2003/*SpellCommonAttribute.Id*/,SubRune->GetRuneSlot() );
	SubRuneList( RuneSlot-1 ) = SubRune;
    //All active ability runes number 5
    SubRune->SkillGuiRuneID = SkillGuiID*UCONST_MaxSubRuneNum + RuneSlot;
}

//Get the currently equipped RuneSlot
int UMMOAbility::GetRuneSlot( )
{
	return SpellCommonAttribute.RuneSlotID;
}

//Set equipped RuneSlot
void UMMOAbility::SetRuneSlot( int inRuneSlot )
{
	SpellCommonAttribute.RuneSlotID = inRuneSlot ;
}

FString UMMOAbility::GetMainSpeciality()
{
	return SkillMainSpeciality;
}

FString UMMOAbility::GetSecondSpeciality()
{
	return SkillSecondSpeciality;
}

FString UMMOAbility::GetThirdSpeciality()
{
	return SkillThirdSpeciality;
}

//------------------------------------------------------------------------
//int UMMOAbility::GetAbilityIconName()
//{
//	return ClientCommonAttribute.DisplayIcon;
//}

INT UMMOAbility::GetWeaponHitType(BYTE InWeaponType)
{
	INT Ret = -1;
	for (INT i = 0; i < ClientCommonAttribute.HitGroupInfo.Num(); ++i)
	{
		if (InWeaponType == ClientCommonAttribute.HitGroupInfo(i).WeaponType)
		{
			Ret = ClientCommonAttribute.HitGroupInfo(i).WeaponHitType;
			break;
		}
	}
	return Ret;
}

class UAbilityHitPointComponent* UMMOAbility::GetAbilityHitPointDataByWeaponType(BYTE InWeaponType,INT InIndex)
{
	TArray<FComponentNode>* SpellEffectList = NULL;
	TArray<FComponentNode>* CastEffectList = NULL;
	UBOOL bFind = GetEffectComponentList(InWeaponType, &SpellEffectList, &CastEffectList);
	if (bFind == FALSE)
	{
		return NULL;
	}

	if (InIndex >= 5)
	{
		for (INT i = 0; i < (*CastEffectList).Num(); ++i)
		{
			if ((*CastEffectList)(i).Component && (*CastEffectList)(i).Component->IsA(UAbilityHitPointComponent::StaticClass()))
			{
				return Cast<UAbilityHitPointComponent>((*CastEffectList)(i).Component);
			}
		}
		return NULL;
	}
	else
	{
		for (INT i = 0; i < (*SpellEffectList).Num(); ++i)
		{
			if ((*SpellEffectList)(i).Component && (*SpellEffectList)(i).Component->IsA(UAbilityHitPointComponent::StaticClass()))
			{
				UAbilityHitPointComponent* hitData = (UAbilityHitPointComponent*)((*SpellEffectList)(i).Component);
				if (hitData->HitPointId == InIndex)
				{
					return hitData;
				}
			}
		}
		return NULL;
	}
}

void UMMOAbility::AddComponent(class UClass* ClassType,UBOOL bSpellComponent, BYTE InWeaponType, class UAbilityComponent* InTemplate)
{
	for (INT i = 0; i < OverrideComponentList.Num(); ++i)
	{
		if (OverrideComponentList(i).Type == InWeaponType)
		{
			if (bSpellComponent)
			{
				INT NewIdx = OverrideComponentList(i).OverrideSpellComponentList.AddZeroed();
				OverrideComponentList(i).OverrideSpellComponentList(NewIdx).StartTime = 0.f;
				OverrideComponentList(i).OverrideSpellComponentList(NewIdx).Component = ConstructObject<UAbilityComponent>(ClassType, this, NAME_None, 0, InTemplate);
			}
			else
			{
				INT NewIdx = OverrideComponentList(i).OverrideCastComponentList.AddZeroed();
				OverrideComponentList(i).OverrideCastComponentList(NewIdx).StartTime = 0.f;
				OverrideComponentList(i).OverrideCastComponentList(NewIdx).Component = ConstructObject<UAbilityComponent>(ClassType, this, NAME_None, 0, InTemplate);
			}
			return;
		}
	}

	if (DefaultWeaponType == InWeaponType)
	{
		if (bSpellComponent)
		{
			INT NewIdx = SpellComponentList.AddZeroed();
			SpellComponentList(NewIdx).StartTime = 0.f;
			SpellComponentList(NewIdx).Component = ConstructObject<UAbilityComponent>(ClassType, this, NAME_None, 0, InTemplate);
		}
		else
		{
			INT NewIdx = CastComponentList.AddZeroed();
			CastComponentList(NewIdx).StartTime = 0.f;
			CastComponentList(NewIdx).Component = ConstructObject<UAbilityComponent>(ClassType, this, NAME_None, 0, InTemplate);
		}
	}
}

UBOOL UMMOAbility::GetEffectComponentList(BYTE InWeaponType, TArray<FComponentNode>** OutSpellComponentList, TArray<FComponentNode>** OutCastComponentList)
{
	for (INT i = 0; i < OverrideComponentList.Num(); ++i)
	{
		if (InWeaponType == OverrideComponentList(i).Type)
		{
			*OutSpellComponentList = &(OverrideComponentList(i).OverrideSpellComponentList);
			*OutCastComponentList = &(OverrideComponentList(i).OverrideCastComponentList);
			return TRUE;
		}
	}

	if (InWeaponType == DefaultWeaponType)
	{
		*OutSpellComponentList = &SpellComponentList;
		*OutCastComponentList = &CastComponentList;
		return TRUE;
	}

	return FALSE;
}


void UGhostSpawner::OnAction(INT CreatorId,INT TargetId,INT InPoint)
{
	AActor* BaseActor = MMOFacade::getSingleton().GetActor(CreatorId);
	AActor* Target = MMOFacade::getSingleton().GetActor(TargetId);

	if (!BaseActor || !Target)
	{
		return;
	}

	AMMOActor* mmoActor = NULL;
	if(BaseActor->IsA(AMMOActor::StaticClass()))
	{
		mmoActor = (AMMOActor*)BaseActor;
	}

	INT NumberToSpawn = appCeil(SpawnNumPerPoint * FLOAT(InPoint));


	 FLOAT Seed = FLOAT(RandHelper(65536)) * 2 * PI / 65536.f;

	 for (INT i = 0; i < NumberToSpawn; ++i)
	 {
		 AMMOEmitter* ProActor = NULL;

		 // base res
		 ProActor = Cast<AMMOEmitter>(GWorld->SpawnActor(AMMOEmitter::StaticClass(), NAME_None));
		 if (!ProActor)
		 {
			 return;
		 }

		 FLOAT TRot = Seed + (2 * PI) / NumberToSpawn * i;

		 FVector BC1(0, 0, 0);
		 FVector BC2(0, 0, 0);
		 EmitterData->GetBezierControlPoint(BC1, BC2);

		 FLOAT len = BC1.Size2D();
		 BC1.X = len * appSin(TRot);
		 BC1.Y = len * appCos(TRot);

		 ProActor->OverwriteBezierControl.v1.X = len * appSin(TRot);
		 ProActor->OverwriteBezierControl.v1.Y = len * appCos(TRot);
		 ProActor->OverwriteBezierControl.v1.Z = BC1.Z;

		 ProActor->OverwriteBezierControl.v2 = BC2;

		 FLOAT RadiusScale = EmitterData->Scale;

		 UParticleSystem* UseParticleSystem = NULL;
		 if (EmitterData->RandParticleSystemTemplates.Num() > 0)
		 {
			 INT idx = RandHelper(EmitterData->RandParticleSystemTemplates.Num());
			 UseParticleSystem = EmitterData->RandParticleSystemTemplates(idx);
		 }

		 if (!UseParticleSystem)
		 {
			 UseParticleSystem = EmitterData->ParticleSystemTemplate;
		 }

		 if(UseParticleSystem)
		 {				
			 ProActor->SetTemplate(UseParticleSystem, TRUE);
			 if(ProActor->ParticleSystemComponent)
			 {
				 ProActor->ParticleSystemComponent->Scale = RadiusScale;
				 ProActor->ParticleSystemComponent->BeginDeferredUpdateTransform();
			 }
		 }

		 EmitterData->EmitterType = EMIT_FlyTo;
		 EmitterData->ProjectileTargetType = EPTT_Actor;

		 ProActor->EmitterData = EmitterData;

		 ProActor->SpawnMatrix = EmitterData->GetSocketMatrix(BaseActor, BaseActor->Location, FALSE);

		 ProActor->BaseActorMatrix = BaseActor->LocalToWorld();

		 ProActor->OverwriteEmitterType = EmitterData->EmitterType;
		 ProActor->OverwriteTargetType = EmitterData->ProjectileTargetType;

		 FVector DstPoint;

		 ProActor->SetLocation(ProActor->SpawnMatrix.GetOrigin());
		 ProActor->SetRotation(ProActor->SpawnMatrix.Rotator());

		 // set light data
		 if(EmitterData->MyPointLightComponent)
		 {
			 // Automatically turn off dynamic lights when the overall effect level is below 3; 0 is custom
			 if (GSystemSettings.AutoFPSLevel > M_SCALE_DOWN_ABILITY_POINT_LIGHT)
			 {
				 UObject* InOuter = UObject::GetTransientPackage();
				 ProActor->MyPointLightComponent = Cast<UPointLightComponent>(UObject::StaticConstructObject(EmitterData->MyPointLightComponent->StaticClass(),
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


		 ProActor->BeginPlay(BaseActor,NULL, Target, Target->Location, 1.0);

		 INT ProjecitleId = EffectProcessor::GetSingleton().AddClientProjectile(ProActor);


		 // set liftspan

		 EmitterData->StopType = ACBT_StopWhenConstDurationPass;
		 if (EmitterData->TrackTimeType == ETTT_ConstSpeed)
		 {
			 ProActor->Duration = (Target->Location - BaseActor->Location).Size() / EmitterData->Speed;
		 }
		 else
		 {
			 ProActor->Duration = EmitterData->FlyTime;
		 }
		 

		 //// debug code
		 //// draw line track
		 //if (EmitterData && EmitterData->EmitterType == EMIT_FlyTo)
		 //{
			// static UBOOL bDrawLine = TRUE;

			// FVector DstPoint = Target->Location;

			// if (bDrawLine)
			// {
			//	 Target->FlushPersistentDebugLines();
			//	 if (EmitterData->TrackLineType == ETLT_Line)
			//	 {
			//		 Target->DrawDebugLine(ProActor->SpawnMatrix.GetOrigin(), DstPoint, 255, 0, 0, TRUE);
			//	 }
			//	 else if(EmitterData->TrackLineType == ETLT_Bezier)
			//	 {
			//		 FVector P0 = FVector(0, 0, 0);
			//		 FVector P1 = ProActor->BaseActorMatrix.InverseTransformFVector(DstPoint);
			//		 FVector T0 = EmitterData->BezierControlPoint1; // SpawnMatrix.TransformFVector(EmitterData->BezierControlPoint1);
			//		 FVector T1 = P1 + EmitterData->BezierControlPoint2;
			//		 for (FLOAT i = 0.f; i <= 1.0f; i += 0.001)
			//		 {
			//			 FVector NewPoint = CubicInterp<FVector, FLOAT>(P0, T0, P1, T1, i);
			//			 NewPoint = ProActor->BaseActorMatrix.TransformFVector(NewPoint);
			//			 Target->DrawDebugPoint(NewPoint, 1, FLinearColor(1.0, 0.f, 0.f), TRUE);
			//		 }
			//	 }

			// }
		 //}

	 }
}

void UAbilityPreviewResources::RebulidMesh(USkeletalMeshComponent* BaseSMC, USkeletalMeshComponent* MergedComponent)
{
	if (!BaseSMC || !MergedComponent)
	{
		return;
	}

	if( Attacker->UseMergedMesh )
		return;

	BaseComponent = BaseSMC;
	AttackerMergedComp = MergedComponent;

	MergedComponent->SetSkeletalMesh(NULL);

	if (AttackerMergedMesh)
	{
		AttackerMergedMesh->MarkPendingKill();
	}

	//local BodyPart bp;
	AttackerMergedMesh = Cast<UMMOSkeletalMeshMerge>(StaticConstructObject(UMMOSkeletalMeshMerge::StaticClass()));

	// merge texture size
	AttackerMergedMesh->TargetTexSizeX = 512;
	AttackerMergedMesh->TargetTexSizeY = 256;
	// face region
	FBodyPart part(EC_EventParm);
	part.Mesh = NULL;
	part.OrigX = 256;
	part.OrigY = 0;
	part.SizeX = 128;
	part.SizeY = 128;
	AttackerMergedMesh->Parts.AddItem(part);

	// chest region
	part.Mesh = NULL;
	part.OrigX = 0;
	part.OrigY = 0;
	part.SizeX = 256;
	part.SizeY = 256;
	AttackerMergedMesh->Parts.AddItem(part);
	// hands region
	part.Mesh = NULL;
	part.OrigX = 256;
	part.OrigY = 128;
	part.SizeX = 128;
	part.SizeY = 128;
	AttackerMergedMesh->Parts.AddItem(part);
	// thigh region
	part.Mesh = NULL;
	part.OrigX = 384;
	part.OrigY = 0;
	part.SizeX = 128;
	part.SizeY = 128;
	AttackerMergedMesh->Parts.AddItem(part);
	// feet region
	part.Mesh = NULL;
	part.OrigX = 384;
	part.OrigY = 128;
	part.SizeX = 128;
	part.SizeY = 128;
	AttackerMergedMesh->Parts.AddItem(part);
	// skirt region
	part.Mesh = NULL;
	part.OrigX = 128;
	part.OrigY = 128;
	part.SizeX = 128;
	part.SizeY = 128;
	AttackerMergedMesh->Parts.AddItem(part);

	bIsPreparing = false;
	bIsMerging = false;


	typedef TArray< UEquipMesh* >	EquipMeshArray;

	EquipMeshArray meshArray[6];

	TArray<UEquipMesh*> &DefaultMeshList = Attacker->DefaultMeshListTemplate;

	for( INT i = 0; i < DefaultMeshList.Num(); ++i )
	{
		if( DefaultMeshList(i) == NULL || DefaultMeshList(i)->MeshType >= EBMT_NoMerge )
			continue;
		if( meshArray[DefaultMeshList(i)->MeshType].Num() == 0 )
			meshArray[DefaultMeshList(i)->MeshType].AddItem( DefaultMeshList(i) );
		else if( meshArray[DefaultMeshList(i)->MeshType](0)->SkeletalMesh == NULL )
		{ // in case the item has no mesh data, use default mesh
			debugf( TEXT( "[MMO Error] No skeletal mesh set in equip %s." ), 
				*meshArray[DefaultMeshList(i)->MeshType](0)->GetName() );
			meshArray[DefaultMeshList(i)->MeshType](0) = DefaultMeshList(i);
		}
	}

	SkinTextures.Remove( 0, SkinTextures.Num() );
	SkinTextures.AddZeroed( EBMT_Waist );
	SkinTexturesBase.Remove( 0, SkinTexturesBase.Num() );
	SkinTexturesBase.AddZeroed( EBMT_Waist );

	for(INT i = 0; i < DefaultMeshList.Num(); ++i)
	{
		UEquipMesh* tEquipMesh = DefaultMeshList(i);
		//`Log( "[MMO Debug] Default mesh:"@tEquipMesh );
		if( tEquipMesh != 0 && tEquipMesh->MeshType < EBMT_Waist )
		{
			if( DefaultMeshList(i)->UsedColorTextureID >= 0 &&
				DefaultMeshList(i)->ColorTextures.Num() > DefaultMeshList(i)->UsedColorTextureID )
			{
				SkinTexturesBase(DefaultMeshList(i)->MeshType) =
					DefaultMeshList(i)->ColorTextures( DefaultMeshList(i)->UsedColorTextureID );
			}
			else
			{
				SkinTexturesBase(DefaultMeshList(i)->MeshType) = NULL;
				debugf( TEXT("Failed to get skin base texture.") );
			}
			if( DefaultMeshList(i)->ColorTextures.Num() > /*CurrentSkinColorIdx*/0 )
			{
				SkinTextures(DefaultMeshList(i)->MeshType) =
					DefaultMeshList(i)->ColorTextures( /*CurrentSkinColorIdx*/0 );
			}
			else
			{
				SkinTextures(DefaultMeshList(i)->MeshType) = NULL;
				debugf( TEXT("Skin color idx out of bound of the texture array.") );
			}
		}
	}

	for( INT i = 0; i < 6; ++i )
	{
		AttackerMergedMesh->Parts(i).Mesh = NULL;
		AttackerMergedMesh->Parts(i).Textures.Empty(0);

		if( meshArray[i].Num() < 1 )
		{	// error
			//continue;
			if( i == 5 )
			{	// may have no skirt
				continue;
			}
			return;
		}

		USkeletalMesh* pMesh = meshArray[i](meshArray[i].Num()-1)->SkeletalMesh;
		AttackerMergedMesh->Parts(i).Mesh = pMesh;
		for( INT j = 0; j < meshArray[i].Num(); ++j )
		{
			// Belt has no skin texture so drop the i==5 case
			if( j == 0 && i != 5 && meshArray[i](j)->bNeedSkinTexture )
			{
				AttackerMergedMesh->Parts(i).Textures.AddItem( SkinTextures(i) );
				AttackerMergedMesh->Parts(i).Textures_Base.AddItem( SkinTexturesBase(i) );
			}

			if( meshArray[i](j)->RegionTexture )
			{
				AttackerMergedMesh->Parts(i).Textures.AddItem( meshArray[i](j)->RegionTexture );
				AttackerMergedMesh->Parts(i).Textures_Base.AddItem( meshArray[i](j)->RegionTexture );
			}
			else if( j != 0 )
			{
				return;
			}
		}
	}


	UBOOL bRet = AttackerMergedMesh->PrepareTextures();

	if (!bRet)
	{
		bIsPreparing = FALSE;
		bIsMerging = FALSE;
		return;
	}

	bIsPreparing = TRUE;
}
void UAbilityPreviewResources::Tick(FLOAT Delta)
{
	if (bIsPreparing)
	{
		AttackerMergedMesh->UpdateState();
		if (AttackerMergedMesh->LastMergeCancelled() && AttackerMergedMesh->IsSourceTexturesFullyLoaded())
		{
			AttackerMergedMesh->DoMerge();
			bIsPreparing = FALSE;
			bIsMerging = TRUE;
		}
	}
	else if (bIsMerging)
	{
		AttackerMergedMesh->UpdateState();
		if (AttackerMergedMesh->CheckFinish())
		{
			PostRebuildMesh();
		}
	}
}
void UAbilityPreviewResources::PostRebuildMesh()
{
	AttackerMergedMesh->PostMerge();
	AttackerMergedComp->SetSkeletalMesh(AttackerMergedMesh);
	AttackerMergedComp->SetParentAnimComponent(BaseComponent);
	AttackerMergedComp->SetLightingChannels(BaseComponent->LightingChannels);
	AttackerMergedComp->SetLightEnvironment(BaseComponent->LightEnvironment);
	bIsMerging = FALSE;
}


BaseComponent* UAbilityMeshChangeComponent::CreateCplusplusNode()
{
	return new MeshChangeComponent;
}

BaseComponent* UPositionTransformComponent::CreateCplusplusNode()
{
	return new PositionTransformComponent;
}

BaseComponent* UAbilityBackgroundMusicComponent::CreateCplusplusNode()
{
	return new BackgroundMusicComponent;
}
