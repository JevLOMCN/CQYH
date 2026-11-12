#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "ClientActionConditionCheck.h"
#include <vector>
#include "MMOUtil.h"
#include "EquipDecomposeManager.h"
#include "MallManager.h"
#include "MallController.h"
#include "MallManager.h"
#include "MMOGuideManager.h"
#include "MMOEquipManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Bag)

using namespace std;

//======================================================================================================================
void UMMOGFXPlayer_Bag::ASStartDrag( UGFxObject* srcItemSlot, BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID)
{
	Super::ASStartDrag( srcItemSlot, srcSlotType, srcContainerID, srcSlotID );

	//
	UMMOUISkillBar* skillbar = Cast<UMMOUISkillBar>( MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar ) );
	if( srcItemSlot == NULL || skillbar == NULL ){
		return;
	}

	skillbar->HintForDraggingItem( ESPType_Bag, srcSlotID );
}

//======================================================================================================================
void UMMOGFXPlayer_Bag::ASLockItem()
{

}

void UMMOGFXPlayer_Bag::ASMoneyExchange()
{
	//MallController::getSingleton().PopupGoodsBuyPanel(2);
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_MoneyExchange , TRUE , TEXT("") );
}

void UMMOGFXPlayer_Bag::ASSplitItem()
{
	if(_Bag)
		_Bag->eventClickSplit();
}

void UMMOGFXPlayer_Bag::ASMoneyExchangeOk()
{
	if( _MoneyExchange )
	{
		_MoneyExchange->ExchangeOk();
	}
}

void UMMOGFXPlayer_Bag::ASMoneyExchangeInputChange()
{
	if( _MoneyExchange )
	{
		_MoneyExchange->InputChange();
	}
}

//--------------------------------------------
void UMMOGFXPlayer_Bag::ASEvaluteItem()
{
	if( _Bag ){
		_Bag->eventClickIdentify();
	}
}

void UMMOGFXPlayer_Bag::ASClickMoneyChain( UBOOL selected )
{
	CMessageHandler::GetInstance().Send_ChainGameMoney( selected );
	MMOFacade::getSingleton().SetChainPay(selected);
	if(_Bag)
	{
		_Bag->SetChainPayText();
	}
}

//--------------------------------------------
void UMMOGFXPlayer_Bag::ASSignatureItem()
{
	if( _Bag ){
		//_Bag->eventClickSignature();
		_Bag->eventClickDecompose();
		//if(EquipDecomposeManager::getSingleton().IsDecomposeEquip())
		//{
		//	EquipDecomposeManager::getSingleton().EndDecompose();
		//}
		//else
		//{
		//	EquipDecomposeManager::getSingleton().BeginDecompose();
		//}

		MMOGameEvent_Guide evtGuide(EGUET_ClickDecompEquip);
		MMOUI::PostGameEvent(&evtGuide);
	}
}

bool SortItemByTemplateID(const ItemInfo* lhs, const ItemInfo* rhs)
{
	return lhs->m_nTemplateID < rhs->m_nTemplateID;
}

