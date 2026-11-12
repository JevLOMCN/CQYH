#include "stdafx.h"
#include "MMOPlayerInfoManager.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
//#include "MMOHintButtonCommand.h"
#include "MMOPowerManager.h"
#include "MMOSkillManager.h"
#include "role_db_def.h"
#include "FriendManager.h"
#include "TeamManager.h"
#include "MMOGuildData.h"

MMOPlayerInfoManager::MMOPlayerInfoManager()
{
	CurRoleId = 0;
	isShowInfoByRankList = false;
}

MMOPlayerInfoManager::~MMOPlayerInfoManager()
{

}

void MMOPlayerInfoManager::UpdatePlayerEquipInfo( INT roleId , INT equipNum , char* equipData , INT crc )
{
	PlayerDetailInfo* playerInfo = FindPlayerDetailInfo( roleId );

	INT BuffSize = 0;

	if( playerInfo )
	{
		//clear first
		appMemzero( playerInfo->EquipList , sizeof( playerInfo->EquipList ) );
		

		for ( int i = 0; i < equipNum ; ++i )
		{
			ItemInfo iteminfo;
			INT buflen = iteminfo.Load( equipData + BuffSize );
			iteminfo.m_byConSlot = E_CONSLOT_PROFILE;
			BuffSize += buflen;

			
			appMemcpy( &playerInfo->EquipList[ iteminfo.m_byItemSlot ] , &iteminfo , sizeof(ItemInfo) );
			//playerInfo->EquipList[ iteminfo.m_byItemSlot ] = iteminfo;

			MMOPowerManager::getSingleton().UpdateEquipPower( &playerInfo->EquipList[ iteminfo.m_byItemSlot ] );

		}
		playerInfo->EquipCRC = crc;
	}

	MMOGameEvent_OneInt evt( GameEvent_PlayerDetailInfoEquipChange , roleId );
	MMOUI::PostGameEvent( &evt );
}

void MMOPlayerInfoManager::UpdatePlayerPower( INT roleId , INT power )
{
	PlayerDetailInfo* playerInfo = FindPlayerDetailInfo( roleId );

	if( playerInfo )
	{
		playerInfo->Power = power;
	}

	MMOGameEvent_OneInt evt( GameEvent_PlayerDetailInfoAttrChange , roleId );
	MMOUI::PostGameEvent( &evt );
}

void MMOPlayerInfoManager::UpdatePlayerRefineInfo( INT roleId , UINT16* level )
{
	PlayerDetailInfo* playerInfo = FindPlayerDetailInfo( roleId );

	if( playerInfo )
	{
		appMemcpy( playerInfo->RefineLevels , level , sizeof(playerInfo->RefineLevels) );
	}
	//comes together with the equipment message, update along with the equipment notification UI
}

void MMOPlayerInfoManager::UpdatePlayerAttrInfo( INT roleId , INT attrCount , ATTR_DATA* attrData , INT crc , INT Fatigue)
{
	
	PlayerDetailInfo* playerInfo = FindPlayerDetailInfo( roleId );

	if( playerInfo )
	{
		for ( INT i = 0 ; i < attrCount ; ++i )
		{
			INT attrId = attrData[i].byAttrID;
			playerInfo->AttrList[ attrId ] = attrData[i].nAttrValue;
		}
		playerInfo->AttrCRC = crc;
		playerInfo->Fatigue = Fatigue;
	}

	MMOGameEvent_OneInt evt( GameEvent_PlayerDetailInfoAttrChange , roleId );
	MMOUI::PostGameEvent( &evt );
	
}

