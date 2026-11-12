#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOHeavenManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOFacade.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIHeavenGasMainPanel)


void UMMOUIHeavenGasMainPanel::OnClickOneKeyMerge()
{
	const GasSlot* pkTarget =  MMOHeavenManager::getSingleton().GetTargetSlot();
	if(pkTarget)
	{
		if(MMOHeavenManager::getSingleton().CheckHavePurpleGasBag())
			ConfirmMergePurple();
		else
			ConfirmMergeHint();
	}	
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HeavenGasMergeAim")),MsgInfoType_Table,TEXT("MMOGame.Heaven.HeavenGasMergeAim"));
		return;
	}
}
void UMMOUIHeavenGasMainPanel::ConfirmMergeHint()
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ConfirmHeavenMerge");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("CancelHeavenMerge");	

	FString text;
	BOOL bRes = MMOHeavenManager::getSingleton().GenerateHeavenMergeGasHint(text);
	if(bRes)
		HeavenMergeConfirmBox = MMOMsgBoxManager::getSingleton().AddModalMessageBox(text, 0, funYes, funNo);
	else
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Heaven.GasBagNoGasSorb"));
}
void UMMOUIHeavenGasMainPanel::ConfirmHeavenMerge(class UGFxObject* mc)
{
	if(mc == NULL)
		return;
	const GasSlot* pkTarget =  MMOHeavenManager::getSingleton().GetTargetSlot();
	if(pkTarget)
		CMessageHandler::GetInstance().Send_MergeGasOneKey(pkTarget->nTagID);
	HeavenMergeConfirmBox = NULL;
}
void UMMOUIHeavenGasMainPanel::CancelHeavenMerge(class UGFxObject* mc)
{
	if(mc == NULL)
		return;
	HeavenMergeConfirmBox = NULL;
}

void UMMOUIHeavenGasMainPanel::ConfirmMergePurple()
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ConfirmHeavenMergePurpleGas");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("CancelHeavenMergePurpleGas");	

	FString text = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.MergePurpleGasHint"));
	HeavenMergePurpleConfirmBox = MMOMsgBoxManager::getSingleton().AddModalMessageBox(text, 0, funYes, funNo);
}

