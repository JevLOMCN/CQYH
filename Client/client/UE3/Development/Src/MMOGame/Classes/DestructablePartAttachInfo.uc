//
// destructible part attachment info
//
class DestructablePartAttachInfo extends Object
	editinlinenew
	hidecategories(Object)
	native;

// destructible part destruction mode
enum EDestructResultType
{
	EDRT_Hide	    <DisplayName="Hide">,
	EDRT_Replace	<DisplayName="Replace">,
	EDRT_Drop	    <DisplayName="Drop">
};

// if it is a static model, attach it to the attach point, otherwise share animation with the main skeleton
var() bool IsStaticMeshComponent;

// only destructible objects create visuals, such as explosion effects, drops, etc.
var() bool IsDestructable;

var() EDestructResultType DestructType;

// only drops on animation notify
var() bool DropByNotify;

// destroyed when the character's HP is below this value
var() float ExplodeHPPercent;

// name used for identification
var() Name AttachName;

// Socket used to create effects or attach a static model
var() Name AttachSocket <DynamicList="SocketList">;

// static model used for attaching or dropping
var() StaticMesh MeshAsset;

// skeletal model used for combined attachment, changed by jimzhang 20122.7.14, use component instead of Skeletal, so SkeletalMesh's own animation can be used
//var()   SkeletalMesh SkelMeshAsset;
var()   SkeletalMeshComponent SkelMeshComponent;

// particle effect produced when equipment is destroyed
var() Emitter ExplodeEmitter;

// data config of the model displayed in place after destruction, different data can be specified recursively for multi-stage destruction
var() editinline DestructablePartAttachInfo ReplaceArmorTemplateOnDestruct;

// whether it is a weapon
var() bool IsWeapon;

//hidden while the character is running
var() bool HideWhenRun;

//var(MMOSFX) ParticleSystem BrokenEmitterTemplate <DisplayName="particle effect on destruction">;;

var public StaticMeshComponent MeshComponent;
//var public SkeletalMeshComponent SkelMeshComponent;

var MMOActorMaterialController RootMaterialController;

var MMOActorAT ActorAT;

function Init()
{
	RootMaterialController = new(self) class'MMOActorMaterialController';
	if(SkelMeshComponent != none)
	{
		RootMaterialController.Init( SkelMeshComponent );
	}
	else if( MeshComponent != none)
	{
		RootMaterialController.Init( MeshComponent );
	}

	`log("_____________ DestructablePartAttachInfo Init, Sk:" @ SkelMeshComponent @ MeshComponent);
}

function AttachActor(MMOActor actor)
{
	if(SkelMeshComponent==none && MeshComponent==none)
	{
		return;
	}
	if(ActorAT ==none ){
		ActorAT = new class'MMOActorAT';
	}
	ActorAT.InitAnim(SkelMeshComponent,actor);
}

DefaultProperties
{
	ActorAT = none;
	IsDestructable=true
	IsStaticMeshComponent=true
	DropByNotify=true
	IsWeapon=false
}
