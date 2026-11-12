#ifndef _UEQUIPMANAGER_H_
#define _UEQUIPMANAGER_H_

#include "MMOSingleton.h"
#include "item_def.h"
#include "CommonGameEvents.h"
#include "MMOFacade.h"

enum EItemUseError
{
	ITEM_Use_Err_None,
	ITEM_Use_Err_Undefine,
	ITEM_Use_Err_RequireClass,
	ITEM_Use_Err_Gender,
	ITEM_Use_Err_LowLevel,
	ITEM_Use_Err_LowPhyAtk,
	ITEM_Use_Err_LowMagAtk,
	ITEM_Use_Err_LowTaoAtk,
};

#define WEAPON_LEVEL_MAX (7)
#define ENHANCE_ITEM_CONSUME (3)
#define ENHANCE_EQUIP_PROPERTY (5)

static const INT ENHANCE_EQUIP_MAX_LV = 300;
//
struct EquipMaxDur{
	INT _ContainerID;
	INT _SlotID;
	INT _MaxDur;
	EquipMaxDur(){
		_ContainerID = 0;
		_SlotID = 0;
		_MaxDur = 0;
	}
};

//Enhancement data
struct ValueData{
	UINT _Type;
	UINT _Value;
	ValueData(){
		_Type = 0;
		_Value = 0;
	}
};

struct MMOGameEvent_BagSlotHintChange : public MMOGameEvent
{
	MMOGameEvent_BagSlotHintChange( INT conID , INT slotID , FClientItemInfo *itemInfo )
		: ConID(conID)
		, SlotID(slotID)
		, ItemInfo(itemInfo)
	{
		EventType = GameEvent_BagSlotHintChange;
	}

	INT ConID;
	INT SlotID;
	FClientItemInfo *ItemInfo;
	EGameEvent EventType;
	virtual EGameEvent GetEventType() const{ return EventType; }
};

struct EnhanceData
{
	UINT _EnhanceNeedMny1;
	UINT _EnhanceNeedMny2;
	UINT _TransferNeedMny2;
	ValueData _EnhanceNeed[ENHANCE_ITEM_CONSUME];

	EnhanceData(){
		_EnhanceNeedMny1 = 0;
		_EnhanceNeedMny2 = 0;
		_TransferNeedMny2 = 0;
		//for( UINT i =0; i < ENHANCE_ITEM_CONSUME; ++i ){
		//	_EnhanceNeed[i]._Type = 0;
		//	_EnhanceNeed[i]._Value = 0;
		//}
	}
};

struct EnhanceResult
{
	//ValueData _Prop[ENHANCE_EQUIP_PROPERTY];
	INT _Prop[ENHANCE_EQUIP_PROPERTY];
	//EnhanceResult(){
	//	for( UINT i =0; i < ENHANCE_EQUIP_PROPERTY; ++i ){
	//		_Prop[i]._Type = 0;
	//		_Prop[i]._Value = 0;
	//	}
	//}
};

//suit
//static const UINT CONST_EquipSuit_Parts_Max = 8;
//static const UINT MAX_SET_ATTR_COUNT = 8;
struct EquipSuitPart
{
	INT _ItemID;
	UINT8 _EquipSlot;
	FString _Name;

	EquipSuitPart();

	const FString &GetDisplayName() const;
};

struct EquipSuitProp
{
	INT _PropID;
	UINT8 _Condition;

	EquipSuitProp();
};

struct EquipSuit
{
	INT _ID;
	UINT8 _PartsCnt;
	UINT8 _PropsCnt;
	EquipSuitPart _Parts[MAX_SET_PART_COUNT];
	EquipSuitProp _Props[MAX_SET_ATTR_COUNT];
	FString _SuitName;

	EquipSuit();
};

struct SoulInfo
{
	INT _ID;
	INT _ItemID;
	INT _AttrID;
	INT _MatchSocket;
	INT _SoulLevel;
	INT _PriceValue;
	INT _UpgradeSoulID;
	INT _DetouchPrice;

	SoulInfo();
};

//Inscription
static const INT EQUIP_RUNE_SKILL_MAX = 2;//MAX_RUNE_COUNT;

struct WeaponSkillCfg
{
	UINT _SkillID;
	UINT _SkillRuneID;
	UBOOL _IsRare;
	UINT _Level;
	UINT _RarePriority;
	//UINT _RemoveSkillCost;
};
//static const INT WEAPON_LV_UP_ITEM_ID = 200;