void MMOPlayerInfoManager::UpdatePlayerSpiritInfo( INT roleId , ROLE_DB_SPIRIT_DATA* data , INT crc )
{
	PlayerDetailInfo* playerInfo = FindPlayerDetailInfo( roleId );


	if( playerInfo )
	{
		
		playerInfo->heavenLevel = data->byLevel;
		playerInfo->heavenExp = data->nExp;
		playerInfo->isLevelLimited = playerInfo->Level < SPIRIT_OPEN_LEVEL;
		
		playerInfo->GasItemList.Empty();
		
		for( INT n = 0 ; n < MAX_AIRSEA_EQUIP_CONTAINER_SIZE ; ++n )
		{
			GasItem gas;
			gas.nContainerID = sasbtEquip;
			gas.nSlotIndex	= n;
			gas.nUniqueID = 0;
			if( data->nEquipContainerTemplate[n] > 0 )
			{
				//has item
				gas.nTempID	= data->nEquipContainerTemplate[n];
				gas.nCurExp	= data->nEquipContainerExp[n];
			}
			else if( n >= data->nEquipContainerSize)
			{
				//not unlocked
				gas.nTempID = 0;
				gas.nCurExp = 0;
				gas.nUniqueID = -1;
			}
			else
			{
				//unlocked, empty
				gas.nTempID = 0;
				gas.nCurExp = 0;
			}
			playerInfo->GasItemList.AddItem( gas );
		}
		
		for(INT n = 0; n < MAX_BLOODSEA_PAGE; ++n)
		{
			playerInfo->nBloodState[n] = (data->byBloodState&(1<<n));
		}

		playerInfo->arrSlotBlood.Empty();
		for (INT n = 0; n < MAX_BLOODSEA_PAGE; ++n)
		{
			for (INT i = 0; i < MAX_BLOODSEA_SLOT_PER_PAGE; ++i)
			{
				BloodSlot bd;
				bd.nPage = n + 1;
				bd.nIndex = i + 1;
				bd.nValue = data->nBloodSlot[n][i];
				FString strCon = TEXT("MMOGame.Heaven.BloodSlotName");
				strCon += appItoa(n);
				strCon += appItoa(i);
				bd.sName = MMOUI::ParseLocalizedPropertyPath(strCon);
				playerInfo->arrSlotBlood.AddItem(bd);
			}
		}

		playerInfo->SpiritCRC = crc;
	}
	MMOGameEvent_OneInt evt( GameEvent_PlayerDetailInfoSpiritChange , roleId );
	MMOUI::PostGameEvent( &evt );
}

void MMOPlayerInfoManager::UpdatePlayerSpellInfo( INT roleId , INT spellCount , SPELL_DATA* spellData , INT crc )
{
	PlayerDetailInfo* playerInfo = FindPlayerDetailInfo( roleId );
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	if( playerInfo )
	{
		playerInfo->SkillList.Empty();

		TMap< INT, INT > allHisAbilities = mmoSkillMgr.GetAllMainAbilitiesIDs( playerInfo->ClassId, ESUIType_Active );
		for (INT i = 0; i < spellCount; ++i)
		{
			INT skillTemplateID = spellData[i].wID;
			if( allHisAbilities.Find( skillTemplateID ) == NULL ){
				continue;
			}

			UINT key = MMOSkillManager::CombinateAbilityID( skillTemplateID, spellData[i].bySpellLevel, spellData[i].byRuneSlot );
			UMMOAbility* ability = mmoSkillMgr.FindAbilityTemplate(key);
			if (ability)
			{
				playerInfo->SkillList.AddItem( ability );
			}
		}
		playerInfo->SpellCRC = crc;
	}

	MMOGameEvent_OneInt evt( GameEvent_PlayerDetailInfoSpellChange , roleId );
	MMOUI::PostGameEvent( &evt );
}

PlayerDetailInfo* MMOPlayerInfoManager::FindPlayerDetailInfo( INT roleId )
{
	PlayerDetailInfo* res = playerDetailInfoMap.Find( roleId );

	if( !res )
	{
		PlayerDetailInfo newInfo;
		playerDetailInfoMap.Set( roleId , newInfo );
		return playerDetailInfoMap.Find( roleId );
	}

	return res;
}

PlayerDetailInfo* MMOPlayerInfoManager::FindCurPlayerDetailInfo(  )
{
	PlayerDetailInfo* res = playerDetailInfoMap.Find( CurRoleId );
	return res;
}

void MMOPlayerInfoManager::showInfoByRankList(INT roleId)
{
	if (isShowInfoByRankList)
	{
		UMMOUIPlayerInfo* playerInfo = Cast<UMMOUIPlayerInfo>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_PlayerInfo ) );
		if(playerInfo)
		{
			if (roleId == playerInfo->_RoleId)
			{
				MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PlayerInfo , TRUE, "" );
			}
		}
	}
	
}
BOOL MMOPlayerInfoManager::ShowDetailCurRole()
{
	if(FriendManager::getSingleton().IsMyFriend(CurRoleId))
		return TRUE;
	if(MMOGuildData::getSingleton().IsGuildFullMember(CurRoleId))
		return TRUE;
	if(TeamManager::getSingleton().GetMemberById(CurRoleId))
		return TRUE;
	return FALSE;
}