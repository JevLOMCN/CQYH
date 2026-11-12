#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "net/message_handler.h"
#include "ClientActionConditionCheck.h"
#include "item_def.h"
#include "UILayout.h"
#include "MMOFacade.h"
#include "UITextParser.h"
#include "MMOChatManager.h"
#include "MMOEquipRemakeMgr.h"

IMPLEMENT_CLASS(UMMOUIRemakeBook)

//========================================================================================================================================
void UMMOUIRemakeBook::PreOnShow( UBOOL show )
{
	Super::PostOnShow( show );

	if( show ){
		UpdatePanel();
	}
	else{
		//ResetUI();
	}
}
//========================================================================================================================================

void UMMOUIRemakeBook::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;
			OnBagSlotChanged( itemDelEvt->ContainerIdx, itemDelEvt->SlotIdx, itemDelEvt->ItemTemplateID );
		}
		break;
	case GameEvent_BookRemakeResult:
		{
			MMOGameEvent_RemakeResult *remakeBookRst = (MMOGameEvent_RemakeResult*)evt;
			HandleRemakeResult( remakeBookRst->_Result, remakeBookRst->_ItemID );
		}
		break;
	}
}
//========================================================================================================================================
void UMMOUIRemakeBook::BagSlotChange( INT bagId, INT slotId )
{
	if( !IsVisible ){
		return;
	}

	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( bagId, slotId );
	if( pItemInfo != NULL ){
		OnBagSlotChanged( bagId, slotId, pItemInfo->m_nTemplateID );
	}
}
//----------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIRemakeBook::OnBagSlotChanged( INT bagId, INT slotId, INT itemID )
{
	if( itemID == _ConsumptionIDForRemakeBook && _ConsumptionIDForRemakeBook != 0 ){
		UpdateConsumption();
	}
}
//========================================================================================================================================
void UMMOUIRemakeBook::HandleRemakeResult( INT isSuccess, INT remakedItemID )
{
	FString iconName = TEXT("");
	FString itemName = TEXT("");

	UBOOL isGood = FALSE;
	if( isSuccess ){
		INT gainedItemID = remakedItemID;

		UItem *pGainedItem = MMOFacade::getSingleton().FindItemTemplate( gainedItemID );

		UMMOAbility* tAbilityInItem = NULL;
		if( pGainedItem != NULL ){
			tAbilityInItem = pGainedItem->GetToUseAbility();

			itemName = pGainedItem->GetDisplayName();

			isGood = pGainedItem->Expensive;
		}

		if( tAbilityInItem != NULL ){
			iconName = tAbilityInItem->GetAbilityIconName();
		}
		else if( pGainedItem != NULL ){
			iconName = pGainedItem->GetIconName();
		}
		////Chat hint
		//FString hintStringName = TEXT("MMOGame.RemakeFactory.BookResultSuccessChatHint");
		//FString hintString = MMOUI::ParseLocalizedPropertyPath( hintStringName );
		//MMOUI::GetUIManager()->delegateAddScreenMessage( hintString, MsgInfoType_Table, hintStringName );
		//MMOChatManager::getSingleton().AddSystemMessage(  FString::Printf( *chatHintFormat, *itemName ) );
	}
	else{
		////Chat hint
		//FString hintStringName = TEXT("MMOGame.RemakeFactory.BookResultFailedChatHint");
		//FString hintString = MMOUI::ParseLocalizedPropertyPath( hintStringName );
		//MMOUI::GetUIManager()->delegateAddScreenMessage( hintString, MsgInfoType_Table, hintStringName );
		//MMOChatManager::getSingleton().AddSystemMessage(  chatHintFormat );
	}
	
	eventCallAsSetRemakeResult( isSuccess, iconName, itemName, isGood );
}
//========================================================================================================================================
void UMMOUIRemakeBook::UpdatePanel()
{
	//Cost
	UpdateConsumption();
	////Money needed
	//UpdateRemakeFee();
}
//========================================================================================================================================
void UMMOUIRemakeBook::UpdateConsumption()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	UItem *pConsumptionItem = mmoFacade.FindItemTemplate( _ConsumptionIDForRemakeBook );
	if( pConsumptionItem == NULL ){
		eventCallAsSetRemakeConsumptionInfo( TEXT(""), 0, -1, 0, 0 );
	}
	else{
		FString iconName = pConsumptionItem->GetIconName();
		INT itemID = pConsumptionItem->ItemId;
		INT itemColor = pConsumptionItem->ItemColor + 1;
		INT inBagTotalCnt = mmoFacade.GetItemCountInBag( _ConsumptionIDForRemakeBook, E_CONSLOT_DEFAULT );
		eventCallAsSetRemakeConsumptionInfo( iconName, itemID, itemColor, inBagTotalCnt, _ConsumptionCntForRemakeBook );

		//INT inBagSlotIdx = -1;
		//FClientItemInfo *pRemakeConsumptionItemInfo = mmoFacade.FindFirstItemPosInContainer( _ConsumptionIDForRemakeBook, E_CONSLOT_DEFAULT, inBagSlotIdx );
	}
}
////========================================================================================================================================
//void UMMOUIRemakeBook::PlayerMoneyChange(BYTE MoneyType, int DeltaMoney)
//{
//	if( !IsVisible ){
//		return;
//	}
//
//	UpdateRemakeFee();
//}
//========================================================================================================================================
void UMMOUIRemakeBook::UpdateRemakeFee()
{
	//UINT64 curMoney = MMOFacade::getSingleton().GetPlayerMoney64(  E_MONEYTYPE_02 );
	//FString colorStr;
	//if( _FeeForRemakeBook <= curMoney ){
	//	colorStr = TEXT("0xFFFFE0A6");
	//}
	//else{
	//	colorStr = UITextParser::GetTipsColor( TEXT("AlertColor") );
	//}
	//FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, _FeeForRemakeBook, colorStr );
	//eventCallAsSetRemakeMoney( mnyStr );
}
//========================================================================================================================================
void UMMOUIRemakeBook::Init()
{
	Super::Init();

	//
	//InitPanelMc();

	SetTitleName();

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_BookRemakeResult);
	evts.AddItem(GameEvent_Item_Delete);
	EventAdapter = new TGameEventAdapter<UMMOUIRemakeBook>(this, evts);

	//
	//MMOEquipRemakeManager::getSingleton().GetMaterialIDAndCost( TEXT("ITEMTYPE_SPELLBOOK"), _ConsumptionIDForRemakeBook, _ConsumptionCntForRemakeBook );

}
//----------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIRemakeBook::NativeInitLoad()
{	
	Super::NativeInitLoad();

	//g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}
//========================================================================================================================================
void UMMOUIRemakeBook::SetTitleName()
{
	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.RemakeBook.UITitleNameRemakeBook") );

	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );
}
//========================================================================================================================================
//========================================================================================================================================
void UMMOUIRemakeBook::ASRemakeBookConfirm()
{
	CMessageHandler::GetInstance().Send_RemakeBook();
}
//========================================================================================================================================
//========================================================================================================================================