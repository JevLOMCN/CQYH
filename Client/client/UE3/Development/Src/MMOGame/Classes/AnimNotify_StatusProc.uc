//
// animation notify extension class, Notify calls the IMMOStatusProc action
//
class AnimNotify_StatusProc extends AnimNotify_Scripted
	dependson(MMOActor);

struct native StatusProcData
{
	var() float Rate;
	var() editinline array<IMMOStatusProc> Proc;
};

// MMOStatusProc called by AnimNotify
var(MMOStatusProc) editinline array<StatusProcData> AniNotifyProcList;
var(MMOStatusProc) editinline array<StatusProcData> AniNotifyEndProcList;


function ProcessStatusProc(MMOActor actor, array<StatusProcData> procs)
{
	local int i, j;
	local float randNumb;
	randNumb = RandRange(0, 1);

	//`log("********************* ProcessStatusProc" @self);
	//ScriptTrace();

	for(i = procs.Length - 1; i >= 0; --i)
	{
		if(randNumb <= procs[i].Rate)
		{
			for(j = 0; j < procs[i].Proc.Length; ++j)
			{
				procs[i].Proc[j].OnExecute(actor);
			}
			break;
		}
		else
		{
			randNumb -= procs[i].Rate;
		}
	}
}

event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local MMOActor actor;
	actor = MMOActor(Owner);
	if(actor == none)
	{
		`log("Error[AnimNotify_StatusProc.Notify]: notify owner actor is not MMOActor!" @Owner);
		return;
	}

	`log("event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator)");
	ProcessStatusProc(actor, AniNotifyEndProcList);
}

event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local MMOActor actor;
	actor = MMOActor(Owner);
	if(actor == none)
	{
		`log("Error[AnimNotify_StatusProc.NotifyEnd]: notify owner actor is not MMOActor!" @Owner);
		return;
	}

	`log("event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator)");
	ProcessStatusProc( actor, AniNotifyEndProcList);
}

DefaultProperties
{
	
}
