/*
 * model definition
 * 
 */

class AvatarData extends Object
	native
	hidecategories(Object);


struct native MeshList
{
	var() EBodyMeshType     MeshType; 
	var() array<EquipMesh> EquipMeshList;
};

var() string                AvatarName					<DisplayName="AvatarName">;
var() SkeletalMesh  SkeletalMesh						<DisplayName="SkeletalMesh">;
var() array<MaterialInterface>	Materials				<DisplayName="Materials">;
var() AnimTree      AnimationTree						<DisplayName="AnimationTree">;
var() editconst PhysicsAsset  PhysicAsset				<DisplayName="PhysicAsset">;
var() EBodyType     PhysicsBodyType						<DisplayName="PhysicsBodyType">;
var() array<WeaponTypeAnimSetPair> WeaponAnimSetPairs	<DisplayName="WeaponAnimSetPairs">;

var() array<EquipMesh>	DefaultMeshListTemplate			<DisplayName="DefaultMeshListTemplate">;

var() array<MeshList>   RandomMeshListTemplate          <DisplayName="RandomMeshListTemplate">; 
var() float AvatarDrawScale								<DisplayName="AvatarDrawScale"|ClampMin=0.1 | ClampMax=4.0>;
var() float HintNameOffsetZ                             <DisplayName="NameOffsetZ">;
var() float MeshScale;

// TRUE: merge shoulder armor FALSE: merge face
var()	bool			MergeShoulder					<DisplayName="MergeShoulder">;

// TRUE: use the merged model instead of outfit swapping. The new design removes body part swapping, only helmet (hair), face and body can be swapped
var()	bool			UseMergedMesh					<DisplayName="UseMergedMesh">;

// still use the old DefaultMeshList, otherwise maintaining two sets of code at once is too error-prone
//// default body model (shown when wearing no equipment)
//var()	SkeletalMesh	DefaultBodyMesh					<DisplayName="DefaultBodyMesh">;
//
//// optional hairstyles and face shapes, used under the new outfit swap mechanism
//var()	array<SkeletalMesh>	HairList					<DisplayName="HairList">;
//var()	array<SkeletalMesh>	FaceList					<DisplayName="FaceList">;

//only for sound effect
var(SoundFX) EBodyType BodyType							<DisplayName="BodyType">;
var(SoundFX) EItemMaterialType     TargetArmorMatType   <DisplayName="TargetArmorMatType">;
var(SoundFX) array<SoundCue> Sound_EnterBattle <DisplayName="Sound_EnterBattle">;

var(SoundFX) array<SoundCue> Sound_OpenDiag <DisplayName="Sound_OpenDiag">;
var(SoundFX) array<SoundCue> Sound_CloseDiag <DisplayName="Sound_CloseDiag">;
var(SoundFX) array<SoundCue> Sound_Spawn <DisplayName="Sound_Spawn">;
var(SoundFX) array<SoundCue> Sound_Talk <DisplayName="Sound_Talk">;
var(SoundFX) array<SoundCue> Sound_Idle <DisplayName="Sound_Idle">;
var(SoundFX) SoundCue QuestSoundAccept <DisplayName="QuestSoundAccept">;
var(SoundFX) SoundCue QuestSoundUnderway <DisplayName="QuestSoundUnderway">;
var(SoundFX) SoundCue QuestSoundSubmit <DisplayName="QuestSoundSubmit">;

var(Collision) float	CollisionHeight					<DisplayName="CollisionHeight">;	
var(Collision) float	CollisionRadius					<DisplayName="CollisionRadius">;

var(Movement) float RotatoDegreePerSecond;
var(Movement) float ToggleMoveAnimCriticalValue         <ClampMin=0.0 | ClampMax=1.0>;

var() float DefaultMoveSpeed                            <DisplayName="DefaultMoveSpeed">;

// default 0, calculated from the original resource; during the transition period, if there is no calculable normal resource, use DefaultMoveSpeed
var float WalkSpeed;
var float RunSpeed;
var float FlySpeed;

//var bool bLoadedPhysicAsset;


// initial list of destructible parts
var(MMOAttach) editinline array<DestructablePartAttachInfo> InitAttachDestructableParts <DisplayName="InitAttachDestructableParts">;
// currently attached particle system
//var(MMOAttach) array<ParticleSystemAttachInfo> InitAttachedParSys						<DisplayName="InitAttachedParSys">;
// changed to a struct list for easier designer configuration
struct native ParticleAttachInfo
{	
	// name used for identification
	//var() Name AttachName <DisplayName="AttachName">;

	// used to create the particle effect and attach it
	var() Name AttachSocket <DisplayName="AttachSocket"|DynamicList="SocketList">;

	// particle effect template
	var() ParticleSystem EmitterTemplate <DisplayName="EmitterTemplate">;

	// scale
	var() float		Scale <DisplayName="Scale">;

	structdefaultproperties
	{
		Scale=1.0
	}
};
var(MMOAttach) editinline array<ParticleAttachInfo> AttachedParticles		<DisplayName="AttachedParticles">;

var(MMOStatusProc) editinline array<IMMOStatusProc> AvatarSpawnProcList					<DisplayName="AvatarSpawnProcList">;

// the placement node tag for UI display
var(UI) name	PlacementNodeTag	<DisplayName="UINodeTag">;

struct native ExplosionData
{
	var() EElementaryType           Type;
	var() editinline array<AbilityComponent>   ExplosionComponents; 
};

var(Dead) array<ExplosionData>      OverrideExplosionEffects;

