class MMOEmitter extends Emitter
	native;




// component end mode
var EAbilityComponentStopType StopType;
// component lifetime duration
var float Duration;

// Solid bullets have an ID on the server side
var int ServerID;
var Missile MissileData;

var int ClientId;
var AbilityEmitterComponent EmitterData;

var Matrix SpawnMatrix;
var Matrix BaseActorMatrix;

var float CurrentTime;
var float DelayToKill;

var float MaxDistance;
var float MinDistance;
var float DistanceToLand;


var MMOEmitter BeamTargetActor;

var EProjectileTargetType OverwriteTargetType; 

var EEmitterType    OverwriteEmitterType;

//var Vector OverwriteBezierControl1;
//var Vector OverwriteBezierControl2;

var TwoVectors OverwriteBezierControl;

var name OverwriteSocketName;

var float Speed;
var Vector MissileDst;


// light
var PointLightComponent MyPointLightComponent;
var float LightLifeSpan;
var float LightFadeUpTime;
var float LightFadeOutTime;

var float OriginalBrightness;
var float FadeSpeed;

var  Actor Creator;
var  Actor Target;   // may be none
var Actor PreviousTarget;  // to be deleted
var  Vector TargetPoint;
var float Rate;

var bool bToAutoDestroy;

var bool bRun;

var bool bSonProjectile;

var DynamicNavMeshObstacle DynamicObstacle;

var transient array<int> ToKillEffectsWhenEnd;

final native function SelfDestroyed();

cpptext
{
	virtual void TickSpecial( FLOAT DeltaSeconds );
}

event Destroyed()
{
	super.Destroyed();
	if(ParticleSystemComponent != none)
	{
		ParticleSystemComponent.DeactivateSystem();
	}
	Creator = none;
	Target = none;
	SelfDestroyed();
}

native final function BuildNavMeshObstacle(Missile InMissile);

native final function SetupCollision(Missile InMissileData);

native final function ShowMissileCollision();

native final function BeginPlay(Actor BaseActor, MMOAbility InAbilityData, Actor InTarget, Vector InTargetPoint, float InRate, optional int SerialId = 0);

event SetupTestCollision(Missile InMissileData)
{
	local StaticMesh CollisionMesh;
	local DynamicSMActor_Spawnable NewActor;
	local Vector NewLoc;

	if (InMissileData == none)
	{
		return;
	}

	//StaticMesh'cp_pbd.STM.S_cp_pbd01_34'

	CollisionMesh = StaticMesh(DynamicLoadObject("EngineMeshes.Cube", class'StaticMesh'));
	if (CollisionMesh == none)
	{
		`log("Can not load engine default mesh EngineMeshes.Cube, so can not setup collision component for the missile");
		return;
	}

	NewLoc = Location;
	NewLoc.Z +=100;
	NewActor = Spawn(class'DynamicSMActor_Spawnable', self, , NewLoc, Rotation);
	if (NewActor == none)
	{
		`log("spawn filed");
		return;
	}
	NewActor.SetCollisionType(COLLIDE_BlockAll);
	NewActor.StaticMeshComponent.SetStaticMesh(CollisionMesh);
}

native function Kill();

//event Kill(optional float NewLife = 0.1)
//{
//	local float maxlife;
//	bToAutoDestroy = true;
//	bDestroyOnSystemFinish = true;
//	maxlife = ParticleSystemComponent.GetMaxLifespanWithoutLoop();
//	ParticleSystemComponent.SetStopSpawning(-1, true);
//	if(maxlife <= 0.001)
//	{
//		maxlife = 0.001;
//	}
	
//	//
//	if(maxlife > 0.1)
//	{
//		maxlife = 0.1;
//	}

//	if(NewLife > 0.1)
//	{
//		NewLife = 0.1;
//	}

	
//	LifeSpan = maxlife;

//	//`log(ParticleSystemComponent.Template$" Kill LiftSpan="$LifeSpan);
//	if(MyPointLightComponent != none)
//	{
//		if(LightFadeOutTime > LifeSpan)
//		{
//			LightFadeOutTime = LifeSpan;
//		}
//		if(LightFadeOutTime < 0.01)
//		{
//			LightFadeOutTime = 0.01f;
//		}
//		FadeSpeed = -OriginalBrightness/LightFadeOutTime;
//	}

//	if(BeamTargetActor != none)
//	{
//		BeamTargetActor.LifeSpan = LifeSpan;
//	}

//	if(DynamicObstacle != none)
//	{
//		DynamicObstacle.UnRegisterObstacle();
//		DynamicObstacle.Destroy();
//	}
//}


// reset instances, currently for their old locations only
native final function ResetInstanceLocations( vector loc );

DefaultProperties
{
	LifeSpan=0
	DelayToKill=0.1
	ServerID=-1
	Physics=PHYS_None
	bRun=false
	bNoDelete=false
	bDestroyOnSystemFinish=true
	bNetTemporary=true

	bCollideActors=false
	bCollideWorld=false
	bBlockActors=false
	DistanceToLand=-1.0
}
