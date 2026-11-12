#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net\message_handler.h"
#include "MMOGuildManager.h"
#include "ClientActionConditionCheck.h"
#include "UITextParser.h"
#include "UILayout.h"

IMPLEMENT_CLASS(UMMOUIGuildWarehouse)


void UMMOUIGuildWarehouse::MoreInfo()
{
}

void UMMOUIGuildWarehouse::SendFreshWarehouse()
{
	CMessageHandler::GetInstance().Send_RefreshWarehouse(_CurPage);
}

void UMMOUIGuildWarehouse::NativeInitOnce()
{
	for (INT i = 0 ; i < E_GUILD_STORAGE_TAG_MAX ; i ++)
	{
		FString mcName = TEXT("Ware");
		mcName += appItoa(i);
		UGFxObject* obj = GetObject(mcName);
		if (obj != NULL)
		{
			obj->SetInt(TEXT("itemid") , E_GUILD_STORAGE_TAG_1 + i);		
			obj->SetString(TEXT("label") , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Warehouse")) + appItoa(i + 1));
		}
	}
}

void UMMOUIGuildWarehouse::FreshWarehouse()
{
	for (INT i = 0; i < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG; i ++)
	{

		UGFxObject* itemSlot = GetObject(FString::Printf(TEXT("ItemSlot%d"), i));
		if (itemSlot == NULL)
		{			
			continue;
		}

		FClientItemInfo* itemInfo = MMOGuildData::getSingleton().GetWarehouseItem(_CurPage, i);
		if (itemInfo)
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
			GFXPlayer->SetItemSlotData(itemSlot, itemTemp, ESPType_GuildWarehouse, E_CONSLOT_GUILD_BANK, i);
		}
		else
		{
			GFXPlayer->SetItemSlotData(itemSlot, NULL, ESPType_GuildWarehouse, E_CONSLOT_GUILD_BANK, i);
		}
	}
}

void UMMOUIGuildWarehouse::ClearUI()
{
	for (INT i = 0; i < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG; i ++)
	{

		UGFxObject* itemSlot = GetObject(FString::Printf(TEXT("ItemSlot%d"),i));
		if (itemSlot == NULL)
		{			
			continue;
		}

		GFXPlayer->SetItemSlotData(itemSlot, NULL, ESPType_GuildWarehouse, E_CONSLOT_GUILD_BANK, i);
	}
}

void UMMOUIGuildWarehouse::TabChange(int tabId)
{
	INT oldPage = _CurPage;
	_CurPage = tabId;

	for (INT i = 0; i < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG; i ++)
	{
		UGFxObject* itemSlot = GetObject(FString::Printf(TEXT("ItemSlot%d"), i));
		if (itemSlot == NULL)
		{			
			continue;
		}
		
		//GFXPlayer->SetItemSlotData(itemSlot, NULL, ESPType_GuildWarehouse, E_CONSLOT_GUILD_BANK, i);
	}

	if (MMOGuildData::getSingleton().GetGuildBaseInfo().Id != 0)
	{
		SendFreshWarehouse();
	}
}

UBOOL UMMOUIGuildWarehouse::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if (bDefault)
	{
		return FALSE;
	}

	return FALSE; 
} 

UBOOL UMMOUIGuildWarehouse::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if (bDefault)
	{
		return FALSE;
	}

	return FALSE;
}

UBOOL UMMOUIGuildWarehouse::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if (bDefault)
	{
		return FALSE;
	}

	if (SlotType == ESPType_GuildWarehouse)
	{	
		//Check withdraw permission
		ItemInfo* itemInfo = MMOGuildData::getSingleton().GetWarehouseItem(_CurPage, SlotID);
		if (itemInfo)
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
			if (itemTemp)
			{
				// If it is equipment, throw it to the equipment tab
				if (Cast<UEquip>(itemTemp))
				{
					INT EmptySlot = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_DEFAULT);
					if (EmptySlot >= 0)
					{
						CMessageHandler::GetInstance().Send_GuildPopItem(_CurPage, SlotID, E_CONSLOT_DEFAULT, EmptySlot);
						MMOUI::GetUIManager()->delegateTurntoPage(E_CONSLOT_DEFAULT);
					}
					else
					{
						FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
						MMOUI::GetUIManager()->delegateAddScreenMessage(msg, MsgInfoType_Table, TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
					}
				}
				else // non-equipment goes to the default tab
				{
					INT EmptySlot = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_DEFAULT);
					if (EmptySlot >= 0)
					{
						CMessageHandler::GetInstance().Send_GuildPopItem(_CurPage, SlotID, E_CONSLOT_DEFAULT, EmptySlot);
						MMOUI::GetUIManager()->delegateTurntoPage(E_CONSLOT_DEFAULT);
					}
					else
					{
						FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
						MMOUI::GetUIManager()->delegateAddScreenMessage(msg, MsgInfoType_Table, TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
					}
				}
			}
		}

		return TRUE;
	}
	else if (SlotType == ESPType_Bag) // deposit
	{
		//Check deposit permission
		ItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(ContainerID , SlotID);
		if (itemInfo)
		{
			INT EmptySlot = MMOGuildData::getSingleton().GetEmptyGuildWarehouseSlot(_CurPage);
			if (EmptySlot >= 0)
			{
				//ECONTAINER_SLOT openBag = (ECONTAINER_SLOT)MMOUI::GetUIManager()->delegateGetOpenBag();
				CMessageHandler::GetInstance().Send_GuildPushItem(ContainerID, SlotID, _CurPage, EmptySlot);//.Send_MoveItem(E_CONSLOT_BANK, EmptySlot, openBag, SlotID);
			}
			else
			{
				FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_WareHouse_Full"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(msg, MsgInfoType_Table, TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_WareHouse_Full"));
			}
		}
		return TRUE;
	}

	return FALSE;
}

