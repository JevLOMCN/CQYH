#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "MMOMsgBoxManager.h"
#include "UILayout.h"
#include "UITextParser.h"
#include "MMOShopManager.h"
#include "Shop.h"
#include "ClientActionConditionCheck.h"
#include "MMOEquipManager.h"

IMPLEMENT_CLASS(UMMOUIBagResource)

#define BagResource_TabNum 3
#define BageResource_Page0 72
#define BageResource_Page1 144
#define BageResource_Page2 216
extern UBOOL g_bMouseInPanel;

//temporarily added
#define BageResource_Money1 10000
#define BageResource_Money2 20000
#define BageResource_Money3 30000

void UMMOUIBagResource::Init()
{
	Super::Init();

	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_SortTimer);
	evts.AddItem(GameEvent_SortTimerBegin);
	evts.AddItem(GameEvent_SortTimerEnd);
	evts.AddItem(GameEvent_ClearBag);
	evts.AddItem(GameEvent_BagSortEnd);
	evts.AddItem(GaemEvent_BagTabChange);
	evts.AddItem(GaemEvent_BagResourceSizeChange);
	evts.AddItem(GameEvent_GrayBagItemByShop);
	//evts.AddItem(GameEvent_Item_Delete);
	evts.AddItem(GameEvent_BagSlotHintChange);
	evts.AddItem(GameEvent_EnterSelectingRole);
	evts.AddItem(GameEvent_BagResSellAll);

	EventAdapter = new TGameEventAdapter<UMMOUIBagResource>(this, evts);

	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}
	pageIdCur = 0;
	lastPage = 0;
	bOpenByNpcShop = FALSE;

	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("BagResourceMoney1") , ExpandMoney1 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("BagResourceMoney2") , ExpandMoney2 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("BagResourceMoney3") , ExpandMoney3 , GDefaultGameIni );

	//temporarily added
	ExpandMoney1 = BageResource_Money1;
	ExpandMoney2 = BageResource_Money2;
	ExpandMoney3 = BageResource_Money3;

	//g_layoutList.push_back(new UILayoutRightLeft(this, MMOUI::GetUIManager()->Panel_Bag));

	InitBagResourcePageTab();

	for(INT i=0;i<UCONST_MaxBagResourceSlotTotalCount;i++)
	{
		HintSlotLast[i] = 0;
	}

	BtnSell_mc = GetObject(TEXT("BtnSell_mc"));
	if (BtnSell_mc)
	{
		BtnSell_mc->SetBool(TEXT("enabled"),FALSE);
	}

	bChangePlayer = TRUE;

	InitShowPanelList();

}

void UMMOUIBagResource::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUIBagResource::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);
	if (show)
	{
		if (bChangePlayer)
		{
			pageIdCur = 0;
			bChangePlayer = FALSE;
			if (bagResourceTab_0)
			{
				bagResourceTab_0->SetBool(TEXT("selected"),TRUE);
			}
			
		}

		BagTab(CurTab);
		UpdateBagResourcePageTab();
		ShowPanelPos();
	}
}

void UMMOUIBagResource::NativeOnShow(UBOOL v)
{
	if(v)
	{
		BagPage( pageIdCur );

	}

}

void UMMOUIBagResource::SetItemSlotData( class UGFxObject* mc, class UItem* tItem, BYTE SlotType, BYTE ContainerID , INT SlotID , BYTE inISS )
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
void UMMOUIBagResource::BeforeViewUpdate(float DeltaTime)
{

	if(onShowDelay>0)
	{
		onShowDelay--;
		if(onShowDelay<=0)
		{
			if(IsVisible){
				NativeOnShow(TRUE);
			}
		}
	}

	if(MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}
	BagPageRefresh();
}

