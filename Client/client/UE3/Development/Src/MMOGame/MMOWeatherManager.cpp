#include "stdafx.h"
#include "MMOWeatherManager.h"
#include "MMOFacade.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOChatManager.h"
#include "MMOMsgBoxManager.h"
#include "CommonGameEvents.h"
#include "EngineInterpolationClasses.h"
#include "MMOFreeTipsUIManager.h"

using namespace ExternalMessage;

MMOWeatherManager::MMOWeatherManager()
{
	_HourPerDay=2.0;
	_StartTime=0.0;
	_StartTotalTime=0.0;
	_CurrentTime=12.0;
	_OldEventTime = 0.0f;
	_AnimTime = 0.0f;

	_WeatherHelper = NULL;
	_DayTimeHelper = NULL;
	_CurWeatherType = swtNormal;
	_CurWeatherLv = WL_Light;
}

MMOWeatherManager::~MMOWeatherManager()
{
}

void MMOWeatherManager::Init()
{
	Clear();
}

void MMOWeatherManager::Shutdown()
{
	Clear();
}

void MMOWeatherManager::Clear()
{
	_HourPerDay = 2.0f;
	_StartTime = 12.0f;
	_StartTotalTime = GCurrentTime;
	_CurrentTime = 12.0f;

	_WeatherHelper = NULL;
	_DayTimeHelper = NULL;
	_CurWeatherType = swtNormal;
	_CurWeatherLv = WL_Light;
	_OldEventTime = 0.0f;
	_AnimTime = 0.0f;
}

void MMOWeatherManager::Update(FLOAT deltaTime)
{
	CTRL_TICK("MMOWeatherManager");

	// temporarily disable day/night system
#if 0
	if (_HourPerDay < 0.01f)
	{
		_HourPerDay = 0.01f;
	}

	_CurrentTime = fmod(_StartTime, _HourPerDay) / _HourPerDay * 24.0f + (GCurrentTime - _StartTotalTime) / 60.0f / 60.0f / _HourPerDay * 24.0f;
	if (_CurrentTime > 24.0f)
	{
		_CurrentTime = fmod(_CurrentTime, 24.0f);
	}

	_AnimTime += deltaTime;
	if (_AnimTime > (_HourPerDay * 3600.0f / 480.0f))
	{
		MMOGameEvent_Any evt(GameEvent_UpdateDayTime);
		MMOUI::PostGameEvent(&evt);
		_AnimTime = 0.0f;
	}

	// triggers once on the hour (equivalent to once every 5 real-world minutes)
	if (abs((INT)(_CurrentTime) - (INT)(_OldEventTime)) >= 1)
	{
		_ActivateDayTimePlanEvent();
		_UpdateDayTimeSunlightEvent();
		_OldEventTime = _CurrentTime;
	}
#endif
}

void MMOWeatherManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
}

void MMOWeatherManager::InitStartTime(UINT32 startTime)
{
	// temporarily disable day/night system
#if 0
	__time32_t localTime = startTime;
	struct tm* date = _localtime32(&localTime);
	if (date)
	{
		_StartTime = date->tm_hour + ((FLOAT)(date->tm_min) / 60.0f);
	}

	_StartTotalTime = GCurrentTime;
	_HourPerDay = (float)DAYTIME_HOURS;
	if (_HourPerDay < 0.01f)
	{
		_HourPerDay = 0.01f;
	}

	_CurrentTime = fmod(_StartTime, _HourPerDay) / _HourPerDay * 24.0f + (GCurrentTime - _StartTotalTime) / 60.0f / 60.0f / _HourPerDay * 24.0f;
	if (_CurrentTime > 24.0f)
	{
		_CurrentTime = fmod(_CurrentTime, 24.0f);
	}

	_OldEventTime = _CurrentTime;
#endif
}

FLOAT MMOWeatherManager::GetCurTime() const
{
	return _CurrentTime;
}

void MMOWeatherManager::EnterScene(UINT8 weaterType)
{
	// temporarily disable day/night system
#if 0
	// enable day/night
	_DayTimeHelper = NULL;
	for (FActorIterator iter; iter; ++iter)
	{
		_DayTimeHelper = Cast<AMMODayTimeHelper>(*iter);
		if (_DayTimeHelper)
		{
			break;
		}
	}
	if (_DayTimeHelper)
	{
		_ActivateDayTimeSunlightEvent();
	}
#endif

	// enable weather
	_WeatherHelper = NULL;
	if (GWorld && GWorld->GetWorldInfo() && GWorld->GetWorldInfo()->MyWeatherManager)
	{
		_WeatherHelper = Cast<AMMOWeatherHelper>(GWorld->GetWorldInfo()->MyWeatherManager);
	}
	if (_WeatherHelper)
	{
		_WeatherHelper->EnableWeather();
		StartWeather(weaterType);
	}
}

