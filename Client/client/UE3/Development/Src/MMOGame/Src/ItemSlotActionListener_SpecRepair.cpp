#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(UItemSlotActionListener_SpecRepair)

UBOOL UItemSlotActionListener_SpecRepair::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if( bDefault ){
		return FALSE;
	}

	if( ( ContainerID != E_CONSLOT_EQUIPPED ) && ( ContainerID != E_CONSLOT_DEFAULT ) ){
		return TRUE;
	}

	ItemInfo* item = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
	if( item == NULL ){
		return TRUE;
	}
	UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate( item->m_nTemplateID );
	if( itemTemp == NULL ){
		return TRUE;
	}

	// only repair equipment, and only repair when durability has been lost
	if( itemTemp->ItemType != TEXT("ITEMTYPE_EQUIP") ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.RepairNotEquip")) ,MsgInfoType_Table,TEXT("MMOGame.Bag.RepairNotEquip"));
		return TRUE;
	}

	if( item->m_nCurDur == item->m_nMaxDur ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.EquipNoNeed")),MsgInfoType_Table,TEXT("MMOGame.Bag.EquipNoNeed") );
		// TODO: play sound effects that need no modification, etc.
		return TRUE;
	}

	CMessageHandler::GetInstance().Send_SpecRepairItem(ContainerID, SlotID);
	//MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();

	return TRUE;
}

//UBOOL UItemSlotActionListener_SpecRepair::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
//{
//	if(bDefault)return FALSE;
//
//	MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();//
//	return TRUE;
//}

extern bool g_lockMouseCursorForUI;
void UItemSlotActionListener_SpecRepair::NativeOnSubscribeItemSlotAction()
{
	//Super::NativeOnSubscribeItemSlotAction();

	MMOFacade::getSingleton().SetIsSpecialRepairing(TRUE);
	MMOFacade::getSingleton().SetMouseCursor(EMC_Repair);
	g_lockMouseCursorForUI = true;
}

void UItemSlotActionListener_SpecRepair::NativeOnUnSubscribeItemSlotAction()
{
	//Super::NativeOnSubscribeItemSlotAction();

	g_lockMouseCursorForUI = false;
	MMOFacade::getSingleton().SetIsSpecialRepairing(FALSE);
	MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
}

//
INT  UItemSlotActionListener_SpecRepair::GetSlotType()
{
	return (INT)E_CONSLOT_EQUIPPED;
}