#ifndef _USKILLMANAGER_H_
#define _USKILLMANAGER_H_

#include "MMOSingleton.h"
#include "item_def.h"
#include "MMOUtil.h"
#include "MMOGameEvent.h"
#include "net/message_handler.h"
#include "MMODataManager.h"

class UMMOAbility;

typedef std::map<UINT, class UMMOAbility*, std::less<UINT>, STLAllocator< std::pair<INT, class UMMOAbility*>, UE3STLAllocatorPolicy> > AbilityTemplateMap;

static const INT SKILL_EQUIP_MAX = 6;
static const UINT MAX_SKILL_NORMAL_LEVEL = 300;//no limit for now
#define RandomItem 80105

//ability UI update notification
struct MMOGameEvent_SkillUiUpdate : public MMOGameEvent
{
public:
	enum ESkillUiEventType
	{
		ESkillUiEvent_NewSkillLearned,
		ESkillUiEvent_NewSkillForget,
		ESkillUiEvent_EquipSkill,
		ESkillUiEvent_EquipRuneSkill,
		ESkillUiEvent_SkillLevelUp,
		ESkillUiEvent_SkillSlotCmdExchanged,
		ESkillUiEvent_SkillExpChanged,
		ESkillUiEvent_EquippedSkillUpdated
	};

	MMOGameEvent_SkillUiUpdate(ESkillUiEventType eventType)
		: EventType(eventType)
	{
		MagicInt = 0;
		MagicInt2 = 0;
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_SkillUiUpdate;
	}

	ESkillUiEventType EventType;
	INT MagicInt;
	INT MagicInt2;
};

struct RuneSkillLevelUpConsum
{
	INT _ItemID;
	INT _ItemCnt;
};

//
enum SpecailSkillType
{
	ESpcSkill_ShenFa,
	ESpcSkill_FanYing,
	ESpcSkill_ShengCun,
	ESpcSkill_ZhanShu,
	ESpcSkill_MAX
};

struct SpecialSkill
{
	UINT _SkillID;
	SpecailSkillType _SpecialSkillType;
	FString _GetRequire;
	//Spell_ClassType _UseClass;
};

struct SkillDynamicData
{
	UINT _CurExp;
	SkillDynamicData(){
		_CurExp = 0;
	}
};

//
struct TransformSkillGroup
{
	UINT _MainSkillID[SKILL_EQUIP_MAX];
};

//
class MMOSkillManager : public SingletonMeyers<MMOSkillManager>, public INativeGameEventHandler
{
public:
	void Init();
	void Shutdown();
	void ResetPersonalData();

	static UINT CombinateAbilityID(UINT AbilityID, UINT LevelID,UINT RuneID=0);
	static void DecomposeAbilityID(UINT uniqueID, UINT &outSkillID, UINT &outSkillLv,UINT &outSkillRuneID);
	static UBOOL IsSameSeriesAbility(UINT uniqueSkillID1, UINT uniqueSkillID2);

	UBOOL GetSkillSlotActive( INT id );
	void  SetSkillSlotActive( INT id, UBOOL bActive );

	UBOOL GetSkillSlotFlag1( INT id );
	void  SetSkillSlotFlag1( INT id, UBOOL bActive );

	FString GetSkillSlotIcon( INT id );
	void SetSkillSlotIcon( INT id, const FString& IconName );

	static void ConvertSpellType(TArray<UBOOL>& ClassUsble, const FSpell_ClassType& classType);
	static UBOOL IsSkillUsableForPlayer( UMMOAbility *pInAbility, INT playerClassType );
public:
	INT GetESUITypeByAbility( UMMOAbility *pAbility );
	INT GetESUITypeByAbility( const struct AbilityTemplateDataIndex *pData );

