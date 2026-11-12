
// the client sends this message after finishing the movie, telling the server which area ID

class SeqAct_SendClientAreaEventComplete extends SequenceAction
	native;

native function SendAreaEventComplete(IServerArea trigger, int areaID);

event Activated( )
{
	local SequenceVariable seqObj;
	local IServerArea srcObj;

	`log("___________ SeqAct_SendClientAreaEventComplete Activated ");
	
	foreach VariableLinks[0].LinkedVariables(seqObj)
	{
		`log("___________ SeqAct_SendClientAreaEventComplete Activated " @SeqVar_Object(seqObj).GetObjectValue());
		srcObj = IServerArea(SeqVar_Object(seqObj).GetObjectValue());
		if(srcObj != none)
		{
			SendAreaEventComplete(srcObj, srcObj.GetAreaID());
		}
	}

	ActivateOutputLink(0);
}

DefaultProperties
{	
	ObjName="Send Client Area Event Complete"
	ObjCategory="MMO Server Old"

	InputLinks.Empty
	InputLinks(0)=( LinkDesc="In" )
	
	OutputLinks.Empty
	OutputLinks(0)=( LinkDesc="Out" )


	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="MMOVolume_Trigger Object",MinVars=1,MaxVars=1)
}
