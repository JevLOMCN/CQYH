//
// PlayerActionEx_AutoAttack, move near to point
//
class PlayerActionEx_AutoAttack extends PlayerActionEx_Base
	native;

var MMOAbility AbilityToUse;
var int      TemplateID;

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
	TemplateID=0;	
}