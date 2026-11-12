#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "MMOMsgBoxManager.h"
#include "UILayout.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIWarehouse)


void UMMOUIWarehouse::ClearUI()
{
	INT cnt = MMOFacade::getSingleton().GetContainerSize(E_CONSLOT_BANK);
	for (INT i = 0; i < cnt; ++i)
	{
		UGFxObject* itemSlot = BagGroup->GetObject(FString::Printf(TEXT("slot%d_mc"), i));
		GFXPlayer->SetItemSlotData( itemSlot, NULL, ESPType_Warehouse, E_CONSLOT_BANK, i);
	}
}

void UMMOUIWarehouse::BagSlotChange(INT ContainerID,INT SlotID)
{
	//eventBagSlotChangeEx(ContainerID, SlotID);
	if( ContainerID == E_CONSLOT_BANK )
	{
		BagSlotChangeEx(ContainerID, SlotID);
	}
	
}


//------------------------------------------------------------------------
UBOOL UMMOUIWarehouse::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
    return FALSE; 
} 

//------------------------------------------------------------------------
void UMMOUIWarehouse::ExpandWarehouseOk( UGFxObject* mc)
{
	//INT containerId = mc->GetInt(TEXT("iteminfoid"));
	INT count = mc->GetInt(TEXT("itemdataid"));
	CMessageHandler::GetInstance().Send_BuyContainerSlot( E_CONSLOT_BANK , count );
}

//------------------------------------------------------------------------
void UMMOUIWarehouse::ExpandWarehouseNo( UGFxObject* mc)
{
	ConfirmMsgBox = NULL;
}


 UBOOL UMMOUIWarehouse::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(  mc && mc->GetBool( "isSlotLocked") && SlotType == ESPType_Warehouse && ContainerID == E_CONSLOT_BANK )
	{
		INT CurCapacity = player->ItemManager->CurCapacity( ContainerID );
		if( SlotID > CurCapacity - 1 )
		{
			INT needMoney = 0;
			
			for ( INT i = 0 ; i < SlotID + 1 - CurCapacity ; i++ )
			{
				//needMoney += Min( 50 + 5 * ( i + CurCapacity - UCONST_OriWarehouseCap ) , 200 );		

				int num = i + CurCapacity - UCONST_OriWarehouseCap;

				if (num < stage)
				{
					needMoney += BaseMoney1 + IncreaseRatio1 * num;
				} 
				else
				{
					needMoney += BaseMoney2 + IncreaseRatio2 * ( num - stage );
				}

			}

			FScriptDelegate fun(EC_EventParm);
			fun.Object = this;
			fun.FunctionName = TEXT("ExpandWarehouseOk");

			FScriptDelegate funNone(EC_EventParm);
			funNone.Object = this;
			funNone.FunctionName = TEXT("ExpandWarehouseNo");	

			FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.UnlockNeedMoney") );
			FString txt = FString::Printf( *temp , ( SlotID +1 - CurCapacity ) ) + UITextParser::HTMLMoneyIconString( MONEYTYPE_02 , needMoney );

			HideMsg();

			ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( txt , ContainerID , fun , funNone );
			if(ConfirmMsgBox)
				ConfirmMsgBox->SetInt( TEXT("itemdataid") , SlotID + 1 - CurCapacity );

			return TRUE;
		}

	}
	return FALSE;
}

