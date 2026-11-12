#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
#include "MMOMsgBoxManager.h"
#include "UStallManager.h"

IMPLEMENT_CLASS(UMMOUINewStallMsgPanel)

void UMMOUINewStallMsgPanel::SetStallMsgPanelData(INT stallSlot, INT bag, INT slot, INT price, INT count, INT maxCount, INT tax)
{
	this->_StallSlot = stallSlot;
	this->_SrcBag = bag;
	this->_SrcSlot = slot;
	this->_Price = price;
	this->_Count = count;
	this->_MaxCount = maxCount;
	this->_Tax = tax;
	UpdatePanel();
}

void UMMOUINewStallMsgPanel::UpdatePanel()
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (!object)
	{
		return;
	}

	object->SetInt(TEXT("Price"), this->_Price);
	object->SetInt(TEXT("Count"), this->_Count);
	object->SetInt(TEXT("MaxCount"), this->_MaxCount);
	object->SetString(TEXT("ItemName"), TEXT(""));
	object->SetInt(TEXT("ConSlot"), this->_SrcBag);
	object->SetInt(TEXT("ItemSlot"), this->_SrcSlot);
	object->SetInt(TEXT("StallSlot"), this->_StallSlot);
	object->SetInt(TEXT("TaxRate"), this->_Tax);
	object->SetString(TEXT("TextCancel"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.Cancel")));
	object->SetString(TEXT("TextOk"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.Ok")));

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer && mainPlayer->ItemManager != NULL)
	{
		UItem* item = mainPlayer->ItemManager->GetItemBySlot(ECONTAINER_SLOT(this->_SrcBag), this->_SrcSlot);
		if (item)
		{
			object->SetString(TEXT("ItemName"), item->GetDisplayName());
			UGFxObject* itemSlot = GetObject(TEXT("SellItem"));
			if (GFXPlayer && itemSlot != NULL)
			{
				GFXPlayer->SetItemSlotData(itemSlot, item, ESPType_Bag, ECONTAINER_SLOT(this->_SrcBag), this->_SrcSlot);
			}
		}
	}

	eventCallInitPanel(object);
}

void UMMOUINewStallMsgPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
	}
}

void UMMOUINewStallMsgPanel::ASStallSettingMsgBoxYes(INT stallSlot, INT conSlot, INT itemSlot, INT count, INT price)
{
	
	if(CheckStall(count,price))
	{
		CMessageHandler::GetInstance().Send_AddStallItem(stallSlot, conSlot, itemSlot, count, price); 
		this->Show(FALSE);
	}
	
}

BOOL UMMOUINewStallMsgPanel::CheckStall(INT count, INT price)
{
	FString text;

	if (price < MIN_STALL_PRICE)
	{
		text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.BuyItemPriceError"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return FALSE;
	}

	if (count < 1)
	{
		text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.BuyItemCountError"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return FALSE;
	}

	//Total money owned
	INT64 allStallPrice = UStallManager::getSingleton().GetStallSellInfoPrice();
	INT64 sellPrice = price * count;
	INT64 playerMoney  = MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02);

	if(sellPrice >= MAX_STALL_PRICE)
	{
		text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.BuyItemTotalPriceError"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return FALSE;
	}

	if(allStallPrice + playerMoney + sellPrice  >= MAX_PRICE)
	{
		text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errStallMoneyExceeded"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return FALSE;
	}

	return TRUE;
}

void UMMOUINewStallMsgPanel::ASStallSettingMsgBoxNo()
{
	this->Show(FALSE);
}