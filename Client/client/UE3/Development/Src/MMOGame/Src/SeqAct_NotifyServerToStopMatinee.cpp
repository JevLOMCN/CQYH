#include "..\stdafx.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(USeqAct_NotifyServerToStopMatinee)


void USeqAct_NotifyServerToStopMatinee::Activated()
{
	CMessageHandler::GetInstance().Send_CancelMatinee();
	ActivateOutputLink(0);

	// Restore the camera when the cutscene ends
	AMMOPlayerCamera* cam = Cast<AMMOPlayerCamera>(MMOFacade::getSingleton().GetMainController()->PlayerCamera);
	if(cam)
	{
		cam->EnableSmoothCameraPos = FALSE;
		cam->EnableSmoothCameraRot = FALSE;
		cam->eventUpdateCamera(0.0f);
	}
}
