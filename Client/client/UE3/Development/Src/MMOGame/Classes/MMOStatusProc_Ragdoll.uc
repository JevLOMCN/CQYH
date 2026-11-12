//
// Enable Ragdoll after death
//
class MMOStatusProc_Ragdoll extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

event OnExecute(MMOActor actor)
{
	actor.EnableRagdoll(true);
}

native function Update(MMOActor actor);


defaultproperties
{
}