#include "stdafx.h"
#include "MMOFacade.h"


void AArmorActor::UpdatePosition()
{
	if (BaseActor == NULL)
	{
		return;
	}

	USkeletalMeshComponent* BaseMesh = NULL;
	if (BaseActor->IsA(AMMOActor::StaticClass()))
	{
		BaseMesh = ((AMMOActor*)BaseActor)->GetActiveBaseMesh();
	}
	else
	{
		BaseMesh = BaseActor->Mesh;
	}

	if (!BaseMesh)
	{
		return;
	}

	SetBase(BaseActor, FVector(0, 0, 1), 1, BaseMesh, CurrentAttachedSocketName);
}

void AArmorActor::SwitchAttachSocket(INT InIndex)
{
	if (!BaseActor)
	{
		return;
	}

	FName ToUseSocketName = Template->ArmorTemplate->AttachSocket0;
	if (InIndex == 1)
	{
		ToUseSocketName = Template->ArmorTemplate->AttachSocket1;
	}

	if(ToUseSocketName == CurrentAttachedSocketName)
	{
		return;
	}

	USkeletalMeshComponent* BaseMesh = BaseActor->Mesh;
	if (BaseActor->IsA(AMMOActor::StaticClass()))
	{
		BaseMesh = ((AMMOActor*)BaseActor)->GetActiveBaseMesh();
	}

	if (BaseMesh == NULL || BaseMesh->SkeletalMesh == NULL)
	{
		return;
	}

	if (BaseMesh->SkeletalMesh->FindSocket(ToUseSocketName) == NULL)
	{
		return;
	}

	CurrentAttachedSocketName = ToUseSocketName;

	UpdatePosition();
}


