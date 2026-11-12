
#ifndef _MMOPlayerInfoManager_H_
#define _MMOPlayerInfoManager_H_

#include "Map.h"
#include "MMOSingleton.h"
#include "MMOUtil.h"
#include "MMOFacade.h"
#include "MMOHeavenManager.h"

typedef struct _PlayerDetailInfo
{
	INT AttrList[catTotal];
	FClientItemInfo EquipList[E_NES_TOTAL];
	TArray<UMMOAbility*> SkillList;
	TArray<GasItem> GasItemList;
	TArray<BloodSlot> arrSlotBlood;
	INT				  nBloodState[MAX_BLOODSEA_PAGE];
	UINT16 RefineLevels[NES_TOTAL];
	INT ClassId;
	INT Level;
	INT Sex;
	FString RoleName;
	INT Power;
	//realm
	INT heavenLevel;
	INT heavenExp;
	UBOOL isLevelLimited;

	INT AttrCRC;
	INT EquipCRC;
	INT SpellCRC;
	INT SpiritCRC;

	INT32 Fatigue;

	_PlayerDetailInfo()
	{
		ZeroMemory( this, sizeof(_PlayerDetailInfo) );
	}

}PlayerDetailInfo;

typedef struct _PlayerSimpleInfo
{

	INT ClassId;
	INT Level;
	INT Sex;
	FString RoleName;
	UBOOL Vip;
	FString GuildName;

	//INT AttrCRC;
	//INT EquipCRC;

	_PlayerSimpleInfo()
	{
		ZeroMemory( this, sizeof(_PlayerSimpleInfo) );
	}

}PlayerSimpleInfo;

typedef TMap< INT , PlayerDetailInfo > PlayerDetailInfoMap;

class MMOPlayerInfoManager : public SingletonMeyers<MMOPlayerInfoManager>
{
public:

	MMOPlayerInfoManager();
	virtual ~MMOPlayerInfoManager();


	void UpdatePlayerRefineInfo( INT roleId , UINT16* level );
	void UpdatePlayerEquipInfo( INT roleId , INT equipNum , char* equipData , INT crc );
	void UpdatePlayerAttrInfo( INT roleId , INT attrCount , ATTR_DATA* attrData , INT crc , INT32 Fatigue);
	void UpdatePlayerSpiritInfo( INT roleId , ROLE_DB_SPIRIT_DATA* data , INT crc );
	void UpdatePlayerPower( INT roleId , INT power );
	void UpdatePlayerSpellInfo( INT roleId , INT attrCount , SPELL_DATA* attrData , INT crc );

	PlayerDetailInfo* FindPlayerDetailInfo( INT roleId );

	PlayerDetailInfo* FindCurPlayerDetailInfo( );
	
	void SetCurRole( INT roldId ){ CurRoleId = roldId; }
	INT GetCurRole( ) { return CurRoleId; }

	BOOL ShowDetailCurRole();

	PlayerSimpleInfo&	GetSimpleInfo(){ return CurSimpleInfo; }

	bool getIsShowInfoByRankList(){return isShowInfoByRankList;}
	void setIsShowInfoByRankList(bool isShow){isShowInfoByRankList = isShow;}

	void showInfoByRankList(INT roleId);

protected:

	PlayerDetailInfoMap playerDetailInfoMap;
	INT CurRoleId;

	PlayerSimpleInfo	CurSimpleInfo;

	bool isShowInfoByRankList;
};


#endif