//
// Behavior initialization handling interface
//
class MMOSteeringBehaviorInitProc_OffsetPursuit extends object implements(IMMOSteeringBehaviorInitProc)
	native
	editinlinenew
	hidecategories(Object);

var() bool on;
var() int TargetCrowdInstanceID;
var() Vector Offset;

native function OnProcess(MMOSteeringBehavior behavior);


defaultproperties
{
	On=1
	Offset=(X=400,Y=400,Z=0)
}
