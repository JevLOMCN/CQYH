#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapImgTiles.h"
#include "MMOMapDataManager.h"
#include "TeamManager.h"

IMPLEMENT_CLASS(UMMOUIPathMap)

void UMMOUIPathMap::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	//check(NULL != evt);
	if (NULL != evt)
	{
		switch (evt->GetEventType())
		{
		case GameEvent_UpdateCurUIMap:
			{
				if( !IsVisible ){
					break;
				}

				//
				InitPathMapTilesByCurSceneData();
			}
			break;
		}
	}
}

void UMMOUIPathMap::ClearUI()
{

}
//================================================================================================
void UMMOUIPathMap::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		INT curReginIdx = MMOMapDataManager::getSingleton().GetCurReginIdxInMap();
		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		if( curReginIdx != InRegionIdx && curMapInfo.MapName != CurMapTexName ){
			InitPathMap();
		}
	}
	else{
		UMMOUIMinimap *pMiniMapPanel = ( UMMOUIMinimap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
		if( pMiniMapPanel != NULL ){
			pMiniMapPanel->DesideToShowMinimap();
		}
	}
}
//================================================================================================
void UMMOUIPathMap::ChangeScene()
{
	if( !IsVisible ){
		return;
	}

	InitPathMap();
}

//================================================================================================
//called when entering a new scene
void UMMOUIPathMap::InitPathMap()
{
	AMMOPlayerController *pCtrl;
	pCtrl = MMOFacade::getSingleton().GetMainController();
	if( pCtrl == NULL || Player == NULL ){
		return;
	}

	//rotation the hole panel to be the same as the player orientation
	if( ctrl != NULL ){
		CameraYawDeg = Cast<AMMOPlayerCamera>(ctrl->PlayerCamera)->curVT.POV.Rotation.Yaw * C_UnrRotToDeg;
		if( MapPaper ){
			MapPaper->SetFloat( "rotation", -CameraYawDeg );
		}
		if( MapNPCView ){
			MapNPCView->SetFloat( "rotation", -CameraYawDeg );
		}
		if( MapPathView ){
			MapPathView->SetFloat( "rotation", -CameraYawDeg );
		}
	}

	//update map fixed points
	InitMapStaticMarks();

	//
	InitPathMapTilesByCurSceneData();
}

