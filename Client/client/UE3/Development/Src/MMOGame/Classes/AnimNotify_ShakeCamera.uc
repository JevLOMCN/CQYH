//
// animation notify screen shake
//
class AnimNotify_ShakeCamera extends AnimNotify_Scripted
	;

// screen shake animation, a pre-edited CameraAnim resource
var(MMO) CameraAnim ShakeCameraTemplate;

// screen shake time speed multiplier, actual shake time is the ShakeCameraTemplate time multiplied by this value
var(MMO) float ShakeCameraTimeScale;

event Notify( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	local Controller C;

	// TODO: check the multi-player case?
	foreach Owner.WorldInfo.AllControllers(class'Controller', C)
	{
		if ( PlayerController(C) != None )
		{
			PlayerController(C).ClientPlayCameraAnim(ShakeCameraTemplate, ShakeCameraTimeScale);
			return;
		}
	}
}

event NotifyEnd( Actor Owner, AnimNodeSequence AnimSeqInstigator )
{
	
}


DefaultProperties
{
	ShakeCameraTemplate=CameraAnim'Shake.HeavyHitShake'
	ShakeCameraTimeScale=1

	NotifyColor=(R=255,G=255,B=0,A=255)
}
