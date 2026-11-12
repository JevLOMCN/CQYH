//
// Drops
//
class MMODropActor extends Actor implements (IPickableActor, IMMOActor, IInteractableActor)
	config(MMOGame)
	native;

// Choose different models depending on the amount of gold dropped (quest #13020)
var config int BindItemId;
var config int GoldItemId;
var config string BindDropMesh1;
var config string BindDropMesh2;
var config string BindDropMesh3;
var config string BindDropMesh4;
var config string BindDropMesh5;
var config string GoldDropMesh1;
var config string GoldDropMesh2;
var config string GoldDropMesh3;
var config string GoldDropMesh4;
var config string GoldDropMesh5;
var config int DropDivide1;
var config int DropDivide2;
var config int DropDivide3;
var config int DropDivide4;

// Actual model component
var transient StaticMeshComponent Mesh;

var transient int DropID;
var transient int TemplateItemID;

var transient float LastPickTime;
var transient Item  ItemTemplate;

// var float ZGravity;
// var float RPS;
// var float RPSDeAcc;
// var float InitZSpeed;

var transient float RotateSpeed;

var transient float SpawnAnimTick;
var transient bool EnableSpawnAnim;
var transient bool HasPlaySpawnSound;
var transient float SpawnDelayTimer;
var transient float SpawnScaleTimer;
var transient float SpawnScaleSpeed;
var transient Vector TargetPosition;
var transient Vector OriginPosition;
var transient Vector XYDirection;
var transient float  XYDistance;

var float fPickupDelay;

native final function EActorClassType GetActorType() const;
native final function EPickPriority GetPickPriority();
native final function EMMOCursorType GetMouseCursor();

native final function PickBindOK(GFxObject mc);
native final function PickBindCancel(GFxObject mc);

native function vector GetInteractActorLocation();
native function float GetMaxInteractRadius();
native function DoInteract();
native function bool CanInteract() const;
native function SetSilhouette( bool show, Vector sc = vect( 1.f, 1.f, 0.f ) );
native function bool IsHover() const;

native final function InitSpawnAnim();

auto state Idle
{
}

state SpawnAnim
{
	event BeginState( Name PreviousStateName )
	{
		super.BeginState( PreviousStateName );	
		InitSpawnAnim();
	}

	event EndState( Name NextStateName )
	{
		super.EndState( NextStateName );
		//EnableSpawnAnim = false;
	}
}

DefaultProperties
{
	Begin Object Class=StaticMeshComponent Name=MarkerMesh
		BlockActors=false
		//CollideActors=false
		BlockRigidBody=false
		CastShadow=false
		bAcceptsStaticDecals=false
		bAcceptsDynamicDecals=false
		bAcceptsLights=false
		//Model file
		StaticMesh=StaticMesh'MMO_Player.Mesh.TargetLocationMark'
		//Scale3D=(X=0.3,Y=0.3,Z=0.3)
		//Rotation=(Pitch=16384, Yaw=0, Roll=0)
	End Object
	Mesh=MarkerMesh
	CollisionComponent=MarkerMesh
	Components.Add(MarkerMesh)
	bCollideWorld=true
	bCollideWhenPlacing=false
	Physics=PHYS_None
	
	SpawnScaleTimer=0.3
	SpawnScaleSpeed=1.0
	fPickupDelay=-1.0
}