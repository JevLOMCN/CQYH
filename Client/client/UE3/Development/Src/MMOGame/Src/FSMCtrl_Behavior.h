/**
 *
 * Game state machine (FSM) - character basic behavior state machine
 * Add by jimZhang@sdna.com 2014.5.7 
 * 
 * Used to drive some more atomic-level behaviors, such as moving to a point, moving to an actor, attacking a target, etc.
 *
 */

#ifndef __FSMCTRL_BEHAVIOR_H__
#define __FSMCTRL_BEHAVIOR_H__
#include "FSMCtrl.h"

//Auto cast----------------------------------------------------
class FSMCtrl_AiAbility: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};
//Shift key use ability in place-----------------------------------------------------------------------------------------------------
class FSMCtrl_ShiftAbility: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Cast ability in place------------------------------------------------------------------------------------------------------------
class FSMCtrl_StayAndUseAbility: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Walk to target and cast ability------------------------------------------------------------------------------------------------------
class FSMCtrl_MoveToAndUseAbility: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Walk close and interact--------------------------------------------------------------------------------------------------------------
class FSMCtrl_MoveToAndInteract: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Walk to target point, with mouse held, fast control of the target point to walk to----------------------------------------------------------------------
class FSMCtrl_MoveToPoint: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Focus activation------------------------------
class FSMCtrl_FocusActor: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Test------------------------------
class FSMCtrl_Test: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
private:
	FLOAT	m_DelTime;
};




#endif