//
static const INT EQUIP_LEVEL_MAX = 7;
struct EquipLvUpCfg
{
	UINT _Level;
	UINT _MnyCost;
	UINT _PowerAdd;
};

typedef TArray< EnhanceData > EnhanceConsumptionContainer;
typedef TMap< UINT32, TArray< EnhanceResult > > EnhanceEquipPropContainer;
typedef TMap< INT, EquipSuit > EquipSuitContainer;
//typedef std::map<INT, class UMMOSoulInfo*, std::less<INT>, STLAllocator< std::pair<INT, class UMMOSoulInfo*>, UE3STLAllocatorPolicy>> SoulInfoTemplateMap;
typedef TMap< INT, SoulInfo > SoulInfoTemplateMap;

class MMOEquipManager : public SingletonMeyers<MMOEquipManager>, public INativeGameEventHandler
{
public:
	MMOEquipManager();
	~MMOEquipManager();
public:
	static UINT CombinateWeaponSkillID(INT clientClassEnum, UINT weaponSkillSlotIdx, UINT weaponSkillTypeInSlot );
	static void DecomposeWeaponSkillID(UINT weaponSkillID, UINT &outClientClassEnum, UINT &outWeaponSkillSlotIdx, UINT &outWeaponSkillTypeInSlot );

	static UINT CombinateItemPosID( INT containerID, INT slotID );
	static void DecomposeItemPosID( UINT itemPosID, INT &containerID, INT &slotID );
public:
	INT GetEquipCurDurPect( ItemInfo *pEquipItemInfo ){
		if( pEquipItemInfo == NULL ){
			return -1;
		}

		INT cvtCurDurability = ( pEquipItemInfo->m_nCurDur + EQUIP_DUR_MULTIPLIER -1 ) / EQUIP_DUR_MULTIPLIER;
		INT cvtCurMaxDurability = ( pEquipItemInfo->m_nMaxDur + EQUIP_DUR_MULTIPLIER -1 ) / EQUIP_DUR_MULTIPLIER;
		if( cvtCurMaxDurability <= 0 ){
			return 0;
		}
		return ( cvtCurDurability * 100 / cvtCurMaxDurability );
	}

	INT GetEquipCurMaxDurPect( ItemInfo *pEquipItemInfo, UItem *pItem ){
		if( pEquipItemInfo == NULL || pItem == NULL ){
			return -1;
		}

		INT cvtCurMaxDurability = ( pEquipItemInfo->m_nMaxDur + EQUIP_DUR_MULTIPLIER -1 ) / EQUIP_DUR_MULTIPLIER;
		INT cvtMaxDurability = pItem->DefaultDuration;
		return ( cvtCurMaxDurability * 100 / cvtMaxDurability );
	}

public:
	//Read item enhancement info
	void Initialize();
	void Shutdown();
	//UBOOL ReadItemEnhanceConsumption();
	//UBOOL ReadItemEnhanceResult();
	void OnNativeGameEvent( struct MMOGameEvent* evt );

	//Get enhancement info
	const EnhanceData* GetEnhanceConsumptionInEquipBag( UINT bagIdx );
	const EnhanceResult* GetEnhanceResultInEquipBag( UINT bagIdx );

	const EnhanceData* GetEnhanceConsumption( UINT curEnhanceLv );
	const EnhanceResult* GetEnhanceResult( const FString & equipSubType, const FString &equipSubSubType, UINT enhanceLv );
	UINT32 GetEnhanceResultKey( const FString & equipSubType, const FString &equipSubSubType );
public:
	//Soul casting
	UBOOL CheckEquipWeaponSkillNeedHint();
	UBOOL EquipSkillsToFitCurWeapon( UBOOL canEquipWeaponSkill );
	void ModifyEquippedSkillsAccordingSyncEquippedWeapon();
	//
	void SetAutoChangeSkills( UBOOL isTrue );
	UBOOL IsAutoChangeSkills(){
		return _IsAutoChangeSkills;
	}
	void SetAllowedChangeSkills( UBOOL isTrue );
	UBOOL IsAllowedChangeSkills(){
		return _IsAllowedChangeSkills;
	}
	//
	void UpdateSkillLevelByCurWeapon();

public:
	//Equipment set
	UBOOL ReadEquipSuit();
	EquipSuit* FindEquipSuitByPartOfSuit( INT itemID );
	EquipSuit* FindOrLoadFullEquipSuitBySuitID( INT suitID );

