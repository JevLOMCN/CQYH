//
// common type definitions, to avoid script circular dependencies
//
class TypeDefine extends Object
	dependson(ServerEnum)
	native;

/*
 * body type, used only for sound effects
 */
 //`define ServeNum 13
enum EMenuType
{	
	EMenuType_Charactor,
	EMenuType_Bag,
	EMenuType_Skill,
	EMenuType_Guild,
	EMenuType_Quest,
	EMenuType_Riding,
	EMenuType_Option,
	EMenuType_Achievement,
	EMenuType_Repute,
	EMenuType_Ranking,
	EMenuType_Schedule,
	EMenuType_Mail,
	EMenuType_Mall,
	EMenuType_GoldStore,
	EMenuType_GMWebPage,
	EMenuType_Handbook,
	EMenuType_Recharge,
	EMenuType_DailySignIn,
	EMenuType_GMService,
	EMenuType_Auction,

	EMenuType_Child_Charactor,
	EMenuType_Child_Bag,
	EMenuType_Child_Heaven,
	EMenuType_Child_Skill,
	EMenuType_Child_NormalQuest,
	EMenuType_Child_StoryQuest,
	EMenuType_Child_Team,
	EMenuType_Child_Guild,
	EMenuType_Child_Friend,
	EMenuType_Child_MasterStudent,
	EMenuType_Child_Riding,
	EMenuType_Child_Option,
	EMenuType_Child_Achievement,
	EMenuType_Child_Repute,
	EMenuType_Child_Ranking,
	EMenuType_Child_Schedule,
	EMenuType_Child_Mail,
	EMenuType_Child_Mall,
	EMenuType_Child_GoldStore,
	EMenuType_Child_GMWebBug,
	EMenuType_Child_SearchScreenShot,
	EMenuType_Child_SearchLog,
	EMenuType_Child_GMWebSurvey,
	EMenuType_Child_Handbook,
	EMenuType_Child_RechargeGamePoints,
	EMenuType_Child_RechargeYuanBao,
	EMenuType_Child_DailySignInGetReward,
	EMenuType_Child_GMService,
	EMenuType_Child_Auction,

};

enum EBodyType
{
	EBDT_Default        <DisplayName="Default">,
	EBDT_Light          <DisplayName="Light">,
	EBDT_Middle         <DisplayName="Middle">,
	EBDT_Heavy          <DisplayName="Heavy">,
	EBDT_SuperHeavy     <DisplayName="SuperHeavy">,
};

// all equipment
enum EEquipType
{
	EET_Normal_MainHand         <DisplayName="MainHand">,
	EET_Normal_OffHand			<DisplayName="OffHand">,
	EET_Normal_Head				<DisplayName="Head">,
	EET_Normal_Shoulder			<DisplayName="Shoulder">,
	EET_Normal_Body				<DisplayName="Body">,
	EET_Normal_Leg				<DisplayName="Leg">,
	EET_Normal_Arm				<DisplayName="Arm">,
	EET_Normal_Waist			<DisplayName="Waist">,
	EET_Normal_Foot				<DisplayName="Foot">,
	EET_Normal_Back				<DisplayName="Back">,
	EET_Normal_Avatar			<DisplayName="Avatar">,
	EET_Normal_Neck				<DisplayName="Neck">,
	EET_Normal_Finger			<DisplayName="Finger">,
	EET_Normal_Wrist			<DisplayName="Wrist">,
////};
//	EET_Normal_Count,
//
////// fashion equipment
////enum EAvatarShowType
////{
//	EET_Avatar_Head			<DisplayName="AvatarHead">,
//	EET_Avatar_Shoulder			<DisplayName="AvatarShoulder">,
//	EET_Avatar_Body			<DisplayName="AvatarBody">,
//	EET_Avatar_Leg			<DisplayName="AvatarLeg">,
//	EET_Avatar_Arm			<DisplayName="AvatarArm">,
//	EET_Avatar_Foot			<DisplayName="AvatarFoot">,
//	EET_Avatar_Hair				<DisplayName="AvatarHair">,
//	EET_Avatar_Back				<DisplayName="AvatarBack">,

	EET_Total_Count
};

// enum ENormalEquipSlot
// {
// 	E_NES_MAIN_HAND,
// 	ENES_OffHand,
// 	ENES_Head,
// 	ENES_Shoulder,
// 	ENES_Body,
// 	ENES_Leg,
// 	ENES_Arm,
// 	ENES_Waist,
// 	ENES_Foot,
// 	ENES_Back,
// 	ENES_Avatar,
// 	ENES_Neck,
// 	ENES_FingerL,
// 	ENES_FingerR,
// 	ENES_WristL,
// 	ENES_WristR,
// 	ENES_Count
// };

//enum EAvatarEquipSlot
//{
//	EAES_Head,
//	EAES_Shoulder,
//	EAES_Body,
//	EAES_Leg,
//	EAES_Arm,
//	EAES_Foot,
//	EAES_Hair,
//	EAES_Back,
//	EAES_Count
//};

/*
EWT_Weapon_0=bare hands
EWT_Weapon_1=single-wield slashing (one-handed sword)
EWT_Weapon_2=single-wield blunt (one-handed hammer)
EWT_Weapon_3=two-handed short-hilt heavy (two-handed sword)
EWT_Weapon_4=two-handed long-hilt heavy (long-hilt axe)
EWT_Weapon_5=dual-wield heavy (dual axes)
EWT_Weapon_6=two-handed Eastern long-hilt (spear)
EWT_Weapon_7=dual-wield gloves (fist spikes)
EWT_Weapon_8=dual-wield short blades (dual daggers)
EWT_Weapon_9=dual-wield chain blades (chain blades)
EWT_Weapon_10=dual-wield hand crossbows (dual crossbows)
EWT_Weapon_11=crossbow
EWT_Weapon_12=bow
EWT_Weapon_13=single-wield short-hilt (saber)
EWT_Weapon_14=staff
EWT_Weapon_14=staff
EWT_Weapon_15=large shield
EWT_Weapon_16=small shield
EWT_Weapon_17=magic implement
EWT_Weapon_18=sword
EWT_Weapon_19=fan
EWT_Weapon_20=staff (club)
*/