//====================================================================================================
void UMMOUIPathMap::InitPathMapTilesByCurSceneData()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	FExportMap &curMapInfo = pMapDataMgr->GetCurMap();

	//initialize map tiles
	INT regionIdx = 0;
	regionIdx = pMapDataMgr->GetCurPlayerRegionIn( Player->Location );

	//
	FInMapRegion *pCurReginInCurMap = pMapDataMgr->GetReginInCurMap( regionIdx );
	InitPathMapTiles( curMapInfo.MapName, *pCurReginInCurMap );
}
//
void UMMOUIPathMap::InitPathMapTiles( const FString& inMapFileName, FInMapRegion RegionInMap )
{
	MMOMapImgTiles *pPathMapTiles = MMOMapImgTiles::GetMapImgTiles(E_PATHMAP_TILES);
	if( pPathMapTiles == NULL ){
		return;
	}
	//prepare the tile picture loader
	bool bLoadOK = pPathMapTiles->LoadUiTile( MapTiles, TEXT("pathmap_alpha%d -nopack") );
	if( !bLoadOK ){
		return;
	}

	//initialize world map tiles data
	FString strTexPath;
	FString texPackName;
	texPackName = inMapFileName + TEXT("_") + RegionInMap.VolumeName;
	if( !RegionInMap.ReuseMapVolumeName.IsEmpty() ){
		texPackName = inMapFileName + TEXT("_") + RegionInMap.ReuseMapVolumeName;
	}

	strTexPath = FString::Printf( TEXT("%s_wMap.%s_wMap"), *texPackName, *texPackName );

	//
	FString strValueKey;
	INT tileSizeX;
	strValueKey = TEXT("WorldMapTileSizeX");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeX, GGameIni );

	strValueKey = TEXT("WorldMapTileSizeY");
	INT tileSizeY;
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeY, GGameIni );
	//
	INT maxImgTileRowNum = RegionInMap.WorldMapTileRowNum;
	INT maxImgTileColNum = RegionInMap.WorldMapTileColNum;

	FVector2D map3Dto2DNoScaled;
	//map3Dto2DNoScaled.X = (FLOAT)realCoordStepToPixels / MAP_SNAPSHOT_COORDINATE_STEP;
	map3Dto2DNoScaled.X = (FLOAT)1 / RegionInMap.WorldMapPixelSize;
	map3Dto2DNoScaled.Y = -map3Dto2DNoScaled.X;

	//scale
	//FLOAT fScale = RegionInMap.WorldMapScale;
	FVector2D map3Dto2D;
	//map3Dto2D = map3Dto2DNoScaled * fScale;
	map3Dto2D = map3Dto2DNoScaled;

	//
	INT minimapX, minimapY;
	GConfig->GetInt( TEXT("MapImg"), TEXT("MiniMapTileSizeX"), minimapX, GGameIni );
	GConfig->GetInt( TEXT("MapImg"), TEXT("MiniMapTileSizeY"), minimapY, GGameIni );
	INT origImgSizeX = RegionInMap.MiniMapTileRowNum * minimapX;
	INT origImgSizeY = RegionInMap.MiniMapTileColNum * minimapY;

	UINT trueWorldMapWidth = origImgSizeX;//origImgSizeX * RegionInMap.WorldMapScale;
	UINT trueWorldMapHeight = origImgSizeY;//origImgSizeY * RegionInMap.WorldMapScale;

	//offset
	FBox regionBox = RegionInMap.BoundingBox;

	FVector2D afterScale2dOffsetInMap;
	afterScale2dOffsetInMap.X = -regionBox.Min.Y * map3Dto2D.X;
	afterScale2dOffsetInMap.Y = -regionBox.Min.X * map3Dto2D.Y + trueWorldMapHeight;

	//
	//FVector2D afterTurned2dOffsetInMap( -origImgSizeX * ( 1 - RegionInMap.WorldMapScale ) * SIN_MAP, origImgSizeY * ( 1 - COS_MAP ) );
	FVector2D afterTurned2dOffsetInMap;
	afterTurned2dOffsetInMap.X = RegionInMap.WorldMapOffsetX;
	afterTurned2dOffsetInMap.Y = RegionInMap.WorldMapOffsetY - trueWorldMapHeight * COS_MAP;

	//
	InRegionIdx = MMOMapDataManager::getSingleton().GetCurReginIdxInMap();
	//if the scene has Multilayer region ,minimap will update map mark's visiblity every frame
	IsMultilayerRegion = MMOMapDataManager::getSingleton().IsCurMapRegionMultilayer();
	//
	CurMapTexName = inMapFileName;

	//
	if( trueWorldMapWidth != 0 ){
		FLOAT sinMap = 0.70710678118654752440084436210485;//45 degrees
		FLOAT cosMap = 0.70710678118654752440084436210485;//45 degrees
		FLOAT edge = ( trueWorldMapHeight * sinMap / cosMap + trueWorldMapWidth );
		FLOAT edge1 = edge * sinMap;
		FLOAT edge2 = edge * cosMap;
		FLOAT longEdge = edge1 > edge2 ? edge1 : edge2;
		ZoomDefaultQuot = MapVisibleZoneSize.X / longEdge;
	}
	else{//should not get here normally
		ZoomDefaultQuot = MapVisibleZoneSize.X / 512;
	}
	ResizeMap( ZoomDefaultQuot );

	//
	FVector player3dPos;
	//if( !isResetPos ){
	//	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	//	if( player != NULL ){
	//		player3dPos = player->Location;
	//	}
	//}
	//else{
	MapUiCenterPos.X = PaperZoneSizeHalf.X;
	MapUiCenterPos.Y = PaperZoneSizeHalf.Y;

	FVector2D beforeTurnPos2D;
	beforeTurnPos2D.X = MapUiCenterPos.X - afterTurned2dOffsetInMap.X;
	beforeTurnPos2D.Y = MapUiCenterPos.Y - afterTurned2dOffsetInMap.Y;
	beforeTurnPos2D.X = -beforeTurnPos2D.Y * SIN_MAP + beforeTurnPos2D.X * COS_MAP;
	beforeTurnPos2D.Y =  beforeTurnPos2D.Y * COS_MAP + beforeTurnPos2D.X * SIN_MAP;

	player3dPos.Y = ( beforeTurnPos2D.X  - afterScale2dOffsetInMap.X ) / map3Dto2D.X;
	player3dPos.X = ( beforeTurnPos2D.Y - afterScale2dOffsetInMap.Y ) / map3Dto2D.Y;
	//}
	pPathMapTiles->Initialize( GFXPlayer, strTexPath, player3dPos, tileSizeX, tileSizeY, 
		maxImgTileRowNum, maxImgTileColNum, map3Dto2D, afterScale2dOffsetInMap, afterTurned2dOffsetInMap, ZoomDefaultQuot );
	//
	pPathMapTiles->TilesGoAroundByPosInTile( MapUiCenterPos, true );
	pPathMapTiles->TileTheMap();
}
//====================================================================================================
void UMMOUIPathMap::ResizeMap( FLOAT scaleValue )
{
	MMOMapImgTiles* pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	if( MapPaper != NULL ){
		FASDisplayInfo DI = MapPaper->GetDisplayInfo();
		DI.hasXScale = true;
		DI.hasYScale = true;
		DI.XScale = scaleValue * 100;
		DI.YScale = scaleValue * 100;
		MapPaper->SetDisplayInfo(DI);
	}
}
//====================================================================================================
void UMMOUIPathMap::ClearPathMapUI()
{
	MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();
	FActorInMapUI *pUIActor = NULL;
	//near
	MMOMapDataManager::MapDynamicActorIterator iterNear = mapDataMgr.BeginIterateDymcActorNear();
	for(  ; iterNear; ++iterNear ){
		pUIActor = &iterNear.Value();
		pUIActor->_NeedUpdateFlag |= EACTUIUF_PathMap;
		//
		DistoryRootMc( pUIActor->_UiObjInPathMap );
	}
	//Global
	MMOMapDataManager::MapDynamicActorIterator iterGlobal = mapDataMgr.BeginIterateDymcActorGlobal();
	for(  ; iterGlobal; ++iterGlobal ){
		pUIActor = &iterGlobal.Value();
		pUIActor->_NeedUpdateFlag |= EACTUIUF_PathMap;

		//
		DistoryRootMc( pUIActor->_UiObjInPathMap );
	}

	//clear static map ui objects
	TArray< FActorInMapUI > &listCurMapUIStaticActors = mapDataMgr.GetCurMapStaticActorPointsList();
	for( INT i = listCurMapUIStaticActors.Num() - 1; i >= 0; --i ){
		pUIActor = &listCurMapUIStaticActors( i );
		pUIActor->_NeedUpdateFlag |= EACTUIUF_PathMap;

		//
		DistoryRootMc( pUIActor->_UiObjInPathMap );
	}
}
//
void UMMOUIPathMap::InitMapStaticMarks()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	pMapDataMgr->PrepareCurMapStaticActorInMapUI();

	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();

	INT cnt = staticMarks.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pStaticMark = &staticMarks(i);

		UGFxObject *pUiObj = pStaticMark->_UiObjInPathMap;
		//if ui object is not exit, create an new one
		if( pUiObj == NULL ){
			UBOOL createRst = CreateMapMarkUIObject( pStaticMark );
			if( createRst != TRUE ){
				continue;
			}
			pUiObj = pStaticMark->_UiObjInPathMap;
		}
	}
}

