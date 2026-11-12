#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOHelperManager.h"
//#include "MMOScreenMsgMgr.h"
#include "ScreenMsgQueue.h"
#include <time.h>
#include "EngineAudioDeviceClasses.h"

IMPLEMENT_CLASS(UMMOUIHelperPlayerPanel)

void UMMOUIHelperPlayerPanel::Init()
{
	Super::Init();
}

void UMMOUIHelperPlayerPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
		FVector2D helperSize;
		helperSize.X = this->GetFloat(TEXT("width"));
		helperSize.Y = this->GetFloat(TEXT("height"));

		FVector2D screenSize = MMOFacade::GetScreenSize();
		INT posX = screenSize.X - helperSize.X;
		INT posY = screenSize.Y - 200;

		SetPosition(posX, posY);
	}
	else
	{
		StopCurSound();
	}
}

void UMMOUIHelperPlayerPanel::UpdateHelperSoundData(UBOOL isPlaying)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (object)
	{
		object->SetString(TEXT("player_head_icon"), FString(TEXT("img://")) + _CurHeadIcon);
		object->SetString(TEXT("player_picture"), FString(TEXT("img://UI_Achievement.Icon.2DUIAchievementIcons_8_35")));
		object->SetBool(TEXT("player_is_playing"), isPlaying);
		eventCallUpdateHelperSoundData(object);
	}
}

void UMMOUIHelperPlayerPanel::ShowPlayerPanel(const FString& headIcon, USoundCue* sound)
{
	Show(TRUE);
	if (sound)
	{
		_CurHeadIcon = headIcon;

		// If a narration is still playing, stop it
		StopCurSound();

		_CurSound = sound;
		
		if (_CurSound)
		{
			MMOFacade::getSingleton().GetMainActor()->PlaySound(_CurSound);
			UpdateHelperSoundData(TRUE);
			MMOHelperManager::getSingleton().OnShowPlayer(_CurSound->Duration);
		}
	}
}

void UMMOUIHelperPlayerPanel::StopCurSound()
{
	if (_CurSound)
	{
		MMOFacade::getSingleton().StopVoiceSound(_CurSound->GetPathName(), FALSE, 1.0, 0.0);
	}
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIHelperPlayerPanel::ASPlayHelperSound()
{
	if (_CurSound)
	{
		MMOFacade::getSingleton().GetMainActor()->PlaySound(_CurSound);
		UpdateHelperSoundData(TRUE);
		MMOHelperManager::getSingleton().OnShowPlayer(_CurSound->Duration);
	}
}

void UMMOUIHelperPlayerPanel::ASPauseHelperSound()
{
	StopCurSound();
	UpdateHelperSoundData(FALSE);
}
