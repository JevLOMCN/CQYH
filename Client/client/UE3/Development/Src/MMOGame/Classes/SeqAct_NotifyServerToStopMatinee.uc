//
// set the object's scale
//
class SeqAct_NotifyServerToStopMatinee extends SequenceAction
	native;



cpptext
{
	virtual void Activated();
}

//event Activated()
//{
//	local SeqVar_Object TargetObject;
//	local Actor actor;

//	foreach LinkedVariables(class'SeqVar_Object', TargetObject, "TargetObject")
//	{
//		actor = Actor(TargetObject.GetObjectValue());
//		if(actor != none)
//		{
//			actor.SetDrawScale(Scale);
//		}
//	}

//	ActivateOutputLink(0);
//}

DefaultProperties
{
	ObjName="NotifyServerToStopMatinee"
	ObjCategory="MMO Client"

	InputLinks.Empty
	InputLinks(0)=(LinkDesc="In")

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Finished")

	VariableLinks.Empty
	//VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TargetObject")
}
