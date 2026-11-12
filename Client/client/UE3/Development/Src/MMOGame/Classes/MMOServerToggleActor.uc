//MMOServerToggleActor
//hemu@snda
//Used for mechanisms and doors in scene interaction

class MMOServerToggleActor extends SkeletalMeshActor
	placeable
	native;

/*
Use one Anim to handle four states +
First frame Closed
Play forward Opening
Last frame Opened
Play reverse Closing
*/
enum EToggleActorState
{
	ETAS_Closed,
	ETAS_Opening,
	ETAS_Opened,
	ETAS_Closing
};

//Animation name
var( )  name                       AnimName         <DisplayName="AnimName">;
//Associated Trigger
var()   MMOServerToggleDoorTrigger DoorTrigger      <DisplayName="DoorTrigger">;
//Current state
var()   EToggleActorState          CurState         <DisplayName="ToggleActorState">;; 

//Animation duration
var     float               AnimTime;
var     AnimNodeSequence    AnimNodeSeq;
var     AnimSequence        AnimSeq;

simulated event PostBeginPlay()
{
	super.PostBeginPlay();
	IniData();
}


native final function  IniData();
native final function  SetState(EToggleActorState inState );
native final function  TriggerEvent( );

//Can be freely controlled using Notify
/*
//Sound when opening
var() array<SoundCue>          Sound_Close    <DisplayName="Sound_Close">;
//Sound when closing
var() array<SoundCue>          Sound_Open     <DisplayName="Sound_Open">;
//Related effects
var( ) PointLightComponent     SourceLight    <DisplayName="SourceLight">;
var( ) ParticleSystemComponent Effect         <DisplayName="Effect">;
*/

DefaultProperties
{
	bBlockActors=true
	bCollideActors=true
	bCollideWorld=false
	Begin Object Name=SkeletalMeshComponent0
		SkeletalMesh=SkeletalMesh'MMOEditorMeshes.SkeletalMesh.DefaultSkeletalMesh'
	End Object
	SkeletalMeshComponent=SkeletalMeshComponent0

	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.ServerTrigger'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Triggers"
		Translation=(X=0,Y=0,Z=400);
	End Object
	Components.Add(Sprite)
}
