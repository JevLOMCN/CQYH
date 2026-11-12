#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapImgTiles.h"
#include "MMOMapDataManager.h"
#include "TeamManager.h"
#include "MMOStats.h"
#include "MailManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOWeatherManager.h"
#include "MMOBattleManager.h"
#include "MMOArenaManager.h"
#include "MMOGuideManager.h"
#include "MMOMapDataManager.h"
#include "ClientActionConditionCheck.h"
#include "MMONavigationManager.h"
#include "FSMIO.h"
#include "MMOAssistManager.h"

IMPLEMENT_CLASS(UMMOUIMinimap)
IMPLEMENT_CLASS(AMMOGameObject)

void UMMOUIMinimap::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	//check(NULL != evt);
	if (NULL != evt)
	{
		switch (evt->GetEventType())
		{
		//case EGameEvent::GameEvent_Mail:
		//	{
		//		MMOGameEvent_Mail* pMailEvt = (MMOGameEvent_Mail*)evt;
		//		ASSERT_CHECK_ERROR_RETURN(pMailEvt);
		//		if (UpdateMaxMailCount == pMailEvt->eMailType)
		//		{
		//			eventCallUpdateMaxMailCount(pMailEvt->_MailCount);
		//		}
		//		else if (NewMail == pMailEvt->eMailType)
		//		{
		//			eventCallASSetNewMailCount(pMailEvt->_MailCount);
		//		}
		//	}
		//	break;
		case EGameEvent::GameEvent_UpdateSceneLines:
			{
				UpdateSceneLinesList();
				PrepareSceneLineID = MMOMapDataManager::getSingleton().GetCurSceneLineIn();
			}
			break;
		case EGameEvent::GameEvent_EnterSceneZone:
			{
				MMOGameEvent_OneInt *enterZoneEvt = (MMOGameEvent_OneInt*)evt;
				eventCallASSetMapZoneType( enterZoneEvt->IntData );
			}
			break;
		case EGameEvent::GameEvent_LeaveSceneZone:
			{
				MMOGameEvent_OneInt *leaveZoneEvt = (MMOGameEvent_OneInt*)evt;
				eventCallASSetMapZoneType( EPKT_Normal );
			}
			break;
		case EGameEvent::GameEvent_EnterScene:
			{
				//
				FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
				UBOOL canChangeLines = ( curMapInfo.ServerMapType == E_est_Line );
				SceneLinesDropDownList->SetVisible( canChangeLines );
				if( canChangeLines == TRUE ){
					CMessageHandler::GetInstance().Send_QuerySceneLine();
				}
				else{
					MMOMapDataManager::getSingleton().ClearCurMapLines();
				}

				//map icon
				if( SceneIconMc != NULL ){
					INT animIdx = (curMapInfo.ServerMapType - 1) * 10 + 1;
					SceneIconMc->GotoAndStopI( animIdx );
				}

				//map zone icon
				if( Player ){
					if( Player->IsInFreePkZone ){
						eventCallASSetMapZoneType( EPKT_Free );
					}
					else if( Player->IsInSafeZone ){
						eventCallASSetMapZoneType( EPKT_Safe );
					}
					else{
						eventCallASSetMapZoneType( EPKT_Normal );
					}
				}

				//
				UpdateArenaEntVisible();
			}
			break;
		case EGameEvent::GameEvent_UpdateDayTime:
			{
				FLOAT curTime = MMOWeatherManager::getSingleton().GetCurTime();
				
				// convert to 480 frames, 0:00 is at frame 360, 10 frames early to match the animation effect
				INT curTimeAnim = (INT)(curTime / 24.0f * 480 + 350) % 480;
				//eventCallUpdateDayTime(curTimeAnim);
			}
			break;
		case EGameEvent::GameEvent_UpdateWeather:
			{
				INT curWeather = MMOWeatherManager::getSingleton().GetCurWeatherType();
				//eventCallUpdateWeather(curWeather);
			}
			break;
		case EGameEvent::GameEvent_SceneFinished:
			{
				MMOGameEvent_OneInt *sceneFinishedEvt = (MMOGameEvent_OneInt*)evt;

				FExportMap *pCurMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( sceneFinishedEvt->IntData );
				if( pCurMapInfo != NULL && pCurMapInfo->ServerMapType == E_est_Dungeon ){
					eventUCCallStopFBTimeCountDown();
				}
			}
			break;

		case EGameEvent::GameEvent_StartNavigate:
			eventCallSetPathFindState(1);
			break;

		case EGameEvent::GameEvent_StopNavigate:
			eventCallSetPathFindState(0);
			break;
		}
	}
}

void UMMOUIMinimap::ClearUI()
{
	BattleEnterToggleBtn->SetVisible( FALSE );
	eventUCCallStopBattleQueueTimeCountDown();

	if( FBAidBtn != NULL ){
		FBAidBtn->SetVisible( FALSE );
	}
}

//================================================================================================
void UMMOUIMinimap::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		if( IsCurrentMapShowing && !IsSmallMinimapMode ){
			SwitchMinimapToPathMap();
		}
	}
	else{
		//so that hiding also hides the dropdown table (the dropdown table currently cannot hide automatically)
		eventResetSceneLinesList();
	}
}

