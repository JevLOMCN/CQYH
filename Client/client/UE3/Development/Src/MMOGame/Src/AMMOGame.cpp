#include "stdafx.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOGameEvent_Splash.h"
using namespace ExternalMessage;

IMPLEMENT_CLASS(AMMOGameReplicationInfo)

void AMMOGame::PreBeginPlayNative()
{
    DefaultLocalMapName = FFilename(GConfig->GetStr( TEXT("URL"), TEXT("LocalMap"),	GEngineIni )).GetBaseFilename();
}

/*
ANavigationPoint* AMMOGame:: FindPlayerStart( class AController* Player, BYTE InTeam, const FString& IncomingName )
{
    //ANavigationPoint *navPoint;
    //`Log( "[MMO Game Deubg] FindPlayerStart:" @InTeam @IncomingName );
    if( !WorldInfo->IsPlayInEditor() && PlayerStartPoint != "" )
    {
        / *for( TObjectIterator<ANavigationPoint> it; it; ++it )
        {
            if( (*it)->Tag.ToString()  == PlayerStartPoint )
            {
                //	PlayerStartPoint = "PlayerStart_0";
                return (*it);
            }
        }* /
        for (INT i = 0; i < GWorld->Levels.Num(); ++i)
        {
            ULevel* level = GWorld->Levels(i);
            for (INT j = 0; j < level->Actors.Num(); ++j)
            {
                AActor* actor = level->Actors(j);
                if(actor)
                {				
                    if(actor->IsA(APlayerStart::StaticClass()))
                    {
                        return static_cast< ANavigationPoint* >( actor );
                    }
                }
            }
        }
    }
    //`Log( "[MMO Deubg] No player start spot:"@ PlayerStartPoint );
    return AGameInfo::eventFindPlayerStart( Player, InTeam, IncomingName );
}
*/

void AMMOGame::SetMainController(class AMMOPlayerController* ctrl)
{
#ifdef _DEBUG
	if(ctrl == NULL)
	{
		warnf(TEXT("[AMMOGame::SetMainController] player controller is NULL"));
		appDebugBreak();
	}
#endif
	MainController = ctrl;
}

class AMMOPlayerController* AMMOGame::GetMainController() const
{
	return MainController;
}

extern void PreloadNPCMesh();

void AMMOGame::SendRequest_FinishLoading()
{
	SET_WARN_COLOR(COLOR_PURPLE);
	warnf(NAME_Log, TEXT("AMMOGame::SendRequest_FinishLoading"));
	CLEAR_WARN_COLOR();

	MMOFacade::getSingleton().ClearAllCharacter();
	/*CMessageHandler::GetInstance().Send_LoadFinish();*/
	SetCameraParameters();  //Bruce@snda 
	MMOFacade::getSingleton().GetMainController()->GotoState(FName(TEXT("Idle")));	

// 	AMMOPlayerController* playerController = MMOFacade::getSingleton().GetMainController() ;
// 	if( NULL != playerController )
// 	{
// 		if( playerController->DropItemMgr )
// 			playerController->DropItemMgr->CleanAll();
// 	}
	if(M_MMO_HD)
	{
		PreloadNPCMesh();
	}

	MMO::ActiveGameState(EGameState_InGame);
	//
	MMOGameEvent_Any evt( GameEvent_FinishLoading );
	MMOUI::PostGameEvent(&evt);

	CMessageHandler::GetInstance().Send_LoadFinish();
	CMessageHandler::GetInstance().Send_HardwareInfo();
}

//After map loading completes, jump according to destination
void AMMOGame::FinishLoading2Login()
{
    MMO::ActiveGameState( EGameState_Login );
}

void AMMOGame::FinishLoading2Select()
{

	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, TRUE, "");

	//MMOGameEvent_Splash loginEvt;
	//loginEvt.EvtType = MMOGameEvent_Splash::ESET_CreateListRoleCheck;
	//MMOUI::PostGameEvent(&loginEvt);
	if(CMessageHandler::GetInstance().IsConnection()){
	    MMO::ActiveGameState( EGameState_Selecting );
	}
	else
	{
		MMO::ActiveGameState( EGameState_Login );
	}
}

FVector AMMOGame::GetCurrentTeleportPosition()
{
	MMOFacade::getSingleton().CorrectTeleportPos();
	FVector rst = MMOFacade::getSingleton().GetInitTeleportPos();
	return rst;
}

void AMMOGame::EnableDeathEffects( UBOOL bEnable )
{
	if( MMOFacade::getSingleton().GetMainController() == NULL )
		return;

	ULocalPlayer* lp = Cast<ULocalPlayer>( MMOFacade::getSingleton().GetMainController()->Player );
	if( lp == NULL )
		return;

	for( INT EffectIdx = 0; EffectIdx < lp->PlayerPostProcess->Effects.Num(); ++EffectIdx )
	{
		UMaterialEffect* MaterialEffect = Cast<UMaterialEffect>(lp->PlayerPostProcess->Effects(EffectIdx));

		if( MaterialEffect && MaterialEffect->Material && MaterialEffect->EffectName == TEXT("DeathEffect") )
		{
			MaterialEffect->bShowInGame = bEnable;
			break;
		}
	}

	// this old method doesn't work for low level settings
	//if( bEnable )
	//{
	//	DefaultSceneDesaturation = GWorld->GetWorldInfo()->DefaultPostProcessSettings.Scene_Desaturation;
	//	GWorld->GetWorldInfo()->DefaultPostProcessSettings.Scene_Desaturation = 1.f;
	//}
	//else
	//{
	//	GWorld->GetWorldInfo()->DefaultPostProcessSettings.Scene_Desaturation = DefaultSceneDesaturation;
	//}
}

