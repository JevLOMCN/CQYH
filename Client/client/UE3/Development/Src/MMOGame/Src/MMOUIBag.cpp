#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "ClientActionConditionCheck.h"
#include "MMOChatManager.h"
#include "CommonGameEvents.h"
#include "MMOMsgBoxManager.h"
#include "EquipDecomposeManager.h"
#include "MMOGuideManager.h"
#include "MMOCoolDownManager.h"
#include "UITextParser.h"
#include "MMOQuestManager.h"
#include "MMOEquipManager.h"
#include "MMOAuctionManager.h"
#include "UILayout.h"
#include "ScaleformEngine.h"
#include "MMOAttributeAccountManager.h"
#include "MMOClientVariableManager.h"
#include "MallManager.h"
#include "MMOShopManager.h"
#include "Shop.h"

IMPLEMENT_CLASS(UMMOUIBag)

extern bool g_lockMouseCursorForUI;


void ClearUIEx(class UMMOUIBag* bag, ECONTAINER_SLOT containerType, UMMOGFXPlayer* gfxPlayer)
{
	INT cnt = MMOFacade::getSingleton().GetContainerSize(containerType);
	for (INT i = 0; i < cnt; ++i)
	{
		UGFxObject* itemSlot = bag->BagGroup->GetObject(FString::Printf(TEXT("slot%d_mc"), i));
		gfxPlayer->SetItemSlotData( itemSlot, NULL, ESPType_Bag, containerType, i);
	}
}

