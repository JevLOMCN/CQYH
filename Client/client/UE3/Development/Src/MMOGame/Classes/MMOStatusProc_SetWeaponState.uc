//
// Sheathe/draw blade
//
class MMOStatusProc_SetWeaponState extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() bool bHoldWeapon;

event OnExecute(MMOActor actor)
{
	MMOPlayer(actor).ReflashWeaponSocket();

	//if(bHoldWeapon==false)
	//{
	//	MMOPlayer(actor).OnEnterSafeZone();
	//}
	//else{
	//	MMOPlayer(actor).OnLeavingSafeZone();
	//}
}
native function Update(MMOActor actor);


defaultproperties
{
	bHoldWeapon = true;
}