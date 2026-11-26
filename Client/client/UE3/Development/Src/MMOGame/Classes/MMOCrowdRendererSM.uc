/*
* MMOCrowdRendererSM.uc
*/
class MMOCrowdRendererSM extends Emitter
	native
	hidecategories(Object)
	ClassGroup(__Crowd__);

var(Crowd) MMOCrowdSystem	CrowdSystem;

var ParticleModuleLocationCrowdSystem	PSModule;

native function Init();

simulated function PostBeginPlay()
{
//	local ParticleLODLevel pll;
//	local ParticleModule pm;

	super.PostBeginPlay();

	if( CrowdSystem == none )
	{
		`log( "[MMO Debug] Crowd system renderer has no system assigned." );
		return;
	}

	Init();

	if( PSModule == none )
	{
		`log( "[MMO Deubg] Failed to find crowd system module in emitter"@self );
		return;
	}

	if( PSModule.ParticleLocations.Length != CrowdSystem.CrowdInstanceData.Length )
	{
		`Log( "[MMO Debug] Crowd system has different instance number than its renderer." );
	}
}

event Tick( float DeltaTime )
{
	local MMOCrowdInstance inst;
	local int i;

	if (PSModule == none)
		return;
	if (CrowdSystem == none)
	{
		PSModule.ParticleLocations.Length = 0;
		PSModule.ParticleRotations.Length = 0;
		return;
	}

	PSModule.ParticleLocations.Length = CrowdSystem.CrowdInstanceData.Length;
	PSModule.ParticleRotations.Length = CrowdSystem.CrowdInstanceData.Length;
	for (i = 0; i < CrowdSystem.CrowdInstanceData.Length; ++i)
	{
		inst = CrowdSystem.CrowdInstanceData[i];
		if (inst != none)
		{
			PSModule.ParticleLocations[i] = inst.Location;
			PSModule.ParticleRotations[i] = inst.Direction;
		}
		else
		{
			PSModule.ParticleLocations[i] = vect(0, 0, 0);
			PSModule.ParticleRotations[i] = vect(0, 0, 0);
		}
	}
}

defaultproperties
{
}