	//set corresponding value
	UBOOL SetSpellVar(UMMOAbility* pAbility,const TCHAR* pName,const TCHAR* pValue);
	//
	void LoadAbilityTemplates(EPlayerClass playerClass);
	UBOOL AddAbilityTemplate( UINT k, class UMMOAbility* ability );
	void ClearAbilityTemplates();
	void AutoLinkAbilityRune();
	void ReflashSkillOutlineData();
//	void UpdateRuneDur( RUNE_DATA *tData );
	void IniRuneData( );
	
	class UMMOAbility* FindRune(int SpellID, int SpellLevel, int RuneSlot );
	class UMMOAbility* FindAbilityTemplate(UINT TemplateID);
	class UMMOAbility* FindAbilityTemplateNoWarning(UINT templateID);
	UMMOAbility *GetAbilityByNextLevel( UMMOAbility *ability );
	UMMOAbility *GetAbilityWithRuneByNextLevel( UMMOAbility *ability, UINT maxLevel = MAX_SKILL_NORMAL_LEVEL );
	UMMOAbility *GetMainAbilityFor( UMMOAbility *ability );
	TArray<class UMMOAbility*> GetAllAbilities() const;
	TMap< INT, class UMMOAbility* > GetAllMainAbilitiesSorted( INT classType, INT skillType );
	//
	void SortAllAbilitiesByRequireLevel( TMap< INT, class UMMOAbility* > &skillMap );
	TMap< INT, INT > GetAllMainAbilitiesIDs( INT classType, INT skillType );

	const TArray<INT> *GetRuneDebrisIds()
	{
		return &vecRuneDebrisIds;
	}
public:
	UBOOL RedirectSkillSlot( INT slotIdx, INT toCmdKey, UBOOL isNeedReflashUI = TRUE );
	UBOOL ExchangeRedirectSkillSlot( INT slotIdx1, INT slotIdx2, UBOOL isNeedReflashUI = TRUE );
	void ReflashSkillSlotRedirection( INT32* addr, INT size );
	void UploadSkillSlotRedirection();

	INT GetSkillbarSlotRealCmd( INT slotIdx );
	INT GetSkillbarSlotIdxByKeyCmd( INT cmdIdx );

	INT GetSkillSlotDefaultBindEnum( INT slotIdx );
	INT GetSkillSlotDefaultIdx( INT cmdIdx );

public:
	void OnNativeGameEvent( struct MMOGameEvent* evt );

	UBOOL IsSkillBindIn( const FSpell_BindSlot &skillBindData, INT hotKeyEnum );
	INT GetSkillBindDefaultSlot( const FSpell_BindSlot &skillBindData );
	INT GetSkillBindHotkey( const FSpell_BindSlot &skillBindData );

	void SendEquipSpell( INT slotHotKey, INT skillID );
	void SendEquipRuneSkill( INT slotHotKey, INT runeSlotId );
	//
	UMMOAbility *GetEquippedSkill( INT slotHotKey, UBOOL onlyOnSkillBar = FALSE );
	INT GetEquippedSkillTemplateID( INT slotHotKey );
	UMMOAbility *GetEquippedSkillBySkillID( INT skillTemplateID );
	//ability upgrade
	void UpdateAbility( INT mainAbilityID, INT abilityRuneID, INT level, INT equipID = -1 );
	//newly learned ability
	UBOOL IsNewLearnedSkillInSlot( INT slotEnum );
	UBOOL IsNewLearnedSkill( INT skillID );

	void AddNewLearnedSkill( UINT skillIDKey );
	void RemoveNewLearnedSkill( INT skillID );

	void ClearNewLearnedSkills(){ _NewLearnedSkills.Empty(); }
	//ability slot unlock
	UBOOL IsNewUnlockSlot( INT slotEnum );
	void AddNewSkillSlot( INT slotEnum );
	void RemoveNewSkillSlot( INT slotEnum );
	void ClearNewSkillSlots(){ _NewUnlockSkillSlots.Empty(); }
	//learned abilities
	void AddMyLearnedMainSkillsBySkillBook();

	const TMap< INT, UMMOAbility* > &GetMyLearnedMainSkills();
	UMMOAbility* FindMyLearnedMainSkill( INT mainSkillID );

