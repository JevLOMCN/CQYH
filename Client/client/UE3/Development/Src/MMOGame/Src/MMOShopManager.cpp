#include "stdafx.h"
#include "MMOShopManager.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"


//==========================================================================================================================
MMOShopManager::MMOShopManager()
	: _shopId(-1)
{
}

//==========================================================================================================================
MMOShopManager::~MMOShopManager()
{
}

//==========================================================================================================================
void MMOShopManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	
}

void MMOShopManager::Init()
{
	_shopId = -1;
}

void MMOShopManager::notifyShowShopMessage(BOOL v)
{
	MMOGameEvent_Shop_ShowMessage evt(v);
	MMOUI::PostGameEvent(&evt);
}

void MMOShopManager::notifyUpdateShopMessageMoney(FString const1,FString const2)
{
	MMOGameEvent_Shop_UpdateMessageMoney evt(const1,const2);
	MMOUI::PostGameEvent(&evt);
}