#ifndef __EQUIPSLOTMANAGER_H__
#define __EQUIPSLOTMANAGER_H__

#include "MMOGameEvent.h"
#include "common.h"
#include "MMOUtil.h"

//Maximum number of material types needed for enhancement
#define ENHANCE_CONSUME_COUNT 3
//Maximum number of equipment slots
#define EQUIP_SLOT_COUNT 13
//Number of attributes that can be added after enhancement
#define ENHANCE_EFFECT_COUNT 5
//Maximum enhancement level, convenient for pre-allocating the vector size to avoid repeated reallocation and copying
#define MAX_ENHANCE_LEVEL 150

//Maximum activation level
#define MAX_ACTIVE_LEVEL 10


enum EnhanceStatus
{
	EES_NONE = 0x00000000,
	EES_INCONFIRM = 0x00000001,
	EES_MAX = 0xf0000000,
};

typedef TMap< UINT, struct EquipSlotEnhancePropCfgData > EquipSlotEnhancePropMap;
typedef TMap< UINT, struct EquipSlotEnhanceCfgData > EquipSlotEnhanceMap;


struct EquipSlotEnhancePropCfgData
{
	INT RefineLv;
	INT ClassId;
	INT EquipSlotId;

	INT EffectProperties[ENHANCE_EFFECT_COUNT];
};

//
struct EquipSlotEnhanceCfgData
{
	INT RefineLv;
	INT PlayerRequireLv;
	INT MoneyCost;
	
	INT BaseSuccessRate;
	UINT SuccessRateFactor;

	UBOOL IsSysTip;
};

struct EquipSlotProperties
{
	EquipSlotProperties(){
		CurEnhanceLevel = 0;
		MaxEnhanceLevel = 0;
	}
	UINT CurEnhanceLevel;     //Current enhancement level
	UINT MaxEnhanceLevel;
};


//Enhancement activation config table data structure
struct EquipSlotEnhanceActiveTemplate
{
	typedef struct
	{
		INT EType; //0 bonus value, 1 add ability, etc.
		INT AttributeId; //Added attribute id
		INT Value; //Bonus value, or ability id
		
	}EffectData;

	INT level;

	TArray<EffectData> ActiveEffectList;
};

/*class EquipSlotData
{
public:

public:
	EquipSlotProperties m_EquipSlot
};*/


struct MMOGameEvent_EquipSlotEnhanceResult : public MMOGameEvent
{
public:

	MMOGameEvent_EquipSlotEnhanceResult( INT slotID, UBOOL isSuccess, INT NewEnhanceLv )
		: EnhanceSlotID(slotID),
		  IsSuccess(isSuccess),
		  NewEnhanceLv(NewEnhanceLv)
	{

	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_EquipSlotEnhanceLvUp;
	}

	INT EnhanceSlotID;
	UBOOL IsSuccess;
	INT NewEnhanceLv;
};

class EquipSlotManager : public SingletonMeyers<EquipSlotManager>, public INativeGameEventHandler
{
public:
	EquipSlotManager();
	virtual ~EquipSlotManager();

	void Init();
	void Shutdown();
	void Clear();

public:
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);
	void SyncEquipSlotLevel(UINT16 level[NES_TOTAL]);

public:
	//
	void ReadEnhancePropConfig();
	const EquipSlotEnhancePropCfgData *FindEnhancePropCfg( INT classID, INT partID, INT refineLv );

	//
	void DecomposeRefineKey(UINT refineKey, UINT &outClassID, UINT &outPartID,UINT &outRefineLv )
	{
		outClassID = refineKey >> 24;
		outPartID = ( ( refineKey & 0x00ff0000 ) >> 16 );
		outRefineLv = refineKey & 0x0000ffff;
	}

	//
	void ReadEquipSlotEnhanceConfig();
	const EquipSlotEnhanceCfgData *FindEnhanceConsumeCfg( INT refineLv );

	//Own enhancement data
	EquipSlotProperties *GetCurMySlotEnhanceState( INT slotID )
	{
		if( slotID < 0 || slotID >= NES_TOTAL )
		{
			return NULL;
		}

		return &_MyEquipSlotProperties[slotID];
	}

	UBOOL IsMaxSlotEnhanceLevel( INT enhanceClassID, INT slotID, INT enhanceLv );
	void CvtEnhanceLevel( INT enhanceLv, INT &ehcMajorLevel, INT &ehcMinorLevel, INT &ehcMinorMinorLv );
	
	//
	EquipSlotProperties* GetMyEquipSlotProperty(INT slotID);
	INT GetMyEquipSlotLevel(INT slotID);
	INT GetCurEnhanceHoleLv();

	UBOOL CheckEnhanceConsume( INT containerID, INT slotID, INT checkSlotID );
	UBOOL CheckEnhanceConsume( UEquip *pEquip, INT checkSlotID );
	//UBOOL CheckMyMaterialCondition(INT slotId);

	//Enhancement stage level
	const TArray<INT> &GetSortedEnhanceExtrPropLvIdx( INT classID )
	{
		check( classID >= 0 && classID < EPCT_MAX );
		return _RefineExtrPropLvIdx[classID];
	}

	//Get display text
	FString GetEquipPropertyNameById(INT id);
	FString GetEquipSlotNameById(INT id);

	//Send message
	void SendEnhanceEquipSlot( INT refineSlot, INT consumeConID, INT consumeSlotID );

	//Popup state
	FORCEINLINE void SetFlag(EnhanceStatus es)
	{
		m_iEnhanceFlag |= es;
	}

	FORCEINLINE INT CheckFlag(EnhanceStatus es) const
	{
		return m_iEnhanceFlag & es;
	}

	FORCEINLINE void ClearFlag(EnhanceStatus es)
	{
		m_iEnhanceFlag &= (~es);
	}

	//-----------------------------------------
	void ApplyEnhancePropertyToEquip();
	INT GetEnhanceActiveLevel() const;
	EquipSlotEnhanceActiveTemplate& GetEquipSlotEnhanceActiveTemplate(INT lv);

private:
	//
	UINT CombineRefineKey( INT classID, INT partID, INT refineLv )
	{
		INT ret = ( classID << 24 & 0xff000000 );
		ret |= ( partID << 16 & 0x00ff0000 );
		ret |= refineLv & 0x0000ffff;
		return (UINT)ret;
	}
	
private:
	EquipSlotEnhancePropMap _EnhancePropCfg;
	EquipSlotEnhanceMap _EnhanceEquipSlotCfg;

	TArray<INT> _RefineExtrPropLvIdx[EPCT_MAX];

private:
	EquipSlotProperties _MyEquipSlotProperties[EQUIP_SLOT_COUNT];
	INT m_iEnhanceFlag;

private:
	EquipSlotEnhanceActiveTemplate m_EquipSlotEnhanceActiveData[MAX_ACTIVE_LEVEL];

};

#endif