void UMMOUIBagResource::BagPageRefresh()
{
	if(slotCountCur<0)return;
	if(slotCountDown<0)
	{
		slotCountDown++;
		return;
	}

	if(this->IsVisible==FALSE)
	{
		slotCountCur=-1;
		return;
	}

	INT pageId = pageIdCur;
	INT slotCountNext =  Min(slotCountCur+16,UCONST_MaxBagResourceSlotCount);


	INT currentContainer = E_CONSLOT_RESOURCE;
	INT labelId = CurLabel;


	AMMOPlayer* pl=  MMOFacade::getSingleton().GetMainPlayer();
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if(!pl)
	{
		return;
	}

	if(!BagGroup)
	{
		return;
	}

	for( INT i =slotCountCur ; i < slotCountNext ; i++ )
	{

		INT CurCapacity = pl->ItemManager->CurCapacity(currentContainer);
		UGFxObject* itemSlot = BagGroup->GetObject( FString::Printf(TEXT("slot%d_mc"),i) );
		if( itemSlot == NULL )
		{			
			continue;
		}

		//UGFxObject* filterAnim = itemSlot->GetObject( TEXT("filterAnim") );
		

		INT realSlot = i + UCONST_MaxBagResourceSlotCount * pageId;

		FClientItemInfo* item = mmoFacade.FindItem( currentContainer, realSlot );
		HintItem( currentContainer, realSlot, item );

		if( realSlot >= CurCapacity )
		{			
			GFXPlayer->SetItemSlotData( itemSlot , NULL, ESPType_BagResource , currentContainer, realSlot, ISS_SlotLocked );

#ifdef UI_OPTIMIZATION
			UBOOL isToGray_Last = itemSlot->GetBool("isToGray");
			if(FALSE!=isToGray_Last)
			{
				eventCallUCGraySlot( itemSlot, FALSE );
				itemSlot->SetBool("isToGray",FALSE);
			}
#else
			eventCallUCGraySlot( itemSlot, FALSE );
#endif

			continue;
		}

		if(item == NULL )
		{
			GFXPlayer->SetItemSlotData( itemSlot , NULL, ESPType_BagResource, currentContainer, realSlot );

#ifdef UI_OPTIMIZATION
			UBOOL isToGray_Last = itemSlot->GetBool("isToGray");
			if(FALSE!=isToGray_Last)
			{
				eventCallUCGraySlot( itemSlot, FALSE );
				itemSlot->SetBool("isToGray",FALSE);
			}
#else
			eventCallUCGraySlot( itemSlot, FALSE );
#endif
			continue;
		}

		
		//HintItem( currentContainer, realSlot, item );

		UItem* itemTemp = mmoFacade.FindItemTemplate(item->m_nTemplateID);
		if( itemTemp )
		{
			GFXPlayer->SetItemSlotData(itemSlot, itemTemp, ESPType_BagResource, currentContainer, realSlot, ISS_Normal);

			if (bOpenByNpcShop == FALSE)
			{
				UBOOL isToGray = GetItemFilterFrame( itemTemp , labelId , CurTab );
				mmoFacade.GrayItem( ESGMark_Bag, currentContainer, realSlot, isToGray );
			}

		}
	}



	if(slotCountNext>=UCONST_MaxBagResourceSlotCount)
	{
		slotCountCur = -1;
	}
	else
	{
		slotCountCur+=16;
	}
}


void UMMOUIBagResource::BagPage(INT labelId)
{
	pageIdCur=labelId;

	if (this->GetVisible() == FALSE)
		return;
	
	slotCountCur=0;
	slotCountDown=-1;//things in bag slots start refreshing one frame later
}
void UMMOUIBagResource::BagTab(INT tabId)
{
	//CurTab = tabId;

	//if (bOpenByNpcShop == FALSE)
	//{
		BagPage(pageIdCur);
	//}
	
}

void UMMOUIBagResource::InitBagResourcePageTab()
{
	bagResourceTab_0 = GetObject(TEXT("tab0_mc"));
	bagResourceTab_1 = GetObject(TEXT("tab1_mc"));
	bagResourceTab_2 = GetObject(TEXT("tab2_mc"));
	//UpdateBagResourcePageTab();
}

