//
// Camera navigation collision box; when the player touches this collision box, the camera automatically transitions smoothly to this angle
//

class MMOVolume_CammeraNavigator extends PhysicsVolume
	native
	dependson(MMOPlayerCamera)
	placeable;

var(MMO) CameraActor LookAtFrom <DisplayName="CameraActor">;

var(MMO) bool ResetCameraOnPlayerExitLeave <DisplayName="ResetCameraOnPlayerExitLeave">;
var(MMO) float YawSpeedDeg <DisplayName="YawSpeedDeg">;
var(MMO) float PitchSpeedDeg <DisplayName="PitchSpeedDeg">;

var(MMO) float PullSpeed <DisplayName="PullSpeed">;
var(MMO) float PullPercent <DisplayName="PullPercent">;

var(MMO) bool       FixCamera   <DisplayName="FixCamera">;
var(MMO) Vector     ExtArea     <DisplayName="ExtArea">;
var(MMO) PathNode   TargetNode  <DisplayName="TargetNode">;

var(MMO) bool UseCustomResetDistanceRatio;
var(MMO) float CustomResetDistanceRatio;

var float YawSpeed;
var float PitchSpeed;
var float LasDistancePercent;

simulated event PostBeginPlay()
{
	super.PostBeginPlay();
	YawSpeed = YawSpeedDeg * DegToUnrRot;
	PitchSpeed = PitchSpeed * DegToUnrRot;
}

simulated event PawnEnteredVolume(Pawn P)
{
	local MMOPlayer player;
	local MMOPlayerController pc;
	local MMOPlayerCamera cam;

	super.PawnEnteredVolume(P);

	player = MMOPlayer(P);

	if(player != none)
	{
		pc = MMOPlayerController(player.Controller);
		cam = MMOPlayerCamera(pc.PlayerCamera);
		cam.CamNavVolume = self;
		cam.PitchDirty = true;
		cam.EnableSmoothCameraRot = true;
		cam.TargetDistancePullSpeed = PullSpeed;
		LasDistancePercent = cam.DistanceRatio;
		cam.DistanceRatio = PullPercent;
		//cam.EnableSmoothCameraPos = true;
		//`Log( "[Camera] Enter CamNav Volume." );
	}
}

simulated event PawnLeavingVolume(Pawn P)
{
	local MMOPlayer player;
	local MMOPlayerController pc;
	local MMOPlayerCamera cam;

	super.PawnLeavingVolume(P);

	player = MMOPlayer(P);

	if(player != none)
	{
		pc = MMOPlayerController(player.Controller);
		cam = MMOPlayerCamera(pc.PlayerCamera);

		// If it needs resetting on leaving and the current reference volume is still itself, clear it
		if(ResetCameraOnPlayerExitLeave && (cam.CamNavVolume == self))
		{
			cam.CamNavVolume = none;
			cam.PitchDirty = true;
			if(UseCustomResetDistanceRatio)
			{
				cam.DistanceRatio = CustomResetDistanceRatio;
			}
			else
			{
				cam.DistanceRatio = LasDistancePercent;
			}
			//`Log( "[Camera] Leave CamNav Volume." );
		}		
	}
}

function Vector GetLookAtPosition( Vector playerPos )
{
	local Vector pos;
	pos = playerPos;
	if( FixCamera && TargetNode != none )
	{
		if( pos.X > TargetNode.Location.X + ExtArea.X )
		{
			pos.X = TargetNode.Location.X + ExtArea.X;
		}
		else if( pos.X < TargetNode.Location.X - ExtArea.X )
		{
			pos.X = TargetNode.Location.X - ExtArea.X;
		}
		if( pos.Y > TargetNode.Location.Y + ExtArea.Y )
		{
			pos.Y = TargetNode.Location.Y + ExtArea.Y;
		}
		else if( pos.Y < TargetNode.Location.Y - ExtArea.Y )
		{
			pos.Y = TargetNode.Location.Y - ExtArea.Y;
		}
		if( pos.Z > TargetNode.Location.Z + ExtArea.Z )
		{
			pos.Z = TargetNode.Location.Z + ExtArea.Z;
		}
		else if( pos.Z < TargetNode.Location.Z - ExtArea.Z )
		{
			pos.Z = TargetNode.Location.Z - ExtArea.Z;
		}
	}
	return pos;
}

defaultproperties
{
	ResetCameraOnPlayerExitLeave=false
	YawSpeed=90
	PitchSpeed=90
	PullPercent=0.2
	PullSpeed=0.5

	FixCamera=false
	UseCustomResetDistanceRatio=false
	CustomResetDistanceRatio=1.0
}