enum EWeaponType
{
	EWT_Weapon_0           <DisplayName="EWT_Weapon_0">,
	EWT_Weapon_1          <DisplayName="EWT_Weapon_1">,
	EWT_Weapon_2          <DisplayName="EWT_Weapon_2">,
	EWT_Weapon_3          <DisplayName="EWT_Weapon_3">,
	EWT_Weapon_4          <DisplayName="EWT_Weapon_4">,
	EWT_Weapon_5         <DisplayName="EWT_Weapon_5">,
	EWT_Weapon_6          <DisplayName="EWT_Weapon_6">,
	EWT_Weapon_7          <DisplayName="EWT_Weapon_7">,
	EWT_Weapon_8          <DisplayName="EWT_Weapon_8">,
	EWT_Weapon_9         <DisplayName="EWT_Weapon_9">,
	EWT_Weapon_10         <DisplayName="EWT_Weapon_10">,
	EWT_Weapon_11         <DisplayName="EWT_Weapon_11">,
	EWT_Weapon_12         <DisplayName="EWT_Weapon_12">,
	EWT_Weapon_13         <DisplayName="EWT_Weapon_13">,
	EWT_Weapon_14         <DisplayName="EWT_Weapon_14">,
	EWT_Weapon_15         <DisplayName="EWT_Weapon_15">,
	EWT_Weapon_16         <DisplayName="EWT_Weapon_16">,
	EWT_Weapon_17         <DisplayName="EWT_Weapon_17">,
	EWT_Weapon_18         <DisplayName="EWT_Weapon_18">,
	EWT_Weapon_19         <DisplayName="EWT_Weapon_19">,
	EWT_Weapon_20         <DisplayName="EWT_Weapon_20">,
	EWT_Riding_1          <DisplayName="EWT_Riding_1">,
	EWT_Riding_2          <DisplayName="EWT_Riding_2">,
	EWT_Riding_3          <DisplayName="EWT_Riding_3">,
	EWT_Riding_4          <DisplayName="EWT_Riding_4">,
};

enum EWeaponAppearenceType
{
	EWAT_Type0 <DisplayName="EWAT_Type0">,
	EWAT_Type1 <DisplayName="EWAT_Type1">,
	EWAT_Type2 <DisplayName="EWAT_Type2">,
	EWAT_Type3 <DisplayName="EWAT_Type3">,
	EWAT_Type4 <DisplayName="EWAT_Type4">,
	EWAT_Type5 <DisplayName="EWAT_Type5">,
	EWAT_Type6 <DisplayName="EWAT_Type6">,
	EWAT_Type7 <DisplayName="EWAT_Type7">,
	EWAT_Type8 <DisplayName="EWAT_Type8">,
	EWAT_Type9 <DisplayName="EWAT_Type9">,
	EWAT_Type10 <DisplayName="EWAT_Type10">,
	EWAT_Type11 <DisplayName="EWAT_Type11">,
	EWAT_Type12 <DisplayName="EWAT_Type12">,
	EWAT_Type13 <DisplayName="EWAT_Type13">,
};

/**
* type of body part model
* used to distinguish different model parts when merging
*/
enum EBodyMeshType
{
	// parts need to merge
	EBMT_Face   <DisplayName="EBMT_Face">,	// 0
	EBMT_Chest  <DisplayName="EBMT_Chest">,	// 1
	EBMT_Hands  <DisplayName="EBMT_Hands">,	// 2
	EBMT_Thigh  <DisplayName="EBMT_Thigh">,	// 3
	EBMT_Feet  <DisplayName="EBMT_Feet">,	// 4
	EBMT_Waist  <DisplayName="EBMT_Waist">,	// 5

	EBMT_NoMerge  <DisplayName="EBMT_NoMerge">,

	// parts don't need merge
	EBMT_Hair  <DisplayName="EBMT_Hair">,
	EBMT_Helmet  <DisplayName="EBMT_Helmet">,
	EBMT_LShoulder  <DisplayName="EBMT_LShoulder">,
	EBMT_RShoulder  <DisplayName="EBMT_RShoulder">,
	EBMT_Weapon  <DisplayName="EBMT_Weapon">,
	EBMT_Back	<DisplayName="EBMT_Back">,

	EBMT_Weapon2	<DisplayName="EBMT_Weapon2>,
	EBMT_BackOrnamental	<DisplayName="EBMT_BackOrnamental">,

	EBMT_None  <DisplayName="EBMT_None">
};

/*
 * item material type, used only for sound effects, defines all item material types, including weapons and clothing
 * 
 */
enum EItemMaterialType
{
	EIMAT_Default <DisplayName="EIMAT_Default">,
	EIMAT_Mat1    <DisplayName="EIMAT_Mat1">,
	EIMAT_Mat2    <DisplayName="EIMAT_Mat2">,
	EIMAT_Mat3    <DisplayName="EIMAT_Mat3">,
	EIMAT_Mat4    <DisplayName="EIMAT_Mat4">,
	EIMAT_Mat5    <DisplayName="EIMAT_Mat5">,
	EIMAT_Mat6    <DisplayName="EIMAT_Mat6">,
	EIMAT_Mat7    <DisplayName="EIMAT_Mat7">,
	EIMAT_Mat8    <DisplayName="EIMAT_Mat8">,
	EIMAT_Mat9    <DisplayName="EIMAT_Mat9">,
};

// UI mouse state
enum Mouse_State
{
	EMS_Normal,
	EMS_Attack,
	EMS_Click,
	EMS_Talk,
	EMS_Pick,
	EMS_Buy,
	EMS_Sell
};

enum EHint_Type
{
	EHT_QteSpace,
	EHT_FrequenceSpace,
	EHT_HoldStone,
	EHT_OpenTrap,
	EHT_TurnTrap,
	EHT_FinAttack,
	EHT_Slip,
	EHT_ClimbWall,
	EHT_WarningTrap,
	EHT_Texture,
};