	void HandleAllAddedMainSkills( INT skillID, INT skillLv, INT equipID );
	void HandleAllDelMainSkills( INT skillID, INT equipID );

	void AddMyLearnedMainSkill( UINT uniqueSkillID );
	void DelMyLearnedMainSkill( UINT skillID );
	//
	UMMOAbility *FindMyLearnedMainSkillByBook( INT skillID );

	void AddMyLearnedMainSkillByBook( UINT uniqueSkillID );
	void DelMyLearnedMainSkillByBook( INT skillID );
	//
	UMMOAbility *FindMyLearnedMainSkillByEquip( INT skillID, INT equipID );
	INT FindCurrentActiveEquipSkill( INT skillID );
	void GetEquipmentEquipSkillBy( INT onEquipSlotIdx, TArray<UMMOAbility*> &equipSkillArray );
	void GetEquippmentOfSameSkill( INT skillID, TArray<INT> &sameSkillEquipIDArray );

	void SwapMyLearnedMainSkillEquipID( INT srcEquipID, INT dstEquipID );

	void AddMyLearnedMainSkillByEquip( UINT uniqueSkillID, INT equipID );
	void DelMyLearnedMainSkillByEquip( INT skillID, INT equipID );

	//some real-time ability data (currently mainly ability experience)
	void SetMySkillExp( INT skillID, UINT exp );
	SkillDynamicData* FindSkillExtraData( INT skillID );
	SkillDynamicData* FindSkillExtraData( UMMOAbility *pAbility );
	void DelSkillExtraData( INT skillID );
	//inscription ability upgrade cost
	RuneSkillLevelUpConsum *GetRuneSkillLevelUpConsum( UINT uniqueSkillID ){
		return _RuneSkillLevelUpMap.Find( uniqueSkillID );
	}
	//interaction ability
	UBOOL LoadSpecialSkills();
	const SpecialSkill *FindSpecialSkill( UINT uniqueSkillID );
	TArray< SpecialSkill > GetSpecialSkillOfType( SpecailSkillType specialType );
	//transformation ability group
	UBOOL LoadTransformSkillGroup();
	const TransformSkillGroup *FindTransformSkillGroupByBuffID( INT buffID );
	UBOOL IsSkillChangeNeedCache( INT hotKey );
	UMMOAbility *GetInTransformCachedSkill( INT hotKey );
	UMMOAbility *GetInTransformCachedSkillByID( INT skillTemplateID );
	void InTransformStateSkillChanged( INT hotKey, INT skillMainID, INT skillLevel, INT skillRuneIdx, UBOOL isLocked );
	void RecoverCachedSkillsInTransform();
	UBOOL EquipTransformSkillGroupAndCachOldSkills( INT skillGroupID );
	ECommandKey GetGetTransformSkillGroupBindHotKey( INT bindSlotIdx );
private:
	AbilityTemplateMap _abilityTemplateMap;
	//all classes ability list
	TArray< TArray< TArray< INT > > > _AllMainAbilityIDList;
	//rune fragments
	TArray<INT> vecRuneDebrisIds;

	TMap< INT, INT > _SkillSlotCmdDefault;
	INT _SkillBarSlotRedirect[SKILL_EQUIP_MAX];

	TMap< INT, UBOOL > _SkillSlotActive;
	TMap< INT, UBOOL > _SkillSlotFlag1;
	TMap< INT, FString > _SkillSlotIcon;
	TMap< INT, SkillDynamicData > _SkillDynData;
	
private:
	//key is the ability ID (not unique ID)
	TMap< INT, UMMOAbility* > _MyLearnedMainSkills;
	TMap< INT, UMMOAbility* > _MyLearnedmainSkillsByBook;
	TMap< INT, TMap< INT, UMMOAbility* > > _MyLearnedSkillsByEquip;
	TMap< INT, INT > _NewLearnedSkills;
	TMap< INT, INT > _NewUnlockSkillSlots;
	//
	TMap< UINT, RuneSkillLevelUpConsum > _RuneSkillLevelUpMap;
private:
	//interaction ability
	TMap< INT, SpecialSkill > _SpecialSkillsMap;
private:
	//ability group after transformation
	TMap< INT, TransformSkillGroup > _TransformSkillGroupMap;
	TMap< INT, UMMOAbility* > _InTransformSkillCache;
	
