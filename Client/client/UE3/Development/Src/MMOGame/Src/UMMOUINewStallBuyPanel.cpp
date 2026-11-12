#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
//#include "StallManager.h"
#include "MMOMsgBoxManager.h"
#include "UStallManager.h"
#include "FriendManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUINewStallBuyPanel)


void UMMOUINewStallBuyPanel::Init()
{
	Super::Init();
	check( EventAdapter == NULL );
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Stall);
	EventAdapter = new TGameEventAdapter<UMMOUINewStallBuyPanel>(this, evts);
}


void UMMOUINewStallBuyPanel::InitUI()
{
	UINT32 playerId = UStallManager::getSingleton().GetStallBuyPlayerId();
	if (playerId > 0)
	{
		AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(playerId));
		if (player)
		{
			eventUpdateStallInfo(player->StallName, playerId);
			eventUpdateFollowState(!UStallManager::getSingleton().IsAttentionStall(player->StallName));
		}
	}

	for( int i = 0 ; i < MAX_STALL_SLOT_COUNT ; ++i )
	{
		UGFxObject* itemObject = GetObject(FString::Printf(TEXT("slot_mc%d"), i));
		if (itemObject)
		{
			itemObject->SetInt("SlotType", ESPType_StallBuy);
			itemObject->SetInt("itemid", i);
			itemObject->SetInt("slotID", i);
			itemObject->SetBool("enabled", true);

			StallItemInfo stallItemInfo;
			if (UStallManager::getSingleton().GetStallBuyItem(i, stallItemInfo))
			{
				UpdateSlot(i, stallItemInfo.ItemId, stallItemInfo.CurDur, stallItemInfo.Price);
			}
			else
			{
				UpdateSlot(i, 0, 0, 0);
			}
		}			
	}
}

void UMMOUINewStallBuyPanel::ClearUI()
{
	for( int i = 0 ; i < MAX_STALL_SLOT_COUNT ; ++i )
	{
		UpdateSlot(i, 0, 0, 0);
	}
}

void UMMOUINewStallBuyPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		InitUI();

		UGFxObject *gfxObj = GFXPlayer->CreateObject(TEXT("Object"));
		gfxObj->SetString(TEXT("WidgetName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.BuyWidgetTitle")));
		eventUpdateStallBuyWidget(gfxObj);
	}
	else
	{
		ClearUI();
	}
}

void UMMOUINewStallBuyPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (evt->GetEventType() == EGameEvent::GameEvent_Stall)
	{
		MMOGameEvent_StallEvent *e = (MMOGameEvent_StallEvent*)evt;
		if(e->EventType == MMOGameEvent_StallEvent::ESET_StallBuyItemChange)
		{
			INT slot = e->Paramter;
			StallItemInfo stallItemInfo;
			if (UStallManager::getSingleton().GetStallBuyItem(slot, stallItemInfo))
			{
				UpdateSlot(slot, stallItemInfo.ItemId, stallItemInfo.CurDur, stallItemInfo.Price);
			}
			else
			{
				UpdateSlot(slot, 0, 0, 0);
			}
		}
	}
}

void UMMOUINewStallBuyPanel::UpdateSlot(INT stallSlot, INT itemId, INT count, INT price)
{
	UGFxObject* itemObject = GetObject(FString::Printf(TEXT("slot_mc%d"), stallSlot));
	if (!itemObject)
	{
		return;
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate( itemId );

	FString itemName = TEXT("");
	FString itemCount = TEXT("");
	FString itemPrice = TEXT("");
	FString itemTotalPrice = TEXT("");
	if (!item || count <= 0)
	{
		GFXPlayer->SetItemSlotData(itemObject, NULL, ESPType_StallBuy, E_CONSLOT_STALL_BUY, stallSlot);
	}
	else 
	{
		itemName = item->GetDisplayName();
		itemCount = appItoa(count);
		itemPrice = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, price);
		itemTotalPrice = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, price * count);

		GFXPlayer->SetItemSlotData(itemObject, item, ESPType_StallBuy, E_CONSLOT_STALL_BUY, stallSlot);
	}

	UGFxObject* itemNameObject = itemObject->GetObject(TEXT("tF_ItemName"));
	if (itemNameObject)
	{
		itemNameObject->SetString(TEXT("htmlText"), itemName);
	}

	UGFxObject* itemCountObject = itemObject->GetObject(TEXT("valueAmount"));
	if (itemCountObject)
	{
		itemCountObject->SetString(TEXT("htmlText"), itemCount);
	}

	UGFxObject* itemDescObject = itemObject->GetObject(TEXT("tf_dec"));
	if (itemDescObject)
	{
		itemDescObject->SetString(TEXT("text"), itemPrice.IsEmpty() ? TEXT("") : TEXT("Price"));
	}

	UGFxObject* itemPriceObject = itemObject->GetObject(TEXT("tF_Gold"));
	if (itemPriceObject)
	{
		itemPriceObject->SetString(TEXT("htmlText"), itemPrice);
	}

	UGFxObject* itemTotalDescObject = itemObject->GetObject(TEXT("tf_dec_total"));
	if (itemTotalDescObject)
	{
		itemTotalDescObject->SetString(TEXT("text"), itemTotalPrice.IsEmpty() ? TEXT("") : TEXT("Total Price"));
	}

	UGFxObject* itemTotalPriceObject = itemObject->GetObject(TEXT("tF_Gold_Total"));
	if (itemTotalPriceObject)
	{
		itemTotalPriceObject->SetString(TEXT("htmlText"), itemTotalPrice);
	}	

	eventCheckSettingItem(stallSlot);
}