UBOOL UMMOUIWarehouse::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	ItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
	if(!itemInfo)
	{
		return TRUE;
	}

	if(SlotType == ESPType_Warehouse)
	{		
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
		if(itemTemp)
		{
			// If it is a resource, first check whether the resource package can store it
			ECONTAINER_SLOT desContainerID = E_CONSLOT_DEFAULT;
			INT emptySlotId = -1;

			if( itemTemp->CanResource && GetEmptySlot(E_CONSLOT_RESOURCE,itemTemp->ItemDuration,itemInfo->m_nTemplateID,itemInfo->m_nCurDur)>=0)
			{
				desContainerID = E_CONSLOT_RESOURCE;
			}
			else
			{
				desContainerID = E_CONSLOT_DEFAULT;
			}

			emptySlotId = GetEmptySlot(desContainerID,itemTemp->ItemDuration,itemInfo->m_nTemplateID,itemInfo->m_nCurDur);

			if( emptySlotId >= 0 )//there is an empty slot
			{
				CMessageHandler::GetInstance().Send_MoveItem( desContainerID, emptySlotId, ContainerID, SlotID );
				MMOUI::GetUIManager()->delegateTurntoPage( desContainerID );
			}				
			else
			{
				FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
			}
		}
	
		return TRUE;
	}
	else if(SlotType == ESPType_Bag || SlotType == ESPType_BagResource) // deposit in
	{
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
		if(itemTemp)
		{
			// 
			ECONTAINER_SLOT desContainerID = E_CONSLOT_BANK;

			INT emptySlot = -1;
			if( itemTemp->ItemDuration == E_DURTYPE_STACK )
			{
				emptySlot = MMOFacade::getSingleton().GetCanStackSlot( desContainerID , itemInfo->m_nTemplateID , itemInfo->m_nCurDur );
			}
			if( emptySlot < 0 )
			{
				emptySlot = MMOFacade::getSingleton().GetEmptyItemSlot( desContainerID );
			}

			if( emptySlot >= 0 )//there is an empty slot
			{
				ECONTAINER_SLOT openBag = SlotType == ESPType_Bag? E_CONSLOT_DEFAULT : E_CONSLOT_RESOURCE;//(ECONTAINER_SLOT)MMOUI::GetUIManager()->delegateGetOpenBag();
				CMessageHandler::GetInstance().Send_MoveItem( desContainerID , emptySlot , openBag  , SlotID);
			}				
			else
			{
				FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_WareHouse_Full"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_WareHouse_Full"));
			}
		}
		
		return TRUE;
	}

	return FALSE;
}

INT UMMOUIWarehouse::GetEmptySlot(BYTE desContainerID,BYTE ItemDuration,INT m_nTemplateID,INT m_nCurDur)
{
		INT emptySlot = -1;
		if( ItemDuration == E_DURTYPE_STACK )
		{
			emptySlot = MMOFacade::getSingleton().GetCanStackSlot((ECONTAINER_SLOT)desContainerID , m_nTemplateID , m_nCurDur );
		}
		if( emptySlot < 0 )
		{
			emptySlot = MMOFacade::getSingleton().GetEmptyItemSlot((ECONTAINER_SLOT) desContainerID );
		}

		return emptySlot;
}

//------------------------------------------------------------------------
void UMMOUIWarehouse::ItemDelete(INT ContainerID, INT SlotID)
{
	if( ContainerID == E_CONSLOT_BANK && BagGroup )
	{
		UGFxObject* itemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc") , SlotID ) );
		GFXPlayer->SetItemSlotData( itemSlot, NULL, ESPType_Warehouse, ContainerID, SlotID );
	}	
}

