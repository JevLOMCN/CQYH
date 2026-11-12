#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"


IMPLEMENT_CLASS(UMMOUIWeaponExpPanel)



UBOOL UMMOUIWeaponExpPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	if(!WeaponSlot)
	{
		return FALSE;
	}

	switch( SlotType )
	{
	case ESPType_Bag:
		{	

			AddWeaponItem( ContainerID , SlotID );

			return TRUE;
		}
		break;
	case ESPType_WeaponExp:
		{
			

			RemoveWeaponItem( ContainerID , SlotID );

			return TRUE;
		}

		break;
	}


	return FALSE;

}

void UMMOUIWeaponExpPanel::NativeInitOnce()
{
	UGFxObject* obj = GetObject( TEXT("confirmButton") );
	if(obj)
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponExp.GetWeaponExp") ) );

	obj = GetObject( TEXT("cancelButton") );
	if(obj)
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponExp.Cancel") ) );

	for( INT i = 0 ; i < BottleIcon.Num() ; i++ )
	{
		
		if(BottleIcon(i))
		{
			UItem* item = MMOFacade::getSingleton().FindItemTemplate(BottleItemTempId(i));
			GFXPlayer->SetIconShowByItem( BottleIcon(i) , item );
		}
	}
}

void UMMOUIWeaponExpPanel::AddWeaponItem( INT ContainerID , INT SlotID )
{
	FClientItemInfo *pInfo = MMOFacade::getSingleton().FindItem( ContainerID, SlotID );
	if( pInfo == NULL )
	{
		return ;
	}
	UEquip* pEquip = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( pInfo->m_nTemplateID ) );
	if( pEquip == NULL )
	{
		return ;
	}

	if(pEquip->GetEquipAppearenceTemplate() &&
		(pEquip->GetEquipAppearenceTemplate()->EquipType != EET_Normal_MainHand))
	{
		return ;
	}

	INT LastContainerID = WeaponSlot->GetInt("containerID");
	INT LastSlotID = WeaponSlot->GetInt("slotID");

	if( LastContainerID == E_CONSLOT_DEFAULT /*E_CONSLOT_EQUIPMENT*/ )
	{
		//First remove the previously drawn weapon
		RemoveWeaponItem(LastContainerID,LastSlotID);
	}

	GFXPlayer->SetItemSlotData( WeaponSlot, pEquip, ESPType_WeaponExp, ContainerID, SlotID );
	//if(WeaponExpText)
	//{
	//	
	//	WeaponExpText->SetText( appItoa(pInfo->m_byProficiency) );
	//}

	pInfo->IsOnWeaponExp = true;

	MMOUI::GetUIManager()->delegateLockItem( ContainerID , SlotID , TRUE );
}

void UMMOUIWeaponExpPanel::RemoveWeaponItem( INT ContainerID , INT SlotID )
{
	FClientItemInfo *pInfo = MMOFacade::getSingleton().FindItem( ContainerID, SlotID );

	if(pInfo)
	{
		pInfo->IsOnWeaponExp = false;
	}

	GFXPlayer->SetItemSlotData( WeaponSlot , NULL , ESPType_WeaponExp );

	if(WeaponExpText)
	{
		WeaponExpText->SetText( TEXT("0") );
	}

	if( ContainerID == E_CONSLOT_DEFAULT /*E_CONSLOT_EQUIPMENT*/ )
	{
		MMOUI::GetUIManager()->delegateLockItem( ContainerID , SlotID , FALSE );
	}
}

//------------------------------------------------------------------------------------------------
UBOOL UMMOUIWeaponExpPanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	if( (SrcSlotType == ESPType_Bag) && (DestSlotType == ESPType_WeaponExp) )
	{
		
		AddWeaponItem( SrcContainerID , SrcSlotID );

		return TRUE;
	}
	else if((SrcSlotType == ESPType_WeaponExp) && (DestSlotType == ESPType_Bag)  )
	{
		RemoveWeaponItem( SrcContainerID , SrcSlotID );

		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------------------------
UBOOL UMMOUIWeaponExpPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
    return FALSE; 
} 

 UBOOL UMMOUIWeaponExpPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

//------------------------------------------------------------------------------------------------
void UMMOUIWeaponExpPanel::GetWeaponExp()
{
	//send
}

//------------------------------------------------------------------------------------------------
void UMMOUIWeaponExpPanel::UpdateBottleCount()
{
	for ( INT i = 0 ; i < Bottle.Num() ; i++ )
	{
		INT itemCount = MMOFacade::getSingleton().GetItemCountInBag( BottleItemTempId(i) );
		if( BottleHaveText(i) )
		{
			BottleHaveText(i)->SetText( appItoa(itemCount) );
		}
		if( BottleNeedText(i) )
		{
			BottleNeedText(i)->SetText( FString::Printf( TEXT("/%d") , BottleNeedNum(i) ) );
		}
	}
}

void UMMOUIWeaponExpPanel::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

	eventOnShowWeaponExp(show);
}