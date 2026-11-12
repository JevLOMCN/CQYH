//
// Set pickup
//
class MMOStatusProc_SetPickable extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() bool bPickable;

event OnExecute(MMOActor actor)
{
	actor.IsPickable = bPickable;
	actor.UpdatePlaneShow();
}
native function Update(MMOActor actor);


defaultproperties
{
	bPickable = true;
}