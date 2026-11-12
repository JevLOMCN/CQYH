//
// set the state transitions of the object controller
//
class SeqAct_ControllerGotoState extends SequenceAction
	;

var(MMO) name StateName;
var(MMO) name LabelName;

event Activated()
{
	local SeqVar_Object TargetObject;
	local Pawn actor;

	foreach LinkedVariables(class'SeqVar_Object', TargetObject, "TargetObject")
	{
		actor = Pawn(TargetObject.GetObjectValue());
		if((actor != none) && (actor.Controller != none))
		{
			actor.Controller.GotoState(StateName, LabelName);
		}
	}

	ActivateOutputLink(0);
}

DefaultProperties
{
	ObjName="Controller Goto State"
	ObjCategory="MMO Client"

	InputLinks.Empty
	InputLinks(0)=(LinkDesc="Set")

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Finished")

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TargetObject")
}
