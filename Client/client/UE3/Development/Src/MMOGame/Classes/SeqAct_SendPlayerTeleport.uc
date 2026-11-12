// send player displacement

class SeqAct_SendPlayerTeleport extends SequenceAction
	native;

final native function SendPlayerMove();

event Activated()
{	
	SendPlayerMove();
	
	ActivateOutputLink(0);
}

DefaultProperties
{
	ObjName="Send Player Teleport"
	ObjCategory="MMO Server Old"
	
	InputLinks.Empty
	InputLinks(0)=(LinkDesc="In")

	OutputLinks.Empty
	OutputLinks(0)=( LinkDesc="Out" )

	VariableLinks.Empty
}
