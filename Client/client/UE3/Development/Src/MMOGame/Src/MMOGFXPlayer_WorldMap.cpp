#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"
#include "MMONavigationManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_WorldMap)


//==============================================================================================
//World map
//==============================================================================================
void UMMOGFXPlayer_WorldMap::ASClickToWorldMap( INT tileIdx, INT mapID, const FString &regionName )
{
	if( _WorldMap == NULL || _WorldMap->_UnitedMapPanel == NULL ){
		return;
	}

	_WorldMap->_UnitedMapPanel->OnClickUnitedMapTile( tileIdx, mapID, regionName );
}

//==============================================================================================
void UMMOGFXPlayer_WorldMap::ASClickToBack( UGFxObject *fromMc, UGFxObject *toBackMc )
{
	//if( _UnitedMap == NULL ){
	//	return;
	//}

	//_UnitedMap->ShowMapTilesIn( fromMc, toBackMc );
}
//======================================================================================
void UMMOGFXPlayer_WorldMap::ASClickToNewWorld( UGFxObject *fromMc, UGFxObject *toNewMc )
{

}

//======================================================================================
void UMMOGFXPlayer_WorldMap::ASShowTipsUnitedMapTile( UGFxObject *fromMc, INT mapID, const FString &regionName, const FString &displayMapName )
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
		//only show those outside the recommended level
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
void UMMOGFXPlayer_WorldMap::ASHideTipsUnitedMapTile()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//======================================================================================
void UMMOGFXPlayer_WorldMap::ASToggleShowUnitedMapTileLevel( UBOOL isToShow )
{

}

//=================================================================================================
//=================================================================================================
void UMMOGFXPlayer_WorldMap::ASShowWorldMapTabPanel( int type )
{
	if( _WorldMap == NULL ){
		return;
	}

	//
	EPanelTypeWorldMap worldMapType = (EPanelTypeWorldMap)type;

	switch( worldMapType )
	{
	case ePnlTyp_WorldMap:
		{
			_WorldMap->ShowCurSceneMapConent();
		}
		break;
	case ePnlTyp_UnitedMap:
		{
			_WorldMap->ShowMapPanelContent( ePnlTyp_UnitedMap );
		}
		break;
	}
}

//=================================================================================================
//big map
//=================================================================================================
void UMMOGFXPlayer_WorldMap::BeforeViewUpdate(float DeltaTime)
{
	//Super::PostAdvance(DeltaTime);
	//world map
	if( _WorldMap && _WorldMap->IsVisible && ( _WorldMap->_CurPanelContentType == ePnlTyp_WorldMap ) ){
		_WorldMap->UpdateWorldMapContents();
	}
}

//
void UMMOGFXPlayer_WorldMap::ASInputCoodinateAndMoveTo( INT inputX, INT inputY )
{
	INT cvtInputPoxX = inputX * 100;
	INT cvtInputPosY = inputY * 100;
	FVector world3DPos = FVector( cvtInputPoxX, cvtInputPosY, 0 );
	MMOMapDataManager::getSingleton().SetLastInputMoveToPos( world3DPos );

	FExportMap curWorldMapInfo = MMOMapDataManager::getSingleton().GetCurWorldMap();
	MMOFacade::getSingleton().PlayerMoveToScenePos( curWorldMapInfo, world3DPos );
}

//
void UMMOGFXPlayer_WorldMap::ASMoveToMapPos( INT clickPosX, INT clickPosY )
{
	FVector world3DPos = _WorldMap->ConvertPosInMapUIToWorld3DPos( clickPosX, clickPosY );
	
	FExportMap curWorldMapInfo = MMOMapDataManager::getSingleton().GetCurWorldMap();
	MMOFacade::getSingleton().PlayerMoveToScenePos( curWorldMapInfo, world3DPos );
}

//move to marker point
void UMMOGFXPlayer_WorldMap::ASMoveToThisMapMark( INT idxInStaticMap )
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
					isSuccess = MMONavigationManager::getSingleton().Navigate( sceneServerID, npcInfo->TemplateID, FALSE );
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

		//if( pathPos.Num() <= 0 ){
		//	pMapDataMgr->InvalidCurPath();
		//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOErrorInfo.Navigation.NoPath") ) ,MsgInfoType_Table,TEXT("MMOErrorInfo.Navigation.NoPath") );	
		//	return;
		//}
		mmoMapDataMgr.SetMapPath( pathPos );
	}
}
//Add marker point
void UMMOGFXPlayer_WorldMap::ASAddOrRemoveMarkMap( int clickPosX, int clickPosY )
{
	_WorldMap->PrepareShowAddUserMarkPanel( clickPosX, clickPosY );
}

