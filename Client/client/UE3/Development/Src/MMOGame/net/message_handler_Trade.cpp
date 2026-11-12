#include "stdafx.h"
#include "message_handler.h"
#include "MMOChatManager.h"
#include "MMOFacade.h"
#include "MMOPowerManager.h"

using namespace ExternalMessage;

void CMessageHandler::Send_RequistTrade(UINT32 roleID)
{
	C2G_TRADE_ASK msg;
	msg.wMsg = c2g_trade_ask;
	msg.dwRoleID = roleID;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_AcceptTradeRequist(UINT32 roleID)
{
	C2G_TRADE_ACCEPT msg;
	msg.wMsg = c2g_trade_accept;
	msg.dwRoleID = roleID;
	_Send(&msg, sizeof(msg));

}

void CMessageHandler::Send_TradeSetItem(UINT8 tradeSlot, UINT8 isAdd, UINT16 containerID, UINT16 slotID)
{
	C2G_TRADE_SET_ITEM msg;
	msg.wMsg = c2g_trade_set_item;
	msg.byTradeSlot = tradeSlot;
	msg.bAdd = isAdd;
	msg.byConSlot = containerID;
	msg.byItemSlot = slotID;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_TradeSetMoney(INT32 money)
{
	C2G_TRADE_SET_MONEY msg;
	msg.wMsg = c2g_trade_set_money;
	msg.nMoney = money;
	_Send(&msg, sizeof(msg));

}

void CMessageHandler::Send_TradeLock()
{
	C2G_TRADE_LOCK msg;
	msg.wMsg = c2g_trade_lock;
	_Send(&msg, sizeof(msg));

}

void CMessageHandler::Send_TradeUnLock()
{
	C2G_TRADE_UNLOCK msg;
	msg.wMsg = c2g_trade_unlock;
	_Send(&msg, sizeof(msg));

}

void CMessageHandler::Send_TradeConfirm()
{
	C2G_TRADE_CONFIRM msg;
	msg.wMsg = c2g_trade_confirm;
	_Send(&msg, sizeof(msg));

}

void CMessageHandler::Send_TradeCancel()
{
	C2G_TRADE_CANCEL msg;
	msg.wMsg = c2g_trade_cancel;
	_Send(&msg, sizeof(msg));

}


void CMessageHandler::Parse_g2c_trade_state_change(const char* buf, UINT32 size)
{
	G2C_TRADE_STATE_CHANGE *msg = (G2C_TRADE_STATE_CHANGE*)buf;
	//
	UMMOUITradeP2P *pTradeP2P = (UMMOUITradeP2P*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_TradeP2P );
	//if( pTradeP2P == NULL ){
	//	MMOUI::GetUIManager()->LoadMoviePlayers( MMOUI::GetUIManager()->MoviePlayer_TradeP2P );
	//	pTradeP2P = (UMMOUITradeP2P*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_TradeP2P ); 
	//}

	if( pTradeP2P == NULL ){
		warnf( NAME_Error, TEXT("Panel_TradeP2P not found at Parse_g2c_trade_state_change") );
		return;
	}
	//
	AMMOPlayer *pChara = (AMMOPlayer*)MMOFacade::getSingleton().GetCharacter( msg->dwRoleID );
	//AMMOPlayer *pMe = MMOFacade::getSingleton().GetMainPlayer();
	AMMOPlayerController *pCtrlMe = MMOFacade::getSingleton().GetMainController();
	if( pChara == NULL || pCtrlMe == NULL ){
		return;
	}
	//
	switch( msg->byTradeState )
	{
	//Trade requested
	case etsBeingAsked:
		{
			//
			if( pChara->IsMainPlayer() )
			{
				if (MMOUI::GetUIManager()->DeclineShowTrade)
				{
					Send_TradeCancel();
					break;
				}

				//Self was requested
				pTradeP2P->NotifyApplyDoTrade( pCtrlMe->TradeTargetID );
				pTradeP2P->MyTradeState = etsBeingAsked;
			}
			else{
				pCtrlMe->TradeTargetID = msg->dwRoleID;
				if( pChara->ActorAttributes != NULL ){
					pChara->ActorAttributes->LevelAttributes.Level = msg->nRoleLevel;
				}
			}
		}
		break;
	//Waiting for request
	case etsAsking:
		{
			//
			if( !pChara->IsMainPlayer() ){
				pCtrlMe->TradeTargetID = msg->dwRoleID;
				if( pChara->ActorAttributes != NULL ){
					pChara->ActorAttributes->LevelAttributes.Level = msg->nRoleLevel;
				}
			}
			else{
				pTradeP2P->MyTradeState = etsAsking;
			}
		}
		break;
	//Trade accepted
	case etsTrading:
		{
			//pChara->LifeState = eDoTradeP2P;
			if( pChara->IsMainPlayer() ){
				//Self
				AMMOPlayer *pChara = (AMMOPlayer*)MMOFacade::getSingleton().GetCharacter( pCtrlMe->TradeTargetID );
				if( pChara != NULL && pTradeP2P->MyTradeState == etsAsking ){
					//hint in chat channel for trade apply be agreed
					FString hint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.TradeP2P.TradeMsg5");
					MMOChatManager::getSingleton().AddSystemMessage( FString::Printf( *hint, *pChara->GetActorName() ) );
				}
				//
				if( etsBeingAsked == pTradeP2P->MyTradeState || etsAsking == pTradeP2P->MyTradeState ){
					//begin to do deal
					INT targetLv = 0;
					if( pChara != NULL && pChara->ActorAttributes != NULL ){
						targetLv = pChara->ActorAttributes->LevelAttributes.Level;
					}
					pTradeP2P->DoTrade( pCtrlMe->TradeTargetID, targetLv );
				}
				else if( etsLocked == pTradeP2P->MyTradeState || etsConfirmed == pTradeP2P->MyTradeState ){
					//back to unlock
					pTradeP2P->NotifyLockP2PTrade( TRUE, FALSE );
				}
				//
				pTradeP2P->MyTradeState = etsTrading;
			}
			else{
				if( pChara->ActorAttributes != NULL ){
					pChara->ActorAttributes->LevelAttributes.Level = msg->nRoleLevel;
				}
				//others, in this state always means unlock
				pTradeP2P->NotifyTargetTradeLockState( FALSE );
			}
		}
		break;
	//Lock
	case etsLocked:
		{
			//pChara->LifeState = eLockTradeP2P;
			if( pChara->IsMainPlayer() ){
				pTradeP2P->MyTradeState = etsLocked;
			}
			pTradeP2P->NotifyLockP2PTrade( pChara->IsMainPlayer(), TRUE );
		}
		break;
	//Confirm trade
	case etsConfirmed:
		{
			if( pChara->IsMainPlayer() ){
				pTradeP2P->MyTradeState = etsConfirmed;
				//
				FString hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TradeP2P.TradeMsgStateMyConfirm") );
				MMOChatManager::getSingleton().AddSystemMessage( hint );
				MMOUI::GetUIManager()->delegateAddScreenMessage( hint ,MsgInfoType_Table,TEXT("MMOGame.TradeP2P.TradeMsgStateMyConfirm"));
			}
			else{
				FString hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TradeP2P.TradeMsgStateOtherConfirm") );
				MMOChatManager::getSingleton().AddSystemMessage( hint );
				MMOUI::GetUIManager()->delegateAddScreenMessage( hint ,MsgInfoType_Table,TEXT("MMOGame.TradeP2P.TradeMsgStateOtherConfirm"));
			}
		}
		break;
	//Trade succeeded
	case etsSuccess:
		{
			if( pChara->IsMainPlayer() ){
				if( pCtrlMe->TradeTargetID != 0 ){
					//Own trade succeeded
					pTradeP2P->NotifyTradeResult( FALSE );
					pCtrlMe->TradeTargetID = 0;
				}
			}
			else{
				//pChara->LifeState = eDoingNoting;
				//pChara->TradeTargetID = 0;
			}
		}
		break;
	//Trade failed
	case etsFree:
		{
			pChara->SetInTrading( FALSE );
			//
			if( pChara->IsMainPlayer() ){
				if( pCtrlMe->TradeTargetID != 0 ){
					//Own trade ended (failed)
					pTradeP2P->NotifyTradeResult( TRUE );
					//
					pCtrlMe->TradeTargetID = 0;
				}
			}
			else{
				//pChara->LifeState = eDoingNoting;
				//pChara->TradeTargetID = 0;
			}
		}
		break;
	default:
		break;
	}
}

void CMessageHandler::Parse_g2c_trade_set_item(const char* buf, UINT32 size)
{
	G2C_TRADE_SET_ITEM *msg = (G2C_TRADE_SET_ITEM*)buf;
	//
	UMMOUITradeP2P *pTradeP2P = (UMMOUITradeP2P*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_TradeP2P );
	if( pTradeP2P == NULL ){
		return;
	}
	//
	AMMOActor *pChara = MMOFacade::getSingleton().GetCharacter( msg->dwRoleID );
	if( pChara == NULL ){
		return;
	}
	//
	bool isMy = pChara->IsMainPlayer() == TRUE;
	ItemInfo iteminfo;
    iteminfo.Load(msg->acItem);
	iteminfo.m_byConSlot = isMy ? CONSLOT_P2P_MY : CONSLOT_P2P_TG;
	iteminfo.m_byItemSlot = msg->byTradeSlot;
	//
	if( msg->bAdd ){
		MMOFacade::getSingleton().AddItem( &iteminfo );
		FClientItemInfo *addItemInfo = MMOFacade::getSingleton().FindItem( iteminfo.m_byConSlot, iteminfo.m_byItemSlot );
		MMOPowerManager::getSingleton().UpdateEquipPower( addItemInfo );

		pTradeP2P->RealAddTradeItem( iteminfo.m_byConSlot, iteminfo.m_byItemSlot, iteminfo.m_nTemplateID );
	}
	else{
		pTradeP2P->RealDelTradeItem( isMy, msg->byTradeSlot );
	}
	
}

void CMessageHandler::Parse_g2c_trade_set_money(const char* buf, UINT32 size)
{
	G2C_TRADE_SET_MONEY *msg = (G2C_TRADE_SET_MONEY*)buf;
	//
	UMMOUITradeP2P *pTradeP2P = (UMMOUITradeP2P*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_TradeP2P );
	if( pTradeP2P == NULL ){
		return;
	}
	//
	AMMOActor *pChara = MMOFacade::getSingleton().GetCharacter( msg->dwRoleID );
	if( pChara == NULL ){
		return;
	}
	//
	pTradeP2P->NotifyTradeMoneyChanged( pChara->IsMainPlayer(), msg->nMoney );
}
