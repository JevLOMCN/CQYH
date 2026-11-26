#include "..\stdafx.h"
#include "..\MMOFacade.h"
#include "net/message_handler.h"
#include "attribute_def.h"
#include "EnginePrivate.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"

IMPLEMENT_CLASS(UMMOActorAT)

void UMMOActorAT::BeginDestroy()
{
	Super::BeginDestroy();

	IdleLoopBlend = 0;
	MoveLoopBlend = 0;
	ActionBlend = 0;
	MoveTypeBlend = 0;
	MoveLoopBlendEx = 0;
	ActionAnimRateNode = 0;
	LoopAnimRateNode = 0;
	ActionLoopBlend = 0;
	IdleLoop1 = 0;
	IdleLoop2 = 0;
	MoveLoop1 = 0;
	MoveLoop2 = 0;
	Action1 = 0;
	Action2 = 0;
	MoveLoop1Ex = 0;
	MoveLoop2Ex = 0;
	Mesh = 0;
	actor = 0;
}

void UMMOActorAT::InitAnim(class USkeletalMeshComponent* InMesh,class AMMOActor* InActor)
{
	Mesh			= InMesh;
	if(Mesh==NULL)return;
	actor			= InActor;

	IdleLoopBlend = Cast<UAnimNodeBlendTwoSequences>(Mesh->FindAnimNode(TEXT("IdleLoopBlend")));
	MoveLoopBlend = Cast<UAnimNodeBlendTwoSequences>(Mesh->FindAnimNode(TEXT("MoveLoopBlend")));
	ActionBlend = Cast<UAnimNodeBlendTwoSequences>(Mesh->FindAnimNode(TEXT("ActionBlend")));

	MoveTypeBlend       = Cast<UAnimNodeBlendTwoSequences>(Mesh->FindAnimNode(TEXT("MoveTypeBlend")));
	MoveLoopBlendEx = Cast<UAnimNodeBlendTwoSequences>(Mesh->FindAnimNode(TEXT("MoveLoopBlendEx")));

	ActionAnimRateNode = Cast<UAnimNodeScalePlayRate>(Mesh->FindAnimNode(TEXT("ActionAnimRateNode")));
	LoopAnimRateNode = Cast<UAnimNodeScalePlayRate>(Mesh->FindAnimNode(TEXT("LoopAnimRateNode")));
	ActionLoopBlend = Cast<UAnimNodeBlendPerBone>(Mesh->FindAnimNode(TEXT("ActionLoopBlend")));
	IdleLoop1 = Cast<UAnimNodeSequence>(Mesh->FindAnimNode(TEXT("IdleLoop1")));
	IdleLoop2 = Cast<UAnimNodeSequence>(Mesh->FindAnimNode(TEXT("IdleLoop2")));
	MoveLoop1 = Cast<UAnimNodeSequence>(Mesh->FindAnimNode(TEXT("MoveLoop1")));
	MoveLoop2 = Cast<UAnimNodeSequence>(Mesh->FindAnimNode(TEXT("MoveLoop2")));
	Action1 = Cast<UAnimNodeSequence>(Mesh->FindAnimNode(TEXT("Action1")));
	Action2 = Cast<UAnimNodeSequence>(Mesh->FindAnimNode(TEXT("Action2")));

	MoveLoop1Ex  = Cast<UAnimNodeSequence>(Mesh->FindAnimNode(TEXT("MoveLoop1Ex")));
	MoveLoop2Ex  = Cast<UAnimNodeSequence>(Mesh->FindAnimNode(TEXT("MoveLoop2Ex")));

	//Mesh->SetLightEnvironment(InActor->Mesh->LightEnvironment);
	//Mesh->SetLightingChannels(InActor->Mesh->LightingChannels);

	if (InActor)
	{
		InActor->RefreshAttachAnim();
	}
	
}

void UMMOActorAT::RefreshMoveAnim()
{
	if(MoveLoopBlend)MoveLoopBlend->ReplayAnim();
}

void UMMOActorAT::SetActionLoopBlendTarget(FLOAT BlendTarget, FLOAT BlendTime)
{
	if(ActionLoopBlend)ActionLoopBlend->SetBlendTarget(BlendTarget,BlendTime);
}

UBOOL UMMOActorAT::GetCurrentAnimInfo(INT& OutActiveAnimKey,BYTE& OutAnimLevel,FLOAT& OutPercent,FName& OutAnimName)
{
	OutActiveAnimKey = 0;
	OutAnimLevel = 0;
	OutPercent = 0.0f;
	OutAnimName = NAME_None;

	if (!ActionLoopBlend || !ActionBlend || ActionLoopBlend->Children.Num() != 2)
	{
		return FALSE;
	}

	if (ActionBlend && ActionLoopBlend->Child2WeightTarget > 0.1 /*ActionLoopBlend->Children(1).Weight >= 0.01*/)
	{
		return ActionBlend->GetCurrentAnimInfo(OutActiveAnimKey, OutAnimLevel, OutPercent, OutAnimName);
	}
	else
	{
		return FALSE;
	}
}


