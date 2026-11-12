//
//Set bound destructible parts
//
class MMOStatusProc_SetMMOAttach extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	dependson(DestructablePartAttachInfo)
	dependson(ParticleSystemAttachInfo)
	hidecategories(Object);

// initial list of destructible parts
var(MMOAttach) editinline array<DestructablePartAttachInfo> NewAttachDestructableParts <DisplayName="NewAttachDestructableParts">;
// currently attached particle system
var(MMOAttach) array<ParticleSystemAttachInfo> NewAttachedParSys    <DisplayName="NewAttachedParSys">;

// attach destructible part
function AttachDestructablePart(DestructablePartAttachInfo dpai , MMOActor actor)
{
	`log("________ AttachDestructablePart" @dpai.AttachSocket);
	actor.AttachDestructablePart(dpai);
	dpai.AttachActor(actor);

	//if(dpai.IsStaticMeshComponent)
	//{
	//	dpai.MeshComponent = new( actor ) class'StaticMeshComponent';
	//	dpai.MeshComponent.SetStaticMesh(dpai.MeshAsset);
	//	dpai.MeshComponent.SetLightEnvironment(actor.LightEnvironment);
	//	dpai.MeshComponent.SetActorCollision(false, false, false);
	//	actor.Mesh.AttachComponentToSocket( dpai.MeshComponent, dpai.AttachSocket );
		
	//	//`log("[MMOSFX] *** [AttachDestructablePart] attach static mesh:"
	//	//	@dpai.MeshAsset $" To:" @dpai.AttachSocket);
	//}
	//else
	//{
	//	if(dpai.SkelMeshComponent != none)
	//	{
	//		if(dpai.AttachSocket =='None' )
	//		{
	//			dpai.SkelMeshComponent.SetParentAnimComponent( actor.Mesh );
	//			actor.AttachComponent(dpai.SkelMeshComponent);
	//		}
	//		else{
	//			`log("________ AttachDestructablePart" @dpai.SkelMeshComponent.Name );
	//			actor.Mesh.AttachComponentToSocket( dpai.SkelMeshComponent, dpai.AttachSocket );
	//		}
	//	}
	//}
	
	//actor.AttachedDestructableParts.AddItem(dpai);
}

// detach destructible part
function DetachDestructablePart(DestructablePartAttachInfo dpai , MMOActor actor)
{
	`log("[MMOSFX] [DetachDestructablePart] :" @dpai.MeshComponent @dpai.SkelMeshComponent);
	//OnDetachDestructablePart(dpai);

	actor.DetachDestructablePart(dpai);
	//if(dpai.IsStaticMeshComponent)
	//{
	//	actor.Mesh.DetachComponent(dpai.MeshComponent);
	//}
	//else
	//{
	//	`log("________ DetachDestructablePart" @dpai.SkelMeshComponent.Name );
	//	actor.Mesh.DetachComponent(dpai.SkelMeshComponent);
	//}
	//actor.AttachedDestructableParts.RemoveItem(dpai);
}

event OnExecute(MMOActor actor)
{
	local DestructablePartAttachInfo asm;
	local ParticleSystemAttachInfo info;
	local int num;
	local int i;

	`log( "############################################MMOStatusProc_SetMMOAttach:" );
	if(NewAttachDestructableParts.Length>0)
	{

		num = actor.AttachedDestructableParts.Length;

		foreach NewAttachDestructableParts(asm)
		{
			asm = new asm.Class ( asm );
			AttachDestructablePart(asm,actor);
		}

		for(i=0;i<num;++i)
		{
			DetachDestructablePart(actor.AttachedDestructableParts[0]   ,actor );
		}

		//while(actor.AttachedDestructableParts.Length > 0)
		//{
		//	actor.AttachedDestructableParts[0].SkelMeshComponent.SetHidden( false );
		//	DetachDestructablePart(actor.AttachedDestructableParts[0]   ,actor );
		//}


	}

	if(NewAttachedParSys.Length>0)
	{
		while(actor.AttachedParSys.Length > 0)
		{
			actor.DetachParticleSystem(actor.AttachedParSys[0] );
		}

		foreach NewAttachedParSys(info)
		{
			actor.AttachParticleSystem(info);
		}			
	}
}
native function Update(MMOActor actor);


defaultproperties
{

}