void UMMOUIBag::Init()
{
	
	Super::Init();

	bOpenByNpcShop = FALSE;
	bOpenResBag = TRUE;

	LabelBagPage.AddZeroed(E_CONSLOT_MAX);

	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_ClearBag);
	evts.AddItem(GameEvent_SortTimer);
	evts.AddItem(GameEvent_SortTimerBegin);
	evts.AddItem(GameEvent_SortTimerEnd);
	evts.AddItem(GameEvent_MsgBoxItemIdentifyAsk);
	evts.AddItem(GameEvent_NewCD);
	evts.AddItem(GameEvent_ChainPayChange);
	evts.AddItem(GameEvent_OpenChainPay);
	evts.AddItem(GameEvent_BagSlotHintChange);
	evts.AddItem(GameEvent_BagOpenPack);
	evts.AddItem(GameEvent_BagWeight);
	evts.AddItem(GameEvent_BagSortEnd);
	evts.AddItem(GameEvent_ExpContainerChanged);
	evts.AddItem(GameEvent_RealMoneyChange);
	evts.AddItem(GameEvent_GrayBagItemByShop);
	evts.AddItem(GameEvent_BagResourceHide);

	EventAdapter = new TGameEventAdapter<UMMOUIBag>(this, evts);

	UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	if(data)
	{
		data->SetString( TEXT("titleName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.Title") ) );

		data->SetString( TEXT("splitButton") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.SplitButton") ) );
		data->SetString( TEXT("sortButton") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.SortButton") ) );
		data->SetString( TEXT("signButton") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.SignButton") ) );
		data->SetString( TEXT("identifyButton") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.IdentifyButton") ) );
		data->SetString( TEXT("stallButton") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.StallButton") ) );

		data->SetString( TEXT("tipsMoney0") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.Money0") ) );
		data->SetString( TEXT("tipsMoney1") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.Money1") ) );
		//data->SetString( TEXT("tipsWeight") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.Weight") ) );
		
		 

		UGFxObject* tabArray = GFXPlayer->CreateArray();
		if( tabArray )
		{
			for ( INT i = 0 ; i < EBTT_MAX ; i++ )
			{
				UGFxObject* tabData = GFXPlayer->CreateObject( TEXT("Object") );
				tabData->SetInt( TEXT("typeId") , i );
				FString tabName = TEXT("MMOGame.Bag.Tab");
				tabName += appItoa(i);
				tabName = MMOUI::ParseLocalizedPropertyPath( tabName );
				tabData->SetString( TEXT("text") , tabName );

				tabArray->SetElementObject(i,tabData);	
			}
			data->SetObject( TEXT("tabData") , tabArray );
		}
		
		UGFxObject* labelArray = GFXPlayer->CreateArray();
		if(labelArray)
		{
			for ( INT i = 0 ; i < EBLT_MAX ; i++ )
			{
				UGFxObject* labelData = GFXPlayer->CreateObject( TEXT("Object") );
				labelData->SetInt( TEXT("typeId") , i );
				FString tabName = TEXT("MMOGame.Bag.Label");
				tabName += appItoa(i);
				tabName = MMOUI::ParseLocalizedPropertyPath( tabName );
				labelData->SetString( TEXT("text") , tabName );

				labelArray->SetElementObject(i,labelData);	
			}
			
			data->SetObject( TEXT("labelData") , labelArray );
		}

		eventCallSetPanelText(data);
	}


	SetChainPay();

	g_layoutList.push_back(new UILayoutRightLeft(this, MMOUI::GetUIManager()->Panel_Character));

	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("BagBaseMoney1") , BaseMoney1 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("BagBaseMoney2") , BaseMoney2 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("BagIncreaseRatio1") , IncreaseRatio1 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("BagIncreaseRatio2") , IncreaseRatio2 , GDefaultGameIni );
	GConfig->GetInt( TEXT("UnlockNeedMoney"), TEXT("BagStage") , stage , GDefaultGameIni );

	for(INT i=0;i<UCONST_MaxBagSlotCount;i++)
	{
		HintSlotLast[i] = 0;
	}

	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}

	//experience bottle
	UGFxObject * pGfxObj = GetObject( TEXT("ExpContainerPanelMc") );
	if( pGfxObj != NULL ){
		ExpContainerSlot = pGfxObj->GetObject( TEXT("ExpContainerSlot") );
		if( ExpContainerSlot != NULL ){
			ExpContainerSlot->SetInt( TEXT("SlotType"), ESPType_ExpContainer );
		}
	}
	CurLoadedExpContainer._ConID = E_CONSLOT_DEPOSIT;
	CurLoadedExpContainer._SlotID = DEPOSIT_EXP_SLOT;
}

void UMMOUIBag::NativeInitLoad()
{	
	Super::NativeInitLoad();

#ifdef UI_OPTIMIZATION
	//after loading, immediately add to the Priority layer,
	INT	panelPriority = this->GFXPlayer->GetPanelPriority(FriendlyName);
	this->SetInt("CurPriority",panelPriority+1);	
	eventShowPanel(TRUE, panelPriority); // use the newly refactored interface
#endif	
}

void UMMOUIBag::PostOnShow(UBOOL show)
{
	// bag is the default ItemSlotActionListener, so no need to set it to current
	if(show==FALSE)
	{
		MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();
		eventSetBagTab(0);
		BagTab(0);
	}
	else
	{
#if SHIPPING_PC_GAME
		CMessageHandler::GetInstance().Send_MallAskRealMoney();
#endif
	}

	NativeOnShow(show);
	Super::PostOnShow(show);

	MMOGameEvent_Guide evtGuide(EGUET_ShowBagPanel, show ? 1 : 0);
	MMOUI::PostGameEvent(&evtGuide);
}

void UMMOUIBag::ClearUI()
{
	ECONTAINER_SLOT containerType = (ECONTAINER_SLOT)GetBagEnum();

	//
	//BagSlotState.Empty();
	//QuestBagSlotState.empty();
	HasAskToLearnSkillBook = false;

	ClearUIEx(this, containerType, GFXPlayer);
}

void UMMOUIBag::UpdateBagCapSlot(INT ContainerID)
{
	BagPage( LabelBagPage(CurLabel) );

}

void AskEquipBind(ItemInfo* info)
{
	UGFxObject* MsgBox = NULL;
	
	UMMOUIBag* bag = Cast<UMMOUIBag>(MMOUI::GetUIManager()->FindUIPanel(TEXT("Bag")));

	FScriptDelegate Fun(EC_EventParm);
	Fun.Object = bag;

	FScriptDelegate FunNone(EC_EventParm);
	Fun.FunctionName = TEXT("OnEquipItemOK");				

	MsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox(
		MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.BindOnEquip") ),
		info->m_byConSlot, Fun, FunNone );
	if(MsgBox)
		MsgBox->SetInt(TEXT("itemdataid"), info->m_byItemSlot);
}

UBOOL UMMOUIBag::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
    return FALSE; 
} 

void UMMOUIBag::ExpandBagOk(UGFxObject *mc)
{
	INT containerId = mc->GetInt(TEXT("iteminfoid"));
	INT count = mc->GetInt(TEXT("itemdataid"));
	CMessageHandler::GetInstance().Send_BuyContainerSlot( containerId , count );
}

void UMMOUIBag::ExpandBagNo()
{
	ConfirmMsgBox = NULL;
}

void UMMOUIBag::HideBag_Msg()
{
	if( ConfirmMsgBox != NULL )
	{
		//ConfirmMsgBox->SetVisible( FALSE );
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( ConfirmMsgBox );
		ConfirmMsgBox = NULL;
	}
}

UBOOL UMMOUIBag::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	
	if( mc && mc->GetBool( "isSlotLocked")  && SlotType == ESPType_Bag && ContainerID == E_CONSLOT_DEFAULT )
	{
		MMOGameEvent_Guide evtGuide(EGUET_Extendbag);
		MMOUI::PostGameEvent(&evtGuide);

        if( ContainerID >= player->ItemManager->CurCapacity.Num() )
        {
            return FALSE;
        }

		INT CurCapacity = player->ItemManager->CurCapacity( ContainerID );
		if( SlotID > CurCapacity - 1 )
		{
			INT needMoney = 0;
			
			for ( INT i = 0 ; i < SlotID + 1 - CurCapacity ; i++ )
			{
				//needMoney += Min( 50 + 5 * ( i + CurCapacity - UCONST_OriBagCap ) , 200 ) ;
				
				int num = i + CurCapacity - UCONST_OriBagCap;

				if (num < stage)
				{
					needMoney += BaseMoney1 + IncreaseRatio1 * num ;
				} 
				
				else 
				{
					needMoney += BaseMoney2 + IncreaseRatio2 * (num - stage);
				}
			
				
			}


			FScriptDelegate fun(EC_EventParm);
			fun.Object = this;
			fun.FunctionName = TEXT("ExpandBagOk");

			FScriptDelegate funNone(EC_EventParm);
			funNone.Object = NULL;
			funNone.FunctionName = TEXT("ExpandBagNo");	

			FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.UnlockNeedMoney") );
			FString txt = FString::Printf( *temp , ( SlotID +1 - CurCapacity ) ) + UITextParser::HTMLMoneyIconString( MONEYTYPE_02 , needMoney );

			HideBag_Msg();

			ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( txt , ContainerID , fun , funNone );
			if(ConfirmMsgBox)
				ConfirmMsgBox->SetInt( TEXT("itemdataid") , SlotID + 1 - CurCapacity );

			return TRUE;
		}

	}
	/*if(MMOFacade::getSingleton().GetMainController()->PlayerInput->IsCtrlPressed())
	{
		UMMOUIAuctionView* auction = Cast<UMMOUIAuctionView>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_AuctionView));
		if (auction && auction->IsVisible)
		{
			MMOAuctionManager::getSingleton().NotifyQuickSearchByBag(SlotType,ContainerID,SlotID);
			return TRUE;
		}
		MMOGameEvent_Chat_AppendInput evt(SlotType,ContainerID,SlotID,bDefault);
		MMOUI::PostGameEvent(&evt);
		return TRUE;
	}*/

	return FALSE;
}

void UMMOUIBag::NativeInitOnce()
{

	UGFxObject* obj = GetObject( TEXT("sign_btn_mc") );
	if (obj)
	{
		obj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.ItemSign")));
	}

	BagGroup = GetObject("BagGroup_mc");

	if( BagGroup != NULL )
	{
		for(INT i =0 ; i < UCONST_MaxBagSlotCount ; i++ )
		{
			UGFxObject* ItemSlot = BagGroup->GetObject(FString::Printf(TEXT("slot%d_mc"),i));
			if( ItemSlot != NULL )
			{
				ItemSlot->SetFloat("itemid",i); 
				//Strange effect here
				ItemSlotList.AddItem( ItemSlot );
			}
		}
	}

}

void UMMOUIBag::OnDecomposeEquipOk()
{

}

void UMMOUIBag::OnDecomposeEquipCancel()
{

}

UBOOL UMMOUIBag::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(!bDefault){//bag is default handling, handle it after other UIs are done
		return FALSE;
	}
	switch (SlotType)
	{
	case ESPType_Bag:			
		{
			switch (ContainerID)
			{
			case E_CONSLOT_DEFAULT:			
				{
					if (MoveItemToBagResource(E_CONSLOT_RESOURCE, ContainerID, SlotID) == FALSE)
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
	case ESPType_ExpContainer:			
		{
			UBOOL isOperated = OnRightMouseClickInExpContainer( SlotID );
			return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL UMMOUIBag::MoveItemToBagResource(BYTE SlotType,BYTE ContainerID,INT SlotID)
{
	UMMOUIBagResource *panel = Cast<UMMOUIBagResource>(MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_BagResource ));
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
			FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_BagResource_Full"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_BagResource_Full"));
			return TRUE;
		}


	}
	return FALSE;
}

//======================================================================
UBOOL UMMOUIBag::UseContainerItem( INT conIdx, INT slotIdx )
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

	if( pItem->ItemType == TEXT("ITEMTYPE_EQUIP") ){
		UEquip* pEquip = Cast<UEquip>(pItem);
		if( pEquip && pEquip->GetEquipAppearenceTemplate() ){

			MMOFacade::getSingleton().SetFingerPosShowType(TRUE);

			INT destCon = 0;
			INT destSlot = 0;
			MMOUI::GetEquippedSlotByEquipType( pEquip->GetEquipAppearenceTemplate()->EquipType, destCon, destSlot );
			if( destCon == E_CONSLOT_EQUIPPED ){
				//equipment that will be swapped in
				ClientActionCondition_EquipItem checker;
				FClientActionConditionContex contex;
				contex.Item = pItemInfo;
				contex.ItemTemplate = pItem;
				if( !checker.CheckClientActionCondition( contex ) ){
					return TRUE;
				}

				//equipment that will be swapped out
				FClientItemInfo* destItemInfo = mmoFacade.FindItem( E_CONSLOT_EQUIPPED, destSlot );
				UItem* itemTemp = NULL;
				if( destItemInfo ){
					itemTemp = mmoFacade.FindItemTemplate( destItemInfo->m_nTemplateID );

					ClientActionCondition_UnEquipItem checker;

					FClientActionConditionContex contex;
					contex.Item = destItemInfo;
					contex.ItemTemplate = itemTemp;

					if( !checker.CheckClientActionCondition(contex) ){
						return FALSE;
					}
				}

				INT nWeight = 0;
				if( itemTemp ){
					nWeight = itemTemp->Weight;
				}

				//first check whether weight is sufficient and whether it can be worn
				if( pItem->ItemType == TEXT("ITEMTYPE_EQUIP") && pItem->ItemSubType != TEXT("ITEMSUBTYPE_EQUIP_WEAPON") ){
					if( mmoFacade.CheckWeight( pItem->Weight, nWeight, WT_Armor ) == FALSE ){
						return FALSE;
					}
				}
				else if( pItem->ItemType == TEXT("ITEMTYPE_EQUIP") && pItem->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON") ){
					if( mmoFacade.CheckWeight( pItem->Weight, nWeight, WT_Weap ) == FALSE ){
						return FALSE;
					}
				}
			}
			else if( destCon == E_CONSLOT_HERO_EQUIPPED ){
				ClientActionCondition_Hero_EquipItem checker;
				FClientActionConditionContex contex;
				contex.Item = pItemInfo;
				contex.ItemTemplate = pItem;

				if( !checker.CheckClientActionCondition( contex ) ){
					return FALSE;
				}						
			}
			else{
				return FALSE;
			}

			// if it needs binding but is not bound yet
			if( pItemInfo->m_byBindType != E_BINDTYPE_BINDED && pEquip->ItemBindType == E_BINDTYPE_BOE ){
				AskEquipBind( pItemInfo );
			}
			else{								
				CMessageHandler::GetInstance().Send_MoveItem( destCon, destSlot, conIdx, slotIdx );
			}
		}

		MMOFacade::getSingleton().SetFingerPosShowType(FALSE);
		return TRUE;
	}
	else if( pItem->ItemSubType == TEXT("ITEMSUBTYPE_DEPOSIT_EXP") ){
		if( pItemInfo->m_nCurDur < pItemInfo->m_nMaxDur ){
			//stored value not full, attach and continue storing
			CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_DEPOSIT, DEPOSIT_EXP_SLOT, E_CONSLOT_DEFAULT, slotIdx );

			eventCallAsShowExpContainer( TRUE );

			return TRUE;
		}
	}
	else if ( pItem->ItemType == TEXT("ITEMTYPE_RECOVER") ){
		CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_RECOVER, 0, E_CONSLOT_DEFAULT, slotIdx );
		return TRUE;
	}
	else if (pItem->ItemType == TEXT("ITEMTYPE_MATERIAL"))
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


	ClientActionCondition_UseItem checker;

	FClientActionConditionContex contex;
	contex.Item = pItemInfo;
	contex.ItemTemplate = pItem;
	if( !checker.CheckClientActionCondition( contex ) ){
		return FALSE;
	}


	//special item: blessing oil
	if( pItem->GetTemplateID() == MMOUI::GetUIManager()->LuckyOilItemId ){
		FClientItemInfo* pMainHandCurEquippedItemInfo = mmoFacade.FindItem( E_CONSLOT_EQUIPPED , EET_Normal_MainHand );
		if( pMainHandCurEquippedItemInfo ){
			CMessageHandler::GetInstance().Send_UseItem( conIdx, slotIdx, pItemInfo->m_nTemplateID );
		}
		else{
			MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.NoWeaponCantUseOil")) );
		}
	}
	// if there is a script, send directly to server for use
	else if( pItem->Script.Len() > 0 ){
		CMessageHandler::GetInstance().Send_UseItem( conIdx, slotIdx, pItemInfo->m_nTemplateID );
	}
	else if( pItem->ItemType == TEXT("ITEMTYPE_SPELLBOOK") ){
		SkillBookMsgBox( slotIdx );
	}
	else if( pItem->ItemType == TEXT("ITEMTYPE_FORMULA") ){
		//if(item->GetToUseAbility()){
		EquipDecomposeManager::getSingleton().BeginCompose( pItem->GetTemplateID());
		//}
	}
	else if( pItem->ItemType == TEXT("ITEMTYPE_USABLE") ){
		CMessageHandler::GetInstance().Send_UseItem( conIdx, slotIdx, pItemInfo->m_nTemplateID );
		//return FALSE;
	}
	else if( pItem->ItemType == TEXT("ITEMTYPE_DEPOSIT") && pItem->ItemSubType == TEXT("ITEMSUBTYPE_DEPOSIT_EXP") ){
		if( pItemInfo->m_nCurDur >= pItemInfo->m_nMaxDur ){
			//stored value full, use
			UBOOL isShowConfirmBox = FALSE;
			GConfig->GetBool( TEXT("PopUpMsgBox"), TEXT("ConfirmUseExpContainer"), isShowConfirmBox , GDefaultGameIni );
			if( isShowConfirmBox ){
				FScriptDelegate funYes(EC_EventParm);
				funYes.Object = this;
				funYes.FunctionName = TEXT("ConfirmUseExpContainer");

				FScriptDelegate funNo(EC_EventParm);
				funNo.Object = this;
				funNo.FunctionName = TEXT( "CancelUseExpContainer" );

				FString strTitle;
				FString strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ExpContainer.ConfirmUsingDepositItemForExperience") );
				if( ConfirmMsgBox != NULL ){
					HideBag_Msg();
				}

				ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBoxWithCheck( strTitle, 0, strContent, funYes, funNo, TRUE );
				if( ConfirmMsgBox != NULL ){
					ConfirmMsgBox->SetBool( TEXT("forbidHint"), FALSE );
					ConfirmMsgBox->SetInt( TEXT("itemdataid"), slotIdx );
				}
			}
			else{
				CMessageHandler::GetInstance().Send_UseItem( conIdx, slotIdx, pItemInfo->m_nTemplateID );
			}
		}
	}
	else{
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.CannotUse") ) );
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.CannotUse") ), MsgInfoType_Table, TEXT("MMOGame.Bag.CannotUse") );
	}

	return FALSE;
}
//----------------------------------------------------------------------
UBOOL UMMOUIBag::OnRightMouseClickInExpContainer( INT slotIdx )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	FClientItemInfo* pItemInfo = mmoFacade.FindItem( E_CONSLOT_DEPOSIT, slotIdx );
	if( pItemInfo == NULL ){
		return FALSE;
	}
	UItem *pItem = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
	if( pItem == NULL ){
		return FALSE;
	}

	if( pItem->ItemType == TEXT("ITEMTYPE_DEPOSIT") ){
		INT destSlot = MMOFacade::getSingleton().GetEmptyItemSlot( E_CONSLOT_DEFAULT );
		if( destSlot < 0 ){
			//bag has no space
			FString errStrKey = TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full");
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrKey ), MsgInfoType_Table, errStrKey );
			return FALSE;
		}

		CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_DEFAULT, destSlot, E_CONSLOT_DEPOSIT, slotIdx );

		return TRUE;
	}

	return FALSE;
}
//
extern UBOOL g_bMouseInPanel;

