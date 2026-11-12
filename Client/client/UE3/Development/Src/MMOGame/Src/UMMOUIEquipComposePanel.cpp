#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "EquipDecomposeManager.h"
#include "EquipSlotManager.h"
#include "UITextParser.h"
#include "UILayout.h"

IMPLEMENT_CLASS(UMMOUIEquipComposePanel)

void UMMOUIEquipComposePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		UpdateUI(EquipDecomposeManager::getSingleton().GetComposedId());
	}
	else
	{
		//ClearUI();
	}
}

void UMMOUIEquipComposePanel::ClearUI()
{
	UGFxObject* itemSlot = GetObject(TEXT("EquipSlot"));
	if(itemSlot)
	{
		//GFXPlayer->SetNoninteractiveIconSlotByItem( itemSlot, NULL, TEXT(""), ESPType_EquipCompose );
		GFXPlayer->SetItemSlotData(itemSlot,NULL,ESPType_EquipCompose);
	}
	//Synthesize item
	eventCallAStoSetItemName( TEXT("") );

	//Material info
	for(INT i = 0; i < COMPOSE_CONSUME_COUNT; ++i)
	{
		UGFxObject* mat_mc = GetObject(FString::Printf(TEXT("MatConsume_%d"),i));
		//check(mat_mc != NULL);
		if(mat_mc == NULL) continue;

		GFXPlayer->SetItemSlotData(mat_mc,NULL,ESPType_EquipCompose);
		/*UGFxObject* cnt_mc = mat_mc->GetObject(TEXT("mat_cnt"));
		check(cnt_mc != NULL);

		GFXPlayer->SetIconShowByItem(mat_mc, NULL);
		cnt_mc->SetText(TEXT(""));*/

	}

	//Gold info
	UGFxObject* coin_mc = GetObject(TEXT("CostText"));
	if(coin_mc != NULL){
		coin_mc->SetString( TEXT("htmlText"), TEXT("") );
	}
}

void UMMOUIEquipComposePanel::UpdateUI(INT itemId)
{
	EquipComposeTemplate& ComposeTemplate = EquipDecomposeManager::getSingleton().GetComposeTemplateById(itemId);
	
	UGFxObject* itemSlot = GetObject(TEXT("EquipSlot"));
	UItem* pEquip = MMOFacade::getSingleton().FindItemTemplate(ComposeTemplate.equipId);
	if( pEquip == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UpdateUI ERROR]AT Equip ID:%d"), ComposeTemplate.equipId );
		return;
	}
	if(itemSlot)
	{
		//GFXPlayer->SetNoninteractiveIconSlotByItem( itemSlot, pEquip, TEXT(""), ESPType_EquipCompose );
		GFXPlayer->SetItemSlotData(itemSlot,pEquip,ESPType_EquipCompose);
	}

	//Synthesize item
	if( pEquip != NULL ){
		eventCallAStoSetItemName( pEquip->GetDisplayName() );
	}
	else{
		eventCallAStoSetItemName( TEXT("") );
	}

	//Material info
	UpdateConsume( &ComposeTemplate );

	//Gold info
	UpdateFee( &ComposeTemplate );
}

//========================================================================================================================================
void UMMOUIEquipComposePanel::BagSlotChange( INT bagId, INT slotId )
{
	if( !IsVisible || E_CONSLOT_DEFAULT != bagId ){
		return;
	}

	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( bagId, slotId );
	if( pItemInfo != NULL ){
		INT curComposeItemID = EquipDecomposeManager::getSingleton().GetComposedId();
		EquipComposeTemplate& composeTemplate = EquipDecomposeManager::getSingleton().GetComposeTemplateById( curComposeItemID );

		UBOOL needUpdate = FALSE;
		for( INT i = 0; i < COMPOSE_CONSUME_COUNT; ++i ){
			if( composeTemplate.matId[i] == pItemInfo->m_nTemplateID ){
				needUpdate = TRUE;
				break;
			}
		}

		//Material info
		if( needUpdate == TRUE ){
			UpdateConsume( &composeTemplate );
		}	
	}
	else{
		INT curComposeItemID = EquipDecomposeManager::getSingleton().GetComposedId();
		EquipComposeTemplate& composeTemplate = EquipDecomposeManager::getSingleton().GetComposeTemplateById( curComposeItemID );
		UpdateConsume( &composeTemplate );
	}
}

//========================================================================================================================================
void UMMOUIEquipComposePanel::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
	if( !IsVisible ){
		return;
	}
	//
	INT curComposeItemID = EquipDecomposeManager::getSingleton().GetComposedId();
	EquipComposeTemplate& composeTemplate = EquipDecomposeManager::getSingleton().GetComposeTemplateById( curComposeItemID );
	UpdateFee( &composeTemplate );
}