	UMMOAbility* LoadAbilityFromLevelOne(UINT skillID, UINT skillRuneID, const FString& dataPath);
	UMMOAbility* LoadAbility(UINT skillID, UINT skillRuneID, UINT skillLv, const FString& dataPath);
	UBOOL InitAbility(UMMOAbility* ability);
	//void LoadAbilityLevel(UMMOAbility* maiAability);
private:
	UMMOAbility* CurRangeShowedSkill;
};

//------------------------------------------------------------------------------------------------
// AbilityTemplateDataIndex
//------------------------------------------------------------------------------------------------
struct AbilityTemplateDataIndex
{
	FString ObjectPath;
	INT TemplateID;
	FSpell_ClassType Class;
	INT RuneSlotID;
	ESPELL_CAST_TYPE CastType;
	ESPELL_QTE_TYPE QTEType;
	UBOOL ShowOnUI;
};


struct AbilityTemplateIndexDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("spell.txt");
	}


	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}


	static AbilityTemplateDataIndex* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		AbilityTemplateDataIndex* item = new AbilityTemplateDataIndex;
		//item->TemplateID = templateID;
		needAddToMap = false;
		return item;
	}

	static void DestroyDataObject(AbilityTemplateDataIndex* obj)
	{
		delete obj;
	}

	static void ParseData(AbilityTemplateDataIndex* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("SpellCommonAttribute.ID");
	}

	static FString FormatAsString(AbilityTemplateDataIndex* obj)
	{
		return obj->ObjectPath;
	}

	static void PostInitData(){}

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(AbilityTemplateDataIndex* obj, TSet<FString>& outResList);
#endif
};

class AbilityTemplateDataIndexManager
	: public TDataManger< AbilityTemplateDataIndex, AbilityTemplateIndexDataProcessor >
{
public:

	friend class MMOSkillManager;

	AbilityTemplateDataIndexManager() {}
	virtual ~AbilityTemplateDataIndexManager() {}
};

//------------------------------------------------------------------------------------------------
// SpellVar Data
//------------------------------------------------------------------------------------------------
struct SpellVarData
{
	INT TemplateID;
	INT RuneSlotID;
	INT Level;
	FString Variable1;
	FString Value1;

	FString Variable2;
	FString Value2;

	FString Variable3;
	FString Value3;

	FString Variable4;
	FString Value4;

	FString SpellName;

	FString MainSpeciality;
	FString SecondSpeciality;
	FString ThirdSpeciality;
	FString RuneDescription;

	FString DescriptiveWords;
	FString UseMethod;
	FString AttackRange;
	FString EffectRange;
	INT EffectRangeEnum;

	FString DetailDescription;
};