// player attack animation hemu@snda used for AnimMixMatrix
//add ability animations that need a recovery animation here in the future; DisplayName is the Anim name
enum EAnimMixSource
{
	EAMS_Combo11    <DisplayName="attack31">,
	EAMS_Combo12    <DisplayName="attack32">,
	EAMS_Combo13    <DisplayName="attack33">,
	EAMS_Combo14    <DisplayName="attack34">,
	EAMS_Combo21    <DisplayName="attack41">,
	EAMS_Combo22    <DisplayName="attack42">,
	EAMS_Combo23    <DisplayName="attack43">,
	EAMS_Combo24    <DisplayName="attack44">,
};
// player state hemu@snda used for AnimMixMatrix
//add player states here; DisplayName is the Anim name
enum EAnimMixTarget
{
	EAMT_Idle	    <DisplayName="attack_idle">,
	EAMT_Run	    <DisplayName="Run">,
};

//song@snda.com test server exclusive property
//enum EModifier
//{
//	EMF_FireDamage,
//	EMF_FireCritical,
//};
//song@snda.com


enum EAbilityState
{
	EAS_Requested,
	EAS_Spell,
	EAS_Cast,
	EAS_End,
};


// movement
enum EActorMoveType
{
//	EMT_Idle,
	EMT_Walk,
	EMT_Run,
	EMT_Walk_Oblique,
	EMT_Run_Oblique,
	EMT_Fly,
	EMT_Fly_Oblique,
 	EMT_PrepareStall, // TODO: later refactor into a separate StallState, and split out the state machines of these states
 	EMT_InStall, 
//	EMT_Ride,
};



enum EItemSubType
{
    EIST_INVALID <DisplayName="Invalid">,
	EIST_TYPE1 <DisplayName="EIST_TYPE1">,
	EIST_TYPE2 <DisplayName="EIST_TYPE2">,
	EIST_TYPE3 <DisplayName="EIST_TYPE3">,
	EIST_TYPE4 <DisplayName="EIST_TYPE4">,
	EIST_TYPE5 <DisplayName="EIST_TYPE5">,
	EIST_TYPE6 <DisplayName="EIST_TYPE6">,
	EIST_TYPE7 <DisplayName="EIST_TYPE7">,
	EIST_TYPE8 <DisplayName="EIST_TYPE8">,
	EIST_TYPE9 <DisplayName="EIST_TYPE9">,
	EIST_TYPE10 <DisplayName="EIST_TYPE10">,
	EIST_TYPE11 <DisplayName="EIST_TYPE11">,
	EIST_TYPE12 <DisplayName="EIST_TYPE12">,
	EIST_TYPE13 <DisplayName="EIST_TYPE13">,
	EIST_TYPE14 <DisplayName="EIST_TYPE14">,
	EIST_TYPE15 <DisplayName="EIST_TYPE15">,
	EIST_TYPE16 <DisplayName="EIST_TYPE16">,
    //EIST_WEAPON_MAINHAND <DisplayName="MainHandWeapon">,
    //EIST_WEAPON_OFFHAND <DisplayName="OffHandWeapon">,
    //EIST_WEAPON_TWOHAND <DisplayName="TwoHandWeapon">,
    //
    //EIST_ARMOR_WRIST <DisplayName="Wrist">,
    //EIST_ARMOR_WAIST <DisplayName="Waist">,
    //EIST_ARMOR_LEG <DisplayName="Leg">,
    //EIST_ARMOR_INNERCHEST <DisplayName="Innerchest">,
    //EIST_ARMOR_HEAD <DisplayName="Head">
};
enum EItemSubSubType
{
    EISST_INVALID <DisplayName="Invalid">,
	EISST_TYPE1 <DisplayName="EISST_TYPE1">,
	EISST_TYPE2 <DisplayName="EISST_TYPE2">,
	EISST_TYPE3 <DisplayName="EISST_TYPE3">,
	EISST_TYPE4 <DisplayName="EISST_TYPE4">,
	EISST_TYPE5 <DisplayName="EISST_TYPE5">,
	EISST_TYPE6 <DisplayName="EISST_TYPE6">,
	EISST_TYPE7 <DisplayName="EISST_TYPE7">,
	EISST_TYPE8 <DisplayName="EISST_TYPE8">,
	EISST_TYPE9 <DisplayName="EISST_TYPE9">,
	EISST_TYPE10 <DisplayName="EISST_TYPE10">,
	EISST_TYPE11 <DisplayName="EISST_TYPE11">,
	EISST_TYPE12 <DisplayName="EISST_TYPE12">,
	EISST_TYPE13 <DisplayName="EISST_TYPE13">,
	EISST_TYPE14 <DisplayName="EISST_TYPE14">,
	EISST_TYPE15 <DisplayName="EISST_TYPE15">,
	EISST_TYPE16 <DisplayName="EISST_TYPE16">,
};

enum EItemColor
{
    EIC_Color1 <DisplayName="EIC_Color1">,
	EIC_Color2 <DisplayName="EIC_Color2">,
	EIC_Color3 <DisplayName="EIC_Color3">,
	EIC_Color4 <DisplayName="EIC_Color4">,
	EIC_Color5 <DisplayName="EIC_Color5">,
	EIC_Color6 <DisplayName="EIC_Color6">,
	EIC_Color7 <DisplayName="EIC_Color7">,
	EIC_Color8 <DisplayName="EIC_Color8">,
	EIC_Color9 <DisplayName="EIC_Color9">,
	EIC_Color10 <DisplayName="EIC_Color10">,
	EIC_Color11 <DisplayName="EIC_Color11">,
	EIC_Color12 <DisplayName="EIC_Color12">,
};

// enum EItemBindType
// {
//     EIBT_NONE <DisplayName="None">,      //no binding
//     EIBT_BOU <DisplayName="BindOnUse">,       //bind on use, bind on use
//     EIBT_BOE <DisplayName="BindOnEquip">,       //bind on equip, bind on equip
//     EIBT_BOP <DisplayName="BindOnPickup">,       //bind on pickup, bind on pick up
//     EIBT_BOT <DisplayName="BindOnTrade">,       //bind on trade, bind on trade
//     EIBT_BINDED <DisplayName="AlreadyBound">     //already bound
// };

