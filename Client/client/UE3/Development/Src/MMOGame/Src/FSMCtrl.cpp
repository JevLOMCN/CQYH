

#include "StdAfx.h"
#include "FSMState.h"
#include "FSMCtrl.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMONavigationManager.h"
#include "ScaleformEngine.h"
#include "UITextParser.h"

using namespace FSM;

//Register to FSMCtrlManager on creation
IFSMCtrl::IFSMCtrl(void)
{
	FSMCtrlManager::getSingleton().AddFSMCtrl(this);
}

//-------------------------------------------------------------------------------------
void FSMCtrlManager::AddFSMCtrl(IFSMCtrl* pFSMCtrl)
{
	if(pFSMCtrl)
	{
		IFSMCtrlList.push_back(pFSMCtrl);
	}
}
void FSMCtrlManager::Update(FLOAT DelTime)
{
	CTRL_TICK("FSMCtrlManager");

	if(MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}

	FSMStateManager::getSingleton().Update(DelTime);

	for( FSMCtrlIterator it = IFSMCtrlList.begin();	it!=IFSMCtrlList.end();	it++)
	{
		IFSMCtrl* pFSMCtrl = *it;
		if(pFSMCtrl && pFSMCtrl->Update(DelTime))
		{
			pFSMCtrl->Action();
		}
	}
}


