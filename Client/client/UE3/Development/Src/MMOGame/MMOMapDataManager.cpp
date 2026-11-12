#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOMapDataManager.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "MMOAssistManager.h"
#include "MMOGuildManager.h"
#include "MMONavigationManager.h"
#include "MMOMapImgTiles.h"
#include "TeamManager.h"
#include "UnPath.h"

using namespace ExternalMessage;

#define IsNpcHasFun(typeInt,FuncType) (((typeInt)&(1<<(INT)(FuncType)))>0)


FObjInMapUI::FObjInMapUI():
_ID(0xffffffff),
_Type(0),
_UiObjInWorldMap(NULL),
_UiObjInMiniMap(NULL),
_UiObjInPathMap(NULL),
_IsValid(FALSE),
_NeedUpdateFlag(EACTUIUF_NoUpdate)
{

}

FUserMarkInMapUI::FUserMarkInMapUI():
FObjInMapUI(),
_RelatedMarkID(-1)
{

}

FMapMark::FMapMark():
_MarkID(-1),
_SceneID(-1),
_isValid(FALSE),
_MarkType(0)
{

}

FActorInMapUI::FActorInMapUI():
FObjInMapUI(),
_ActorID(-1),
_AidID(-1),
_InMapID(-1),
_ToggleFlag(0)
{

}

//FActorGroupInMapUI::FActorGroupInMapUI():
//FObjInMapUI(),
//_LookupIdx(-1),
//_GroupType(-1),
//_ActorTemplateID(-1),
//_InMapID(-1),
//_ToggleFlag(0)
//{
//
//}

MMOMapDataManager::MMOMapDataManager(void)
	:_IsTeleportInited(FALSE),
	_IsTeleportUpdatedByServer(FALSE),
	_LastOpenedTeleportNodeID(-1),
	//_CurFBDiff(ENUM_SCENE_DIFFICULTY::esdNormal1),
	_InCurMapRegionIdx(-1),
	_InCurMapRegion(NULL),
	_InWorldmapRegionIdx(-1),
	_InWorldmapRegion(NULL),
	_LastInputPos( 0, 0, 0 ),
	_DungeonStartUtcTime(0)
{
	//_CurFBMaxDiff = ( 1 << ENUM_SCENE_DIFFICULTY::esdNormal1 ) | ( 1 << ENUM_SCENE_DIFFICULTY::esdHeroic1 );
	//_CurFBMaxDiff >>= ENUM_SCENE_DIFFICULTY::esdStatic;

	_CurSceneLineIn = 0;
	_CurFBFrequence = 0;
}
MMOMapDataManager::~MMOMapDataManager(void)
{

}

//
void MMOMapDataManager::Initialize()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOMapDataManager::Initialize"), 0);

	InitTeleportClientData();
	InitAllMapRegionData();

	//
	//LoadDungonInfo();
	InitFBConstAchievements();
	//
	MMOUI::RegisterEventHandler( this, GameEvent_CharacterDie );
	MMOUI::RegisterEventHandler( this, GameEvent_UpdateMovePath );
	MMOUI::RegisterEventHandler( this, GameEvent_QuestStateChanged );
	MMOUI::RegisterEventHandler( this, GameEvent_QuestModeChange );
	MMOUI::RegisterEventHandler( this, GameEvent_GuildSabacStateChanged );
	//MMOUI::RegisterEventHandler( this, GameEvent_SceneFinished );
	MMOUI::RegisterEventHandler( this, GameEvent_TeamMemberUpdate );
	MMOUI::RegisterEventHandler( this, GameEvent_TeamRemoveMember );
	MMOUI::RegisterEventHandler( this, GameEvent_TeamJoin );
	MMOUI::RegisterEventHandler( this, GameEvent_TeamLeave);
	//MMOUI::RegisterEventHandler( this, GameEvent_SelfCompatStateChange );
	MMOUI::RegisterEventHandler( this, GameEvent_ServerError );
	MMOUI::RegisterEventHandler( this, GameEvent_ActiveRevivePnt );
	MMOUI::RegisterEventHandler( this, GameEvent_StopNavigate );
	MMOUI::RegisterEventHandler( this, GameEvent_EnterScene );
	MMOUI::RegisterEventHandler( this, GameEvent_LeaveServerScene );
	MMOUI::RegisterEventHandler( this, GameEvent_ActorTypeChanged ); 
	MMOUI::RegisterEventHandler( this, GameEvent_GuildOccupy );
}

//=======================================================================================
void MMOMapDataManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_EnterScene:
		{
			MMOGameEvent_Any *enterSceneEvt = (MMOGameEvent_Any*)evt;
			MMOUI::GetUIManager()->ShowHintSceneWidget();// delegateShowHintSceneWidget();

			//
			FExportMap &curMapData = GetCurMap();
			if( curMapData.ServerMapType == E_est_Dungeon ){
				//dungeon rating point initialization
				INT dungeonDifficulty = GetCurFBDiff();
				PrepareCurFBAchievementsScore( curMapData.ServerID, dungeonDifficulty );
			}
		}
		break;
	case GameEvent_LeaveServerScene:
		{
			//clear path
			//InvalidCurPath();
			TArray<FVector> pathPos;
			pathPos.Empty();
			SetMapPath( pathPos );
		}
		break;
	case GameEvent_CharacterDie:
		{
			MMOGameEvent_CharacterDie* infoEvt = (MMOGameEvent_CharacterDie*)evt;
			if(infoEvt == NULL || infoEvt->DiedActor == NULL ){
				break;
			}

			//
			INT actorID = infoEvt->DiedActor->ServerID;
			RemoveMapDymcActorNear( actorID );

			////
			//UMMOUIMinimap *pMiniMapPanel = ( UMMOUIMinimap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
			//if( pMiniMapPanel != NULL ){
			//	pMiniMapPanel->NotifyActorDead( actorID );
			//}
			//
			////
			//UMMOUIWorldMap *pWorldMapPanel = ( UMMOUIWorldMap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WorldMap );
			//if( pWorldMapPanel != NULL ){
			//	pWorldMapPanel->NotifyActorDead( actorID );
			//}
		}
		break;
	case GameEvent_UpdateMovePath:
		{
			AMMOActor *mainActor = MMOFacade::getSingleton().GetMainActor();
			if( mainActor != NULL ){
				//recompute the current area, Tick's update may not have arrived yet, still old data
				INT regionIdx = GetCurPlayerRegionIn( mainActor->Location );
			}

			UINT interval = GetCurSceneWorldMapPathInterval();
			TArray<FVector> pathPos = GetCurMainPlayerPath( interval, TRUE );
			SetMapPath( pathPos );
		}
		break;
	case GameEvent_StopNavigate:
		{
			TArray<FVector> pathPos;
			pathPos.Empty();
			SetMapPath( pathPos );
		}
		break;

	case GameEvent_QuestStateChanged:
		{
			// minimap
			PrepareCurMapStaticActorInMapUI();
			// world map
			PrepareWorldMapStaticActorInMapUI();
			UMMOUIWorldMap *pWorldMapPanel = ( UMMOUIWorldMap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WorldMap );
			if( pWorldMapPanel != NULL && pWorldMapPanel->IsVisible ){
				pWorldMapPanel->UpdateMapStaticMarkList();
				pWorldMapPanel->UpdateMapTaskList();
			}
		}
		break;

	case GameEvent_GuildSabacStateChanged:
		{
			MMOGameEvent_OneInt *sabacStateChanged = (MMOGameEvent_OneInt*)evt;
			if( sabacStateChanged == NULL ){
				break;
			}
			INT playerServerID = sabacStateChanged->IntData;
			FActorInMapUI *pNearPlayerMapIcon = GetMapDymcActorNear( playerServerID );
			if( pNearPlayerMapIcon == NULL ){
				break;
			}

			AMMOPlayer *pPlayer = Cast<AMMOPlayer>( MMOFacade::getSingleton().GetCharacter( playerServerID ) );
			if( pPlayer == NULL ){
				warnf( NAME_Error, TEXT( "!!!GameEvent_GuildSabacStateChanged player[%d] NOT Found"), playerServerID );
				break;
			}

			ChangeMapNearPlayerMarkDisplayIcon( pNearPlayerMapIcon, *pPlayer );
			pNearPlayerMapIcon->_NeedUpdateFlag = EACTUIUF_UpdateAll;

			//FActorInMapUI *GetMapDymcActorGlobal( playerServerID );
		}
		break;
	case GameEvent_TeamRemoveMember:
		{
			MMOFacade &mmoFacade = MMOFacade::getSingleton();
			TArray<FActorInMapUI*> actorArray;
			FindMapDymcActorGlobalOfType( EACTMUI_GlobalTeammate, actorArray );
			//
			INT cnt = actorArray.Num();
			for( INT i = 0; i < cnt; ++i ){
				FActorInMapUI *pActor = actorArray(i);
				MMOTeamMember* member = TeamManager::getSingleton().GetMemberById( pActor->_ActorID );
				if( member == NULL ){
					//already left the team
					RemoveMapDymcActorGlobal( pActor->_ActorID );
				}
			}
		}
		break;
	case GameEvent_TeamLeave:
		{
			MMOFacade &mmoFacade = MMOFacade::getSingleton();
			TArray<FActorInMapUI*> actorArray;
			FindMapDymcActorGlobalOfType( EACTMUI_GlobalTeammate, actorArray );
			//
			INT cnt = actorArray.Num();
			for( INT i = 0; i < cnt; ++i ){
				FActorInMapUI *pActor = actorArray(i);
				AMMOActor *actor = mmoFacade.GetCharacter( pActor->_ActorID );
				if( actor != NULL ){
					AddOrUpdateMapDymcActorNear( *actor );
				}
			}
			
			RemoveAllMapDymcActorGlobalOfType( EACTMUI_GlobalTeammate );
		}
		break;
	case GameEvent_TeamJoin:
		{
			AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if( mainPlayer == NULL ){
				break;
			}
			INT myActorServerID = mainPlayer->ServerID;

			TArray<MMOTeamMember> &allTeammateAndI = TeamManager::getSingleton().GetTeamMemberList();
			//
			INT cnt = allTeammateAndI.Num();
			for( INT i = 0; i < cnt; ++i ){
				const MMOTeamMember &inTeamActor = allTeammateAndI(i);
				if( inTeamActor.ActorServerId != myActorServerID ){
					if( inTeamActor.Online ){
						//add teammate map data
						AddOrUpdateMapDymcActorGlobal( inTeamActor.ActorServerId, EACTMUI_GlobalTeammate, inTeamActor.MapID/*no data yet*/, inTeamActor.ActorName );
					}
				}
			}
		}
		break;
	case GameEvent_TeamMemberUpdate:
		{
			MMOGameEvent_OneInt *teamMemberUpdateEvt = (MMOGameEvent_OneInt*)evt;
			if( teamMemberUpdateEvt == NULL ){
				break;
			}
			AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if( mainPlayer == NULL ){
				break;
			}

			INT actorServerID = teamMemberUpdateEvt->IntData;
			if( mainPlayer->ServerID == actorServerID ){
				//do not update self
				break;
			}

			MMOTeamMember* teamMember = TeamManager::getSingleton().GetMemberById( actorServerID );
			if( teamMember != NULL ){
				if( teamMember->Online ){
					AddOrUpdateMapDymcActorGlobal( teamMember->ActorServerId, EACTMUI_GlobalTeammate, teamMember->MapID, teamMember->ActorName );
				}
				else{
					//already offline
					RemoveMapDymcActorGlobal( teamMember->ActorServerId );
				}
			}
			else{
				RemoveMapDymcActorGlobal( actorServerID );
			}
		}
		break;
	case GameEvent_ServerError:
		{
			MMOGameEvent_ServerError *errorEvt = (MMOGameEvent_ServerError*)evt;
			if( errorEvt != NULL && errorEvt->ErrCode == errJumpChangeLineBreak ){
				UMMOUIMinimap *pMinimap = (UMMOUIMinimap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
				if( pMinimap != NULL ){
					pMinimap->TryInterruptChangingSceneLine();
				}
			}
		}
		break;
	case GameEvent_SelfCompatStateChange:
		{
			MMOGameEvent_OneInt *selfCompatStateEvt = (MMOGameEvent_OneInt*)evt;
			UBOOL isToFightting = selfCompatStateEvt->IntData;
			if( isToFightting ){
				UMMOUIMinimap *pMinimap = (UMMOUIMinimap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
				if( pMinimap != NULL ){
					pMinimap->TryInterruptChangingSceneLine();
				}
			}

		}
		break;
	case GameEvent_ActiveRevivePnt:
		{
			MMOGameEvent_ActiveRevivePnt *activeEvt = (MMOGameEvent_ActiveRevivePnt*)evt;
			ActiveRevivePoint( activeEvt->_ActivePos );
		}
		break;
	case GameEvent_ActorTypeChanged:
		{
			MMOGameEvent_OneInt *actTypeChangedEvt = (MMOGameEvent_OneInt*)evt;

			AMMOActor *actor = MMOFacade::getSingleton().GetCharacter( actTypeChangedEvt->IntData );
			if( actor != NULL ){
				FActorInMapUI *pActorInMapUI = _MapNearActors.Find( actTypeChangedEvt->IntData );
				if( pActorInMapUI != NULL ){
					AddOrUpdateMapDymcActorNear( *actor );
				}
			}
		}
		break;
	case GameEvent_GuildOccupy:
		{
			UMMOUIWorldMap *pWorldMapPanel = (UMMOUIWorldMap*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_WorldMap);
			if (pWorldMapPanel != NULL && pWorldMapPanel->IsVisible)
			{
				FExportMap& curMapData = GetCurMap();
				pWorldMapPanel->UpdateGuildOccupyInfo(curMapData.ServerID);
			}
		}
		break;
	}
}

//===================================================================================================
void MMOMapDataManager::ParseServerMapLine(UINT serverMapLine, UINT &outServerID, UINT &outLineID)
{
	outServerID = serverMapLine>>16;
	outLineID = serverMapLine & 0x0000ffff;
}

//---------------------------------------------------------------------------------------------------
void MMOMapDataManager::AddNewCurMapLine( INT lineID, INT percentState )
{
	MapLineInfo newLineInfo;
	newLineInfo._SceneLineCombineID = lineID;
	newLineInfo._PercentState = percentState;

	_CurSceneLines.Push( newLineInfo );
}

//---------------------------------------------------------------------------------------------------
void MMOMapDataManager::RemoveOldCurMapLine( INT lineID )
{
	INT linesCnt = _CurSceneLines.Num();
	for( INT i = 0; i < linesCnt; ++i ){
		if( _CurSceneLines( i )._SceneLineCombineID == lineID ){
			_CurSceneLines.Remove( i );
			break;
		}
	}
}


