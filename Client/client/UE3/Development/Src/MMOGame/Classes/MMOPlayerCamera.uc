//
// Main camera class
//
class MMOPlayerCamera extends Camera
	native
	dependson(MMOCameraActor)
	config(MMOPlayerCamera)
	;

var config vector TraceExtend;
// Max and min camera distance
var config float FarCameraDistance;
var config float NearCameraDistance;

// YawYaw offset value
var config float CameraYawOffset;

// PitchPitch offset value
var config float FarCameraPitch;
var config float NearCameraPitch;

// Target point position offset, screen vertical value
var config float FarCameraLookAtOffset;
var config float NearCameraLookAtOffset;

// FOV at max distance
var config float FarFov;
var config float NearFov;

// Ratio of current distance
var config float DistanceRatio;

// Zoom in to what ratio
var float TargetDistancePullPercent;

// Zoom in speed
var float TargetDistancePullSpeed;
// Reference point of current camera navigation
var MMOVolume_CammeraNavigator CamNavVolume;
// Target Yaw value
var float TargetYaw;
var float TargetPitch;
var float CachedPitch;
var bool PitchDirty;

// Default rotation speed
var float DefaultYawSpeed;
var float DefaultPitchSpeed;

// Default follow speed
var config bool DefaultSmoothMove;
var config float DefaultMoveSpeed;
var config float DefaultMoveScale;
var config bool EnableDebugMode;

// Cache the previous one
var TViewTarget LastViewTarget;
var bool IsMapJustLoaded;
var bool EnableSmoothCameraPos;
var bool EnableSmoothCameraRot;

struct native AutoHideBlockActor
{
	var() Actor HideActor;
	var() MMOActorMaterialController MaterialCtrl;
};

var transient array<AutoHideBlockActor>  HiddenActorList;
var transient array<AutoHideBlockActor>  UnHiddenActorList;
var() float CheckHiddenActorTime;
var vector vLastTargetPos;

var MMOCameraActor PlayerCameraActor[6];
var MMOCameraActor CurrentCameraActor;
var bool isCameraActorImmediatly;
var TViewTarget curVT;

var config float BlockObjectFadeInOutSpeed;

var config bool EnableNearDof;
var config float NearDofFocusDistance;
var transient float FarDofFocusDistance;
var config float NearDofFocusRadius;
var transient float FarDofFocusRadius;
var config float NearDofFalloffExponent;
var transient float FarDofFalloffExponent;
var config float NearDofMaxFarBlurAmount;
var transient float FarDofMaxFarBlurAmount;
var config float NearDofFarBlurKernelSize;
var transient float FarDofFarBlurKernelSize;


var array<Material> MeshTranslucencyMaterials;

final native function CreateMaterial( MeshComponent MeshComp );


//Pow function, computed in native for speed; a is the base, b is the exponent
//The (**) operator can also be used
final native function float Pow( float a, float b);

native function UpdateDebug();


