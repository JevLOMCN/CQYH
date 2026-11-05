#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapImgTiles.h"
#include "MMOMapDataManager.h"
#include "MMOQuestManager.h"
#include "TeamManager.h"
#include "MMOStats.h"
#include "MMOGuildData.h"
#include "MMOChatManager.h"
#include "MMONavigationManager.h"
#include "MMOGuildManager.h"

IMPLEMENT_CLASS(UMMOUIWorldMap)


void UMMOUIWorldMap::ChangeScene()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	if( Player != NULL ){
		pMapDataMgr->GetCurPlayerRegionIn( Player->Location );
	}
	//
	//InitWorldMap( pMapDataMgr->GetCurMap().ServerID, pMapDataMgr->GetCurReginIdxInMap() );
}

//
void UMMOUIWorldMap::ShowCurSceneMapConent()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	INT curMapRegionIdx =  pMapDataMgr->GetCurReginIdxInMap();
	INT curMapId = -1;
	FInMapRegion *curRegion = pMapDataMgr->GetCurReginInCurMap();
	if( curRegion != NULL ){
		curMapId = curRegion->MapID;
		//in some map hide map list
		//if( _MapListMc != NULL ){
		//	_MapListMc->SetVisible( !curRegion->IsReuseMapResouce );
		//}
	}

	RecoverWorldMapUI();
	//
	//if( ( pMapDataMgr->GetCurWorldMap().ServerID != curMapId ) || ( curMapRegionIdx != InRegionIdx ) ){
	InitWorldMap( curMapId, curMapRegionIdx );
	//}
	//else{
	//	//update position
	//	MapUiCenterPos = pWorldMapTiles->Map2DPosByWorld3DPos( player->Location );// * pWorldMapTiles->_ZoomFactor;
	//	//initial map npc related to me
	//	pMapDataMgr->InitWorldMapPuttedUsefulNpcs( InRegionIdx );
	//	//initial map FB points in map
	//	//pMapDataMgr->InitWorldMapPuttedFBs( InRegionIdx );
	//	//
	//	UpdateMapTaskList();
	//	//
	//	UpdateMapStaticMarkList();
	//}

	//回世界地图按钮
	eventCallASShowBackUnitedMapBtn( FALSE );

	//Tab键
	ShowMapPanelContent( ePnlTyp_WorldMap );
}

void UMMOUIWorldMap::UpdateGuildOccupyInfo(INT mapId)
{
	UBOOL isVisible = FALSE;
	FString occupyName = TEXT("");
	if (mapId == GUILD_OCCUPY_MAP_ID_1 ||
		mapId == GUILD_OCCUPY_MAP_ID_2 ||
		mapId == GUILD_OCCUPY_MAP_ID_3 ||
		mapId == GUILD_OCCUPY_MAP_ID_4)
	{
		if (MMOGuildManager::getSingleton().GetOccupyState(mapId) > 0)
		{
			isVisible = TRUE;
			MMOGuildManager::getSingleton().GetOccupyGuildName(mapId, occupyName);
		}
	}
	eventCallShowGuildOccupyName(isVisible, occupyName);
}

//
void UMMOUIWorldMap::InitWorldMap( INT mapID, INT regionDataIdx )
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	//
	if( ctrl != NULL ){
		//CameraYawDeg = Cast<AMMOPlayerCamera>(ctrl->PlayerCamera)->curVT.POV.Rotation.Yaw * C_UnrRotToDeg;
		//if( MapPaper ){
		//	MapPaper->SetFloat( "rotation", -CameraYawDeg );
		//}
		if( MapNPCView ){
			MapNPCView->SetFloat( "rotation", -CameraYawDeg );
		}
		if( MapPathView ){
			MapPathView->SetFloat( "rotation", -CameraYawDeg );
		}
		if( MapExtraInfoView ){
			MapExtraInfoView->SetFloat( "rotation", -CameraYawDeg );
		}
	}
	
	//标签页
	if( mapID == pMapDataMgr->GetCurMap().ServerID ){
		eventCallASChangeWorldMapTabBtn( ePnlTyp_WorldMap );
	}
	else{
		eventCallASChangeWorldMapTabBtn( ePnlTyp_UnitedMap );
	}

	//设置为当前所在场景地图为大地图所显示的地图
	pMapDataMgr->SetCurWorldMap( mapID, regionDataIdx );
	IsMultilayerRegion = pMapDataMgr->IsMapRegionMultilayer( mapID );

	FExportMap mapExportInfo = pMapDataMgr->GetCurWorldMap();
	CurMapTexName = mapExportInfo.MapName;
	//
	FInMapRegion *pCurReginInMap = pMapDataMgr->GetCurWorldMapRegion();
	if( pCurReginInMap == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUIWorldMap::InitWorldMap] NoMapRegion in Map:[%d]"),mapID );
		//return;
	}

	if( pCurReginInMap != NULL ){
		MapNameTitle_tf->SetText( pCurReginInMap->DisplayName );

		//UBOOL needReload = IfNeedReloadMap( pMapDataMgr->GetCurReginIdxInMap() );
		//if( needReload && pCurReginInMap != NULL ){
		UBOOL isResetWorldMapOrig = ( (pMapDataMgr->GetCurMap().ServerID == mapExportInfo.ServerID) || (InRegionIdx == regionDataIdx) );
		InRegionIdx = regionDataIdx;

		InitWorldMapTiles( CurMapTexName, *pCurReginInMap, isResetWorldMapOrig );
		////
		//ResizeMap();
		//UpdateMapTilesPosition();
	}

	//map static objects
	InitMapStaticMarks();
	//map user mark
	pMapDataMgr->InitialMapUserMarks( mapID );

	//near player objects
	FExportMap &curPlayerMap = pMapDataMgr->GetCurMap();
	UBOOL canShowNearMarks;
	canShowNearMarks = ( ( curPlayerMap.ServerID == mapID ) && ( regionDataIdx == InRegionIdx ) );
	ToggleShowNearPlayerMarksAndMyself( canShowNearMarks );

	//
	//UpdateMapList();

	//
	//UpdateMapTaskList();

	//
	UpdateMapStaticMarkList();

	//路径只显示在当前地图（大地图）
	UGFxObject *pPathPointContainerMc = eventCallASGetMapMarkUIObjectContainer( EACTMUI_PathPoint );
	if( pPathPointContainerMc != NULL ){
		UBOOL isShowInWorldMap = ( _CurPanelType == ePnlTyp_WorldMap );
		pPathPointContainerMc->SetVisible( isShowInWorldMap );
	}

	//显示公会据点战文字
	UpdateGuildOccupyInfo(mapID);
}
/*
void UMMOUIWorldMap::InitWorldMapTiles( const FString& mapName, FInMapRegion RegionInMap, UBOOL isResetPos )
{
	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	if( pWorldMapTiles == NULL ){
		return;
	}
	//prepare the tile picture loader
	bool bLoadOK = pWorldMapTiles->LoadUiTile( MapTiles, TEXT("map_alpha%d -nopack") );
	if( !bLoadOK ){
		return;
	}

	//initialize world map tiles data
	FString texPackName;
	texPackName = mapName + TEXT("_") + RegionInMap.VolumeName;

	FString strTexPath;
	strTexPath = FString::Printf( TEXT("%s_wMap.%s_wMap"), *texPackName, *texPackName );

	FString strUILoaderBgTexPath = TEXT("MMO_UIWorldMap.");
	strUILoaderBgTexPath = FString::Printf( TEXT("MMO_UIWorldMapImg.%s_wMapBg"), *texPackName );
	//
	ZoomLevel = 0;

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

	//scale3dTo2d为3d坐标到2d坐标的转换比例，此局域地图坐标Y轴方向与世界3d坐标Y轴相反
	//INT oneCoordToPixels = RegionInMap.WorldMapPixelSize;
	//if(oneCoordToPixels == 0){
	//	//错的
	//	oneCoordToPixels = 54;
	//}
	//INT realCoordStepToPixels = (INT)( MAP_SNAPSHOT_COORDINATE_STEP / oneCoordToPixels );
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

	UINT trueWorldMapWidth = origImgSizeX * RegionInMap.WorldMapScale;
	UINT trueWorldMapHeight = origImgSizeX * RegionInMap.WorldMapScale;

	//offset
	FBox regionBox = RegionInMap.BoundingBox;
	FVector2D beforeUIScale2dOffsetInMap;
	beforeUIScale2dOffsetInMap.X = -regionBox.Min.Y * map3Dto2D.X;
	beforeUIScale2dOffsetInMap.Y = -regionBox.Min.X * map3Dto2D.Y + trueWorldMapHeight;

	//
	FVector2D afterTurned2dOffsetInMap;
	afterTurned2dOffsetInMap.X = RegionInMap.WorldMapOffsetX;
	afterTurned2dOffsetInMap.Y = RegionInMap.WorldMapOffsetY - trueWorldMapHeight * COS_MAP;

	//
	if( trueWorldMapWidth > 0 ){
		MapUIZoomDefault = (FLOAT)BorderHeight / (FLOAT)( trueWorldMapWidth * 1.4142135623730950488016887242097 );
	}

	//
	FVector player3dPos;
	//MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	if( !isResetPos ){
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if( player != NULL ){
			player3dPos = player->Location;
		}
	}
	else{
		MapUiCenterPos.X = PaperZoneSize.X / 2;//PaperZoneSize.X / 2 * MapUIZoomDefault;
		MapUiCenterPos.Y = PaperZoneSize.Y / 2;//PaperZoneSize.Y / 2 * MapUIZoomDefault;

		FVector2D beforeTurnPos2D;
		beforeTurnPos2D.X = MapUiCenterPos.X - afterTurned2dOffsetInMap.X;
		beforeTurnPos2D.Y = MapUiCenterPos.Y - afterTurned2dOffsetInMap.Y;
		beforeTurnPos2D.X = -beforeTurnPos2D.Y * SIN_MAP + beforeTurnPos2D.X * COS_MAP;
		beforeTurnPos2D.Y =  beforeTurnPos2D.Y * COS_MAP + beforeTurnPos2D.X * SIN_MAP;

		player3dPos.Y = ( beforeTurnPos2D.X - beforeUIScale2dOffsetInMap.X ) / map3Dto2D.X;
		player3dPos.X = ( beforeTurnPos2D.Y - beforeUIScale2dOffsetInMap.Y ) / map3Dto2D.Y;
	}

	pWorldMapTiles->Initialize( GFXPlayer, strTexPath, player3dPos, tileSizeX, tileSizeY,
							 maxImgTileRowNum, maxImgTileColNum, map3Dto2D, beforeUIScale2dOffsetInMap, afterTurned2dOffsetInMap, MapUIZoomDefault );

	//底板路径
	if( MapBGLoader != NULL ){
		UTexture2D* tex = Cast<UTexture2D>( UObject::StaticLoadObject( UTexture2D::StaticClass(), NULL, *strUILoaderBgTexPath, NULL, LOAD_NoWarn, NULL) );
		if( tex == NULL ){
			//没图，用默认的
			tex = Cast<UTexture2D>( UObject::StaticLoadObject( UTexture2D::StaticClass(), NULL, TEXT("MMO_UIWorldMapImg.wMapDefaultBg"), NULL, LOAD_NoWarn, NULL) );
		}

		if( GFXPlayer != NULL && tex != NULL){
			GFXPlayer->SetExternalTexture( TEXT("mapBg__nopack"), tex );
		}
	}
}
*/

void UMMOUIWorldMap::InitWorldMapTiles( const FString& mapName, FInMapRegion RegionInMap, UBOOL isResetPos )
{
	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	if( pWorldMapTiles == NULL ){
		return;
	}
	//prepare the tile picture loader
	bool bLoadOK = pWorldMapTiles->LoadUiTile( MapTiles, TEXT("worldmap_alpha%d -nopack") );
	if( !bLoadOK ){
		return;
	}

	//initialize world map tiles data
	FString texPackName;
	texPackName = mapName + TEXT("_") + RegionInMap.VolumeName;

	if( !RegionInMap.ReuseMapVolumeName.IsEmpty() ){
		texPackName = mapName + TEXT("_") + RegionInMap.ReuseMapVolumeName;
	}

	FString strTexPath;
	strTexPath = FString::Printf( TEXT("%s_wMap.%s_wMap"), *texPackName, *texPackName );

	FString strUILoaderBgTexPath = TEXT("MMO_UIWorldMap.");
	strUILoaderBgTexPath = FString::Printf( TEXT("MMO_UIWorldMapImg.%s_wMapBg"), *texPackName );
	//
	FString strValueKey;
	INT tileSizeX;
	strValueKey = TEXT("WorldMapTileSizeX");
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeX, GGameIni );

	strValueKey = TEXT("WorldMapTileSizeY");
	INT tileSizeY;
	GConfig->GetInt( TEXT("MapImg"), *strValueKey, tileSizeY, GGameIni );

	//
	INT maxImgTileRowNum = 4;//RegionInMap.WorldMapTileRowNum;
	INT maxImgTileColNum = 4;//RegionInMap.WorldMapTileColNum;

	FVector2D map3Dto2DNoScaled;
	//map3Dto2DNoScaled.X = (FLOAT)realCoordStepToPixels / MAP_SNAPSHOT_COORDINATE_STEP;
	map3Dto2DNoScaled.X = (FLOAT)1 / RegionInMap.WorldMapPixelSize;
	map3Dto2DNoScaled.Y = -map3Dto2DNoScaled.X;

	//scale
	FVector2D map3Dto2D;
	map3Dto2D = map3Dto2DNoScaled;

	//地图的数据都是按512大小的输出图来计算的，这里把他当做1024的图用
	FLOAT worldMapResScale = 0.5;
	map3Dto2D /= worldMapResScale;//MapUIZoomDefault;
	//
	INT minimapY;
	GConfig->GetInt( TEXT("MapImg"), TEXT("MiniMapTileSizeY"), minimapY, GGameIni );
	INT origImgSizeY = RegionInMap.MiniMapTileColNum * minimapY;

	UINT trueWorldMapHeight = origImgSizeY / worldMapResScale;//origImgSizeY * RegionInMap.WorldMapScale;

	//offset
	FBox regionBox = RegionInMap.BoundingBox;
	FVector2D beforeUIScale2dOffsetInMap;
	beforeUIScale2dOffsetInMap.X = -regionBox.Min.Y * map3Dto2D.X;
	beforeUIScale2dOffsetInMap.Y = -regionBox.Min.X * map3Dto2D.Y + trueWorldMapHeight;

	//
	FVector player3dPos;
	//MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	if( !isResetPos ){
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if( player != NULL ){
			player3dPos = player->Location;
		}
	}
	else{
		//UI中地图Img为1304 X 1080
		MapUiCenterPos.X = PaperZoneSize.X / 2 / MapUIResScale;
		MapUiCenterPos.Y = PaperZoneSize.Y / 2 / MapUIResScale;
		//MapUiCenterPos.X = PaperZoneSize.X / 2;
		//MapUiCenterPos.Y = PaperZoneSize.Y / 2;

		FVector2D uiCenterPosInRawMap;
		uiCenterPosInRawMap.X = MapUiCenterPos.X - MapImgOffset.X;
		uiCenterPosInRawMap.Y = MapUiCenterPos.Y - MapImgOffset.Y;

		player3dPos.Y = ( uiCenterPosInRawMap.X - beforeUIScale2dOffsetInMap.X ) / map3Dto2D.X;
		player3dPos.X = ( uiCenterPosInRawMap.Y - beforeUIScale2dOffsetInMap.Y ) / map3Dto2D.Y;
	}

	//
	ZoomLevel = 3;
	//MapUIZoom = MapUIZoomDefault;
	//ResizeMap();
	ZoomTo( ZoomLevel );

	FLOAT totalUIScale = MapUIZoom * MapUIResScale;
	pWorldMapTiles->Initialize( GFXPlayer, strTexPath, player3dPos, tileSizeX, tileSizeY, maxImgTileRowNum, maxImgTileColNum, map3Dto2D, beforeUIScale2dOffsetInMap, MapImgOffset, totalUIScale );

	//底板路径
	if( MapBGLoader != NULL ){
		UTexture2D* tex = Cast<UTexture2D>( UObject::StaticLoadObject( UTexture2D::StaticClass(), NULL, *strUILoaderBgTexPath, NULL, LOAD_NoWarn, NULL) );
		if( tex == NULL ){
			//是否沿用其他的图
			tex = Cast<UTexture2D>( UObject::StaticLoadObject( UTexture2D::StaticClass(), NULL, *strUILoaderBgTexPath, NULL, LOAD_NoWarn, NULL) );
		}

		if( GFXPlayer != NULL && tex != NULL){
			GFXPlayer->SetExternalTexture( TEXT("worldmapBg__nopack"), tex );
		}

		//Pos
		MapBGLoaderPos.X = 0;
		MapBGLoaderPos.Y = 0;
		UpdateMapTilesPosition();
	}
}