//===================================================================================================
void MMOMapDataManager::InitTeleportClientData()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOMapDataManager::InitTeleportClientData"), 0);

	TArray<FTeleportNodeObj*> pNodesList;
	for( INT i = 0; i < EWR_MAX; ++i  ){
		pNodesList.Empty();
		UGlobalMapInfo::GetSingleton()->FindTeleportNetNodesInRegion( &pNodesList, i );

		INT cnt = pNodesList.Num();
		for( INT j = 0; j < cnt; ++j  ){
			FTeleportNetNodeData data;
			data._NodeTemplate = pNodesList(j);
			data._isPlayerOpened = pNodesList(j)->IsOpened;
			_RegionTeleportNetList[i].Push( data );
		}
	}
	//for( INT i = 0; i < EWR_MAX; ++i  ){
	//	UGlobalMapInfo::GetSingleton()->FindTeleportNetNodesInRegion( &_RegionTeleportNetList[i], i );
	//}
	_IsTeleportInited = TRUE;

	MMONavigationManager::getSingleton().Init();
}
//
void MMOMapDataManager::InitAllMapRegionData()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOMapDataManager::InitAllMapRegionData"), 0);

	TArray<FInMapRegion> &datas = UGlobalMapInfo::GetSingleton()->GetAllMapRegionInfo();

	INT dataCnt = datas.Num();
	FInMapRegion *pMapZone;
	INT mapID = -1;
	for( INT i = 0; i < dataCnt; ++i ){
		pMapZone = &datas(i);

		mapID = pMapZone->MapID;
		TArray<FInMapRegion*> &singleMapRegionData = _AllMapRegionsInfo.FindOrAdd( mapID );

		singleMapRegionData.Push( pMapZone );
	}
}
//
void MMOMapDataManager::Shutdown()
{
	MMOUI::UnRegisterEventHandler( this, GameEvent_ActorTypeChanged );
	MMOUI::UnRegisterEventHandler( this, GameEvent_StopNavigate);
	MMOUI::UnRegisterEventHandler( this, GameEvent_ActiveRevivePnt );
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SelfCompatStateChange );
	MMOUI::UnRegisterEventHandler( this, GameEvent_ServerError );
	MMOUI::UnRegisterEventHandler( this, GameEvent_TeamLeave );
	MMOUI::UnRegisterEventHandler( this, GameEvent_TeamJoin );
	MMOUI::UnRegisterEventHandler( this, GameEvent_TeamRemoveMember);
	MMOUI::UnRegisterEventHandler( this, GameEvent_TeamMemberUpdate);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SceneFinished);
	MMOUI::UnRegisterEventHandler( this, GameEvent_LeaveServerScene );
	MMOUI::UnRegisterEventHandler( this, GameEvent_EnterScene );
	MMOUI::UnRegisterEventHandler( this, GameEvent_CharacterDie );
	MMOUI::UnRegisterEventHandler( this, GameEvent_UpdateMovePath );
	MMOUI::UnRegisterEventHandler( this, GameEvent_QuestStateChanged );
	MMOUI::UnRegisterEventHandler( this, GameEvent_QuestModeChange );
	MMOUI::UnRegisterEventHandler( this, GameEvent_GuildSabacStateChanged );
	MMOUI::UnRegisterEventHandler( this, GameEvent_GuildOccupy );
	//map UI
	UMMOUIWorldMap *pWorldMapPanel = ( UMMOUIWorldMap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WorldMap );
	UMMOUIMinimap *pMiniMapPanel = ( UMMOUIMinimap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	UMMOUIPathMap *pPathMapPanel = ( UMMOUIPathMap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_PathMap );
	//clear dynamic map ui objects
	FActorInMapUI *pUIActor = NULL;
	//near
	MapDynamicActorIterator iterNear = BeginIterateDymcActorNear();
	for(  ; iterNear; ++iterNear ){
		pUIActor = &iterNear.Value();
		pUIActor->_IsValid = FALSE;

		//
		if( pWorldMapPanel != NULL ){
			pWorldMapPanel->DistoryRootMc( pUIActor->_UiObjInWorldMap );
		}
		
		//
		if( pMiniMapPanel != NULL ){
			pMiniMapPanel->DistoryRootMc( pUIActor->_UiObjInMiniMap );
		}

		//
		if( pPathMapPanel != NULL ){
			pPathMapPanel->DistoryRootMc( pUIActor->_UiObjInPathMap );
		}
	}
	//Global
	MapDynamicActorIterator iterGlobal = BeginIterateDymcActorGlobal();
	for(  ; iterGlobal; ++iterGlobal ){
		pUIActor = &iterGlobal.Value();
		pUIActor->_IsValid = FALSE;
		pUIActor->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		//
		if( pMiniMapPanel != NULL ){
			pMiniMapPanel->DistoryRootMc( pUIActor->_UiObjInWorldMap );
		}
		//
		if( pWorldMapPanel != NULL ){
			pMiniMapPanel->DistoryRootMc( pUIActor->_UiObjInMiniMap );
		}
		//
		if( pPathMapPanel != NULL ){
			pPathMapPanel->DistoryRootMc( pUIActor->_UiObjInPathMap );
		}
	}

	//clear static map ui objects
	for( INT i = _ListCurMapUIStaticActors.Num() - 1; i >= 0; --i ){
		pUIActor = &_ListCurMapUIStaticActors( i );
		pUIActor->_IsValid = FALSE;
		pUIActor->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		//
		if( pMiniMapPanel != NULL ){
			pMiniMapPanel->DistoryRootMc( pUIActor->_UiObjInMiniMap );
		}
		//
		if( pPathMapPanel != NULL ){
			pPathMapPanel->DistoryRootMc( pUIActor->_UiObjInPathMap );
		}
	}

	for( INT i = _ListWorldMapUIStaticActors.Num() - 1; i >= 0; --i ){
		pUIActor = &_ListWorldMapUIStaticActors( i );
		pUIActor->_IsValid = FALSE;
		pUIActor->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		//
		if( pWorldMapPanel != NULL ){
			pWorldMapPanel->DistoryRootMc( pUIActor->_UiObjInWorldMap );
		}
	}

	//user marks
	FUserMarkInMapUI *pUIUserMark = NULL;
	for( INT i = _ListWorldMapUserMarks.Num() - 1; i >= 0; --i ){
		pUIUserMark = &_ListWorldMapUserMarks( i );
		pUIUserMark->_IsValid = FALSE;
		pUIUserMark->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		//
		if( pWorldMapPanel != NULL ){
			pWorldMapPanel->DistoryRootMc( pUIUserMark->_UiObjInWorldMap );
		}
	}

	//
	INT posCnt = _MapCurPath.Num();
	FPathPointInMapUI *pPathPosUi = NULL;
	for( INT i = 0; i < posCnt; ++i ){
		pPathPosUi = &_MapCurPath(i);
		pPathPosUi->_IsValid = FALSE;
		pPathPosUi->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		//
		if( pWorldMapPanel != NULL ){
			pWorldMapPanel->DistoryRootMc( pPathPosUi->_UiObjInWorldMap );
		}
		//
		//pMiniMapPanel->DistoryRootMc( pPathPosUi->_UiObjInMiniMap );
	}
}

//
void MMOMapDataManager::UpdateTeleportDynamicData()
{
	for( INT i = 0; i < EWR_MAX; ++i  ){
		INT inReginCnt = _RegionTeleportNetList[i].Num();

		for( INT j = 0; j < inReginCnt; ++j  ){
			FTeleportNetNodeData *pNode = &_RegionTeleportNetList[i](j);

			pNode->_isPlayerOpened = pNode->_NodeTemplate->IsOpened ? pNode->_NodeTemplate->IsOpened : pNode->_isPlayerOpened;
		}
	}
}
//
void MMOMapDataManager::ClearTeleportDynamicData()
{
	_IsTeleportUpdatedByServer = FALSE;

	for( INT i = 0; i < EWR_MAX; ++i  ){
		INT inReginCnt = _RegionTeleportNetList[i].Num();

		for( INT j = 0; j < inReginCnt; ++j  ){
			FTeleportNetNodeData *pNode = &_RegionTeleportNetList[i](j);

			pNode->_isPlayerOpened = pNode->_NodeTemplate->IsOpened;
		}
	}
}
//
//FTeleportNodeObj *MMOMapDataManager::GetTeleportNetNodes( EWorldRegion regionID, INT nodeID )
//{
//	if( regionID >= EWR_MAX ){
//		return NULL;
//	}
//
//	FTeleportNodeObj *pRtn = NULL;
//	FTeleportNodeObj *pTemp = NULL;
//	INT inReginCnt = _RegionTeleportNetList[regionID].Num();
//	for( INT i = 0; i < inReginCnt; ++i  ){
//		pTemp = _RegionTeleportNetList[regionID](i);
//
//		if( pTemp->ServerID == nodeID ){
//			pRtn = pTemp;
//			break;
//		}	
//	}
//
//	return pRtn;
//}
///*
//*@Desc:isAllMap = TRUE: search all map in this region
//*/
//INT MMOMapDataManager::GetTeleportNetNodesIn( EWorldRegion regionID, INT mapID, UBOOL isAllMap, TArray<FTeleportNodeObj*> *rtnList )
//{
//	INT rtnCnt = -1;
//	if( !_IsTeleportInited ){
//		return rtnCnt;
//	}
//
//	if( regionID >= EWR_MAX ){
//		return rtnCnt;
//	}
//
//	if( rtnList == NULL ){
//		return rtnCnt;
//	}
//
//	rtnCnt = 0;
//	rtnList->Empty();
//
//	INT inReginCnt = _RegionTeleportNetList[regionID].Num();
//	for( INT i = 0; i < inReginCnt; ++i  ){
//		FTeleportNodeObj *pNode = _RegionTeleportNetList[regionID](i);
//
//		if( isAllMap || pNode->InSceneID == mapID ){
//			rtnList->Push( pNode );
//			++rtnCnt;
//		}	
//	}
//
//	return rtnCnt;
//}
//
FTeleportNetNodeData *MMOMapDataManager::GetTeleportNetNodes( EWorldRegion regionID, INT nodeID )
{
	if( regionID >= EWR_MAX ){
		return NULL;
	}

	FTeleportNetNodeData *pRtn = NULL;
	FTeleportNodeObj *pTemp = NULL;
	INT inReginCnt = _RegionTeleportNetList[regionID].Num();
	for( INT i = 0; i < inReginCnt; ++i  ){
		pTemp = _RegionTeleportNetList[regionID](i)._NodeTemplate;

		if( pTemp->ServerID == nodeID ){
			pRtn = &_RegionTeleportNetList[regionID](i);
			break;
		}	
	}

	return pRtn;
}
//
FTeleportNetNodeData *MMOMapDataManager::GetTeleportNetNodes( EWorldRegion regionID, INT inSceneID, const FString &nodeName )
{
	if( regionID >= EWR_MAX ){
		return NULL;
	}

	FTeleportNetNodeData *pRtn = NULL;
	FTeleportNodeObj *pNodeTemplate = NULL;
	INT inReginCnt = _RegionTeleportNetList[regionID].Num();

	for( INT j = 0; j < inReginCnt; ++j  ){
		pNodeTemplate = _RegionTeleportNetList[regionID](j)._NodeTemplate;

		if( pNodeTemplate->InSceneID == inSceneID && pNodeTemplate->Name == nodeName ){
			pRtn = &_RegionTeleportNetList[regionID](j);
			break;
		}	
	}

	return pRtn;
}
//
FTeleportNetNodeData *MMOMapDataManager::GetTeleportNetNodes( INT nodeID )
{
	FTeleportNetNodeData *pRtn = NULL;

	for( INT i = 0; i < EWR_MAX; ++i  ){
		pRtn = GetTeleportNetNodes( (EWorldRegion)i, nodeID );
		if( pRtn != NULL ){
			break;
		}
	}

	return pRtn;
}
//
INT MMOMapDataManager::GetTeleportNetNodesIn( EWorldRegion regionID, INT mapID, UBOOL isAllMap, TArray<FTeleportNetNodeData*> *rtnList )
{
	INT rtnCnt = -1;
	if( !_IsTeleportInited ){
		return rtnCnt;
	}

	if( regionID >= EWR_MAX ){
		return rtnCnt;
	}

	if( rtnList == NULL ){
		return rtnCnt;
	}

	rtnCnt = 0;
	rtnList->Empty();

	INT inReginCnt = _RegionTeleportNetList[regionID].Num();
	for( INT i = 0; i < inReginCnt; ++i  ){
		FTeleportNetNodeData *pNode = &_RegionTeleportNetList[regionID](i);

		if( isAllMap || pNode->_NodeTemplate->InSceneID == mapID ){
			rtnList->Push( pNode );
			++rtnCnt;
		}	
	}

	return rtnCnt;
}

//
INT MMOMapDataManager::GetTeleportNetNodesIn( INT mapID, TArray<FTeleportNetNodeData*> *rtnList )
{
	INT rtnCnt = -1;
	if( !_IsTeleportInited ){
		return rtnCnt;
	}

	if( rtnList == NULL ){
		return rtnCnt;
	}

	rtnCnt = 0;
	rtnList->Empty();
	
	for( INT i = 0; i < EWR_MAX; ++i  ){
		INT inReginCnt = _RegionTeleportNetList[i].Num();

		for( INT j = 0; j < inReginCnt; ++j  ){
			FTeleportNetNodeData *pNode = &_RegionTeleportNetList[i](j);

			if( pNode->_NodeTemplate->InSceneID == mapID ){
				rtnList->Push( pNode );
				++rtnCnt;
			}
		}
	}

	return rtnCnt;
}

//
void MMOMapDataManager::NotifyTeleportsNetNodesOpened( INT nodeID, UBOOL isOpened )
{
	//FTeleportNodeObj* pNode = UGlobalMapInfo::GetSingleton()->FindTeleportNetNodes( nodeID );
	//pNode->IsOpened = isOpened;

	FTeleportNetNodeData* pNode = GetTeleportNetNodes( nodeID );
	if( pNode != NULL ){
		FTeleportNodeObj* pNodeClientData = pNode->_NodeTemplate;

		//pNodeClientData->IsOpened |= isOpened;
		pNode->_isPlayerOpened = ( pNodeClientData->IsOpened || isOpened );
	}
}
//
void MMOMapDataManager::ActiveCurSceneTeleports()
{
	for( INT j = 0; j < GWorld->Levels.Num(); ++j ){
		ULevel* Level = GWorld->Levels(j);

		for( INT k = 0; k < Level->Actors.Num(); ++k ){
			AActor* actor = Level->Actors(k);

			if( actor){				
				if( actor->IsA( ATeleportNetNode::StaticClass() ) ){
					ATeleportNetNode* pTeleportNode = Cast<ATeleportNetNode>(actor);
					FTeleportNetNodeData *pTeleportNodeData = GetTeleportNetNodes( (EWorldRegion)pTeleportNode->RegionID, pTeleportNode->InSceneID, pTeleportNode->GetName() );
					UBOOL isOpened = ( pTeleportNodeData != NULL && pTeleportNodeData->_isPlayerOpened );
					pTeleportNode->eventActiveThis( isOpened );
				}
			}//end of if actor
		}//end of level
	}//end of world

}
//
void MMOMapDataManager::FirstOpeneTeleportsNetNodes( INT nodeID )
{
	FTeleportNodeObj *pTeleportNodeObj = UGlobalMapInfo::GetSingleton()->FindTeleportNetNodes( nodeID );
	if( pTeleportNodeObj == NULL ){
		return;
	}

	ATeleportNetNode *pTeleportInScene = FindSceneTeleportsNetNode( pTeleportNodeObj->InSceneID, pTeleportNodeObj->Name );
	if( pTeleportInScene == NULL ){
		return;
	}

	pTeleportInScene->eventPlayFirstActived();
}

//
ATeleportNetNode *MMOMapDataManager::FindSceneTeleportsNetNode( INT inSceneID, const FString &nodeName )
{
	ATeleportNetNode *pTeleportInScene = NULL;

	for( INT j = 0; j < GWorld->Levels.Num(); ++j ){
		ULevel* Level = GWorld->Levels(j);

		for( INT k = 0; k < Level->Actors.Num(); ++k ){
			AActor* actor = Level->Actors(k);

			if( actor){				
				if( actor->IsA( ATeleportNetNode::StaticClass() ) ){
					ATeleportNetNode* pTeleportNode = Cast<ATeleportNetNode>(actor);
					
					if( pTeleportNode->InSceneID == inSceneID && nodeName == pTeleportNode->GetName() ){
						pTeleportInScene = pTeleportNode;
						break;
					}
				}
			}//end of if actor
		}//end of level
	}//end of world
	return pTeleportInScene;
}
//map
UBOOL MMOMapDataManager::IsMapRegionMultilayer( INT mapID )
{
	//FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( sceneID );
	//if( pMapInfo == NULL ){
	//	return FALSE;
	//}

	//
	const TArray<FInMapRegion*> *singleMapRegionListPointer = _AllMapRegionsInfo.Find( mapID );
	if( singleMapRegionListPointer == NULL ){
		return FALSE;
	}

	const TArray<FInMapRegion*> &singleMapRegionList = *singleMapRegionListPointer;
	INT regionCnt = singleMapRegionList.Num();
	if( regionCnt <= 1 ){
		return FALSE;
	}

	//
	for( INT i = 0; i < regionCnt; ++i ){
		const FBox &mapRegionBoxA = singleMapRegionList(i)->BoundingBox;
		for( INT j = i+1; j < regionCnt; ++j ){
			const FBox &mapRegionBoxB = singleMapRegionList(j)->BoundingBox;
			if( mapRegionBoxA.IntersectXY( mapRegionBoxB ) ){
				return TRUE;
			}
		}
	}

	return FALSE;
}
//
INT MMOMapDataManager::GetCurPlayerRegionIn( const FVector &playerPos )
{ 
	INT idx =  CheckRegionIn( _CurSceneInfo.ServerID, playerPos );
	_InCurMapRegionIdx = idx;
	_InCurMapRegion = GetReginInCurMap( _InCurMapRegionIdx );
	return _InCurMapRegionIdx;
}

//
void MMOMapDataManager::SetCurMap( INT sceneID )
{
	_CurSceneInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( sceneID );

	AMMOPlayerController *pMainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if( pMainPlayerCtrl != NULL ){
		pMainPlayerCtrl->CurMapID = sceneID;
	}
	//
	InitCurSceneProgress();
}
void MMOMapDataManager::SetCurMap( const FString &mapName )
{
	FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoByName( mapName );

	//
	SetCurMap( mapInfo.ServerID );
}
//
void MMOMapDataManager::SetCurWorldMap( INT sceneID, INT inRegionIdx )
{
	_CurWorldMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( sceneID );
	//
	_InWorldmapRegionIdx = inRegionIdx;
	_InWorldmapRegion = GetReginInMap( _CurWorldMapInfo.ServerID, inRegionIdx );
}

