#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "EquipDecomposeManager.h"
#include "MMOQuestManager.h"
#include "MMOGuideManager.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

void CMessageHandler::Send_Decompose(UINT8 byConSlot, UINT8 byItemSlot)
{
	C2G_DECOMPOSE msg;
	msg.byConSlot = byConSlot;
	msg.byItemSlot = byItemSlot;
	msg.wMsg = c2g_decompose;
	_Send(&msg,sizeof(msg));
}

void CMessageHandler::Send_Compose(UINT32 iFomulaId)
{
	C2G_COMPOSE msg;
	msg.nFormulaID = iFomulaId;
	msg.wMsg = c2g_compose;
	_Send(&msg,sizeof(msg));
}

void CMessageHandler::Parse_g2c_decompose(const char* buf, UINT32 size)
{
	G2C_DECOMPOSE* msg = (G2C_DECOMPOSE*)buf;
	if(!msg)
	{
		return;
	}

	if( msg->byResult )
	{
		TArray<DecomposeResult>& ret_item = EquipDecomposeManager::getSingleton().GetDecomposeResult();
		ret_item.Empty();
		for(INT i = 0; i < MAX_DECOMPOSE_ITEM_COUNT; ++i)
		{
			if(msg->nItemID[i] > 0)
			{
				DecomposeResult DItem;
				DItem.itemId = msg->nItemID[i];
				DItem.cnt = msg->nItemCount[i];
				ret_item.AddItem(DItem);
			}
		}
	}

	if (msg->byResult)
	{
		// Guide quest
		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Decompose, msg->nOriginalItemID);

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_DecompEquip);
		MMOUI::PostGameEvent(&evtGuide);
	}

	MMOGameEvent_EquipMaker equipDcompEvt(MMOGameEvent_EquipMaker::EEDT_DecomposeSuccess);
	equipDcompEvt.IntRst = msg->byResult;
	MMOUI::PostGameEvent(&equipDcompEvt);
}

void CMessageHandler::Parse_g2c_compose(const char* buf, UINT32 size)
{
	G2C_COMPOSE* msg = (G2C_COMPOSE*)buf;
	MMOGameEvent_EquipMaker equipCompEvt(MMOGameEvent_EquipMaker::EEDT_ComposeSuccess);
	equipCompEvt.IntRst = msg->byResult;
	MMOUI::PostGameEvent(&equipCompEvt);

	if (msg->byResult)
	{
		// Guide quest
		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Compose, msg->nResultItemID);
	
		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_CompEquip);
		MMOUI::PostGameEvent(&evtGuide);
	}
}