void UMMOUIGuildWarehouse::UpdateAccessInfo()
{
	AccessInfoList& list = MMOGuildData::getSingleton().GetAccessInfoList();
	FString infoText;
	//Placed in reverse here
	for (INT i = (INT)list.size() - 1; i >= 0; i--)
	{
		GuildWareAccessInfo& item = list[i];
		
		PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo(item.RoleId);
		if (!playerInfo)
		{
			//If not yet synced down, do not show this entry for now
			continue;			
		}
		
		INT year , month , dayOfWeek , day , hour , min , sec;
		year = month = dayOfWeek = day = hour = min = sec = 0;
		appSecondsToLocalTime((time_t)item.Time, year, month, dayOfWeek, day, hour, min, sec);

		infoText += FString::Printf(TEXT("%d-%d-%d %d:%02d:%02d \n"), year, month, day, hour, min, sec);

		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(item.ItemTempId);
		FString itemName = UITextParser::HTMLFormatItemName(itemTemp);
		
		//Do not place time for now
		if (item.Type == STORAGE_HE_TYPE_PUSH)
		{
			infoText += FString::Printf(TEXT("%s deposited %s x%d \n\n"), *UITextParser::HTMLFormatPlayerName(item.RoleName), *itemName, item.Count);
		}
		else if (item.Type == STORAGE_HE_TYPE_POP)
		{
			infoText += FString::Printf(TEXT("%s withdrew %s x%d \n\n"), *UITextParser::HTMLFormatPlayerName(item.RoleName), *itemName, item.Count);
		}
	}

	eventCallSetAccessInfoText(infoText);
}

void UMMOUIGuildWarehouse::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!IsVisible)
	{
		return;
	}

	AccessInfoList& list = MMOGuildData::getSingleton().GetAccessInfoList();
	switch(evt->GetEventType())
	{
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* baseInfoEvt = (MMOGameEvent_PlayerBaseInfo*)evt;
			if (baseInfoEvt)
			{
				for (INT i = 0; i < (INT)list.size(); i ++)
				{
					if (list[i].RoleId == baseInfoEvt->_RoldID)
					{
						PlayerBaseInfo *baseInfo = baseInfoEvt->_PlayerBaseInfo;
						if (baseInfo)
						{
							list[i].RoleName = baseInfo->_RoleName;
							UpdateAccessInfo();
						}
					}
				}
			}
		}
		break;
	case GameEvent_Guild:
		{
			MMOGameEvent_Guild* guildEvent = (MMOGameEvent_Guild*)evt;
			if (guildEvent == NULL)
			{
				return;
			}

			switch (guildEvent->_EventType)
			{
			case MMOGameEvent_Guild::EGET_UpdateGuildBuildingList:
				UpdateTabEnable();
				break;
			}
		}
		break;
	}
}

UBOOL UMMOUIGuildWarehouse::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	FClientItemInfo* srcInfo = NULL;
	FClientItemInfo* desInfo = NULL;

	if (SrcSlotType == ESPType_Bag)
	{
		srcInfo = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
	}
	else if (SrcSlotType == ESPType_GuildWarehouse)
	{
		srcInfo = MMOGuildData::getSingleton().GetWarehouseItem(_CurPage, SrcSlotID);
	}

	if (DestSlotType == ESPType_Bag)
	{
		desInfo = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
	}
	else if (DestSlotType == ESPType_GuildWarehouse)
	{
		desInfo = MMOGuildData::getSingleton().GetWarehouseItem(_CurPage, DestSlotID);
	}
