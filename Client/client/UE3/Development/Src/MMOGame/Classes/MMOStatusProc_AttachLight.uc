//
// Bind effect
//
class MMOStatusProc_AttachLight extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() float DelayTime;
var() bool BindToWeapon;
var() bool BindToSocket;
var() name SocketName<DynamicList="SocketList">;
var() LightComponent LightTemplate;


event OnExecute(MMOActor actor)
{
	local array<SkeletalMeshComponent> arrayMesh;
	local int i;
	local LightComponent lc;

	if(BindToWeapon)	
	{
		arrayMesh = actor.GetClientWeaponSkMesh();
	}
	else	
	{
		arrayMesh.AddItem(actor.Mesh);
	}

	if(LightTemplate != none)
	{
		for(i = 0; i < arrayMesh.Length; ++i)
		{
			lc = new( arrayMesh[i] ) LightTemplate.Class (LightTemplate);

			if (BindToSocket || BindToWeapon)
			{
				arrayMesh[i].AttachComponentToSocket(lc, SocketName);
			}
			else
			{
				actor.AttachComponent(lc);
			}
		}
	}
}

native function Update(MMOActor actor);

defaultproperties
{
	DelayTime = 0;
	BindToWeapon = false;
	BindToSocket = false;
}