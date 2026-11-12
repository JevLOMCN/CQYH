//
// Sound effect config management
//
class MMOSoundManager extends Object
	hidecategories(Object)
	dependson(MMOPhysicalMaterialProperty)
	native;

// Weapon sound material
// enum ESoundMaterialWeapon
// {
// 	ESMW_None <DisplayName="None">,
// 	ESMW_Blunt <DisplayName="Blunt">,
// 	ESMW_Sharp <DisplayName="Sharp">
// };


// Clothing sound material
// enum ESoundMaterialCloth
// {
// 	ESMC_None <DisplayName="None">,
// 	ESMW_Metal <DisplayName="Metal">,
// 	ESMW_Wooden <DisplayName="Wooden">,
// 	ESMW_Flesh <DisplayName="Flesh">,
// 	ESMW_Crystal <DisplayName="Crystal">,
// 	ESMW_Stone <DisplayName="Stone">,
// 	ESMW_Ghost <DisplayName="Ghost">
// };


struct native FootstepSoundInfo
{
	var() name MaterialType;
	var() SoundCue Sound;
	var() ParticleSystem FootEffect;
};

/*
 * search soundCue by CharacterType and MaterialType and ActionType
 * for example
 * CharacterType    : male, female, normal npc, boss, super boss
 * ActionType       : land, walksound..
 * MaterialType     : water, stone...
 */
struct native AnimMaterialSound
{
	var() name CharacterType;
	var() name ActionType;
	var() name MaterialType;
	var() SoundCue  Sound;
};

// footstep

/** footstep sound effect to play per material type */
var(MMO) editinline array<FootstepSoundInfo> FootstepSounds;

/** default footstep sound used when a given material type is not found in the list */
var(MMO) SoundCue DefaultFootstepSound;

var(MMO) float FootSetpEffectLifeTime;

// Default parameters
var(MMO) FootstepSoundInfo DefaultInfo;


// anim sound
var(MaterialSound) editinline array<AnimMaterialSound> AnimMaterialSounds;


// Sound effect play mode
enum ESoundFXPlayStyle
{
	ESPS_Original  <DisplayName="Original">,
	ESPS_ByCount  <DisplayName="ByCount">,
	ESPS_ByLevel  <DisplayName="ByLevel">,
	ESPS_Random  <DisplayName="Random">
};

// Info for playing sound effects by count or level
struct native SoundFXPlayStyleByLevelInfo
{
	var() int Level;
	var() SoundCue SourceSoundFX;	
};

// Sound effect info
struct native SoundFXInfo
{
	var() SoundCue SourceSoundFX;
	var() ESoundFXPlayStyle PlayStyle;
	var() int MaxPlayCount;
	var() editinline array<SoundFXPlayStyleByLevelInfo> SoundFXList;
};

// Default sound effect info parameters
var(MMO) SoundFXInfo DefaultSoundFXInfo;

// Sound effect play list
var(MMO) editinline array<SoundFXInfo> SoundFXInfoList;

// Sound effect count info
struct native SoundFXCountInfo
{
	// Sound sample
	var SoundCue SoundCue;

	// Number of times played so far
	var int Count;
};

// Count management of the current sound effect
var array<SoundFXCountInfo> SoundFXCountInfoList;

native final static function MMOSoundManager GetSoundManager();

function SoundCue GetSoundCue(name ct, name at, name mt)
{
	local int i;
	for(i = 0; i < AnimMaterialSounds.Length; ++i)
	{
		// temp no material type
		if(AnimMaterialSounds[i].CharacterType == ct && AnimMaterialSounds[i].CharacterType == at)
		{
			return AnimMaterialSounds[i].Sound;
		}
	}
	return none;
}

