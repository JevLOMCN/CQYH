//
// set RadialBlur switch
//
class SeqAct_RadialBlurEnable extends SequenceAction;

var(MMO) bool FadeIn;
var(MMO) float FadeInTime;
var(MMO) float FadeInScale;
var(MMO) float FadeOutTime;

event Activated()
{
	local SeqVar_Object TargetObject;
	local MMORadialBlurActor blurActor;

	foreach LinkedVariables(class'SeqVar_Object', TargetObject, "TargetObject")
	{
		blurActor = MMORadialBlurActor(TargetObject.GetObjectValue());
		if(blurActor != none)
		{
			if(FadeIn)
			{
				blurActor.FadeIn(FadeInTime, FadeInScale);
			}
			else
			{
				blurActor.FadeOut(FadeOutTime);
			}
		}
	}

	ActivateOutputLink(0);
}

DefaultProperties
{
	FadeInTime=0.3
	FadeOutTime=0.1
	FadeInScale=1


	ObjName="Radial Blur Actor Fade In/Out"
	ObjCategory="MMO Client"

	InputLinks.Empty
	InputLinks(0)=(LinkDesc="Set")

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Finished")

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TargetObject")
}
