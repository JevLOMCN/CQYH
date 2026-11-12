#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "UITextParser.h"
#include "UILayout.h"
#include "MMOMsgBoxManager.h"
#include "Shop.h"
#include "MMOGuideManager.h"
#include "MMOShopManager.h"
#include "MMOEquipManager.h"
//const ServerShopData* CurShop = 0;

using namespace std;

IMPLEMENT_CLASS(UMMOUINPCShop)

#define MAXPRICE 100000
	
void UMMOUINPCShop::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_ShopUpdate);
	evts.AddItem(GameEvent_ShopTypeUpdate);
	evts.AddItem(GameEvent_BlackMarket_UpdateItemCoolDown);
	evts.AddItem(GameEvent_BlackMarket_UpdateItemCount);
	evts.AddItem(GameEvent_BagSortEnd);
	EventAdapter = new TGameEventAdapter<UMMOUINPCShop>(this, evts);

	UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	if( data )
	{
		data->SetString( TEXT("titleName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Shop") ) );
		//data->SetString( TEXT("btn0") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.RepairAll") ) );
		data->SetString( TEXT("btn1") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.Repair") ) );
		eventCallSetPanelText(data);
	}
	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}

void UMMOUINPCShop::UnInit()
{
	Super::UnInit();

	MMOUI::GetUIManager()->SetOpenShopDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetOpenBlackMarketDelegate(FScriptDelegate(EC_EventParm));
}

void UMMOUINPCShop::UsebleFilterToggle( UBOOL  bToggle )
{
	IsFilterUseToggle = bToggle;
	Page = 0;
	UpdateListItem();
}

void UMMOUINPCShop::ClearWidget()
{
	ShopManager::getSingleton().ClearCurShopItemList();
	
}

void UMMOUINPCShop::OpenShop( int ShopId )
{
	ShopManager::getSingleton().SetCurShopType(NormalShop);

	ShopManager::getSingleton().CheckShopData();

	ShopManager::getSingleton().SetCurShop(ShopId);

	UpdateShopData();

	//eventCallSelectFilter(ShopId);
	//GrayAllInBagNotInThisShop(TRUE);
	NotifyGrayItemInBag(TRUE);
}

void UMMOUINPCShop::OpenBlackMaret( int id)
{
	ShopManager::getSingleton().SetCurShopType(BlackMarket);
	ShopManager::getSingleton().SetCurShop(id);
	//UpdateShopData();
	CMessageHandler::GetInstance().Send_BlackMarketListInfo();
	CMessageHandler::GetInstance().Send_BlackMarketItemCoolDown();
}

void UMMOUINPCShop::UpdateShopData()
{
	//cleanup
	ClearWidget(); // FIX:
	//No filter by default
	if( ShopContent )
	{
		UGFxObject* filterObj = ShopContent->GetObject( TEXT("checkboxFilter_mc") );
		if(filterObj)
		{
			filterObj->SetBool( TEXT("selected") , false );
		}
	}

	IsFilterUseToggle = false;

	ShopManager::getSingleton().UpdateShopItemList();

	UpdateTabButton();
	UGFxObject* tab = GetObject( TEXT("tab0_mc") );
	if(tab)
	{
		CurTab= tab->GetInt( TEXT("itemid") );
	}


	GFXPlayer->eventCallWidgetDispatchEvent( tab , TEXT("click") );
	NpcShopTab(CurTab);

	ShopManager::getSingleton().ShowShopType();
}

void UMMOUINPCShop::UpdateShopItemList()
{
	//if( CurShop == NULL)
	//	return;
	//for( int i = 0 ; i < CurShop->_ShowItemList.Num() ; i++ )
	//{
	//	const ServerShopItemData& SI = CurShop->_ShowItemList(i);
	//	if( !SI.ShopItem )
	//		continue;
	//	SI.ShopSlot = i;
	//	if( SI.ShopItem->ItemType == TEXT("ITEMTYPE_EQUIP") )
	//		EquipShopItemList.AddItem(SI);
	//	else if( SI.ShopItem->ItemType == TEXT("ITEMTYPE_MATERIAL") )
	//		MaterialShopItemList.AddItem(SI);
	//	else
	//		NormalShopItemList.AddItem(SI);
	//}
}

void UMMOUINPCShop::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch( evt->GetEventType() )
	{
	case GameEvent_ShopUpdate:
		{
			UpdateShopData();
		
			NotifyGrayItemInBag(TRUE);
			//eventCallSelectFilter(ShopManager::getSingleton().GetCurShopId());
		}
		break;
	case GameEvent_ShopTypeUpdate:
		{
			UpdateShopType();
		}
		break;
	case GameEvent_BlackMarket_UpdateItemCount:
		{
			MMOGameEvent_BlackMarket_UpdateItemCount* e = (MMOGameEvent_BlackMarket_UpdateItemCount*)evt;
			if (e)
			{
				UpdateBlackMarketItemCount(e->id,e->count);
			}
			
		}
		break;
	case GameEvent_BlackMarket_UpdateItemCoolDown:
			UpdateCoolDown();
		break;
	case GameEvent_BagSortEnd:
		{
			if (this->GetVisible() == TRUE)
			{
				NotifyGrayItemInBag(TRUE);
			}
		}
		break;
		
	}
}

