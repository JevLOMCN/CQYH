#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Warehouse)

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Warehouse::ASSortWarehouse()
{
	CMessageHandler::GetInstance().Send_SortBag(E_CONSLOT_BANK);
	MMOFacade::getSingleton().StartSortTimer( LSM_SortWarehouse );
}


void UMMOGFXPlayer_Warehouse::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Warehouse::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Warehouse);
}

void UMMOGFXPlayer_Warehouse::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_Warehouse::SetItemSlotData( class UGFxObject* mc, class UItem* tItem, BYTE SlotType, BYTE ContainerID , INT SlotID , BYTE inISS )
{
	UMMOGFXPlayer::SetItemSlotData( mc, tItem, SlotType, ContainerID, SlotID,inISS);

	//finishing blow
	if(!mc)
	{
		return;
	}

	UGFxObject* abnormity = mc->GetObject(TEXT("abnormity_mc"));

	if(tItem)
	{
		FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem( ContainerID , SlotID );


		//changed here so weapons in the bag that do not suit this class are marked with x; since this icon is the same as the locked-item icon when trading or stalling, there is ambiguity. But currently this is the only icon.
		if( tItem->ItemType == TEXT("ITEMTYPE_EQUIP") && !mc->GetBool( TEXT("isSlotUnusable") ) )
		{
			AMMOPlayer* pl=  MMOFacade::getSingleton().GetMainPlayer();


			if(abnormity)
			{
				if( !MMO::CanBeUsedByPlayerClass( EPlayerClass( pl->GetClassId() ) , tItem->RequireClass ) )
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotImpossible") );
				}
				else if( pl->GetActorLevel() < tItem->RequireLevel )
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotLock") );
				}
				else if( !itemInfo->m_byIdentified )
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotUnidentify") );
				}
				else
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
				}
			}

		}
	}
	else
	{
		if(abnormity)
		{
			abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
		}
		
	}
}