//================================================================================================
void UMMOUIMinimap::ChangeScene()
{
	InitMiniMap();
	UpdateFramePinVisible();
	//
	MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();
	FExportMap &curMapInfo = mapDataMgr.GetCurMap();
	if( FBAidBtn != NULL ){
		UBOOL isInDungeon = ( E_est_Dungeon == curMapInfo.ServerMapType || E_est_GuildDungeon == curMapInfo.ServerMapType );
		
		if( isInDungeon ){
			FBAidBtn->SetVisible( TRUE );
			//const FDungeonInfo *fbInfo = MMOMapDataManager::getSingleton().GetDungeonInfo( curMapInfo.ServerID, mapDataMgr.GetCurFBDiff() );
			//if( fbInfo ){
			//	INT totalSecond = fbInfo->StandardTime / 1000;
			//	eventStartFBTimeCountDown( totalSecond );
			//}
		}
		else{
			UBOOL isFBAidBtnVisible = FBAidBtn->GetBool( TEXT("visible") );
			if( isFBAidBtnVisible ){
				StopFBTimeCountDownAndHide();
			}
			FBAidBtn->SetVisible( FALSE );
		}
	}

	//if a confirmation box has not been clicked, cancel it automatically
	if( ConfirmMsgBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( ConfirmMsgBox );
	}

	//update dropdown list of scene lines
	eventResetSceneLinesList();
}
//countdown
void UMMOUIMinimap::ShowMapTimeCount( INT sec, UBOOL isToShow/* = TRUE*/ )
{
	if( !isToShow ){
		StopFBTimeCountDownAndHide();
		return;
	}

	eventStartFBTimeCountDown( sec );
}
//called when entering a new scene
void UMMOUIMinimap::InitMiniMap()
{
	AMMOPlayerController *pCtrl;
	pCtrl = MMOFacade::getSingleton().GetMainController();
	if( pCtrl == NULL || Player == NULL ){
		return;
	}

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	FExportMap &curMapInfo = pMapDataMgr->GetCurMap();
	CurMapTexName = curMapInfo.MapName;

	//ImgResPrefixName = CurMapTexName;
	//initialize map tiles
	INT regionIdx = 0;
	regionIdx = pMapDataMgr->GetCurPlayerRegionIn( Player->Location );
	//InitMiniMapTiles( ImgResPrefixName, InRegionIdx );
	FInMapRegion *pCurReginInCurMap = pMapDataMgr->GetReginInCurMap( regionIdx );
	if( pCurReginInCurMap == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUIMinimap::InitMiniMap] NoMapRegion in Map:[%s]"), *CurMapTexName );
		if( MapName ){
			MapName->SetText( TEXT("Unknown area") );
		}
	}
	else{
		//update map name
		CurMapDisplayName = pCurReginInCurMap->DisplayName;
		if( MapName ){
			MapName->SetText( CurMapDisplayName );
			if( CurMapDisplayName.IsEmpty() ){
				MapName->SetText( TEXT("No name") );
			}
		}
	}


	//restet in region mark,-1 means not in any region of this map
	InRegionIdx = -1;

	//if the scene has Multilayer region ,minimap will update map mark's visiblity every frame
	IsMultilayerRegion = pMapDataMgr->IsCurMapRegionMultilayer();

	//rotation the hole panel to be the same as the player orientation
	if(pCtrl->PlayerCamera && Cast<AMMOPlayerCamera>(pCtrl->PlayerCamera))
	{
		CameraYawDeg = pCtrl->PlayerCamera ? (INT)(Cast<AMMOPlayerCamera>(pCtrl->PlayerCamera)->curVT.POV.Rotation.Yaw * C_UnrRotToDeg + 0.5f) : 0;
	}
	else
	{
		CameraYawDeg = 0;
	}

	if( MapBlock )
	{
		MapBlock->SetFloat( "rotation", -CameraYawDeg );
	}

	if( MapNPCView )
	{
		//MapNPCView->SetFloat( "rotation", -CameraYawDeg );
		//MapNPCView->SetFloat( "rotation", 0 );
	}

	//update map fixed points
	InitMapStaticMarks();
	
	return;
}
/*
//
void UMMOUIMinimap::InitMiniMapTiles( const FString& inMapFileName, FInMapRegion RegionInMap )
{
	bool bLoadOK = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES)->LoadUiTile( MapTiles, TEXT("map_alpha%d__nopack") );
	if( !bLoadOK ){
		return;
	}
	//
	FString strValueKey;
	INT tileSizeX;
	strValueKey = TEXT("MiniMapTileSizeX");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeX, GGameIni );
	INT tileSizeY;
	strValueKey = TEXT("MiniMapTileSizeY");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeY, GGameIni );
	//
	INT maxImgTileRowNum = RegionInMap.MiniMapTileRowNum;
	INT maxImgTileColNum = RegionInMap.MiniMapTileColNum;
	//
	//INT oneCoordToPixels = RegionInMap.MiniMapPixelSize;
	////scale3dTo2d is the conversion ratio from 3d coordinates to 2d coordinates; this local map's Y axis direction is opposite to the world 3d Y axis
	//if(oneCoordToPixels==0){
	//	//wrong
	//	oneCoordToPixels = 54;
	//}
	//INT realCoordStepToPixels = (INT)( MAP_SNAPSHOT_COORDINATE_STEP / oneCoordToPixels );
	FVector2D map3Dto2DNoScaled;
	//map3Dto2DNoScaled.X = (FLOAT)realCoordStepToPixels / MAP_SNAPSHOT_COORDINATE_STEP;
	map3Dto2DNoScaled.X =  (FLOAT)1 / RegionInMap.MiniMapPixelSize;
	map3Dto2DNoScaled.Y = -map3Dto2DNoScaled.X;

	//scale
	//FLOAT fScale = RegionInMap.MiniMapScale;
	FVector2D map3Dto2D;
	//map3Dto2D = map3Dto2DNoScaled * fScale;
	map3Dto2D = map3Dto2DNoScaled;

	//offset
	FBox regionBox = RegionInMap.BoundingBox;//RegionInMap.GetComponentsBoundingBox( TRUE );
	FVector2D afterScale2dOffsetInMap;
	afterScale2dOffsetInMap.X = -regionBox.Min.Y * map3Dto2D.X + RegionInMap.MiniMapOffsetX;
	afterScale2dOffsetInMap.Y = -regionBox.Min.X * map3Dto2D.Y + RegionInMap.MiniMapOffsetY;

	//
	INT origImgSizeX = RegionInMap.MiniMapTileRowNum * tileSizeX;
	INT origImgSizeY = RegionInMap.MiniMapTileColNum * tileSizeY;
	FVector2D afterTurned2dOffsetInMap( -origImgSizeX * ( 1 - RegionInMap.MiniMapScale ) * SIN_MAP, origImgSizeY * ( 1 - COS_MAP ) );
	//
	FString strTexPath;
	FString texPackName;
	texPackName = inMapFileName + TEXT("_") + RegionInMap.VolumeName;
	strTexPath = FString::Printf( TEXT("%s_mMap.%s_mMap"), *texPackName, *texPackName );
	
	//
	MMOMapImgTiles *pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
	FVector player3dPos = MMOFacade::getSingleton().GetMainPlayer()->Location;
	//
	pMapTiles->SetTilesGap( 0 );
	pMapTiles->Initialize( GFXPlayer, strTexPath, player3dPos, tileSizeX, tileSizeY,
						 maxImgTileRowNum, maxImgTileColNum, map3Dto2D, afterScale2dOffsetInMap, afterTurned2dOffsetInMap, ZoomQuot );
	//
	CurMapViewBound = MapViewBound / ZoomQuot;
}

*/
////changed to use the same resource as the big map
void UMMOUIMinimap::InitMiniMapTiles( const FString& inMapFileName, FInMapRegion RegionInMap )
{
	bool bLoadOK = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES)->LoadUiTile( MapTiles, TEXT("minimap_alpha%d__nopack") );
	if( !bLoadOK ){
		return;
	}
	//
	FString strValueKey;
	INT tileSizeX;
	strValueKey = TEXT("WorldMapTileSizeX");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeX, GGameIni );
	INT tileSizeY;
	strValueKey = TEXT("WorldMapTileSizeY");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeY, GGameIni );
	//
	INT maxImgTileRowNum = RegionInMap.WorldMapTileRowNum;
	INT maxImgTileColNum = RegionInMap.WorldMapTileColNum;
	//
	FVector2D map3Dto2DNoScaled;
	map3Dto2DNoScaled.X =  (FLOAT)1 / RegionInMap.WorldMapPixelSize;
	map3Dto2DNoScaled.Y = -map3Dto2DNoScaled.X;

	//scale
	FVector2D map3Dto2D;
	map3Dto2D = map3Dto2DNoScaled;

	//
	INT minimapX, minimapY;
	GConfig->GetInt( TEXT("MapImg"), TEXT("MiniMapTileSizeX"), minimapX, GGameIni );
	GConfig->GetInt( TEXT("MapImg"), TEXT("MiniMapTileSizeY"), minimapY, GGameIni );
	INT origImgSizeX = RegionInMap.MiniMapTileRowNum * minimapX;
	INT origImgSizeY = RegionInMap.MiniMapTileColNum * minimapY;

	UINT trueWorldMapWidth = origImgSizeX;//( origImgSizeX * RegionInMap.WorldMapScale + 0.5 );
	UINT trueWorldMapHeight = origImgSizeY;//( origImgSizeY * RegionInMap.WorldMapScale + 0.5 );

	//offset
	FBox regionBox = RegionInMap.BoundingBox;
	FVector2D afterScale2dOffsetInMap;
	afterScale2dOffsetInMap.X = -regionBox.Min.Y * map3Dto2D.X;
	afterScale2dOffsetInMap.Y = -regionBox.Min.X * map3Dto2D.Y + trueWorldMapHeight;

	//
	FVector2D afterTurned2dOffsetInMap;
	afterTurned2dOffsetInMap.X = RegionInMap.WorldMapOffsetX;
	afterTurned2dOffsetInMap.Y = RegionInMap.WorldMapOffsetY - trueWorldMapHeight * COS_MAP;

	//
	FString strTexPath;
	FString texPackName;
	texPackName = inMapFileName + TEXT("_") + RegionInMap.VolumeName;
	if( !RegionInMap.ReuseMapVolumeName.IsEmpty() ){
		texPackName = inMapFileName + TEXT("_") + RegionInMap.ReuseMapVolumeName;
	}

	strTexPath = FString::Printf( TEXT("%s_wMap.%s_wMap"), *texPackName, *texPackName );
	//
	//ZoomQuot = RegionInMap.WorldMapPixelSize / RegionInMap.MiniMapPixelSize;//RegionInMap.MiniMapScale / RegionInMap.WorldMapScale;
	ZoomQuot = RegionInMap.MiniMapScale;// / RegionInMap.WorldMapScale;

	//
	MMOMapImgTiles *pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
	FVector player3dPos = MMOFacade::getSingleton().GetMainPlayer()->Location;
	//
	pMapTiles->SetTilesGap( 0.5 );
	pMapTiles->Initialize( GFXPlayer, strTexPath, player3dPos, tileSizeX, tileSizeY,
							maxImgTileRowNum, maxImgTileColNum, map3Dto2D, afterScale2dOffsetInMap, afterTurned2dOffsetInMap, ZoomQuot );
	ResizeMap();
	//
	CurMapViewBound = MapViewBound / ZoomQuot;
}
//====================================================================================================
void UMMOUIMinimap::ClearMiniMapUI()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	//clear dynamic map ui objects
	FActorInMapUI *pUIActor = NULL;
	//near
	MMOMapDataManager::MapDynamicActorIterator iterNear = pMapDataMgr->BeginIterateDymcActorNear();
	for(  ; iterNear; ++iterNear ){
		pUIActor = &iterNear.Value();

		//
		if( pUIActor->_UiObjInMiniMap != NULL ){
			eventRemoveChild( pUIActor->_UiObjInMiniMap );
			pUIActor->_UiObjInMiniMap->RemoveFromRoot();
			pUIActor->_UiObjInMiniMap = NULL;
		}
	}
	//Global
	MMOMapDataManager::MapDynamicActorIterator iterGlobal = pMapDataMgr->BeginIterateDymcActorGlobal();
	for(  ; iterGlobal; ++iterGlobal ){
		pUIActor = &iterGlobal.Value();

		//
		if( pUIActor->_UiObjInMiniMap != NULL ){
			eventRemoveChild( pUIActor->_UiObjInMiniMap );
			pUIActor->_UiObjInMiniMap->RemoveFromRoot();
			pUIActor->_UiObjInMiniMap = NULL;
		}
	}

	//clear static map ui objects
	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();
	for( INT i = staticMarks.Num() - 1; i >= 0; --i ){
		pUIActor = &staticMarks( i );

		if( pUIActor->_UiObjInMiniMap == NULL ){
			continue;
		}

		eventRemoveChild( pUIActor->_UiObjInMiniMap );
		pUIActor->_UiObjInMiniMap->RemoveFromRoot();
		pUIActor->_UiObjInMiniMap = NULL;
	}

}
//
void UMMOUIMinimap::InitMapStaticMarks()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	pMapDataMgr->PrepareCurMapStaticActorInMapUI();

	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();

	INT cnt = staticMarks.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pStaticMark = &staticMarks(i);

		UGFxObject *pUiObj = pStaticMark->_UiObjInMiniMap;
		//if ui object is not exit, create an new one
		if( pUiObj == NULL ){
			UBOOL createRst = CreateMapMarkUIObject( pStaticMark );
			if( createRst != TRUE ){
				continue;
			}
			pUiObj = pStaticMark->_UiObjInMiniMap;
		}
	}
}

