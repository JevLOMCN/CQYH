#ifndef _HURT_MANAGER_H_
#define _HURT_MANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOGameUIClasses.h"
#include "MMOUtil.h"
#include "MMOFacade.h"

#define MAX_HURT 4000000000
#define Seconds 1.0f
struct HurtData
{
	UINT AllHurts; //total damage
	float HurtTime;//total combat time

	void Reset()
	{
		AllHurts = 0;
		HurtTime = 0.0f;
	}
};

class MMOHurtManager : public SingletonMeyers<MMOHurtManager>, public INativeGameEventHandler
{
private:
	HurtData _HurtData;
	float _TimeTemp;

	DOUBLE _MaxHurt;
public:
	MMOHurtManager();
	virtual ~MMOHurtManager();

	void SetHurtTime(float deltaTime);
	float GetHurtTime();

	void SetAllHurt(UINT hurt);
	UINT GetAllhurt();


	void Init();
	void Reset();
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);
	void RestHurt();

	void Update(float deltaTime);
	void UpdateAllHurt(UINT playerId,INT hurt);
	void CalculateHurtTime(float deltaTime);
	BOOL CheckHurt();

	void ShowScreenMsg();

	void PostUpdateEvent();
};

#endif