//
void UMMOGFXPlayer_WorldMap::ASConfirmAddUserMark()
{
	_WorldMap->AddMapMark();
	_WorldMap->CloseUserMarkPanel();
}
//
void UMMOGFXPlayer_WorldMap::ASConfirmDelUserMark()
{
	_WorldMap->DelMapMark();
	_WorldMap->CloseUserMarkPanel();
}
//
void UMMOGFXPlayer_WorldMap::ASCloseUserMarkPanel()
{
	_WorldMap->CloseUserMarkPanel();
}
//find existing fixed points on the map
void UMMOGFXPlayer_WorldMap::ASFindThisMapMark( int idx )
{
	_WorldMap->HighlightMark( idx );
}

//find quest points on the map
void UMMOGFXPlayer_WorldMap::ASFindMapTask( int npcIdx )
{
	const TMap< UINT, UINT > &npcInfoInMapRegion = MMOMapDataManager::getSingleton().GetWorldMapPuttedUsefulNpcs();

	const UINT *npcPointIdx = npcInfoInMapRegion.Find( npcIdx );
	if( npcPointIdx == NULL ){
		return;
	}
	//
	_WorldMap->HighlightMark( *npcPointIdx );
}

//Switch map
void UMMOGFXPlayer_WorldMap::ASChangeMap( int mapID, int regionInMapIdx )
{
	//
	_WorldMap->InitWorldMap( mapID, regionInMapIdx );
	//_WorldMap->UpdateMapTilesPosition();
	_WorldMap->ResizeMap();
}

//Toggle showing function NPCs
void UMMOGFXPlayer_WorldMap::ASToggleFuncNpc( UBOOL isShow )
{
	_WorldMap->ToggleShowStaticMarksOf( EACTUIF_FunctionNpc, isShow );
}

//Toggle showing quest NPCs
void UMMOGFXPlayer_WorldMap::ASToggleTaskNpc( UBOOL isShow )
{
	_WorldMap->ToggleShowStaticMarksOf( EACTUIF_TaskNpc, isShow );
}

//
void UMMOGFXPlayer_WorldMap::ASShowWorldMapMarkHint( INT type, INT idx )
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
			//because when this player was added, his info had not arrived yet
			INT roleID = ((FActorInMapUI*)pMapMark)->_ActorID;
			AMMOActor *pPlayer = MMOFacade::getSingleton().GetCharacter( roleID );
			if( pPlayer != NULL && pPlayer->GetActorName().IsEmpty() == FALSE ){
				tipsText = pPlayer->GetActorName();
			}
			else{
				PlayerBaseInfo *pPlayerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( roleID );
				if( pPlayerInfo != NULL ){
					tipsText = pPlayerInfo->_RoleName;
				}
			}
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
	if( tipsText.IsEmpty() ){
		return;
	}

	if( _WorldMap != NULL ){
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
		_WorldMap->ShowMapTips( TRUE );
	}
}

//
void UMMOGFXPlayer_WorldMap::ASHideWorldMapMarkHint()
{
	MMOUI::GetUIManager()->delegateHideTips();

	if( _WorldMap != NULL ){
		_WorldMap->ShowMapTips( FALSE );
	}
}

//======================================================================================
void UMMOGFXPlayer_WorldMap::ASShowTipsWorldMapMarkListItem( UGFxObject *mc, INT idxInStaticMap, const FString &contentText )
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
void UMMOGFXPlayer_WorldMap::ASHideTipsWorldMapMarkListItem( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	MMOUI::GetUIManager()->delegateHideTips();
}
//======================================================================================
void UMMOGFXPlayer_WorldMap::ASChangeShareUserMarkTarget( INT listIdx, INT targetType )
{
	if( _WorldMap == NULL ){
		return;
	}

	//
	_WorldMap->UpdateShareUserMarkButtonState( targetType );
}

//-------------------------------------------------------------------------------------
void UMMOGFXPlayer_WorldMap::ASShareUserMark( INT targetType )
{
	if( _WorldMap == NULL ){
		return;
	}

	//
	_WorldMap->ShareCurUserMarkTo( targetType );
}

//
//======================================================================================
void UMMOGFXPlayer_WorldMap::ASWorldMapZoom( UBOOL ZoomIn )
{
	if( _WorldMap == NULL ){
		return;
	}

	_WorldMap->Zoom( ZoomIn );
}
//---------------------------------------------------------------------------------------
void UMMOGFXPlayer_WorldMap::ASWorldMapZoomTo( INT level )
{
	if( _WorldMap == NULL ){
		return;
	}

	_WorldMap->ZoomTo( level );
}
//======================================================================================
void UMMOGFXPlayer_WorldMap::ASMapDrag( FLOAT deltaX, FLOAT deltaY )
{
	if( _WorldMap == NULL ){
		return;
	}

	_WorldMap->MoveMapByDelta( deltaX, deltaY );
}
//======================================================================================
void UMMOGFXPlayer_WorldMap::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_WorldMap::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_WorldMap);
}

void UMMOGFXPlayer_WorldMap::UnInit()
{
	Super::UnInit();

	if( _WorldMap != NULL )
	{
		_WorldMap->ClearWorldMapUI();
	}
}