UBOOL UMMOUIBag::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	FClientItemInfo* srcItemInfo = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
	if( srcItemInfo == NULL )
	{
		return FALSE;
	}

	if(SrcSlotType == ESPType_Bag )
	{
		switch( DestSlotType )
		{
		case ESPType_Invalid:// discard from bag
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

				//DelegateDeleteItem(SrcContainerID, SrcSlotID);
				if (srcItemInfo  && !g_bMouseInPanel)
				{
					DeleteItem( SrcContainerID , SrcSlotID , srcItemInfo->m_nTemplateID );
				}

				return TRUE;
			}
			break;
		case ESPType_Bag:
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
			break;
		case ESPType_Hero://hero equipment
			{
				UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(srcItemInfo->m_nTemplateID);
				UEquip* equip = Cast<UEquip>(itemTemp);

				ClientActionCondition_Hero_EquipItem checker;

				FClientActionConditionContex contex;
				contex.Item = srcItemInfo;
				contex.ItemTemplate = itemTemp;

				if(!checker.CheckClientActionCondition(contex))
				{
					return TRUE;
				}

				if(itemTemp && equip && srcItemInfo->m_byBindType != E_BINDTYPE_BINDED
					&& equip->ItemBindType == E_BINDTYPE_BOE) // if not bound
				{
					AskEquipBind(srcItemInfo);
				}
				else
				{
					CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
				}

				return TRUE;
			}
			break;
		case ESPType_Recover:
			{
				//whether it is a mana blood stone
				if (SrcContainerID == E_CONSLOT_RECOVER && DestContainerID == E_CONSLOT_DEFAULT)
				{
					FClientItemInfo* destItemInfo = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
					if(destItemInfo == NULL)
					{
						AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
						UItem* tItem = Player->ItemManager->GetItemBySlot( E_CONSLOT_RECOVER , 0 );
						if(tItem)
						{
							UItem* itemTemplate = MMOFacade::getSingleton().FindItemTemplate(tItem->ItemId);
							if( itemTemplate&&itemTemplate->ItemType == TEXT("ITEMTYPE_RECOVER") )
							{
								CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_DEFAULT,DestSlotID,E_CONSLOT_RECOVER,0 );
							}
						}
					}
				}
			}
			break;
		case ESPType_Charactor:
			{
				UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(srcItemInfo->m_nTemplateID);
				UEquip* equip = Cast<UEquip>(itemTemp);

				ClientActionCondition_EquipItem checker;

				FClientActionConditionContex contex;
				contex.Item = srcItemInfo;
				contex.ItemTemplate = itemTemp;

				if(!checker.CheckClientActionCondition(contex))
				{
					return TRUE;
				}

				//target item check
				FClientItemInfo* destItemInfo = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
				UItem* itemDest = NULL;

				if(destItemInfo)
				{
					itemDest = MMOFacade::getSingleton().FindItemTemplate(destItemInfo->m_nTemplateID);

					ClientActionCondition_UnEquipItem checker;

					FClientActionConditionContex contex;
					contex.Item = destItemInfo;
					contex.ItemTemplate = itemTemp;

					if(!checker.CheckClientActionCondition(contex))
					{
						return TRUE;
					}
				}

				if( CheckWeight(itemTemp,itemDest) == FALSE)
					return FALSE;

				if(itemTemp && equip && srcItemInfo->m_byBindType != E_BINDTYPE_BINDED
					&& equip->ItemBindType == E_BINDTYPE_BOE) // if not bound
				{
					AskEquipBind(srcItemInfo);
				}
				else
				{
					CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
				}
				return TRUE;
			}
			break;
		case ESPType_SkillBar:
			{
				UBOOL dragDropResult = FALSE;
				//a bit of a hack here, because without clicking the skillbar, the current player's focus will not go to the skillbar's player
				UMMOUISkillBar* skillbar = Cast<UMMOUISkillBar>( MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar ) );
				if(skillbar)
				{
					dragDropResult = skillbar->OnDragDrop( SrcSlotMC , DestSlotMC , SrcSlotType , SrcContainerID , SrcSlotID , DestSlotType , DestContainerID , DestSlotID);

					// Tutorial
					MMOGameEvent_Guide evtGuide(EGUET_DragItemToSkillBar);
					MMOUI::PostGameEvent(&evtGuide);
				}

				return dragDropResult;
			}
			break;
		case ESPType_ExpContainer:
			{
				UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( srcItemInfo->m_nTemplateID );
				if( pItem == NULL ){
					return FALSE;
				}

				if( pItem->ItemSubType != TEXT("ITEMSUBTYPE_DEPOSIT_EXP") ){
					FString errStrKey = TEXT("MMOGame.ExpContainer.Error_ToLoadIsNotExpContainer");
					MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrKey ), MsgInfoType_Table, errStrKey );

					return FALSE;
				}

				CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_DEPOSIT, DEPOSIT_EXP_SLOT, E_CONSLOT_DEFAULT, SrcSlotID );

				eventCallAsShowExpContainer( TRUE );
				return TRUE;
			}
			break;
		}
	}
	else if( DestSlotType == ESPType_Bag )
	{
		switch( SrcSlotType )
		{
		case ESPType_HeavenSoul:
			{
				FClientItemInfo* destItemInfo = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
				if(destItemInfo == NULL)
				{
					AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
					//soul bottle
					if(SrcContainerID == E_CONSLOT_DEPOSIT&&DestContainerID == E_CONSLOT_DEFAULT)
					{
						UItem* tItem = Player->ItemManager->GetItemBySlot(E_CONSLOT_DEPOSIT,0);
						if(tItem)
						{
							UItem* itemTemplate = MMOFacade::getSingleton().FindItemTemplate(tItem->ItemId);
							if( itemTemplate&&itemTemplate->ItemType == TEXT("ITEMTYPE_RECOVER") )
							{
								CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_DEFAULT,DestSlotID,E_CONSLOT_DEPOSIT,0 );
							}
						}
					}
				}	
			}
			break;
		case ESPType_Recover:
			{
				FClientItemInfo* destItemInfo = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
				if(destItemInfo == NULL)
				{
					//mana blood stone
					if(SrcContainerID == E_CONSLOT_RECOVER && DestContainerID == E_CONSLOT_DEFAULT)
					{
						CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_DEFAULT,DestSlotID,E_CONSLOT_RECOVER,0 );
					}
				}
			}
			break;
		case ESPType_ExpContainer:
			{
				UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( srcItemInfo->m_nTemplateID );
				if( pItem == NULL ){
					return FALSE;
				}
				FClientItemInfo* destItemInfo = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
				if(destItemInfo != NULL){
					//FString errStrKey = TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full");
					//MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrKey ), MsgInfoType_Table, errStrKey );

					return FALSE;
				}

				if( pItem->ItemType == TEXT("ITEMTYPE_DEPOSIT") ){
					CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_DEFAULT, DestSlotID, E_CONSLOT_DEPOSIT, SrcSlotID );
					return TRUE;
				}

				return FALSE;
			}
			break;
		}
	}

	return FALSE;
}

