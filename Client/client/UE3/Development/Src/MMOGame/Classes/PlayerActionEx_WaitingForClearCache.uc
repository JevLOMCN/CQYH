//
// PlayerActionEx_WaitingToMove, wait for ability cache to clear, send move
//
class PlayerActionEx_WaitingForClearCache extends PlayerActionEx_Base
	native;

var float DelayTime;
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
	DelayTime=-1.0
}