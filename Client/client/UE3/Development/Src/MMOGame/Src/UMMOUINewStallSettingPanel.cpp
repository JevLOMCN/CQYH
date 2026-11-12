#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
//#include "StallManager.h"
#include "UStallManager.h"
#include "ClientActionConditionCheck.h"
#include "MMOMsgBoxManager.h"
#include "UStallManager.h"
#include "UITextParser.h"
#include "UILayout.h"

IMPLEMENT_CLASS(UMMOUINewStallSettingPanel)

void UMMOUINewStallSettingPanel::Init()
{
	Super::Init();

	check( EventAdapter == NULL );
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Stall);
	EventAdapter = new TGameEventAdapter<UMMOUINewStallSettingPanel>(this, evts);

	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}

void UMMOUINewStallSettingPanel::UnInit()
{
	Super::UnInit();
}

void UMMOUINewStallSettingPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		RefreshButton();
		RefreshMoney();
		RefreshNameAndAdd();

		UpdateStallUpgradeLeftTime();
		UpdateStallRecordList();

		UGFxObject* object = GFXPlayer->CreateObject("Object");
		if (object)
		{
			object->SetString(TEXT("WidgetName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.SettingWidgetTitle")));
			eventUpdateStallSettingWidget(object);
		}

		eventFocusInput();
		InitUI();
	}
	else
	{
		ClearUI();
	}
}

void UMMOUINewStallSettingPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (evt && evt->GetEventType() == EGameEvent::GameEvent_Stall)
	{
		MMOGameEvent_StallEvent* stallEvent = (MMOGameEvent_StallEvent*)evt;
		if (stallEvent)
		{
			switch (stallEvent->EventType)
			{
			case MMOGameEvent_StallEvent::ESET_MoneyChange:
				RefreshMoney();
				break;

			case MMOGameEvent_StallEvent::ESET_StateChange:
				{
					if (MMOFacade::getSingleton().IsSelf(stallEvent->Paramter) 
						&& MMOFacade::getSingleton().GetMainPlayer()->MoveType >= EMT_PrepareStall)
					{
						RefreshButton();
						UpdateStallUpgradeLeftTime();
						RefreshNameAndAdd();
					}
				}
				break;

			case MMOGameEvent_StallEvent::ESET_StallSettingItemChange:
				{
					INT slot = stallEvent->Paramter;
					StallItemInfo stallItemInfo;
					if (UStallManager::getSingleton().GetStallSellItem(slot, stallItemInfo))
					{
						UpdateItemSlot(slot, stallItemInfo.SrcBag, stallItemInfo.SrcSlot, stallItemInfo.CurDur, stallItemInfo.Price);
					}
					else
					{
						UpdateItemSlot(slot, -1, -1, 0, 0);
					}

					RefreshMoney();
				}
				break;

			case MMOGameEvent_StallEvent::ESET_SyncStall:
				{
					UpdateStallUpgradeLeftTime();
				}
				break;
			}
		}
	}
}

void UMMOUINewStallSettingPanel::ClearUI()
{
	for (INT i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		UpdateItemSlot(i, -1, -1, 0, 0);
	}
}

void UMMOUINewStallSettingPanel::InitUI()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UGFxObject* itemSettingObject = GetObject(TEXT("itemSetting"));
	if (!itemSettingObject)
	{
		return;
	}

	for (INT i = 0; i < MAX_STALL_SLOT_COUNT; ++i)
	{
		UGFxObject* itemObject = itemSettingObject->GetObject(FString::Printf(TEXT("slot_mc%d"), i));
		if (!itemObject)
		{
			continue;
		}

		itemObject->SetInt(TEXT("SlotType"), ESPType_StallSetting);
		itemObject->SetInt(TEXT("containerID"), 0);
		itemObject->SetInt(TEXT("itemid"), i);
		itemObject->SetInt(TEXT("slotID"), i);

		StallItemInfo stallItemInfo;
		if (UStallManager::getSingleton().GetStallSellItem(i, stallItemInfo))
		{
			UpdateItemSlot(i, stallItemInfo.SrcBag, stallItemInfo.SrcSlot, stallItemInfo.CurDur, stallItemInfo.Price);
		}
		else
		{
			UpdateItemSlot(i, -1, -1, 0, 0);
		}
	}
}