void UMMOUIWorldMap::ClearWorldMapUI()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	//hide tooltips
	if( IsTipsShowed ){
		MMOUI::GetUIManager()->delegateHideTips();
	}

	//clear dynamic map ui objects
	FActorInMapUI *pUIActor = NULL;
	//near
	MMOMapDataManager::MapDynamicActorIterator iterNear = pMapDataMgr->BeginIterateDymcActorNear();
	for(  ; iterNear; ++iterNear ){
		pUIActor = &iterNear.Value();

		//
		if( pUIActor->_UiObjInWorldMap != NULL ){
			eventRemoveChild( pUIActor->_UiObjInWorldMap );
			pUIActor->_UiObjInWorldMap->RemoveFromRoot();
			pUIActor->_UiObjInWorldMap = NULL;
		}
	}
	//Global
	MMOMapDataManager::MapDynamicActorIterator iterGlobal = pMapDataMgr->BeginIterateDymcActorGlobal();
	for(  ; iterGlobal; ++iterGlobal ){
		pUIActor = &iterGlobal.Value();

		//
		if( pUIActor->_UiObjInWorldMap != NULL ){
			eventRemoveChild( pUIActor->_UiObjInWorldMap );
			pUIActor->_UiObjInWorldMap->RemoveFromRoot();
			pUIActor->_UiObjInWorldMap = NULL;
		}
	}

	//clear static map ui objects
	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurWorldMapStaticActorPointsList();
	for( INT i = staticMarks.Num() - 1; i >= 0; --i ){
		pUIActor = &staticMarks( i );

		if( pUIActor->_UiObjInWorldMap == NULL ){
			continue;
		}

		eventRemoveChild( pUIActor->_UiObjInWorldMap );
		pUIActor->_UiObjInWorldMap->RemoveFromRoot();
		pUIActor->_UiObjInWorldMap = NULL;
	}

	//user marks
	FUserMarkInMapUI *pUIUserMark = NULL;
	TArray<FUserMarkInMapUI> &marksList = pMapDataMgr->GetCurWorldMapMarks();
	for( INT i = marksList.Num() - 1; i >= 0; --i ){
		pUIUserMark = &marksList( i );

		if( pUIUserMark->_UiObjInWorldMap == NULL ){
			continue;
		}

		eventRemoveChild( pUIUserMark->_UiObjInWorldMap );
		pUIUserMark->_UiObjInWorldMap->RemoveFromRoot();
		pUIUserMark->_UiObjInWorldMap = NULL;
	}
}

//
void UMMOUIWorldMap::RecoverWorldMapUI()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	//clear dynamic map ui objects
	FActorInMapUI *pUIActor = NULL;
	//near
	MMOMapDataManager::MapDynamicActorIterator iterNear = pMapDataMgr->BeginIterateDymcActorNear();
	for(  ; iterNear; ++iterNear ){
		pUIActor = &iterNear.Value();

		//
		if( !pUIActor->_IsValid ){
			continue;
		}
		pUIActor->_NeedUpdateFlag |= EACTUIUF_WorldMap;
	}
	//Global
	MMOMapDataManager::MapDynamicActorIterator iterGlobal = pMapDataMgr->BeginIterateDymcActorGlobal();
	for(  ; iterGlobal; ++iterGlobal ){
		pUIActor = &iterGlobal.Value();

		if( !pUIActor->_IsValid ){
			continue;
		}
		pUIActor->_NeedUpdateFlag |= EACTUIUF_WorldMap;
	}

	//clear static map ui objects
	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurWorldMapStaticActorPointsList();
	for( INT i = staticMarks.Num() - 1; i >= 0; --i ){
		pUIActor = &staticMarks( i );

		if( !pUIActor->_IsValid ){
			continue;
		}
		pUIActor->_NeedUpdateFlag |= EACTUIUF_WorldMap;
	}

	//user marks
	FUserMarkInMapUI *pUIUserMark = NULL;
	TArray<FUserMarkInMapUI> &marksList = pMapDataMgr->GetCurWorldMapMarks();
	for( INT i = marksList.Num() - 1; i >= 0; --i ){
		pUIUserMark = &marksList( i );

		if( !pUIUserMark->_IsValid ){
			continue;
		}
		pUIUserMark->_NeedUpdateFlag |= EACTUIUF_WorldMap;
	}

	//path update
	//if( Player != NULL && Player->isMoving ){
	//	MMOGameEvent_Any evt(GameEvent_UpdateMovePath);
	//	MMOUI::PostGameEvent(&evt);
	//}
}

//=====================================================================================================
void UMMOUIWorldMap::PostOnShow( UBOOL v )
{
	Super::PostOnShow( v );
	//
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	if( v ){
		ShowCurSceneMapConent();

		MMOFacade::getSingleton().PlayUISound(TEXT("menu_map_on"));
	}
	else{
		switch( _CurPanelContentType )
		{
		case ePnlTyp_WorldMap:
			{
				WorldMapOnShow( FALSE );
			}
			break;
		case ePnlTyp_UnitedMap:
			{
				if( _UnitedMapPanel == NULL ){
					break;
				}
				_UnitedMapPanel->UnitedMapOnShow( FALSE );
			}
			break;
		}

		//
		eventResetWorldMapUI();

		//
		TMap< INT, INT > &needUpdateMap = pMapDataMgr->GetNeedUpdateMap();
		for( TMap< INT, INT >::TConstIterator it(needUpdateMap); it; ++it ){
			pMapDataMgr->ExportMapUserMarks( it.Key() );
		}

		//clear datas
		needUpdateMap.Empty();

		//
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_map_off"));
	}
}

void UMMOUIWorldMap::WorldMapOnShow( UBOOL isShow )
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	if( isShow ){
		//LastMoveModify.X = 0;
		//LastMoveModify.Y = 0;

		FVector lastInputPos = MMOMapDataManager::getSingleton().GetLastInputMoveToPos();
		eventUCSetXY( lastInputPos.X / 100, lastInputPos.Y / 100 );
	}
	else{
		//隐藏地图Icon信息显示
		ShowMapMarksInfomation( FALSE );
	}
}

//----------------------------------------------------------------------
void UMMOUIWorldMap::ShowMapPanelContent( EPanelTypeWorldMap worldMapType )
{
	if( worldMapType != _CurPanelContentType ){
		switch( _CurPanelContentType )
		{
		case ePnlTyp_WorldMap:
			{
				WorldMapOnShow( FALSE );
			}
			break;
		case ePnlTyp_UnitedMap:
			{
				if( _UnitedMapPanel == NULL ){
					break;
				}
				_UnitedMapPanel->UnitedMapOnShow( FALSE );
			}
			break;
		}
	}

	//
	eventCallASChangeWorldMapTabBtn( worldMapType );

	//
	eventCallASChangeWorldMapContent( worldMapType );

	//
	switch( _CurPanelContentType )
	{
	case ePnlTyp_WorldMap:
		{
			WorldMapOnShow( TRUE );

			//if( Player != NULL && Player->isMoving ){
			//	MMOGameEvent_Any evt(GameEvent_UpdateMovePath);
			//	MMOUI::PostGameEvent(&evt);
			//}
		}
		break;
	case ePnlTyp_UnitedMap:
		{
			if( _UnitedMapPanel == NULL ){
				break;
			}
			_UnitedMapPanel->UnitedMapOnShow( TRUE );
		}
		break;
	}
}

//------------------------------------------------------------------------
void UMMOUIWorldMap::ShowWorldMapInUnitedMapContent( INT mapID, INT regionInMapIdx )
{
	InitWorldMap( mapID, regionInMapIdx );

	//
	if( ePnlTyp_UnitedMap == _CurPanelType && _UnitedMapPanel != NULL ){
		_UnitedMapPanel->UnitedMapOnShow( FALSE );
	}

	//
	if( _CurPanelContentType != ePnlTyp_WorldMap ){
		WorldMapOnShow( TRUE );

		//
		eventCallASChangeWorldMapContent( ePnlTyp_WorldMap );

		//if( Player != NULL && Player->isMoving ){
		//	MMOGameEvent_Any evt(GameEvent_UpdateMovePath);
		//	MMOUI::PostGameEvent(&evt);
		//}
	}

	//
	eventCallASShowBackUnitedMapBtn( TRUE );
}
//------------------------------------------------------------------------
void UMMOUIWorldMap::UpdateWorldMapContents()
{
	SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap);
	if( !MyIcon)
		return;
	AMMOPlayer* player =	MMOFacade::getSingleton().GetMainPlayer();
	if( !player)
		return;
	//if( MapStone[0] == NULL || MapStone[1] == NULL || CurMapTexName.IsEmpty())
	//	return;
	if( CurMapTexName.IsEmpty() ){
		return;
	}
	//FVector2D Pos2D = ScaledMap2DPosByWorld3DPos( player->Location );
	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	//pWorldMapTiles->World3DPosByScaledMap2DPos( MapUiCenterPos );
	FVector centerPos3D = pWorldMapTiles->World3DPosByMap2DPos( MapUiCenterPos );
	//FVector2D pos2D = pWorldMapTiles->Map2DOffsetFromWorld3DPos( player->Location, centerPos3D );
	FVector2D myPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( player->Location, centerPos3D );
	//FVector2D originOffset = pWorldMapTiles->GetOriginOffset();
	//pos2D += originOffset;
	MyIcon->SetPosition( myPos2D.X ,myPos2D.Y);
	//float fYawDeg = Cast<AMMOPlayerCamera>(ctrl->PlayerCamera)->curVT.POV.Rotation.Yaw * C_UnrRotToDeg;
	if(MyIcon){
		//MyIcon->SetFloat( "rotation", player->Rotation.Yaw * C_UnrRotToDeg - fYawDeg );
		MyIcon->SetFloat( "rotation",  player->Rotation.Yaw * C_UnrRotToDeg );
	}

	//if( CameraYawDeg != fYawDeg ){
	//	CameraYawDeg = fYawDeg;
	//	if( MapPaper ){
	//		MapPaper->SetFloat( "rotation", -CameraYawDeg );
	//	}
	//	if( MapNPCView ){
	//		MapNPCView->SetFloat( "rotation", -CameraYawDeg );
	//	}
	//	if( MapPathView ){
	//		MapPathView->SetFloat( "rotation", -CameraYawDeg );
	//	}
	//}
	//update player position text
	MyPostion_Tf->SetText( FString::Printf( TEXT("(%d,%d)"), int( player->Location.X/100), int( player->Location.Y/100)  ) );
	//FVector playerGridIdx = MMOMapDataManager::getSingleton().CvtSceneVecToGridIdx( player->Location );
	//MyPostion_Tf->SetText( FString::Printf( TEXT("(%d,%d)"), INT( playerGridIdx->Location.X ), INT( playerGridIdx->Location.Y ) ) );

	//update mouse position text
	FVector2D mouse2D, mouseMap2DPos;
	//mouse2D.X = GFXPlayer->GetVariableObject("root")->GetInt("mouseX");
	//mouse2D.Y = GFXPlayer->GetVariableObject("root")->GetInt("mouseY");
	//mouse2D.X = RootMc->GetInt("mouseX");
	//mouse2D.Y = RootMc->GetInt("mouseY");
	FIntPoint pt = MMOFacade::getSingleton().GetMouseCursorPosition();
	mouse2D.X = pt.X;
	mouse2D.Y = pt.Y;

	ThisPos.X = GetInt( TEXT("x") );
	ThisPos.Y = GetInt( TEXT("y") );

	FVector2D maskZonePosInScreen = MaskZonePos + ThisPos;
	if( mouse2D.X > maskZonePosInScreen.X &&  mouse2D.X < maskZonePosInScreen.X+ PaperMaskBorder.X 
		&& mouse2D.Y  > maskZonePosInScreen.Y &&  mouse2D.Y < maskZonePosInScreen.Y+ PaperMaskBorder.Y )
	{
		//FVector mouseVec = World3DPosByScaledMap2DPos(mouse2D-PaperZone);
		mouseMap2DPos = mouse2D - ThisPos - PaperZoneCenterPos;
		FVector mouseVec = ConvertPosInMapUIToWorld3DPos( mouseMap2DPos.X, mouseMap2DPos.Y );
		MousePostion_Tf->SetText( FString::Printf( TEXT("(%d,%d)"), int( mouseVec.X/100), int( mouseVec.Y/100)  ) );
		//mouseVec = MMOMapDataManager::getSingleton().CvtSceneVecToGridIdx( mouseVec );
		//MousePostion_Tf->SetText( FString::Printf( TEXT("(%d,%d)"), int( mouseVec.X ), int( mouseVec.Y )  ) );
	}
	else{
		MousePostion_Tf->SetText( TEXT("(---,---)") );
	}

	//update NPCs and other static actors
	UpdateMapStaticMarks( centerPos3D, CameraYawDeg );

	//update teamate and so on
	UpdateMarksGlobalPlayer( centerPos3D, CameraYawDeg );

	//update monsters and player
	UpdateMarksNearPlayer( centerPos3D, CameraYawDeg );

	//update player marks
	UpdatePlayerMarksInMap( centerPos3D, CameraYawDeg );

	//update path
	UpdateMapPath( centerPos3D, CameraYawDeg );

	//update icon infomation
	UpdateMapMarksInfomation( centerPos3D, CameraYawDeg );
}

