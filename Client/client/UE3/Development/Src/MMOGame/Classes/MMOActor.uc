
//
// 主要的抽象角色类，用于游戏中所有角色对象的基类
//
class MMOActor extends Pawn implements (IAbilityDamageable, IPickableActor, IMMOActor)
	config(MMOActor)
	dependson(PhysicalMaterialProperty)
	dependson(MMOAbility)
	dependson(TypeDefine)
	dependson(ActorAttribute)
	dependson(AvatarData)
	dependson(MMOActorAT)
	dependson(Buff)
	dependson(DestructablePartAttachInfo)
	native;

 // 每隔1秒尝试修正贴地
var float PlaceToGoundTickTime;

// 超过多少次后就不再修正
var int PlaceToGoundTickCount;

var protected Vector ServerLocation;
var protected Vector LastServerLocation;

var int CurBlockRadius;
var int CurBlockType;

var EPickPriority PickPriority;
var(AvatarData) AvatarData UseingAvatar;    //Add by jimzhang 2012.7.11
var(MoveController)MoveController TempController;

var float RotationSpeed;
var bool bInstantRotate;
var float CurrentMoveSpeed;
var int PetId;

var          bool      ShowHeadName;
var          GFxObject HeadName;
var          GFxObject TalkBubble;
var          GFxObject MinimapObj;
var          GFxObject WorldmapObj;
var          float     ResetMovePhaseTime;
var          float     ResetLastLocationTime;
var          float     AttackAi_LockTime;

// 游戏通用数据处理器的引用,直接使用,避免每次强转
var transient GamePlayDataManager GpDataManager;

// var bool IsDead;
var name PlayingAnim;

// only for sound effect
var(SoundFX) EBodyType BodyType;

// 当没有换装数据,但需要音效材质数据时,这是一个缺省的属性
var(SoundFX) EItemMaterialType SoundMaterialType;

// nActionOrLoop,记录动画层状态
var int nActorActionOrLoop;


var bool bSendRequestToServerToGetFullInfo;


// 是否显示地形与Navment高度差开关
var(Debug) bool ShowNavmeshDistance;

// 近战范围
var(Debug) float MeleeAttackRange;

// 远程攻击范围
var(Debug) float RangeAttackRange;

var(MMO_UI) int   NPCIcon;
var(MMO_UI) Texture2D HUDDisplayIcon;

var transient EActorClassType ActorType;

var(MMO_UI) string ActorName;

var Rotator TargetRotation;
var float CurrentAttackRange;
var EOBJ_BODY_TYPE  BodySize;

// 统一用g_moveModGridSize  const GRID_SIZE=64;
// 是否使用虚幻自己的移动函数
var(Movement) bool bUsingUnrealWalking;

// 阵营标识, 两阵营标识做 AND 操作，不为0是友好
var(MMO) int FriendlyMask;

// 角色属性
var(MMO) editinline ActorAttribute ActorAttributes;

var   ActorAttribute HeroAttributes;     

//队伍号，其实这根本不应该放这里，都怪方如意写的mmoactor结构
var int TeamID;
//个人竞技积分，放在人身上，这个属性虽说是竞技场积分，但是和人属性一起同步
var int PersonalArenaScore;

// 模型相关属性
// 动画
var(Anim) MMOAnimGroup PawnAnimInfo;
var transient int CurrentModelTypeIdx;


// 攻击状态持续时间，每次使用技能时置为最大值MaxAttackStateTime，倒计时为0后重置为非攻击状态
var float AttackStateDurationTime;
var bool IsAttackState;

var transient bool bWaitForAbilityReply;
// 第一人称有效
var transient EAbilityState CurrentAbilityState;
var transient int   CurrentRequestAbilityId;
var transient int   CurrentRequestAbilitySerialId;
var transient Spell_ControlTagType  SpellControlTag;
var transient Spell_ControlTagType  CastControlTag;
var transient bool CanMoveInSpellTime;
var transient bool CanMoveInCastTime;
var transient float AbilityStateCD;

// state
var transient bool bFreeze;
var transient bool bStun;

//var transient bool bVisible;

var transient bool bVisibleByAbility;
var transient bool bVisibleByMergeMesh;
var transient bool bVisibleByKismet;
var transient bool bVisibleByHotKey;
var transient bool bVisibleByCharacterCountLimit;

var transient bool bVisibleWeaponByAbility;

var transient bool bShowTime;   // 客户端表演状态,不一定与服务器坐标保持一致,但在结束时要与服务器坐标一致

var transient bool bDead;

var bool bFightingState;

var bool bResourceInitialized;
var bool bActorDataReset;
var float fActorDataResetDelay;
/////////////// Passive move
var transient EPassiveMoveType PassiveMoveType;
var transient Vector PassiveMoveDestination;
var transient float PassiveMoveLastTime;

// rotate
var transient bool bRotationAnimation;
var transient float RotationLeftTime;
var transient int RotationAnimKey;

// scale
var bool bScaling;
var int ScaleEffectId;
var transient Vector ScaleTarget;
var transient float ScaleLeftTime;
var transient float ScaleTime;


// dash
var transient Vector DashStartPoint;

// THROW
var transient float PassiveMoveCurrentTime;
var transient int PassiveMoveStep;
var transient Matrix ThrowStartMatrix;
var transient Vector P0;
var transient Vector P1;
var transient Vector T0;
var transient Vector T1;
var transient float ThrowAnimRate;
var transient float ThrowExp;


// movement
var transient bool bMovingToMousePoint;
var transient float LastUpdateTime;
// rotate
var transient int RotateToMousePointRefrence;
var transient float RotateSpeed;
var transient int TurnActionId;

// collision rate
var transient float CollisionHeightRate;
var transient float CollisionRadiusRate;

// element type
var transient EElementaryType ElementaryType;
var transient int   ElementaryCreatorId;			 // default 0

var transient MMOPlayerController MainController;

struct native EquipNode
{
	var EBodyMeshType BodyMeshType;
	var Object ComponentOrActor;
	var bool bVisible;
};

var init array<EquipNode> EquipList;

// 头顶上的特效
var Emitter SFX_OverHead;

// 显示血量数字标识
var int ShowHpNumber;

var int GridX;
var int GridY;
var bool bIsActorInSafeZone;
//---------------------------------------------------------------
// 静态绑定部件
//---------------------------------------------------------------

// 当前绑定的StaticMesh信息
struct native StaticMeshAttachInfo
{
	var() Name AttachName;
	var() StaticMesh MeshAsset;
	var() Name AttachSocket <DynamicList="SocketList">;

	var StaticMeshComponent MeshComponent;
};

var init array<StaticMeshAttachInfo> AttachedStaticMeshs;

// 当前绑定的可破坏部件，add by jimzhang 2012.7.11
var init array<DestructablePartAttachInfo> AttachedDestructableParts;

var Mount MountInfo;
var Mount WingInfo;
var EWeaponType AttitudeType;
var EWeaponType AttitudeTypeLast;

// 当前角色身上装备列表
var const int C_EquipCount;

var transient bool						bHoldWeapon;            // 如果有装备武器的话,true表示手持武器状态,false表示背着武器状态

var transient ItemManager				ItemManager;



//var transient EEStallState StallState;
var transient EActorMoveType MoveType;       // 当前移动方式
var transient float MoveSpeedFactor;    // 移动速度变化值(buff管理)    , deafault is 1.0f


/** The pawn's light environment */
var(MMO) const editconst DynamicLightEnvironmentComponent LightEnvironment;

//---------------------------------------------------------------
// 粒子系统绑定部件
//---------------------------------------------------------------

// 当前绑定的粒子系统
var init array<ParticleSystemAttachInfo> AttachedParSys;

// 初始时的绑定粒子系统列表
var(MMOAttach) editinline array<ParticleSystemAttachInfo> InitAttachParSys;

//var transient AutoReleaseRadialBlurActor BlurActor;


/** Max distance from listener to play footstep sounds */
var float MaxFootstepDistSq;

var bool    hasBladeOutAnim;    // whether the actor has blade in/out animation
var bool    isDivideBody;       // whether the actor is divided to half bodies

var bool    isHoldingStone;

/// skeletal controllers
var MMOActor            lookAtActor;
var SkelControlLookAt   lookAtController;

var transient MMOActorAT    ActorAT;   

var transient float ActionPlayedLeftTime;
//当前正在播放的动画
var transient name PlayingAinmName;

// 特用材质模板
var MaterialInstanceConstant ConstantMat;
var MaterialInstanceTimeVarying TimeMat;


var transient   array<MMOAbility>   Abilities;
var transient   array<MMOAbility>   NormalAttackAbilities;

// 方便于管理技能有生命期的组件
var transient   array<ParticleSystemComponent> MeshParticleAttachments;
var transient   array<AudioComponent>   ActorAudioComponent;

var transient   array<int> AbilityShowEffectAreaIds;

struct native BuffNode
{
	var int Id;
	var float CurrentTime;
	var float MaxTime;
	var int StackCount;
	var Buff BuffData;
	var init array<MMOEmitter> BuffProjectile;
};

var transient array<BuffNode> BuffList;

var transient bool bCanRecvCmd;

var init array<MMOActor>   onTouchActor;
var init array<MMOActor>   onTouchActor2;
var int   onTouchActorTolNum;
var int   onTouchActorCurNum;

var vector LastLocation;

//---------------------------------------------------------------
// Plane Shadow Index
//---------------------------------------------------------------
var bool	  UsePlaneShadow;
var transient INT	planeShadowIndex;
//---------------------------------------------------------------
// 网络协议相关变量
//---------------------------------------------------------------

// 对象在服务器端的ID
var int ServerID;
var int ServerHeroID;
var bool HeroSummon; //出征标志
var int  HeroAi;//英雄跟随方式

// 对象的角色名（昵称）
//统一用ActorNamevar init string RoleName;
var init string MasterName;
var init string HeroName;
var int dwHostID;

// 对象的家族名
var init string GuildName;
var int	MasterState;

var float AnimBlendInTime;
var float AnimBlendOutTime;

//记录上次改变的材质
var init string LastMaterialName;

var transient ESceneObjectAppearType SceneObjectAppearType;

var transient bool bMainSkelChanged;

var transient array<AbilityComponent> SpecialDyingActions;

