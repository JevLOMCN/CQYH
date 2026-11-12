//
// Play effect
//
class MMOStatusProc_SpawnParticle extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() ParticleSystem EffectTemplate;
var() bool BindToSocket;
var() name SocketName<DynamicList="SocketList">;
var() float EffectScale;
var() float DelaySpawnTime;

var MMOActor Target;

function DelayExecute()
{
	local ParticleSystemComponent sfx;
	local MMOActor actor ;
	actor = Target;

	if(EffectTemplate != none)
	{
		if (BindToSocket)
		{
			sfx = Target.WorldInfo.MyEmitterPool.SpawnEmitterMeshAttachment(
				EffectTemplate, actor.Mesh, SocketName, TRUE, 
				Target.Location, Target.Rotation);
		}
		else
		{
			sfx = Target.WorldInfo.MyEmitterPool.SpawnEmitter(
					EffectTemplate, Target.Location, Target.Rotation);
		}

		if(sfx != none) 
		{
			sfx.SetScale(EffectScale);
			sfx.ActivateSystem();
			//`log("********* Proc Particle" @ EffectScale @ EffectTemplate @ Target.Location);
		}
	}
}

event OnExecute(MMOActor actor)
{
	Target = actor;
	if(DelaySpawnTime > 0)	
	{
		actor.SetTimer(DelaySpawnTime, false, 'DelayExecute', self);
	}	
	else
	{
		DelayExecute();
	}	
}
native function Update(MMOActor actor);

defaultproperties
{
	EffectScale=1
	BindToSocket = false;
}