enum EGenderType
{
	EGT_None	<DisplayName="None">,
	EGT_Male	<DisplayName="Male">,
	EGT_Female	<DisplayName="Female">,
	EGT_Asexua	<DisplayName="Asexua">
};

enum EEmitterType
{
	// regular particle effect (disappears automatically after playing once)
	EMIT_Normal,
	// trail
	EMIT_Trail,
		// appears as flying to a target in a straight line or curve
	EMIT_FlyTo,
		// chain lightning or life-drain effect
	EMIT_ConnectTo,
		// rotate around self
	EMIT_Rotate,
		// orbit the MESH
	EMIT_Surround,
};

enum ETrackLineType
{
	ETLT_Line       <DisplayName="EETLT_Line">,
	ETLT_Parabola   <DisplayName="EETLT_Parabola">,
	ETLT_Helix      <DisplayName="EETLT_Helix">,
	ETLT_Bezier     <DisplayName="ETLT_Bezier">,
};

enum ETrackTimeType
{
	ETTT_ConstSpeed <DisplayName="ETTT_ConstSpeed">,
	ETTT_ConstTime  <DisplayName="ETTT_ConstTime">,
};

enum EAbilityComponentStopType
{
	// use Duration or its own Duration (PariticleSystem)
	ACBT_StopWhenConstDurationPass,
	ACBT_StopWhenAbilityStateEnd,
	ACBT_StopWhenAbilityEnd,
};

// material effect type
enum EMaterialEffectType
{
	// no effect
	EMET_None,
	// modify material parameter
	EMET_ChangeMaterialProperty,
	// transparency
	EMET_LoadOpacityMaterial,
	// replace material
	EMET_ChangeMaterial,
};

struct native MaterialEffectData
{
	var int EffectId;
	var MaterialInstanceTimeVaryingEffectNode EffectData;
};

enum EMaterailCreateType
{
	EMCT_Constant   <DisplayName="EMCT_Constant">,
	EMCT_TimeVaring   <DisplayName="EMCT_TimeVaring">,
};


// to cope with repeated changes to the sect concept, enum names use A,B,C.. etc., and displayname uses the current version
enum EPlayerClass
{
	EPCT_PlayerClassA <DisplayName="E_PlayerClassA">,
	EPCT_PlayerClassB <DisplayName="E_PlayerClassB">,
	EPCT_PlayerClassC <DisplayName="E_PlayerClassC">,
	EPCT_PlayerClassD <DisplayName="E_PlayerClassD">,
	EPCT_PlayerClassE <DisplayName="E_PlayerClassE">, // Taoist
	// 1.05 backport: values match the server (clsSpear=5, clsSorcerer=6, clsNpc=7, none/any=8); names match the 1.05 client
	EPCT_PlayerClassG <DisplayName="E_PlayerClassG">, // Lancer (Spear)
	EPCT_PlayerClassH <DisplayName="E_PlayerClassH">, // Sorcerer
	EPCT_PlayerClassF <DisplayName="E_PlayerClassF">, // npc
	EPCT_None <DisplayName="E_None">,
};

//Ability categorization in the UI presentation
enum ESPELL_UI_TYPE
{
	ESUIType_Active                    <DisplayName="ESUIType_None">,
	ESUIType_Passive                   <DisplayName="ESUIType_Passive">,
	ESUIType_Counter                   <DisplayName="ESUIType_Counter">,
	ESUIType_Chase                     <DisplayName="ESUIType_Chase">,
	ESUIType_Terminate                 <DisplayName="ESUIType_Terminate">,
	ESUIType_Total                     <DisplayName="ESUIType_Total">,
};

// in-game currency type
enum ECurrencyType
{
	ECUT_GameGold <DisplayName="GameGold">,
	ECUT_RMB <DisplayName="RMB">
};

// damage result type
/*
Result0=invalid
Result1=miss
Result2=immune
Result3=block
Result4=parry
Result5=dodge
Result6=normal attack
Result7=critical hit
Result8=empty effect
*/

enum EDamageResultType
{
	DRT_Result0  <DisplayName="Result0">,
	DRT_Result1  <DisplayName="Result1">,
	DRT_Result2  <DisplayName="Result2">,
	DRT_Result3  <DisplayName="Result3">,
	DRT_Result4  <DisplayName="Result4">,
	DRT_Result5  <DisplayName="Result5">,
	DRT_Result6  <DisplayName="Result6">,
	DRT_Result7  <DisplayName="Result7">,
	DRT_Result8  <DisplayName="Result8">,
};

enum EDamagePropertyType
{
	EDPT_None,
	EDPT_Hp,
	EDPT_Mp,
};


/*
HitType0=no type
HitType1=slash
HitType2=stab
HitType3=shoot
HitType4=bash
HitType5=brawl
HitType6=element
*/
enum EWeaponHitType
{
	EWHT_HitType0   <DisplayName="HitType0">,
	EWHT_HitType1   <DisplayName="HitType1">,
	EWHT_HitType2   <DisplayName="HitType2">,
	EWHT_HitType3   <DisplayName="HitType3">,
	EWHT_HitType4   <DisplayName="HitType4">,
	EWHT_HitType5   <DisplayName="HitType5">,
	EWHT_HitType6   <DisplayName="HitType6">,
	EWHT_HitType7   <DisplayName="HitType7">,
	EWHT_HitType8   <DisplayName="HitType8">,
};

enum EHitDirectionType
{
	EHDT_Type0  <DisplayName="Type0">,
	EHDT_Type1  <DisplayName="Type1">,
	EHDT_Type2  <DisplayName="Type2">,
	EHDT_Type3  <DisplayName="Type3">,
	EHDT_Type4  <DisplayName="Type4">,
	EHDT_Type5  <DisplayName="Type5">,
	EHDT_Type6  <DisplayName="Type6">,
	EHDT_Type7  <DisplayName="Type7">,
	EHDT_Type8  <DisplayName="Type8">,
};

