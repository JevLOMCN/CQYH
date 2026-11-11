/*
 * the main class of the ability
 * it can include many functionality components and effect components
 * 
 * fangruyi@snda.com
 * 
 * 
 */
class MMOAbility extends Object implements(IExcelable, IReferenceID)
	native
	hidecategories(Object)
	hidecategories(Preview);


enum EPoseStatusType
{
	EPST_Standing,
	EPST_Flying,
	EPST_Lying,
};


enum ETargetSelectType
{
	TST_Default,
	TST_Aim,
	TST_Area,
};

//enum ESpell_CastType
//{
//	ESCa_Type1 <DisplayName="SCa_Type1">, //spctPassive
//	ESCa_Type2 <DisplayName="SCa_Type2">,//spctActive
//	ESCa_Type3 <DisplayName="SCa_Type3">,  // sub ability
//	ESCa_Type4 <DisplayName="SCa_Type4">,  // direct ability
//};

enum ESpell_QTEType
{
	ESQTE_Type1 <DisplayName="SQTE_Type1">, 
	ESQTE_Type2 <DisplayName="SQTE_Type2">,
	ESQTE_Type3 <DisplayName="SQTE_Type3">,
	ESQTE_Type4 <DisplayName="SQTE_Type4">,  
};


//enum ESpell_JudgeType
//{
//	ESJu_Type1 <DisplayName="SJu_Type1">,
//	ESJu_Type2 <DisplayName="SJu_Type2">,
//	ESJu_Type3 <DisplayName="SJu_Type3">,
//	ESJu_Type4 <DisplayName="SJu_Type4">,
//};

//enum ESpell_AttackType
//{
//	ESA_Type1 <DisplayName="SA_Type1">,
//	ESA_Type2 <DisplayName="SA_Type2">,	
//	ESA_Type3 <DisplayName="SA_Type3">,	
//	ESA_Type4 <DisplayName="SA_Type4">,	
//};

//enum ESpell_LockType
//{
//	ESL_Type1 <DisplayName="SL_Type1">,
//	ESL_Type2 <DisplayName="SL_Type2">,
//	ESL_Type3 <DisplayName="SL_Type3">,
//	ESL_Type4 <DisplayName="SL_Type4">,
//	ESL_Type5 <DisplayName="SL_Type5">,
//};

//enum ESpell_RangeType
//{
//	ESR_Type1 <DisplayName="SR_Type1">,
//	ESR_Type2 <DisplayName="SR_Type2">,
//	ESR_Type3 <DisplayName="SR_Type3">,
//	ESR_Type4 <DisplayName="SR_Type4">,
//	ESR_Type5 <DisplayName="SR_Type5">,
//	ESR_Type6 <DisplayName="SR_Type6">,
//};

enum ESpell_PrepareType
{
	ES_PT_Type1 <DisplayName="S_PT_Type1">,
	ES_PT_Type2 <DisplayName="S_PT_Type2">,
	ES_PT_Type3 <DisplayName="S_PT_Type3">,
	ES_PT_Type4 <DisplayName="S_PT_Type4">,
	ES_PT_Type5 <DisplayName="S_PT_Type5">,
};

enum ESpell_HitTag
{
	ESH_Type1 <DisplayName="SH_Type1">,
	ESH_Type2 <DisplayName="SH_Type2">,
	ESH_Type3 <DisplayName="SH_Type3">,
	ESH_Type4 <DisplayName="SH_Type4">,
	
};

enum ESpell_CostType
{
	ES_COST_Type1 <DisplayName="SPELL_COST_Type1">,
	ES_COST_Type2 <DisplayName="SPELL_COST_Type2">,
	ES_COST_Type3 <DisplayName="SPELL_COST_Type3">,
	ES_COST_Type4 <DisplayName="SPELL_COST_Type4">,
};

//enum ESpell_CancelType
//{
//	ES_CT_Type1 <DisplayName="S_CT_Type1">,
//	ES_CT_Type2 <DisplayName="S_CT_Type2">,
//	ES_CT_Type3 <DisplayName="S_CT_Type3">,
//};

enum ESpell_CDTriggerType
{
	ES_CD_TRI_Type1 <DisplayName="S_CD_TRI_Type1">,
	ES_CD_TRI_Type2 <DisplayName="S_CD_TRI_Type2">,
};

struct native HitGroup
{
	var()   EWeaponType WeaponType				<DisplayName="WeaponType">;
	var()   EWeaponHitType  WeaponHitType		<DisplayName="WeaponHitType">;
	var()   EHitDirectionType   DirectionType	<DisplayName="DirectionType">;
	var     byte NoUse;
};

struct native ComponentNode
{
	var(FuncNode)   float StartTime								<DisplayName="StartTime">;
	var(FuncNode) editinline   AbilityComponent    Component	<DisplayName="Component">;
};

struct native ComponentNodePair
{
	var() EWeaponType Type;
	var() array<ComponentNode>  OverrideSpellComponentList;
	var() array<ComponentNode>  OverrideCastComponentList;
};

struct native Spell_ClassType
{
	var(Class) bool bClassType1 <ExportExcel="0"|DisplayName="ClassType1">;
	var(Class) bool bClassType2 <ExportExcel="1"|DisplayName="ClassType2">;
	var(Class) bool bClassType3 <ExportExcel="2"|DisplayName="ClassType3">;
	var(Class) bool bClassType4 <ExportExcel="3"|DisplayName="ClassType4">;
	var(Class) bool bClassType5 <ExportExcel="4"|DisplayName="ClassType5">;
	var(Class) bool bClassType6 <ExportExcel="5"|DisplayName="ClassType6">;
	var(Class) bool bClassType7 <ExportExcel="6"|DisplayName="ClassType7">; // Lancer (1.05)
	var(Class) bool bClassType8 <ExportExcel="7"|DisplayName="ClassType8">; // Sorcerer (1.05)
};

struct native Spell_WeaponTypeR
{
	var(WeaponType) bool bWeaponTypeR0  <ExportExcel="0"|DisplayName="WeaponTypeR0">;
	var(WeaponType) bool bWeaponTypeR1  <ExportExcel="1"|DisplayName="WeaponTypeR1">;
	var(WeaponType) bool bWeaponTypeR2  <ExportExcel="2"|DisplayName="WeaponTypeR2">;
	var(WeaponType) bool bWeaponTypeR3  <ExportExcel="3"|DisplayName="WeaponTypeR3">;
	var(WeaponType) bool bWeaponTypeR4  <ExportExcel="4"|DisplayName="WeaponTypeR4">;
	var(WeaponType) bool bWeaponTypeR5  <ExportExcel="5"|DisplayName="WeaponTypeR5">;
	var(WeaponType) bool bWeaponTypeR6  <ExportExcel="6"|DisplayName="WeaponTypeR6">;
	var(WeaponType) bool bWeaponTypeR7  <ExportExcel="7"|DisplayName="WeaponTypeR7">;
	var(WeaponType) bool bWeaponTypeR8  <ExportExcel="8"|DisplayName="WeaponTypeR8">;
	var(WeaponType) bool bWeaponTypeR9  <ExportExcel="9"|DisplayName="WeaponTypeR9">;
	var(WeaponType) bool bWeaponTypeR10 <ExportExcel="10"|DisplayName="WeaponTypeR10">;
	var(WeaponType) bool bWeaponTypeR11 <ExportExcel="11"|DisplayName="WeaponTypeR11">;
	var(WeaponType) bool bWeaponTypeR12 <ExportExcel="12"|DisplayName="WeaponTypeR12">;
	var(WeaponType) bool bWeaponTypeR13 <ExportExcel="13"|DisplayName="WeaponTypeR13">;
	var(WeaponType) bool bWeaponTypeR14 <ExportExcel="14"|DisplayName="WeaponTypeR14">;

