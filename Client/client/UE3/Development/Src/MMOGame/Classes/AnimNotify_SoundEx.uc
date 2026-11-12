//
// sound notify extension class
//
class AnimNotify_SoundEx extends AnimNotify_Scripted;

/*
 * search soundCue by CharacterType and MaterialType and ActionType
 * for example
 * CharacterType    : male, female, normal npc, boss, super boss
 * ActionType       : land, walksound..
 * MaterialType     : water, stone...
 */
struct SoundSearchKey
{
	var() name CharacterType;
	var() name ActionType;
	var() name MaterialType;
};

//sound played directly
var(CommonSound)	array<SoundCue> 	SoundCues;
/*identification parameters matched by object type, action, material type,
 * this cannot be set together with SoundCues; if SoundCues has sound data configured,
 * the parameters in SoundSearchKeys are ignored
 */
var(MaterialSound)  array<SoundSearchKey>   SoundSearchKeys;
// when there is a chance to play, one sound is randomly chosen from SoundCues to play
var()   float       Rate;
var()	bool		bFollowActor;
var()	Name		BoneName;
var()	bool		bIgnoreIfActorHidden;

/** This is the percent to play this Sound.  Defaults to 100% (aka 1.0f) **/
var()   float       PercentToPlay;
var()   float		VolumeMultiplier;
var()	float		PitchMultiplier;

var transient int idx;
var transient SoundCue ToPlaySound;



function int GetNewSoundCueIdx()
{
	if(Rate < FRand())
	{
		return -1;
	}
	if(SoundCues.Length <= 0)
	{
		if(SoundSearchKeys.Length <= 0)
		{
			return -1;
		}
		else
		{
			return Rand(SoundSearchKeys.Length);
		}
	}
	else
	{
		return Rand(SoundCues.Length);
	}
}

event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local MMOSoundManager smgr;
	local MMOActor actor;
	local AudioComponent audio;

	actor = MMOActor(Owner);
	smgr = MMOGame(Owner.WorldInfo.Game).SoundManager;

	if(bIgnoreIfActorHidden)
	{
		if(Owner.bHidden)
		{
			return;
		}
	}

	idx = GetNewSoundCueIdx();
	if(idx == -1)
	{
		return;
	}
	else
	{
		if(SoundCues.Length > 0)
		{
			ToPlaySound = SoundCues[idx];	
		}
		else if(SoundSearchKeys.Length > 0)
		{
			ToPlaySound = smgr.GetSoundCue(SoundSearchKeys[idx].CharacterType, SoundSearchKeys[idx].ActionType, SoundSearchKeys[idx].MaterialType);
			if(ToPlaySound == none)
			{
				`warn("can not find SoundCue by "$SoundSearchKeys[idx].CharacterType $SoundSearchKeys[idx].ActionType $SoundSearchKeys[idx].MaterialType);
				return;
			}
		}
		else
		{
			`warn("AnimNotify_SoundEx :£±£°£µ¡¡no sound date to play");
		}
	}



	if((smgr != none) && (actor != none))
	{
		smgr.PlaySoundFX(Owner, AnimSeqInstigator, self);
	}
	else
	{
		audio = Owner.CreateAudioComponent(ToPlaySound);
		if(audio != none)
		{
			if( BoneName != '' )
			{
				audio.bUseOwnerLocation	= false;
				audio.Location = AnimSeqInstigator.SkelComponent.GetBoneLocation(BoneName, 0);
			}
			else if( !(bFollowActor && (Owner != none)) )
			{	
				audio.bUseOwnerLocation	= false;
				audio.Location = Owner.Location;// AnimSeqInstigator.SkelComponent.LocalToWorld.GetOrigin();
			}
			audio.bAllowSpatialization = audio.bAllowSpatialization && !Owner.WorldInfo.bBegunPlay;
			audio.bAutoDestroy = true;
			audio.VolumeMultiplier = VolumeMultiplier;
			audio.PitchMultiplier = PitchMultiplier;
			//audio.PlaybackTime = cue.GetCueDuration() * PercentToPlay;
			audio.Play();
		}
	}
}

event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	
}

DefaultProperties
{
	NotifyColor=(R=255,G=255,B=0,A=255)

	PercentToPlay=1.0f
	VolumeMultiplier=1.f
	PitchMultiplier=1.f
	bFollowActor=TRUE
	Rate=1.f
	bIgnoreIfActorHidden=true
}