void MMOMapDataManager::InitCurWorldMapRegionData()
{
	_CurMapRegionInfo.Reset();
	for( INT j = 0; j < GWorld->Levels.Num(); ++j ){
		ULevel* Level = GWorld->Levels(j);

		for( INT k = 0; k < Level->Actors.Num(); ++k ){
			AActor* actor = Level->Actors(k);

			if( actor){				
				if( actor->IsA( AMMOVolume_MapSize::StaticClass() ) ){
					_CurMapRegionInfo.AddItem( Cast<AMMOVolume_MapSize>( actor ) );
				}
			}//end of if actor
		}//end of level
	}//end of world
}

//===================================================================================
FInMapRegion *MMOMapDataManager::GetReginInMap( INT mapID, const FString &inRegionVolumeName )
{
	TArray<FInMapRegion*> &singleMapRegionList = _AllMapRegionsInfo.FindOrAdd( mapID );
	INT regionCnt = singleMapRegionList.Num();

	for( INT i = 0; i < regionCnt; ++i ){
		if( singleMapRegionList(i)->VolumeName == inRegionVolumeName ){
			return singleMapRegionList(i);
		}
	}
	return NULL;
}
//
INT MMOMapDataManager::GetReginInMapIdx( INT mapID, const FString &inRegionVolumeName )
{
	TArray<FInMapRegion*> &singleMapRegionList = _AllMapRegionsInfo.FindOrAdd( mapID );
	INT regionCnt = singleMapRegionList.Num();

	for( INT i = 0; i < regionCnt; ++i ){
		if( singleMapRegionList(i)->VolumeName == inRegionVolumeName ){
			return i;
		}
	}
	return -1;
}
//===================================================================================
//map area check
INT MMOMapDataManager::CheckRegionIn( INT mapID, const FVector &checkPos )
{
	//
	FVector max;
	FVector min;
	UBOOL isIn = false;
	INT curInIdx = -1;
	INT curInPriority = -1;
	TArray<FInMapRegion*> &singleMapRegionList = _AllMapRegionsInfo.FindOrAdd( mapID );
	FInMapRegion* pRegion;
	FBox regionBox;
	INT regionCnt = singleMapRegionList.Num();
	for( int idx = 0; idx < regionCnt; ++idx ){
		pRegion = singleMapRegionList( idx );

		isIn = pRegion->BoundingBox.IsInside( checkPos );

		if( isIn ){
			//first time
			if( curInIdx == -1 ){
				curInIdx = idx;
				curInPriority = singleMapRegionList( curInIdx )->Priority;
			}
			//take the one with the highest priority
			if(  pRegion->Priority > curInPriority ){
				curInIdx = idx;
				curInPriority = pRegion->Priority;
			}
		}
	}

	return curInIdx;
}
//=================================================
UBOOL MMOMapDataManager::IsInCurRegion( const FVector &checkPos )
{
	if( _InCurMapRegion == NULL ){
		return FALSE;
	}

	UBOOL isInRegion = _InCurMapRegion->BoundingBox.IsInside( checkPos );
	return isInRegion;
}
//INT MMOMapDataManager::CheckRegion()
//{
//	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
//	//
//	FVector max;
//	FVector min;
//	FVector check = pl->Location;
//	UBOOL isIn = false;
//	INT curInIdx = -1;
//	INT curInPriorrity = -1;
//	AMMOVolume_MapSize *pRegion = NULL;
//	FBox regionBox;
//	for( int idx = 0; idx < _CurMapRegionInfo.Num(); ++idx ){
//		pRegion = _CurMapRegionInfo( idx );
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
//				curInPriorrity = _CurMapRegionInfo( curInIdx )->Priorrity;
//			}
//			//
//			if(  pRegion->Priorrity > curInPriorrity ){
//				curInIdx = idx;
//				curInPriorrity = pRegion->Priorrity;
//			}
//		}
//	}
//
//	_InCurMapRegionIdx = curInIdx;
//	return _InCurMapRegionIdx;
//}

//========================================================================================================
void MMOMapDataManager::InitialMapUserMarks( INT sceneID )
{
	const TMap<INT, FMapMark> *pMapUserMarksMap = _AllMapUserMarkContainer.Find( sceneID );

	//reset UI data
	ResetCurWorldUserMarkUI();
	//
	if( pMapUserMarksMap == NULL ){
		//
		LoadMapUserMarks( sceneID );
	}
	else{
		TMap<INT, FMapMark> &mapUserMarksMap = _AllMapUserMarkContainer.FindOrAdd( sceneID );
		for( TMap<INT, FMapMark>::TIterator iter( mapUserMarksMap ); iter; ++iter){
			FMapMark &mark = iter.Value();

			if( mark._isValid ){
				AddMapMarkToUI( &mark );
			}
		}
	}

}

UBOOL MMOMapDataManager::LoadMapUserMarks( INT sceneID )
{
	FString mapName = UGlobalMapInfo::GetSingleton()->FindMapInfo( sceneID ).MapName;
	if( mapName.IsEmpty() ){
		return FALSE;
	}

	//read data from file
	TArray<FMapMarkExport> vecUserMarkData;
	FArchive* pArchive      = NULL;
	if( MMOFacade::getSingleton().BeginLoadAccountBinary( &pArchive,mapName ) )
	{
		INT TotalSize = pArchive->TotalSize();
		INT Size = TotalSize *sizeof(BYTE)/sizeof(FMapMarkExport);
		vecUserMarkData.Add( Size  );
		pArchive->Serialize( vecUserMarkData.GetData(), pArchive->TotalSize() );
	}
	MMOFacade::getSingleton().EndLoadAccountBinary( pArchive );

	//
	INT dataCnt = vecUserMarkData.Num();
	if( dataCnt <= 0 ){
		//no data
		return TRUE;
	}

	TMap<INT, FMapMark> &mapUserMarksMap = _AllMapUserMarkContainer.FindOrAdd( sceneID );
	mapUserMarksMap.Empty();
	
	//convert read data
	for( INT i = 0; i < dataCnt; ++i )
	{
		FMapMarkExport &markData = vecUserMarkData(i);

		FMapMark &mark = mapUserMarksMap.FindOrAdd( i );

		mark._MarkID = i;
		mark._SceneID = sceneID;
		mark._MarkName = markData._MarkName;
		mark._RegionName = markData._RegionName;
		mark._Pos = markData._Pos;
		mark._MarkType = markData._MarkType;
		mark._isValid = TRUE;

		AddMapMarkToUI( &mark );
	}

	return TRUE;
}

//
UBOOL MMOMapDataManager::ExportMapUserMarks( INT sceneID )
{
	FString mapName = UGlobalMapInfo::GetSingleton()->FindMapInfo( sceneID ).MapName;
	if( mapName.IsEmpty() ){
		return FALSE;
	}

	//if no data return
	if( _AllMapUserMarkContainer.Find( sceneID ) == NULL ){
		return TRUE;
	}

	TMap<INT, FMapMark> &mapUserMarksMap = _AllMapUserMarkContainer.FindOrAdd( sceneID );

	//convert data for export
	TArray<FMapMarkExport> vecUserMarkData;
	INT markNameStrSize = 17;
	INT regionNameStrSize = 33;
	for( TMap<INT, FMapMark>::TIterator iter( mapUserMarksMap ); iter; ++iter){
		FMapMark &mark = iter.Value();

		if( mark._isValid == FALSE ){
			continue;
		}

		FMapMarkExport exportMarkData;
		appStrncpy( exportMarkData._MarkName, *mark._MarkName, markNameStrSize );
		appStrncpy( exportMarkData._RegionName, *mark._RegionName, regionNameStrSize );
		exportMarkData._Pos = mark._Pos;
		exportMarkData._MarkType = mark._MarkType;

		vecUserMarkData.Push( exportMarkData );
	}

	//write data to file
	size_t  lDataLength = sizeof(FMapMarkExport) * vecUserMarkData.Num();
	FArchive* pArchive = NULL;
	void *pData = vecUserMarkData.GetData();
	if( MMOFacade::getSingleton().BeginExportAccountData( &pArchive, mapName ) ){
		pArchive->ByteOrderSerialize( pData, lDataLength );
	}
	MMOFacade::getSingleton().EndExportAccountData( pArchive );

	return TRUE;
}

//
void MMOMapDataManager::ResetCurWorldUserMarkUI()
{
	FUserMarkInMapUI *pMapMarkUI = NULL;
	INT markCnt = _ListWorldMapUserMarks.Num();
	for( INT i = 0; i < markCnt; ++i ){
		pMapMarkUI = &_ListWorldMapUserMarks(i);

		pMapMarkUI->_IsValid = FALSE;
		pMapMarkUI->_NeedUpdateFlag = EACTUIUF_UpdateAll;
	}
}

//
FUserMarkInMapUI *MMOMapDataManager::AddMapMark( INT sceneID, const FString &regionVolumeName )
{
	FMapMark *pNewMapMarkData = AddMapMarkData( sceneID, regionVolumeName );
	return AddMapMarkToUI( pNewMapMarkData );
}

//
FUserMarkInMapUI *MMOMapDataManager::AddMapMarkToUI( FMapMark *pNewMapMarkData )
{
	if( pNewMapMarkData == NULL ){
		return NULL;
	}

	//try to find the same one or unused slot
	FUserMarkInMapUI *pMapMarkUI = NULL;
	FMapMark *pMapMarkData = NULL;
	INT markCnt = _ListWorldMapUserMarks.Num();
	INT findUnusedIdx = -1;
	for( INT i = 0; i < markCnt; ++i ){
		pMapMarkUI = &_ListWorldMapUserMarks(i);

		if( ( pMapMarkUI->_IsValid == FALSE ) ){
			findUnusedIdx = i;
			break;
		}
	}

	//
	pMapMarkUI = NULL;
	if( findUnusedIdx >= 0){
		//find unused slot but not exist a same one
		pMapMarkUI = &_ListWorldMapUserMarks( findUnusedIdx );
		pMapMarkUI->_IsValid = TRUE;
		pMapMarkUI->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		//
		pMapMarkUI->_Type = EACTMUI_UserMark;
		pMapMarkUI->_RelatedMarkID = pNewMapMarkData->_MarkID;
		pMapMarkUI->_DisplayName = pNewMapMarkData->_MarkName;
	}
	//if nothing found then add an new
	else{
		FUserMarkInMapUI newMarkUI;

		newMarkUI._ID = _ListWorldMapUserMarks.Num();
		newMarkUI._IsValid = TRUE;
		newMarkUI._NeedUpdateFlag = EACTUIUF_UpdateAll;
		//
		newMarkUI._Type = EACTMUI_UserMark;
		newMarkUI._RelatedMarkID = pNewMapMarkData->_MarkID;
		newMarkUI._DisplayName = pNewMapMarkData->_MarkName;

		_ListWorldMapUserMarks.Push( newMarkUI );

		pMapMarkUI = &_ListWorldMapUserMarks.Last();
	}

	pMapMarkUI->_UiIconIdx = FString::Printf( TEXT("mark%d"), ( pNewMapMarkData->_MarkType + 1) );

	return pMapMarkUI;
}

//
FMapMark *MMOMapDataManager::AddMapMarkData( INT sceneID, const FString &regionVolumeName, const FString &markName, INT markType, const FVector &pos )
{
	FMapMark *pNewMapMarkData = AddMapMarkData( sceneID, regionVolumeName );
	if( pNewMapMarkData == NULL ){
		return NULL;
	}

	pNewMapMarkData->_MarkName = markName;
	pNewMapMarkData->_MarkType = markType;
	pNewMapMarkData->_Pos = pos;

	return pNewMapMarkData;
}
//
FMapMark *MMOMapDataManager::AddMapMarkData( INT sceneID, const FString &regionVolumeName )
{
	TMap< INT, FMapMark > &mapMarkMap = _AllMapUserMarkContainer.FindOrAdd( sceneID );

	UINT idx =  mapMarkMap.Num();
	FMapMark &newMark = mapMarkMap.FindOrAdd( idx );

	//assign the highest index in the list
	newMark._MarkID = idx;//( ( mapMarkList.Num() > 0 ) ? ( mapMarkList.Last()._MarkID + 1 ) : 0 );
	newMark._SceneID = sceneID;
	newMark._RegionName = regionVolumeName;
	newMark._isValid = TRUE;

	//mark the map
	_UserMarksEditedMapContainer.FindOrAdd( sceneID );

	return &newMark;//&mapMarkList.Last();
}

//
TMap< INT, FMapMark > *MMOMapDataManager::GetMapMarkDatas( INT sceneID )
{
	TMap< INT, FMapMark > *pRst = _AllMapUserMarkContainer.Find( sceneID );
	return pRst;
}

//
FMapMark *MMOMapDataManager::GetMapMarkData( INT sceneID, INT markID )
{
	const FMapMark *pConstFoundMark = NULL;
	FMapMark *pFoundMark = NULL;

	//if no data return
	if( _AllMapUserMarkContainer.Find( sceneID ) == NULL ){
		return NULL;
	}

	TMap< INT, FMapMark > &mapMarkMap = _AllMapUserMarkContainer.FindOrAdd( sceneID );
	pConstFoundMark = mapMarkMap.Find( markID );
	if( pConstFoundMark != NULL && pConstFoundMark->_isValid ){
		pFoundMark = &mapMarkMap.FindOrAdd( markID );
	}

	return pFoundMark;
}

//
void MMOMapDataManager::RemoveMapMark( INT sceneID, INT markID )
{
	FUserMarkInMapUI *pMapMarkUI = NULL;
	INT markCnt = _ListWorldMapUserMarks.Num();
	INT findIdx = -1;
	for( INT i = 0; i < markCnt; ++i ){
		pMapMarkUI = &_ListWorldMapUserMarks(i);

		if( pMapMarkUI->_RelatedMarkID == markID ){
			findIdx = i;
			break;
		}
	}
	
	//
	if( findIdx < 0 ){
		return;
	}

	pMapMarkUI->_IsValid = FALSE;
	pMapMarkUI->_NeedUpdateFlag = EACTUIUF_UpdateAll;

	//remove map mark data
	RemoveMapMarkData( sceneID, markID );
}

//
FMapMark *MMOMapDataManager::RemoveMapMarkData( INT sceneID, INT markID )
{
	FMapMark * pFoundMark = GetMapMarkData( sceneID, markID );

	if( pFoundMark == NULL ){
		return NULL;
	}

	pFoundMark->_isValid = FALSE;

	//mark the map
	_UserMarksEditedMapContainer.FindOrAdd( sceneID );

	return pFoundMark;
}

//============================================================================================================
//dynamic objects, shown on the whole map

//
void MMOMapDataManager::RemoveAllMapDymcActorGlobalOfType( EActorInMapUIType actorType )
{
	TArray<FActorInMapUI*> actorArray;
	FindMapDymcActorGlobalOfType( actorType, actorArray );

	//
	INT cnt = actorArray.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pActor = actorArray(i);
		//INT actorIDInMap = actorArray(i)->_ActorID;
		//RemoveMapDymcActorGlobal( actorIDInMap );
		pActor->_ActorID = -1;
		pActor->_AidID = -1;
		pActor->_IsValid = FALSE;
		pActor->_NeedUpdateFlag = EACTUIUF_UpdateAll;
	}
}

//
void MMOMapDataManager::RemoveMapDymcActorGlobalOfType(  EActorInMapUIType actorType, INT actorID )
{
	TArray<FActorInMapUI*> actorArray;
	FindMapDymcActorGlobalOfType( actorType, actorArray );

	//
	INT cnt = actorArray.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI *pActor = actorArray(i);
		if( pActor->_ActorID != actorID ){
			continue;
		}

		pActor->_ActorID = -1;
		pActor->_AidID = -1;
		pActor->_IsValid = FALSE;
		pActor->_NeedUpdateFlag = EACTUIUF_UpdateAll;
	}
}

//
void MMOMapDataManager::FindMapDymcActorGlobalOfType( EActorInMapUIType actorType, TArray<FActorInMapUI*> &rtnArray )
{
	rtnArray.Empty();

	MapDynamicActorContainer::TIterator iter(_MapGlobalActors);
	FActorInMapUI *pInValidActorInMap;
	for ( ; iter; ++iter){
		pInValidActorInMap = &iter.Value();

		if( pInValidActorInMap->_Type == actorType ){
			rtnArray.Push( pInValidActorInMap );
		}
	}
}

