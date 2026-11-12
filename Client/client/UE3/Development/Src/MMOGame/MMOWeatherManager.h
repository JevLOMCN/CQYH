#ifndef _MMOWEATHERMANAGER_H_
#define _MMOWEATHERMANAGER_H_

#include "MMOSingleton.h"
#include "MMOFacade.h"

// weather level
enum WeatherLevel
{
	WL_Light = 0,										// light rain, light snow, intermittent lightning, little blowing sand, light fog
	WL_Showers,											// moderate rain + lightning, moderate snow, moderate sandstorm, moderate fog
	WL_Heavy,											// rain + snow, heavy snow, continuous lightning, blowing sand + tornado, heavy fog
	WL_Total
};

#define DAY_TIME_MATINEE_DURATION	2.0

class MMOWeatherManager : public SingletonMeyers<MMOWeatherManager>, public INativeGameEventHandler
{
public:
	MMOWeatherManager();
	virtual ~MMOWeatherManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void InitStartTime(UINT32 startTime);				// initialize game start time
	void EnterScene(UINT8 weaterType);					// enter map
		
	void StartWeather(UINT8 weaterType);				// start weather
	void StopWeather(UINT8 weaterType);					// stop weather
	void StopAllWeather();								// stop all weather

	FLOAT GetCurTime() const;							// get current time, 24-hour format, unit: hours
	void SetCurTime(FLOAT t) { _CurrentTime = t; }
	UINT8 GetCurWeatherType();							// get current weather type

	void Enable( UBOOL bEnable );						// enable/disable weather system

	bool IsNight() const;
	void TestDay();
	void TestNight();

	void SetFire( const FVector& Center, FLOAT Radius )
	{
		if( _WeatherHelper )
			_WeatherHelper->SetFire( Center, Radius );
	}
		
protected:
	void _ActivateWeatherEvent();						// trigger Kismet weather event
	void _ActivateDayTimePlanEvent();					// trigger Kismet schedule event
	void _ActivateDayTimeSunlightEvent();				// trigger Kismet day/night change event
	void _UpdateDayTimeSunlightEvent();					// trigger Kismet day/night change event

	void _SetWeatherChangeHint(const FString& content);
	void _UpdateWeatherTips();

	AMMOWeatherHelper* _WeatherHelper;
	AMMODayTimeHelper* _DayTimeHelper;

	FLOAT _StartTime;									// game start time, 24-hour format, unit: hours
	FLOAT _HourPerDay;
	FLOAT _StartTotalTime;
	FLOAT _CurrentTime;

	FLOAT _AnimTime;									// animation update time
	FLOAT _OldEventTime;								// event trigger time

	UINT8 _CurWeatherType;								// current weather type
	UINT8 _CurWeatherLv;								// current weather level
};

#endif