void AArmorActor::OnTakeOn(class APawn* InActor,class UEquipMesh* InData)
{
	check(BaseActor == NULL);
	check(InActor);
	check(InData);

	BaseActor = InActor;
	Template = InData;

	// #22374 In game state the weapon scale does not shrink along with the avatar scale
	float actorScale = 1.0f;
	if (BaseActor)
	{
		if (BaseActor->IsA(AMMOPlayer::StaticClass()))
		{
			float PlayerGlobalScale = GWorld->GetWorldInfo()->PlayerGlobalScale;
			AMMOPlayer* player = Cast<AMMOPlayer>(BaseActor);
			if (player && player->UseingAvatar)
			{
				actorScale = player->UseingAvatar->MeshScale * player->UseingAvatar->AvatarDrawScale * PlayerGlobalScale;
			}
		} 
		else if (BaseActor->IsA(ANPC::StaticClass()))
		{
			float NPCGlobalScale = GWorld->GetWorldInfo()->NpcGlobalScale;
			ANPC* npc = Cast<ANPC>(BaseActor);
			if (npc && npc->UseingAvatar)
			{
				actorScale = npc->UseingAvatar->MeshScale * npc->UseingAvatar->AvatarDrawScale * NPCGlobalScale;
			}
		}
	}

	this->SetDrawScale(actorScale * Template->ArmorTemplate->Scale);

	//UpdatePosition();

	SwitchAttachSocket(0);

	UPrimitiveComponent* pc = NULL;
	if (Template->ArmorTemplate->Type == EAART_StaticMesh)
	{
		UStaticMeshComponent* smc = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), this);
		check(smc);
		pc = smc;
		smc->SetStaticMesh(Template->ArmorTemplate->StaticMeshTemplate);

		MainMesh = smc;

		if (Template->ArmorTemplate->StaticMeshShellTemplate)
		{
			smc = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), this);
			check(smc);
			smc->SetStaticMesh(Template->ArmorTemplate->StaticMeshShellTemplate);
			AttachComponent(smc);
			SubMesh = smc;
		}
	}
	else if (Template->ArmorTemplate->Type == EAART_SkeletalMesh)
	{
		USkeletalMeshComponent* smc = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass(), this);
		check(smc);
		pc = smc;
		MainMesh = smc;
		smc->SetSkeletalMesh(Template->ArmorTemplate->SkeletalMeshTemplate);
		smc->SetAnimTreeTemplate(AnimTree);
		smc->AnimSets.AddItem(Template->ArmorTemplate->WeaponAnimSet);

		AnimationController = ConstructObject<UMMOActorAT>(UMMOActorAT::StaticClass(), this);
		check(AnimationController);
		AnimationController->InitAnim(smc, NULL);

		if (Template->ArmorTemplate->OverrideIdleAnim != NAME_None)
		{
			AnimationController->SetIdleAnim(Template->ArmorTemplate->OverrideIdleAnim);
		}
		else
		{
			AnimationController->SetIdleAnim(((AMMOActor*)BaseActor)->GetIdleAnim());
		}



		//WeaponActionLoopBlendNode = Cast<UAnimNodeBlend>(smc->FindAnimNode(TEXT("WeaponActionLoopBlendNode")));
		//ActionNode = Cast<UAnimNodeSequence>(smc->FindAnimNode(TEXT("Action")));
		//LoopNode = Cast<UAnimNodeSequence>(smc->FindAnimNode(TEXT("Idle")));

		//if (WeaponActionLoopBlendNode && ActionNode &&  LoopNode && BaseActor->IsA(AMMOActor::StaticClass()))
		//{
		//	WeaponActionLoopBlendNode->SetBlendTarget(1.0, 0.f);
		//	if (Template->ArmorTemplate->OverrideIdleAnim != NAME_None)
		//	{
		//		LoopNode->SetAnim(Template->ArmorTemplate->OverrideIdleAnim);
		//	}
		//	else
		//	{
		//		LoopNode->SetAnim(((AMMOActor*)BaseActor)->GetIdleAnim());
		//	}
		//	
		//	LoopNode->PlayAnim(TRUE, 1.0, 0.f);
		//}
	}
	else if (Template->ArmorTemplate->Type == EAART_Particle)
	{
		UParticleSystemComponent* psc = ConstructObject<UParticleSystemComponent>(UParticleSystemComponent::StaticClass(), this);
		check(psc);
		pc = psc;
		psc->SetTemplate(Template->ArmorTemplate->ParticleSystemTemplate);
	}

	if (MainMesh)
	{
		MainMesh->RebulidMeshMaterialList();
	}

	AttachComponent(pc);

	pc->bAcceptsStaticDecals = FALSE;
	pc->bAcceptsDynamicDecals = FALSE;

	if (BaseActor->Mesh)
	{
		if (BaseActor->Mesh->LightEnvironment)
		{
			if (pc->IsA(UParticleSystemComponent::StaticClass()))
			{
				UParticleLightEnvironmentComponent* ParticleDLE = Cast<UParticleLightEnvironmentComponent>(BaseActor->Mesh->LightEnvironment);
				if (ParticleDLE)
				{
					pc->SetLightEnvironment(BaseActor->Mesh->LightEnvironment);
				}
			}
			else
			{
				pc->SetLightEnvironment(BaseActor->Mesh->LightEnvironment);
			}
		}

		pc->SetLightingChannels(BaseActor->Mesh->LightingChannels);
		
		

		if (SubMesh)
		{
			SubMesh->SetLightEnvironment(BaseActor->Mesh->LightEnvironment);
			SubMesh->SetLightingChannels(BaseActor->Mesh->LightingChannels);
		}
	}


	if (Template->ArmorTemplate->MeshSurfaceParticle && MainMesh)
	{
		UParticleSystemComponent* psc = ConstructObject<UParticleSystemComponent>(UParticleSystemComponent::StaticClass(), this);
		psc->SetTemplate(Template->ArmorTemplate->MeshSurfaceParticle);
		UMeshComponent* ToUseMesh = SubMesh ? SubMesh : MainMesh;
		psc->SetMeshComponentParameter(FName(TEXT("VertSurfaceActor")), ToUseMesh);
		AttachComponent(psc);
	}

	for (INT i = 0; i < Template->ArmorTemplate->AttachList.Num(); ++i)
	{
		FMatrix BaseMat = LocalToWorld();
		BaseMat *= FRotationTranslationMatrix(Template->ArmorTemplate->AttachList(i).RelativeRotation, Template->ArmorTemplate->AttachList(i).RelativeLocation);
		AEmitterAttachment* NewEmitter = Cast<AEmitterAttachment>(GWorld->SpawnActor(AEmitterAttachment::StaticClass(), NAME_None, BaseMat.GetOrigin(), BaseMat.Rotator()));
		if (!NewEmitter)
		{
			warnf(L"Spawn armor attachment faild");
			return;
		}

		if (Template->ArmorTemplate->AttachList(i).Particles)
		{
			NewEmitter->SetTemplate(Template->ArmorTemplate->AttachList(i).Particles);
		}

		NewEmitter->SetBase(this, FVector(0, 0, 1), 1, NULL, NAME_None);
	}
}

void AArmorActor::OnTakeOff()
{
	DestroyAttachment();

	if(!HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		GWorld->DestroyActor(this);
	}
}

void AArmorActor::DestroyAttachment()
{
	for (INT i = 0; i < this->Attached.Num(); ++i)
	{
		if (Attached(i) && !Attached(i)->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			GWorld->DestroyActor(Attached(i));
		}
	}
}