void UMMOUINewStallSettingPanel::RefreshButton()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UGFxObject* buttonObject = GFXPlayer->CreateObject("Object");
	if (!buttonObject)
	{
		return;
	}

	buttonObject->SetBool(TEXT("npcSetting"), FALSE);
	if (mainPlayer->MoveType == EMT_PrepareStall)
	{
		buttonObject->SetBool(TEXT("startStall"), TRUE);
		buttonObject->SetBool(TEXT("arrangeStall"), FALSE);
		buttonObject->SetBool(TEXT("shutStall"), TRUE);
	}
	else if (mainPlayer->MoveType == EMT_InStall)
	{
		buttonObject->SetBool(TEXT("startStall"), FALSE);
		buttonObject->SetBool(TEXT("arrangeStall"), TRUE);
		buttonObject->SetBool(TEXT("shutStall"), TRUE);
	}
	eventUpdateButtonStateUI(buttonObject);
}

void UMMOUINewStallSettingPanel::RefreshMoney()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UGFxObject* moneyObject = GFXPlayer->CreateObject(TEXT("Object"));
	if (!moneyObject)
	{
		return;
	}

	moneyObject->SetInt(TEXT("goldOnSale"), UStallManager::getSingleton().CalculateStallSettingTotalValue());
	moneyObject->SetInt(TEXT("goldTotal"), mainPlayer->GetMoney(MONEYTYPE_02));
	eventUpdateMoneyUI(moneyObject);
}

void UMMOUINewStallSettingPanel::RefreshNameAndAdd()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UGFxObject* nameObject = GFXPlayer->CreateObject(TEXT("Object"));
	if (!nameObject)
	{
		return;
	}

	nameObject->SetString(TEXT("StallName"), UStallManager::getSingleton().GetStallName());
	
	if (mainPlayer->MoveType == EMT_PrepareStall)
	{
		nameObject->SetBool(TEXT("StallNameEnabled"), TRUE);
		nameObject->SetBool(TEXT("StallAddEnabled"), TRUE);
	}
	else if (mainPlayer->MoveType == EMT_InStall)
	{
		nameObject->SetBool(TEXT("StallNameEnabled"), FALSE);
		nameObject->SetBool(TEXT("StallAddEnabled"), FALSE);
	}

	eventUpdateStallNameAndAdd(nameObject);
}

void UMMOUINewStallSettingPanel::AddItem(int srcConId, int srcSlotId, int dstSlotId)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (mainPlayer->MoveType != EMT_PrepareStall)
	{
		return;
	}

	if (dstSlotId < 0 || dstSlotId >= MAX_STALL_SLOT_COUNT)
	{
		return;
	}

	UItem* item = NULL;
	if (mainPlayer->ItemManager)
	{
		item = mainPlayer->ItemManager->GetItemBySlot(ECONTAINER_SLOT(srcConId), srcSlotId);
	}

	if (!item)
	{
		return;
	}

	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(srcConId, srcSlotId);
	if (itemInfo && itemInfo->m_dwRoleID != 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.ErrorSignatureForbidTrade")),
			MsgInfoType_Table,
			TEXT("MMOGame.Bag.ErrorSignatureForbidTrade"));
		return;
	}

	// Bound items cannot be sold
	FClientActionConditionContex contex1(ECACCT_UseItem_CheckBind);
	CheckClientActionCondition(contex1);
	if (itemInfo && itemInfo->m_byBindType == E_BINDTYPE_BINDED)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.BindCannotSell")), 
			MsgInfoType_Table,
			MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.BindCannotSell")));
		return;
	}

	// This item cannot be sold at a stall
	ClientActionCondition_StallItem checker;
	FClientActionConditionContex contex;
	contex.ItemTemplate = item;
	contex.Item = itemInfo;
	if (!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	// Item is being sold
	if (itemInfo && itemInfo->IsOnSale)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.IsOnSale")), 
			MsgInfoType_Table, 
			MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.IsOnSale")));
		return;
	}

	// Pop-up box
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallMsg, TRUE, "");
	UMMOUINewStallMsgPanel* panel = Cast<UMMOUINewStallMsgPanel>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Stall_StallMsg));
	if (panel)
	{
		BYTE durType = item->ItemDuration;
		int curCount = 0;
		int maxCount = 0;
		if (durType == DURTYPE_NONE || 
			durType == DURTYPE_EQUIP ||
			durType == DURTYPE_USE || 
			durType == DURTYPE_PURITY ||
			durType == DURTYPE_DEPOSIT)
		{
			curCount = 1;
			maxCount = 1;
		}
		else 
		{
			curCount = itemInfo != NULL ? itemInfo->m_nCurDur : item->DefaultDuration;
			maxCount = item->DefaultDuration;
		}
		panel->SetStallMsgPanelData(dstSlotId, 
			srcConId, 
			srcSlotId,
			UStallManager::getSingleton().GetCachedStallSettingPrice(item->ItemId), 
			curCount,
			maxCount,
			UStallManager::getSingleton().GetTax());
	}
}