void UMMOUIWarehouse::BagPage(INT labelId)
{
	
	UItem* tItem = NULL;
	UGFxObject* ItemSlot = NULL;

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	
	INT CurCapacity = Player->ItemManager->CurCapacity(E_CONSLOT_BANK);
	for( INT i = 0 ; i < UCONST_MaxWarehouseSlotCount ; i++ )
	{		
		ItemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc") , i ) );
		if( ItemSlot == NULL )
		{			
			continue;
		}

		INT realSlot = i + UCONST_MaxWarehouseSlotCount * labelId;

		if( i >= CurCapacity )
		{
			GFXPlayer->SetItemSlotData( ItemSlot , NULL , ESPType_Warehouse , E_CONSLOT_BANK , realSlot , ISS_SlotLocked );

#ifdef UI_OPTIMIZATION
			UBOOL isToGray_Last = ItemSlot->GetBool("isToGray");
			if(FALSE!=isToGray_Last)
			{
				eventCallUCGraySlot( ItemSlot, FALSE );
				ItemSlot->SetBool("isToGray",FALSE);
			}
#else
			eventCallUCGraySlot( ItemSlot, FALSE );
#endif

			continue; 
		}

		tItem = MMOFacade::getSingleton().FindItemTemplate( E_CONSLOT_BANK , i); // Player->ItemManager->BagBankList(i);				

		if(tItem == NULL )
		{
			GFXPlayer->SetItemSlotData( ItemSlot, NULL , ESPType_Warehouse, E_CONSLOT_BANK, i );

#ifdef UI_OPTIMIZATION
			UBOOL isToGray_Last = ItemSlot->GetBool("isToGray");
			if(FALSE!=isToGray_Last)
			{
				eventCallUCGraySlot( ItemSlot, FALSE );
				ItemSlot->SetBool("isToGray",FALSE);
			}
#else
			eventCallUCGraySlot( ItemSlot, FALSE );
#endif

		}		
		else		
		{
			GFXPlayer->SetItemSlotData( ItemSlot, tItem, ESPType_Warehouse, E_CONSLOT_BANK, i );
			UBOOL isToGray = GetItemFilterFrame( tItem , EBLT_NormalItem , CurTab );
			mmoFacade.GrayItem( ESGMark_Bag, E_CONSLOT_BANK, i, isToGray );
		}
	}
}

//==========================================================================================================
//items to gray out
void UMMOUIWarehouse::GrayItem( BYTE ContainerID, INT SlotID, struct FClientItemInfo *pItemInfo )
{
	UGFxObject *pItemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc"), SlotID ) );

	if( pItemSlot == NULL ){
		return;
	}

	if( pItemInfo == NULL ){
		//eventCallUCGraySlot( pItemSlot, FALSE );
		return;
	}

	//if( CurTab == EBTT_AllItem ){
	//	UBOOL isToGray = ( pItemInfo->IsGrayMarkedInUI != 0 );
	//	eventCallUCGraySlot( pItemSlot, isToGray );
	//}
	//else{
	//	//If the current bag filter setting is not "all items", filter by package only
	//	UBOOL isGrayByBag = ( ( pItemInfo->IsGrayMarkedInUI & ESGMark_Bag ) != 0 );
	//	eventCallUCGraySlot( pItemSlot, isGrayByBag );
	//}


	//
	UBOOL isToGray = FALSE;
	if( CurTab == EBTT_AllItem ){
		isToGray= ( pItemInfo->IsGrayMarkedInUI != 0 );
	}
	else{
		isToGray = ( ( pItemInfo->IsGrayMarkedInUI & ESGMark_Bag ) != 0 );
	}

#ifdef UI_OPTIMIZATION
	UBOOL isToGray_Last = pItemSlot->GetBool("isToGray");
	if(isToGray!=isToGray_Last)
	{
		eventCallUCGraySlot( pItemSlot, isToGray );
		pItemSlot->SetBool("isToGray",isToGray);
	}
#else
	eventCallUCGraySlot( pItemSlot, isToGray );
#endif


}

UBOOL UMMOUIWarehouse::GetItemFilterFrame( UItem* item , INT label , INT filterTab )
{
	if( !item )
	{
		return FALSE;//TEXT("all");
	}

	if( label != EBLT_NormalItem )
	{
		return FALSE;//TEXT("all");
	}

	if( filterTab == EBTT_AllItem )
	{
		return FALSE;//TEXT("all");
	}
	else
	{
		UBOOL match = FALSE;
		switch(filterTab)
		{
		case EBTT_FilterEquip:
			match = item->ItemType == TEXT("ITEMTYPE_EQUIP");
			break;
		case EBTT_FilterItem:
			match = ( item->ItemType != TEXT("ITEMTYPE_EQUIP") && item->ItemType != TEXT("ITEMTYPE_MATERIAL") );
			break;
		case EBTT_FilterMaterial:
			match = item->ItemType == TEXT("ITEMTYPE_MATERIAL");
			break;
		}

		if( match )
		{
			return FALSE;//TEXT("off");
		}
		else
		{
			return TRUE;//TEXT("on");
		}
	}
	return FALSE;//TEXT("all");
}