void AArmorActor::SetVisible(UBOOL bVisible)
{
	SetHidden(!bVisible);
	for (INT i = 0; i < this->Attached.Num(); ++i)
	{
		if (Attached(i))
		{
			Attached(i)->SetHidden(!bVisible);
		}
	}
}

FLOAT AArmorActor::PlayAnimation(FName Anim,INT InKey,FLOAT Rate,UBOOL bLoop,BYTE RootBoneAxisX,BYTE RootBoneAxisY,BYTE RootBoneAxisZ,BYTE BlendType,FLOAT Position,FLOAT InBlendIn,FLOAT InBlendOut,BYTE InAnimLevel)
{
	if (AnimationController)
	{
		return AnimationController->PlayAnimation(Anim, InKey, Rate, bLoop, RootBoneAxisX, RootBoneAxisY, RootBoneAxisZ, BlendType, Position, InBlendIn, InBlendOut, InAnimLevel);
	}

	return 0.f;
}

void AArmorActor::PlayAnim(FName Anim,UBOOL bLoop,FLOAT InRate,FLOAT InStartTime)
{
	if (AnimationController && WeaponActionLoopBlendNode && ActionNode && LoopNode)
	{
		USkeletalMeshComponent* sm = AnimationController->Mesh;
		if (sm == NULL || sm->FindAnimSequence(Anim) == FALSE)
		{
			warnf(TEXT("Play Weapon Anim, but the mesh does not have the anim : %s"), *(Anim.ToString()));
			return;
		}

		WeaponActionLoopBlendNode->SetBlendTarget(0.f, 0.f);
		ActionNode->SetAnim(Anim);
		ActionNode->PlayAnim(bLoop, InRate, InStartTime);
	}
}
void AArmorActor::PlayAnimByKey(FName Anim,UBOOL bLoop,FLOAT InRate,FLOAT InStartTime,INT InKey)
{
	if (AnimationController && WeaponActionLoopBlendNode && ActionNode && LoopNode)
	{
		USkeletalMeshComponent* sm = AnimationController->Mesh;
		if (sm == NULL || sm->FindAnimSequence(Anim) == FALSE)
		{
			warnf(TEXT("Play Weapon Anim, but the mesh does not have the anim : %s"), *(Anim.ToString()));
			return;
		}

		WeaponActionLoopBlendNode->SetBlendTarget(0.f, 0.f);
		ActionNode->SetAnim(Anim);
		ActionNode->PlayAnim(bLoop, InRate, InStartTime);
		CurrentAnimKey = InKey;
	}

}
void AArmorActor::StopAnimByKey(INT InKey)
{
	if (WeaponActionLoopBlendNode && ActionNode && LoopNode && CurrentAnimKey == InKey)
	{
		ActionNode->StopAnim();
		WeaponActionLoopBlendNode->SetBlendTarget(1.f, 0.f);
		CurrentAnimKey = 0;
	}
}
void AArmorActor::OnAnimEnd(class UAnimNodeSequence* SeqNode,FLOAT PlayedTime,FLOAT ExcessTime)
{
	if (WeaponActionLoopBlendNode && ActionNode && LoopNode)
	{
		if (SeqNode == ActionNode)
		{
			WeaponActionLoopBlendNode->SetBlendTarget(1.f, 0.1f);
			CurrentAnimKey = 0;
		}
	}

}
void AArmorActor::RefreshAnim()
{
	if (WeaponActionLoopBlendNode && ActionNode && LoopNode && BaseActor && BaseActor->IsA(AMMOActor::StaticClass()))
	{
		WeaponActionLoopBlendNode->SetBlendTarget(1.0, 0.1f);
		LoopNode->SetAnim(((AMMOActor*)BaseActor)->GetIdleAnim());
		LoopNode->PlayAnim(TRUE, 1.0, 0.f);
	}
}

void AArmorActor::SetIdleAnim(FName NewIdleLoopAnim)
{
	if (LoopNode)
	{
		FName ToUseAnim = NewIdleLoopAnim;
		if (Template->ArmorTemplate->OverrideIdleAnim != NAME_None)
		{
			ToUseAnim = Template->ArmorTemplate->OverrideIdleAnim;
		}
		LoopNode->SetAnim(ToUseAnim);
		LoopNode->PlayAnim(TRUE, 1.0, 0.f);
	}
}

void AArmorActor::SetMoveAnim(FName NewMoveLoopAnim)
{
	if (LoopNode)
	{
		FName ToUseAnim = NewMoveLoopAnim;
		if (Template->ArmorTemplate->OverrideIdleAnim != NAME_None)
		{
			ToUseAnim = Template->ArmorTemplate->OverrideIdleAnim;
		}
		LoopNode->SetAnim(ToUseAnim);
		LoopNode->PlayAnim(TRUE, 1.0, 0.f);
	}
}