//
FActorInMapUI *MMOMapDataManager::FindMapDymcActorGlobal( INT actorID )
{
	FActorInMapUI *pActorInMap = _MapGlobalActors.Find( actorID );

	if( pActorInMap == NULL ){
		return NULL;
	}
	
	if( !pActorInMap->_IsValid ){
		return NULL;
	}

	return pActorInMap;
}

//
FActorInMapUI *MMOMapDataManager::AddOrUpdateMapDymcActorGlobal( INT actorID, EActorInMapUIType type, INT inMapID, const FString &displayName )
{
	FActorInMapUI *pActorInMapUI = NULL;
	pActorInMapUI = AddOrUpdateMapDymcActor( _MapGlobalActors, actorID, displayName );

	//if teammate
	pActorInMapUI->_InMapID = inMapID;
	pActorInMapUI->_Type = type;
	switch( type )
	{
	case EACTMUI_GlobalTeammate:
		{
			pActorInMapUI->_UiIconIdx = TEXT("ally");
			//remove the object already added to near
			AMMOActor* nearActor = MMOFacade::getSingleton().GetCharacter( actorID );
			if( nearActor != NULL ){
				RemoveMapDymcActorNear( actorID );
			}
		}
		break;
	case EACTMUI_BattleFlag:
		{
			if( actorID == EBTLTMS_Team1 ){
				pActorInMapUI->_UiIconIdx = TEXT("flagRed");//FString::Printf( TEXT("flag%d"), actorID );
			}
			else{
				pActorInMapUI->_UiIconIdx = TEXT("flagBlue");
			}
		}
		break;
	default:
		break;
	}
	
	return pActorInMapUI;
}

//dynamic objects, shown around the local player
FActorInMapUI *MMOMapDataManager::AddOrUpdateMapDymcActorNear( AMMOActor &addCharactor )
{
	FString displayName;

	//dead are not added
	if( addCharactor.eventIsDead() ){
		return NULL;
	}

	EActorClassType actorType = (EActorClassType)addCharactor.GetActorType();
	switch( actorType ){
	case EACT_Monster:
	case EACT_Boss:
	case EACT_Pet:
		{
			ANPC *pNpc = Cast<ANPC>( &addCharactor );
			if( pNpc->Template && pNpc->Template->HideHUDName ){
				return NULL;
			}

			displayName = pNpc->DisplayName;
		}
		break;

	case EACT_OtherPlayer:
		{
			AMMOPlayer *pPlayer = Cast<AMMOPlayer>( &addCharactor );

			//if he is teamMate, refuse to add, this will be add in _MapGlobalActors
			//AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			MMOTeamMember *pTeammate = TeamManager::getSingleton().GetMemberById( addCharactor.ServerID );
			if( pTeammate != NULL ){
				return NULL;
			}
			//bool isTeamMate = ( pMainPlayer->TeamManager->GetMemberPosByMemberId( addCharactor.ServerID ) >= 0 );
			//if( isTeamMate ){
			//	return NULL;
			//}

			displayName = TEXT("");//name cannot be obtained here, it will be obtained later in SyncRoleData
		}
		break;

	case EACT_NPC:
		{
			ANPC *pNpc = Cast<ANPC>( &addCharactor );
			const UINT *pInListIdx = GetCurScenePuttedUsefulNpcs().Find( pNpc->NPCID );
			if( pInListIdx != NULL ){
				//already showed in static map points
				return NULL;
			}
			displayName = pNpc->DisplayName;
		}
		break;
	}

	//
	FActorInMapUI *pNew = AddOrUpdateMapDymcActor( _MapNearActors, addCharactor.ServerID, displayName );
	//
	ChangeMapMarkDisplayIcon( pNew, addCharactor );

	return pNew;
}

//dynamic points on the map UI
FActorInMapUI *MMOMapDataManager::AddOrUpdateMapDymcActor( MapDynamicActorContainer &container, INT actorID, const FString &displayName )
{
	FActorInMapUI *pActorInMapUI = container.Find( actorID );

	if( pActorInMapUI == NULL ){
		//find unused
		FActorInMapUI *pInValidActorInMap = NULL;
		bool hasUnused = false;
		MapDynamicActorContainer::TIterator iter(container);
		for ( ; iter; ++iter){
			pInValidActorInMap = &iter.Value();
			if( pInValidActorInMap->_IsValid == FALSE ){
				hasUnused = true;
				break;
			}
		}

		//
		UGFxObject *pMinimapUiObj = NULL;
		UGFxObject *pWorldmapUiObj = NULL;
		UGFxObject *pPathMapUiObj = NULL;
		UINT objID = 0;
		if( hasUnused && ( pInValidActorInMap != NULL ) ){
			pMinimapUiObj = pInValidActorInMap->_UiObjInMiniMap;
			pInValidActorInMap->_UiObjInMiniMap = NULL;
			
			pPathMapUiObj = pInValidActorInMap->_UiObjInPathMap;
			pInValidActorInMap->_UiObjInPathMap = NULL;

			pWorldmapUiObj = pInValidActorInMap->_UiObjInWorldMap;
			pInValidActorInMap->_UiObjInWorldMap = NULL;
			
			objID = pInValidActorInMap->_ID;

			//
			MapDynamicActorContainer::KeyInitType key = iter.Key();
			container.Remove( key );
		}
		else{
			objID = container.Num() + 1;//0 is reserved
		}

		//if all is using, add an new one
		pActorInMapUI = &container.FindOrAdd( actorID );
		pActorInMapUI->_UiObjInMiniMap = pMinimapUiObj;
		pActorInMapUI->_UiObjInPathMap = pPathMapUiObj;
		pActorInMapUI->_UiObjInWorldMap = pWorldmapUiObj;
		pActorInMapUI->_ID = objID;
	}
		
	//
	pActorInMapUI->_IsValid = TRUE;
	pActorInMapUI->_NeedUpdateFlag = EACTUIUF_UpdateAll;

	//
	pActorInMapUI->_ActorID = actorID;

	//determine the display icon and add name
	//EActorClassType actorType = (EActorClassType)addCharactor.GetActorType();
	pActorInMapUI->_DisplayName = displayName;

	return pActorInMapUI;
}

//
void MMOMapDataManager::ChangeMapMarkDisplayIcon( FActorInMapUI *pActorInMapUI,  AMMOActor &charactor )
{
	EActorClassType actorType = (EActorClassType)charactor.GetActorType();
	switch( actorType ){
	case EACT_Monster:
		{
			UBOOL isHasMaster = !charactor.MasterName.IsEmpty();
			if( !isHasMaster ){
				pActorInMapUI->_Type = EACTMUI_NearMonster;
				pActorInMapUI->_UiIconIdx = TEXT("monster");
			}
			else{
				if( MMOFacade::getSingleton().IsMonsterOwnedByMe( &charactor ) ){
					pActorInMapUI->_Type = EACTMUI_MyPet;
					pActorInMapUI->_UiIconIdx = TEXT("myPet");
				}
				else{
					pActorInMapUI->_Type = EACTMUI_OtherPet;
					pActorInMapUI->_UiIconIdx = TEXT("otherPet");
				}
			}
		}
		break;

	case EACT_Boss:
		{
			pActorInMapUI->_Type = EACTMUI_NearBoss;
			pActorInMapUI->_UiIconIdx = TEXT("boss");
		}
		break;

	case EACT_Pet:
		{
			if( MMOFacade::getSingleton().IsMonsterOwnedByMe( &charactor ) ){
				pActorInMapUI->_Type = EACTMUI_MyPet;
				pActorInMapUI->_UiIconIdx = TEXT("myPet");
			}
			else{
				pActorInMapUI->_Type = EACTMUI_OtherPet;
				pActorInMapUI->_UiIconIdx = TEXT("otherPet");
			}
		}
		break;

	case EACT_OtherPlayer:
		{
			ChangeMapNearPlayerMarkDisplayIcon( pActorInMapUI, charactor );
		}
		break;

	case EACT_NPC:
		{
			pActorInMapUI->_Type = EACTMUI_NearNormalNPC;
			pActorInMapUI->_UiIconIdx = TEXT("npc");
		}
		break;

	default:
		{
			pActorInMapUI->_DisplayName = TEXT("UnKnown");
			pActorInMapUI->_Type = EACTMUI_InVisible;
			pActorInMapUI->_UiIconIdx = TEXT("monster");
		}
		break;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void MMOMapDataManager::ChangeMapNearPlayerMarkDisplayIcon( FActorInMapUI *pActorInMapUI,  AMMOActor &charactor )
{
	if( pActorInMapUI == NULL ){
		return;
	}

	pActorInMapUI->_Type = EACTMUI_NearPlayer;
	pActorInMapUI->_UiIconIdx = TEXT("normalPlayer");

	UBOOL isSameGuild = MMOGuildData::getSingleton().IsGuildFullMember( charactor.ServerID );
	if( isSameGuild ){
		pActorInMapUI->_UiIconIdx = TEXT("guildPlayer");
	}

	AMMOPlayer *pOtherPlayer =  Cast<AMMOPlayer>( &charactor );
	INT guildSabacState = MMOGuildData::getSingleton().GetGuildSabacState( pOtherPlayer );
	switch( guildSabacState )
	{
	case GSS_ORANGE:
		pActorInMapUI->_UiIconIdx = TEXT("sbk3");
		break;
	case GSS_BLUE:
		pActorInMapUI->_UiIconIdx = TEXT("sbk1");
		break;
	case GSS_GREEN:
		pActorInMapUI->_UiIconIdx = TEXT("sbk2");
		break;
	}
}
//----------------------------------------------------------------------------------------------
//
void MMOMapDataManager::RemoveMapDymcActorGlobal( INT actorID )
{
	RemoveMapDymcActor( _MapGlobalActors, actorID );
	//
	AMMOActor* nearActor = MMOFacade::getSingleton().GetCharacter( actorID );
	//add to near
	if( nearActor != NULL ){
		AddOrUpdateMapDymcActorNear( *nearActor );
	}
}

//
void MMOMapDataManager::RemoveMapDymcActor( MapDynamicActorContainer &container, INT actorID )
{
	FActorInMapUI *pActorInMap = container.Find( actorID );

	if( pActorInMap != NULL && ( pActorInMap->_IsValid != FALSE ) ){
		pActorInMap->_ActorID = -1;
		pActorInMap->_IsValid = FALSE;
		pActorInMap->_NeedUpdateFlag = EACTUIUF_UpdateAll;

		////hide ui
		//if( pActorInMap->_UiObjInWorldMap != NULL ){
		//	pActorInMap->_UiObjInWorldMap->SetVisible( FALSE );
		//}

		//if( pActorInMap->_UiObjInMiniMap != NULL ){
		//	pActorInMap->_UiObjInMiniMap->SetVisible( FALSE );
		//}
	}
}

//
FActorInMapUI *MMOMapDataManager::GetMapDymcActor( MapDynamicActorContainer &container, INT actorID )
{
	FActorInMapUI *pActorInMap = container.Find( actorID );

	return pActorInMap;
}

//
FActorInMapUI *MMOMapDataManager::GetMapDymcActorByUiID( MapDynamicActorContainer &container, INT uiID )
{
	FActorInMapUI *pActorInMap = NULL;
	const FActorInMapUI *pActorInMapTryToFind = container.Find( uiID );
	if( pActorInMapTryToFind != NULL && pActorInMapTryToFind->_ID == uiID ){
		pActorInMap = &container.FindOrAdd( uiID );
		return pActorInMap;
	}

	MMOMapDataManager::MapDynamicActorIterator iter(container);

	for(  ; iter; ++iter ){
		if( iter.Value()._ID == uiID ){
			pActorInMap = &iter.Value();
			break;
		}
	}
	return pActorInMap;
}

//=================================================================================================================
//static points on the map UI
//void MMOMapDataManager::UpdateMapNpcMarks( INT actorID, INT sceneID )
//{
//
//}

//
void MMOMapDataManager::PrepareMapStaticActorInMapUI( INT sceneServerID, TArray< FActorInMapUI > &container, bool isCurScene )
{
	INT curObjIdx = 0;

	//NPC
	if( isCurScene ){
		curObjIdx = PutNpcToMapUI( sceneServerID, curObjIdx, container, _CurScenePuttedUsefulNpcContainer );
	}
	else{
		curObjIdx = PutNpcToMapUI( sceneServerID, curObjIdx, container, _ScenePuttedUsefulNpcContainer );
	}
	
	//monster points (only shown on the world map, so no need to record separately)
	curObjIdx = PutNpcGroupsToMapUI( sceneServerID, curObjIdx, container );

	//teleport point
	curObjIdx = PutTelportToMapUI( sceneServerID, curObjIdx, container );

	//teleport station
	curObjIdx = PutTelportNodeToMapUI( sceneServerID, curObjIdx, container );

	//special area
	curObjIdx = PutSpeciZoneToMapUI( sceneServerID, curObjIdx, container );
	
	//pathfinding points (areas, shown with text instead of icons)
	curObjIdx = PutNavPointsToMapUI( sceneServerID, curObjIdx, container );

	//invalid the rest
	FActorInMapUI *pStaticActorInMap;
	INT cntAllObjInMap = container.Num();
	for( ; curObjIdx < cntAllObjInMap; ++curObjIdx ){
		pStaticActorInMap = &container( curObjIdx );

		pStaticActorInMap->_IsValid = FALSE;
		pStaticActorInMap->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		pStaticActorInMap->_ActorID = -1;
	}

}

//
int MMOMapDataManager::PutNpcToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container, TMap< UINT, UINT > &npcContainer )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	const TArray<FNPCInstanceInfo*> &npcInMapInfoList = *( GetNpcInstanceInMap( sceneServerID ) );

	_CompleteTaskContainer.Empty();
	_UncompleteTaskContainer.Empty();
	_AcceptableTaskContainer.Empty();
	//_ScenePuttedUsefulNpcContainer.Empty();
	npcContainer.Empty();

	FActorInMapUI *pStaticActorInMap = NULL;
	FNPCInstanceInfo *pNpcInMapInfo;
	INT cntAllNpc = npcInMapInfoList.Num();
	INT cntAllObjInMap = container.Num();
	TArray<INT> completeList;
	TArray<INT> uncompleteList;
	TArray<INT> acceptableList;
	for( INT i = 0; i < cntAllNpc; ++i )
	{
		pNpcInMapInfo = npcInMapInfoList(i);
		//pQuestInfo = pQuestManager->FindNPCQuestInfo( pNpcInMapInfo->TemplateID );

		completeList.Empty();
		uncompleteList.Empty();
		acceptableList.Empty();

		EQuestHUDStatus npcTaskState = EQHUDS_None;
		BYTE questType = E_qt_MAX;
		MMOQuestData::getSingleton().IsNpcHasMyQuest( pNpcInMapInfo->TemplateID, &completeList, &uncompleteList, &acceptableList );
		UBOOL isShowQuest = MMOAssistManager::getSingleton().IsQuestMode();
		if( isShowQuest ){
			//1
			if( acceptableList.Num() > 0 ){
				npcTaskState = EQHUDS_Acceptable;

				const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(acceptableList(0));
				if (quest) {
					questType = quest->QuestType;	
				}	
			}
			//2
			if( uncompleteList.Num() > 0 ){
				npcTaskState = EQHUDS_NotValidForSubmit;

				const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(uncompleteList(0));
				if (quest) {
					questType = quest->QuestType;	
				}	
			}
			//3
			if( completeList.Num() > 0 ){
				npcTaskState = EQHUDS_CanSubmit;

				const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(completeList(0));
				if (quest) {
					questType = quest->QuestType;	
				}	
			}
		}

		//if it is not a function NPC and has no quests, skip it
		if( pNpcInMapInfo->NpcFuncType == 0 &&
			( npcTaskState != EQHUDS_CanSubmit && npcTaskState != EQHUDS_Acceptable && npcTaskState != EQHUDS_NotValidForSubmit ) ){
				continue;
		}

		//add more if not enough
		if( curObjIdx >= cntAllObjInMap ){
			FActorInMapUI newStaticActorInMap;
			container.Push( newStaticActorInMap );
			//recompute the total
			cntAllObjInMap = container.Num();
		}
		//
		pStaticActorInMap = &container( curObjIdx );

		//
		//_ScenePuttedUsefulNpcContainer.FindOrAdd( pNpcInMapInfo->TemplateID ) = curObjIdx;//pNpcInMapInfo;
		//npcContainer.FindOrAdd( pNpcInMapInfo->TemplateID ) = curObjIdx;//pNpcInMapInfo;
		npcContainer.FindOrAdd( i ) = curObjIdx;//pNpcInMapInfo;

		pStaticActorInMap->_Type = EACTMUI_StaticNPC;
		pStaticActorInMap->_ToggleFlag = EACTUIF_None;
		pStaticActorInMap->_IsValid = TRUE;
		pStaticActorInMap->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		pStaticActorInMap->_ID = curObjIdx;

		//const UNPCTemplate* npcTemp = mmoFacade.FindNPCTemplate( pNpcInMapInfo->TemplateID );
		//if( npcTemp != NULL ){
		//	pStaticActorInMap->_DisplayName = npcTemp->IdentityName;
		//}
		//else{
			pStaticActorInMap->_DisplayName = pNpcInMapInfo->NpcDiplayName;
		//}
		

		++curObjIdx;

		pStaticActorInMap->_ActorID = i;//pNpcInMapInfo->TemplateID;//i;//idx in NpcInstanceMap
		pStaticActorInMap->_AidID = pNpcInMapInfo->TemplateID;
		pStaticActorInMap->_OrigInMapPos = pNpcInMapInfo->Position;
		pStaticActorInMap->_InMapID = sceneServerID;

		//switch the icon
		INT npcFuncType = pNpcInMapInfo->NpcFuncType;
		//if he is a function npc
		if( npcFuncType != 0 ){
			pStaticActorInMap->_ToggleFlag |= EACTUIF_FunctionNpc;
		}
		//defaut, is a normal npc
		pStaticActorInMap->_UiIconIdx = TEXT("npc");
		//
		//pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_NormalFun ) ? TEXT("normalPalyer") : pStaticActorInMap->_UiIconIdx );
		//
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_RecruitNpc ) ? TEXT("recruitNpc") : pStaticActorInMap->_UiIconIdx );
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_GuildNpc ) ? TEXT("guildNpc") : pStaticActorInMap->_UiIconIdx );
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_SalaryNpc ) ? TEXT("salaryNpc") : pStaticActorInMap->_UiIconIdx );
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_ReputationNpc ) ? TEXT("reputationNpc") : pStaticActorInMap->_UiIconIdx );

		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_ArenaNpc ) ? TEXT("areanaNpc") : pStaticActorInMap->_UiIconIdx );
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_PVP ) ? TEXT("battleNpc") : pStaticActorInMap->_UiIconIdx );
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_Warehouse ) ? TEXT("warehouse") : pStaticActorInMap->_UiIconIdx );
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_EquipEnhance ) ? TEXT("blacksmith") : pStaticActorInMap->_UiIconIdx );
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_Merchant ) ? TEXT("merchant") : pStaticActorInMap->_UiIconIdx );
		//
		pStaticActorInMap->_UiIconIdx = ( IsNpcHasFun( npcFuncType, ENPCFY_Warehouse ) ? TEXT("warehouse") : pStaticActorInMap->_UiIconIdx );
		

		//finally check quests
		if( isShowQuest ){
			if( npcTaskState != EQHUDS_None ){
				pStaticActorInMap->_ToggleFlag |= EACTUIF_TaskNpc;
			}

			switch( npcTaskState )
			{
			case EQHUDS_Acceptable:
				pStaticActorInMap->_UiIconIdx = TEXT("taskStart");
				//pStaticActorInMap->_ToggleFlag |= EACTUIF_TaskNpc;
				break;
			case EQHUDS_CanSubmit:
				pStaticActorInMap->_UiIconIdx = TEXT("taskEnd");
				//pStaticActorInMap->_ToggleFlag |= EACTUIF_TaskNpc;
				break;
			case EQHUDS_NotValidForSubmit:
				pStaticActorInMap->_UiIconIdx = TEXT("taskUnfinished");
				break;
			default:
				break;
			}
			//
			if( npcTaskState == EQHUDS_Acceptable || npcTaskState == EQHUDS_CanSubmit ){
				switch( questType )
				{
				case E_qt_Daily:
					pStaticActorInMap->_UiIconIdx += TEXT("Daily");
					break;
				case E_qt_Reward:
					pStaticActorInMap->_UiIconIdx += TEXT("Reward");
					break;
				case E_qt_Master:
				case E_qt_Guild:
					pStaticActorInMap->_UiIconIdx += TEXT("Master");
					break;
				default:
					break;
				}
			}

			//record quests
			INT completeListCnt = completeList.Num();
			for( INT i = 0; i < completeListCnt; ++i ){
				UINT &npcID = _CompleteTaskContainer.FindOrAdd( completeList( i ) );
				npcID = pStaticActorInMap->_ActorID;//pNpcInMapInfo->TemplateID;
			}

			INT uncompleteListCnt = uncompleteList.Num();
			for( INT i = 0; i < uncompleteListCnt; ++i ){
				UINT &npcID = _UncompleteTaskContainer.FindOrAdd( uncompleteList( i ) );
				npcID = pStaticActorInMap->_ActorID;//pNpcInMapInfo->TemplateID;
			}

			INT acceptableListCnt = acceptableList.Num();
			for( INT i = 0; i < acceptableListCnt; ++i ){
				UINT &npcID = _AcceptableTaskContainer.FindOrAdd( acceptableList( i ) );
				npcID = pStaticActorInMap->_ActorID;//pNpcInMapInfo->TemplateID;
			}
		}
	}//end of loop for all npc

	return curObjIdx;
}

