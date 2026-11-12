#include "stdafx.h"
#include "message_handler.h"
#include "LinkManager.h"
#include "money_def.h"
#include "MMOHeavenManager.h"
#include "MMOGuideManager.h"
#include "MMOQuestManager.h"
#include "MMOAchievementManager.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

#define  TESTHEAVENDATA 0

void CMessageHandler::Send_HeavenConverter()
{
//	if(MMOHeavenManager::getSingleton().CheckSoultoConvert())
//		return;
//	MMOHeavenManager::getSingleton().SetConvertType(1);
//	C2G_SPIRIT_EXP_CHANGE msg;
//	msg.wMsg = c2g_spirit_exp_change;
//	msg.bIsOnline = TRUE;
//	msg.bUseMoney = TRUE;
//	_Send( &msg, sizeof(msg) );
//
//#if TESTHEAVENDATA
//	static INT nIndex = 1;
//	MMOHeavenManager::getSingleton().NotifyUpdateHeavenExp(nIndex*100,nIndex*50);
//	MMOHeavenManager::getSingleton().NotifyUpdateHeavenLevel(nIndex++);
//#endif
}

void CMessageHandler::Send_HeavenNormalConverter()
{
//	//if(MMOHeavenManager::getSingleton().CheckSoultoConvert())
//	//	return;
//	MMOHeavenManager::getSingleton().SetConvertType(0);
//	C2G_SPIRIT_EXP_CHANGE msg;
//	msg.wMsg = c2g_spirit_exp_change;
//	msg.bIsOnline = TRUE;
//	msg.bUseMoney = FALSE;
//	_Send( &msg, sizeof(msg) );
//
//#if TESTHEAVENDATA
//	static INT nIndex = 1;
//	MMOHeavenManager::getSingleton().NotifyUpdateHeavenExp(nIndex*100,nIndex*50);
//	MMOHeavenManager::getSingleton().NotifyUpdateHeavenLevel(nIndex++);
//#endif
}

void CMessageHandler::Send_OutLineConverter()
{
	//MMOHeavenManager::getSingleton().SetConvertType(2);
	//C2G_SPIRIT_EXP_CHANGE msg;
	//msg.wMsg = c2g_spirit_exp_change;
	//msg.bIsOnline = FALSE;
	//msg.bUseMoney = FALSE;
	//_Send( &msg, sizeof(msg) );

}

void CMessageHandler::Send_PractiseGasOneKey()
{
//	if(MMOHeavenManager::getSingleton().CheckXiuLiantoPractise(0))
//		return;
//	if(MMOHeavenManager::getSingleton().CheckGasBagFull())
//		return;
//
//	MMOHeavenManager::getSingleton().SetGasOperateType(HeavenGasOperateType_Practise);
//	C2G_SPIRIT_ADD_AIR msg;
//	msg.wMsg = c2g_spirit_add_air;
//	msg.bQuick = TRUE;
//	_Send( &msg, sizeof(msg) );
//
//#if TESTHEAVENDATA
//	static INT nIndex = 0;
//	if(nIndex > 4)
//		nIndex = 0;
//	MMOHeavenManager::getSingleton().NotifyUpdatePractiseBtnIndex(nIndex ++);
//#endif
}

void CMessageHandler::Send_NormalPractiseGas(INT nIndex)
{
	if(MMOHeavenManager::getSingleton().CheckXiuLiantoPractise(nIndex))
		return;
	if(MMOHeavenManager::getSingleton().CheckGasBagFull())
		return;
	MMOHeavenManager::getSingleton().SetGasOperateType(HeavenGasOperateType_Practise);

	C2G_SPIRIT_ADD_AIR msg;
	msg.wMsg = c2g_spirit_add_air;
	msg.bQuick = FALSE;
	msg.dwIndex = nIndex - 1;
	_Send( &msg, sizeof(msg) );

#if TESTHEAVENDATA
	static INT nIndex = 0;
	if(nIndex > 4)
		nIndex = 0;
	MMOHeavenManager::getSingleton().NotifyUpdatePractiseBtnIndex(nIndex ++);
#endif
}

void CMessageHandler::Send_BuyGasSlot(INT nContainerID,INT nSlotID)
{
//	if(MMOHeavenManager::getSingleton().CheckMoneytoOpenSlot(nContainerID,nSlotID))
//		return;
//	if(MMOHeavenManager::getSingleton().CheckHeavenLeveltoEquipSlot(nContainerID,nSlotID))
//		return;
//
//	C2G_SPIRIT_BUY_AIR_SLOT msg;
//	msg.wMsg = c2g_spirit_buy_air_slot;
//	msg.nBagType = nContainerID;
//	msg.nCount   = nSlotID;
//	_Send( &msg, sizeof(msg) );
//
//#if TESTHEAVENDATA
//	static INT nIndex = 0;
//	MMOHeavenManager::getSingleton().NotifyOpenGasSlot(nContainerID,nSlotID);
//#endif
}