void UMMOUIBagResource::UpdateBagResourcePageTab()
{
	AMMOPlayer* pl=  MMOFacade::getSingleton().GetMainPlayer();
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if(!pl)
	{
		return;
	}

	INT num = pl->ItemManager->CurCapacity(E_CONSLOT_RESOURCE);

	//for (INT i = 0 ; i < UCONST_MaxBagResourceSlotTotalCount; i++)
	//{
	//	INT realSlot = i + UCONST_MaxBagResourceSlotCount * pageIdCur;
	//	FClientItemInfo* item = mmoFacade.FindItem( E_CONSLOT_RESOURCE, realSlot );
	//	if (item)
	//	{
	//		UItem* itemTemp = mmoFacade.FindItemTemplate(item->m_nTemplateID);
	//		if( itemTemp )
	//		{
	//			num++;
	//		}
	//	}
	//}
	if (num < BageResource_Page0)
	{
		UpdateBageBagResourcePageTabEnable(TRUE,FALSE,FALSE);
	}
	else if (num >= BageResource_Page0 && num < BageResource_Page1)
	{
		UpdateBageBagResourcePageTabEnable(TRUE,TRUE,FALSE);
	}
	else 
	{
		UpdateBageBagResourcePageTabEnable(TRUE,TRUE,TRUE);
	}

}

void UMMOUIBagResource::UpdateBageBagResourcePageTabEnable(BOOL v1,BOOL v2, BOOL v3)
{
	if (bagResourceTab_0 && bagResourceTab_1 && bagResourceTab_2)
	{
		bagResourceTab_0->SetBool(TEXT("enabled"),v1);
		bagResourceTab_1->SetBool(TEXT("enabled"),v2);
		bagResourceTab_2->SetBool(TEXT("enabled"),v3);
	}
}

void UMMOUIBagResource::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GaemEvent_BagTabChange:
		{
			MMOGameEvent_Bag_TabChange* msg = (MMOGameEvent_Bag_TabChange*)evt;
			if(msg)
			{
				CurTab = msg->curTab;
				BagPage(pageIdCur);
			}
		}
		break;
	case GameEvent_ClearBag:
		{
			if (MMOFacade::getSingleton().GetPlayerSortType() == IST_BagResource)
			{
				ClearUI();
			}
		}
		break;
	case GameEvent_BagSortEnd:
		{
			if (MMOFacade::getSingleton().GetPlayerSortType() == IST_BagResource)
			{
				BagPage(pageIdCur);
				MMOFacade::getSingleton().SetPlayerSortType(IST_Invalid);
			}

		}
		break;
	case GaemEvent_BagResourceSizeChange:
		{
			BagPage(pageIdCur);
			UpdateBagResourcePageTab();
		}
		break;

	case GameEvent_GrayBagItemByShop:
		{
			MMOGameEvent_GaryBagItemByNpcShop * msg = (MMOGameEvent_GaryBagItemByNpcShop*)evt;
			if (msg)
			{
				GrayAllInBagNotInNpcShop(msg->bShow);
			}

		}
		break;
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

	case GameEvent_BagSlotHintChange:
		{
			MMOGameEvent_BagSlotHintChange* msg = (MMOGameEvent_BagSlotHintChange*)evt;
			if(msg && msg->ConID == E_CONSLOT_RESOURCE)
			{
				HintItem( msg->ConID , msg->SlotID , msg->ItemInfo );
			}

		}
		break;
	case GameEvent_EnterSelectingRole:
		{
			bChangePlayer = TRUE;
		}
		break;
	case GameEvent_BagResSellAll:
		{
			BagPage(pageIdCur);
		}
		break;
	}
}

void UMMOUIBagResource::BagSlotChange(INT ContainerID, INT SlotID)
{
	if( ContainerID == E_CONSLOT_RESOURCE )
	{
		if(IsVisible)
		{
			BagSlotChangeEx(ContainerID, SlotID);
		}
	}
	if( ConfirmMsgBox != NULL ){
		HideMsg();
	}
}
void UMMOUIBagResource::BagSlotChangeEx(INT ContainerID, INT SlotID)
{
	if( E_CONSLOT_RESOURCE != ContainerID)		
	{
		return;
	}

	UItem* item = FindItemTemplate(ContainerID, SlotID);
	FClientItemInfo *clientItemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);

	INT pageSlotId =  GetCurrentPageSlotId(SlotID);;

	if( pageIdCur == 0 && SlotID >= BageResource_Page0 )
	{
		return;
	}
	else if(pageIdCur == 1 && (SlotID >= BageResource_Page1 || SlotID < BageResource_Page0))
	{
		return;
	}
	else if (pageIdCur == 2 && (SlotID >= BageResource_Page2 ||  SlotID < BageResource_Page1))
	{
		return;
	}
	
	if (pageSlotId >= ItemSlotList.Num())
	{
		return;
	}

	UGFxObject* itemSlot = ItemSlotList(pageSlotId);

	if(itemSlot)
	{
		UBOOL isToGray = GetItemFilterFrame( item , CurLabel , CurTab );
		MMOFacade::getSingleton().GrayItem( ESGMark_Bag, ContainerID, SlotID, isToGray );

		//when shop is open
		if (ShopManager::getSingleton().GetShopShow())
		{
			UBOOL gray = GrayItemInBagNotInNpcShop(clientItemInfo,item);
			if (gray)
			{
				MMOFacade::getSingleton().GrayItem( ESGMark_Bag, ContainerID, SlotID, gray );
			}

		}

	}

	HintItem( E_CONSLOT_RESOURCE , SlotID , clientItemInfo );

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{

		GFXPlayer->SetItemSlotData( itemSlot, item, ESPType_BagResource, ContainerID, SlotID );
	}
}