	var(WeaponType) bool bWeaponTypeR15 <ExportExcel="15"|DisplayName="WeaponTypeR15">;
	var(WeaponType) bool bWeaponTypeR16 <ExportExcel="16"|DisplayName="WeaponTypeR16">;
	var(WeaponType) bool bWeaponTypeR17 <ExportExcel="17"|DisplayName="WeaponTypeR17">;
};

struct native Spell_WeaponTypeL
{
	var(WeaponType) bool bWeaponTypeL0  <ExportExcel="0"|DisplayName="WeaponTypeL0">;
	var bool bWeaponTypeL1  <ExportExcel="1"|DisplayName="WeaponTypeL1">;
	var bool bWeaponTypeL2  <ExportExcel="2"|DisplayName="WeaponTypeL2">;
	var bool bWeaponTypeL3  <ExportExcel="3"|DisplayName="WeaponTypeL3">;
	var bool bWeaponTypeL4  <ExportExcel="4"|DisplayName="WeaponTypeL4">;
	var bool bWeaponTypeL5  <ExportExcel="5"|DisplayName="WeaponTypeL5">;
	var bool bWeaponTypeL6  <ExportExcel="6"|DisplayName="WeaponTypeL6">;
	var bool bWeaponTypeL7  <ExportExcel="7"|DisplayName="WeaponTypeL7">;
	var bool bWeaponTypeL8  <ExportExcel="8"|DisplayName="WeaponTypeL8">;
	var bool bWeaponTypeL9  <ExportExcel="9"|DisplayName="WeaponTypeL9">;
	var bool bWeaponTypeL10 <ExportExcel="10"|DisplayName="WeaponTypeL10">;
	var bool bWeaponTypeL11 <ExportExcel="11"|DisplayName="WeaponTypeL11">;
	var bool bWeaponTypeL12 <ExportExcel="12"|DisplayName="WeaponTypeL12">;
	var bool bWeaponTypeL13 <ExportExcel="13"|DisplayName="WeaponTypeL13">;
	var bool bWeaponTypeL14 <ExportExcel="14"|DisplayName="WeaponTypeL14">;

	var(WeaponType) bool bWeaponTypeL15 <ExportExcel="15"|DisplayName="WeaponTypeL15">;
	var(WeaponType) bool bWeaponTypeL16 <ExportExcel="16"|DisplayName="WeaponTypeL16">;
	var(WeaponType) bool bWeaponTypeL17 <ExportExcel="17"|DisplayName="WeaponTypeL17">;
};

struct native Spell_BindSlot
{
	var(BindSlot) bool bNormalAtk	<ExportExcel="0"|DisplayName="NormalAtk">;
	var(BindSlot) bool bBossQTE		<ExportExcel="1"|DisplayName="BossQTE">;
	var(BindSlot) bool bPassive1	<ExportExcel="2"|DisplayName="Passive1">;
	var(BindSlot) bool bPassive2	<ExportExcel="3"|DisplayName="Passive2">;
	var(BindSlot) bool bPassive3	<ExportExcel="4"|DisplayName="Passive3">;

	var(BindSlot) bool bKey0		<ExportExcel="5"|DisplayName="Key0">;
	var(BindSlot) bool bKey1		<ExportExcel="6"|DisplayName="Key1">;
	var(BindSlot) bool bKey2		<ExportExcel="7"|DisplayName="Key2">;
	var(BindSlot) bool bKey3		<ExportExcel="8"|DisplayName="Key3">;
	var(BindSlot) bool bKey4		<ExportExcel="9"|DisplayName="Key4">;
	var(BindSlot) bool bKey5		<ExportExcel="10"|DisplayName="Key5">;
	var(BindSlot) bool bKey6		<ExportExcel="11"|DisplayName="Key6">;
	var(BindSlot) bool bKey7		<ExportExcel="12"|DisplayName="Key7">;
	var(BindSlot) bool bKey8		<ExportExcel="13"|DisplayName="Key8">;
	var(BindSlot) bool bKey9		<ExportExcel="14"|DisplayName="Key9">;
	var(BindSlot) bool bKey10		<ExportExcel="15"|DisplayName="Key10">;
	var(BindSlot) bool bKey11		<ExportExcel="16"|DisplayName="Key11">;
};


struct native Spell_DamageType
{
	var(DamageType) bool bDamageTypeType1 <ExportExcel="0"|DisplayName="DamageTypeType1">;
	var(DamageType) bool bDamageTypeType2 <ExportExcel="1"|DisplayName="DamageTypeType2">;
	var(DamageType) bool bDamageTypeType3 <ExportExcel="2"|DisplayName="DamageTypeType3">;
	var(DamageType) bool bDamageTypeType4 <ExportExcel="3"|DisplayName="DamageTypeType4">;
	var(DamageType) bool bDamageTypeType5 <ExportExcel="4"|DisplayName="DamageTypeType5">;
	var(DamageType) bool bDamageTypeType6 <ExportExcel="5"|DisplayName="DamageTypeType6">;
	var(DamageType) bool bDamageTypeType7 <ExportExcel="6"|DisplayName="DamageTypeType7">;
	var(DamageType) bool bDamageTypeType8 <ExportExcel="7"|DisplayName="DamageTypeType8">;
};

struct native Spell_TargetObjectType
{
	var(TargetObjectType) bool bTargetObjectType1 <ExportExcel="0"|DisplayName="TargetObjectType1">;
	var(TargetObjectType) bool bTargetObjectType2 <ExportExcel="1"|DisplayName="TargetObjectType2">;
	var(TargetObjectType) bool bTargetObjectType3 <ExportExcel="2"|DisplayName="TargetObjectType3">;
	var(TargetObjectType) bool bTargetObjectType4 <ExportExcel="3"|DisplayName="TargetObjectType4">;
	var(TargetObjectType) bool bTargetObjectType5 <ExportExcel="4"|DisplayName="TargetObjectType5">;

	structdefaultproperties
	{
		bTargetObjectType1=true
		bTargetObjectType2=true
	}

};

struct native Scene_Difficulty_Limit
{
	var(SceneDifficultyLimit) bool bSceneLevel0 <ExportExcel="0"|DisplayName="SceneLevel0">;
	var(SceneDifficultyLimit) bool bSceneLevel1 <ExportExcel="1"|DisplayName="SceneLevel1">;
	var(SceneDifficultyLimit) bool bSceneLevel2 <ExportExcel="2"|DisplayName="SceneLevel2">;
	var(SceneDifficultyLimit) bool bSceneLevel3 <ExportExcel="3"|DisplayName="SceneLevel3">;
	var(SceneDifficultyLimit) bool bSceneLevel4 <ExportExcel="4"|DisplayName="SceneLevel4">;
	var(SceneDifficultyLimit) bool bSceneLevel5 <ExportExcel="5"|DisplayName="SceneLevel5">;