enum EElementaryType
{
	EETT_Elementary0    <DisplayName="Elementary0">,
	EETT_Elementary1    <DisplayName="Elementary1">,
	EETT_Elementary2    <DisplayName="Elementary2">,
	EETT_Elementary3    <DisplayName="Elementary3">,
	EETT_Elementary4    <DisplayName="Elementary4">,
	EETT_Elementary5    <DisplayName="Elementary5">,
	EETT_Elementary6    <DisplayName="Elementary6">,
	EETT_Elementary7    <DisplayName="Elementary7">,
	EETT_Elementary8    <DisplayName="Elementary8">,
	EETT_Elementary9    <DisplayName="Elementary9">,
};


enum EBloodEffectType
{
	BET_Type0           <DisplayName="BET_Type0">,
	BET_Type1           <DisplayName="BET_Type1">,
	BET_Type2           <DisplayName="BET_Type2">,
	BET_Type3           <DisplayName="BET_Type3">,
	BET_Type4           <DisplayName="BET_Type4">,
};

enum ESourceType
{
	EST_BaseActor,
	EST_TargetActor,
	EST_TargetPoint,
	EST_BaseWeapon1,
	EST_BaseWeapon2,
};


struct native MMOAnimGroup
{
	// idle
	var() name IdleAnim;
	var() name IdleWithHorseAnim;
	var() name FightIdleAnim;

	var() array<name> IdleRestAnims;
	var() array<name> FightIdleRestAnims;


	var() name WalkAnim;
	var() name WalkAnim_Oblique;
	var() name WalkWithStoneAnim;
	var() name RunAnim;
	var() name RunAnim_Oblique;
	var() name RunWithStoneAnim;
	var() name RideAnim;

	var() name FlyAnim;
	var() name FlyAnim_Oblique;


	var() name TurnLeftAnim;
	var() name TurnRightAnim;

	var() name JumpAnims[4];
	var() name BeatFallDownAnims[2];
	var() name BeatFlyAwayAnims[2];

	var() name SpawnAnim;
	var() name DieAnim;
	var() name DeadAnim;

	var() name BladeWeaponAnim[2];

	var() name WoundAnim;
	var() name WoundFaintAnim;
	var() name FaintAnim;

	var() name MissAnim;
	var() name ParryAnim; //block
	var() name BlockAnim; // parry

	// for human
	var() name SpineBoneName;
	var() name RootBoneName;

	var() array<name> PhysicSimulateRootBones;

	var() name AnimSpawn;
	var() name AnimSeeEnemy;
	var() name AnimLvUp;
 
	var() name AnimEscape;
	var() name AnimAlert;
	var() name AnimPain;
	var() name AnimCry;
	var() name AnimEffort;
	var() name AnimRoaring;
	var() name AnimHappy;
	//var() name AnimIdle;
	var() name AnimTired;

	var() name AnimTalkKey;

	structdefaultproperties
	{
		IdleAnim=Idle
		FightIdleAnim=Standby
		AnimSpawn=Spawn
		DieAnim=Death  //designer's knockoff naming
		DeadAnim=Dead  //
		AnimSeeEnemy=SeeEnemy
		AnimLvUp=LvUp
		WalkAnim=Walk
		WalkAnim_Oblique=Walk_Oblique
		RunAnim=Run
		RunAnim_Oblique=Run_Oblique
		FlyAnim=Fly
		FlyAnim_Oblique=Fly_Oblique
		TurnLeftAnim=TurnLeft
		TurnRightAnim=TurnRight
		FightIdleRestAnims.Add(standbyshow1)
		FightIdleRestAnims.Add(standbyshow2)



		WoundAnim=Wound
		AnimEscape=Escape
		AnimAlert=Alert
		AnimPain=Pain
		AnimCry=Cry
		AnimEffort=Effort
		AnimRoaring=Roaring
		AnimHappy=Happy
		AnimTired=Tired
		RootBoneName=Root
		SpineBoneName=Bip01-Spine1
		PhysicSimulateRootBones.Add(Bip01-L-Thigh);
		PhysicSimulateRootBones.Add(Bip01-R-Thigh);
		PhysicSimulateRootBones.Add(Bip01-L-Clavicle);
		PhysicSimulateRootBones.Add(Bip01-R-Clavicle);
		AnimTalkKey=Talk
	}
};

struct native WeaponTypeAnimSetPair
{
	var() EWeaponType WeaponType;
	var() array<AnimSet> AnimSets;
};

enum EProjectileTargetType
{
	// connect to a character
	EPTT_Actor,
	// connect to a fixed coordinate point
	EPTT_TargetPoint,
	// based on the caster's facing, distance is the MaxDistance value in the ability
	EPTT_UseCasterDirection,
	// based on the caster's facing, max distance is the MaxDistance value in the ability, with real-time detection, stops wherever it hits
	EPTT_UseCasterDirectionAndCollision,
};

// item action sound effect type list
enum EItemActionSoundType
{
	EIAST_Bag <DisplayName="Bag">,
	EIAST_Book <DisplayName="Book">,
	EIAST_ClothLeather <DisplayName="ClothLeather">,
	EIAST_FoodGeneric <DisplayName="FoodGeneric">,
	EIAST_Gems <DisplayName="Gems">,
	EIAST_Herb <DisplayName="Herb">,
	EIAST_LargeChain <DisplayName="LargeChain">,
	EIAST_SmallChain <DisplayName="SmallChain">,
	EIAST_Meat <DisplayName="Meat">,
	EIAST_MetalLarge <DisplayName="MetalLarge">,
	EIAST_MetalSmall <DisplayName="MetalSmall">,
	EIAST_ParchmentPaper <DisplayName="ParchmentPaper">,
	EIAST_Ring <DisplayName="Ring">,
	EIAST_RocksOre <DisplayName="RocksOre">,
	EIAST_Wand <DisplayName="Wand">,
	EIAST_WaterLiquid <DisplayName="WaterLiquid">,
	EIAST_WoodLarge <DisplayName="WoodLarge">,
	EIAST_WoodSmall <DisplayName="WoodSmall">,
};