void UMMOUIBagResource::ClearUI()
{
	INT cnt = MMOFacade::getSingleton().GetContainerSize(E_CONSLOT_BANK);
	for (INT i = 0; i < cnt; ++i)
	{
		UGFxObject* itemSlot = BagGroup->GetObject(FString::Printf(TEXT("slot%d_mc"), i));
		GFXPlayer->SetItemSlotData( itemSlot, NULL, ESPType_Warehouse, E_CONSLOT_BANK, i);
	}
}


void UMMOUIBagResource::HideMsg()
{
	if( ConfirmMsgBox != NULL )
	{
		//ConfirmMsgBox->SetVisible( FALSE );
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( ConfirmMsgBox );
		ConfirmMsgBox = NULL;
	}
}


//------------------------------------------------------------------------
UBOOL UMMOUIBagResource::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 


UBOOL UMMOUIBagResource::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	//if(bDefault)return FALSE;

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(  mc && mc->GetBool( "isSlotLocked") && SlotType == ESPType_BagResource && ContainerID == E_CONSLOT_RESOURCE )
	{
		INT CurCapacity = player->ItemManager->CurCapacity( ContainerID );
		if( SlotID > CurCapacity - 1 && SlotID < UCONST_MaxBagResourceSlotTotalCount)
		{
			INT needMoney = 0;

			for ( INT i = 0 ; i < SlotID + 1 - CurCapacity ; i++ )
			{		

				//int num = GetCurPage(SlotID);

				if (SlotID < BageResource_Page0)
				{
					needMoney  += ExpandMoney1;
				} 
				else if(SlotID >= BageResource_Page0 && SlotID < BageResource_Page1)
				{
					needMoney  += ExpandMoney2;
				}
				else
				{
					needMoney  += ExpandMoney3;
				}

			}

			FScriptDelegate fun(EC_EventParm);
			fun.Object = this;
			fun.FunctionName = TEXT("ExpandBagResourceOk");

			FScriptDelegate funNone(EC_EventParm);
			funNone.Object = this;
			funNone.FunctionName = TEXT("ExpandBagResourceNo");	

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

UBOOL UMMOUIBagResource::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(!bDefault){//bag is default handling, handle it after other UIs are done
		return FALSE;
	}


	switch (SlotType)
	{
	case ESPType_BagResource:			
		{
			switch (ContainerID)
			{
			case E_CONSLOT_RESOURCE:			
				{
					if (MoveItemToBag(E_CONSLOT_DEFAULT, ContainerID, SlotID) == FALSE)
					{
						UBOOL isUsed = UseContainerItem(ContainerID, SlotID );
					}
					return TRUE;
				}
			default:
				return FALSE;
			}
		}
		break;
	}
	return FALSE;
}

BOOL UMMOUIBagResource::MoveItemToBag(BYTE SlotType,BYTE ContainerID,INT SlotID)
{
	UMMOUIBag *panel = Cast<UMMOUIBag>(MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag ));
	if (panel && panel->GetVisible())
	{
		ECONTAINER_SLOT desContainerID = (ECONTAINER_SLOT)SlotType;

		ItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
		if(!itemInfo)
		{
			return FALSE;
		}

		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);
		if(!itemTemp)
		{
			return FALSE;
		}

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
			CMessageHandler::GetInstance().Send_MoveItem( desContainerID , emptySlot , ContainerID  , SlotID);
			return TRUE;
		}				
		else
		{
			FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
			return TRUE;
		}


	}
	return FALSE;
}
UBOOL UMMOUIBagResource::UseContainerItem( INT conIdx, INT slotIdx )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	FClientItemInfo* pItemInfo = mmoFacade.FindItem( conIdx, slotIdx );
	if( pItemInfo == NULL ){
		return FALSE;
	}
	UItem *pItem = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
	if( pItem == NULL ){
		return FALSE;
	}

	if (pItem->ItemType == TEXT("ITEMTYPE_MATERIAL"))
	{
		if (pItem->ItemSubType == TEXT("ITEMSUBTYPE_MATERIAL_MISC") /*|| pItem->ItemSubType == TEXT("ITEMSUBTYPE_MATERIAL_WEAPON_LEVELUP")*/)
		{
			ClientActionCondition_OperationMaterial materialOperationChecker;

			FClientActionConditionContex materialOperationContex;
			materialOperationContex.Item = pItemInfo;
			materialOperationContex.ItemTemplate = pItem;
			if( !materialOperationChecker.CheckClientActionCondition( materialOperationContex ) ){
				return FALSE;
			}

			CMessageHandler::GetInstance().Send_UseItem( conIdx, slotIdx, pItemInfo->m_nTemplateID );
			mmoFacade.GetMainPlayer()->BeginOperateMaterial();

			return TRUE;
		}

	}

	return FALSE;
}

