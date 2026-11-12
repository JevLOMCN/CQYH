//
// Class used to place reference models and animations in the scene
//

class MMORefPostureSkeletalMeshActor extends SkeletalMeshActor
	placeable;

var(MMO) name AnimationNameToPlay <DisplayName="AnimationNameToPlay">;
var(MMO) float PauseAnimTimePosition <DisplayName="PauseAnimTimePosition">;
var(MMO) StaticMeshComponent WeaponPreview;

// Initialization function
simulated event PostBeginPlay()
{
	super.PostBeginPlay();
}

simulated event Tick( float deltaTime )
{
	//local AnimNodeSequence anim;

	super.Tick(deltaTime);
	
	//`log("xxx");

	//if(SkeletalMeshComponent != none)
	//{
	//	anim = AnimNodeSequence(SkeletalMeshComponent.Animations);
	//	if(anim != none)
	//	{
	//		anim.SetAnim(AnimationNameToPlay);
	//		anim.SetPosition(PauseAnimTimePosition, false);
	//		SkeletalMeshComponent.UpdateAnimations();
	//		//SkeletalMeshComponent.ForceUpdate(false);
	//		//self.ForceUpdateComponents(false, false);
	//	}

	//	if(WeaponPreview == none)
	//	{			
	//		WeaponPreview = new( self ) class'StaticMeshComponent';
	//		WeaponPreview.SetStaticMesh(StaticMesh'Monster_Boss_Model.Mesh.G_Boss_weapon');
	//		WeaponPreview.SetLightEnvironment(self.LightEnvironment);
	//		self.SkeletalMeshComponent.AttachComponentToSocket( WeaponPreview, 'Weapon_R' );
	//	}
	//}
}

DefaultProperties
{
	//Begin Object Class=StaticMeshComponent Name=PreviewModel
	//	HiddenGame=TRUE
	//	CollideActors=FALSE
	//	BlockRigidBody=FALSE
	//	CastShadow=FALSE
	//	AlwaysLoadOnClient=FALSE
	//	AlwaysLoadOnServer=FALSE
	//End Object
	//WeaponPreview=PreviewModel
	//Mesh.AttachComponentToSocket( PreviewModel, 'Weapon_R');
}