void UMMOUINPCShop::UpdateShopType()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
		return;

	UGFxObject* dat = GFXPlayer->CreateObject(TEXT("UI_ShopWidget.AS3.ShopTypeData"));
	if (!dat)
		return;

	TArray<INT>* typeList = ShopManager::getSingleton().GetShopTypeList();

	for(int i = 0; i < typeList->Num(); i++)
	{
		switch((*typeList)(i))
		{
		case MONEYTYPE_ACHIEVEMENTPTS:
			{
				dat->SetBool("Achiecement",TRUE);
				INT achiecement = mainPlayer->GetMoney(MONEYTYPE_ACHIEVEMENTPTS);
				dat->SetString("AchiecementNum",UITextParser::HTMLMoneyIconString( MONEYTYPE_ACHIEVEMENTPTS ,achiecement));
			}
			break;
		case MONEYTYPE_MORAL:
			{
				dat->SetBool("Faith",TRUE);
				INT faith = mainPlayer->GetMoney(MONEYTYPE_MORAL);
				dat->SetString("FaithNum",UITextParser::HTMLMoneyIconString( MONEYTYPE_MORAL , faith*1000));
			}
			break;
		case MONEYTYPE_03:
			{
				dat->SetBool("ArenaReward",TRUE);
				INT arenaMoney = mainPlayer->GetMoney(MONEYTYPE_03);
				dat->SetString("ArenaRewardPointNum",UITextParser::HTMLMoneyIconString( MONEYTYPE_03 , arenaMoney));
			}
			break;
		}
	}

	eventCallUpdateShopType(dat);
}

void UMMOUINPCShop::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
	if(MoneyType==MONEYTYPE_MORAL || MoneyType==MONEYTYPE_ACHIEVEMENTPTS || MoneyType==MONEYTYPE_03)
	{
		INT64 money64 = MMOFacade::getSingleton().GetPlayerMoney64((EMONEY_TYPE)MoneyType);
		FString num = UITextParser::HTMLMoneyIconString( MoneyType , money64);
		eventCallUpdateText(num);
	}
}
void UMMOUINPCShop::UpdateMaxPage(  INT curPage, INT maxPage )
{

	UBOOL preEnable = TRUE;
	UBOOL nextEnable = TRUE;
	if( curPage == 0 )
	{
		preEnable = FALSE;
	}
	if( curPage + 1 >= maxPage )
	{
		nextEnable = FALSE;
	}

	if( GFXPlayer )
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
		if(data)
		{
			data->SetBool( TEXT("preEnable") , preEnable );
			data->SetBool( TEXT("nextEnable") , nextEnable );
			data->SetString( TEXT("PageNumTxt") , FString::Printf( TEXT("%d/%d") , curPage + 1 , maxPage ) );

			eventCallSetPageButton( data );
		}
		
	}
	
}

void UMMOUINPCShop::UpdateItemSlot( UGFxObject* itemSlotMc, UBOOL show, const FString& itemName, BYTE currencyType0, INT sellPrice0 , BYTE currencyType1, INT sellPrice1 ,INT itemCount, INT limitType,FString objName)
{
	UGFxObject* itemNameMc = itemSlotMc->GetObject( TEXT("tF_ItemName") );
	if( itemNameMc )
	{
		itemNameMc->SetString( TEXT("htmlText") , show ? itemName : TEXT("") );
	}

	UGFxObject* goldMc = itemSlotMc->GetObject( TEXT("tF_Gold") );
	if( goldMc )
	{		
		goldMc->SetString( TEXT("htmlText") , show ? UITextParser::HTMLMoneyIconString( currencyType0 , sellPrice0 )  : TEXT("") );
	}

	
	UGFxObject* goldMc2 = itemSlotMc->GetObject( TEXT("tF_Gold2") );
	if( goldMc2 )
	{
		FString moneyString;
		if( sellPrice1 != 0 )
		{
			moneyString = show ? UITextParser::HTMLMoneyIconString( currencyType1 , sellPrice1 ) : TEXT("");
		}
		goldMc2->SetString( TEXT("htmlText") , moneyString );
	}

	UGFxObject* countMc = itemSlotMc->GetObject( TEXT("valueAmount") );
	if (countMc)
	{
		FString itemCountString = itemCount > 1 ? FString::FormatAsNumber(itemCount) : TEXT("");
		countMc->SetText(itemCountString);
	}

	if ( ShopManager::getSingleton().GetCurShopType() == BlackMarket && itemCount <= 0 && limitType != 0)
		eventUCGraySlot( objName, TRUE );
	else
		eventUCGraySlot( objName, FALSE );


	UGFxObject* priceTxtMc = itemSlotMc->GetObject( TEXT("tf_dec") );
	if(priceTxtMc)
	{
		priceTxtMc->SetVisible(show);
	}
	
	// TODO: verify whether this is no longer needed
// 	UGFxObject* silverMc = itemSlotMc->GetObject( TEXT("tF_Silver") );
// 	if( silverMc )
// 	{
// 		silverMc->SetText( TEXT("")  );
// 	}
}

//-----------------------------------------------------------------------------------------------

void UMMOUINPCShop::ClearAllItemSlot(INT itemSize)
{
	for(INT i =0 ; i< itemSize ; i++ )
	{
		FString listName = FString::Printf( TEXT( "NPCShopList%d" ), i );
		UGFxObject* mc = ShopContent->GetObject(listName);

		if( mc )
		{
			GFXPlayer->eventClearItemSlot(mc);
			mc->SetVisible(TRUE);
			UpdateItemSlot( mc , FALSE, TEXT("") , 0 , 0 , 0 , 0, 0 ,0,listName);
		}
	}
}

