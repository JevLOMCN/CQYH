#ifndef _SCENE_H_
#define _SCENE_H_

#include "base_list.h"

#include "scene_def.h"
#include "grid_def.h"
#include "zone_def.h"
#include "scene_file_def.h"
#include "string_def.h"
#include "variable_def.h"
#include "role.h"
#include "dungeon_data.h"

class CRegion;
class CNpc;
class CDoodad;
class CDrop;
class CMissile;
class CNavData;
class CSceneStaticData;
class CLuaScript;
class CHero;

struct SCENE_TELEPORTS;
struct GRID_DATA;
struct SCENE_DATA;
struct NPC_SPAWN_POINT;
struct DOODAD_SPAWN_GROUP;
struct DOODAD_SPAWN_POINT;
struct BTCustomCtx;
struct BTNod;
class CTeamInGS;

class CScene
{
public:
	CScene(void);
	virtual ~CScene(void);

	BOOL Init(INT32 nServerGroupID, INT32 nSceneTempID, INT32 nSceneID);
	BOOL UnInit(void);
	BOOL MainLoop(void);

	BOOL Close(INT32 nDelayFrame);
	BOOL CalculateDungeonScore();

	BOOL IsCanReward();


	BOOL AddRole(CRole* poRole, INT32 nAppearType);
	BOOL RemoveRole(CRole* poRole);

	BOOL AddNpc(CNpc* poNpc, INT32 nAppearType);
	BOOL RemoveNpc(CNpc* poNpc);

	BOOL AddDoodad(CDoodad* poDoodad);
	BOOL RemoveDoodad(CDoodad* poDoodad);

	BOOL AddDrop(CDrop* poDrop, INT64 llX, INT64 llY, INT64 llZ, UINT32 dwSrcID);
	BOOL RemoveDrop(CDrop* poDrop);

	BOOL AddMissile(CMissile* poMissile);
	BOOL RemoveMissile(CMissile* poMissile);

	BOOL AddHero(CHero* poHero, INT32 nAppearType);
	BOOL RemoveHero(CHero* poHero);

	template<class TFunc>
	inline BOOL TraverseRole(TFunc& rFunc);

	inline INT32 GetSceneTempID(void);
	inline INT32 GetSceneID(void);

	BOOL CheckTargetID(UINT32 dwNpcTempID); 
	//void UpdateTarget(INT32 nType);
	inline INT32 GetState(void);
	inline INT32 GetStartFrame(void);

	inline const SCENE_DATA* GetSceneData(void);
	inline CSceneStaticData* GetStaticData(void);

	inline CRegion* GetRegion(INT32 nRegionX, INT32 nRegionY);
	inline GRID_INSTANCE_DATA* GetGridInstance(const ZONE_DATA* pcZoneData, const GRID_DATA* pcGridData);
	BOOL CheckAdjGrid(INT64 llX, INT64 llY, INT64 llZ, GRID_INSTANCE_DATA* pSelfGridInstance);
	inline ZONE_INSTANCE_DATA* GetZoneInstance(INT64 llX, INT64 llY);

	inline void SetFirstBloodFlag(BOOL bFlag);
	inline BOOL GetFirstBloodFlag();

	inline BOOL GetDLRecordRole();

    CNpc* SpawnPet(INT32 nTemplateID, INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection, 
        UINT32 dwHostID, const NPC_SPAWN_POINT* pcSpawnPoint, INT32 nAppearType);
	CNpc* SpawnNpc(INT32 nTemplateID, INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection, 
		UINT32 dwHostID, const NPC_SPAWN_POINT* pcSpawnPoint);
	CNpc* SpawnNpcPoint(const NPC_SPAWN_POINT* pcSpawnPoint);
	BOOL SpawnNpcMediumGroup(INT32 nMediumGroupID);
	BOOL SpawnNpcLargeGroup(INT32 nLargeGroupID);
	BOOL AddNpcToRespawnList(CNpc* poNpc);
	BOOL RespawnNpc(CNpc* poNpc);
	void IncreaseDieCount(CNpc* poNpc, INT32 nLargeGroupID, INT32 nMediumGroupID);
	CNpc* SpawnSingleNpc(INT32 nSingleNpcID);
	INT32 GetMediumGroupSpawnCount(INT32 nMediumGroupID);
	void AddNpcLiveTimer(UINT32 dwNpcID, UINT32 dwSuicideTime);

	CDoodad* SpawnDoodad(INT32 nTemplateID, INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection, const DOODAD_SPAWN_POINT* pcSpawnPoint = NULL);
    CDoodad* SpawnDoodadPoint(const DOODAD_SPAWN_POINT* pcSpawnPoint);
	BOOL SpawnDoodadGroup(INT32 nGroupID);
	BOOL AddDoodadToRespawnList(CDoodad* poDoodad);
	BOOL RespawnDoodad(CDoodad* poDoodad);

