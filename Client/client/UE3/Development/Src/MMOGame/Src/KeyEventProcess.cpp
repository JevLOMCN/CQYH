// fangruyi@snda.com
#include "stdafx.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(UKeyEventProcess)
IMPLEMENT_CLASS(UKeyEventProcessor)
IMPLEMENT_CLASS(UKeyEventProcessDefaultState)
IMPLEMENT_CLASS(UKeyEventProcessDisableState)
IMPLEMENT_CLASS(UKeyEventProcessStateAbilityTargetSelect)


///////////////////////////// UKeyEventProcessor ////////////////////////
void UKeyEventProcessor::Init(class AMMOPlayerController* InController)
{
	check(InController);

	Controller = InController;

	for( TObjectIterator<UClass> It ; It ; ++It )
	{
		UStruct* SuperStruct = It->GetSuperClass();
		if (SuperStruct && SuperStruct == UKeyEventProcess::StaticClass())
		{
			UKeyEventProcess* NewState = ConstructObject<UKeyEventProcess>(Cast<UClass>(*It), this);
			check(NewState);
			NewState->Controller = Controller;
			ProcessStateList.AddItem(NewState);
		}
	}

	GotoKeyProcessState(this->DefaultStateName);
}

UBOOL UKeyEventProcessor::GotoKeyProcessState(const FString& InNewState,class UObject* InContextObject)
{
	for (INT i = 0; i < ProcessStateList.Num(); ++i)
	{
		if (InNewState == ProcessStateList(i)->GetClass()->GetName())
		{
			UKeyEventProcess* previousState = GetActiveState();
			if (previousState)
			{
				previousState->OnLeave();
			}

			ActiveIndex = i;
			ProcessStateList(i)->SetContext(InContextObject);
			ProcessStateList(i)->OnEnter();
			return TRUE;
		}
	}

	return FALSE;
}

UKeyEventProcess* UKeyEventProcessor::GetActiveState()
{
	if (ActiveIndex >= 0 && ActiveIndex < ProcessStateList.Num())
	{
		return ProcessStateList(ActiveIndex);
	}
	else
	{
		return NULL;
	}
}

void UKeyEventProcessor::OnNumberKey(BYTE KeyValue,UBOOL bRelease)
{
	UKeyEventProcess* ActiveState = GetActiveState();
	if (ActiveState)
	{
		ActiveState->OnNumberKey(KeyValue, bRelease);
	}
}

void UKeyEventProcessor::OnCharacterKey(BYTE KeyValue,UBOOL bRelease)
{
	UKeyEventProcess* ActiveState = GetActiveState();
	if (ActiveState)
	{
		ActiveState->OnCharacterKey(KeyValue, bRelease);
	}
}

void UKeyEventProcessor::OnSymbolKey(BYTE KeyValue,UBOOL bRelease)
{
	UKeyEventProcess* ActiveState = GetActiveState();
	if (ActiveState)
	{
		ActiveState->OnSymbolKey(KeyValue, bRelease);
	}
}

void UKeyEventProcessor::OnFunctionKey(BYTE KeyValue,UBOOL bRelease)
{
	UKeyEventProcess* ActiveState = GetActiveState();
	if (ActiveState)
	{
		ActiveState->OnFunctionKey(KeyValue, bRelease);
	}
}

void UKeyEventProcessor::OnModifierKey(BYTE KeyValue,UBOOL bRelease)
{
	UKeyEventProcess* ActiveState = GetActiveState();
	if (ActiveState)
	{
		ActiveState->OnModifierKey(KeyValue, bRelease);
	}

}

void UKeyEventProcessor::OnNavigationKey(BYTE KeyValue,UBOOL bRelease)
{
	UKeyEventProcess* ActiveState = GetActiveState();
	UMMOUIBase* pUI = MMOUI::GetFocusPanel();
	if (ActiveState && pUI )
	{
		ActiveState->OnNavigationKey(KeyValue, bRelease);
	}
}