struct SpellVarDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("spell_var.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static SpellVarData* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		SpellVarData* item = new SpellVarData;
		//item->TemplateID = templateID;
		needAddToMap = false;
		return item;
	}

	static void DestroyDataObject(SpellVarData* obj)
	{
		delete obj;
	}

	static void ParseData(SpellVarData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(SpellVarData* obj)
	{
		return FString::Printf(TEXT("SpellVar ID[%d] RuneSlotID[%d] Lv[%d] Desc[%s]"),
			obj->TemplateID, obj->RuneSlotID, obj->Level, *obj->DetailDescription);
	}

	static void PostInitData(){}

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(SpellVarData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

class SpellVarManager
	: public TDataManger< SpellVarData, SpellVarDataProcessor >
{
public:

	SpellVarManager() {}
	virtual ~SpellVarManager() {}
};

//------------------------------------------------------------------------------------------------
// RuneData
//------------------------------------------------------------------------------------------------
// struct RuneData
// {
// 	INT SpellID;
// 	INT SlotID;
// 	
// 	INT ItemID1;
// 	INT ItemCount1;
// 
// 	INT ItemID2;
// 	INT ItemCount2;
// 
// 	INT ItemID3;
// 	INT ItemCount3;
// 
// 	INT ItemID4;
// 	INT ItemCount4;
// 
// 	INT ItemID5;
// 	INT ItemCount5;
// 	
// 	INT DurabilityType;
// 	INT DurabilityType;
// };

// 
// struct RuneDataProcessor
// {
// 	static RuneData* CreateDataObject(INT templateID, bool& needAddToMap)
// 	{
// 		RuneData* item = new RuneData;
// 		item->TemplateID = templateID;
// 		return item;
// 	}
// 
// 	static void DestroyDataObject(RuneData* obj)
// 	{
// 		delete obj;
// 	}
// 
// 	static void ParseData(RuneData* obj, class CTabFile* dataRow, int rowIdx);
// 
// 	static FString GetTemplateIDName()
// 	{
// 		return TEXT("SpellID");
// 	}
// 
// 	static FString FormatAsString(RuneData* obj)
// 	{
// 		return obj->DetailDescription;
// 	}
// 
// 	static void PostInitData(){}
// };
// 
// class RuneDataManager
// 	: public TDataManger< RuneData, RuneDataProcessor >
// {
// public:
// 
// 	RuneDataManager() {}
// 	virtual ~RuneDataManager() {}
// };


//------------------------------------------------------------------------------------------------
// SpellProgessData
//------------------------------------------------------------------------------------------------
struct SpellProgessData
{
	INT TemplateID;
	INT RuneSlot;
	INT Level;
	INT ConsumePts;
	INT RequireLevel;
	INT RequireItem;
	INT RequireItemCount;
	INT Pow;
};


struct SpellProgessDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("spell_progress.txt");
	}


	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static SpellProgessData* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		SpellProgessData* item = new SpellProgessData;
		needAddToMap = false;
		//item->TemplateID = templateID;
		return item;
	}

	static void DestroyDataObject(SpellProgessData* obj)
	{
		delete obj;
	}

	static void ParseData(SpellProgessData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("SpellID");
	}

	static FString FormatAsString(SpellProgessData* obj)
	{
		return FString::Printf(TEXT("SpellProgessData:%d"), obj->TemplateID);
	}

	static void PostInitData(){}

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(SpellProgessData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

class SpellProgessDataManager
	: public TDataManger< SpellProgessData, SpellProgessDataProcessor >
{
public:

	SpellProgessDataManager() {}
	virtual ~SpellProgessDataManager() {}
};


//------------------------------------------------------------------------------------------------
// MissileTemplateDataIndex
//------------------------------------------------------------------------------------------------
struct MissileTemplateDataIndex
{
	FString ObjectPath;
	INT TemplateID;
};


struct MissileTemplateIndexDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("missile.txt");
	}


	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}


	static MissileTemplateDataIndex* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		MissileTemplateDataIndex* item = new MissileTemplateDataIndex;
		needAddToMap = false;
		return item;
	}

	static void DestroyDataObject(MissileTemplateDataIndex* obj)
	{
		delete obj;
	}

	static void ParseData(MissileTemplateDataIndex* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(MissileTemplateDataIndex* obj)
	{
		return obj->ObjectPath;
	}

	static void PostInitData(){}

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(MissileTemplateDataIndex* obj, TSet<FString>& outResList);
#endif
};

class MissileTemplateDataIndexManager
	: public TDataManger< MissileTemplateDataIndex, MissileTemplateIndexDataProcessor >
{
public:

	MissileTemplateDataIndexManager() {}
	virtual ~MissileTemplateDataIndexManager() {}
};



#endif
