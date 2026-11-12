//
// PlayerActionEx_MoveTo, move to point
//
class PlayerActionEx_MoveTo extends PlayerActionEx_Base
	native;

var Vector Destination;
var float MaxMovedTime;

//
var float CurrentTime;
var bool  CheckCanMove;
var float CheckCanMoveDelay;
var bool  bRunOrWalk;
var bool  bCrossJudge;

cpptext
{
	virtual FString Dump();

	virtual void Update(FLOAT DeltaTime);
	virtual void ResetAction(UPlayerActionEx_Base* InAction);
	virtual void OnAction(UBOOL bOn);
	virtual UBOOL IsComplete();
	virtual UBOOL IsLatenAction();

	UBOOL PreStart();
}


defaultproperties
{
	MaxMovedTime=70.0
	CheckCanMove=false
	CheckCanMoveDelay=0.0
	bRunOrWalk=true
	bCrossJudge=true
}