void UKeyEventProcessor::OnSystemCommandKey(BYTE KeyValue,UBOOL bRelease)
{
	UKeyEventProcess* ActiveState = GetActiveState();
	if (ActiveState)
	{
		ActiveState->OnSystemCommandKey(KeyValue, bRelease);
	}
}

void UKeyEventProcessor::OnMouseKey(BYTE KeyValue,UBOOL bRelease)
{
	UKeyEventProcess* ActiveState = GetActiveState();
	if (ActiveState)
	{
		ActiveState->OnMouseKey(KeyValue, bRelease);
	}
}

/////////////////////////////////UKeyEventProcess///////////////////////////////////

void UKeyEventProcess::OnEnter()
{

}

void UKeyEventProcess::OnLeave()
{

}

void UKeyEventProcess::Tick(FLOAT Delta)
{

}

void UKeyEventProcess::UpdateCursor()
{

}

void UKeyEventProcess::SetContext(class UObject* InObject)
{

}

void UKeyEventProcess::OnNumberKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcess::OnCharacterKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcess::OnSymbolKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcess::OnFunctionKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcess::OnModifierKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcess::OnNavigationKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcess::OnSystemCommandKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcess::OnMouseKey(BYTE KeyValue,UBOOL bRelease)
{

}



/////////////////////////////////UKeyEventProcessDefaultState///////////////////////////////////

void UKeyEventProcessDefaultState::OnEnter()
{

}

void UKeyEventProcessDefaultState::OnLeave()
{

}


void UKeyEventProcessDefaultState::UpdateCursor()
{
	typedef TScriptInterface<IIPickableActor> IPickableActor;
	IPickableActor pickable = ScriptInterfaceCast<IIPickableActor>(Controller->GetTraceActor());
	UBOOL CaptureInput = MMOFacade::GetCaptureInput();
	
	extern UBOOL g_bMouseInPanel;
	
	if(pickable && CaptureInput==FALSE && g_bMouseInPanel==FALSE)
	{
		MMOFacade::getSingleton().SetMouseCursor((EMMOCursorType)pickable->GetMouseCursor());
	}
	else
	{
		MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
	}
	
}

void UKeyEventProcessDefaultState::OnNumberKey(BYTE KeyValue,UBOOL bRelease)
{
}

void UKeyEventProcessDefaultState::OnCharacterKey(BYTE KeyValue,UBOOL bRelease)
{
}

void UKeyEventProcessDefaultState::OnSymbolKey(BYTE KeyValue,UBOOL bRelease)
{
}

void UKeyEventProcessDefaultState::OnFunctionKey(BYTE KeyValue,UBOOL bRelease)
{
	switch (KeyValue)
	{
	case KC_Enter:
		{
			AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
			if (MMO::GetCurrentGameState() == EGameState_InGame && pc)
				pc->NativeOnEnterPressed();
		}
		break;
	}
}

void UKeyEventProcessDefaultState::OnModifierKey(BYTE KeyValue,UBOOL bRelease)
{
}

void UKeyEventProcessDefaultState::OnNavigationKey(BYTE KeyValue,UBOOL bRelease)
{
}

void UKeyEventProcessDefaultState::OnSystemCommandKey(BYTE KeyValue,UBOOL bRelease)
{
}

void UKeyEventProcessDefaultState::OnMouseKey(BYTE KeyValue,UBOOL bRelease)
{
	switch(KeyValue)
	{
	case KC_MouseLeft:
		Controller->eventExecOnLeftMouseKey(bRelease);
		break;
	case KC_MouseRight:
		Controller->NativeOnRightMouseKey(bRelease);
		break;
	case KC_MouseScrollUp:
		Controller->ZoomCameraIn();
		break;
	case KC_MouseScrollDown:
		Controller->ZoomCameraOut();
		break;
	default:
		break;
	}
}



/////////////////////////////////UKeyEventProcessStateAbilityTargetSelect///////////////////////////////////

void UKeyEventProcessStateAbilityTargetSelect::SetContext(class UObject* InObject)
{
	AbilityToUse = Cast<UMMOAbility>(InObject);
}

