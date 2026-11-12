//
// Set ability activation
//
class MMOStatusProc_LoopSkillSlot extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() int   AbilityId;
var() bool  bActive;
var() int   nType;

event OnExecute(MMOActor actor)
{
	NativeOnExecute(actor);
	//actor.UseAbility(AbilityTemplate.GetAbilityId());
}
native function NativeOnExecute(MMOActor actor);
native function Update(MMOActor actor);


defaultproperties
{
	nType=0
}