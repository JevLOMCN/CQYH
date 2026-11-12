#ifndef _UMAPDATAMANAGER_H_
#define _UMAPDATAMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameClasses.h"
#include "CommonGameEvents.h"

namespace ExternalMessage
{
    struct G2C_SYNC_COMBAT_STAT;
}

	struct FTeleportNetNodeData
	{
		FTeleportNodeObj* _NodeTemplate;
		UBOOL _isPlayerOpened;
	};

	enum EActorInMapUIType
	{
		EACTMUI_InVisible,
		EACTMUI_StaticNPC,
		EACTMUI_NpcGroup,
		EACTMUI_QuestZone,
		EACTMUI_NavPoint,
		EACTMUI_TeleportFB,
		EACTMUI_TeleportMap,
		EACTMUI_TeleportInDoor,
		EACTMUI_TeleportNode,
		EACTMUI_NearMonster,
		EACTMUI_NearBoss,
		EACTMUI_NearPlayer,
		EACTMUI_NearNormalNPC,
		EACTMUI_UserMark,
		EACTMUI_PathPoint,
		EACTMUI_GlobalTeammate,
		EACTMUI_BattleFlag,
		EACTMUI_ExtaInfo,//not a marker, text description shown along with a marker
		EACTMUI_MyPet,
		EACTMUI_OtherPet,
		EACTMUI_Max
	};

	enum EActorInMapFlag
	{
		EACTUIF_None=0x00,
		EACTUIF_FunctionNpc=0x01,
		EACTUIF_TaskNpc=0x02,
		EACTUIF_FB=0x04
	};

	enum EActorInMapUpdateFlag
	{
		EACTUIUF_NoUpdate=0x00,
		EACTUIUF_WorldMap=0x01,
		EACTUIUF_MiniMap=0x02,
		EACTUIUF_PathMap=0x04,
		EACTUIUF_UpdateAll=0x07,
	};

	enum EShareUserMarkTargetType
	{
		ESUType_Team,
		ESUType_Guild
	};

	struct FObjInMapUI
	{
		UINT _ID;
		INT _Type;
		FString _DisplayName;
		UGFxObject *_UiObjInWorldMap;
		UGFxObject *_UiObjInMiniMap;
		UGFxObject *_UiObjInPathMap;
		INT _NeedUpdateFlag;
		//UBOOL _IsWorldmapVisible;
		//UBOOL _IsMinimapVisible;
		FString _UiIconIdx;
		UBOOL _IsValid;

		FObjInMapUI();
	};

	struct FMapMark
	{
		INT _MarkID;
		FString _MarkName;
		INT _SceneID;
		FString _RegionName;
		FVector _Pos;
		INT _MarkType;

		UBOOL _isValid;

		FMapMark();
	};

	struct FMapMarkExport
	{
		TCHAR _MarkName[17];
		TCHAR _RegionName[33];
		FVector _Pos;
		INT _MarkType;
	};

	struct FUserMarkInMapUI : FObjInMapUI
	{
		UINT _RelatedMarkID;

		FUserMarkInMapUI();
	};

	struct FActorInMapUI : FObjInMapUI
	{
		UINT _ActorID;
		UINT _AidID;
		UINT _InMapID;
		INT _ToggleFlag;
		FVector _OrigInMapPos;
		FVector2D _CurMapPos2D;

		FActorInMapUI();
	};

	//struct FActorGroupInMapUI : FObjInMapUI
	//{
	//	INT _LookupIdx;
	//	INT _GroupType;
	//	INT _ActorTemplateID;
	//	UINT _InMapID;
	//	INT _ToggleFlag;
	//	FVector _OrigInMapPos;

	//	FActorGroupInMapUI();
	//};

	struct FPathPointInMapUI : FObjInMapUI
	{
		FVector _OrigInMapPos;
		INT _MapID;
		//FPathPointInMapUI();
	};

	//
	struct MapLineInfo
	{
		INT _SceneLineCombineID;
		INT _PercentState;
	};

	//
	struct CurSceneProgress
	{
		const FSceneProgress *_Template;
		INT _CurValue;
	};

	struct CurSceneTarget
	{
		CurSceneProgress _MainTarget;
		CurSceneProgress _ChildTargets[UCONST_MaxSceneMinorTarget];
	};

	enum EDungeonConstTargetType
	{
		EDgenCnstTgtTyp_TargetKilled,
		EDgenCnstTgtTyp_SpecialKilled,
		EDgenCnstTgtTyp_ShortTimeKill,
		EDgenCnstTgtTyp_Undead,
		EDgenCnstTgtTyp_Max
	};
	static INT MAX_Dungeon_Target_ID_Start = 24;
	static INT MAX_Dungeon_Target = 8;
	struct FBAchievementsState
	{
		const FDungeonTarget *_achievementTemplate;
		INT _score;
	};

	static UINT MAP_PATH_POINT_INTERVAL_FACTOR = 4;
	static UINT MAP_PATH_POINT_REMOVE_LENGTH = 100000;
	
	class MMOMapDataManager : public SingletonMeyers<MMOMapDataManager>, public INativeGameEventHandler
	{
	public:
		static void ParseServerMapLine(UINT serverMapLine, UINT &serverID, UINT &lineID);
	public:
		typedef TMap< INT, TMap< INT, FMapMark > > MapUserMarksContainer;
		typedef TMap< INT, TArray<FNPCInstanceInfo*> > MapNPCsContainer;
		//typedef TMap< INT, const TArray<FNPCGroupInfo>* > MapNpcGroupsContainer;
		typedef TMap< UINT, UINT > NPCsMarkedInMapContainer;
		typedef TMap< INT, UINT > NpcGroupsMarkedInMapContainer;
		typedef TMap< UINT, UINT > FBsMarkedInMapContainer;
		typedef TMap< UINT, UINT > MapNavPointsInMapContainer;
		typedef TMap< INT, TArray<FInMapRegion*> > MapRegionsContainer;

		typedef TMap< INT, FActorInMapUI > MapDynamicActorContainer;
		typedef TMap< INT, FActorInMapUI >::TIterator MapDynamicActorIterator;
		typedef TArray< FActorInMapUI > MapStaticActorContainer;
		typedef TArray< FUserMarkInMapUI > MapUserMarkContainer;
	public:
		MMOMapDataManager(void);
		virtual ~MMOMapDataManager(void);

		void Initialize();
		//
		void InitTeleportClientData();
		//
		void InitAllMapRegionData();
		//
		void InitCurWorldMapRegionData();
		//
		void ClearTeleportDynamicData();
		void UpdateTeleportDynamicData();
		void Shutdown();
		void OnNativeGameEvent( struct MMOGameEvent* evt );

		FVector MMOMapDataManager::CvtSceneVecToGridIdx( const FVector &pos3D );
		//teleport point
	public:
		//FTeleportNodeObj *GetTeleportNetNodes( EWorldRegion regionID, INT nodeID );
		//INT GetTeleportNetNodesIn( EWorldRegion regionID, INT mapID, UBOOL isAllMap, TArray<FTeleportNodeObj*> *rtnList );
		FTeleportNetNodeData *GetTeleportNetNodes( EWorldRegion regionID, INT nodeID );
		FTeleportNetNodeData *GetTeleportNetNodes( EWorldRegion regionID, INT inSceneID, const FString &nodeName );
		FTeleportNetNodeData *GetTeleportNetNodes( INT nodeID );
		INT GetTeleportNetNodesIn( EWorldRegion regionID, INT mapID, UBOOL isAllMap, TArray<FTeleportNetNodeData*> *rtnList );
		INT GetTeleportNetNodesIn( INT mapID, TArray<FTeleportNetNodeData*> *rtnList );
		ATeleportNetNode *FindSceneTeleportsNetNode( INT inSceneID, const FString &nodeName );

		void SetGettedServerTeleportData(UBOOL isGetted){ _IsTeleportUpdatedByServer = isGetted; }
		UBOOL IsGettedServerTeleportData(){ return _IsTeleportUpdatedByServer; }
		void SetCurOpenedNodeID( INT nodeID ){ _LastOpenedTeleportNodeID = nodeID; }
		FTeleportNodeObj *GetLastOpendNode(){ return UGlobalMapInfo::GetSingleton()->FindTeleportNetNodes( _LastOpenedTeleportNodeID ); }

		void NotifyTeleportsNetNodesOpened( INT nodeID, UBOOL isOpened );
		void ActiveCurSceneTeleports();
		void FirstOpeneTeleportsNetNodes( INT nodeID );

		//map
	public:
		////area in the map where the current character is
		//INT GetCurReginIdxInMap(){ return _InCurMapRegionIdx; };
		//AMMOVolume_MapSize *GetReginInMap( INT inRegionIdx ){ 
		//	if( inRegionIdx < 0 || inRegionIdx >= _CurMapRegionInfo.Num() ){
		//		return NULL;
		//	}
		//	return _CurMapRegionInfo(inRegionIdx);
		//}

		////check the area in the map where the current character is
		//INT CheckRegion();
		FInMapRegion *GetCurReginInCurMap(){ return _InCurMapRegion; }
		//
		FInMapRegion *GetReginInCurMap( INT inRegionIdx ){
			return GetReginInMap( _CurSceneInfo.ServerID, inRegionIdx );
		}
		//
		UBOOL IsCurMapRegionMultilayer(){ return IsMapRegionMultilayer( _CurSceneInfo.ServerID ); }
		UBOOL IsWorldmapRegionMultilayer(){ return IsMapRegionMultilayer( _CurWorldMapInfo.ServerID ); }
		UBOOL IsMapRegionMultilayer( INT mapID );
		//
		FInMapRegion *GetReginInMap( INT mapID, INT inRegionIdx ){
			TArray<FInMapRegion*> &singleMapRegionList = _AllMapRegionsInfo.FindOrAdd( mapID );
			if( !singleMapRegionList.IsValidIndex( inRegionIdx ) ){
				return NULL;
			}
			return singleMapRegionList(inRegionIdx);
		}
		//
		FInMapRegion *GetReginInMap( INT mapID, const FString &inRegionVolumeName );
		INT GetReginInMapIdx( INT mapID, const FString &inRegionVolumeName );
		//
		TMap< INT, TArray<FInMapRegion*> > &GetAllMapRegions(){
			return _AllMapRegionsInfo;
		}

		//check the area in the map where the current character is
		INT GetCurPlayerRegionIn( const FVector &playerPos );
		UBOOL IsInCurRegion( const FVector &checkPos );
		//
		INT CheckRegionIn( INT mapID, const FVector &checkPos );
		//area in the map where the current character is
		INT GetCurReginIdxInMap(){ return _InCurMapRegionIdx; }
		//INT SetCurReginIdxInMap( UINT regionInMapIdx ){ _InRegionIdx = regionInMapIdx; }

		//info of the map where the current character is
		void SetCurMap( INT sceneID );
		void SetCurMap( const FString &mapName );
		FExportMap &GetCurMap(){ return _CurSceneInfo; }
		//line info of the current map
		INT GetCurSceneLineIn(){ return _CurSceneLineIn; }
		void SetCurSceneLineIn( INT lineID );

		TArray< MapLineInfo > &GetCurMapLines(){ return _CurSceneLines; }
		void AddNewCurMapLine( INT lineID, INT percentState );
		void RemoveOldCurMapLine( INT lineID );
		void ClearCurMapLines(){ _CurSceneLines.Empty(); }
		//progress info of the current map
		TArray<CurSceneTarget> &GetCurSceneProgress(){ return _CurSceneProgress; }
		void InitCurSceneProgress();
		//dungeon rating points
		void InitFBConstAchievements();
		TArray<FDungeonTarget> &GetFBConstAchievements(){ return _FBConstAchievements; }

		void ParseDungeonFinishDatasForAchievement( G2C_DUNGEON_FINISH_DATA* serverMsg );
		void ParseDungeonCombatDatas( ExternalMessage::G2C_SYNC_COMBAT_STAT* serverMsg, UINT dataCnt );
		void PrepareCurFBAchievementsScore( INT mapID, INT fbDifficult );

		TArray<FBAchievementsState> &GetFBAchievementsScoreData(){ return _FBAchievementsData; }
		FBAchievementsState *FindFBAchievementCurrentDungeon( INT fbAchieveID );
		//current world map info
		void SetCurWorldMap( INT sceneID, INT inRegionIdx );
		FExportMap &GetCurWorldMap(){ return _CurWorldMapInfo; }
		FInMapRegion *GetCurWorldMapRegion(){ return _InWorldmapRegion; }
	public:
		//maps whose map markers have been modified
		TMap< INT, INT > &GetNeedUpdateMap(){ return _UserMarksEditedMapContainer; }
		//map markers
		void InitialMapUserMarks( INT sceneID );
		UBOOL LoadMapUserMarks( INT sceneID );
		UBOOL ExportMapUserMarks( INT sceneID );
		void ResetCurWorldUserMarkUI();
		FMapMark *GetMapMarkData( INT sceneID, INT markID );
		FMapMark *AddMapMarkData( INT sceneID, const FString &regionVolumeName, const FString &markName, INT markType, const FVector &pos );
		FUserMarkInMapUI *AddMapMarkToUI( FMapMark *pNewMapMarkData );
		//map marker display
		FUserMarkInMapUI *AddMapMark( INT sceneID, const FString &regionVolumeName );
		void RemoveMapMark( INT sceneID, INT markID );
		TArray<FUserMarkInMapUI> &GetCurWorldMapMarks(){
			return _ListWorldMapUserMarks;
		}

		//dynamic objects
		//void ClearAllTeammateInDymcActorGlobal();
		FActorInMapUI *AddOrUpdateMapDymcActorNear( AMMOActor &addCharactor );
		FActorInMapUI *AddOrUpdateMapDymcActorGlobal( INT actorID, EActorInMapUIType type, INT inMapID, const FString &displayName );
		FActorInMapUI *FindMapDymcActorGlobal( INT actorID );
		void FindMapDymcActorGlobalOfType( EActorInMapUIType actorType, TArray<FActorInMapUI*> &rtnArray );

		void RemoveMapDymcActorNear( INT actorID ){
			RemoveMapDymcActor( _MapNearActors, actorID );
		}
		void RemoveMapDymcActorGlobal( INT actorID );
		void RemoveAllMapDymcActorGlobalOfType( EActorInMapUIType actorType );
		void RemoveMapDymcActorGlobalOfType( EActorInMapUIType actorType, INT actorID );

		MapDynamicActorIterator BeginIterateDymcActorNear(){
			MapDynamicActorIterator it( _MapNearActors );
			return it;
		}
		MapDynamicActorIterator BeginIterateDymcActorGlobal(){
			MapDynamicActorIterator it( _MapGlobalActors );
			return it;
		}

		FActorInMapUI *GetMapDymcActorNear( INT actorID ){
			return GetMapDymcActor( _MapNearActors, actorID );
		}
		FActorInMapUI *GetMapDymcActorGlobal( INT actorID ){
			return GetMapDymcActor( _MapGlobalActors, actorID );
		}

		FActorInMapUI *GetMapDymcActorNearByUiID( INT uiID ){
			return GetMapDymcActorByUiID( _MapNearActors, uiID );
		}
		FActorInMapUI *GetMapDymcActorGlobalByUiID( INT uiID ){
			return GetMapDymcActorByUiID( _MapGlobalActors, uiID );
		}

		//scene static NPC info
		const TArray<FNPCInstanceInfo*> *GetNpcInstanceInMap( INT sceneID );

		//-------------------------------------------
		//
		void InitMinimapStaticActorsContainer();
		//table of NPCs shown in the current map scene, key is the index in GetNpcInstanceInMap, value is the number in the marker list (_ListCurMapUIStaticActors)
		const TMap< UINT, UINT > &GetCurScenePuttedUsefulNpcs(){ return _CurScenePuttedUsefulNpcContainer; }

		//table of NPCs shown in the current minimap UI, key is the NPC ID, value is the number in the marker list (_ListCurMapUIStaticActors)
		TArray<FActorInMapUI*> &GetStaticActorsInCurRegion(){ return _MinimapStaticActorsContainer; }

		//-------------------------------------------
		//
		void InitWorldMapStaticActorsContainer( INT inRegionIdx );
		TArray<FActorInMapUI*> &GetStaticActorsInWorldmap(){ return _WorldMapStaticActorsContainer; }
		//table of NPCs shown in the current world map scene, key is the index in GetNpcInstanceInMap, value is the number in the marker list (_ListWorldMapUIStaticActors)
		const TMap< UINT, UINT > &GetScenePuttedUsefulNpcs(){ return _ScenePuttedUsefulNpcContainer; }

		//
		void ClassifyWorldMapStaicMarks();

		//
		void InitWorldMapPuttedUsefulNpcs();
		//table of NPCs shown in the current world map UI, key is the NPC ID, value is the number in the marker list (_ListWorldMapUIStaticActors)
		const TMap< UINT, UINT > &GetWorldMapPuttedUsefulNpcs(){ return _WorldMapPuttedUsefulNpcContainer; }

		//monster points in the scene
		void InitWorldMapPuttedNpcGroups();
		const TMap< INT, UINT > &GetWorldMapPuttedNpcGroups(){ return _WorldMapPuttedNpcGroupsContainer; }
		//
		const TArray<FNPCGroupInfo> *GetNpcGroupsInMap( INT sceneID );

		//special areas in the scene
		const TArray<FQuestVolumeInfo> *GetSpeciZoneInMap( INT sceneID );
		//pathfinding points in the scene
		const TArray<FNaviPointInfo> *GetNavPointsInMap( INT sceneID );

		//dungeons in the scene
		void InitWorldMapPuttedFBs();
		//table of FBs shown in the current world map UI, key is the FB jump point ID, value is the number in the marker list (_ListWorldMapUIStaticActors)
		const TMap< UINT, UINT > &GetWorldMapPuttedFBs(){ return _WorldMapPuttedFBContainer; }

		//dungeons in the scene
		void InitWorldMapNavPoints();
		//table of pathfinding points shown in the current world map UI, key is the number in GetNavPointsInMap, value is the marker number in the map
		const TMap< UINT, UINT > &GetWorldMapNavPoints(){ return _WorldMapNavPointsContainer; }
		
		//update fixed point state
		//void UpdateCurMapStaticMarksOfType( EActorInMapUIType actorType ){
		//	UpdateMapStaticMarksOfType( actorType, _CurSceneInfo.ServerID );
		//}
		//void UpdateWorldMapStaticMarksOfType( EActorInMapUIType actorType ){
		//	UpdateMapStaticMarksOfType( actorType, _CurWorldMapInfo.ServerID );
		//}
		//static objects: map NPCs
		//SetCurMap must be called first
		void PrepareCurMapStaticActorInMapUI(){
			PrepareMapStaticActorInMapUI( _CurSceneInfo.ServerID, _ListCurMapUIStaticActors, true );
			InitMinimapStaticActorsContainer();
		}
		//SetCurWorldMap must be called first
		void PrepareWorldMapStaticActorInMapUI(){
			PrepareMapStaticActorInMapUI( _CurWorldMapInfo.ServerID, _ListWorldMapUIStaticActors, false );
			InitWorldMapStaticActorsContainer( _InWorldmapRegionIdx );
		}
		//
		void PrepareMapStaticActorInMapUI( INT sceneServerID, TArray< FActorInMapUI > &container, bool isCurScene );
		//
		TArray< FActorInMapUI > &GetCurMapStaticActorPointsList(){
			return _ListCurMapUIStaticActors;
		}

		TArray< FActorInMapUI > &GetCurWorldMapStaticActorPointsList(){
			return _ListWorldMapUIStaticActors;
		}

		FActorInMapUI &GetCurWorldMapStaticActorInMapUIByActorID( UINT id ){
			INT cnt = _ListWorldMapUIStaticActors.Num();
			for( INT i = 0; i < cnt; ++i ){
				FActorInMapUI &actorInMap = _ListWorldMapUIStaticActors( i );
				if( actorInMap._ActorID == id ){
					return actorInMap;
				}
			}
		}

		//map flag info
		TArray< FActorInMapUI > &GetCurWorldMapMarksHintInfo(){
			return _ListWorldMapUIMarksHintInfo;
		}
		void AddWorldMapMarkInfo( FVector real3DPos, const FString &info );
		void InvalidAllWorldMapMarkInfo();

		//pathfinding path
		TArray<FVector> GetCurMainPlayerPath( UINT interval, UBOOL is2D );
		void SetLastInputMoveToPos( FVector pos ){ _LastInputPos = pos; }
		FVector GetLastInputMoveToPos(){ return _LastInputPos; }

		//
		void InvalidCurPath();
		UINT GetCurSceneWorldMapPathInterval();
		void SetMapPath( TArray< FVector > &pathList );
		TArray< FPathPointInMapUI > &GetCurMapPath();
		TArray< FPathPointInMapUI > &GetAllMapPathResource(){ return _MapCurPath; }

		//list of acceptable, completable and completed quests on the current world map, key is the quest ID, value is the owning NPC's ID
		TMap< UINT, UINT > &GetCompleteTaskInWorldMap(){ return _CompleteTaskContainer; }
		TMap< UINT, UINT > &GetUncompleteTaskInWorldMap(){ return _UncompleteTaskContainer; }
		TMap< UINT, UINT > &GetAcceptableTaskInWorldMap(){ return _AcceptableTaskContainer; }
	public:
		//dungeon difficulty
		INT GetCurFBDiff(){ return _CurFBDiff; }
		void SetCurFBDiff( INT difficulty ){ _CurFBDiff = difficulty; }

		//dungeon difficulty
		INT GetMaxDiffOfType( INT mapID, INT type, INT allTypeMaxDiff );
		INT GetCurFBMaxDiff(){ return _CurFBMaxDiff; }
		void SetCurFBMaxDiff( INT difficulty ){ _CurFBMaxDiff = difficulty; }

		//
		INT GetCurFBEnterFrequence(){ return _CurFBFrequence; }
		void SetCurFBEnterFrequence( INT frequence ){ _CurFBFrequence = frequence; }

		//
		//UBOOL LoadDungonInfo();
		const FDungeonInfo *GetDungeonInfo( UINT mapID, UINT diffculty );
		INT GetDungeonInfoDifftyMask( UINT mapID );

		//dungeon time
		INT GetTimeSinceDungeonBegin();
		UINT32 GetDungeonStartTime(){ return _DungeonStartUtcTime; }
		void SetDungeonStartTime( UINT32 startTime, INT modifySec = 0 ){ _DungeonStartUtcTime = startTime + modifySec; }

	private:
		//dynamically updated objects on the map
		FActorInMapUI *AddOrUpdateMapDymcActor( MapDynamicActorContainer &container, INT actorID, const FString &displayName );
		void ChangeMapMarkDisplayIcon( FActorInMapUI *pActorInMapUI, AMMOActor &charactor );
		void ChangeMapNearPlayerMarkDisplayIcon( FActorInMapUI *pActorInMapUI,  AMMOActor &charactor );

		void RemoveMapDymcActor( MapDynamicActorContainer &container, INT actorID );
		FActorInMapUI *GetMapDymcActor( MapDynamicActorContainer &container, INT actorID );
		FActorInMapUI *GetMapDymcActorByUiID( MapDynamicActorContainer &container, INT uiID );

		//map markers
		FMapMark *AddMapMarkData( INT sceneID, const FString &regionVolumeName );
		FMapMark *RemoveMapMarkData( INT sceneID, INT markID );
		TMap< INT, FMapMark > *GetMapMarkDatas( INT sceneID );
		//
		int PutNpcToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container, TMap< UINT, UINT > &npcContainer );
		int PutNpcGroupsToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container );
		int PutTelportToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container );
		int PutTelportNodeToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container );
		int PutSpeciZoneToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container );
		int PutNavPointsToMapUI( INT sceneServerID, INT curObjIdx, TArray< FActorInMapUI > &container );

		void UpdateMapStaticMarksOfType( EActorInMapUIType actorType, INT sceneID );

		//respawn point
	public:
		void ActiveRevivePoint( FVector &activePnt );
		//void OnActiveRevivePointActiveEffectFinished( UParticleSystemComponent *psc );

		//teleport point
	private:
		UBOOL _IsTeleportInited;
		UBOOL _IsTeleportUpdatedByServer;
		//a jump point table sorted by display priority and stored per map, contained in their respective areas (fixed array)
		//TArray<FTeleportNodeObj*> _RegionTeleportNetList[EWR_MAX];
		TArray<FTeleportNetNodeData> _RegionTeleportNetList[EWR_MAX];
		//
		INT _LastOpenedTeleportNodeID;

		//Pathfinding
	private:
		FVector _LastInputPos;

		//map
	private:
		//block info of all scene maps
		MapRegionsContainer _AllMapRegionsInfo;
		//current map info
		FExportMap _CurSceneInfo;
		//current map line info
		TArray< MapLineInfo > _CurSceneLines;
		//line currently on
		INT _CurSceneLineIn;
		//current dungeon difficulty
		INT _CurFBDiff;
		INT _CurFBMaxDiff;
		//current dungeon entry
		INT _CurFBFrequence;
		//dungeon progress
		TArray<CurSceneTarget> _CurSceneProgress;
		////dungeon info
		//DungeonInfoContainer _AllDungeonBaseInfo;
		//current map block info
		TArray< AMMOVolume_MapSize* > _CurMapRegionInfo;
		//current map area
		INT _InCurMapRegionIdx;
		FInMapRegion *_InCurMapRegion;
		//current world map area
		INT _InWorldmapRegionIdx;
		FInMapRegion *_InWorldmapRegion;
		//map info shown by the current world map
		FExportMap _CurWorldMapInfo;

		//NPC points of each map
		MapNPCsContainer _AllMapNpcContainer;
		//monster points of each map
		//MapNpcGroupsContainer _AllMapNpcGroupsContainer;

		//marker points of each map
		MapUserMarksContainer _AllMapUserMarkContainer;
		TMap< INT, INT > _UserMarksEditedMapContainer;

		//dynamic markers (currently players and monsters) in the nine screens around the player that need to be shown, key is actorID
		MapDynamicActorContainer _MapNearActors;

		//dynamic markers that need to be shown on the whole map (such as teammates, some special objects), key is actorID
		MapDynamicActorContainer _MapGlobalActors;

		//static marker points on the map, i.e. NPCs pre-placed in the scene (_ActorID is the index in the map's NPC table _AllMapNpcContainer) and jump points.
		//static map markers the current map needs to show (currently NPCs placed on the map that need to be shown on the map UI, where _UiObjInWorldMap is invalid)
		MapStaticActorContainer _ListCurMapUIStaticActors;
		//static map markers the world map needs to show (currently NPCs placed on the map that need to be shown on the map UI, where _UiObjInMinimapap is invalid)
		MapStaticActorContainer _ListWorldMapUIStaticActors;
		MapStaticActorContainer _ListWorldMapUIMarksHintInfo;

		//player-defined markers on the world map
		MapUserMarkContainer _ListWorldMapUserMarks;

		//current path that needs to be shown on the map
		TArray<FPathPointInMapUI> _MapCurPath;

		//existence of _CurScenePuttedUsefulNpcContainer and _ScenePuttedUsefulNpcContainer
		//is to remove already shown NPCs when showing nearby NPCs
		//NPC table of the current map, key is the index in GetNpcInstanceInMap
		NPCsMarkedInMapContainer _CurScenePuttedUsefulNpcContainer;
		//NPC table shown by the current minimap, key is the index in GetNpcInstanceInMap
		TArray< FActorInMapUI* > _MinimapStaticActorsContainer;

		//NPC table in the scene of the current world map, key is the index in GetNpcInstanceInMap
		NPCsMarkedInMapContainer _ScenePuttedUsefulNpcContainer;
		//NPC table shown by the current world map, key is the index in GetNpcInstanceInMap
		NPCsMarkedInMapContainer _WorldMapPuttedUsefulNpcContainer;
		//
		TArray< FActorInMapUI* > _WorldMapStaticActorsContainer;
		//monster points on the world map
		NpcGroupsMarkedInMapContainer _WorldMapPuttedNpcGroupsContainer;
		//current world map dungeon table, key is the jump point ID, value is the marker number of the uiObject shown on the map
		FBsMarkedInMapContainer _WorldMapPuttedFBContainer;
		//
		MapNavPointsInMapContainer _WorldMapNavPointsContainer;

		//list of acceptable, completable and completed quests on the current world map
		TMap< UINT, UINT > _CompleteTaskContainer;
		TMap< UINT, UINT > _UncompleteTaskContainer;
		TMap< UINT, UINT > _AcceptableTaskContainer;

		//dungeon rating points
		TArray<FDungeonTarget> _FBConstAchievements;
		TArray<FBAchievementsState> _FBAchievementsData;
		//dungeon time
		UINT32 _DungeonStartUtcTime;
	};
	
#endif