void UMMOUINPCShop::ClearUI()
{
	ClearAllItemSlot(ItemSize);
}


void UMMOUINPCShop::UpdateListItem()
{
	if( !ShopContent )
	{
		return;
	}

	ClearAllItemSlot(ItemSize);
	

	INT Lenth = 0 ;
	ServerShopItemData* SI;
	TArray<ServerShopItemData>* vecAllList;
	TArray<ServerShopItemData*> ItemShowList;
	ItemShowList.Init(0);
	switch( CurTab )
	{
	case E_ITEMTYPE_USABLE:
		vecAllList = ShopManager::getSingleton().GetNormalItemlist();
		break;
	case E_ITEMTYPE_EQUIP:
		vecAllList = ShopManager::getSingleton().GetEquipItemlist();
		break;
	case E_ITEMTYPE_MATERIAL:
		vecAllList = ShopManager::getSingleton().GetMaterialItemlist();
		break;

	case E_ITEMTYPE_MAX: // buyback
		//vecAllList = 0;
		UpdateSoldItemView();
		return;
		break;

	default:
		vecAllList = NULL;
		break;
	}

	if( ! vecAllList )
		return;

	Lenth = vecAllList->Num();
	for(INT i = 0; i < Lenth; ++i)
	{
		SI = &(*vecAllList)(i);
		if(!SI)
			continue;

		if( SI->ShopItem  == NULL )
			continue;

		//if( IsFilterUseToggle  && ! SI->ShopItem->IsUseblForActor( MainPlayer ) )
		if( IsFilterUseToggle  && ( MMOEquipManager::getSingleton().CheckItemUseableForMe( SI->ShopItem ) != ITEM_Use_Err_None ) )
			continue;

		ItemShowList.AddItem( SI );
	}

	Lenth = ItemShowList.Num();

	MaxPage = Lenth / ItemSize;
	if ( Lenth % ItemSize > 0)
	{
		MaxPage++;
	}

	MaxPage = Clamp( MaxPage, 1, MaxPage ); 
	Page = Clamp( Page, 0, MaxPage - 1 );

	UpdateMaxPage( Page, MaxPage);

	for(INT i = 0 ; i < ItemSize ; i++)
	{
		INT TrueIndex = Page * ItemSize + i ;

		if( TrueIndex >= Lenth )
			break;

		SI = ItemShowList(TrueIndex);

		if( SI && SI->ShopItem )
		{
			FString listName = FString::Printf( TEXT( "NPCShopList%d" ) , i );
			UGFxObject* mc = ShopContent->GetObject( listName  );
			if( !mc )
			{
				continue;
			}
			
			if (ShopManager::getSingleton().GetCurShopType() == BlackMarket)
				GFXPlayer->SetItemSlotData( mc , SI->ShopItem, ESPType_BlackMarket, E_CONSLOT_MAX, i);
			else
				GFXPlayer->SetItemSlotData( mc , SI->ShopItem, ESPType_Shop, E_CONSLOT_MAX, i);

			mc->SetInt( TEXT("ItemTemplateID"),  SI->ShopItem->ItemId  );
			mc->SetInt( TEXT("iteminfoid") , SI->ShopSlot);
			mc->SetInt( TEXT("itemdataid") , ShopManager::getSingleton().GetCurShopId() );
			//mc->SetBool( TEXT("enabled") , TRUE );

			UpdateItemSlot( mc, TRUE, UITextParser::HTMLFormatItemColorString(SI->ShopItem) , SI->CostType[0], SI->CostValue[0] , 0, 0 , SI->ItemCount,SI->LimitType,listName);
		}
	}
	
}

void UMMOUINPCShop::NativeInitOnce()
{
	/*UGFxObject* obj = GetObject( TEXT("btn_repairAll") );
	if(obj)
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.RepairAll") ) );

	obj = GetObject( TEXT("btn_repair") );
	if(obj)
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.Repair") ) );*/


}

//------------------------------------------------------------------------
void UMMOUINPCShop::SendBuyShopItem(INT ShopItemId,INT ItemCount,INT ItemTempId)
{

	const ServerShopItemData* itemData = ShopManager::getSingleton().GetCurShopItem( ShopItemId );
	if(itemData)
	{
		if( !MMOFacade::getSingleton().CheckEnoughMoney( (EMONEY_TYPE)itemData->CostType[0] , itemData->CostValue[0]*ItemCount ) )
		{
			return;
		}
		if( !MMOFacade::getSingleton().CheckEnoughMoney( (EMONEY_TYPE)itemData->CostType[1] , itemData->CostValue[1]*ItemCount ) )
		{
			return;
		}
	}	

	CMessageHandler::GetInstance().Send_BuyShopItem( ShopItemId , ItemCount , ItemTempId , MMOShopManager::getSingleton().getShopId());
}