var(Lighting) bool	OverrideLightingChannels;
var(Lighting) LightingChannelContainer	LightingChannels;
var(Lighting) bool	UseSHLighting;

var() editconst AbilityAnimationComponent TurnLeftAction;
var() editconst AbilityAnimationComponent TurnRightAction;

//var(color) array<linearcolor>	SkinColorSet;
var(color) array<linearcolor>	HairColorSet;

var(color) transient editoronly int	TestSkinColorIdx;
var(color) transient editoronly int	TestHairColorIdx;

var(color) linearcolor	SkinSpecular;
var(color) linearcolor	SilhouetteColor;

// merge attributes, only for editor
var transient init array<Texture2D> SkinTextures;		// base textures of default meshes
var transient init array<Texture2D> SkinTexturesBase;
var transient MMOSkeletalMeshMerge	MergedMesh;		// use the merged mesh instead of separate meshes
var transient SkeletalMeshComponent	MergedComp;
var transient SkeletalMeshComponent BaseComponent;
var transient bool	bIsMerging;
var transient bool	bIsPreparing;

native final function float GetMoveSpeed(EActorMoveType InMoveType);
// if the return value <= 0.f, it means that there is some error, no need to toggle the move anim.
native final function float GetToggleMoveAnimCriticalSpeed();

native final function DynamicLoadPhysicAsset();

native function bool NeedToBeMerged();

event float GetWeightScale()
{
	switch(PhysicsBodyType)
	{
	case EBDT_Light:
		return 1.0f;
	case EBDT_Heavy:
		return 0.5f;
	case EBDT_SuperHeavy:
		return 0;
	default:
		return 0.75f;
	}
}

event string GetDetailedDescription( int InIndex )
{
	local string Description;
	switch( InIndex )
	{
	case 1:
		Description = AvatarName;
		break;
	default:
		break;
	}
	return( Description );
}
cpptext
{
	virtual FString GetDetailedDescription( INT InIndex )
	{
		return eventGetDetailedDescription(InIndex);
	}

	void RebulidMesh(USkeletalMeshComponent* BaseSMC, USkeletalMeshComponent* InMergedComponent);
	void TickMerge(FLOAT Delta);
	void PostRebuildMesh();
	UPhysicsAsset* LoadPhysicsAsset();
	
	static void ToogleBoneBreak(USkeletalMeshComponent* mesh, UBOOL breakWeight = TRUE);
}




// attach destructible part
function AttachDestructablePart(DestructablePartAttachInfo dpai , MMOActor actor)
{
	actor.AttachDestructablePart(dpai);
	dpai.AttachActor(actor);
}

// detach destructible part
function DetachDestructablePart(DestructablePartAttachInfo dpai , MMOActor actor)
{
	actor.DetachDestructablePart(dpai);
}

event OnExecute(MMOActor Actor)
{
	local DestructablePartAttachInfo asm;
	//local ParticleSystemAttachInfo info;
	local ParticleAttachInfo info;
	local ParticleSystemAttachInfo psai;
	local int num;
	local int i;


	for(i = 0; i < AvatarSpawnProcList.Length;++i)
	{
		AvatarSpawnProcList[i].OnExecute(Actor);
	}


	if(InitAttachDestructableParts.Length>0)
	{

		num = actor.AttachedDestructableParts.Length;

		foreach InitAttachDestructableParts(asm)
		{
			asm = new asm.Class ( asm );
			AttachDestructablePart(asm,actor);
		}

		for(i=0;i<num;++i)
		{
			DetachDestructablePart(actor.AttachedDestructableParts[0]   ,actor );
		}
	}

	//if(InitAttachedParSys.Length>0)
	//{
	//	while(actor.AttachedParSys.Length > 0)
	//	{
	//		actor.DetachParticleSystem(actor.AttachedParSys[0] );
	//	}

	//	foreach InitAttachedParSys(info)
	//	{
	//		actor.AttachParticleSystem(info);
	//	}			
	//	
	//}

	// fix bug #23180
	while (Actor.AttachedParSys.Length > 0)
	{
		Actor.DetachParticleSystem(Actor.AttachedParSys[0]);
	}

	foreach AttachedParticles( info )
	{
		//`Log( "********AttachParticle"@info.EmitterTemplate );
		psai = new class'ParticleSystemAttachInfo';
		psai.AttachSocket = info.AttachSocket;
		psai.EmitterTemplate = info.EmitterTemplate;
		psai.EffectScale = info.Scale;
		Actor.AttachParticleSystem(psai);
	}
}

native function EquipMesh GetDefaultEquipMeshByType(EBodyMeshType type);

defaultproperties
{
	AvatarDrawScale=1.0
	PlacementNodeTag=Place01
	PhysicsBodyType=EBDT_Middle
	DefaultMoveSpeed=580.0
	OverrideLightingChannels=false
	UseSHLighting=false
	WalkSpeed=-1.f
	RunSpeed=-1.f
	FlySpeed=-1.f
	MeshScale=1.f
	//bLoadedPhysicAsset=false
	RotatoDegreePerSecond=360.f
	ToggleMoveAnimCriticalValue=0.5
	HintNameOffsetZ=0.0

	Begin Object Class=AbilityAnimationComponent Name=TurnLeft
		bLoop=true
	End Object
	TurnLeftAction=TurnLeft;

	Begin Object Class=AbilityAnimationComponent Name=TurnRight
		bLoop=true
	End Object
	TurnRightAction=TurnRight;

	SkinSpecular=(R=0.478,G=0.437,B=0.432)
	SilhouetteColor=(R=0.32,G=0.32,B=0.30)

	UseMergedMesh=false
	MergeShoulder=false
}