	//Repair cost
	INT CountEquipRepairCostAll();
	INT GetEquipRepairCost( const UEquip *pEquip, const ItemInfo *pItemInfo, UBOOL isSpecialRepair = FALSE ) const;

	//
	void ClearLastCachedRepairEquip();
	void CacheMaxDurForAllEquipRepair();
	void CacheMaxDurForEquipRepair( ItemInfo *equipInfo );

	//Sum of equipment luck values
	UINT32 CountEquipLuckTotal();
	//
	UBOOL CheckItemUseableForMe( const UItem *pItem ) const;
	UBOOL CheckEquipPropRequireForMe( const UEquip *pEquip ) const;

	//Weapon upgrade
	UBOOL IsMaxUpgradeLv( FClientItemInfo *pWeaponInfo ){
		if( pWeaponInfo == NULL ){
			return FALSE;
		}

		return ( ( pWeaponInfo->m_byLevelupMagPoint + pWeaponInfo->m_byLevelupPhyPoint + pWeaponInfo->m_byLevelupTaoPoint ) >= WEAPON_LEVEL_MAX );
	}
	//Equipment obtained prompt
	UBOOL DescideToShowEquipNewHint( INT conID, INT slotID );
public:
	UBOOL ReadWeaponRuneSkillData();

	//When the return value is TRUE, outuniqueSkillID returns the weapon's default ability ID
	UBOOL GetSkillUniqueIDOnWeapon( UINT &outuniqueSkillID, UINT weaponSkillID );

	const WeaponSkillCfg *GetWeaponSkillCfg( UINT weaponSkillID );
	//const WeaponSkillCfg *GetWeaponSkillCfgByItemInfo( const ItemInfo *pItemInfo, INT skillBindSlot );

	//Returns the rune ability of this slot on the character's equipped weapon, outIsSkillActive is whether the ability is activated
	UMMOAbility* GetWeaponRuneSkillBySlotIdx( INT weaponSkillSlotIdx, INT contID, INT slotID, UBOOL &outIsSkillActive ){
		ItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( contID, slotID );
		return GetWeaponRuneSkillBySlotIdx( weaponSkillSlotIdx, pItemInfo, outIsSkillActive );
	}
	UMMOAbility* GetWeaponRuneSkillBySlotIdx( INT weaponSkillSlotIdx, const ItemInfo *pItemInfo, UBOOL &outIsSkillActive );
	UMMOAbility* GetCurWeaponRuneSkillOfMainSkillID( INT mainSkillID, UBOOL &outIsSkillActive );
	INT GetWeaponRuneSkillIDBySlotIdx( INT weaponSkillSlotIdx, const ItemInfo *pItemInfo );
	//
	UMMOAbility* GetWeaponRuneSkillByCmdKey( INT skillBindCmdKey, INT contID, INT slotID, UBOOL &outIsSkillActive ){
		return NULL;
		//return GetWeaponRuneSkillBySlotIdx( skillBindCmdKey - ECK_Slot1, contID, slotID, outIsSkillActive );
	}
	UMMOAbility* GetWeaponRuneSkillByCmdKey( INT skillBindCmdKey, const ItemInfo *pItemInfo, UBOOL &outIsSkillActive ){
		return NULL;
		//return GetWeaponRuneSkillBySlotIdx( skillBindCmdKey - ECK_Slot1, pItemInfo, outIsSkillActive );
	}
	//UMMOAbility* GetWeaponRuneSkillBase( UINT itemTemplateID, INT skillBindSlot, INT weaponSkillTypeInSlot, INT weaponSkillLv );
	INT GetWeaponCurSkillSlotCnt( const ItemInfo *pItemInfo );