//===================================================================================================================================
/*
//
void UMMOUIMinimap::InitMiniMapTiles( const FString& pixName, INT regionIdx )
{
	bool bLoadOK = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES)->LoadUiTile( MapTiles, TEXT("map_alpha") );
	//
	FString strValueKey;
	INT tileSizeX;
	strValueKey = TEXT("MiniMapTileSizeX");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeX, GGameIni );
	INT tileSizeY;
	strValueKey = TEXT("MiniMapTileSizeY");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeY, GGameIni );
	//
	INT oneCoordToPixels;
	strValueKey = pixName + TEXT("_MiniMapPixelSize");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, oneCoordToPixels, GGameIni );
	//scale3dTo2d is the conversion ratio from 3d coordinates to 2d coordinates; this local map's Y axis direction is opposite to the world 3d Y axis
	INT realCoordStepToPixels = (INT)( MAP_SNAPSHOT_COORDINATE_STEP / oneCoordToPixels );
	FVector2D map3Dto2D;
	map3Dto2D.X = (FLOAT)realCoordStepToPixels / MAP_SNAPSHOT_COORDINATE_STEP;
	map3Dto2D.Y = -map3Dto2D.X;
	//
	FVector2D loadMap2dOffsetInMap;
	strValueKey = pixName + TEXT("_miniMapOffsetX");
	GConfig->GetFloat( TEXT("MapImg"), *strValueKey, loadMap2dOffsetInMap.X, GGameIni );
	strValueKey = pixName + TEXT("_miniMapOffsetY");
	GConfig->GetFloat( TEXT("MapImg"), *strValueKey, loadMap2dOffsetInMap.Y, GGameIni );
	//
	INT maxImgTileRowNum;
	strValueKey = TEXT("MiniMapTileRowNum");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, maxImgTileRowNum, GGameIni );
	INT maxImgTileColNum;
	strValueKey = TEXT("MiniMapTileColNum");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, maxImgTileColNum, GGameIni );
	//
	FString strTexPath;
	strTexPath = FString::Printf( TEXT("%s_mMap.%s_mMap"), *pixName, *pixName );
	//handle special in region map
	if( InRegionIdx >= 0 ){
		//offset
		AMMOVolume_MapSize *pRegion = RegionMapData( regionIdx );
		FBox regionBox = pRegion->GetComponentsBoundingBox( TRUE );
		FVector2D mapPos;
		mapPos.X = regionBox.Min.Y * map3Dto2D.X;// + loadMap2dOffsetInMap.X;
		mapPos.Y = - regionBox.Min.X * map3Dto2D.Y;// + loadMap2dOffsetInMap.Y;
		//scale
		strValueKey = pixName + TEXT("_miniMapScale");
		FLOAT fScale = 0.0;
		GConfig->GetFloat( TEXT("MapImg"), *strValueKey, fScale, GGameIni );
		map3Dto2D.X *= fScale;
		map3Dto2D.Y *= fScale;
		//offset
		loadMap2dOffsetInMap = mapPos * fScale + loadMap2dOffsetInMap;
	}
	//
	MMOMapImgTiles *pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
	FVector player3dPos = MMOFacade::getSingleton().GetMainPlayer()->Location;
	pMapTiles->Initialize( GFXPlayer, strTexPath, player3dPos, tileSizeX, tileSizeY,
						 maxImgTileRowNum, maxImgTileColNum, map3Dto2D, loadMap2dOffsetInMap, ZoomQuot );
}
*/
//
//INT UMMOUIMinimap::CheckRegion()
//{
//	AMMOPlayer* pl =	MMOFacade::getSingleton().GetMainPlayer();
//	//
//	FVector max;
//	FVector min;
//	FVector check = pl->Location;
//	UBOOL isIn = false;
//	INT curInIdx = -1;
//	INT curInPriorrity = -1;
//	AMMOVolume_MapSize *pRegion = NULL;
//	FBox regionBox;
//	for( int idx = 0; idx < RegionMapData.Num(); ++idx ){
//		pRegion = RegionMapData( idx );
//		
//		regionBox = pRegion->GetComponentsBoundingBox( TRUE );
//		isIn = regionBox.IsInside( pl->Location );
//		//max = regionBox.Max;
//		//min = regionBox.Min;
//		//isIn = check.X > min.X && check.Y > min.Y && check.X < max.X && check.Y < max.Y;
//		if( isIn ){
//			//
//			if( curInIdx == -1 ){
//				curInIdx = idx;
//				curInPriorrity = RegionMapData( curInIdx )->Priorrity;
//			}
//			//
//			if(  pRegion->Priorrity > curInPriorrity ){
//				curInIdx = idx;
//				curInPriorrity = pRegion->Priorrity;
//			}
//		}
//	}
//
//	return curInIdx;
//}
//for main map and regin map swap,but now main map is removed,
//only region map left, so this may be not needed any more
UBOOL UMMOUIMinimap::IfNeedReloadMap( INT regionIdx )
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	FInMapRegion *pReginInMap = pMapDataMgr->GetReginInCurMap( regionIdx );
	bool isIn = ( pReginInMap != NULL );

	UBOOL isMapChanged = FALSE;
	//FString mapName; 
	if( isIn ){
		//in a region
		if( regionIdx != InRegionIdx ){
			//in a new region,
			InRegionIdx = regionIdx;
			//
			isMapChanged = TRUE;
		}
	}
	else if( InRegionIdx >= 0 ){
		//get out of region
		InRegionIdx = -1;
		//ERROR,NOT In any Volume!!!!
		isMapChanged = FALSE;
	}

	return isMapChanged;
}
//------------------------------------------------------------------------
void UMMOUIMinimap::UpdateMinimap()
{
	SCOPE_CYCLE_COUNTER(STAT_MMOMinimap);
	AMMOPlayerController* ctrl =	MMOFacade::getSingleton().GetMainController();
	if( ctrl == NULL ){
		return;
	}

	AMMOPlayer* pl = Cast<AMMOPlayer>(ctrl->Pawn);
	if( pl == NULL ){
		return;
	}
	//check the map region
	//CheckRegionToReloadMap();
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	INT regionIdx = pMapDataMgr->GetCurPlayerRegionIn( pl->Location );
	UBOOL willReload = IfNeedReloadMap( regionIdx );
	if( willReload ){
		FInMapRegion *pCurReginInCurMap = pMapDataMgr->GetReginInCurMap( regionIdx );
		//
		if( pCurReginInCurMap != NULL ){
			//pMapDataMgr->SetCurReginIdxInMap( regionIdx );
			InitMiniMapTiles( CurMapTexName, *pCurReginInCurMap );
			MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES)->TileTheMap();
			//initialize static actors in this region
			//pMapDataMgr->InitMinimapStaticActorsContainer();
			pMapDataMgr->PrepareCurMapStaticActorInMapUI();

			CurMapDisplayName = pCurReginInCurMap->DisplayName;
			if( MapName != NULL ){
				MapName->SetText( CurMapDisplayName );
				if( CurMapDisplayName.IsEmpty() ){
					MapName->SetText( TEXT("No name") );
				}
			}

			//
			MMOGameEvent_Any evt( GameEvent_UpdateCurUIMap );
			MMOUI::PostGameEvent(&evt);
		}
	}

	//update coordinate
	if( MapCoord ) {
		//FVector playerGridIdx = pMapDataMgr->CvtSceneVecToGridIdx( pl->Location );
		MapCoord->SetText(  FString::Printf( TEXT("(%d,%d)"), INT( pl->Location.X / 100 ), INT( pl->Location.Y / 100 ) ) );
	}

	//
	if( IsCurrentMapShowing && IsSmallMinimapMode ){
		//don't update if the minimap is not shown
		MMOMapImgTiles* pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);

		if( CurMapTexName.IsEmpty() || !pMapTiles->isReady() ){
			return;
		}

		//check(pMapTiles != NULL);	//zhaojun@snda
		if(pMapTiles == NULL)
			return;
		//update player icon
		INT fYawDeg = 0;
		
		if(ctrl->PlayerCamera && Cast<AMMOPlayerCamera>(ctrl->PlayerCamera))
		{
			fYawDeg =(INT)( Cast<AMMOPlayerCamera>(ctrl->PlayerCamera)->curVT.POV.Rotation.Yaw * C_UnrRotToDeg + 0.5);
		}

		if(PlayerArrow){
			PlayerArrow->SetFloat( "rotation", pl->Rotation.Yaw * C_UnrRotToDeg - fYawDeg );
		}

		INT diffDeg = CameraYawDeg - fYawDeg;
		if( diffDeg >= 1 || diffDeg <= -1 ){
			CameraYawDeg = fYawDeg;
			if( MapBlock ){
				MapBlock->SetFloat( "rotation", -CameraYawDeg );
			}
			if( MapNPCView ){
				//MapNPCView->SetFloat( "rotation", -CameraYawDeg );	// commentted by zhaojun@snda, 2014-8=10
				//MapNPCView->SetFloat( "rotation", 0 );
			}
		}

		//update map tiles
		FVector2D player2dPosInMap = pMapTiles->Map2DPosByWorld3DPos( pl->Location );

		/////////////////////////zhaojun@snda begin/////////////////////////////
		float cosine = appCos(CameraYawDeg * PI / 180.0f);
		float sine = appCos(CameraYawDeg * PI / 180.0f);

		// (0,0) in the playerSpace with MiniMapTiles's zoom
		FVector2D pivotInPlayerSpace;	
		float ox = -player2dPosInMap.X * pMapTiles->_ZoomFactor;
		float oy = -player2dPosInMap.Y * pMapTiles->_ZoomFactor;

		// rotate
		pivotInPlayerSpace.X = ox * cosine + oy * sine;
		pivotInPlayerSpace.Y = -ox * sine + oy * cosine;
		///////////////////////// zhaojun@snda end /////////////////////////////

		FVector2D fixedPivotInPlayerSpace;
		fixedPivotInPlayerSpace.X = appFloor(pivotInPlayerSpace.X);
		fixedPivotInPlayerSpace.Y = appFloor(pivotInPlayerSpace.Y);

		// rotate back
		FVector2D fixedPlayerPos;
		fixedPlayerPos.X = fixedPivotInPlayerSpace.Y * sine - fixedPivotInPlayerSpace.X * cosine;
		fixedPlayerPos.Y = -fixedPivotInPlayerSpace.Y * cosine - fixedPivotInPlayerSpace.X * sine;

		////////////////////////////zhaojun@snda end/////////////////////////////////

		pMapTiles->OnlyUpdateTilesPosition( fixedPlayerPos / pMapTiles->_ZoomFactor );
			pMapTiles->TileTheMap();

		//update NPCs and other static actors
		//UpdateMapStaticMarks( pl->Location, fYawDeg );
		UpdateMapStaticMarks( fixedPivotInPlayerSpace, cosine, sine );

		//update teamate and so on
		UpdateMarksGlobalPlayer( fixedPivotInPlayerSpace, cosine, sine );

		//update monsters and player
		UpdateMarksNearPlayer( fixedPivotInPlayerSpace, cosine, sine );

		//
		UpdateMapPath( fixedPivotInPlayerSpace, cosine, sine );

		//update player marks
		//UpdatePlayerMarksInMap( player2dPosInMap, fYawDeg );
	}
}

//===================================================================================================================================
void UMMOUIMinimap::ResizeMap()
{
	MMOMapImgTiles* pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);

	FASDisplayInfo DI = MapBlock->GetDisplayInfo();
	DI.hasXScale = true;
	DI.hasYScale = true;
	DI.XScale = pMapTiles->_ZoomFactor * 100;
	DI.YScale = pMapTiles->_ZoomFactor * 100;
	//DI.Rotation = -CameraYawDeg; 
	MapBlock->SetDisplayInfo( DI );
	////
	//FASDisplayInfo npcViewDI = MapNPCView->GetDisplayInfo();
	//npcViewDI.hasXScale = true;
	//npcViewDI.hasYScale = true;
	//npcViewDI.XScale = pMapTiles->_ZoomFactor * 100;
	//npcViewDI.YScale = pMapTiles->_ZoomFactor * 100;

	//MapBlock->SetDisplayInfo( npcViewDI );
	////
	//FASDisplayInfo pathViewDI = MapPathView->GetDisplayInfo();
	//pathViewDI.hasXScale = true;
	//pathViewDI.hasYScale = true;
	//pathViewDI.XScale = pMapTiles->_ZoomFactor * 100;
	//pathViewDI.YScale = pMapTiles->_ZoomFactor * 100;

	//MapPathView->SetDisplayInfo( pathViewDI );
}
//===================================================================================================================================
//FVector2D UMMOUIMinimap::CvtScenePosToCurMinimapUiPosByScaledCenterPos( FVector mapPos3D, FVector2D pivotPos2D )
//{
//	//MMOMapImgTiles *pMiniMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
//	//centerPos2D.X = (INT)( centerPos2D.X );
//	//centerPos2D.Y = (INT)( centerPos2D.Y );
//
//	//FVector2D mapPos2D = pMiniMapTiles->Map2DPosByWorld3DPos( mapPos3D );
//	//mapPos2D = mapPos2D - centerPos2D;
//
//	//return mapPos2D;
//
//	//XXXXXXXXXXXXXXXXX
//	MMOMapImgTiles *pMiniMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
//	pivotPos2D.X = pivotPos2D.X * pMiniMapTiles->_ZoomFactor;
//	pivotPos2D.Y = pivotPos2D.Y * pMiniMapTiles->_ZoomFactor;
//	//centerPos2D.X = (INT)( centerPos2D.X );
//	//centerPos2D.Y = (INT)( centerPos2D.Y );
//
//	FVector2D mapPos2D = pMiniMapTiles->Map2DPosByWorld3DPos( mapPos3D );
//
//	FVector2D mapPos2D2 = mapPos2D * pMiniMapTiles->_ZoomFactor;
//	mapPos2D2 -= pivotPos2D;
//
//	mapPos2D = mapPos2D - pMiniMapTiles->GetWorldPosition() * pMiniMapTiles->_ZoomFactor + pMiniMapTiles->GetOriginOffset();
//
//	check(mapPos2D == mapPos2D2);
//
//	//mapPos2D.X = appFloor(mapPos2D.X);
//	//mapPos2D.Y = appFloor(mapPos2D.Y);
//
//	float s = appSin(45 * PI / 180.0f);
//	float c = appCos(45 * PI / 180.0f);
//
//	float x = mapPos2D2.X * c + mapPos2D2.Y * s;
//	float y = -mapPos2D2.X * s + mapPos2D2.Y * c;
//	
//	//mapPos2D += pMiniMapTiles->GetOriginOffset();
//
//	mapPos2D2.X = x;//appFloor(x+ 0.05f) +0.5f;
//	mapPos2D2.Y = y;//appFloor(y+ 0.05f) +0.5f;
//
//	return mapPos2D2;
//	
//	//xxxxxxx
//	//FVector2D rst;
//	//rst.X = mapPos2D.X * MiniMap_COS_MAP - mapPos2D.Y * MiniMap_SIN_MAP;
//	//rst.Y = mapPos2D.X * MiniMap_SIN_MAP + mapPos2D.Y * MiniMap_COS_MAP;
//	//return rst;
//}


FVector2D UMMOUIMinimap::CvtScenePosToCurMinimapUiPosByScaledCenterPos( 
	FVector mapPos3D, FVector2D pivotPos2D, float cosine, float sine )
{
	MMOMapImgTiles *pMiniMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
	FVector2D mapPos2D = pMiniMapTiles->Map2DPosByWorld3DPos( mapPos3D ) * pMiniMapTiles->_ZoomFactor;

	// rotate in player-space
	FVector2D vTarget;
	vTarget.X = mapPos2D.X * cosine + mapPos2D.Y * sine;
	vTarget.Y = -mapPos2D.X * sine + mapPos2D.Y * cosine;

	float dx = (vTarget.X);
	float dy = (vTarget.Y);

	return FVector2D(pivotPos2D.X + dx, pivotPos2D.Y + dy);
}

//
FVector UMMOUIMinimap::ConvertPosInMapUIToWorld3DPos( INT posX, INT posY )
{
	FVector2D inMap2DPos;

	float cosine = appCos(CameraYawDeg * PI / 180.0f);
	float sine = appCos(CameraYawDeg * PI / 180.0f);
	FVector2D afterTurnPos2D;
	afterTurnPos2D.X = posX * cosine + ( -posY * sine );
	afterTurnPos2D.Y = posX * sine + posY * cosine;

	MMOMapImgTiles* pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
	FVector2D center2dPosInMap = pMapTiles->Map2DPosByWorld3DPos( Player->Location );
	afterTurnPos2D /= pMapTiles->_ZoomFactor;
	return pMapTiles->World3DPosByMap2DPos( center2dPosInMap + afterTurnPos2D );
}

