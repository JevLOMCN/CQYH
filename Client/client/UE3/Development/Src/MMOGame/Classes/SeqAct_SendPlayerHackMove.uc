// used to switch the client sending movement messages to the server
// handle movement simulation while on a ship or in other stationary states

class SeqAct_SendPlayerHackMove extends SequenceAction;

event Activated()
{	
	if( InputLinks[0].bHasImpulse )
		class'WorldInfo'.static.GetWorldInfo().GetALocalPlayerController().PushState('HackMove');
	else
		class'WorldInfo'.static.GetWorldInfo().GetALocalPlayerController().PopState();
	
	ActivateOutputLink(0);
}

DefaultProperties
{
	ObjName="Send Player Hack Move"
	ObjCategory="MMO Server Old"
	
	InputLinks.Empty
	InputLinks(0)=(LinkDesc="Start")
	InputLinks(1)=(LinkDesc="Stop")

	OutputLinks.Empty
	OutputLinks(0)=( LinkDesc="Out" )

	VariableLinks.Empty
}
