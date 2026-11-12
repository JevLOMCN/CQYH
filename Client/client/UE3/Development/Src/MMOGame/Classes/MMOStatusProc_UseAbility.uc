//
// Use ability
//
class MMOStatusProc_UseAbility extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() MMOAbility AbilityTemplate;

event OnExecute(MMOActor actor)
{
	//actor.UseAbility(AbilityTemplate.GetAbilityId());
}
native function Update(MMOActor actor);


defaultproperties
{
}