#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
//#include "MMOPowerManager.h"

IMPLEMENT_CLASS(UMMOUIEquipGainHintBox)

//================================================================================================
void UMMOUIEquipGainHintBox::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		//FVector2D scrSz = MMOFacade::GetScreenSize();

		//SetPosition( scrSz.X - PanelX, scrSz.Y - PanelY );

		//
		if( _NewGainedEquip.Num() > 0 ){
			const FOperationItemBagPos &equipPos = _NewGainedEquip.Last();
			ShowContent( equipPos._ConID, equipPos._SlotID );
		}
	}
	else{
		ClearUI();
	}
}
//========================================================================================
void UMMOUIEquipGainHintBox::ClearUI()
{
	_NewGainedEquip.Empty();
}

//========================================================================================
void UMMOUIEquipGainHintBox::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;

			if( IsVisible ){
				ItemsChanged( itemDelEvt->ContainerIdx, itemDelEvt->SlotIdx );
			}
		}
		break;
	case GameEvent_Item_SlotSwapped:
		{
			MMOGameEvent_ItemSwap *itemSwapEvt = (MMOGameEvent_ItemSwap*)evt;
			OnItemSwapped( itemSwapEvt->SrcConID, itemSwapEvt->SrcSlotID, itemSwapEvt->DstConID, itemSwapEvt->DstSlotID );
		}
		break;
	}
}
//
//========================================================================================================================================
void UMMOUIEquipGainHintBox::ItemsChanged( INT bagId, INT slotId )
{
	if( !IsVisible ){
		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	//
	INT cnt = _NewGainedEquip.Num();
	for( INT i = 0; i < cnt; ++i ){
		const FOperationItemBagPos &gainedItemIdx = _NewGainedEquip(i);
		if( gainedItemIdx._ConID == bagId && gainedItemIdx._SlotID == slotId ){
			//if( i == 0 ){
			//	TryShowNextOrHide();
			//}
			//else{
			//	_NewGainedEquip.Remove( i );
			//}

			if( i == cnt - 1 ){
				//The currently displayed one was deleted
				Show( FALSE );
			}
			
			break;
		}
	}
}

//
void UMMOUIEquipGainHintBox::OnItemSwapped( INT srcConIdx, INT srcSlotIdx, INT dstConIdx, INT dstSlotIdx )
{
	INT cnt = _NewGainedEquip.Num();
	for( INT i = 0; i < cnt; ++i ){
		FOperationItemBagPos &gainedItemIdx = _NewGainedEquip(i);
		if( gainedItemIdx._ConID == srcConIdx && gainedItemIdx._SlotID == srcSlotIdx ){
			gainedItemIdx._ConID = dstConIdx;
			gainedItemIdx._SlotID = dstSlotIdx;

			break;
		}
		else if( gainedItemIdx._ConID == dstConIdx && gainedItemIdx._SlotID == dstSlotIdx ){
			gainedItemIdx._ConID = srcConIdx;
			gainedItemIdx._SlotID = srcSlotIdx;

			break;
		}
	}
}
//===================================================================================================================================
void UMMOUIEquipGainHintBox::AddNewEquipGain( INT conID, INT slotID )
{
	FOperationItemBagPos newEquipPos;
	newEquipPos._ConID = conID;
	newEquipPos._SlotID = slotID;

	_NewGainedEquip.Push( newEquipPos );
}

//===================================================================================================================================
void UMMOUIEquipGainHintBox::TryShowNextOrHide()
{
	_NewGainedEquip.Remove( 0 );

	//
	INT leftCnt = _NewGainedEquip.Num();
	if( leftCnt <= 0 ){
		Show( FALSE );
		return;
	}

	//
	const FOperationItemBagPos &equipPos = _NewGainedEquip( 0 );
	ShowContent( equipPos._ConID, equipPos._SlotID );
}
//===================================================================================================================================
void UMMOUIEquipGainHintBox::ShowContent( INT conID, INT slotID )
{
	if( conID < 0 || slotID < 0 ){
		GFXPlayer->eventCallSetItemData( _GainedEquipIconSlot, 0, TEXT(""), -1, ESPType_EquipGainHintBox, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		return;
	}

	//
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	const FClientItemInfo *pEquipItemInfo = mmoFacade.FindItem( conID, slotID );
	if( pEquipItemInfo == NULL ){
		return;
	}

	UItem *pItemTemplate = mmoFacade.FindItemTemplate( pEquipItemInfo->m_nTemplateID );

	//
	GFXPlayer->eventCallSetItemData( _GainedEquipIconSlot, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pItemTemplate, pEquipItemInfo ), pItemTemplate->ItemId, 
									 ESPType_EquipGainHintBox, TEXT(""), pItemTemplate->ItemColor+1, conID, slotID );

	//
	FString equipScore = appItoa( pEquipItemInfo->Pow );
	_EquipScoreTf->SetText( equipScore );
}

//===================================================================================================================================
void UMMOUIEquipGainHintBox::CheckNewEquipment()
{
	UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Bag );
	if( pBagPanel != NULL ){
		MMOUI::GetUIManager()->delegateOpenBag( E_CONSLOT_DEFAULT );
	}

	//
	Show( FALSE );
}

//===================================================================================================================================
void UMMOUIEquipGainHintBox::Init()
{
	Super::Init();

	InitMc();

	check(NULL == EventAdapter);
	if (NULL == EventAdapter){
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_Item_Delete);
		evts.AddItem(GameEvent_Item_SlotSwapped);
		EventAdapter = new TGameEventAdapter<UMMOUIEquipGainHintBox>(this, evts);
	}
}

//
void UMMOUIEquipGainHintBox::InitMc()
{
	_GainedEquipIconSlot = GetObject( TEXT("GainedEquipIconSlot") );
	_GainedEquipIconSlot->SetBool( TEXT("DisableDrag"), TRUE );

	_EquipScoreTf = GetObject( TEXT("EquipScoreTf") );
	_HintBoxMsgTf = GetObject( TEXT("HintBoxMsgTf") );
	_CloseHintBtn = GetObject( TEXT("CloseHintBtn") );
	_CheckEquipBtn = GetObject( TEXT("CheckEquipBtn") );

	//
	PanelX = eventGetWidth() + PanelOrigX;
	PanelY = eventGetHeight() + PanelOrigY;
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIEquipGainHintBox::ASCloseNewEquipmentHintBox()
{
	Show( FALSE );
}

//==============================================================================================
void UMMOUIEquipGainHintBox::ASCheckNewEquipment()
{
	CheckNewEquipment();
}
//===============================================================================================================
//===============================================================================================================