//===================================================================================================================================
void UMMOUIPathMap::UpdatePathMap()
{
	AMMOPlayerController* ctrl =	MMOFacade::getSingleton().GetMainController();
	if( ctrl == NULL ){
		return;
	}

	AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
	if( player == NULL ){
		return;
	}

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	INT regionIdx = pMapDataMgr->GetCurPlayerRegionIn( player->Location );

	MMOMapImgTiles* pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_PATHMAP_TILES);
	if( CurMapTexName.IsEmpty() || !pMapTiles->isReady() ){
		return;
	}

	//update map tiles
	FVector2D player2dPosInMap = pMapTiles->Map2DPosByWorld3DPos( player->Location  );
	FVector centerPos3D = pMapTiles->World3DPosByMap2DPos( MapUiCenterPos );
	FVector2D myPos2D = pMapTiles->ScaledMap2DOffsetFromWorld3DPos( player->Location, centerPos3D );
	MyIcon->SetPosition( myPos2D.X, myPos2D.Y);

	float fYawDeg = Cast<AMMOPlayerCamera>(ctrl->PlayerCamera)->curVT.POV.Rotation.Yaw * C_UnrRotToDeg;
	if(MyIcon){
		//MyIcon->SetFloat( "rotation", player->Rotation.Yaw * C_UnrRotToDeg - fYawDeg );
		MyIcon->SetFloat( "rotation",  player->Rotation.Yaw * C_UnrRotToDeg);
	}

	if( CameraYawDeg != fYawDeg ){
		CameraYawDeg = fYawDeg;
		if( MapPaper ){
			MapPaper->SetFloat( "rotation", -CameraYawDeg );
		}
		if( MapNPCView ){
			MapNPCView->SetFloat( "rotation", -CameraYawDeg );
		}
		if( MapPathView ){
			MapPathView->SetFloat( "rotation", -CameraYawDeg );
		}
	}

	//update NPCs and other static actors
	UpdateMapStaticMarks( centerPos3D, fYawDeg );

	//update teamate and so on
	UpdateMarksGlobalPlayer( centerPos3D, fYawDeg );

	//update monsters and player
	UpdateMarksNearPlayer( centerPos3D, fYawDeg );

	//update player marks
	//UpdatePlayerMarksInMap( centerPos3D, fYawDeg );
}

