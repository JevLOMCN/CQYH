//
// Use ability
//
class MMOStatusProc_SetMaterial extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	dependson(MaterialInterface)
	hidecategories(Object);

var()	array<MaterialInterface>	Materials;

event OnExecute(MMOActor actor)
{
	local int i;
	for (i = 0; i < Materials.length; i++)
	{
		actor.Mesh.SetMaterial( i, Materials[i] );
	}
}
native function Update(MMOActor actor);


defaultproperties
{
}