//===================================================================================================================================
void UMMOUIWorldMap::UpdateMapStaticMarks( FVector centerPos3D, float fYawDeg )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap_UpdateMapStaticMarks);

	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurWorldMapStaticActorPointsList();

	INT cnt = staticMarks.Num();
	INT maxPerFrameUpdateCnt = 6;
	UGFxObject* dataProvider = NULL;
	INT loopAddedCnt = 0;
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pStaticMark = &staticMarks(i);
		//if( pStaticMark == NULL ){
		//	continue;
		//}

		//
		if( pStaticMark->_NeedUpdateFlag & EACTUIUF_WorldMap && maxPerFrameUpdateCnt-- > 0 ){
			if( dataProvider == NULL ){
				dataProvider = GFXPlayer->CreateArray();
			}
			//reset update flag
			pStaticMark->_NeedUpdateFlag &= ~EACTUIUF_WorldMap;

			UGFxObject *pUiObj = pStaticMark->_UiObjInWorldMap;
			//if is invalid now, hide unused this ui points
			if( pStaticMark->_IsValid == FALSE ){
				if( pUiObj != NULL ){
					pUiObj->SetVisible( FALSE );
				}
				continue;
			}

			if( pUiObj == NULL ){
				SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap_UpdateMapStaticMarks_CreateMapMarkUIObject);
				UBOOL createRst = CreateMapMarkUIObject( pStaticMark );
				if( createRst != TRUE ){
					continue;
				}
				pUiObj = pStaticMark->_UiObjInWorldMap;
			}

			//descide show
			bool isFuncNpc = ( ( pStaticMark->_ToggleFlag & EACTUIF_FunctionNpc ) > 0 );
			bool isTaskNpc = ( ( pStaticMark->_ToggleFlag & EACTUIF_TaskNpc ) > 0 );
			UBOOL canShow = TRUE;
			//
			if( isFuncNpc || isTaskNpc){
				canShow = FALSE;
				canShow |= CanFuncNpcShow;
				canShow |= CanTaskNpcShow;
			}

			pUiObj->SetVisible( canShow );
			//
			//pUiObj->SetInt( TEXT("Type"), pStaticMark->_Type );
			//pUiObj->SetInt( TEXT("ID"), pStaticMark->_ID );
			////show correct icon for its type
			//pUiObj->GotoAndStop( pStaticMark->_UiIconIdx );
			{
				SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap_UpdateMapStaticMarks_eventInitMapMark);
				//eventInitMapMark( pUiObj, pStaticMark->_ID, pStaticMark->_Type, pStaticMark->_UiIconIdx );
				//2
				if( dataProvider != NULL ){
					UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapIconUpdateData") );
					if( itemSingleObj == NULL ){
						// The world map SWF has no MapIconUpdateData class: init the mark directly instead of crashing
						eventInitMapMark( pUiObj, pStaticMark->_ID, pStaticMark->_Type, pStaticMark->_UiIconIdx );
					}
					else{
						itemSingleObj->SetObject( TEXT("newMarkObj"), pUiObj );
						itemSingleObj->SetInt( TEXT("id"), pStaticMark->_ID );
						itemSingleObj->SetInt( TEXT("type"), pStaticMark->_Type );
						itemSingleObj->SetString( TEXT("frameName"), pStaticMark->_UiIconIdx);
						dataProvider->SetElementObject( loopAddedCnt, itemSingleObj );
						++loopAddedCnt;
					}
				}
			}
		}//end of if need update

		//if valid,update position
		if( pStaticMark->_IsValid == TRUE && pStaticMark->_UiObjInWorldMap != NULL ){
			//FVector2D inWorldMapPos2D = pWorldMapTiles->Map2DOffsetFromWorld3DPos( pStaticMark->_OrigInMapPos, centerPos3D );
			FVector2D inWorldMapPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( pStaticMark->_OrigInMapPos, centerPos3D );
			pStaticMark->_UiObjInWorldMap->SetPosition( inWorldMapPos2D.X, inWorldMapPos2D.Y );
			pStaticMark->_UiObjInWorldMap->SetFloat( TEXT( "rotation" ), fYawDeg );
		}
	}//end of for loop of all static actors

	if( loopAddedCnt > 0 ){
		eventSetMapMarkIcon( dataProvider );
	}
}

//
void UMMOUIWorldMap::UpdateMarksNearPlayer( FVector centerPos3D, float fYawDeg )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap_UpdateMarksNearPlayer);

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	MMOFacade &gameFacade = MMOFacade::getSingleton();

	const FExportMap &curWorldmap = pMapDataMgr->GetCurWorldMap();
	const FExportMap &curMap = pMapDataMgr->GetCurMap();
	UBOOL isShowingCurSceneMap = ( curWorldmap.ServerID == curMap.ServerID);

	if( isShowingCurSceneMap == FALSE ){
		return;
	}
	const FInMapRegion *curWorldmapRegion = pMapDataMgr->GetCurWorldMapRegion();
	const FBox *curWorldmapRegionBox = NULL;
	if( curWorldmapRegion != NULL ){
		curWorldmapRegionBox = &curWorldmapRegion->BoundingBox;
	}

	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	FActorInMapUI *pDynmActor;
	MMOMapDataManager::MapDynamicActorIterator iter = pMapDataMgr->BeginIterateDymcActorNear();
	//UINT key;
	UGFxObject* dataProvider = NULL;
	INT loopAddedCnt = 0;
	INT maxPerFrameUpdateCnt = 6;
	for(  ; iter;  )
	{
		pDynmActor = &iter.Value();
		//key = iter.Key();
		if( pDynmActor->_NeedUpdateFlag & EACTUIUF_WorldMap && maxPerFrameUpdateCnt-- > 0 ){
			if( dataProvider == NULL ){
				dataProvider = GFXPlayer->CreateArray();
			}
			//reset update flag
			pDynmActor->_NeedUpdateFlag &= ~EACTUIUF_WorldMap;

			UGFxObject *pUiObj = pDynmActor->_UiObjInWorldMap;
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
				pUiObj = pDynmActor->_UiObjInWorldMap;
			}

			//show it again
			UBOOL canShow;
			canShow = ( pDynmActor->_Type != EACTMUI_InVisible ) ? TRUE : FALSE;
			pUiObj->SetVisible( canShow );
			//pUiObj->SetInt( TEXT("Type"), pDynmActor->_Type );
			//pUiObj->SetInt( TEXT("ID"), pDynmActor->_ID );
			////show correct icon for its type
			//pUiObj->GotoAndStop( pDynmActor->_UiIconIdx );
			//1
			//eventInitMapMark( pUiObj, pDynmActor->_ID, pDynmActor->_Type, pDynmActor->_UiIconIdx );
			//2
			if( dataProvider != NULL ){
				UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapIconUpdateData") );
				if( itemSingleObj == NULL ){
					// The world map SWF has no MapIconUpdateData class: init the mark directly instead of crashing
					eventInitMapMark( pUiObj, pDynmActor->_ID, pDynmActor->_Type, pDynmActor->_UiIconIdx );
				}
				else{
					itemSingleObj->SetObject( TEXT("newMarkObj"), pUiObj );
					itemSingleObj->SetInt( TEXT("id"), pDynmActor->_ID );
					itemSingleObj->SetInt( TEXT("type"), pDynmActor->_Type );
					itemSingleObj->SetString( TEXT("frameName"), pDynmActor->_UiIconIdx);
					dataProvider->SetElementObject( loopAddedCnt, itemSingleObj );
					++loopAddedCnt;
				}
			}
		}//end of if need update

		//if valid,update position
		if( pDynmActor->_IsValid == TRUE && pDynmActor->_Type != EACTMUI_InVisible ){
			AMMOActor* nearActor = gameFacade.GetCharacter( pDynmActor->_ActorID );
			if( nearActor == NULL ){
				//this become invalid,remove it(应该早被移除了的)
				pMapDataMgr->RemoveMapDymcActorNear( pDynmActor->_ActorID );
				if( pDynmActor->_UiObjInWorldMap ){
					pDynmActor->_UiObjInWorldMap->SetVisible( FALSE );
				}
				
				++iter;
				continue;
			}
			
			if( pDynmActor->_UiObjInWorldMap ){
				//if( IsMultilayerRegion && curWorldmapRegionBox != NULL ){
				//	UBOOL isMarkVisible = curWorldmapRegionBox->IsInside( nearActor->Location );
				//	pDynmActor->_UiObjInWorldMap->SetVisible( isMarkVisible );
				//}

				//FVector2D inWorldMapPos2D = pWorldMapTiles->Map2DOffsetFromWorld3DPos( nearActor->Location, centerPos3D );
				FVector2D inWorldMapPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( nearActor->Location, centerPos3D );
				pDynmActor->_UiObjInWorldMap->SetPosition( inWorldMapPos2D.X, inWorldMapPos2D.Y );
				pDynmActor->_UiObjInWorldMap->SetFloat( TEXT( "rotation" ), fYawDeg );
			}	
		}

		++iter;
	}//end of for loop of all nearby actors

	if( loopAddedCnt > 0 ){
		eventSetMapMarkIcon( dataProvider );
	}
}

//
void UMMOUIWorldMap::UpdateMarksGlobalPlayer( FVector centerPos3D, float fYawDeg )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap_UpdateMarksGlobalPlayer);

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	const FInMapRegion *curWorldmapRegion = pMapDataMgr->GetCurWorldMapRegion();
	const FBox *curWorldMapBox = NULL;
	if( curWorldmapRegion != NULL ){
		curWorldMapBox = &curWorldmapRegion->BoundingBox;
	}

	const FExportMap &curMap = pMapDataMgr->GetCurMap();
	INT curMapID = curMap.ServerID;
	INT curMapLineID = pMapDataMgr->GetCurSceneLineIn();
	INT curWorldMapID =  pMapDataMgr->GetCurWorldMap().ServerID;

	MMOFacade &gameFacade = MMOFacade::getSingleton();
	TeamManager& teamManager = TeamManager::getSingleton();

	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	FActorInMapUI *pDynmActor;
	MMOMapDataManager::MapDynamicActorIterator iter = pMapDataMgr->BeginIterateDymcActorGlobal();
	//UINT key;
	for(  ; iter;  )
	{
		pDynmActor = &iter.Value();
		//key = iter.Key();

		if( pDynmActor->_NeedUpdateFlag & EACTUIUF_WorldMap ){
			//reset update flag
			pDynmActor->_NeedUpdateFlag &= ~EACTUIUF_WorldMap;

			UGFxObject *pUiObj = pDynmActor->_UiObjInWorldMap;
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
				pUiObj = pDynmActor->_UiObjInWorldMap;
			}

			//是否在此地图
			UBOOL canShow = ( pDynmActor->_InMapID == curWorldMapID );
			pUiObj->SetVisible( canShow );
			//pUiObj->SetInt( TEXT("Type"), pDynmActor->_Type );
			//pUiObj->SetInt( TEXT("ID"), pDynmActor->_ID );
			////show correct icon for its type
			//pUiObj->GotoAndStop( pDynmActor->_UiIconIdx );
			eventInitMapMark( pUiObj, pDynmActor->_ID, pDynmActor->_Type, pDynmActor->_UiIconIdx );
		}//end of if need update

		//if valid,update position
		if( pDynmActor->_IsValid == TRUE ){
			FVector2D inWorldMapPos2D;

			switch( pDynmActor->_Type )
			{
				case EACTMUI_GlobalTeammate:
					{
						AMMOActor* nearActor = gameFacade.GetCharacter( pDynmActor->_ActorID );
						MMOTeamMember *pTeammate = teamManager.GetMemberById( pDynmActor->_ActorID );
						if( pTeammate == NULL ){
							//this become invalid,remove it(应该早被移除了的)
							pMapDataMgr->RemoveMapDymcActorGlobal( pDynmActor->_ActorID );
							++iter;
							//
							continue;
						}

						//
						FVector teammateWorldPos;
						teammateWorldPos = ( ( nearActor != NULL ) ? nearActor->Location : pTeammate->WorldPos );
						//
						if( pDynmActor->_UiObjInWorldMap ){
							pDynmActor->_InMapID = pTeammate->MapID;

							UBOOL canShow = ( pTeammate->MapID == curWorldMapID );
							if( canShow && curWorldMapID == curMapID ){
								//队友在当前大地图上，且当前大地图就是当前所在场景地图
								if( curMap.ServerMapType == E_est_Line ){
									canShow &= ( pTeammate->MapLineID == curMapLineID );
								}
							}

							if( canShow == FALSE ){
								pDynmActor->_UiObjInWorldMap->SetVisible( FALSE );
							}
							else{
								UBOOL isMarkInRegion = ( curWorldMapBox != NULL && curWorldMapBox->IsInside( teammateWorldPos ) );
								pDynmActor->_UiObjInWorldMap->SetVisible( isMarkInRegion );
							}
						}
						inWorldMapPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( teammateWorldPos, centerPos3D );
					}
					break;
				case EACTMUI_BattleFlag:
					{
						AMMOActor* nearActor = NULL;
						if( pDynmActor->_AidID < UINT(-1) ){
							nearActor = gameFacade.GetCharacter( pDynmActor->_AidID );
						}

						if( nearActor != NULL ){
							inWorldMapPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( nearActor->Location, centerPos3D );
						}
						else{
							inWorldMapPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( pDynmActor->_OrigInMapPos, centerPos3D );
						}
						
					}
					break;
				default:
					break;
			}

			//
			if( pDynmActor->_UiObjInWorldMap ){
				pDynmActor->_UiObjInWorldMap->SetPosition( inWorldMapPos2D.X, inWorldMapPos2D.Y );
				pDynmActor->_UiObjInWorldMap->SetFloat( TEXT( "rotation" ), fYawDeg );
			}	
		}

		++iter;
	}//end of for loop of all nearby actors

}

//
void UMMOUIWorldMap::UpdatePlayerMarksInMap( FVector centerPos3D, float fYawDeg )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap_UpdatePlayerMarksInMap);

	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	//INT mapID = pMapDataMgr->GetCurWorldMap().ServerID;
	TArray<FUserMarkInMapUI> &marksList = pMapDataMgr->GetCurWorldMapMarks();

	INT marksCnt = marksList.Num();
	INT sceneID = pMapDataMgr->GetCurWorldMap().ServerID;
	FUserMarkInMapUI *pMarkUI;
	for( INT i = 0; i < marksCnt; ++i ){
		pMarkUI = &marksList(i);

		if( pMarkUI->_NeedUpdateFlag & EACTUIUF_WorldMap ){
			//reset update flag
			pMarkUI->_NeedUpdateFlag &= ~EACTUIUF_WorldMap;

			UGFxObject *pUiObj = pMarkUI->_UiObjInWorldMap;
			//if is invalid now, hide unused this ui points
			if( pMarkUI->_IsValid == FALSE ){
				if( pUiObj != NULL ){
					pUiObj->SetVisible( FALSE );
				}
				continue;
			}

			//if ui object is not exit, create an new one
			if( pUiObj == NULL ){
				UBOOL createRst = CreateMapMarkUIObject( pMarkUI );
				if( createRst != TRUE ){
					continue;
				}
				pUiObj = pMarkUI->_UiObjInWorldMap;
			}

			//show it again
			pUiObj->SetVisible( TRUE );
			eventInitMapMark( pUiObj, pMarkUI->_ID, pMarkUI->_Type, pMarkUI->_UiIconIdx );
			//pUiObj->SetInt( TEXT("Type"), pMarkUI->_Type );
			//pUiObj->SetInt( TEXT("ID"), pMarkUI->_ID );
			////show correct icon for its type
			//pUiObj->GotoAndStop( pMarkUI->_UiIconIdx );
		}
		//if valid,update position
		if( pMarkUI->_IsValid == TRUE && pMarkUI->_UiObjInWorldMap != NULL ){
			FMapMark *pMarkData = pMapDataMgr->GetMapMarkData( sceneID, pMarkUI->_RelatedMarkID );

			if( pMarkData != NULL ){
				//FVector2D inWorldMapPos2D = pWorldMapTiles->Map2DOffsetFromWorld3DPos(  pMarkData->_Pos, centerPos3D );
				FVector2D inWorldMapPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( pMarkData->_Pos, centerPos3D );
				pMarkUI->_UiObjInWorldMap->SetPosition( inWorldMapPos2D.X, inWorldMapPos2D.Y );
				pMarkUI->_UiObjInWorldMap->SetFloat( TEXT( "rotation" ), fYawDeg );
			}
		}
	}//end of for
}

