#include "stdafx.h"
//#include "MMOGameClasses.h"
#include "MMOUtil.h"
#include "net/message_handler.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"
#include "TeamManager.h"
#include "FSMIO.h"

IMPLEMENT_CLASS(ATeleportOrig);
IMPLEMENT_CLASS(ATeleportTarget);
IMPLEMENT_CLASS(ATeleportNetNode);

FVector ATeleportOrig::GetServerPosition()
{
	FVector result = Location;
	ConvertCoordC2S(result);
	result.X = (INT32)result.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	result.Y = (INT32)result.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	return result;
}


FVector ATeleportTarget::GetServerPosition()
{
	FVector result = Location;
	ConvertCoordC2S(result);
	result.X = (INT32)result.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	result.Y = (INT32)result.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	return result;
}

FVector ATeleportNetNode::GetServerPosition()
{
	FVector result = Location;
	ConvertCoordC2S(result);
	result.X = (INT32)result.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	result.Y = (INT32)result.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	return result;
}
//-------------------------------------------------

//void ATeleportOrig::BeginDestroy()
//{
//	MMOTargetManager::getSingleton().DeleteActor(this);
//}

FVector ATeleportOrig::GetInteractActorLocation()
{
	return Location;
}

FLOAT ATeleportOrig::GetMaxInteractRadius()
{
	FLOAT radius = TeleportClientRange;
	if( CollisionCylinderComponent != NULL ){
		radius = radius < CollisionCylinderComponent->CollisionRadius ? radius : CollisionCylinderComponent->CollisionRadius;
	}

	return radius;
}

void ATeleportOrig::DoInteract()
{
	HandleTeleport();
}

UBOOL ATeleportOrig::CanInteract() const
{
	return FALSE;
}

void ATeleportOrig::SetSilhouette( UBOOL show, FVector sc )
{
	// nothing to do
}

BYTE ATeleportOrig::GetPickPriority()
{
	return PickPriority;
}

//void ATeleportTarget::BeginDestroy()
//{
//	MMOTargetManager::getSingleton().DeleteActor(this);
//}


BYTE ATeleportTarget::GetPickPriority()
{
	return PickPriority;
}

//void ATeleportNetNode::BeginDestroy()
//{
//	MMOTargetManager::getSingleton().DeleteActor(this);
//}

BYTE ATeleportNetNode::GetPickPriority()
{
	return PickPriority;
}

// oscar@snda
BYTE ATeleportOrig::GetMouseCursor()
{
	return EMC_Hand;
}

BYTE ATeleportTarget::GetMouseCursor()
{
	return EMC_Arrow;
}

BYTE ATeleportNetNode::GetMouseCursor()
{
	return EMC_Hand;
}

// oscar@snda

//-------------------------------------------------
void ATeleportOrig::ActiveStaticEffect( UBOOL isActive )
{
	if( TeleportStaticEffect != NULL ){
		TeleportStaticEffect->SetActive( isActive, FALSE );
	}
}
//-------------------------------------------------
void ATeleportOrig::ActiveFoucsEffect( UBOOL isActive )
{
	if( TeleportFoucsEffect == NULL ){
		return;
	}

	TeleportFoucsEffect->SetActive( isActive, FALSE );
}
//-------------------------------------------------
void ATeleportOrig::Send_TeleportReq()
{
	if(!GIsEditor)
	{
		CMessageHandler::GetInstance().Send_Teleport(AutoServerID);
	}
}
//-------------------------------------------------
void ATeleportOrig::HandleTeleportDisapart()
{
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_FBEnter_FBEnter,FALSE, "");

	//UMMOUIFBEnter *pFBEnter = (UMMOUIFBEnter*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEnter_FBEnter );
	////if( !pFBEnter || pFBEnter->IsVisible ){
	//if( pFBEnter != NULL && pFBEnter->IsVisible ){
	//	pFBEnter->Show( FALSE );
	//}
}