	structdefaultproperties
	{
		bSceneLevel0=false
		bSceneLevel1=true
		bSceneLevel2=true
		bSceneLevel3=true
		bSceneLevel4=true
		bSceneLevel5=true
	}
};

struct native Scene_Type_Limit
{
	var(SceneTypeLimit) bool bSceneType0 <ExportExcel="0"|DisplayName="SceneType0">;
	var(SceneTypeLimit) bool bSceneType1 <ExportExcel="1"|DisplayName="SceneType1">;
	var(SceneTypeLimit) bool bSceneType2 <ExportExcel="2"|DisplayName="SceneType2">;
	var(SceneTypeLimit) bool bSceneType3 <ExportExcel="3"|DisplayName="SceneType3">;
	var(SceneTypeLimit) bool bSceneType4 <ExportExcel="4"|DisplayName="SceneType4">;
	var(SceneTypeLimit) bool bSceneType5 <ExportExcel="5"|DisplayName="SceneType5">;
	var(SceneTypeLimit) bool bSceneType6 <ExportExcel="6"|DisplayName="SceneType6">;
	var(SceneTypeLimit) bool bSceneType7 <ExportExcel="7"|DisplayName="SceneType7">;
	var(SceneTypeLimit) bool bSceneType8 <ExportExcel="8"|DisplayName="SceneType8">;

	structdefaultproperties
	{
		bSceneType0=false
		bSceneType1=true
		bSceneType2=true
		bSceneType3=true
		bSceneType4=true
		bSceneType5=true
		bSceneType6=true
		bSceneType7=true
		bSceneType8=true
	}
};


struct native Spell_TargetRelationship
{
	var(TargetRelationship) bool bTargetRelationshipType1 <ExportExcel="0"|DisplayName="TargetRelationshipType1">;
	var(TargetRelationship) bool bTargetRelationshipType2 <ExportExcel="1"|DisplayName="TargetRelationshipType2">;
	var(TargetRelationship) bool bTargetRelationshipType3 <ExportExcel="2"|DisplayName="TargetRelationshipType3">;

	structdefaultproperties
	{
		bTargetRelationshipType3=true
	}

};

struct native Spell_TargetRace
{
	var(TargetRace) bool bTargetRaceType1 <ExportExcel="0"|DisplayName="TargetRaceType1">;
	var(TargetRace) bool bTargetRaceType2 <ExportExcel="1"|DisplayName="TargetRaceType2">;
	var(TargetRace) bool bTargetRaceType3 <ExportExcel="2"|DisplayName="TargetRaceType3">;
	var(TargetRace) bool bTargetRaceType4 <ExportExcel="3"|DisplayName="TargetRaceType4">;
	var(TargetRace) bool bTargetRaceType5 <ExportExcel="4"|DisplayName="TargetRaceType5">;
	var(TargetRace) bool bTargetRaceType6 <ExportExcel="5"|DisplayName="TargetRaceType6">;
	var(TargetRace) bool bTargetRaceType7 <ExportExcel="6"|DisplayName="TargetRaceType7">;
	var(TargetRace) bool bTargetRaceType8 <ExportExcel="7"|DisplayName="TargetRaceType8">;
	var(TargetRace) bool bTargetRaceType9 <ExportExcel="8"|DisplayName="TargetRaceType9">;
	var(TargetRace) bool bTargetRaceType10 <ExportExcel="9"|DisplayName="TargetRaceType10">;
	var(TargetRace) bool bTargetRaceType11 <ExportExcel="10"|DisplayName="TargetRaceType11">;
	var(TargetRace) bool bTargetRaceType12 <ExportExcel="11"|DisplayName="TargetRaceType12">;

	structdefaultproperties
	{
		bTargetRaceType1=true 
		bTargetRaceType2=true 
		bTargetRaceType3=true 
		bTargetRaceType4=true 
		bTargetRaceType5=true 
		bTargetRaceType6=true 
		bTargetRaceType7=true 
		bTargetRaceType8=true 
		bTargetRaceType9=true 
		bTargetRaceType10=true
		bTargetRaceType11=true
		bTargetRaceType12=true
	}


};

struct native Spell_TargetSex
{
	var(TargetSex) bool bTargetSex1 <ExportExcel="0"|DisplayName="TargetSex1">;
	var(TargetSex) bool bTargetSex2 <ExportExcel="1"|DisplayName="TargetSex2">;
	var(TargetSex) bool bTargetSex3 <ExportExcel="2"|DisplayName="TargetSex3">;

	structdefaultproperties
	{
	bTargetSex1=true
	bTargetSex2=true
	bTargetSex3=true
	}

};

struct native Spell_TargetDirection
{
	var(TargetDirection) bool bTargetDirection1 <ExportExcel="0"|DisplayName="TargetDirection1">;
	var(TargetDirection) bool bTargetDirection2 <ExportExcel="1"|DisplayName="TargetDirection2">;

	structdefaultproperties
	{
	bTargetDirection1=true
	bTargetDirection2=true
	}

};

struct native Spell_SelfStance
{
	var(SelfStance) bool bSelfStance1 <ExportExcel="0"|DisplayName="SelfStance1">;
	var(SelfStance) bool bSelfStance2 <ExportExcel="1"|DisplayName="SelfStance2">;
	var(SelfStance) bool bSelfStance3 <ExportExcel="2"|DisplayName="SelfStance3">;

	structdefaultproperties
	{
	bSelfStance1 =true
	bSelfStance2 =true
	bSelfStance3 =true
	}

};

struct native Spell_TargetStance
{
	var(TargetStance) bool bTargetStance1 <ExportExcel="0"|DisplayName="TargetStance1">;
	var(TargetStance) bool bTargetStance2 <ExportExcel="1"|DisplayName="TargetStance2">;
	var(TargetStance) bool bTargetStance3 <ExportExcel="2"|DisplayName="TargetStance3">;

	structdefaultproperties
	{
	bTargetStance1 =true
	bTargetStance2 =true
	bTargetStance3 =true
	}

};

struct native Spell_ControlTagType
{
	var(ControlTag) bool bControlTagType1 <ExportExcel="0"|DisplayName="ControlTagType1">;
	var(ControlTag) bool bControlTagType2 <ExportExcel="1"|DisplayName="ControlTagType2">;

	structdefaultproperties
	{
		bControlTagType1=true
		bControlTagType2=true
	}

};

