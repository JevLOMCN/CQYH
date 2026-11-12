//ToggleTrigger send click request to server

class SeqAct_Server_ToggleTriggerRequest extends SequenceAction
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
	ObjName="SeqAct_Server_ToggleTriggerRequest"
	ObjCategory="MMO Server Old"

	InputLinks.Empty
	InputLinks(0)=( LinkDesc="In" )
}