BOOL UMMOUIBag::CheckWeight(UItem* item, UItem* itemDest)
{
	BOOL v = TRUE;
	INT nWeight = 0;

	if (itemDest)
		nWeight = itemDest->Weight;

	//first check whether weight is sufficient and whether it can be worn
	if(item->ItemType == TEXT("ITEMTYPE_EQUIP") && item->ItemSubType != TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
	{
		if (MMOFacade::getSingleton().CheckWeight(item->Weight,nWeight,WT_Armor) == FALSE)
			v =  FALSE;
	}
	else if(item->ItemType == TEXT("ITEMTYPE_EQUIP") && item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
	{
		if (MMOFacade::getSingleton().CheckWeight(item->Weight,nWeight,WT_Weap) == FALSE)
			v = FALSE;
	}
	return v;
}

UBOOL UMMOUIBag::OnDoubleClickSlot(UGFxObject* SrcSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,UBOOL bDefault)
{
	// #16707	double-click to use item
	if(SrcSlotType == ESPType_Bag && CanUseDoubleClickItem())
	{
		OnRightMouseClick(SrcSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, bDefault);
	}
	return FALSE;
}


BOOL UMMOUIBag::CanUseDoubleClickItem()
{
	BOOL v = TRUE;
	if (MMOUI::GetUIManager()->Panel_Warehouse.IsValid())
	{
		v = FALSE;
	}
	return v;
}


void UMMOUIBag::DelegateDeleteItem(BYTE ContainerID,INT SlotID)
{
	INT actorLevel = MMOFacade::getSingleton().GetMyLevel();
	if( actorLevel < s_ActorLvAllowGameSystem ){
		FString strString = TEXT("MMOErrorInfo.ERROR_CODE_DEF.actorLevelTooLow");
		FString errorStr = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( strString ), s_ActorLvAllowGameSystem );
		MMOUI::GetUIManager()->delegateAddScreenMessage( errorStr, MsgInfoType_Table, strString );
		return;
	}

	UItem* Item = FindItemTemplate(ContainerID, SlotID);
	if( Item )
	{
		UGFxObject* MsgBox = NULL;
		FString InputText = FString::Printf( TEXT("%s%s"),
			*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PleaseInput")),
			*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.DestroyAffirmText") ) );

		FScriptDelegate Fun(EC_EventParm);
		Fun.Object = this;

		FScriptDelegate FunNone(EC_EventParm);

		if( Item->ItemColor >= EIC_Color3 )
		{
			Fun.FunctionName = TEXT("OnInputThrowItemOk");				

			MsgBox = MMOMsgBoxManager::getSingleton().AddInputMessageBox(
				TEXT("") , ContainerID,
				FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ThrowItem") ), *UITextParser::HTMLFormatItemColorString(Item) )  ,
				Fun, FunNone, eInputFormatAll );
			MsgBox->GetObject( "textInput" )->SetString(TEXT("defaultText"),InputText);

			LostFocus();
		
		}
		else
		{
			Fun.FunctionName = TEXT("OnThrowItemOk");
			MsgBox = MMOMsgBoxManager::getSingleton().AddModalMessageBox(
				FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ThrowItem") ), *UITextParser::HTMLFormatItemColorString(Item) ),
				ContainerID, Fun, FunNone );				
		}

		if( MsgBox != NULL )
		{
			MsgBox->SetInt( TEXT("iteminfoid") , ContainerID );
			MsgBox->SetInt( TEXT("itemdataid") , SlotID );
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("[UMMOUIBag::DelegateDeleteItem] item template not found:[%d][%d]"), ContainerID, SlotID);
	}
}

//------------------------------------------------------------------------
void UMMOUIBag::OnThrowItemOk(class UGFxObject* mc)
{
	if( mc )
	{
		INT conId = mc->GetInt( TEXT("iteminfoid") );
		INT slotId = mc->GetInt( TEXT("itemdataid") );
		ItemInfo* item = MMOFacade::getSingleton().FindItem( conId , slotId );

		if(item)
		{
			DeleteItem( conId , slotId , item->m_nTemplateID );
		}

		
	}
}

//------------------------------------------------------------------------
void UMMOUIBag::OnInputThrowItemOk(class UGFxObject* mc)
{
	
	if( mc )
	{
		FString InputText = mc->GetObject( "textInput" )->GetString("text");
		if( InputText ==  MMOUI::ParseLocalizedPropertyPath( "MMOGame.GamePlay.DestroyAffirmText" ) )
		{
			INT conId = mc->GetInt( TEXT("iteminfoid") );
			INT slotId = mc->GetInt( TEXT("itemdataid") );
			ItemInfo* item = MMOFacade::getSingleton().FindItem( conId , slotId );

			if(item)
			{
				DeleteItem( conId , slotId , item->m_nTemplateID );
			}
		}
		else
		{
			FScriptDelegate funcOK(EC_EventParm);
			MMOMsgBoxManager::getSingleton().AddMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.ErrorDestroyAffirmText")),0,funcOK,funcOK,FALSE);
		}
	}
}


//------------------------------------------------------------------------
UItem* UMMOUIBag::GetItemInCurBag(INT Index )
{
	//TArrayNoInit<class UItem*>* ItemList;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	UItemManager* IM = NULL;
	if( pl)
		IM = pl->ItemManager;
	if( !IM )
		return NULL;

	switch( GetBagEnum() )
	{
	case E_CONSLOT_DEFAULT://item
		if( IM->BagItemList.Num() > Index)
			return IM->BagItemList(Index);
		break;
	//case E_CONSLOT_QUEST://
	//	if( IM->BagQuestList.Num() > Index)
	//		return IM->BagQuestList(Index);
	//	break;
	default:
		return NULL;
	}

		return NULL;
	
	

}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void UMMOUIBag::TurntoPage(BYTE BagType)
{

	INT Page = 0;
	switch(BagType)
	{
	case E_CONSLOT_DEFAULT:
		Page = EBLT_NormalItem;
		break;
	/*case E_CONSLOT_QUEST:
		Page = EBLT_Quest;
		break;*/
	}

	if( Page == CurLabel )
	{
		return;
	}

	UGFxObject* Mc = GetObject( FString::Printf(TEXT("label%d_mc"),Page) );
	if( GFXPlayer && Mc )
	{
		GFXPlayer->eventCallWidgetDispatchEvent( Mc , TEXT("click") );
	}

}
//------------------------------------------------------------------------
BYTE UMMOUIBag::GetBagEnum()
{
	switch(CurLabel)
	{
	case EBLT_NormalItem:
		return E_CONSLOT_DEFAULT;
	/*case EBLT_Quest:
		return E_CONSLOT_QUEST;*/
	}
	//filter pages are all DEFAULT
	return E_CONSLOT_DEFAULT;
}
//------------------------------------------------------------------------


void UMMOUIBag::BagFilter()
{

}

void UMMOUIBag::UnInit()
{
	FScriptDelegate nullDelgate(EC_EventParm);
	
	MMOUI::GetUIManager()->SetDeleteItemDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetGetOpenBagDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetTurntoPageDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetLockItemDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetUpdateBagCapSlotDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetHintNewItemLabelDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetOpenBagDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetOnShiftDelegate(nullDelgate);

	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}
//------------------------------------------------------------------------

void UMMOUIBag::BeforeViewUpdate(float DeltaTime)
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

void UMMOUIBag::BagPageRefresh()
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
	INT slotCountNext =  Min(slotCountCur+16,UCONST_MaxBagSlotCount);


	INT currentContainer = GetBagEnum();
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

		INT realSlot = i + UCONST_MaxBagSlotCount * pageId;

		SetBagSlotCD(currentContainer,realSlot);

		if( realSlot >= CurCapacity )
		{			
			GFXPlayer->SetItemSlotData( itemSlot , NULL, ESPType_Bag , currentContainer, realSlot, ISS_SlotLocked );

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

		FClientItemInfo* item = mmoFacade.FindItem( currentContainer, realSlot );
		//
		HintItem( currentContainer, realSlot, item );

		//
		if(item == NULL )
		{
			GFXPlayer->SetItemSlotData( itemSlot , NULL, ESPType_Bag, currentContainer, realSlot );

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

		UItem* itemTemp = mmoFacade.FindItemTemplate(item->m_nTemplateID);
		if( itemTemp )
		{
			GFXPlayer->SetItemSlotData(itemSlot, itemTemp, ESPType_Bag, currentContainer, realSlot, ISS_Normal);

			if (bOpenByNpcShop == FALSE)
			{
				UBOOL isToGray = GetItemFilterFrame( itemTemp , labelId , CurTab );
				mmoFacade.GrayItem( ESGMark_Bag, currentContainer, realSlot, isToGray );
			}
			
		}
	}



	if(slotCountNext>=UCONST_MaxBagSlotCount)
	{
		slotCountCur = -1;
	}
	else
	{
		slotCountCur+=16;
	}
}

void UMMOUIBag::BagPage(INT pageId)
{
	pageIdCur=pageId;
	slotCountCur=0;
	slotCountDown=-1;//things in bag slots start refreshing one frame later
}

UBOOL UMMOUIBag::GetItemFilterFrame( UItem* item , INT label , INT filterTab )
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

//------------------------------------------------------------------------
void UMMOUIBag::BagLabel(INT labelId)
{	
	//click tab
	LastLabel = CurLabel;

	ClearNewHint();

	CurLabel = labelId;
	
	INT pageId = LabelBagPage(labelId);
	
	//page turning removed, currently defaults to the first page
	//FString mcName = TEXT("bagTab");
	//mcName += appItoa(pageId);
	//UGFxObject* mc = GetObject( mcName );
	//GFXPlayer->eventCallWidgetDispatchEvent( mc , TEXT("click") );

	BagPage(0);
	
}

void UMMOUIBag::BagTab(INT tabId)
{
	CurTab = tabId;

	if (bOpenByNpcShop == FALSE)
	{
		BagPage(0);
	}

	MMOGameEvent_Bag_TabChange evt(CurTab);
	MMOUI::PostGameEvent(&evt);
	
}

void UMMOUIBag::ClearNewHint()
{
	//if(LabelButton[CurLabel])
	//{
	//	UGFxObject* labelNewHint = LabelButton[CurLabel]->GetObject("newHint");
	//	if(labelNewHint)
	//		labelNewHint->SetVisible(false);
	//}
	//for ( INT i = 0 ; i < UCONST_MaxBagSlotCount ; i++ )
	//{
	//	INT containerId = GetBagEnum();
	//	FClientItemInfo* item = MMOFacade::getSingleton().FindItem( containerId , i );
	//	if(item)
	//	{
	//		item->IsNew = FALSE;
	//	}
	//}
}

void UMMOUIBag::HintNewItemLabel(INT containerId)
{
	INT tabIdx = 0;
	switch( containerId )
	{
	case CONSLOT_DEFAULT: tabIdx = 1;
		break;
	//case CONSLOT_QUEST: tabIdx = 3;
	//	break;
	default: 
		return;
	}
	//no longer need tab new
	//if(LabelButton[tabIdx])
	//{
	//	UGFxObject* labelNewHint = LabelButton[tabIdx]->GetObject("newHint");
	//	if(labelNewHint)
	//		labelNewHint->SetVisible(true);
	//}
}

//-------------------------------------------------------------------------
void UMMOUIBag::PlayerAttributeChange(INT AtrrId,INT inValue)
{
	if( !Player  || !Player->ActorAttributes )
		return;
	if( !GFXPlayer || AtrrId == 0 )
		return;

	if(AtrrId == catBagWeightLmt )
	{
		SetBagWeight();
	}


}

void UMMOUIBag::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
	if(MoneyType==E_MONEYTYPE_01 || MoneyType==E_MONEYTYPE_02)
	{
		UpdateMoneyDisplay();
	}
	//// TODO: deal with each money type
	//UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	//
	//data->SetString( TEXT("moneyString0") , UITextParser::HTMLMoneyIconString( E_MONEYTYPE_02 , MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_02) ) );
	//data->SetString( TEXT("moneyString1") , UITextParser::HTMLMoneyIconString( E_MONEYTYPE_01 , MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_01) ) );

	//eventCallSetMoney(data);
}
//-------------------------------------------------------------------------
void UMMOUIBag::UpdateMoneyDisplay()
{	
	if(IsVisible==FALSE)
	{
		return;
	}

	UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	if(data){
		data->SetPureString( TEXT("moneyString0") , UITextParser::HTMLMoneyIconString( E_MONEYTYPE_02 , MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02) ) );
		
		//changed to bag weight
		data->SetInt(TEXT("moneyString1") ,MallManager::getSingleton().GetRealMoney());

		eventCallSetMoney(data);
		SetBagWeight();
	}
}