void UMMOUINewStallSettingPanel::UpdateItemSlot(INT stallSlot, INT srcConId, INT srcSlotId, INT count, INT price)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UGFxObject* itemSettingObject = GetObject(TEXT("itemSetting"));
	if (!itemSettingObject)
	{
		return;
	}

	UGFxObject* itemObject = itemSettingObject->GetObject(FString::Printf(TEXT("slot_mc%d"), stallSlot));
	if (!itemObject)
	{
		return;
	}

	UItem* item = NULL;
	if (mainPlayer->ItemManager)
	{
		item = mainPlayer->ItemManager->GetItemBySlot(ECONTAINER_SLOT(srcConId), srcSlotId);
	}

	FString itemName = TEXT("");
	FString itemCount = TEXT("");
	FString itemPrice = TEXT("");
	FString itemTotalPrice = TEXT("");
	if (!item || count <= 0)
	{
		GFXPlayer->SetItemSlotData(itemObject, NULL, ESPType_StallSetting, E_CONSLOT_STALL_SETTING, 0);
	}
	else
	{
		itemName = item->GetDisplayName();
		itemCount = appItoa(count);
		itemPrice = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, price);
		itemTotalPrice = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, price * count);

		GFXPlayer->SetItemSlotData(itemObject, item, ESPType_StallSetting, srcConId, srcSlotId);
		//Local price record
		UStallManager::getSingleton().UpdateStallSettingPrice(item->ItemId, price);
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

UBOOL UMMOUINewStallSettingPanel::OnLeftMouseClick(class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault)
{
	return FALSE;
}

UBOOL UMMOUINewStallSettingPanel::OnLeftMousePress(class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault)
{
	return FALSE;
}

UBOOL UMMOUINewStallSettingPanel::OnRightMouseClick(class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault)
{
	if (bDefault) 
	{
		return FALSE;
	}

	if (MMOFacade::getSingleton().GetMainPlayer()->MoveType == EMT_InStall)
	{
		return FALSE;
	}

	if (SlotType == ESPType_BagResource)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.BagResourcr.CannotStall")),MsgInfoType_Table,TEXT("MMOGame.BagResourcr.CannotStall") );		
		return TRUE;
	}

	// Right-click on a bag item is handled as listing it for sale
	if (SlotType == ESPType_Bag /*|| SlotType == ESPType_BagResource*/)
	{
		INT emptySlotId = UStallManager::getSingleton().GetEmptyStallSettingSlot();
		if (emptySlotId >= 0)
		{
			AddItem(ContainerID, SlotID, emptySlotId);
		}
		else
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.StallSetting.HaveNotEmptySltot")),
				MsgInfoType_Table,
				TEXT("MMOGame.StallSetting.HaveNotEmptySltot"));
		}
		return TRUE;
	}
	else if (SlotType == ESPType_StallSetting)
	{
		int srcSlotId = mc->GetInt("itemid");
		StallItemInfo stallItemInfo;
		if (UStallManager::getSingleton().GetStallSellItem(srcSlotId, stallItemInfo))
		{
			if (stallItemInfo.ItemId > 0)
			{
				CMessageHandler::GetInstance().Send_RemoveStallItem(srcSlotId); 
			}
		}
		return TRUE;
	}

	return FALSE;
}

UBOOL UMMOUINewStallSettingPanel::OnDragDrop(class UGFxObject* SrcSlotMC, class UGFxObject* DestSlotMC, BYTE SrcSlotType, BYTE SrcContainerID, INT SrcSlotID, BYTE DestSlotType, BYTE DestContainerID, INT DestSlotID)
{
	if (MMOFacade::getSingleton().GetMainPlayer()->MoveType == EMT_InStall)
	{
		return FALSE;
	}
		
	if (SrcSlotType == ESPType_Bag && DestSlotType == ESPType_StallSetting)
	{
		INT emptySlotId = UStallManager::getSingleton().GetEmptyStallSettingSlot();
		if (emptySlotId >= 0)
		{
			AddItem(SrcContainerID, SrcSlotID, emptySlotId);
		}
		else
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.StallSetting.HaveNotEmptySltot")),
				MsgInfoType_Table,
				TEXT("MMOGame.StallSetting.HaveNotEmptySltot"));
		}
		return TRUE;
	}
	else if (SrcSlotType == ESPType_StallSetting &&  DestSlotType == ESPType_Bag )
	{
		SrcSlotID = SrcSlotMC->GetInt("itemid");
		CMessageHandler::GetInstance().Send_RemoveStallItem(SrcSlotID);
		return TRUE;
	}

	if ( (SrcSlotType == ESPType_BagResource && DestSlotType == ESPType_StallSetting) || (SrcSlotType == ESPType_StallSetting &&  DestSlotType == ESPType_BagResource))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.BagResourcr.CannotStall")),MsgInfoType_Table,TEXT("MMOGame.BagResourcr.CannotStall") );		
		return FALSE;
	}

	return FALSE;
}