//
void UMMOUIWorldMap::UpdateMapPath( FVector centerPos3D, float fYawDeg )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap_UpdateMapPath);

	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	FVector curPlayerPos = Player->Location;

	//UBOOL isShowInWorldMap = ( pMapDataMgr->GetCurWorldMap().ServerID == pMapDataMgr->GetCurMap().ServerID );
	UBOOL isShowInWorldMap = ( _CurPanelType == ePnlTyp_WorldMap );
	TArray< FPathPointInMapUI > &path = pMapDataMgr->GetCurMapPath();
	INT cnt = path.Num();
	INT nowAtPosIdx = -1;
	UINT disBetween = 0xffffffff;
	for( INT i = 0; i < cnt; ++i ){
		FPathPointInMapUI *pPathPoint = &path(i);

		if( pPathPoint->_NeedUpdateFlag & EACTUIUF_WorldMap ){
			//reset update flag
			pPathPoint->_NeedUpdateFlag &= ~EACTUIUF_WorldMap;

			UGFxObject *pUiObj = pPathPoint->_UiObjInWorldMap;
			//if is invalid now, hide unused this ui points
			if( pPathPoint->_IsValid == FALSE || !isShowInWorldMap ){
				if( pUiObj != NULL ){
					pUiObj->SetVisible( FALSE );

					//eventRemoveChild( pPathPoint->_UiObjInWorldMap );
					//pPathPoint->_UiObjInWorldMap->RemoveFromRoot();
					//pPathPoint->_UiObjInWorldMap = NULL;
				}
				continue;
			}

			//if ui object is not exit, create an new one
			if( pUiObj == NULL ){
				UBOOL createRst = CreateMapMarkUIObject( pPathPoint );
				if( createRst != TRUE ){
					continue;
				}
				pUiObj = pPathPoint->_UiObjInWorldMap;
			}

			pUiObj->SetVisible( TRUE );
			eventInitMapMark( pUiObj, pPathPoint->_ID, pPathPoint->_Type, pPathPoint->_UiIconIdx );
			//pUiObj->SetInt( TEXT("Type"), pPathPoint->_Type );
			//pUiObj->SetInt( TEXT("ID"), pPathPoint->_ID );
			//pUiObj->GotoAndStop( pPathPoint->_UiIconIdx );
		}//end of if need update

		//if valid,update position
		if( pPathPoint->_IsValid == TRUE && pPathPoint->_UiObjInWorldMap != NULL ){
			//FVector2D inWorldMapPos2D = pWorldMapTiles->Map2DOffsetFromWorld3DPos( pPathPoint->_OrigInMapPos, centerPos3D );
			FVector2D inWorldMapPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( pPathPoint->_OrigInMapPos, centerPos3D );
			pPathPoint->_UiObjInWorldMap->SetPosition( inWorldMapPos2D.X, inWorldMapPos2D.Y );
			pPathPoint->_UiObjInWorldMap->SetFloat( TEXT( "rotation" ), fYawDeg );

			////UINT curDisBetween = curPlayerPos.DistanceSquared( pPathPoint->_OrigInMapPos );
			//INT xDis = curPlayerPos.X - pPathPoint->_OrigInMapPos.X;
			//INT yDis = curPlayerPos.Y - pPathPoint->_OrigInMapPos.Y;
			//UINT curDisBetween = xDis*xDis + yDis*yDis;
			//if( disBetween > curDisBetween ){
			//	nowAtPosIdx = i;
			//	disBetween = curDisBetween;
			//}
		}
	}//end of for loop of all static actors

	////隐藏已经经过的点
	//INT removePtCnt = 0;
	//if( disBetween <= MAP_PATH_POINT_REMOVE_LENGTH ){
	//	removePtCnt = nowAtPosIdx + 1;
	//}
	//for( INT i = 0; i < removePtCnt; ++i ){
	//	FPathPointInMapUI *pPathPoint = &path(i);
	//	if( pPathPoint->_IsValid == TRUE ){

	//		if( pPathPoint->_UiObjInWorldMap != NULL ){
	//			pPathPoint->_UiObjInWorldMap->SetVisible( FALSE );
	//		}

	//		if( pPathPoint->_UiObjInMiniMap != NULL ){
	//			pPathPoint->_UiObjInMiniMap->SetVisible( FALSE );
	//		}
	//		//pPathPoint->_NeedUpdateFlag |= EACTUIUF_WorldMap;
	//		pPathPoint->_IsValid = FALSE;
	//	}
	//}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIWorldMap::UpdateMapMarksInfomation( FVector centerPos3D, float fYawDeg )
{
	SCOPE_CYCLE_COUNTER(STAT_MMOWorldmap_UpdateMapMarksInfomation);

	if( IsToShowMapMarksExtraInfo != TRUE ){
		return;
	}

	MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	TArray< FActorInMapUI > &marksHintInfo = pMapDataMgr->GetCurWorldMapMarksHintInfo();

	INT cnt = marksHintInfo.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pMarkInfo = &marksHintInfo(i);
		//UGFxObject *pUiObj = pMarkInfo->_UiObjInWorldMap;

		if( pMarkInfo->_NeedUpdateFlag & EACTUIUF_WorldMap ){
			//reset update flag
			pMarkInfo->_NeedUpdateFlag &= ~EACTUIUF_WorldMap;
			eventInitNewMapMarkIcon( pMarkInfo->_Type, pMarkInfo->_ID, pMarkInfo->_IsValid, pMarkInfo->_DisplayName );
		}//end of if need update

		//if valid,update position
		if( pMarkInfo->_IsValid == TRUE ){
			//FVector2D inWorldMapPos2D = pWorldMapTiles->Map2DOffsetFromWorld3DPos( pMarkInfo->_OrigInMapPos, centerPos3D );
			FVector2D inWorldMapPos2D;
			if( pMarkInfo->_OrigInMapPos.IsZero() ){
				//无效坐标
				inWorldMapPos2D.X = inWorldMapPos2D.Y = 0;
			}
			else{
				inWorldMapPos2D = pWorldMapTiles->ScaledMap2DOffsetFromWorld3DPos( pMarkInfo->_OrigInMapPos, centerPos3D );
			}
			//pUiObj->SetPosition( inWorldMapPos2D.X, inWorldMapPos2D.Y );
			//pUiObj->SetFloat( TEXT( "rotation" ), fYawDeg );
			eventUpdateNewMapMarkIcon( pMarkInfo->_Type, pMarkInfo->_ID, inWorldMapPos2D.X, inWorldMapPos2D.Y, fYawDeg );
		}
	}//end of for loop of all
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//@desc:每次大地图隐藏时都会随之隐藏，需再次显示时需要要再次调用
void UMMOUIWorldMap::ShowMapMarksInfomation( UBOOL isToShow )
{ 
	IsToShowMapMarksExtraInfo = isToShow;
	if( MapExtraInfoView != NULL ){
		MapExtraInfoView->SetVisible( IsToShowMapMarksExtraInfo );
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIWorldMap::UpdateMapTilesPosition()
{
	if( MapBGLoader != NULL ){
		MapBGLoader->SetPosition( MapBGLoaderPos.X, MapBGLoaderPos.Y );
	}

	//MMOMapImgTiles *pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	//if( !pMapTiles->isReady() ){
	//	return;
	//}
	////FVector2D noScaledCenter = MapUiCenterPos / pMapTiles->_ZoomFactor;
	//pMapTiles->TilesGoAroundByPosInTile( MapUiCenterPos, true );

	////372，279为遮罩的半长宽
	////LastMoveModify = pMapTiles->KeepTilesNotExceed( PaperMaskBorder.X / 2, PaperMaskBorder.Y / 2 );
	////FVector2D offset = pMapTiles->TilesKeepNotExceedBoundAround( MapUiCenterPos, 372, 279 );
	////MapUiCenterPos -= LastMoveModify;

	//pMapTiles->TileTheMap();
}

UBOOL UMMOUIWorldMap::CheckMapPosition( float &moveDeltaX, float &moveDeltaY )
{
	UBOOL isOutside = FALSE;
	//是否已经到边缘
	//MapBGMoveLimit.X = ( PaperZoneSize.X * MapUIZoom - PaperMaskBorder.X ) / 2;
	//MapBGMoveLimit.Y = ( PaperZoneSize.Y * MapUIZoom - PaperMaskBorder.Y ) / 2;
	FLOAT limitX = MapBGMoveLimit.X;
	FLOAT limitY = MapBGMoveLimit.Y;
	//
	if( MapBGLoaderPos.X + moveDeltaX < -limitX ){
		moveDeltaX = -( MapBGLoaderPos.X + limitX ) * MapUIZoom;
		isOutside = TRUE;
	}
	//
	if( MapBGLoaderPos.X + moveDeltaX > limitX ){
		moveDeltaX = -( MapBGLoaderPos.X - limitX ) * MapUIZoom;
		isOutside = TRUE;
	}
	//
	if( MapBGLoaderPos.Y + moveDeltaY < -limitY ){
		moveDeltaY = -( MapBGLoaderPos.Y + limitY ) * MapUIZoom;
		isOutside = TRUE;
	}
	//
	if( MapBGLoaderPos.Y + moveDeltaY > limitY ){
		moveDeltaY = -( MapBGLoaderPos.Y - limitY ) * MapUIZoom;
		isOutside = TRUE;
	}

	return isOutside;
}
//
void UMMOUIWorldMap::MoveMapByDelta( INT deltaX, INT deltaY )
{
	if( deltaX == 0 && deltaY == 0 ){
		return;
	}

	FLOAT checkedDtaX = (FLOAT)deltaX;
	FLOAT checkedDtaY = (FLOAT)deltaY;
	CheckMapPosition( checkedDtaX, checkedDtaY );
	deltaX = (INT)checkedDtaX;
	deltaY = (INT)checkedDtaY;

	//
	//FVector2D realMapMove( deltaX / MapUIZoom, deltaY / MapUIZoom );
	MapBGLoaderPos.X += deltaX / MapUIZoom;
	MapBGLoaderPos.Y += deltaY / MapUIZoom;

	//
	MapUiCenterPos = ConvertPosInMapUIToWorldmapResPos( -deltaX, -deltaY );

	//
	UpdateMapTilesPosition();

	//
	UpdateWorldMapContents();
}

//
void UMMOUIWorldMap::HighlightMark( INT idxInStaticMap )
{
	TArray< FActorInMapUI > &staticMarks = MMOMapDataManager::getSingleton().GetCurWorldMapStaticActorPointsList();
	if( !staticMarks.IsValidIndex( idxInStaticMap ) ){
		return;
	}

	FActorInMapUI &staticMark = staticMarks( idxInStaticMap );
	//UI表现
	if( staticMark._UiObjInWorldMap ){
		UGFxObject *pHintAnim = staticMark._UiObjInWorldMap->GetObject( TEXT("glow_mc") );
		if( pHintAnim != NULL ){
			pHintAnim->GotoAndPlayI( 2 );
		}
	}
	
	//满屏时不移动地图,ZoomLevel为0即为满屏(地图缩到最小)
	if( ZoomLevel != 0 ){
		MMOMapImgTiles *pWorldMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
		//FVector2D markInWorldMapPos2D = pWorldMapTiles->Map2DPosByWorld3DPos( staticMark._OrigInMapPos );

		FVector2D moveDelta = ConvertWorld3DPosToPosInMapUI( staticMark._OrigInMapPos );
		MoveMapByDelta( (INT)-moveDelta.X , (INT)-moveDelta.Y );
	}
}
//===================================================================================================================================
//
void UMMOUIWorldMap::InitMapStaticMarks()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	pMapDataMgr->PrepareWorldMapStaticActorInMapUI();

	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurWorldMapStaticActorPointsList();

	INT cnt = staticMarks.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pStaticMark = &staticMarks(i);

		UGFxObject *pUiObj = pStaticMark->_UiObjInWorldMap;
		//if ui object is not exit, create an new one
		if( pUiObj == NULL ){
			UBOOL createRst = CreateMapMarkUIObject( pStaticMark );
			if( createRst != TRUE ){
				continue;
			}
			pUiObj = pStaticMark->_UiObjInWorldMap;
		}
	}

	//initial map npc related to me
	pMapDataMgr->InitWorldMapPuttedUsefulNpcs();
	////initial map grouped npcs(monsters)
	//pMapDataMgr->InitWorldMapPuttedNpcGroups();
	////initial map FB points in map
	//pMapDataMgr->InitWorldMapPuttedFBs();
	////initial map Navgation points in map
	//pMapDataMgr->InitWorldMapNavPoints();
	pMapDataMgr->ClassifyWorldMapStaicMarks();
}

