/********************************************************************
	Created:	2013/06
	Filename: 	UMMOGFXPlayer_PickWidget
	file ext:	cpp
	Author:		hemu@snda
	Purpose:	Drop pickup GFXPlayer-PickWidget
*********************************************************************/

#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

#include "ScaleformEngine.h"
#include "MMODropManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_PickWidget)


void UMMOGFXPlayer_PickWidget::OnHUDLoaded()
{
	//add delegate after HUD finishes loading
// 	AMMOHud *hud = MMOFacade::getSingleton().GetMyHud();
// 
// 	FScriptDelegate fun(EC_EventParm);
// 	fun.FunctionName = "OnPostRender";
// 	fun.Object = this;
// 
// 	if( hud )
// 		hud->AddPostRenderDelegate( fun );
}

void UMMOGFXPlayer_PickWidget::OnPostRender( UCanvas*  cvs, float fDelta )
{
// 	if(!cvs)
// 		return;
// 	if( _DropItemTips)
// 	{
// 		_DropItemTips->Update( cvs, fDelta );
// 	}
}

void UMMOGFXPlayer_PickWidget::PostAdvance(float DeltaTime)
{

}

void UMMOGFXPlayer_PickWidget::Init()
{
	Super::Init();
	
	check(FALSE && "UMMOGFXPlayer_PickWidget::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_PickWidget);

	//__OnPostAdvance__Delegate = FScriptDelegate(this, TEXT("PostAdvance"));
}

void UMMOGFXPlayer_PickWidget::UnInit()
{
	Super::UnInit();

	//__OnPostAdvance__Delegate = FScriptDelegate(EC_EventParm);
}

void UMMOGFXPlayer_PickWidget::Pickup(INT ItemID)
{
	MMODropManager::getSingleton().PickupDropActorByUI(ItemID);
}


