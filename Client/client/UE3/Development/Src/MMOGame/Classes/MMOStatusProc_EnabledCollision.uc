//
// Hide corpse after death
//
class MMOStatusProc_EnabledCollision extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

// If a disappear time is set, it is not hidden directly
var() bool Enabled;

event OnExecute(MMOActor actor)
{
	if(Enabled)
	{
		actor.bEnabledCollsion = true;
	}
	else
	{
		actor.bEnabledCollsion = false;
	}
}
native function Update(MMOActor actor);

defaultproperties
{
	Enabled=true
}