//============================================================================================================
//get all NPCs in the current area
void MMOMapDataManager::InitMinimapStaticActorsContainer()
{
	_MinimapStaticActorsContainer.Empty();
	//area shown by the current map
	FInMapRegion *pCurReginInMap = _InCurMapRegion;
	if( pCurReginInMap == NULL ){
		return;
	}
	FBox regionBox = pCurReginInMap->BoundingBox;
	INT curMapID = pCurReginInMap->MapID;
	//table of all NPCs in the current scene
	//
	INT cnt = _ListCurMapUIStaticActors.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI &uiActorPointInfo = _ListCurMapUIStaticActors( i );

		if( curMapID != uiActorPointInfo._InMapID ){
			if( uiActorPointInfo._IsValid == TRUE ){
				uiActorPointInfo._IsValid = FALSE;
				uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_MiniMap;
				uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_PathMap;
			}
			continue;
		}

		UBOOL isInRegion = regionBox.IsInside( uiActorPointInfo._OrigInMapPos );
		if( isInRegion == FALSE && uiActorPointInfo._IsValid == TRUE ){
		//if( isInRegion != uiActorPointInfo._IsValid ){
			//uiActorPointInfo._IsValid = isInRegion;
			uiActorPointInfo._IsValid = FALSE;
			uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_MiniMap;
			uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_PathMap;
			continue;
		}

		//uiActorPointInfo._IsValid = TRUE;
		uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_MiniMap;
		uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_PathMap;
		_MinimapStaticActorsContainer.Push( &uiActorPointInfo );
	}
}

//============================================================================================================
void MMOMapDataManager::InitWorldMapStaticActorsContainer( INT inRegionIdx )
{
	_WorldMapStaticActorsContainer.Empty();
	//area shown by the current world map
	INT sceneServerID = GetCurWorldMap().ServerID;
	FInMapRegion *pCurReginInMap = GetReginInMap( sceneServerID, inRegionIdx );
	if( pCurReginInMap == NULL ){
		return;
	}
	FBox regionBox = pCurReginInMap->BoundingBox;

	INT cnt = _ListWorldMapUIStaticActors.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI &uiActorPointInfo = _ListWorldMapUIStaticActors( i );

		if( sceneServerID != uiActorPointInfo._InMapID ){
			if( uiActorPointInfo._IsValid == TRUE ){
				uiActorPointInfo._IsValid = FALSE;
				uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_WorldMap;
			}
			continue;
		}

		UBOOL isInRegion = regionBox.IsInside( uiActorPointInfo._OrigInMapPos );
		if( isInRegion == FALSE ){
			uiActorPointInfo._IsValid = FALSE;
			uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_WorldMap;
			continue;
		}

		//uiActorPointInfo._IsValid = TRUE;
		uiActorPointInfo._NeedUpdateFlag |= EACTUIUF_WorldMap;
		_WorldMapStaticActorsContainer.Push( &uiActorPointInfo );
	}
}
//------------------------------------------------------------------------------------
//get all NPCs in the current area
void MMOMapDataManager::InitWorldMapPuttedUsefulNpcs()
{
	_WorldMapPuttedUsefulNpcContainer.Empty();
	//area shown by the current world map
	INT sceneServerID = GetCurWorldMap().ServerID;
	FInMapRegion *pCurReginInMap = GetReginInMap( sceneServerID, _InWorldmapRegionIdx );
	if( pCurReginInMap == NULL ){
		return;
	}
	FBox regionBox = pCurReginInMap->BoundingBox;

	//table of all NPCs in the current scene
	for( NPCsMarkedInMapContainer::TIterator it(_ScenePuttedUsefulNpcContainer); it; ++it ){
		FActorInMapUI &npcPointInfo = _ListWorldMapUIStaticActors( it.Value() );

		UBOOL isInRegion = regionBox.IsInside( npcPointInfo._OrigInMapPos );
		if( isInRegion == TRUE ){
			_WorldMapPuttedUsefulNpcContainer.FindOrAdd( it.Key() ) = it.Value();
		}
	}
}
//------------------------------------------------------------------------------------
//classify all static points in the current area map
void MMOMapDataManager::ClassifyWorldMapStaicMarks()
{
	_WorldMapPuttedFBContainer.Empty();
	_WorldMapNavPointsContainer.Empty();
	_WorldMapPuttedNpcGroupsContainer.Empty();
	//
	const TArray<FActorInMapUI*> worldMapUIStaticActors = GetStaticActorsInWorldmap();
	INT cnt = worldMapUIStaticActors.Num();
	for( INT i = 0; i < cnt; ++i ){
		const FActorInMapUI *staticPointInfo = worldMapUIStaticActors( i );
		if( staticPointInfo == NULL ){
			continue;
		}

		if( !staticPointInfo->_IsValid ){
			continue;
		}

		switch( staticPointInfo->_Type )
		{
		case EACTMUI_TeleportFB:
			_WorldMapPuttedFBContainer.FindOrAdd( staticPointInfo->_ActorID ) = staticPointInfo->_ID;
			break;
		case EACTMUI_NpcGroup:
			_WorldMapPuttedNpcGroupsContainer.FindOrAdd( staticPointInfo->_ActorID ) = staticPointInfo->_ID;
			break;
		case EACTMUI_NavPoint:
			_WorldMapNavPointsContainer.FindOrAdd( staticPointInfo->_ActorID ) = staticPointInfo->_ID;
			break;
		}
		
	}
}
//------------------------------------------------------------------------------------
//get all FBs in the current area
void MMOMapDataManager::InitWorldMapPuttedFBs()
{
	_WorldMapPuttedFBContainer.Empty();

	//
	const TArray<FActorInMapUI*> worldMapUIStaticActors = GetStaticActorsInWorldmap();
	INT cnt = worldMapUIStaticActors.Num();
	for( INT i = 0; i < cnt; ++i ){
		const FActorInMapUI *fbPointInfo = worldMapUIStaticActors( i );
		if( fbPointInfo == NULL ){
			continue;
		}

		if( !fbPointInfo->_IsValid || fbPointInfo->_Type != EACTMUI_TeleportFB ){
			continue;
		}

		_WorldMapPuttedFBContainer.FindOrAdd( fbPointInfo->_ActorID ) = fbPointInfo->_ID;
	}
}
//------------------------------------------------------------------------------------
//get all FBs in the current area
void MMOMapDataManager::InitWorldMapNavPoints()
{
	_WorldMapNavPointsContainer.Empty();

	//
	const TArray<FActorInMapUI*> worldMapUIStaticActors = GetStaticActorsInWorldmap();
	INT cnt = worldMapUIStaticActors.Num();
	for( INT i = 0; i < cnt; ++i ){
		const FActorInMapUI *navPointInfo = worldMapUIStaticActors( i );
		if( navPointInfo == NULL ){
			continue;
		}

		if( !navPointInfo->_IsValid || navPointInfo->_Type != EACTMUI_NavPoint ){
			continue;
		}

		_WorldMapNavPointsContainer.FindOrAdd( navPointInfo->_ActorID ) = navPointInfo->_ID;
	}
}
//----------------------------------------------------------------------------------------
//get all monster points in the current area
void MMOMapDataManager::InitWorldMapPuttedNpcGroups()
{
	_WorldMapPuttedNpcGroupsContainer.Empty();

	//
	const TArray<FActorInMapUI*> worldMapUIStaticActors = GetStaticActorsInWorldmap();
	INT cnt = worldMapUIStaticActors.Num();
	for( INT i = 0; i < cnt; ++i ){
		const FActorInMapUI *npcGroupPointInfo = worldMapUIStaticActors( i );
		if( npcGroupPointInfo == NULL ){
			continue;
		}

		if( !npcGroupPointInfo->_IsValid || npcGroupPointInfo->_Type != EACTMUI_NpcGroup ){
			continue;
		}

		_WorldMapPuttedNpcGroupsContainer.FindOrAdd( npcGroupPointInfo->_ActorID ) = npcGroupPointInfo->_ID;
	}
}
//============================================================================================================
//
int MMOMapDataManager::PutNpcGroupsToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container )
{
	const TArray<FNPCGroupInfo> *pNpcGroups = GetNpcGroupsInMap( sceneServerID );
	if( pNpcGroups == NULL ){
		return curObjIdx;
	}

	FString hintString[ENpcGroupType_MAX];
	hintString[ENpcGroupType_Boss] = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.NpcGroupType1") );
	hintString[ENpcGroupType_Gold] = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.NpcGroupType2") );
	hintString[ENpcGroupType_Silver] = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WorldMap.NpcGroupType3") );
	const TArray<FNPCGroupInfo> &npcGroups = *pNpcGroups;
	INT cntAllNpcGroup = npcGroups.Num();
	INT cntAllObjInMap = container.Num();
	for( INT i = 0; i < cntAllNpcGroup; ++i ){
		const FNPCGroupInfo &groupNpcInfo = npcGroups(i);

		//add more if not enough
		if( curObjIdx >= cntAllObjInMap ){
			FActorInMapUI newStaticActorInMap;
			container.Push( newStaticActorInMap );
			//recompute the total
			cntAllObjInMap = container.Num();
		}
		//
		FActorInMapUI *pStaticActorInMap = &container( curObjIdx );

		pStaticActorInMap->_Type = EACTMUI_NpcGroup;
		pStaticActorInMap->_ToggleFlag = EACTUIF_None;
		pStaticActorInMap->_IsValid = TRUE;
		pStaticActorInMap->_NeedUpdateFlag = EACTUIUF_WorldMap;
		pStaticActorInMap->_ID = curObjIdx;

		pStaticActorInMap->_ActorID = i;
		pStaticActorInMap->_InMapID = groupNpcInfo.MapID;
		pStaticActorInMap->_OrigInMapPos = groupNpcInfo.Position;

		pStaticActorInMap->_DisplayName = TEXT("");
		//if( 0 <= groupNpcInfo.GroupType && groupNpcInfo.GroupType < ENpcGroupType_MAX ){
		if( groupNpcInfo.GroupType < ENpcGroupType_MAX ){
			pStaticActorInMap->_DisplayName = hintString[groupNpcInfo.GroupType];
			pStaticActorInMap->_DisplayName += TEXT(":");
		}
		pStaticActorInMap->_DisplayName += groupNpcInfo.DisplayName;

		//switch the icon
		switch( groupNpcInfo.GroupType )
		{
		case ENpcGroupType_Boss:
			pStaticActorInMap->_UiIconIdx = TEXT("monsterGp1");
			break;
		case ENpcGroupType_Gold:
			pStaticActorInMap->_UiIconIdx = TEXT("monsterGp2");
			break;
		case ENpcGroupType_Silver:
			pStaticActorInMap->_UiIconIdx = TEXT("monsterGp3");
			break;
		default:
			pStaticActorInMap->_UiIconIdx = TEXT("npc");
			break;
		}

		//
		++curObjIdx;
	}

	return curObjIdx;
}
//============================================================================================================
//
int MMOMapDataManager::PutSpeciZoneToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container )
{
	const TArray<FQuestVolumeInfo> *pZones = GetSpeciZoneInMap( sceneServerID );
	if( pZones == NULL ){
		return curObjIdx;
	}

	const TArray<FQuestVolumeInfo> &zonesData = *pZones;
	INT cntAllZones = zonesData.Num();
	INT cntAllObjInMap = container.Num();
	for( INT i = 0; i < cntAllZones; ++i ){
		const FQuestVolumeInfo &zoneInfo = zonesData(i);
		
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(zoneInfo.Quests(0));
		if (!quest)
		{
			continue;
		}

		//only count emergency quests
		if (quest->QuestType != E_qt_Emegency)
		{
			continue;
		}

		//add more if not enough
		if( curObjIdx >= cntAllObjInMap ){
			FActorInMapUI newStaticActorInMap;
			container.Push( newStaticActorInMap );
			//recompute the total
			cntAllObjInMap = container.Num();
		}
		//
		FActorInMapUI *pStaticActorInMap = &container( curObjIdx );
		
		pStaticActorInMap->_ToggleFlag = EACTUIF_None;

		// fix bug  #23493
		UBOOL isAvailable = MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID());
		UBOOL isAccepted = MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID());
		UBOOL isCompleteAll = MMOQuestData::getSingleton().IsEmegencyQuestAllCompleteToday();

		pStaticActorInMap->_IsValid = (isAvailable || isAccepted) && !isCompleteAll;
		pStaticActorInMap->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		pStaticActorInMap->_ID = curObjIdx;

		pStaticActorInMap->_ActorID = i;
		pStaticActorInMap->_InMapID = zoneInfo.MapID;
		pStaticActorInMap->_OrigInMapPos = zoneInfo.Position;

		pStaticActorInMap->_DisplayName = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.MapQuestHintText1") );

		pStaticActorInMap->_Type = EACTMUI_QuestZone;
		pStaticActorInMap->_UiIconIdx = TEXT("questZone");

		//UBOOL isQuestCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());
		//UBOOL isQuestCompleted = MMOQuestData::getSingleton().IsQuestCompleted(quest->GetTemplateID());
		
		pStaticActorInMap->_ToggleFlag = EACTUIF_TaskNpc;

		//
		++curObjIdx;
	}

	return curObjIdx;
}
//============================================================================================================
//
int MMOMapDataManager::PutNavPointsToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container )
{
	const TArray<FNaviPointInfo> *pMapNavPoints = GetNavPointsInMap( sceneServerID );
	if( pMapNavPoints == NULL ){
		return curObjIdx;
	}

	//_WorldMapNavPointsContainer.Empty();

	const TArray<FNaviPointInfo> &mapNavPoints = *pMapNavPoints;
	INT cntAllNavPoints = mapNavPoints.Num();
	INT cntAllObjInMap = container.Num();
	for( INT i = 0; i < cntAllNavPoints; ++i ){
		const FNaviPointInfo &navPointInfo = mapNavPoints(i);
		if( !navPointInfo.IsShowOnUI ){
			continue;
		}

		//add more if not enough
		if( curObjIdx >= cntAllObjInMap ){
			FActorInMapUI newStaticActorInMap;
			container.Push( newStaticActorInMap );
			//recompute the total
			cntAllObjInMap = container.Num();
		}
		//
		FActorInMapUI *pStaticActorInMap = &container( curObjIdx );

		pStaticActorInMap->_ToggleFlag = EACTUIF_None;

		pStaticActorInMap->_IsValid = TRUE;
		pStaticActorInMap->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		pStaticActorInMap->_ID = curObjIdx;

		pStaticActorInMap->_ActorID = i;
		pStaticActorInMap->_InMapID = sceneServerID;//navPointInfo.MapID;
		pStaticActorInMap->_OrigInMapPos = navPointInfo.Position;

		pStaticActorInMap->_DisplayName = navPointInfo.DisplayName;

		pStaticActorInMap->_Type = EACTMUI_NavPoint;
		pStaticActorInMap->_UiIconIdx = appItoa( navPointInfo.IsShowOnUI );
		pStaticActorInMap->_UiIconIdx += navPointInfo.DisplayName;
		//
		++curObjIdx;

		//
		//_WorldMapNavPointsContainer.FindOrAdd( pStaticActorInMap->_ActorID ) = curObjIdx;
	}

	return curObjIdx;
}
//============================================================================================================
int MMOMapDataManager::PutTelportToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container )
{
	const TArray< FTeleportObject > *pTeleportsList;
	UGlobalMapInfo *pGlobalMapInfo = UGlobalMapInfo::GetSingleton();
	FExportMap origTeleportMapInfo = pGlobalMapInfo->FindMapInfo( sceneServerID );
	FString mapName = origTeleportMapInfo.MapName;
	pTeleportsList = pGlobalMapInfo->FindAllOrigInMap( mapName );

	if( pTeleportsList == NULL ){
		return curObjIdx;
	}

	
	const TArray< FTeleportObject > &teleportsList = *pTeleportsList;
	FActorInMapUI *pStaticActorInMap = NULL;
	INT cntAllObjInMap = container.Num();
	INT teleportCnt = pTeleportsList->Num();
	for( INT i = 0; i < teleportCnt; ++i ){
		const FTeleportObject *pTeleport = &teleportsList(i);
		
		//whether this jump point leads to a dungeon
		TArray<FTeleportLink*> teleLinkList;
		int LnkCnt = pGlobalMapInfo->FindLinkByOrigID( &teleLinkList, mapName, pTeleport->ServerID );
		if( LnkCnt <= 0 ){
			warnf( TEXT( "!!!teleport Orig %s has no teleport target"), *pTeleport->Name );
			continue;
		}

		//add more if not enough
		if( curObjIdx >= cntAllObjInMap ){
			FActorInMapUI newStaticActorInMap;
			container.Push( newStaticActorInMap );
			//recompute the total
			cntAllObjInMap = container.Num();
		}
		//
		pStaticActorInMap = &container( curObjIdx );

		pStaticActorInMap->_ToggleFlag = EACTUIF_None;
		pStaticActorInMap->_IsValid = TRUE;
		pStaticActorInMap->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		pStaticActorInMap->_ID = curObjIdx;
		pStaticActorInMap->_DisplayName = pTeleport->DisplayName;
		++curObjIdx;

		pStaticActorInMap->_ActorID = pTeleport->ServerID;//this ID is unique within a scene, not unique across all game scenes
		pStaticActorInMap->_OrigInMapPos = pTeleport->Location;
		pStaticActorInMap->_InMapID = sceneServerID;
		//
		FExportMap mapInfo	= pGlobalMapInfo->FindMapInfoByName( teleLinkList(0)->ToMapName );
		if( mapInfo.ServerID == origTeleportMapInfo.ServerID ){
			//jump within the map
			pStaticActorInMap->_Type = EACTMUI_TeleportInDoor;
			pStaticActorInMap->_UiIconIdx = TEXT("toInDoor");
		}
		else{
			if( mapInfo.ServerMapType == E_est_Dungeon ){
				pStaticActorInMap->_Type = EACTMUI_TeleportFB;
				pStaticActorInMap->_UiIconIdx = TEXT("entrance");
			}
			else if( mapInfo.ServerMapType == E_est_Static || mapInfo.ServerMapType == E_est_Line ){
					pStaticActorInMap->_Type = EACTMUI_TeleportMap;
					pStaticActorInMap->_UiIconIdx = TEXT("exit");
			}
			else{
				pStaticActorInMap->_Type = EACTMUI_TeleportMap;
				pStaticActorInMap->_UiIconIdx = TEXT("doorUnavailable");
			}
		}
	}

	return curObjIdx;
}

