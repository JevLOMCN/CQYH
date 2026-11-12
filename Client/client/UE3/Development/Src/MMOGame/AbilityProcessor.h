#ifndef _ABILITY_PROCESSOR_H_
#define _ABILITY_PROCESSOR_H_

//#include "stdafx.h"
#include "spell_def.h"
#include "MMOUtil.h"

#include <vector>
#include <map>

//using namespace std;

#define SHOW_ABILITY_LOG 0
#define SHOW_PUSH_EFFECT 0
#define SHOW_ABILITY_WATCH_ABILITY_ID 0
#define SHOW_ABILITY_MEMORY 0
#define SHOW_ABILITY_COMPONENT_LOG 0
#define SHOW_ABILITY_ANIMATION_LOG 0
#define SHOW_ABILITY_MOVE_CONTROL_LOG 0
#define SHOW_TRAIL_LOG 0
#define SHOW_MIssile_LOG 1
#define SHOW_PHYSIC_LOG 0
#define SHOW_POSITION_TRANSFORM 0
#define SHOW_ABILITY_EMITTER_LOG 0

#define SHOW_MOVEMENT_LOG 0

#define DEBUG_RATE 2.f

#define MAX_EFFECT_LIFE_SPAN 60.f

#define HIT_POINT_MAX_NUMBER 6
#define ON_CAST_HIT_POINT_INDEX 5

#define DEFAULT_EMITTER_MAX_NUMBER 80



class AbilityNode;
class AbilityProcessor;
class EffectProcessor;

typedef std::vector<struct EffectNode*, STLAllocator<struct EffectNode*, UE3STLAllocatorPolicy> > STLEffectNodeVector;
typedef std::vector<struct CastNode*, STLAllocator<struct CastNode*, UE3STLAllocatorPolicy> > STLCastNodeVector;

typedef std::map<INT, BaseComponent*, std::less<INT>, STLAllocator< std::pair<INT, BaseComponent*>, UE3STLAllocatorPolicy> > STLInt2BaseComponentMap;
typedef std::map<LONG64, AbilityNode*, std::less<LONG64>, STLAllocator< std::pair<LONG64, AbilityNode*>, UE3STLAllocatorPolicy> > STLLong64ToAbilityNodeMap;


enum EEffectCreatorType
{
	ECT_Unknown,
	ECT_Ability,
	ECT_Buff,
	ECT_Missle,
	ECT_Action,
};


struct EffectNode
{
	UINT CasterID;
	UINT TargetID;
	UINT BaseActorId;
	UINT AbilityID;
	UINT AbilityLevel;
	UINT AbilityRune;
	UINT SerialId;
	INT CastId;
	INT Index;
	UINT Flag;
	INT Param1;
	INT Param2;
	AbilityNode* BaseAbilityNode;
};

struct InitData
{
	UAbilityComponent* ComponentData;
	INT CreatorId;
	INT BaseActorId;
	INT TargetId;
	FVector TargetPoint;
	FLOAT Rate;
	FLOAT MaxDistance;
	FLOAT StartTime;
	InitData()
	{
		CreatorId = 0;
		BaseActorId = 0;
		TargetId = 0;
		TargetPoint = FVector(0, 0, 0);
		Rate = 1.0;
		MaxDistance = 800.f;
		StartTime = 0.f;
		ComponentData = NULL;
	}
};


struct CastNode
{
	UINT				CastId;
	STLEffectNodeVector	EffectList;
};

struct HitNode
{
	INT Id;
	UBOOL bActive;
	UAbilityHitPointComponent* HitPointData;
	STLEffectNodeVector EffectList;
	HitNode()
	{
		Id = -2;
		bActive = FALSE;
		HitPointData = NULL;
	}
};

enum EComponentType
{
	ECPT_Anim,
	ECPT_ActionProc,
	ECPT_Sound,
	ECPT_Emitter,
	ECPT_SkeletalMesh,
	ECPT_StaticMesh,
	ECPT_Decal,
	ECPT_CameraAnim,
	ECPT_MaterialEffect,
	ECPT_WindSource,
	ECPT_TimeFactor,
	ECPT_PhysicHit,
	ECPT_HitPoint,
	ECPT_Trail,
	ECPT_MoveControl,
	ECPT_Throw,
	ECPT_PostProcess,
	ECPT_Actions,
	ECPT_Visible,
	ECPT_PhysicalSimulate,
	ECPT_Scale,
	ECPT_Mirror,
	ECPT_MeshChange,
	ECPT_PositionTransform,
	ECPT_BackgroundMusic,
};