void ATeleportOrig::HandleTeleport()
{
	if (GIsEditor)
		return;			// added by zhaojun@snda; avoid crash when playing in the editor

	if (bInTeleportOrig==FALSE)
	{
		return;
	}

	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( !pl || !ctrl){
		return;
	}

	if(ctrl)
	{
		ctrl->GotoState(NAME_None);
		ctrl->eventClearAllCmd();

		FSMIO::getSingleton().Target.ClearDesireTarget(NULL);
		MMOFacade::getSingleton().GetMainController()->StopMove();
		FSMIO::getSingleton().CDTime.ResetCD_WalkAndRunDelay(0.3f,0.3f);
		FSMIO::getSingleton().CDTime.ResetCD_MoveDelay_Frame(2);

	}
	//
	TArray<FTeleportLink*> teleLinkList;
	int LnkCnt = UGlobalMapInfo::GetSingleton()->FindLinkByOrigID( &teleLinkList, ctrl->MapName, AutoServerID );
	if( LnkCnt <= 0 ){
		warnf( TEXT( "!!!Not target teleport to go to from %s, ID = %d"), *ToSceneName, AutoServerID );
		return;
	}
	//
	FExportMap mapInfo	= UGlobalMapInfo::GetSingleton()->FindMapInfoByName( teleLinkList(0)->ToMapName );
	FExportMap curMapInfo	= UGlobalMapInfo::GetSingleton()->FindMapInfoByName( ctrl->MapName );
	
	//if( LnkCnt == 1 && ( mapInfo.ServerMapType == E_est_Static || mapInfo.ServerMapType == E_est_Line ) ){
	if( LnkCnt == 1 && ( ( mapInfo.ServerID == curMapInfo.ServerID ) || ( mapInfo.ServerMapType == E_est_Static || mapInfo.ServerMapType == E_est_Line ) ) ){
		int svrLinkID = 0;
		//goto there directly
		svrLinkID = teleLinkList(0)->srcLinkID;//teleLinkList(0)->srcLinkID & 0x0000ffff;
		if( curMapInfo.ServerMapType == E_est_Dungeon && mapInfo.ServerID != curMapInfo.ServerID  ){
			//in dungeon jump to other map(not dungeon),popup confirm message box
			UMMOUIFBBattleComfirm *pFBBattleComfirm = (UMMOUIFBBattleComfirm*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEndAll_FBBattleComfirm );
			if( pFBBattleComfirm != NULL ){
				pFBBattleComfirm->ConfirmLeaveFromTeleport( svrLinkID );
			}
			else{
				CMessageHandler::GetInstance().Send_Teleport( svrLinkID );
			}
		}
		else{
			INT myLv = pl->GetActorLevel();
			if( myLv < MinPortalLevel ){
				FString errorName =TEXT("MMOErrorInfo.ERROR_CODE_DEF.errSceneEnterMinLevelLimit");
				FString errStr = MMOUI::ParseLocalizedPropertyPath(errorName);
				MMOUI::GetUIManager()->delegateAddScreenMessage( errStr.Replace( TEXT("[A]"), *appItoa( MinPortalLevel ) ), MsgInfoType_Table, errorName);
				//MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath(errorName));

				return;
			}
			if( myLv > MaxPortalLevel ){
				FString errorName =TEXT("MMOErrorInfo.ERROR_CODE_DEF.errSceneEnterMaxLevelLimit");
				FString errStr = MMOUI::ParseLocalizedPropertyPath(errorName);
				MMOUI::GetUIManager()->delegateAddScreenMessage( errStr.Replace( TEXT("[A]"), *appItoa( MaxPortalLevel ) ), MsgInfoType_Table, errorName);
				//MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath(errorName));

				return;
			}

			CMessageHandler::GetInstance().Send_Teleport( svrLinkID );
		}
	}
	//if( esmt_Dungeon == mapInfo.ServerMapType && esmt_Dungeon != curMapInfo.ServerMapType )
	else if( mapInfo.ServerMapType == E_est_Dungeon ){
		//stop moving
		ctrl->StopMove( TRUE );

		//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_FBEnter_FBEnter,TRUE);
		//MMOUI::GetUIManager()->LoadMoviePlayers( MMOUI::GetUIManager()->MoviePlayer_FBEnter );

		//goto FB, open UI to select which one to go
		UMMOUIFBEnter *pFBEnter = (UMMOUIFBEnter*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEnter_FBEnter );
		//if( !pFBEnter || pFBEnter->IsVisible ){
		if(pFBEnter){
			UBOOL bCanSelectFB = TeamManager::getSingleton().HasTeam() ? TeamManager::getSingleton().IsTeamLeader() : true;
			if( bCanSelectFB )
			{
				//teamlead select FB
				pFBEnter->PrepareShow( ctrl->MapName, AutoServerID );
				//pFBEnter->Show( true );
			}
			else
			{
				FString hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral12") );
				MMOUI::GetUIManager()->delegateAddScreenMessage( hint,MsgInfoType_Table,TEXT("MMOGame.FBGeneral.FBGeneral12") );
				//MMOChatManager::getSingleton().AddSystemMessage( hint );

				//teamate only show confirm dialog
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_FBEnter_FBEnter,FALSE, "");
			}	
		}
		
	}
}
//================================================================================
FVector ATeleportNetNode::GetInteractActorLocation()
{
	return Location;
}

