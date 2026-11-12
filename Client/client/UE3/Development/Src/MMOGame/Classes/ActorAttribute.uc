//
// character attribute management, combat attribute calculation
//
class ActorAttribute extends Object
	native
	editinlinenew
	dependson(IMMOActor)
	hidecategories(Object);


var array<INT> AttributeList;
//---------------------------------------------------------------
// combat attribute structs and calculation functions
//---------------------------------------------------------------

// level attributes
struct native LevelAttribute
{

	// level
	var() int Level;

	// max level
	var() int MaxLevel;

	// current experience
	var() int CurrentExp;

	// experience to level up
	var() int MaxExp;
};

// base attributes
struct native BaseAttribute
{
	// strength, affects external attack
	var() int Strength;

	// qi, affects internal attack
	var() int Intellect;

	// physique, affects HP, HP recovery, external defense
	var() int Stamina;

	// inner breath, affects character MP, MP recovery, internal defense
	var() int Spirit;

	// agility, affects character dodge, resilience
	var() int Agility;

	// willpower, affects character hit and critical
	var() int Impression;
};

// attack attributes
struct native AttackAttribute
{
	// external attack
	var int CurrentMeleeAttack;
	var() int MeleeAttackMin;
	var() int MeleeAttackMax;

	// internal attack
	var int CurrentSpellAttack;
	var() int SpellAttackMin;
	var() int SpellAttackMax;

	// hit rate, how well the character's attacks hit; both normal and ability attacks have hit checks
	var() float HitRating;

	// critical, chance of double damage in combat, unit: %
	var() float CriticalChance;

	// attack fluctuation range
	var() Vector2D AttackRatioRange;
};

// defense attributes
struct native DefenseAttribute
{
	// external defense, physical damage is reduced by armor
	var() int Armor;
	
	// internal defense, spell resistance, spell damage is reduced by resistance
	var() int SpellResistance;

	// dodge, character's ability to avoid attacks
	var() float Dodge;

	// resilience, reduces chance of being critically hit, unit: %
	var() float Resilience;

	// block parameters
	var() float BlockChance;
	var() float BlockReduce;
};

// extended attributes
struct native ExtendAttribute
{
	// damage bonus
	var() int DamageBonus;
};

// damage calculation result
struct native DamageResult
{
	// damage type
	var EDamageResultType Type;

	// HP increase or decrease
	var int HPChange;

	// MP increase or decrease
	var int MPChange;

	// critical hit damage amount
	var int CriticalHit;
	//main character is hurt
	var bool MainRole; 
	// damage reduced by block
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

var      int        MoveTypeId;//move mode, 0 normal, 1 chess piece move mode
var      int        MoveDirId; //move direction sub-parameter mode, 0 default, 1234 left/right/front/back
var      byte       PkMode;
var      int        PkValue;
var      byte       TitleID;
var      int        Growth;     //growth row
var      bool       IsGreyName; //whether grey name
var      byte       PetLevel;
var      string     FrameEx1;   //name special effect 1
var      string     FrameEx2;   //name special effect 2
var      string     ColorEx1;   //name special color 1
var      string     ColorEx2;   //name special color 2
//------------------------------------------------------------------------

var localized string ClassName[6];

// level attributes
var(MMO) LevelAttribute LevelAttributes;

// base attributes
var(MMO) BaseAttribute BaseAttributes;

// base damage
var(MMO) AttackAttribute BaseAttackAttribute;

// base defense
var(MMO) DefenseAttribute BaseDefenseAttribute;



// current HP
var(MMO) int HP;

// max HP
var(MMO) int MaxHP;

// current MP
var(MMO) int MP;

// max MP
var(MMO) int MaxMP;

// whether HP/MP can regenerate
var(MMO) bool EnableRegenHPMP;

// sect
var(MMO) int Faction;

var(MMO) EPlayerClass   ClassType;
var(MMO) EGenderType    GenderType;

var init array<int> ClientPlayerAttributes; //array synced from server

// equipment bonus attributes
var BaseAttribute       BonusAttributes;
var AttackAttribute     BonusAttackAttributes;
var DefenseAttribute    BonusDefenseAttributes;

//realm
var HeavenAttribute      heavenAttributes;
//mount
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

// calculate attack attributes
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

// calculate defense attributes
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


// auto HP/MP regeneration
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