	BOOL FindValidPos(CSceneObject* poSceneObjcet, BOOL bStacked,
		const ZONE_DATA*& rpcZone, const GRID_DATA*& rpcGrid, GRID_INSTANCE_DATA*& rpGridInstance, CRegion*& rpRegion);
	BOOL GetLowerGrid(INT64 llX, INT64 llY, INT64 llZ, const ZONE_DATA*& rpcZoneData, const GRID_DATA*& rpcGridData);
	INT32 GetLowerZ(INT64 llX, INT64 llY, INT64 llZ);
	INT32 GetNearestAvailablePos(INT32 nCenterX, INT32 nCenterY, INT32 nCenterZ, INT32 nMaxRange, 
		INT32 nPosCount, INT32 nPos[][3]);
	inline INT32 GetVolumeTriggerCount(INT32 nVolumeID);
	inline void  IncVolumeTriggerCount(INT32 nVolumeID);

	void SetDoorState(INT32 nDoorID, BOOL bClosed);
	BOOL IsDoorClosed(INT32 nDoorID);
	inline UINT64 GetDoorState(void);

	void SetVariable(INT32 nIndex, INT32 nValue);
	inline INT32 GetVariable(INT32 nIndex);

	inline INT32 GetRoleCount(void);
    inline INT32 GetNpcCount(void);
	inline INT32 GetServerGroup(void);

	BOOL TouchVolume(CRole* poRole, INT32 nVolumeID);
	BOOL RunBT(INT32 eEventType, UINT32 dwRoleID, BTNod* pRoot);

	BOOL IsObstacled(INT64 llSrcX, INT64 llSrcY, INT64 llSrcZ, INT64 llDstX, INT64 llDstY, INT64 llDstZ, BOOL bCheckBlockMove);
    BOOL IsObstacled(CSceneObject* poSrcObj, CSceneObject* poDestObj, BOOL bCheckBlockMove);

	BOOL Say(UINT32 dwTargetID, UINT32 dwLocation, const char* pcszMessage);

	inline CBaseList& GetCombatStatList(void);

	inline INT32 GetWeather(void);
	BOOL SetWeather(INT32 nWeather, INT32 nDurationFrame);

    void CheckDungeonGoal(CRole* poRole);

	inline BOOL IsSabacStarted(void);

	//////////////////////////////////////////////////////////////////////////
	// guild scene
	CNpc *FindNpc(INT32 nSingleNpcID);
	//////////////////////////////////////////////////////////////////////////

	BOOL AddMissilePlacedGrid(INT32 nGridX, INT32 nGridY, CMissile* poMissile);
	CMissile* FindMissileByPlacedGrid(INT32 nGridX, INT32 nGridY);
	BOOL RemoveMissilePlacedGrid(INT32 nGridX, INT32 nGridY);

private:
	BOOL _LoadNpc(void);
	BOOL _LoadDoodad(void);

	void _PushBTCtx(BTCustomCtx* pBTCtx);
	BOOL _ProcessSyncRun(BTCustomCtx* pBTCtx);

	INT32 _GetRandDoodadPoint(const DOODAD_SPAWN_GROUP* pcSpawnGroup);
	BOOL _IsDoodadPointSpawned(INT32 nGroupID, INT32 nPointID);
	void _SetDoodadPointSpawned(INT32 nGroupID, INT32 nPointID, BOOL bSpawned);

	void _AttachCombatStatNode(CRole* poRole);
	BOOL _CheckGrid(INT64 llX, INT64 llY, INT64 llZ, GRID_INSTANCE_DATA* pSelfGridInstance);

	inline void _CheckDLRecordRoleBegin(UINT32 dwTime);
	inline void _CheckDLRecordRoleEnd();
#ifndef _ROBOT
	void _OnSabacMainLoop(void);
	void _DeliverSabacAward(UINT32 dwGuildID);
#endif	// _ROBOT
	INT32 _SideConvertToFaction();
	struct CTraverseRoleOnSceneDestroy
	{
		BOOL operator()(CRole* poRole);
	}           m_TraverseRoleOnDestroy;

	struct CTraverseRoleCalculate
	{
		INT32               m_nTotalFrame;
		const DUNGEON_DATA* m_pDungeonData;
		BOOL operator()(CRole* poRole);
	}           m_TraverseRoleOnCalculate;