// Initialization function
simulated event PostBeginPlay()
{
	local MMOCameraActor A;
	super.PostBeginPlay();
	IsMapJustLoaded = true;
	//By default, the camera is placed directly after the map loads
	EnableSmoothCameraPos = false;
	EnableSmoothCameraRot = false;
	if(EnableDebugMode)
		`log("[MMOPlayerCamera]PostBeginPlay():EnableSmoothCameraPos" @EnableSmoothCameraPos);
	foreach  AllActors(class'MMOCameraActor', A)
	{
		//if( A.isOnStart )
		//{
			switch( A.CameraType )
			{
			case ECSC_Girl:
			case ECSC_Female:
			case ECSC_Male:
			case ECSC_Base:
			case ECSC_Start:
			case ECSC_Selected:
					PlayerCameraActor[int(A.CameraType)] = A;
				break;
			default:
				break;
			}

		//}
		
	}

	if( PlayerCameraActor[ECSC_Start] != none )
	{
		CurrentCameraActor = PlayerCameraActor[ECSC_Start];
	}
	FarDofFocusDistance = WorldInfo.DefaultPostProcessSettings.DOF_FocusDistance;
	FarDofFocusRadius   = WorldInfo.DefaultPostProcessSettings.DOF_FocusInnerRadius;
	FarDofFalloffExponent = WorldInfo.DefaultPostProcessSettings.DOF_FalloffExponent;
	FarDofMaxFarBlurAmount  = WorldInfo.DefaultPostProcessSettings.DOF_MaxFarBlurAmount;
	FarDofFarBlurKernelSize  = WorldInfo.DefaultPostProcessSettings.DOF_BlurKernelSize;
}



function ChangeStartCameraActor( ECharactorSelectCamera CameraType,bool isImmediatly )
{
		isCameraActorImmediatly = isImmediatly;
		CurrentCameraActor = PlayerCameraActor[CameraType];
		CurrentCameraActor.Start();
	
}

function ChangeShotCameraMode(ECharactorSelectCamera CameraType,float h)
{
	local vector loc;
	loc = CurrentCameraActor.Location;
	loc.Z = h;
			isCameraActorImmediatly = true;
		CurrentCameraActor = PlayerCameraActor[CameraType];
		if( h>0)
			CurrentCameraActor.SetLocation(loc);
}



//---------------------------------------------------------------
// UpdateViewTarget() method description
// This class inherits from the base Camera class.
// When this class is created, the base GamePawn virtual method GetDefaultCameraMode (must be overridden) is called automatically (ps. in UE3 there are no virtual functions, extends is always an override and the method already exists; check the base uc file to see how it is written).
// To create a new camera mode, override GamePawn's GetDefaultCameraMode method.
// Camera Mode var  Name  CameraStyle;
// The CameraStyle member variable in the Camera class is implemented via GamePawn's GetDefaultCameraMode. (ps. I think Set would be a better name, but it is not a method, it must be inherited, so Get is also fine)
// In my derived GamePawn class (KWPawn) I return Isometric, so in Camera.uc find the UpdateViewTarget method and copy it over (copying is optional; the benefit is that the original camera mode is kept)
// Add handling for Isometric where switch( CameraStyle ) is (Isometric can be named anything, but whatever you case on must match what you Get in your Pawn)
// Can be set in this class
// This is the basic functionality of an easy-to-use, quality-assured camera class; it adds to or extends the functionality of another GameCamera of the same level
//---------------------------------------------------------------

native final function BeforeUpdateTick(float DeltaTime);
native final function AfterUpdateTick(float DeltaTime);

native final function EnableUpdatePlayerHUD(bool on);
/**
 * Query ViewTarget and outputs Point Of View.
 *
 * @param	OutVT		ViewTarget to use.
 * @param	DeltaTime	Delta Time since last camera update (in seconds).
 */
function UpdateViewTarget(out TViewTarget OutVT, float DeltaTime)
{
	local vector		        Loc, Pos, HitLocation, HitNormal;
	local rotator		        Rot;
	local Actor			    HitActor;
	local CameraActor  CamActor;
	local TPOV			    OrigPOV;
	//local bool                bSmooth;

	// Don't update outgoing viewtarget during an interpolation 
	//if( PendingViewTarget.Target != None && OutVT == ViewTarget && BlendParams.bLockOutgoing )
	//{
	//	super.UpdateViewTarget(OutVT, DeltaTime);
	//	return;
	//}
	// store previous POV, in case we need it later
	BeforeUpdateTick(DeltaTime);

	UpdateDebug();
	OrigPOV = OutVT.POV;

	// Default FOV on viewtarget
	OutVT.POV.FOV = DefaultFOV;

	// Viewing through a camera actor.
	CamActor = CameraActor(OutVT.Target);

	if(CurrentCameraActor != none)
		CamActor = CurrentCameraActor;
	//If a camera Actor is currently specified, skip the local calculation directly
	if( CamActor != None )
	{
		if( isCameraActorImmediatly )
		{
			OutVT.POV.Rotation = CamActor.Rotation;
			OutVT.POV.Location = CamActor.Location;
			OutVT.POV.FOV =CamActor.FOVAngle;
		}
		else
				CamActor.GetCameraView(DeltaTime, OutVT.POV);

		//`log("[MMOPlayerCamera]UseAcotr2UpdateViewTarget():" @CamActor.Name);
		// Grab aspect ratio from the CameraActor.
		bConstrainAspectRatio	= bConstrainAspectRatio || CamActor.bConstrainAspectRatio;
		OutVT.AspectRatio		= CamActor.AspectRatio;

		// See if the CameraActor wants to override the PostProcess settings used.
		CamOverridePostProcessAlpha = CamActor.CamOverridePostProcessAlpha;
		CamPostProcessSettings = CamActor.CamOverridePostProcess;
	}
	else
	{
		// Give Pawn Viewtarget a chance to dictate the camera position.
		// If Pawn doesn't override the camera view, then we proceed with our own defaults
		if( Pawn(OutVT.Target) == None ||
			!Pawn(OutVT.Target).CalcCamera(DeltaTime, OutVT.POV.Location, OutVT.POV.Rotation, OutVT.POV.FOV) )
		{
			if(TargetDistancePullPercent != DistanceRatio)
			{
				EnableUpdatePlayerHUD(true);
				if(TargetDistancePullPercent > DistanceRatio)
				{
					if(TargetDistancePullPercent - DistanceRatio < TargetDistancePullSpeed * DeltaTime)
					{
						TargetDistancePullPercent = DistanceRatio;
					}
					else
					{
						TargetDistancePullPercent -= TargetDistancePullSpeed * DeltaTime;
					}
				}
				else if(TargetDistancePullPercent < DistanceRatio)
				{
					if(DistanceRatio - TargetDistancePullPercent < TargetDistancePullSpeed * DeltaTime)
					{
						TargetDistancePullPercent = DistanceRatio;
					}
					else
					{
						TargetDistancePullPercent += TargetDistancePullSpeed * DeltaTime;
					}
				}
			}
			// don't apply modifiers when using these debug camera modes.
				
			switch( CameraStyle )
			{
			case 'Fixed'		:	// do not update, keep previous camera position by restoring
				// saved POV, in case CalcCamera changes it but still returns false
				OutVT.POV = OrigPOV;
				break;

			case 'ThirdPerson'	: // Simple third person view implementation
			case 'FreeCam'		:
			case 'FreeCam_Default':
				Loc = OutVT.Target.Location;
				Rot = OutVT.Target.Rotation;

				//OutVT.Target.GetActorEyesViewPoint(Loc, Rot);
				if( CameraStyle == 'FreeCam' || CameraStyle == 'FreeCam_Default' )
				{
					Rot = PCOwner.Rotation;
				}
				Loc += FreeCamOffset >> Rot;


				Pos = Loc - Vector(Rot) * TargetDistancePullPercent;
				// @fixme, respect BlockingVolume.bBlockCamera=false
				HitActor = Trace(HitLocation, HitNormal, Pos, Loc, FALSE, vect(12,12,12));
				OutVT.POV.Location = (HitActor == None) ? Pos : HitLocation;
				OutVT.POV.Rotation = Rot;

				break;

				//---------------------------------------------------------------
				// Custom camera mode begin
				//---------------------------------------------------------------

				//Comes from GetDefaultCameraMode of the custom Pawn class (MMOPlayer)
			case 'Isometric':
				//If an ability is currently being cast, it may carry a camera animation, so temporarily disable smooth follow
				if( ActiveAnims.Length > 0 && EnableSmoothCameraPos )
				{
					if(EnableDebugMode)
						`log("[MMOPlayerCamera]:PlayingAinm,Follow immediately" @ActiveAnims.Length  );
					//bSmooth=EnableSmoothCameraPos;
					EnableSmoothCameraPos=false;
				}
				//When the ability finishes, restore the default camera follow state
				else if( EnableSmoothCameraPos != DefaultSmoothMove )
				{
					EnableSmoothCameraPos = DefaultSmoothMove;
				}
				//Modify camera orientation
				Rot = CalcRotation( DeltaTime );

				//Modify camera position
				Pos = CalcPosition( Rot, CalcLookAtOffset( Rot ), DeltaTime );

				OutVT.POV.Location = Pos;
				OutVT.POV.Rotation = Rot;
				OutVT.POV.FOV = GetCurrentFOV();

				WorldInfo.DefaultPostProcessSettings.DOF_FocusDistance    = GetDofFocusDistance();
				WorldInfo.DefaultPostProcessSettings.DOF_FocusInnerRadius = GetDofFocusRadius();
				WorldInfo.DefaultPostProcessSettings.DOF_FalloffExponent  = GetDofFalloffExponent();
				WorldInfo.DefaultPostProcessSettings.DOF_MaxFarBlurAmount = GetDofFarMaxFarBlurAmount();
				WorldInfo.DefaultPostProcessSettings.DOF_BlurKernelSize   = GetDofFarBlurKernelSize();
/*
				if( ActiveAnims.Length > 0 )
				{
					EnableSmoothCameraPos=bSmooth;
				}
*/

				break;

				//---------------------------------------------------------------
				// Custom camera mode end
				//---------------------------------------------------------------

			case 'FirstPerson'	: // Simple first person, view through viewtarget's 'eyes'
			default				:	
				OutVT.Target.GetActorEyesViewPoint(OutVT.POV.Location, OutVT.POV.Rotation);
				break;

			}
		}
	}
	curVT = OutVT;

	//After the map is loaded, use the smooth zoom-in camera
	if(IsMapJustLoaded)
	{   
		IsMapJustLoaded=false;
		//Whether it is enabled differs a lot, so configure it in the Ini
		EnableSmoothCameraPos=DefaultSmoothMove;
		EnableSmoothCameraRot=true;
		if(EnableDebugMode)
			`log("[MMOPlayerCamera]UpdateViewTarget():EnableSmoothCameraPos" @EnableSmoothCameraPos);
	}
	ApplyCameraModifiers(DeltaTime, OutVT.POV);

	if(PCOwner != none)
	{
		PCOwner.SetLocation(OutVT.POV.Location);
		PCOwner.SetRotation(OutVT.POV.Rotation);
	}

	AfterUpdateTick(DeltaTime);
}

// Zoom camera in
native final function ZoomIn();
// Zoom camera out
native final function ZoomOut();


// Get current Dof focus distance
function float  GetDofLerpParameter()
{
	local bool inMatinee;
	local float par;
	inMatinee = IsInMatinee();
	par       = ( !EnableNearDof && inMatinee ) ? 1.0f : TargetDistancePullPercent;
	return par;
}

function float GetDofFarBlurKernelSize()
{
	local float par;
	par  = GetDofLerpParameter();
	return NearDofFarBlurKernelSize + ( FarDofFarBlurKernelSize - NearDofFarBlurKernelSize )  * par;
}

function float GetDofFarMaxFarBlurAmount()
{
	local float par;
	par  = GetDofLerpParameter();
	return NearDofMaxFarBlurAmount + ( FarDofMaxFarBlurAmount - NearDofMaxFarBlurAmount )  * par;
}

function float GetDofFocusDistance()
{
	local float par;
	par  = GetDofLerpParameter();
	return NearDofFocusDistance + ( FarDofFocusDistance - NearDofFocusDistance )  * par;
}

// Get current Dof focus radius
function float GetDofFocusRadius()
{
	local float par;
	par  = GetDofLerpParameter();
	return NearDofFocusRadius + ( FarDofFocusRadius - NearDofFocusRadius )  * par;
}

// Get current dof falloff coefficient
function float GetDofFalloffExponent()
{
	local float par;
	par  = GetDofLerpParameter();
	return NearDofFalloffExponent + ( FarDofFalloffExponent - NearDofFalloffExponent )  * par;
}

// Get current FOV, linearly interpolated by near/far parameter
function float GetCurrentFOV()
{
	return NearFov + (FarFOV - NearFov) * TargetDistancePullPercent;
}

// Get current distance offset
native final function float GetCurrentDistance();
// {
// 	return TargetDistancePullPercent * (FarCameraDistance -NearCameraDistance) + NearCameraDistance;
// }

// Get current up/down look at offset
function float GetCurrentLookAtOffset()
{
	return -(NearCameraLookAtOffset + (FarCameraLookAtOffset - NearCameraLookAtOffset) * TargetDistancePullPercent);
}

// Get current PITCH, linearly interpolated by near/far parameter
function int GetTargetPitch()
{
	//if(PitchDirty)
	//{
		if( CamNavVolume != none && CamNavVolume.LookAtFrom != none )
		{
			CachedPitch = CamNavVolume.LookAtFrom.Rotation.Pitch;
		}
		else
		{
			CachedPitch = -(NearCameraPitch + (FarCameraPitch - NearCameraPitch) * TargetDistancePullPercent) * DegToUnrRot;
		}

		if(TargetDistancePullPercent == DistanceRatio)
		{
			//PitchDirty = false;
		}
	//}

	return CachedPitch;
}

// Get current YAW, linearly interpolated by near/far parameter
function int GetTargetYaw()
{
	local int curYaw;

	if( CamNavVolume != none && CamNavVolume.LookAtFrom != none )
	{
		curYaw = CamNavVolume.LookAtFrom.Rotation.Yaw;
	}
	else
	{
		curYaw = CameraYawOffset * DegToUnrRot;
	}

	return curYaw;
}

function float GetPitchSpeed()
{
	if( CamNavVolume != none )
		return CamNavVolume.PitchSpeed;
	return DefaultPitchSpeed;
}

function float GetYawSpeed()
{
	if( CamNavVolume != none )
		return CamNavVolume.YawSpeed;
	return DefaultYawSpeed;
}

function Rotator CalcRotation( float DeltaTime )
{
	local bool  dirty;
	local Rotator Rot;
	local float deltaPitch, deltaYaw, accuPitch, accuYaw;

	Rot.Roll  = 0.f;
	Rot.Yaw = GetTargetYaw()&0xFFFF; 
	if(Rot.Yaw < 0) 
		Rot.Yaw += 65536;
	Rot.Pitch = GetTargetPitch() & 0xFFFF; 
	if(Rot.Pitch < 0) 
		Rot.Pitch += 65536;

	if( EnableSmoothCameraRot )
	{
		dirty = false;

		if(CameraCache.POV.Rotation.Yaw < 0 || CameraCache.POV.Rotation.Yaw > 65536)
		{
			CameraCache.POV.Rotation.Yaw = CameraCache.POV.Rotation.Yaw%0xFFFF;
			if(CameraCache.POV.Rotation.Yaw < 0)
			{
				CameraCache.POV.Rotation.Yaw += 65536;
			}
		}

		if(CameraCache.POV.Rotation.Pitch < 0 || CameraCache.POV.Rotation.Pitch > 65536)
		{
			CameraCache.POV.Rotation.Pitch = CameraCache.POV.Rotation.Pitch%0xFFFF;
			if(CameraCache.POV.Rotation.Pitch < 0)
			{
				CameraCache.POV.Rotation.Pitch += 65536;
			}
		}

		deltaPitch = Rot.Pitch - CameraCache.POV.Rotation.Pitch;
		deltaYaw = Rot.Yaw - CameraCache.POV.Rotation.Yaw;


		if(deltaPitch > 32768)
		{
			deltaPitch -= 65536;
		}
		else if(deltaPitch < -32768)
		{
			deltaPitch += 65536;
		}
		if(deltaYaw > 32768)
		{
			deltaYaw -= 65536;
		}
		else if(deltaYaw < -32768)
		{
			deltaYaw += 65536;
		}

		accuPitch = GetPitchSpeed() * DeltaTime;
		if( abs( deltaPitch ) > accuPitch )
		{
			if( deltaPitch < 0 )
			{
				Rot.Pitch = CameraCache.POV.Rotation.Pitch - accuPitch;
			}
			else
			{
				Rot.Pitch = CameraCache.POV.Rotation.Pitch + accuPitch;
			}
			dirty = true;
		}
		accuYaw = GetYawSpeed() * DeltaTime;
		if( abs( deltaYaw ) > accuYaw )
		{
			if( deltaYaw < 0 )
				Rot.Yaw = CameraCache.POV.Rotation.Yaw - accuYaw;
			else
				Rot.Yaw = CameraCache.POV.Rotation.Yaw + accuYaw;
			dirty = true;
		}

		if( !dirty )
		{
			PitchDirty = false;
		}
	}
	return Rot;
}

function Vector CalcLookAtOffset( Rotator CurrRot )
{
	local Rotator offsetRot;
	local Vector lookat;

	if( CamNavVolume != none && CamNavVolume.FixCamera )
	{
		lookat = CamNavVolume.GetLookAtPosition( PCOwner.Pawn.Location );
	}
	else
	{
		offsetRot.Yaw = CurrRot.Yaw;
		offsetRot.Pitch = 0;
		offsetRot.Roll = 0;

		if(PCOwner.Pawn != none)
		{
			lookat = PCOwner.Pawn.Location 
				+ Vector(offsetRot) * GetCurrentLookAtOffset();
		}
		//`Log("[CalcLookAtOffset]"@offsetRot.Yaw @GetCurrentLookAtOffset());
	}

	return lookat;
}

function Vector CalcPosition( Rotator CurrRot, Vector LookAt, float DeltaTime )
{
	local Vector Pos, Diff,nDiff,RotPos;
	local float fDiffLength;
	local float accuMove;
	local float fRealMoveSpeed;
	local LinearColor LColor;
	local Color C;
	//LooAt is the position of the target the camera is currently looking at, Pos is the desired camera position
	Pos = LookAt - Vector(CurrRot) * GetCurrentDistance();

	//`log("_______ CalcPosition");
	// Only smooth follow when not zooming the camera
	if( EnableSmoothCameraPos && (DistanceRatio == self.TargetDistancePullPercent))
	{
		//`log("_______ EnableSmoothCameraPos");

		Diff = Pos - CameraCache.POV.Location;
		fDiffLength=vsize( Diff );
		if( fDiffLength < 1.0f )
			return Pos;

		if(fDiffLength > 256.0f)		
		{
			return Pos;
		}

		fRealMoveSpeed = fDiffLength*DefaultMoveScale+DefaultMoveSpeed;
		accuMove = DeltaTime * fRealMoveSpeed;
		if( ( accuMove < fDiffLength ) && ( accuMove > 1.0f ) && ( fRealMoveSpeed > 0.0f ) )
		{
			if( EnableDebugMode && vLastTargetPos != Pos )
			{
				LColor.A = 1.0;
				LColor.R = 1.0;
				LColor.G = 0;
				LColor.B = 0;
				DrawDebugPoint(LookAt, 5, LColor, true);
				LColor.A = accuMove/fDiffLength;
				LColor.R = 0.0;
				LColor.G = 1.0;
				//Desired position to move to
				RotPos = Pos+Normal( Vector( CurrRot ) )*1400;
				DrawDebugLine (Pos,RotPos,0,255,0,true);
				DrawDebugPoint(Pos, 10, LColor, true);
				C.R = 255;
				C.G = 0;
				C.B = 255;
				DrawDebugCone(Pos, Normal( Vector( CurrRot ) ), 60, 3.0, 3.0,32,C, true);
			}
			nDiff = normal(Diff);
			Pos=CameraCache.POV.Location+accuMove*nDiff;
			if( EnableDebugMode )
			{
				//LColor.R =RandRange(0,1);
				LColor.G=0;
				LColor.B = 1.0;
				//Actual position moved to
				C.R = 255;
				C.G = 255;
				C.B = 0;
				C.A = 255;
				DrawDebugCone(Pos, nDiff,10, 3.0, 3.0,32,C, true);
				DrawDebugPoint(Pos, 5, LColor, true);
			}
			if( EnableDebugMode )
			{   		
				`Log("[MMOPlayerCamera]DiffLen_RealSpeed_AccuMove:"@fDiffLength@fRealMoveSpeed@accuMove@"Pos_Dir:"@Pos@nDiff);
				`Log("[BeginPos:]"@CameraCache.POV.Location);
			}
		}
		else
		{
			//If the speed is too high, skip the smoothing calculation and move directly to the target position
			if( EnableDebugMode )
				`Log("[MMOPlayerCameraJump] fDiffLength fRealMoveSpeed accuMove" @accuMove@fDiffLength@fRealMoveSpeed);
		}
	}
	vLastTargetPos=Pos;

	return Pos;
}

unreliable client event ClientPlayCameraAnim( CameraAnim AnimToPlay, optional float Scale=1.f, optional float Rate=1.f,
						optional float BlendInTime, optional float BlendOutTime, optional bool bLoop,
						optional bool bRandomStartTime, optional ECameraAnimPlaySpace Space=CAPS_CameraLocal, optional rotator CustomPlaySpace )
{
	local CameraAnimInst AnimInst;
/*
	`Log("[MMOPlayer]:Stop play Anim!");
	return;*/
	AnimInst = PlayCameraAnim(AnimToPlay, Rate, Scale, BlendInTime, BlendOutTime, bLoop, bRandomStartTime);
	if (AnimInst != None && Space != CAPS_CameraLocal)
	{
		AnimInst.SetPlaySpace(Space, CustomPlaySpace);
	}
}

//function ClearHiddenList()
//{
//	local int i;
//	for(i = 0; i < HiddenActorList.Length; ++i)
//	{
//// 		HiddenActorList[i].SetHidden(false);
//// 		HiddenActorList[i].SetCollision(true, true, true);
//	}
//}

native final function bool HasHidden(Actor actor);
native final function bool IsUnHiding(Actor actor, out int idx);
native final function CheckStaticMeshToHide(float deltaTime);
native final function NativeTick(float deltaTime);
native final function UnHiddenAll();
native final function bool IsInMatinee();

event Tick(float deltaTime)
{
	super.Tick(deltaTime);
	NativeTick(deltaTime);
}

DefaultProperties
{
	DefaultYawSpeed=16384
	DefaultPitchSpeed=16384
	PitchDirty=true
	CheckHiddenActorTime=0.2f

	//DefaultCameraActor=none
	isCameraActorImmediatly=true
	TargetDistancePullSpeed=0.5
	EnableSmoothCameraPos=true
	EnableSmoothCameraRot=true

	CameraStyle=Isometric

	//NearDofFocusDistance = 700
	//NearDofFocusRadius    = 1200
}
