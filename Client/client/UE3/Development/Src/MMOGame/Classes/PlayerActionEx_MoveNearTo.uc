//
// PlayerActionEx_MoveNearTo, move near to point
//
class PlayerActionEx_MoveNearTo extends PlayerActionEx_Base
	native;

var Vector   Destination;
var int      targetActorID;
var int      TemplateID;
var float    MinDistance;
var float    MaxDistance;
var float    CurUpdateTime;
var float    MaxUpdateTime;
//
var float    CurDistance;


var float    MaxMovedTime;
//
var float    CurrentTime;
var bool     FollowFlag;

var bool     IsDoodad;
var bool     bPrimAttackDynamicRange;

var Vector TargetLastLocation;
var int      targetActorType;

cpptext
{
	virtual FString Dump();

	virtual void Update(FLOAT DeltaTime);
	virtual void ResetAction(UPlayerActionEx_Base* InAction);
	virtual void OnAction(UBOOL bOn);
	virtual UBOOL IsComplete();
	virtual UBOOL IsLatenAction();
}


defaultproperties
{
	TemplateID = 0;
	MaxMovedTime=1000.0
	MinDistance =0.0
	MaxDistance =0.0
	CurDistance =0.0
	CurUpdateTime = 0.0
	MaxUpdateTime = 0.125
	FollowFlag    = false
	IsDoodad    = false
	targetActorType = 0
}