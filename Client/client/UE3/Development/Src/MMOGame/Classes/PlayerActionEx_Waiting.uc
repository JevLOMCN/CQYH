//
// PlayerActionEx_Waiting, waiting
//
class PlayerActionEx_Waiting extends PlayerActionEx_Base
	native;


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
}