//==========================================================================================================
//items to gray out
void UMMOUIBagResource::GrayItem( BYTE ContainerID, INT SlotID, struct FClientItemInfo *pItemInfo )
{
	UGFxObject *pItemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc"), SlotID ) );

	if( pItemSlot == NULL ){
		return;
	}

	if( pItemInfo == NULL ){
		return;
	}

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

UBOOL UMMOUIBagResource::GetItemFilterFrame( UItem* item , INT label , INT filterTab )
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


UBOOL UMMOUIBagResource::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	FClientItemInfo* srcItemInfo = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
	if( srcItemInfo == NULL )
	{
		return FALSE;
	}

	if(SrcSlotType == ESPType_BagResource && DestSlotType == ESPType_Invalid) // discard
	{
		INT actorLevel = MMOFacade::getSingleton().GetMyLevel();
		if( actorLevel < s_ActorLvAllowGameSystem ){
			FString strString = TEXT("MMOErrorInfo.ERROR_CODE_DEF.actorLevelTooLow");
			FString errorStr = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( strString ), s_ActorLvAllowGameSystem );
			MMOUI::GetUIManager()->delegateAddScreenMessage( errorStr, MsgInfoType_Table, strString );
			return TRUE;
		}

		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(srcItemInfo->m_nTemplateID);

		ClientActionCondition_DelItem checker;

		FClientActionConditionContex contex;
		contex.Item = srcItemInfo;
		contex.ItemTemplate = itemTemp;

		if(!checker.CheckClientActionCondition(contex))
		{
			return TRUE;
		}
		if (srcItemInfo  && !g_bMouseInPanel)
		{
			DeleteItem( SrcContainerID , SrcSlotID , srcItemInfo->m_nTemplateID );
		}

		return TRUE;
	}
	else if(SrcSlotType == ESPType_BagResource && DestSlotType == ESPType_BagResource)  // swap
	{
		if(SrcContainerID!=DestContainerID)
		{
			return FALSE;
		}

		//check whether locked by UI function lock
		//source item check
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(srcItemInfo->m_nTemplateID);

		ClientActionCondition_SwampItem checker;

		FClientActionConditionContex contex;
		contex.Item = srcItemInfo;
		contex.ItemTemplate = itemTemp;

		if(!checker.CheckClientActionCondition(contex))
		{
			return TRUE;
		}

		//target item check

		FClientItemInfo *destItemInfo = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
		if(destItemInfo)
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(destItemInfo->m_nTemplateID);

			ClientActionCondition_SwampItem checker;

			FClientActionConditionContex contex;
			contex.Item = destItemInfo;
			contex.ItemTemplate = itemTemp;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}
		}
		//
		if((SrcSlotID != DestSlotID) && srcItemInfo)
		{
			CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
			return TRUE;
		}
	}
	else if((SrcSlotType == ESPType_BagResource && DestSlotType == ESPType_Bag)
		|| (SrcSlotType == ESPType_Bag && DestSlotType == ESPType_BagResource)) // swap
	{
		if(MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID))
		{
			CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
			
		}
		return TRUE;
	}

	return FALSE;
}

