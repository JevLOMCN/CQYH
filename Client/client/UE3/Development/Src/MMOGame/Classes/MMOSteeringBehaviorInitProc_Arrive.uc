//
// Behavior initialization handling interface
//
class MMOSteeringBehaviorInitProc_Arrive extends object implements(IMMOSteeringBehaviorInitProc)
	native
	editinlinenew
	hidecategories(Object);

var() bool on;

native function OnProcess(MMOSteeringBehavior behavior);


defaultproperties
{
	on=1
}