//One-click merge
void CMessageHandler::Send_MergeGasOneKey(INT destSlotID)
{
	MMOHeavenManager::getSingleton().SetGasOperateType(HeavenGasOperateType_Merge);

	C2G_SPIRIT_MERGE_AIR msg;
	msg.wMsg = c2g_spirit_merge_air;
	msg.nSrcCon = destSlotID;
	_Send( &msg, sizeof(msg) );

#if TESTHEAVENDATA
	static INT nIndex = 1;
	MMOHeavenManager::getSingleton().NotifyUpdateGasChange(nIndex*100,nIndex++,0,destSlotID);
#endif
}

void CMessageHandler::Send_MoveGasItem(INT destSlotID,INT nDestContainerID,INT srcSlotID,INT nSrcContainerID)
{
//	MMOHeavenManager::getSingleton().SetGasOperateType(HeavenGasOperateType_Invalid);
//
//	C2G_SPIRIT_MOVE_AIR msg;
//	msg.wMsg = c2g_spirit_move_air;
//	msg.nSrcBagType = nSrcContainerID;
//	msg.nSrcIndex = srcSlotID;
//	msg.nTarBagType = nDestContainerID;
//	msg.nTarIndex = destSlotID;
//
//	_Send( &msg, sizeof(msg) );
//
//#if TESTHEAVENDATA
//	MMOHeavenManager::getSingleton().NotifyUpdateGasChange(20,2,nDestContainerID,destSlotID);
//	MMOHeavenManager::getSingleton().NotifyUpdateGasChange(0,0,nSrcContainerID,srcSlotID);
//#endif
}

void CMessageHandler::Send_GasExchange(INT nGasID)
{
//	if(MMOHeavenManager::getSingleton().CheckGasBagFull())
//		return;
//	MMOHeavenManager::getSingleton().SetGasOperateType(HeavenGasOperateType_Exchange);
//
//	C2G_SPIRIT_EXCHANGE_AIR msg;
//	msg.wMsg = c2g_spirit_exchange_air;
//	msg.wAirID = nGasID;
//	_Send( &msg, sizeof(msg) );
//
//#if TESTHEAVENDATA
//	MMOHeavenManager::getSingleton().NotifyUpdateGasChange(20,2,0,0);
//#endif
}
void CMessageHandler::Send_HeavenBloodLevel(INT nIndex, INT nTypeIndex)
{
//	C2G_SPIRIT_BRUSH_SLOT msg;
//	msg.wMsg = c2g_spirit_brush_slot;
//	msg.nPage = nTypeIndex;
//	msg.nIndex = nIndex;
//	_Send( &msg, sizeof(msg) );
//
//#if TESTHEAVENDATA
//MMOHeavenManager::getSingleton().NotifyUpdateBloodState(nTypeIndex,nIndex,2);
//#endif
}

void CMessageHandler::Send_EquipGasItem(INT nEquipSlot,INT nSlotIndex,INT nContaineID)
{
	Send_MoveGasItem(nEquipSlot,sasbtEquip,nSlotIndex,nContaineID);
}

void CMessageHandler::Send_UnequipGasItem(INT nEquipSlot,INT nSlotIndex,INT nContaineID)
{
	Send_MoveGasItem(nSlotIndex,nContaineID,nEquipSlot,sasbtEquip);
}

void CMessageHandler::Send_HeavenBloodOpen(INT nPage)
{
	//C2G_SPIRIT_BLOOD_OPEN msg;
	//msg.wMsg = c2g_spirit_blood_open;
	//msg.nPage = nPage;
	//_Send( &msg, sizeof(msg) );
}
//-----------------------------------------------------------------------------------------------------------------

void CMessageHandler::Parse_g2c_sync_spirit(const char* buf, UINT32 size)
{
	G2C_SYNC_SPIRIT* msg = (G2C_SYNC_SPIRIT*)buf;
	if(msg == NULL)
		return;
 	MMOHeavenManager::getSingleton().NotifyUpdateHeavenData((ROLE_DB_SPIRIT_DATA*)msg->szData);
}