void UMMOUIBagResource::DeleteItem( INT ConId , INT SlotId , INT TemplateId )
{
	UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate( TemplateId );
	FClientItemInfo* item = MMOFacade::getSingleton().FindItem( ConId , SlotId );
	if( item && itemTemp )
	{
		INT stack = 0;
		stack = item->GetCurStackCount(itemTemp);
		CMessageHandler::GetInstance().Send_DeleteItem( ConId , SlotId , stack , TemplateId );
	}
	
}
void UMMOUIBagResource::GrayAllInBagNotInNpcShop(BOOL isToGray)
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( Player == NULL ){
		return;
	}

	bOpenByNpcShop = isToGray;

	SetSellBtnEnable(bOpenByNpcShop);

	//INT sellType = ShopManager::getSingleton().GetCurShopSellType();

	UINT bagCapability = Player->ItemManager->CurCapacity( E_CONSLOT_RESOURCE );

	for( UINT j = 0; j < bagCapability; ++j)
	{

		FClientItemInfo *pItemInfo = mmoFacade.FindItem( E_CONSLOT_RESOURCE, j );
		if( pItemInfo == NULL )
		{
			continue;
		}

		UItem *pItemInBag = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
		if( pItemInBag == NULL )
		{
			continue;
		}



		if (isToGray)
		{
			UBOOL gray = GrayItemInBagNotInNpcShop(pItemInfo,pItemInBag);
			mmoFacade.GrayItem( ESGMark_Bag, E_CONSLOT_RESOURCE, j, gray );
		}
		else //do not gray out
		{
			BagPage(pageIdCur);
		}

	}
}

BOOL UMMOUIBagResource::GrayItemInBagNotInNpcShop(FClientItemInfo *pItemInfo,UItem *pItemInBag)
{
	UBOOL gray = FALSE;

	if (pItemInfo && pItemInBag)
	{
		INT sellType = ShopManager::getSingleton().GetCurShopSellType();

		INT bindType = BINDTYPE_NONE;
		if( pItemInfo != NULL ){
			bindType = pItemInfo->m_byBindType;
		}
		else if( pItemInBag != NULL ){
			bindType = pItemInBag->ItemBindType;
		}

		//needs graying out
		if(!pItemInBag->CanSell)
		{
			gray = TRUE;
		}
		else if (sellType != pItemInBag->ShopTypeSell)
		{
			gray = TRUE;
		}
		else if(bindType != E_BINDTYPE_NONE)
		{
			gray = TRUE;
		}
		else
		{
			gray = FALSE;
		}
	}


	return gray;
}


void UMMOUIBagResource::ExpandBagResourceOk( UGFxObject* mc)
{
	//INT containerId = mc->GetInt(TEXT("iteminfoid"));
	INT count = mc->GetInt(TEXT("itemdataid"));
	CMessageHandler::GetInstance().Send_BuyContainerSlot( E_CONSLOT_RESOURCE , count );
}

//------------------------------------------------------------------------
void UMMOUIBagResource::ExpandBagResourceNo( UGFxObject* mc)
{
	ConfirmMsgBox = NULL;
}

void UMMOUIBagResource::LockItem( BYTE ContainerID, INT SlotID, INT isToLockIn )
{
	UGFxObject *pItemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc"), SlotID ) );
	if( pItemSlot == NULL ){
		return;
	}
	pItemSlot->SetBool( TEXT("MarkLocked"), isToLockIn >= 0 );
}

void UMMOUIBagResource::ASMaterialTypeTabChange(int id)
{
	if (id == pageIdCur)
		return;

	pageIdCur = id;
	//HideAllHint();

	BagPage(pageIdCur);
	lastPage = pageIdCur;
}

void UMMOUIBagResource::ASMaterialSellAll()
{
	if (bOpenByNpcShop)
	{
		FScriptDelegate fun(EC_EventParm);
		fun.Object = this;
		fun.FunctionName = TEXT("SellBagResourceAllOk");

		FScriptDelegate funNone(EC_EventParm);
		funNone.Object = this;
		funNone.FunctionName = TEXT("SellBagResourceAllNo");	

		FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BagResource.SellAll") );

		HideMsg();

		ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( temp , 0 , fun , funNone );
	}
	
}

