
// server blocking change event notification

class SeqEvent_Server_DynamicBlockChange extends SequenceEvent;

function bool IsDesiredInstigator(DynamicBlockingVolume obj)
{
	local Object val;
	local SequenceVariable seqObj;

	foreach VariableLinks[0].LinkedVariables(seqObj)
	{
		`log("OSCAR:_________ SeqEvent_Server_DynamicBlockChange IsDesiredInstigator:" @seqObj @obj);
		val = SeqVar_Object(seqObj).GetObjectValue();
		if(val != none)
		{
			`log("OSCAR:_________ SeqEvent_Server_DynamicBlockChange:" @val @obj);
			if( val == obj)
			{
				return true;
			}
		}
	}

	`log("OSCAR:_________ SeqEvent_Server_DynamicBlockChange: false " @obj);
	return false;
}

DefaultProperties
{	
	ObjName="Server Dynamic Block Volume Change"
	ObjCategory="MMO Server Old"
	bPlayerOnly=false

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Off")
	OutputLinks(1)=(LinkDesc="On")


	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="DynamicBlockingVolume Object",MinVars=1,MaxVars=1)
}