void UMMOUIMinimap::UpdateMapStaticMarks( FVector2D pivotPos, FLOAT fYawDegCosine,FLOAT fYawDegSine )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOMinimap_UpdateMapStaticMarks);
	if( GFXPlayer == NULL ){
		return;
	}

	MMOMapImgTiles *pMiniMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();

	FVector2D temp;

//	float f ;

	INT cnt = staticMarks.Num();
	UGFxObject* dataProvider = NULL;
	INT loopAddedCnt = 0;
	INT maxPerFrameUpdateCnt = 9;
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pStaticMark = &staticMarks(i);
		//if( pStaticMark == NULL ){
		//	continue;
		//}

		if( pStaticMark->_NeedUpdateFlag & EACTUIUF_MiniMap && maxPerFrameUpdateCnt-- > 0 ){
			if( dataProvider == NULL ){
				dataProvider = GFXPlayer->CreateArray();
			}
			//reset update flag
			pStaticMark->_NeedUpdateFlag &= ~EACTUIUF_MiniMap;

			UGFxObject *pUiObj = pStaticMark->_UiObjInMiniMap;
			//if is invalid now, hide unused this ui points
			if( pStaticMark->_IsValid == FALSE ){
				if( pUiObj != NULL ){
					pUiObj->SetVisible( FALSE );
				}
				continue;
			}

			if( pUiObj == NULL ){
				UBOOL createRst = CreateMapMarkUIObject( pStaticMark );
				if( createRst != TRUE ){
					continue;
				}
				pUiObj = pStaticMark->_UiObjInMiniMap;
			}

			//descide show
			UBOOL canShow;
			canShow = ( pStaticMark->_Type != EACTMUI_InVisible ) ? TRUE : FALSE;
			pUiObj->SetVisible( canShow );
			//eventInitMapMark( pUiObj, pStaticMark->_ID, pStaticMark->_Type, pStaticMark->_UiIconIdx );
			//2
			if( dataProvider != NULL ){
				UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapIconUpdateData") );
				if( itemSingleObj != NULL ){
					itemSingleObj->SetObject( TEXT("newMarkObj"), pUiObj );
					itemSingleObj->SetInt( TEXT("id"), pStaticMark->_ID );
					itemSingleObj->SetInt( TEXT("type"), pStaticMark->_Type );
					itemSingleObj->SetString( TEXT("frameName"), pStaticMark->_UiIconIdx);
					dataProvider->SetElementObject( loopAddedCnt, itemSingleObj );
					++loopAddedCnt;
				}
			}
			//f = pUiObj->GetFloat(TEXT("rotation"));
			//pUiObj->SetInt( TEXT("Type"), pStaticMark->_Type );
			//pUiObj->SetInt( TEXT("ID"), pStaticMark->_ID );
			////show correct icon for its type
			//pUiObj->GotoAndStop( pStaticMark->_UiIconIdx );
			//pUiObj->SetFloat( TEXT( "rotation" ), -fYawDeg );
		}//end of if need update

		//if valid,update position
		if( pStaticMark->_IsValid == TRUE && pStaticMark->_Type != EACTMUI_InVisible && pStaticMark->_UiObjInMiniMap != NULL ){
			//FVector2D inMiniMapPos2D = pMiniMapTiles->ScaledMap2DOffsetFromWorld3DPos( pStaticMark->_OrigInMapPos, centerPos3D );
			FVector2D inMiniMapPos2D = CvtScenePosToCurMinimapUiPosByScaledCenterPos( pStaticMark->_OrigInMapPos, pivotPos, fYawDegCosine, fYawDegSine );

			pStaticMark->_UiObjInMiniMap->SetPosition( inMiniMapPos2D.X, inMiniMapPos2D.Y);

		}
	}//end of for loop of all static actors

	if( loopAddedCnt > 0 ){
		eventSetMapMarkIcon( dataProvider );
	}

	////test
	//if( Player->isMoving && cnt >= 8 ){
	//	static int Debug_MapMarkIdx1 = 5;
	//	FActorInMapUI *pStaticMark1 = &staticMarks(Debug_MapMarkIdx1);
	//	static int Debug_MapMarkIdx2 = 8;
	//	FActorInMapUI *pStaticMark2 = &staticMarks(Debug_MapMarkIdx2);
	//	FVector2D in1MiniMapPos2D = CvtScenePosToCurMinimapUiPosByScaledCenterPos( pStaticMark1->_OrigInMapPos, centerPos );
	//	FVector2D in2MiniMapPos2D = CvtScenePosToCurMinimapUiPosByScaledCenterPos( pStaticMark2->_OrigInMapPos, centerPos );
	//	FVector2D diff = in1MiniMapPos2D - in2MiniMapPos2D;

	//	FVector2D in1MiniMapUIPos;
	//	pStaticMark1->_UiObjInMiniMap->GetPosition( in1MiniMapUIPos.X, in1MiniMapUIPos.Y );
	//	SET_WARN_COLOR(COLOR_CYAN);
	//	warnf( NAME_Log, TEXT("======!!!!========= [Map centerPos: Pos:x=%f,y=%f, npcViewRot:%f, mapRot:%f-%f ]") , centerPos.X, centerPos.Y, MapNPCView->GetFloat( TEXT( "rotation" ) ), MapBlock->GetFloat( TEXT( "rotation" ) ), CameraYawDeg );
	//	warnf( NAME_Log, TEXT("======!!!!========= [Map Static1 Mark:%s Pos:x=%f,y=%f InUIPos:x=%f,y=%f]"), *pStaticMark1->_DisplayName, in1MiniMapPos2D.X, in1MiniMapPos2D.Y, in1MiniMapUIPos.X, in1MiniMapUIPos.Y );
	//	FVector2D in2MiniMapUIPos;
	//	pStaticMark2->_UiObjInMiniMap->GetPosition( in2MiniMapUIPos.X, in2MiniMapUIPos.Y );
	//	FVector2D diffOnUI = in1MiniMapUIPos - in2MiniMapUIPos;
	//	warnf( NAME_Log, TEXT("======!!!!========= [Map Static2 Mark:%s Pos:x=%f,y=%f InUIPos:x=%f,y=%f]"), *pStaticMark2->_DisplayName, in2MiniMapPos2D.X, in2MiniMapPos2D.Y, in2MiniMapUIPos.X, in2MiniMapUIPos.Y );

	//	warnf( NAME_Log, TEXT("======!!!!========= [Map Static Mark:%s-%s DisCaculate:x=%f,y=%f DisInUI:x=%f,y=%f]"), *pStaticMark1->_DisplayName, *pStaticMark2->_DisplayName, 
	//		diff.X, diff.Y, diffOnUI.X, diffOnUI.Y );
	//}
}

//
void UMMOUIMinimap::UpdateMarksNearPlayer( FVector2D centerPos, FLOAT fYawDegCosine,FLOAT fYawDegSine )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOMinimap_UpdateMapNearPlayer);
	if( GFXPlayer == NULL ){
		return;
	}

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	const FInMapRegion *curRegion = pMapDataMgr->GetCurReginInCurMap();
	const FBox *curRegionBox = NULL;
	if( curRegion != NULL ){
		curRegionBox = &curRegion->BoundingBox;
	}

	MMOFacade &gameFacade = MMOFacade::getSingleton();

	MMOMapImgTiles *pMiniMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);

	FActorInMapUI *pDynmActor;
	MMOMapDataManager::MapDynamicActorIterator iter = pMapDataMgr->BeginIterateDymcActorNear();
	//UINT key;
	UGFxObject* dataProvider = NULL;
	INT loopAddedCnt = 0;
	INT maxPerFrameUpdateCnt = 10;
	for(  ; iter;  )
	{
		pDynmActor = &iter.Value();
		//UINT key = iter.Key();

		if( ( pDynmActor->_NeedUpdateFlag & EACTUIUF_MiniMap ) && maxPerFrameUpdateCnt-- > 0 ){
			if( dataProvider == NULL ){
				dataProvider = GFXPlayer->CreateArray();
			}
			//reset update flag
			pDynmActor->_NeedUpdateFlag  &= ~EACTUIUF_MiniMap;

			UGFxObject *pUiObj = pDynmActor->_UiObjInMiniMap;
			//if is invalid now, hide unused this ui points
			if( pDynmActor->_IsValid == FALSE ){
				if( pUiObj != NULL ){
					pUiObj->SetVisible( FALSE );
				}

				++iter;
				continue;
			}

			//if ui object is not exit, create an new one
			if( pUiObj == NULL ){
				UBOOL createRst = CreateMapMarkUIObject( pDynmActor );
				if( createRst != TRUE ){
					continue;
				}
				pUiObj = pDynmActor->_UiObjInMiniMap;
			}

			//show it again
			pUiObj->SetVisible( TRUE );
			//eventInitMapMark( pUiObj, pDynmActor->_ID, pDynmActor->_Type, pDynmActor->_UiIconIdx );
			//2
			if( dataProvider != NULL ){
				UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapIconUpdateData") );
				if( itemSingleObj != NULL ){
					itemSingleObj->SetObject( TEXT("newMarkObj"), pUiObj );
					itemSingleObj->SetInt( TEXT("id"), pDynmActor->_ID );
					itemSingleObj->SetInt( TEXT("type"), pDynmActor->_Type );
					itemSingleObj->SetString( TEXT("frameName"), pDynmActor->_UiIconIdx);
					dataProvider->SetElementObject( loopAddedCnt, itemSingleObj );
					++loopAddedCnt;
				}
			}
			//pUiObj->SetInt( TEXT("Type"), pDynmActor->_Type );
			//pUiObj->SetInt( TEXT("ID"), pDynmActor->_ID );

			////show correct icon for its type
			//pUiObj->GotoAndStop( pDynmActor->_UiIconIdx );
		}//end of if need update

		//if valid,update position
		if( pDynmActor->_IsValid == TRUE ){
			AMMOActor* nearActor = gameFacade.GetCharacter( pDynmActor->_ActorID );
			if( nearActor == NULL ){
				//this become invalid,remove it(should have been removed long ago)
				pMapDataMgr->RemoveMapDymcActorNear( pDynmActor->_ActorID );
				if( pDynmActor->_UiObjInMiniMap ){
					pDynmActor->_UiObjInMiniMap->SetVisible( FALSE );
				}
				
				++iter;
				continue;
			}
			if( pDynmActor->_UiObjInMiniMap ){
				//show and hide points on the 3D map
				//if( IsMultilayerRegion && curRegionBox != NULL ){
				//	UBOOL isMarkVisible = curRegionBox->IsInside( nearActor->Location );
				//	pDynmActor->_UiObjInMiniMap->SetVisible( isMarkVisible );
				//}
				//FVector2D inMiniMapPos2D = pMiniMapTiles->ScaledMap2DOffsetFromWorld3DPos( nearActor->Location, centerPos3D );
				FVector2D inMiniMapPos2D = CvtScenePosToCurMinimapUiPosByScaledCenterPos( nearActor->Location, centerPos, fYawDegCosine, fYawDegSine );
				if((pDynmActor->_CurMapPos2D-inMiniMapPos2D).SizeSquared()>0.5)
				{
					pDynmActor->_CurMapPos2D=inMiniMapPos2D;
					pDynmActor->_UiObjInMiniMap->SetPosition( inMiniMapPos2D.X, inMiniMapPos2D.Y );
				}
				else
				{

				}

				//pDynmActor->_UiObjInMiniMap->SetFloat( TEXT( "rotation" ), fYawDeg );
			}
		}

		++iter;
	}//end of for loop of all nearby actors

	if( loopAddedCnt > 0 ){
		SCOPE_CYCLE_COUNTER(STAT_MMOMinimap_UpdateMapNearPlayer_SetMapMarkIcon);
		eventSetMapMarkIcon( dataProvider );
	}
}