void UMMOUIBagResource::SellBagResourceAllOk(UGFxObject* mc)
{
	if (bOpenByNpcShop)
	{
		/*AMMOPlayer* pl=  MMOFacade::getSingleton().GetMainPlayer();
		MMOFacade &mmoFacade = MMOFacade::getSingleton();
		if(!pl)
		{
			return;
		}

		INT num = pl->ItemManager->CurCapacity(E_CONSLOT_RESOURCE);

		for(INT i = 0; i <num ;i ++)
		{
			UMMOUINPCShop *pShopPanel = (UMMOUINPCShop*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Shop );
			if( pShopPanel != NULL ){

				FClientItemInfo* item = mmoFacade.FindItem( E_CONSLOT_RESOURCE, i );
				if (item)
				{
					pShopPanel->SellItem( E_CONSLOT_RESOURCE, i);
				}
			}
		}*/
		CMessageHandler::GetInstance().Send_SellAllItemInBagResource();
	}
	
	
}
void UMMOUIBagResource::SellBagResourceAllNo(UGFxObject* mc)
{
	ConfirmMsgBox = NULL;
}

void UMMOUIBagResource::ItemDelete(INT ContainerID, INT SlotID)
{
	if( ContainerID == E_CONSLOT_RESOURCE && BagGroup )
	{
		if(IsVisible)
		{
			ClearItemSlot(ContainerID, SlotID);
		}
	}
}

void UMMOUIBagResource::ClearItemSlot( BYTE ContainerID, int SlotID)
{
	if(!BagGroup)
		return;

	INT pageSlotId = GetCurrentPageSlotId(SlotID);

	if( pageIdCur == 0 && SlotID >= BageResource_Page0 )
	{
		return;
	}
	else if(pageIdCur == 1 && (SlotID >= BageResource_Page1 || SlotID < BageResource_Page0))
	{
		return;
	}
	else if (pageIdCur == 2 && (SlotID >= BageResource_Page2 ||  SlotID < BageResource_Page1))
	{
		return;
	}

	UGFxObject* itemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc") , pageSlotId ) );	

	GFXPlayer->SetItemSlotData( itemSlot, NULL, ESPType_BagResource, ContainerID, SlotID );
}

void UMMOUIBagResource::ASSortMaterialBag()
{
	ClientActionCondition_ItemsSort checker;

	FClientActionConditionContex contex;
	contex.Item = NULL;
	contex.ItemTemplate = NULL;
	contex.UsefulInt = E_CONSLOT_DEFAULT;

	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	// #24372
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		if (mainPlayer->MoveType == EMT_PrepareStall ||
			mainPlayer->MoveType == EMT_InStall)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.CanNotSortBagInStall"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Bag.CanNotSortBagInStall"));
			return;
		}
	}

	CMessageHandler::GetInstance().Send_SortBag(E_CONSLOT_RESOURCE);
	MMOFacade::getSingleton().StartSortTimer( LSM_SortBagResource );
	MMOFacade::getSingleton().SetPlayerSortType(IST_BagResource);
}

void UMMOUIBagResource::HintItem( BYTE ContainerID, INT SlotID, struct FClientItemInfo *pItemInfo )
{
	INT slotId = GetCurrentPageSlotId(SlotID);

	if (slotId >= 0)
	{
		if( !pItemInfo )
		{
			if(HintSlotLast[SlotID]!=0)
			{
				HintSlotLast[SlotID]=0;
			}
			eventCallUCHintSlot( slotId, FALSE , FALSE );
			return;
		}
		if( pItemInfo->NeedHintMode == ItemSlotHint_Normal )
		{
			if(HintSlotLast[SlotID]!=0)
			{
				HintSlotLast[SlotID]=0;
			}
			eventCallUCHintSlot( slotId, FALSE , FALSE );
		}
		/*else if( ( pItemInfo->NeedHintMode | ItemSlotHint_PowerEquip ) == pItemInfo->NeedHintMode )
		{
		if(HintSlotLast[SlotID]!=2)
		{
		HintSlotLast[SlotID]=2;
		eventCallUCHintSlot( slotId, TRUE , FALSE );
		}
		}*/
		else if(HintSlotLast[SlotID]!=3 && pItemInfo->NeedHintMode == ( pItemInfo->NeedHintMode | ItemSlotHint_New ))
		{
			HintSlotLast[SlotID]=3;
			eventCallUCHintSlot( slotId, TRUE , TRUE );
		}
		else if(HintSlotLast[SlotID]==3)
		{
			eventCallUCHintSlot( slotId, TRUE , TRUE );
		}
	}

	
}