/*
	if (SrcSlotType == ESPType_Bag && DestSlotType == ESPType_Invalid) // discard from bag
	{
		if (srcInfo) 
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(srcInfo->m_nTemplateID);

			ClientActionCondition_DelItem checker;

			FClientActionConditionContex contex;
			contex.Item = srcInfo;
			contex.ItemTemplate = itemTemp;

			if (!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}
		}

		MMOUI::GetUIManager()->delegateDeleteItem(SrcContainerID, SrcSlotID);

		return TRUE;
	}
	else 
*/
	if ((SrcSlotType == ESPType_Bag || SrcSlotType == ESPType_GuildWarehouse) && 
		(DestSlotType == ESPType_Bag || DestSlotType == ESPType_GuildWarehouse))// swap
	{
		//check whether locked by UI function lock
		if (srcInfo)
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(srcInfo->m_nTemplateID);

			ClientActionCondition_SwampItem checker;

			FClientActionConditionContex contex;
			contex.Item = srcInfo;
			contex.ItemTemplate = itemTemp;

			if (!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}
		}

		//target item check
		if (desInfo)
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(desInfo->m_nTemplateID);

			ClientActionCondition_SwampItem checker;

			FClientActionConditionContex contex;
			contex.Item = desInfo;
			contex.ItemTemplate = itemTemp;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}
		}

		//
		if (DestSlotType == ESPType_Bag && SrcSlotType == ESPType_Bag)
		{
			CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
		}
		else if (DestSlotType == ESPType_GuildWarehouse && SrcSlotType == ESPType_GuildWarehouse)
		{
			CMessageHandler::GetInstance().Send_GuildMoveItem(_CurPage, SrcSlotID, _CurPage, DestSlotID);
		}
		else if (DestSlotType == ESPType_GuildWarehouse && SrcSlotType == ESPType_Bag)
		{
			//Check permission
			//Put in
			CMessageHandler::GetInstance().Send_GuildPushItem(SrcContainerID, SrcSlotID, _CurPage, DestSlotID);
		}
		else if (DestSlotType == ESPType_Bag && SrcSlotType == ESPType_GuildWarehouse)
		{
			//Check permission
			CMessageHandler::GetInstance().Send_GuildPopItem(_CurPage, SrcSlotID, DestContainerID, DestSlotID);
		}
		return TRUE;
	}
	
	return FALSE;
}

void UMMOUIGuildWarehouse::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_PlayerBaseInfo);
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildWarehouse>(this, evts);

	if (GFXPlayer)
	{
		UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
		if (data)
		{
			data->SetString(TEXT("titleName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Warehouse")));
			data->SetString(TEXT("updateButton"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.FreshWarehouse")));
			data->SetString(TEXT("moreButton"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoreInfo")));
		}

		eventCallSetPanelText(data);
	}
}

void UMMOUIGuildWarehouse::NativeInitLoad()
{	
	Super::NativeInitLoad();

	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}

void UMMOUIGuildWarehouse::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

	if (show)
	{
		if (MMOGuildData::getSingleton().GetGuildBaseInfo().Id != 0)
		{
			eventCallSetAccessInfoText(TEXT(""));

			_Tab = GetObject(FString(TEXT("Ware")) + appItoa(_CurPage));
			if (_Tab != NULL)
			{
				GFXPlayer->eventCallWidgetDispatchEvent(_Tab, TEXT("click"));
			}

			UGFxObject* logPanel = GetObject(TEXT("log"));
			if (logPanel)
			{
				logPanel->SetVisible(FALSE);
			}

			UpdateTabEnable();
			UpdatePosition(FALSE);
		}
		else
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GuildWarehouse.NoGuild")), MsgInfoType_Table, TEXT("MMOGame.GuildWarehouse.NoGuild"));
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Guild_Warehouse , FALSE, "");
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Bag, FALSE, "");
		}
	}
}

void UMMOUIGuildWarehouse::UpdateTabEnable()
{
	INT openWare = MMOGuildData::getSingleton().GetGuildBaseInfo().level;
	eventCallSetTabEnable(openWare);
}

void UMMOUIGuildWarehouse::UpdatePosition(UBOOL showLog)
{
	UMMOUIBag* bagPanel = (UMMOUIBag*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Bag);
	if (bagPanel == NULL)
	{
		return;
	}

	FVector2D screenSize = MMOFacade::GetScreenSize();

	FLOAT x = 0.0f, y = 0.0f;
	this->GetPosition(x, y);
	INT height = this->GetInt(TEXT("height"));
	//INT width = this->GetInt(TEXT("width"));
	INT width = showLog ? 880 : 640;

	INT bagHeight = bagPanel->GetInt(TEXT("height"));
	//INT bagWidth = bagPanel->GetInt(TEXT("width"));
	INT bagWidth = 420;

	this->SetPosition(INT(screenSize.X - width - bagWidth) / 2, y);
	bagPanel->SetPosition(INT(screenSize.X - width - bagWidth) / 2 + width, y);
}

//////////////////////////////////////////////////////////////////////////
void UMMOUIGuildWarehouse::ASGuildWarehouseMore()
{
	MoreInfo();
}

void UMMOUIGuildWarehouse::ASGuildWarehouseUpdate()
{
	SendFreshWarehouse();
}

void UMMOUIGuildWarehouse::ASGuildWarehouseTab(INT tabId)
{
	TabChange(tabId);
}

void UMMOUIGuildWarehouse::ASViewGuildLog(UBOOL visible)
{
	UpdatePosition(visible);
}