enum DeathStyle
{
	DS_None             <DisplayName="DS_None">,
	DS_Anim             <DisplayName="DS_Anim">,
	DS_Ragdoll          <DisplayName="DS_Ragdoll">,
	DS_Explode          <DisplayName="DS_Explode">
};

enum ECollisionType2
{
	COLLIDE2_CustomDefault, // custom programmer set collison (PostEditChange() will restore collision to defaults when this is selected)
	COLLIDE2_NoCollision, // doesn't collide
	COLLIDE2_BlockAll, // blocks everything
	COLLIDE2_BlockWeapons, // only blocks zero extent things (usually weapons)
	COLLIDE2_TouchAll, // touches (doesn't block) everything
	COLLIDE2_TouchWeapons, // touches (doesn't block) only zero extent things
	COLLIDE2_BlockAllButWeapons, // only blocks non-zero extent things (Pawns, etc)
	COLLIDE2_TouchAllButWeapons, // touches (doesn't block) only non-zero extent things
	COLLIDE2_BlockWeaponsKickable // Same as BlockWeapons, but enables flags to be kicked by player physics
};

enum EQueueAction
{
	EQA_None,
	EQA_Lock,
	EQA_Unlock,
};

// players only have login and inrange, npc only have inrange spawn and revive
enum ESceneObjectAppearType
{
    ESOAT_Invalid    <DisplayName="ESOAT_Invalid">,
    ESOAT_Login      <DisplayName="ESOAT_Login">,
    ESOAT_InRange    <DisplayName="ESOAT_InRange">,
    ESOAT_Spawn      <DisplayName="ESOAT_Spawn">,
    ESOAT_Revive     <DisplayName="ESOAT_Revive">,

    ESOAT_Total <DisplayName="ESOAT_Total">
};

// time data, used to configure server time intervals, converted to milliseconds when exported to the server
struct native IntervalTimeData
{
	var() int Hours		<DisplayName="Hours"|ClampMin=0>;
	var() int Minutes	<DisplayName="Minutes"|ClampMin=0>;
	var() int Seconds	<DisplayName="Seconds"|ClampMin=0>;
};

// panel types with SLOT
/*
panel identifiers are currently const, not Enum, and a panel does not necessarily have an ItemSlot;
all UI uses SlotType so PanelType is not used again;
SlotType in as corresponds to ESlotPanelType,
if different Slots appear in a unified panel, add an ESPType_ or identify by itemtype;
hemu@snda
*/
enum ESlotPanelType
{
	ESPType_Bag,
	ESPType_SkillBar,
	ESPType_ItemBar,
	ESPType_MenuPanel,
	ESPType_MenuBar,
	ESPType_Charactor,
	ESPType_SkillPanel,
	ESPType_Shop,
	ESPType_ShopSell,
	ESPType_Warehouse,
	ESPType_StallSetting,    //stall sell
	ESPType_StallBuy,        //stall buy
	ESPType_Enhance,         //enhance UI (no longer needed, enhance was replaced by ESPType_EquipEnhance)
	ESPType_EquipLevelUp,     //weapon level up
	ESPType_WeaponExp,       //weapon proficiency
	ESPType_TradeP2P,        //P2P trade
	ESPType_FBAwards,        //dungeon rewards UI
	ESPType_PVPBattle,      //battlefield UI
	ESPType_PVPArena,       //arena
	ESPType_AvartaSell,      //future shop sells Avarta abilities, ItemSlot must be reset
	ESPType_GuildWarehouse,     //clan warehouse
	ESPType_Quest,
	ESPType_Hero,            //hero
	ESPType_Mount,            //mount collection
	ESPType_EquipSlotEnhance,   //enhance
	ESPType_SoulEmbed,		//soul embed UI
	ESPType_SoulLevelUp,		//soul level up UI
	ESPType_SoulUnEmbed,		//soul remove UI
	ESPType_WeaponSkill,		//soul forging UI
	ESPType_WeaponSkillSetting,	//soul forging upgrade UI
	ESPType_SkillTree,           //skill tree UI (interactive ability panel)
	ESPType_EquipGainHintBox,
	ESPType_ChatInput,
	ESPType_NPCDialog,
	ESPType_EquipEnhance,       //equipment slot enhance
	ESPType_EquipCompose,       //equipment compose
	ESPType_ReadMail,						//read mail
	ESPType_HeavenGasEquip,
	ESPType_HeavenGasBag,
	ESPType_HeavenGasBank,
	ESPType_ViewProfile,
	ESPType_Achievement,     // achievement rewards
	ESPType_MallStore,
	ESPType_Buff,
	ESPType_AuctionSell,
	ESPType_TeamAuctionPanel,
	ESPType_TeamAuctionList,
	ESPType_HeavenSoul,
	ESPType_ItemCollection,  //item recycling
	ESPType_AuctionItem,     //auction house
	ESPType_ChatMsgDisplay,//chat UI
	ESPType_QuestTraceDisplay,//quest tracking
	ESPType_SkillSetting,
	ESPType_Recover, //magic blood stone
	ESPType_DailySignIn,
	ESPType_ExpContainer,
	ESPType_BackDecoratedMakeUp,
	ESPType_BackDecoratedUpgrade,
	ESPType_BlackMarket,
	ESPType_BagResource,
	ESPType_WeaponSkillRplc,		//weapon ability replace
	ESPType_Invalid          //keep this last, used as the invalid value
	
};

enum EChat_Channel
{
	//ECC_All,
	ECC_World,
	
	ECC_Nearby,
	ECC_Guild,
	ECC_Team,
	ECC_Private,
	ECC_System, //system special, not a sendable channel
	ECC_Hint,//hint channel, similar to the system channel
	ECC_Trumpet,//small trumpet
	ECC_Trumpet_World, //world channel small trumpet
	ECC_Num,
};

enum EClothNoiseType
{
	ECNT_None,
	ECNT_Chain,
	ECNT_Plate,
	ECNT_Cloth,
	ECNT_Leather,
};

