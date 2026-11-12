//
// Camera navigation collision box; when the player touches this collision box, the camera automatically transitions smoothly to this angle
//

class MMOVolume_EnableAbility extends Volume
	placeable;

// Whether abilities can be used on entering
var(MMO) bool EnterEnableAbility;

// Whether abilities can be used on exiting
var(MMO) bool ExitEnableAbility;

simulated event PostBeginPlay()
{
	super.PostBeginPlay();
}

event Touch(Actor Other, PrimitiveComponent OtherComp, vector HitLocation, vector HitNormal)
{
	local MMOPlayer player;
	local MMOPlayerController pc;

	Super.Touch(Other,OtherComp,HitLocation,HitNormal);

	player = MMOPlayer(Other);

	if(player != none)
	{
		pc = MMOPlayerController(player.Controller);
		pc.ClientCanUseAbility = EnterEnableAbility;
	}
}

event UnTouch(Actor Other)
{
	local MMOPlayer player;
	local MMOPlayerController pc;

	super.UnTouch(Other);

	player = MMOPlayer(Other);

	if(player != none)
	{
		pc = MMOPlayerController(player.Controller);
		pc.ClientCanUseAbility = ExitEnableAbility;
	}
}

defaultproperties
{
	EnterEnableAbility=false
	ExitEnableAbility=true
}
