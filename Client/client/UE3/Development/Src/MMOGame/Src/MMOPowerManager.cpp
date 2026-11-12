#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOPowerManager.h"
#include "CommonGameEvents.h"
#include "MMOEquipManager.h"
#include "MMOHeavenManager.h"
#include "EquipSlotManager.h"

#include "MMOPowerPart.h"

#define POWER_ADD_OF_EQUIP_EMPTY_SOUL_SLOT (10)

MMOPowerManager::MMOPowerManager()
{
	Init();
}

MMOPowerManager::~MMOPowerManager()
{
	UnInit();
}

void MMOPowerManager::Init()
{
	_powerPartMap.Set( PowerPart_HeavenPow , new MMOPowerPart_Heaven() );
	_powerPartMap.Set( PowerPart_SuitPow , new MMOPowerPart_Suit() );
	_powerPartMap.Set( PowerPart_TitlePow , new MMOPowerPart_Title() );
	_powerPartMap.Set( PowerPart_EquipSlotEnhancePow , new MMOPowerPart_EquipSlotEnhance() );
	_powerPartMap.Set( PowerPart_AllEquippedPow , new MMOPowerPart_EquippedAll() );
	_powerPartMap.Set( PowerPart_CharacterBasePow , new MMOPowerPart_CharacterBase() );
	_powerPartMap.Set( PowerPart_SkillPow , new MMOPowerPart_Skill() );
}

void MMOPowerManager::UnInit()
{
	PowerPartMap::TIterator itor(_powerPartMap);
	for ( ; itor ; ++itor )
	{
		MMOPowerPart* part = itor.Value();
		SAFE_DELETE(part);
	}
	_powerPartMap.Empty();
}

void MMOPowerManager::Reset()
{
	PowerPartMap::TIterator itor(_powerPartMap);
	for ( ; itor ; ++itor )
	{
		MMOPowerPart* part = itor.Value();
		if(part)
		{
			part->Reset();
		}
	}
}

void MMOPowerManager::UpdatePartPower( EPowerPart type )
{
	MMOPowerPart* powerPart = *_powerPartMap.Find( type );
	if(powerPart)
	{
		powerPart->RecountPower();

		//_NotifyUpdateUIPower();
	}
}


void MMOPowerManager::UpdateEquipPower( FClientItemInfo* item )
{
	//equipment combat power is special, since it must be computed and saved in the equipment
	if(!item)
	{
		return;
	}
	//only equipment can be operated
	UEquip* equipTemp = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( item->m_nTemplateID ) );

	if(!equipTemp)
	{
		return;
	}
	//weapon base combat power
	INT basePower = equipTemp->FixAttrbutes.BasePow;

	//Luck value
	//basePower *= ( 1 + (FLOAT)item->m_byLuck * 0.05 );
	if( item->m_byLuck > 0 ){
		basePower = basePower * ( 20 + item->m_byLuck ) / 20;
	}

	//random attribute combat power
	INT randomPower = 0;
	ItemRandomAttr* randomAttr[MAX_RANDOM_ATTR_COUNT] = {};

	for ( INT i = 0 ; i < MAX_RANDOM_ATTR_COUNT ; i++ )
	{
		const ItemRandomAttr* randomAttr = MMOFacade::getSingleton().GetRandomAttributById( item->m_wRandomAttrID[i] );
		if( randomAttr )
		{
			randomPower += randomAttr->power;
		}		
	}

	//Spirit fusion
	INT soulPower = 0;
	for ( INT i = 0 ; i < MAX_SOCKET_COUNT ; i++ )
	{
		if( item->GetSocketType( i ) != SOCKET_NONE )
		{
			soulPower += POWER_ADD_OF_EQUIP_EMPTY_SOUL_SLOT;
		}

		const SoulInfo *pSoulInfo = MMOEquipManager::getSingleton().FindSoulInfoByItemTemplateID( item->m_nSoulID[i] );
		if( pSoulInfo == NULL )
		{
			continue;
		}
		
		const ItemRandomAttr* soulAttr = MMOFacade::getSingleton().GetRandomAttributById( pSoulInfo->_AttrID );
		if( soulAttr )
		{
			soulPower += soulAttr->power;
		}		
	}

	//equipment rune combat power
	INT runePower = 0;
	UBOOL runeChange = FALSE;
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i )
	{
		UBOOL actived = FALSE;
		UMMOAbility* skill = MMOEquipManager::getSingleton().GetWeaponRuneSkillBySlotIdx( i , item , actived );
		if( actived && skill )
		{
			runePower += skill->Pow;
			runeChange = TRUE;
		}
	}
	
	////weapon level
	INT upgradePower = 0;
	//if( item->m_byLevel > 0 ){
	//	const EquipLvUpCfg *pLevelUpData = MMOEquipManager::getSingleton().FindEquipLevelUpData( item->m_byLevel - 1 );
	//	upgradePower += pLevelUpData ? pLevelUpData->_PowerAdd : 0;
	//}
	//upgradePower *= ( 1 + (FLOAT)item->m_byLevel * 0.05 );
	if( item->m_byLevelupPhyPoint > 0 ){
		upgradePower += item->m_byLevelupPhyPoint * EQUIP_LV_UP_POWER_FACTER;
	}
	if( item->m_byLevelupMagPoint > 0 ){
		upgradePower += item->m_byLevelupMagPoint * EQUIP_LV_UP_POWER_FACTER;
	}
	if( item->m_byLevelupTaoPoint > 0 ){
		upgradePower += item->m_byLevelupTaoPoint * EQUIP_LV_UP_POWER_FACTER;
	}
	

	item->Pow = basePower + randomPower + soulPower + runePower + upgradePower;

	if( item->m_byConSlot == E_CONSLOT_EQUIPPED )
	{
		UpdatePartPower( PowerPart_AllEquippedPow );
		UpdatePartPower( PowerPart_SkillPow );
	}
	
};

void MMOPowerManager::_NotifyUpdateUIPower()
{
	UMMOUICharacter* panel = Cast<UMMOUICharacter>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_Character ) );
	if(panel)
	{
		panel->UpdateBattlePoint();
	}
}


INT MMOPowerManager::GetPower()
{
	INT totalPow = 0;
	PowerPartMap::TIterator itor(_powerPartMap);
	for ( ; itor ; ++itor )
	{
		MMOPowerPart* part = itor.Value();
		if(part)
		{
			totalPow += part->GetPow();
		}
	}
	return totalPow;
}