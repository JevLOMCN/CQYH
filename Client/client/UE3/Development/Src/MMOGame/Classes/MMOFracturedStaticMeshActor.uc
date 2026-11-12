//
// FracturedStaticMeshActor class
//

class MMOFracturedStaticMeshActor extends FracturedStaticMeshActor implements(IAbilityDamageable)
	native
	placeable;

var(MMO) DynamicLightEnvironmentComponent LightEnvironment <DisplayName="LightEnvironment">;
//var(MMO) SoundCue ExplosionSound <DisplayName="Explosion sound">;
var(MMO) PhysicalMaterial PhysMaterial <DisplayName="PhysMaterial">;

var(MMO) bool SpawnFracPartChunk <DisplayName="SpawnFracPartChunk">;

var(MMO) ParticleSystem ExplodePSTemplate <DisplayName="ExplodePSTemplate">;
var(MMO) float ExplodeEffectScale <DisplayName="ExplodeEffectScale">;

var ActorAttribute MyAttribute;

// Faction ID, AND two faction IDs; non-zero means friendly
var int FriendlyMask;

simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	//self.FracturedStaticMeshComponent.SetLightEnvironment(LightEnvironment);
	//self.FracturedStaticMeshComponent.CastShadow = true;
	//self.FracturedStaticMeshComponent.bCastDynamicShadow = true;
	//self.FracturedStaticMeshComponent.bAcceptsDynamicDominantLightShadows = true;
	//self.FracturedStaticMeshComponent.bCastDynamicShadow = true;
	MyAttribute = new class'ActorAttribute';
}

simulated event Explode()
{
	//local FracturedStaticMesh mesh;
	//local PhysicalMaterial pm;
	// Explosion effect
	local EmitterSpawnable ExplodeSFX;

	local array<byte> FragmentVis;
	local int i;
	local vector SpawnDir;
	local FracturedStaticMesh FracMesh;
	local FracturedStaticMeshPart FracPart;
	local float PartScale;

	local Sequence GameSequence;
	local array<SequenceObject> Events;

	// HACK: HACK: no more explosion after destroyed
	if(bHidden)
	{
		return;
	}

	FracMesh = FracturedStaticMesh(FracturedStaticMeshComponent.StaticMesh);

	// Iterate over all visible fragments spawning them
	FragmentVis = FracturedStaticMeshComponent.GetVisibleFragments();
	for(i=0; i<FragmentVis.length; i++)
	{
		// If this is a currently-visible, non-core fragment, spawn it off.
		if((FragmentVis[i] != 0) && (i != FracturedStaticMeshComponent.GetCoreFragmentIndex()))
		{
			if(SpawnFracPartChunk)
			{
				SpawnDir = FracturedStaticMeshComponent.GetFragmentAverageExteriorNormal(i);
				PartScale = FracMesh.ExplosionPhysicsChunkScaleMin + FRand() * (FracMesh.ExplosionPhysicsChunkScaleMax - FracMesh.ExplosionPhysicsChunkScaleMin);
				// Spawn part- inherit this actors velocity
				FracPart = SpawnPart(i, (0.5 * SpawnDir * FracMesh.ChunkLinVel) + Velocity, 0.5 * VRand() * FracMesh.ChunkAngVel, PartScale, TRUE);
			}

			if(FracPart != None)
			{
				// When something explodes we disallow collisions between all those parts.
				FracPart.FracturedStaticMeshComponent.SetRBCollidesWithChannel(RBCC_FracturedMeshPart, FALSE);
			}

			FragmentVis[i] = 0;
		}
	}

	// Update the visibility of the actor being spawned off of
	FracturedStaticMeshComponent.SetVisibleFragments(FragmentVis);
	
	//mesh = FracturedStaticMesh(FracturedStaticMeshComponent.StaticMesh);
	//pm = mesh.PhysMaterial;
	
	self.SetCollision(false, false);
	self.SetCollisionType(COLLIDE_NoCollision);


	if((PhysMaterial != none) && (PhysMaterial.FractureSoundExplosion != none))
	{
		PlaySound(PhysMaterial.FractureSoundExplosion);
	}

	TriggerGlobalEventClass( class'SeqEvent_FracturedSMAExplode', self );

	if(ExplodePSTemplate != none)
	{
		ExplodeSFX = Spawn(class'EmitterSpawnable',,,
			Location, Rotation, , true);
		ExplodeSFX.SetTemplate(ExplodePSTemplate);
		//ExplodeSFX.LifeSpan = SoundMgr.FootSetpEffectLifeTime;
		ExplodeSFX.ParticleSystemComponent.SetScale(ExplodeEffectScale);
		ExplodeSFX.ParticleSystemComponent.ActivateSystem();
	}

	//WorldInfo.Game.get
	self.SetHidden(!SpawnFracPartChunk);

	// trigger any Kismet "Destroyed" events
	GameSequence = WorldInfo.GetGameSequence();
	if (GameSequence != None)
	{
		GameSequence.FindSeqObjectsByClass(class'SeqEvent_Destroyed', true, Events);
		for (i = 0; i < Events.length; i++)
		{
			SeqEvent_Destroyed(Events[i]).CheckActivate(self, self);
		}
	}
}

// Get faction ID to calculate damage
event int GetFriendlyMask()
{
	return FriendlyMask;
}

event ProcessDamage(DamageData Damage)
{
	Explode();
}

event bool IsDead()
{
	return false;
}

// Get attack/defense attributes
function ActorAttribute GetActorAttribute()
{
	return MyAttribute;
}

DefaultProperties
{
	bStatic=false
	bNoDelete=false

	Begin Object Class=DynamicLightEnvironmentComponent Name=MyLightEnvironment
		bEnabled=TRUE
		TickGroup=TG_DuringAsyncWork
		// Using a skylight for secondary lighting by default to be cheap
		// Characters and other important skeletal meshes should set bSynthesizeSHLight=true
		//bSynthesizeSHLight=TRUE
		//bIsCharacterLightEnvironment=TRUE
		//bUseBooleanEnvironmentShadowing=FALSE
	End Object
	Components.Add(MyLightEnvironment)
	LightEnvironment=MyLightEnvironment

	SpawnFracPartChunk=true;
	ExplodeEffectScale=1

	FriendlyMask = 4
}