void UMMOUIWarehouse::UpdateWarehouseCapSlot()
{
	BagPage(0);
}

UBOOL UMMOUIWarehouse::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	if(SrcSlotType == ESPType_Warehouse && DestSlotType == ESPType_Invalid) // discard
	{
		MMOUI::GetUIManager()->delegateDeleteItem(SrcContainerID, SrcSlotID);
		return TRUE;
	}
	else if(SrcSlotType == ESPType_Warehouse && DestSlotType == ESPType_Warehouse)  // swap
	{
		if((SrcSlotID != DestSlotID) && // no swap for the same cell
			MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID))
		{
			CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
		}
		return TRUE;
	}
	else if((SrcSlotType == ESPType_Warehouse && DestSlotType == ESPType_Bag)
		|| (SrcSlotType == ESPType_Bag && DestSlotType == ESPType_Warehouse)
		|| (SrcSlotType == ESPType_Warehouse && DestSlotType == ESPType_BagResource)
		|| (SrcSlotType == ESPType_BagResource && DestSlotType == ESPType_Warehouse)) // swap
	{
		if(MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID))
		{
			CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
		}
		return TRUE;
	}

	return FALSE;
}


void UMMOUIWarehouse::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{	
	case GameEvent_SortTimer:
		{
			MMOGameEvent_SortTimer* e = (MMOGameEvent_SortTimer*)(evt);			
			eventCallUpdateSortTimer(e->TimeSecondsLeft);
		}
		break;

	case GameEvent_SortTimerBegin:
		{			
			eventCallEnableSort(FALSE, MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.SortBtn.Label")));
		}
		break;

	case GameEvent_SortTimerEnd:
		{			
			eventCallEnableSort(TRUE, MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.SortBtn.Label")));
			MMOFacade::getSingleton().SetPlayerSortType(IST_Invalid);
		}
		break;
	case GameEvent_ClearBag:
		{
			if (MMOFacade::getSingleton().GetPlayerSortType() == IST_Warehouse)
			{
				ClearUI();
			}
		}
		break;
	case GameEvent_BagSortEnd:
		{
			if (MMOFacade::getSingleton().GetPlayerSortType() == IST_Warehouse)
			{
				BagPage(0);
				MMOFacade::getSingleton().SetPlayerSortType(IST_Invalid);
			}
			
		}
		break;

	}
}

void UMMOUIWarehouse::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_SortTimer);
	evts.AddItem(GameEvent_SortTimerBegin);
	evts.AddItem(GameEvent_SortTimerEnd);
	evts.AddItem(GameEvent_ClearBag);
	evts.AddItem(GameEvent_BagSortEnd);
	EventAdapter = new TGameEventAdapter<UMMOUIWarehouse>(this, evts);

	if(GFXPlayer)
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
		data->SetString( TEXT("panelName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Warehouse.Warehouse") ) );
		data->SetString( TEXT("button1") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Warehouse.Sort") ) );
		eventCallSetPanelText(data);
	}
	
	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));

	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("WHBaseMoney1") , BaseMoney1 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("WHBaseMoney2") , BaseMoney2 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("WHIncreaseRatio1") , IncreaseRatio1 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("WHIncreaseRatio2") , IncreaseRatio2 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("WHStage") , stage , GDefaultGameIni );
}


void UMMOUIWarehouse::UnInit()
{
	Super::UnInit();
	FScriptDelegate nullDelgate(EC_EventParm);
	MMOUI::GetUIManager()->SetUpdateWarehouseCapSlotDelegate(nullDelgate);
}

