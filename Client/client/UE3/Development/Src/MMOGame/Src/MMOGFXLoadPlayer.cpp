#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "LoadMapTipsManager.h"

void UMMOGFXLoadPlayer::NativeOnClose()
{

}

void UMMOGFXLoadPlayer::OnEnter()
{
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if(ctrl)
	{
		AMMOHud* hud = Cast<AMMOHud>( ctrl->myHUD );
		if(hud)
		{
			//if( !BgLoader )
			//{
			//	return;//BgLoader = new UMMOUIHudLevel;
			//}		
			//MMOUI::GetUIManager()->RegisterMMOActorHUD( BgLoader );
			//
			////hud->UILevel.AddItem(BgLoader);
			//BgLoader->draw = true;
			//BgLoader->LoadBg();

			if(loadingBar == NULL)
			{
				loadingBar = (UMMOUILoadingBar*)GetVariableObject("root.bar_mc");
			}
			if(loadingBar)
			{
				//loadingBar->NativeCallSet_Back(Cast<UGFxObject>(BgLoader));
				loadingBar->eventCallUpdateLoadingBar(0);
				INT sizeY = MMOFacade::getSingleton().GetScreenSize().Y;
				INT sizeX = MMOFacade::getSingleton().GetScreenSize().X;
				FLOAT fHeight = loadingBar->GetFloat("height");
				FLOAT fWidth = loadingBar->GetFloat("width");
					warnf(TEXT("[LoadingDiag] OnEnter screen=%dx%d bar w=%.1f h=%.1f x=%.1f y=%.1f"), sizeX, sizeY, fWidth, fHeight, loadingBar->GetFloat("x"), loadingBar->GetFloat("y"));
				loadingBar->SetFloat("y",0.0f /* was sizeY-fHeight+30; swf lays out in stage coords */);
				loadingBar->SetFloat("x",0.0f);
			}
			OnUpdate();
		}
	}
}

void UMMOGFXLoadPlayer::OnLeave()
{
	//if(BgLoader)
	//{
	//	BgLoader->ClearBg();
	//}

	//MMOUI::GetUIManager()->UnRegisterMMOActorHUD(BgLoader);

}
void UMMOGFXLoadPlayer::OnUpdate()
{
	//UGFxObject* textTips = GetVariableObject("root.bar_mc.tF_title");
	if(loadingBar)
	{
		FString randTips = LoadMapTipsManager::getSingleton().RandomMapTips();
		loadingBar->eventCallSetLoadTips(randTips);
	}
}

void UMMOGFXLoadPlayer::PostAdvance ( FLOAT DeltaTime )
{
	UGFxMoviePlayer::PostAdvance(DeltaTime);

	//if(BgLoader&&BgLoader->textBg)
	//{
		static FLOAT fPro = -1;
		if(abs(fPro - UObject::CurrentTotalPercent) >= 0.01)
		{
			if(loadingBar)
			{
				fPro = UObject::CurrentTotalPercent;
				loadingBar->eventCallUpdateLoadingBar(fPro);
			}
		}
	//}
}


void UMMOGFXLoadPlayer::ASPreEnterCompleted()
{
	if(loadingBar)
	{
		loadingBar->PreEnterCompleted();
	}
}

void UMMOGFXLoadPlayer::ASPreLeaveCompleted()
{
	if(loadingBar)
	{
		loadingBar->PreLeaveCompleted();
	}
	OnLeave();
	GetVariableObject(TEXT("root"))->SetVisible(FALSE);
	eventSetVisible(FALSE);
}

void UMMOGFXLoadPlayer::ASCallBackSetAlpha(FLOAT alphaValue)
{
	//if(BgLoader)
	//{
	//	BgLoader->fAlpha = alphaValue;
	//}
}
