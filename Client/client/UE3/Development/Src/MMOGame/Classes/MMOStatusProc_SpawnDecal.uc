//
// Spawn object
//
class MMOStatusProc_SpawnDecal extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var(DecalComp) MaterialInterface ImpactDecalMaterial;
var(DecalComp) float ImpactDecalSizeMin;
var(DecalComp) float ImpactDecalSizeMax;
var(DecalComp) float ImpactDecalDepth;
/** The Lifespan to give to the decal if it's not a MITV (otherwise Lifespan is automatically calculated from the MITV's keyframes) */
var(DecalComp) float ImpactDecalLifespanNonMITV;
var(DecalComp) float DelaySpawnTime;

var MMOActor Target;

function SpawnImpactDecal(Actor inTarget, MaterialInterface impactMat, Vector HitLocation, Vector HitNormal)
{
   local float SizeChange;
   local MaterialInstanceTimeVarying MITV;
   local MaterialInterface theDecal;
   local float DecalLifespan;
   local float decalOrientation;

   Target = MMOActor(inTarget);

	if(impactMat == none)
		return;

	decalOrientation = FRand() * 360.0;

	//`log("&&&&&&&&&&&&&&&" @HitNormal @HitLocation);

	SizeChange = ImpactDecalSizeMax;//RandRange(ImpactDecalSizeMin, ImpactDecalSizeMax);

	theDecal = impactMat;
	DecalLifespan = ImpactDecalLifespanNonMITV;
	//if it's a MITV, create a new MIC of the MITV, use that for the Decal's Material, and calculate its lifespan from the MITV
	if(impactMat.IsA('MaterialInstanceTimeVarying'))
	{
		MITV = new class'MaterialInstanceTimeVarying';
		MITV.SetParent(impactMat);
		DecalLifespan = MITV.GetMaxDurationFromAllParameters();

		Target.WorldInfo.MyDecalManager.SpawnDecal(MITV, HitLocation, Rotator(Normal(-HitNormal + vect(0.01, 0, 0))),
			SizeChange, SizeChange, ImpactDecalDepth, true, decalOrientation, none, true, false, , , , DecalLifespan);

		MITV.SetDuration(DecalLifespan);
		//MITV.SetScalarStartTime('DecalAlpha', 1);
		//`log("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
	}
	else
	{
		//create a decal in the World with this handy helper function!
		Target.WorldInfo.MyDecalManager.SpawnDecal(theDecal, HitLocation, Rotator(Normal(-HitNormal + vect(0.01, 0, 0))),
			SizeChange, SizeChange, ImpactDecalDepth, true, decalOrientation, none, true, false, , , , DecalLifespan);
	}
}

function DelayExecute()
{
	local Vector start, end,HitLocation, HitNormal;
	local Actor TracedActor;

	start = Target.Location;
	end = start;
	end.Z -= 500;

	TracedActor = Target.Trace(HitLocation, HitNormal, end, Start, true, ,,8);
	if(TracedActor != none)
	{
		SpawnImpactDecal(Target, ImpactDecalMaterial, HitLocation, HitNormal);
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
	ImpactDecalDepth=50.0f
}