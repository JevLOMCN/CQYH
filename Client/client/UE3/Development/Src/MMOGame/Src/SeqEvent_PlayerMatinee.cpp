#include "..\stdafx.h"
#include "MMOFacade.h"


IMPLEMENT_CLASS(USeqEvent_PlayerMatineeStop);


void USeqEvent_PlayerMatineeStop::Activated()
{
    if (!GWorld || !GWorld->CurrentLevel)
        return;

	TArray<USeqAct_Interp*> objList = MMO::FindObjectInKisemetByType<USeqAct_Interp>();

	for (INT i = 0; i < objList.Num(); ++i)
	{
        USeqAct_Interp* seqAct = objList(i);

        if (!seqAct->bIsPlaying)
            continue;

        if (!seqAct->bIsSkippable)
            continue;

        const FLOAT RightBeforeEndTime = 0.1f;  // remaining time
        const FLOAT InitialNoSkipTime = 0;      // must play at least
        if ((seqAct->Position < seqAct->InterpData->InterpLength - RightBeforeEndTime) && 
            (seqAct->Position >= InitialNoSkipTime))
        {
            seqAct->SetPosition(seqAct->InterpData->InterpLength - RightBeforeEndTime, TRUE);

			seqAct->bCanceled = TRUE;//fangruyi
        }
    }
    Super::Activated();
}