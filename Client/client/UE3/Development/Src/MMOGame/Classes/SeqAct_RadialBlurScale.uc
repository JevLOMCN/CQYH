//
// set RadialBlur intensity
//
class SeqAct_RadialBlurScale extends SequenceAction;

var(MMO) float RadialScale;

event Activated()
{
	local SeqVar_Object TargetObject;
	local RadialBlurActor blurActor;

	foreach LinkedVariables(class'SeqVar_Object', TargetObject, "TargetObject")
	{
		blurActor = RadialBlurActor(TargetObject.GetObjectValue());
		if(blurActor != none)
		{
			blurActor.RadialBlur.SetBlurScale(RadialScale);
		}
	}

	ActivateOutputLink(0);
}

DefaultProperties
{
	RadialScale=1

	ObjName="Set Radial Blur Scale"
	ObjCategory="MMO Client"

	InputLinks.Empty
	InputLinks(0)=(LinkDesc="Set")

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Finished")

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TargetObject")
}