//
void UMMOUIMinimap::UpdateMarksGlobalPlayer( FVector2D centerPos, FLOAT fYawDegCosine,FLOAT fYawDegSine )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOMinimap_UpdateMapGlobalPlayer);
	if( GFXPlayer == NULL ){
		return;
	}

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	const FInMapRegion *curRegion = pMapDataMgr->GetCurReginInCurMap();
	const FBox *curRegionBox = NULL;
	if( curRegion != NULL ){
		curRegionBox = &curRegion->BoundingBox;
	}

	MMOFacade &gameFacade = MMOFacade::getSingleton();

	const FExportMap &curMap= pMapDataMgr->GetCurMap();
	INT curMapID = curMap.ServerID;
	INT curMapLineID = pMapDataMgr->GetCurSceneLineIn();

	MMOMapImgTiles *pMiniMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);

	FActorInMapUI *pDynmActor;
	MMOMapDataManager::MapDynamicActorIterator iter = pMapDataMgr->BeginIterateDymcActorGlobal();
	//UINT key;
	UGFxObject* dataProvider = NULL;
	INT loopAddedCnt = 0;
	for(  ; iter;  )
	{
		pDynmActor = &iter.Value();
		//key = iter.Key();

		if( pDynmActor->_NeedUpdateFlag & EACTUIUF_MiniMap ){
			if( dataProvider == NULL ){
				dataProvider = GFXPlayer->CreateArray();
			}
			//reset update flag
			pDynmActor->_NeedUpdateFlag &= ~EACTUIUF_MiniMap;

			UGFxObject *pUiObj = pDynmActor->_UiObjInMiniMap;
			//if is invalid now, hide unused this ui points
			if( pDynmActor->_IsValid == FALSE ){
				if( pUiObj != NULL ){
					pUiObj->SetVisible( FALSE );
				}

				++iter;
				continue;
			}

			//if ui object is not exit, create an new one
			if( pUiObj == NULL ){
				UBOOL createRst = CreateMapMarkUIObject( pDynmActor );
				if( createRst != TRUE ){
					continue;
				}
				pUiObj = pDynmActor->_UiObjInMiniMap;
			}

			//whether in this map
			UBOOL canShow = ( pDynmActor->_InMapID == curMapID );
			pUiObj->SetVisible( canShow );
			//eventInitMapMark( pUiObj, pDynmActor->_ID, pDynmActor->_Type, pDynmActor->_UiIconIdx );
			//2
			if( dataProvider != NULL ){
				UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapIconUpdateData") );
				if( itemSingleObj != NULL ){
					itemSingleObj->SetObject( TEXT("newMarkObj"), pUiObj );
					itemSingleObj->SetInt( TEXT("id"), pDynmActor->_ID );
					itemSingleObj->SetInt( TEXT("type"), pDynmActor->_Type );
					itemSingleObj->SetString( TEXT("frameName"), pDynmActor->_UiIconIdx);
					dataProvider->SetElementObject( loopAddedCnt, itemSingleObj );
					++loopAddedCnt;
				}
			}
			//pUiObj->SetInt( TEXT("Type"), pDynmActor->_Type );
			//pUiObj->SetInt( TEXT("ID"), pDynmActor->_ID );
			////show correct icon for its type
			//pUiObj->GotoAndStop( pDynmActor->_UiIconIdx );
		}//end of if need update

		//if valid,update position
		if( pDynmActor->_IsValid == TRUE ){
			FVector2D inMiniMapPos2D;

			switch( pDynmActor->_Type )
			{
			case EACTMUI_GlobalTeammate:
				{
					AMMOActor* nearActor = gameFacade.GetCharacter( pDynmActor->_ActorID );
					MMOTeamMember *pTeammate = TeamManager::getSingleton().GetMemberById( pDynmActor->_ActorID );
					if( pTeammate == NULL ){
						//this become invalid,remove it(should have been removed long ago)
						pMapDataMgr->RemoveMapDymcActorGlobal( pDynmActor->_ActorID );

						++iter;
						//
						continue;
					}
					//
					FVector teammateWorldPos;
					teammateWorldPos = ( ( nearActor != NULL ) ? nearActor->Location : pTeammate->WorldPos );
					//
					if( pDynmActor->_UiObjInMiniMap ){
						UBOOL canShow = ( pTeammate->MapID == curMapID );
						if( curMap.ServerMapType == E_est_Line ){
							canShow = canShow && ( pTeammate->MapLineID == curMapLineID );
						}
						if( canShow == FALSE ){
							pDynmActor->_UiObjInMiniMap->SetVisible( FALSE );
						}
						else{
							UBOOL isMarkInRegion = ( curRegionBox != NULL && curRegionBox->IsInside( teammateWorldPos ) );
							pDynmActor->_UiObjInMiniMap->SetVisible( isMarkInRegion );
						}
					}
					//
					//inMiniMapPos2D = pMiniMapTiles->ScaledMap2DOffsetFromWorld3DPos( teammateWorldPos, centerPos3D );
					inMiniMapPos2D = CvtScenePosToCurMinimapUiPosByScaledCenterPos( teammateWorldPos, centerPos, fYawDegCosine, fYawDegSine );
				}
				break;
			case EACTMUI_BattleFlag:
				{
					AMMOActor* nearActor = NULL;
					if( pDynmActor->_AidID < (UINT)(-1) ){
						nearActor = gameFacade.GetCharacter( pDynmActor->_AidID );
					}

					if( nearActor != NULL ){
						//inMiniMapPos2D = pMiniMapTiles->ScaledMap2DOffsetFromWorld3DPos( nearActor->Location, centerPos3D );
						inMiniMapPos2D = CvtScenePosToCurMinimapUiPosByScaledCenterPos( nearActor->Location, centerPos, fYawDegCosine, fYawDegSine );
					}
					else{
						//inMiniMapPos2D = pMiniMapTiles->ScaledMap2DOffsetFromWorld3DPos( pDynmActor->_OrigInMapPos, centerPos3D );
						inMiniMapPos2D = CvtScenePosToCurMinimapUiPosByScaledCenterPos( pDynmActor->_OrigInMapPos, centerPos, fYawDegCosine, fYawDegSine );
					}
				}
				break;
			default:
				break;
			}

			//
			if( pDynmActor->_UiObjInMiniMap ){
				pDynmActor->_UiObjInMiniMap->SetPosition( inMiniMapPos2D.X, inMiniMapPos2D.Y );
				//pDynmActor->_UiObjInMiniMap->SetFloat( TEXT( "rotation" ), fYawDeg );
			}
		}

		++iter;
	}//end of for loop of all nearby actors

	if( loopAddedCnt > 0 ){
		eventSetMapMarkIcon( dataProvider );
	}
}

//
void UMMOUIMinimap::UpdateMapPath( FVector2D pivotPos, FLOAT fYawDegCosine, FLOAT fYawDegSine )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOMinimap_UpdateMapPath);
	if( GFXPlayer == NULL ){
		return;
	}

	MMOMapImgTiles *pMiniMapTiles = MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES);
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	FVector curPlayerPos = Player->Location;

	//UBOOL isShowInWorldMap = ( pMapDataMgr->GetCurWorldMap().ServerID == pMapDataMgr->GetCurMap().ServerID );
	TArray< FPathPointInMapUI > &path = pMapDataMgr->GetCurMapPath();
	INT cnt = path.Num();
	INT nowAtPosIdx = -1;
	UINT disBetween = 0xffffffff;
	UGFxObject* dataProvider = NULL;
	INT loopAddedCnt = 0;
	for( INT i = 0; i < cnt; ++i ){
		FPathPointInMapUI *pPathPoint = &path(i);

		if( pPathPoint->_NeedUpdateFlag & EACTUIUF_MiniMap ){
			if( dataProvider == NULL ){
				dataProvider = GFXPlayer->CreateArray();
			}
			//reset update flag
			pPathPoint->_NeedUpdateFlag &= ~EACTUIUF_MiniMap;

			UGFxObject *pUiObj = pPathPoint->_UiObjInMiniMap;
			//if is invalid now, hide unused this ui points
			if( pPathPoint->_IsValid == FALSE ){
				if( pUiObj != NULL ){
					pUiObj->SetVisible( FALSE );
				}
				continue;
			}

			//if ui object is not exit, create an new one
			if( pUiObj == NULL ){
				UBOOL createRst = CreateMapMarkUIObject( pPathPoint );
				if( createRst != TRUE ){
					continue;
				}
				pUiObj = pPathPoint->_UiObjInMiniMap;
			}

			pUiObj->SetVisible( TRUE );
			//eventInitMapMark( pUiObj, pPathPoint->_ID, pPathPoint->_Type, pPathPoint->_UiIconIdx );
			//2
			if( dataProvider != NULL ){
				UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapIconUpdateData") );
				if( itemSingleObj != NULL ){
					itemSingleObj->SetObject( TEXT("newMarkObj"), pUiObj );
					itemSingleObj->SetInt( TEXT("id"), pPathPoint->_ID );
					itemSingleObj->SetInt( TEXT("type"), pPathPoint->_Type );
					itemSingleObj->SetString( TEXT("frameName"), pPathPoint->_UiIconIdx);
					dataProvider->SetElementObject( loopAddedCnt, itemSingleObj );
					++loopAddedCnt;
				}
			}
			//pUiObj->SetInt( TEXT("Type"), pPathPoint->_Type );
			//pUiObj->SetInt( TEXT("ID"), pPathPoint->_ID );
			//pUiObj->GotoAndStop( pPathPoint->_UiIconIdx );
		}//end of if need update

		//if valid,update position
		if( pPathPoint->_IsValid == TRUE && pPathPoint->_UiObjInMiniMap != NULL ){
			FVector2D inMiniMapPos2D = CvtScenePosToCurMinimapUiPosByScaledCenterPos( pPathPoint->_OrigInMapPos, pivotPos, fYawDegCosine, fYawDegSine );
			pPathPoint->_UiObjInMiniMap->SetPosition( inMiniMapPos2D.X, inMiniMapPos2D.Y );

			//UINT curDisBetween = curPlayerPos.DistanceSquared( pPathPoint->_OrigInMapPos );
			INT xDis = curPlayerPos.X - pPathPoint->_OrigInMapPos.X;
			INT yDis = curPlayerPos.Y - pPathPoint->_OrigInMapPos.Y;
			UINT curDisBetween = xDis*xDis + yDis*yDis;
			if( disBetween > curDisBetween ){
				nowAtPosIdx = i;
				disBetween = curDisBetween;
			}
		}
	}//end of for loop of all static actors
	
	if( loopAddedCnt > 0 ){
		eventSetMapMarkIcon( dataProvider );
	}

	//hide points already passed
	INT removePtCnt = 0;
	if( disBetween <= MAP_PATH_POINT_REMOVE_LENGTH ){
		removePtCnt = nowAtPosIdx + 1;
	}
	for( INT i = 0; i < removePtCnt; ++i ){
		FPathPointInMapUI *pPathPoint = &path(i);
		if( pPathPoint->_IsValid == TRUE ){

			if( pPathPoint->_UiObjInWorldMap != NULL ){
				pPathPoint->_UiObjInWorldMap->SetVisible( FALSE );
			}

			if( pPathPoint->_UiObjInMiniMap != NULL ){
				pPathPoint->_UiObjInMiniMap->SetVisible( FALSE );
			}
			pPathPoint->_NeedUpdateFlag |= EACTUIUF_WorldMap;
			pPathPoint->_NeedUpdateFlag |= EACTUIUF_MiniMap;
			pPathPoint->_IsValid = FALSE;
		}
	}

}
//
void UMMOUIMinimap::UpdatePlayerMarksInMap( FVector2D centerPos, FLOAT fYawDegCosine,FLOAT fYawDegSine )
{
}