//
void UMMOUIWorldMap::PrepareShowAddUserMarkPanel( float uiPosX, float uiPosY )
{
	AddMarkPos = ConvertPosInMapUIToWorld3DPos( uiPosX, uiPosY );

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	INT curMapID = pMapDataMgr->GetCurWorldMap().ServerID;

	//
	SelectedUserMarkIdx = -1;

	//
	TArray<FUserMarkInMapUI> &markList = pMapDataMgr->GetCurWorldMapMarks();
	INT marksCnt = markList.Num();
	//INT removeIdx = -1;
	INT sceneID = pMapDataMgr->GetCurWorldMap().ServerID;
	FLOAT gapValue = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES)->_ZoomFactor * 8;
	FMapMark *pMarkData = NULL;
	for( INT i = 0; i < marksCnt; ++i ){
		FUserMarkInMapUI &markUI = markList(i);
		if( markUI._IsValid == FALSE ){
			continue;
		}

		pMarkData = pMapDataMgr->GetMapMarkData( sceneID, markUI._RelatedMarkID );
		if( pMarkData == NULL ){
			continue;
		}

		FVector2D inUIPosOffset = ConvertWorld3DPosToPosInMapUI( pMarkData->_Pos );
		inUIPosOffset.X -= uiPosX;
		inUIPosOffset.Y -= uiPosY;

		if( -gapValue <= inUIPosOffset.X && inUIPosOffset.X <= gapValue && -gapValue <= inUIPosOffset.Y && inUIPosOffset.Y <= gapValue ){
			SelectedUserMarkIdx = i;
			break;
		}
	}

	//
	FVector2D panelShowPos;
	panelShowPos.X = MaskZonePos.X + uiPosX + PaperMaskBorder.X / 2;
	panelShowPos.Y = MaskZonePos.Y + uiPosY + PaperMaskBorder.Y / 2;
	_UserMarkPanelMc->SetPosition( panelShowPos.X, panelShowPos.Y );

	FString markName;
	BYTE markType = 0;
	if( SelectedUserMarkIdx >= 0 ){
		//FUserMarkInMapUI &markUI = markList( SelectedUserMarkIdx );
		//name
		markName = pMarkData->_MarkName;
		//type
		markType = pMarkData->_MarkType;
	}
	else{
		//_UserMarkNameTf->SetString( TEXT("defaultText"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.WorldMapDefalutUserMarkName") ) );
	}
	//
	//name
	eventCallASSetUserMarkPanelIconName( markName );
	//type
	_ChooseUserMarkTypeBtn[markType]->SetBool( TEXT("selected"), TRUE );

	//share user mark list
	UpdateMapUserMarkShareList();

	//
	_UserMarkPanelMc->SetVisible( TRUE );
}
//
//============================================================================================================================================
void UMMOUIWorldMap::UpdateMapUserMarkShareList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	UGFxObject* shareTargetListItem = NULL;
	UINT addListItemCnt = 0;
	//
	UBOOL isInTeam = TeamManager::getSingleton().HasTeam();
	UBOOL isTeamLeader = isInTeam && TeamManager::getSingleton().IsTeamLeader();
	if( isTeamLeader ){
		shareTargetListItem = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapDropDownListData") );
		shareTargetListItem->SetString( TEXT("label"),  MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.ShareUserMarkTargetList0") ) );
		shareTargetListItem->SetInt( TEXT("inputedID1"), ESUType_Team );

		dataProvider->SetElementObject( addListItemCnt, shareTargetListItem );

		++addListItemCnt;
	}

	//
	const GuildBaseInfo& myGuildInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	UBOOL isGuildMember = ( myGuildInfo.Id != 0 );
	UBOOL isGuildLeader = isGuildMember && ( myGuildInfo.LeaderName == GetPlayerRoleName() );
	if( isGuildLeader ){
		shareTargetListItem = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapDropDownListData") );
		shareTargetListItem->SetString( TEXT("label"),  MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.ShareUserMarkTargetList1") ) );
		shareTargetListItem->SetInt( TEXT("inputedID1"), ESUType_Guild );

		dataProvider->SetElementObject( addListItemCnt, shareTargetListItem );

		++addListItemCnt;
	}

	//
	eventFlashWorldMapShareUserMarkkList(dataProvider);

	//
	if( _ShareUserMarkList != NULL ){
		_ShareUserMarkList->SetVisible( addListItemCnt > 0 );
	}

	//
	if( _ShareUserMarkBtn != NULL ){
		_ShareUserMarkBtn->SetVisible( addListItemCnt > 0 );
	}
}
//
void UMMOUIWorldMap::UpdateShareUserMarkButtonState( INT shareTargetType )
{
	//INT shareTargetType = _ShareUserMarkList->GetInt( TEXT("selectedIndex") );

	switch( shareTargetType )
	{
	case ESUType_Team:
		{
			UBOOL isInTeam = TeamManager::getSingleton().HasTeam();
			UBOOL isTeamLeader = isInTeam && TeamManager::getSingleton().IsTeamLeader();

			_ShareUserMarkBtn->SetBool( TEXT("enabled"), isTeamLeader );
		}
		break;
	case ESUType_Guild:
		{
			const GuildBaseInfo& myGuildInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			UBOOL isGuildMember = ( myGuildInfo.Id != 0 );
			UBOOL isGuildLeader = isGuildMember && ( myGuildInfo.LeaderName == GetPlayerRoleName() );

			_ShareUserMarkBtn->SetBool( TEXT("enabled"), isGuildLeader );
		}
		break;
	}
	
}

//===========================================================================================================================================
const FMapMark *UMMOUIWorldMap::AddMapMark()
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	INT curMapID = pMapDataMgr->GetCurWorldMap().ServerID;
	//
	FInMapRegion *pCurReginInMap = MMOMapDataManager::getSingleton().GetReginInMap( pMapDataMgr->GetCurWorldMap().ServerID, InRegionIdx );
	if( pCurReginInMap == NULL ){
		return NULL;
	}

	//
	FUserMarkInMapUI *pMarkUI = NULL;
	FMapMark *pMark = NULL;
	if( SelectedUserMarkIdx >= 0 ){
		TArray<FUserMarkInMapUI> &markList = pMapDataMgr->GetCurWorldMapMarks();
		pMarkUI = &markList( SelectedUserMarkIdx );
		pMark = pMapDataMgr->GetMapMarkData( curMapID, pMarkUI->_RelatedMarkID );
	}
	else{
		//add
		pMarkUI = pMapDataMgr->AddMapMark( curMapID, pCurReginInMap->VolumeName );
		pMark = pMapDataMgr->GetMapMarkData( curMapID, pMarkUI->_RelatedMarkID );//pMapDataMgr->AddMapMark( curMapID, pCurReginInMap->VolumeName ); 

		//position
		pMark->_Pos = AddMarkPos;
	}

	//name
	pMark->_MarkName = _UserMarkNameTf->GetString( TEXT("text") );
	if( pMark->_MarkName.IsEmpty() ){
		pMark->_MarkName = _UserMarkNameTf->GetString( TEXT("defaultText") );
	}

	pMarkUI->_DisplayName = pMark->_MarkName;
	//type
	for( UINT i = 0; i < 6; ++i ){
		UBOOL isChecked = _ChooseUserMarkTypeBtn[i]->GetBool( TEXT("selected") );
		if( isChecked == TRUE ){
			pMark->_MarkType = i;
			break;
		}
	}
	pMarkUI->_UiIconIdx = FString::Printf( TEXT("mark%d"), ( pMark->_MarkType + 1) );

	pMarkUI->_NeedUpdateFlag = EACTUIUF_WorldMap;

	//add ui
	//FUserMarkInMapUI *pMarkUI = pMapDataMgr->AddMapMark( pMark );
	if( pMarkUI->_UiObjInWorldMap == NULL ){
		UBOOL createRst = CreateMapMarkUIObject( pMarkUI );
		if( createRst == TRUE ){
			UGFxObject *newMarkObj = pMarkUI->_UiObjInWorldMap;
			FVector2D markMapPos = ConvertWorld3DPosToPosInMapUI( AddMarkPos );
			newMarkObj->SetPosition( markMapPos.X, markMapPos.Y );
			newMarkObj->SetVisible( FALSE );
		}
	}

	//
	return pMark;
}

//=========================================================================================================
void UMMOUIWorldMap::DelMapMark()
{
	if( SelectedUserMarkIdx >= 0 ){
		MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
		TArray<FUserMarkInMapUI> &markList = pMapDataMgr->GetCurWorldMapMarks();
		FUserMarkInMapUI &markUI = markList( SelectedUserMarkIdx );

		//remove
		INT toRemoveMarkID = markUI._RelatedMarkID;
		INT curMapID = pMapDataMgr->GetCurWorldMap().ServerID;
		pMapDataMgr->RemoveMapMark( curMapID, toRemoveMarkID );
	}
}

//
void UMMOUIWorldMap::ShareCurUserMarkTo( INT targetType )
{
	const FMapMark *pNewMark = AddMapMark();
	if( pNewMark == NULL ){
		return;
	}

	INT channelID = 0;
	INT channelTargetID = 0;
	switch( targetType )
	{
	case ESUType_Team:
		{
			//UBOOL isInTeam = TeamManager::getSingleton().HasTeam();
			//UBOOL isTeamLeader = isInTeam && TeamManager::getSingleton().IsTeamLeader();
			//if( !isTeamLeader ){
			//	return;
			//}

			channelID = otTeam;
			channelTargetID = TeamManager::getSingleton().GetMyTeamID();

			//ActiveCurChatMessageFilterChannel( EChat_Channel(ECC_Team) );
		}
		break;
	case ESUType_Guild:
		{
			//const GuildBaseInfo& myGuildInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			//UBOOL isGuildMember = ( myGuildInfo.Id != 0 );
			//UBOOL isGuildLeader = isGuildMember && ( myGuildInfo.LeaderName == ctrl->GetActorName() );
			//if( !isGuildLeader ){
			//	return;
			//}

			channelID = otGuild;
			channelTargetID = MMOGuildData::getSingleton().GetGuildBaseInfo().Id;

			//ActiveCurChatMessageFilterChannel( EChat_Channel(ECC_Guild) );
		}
		break;
	default:
		return;
	}
	
	//
	FString addUserMarkMsgContent;
	addUserMarkMsgContent += ADD_MAP_USER_MARK_PROFIX;
	//0
	addUserMarkMsgContent += pNewMark->_MarkName;
	addUserMarkMsgContent += TEXT(",");
	//1
	addUserMarkMsgContent += appItoa( pNewMark->_MarkType );
	addUserMarkMsgContent += TEXT(",");
	//2
	addUserMarkMsgContent += appItoa( pNewMark->_SceneID );
	addUserMarkMsgContent += TEXT(",");
	//3
	addUserMarkMsgContent += pNewMark->_RegionName;
	addUserMarkMsgContent += TEXT(",");
	//4
	addUserMarkMsgContent += appItoa( (INT)pNewMark->_Pos.X );
	addUserMarkMsgContent += TEXT(",");
	//5
	addUserMarkMsgContent += appItoa( (INT)pNewMark->_Pos.Y );
	addUserMarkMsgContent += TEXT(",");
	//6
	addUserMarkMsgContent += appItoa( 0 );
	addUserMarkMsgContent += TEXT(",");
	addUserMarkMsgContent += TEXT(">");

	CMessageHandler::GetInstance().Send_Chat( channelID , channelTargetID , addUserMarkMsgContent );
	//
	CloseUserMarkPanel();
}
//=============================================================================================================
void UMMOUIWorldMap::CloseUserMarkPanel()
{
	SelectedUserMarkIdx = -1;
	_UserMarkPanelMc->SetVisible( FALSE );
}
//==============================================================================================================================================================
void UMMOUIWorldMap::ToggleShowNearPlayerMarksAndMyself( UBOOL isShow )
{
	FActorInMapUI *pDynmActor;
	MMOMapDataManager::MapDynamicActorIterator iter = MMOMapDataManager::getSingleton().BeginIterateDymcActorNear();
	for(  ; iter; ++iter ){
		pDynmActor = &iter.Value();

		if( pDynmActor->_IsValid == TRUE && pDynmActor->_UiObjInWorldMap != NULL ){
			pDynmActor->_UiObjInWorldMap->SetVisible( isShow );
		}
	}

	//show/hide myself
	MyIcon->SetVisible( isShow );
	MyPostion_Tf->SetVisible( isShow );
}

//
void UMMOUIWorldMap::ToggleShowStaticMarksOf( INT enumEActorInMapFlag, UBOOL isShow )
{
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurWorldMapStaticActorPointsList();

	switch( enumEActorInMapFlag )
	{
	case EACTUIF_FunctionNpc:
		CanFuncNpcShow = isShow;
		break;
	case EACTUIF_TaskNpc:
		CanTaskNpcShow = isShow;
		break;
	default:
		break;
	}

	INT cnt = staticMarks.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pStaticMark = &staticMarks(i);

		UGFxObject *pUiObj = pStaticMark->_UiObjInWorldMap;
		bool isFuncNpcOfType = ( pStaticMark->_IsValid && ( pStaticMark->_ToggleFlag & enumEActorInMapFlag ) > 0 );
		//
		if( isFuncNpcOfType && pUiObj != NULL){
			pUiObj->SetVisible( isShow );

		}
	}
}

//============================================================================================================================================
//更新地图下拉框
void UMMOUIWorldMap::UpdateMapList()
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();
	//UGlobalMapInfo *pGlobalMapInfo = UGlobalMapInfo::GetSingleton();

	//获得当前区域里的所有NPC
	TMap< INT, TArray<FInMapRegion*> > &mapRegions = pMapDataManager->GetAllMapRegions();
	INT curMapID = pMapDataManager->GetCurWorldMap().ServerID;

	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	UINT itemObjIdx = 0;
	UINT curItemObjIdx = 0;

	UBOOL exitMapShowInList = FALSE;
	UGFxObject* mapListObj = NULL;
	FInMapRegion* pRegion = NULL;
	for( TMap< INT, TArray<FInMapRegion*> >::TIterator it(mapRegions); it; ++it ){
		TArray<FInMapRegion*> &singleMapRegions = it.Value();

		UINT regionCnt = singleMapRegions.Num();

		for( UINT i = 0; i < regionCnt; ++i ){
			pRegion = singleMapRegions(i);

			bool isCurIn = ( curMapID == pRegion->MapID && i == InRegionIdx );
			//if no need to display in list
			if( !isCurIn ){
				continue;
			}

			//写入控件数据
			mapListObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapDropDownListData") );

			mapListObj->SetString( TEXT("label"), pRegion->DisplayName );
			if( pRegion->DisplayName.IsEmpty() ){
				//等配了删除这节
				FString name = UGlobalMapInfo::GetSingleton()->FindMapInfo( pRegion->MapID ).MapDisplayName;
				name += TEXT("缺少名字");
				mapListObj->SetString( TEXT("label"), name );
			}
			mapListObj->SetInt( TEXT("listIndex"), itemObjIdx );

			mapListObj->SetInt( TEXT("inputedID1"), pRegion->MapID );
			mapListObj->SetInt( TEXT("inputedID2"), i );

			dataProvider->SetElementObject( itemObjIdx, mapListObj );

			if( isCurIn ){
				curItemObjIdx = itemObjIdx;
			}
			++itemObjIdx;

			exitMapShowInList = TRUE;
		}
	}

	if( _MapListMc != NULL ){
		_MapListMc->SetVisible( exitMapShowInList );
	}
	//_MapListMc->SetInt( TEXT("selectedIndex"), curItemObjIdx );
	//
	eventFlashWorldMapMapList( dataProvider, curItemObjIdx );
}
//============================================================================================================================================
//更新任务下拉框
void UMMOUIWorldMap::UpdateMapTaskList()
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();

	//获得当前区域里的所有NPC
	const TMap< UINT, UINT > &npcInfoInMapRegion = pMapDataManager->GetWorldMapPuttedUsefulNpcs();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	UINT itemObjIdx = 0;
	UGFxObject* mapTaskListObj = NULL;

	//完成的任务
	TMap< UINT, UINT > &completeTasks = pMapDataManager->GetCompleteTaskInWorldMap();
	for( TMap< UINT, UINT >::TIterator it(completeTasks); it; ++it ){
		UINT npcIdx = it.Value();
		if( npcInfoInMapRegion.Find( npcIdx ) == NULL ){
			//this task npc is not in current world map region
			continue;
		}
		//
		//UINT npcPointIdx = *npcInfoInMapRegion.Find( npcID );
		//
		UINT taskID = it.Key();
		const UQuest* pTask = MMOQuestData::getSingleton().GetQuestData( taskID );
		if( pTask == NULL ){
			continue;
		}
		//写入控件数据
		mapTaskListObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapDropDownListData") );

		mapTaskListObj->SetString( TEXT("label"), pTask->QuestName );
		mapTaskListObj->SetInt( TEXT("listIndex"), itemObjIdx );

		mapTaskListObj->SetInt( TEXT("inputedID1"), npcIdx );

		dataProvider->SetElementObject( itemObjIdx, mapTaskListObj );
		++itemObjIdx;
	}

	//为完成的任务
	TMap< UINT, UINT > &unCompleteTasks = pMapDataManager->GetUncompleteTaskInWorldMap();
	for( TMap< UINT, UINT >::TIterator it(unCompleteTasks); it; ++it ){
		UINT npcID = it.Value();
		if( npcInfoInMapRegion.Find( npcID ) == NULL ){
			//this task npc is not in current world map region
			continue;
		}
		//
		//UINT npcPointIdx = *npcInfoInMapRegion.Find( npcID );
		//
		UINT taskID = it.Key();
		const UQuest* pTask = MMOQuestData::getSingleton().GetQuestData( taskID );
		if( pTask == NULL ){
			continue;
		}
		//写入控件数据
		mapTaskListObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapDropDownListData") );

		mapTaskListObj->SetString( TEXT("label"), pTask->QuestName );
		mapTaskListObj->SetInt( TEXT("listIndex"), itemObjIdx );

		mapTaskListObj->SetInt( TEXT("inputedID1"), npcID );

		dataProvider->SetElementObject( itemObjIdx, mapTaskListObj );
		++itemObjIdx;
	}

	//可以接的任务
	TMap< UINT, UINT > &acceptableTasks = pMapDataManager->GetAcceptableTaskInWorldMap();
	for( TMap< UINT, UINT >::TIterator it(acceptableTasks); it; ++it ){
		UINT npcID = it.Value();
		if( npcInfoInMapRegion.Find( npcID ) == NULL ){
			//this task npc is not in current world map region
			continue;
		}
		//
		//UINT npcPointIdx = *npcInfoInMapRegion.Find( npcID );
		//
		UINT taskID = it.Key();
		const UQuest* pTask = MMOQuestData::getSingleton().GetQuestData( taskID );
		if( pTask == NULL ){
			continue;
		}
		//写入控件数据
		mapTaskListObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapDropDownListData") );

		mapTaskListObj->SetString( TEXT("label"), pTask->QuestName );
		mapTaskListObj->SetInt( TEXT("listIndex"), itemObjIdx );

		mapTaskListObj->SetInt( TEXT("inputedID1"), npcID );

		dataProvider->SetElementObject( itemObjIdx, mapTaskListObj );
		++itemObjIdx;
	}

	//
	eventFlashWorldMapTaskList(dataProvider);
}