struct BaseComponent
{
	INT ComponentId;
	INT ComponentType;
	UAbilityComponent*	ComponentData;	

	UBOOL				bStopped;
	INT					CreatorId;
	INT					BaseActorId;
	INT					TargetId;
	FVector				TargetPoint;
	FLOAT Rate;
	FLOAT MaxDistance;
	UBOOL bClearWhenOwerRevive;

	INT WeaponHitType;

	UBOOL bUseEngineObjectId;

	UBOOL bAddedToRoot;

	// 
	BYTE StopType;
	FLOAT CurrentTime;
	FLOAT Duration;

	BaseComponent()
	{
		bStopped = TRUE;
		StopType = 0;
		BaseActorId = 0;
		TargetId = 0;
		CurrentTime = 0.f;
		MaxDistance = 100;
		Rate = 1.0;
		WeaponHitType = -1;
		Duration = 1.0f;
		bUseEngineObjectId = FALSE;
		bAddedToRoot = FALSE;
		bClearWhenOwerRevive = TRUE;
	}

	INT CreateNewId();

	INT GetId()
	{
		return ComponentId;
	}

	INT GetType()
	{
		return ComponentType;
	}

	UBOOL IsAddedToRoot()
	{
		return bAddedToRoot;
	}

	void SetWeaponHitType(BYTE InHitType)
	{
		WeaponHitType = InHitType;
	}

	void SetUseEngineObjectId(UBOOL InUseEngineObjectId)
	{
		bUseEngineObjectId = InUseEngineObjectId;
	}

	AActor* GetIndexedObject(INT InId);

	// for some other
	virtual UBOOL Init(InitData& InData);

	virtual void Tick(FLOAT DeltaTime);
	inline EAbilityComponentStopType GetStopType()
	{
		return EAbilityComponentStopType(ComponentData->StopType);
	}
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual	void ResetLifeSpan(FLOAT InCurrentTime, FLOAT InDuration);

	virtual void SetRate(FLOAT NewRate)	{ Rate = NewRate; }

	virtual AActor* GetBaseActor();
	virtual AActor* GetTargetActor();
	virtual AMMOActor* GetBaseCharacter();
	virtual AMMOActor* GetTargetCharacter();
	virtual AActor* GetCreatorActor();
	virtual AMMOActor* GetCreatorCharacter();
};

struct AnimComponent : public BaseComponent
{
	UAbilityAnimationComponent* AnimData;
	// dy
	INT AnimIdx;
	FLOAT AnimLastTime;
	INT AnimKey;

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
	virtual void SetRate(FLOAT NewRate);
};

struct ActionProcComponent : public BaseComponent
{
	UAbilityActionProcComponent* ActionProcData;
	// dy
	INT AnimIdx;
	FLOAT AnimLastTime;

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);

};

struct SoundComponent : public BaseComponent
{
	UAbilitySoundCueComponent* SoundData;

	INT SoundCueComponentIdx;
	TArray<INT> SoundCueComponentIdxs;

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct EmitterComponent : public BaseComponent
{
	UAbilityEmitterComponent* EmitterData;

	// dy
	INT ProjecitleId;
	FLOAT StartTime;
	INT ProActorId;
	INT BeamTargetActorId;
	BOOL Visible;

	EmitterComponent()
	{
		ProjecitleId = 0;
		ProActorId = 0;
		BeamTargetActorId = 0;
		StartTime = 0.f;
		Visible = TRUE;
	}

	virtual AActor* GetBaseActor();
	virtual AActor* GetTargetActor();

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
	virtual	void ResetLifeSpan(FLOAT InCurrentTime, FLOAT InDuration);

};

struct SkeletalMeshComponent : public BaseComponent
{
	UAbilitySkeletalMeshComponent* MeshData;

	//dy
	INT ActorIdx;

	virtual AActor* GetBaseActor();
	virtual AActor* GetTargetActor();

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct StaticMeshComponent : public BaseComponent
{
	UAbilityStaticMeshComponent* MeshData;

	//dy
	INT ActorIdx;

	virtual AActor* GetBaseActor();
	virtual AActor* GetTargetActor();

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};

struct DecalComponent : public BaseComponent
{
	UAbilityDecalComponent* DecalData;

	//dy
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};

struct CameraAnimComponent : public BaseComponent
{
	UCameraShakeComponent* CameraAnimData;

