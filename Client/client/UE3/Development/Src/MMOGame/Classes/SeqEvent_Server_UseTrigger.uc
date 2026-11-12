
// server Trigger event notification

class SeqEvent_Server_UseTrigger extends SequenceEvent;

function bool IsDesiredInstigator(IServerTrigger obj)
{
	local Object val;
	local SequenceVariable seqObj;
	//local SequenceObject parentSeq;

	//parentSeq = self.ParentSequence;
	//while(parentSeq != none)
	//{
	//	`log("OSCAR:_________ SeqEvent_Server_UseTrigger" @parentSeq @parentSeq.Outer);
	//	parentSeq = parentSeq.ParentSequence;
	//}

	if(VariableLinks.Length <= 0)
	{
		`log("OSCAR:_________ SeqEvent_Server_UseTrigger IsDesiredInstigator Empty Link Variable, TriggerID:"
			@obj.GetTriggerID() @self @self.Outer);
		return false;
	}

	foreach VariableLinks[0].LinkedVariables(seqObj)
	{
		//`log("OSCAR:_________ SeqEvent_Server_UseTrigger IsDesiredInstigator:" @seqObj @obj);
		val = SeqVar_Object(seqObj).GetObjectValue();
		if(val != none)
		{
			//`log("OSCAR:_________ SeqEvent_Server_AreaChange:" @val @obj);
			if( val == obj)
			{
				return true;
			}
		}
	}

	`log("OSCAR:_________ SeqEvent_Server_AreaChange: false " @obj);
	return false;
}

DefaultProperties
{	
	ObjName="Server Trigger"
	ObjCategory="MMO Server Old"
	bPlayerOnly=false

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Off")
	OutputLinks(1)=(LinkDesc="On")


	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="IServerTrigger Object",MinVars=1,MaxVars=1)
}
