#include "stdafx.h"

IMPLEMENT_CLASS(AMMOTorchLight)

void AMMOTorchLight::PostBeginPlay()
{
	Super::PostBeginPlay();
	CurrentParameter = RandRange( 0, 0.25f/BrightnessFrequency );
	LastUpdateWindTime = CurrentParameter;
	// The persistent helper starts before streamed levels. Register here so
	// torches receive the current authored day/night value as they enter play.
	for (FActorIterator It; It; ++It)
	{
		AMMODayTimeHelper* Helper = Cast<AMMODayTimeHelper>(*It);
		if (Helper)
		{
			Helper->InsetTorchLight(this);
		}
	}
}

void AMMOTorchLight::UpdateShining(FLOAT DeltaTime)
{
	FColor color = LightComponent->LightColor;
	ULightFunction *fun = LightComponent->Function;

	CurrentParameter += DeltaTime;
	float Shining = sin( ( CurrentParameter ) * BrightnessFrequency * 2.0f * PI ) * BrightnessRange;

	if ( WindAffect != 0.f && CurrentParameter - LastUpdateWindTime > WindAffectTime )
	{
		LastUpdateWindTime = CurrentParameter;
		Shining += RandRange( -WindAffect, WindAffect ); 
	}

	float Brightness = CurrentBrightness > 0.0001f ? std::max( 0.f, CurrentBrightness + Shining ): 0.f;
	LightComponent->SetLightProperties( Brightness * BrightnessFactor , color, fun );
}

UBOOL AMMOTorchLight::Tick( FLOAT DeltaTime, enum ELevelTick TickType )
{
	CTRL_TICK_F("MMOTorchLight");

	UpdateShining( DeltaTime );
	return APointLightToggleable::Tick( DeltaTime, TickType );
}

void AMMOTorchLight::SetCurrentBrightness( float Brightness )
{
	CurrentBrightness = Brightness;
}