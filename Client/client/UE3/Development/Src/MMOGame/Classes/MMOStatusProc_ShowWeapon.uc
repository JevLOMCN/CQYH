//
// Set pickup
//
class MMOStatusProc_ShowWeapon extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() bool bShowWeapon;

event OnExecute(MMOActor actor)
{
	//actor.HideWeapon(bShowWeapon==false);
	actor.SetWeaponVisibleByAbility(bShowWeapon);
}
native function Update(MMOActor actor);


defaultproperties
{
	bShowWeapon = true;
}