/**
*	MMODayTimeHelper.cpp
*/

#include "stdafx.h"

IMPLEMENT_CLASS(AMMODayTimeHelper)

void AMMODayTimeHelper::PostBeginPlay()
{
	if (!GWorld)
	{
		return;
	}

	if (GWorld->DominantDirectionalLight)
	{
		MainLight = Cast<ADominantDirectionalLight>(GWorld->DominantDirectionalLight->GetOwner());
		MainLightColor = MainLight->LightComponent->LightColor;
		MainLightIntensity = 1.0f;
	}

	for (FActorIterator iter; iter; ++iter)
	{
		ADirectionalLightToggleable* Light = Cast<ADirectionalLightToggleable>(*iter);
		if (Light)
		{
			DirectionalLights.Push(Light);
			DirectionalLightColors.Push(Light->LightComponent->LightColor);
			DirectionalLightIntensities.Push(1.0f);
		}

		AMMOTorchLight* pTorchLight = Cast<AMMOTorchLight>(*iter);
		if ( pTorchLight )
		{
			InsetTorchLight(pTorchLight);
		}
	}

	LightMapScale = FColor(255, 255, 255, 255);
}

void AMMODayTimeHelper::ForceUpdateComponents(UBOOL bCollisionUpdate,UBOOL bTransformOnly)
{
	if (GIsEditor)
	{
		if (WorldInfo)
		{
			WorldInfo->LightMapScale = LightMapScale;
			//WorldInfo->LuminanceThreshold = LuminanceThreshold;	// zhaojun@snda
			//shanling@snda
			WorldInfo->NewLimitLuminance  = NewLimitLuminance;
			WorldInfo->NewMiddleLuminance = NewMiddleLuminance;
			WorldInfo->NewWhiteLuminance  = NewWhiteLuminance;
			WorldInfo->NewAvgLuminanceParameter = NewAvgLuminanceParameter;
			//shanling@snda
		}
	}

	GetTorchLights();
	UpdateTorchLightsBrightness();
	Super::ForceUpdateComponents(bCollisionUpdate, bTransformOnly);
}

void AMMODayTimeHelper::TickSpecial(FLOAT DeltaTime)
{
	if (WorldInfo)
	{
		WorldInfo->LightMapScale = LightMapScale;
		//WorldInfo->LuminanceThreshold = LuminanceThreshold;	// zhaojun@snda

		//shanling@snda
// 		WorldInfo->NewLimitLuminance  = NewLimitLuminance;
// 		WorldInfo->NewMiddleLuminance = NewMiddleLuminance;
// 		WorldInfo->NewWhiteLuminance  = NewWhiteLuminance;
// 		WorldInfo->NewAvgLuminanceParameter = NewAvgLuminanceParameter;
		//shanling@snda
	}
	
	UpdateTorchLightsBrightness();
	
}

void AMMODayTimeHelper::GetTorchLights()
{
	ClearTorchLights();
	for (FActorIterator iter; iter; ++iter)
	{
		AMMOTorchLight* pTorchLight = Cast<AMMOTorchLight>(*iter);
		if ( pTorchLight )
		{
			InsetTorchLight( pTorchLight );
		}
	}
}

void AMMODayTimeHelper::InsetTorchLight( AMMOTorchLight *plithgt )
{
	if (plithgt && !plithgt->bDeleteMe && !plithgt->IsPendingKill())
	{
		TorchLights.AddUniqueItem(plithgt);
		// Streaming can introduce a torch after the last day/night transition.
		plithgt->SetCurrentBrightness(TorchLightBrightness);
	}
}

void AMMODayTimeHelper::ClearTorchLights()
{
	TorchLights.Empty();
}

void AMMODayTimeHelper::UpdateTorchLightsBrightness()
{
	// Do not rely only on the cached brightness: a streamed-in torch also needs
	// the current state when the day/night value itself has not changed.
	for (INT TorchIndex = TorchLights.Num() - 1; TorchIndex >= 0; --TorchIndex)
	{
		AMMOTorchLight* Torch = TorchLights(TorchIndex);
		if (!Torch || Torch->bDeleteMe || Torch->IsPendingKill())
		{
			TorchLights.Remove(TorchIndex);
		}
		else if (Torch->CurrentBrightness != TorchLightBrightness)
		{
			Torch->SetCurrentBrightness(TorchLightBrightness);
		}
	}
	DefaultTorchLightBrightness = TorchLightBrightness;
}
