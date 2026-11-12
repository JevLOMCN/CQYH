
//
// mount
//

class Mount extends Object
	dependson(TypeDefine)
	dependson(ActorAttribute)
	dependson(AvatarData)
	native;

var     editinline SkeletalMeshComponent SkelMeshComponent;
var     int                   MountID;

var     MMOActorMaterialController RootMaterialController;
var     MMOActor actor;
var     MMOActorAT ActorAT;


//native function InitMount(AvatarData Mount);
//native function AttachActor(MMOActor InActor);
native function RefreshMountAnimSet(AvatarData MountAvatar);


event  InitMount(AvatarData Mount)
{
	if(SkelMeshComponent==none)
	{
		SkelMeshComponent = new(self) class 'SkeletalMeshComponent';
	}

	SkelMeshComponent.SetSkeletalMesh(Mount.SkeletalMesh );
	SkelMeshComponent.SetAnimTreeTemplate(Mount.AnimationTree );
	SkelMeshComponent.SetPhysicsAsset(Mount.PhysicAsset );

	RefreshMountAnimSet(Mount);

	RootMaterialController = new(self) class'MMOActorMaterialController';
	if(SkelMeshComponent != none)
	{
		RootMaterialController.Init( SkelMeshComponent );
	}

}

event  AttachActor(MMOActor InActor)
{
	if(SkelMeshComponent==none)
	{
		return;
	}
	if(ActorAT ==none ){
		ActorAT = new class'MMOActorAT';
	}
	actor = InActor;
	ActorAT.InitAnim(SkelMeshComponent,actor);
}


defaultproperties
{

}
