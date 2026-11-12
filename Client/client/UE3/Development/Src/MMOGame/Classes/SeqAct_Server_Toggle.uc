
// ToggleTrigger server confirms open

class SeqAct_Server_Toggle extends SequenceAction
	native;

cpptext
{
	virtual UBOOL IsServerKismetObject() const { return TRUE; }
	virtual int GetKismetObjectServerType() const;
	virtual void ExportServerData(FArchive& inStream);
}

function bool IsDesiredInstigator(IServerTrigger obj)
{

	return false;
}

DefaultProperties
{	
	ObjName="Server Toggle"
	ObjCategory="MMO Server"

	InputLinks.Empty
	InputLinks(0)=( LinkDesc="Open" )
	InputLinks(1)=(LinkDesc="Close")
	InputLinks(2)=(LinkDesc="Toggle")
	
	VariableLinks.Empty
	VariableLinks(0)=( ExpectedType=class'SeqVar_Object', LinkDesc="ServerToggleActor" )
	//VariableLinks(1)=( ExpectedType=class'MMOServerToggleActor', LinkDesc="MMOServerToggleActor" )
	
	OutputLinks.Empty
	OutputLinks(0)=( LinkDesc="Out" )
}