//============================================================================================================================================
//update the map line you are on
void UMMOUIMinimap::UpdateSceneLinesList()
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();

	//
	FExportMap &curMapInfo = pMapDataManager->GetCurMap();
	UBOOL canChangeLines = ( curMapInfo.ServerMapType == E_est_Line );
	SceneLinesDropDownList->SetVisible( canChangeLines );
	if( canChangeLines == FALSE ){
		return;
	}
	//get the lines of the current map
	TArray< MapLineInfo > &sceneLines = pMapDataManager->GetCurMapLines();
	INT curCombinatedSceneID = pMapDataManager->GetCurSceneLineIn();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	INT linesCnt = sceneLines.Num();
	INT curSelectedIdx = -1;
	FString listItemStringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.SceneLineListString") );
	INT listItemIdx = -1;
	for( INT i = 0; i < linesCnt; ++i ){
		//write control data
		UGFxObject* listObj = GFXPlayer->CreateObject( TEXT("UI_Minimap.MinimapAS3.SceneLinesDropDownListData") );
		if( listObj == NULL ){
			continue;
		}
		INT combinatedSceneID = sceneLines(i)._SceneLineCombineID;
		UINT sceneLineID = 0;
		UINT sceneServerID = 0;
		MMOMapDataManager::ParseServerMapLine( combinatedSceneID, sceneServerID, sceneLineID );
		listObj->SetString( TEXT("label"), FString::Printf( *listItemStringFormat, sceneLineID ) );
		listObj->SetInt( TEXT("lineID"), combinatedSceneID );
		listObj->SetInt( TEXT("lineState"), sceneLines(i)._PercentState );

		if( curCombinatedSceneID == combinatedSceneID ){
			curSelectedIdx = i;
		}
		dataProvider->SetElementObject( ++listItemIdx, listObj );
	}

	eventFlashSceneLinesList( dataProvider, curSelectedIdx );
}

//===================================================================================================================================
void UMMOUIMinimap::PreparingToChangeSceneLine( INT combinatedSceneID )
{
	UMMOUIProgressBarWidget *pProgressBar = (UMMOUIProgressBarWidget*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_ProgressBar );
	if( pProgressBar == NULL ){
		return;
	}

	PrepareSceneLineID = combinatedSceneID;
	UINT sceneLineID = 0;
	UINT sceneServerID = 0;
	MMOMapDataManager::ParseServerMapLine( PrepareSceneLineID, sceneServerID, sceneLineID );
	//send the message first, then run the progress
	CMessageHandler::GetInstance().Send_ChangeToSceneLine( sceneLineID );

	//
	FString strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.PreparingChangeSceneLine") );

	FScriptDelegate fun;
	fun.FunctionName = "NativeFinishPreparingToChangeSceneLine";
	fun.Object = this;
	pProgressBar->Show(TRUE);
	pProgressBar->eventOpen( 5000, strTitle, 0, fun );
}
//===================================================================================================================================
void UMMOUIMinimap::NativeFinishPreparingToChangeSceneLine()
{
	INT curCombinatedSceneID = MMOMapDataManager::getSingleton().GetCurSceneLineIn();
	if( PrepareSceneLineID == curCombinatedSceneID ){
		return;
	}

	UMMOUIProgressBarWidget *pProgressBar = (UMMOUIProgressBarWidget*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_DynamicUI_ProgressBar );
	if( pProgressBar != NULL ){
		pProgressBar->eventClose( TRUE );
		pProgressBar->Show(FALSE);
		//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_ProgressBar, FALSE,"");
	}

	//changed to send the message first then run the progress//request line switch
	//UINT sceneLineID = 0;
	//UINT sceneServerID = 0;
	//MMOMapDataManager::ParseServerMapLine( PrepareSceneLineID, sceneServerID, sceneLineID );
	//CMessageHandler::GetInstance().Send_ChangeToSceneLine( sceneLineID );
	//
	PrepareSceneLineID = curCombinatedSceneID;
}
//===================================================================================================================================
void UMMOUIMinimap::TryInterruptChangingSceneLine()
{
	INT curCombinatedSceneID = MMOMapDataManager::getSingleton().GetCurSceneLineIn();
	if( PrepareSceneLineID == curCombinatedSceneID ){
		return;
	}

	//
	UMMOUIProgressBarWidget *pProgressBar = (UMMOUIProgressBarWidget*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_ProgressBar );
	if( pProgressBar == NULL ){
		return;
	}
	pProgressBar->eventClose( TRUE );
	pProgressBar->Show(FALSE);
	//
	//MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( "MMOGame.Minimap.PreparingChangeSceneLineInterrupt") ,MsgInfoType_Table,TEXT("MMOGame.Minimap.PreparingChangeSceneLineInterrupt"));
	//
	PrepareSceneLineID = curCombinatedSceneID;
	//refresh the list again
	UpdateSceneLinesList();
}
//===================================================================================================================================
void UMMOUIMinimap::ZoomIn()
{
	ZoomLevel++;
	ZoomLevel = Clamp( ZoomLevel , 0 , 4);
	//UGFxObject* mc = GetObject("maprim");
	//if(mc )
	//	if( ZoomLevel == 0 )
	//	{
	//		mc->SetVisible(true);
	//	}
	//	else
	//		mc->SetVisible(false);
	ZoomQuot = (float)ZoomLevel*0.2 + ZoomDefaultQuot;
	MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES)->_ZoomFactor = ZoomQuot;
	
	CurMapViewBound = MapViewBound / ZoomQuot;
}

//
void UMMOUIMinimap::ZoomOut()
{
	ZoomLevel--;
	ZoomLevel = Clamp( ZoomLevel , 0 , 4);
	ZoomQuot = (float)ZoomLevel*0.2 + ZoomDefaultQuot;
	MMOMapImgTiles::GetMapImgTiles(E_MINIMAP_TILES)->_ZoomFactor = ZoomQuot;

	CurMapViewBound = MapViewBound / ZoomQuot;
}

//----------------------------------------------------------------------------------------------------------------
//
void UMMOUIMinimap::NotifyJoinedBattleQueue()
{
	BattleEnterToggleBtn->SetVisible( TRUE );
	eventUCCallStartBattleQueueTimeCountDown( 3600 * 24 * 31 );
}

//
void UMMOUIMinimap::NotifyLeaveBattleQueue()
{
	BattleEnterToggleBtn->SetVisible( FALSE );
	eventUCCallStopBattleQueueTimeCountDown();
}

//----------------------------------------------------------------------------------------------------------------
void UMMOUIMinimap::StopFBTimeCountDownAndHide()
{
	if( FBAidBtn != NULL ){
		FBAidBtn->SetVisible( FALSE );
	}
	//
	eventUCCallStopFBTimeCountDown();
}

//
void UMMOUIMinimap::ReqExitDuplicate_Ok( UGFxObject *pMc )
{
	if( pMc == ConfirmMsgBox && ConfirmMsgBox != NULL ){
		//int type = ConfirmMsgBox->GetInt( TEXT("iteminfoid") );
		CMessageHandler::GetInstance().Send_LeaveScene();
	}

	ConfirmMsgBox = NULL;
}

void UMMOUIMinimap::ReqExitDuplicate_Cancel( UGFxObject *pMc )
{
	ConfirmMsgBox = NULL;
}

//====================================================================================
void UMMOUIMinimap::Init()
{
	Super::Init();
	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}

	InitMc();

	//
	IsCurrentMapShowing = TRUE;
	SwitchMinimapMode();

	//
	check(NULL == EventAdapter);
	if (NULL == EventAdapter)
	{
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_EnterSceneZone);
		evts.AddItem(GameEvent_LeaveSceneZone);
		evts.AddItem(GameEvent_UpdateSceneLines);
		evts.AddItem(GameEvent_EnterScene);
		evts.AddItem(GameEvent_UpdateDayTime);
		evts.AddItem(GameEvent_UpdateWeather);
		evts.AddItem(GameEvent_SceneFinished);
		evts.AddItem(GameEvent_StartNavigate);
		evts.AddItem(GameEvent_StopNavigate);
		evts.AddItem(GameEvent_QuestModeChange);
		EventAdapter = new TGameEventAdapter<UMMOUIMinimap>(this, evts);
	}
}

//
void UMMOUIMinimap::InitMc()
{
	MapNPCView = GetObject( TEXT("MapNpcView_mc") );
	MapPathView = GetObject( TEXT("MapPathView_mc") );
	MapCoord = GetObject( TEXT("mapcoord_mc") );
	MapName = GetObject( TEXT("mapName_mc") );
	PlayerArrow = GetObject( TEXT("PlayerArraw_mc") );
	MapBlock = GetObject( TEXT("mapblock_mc") );
	if( MapBlock != NULL ){
		for( INT i = 0; i < 4; ++i ){
			FString objName = TEXT("MapPiece_");
			objName += appItoa( i + 1 );
			objName += TEXT("_mc");
			UGFxObject *pTile = MapBlock->GetObject( objName );
			MapTiles.Push( pTile );
		}
	}

	//
	MailBtn = GetObject( TEXT("btn_mail_icon") );
	if( MailBtn != NULL ){
		MailBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.MailBtnHint") ) );
	}
	
	MiniMapZoomInBtn = GetObject( TEXT("btnMinimapZoomIn") );
	if( MiniMapZoomInBtn != NULL ){
		MiniMapZoomInBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.MiniMapZoomInBtnHint") ) );
	}

	MiniMapZoomOutBtn = GetObject( TEXT("btnMinimapZoomOut") );
	if( MiniMapZoomOutBtn != NULL ){
		MiniMapZoomOutBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.MiniMapZoomOutBtnHint") ) );
	}

	WorldMapOpenBtn = GetObject( TEXT("ZoneMap") );
	if( WorldMapOpenBtn != NULL ){
		WorldMapOpenBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.WorldMapOpenBtnHint") ) );
	}

	//battle button icon
	ButtonHoverHintMc = GetObject( TEXT("ButtonHoverHintMc") );
	BattleLeaveBtn = GetObject( TEXT("BattleLeaveBtn") );
	BattleEnterToggleBtn = GetObject( TEXT("BattleEnterToggleBtn") );
	//
	FBAidBtn = GetObject( TEXT("FBAidBtn") );
	if( FBAidBtn != NULL ){
		FBAidBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.FBAidBtnHint") ) );
	//	UGFxObject *pTextTf = FBAidBtn->GetObject( TEXT("textTitleTf") );
	//	if( pTextTf != NULL ){
	//		pTextTf->SetString( TEXT("text"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.FBInfo") ) );
	//	}
	}
	//
	SceneLinesDropDownList = GetObject( TEXT("SceneLinesList_mc") );
	SceneIconMc = GetObject( TEXT("SceneIconMc") );

	//
	if( MapNPCView != NULL ){
		MapViewBound = MapNPCView->GetFloat( TEXT("width") ) * 1.414;
	}

	PanelWidth = 248;//minimap base point is no longer top-left GetInt( TEXT("width") );
	PanelHeight = 250;//GetInt( TEXT("height") );
}

//==================================================================
UBOOL UMMOUIMinimap::CreateMapMarkUIObject( FObjInMapUI *pMapMarkData )
{
	if( pMapMarkData == NULL ){
		return FALSE;
	}

	//
	if( pMapMarkData->_UiObjInMiniMap ){
		return TRUE;
	}

	//
	FString uiObjName = FString::Printf( TEXT("MMapMarkIconType%d_%d"), pMapMarkData->_Type, pMapMarkData->_ID );
	UGFxObject *pMarkUiParent = eventCallASGetMapMarkUIObjectContainer( pMapMarkData->_Type );
	if( pMarkUiParent == NULL ){
		return FALSE;
	}

	pMapMarkData->_UiObjInMiniMap = pMarkUiParent->AttachMovie( TEXT("MapMarkIconAOG"), uiObjName );
	if( pMapMarkData->_UiObjInMiniMap == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [Map AttachMovie faile] MapMarkIcon not exit") );
		return FALSE;
	}
	pMapMarkData->_UiObjInMiniMap->AddToRoot();

	//
	//pMapMarkData->_UiObjInMiniMap->SetInt( TEXT("Type"), pMapMarkData->_Type );
	//pMapMarkData->_UiObjInMiniMap->SetInt( TEXT("ID"), pMapMarkData->_ID );
	eventInitMapMark( pMapMarkData->_UiObjInMiniMap, pMapMarkData->_ID, pMapMarkData->_Type, pMapMarkData->_UiIconIdx );
	eventAddMapMarkEventListerner( pMapMarkData->_UiObjInMiniMap );

	return TRUE;
}