void UMMOUINewStallBuyPanel::BuyItemReal(int stallSlot)
{
	StallItemInfo stallItemInfo;
	if (!UStallManager::getSingleton().GetStallBuyItem(stallSlot, stallItemInfo))
	{
		return;
	}

	if (stallItemInfo.ItemId <= 0)
	{
		return;
	}

	const UItem* item = MMOFacade::getSingleton().FindItemTemplate(stallItemInfo.ItemId);
	if (!item)
	{
		return;
	}

	BYTE durType = item->ItemDuration;
	int maxCount = 1;
	if (durType == DURTYPE_NONE ||
		durType == DURTYPE_EQUIP ||
		durType == DURTYPE_USE ||
		durType == DURTYPE_PURITY ||
		durType == DURTYPE_DEPOSIT)
	{//equipment or items without durability cannot stack
		if (stallItemInfo.CurDur >= 0)
		{
			maxCount = 1;
		}
	}
	else
	{
		if (stallItemInfo.CurDur > 0)
		{
			maxCount = stallItemInfo.CurDur;
		}
	}

	// Pop-up box
	if (maxCount > 1)
	{
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallBuyMsg, TRUE, "");
		UMMOUINewStallBuyMsgPanel* panel = Cast<UMMOUINewStallBuyMsgPanel>(MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallBuyMsg));
		if (panel)
		{
			panel->SetStallBuyMsgPanelData(stallSlot, 0, 0, stallItemInfo.Price, 1, maxCount);
		}
	}
	else
	{
		ConfirmBuyItem(stallSlot, maxCount);
	}
}

void UMMOUINewStallBuyPanel::BuyItem(INT stallSlot)
{
	StallItemInfo stallItemInfo;
	if (!UStallManager::getSingleton().GetStallBuyItem(stallSlot, stallItemInfo))
	{
		return;
	}

	if (stallItemInfo.ItemId <= 0)
	{
		return;
	}

	const UItem* item = MMOFacade::getSingleton().FindItemTemplate(stallItemInfo.ItemId);
	if (!item)
	{
		return;
	}

	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(stallItemInfo.SrcBag, stallItemInfo.SrcSlot);
	if (!itemInfo)
	{
		return;
	}

	if (itemInfo->m_byIdentified == IDENTIFY_TYPE_LEVEL_UP)
	{// Task #24686
		ConfirmBuyExercise(stallSlot);
	} 
	else
	{
		BuyItemReal(stallSlot);
	}
}

UBOOL UMMOUINewStallBuyPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault) return FALSE;
	return FALSE;
}

UBOOL UMMOUINewStallBuyPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if (bDefault)
	{
		return FALSE;
	}

	if (SlotType == ESPType_StallBuy)
	{
		if (mc)
		{
			_CurSlotId  = mc->GetInt("itemid");
		}
		return TRUE;
	}

	return FALSE;
}

UBOOL UMMOUINewStallBuyPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if (bDefault)
	{
		return FALSE;
	}

	if (SlotType == ESPType_StallBuy)
	{
		if (mc)
		{
			_CurSlotId  = mc->GetInt("itemid");
			BuyItem(_CurSlotId);
		}
		return TRUE;
	}

	return FALSE;
}

UBOOL UMMOUINewStallBuyPanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return FALSE;
}