void UMMOGFXPlayer_Bag::SetItemSlotData( class UGFxObject* mc, class UItem* tItem, BYTE SlotType, BYTE ContainerID , INT SlotID , BYTE inISS )
{
	UMMOGFXPlayer::SetItemSlotData( mc, tItem, SlotType, ContainerID, SlotID,inISS);

	//finishing blow
	if(!mc)
	{
		return;
	}

	UGFxObject* abnormity = mc->GetObject(TEXT("abnormity_mc"));
	UGFxObject* newHint = mc->GetObject("newHint");
	UGFxObject* colorFrame = mc->GetObject("colorFrame");

	if(!newHint)
		return;

	if(tItem)
	{
		FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem( ContainerID , SlotID );
		if(itemInfo)
		{
			//UBOOL showNewHint = ( ( itemInfo->NeedHintMode | ItemSlotHint_New ) == itemInfo->NeedHintMode ) && ( ( itemInfo->NeedHintMode | ItemSlotHint_PowerEquip ) != itemInfo->NeedHintMode );
			//newHint->SetVisible( showNewHint );
		}

		if( tItem->ItemType == TEXT("ITEMTYPE_EQUIP") && itemInfo->m_nCurDur == 0 )
		{
			if(colorFrame)
			{
				colorFrame->SetVisible(TRUE);
			}
		}
		else
		{
			if(colorFrame)
			{
				colorFrame->SetVisible(FALSE);
			}
		}


		//changed here so weapons in the bag that do not suit this class are marked with x; since this icon is the same as the locked-item icon when trading or stalling, there is ambiguity. But currently this is the only icon.
		if( tItem->ItemType == TEXT("ITEMTYPE_EQUIP") && !mc->GetBool( TEXT("isSlotUnusable") ) )
		{
			const UEquip *pEquip = ConstCast<UEquip>( tItem );
			AMMOPlayer* pl=  MMOFacade::getSingleton().GetMainPlayer();
			MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();

			if(abnormity)
			{
				if( itemInfo->m_byIdentified == IDENTIFY_TYPE_NO )
				{
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotUnidentify") );
				}

				UBOOL equipError = mmoEquipMgr.CheckItemUseableForMe( tItem );
				switch( equipError )
				{
				case ITEM_Use_Err_None:
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
					break;

				case ITEM_Use_Err_LowLevel:
				case ITEM_Use_Err_LowPhyAtk:
				case ITEM_Use_Err_LowMagAtk:
				case ITEM_Use_Err_LowTaoAtk:
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotLock") );
					break;

				case ITEM_Use_Err_Undefine:
				case ITEM_Use_Err_RequireClass:
				case ITEM_Use_Err_Gender:
				default:
					abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.SlotAbnormity.slotImpossible") );
					break;
				}
			}

		}
		else
		{
			if(abnormity)
				abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
		}
	}
	else
	{
		if(abnormity)
		{
			abnormity->SetString( TEXT("source") , TEXT("img://MMO_UIIcon.icon.blank") );
		}
		//newHint->SetVisible(FALSE);
		if(colorFrame)
		{
			colorFrame->SetVisible(FALSE);
		}
	}
}

void UMMOGFXPlayer_Bag::ASSortBag()
{
#if 1
	ClientActionCondition_ItemForbiddenInPanelOpen checker;

	FClientActionConditionContex contex;
	contex.Item = NULL;
	contex.ItemTemplate = NULL;
	contex.UsefulInt = E_CONSLOT_DEFAULT;

	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	CMessageHandler::GetInstance().Send_SortBag(this->_Bag->GetBagEnum());
	MMOFacade::getSingleton().StartSortTimer( LSM_SortBag );
#else
	//FClientActionConditionContex contex(ECACCT_UseItem_CheckStallState);
	//if(!CheckClientActionCondition(contex))
	//{
	//	return;
	//}

	ClientActionCondition_ItemForbiddenInPanelOpen checker;

	FClientActionConditionContex contex;
	contex.Item = NULL;
	contex.ItemTemplate = NULL;

	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	if(_Bag)
	{
		ECONTAINER_SLOT curCont = (ECONTAINER_SLOT)_Bag->GetBagEnum();

		ItemsMap& container = MMOFacade::getSingleton().GetContainer(curCont);

		typedef std::vector<ItemInfo*, M_UE3_STL_ALLOC(ItemInfo*) > ItemsList;

		ItemsList sortArr;
		for (ItemsMap::iterator i = container.begin(); i != container.end(); ++i)
		{
			ItemInfo* newItem = new ItemInfo();
			appMemcpy(newItem, i->second, sizeof(ItemInfo));
			sortArr.push_back(newItem);
		}

		std::sort(sortArr.begin(), sortArr.end(), SortItemByTemplateID);
		
		for (size_t j = 0; j < sortArr.size(); ++j)
		{
			ItemInfo* info = sortArr[j];
			
			if(j != info->m_wItemSlot)
			{
				CMessageHandler::GetInstance().Send_MoveItem(curCont, j, curCont, info->m_wItemSlot);
			}

			delete info;
		}
	}
#endif
}

void UMMOGFXPlayer_Bag::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Bag::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Bag);

}

void UMMOGFXPlayer_Bag::UnInit()
{
	Super::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);

	MMOUI::GetUIManager()->SetDeleteItemDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetGetOpenBagDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetTurntoPageDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetLockItemDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetUpdateBagCapSlotDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetHintNewItemLabelDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetOpenBagDelegate(nullDelgate);

}