enum ENoiseActionType
{
	ENAT_Attack,
	ENAT_Move,
	ENAT_Jump,
	ENAT_Land
};


enum EClientActionConditionCheckType
{
	ECACCT_None,
	ECACCT_EquipItem_CheckClass,     // class condition when equipping
	ECACCT_EquipItem_CheckLv,        // level condition when equipping

	ECACCT_ALL_ActorLevel,          // level requirement

	ECACCT_UseItem_CheckClass,      // class condition when using
	ECACCT_UseItem_CheckLv,
	ECACCT_UseItem_CheckPhyAtk,     // item attack power condition
	ECACCT_UseItem_CheckMagAtk,     // magic attack power condition
	ECACCT_UseItem_CheckTaoAtk,     // taoist attack power condition
	ECACCT_UseItem_CheckStallState, // stall state check
	ECACCT_UseItem_CheckBind,       // bind state check

	ECACCT_MailSend_CheckReceiver,
	ECACCT_MailSend_CheckReceiverSelf,
	ECACCT_MailSend_CheckSubject,
	ECACCT_MailSend_CheckContent,
	ECACCT_MailSend_CheckContentMaxLen,
	ECACCT_Item_CheckAllInContainerIfInPanelUsing,//whether any item in the container is being operated in a panel
	ECACCT_Item_CheckInEnhance,     // in-enhancing state check
	ECACCT_Item_CheckInWeaponExp,     // extracting proficiency
	ECACCT_Item_CheckInTrade,       // in-P2P-trade state check
	ECACCT_Item_CheckInPanelUsing,//whether being operated in the UI
	ECACCT_Item_CheckInSoulEmbed,//whether in soul embedding
	ECACCT_Item_CheckInSoulUnEmbed,//whether in soul removal
	ECACCT_Item_CheckInSoulLevelUp,//whether in soul level up
	ECACCT_TradeItem_CheckDurablility,//durability check
	ECACCT_TradeItem_CheckTaskItem,      // quest item check
	ECACCT_TradeItem_CheckItemSign,		// check whether already signed
	ECACCT_Item_CheckInEnhanceOpened,       //enhance UI opened
	ECACCT_Item_CheckInTradeOpend,       //trade UI opened
	ECACCT_Item_CheckCanStall,          //check whether the item can be sold at a stall
	ECACCT_Item_CheckCanTrade,          //check whether the item can be traded
	ECACCT_Item_CheckMySex,            // item usage gender

	ECACCT_Hero_EquipItem_CheckClass,     // hero class condition when equipping
	ECACCT_Hero_EquipItem_CheckLv,        // hero level condition when equipping
	ECACCT_CurrentScene_Has_Lines,        // whether the current scene supports line switching
	ECACCT_MainPlayer_Is_Combating,       // whether the main character is in combat
	ECACCT_MainPlayer_Is_Trading,        // whether the main character is trading
	ECACCT_Quest_Item_Collection,       // whether the main character is in item recycling

	ECACCT_EquipLevelUp_CheckDurability,       // weapon level up durability check
	ECACCT_Material_CheckEquipTool,             // when refining, check whether the refining knife is equipped
	ECACCT_Material_WeaponDuration,             // whether the refining knife still has durability
	ECACCT_Material_BagFull,                    // when refining, whether the bag is full
};

enum EBTEditType
{
	EBET_Scene,
	EBET_AI,
};

// html link type
enum ELinkType 
{
	ELinkType_NPCNav,
	ELinkType_CoordNav,
	ELinkType_NavPointName,
	ELinkType_ItemTip,
	ELinkType_ChatPlayer,
	ELinkType_KillInfoPlayer,
	ELinkType_SkillTip,
	ELinkType_QuestTraceTitle,
	ELinkType_QuestTraceAccept,
	ELinkType_QuestTraceComplete,
	ELinkType_Achievement,
	ELinkType_AchievementClass,
	ELinkType_OpenPanel,
	ELinkType_NavFly,
	ELinkType_DoodadNav,
	ELinkType_ItemNav,
	ELinkType_WebPath,
	ELinkType_OpenMap,
	ELinkType_ChatItemtTip,
	ELinkTpye_AuctionItem,
	ELinkType_AuctionPlayer
};

enum EPassiveMoveType
{
	PassiveMoveType_None,
	PassiveMoveType_DirectlyMove,
	PassiveMoveType_Throw,
	PassiveMoveType_Fear,
};


enum EAnimBlendType
{
	EABT_FullBody,
	EABT_UpperBody,
	EABT_LeftFoot,
	EABT_RightFoot,
	EABT_Head,
	EABT_Tail,
};

enum TurnButtonStats
{
	TBS_Left,
	TBS_Right,
	TBS_None,
};

enum EAnimLevel
{
	EAL_LowWound,
	EAL_Action,
	EAL_HighWound,
	EAL_State,
};

enum EMoveCtrlKey
{
	EMCK_Empty,
	EMCK_Left,
	EMCK_Right,
	EMCK_Front,
	EMCK_Back,
};

enum ELocalAICheckType
{
    ELAICT_TASK,
    ELAICT_KILL_MONSTER,
    ELAICT_GET_ITEM,
};

enum EBuffType
{
    EBuff_Normal,
    EBuff_Function,
};

enum EScreenMsgType{
	EScnMsgType_Invalid,
	EScnMsgType_Skill,
	EScnMsgType_SkillSlot,
	EScnMsgType_MainSkillLevelUp,
	EScnMsgType_Achievement,
	EScnMsgType_FBAchievement,
	EScnMsgType_FBUnlock
};

enum EAuctionTypeMain
{
    EATM_ALL,               // all
	EATM_WEAPON_WARRIOR,    // warrior weapon
    EATM_WEAPON_ASSASSIN,   // assassin weapon
    EATM_WEAPON_WIZARD,     // mage weapon
    EATM_WEAPON_ARCHER,     // archer weapon
    EATM_ARMOR,             // armor
    EATM_JEWELRY,           // jewelry
    EATM_SEPCIAL_EQUIP,     // special equipment
    EATM_SPIRIT_STALL,      // spirit stone
    EATM_EQUIP_FORMULA,     // equipment formula
    EATM_MATERIAL,          // material
    EATM_RIDE,              // mount
    EATM_DRUG,              // elixir
    EATM_RESTORE_DRUG,      // restorative medicine
    EATM_OTHER,             // other
};

