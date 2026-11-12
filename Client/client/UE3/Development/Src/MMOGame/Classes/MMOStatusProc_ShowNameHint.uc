//
// Set display name
//
class MMOStatusProc_ShowNameHint extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() bool bShowNameHint;

event OnExecute(MMOActor actor)
{
	actor.ShowHeadName = bShowNameHint;
	actor.UpdatePlaneShow();
}
native function Update(MMOActor actor);


defaultproperties
{
	bShowNameHint = true;
}