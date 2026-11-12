
#include "stdafx.h"
#include "MallController.h"
#include "MMOFacade.h"
#include "MMOGameClasses.h"
#include "net/message_handler.h"
#include "CommonGameEvents.h"
#include "MallManager.h"

MallController::MallController()
{
	bAskGoodIng = FALSE;
	nShowGoodsID = 0;
	nOperateCode = eMallBuyValid;
	nGoodsNum = 0;
}
void MallController::OnNativeGameEvent(struct MMOGameEvent* evt)
{

}
void MallController::AddPage()
{
	INT nNewPage  = MallManager::getSingleton().GetPage();
	INT nMaxPage  = MallManager::getSingleton().GetMaxPage();
	nNewPage++;
	if(nNewPage >= 0 && nNewPage < nMaxPage)
	{
		MallManager::getSingleton().SetPage(nNewPage);
		NotifyPageChange();
	}
}
void MallController::DecPage()
{
	INT nNewPage  = MallManager::getSingleton().GetPage();
	INT nMaxPage  = MallManager::getSingleton().GetMaxPage();
	nNewPage--;
	if(nNewPage >= 0 && nNewPage < nMaxPage)
	{
		MallManager::getSingleton().SetPage(nNewPage);
		NotifyPageChange();
	}
}
void MallController::SetSortType(INT nSortType)
{
	MallManager::getSingleton().SetGoodsType(nSortType);
	NotifyGoodsListChange();
}
void MallController::SetShowType(INT nShowType)
{
	MallManager::getSingleton().SetGoodsToMainPlayerType(nShowType);
	NotifyGoodsListChange();
}
void MallController::NotifyPageChange()
{
	MMOGameEvent_MallStore evt( MMOGameEvent_MallStore::EMallStoreEvent_PageChange);
	MMOUI::PostGameEvent(&evt);
}
void MallController::NotifyRealMoneyChange()
{
	MMOGameEvent_MallStore evt( MMOGameEvent_MallStore::EMallStoreEvent_GoldChange);
	MMOUI::PostGameEvent(&evt);
}
void MallController::NotifyGoodsListChange()
{
	if(bAskGoodIng)
	{
		bAskGoodIng = FALSE;
		ShowGoodsInfo();
	}
	MMOGameEvent_MallStore evt( MMOGameEvent_MallStore::EMallStoreEvent_TypeChange);
	MMOUI::PostGameEvent(&evt);
}
void MallController::NotifyBuyResult(INT nGoodsID)
{
	NotifyRealMoneyChange();
	UItem* pkItem = MMOFacade::getSingleton().FindItemTemplate( nGoodsID );
	if(pkItem == 0)
		return;
	//Whether the bag can hold it
	if(nOperateCode == eMallBuytoSelf)
	{
		FString strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.BuySuccessHint")),*(pkItem->GetDisplayName()));
		MMOUI::GetUIManager()->delegateAddScreenMessage(strHint,MsgInfoType_Table,TEXT("MMOGame.Mall.BuySuccessHint"));
	}
	else if (nOperateCode == eMallBuytoFriend)
	{
		FString strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.GiveSuccessHint")),*(pkItem->GetDisplayName()));
		MMOUI::GetUIManager()->delegateAddScreenMessage(strHint,MsgInfoType_Table,TEXT("MMOGame.Mall.GiveSuccessHint"));
	}
}
void MallController::NotifyBuyInfoChange()
{
	MMOGameEvent_MallStore evt( MMOGameEvent_MallStore::EMallStoreEvent_BuyInfoChange);
	MMOUI::PostGameEvent(&evt);
}
void MallController::NotifyReqBuyInfoChange()
{
	MMOGameEvent_MallStore evt( MMOGameEvent_MallStore::EMallStoreEvent_NeedReqBuyInfo);
	MMOUI::PostGameEvent(&evt);
}
void MallController::SendMallRealMoney()
{
	CMessageHandler::GetInstance().Send_MallAskRealMoney();
}
void MallController::SendMallGoodsList()
{	
	CMessageHandler::GetInstance().Send_MallAskCRC(MallManager::getSingleton().GetMallCrc());
}
void MallController::SendBuyGoods(INT nGoodsID,INT nNum)
{
	if(CheckEnoughGold(nGoodsID,nNum))
	{
		nOperateCode = eMallBuytoSelf;
		nGoodsNum = nNum;
		CMessageHandler::GetInstance().Send_MallBuyGoods(nGoodsID,nNum);
	}
}
void MallController::SendGiveFriendGoods(FString roleName, INT nGoodsID,INT nNum)
{
	if(CheckEnoughGold(nGoodsID,nNum))
	{
		nOperateCode = eMallBuytoFriend;
		nGoodsNum = nNum;
		CMessageHandler::GetInstance().Send_MallGiveFriend(roleName,nGoodsID,nNum);
	}
}
void MallController::SendReqUpdateBuyInfo()
{
	CMessageHandler::GetInstance().Send_MallReqBuyInfo();
}
void MallController::PopupGoodsBuyPanel(INT nGoodsID)
{
	nShowGoodsID = nGoodsID;
	const REAL_MONEY_ITEM_INFO* pkGood = MallManager::getSingleton().GetGoodsByID(nShowGoodsID);
	if(pkGood == NULL)
	{
		bAskGoodIng = TRUE;
		SendMallRealMoney();
		SendMallGoodsList();
	}
	else
		ShowGoodsInfo();
}
void MallController::ShowGoodsInfo()
{
	const REAL_MONEY_ITEM_INFO* pkGood = MallManager::getSingleton().GetGoodsByID(nShowGoodsID);
	if(pkGood != NULL)
	{
		UMMOUIMallPanel* panel = Cast<UMMOUIMallPanel>(MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Mall));
		if(panel)
		{
			panel->ASClickMallGoods(nShowGoodsID);
		}
		else
		{
			warnf(NAME_Error, TEXT("Mall panel not found£¬ please fix this bug!"));
		}
// 		UMMOGFXPlayer_Mall* pkMovie = (UMMOGFXPlayer_Mall*)MMOUI::GetUIManager()->LoadMoviePlayers( MMOUI::GetUIManager()->MoviePlayer_Mall);
// 		if(pkMovie)
// 		{
// 			pkMovie->ASClickMallGoods(nShowGoodsID);
// 		}
	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.NoFindGoodsInfo")),MsgInfoType_Table,TEXT("MMOGame.Mall.NoFindGoodsInfo"));
	}

	nShowGoodsID = 0;
}
BOOL MallController::CheckEnoughGold(INT nGoodsID, INT nNum)
{
	return TRUE;
}