//------------------------------------------------------------------------
void UMMOUINPCShop::NativeRepair()
{
	//MMOFacade::getSingleton().ChangeRepairState();
}
//------------------------------------------------------------------------
void UMMOUINPCShop::ClickRepairAll()
{
	if( !Player || !Player->ItemManager )
	{
		return;
	}

	UBOOL hasCannotRepairItems = FALSE;
	UBOOL needRepair = FALSE;
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	for( INT i = 0 ; i < E_NES_TOTAL; i++ )
	{
		FClientItemInfo *pEquipInfo = mmoFacade.FindItem( E_CONSLOT_EQUIPPED, i );
		if( pEquipInfo == NULL ){
			continue;
		}

		UEquip* equip = Cast<UEquip>( mmoFacade.FindItemTemplate( pEquipInfo->m_nTemplateID ) );
		if( equip == NULL ){
			continue;
		}

		if( pEquipInfo->m_nMaxDur <= 0 || !equip->CanRepair ){
			hasCannotRepairItems = TRUE;
			continue;
		}

		if( pEquipInfo->m_nMaxDur > 0 && ( pEquipInfo->m_nCurDur != pEquipInfo->m_nMaxDur) ){
			needRepair = TRUE;
			break;
		}
	}

	if( needRepair == FALSE && hasCannotRepairItems == TRUE ){
		FString errorName = TEXT("MMOGame.Bag.RepairCurEquipHasCannot");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName);
		return;
	}

	CMessageHandler::GetInstance().Send_RepairAll();

	////Repair all hero equipment, add by Jimzhang
	//AMMOPlayer* pHero = MMOFacade::getSingleton().GetMainHero();
	//if(pHero && pHero->ItemManager)
	//{
	//	for( INT i = 0 ; i < pHero->ItemManager->EquippedItemList_Normal.Num() ; i++ )
	//	{
	//		UEquip* Equip = pHero->ItemManager->EquippedItemList_Normal(i);
	//		if( Equip != NULL )
	//		{
	//			RepairItem( E_CONSLOT_HERO_EQUIPPED , i );
	//		}
	//	}
	//}
}

//------------------------------------------------------------------------
void UMMOUINPCShop::RepairItem(INT ContainerID, INT SlotID)
{
	ItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
	if(itemInfo)
	{
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
		if(itemTemp)
		{
			// Only repair if it is equipment with durability loss
			if(Cast<UEquip>(itemTemp))
			{
				if(itemInfo->m_nCurDur != itemInfo->m_nMaxDur)
				{
					CMessageHandler::GetInstance().Send_RepairItem( ContainerID, SlotID);
				}
			}
		}
	}
}
//------------------------------------------------------------------------
UBOOL UMMOUINPCShop::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
    return FALSE; 
} 

 UBOOL UMMOUINPCShop::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	//if(SlotType == ESPType_Shop)
	//{
	//	// Tutorial event
	//	MMOGameEvent_Guide evtGuide(EGUET_BuyItem);
	//	MMOUI::PostGameEvent(&evtGuide);

	//	INT tempId = mc->GetInt( TEXT("ItemTemplateID") );
	//	if( tempId >= 0 )
	//	{
	//		UItem* item = MMOFacade::getSingleton().FindItemTemplate(tempId);
	//		if(item)
	//		{
	//			if( item->ItemDuration == E_DURTYPE_STACK && item->MaxStackNum > 1 )
	//			{
	//				//Only pop up for stackable multi-item stacks
	//				ShowShopPurchaseDialog(SlotID);
	//			}
	//			else
	//			{
	//				INT shopSlot = mc->GetInt(TEXT("iteminfoid"));
	//				SendBuyShopItem( shopSlot , 1 , tempId );
	//			}
	//		}
	//	}
	//	
	//	return TRUE;
	//}
	if(SlotType == ESPType_Shop)
	{
		MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_ItemSlot, mc);
	}
	return TRUE;
}

UBOOL UMMOUINPCShop::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	INT tmpSlotType = mc->GetInt(TEXT("SlotType"));

	if(SlotType == ESPType_Bag || SlotType == ESPType_BagResource)
	{
		SellItem(ContainerID, SlotID);
		return TRUE;
	}
	else if(SlotType == ESPType_Shop) // right click buys 1 directly
	{
		//INT ShopSlot = mc->GetInt(TEXT("iteminfoid"));
		//INT TempId = mc->GetInt( TEXT("ItemTemplateID") );
		//SendBuyShopItem( ShopSlot , 1 , TempId );
			// Tutorial event
			MMOGameEvent_Guide evtGuide(EGUET_BuyItem);
			MMOUI::PostGameEvent(&evtGuide);

			INT tempId = mc->GetInt( TEXT("ItemTemplateID") );
			if( tempId >= 0 )
			{
				UItem* item = MMOFacade::getSingleton().FindItemTemplate(tempId);
				if(item)
				{
					if( item->ItemDuration == E_DURTYPE_STACK && item->MaxStackNum > 1 )
					{
						if(MMOFacade::getSingleton().GetMainController()->PlayerInput->IsCtrlPressed())
						{
							INT shopSlot = mc->GetInt(TEXT("iteminfoid"));
							SendBuyShopItem( shopSlot , item->MaxStackNum , tempId);
						}
						else
						{
							//Only pop up for stackable multi-item stacks
							ShowShopPurchaseDialog(SlotID);
						}
						
					}
					else
					{
						INT shopSlot = mc->GetInt(TEXT("iteminfoid"));

						if (ShowMessageBox(shopSlot ,ESPType_Shop, 1 , tempId))
						{
							SendBuyShopItem( shopSlot , 1 , tempId );
						}
						
						
					}
				}
			}
		return TRUE;
	}
	else if(SlotType == ESPType_ShopSell) // right click on item in the sell panel to buy back
	{
		CMessageHandler::GetInstance().Send_BuybackItem(SlotID);
		return TRUE;
	}
	else if(SlotType == ESPType_BlackMarket) // black market
	{
		INT id = mc->GetInt(TEXT("iteminfoid"));
		
		if (ShowMessageBox(id ,ESPType_BlackMarket))
		{
			CMessageHandler::GetInstance().Send_BlackMarketBuy(id);
		}
		
	}
	return FALSE;
}

