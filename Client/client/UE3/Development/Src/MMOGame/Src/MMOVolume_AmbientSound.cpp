#include "stdafx.h"
#include "EngineAudioDeviceClasses.h"
#include "MMOWeatherManager.h"

void AMMOVolume_AmbientSound::OnActive(APawn* P, UBOOL bOn)
{
	AMMOActor* mmoActor = Cast<AMMOActor>(P);
	if (mmoActor && mmoActor->IsMainPlayer())
	{
		FSceneInterface* CS = NULL;
		for (INT i = 0; i < mmoActor->Components.Num(); ++i)
		{
			if (mmoActor->Components(i))
			{
				CS = mmoActor->Components(i)->GetScene();
				if (CS)
				{
					break;
				}
			}
		}

		if (!CS)
		{
			debugf(L"CS is null ,check!!!");
			return;
		}
		if (bOn)
		{
			MainActor = mmoActor;
			AudioComponentList.Empty();

			TArrayNoInit<class USoundCue*>* soundToPlay = &SoundCueList;
			// modified by zhangrui05 removed day/night system
			/*
			if(MMOWeatherManager::getSingleton().IsNight())
			{
				soundToPlay = &NightSoundCueList;
			}
			*/
			for(INT i = 0; i < (*soundToPlay).Num(); ++i)
			{
				RelativeX = RandHelper(2 *MaxRandX) - MaxRandX;
				RelativeY = RandHelper(2 * MaxRandY) - MaxRandY;
				
				FVector Point = mmoActor->Location + FVector(RelativeX, RelativeY, 0);
				UAudioComponent* AudioComponent = UAudioDevice::CreateComponent( (*soundToPlay)(i), CS, this);
				if( AudioComponent )
				{
					AudioComponent->Location = Point;
					AudioComponent->bUseOwnerLocation = FALSE;
					AudioComponent->bStopWhenOwnerDestroyed = TRUE;
					AudioComponent->bAutoDestroy			= 1;
					AudioComponent->FadeIn(FadeInTime, 1.f);
					AudioComponentList.AddItem(AudioComponent);
				}
			}
		}
		else
		{
			for (INT i = 0; i < AudioComponentList.Num(); ++i)
			{
				if (AudioComponentList(i) && AudioComponentList(i)->IsPlaying())
				{
					AudioComponentList(i)->FadeOut(FadeOutTime, 0.0f);
				}
			}
			AudioComponentList.Empty();
		}
	}
}

void AMMOVolume_AmbientSound::TickSpecial( FLOAT DeltaSeconds )
{
	AVolume::TickSpecial(DeltaSeconds);
	if (MainActor == NULL || AudioComponentList.Num() == 0)
	{
		return;
	}

	for (INT i = 0; i < AudioComponentList.Num(); ++i)
	{
		if (AudioComponentList(i))
		{
			FVector NewPoint = MainActor->Location + FVector(RelativeX, RelativeY, 0);
			AudioComponentList(i)->Location = NewPoint;
		}
	}

}