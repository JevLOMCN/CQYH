//
// general game logic data manager, one shared globally
// holds common game data such as sound, effects, animation, abilities; this class provides fast lookup and reading of the related resources, and does not actually execute game logic
//
class GamePlayDataManager extends Object
	hidecategories(Object)
 	dependson(PhysicalMaterialProperty)
 	dependson(Item)
	dependson(TypeDefine)
	dependson(AvatarData)
	dependson(Mount)
	dependson(UIKeyMapManager)
	native;

struct native MenuInfo
{
    // menu type
	var() EMenuType menuType;
    // identifier and description
	var() init string MenuName;
    // bound panel name
	var() string PanelName; 
    // tab page of this panel, leave empty if none
    var() string TabName;
    // icon frame name in flash
	var() string IconName;
    // key code (no need to fill in, filled automatically by the hotkey system)
	var() string KeyName;
	//list of components included
	var() array<string> ComponentList;
    // submenu list
	var() int RequireLevel;
	var() array<MenuInfo> ChildList;
};

struct native DoodadSound
{
    var() SoundCue AnimBegin;
    var() SoundCue AnimLoop;
    var() SoundCue AnimEnd;
	var() SoundCue HitSound;
	var() SoundCue DestructSound;
};

var (Menu) array<MenuInfo> MenuInfoList; 
var (Menu) array<MenuInfo> RightMenuInfoList; 
// UI Sound
var(Sound) array<SoundCue> UISoundList;

// Doodad Sound
var(Sound) DoodadSound DoodadSndList[EItemMaterialType];

struct native SoundParticleEffect
{
	var() SoundCue Sound;
	var() ParticleSystem    Particle;
};

struct native FootstepEffectNode
{
	var() EBodyType             ShapeType;
	var() EPhyMatType           LandMatType;
	var() SoundParticleEffect   EffectNode;
};

var(Sound) array<FootstepEffectNode> FootstepEffectList;



struct native HitSoundNode
{
	var() EWeaponHitType     HitType;
	var() EItemMaterialType     TargetArmorMatType;
	var() SoundCue   ToPlaySound;
};

var(Sound) array<HitSoundNode> HitEffectList;

struct native AnimationNode
{
	var() name animation;
    var() float Rate;
    var() bool bLoop;
    var() float LastTime;
    var() ERootBoneAxis RootBoneAxis;
    var() ERootMotionMode RootMotionMode;
    var() float LeftTime;
};

struct native AnimMixNode
{
	//source animation
	var() editconst EAnimMixSource   SourceAnim;
	//target animation
	var() editconst EAnimMixTarget    TargetAnim;
	//corresponding blend animation
	var() AnimationNode                     Anim;
};
//blend animation table
var(AnimMix) array< AnimMixNode >     AnimMixList;


struct native AvatarMap
{
	var() EPlayerClass  ClassType;
	var() EGenderType   GenderType;
	var() AvatarData     AvatarDataLink;
};
var() array<AvatarMap>  PlayerList;


struct native MountMap
{
	var() int            MountID;
	var() AvatarData     AvatarDataLink;
};
var() array<MountMap>      MountList;

struct native ClothNoise
{
	var() EGenderType       GenderType;
	var() EClothNoiseType   ClothType;
	var() ENoiseActionType  ActionType;
	var() SoundCue  SoundToPlay;
};

var(Sound) array<ClothNoise> ClothNoiseList;

enum EActionSound
{
	EActionSound_MousePickup,
	EActionSound_MousePutdown,
	EActionSound_Drop,
	EActionSound_Use,
	EActionSound_Get,
};

struct native ItemActionSound
{
	var() EItemActionSoundType ItemType;

	var(SoundFX) array<SoundCue> MousePickup;
	var(SoundFX) array<SoundCue> MousePutdown;
	var(SoundFX) array<SoundCue> Drop;
	var(SoundFX) array<SoundCue> Use;
	var(SoundFX) array<SoundCue> Get;
};

var (ItemSound) array<ItemActionSound> ItemActionSounds;


var(GhostSpawner) GhostSpawner GameGhostSpawner;

