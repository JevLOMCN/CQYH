//
//Show a larger Hint on screen
//
class MMOStatusProc_SetAlwaysWalk extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);


var() bool bAlwaysWalk;

event OnExecute(MMOActor actor)
{
	NativeOnExecute(actor);
}
native function NativeOnExecute(MMOActor actor);
native function Update(MMOActor actor);


defaultproperties
{

}