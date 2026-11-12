//
// blow up the FracturedStaticMeshActor
//
class SeqAct_ExplodeFracturedStaticMesh extends SequenceAction
	dependson(FracturedStaticMeshActor);

event Activated()
{
	local SeqVar_Object TargetObject;
	local FracturedStaticMeshActor actor;

	foreach LinkedVariables(class'SeqVar_Object', TargetObject, "TargetObject")
	{
		actor = FracturedStaticMeshActor(TargetObject.GetObjectValue());
		if(actor != none)
		{
			actor.Explode();
		}
	}

	ActivateOutputLink(0);
}

DefaultProperties
{
	ObjName="Explode FracturedStaticMeshActor"
	ObjCategory="MMO Client Old"

	InputLinks.Empty
	InputLinks(0)=(LinkDesc="Explode")

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Finished")

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TargetObject")
}
