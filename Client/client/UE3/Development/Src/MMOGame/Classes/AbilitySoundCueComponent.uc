class AbilitySoundCueComponent extends AbilityComponent
	hidecategories(Object)
	editinlinenew
	native;


enum ESoundPlayType
{
	// play weapon swing sound (determined by weapon)
	ESPT_WeaponUseSound,
	// play weapon hit sound (determined by weapon material and hit BODY material)
	ESPT_WeaponHitSound,
	// play custom sound, defined in the Sounds array
	ESPT_CustomSound,
	//...
	ESPT_ClothNoise,
	// play mining sound
	ESPT_MineHitSound,

};

var(SoundCueComponent) ESoundPlayType   SoundPlayType;
//
var(WeaponUseSound) int SoundIdx;
// if using weapon sound, this property does not need to be configured
var(CustomSound)  Array<SoundCue> Sounds;
var(CustomSound) array<SoundCueByElementaryType> SoundsByElementaryType;

var(CustomSound) SoundCue EndSound;

var(ClothNoise) ENoiseActionType ActionType;

// if the sound type is realtime, this sound takes effect in the ability editor, and the realtime sound is used in game
var(PreviewSound)   SoundCue    PreviewSound;

cpptext
{
	virtual FString GetResName();
	virtual struct BaseComponent* CreateCplusplusNode();

	UBOOL GetSoundCueByElementaryType(BYTE InType, TArray<USoundCue*>& SoundList);
}

DefaultProperties
{
	LineColor=(R=0.0,G=0.6353,B=0.9098,A=1.0)
	TitleString="Sound"
	bHaveTrail=true
}
