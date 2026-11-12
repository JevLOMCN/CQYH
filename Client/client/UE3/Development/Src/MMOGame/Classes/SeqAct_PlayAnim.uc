
// play the specified animation

class SeqAct_PlayAnim extends SequenceAction;

var(MMO) name AnimationName;
var(MMO) bool LoopAnim ;

event Activated()
{
	local SeqVar_Object TargetObject;
	local MMOActor actor;

	foreach LinkedVariables(class'SeqVar_Object', TargetObject, "TargetObject")
	{
		actor = MMOActor(TargetObject.GetObjectValue());
		if(actor != none)
		{
			//actor.PlayCustomAnim(AnimationName, LoopAnim);
			actor.PlayAnimation( AnimationName, , ,LoopAnim);
		}
	}

	ActivateOutputLink(0);
}

DefaultProperties
{	
	ObjName="Play Anim"
	ObjCategory="MMO Client"
	InputLinks.Empty
	InputLinks(0)=(LinkDesc="Play")

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Finished")

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TargetObject")
}