struct native Spell_CCLimitBitsType
{
	var() bool bCCLimitBitsType0 <ExportExcel="0"|DisplayName="CCLimitBitsType0">;
	var() bool bCCLimitBitsType1 <ExportExcel="1"|DisplayName="CCLimitBitsType1">;
	var() bool bCCLimitBitsType2 <ExportExcel="2"|DisplayName="CCLimitBitsType2">;
	var() bool bCCLimitBitsType3 <ExportExcel="3"|DisplayName="CCLimitBitsType3">;
	var() bool bCCLimitBitsType4 <ExportExcel="4"|DisplayName="CCLimitBitsType4">;
	var() bool bCCLimitBitsType5 <ExportExcel="5"|DisplayName="CCLimitBitsType5">;
	var() bool bCCLimitBitsType6 <ExportExcel="6"|DisplayName="CCLimitBitsType6">;
	var() bool bCCLimitBitsType7 <ExportExcel="7"|DisplayName="CCLimitBitsType7">;
	var() bool bCCLimitBitsType8 <ExportExcel="8"|DisplayName="CCLimitBitsType8">;
	var() bool bCCLimitBitsType9 <ExportExcel="9"|DisplayName="CCLimitBitsType9">;
	var() bool bCCLimitBitsType10 <ExportExcel="10"|DisplayName="CCLimitBitsType10">;
	var() bool bCCLimitBitsType11 <ExportExcel="11"|DisplayName="CCLimitBitsType11">;
	var() bool bCCLimitBitsType12 <ExportExcel="12"|DisplayName="CCLimitBitsType12">;
	var() bool bCCLimitBitsType13 <ExportExcel="13"|DisplayName="CCLimitBitsType13">;

	structdefaultproperties
	{
		bCCLimitBitsType0=true;
		bCCLimitBitsType1=true;
		bCCLimitBitsType2=true;
		bCCLimitBitsType3=true;
		bCCLimitBitsType4=true;
		bCCLimitBitsType5=true;
		bCCLimitBitsType6=false;
		bCCLimitBitsType7=true;
		bCCLimitBitsType8=false;
		bCCLimitBitsType9=false;
		bCCLimitBitsType10=false;
		bCCLimitBitsType11=false;
		bCCLimitBitsType12=false;
		bCCLimitBitsType13=false;
	}
};

struct native Spell_LimitBitsType
{
	var() bool bLimitBitsType0 <ExportExcel="0"|DisplayName="LimitBitsType0">;
	var() bool bLimitBitsType1 <ExportExcel="1"|DisplayName="LimitBitsType1">;
	var() bool bLimitBitsType2 <ExportExcel="2"|DisplayName="LimitBitsType2">;
	var() bool bLimitBitsType3 <ExportExcel="3"|DisplayName="LimitBitsType3">;
};

struct native Spell_DaytimeLimit
{
	var() bool bLimitBitsType0 <ExportExcel="0"|DisplayName="Daytime">;
	var() bool bLimitBitsType1 <ExportExcel="1"|DisplayName="Night">;

	structdefaultproperties
	{
		bLimitBitsType0=true;
		bLimitBitsType1=true;
	}
};

struct native Spell_WeatherLimit
{
	var() bool bLimitBitsType0 <ExportExcel="0"|DisplayName="swtNormal">;
	var() bool bLimitBitsType1 <ExportExcel="1"|DisplayName="swtFog">;
	var() bool bLimitBitsType2 <ExportExcel="2"|DisplayName="swtThunder">;
	var() bool bLimitBitsType3 <ExportExcel="3"|DisplayName="swtRain">;
	var() bool bLimitBitsType4 <ExportExcel="4"|DisplayName="swtSnow">;
	var() bool bLimitBitsType5 <ExportExcel="5"|DisplayName="swtStorm">;
	var() bool bLimitBitsType6 <ExportExcel="6"|DisplayName="swtEclipse">;

	structdefaultproperties
	{
		bLimitBitsType0=true;
		bLimitBitsType1=true;
		bLimitBitsType2=true;
		bLimitBitsType3=true;
		bLimitBitsType4=true;
		bLimitBitsType5=true;
		bLimitBitsType6=true;
	}
};


struct native ClientAttribute
{
	var()   string  AbilityName							<ExportExcel="0"|DisplayName="AbilityName">;
	var()   bool    bShowAbilityName                    <ExportExcel="Client">;
	var()   string  AbilityDescribe						<ExportExcel="1"|DisplayName="AbilityDescribe">;
	//var()   int IconId;
	var()   Texture2D     DisplayIcon					<ExportExcel="2"|DisplayName="DisplayIcon">;
	var()   bool    bRotateToMousePoint					<ExportExcel="3"|DisplayName="RotateToMousePoint">;
	var()   bool    bNormalAttack						<ExportExcel="4"|DisplayName="NormalAttack">;
	// hit effect
	var()   bool    bUseHitEffectByWeaponBodyMatrialType<ExportExcel="5"|DisplayName="UseHitEffectByWeaponBodyMatrialType">;
	// 每秒转的度数
	var()   float   RotateSpeed							<ExportExcel="6"|DisplayName="RotateSpeed">;

	//var()   Spell_ControlTagType ControlTag;
	//var()   Spell_ControlTagType CastControlTag;

	var()   array<HitGroup>  HitGroupInfo				<DisplayName="HitGroupInfo">;
	var()   bool ShowOnUI								<ExportExcel="7"|DisplayName="ShowOnUI">; //勾上时则不在UI上显示
	var()   bool bShowLeadUI							<ExportExcel="8"|DisplayName="ShowLeadUI">;   // 是否显示引导条
	var()   bool CanLMouseEquip							<ExportExcel="9"|DisplayName="CanLMouseEquip">; //左键是否可以装配 
	var()   bool bRequestCancelOnRelease				<ExportExcel="10"|DisplayName="RequestCancelOnRelease">;
	var()   bool PickTarget								<ExportExcel="11"|DisplayName="PickTarget">;
	var()   bool GenerateLockStatus;
	var()   bool TargetSelectActor;
	var()   float ModifyCD;                 
	var()   bool bHideWeaponInSpell;
	var()   bool bHideWeaponInCast;
	var()   bool UseDefaultAbility;
	var()   ETargetSelectType    TargetSelectType;

	structdefaultproperties
	{
		RotateSpeed=983040
		bUseHitEffectByWeaponBodyMatrialType=true
		bShowAbilityName=false
		bNormalAttack=false
		ShowOnUI=true
		bShowLeadUI=true
		CanLMouseEquip=true
		bRequestCancelOnRelease=false
		PickTarget = false
		GenerateLockStatus = false;
		TargetSelectActor  = false;
		UseDefaultAbility = false; 
		ModifyCD = 0.0
	}
};