//
//UGFxObject* UMMOUIMinimap::eventCallASGetMapMarkUIObjectContainer( INT type )
//{
//	switch( type )
//	{
//	case EACTMUI_PathPoint:
//		return MapPathView;
//	case EACTMUI_StaticNPC:
//	case EACTMUI_NpcGroup:
//	case EACTMUI_QuestZone:
//	case EACTMUI_TeleportFB:
//	case EACTMUI_TeleportMap:
//	case EACTMUI_TeleportInDoor:
//	case EACTMUI_TeleportNode:
//	case EACTMUI_NearMonster:
//	case EACTMUI_NearPlayer:
//	case EACTMUI_NearNormalNPC:
//	case EACTMUI_UserMark:
//	case EACTMUI_GlobalTeammate:
//	case EACTMUI_BattleFlag:
//		return MapNPCView;
//	}
//
//	return MapNPCView;
//}
//==================================================================
void UMMOUIMinimap::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}

	MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();
	FActorInMapUI *pUIActor = NULL;
	//near
	MMOMapDataManager::MapDynamicActorIterator iterNear = mapDataMgr.BeginIterateDymcActorNear();
	for(  ; iterNear; ++iterNear ){
		pUIActor = &iterNear.Value();
		pUIActor->_NeedUpdateFlag |= EACTUIUF_MiniMap;
		//
		DistoryRootMc( pUIActor->_UiObjInMiniMap );
	}
	//Global
	MMOMapDataManager::MapDynamicActorIterator iterGlobal = mapDataMgr.BeginIterateDymcActorGlobal();
	for(  ; iterGlobal; ++iterGlobal ){
		pUIActor = &iterGlobal.Value();
		pUIActor->_NeedUpdateFlag |= EACTUIUF_MiniMap;

		//
		DistoryRootMc( pUIActor->_UiObjInMiniMap );
	}

	//clear static map ui objects
	TArray< FActorInMapUI > &listCurMapUIStaticActors = mapDataMgr.GetCurMapStaticActorPointsList();
	for( INT i = listCurMapUIStaticActors.Num() - 1; i >= 0; --i ){
		pUIActor = &listCurMapUIStaticActors( i );
		pUIActor->_NeedUpdateFlag |= EACTUIUF_MiniMap;
		
		//
		DistoryRootMc( pUIActor->_UiObjInMiniMap );
	}

	//TArray< FPathPointInMapUI > &mapCurPath = mapDataMgr.GetAllMapPathResource();
	//UINT posCnt = mapCurPath.Num();
	//FPathPointInMapUI *pPathPosUi = NULL;
	//for( UINT i = 0; i < posCnt; ++i ){
	//	pPathPosUi = &mapCurPath(i);
	//	pPathPosUi->_NeedUpdateFlag |= EACTUIUF_MiniMap;

	//	DistoryRootMc( pPathPosUi->_UiObjInMiniMap );
	//}
}

//
void UMMOUIMinimap::DistoryRootMc( UGFxObject* &pMc )
{
	if( pMc == NULL ){
		return;
	}
	eventRemoveChild( pMc );
	pMc->RemoveFromRoot();
	pMc = NULL;
}

//=========================================================
void UMMOUIMinimap::OnClickBattleInfoBtn( INT x , INT y )
{
	if( ButtonHoverHintMc != NULL ){
		ButtonHoverHintMc->SetVisible( FALSE );
	}

	//
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	FString fixText;
	if( !pBattleManager->IsWaiting() ){
		return;
	}
	INT battleID = pBattleManager->GetJoinedBattleID();
	FBattleTemplate *pBattle = pBattleManager->FindBattleTemplateBy( battleID );
	if( pBattle == NULL ){
		return;
	}
	//
	MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_BattleEnter, x, y, 0, pBattle->DisplayName, 0 );
}

//-------------------------------------------------------------
void UMMOUIMinimap::HintBattleInfo()
{
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	FString fixText;
	if( !pBattleManager->IsWaiting() ){
		return;
	}
	INT battleID = pBattleManager->GetJoinedBattleID();
	FBattleTemplate *pBattle = pBattleManager->FindBattleTemplateBy( battleID );
	if( pBattle == NULL ){
		return;
	}

	eventUCCallSetHoverHintTitle( pBattle->DisplayName );

	//
	FString displayTextFormat = MMOUI::ParseLocalizedPropertyPath( TEXT( "MMOGame.BattleField.BattleQueueTimeHint" ) );
	eventUCCallShowBattleBtnHoverHint( displayTextFormat );
}

//=========================================================
void UMMOUIMinimap::HideHintBattleInfo()
{
	if( ButtonHoverHintMc != NULL ){
		ButtonHoverHintMc->SetVisible( FALSE );
	}
}
//=========================================================
void UMMOUIMinimap::SwitchMinimapToPathMap()
{
	eventCallASToggleShowMap( FALSE );
	IsSmallMinimapMode = FALSE;

	//
	UMMOUIPathMap *pathMapPanel = (UMMOUIPathMap*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_PathMap );
	if( pathMapPanel != NULL ){
		pathMapPanel->Show( IsCurrentMapShowing );
	}
}
//
void UMMOUIMinimap::ToggleMinimapShowType()
{
	//disable this feature for now

	//if( IsSmallMinimapMode ){
	//	SwitchMinimapToPathMap();
	//}
	//else{
	//	SwitchMinimapMode();
	//	//
	//	UMMOUIPathMap *pathMapPanel = (UMMOUIPathMap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_PathMap );
	//	if( pathMapPanel != NULL ){
	//		pathMapPanel->Show( FALSE );
	//	}
	//}
}
//=========================================================
void UMMOUIMinimap::ToggleShowMap()
{
	IsCurrentMapShowing = !IsCurrentMapShowing;
	if( IsSmallMinimapMode ){
		eventCallASToggleShowMap( IsCurrentMapShowing );
	}
	else{
		//UMMOUIPathMap *pathMapPanel = (UMMOUIPathMap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_PathMap );
		MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PathMap, IsCurrentMapShowing, TEXT("") );
	}
}
//
void UMMOUIMinimap::ShowTipsToggleShowMap()
{
	UGFxObject *pToggleBtn = GetObject( TEXT("ToggleShowMapBtn") );
	if( pToggleBtn == NULL ){
		return;
	}

	FString tipsText;
	if( IsCurrentMapShowing ){
		tipsText =  MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.ToggleShowMapBtnHintHide") );
	}
	else{
		tipsText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.ToggleShowMapBtnHintShow") );
	}

	pToggleBtn->SetString( TEXT("tipsHtml"), tipsText );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, pToggleBtn );
}
//=========================================================
void UMMOUIMinimap::SwitchMinimapMode()
{
	IsSmallMinimapMode = TRUE;
	eventCallASToggleShowMap( IsCurrentMapShowing );

}
//=========================================================
void UMMOUIMinimap::DesideToShowMinimap()
{
	eventCallASToggleShowMap( IsCurrentMapShowing );

}

void UMMOUIMinimap::UpdatePingRate(float DeltaTime)
{
	//tick
	const static INT cs_Fluency = 3;
	const static INT cs_Normal  = 2;
	const static INT cs_Block   = 1;

	static FLOAT nLastFrameUpdateTime = 1.0f;
	static INT   nFrameCount = 30;
	if(nLastFrameUpdateTime >= 1.0f)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if(object)
		{
			UINT32 nPin = MMOFacade::getSingleton().GetGameWorldPingDelayTime();
			INT nSign = cs_Fluency;
			if(nPin >= 100 && nPin <= 150)
				nSign = cs_Normal;
			if(nPin > 150)
				nSign = cs_Block;
			object->SetInt("ftp",nFrameCount/*nPin*/);
			object->SetInt("sign",nSign);
			eventCallUCupdateSign(object);

			/*
			 when the current game window is the active desktop window, sample the in-game frame rate
			*/
			extern HWND GGameWindow;
			if(/*!(::IsIconic(GGameWindow))*/::GetActiveWindow() == GGameWindow)
			{
				MMOFacade::getSingleton().AddNewGameStateFPSData(nFrameCount);
			}
		}
		nFrameCount = 0;
		nLastFrameUpdateTime = 0.0f;
	}
	else
	{
		nFrameCount++;
		nLastFrameUpdateTime += DeltaTime;
	}
}
void UMMOUIMinimap::UpdateFramePinVisible()
{
	UGFxObject* sign_mc = GetObject( TEXT("btn_sign"));
	if (sign_mc)
	{
		UGFxObject* pkText = sign_mc->GetObject( TEXT("txtSign"));
		UGFxObject* pkText_FPS = sign_mc->GetObject( TEXT("txtFps"));
		if(pkText && pkText_FPS)
		{
			pkText->SetVisible(MMOUI::GetUIManager()->DrawGameFrame);
			pkText_FPS->SetVisible(MMOUI::GetUIManager()->DrawGameFrame);
		}
	}
}

//
void UMMOUIMinimap::UpdateArenaEntVisible()
{
	UGFxObject* btn = GetObject( TEXT("ArenaEntToggleBtn"));
	if( btn != NULL ){
		UBOOL isArenaDay = MMOArenaManager::getSingleton().IsArenaDay();
		//
		FExportMap &curScene = MMOMapDataManager::getSingleton().GetCurMap();
		UBOOL isInArena = ( curScene.ServerMapType == E_est_Arena );
		btn->SetVisible( isArenaDay && !isInArena );
	}
}

FString UMMOUIMinimap::GetFPS()
{
	UGFxObject* sign_mc = GetObject( TEXT("btn_sign"));
	if (sign_mc)
	{
		UGFxObject* pkText_FPS = sign_mc->GetObject( TEXT("txtSign"));
		if(pkText_FPS)
		{
			return pkText_FPS->GetString(TEXT("text"));
		}
	}
	return "";
}
//===============================================================================================================
//===============================================================================================================
void UMMOUIMinimap::BeforeViewUpdate(float DeltaTime)
{
	if(MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}
	//Super::PostAdvance(DeltaTime);
	//mini Map
	if( IsVisible )
	{
		UpdateMinimap();
		UpdatePingRate(DeltaTime);
	}
}
//========================================================================================================
UBOOL UMMOUIMinimap::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( strHoverControllerName == TEXT("PlayerArraw_mc") )
	{
		return TRUE;
	}

	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}
//========================================================================================================
void UMMOUIMinimap::ASMoveToMinimapPos( INT clickPosX, INT clickPosY )
{
	FVector world3DPos = ConvertPosInMapUIToWorld3DPos( clickPosX, clickPosY );

	FExportMap curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	MMOFacade::getSingleton().PlayerMoveToScenePos( curMapInfo, world3DPos );
}
//========================================================================================================
void UMMOUIMinimap::ASShowMapLineDropList()
{
	CMessageHandler::GetInstance().Send_QuerySceneLine();
}
//========================================================================================================
void UMMOUIMinimap::ASChangeSceneLine( int lineID )
{
	ClientActionCondition_ChangeSceneLine checker;
	FClientActionConditionContex contex;
	if( !checker.CheckClientActionCondition(contex) ){
		UpdateSceneLinesList();
		return;
	}

	INT curCombinatedSceneID = MMOMapDataManager::getSingleton().GetCurSceneLineIn();
	if( curCombinatedSceneID == lineID ){
		FString errorName = TEXT("MMOGame.Minimap.ErrorChangeSceneLineIsCurrent");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return;
	}
	//
	PreparingToChangeSceneLine( lineID );
}

