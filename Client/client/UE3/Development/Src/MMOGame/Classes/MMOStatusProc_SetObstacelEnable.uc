//
//Set blocker
//
class MMOStatusProc_SetObstacelEnable extends Object implements(IMMOStatusProc)
	native	
	editinlinenew
	hidecategories(Object);

var() name ObstacelName;
var() bool Enable;

event OnExecute(MMOActor actor)
{
	`log( "############################################MMOStatusProc_SetObstacelEnable:");
	actor.SetObstracel(ObstacelName,Enable);
}
native function Update(MMOActor actor);


defaultproperties
{

}