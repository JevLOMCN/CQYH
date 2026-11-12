//
// PlayerActionEx_CannotInterrupt, whether it can be interrupted
//
class PlayerActionEx_CannotInterrupt extends PlayerActionEx_Base
	native;

var int InterruptLevel;
var float InterruptLevelTime;
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
	InterruptLevel=0
	InterruptLevelTime=-0.0
}