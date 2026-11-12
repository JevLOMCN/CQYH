//
// Hide corpse after death
//
class MMOStatusProc_HideBody extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

// If a disappear time is set, it is not hidden directly
var() int BodyLifeSpan;

event OnExecute(MMOActor actor)
{
	if(BodyLifeSpan > 0)
	{
		actor.LifeSpan = BodyLifeSpan;
	}
	else
	{
		//actor.SetHidden(true);
		actor.SetVisibleByAbility(false);
	}
}
native function Update(MMOActor actor);


defaultproperties
{
	BodyLifeSpan=0
}