enum EAuctionTypeSub_Weapon_Warrior
{
    EATS_WW_ALL,            // all
	EATS_WW_DOUBLE_SWORD,   // two-handed sword
    EATS_WW_DOUBLE_AXE,     // dual axes
    EATS_WW_SWORD_SHIELD,   // sword and shield
};

enum EAuctionTypeSub_Weapon_Assassin
{
    EATS_WA_ALL,            // all
    EATS_WA_DAGGER,         // dagger
    EATS_WA_LONG_SHANK,     // long hilt
    EATS_WA_CHAIN_BLADE,    // chain blade
};

enum EAuctionTypeSub_Weapon_Wizard
{
    EATS_WI_ALL,            // all
    EATS_WI_STAVES,         // staff
    EATS_WI_BEAT,           // magic hammer
    EATS_WI_BLADE,          // magic blade
};

enum EAuctionTypeSub_Weapon_Archer
{
    EATS_WR_ALL,            // all
    EATS_WR_HEAVY_CROSSBOW, // heavy crossbow
    EATS_WR_LIGHT_CROSSBOW, // light crossbow
    EATS_WR_BOW,            // bow
};

enum EAuctionTypeSub_Armor
{
    EATS_A_ALL,             // all
    EATS_A_CAP,             // hat
    EATS_A_SHOULDER,        // shoulder
    EATS_A_CLOTHES,         // upper garment
    EATS_A_TROUSERS,        // lower garment
    EATS_A_WRISTER,         // wristguard
    EATS_A_WAISTBAND,       // belt
    EATS_A_SHOES,           // shoes
};

enum EAuctionTypeSub_Jewelry
{
    EATS_J_ALL,             // all
    EATS_J_NECKLACE,        // necklace
    EATS_J_BRACELET,        // bracelet
    EATS_J_RING,            // ring
};

enum EAuctionTypeSub_Sepcial_Equip
{
    EATS_SE_ALL,            // all
    EATS_SE_CLOAK,          // cloak
    EATS_SE_FASHION,        // fashion
};

enum EAuctionTypeSub_Spirit_Stall
{
    EATS_SS_ALL,            // all
    EATS_SS_RED,            // red spirit stone
    EATS_SS_YELLOW,         // yellow spirit stone
    EATS_SS_BLUE,           // blue spirit stone
    EATS_SS_GREEN,          // green spirit stone
    EATS_SS_PURPLE,         // purple spirit stone
    EATS_SS_COLOUR,         // colored spirit stone
};

enum EAuctionTypeSub_Equip_Formula
{
    EATS_EF_ALL,            // all
    EATS_EF_WEAPON,         // weapon
    EATS_EF_ARMOR,          // armor
    EATS_EF_JEWELRY,        // jewelry
};

enum EAuctionTypeSub_Material
{
    EATS_M_ALL,             // all
    EATS_M_INTENSIFY,       // physique enhancement
    EATS_M_ENGRAVE,         // inscription
    EATS_M_EQUIP_BENISON,   // equipment blessing
};

enum EAuctionTypeSub_Ride
{
    EATS_R_ALL,             // all
};

enum EAuctionTypeSub_Drug
{
    EATS_D_ALL,             // all
    EATS_D_STRENGTH,        // strength elixir
    EATS_D_SPEED,           // speed elixir
    EATS_D_INTELLECT,       // wisdom elixir
    EATS_D_SOMA,            // constitution elixir
    EATS_D_STAMINA,         // stamina elixir
};

enum EAuctionTypeSub_Restore_Drug
{
    EATS_RD_ALL,            // all
    EATS_RD_POTION,         // potion
    EATS_RD_LIQUOR,         // liquor
};

enum EAuctionTypeSub_Other
{
    EATS_O_ALL,             // all
};

enum EATTACK_FLAG
{
    EAT_FLAG_ZHURI,           // Sun Chasing
    EAT_FLAG_LIEHUO,          // Raging Fire
    EAT_FLAG_BANYUE,          // Half Moon
    EAT_FLAG_CISHA,           // Assassinate
	EAT_FLAG_MOFADUN,         // Magic Shield
	EAT_FLAG_YINSHEN,         // Invisibility
	EAT_FLAG_MAX,
};

enum ESceneHintUpdate
{
    ESHU_NONE,            // none
    ESHU_HP,              // health bar
	ESHU_NAME,            // name
    ESHU_ALL,             // all
};

// enum EDoodadSoundType
// {
//     EDST_WOOD,
//     EDST_VEGETATION,
//     EDST_METAL,
// };

struct native ShopItemTemplate
{
	var() transient Item ShopItem			<DisplayName="ShopItem">;
	var() transient int ItemCount			<DisplayName="ItemCount">;
	var() transient EMONEY_TYPE CostType	<DisplayName="CostType">;
	var() transient int CostValue			<DisplayName="CostValue">;
	var   transient int ShopSlot; // records the index in the shop, needed for joint debugging
};

struct native ShopData
{
	var() transient int Id							<DisplayName="ID">;
	var() transient init string ShopName					<DisplayName="ShopName">;
	var() transient init array<ShopItemTemplate> ItemList	<DisplayName="ItemList">;
};

struct native OperationItemBagPos
{
	var int _ConID;
	var int _SlotID;
	var bool _IsValid;
	structdefaultproperties
	{
		_ConID=-1
		_SlotID=-1
		_IsValid=true
	}
};

struct native ParticlesByElementType
{
	var() EElementaryType Type;
	var() array<ParticleSystem> ParticleSystems;
};

struct native SoundCueByElementaryType
{
	var() EElementaryType Type;
	var() array<SoundCue> Sounds;
};

cpptext
{
	static INT ConvertToMS(const FIntervalTimeData& IntervalTime);
}

defaultproperties
{
}