struct native AbilitySpellCommonAttribute
{
	var(SpellCommonAttribute)    int Id												<ExportExcel="0"|DisplayName="Id">;
	var(SpellCommonAttribute)    int Level											<DisplayName="Level">;
	var(SpellCommonAttribute)    int SearchDistance									<DisplayName="SearchDistance">;
	var(SpellCommonAttribute)    Spell_ClassType Class								<ExportExcel="Both"|Merger="0"|DisplayName="Class">;
	var(SpellCommonAttribute)    int RequireLevel									<ExportExcel="Both"|Merger="0"|DisplayName="RequireLevel">;
	var(SpellCommonAttribute)    int RuneSlotID										<ExportExcel="Both"|DisplayName="RuneSlotID">;
	var(SpellCommonAttribute)    Spell_BindSlot BindSlot							<ExportExcel="Both"|Merger="0"|DisplayName="BindSlot">;
	var(SpellCommonAttribute)    int NextSpell										<ExportExcel="Both"|DisplayName="NextSpell">;
	var(SpellCommonAttribute)    int SwitchSpell									<ExportExcel="Both"|Merger="0"|DisplayName="SwitchSpell">;
	var(SpellCommonAttribute)    float ResetTime									<ExportExcel="Both"|Convert="1000"|DisplayName="ResetTime">;
	var(SpellCommonAttribute)    float CacheTime									<ExportExcel="Both"|Convert="1000"|DisplayName="CacheTime">;
	var(SpellCommonAttribute)    bool NeedCache										<ExportExcel="Both"|DisplayName="NeedCache">;
	var(SpellCommonAttribute)    Spell_WeaponTypeR	WeaponTypeR						<ExportExcel="Both"|Merger="0"|DisplayName="WeaponTypeR">;
	var(SpellCommonAttribute)    Spell_WeaponTypeL	WeaponTypeL						<ExportExcel="Both"|Merger="0"|DisplayName="WeaponTypeL">;
	var(SpellCommonAttribute)    bool CanEquip										<ExportExcel="Both"|DisplayName="CanEquip">;
	var(SpellCommonAttribute)    bool TrigEvent										<ExportExcel="Both"|DisplayName="TrigEvent">;
	var(SpellCommonAttribute)    ESPELL_QTE_TYPE QTEType							<ExportExcel="Both"|DisplayName="QTEType">;
	var(SpellCommonAttribute)    ESPELL_CAST_TYPE CastType							<ExportExcel="Both"|DisplayName="CastType">;
	var(SpellCommonAttribute)    ESPELL_JUDGE_TYPE JudgeType						<ExportExcel="Both"|DisplayName="JudgeType">;
	var(SpellCommonAttribute)    ESPELL_ATTACK_TYPE AttackType						<ExportExcel="Both"|DisplayName="AttackType">;
	var(SpellCommonAttribute)    ESPELL_DAMAGE_TYPE ElementaryType					<ExportExcel="Both"|DisplayName="ElementaryType">;
	var(SpellCommonAttribute)    ESPELL_LOCK_TYPE LockType							<ExportExcel="Both"|DisplayName="LockType">;
	var(SpellCommonAttribute)    int MinDistance									<ExportExcel="Both"|DisplayName="MinDistance">;
	var(SpellCommonAttribute)    int MinGrids										<ExportExcel="Both"|DisplayName="MinGrids">;
	var(SpellCommonAttribute)    int MaxDistance									<ExportExcel="Both"|DisplayName="MaxDistance">;
	var(SpellCommonAttribute)    int MaxGrids										<ExportExcel="Both"|DisplayName="MaxGrids">;
	var(SpellCommonAttribute)    int TolerantDistance								<ExportExcel="Both"|DisplayName="TolerantDistance">;
	var(SpellCommonAttribute)    int TolerantGrids									<ExportExcel="Both"|DisplayName="TolerantGrids">;
	var(SpellCommonAttribute)    ESPELL_RANGE_TYPE RangeType						<ExportExcel="Both"|DisplayName="RangeType">;
	var(SpellCommonAttribute)    int RangeParam1									<ExportExcel="Both"|DisplayName="RangeParam1">;
	var(SpellCommonAttribute)    int RangeParam2									<ExportExcel="Both"|DisplayName="RangeParam2">;
	var(SpellCommonAttribute)    int RangeParam3									<ExportExcel="Both"|DisplayName="RangeParam3">;
	var(SpellCommonAttribute)    bool IgnoreObstacle								<ExportExcel="Both"|DisplayName="IgnoreObstacle">;
	var(SpellCommonAttribute)    bool UseWeaponRange								<ExportExcel="Both"|DisplayName="UseWeaponRange">;
	var(SpellCommonAttribute)    bool IgnoreTargetRadius							<ExportExcel="Both"|DisplayName="IgnoreTargetRadius">;
	var(SpellCommonAttribute)    bool AffectedByAtkSpeed							<ExportExcel="Both"|DisplayName="AffectedByAtkSpeed">;
	var(SpellCommonAttribute)    bool UseDPH            							<ExportExcel="Both"|DisplayName="UseDPH">;
	var(SpellCommonAttribute)    bool IgnoreDefense            						<ExportExcel="Both"|DisplayName="IgnoreDefense">;
	var(SpellCommonAttribute)    bool IgnoreClientTarget       						<ExportExcel="Both"|DisplayName="IgnoreClientTarget">;


	var(SpellCommonAttribute)    int MaxTargetCount									<ExportExcel="Both"|DisplayName="MaxTargetCount">;
	var(SpellCommonAttribute)    int OffsetDirection								<ExportExcel="Both"|DisplayName="OffsetDirection">;
	var(SpellCommonAttribute)    int OffsetDistance									<ExportExcel="Both"|DisplayName="OffsetDistance">;

	var(SpellCommonAttribute)    int HOffsetGrids									<ExportExcel="Both"|DisplayName="HOffsetGrids">;
	var(SpellCommonAttribute)    int VOffsetGrids									<ExportExcel="Both"|DisplayName="VOffsetGrids">;

	var(SpellCommonAttribute)    Scene_Difficulty_Limit SceneDifficultyLimit       	<ExportExcel="Both"|Merger="0"|DisplayName="SceneDifficultyLimit">;
	var(SpellCommonAttribute)    Scene_Type_Limit SceneTypeLimit                  	<ExportExcel="Both"|Merger="0"|DisplayName="SceneTypeLimit">;
	var(SpellCommonAttribute)    Spell_TargetObjectType TargetObjectType			<ExportExcel="Both"|Merger="0"|DisplayName="TargetObjectType">;
	var(SpellCommonAttribute)    Spell_TargetRelationship TargetRelationship		<ExportExcel="Both"|Merger="0"|DisplayName="TargetRelationship">;
	var(SpellCommonAttribute)    Spell_TargetRace TargetRace						<ExportExcel="Both"|Merger="0"|DisplayName="TargetRace">;
	var(SpellCommonAttribute)	 Spell_TargetSex TargetSex							<ExportExcel="Both"|Merger="0"|DisplayName="TargetSex">;
	var(SpellCommonAttribute)	 Spell_TargetDirection TargetDirection				<ExportExcel="Both"|Merger="0"|DisplayName="TargetDirection">;
	var(SpellCommonAttribute)	 Spell_TargetStance TargetStance					<ExportExcel="Both"|Merger="0"|DisplayName="TargetStance">;
	var(SpellCommonAttribute)    Spell_SelfStance SelfStance						<ExportExcel="Both"|Merger="0"|DisplayName="SelfStance">;
	var(SpellCommonAttribute)    ESpell_CostType CostType							<ExportExcel="Both"|DisplayName="CostType">;
	var(SpellCommonAttribute)    int InitialCostValue								<ExportExcel="Both"|DisplayName="InitialCostValue">;
	var(SpellCommonAttribute)    int InitialCostValuePct							<ExportExcel="Both"|DisplayName="InitialCostValuePct">;
	var(SpellCommonAttribute)    int DurativeCostValue								<ExportExcel="Both"|DisplayName="DurativeCostValue">;
	var(SpellCommonAttribute)    int DurativeCostValuePct							<ExportExcel="Both"|DisplayName="DurativeCostValuePct">;
	var(SpellCommonAttribute)    int ModifierCostValue								<ExportExcel="Both"|DisplayName="ModifierCostValue">;
	var(SpellCommonAttribute)    int ModifierCostValuePct							<ExportExcel="Both"|DisplayName="ModifierCostValuePct">;
	var(SpellCommonAttribute)    int ItemCostID										<ExportExcel="Both"|DisplayName="ItemCostID">;
	var(SpellCommonAttribute)    string ItemCostType								<ExportExcel="Both"|DisplayName="ItemCostType">;
	var(SpellCommonAttribute)    string ItemCostSubType								<ExportExcel="Both"|DisplayName="ItemCostSubType">;
	var(SpellCommonAttribute)    int ItemCostCount									<ExportExcel="Both"|DisplayName="ItemCostCount">;
	var(SpellCommonAttribute)    bool ConsumeItem									<ExportExcel="Both"|DisplayName="ConsumeItem">;