//===================================================================================================================================
void UMMOUIPathMap::UpdateMapStaticMarks( FVector centerPos3D, float fYawDeg )
{
	MMOMapImgTiles *pPathMapTiles = MMOMapImgTiles::GetMapImgTiles(E_PATHMAP_TILES);
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();

	INT cnt = staticMarks.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pStaticMark = &staticMarks(i);
		//if( pStaticMark == NULL ){
		//	continue;
		//}

		if( pStaticMark->_NeedUpdateFlag & EACTUIUF_PathMap ){
			//reset update flag
			pStaticMark->_NeedUpdateFlag &= ~EACTUIUF_PathMap;

			UGFxObject *pUiObj = pStaticMark->_UiObjInPathMap;
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
				pUiObj = pStaticMark->_UiObjInPathMap;
			}

			//descide show
			UBOOL canShow;
			canShow = ( pStaticMark->_Type != EACTMUI_InVisible ) ? TRUE : FALSE;
			pUiObj->SetVisible( canShow );
			eventInitMapMark( pUiObj, pStaticMark->_ID, pStaticMark->_Type, pStaticMark->_UiIconIdx );

		}//end of if need update

		//if valid,update position
		if( pStaticMark->_IsValid == TRUE && pStaticMark->_Type != EACTMUI_InVisible && pStaticMark->_UiObjInPathMap != NULL ){
			FVector2D inMiniMapPos2D = pPathMapTiles->ScaledMap2DOffsetFromWorld3DPos( pStaticMark->_OrigInMapPos, centerPos3D );
			pStaticMark->_UiObjInPathMap->SetPosition( inMiniMapPos2D.X, inMiniMapPos2D.Y );
			pStaticMark->_UiObjInPathMap->SetFloat( TEXT( "rotation" ), fYawDeg );
		}
	}//end of for loop of all static actors
}