void UMMOUIHeavenGasMainPanel::ConfirmHeavenMergePurpleGas(class UGFxObject* mc)
{
	if(mc == NULL)
		return;
	ConfirmMergeHint();
	HeavenMergePurpleConfirmBox = NULL;
}
void UMMOUIHeavenGasMainPanel::CancelHeavenMergePurpleGas(class UGFxObject* mc)
{
	if(mc == NULL)
		return;
	HeavenMergePurpleConfirmBox = NULL;
}
UBOOL UMMOUIHeavenGasMainPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if (ESPType_HeavenGasBank == SlotType || ESPType_HeavenGasBag == SlotType || ESPType_HeavenGasEquip == SlotType)
		return TRUE;
	return FALSE;
}
UBOOL UMMOUIHeavenGasMainPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if( mc && mc->GetBool( "isSlotLocked"))
	{
		if(ESPType_HeavenGasEquip == SlotType)
		{
			INT CurCapacity = MMOHeavenManager::getSingleton().GetEquipContainerSize(); 
			if( SlotID < CurCapacity)
			{
				return TRUE;
			}
			if( SlotID > CurCapacity )
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.EquipGasOpenHint")));
				return TRUE;
			}
			if( SlotID == CurCapacity)
			{
				INT needMoney = MMOHeavenManager::getSingleton().GetNeedMoneyOpenSlot(sasbtEquip,1);

				FScriptDelegate fun(EC_EventParm);
				fun.Object = this;
				fun.FunctionName = TEXT("ExpandGasEquipOk");

				FScriptDelegate funNone(EC_EventParm);

				//FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.UnlockNeedMoney") );
				FString txt = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.OpenSlotHint")),1,*UITextParser::HTMLMoneyIconString(MONEYTYPE_02, needMoney));
				//FString txt = FString::Printf( *temp , 1/*( SlotID +1 - CurCapacity )*/ , needMoney , *MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_02") ) );
				UGFxObject* msgMc = MMOMsgBoxManager::getSingleton().AddModalMessageBox( txt , ContainerID , fun , funNone );
				if(msgMc)
					msgMc->SetInt( TEXT("itemdataid") , 1/*SlotID + 1 - CurCapacity*/ );

				return TRUE;
			}
		}
		else if (ESPType_HeavenGasBag == SlotType)
		{
			INT CurCapacity = MMOHeavenManager::getSingleton().GetBagContainerSize(); 
			if( SlotID < CurCapacity - 1 )
			{
				return TRUE;
			}
			if( SlotID >= CurCapacity - 1 )
			{
				INT needMoney =  MMOHeavenManager::getSingleton().GetNeedMoneyOpenSlot(sasbtBag,(SlotID + 1 - CurCapacity));

				FScriptDelegate fun(EC_EventParm);
				fun.Object = this;
				fun.FunctionName = TEXT("ExpandGasBagOk");

				FScriptDelegate funNone(EC_EventParm);

				//FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.UnlockNeedMoney") );
				//FString txt = FString::Printf( *temp ,1, needMoney , *MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_02") ) );
				FString txt = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.OpenSlotHint")),(SlotID + 1 - CurCapacity),*UITextParser::HTMLMoneyIconString(MONEYTYPE_02, needMoney));
				UGFxObject* msgMc = MMOMsgBoxManager::getSingleton().AddModalMessageBox( txt , ContainerID , fun , funNone );
				if(msgMc)
					msgMc->SetInt( TEXT("itemdataid") ,(SlotID + 1 - CurCapacity));

				return TRUE;
			}
		}
		else if (ESPType_HeavenGasBank == SlotType)
		{
			INT CurCapacity = MMOHeavenManager::getSingleton().GetBankContainerSize(); 
			if( SlotID < CurCapacity - 1 )
			{
				return TRUE;
			}
			if( SlotID >= CurCapacity - 1 )
			{
				INT needMoney = MMOHeavenManager::getSingleton().GetNeedMoneyOpenSlot(sasbtStall,(SlotID + 1 - CurCapacity));
				FScriptDelegate fun(EC_EventParm);
				fun.Object = this;
				fun.FunctionName = TEXT("ExpandGasBankOk");

				FScriptDelegate funNone(EC_EventParm);

				//FString temp = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.UnlockNeedMoney") );
				//FString txt = FString::Printf( *temp ,1,needMoney , *MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_02") ) );
				FString txt = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.OpenSlotHint")),(SlotID + 1 - CurCapacity),*UITextParser::HTMLMoneyIconString(MONEYTYPE_02, needMoney));
				UGFxObject* msgMc = MMOMsgBoxManager::getSingleton().AddModalMessageBox( txt , ContainerID , fun , funNone );
				if(msgMc)
					msgMc->SetInt( TEXT("itemdataid") , (SlotID + 1 - CurCapacity));
				return TRUE;
			}
		}
	}
	if (ESPType_HeavenGasBank == SlotType || ESPType_HeavenGasBag == SlotType || ESPType_HeavenGasEquip == SlotType)
		return TRUE;
	return FALSE;
}

UBOOL UMMOUIHeavenGasMainPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	if(ESPType_HeavenGasBag == SlotType&&SlotID != UCONST_GasBagSlotMaxNum)
	{
		INT nDesID = MMOHeavenManager::getSingleton().GetCanEquipSlotID();
		CMessageHandler::GetInstance().Send_EquipGasItem(nDesID,SlotID,sasbtBag);
	}
	if (ESPType_HeavenGasBank == SlotType|| ESPType_HeavenGasEquip == SlotType)
		return TRUE;
	return FALSE;
}
UBOOL UMMOUIHeavenGasMainPanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	OnStartDragGas(SrcContainerID,SrcSlotID,FALSE);
	if((DestContainerID == SrcContainerID )&& (SrcSlotID == DestSlotID))
		return TRUE;
	if(DestSlotMC)
	{
		if (DestSlotMC->GetBool("isSlotLocked"))
			return TRUE;
	}

	const GasItem*pkGasItem = MMOHeavenManager::getSingleton().GetGasItemBySlot(SrcContainerID,SrcSlotID);
	if(pkGasItem == NULL)
	{
		return TRUE;
	}
	if(SrcSlotType == ESPType_HeavenGasEquip && (DestSlotType == ESPType_HeavenGasBag))
	{
		//Qi equipment unequip, target grid excluded
		if(DestSlotID != UCONST_GasBagSlotMaxNum)
			CMessageHandler::GetInstance().Send_UnequipGasItem(SrcSlotID,DestSlotID,DestContainerID);
		else
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasEquipNoMoveAimGas")),MsgInfoType_Table,TEXT("MMOGame.Heaven.GasEquipNoMoveAimGas"));
	}
	else if(DestSlotType == ESPType_HeavenGasEquip && (SrcSlotType == ESPType_HeavenGasBag))
	{
		//Qi equipment equip, target grid excluded
		if(SrcSlotID != UCONST_GasBagSlotMaxNum)
			CMessageHandler::GetInstance().Send_EquipGasItem(DestSlotID,SrcSlotID,SrcContainerID);
	}
	else if(DestSlotType == ESPType_HeavenGasEquip && (SrcSlotType == ESPType_HeavenGasBank))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasBagCanEquip")),MsgInfoType_Table,TEXT("MMOGame.Heaven.GasBagCanEquip"));
	}
	else if(DestSlotType == ESPType_HeavenGasBank && (SrcSlotType == ESPType_HeavenGasEquip))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasEquipCanMoveGasBag")),MsgInfoType_Table,TEXT("MMOGame.Heaven.GasEquipCanMoveGasBag"));
	}

	if(DestSlotType == ESPType_HeavenGasBank && (SrcSlotType == ESPType_HeavenGasBag || SrcSlotType == ESPType_HeavenGasBank ))
	{
		//huan
		if(SrcSlotID != UCONST_GasBagSlotMaxNum)
			CMessageHandler::GetInstance().Send_MoveGasItem(DestSlotID,DestContainerID,SrcSlotID,SrcContainerID);
	}
	else if((DestSlotType == ESPType_HeavenGasBag || DestSlotType == ESPType_HeavenGasBank)&& SrcSlotType == ESPType_HeavenGasBank)
	{
		//huan
		if(DestSlotID != UCONST_GasBagSlotMaxNum)
			CMessageHandler::GetInstance().Send_MoveGasItem(DestSlotID,DestContainerID,SrcSlotID,SrcContainerID);
	}

	//Inside bag
	if(DestSlotType == ESPType_HeavenGasBag && SrcSlotType == ESPType_HeavenGasBag)
	{
		//huan
		if(SrcSlotID == UCONST_GasBagSlotMaxNum || DestSlotID == UCONST_GasBagSlotMaxNum)
		{
			if (SrcSlotID == UCONST_GasBagSlotMaxNum )
				MMOHeavenManager::getSingleton().ClearTargetSlot();
			MMOHeavenManager::getSingleton().ChangeTargetGasItem(SrcSlotID);
		}
		else
			CMessageHandler::GetInstance().Send_MoveGasItem(DestSlotID,DestContainerID,SrcSlotID,SrcContainerID);
	}
	
	if (ESPType_HeavenGasBank == SrcSlotType || ESPType_HeavenGasBag == SrcSlotType || ESPType_HeavenGasEquip == SrcSlotType)
		return TRUE;
	return FALSE;
}
void UMMOUIHeavenGasMainPanel::OnStartDragGas(INT nContainerID,INT nSlotID,BOOL bSelected)
{
	if(nContainerID == sasbtEquip || nContainerID == sasbtStall)
	{
		for (INT n = 0; n < BagGasItemSlotList.Num(); ++n)
		{
			if(BagGasItemSlotList(n))
			{
				if( n < MMOHeavenManager::getSingleton().GetBagContainerSize())
				{
					BagGasItemSlotList(n)->SetBool("selected", bSelected );
				}
			}
		}
	}
	else if (nContainerID == sasbtBag)
	{
		for (INT n = 0; n < EquipGasItemSlotList.Num(); ++n)
		{
			if(EquipGasItemSlotList(n))
			{
				if( n < MMOHeavenManager::getSingleton().GetEquipContainerSize())
				{
					EquipGasItemSlotList(n)->SetBool("selected", bSelected );
				}
			}
		}

		for (INT n = 0; n < BankGasItemSlotList.Num(); ++n)
		{
			if(BankGasItemSlotList(n))
			{
				if( n < MMOHeavenManager::getSingleton().GetBankContainerSize())
				{
					BankGasItemSlotList(n)->SetBool("selected", bSelected );
				}
			}
		}

		if(GasTargetSlot)
			GasTargetSlot->SetBool("selected", bSelected );
	}
}
void UMMOUIHeavenGasMainPanel::GasItemDelete(INT ContainerID,INT SlotID)
{

}
void UMMOUIHeavenGasMainPanel::LockGasItem(INT nContainerID,INT SlotID)
{

}