void UMMOUIBag::SetBagWeight()
{

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( player && player->ActorAttributes )
	{
		INT bagW =  player->ActorAttributes->GetAttribute(catBagWeightLmt);
		INT weight = MMOFacade::getSingleton().GetCurrentBagWgt();
		eventSetBagWeight(weight,bagW);
	}

	
}
//-------------------------------------------------------------------------
void UMMOUIBag::ItemChange(INT ContainerID,INT SlotID)
{
	if( ContainerID == GetBagEnum() )
	{
		//eventBagSlotChangeEx(ContainerID, SlotID);
	}
}

void UMMOUIBag::BagSlotChangeEx(BYTE ContainerID,INT SlotID)
{
	if( GetBagEnum() != ContainerID)		
	{
		return;
	}

	UItem* item = FindItemTemplate(ContainerID, SlotID);
	FClientItemInfo *clientItemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);

	INT pageSlotId = SlotID - LabelBagPage(CurLabel) * UCONST_MaxBagSlotCount;

	if( pageSlotId > ItemSlotList.Num() )
	{
		return;
	}
		
	UGFxObject* itemSlot = ItemSlotList(pageSlotId);

	if(itemSlot)
	{
		//eventCallUCGraySlot( itemSlot , GetItemFilterFrame( item , CurLabel , CurTab ) );
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

	SetBagSlotCD( ContainerID , SlotID );
	HintItem( ContainerID , SlotID , clientItemInfo );

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{
				
		GFXPlayer->SetItemSlotData( itemSlot, item, ESPType_Bag, ContainerID, SlotID );
	}
	
}
//-------------------------------------------------------------------------
void UMMOUIBag::BagSlotChange(INT ContainerID, INT SlotID)
{
	if( ContainerID == GetBagEnum() )
	{
		if(IsVisible)
		{
			BagSlotChangeEx(ContainerID, SlotID);
		}
	}

	//experience bottle
	if( ContainerID == E_CONSLOT_DEPOSIT && IsVisible )
	{
		ExpContainerChanged(ContainerID, SlotID);
	}

	if( ConfirmMsgBox != NULL ){
		HideBag_Msg();
	}
}
//------------------------------------------------------------------------
void UMMOUIBag::ItemDelete(INT ContainerID, INT SlotID)
{
	if( ContainerID == GetBagEnum() )
	{
		if(IsVisible)
		{
			ClearItemSlot(ContainerID, SlotID);
		}
	}
}
//------------------------------------------------------------------------
//do some data updates when displaying
void UMMOUIBag::NativeOnShow(UBOOL v)
{
	if(v)
	{
		BagPage( LabelBagPage(CurLabel) );
		UpdateMoneyDisplay();
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_bag_on"));
		FScriptDelegate fun( this , TEXT("OnShift") );
		MMOUI::GetUIManager()->SetOnShiftDelegate(fun);

		//
		ExpContainerChanged( CurLoadedExpContainer._ConID, CurLoadedExpContainer._SlotID );
	}
	else
	{
		FScriptDelegate nullDelgate(EC_EventParm);
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_bag_off"));
		MMOUI::GetUIManager()->SetOnShiftDelegate(nullDelgate);
	}
}
//------------------------------------------------------------------------

void UMMOUIBag::SkillBookMsgBox(INT ItemSlot)
{
	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer *MainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(  !pc || !MainPlayer )
	{
		debugf(NAME_Error, TEXT("[SkillBookMsgBox] can't find pc"));
		return;
	}
	UGFxObject* MsgBox;
	FString sMsg ;
	UItem* tSkillBook = GetItemInCurBag( ItemSlot );
	if( !tSkillBook )
		return;

	UMMOAbility* tAbilityLearned = NULL;
	UMMOAbility* tAbilityInBook = tSkillBook->GetToUseAbility();
	FString learnSkillName;
	if( tAbilityInBook != NULL)
	{
		learnSkillName = tAbilityInBook->eventGetAbilityName();
	}
	else
	{
		learnSkillName = tSkillBook->GetDisplayName();
	}

	if( !tSkillBook->IsUseblForActor( MainPlayer ) )
	{
		//cannot learn this ability
		sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.SkillCannotLearne" ) ),
			*learnSkillName,*tSkillBook->Description );
		MMOChatManager::getSingleton().AddSystemMessage( sMsg );
		MMOUI::GetUIManager()->delegateAddScreenMessage(sMsg,MsgInfoType_Table,TEXT("MMOGame.GamePlay.SkillCannotLearne" ));
		return;
	}

	if( tAbilityInBook )
		tAbilityLearned = MainPlayer->GetAbilityById( tAbilityInBook->eventGetAbilityId(), tAbilityInBook->GetRuneSlot() );
	if( tAbilityLearned )
	{
		//already learned this ability
		/*sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.SkillBookAlreadyLearned" ) ),*learnSkillName );
		MMOChatManager::getSingleton().AddSystemMessage( sMsg );
		MMOUI::GetUIManager()->delegateAddScreenMessage(sMsg,MsgInfoType_Table, TEXT("MMOGame.GamePlay.SkillBookAlreadyLearned" ));*/
		if( !HasAskToLearnSkillBook )
		{
			FScriptDelegate fsdSkillBookOk(EC_EventParm);
			fsdSkillBookOk.FunctionName = TEXT("SkillBookMsgBoxOk");
			fsdSkillBookOk.Object = this;

			FScriptDelegate fsdCancel(EC_EventParm);
			fsdCancel.Object = this;
			fsdCancel.FunctionName = TEXT("SkillBookMsgBoxCancel");

			sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.SkillBookMsg1" )),*learnSkillName );
			MsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( sMsg, ItemSlot , fsdSkillBookOk ,fsdCancel );

			HasAskToLearnSkillBook = true;
		}

	}
	else if( !HasAskToLearnSkillBook )
	{
		FScriptDelegate fsdSkillBookOk(EC_EventParm);
		fsdSkillBookOk.FunctionName = TEXT("SkillBookMsgBoxOk");
		fsdSkillBookOk.Object = this;

		FScriptDelegate fsdCancel(EC_EventParm);
		fsdCancel.Object = this;
		fsdCancel.FunctionName = TEXT("SkillBookMsgBoxCancel");

		sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.SkillBookMsg" )),*learnSkillName );
		MsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( sMsg, ItemSlot , fsdSkillBookOk ,fsdCancel );

		HasAskToLearnSkillBook = true;
	}
}

void UMMOUIBag::SkillBookMsgBoxOk(UGFxObject* mc)
{
	HasAskToLearnSkillBook = false;
	if( mc != NULL )
	{
		INT ItemSlot = mc->GetInt("iteminfoid");
		UItem* SelItem = GetItemInCurBag( ItemSlot );
		if( SelItem )
		{
			CMessageHandler::GetInstance().Send_UseItem( E_CONSLOT_DEFAULT,ItemSlot, SelItem->ItemId );
		}
	}
}

void UMMOUIBag::SkillBookMsgBoxCancel(UGFxObject* mc)
{
	HasAskToLearnSkillBook = false;
}

void UMMOUIBag::DeleteItem( INT ConId , INT SlotId , INT TemplateId )
{
	UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate( TemplateId );
	FClientItemInfo* item = MMOFacade::getSingleton().FindItem( ConId , SlotId );
	if( item && itemTemp )
	{
		INT stack = 0;
		/*if( itemTemp->ItemDuration == E_DURTYPE_STACK ) 
		{
			//type is stackable
			stack = item->m_nCurDur;
		}
		else
		{
			stack = 1;
		}*/
		stack = item->GetCurStackCount(itemTemp);
		CMessageHandler::GetInstance().Send_DeleteItem( ConId , SlotId , stack , TemplateId );
	}
	
}


void UMMOUIBag::OnEquipItemOK(class UGFxObject* mc)
{
	INT srcCon = mc->GetInt(TEXT("iteminfoid"));
	INT srcSlot = mc->GetInt(TEXT("itemdataid"));
	ItemInfo* item = MMOFacade::getSingleton().FindItem(srcCon, srcSlot);
	if(item)
	{
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(item->m_nTemplateID);
		UEquip* equip = Cast<UEquip>(itemTemp);

		if(itemTemp && equip && item->m_byBindType != E_BINDTYPE_BINDED
			&& equip->ItemBindType != E_BINDTYPE_NONE
			&& equip->GetEquipAppearenceTemplate())
		{
			INT destCon = 0, destSlot = 0;
			MMOUI::GetEquippedSlotByEquipType(equip->GetEquipAppearenceTemplate()->EquipType, destCon, destSlot);
			CMessageHandler::GetInstance().Send_MoveItem(destCon, destSlot, srcCon, srcSlot);
		}
	}
}
//==================================================================================================================
//lock items on current page
void UMMOUIBag::LockItem( BYTE ContainerID, INT SlotID, INT isToLockIn )
{
	UGFxObject *pItemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc"), SlotID ) );
	//FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( ContainerID, SlotID );

	if( pItemSlot == NULL ){
		return;
	}

	//pItemInfo->IsDragIntoOtherUI = isToLockIn;
	pItemSlot->SetBool( TEXT("MarkLocked"), isToLockIn >= 0 );
	////
	//TMap< INT, EItemSlotState > *pContainer = NULL
	//switch( ContainerID )
	//{
	//case CONSLOT_DEFAULT:
	//	{
	//		pContainer = &BagSlotState;
	//	}
	//	break;
	//case CONSLOT_QUEST:
	//	{
	//		pContainer = &QuestBagSlotState;
	//	}
	//	break;
	//default: 
	//	return;
	//}
	////lock items on current page by default
	//INT realSlot = SlotID + UCONST_MaxBagSlotCount * CurPageIdx;
	//if( isToLock ){
	//	pContainer->FindOrAdd( realSlot ) = ISS_Unusable;
	//}
	//else if( pContainer->Find( realSlot ) ){
	//	pContainer->Remove( realSlot );
	//}

}

