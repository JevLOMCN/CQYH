//
// PlayerActionEx_UseAbility, use ability
//
class PlayerActionEx_UseAbility extends PlayerActionEx_Base
	native;

var MMOAbility AbilityToUse;
var MMOActor Target;
var int      targetActorID;
var int      TemplateID;
var int      targetActorType;
var Vector TargetLocation;
var Vector FixedLocation;
var bool bCancelAbility;
var bool bPrimAttackDynamicAbility;

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
	bCancelAbility=false
	targetActorType=0
}