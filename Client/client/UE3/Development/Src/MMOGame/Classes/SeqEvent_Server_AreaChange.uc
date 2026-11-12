
// server area event notification

class SeqEvent_Server_AreaChange extends SequenceEvent;

function bool IsDesiredInstigator(Actor obj)
{
	local Object val;
	local SequenceVariable seqObj;

	foreach VariableLinks[0].LinkedVariables(seqObj)
	{
		val = SeqVar_Object(seqObj).GetObjectValue();
		if(val != none)
		{
			if( val == obj)
			{
				`log("OSCAR:_________ SeqEvent_Server_AreaChange: true " @obj);
				return true;
			}
		}
	}

	`log("OSCAR:_________ SeqEvent_Server_AreaChange: false " @obj);
	return false;
}

DefaultProperties
{	
	ObjName="Server Area Change"
	ObjCategory="MMO Server Old"
	bPlayerOnly=false

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Player Enter")
	OutputLinks(1)=(LinkDesc="Player Exit")


	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="MMOVolume_Trigger Object",MinVars=1,MaxVars=1)
}