//
void UMMOUIPathMap::UpdateMarksNearPlayer( FVector centerPos3D, float fYawDeg )
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	const FInMapRegion *curRegion = pMapDataMgr->GetCurReginInCurMap();
	const FBox *curRegionBox = NULL;
	if( curRegion != NULL ){
		curRegionBox = &curRegion->BoundingBox;
	}

	MMOFacade &gameFacade = MMOFacade::getSingleton();

	MMOMapImgTiles *pPathMapTiles = MMOMapImgTiles::GetMapImgTiles(E_PATHMAP_TILES);
	FActorInMapUI *pDynmActor;
	MMOMapDataManager::MapDynamicActorIterator iter = pMapDataMgr->BeginIterateDymcActorNear();
	//UINT key;
	INT maxPerFrameUpdateCnt = 6;
	for(  ; iter;  )
	{
		pDynmActor = &iter.Value();
		//UINT key = iter.Key();

		if( pDynmActor->_NeedUpdateFlag & EACTUIUF_PathMap  && maxPerFrameUpdateCnt-- > 0 ){
			//reset update flag
			pDynmActor->_NeedUpdateFlag  &= ~EACTUIUF_PathMap;

			UGFxObject *pUiObj = pDynmActor->_UiObjInPathMap;
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
				pUiObj = pDynmActor->_UiObjInPathMap;
			}

			//show it again
			pUiObj->SetVisible( TRUE );
			eventInitMapMark( pUiObj, pDynmActor->_ID, pDynmActor->_Type, pDynmActor->_UiIconIdx );
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
				if( pDynmActor->_UiObjInPathMap ){
					pDynmActor->_UiObjInPathMap->SetVisible( FALSE );
				}
				
				++iter;
				continue;
			}
			if( pDynmActor->_UiObjInPathMap ){
				//if( IsMultilayerRegion && curRegionBox != NULL ){
				//	UBOOL isMarkVisible = curRegionBox->IsInside( nearActor->Location );
				//	pDynmActor->_UiObjInPathMap->SetVisible( isMarkVisible );
				//}
				FVector2D inMiniMapPos2D = pPathMapTiles->ScaledMap2DOffsetFromWorld3DPos( nearActor->Location, centerPos3D );
				pDynmActor->_UiObjInPathMap->SetPosition( inMiniMapPos2D.X, inMiniMapPos2D.Y );
				pDynmActor->_UiObjInPathMap->SetFloat( TEXT( "rotation" ), fYawDeg );
			}
		}

		++iter;
	}//end of for loop of all nearby actors

}