UBOOL UMMOUINPCShop::OnDragDropOnPanel(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	if(SrcSlotType == ESPType_Bag)
	{
		SellItem(SrcContainerID, SrcSlotID);
		return TRUE;
	}
	return FALSE;
}

UBOOL UMMOUINPCShop::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	if(SrcSlotType == ESPType_Bag && (DestSlotType == ESPType_Shop))//||DestSlotType == ESPType_Invalid))
	{
		SellItem(SrcContainerID, SrcSlotID);
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------
void UMMOUINPCShop::ShowShopPurchaseDialog(INT SlotID)
{

	UGFxObject* mc = eventGetSlot(SlotID);
	if(!mc)
	{
		return;
	}
	INT ItemTemplateID = mc->GetInt( TEXT("ItemTemplateID") );

	UItem* ItemTemplate = FindItemTemplateByID(ItemTemplateID);

	if( ItemTemplate == NULL )
		return ;
	
	//show shopMessage
	MMOShopManager::getSingleton().notifyShowShopMessage(TRUE);
	
	/*if( ShopMessage )
	{
		ShopMessage->SetVisible( true );

		INT stackNum = 1;
		if( !Cast<UEquip>( ItemTemplate ) )
		{
			stackNum = ItemTemplate->MaxStackNum;
		}
		UGFxObject* stackTf = ShopMessage->GetObject("stackNum");
		if(stackTf)
			stackTf->SetText( appItoa(stackNum) );

		UGFxObject* numberTF = ShopMessage->GetObject("tF_Number");

		if(numberTF)
			numberTF->SetText( TEXT("1") );

		UGFxObject* buyCountText = ShopMessage->GetObject("buycounttext");
		if(buyCountText)
		{
			UGFxObject* buyCountTextTF = buyCountText->GetObject("textField");
			if(buyCountTextTF)
			{
				buyCountTextTF->SetText( ItemTemplate->GetDisplayName() );
			}

		}

		TArray<	FASValue> args;
		ShopMessage->Invoke( TEXT("setFocus"),args );*/
		
		MessageItem = ItemTemplate;
		MessageItemShopSlot = mc->GetInt(TEXT("iteminfoid"));
		NumChange(1);

		//NumChange();
	//}
	

}

//------------------------------------------------------------
void UMMOUINPCShop::SetupTab(INT idx, BYTE itemType, const FString& tabIconFrame, const FString& tabTips)
{
	//UGFxObject* mc = GetObject( FString::Printf(TEXT("tab%d_mc"), idx) );
	//if( mc )
	//{
	//	mc->SetInt( TEXT("itemid"), itemType );
	//	mc->SetString( TEXT("label"), tabTips );
	//	mc->SetVisible( TRUE );
	//	//UGFxObject* iconMC = mc->GetObject(TEXT("icon_mc"));
	//	
	//	TArray<struct FASValue> asFunArgs;
	//	FASValue asIconPath(EC_EventParm);
	//	asIconPath.Type = AS_String;
	//	asIconPath.S = tabIconFrame;
	//	asFunArgs.AddItem( asIconPath );
	//	mc->Invoke( TEXT("loadPic"), asFunArgs );
	//}
	eventCallUpdateTabLabel(tabTips,idx,itemType);

}


void UMMOUINPCShop::UpdateTabButton()
{
	INT used = 0;
	INT i = 0;
	UGFxObject* mc = 0;
	for(i = 0; i < TabNum; ++i)
	{
		mc = GetObject( FString::Printf(TEXT("tab%d_mc"), i) );
		if(mc)
		{
			mc->SetVisible(false);
		}
	}

	if( ShopManager::getSingleton().GetNormalItemlist()->Num() > 0 )
	{
		SetupTab(used, E_ITEMTYPE_USABLE, TEXT("img://MMO_UIImg.img_tab_shop_3") , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Shop.Item")));
		
		++used;
	}

	if( ShopManager::getSingleton().GetEquipItemlist()->Num() > 0 )
	{
		SetupTab(used, E_ITEMTYPE_EQUIP , TEXT("img://MMO_UIImg.img_tab_shop_1") , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Shop.Equip")));

		++used;
	}

	if( ShopManager::getSingleton().GetMaterialItemlist()->Num() > 0 )
	{
		SetupTab(used, E_ITEMTYPE_MATERIAL, TEXT("img://MMO_UIImg.img_tab_shop_2") , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Shop.Material")));

		++used;
	}

	if(used < TabNum) // buyback
	{
		SetupTab(used, E_ITEMTYPE_MAX, TEXT("img://MMO_UIImg.img_tab_shop_6") , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Shop.Buyback")));
		++used;
	}
}

void UMMOUINPCShop::SoldItemChange()
{
	// If it is a buyback flag
	if(CurTab == E_ITEMTYPE_MAX)
	{
		INT soldItemCnt = MMOFacade::getSingleton().GetSoldItemCount();
		MaxPage = Max(1, appCeil((FLOAT)soldItemCnt / (FLOAT)ItemSize)); 
		Page = 0;
		UpdateSoldItemView();
	}
}

void UMMOUINPCShop::UpdateSoldItemView()
{
	ClearAllItemSlot(ItemSize);

	UpdateMaxPage( Page, MaxPage);
	
	INT soldItemCnt = MMOFacade::getSingleton().GetSoldItemCount();

	INT nIdx = 0;

	for(INT i = Page * ItemSize ; i < soldItemCnt && i < (Page + 1) * ItemSize ; ++i)
	{
		ItemInfo* info = MMOFacade::getSingleton().FindSoldItem(i);
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);

		if(itemTemp)
		{
			FString listName =  FString::Printf( TEXT( "NPCShopList%d" ) , nIdx );
			UGFxObject* mc = ShopContent->GetObject( listName );

			if( !mc )
			{
				continue;
			}

			// Can only be sold for gold coins
			INT price = MMOFacade::getSingleton().GetEquipNpcPrice(itemTemp, info);
			UpdateItemSlot(mc, TRUE, itemTemp->GetDisplayName(), MONEYTYPE_01,
				/*itemTemp->ItemDuration == E_DURTYPE_STACK ? info->m_nCurDur * price :*/ price , 0 , 0 ,info->GetCurStackCount(itemTemp),0,listName);
			
			GFXPlayer->SetItemSlotData( mc , itemTemp, ESPType_ShopSell, E_CONSLOT_MAX, i);
			mc->SetInt( TEXT("ItemTemplateID") , itemTemp->ItemId );
			mc->SetInt( TEXT("iteminfoid") , i);
			mc->SetInt( TEXT("itemdataid") , E_ITEMTYPE_MAX );

			nIdx++;
		}
	}
}

void UMMOUINPCShop::NpcShopTab( INT itemID )
{	
	debugf(TEXT("NpcShopTab:%d"), itemID);
	CurTab = itemID;
	Page = 0 ;
	
	if(E_ITEMTYPE_MAX == CurTab)
	{
		// Request the buyback list when shown
		CMessageHandler::GetInstance().Send_ViewSoldItems();
	}
	else
	{
		UpdateListItem();
	}
}

void UMMOUINPCShop::PagePrev()
{
	Page = Max(0, --Page);
	UpdateListItem();
}

void UMMOUINPCShop::PageNext()
{
	Page = Min(MaxPage - 1, ++Page);
	UpdateListItem();
}

void UMMOUINPCShop::SellItem(INT containerID, INT slotID)
{
	ItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(containerID, slotID);
	if(itemInfo)
	{
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
		if(itemTemp)
		{
			if( itemTemp->ItemColor >= ITEMCOLOR_BLUE )
			{
				FScriptDelegate fun(EC_EventParm);
				fun.Object = this;
				fun.FunctionName = TEXT("OnSellItem");

				FScriptDelegate funNone(EC_EventParm);
				funNone.Object = this;
				funNone.FunctionName = TEXT("OnCancelSellItem");	

				FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.SellConfirm") );
				FString txt = FString::Printf( *temp,  *UITextParser::HTMLFormatItemName(itemTemp) );
				UGFxObject* msgMc = MMOMsgBoxManager::getSingleton().AddModalMessageBox( txt , containerID , fun , funNone );
				if(msgMc)
					msgMc->SetInt( TEXT("itemdataid") , slotID);
			}
			else
			{
				DoSellItem(containerID, slotID);
			}
		}
	}
}

void UMMOUINPCShop::DoSellItem(INT containerID, INT slotID)
{
	ItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(containerID, slotID);

	if(itemInfo)
	{
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
		if(itemTemp)
		{
			INT sellType = ShopManager::getSingleton().GetCurShopSellType();

			BOOL canSell = TRUE;

			INT bindType = BINDTYPE_NONE;
			if( itemInfo != NULL ){
				bindType = itemInfo->m_byBindType;
			}
			else if( itemTemp != NULL ){
				bindType = itemTemp->ItemBindType;
			}

			if (bindType != E_BINDTYPE_NONE && itemInfo->m_nTemplateID != 2800309 &&  itemInfo->m_nTemplateID != 2800310   &&  itemInfo->m_nTemplateID != 2800313)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.CannotSellBindItem") ) , MsgInfoType_Warning , TEXT("MMOGame.Shop.CannotSellBindItem") );
				return;
			}
			if (!itemTemp->CanSell)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOErrorInfo.ERROR_CODE_DEF.errItemCannotSell") ) , MsgInfoType_Warning , TEXT("MMOErrorInfo.ERROR_CODE_DEF.errItemCannotSell") );
				return;
			}
			
			if (sellType !=0 && itemTemp->ShopTypeSell !=0)
			{
				if (sellType != itemTemp->ShopTypeSell)
					canSell = FALSE;
			}
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if(mainPlayer->GetVipType() >= 0  )
			{
				warnf(TEXT("==============="));
				canSell = TRUE;
			}
			if (canSell == FALSE)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOErrorInfo.ERROR_CODE_DEF.errShopTypeSell") ) , MsgInfoType_Warning , TEXT("MMOErrorInfo.ERROR_CODE_DEF.errShopTypeSell") );
				return;
			}

			// If it is equipment, do not use durability as the sell quantity
			if(Cast<UEquip>(itemTemp))
			{
				CMessageHandler::GetInstance().Send_SellItem( containerID, slotID, 1 , itemInfo->m_nTemplateID );
			}
			else // for non-equipment, durability represents stack count
			{
				CMessageHandler::GetInstance().Send_SellItem( containerID, slotID, itemInfo->m_nCurDur , itemInfo->m_nTemplateID );
			}
		}
	}
}

void UMMOUINPCShop::OnSellItem(class UGFxObject* mc)
{
	INT containerId = mc->GetInt(TEXT("iteminfoid"));
	INT slotID = mc->GetInt(TEXT("itemdataid"));

	DoSellItem(containerId, slotID);
}

void UMMOUINPCShop::OnCancelSellItem(class UGFxObject* mc)
{
}

void UMMOUINPCShop::NativeInitLoad()
{	
	Super::NativeInitLoad();

	//g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}


void UMMOUINPCShop::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

	if (show)
	{
		// Tutorial event
		MMOGameEvent_Guide evtGuide(EGUET_ShowShopPanel);
		MMOUI::PostGameEvent(&evtGuide);

		//Hide shopMessage
		MMOShopManager::getSingleton().notifyShowShopMessage(FALSE);
		//SetSlotEnable();

	}
	else{
		// Tutorial event
		MMOGameEvent_Guide evtGuide(EGUET_CloseShopPanel);
		MMOUI::PostGameEvent(&evtGuide);

		NotifyGrayItemInBag(FALSE);
	}
	
	ShopManager::getSingleton().SetShopShow(show);

}



void UMMOUINPCShop::UnitWidget()
{
	ShopManager::getSingleton().ClearCurShopItemList();
}

void UMMOUINPCShop::NumChange(int count)
{
	//INT count = appAtoi( *ShopMessage->GetObject( TEXT("tF_Number") )->GetString(TEXT("text")) );

	if( MessageItemShopSlot < ShopManager::getSingleton().GetCurShopItemNum() )
	{
		const ServerShopItemData* itemData = ShopManager::getSingleton().GetCurShopItem(MessageItemShopSlot);
		if(itemData)
		{

			//UGFxObject* msgText = ShopMessage->GetObject( TEXT("MsgText_mc"));
			//if(msgText)
			//	msgText->SetString( TEXT("htmlText") , UITextParser::HTMLMoneyIconString( itemData->CostType[0] , itemData->CostValue[0]*count ) );

			//UGFxObject* msgText2 = ShopMessage->GetObject( TEXT("MsgText2_mc"));
			//if(msgText2)
			//{
			//	if( itemData->CostValue[1] > 0 )
			//	{
			//		msgText2->SetString( TEXT("htmlText") , UITextParser::HTMLMoneyIconString( itemData->CostType[1] , itemData->CostValue[1]*count ) );
			//	}
			//	else
			//	{
			//		msgText2->SetString( TEXT("htmlText") , TEXT("") );
			//	}
			//}

			FString const1 = UITextParser::HTMLMoneyIconString( itemData->CostType[0] , itemData->CostValue[0]*count );
			FString const2 = "";
			if( itemData->CostValue[1] > 0 )
			{
				const2 = UITextParser::HTMLMoneyIconString( itemData->CostType[1] , itemData->CostValue[1]*count );
			}
			MMOShopManager::getSingleton().notifyUpdateShopMessageMoney(const1,const2);
		}		
			
	}
}

void UMMOUINPCShop::ShopMsgAck( UBOOL agree , int count)
{
	if( agree && ( MessageItem != NULL) )
	{
		Clamp( count ,1,99);
		SendBuyShopItem( MessageItemShopSlot , count , MessageItem->GetTemplateID() );
		
	}
	//Hide shopMessage
	MMOShopManager::getSingleton().notifyShowShopMessage(FALSE);
}

//---------------------------moved-------------------------------------

void UMMOUINPCShop::ASNPCShopRepair()
{
	eventClickRepair();

}

void UMMOUINPCShop::ASNPCShopRepairAll()
{
	ClickRepairAll();
}

void UMMOUINPCShop::ASNPCShopClose()
{

}
void UMMOUINPCShop::ASShowTips_RepairAll( UGFxObject* butt )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_RepairAll , butt );
}

