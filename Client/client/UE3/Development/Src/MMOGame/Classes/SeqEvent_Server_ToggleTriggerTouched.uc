
// ToggleTrigger respond to Event after click

class SeqEvent_Server_ToggleTriggerTouched extends SequenceEvent;

function bool IsDesiredInstigator(Actor obj)
{

	return false;
}

DefaultProperties
{	
	ObjName="Toggle Trigger Touched"
	ObjCategory="MMO Server To Client"
	bPlayerOnly=false

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Out")

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="MMOServerToggleActor Object")
}
