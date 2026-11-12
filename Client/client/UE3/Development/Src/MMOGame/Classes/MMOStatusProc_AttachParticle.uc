//
// Bind effect
//
class MMOStatusProc_AttachParticle extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() ParticleSystem EffectTemplate;
var() float EffectScale;
var() name SocketName<DynamicList="SocketList">;
var() bool BindToWeapon;
var() EWeaponType WeaponType<EditCondition=BindToWeapon>;
var() name VertSurfaceActorParameterName;

event OnExecute(MMOActor actor)
{
	local ParticleSystemComponent sfx;
	local array<SkeletalMeshComponent> wpMesh;
	local int i;

	if(BindToWeapon)	
	{
		wpMesh = actor.GetClientWeaponSkMesh();
	}
	else	
	{
		wpMesh.AddItem(actor.Mesh);
	}

	if(EffectTemplate != none)
	{
		for(i = 0; i < wpMesh.Length; ++i)
		{
			//sfx = new( self ) class'ParticleSystemComponent';
			// self is a template and cannot produce an instance
			sfx = new( wpMesh[i] ) class'ParticleSystemComponent';  // zhaojun@snda
			sfx.SetTemplate(EffectTemplate);
			sfx.SetScale(EffectScale);			
			wpMesh[i].AttachComponentToSocket(sfx, SocketName);			
			sfx.SetSkeletalMeshComponentParameter(VertSurfaceActorParameterName, wpMesh[i]);
			sfx.ActivateSystem();
		}
	}	
}
native function Update(MMOActor actor);

defaultproperties
{
	EffectScale=1
	VertSurfaceActorParameterName=VertSurfaceActor
}