	var(SpellCommonAttribute)    bool DuraCost										<ExportExcel="Both"|DisplayName="DuraCost">;

	var(SpellCommonAttribute)    ESpell_CDTriggerType CoolDownTriggerType			<ExportExcel="Both"|DisplayName="CoolDownTriggerType">;
	var(SpellCommonAttribute)    float SelfCoolDown									<ExportExcel="Both"|Convert="1000"|DisplayName="SelfCoolDown">;
	var(SpellCommonAttribute)    float EventCoolDown								<ExportExcel="Both"|Convert="1000"|DisplayName="EventCoolDown">;
	var(SpellCommonAttribute)    string GroupCoolDown								<ExportExcel="Both"|DisplayName="GroupCoolDown">;
	var(SpellCommonAttribute)    ESpell_PrepareType PrepareType						<ExportExcel="Both"|DisplayName="PrepareType">;
	var(SpellCommonAttribute)    bool AdjustFaceDirection							<ExportExcel="Both"|DisplayName="AdjustFaceDirection">;
	var(SpellCommonAttribute)    bool AffectedBySpeedParam							<ExportExcel="Both"|DisplayName="AffectedBySpeedParam">;
	var(SpellCommonAttribute)    float PrepareTimeMin								<ExportExcel="Both"|Convert="1000"|DisplayName="PrepareTimeMin">;
	var(SpellCommonAttribute)    float PrepareTimeMax								<ExportExcel="Both"|Convert="1000"|DisplayName="PrepareTimeMax">;
	var(SpellCommonAttribute)    float AccumulateTimeMax							<ExportExcel="Both"|Convert="1000"|DisplayName="AccumulateTimeMax">;
	var(SpellCommonAttribute)    int AccumulateSeg									<ExportExcel="Both"|DisplayName="AccumulateSeg">;
	var(SpellCommonAttribute)    int ChannelCount									<ExportExcel="Both"|DisplayName="ChannelCount">;
	var(SpellCommonAttribute)    int MinEffectCount									<ExportExcel="Both"|DisplayName="MinEffectCount">;
	var(SpellCommonAttribute)    float Interval										<ExportExcel="Both"|Convert="1000"|DisplayName="Interval">;
	var(SpellCommonAttribute)    float HardTime										<ExportExcel="Both"|Convert="1000"|DisplayName="HardTime">;
	var(SpellCommonAttribute)    float WalkDelayTime								<ExportExcel="Both"|Convert="1000"|DisplayName="WalkDelayTime">;
	var(SpellCommonAttribute)    float RunDelayTime									<ExportExcel="Both"|Convert="1000"|DisplayName="RunDelayTime">;
	var(SpellCommonAttribute)    int Power											<ExportExcel="Both"|DisplayName="Power">;
	var(SpellCommonAttribute)    int HitLevel										<ExportExcel="Both"|DisplayName="HitLevel">;
	var(SpellCommonAttribute)    bool MoveBreak										<ExportExcel="Both"|DisplayName="MoveBreak">;
	var(SpellCommonAttribute)    bool BreakMove										<ExportExcel="Both"|DisplayName="BreakMove">;
	var(SpellCommonAttribute)    bool CanMoveInPrepare								<ExportExcel="Both"|DisplayName="CanMoveInPrepare">;
	var(SpellCommonAttribute)    bool CanMoveInHardTime								<ExportExcel="Both"|DisplayName="CanMoveInHardTime">;
	var(SpellCommonAttribute)    bool HardTimeAffectedByCharLevel					<ExportExcel="Both"|DisplayName="HardTimeAffectedByCharLevel">;

	var(SpellCommonAttribute)    bool HitBreak										<ExportExcel="Both"|DisplayName="HitBreak">;
	var(SpellCommonAttribute)    ESPELL_CANCEL_TYPE CancelType						<ExportExcel="Both"|DisplayName="CancelType">;
	var(SpellCommonAttribute)    int CancelLevel									<ExportExcel="Both"|DisplayName="CancelLevel">;
	var(SpellCommonAttribute)    bool CancelHardTime								<ExportExcel="Both"|DisplayName="CancelHardTime">;
	var(SpellCommonAttribute)    Spell_LimitBitsType HitTag							<ExportExcel="Both"|Merger="0"|DisplayName="HitTag">;
	var(SpellCommonAttribute)    int Speed											<ExportExcel="Both"|DisplayName="Speed">;

	var(SpellCommonAttribute)    bool IsMixEffect 									<ExportExcel="Both"|DisplayName="IsMixEffect">;
	var(SpellCommonAttribute)    int MixEffectPhyRate	 							<ExportExcel="Both"|DisplayName="MixEffectPhyRate">;
	var(SpellCommonAttribute)    int MixEffectMagRate 								<ExportExcel="Both"|DisplayName="MixEffectMagRate">;
	
	var(SpellCommonAttribute)    int EffectPct										<ExportExcel="Both"|DisplayName="EffectPct">;
	var(SpellCommonAttribute)    int EffectValue									<ExportExcel="Both"|DisplayName="EffectValue">;
	var(SpellCommonAttribute)    int EffectValueMax									<ExportExcel="Both"|DisplayName="EffectValueMax">;
	var(SpellCommonAttribute)    int DefaultValue									<ExportExcel="Both"|DisplayName="DefaultValue">;
	var(SpellCommonAttribute)    int DefaultValueMax								<ExportExcel="Both"|DisplayName="DefaultValueMax">;
	var(SpellCommonAttribute)    bool AffectedByNpcParam 							<ExportExcel="Both"|DisplayName="AffectedByNpcParam">;

