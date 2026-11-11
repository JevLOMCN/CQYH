//
// 角色属性管理，战斗属性数值计算
//
class ActorAttribute extends Object
	native
	editinlinenew
	dependson(IMMOActor)
	hidecategories(Object);


var array<INT> AttributeList;
//---------------------------------------------------------------
// 战斗属性结构、计算函数
//---------------------------------------------------------------

// 等级属性
struct native LevelAttribute
{

	// 等级
	var() int Level;

	// 最大等级
	var() int MaxLevel;

	// 当前经验
	var() int CurrentExp;

	// 升级经验
	var() int MaxExp;
};

// 基础属性
struct native BaseAttribute
{
	// 力道 影响外攻
	var() int Strength;

	// 气劲 影响内攻
	var() int Intellect;

	// 筋骨 影响HP HP回复 外防
	var() int Stamina;

	// 内息 影响角色的MP、MP回复、内功防御
	var() int Spirit;

	// 身法 影响角色的闪避、化劲
	var() int Agility;

	// 意念 影响角色的命中，致命
	var() int Impression;
};

// 攻击属性
struct native AttackAttribute
{
	// 外攻攻击
	var int CurrentMeleeAttack;
	var() int MeleeAttackMin;
	var() int MeleeAttackMax;

	// 内攻攻击
	var int CurrentSpellAttack;
	var() int SpellAttackMin;
	var() int SpellAttackMax;

	// 命中率 角色攻击命中的程度，普通攻击、技能攻击都有命中问题
	var() float HitRating;

	// 致命 战斗中出现加倍伤害的几率，单位：% 
	var() float CriticalChance;

	// 攻击浮动值范围
	var() Vector2D AttackRatioRange;
};

// 防御属性
struct native DefenseAttribute
{
	// 外防 物理伤害用护甲来减免伤害
	var() int Armor;
	
	// 内防 法术抗性 法术伤害用搞性来减免伤害
	var() int SpellResistance;

	// 闪避 角色躲避攻击的能力
	var() float Dodge;

	// 化劲 降低被致命的几率，单位：%  就是挈性
	var() float Resilience;

	// 格挡参数
	var() float BlockChance;
	var() float BlockReduce;
};

// 扩展属性
struct native ExtendAttribute
{
	// 伤害加成
	var() int DamageBonus;
};

// 伤害计算结果
struct native DamageResult
{
	// 伤害类型
	var EDamageResultType Type;

	// HP增加或减少
	var int HPChange;

	// MP增加或减少
	var int MPChange;

	// 爆击伤害量
	var int CriticalHit;
	//主角自己受伤
	var bool MainRole; 
	// 格挡减免伤害量
	var int BlockReduce;
};

//add by jimzhang-------Reputation-------------------------------------------------
struct native Reputation 
{
    var int  ForceID;
    var int  ReputationData;
	var bool  ActiveState; 
	var bool  AwardState;
};

struct native HeavenAttribute
{
	var int  heavenLevel;
	var int  heavenExp;
};

//var(MMO) Reputation ReputationList[ MAX_NPC_FORCE_ID ];
var array<Reputation> ReputationArray;

var      int        MoveTypeId;//移动模式，0正常，1棋子移动模式
var      int        MoveDirId; //移动方向副参数模式，0默认，1234，左右前后
var      byte       PkMode;
var      int        PkValue;
var      byte       TitleID;
var      int        Growth;     //成长行
var      bool       IsGreyName; //是否灰名
var      byte       PetLevel;
var      string     FrameEx1;   //名字特殊效果1
var      string     FrameEx2;   //名字特殊效果2
var      string     ColorEx1;   //名字特殊颜色1
var      string     ColorEx2;   //名字特殊颜色2
//------------------------------------------------------------------------

var localized string ClassName[6];

// 等级属性
var(MMO) LevelAttribute LevelAttributes;

// 基础属性
var(MMO) BaseAttribute BaseAttributes;

// 基础伤害
var(MMO) AttackAttribute BaseAttackAttribute;

// 基础防御
var(MMO) DefenseAttribute BaseDefenseAttribute;



// 当前血量
var(MMO) int HP;

// 最大血量
var(MMO) int MaxHP;

// 当前蓝量
var(MMO) int MP;

// 最大蓝量
var(MMO) int MaxMP;

// 是否能回血、回蓝
var(MMO) bool EnableRegenHPMP;

// 门派
var(MMO) int Faction;

var(MMO) EPlayerClass   ClassType;
var(MMO) EGenderType    GenderType;

var init array<int> ClientPlayerAttributes; //服务器同步的数组

// 装备附加属性
var BaseAttribute       BonusAttributes;
var AttackAttribute     BonusAttackAttributes;
var DefenseAttribute    BonusDefenseAttributes;

//境界
var HeavenAttribute      heavenAttributes;
//坐骑
var int MountID;
var array<int> MountList;
// wing (wings.txt ID) the character wears, 0: none
var int WingID;

var bool byCombatPoseOn;

var int HpPercent;