void CMessageHandler::Parse_g2c_spirit_exp_change(const char* buf, UINT32 size)
{
	G2C_SPIRIT_EXP_CHANGE* msg = (G2C_SPIRIT_EXP_CHANGE*)buf;
	if(msg == NULL)
		return;
	MMOHeavenManager::getSingleton().NotifyUpdateHeavenExp(msg->nExp);

	if (MMOHeavenManager::getSingleton().GetConvertType() == (INT)HeavenConvertType_JustConvert)
	{
		// Guide quest
		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Soul);
	}
}

void CMessageHandler::Parse_g2c_spirit_up(const char* buf, UINT32 size)
{
	G2C_SPIRIT_UP* msg = (G2C_SPIRIT_UP*)buf;
	if(msg == NULL)
		return;
	MMOHeavenManager::getSingleton().NotifyUpdateHeavenLevel(msg->nLevel);

	// Achievement
	MMOGameEvent_Achievement evtAch(adSpiritLevel, msg->nLevel, 0, 0);
	MMOUI::PostGameEvent(&evtAch);
}

void CMessageHandler::Parse_g2c_spirit_air_change(const char* buf, UINT32 size)
{
	G2C_SPIRIT_AIR_CHANGE* msg = (G2C_SPIRIT_AIR_CHANGE*)buf;
	if(msg == NULL)
		return;
	MMOHeavenManager::getSingleton().NotifyUpdateGasChange(msg->nExp,msg->nAirID,msg->nBagType,msg->nIndex);

	if(MMOHeavenManager::getSingleton().GetGasOperateType() == (INT)HeavenGasOperateType_Practise)
	{
		//Qi training succeeded

		//Newbie guide
		MMOGameEvent_Guide evtGuide(EGUET_OpenGas);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else if(MMOHeavenManager::getSingleton().GetGasOperateType() == (INT)HeavenGasOperateType_Merge)
	{
		//Synthesis succeeded
	}
	else if (MMOHeavenManager::getSingleton().GetGasOperateType() == (INT)HeavenGasOperateType_Invalid)
	{
		//
		if(msg->nExp > 0 && msg->nAirID>0 && msg->nBagType == (INT)sasbtEquip)
		{
			//Guide quest
			MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Heaven);

			//Newbie guide
			MMOGameEvent_Guide evtGuide(EGUET_EquipGas);
			MMOUI::PostGameEvent(&evtGuide);
		}
	}
}

void CMessageHandler::Parse_g2c_spirit_air_levelup(const char* buf, UINT32 size)
{
	G2C_SPIRIT_AIR_LEVELUP* msg = (G2C_SPIRIT_AIR_LEVELUP*)buf;
	if(msg == NULL)
		return;
	MMOHeavenManager::getSingleton().NotifyUpdatePractiseBtnIndex(msg->byLevel);
}

void CMessageHandler::Parse_g2c_spirit_brush_slot(const char* buf, UINT32 size)
{
	G2C_SPIRIT_BRUSH_SLOT* msg = (G2C_SPIRIT_BRUSH_SLOT*)buf;
	if(msg == NULL)
		return;
	MMOHeavenManager::getSingleton().NotifyUpdateBloodState(msg->nPage,msg->nIndex,msg->nNumber);
}
void CMessageHandler::Parse_g2c_spirit_slot_size_change(const char* buf, UINT32 size)
{
	G2C_SPIRIT_SLOT_SIZE_CHANGE* msg = (G2C_SPIRIT_SLOT_SIZE_CHANGE*)buf;
	if(msg == NULL)
		return;
	MMOHeavenManager::getSingleton().NotifyOpenGasSlot(msg->nType,msg->nSize);
	if(msg->nType == (INT)sasbtEquip)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasEquipUnLockSucc")),MsgInfoType_Table,TEXT("MMOGame.Heaven.GasEquipUnLockSucc"));
	}
}
void CMessageHandler::Parse_g2c_spirit_notify_exp_change_time(const char* buf, UINT32 size)
{
	G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME* msg = (G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME*)buf;
	if(msg == NULL)
		return;
	FString sInfo;
	MMOHeavenManager::getSingleton().ParseHeavenError(msg->nResultCode,msg->nRemainTime,0,sInfo);
}
void CMessageHandler::Parse_g2c_spirit_blood_open(const char* buf, UINT32 size)
{
	G2C_SPIRIT_BLOOD_OPEN* msg = (G2C_SPIRIT_BLOOD_OPEN*)buf;
	if(msg == NULL)
		return;
	MMOHeavenManager::getSingleton().NotifyBloodOpen(msg->nPage);
}