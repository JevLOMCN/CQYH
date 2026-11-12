//
// Represented through skeletal animation
//
class MMOStatusProc_ExplodeSkelAnim extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() SkeletalMesh Mesh;
var() AnimSet AnimSet;
var() name AnimToPlay;

var() Vector2d PositionOffsetRange;

// If orientation is not random, it matches the character's orientation
var() bool EnableRandomRotation;

// Model lifetime (seconds)
var() float SpawnMeshLifeSpan;

// Whether to destroy after the animation finishes (if so, the SpawnMeshLifeSpan parameter is not used)
var() bool DestroyWhenAnimEnd;

var() PhysicsAsset PhysicsAsset;
var() AnimTree AnimTree;

event OnExecute(MMOActor actor)
{	
	local Vector spawnLocationt;
	local float ofx, ofy;
	local Rotator oyaw;
	local SkeletalMeshActorSpawnable sma;
	local AnimNodeSequence animNode;


	ofx = RandRange(-PositionOffsetRange.X, PositionOffsetRange.X);
	ofy = RandRange(-PositionOffsetRange.Y, PositionOffsetRange.Y);
	spawnLocationt = actor.Location;
	spawnLocationt.X += ofx;
	spawnLocationt.Y += ofy;
	
	if(EnableRandomRotation)
	{
		oyaw.Yaw = FRand() * 360.0 * DegToUnrRot;
	}
	else
	{
		oyaw.Yaw = actor.Rotation.Yaw;
	}

	sma = actor.Spawn(class'SkeletalMeshActorSpawnable',,, spawnLocationt, oyaw, , true);
	sma.SkeletalMeshComponent.AnimSets.AddItem(AnimSet);
	sma.SkeletalMeshComponent.SetSkeletalMesh(Mesh);
	sma.SkeletalMeshComponent.SetAnimTreeTemplate(AnimTree);
	
	if(PhysicsAsset != none)
	{
		sma.SkeletalMeshComponent.SetPhysicsAsset(PhysicsAsset, true);
	}

	//sma.PostBeginPlay();
	animNode = AnimNodeSequence(sma.SkeletalMeshComponent.Animations.FindAnimNode('Default'));	
	animNode.SetAnim(AnimToPlay);
	//animNode.StopAnim();
	animNode.bPlaying = true;
	animNode.PlayAnim(false, 1, 0.0f);
		
	if(DestroyWhenAnimEnd)
	{
		sma.LifeSpan = animNode.GetAnimPlaybackLength();
	}
	else
	{
		sma.LifeSpan = SpawnMeshLifeSpan;
	}

	`log("_________ sma.LifeSpan" @sma.LifeSpan @AnimToPlay @animNode @animNode.AnimSeq @animNode.AnimSeq.SequenceLength);
}
native function Update(MMOActor actor);

defaultproperties
{
	PositionOffsetRange=(X=0,Y=0)
	EnableRandomRotation=false
	SpawnMeshLifeSpan=3
	DestroyWhenAnimEnd=true
	// TODO: use the right animtree
//	AnimTree=AnimTree'AnimationTreeTemplate.AT_OneNode'
}