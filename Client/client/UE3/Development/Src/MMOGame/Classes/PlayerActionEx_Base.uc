//
// PlayerActionEx_Base, player action base class
//
class PlayerActionEx_Base extends Object
	native
	abstract;

var transient MMOPlayerController ActionOwner;

var bool bStart;
var bool bEnd;
var bool bNeedReplaceTheFrontAction;
var float LiftSpan;

// dynamic data
var float PassedTime;

cpptext
{
	// debug info
	virtual FString Dump();

	virtual void Update(FLOAT DeltaTime);
	virtual void ResetAction(UPlayerActionEx_Base* InAction);
	virtual void OnAction(UBOOL bOn);
	virtual UBOOL IsComplete();
	virtual UBOOL IsStart();
	virtual UBOOL IsLatenAction();
	
}

defaultproperties
{
	bStart=false
	bEnd=false
	bNeedReplaceTheFrontAction=true
	LiftSpan=0.6
	PassedTime=0.f
}