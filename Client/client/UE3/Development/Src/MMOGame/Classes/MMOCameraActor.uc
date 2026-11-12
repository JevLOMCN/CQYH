class MMOCameraActor extends CameraActor;

enum ECharactorSelectCamera
{
	ECSC_Base,
	ECSC_Male,
	ECSC_Female,
	ECSC_Girl,
	ECSC_Start,
	ECSC_Selected,
	ECSC_AllNo,
};

var () bool isOnStart;
var () ECharactorSelectCamera CameraType;

var () float fTime;
var     float fCurrentTime;

simulated event PostBeginPlay()
{
	fCurrentTime = fTime;
}

simulated function GetCameraView(float DeltaTime, out TPOV OutPOV)
{
	local float percent;
	fCurrentTime -= DeltaTime;
	if( fCurrentTime <0 )
		fCurrentTime = 0;


	percent = fCurrentTime/fTime;
	//if( fCurrentTime > 0 )
	//{
	//	`log( "dsff"@percent@DeltaTime);
	//}
	//GetActorEyesViewPoint(OutPOV.Location, OutPOV.Rotation);
	OutPOV.Location = Location*(1-percent) + OutPOV.Location*percent;

	OutPOV.Rotation = Rotation*(1-percent) + OutPOV.Rotation*percent;//This interpolation seems to have a problem
	OutPOV.FOV = FOVAngle;

}

function Start()
{
	fCurrentTime = fTime;
}

DefaultProperties
{
	isOnStart=false
	CameraType=ECSC_AllNo
	fTime=20
	//fCurrentTime=fTime
}