void AMMOGame::NativePostSeamlessTravel()
{
#ifdef _DEBUG
	UBOOL isEditor = GIsEditor;
	FString name = GWorld->GetName();
#endif

	g_updateMainPlayerHudName = true;
}

void AMMOGame::NativeResetLevel()
{
	MMO::UpdateAllServerSceneVars();
}

//Bruce@snda 
void AMMOGame::SetCameraParameters()
{
	AWorldInfo*	WorldInfo = GWorld->GetWorldInfo();
	AMMOPlayerController* MMOCtrl = GetMainController();

	if ( WorldInfo && MMOCtrl )
	{
		AMMOPlayerCamera* PCCam = Cast<AMMOPlayerCamera>( MMOCtrl->PlayerCamera );

		if ( PCCam )
		{
			if(WorldInfo->NearFov != WorldInfo->GetClass()->GetDefaultObject<AWorldInfo>()->NearFov)
			{
				PCCam->NearFov = WorldInfo->NearFov;
			}

			if(WorldInfo->FarFov != WorldInfo->GetClass()->GetDefaultObject<AWorldInfo>()->FarFov)
			{
				PCCam->FarFov = WorldInfo->FarFov;
			}

			if(WorldInfo->NearCameraPitch != WorldInfo->GetClass()->GetDefaultObject<AWorldInfo>()->NearCameraPitch)
			{
				PCCam->NearCameraPitch = WorldInfo->NearCameraPitch;
			}

			if(WorldInfo->FarCameraPitch != WorldInfo->GetClass()->GetDefaultObject<AWorldInfo>()->FarCameraPitch)
			{
				PCCam->FarCameraPitch = WorldInfo->FarCameraPitch;
			}

			if(WorldInfo->NearCameraDistance != WorldInfo->GetClass()->GetDefaultObject<AWorldInfo>()->NearCameraDistance)
			{
				PCCam->NearCameraDistance = WorldInfo->NearCameraDistance;
			}

			if(WorldInfo->FarCameraDistance != WorldInfo->GetClass()->GetDefaultObject<AWorldInfo>()->FarCameraDistance)
			{
				PCCam->FarCameraDistance = WorldInfo->FarCameraDistance;
			}
		}
	}
}
//Bruce@snda 



void AMMOGame::StartLongJump( FVector TargetLocation, UBOOL NotifyServerWhenFinish, UBOOL SameScene )
{
	//SuspendRenderingThread = new FSuspendRenderingThread( FSuspendRenderingThread::ST_SuspendThread );

	if( GWorld->ProcessLevelStreamingVolumes( &TargetLocation, TRUE ) )
	{
		// has new level to load, block the location setting until the jump is done
		IsLongJumping = TRUE;
		NotifyServerAfterLongJump = NotifyServerWhenFinish;
		LongJumpTargetLocation = TargetLocation;
	}
	else
	{	
		// no level to load, just set the location
		AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
		if( actor )
		{
			actor->SetLocation( TargetLocation );
		}
		if( NotifyServerWhenFinish )
		{
			SendRequest_FinishLoading();
		}
	}

	JumpInSameScene = SameScene;
}

void AMMOGame::EndLongJump()
{
	if( !IsLongJumping )
		return;

	//delete SuspendRenderingThread;
	//SuspendRenderingThread = NULL;

	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	if( actor )
	{
		actor->SetLocation( LongJumpTargetLocation );
	}

	IsLongJumping = FALSE;

	if( NotifyServerAfterLongJump )
	{
		SendRequest_FinishLoading();
		NotifyServerAfterLongJump = FALSE;
	}
}

void AMMOGame::NotifyAsyncLoadingFinished()
{
	SET_WARN_COLOR(COLOR_PURPLE);
	warnf(NAME_Log, TEXT("AMMOGame::NotifyAsyncLoadingFinished"));
	CLEAR_WARN_COLOR();

	Super::NotifyAsyncLoadingFinished();

	// set player position
	//AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	//if( actor && MMOFacade::getSingleton().DelayPositionSetting )
	//{
	//	actor->SetLocation( MMOFacade::getSingleton().GetInitTeleportPos() );
	//	MMOFacade::getSingleton().DelayPositionSetting = FALSE;
	//		
	//	////if( MMOFacade::getSingleton().GetMainController()->OnRevive )
	//	//{	// tell the controller revive state is clear, so it can accept new position setting
	//	//	MMOFacade::getSingleton().GetMainController()->OnRevive = FALSE;
	//	//}
	//}

	EndLongJump();
}

void LoadingCompleted()
{
	//if( GWorld->GetWorldInfo()->Game )
	//	GWorld->GetWorldInfo()->Game->NotifyAsyncLoadingFinished();
}