// move/idle anim
var int CurrentMoveControlComponentId;
var name BackupMoveAnim;
var name BackupIdleAnim;
var float MoveAnimRate;


// mesh change effect
var int MeshChangeEffectId; // default is zero
var AvatarData MeshChangeAvatarData;
var MMOActorAT MeshChangeAnimController;

// Position Transform effect
var int PositionTransformEffectId;
var PositionTransformComponent PositionTransformData;
var float PositionTransformCurrentTime;
var Matrix PositionTransformBaseTransform;


var transient array<int> WeaponLevelUpEffectIds;
var transient array<MMOEmitter> CanNotBeGCEmitters;

struct native MirrorActor
{
	var float LifeSpan;
	var AbilitySkeletalMeshActor MeshActor;
};

var transient array<MirrorActor> ActiveMirrorActors;

// 比奇争霸战
// 进/出Volume计数
var int inSabacZoneCount;

// 材质需要刷新
var bool NeedRefreshMaterial;

native function string GetActorName();
native function InitPlaneShow();
native function UpdatePlaneShow();
native function UpdatePlaneShadowPosition();
native function ReleasePlanShow();
event SetUsePlaneShadow( bool bUse ) { UsePlaneShadow = bUse; }
event CheckPlaneShadow()
{
	if ( !UsePlaneShadow && planeShadowIndex != -1 ) 
	{
		ReleasePlanShow();
	}
}

native function SetEnvironmentComponentEnable();

native function vector GetShadowLocation();

native final function AttachEquip( EBodyMeshType InType, Object InObj, optional bool bVisible = TRUE );
native final function DetachEquip( EBodyMeshType InType );
native final function DetachAllEquip();
native final function SetEquipVisibleFlag( EBodyMeshType InType, bool bVisible );
native final function bool GetEquipVisibleFlag( Object InObj );
native final function Object GetEquip(EBodyMeshType InType);

 native final function bool HoldWeapon(bool bHold);

native function int SpawnMirror(AbilityMirrorComponent InData, float InLifeSpan);
native function ClearMirror(int InObjectId);
native function ClearAllMirrors();
native function UpdateMirrorActors(float Delta);

final native function SelfDestroyed(int cid);

native function AvatarData GetAvatarData();
native function float GetGridRadius();

// 材质效果相关处理函数
native function RebuildMaterialControllerList();
native function SetToNewMaterialEffect(EMaterialEffectType InType, int InId, bool bOn, optional MaterialInstanceTimeVaryingEffectNode InEffectData, optional MaterialInstance InNewMaterial);
native function SetInitColor(name InColorName, Vector InColor);
native function SetOpacity(name InOpacityName, float OpacityValue);

native function EActorClassType GetActorType() const;
native final function bool IsMainPlayer() const;
native function SetActorType( EActorClassType newType );

// 取得脚底坐标
native final function vector GetFootLocation();

// 取得全身高
native final function float GetFullBodyHeight();

native final function EnableRagdoll(bool bOn);

native final function float CalculateRootBoneMoveSpeed(name InAnimSeq);

native final function ReplaceMovementAnim(bool bOn, int ProcessId, name InMoveAnim, name InIdleAnim);

native final function RotateTo(Rotator NewRotation, float DeltaTime);

native final function ScaleTo(bool bOn, int InEffectKey, Vector InValue, float InLastTime);

native final function UpdateLocationZ();

native function int GetSex();
native function int GetHeroSex();
//{
//	return ActorAttributes.GenderType;
//}

native function int GetClassId();
native function int GetHeroClassId();
//{
//	return ActorAttributes.ClassType;
//}

native function SetObstracel(name fName , bool bEnable);

native function float GetValidZValue(Vector InLocation, optional float MaxHeight = 600, optional float MinHeight = -600);

event bool GetValidPoint(Vector In, out Vector outp)
{
	local Vector hitl, hitn,vs,ve;
	local Actor traced;
	vs = in;
	vs.Z += 300;
	ve = in;
	ve.Z -= 300;
	traced = Trace(hitl, hitn, ve,vs, ,,,TRACEFLAG_Blocking);
	if(traced != none)
	{
		outp = hitl;
		return true;
	}
	else
	{
		return false;
	}
}

event bool IsShowAbilityLine()
{
	return MMOGame(WorldInfo.Game).DrawAbilityDebugLine;
}

native function float GetWoundAnimThresholdValue();

native function SetActorAttributeValue( int AttrID, int value );
native function int GetActorAttributeValue( int AttrID);

native function float GetMp();

native function SetHeroAttributeValue( int AttrID, int value );
native function int GetHeroAttributeValue( int AttrID);

native function bool CanUseAnyAbility();
native function bool CanMove();
native function bool CanMoveInAdvance();

native function EquipSpell( int hotKeySlotId, MMOAbility newAbility);
native function MMOAbility GetEquipedAbilityById( INT mainAbilityID );
native function MMOAbility GetMainAbilityById( int mainAbilityID );
native function MMOAbility GetAbilityById( INT mainAbilityID, INT abilityRuneID );
native final function UpdateAbility( INT mainAbilityID, INT abilityRuneID, INT level );

native function int PlayAction(AbilityComponent InComponent);
native function array<int> PlayActions(array<AbilityComponent> InComponents);

native function PlayActionById(int ActionId);

native function PlayBloodEffect(EDamageResultType InResultType, bool bCauseDead);

// if have anything to play, return true
native function bool PlaySpecialDying(float delay=0.0f);
native function bool PlayStateDying(float delay=0.0f);
native function bool PlayCommonDying(DeathStyle InStyle, EElementaryType InElementaryType, float delay=0.0f);
native function ProcessActionsDelay(out array<AbilityComponent> OutComponents, float Delay);

//native function ShowEquipLevelUpEffect(int InNewLevel, bool bWeapon);

// add by zhangrui05 for fix bug #22643
// 这个是用来将角色身上Buff效果影响的所有材质还原,以便重新施加Buff的效果
native function RebuildAllMeshMaterialList();

native function NotifyRefreshEquipLevel(ENORMAL_EQUIP_SLOT InSlot);
native function NotifyRefreshEquipLevelByIndex(int InIndex);
native function ClearEquipEffects();

native function SetServerLocation(Vector NewLoc);	// zhaojun@snda
native function ResetLastServerLocation(optional float ResetTime=-1.0f);
native function SetAutoAttackLockTime();

function bool CanUseAbility(int id, int level, Actor selected, Vector pos)
{
	return true;
}

native function bool IsUsingAbility();

function bool CanInteruptAbility()
{
	return false;
}

function bool NotifyAbilityEnd(int id)
{

}

native final function MMOPlayerController GetMainController() const;
native function SetMainController(MMOPlayerController ctrl);

native function TouchNative(Actor Other, PrimitiveComponent OtherComp,Vector HitLocation, Vector HitNormal);
native function UnTouchNative(Actor Other);

native function PlayWeaponUseSound(int SoundIdx);
native function PlayWeaponHitSound(EWeaponHitType HitType, EItemMaterialType MaterialType);

final native function HideWeapon(bool bHide);
final native function SetBodyPartVisible(EBodyMeshType InType, bool bVisible);

native final function PlayClothNoise(ENoiseActionType InActionType);

// mesh change effect
native final function StartMeshChangeEffect(int InEffectId, AvatarData InNewAvatar);
native final function StopMeshChangeEffect(int InEffectId);
native final function bool IsInMeshChangeState();

native final function bool IsBaseMeshHidden();

// 
native final function StartPositionTransformEffect(int InEffectId, PositionTransformComponent InData);
native final function StopPositionTransformEffect(int InEffectId);
native final function UpdatePositionTransformEffect(float Delta);

event Touch( Actor Other, PrimitiveComponent OtherComp, 
	Vector HitLocation, Vector HitNormal )
{
	//TouchNative( Other,  OtherComp, HitLocation,  HitNormal);

}

event UnTouch( Actor Other )
{
	//UnTouchNative( Other);

}

event ScriptMoveTo(Vector Des, float Speed,optional float inMoveRateScale =1.0)
{
	self.MoveTo(Des, speed, false,inMoveRateScale);
	
	UpdateAttachAnim(-1);
}


final native function AddAbility(MMOAbility newAbility);
final native function DelAbility(int mainAbilityID,int runeSlotID);
final native function ResetAbilities();

function MMOAbility GetNormalAttackAbility()
{
	if(NormalAttackAbilities.Length == 0)
	{
		return none;
	}
	else
	{
		return NormalAttackAbilities[Rand(NormalAttackAbilities.Length)];
	}
}

event PostInitAbility()
{
	//UI通知全部在外围做
	//这个暂时留着 可留作它用 
	/*if(IsMainPlayer())
	{
		MainController.UpdateSkillPanel();
	}*/
}


native function TickAllAbility(float DeltaTime);


event StartRecvCmd()
{
	bCanRecvCmd = true;
}

function bool CanRecvCmd()
{
	return bCanRecvCmd;
}

function bool IsTargetLocationOk(Vector Loc)
{
	return true;
}
function bool IsTargetActorOk(Actor T)
{
	return true;
}

native function StopMovement();


event HitWall( vector HitNormal, actor Wall, PrimitiveComponent WallComp )
{
	super.HitWall(HitNormal, Wall, WallComp);
	//`log("HitWall!!!!!!!!!!!");
}