void UKeyEventProcessStateAbilityTargetSelect::OnEnter()
{
	check(AbilityToUse);
	if (AbilityToUse->ClientCommonAttribute.TargetSelectType == TST_Aim)
	{
		Controller->SetCurrentMouseCursor(EMC_Aim);

		AMMOActor* BaseActor = Cast<AMMOActor>(Controller->Pawn);
		if (BaseActor && BaseActor->GpDataManager && BaseActor->GpDataManager->CommonShootCameraAnim)
		{
			BaseActor->GpDataManager->CommonShootCameraAnim->eventShake(Controller, 1.0);
		}
	}
}

void UKeyEventProcessStateAbilityTargetSelect::OnLeave()
{
	check(AbilityToUse);
	if (AbilityToUse->ClientCommonAttribute.TargetSelectType == TST_Aim)
	{
		Controller->SetCurrentMouseCursor(EMC_Arrow);

		AMMOActor* BaseActor = Cast<AMMOActor>(Controller->Pawn);
		if (BaseActor && BaseActor->GpDataManager && BaseActor->GpDataManager->CommonShootCameraAnim)
		{
			BaseActor->GpDataManager->CommonShootCameraAnim->eventStopShake(Controller);
		}
	}
}

void UKeyEventProcessStateAbilityTargetSelect::UpdateCursor()
{

}

void UKeyEventProcessStateAbilityTargetSelect::OnNumberKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessStateAbilityTargetSelect::OnCharacterKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessStateAbilityTargetSelect::OnSymbolKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessStateAbilityTargetSelect::OnFunctionKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessStateAbilityTargetSelect::OnModifierKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessStateAbilityTargetSelect::OnNavigationKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessStateAbilityTargetSelect::OnSystemCommandKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessStateAbilityTargetSelect::OnMouseKey(BYTE KeyValue,UBOOL bRelease)
{
	UBOOL bNeedJumpOutState = FALSE;

	if (bRelease == TRUE)
	{
		return;
	}

	switch(KeyValue)
	{
	case KC_MouseLeft:
		{
			AActor* TracedActor = Controller->GetTraceActor();
			FVector TraceLocation = Controller->GetFixedMouseHitWorldLocation();
			if (AbilityToUse->SpellCommonAttribute.LockType == E_splt_Self) // self actor
			{
				if (TracedActor && TracedActor == Controller->Pawn)
				{
					Controller->eventAddCmdUseAbility(AbilityToUse, TracedActor, TraceLocation);
					bNeedJumpOutState = TRUE;
				}
			}
			else if (AbilityToUse->SpellCommonAttribute.LockType == E_splt_Object) // target actor
			{
				if (TracedActor && MMOFacade::getSingleton().IsAttackable(Cast<AMMOActor>(Controller->Pawn), Cast<AMMOActor>(TracedActor)))
				{
					Controller->eventAddCmdUseAbility(AbilityToUse, TracedActor, TraceLocation);
					bNeedJumpOutState = TRUE;
				}
			}
			else
			{
				Controller->eventAddCmdUseAbility(AbilityToUse, TracedActor, TraceLocation);
				bNeedJumpOutState = TRUE;
			}

			break;
		}
	case KC_MouseRight:
		{
			bNeedJumpOutState = TRUE;
			break;
		}
	default:
		break;
	}

	if (bNeedJumpOutState)
	{
		Controller->SetGameKeyEventProcessState(FString(TEXT("KeyEventProcessDefaultState")));
	}
}





/////////////////////////////////UKeyEventProcessDisableState///////////////////////////////////

void UKeyEventProcessDisableState::OnEnter()
{

}

void UKeyEventProcessDisableState::OnLeave()
{

}

void UKeyEventProcessDisableState::OnNumberKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessDisableState::OnCharacterKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessDisableState::OnSymbolKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessDisableState::OnFunctionKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessDisableState::OnModifierKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessDisableState::OnNavigationKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessDisableState::OnSystemCommandKey(BYTE KeyValue,UBOOL bRelease)
{

}

void UKeyEventProcessDisableState::OnMouseKey(BYTE KeyValue,UBOOL bRelease)
{

}