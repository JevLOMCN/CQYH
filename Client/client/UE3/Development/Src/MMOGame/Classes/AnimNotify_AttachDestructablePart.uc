//
// notify to attach a model at a specific time
//
class AnimNotify_AttachDestructablePart extends AnimNotify_Scripted
	dependson(NPC);

// initial list of destructible parts
var(MMO) editinline DestructablePartAttachInfo DestructablePartTemplate;

var(MMO) bool NotifyScript;
var(MMO) name ScriptEventName;

var DestructablePartAttachInfo AttachedInstance;

event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local NPC actor;
	actor = NPC(Owner);
	if(actor == none)
	{
		//`log("Error[AnimNotify_AttachDestructablePart.Notify]: notify owner actor is not MMOActor!" @Owner);
		return;
	}

	//`log("event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator)");
	if(AttachedInstance == none)
	{
		AttachedInstance = new DestructablePartTemplate.Class ( DestructablePartTemplate );
	}

	actor.AttachDestructablePart(AttachedInstance);

	`log("___________AnimNotify_AttachDestructablePart::AttachDestructablePart" @AttachedInstance);

	if(NotifyScript)
	{
		// TODO: call by script config name, seems to need a C++ extension.
		actor.OnAttachDestructablePart(AttachedInstance);
	}
}

event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local NPC actor;

	actor = NPC(Owner);
	if(actor == none)
	{
		//`log("Error[AnimNotify_AttachDestructablePart.NotifyEnd]: notify owner actor is not MMOActor!" @Owner);
		return;
	}

	//`log("event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator)");
	if(AttachedInstance != none)
	{
		actor.DetachDestructablePart(AttachedInstance);
		AttachedInstance = none;
	}
}


DefaultProperties
{
	NotifyColor=(R=255,G=255,B=0,A=255)
}
