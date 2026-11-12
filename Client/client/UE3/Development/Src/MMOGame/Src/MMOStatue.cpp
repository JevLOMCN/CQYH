#include "stdafx.h"
#include "MMOFacade.h"
#include "MMOGuildData.h"

void AMMOStatue::DrawHUDPostGFx(AHUD* hud)
{
#if M_DEBUG_HUD_NAME
	FVector screenPos;
	FVector objectIconWorldPos;


	objectIconWorldPos = Location;//(HealthBarPositionOffset>>Rotation);//actual position
	objectIconWorldPos.Z += 120;
	screenPos = H->Canvas->Project(objectIconWorldPos);

	H->Canvas->SetDrawColor(255, 0, 0, 255);
	//H->Canvas->SetPos(drawX, drawY);
	FVector2D drawPos(screenPos.X, screenPos.Y);
	DrawBox2D(H->Canvas->Canvas, drawPos, drawPos + FVector2D(100, 20), FLinearColor(1, 0, 0));

#endif

	if(g_tickOrder == ETO_AfterDrawHUDPostGfxTick)
	{
		if(IsMainPlayer())
		{
			_OnMMOTick(GDeltaTime);
			logTickOrderf(TEXT("ETO_AfterDrawHUDPostGfxTick"));
		}
	}
}

UBOOL AMMOStatue::DrawMyHUD(class AMMOHud* hud)
{
	return TRUE;
}

void AMMOStatue::SetSilhouette( UBOOL show, FVector sc )
{
	Super::SetSilhouetteEx(show, sc);
}

void AMMOStatue::DoInteract()
{
}

UBOOL AMMOStatue::CanInteract() const
{
	return TRUE;
}

FVector AMMOStatue::GetInteractActorLocation()
{
	return Location;
}

FLOAT AMMOStatue::GetMaxInteractRadius()
{
	return MMOFacade::getSingleton().GetMainController()->InteractDistance;
}

