#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"
#include "ClientActionConditionCheck.h"

IMPLEMENT_CLASS(UMMOUIQuestItemCollectionPanel)

void UMMOUIQuestItemCollectionPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Quest);
	EventAdapter = new TGameEventAdapter<UMMOUIQuestItemCollectionPanel>(this, evts);

	_ItemSlot = GetObject(TEXT("quest_collection_item_mc"));
	if (_ItemSlot)
	{
		_ItemSlot->SetInt(TEXT("SlotType"), ESPType_ItemCollection);
	}
}

void UMMOUIQuestItemCollectionPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
	}

	ClearCollectionItem();
}

void UMMOUIQuestItemCollectionPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!IsVisible)
	{
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_Quest:
		{
			MMOGameEvent_Quest* questEvt = (MMOGameEvent_Quest*)evt;
			if(questEvt == NULL)
			{
				return;
			}

			switch(questEvt->EventType)
			{
			case MMOGameEvent_Quest::EQET_AcceptQuest:
			case MMOGameEvent_Quest::EQET_UpdateQuest:
			case MMOGameEvent_Quest::EQET_AbandonQuest:
			case MMOGameEvent_Quest::EQET_CompleteQuest:
				break;
			}
		}
		break;
	}
}

void UMMOUIQuestItemCollectionPanel::ItemChange(INT ContainerID, INT SlotID)
{
	if (EGameState_InGame == MMO::GetCurrentGameState())
	{
	}
}

void UMMOUIQuestItemCollectionPanel::ItemDelete(INT ContainerID, INT SlotID)
{
	if (EGameState_InGame == MMO::GetCurrentGameState())
	{
	}
}

void UMMOUIQuestItemCollectionPanel::ShowItemCollectionPanel(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (object)
	{
		object->SetInt(TEXT("quest_id"), questId);
		object->SetString(TEXT("quest_desc"), quest->CompleteQuestDialog);
		eventCallUpdateQuestData(object);
	}

	Show(TRUE);
}

UBOOL UMMOUIQuestItemCollectionPanel::SetCollectionItem(INT containerId, INT slotId)
{
	if (!_ItemSlot)
	{
		return FALSE;
	}

	// Clear
	if (containerId < 0 || slotId < 0)
	{
		if (_ItemBagPos._ConID >= 0 && _ItemBagPos._SlotID >= 0)
		{
			MMOFacade::getSingleton().LockItem(_ItemBagPos._ConID, _ItemBagPos._SlotID, -1);
			GFXPlayer->eventCallSetItemData(_ItemSlot, 0, TEXT(""), -1, ESPType_ItemCollection, TEXT(""), ITEMCOLOR_INVALID, -1, -1);
		}

		_ItemBagPos._ConID = -1;
		_ItemBagPos._SlotID = -1;
		_ItemTempId = 0;
		return TRUE;
	}

	// Check items
	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(containerId, slotId);
	if (!itemInfo)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EnhanceGeneral.ERROR_ITEM_NOT_FOUND")), 
			MsgInfoType_Table, 
			TEXT("MMOGame.EnhanceGeneral.ERROR_ITEM_NOT_FOUND"));
		return FALSE;
	}

	// Whether it is equipment
	UItem* pItem = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
	UEquip* pEquip = Cast<UEquip>(pItem);
	if (pEquip == NULL)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EnhanceGeneral.ERROR_NOT_A_EQUIP") ) ,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.ERROR_NOT_A_EQUIP"));
		return FALSE;
	}

	//check whether in a UI (this UI or another operation UI)
	ClientActionCondition_ItemCollection checker;
	FClientActionConditionContex contex;
	contex.Item = itemInfo;
	if (!checker.CheckClientActionCondition(contex))
	{
		return FALSE;
	}

	// Unlock the equipment originally placed in the bag
	if (_ItemBagPos._ConID >= 0 && _ItemBagPos._SlotID >= 0)
	{
		MMOFacade::getSingleton().LockItem(_ItemBagPos._ConID, _ItemBagPos._SlotID, -1);
	}

	// New equipment
	_ItemBagPos._ConID = containerId;
	_ItemBagPos._SlotID = slotId;

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData(_ItemSlot, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pItem, itemInfo ), pItem->ItemId, ESPType_ItemCollection, TEXT(""), pItem->ItemColor + 1, _ItemBagPos._ConID, _ItemBagPos._SlotID);
	MMOFacade::getSingleton().LockItem(_ItemBagPos._ConID, _ItemBagPos._SlotID, ESPType_ItemCollection);
	_ItemTempId = itemInfo->m_nTemplateID;

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (object)
	{
		object->SetInt(TEXT("quest_item_id"), _ItemTempId);
		object->SetInt(TEXT("quest_item_index"), slotId);
		object->SetString(TEXT("quest_item_icon_name"), MMOFacade::getSingleton().GetItemIconNamePersonalized( pItem ));
		object->SetString(TEXT("quest_item_name"), pItem->GetDisplayName());
		eventCallUpdateItemData(object);
	}

	return TRUE;
}