FLOAT UMMOActorAT::PlayAnimation(FName Anim,INT InKey,FLOAT Rate,UBOOL bLoop,BYTE RootBoneAxisX,BYTE RootBoneAxisY,BYTE RootBoneAxisZ,BYTE BlendType,FLOAT Position,FLOAT InBlendIn,FLOAT InBlendOut,BYTE InAnimLevel)
{

	//FLOAT len;
	FName BoneName(TEXT("Root"));

	FMMOAnimGroup* PawnAnimInfo = NULL;
	if (actor)
	{
		PawnAnimInfo = &(actor->PawnAnimInfo);
	}

	if(Anim.GetNameString().Len() == 0)
	{
		return 0.0f;
	}

	if(Mesh == NULL || Mesh->FindAnimSequence(Anim) == NULL)
	{
		return 0.0f;
	}

	if (PawnAnimInfo)
	{
		switch(BlendType)
		{
		case EABT_FullBody:
			BoneName = PawnAnimInfo->RootBoneName;
			break;
		case EABT_UpperBody:
			BoneName = PawnAnimInfo->SpineBoneName;
			break;
		case EABT_Head:
			BoneName = TEXT("Neck");
			break;
		case EABT_LeftFoot:
		case EABT_RightFoot:
		case EABT_Tail:
			BoneName = PawnAnimInfo->RootBoneName;
			break;
		}
	}


	if(actor && BlendType != EABT_FullBody)
	{
		if(actor->Velocity.SizeSquared2D() < 1.f)
		{
			BoneName = PawnAnimInfo->RootBoneName;
		}
	}



	if( ActionBlend != NULL && ActionLoopBlend != NULL )
	{
		FLOAT len = ActionBlend->PlayAnimation(Anim, InKey, 1.0, InBlendIn, InBlendOut, bLoop, RootBoneAxisX, RootBoneAxisY, RootBoneAxisZ, FALSE, Position, InAnimLevel);
		ActionAnimRateNode->ScaleByValue = Rate;
		ActionPlayedLeftTime = len;

		if(len > 0.f)
		{
			if(ActionLoopBlend->BranchStartBoneName.Num() == 0)
			{
				ActionLoopBlend->BranchStartBoneName.AddItem(FName(NAME_None));
			}
			if(ActionLoopBlend->BranchStartBoneName(0) != BoneName)
			{
				ActionLoopBlend->BranchStartBoneName(0) = BoneName;
				ActionLoopBlend->RefreshWeightList();
			}

			ActionLoopBlend->SetBlendTarget(1.0, AnimBlendInTime);
		}
		return len;
	}
	return 0.f;
}



void UMMOActorAT::OnAnimEnd(class UAnimNodeSequence* SeqNode, FLOAT PlayedTime, FLOAT ExcessTime)
{
	if(SeqNode == Action1 || SeqNode == Action2)
	{
		ActionLoopBlend->SetBlendTarget(0.f, AnimBlendInTime);
	}
}

static FLOAT s_BlendInTime  = 0.1f;
static FLOAT s_BlendOutTime = 0.1f;

//------------------------------------------------------------------------
void UMMOActorAT::SetIdleAnim(FName NewIdleLoopAnim)
{	
	if(!NewIdleLoopAnim.IsValid())
	{
		warnf(TEXT("Set a null anim to IdleLoopAnim"));
		return;
	}

	if(IdleLoopBlend == NULL)
	{
		return;
	}

	if(IdleLoopBlend->ActiveChildIdx == 0)
	{
		if(IdleLoop2->AnimSeqName != NewIdleLoopAnim)
		{
			IdleLoop2->SetAnim(NewIdleLoopAnim);
		}
	}
	else if(IdleLoopBlend->ActiveChildIdx == 1)
	{
		if(IdleLoop1->AnimSeqName != NewIdleLoopAnim)
		{
			IdleLoop1->SetAnim(NewIdleLoopAnim);
		}
	}
	else
	{
		//`warn(self$"IdleLoopBlend->ActiveChildIdx == -1");
		IdleLoop1->SetAnim(NewIdleLoopAnim);
		IdleLoop2->SetAnim(NewIdleLoopAnim);
	}

	IdleLoopBlend->PlayAnimation(NewIdleLoopAnim,0, 1.0, s_BlendInTime, s_BlendOutTime, TRUE, RBA_Discard, RBA_Discard, RBA_Discard, TRUE);

}
//------------------------------------------------------------------------
void UMMOActorAT::SetMoveAnim(FName NewMoveLoopAnim)
{
	if(!NewMoveLoopAnim.IsValid())
	{
		warnf(TEXT("Set a null anim to IdleLoopAnim"));
		return;
	}

	if(MoveLoopBlend == NULL)
	{
		return;
	}

	if(MoveLoopBlend->ActiveChildIdx == 0)
	{
		if(MoveLoop2->AnimSeqName != NewMoveLoopAnim)
		{
			MoveLoop2->SetAnim(NewMoveLoopAnim);
		}
	}
	else if(MoveLoopBlend->ActiveChildIdx == 1)
	{
		if(MoveLoop1->AnimSeqName != NewMoveLoopAnim)
		{
			MoveLoop1->SetAnim(NewMoveLoopAnim);
		}
	}
	else
	{
		//`warn(self$"MoveLoopBlend->ActiveChildIdx == -1");
		MoveLoop1->SetAnim(NewMoveLoopAnim);
	}

	MoveLoopBlend->PlayAnimation(NewMoveLoopAnim,0, 1.0, s_BlendInTime, s_BlendOutTime, TRUE, RBA_Discard, RBA_Discard, RBA_Discard,TRUE);

}

