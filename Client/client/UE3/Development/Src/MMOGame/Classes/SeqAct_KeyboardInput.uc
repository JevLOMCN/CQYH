//
// set keyboard movement switch
//
class SeqAct_KeyboardInput extends SequenceAction
	;

event Activated()
{
	local SeqVar_Player TargetObject;
	local MMOPlayer actor;
	local bool toggleOn;


	if( InputLinks[0].bHasImpulse )
	{
		toggleOn = true;
	}
	else if( InputLinks[1].bHasImpulse )
	{
		toggleOn = false;
	}


	foreach LinkedVariables(class'SeqVar_Player', TargetObject, "TargetObject")
	{
		actor = MMOPlayer(TargetObject.GetObjectValue());
		if((actor != none) && (actor.Controller != none))
		{
			MMOPlayerController(actor.Controller).EnableKeyboardMove = toggleOn; 
			//MMOPlayerController(actor.Controller).ForceLeaveWalkingState();
			//MMOPlayerController(actor.Controller).StopLatentExecution();
		}
	}

	ActivateOutputLink(0);
}

DefaultProperties
{
	ObjName="Toggle Keyboard Movement"
	ObjCategory="MMO Client"

	InputLinks.Empty
	InputLinks(0)=( LinkDesc="Toggle On" )
	InputLinks(1)=( LinkDesc="Toggle Off" )

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Out")

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Player',LinkDesc="TargetObject")
}