UBOOL UMMOUIQuestItemCollectionPanel::ClearCollectionItem()
{
	return SetCollectionItem(-1, -1);
}

UBOOL UMMOUIQuestItemCollectionPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if (bDefault)
	{
		return FALSE;
	}
	return FALSE; 
} 

UBOOL UMMOUIQuestItemCollectionPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if (bDefault)
	{
		return FALSE;
	}
	return FALSE;
}

UBOOL UMMOUIQuestItemCollectionPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if (bDefault)
	{
		return FALSE;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	switch (SlotType)
	{
	case ESPType_Bag:
		{
			return SetCollectionItem(ContainerID, SlotID);
		}
		break;
	case ESPType_ItemCollection:
		{
			return ClearCollectionItem();
		}
		break;
	}

	return FALSE;
}

UBOOL UMMOUIQuestItemCollectionPanel::OnDragDrop(class UGFxObject* srcSlotMC, class UGFxObject* destSlotMC,
	BYTE srcSlotType, 
	BYTE srcContainerID,
	INT srcSlotID,
	BYTE destSlotType,
	BYTE destContainerID,
	INT destSlotID)
{
	if (destSlotMC == NULL || srcSlotMC == NULL)
	{
		return FALSE;
	}

	if (srcSlotType == ESPType_Bag && destSlotType == ESPType_ItemCollection)
	{
		SetCollectionItem(srcContainerID, srcSlotID);
	}

	if (srcSlotType == ESPType_ItemCollection && destSlotType == ESPType_Bag)
	{
		ClearCollectionItem();
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIQuestItemCollectionPanel::ASSubmitItem(INT questId, INT itemId, INT itemIndex)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	// get required attribute
	INT itemColorReq = 0;
	INT itemLevelReq = 0;
	FString itemSubTypeReq = TEXT("");
	for (INT i = 0; i < quest->CheckVar.Num(); ++i)
	{
		FQuestCheckVar checkVar = quest->CheckVar(i);
		if (checkVar.CheckVarType == TEXT("vtItemRecycle") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_ROLE_VARIABLE_COUNT)
		{
			QuestRecycleDataList questRecycleList = MMOQuestData::getSingleton().GetQuestRecycleList();
			QuestRecycleDataList::iterator iter = questRecycleList.begin();
			for (; iter != questRecycleList.end(); ++ iter)
			{
				if (iter->VarIndex == checkVar.CheckVarID)
				{
					itemColorReq = iter->ItemColor;
					itemLevelReq = iter->ItemLevel;
					itemSubTypeReq = iter->ItemSubType;
					break;
				}
			}
			break;
		}
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemId);
	if (!item)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item does not exist"), 
			MsgInfoType_Table, 
			TEXT("Item does not exist"));
		return;
	}

	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEFAULT, itemIndex);
	if (!itemInfo)
	{
		FString text = TEXT("MMOGame.EnhanceGeneral.ERROR_ITEM_NOT_FOUND");
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(text), MsgInfoType_Table, text);
		return;
	}

	if (itemInfo->m_nTemplateID != itemId)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item does not exist"), 
			MsgInfoType_Table, 
			TEXT("Item does not exist"));
		return;
	}

	if (item->ItemColor != itemColorReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item quality does not meet the requirement"), 
			MsgInfoType_Table, 
			TEXT("Item quality does not meet the requirement"));
		return;
	}

	if (item->ItemLevel != itemLevelReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item level does not meet the requirement"), 
			MsgInfoType_Table, 
			TEXT("Item level does not meet the requirement"));
		return;
	}

	if (item->ItemSubType != itemSubTypeReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item type does not meet the requirement"), 
			MsgInfoType_Table, 
			TEXT("Item type does not meet the requirement"));
		return;
	}

	CMessageHandler::GetInstance().Send_Recycle(itemId, itemIndex);
}

