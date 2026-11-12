//=============================================================================
// DunDefEmitterSpawnable: An Actor used for one-off spawning dynamic particle effects. 
// Supports sound effects and dynamic lighting that fade in & out with the particle lifespan.
// Not particularly efficient (haha), DO NOT use for static particles, would be a waste.
//
// Author: Jeremy Stieglitz, December 2009
//=============================================================================

class MMOEmitterSpawnable extends EmitterSpawnable;

/** Light from the Emitter */
var(MMO) PointLightComponent MyPointLightComponent <DisplayName="MyPointLightComponent">;
/** How long the light should stay in existance, if enabled. 
 *  Zero keeps it active for the entire lifetime of the emitter. */
var(MMO) float LightLifeSpan <DisplayName="LightLifeSpan">;
/** How long after spawning the Light should take to interpolate up to full brightness. */
var(MMO) float LightFadeUpTime <DisplayName="LightFadeUpTime">;
/** How long BEFORE lifespan expires the Light should take to interpolate down to zero brightness. */
var(MMO) float LightFadeOutTime <DisplayName="LightFadeOutTime">;
/** A list of cues, one randomly chosen from, to play when this emitter spawns. */
var(MMO) array<SoundCue> RandomSoundToPlay <DisplayName="RandomSoundToPlay">;
/** The audio component of the Emitter. */
var(MMO) AudioComponent MyAudioComponent <DisplayName="MyAudioComponent">;
/** Whether the Emitter should stay alive until the sound is finished playing (in case the sound is longer than thee particle effect). 
 *  DO NOT use for Looping sound or the emitter will never die! */
var(MMO) bool AllowSoundToFinishBeforeDestroying <DisplayName="AllowSoundToFinishBeforeDestroying">;

var(MMO) MaterialInterface ImpactDecalMaterial <DisplayName="ImpactDecalMaterial">;
var(MMO) float ImpactDecalSizeMin <DisplayName="ImpactDecalSizeMin">;
var(MMO) float ImpactDecalSizeMax <DisplayName="ImpactDecalSizeMax">;
var(MMO) float ImpactDecalDepth <DisplayName="ImpactDecalDepth">;
/** The Lifespan to give to the decal if it's not a MITV (otherwise Lifespan is automatically calculated from the MITV's keyframes) */
var(MMO) float ImpactDecalLifespanNonMITV <DisplayName="ImpactDecalLifespanNonMITV">;


var float OriginalBrightness;
var float FadeOutStartTime;
var bool DoLightFadeDown;
var bool DoLightFadeUp;

simulated function OnParticleSystemFinished(ParticleSystemComponent FinishedComponent)
{
	//if not already fading down, start the fade down
	if(!DoLightFadeDown)
	{
		DoLightFadeDown=true;
		FadeOutStartTime = WorldInfo.TimeSeconds;
		if(MyAudioComponent.IsPlaying() && !AllowSoundToFinishBeforeDestroying)
			MyAudioComponent.FadeOut(LightFadeOutTime,0);
	}

	//stay alive just long enough to completely fade-out our light
	LifeSpan = FMax(LightFadeOutTime,LightLifeSpan) + 0.1;

	bCurrentlyActive = false;
}

/** Force deactivation of the emitter, its sound, and fade-out of its light over an optionally-specified time. */
function FadeOut(optional float theLightFadeOutTime)
{
	if(!DoLightFadeDown)
	{
		DoLightFadeUp=false;
		DoLightFadeDown=true;
		FadeOutStartTime = WorldInfo.TimeSeconds;
		if(theLightFadeOutTime > 0)
			LightFadeOutTime = theLightFadeOutTime;
		if(MyAudioComponent.IsPlaying() && !AllowSoundToFinishBeforeDestroying)
			MyAudioComponent.FadeOut(LightFadeOutTime,0);
		LifeSpan = 3;
	}

	ParticleSystemComponent.DeactivateSystem();
}

simulated event PostBeginPlay()
{
	super.PostBeginPlay();
	//start fading light up immediately upon spawning
	OriginalBrightness = MyPointLightComponent.Brightness;
	DoLightFadeUp=true;
	if(RandomSoundToPlay.Length > 0)
	{
		MyAudioComponent.SoundCue = RandomSoundToPlay[Rand(RandomSoundToPlay.Length)];
		MyAudioComponent.Play();
	}

	if(ImpactDecalMaterial != none)
	{
		SpawnImpactDecal(ImpactDecalMaterial, Location, vect(0, 0, 1), false);
	}
}

function SetSize(float size)
{
	MyPointLightComponent.Radius *= size/DrawScale;
	SetDrawScale(size);
}