//============================================================================================================================================
//更新NPC，怪点，副本列表
void UMMOUIWorldMap::UpdateMapStaticMarkList()
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	UINT itemObjIdx = 0;
	TArray< FActorInMapUI > &staticMarks = pMapDataManager->GetCurWorldMapStaticActorPointsList();

	//NPC
	enum{
		MapMarkListClass_FunNpc,
		MapMarkListClass_TaskNpc,
		MapMarkListClass_Max
	};

	const TMap< UINT, UINT > &npcInfoInMapRegion = pMapDataManager->GetWorldMapPuttedUsefulNpcs();

	TArray<UINT> funcNpcIdx[MapMarkListClass_Max];
	for ( TMap< UINT, UINT >::TConstIterator iter(npcInfoInMapRegion); iter; ++iter ){
		UINT npcPointIdx = iter.Value();
		if( !staticMarks.IsValidIndex( npcPointIdx ) ){
			continue;
		}

		FActorInMapUI &npcPointInfo = staticMarks( npcPointIdx );
		if( npcPointInfo._ToggleFlag & EACTUIF_FunctionNpc ){
			funcNpcIdx[MapMarkListClass_FunNpc].Push( npcPointIdx );
		}
		if( npcPointInfo._ToggleFlag & EACTUIF_TaskNpc ){
			funcNpcIdx[MapMarkListClass_TaskNpc].Push( npcPointIdx );
		}
	}

	//第二级表项：NPC明细
	for( INT i = 0; i < MapMarkListClass_Max; ++i ){
		TArray<UINT> &npcIdxArray = funcNpcIdx[i];
		INT npcCnt = npcIdxArray.Num();

		for( INT j = 0; j < npcCnt; ++j ){
			if( j == 0 ){
				//NPC标题
				UGFxObject* npcListTitleObj = GFXPlayer->CreateObject( TEXT("ActionScript3.TreeListData") );
				FString titleLookUp = TEXT( "MMOGame.WorldMap.WorldMapUiText");
				titleLookUp += appItoa( i );
				npcListTitleObj->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( titleLookUp ) );
				npcListTitleObj->SetInt( TEXT("index"), itemObjIdx );
				npcListTitleObj->SetInt( TEXT("type"), 2 );
				npcListTitleObj->SetInt( TEXT("deepth"), 0 );
				npcListTitleObj->SetBool( TEXT("visible"), TRUE );

				dataProvider->SetElementObject( itemObjIdx, npcListTitleObj );
				++itemObjIdx;
			}

			//第二级表项：NPC明细
			UINT npcPointIdx = npcIdxArray(j);
			if( !staticMarks.IsValidIndex( npcPointIdx ) ){
				continue;
			}

			UGFxObject* itemNpcObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapStaticMarkData") );

			FActorInMapUI &npcPointInfo = staticMarks( npcPointIdx );

			itemNpcObj->SetString( TEXT("label"), npcPointInfo._DisplayName );

			const UNPCTemplate* npcTemp = mmoFacade.FindNPCTemplate( npcPointInfo._AidID );
			if( npcTemp != NULL ){
				itemNpcObj->SetString( TEXT("ExtName"), npcTemp->IdentityName );
			}
			else{
				itemNpcObj->SetString( TEXT("ExtName"), TEXT("") );
			}

			itemNpcObj->SetInt( TEXT("index"), itemObjIdx );
			itemNpcObj->SetInt( TEXT("type"), 0 );
			itemNpcObj->SetInt( TEXT("deepth"), 1 );
			itemNpcObj->SetBool( TEXT("visible"), TRUE );

			itemNpcObj->SetString( TEXT("IconType"), npcPointInfo._UiIconIdx );
			itemNpcObj->SetInt( TEXT("Index"), npcPointIdx );

			dataProvider->SetElementObject( itemObjIdx, itemNpcObj );
			++itemObjIdx;
		}
	}


	////怪点
	//const TMap< INT, UINT > &npcGroupInfoInMapRegion = pMapDataManager->GetWorldMapPuttedNpcGroups();

	//if( npcGroupInfoInMapRegion.Num() > 0 ){
	//	UGFxObject* npcGroupListTitleObj = GFXPlayer->CreateObject( TEXT("ActionScript3.TreeListData") );

	//	npcGroupListTitleObj->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( "MMOGame.WorldMap.WorldMapUiText3") );
	//	npcGroupListTitleObj->SetInt( TEXT("index"), itemObjIdx );
	//	npcGroupListTitleObj->SetInt( TEXT("type"), 2 );
	//	npcGroupListTitleObj->SetInt( TEXT("deepth"), 0 );
	//	npcGroupListTitleObj->SetBool( TEXT("visible"), TRUE );

	//	dataProvider->SetElementObject( itemObjIdx, npcGroupListTitleObj );
	//	++itemObjIdx;
	//}

	////第二级表项：怪点明细
	//for ( TMap< INT, UINT >::TConstIterator iter(npcGroupInfoInMapRegion); iter; ++iter ){
	//	UINT npcGroupPointIdx = iter.Value();
	//	if( !staticMarks.IsValidIndex( npcGroupPointIdx ) ){
	//		continue;
	//	}

	//	UGFxObject* itemGameLandObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapStaticMarkData") );

	//	FActorInMapUI &npcGroupPointInfo = staticMarks( npcGroupPointIdx );
	//	INT strPos = npcGroupPointInfo._DisplayName.InStr( TEXT(":") );
	//	FString listName;
	//	if( strPos >= 0 ){
	//		INT charCnt = npcGroupPointInfo._DisplayName.Len() - strPos - 1;
	//		listName = npcGroupPointInfo._DisplayName.Right( charCnt );
	//	}
	//	itemGameLandObj->SetString( TEXT("label"), listName );
	//	itemGameLandObj->SetString( TEXT("ExtName"), TEXT("") );
	//	itemGameLandObj->SetInt( TEXT("index"), itemObjIdx );
	//	itemGameLandObj->SetInt( TEXT("type"), 0 );
	//	itemGameLandObj->SetInt( TEXT("deepth"), 1 );
	//	itemGameLandObj->SetBool( TEXT("visible"), TRUE );

	//	itemGameLandObj->SetString( TEXT("IconType"), npcGroupPointInfo._UiIconIdx );
	//	itemGameLandObj->SetInt( TEXT("Index"), npcGroupPointIdx );

	//	dataProvider->SetElementObject( itemObjIdx, itemGameLandObj );
	//	++itemObjIdx;
	//}

	//FB
	const TMap< UINT, UINT > &fbInfoInMapRegion = pMapDataManager->GetWorldMapPuttedFBs();

	if( fbInfoInMapRegion.Num() > 0 ){
		UGFxObject* fbListTitleObj = GFXPlayer->CreateObject( TEXT("ActionScript3.TreeListData") );

		fbListTitleObj->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( "MMOGame.WorldMap.WorldMapUiText4") );
		fbListTitleObj->SetInt( TEXT("index"), itemObjIdx );
		fbListTitleObj->SetInt( TEXT("type"), 2 );
		fbListTitleObj->SetInt( TEXT("deepth"), 0 );
		fbListTitleObj->SetBool( TEXT("visible"), TRUE );

		dataProvider->SetElementObject( itemObjIdx, fbListTitleObj );
		++itemObjIdx;
	}

	//第二级表项：FB明细
	for ( TMap< UINT, UINT >::TConstIterator iter(fbInfoInMapRegion); iter; ++iter ){
		UINT fbPointIdx = iter.Value();
		if( !staticMarks.IsValidIndex( fbPointIdx ) ){
			continue;
		}
		
		UGFxObject* itemFbObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapStaticMarkData") );

		FActorInMapUI &fbPointInfo = staticMarks( fbPointIdx );
		itemFbObj->SetString( TEXT("label"), fbPointInfo._DisplayName );
		itemFbObj->SetString( TEXT("ExtName"), TEXT("") );
		itemFbObj->SetInt( TEXT("index"), itemObjIdx );
		itemFbObj->SetInt( TEXT("type"), 0 );
		itemFbObj->SetInt( TEXT("deepth"), 1 );
		itemFbObj->SetBool( TEXT("visible"), TRUE );

		itemFbObj->SetString( TEXT("IconType"), fbPointInfo._UiIconIdx );
		itemFbObj->SetInt( TEXT("Index"), fbPointIdx );

		dataProvider->SetElementObject( itemObjIdx, itemFbObj );
		++itemObjIdx;
	}

	//寻路点
	UBOOL hasNavPointsTitle = FALSE;
	const TMap< UINT, UINT > &mapNavPoints = pMapDataManager->GetWorldMapNavPoints();
	for ( TMap< UINT, UINT >::TConstIterator iter(mapNavPoints); iter; ++iter ){
		UINT mapNavPointInStaticMarkIdx = iter.Value();
		if( !staticMarks.IsValidIndex( mapNavPointInStaticMarkIdx ) ){
			continue;
		}

		if( !hasNavPointsTitle ){
			//标题
			UGFxObject* navPointListTitleObj = GFXPlayer->CreateObject( TEXT("ActionScript3.TreeListData") );

			navPointListTitleObj->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.WorldMapUiText5") ) );
			navPointListTitleObj->SetInt( TEXT("index"), itemObjIdx );
			navPointListTitleObj->SetInt( TEXT("type"), 2 );
			navPointListTitleObj->SetInt( TEXT("deepth"), 0 );
			navPointListTitleObj->SetBool( TEXT("visible"), TRUE );

			dataProvider->SetElementObject( itemObjIdx, navPointListTitleObj );
			++itemObjIdx;

			hasNavPointsTitle = TRUE;
		}

		//路点明细
		UGFxObject* itemFbObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapStaticMarkData") );

		FActorInMapUI &navPointInfo = staticMarks( mapNavPointInStaticMarkIdx );
		itemFbObj->SetString( TEXT("label"), navPointInfo._DisplayName );
		itemFbObj->SetString( TEXT("ExtName"), TEXT("") );
		itemFbObj->SetInt( TEXT("index"), itemObjIdx );
		itemFbObj->SetInt( TEXT("type"), 0 );
		itemFbObj->SetInt( TEXT("deepth"), 1 );
		itemFbObj->SetBool( TEXT("visible"), TRUE );

		itemFbObj->SetString( TEXT("IconType"), TEXT("") );
		itemFbObj->SetInt( TEXT("Index"), mapNavPointInStaticMarkIdx );

		dataProvider->SetElementObject( itemObjIdx, itemFbObj );
		++itemObjIdx;
	}

	eventFlashWorldMapPointsList(dataProvider);
}
//============================================================================================================================================
//
void UMMOUIWorldMap::Zoom(UBOOL ZoomIn)
{
	if( ZoomIn ){
		ZoomTo( ZoomLevel + 1 );
	}
	else{
		ZoomTo( ZoomLevel - 1 );
	}
}
	//
void UMMOUIWorldMap::ZoomTo(INT level)
{
	ZoomLevel = Clamp( level, 0, MaxZoomLevel );

	MapUIZoom = (float)ZoomLevel * 0.125 + MapUIZoomDefault;
	//if( MapUIZoom > 1 ){
	//	MapUIZoom = 1;
	//}

	MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES)->_ZoomFactor = MapUIZoom * MapUIResScale;
	ResizeMap();

	//eventCallASSetMapScaleSlider( (FLOAT)ZoomLevel / MaxZoomLevel );
	eventCallASSetMapScaleSlider( MaxZoomLevel, ZoomLevel );
	//
	FLOAT moveDeltaX = 0;
	FLOAT moveDeltaY = 0;
	UBOOL isNeedRePos = CheckMapPosition( moveDeltaX, moveDeltaY );

	////update path points
	////if( Player != NULL && Player->isMoving ){
	//	MMOGameEvent_Any evt(GameEvent_UpdateMovePath);
	//	MMOUI::PostGameEvent(&evt);
	////}

	//
	if( isNeedRePos ){
		MoveMapByDelta( moveDeltaX, moveDeltaY );
	}
	else{
		UpdateWorldMapContents();
	}
}

