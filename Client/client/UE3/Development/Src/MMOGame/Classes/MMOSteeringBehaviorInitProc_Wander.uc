//
// Behavior initialization handling interface
//
class MMOSteeringBehaviorInitProc_Wander extends object implements(IMMOSteeringBehaviorInitProc)
	native
	editinlinenew
	hidecategories(Object);

var() bool on;

native function OnProcess(MMOSteeringBehavior behavior);

defaultproperties
{
	On=1
}