var int HeroID;

var int HairType;
var int HairColor;
var int FaceType;
var int SkinColor;

native function Reset();
native function SetHP(int val);
native function SetMaxHP(int val);
native function float GetHPPercent();

native function  SetMP(int val);
native function  SetMaxMP(int val);
native function float GetMPPercent();

native function  SetPkValue(int val);
native function  int GetPkValue();

native function int GetCurMp();
native function int GetMaxMp();
native function int GetCurHp();
native function int GetMaxHp();
native function int GetAttribute( int AttrId);

//native function SetAttribute( int AttrId,);


native function SetReputation(int Index, int ForceID, int ReputationData);
native function int GetReputation(int ForceID);
native function SetActiveState(int Index, int ForceID, bool ActiveState);
native function bool GetActiveState(int ForceID);

native function SetAwardState(int Index, int ForceID, bool AwardState);
native function bool GetAwardState(INT ForceID);

//add by jimzhang-------Reputation-------------------------------------------------
//event         SetReputation(int Index, int ForceID, int ReputationData)
//{ 
//	local int i;
//	local Reputation buf;
//	local bool AddNew;
//	AddNew = true;

//	for(i = ReputationArray.Length - 1; i >= 0; --i)
//	{
//		if(ReputationArray[i].ForceID == ForceID){
//			ReputationArray[i].ReputationData = ReputationData; 
//			AddNew = false;
//			break;		
//		}
//	}

//	if(AddNew){
//		buf.ForceID = ForceID;
//		buf.ReputationData = ReputationData; 
//		ReputationArray.AddItem(buf);

//		`log("---------------------------------SetReputation ReputationArray.size="$ReputationArray.Length );
//	}
//	//ReputationList[Index].ForceID        = ForceID;
//	//ReputationList[Index].ReputationData = ReputationData; 
//}

//event  int    GetReputation(int ForceID)
//{ 
//	local int i;
//	for(i = ReputationArray.Length - 1; i >= 0; --i)
//	{
//		if(ReputationArray[i].ForceID == ForceID){
//			return ReputationArray[i].ReputationData; 
//		}
//	}

//	return 0;

//	//local int i;
//	//for(i=0;i<16;i++)
//	//{
//	//	if(ReputationList[i].ForceID == ForceID){
//	//		return ReputationList[i].ReputationData;
//	//	}
//	//}
//	//return 0;
//}

//event         SetActiveState(int Index, int ForceID, bool ActiveState)
//{ 
//	local int i;
//	for(i = ReputationArray.Length - 1; i >= 0; --i)
//	{
//		if(ReputationArray[i].ForceID == ForceID){
//			ReputationArray[i].ActiveState = ActiveState; 
//			break;
//		}
//	}

//	//local int i;
//	//for(i=0;i<16;i++)
//	//{
//	//	if(ReputationList[i].ForceID == ForceID){
//	//		ReputationList[i].ActiveState = ActiveState;
//	//		return;
//	//	}
//	//}
//}

//event  bool    GetActiveState(int ForceID)
//{ 
//	local int i;
//	for(i = ReputationArray.Length - 1; i >= 0; --i)
//	{
//		if(ReputationArray[i].ForceID == ForceID){
//			return ReputationArray[i].ActiveState;
//		}
//	}
//	return false;

//	//local int i;
//	//for(i=0;i<16;i++)
//	//{
//	//	if(ReputationList[i].ForceID == ForceID){
//	//		return ReputationList[i].ActiveState;
//	//	}
//	//}
//	//return false;
//}

//---------------------------------------------------------------------------------

function ChangeBonusAttributes( BaseAttribute attri, bool add )
{
	if( add )
	{
		BonusAttributes.Strength     += attri.Strength;
		BonusAttributes.Intellect    += attri.Intellect;
		BonusAttributes.Agility      += attri.Agility;
		BonusAttributes.Stamina      += attri.Stamina;
		BonusAttributes.Spirit       += attri.Spirit;
		BonusAttributes.Impression   += attri.Impression;
	}
	else
	{
		BonusAttributes.Strength     -= attri.Strength;
		BonusAttributes.Intellect    -= attri.Intellect;
		BonusAttributes.Agility      -= attri.Agility;
		BonusAttributes.Stamina      -= attri.Stamina;
		BonusAttributes.Spirit       -= attri.Spirit;
		BonusAttributes.Impression   -= attri.Impression;
	}
}