FLOAT ATeleportNetNode::GetMaxInteractRadius()
{
	FLOAT radius = TeleportClientRange;
	if( CollisionCylinderComponent != NULL ){
		radius = radius < CollisionCylinderComponent->CollisionRadius ? radius : CollisionCylinderComponent->CollisionRadius;
	}

	return radius;
}

void ATeleportNetNode::DoInteract()
{
	HandleTeleport();
}

UBOOL ATeleportNetNode::CanInteract() const
{
	return FALSE;
}

void ATeleportNetNode::SetSilhouette(UBOOL show, FVector sc)
{
	if(TeleportNodeMesh != NULL)
	{
		TeleportNodeMesh->UpdateSilhouetteState( show, sc );
		//TeleportNodeMesh->ShowSilhouette = show;
		//TeleportNodeMesh->SilhouetteColor = sc;
	}
}

//------------------------------------------------------------------------------
void ATeleportNetNode::HandleTeleportDisapart()
{
	UMMOUITeleportNet *pTeleportStation = (UMMOUITeleportNet*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_TeleportNet );
	if( pTeleportStation != NULL && pTeleportStation->IsVisible ){
		MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_TeleportNet, FALSE, "" );
	}
}
//------------------------------------------------------------------------------
void ATeleportNetNode::HandleTeleport()
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();
	//FTeleportNodeObj* pNetNode = UGlobalMapInfo::GetSingleton()->FindTeleportNetNodes( InSceneID, GetName() );
	FTeleportNetNodeData *pNetNode = pMapDataManager->GetTeleportNetNodes( (EWorldRegion)RegionID, InSceneID, GetName() );
	if( pNetNode == NULL ){
		return;
	}

	FTeleportNodeObj *pNodeTemplate = pNetNode->_NodeTemplate;
	//UBOOL dataPrepared = pMapDataManager->IsGettedServerTeleportData();
	if( !pNetNode->_isPlayerOpened ){
		//notify server open it
		CMessageHandler::GetInstance().Send_TeleportToNetNodes( pNodeTemplate->ServerID, pNodeTemplate->ServerID );
		//CMessageHandler::GetInstance().Send_Require_TeleportNetNodesData();
		//
		pMapDataManager->SetCurOpenedNodeID( pNodeTemplate->ServerID );
	}
	//else if( !dataPrepared ){
	//	//not initilized by server
	//	CMessageHandler::GetInstance().Send_Require_TeleportNetNodesData();
	//	//
	//	pMapDataManager->SetCurOpenedNodeID( pNodeTemplate->ServerID );
	//}
	else{
		//add by JimZhang@snda.com
		//MMOUI::GetUIManager()->LoadMoviePlayers( MMOUI::GetUIManager()->MoviePlayer_TeleportNet );
		UMMOUITeleportNet *pTeleportStation = (UMMOUITeleportNet*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_TeleportNet );
		if( pTeleportStation != NULL && !pTeleportStation->IsVisible ){
			pTeleportStation->Open( pNodeTemplate->Region, pNodeTemplate->ServerID );
		}
	}
}



FVector ATeleportTarget::GetInteractActorLocation()
{
	return Location;
}

FLOAT ATeleportTarget::GetMaxInteractRadius()
{

	return 1000;
}

void ATeleportTarget::DoInteract()
{
}

UBOOL ATeleportTarget::CanInteract() const
{
	return FALSE;
}

void ATeleportTarget::SetSilhouette( UBOOL show, FVector sc )
{
	// nothing to do
}