//
// Tip info
//
class MMOStatusProc_AddMessage extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() string  ScreenMessage;
var() string  SystemMessage;


event OnExecute(MMOActor actor)
{
	NativeOnExecute(actor);
}
native function NativeOnExecute(MMOActor actor);
native function Update(MMOActor actor);


defaultproperties
{

}