function PlaySoundCue(Actor Owner, SoundCue sound)
{
	if(Owner != none)
	{
		Owner.PlaySound(sound, ,,true, Owner.Location);
	}
	else
	{
		`warn("Owner is none, can not play soundCue");
	}
}

function SoundCue GetFootstepSound(int FootDown, name MaterialType)
{
	local int i;

	i = FootstepSounds.Find('MaterialType', MaterialType);
	return (i == -1 || MaterialType=='') ? DefaultInfo.Sound : FootstepSounds[i].Sound;
	// checking for a '' material in case of empty array elements
}

function FootstepSoundInfo GetFootstepInfo(int FootDown, name MaterialType)
{
	local int i;

	i = FootstepSounds.Find('MaterialType', MaterialType);
	return (i == -1 || MaterialType=='') ? DefaultInfo : FootstepSounds[i];
	// checking for a '' material in case of empty array elements
}

// Get sound effect play info
function SoundFXInfo GetSoundFXInfo(SoundCue srcCue)
{
	local int i;

	i = SoundFXInfoList.Find('SourceSoundFX', srcCue);
	return (i == -1) ? DefaultSoundFXInfo : SoundFXInfoList[i];
}

// Get sound effect play info
function SoundCue GetSoundFXCue(int count, SoundCue srcCue)
{
	local int i, j;
	local SoundCue lastCue;

	i = SoundFXInfoList.Find('SourceSoundFX', srcCue);
	if(i == -1)
	{
		return srcCue;
	}
	else
	{
		switch(SoundFXInfoList[i].PlayStyle)
		{
		case ESPS_Original:
			return srcCue;
			break;

		// Return the last sound effect greater than or equal to it by count
		case ESPS_ByCount:
			for(j = 0; j < SoundFXInfoList[i].SoundFXList.Length; ++j)
			{
				if(count > SoundFXInfoList[i].SoundFXList[j].Level)
				{
					lastCue = SoundFXInfoList[i].SoundFXList[j].SourceSoundFX;
				}
				else
				{
					return lastCue;
				}
			}
			break;

			// By level
		case ESPS_ByLevel:
			for(j = 0; j < SoundFXInfoList[i].SoundFXList.Length; ++j)
			{
				if(count > SoundFXInfoList[i].SoundFXList[j].Level)
				{
					lastCue = SoundFXInfoList[i].SoundFXList[j].SourceSoundFX;
				}
				else
				{
					return lastCue;
				}
			}
			break;

		case ESPS_Random:
			return SoundFXInfoList[i].SoundFXList[Rand(SoundFXInfoList[i].SoundFXList.Length)].SourceSoundFX;
			break;
		}
	}

	return srcCue;
}

function PlaySoundFX(Actor Owner, AnimNodeSequence AnimSeqInstigator, AnimNotify_SoundEx par)
{
	local AudioComponent audio;
	local int i, j;
	local SoundCue lastCue;
	local SoundFXCountInfo sci;

	i = SoundFXInfoList.Find('SourceSoundFX', par.ToPlaySound);
	if(i == -1)
	{
		lastCue = par.ToPlaySound;
	}
	else
	{
		switch(SoundFXInfoList[i].PlayStyle)
		{
		case ESPS_Original:
			lastCue = par.ToPlaySound;
			break;

		// Limit play count by count
		case ESPS_ByCount:
			j = SoundFXCountInfoList.Find('SoundCue', par.ToPlaySound);
			if(j != -1)
			{
				if(SoundFXCountInfoList[j].Count >= SoundFXInfoList[i].MaxPlayCount)
				{
					// Do not play if the max count is reached
					return;
				}
				
				SoundFXCountInfoList[j].Count = SoundFXCountInfoList[j].Count + 1;
			}
			else // first play
			{
				sci.SoundCue = SoundFXInfoList[i].SourceSoundFX;
				sci.Count = 1;
				SoundFXCountInfoList.AddItem(sci);
				j = 0;
			}

			lastCue = SoundFXInfoList[i].SourceSoundFX;
			`log("____________Play SFX" @lastCue @SoundFXCountInfoList[j].Count);

			break;

			// TODO: by level
		case ESPS_ByLevel:
			//for(j = 0; j < SoundFXInfoList[i].SoundFXList.Length; ++j)
			//{
			//	if(count > SoundFXInfoList[i].SoundFXList[j].Level)
			//	{
			//		lastCue = SoundFXInfoList[i].SoundFXList[j].SourceSoundFX;
			//	}
			//	else
			//	{
			//		break;
			//	}
			//}
			break;

		case ESPS_Random:
			lastCue = SoundFXInfoList[i].SoundFXList[Rand(SoundFXInfoList[i].SoundFXList.Length)].SourceSoundFX;
			break;
		}
	}

	audio = Owner.CreateAudioComponent(lastCue);
	if(audio != none)
	{
		if( par.BoneName != '' )
		{
			audio.bUseOwnerLocation	= false;
			audio.Location = AnimSeqInstigator.SkelComponent.GetBoneLocation(par.BoneName, 0);
		}
		else if( !(par.bFollowActor && (Owner != none)) )
		{	
			audio.bUseOwnerLocation	= false;
			audio.Location = Owner.Location;// AnimSeqInstigator.SkelComponent.LocalToWorld.GetOrigin();
		}
		audio.bAllowSpatialization = audio.bAllowSpatialization && !Owner.WorldInfo.bBegunPlay;
		audio.bAutoDestroy = true;
		audio.VolumeMultiplier = par.VolumeMultiplier;
		audio.PitchMultiplier = par.PitchMultiplier;
		//audio.PlaybackTime = lastCue.GetCueDuration() * PercentToPlay;
		audio.Play();
	}
}

function RestSoundFXCount()
{
	SoundFXCountInfoList.Remove(0, SoundFXCountInfoList.Length);
}

DefaultProperties
{
	FootSetpEffectLifeTime=1
	FootstepSounds[0]=(MaterialType=Stone,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_StoneCue')
	FootstepSounds[1]=(MaterialType=Dirt,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_DirtCue')
	FootstepSounds[2]=(MaterialType=Energy,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_EnergyCue')
	FootstepSounds[3]=(MaterialType=Flesh_Human,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_FleshCue')
	FootstepSounds[4]=(MaterialType=Foliage,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_FoliageCue')
	FootstepSounds[5]=(MaterialType=Glass,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_GlassPlateCue')
	FootstepSounds[6]=(MaterialType=Water,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_WaterDeepCue')
	FootstepSounds[7]=(MaterialType=ShallowWater,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_WaterShallowCue')
	FootstepSounds[8]=(MaterialType=Metal,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_MetalCue')
	FootstepSounds[9]=(MaterialType=Snow,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_SnowCue')
	FootstepSounds[10]=(MaterialType=Wood,Sound=SoundCue'A_Character_Footsteps.FootSteps.A_Character_Footstep_WoodCue')
}