function ChangeBonusAttackAttributes( AttackAttribute attri, bool add )
{
	if( add )
	{
		BonusAttackAttributes.CurrentMeleeAttack  += attri.CurrentMeleeAttack;
		BonusAttackAttributes.MeleeAttackMin      += attri.MeleeAttackMin;
		BonusAttackAttributes.MeleeAttackMax      += attri.MeleeAttackMax;
		BonusAttackAttributes.CurrentSpellAttack  += attri.CurrentSpellAttack;
		BonusAttackAttributes.SpellAttackMin      += attri.SpellAttackMin;
		BonusAttackAttributes.SpellAttackMax      += attri.SpellAttackMax;
		BonusAttackAttributes.HitRating           += attri.HitRating;
		BonusAttackAttributes.CriticalChance      += attri.CriticalChance;
		BonusAttackAttributes.AttackRatioRange    += attri.AttackRatioRange;
	}
	else
	{
		BonusAttackAttributes.CurrentMeleeAttack  -= attri.CurrentMeleeAttack;
		BonusAttackAttributes.MeleeAttackMin      -= attri.MeleeAttackMin;
		BonusAttackAttributes.MeleeAttackMax      -= attri.MeleeAttackMax;
		BonusAttackAttributes.CurrentSpellAttack  -= attri.CurrentSpellAttack;
		BonusAttackAttributes.SpellAttackMin      -= attri.SpellAttackMin;
		BonusAttackAttributes.SpellAttackMax      -= attri.SpellAttackMax;
		BonusAttackAttributes.HitRating           -= attri.HitRating;
		BonusAttackAttributes.CriticalChance      -= attri.CriticalChance;
		BonusAttackAttributes.AttackRatioRange    -= attri.AttackRatioRange;
	}
}

function ChangeBonusDefenseAttributes( DefenseAttribute attri, bool add )
{
	if( add )
	{
		BonusDefenseAttributes.Armor            += attri.Armor;
		BonusDefenseAttributes.SpellResistance  += attri.SpellResistance;
		BonusDefenseAttributes.Dodge            += attri.Dodge;
		BonusDefenseAttributes.Resilience       += attri.Resilience;
	}
	else
	{
		BonusDefenseAttributes.Armor            -= attri.Armor;
		BonusDefenseAttributes.SpellResistance  -= attri.SpellResistance;
		BonusDefenseAttributes.Dodge            -= attri.Dodge;
		BonusDefenseAttributes.Resilience       -= attri.Resilience;
	}
}

// 计算攻击属性
function AttackAttribute GetAttackAttribute()
{
	local AttackAttribute attack;

	attack.CurrentMeleeAttack = ( BaseAttributes.Strength + BonusAttributes.Strength ) * 2;
	attack.CurrentSpellAttack = ( BaseAttributes.Intellect + BonusAttributes.Intellect ) * 2;
	attack.CriticalChance = BaseAttributes.Impression + BonusAttributes.Impression;
	attack.HitRating = BaseAttributes.Impression + BonusAttributes.Impression;

	attack.CriticalChance += BaseAttackAttribute.CriticalChance + BonusAttackAttributes.CriticalChance;
	attack.HitRating += BaseAttackAttribute.HitRating + BonusAttackAttributes.HitRating;
	attack.CurrentMeleeAttack += RandRange(BaseAttackAttribute.MeleeAttackMin + BonusAttackAttributes.MeleeAttackMin,
		BaseAttackAttribute.MeleeAttackMax + BonusAttackAttributes.MeleeAttackMax);
	attack.CurrentSpellAttack += RandRange(BaseAttackAttribute.SpellAttackMin + BonusAttackAttributes.SpellAttackMin, 
		BaseAttackAttribute.SpellAttackMax + BonusAttackAttributes.SpellAttackMax);

	return attack;
}

// 计算防御属性
function DefenseAttribute GetDefenseAttribute()
{
	local DefenseAttribute defense;
	defense.Armor = ( BaseAttributes.Stamina + BonusAttributes.Stamina ) * 5;
	defense.SpellResistance = ( BaseAttributes.Stamina + BonusAttributes.Stamina ) * 4;
	
	defense.Dodge = BaseAttributes.Agility + BonusAttributes.Agility;
	defense.Resilience = BaseAttributes.Impression + BonusAttributes.Impression;

	defense.Armor += BaseDefenseAttribute.Armor + BonusDefenseAttributes.Armor;
	defense.Dodge += BaseDefenseAttribute.Dodge + BonusDefenseAttributes.Dodge;
	defense.Resilience += BaseDefenseAttribute.Resilience + BonusDefenseAttributes.Resilience;
	defense.SpellResistance += BaseDefenseAttribute.SpellResistance + BonusDefenseAttributes.SpellResistance;

	return defense;
}


// 自动回血回蓝
function RegenHPMP(float deltaTime)
{
	if(!EnableRegenHPMP)
	{
		return;
	}

	HP += BaseAttributes.Spirit * deltaTime;
	MP += BaseAttributes.Spirit * deltaTime;
}

defaultproperties
{
	HP = -1
	MaxHP = 1000
	
	MP = -1
	MaxMP = 1000

	HpPercent=0
	MountID=0
	WingID=0
	HeroID=0

	EnableRegenHPMP=true

	MoveTypeId=1
	Growth=0
	IsGreyName = false
	byCombatPoseOn = false
	PetLevel=0
	FrameEx1=""
	FrameEx2=""
	ColorEx1=""
	ColorEx2=""

	HairType=0
	HairColor=0
	FaceType=0
	SkinColor=0
}
