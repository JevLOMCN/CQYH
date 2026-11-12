#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FSMIO.h"

IMPLEMENT_CLASS(UMMOUIProgressBarQTEWidget)


void UMMOUIProgressBarQTEWidget::Init()
{
	Super::Init();
	MMOUI::GetUIManager()->SetASClickQTEDelegate(FScriptDelegate(this, TEXT("ASClickQTE")));

}

void UMMOUIProgressBarQTEWidget::UnInit()
{
	Super::UnInit();
	MMOUI::GetUIManager()->SetASClickQTEDelegate(FScriptDelegate(EC_EventParm));
}

void UMMOUIProgressBarQTEWidget::UpdateQTE(UCanvas* cvs)
{
	if( !cvs || !cvs->SceneView )
		return;

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(m_nActorID);
	if(actor==NULL)
	{
		eventClose(1);
		return;
	}
	if(actor->eventIsDead())
	{
		eventClose(1);
		return;
	}

	FPlane V(0,0,0,0);
	V = cvs->SceneView->Project( actor->Location + FVector(0,0,185) );
	FVector resultVec(V);
	resultVec.X = (cvs->ClipX/2.f) + (resultVec.X*(cvs->ClipX/2.f));
	resultVec.Y *= -1.f;
	resultVec.Y = (cvs->ClipY/2.f) + (resultVec.Y*(cvs->ClipY/2.f));

	if (V.W <= 0.0f)
	{
		resultVec.Z = 0.0f;
	}
	FVector ScreenPos = resultVec;
	SetPosition( ScreenPos.X,ScreenPos.Y );//image center

}

int UMMOUIProgressBarQTEWidget::GetTargetActorID()
{
	return m_nActorID;
}
int UMMOUIProgressBarQTEWidget::GetTargetAbilityID()
{
	return m_nAbilityID;
}

UBOOL UMMOUIProgressBarQTEWidget::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if(strHoverControllerName==TEXT("_QTEBar"))
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

void UMMOUIProgressBarQTEWidget::ShowProgressBarQTE(INT time,const FString& keyName,const FString& imgName,INT nActorID,INT nAbilityID)
{
	eventOpen(time, keyName,imgName, nActorID, nAbilityID);
}
void UMMOUIProgressBarQTEWidget::CloseProgressBarQTE(INT closeType)
{
	eventClose(closeType);
}

void UMMOUIProgressBarQTEWidget::ASClickQTE()
{
	if(IsVisible)
	{
		CloseProgressBarQTE(0);	

		AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();	
		if(ctrl&&ctrl->GetPlayer())
		{
			UMMOAbility*	useAbility	= ctrl->GetPlayer()->GetEquipedAbilityById( ctrl->KeyMapManager->GetCmdIDBySlotID(ECK_QTE1) );// ECK_QTE2) );
			//UMMOAbility*	useAbility	= ctrl->GetPlayer()->GetAbilityById( _QTEBar->GetTargetAbilityID()); 
			AMMOActor*		Target = MMOFacade::getSingleton().GetCharacter( GetTargetActorID());
			if(useAbility&&Target)
			{
				//MMOTargetManager::getSingleton().SetDesireTarget(useAbility, Target);
				FSMIO::getSingleton().Target.SetDesireTarget(useAbility, Target);
				//ctrl->AddCmdHuntToUseAbility(useAbility,Target,Target->Location);
			}
		}
	}
}

void UMMOUIProgressBarQTEWidget::PostAdvance(float DeltaTime)
{
	if( !IsVisible ){
		return;
	}

	//UpdateQTE(DeltaTime);
}
