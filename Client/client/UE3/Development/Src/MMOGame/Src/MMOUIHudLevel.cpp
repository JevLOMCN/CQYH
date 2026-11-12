#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "LoadMapTipsManager.h"

IMPLEMENT_CLASS(UMMOUIHudLevel)

UBOOL UMMOUIHudLevel::DrawMyHUD( class AMMOHud* hud )
{
	INT sizeX = MMOFacade::getSingleton().GetScreenSize().X;
	INT sizeY = MMOFacade::getSingleton().GetScreenSize().Y;

	// 1600x900 raw
	INT offsetY = 0;
	INT offsetX = 0;
	INT drawSizeX = 1600;
	INT drawSizeY = 900;

	// Stretch only if exceeded, cut everything if smaller
	//if(sizeX > 1600)
	{
		FLOAT xScale = (FLOAT)sizeX / 1600.f;
		FLOAT yScale = (FLOAT)sizeY / 900.0f;

		if(xScale > yScale)
		{
			drawSizeX *= xScale;
			drawSizeY *= xScale;
		}
		else
		{
			drawSizeX *= yScale;
			drawSizeY *= yScale;
		}
	}

	offsetX = (sizeX - drawSizeX) / 2;
	offsetY = (sizeY - drawSizeY) / 2;

	if( hud && textBg )
	{
		hud->Canvas->SetPos(0, 0);
		//hud->Canvas->SetDrawColor(255, 255, 255, (BYTE)(255.0*fAlpha));		
		hud->Canvas->DrawTile( textBg, offsetX, offsetY, 1 , drawSizeX , drawSizeY , 0 , 0 , textBg->GetSurfaceWidth() , textBg->GetSurfaceHeight() , FColor(255,255,255,(BYTE)(255.0*fAlpha)) );

	}
	return true;
}



void UMMOUIHudLevel::LoadBg()
{
	//INT imgNum = 1;
	//GConfig->GetInt(TEXT("LoadingBg"), TEXT("Count"), imgNum, GUIIni);

	//FString imgPath = TEXT("MMOUIImage.Loading.bg");
	//imgPath += appItoa( appRand()%imgNum );
	//textBg = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass() , NULL , *imgPath , NULL, LOAD_NoWarn , NULL) );

	textBg = LoadUEObject<UTexture2D>(LoadMapTipsManager::getSingleton().RandomImage(), TEXT("loadmaptips.txt"));
}

void UMMOUIHudLevel::ClearBg()
{
	//Smart pointer should be directly usable
	textBg = NULL;
	//if( textBg )
	//{		
	//	textBg->MarkPendingKill();
	//}
}

void UMMOUIHudLevel::DrawHUDPostGFx( AHUD* H )
{
}
