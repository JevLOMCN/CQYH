#include "stdafx.h"

FLOAT UAnimNodeBlendTwoSequences::PlayAnimation(
	FName animName,
	INT Key,
	FLOAT rate, 
	FLOAT blendInTime,
	FLOAT blendOutTime, 
	UBOOL loop, 
	BYTE rba_x,
	BYTE rba_y,
	BYTE rba_z,
	UBOOL noAnimEnd,
	float Position,
	BYTE InAnimLevel)
{
	if (Children.Num() < 2 || ActiveChildIdx < -1 || ActiveChildIdx > 1)
		return 0.0f;

	UAnimNodeSequence* child = 0;
	UAnimNodeSequence* child2 = 0;
	
	FLOAT length = 0.0f;

	ActiveAnimKey = Key;
	AnimLevel = InAnimLevel;

	if( ActiveChildIdx == 0 )
	{
		child = Cast<UAnimNodeSequence>(Children(1).Anim);
		if( child != NULL )
		{
			child->SetAnim( animName );
			//child->bCauseActorAnimPlay = true;
			child->bCauseActorAnimEnd = !noAnimEnd;

			child2 = Cast<UAnimNodeSequence>(Children(0).Anim);
			if (child2 && child2->bPlaying)
			{
				child2->bCauseActorAnimEnd = FALSE;
			}

			//`Log( "Play Anim on node 2: " @animName );

			child->SetRootBoneAxisOption( rba_x, rba_y, rba_z );

			//child->bTickDuringPausedAnims = true;

			FLOAT NewTime = 0.f;
			if (child->AnimSeq)
			{
				NewTime = /*child->AnimSeq->SequenceLength **/ Position;
			}
			child->PlayAnim( loop, rate, NewTime );

			length = child->GetTimeLeft();
			if( length > 0.f )
			{
				SetBlendTarget( 1.f, blendInTime );
				ActiveChildIdx = 1;
			}
		}
	}
	else// if( ActiveChildIdx == 1 )
	{
		child = Cast<UAnimNodeSequence>(Children(0).Anim);
		if( child != NULL )
		{
			child->SetAnim( animName );
			child->Rate = rate;
			//child->bCauseActorAnimPlay = true;
			child->bCauseActorAnimEnd = !noAnimEnd;

			child2 = Cast<UAnimNodeSequence>(Children(1).Anim);
			if (child2 && child2->bPlaying)
			{
				child2->bCauseActorAnimEnd = FALSE;
			}

			//`Log( "Play Anim on node 1: " @animName );

			child->SetRootBoneAxisOption( rba_x, rba_y, rba_z );

			//child->bTickDuringPausedAnims = true;
			
			FLOAT NewTime = 0.f;
			if (child->AnimSeq)
			{
				NewTime = /*child->AnimSeq->SequenceLength **/ Position;
			}
			child->PlayAnim( loop, rate, NewTime );

			length = child->GetTimeLeft();
			if( length > 0.f )
			{
				SetBlendTarget( 0, blendInTime );
				ActiveChildIdx = 0;
			}
		}
	}
	return length;
}

UBOOL UAnimNodeBlendTwoSequences::StopAnimationByName(FName InAnim)
{
	for (int i = 0; i < 2 && i < Children.Num(); ++i)
	{
		UAnimNodeSequence* node = Cast<UAnimNodeSequence>(Children(i).Anim);
		if (node && node->AnimSeqName == InAnim && node->bPlaying)
		{
			node->StopAnim();
			return TRUE;
		}
	}
	return FALSE;
}

UBOOL UAnimNodeBlendTwoSequences::StopAnimationByKey(INT InKey)
{
	UBOOL bRet = FALSE;
	if (InKey == ActiveAnimKey)
	{
		for (int i = 0; i < 2 && i < Children.Num(); ++i)
		{
			UAnimNodeSequence* node = Cast<UAnimNodeSequence>(Children(i).Anim);
			if (node && node->bPlaying)
			{
				node->StopAnim();
				bRet = TRUE;
			}
		}

		ActiveAnimKey = 0;
	}

	return bRet;
}

void UAnimNodeBlendTwoSequences::ReplayAnim()
{
	for (int i = 0; i < 2 && i < Children.Num(); ++i)
	{
		UAnimNodeSequence* node = Cast<UAnimNodeSequence>(Children(i).Anim);
		if (node&&node->bPlaying)
		{		
			node->ReplayAnim();
		}
	}
}

UBOOL UAnimNodeBlendTwoSequences::IsPlayingAnimationByName(FName InAnim)
{
	if (!Children.IsValidIndex(ActiveChildIdx))
	{
		return FALSE;
	}
	UAnimNodeSequence* node = Cast<UAnimNodeSequence>(Children(ActiveChildIdx).Anim);
	if (node && node->AnimSeqName == InAnim && node->bPlaying)
	{
		return TRUE;
	}

	return FALSE;
}

UBOOL UAnimNodeBlendTwoSequences::IsPlayingAnimationByKey(INT InKey)
{
	if (!Children.IsValidIndex(ActiveChildIdx))
	{
		return FALSE;
	}
	if (ActiveAnimKey == InKey)
	{
		UAnimNodeSequence* node = Cast<UAnimNodeSequence>(Children(ActiveChildIdx).Anim);
		if (node && node->bPlaying)
		{
			return TRUE;
		}
	}
	return FALSE;
}
UBOOL UAnimNodeBlendTwoSequences::IsPlayAnyAnimation()
{
	if (!Children.IsValidIndex(ActiveChildIdx))
	{
		return FALSE;
	}

	UBOOL bRet = FALSE;
	UAnimNodeSequence* node = Cast<UAnimNodeSequence>(Children(ActiveChildIdx).Anim);
	if (node && node->bPlaying)
	{
		bRet = TRUE;
	}
	return bRet;
}

void UAnimNodeBlendTwoSequences::SetAnimRate(INT InKey,FLOAT InRate)
{
	if (ActiveAnimKey != InKey)
	{
		return;
	}

	for (INT i = 0; i < Children.Num(); ++i)
	{
		UAnimNodeSequence* node = Cast<UAnimNodeSequence>(Children(i).Anim);
		if (node)
		{
			node->Rate = InRate;
		}
	}
}

UBOOL UAnimNodeBlendTwoSequences::GetCurrentAnimInfo(INT& OutActiveAnimKey,BYTE& OutAnimLevel,FLOAT& OutPercent,FName& OutAnimName)
{
	OutActiveAnimKey = 0;
	OutAnimLevel = 0;
	OutPercent = 0.0f;
	OutAnimName = NAME_None;

	if (!Children.IsValidIndex(ActiveChildIdx))
	{
		return FALSE;
	}

	UBOOL bRet = FALSE;
	UAnimNodeSequence* node = Cast<UAnimNodeSequence>(Children(ActiveChildIdx).Anim);
	if (node && node->bPlaying)
	{
		OutActiveAnimKey = ActiveAnimKey;
		OutAnimLevel = AnimLevel;
		OutAnimName = node->AnimSeqName;
		FLOAT maxLen = node->GetAnimPlaybackLength();
		if (maxLen < 0.001)
		{
			OutPercent = 1.f;
		}
		else
		{
			OutPercent = node->CurrentTime / node->GetAnimPlaybackLength();
		}
		return TRUE;
	}

	return FALSE;
}