struct native WeaponUseSound
{
	var() EWeaponType WeaponType;
	var() EWeaponAppearenceType WeaponAppearenceType;
	var() array<SoundCue>  SoundToPlay;
};

// sounds used by common weapons
var() editinline array<WeaponUseSound> CommonWeaponSoundList;

var() editinline DamageProcessData DamageTemplateData;


struct native EquipEffectByWeaponType
{
	var() int Level;
	var() editinline array<AbilityComponent> Effects;
};

struct native WeaponLevelUpEffect
{
	var() EWeaponType WeaponType;
	var() editinline array<EquipEffectByWeaponType> WeaponEffects;
};

struct native EquipLevelUpEffect
{
	var() int Level;
	var() editinline array<AbilityComponent> TheOtherEquipLevelUpEffects;
};

var() array<WeaponLevelUpEffect> WeaponLevelUpEffects;
var() array<EquipLevelUpEffect> EquipLevelUpEffects;

var() editinline CameraShakeComponent CommonShootCameraAnim;

var() MaterialInstanceTimeVaryingEffect MaterialTimeVaryingEffectData;

struct native NpcLevelColorData
{
	var() int Quarity;
	var() name ParameterName;
	var() Vector ColorValue;
};

var() array<NpcLevelColorData> NpcLevelColorList;

struct native WeaponSpeed
{
	var() EWeaponType WeaponType;
	var() float SpeedFactor;
	structdefaultproperties
	{
		SpeedFactor=1.0f
	}
};

var() array<WeaponSpeed> WeaponTypeSpeeds;

// anim control data
var() float WoundAnimCanBeResetThreshold <ClampMin = 0.0 | ClampMax = 1.0>;

native function float GetWeaponSpeedFactor(EWeaponType InWeaponType);

native function  PlayItemActionSounds(Actor Actor,EItemActionSoundType Type,EActionSound eas);
native final function PlayDoodadBeginSound(Actor actor, EItemMaterialType type);
native final function PlayDoodadLoopSound(Actor actor, EItemMaterialType type);
native final function PlayDoodadEndSound(Actor actor, EItemMaterialType type);
native final function PlayDoodadHitSound(Actor actor, EItemMaterialType type);
native final function PlayDoodadDestructSound(Actor actor, EItemMaterialType type);

native function SoundCue GetWeaponUseSound(EWeaponAppearenceType InWeaponType, optional int Index = 0);

// Init / UnInit
native final function InitGamePlayData();
native final function UnInitGamePlayData();

// if find return true
native final function SoundCue FindHitSound(EWeaponHitType InHitType, EItemMaterialType TargetArmorMatType);
native final function bool FindFootstepEffect(EBodyType ShapeType, EPhyMatType LandMatType, out SoundCue Sound, out ParticleSystem Particle);

native final function  name FindAnimMix(name SourceAnim,name TargetAnim );

static native function GamePlayDataManager Singleton();
native final function SavePackage();

final native function DamageEffectPair GetDamageEffect(EDamageResultType ResultType, EWeaponHitType WeaponHitType, EHitDirectionType DirectionType, EElementaryType ElementaryType);

final native function bool GetStateDeath(EAURA_EFFECT_TYPE InType, out array<AbilityComponent> OutData);

final native function bool GetCommonDeath(DeathStyle InStyle, EElementaryType InElementaryType, out array<AbilityComponent> OutData);

// for editor
final native function int GetDamageEffectListIndex(EDamageResultType ResultType, EWeaponHitType WeaponHitType, EHitDirectionType DirectionType, EElementaryType ElementaryType);

native final function ItemActionSound GetItemActionSounds(EITEM_TYPE type);

native final function MaterialInstanceTimeVaryingEffectNode GetMaterialEffectData(int InEffectId);

native final function bool GetNpcLevelColorData(int InQuality, out NpcLevelColorData OutData);

native final function SoundCue GetClothNoise(EGenderType GenderType, EClothNoiseType ClothType, ENoiseActionType ActionType);

cpptext
{
	static UGamePlayDataManager* GetSingleton();
	// void Draw(FViewport* Viewport, FCanvas* Canvas);
	static UPackage* GetGameplayDataMgrPackage();
}

DefaultProperties
{
	WoundAnimCanBeResetThreshold=0.5
}
