#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOMapDataManager.h"
#include "MMOAchievementData.h"

IMPLEMENT_CLASS(UMMOUIUnitedMap)

//================================================================================================
void UMMOUIUnitedMap::PreOnShow(UBOOL show)
{
	//Super::PreOnShow( show );

	//if( IsInitialized == FALSE ){
	//	return;
	//}

	//
	UnitedMapOnShow( show );
}

//--------------------------------------------------------------------------------------------------
void UMMOUIUnitedMap::UnitedMapOnShow( UBOOL isShow )
{
	if( isShow ){
		ShowCurMap();
	}
	else{
		ResetLastMapTile();
	}
}
//=============================================================================================================================
void UMMOUIUnitedMap::ShowCurMap()
{
	const FInMapRegion *curRegion = MMOMapDataManager::getSingleton().GetCurReginInCurMap();
	if( curRegion == NULL ){
		return;
	}
	//
	ResetLastMapTile();
	//
	eventShowMapTile( curRegion->MapID, curRegion->VolumeName, TRUE );

	//
	INT myLevel = MMOFacade::getSingleton().GetMyLevel();

	FString tipsContentKey = TEXT("MMOGame.UnitedMapBlock.LevelLimit");
	UGFxObject *tileMapDataArray = GFXPlayer->CreateArray();
	//MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();
	UGlobalMapInfo *globalMapInfo = UGlobalMapInfo::GetSingleton();
	MMOAchievementData &achiDataMgr = MMOAchievementData::getSingleton();
	if( tileMapDataArray != NULL ){
		INT addSceneCnt = 0;
		INT sceneCnt = MapTileData.Num();
		for( INT i = 0; i < sceneCnt; ++i ){
			const FUnitedMapTileData &mapTileInfo = MapTileData( i );

			UGFxObject* listObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapItemData") );
			if( listObj == NULL ){
				continue;
			}

			FExportMap mapInfo = globalMapInfo->FindMapInfo( mapTileInfo.SceneID );
			listObj->SetInt( TEXT("id"), mapTileInfo.SceneID );
			if( ( mapInfo.MinPlayerLevel == 99 ) && ( mapInfo.MaxPlayerLevel == 99 ) ){
				listObj->SetString( TEXT("level"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.UnitedMapBlockTips.Unfinished") ) );
			}
			else{
				listObj->SetString( TEXT("level"), FString::Printf( *MMOUI::ParseLocalizedPropertyPath( tipsContentKey ), mapInfo.MinPlayerLevel, mapInfo.MaxPlayerLevel ) );
			}

			UBOOL isToGray = TRUE;
			if( mapTileInfo.AchievementID == MapUnachieveID ){
				isToGray = TRUE;
			}
			else if( mapTileInfo.AchievementID == MapDefaultAchieveID ){
				isToGray = FALSE;
			}
			else{
				isToGray = !achiDataMgr.IsAchievementObtain( mapTileInfo.AchievementID );
			}

			listObj->SetBool( TEXT("isGray"), isToGray );

			tileMapDataArray->SetElementObject( addSceneCnt++, listObj );
		}

		eventCallASGrapUnitedMapTile( tileMapDataArray );
	}

	////
	//INT myLevel = MMOFacade::getSingleton().GetMyLevel();
	//FString tipsContentKey = TEXT("MMOGame.UnitedMapBlock.LevelLimit");
	//UGFxObject *tileMapDataArray = GFXPlayer->CreateArray();
	//if( tileMapDataArray != NULL ){
	//	INT addSceneCnt = 0;
	//	const TArray<FExportMap> &allSceneInfo = UGlobalMapInfo::GetSingleton()->GetAllScenesInfo();
	//	INT sceneCnt = allSceneInfo.Num();
	//	for( INT i = 0; i < sceneCnt; ++i ){
	//		const FExportMap &mapInfo = allSceneInfo( i );

	//		UGFxObject* listObj = GFXPlayer->CreateObject( TEXT("UI_WorldMap.WorldMapAS3.MapItemData") );
	//		if( listObj == NULL ){
	//			continue;
	//		}

	//		listObj->SetInt( TEXT("id"), mapInfo.ServerID );
	//		if( ( mapInfo.MinPlayerLevel == 99 ) && ( mapInfo.MaxPlayerLevel == 99 ) ){
	//			listObj->SetString( TEXT("level"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.UnitedMapBlockTips.Unfinished") ) );
	//		}
	//		else{
	//			listObj->SetString( TEXT("level"), FString::Printf( *MMOUI::ParseLocalizedPropertyPath( tipsContentKey ), mapInfo.MinPlayerLevel, mapInfo.MaxPlayerLevel ) );
	//		}

	//		UBOOL isToGray = ( mapInfo.MinPlayerLevel <= myLevel );
	//		listObj->SetBool( TEXT("isGray"), isToGray );

	//		tileMapDataArray->SetElementObject( addSceneCnt++, listObj );
	//	}

	//	eventCallASGrapUnitedMapTile( tileMapDataArray );
	//}
}
//===================================================================================================================================
void UMMOUIUnitedMap::ResetLastMapTile()
{
	if( LastMapID <= 0 ){
		return;
	}

	eventHighlightMapTile( LastMapID, LastMapVolumeName, FALSE );

	//
	LastMapID = -1;
	LastMapVolumeName = TEXT("");
}
//===================================================================================================================================
void UMMOUIUnitedMap::OnClickUnitedMapTile( INT tileIdx, INT mapID, const FString &regionName )
{
	//if( mapID <= 0 && TilesLinkGraph.Find( tileIdx ) ){
	//	UGFxObject *pNextContainer = TilesLinkGraph.FindRef( tileIdx );
	//	if( pNextContainer != NULL ){
	//		ShowMapTilesIn( pNextContainer );
	//	}
	//	return;
	//}

	//
	UMMOUIWorldMap *worldMapPanel = (UMMOUIWorldMap*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_WorldMap );
	if( worldMapPanel != NULL ){
		INT regionInMapIdx = MMOMapDataManager::getSingleton().GetReginInMapIdx( mapID, regionName );
		if( regionInMapIdx >= 0 ){
			if( worldMapPanel->IsVisible ){
				worldMapPanel->ShowWorldMapInUnitedMapContent( mapID, regionInMapIdx );
			}
			else{
				worldMapPanel->Show( TRUE );//this opens the current map by default
				worldMapPanel->ShowWorldMapInUnitedMapContent( mapID, regionInMapIdx );
			}
		}
	}

	//
	//if( _ContainerPanel != NULL ){

	//}
	//else{
	//	Show( FALSE );
	//}
}
	
//
void UMMOUIUnitedMap::ShowMapTilesIn( UGFxObject *fromContainer, UGFxObject *toContainer )
{
	if( fromContainer == NULL || toContainer == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUIUnitedMap::ShowMapTilesIn has NULL tilesContainer]") );
		return;
	}
	//
	fromContainer->SetVisible( FALSE );

	toContainer->SetVisible( TRUE );

	////Hida All
	//INT allContCnt = MapTileContainers.Num();
	//for( INT i = 0; i < allContCnt; ++i ){
	//	UGFxObject *pContainer = MapTileContainers( i );
	//	if( pContainer == NULL ){
	//		continue;
	//	}
	//	pContainer->SetVisible( FALSE );
	//}
	////Show this
	//tilesContainer->SetVisible( TRUE );
}
//===================================================================================================================================
//void UMMOUIUnitedMap::Init()
//{
//	Super::Init();
//
//	TArray<struct FASValue> asFunArgs;
//	FASValue asTitleName(EC_EventParm);
//	asTitleName.Type = AS_String;
//	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.UnitedMap.UITitleName") );
//	asFunArgs.AddItem( asTitleName );
//	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );
//
//	InitMc();
//}

//------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIUnitedMap::InitByParent( UMMOUIBase *parentPanel )
{
	if( parentPanel == NULL ){
		return;
	}

	_ContainerPanel = parentPanel;
	GFXPlayer = _ContainerPanel->GFXPlayer;
	ctrl = _ContainerPanel->ctrl;
	Player = _ContainerPanel->Player;

	InitMc();
}

//------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIUnitedMap::UnInit()
{
	_ContainerPanel = NULL;
	GFXPlayer = NULL;
	ctrl = NULL;
	Player = NULL;
}
//===================================================================================================================================
//
void UMMOUIUnitedMap::InitMc()
{
	//RootMc = GFXPlayer->GetVariableObject( TEXT("root") );
	InitAllTiles();
}

//
void UMMOUIUnitedMap::InitAllTiles()
{
	////tiles container
	//UGFxObject *pMapTilesContainer = NULL;
	////main
	//pMapTilesContainer = GetObject( TEXT("TilesContainerMc1") );
	//MapTileContainers.AddItem( pMapTilesContainer );

	////
	//MapTiles;

	////other
	//pMapTilesContainer = GetObject( TEXT("TilesContainerMc2") );
	//MapTileContainers.AddItem( pMapTilesContainer );

	MapTileData.Empty();
	TArray<UGFxObject*> getDataArray = eventCallASGetAllUnitedMapTileData();
	INT cnt = getDataArray.Num();
	MapTileData.AddZeroed( cnt );
	for( INT i = 0; i < cnt; ++i ){
		UGFxObject *tileData = getDataArray( i );
		if( tileData == NULL ){
			continue;
		}

		FUnitedMapTileData &newData = MapTileData( i );
		newData.SceneID = tileData->GetInt( TEXT("MapID") );
		newData.RegionName = tileData->GetString( TEXT("MapRegionName") );
		newData.AchievementID = tileData->GetInt( TEXT("AchieveID") );
	}
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIUnitedMap::ASClickToWorldMap( INT tileIdx, INT mapID, const FString &regionName )
{
	OnClickUnitedMapTile( tileIdx, mapID, regionName );
}

//==============================================================================================
void UMMOUIUnitedMap::ASClickToBack( UGFxObject *fromMc, UGFxObject *toBackMc )
{
	ShowMapTilesIn( fromMc, toBackMc );
}
//======================================================================================
void UMMOUIUnitedMap::ASClickToNewWorld( UGFxObject *fromMc, UGFxObject *toNewMc )
{

}
//===============================================================================================================
//===============================================================================================================