void UMMOUIHeavenGasMainPanel::NativeOnShow(UBOOL v)
{

}

void UMMOUIHeavenGasMainPanel::InitContainer(UMMOGFXPlayer*GFXPlayer)
{
	if(GFXPlayer == 0)
		return;
	//eventInitHeavenGasMain();
	UGFxObject* mc = NULL;
	for (INT n = 0; n < EquipGasItemSlotList.Num(); ++n)
	{
		if(EquipGasItemSlotList(n))
		{
			if( n+1 >  MMOHeavenManager::getSingleton().GetEquipContainerSize())
			{
				EquipGasItemSlotList(n)->SetBool("isSlotLocked", TRUE );
			}
			GFXPlayer->eventCallSetItemData( EquipGasItemSlotList(n) , 0 , TEXT("") , -1, ESPType_HeavenGasEquip, TEXT("") , ITEMCOLOR_INVALID, sasbtEquip, n );
			UpdateContainerSlot(GFXPlayer,EquipGasItemSlotList(n),ESPType_HeavenGasEquip,sasbtEquip,n);
			
		}
	}

	for (INT n = 0; n < BagGasItemSlotList.Num(); ++n)
	{
		if(BagGasItemSlotList(n))
		{
			if( n+1 >  MMOHeavenManager::getSingleton().GetBagContainerSize())
			{
				BagGasItemSlotList(n)->SetBool("isSlotLocked", TRUE );
			}
			GFXPlayer->eventCallSetItemData( BagGasItemSlotList(n) , 0 , TEXT("") , -1, ESPType_HeavenGasBag, TEXT("") , ITEMCOLOR_INVALID, sasbtBag, n );
			UpdateContainerSlot(GFXPlayer,BagGasItemSlotList(n),ESPType_HeavenGasBag,sasbtBag,n);
			
		}
	}
	for (INT n = 0; n < BankGasItemSlotList.Num(); ++n)
	{
		if(BankGasItemSlotList(n))
		{
			if( n+1 >  MMOHeavenManager::getSingleton().GetBankContainerSize())
			{
				BankGasItemSlotList(n)->SetBool("isSlotLocked", TRUE );
			}
			GFXPlayer->eventCallSetItemData( BankGasItemSlotList(n) , 0 , TEXT("") , -1, ESPType_HeavenGasBank, TEXT("") , ITEMCOLOR_INVALID, sasbtStall, n );
			UpdateContainerSlot(GFXPlayer,BankGasItemSlotList(n),ESPType_HeavenGasBank,sasbtStall,n);
			
		}
	}
	
	if(GasTargetSlot)
	{
		GFXPlayer->eventCallSetItemData( GasTargetSlot , 0 , TEXT("") , -1, ESPType_HeavenGasBag, TEXT("") , ITEMCOLOR_INVALID, sasbtBag, UCONST_GasBagSlotMaxNum );
		eventUpdateSetGasIconState(sasbtBag,UCONST_GasBagSlotMaxNum,false,0,0,TEXT(""),TEXT(""));
	}
}
void UMMOUIHeavenGasMainPanel::UpdateSlotState(UMMOGFXPlayer*GFXPlayer,INT nContainerID,INT nSlotID)
{
	if(GFXPlayer == 0)
		return;
	if (nContainerID == sasbtEquip)
	{
		if(nSlotID < EquipGasItemSlotList.Num() && nSlotID <=  MMOHeavenManager::getSingleton().GetEquipContainerSize())
		{
			for (INT n = 0; n <= nSlotID;++n)
			{
				if(EquipGasItemSlotList(n))
					EquipGasItemSlotList(n)->SetBool("isSlotLocked", FALSE );
			}
			UpdateEquipSlotStateShow(GFXPlayer,nContainerID,nSlotID);
		}
	}
	else if (nContainerID == sasbtBag)
	{
		if(nSlotID < BagGasItemSlotList.Num() && nSlotID <=  MMOHeavenManager::getSingleton().GetBagContainerSize())
		{
			for (INT n = 0; n <= nSlotID;++n)
			{
				if(BagGasItemSlotList(n))
					BagGasItemSlotList(n)->SetBool("isSlotLocked", FALSE );
			}
		}
	}
	else if (nContainerID == sasbtStall)
	{
		if(nSlotID < BankGasItemSlotList.Num() && nSlotID <=  MMOHeavenManager::getSingleton().GetBankContainerSize())
		{
			for (INT n = 0; n <= nSlotID;++n)
			{
				if(BankGasItemSlotList(n))
					BankGasItemSlotList(n)->SetBool("isSlotLocked", FALSE );
			}
		}
	}
}
void UMMOUIHeavenGasMainPanel::UpdateGasEquipPower()
{
	INT nPower = MMOHeavenManager::getSingleton().GetEquipGasPower();
	eventUpdateEquipGasPower(nPower);
}
void UMMOUIHeavenGasMainPanel::UpdateSlotItem(UMMOGFXPlayer*GFXPlayer,INT nContainerID,INT nSlotID)
{
	if(GFXPlayer == 0)
		return;
	if (nContainerID == sasbtEquip)
	{
		if(nSlotID < EquipGasItemSlotList.Num())
		{
			UpdateContainerSlot(GFXPlayer,EquipGasItemSlotList(nSlotID),ESPType_HeavenGasEquip,nContainerID,nSlotID);
			UpdateEquipSlotStateShow(GFXPlayer,sasbtEquip,nSlotID);
			UpdateGasEquipPower();
		}
	}
	else if (nContainerID == sasbtBag)
	{
		if(nSlotID < BagGasItemSlotList.Num())
			UpdateContainerSlot(GFXPlayer,BagGasItemSlotList(nSlotID),ESPType_HeavenGasBag,nContainerID,nSlotID);
	}
	else if (nContainerID == sasbtStall)
	{
		if(nSlotID < BankGasItemSlotList.Num())
			UpdateContainerSlot(GFXPlayer,BankGasItemSlotList(nSlotID),ESPType_HeavenGasBank,nContainerID,nSlotID);
	}
}
void UMMOUIHeavenGasMainPanel::UpdateContainerSlot(UMMOGFXPlayer*GFXPlayer,UGFxObject* mc,ESlotPanelType panelType,INT nContainerID,INT nSlotID)
{
	if(GFXPlayer == 0 || mc == 0)
		return;

	const GasItem* pkItem = MMOHeavenManager::getSingleton().GetGasItemBySlot(nContainerID,nSlotID);
	bool bInit = true;
	if(pkItem)
	{
		const HeavenGasItemConfig* pkTemp =  MMOHeavenManager::getSingleton().GetGasTemplate(pkItem->nTempID);
		if(pkTemp)
		{
			bInit = false;
			
			FString strSwf = FString::Printf(TEXT("../%s"),*(pkTemp->Icon0));
			eventUpdateSetGasIconState(nContainerID,nSlotID,true,pkTemp->nQuality,10/*pkTemp->nLevel*/,strSwf,pkTemp->Icon1);
			mc->SetInt(TEXT("itemIndex"),0);
		}
	}
	if(bInit)
	{
		eventUpdateSetGasIconState(nContainerID,nSlotID,false,0,0,TEXT(""),TEXT(""));
		mc->SetInt(TEXT("itemIndex"),-1);
	}
}
void UMMOUIHeavenGasMainPanel::UpdateEquipSlotStateShow(UMMOGFXPlayer*GFXPlayer,INT nContainerID,INT nSlotID)
{
	if(GFXPlayer == NULL)
		return;
	INT nSize = MMOHeavenManager::getSingleton().GetEquipContainerSize();
	if(nSlotID == -1)
	{
		UGFxObject* pkArrData = GFXPlayer->CreateArray();
		if(pkArrData == NULL)
			return;
		for (INT n = 0; n < UCONST_GasEquipSlotMaxNum;++n)
		{
			UGFxObject* pkData = GFXPlayer->CreateObject(TEXT("UI_SkillTree.SkillTreeAS3.HeavenGasEquipData"));
			if(pkData == NULL)
				return;
			FString hint0;
			FString hint1;
			pkData->SetInt("nIndex",n);
			//open
			if(n < nSize)
			{
				const GasItem*pkItem = MMOHeavenManager::getSingleton().GetGasItemBySlot(sasbtEquip,n);
				if(pkItem != NULL)
				{
					const HeavenGasItemConfig* pkConfig = MMOHeavenManager::getSingleton().GetGasTemplate(pkItem->nTempID);
					if(pkConfig != NULL)
					{
						hint0 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkConfig->nQuality));
						hint0 += pkConfig->name;
						hint0 += UITextParser::HTMLEndFont();
						hint1 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkConfig->nQuality));
						hint1 += FString::Printf(TEXT("Lv %d"),pkConfig->nLevel);
						hint1 += UITextParser::HTMLEndFont();
					}
				}
				else
				{
					hint0 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)1));
					hint0 += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.EquipGasUnLock"));
					hint0 += UITextParser::HTMLEndFont();
					hint1 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
					hint1 += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.EquipGasCanEquip"));
					hint1 += UITextParser::HTMLEndFont();
				}
			}
			else
			{
				const SlotOpenTaxData* pkSlot = MMOHeavenManager::getSingleton().GetSlotEquipTax(n);
				if(pkSlot != NULL)
				{
					hint0 = UITextParser::HTMLBeginFont(TEXT("#FF0000"));
					hint0 += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HeavenHowLevel")),pkSlot->nHeavenLevel);
					hint0 += UITextParser::HTMLEndFont();
					hint1 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
					if (pkSlot->nMoney > 0)
						hint1 += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.EquipGasGoldUnLock"));
					else
						hint1 += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.EquipGasFreeUnlock"));
					hint1 += UITextParser::HTMLEndFont();
				}
			}
			pkData->SetString("sLabel0",hint0);
			pkData->SetString("sLabel1",hint1);
			pkArrData->SetElementObject(n,pkData);
		}
		eventUpdateEquipGasSlot(pkArrData);
	}
	else
	{
		UGFxObject* pkData = GFXPlayer->CreateObject(TEXT("UI_SkillTree.SkillTreeAS3.HeavenGasEquipData"));
		if(pkData == NULL)
			return;
		FString hint0;
		FString hint1;
		pkData->SetInt("nIndex",nSlotID);
		const GasItem*pkItem = MMOHeavenManager::getSingleton().GetGasItemBySlot(sasbtEquip,nSlotID);
		if(pkItem != NULL)
		{
			const HeavenGasItemConfig* pkConfig = MMOHeavenManager::getSingleton().GetGasTemplate(pkItem->nTempID);
			if(pkConfig != NULL)
			{
				hint0 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkConfig->nQuality));
				hint0 += pkConfig->name;
				hint0 += UITextParser::HTMLEndFont();
				hint1 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkConfig->nQuality));
				hint1 += FString::Printf(TEXT("Lv %d"),pkConfig->nLevel);
				hint1 += UITextParser::HTMLEndFont();
			}
		}
		else
		{
			hint0 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)1));
			hint0 +=  MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.EquipGasUnLock"));
			hint0 += UITextParser::HTMLEndFont();
			hint1 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
			hint1 += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.EquipGasCanEquip"));
			hint1 += UITextParser::HTMLEndFont();
		}
		pkData->SetString("sLabel0",hint0);
		pkData->SetString("sLabel1",hint1);
		eventUpdateEquipGasSingleSlot(pkData);
		//pkArrData->SetElementObject(nSlotID,pkData);
	}
}
void UMMOUIHeavenGasMainPanel::ExpandGasBagOk(class UGFxObject* mc)
{
	INT containerId = mc->GetInt(TEXT("iteminfoid"));
	INT count = mc->GetInt(TEXT("itemdataid"));
	CMessageHandler::GetInstance().Send_BuyGasSlot( containerId , count );
}
void UMMOUIHeavenGasMainPanel::ExpandGasBankOk(class UGFxObject* mc)
{
	INT containerId = mc->GetInt(TEXT("iteminfoid"));
	INT count = mc->GetInt(TEXT("itemdataid"));
	CMessageHandler::GetInstance().Send_BuyGasSlot( containerId , count );
}
void UMMOUIHeavenGasMainPanel::ExpandGasEquipOk(class UGFxObject* mc)
{
	INT containerId = mc->GetInt(TEXT("iteminfoid"));
	INT count = mc->GetInt(TEXT("itemdataid"));
	CMessageHandler::GetInstance().Send_BuyGasSlot( containerId , count );
}
void UMMOUIHeavenGasMainPanel::OnClickBtnPractiseGas(INT nBtnIndex)
{
	CMessageHandler::GetInstance().Send_NormalPractiseGas(nBtnIndex);
}
void UMMOUIHeavenGasMainPanel::UpdateHeavenGasMainPanel()
{

}
void UMMOUIHeavenGasMainPanel::UpdateTargetState(UMMOGFXPlayer*GFXPlayer)
{
	if(GFXPlayer == NULL)
		return;
	for (INT n = 0; n < BagGasItemSlotList.Num();++n)
	{
		if(BagGasItemSlotList(n))
		{
			if(!BagGasItemSlotList(n)->GetBool("isSlotLocked"))
			{
				BagGasItemSlotList(n)->SetBool("bDisableDrag",TRUE);
			}
		}
	}
	GasTargetSlot =  GetObject(TEXT("GasBagTarget"));
	if(GasTargetSlot == NULL)
		return;
	const GasSlot* pkSlot = MMOHeavenManager::getSingleton().GetTargetSlot();
	if(pkSlot==0 || pkSlot->nTagID < 0)
	{
		eventUpdateSetGasIconState(sasbtBag,UCONST_GasBagSlotMaxNum,false,0,0,TEXT(""),TEXT(""));
		GasTargetSlot->SetInt(TEXT("itemIndex"),-1);
		//GFXPlayer->eventCallSetItemData( GasTargetSlot , 0 , TEXT("") , -1, ESPType_HeavenGasBag, TEXT("") , ITEMCOLOR_INVALID, sasbtBag, UCONST_GasBagSlotMaxNum );
		return;
	}

	INT nTargetID = pkSlot->nTagID;
	if (nTargetID >= 0&&nTargetID < BagGasItemSlotList.Num())
	{
		if(BagGasItemSlotList(nTargetID))
		{
			if(!BagGasItemSlotList(nTargetID)->GetBool("isSlotLocked"))
				BagGasItemSlotList(nTargetID)->SetBool("bDisableDrag",FALSE);
			const GasItem* pkItem = MMOHeavenManager::getSingleton().GetGasItemBySlot(sasbtBag,nTargetID);
			if(pkItem)
			{
				const HeavenGasItemConfig* pkTemp =  MMOHeavenManager::getSingleton().GetGasTemplate(pkItem->nTempID);
				if(pkTemp)
				{
					eventUpdateSetSelectedIconTarget(sasbtBag,nTargetID);
					
					FString strSwf = FString::Printf(TEXT("../%s"),*(pkTemp->Icon0));
					//GFXPlayer->eventCallSetItemData( GasTargetSlot , 0 , TEXT("") , 0, ESPType_HeavenGasBag, TEXT("") , ITEMCOLOR_INVALID, sasbtBag, UCONST_GasBagSlotMaxNum );
					eventUpdateSetGasIconState(sasbtBag,UCONST_GasBagSlotMaxNum,true,pkTemp->nQuality,10,strSwf,pkTemp->Icon1);
					GasTargetSlot->SetInt(TEXT("itemIndex"),0);
				}
			}
		}
	}
}