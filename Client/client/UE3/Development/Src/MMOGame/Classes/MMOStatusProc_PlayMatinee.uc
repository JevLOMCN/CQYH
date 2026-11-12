//
// Open matinee
//
class MMOStatusProc_PlayMatinee extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() int nMatineeIndex;
var() bool bInputEnble;

native function NativeOnExecute(MMOActor actor);
event OnExecute(MMOActor actor)
{
	//actor.PlayTestAnim(0);
	`log( "############################################MMOStatusProc_PlayMatinee:"@nMatineeIndex);
	actor.TriggerGlobalEventClass( class'SeqEvent_StartMatinee', actor, nMatineeIndex );
   
}
native function Update(MMOActor actor);


defaultproperties
{
	bInputEnble		= true;
	nMatineeIndex	= -1;
}