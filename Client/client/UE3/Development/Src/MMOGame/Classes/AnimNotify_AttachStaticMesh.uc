//
// notify to attach a model at a specific time
//
class AnimNotify_AttachStaticMesh extends AnimNotify_Scripted
	dependson(MMOActor);

var(MMO) StaticMesh MeshTemplate;
var(MMO) name AttachName;
var(MMO) name SocketName <DynamicList="SocketList">;
var(MMO) editoronly bool Preview;

var bool Attached;

event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local MMOActor actor;
	actor = MMOActor(Owner);
	if(actor == none)
	{
		`log("Error[AnimNotify_AttachStaticMesh.Notify]: notify owner actor is not MMOActor!" @Owner);
		return;
	}

	`log("event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator)");
	Attached = true;
	actor.AttachStaticMesh(AttachName, MeshTemplate, SocketName);
}

event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local MMOActor actor;
	Attached = false;
	actor = MMOActor(Owner);
	if(actor == none)
	{
		`log("Error[AnimNotify_AttachStaticMesh.NotifyEnd]: notify owner actor is not MMOActor!" @Owner);
		return;
	}

	`log("event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator)");
	actor.DetachStaticMesh(AttachName);
}


DefaultProperties
{
	AttachName=AnimNotifyAttach
	Attached=false
	NotifyColor=(R=255,G=255,B=0,A=255)
}
