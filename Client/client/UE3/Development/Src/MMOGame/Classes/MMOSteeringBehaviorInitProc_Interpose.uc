//
// Behavior initialization handling interface
//
class MMOSteeringBehaviorInitProc_Interpose extends object implements(IMMOSteeringBehaviorInitProc)
	native
	editinlinenew
	hidecategories(Object);

var() bool on;
var() int TargetCrowdInstanceID1;
var() int TargetCrowdInstanceID2;

native function OnProcess(MMOSteeringBehavior behavior);


defaultproperties
{
	On=1
}