//
void UMMOUIWorldMap::ResizeMap()
{
	//Map2DWidth = OriMap2DWidth*ZoomQuot;
	//Map2DHeight = OriMap2DHeight*ZoomQuot;

	//MMOMapImgTiles* pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	FASDisplayInfo DI = MapPaper->GetDisplayInfo();
	DI.hasXScale = true;
	DI.hasYScale = true;
	DI.XScale = MapUIZoom * 100;
	DI.YScale = MapUIZoom * 100;
	MapPaper->SetDisplayInfo(DI);
	//
	MapBGMoveLimit.X = ( PaperZoneSize.X - PaperMaskBorder.X / MapUIZoom ) / 2;
	MapBGMoveLimit.X = ( MapBGMoveLimit.X > 0 ? MapBGMoveLimit.X : 0 );
	MapBGMoveLimit.Y = ( PaperZoneSize.Y - PaperMaskBorder.Y / MapUIZoom ) / 2;
	MapBGMoveLimit.Y = ( MapBGMoveLimit.Y > 0 ? MapBGMoveLimit.Y : 0 );
}

//================================================================================================
FVector2D UMMOUIWorldMap::ConvertPosInMapUIToWorldmapResPos( INT posX, INT posY )
{
	MMOMapImgTiles* pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	FVector2D inMap2DPos;

	FVector2D afterTurnPos2D;
	FLOAT yawReg = CameraYawDeg * PI / 180;
	FLOAT cosMap = appCos( yawReg );
	FLOAT sinMap = appSin( yawReg );
	afterTurnPos2D.X = posX * cosMap + ( -posY * sinMap );
	afterTurnPos2D.Y = posX * sinMap + posY * cosMap;
	//inMap2DPos.X = MapUiCenterPos.X + ( posX - PaperMaskBorder.X / 2 ) / pMapTiles->_ZoomFactor;
	//inMap2DPos.Y = MapUiCenterPos.Y + ( posY - PaperMaskBorder.Y / 2 ) / pMapTiles->_ZoomFactor;
	inMap2DPos.X = MapUiCenterPos.X + afterTurnPos2D.X / pMapTiles->_ZoomFactor;
	inMap2DPos.Y = MapUiCenterPos.Y + afterTurnPos2D.Y / pMapTiles->_ZoomFactor;

	return inMap2DPos;
}
//
FVector UMMOUIWorldMap::ConvertPosInMapUIToWorld3DPos( INT posX, INT posY )
{
	FVector2D inMap2DPos = ConvertPosInMapUIToWorldmapResPos( posX, posY);

	return MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES)->World3DPosByMap2DPos( inMap2DPos );
}

//============================================================================================================
FVector2D UMMOUIWorldMap::ConvertWorld3DPosToPosInMapUI( FVector pos3D )
{
	MMOMapImgTiles* pMapTiles = MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES);
	FVector2D inMap2DPos = pMapTiles->Map2DPosByWorld3DPos( pos3D );

	FVector2D inUI2DPos;
	inUI2DPos.X = ( inMap2DPos.X - MapUiCenterPos.X ) * pMapTiles->_ZoomFactor;// + PaperMaskBorder.X / 2;
	inUI2DPos.Y = ( inMap2DPos.Y - MapUiCenterPos.Y ) * pMapTiles->_ZoomFactor;// + PaperMaskBorder.Y / 2;

	FVector2D afterTurnPos2D;
	FLOAT yawReg = CameraYawDeg * PI / 180;
	FLOAT cosMap = appCos( yawReg );
	FLOAT sinMap = appSin( yawReg );
	afterTurnPos2D.X =  inUI2DPos.X * cosMap + inUI2DPos.Y * sinMap;
	afterTurnPos2D.Y = -inUI2DPos.X * sinMap + inUI2DPos.Y * cosMap;

	return afterTurnPos2D;
}

//
void UMMOUIWorldMap::Init()
{
	Super::Init();
	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}

	InitWorldMapMc();

	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );

	//
	_UnitedMapPanel = ConstructObject<UMMOUIUnitedMap>( UMMOUIUnitedMap::StaticClass(), this );
	if( _UnitedMapPanel != NULL ){
		_UnitedMapPanel->InitByParent( this );
	}

}

//
void UMMOUIWorldMap::InitWorldMapMc()
{
	RootMc = GFXPlayer->GetVariableObject( TEXT("root") );
	
	MapNameTitle_tf = GetObject( TEXT("tF_CurrentMapName") );
	MyPostion_Tf = GetObject( TEXT("Coordtext") );
	MousePostion_Tf = GetObject( TEXT("MouseCoordtext") );

	UGFxObject *mapZoneMc = GetObject( TEXT("mapZone_mc") );
	if( mapZoneMc != NULL ){
		MapNPCView = mapZoneMc->GetObject( TEXT("MapNpcView_mc") );

		//
		MapPathView = mapZoneMc->GetObject( TEXT("MapPathView_mc") );
		if( MapPathView != NULL ){
			MyIcon = MapPathView->GetObject( TEXT("PlayerArrow_mc") );
		}
		
		//
		MapExtraInfoView = mapZoneMc->GetObject( TEXT("MapExtraInfo_mc") );
		IsToShowMapMarksExtraInfo = FALSE;
		ShowMapMarksInfomation( FALSE );

		//
		MapPaper = mapZoneMc->GetObject( TEXT("mapPaper_mc") );
		if( MapPaper != NULL ){
			MapTiles.AddZeroed( 4 );
			for( INT i = 0; i < 4; ++i ){
				FString obName = FString::Printf( TEXT("MapPiece_%d_mc" ), i + 1 );
				MapTiles(i) = MapPaper->GetObject( obName );
			}

			//
			MapBGLoader = MapPaper->GetObject( TEXT("WorldMapBgMc") );
			if( MapBGLoader != NULL ){
				PaperZoneSize.X = MapBGLoader->GetInt( TEXT("width") );
				PaperZoneSize.Y = MapBGLoader->GetInt( TEXT("height") );
			}
		}
	}
	//

	MapUIResScale = ScaledValidMapPhotoSize / ValidMapPhotoSize;
	MapImgOffset.X = ( PaperZoneSize.X - ScaledValidMapPhotoSize ) / 2 / MapUIResScale;
	MapImgOffset.Y = ( PaperZoneSize.Y - ScaledValidMapPhotoSize ) / 2 / MapUIResScale;
	//MapImgOffset.X = ( PaperZoneSize.X - ScaledValidMapPhotoSize ) / 2;
	//MapImgOffset.Y = ( PaperZoneSize.Y - ScaledValidMapPhotoSize ) / 2;

	//
	ThisPos.X = GetInt( TEXT("x") );
	ThisPos.Y = GetInt( TEXT("y") );
	UGFxObject *mapMaskZoneMc = GetObject( TEXT("mapMaskZone_mc") );
	if( mapMaskZoneMc != NULL ){
		MaskZonePos.X = mapMaskZoneMc->GetInt( TEXT("x") );
		MaskZonePos.Y = mapMaskZoneMc->GetInt( TEXT("y") );
		PaperMaskBorder.X = mapMaskZoneMc->GetInt( TEXT("width") );//取出来总是651，但实际是652
		PaperMaskBorder.Y = mapMaskZoneMc->GetInt( TEXT("height") );
	}

	PaperZoneCenterPos.X = MaskZonePos.X + PaperMaskBorder.X / 2;
	PaperZoneCenterPos.Y = MaskZonePos.Y + PaperMaskBorder.Y / 2;

	//
	_MapMarkfilter_tf = GetObject( TEXT("mapMarkfilter_tf") );
	//_MapListMc = GetObject( TEXT("MapList_mc") );
	//_MapTaskListMc = GetObject( TEXT("MapTaskList_mc") );
	_MapMarksListMc = GetObject( TEXT("MapMarksList_mc") );

	//
	_ToggleStaticMarkBtn = GetObject( TEXT("ToggleStaticMarkBtn") );
	_ToggleTaskBtn = GetObject( TEXT("ToggleTaskBtn") );

	//
	_UserMarkPanelMc = GetObject( TEXT("UserMarkPanelMc") );
	if( _UserMarkPanelMc != NULL ){
		_UserMarkNameTf = _UserMarkPanelMc->GetObject( TEXT("UserMarkNameTf") );
		if( _UserMarkNameTf != NULL ){
			_UserMarkNameTf->SetString( TEXT("defaultText"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.WorldMapDefalutUserMarkName") ) );
		}
		
		_AddUserMarkBtn = _UserMarkPanelMc->GetObject( TEXT("AddUserMarkBtn") );
		_DelUserMarkBtn = _UserMarkPanelMc->GetObject( TEXT("DelUserMarkBtn") );

		//
		for( INT i = 0; i < 6; ++i ){
			FString obName = FString::Printf( TEXT("ChooseUserMarkTypeBtn%d" ), i );
			_ChooseUserMarkTypeBtn[i] = _UserMarkPanelMc->GetObject( obName );
		}

		//
		_ShareUserMarkBtn = _UserMarkPanelMc->GetObject( TEXT("ShareUserMarkBtn") );
		_ShareUserMarkList = _UserMarkPanelMc->GetObject( TEXT("ShareUserMarkList") );
	}

}

//==================================================================
void UMMOUIWorldMap::ShowMapTips( UBOOL isShow )
{
	IsTipsShowed = isShow;
}
//==================================================================
UBOOL UMMOUIWorldMap::CreateMapMarkUIObject( FObjInMapUI *pMapMarkData )
{
	if( pMapMarkData == NULL ){
		return FALSE;
	}

	//
	if( pMapMarkData->_UiObjInWorldMap ){
		return TRUE;
	}

	//UGFxObject *newMarkObj = MapNPCView->AttachMovie( TEXT("MapMarkIconAOG") , FString::Printf( TEXT("WorldMapMark%d"), pMarkUI->_ID )  );
	FString uiObjName = FString::Printf( TEXT("WMapMarkIconType%d_%d"), pMapMarkData->_Type, pMapMarkData->_ID );
	UGFxObject *pMarkUiParent = eventCallASGetMapMarkUIObjectContainer( pMapMarkData->_Type );
	if( pMarkUiParent == NULL ){
		return FALSE;
	}

	pMapMarkData->_UiObjInWorldMap = pMarkUiParent->AttachMovie( TEXT("MapMarkIconAOG"), uiObjName );
	if( pMapMarkData->_UiObjInWorldMap == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [Map AttachMovie faile] MapMarkIcon not exit") );
		return FALSE;
	}
	pMapMarkData->_UiObjInWorldMap->AddToRoot();

	//
	//pMapMarkData->_UiObjInWorldMap->SetInt( TEXT("Type"), pMapMarkData->_Type );
	//pMapMarkData->_UiObjInWorldMap->SetInt( TEXT("ID"), pMapMarkData->_ID );
	eventInitMapMark( pMapMarkData->_UiObjInWorldMap, pMapMarkData->_ID, pMapMarkData->_Type, pMapMarkData->_UiIconIdx );
	eventAddMapMarkEventListerner( pMapMarkData->_UiObjInWorldMap );

	return TRUE;
}

//
//UGFxObject* UMMOUIWorldMap::GetMapMarkUIObjectContainer( INT type )
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
//	case EACTMUI_ExtaInfo:
//		return MapExtraInfoView;
//	}
//
//	return MapNPCView;
//}

//==================================================================
void UMMOUIWorldMap::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}

	if( _UnitedMapPanel != NULL ){
		_UnitedMapPanel->UnInit();
		//_UnitedMapPanel->MarkPendingKill();
		_UnitedMapPanel = NULL;
	}

	MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();

	FActorInMapUI *pUIActor = NULL;
	//near
	MMOMapDataManager::MapDynamicActorIterator iterNear = mapDataMgr.BeginIterateDymcActorNear();
	for(  ; iterNear; ++iterNear ){
		pUIActor = &iterNear.Value();
		pUIActor->_NeedUpdateFlag |= EACTUIUF_WorldMap;
		//
		DistoryRootMc( pUIActor->_UiObjInWorldMap );
	}
	//Global
	MMOMapDataManager::MapDynamicActorIterator iterGlobal = mapDataMgr.BeginIterateDymcActorGlobal();
	for(  ; iterGlobal; ++iterGlobal ){
		pUIActor = &iterGlobal.Value();
		pUIActor->_NeedUpdateFlag |= EACTUIUF_WorldMap;

		//
		DistoryRootMc( pUIActor->_UiObjInWorldMap );
	}

	//
	TArray< FActorInMapUI > &listWorldMapUIStaticActors = mapDataMgr.GetCurWorldMapStaticActorPointsList();
	for( INT i = listWorldMapUIStaticActors.Num() - 1; i >= 0; --i ){
		pUIActor = &listWorldMapUIStaticActors( i );
		pUIActor->_NeedUpdateFlag |= EACTUIUF_WorldMap;

		DistoryRootMc( pUIActor->_UiObjInWorldMap );
	}

	//user marks
	FUserMarkInMapUI *pUIUserMark = NULL;
	TArray< FUserMarkInMapUI > &listWorldMapUserMarks = mapDataMgr.GetCurWorldMapMarks();
	for( INT i = listWorldMapUserMarks.Num() - 1; i >= 0; --i ){
		pUIUserMark = &listWorldMapUserMarks( i );
		pUIUserMark->_NeedUpdateFlag |= EACTUIUF_WorldMap;

		DistoryRootMc( pUIUserMark->_UiObjInWorldMap );
	}

	//
	TArray< FPathPointInMapUI > &mapCurPath = mapDataMgr.GetAllMapPathResource();
	UINT posCnt = mapCurPath.Num();
	FPathPointInMapUI *pPathPosUi = NULL;
	for( UINT i = 0; i < posCnt; ++i ){
		pPathPosUi = &mapCurPath(i);
		pPathPosUi->_NeedUpdateFlag |= EACTUIUF_WorldMap;

		DistoryRootMc( pPathPosUi->_UiObjInWorldMap );
	}
}

//
void UMMOUIWorldMap::DistoryRootMc( UGFxObject* &pMc )
{
	if( pMc == NULL ){
		return;
	}
	eventRemoveChild( pMc );
	pMc->RemoveFromRoot();
	pMc = NULL;
}

//
//
//==============================================================================================
//世界地图
//==============================================================================================
void UMMOUIWorldMap::ASClickToWorldMap( INT tileIdx, INT mapID, const FString &regionName )
{
	if( _UnitedMapPanel == NULL ){
		return;
	}
	_UnitedMapPanel->OnClickUnitedMapTile( tileIdx, mapID, regionName );
}

//==============================================================================================
void UMMOUIWorldMap::ASClickToBack( UGFxObject *fromMc, UGFxObject *toBackMc )
{
	//if( _UnitedMap == NULL ){
	//	return;
	//}

	//_UnitedMap->ShowMapTilesIn( fromMc, toBackMc );
}
//======================================================================================
void UMMOUIWorldMap::ASClickToNewWorld( UGFxObject *fromMc, UGFxObject *toNewMc )
{

}

