//
// Input management class
//
class MMOPlayerInput extends PlayerInput within MMOPlayerController
	config(Input);


function bool InputKey( int ControllerId, name Key, EInputEvent Event, float AmountDepressed = 1.f, bool bGamepad = FALSE )
{
	`log("~~~~~~~~~~~~~~~~~" @Key @Event);

	//local PlayerController		PC;
 //   local DebugCameraController	DCC;

	//foreach WorldInfo.AllControllers(class'PlayerController', PC)
	//{
	//	if ( PC.bIsPlayer && PC.IsLocalPlayerController() )
	//	{
	//		DCC = DebugCameraController(PC);
	//		if( DCC!=none && DCC.OryginalControllerRef==none )
	//		{
	//			//dcc are disabled, so we are looking for normal player controller
	//			continue;
	//		}

	//		return DCC.NativeInputKey( ControllerId, Key, Event, AmountDepressed, bGamepad );
	//	}
	//}

	return false;
}



DefaultProperties
{	
	OnReceivedNativeInputKey=InputKey
}
