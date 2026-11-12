class MMOLoadingTrigger extends MMOServerTrigger
	placeable
	native;
// TriggerTrigger model component
var() StaticMeshComponent TriggerMesh;
// TriggerTrigger effect
var(EmitterComp) ParticleSystem             TriggerParticleSystem;
var transient EmitterSpawnable                 NewEmitter;
var transient ParticleSystemComponent   EmitterComponent;
//Related sound effects

//Associated MMOActor
var()  MMOActor        TriggerUser;
//Local timing time
var()   float                   fLoadingTime;


//Set associated MMOActor
/*
 Correspondence between the two
	UINT32           dwTrapID;
	UINT32           dwObjID;
*/
function SetUser( MMOActor User )
{
	if(!User.IsA('MMOActor'))
	{
		return;
	}
	TriggerUser = User;
}
//Send Trigger open message to the server
native function OpenTriggerReq();
//Server notifies open
event OnTriggerOpen(int iLoadingTime);
//Server notifies interrupt
event OnTriggerLoadingStop();
//Notify the server that local timing ended
native function OpenTriggerCompleteReq();

function SwitchState(ETriggerInitState newState)
{
	//name newName;
	switch(newState)
	{

	}
	if(!IsInState('MouseClickMove'))
	{
		 GotoState( 'MouseClickMove' );
	}
}
simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	RegisterToFacade();
}
// Register with Facade for lookup
native function RegisterToFacade();
//Hide
function Hide()
{
	TriggerMesh.SetHidden(true);
}
//Show
function Show()
{
	TriggerMesh.SetHidden(false);
}
event Tick(float deltaTime)
{
	super.Tick(deltaTime);

}
//Mechanism disappears, needs to refresh after a certain time
state() Invisible
{


Begin:
	InitTriggerState=eTIS_Invisible;
}
//Default closed state, only shows Mesh
auto state() Off
{


Begin:
	InitTriggerState=eTIS_Off;
}
//Activated state, shows Mesh and Particle
state() On
{

Begin:
	InitTriggerState=eTIS_On;
}
//Open state
//11, countdown, notify the server when done, effect
//22, call User, show User related animation / play sound / UI related
state() Loading
{
	event Tick(float deltaTime)
	{
		super.Tick(deltaTime);

	}
Begin:
	InitTriggerState=eTIS_Loading;
}
DefaultProperties
{
/*
	//Override the default icon in the previous layer
	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.ServerTrigger'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"
		//Scale=0.2
	End Object
	Components.Add(Sprite);*/
	TriggerType=eTT_TRAPTYPE_LOADING
	Begin Object Class=StaticMeshComponent Name=tTriggerMesh
		//Model file
		StaticMesh=StaticMesh'MMO_Player.Mesh.TargetLocationMark'
		Rotation=(Pitch=16384, Yaw=0, Roll=0)
		RBChannel=RBCC_Nothing
	End Object
	TriggerMesh = tTriggerMesh
	CollisionComponent=tTriggerMesh
	//tTriggerMesh.SetActorCollision( true,false )
	//tTriggerMesh.SetTraceBlocking( true, true )
	//CollisionComponent.AddRadialForce( vect(0,0,0), 200, 50, 0 )
	Components.Add( tTriggerMesh )
}
