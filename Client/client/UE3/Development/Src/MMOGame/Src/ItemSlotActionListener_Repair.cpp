#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(UItemSlotActionListener_Repair)


UBOOL UItemSlotActionListener_Repair::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
    return FALSE; 
} 

 UBOOL UItemSlotActionListener_Repair::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	UBOOL isRepairOK = FALSE;
	ItemInfo* item = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
	if(item != NULL)
	{
		FString errorName;

		// only repair equipment, and only repair when durability has been lost
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate( item->m_nTemplateID );
		UEquip* equip = Cast<UEquip>( itemTemp );
		if( equip == NULL || itemTemp->ItemType != TEXT("ITEMTYPE_EQUIP") )
		{
			errorName = TEXT("MMOGame.Bag.RepairNotEquip");
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName);
			MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();
			return TRUE;
		}

		//
		if(item->m_nMaxDur <= 0 || !equip->CanRepair )
		{
			errorName = TEXT("MMOGame.Bag.RepairCannot");
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName);
			MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();
			return TRUE;
		}

		//
		if(item->m_nMaxDur > 0 && (item->m_nCurDur != item->m_nMaxDur) )
		{
			CMessageHandler::GetInstance().Send_RepairItem(ContainerID, SlotID);
		}
		else
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.EquipNoNeed")),MsgInfoType_Table,TEXT("MMOGame.Bag.EquipNoNeed") );
			// TODO: play sound effects that need no modification, etc.
		}
	}

	MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();
	return TRUE;
}

UBOOL UItemSlotActionListener_Repair::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();//
	//MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(LastItemActionListener);	
	return TRUE;
}

UBOOL UItemSlotActionListener_Repair::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return FALSE;
}

extern bool g_lockMouseCursorForUI;
void UItemSlotActionListener_Repair::NativeOnSubscribeItemSlotAction()
{
	MMOFacade::getSingleton().SetIsRepairing(TRUE);
	MMOFacade::getSingleton().SetMouseCursor(EMC_Repair);
	g_lockMouseCursorForUI = true;
}

void UItemSlotActionListener_Repair::NativeOnUnSubscribeItemSlotAction()
{
	g_lockMouseCursorForUI = false;
	MMOFacade::getSingleton().SetIsRepairing(FALSE);
	MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
}

INT  UItemSlotActionListener_Repair::GetSlotType()
{
	return (INT)E_CONSLOT_EQUIPPED;
}

UBOOL UItemSlotActionListener_Repair::OnDoubleClickSlot(class UGFxObject* SrcSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,UBOOL bDefault)
{
	return FALSE;
}