void UMMOUINewStallBuyPanel::ASOnBuyItem()
{
	if (_CurSlotId >= 0)
	{
		BuyItem(_CurSlotId);
	}
}

void UMMOUINewStallBuyPanel::ASOnCallOwner(INT ownerId)
{
	UINT32 playerId = UStallManager::getSingleton().GetStallBuyPlayerId();
	if (playerId > 0)
	{
		AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(playerId));
		if (player)
		{
			if (FriendManager::getSingleton().IsMyFriend(playerId))
			{
				MMOUI::GetUIManager()->delegateSelectFriendChat(ownerId, player->GetActorName());
			}
			else
			{
				FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.NotFriend"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Stall.NotFriend"));
			}
		}
	}
}

void UMMOUINewStallBuyPanel::ASOnFollowOwner(INT ownerId)
{
	UINT32 playerId = UStallManager::getSingleton().GetStallBuyPlayerId();
	if (playerId > 0)
	{
		AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(playerId));
		if (player)
		{
			UStallManager::getSingleton().AddAttention(player->StallName);

			UMMOUIManager* uiManager = MMOUI::GetUIManager();
			if (uiManager)
			{
				uiManager->delegateUpdateHintText(player);
			}

			eventUpdateFollowState(FALSE);
		}
	}
}

void UMMOUINewStallBuyPanel::ASCancelFollowOwner(INT ownerId)
{
	UINT32 playerId = UStallManager::getSingleton().GetStallBuyPlayerId();
	if (playerId > 0)
	{
		AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(playerId));
		if (player)
		{
			UStallManager::getSingleton().RemoveAttention(player->StallName);

			UMMOUIManager* uiManager = MMOUI::GetUIManager();
			if (uiManager)
			{
				uiManager->delegateUpdateHintText(player);
			}

			eventUpdateFollowState(TRUE);
		}
	}
}

void UMMOUINewStallBuyPanel::ConfirmBuyItem(int slot, int count)
{
	if (_ConfirmBuyBox)
	{
		return;
	}

	StallItemInfo stallItemInfo;
	if (!UStallManager::getSingleton().GetStallBuyItem(slot, stallItemInfo))
	{
		return;
	}

	const UItem* item = MMOFacade::getSingleton().FindItemTemplate(stallItemInfo.ItemId);
	if (!item)
	{
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("AgreeBuyNow");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("RefuseBuyNow");

	FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.BuyItemConfirm")), 
		*UITextParser::HTMLMoneyIconString(MONEYTYPE_02, stallItemInfo.Price * count),
		count, 
		*(item->GetDisplayName()));
	_ConfirmBuyBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, slot, funYes, funNo);
	if (_ConfirmBuyBox)
	{
		_ConfirmBuyBox->SetInt(TEXT("itemdataid"), count);
	}
}

void UMMOUINewStallBuyPanel::AgreeBuyNow(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	INT slot = mc->GetInt(TEXT("iteminfoid"));
	INT count = mc->GetInt(TEXT("itemdataid"));

	StallItemInfo stallItemInfo;
	if (UStallManager::getSingleton().GetStallBuyItem(slot, stallItemInfo))
	{
		CMessageHandler::GetInstance().Send_BuyStallItem(UStallManager::getSingleton().GetStallBuyPlayerId(), slot, count, stallItemInfo.ModifyTime);
	}

	_ConfirmBuyBox = NULL;
}

void UMMOUINewStallBuyPanel::RefuseBuyNow(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	_ConfirmBuyBox = NULL;
}


void UMMOUINewStallBuyPanel::ConfirmBuyExercise(int slot)
{
	if (_ConfirmBuyExerciseBox)
	{
		return;
	}

	StallItemInfo stallItemInfo;
	if (!UStallManager::getSingleton().GetStallBuyItem(slot, stallItemInfo))
	{
		return;
	}

	const UItem* item = MMOFacade::getSingleton().FindItemTemplate(stallItemInfo.ItemId);
	if (!item)
	{
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("AgreeBuyExercise");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("RefuseBuyExercise");

	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.Exercise"));
	_ConfirmBuyExerciseBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, slot, funYes, funNo);
}

void UMMOUINewStallBuyPanel::AgreeBuyExercise(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	INT slot = mc->GetInt(TEXT("iteminfoid"));
	BuyItemReal(slot);

	_ConfirmBuyExerciseBox = NULL;
}

void UMMOUINewStallBuyPanel::RefuseBuyExercise(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	_ConfirmBuyExerciseBox = NULL;
}