event Tick(float DeltaTime)
{
	local float percentage;

	super.Tick(DeltaTime);

	//if the light is enabled, logic associated with fading up and fading down
	if(MyPointLightComponent.bEnabled)
	{
		//if we're not yet fading down, but we have a light lifespan, decrement it...
		if(LightLifeSpan > 0 && !DoLightFadeDown)
		{
			LightLifeSpan-=DeltaTime;
			//and see if we should start fading down, if we're past that point.
			if(LightLifeSpan <= LightFadeOutTime)
			{
				DoLightFadeUp=false;
				DoLightFadeDown=true;
				FadeOutStartTime = WorldInfo.TimeSeconds;
				if(MyAudioComponent.IsPlaying() && !AllowSoundToFinishBeforeDestroying)
					MyAudioComponent.FadeOut(LightFadeOutTime,0);
			}
		}

		//if we're fading up, lerp the brightness up until we're finished fading up
		if(DoLightFadeUp)
		{
			percentage = FMin((WorldInfo.TimeSeconds - CreationTime)/LightFadeUpTime,1);
			if(percentage == 1)
				DoLightFadeUp=false;

			MyPointLightComponent.SetLightProperties(OriginalBrightness*percentage);
		}

		//if we're fading down, lerp the brightness down until we're finished fading down
		if(DoLightFadeDown)
		{		
			percentage = FMin((WorldInfo.TimeSeconds - FadeOutStartTime)/LightFadeOutTime,1);
			if(percentage == 1)
			{
				DoLightFadeDown=false;
				MyPointLightComponent.SetEnabled(false);
			}
			
			MyPointLightComponent.SetLightProperties(OriginalBrightness*(1.0 - percentage));
		}
	}
}

// Generate explosion texture
function SpawnImpactDecal(MaterialInterface impactMat,
	Vector HitLocation, Vector HitNormal, bool userVelocityDir)
{
   local float SizeChange;
   local MaterialInstanceTimeVarying MITV;
   local MaterialInterface theDecal;
   local float DecalLifespan;
   local float decalOrientation;

	if(impactMat == none)
		return;

	if(userVelocityDir)
	{
		decalOrientation = 180 - Rotator(Velocity).Yaw * UnrRotToRad * RadToDeg;
		//decalOrientation  = -45;
		//`log("))))))))))))))))))" @Rotator(Velocity).Yaw*  UnrRotToRad * RadToDeg);
	}
	else
	{
		decalOrientation = FRand() * 360.0;
	}

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

		WorldInfo.MyDecalManager.SpawnDecal(MITV, HitLocation, Rotator(Normal(-HitNormal + vect(0.01, 0, 0))),
			SizeChange, SizeChange, ImpactDecalDepth, true, decalOrientation, none, true, false, , , , DecalLifespan);

		MITV.SetDuration(DecalLifespan);
		//MITV.SetScalarStartTime('DecalAlpha', 1);
		//`log("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
	}
	else
	{
		//create a decal in the World with this handy helper function!
		WorldInfo.MyDecalManager.SpawnDecal(theDecal, HitLocation, Rotator(Normal(-HitNormal + vect(0.01, 0, 0))),
			SizeChange, SizeChange, ImpactDecalDepth, true, decalOrientation, none, true, false, , , , DecalLifespan);
	}
}

DefaultProperties
{
	Begin Object Class=PointLightComponent Name=PointLightComponent0
		LightAffectsClassification=LAC_DYNAMIC_AND_STATIC_AFFECTING
	    CastShadows=FALSE
	    CastStaticShadows=FALSE
	    CastDynamicShadows=FALSE
	    bForceDynamicLight=TRUE
	    UseDirectLightMap=FALSE
	    LightingChannels=(BSP=TRUE,Static=TRUE,Dynamic=TRUE,bInitialized=TRUE)
	    Brightness = 8.0;
		LightColor=(R=255,G=255,B=255)
		Radius=500
		bEnabled=true
	End Object
	Components.Add(PointLightComponent0)
	MyPointLightComponent=PointLightComponent0

	Begin Object Class=AudioComponent Name=AudioComponent0
	End Object
	Components.Add(AudioComponent0)
	MyAudioComponent=AudioComponent0

	bDestroyOnSystemFinish=true
	LightFadeUpTime=0.1
	LightFadeOutTime=0.1
	AllowSoundToFinishBeforeDestroying=true
	
	ImpactDecalSizeMin=150
	ImpactDecalSizeMax=150
	ImpactDecalDepth=200
	ImpactDecalLifespanNonMITV=5
}