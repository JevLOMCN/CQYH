class AbilityComponent extends Object
	dependson(TypeDefine)
	native;

// component end mode
var(LifeSpan) EAbilityComponentStopType StopType;
// component lifetime duration
var(LifeSpan) float Duration;

var(LifeSpan) bool bClearWhenRevive;

var(ValidRadius) float MinRadius;
var(ValidRadius) float MaxRadius;

// whether this component is attached under the hitpoint component
var transient bool bInHitPoint;

var bool bAddToRoot;

//|<- for editor
var LinearColor LineColor;
var string TitleString;
var bool bHaveTrail;

cpptext
{
	virtual FString GetResName();
	virtual FLOAT GetResLength(USkeletalMeshComponent* InSM);
	virtual void SetResLength(USkeletalMeshComponent* InSM, FLOAT InTime);
	virtual struct BaseComponent* CreateCplusplusNode();
	virtual void CopyObjectProperties(UAbilityComponent* DstComponent){}

	virtual UBOOL IsEffectValid();

	FMatrix GetSpawnMatrix(USkeletalMeshComponent* InBaseComponent, USkeletalMeshComponent* InWeaponComponent1, USkeletalMeshComponent* InWeaponComponent2, FVector InTargetPoint, FName InSocketName, BYTE InSourceType, FVector InRelativeLocation, FRotator InRelativeRotation, INT bLocal = 0);
}
/// for editor ->|

///
event Play(Actor BaseActor, MMOAbility AbilityData, Actor Target, Vector TargetPoint, float InRate, optional int SerialId = 0);
event Stop();


defaultproperties
{
	Duration = 1.5
	LineColor=(R=1.0,G=1.0,B=1.0,A=1.0)
	TitleString="AbilityComponent"
	bHaveTrail=true
	bAddToRoot=false
	MinRadius=1000
	MaxRadius=8000
	bClearWhenRevive=true
}