//
void UMMOUIPathMap::UpdateMarksGlobalPlayer( FVector centerPos3D, float fYawDeg )
{
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

	MMOMapImgTiles *pPathMapTiles = MMOMapImgTiles::GetMapImgTiles(E_PATHMAP_TILES);
	FActorInMapUI *pDynmActor;
	MMOMapDataManager::MapDynamicActorIterator iter = pMapDataMgr->BeginIterateDymcActorGlobal();
	//UINT key;
	for(  ; iter;  )
	{
		pDynmActor = &iter.Value();
		//key = iter.Key();

		if( pDynmActor->_NeedUpdateFlag & EACTUIUF_PathMap ){
			//reset update flag
			pDynmActor->_NeedUpdateFlag &= ~EACTUIUF_PathMap;

			UGFxObject *pUiObj = pDynmActor->_UiObjInPathMap;
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
				pUiObj = pDynmActor->_UiObjInPathMap;
			}

			//whether in this map
			UBOOL canShow = ( pDynmActor->_InMapID == curMapID );
			pUiObj->SetVisible( canShow );
			eventInitMapMark( pUiObj, pDynmActor->_ID, pDynmActor->_Type, pDynmActor->_UiIconIdx );
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
					if( pDynmActor->_UiObjInPathMap ){
						UBOOL canShow = ( pTeammate->MapID == curMapID );
						if( curMap.ServerMapType == E_est_Line ){
							canShow = canShow && ( pTeammate->MapLineID == curMapLineID );
						}
						if( canShow == FALSE ){
							pDynmActor->_UiObjInPathMap->SetVisible( FALSE );
						}
						else{
							UBOOL isMarkInRegion = ( curRegionBox != NULL && curRegionBox->IsInside( teammateWorldPos ) );
							pDynmActor->_UiObjInPathMap->SetVisible( isMarkInRegion );
						}
					}
					//
					inMiniMapPos2D = pPathMapTiles->ScaledMap2DOffsetFromWorld3DPos( teammateWorldPos, centerPos3D );
				}
				break;
			case EACTMUI_BattleFlag:
				{
					AMMOActor* nearActor = NULL;
					if( pDynmActor->_AidID < (UINT)(-1) ){
						nearActor = gameFacade.GetCharacter( pDynmActor->_AidID );
					}

					if( nearActor != NULL ){
						inMiniMapPos2D = pPathMapTiles->ScaledMap2DOffsetFromWorld3DPos( nearActor->Location, centerPos3D );
					}
					else{
						inMiniMapPos2D = pPathMapTiles->ScaledMap2DOffsetFromWorld3DPos( pDynmActor->_OrigInMapPos, centerPos3D );
					}
				}
				break;
			default:
				break;
			}

			//
			if( pDynmActor->_UiObjInPathMap ){
				pDynmActor->_UiObjInPathMap->SetPosition( inMiniMapPos2D.X, inMiniMapPos2D.Y );
				pDynmActor->_UiObjInPathMap->SetFloat( TEXT( "rotation" ), fYawDeg );
			}
		}

		++iter;
	}//end of for loop of all nearby actors

}

//
void UMMOUIPathMap::UpdatePlayerMarksInMap( FVector centerPos3D, float fYawDeg )
{
}

//===================================================================================================================================
void UMMOUIPathMap::Init()
{
	Super::Init();
	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}

	InitMc();

	check(NULL == EventAdapter);
	if (NULL == EventAdapter)
	{
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_UpdateCurUIMap);

		EventAdapter = new TGameEventAdapter<UMMOUIPathMap>(this, evts);
	}
}

//
void UMMOUIPathMap::InitMc()
{
	//RootMc = GFXPlayer->GetVariableObject( TEXT("root") );

	UGFxObject *mapZoneMc = GetObject( TEXT("mapZone_mc") );
	if( mapZoneMc != NULL ){
		MapNPCView = mapZoneMc->GetObject( TEXT("MapNpcView_mc") );

		//
		MapPathView = mapZoneMc->GetObject( TEXT("MapPathView_mc") );
		if( MapPathView != NULL ){
			MyIcon = MapPathView->GetObject( TEXT("PlayerArrow_mc") );
		}

		//
		MapPaper = mapZoneMc->GetObject( TEXT("mapPaper_mc") );
		if( MapPaper != NULL ){
			MapTiles.AddZeroed( 4 );
			for( INT i = 0; i < 4; ++i ){
				FString obName = FString::Printf( TEXT("MapPiece_%d_mc" ), i + 1 );
				MapTiles(i) = MapPaper->GetObject( obName );
			}

			//
			PaperZoneSizeHalf.X = MapPaper->GetInt( TEXT("width") ) / 2;
			PaperZoneSizeHalf.Y = MapPaper->GetInt( TEXT("height") ) / 2;
		}
	}

	MapZoneMaskMc = GetObject( TEXT("mapMaskZone_mc") );
	if( MapZoneMaskMc != NULL ){
		MapVisibleZoneSize.X = MapZoneMaskMc->GetInt( TEXT("width") );
		MapVisibleZoneSize.Y = MapZoneMaskMc->GetInt( TEXT("height") );
	}
}

