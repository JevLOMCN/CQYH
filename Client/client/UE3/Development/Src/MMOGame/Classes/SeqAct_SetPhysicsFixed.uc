//
// set whether the object's physics joints are fixed
//
class SeqAct_SetPhysicsFixed extends SequenceAction
	dependson(SkeletalMeshActor);

var(MMO) name BodyName;
var(MMO) bool FixBody;

event Activated()
{
	local SeqVar_Object TargetObject;
	local int idx;
	local KAsset actor;

	`log("_____[SeqAct_SetPhysicsFixed]" @BodyName @FixBody);

	foreach LinkedVariables(class'SeqVar_Object', TargetObject, "TargetObject")
	{
		actor = KAsset(TargetObject.GetObjectValue());
		idx = actor.ReplicatedPhysAsset.FindBodyIndex(BodyName);
		actor.ReplicatedPhysAsset.BodySetup[idx].bFixed = FixBody;		
		actor.SkeletalMeshComponent.WakeRigidBody();
		`log("_____[SeqAct_SetPhysicsFixed]" @actor @BodyName @FixBody);
	}

	ActivateOutputLink(0);
}

DefaultProperties
{
	ObjName="Fix Physics Body"
	ObjCategory="MMO Client"

	InputLinks.Empty
	InputLinks(0)=(LinkDesc="Set")

	OutputLinks.Empty
	OutputLinks(0)=(LinkDesc="Finished")

	VariableLinks.Empty
	VariableLinks(0)=(ExpectedType=class'SeqVar_Object',LinkDesc="TargetObject")
}
