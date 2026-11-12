#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMsgBoxManager.h"
#include "ClientActionConditionCheck.h"

IMPLEMENT_CLASS(UItemSlotActionListener_ItemSplit)

	extern bool g_lockMouseCursorForUI;

UBOOL UItemSlotActionListener_ItemSplit::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UItemSlotActionListener_ItemSplit::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	if( SlotType != ESPType_Bag || ContainerID != E_CONSLOT_DEFAULT )
	{
		if (SlotType == ESPType_BagResource || ContainerID == E_CONSLOT_RESOURCE)
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.BagResourcr.CannotSplit")),MsgInfoType_Table,TEXT("MMOGame.BagResourcr.CannotSplit") );		
			MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();
		}
		else
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.CannotSplit")),MsgInfoType_Table,TEXT("MMOGame.Bag.CannotSplit") );		
			MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();
		}
		
		return TRUE;
	}

	ItemInfo* item = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
	if(item)
	{
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(item->m_nTemplateID);
		if(itemTemp)
		{

			if(item->GetCurStackCount(itemTemp) > 1/*itemTemp->ItemDuration == E_DURTYPE_STACK && item->m_nMaxDur > 1 && item->m_nCurDur > 1*/ )
			{
				FScriptDelegate funOk;
				funOk.Object = this;
				funOk.FunctionName = "SplitItem";

				FScriptDelegate funCancel;
				funCancel.Object = this;
				funCancel.FunctionName = "SplitCancel";

				UGFxObject* box = MMOMsgBoxManager::getSingleton().AddModalInputMsgbox( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.ItemSplit")) , SlotID ,
					MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.SplitNum")) , funOk , funCancel , eInputFormatNumOnly );

				if(box)
				{
					TArray<struct FASValue> para;
					para.AddZeroed();
					para(0).Type = AS_String;
					para(0).S = TEXT("1");
					box->Invoke( TEXT("SetInputText") , para );
				}
			}
			else
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.CannotSplit")) ,MsgInfoType_Table,TEXT("MMOGame.Bag.CannotSplit"));			
			}

			UMMOUIBag *pBagPanel = Cast<UMMOUIBag>(MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag ));
			MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(pBagPanel->IdentifyAction);
		}


	}
	MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();

	return TRUE;
}

UBOOL UItemSlotActionListener_ItemSplit::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();
	//MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(LastItemActionListener);	
	return TRUE;
}

UBOOL UItemSlotActionListener_ItemSplit::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return FALSE;
}

void UItemSlotActionListener_ItemSplit::NativeOnSubscribeItemSlotAction()
{

	MMOFacade::getSingleton().SetMouseCursor(EMC_Split );
	g_lockMouseCursorForUI = true;
}

void UItemSlotActionListener_ItemSplit::NativeOnUnSubscribeItemSlotAction()
{
	g_lockMouseCursorForUI = false;
	MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
}

INT  UItemSlotActionListener_ItemSplit::GetSlotType()
{
	return (INT)E_CONSLOT_DEFAULT;
}

void  UItemSlotActionListener_ItemSplit::SplitItem( UGFxObject* mc )
{
	INT slotId = mc->GetInt( TEXT("iteminfoid") );
	FClientItemInfo* item = MMOFacade::getSingleton().FindItem( E_CONSLOT_DEFAULT , slotId );
	if( item )
	{
		//check validity
		ClientActionCondition_SplitItem checker;

		FClientActionConditionContex contex;
		contex.Item = item;

		if( !checker.CheckClientActionCondition(contex) ){
			return;
		}

		FString inputMny = mc->GetObject( TEXT("textInput") )->GetObject( TEXT("textField") )->GetString( TEXT("text") );
		INT64 splitNum = (INT64)appAtoi( *inputMny );

		if( splitNum == 0 )
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.SplitCanotZero")),MsgInfoType_Table,TEXT("MMOGame.Bag.SplitCanotZero") );	
		}
		else if(item->m_nMaxDur > 1 && item->m_nCurDur > splitNum )
		{
			INT desSlotId = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_DEFAULT);
			if( desSlotId >= 0 )
			{
				CMessageHandler::GetInstance().Send_SplitItem( E_CONSLOT_DEFAULT , slotId , E_CONSLOT_DEFAULT , desSlotId , splitNum );
			}
			else
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.NoSpaceSplit")),MsgInfoType_Table,TEXT("MMOGame.Bag.NoSpaceSplit") );	
			}
		}
		else
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.SplitNumError")),MsgInfoType_Table,TEXT("MMOGame.Bag.SplitNumError") );	
		}

	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.CannotSplit")),MsgInfoType_Table,TEXT("MMOGame.Bag.CannotSplit") );		
	}

	MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();//
	//MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(LastItemActionListener);
}

void  UItemSlotActionListener_ItemSplit::SplitCancel( UGFxObject* mc )
{
	MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();//
	//MMOUI::GetUIManager()->SetCurrentItemSlotActionListener(LastItemActionListener);
}

UBOOL UItemSlotActionListener_ItemSplit::OnDoubleClickSlot(class UGFxObject* SrcSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,UBOOL bDefault)
{
	return FALSE;
}