//==================================================================
UBOOL UMMOUIPathMap::CreateMapMarkUIObject( FObjInMapUI *pMapMarkData )
{
	if( pMapMarkData == NULL ){
		return FALSE;
	}

	//
	if( pMapMarkData->_UiObjInPathMap ){
		return TRUE;
	}

	//
	FString uiObjName = FString::Printf( TEXT("PMapMarkIconType%d_%d"), pMapMarkData->_Type, pMapMarkData->_ID );
	UGFxObject *pMarkUiParent = eventCallASGetMapMarkUIObjectContainer( pMapMarkData->_Type );
	if( pMarkUiParent == NULL ){
		return FALSE;
	}

	pMapMarkData->_UiObjInPathMap = pMarkUiParent->AttachMovie( TEXT("MapMarkIconAOG"), uiObjName );
	if( pMapMarkData->_UiObjInPathMap == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [Map AttachMovie faile] MapMarkIcon not exit") );
		return FALSE;
	}
	pMapMarkData->_UiObjInPathMap->AddToRoot();

	//
	//pMapMarkData->_UiObjInPathMap->SetInt( TEXT("Type"), pMapMarkData->_Type );
	//pMapMarkData->_UiObjInPathMap->SetInt( TEXT("ID"), pMapMarkData->_ID );
	eventInitMapMark( pMapMarkData->_UiObjInPathMap, pMapMarkData->_ID, pMapMarkData->_Type, pMapMarkData->_UiIconIdx );
	//eventAddMapMarkEventListerner( pMapMarkData->_UiObjInPathMap );

	return TRUE;
}

//
//UGFxObject* UMMOUIPathMap::eventCallASGetMapMarkUIObjectContainer( INT type )
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
void UMMOUIPathMap::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}

	ClearPathMapUI();
}

//
void UMMOUIPathMap::DistoryRootMc( UGFxObject* &pMc )
{
	if( pMc == NULL ){
		return;
	}
	eventRemoveChild( pMc );
	pMc->RemoveFromRoot();
	pMc = NULL;
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIPathMap::BeforeViewUpdate(FLOAT DeltaTime)
{
	//Super::BeforeViewUpdate(DeltaTime);
	//world map
	if( IsVisible ){
		UpdatePathMap();
	}
}
//===============================================================================================================
UBOOL UMMOUIPathMap::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if( strHoverControllerName == TEXT("BackToMinimapBtn") )
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}
//========================================================================================================
void UMMOUIPathMap::ASShowPathMapMarkHint( INT type, INT idx )
{
	warnf( TEXT( "ASShowPathMapMarkHint at Type:%d Idx:%d"), type, idx );

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	//FString displayText;
	FObjInMapUI *pMapMark = NULL;
	switch( type )
	{
	case EACTMUI_StaticNPC:
	case EACTMUI_TeleportFB:
	case EACTMUI_TeleportMap:
	case EACTMUI_TeleportInDoor:
	case EACTMUI_TeleportNode:
		{
			TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();
			if( staticMarks.IsValidIndex( idx ) ){
				pMapMark = &staticMarks( idx );
			}
		}

		break;

	case EACTMUI_NearMonster:
	case EACTMUI_NearBoss:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorNearByUiID( idx );
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
			}
		}
		break;

	case EACTMUI_GlobalTeammate:
	case EACTMUI_BattleFlag:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorGlobalByUiID( idx );
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
	UGFxObject *pMc = pMapMark->_UiObjInPathMap;
	if( pMc == NULL ){
		return;
	}

	//
	pMc->SetString( TEXT("tipsHtml"), pMapMark->_DisplayName );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, pMc );
}

void UMMOUIPathMap::ASHidePathMapMarkHint()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//==============================================================================================
void UMMOUIPathMap::ASPathMapToMinimap()
{
	UMMOUIMinimap *minimapPanel = (UMMOUIMinimap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	if( minimapPanel != NULL ){
		minimapPanel->SwitchMinimapMode();
	}

	//
	Show( FALSE );
}
//===============================================================================================================
//===============================================================================================================