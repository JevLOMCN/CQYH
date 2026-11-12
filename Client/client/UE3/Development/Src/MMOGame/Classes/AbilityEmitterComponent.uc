class AbilityEmitterComponent extends AbilityComponent
	hidecategories(Object)
	dependson(TypeDefine)
	editinlinenew
	native;


var(Common) EEmitterType     EmitterType;

// effect template
var(Common) ParticleSystem ParticleSystemTemplate;

var(Common) array<ParticleSystem> RandParticleSystemTemplates;

var(Common) array<ParticlesByElementType> ParticlesByElementTypeDataList;

var(Common) bool bImportant;

struct native TwoVector
{
	var() Vector v1;
	var() Vector v2;
	structdefaultproperties
	{
		v1=(X=-1, Y=0, Z=1000)
		v2=(X=1, Y=0, Z=0)
	}
};

var(Common) ESourceType SourceType;
var(Common) bool bAttach;
var(Common) bool    bLittleYawRand;
var(Common) bool bUseBaseRotation;


// bind point for playing the hit effect, use the character's position if none
var(Common) name AttachSocketName <DynamicList="SocketList">;

var(Common) float Scale;
var(Common) bool bScaleByStandardCollision;
var(Common) bool bUseSocketRotation;

var(Common) Vector RelativeLocation;
var(Common) Rotator RelativeRotation;

// lightning or fireball effect, target point type
var(Common) EProjectileTargetType ProjectileTargetType; 
var(Common) editinline array<AbilityComponent> EmitterEndEffectList;

var(Normal) bool bDynamicFindTheNearestSocket;

var(FlyTo) ETrackLineType   TrackLineType;
var(FlyTo) ETrackTimeType   TrackTimeType;
var(FlyTo) int LoopCount;
//const speed
var(FlyTo) float Speed;
// const time
var(FlyTo) float FlyTime;

//parabola upward deflection angle [0,90)
var(FlyTo) float MaxHeight;
//parabola orientation-correction time ratio (0,1)
var(FlyTo) float TimeToFinishFixRotation<ClampMin = 0.0 | ClampMax = 1.0>;
// flight direction when there is no target [-360, 360]
var(FlyTo) Rotator FlyDirection;
var(FlyTo) Rotator DirectionDelta;

// bezier properties
//var(FlyTo) Vector BezierControlPoint1;
//var(FlyTo) Vector BezierControlPoint2;
var(FlyTo) array<TwoVector> BezierControlPointList;

// orbit velocity direction, only valid for orbit type
var(RotateProjectile) Rotator FirstSpawnDirection;
var(RotateProjectile) Rotator RotateVelocity;
// orbit radius, only valid for orbit type
var(RotateProjectile) float RotateRadius;

/** Light from the Emitter */
var(Light) editinline PointLightComponent MyPointLightComponent;
/** How long the light should stay in existance, if enabled. 
 *  Zero keeps it active for the entire lifetime of the emitter. */
var(Light) float LightLifeSpan;
/** How long after spawning the Light should take to interpolate up to full brightness. */
var(Light) float LightFadeUpTime;
/** How long BEFORE lifespan expires the Light should take to interpolate down to zero brightness. */
var(Light) float LightFadeOutTime;

// sound
var(Sound) SoundCue SoundCueToPlay;
var(Sound) array<SoundCueByElementaryType> SoundCueToPlayByElementaryType;

var(Beam) ParticleSystem BeamTargetParticleSystemTemplate;
var(Beam) SoundCue BeamTargetSoundCue;
var(Beam) float BeamTargetScale;
var(Beam) Material BeamTargetDecl;
var(Beam) Vector TraceExtend;
var(Common) bool bHaveCollisionCheck;
var(Common) name TargetSocketName <DynamicList="SocketList">;



cpptext
{
	virtual FString GetResName();
	virtual FLOAT GetResLength(USkeletalMeshComponent* InSM);
	virtual struct BaseComponent* CreateCplusplusNode();

	FMatrix GetSocketMatrix(USkeletalMeshComponent* InComponent, FName InSocketName, UBOOL bUseRelativePostion = TRUE);
	FMatrix GetSocketMatrix(AActor* InActor, FVector& InTargetPoint, UBOOL bTarget, FName OverwriteSocketName = NAME_None);
	//UBOOL GetNearestSocketWorldMatrixToTarget(USkeletalMeshComponent* InBaseComponent, AActor* InTarget, FName& OutName, FMatrix& OutMat);
	FName FindTheNearestSocketToTarget(AActor* InBaseActor, AActor* InTargetActor, FName InSocketName);
	// will replace the first two GetSocketMatrix functions
	FMatrix GetSocketMatrix(USkeletalMeshComponent* InBaseComponent, USkeletalMeshComponent* InWeaponComponent1, USkeletalMeshComponent* InWeaponComponent2, FVector InTargetPoint, UBOOL bTarget, FName OverwriteSocketName = NAME_None);
	FLOAT GetLifeSpan(FVector InRelativeTargetPoint);
	UBOOL GetBezierControlPoint(FVector& OutV1, FVector& OutV2);

	UParticleSystem* GetParticleSystemByElementaryType(EElementaryType InType = EETT_Elementary0);
	USoundCue* GetSoundCueByElementaryType(EElementaryType InType = EETT_Elementary0);
}



DefaultProperties
{
	Scale=1.0f
	RotateRadius=80
	FirstSpawnDirection=(Pitch=0,Yaw=16384,Roll=0)
	RotateVelocity=(Pitch=16384,Yaw=16384,Roll=0)
	LightLifeSpan=1.0
	LightFadeUpTime=0.1
	LightFadeOutTime=0.1
	LineColor=(R=0.7098,G=0.90196,B=0.1137,A=1.0)
	TitleString="Emitter"
	bScaleByStandardCollision=false
	bLittleYawRand=false
	bUseSocketRotation=true
	BeamTargetScale=1.0
	MaxHeight=50.0
	Speed=100.f
	FlyTime=3.0f
	TimeToFinishFixRotation=1.0
	LoopCount=1
	bDynamicFindTheNearestSocket=false
	//BezierControlPoint1=(X=-1, Y=0, Z=1000)
	//BezierControlPoint2=(X=1, Y=0, Z=0)
	TraceExtend=(X=30, Y=0, Z=30)
	bImportant=false
	bUseBaseRotation=true
}