void UMMOUINPCShop::ASHideTips_RepairAll( UGFxObject* butt )
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUINPCShop::ASShowRepairTips( UGFxObject* butt )
{
	butt->SetString( TEXT("tipsHtml"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.Repair") ) );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, butt );
}

void UMMOUINPCShop::ASHideRepairTips( UGFxObject* butt )
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUINPCShop::PostAdvance(FLOAT inDeltaTime )
{
	if(IsTargetOutDistanceClose&&(this->IsVisible))
	{
		AMMOActor* selectActor = Cast<AMMOActor>(MMOFacade::getSingleton().GetMainController()->GetSelectTarget());
		if(!selectActor)
		{
			Show(FALSE);
		}
		else
		{
			static INT fTimeSpan = 0;
			if(++fTimeSpan > 10)
			{
				fTimeSpan = 0;//delay the distance check once
				INT CurDistance = (selectActor->Location-MMOFacade::getSingleton().GetMainController()->Pawn->Location).Size2D();
				if(CurDistance >= nTargetMaxDistance)
					Show(FALSE);
			}
		}
	}//end of if IsVisible
}

UBOOL UMMOUINPCShop::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if ((key == "MouseScrollUp" || key == "MouseScrollDown") && (strHoverControllerName == ""))
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

void UMMOUINPCShop::SetSlotEnable()
{
	for(INT i = 0 ; i < 10 ; i++)
	{
		
		UGFxObject* mc = ShopContent->GetObject(  FString::Printf( TEXT( "NPCShopList%d" ) , i ) );
		if( !mc )
		{
				continue;
		}

		mc->SetBool( TEXT("enabled") , FALSE );
		
	}
}

void UMMOUINPCShop::UpdateBlackMarketItemCount(INT id, INT count)
{
	for(INT i = 0 ; i < ShopManager::getSingleton().GetCurShopItemNum() ;  i++)
	{
		FString name = FString::Printf( TEXT( "NPCShopList%d" ) , i );
		UGFxObject* mc = ShopContent->GetObject(  name );
		if( !mc )
		{
			continue;
		}

		if( mc->GetInt( TEXT("iteminfoid") ) == id)
		{
			UGFxObject* countMc = mc->GetObject( TEXT("valueAmount") );
			const ServerShopItemData* itemData = ShopManager::getSingleton().GetCurShopItem( i );

			if (!itemData)
				return;

			FString itemCountString = count > 1 ? FString::FormatAsNumber(count) : TEXT("");
			if (countMc)
			{
				countMc->SetText(itemCountString);
			}
			
			//Gray out
			if ( count <= 0 && itemData->LimitType != 0)
				eventUCGraySlot( name, TRUE );
			else
				eventUCGraySlot( name, FALSE );

			
			//CD
			if (itemData->MaxCoolDownTime > 0)
			{
				SetItemSlotCD(mc,itemData->MaxCoolDownTime,itemData->MaxCoolDownTime);
			}
			
			break;
		}
	}
}

void UMMOUINPCShop::UpdateCoolDown()
{
	for(INT i = 0 ; i < ShopManager::getSingleton().GetCurShopItemNum() ; i++)
	{
		FString name = FString::Printf( TEXT( "NPCShopList%d" ) , i );

		UGFxObject* mc = ShopContent->GetObject(  name );
		
		const ServerShopItemData* itemData = ShopManager::getSingleton().GetCurShopItem( i );

		if( !mc || !itemData)
		{
			continue;
		}

		if (itemData->CurrentCoolDown <= 0)
		{
			StopItemSlotCD(mc);
		}
		else
		{
			SetItemSlotCD(mc,itemData->CurrentCoolDown,itemData->MaxCoolDownTime);
		}
		
	}
}

void UMMOUINPCShop::NotifyGrayItemInBag(BOOL v)
{
	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		MMOGameEvent_GaryBagItemByNpcShop evt(v);
		MMOUI::PostGameEvent(&evt);
	}
	
}

BOOL UMMOUINPCShop::ShowMessageBox(INT ShopItemId,BYTE SlotType,INT ItemCount,INT ItemTempId)
{
	BOOL show = TRUE;

	const ServerShopItemData* itemData = ShopManager::getSingleton().GetCurShopItem( ShopItemId );

	if (itemData)
	{
		INT32 price0 = 0;
		INT32 price1 = 0;

		if (itemData->CostType[0] == MONEYTYPE_02 ||itemData->CostType[1] == MONEYTYPE_02 )
		{
			price0 = itemData->CostValue[0];
			price1 = itemData->CostValue[1];

			if (price0 > MAXPRICE || price1 > MAXPRICE)
			{
				if( ConfirmMsgBox != NULL )
				{
					ConfirmMsgBox->SetVisible( FALSE );
					ConfirmMsgBox = NULL;
				}

				FScriptDelegate AuctionDelegate(EC_EventParm);
				AuctionDelegate.FunctionName = TEXT("NpcShopMsg_OK");
				AuctionDelegate.Object = this;

				FScriptDelegate Nofunction(EC_EventParm);
				Nofunction.FunctionName = TEXT("NpcShopMsg_Cancel");
				Nofunction.Object = this;

				ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.Buy") ) , ShopItemId , AuctionDelegate , Nofunction );
				if (ConfirmMsgBox)
				{
					if(ConfirmMsgBox)
					{
						ConfirmMsgBox->SetInt( TEXT("itemType") , SlotType);
						ConfirmMsgBox->SetInt( TEXT("itemCount") , ItemCount);
						ConfirmMsgBox->SetInt( TEXT("itemTempId") , ItemTempId);
					}
				}

				show = FALSE;
			}
			
		}
	}
	

	return show;
}

void UMMOUINPCShop::NpcShopMsg_OK(UGFxObject* mc)
{
	if (NULL == mc)
		return;
	INT itemId = mc->GetInt( TEXT("iteminfoid"));
	INT itemType = mc->GetInt( TEXT("itemType"));
	INT itemCount = mc->GetInt( TEXT("itemCount"));
	INT itemTempId = mc->GetInt( TEXT("itemTempId"));

	if (itemType == ESPType_Shop)
	{
		SendBuyShopItem( itemId , itemCount , itemTempId );
	}
	else if (itemType == ESPType_BlackMarket)
	{
		CMessageHandler::GetInstance().Send_BlackMarketBuy(itemId);
	}
	 
}
void UMMOUINPCShop::NpcShopMsg_Cancel(UGFxObject* mc)
{
	mc = NULL;
}