//========================================================================================================
void UMMOUIMinimap::ASShowHintMapType( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	//
	FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	FString stringLookforKey = TEXT("MMOGame.Minimap.SceneType");
	stringLookforKey += appItoa( curMapInfo.ServerMapType );

	mc->SetString( TEXT("tipsHtml"), MMOUI::ParseLocalizedPropertyPath( stringLookforKey ) );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

//
void UMMOUIMinimap::ASHideHintMapType()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//========================================================================================================
void UMMOUIMinimap::ASClickMailIcon()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Mail, "");
}

//========================================================================================================
void UMMOUIMinimap::ASShowMinimapMarkHint( INT type, INT idx )
{
	warnf( TEXT( "ASShowMinimapMarkHint at Type:%d Idx:%d"), type, idx );

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	FString tipsText;
	FObjInMapUI *pMapMark = NULL;
	switch( type )
	{
	case EACTMUI_StaticNPC:
	case EACTMUI_TeleportFB:
	case EACTMUI_TeleportMap:
	case EACTMUI_TeleportInDoor:
	case EACTMUI_TeleportNode:
	case EACTMUI_QuestZone:
		{
			TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();
			if( staticMarks.IsValidIndex( idx ) ){
				pMapMark = &staticMarks( idx );
			}
			if( pMapMark != NULL ){
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;
	case EACTMUI_NearMonster:
	case EACTMUI_NearBoss:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorNearByUiID( idx );
			if( pMapMark != NULL ){
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;
	case EACTMUI_NpcGroup:
		{
			TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();
			if( staticMarks.IsValidIndex(idx) ){
				pMapMark = &staticMarks(idx);

				if( pMapMark != NULL ){
					INT strPos = pMapMark->_DisplayName.InStr( TEXT(":") );
					if( strPos >= 0 ){
						tipsText = pMapMark->_DisplayName.Left( strPos );
					}
				}
			}
		}
		break;
	case EACTMUI_NearPlayer:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorNearByUiID( idx );
			//because when this player was added, his info had not arrived yet
			if( pMapMark != NULL ){
				AMMOActor *pPlayer = MMOFacade::getSingleton().GetCharacter(  ((FActorInMapUI*)pMapMark)->_ActorID );
				if( pPlayer != NULL ){
					pMapMark->_DisplayName = pPlayer->GetActorName();
				}
				tipsText = pMapMark->_DisplayName;
			}

			////because when this player was added, his info had not arrived yet
			//if( pMapMark != NULL ){
			//	AMMOActor *pPlayer = MMOFacade::getSingleton().GetCharacter(  ((FActorInMapUI*)pMapMark)->_ActorID );
			//	if( pPlayer != NULL ){
			//		pMapMark->_DisplayName = pPlayer->RoleName;
			//	}
			//	tipsText = pMapMark->_DisplayName;
			//}
		}
		break;
	case EACTMUI_GlobalTeammate:
	case EACTMUI_BattleFlag:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorGlobalByUiID( idx );
			if( pMapMark != NULL ){
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;
	default:
		break;
	}

	//
	if( pMapMark == NULL ){
		return;
	}

	//
	UGFxObject *pMc = pMapMark->_UiObjInMiniMap;
	if( pMc == NULL ){
		return;
	}

	//
	if( tipsText.IsEmpty() || tipsText.Len()<2){
		return;
	}


	pMc->SetString( TEXT("tipsHtml"), tipsText );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, pMapMark->_UiObjInMiniMap );
	FSMIO::getSingleton().Trigger.SetShowTips(TRUE,TRUE);
}

void UMMOUIMinimap::ASHideMinimapMarkHint()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//============================================================================
//
void UMMOUIMinimap::ASClickBattleInfo( INT x , INT y )
{
	OnClickBattleInfoBtn( x, y );
}

//
void UMMOUIMinimap::ASHintBattleInfo()
{
	HintBattleInfo();
}

//
void UMMOUIMinimap::ASHideHintBattleInfo()
{
	HideHintBattleInfo();
}

//==============================================================================================
void UMMOUIMinimap::ASToggleArenaEnt()
{
	UMMOUIArenaEntrance *pArenaEnt = (UMMOUIArenaEntrance*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Arena_ArenaEntrance );
	if( pArenaEnt == NULL ){
		pArenaEnt = (UMMOUIArenaEntrance*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Arena_ArenaEntrance );
		if( pArenaEnt != NULL ){
			pArenaEnt->Show( TRUE );
		}
	}
	else{
		pArenaEnt->Show( !pArenaEnt->IsVisible );
	}
}

//==============================================================================================
void UMMOUIMinimap::ASClickFbInfo( INT x , INT y )
{
	FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_Dungeon, x, y, 0, curMapInfo.MapDisplayName, 0 );
}
//
void UMMOUIMinimap::ASNotifyFBTimesUp()
{
	//StopFBTimeCountDownAndHide();
}

//==============================================================================================
void UMMOUIMinimap::ASToggleMap()
{
	ToggleMinimapShowType();
}

//---------------------------------------------------------------
void UMMOUIMinimap::ASShowTipsToggleShowMap()
{
	ShowTipsToggleShowMap();
}
//==============================================================================================
void UMMOUIMinimap::ASToggleShowMap()
{
	ToggleShowMap();
}

//==============================================================================================
void UMMOUIMinimap::ASStopPathFind()
{
	MMONavigationManager::getSingleton().StopNavigate();
}

//==============================================================================================
void UMMOUIMinimap::ASClickShoppingMallBtn()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Mall, "");
}
void UMMOUIMinimap::ASClickActivityBtn()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Activity, "");
}
////==============================================================================================
//void UMMOUIMinimap::Init()
//{
//	Super::Init();
//
//	MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Minimap);
//
//	MMOUI::GetUIManager()->SetUpdateMinimapDelegate(FScriptDelegate(this, TEXT("UpdateMinimap")));
//	MMOUI::GetUIManager()->SetUpdateTraceReputationDataDelegate(FScriptDelegate(this, TEXT("UpdateTraceReputationData")));
//
//}
//
//void UMMOUIMinimap::UnInit()
//{
//	Super::UnInit();
//	MMOUI::GetUIManager()->SetUpdateTraceReputationDataDelegate(FScriptDelegate(EC_EventParm));
//	MMOUI::GetUIManager()->SetUpdateMinimapDelegate(FScriptDelegate(EC_EventParm));
//}

void UMMOUIMinimap::UpdateTraceReputationData( UGFxObject *data)
{
	if( data ){
		UGFxObject * ReputationObj = GFXPlayer->CreateObject("Object");
		if(ReputationObj){
			ReputationObj->SetString("label",data->GetString("label"));
			ReputationObj->SetInt("index",0);
			ReputationObj->SetInt("type",data->GetInt("type"));
			ReputationObj->SetInt("deepth",data->GetInt("deepth"));
			ReputationObj->SetString("tips",data->GetString("tips"));
			UBOOL bV = data->GetBool("visible");
			ReputationObj->SetBool("visible",bV);
			if(bV)
			{
				ReputationObj->SetInt("colorReputation",data->GetInt("colorReputation"));
				ReputationObj->SetInt("curReputation",data->GetInt("curReputation"));
				ReputationObj->SetInt("minReputation",data->GetInt("minReputation"));
				ReputationObj->SetInt("maxReputation",data->GetInt("maxReputation"));
				ReputationObj->SetString("txtReputation",data->GetString("txtReputation"));
			}


			
			IsReputationTrace = bV;
			eventCallUpdateTraceRelation(ReputationObj);

			UMMOUIFBTask* fbTaskPanel = (UMMOUIFBTask*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_FBTask);
			if (fbTaskPanel && fbTaskPanel->IsVisible)
			{
				fbTaskPanel->UpdatePosition();
			}
			else
			{
//				UMMOUIQuestTracePanel* questTracePanel = (UMMOUIQuestTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_QuestTrace);
//				if (questTracePanel && questTracePanel->IsVisible)
//				{
//					questTracePanel->UpdatePosition();
//				}
			}
		}
	}

}
//============================================================================================
void UMMOUIMinimap::ASShowTips_PinValue(class UGFxObject* mc)
{
	//
	if(mc == NULL)
		return;

	UINT32 nPin = MMOFacade::getSingleton().GetGameWorldPingDelayTime();
	FString nFPS = GetFPS();//MMOFacade::getSingleton().GetGatePingDelayTime();
	UINT32 nGate =  MMOFacade::getSingleton().GetGatePingDelayTime();
	FString tipStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FPS.Ping"));
	FString tips = FString::Printf(*tipStr, nPin,nGate,*nFPS);
	mc->SetString( TEXT("tipsHtml"),tips);
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
	
	bShowTips = TRUE;

}
void UMMOUIMinimap::ASHideTips_PinValue(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
	bShowTips = FALSE;
}
//===================================================================================================================================
void UMMOUIMinimap::ASShowMiniMapTrace()
{
	/*
	UMMOUIQuestTracePanel* questTracePanel = (UMMOUIQuestTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_QuestTrace);
	if (questTracePanel)
	{
		questTracePanel->_IsExpand = TRUE;
		questTracePanel->Show(TRUE);
	}

	UMMOUIFBTask* fbTaskPanel = (UMMOUIFBTask*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_FBTask);
	if (fbTaskPanel)
	{
		fbTaskPanel->Show(TRUE);
	}
	*/
	UMMOUIJourneyTracePanel* journeyPanel = (UMMOUIJourneyTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Journey);
	if (journeyPanel)
	{
		journeyPanel->Show(TRUE);
	}
	
}

void UMMOUIMinimap::ASHideMiniMapTrace()
{
	/*
	UMMOUIQuestTracePanel* questTracePanel = (UMMOUIQuestTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_QuestTrace);
	if (questTracePanel && questTracePanel->IsVisible)
	{
		questTracePanel->_IsExpand = FALSE;
		questTracePanel->Show(FALSE);
	}

	UMMOUIFBTask* fbTaskPanel = (UMMOUIFBTask*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_FBTask);
	if (fbTaskPanel && fbTaskPanel->IsVisible)
	{
		fbTaskPanel->Show(FALSE);
	}
	*/
	UMMOUIJourneyTracePanel* journeyPanel = (UMMOUIJourneyTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Journey);
	if (journeyPanel)
	{
		journeyPanel->Show(FALSE);
	}
}

//===================================================================================================================================
void UMMOUIMinimap::ASUpdateFPSTips(class UGFxObject* mc)
{
	if (bShowTips)
		ASShowTips_PinValue(mc);
	
}

//===================================================================================================================================
void UMMOUIMinimap::ASShowTips_MapLineInfo( INT stateType )
{
	FString tipsStrName = TEXT("MMOGame.Minimap.MapLineHint");
	tipsStrName += appItoa( stateType );
	FString tipStr = MMOUI::ParseLocalizedPropertyPath( tipsStrName );

	MMOUI::GetUIManager()->delegateShowTextTips( tipStr );
}

//--------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIMinimap::ASHideTips_MapLineInfo()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//===================================================================================================================================
void UMMOUIMinimap::ASShowTips_MapZone( INT zoneType )
{
	FString tipsStrName = TEXT("MMOGame.Minimap.MapZoneHint");
	tipsStrName += appItoa( zoneType );
	FString tipStr = MMOUI::ParseLocalizedPropertyPath( tipsStrName );

	MMOUI::GetUIManager()->delegateShowTextTips( tipStr );
}

//--------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIMinimap::ASHideTips_MapZone()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//===============================================================================================================
//===============================================================================================================