//
int MMOMapDataManager::PutTelportNodeToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container )
{
	TArray<FTeleportNetNodeData*> teleportNodeList;
	GetTeleportNetNodesIn( sceneServerID, &teleportNodeList );
	
	FActorInMapUI *pStaticActorInMap = NULL;
	INT cntAllObjInMap = container.Num();
	INT teleportNodeCnt = teleportNodeList.Num();
	for( INT i = 0; i < teleportNodeCnt; ++i ){
		FTeleportNetNodeData *pTNode = teleportNodeList(i);

		//add more if not enough
		if( curObjIdx >= cntAllObjInMap ){
			FActorInMapUI newStaticActorInMap;
			container.Push( newStaticActorInMap );
			//recompute the total
			cntAllObjInMap = container.Num();
		}
		//
		pStaticActorInMap = &container( curObjIdx );

		pStaticActorInMap->_Type = EACTMUI_TeleportNode;
		pStaticActorInMap->_ToggleFlag = EACTUIF_None;
		pStaticActorInMap->_IsValid = TRUE;
		pStaticActorInMap->_NeedUpdateFlag = EACTUIUF_UpdateAll;
		pStaticActorInMap->_ID = curObjIdx;
		pStaticActorInMap->_DisplayName = pTNode->_NodeTemplate->DisplayName;
		++curObjIdx;

		pStaticActorInMap->_ActorID = pTNode->_NodeTemplate->ServerID;
		pStaticActorInMap->_OrigInMapPos = pTNode->_NodeTemplate->Location;
		pStaticActorInMap->_InMapID = sceneServerID;

		//switch the icon
		pStaticActorInMap->_UiIconIdx = TEXT("teleportNode");
	}

	return curObjIdx;
}
//============================================================================================================
void MMOMapDataManager::AddWorldMapMarkInfo( FVector real3DPos, const FString &info )
{
	UBOOL isFoundReuse = FALSE;
	INT cnt = _ListWorldMapUIMarksHintInfo.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI &worldMapMarkInfo = _ListWorldMapUIMarksHintInfo( i );

		if( worldMapMarkInfo._IsValid == FALSE ){
			worldMapMarkInfo._OrigInMapPos = real3DPos;
			worldMapMarkInfo._DisplayName = info;
			worldMapMarkInfo._Type = EACTMUI_ExtaInfo;

			worldMapMarkInfo._IsValid = TRUE;
			worldMapMarkInfo._NeedUpdateFlag |= EACTUIUF_WorldMap;

			isFoundReuse = TRUE;
			break;
		}
	}

	if( isFoundReuse == FALSE ){
		FActorInMapUI newMarkInfo;
		newMarkInfo._OrigInMapPos = real3DPos;
		newMarkInfo._DisplayName = info;
		newMarkInfo._ID = cnt;
		newMarkInfo._Type = EACTMUI_ExtaInfo;

		newMarkInfo._IsValid = TRUE;
		newMarkInfo._NeedUpdateFlag |= EACTUIUF_WorldMap;

		_ListWorldMapUIMarksHintInfo.Push( newMarkInfo );
	}
}
//------------------------------------------------------------------------------------
void MMOMapDataManager::InvalidAllWorldMapMarkInfo()
{
	INT cnt = _ListWorldMapUIMarksHintInfo.Num();
	for( INT i = 0; i < cnt; ++i ){
		FActorInMapUI &worldMapMarkInfo = _ListWorldMapUIMarksHintInfo( i );
		worldMapMarkInfo._IsValid = FALSE;
		worldMapMarkInfo._NeedUpdateFlag |= EACTUIUF_WorldMap;
	}
}

//==========================================================================================================
//
const TArray<FNPCInstanceInfo*> *MMOMapDataManager::GetNpcInstanceInMap( INT sceneID )
{
	UBOOL isDataFound = ( _AllMapNpcContainer.Find( sceneID ) != NULL );
	TArray<FNPCInstanceInfo*> &mapNpcList = _AllMapNpcContainer.FindOrAdd( sceneID );

	//if not found before, find it
	if( !isDataFound ){
		UGlobalMapInfo::GetSingleton()->GetNpcInstanceInMap( sceneID, mapNpcList );
	}

	return &mapNpcList;
}