void UMMOUINewStallSettingPanel::ASOpenStall(const FString& stallName)
{
	if (!UStallManager::getSingleton().CheckStartStallSetting())
	{
		FScriptDelegate funcOk(EC_EventParm);
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.NotHasAnyToSell")),
			0,
			funcOk,
			funcOk,
			FALSE);
		return;
	}

	if (stallName.IsEmpty())
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.EnterName"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Stall.EnterName"));
		return;
	}

	if (MMOUI::HasSpecialSymbol(stallName))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.SpecialSymbol"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Stall.SpecialSymbol"));
		return;
	}

	if (MMOUI::HasCensoredWord(stallName))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.SpecialName"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Stall.SpecialName"));
		return;
	}

	UStallManager::getSingleton().SetStallName(stallName);

	CMessageHandler::GetInstance().Send_ChangeStallName(*stallName); 
	CMessageHandler::GetInstance().Send_RealOpenStall();
}

void UMMOUINewStallSettingPanel::ASCloseStall()
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if (player)
	{
		if (player->MoveType == EMT_PrepareStall ||
			player->MoveType == EMT_InStall)
		{
			CMessageHandler::GetInstance().Send_RealCloseStall(); 
		}
	}
}

void UMMOUINewStallSettingPanel::ASArrangeStall()
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if (player)
	{
		if (player->MoveType == EMT_InStall)
		{
			CMessageHandler::GetInstance().Send_CloseStall();
		}
	}
}

void UMMOUINewStallSettingPanel::ASCloseStallPanel()
{
	Show(FALSE);
}

void UMMOUINewStallSettingPanel::ASViewStallRecord()
{
	UpdateStallRecordList();
}

void UMMOUINewStallSettingPanel::ASUpgradeStall()
{
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUINewStallUpgradePanel* panel = (UMMOUINewStallUpgradePanel*)MMOUI::FindUIPanelEx(uiManager->Panel_Stall_StallUpgrade);
		if (panel)
		{
			panel->Show(TRUE);
		}
	}
}

void UMMOUINewStallSettingPanel::UpdateStallUpgradeLeftTime()
{
	INT leftTime = UStallManager::getSingleton().GetStallUpgradeLeftTime();
	if (leftTime > 0)
	{
		UINT32 day = leftTime / 86400;
		UINT32 hour = (leftTime % 86400) / 3600;
		UINT32 min = (leftTime % 86400 % 3600) / 60;
		UINT32 sec = leftTime % 86400 % 3600 % 60;

		FString timeString = TEXT("");
		if (day > 0)
		{
			timeString += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TimeString.Day")), day);
		}
		if (hour > 0)
		{
			timeString += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TimeString.Hour")), hour);
		}
		else
		{
			if (min > 0)
			{
				timeString += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TimeString.Min")), min);
			} 
			else
			{
				timeString += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.TimeString.LessSec"));
			}
		}

		eventUpdateUpgradeLeftTime(timeString, FALSE);
	}
	else
	{
		eventUpdateUpgradeLeftTime(TEXT(""), TRUE);
	}
}

void UMMOUINewStallSettingPanel::UpdateStallRecordList()
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	StallRecordInfoList recordInfoList = UStallManager::getSingleton().GetStallRecordInfoList();
	StallRecordInfoList::iterator iter = recordInfoList.begin();
	int index = 0;
	for (; iter != recordInfoList.end(); ++ iter)
	{
		StallRecordInfo record = *iter;

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetString(TEXT("label"), record.Time);
			object->SetInt(TEXT("index"), index);
			object->SetInt(TEXT("type"), 0);
			object->SetInt(TEXT("deepth"), 0);

			object->SetString(TEXT("time"), record.Time);
			object->SetString(TEXT("action"), record.Operate);
			object->SetString(TEXT("item_name"), record.ItemName);
			object->SetString(TEXT("gain"), UITextParser::HTMLMoneyIconString(MONEYTYPE_02, record.Profit));

			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventUpdateStallRecord(objectArray);
}