	var(SpellCommonAttribute)    int ThreatPct										<ExportExcel="Both"|DisplayName="ThreatPct">;
	var(SpellCommonAttribute)    int ThreatValue									<ExportExcel="Both"|DisplayName="ThreatValue">;
	var(SpellCommonAttribute)    int ThreatMax										<ExportExcel="Both"|DisplayName="ThreatMax">;
	var(SpellCommonAttribute)    bool CasterDeadLimit								<ExportExcel="Both"|DisplayName="CasterDeadLimit">;
	var(SpellCommonAttribute)    bool TargetDeadLimit								<ExportExcel="Both"|DisplayName="TargetDeadLimit">;
	var(SpellCommonAttribute)    bool CanCastOnMount								<ExportExcel="Both"|DisplayName="CanCastOnMount">;
	var(SpellCommonAttribute)	 Spell_LimitBitsType CasterLimitBits				<ExportExcel="Both"|Merger="0"|DisplayName="CasterLimitBits">;
	var(SpellCommonAttribute)	 Spell_CCLimitBitsType CasterCCLimitBits			<ExportExcel="Both"|Merger="0"|DisplayName="CasterCCLimitBits">;
	var(SpellCommonAttribute)	 Spell_DaytimeLimit DaytimeLimit		        	<ExportExcel="Both"|Merger="0"|DisplayName="DaytimeLimit">;
	var(SpellCommonAttribute)	 Spell_WeatherLimit WeatherLimit		        	<ExportExcel="Both"|Merger="0"|DisplayName="WeatherLimit">;
	var(SpellCommonAttribute)    float ExCastTime1									<ExportExcel="Both"|Convert="1000"|DisplayName="ExCastTime1">;
	var(SpellCommonAttribute)    float ExCastTime2									<ExportExcel="Both"|Convert="1000"|DisplayName="ExCastTime2">;
	var(SpellCommonAttribute)    float ExCastTime3									<ExportExcel="Both"|Convert="1000"|DisplayName="ExCastTime3">;
	var(SpellCommonAttribute)    float ExCastTime4									<ExportExcel="Both"|Convert="1000"|DisplayName="ExCastTime4">;
	var(SpellCommonAttribute)    float ExCastTime5									<ExportExcel="Both"|Convert="1000"|DisplayName="ExCastTime5">;
	var(SpellCommonAttribute)    bool CancelStiff									<ExportExcel="Both"|DisplayName="CancelStiff">;
	var(SpellCommonAttribute)    int KnockBackRate									<ExportExcel="Both"|DisplayName="KnockBackRate">;
	var(SpellCommonAttribute)    float BaseStiffTime								<ExportExcel="Both"|Convert="1000"|DisplayName="BaseStiffTime">;
	var(SpellCommonAttribute)    float MonsterChaseDelayTime						<ExportExcel="Both"|Convert="1000"|DisplayName="MonsterChaseDelayTime">;
	var(SpellCommonAttribute)    bool ChainEffect									<ExportExcel="Both"|DisplayName="ChainEffect">;
	var(SpellCommonAttribute)    string Description									<ExportExcel="Both"|DisplayName="Description">;
	var(SpellCommonAttribute)    string LearnDesc									<ExportExcel="Both"|DisplayName="LearnDesc">;
	var(SpellCommonAttribute)    string EffectDesc									<ExportExcel="Both"|DisplayName="EffectDesc">;
	var(SpellCommonAttribute)    string LevelUpDesc									<ExportExcel="Both"|DisplayName="LevelUpDesc">;
	var(SpellCommonAttribute)    string OpenDesc									<ExportExcel="Both"|DisplayName="OpenDesc">;
	var(SpellCommonAttribute)    string CloseDesc									<ExportExcel="Both"|DisplayName="CloseDesc">;

    var(SpellCommonAttribute)    SeqAct_Modifier OnExCast1							<ExportExcel="Server"|DisplayName="OnExCast1">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnExCast2							<ExportExcel="Server"|DisplayName="OnExCast2">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnExCast3							<ExportExcel="Server"|DisplayName="OnExCast3">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnExCast4							<ExportExcel="Server"|DisplayName="OnExCast4">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnExCast5							<ExportExcel="Server"|DisplayName="OnExCast5">;

	var(SpellCommonAttribute)    SeqAct_Modifier CasterCheckNode					<ExportExcel="Server"|DisplayName="CasterCheckNode">;
	var(SpellCommonAttribute)    SeqAct_Modifier TargetCheckNode					<ExportExcel="Server"|DisplayName="TargetCheckNode">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnLearn							<ExportExcel="Server"|DisplayName="OnLearn">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnForget							<ExportExcel="Server"|DisplayName="OnForget">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnEquip							<ExportExcel="Server"|DisplayName="OnEquip">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnUnEquip							<ExportExcel="Server"|DisplayName="OnUnEquip">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnPrepare							<ExportExcel="Server"|DisplayName="OnPrepare">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnCast								<ExportExcel="Server"|DisplayName="OnCast">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnBreak							<ExportExcel="Server"|DisplayName="OnBreak">;
	var(SpellCommonAttribute)    SeqAct_Modifier OnBulletCreate						<ExportExcel="Server"|DisplayName="OnBulletCreate">;
	var(SpellCommonAttribute)    SeqAct_Modifier BeforeRoll							<ExportExcel="Server"|DisplayName="BeforeRoll">;
	var(SpellCommonAttribute)    SeqAct_Modifier AfterRoll							<ExportExcel="Server"|DisplayName="AfterRoll">;
	var(SpellCommonAttribute)    SeqAct_Modifier BeforeEffect						<ExportExcel="Server"|DisplayName="BeforeEffect">;
	var(SpellCommonAttribute)    SeqAct_Modifier AfterEffect						<ExportExcel="Server"|DisplayName="AfterEffect">;

	var                          int X1;
	var                          int X2;
	var                          int X3;
	var                          int X4;

	structdefaultproperties
	{
		NeedCache=true;
		BreakMove=true;
		SearchDistance = 640;
		QTEType=E_spqte_None;
		ThreatValue=1;
		ThreatMax=1;
		ChannelCount=0;
		TrigEvent=true;
		AffectedByNpcParam=false;
	}
};

struct native RuneDebris
{
	var int ItemId;
	var int ItemCount;
};

// cd info
struct native CoolDown
{
	var float GroupTimeMax;
	var float GroupTimeLeft;

	var float SpellTimeMax;
	var float SpellTimeLeft;
};

var(ServerAttr) AbilitySpellCommonAttribute SpellCommonAttribute	<ExportExcel="0"|DisplayName="SpellCommonAttribute">;
//song@snda.com 测试 服务器专属属性
var(ServerAttr) name Modifier										<DynamicList="AbilityModifier"|DisplayName="Modifier">;
var(ServerAttr) float ModifierPara1									<TooltipCondition="Modifier"|DisplayName="ModifierPara1">;
var(ServerAttr) float ModifierPara2									<TooltipCondition="Modifier"|DisplayName="ModifierPara2">;
var(ServerAttr) float ModifierPara3									<TooltipCondition="Modifier"|DisplayName="ModifierPara3">; 
//song@snda.com

var(ClientAttr) ClientAttribute             ClientCommonAttribute	<ExportExcel="1"|DisplayName="ClientCommonAttribute">;
var(ClientAttr) array<ComponentNode>        SpellComponentList		<DisplayName="SpellComponentList">;
var(ClientAttr) array<ComponentNode>        CastComponentList		<DisplayName="CastComponentList">;
var(ClientAttr) EWeaponType                 DefaultWeaponType       <DisplayName="DefaultWeaponType">;

var(ClientAttr) editinline array<AbilityComponent>        TipsComponentList;

var(ClientAttr) array<ComponentNodePair>    OverrideComponentList;

var(ClientAttr) bool bHasSpellProgressData;
var(ClientAttr) bool bCanExpandAbilityByLevel;   
var(ClientAttr) bool bClientPlayFirst;
var(ClientAttr) bool bClientRotateFirst;
var(ClientAttr) bool bCanUITarget;