//==========================================================================================================
//
const TArray<FNPCGroupInfo> *MMOMapDataManager::GetNpcGroupsInMap( INT sceneID )
{
	return UGlobalMapInfo::GetSingleton()->GetNpcGroupsInMap( sceneID );
}
//==========================================================================================================
//
const TArray<FQuestVolumeInfo> *MMOMapDataManager::GetSpeciZoneInMap( INT sceneID )
{
	return UGlobalMapInfo::GetSingleton()->GetQuestVolumesInMap( sceneID );
}
//==========================================================================================================
//
const TArray<FNaviPointInfo> *MMOMapDataManager::GetNavPointsInMap( INT sceneID )
{
	return UGlobalMapInfo::GetSingleton()->GetAllNavPointsInMap( sceneID );
}
//============================================================================================================
//pathfinding path
UINT MMOMapDataManager::GetCurSceneWorldMapPathInterval()
{
	UINT interval = 600;
	FInMapRegion *curRegion = GetCurReginInCurMap();
	if( curRegion != NULL ){
		//UINT pixelPerStep = (UINT)( MAP_SNAPSHOT_COORDINATE_STEP / curRegion->WorldMapPixelSize );
		//interval = MAP_PATH_POINT_INTERVAL_FACTOR * MAP_SNAPSHOT_COORDINATE_STEP / curRegion->WorldMapScale / pixelPerStep;
		interval = MAP_PATH_POINT_INTERVAL_FACTOR * curRegion->WorldMapPixelSize;
		//interval /= MMOMapImgTiles::GetMapImgTiles(E_WORLDMAP_TILES)->_ZoomFactor;
	}

	return interval;
}
TArray<FVector> MMOMapDataManager::GetCurMainPlayerPath( UINT interval, UBOOL is2D )
{
	INT POSITION_NEAR = 30;
	INT LAST_POSITION_NEAR = 100;
	//static UINT degugThreshold = 300;
	if( interval == 0 ){
		interval = 600;
	}

	TArray<FVector> rstPath;
	//
	AMMOPlayerController *mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayerCtrl == NULL || mainPlayer == NULL ){
		return rstPath;
	}
	//
	TArray<FVector> routingPath;
	routingPath.Push( mainPlayer->Location );

	FVector curFetchPos;
	INT32 pathPointIndex = gServerGridWorld.FetchArrayPointer() - 1;
	while( gServerGridWorld.FetchPath( curFetchPos, pathPointIndex++ ) ){
		routingPath.Push( curFetchPos );
	}

	//SET_WARN_COLOR(COLOR_CYAN);
	//for( INT i = 0; i < (INT)routingPath.Num() - 1; ++i ){
	//	FVector &curPos = routingPath( i );
	//	FVector &nxtPos = routingPath(i+1);
	//	warnf( NAME_Log, TEXT("======!!!!========= [CurMainPlayerPath: %dDis:%f, Pos:x=%f,y=%f]"), i, nxtPos.Distance( curPos ), curPos.X, curPos.Y );
	//}

	//routingPathPointCnt is certainly greater than or equal to 2
	TArray<FVector> routingPathModified;
	FVector &firstRoutingPathPos = routingPath(0);
	if( is2D ){
		firstRoutingPathPos.Z = 0;
	}
	routingPathModified.Push( firstRoutingPathPos );

	INT routingPathPointCnt = routingPath.Num();
	for( INT i = 0; i < routingPathPointCnt - 1; ++i ){
		FVector &posStart = routingPath(i);
		FVector &posEnd = routingPath(i + 1);
		if( is2D ){
			posStart.Z = 0;
			posEnd.Z = 0;
		}
		FVector pathVec = posEnd - posStart;
		UINT pathDis = pathVec.Size();
		//too short
		if( pathDis <= interval ){
			continue;
		}
		routingPathModified.Push( posEnd );
	}
	//routingPath certainly has at least 2 members; if only 2 points, record regardless of how short
	FVector &lastRoutingPathPos = routingPath.Last();
	if( lastRoutingPathPos.Distance( routingPathModified.Last() ) > POSITION_NEAR ){
		routingPathModified.Push( lastRoutingPathPos );
	}

	SET_WARN_COLOR(COLOR_CYAN);
	//
	INT posOffset = 0;
	INT leftPathDis = 0;
	FVector lastRoutingInverseVec = FVector(0);
	routingPathPointCnt = routingPathModified.Num();
	for( INT i = 0; i < routingPathPointCnt - 1; ++i ){
		FVector &posStart = routingPathModified(i);
		FVector &posEnd = routingPathModified(i + 1);

		FVector pathVec = posEnd - posStart;
		INT origPathDis = pathVec.Size();
		if( origPathDis < 0 ){
			warnf( NAME_Log, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath path segment is too LONG]") );
			continue;
		}
		pathVec.Normalize();

		//
		INT curPathDis = origPathDis;
		if( leftPathDis > 0 ){
			FLOAT minusB = leftPathDis * ( pathVec | lastRoutingInverseVec );
			FLOAT minusACSquare = interval * interval - leftPathDis * leftPathDis;
			posOffset = INT( minusB + appSqrt( minusB * minusB + minusACSquare ) );
			if( posOffset < 0 ){
				warnf(NAME_Log, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath posOffset is minus]") );
			}
			//
			if( posOffset <= curPathDis ){
				rstPath.Push( posStart + pathVec * posOffset );
#if DEBUG_DRAW_NAVMESH_PATH
				if( rstPath.Num() >= 2  ){
					UINT debugInt = rstPath.Last().Distance( rstPath( rstPath.Num() - 2 ) );
					if( debugInt < degugThreshold ){
						warnf(NAME_Warning, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath Debug]") );
					}
				}
#endif
				curPathDis -= posOffset;
			}
		}
		else{
			posOffset = 0;
		}

		//
		INT segCnt = ( curPathDis + interval ) / interval;
		//no need to mark the start point
		FVector segPos;
		for( INT j = 1; j < segCnt; ++j ){
			segPos = posStart + pathVec * ( posOffset + j * interval );
			rstPath.Push( segPos );
#if DEBUG_DRAW_NAVMESH_PATH
			if( rstPath.Num() >= 2  ){
				UINT debugInt = rstPath.Last().Distance( rstPath( rstPath.Num() - 2 ) );
				if( debugInt < degugThreshold ){
					warnf(NAME_Warning, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath Debug]") );
				}
			}
#endif
		}

		//navMeshLastPoint = segPos;

		//
		lastRoutingInverseVec = -pathVec;
		if( segCnt >= 1 ){
			leftPathDis = curPathDis - ( segCnt - 1 ) * interval;
		}
		else{
			warnf(NAME_Log, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath [( curPathDis + interval ) / interval] is less than 1]") );
			leftPathDis = 0;
		}

		//
		INT compareToStandard = (INT)interval - leftPathDis;
		if( compareToStandard <= POSITION_NEAR && compareToStandard > 0 ){
			//slightly shorter than interval, to be just right
			rstPath.Push( posEnd );
#if DEBUG_DRAW_NAVMESH_PATH
			if( rstPath.Num() >= 2  ){
				UINT debugInt = rstPath.Last().Distance( rstPath( rstPath.Num() - 2 ) );
				if( debugInt < degugThreshold ){
					warnf(NAME_Warning, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath Debug]") );
				}
			}
#endif
			leftPathDis = 0;
			//navMeshLastPoint = posEnd;
		}

		if( leftPathDis < POSITION_NEAR ){
			//slightly over, to avoid overshooting
			leftPathDis = 0;
		}
	}
	//the last one may not have been processed yet
	if( routingPathPointCnt >= 1 && leftPathDis > INT(interval / 6) ){
		rstPath.Push( routingPathModified.Last() );
	}

	return rstPath;
}
#if 0
TArray<FVector> MMOMapDataManager::GetCurMainPlayerPath( UINT interval, UBOOL is2D )
{
	INT POSITION_NEAR = 30;
	INT LAST_POSITION_NEAR = 100;
	//static UINT degugThreshold = 300;
	if( interval == 0 ){
		interval = 600;
	}

	TArray<FVector> rstPath;
	//
	AMMOPlayerController *mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayerCtrl == NULL || mainPlayerCtrl->m_NavHelper == NULL || mainPlayerCtrl->m_NavHelper->_NavigationHandle == NULL || mainPlayer == NULL ){
		return rstPath;
	}
	//
	FVector destPos = *mainPlayerCtrl->m_NavHelper->_NavigationHandle->FinalDestination;
	FVector oriPos = mainPlayerCtrl->Pawn->Location;
	if( is2D ){
		destPos.Z = 0;
		oriPos.Z = 0;
	}

	//
	//TArray<FVector> routingPath;
	//routingPath.Push( oriPos );

	//routingPath.Append( mainPlayerCtrl->m_NavHelper->_NavigationHandle->MovePoints );

	TArray<FVector> routingPath;
	mainPlayerCtrl->m_NavHelper->_NavigationHandle->CopyMovePointsFromPathCache( destPos, routingPath );
	routingPath.InsertItem( oriPos, 0 );

	UBOOL isNavigating = MMONavigationManager::getSingleton().IsNavigating();
	if( !isNavigating ){
		routingPath.Push( oriPos );
	}
	else{
		routingPath.Push( destPos );
	}


	//routingPathPointCnt is certainly greater than or equal to 2
	TArray<FVector> routingPathModified;
	FVector &firstRoutingPathPos = routingPath(0);
	routingPathModified.Push( firstRoutingPathPos );

	INT routingPathPointCnt = routingPath.Num();
	for( INT i = 0; i < routingPathPointCnt - 1; ++i ){
		FVector &posStart = routingPath(i);
		FVector &posEnd = routingPath(i + 1);
		if( is2D ){
			posStart.Z = 0;
			posEnd.Z = 0;
		}
		FVector pathVec = posEnd - posStart;
		UINT pathDis = pathVec.Size();
		//too short
		if( pathDis <= interval ){
			continue;
		}
		routingPathModified.Push( posEnd );
	}
	//routingPath certainly has at least 2 members; if only 2 points, record regardless of how short
	FVector &lastRoutingPathPos = routingPath.Last();
	if( lastRoutingPathPos.Distance( routingPathModified.Last() ) > POSITION_NEAR ){
		routingPathModified.Push( lastRoutingPathPos );
	}

	////test
	//routingPathModified.Empty();
	//routingPathModified.Push( FVector( 12305.418, 17325.670, 0 ) );
	//routingPathModified.Push( FVector( 12781.985, 16804.598, 0 ) );
	//routingPathModified.Push( FVector( 13536.242, 16006.384, 0 ) );
	//routingPathModified.Push( FVector( 13888.000, 15360.000, 0 ) );
	//routingPathModified.Push( FVector( 15104.000, 14784.000, 0 ) );
	//routingPathModified.Push( FVector( 16192.000, 14784.000, 0 ) );
	//routingPathModified.Push( FVector( 17280.000, 14784.000, 0 ) );
	//routingPathModified.Push( FVector( 17280.000, 13760.000, 0 ) );
	//routingPathModified.Push( FVector( 16896.000, 11904.000, 0 ) );
	//routingPathModified.Push( FVector( 17425.713, 10664.317, 0 ) );
	//routingPathModified.Push( FVector( 17425.713, 10664.317, 0 ) );

	//
	INT posOffset = 0;
	INT leftPathDis = 0;
	FVector lastRoutingInverseVec = FVector(0);
	routingPathPointCnt = routingPathModified.Num();
	for( INT i = 0; i < routingPathPointCnt - 1; ++i ){
		FVector &posStart = routingPathModified(i);
		FVector &posEnd = routingPathModified(i + 1);

		FVector pathVec = posEnd - posStart;
		INT origPathDis = pathVec.Size();
		if( origPathDis < 0 ){
			warnf(NAME_Error, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath path segment is too LONG]") );
			continue;
		}
		pathVec.Normalize();

		//
		INT curPathDis = origPathDis;
		if( leftPathDis > 0 ){
			FLOAT minusB = leftPathDis * ( pathVec | lastRoutingInverseVec );
			FLOAT minusACSquare = interval * interval - leftPathDis * leftPathDis;
			posOffset = INT( minusB + appSqrt( minusB * minusB + minusACSquare ) );
			if( posOffset < 0 ){
				warnf(NAME_Error, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath posOffset is minus]") );
			}
			//
			if( posOffset <= curPathDis ){
				rstPath.Push( posStart + pathVec * posOffset );
#if DEBUG_DRAW_NAVMESH_PATH
				if( rstPath.Num() >= 2  ){
					UINT debugInt = rstPath.Last().Distance( rstPath( rstPath.Num() - 2 ) );
					if( debugInt < degugThreshold ){
						warnf(NAME_Warning, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath Debug]") );
					}
				}
#endif
				curPathDis -= posOffset;
			}
		}
		else{
			posOffset = 0;
		}

		//
		INT segCnt = ( curPathDis + interval ) / interval;
		//no need to mark the start point
		FVector segPos;
		for( INT j = 1; j < segCnt; ++j ){
			segPos = posStart + pathVec * ( posOffset + j * interval );
			rstPath.Push( segPos );
#if DEBUG_DRAW_NAVMESH_PATH
			if( rstPath.Num() >= 2  ){
				UINT debugInt = rstPath.Last().Distance( rstPath( rstPath.Num() - 2 ) );
				if( debugInt < degugThreshold ){
					warnf(NAME_Warning, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath Debug]") );
				}
			}
#endif
		}

		//navMeshLastPoint = segPos;

		//
		lastRoutingInverseVec = -pathVec;
		if( segCnt >= 1 ){
			leftPathDis = curPathDis - ( segCnt - 1 ) * interval;
		}
		else{
			warnf(NAME_Error, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath [( curPathDis + interval ) / interval] is less than 1]") );
			leftPathDis = 0;
		}

		//
		INT compareToStandard = (INT)interval - leftPathDis;
		if( compareToStandard <= POSITION_NEAR && compareToStandard > 0 ){
			//slightly shorter than interval, to be just right
			rstPath.Push( posEnd );
#if DEBUG_DRAW_NAVMESH_PATH
			if( rstPath.Num() >= 2  ){
				UINT debugInt = rstPath.Last().Distance( rstPath( rstPath.Num() - 2 ) );
				if( debugInt < degugThreshold ){
					warnf(NAME_Warning, TEXT("======!!!!======= [MMOMapDataManager::GetCurMainPlayerPath Debug]") );
				}
			}
#endif
			leftPathDis = 0;
			//navMeshLastPoint = posEnd;
		}

		if( leftPathDis < POSITION_NEAR ){
			//slightly over, to avoid overshooting
			leftPathDis = 0;
		}
	}
	//the last one may not have been processed yet
	if( routingPathPointCnt >= 1 && leftPathDis > INT(interval / 6) ){
		rstPath.Push( routingPathModified.Last() );
	}

	return rstPath;
}
#endif

//
void MMOMapDataManager::SetMapPath( TArray< FVector > &pathList )
{
	//invalid all current
	InvalidCurPath();

	//UBOOL isShowInWorldMap = ( GetCurWorldMap().ServerID == GetCurMap().ServerID );

	INT posCnt = pathList.Num();
	if( posCnt <= 0 ){
		return;
	}
	

	INT curContainerSize = _MapCurPath.Num();
	FPathPointInMapUI *pPathPosUi;
	for( INT idx = 0; idx < posCnt; ++idx ){
		//add if not enough
		if( idx >= curContainerSize ){
			FPathPointInMapUI pathPosUi;
			_MapCurPath.Push( pathPosUi );
			//recompute the total
			curContainerSize = _MapCurPath.Num();
		}
		//
		pPathPosUi = &_MapCurPath( idx );

		pPathPosUi->_Type = EACTMUI_PathPoint;
		pPathPosUi->_IsValid = TRUE;
		pPathPosUi->_NeedUpdateFlag = EACTUIUF_MiniMap;
		pPathPosUi->_NeedUpdateFlag |= EACTUIUF_WorldMap;
		pPathPosUi->_ID = (UINT)idx;
		//pPathPosUi->_MapID = ;

		pPathPosUi->_OrigInMapPos = pathList(idx);
		//switch the icon
		pPathPosUi->_UiIconIdx = TEXT("pathPoint");
	}

	//path starting and destination point is different
	//_MapCurPath( 0 )._UiIconIdx = TEXT("pathStart");
	_MapCurPath( posCnt - 1 )._UiIconIdx = TEXT("pathEnd");


	//SET_WARN_COLOR(COLOR_CYAN);
	//for( INT i = 0; i < (INT)posCnt - 1; ++i ){
	//	FVector &curPos = pathList( i );
	//	FVector &nxtPos = pathList(i+1);
	//	warnf( NAME_Log, TEXT("======!!!!========= [MainPlayerPath: %dDis:%f, Pos:x=%f,y=%f]"), i, nxtPos.Distance( curPos ), curPos.X, curPos.Y );
	//}
	//for( INT i = 0; i < (INT)curContainerSize - 1; ++i ){
	//	FPathPointInMapUI &curPos = _MapCurPath( i );
	//	FPathPointInMapUI &nxtPos = _MapCurPath(i+1);
	//	if( nxtPos._IsValid ){
	//		warnf( NAME_Log, TEXT("======!!!!========= [MainPlayerPath: isVisible:%d, Dis:%f, Pos:x=%f,y=%f]"), curPos._IsValid, nxtPos._OrigInMapPos.Distance( curPos._OrigInMapPos ), curPos._OrigInMapPos.X, curPos._OrigInMapPos.Y );
	//	}
	//}
}

//
TArray< FPathPointInMapUI > &MMOMapDataManager::GetCurMapPath()
{
	return _MapCurPath;
}

//
void MMOMapDataManager::InvalidCurPath()
{
	UMMOUIWorldMap *pWorldMapPanel = ( UMMOUIWorldMap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WorldMap );

	INT posCnt = _MapCurPath.Num();
	FPathPointInMapUI *pPathPosUi = NULL;
	for( INT i = 0; i < posCnt; ++i ){
		pPathPosUi = &_MapCurPath(i);

		if( !pPathPosUi->_IsValid ){
			continue;
		}
		pPathPosUi->_IsValid = FALSE;

		//pPathPosUi->_NeedUpdateFlag = EACTUIUF_UpdateAll;

		//worldMap
		if( pPathPosUi->_UiObjInWorldMap != NULL ){
			pPathPosUi->_NeedUpdateFlag |= EACTUIUF_WorldMap;
			//pUiObj->SetVisible( FALSE );
		}
		////the world map's structures may be cleaned up when closed, these new'd objects cannot be kept, so destruct them directly here
		//if( pWorldMapPanel != NULL ){
		//	pWorldMapPanel->DistoryRootMc( pPathPosUi->_UiObjInWorldMap );
		//}

		//minimap
		if( pPathPosUi->_UiObjInMiniMap != NULL ){
			pPathPosUi->_NeedUpdateFlag |= EACTUIUF_MiniMap;
			//pUiObj->SetVisible( FALSE );
		}

		//PathMap
		if( pPathPosUi->_UiObjInPathMap != NULL ){
			pPathPosUi->_NeedUpdateFlag |= EACTUIUF_PathMap;
			//pUiObj->SetVisible( FALSE );
		}
	}
}

//=============================================================================
//UBOOL MMOMapDataManager::LoadDungonInfo()
//{
//	SCOPE_LOG_TIME(___INIT__, TEXT("MMOMapDataManager::LoadDungonInfo"), 0);
//
//	MMO::Facade &mmoFacade = MMOFacade::getSingleton();
//	TArray<TArray<FString>> ArrayString2D;
//	if( !mmoFacade.ReadConfigString( ArrayString2D, TEXT("dungeon.txt"), true) )
//	{
//		return FALSE;
//	}
//
//	_AllDungeonBaseInfo.Empty();
//	//remove the title row
//	UINT rowCnt = ArrayString2D.Num();
//	UINT idx = 0;
//	for( UINT i=0+1; i < rowCnt; ++i, ++idx )
//	{
//		TArray<FString>& ArrayString = ArrayString2D(i);
//
//		//
//		UINT len = ArrayString.Num();
//		if( len < 24 ){
//			warnf(NAME_Error, TEXT("======!!!!======= [LoadDungonInfo ERROR]AT %d"), i );
//			continue;
//		}
//
//		INT dataID = appAtoi( *ArrayString(0) );
//		MMO::DungeonInfo &data = _AllDungeonBaseInfo.FindOrAdd( dataID );
//		data._Difficulty = dataID % 10000;
//		data._MapID = dataID / 10000;
//		data._MinLevel = appAtoi( *ArrayString(2) );
//		data._MaxLevel = appAtoi( *ArrayString(3) );
//		data._ExpForVip = appAtoi( *ArrayString(8) );
//		data._ExpForTeam = appAtoi( *ArrayString(9) );
//		data._ExpForGuild = appAtoi( *ArrayString(10) );
//		data._ExpForFirst = appAtoi( *ArrayString(11) );
//
//		data._Reward1_free = appAtoi( *ArrayString(17) );
//		data._Reward1_cost = appAtoi( *ArrayString(20) );
//		data._Reward2_free = appAtoi( *ArrayString(18) );
//		data._Reward2_cost = appAtoi( *ArrayString(21) );
//		data._Reward3_free = appAtoi( *ArrayString(19) );
//		data._Reward3_cost = appAtoi( *ArrayString(22) );
//	}
//
//	return TRUE;
//}

//=============================================================================
const FDungeonInfo *MMOMapDataManager::GetDungeonInfo( UINT mapID, UINT diffculty )
{
	//INT id = mapID*10000 + diffculty;
	INT id = mapID;
	TArray<FDungeonInfo> &dungeonInfoList = UGlobalMapInfo::GetSingleton()->GetDungeonInfoList();
	INT dataCnt = dungeonInfoList.Num();
	UBOOL isFound = FALSE;
	for( INT i = 0 ; i < dataCnt; ++i ){
		if( dungeonInfoList(i).DungeonInfoID == id ){
			return &dungeonInfoList(i);
		}
	}

	return NULL;
}

