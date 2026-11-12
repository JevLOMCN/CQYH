/**
 *
 * Game state machine (FSM) - character AI behavior state machine
 * Add by jimZhang@sdna.com 2014.5.9 
 * 
 * Used to drive some more complex behavior logic, such as continuous auto monster fighting, Attack to a location, etc.
 * 
 */

#ifndef __FSMCTRL_AI_H__
#define __FSMCTRL_AI_H__
#include "FSMCtrl.h"

//Keep Running----------------------------------------------------
class FSMCtrl_KeepRunning: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Auto doodad----------------------------------------------------
class FSMCtrl_SearchDoodad: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};


//Enter/leave safe zone performance Ai----------------------------------------------------
class FSMCtrl_AIInSafeZone: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Quick view player info----------------------------------------------------
class FSMCtrl_ShowPlayerInfo: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
};

//Player overhead name button----------------------------------------------------
class FSMCtrl_ShowHintBtn: public IFSMCtrl
{
public:	
	UBOOL	Update(FLOAT DelTime);
	void	Action();
private:
	UBOOL   _bShowHintBtn;
};

#endif