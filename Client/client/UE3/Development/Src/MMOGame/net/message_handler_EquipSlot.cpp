#include "stdafx.h"
#include "net/message_handler.h"
#include "MMOFacade.h"
#include "EquipSlotManager.h"

using namespace ExternalMessage;


void CMessageHandler::Send_Refine( UINT8 refineSlot, UINT8 hasConsume, UINT8 consumeConID,  UINT8 consumeSlotID )
{
	//C2G_REFINE msg;
	//msg.wMsg = c2g_refine;

	//msg.byRefineSlot = refineSlot;

	//msg.byAddEquip = hasConsume;
	//msg.byConSlot = consumeConID;
	//msg.byItemSlot = consumeSlotID;
	//
	//_Send(&msg,sizeof(msg));
}

void CMessageHandler::Parse_g2c_refine(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_REFINE))
		return;
	G2C_REFINE* msg = (G2C_REFINE*)buf;
	MMOGameEvent_EquipSlotEnhanceResult event( msg->bySlot, msg->byResult, msg->wRefineLevel );
	MMOUI::PostGameEvent(&event);
}

void CMessageHandler::Parse_g2c_sync_refine(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_SYNC_REFINE))
		return;
	G2C_SYNC_REFINE* msg = (G2C_SYNC_REFINE*)buf;
	EquipSlotManager::getSingleton().SyncEquipSlotLevel(msg->awRefineLevel);
}