//==========================================================================================================
//items to gray out
void UMMOUIBag::GrayItem( BYTE ContainerID, INT SlotID, struct FClientItemInfo *pItemInfo )
{
	UGFxObject *pItemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc"), SlotID ) );

	if( pItemSlot == NULL ){
		return;
	}

	//FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( ContainerID, SlotID );
	if( pItemInfo == NULL ){
		//eventCallUCGraySlot( pItemSlot, FALSE );
		return;
	}

	//
	UBOOL isToGray = FALSE;
	if( CurTab == EBTT_AllItem ){
		isToGray= ( pItemInfo->IsGrayMarkedInUI != 0 );
		//UBOOL isToGray = ( pItemInfo->IsGrayMarkedInUI != 0 );
		//eventCallUCGraySlot( pItemSlot, isToGray );
	}
	else{
		isToGray = ( ( pItemInfo->IsGrayMarkedInUI & ESGMark_Bag ) != 0 );
		////if the current bag filter setting is not "all items", follow only the bag's filter
		//UBOOL isGrayByBag = ( ( pItemInfo->IsGrayMarkedInUI & ESGMark_Bag ) != 0 );
		//eventCallUCGraySlot( pItemSlot, isGrayByBag );
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

//==========================================================================================================
void UMMOUIBag::HintItem( BYTE ContainerID, INT SlotID, struct FClientItemInfo *pItemInfo )
{
	if( !pItemInfo )
	{
		if(HintSlotLast[SlotID]!=0)
		{
			HintSlotLast[SlotID]=0;
			eventCallUCHintSlot( SlotID, FALSE , FALSE );
		}
		return;
	}
	if( pItemInfo->NeedHintMode == ItemSlotHint_Normal )
	{
		if(HintSlotLast[SlotID]!=0)
		{
			HintSlotLast[SlotID]=0;
			eventCallUCHintSlot( SlotID, FALSE , FALSE );
		}
	}
	else if( ( pItemInfo->NeedHintMode | ItemSlotHint_PowerEquip ) == pItemInfo->NeedHintMode )
	{
		if(HintSlotLast[SlotID]!=2)
		{
			HintSlotLast[SlotID]=2;
			eventCallUCHintSlot( SlotID, TRUE , FALSE );
		}
	}
	else
	{
		if(HintSlotLast[SlotID]!=3)
		{
			HintSlotLast[SlotID]=3;
			eventCallUCHintSlot( SlotID, TRUE , TRUE );
		}
	}
	
}

//==========================================================================================================
void UMMOUIBag::OnStallStateChange(BYTE state)
{
	UGFxObject* btn = GetObject(TEXT("btnFuc3_mc"));
	if(btn)
	{
		btn->SetBool(TEXT("enabled"), state == essNotOpen );
	}
}

void UMMOUIBag::OnFocusGained()
{
	//FScriptDelegate fun( this , TEXT("OnShift") );
	//MMOUI::GetUIManager()->SetOnShiftDelegate(fun);
	UMMOUIBase::OnFocusGained();
}
void UMMOUIBag::OnFocusLost()
{
	//FScriptDelegate nullDelgate(EC_EventParm);
	//MMOUI::GetUIManager()->SetOnShiftDelegate(nullDelgate);
	UMMOUIBase::OnFocusLost();
}

void UMMOUIBag::OnShift(UBOOL bRelease)
{
	if(bRelease==FALSE)
	{
		//debugf(NAME_Error, TEXT("UMMOUIBag::OnShift = FALSE------------------------------------"));
		eventClickSplit();
	}
	else{
		//debugf(NAME_Error, TEXT("UMMOUIBag::OnShift = TRUE------------------------------------"));
		//MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
		//MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(this);
	}
} 

void UMMOUIBag::NativeOnSubscribeItemSlotAction()
{
	g_lockMouseCursorForUI = false;
	MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
}

void UMMOUIBag::NativeOnUnSubscribeItemSlotAction()
{
	g_lockMouseCursorForUI = false;
	MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
}

#if 1 && !SHIPPING_PC_GAME
#	define dbgSort(msg) SET_WARN_COLOR(COLOR_PURPLE); warnf(NAME_Log, msg); CLEAR_WARN_COLOR();
#else
#	define dbgSort(...)
#endif

void UMMOUIBag::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_ClearBag:
		{
			if (MMOFacade::getSingleton().GetPlayerSortType() == IST_Bag)
			{
				ClearUIEx(this, E_CONSLOT_DEFAULT, GFXPlayer);
			}
			
			//ClearUIEx(this, E_CONSLOT_EQUIPMENT, GFXPlayer);
			//ClearUIEx(this, E_CONSLOT_MATERIAL, GFXPlayer);
			//ClearUIEx(this, E_CONSLOT_QUEST, GFXPlayer);
			//BagPage( LabelBagPage(CurLabel) );
		}
		break;
	case GameEvent_OpenChainPay:
		{
			MMOGameEvent_MsgboxCallBack* msg = (MMOGameEvent_MsgboxCallBack*)evt;
			if(msg)
			{
				if( msg->Agree && msg->Box )
				{
					CMessageHandler::GetInstance().Send_ChainGameMoney( TRUE );
					MMOFacade::getSingleton().SetChainPay(TRUE);

					SetChainPay();
				}
			}
		}
		break;
	case GameEvent_SortTimer:
		{
			MMOGameEvent_SortTimer* e = (MMOGameEvent_SortTimer*)(evt);
			eventCallUpdateSortTimer(e->TimeSecondsLeft);
			dbgSort(*FString::Printf(TEXT("OnSortTimer:%d"), e->TimeSecondsLeft));
		}
		break;

	case GameEvent_SortTimerBegin:
		{			
			eventCallEnableSort(FALSE, MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.SortBtn.Label")));
			dbgSort(TEXT("GameEvent_SortTimerBegin"));
		}
		break;

	case GameEvent_SortTimerEnd:
		{			
			eventCallEnableSort(TRUE, MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.SortBtn.Label")));
			MMOFacade::getSingleton().SetPlayerSortType(IST_Invalid);
			dbgSort(TEXT("GameEvent_SortTimerEnd"));
		}
		break;
	case GameEvent_ChainPayChange:
		{			
			SetChainPay( );
		}
		break;
	case GameEvent_MsgBoxItemIdentifyAsk:
		{
			MMOGameEvent_MsgboxCallBack* msg = (MMOGameEvent_MsgboxCallBack*)evt;
			if(msg)
			{
				if( msg->Agree && msg->Box )
				{
					INT slotID = msg->Box->GetInt( TEXT("itemdataid") );
					INT containerID = msg->Box->GetInt( TEXT("iteminfoid") );

					CMessageHandler::GetInstance().Send_IdentifyItem( containerID, slotID );

					MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Appraisal);
				}
				
			}
		}
		break;
	case GameEvent_NewCD:
		{
			MMOGameEvent_NewCD* msg = (MMOGameEvent_NewCD*)evt;
			if( msg )
			{
				UpdateAllBagSlotCDByItemUse( msg->CDType , msg->CDId );
			}
		}
		break;
	case GameEvent_BagSlotHintChange:
		{
			MMOGameEvent_BagSlotHintChange* msg = (MMOGameEvent_BagSlotHintChange*)evt;
			if(msg && msg->ConID == E_CONSLOT_DEFAULT)
			{
				HintItem( msg->ConID , msg->SlotID , msg->ItemInfo );
			}
			
		}
		break;
	case GameEvent_BagOpenPack:
		{
			MMOGameEvent_BagOpenPack* msg = (MMOGameEvent_BagOpenPack*)evt;
			if (msg)
			{
				OpenPack(msg->itemId);
			}
		}
		break;
	case GameEvent_BagWeight:
		{
			SetBagWeight();
		}
		break;
	case GameEvent_BagSortEnd:
		{
			if (MMOFacade::getSingleton().GetPlayerSortType() == IST_Bag)
			{
				BagPage( LabelBagPage(CurLabel) );
				MMOFacade::getSingleton().SetPlayerSortType(IST_Invalid);
			}
			
		}
		break;
	case GameEvent_ExpContainerChanged:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;

			NotifyExpContainerFillIn( msg->IntData );

		}
		break;
	case GameEvent_RealMoneyChange:
		{
			if (this->GetVisible())
			{
				UpdateMoneyDisplay();
			}
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
	case GameEvent_BagResourceHide:
		{
			ChangeResBagBtn();
		}
		break;
	}
}

void UMMOUIBag::OpenPack(INT itemId)
{
	INT currentContainer = GetBagEnum();	
	INT CurCapacity = MMOFacade::getSingleton().GetMainPlayer()->ItemManager->CurCapacity(currentContainer);

	for( INT i = 0 ; i < CurCapacity ; i++ )
	{

		UItem* item = MMOFacade::getSingleton().FindItemTemplate( currentContainer, i );
		if(!item)
		{
			//empty slot
			continue;
		}
		if (item->ItemId == itemId)
		{
			if(item->Script.Len() > 0)
			{
				CMessageHandler::GetInstance().Send_UseItem(currentContainer, i, item->ItemId);
				break;
			}
		}
	}
}

void UMMOUIBag::UpdateAllBagSlotCDByItemUse( INT type , INT cdID )
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !player || !player->ItemManager || !BagGroup )
	{
		return;
	}
	INT currentContainer = GetBagEnum();	
	INT CurCapacity = player->ItemManager->CurCapacity(currentContainer);

	for( INT i = 0 ; i < CurCapacity ; i++ )
	{

		UItem* item = MMOFacade::getSingleton().FindItemTemplate( currentContainer, i );
		if(!item)
		{
			//empty slot
			continue;
		}
		UMMOAbility* skill = item->GetToUseAbility();
		FString groupCoolDown = item->GroupCoolDown;
		
		if(!skill)
		{
			//item has no ability
			continue;
		}

		if( !groupCoolDown.IsEmpty() && type == COOLDOWN_GROUP )
		{
			INT pos = groupCoolDown.InStr( TEXT("-") );
			INT groupId = appAtoi( *groupCoolDown.Left(pos) );

			//for ( INT groupId = 0 ; groupId < skill->GroupIds.Num() ; i++ )
			{
				if( groupId == cdID )
				{
					CoolDownData* cd = MMOCoolDownManager::getSingleton().GetCDData( (COOLDOWN_TYPE)type , cdID );
					UGFxObject* itemSlot = BagGroup->GetObject( FString::Printf(TEXT("slot%d_mc"),i) );

					if( cd && itemSlot )
					{
						SetItemSlotCD( itemSlot , cd->CurValue , cd->MaxValue );
					}

					
				}
			}
		}


	}
}