//=============================================================================
INT MMOMapDataManager::GetDungeonInfoDifftyMask( UINT mapID )
{
	INT allTypeMaxDiff = 0;

	//TArray<FDungeonInfo> &dungeonInfoList = UGlobalMapInfo::GetSingleton()->GetDungeonInfoList();
	//INT dataCnt = dungeonInfoList.Num();
	//UBOOL isFound = FALSE;
	//for( INT i = 0 ; i < dataCnt; ++i ){
	//	const FDungeonInfo &info = dungeonInfoList(i);
	//	if( info.MapID == mapID && info.Difficulty >= esdNormal1 ){
	//		allTypeMaxDiff |= ( 1 << ( info.Difficulty - esdNormal1 ) );
	//	}
	//}

	return allTypeMaxDiff;
}
//=============================================================================
void MMOMapDataManager::SetCurSceneLineIn( INT lineID )
{ 
	_CurSceneLineIn = lineID;

	MMOGameEvent_Any evt( GameEvent_UpdateSceneLines );
	MMOUI::PostGameEvent(&evt);
}
//void MMOMapDataManager::RemoveOldCurMapLinew( INT lineID )
//{
//	INT lineCnt = _CurSceneLines.Num();
//	for( INT i = 0; i < lineCnt; ++i ){
//		if( _CurSceneLines(i) == lineID ){
//			_CurSceneLines.Remove( i );
//			break;
//		}
//	}
//	return _CurSceneLines; 
//}

//
void MMOMapDataManager::InitCurSceneProgress()
{
	_CurSceneProgress.Empty();
	INT mainProgressCnt = _CurSceneInfo.Progress.Num();
	for( INT i = 0; i < mainProgressCnt; ++i ){
		FSceneTarget &mainProgress = _CurSceneInfo.Progress(i);
		CurSceneTarget newSceneTarget;
		newSceneTarget._MainTarget._Template = &mainProgress.MainTarget;
		newSceneTarget._MainTarget._CurValue = 0;
		for( INT j = 0; j < UCONST_MaxSceneMinorTarget; ++j ){
			newSceneTarget._ChildTargets[j]._Template = &mainProgress.ChildTargets[j];
			newSceneTarget._ChildTargets[j]._CurValue = 0;
		}

		//
		_CurSceneProgress.Push( newSceneTarget );
	}
	
}

//--------------------------------------------------------------------------------------------
void MMOMapDataManager::InitFBConstAchievements()
{
	_FBConstAchievements.Empty();
	_FBConstAchievements.AddZeroed( EDgenCnstTgtTyp_Max );

	for( INT i = 0; i < EDgenCnstTgtTyp_Max; ++i ){
		FString nameLookUp = FString::Printf( TEXT("MMOGame.FBGeneral.FBAchievement%dTitle"), i+1 );
		FString descLookUp = FString::Printf( TEXT("MMOGame.FBGeneral.FBAchievement%dTips"), i+1 );
		FString typeLookUp = FString::Printf( TEXT("MMOGame.FBGeneral.FBAchievement%dType"), i+1 );
		//FString scoreTypeLookUp = FString::Printf( TEXT("MMOGame.FBGeneral.FBAchievement%dScoreType"), i );

		//_FBConstAchievements(i).TargetValue = appAtoi( MMOUI::ParseLocalizedPropertyPath( scoreTypeLookUp ) );
		_FBConstAchievements(i).ID = MAX_Dungeon_Target_ID_Start + i;
		_FBConstAchievements(i).TargetName = MMOUI::ParseLocalizedPropertyPath( nameLookUp );
		_FBConstAchievements(i).TargetDesc = MMOUI::ParseLocalizedPropertyPath( descLookUp );
		FString targetLevelString = MMOUI::ParseLocalizedPropertyPath( typeLookUp );
		if( targetLevelString.IsNumeric() ){
			_FBConstAchievements(i).TargetLevel = appAtoi( *targetLevelString );
		}
		else{
			_FBConstAchievements(i).TargetLevel = EFBAchivType_Invalid;
		}
		if( _FBConstAchievements(i).TargetLevel > EFBAchivType_Gold ){
			_FBConstAchievements(i).TargetLevel = EFBAchivType_Bronze;
		}
	}

	//
	_FBConstAchievements(EDgenCnstTgtTyp_TargetKilled).IsHighTargetValue = TRUE;
	_FBConstAchievements(EDgenCnstTgtTyp_SpecialKilled).IsHighTargetValue = TRUE;
	_FBConstAchievements(EDgenCnstTgtTyp_ShortTimeKill).IsHighTargetValue = FALSE;
	_FBConstAchievements(EDgenCnstTgtTyp_Undead).IsHighTargetValue = FALSE;
}

//----------------------------------------------------------------------------------------------------------------
void MMOMapDataManager::ParseDungeonCombatDatas( G2C_SYNC_COMBAT_STAT* serverMsg, UINT dataCnt )
{
	if( serverMsg == NULL ){
		return;
	}

	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( pMainPlayer == NULL ){
		return;
	}

	//initialize all data that needs to be recorded
	FExportMap &curDungeonMapData = GetCurMap();
	INT dungeonDifficulty = GetCurFBDiff();
	PrepareCurFBAchievementsScore( curDungeonMapData.ServerID, dungeonDifficulty );

	//
	for (UINT i = 0; i < dataCnt; ++i){

		G2C_SYNC_COMBAT_STAT::ROLE_STAT_DATA* pCombatData = ( G2C_SYNC_COMBAT_STAT::ROLE_STAT_DATA* )&serverMsg->DataList[i];
		if( pMainPlayer->ServerID != pCombatData->dwRoleID ){
			//only care about own data
			continue;
		}
		//
		_FBAchievementsData(EDgenCnstTgtTyp_TargetKilled)._score = -1;
		_FBAchievementsData(EDgenCnstTgtTyp_SpecialKilled)._score = -1;
		_FBAchievementsData(EDgenCnstTgtTyp_ShortTimeKill)._score = -1;
		_FBAchievementsData(EDgenCnstTgtTyp_Undead)._score = -1;
	}

	//refresh the UI data again, in case it arrived late
	UMMOUIFBSummary *pFBSummary = (UMMOUIFBSummary*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEndAll_FBSummary );
	if( pFBSummary != NULL && pFBSummary->IsVisible ){
		pFBSummary->UpdateFBAchievement();
	}
}

//================================================================================================================
void MMOMapDataManager::ParseDungeonFinishDatasForAchievement( G2C_DUNGEON_FINISH_DATA* serverMsg )
{
	if( serverMsg == NULL ){
		return;
	}

	TArray<FBAchievementsState> &curFBAchievScore = GetFBAchievementsScoreData();

	INT curFBAchievementCnt = curFBAchievScore.Num();
	UBOOL tooMany = FALSE;
	if( curFBAchievementCnt > MAX_Dungeon_Target ){
		warnf(NAME_Error, TEXT("======!!!!======= [MMOMapDataManager::ParseDungeonFinishDatasForAchievement curFBAchievementCnt is Too many[%d]"), curFBAchievementCnt );
		tooMany = TRUE;
	}

	INT idx = 0;
	for( INT i = ( tooMany ? MAX_Dungeon_Target - 1 : curFBAchievementCnt - 1 ); i >= 0; --i, ++idx ){
		FBAchievementsState &achiScore = curFBAchievScore(idx);

		const FDungeonTarget *achiTemplate = achiScore._achievementTemplate;
		if( achiTemplate == NULL ){
			continue;
		}
		//
		if( ( serverMsg->byGoalResult & ( 1 << i ) ) > 0 ){
			achiScore._score = achiTemplate->TargetValue;
		}
		else{
			if( achiTemplate->IsHighTargetValue ){
				if( achiScore._score >= achiTemplate->TargetValue ){
					achiScore._score = achiTemplate->TargetValue - 1;
				}
			}
			else{
				if( achiScore._score <= achiTemplate->TargetValue ){
					achiScore._score = achiTemplate->TargetValue + 1;
				}
			}
		}
	}
}
//================================================================================================================
FBAchievementsState *MMOMapDataManager::FindFBAchievementCurrentDungeon( INT fbAchieveID )
{
	TArray<FBAchievementsState> &curFBAchievScore = GetFBAchievementsScoreData();

	INT curFBAchievementCnt = curFBAchievScore.Num();
	INT idx = 1;
	for( UINT i = 0; idx <= curFBAchievementCnt; ++idx, ++i ){
		FBAchievementsState &achiScore = curFBAchievScore(i);

		const FDungeonTarget *achiTemplate = achiScore._achievementTemplate;
		if( achiTemplate == NULL ){
			continue;
		}

		//
		if( fbAchieveID == achiTemplate->ID ){
			return &achiScore;
		}
	}

	return NULL;
}
//----------------------------------------------------------------------------------------------------------------
void MMOMapDataManager::PrepareCurFBAchievementsScore( INT mapID, INT fbDifficult )
{
	_FBAchievementsData.Empty();
	_FBAchievementsData.AddZeroed( MAX_Dungeon_Target );

	FExportMap *mapSceneData = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( mapID );
	if( mapSceneData == NULL ){
		return;
	}

	const FDungeonInfo *fbInfo = GetDungeonInfo( mapSceneData->ServerID, fbDifficult );
	if( fbInfo == NULL ){
		return;
	}

	//prepare fixed rating point target values
	_FBConstAchievements(EDgenCnstTgtTyp_TargetKilled).TargetValue = fbInfo->StandardBossValue;
	_FBAchievementsData(EDgenCnstTgtTyp_TargetKilled)._achievementTemplate = &_FBConstAchievements(EDgenCnstTgtTyp_TargetKilled);
	_FBAchievementsData(EDgenCnstTgtTyp_TargetKilled)._score = 0;

	_FBConstAchievements(EDgenCnstTgtTyp_SpecialKilled).TargetValue = fbInfo->StandardEliteNpcValue;
	_FBAchievementsData(EDgenCnstTgtTyp_SpecialKilled)._achievementTemplate = &_FBConstAchievements(EDgenCnstTgtTyp_SpecialKilled);
	_FBAchievementsData(EDgenCnstTgtTyp_SpecialKilled)._score = 0;

	_FBConstAchievements(EDgenCnstTgtTyp_ShortTimeKill).TargetValue = fbInfo->StandardSwiftTime;
	_FBAchievementsData(EDgenCnstTgtTyp_ShortTimeKill)._achievementTemplate = &_FBConstAchievements(EDgenCnstTgtTyp_ShortTimeKill);
	_FBAchievementsData(EDgenCnstTgtTyp_ShortTimeKill)._score = 0;

	_FBConstAchievements(EDgenCnstTgtTyp_Undead).TargetValue = 0;
	_FBAchievementsData(EDgenCnstTgtTyp_Undead)._achievementTemplate = &_FBConstAchievements(EDgenCnstTgtTyp_Undead);
	_FBAchievementsData(EDgenCnstTgtTyp_Undead)._score = 0;

	INT curIdx = EDgenCnstTgtTyp_Max;

	//dynamic
	for( INT curFBTargetIdx = 0; curFBTargetIdx < UCONST_MaxDungeonCustomTarget && curIdx < MAX_Dungeon_Target; ++curFBTargetIdx, ++curIdx ){
		const FDungeonTarget *achiTemplate = &fbInfo->CustomTarget[curFBTargetIdx];
		if( achiTemplate->TargetValue >= 0 ){
			//has data, add to stats
			_FBAchievementsData(curIdx)._achievementTemplate = achiTemplate;
			_FBAchievementsData(curIdx)._score = -1;
		}
	}
}

//
//=========================================================================================
INT MMOMapDataManager::GetTimeSinceDungeonBegin()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	//Local battlefield start time
	__time32_t localStartServerTime = _DungeonStartUtcTime;
	_localtime32(&localStartServerTime);

	//Current local time
	SYSTEMTIME curTime;
	::GetLocalTime(&curTime);
	UINT32 curTimeSec = mmoFacade.ConvertSystemTimeToTmTSecond( curTime );

	//Time difference between local and server
	INT32 secDiff = mmoFacade.GetClientToSceneSecondDiff();

	warnf( NAME_Log, TEXT("GetTimeSinceDungeonBegin  curTime:H-%d,M-%d,S-%d"), curTime.wHour, curTime.wMinute, curTime.wSecond );
	warnf( NAME_Log, TEXT("GetTimeSinceDungeonBegin  curTimeSec:%d, localBattleStartServerTime:%d, secDiff:%d"), curTimeSec, localStartServerTime, secDiff );
	//
	INT32 secSinceStart = curTimeSec - ( localStartServerTime + secDiff );
	return secSinceStart;
}

//
INT MMOMapDataManager::GetMaxDiffOfType( INT mapID, INT type, INT allTypeMaxDiff )
{
	////allTypeMaxDiff represents the difficulty already cleared
	//INT maxDiff = esdInvalid;
	//INT startPos = esdNormal1;
	//if( type == EFBDTyp_OnePerson ){
	//	for( INT i = esdNormal1; i <= esdHeroic1; ++i ){
	//		if( allTypeMaxDiff & ( 1 << ( i - startPos ) ) ){
	//			maxDiff = i;
	//		}
	//	}
	//	//
	//	if( maxDiff == esdInvalid ){
	//		maxDiff = esdNormal1;
	//	}
	//	else if( maxDiff < esdHeroic1 ){
	//		++maxDiff;
	//	}
	//}
	//else if( type == EFBDTyp_FivePerson ){
	//	for( INT i = esdNormal5; i <= esdHeroic5; ++i ){
	//		if( allTypeMaxDiff & ( 1 << ( i - startPos ) ) ){
	//			maxDiff = i;
	//		}
	//	}
	//	//
	//	if( maxDiff == esdInvalid ){
	//		maxDiff = esdNormal5;
	//	}
	//	else if( maxDiff < esdHeroic5 ){
	//		++maxDiff;
	//	}
	//}


	//return maxDiff;
	return 0;
}

//=================================================================================
void MMOMapDataManager::ActiveRevivePoint( FVector &activePnt )
{
	if( GWorld == NULL ){
		return;
	}

	if( GWorld->GetWorldInfo() == NULL ){
		return;
	}

	AEmitterPool* emitterPool = GWorld->GetWorldInfo()->MyEmitterPool;
	if( emitterPool == NULL){
		return;
	}

	AMMOPlayerController *pMainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if( pMainPlayerCtrl == NULL ){
		return;
	}

	FString reviveEffectPath;
	GConfig->GetString( TEXT("Scene"), TEXT("reviveEffectPath"), reviveEffectPath, GGameIni );
	UParticleSystem* particleTemplate = LoadObject<UParticleSystem> ( NULL, *reviveEffectPath, NULL, LOAD_None, NULL );
	if( particleTemplate == NULL ){
		return;
	}
	UParticleSystemComponent* particleComp = emitterPool->SpawnEmitter( particleTemplate, activePnt );
	if( particleComp == NULL ){
		return;
	}

	particleComp->SetHiddenGame( pMainPlayerCtrl->IsPlayingMatinee() );
	particleComp->Translation = activePnt;

	//FScriptDelegate endCallBackFun( this , TEXT("OnActiveRevivePointActiveEffectFinished") );
	//particleComp->__OnSystemFinished__Delegate = endCallBackFun;

	particleComp->ActivateSystem( FALSE );

	//text tips
	UMMOUIHintSceneWidget *pSceneHintMsg = ( UMMOUIHintSceneWidget* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_DynamicUI_Scene );
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( pSceneHintMsg != NULL && mainPlayer != NULL ){
		MMOMapDataManager &mapDataManager = MMOMapDataManager::getSingleton();

		INT regionIdx = 0;
		regionIdx = mapDataManager.GetCurPlayerRegionIn( mainPlayer->Location );
		
		FString hintContent;
		FInMapRegion *pCurReginInCurMap = mapDataManager.GetReginInCurMap( regionIdx );
		if( pCurReginInCurMap != NULL ){
			hintContent = pCurReginInCurMap->DisplayName;
		}

		hintContent += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SceneHint.ActiveRevivePoint") );
		pSceneHintMsg->ShowHintSceneMessage( hintContent, 24 );
	}
}

////------------------------------------------------------------------------------------------------------
//void MMOMapDataManager::OnActiveRevivePointActiveEffectFinished( UParticleSystemComponent *psc )
//{
//	if( psc == NULL ){
//		return;
//	}
//
//	FScriptDelegate nullDelgate(EC_EventParm);
//	psc->__OnSystemFinished__Delegate = nullDelgate;
//
//	psc->DeactivateSystem();
//}

//
FVector MMOMapDataManager::CvtSceneVecToGridIdx( const FVector &pos3D )
{
	INT64 llX = 0;
	INT64 llY = 0;
	INT64 llZ = 0;
	FServerGridWorld::ConvertPosClient2Server( pos3D, llX, llY, llZ );

	INT32 nGridXInZone = (INT32)( llX / GRID_WIDTH );
	INT32 nGridYInZone = (INT32)( llY / GRID_WIDTH );

	return FVector( nGridXInZone, nGridYInZone, 0 );
}