const           MaxSubRuneNum = 9;         
var             array<MMOAbility>           SubRuneList;
var             string                      RuneIconName;
var             MMOAbility                  MainSkill;
//存在对应的符文技能模板表内存中
//数据是通用的 放这ms不太合理 但省（技能表3维展开很大，基本都有3~5个符文） 功能上这个也只属于玩家UI可见 暂时未见有问题
//再如需要 可以将表挂至Actor上
//激活符文所需要的碎片数量
var             array<RuneDebris>           vecRuneDebris;
//符文消耗方式
var             int                         RuneDurabilityType;
//符文最大耐久
var             int                         RuneDurMax;
//符文当前耐久
var             int                         RuneDurability;

//技能熟练度
var int SkillPracticeProgress;
//技能当前等级最大熟练度
var int LvUpRequirePoints;
//升级到下一级需要的等级
var int LvUpRequireRoleLevel;
////铭文技能升级物品
//var int LevelUpItemID;
////铭文技能升级物品数量
//var int LevelUpItemCnt;
//技能升级类型
var int SkillPracticeType;
//技能升级消耗类型 0为不在升级
var int SkillPracticeCostType;
//升级剩余时间
var int SkillPracticeEndTime;
//所有玩家可见主动技能 唯一标识索引（不包括符文）
var int SkillGuiID;
//所有玩家可见符文 唯一标识索引
var int SkillGuiRuneID;

// for preview only
var(Preview) editoronly  editinline AbilityPreviewResources PreviewResource;

var transient array<int> GroupIds;

var transient bool bCoolDown;
var transient float CoolDownPercent;
var transient CoolDown CoolDownInfo;
var transient int SlotHotKeyID;

var string	SkillDetailDescribe;

var array<string>  SkillDetailDescribeArray;

var string	SkillMainSpeciality;
var string	SkillSecondSpeciality;
var string	SkillThirdSpeciality;

var string	DescriptiveWords;
var string	UseMethod;
var string	AttackRange;
var string	EffectRange;
var int	EffectRangeEnum;

var int Pow;

native function int GetWeaponHitType(EWeaponType InWeaponType);

native function AbilityHitPointComponent GetAbilityHitPointDataByWeaponType(EWeaponType InWeaponType, int InIndex);


native function AddComponent(class<AbilityComponent> ClassType, bool bSpellComponent, EWeaponType InWeaponType, optional AbilityComponent InTemplate);

event int GetAbilityId()
{
	return SpellCommonAttribute.ID;
}


function string GetAbilitySubSchool()
{
	return "";
}

native function string GetAbilityIconName();

//获得指定Slot位置的符文Icon
native function string GetSubRuneIconName(int RuneSlot );
//获得符文Icon
native function string GetRuneIconName( );
//获得SkillGuiRuneID
native function int    GetSkillGuiRuneID( );
//添加关联子符文
native function        AddSubRune(int RuneSlot,MMOAbility SubRune );
//得到当前的装配的RuneSlot
native function int    GetRuneSlot();
//设置装配RuneSlot
native function        SetRuneSlot( int inRuneSlot );

event string GetAbilityName()
{
	return ClientCommonAttribute.AbilityName;
}
event string GetAbilityDescribe()
{
	return ClientCommonAttribute.AbilityDescribe;
}

native function string GetAbilityDetailDescribe();
//{
//	return SkillDetailDescribe;
//}

native function ParseAbilityDetailDescribe();

native function string GetMainSpeciality();
native function string GetSecondSpeciality();
native function string GetThirdSpeciality();

event float GetMaxAttackDistance()
{
	if(SpellCommonAttribute.LockType == E_splt_Self)
	{
		return 9999999;
	}
	else if(SpellCommonAttribute.LockType == E_splt_Object)
	{
		return SpellCommonAttribute.MaxDistance;
	}
	else if(SpellCommonAttribute.LockType == E_splt_Direction)
	{
		return 9999999;
	}
	else if(SpellCommonAttribute.LockType == E_splt_Pos)
	{
	   return SpellCommonAttribute.MaxDistance;
	}
	else //if(SpellCommonAttribute.LockType == ESL_Type5)
	{
		return 9999999;
	}
}

event float GetMinAttackDistance()
{
	if(SpellCommonAttribute.LockType == E_splt_Self)
	{
		return 0.0;
	}
	else if(SpellCommonAttribute.LockType == E_splt_Object)
	{
		return SpellCommonAttribute.MinDistance;
	}
	else if(SpellCommonAttribute.LockType == E_splt_Direction)
	{
		return 0.0;
	}
	else if(SpellCommonAttribute.LockType == E_splt_Pos)
	{
	   return SpellCommonAttribute.MinDistance;
	}
	else //if(SpellCommonAttribute.LockType == ESL_Type5)
	{
		return 0.0;
	}

}

function bool IsPassiveAbility()
{
	return false;
}




native function UpdateCd(float DeltaTime);



native function Tick(float DeltaTime);

native function float GetAbilitySpellTime();
native function float GetAbilityCastTime();

native function GetSpellTime(out float Time);
native function SetSpellTime(float InNewTime);
native function GetCastTime(out float Time, out int CycleNumber, out float FixTime);
native function SetCastTime(float InCastTime);

native function float GetSpellBufTime(MMOActor player);
native function float GetCoolDownBufTime(MMOActor player);


native function bool IsNeedArchetype();

event string GetDetailedDescription( int InIndex )
{
	local string Description;
	switch( InIndex )
	{
	case 0:
		Description = string(SpellCommonAttribute.ID);
		break;
	case 1:
		Description = ClientCommonAttribute.AbilityName;
		break;
	case 2:
		Description = ClientCommonAttribute.AbilityDescribe;
		break;
	case 3:
		break;
	default:
		break;
	}
	return( Description );

}

//-------------------------------------------------------
// 导出到Excel
//-------------------------------------------------------
cpptext
{
	virtual FString GetDetailedDescription( INT InIndex )
	{
		return eventGetDetailedDescription(InIndex);
	}
	//bool DataToExcel(TArray<FString>& Row);

	UBOOL GetEffectComponentList(BYTE InWeaponType, TArray<FComponentNode>** OutSpellComponentList, TArray<FComponentNode>** OutCastComponentList);
	virtual void GetDynamicListValues(const DynamicListInfo& ListInfo, TArray<FString>& Values); // oscar@snda
}

// 导出表头,先调用脚本函数，再调用native函数
event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues)
{

}

// 导出每行数据,先调用脚本函数，再调用native函数
event ScriptDataToExcel(out array<string> Row)
{
}


native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function DataToExcel2(out array<string> Row);
native function ExportExcelHeaders2(out array<string> Header, out array<string> DefaultValues);
native function ImportFromExcel(array<string> values);
event ScriptImportFromExcel(array<string> Values){}
native function InitDataToExcel();

native function ParseGroupIds();
native function int GetTemplateID() const;

native function bool CanUseSelfCheck(MMOActor InActor, optional bool bCheckCD);

native function bool CanUseManaCheck(MMOActor InActor);

DefaultProperties
{
	SlotHotKeyID=-1
	bHasSpellProgressData=true
	bCanExpandAbilityByLevel=true
	bClientPlayFirst=false
	bClientRotateFirst=true
	bCanUITarget=false
}