void UMMOUIBagResource::HideAllHint()
{
	for( INT i =0 ; i < UCONST_MaxBagResourceSlotCount ; i++ )
	{
		eventCallUCHintSlot( i, FALSE , FALSE );
	}
}

void UMMOUIBagResource::SetSellBtnEnable(BOOL v)
{
	if (BtnSell_mc)
	{
		BtnSell_mc->SetBool(TEXT("enabled"),v);
	}
}

INT UMMOUIBagResource::GetCurrentPageSlotId(INT realSlotId)
{
	INT id = realSlotId % UCONST_MaxBagResourceSlotCount;
	
	return id;
}


UBOOL UMMOUIBagResource::OnEsc()
{
	NotifyHideBagResource();
	Super::OnEsc();
	return TRUE;
}

void UMMOUIBagResource::ASOnMaterialPanelClose()
{
	NotifyHideBagResource();
}

void UMMOUIBagResource::NotifyHideBagResource()
{
	MMOGameEvent_Any evt( GameEvent_BagResourceHide );
	MMOUI::PostGameEvent(&evt);
}

void UMMOUIBagResource::InitShowPanelList()
{
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_BackDecorated);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_Warehouse);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_Guild_Warehouse);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_Shop);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_WeaponSkillRplc);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_TradeP2P);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_EquipLevelUp);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_Stall_StallSetting);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_WeaponSkill);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_SoulEmbed);
	showPanelList.AddItem(MMOUI::GetUIManager()->Panel_SoulLevelUp);
}

void UMMOUIBagResource::ShowPanelPos()
{
	BOOL showbagPanel = TRUE;

	for(int i = 0 ; i < showPanelList.Num();i++)
	{
		UMMOUIBase* leftPanel = MMOUI::FindUIPanel(showPanelList(i));
		if (leftPanel && leftPanel ->IsVisible)
		{
			UpdatePos(leftPanel);
			showbagPanel = FALSE;
			break;
		}
	}
	if (showbagPanel)
	{
		UpdatePosByBag();
	}
}

void UMMOUIBagResource::UpdatePos(UMMOUIBase* leftPanel)
{
	UMMOUIBase* midPanel = MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Bag);

	FVector2D scrSz = MMOFacade::GetScreenSize();
	INT rightW = this->eventGetWidth();
	INT rightH = this->eventGetHeight();

	if (leftPanel ->IsVisible && midPanel && midPanel->IsVisible)
	{

		INT leftW = leftPanel->eventGetWidth();
		INT leftH = leftPanel->eventGetHeight();

		INT midW = midPanel->eventGetWidth();
		INT midH = midPanel->eventGetHeight();

		INT panelX = (scrSz.X - leftW - rightW - midW) / 3;
		INT panelY = (scrSz.Y - Max(leftH, rightH)) / 2;

		leftPanel->SetPosition(panelX, panelY);
		midPanel->SetPosition(panelX + leftW, panelY);
		this->SetPosition(panelX + leftW + midW, panelY);
	}
}

void UMMOUIBagResource::UpdatePosByBag()
{
	UMMOUIBase* midPanel = MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Bag);

	FVector2D scrSz = MMOFacade::GetScreenSize();
	INT rightW = this->eventGetWidth();
	INT rightH = this->eventGetHeight();

	if (midPanel && midPanel->IsVisible)
	{
		INT leftW = midPanel->eventGetWidth();
		INT leftH = midPanel->eventGetHeight();

		INT panelX = (scrSz.X - leftW - rightW) / 2;
		INT panelY = (scrSz.Y - Max(leftH, rightH)) / 2;

		midPanel->SetPosition(panelX, panelY);
		this->SetPosition(panelX + leftW, panelY);
	}
}