//==========================================================================================
void UMMOUIEquipComposePanel::UpdateFee( const struct EquipComposeTemplate *composeTemplate )
{
	if( composeTemplate == NULL ){
		return;
	}

	UGFxObject* coin_mc = GetObject(TEXT("CostText"));
	//check(coin_mc != NULL);
	if(coin_mc == NULL) return;

	FString moneyColor;

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL ){
		return;
	}

	if(composeTemplate->cost > mainPlayer->GetMoney(MONEYTYPE_02))
	{
		moneyColor = UITextParser::GetNormalColor( (TEXT("Decrease") ) );
	}
	else
	{
		moneyColor = UITextParser::GetNormalColor( (TEXT("Increase") ) );
	}
	coin_mc->SetString( TEXT("htmlText"), UITextParser::HTMLMoneyIconString( MONEYTYPE_02, composeTemplate->cost, moneyColor ) );
}
//==========================================================================================
void UMMOUIEquipComposePanel::UpdateConsume( const struct EquipComposeTemplate *composeTemplate )
{
	for(INT i = 0; i < COMPOSE_CONSUME_COUNT; ++i)
	{
		UGFxObject* mat_mc = GetObject(FString::Printf(TEXT("MatConsume_%d"),i));
		if( mat_mc == NULL ){
			continue;
		}

		/*UGFxObject* cnt_mc = mat_mc->GetObject(TEXT("mat_cnt"));
		check(cnt_mc != NULL);*/
		UGFxObject* text_mc = GetObject(FString::Printf(TEXT("MatConsumeTf_%d"),i));//mat_mc->GetObject(TEXT("valueAmount"));
		if( composeTemplate != NULL && composeTemplate->matId[i] > 0)
		{
			UItem* pItem = MMOFacade::getSingleton().FindItemTemplate(composeTemplate->matId[i]);
			if( pItem == NULL ){
				warnf(NAME_Error, TEXT("======!!!!======= [UpdateConsume ERROR]AT Item Consume:%d"), composeTemplate->matId[i] );
				continue;
			}

			GFXPlayer->SetItemSlotData(mat_mc, pItem, ESPType_EquipCompose);
			INT Cnt_Have = MMOFacade::getSingleton().GetItemCountInBag( composeTemplate->matId[i], E_CONSLOT_DEFAULT );
			//cnt_mc->SetText(FString::Printf(TEXT("%d/%d"),Cnt_Have,composeTemplate->matCnt[i]));
			if( text_mc != NULL )
			{
				text_mc->SetText(FString::Printf(TEXT("%d/%d"), Cnt_Have, composeTemplate->matCnt[i]));
				if(composeTemplate->matCnt[i] > Cnt_Have)
				{
					text_mc->SetInt( TEXT("textColor") , 0xFF0000 );
				}
				else
				{
					text_mc->SetInt( TEXT("textColor") , 0x00FF00 );
				}
			}
		}
		else
		{
			GFXPlayer->SetItemSlotData(mat_mc, NULL, ESPType_EquipCompose);
			//cnt_mc->SetText(TEXT(""));
			if( text_mc != NULL )
			{
				text_mc->SetText( TEXT("") );
			}
		}

	}
}
//=========================================================================================
void UMMOUIEquipComposePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;
			BagSlotChange( itemDelEvt->ContainerIdx, itemDelEvt->SlotIdx );
		}
	}
}

//========================================================================================================================================
void UMMOUIEquipComposePanel::NotifyComposeResult( INT result )
{
	if( !IsVisible )
	{
		return;
	}
	
	eventCallASPlayResultAnim( result );
}

//----------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIEquipComposePanel::NotifyComposeSuccessAnimPlayedOver()
{
	INT curComposeItemID = EquipDecomposeManager::getSingleton().GetComposedId();
	INT composeItemHave = MMOFacade::getSingleton().GetItemCountInBag( curComposeItemID, E_CONSLOT_DEFAULT );
	if( composeItemHave <= 0 ){
		Show( FALSE );
	}
}

UBOOL UMMOUIEquipComposePanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIEquipComposePanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIEquipComposePanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return FALSE;
}

UBOOL UMMOUIEquipComposePanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

//
void UMMOUIEquipComposePanel::Init()
{
	Super::Init();

	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipCompose.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );

	//check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Item_Delete);
	EventAdapter = new TGameEventAdapter<UMMOUIEquipComposePanel>(this, evts);

	g_layoutList.push_back(new UILayoutRightLeft(this, MMOUI::GetUIManager()->Panel_Bag));
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIEquipComposePanel::ASCompose()
{
	INT id = EquipDecomposeManager::getSingleton().GetComposedId();
	if( id >= 0 )
	{
		EquipDecomposeManager::getSingleton().ComposeEquip(id);
	}
}

void UMMOUIEquipComposePanel::ASNotifyComposeSuccessAnimPlayedOver()
{
	NotifyComposeSuccessAnimPlayedOver();
}
//===============================================================================================================
//===============================================================================================================