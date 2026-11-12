
// server notifies portal state change

class SeqEvent_Server_TeleportOpen extends SequenceEvent;

function bool IsDesiredInstigator(int id)
{
	local TeleportOrig val;
	local SequenceVariable seqObj;

	if(VariableLinks.Length <= 0)
	{
		return false;
	}

	foreach VariableLinks[0].LinkedVariables(seqObj)
	{
		//`log("OSCAR:_________ SeqEvent_Server_TeleportOpen IsDesiredInstigator:" @seqObj @obj);
		val = TeleportOrig(SeqVar_Object(seqObj).GetObjectValue());
		if( (val != none)
			&& (val.AutoServerID == id))
		{
			//`log("OSCAR:_________ SeqEvent_Server_AreaChange:" @val @obj);
			return true;
		}
	}

	return false;
}

DefaultProperties
{	
	ObjName="Server Teleport Open"
	ObjCategory="MMO Server Old"
	bPlayerOnly=false

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Closed")
	OutputLinks(1)=(LinkDesc="Open")


	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TeleportOrig Object")
}