event LevelUp(int NewLevel, int NewExp)
{
	`log(ServerId$"  LevelUp NewLevel="$NewLevel$"  NewExp="$NewExp);
}



function NotifyAbilityFinished(int id)
{
	bCanRecvCmd = false;
}


event SetToNewMaterialByName(string Type,string tName )
{
	//local array<AnimSet> AnimSets;
	local MaterialInstanceConstant newMaterialConst;
	local MaterialInstanceTimeVarying newMaterialTime;
	local int i;

	/*if( LastMaterialName == tName )
	{
		`log("SetToNewMaterialByName::Nothing Changed" );
		return;
	}*/
	if( Type=="MaterialInstanceConstant" )
	{
		newMaterialConst = MaterialInstanceConstant( DynamicLoadObject( tName , class'MaterialInstanceConstant' ) );
		//Buff无广播 这里这能暂且如此了
		if( newMaterialConst == none )
		{
			`log("SetToNewMaterialByName::Can't find Material"@newMaterialConst );
			return;
		}
		`log("SetToNewMaterialByName::newMaterial"@tName@newMaterialConst );
		//SetToNewMaterial( newMaterialConst ); 
		//SetToNewMaterial( ConstantMat ); 

		for(i = 0; i < Mesh.GetNumElements(); ++i)
		{			
			Mesh.SetMaterial(i, ConstantMat);
		}
		
		LastMaterialName = tName;
	}
	else if( Type=="MaterialInstanceTimeVarying" )
	{
		newMaterialTime = MaterialInstanceTimeVarying( DynamicLoadObject( tName , class'MaterialInstanceTimeVarying' ) );
		//Buff无广播 这里这能暂且如此了
		if( newMaterialTime == none )
		{
			`log("SetToNewMaterialByName::Can't find Material"@newMaterialTime );
			return;
		}
		`log("SetToNewMaterialByName::newMaterial"@tName@newMaterialTime );
		//SetToNewMaterial( newMaterialTime ); 
		//SetToNewMaterial( TimeMat ); 
		for(i = 0; i < Mesh.GetNumElements(); ++i)
		{			
			Mesh.SetMaterial(i, TimeMat);
		}
		//SetToNewMaterial( ConstantMat ); 
		LastMaterialName = tName;
	}
	
}
	
event SetToNewMaterial(MaterialInstance InNewMaterial)
{
	//if(ItemManager != none)
	//{
	//	`log("SetToNewMaterial::"@InNewMaterial );
	//	ItemManager.SetToNewMaterial(InNewMaterial);
	//}
}

function PlaySpawnAnim()
{
	if(PawnAnimInfo.SpawnAnim != '')
	{
		PlayAnimation(PawnAnimInfo.SpawnAnim);
	}
}



//-------------------------新技能代码 ------------------------>


// 取得攻击防御属属性
function ActorAttribute GetActorAttribute()
{
	return ActorAttributes;
}

// 计算血量的百分比
native function float GetHealthPercent();
//{
//	if( ActorAttributes.GetMaxHp() == 0 )
//		return 0.001f;//如果对flash的宽度设0有点问题

//	return float(ActorAttributes.GetCurHp()) / float(ActorAttributes.GetMaxHp());
//}

// 计算蓝的百分比
function float GetManaPercent()
{
	if( ActorAttributes.GetMaxMp() == 0 )
		return 0.001f;

	return float(ActorAttributes.GetCurMp()) / float(ActorAttributes.GetMaxMp());
}

event InitAnimNodes()
{
	//SetDrawScale(3.0f);

	LookAtController = SkelControlLookAt(Mesh.FindSkelControl('HeadLookAt'));


	if(lookAtController == none)
	{
		//`warn("[MMOAnim]: HeadLookAt node not found in anim tree.");
	}
	else
	{
		lookAtController.ControlStrength = 0.f;
	}

	if(ActorAT ==none ){
		ActorAT = new(self) class'MMOActorAT';
	}
	ActorAT.InitAnim(Mesh,self);

	//IdleLoopBlend = AnimNodeBlendTwoSequences(Mesh.FindAnimNode('IdleLoopBlend'));
	//MoveLoopBlend = AnimNodeBlendTwoSequences(Mesh.FindAnimNode('MoveLoopBlend'));
	//ActionBlend = AnimNodeBlendTwoSequences(Mesh.FindAnimNode('ActionBlend'));

	//MoveTypeBlend       = AnimNodeBlendTwoSequences(Mesh.FindAnimNode('MoveTypeBlend'));
	//MoveLoopBlendEx = AnimNodeBlendTwoSequences(Mesh.FindAnimNode('MoveLoopBlendEx'));

	//ActionAnimRateNode = AnimNodeScalePlayRate(Mesh.FindAnimNode('ActionAnimRateNode'));
	//LoopAnimRateNode = AnimNodeScalePlayRate(Mesh.FindAnimNode('LoopAnimRateNode'));
	//ActionLoopBlend = AnimNodeBlendPerBone(Mesh.FindAnimNode('ActionLoopBlend'));
	//IdleLoop1 = AnimNodeSequence(Mesh.FindAnimNode('IdleLoop1'));
	//IdleLoop2 = AnimNodeSequence(Mesh.FindAnimNode('IdleLoop2'));
	//MoveLoop1 = AnimNodeSequence(Mesh.FindAnimNode('MoveLoop1'));
	//MoveLoop2 = AnimNodeSequence(Mesh.FindAnimNode('MoveLoop2'));
	//Action1 = AnimNodeSequence(Mesh.FindAnimNode('Action1'));
	//Action2 = AnimNodeSequence(Mesh.FindAnimNode('Action2'));

	//MoveLoop1Ex  = AnimNodeSequence(Mesh.FindAnimNode('MoveLoop1Ex'));
	//MoveLoop2Ex  = AnimNodeSequence(Mesh.FindAnimNode('MoveLoop2Ex'));


	// todo fix
	RefreshAnim();
}

native function RefreshMoveAnim();
native function SetMoveAnimRate(float InRate);
native function RefreshAnim();
native function RefreshAttachAnim();
native function RefreshCollision();




// 更新碰撞信息, add by jimzhang 2012.8.14
//event RefreshCollision()
//{
//	if(UseingAvatar!=none && UseingAvatar.CollisionHeight>0.0)// && UseingAvatar.CollisionComponent!=none)
//	{
//		BodyType				= UseingAvatar.BodyType;

//		bCanStepUpOn			= UseingAvatar.bCanStepUpOn;
//		CollisionType			= ECollisionType(UseingAvatar.CollisionType);
//		BlockRigidBody			= UseingAvatar.BlockRigidBody;
//		bCollideComplex			= UseingAvatar.bCollideComplex;	
//		bNoEncroachCheck		= UseingAvatar.bNoEncroachCheck;
//		bPhysRigidBodyOutOfWorldCheck	= UseingAvatar.bPhysRigidBodyOutOfWorldCheck;
//		bPathColliding			= UseingAvatar.bPathColliding;
		
//		SetCollisionSize(UseingAvatar.CollisionRadius*UseingAvatar.AvatarDrawScale,UseingAvatar.CollisionHeight*UseingAvatar.AvatarDrawScale);
//		CylinderComponent       = UseingAvatar.CollisionComponent;
//		CollisionComponent      = UseingAvatar.CollisionComponent;

//		SetCollisionFromComponent(CollisionType,UseingAvatar.CollisionComponent);
		
//	}		
//}

event RefreshCollisionForce( float scale )
{
	if( UseingAvatar!=none && UseingAvatar.CollisionHeight>0.0 )// && UseingAvatar.CollisionComponent!=none)
	{
		SetCollisionSize(UseingAvatar.CollisionRadius * scale,UseingAvatar.CollisionHeight * scale );
	}		
}


//function RefreshCollisonRate()
//{
//	if(CylinderComponent != none)
//	{
//		CollisionHeightRate = CylinderComponent.CollisionHeight / MMOGame(WorldInfo.Game).StandardCollisionHeight;
//		CollisionRadiusRate = CylinderComponent.CollisionRadius / MMOGame(WorldInfo.Game).StandardCollisionRadius;
//	}
//}

native function RefreshCollisonRate();
native function NativeSetMesh(EPlayerClass ClassType, EGenderType GenderType);

event SetMesh(EPlayerClass ClassType, EGenderType GenderType)
{
	NativeSetMesh( ClassType, GenderType);
}

// 当切换新武器时,需要切换一下动作集，changed by jimzhang 2012.7.11
native function RefreshAnimSet();

native function RefreshMovementSpeed();

//一些特殊情况下调用 
event ChangeAnimSet(string AnimSetName )
{
	//local array<AnimSet> AnimSets;
	local AnimSet Anim;
	`log("ChangeAnimSet::ChangeName"@AnimSetName );
	Anim = AnimSet( DynamicLoadObject( AnimSetName , class'AnimSet' ) );
	//Buff无广播 这里这能暂且如此了
	if( Mesh.AnimSets[0] == Anim )
	{
		`log("ChangeAnimSet::Nothing Changed" );
		return;
	}
	`log("ChangeAnimSet::New Anim"@Anim );
	if( Anim != none )
	{
		//Add vs AddItem 
		/*AnimSets.AddItem( Anim );
		SetAnimSet( AnimSets );*/
		Mesh.AnimSets.Remove(0, Mesh.AnimSets.Length);
		Mesh.AnimSets.AddItem( Anim );	
		Mesh.UpdateAnimations();
		RefreshAnim();
	}
}

// 强行设置动作集，用于boss的特殊动作，changed by jimzhang 2012.7.11
event SetAnimSet(array<AnimSet> ani)
{
	
	local int  j;
	`log("________ 强行设置动作集" );
	if(Mesh != none )
	{
		if(ani.Length == 0)//如果等于0,就恢复武器对应动画集
		{
			RefreshAnimSet();
		}
		else{
			Mesh.AnimSets.Remove(0, Mesh.AnimSets.Length);
			for(j = 0; j < ani.Length; ++j)
			{	
				Mesh.AnimSets.AddItem(ani[j]);
			}		
			Mesh.UpdateAnimations();
			RefreshAnim();
		}
	}	
}

native function EWeaponType GetWeaponType();
native function EWeaponAppearenceType GetWeaponAppearenceType();
native function EItemMaterialType GetBodyMaterialType();
native function SetIdleAnim(name NewIdleLoopAnim);
native function SetMoveAnim(name NewMoveLoopAnim);
native function SwitchFightIdle(bool bFight);
native final function name GetIdleAnim();
native final function name GetMoveAnim();
native final function bool GetCurrentAnimName(out name OutAnim);

// Inpos : [0, 1.0]
native final function SetMoveAnimPosition(float InPos);
native final function float GetMoveAnimPosition();

native function EClothNoiseType GetBodyClothNoiseType();

native function int GetNewAnimKey();
native function int GetCurrentAnimKey();

native function SkeletalMeshComponent GetMainMesh();