	struct CTraverseRoleSceneClose
	{
		BOOL bRoleAura;
		BOOL operator()(CRole* poRole);
	}         m_TraverseRoleOnSceneClose;

	struct CTraverseNpcSceneClose
	{
		BOOL operator()(CNpc* poNpc);
	}		m_TraverseNpcOnSceneClose;

#ifndef _ROBOT
	struct CTraverseRoleForSabac
	{
		UINT32 dwGuildID;
		BOOL operator()(CRole* poRole);
	};
#endif	// _ROBOT

private:
	INT32					m_nServerGroupID;
	INT32					m_nSceneTempID;
	INT32					m_nSceneID;
	INT32					m_nState;
	INT32					m_nStartFrame;
	INT32					m_nWeather;
	INT32					m_nWeatherEndFrame;
	INT32					m_nWeatherRefreshFrame;

	CRegion*				m_poRegionList[MAX_REGION_PER_SCENE][MAX_REGION_PER_SCENE];
	INT32					m_nRegionWidth;
	INT32					m_nRegionHeight;

    char*					m_pszZoneInstanceData;
	ZONE_INSTANCE_DATA**	m_ppZoneInstanceData;
	INT32					m_nZoneWidth;
	INT32					m_nZoneHeight;

	CSceneStaticData*		m_poStaticData;
	const SCENE_DATA*		m_pcSceneData;
	UINT64					m_qwDoorTable;
	UINT8					m_byVolumeTriggerCount[MAX_VOLUME_COUNT];

	INT32					m_nTotalFrame;
	INT32					m_nClosingFrame;
	BOOL					m_bReward;

	INT32					m_nDLSceneID;
	BOOL					m_bDLRecordRole;	

	BOOL					m_bFirstBloodFlag;

	struct NPC_MEDIUM_GROUP
	{
		INT32					nSpawnCount;
		INT32					nDieCount;
	};

	struct NPC_LARGE_GROUP
	{
		INT32					nDieCount;
	};

	NPC_MEDIUM_GROUP		m_MediumGroupList[MAX_NPC_MEDIUM_GROUP_PER_LARGE_GROUP];
	NPC_LARGE_GROUP			m_LargeGroupList[MAX_NPC_LARGE_GROUP_COUNT];
	CBaseList				m_lstRespawnNpc;

	struct DOODAD_GROUP
	{
		INT32					nSpawnCount;
		UINT32					bSpawnedList[MAX_DOODAD_SPAWN_POINT_COUNT / 32];
	};
	DOODAD_GROUP			m_DoodadGroupList[MAX_DOODAD_SPAWN_GROUP_COUNT];
	CBaseList				m_lstRespawnDoodad;

	CBaseList				m_lstRole;
	INT32					m_nRoleCount;
    INT32					m_nNpcCount;
	INT32					m_nVarList[MAX_SCENE_VARIABLE_COUNT];

	struct NPC_LIVE_TIMER
	{
		UINT32				dwSuicideTime;
		UINT32				dwNpcID;
	};
	typedef std::list<NPC_LIVE_TIMER>	NPC_LIVE_TIMER_LIST;
	NPC_LIVE_TIMER_LIST		m_LiveTimerList;

	CBaseList				m_CombatStatList;

	typedef std::map<UINT32, CMissile*> TMissilePlaceMap;
	TMissilePlaceMap		m_MissilePlaceMap;

	// battle 
	CTeamInGS*				m_lstpoBattleTeam[MAX_FACTION_COUNT];

	CBaseList				m_lstBTCtx;

	UINT32					m_SingleNpcObjID[MAX_SINGLE_NPC_COUNT];

	// sabac
	BOOL					m_bSabacStarted;
	BOOL					m_bSabacEventTriggered;
};

inline INT32 CScene::GetSceneTempID(void)
{
	return m_nSceneTempID;
}

inline INT32 CScene::GetSceneID(void)
{
	return m_nSceneID;
}

inline INT32 CScene::GetState(void)
{
	return m_nState;
}

inline INT32 CScene::GetStartFrame(void)
{
	return m_nStartFrame;
}

inline const SCENE_DATA* CScene::GetSceneData()
{
	return m_pcSceneData;
}

inline CSceneStaticData* CScene::GetStaticData(void)
{
	return m_poStaticData;
}

inline CRegion* CScene::GetRegion(INT32 nRegionX, INT32 nRegionY)
{
	PROCESS_ERROR(nRegionX >= 0 && nRegionX < m_nRegionWidth);
	PROCESS_ERROR(nRegionY >= 0 && nRegionY < m_nRegionHeight);

	return m_poRegionList[nRegionX][nRegionY];
Exit0:
	return NULL;
}

