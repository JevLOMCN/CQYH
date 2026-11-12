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

IMPLEMENT_CLASS(UMMOUIRemakeFactory)

//========================================================================================================================================
void UMMOUIRemakeFactory::PreOnShow( UBOOL show )
{
	Super::PreOnShow( show );

	if( show ){
		UpdatePanel();
	}
	else{
		//ResetUI();
	}
}
//========================================================================================================================================
void UMMOUIRemakeFactory::OnNativeGameEvent( struct MMOGameEvent* evt )
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
	case GameEvent_MedicineRemakeResult:
	case GameEvent_BookRemakeResult:
		{
			MMOGameEvent_RemakeResult *remakeRst = (MMOGameEvent_RemakeResult*)evt;
			HandleRemakeResult( evt->GetEventType(), remakeRst->_Result, remakeRst->_ItemID );
		}
		break;
	}
}
//========================================================================================================================================
void UMMOUIRemakeFactory::BagSlotChange( INT bagId, INT slotId )
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
void UMMOUIRemakeFactory::OnBagSlotChanged( INT bagId, INT slotId, INT itemID )
{
	if( itemID == _ConsumptionIDForRemake && _ConsumptionIDForRemake != 0 ){
		UpdateConsumption();
	}
}
//========================================================================================================================================
void UMMOUIRemakeFactory::HandleRemakeResult( INT remakeType, INT isSuccess, INT remakedItemID )
{
	switch( remakeType )
	{
	case GameEvent_BookRemakeResult:
		{	
			HandleRemakeBookResult( isSuccess, remakedItemID );
		}
		break;
	case GameEvent_MedicineRemakeResult:
		{
			HandleRemakeMedicineResult( isSuccess, remakedItemID );
		}
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------------------------------------------------
void UMMOUIRemakeFactory::HandleRemakeMedicineResult( INT isSuccess, INT remakedItemID )
{
	if( !IsVisible ){
		return;
	}

	FString iconName = TEXT("");
	FString itemName = TEXT("");

	UBOOL isGood = FALSE;
	if( isSuccess ){
		INT gainedItemID = remakedItemID;

		UItem *pGainedItem = MMOFacade::getSingleton().FindItemTemplate( gainedItemID );
		if( pGainedItem != NULL ){
			itemName = pGainedItem->GetDisplayName();
			isGood = pGainedItem->Expensive;
			iconName = pGainedItem->GetIconName();
		}

		////Chat hint
		//FString hintStringName = TEXT("MMOGame.RemakeFacotry.MedicineResultSuccessChatHint");
		//FString hintString = MMOUI::ParseLocalizedPropertyPath( hintStringName );
		//MMOUI::GetUIManager()->delegateAddScreenMessage( hintString, MsgInfoType_Table, hintStringName );
		//MMOChatManager::getSingleton().AddSystemMessage(  FString::Printf( *chatHintFormat, *itemName ) );
	}
	else{
		////Chat hint
		//FString hintStringName = TEXT("MMOGame.RemakeFacotry.MedicineResultFailedChatHint");
		//FString hintString = MMOUI::ParseLocalizedPropertyPath( hintStringName );
		//MMOUI::GetUIManager()->delegateAddScreenMessage( hintString, MsgInfoType_Table, hintStringName );
		//MMOChatManager::getSingleton().AddSystemMessage(  chatHintFormat );
	}

	eventCallAsSetRemakeResult( isSuccess, iconName, itemName, isGood );
}
//--------------------------------------------------------------------------------------------------------------------
void UMMOUIRemakeFactory::HandleRemakeBookResult( INT isSuccess, INT remakedItemID )
{
	if( !IsVisible ){
		return;
	}

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
void UMMOUIRemakeFactory::ChangeMode( INT newMode, UBOOL isFromAS )
{
	_CurRemakeType = newMode;

	switch( _CurRemakeType )
	{
	case EReMkTp_Book:
		{	
			_ConsumptionIDForRemake = _ConsumptionIDForRemakeBook;
			_ConsumptionCntForRemake = _ConsumptionCntForRemakeBook;
		}
		break;
	case EReMkTp_Medicine:
		{
			_ConsumptionIDForRemake = _ConsumptionIDForRemakeMedicine;
			_ConsumptionCntForRemake = _ConsumptionCntForRemakeMedicine;
		}
		break;
	default:
		break;
	}
	//
	if( !isFromAS ){
		eventCallAsChangeMode( _CurRemakeType );
	}

	//
	if( IsVisible ){
		UpdatePanel();
	}
}
//========================================================================================================================================
void UMMOUIRemakeFactory::UpdatePanel()
{
	UpdateConsumption();
}
//========================================================================================================================================
void UMMOUIRemakeFactory::UpdateConsumption()
{
	//switch( _CurRemakeType )
	//{
	//case EReMkTp_Book:
	//	{
	//		UpdateRemakeBookConsumption();
	//	}
	//	break;
	//case EReMkTp_Medicine:
	//	{
	//		UpdateRemakeMedicineConsumption();
	//	}
	//	break;
	//default:
	//	break;
	//}
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	UItem *pConsumptionItem = mmoFacade.FindItemTemplate( _ConsumptionIDForRemake );
	if( pConsumptionItem == NULL ){
		eventCallAsSetRemakeConsumptionInfo( TEXT(""), 0, -1, 0, 0 );
	}
	else{
		FString iconName = pConsumptionItem->GetIconName();
		INT itemID = pConsumptionItem->ItemId;
		INT itemColor = pConsumptionItem->ItemColor + 1;
		INT inBagTotalCnt = mmoFacade.GetItemCountInBag( _ConsumptionIDForRemake, E_CONSLOT_DEFAULT );
		eventCallAsSetRemakeConsumptionInfo( iconName, itemID, itemColor, inBagTotalCnt, _ConsumptionCntForRemake );

		//INT inBagSlotIdx = -1;
		//FClientItemInfo *pRemakeConsumptionItemInfo = mmoFacade.FindFirstItemPosInContainer( _ConsumptionIDForRemakeBook, E_CONSLOT_DEFAULT, inBagSlotIdx );
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIRemakeFactory::UpdateRemakeMedicineConsumption()
{

}
//---------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIRemakeFactory::UpdateRemakeBookConsumption()
{
}
////========================================================================================================================================
//void UMMOUIRemakeFactory::PlayerMoneyChange(BYTE MoneyType, int DeltaMoney)
//{
//	if( !IsVisible ){
//		return;
//	}
//
//	UpdateRemakeFee();
//}
//========================================================================================================================================
void UMMOUIRemakeFactory::UpdateRemakeFee()
{
	//UINT64 curMoney = MMOFacade::getSingleton().GetPlayerMoney64(  E_MONEYTYPE_02 );
	//FString colorStr;
	//if( _FeeForRemake <= curMoney ){
	//	colorStr = TEXT("0xFFFFE0A6");
	//}
	//else{
	//	colorStr = UITextParser::GetTipsColor( TEXT("AlertColor") );
	//}
	//FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, _FeeForRemake, colorStr );
	//eventCallAsSetRemakeMoney( mnyStr );
}
//========================================================================================================================================
void UMMOUIRemakeFactory::Init()
{
	Super::Init();

	_CurRemakeType = EReMkTp_Medicine;
	_ConsumptionIDForRemake = _ConsumptionIDForRemakeMedicine;
	_ConsumptionCntForRemake = _ConsumptionCntForRemakeMedicine;

	//
	//InitPanelMc();

	SetTitleName();

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MedicineRemakeResult);
	evts.AddItem(GameEvent_BookRemakeResult);
	evts.AddItem(GameEvent_Item_Delete);
	EventAdapter = new TGameEventAdapter<UMMOUIRemakeFactory>(this, evts);
}
//----------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIRemakeFactory::NativeInitLoad()
{	
	Super::NativeInitLoad();

	//g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}
//========================================================================================================================================
void UMMOUIRemakeFactory::SetTitleName()
{
	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;

	
	switch( _CurRemakeType )
	{
	case EReMkTp_Book:
		{
			asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.RemakeFactory.UITitleNameRemakeBook") );
		}
		break;
	case EReMkTp_Medicine:
		{
			asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.RemakeFactory.UITitleNameRemakeBook") );
		}
		break;
	default:
		break;
	}

	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );
}
//========================================================================================================================================
//========================================================================================================================================
void UMMOUIRemakeFactory::ASRemakeConfirm()
{
	if( MMOFacade::getSingleton().IsBagFull() ){
		FString errorName =TEXT("MMOErrorInfo.ERROR_CODE_DEF.errItemBagFull");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(errorName), MsgInfoType_Table , errorName);
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(errorName) );
		return;
	}

	switch( _CurRemakeType )
	{
	case EReMkTp_Book:
		{
			CMessageHandler::GetInstance().Send_RemakeBook();
		}
		break;
	case EReMkTp_Medicine:
		{
			CMessageHandler::GetInstance().Send_RemakeMedicine();
		}
		break;
	default:
		break;
	}
}
//========================================================================================================================================
//========================================================================================================================================