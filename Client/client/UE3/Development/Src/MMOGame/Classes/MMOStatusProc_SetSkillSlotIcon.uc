//
// Set ability icon
//
class MMOStatusProc_SetSkillSlotIcon extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() int   AbilityId;
var() string  IconName;

event OnExecute(MMOActor actor)
{
	NativeOnExecute(actor);
	//actor.UseAbility(AbilityTemplate.GetAbilityId());
}
native function NativeOnExecute(MMOActor actor);
native function Update(MMOActor actor);


defaultproperties
{
}