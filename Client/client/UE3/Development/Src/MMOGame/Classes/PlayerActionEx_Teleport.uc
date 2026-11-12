//
// PlayerActionEx_Teleport, teleport
//
class PlayerActionEx_Teleport extends PlayerActionEx_Base
	native;

var int LinkId;

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
}