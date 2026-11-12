/**
 *
 * Game finite state machine (FSM) - control set
 * jimZhang@sdna.com 2014.5.6 
 * 
 * Doule-Level Extensible Finite State Machine
 * Two-layer extensible finite state machine
 *  
 * 1. Bottom-layer states are limited common base states; bottom-layer states can be input to upper-layer states. Same-layer states cannot be input.
 * 2. Upper-layer states are specific states and cannot be input to bottom-layer states or same-layer side branches.
 * 3. State refresh has two modes, pull type and trigger type, due to different storage (self-stored and other-stored).
 * 4. Trigger-type state refresh provides an extensible interface; state update depends only on its own stored flags.
 * 5. State decision modules can use bottom-layer and upper-layer states as input.
 * 6. State execution modules are called directly only by the state decision module.
 *
 */

#ifndef __FSMCTRL_H__
#define __FSMCTRL_H__

#include "stdafx.h"
#include <list>
#include <vector>
#include <map>
#include "MMOSingleton.h"

using namespace std;

class AActor;
class UMMOAbility;
class AMMOPlayerController;

#define ADD_FSM_CTRL(ctrl) ctrl g_##ctrl##_Unvisible_Instance;


class IFSMCtrl
{
public:
	IFSMCtrl();
	virtual UBOOL Update(FLOAT DelTime) = 0;
	virtual void Action() = 0;

};


class FSMCtrlManager : public SingletonMeyers<FSMCtrlManager>
{
private:
	std::vector<IFSMCtrl*> IFSMCtrlList;
	typedef std::vector<IFSMCtrl*>::iterator FSMCtrlIterator;
public:
	void AddFSMCtrl(IFSMCtrl* pFSMCtrl);
	void Update(FLOAT DelTime);

};

#endif