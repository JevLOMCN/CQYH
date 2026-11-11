//
// 通用类型定义,避免脚本循环依赖
//
class TypeDefine extends Object
	dependson(ServerEnum)
	native;

/*
 * 体形类型,仅用于音效
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

// 全部装备
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
////// 时装装备
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
EWT_Weapon_0=空手
EWT_Weapon_1=单持挥砍类(单手剑)
EWT_Weapon_2=单持敲打类(单手锤)
EWT_Weapon_3=并持短柄重型类(双手剑)
EWT_Weapon_4=并持长柄重型类(长柄斧)
EWT_Weapon_5=双持重型类(双斧)
EWT_Weapon_6=并持东方长柄类(长矛)
EWT_Weapon_7=双持手套类(拳刺)
EWT_Weapon_8=双持短兵类(双匕)
EWT_Weapon_9=双持链刃类(链刃)
EWT_Weapon_10=双持手弩类(双弩)
EWT_Weapon_11=弩
EWT_Weapon_12=弓
EWT_Weapon_13=单持短柄类(刀)
EWT_Weapon_14=法杖
EWT_Weapon_14=法杖
EWT_Weapon_15=大型盾
EWT_Weapon_16=小型盾
EWT_Weapon_17=法器
EWT_Weapon_18=剑
EWT_Weapon_19=扇子
EWT_Weapon_20=棍
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
* 身体部分模型的类型
* 用于在合并时区分不同模型部位
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
 * 物品材质类型,仅用于音效,定义所有物品材质类型,包含武器,服装
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

// UI鼠标状态
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

// 玩家攻击动画 hemu@snda 用于AnimMixMatrix
//以后添加需要回招动画的技能动画在这里添加 DisplayName为Anim的名字
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
// 玩家状态 hemu@snda 用于AnimMixMatrix
//玩家的状态在这里添加 DisplayName为Anim的名字
enum EAnimMixTarget
{
	EAMT_Idle	    <DisplayName="attack_idle">,
	EAMT_Run	    <DisplayName="Run">,
};

//song@snda.com 测试服务器专属属性
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
 	EMT_PrepareStall, // TODO:以后重构为面的StallState，并且把这些状态的状态机划分出来
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
//     EIBT_NONE <DisplayName="None">,      //不绑定
//     EIBT_BOU <DisplayName="BindOnUse">,       //使用绑定, bind on use
//     EIBT_BOE <DisplayName="BindOnEquip">,       //装备绑定, bind on equip
//     EIBT_BOP <DisplayName="BindOnPickup">,       //拾取绑定, bind on pick up
//     EIBT_BOT <DisplayName="BindOnTrade">,       //交易绑定, bind on trade
//     EIBT_BINDED <DisplayName="AlreadyBound">     //已绑定
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
	// 常规粒子效果(播放一下就自动消失)
	EMIT_Normal,
	// 拖尾
	EMIT_Trail,
		// 表现为直线或曲线飞向一个目标
	EMIT_FlyTo,
		// 闪电链或吸精大法效果
	EMIT_ConnectTo,
		// 环绕自己身旋转
	EMIT_Rotate,
		// 环绕MESH
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
	// 使用Duration或自身Duration(PariticleSystem)
	ACBT_StopWhenConstDurationPass,
	ACBT_StopWhenAbilityStateEnd,
	ACBT_StopWhenAbilityEnd,
};

// 材质效果类型
enum EMaterialEffectType
{
	// 无效果
	EMET_None,
	// 修改材质参数
	EMET_ChangeMaterialProperty,
	// 透明度
	EMET_LoadOpacityMaterial,
	// 替换材质
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


// 为了应对门派概念的反复修改,枚举名用A,B,C..等表示,displayname用当前版本来表示
enum EPlayerClass
{
	EPCT_PlayerClassA <DisplayName="E_PlayerClassA">,
	EPCT_PlayerClassB <DisplayName="E_PlayerClassB">,
	EPCT_PlayerClassC <DisplayName="E_PlayerClassC">,
	EPCT_PlayerClassD <DisplayName="E_PlayerClassD">,
	EPCT_PlayerClassE <DisplayName="E_PlayerClassE">, // 道士
	// 1.05 backport: values match the server (clsSpear=5, clsSorcerer=6, clsNpc=7, none/any=8); names match the 1.05 client
	EPCT_PlayerClassG <DisplayName="E_PlayerClassG">, // Lancer (Spear)
	EPCT_PlayerClassH <DisplayName="E_PlayerClassH">, // Sorcerer
	EPCT_PlayerClassF <DisplayName="E_PlayerClassF">, // npc
	EPCT_None <DisplayName="E_None">,
};

//UI表现上对技能的归类
enum ESPELL_UI_TYPE
{
	ESUIType_Active                    <DisplayName="ESUIType_None">,
	ESUIType_Passive                   <DisplayName="ESUIType_Passive">,
	ESUIType_Counter                   <DisplayName="ESUIType_Counter">,
	ESUIType_Chase                     <DisplayName="ESUIType_Chase">,
	ESUIType_Terminate                 <DisplayName="ESUIType_Terminate">,
	ESUIType_Total                     <DisplayName="ESUIType_Total">,
};

// 游戏内的货币类型
enum ECurrencyType
{
	ECUT_GameGold <DisplayName="GameGold">,
	ECUT_RMB <DisplayName="RMB">
};

// 伤害结果类型
/*
Result0=无效
Result1=未命中
Result2=免疫
Result3=格档
Result4=招架
Result5=闪避
Result6=普通攻击
Result7=爆击
Result8=空效果
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
HitType0=无类型
HitType1=挥砍
HitType2=戳刺
HitType3=射击
HitType4=敲打
HitType5=肉搏
HitType6=元素
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
	var() name ParryAnim; //格档
	var() name BlockAnim; // 招架

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
		DieAnim=Death  //策划山寨的命名
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
	// 连接一角色
	EPTT_Actor,
	// 连接固定坐标点
	EPTT_TargetPoint,
	// 依据施法者的朝向,距离为技能里的MaxDistance值
	EPTT_UseCasterDirection,
	// 依据施法者的朝向,最大距离为技能里的MaxDistance值,并带有实时检测,碰到哪里就停在哪里
	EPTT_UseCasterDirectionAndCollision,
};

// 道具动作音效类型列表
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

// 玩家只有login和inrange, npc只有inrange spawn和revive
enum ESceneObjectAppearType
{
    ESOAT_Invalid    <DisplayName="ESOAT_Invalid">,
    ESOAT_Login      <DisplayName="ESOAT_Login">,
    ESOAT_InRange    <DisplayName="ESOAT_InRange">,
    ESOAT_Spawn      <DisplayName="ESOAT_Spawn">,
    ESOAT_Revive     <DisplayName="ESOAT_Revive">,

    ESOAT_Total <DisplayName="ESOAT_Total">
};

// 时间数据，供配置服务器的时间间隔用，导出到服务器时转换成毫秒
struct native IntervalTimeData
{
	var() int Hours		<DisplayName="Hours"|ClampMin=0>;
	var() int Minutes	<DisplayName="Minutes"|ClampMin=0>;
	var() int Seconds	<DisplayName="Seconds"|ClampMin=0>;
};

// 带SLOT的面板类型
/*
面板标识目前const不是Enum,且面板不一定带有ItemSlot;
各处UI全部都是SlotType 故而没有再用PanelType;
as中SlotType 对应 ESlotPanelType,
统一面板中出现不同Slot则应增加 ESPType_ 或者用itemtype标识;
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
	ESPType_StallSetting,    //摆摊 卖出
	ESPType_StallBuy,        //摊位买入
	ESPType_Enhance,         //强化界面(不要了，强化被ESPType_EquipEnhance替代)
	ESPType_EquipLevelUp,     //武器升级
	ESPType_WeaponExp,       //武器熟练度
	ESPType_TradeP2P,        //P2P交易
	ESPType_FBAwards,        //副本奖励界
	ESPType_PVPBattle,      //战场界面
	ESPType_PVPArena,       //竞技场
	ESPType_AvartaSell,      //未来商店出售Avarta技能 则需要重设ItemSlot
	ESPType_GuildWarehouse,     //家族仓库
	ESPType_Quest,
	ESPType_Hero,            //英雄
	ESPType_Mount,            //坐骑收集
	ESPType_EquipSlotEnhance,   //强化
	ESPType_SoulEmbed,		//安装灵魂界面
	ESPType_SoulLevelUp,		//升级灵魂界面
	ESPType_SoulUnEmbed,		//移除灵魂界面
	ESPType_WeaponSkill,		//铸魂界面
	ESPType_WeaponSkillSetting,	//铸魂升级界面
	ESPType_SkillTree,           //技能树界面（交互技能面板）
	ESPType_EquipGainHintBox,
	ESPType_ChatInput,
	ESPType_NPCDialog,
	ESPType_EquipEnhance,       //装备栏强化
	ESPType_EquipCompose,       //装备合成
	ESPType_ReadMail,						//读邮件
	ESPType_HeavenGasEquip,
	ESPType_HeavenGasBag,
	ESPType_HeavenGasBank,
	ESPType_ViewProfile,
	ESPType_Achievement,     // 成就奖励
	ESPType_MallStore,
	ESPType_Buff,
	ESPType_AuctionSell,
	ESPType_TeamAuctionPanel,
	ESPType_TeamAuctionList,
	ESPType_HeavenSoul,
	ESPType_ItemCollection,  //道具回收
	ESPType_AuctionItem,     //拍卖行
	ESPType_ChatMsgDisplay,//聊天界面
	ESPType_QuestTraceDisplay,//任务追踪
	ESPType_SkillSetting,
	ESPType_Recover, //魔血石
	ESPType_DailySignIn,
	ESPType_ExpContainer,
	ESPType_BackDecoratedMakeUp,
	ESPType_BackDecoratedUpgrade,
	ESPType_BlackMarket,
	ESPType_BagResource,
	ESPType_WeaponSkillRplc,		//武器技能替换
	ESPType_Invalid          //保持这个在最后作为无效值用
	
};

enum EChat_Channel
{
	//ECC_All,
	ECC_World,
	
	ECC_Nearby,
	ECC_Guild,
	ECC_Team,
	ECC_Private,
	ECC_System, //系统特殊，非可发送频道
	ECC_Hint,//提示频道，和系统频道差不多
	ECC_Trumpet,//小喇叭
	ECC_Trumpet_World, //世界频道的小喇叭
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
	ECACCT_EquipItem_CheckClass,     // 装备时职业条件
	ECACCT_EquipItem_CheckLv,        // 装备时等级条件

	ECACCT_ALL_ActorLevel,          // 等级需求

	ECACCT_UseItem_CheckClass,      // 使用时职业条件
	ECACCT_UseItem_CheckLv,
	ECACCT_UseItem_CheckPhyAtk,     // 物品攻击力条件
	ECACCT_UseItem_CheckMagAtk,     // 魔法攻击力条件
	ECACCT_UseItem_CheckTaoAtk,     // 道术攻击力条件
	ECACCT_UseItem_CheckStallState, // 摆摊状态检查
	ECACCT_UseItem_CheckBind,       // 绑定状态检查

	ECACCT_MailSend_CheckReceiver,
	ECACCT_MailSend_CheckReceiverSelf,
	ECACCT_MailSend_CheckSubject,
	ECACCT_MailSend_CheckContent,
	ECACCT_MailSend_CheckContentMaxLen,
	ECACCT_Item_CheckAllInContainerIfInPanelUsing,//容器中的物品是否有正在界面操作中的
	ECACCT_Item_CheckInEnhance,     // 强化中状态检查
	ECACCT_Item_CheckInWeaponExp,     // 抽熟练度
	ECACCT_Item_CheckInTrade,       // P2P交易中状态检查
	ECACCT_Item_CheckInPanelUsing,//是否在UI界面操作中
	ECACCT_Item_CheckInSoulEmbed,//是否灵镶嵌中
	ECACCT_Item_CheckInSoulUnEmbed,//是否灵拆除中
	ECACCT_Item_CheckInSoulLevelUp,//是否灵升级中
	ECACCT_TradeItem_CheckDurablility,//耐久度检测
	ECACCT_TradeItem_CheckTaskItem,      // 任务物品检查
	ECACCT_TradeItem_CheckItemSign,		// 检查是否已签名
	ECACCT_Item_CheckInEnhanceOpened,       //强化界面打开
	ECACCT_Item_CheckInTradeOpend,       //交易界面打开
	ECACCT_Item_CheckCanStall,          //判断物品是否能摆摊出售
	ECACCT_Item_CheckCanTrade,          //判断物品是否能交易
	ECACCT_Item_CheckMySex,            // 物品使用性别

	ECACCT_Hero_EquipItem_CheckClass,     // 英雄装备时职业条件
	ECACCT_Hero_EquipItem_CheckLv,        // 英雄装备时等级条件
	ECACCT_CurrentScene_Has_Lines,        // 当前场景是否支持换线
	ECACCT_MainPlayer_Is_Combating,       // 主角是否在战斗中
	ECACCT_MainPlayer_Is_Trading,        // 主角是否在交易中
	ECACCT_Quest_Item_Collection,       // 主角是否在道具回收中

	ECACCT_EquipLevelUp_CheckDurability,       // 武器升级耐久度检查
	ECACCT_Material_CheckEquipTool,             // 洗练的时候检查是否装备洗练刀
	ECACCT_Material_WeaponDuration,             // 洗练刀的是否还有耐久
	ECACCT_Material_BagFull,                    // 洗练的时候，背包是否已满
};

enum EBTEditType
{
	EBET_Scene,
	EBET_AI,
};

// html链接类型
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
    EATM_ALL,               // 全部
	EATM_WEAPON_WARRIOR,    // 战士武器
    EATM_WEAPON_ASSASSIN,   // 刺客武器
    EATM_WEAPON_WIZARD,     // 法师武器
    EATM_WEAPON_ARCHER,     // 弓手武器
    EATM_ARMOR,             // 防具
    EATM_JEWELRY,           // 首饰
    EATM_SEPCIAL_EQUIP,     // 特殊装备
    EATM_SPIRIT_STALL,      // 灵石
    EATM_EQUIP_FORMULA,     // 装备配方
    EATM_MATERIAL,          // 材料
    EATM_RIDE,              // 坐骑
    EATM_DRUG,              // 药剂
    EATM_RESTORE_DRUG,      // 恢复药品
    EATM_OTHER,             // 其它
};

enum EAuctionTypeSub_Weapon_Warrior
{
    EATS_WW_ALL,            // 全部
	EATS_WW_DOUBLE_SWORD,   // 双手剑
    EATS_WW_DOUBLE_AXE,     // 双斧
    EATS_WW_SWORD_SHIELD,   // 剑盾
};

enum EAuctionTypeSub_Weapon_Assassin
{
    EATS_WA_ALL,            // 全部
    EATS_WA_DAGGER,         // 匕首
    EATS_WA_LONG_SHANK,     // 长柄
    EATS_WA_CHAIN_BLADE,    // 链刃
};

enum EAuctionTypeSub_Weapon_Wizard
{
    EATS_WI_ALL,            // 全部
    EATS_WI_STAVES,         // 法杖
    EATS_WI_BEAT,           // 法搥
    EATS_WI_BLADE,          // 法刃
};

enum EAuctionTypeSub_Weapon_Archer
{
    EATS_WR_ALL,            // 全部
    EATS_WR_HEAVY_CROSSBOW, // 重弩
    EATS_WR_LIGHT_CROSSBOW, // 累弩
    EATS_WR_BOW,            // 弓
};

enum EAuctionTypeSub_Armor
{
    EATS_A_ALL,             // 全部
    EATS_A_CAP,             // 帽子
    EATS_A_SHOULDER,        // 肩膀
    EATS_A_CLOTHES,         // 上衣
    EATS_A_TROUSERS,        // 下衣
    EATS_A_WRISTER,         // 护腕
    EATS_A_WAISTBAND,       // 腰带
    EATS_A_SHOES,           // 鞋子
};

enum EAuctionTypeSub_Jewelry
{
    EATS_J_ALL,             // 全部
    EATS_J_NECKLACE,        // 项链
    EATS_J_BRACELET,        // 手镯
    EATS_J_RING,            // 戒指
};

enum EAuctionTypeSub_Sepcial_Equip
{
    EATS_SE_ALL,            // 全部
    EATS_SE_CLOAK,          // 披风
    EATS_SE_FASHION,        // 时装
};

enum EAuctionTypeSub_Spirit_Stall
{
    EATS_SS_ALL,            // 全部
    EATS_SS_RED,            // 红灵石
    EATS_SS_YELLOW,         // 黄灵石
    EATS_SS_BLUE,           // 蓝灵石
    EATS_SS_GREEN,          // 绿灵石
    EATS_SS_PURPLE,         // 紫灵石
    EATS_SS_COLOUR,         // 彩色灵石
};

enum EAuctionTypeSub_Equip_Formula
{
    EATS_EF_ALL,            // 全部
    EATS_EF_WEAPON,         // 武器
    EATS_EF_ARMOR,          // 防具
    EATS_EF_JEWELRY,        // 首饰
};

enum EAuctionTypeSub_Material
{
    EATS_M_ALL,             // 全部
    EATS_M_INTENSIFY,       // 体格强化
    EATS_M_ENGRAVE,         // 铭文
    EATS_M_EQUIP_BENISON,   // 装备祝福
};

enum EAuctionTypeSub_Ride
{
    EATS_R_ALL,             // 全部
};

enum EAuctionTypeSub_Drug
{
    EATS_D_ALL,             // 全部
    EATS_D_STRENGTH,        // 力量药剂
    EATS_D_SPEED,           // 迅捷药剂
    EATS_D_INTELLECT,       // 智慧药剂
    EATS_D_SOMA,            // 体质药剂
    EATS_D_STAMINA,         // 耐力药剂
};

enum EAuctionTypeSub_Restore_Drug
{
    EATS_RD_ALL,            // 全部
    EATS_RD_POTION,         // 药水
    EATS_RD_LIQUOR,         // 酒
};

enum EAuctionTypeSub_Other
{
    EATS_O_ALL,             // 全部
};

enum EATTACK_FLAG
{
    EAT_FLAG_ZHURI,           // 逐日
    EAT_FLAG_LIEHUO,          // 烈火
    EAT_FLAG_BANYUE,          // 半月
    EAT_FLAG_CISHA,           // 刺杀
	EAT_FLAG_MOFADUN,         // 魔法盾
	EAT_FLAG_YINSHEN,         // 隐身
	EAT_FLAG_MAX,
};

enum ESceneHintUpdate
{
    ESHU_NONE,            // 无
    ESHU_HP,              // 血条
	ESHU_NAME,            // 名字
    ESHU_ALL,             // 全部
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
	var   transient int ShopSlot; // 这个记录商店中的下标，联调时需要
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