	UBOOL bPlayed;

	//dy
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};


struct MaterialEffectComponent : public BaseComponent
{
	// static data
	UAbilityComp_MaterialEffect* MaterialEffectData;

	// dynamic data
	INT EffectId;
	UMaterialInstanceTimeVaryingEffectNode* InData;
	inline INT GetNewId()	{ static INT i = 0; return ++i; }

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct WindSourceComponent : public BaseComponent
{
	// static data
	UAbilityWindSourceComponent* WindSourceData;

	// dynamic Data

	INT ActorIdx;
	UWindPointSourceComponent* WindPointSourceComponent;

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct TimeFactorComponent : public BaseComponent
{
	// static data
	UAbilityTimeFactorComponent* TimeFactorData;

	// dynamic data
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct PhysicHitComponent : public BaseComponent
{
	// static data
	UAbilityHitEffectComponent* PhysicHitData;

	// dynamic data
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};

struct HitPointComponent : public BaseComponent
{
	// static data
	UAbilityHitPointComponent* HitPointData;

	// dynamic data
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};


struct TrailComponent : public BaseComponent
{
	UAbilityTrailComponent* TrailData;

	// danamic data
	UAnimNotify_Trails* TrailNotify;
	UAnimNodeSequence* Seq;

#if SHOW_TRAIL_LOG
	FVector SrcPoint1, SrcPoint2, SrcPoint3;
	UBOOL bFirstTime;
#endif


	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct MoveControlComponent : public BaseComponent
{
	UAbilityMoveControlComponent* MoveControlData;

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct ThrowComponent : public BaseComponent
{
	UAbilityThrowComponent* ThrowData;

	FVector LastVelocity;

	UBOOL bSuccessed;

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct PostProcessComponent : public BaseComponent
{
	UAbilityPostProcessComponent* PostProcessData;
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};

struct ActionsComponent : public BaseComponent
{
	UAbilityActionsComponent* ActionsData;

	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct VisibleComponent : public BaseComponent
{
	UAbilityVisibleComponent* VisibleData;
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};

struct PhysicalSimulateComponent : public BaseComponent
{
	UAbilityPhysicSimulateComponent* PhysicSimulateData;
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};

struct ScaleComponent : public BaseComponent
{
	UAbilityScaleComponent* ScaleData;
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};

struct MirrorComponent : public BaseComponent
{
	UAbilityMirrorComponent* MirrorData;
	FLOAT NextCreateTime;
	std::list<INT> MirrorIdList;
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
	virtual void Tick(FLOAT DeltaTime);
};

struct MeshChangeComponent : public BaseComponent
{
	UAbilityMeshChangeComponent* MeshChangeData;
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};

struct PositionTransformComponent : public BaseComponent
{
	UPositionTransformComponent* PositionTransformData;
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};


struct BackgroundMusicComponent : public BaseComponent
{
	UAbilityBackgroundMusicComponent* BackgroundMusicData;
	virtual UBOOL Init(InitData& InData);
	virtual void OnAction(UBOOL bOn = TRUE);
};




class AbilityNode
{
private:
	AbilityProcessor* Processor;

public:
	UBOOL				bClientAbility;
	INT					SelfID;
	INT					TargetID;
	FVector				TargetPoint;
	EAbilityState		State;
	FLOAT				RequestCurrentTime;
	FLOAT				SpellCurrentTime;
	FLOAT				CastCurrentTime;
	FLOAT				AbilityCurrentTime;
	FLOAT				SpeedScale;
	FLOAT				TimeScaleByWeaponType;
	FLOAT				Rate;
	INT					Serial;
	STLCastNodeVector	CastList;
	FLOAT				OverrideSpellTime;
	FLOAT				OverrideCastTime;
	FName				LoopAnimToStop;
	UBOOL				bInterrupted;
	UBOOL				bLeadAbilityOver;
	LONG64				AbilityKey;
	// static data
	UMMOAbility*		AbilityData;

	// fast query data. from static data
	FLOAT				HitPointTime;

	INT					ComponentPairIndex;

	BYTE			WeaponType;

	TArray<FComponentNode>* SpellEffectList;
	TArray<FComponentNode>* CastEffectList;

	HitNode*		PrepareEffectList[HIT_POINT_MAX_NUMBER];
	



	/// new property
	STLIntVector	SpellComponentIds;
	STLIntVector	CastComponentIds;
	STLIntVector	AbilityComponentIds;

	STLIntVector AutoDestroyComponentIds;

	inline void SetProcessor(AbilityProcessor* InProcessor){ Processor = InProcessor; }

	void ClearSpellComponents();
	void ClearCastComponents();
	void ClearAbilityComponents();

	void DeleteAllEffectData();

	void RefreshAutoDestroyComponentsRate();

	FLOAT NormalizeTime(FLOAT InLastTime);

	void AddEffectNode(EffectNode* NewNode);

	void PushHitEffect(INT Idx);

	void PushAllHitEffect();

	void GenerateHitPointInfos(BYTE InType);


	AbilityNode()
	{
		Processor = NULL;
		RequestCurrentTime = 0.f;
		SpellCurrentTime = 0.f;
		CastCurrentTime = 0.f;
		AbilityCurrentTime = 0.f;
		SpeedScale = 1.0f;
		Rate = 1.f;
		HitPointTime = 0.0f;
		bInterrupted = FALSE;
		LoopAnimToStop = NAME_None;
		bLeadAbilityOver = FALSE;
		SpellEffectList = NULL;
		CastEffectList = NULL;
		bClientAbility = FALSE;
		SelfID = 0;
		TargetID = 0;
		State = EAS_End;
		TimeScaleByWeaponType = 1.0;
		Serial = 0;
		OverrideCastTime = 0.f;
		OverrideSpellTime = 0.f;
		AbilityKey = 0;
		AbilityData = NULL;
		ComponentPairIndex = 0;
		WeaponType = 0;
	}
};

struct IdNode
{
	STLIntVector Ids;
};

typedef std::vector<IdNode, STLAllocator<IdNode, UE3STLAllocatorPolicy> > STLIdNodeVector;


struct BuffData
{
	INT Id;
	INT CreatorId;
	INT ObjId;
	INT Level;
	UBuff* Buff;
	FString BuffName;
	FString BuffColor;
	FString DisplayIconName;
	FString BuffDescription;

	STLIntVector SingleEffectComponentIds;
	STLIdNodeVector OverlapEffectComponentIds;
	INT StackCount;
	FLOAT Duration;
	FLOAT CurrentTime;
	UBOOL bAutoClose;

	UBOOL bHasComponentNotApply;

	BuffData()
	{
		Id = 0;
		CreatorId =0;
		ObjId = 0;
		Level = 1;
		Buff = NULL;
		StackCount = 0;
		Duration = 0.f;
		CurrentTime = 0.f;
		bAutoClose = FALSE;
		BuffColor = "";
		bHasComponentNotApply = FALSE;
	}
};

class EffectProcessor
{
	// Entity projectile managed by the server
	STLInt2IntMap	ServerProjectiles;

	// Pure client-side particles
	STLInt2IntMap	ClientProjectiles;

	STLInt2BaseComponentMap ActiveComponents;

	// buff
	//STLInt2BuffDataMap	ActiveBuffs;

	INT CurrentEmitterNumber;
	INT MaxEmitterNumber;

public:
	EffectProcessor();
	static EffectProcessor& GetSingleton()
	{
		static EffectProcessor Node;
		return Node;
	}

	void Tick(FLOAT DeltaTime);

	INT GetProjectileId();

	INT CreateEffectId();

	void AddActiveComponent(BaseComponent* InBaseComponent);

	BaseComponent* GetActiveComponent(INT Id);

	void DeleteActiveComponent(INT Id);

	void DeleteAllComponent(UBOOL bButMainPlayer = FALSE);

	void AddServerProjectile(INT ObjId, AMMOEmitter* ProActor);

	void DeleteServerProjectile(INT ObjId);

	void ClearAllComponentByBaseActorId(INT ObjId);
	void ClearAllComponentByBaseActorIdWhenRevive(INT ObjId);

	INT AddClientProjectile(AMMOEmitter* ProActor);

	void DeleteClientProjectile(INT ClientId);

	void SetMaxEmitterNumber(INT InMax);
	inline INT GetCurrentEmitterNumber() { return CurrentEmitterNumber; }
	void ClearCurrentEmitterNumber();
	void AddCurrentEmitterNumber();
	UBOOL CanSpawnEmitter();

	void SetClientProjectileVisible(INT InBaseId, UBOOL bVisible);
};

class AbilityProcessor
{
protected:
	STLLong64ToAbilityNodeMap AbilityList;

	STLInt2IntMap BuffMap;

public:
	static UINT CombinateID(UINT AbilityID, UINT LevelID,UINT RuneID);

	static EDamageResultType FlagConvertToClientType(UINT Flag);


protected:

	inline LONG64 CombinateID64(UINT AbilityID, UINT ObjID, UINT SerialID)
	{
		LONG64 ret = AbilityID;
		ret = ret<<32;
		ret += ObjID;
		ret = ret<<16;
		ret += SerialID;
		return ret;
	}

public:
	void Tick(FLOAT DeltaTime);
	void PushEffect(EffectNode* node);
	void PushEffect(LONG64 Key);

	UBOOL LineCheck(AActor* SourceActor, FVector& StartPoint, FVector& EndPoint, FVector& HitPoint, FVector& HitNormal, FVector TraceExtned = FVector(0, 0, 0));

	void PushBullet(UMMOAbility* AbilityData, UAbilityHitPointComponent* HitData, INT CreatorId,  AActor* BaseActor, AActor* TargetActor, FVector& TargetPoint, FLOAT Duration, FLOAT InStartTime = 0.f);



	void UpdateAbility(AbilityNode* node, FLOAT DeltaTime);
	void SetAbilityAnimInfo(AbilityNode* node, UMMOAbility* abilityData);
	void DeteleAbilityNode(LONG64 key);

	void ClearAllAbilityNode();

	UMMOAbility* GetAbilityData(UINT Id);

	AbilityNode* GetAbilityNode(LONG64 Key);

	static BaseComponent* CreateDynamicComponent(UAbilityComponent* InComponentNode, UBOOL bInHitPoint,INT InCreatorId, INT InBaseActorId, INT InTargetId, FVector& InTargetPoint, FLOAT InRate, FLOAT InMaxDistance = 800, FLOAT InStartTime = 0.f);

	void 
		CreateDynamicComponent(AbilityNode* InAbilityNode, UAbilityComponent* InComponentNode, EAbilityState ArrayState, FLOAT InStartTime = 0.f);

	void OnRecvSelectTarget(UINT CasterID, UINT AbilityId, UINT AbilityLevel, UINT RuneId);

	void OnRecvPrepareSpell(
		UINT CasterID = 0, 
		UINT AbilityID = 0, 
		UINT AbilityLevel = 0, 
		UINT AbilityRune = 0, 
		UINT TargetID = 0, 
		FVector TargetPoint = FVector(0, 0, 0), 
		UINT BaseWeaponSpeedFactor = 0,
		UINT SpeedFactor = 0,
		UINT SerialId = 0
		);

	void PlayAbilityBeforeServer(UINT CasterID, UINT AbilityID, UINT AbilityLevel, UINT AbilityRune, UINT TargetID, FVector TargetPoint, UINT SerialID);

	void OnProcessTargetList(SPELL_CAST_INFO* InInfo, INT InSerial, INT TargetCount, TARGET_DATA* InData);

	void OnRecvCastSpell(G2C_CHAR_CAST_SPELL* InData);

	void OnRecvExCast(G2C_CHAR_EX_CAST_SPELL* InData);

	void OnRecvEffect(
		UINT CreatorId,
		UINT AbilityID, 
		UINT AbilityLevel, 
		UINT AbilityRune, 
		UINT TargetID,
		INT Param1, 
		INT Param2, 
		UINT Flag,
		UBOOL InMissileEffect,
		UINT SerialId,
		UINT CastId, 
		UINT EffectSerial
		);

	void OnRecvBreakCast(G2C_CHAR_BREAK_CAST* InData);

	void OnRecvError(INT ErrCode, INT Param1, INT Param2);

	void OnMainRoleEndAbility(INT AbilityId, INT SerialId);

	void OnMissileAppear(INT Id, INT InMissileID, int CreatorId, FLOAT InSpeed, FVector& Src, FRotator& InRot, FLOAT InLifeSpan);
	void OnMissileChangeMovement(INT Id, FVector& Dst, FRotator& InRot, FLOAT CostTime);


	//// buff
	//void AddNewBuff(INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, INT StackCount);
	//void ChangeBuff(INT ObjId, INT Id, FLOAT Duration, INT StackCount);
	//void DeleteBuff(INT Id);

	// cd
	void AddCoolDown(INT InType, INT32 InId, INT32 InValue);

	void PlayClientAbility(INT CasterId, UMMOAbility* ToUseAbility);
};



#endif