void MMOWeatherManager::_UpdateWeatherTips()
{
	// Temporarily disabled
#if 0
	FString weatherStr = TEXT("");
	switch (_CurWeatherType)
	{
	case swtInvalid:
	case swtNormal:
		break;
	case swtFog:
		break;
	case swtThunder:
		break;
	case swtRain:
		weatherStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.Rain"));
		break;
	case swtSnow:
		weatherStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.Snow"));
		break;
	case swtStorm:
		weatherStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.Storm"));
		break;
	case swtEclipse:
		break;
	}

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIMinimap* pMinimap = Cast<UMMOUIMinimap>(MMOUI::FindUIPanelEx(uiManager->Panel_Minimap));
		if (pMinimap)
		{

			FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.Tips")), 
				IsNight() ? *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.Night")) : *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.Day")), 
				*weatherStr);
			MMOFreeTipsUIManager::getSingleton().CallUCSetFreeTips(pMinimap, TEXT("DayTime_mc"), TEXT(""), TEXT(""), TEXT(""), text, TEXT("default"));
		}
	}
#endif
}

void MMOWeatherManager::StartWeather(UINT8 weaterType)
{
	if (!_WeatherHelper)
	{
		return;
	}
    if (!MMOUI::GetUIManager()->DrawWeather)
        return;

	_CurWeatherType = weaterType;
	_CurWeatherLv = appRand() % WL_Total;

	switch (_CurWeatherType)
	{
	case swtInvalid:
	case swtNormal:
		StopAllWeather();
		break;
	case swtFog:
		break;
	case swtThunder:
		{
			if (!_WeatherHelper->IsLightning())
			{
				_WeatherHelper->StartLightning(_CurWeatherLv);
				_SetWeatherChangeHint(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.HintThunder")));
			}
			//weatherStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.Thunder"));
		}
		break;
	case swtRain:
		{
			if (!_WeatherHelper->IsRaining())
			{
				if (_CurWeatherLv == WL_Light)
				{
					_WeatherHelper->StartRain(WL_Light);
				}
				if (_CurWeatherLv == WL_Showers)
				{
					_WeatherHelper->StartRain(WL_Showers);
					//_WeatherHelper->StartSnow(WL_Light);
				}
				else
				{
					_WeatherHelper->StartRain(WL_Heavy);
					//_WeatherHelper->StartLightning(WL_Light);
				}
				_SetWeatherChangeHint(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.HintRain")));
			}
		}
		break;
	case swtSnow:
		{
			if (!_WeatherHelper->IsSnowing())
			{
				_WeatherHelper->StartSnow(_CurWeatherLv);
				_SetWeatherChangeHint(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.HintSnow")));
			}
		}
		break;
	case swtStorm:
		{
			if (!_WeatherHelper->IsStorming())
			{
				_WeatherHelper->StartStorm(_CurWeatherLv);
				_SetWeatherChangeHint(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.HintStorm")));
			}
		}
		break;
	case swtEclipse:
		break;
	}

	MMOGameEvent_Any evt(GameEvent_UpdateWeather);
	MMOUI::PostGameEvent(&evt);

	_UpdateWeatherTips();
	_ActivateWeatherEvent();
}

void MMOWeatherManager::StopWeather(UINT8 weaterType)
{
	if (!_WeatherHelper)
	{
		return;
	}

	switch (_CurWeatherType)
	{
	case swtFog:
		break;
	case swtThunder:
		_WeatherHelper->StopLightning();
		break;
	case swtRain:
		{
			_WeatherHelper->StopRain();
			if (_CurWeatherLv == WL_Showers)
			{
				_WeatherHelper->StopSnow();
			}
			else if (_CurWeatherLv == WL_Heavy)
			{
				_WeatherHelper->StopLightning();
			}
		}
		break;
	case swtSnow:
		_WeatherHelper->StopSnow();
		break;
	case swtStorm:
		_WeatherHelper->StopStorm();
		break;
	case swtEclipse:
		break;
	}

	_CurWeatherType = swtNormal;
	_CurWeatherLv = WL_Light;

	MMOGameEvent_Any evt(GameEvent_UpdateWeather);
	MMOUI::PostGameEvent(&evt);

	_UpdateWeatherTips();
}

void MMOWeatherManager::StopAllWeather()
{
	if (!_WeatherHelper)
	{
		return;
	}

	if (_WeatherHelper->IsRaining())
	{
		_WeatherHelper->StopRain();
	}

	if (_WeatherHelper->IsSnowing())
	{
		_WeatherHelper->StopSnow();
	}

	if (_WeatherHelper->IsStorming())
	{
		_WeatherHelper->StopStorm();
	}

	if (_WeatherHelper->IsLightning())
	{
		_WeatherHelper->StopLightning();
	}

	_CurWeatherType = swtNormal;
	_CurWeatherLv = WL_Light;

	MMOGameEvent_Any evt(GameEvent_UpdateWeather);
	MMOUI::PostGameEvent(&evt);

	_UpdateWeatherTips();
}

void MMOWeatherManager::Enable( UBOOL bEnable )
{
	if ( _WeatherHelper )
	{
		if( bEnable )
			_WeatherHelper->EnableWeather();
		else
			_WeatherHelper->DisableWeather();
	}
}

UINT8 MMOWeatherManager::GetCurWeatherType()
{
	return _CurWeatherType;
}

void MMOWeatherManager::_ActivateWeatherEvent()
{
	if (GWorld && GWorld->CurrentLevel)
	{
		USequence* mainSeq = GWorld->CurrentLevel->GetGameSequence();
		if (mainSeq)
		{
			TArray<USequenceObject*> events;
			mainSeq->FindSeqObjectsByClass(USeqEvent_WeatherChanged::StaticClass(), events);
			for (INT i = 0; i < events.Num(); ++ i)
			{
				USeqEvent_WeatherChanged* weatherChangeEvent = (USeqEvent_WeatherChanged*)(events(i));
				if (weatherChangeEvent)
				{
					weatherChangeEvent->Activated();
				}
			}
		}
	}
}

void MMOWeatherManager::_ActivateDayTimePlanEvent()
{
	if (GWorld && GWorld->CurrentLevel)
	{
		USequence* mainSeq = GWorld->CurrentLevel->GetGameSequence();
		if (mainSeq)
		{
			TArray<USequenceObject*> events;
			mainSeq->FindSeqObjectsByClass(USeqEvent_DayTimeChanged::StaticClass(), events);
			for (INT i = 0; i < events.Num(); ++ i)
			{
				USeqEvent_DayTimeChanged* dayTimeChangeEvent = (USeqEvent_DayTimeChanged*)(events(i));
				if (dayTimeChangeEvent)
				{
					dayTimeChangeEvent->ActivatePlan();
				}
			}
		}
	}
}

void MMOWeatherManager::_ActivateDayTimeSunlightEvent()
{
	if (!_DayTimeHelper)
	{
		return;
	}
	
	if (GWorld && GWorld->CurrentLevel)
	{
		USequence* mainSeq = GWorld->CurrentLevel->GetGameSequence();
		if (mainSeq)
		{
			TArray<USequenceObject*> events;
			mainSeq->FindSeqObjectsByClass(USeqEvent_DayTimeChanged::StaticClass(), events);
			for (INT i = 0; i < events.Num(); ++ i)
			{
				USeqEvent_DayTimeChanged* dayTimeChangeEvent = (USeqEvent_DayTimeChanged*)(events(i));
				if (dayTimeChangeEvent)
				{
					if ((INT)(_CurrentTime) >= 6 && (INT)(_CurrentTime) < 18)
					{
						dayTimeChangeEvent->ActivateDay();
					} 
					else
					{
						dayTimeChangeEvent->ActivateNight();
					}
				}
			}
		}
	}
}

void MMOWeatherManager::_UpdateDayTimeSunlightEvent()
{
	if (!_DayTimeHelper)
	{
		return;
	}

	if (GWorld && GWorld->CurrentLevel)
	{
		USequence* mainSeq = GWorld->CurrentLevel->GetGameSequence();
		if (mainSeq)
		{
			TArray<USequenceObject*> events;
			mainSeq->FindSeqObjectsByClass(USeqEvent_DayTimeChanged::StaticClass(), events);
			for (INT i = 0; i < events.Num(); ++ i)
			{
				USeqEvent_DayTimeChanged* dayTimeChangeEvent = (USeqEvent_DayTimeChanged*)(events(i));
				if (dayTimeChangeEvent)
				{
					if ((INT)(_CurrentTime) == 6)
					{
						dayTimeChangeEvent->ActivateNightToDay();
						_SetWeatherChangeHint(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.HintDay")));
						_UpdateWeatherTips();
					} 
					else if ((INT)(_CurrentTime) == 18)
					{
						dayTimeChangeEvent->ActivateDayToNight();
						_SetWeatherChangeHint(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Weather.HintNight")));
						_UpdateWeatherTips();
					}
				}
			}
		}
	}
}

void MMOWeatherManager::_SetWeatherChangeHint(const FString& content)
{
	if (MMOUI::GetUIManager())
	{
		UMMOUIWeatherChangePanel* weatherChangePanel = (UMMOUIWeatherChangePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_WeatherChange);
		if (weatherChangePanel)
		{
			//weatherChangePanel->SetVisible(TRUE);
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_WeatherChange, TRUE, "");
			weatherChangePanel->SetWeatherChangeTip(content);
		}
	}
}

bool MMOWeatherManager::IsNight() const
{
	if((INT)(_CurrentTime) >= 6 && (INT)(_CurrentTime) < 18)
	{
		return false;
	}

	return true;
}

void MMOWeatherManager::TestDay()
{
	SetCurTime(6.0f);
	_UpdateDayTimeSunlightEvent();
}

void MMOWeatherManager::TestNight()
{
	SetCurTime(18.0f);
	_UpdateDayTimeSunlightEvent();
}