FName UMMOActorAT::GetIdleAnim()
{
	if(IdleLoopBlend == NULL)
	{
		return NAME_None;
	}

	if(IdleLoopBlend->ActiveChildIdx == 0)
	{
		return IdleLoop1->AnimSeqName;
	}
	else if(IdleLoopBlend->ActiveChildIdx == 1)
	{
		return IdleLoop2->AnimSeqName;
	}
	else
	{
		return NAME_None;
	}

}


FName UMMOActorAT::GetMoveAnim()
{
	if(MoveLoopBlend == NULL)
	{
		return NAME_None;
	}

	if(MoveLoopBlend->ActiveChildIdx == 0)
	{
		return MoveLoop1->AnimSeqName;
	}
	else if(MoveLoopBlend->ActiveChildIdx == 1)
	{
		return MoveLoop2->AnimSeqName;
	}
	else
	{
		return NAME_None;
	}
}

void UMMOActorAT::SetAnimRateById(INT InId, FLOAT InRate)
{
	if (ActionBlend)
	{
		ActionBlend->SetAnimRate(InId, InRate);
	}
}

void UMMOActorAT::SetActionRate(float InRate)
{
	if(ActionAnimRateNode)
	{
		ActionAnimRateNode->ScaleByValue = InRate;
	}
}

void UMMOActorAT::SetMoveAnimRate(float InRate)
{
	if(LoopAnimRateNode)
	{
		LoopAnimRateNode->ScaleByValue = InRate;
	}
}

void UMMOActorAT::StopAnimation()
{
	if(Action1)Action1->StopAnim();
	if(Action2)Action2->StopAnim();
}

FLOAT UMMOActorAT::GetMoveAnimPosition()
{
	if(MoveLoopBlend == NULL)
	{
		return 0.0f;
	}

	if(MoveLoopBlend->ActiveChildIdx == 0)
	{
		if (MoveLoop1->AnimSeq)
		{
			return MoveLoop1->GetNormalizedPosition();
		}
	}
	else if(MoveLoopBlend->ActiveChildIdx == 1)
	{
		if (MoveLoop2->AnimSeq)
		{
			return MoveLoop2->GetNormalizedPosition();
		}
	}

	return 0.0f;
}

void UMMOActorAT::SetMoveAnimPosition(FLOAT InPos)
{
	if(MoveLoopBlend == NULL)
	{
		return;
	}

	if(MoveLoopBlend->ActiveChildIdx == 0)
	{
		if (MoveLoop1->AnimSeq)
		{
			MoveLoop1->SetPosition(InPos * MoveLoop1->AnimSeq->SequenceLength, FALSE);
		}
	}
	else if(MoveLoopBlend->ActiveChildIdx == 1)
	{
		if (MoveLoop2->AnimSeq)
		{
			MoveLoop2->SetPosition(InPos * MoveLoop2->AnimSeq->SequenceLength, FALSE);
		}
	}
	else
	{
		return;
	}
}


void UMMOActorAT::StopAnimationByName(FName InAnim)
{
	if(Mesh && ActionBlend&&ActionBlend->StopAnimationByName(InAnim))
	{
		BlendToLoops();
	}
}

void UMMOActorAT::StopAnimationByKey(int InKey)
{
	if(Mesh && ActionBlend && ActionBlend->StopAnimationByKey(InKey))
	{
		BlendToLoops();
	}
}



void UMMOActorAT::BlendToIdle()
{
	StopAnimation();
}

void UMMOActorAT::StopLoopAnim(FName InAnim)
{
	
	if(Action1&&Action1->AnimSeqName == InAnim)
	{
		Action1->bLooping = false;
	}

	if(Action2&&Action2->AnimSeqName == InAnim)
	{
		Action2->bLooping = false;
	}

	BlendToLoops();
}

void UMMOActorAT::BlendToLoops()
{
	if (Mesh && ActionLoopBlend)
	{
		ActionLoopBlend->SetBlendTarget(0.f, AnimBlendInTime);
	}
}


UBOOL UMMOActorAT::IsPlayingAnimationByName(FName InAnim)
{
	if(ActionBlend)
		return ActionBlend->IsPlayingAnimationByName(InAnim);
	else
		return FALSE;
}
UBOOL UMMOActorAT::IsPlayingAnimationByKey(INT InKey)
{
	if(ActionBlend)
		return ActionBlend->IsPlayingAnimationByKey(InKey);
	else
		return FALSE;
}
UBOOL UMMOActorAT::IsPlayingAnyAnimation()
{
	if(ActionBlend)
		return ActionBlend->IsPlayAnyAnimation();
	else
		return FALSE;
}