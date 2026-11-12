//
// PlayerActionEx_MoveNearToPoint, move close to point
//
class PlayerActionEx_MoveNearToPoint extends PlayerActionEx_Base
	native;

var Vector Destination;
var float MaxMovedTime;

//
var float CurrentTime;
var float CheckTime;

cpptext
{
	virtual FString Dump();

	virtual void Update(FLOAT DeltaTime);
	virtual void OnAction(UBOOL bOn);
	virtual UBOOL IsComplete();
	virtual UBOOL IsLatenAction();
}


defaultproperties
{
	MaxMovedTime=70.0
	CheckTime=0.0
}