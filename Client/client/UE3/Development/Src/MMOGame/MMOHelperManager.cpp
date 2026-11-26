#include "stdafx.h"
#include "MMOHelperManager.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
#include "LinkManager.h"
//#include "MMOScreenMsgMgr.h"
#include "ScreenMsgQueue.h"
#include "MMOGuideManager.h"

using namespace ExternalMessage;

MMOHelperManager::MMOHelperManager()
{
	_CurPlayerShowProgress = 0.0f;
	_CurPlayerShowTime = 0;
}

MMOHelperManager::~MMOHelperManager()
{
}

void MMOHelperManager::Init()
{
	MMOHelperData::getSingleton().Init();
}

void MMOHelperManager::Shutdown()
{
	MMOHelperData::getSingleton().Shutdown();
}

void MMOHelperManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!evt)
	{
		return;
	}
}

void MMOHelperManager::Clear()
{
	MMOHelperData::getSingleton().Clear();
	_CurPlayerShowProgress = 0.0f;
	_CurPlayerShowTime = 0;
}

void MMOHelperManager::Update(FLOAT deltaTime)
{
	CTRL_TICK("MMOHelperManager");

	if (_CurPlayerShowTime > 0)
	{
		_CurPlayerShowProgress += deltaTime;
		if(_CurPlayerShowProgress > 1.0f)
		{
			_CurPlayerShowTime --;
			_CurPlayerShowProgress = 0.0f;
		}

		if (_CurPlayerShowTime == 0)
		{
			OnHidePlayer();
		}
	}
}

void MMOHelperManager::OnShowPlayer(FLOAT duration)
{
	_CurPlayerShowTime = INT(duration + 1.0F);
	_CurPlayerShowProgress = 0.0f;
}

void MMOHelperManager::OnHidePlayer()
{
	_CurPlayerShowTime = 0;
	_CurPlayerShowProgress = 0.0f;

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIHelperPlayerPanel* helperPanel = (UMMOUIHelperPlayerPanel*)MMOUI::FindUIPanelEx(uiManager->Panel_HelperPlayer);
		if (helperPanel)
		{
			helperPanel->Show(FALSE);
		}
	}
}
