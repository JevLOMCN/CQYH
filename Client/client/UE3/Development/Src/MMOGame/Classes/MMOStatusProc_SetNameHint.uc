//
// Modify name type and color
//
class MMOStatusProc_SetNameHint extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() string NameHintFrame;
var() string NameHintColor;
var() bool   DeleteFlag;

event OnExecute(MMOActor actor)
{
	NativeOnExecute(actor);
}
native function NativeOnExecute(MMOActor actor);
native function Update(MMOActor actor);


defaultproperties
{
}