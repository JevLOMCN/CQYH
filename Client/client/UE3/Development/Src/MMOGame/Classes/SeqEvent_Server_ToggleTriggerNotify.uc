//ToggleTrigger server notifies Event
//associate MMOServerToggleActor set State
class SeqEvent_Server_ToggleTriggerNotify extends SequenceEvent;

function bool IsDesiredInstigator(Actor obj)
{

	return false;
}

DefaultProperties
{	
	ObjName="Toggle Trigger Notify"
	ObjCategory="MMO Server To Client"
	bPlayerOnly=false

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Open")
	OutputLinks(1)=(LinkDesc="Close")

}