void UMMOUIBag::SetChainPay( )
{
	UBOOL sel = MMOFacade::getSingleton().GetChainPay();
	UGFxObject* butt = GetObject( TEXT("MoneyChainButton") );
	if(butt)
	{
		butt->SetBool( TEXT("selected") , sel );

		SetChainPayText();
	}
}

void UMMOUIBag::SetChainPayText()
{
	UBOOL sel = MMOFacade::getSingleton().GetChainPay();
	UGFxObject* butt = GetObject( TEXT("MoneyChainButton") );
	if(butt)
	{
		if( sel )
		{
			butt->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.ChainPayOnTip") ) );
		}
		else
		{
			butt->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.ChainPayOffTip") ) );
		}
	}
}

void UMMOUIBag::SetBagSlotCD( INT containerId , INT slotId )
{
	if(!BagGroup)
	{
		return;
	}

	UGFxObject* itemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc") , slotId ) );
	if(!itemSlot)
	{
		return;
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate( containerId , slotId );
	if(!item)
	{
		//empty slot
		StopItemSlotCD(itemSlot);
		return;
	}

	FString groupCoolDown = item->GroupCoolDown;	

	if( !groupCoolDown.IsEmpty() && groupCoolDown != TEXT("0") )
	{
		INT pos = groupCoolDown.InStr( TEXT("-") );
		INT groupId = appAtoi( *groupCoolDown.Left(pos) );

		CoolDownData* cd = MMOCoolDownManager::getSingleton().GetCDData( COOLDOWN_GROUP , groupId );
		if(cd)
		{
			SetItemSlotCD( itemSlot , cd->CurValue , cd->MaxValue );
		}
		else
		{
			StopItemSlotCD(itemSlot);
		}
	}
	else if( item->SelfCoolDown > 0 )
	{
		INT itemId = item->GetTemplateID();
		CoolDownData* cd = MMOCoolDownManager::getSingleton().GetCDData( COOLDOWN_ITEM , itemId );
		if(cd)
		{
			SetItemSlotCD( itemSlot , cd->CurValue , cd->MaxValue );
		}
		else
		{
			StopItemSlotCD(itemSlot);
		}
	}
	else
	{
		StopItemSlotCD(itemSlot);
	}
		

}

void UMMOUIBag::ClearItemSlot( BYTE ContainerID, int SlotID)
{
	if(!BagGroup)
		return;

	INT pageSlotId = SlotID - LabelBagPage(CurLabel) * UCONST_MaxBagSlotCount;
	UGFxObject* itemSlot = BagGroup->GetObject( FString::Printf( TEXT("slot%d_mc") , pageSlotId ) );	
	StopItemSlotCD( itemSlot );
	
	GFXPlayer->SetItemSlotData( itemSlot, NULL, ESPType_Bag, ContainerID, SlotID );

	//eventClearItemSlot(itemSlot);
}

//==============================================================================================================
void UMMOUIBag::ConfirmUseExpContainer( UGFxObject* mc )
{
	if( mc == NULL ){
		return;
	}

	INT nSlot = mc->GetInt("itemdataid");
	if( nSlot >= 0 ){
		FClientItemInfo* pItemInfo = MMOFacade::getSingleton().FindItem( CONSLOT_DEFAULT, nSlot );
		if( pItemInfo != NULL ){
			CMessageHandler::GetInstance().Send_UseItem( CONSLOT_DEFAULT, nSlot, pItemInfo->m_nTemplateID );
		}
	}

	UBOOL isNotShowAgain = mc->GetBool( TEXT("forbidHint") );
	GConfig->SetBool( TEXT("PopUpMsgBox"), TEXT("ConfirmUseExpContainer"), !isNotShowAgain, GDefaultGameIni );

	ConfirmMsgBox = NULL;
}

void UMMOUIBag::CancelUseExpContainer( UGFxObject* mc )
{
	ConfirmMsgBox = NULL;
}

//==============================================================================================================
void UMMOUIBag::LostFocus()
{
	if (FGFxEngine::GetEngine()->GetFocusMovie(0) && FGFxEngine::GetEngine()->GetFocusMovie(0)->pUMovie)
		FGFxEngine::GetEngine()->GetFocusMovie(0)->pUMovie->eventOnFocusLost(0);
}

void UMMOUIBag::ASLockItem()
{

}


void UMMOUIBag::ASSplitItem()
{
	eventClickSplit();
}

//--------------------------------------------
void UMMOUIBag::ASEvaluteItem()
{
	eventClickIdentify();
}

void UMMOUIBag::ASClickMoneyChain( UBOOL selected )
{
	CMessageHandler::GetInstance().Send_ChainGameMoney( selected );
	MMOFacade::getSingleton().SetChainPay(selected);
	SetChainPayText();
}

//--------------------------------------------
void UMMOUIBag::ASSignatureItem()
{
	eventClickDecompose();

	MMOGameEvent_Guide evtGuide(EGUET_ClickDecompEquip);
	MMOUI::PostGameEvent(&evtGuide);
}


//void UMMOUIBag::SetItemSlotData( class UGFxObject* mc, class UItem* tItem, BYTE SlotType, BYTE ContainerID , INT SlotID , BYTE inISS )
//{
//	UMMOUIBase::SetItemSlotData( mc, tItem, SlotType, ContainerID, SlotID,inISS);
//
//	//add another hit
//	if(!mc)
//	{
//		return;
//	}
//
//	UGFxObject* abnormity = mc->GetObject(TEXT("abnormity_mc"));
//	UGFxObject* newHint = mc->GetObject("newHint");
//	UGFxObject* colorFrame = mc->GetObject("colorFrame");
//
//	if(!newHint)
//		return;
//
//	if(tItem)
//	{
//		FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem( ContainerID , SlotID );
//		if(itemInfo)
//		{
//			//UBOOL showNewHint = ( ( itemInfo->NeedHintMode | ItemSlotHint_New ) == itemInfo->NeedHintMode ) && ( ( itemInfo->NeedHintMode | ItemSlotHint_PowerEquip ) != itemInfo->NeedHintMode );
//			//newHint->SetVisible( showNewHint );
//		}
//
//		if( tItem->ItemType == TEXT("ITEMTYPE_EQUIP") && itemInfo->m_nCurDur == 0 )
//		{
//			if(colorFrame)
//			{
//				colorFrame->SetVisible(TRUE);
//			}
//		}
//		else
//		{
//			if(colorFrame)
//			{
//				colorFrame->SetVisible(FALSE);
//			}
//		}
//
//
//		//changed here to mark weapons in the bag that do not suit this class with x, since this icon is the same as the locked item icon in trade/stall, causing ambiguity. But for now only this icon exists.
//		if( tItem->ItemType == TEXT("ITEMTYPE_EQUIP") && !mc->GetBool( TEXT("isSlotUnusable") ) )
//		{
//			const UEquip *pEquip = ConstCast<UEquip>( tItem );
//			AMMOPlayer* pl=  MMOFacade::getSingleton().GetMainPlayer();
//			MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
//
//			if(abnormity)
//			{
//				if( itemInfo->m_byIdentified == IDENTIFY_TYPE_NO )
//				{
//					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotUnidentify") );
//				}
//
//				UBOOL equipError = mmoEquipMgr.CheckItemUseableForMe( tItem );
//				switch( equipError )
//				{
//				case ITEM_Use_Err_None:
//					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
//					break;
//				
//				case ITEM_Use_Err_LowLevel:
//				case ITEM_Use_Err_LowPhyAtk:
//				case ITEM_Use_Err_LowMagAtk:
//				case ITEM_Use_Err_LowTaoAtk:
//					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotLock") );
//					break;
//
//				case ITEM_Use_Err_Undefine:
//				case ITEM_Use_Err_RequireClass:
//				case ITEM_Use_Err_Gender:
//				default:
//					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotImpossible") );
//					break;
//				}
//			}
//
//		}
//		else
//		{
//			if(abnormity)
//				abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
//		}
//	}
//	else
//	{
//		if(abnormity)
//		{
//			abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
//		}
//		//newHint->SetVisible(FALSE);
//		if(colorFrame)
//		{
//			colorFrame->SetVisible(FALSE);
//		}
//	}
//}

void UMMOUIBag::ASSortBag()
{
#if 1
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

	CMessageHandler::GetInstance().Send_SortBag(GetBagEnum());
	MMOFacade::getSingleton().StartSortTimer( LSM_SortBag );
	MMOFacade::getSingleton().SetPlayerSortType(IST_Bag);

#else
	//FClientActionConditionContex contex(ECACCT_UseItem_CheckStallState);
	//if(!CheckClientActionCondition(contex))
	//{
	//	return;
	//}

	ClientActionCondition_ItemForbiddenInPanelOpen checker;

	FClientActionConditionContex contex;
	contex.Item = NULL;
	contex.ItemTemplate = NULL;

	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}


	ECONTAINER_SLOT curCont = (ECONTAINER_SLOT)GetBagEnum();

	ItemsMap& container = MMOFacade::getSingleton().GetContainer(curCont);

	typedef std::vector<ItemInfo*, M_UE3_STL_ALLOC(ItemInfo*) > ItemsList;

	ItemsList sortArr;
	for (ItemsMap::iterator i = container.begin(); i != container.end(); ++i)
	{
		ItemInfo* newItem = new ItemInfo();
		appMemcpy(newItem, i->second, sizeof(ItemInfo));
		sortArr.push_back(newItem);
	}

	std::sort(sortArr.begin(), sortArr.end(), SortItemByTemplateID);

	for (size_t j = 0; j < sortArr.size(); ++j)
	{
		ItemInfo* info = sortArr[j];

		if(j != info->m_wItemSlot)
		{
			CMessageHandler::GetInstance().Send_MoveItem(curCont, j, curCont, info->m_wItemSlot);
		}

		delete info;
	}
#endif
}



