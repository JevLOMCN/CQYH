#include "stdafx.h"

void AMMOVolume_AmbientMusic::PlayTrack()
{
	AWorldInfo *WorldInfo = GWorld->GetWorldInfo();
	if (WorldInfo != NULL)
	{
		WorldInfo->UpdateMusicTrack(MusicTrack);
	}
}

void AMMOVolume_AmbientMusic::StopPlayTrack()
{
	AWorldInfo *WorldInfo = GWorld->GetWorldInfo();
	if (WorldInfo != NULL && WorldInfo->MusicComp != NULL)
	{
		// can only stop ones identical to itself; if different, it was probably overridden by another Volume or KISMET
		if(WorldInfo->MusicComp->SoundCue == MusicTrack.TheSoundCue)
		{
			WorldInfo->MusicComp->FadeOut(0.5f, 0);
		}
	}
	//AWorldInfo *WorldInfo = GWorld->GetWorldInfo();
	//if (WorldInfo != NULL && MusicTrack.TheSoundCue)
	//{
	//	FString SoundCueName;
	//	MusicTrack.TheSoundCue->GetName(SoundCueName);
	//	WorldInfo->ClearAcrossLevelMusicByCueName(SoundCueName);
	//}
}