inline GRID_INSTANCE_DATA* CScene::GetGridInstance(const ZONE_DATA* pcZoneData, const GRID_DATA* pcGridData)
{
    INT32 nZoneX = 0;
    INT32 nZoneY = 0;
    INT64 nGridIndex = 0;
    LOG_PROCESS_ERROR(pcZoneData);
    LOG_PROCESS_ERROR(pcGridData);

	nZoneX = pcZoneData->usZoneX;
	nZoneY = pcZoneData->usZoneY;
	nGridIndex = pcGridData - pcZoneData->GridList;

    PROCESS_ERROR(nZoneX >= 0 && nZoneX < m_nZoneWidth);
	PROCESS_ERROR(nZoneY >= 0 && nZoneY < m_nZoneHeight);
	LOG_PROCESS_ERROR(nGridIndex > 0 && nGridIndex <= pcZoneData->nGridCount);

	return m_ppZoneInstanceData[nZoneX + nZoneY * m_nZoneWidth]->GridList + nGridIndex;
Exit0:
	return NULL;
}

inline ZONE_INSTANCE_DATA* CScene::GetZoneInstance(INT64 llX, INT64 llY)
{
	INT32 nZoneX = (INT32)(llX / ZONE_WIDTH);
	INT32 nZoneY = (INT32)(llY / ZONE_WIDTH);

	PROCESS_ERROR(nZoneX >= 0 && nZoneX < m_nZoneWidth);
	PROCESS_ERROR(nZoneY >= 0 && nZoneY < m_nZoneHeight);

	return m_ppZoneInstanceData[nZoneX + nZoneY * m_nZoneWidth];
Exit0:
	return NULL;
}

template<class TFunc>
inline BOOL CScene::TraverseRole(TFunc& rFunc)
{
	INT32 nRetCode = 0;
	BASE_LIST_NODE* pNode = NULL;
	BASE_LIST_NODE* pNextNode = NULL;

	pNode = m_lstRole.Head().pNext;
	while (pNode != &(m_lstRole.Rear()))
	{
		CRole* poRole = node_cast(CRole*, GetSceneListNode(), pNode);
		pNextNode = pNode->pNext;

		nRetCode = rFunc(poRole);
		PROCESS_ERROR(nRetCode);

		pNode = pNextNode;
	}

	return TRUE;
Exit0:
	return FALSE;
}

inline UINT64 CScene::GetDoorState(void)
{
	return m_qwDoorTable;
}

inline INT32 CScene::GetVolumeTriggerCount(INT32 nVolumeID)
{
	LOG_PROCESS_ERROR(nVolumeID >= 0 && nVolumeID < MAX_VOLUME_COUNT);
	return m_byVolumeTriggerCount[nVolumeID];
Exit0:
	return INT_MAX;
}

inline void CScene::IncVolumeTriggerCount(INT32 nVolumeID)
{
	LOG_PROCESS_ERROR(nVolumeID >= 0 && nVolumeID < MAX_VOLUME_COUNT);
	m_byVolumeTriggerCount[nVolumeID]++;
	return;
Exit0:
	return;
}

inline INT32 CScene::GetVariable(INT32 nIndex)
{
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_SCENE_VARIABLE_COUNT);
	return m_nVarList[nIndex];
Exit0:
	return 0;
}

inline INT32 CScene::GetRoleCount(void)
{
	return m_nRoleCount;
}

inline INT32 CScene::GetNpcCount(void)
{
    return m_nNpcCount;
}

inline INT32 CScene::GetServerGroup(void)
{
	return m_nServerGroupID;
}

inline CBaseList& CScene::GetCombatStatList(void)
{
	return m_CombatStatList;
}

inline void CScene::SetFirstBloodFlag(BOOL bFlag)
{
	m_bFirstBloodFlag = bFlag;
}

inline BOOL CScene::GetFirstBloodFlag()
{
	return m_bFirstBloodFlag;
}

inline INT32 CScene::GetWeather(void)
{
	return m_nWeather;
}

inline BOOL CScene::GetDLRecordRole()
{
	return m_bDLRecordRole;
}

inline void CScene::_CheckDLRecordRoleBegin(UINT32 dwTime)
{
	if ((m_nDLSceneID%DL_RECORD_ROLET_INTERVAL) == (dwTime%DL_RECORD_ROLET_INTERVAL))
	{
		m_bDLRecordRole = TRUE;
	}
}

inline void CScene::_CheckDLRecordRoleEnd()
{
	if (m_bDLRecordRole)
	{
		m_bDLRecordRole = FALSE;
	}
}

inline BOOL CScene::IsSabacStarted(void)
{
	return m_bSabacStarted;
}

#endif	// _SCENE_H_
