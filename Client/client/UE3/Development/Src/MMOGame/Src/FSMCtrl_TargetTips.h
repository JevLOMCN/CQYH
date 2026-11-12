/**
 *
 * Game state machine (FSM) - target object info state machine
 * Add by jimZhang@sdna.com 2014.5.9 
 * 
 * Used to trigger info that needs to be shown when the mouse picks an object
 * 
 */

#ifndef __FSMCTRL_TARGETTIPS_H__
#define __FSMCTRL_TARGETTIPS_H__
#include "FSMCtrl.h"

struct MMOGameEvent_TargetEvent : public MMOGameEvent
{
public:

	enum ETargetChangeType
	{
		ETCT_DesiredTarget,
		ETCT_CurrentTarget
	};

	MMOGameEvent_TargetEvent(ETargetChangeType evtType, AMMOActor* currentTarget, AMMOActor* desiredTarget = 0)
		: EventType(evtType)
		, CurrentTarget(currentTarget)
		, DesiredTarget(desiredTarget)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_TargetChange;
	}

	ETargetChangeType EventType;
	class AMMOActor* CurrentTarget;
	class AMMOActor* DesiredTarget;

};

//Target object pops up tips

//Target portrait bar-------------------------------------------------------------------------------
class FSMCtrl_TargetInfoHUD: public IFSMCtrl
{
public:
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Portal tips---------------------------------------------------------------------------------
class FSMCtrl_TeleportTips: public IFSMCtrl
{
public:
	FSMCtrl_TeleportTips();
	UBOOL	Update(FLOAT DelTime);
	void	Action();

private:
	UBOOL	IsShowTips;
};

//Doodad tips-------------------------------------------------------------------------------
class FSMCtrl_DoodadTips: public IFSMCtrl
{
public:
	FSMCtrl_DoodadTips();
	UBOOL	Update(FLOAT DelTime);
	void	Action();

private:
	UBOOL	IsShowTips;
};

#endif