event bool HaveAnim(name Anim)
{
	local AnimSequence AnimSeq;
	if(Mesh == none)
	{
		return false;
	}
	else
	{
		AnimSeq = Mesh.FindAnimSequence(Anim);
		if(AnimSeq == none)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

native function Equip GetWeaponEquip();
//native function EquipMesh GetWeaponEquipMesh(int InIndex);
native function bool GetWeaponSkeletalMeshComponents(out SkeletalMeshComponent Mesh1, out SkeletalMeshComponent Mesh2);
native function bool GetWeaponMeshComponents(out MeshComponent Mesh1, out MeshComponent Mesh2);
native function MMOActorAT GetWeaponEquipMeshAnimationController(int InIndex);

native function SetAnimRateById(int InId, float InRate);

function PlayStandUpAnimFromBeatFlyAnim()
{
	if(!bDead)
	{
		PlayAnimation(PawnAnimInfo.BeatFlyAwayAnims[1], 0, ThrowAnimRate, , , , , , , , ,  EAL_State);
	}
}

native function float PlayAnimation(
	name Anim, 
	optional int InKey = 0, 
	optional float Rate = 1.0f, 
	optional bool bLoop = false, 
	optional ERootBoneAxis RootBoneAxisX = RBA_Discard,
	optional ERootBoneAxis RootBoneAxisY = RBA_Discard,
	optional ERootBoneAxis RootBoneAxisZ = RBA_Discard, 
	optional EAnimBlendType BlendType = EABT_FullBody, 
	optional float Position = 0.f, 
	optional float InBlendIn=0.1, 
	optional float InBlendOut=0.1,
	optional EAnimLevel InLevel = EAL_Action
	);

// 不太准确的查询
event bool IsPlayingAnimationByName(name InAnim)
{
	if(IsInMeshChangeState())
	{
		if(MeshChangeAnimController != none)
		{
			return MeshChangeAnimController.IsPlayingAnimationByName(InAnim);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return ActorAT.IsPlayingAnimationByName(InAnim);
	}
}

// 准确的查询
event bool IsPlayingAnimationByKey(int InKey)
{
	if(IsInMeshChangeState())
	{
		if(MeshChangeAnimController != none)
		{
			return MeshChangeAnimController.IsPlayingAnimationByKey(InKey);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return ActorAT.IsPlayingAnimationByKey(InKey);
	}
}

native function bool IsPlayingAnyAnimation();

native function StopAnimation();

native function float GetAnimSeqLength(name InName);


event StopAnimationByName(name InAnim)
{
	local DestructablePartAttachInfo info;
	if(IsInMeshChangeState())
	{
		MeshChangeAnimController.StopAnimationByName(InAnim);
	}
	else
	{
		ActorAT.StopAnimationByName(InAnim);
	
		foreach AttachedDestructableParts(info)
		{
			if(info.SkelMeshComponent != none &&   info.ActorAT !=none )
			{
				info.ActorAT.StopAnimationByName(InAnim);
			}
		}

		if(MountInfo!=none)
		{
			MountInfo.ActorAT.StopAnimationByName(InAnim);
		}
	}
}

event StopAnimationByKey(int InKey)
{
	local DestructablePartAttachInfo info;
	local MMOActorAT AT;
	if(MeshChangeAnimController != none)
	{
		MeshChangeAnimController.StopAnimationByKey(InKey);
	}

	ActorAT.StopAnimationByKey(InKey);
	
	foreach AttachedDestructableParts(info)
	{
		if(info.SkelMeshComponent != none &&   info.ActorAT !=none )
		{
			info.ActorAT.StopAnimationByKey(InKey);	
		}
	}

	if(MountInfo != none && MountInfo.ActorAT != none)
	{
		MountInfo.ActorAT.StopAnimationByKey(InKey);	
	}

	//WeaponEquip = GetWeaponEquip();
	//if (WeaponEquip != none)
	//{
	//	WeaponEquip.StopAnimByKey(InKey);
	//}

	AT = GetWeaponEquipMeshAnimationController(0);
	if(AT != none)
	{
		AT.StopAnimationByKey(InKey);
	}
	AT = GetWeaponEquipMeshAnimationController(1);
	if(AT != none)
	{
		AT.StopAnimationByKey(InKey);
	}
}



event BlendToIdle()
{
	StopAnimation();
}

event StopLoopAnim(name InAnim)
{
	local DestructablePartAttachInfo info;
	if(IsInMeshChangeState())
	{
		MeshChangeAnimController.StopLoopAnim(InAnim);
	}
	else
	{
		ActorAT.StopLoopAnim(InAnim);
	
		foreach AttachedDestructableParts(info)
		{
			if(info.SkelMeshComponent != none &&   info.ActorAT !=none )
			{
				info.ActorAT.StopLoopAnim(InAnim);
			}
		}

		if(MountInfo!=none)
		{
			MountInfo.ActorAT.StopLoopAnim(InAnim);
		}
	}
}

function BlendToLoops()
{
	local DestructablePartAttachInfo info;
	if(IsInMeshChangeState())
	{
		MeshChangeAnimController.BlendToLoops();
	}
	else
	{
		ActorAT.BlendToLoops();
	
		foreach AttachedDestructableParts(info)
		{
			if(info.SkelMeshComponent != none &&   info.ActorAT !=none )
			{
				info.ActorAT.BlendToLoops();
			}
		}
	}
}

native function OnAnimEndNative(AnimNodeSequence SeqNode, float PlayedTime, float ExcessTime);

event OnAnimEnd(AnimNodeSequence SeqNode, float PlayedTime, float ExcessTime)
{
	OnAnimEndNative(SeqNode, PlayedTime, ExcessTime);
}

event SetActionRate(float InRate)
{
	local DestructablePartAttachInfo info;
	ActorAT.SetActionRate(InRate);

	if(MeshChangeAnimController != none)
	{
		MeshChangeAnimController.SetActionRate(InRate);
	}

	foreach AttachedDestructableParts(info)
	{
		if(info.SkelMeshComponent != none &&   info.ActorAT !=none )
		{
			info.ActorAT.SetActionRate(InRate);
		}
	}

	if(MountInfo!=none && MountInfo.ActorAT!=none)
	{
		MountInfo.ActorAT.SetActionRate(InRate);
	}
	//ActionAnimRateNode.ScaleByValue = InRate;
}

event SetAnimRate(float InRate)
{
	SetActionRate(InRate);
	SetMoveAnimRate(InRate);
}

event PauseAnim(bool bPause)
{
	GetMainMesh().bPauseAnims = bPause;
	//Mesh.bPauseAnims = bPause;
}


function AddPreviewWeapon(StaticMesh sm, name socket)
{
	local StaticMeshComponent comp;
	if(sm == none || !class'Engine'.static.IsEditor())
	{
		return;
	}
	comp = new class'StaticMeshComponent';
	comp.SetStaticMesh(sm, true);
	Mesh.AttachComponentToSocket(comp, socket);
}

native function AddInteractiveWind(optional float Strength = 1.f);

// 物品管理类,角色初始装备在此这里被初始化
native function InitItems();

event SpawnMoveController()
{
	if(Controller != none)
	{
		Controller.Destroy();
	}

	if(TempController != none)
	{
		Controller = Spawn(TempController.Class, self, , , ,TempController, true);
	}
	else
	{   
		Controller = Spawn(class'MoveController', self, , , , , true);
	}
	//`log(" Controller ======= "$Controller);
	Controller.Pawn = self;
}

native function SetFreeze(bool InFreeze, Vector FreezeLocation);
native function SetShowTime(bool bOn, Vector EndShowLocation);

event SetStun(bool InStun)
{
	bStun = InStun;
}

event SetRevive()
{
	ClearOldEffects();

	DoRevive();
}

event PlayParEffect(ParticleSystem ParTemplate, Vector inLoc, Rotator inRot)
{
	WorldInfo.MyEmitterPool.SpawnEmitter(ParTemplate, inLoc, inRot);
}

native function RefreshAvatar();
native function NativePostBeginPlay();

native function bool ActorIsMoving();

native function EnableSkeletalControlFootPlacement(bool bOn);

// 初始化函数
simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	planeShadowIndex = -1;
	ServerID = -1;

	bSendRequestToServerToGetFullInfo = false;

	// init input state

	CurrentAbilityState = EAS_End;
	CanMoveInSpellTime = false;
	CanMoveInCastTime = false;
	AbilityStateCD=0.0;
	bCanRecvCmd = false;

	MoveSpeedFactor=1.0;
	//AttackSpeedFactor=1.0f;

	PassiveMoveType = PassiveMoveType_None;

	bRotationAnimation = false;

	//bVisible = true;
	bVisibleByAbility = true;
	bVisibleByMergeMesh = true;
	bVisibleByKismet = true;
	bVisibleByHotKey = true;
	bVisibleByCharacterCountLimit = true;
	bVisibleWeaponByAbility = true;


	RotateToMousePointRefrence = 0;

	bMovingToMousePoint = false;

	if(ActorAttributes != none)
	{
		Health = ActorAttributes.GetMaxHp();	
	}
	else
	{
		ActorAttributes = new class'ActorAttribute';
		Health = 1000;
	}

	bFreeze = false;

	if( GetAnimTypeByActor()>0 )
	{
		AnimBlendInTime     = 0.15;
		AnimBlendOutTime    = 0.15;
	}

	ElementaryType = 0;
	ElementaryCreatorId = 0;

	GpDataManager = class'GamePlayDataManager'.static.Singleton();
	//if(GpDataManager == none)
	//{
	//	`warn("GamePlayDataManage is none, check!!!");
	//}

	MountInfo = new class'Mount';
	HeroAttributes = new class 'ActorAttribute';

	InitItems();
	if(UseingAvatar!=none)
	{
		if(Mesh==none )
		{
			Mesh =  new() class'SkeletalMeshComponent';
		}
		RefreshAvatar();
		RefreshCollision();
	}
	else
	{
		NativePostBeginPlay();
	}

	RefreshCollisonRate();

	InitAnimNodes();
	RefreshAnimSet();
	//ResetAnimation();
	
	PlaySpawnAnim();
	InitPlaneShow();
	SetEnvironmentComponentEnable();

}

//event EnterGame()
//{
//	SetPhysics(PHYS_Walking);
//	bWaitForAbilityReply = false;
//}

final native function EnterGame();

final native function LeaveGame();

//event LeaveGame()
//{
//	SetPhysics(PHYS_None);
//}


//---------------------------------------------------------------
// 调试函数
//---------------------------------------------------------------

// 设置近战范围半径
function SetMeleeAttackRange(float val)
{
// 	if( DebugRangeNear != none )
// 	{
// 		DebugRangeNear.CylinderRadius = val;
// 		DebugRangeNear.CylinderTopRadius = val;
// 		DebugRangeNear.ForceUpdate(false);
// 	}
	RangeAttackRange = val;
}

// 设置远程范围半径
function SetRangeAttackRange(float val)
{
// 	DebugRangeFar.CylinderRadius = val;
// 	DebugRangeFar.CylinderTopRadius = val;
// 	DebugRangeFar.ForceUpdate(false);
	MeleeAttackRange = val;
}

//---------------------------------------------------------------
// 战斗相关函数接口，供各状态调用
//---------------------------------------------------------------

event int GetFriendlyMask()
{
	return FriendlyMask;
}

// 获取动画控制打断类型，比如受伤害动作是否打断其他正在进行的动作，add by jimzhang 2012.8.10
event int GetAnimTypeByActor()
{
	return 0;
}

// 检查可破坏的装备，并以各种方式表现出来
event OnCheckDestructableEquipments();

// 死亡动画播放完成的事件
event OnDeathAnimationEnd();

// 死亡动画播放完成后停止动画
function PauseAnimationAfterDeath()
{
	Mesh.bPauseAnims = true;
	OnDeathAnimationEnd();
}

//function DoRevive()
//{
//	bDead = false;
//	bFreeze = false;
//	bStun = false;
//	bWaitForAbilityReply = false;
//	bCollideWorld = false;
//	SetPhysics(PHYS_Walking);
//	RefreshAnim();

//	// play dying animation
//	PlayAnimation(PawnAnimInfo.IdleAnim);

//	if(IsMainPlayer())
//	{
//		RebuildMaterialControllerList();
//	}
//}

native final function DoRevive();

native function SetDead();

// 处理由技能直接带来的死亡函数
native function ProcessAbilityCauseDie(DeathStyle InStyle, EElementaryType InElementaryType, float Delay);

// 处理服务器通用死亡协议带来的死亡
//native function ProcessDefaultDie(bool bNeedPlayDieAction = false);
native function ProcessDefaultDie(byte DieCause);


native function ClearOldEffects();


//---------------------------------------------------------------
// 动画播放控制
//---------------------------------------------------------------
event OnAnimPlay(AnimNodeSequence SeqNode)
{
	PlayingAnim = SeqNode.AnimSeqName;
}

exec function float PlayTestAnim(int i)
{
	local float ret;
	switch (i)
	{
	case 0:
		ret = PlayAnimation('Run');
		break;
	case 1:
		ret = PlayAnimation('attack01');
		break;
	case 2:
		ret = PlayAnimation('attack02');
		break;
	case 3:
		ret = PlayAnimation('attack03');
	case 4:
		ret = PlayAnimation('SlideDown_Loop');
		break;
	}
	return ret;
}

//function AnimNodeSequence GetAnimSeq(name AnimName)
//{
//	if(ActorAT.Action1.AnimSeqName == AnimName)
//	{
//		return ActorAT.Action1;
//	}
//	else if(ActorAT.Action2.AnimSeqName == AnimName)
//	{
//		return ActorAT.Action2;
//	}
//	else
//	{
//		return none;
//	}

//	//if(Action1.AnimSeqName == AnimName)
//	//{
//	//	return Action1;
//	//}
//	//else if(Action2.AnimSeqName == AnimName)
//	//{
//	//	return Action2;
//	//}
//	//else
//	//{
//	//	return none;
//	//}
//}

/** Util for scaling running anims etc. */
simulated function SetAnimRateScale(float RateScale)
{
	Mesh.GlobalAnimRateScale = RateScale;
}


/** returns whether a particular custom animation is playing on
 *  any of our custom anim blenders 
function bool IsPlayingCustomAnim(name inAnim, optional float timeFromEndToConsiderFinished)
{
	local int it;
	local bool value;

	//see if any of the custom blenders is active and playing the animation we're interested in
	//for(it=0;it < CustomAnimNodes.Length;it++)
	//{
	it = LastCustomAnimNodePlayIndex;
		value = CustomAnimNodes[it].GetCustomAnimNodeSeq().AnimSeqName == inAnim
			&& (CustomAnimNodes[it].Child2WeightTarget > 0 || CustomAnimNodes[it].Child2Weight > 0)
			&& CustomAnimNodes[it].GetCustomAnimNodeSeq().GetTimeLeft() > timeFromEndToConsiderFinished;
		
		if(value)
			return true;
	//}

	return false;
}*/

// 取得范围内可见的角色，被阻挡的不算
function array<Actor> GetVisibleActors(float radius, optional Vector orig = Location)
{
	local Actor tracedActor;
	local array<Actor> actors;
	foreach VisibleActors(class'Actor', tracedActor, radius, orig)
	{
		actors.AddItem(tracedActor);
	}

	return actors;
}

// 进入安全区
event OnEnterSafeZone()
{
}

// 离开安全区
event OnLeavingSafeZone()
{
}


native function RotateToMosuePoint();


simulated event Tick( float deltaTime )
{
	// local int idx;
	local int rad;
	local Vector NewVec;//, hitloc, hitnor, vs,ve;
//	local float deltaf;
//	local Rotator NewRot;

	super.Tick(deltaTime);


	if( lookAtActor != none && lookAtController != none )
	{
		lookAtController.TargetLocation = lookAtActor.Location;
	}

	if(RotateToMousePointRefrence > 0)
	{
		RotateToMosuePoint();
		////NewVec = Normal(MainController.MouseHitWorldLocation - Location);
		//NewVec = Normal(MainController.MouseProjectionDirection) - Normal(Location - MainController.MouseProjectionOrigin);
		//NewVec.Z = 0;
		//NewVec = Normal(NewVec);
		//NewRot = Rotator(NewVec);
		//NewRot = Normalize(NewRot);
		//deltaf = Abs(Abs(NewRot.Yaw) - Abs(Rotation.Yaw)) / RotateSpeed;
		//LockDesiredRotation(false, true);
		//SetDesiredRotation(NewRot, false, false, deltaf, true);
		////`log("NewRot="$NewRot$"  deltaf="$deltaf);
	}

	if(ActionPlayedLeftTime > 0)
	{
		ActionPlayedLeftTime -= deltaTime;
		if( ActionPlayedLeftTime <= 0 )
			PlayingAinmName ='';
	}

	// show centerbox
	if(MMOGame(WorldInfo.Game).DrawSelfCenterBox)
	{
		rad = GetCollisionRadius();
		rad += 10;
		NewVec.X = rad;
		NewVec.Y = rad;
		NewVec.Z = rad;
		DrawDebugBox(Location, NewVec, 0, 255, 0, false);
	}

	// update item manager
	if( ItemManager != none )
		ItemManager.Tick( deltaTime );

	//UpdatePlaneShadowPosition();
}

simulated event Destroyed()
{
	super.Destroyed();
	SelfDestroyed(ServerID);
	ReleasePlanShow();
}

native function PlayRandomAnim();

native function ResetIdleTimer();

native function SkeletalmeshComponent GetActiveBaseMesh();

native function SetVisible(bool InVisible);

native function SetEmitterAttachedVisible(bool InVisible);

native function SetVisibleByAbility(bool InVisible);
native function SetVisibleByMergeMesh(bool InVisible);
native function SetVisibleByKismet(bool InVisible);
native function SetVisibleByHotKey(bool InVisible);
native function SetVisibleByCharacterCountLimit(bool InVisible);
native function RefreshVisible();
native function SetWeaponVisibleByAbility(bool InVisible);

native function bool CanShowAbilityEmitter(int CreatorId);


// 取得Socket位置
function vector GetSocketWorldPosition(name socketName)
{
	local Vector pos;
	local rotator rot;

	Mesh.GetSocketWorldLocationAndRotation(
		socketName, pos, rot, 0);// 0 == World, 1 == Local (Component)

	return pos;
}

// 取得Socket朝向
function rotator GetSocketWorldRotation(name socketName)
{
	local Vector pos;
	local rotator rot;

	Mesh.GetSocketWorldLocationAndRotation(
		socketName, pos, rot, 0);// 0 == World, 1 == Local (Component)

	return rot;
}

event GetSocketWorldLocationAndRotation(name SockeName, out Vector vec, out Rotator Rot, optional int Space = 0)
{
	if(Mesh == none || SockeName == '' || Mesh.GetSocketByName(SockeName) == none)
	{
		vec = Location;
		rot = Rotation;
	}
	else
	{
		Mesh.GetSocketWorldLocationAndRotation(SockeName, vec, rot, space);
	}
}

// 在头顶播放特效
function PlayOverHeadSFX(ParticleSystem template)
{
	if(SFX_OverHead == none)
	{
		SFX_OverHead = Spawn(class'MMOEmitterSpawnable',,, Location + vect(0, 0, 100), Rotation, , true);
		SFX_OverHead.SetBase(self);
		SFX_OverHead.ParticleSystemComponent.SetTemplate(template);
		MMOEmitterSpawnable(SFX_OverHead).MyPointLightComponent.SetEnabled(false);
		Mesh.AttachComponentToSocket( SFX_OverHead.ParticleSystemComponent, 'Hit_OverHead' );
	}

	SFX_OverHead.ParticleSystemComponent.ActivateSystem();
	
	SetTimer(0.4, false, 'StopOverHeadSFX' );
}

// 停止头顶特效
function StopOverHeadSFX()
{
	if(SFX_OverHead != none)
	{
		SFX_OverHead.ParticleSystemComponent.DeactivateSystem();
	}
}

//---------------------------------------------------------------------------------------------------
// *    weapon releated functions
//---------------------------------------------------------------------------------------------------

// 显示或隐藏当前装备的武器
function ShowWeapon(bool show)
{
	//if(ItemManager.EquippedItemList_Normal[E_NES_MAIN_HAND]!=none)
	//{
	//	ItemManager.EquippedItemList_Normal[E_NES_MAIN_HAND].SetHidden(!show);
	//}
	HideWeapon(!show);
}

// 恢复到默认的武器显示状态
function RestoreWeaponVisiblity()
{
	ShowWeapon(true);
}

// 校验武器模型
function ValidateWeaponMesh()
{
	//if(WeaponMesh[0] == none)
	//{
	//	WeaponMesh[0] = new( self ) class'StaticMeshComponent';
	//}

	//if(WeaponMesh[1] == none)
	//{
	//	WeaponMesh[1] = new( self ) class'StaticMeshComponent';
	//}
}


// 绑定静态模型到绑定点
function AttachStaticMesh(name attachName, StaticMesh sm, name socketName)
{
	local StaticMeshAttachInfo smai;
	local LightingChannelContainer lcc;

	//lcc.Dynamic = true;
	//lcc.CompositeDynamic = true;
	//lcc.Gameplay_4 = true;
	lcc = Mesh.LightingChannels;

	smai.AttachName = attachName;
	smai.AttachSocket = socketName;
	smai.MeshAsset = sm;
	smai.MeshComponent = new( self ) class'StaticMeshComponentEx';
	smai.MeshComponent.SetStaticMesh(sm);
	smai.MeshComponent.SetLightEnvironment(self.LightEnvironment);
//	smai.MeshComponent.bAcceptsDynamicDecals = false;
	smai.MeshComponent.SetLightingChannels(lcc);
	self.Mesh.AttachComponentToSocket( smai.MeshComponent, socketName );
	//`logd("SSSSSSSSSSSSSSSSSSS attach "@socketName);

	AttachedStaticMeshs.AddItem(smai);
}

// 解除静态模型绑定
function DetachStaticMesh(name attachName)
{
	local StaticMeshAttachInfo smai;
	foreach AttachedStaticMeshs(smai)
	{
		if(smai.AttachName == attachName)
		{
			self.Mesh.DetachComponent(smai.MeshComponent);
			AttachedStaticMeshs.RemoveItem(smai);
		}
	}
}

// 死亡后丢掉武器
function ThrowWeaponOnDeath();

// 抽刀收刀动画事件
event OnBladeInOut();

// 动画通知死亡时掉落武器
event OnDeathTrowWeapons();

function Vector GetAbilityTargetDirection( );
function Vector GetAbilityTargetLocation( );

// 绑定粒子特效
function AttachParticleSystem(ParticleSystemAttachInfo psai)
{
	local array<SkeletalMeshComponent> wpMesh;
	local int i;

	if(psai.EmitterInstance != none)
	{
		`log("[MMOActor::AttachParticleSystem] Emitter instance already exist!");
		return;
	}

	//`log("++++++++++++++++++++Attach particles" @ psai.EmitterTemplate @ psai.BindToWeapon);

	if(!psai.BindToWeapon)
	{
		psai.EmitterInstance = new( self ) class'ParticleSystemComponent';
			psai.EmitterInstance.SetScale(psai.EffectScale);
		psai.EmitterInstance.SetTemplate(psai.EmitterTemplate);
		psai.EmitterInstance.ActivateSystem();

		Mesh.AttachComponentToSocket(psai.EmitterInstance, psai.AttachSocket);
	}
	else
	{
		wpMesh = GetClientWeaponSkMesh();
		//`log("++++++++++++++++++++Attach to weapon" @ wpMesh.Length @ psai.EmitterTemplate);
		for(i = 0; i < wpMesh.Length; ++i)
		{
			// TODO: 
			//psai.EmitterInstance = WorldInfo.MyEmitterPool.SpawnEmitter(psai.EmitterTemplate, vect(0, 0, 0), MakeRotator(0,0,0));
			psai.EmitterInstance = new( self ) class'ParticleSystemComponent';
			psai.EmitterInstance.SetTemplate(psai.EmitterTemplate);
			psai.EmitterInstance.SetScale(psai.EffectScale);			
			wpMesh[i].AttachComponentToSocket(psai.EmitterInstance, psai.AttachSocket);
			psai.EmitterInstance.SetSkeletalMeshComponentParameter(psai.VertSurfaceActorParameterName, wpMesh[i]);
			psai.EmitterInstance.ActivateSystem();
			//`log("++++++++++++++++++++Attach to weapon" @ wpMesh[i] @ psai.EmitterTemplate @ psai.AttachSocket);
		}
	}

	self.AttachedParSys.AddItem(psai);
}

// 解除绑定粒子特效
function DetachParticleSystem(ParticleSystemAttachInfo psai)
{
	local ParticleSystemAttachInfo info;
	foreach AttachedParSys(info)
	{
		if(info == psai)
		{
			Mesh.DetachComponent(info.EmitterInstance);
			AttachedParSys.RemoveItem(info);
			break;
		}
	}
}

/**
 * Handles actual playing of sound.  Separated from PlayFootstepSound so we can
 * ignore footstep sound notifies in first person.
 */
simulated function ActuallyPlayFootstepSound(int FootDown)
{
	local SoundCue Sound;
	local ParticleSystem Particle;
	local EPhyMatType MatType;
	local EmitterSpawnable FootEmitter;
	local Vector HitLocation, HitNormal;


	MatType = GetHitInfoBelowFeet(HitLocation, HitNormal);
	if(!GpDataManager.FindFootstepEffect(BodyType, MatType, Sound, Particle))
	{
		`log("can not get foot step effect by BodyType="$BodyType$"  LandType="$MatType);
		return;
	}

	if (Sound != None)
	{
		PlaySound(Sound, false, true,,, true);
	}
	else
	{
		//`log("no sound to play");
	}

	// 这种生成不是最好(当地是斜面时),
	// 最好是从人中心向地面trace,然后获得trace到的normal,
	// 计算平铺于地面的角度作为正确的角度 by fangruyi

	if(Particle != none)
	{
		FootEmitter = Spawn(class'EmitterSpawnable',,,
			HitLocation, Rotator(HitNormal), , true);
		FootEmitter.SetTemplate(Particle);
		FootEmitter.ParticleSystemComponent.ActivateSystem();
		FootEmitter.LifeSpan = 10;
	}
}

simulated function EPhyMatType GetHitInfoBelowFeet(out Vector HitLocation, out Vector HitNormal)
{
	//local vector HitLocation, HitNormal;
	local TraceHitInfo HitInfo;
	local PhysicalMaterialProperty PhysicalProperty;
	local actor HitActor;
	local float TraceDist;

	TraceDist = 1.5 * GetCollisionHeight();

	HitActor = Trace(HitLocation, HitNormal, Location - TraceDist*vect(0,0,1), Location, false,, HitInfo, TRACEFLAG_PhysicsVolumes);
	//`log("__Hit actor" @HitActor @HitInfo.Material @HitInfo.PhysMaterial @HitInfo.BoneName @HitInfo.HitComponent @HitInfo.Item @HitInfo.LevelIndex);
	if ( WaterVolume(HitActor) != None )
	{
		//return (Location.Z - HitLocation.Z < 0.33*TraceDist) ? EPMAT_Water : EPMAT_ShallowWater;
		return EPMAT_Water;
	}

	if (HitInfo.PhysMaterial != None)
	{
		PhysicalProperty = PhysicalMaterialProperty(HitInfo.PhysMaterial.GetPhysicalMaterialProperty(class'PhysicalMaterialProperty'));
		if (PhysicalProperty != None)
		{
			return PhysicalProperty.MaterialType;
		}
	}
	return EPMAT_Default;

}

event PlayFootStepSound(int FootDown)
{
	PlayFootStepSoundEx();
}

event PlayFootStepSoundEx()
{
	//if(!class'Engine'.static.IsEditor())
	//{
		ActuallyPlayFootstepSound(0);
	//}
}

native function SkeletalMeshComponent GetMasterComponent();

function SetLookAtSomebody( MMOActor targetActor )
{
	if( lookAtController != none )
	{
		if( targetActor != none )
		{
			lookAtController.ControlStrength = 1.f;
			lookAtActor = targetActor;
		}
		else
		{
			lookAtController.ControlStrength = 0.f;
			lookAtActor = none;
		}
	}
}

// 模型淡入淡出效果
event StartMeshFade( bool FadeIn, float Time )
{
	ItemManager.StartMeshFade( fadeIn, time );
}



native function SetMoveType(EActorMoveType InType);
native function EActorMoveType GetMoveType();

native function UpdateMoveType(float NewSpeed);



// DamageValue > 0 add, < 0 : sub
native function ShowDamageText(EDamageResultType InType, bool bCauseDead, EDamagePropertyType PropertyType, int DamageValue);

native function MMOPlayer GetBasePlayer(Actor InActor);
native function bool GetRelevantInfo(Actor InA, Actor InB, out Actor Relevant);

native function NativeProcessDamage(DamageData InDamage);

event ProcessDamage(DamageData Damage)
{
	NativeProcessDamage(Damage);

	////`log("Script ProcessDamage ");
	//if(Damage.bServerDamageData)
	//{
	//	if(Damage.bCauseDead)
	//	{
	//		ProcessAbilityCauseDie(Damage.AbilityData.AbilityDeath, Damage.ElementaryType);
	//		if(Damage.AbilityData != none)
	//		{
	//			Damage.AbilityData.ClientPlay(Damage.Attacker, none, self, Location, 1.0f);
	//		}
	//	}
	//	// only show main-player relevant texts( dmg that caused by the player or to the player )
	//	if( IsMainPlayer() || ( MMOPlayer(Damage.Attacker) != none && MMOPlayer(Damage.Attacker).IsMainPlayer() ) )
	//		ShowDamageText(Damage.DamageType, Damage.bCauseDead, EDPT_Hp, Damage.DamageValue);
	//}
	//else
	//{
	//	if(Damage.AbilityData != none)
	//	{
	//		Damage.AbilityData.ClientPlay(Damage.Attacker, none, self, Location, 1.0f);
	//	}
	//}
}

event bool IsDead()
{
	return bDead;
}

native function SetActorLevel( int level );
native function int GetActorLevel( );

native function float GetCollisionRadius();
native function float GetCollisionHeight();

state Dying
{
	//function bool ProcessDamage(DamageData Damage)
	//{
	//	//`log("Dying====> HitMomention="$Damage.HitMomention);
	//	Mesh.AddImpulse(Damage.HitMomention);
	//	return true;
	//}
}

native function SetSilhouetteEx( bool show, Vector sc = vect( 0.f, 0.f, 0.f ) );


// 根据目标坐标设置角色朝向
event SetDirection( Vector varDestLocation )
{
	local Vector PawnLocationXY;
	local Vector DestLocationXY;

	PawnLocationXY.X = Location.X;
	PawnLocationXY.Y = Location.Y;

	DestLocationXY.X = varDestLocation.X;
	DestLocationXY.Y = varDestLocation.Y;

	TargetRotation = Rotator( DestLocationXY - PawnLocationXY );
}

function UpdateRotation(float Delta)
{
	if(TargetRotation.Yaw != Rotation.Yaw)
	{
		SetRotation(RInterpTo(Rotation, TargetRotation, Delta, RotationSpeed));
	}
}

//-----

native function PlayRandomSound(array<SoundCue> soundList);


event Stopped()
{
	//`log("Stopped");
	isMoving = false;
	SetMoveAnimRate(1.0f);
	NotifyWalkEnd();
	UpdateAttachAnim(-1);
}

event NotifyWalkEnd()
{
	if(Controller != none)
	{
		Controller.NotifyWalkEnd();
	}
}

//add by jimzhang 2012.7.13,同步绑定有骨骼动画物体的动画
function PlayAttachLoopAnim()
{
		local Name   nameAction;
		local DestructablePartAttachInfo info;

		if(isMovingLast == false)
		{
			nameAction = 'Idle';
		}
		else if( MoveType == EMT_Walk || MoveType == EMT_Walk_Oblique )
		{
			nameAction = 'Walk';
		}
		else if( MoveType == EMT_Run || MoveType == EMT_Run_Oblique)
		{
			nameAction = 'Run';
		}
		else if( MoveType == EMT_InStall || MoveType == EMT_PrepareStall )
		{
			//@todo 等待资源
			nameAction = 'Fly';
		}
		else
		{
			nameAction = 'Fly';
		}

		foreach AttachedDestructableParts(info)
		{
			if(info.SkelMeshComponent != none &&   info.SkelMeshComponent.AnimSets.Length>0 )
			{
				`log("2#################AttachedDestructableParts[i].SkelMeshComponent.PlayAnim " @nameAction $"    ServerID="$ServerID$"SkelMeshComponentIndex="   );
				info.SkelMeshComponent.PlayAnim( nameAction ,0.0, true);
			}
		}

		if(MountInfo!=none && MountInfo.SkelMeshComponent!=none)
		{
			MountInfo.SkelMeshComponent.PlayAnim( nameAction ,0.0, true);
		}
	}

function SetAttachAnim( int nActionOrLoop )
{
	nActorActionOrLoop = nActionOrLoop;
}

function UpdateAttachAnim( int nActionOrLoop )
{
	//action和loop的切换
	if( nActorActionOrLoop!= nActionOrLoop&&nActionOrLoop>=0)
	{
		nActorActionOrLoop = nActionOrLoop;
		if(nActionOrLoop==0)
		{
			PlayAttachLoopAnim();
		}			
	}

	if( nActorActionOrLoop == 1)
	{
		return;
	}


	//行走和待机的切换
	if( isMovingLast != isMoving || isMoving == true )
	{
		isMovingLast = isMoving;
		//if(isMovingLast || IsPlayingAnim() == false)
		//{
			PlayAttachLoopAnim();
		//}
	}

}

native function float GetMoveAnimRate(int NewSpeed);
native function MoveTo(Vector Des, optional float speed = 0 ,optional bool rotationToDes =false,optional float inMoveRateScale =1.0);

native function RotatoTo(Rotator InRot, float InDuration);

native function ResetMoveTo(byte InMoveState, float speed, Vector Dst, Vector Src, Rotator Dir);

native function UpdateMoveSpeed(float NewSpeed);

// 设置被抛状态
native function SetThrowTo(Vector Des, float LastTime);
// 开始执行抛物状态
native function DoThrowTo(Vector InP0, Vector InP1, Vector InT0, Vector InT1, float InLastTime, float InExp);

native function DashTo(Vector Des, float InLastTime);

native function FearTo(Vector Des, float InLastTime);

event TriggerEvent(int outputIndex)
{
	local Sequence GameSeq;
	local array<SequenceObject> AllSeqEvents;
// 	local array<int> ActivateIndices;
	local int i;
	local SeqEvent_Server2Client_NPCDie eventObj;
	local SeqEvent_Server2Client_NPCGroupDie eventGroupDie;

	//`log("OSCAR:!!!!_______ Trigger Event Index:" @outputIndex);

	// reset Kismet and activate any Level Reset events
	GameSeq = WorldInfo.GetGameSequence();
	if (GameSeq != None)
	{
		// reset the game sequence
		//GameSeq.Reset();

		// find any Level Loaded events that exist
		GameSeq.FindSeqObjectsByClass(class'SeqEvent_Server2Client_NPCDie', true, AllSeqEvents);

		// activate them
// 		ActivateIndices[0] = outputIndex;
		for (i = 0; i < AllSeqEvents.Length; i++)
		{
			eventObj = SeqEvent_Server2Client_NPCDie(AllSeqEvents[i]);
			`log("=============================SeqEvent_Server2Client_NPCDie" );
			//if(!eventObj.HasInstigator() || eventObj.IsDesiredInstigator(self))
			//{
				////`log("OSCAR:!!!!_______ Trigger Event Index:" @outputIndex);
				//eventObj.CheckActivate(WorldInfo, self, false, ActivateIndices);
				eventObj.ForceActivateOutput(outputIndex);
			//}
		}

		//NPC Group Die
		GameSeq.FindSeqObjectsByClass(class'SeqEvent_Server2Client_NPCGroupDie', true, AllSeqEvents);

		// activate them
// 		ActivateIndices[0] = outputIndex;
		for (i = 0; i < AllSeqEvents.Length; i++)
		{
			eventGroupDie = SeqEvent_Server2Client_NPCGroupDie(AllSeqEvents[i]);
			`log("=============================SeqEvent_Server2Client_NPCGroupDie" );
			//if(!eventObj.HasInstigator() || eventObj.IsDesiredInstigator(self))
			//{
				////`log("OSCAR:!!!!_______ Trigger Event Index:" @outputIndex);
				//eventObj.CheckActivate(WorldInfo, self, false, ActivateIndices);
				eventGroupDie.ForceActivateOutput(outputIndex);
			//}
		}



	}
}

// 动画事件通知，处理武器拔出成功后消除场景中的对象
event OnAttachDestructablePart(DestructablePartAttachInfo Info);

// 绑定的武器掉落时清除特效
event OnDetachDestructablePart(DestructablePartAttachInfo dpai);


native function SetAttitudeType(EWeaponType type);
native function RevertAttitudeType();

// 装备坐骑
native function EquipMount(int MountID);
//{

//	if(mount!=none)
//	{
//		MountInfo = new class'Mount';
//		MountInfo.SkelMeshComponent = mount.SkelMeshComponent;
//		MountInfo.ActorAT           = mount.ActorAT;
//		MountInfo.actor             = self;
//		MountInfo.Init();

//		AttachComponent( MountInfo.SkelMeshComponent );
//		MountInfo.SkelMeshComponent.AttachComponentToSocket( Mesh, name("FX_Waist"));
//	}

//}

// 卸下坐骑
native function UnEquipMount();

// wing (1.05 wings.txt)
native function EquipWing(int WingID);
native function UnEquipWing();
//{
//	if(MountInfo!=none)
//	{
//		DetachComponent(MountInfo.SkelMeshComponent);
//		MountInfo = none;
//	}
//}


// 绑定可破坏部件
function AttachDestructablePart(DestructablePartAttachInfo dpai)
{
	//local int j;
	//`log("________ AttachDestructablePart" @ self @ dpai.AttachSocket @ dpai.IsWeapon);
	if(dpai.SkelMeshComponent==none &&  dpai.IsStaticMeshComponent==false)
	{
		return;
	}

	if(dpai.IsStaticMeshComponent)
	{
		dpai.MeshComponent = new( self ) class'StaticMeshComponent';
		dpai.MeshComponent.SetStaticMesh(dpai.MeshAsset);
		dpai.MeshComponent.SetLightEnvironment(self.LightEnvironment);
		dpai.MeshComponent.SetActorCollision(true, false, false);
		self.Mesh.AttachComponentToSocket( dpai.MeshComponent, dpai.AttachSocket );
		
		//`log("[MMOSFX] *** [AttachDestructablePart] attach static mesh:"
		//	@dpai.MeshAsset $" To:" @dpai.AttachSocket);
	}
	else
	{

		//`log("_________ AttachDestructablePart: SkeletalMeshComponent"@dpai.AttachSocket);

		//dpai.SkelMeshComponent = new(self) class'SkeletalMeshComponent';
		//dpai.SkelMeshComponent.SetSkeletalMesh(dpai.SkelMeshComponentAsset.SkeletalMesh);

		//for(j = 0; j < dpai.SkelMeshComponentAsset.AnimSets.Length; ++j)
		//{	
		//	dpai.SkelMeshComponent.AnimSets.AddItem(dpai.SkelMeshComponentAsset.AnimSets[j]);
		//}

		//dpai.SkelMeshComponent.SetAnimTreeTemplate(dpai.SkelMeshComponentAsset.AnimTreeTemplate);
		//dpai.SkelMeshComponent.SetPhysicsAsset(dpai.SkelMeshComponentAsset.PhysicsAsset );

		//dpai.SkelMeshComponent.SetLightEnvironment(self.LightEnvironment);
		//dpai.SkelMeshComponent.SetActorCollision(false, false, false);


		if(dpai.AttachSocket =='None' )
		{
			AttachComponent(dpai.SkelMeshComponent);
			Mesh.AttachComponentToSocket( dpai.SkelMeshComponent, dpai.AttachSocket );
			dpai.SkelMeshComponent.SetParentAnimComponent( Mesh );
		}
		else
		{
			Mesh.AttachComponentToSocket( dpai.SkelMeshComponent, dpai.AttachSocket );
		}	
	}

	
	dpai.Init();
	
	AttachedDestructableParts.AddItem(dpai);

	//`log("[MMOSFX] ***" @ dpai.SkelMeshComponent @ dpai.MeshComponent @ AttachedDestructableParts.Length);
}

// 解除可破坏部件
function DetachDestructablePart(DestructablePartAttachInfo dpai)
{
	//`log("[MMOSFX] [DetachDestructablePart] :" @dpai.MeshComponent @dpai.SkelMeshComponent);
	OnDetachDestructablePart(dpai);

	if(dpai.IsStaticMeshComponent)
	{
		self.Mesh.DetachComponent(dpai.MeshComponent);
	}
	else
	{
		if(dpai.AttachSocket =='None' )
		{
			DetachComponent(dpai.SkelMeshComponent);
			Mesh.SetParentAnimComponent( none );
		}
		else{
			Mesh.DetachComponent(dpai.SkelMeshComponent);
			Mesh.SetParentAnimComponent( none );
		}		
	}

	AttachedDestructableParts.RemoveItem(dpai);
	//`log("[MMOSFX] [DetachDestructablePart] :" @AttachedDestructableParts.Length);
}

native function EPickPriority GetPickPriority();
native function EMMOCursorType GetMouseCursor(); // oscar@snda

native final function array<SkeletalMeshComponent> GetClientWeaponSkMesh();
native function CloneFrom( const MMOActor BaseActor );

native function SetNewElementary(int InCreatorId, EElementaryType InType);
native function RemoveElementary(int InCreatorId);


//是否在摆摊相关的状态中 
event bool IsStallState()
{
	if( MoveType == EMT_PrepareStall || MoveType ==EMT_InStall ) 
		return true;
	else 
		return false;
}

simulated event FellOutOfWorld(class<DamageType> dmgType)
{
}

simulated singular event OutsideWorldBounds()
{
}

/**
* UpdateStaticLightEnvironment
* Update the light environment's static lighting, during day-night changes
*/
native function UpdateStaticLightEnvironment();

native function TryPlaceActorToGround();
native function ResetPlaceActorToGroundTimer();
native function DisablePlaceActorToGroundTimer();

defaultproperties
{
	ControllerClass=class'MoveController'
	FriendlyMask=1
	ActorAT = none
	MountInfo = none
	AttitudeType = EWT_Weapon_0
	AttitudeTypeLast = EWT_Weapon_0
	ServerHeroID = 0
	HeroSummon   = false
	HeroAi       = 1
	ShowHeadName = true
	dwHostID     = 0

	Physics=PHYS_None
	LandMovementState=none

	CollisionType = COLLIDE_TouchAll
	CollisionHeightRate=1.0f
	CollisionRadiusRate=1.0f

	RangeAttackRange=800.0f
	MeleeAttackRange=440.0f

	BodySize=E_body_1x1
	//ConstantMat=MaterialInstanceConstant'FX_UI_Material.UI_MIC_B'
	//ConstantMat=MaterialInstanceConstant'FX_MageIce.02Mat.M_IceMatBuff_001_INST'
	ConstantMat=MaterialInstanceConstant'Fx_LevelActor.M_Portal_001_INST'
	TimeMat=MaterialInstanceTimeVarying'Fx_LevelActor.M_PorTal_002_INST'
	

	//// 添加调试用范围线框 - 远程攻击范围
	//Begin Object Class=DrawCylinderComponent  Name=DebugRangeFar0
	//	CylinderColor=(R=255,G=0,B=0,A=255)
	//	CylinderRadius=600.0
	//	CylinderTopRadius=600.0
	//	CylinderHeight=0.0
	//	CylinderHeightOffset=0.0
	//	CylinderSides=32
	//	bDrawWireCylinder=true
	//	HiddenGame=true
	//End Object
	//Components.Add(DebugRangeFar0)
	//DebugRangeFar=DebugRangeFar0

	//// 添加调试用范围线框 - 近战范围
	//Begin Object Class=DrawCylinderComponent  Name=DebugRangeNear0
	//	CylinderColor=(R=255,G=0,B=0,A=255)
	//	CylinderRadius=200.0
	//	CylinderTopRadius=200.0
	//	CylinderHeight=0.0
	//	CylinderHeightOffset=0.0
	//	CylinderSides=32
	//	bDrawWireCylinder=true
	//	HiddenGame=true
	//End Object
	//Components.Add(DebugRangeNear0)
	//DebugRangeNear=DebugRangeNear0

	Begin Object Class=DynamicLightEnvironmentComponent Name=MyLightEnvironment
		bEnabled=TRUE
		TickGroup=TG_DuringAsyncWork
		bSynthesizeSHLight=FALSE
		bIsCharacterLightEnvironment=TRUE
		bUseBooleanEnvironmentShadowing=FALSE
		//bCastShadows=FALSE
		//bForceCompositeAllLights=FALSE
		//bAlwaysInfluencedByDominantDirectionalLight=true
		//bForceCompositeAllLights=true
	End Object
	Components.Add(MyLightEnvironment)
	LightEnvironment=MyLightEnvironment


	//Begin Object Class=StaticMeshComponent Name=StaticMeshComponent1
	//	HiddenGame=false
	//	CastShadow=false
	//	CollideActors=false
	//	AlwaysLoadOnServer=FALSE
	//	AlwaysLoadOnClient=TRUE
	//	Scale3D=(X=-1,Y=1,Z=1)
	//	StaticMesh=StaticMesh'axes.axes'
	//End Object
	//Components.Add(StaticMeshComponent1)

	////创建骨骼模型
	//clothing system
	//Begin Object Class=SkeletalMeshComponent   Name=MasterMesh     // master piece pour caching animations
	//	//SkeletalMesh=SkeletalMesh'MMO_Player_Body.man_body'
	//	//AnimSets.Add(AnimSet'Demo_Characters.Anim.Man_Run')
	//	////AnimSets.Add(AnimSet'Demo_Characters_ManAnim.man_melee01')
	//	//AnimSets.Add(AnimSet'man_fight.amin.man_fight');
	//	//AnimSets.Add(AnimSet'man_fight.Melee_Normal');
	//	//AnimSets.Add(AnimSet'man_fight.Melee_Round');
	//	//Animations=none
	//	//AnimTreeTemplate=AnimTree'MMO_PlayerAnim.Anim.Man_AnimTree'
	//	//PhysicsAsset=PhysicsAsset'MMO_PlayerAnim.Anim.SK_MMO_Player_Physics'
	//	bUpdateSkelWhenNotRendered=true
		
	//	bCacheAnimSequenceNodes=FALSE
	//	AlwaysLoadOnClient=true
	//	AlwaysLoadOnServer=true
	//	bOwnerNoSee=true
	//	CastShadow=true
	//	BlockRigidBody=TRUE
	//	bUpdateSkelWhenNotRendered=false
	//	bIgnoreControllersWhenNotRendered=TRUE
	//	bUpdateKinematicBonesFromAnimation=true
	//	bCastDynamicShadow=true
	//	Translation=(Z=8.0)
	//	LightEnvironment=MyLightEnvironment
	//	bOverrideAttachmentOwnerVisibility=true
	//	bAcceptsDynamicDecals=FALSE
	//	//AnimTreeTemplate=AnimTree'CH_AnimHuman_Tree.AT_CH_Human'
	//	bHasPhysicsAssetInstance=true
	//	TickGroup=TG_PreAsyncWork
	//	MinDistFactorForKinematicUpdate=0.2
	//	bChartDistanceFactor=true
	//	//bSkipAllUpdateWhenPhysicsAsleep=TRUE
	//	RBDominanceGroup=20
	//	Scale=1.075
	//	MotionBlurScale=0.0
	//	bAllowAmbientOcclusion=false
	//	// Nice lighting for hair
	//	bUseOnePassLightingOnTranslucency=TRUE
	//End Object
	//Mesh=MasterMesh
	//Components.Add( MasterMesh )
	
	
	//HurtEffectTemplate=MMOEmitterSpawnable'MMOArchetTypes.Emitter_BodyHit_Sword'
	//HurtEffectTemplate=MMOEmitterSpawnable'MMOArchetTypes.Emitter_FireballExplosion'
	//ParticleSystem'VH_Manta.Effects.PS_Manta_Gun_MuzzleFlash'

	//BlockRigidBody=true

	//bEnableFullAnimWeightBodies=true

	MoveType=EMT_Run

	MaxFootstepDistSq=9000000.0


	Begin Object Class=AudioComponent Name=AudioComponent01
	End Object
	Components.Add(AudioComponent01)
	//CustomSoundComponent=AudioComponent01
	CurrentAttackRange=200

	hasBladeOutAnim=false
	isDivideBody=false
	isHoldingStone=false

	lookAtActor=none
	lookAtController=none

	bHoldWeapon=true
	isMoving=false
	isMovingLast = false;
	ResetMovePhaseTime=-1.0f
	ResetLastLocationTime=-1.0f
	AttackAi_LockTime=-1.0f
	C_EquipCount=14
	Mass=6000
	
	ShowHpNumber=0

	nActorActionOrLoop=-1
	GridX=-100
	GridY=-100
	bIsActorInSafeZone=false


	//EmitterScaleValue=1.0

	RotationSpeed=10.0f
	bRunPhysicsWithNoController=true
	bUseSimpleCheckReachDestination=true
	bForceMaxAccel=true
	bCanStepUpOn=false
	bPushedByEncroachers=false
	RotationRate=(Pitch=30000,Yaw=120000,Roll=2048)

	AnimBlendInTime=0.1
	AnimBlendOutTime=0.1
	MoveAnimRate=1.0

	bCollideAsEncroacher=false
	bNoEncroachCheck=true
	bUsingUnrealWalking=false
	bCollideWorld=false

	TeamId=0
	
	// oscar@snda disable apex destruction trace when move
	bMoveIgnoresDestruction=false

	// 测试RT特效时打开
// 	Begin Object Class=ParticleSystemComponent Name=ParEffect0test
// 		bAutoActivate=true
// 		bIsActive=true
// 		Template=ParticleSystem'FX_Scene.Fire.Scene_fire_L'
// 	End Object
// 	Components.Add(ParEffect0test)
	
	inSabacZoneCount=0
	CurrentMoveSpeed=500

	PlaceToGoundTickCount=5
	bResourceInitialized=false
	bActorDataReset=false
	fActorDataResetDelay=0.3
	PetId=0

	planeShadowIndex=-1
	UsePlaneShadow=true;


	CurBlockRadius=-1
	CurBlockType=-1
	NeedRefreshMaterial=false
}