	//Inscription replacement system
	UMMOAbility* GetWeaponRplcCacheRuneSkillBySlotIdx( INT weaponSkillSlotIdx, INT contID, INT slotID ){
		ItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( contID, slotID );
		return GetWeaponRplcCacheRuneSkillBySlotIdx( weaponSkillSlotIdx, pItemInfo );
	}
	UMMOAbility* GetWeaponRplcCacheRuneSkillBySlotIdx( INT weaponSkillSlotIdx, const ItemInfo *pItemInfo );
	INT GetWeaponRplcCacheRuneSkillID( INT weaponSkillSlotIdx, const ItemInfo *pItemInfo );
	//
	void NotifyWeaponRplcCachedRuneChg( UINT16 (&weaponSkillIDs)[MAX_RUNE_COUNT], UBOOL isSuccess );
	void SetWeaponRplcCachedRune( INT weaponSkillSlotIdx, INT weaponSkillID );
	void ResetWeaponRplcCachedRune(){ appMemset( &_WeaponRplcCachedRune, 0, sizeof(UINT) * EQUIP_RUNE_SKILL_MAX); }

public:
	UBOOL ShortcutToPutIntoUIPanel( INT fromSlotType, INT containerID, INT slotID );
	//
	UBOOL IsItemUsableOnSkillBar( UItem *pItem );
	UBOOL IsItemMultiOnSkillBar( UItem *pItem );

	void BeforeServerEquipSwapHandleWeaponMainSkill( INT srcConIdx, INT srcSlotIdx, INT dstConIdx, INT dstSlotIdx );
	void CheckEquipmentHandleWeaponMainSkill( INT toCheckEquipSkillEquipSlot );
private:
	//Equipment enhancement
	EnhanceConsumptionContainer _EnhanceConsumptionContainer;
	EnhanceEquipPropContainer _EnhanceEquipProp;
	TMap<FString, UINT32> _EnhanceKey;

	//Enhancement UI data
	//INT _CurEnhanceEquipBagIdx;
private:
	//Equipment set
	EquipSuitContainer _EquipSuits;
	//key is itemID, value is set ID
	//TMap< UINT, INT > _EquipSuitLookup;
private:
	//Equipment ability (rune ability)
	//EquipSuitContainer _EquipSuits;
	//key is the combination of the first 3 columns of weapon_rune_id,
	TMap< UINT, WeaponSkillCfg > _EquipRuneSkills;
	//
	UBOOL _IsAutoChangeSkills;
	UBOOL _IsAllowedChangeSkills;
	UGFxObject *_ConfirmEquipWeaponSkill;
	//
	UINT _WeaponRplcCachedRune[EQUIP_RUNE_SKILL_MAX];
public:
	//Spirit fusion
	UBOOL ReadSoulInfoTemplates();
	//SoulItem* FindSoulInfoBySoulID(INT soulID);
	const SoulInfo* FindSoulInfoByItemTemplateID( INT itemID );
	//void AddSoulInfoTemplate( INT key, class UMMOSoulInfo *pNode);
private:
	//Spirit fusion
	SoulInfoTemplateMap _SoulInfoTemplateMap;

	//Weapon upgrade
public:
	UBOOL ReadEquipLevelUpData();
	const EquipLvUpCfg* FindEquipLevelUpData( INT level );
	EquipLvUpCfg* AddNewEquipLevelUpData( INT level );
private:
	TArray< EquipLvUpCfg > _EquipLevelUpCfg;

private:
	TArray< EquipMaxDur > _EquipRepairDurability;
};

////
//struct EquipLevelUpDataProcessor
//{
//	static const TCHAR* GetName()
//	{
//		return TEXT("equip_levelup.txt");
//	}
//
//	static INT ParseID(const FString& idStr)
//	{
//		return appAtoi(*idStr);
//	}
//
//	static EquipLvUpCfg* CreateDataObject(INT level, bool& needAddToMap)
//	{
//		EquipLvUpCfg* item = MMOSkillManager::getSingleton().AddNewEquipLevelUpData( level );
//		//item->TemplateID = templateID;
//		needAddToMap = false;
//		return item;
//	}
//
//	static void DestroyDataObject( EquipLvUpCfg* obj )
//	{
//	}
//
//	static void ParseData( EquipLvUpCfg* obj, class CTabFile* dataRow, int rowIdx );
//
//	static FString GetTemplateIDName()
//	{
//		return TEXT("EquipLevel");
//	}
//
//	static void PostInitData(){}
//};
//
//class EquipLevelUpDataManager: public TDataManger< EquipLvUpCfg, EquipLevelUpDataProcessor >
//{
//public:
//
//	friend class MMOEquipManager;
//
//	EquipLevelUpDataManager() {}
//	virtual ~EquipLevelUpDataManager() {}
//};

#endif