UBOOL UMMOUIBag::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{

	if( IsVisible )
	{
		// Mouse wheel scroll, and not inside the UI
		if ((key == TEXT("MouseScrollUp") || key == TEXT("MouseScrollDown")) && (strHoverControllerName == TEXT("")))
			return TRUE;
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

void UMMOUIBag::ASStartDrag( UGFxObject* srcItemSlot, BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID)
{
	// TODO: Super::ASStartDrag( srcItemSlot, srcSlotType, srcContainerID, srcSlotID );

	//
	UMMOUISkillBar* skillbar = Cast<UMMOUISkillBar>( MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar ) );
	if( srcItemSlot == NULL || skillbar == NULL ){
		return;
	}

	skillbar->HintForDraggingItem( ESPType_Bag, srcSlotID );
}


void UMMOUIBag::ASMoneyExchange()
{
	//MallController::getSingleton().PopupGoodsBuyPanel(2);
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_MoneyExchange , TRUE , TEXT("") );
}

void UMMOUIBag::RequireOpenStall()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (mainPlayer->MoveType >= EMT_PrepareStall)
	{
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallSetting, TRUE, "");
	}
	else
	{
		
		UActorAttribute* attri = mainPlayer->ActorAttributes;
		if (attri)
		{
			if (attri && attri->LevelAttributes.Level >= OPEN_STALL_LEVEL)
			{
				UStallManager::getSingleton().RequestOpenStall();
			}
			else
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Stall.LevelLimit")), OPEN_STALL_LEVEL);
				MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table, TEXT("MMOGame.Quest.LevelLimit"));
			}
		}
	}
}

void UMMOUIBag::ASShowBagWeightTips(class UGFxObject* mc)
{
	INT bagW = 0;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( player && player->ActorAttributes )
	{
		bagW =  player->ActorAttributes->GetAttribute(catBagWeightLmt);
	}

	FString tipStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.Weight"));
	FString tips = FString::Printf(*tipStr, *appItoa(MMOFacade::getSingleton().GetCurrentBagWgt()),*appItoa(bagW));
	mc->SetString( TEXT("tipsHtml"),tips);
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOUIBag::ASHideBagWeightTips(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//===============================================================================
void UMMOUIBag::PrepareForRepair()
{
	UMMOUICharacter *pCharactorPanel = (UMMOUICharacter*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Character );
	if( pCharactorPanel == NULL ){
		return;
	}

	pCharactorPanel->Show( TRUE );

	UILayoutLeftRight repairUILayout( pCharactorPanel, MMOUI::GetUIManager()->Panel_Bag );
	repairUILayout.Layout( pCharactorPanel );
}
//=====================================================================================================
//experience bottle system
//=====================================================================================================
void UMMOUIBag::ExpContainerChanged( INT containerID, INT slotID )
{
	if( CurLoadedExpContainer._ConID != containerID || CurLoadedExpContainer._SlotID != slotID ){
		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	FClientItemInfo *pItemInfo = mmoFacade.FindItem( containerID, slotID );
	if( pItemInfo == NULL ){
		GFXPlayer->eventCallSetItemData( ExpContainerSlot , 0 , TEXT("") , -1, ESPType_ExpContainer, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );

		eventCallAsSetExpContainerProgress( 0, 0 );
		return;
	}

	UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( pItemInfo->m_nTemplateID );
	if( pItem == NULL ){
		return;
	}

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData( ExpContainerSlot, 1, mmoFacade.GetItemIconNamePersonalized( pItem, pItemInfo ), pItem->ItemId, ESPType_ExpContainer, TEXT(""), pItem->ItemColor+1, CurLoadedExpContainer._ConID, CurLoadedExpContainer._SlotID );

	//set storage amount
	INT expProgressCur = pItemInfo->m_nCurDur;
	INT expProgressMax = pItem->DefaultDuration;
	eventCallAsSetExpContainerProgress( expProgressCur, expProgressMax );
}
//=====================================================================================================
void UMMOUIBag::NotifyExpContainerFillIn( INT value )
{
	INT expProgressCur = 0;
	INT expProgressMax = 0;
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	FClientItemInfo *pItemInfo = mmoFacade.FindItem( CurLoadedExpContainer._ConID, CurLoadedExpContainer._SlotID );
	if( pItemInfo != NULL ){
		expProgressCur = pItemInfo->m_nCurDur;
		expProgressMax = pItemInfo->m_nMaxDur;
	}

	//set storage amount
	if( IsVisible ){
		ExpContainerChanged( CurLoadedExpContainer._ConID, CurLoadedExpContainer._SlotID );
	}
}
//=====================================================================================================
void UMMOUIBag::AutoUnloadCurExpContainer()
{
	OnRightMouseClickInExpContainer( CurLoadedExpContainer._SlotID );
}
#if 0
//----------------------------------------------------------------------------------------------------
UBOOL UMMOUIBag::UnloadExpContainer( INT toContainerID, INT toSlotID )
{
	UBOOL isUnloadSuccess = LoadExpContainer( -1, -1 );

	return isUnloadSuccess;
}
//----------------------------------------------------------------------------------------------------
UBOOL UMMOUIBag::LoadExpContainer( INT containerID, INT slotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	if( containerID < 0 || slotID < 0 ){
		if( CurLoadedExpContainer._ConID >= 0 && CurLoadedExpContainer._SlotID >= 0 ){

			//MMOFacade::getSingleton().LockItem( CurLoadedExpContainer._ConID, CurLoadedExpContainer._SlotID, -1 );

			GFXPlayer->eventCallSetItemData( ExpContainerSlot , 0 , TEXT("") , -1, ESPType_ExpContainer, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		}

		//
		CurLoadedExpContainer._ConID = CONSLOT_DEPOSIT;
		CurLoadedExpContainer._SlotID = -1;

		//refresh related display
		eventCallAsSetExpContainerProgress( 0, 0 );

		return TRUE;
	}

	//set
	FClientItemInfo *pItemInfo = mmoFacade.FindItem( containerID, slotID );
	if( pItemInfo == NULL ){
		FString errStrKey = TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrKey ), MsgInfoType_Table, errStrKey );
		return FALSE;
	}

	UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( pItemInfo->m_nTemplateID );
	if( pItem == NULL ){
		FString errStrKey = TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrKey ), MsgInfoType_Table, errStrKey );
		return FALSE;
	}

	//whether it is a spirit box
	if( pItem->ItemDuration != E_DURTYPE_DEPOSIT || pItem->ItemType != TEXT("ITEMTYPE_DEPOSIT") ){
		FString errStrKey = TEXT("MMOGame.ExpContainer.Error_ToLoadIsNotExpContainer");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrKey ), MsgInfoType_Table, errStrKey );
		return FALSE;
	}

	////unlock the equipment previously placed in the UI from the bag
	//if( CurLoadedExpContainer._ConID >= 0 && CurLoadedExpContainer._SlotID >= 0 ){
	//	MMOFacade::getSingleton().LockItem( CurLoadedExpContainer._ConID, CurLoadedExpContainer._SlotID, -1 );
	//}

	CurLoadedExpContainer._ConID = CONSLOT_DEPOSIT;
	CurLoadedExpContainer._SlotID =  DEPOSIT_EXP_SLOT;//slotID;

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData( ExpContainerSlot, 1, mmoFacade.GetItemIconNamePersonalized( pItem, pItemInfo ), pItem->ItemId, ESPType_ExpContainer, TEXT(""), pItem->ItemColor+1, CurLoadedExpContainer._ConID, CurLoadedExpContainer._SlotID );

	//lock the spirit previously placed in the bag
	//MMOFacade::getSingleton().LockItem( CurLoadedExpContainer._ConID, CurLoadedExpContainer._SlotID, ESPType_ExpContainer );

	//refresh related display
	//set storage amount
	eventCallAsSetExpContainerProgress( pItemInfo->m_nCurDur, pItem->DefaultDuration );

	return TRUE;
}
#endif
//====================================================================================================

void UMMOUIBag::GrayAllInBagNotInNpcShop(BOOL isToGray)
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( Player == NULL ){
		return;
	}

	bOpenByNpcShop = isToGray;

	//INT sellType = ShopManager::getSingleton().GetCurShopSellType();

	UINT bagCapability = Player->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );

	for( UINT j = 0; j < bagCapability; ++j)
	{

		FClientItemInfo *pItemInfo = mmoFacade.FindItem( E_CONSLOT_DEFAULT, j );
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
			mmoFacade.GrayItem( ESGMark_Bag, E_CONSLOT_DEFAULT, j, gray );
		}
		else //do not gray out
		{
			BagPage(0);
		}

	}
}

BOOL UMMOUIBag::GrayItemInBagNotInNpcShop(FClientItemInfo *pItemInfo,UItem *pItemInBag)
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

void UMMOUIBag::cleanAllCD()
{
	if(!BagGroup)
	{
		return;
	}

	INT cnt = MMOFacade::getSingleton().GetContainerSize(E_CONSLOT_DEFAULT);
	for (INT i = 0; i < cnt; ++i)
	{
		UGFxObject* itemSlot = BagGroup->GetObject(FString::Printf(TEXT("slot%d_mc"), i));
		StopItemSlotCD(itemSlot);
	}
}

void UMMOUIBag::ASOpenMaterialPanel()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_BagResource,bOpenResBag, "");
	ChangeResBagBtn();
}

void UMMOUIBag::ChangeResBagBtn()
{
	bOpenResBag = !bOpenResBag;

}