void UMMOUIWarehouse::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

	if(show)
	{
		BagPage( 0 );		
	}

}

void UMMOUIWarehouse::SetItemSlotData( class UGFxObject* mc, class UItem* tItem, BYTE SlotType, BYTE ContainerID , INT SlotID , BYTE inISS )
{
	Super::SetItemSlotData( mc, tItem, SlotType, ContainerID, SlotID,inISS);

	//finishing blow
	if(!mc)
	{
		return;
	}

	UGFxObject* abnormity = mc->GetObject(TEXT("abnormity_mc"));

	if(tItem)
	{
		FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem( ContainerID , SlotID );


		//changed here so weapons in the bag that do not suit this class are marked with x; since this icon is the same as the locked-item icon when trading or stalling, there is ambiguity. But currently this is the only icon.
		if( tItem->ItemType == TEXT("ITEMTYPE_EQUIP") && !mc->GetBool( TEXT("isSlotUnusable") ) )
		{
			AMMOPlayer* pl=  MMOFacade::getSingleton().GetMainPlayer();


			if(abnormity)
			{
				if( !MMO::CanBeUsedByPlayerClass( EPlayerClass( pl->GetClassId() ) , tItem->RequireClass ) )
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotImpossible") );
				}
				else if( pl->GetActorLevel() < tItem->RequireLevel )
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotLock") );
				}
				else if( !itemInfo->m_byIdentified )
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotUnidentify") );
				}
				else
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
				}
			}

		}
	}
	else
	{
		if(abnormity)
		{
			abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
		}

	}
}

void UMMOUIWarehouse::ASSortWarehouse()
{
	CMessageHandler::GetInstance().Send_SortBag(E_CONSLOT_BANK);
	MMOFacade::getSingleton().StartSortTimer( LSM_SortWarehouse );
	MMOFacade::getSingleton().SetPlayerSortType(IST_Warehouse);
}

void UMMOUIWarehouse::ASWareHouseTabChange(int tabId)
{
	CurTab = tabId;
	BagPage(0);
}

void UMMOUIWarehouse::BagSlotChangeEx(BYTE ContainerID,INT SlotID)
{
	if( E_CONSLOT_BANK != ContainerID)		
	{
		return;
	}

	UItem* tItem = FindItemTemplate(ContainerID, SlotID);
	INT realSlot = SlotID /*- UCONST_MaxWarehouseSlotCount * labelId*/;

  	if( realSlot > ItemSlotList.Num() )
	{
		return;
	}

	UGFxObject* ItemSlot = ItemSlotList(realSlot);

	if(tItem == NULL )
	{
		GFXPlayer->SetItemSlotData( ItemSlot, NULL , ESPType_Warehouse, E_CONSLOT_BANK, SlotID );
#ifdef UI_OPTIMIZATION
		UBOOL isToGray_Last = ItemSlot->GetBool("isToGray");
		if(FALSE!=isToGray_Last)
		{
			eventCallUCGraySlot( ItemSlot, FALSE );
			ItemSlot->SetBool("isToGray",FALSE);
		}
#else
		eventCallUCGraySlot( ItemSlot, FALSE );
#endif
	}		
	else		
	{
		GFXPlayer->SetItemSlotData( ItemSlot, tItem, ESPType_Warehouse, E_CONSLOT_BANK, SlotID );
		UBOOL isToGray = GetItemFilterFrame( tItem , EBLT_NormalItem , CurTab );
		MMOFacade::getSingleton().GrayItem( ESGMark_Bag, E_CONSLOT_BANK, SlotID, isToGray );
	}

}

void UMMOUIWarehouse::HideMsg()
{
	if( ConfirmMsgBox != NULL )
	{
		MMOMsgBoxManager::getSingleton().GetMessageAck( FALSE , ConfirmMsgBox);
		//ConfirmMsgBox->SetVisible( FALSE );
		ConfirmMsgBox = NULL;
	}
}