//======================================================================================
void UMMOUIWorldMap::ASShowTipsUnitedMapTile( UGFxObject *fromMc, INT mapID, const FString &regionName, const FString &displayMapName )
{
	if( fromMc == NULL ){
		return;
	}

	FExportMap* mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( mapID );
	if( mapInfo == NULL ){
		return;
	}

	INT myLevel = MMOFacade::getSingleton().GetMyLevel();
	if( ( mapInfo->MinPlayerLevel <= myLevel ) && ( myLevel <= mapInfo->MaxPlayerLevel ) ){
		//只显示在推荐等级之外的
		return;
	}
	//tipsContentKey += mapInfo->MapName;
	//tipsContentKey += TEXT("_");
	//tipsContentKey += regionName;

	//UBOOL isFound = FALSE;
	//FString displayTipContent = MMOUI::ParseLocalizedPropertyPath( tipsContentKey, isFound );
	//if( displayTipContent.Len() <= 0 || !isFound ){
	//	return;
	//}

	FString displayTipContent;
	if( ( mapInfo->MinPlayerLevel == 99 ) && ( mapInfo->MaxPlayerLevel == 99 ) ){
		displayTipContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.UnitedMapBlockTips.Unfinished") );
	}
	else{
		displayTipContent = displayMapName;//mapInfo->MapDisplayName;
		displayTipContent += TEXT("<br>");
		FString tipsContentKey = TEXT("MMOGame.UnitedMapBlockTips.Fomate");
		displayTipContent += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( tipsContentKey ), mapInfo->MinPlayerLevel, mapInfo->MaxPlayerLevel );
	}

	fromMc->SetString( TEXT("tipsHtml"), displayTipContent );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_AutoSizeTips, fromMc );
}

//======================================================================================
void UMMOUIWorldMap::ASHideTipsUnitedMapTile()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//======================================================================================
void UMMOUIWorldMap::ASToggleShowUnitedMapTileLevel( UBOOL isToShow )
{

}

//=================================================================================================
//=================================================================================================
void UMMOUIWorldMap::ASShowWorldMapTabPanel( int type )
{
	EPanelTypeWorldMap worldMapType = (EPanelTypeWorldMap)type;

	switch( worldMapType )
	{
	case ePnlTyp_WorldMap:
		{
			ShowCurSceneMapConent();
		}
		break;
	case ePnlTyp_UnitedMap:
		{
			ShowMapPanelContent( ePnlTyp_UnitedMap );
		}
		break;
	}
}

//=================================================================================================
//大地图
//=================================================================================================
void UMMOUIWorldMap::BeforeViewUpdate(float DeltaTime)
{
	//Super::BeforeViewUpdate(DeltaTime);
	//world map
	if( IsVisible && ( _CurPanelContentType == ePnlTyp_WorldMap ) ){
		UpdateWorldMapContents();
	}
}

//
void UMMOUIWorldMap::ASInputCoodinateAndMoveTo( INT inputX, INT inputY )
{
	INT cvtInputPoxX = inputX * 100;
	INT cvtInputPosY = inputY * 100;
	FVector world3DPos = FVector( cvtInputPoxX, cvtInputPosY, 0 );
	MMOMapDataManager::getSingleton().SetLastInputMoveToPos( world3DPos );

	FExportMap curWorldMapInfo = MMOMapDataManager::getSingleton().GetCurWorldMap();
	UBOOL isSuccess = MMOFacade::getSingleton().PlayerMoveToScenePos( curWorldMapInfo, world3DPos );
	if( isSuccess != TRUE ){
		FString errorName = TEXT("MMOErrorInfo.Navigation.NoPathIvalideCoodinate");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
	}
}

//
void UMMOUIWorldMap::ASMoveToMapPos( INT clickPosX, INT clickPosY )
{
	FVector world3DPos = ConvertPosInMapUIToWorld3DPos( clickPosX, clickPosY );

	FExportMap curWorldMapInfo = MMOMapDataManager::getSingleton().GetCurWorldMap();
	MMOFacade::getSingleton().PlayerMoveToScenePos( curWorldMapInfo, world3DPos );
}


//移动到标记点
void UMMOUIWorldMap::ASMoveToThisMapMark( INT idxInStaticMap )
{
	MMOMapDataManager &mmoMapDataMgr = MMOMapDataManager::getSingleton();
	TArray< FActorInMapUI > &staticMarks = mmoMapDataMgr.GetCurWorldMapStaticActorPointsList();
	if( !staticMarks.IsValidIndex( idxInStaticMap ) ){
		return;
	}

	FActorInMapUI &staticMark = staticMarks( idxInStaticMap );

	//
	UBOOL isSuccess = FALSE;
	INT sceneServerID = mmoMapDataMgr.GetCurWorldMap().ServerID;
	switch( staticMark._Type )
	{
	case EACTMUI_StaticNPC:
		{
			const TArray<FNPCInstanceInfo*> &npcInMapInfoList = *( mmoMapDataMgr.GetNpcInstanceInMap( sceneServerID ) );
			if( npcInMapInfoList.IsValidIndex( staticMark._ActorID ) ){
				const FNPCInstanceInfo *npcInfo = npcInMapInfoList( staticMark._ActorID );
				if( npcInfo!= NULL ){
					//isSuccess = MMONavigationManager::getSingleton().Navigate( sceneServerID, npcInfo->TemplateID, FALSE );
					isSuccess = MMONavigationManager::getSingleton().Navigate( sceneServerID, npcInfo->Position, FALSE );
				}

			}
		}
		break;
	case EACTMUI_NpcGroup:
		{
			const TArray<FNPCGroupInfo> *pNpcGroups = mmoMapDataMgr.GetNpcGroupsInMap( sceneServerID );
			if( pNpcGroups == NULL ){
				break;
			}
			const TArray<FNPCGroupInfo> &npcGroups = *pNpcGroups;

			if( !npcGroups.IsValidIndex( staticMark._ActorID ) ){
				break;
			}
			const FNPCGroupInfo &groupNpcInfo = npcGroups( staticMark._ActorID );

			isSuccess = MMONavigationManager::getSingleton().Navigate( sceneServerID, groupNpcInfo.NpcTemplateID, FALSE );
		}
		break;
	case EACTMUI_NavPoint:
		{
			const TArray<FNaviPointInfo> *pMapNavPoints = mmoMapDataMgr.GetNavPointsInMap( sceneServerID );
			if( pMapNavPoints == NULL ){
				break;
			}
			const TArray<FNaviPointInfo> &mapNavPoints = *pMapNavPoints;

			if( !mapNavPoints.IsValidIndex( staticMark._ActorID ) ){
				break;
			}
			const FNaviPointInfo &navPointInfo = mapNavPoints( staticMark._ActorID );
			if( !navPointInfo.RedirectLoc.IsZero() ){
				isSuccess = MMONavigationManager::getSingleton().Navigate( sceneServerID, navPointInfo.RedirectLoc, FALSE );
			}
			else{
				isSuccess = MMONavigationManager::getSingleton().Navigate( sceneServerID, staticMark._OrigInMapPos, FALSE );
			}
		}
		break;
	case EACTMUI_TeleportFB:
	case EACTMUI_TeleportMap:
	case EACTMUI_TeleportInDoor:
	default:
		isSuccess = MMONavigationManager::getSingleton().Navigate( sceneServerID, staticMark._OrigInMapPos, FALSE );
	}

	//
	if( isSuccess ){
		UINT interval = mmoMapDataMgr.GetCurSceneWorldMapPathInterval();
		TArray<FVector> pathPos = mmoMapDataMgr.GetCurMainPlayerPath( interval, TRUE );

		mmoMapDataMgr.SetMapPath( pathPos );
	}
}

//加注标记点
void UMMOUIWorldMap::ASAddOrRemoveMarkMap( int clickPosX, int clickPosY )
{
	PrepareShowAddUserMarkPanel( clickPosX, clickPosY );
}

//
void UMMOUIWorldMap::ASConfirmAddUserMark()
{
	AddMapMark();
	CloseUserMarkPanel();
}
//
void UMMOUIWorldMap::ASConfirmDelUserMark()
{
	DelMapMark();
	CloseUserMarkPanel();
}
//
void UMMOUIWorldMap::ASCloseUserMarkPanel()
{
	CloseUserMarkPanel();
}
//找寻地图上已有的固定点
void UMMOUIWorldMap::ASFindThisMapMark( int idx )
{
	HighlightMark( idx );
}

//找寻地图上任务点
void UMMOUIWorldMap::ASFindMapTask( int npcIdx )
{
	const TMap< UINT, UINT > &npcInfoInMapRegion = MMOMapDataManager::getSingleton().GetWorldMapPuttedUsefulNpcs();

	const UINT *npcPointIdx = npcInfoInMapRegion.Find( npcIdx );
	if( npcPointIdx == NULL ){
		return;
	}
	//
	HighlightMark( *npcPointIdx );
}

//切换地图
void UMMOUIWorldMap::ASChangeMap( int mapID, int regionInMapIdx )
{
	//
	InitWorldMap( mapID, regionInMapIdx );
	//_WorldMap->UpdateMapTilesPosition();
	ResizeMap();
}

//切换显示功能NPC
void UMMOUIWorldMap::ASToggleFuncNpc( UBOOL isShow )
{
	ToggleShowStaticMarksOf( EACTUIF_FunctionNpc, isShow );
}

//切换显示任务NPC
void UMMOUIWorldMap::ASToggleTaskNpc( UBOOL isShow )
{
	ToggleShowStaticMarksOf( EACTUIF_TaskNpc, isShow );
}

//
void UMMOUIWorldMap::ASShowWorldMapMarkHint( INT type, INT idx )
{
	warnf( TEXT( "ASShowWorldMapMarkHint at Type:%d Idx:%d"), type, idx );

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	//FString displayText;
	FObjInMapUI *pMapMark = NULL;
	FString tipsText;
	switch( type )
	{
	case EACTMUI_StaticNPC:
	case EACTMUI_TeleportFB:
	case EACTMUI_TeleportMap:
	case EACTMUI_TeleportInDoor:
	case EACTMUI_TeleportNode:
	case EACTMUI_QuestZone:
		{
			TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurWorldMapStaticActorPointsList();
			if( staticMarks.IsValidIndex(idx) ){
				pMapMark = &staticMarks(idx);
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;
	case EACTMUI_NpcGroup:
		{
			TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurWorldMapStaticActorPointsList();
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

	case EACTMUI_NearMonster:
	case EACTMUI_NearBoss:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorNearByUiID( idx );
			if( pMapMark != NULL ){
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;

	case EACTMUI_NearPlayer:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorNearByUiID( idx );
			if( pMapMark == NULL ){
				break;
			}

			if( pMapMark->_DisplayName.IsEmpty() == FALSE ){
				break;
			}
			//因为当初加入这个玩家时，他的信息还没到
			INT roleID = ((FActorInMapUI*)pMapMark)->_ActorID;
			AMMOActor *pPlayer = MMOFacade::getSingleton().GetCharacter( roleID );
			if( pPlayer != NULL && pPlayer->GetActorName().IsEmpty() == FALSE ){
				tipsText = pPlayer->GetActorName();
			}
			else
			{
				PlayerBaseInfo *pPlayerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( roleID );
				if( pPlayerInfo != NULL ){
					tipsText = pPlayerInfo->_RoleName;
				}
			}

			//if( pPlayer != NULL && pPlayer->GetActorName().IsEmpty() == FALSE ){
			//	tipsText = pPlayer->GetActorName();
			//}
			//else{
			//	PlayerBaseInfo *pPlayerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( roleID );
			//	if( pPlayerInfo != NULL ){
			//		tipsText = pPlayerInfo->_RoleName;
			//	}
			//}
		}
		break;

	case EACTMUI_UserMark:
		{
			TArray<FUserMarkInMapUI> &marksList = pMapDataMgr->GetCurWorldMapMarks();
			if( marksList.IsValidIndex(idx) ){
				pMapMark = &marksList(idx);
				if( pMapMark != NULL ){
					tipsText = pMapMark->_DisplayName;

					tipsText.ReplaceInline( TEXT("<"), TEXT("&lt;") );
					tipsText.ReplaceInline( TEXT(">"), TEXT("&gt;") );
				}
			}
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
	UGFxObject *pMc = pMapMark->_UiObjInWorldMap;
	if( pMc == NULL ){
		return;
	}

	//
	if( tipsText.IsEmpty() || tipsText.Len()<2){
		return;
	}

	//FString tipsText;
	//if( type == EACTMUI_StaticNPC ){
	//	const UNPCTemplate* npcTemp = MMOFacade::getSingleton().FindNPCTemplate( ( (FActorInMapUI*)pMapMark )->_AidID );
	//	if( npcTemp != NULL ){
	//		tipsText = npcTemp->NameID;
	//	}
	//	else{
	//		tipsText = pMapMark->_DisplayName;
	//	}
	//}
	//else{
	//	tipsText = pMapMark->_DisplayName;
	//}

	pMc->SetString( TEXT("tipsHtml"), tipsText );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, pMapMark->_UiObjInWorldMap );
	ShowMapTips( TRUE );
}

//
void UMMOUIWorldMap::ASHideWorldMapMarkHint()
{
	MMOUI::GetUIManager()->delegateHideTips();

	ShowMapTips( FALSE );
}

//======================================================================================
void UMMOUIWorldMap::ASShowTipsWorldMapMarkListItem( UGFxObject *mc, INT idxInStaticMap, const FString &contentText )
{
	if( mc == NULL ){
		return;
	}

	MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();

	TArray< FActorInMapUI > &staticMarks = mapDataMgr.GetCurWorldMapStaticActorPointsList();
	if( !staticMarks.IsValidIndex( idxInStaticMap ) ){
		return;
	}

	FActorInMapUI &staticMark = staticMarks( idxInStaticMap );

	//
	FString displayTipContent;
	INT sceneServerID = mapDataMgr.GetCurWorldMap().ServerID;
	switch( staticMark._Type )
	{
	case EACTMUI_StaticNPC:
		{
			const UNPCTemplate* npcTemp = MMOFacade::getSingleton().FindNPCTemplate( staticMark._AidID );
			if( npcTemp != NULL ){
				displayTipContent = npcTemp->GetDisplayName();
			}
		}
		break;
	default:
		{
			displayTipContent = contentText;
		}
		break;
	}

	mc->SetString( TEXT("tipsHtml"), displayTipContent );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

//-------------------------------------------------------------------------------------
void UMMOUIWorldMap::ASHideTipsWorldMapMarkListItem( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	MMOUI::GetUIManager()->delegateHideTips();
}
//======================================================================================
void UMMOUIWorldMap::ASChangeShareUserMarkTarget( INT listIdx, INT targetType )
{
	UpdateShareUserMarkButtonState( targetType );
}

//-------------------------------------------------------------------------------------
void UMMOUIWorldMap::ASShareUserMark( INT targetType )
{
	ShareCurUserMarkTo( targetType );
}

//
//======================================================================================
void UMMOUIWorldMap::ASWorldMapZoom( UBOOL ZoomIn )
{
	Zoom( ZoomIn );
}
//---------------------------------------------------------------------------------------
void UMMOUIWorldMap::ASWorldMapZoomTo( INT level )
{
	ZoomTo( level );
}
//======================================================================================
void UMMOUIWorldMap::ASMapDrag( FLOAT deltaX, FLOAT deltaY )
{
	MoveMapByDelta( deltaX, deltaY );
}
//======================================================================================
//======================================================================================
