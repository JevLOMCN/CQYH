//
// destructible object explosion
//

class SeqEvent_FracturedSMAExplode extends SequenceEvent;

//var FracturedStaticMeshActor Actor;
var() FracturedStaticMeshActor InstigatorActor;

event Activated()
{
	InstigatorActor = FracturedStaticMeshActor(Instigator);
	`log("_________FracturedStaticMeshActor" @InstigatorActor @Instigator);

	ActivateOutputLink(0);
}

DefaultProperties
{	
	ObjName="FracturedStaticMeshActor Explode"
	ObjCategory="MMO Client Old"
	bPlayerOnly=false
}
