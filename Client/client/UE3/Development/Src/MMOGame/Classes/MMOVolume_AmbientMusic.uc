//
// Ambient music volume
// Play the corresponding music after entering the volume
//

class MMOVolume_AmbientMusic extends Volume
	native
	placeable;

var() bool StopWhenUnTouch <DisplayName="Stop playing after leaving the volume">;

var() MusicTrackStruct MusicTrack;
native final function PlayTrack();
native final function StopPlayTrack();

simulated event Touch( Actor Other, PrimitiveComponent OtherComp, vector HitLocation, vector HitNormal )
{
	Super.Touch(Other, OtherComp, HitLocation, HitNormal);

	if(class'Engine'.static.IsEditor() || (MMOActor(Other) != none && MMOActor(Other).IsMainPlayer()))
	{
		PlayTrack();
	}
}

event UnTouch(Actor Other)
{
	super.UnTouch(Other);
	
	if(StopWhenUnTouch && Pawn(Other) != none && MMOActor(Other).IsMainPlayer())
	{
		StopPlayTrack();
		//OnActive(Pawn(Other), false);
	}
}

defaultproperties
{
	bStatic=False
}

/*
//var(MMO) AudioComponent AmbientAudioComponent <DisplayName="Music play component">;
//var(MMO) float FadeInDuration <DisplayName="Music fade-in time (seconds)">;
//var(MMO) float FadeOutDuration <DisplayName="Music fade-out time (seconds)">;
//var(MMO) float FadeInVolumeLevel <DisplayName="Fade-in volume percentage">;
//var(MMO) float FadeOutVolumeLevel <DisplayName="Fade-out volume percentage">;

//var(MMO) array< AudioComponent >    AlterAudioComps <DisplayName="Dynamic sound effect list">;

//var      AudioComponent             CurrAudioComp;

simulated event PawnEnteredVolume(Pawn P)
{
	super.PawnEnteredVolume(P);
	PlayTrack();
	//StartPlaying();
}

simulated event PawnLeavingVolume(Pawn P)
{	
	super.PawnLeavingVolume(P);
	//StopPlaying();
}

simulated event PhysicsChangedFor(Actor Other)
{
	//`log("@@@@@@@@@@@@@ PhysicsChangedFor == " @Other.Physics);
	super.PhysicsChangedFor(Other);
}


simulated function StartPlaying()
{
// 	AmbientAudioComponent.FadeIn( FadeInDuration, FadeInVolumeLevel );	
// 	CurrAudioComp = AmbientAudioComponent;
}

simulated function StopPlaying()
{
// 	CurrAudioComp.FadeOut( FadeOutDuration, FadeOutVolumeLevel );
// 	CurrAudioComp = none;
}

event ChangeAudio( int Idx, float FadeOutTime, float FadeInTime )
{
// 	if( idx < -1 || idx >= AlterAudioComps.Length )
// 	{
// 		`Log( "[MMO Error] Audio index out of range." );
// 		return;
// 	}
// 	if( CurrAudioComp != none )
// 	{
// 		CurrAudioComp.FadeOut( fadeOutTime, 0.f );
// 	}
// 	if( idx == -1 )
// 		CurrAudioComp = AmbientAudioComponent;
// 	else
// 		CurrAudioComp = AlterAudioComps[idx];
// 	CurrAudioComp.FadeIn( fadeInTime, 1.f );
}

defaultproperties
{
// 	Begin Object Class=AudioComponent Name=AudioComponent0
// 	End Object
// 	Components.Add(AudioComponent0)
// 	AmbientAudioComponent=AudioComponent0
// 
// 	FadeInDuration=3
// 	FadeOutDuration=3
// 	FadeInVolumeLevel=1.f
// 	FadeOutVolumeLevel=0.f

}

*/