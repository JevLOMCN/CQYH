#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOPowerPart.h"
#include "CommonGameEvents.h"
#include "MMOEquipManager.h"
#include "MMOHeavenManager.h"
#include "EquipSlotManager.h"
#include "MMOSkillManager.h"


MMOPowerPart::MMOPowerPart()
{
	_partPower = 0;
	_initial = FALSE;
}

void MMOPowerPart::RecountPower()
{
	_initial = TRUE;
}

INT MMOPowerPart::GetPow()
{ 
	if( !_initial )
	{
		RecountPower();
	}
	return _partPower; 
}


void MMOPowerPart_Heaven::RecountPower()
{
	_partPower = 0 ;

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

	if( player )
	{
		INT lv = player->GetActorLevel();
		if ( lv >= HEAVEN_OPEN_LEVEL )
		{
			_partPower = MMOFacade::getSingleton().GetSpiritPow(player->ActorAttributes->heavenAttributes.heavenLevel);
		}
		
	}
	MMOPowerPart::RecountPower();
}

void MMOPowerPart_CharacterBase::RecountPower()
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

	if( player )
	{
		INT lv = player->GetActorLevel();
		_partPower = MMOFacade::getSingleton().GetLevelPow(lv);
	}
	MMOPowerPart::RecountPower();
}

void MMOPowerPart_Title::RecountPower()
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !player || !player->ActorAttributes )
	{
		return;
	}
	//not configured currently
	_partPower = 0;
	TitleData* title = MMOFacade::getSingleton().GetTitleData( player->ActorAttributes->TitleID );
	if(title)
	{
		for ( INT i = 0 ; i < 5 ; i++ )
		{
			const ItemRandomAttr* titleAttr = MMOFacade::getSingleton().GetRandomAttributById( title->AttrID[i] );
			if( titleAttr )
			{
				_partPower = titleAttr->power;
			}
		}
		
	}

	MMOPowerPart::RecountPower();
}

void MMOPowerPart_Suit::RecountPower()
{
	_partPower = 0; 

	TArray<INT> hasAddedSuitList;

	MMOEquipManager& equipMgr = MMOEquipManager::getSingleton();
	MMOFacade& facade = MMOFacade::getSingleton();

	AMMOPlayer* player = facade.GetMainPlayer();

	if( !player || !player->ItemManager )
	{
		return;
	}

	for( INT i = 0 ; i < E_NES_TOTAL ; i++)
	{
		//iterate over worn equipment

		FClientItemInfo* equip = facade.FindItem( E_CONSLOT_EQUIPPED , i );//Player->ItemManager->GetEquipBySlot( i , true );
		if(equip)
		{
			//whether this equipment is part of a set
			const EquipSuit* suit =  equipMgr.FindEquipSuitByPartOfSuit( equip->m_nTemplateID );
			if(suit)
			{
				if( hasAddedSuitList.FindItemIndex( suit->_ID ) >= 0 )
				{
					continue;
				}

				//count how many pieces of the set are owned
				UINT partsCnt = suit->_PartsCnt;
				UINT totalSuitPartHave = 0;
				for( UINT i = 0; i < partsCnt; ++i )
				{
					ItemInfo* itemInfo = facade.FindItem( E_CONSLOT_EQUIPPED, suit->_Parts[i]._EquipSlot );
					bool isHaveSuit = ( itemInfo != NULL && ( itemInfo->m_nTemplateID == suit->_Parts[i]._ItemID ) );
					if( isHaveSuit )
					{
						++totalSuitPartHave;
					}

				}
				//entries matching the set
				for( UINT i = 0; i < suit->_PropsCnt ; ++i )
				{
					bool isConditionOK = ( suit->_Props[i]._Condition <= totalSuitPartHave );
					if( isConditionOK )
					{
						const ItemRandomAttr* pAttr = facade.GetRandomAttributById( suit->_Props[i]._PropID );
						if( pAttr )
						{
							_partPower += pAttr->power;
						}
					}					
				}

				hasAddedSuitList.AddItem( suit->_ID );
			}
		}		
	}

	MMOPowerPart::RecountPower();
}

void MMOPowerPart_EquippedAll::RecountPower()
{
	_partPower = 0;
	for( INT i = 0 ; i < E_NES_TOTAL ; i++)
	{
		//iterate over worn equipment
		FClientItemInfo* equip = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED , i );
		if(equip)
		{
			_partPower += equip->Pow;
		}

	}

	MMOPowerPart::RecountPower();
}

void MMOPowerPart_EquipSlotEnhance::RecountPower()
{
	_partPower = 0;

	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( pMainPlayer == NULL )
	{
		return;
	}
	INT playerClass = pMainPlayer->GetClassId();

	EquipSlotManager &eSlotMgr = EquipSlotManager::getSingleton();
	for ( INT slotId = 0 ; slotId < NES_TOTAL ; slotId++ )
	{
		EquipSlotProperties *pCurEnhanceSlotLv = eSlotMgr.GetCurMySlotEnhanceState(slotId);
		if( pCurEnhanceSlotLv == NULL )
		{
			continue;
		}
		INT curSlotEnhanceLv = pCurEnhanceSlotLv->CurEnhanceLevel;

		//
		if( curSlotEnhanceLv > 0 )
		{
			const EquipSlotEnhancePropCfgData *pCurSelSlotProp = eSlotMgr.FindEnhancePropCfg( playerClass, slotId, curSlotEnhanceLv );
			if(pCurSelSlotProp == NULL)
			{
				continue;
			}

			for( INT i = 0; i < ENHANCE_EFFECT_COUNT; i++)
			{
				const ItemRandomAttr* randomAttr = MMOFacade::getSingleton().GetRandomAttributById( pCurSelSlotProp->EffectProperties[i] );

				if( randomAttr )
				{
					_partPower += randomAttr->power;
				}	
			}
		}
	}


	MMOPowerPart::RecountPower();
}

void MMOPowerPart_Skill::RecountPower()
{
	_partPower = 0;

	TMap< INT, UMMOAbility* > learnedSkillMap = MMOSkillManager::getSingleton().GetMyLearnedMainSkills();

	for ( TMap< INT, class UMMOAbility* >::TIterator it(learnedSkillMap); it; ++it )
	{
		UMMOAbility *mainAbility = it.Value();
		if(mainAbility)
		{
			//if the main ability matches the weapon rune ability, don't count it, since the weapon already counted it
			UBOOL matchRune = FALSE;
			FClientItemInfo* equipWeapon = MMOFacade::getSingleton().FindItem( CONSLOT_EQUIPPED , EET_Normal_MainHand );
			if( equipWeapon )
			{
				for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i )
				{
					UBOOL actived = FALSE;
					UMMOAbility* skill = MMOEquipManager::getSingleton().GetWeaponRuneSkillBySlotIdx( i , equipWeapon , actived );
					if(skill)
					{
						if( skill->eventGetAbilityId() == mainAbility->eventGetAbilityId() )
						{
							matchRune = TRUE;
							break;
						}
					}
				}
			}
			
			if( !matchRune )
			{
				_partPower += mainAbility->Pow;
			}
		}
	}



	//get the total power of non-rune abilities worn
	//rune abilities are reflected in the weapon combat power

	MMOPowerPart::RecountPower();
}