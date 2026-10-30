#include "stdafx.h"
#include "scene_file_mgr.h"

#include "weight_rand.h"
#include "ifile.h"

#include "id_def.h"
#include "string_def.h"
#include "ai_def.h"
#include "scene.h"
#include "kismet/kismet_node.h"
#include "zone_def.h"
#include "scene_phyx_file_def.h"
#include "game_world.h"
#include "script_mgr.h"
#include "statis_info.h"
#include "read_data_helper.h"
#include "npc_data.h"
#include "doodad_data.h"

/************************************************************************/
/* CSceneStaticData                                                     */
/************************************************************************/

#define SQRT_TABLE_SIZE				(1024 * 4)
#define MAX_OPEN_LIST_DATA_COUNT	(65536)
#define MAX_SEARCH_EDGE_COUNT		(64)

static UINT16	                    s_wSqrtTable[SQRT_TABLE_SIZE];
static UINT16	                    s_wVertexList[MAX_SEARCH_EDGE_COUNT];
static std::map<INT32, INT32>       s_mapNpcCount;


//#define PERFORMANCE_FIND_PATH
#ifdef PERFORMANCE_FIND_PATH 
static UINT64   gs_qwTotalFindPathCount = 0;
static UINT64	gs_qwTotalFindPointCount = 0;
static UINT32	gs_TempFindPathCount = 0;
static UINT32	gs_TempFindPointCount = 0;
static UINT64	gs_qwLastPrintFindTickCount = 0;
#endif

struct OPEN_LIST_DATA : BASE_LIST_NODE
{
    UINT16					wEdge;
    UINT16					wFromEdge;
    UINT16					wCost;
};
static OPEN_LIST_DATA		s_OpenListDataPool[MAX_OPEN_LIST_DATA_COUNT];
static UINT8				s_byVisitTable[MAX_OPEN_LIST_DATA_COUNT / 8];

CSceneStaticData::CSceneStaticData()
{
    m_pcSceneData = NULL;
    m_pNpcLargeGroupTable = NULL;
    m_nNpcLargeGroupCount = 0;
    m_pNpcMediumGroupTable = NULL;
    m_nNpcMediumGroupCount = 0;
    m_pSingleNpcTable = NULL;
    m_nSingleNpcCount = 0;
    m_pDoodadSpawnGroupTable = NULL;
    m_nDoodadSpawnGroupCount = 0;
    m_pServerPointTable = NULL;
    m_nServerPointCount = 0;
    m_pRevivePointTable = NULL;
    m_nRevivePointCount = 0;

    memset(m_PathPointTable, 0, sizeof(m_PathPointTable));

    memset(m_TeleportPointList, 0, sizeof(m_TeleportPointList));
    memset(m_pBTTable, 0, sizeof(m_pBTTable));

    memset(m_ZoneData, NULL, sizeof(m_ZoneData));
    m_pZoneRawData = NULL;
    m_nMaxZoneX = 0;
    m_nMaxZoneY = 0;

    m_nNavVertexCount = 0;
    m_NavVertexData = NULL;
    m_NavFixedVertexData = NULL;
    m_nNavEdgeCount = 0;
    m_NavEdgeData = NULL;

    m_pScript = NULL;
    m_nInstanceZoneTotalSize = 0;
    memset(m_VolumeTreeTable, 0, sizeof(m_VolumeTreeTable));
}

CSceneStaticData::~CSceneStaticData()
{
    UnInit();
}

BOOL CSceneStaticData::Init(void)
{
    LOG_PROCESS_ERROR(sizeof(GRID_DATA) == 16);

    m_pNpcLargeGroupTable = NULL;
    m_nNpcLargeGroupCount = 0;
    m_pNpcMediumGroupTable = NULL;
    m_nNpcMediumGroupCount = 0;
    m_pSingleNpcTable = NULL;
    m_nSingleNpcCount = 0;
    m_pDoodadSpawnGroupTable = NULL;
    m_nDoodadSpawnGroupCount = 0;
    m_pServerPointTable = NULL;
    m_nServerPointCount = 0;
    m_pRevivePointTable = NULL;
    m_nRevivePointCount = 0;

    memset(m_PathPointTable, 0, sizeof(m_PathPointTable));
    memset(m_TeleportPointList, 0, sizeof(m_TeleportPointList));
    memset(m_ZoneData, NULL, sizeof(m_ZoneData));
    m_pZoneRawData = NULL;
    m_nMaxZoneX = m_nMaxZoneY = 0;
    m_nInstanceZoneTotalSize = 0;
    m_pScript = NULL;
    memset(m_pBTTable, 0, sizeof(m_pBTTable));

    for (INT32 i = 0; i < SQRT_TABLE_SIZE; i++)
    {
        s_wSqrtTable[i] = (UINT16)sqrt((double)i);
    }

    for (INT32 i = 0; i < MAX_OPEN_LIST_DATA_COUNT; i++)
    {
        s_OpenListDataPool[i].wEdge = i;
    }

    m_dwMemorySize = sizeof(CSceneStaticData);
    m_dwSceneArea = 0;
    m_dwSceneWalkableArea = 0;
    m_mapSingleNpc.clear();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSceneStaticData::UnInit(void)
{
    INT32 nRetCode = 0;

    _ClearNpcSpawnData();
    _ClearDoodadSpawnData();

    SAFE_DELETE_ARRAY(m_pSingleNpcTable);
    m_nSingleNpcCount = 0;

    SAFE_DELETE_ARRAY(m_pServerPointTable);
    m_nServerPointCount = 0;

    SAFE_DELETE_ARRAY(m_pRevivePointTable);
    m_nRevivePointCount = 0;

    SAFE_DELETE_ARRAY(m_NavVertexData);
    SAFE_DELETE_ARRAY(m_NavFixedVertexData);
    m_nNavVertexCount = 0;

    SAFE_DELETE_ARRAY(m_NavEdgeData);
    m_nNavEdgeCount = 0;

    memset(m_ZoneData, NULL, sizeof(m_ZoneData));
    SAFE_DELETE_ARRAY(m_pZoneRawData);

    if (m_pScript)
    {
        m_pScript = NULL;
    }

    for (INT32 nIndex = evtSceneBegin; nIndex < evtSceneEnd; nIndex++)
    {
        if (m_pBTTable[nIndex])
        {
            nRetCode = CBTManager::GetInstance().DestroyBT(m_pBTTable[nIndex]);
            LOG_CHECK_ERROR(nRetCode);

            m_pBTTable[nIndex] = NULL;
        }
    }

    return TRUE;
}

INT32 CSceneStaticData::GetNpcLargeGroupCount(void)
{
    return m_nNpcLargeGroupCount;
}

const NPC_LARGE_SPAWN_GROUP* CSceneStaticData::GetNpcLargeGroup(INT32 nGroupID)
{
    LOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < m_nNpcLargeGroupCount);
    PROCESS_ERROR(m_pNpcLargeGroupTable[nGroupID].nID == nGroupID);

    return m_pNpcLargeGroupTable + nGroupID;
Exit0:
    return NULL;
}

INT32 CSceneStaticData::GetNpcMediumGroupCount(void)
{
    return m_nNpcMediumGroupCount;
}

const NPC_MEDIUM_SPAWN_GROUP* CSceneStaticData::GetNpcMediumGroup(INT32 nGroupID)
{
    LOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < m_nNpcMediumGroupCount);
    PROCESS_ERROR(m_pNpcMediumGroupTable[nGroupID].nID == nGroupID);

    return m_pNpcMediumGroupTable + nGroupID;
Exit0:
    return NULL;
}

INT32 CSceneStaticData::GetSingleNpcCount(void)
{
    return m_nSingleNpcCount;
}

const NPC_SPAWN_POINT* CSceneStaticData::GetSingleNpc(INT32 nSingleNpcID)
{
    LOG_PROCESS_ERROR(nSingleNpcID >= 0 && nSingleNpcID < m_nSingleNpcCount);
    PROCESS_ERROR(m_pSingleNpcTable[nSingleNpcID].nID == nSingleNpcID);

    return m_pSingleNpcTable + nSingleNpcID;
Exit0:
    return NULL;
}

const NPC_SPAWN_POINT* CSceneStaticData::GetSingleNpc(const char* pcszName)
{
    UINT64 qwNameID = 0;

    LOG_PROCESS_ERROR(pcszName);

    strncpy((char*)&qwNameID, pcszName, sizeof(qwNameID));

    return GetSingleNpcByName(qwNameID);
Exit0:
    return NULL;
}

const NPC_SPAWN_POINT* CSceneStaticData::GetSingleNpcByName(NAMEID qwNameID)
{
    MAP_NAME_2_NPC::iterator it;

    it = m_mapSingleNpc.find(qwNameID);
    LOG_PROCESS_ERROR(it != m_mapSingleNpc.end());

    return it->second;
Exit0:
    return NULL;
}

INT32 CSceneStaticData::GetDoodadSpawnGroupCount(void)
{
    return m_nDoodadSpawnGroupCount;
}

const DOODAD_SPAWN_GROUP* CSceneStaticData::GetDoodadSpawnGroup(INT32 nGroupID)
{
    LOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < m_nDoodadSpawnGroupCount);

    return m_pDoodadSpawnGroupTable + nGroupID;
Exit0:
    return NULL;
}

UINT32 CSceneStaticData::GetServerPointCount(void)
{
    return m_nServerPointCount;
}

const SERVER_POINT* CSceneStaticData::GetServerPoint(const char* pcszName)
{
    UINT64 qwNameID = 0;
    MAP_NAME_2_POINT::iterator it;

    LOG_PROCESS_ERROR(pcszName);

    strncpy((char*)&qwNameID, pcszName, sizeof(qwNameID));

    it = m_mapServerPoint.find(qwNameID);
    LOG_PROCESS_ERROR(it != m_mapServerPoint.end());

    return it->second;
Exit0:
    return NULL;
}

const SERVER_POINT* CSceneStaticData::GetServerPoint(INT32 nPointID)
{
    LOG_PROCESS_ERROR(nPointID >= 0 && nPointID < m_nServerPointCount);
    LOG_PROCESS_ERROR(m_pServerPointTable);
    PROCESS_ERROR(m_pServerPointTable[nPointID].nID == nPointID);

    return m_pServerPointTable + nPointID;
Exit0:
    return NULL;
}

UINT32 CSceneStaticData::GetRevivePointCount(void)
{
    return m_nRevivePointCount;
}

const SERVER_POINT* CSceneStaticData::GetRevivePoint(INT32 nPointID)
{
    LOG_PROCESS_ERROR(nPointID >= 0 && nPointID < m_nServerPointCount);
    LOG_PROCESS_ERROR(m_pRevivePointTable);
    PROCESS_ERROR(m_pRevivePointTable[nPointID].nID == nPointID);

    return m_pRevivePointTable + nPointID;
Exit0:
    return NULL;
}

const SERVER_POINT* CSceneStaticData::GetNearestRevivePoint(INT32 nFaction, INT64 llX, INT64 llY, INT64 llZ)
{
    const SERVER_POINT* pResult = NULL;
    INT64 llMinDistance = (INT64)INT_MAX * INT_MAX;

    PROCESS_ERROR(m_pRevivePointTable);

    for (INT32 nIndex = 0; nIndex < m_nServerPointCount; nIndex++)
    {
        REVIVE_POINT* pRevivePoint = m_pRevivePointTable + nIndex;
        if (pRevivePoint->nFaction != nFaction)
            continue;
        INT64 llDistance = g_GetDistance3D(llX, llY, llZ, pRevivePoint->nX, pRevivePoint->nY, pRevivePoint->nZ);
        if (llDistance < llMinDistance)
        {
            pResult = pRevivePoint;
            llMinDistance = llDistance;
        }
    }

    return pResult;
Exit0:

    return NULL;
}

INT32 CSceneStaticData::GetMaxVolumeTriggerCount(INT32 nVolumeID)
{
    PROCESS_ERROR(nVolumeID >= 0 && nVolumeID < MAX_VOLUME_COUNT);

    return m_nMaxVolumeTriggerCount[nVolumeID];
Exit0:
    return -1;
}

INT32 CSceneStaticData::GetPathPointCount(INT32 nPathID)
{
    LOG_PROCESS_ERROR(nPathID > 0 && nPathID < MAX_PATROL_PATH_ID);

    return m_PathPointTable[nPathID].nPointCount;
Exit0:
    return 0;
}

const SERVER_POINT* CSceneStaticData::GetPathPoint(INT32 nPathID, INT32 nPointIndex)
{
    BOOL nRetCode = FALSE;

    LOG_PROCESS_ERROR(nPathID > 0 && nPathID < MAX_PATROL_PATH_ID);
    LOG_PROCESS_ERROR(nPointIndex >= 0 && nPointIndex < m_PathPointTable[nPathID].nPointCount);

    return m_PathPointTable[nPathID].PointList + nPointIndex;
Exit0:
    return NULL;
}

const SCENE_TELEPORTS* CSceneStaticData::FindTeleportPoint(INT32 nPointID)
{
    LOG_PROCESS_ERROR(nPointID > 0 && nPointID < MAX_TELEPORT_POINT_PER_SCENE);
    PROCESS_ERROR(m_TeleportPointList[nPointID].ID > 0);
    LOG_PROCESS_ERROR(nPointID == m_TeleportPointList[nPointID].ID);

    return &(m_TeleportPointList[nPointID]);
Exit0:
    return NULL;
}

INT32 CSceneStaticData::GetLowerZ(INT64 llX, INT64 llY, INT64 llZ)
{
    INT32 nRetCode = 0;
    const ZONE_DATA* pcZone;
    const GRID_DATA* pcGrid;

    nRetCode = GetLowerGrid(llX, llY, llZ, pcZone, pcGrid);
    PROCESS_ERROR(nRetCode);

    return pcGrid->uHighLayer * GRID_LAYER_HEIGHT;
Exit0:
    return -1;
}

BOOL CSceneStaticData::CheckGrid(INT64 llX, INT64 llY, INT64 llZ)
{
    INT32 nRetCode = 0;
    const ZONE_DATA* pcZone;
    const GRID_DATA* pcGrid;

    nRetCode = GetLowerGrid(llX, llY, llZ, pcZone, pcGrid);
    PROCESS_ERROR(nRetCode);
    PROCESS_ERROR(pcZone && pcGrid);
    PROCESS_ERROR(!pcGrid->bOutOfPoly);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSceneStaticData::GetLowerGrid(INT64 llX, INT64 llY, INT64 llZ, const ZONE_DATA*& rpcZoneData, const GRID_DATA*& rpcGridData)
{
    INT32 nZoneX = (INT32)(llX / ZONE_WIDTH);
    INT32 nZoneY = (INT32)(llY / ZONE_WIDTH);
    INT32 nGridXInZone = (INT32)((llX / GRID_WIDTH) % GRID_PER_ZONE);
    INT32 nGridYInZone = (INT32)((llY / GRID_WIDTH) % GRID_PER_ZONE);
    UINT16 usLayer = (UINT16)(llZ / GRID_LAYER_HEIGHT);
    ZONE_DATA* pZoneData = NULL;
    UINT16 usGridIndex = NULL;
    UINT16 usNextIndex = NULL;

    PROCESS_ERROR(llX >= 0 && llY >= 0 && llZ >= 0);
    PROCESS_ERROR(nZoneX >= 0 && nZoneX < MAX_ZONE_PER_SCENE);
    PROCESS_ERROR(nZoneY >= 0 && nZoneY < MAX_ZONE_PER_SCENE);

    pZoneData = m_ZoneData[nZoneX][nZoneY];
    PROCESS_ERROR(pZoneData);

    usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];
    PROCESS_ERROR(usGridIndex);

    usNextIndex = pZoneData->GridList[usGridIndex].uNextGrid;
    while (usNextIndex != NULL && usLayer > pZoneData->GridList[usNextIndex].uLowLayer)
    {
        usGridIndex = usNextIndex;
        usNextIndex = pZoneData->GridList[usNextIndex].uNextGrid;
    }

    rpcZoneData = pZoneData;
    rpcGridData = pZoneData->GridList + usGridIndex;

    return TRUE;
Exit0:
    rpcZoneData = NULL;
    rpcGridData = NULL;

    return FALSE;
}

const GRID_DATA* CSceneStaticData::GetGroundGrid(INT64 llX, INT64 llY)
{
    INT32 nZoneX = (INT32)(llX / ZONE_WIDTH);
    INT32 nZoneY = (INT32)(llY / ZONE_WIDTH);
    INT32 nGridXInZone = (INT32)((llX / GRID_WIDTH) % GRID_PER_ZONE);
    INT32 nGridYInZone = (INT32)((llY / GRID_WIDTH) % GRID_PER_ZONE);
    ZONE_DATA* pZoneData = NULL;

    PROCESS_ERROR(llX >= 0 && llY >= 0);
    PROCESS_ERROR(nZoneX >= 0 && nZoneX < MAX_ZONE_PER_SCENE);
    PROCESS_ERROR(nZoneY >= 0 && nZoneY < MAX_ZONE_PER_SCENE);

    pZoneData = m_ZoneData[nZoneX][nZoneY];
    PROCESS_ERROR(pZoneData);

    return pZoneData->GridList + pZoneData->GroundGrid[nGridXInZone][nGridYInZone];
Exit0:
    return NULL;
}

const GRID_DATA* CSceneStaticData::GetNextGrid(INT64 llX, INT64 llY, const GRID_DATA* pcGrid)
{
    INT32 nZoneX = (INT32)(llX / ZONE_WIDTH);
    INT32 nZoneY = (INT32)(llY / ZONE_WIDTH);
    ZONE_DATA* pZoneData = NULL;

    LOG_PROCESS_ERROR(pcGrid);
    PROCESS_ERROR(pcGrid->uNextGrid > 0);

    PROCESS_ERROR(llX >= 0 && llY >= 0);
    PROCESS_ERROR(nZoneX >= 0 && nZoneX < MAX_ZONE_PER_SCENE);
    PROCESS_ERROR(nZoneY >= 0 && nZoneY < MAX_ZONE_PER_SCENE);

    pZoneData = m_ZoneData[nZoneX][nZoneY];
    PROCESS_ERROR(pZoneData);

    LOG_PROCESS_ERROR((INT32)(pcGrid->uNextGrid) <= pZoneData->nGridCount);

    return pZoneData->GridList + pcGrid->uNextGrid;
Exit0:
    return NULL;
}

#define MAX_HEIGHT_OFFSET   (256)

inline BOOL _CheckGrid(const GRID_DATA* pcGrid, INT32 nCenterZ)
{
    if (pcGrid && !pcGrid->bOutOfPoly && !pcGrid->uDoor)
    {
        return TRUE;
    }

    return FALSE;
}

// Find available positions around (nCenterX, nCenterY, nCenterZ), within nMaxRange, returning nPosCount positions
INT32 CSceneStaticData::GetNearestAvailablePos(INT32 nCenterX, INT32 nCenterY, INT32 nCenterZ,
    INT32 nMaxRange, INT32 nPosCount, INT32 nPos[][3])
{
    INT32 nResult = 0;
    INT32 nRetCode = 0;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;

    if (nCenterX <= 0 || nCenterY <= 0 || nCenterZ <= 0 ||
        nCenterX >= MAX_SCENE_WIDTH || nCenterY >= MAX_SCENE_WIDTH)
        return 0;

    if (nPosCount <= 0)
        return 0;
    nCenterX = nCenterX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    nCenterY = nCenterY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;

    GetLowerGrid(nCenterX, nCenterY, nCenterZ, pcZone, pcGrid);
    if (_CheckGrid(pcGrid, nCenterZ))
    {
        INT32 nZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;
        nPos[nResult][0] = nCenterX;
        nPos[nResult][1] = nCenterY;
        nPos[nResult++][2] = nZ;
        if (nResult >= nPosCount)
            return nResult;
    }

    for (INT32 nRange = 1; nRange < nMaxRange; nRange++)
    {
        for (INT32 nX = nCenterX - nRange * GRID_WIDTH; nX < nCenterX + nRange * GRID_WIDTH; nX += GRID_WIDTH)
        {
            INT32 nY = nCenterY - nRange * GRID_WIDTH;

            GetLowerGrid(nX, nY, nCenterZ, pcZone, pcGrid);
            if (_CheckGrid(pcGrid, nCenterZ))
            {
                INT32 nZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;
                nPos[nResult][0] = nX;
                nPos[nResult][1] = nY;
                nPos[nResult++][2] = nZ;
                if (nResult >= nPosCount)
                    return nResult;
            }
        }

        for (INT32 nY = nCenterY - nRange * GRID_WIDTH; nY < nCenterY + nRange * GRID_WIDTH; nY += GRID_WIDTH)
        {
            INT32 nX = nCenterX + nRange * GRID_WIDTH;

            GetLowerGrid(nX, nY, nCenterZ, pcZone, pcGrid);
            if (_CheckGrid(pcGrid, nCenterZ))
            {
                INT32 nZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;
                nPos[nResult][0] = nX;
                nPos[nResult][1] = nY;
                nPos[nResult++][2] = nZ;
                if (nResult >= nPosCount)
                    return nResult;
            }
        }

        for (INT32 nX = nCenterX + nRange * GRID_WIDTH; nX > nCenterX - nRange * GRID_WIDTH; nX -= GRID_WIDTH)
        {
            INT32 nY = nCenterY + nRange * GRID_WIDTH;

            GetLowerGrid(nX, nY, nCenterZ, pcZone, pcGrid);
            if (_CheckGrid(pcGrid, nCenterZ))
            {
                INT32 nZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;
                nPos[nResult][0] = nX;
                nPos[nResult][1] = nY;
                nPos[nResult++][2] = nZ;
                if (nResult >= nPosCount)
                    return nResult;
            }
        }

        for (INT32 nY = nCenterY + nRange * GRID_WIDTH; nY > nCenterY - nRange * GRID_WIDTH; nY -= GRID_WIDTH)
        {
            INT32 nX = nCenterX - nRange * GRID_WIDTH;

            GetLowerGrid(nX, nY, nCenterZ, pcZone, pcGrid);
            if (_CheckGrid(pcGrid, nCenterZ))
            {
                INT32 nZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;
                nPos[nResult][0] = nX;
                nPos[nResult][1] = nY;
                nPos[nResult++][2] = nZ;
                if (nResult >= nPosCount)
                    return nResult;
            }
        }
    }

    return nResult;
}

void CSceneStaticData::_ClearNpcSpawnData(void)
{
    INT32 nRetCode = 0;

    if (m_pNpcMediumGroupTable)
    {
        for (INT32 nMediumGroupID = 0; nMediumGroupID < m_nNpcMediumGroupCount; nMediumGroupID++)
        {
            NPC_MEDIUM_SPAWN_GROUP* pMediumGroup = m_pNpcMediumGroupTable + nMediumGroupID;

            for (INT32 nSmallGroupID = 0; nSmallGroupID < pMediumGroup->nSmallGroupCount; nSmallGroupID++)
            {
                SAFE_FREE(pMediumGroup->SmallGroupList[nSmallGroupID]);
            }
        }
    }

    SAFE_DELETE_ARRAY(m_pNpcLargeGroupTable);
    SAFE_DELETE_ARRAY(m_pNpcMediumGroupTable);
    m_nNpcLargeGroupCount = 0;
}

void CSceneStaticData::_ClearDoodadSpawnData(void)
{
    INT32 nRetCode = 0;

    if (m_pDoodadSpawnGroupTable)
    {
        for (INT32 nDoodadGroupID = 0; nDoodadGroupID < m_nDoodadSpawnGroupCount; nDoodadGroupID++)
        {
            SAFE_DELETE_ARRAY(m_pDoodadSpawnGroupTable[nDoodadGroupID].pPointList);
        }
    }
    SAFE_DELETE_ARRAY(m_pDoodadSpawnGroupTable);
    m_nDoodadSpawnGroupCount = 0;

}

void CSceneStaticData::_CheckSpawnPos(void)
{
    INT32 nRetCode = 0;
    const GRID_DATA* pGridData = NULL;
    const ZONE_DATA* pZoneData = NULL;

    for (INT32 nIndex = 1; nIndex < m_nSingleNpcCount; nIndex++)
    {
        NPC_SPAWN_POINT* pPoint = m_pSingleNpcTable + nIndex;
        if (pPoint->nID != nIndex)
            continue;

        nRetCode = GetLowerGrid(pPoint->nX, pPoint->nY, pPoint->nZ, pZoneData, pGridData);
        if (pGridData == NULL || pGridData->bOutOfPoly)
        {
            INT32 nX = pPoint->nX / POINT_PER_CM;
            INT32 nY = pPoint->nY / POINT_PER_CM;
            INT32 nZ = pPoint->nZ / POINT_PER_CM;
            ConvertCoordN2O(nX, nY, nZ);

            WRN("spawn point [%d, %d, %d] of single npc %d is invalid", nX, nY, nZ, pPoint->nID);
        }
    }

    for (INT32 nLargeGroupIndex = 0; nLargeGroupIndex < m_nNpcLargeGroupCount; nLargeGroupIndex++)
    {
        NPC_LARGE_SPAWN_GROUP* pLargeGroup = m_pNpcLargeGroupTable + nLargeGroupIndex;

        for (INT32 nMediumGroupIndex = 0; nMediumGroupIndex < pLargeGroup->nMediumGroupCount; nMediumGroupIndex++)
        {
            NPC_MEDIUM_SPAWN_GROUP* pMediumGroup = m_pNpcMediumGroupTable + pLargeGroup->MediumGroupList[nMediumGroupIndex];

            for (INT32 nSmallGroupIndex = 0; nSmallGroupIndex < pMediumGroup->nSmallGroupCount; nSmallGroupIndex++)
            {
                NPC_SMALL_SPAWN_GROUP* pSmallGroup = pMediumGroup->SmallGroupList[nSmallGroupIndex];

                for (INT32 nPointIndex = 0; nPointIndex < pSmallGroup->nPointCount; nPointIndex++)
                {
                    NPC_SPAWN_POINT* pPoint = pSmallGroup->PointList + nPointIndex;

                    nRetCode = GetLowerGrid(pPoint->nX, pPoint->nY, pPoint->nZ, pZoneData, pGridData);
                    if (pGridData == NULL || pGridData->bOutOfPoly)
                    {
                        INT32 nX = pPoint->nX / POINT_PER_CM;
                        INT32 nY = pPoint->nY / POINT_PER_CM;
                        INT32 nZ = pPoint->nZ / POINT_PER_CM;
                        ConvertCoordN2O(nX, nY, nZ);

                        WRN("spawn point [%d, %d, %d] of npc point %d (%d, %d, %d, %d) is invalid",
                            nX, nY, nZ, pPoint->nID, nLargeGroupIndex, nMediumGroupIndex, nSmallGroupIndex, nPointIndex);
                    }

                }
            }
        }
    }

    return;
}

inline INT32 _QuickSqrt(INT64 nValue)
{
    nValue = nValue / GRID_WIDTH / GRID_WIDTH / 4;
    PROCESS_ERROR(nValue < SQRT_TABLE_SIZE);

    return s_wSqrtTable[nValue];
Exit0:
    return 0xFF;
}

inline INT64 _CheckPos(NAV_VERTEX_DATA* pPoint, NAV_VERTEX_DATA* pLeftPoint, NAV_VERTEX_DATA* pRightPoint)
{
    INT64 xp = pPoint->nX;
    INT64 yp = pPoint->nY;
    INT64 xl = pLeftPoint->nX;
    INT64 yl = pLeftPoint->nY;
    INT64 xr = pRightPoint->nX;
    INT64 yr = pRightPoint->nY;

    return ((yr - yl) * xp + (xl - xr) * yp + (xr * yl - xl * yr));
}

// Find a route from (nSrcX, nSrcY, nSrcZ) to (nDestX, nDestY, nDestZ)
// rnResultSize: number of route positions; pnResult: route points
BOOL CSceneStaticData::FindPath(INT32 nSrcX, INT32 nSrcY, INT32 nSrcZ, UINT16 wSrcEdge,
    INT32 nDestX, INT32 nDestY, INT32 nDestZ, UINT16 wDestEdge,
    INT32& rnResultSize, INT32* pnResult)
{
    //PROFILE_START();

    //PROFILE_POINT(0);
#ifdef PERFORMANCE_FIND_PATH 
    gs_TempFindPathCount++;
    gs_qwTotalFindPathCount++;
#endif

#ifndef _ROBOT
    g_ChangeStatisInfo(statFindPathCount, 1);
#endif

    OPEN_LIST_DATA* pOpenData;
    OPEN_LIST_DATA* pInsertPos;
    CBaseList OpenList;
    UINT16 wCost;
    INT32 nVertexCount;
    UINT16 wPortalApex, wPortalLeft, wPortalRight;
    INT32 nPortalApex, nPortalLeft, nPortalRight;
    INT32 nResultSize;
    INT32 PushCount = 0;
    INT32 PopCount = 0;


    PROCESS_ERROR(rnResultSize >= 2);
    PROCESS_ERROR(wSrcEdge < m_nNavEdgeCount);
    PROCESS_ERROR(wDestEdge < m_nNavEdgeCount);

    nResultSize = 0;
    pnResult[0] = nSrcX;
    pnResult[1] = nSrcY;
    pnResult[2] = nSrcZ;
    nResultSize++;

    // check in same poly
    if (wSrcEdge == wDestEdge)
    {
        LOG_PROCESS_ERROR(nResultSize < rnResultSize);
        pnResult[nResultSize * 3 + 0] = nDestX;
        pnResult[nResultSize * 3 + 1] = nDestY;
        pnResult[nResultSize * 3 + 2] = nDestZ;
        nResultSize++;
        rnResultSize = nResultSize;

        //PROFILE_POINT(0);

        //PROFILE_END();

#ifndef _ROBOT
        g_ChangeStatisInfo(statFindPathSamePolyReturnCount, 1);
#endif
        return TRUE;
    }

    // search

    memset(s_byVisitTable, 0, m_nNavEdgeCount / 8 + 1);
    OpenList.Clear();

    // prepare src edge
    pOpenData = s_OpenListDataPool + wDestEdge;

    s_byVisitTable[wDestEdge / 8] |= 1 << (wDestEdge % 8);
    pOpenData->pPrev = pOpenData->pNext = NULL;
    pOpenData->wFromEdge = 0xFFFF;
    pOpenData->wCost = _QuickSqrt(g_GetDistance3D(nDestX, nDestY, nDestZ, m_NavVertexData[pOpenData->wEdge].nX,
        m_NavVertexData[pOpenData->wEdge].nY, m_NavVertexData[pOpenData->wEdge].nZ));

    OpenList.PushHead(pOpenData);

    for (INT32 nIndex = 0; nIndex < MAX_EDGE_PER_POLY - 1 && m_NavEdgeData[wDestEdge].wLinkEdge[nIndex] != 0xFFFF; nIndex++)
    {
        pOpenData = s_OpenListDataPool + m_NavEdgeData[wDestEdge].wLinkEdge[nIndex];
        s_byVisitTable[pOpenData->wEdge / 8] |= 1 << (pOpenData->wEdge % 8);

        pOpenData->pPrev = pOpenData->pNext = NULL;
        pOpenData->wFromEdge = 0xFFFF;
        pOpenData->wCost = _QuickSqrt(g_GetDistance3D(nDestX, nDestY, nDestZ, m_NavVertexData[pOpenData->wEdge].nX,
            m_NavVertexData[pOpenData->wEdge].nY, m_NavVertexData[pOpenData->wEdge].nZ));

        pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
        while (pInsertPos != &OpenList.Rear() && pInsertPos->wCost < pOpenData->wCost)
            pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;
        OpenList.InsertBefore(pOpenData, pInsertPos);
    }

    while (!OpenList.IsEmpty())
    {
        pOpenData = (OPEN_LIST_DATA*)(OpenList.PopHead());
        LOG_PROCESS_ERROR(pOpenData);

        PopCount++;
        UINT16 wPairEdge = m_NavEdgeData[pOpenData->wEdge].wPairEdge;

        if (m_NavEdgeData[pOpenData->wEdge].wStartEdge == wSrcEdge)
            break;

        if (wPairEdge == 0xFFFF)
            continue;

        if (m_NavEdgeData[wPairEdge].wStartEdge == wSrcEdge)
        {
            wCost = pOpenData->wCost + _QuickSqrt(g_GetDistance3D(nSrcX, nSrcY, nSrcZ, m_NavVertexData[wPairEdge].nX,
                m_NavVertexData[wPairEdge].nY, m_NavVertexData[wPairEdge].nZ));

            if ((s_byVisitTable[wPairEdge / 8] & (1 << (wPairEdge % 8))) == 0)
            {
                s_byVisitTable[wPairEdge / 8] |= 1 << (wPairEdge % 8);

                s_OpenListDataPool[wPairEdge].pPrev = s_OpenListDataPool[wPairEdge].pNext = NULL;
                s_OpenListDataPool[wPairEdge].wCost = wCost;
                s_OpenListDataPool[wPairEdge].wFromEdge = pOpenData->wFromEdge;

                pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
                while (pInsertPos != &OpenList.Rear() && pInsertPos->wCost < s_OpenListDataPool[wPairEdge].wCost)
                    pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

                OpenList.InsertBefore(s_OpenListDataPool + wPairEdge, pInsertPos);
                PushCount++;
            }
            else if (wCost < s_OpenListDataPool[wPairEdge].wCost)
            {
                s_OpenListDataPool[wPairEdge].wFromEdge = pOpenData->wFromEdge;
                s_OpenListDataPool[wPairEdge].wCost = wCost;

                CBaseList::Remove(s_OpenListDataPool + wPairEdge);

                pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
                while (pInsertPos != &OpenList.Rear() && pInsertPos->wCost <= s_OpenListDataPool[wPairEdge].wCost)
                    pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

                OpenList.InsertBefore(s_OpenListDataPool + wPairEdge, pInsertPos);
                PushCount++;
            }
        }
        else
        {
            if ((s_byVisitTable[wPairEdge / 8] & (1 << (wPairEdge % 8))) == 0)
            {
                s_byVisitTable[wPairEdge / 8] |= 1 << (wPairEdge % 8);
                s_OpenListDataPool[wPairEdge].pPrev = s_OpenListDataPool[wPairEdge].pNext = NULL;
                s_OpenListDataPool[wPairEdge].wCost = pOpenData->wCost;
                s_OpenListDataPool[wPairEdge].wFromEdge = pOpenData->wFromEdge;
            }
            else if (pOpenData->wCost < s_OpenListDataPool[wPairEdge].wCost)
            {
                s_OpenListDataPool[wPairEdge].wFromEdge = pOpenData->wFromEdge;
                s_OpenListDataPool[wPairEdge].wCost = pOpenData->wCost;

                CBaseList::Remove(s_OpenListDataPool + wPairEdge);

                pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
                while (pInsertPos != &OpenList.Rear() && pInsertPos->wCost <= s_OpenListDataPool[wPairEdge].wCost)
                    pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

                OpenList.InsertBefore(s_OpenListDataPool + wPairEdge, pInsertPos);
                PushCount++;
            }
        }

        NAV_EDGE_DATA* pPairEdge = m_NavEdgeData + m_NavEdgeData[pOpenData->wEdge].wPairEdge;
        for (INT32 nIndex = 0; nIndex < MAX_EDGE_PER_POLY - 1 && pPairEdge->wLinkEdge[nIndex] != 0xFFFF; nIndex++)
        {
            UINT16 wEdge = pPairEdge->wLinkEdge[nIndex];
            OPEN_LIST_DATA* pLinkData = (OPEN_LIST_DATA*)s_OpenListDataPool + wEdge;

            wCost = pOpenData->wCost + pPairEdge->wLinkCost[nIndex];
            if (m_NavEdgeData[wEdge].wStartEdge == wSrcEdge)
            {
                wCost += _QuickSqrt(g_GetDistance3D(nSrcX, nSrcY, nSrcZ, m_NavVertexData[wEdge].nX,
                    m_NavVertexData[wEdge].nY, m_NavVertexData[wEdge].nZ));
            }

            if ((s_byVisitTable[wEdge / 8] & (1 << (wEdge % 8))) == 0)
            {
                s_byVisitTable[wEdge / 8] |= 1 << (wEdge % 8);
                pLinkData->pPrev = pLinkData->pNext = NULL;
                pLinkData->wFromEdge = pOpenData->wEdge;
                pLinkData->wCost = wCost;

                if (pLinkData->wCost < ((OPEN_LIST_DATA*)OpenList.Rear().pPrev)->wCost / 2)
                {
                    pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
                    while (pInsertPos != &OpenList.Rear() && pInsertPos->wCost <= pLinkData->wCost)
                        pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

                    OpenList.InsertBefore(pLinkData, pInsertPos);
                }
                else
                {
                    pInsertPos = (OPEN_LIST_DATA*)OpenList.Rear().pPrev;
                    while (pInsertPos != &OpenList.Head() && pInsertPos->wCost > pLinkData->wCost)
                        pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pPrev;

                    OpenList.InsertAfter(pLinkData, pInsertPos);
                }
                PushCount++;
            }
            else if (wCost < pLinkData->wCost)
            {
                pLinkData->wFromEdge = pOpenData->wEdge;
                pLinkData->wCost = wCost;

                CBaseList::Remove(pLinkData);

                if (pLinkData->wCost < ((OPEN_LIST_DATA*)OpenList.Rear().pPrev)->wCost / 2)
                {
                    pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
                    while (pInsertPos != &OpenList.Rear() && pInsertPos->wCost <= pLinkData->wCost)
                        pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

                    OpenList.InsertBefore(pLinkData, pInsertPos);
                }
                else
                {
                    pInsertPos = (OPEN_LIST_DATA*)OpenList.Rear().pPrev;
                    while (pInsertPos != &OpenList.Head() && pInsertPos->wCost > pLinkData->wCost)
                        pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pPrev;

                    OpenList.InsertAfter(pLinkData, pInsertPos);
                }

                PushCount++;
            }
        }
    }
    PROCESS_ERROR(!OpenList.IsEmpty());

    m_NavVertexData[m_nNavVertexCount].nX = nSrcX;
    m_NavVertexData[m_nNavVertexCount].nY = nSrcY;
    m_NavVertexData[m_nNavVertexCount].nZ = nSrcZ;
    m_NavVertexData[m_nNavVertexCount + 1].nX = nDestX;
    m_NavVertexData[m_nNavVertexCount + 1].nY = nDestY;
    m_NavVertexData[m_nNavVertexCount + 1].nZ = nDestZ;

    m_NavFixedVertexData[m_nNavVertexCount].nX = nSrcX;
    m_NavFixedVertexData[m_nNavVertexCount].nY = nSrcY;
    m_NavFixedVertexData[m_nNavVertexCount].nZ = nSrcZ;
    m_NavFixedVertexData[m_nNavVertexCount + 1].nX = nDestX;
    m_NavFixedVertexData[m_nNavVertexCount + 1].nY = nDestY;
    m_NavFixedVertexData[m_nNavVertexCount + 1].nZ = nDestZ;

    nVertexCount = 0;
    s_wVertexList[nVertexCount++] = m_nNavVertexCount;
    if (m_NavEdgeData[pOpenData->wEdge].wPairEdge != 0xFFFF)
        s_wVertexList[nVertexCount++] = m_NavEdgeData[pOpenData->wEdge].wPairEdge;
    while (pOpenData->wFromEdge != 0xFFFF && nVertexCount < MAX_SEARCH_EDGE_COUNT - 1)
    {
        s_wVertexList[nVertexCount++] = pOpenData->wFromEdge;
        pOpenData = s_OpenListDataPool + pOpenData->wFromEdge;
    }
    LOG_CHECK_ERROR(nVertexCount < MAX_SEARCH_EDGE_COUNT - 1);
    s_wVertexList[nVertexCount++] = m_nNavVertexCount + 1;

    wPortalApex = wPortalLeft = wPortalRight = m_nNavVertexCount;
    nPortalApex = nPortalLeft = nPortalRight = 0;

    for (INT32 nVertexIndex = 1; nVertexIndex < nVertexCount; nVertexIndex++)
    {
        UINT16 wLeftPoint, wRightPoint;

        if (nVertexIndex + 1 < nVertexCount)
        {
            // Next portal.
            NAV_EDGE_DATA* pEdgeData = m_NavEdgeData + s_wVertexList[nVertexIndex];
            wLeftPoint = pEdgeData->wRightPoint;
            wRightPoint = pEdgeData->wLeftPoint;
        }
        else
        {
            // End of the path.
            wLeftPoint = m_nNavVertexCount + 1;
            wRightPoint = m_nNavVertexCount + 1;
        }

        // Right vertex.
        if (_CheckPos(m_NavVertexData + wPortalApex, m_NavVertexData + wPortalRight, m_NavVertexData + wRightPoint) >= 0)
        {
            if (wPortalApex == wPortalRight || _CheckPos(m_NavVertexData + wPortalApex, m_NavVertexData + wPortalLeft, m_NavVertexData + wRightPoint) < 0)
            {
                wPortalRight = wRightPoint;
                nPortalRight = nVertexIndex;
            }
            else
            {
                wPortalApex = wPortalLeft;
                nPortalApex = nPortalLeft;

                //if (g_GetDistance2D(pnResult[nResultSize * 3 - 3], pnResult[nResultSize * 3 - 2], m_NavVertexData[wPortalApex].nX, 
                //	m_NavVertexData[wPortalApex].nY) >= GRID_WIDTH * GRID_WIDTH)
                {
                    LOG_CHECK_ERROR(nResultSize < rnResultSize);
                    if (nResultSize < rnResultSize)
                    {
                        pnResult[nResultSize * 3 + 0] = m_NavFixedVertexData[wPortalApex].nX;
                        pnResult[nResultSize * 3 + 1] = m_NavFixedVertexData[wPortalApex].nY;
                        pnResult[nResultSize * 3 + 2] = m_NavFixedVertexData[wPortalApex].nZ;
                        nResultSize++;
                    }
                }

                wPortalRight = wPortalApex;
                nPortalRight = nPortalApex;

                // Restart
                nVertexIndex = nPortalApex;

                continue;
            }
        }

        // Left vertex.
        if (_CheckPos(m_NavVertexData + wPortalApex, m_NavVertexData + wPortalLeft, m_NavVertexData + wLeftPoint) <= 0)
        {
            if (wPortalApex == wPortalLeft || _CheckPos(m_NavVertexData + wPortalApex, m_NavVertexData + wPortalRight, m_NavVertexData + wLeftPoint) > 0)
            {
                wPortalLeft = wLeftPoint;
                nPortalLeft = nVertexIndex;
            }
            else
            {
                wPortalApex = wPortalRight;
                nPortalApex = nPortalRight;

                //if (g_GetDistance2D(pnResult[nResultSize * 3 - 3], pnResult[nResultSize * 3 - 2], m_NavVertexData[wPortalApex].nX, 
                //	m_NavVertexData[wPortalApex].nY) >= GRID_WIDTH * GRID_WIDTH)
                {
                    LOG_CHECK_ERROR(nResultSize < rnResultSize);
                    if (nResultSize < rnResultSize)
                    {
                        pnResult[nResultSize * 3 + 0] = m_NavFixedVertexData[wPortalApex].nX;
                        pnResult[nResultSize * 3 + 1] = m_NavFixedVertexData[wPortalApex].nY;
                        pnResult[nResultSize * 3 + 2] = m_NavFixedVertexData[wPortalApex].nZ;
                        nResultSize++;
                    }
                }

                wPortalLeft = wPortalApex;
                nPortalLeft = nPortalApex;

                // Restart
                nVertexIndex = nPortalApex;

                continue;
            }
        }
    }

    LOG_PROCESS_ERROR(nResultSize < rnResultSize);
    pnResult[nResultSize * 3 + 0] = nDestX;
    pnResult[nResultSize * 3 + 1] = nDestY;
    pnResult[nResultSize * 3 + 2] = nDestZ;
    nResultSize++;

    rnResultSize = nResultSize;


#ifdef PERFORMANCE_FIND_PATH
    gs_qwTotalFindPointCount += rnResultSize;
    gs_TempFindPointCount += rnResultSize;
    UINT64 qwCurTick = GetTickCountEx();
    if (gs_qwLastPrintFindTickCount < qwCurTick)
    {
        INT32 nValue = gs_TempFindPathCount / 60;
        INT32 nValue2 = gs_TempFindPointCount / gs_TempFindPathCount;
        CRI("findPath total count %lld, cur count: %d per minutes, %d per second",
            gs_qwTotalFindPathCount, gs_TempFindPathCount, nValue);

        CRI("Path Point total count %lld, cur count: %d, %d per FindPath function",
            gs_qwTotalFindPointCount, gs_TempFindPointCount, nValue2);

        gs_TempFindPathCount = 0;
        gs_TempFindPointCount = 0;
        gs_qwLastPrintFindTickCount = qwCurTick + 60 * 1000;
    }
#endif


    //PROFILE_POINT(0);

    //PROFILE_END();
    //INF("result size : %d, push count : %d, pop count : %d", nResultSize, PushCount, PopCount);

#ifndef _ROBOT
    g_ChangeStatisInfo(statFindPathNormalReturnCount, 1);
#endif
    OpenList.Clear();

    return TRUE;
Exit0:
    CRI("failed to find path from (%d, %d, %d) to (%d, %d, %d) scene %d",
        nSrcX / POINT_PER_CM, nSrcY / POINT_PER_CM, nSrcZ / POINT_PER_CM,
        nDestX / POINT_PER_CM, nDestY / POINT_PER_CM, nDestZ / POINT_PER_CM,
        m_pcSceneData->nSceneID);
    OpenList.Clear();

    return FALSE;
}

/************************************************************************/
/* CSceneFileMgr                                                        */
/************************************************************************/
CSceneFileMgr CSceneFileMgr::ms_Instance;

CSceneFileMgr::CSceneFileMgr()
{
    memset(m_lstTeleportPlatforms, 0, sizeof(m_lstTeleportPlatforms));
}

CSceneFileMgr::~CSceneFileMgr()
{
    UnInit();
}

std::set<INT32> setNpcID =
{
    841, 838, 839, 842, 843, 844, 845, 846, 847, 848, 849, 850,
    3180, 3181, 3182, 3183, 3184, 3185, 3186, 3187, 3188, 3189,
    3190, 3191, 3094, 3095, 3096, 3097, 3098, 3099, 3100, 3101,
    3102, 3103, 3104, 3105, 3106, 3107, 3108, 2268, 3120, 3121,
    3122, 3123, 3124, 3125, 3126, 3127, 3128, 3159, 3160, 3161,
    3162, 3163, 3164, 3150, 3151, 3152, 3153, 3154, 3155, 3156,
    3157, 3158, 9707, 9708, 9709, 9710, 9711, 9712, 3210, 3211,
    3212, 3213, 3214, 3215, 1019, 1020, 3216, 3217, 3218, 3219,
    3220, 3221, 3240, 3241, 3242, 1045, 1046, 1047, 1048, 1049,
    1050, 1051, 1052, 1053, 1054, 1055, 1056, 9622, 9623, 2251,
    2420, 2422, 3506, 1086, 1087, 1088, 1089, 1090, 3300, 3301,
    3302, 3303, 3304, 3305, 3306, 9628, 9629, 9630, 9722, 9723,
    9724, 9725, 9726, 9727, 2315, 2316, 2317, 2323, 2327, 1097,
    1109, 1110, 1111, 1120, 1121, 1122, 1103, 1104, 1105, 1106,
    1107, 1108, 1414, 1415, 1416, 1417, 1424, 1425, 1426, 1427,
    1434, 1435, 1436, 1437, 2115, 2117, 2118, 2106, 2175, 9737,
    9738, 9739, 9740, 9741, 9742, 5137, 5138, 5139, 304 , 307 ,
    338 , 339 , 2500, 2501, 3332, 3333, 3334, 3335, 3336, 3337,
    3345, 330 , 331 , 332 , 333 , 334 , 335 , 336 , 337 , 340 ,
    3358, 2303, 2306, 1140, 1145, 1146, 1136, 1141, 1143, 1144,
    1147, 1148, 1159,
};

BOOL CSceneFileMgr::_LoadSingleNpc(const SCENE_DATA* pSceneData, CSceneStaticData* pSceneStaticData, char* pData, INT32 nSize)
{
    auto addNewNpc = m_vecAddSpawnNpcs.find(pSceneData->nSceneID);
    auto blockNpc = m_mapBlockNpc.find(pSceneData->nSceneID);

    // Additional monsters exist
    if (addNewNpc != m_vecAddSpawnNpcs.end())
    {
        std::vector<ADD_NPC_SPAWN_POINT> vecData = addNewNpc->second;

        INT32 nRetCode = 0;
        char szScript[256] = { 0 };
        char szBTTreeName[256] = { 0 };
        NPC_SINGLE_SPAWN_HEADER* pHeader = NULL;
        std::pair<CSceneStaticData::MAP_NAME_2_NPC::iterator, bool> InsRet;

        LOG_PROCESS_ERROR(pSceneStaticData);
        LOG_PROCESS_ERROR(pData);

        pHeader = (NPC_SINGLE_SPAWN_HEADER*)pData;
        LOG_PROCESS_ERROR(pHeader->nNpcCount >= 0 && pHeader->nNpcCount < MAX_SINGLE_NPC_COUNT);

        pSceneStaticData->m_nSingleNpcCount = 0;

        // Calculate pSceneStaticData->m_nSingleNpcCount
        for (INT32 nIndex = 0; nIndex < pHeader->nNpcCount; nIndex++)
        {
            NPC_SINGLE_SPAWN& rSingleNpc = pHeader->NpcTable[nIndex];

            if (pSceneStaticData->m_nSingleNpcCount < rSingleNpc.nID)
                pSceneStaticData->m_nSingleNpcCount = rSingleNpc.nID;
        }
        for (auto& SingleNpc : vecData)
        {
            if (SingleNpc.nID > pSceneStaticData->m_nSingleNpcCount)
            {
                pSceneStaticData->m_nSingleNpcCount = SingleNpc.nID;
            }
        }
        pSceneStaticData->m_nSingleNpcCount++;

        pSceneStaticData->m_pSingleNpcTable = new NPC_SPAWN_POINT[pSceneStaticData->m_nSingleNpcCount];
        LOG_PROCESS_ERROR(pSceneStaticData->m_pSingleNpcTable);
        pSceneStaticData->m_dwMemorySize += sizeof(NPC_SPAWN_POINT) * pSceneStaticData->m_nSingleNpcCount;
        memset(pSceneStaticData->m_pSingleNpcTable, 0, sizeof(NPC_SPAWN_POINT) * pSceneStaticData->m_nSingleNpcCount);

        // NPCs from custom configuration
        for (INT32 nIndex = 0; nIndex < pHeader->nNpcCount; nIndex++)
        {
            NPC_SINGLE_SPAWN& rSingleNpc = pHeader->NpcTable[nIndex];
            NPC_SPAWN_POINT& rPoint = pSceneStaticData->m_pSingleNpcTable[rSingleNpc.nID];

            // NPC filtering is unnecessary for custom configuration: unwanted NPCs should not be listed
            //...

            rPoint.qwNameID = rSingleNpc.qwNameID;                   // npcID
            rPoint.nX = rSingleNpc.nX;                               // X coordinate
            rPoint.nY = rSingleNpc.nY;                               // Y coordinate
            rPoint.nZ = rSingleNpc.nZ;                               // Z coordinate
            rPoint.nRange = (INT32)rSingleNpc.wRange * POINT_PER_CM; // Range
            rPoint.nDirection = rSingleNpc.wDirection;               // Direction

            // NPC with invalid direction
            if (rPoint.nDirection < 0 || rPoint.nDirection >= MAX_DIRECTION_COUNT)
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("wrong face direction at point [%d, %d, %d]", nX, nY, nZ, pSceneData->szConfigName);
                m_nErrorCount++;
            }

            // NPC not found
            if (CDataManager<NPC_DATA>::GetInstance().GetData(rSingleNpc.nNpcTemplateID) == NULL)
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("invalid npc template id %d at point [%d, %d, %d] scene %s", rSingleNpc.nNpcTemplateID,
                    nX, nY, nZ, pSceneData->szConfigName);
                m_nErrorCount++;
            }

            // Add monster
            rPoint.RandNpc.Add(rSingleNpc.nNpcTemplateID, 1);

            rPoint.nLevelOffset = rSingleNpc.nLevelOffset;                     // Level offset?
            rPoint.nAIID = rSingleNpc.nAIID;                                   // AIID
            rPoint.nQuality = rSingleNpc.byQuality;                            // Quality type
            rPoint.nMinRespawnTime = CONVERT_TIME(rSingleNpc.nMinRespawnTime); // Minimum respawn time
            rPoint.nMaxRespawnTime = CONVERT_TIME(rSingleNpc.nMaxRespawnTime); // Maximum respawn time
            rPoint.nSpawnTime = CONVERT_TIME(rSingleNpc.nSpawnTime);           // Spawn time
            rPoint.nReviveTime = CONVERT_TIME(rSingleNpc.nReviveTime);         // Revival time
            rPoint.nCurrentHp = rSingleNpc.nCurrentHp;                         // HP
            rPoint.nCurrentMp = rSingleNpc.nCurrentMp;                         // MP
            rPoint.bCanDropItem = rSingleNpc.bCanDropItem;                     // Whether items can drop

            rPoint.nPatrolPathID = rSingleNpc.nPatrolPathID;   // Patrol path ID
            rPoint.nPatrolStyle = rSingleNpc.nPatrolStyle;     // Patrol style
            rPoint.nFollowHostID = rSingleNpc.nFollowHostID;   // Follow ID?
            rPoint.nFollowOffsetX = rSingleNpc.nFollowOffsetX; // Follow X offset
            rPoint.nFollowOffsetY = rSingleNpc.nFollowOffsetY; // Follow Y offset

            rPoint.bAutoSpawn = rSingleNpc.IsAutoSpawn;        // Automatic spawn?
            rPoint.bAutoRespawn = rSingleNpc.IsAutoReSpawn;    // Automatic respawn?
            rPoint.nSmallGroupID = rPoint.nMediumGroupID = rPoint.nLargeGroupID = -1;  // Group IDs
            rPoint.nID = rSingleNpc.nID;                       // Index in array?

            // Load BT - load the scripted behavior tree
            rPoint.pDeadBTRoot = NULL;
            snprintf(szBTTreeName, sizeof(szBTTreeName), "%s_%d", g_BTEventName[evtSingleNpcDie], rPoint.nID);
            if (pSceneStaticData->m_pScript && pSceneStaticData->m_pScript->IsTableExist(szBTTreeName))
            {
                nRetCode = pSceneStaticData->m_pScript->CallFunction(LOAD_BEHAVIOR_TREE, "s:o", szBTTreeName, &rPoint.pDeadBTRoot);
                if (FALSE == nRetCode || NULL == rPoint.pDeadBTRoot)
                {
                    CRI("failed to load script %s of single npc %d scene %d", szBTTreeName, rPoint.nID, pSceneData->nSceneID);
                    m_nErrorCount++;
                }
            }

            // Add to pSceneStaticData->m_mapSingleNpc
            if (rPoint.qwNameID > 0)
            {
                InsRet = pSceneStaticData->m_mapSingleNpc.insert(std::make_pair(rPoint.qwNameID, &rPoint));
                if (!InsRet.second)
                {
                    char szName[256] = { 0 };
                    INT32 nX = rPoint.nX / POINT_PER_CM;
                    INT32 nY = rPoint.nY / POINT_PER_CM;
                    INT32 nZ = rPoint.nZ / POINT_PER_CM;

                    memcpy(szName, &rPoint.qwNameID, sizeof(rPoint.qwNameID));
                    ConvertCoordN2O(nX, nY, nZ);

                    WRN("duplicated npc name id %s at [%d, %d, %d] scene %s %d", szName,
                        nX, nY, nZ, pSceneData->szConfigName, pSceneData->nSceneID);
                    m_nErrorCount++;
                }
            }

            // s_mapNpcCount[rSingleNpc.nNpcTemplateID]++;

            // Check grid coordinates
            if (!pSceneStaticData->CheckGrid(rPoint.nX, rPoint.nY, rPoint.nZ + GRID_WIDTH))
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("detected wrong spawn pos [%d, %d, %d] while loading single npc %d in scene %s %d",
                    nX, nY, nZ, rPoint.nID, pSceneData->szConfigName, pSceneData->nSceneID);
                m_nErrorCount++;
            }

#ifdef _ROBOT
            m_NpcPosMap[rSingleNpc.nNpcTemplateID] = { pSceneData->nSceneID, rPoint.nX, rPoint.nY, rPoint.nZ };
#endif
        }

        // Newly added monsters
        for (ADD_NPC_SPAWN_POINT& rSingleNpc : vecData)
        {
            NPC_SPAWN_POINT& rPoint = pSceneStaticData->m_pSingleNpcTable[rSingleNpc.nID];

            rPoint.qwNameID = 0;                   // npcID
            rPoint.nX = rSingleNpc.nX;                               // X coordinate
            rPoint.nY = rSingleNpc.nY;                               // Y coordinate
            rPoint.nZ = rSingleNpc.nZ;                               // Z coordinate
            rPoint.nRange = rSingleNpc.nRange; // Range
            rPoint.nDirection = rSingleNpc.nDirection;               // Direction

            // NPC with invalid direction
            if (rPoint.nDirection < 0 || rPoint.nDirection >= MAX_DIRECTION_COUNT)
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("wrong face direction at point [%d, %d, %d]", nX, nY, nZ, pSceneData->szConfigName);
                m_nErrorCount++;
            }

            // NPC not found
            if (CDataManager<NPC_DATA>::GetInstance().GetData(rSingleNpc.nTemplateID) == NULL)
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("invalid npc template id %d at point [%d, %d, %d] scene %s", rSingleNpc.nTemplateID,
                    nX, nY, nZ, pSceneData->szConfigName);
                m_nErrorCount++;
            }

            rPoint.RandNpc.Add(rSingleNpc.nTemplateID, 1);

            rPoint.nLevelOffset = rSingleNpc.nLevelOffset;                     // Level offset?
            rPoint.nAIID = 0;                                   // AIID
            rPoint.nQuality = rSingleNpc.nQuality;                            // Quality type
            rPoint.nMinRespawnTime = CONVERT_TIME(rSingleNpc.nMinRespawnTime); // Minimum respawn time
            rPoint.nMaxRespawnTime = CONVERT_TIME(rSingleNpc.nMaxRespawnTime); // Maximum respawn time
            rPoint.nSpawnTime = 0;           // Spawn time
            rPoint.nReviveTime = 0;           // Revival time
            rPoint.nCurrentHp = rSingleNpc.nCurrentHp;                         // HP
            rPoint.nCurrentMp = rSingleNpc.nCurrentMp;                         // MP
            rPoint.bCanDropItem = rSingleNpc.bCanDropItem;                     // Whether items can drop

            rPoint.nPatrolPathID = 0;  // Patrol path ID
            rPoint.nPatrolStyle = 0;  // Patrol style
            rPoint.nFollowHostID = 0;  // Follow ID?
            rPoint.nFollowOffsetX = 0;  // Follow X offset
            rPoint.nFollowOffsetY = 0;  // Follow Y offset

            rPoint.bAutoSpawn = rSingleNpc.bAutoSpawn;        // Automatic spawn?
            rPoint.bAutoRespawn = rSingleNpc.bAutoRespawn;    // Automatic respawn?
            rPoint.nSmallGroupID = rPoint.nMediumGroupID = rPoint.nLargeGroupID = -1;  // Group IDs
            rPoint.nID = rSingleNpc.nID;                       // Index in array?

            // Load BT - load the scripted behavior tree
            rPoint.pDeadBTRoot = NULL;
            snprintf(szBTTreeName, sizeof(szBTTreeName), "%s_%d", g_BTEventName[evtSingleNpcDie], rPoint.nID);
            if (pSceneStaticData->m_pScript && pSceneStaticData->m_pScript->IsTableExist(szBTTreeName))
            {
                nRetCode = pSceneStaticData->m_pScript->CallFunction(LOAD_BEHAVIOR_TREE, "s:o", szBTTreeName, &rPoint.pDeadBTRoot);
                if (FALSE == nRetCode || NULL == rPoint.pDeadBTRoot)
                {
                    CRI("failed to load script %s of single npc %d scene %d", szBTTreeName, rPoint.nID, pSceneData->nSceneID);
                    m_nErrorCount++;
                }
            }

            // Add to pSceneStaticData->m_mapSingleNpc
            if (rPoint.qwNameID > 0)
            {
                InsRet = pSceneStaticData->m_mapSingleNpc.insert(std::make_pair(rPoint.qwNameID, &rPoint));
                if (!InsRet.second)
                {
                    char szName[256] = { 0 };
                    INT32 nX = rPoint.nX / POINT_PER_CM;
                    INT32 nY = rPoint.nY / POINT_PER_CM;
                    INT32 nZ = rPoint.nZ / POINT_PER_CM;

                    memcpy(szName, &rPoint.qwNameID, sizeof(rPoint.qwNameID));
                    ConvertCoordN2O(nX, nY, nZ);

                    WRN("duplicated npc name id %s at [%d, %d, %d] scene %s %d", szName,
                        nX, nY, nZ, pSceneData->szConfigName, pSceneData->nSceneID);
                    m_nErrorCount++;
                }
            }

            // s_mapNpcCount[rSingleNpc.nNpcTemplateID]++;

            // Check grid coordinates
            if (!pSceneStaticData->CheckGrid(rPoint.nX, rPoint.nY, rPoint.nZ + GRID_WIDTH))
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("detected wrong spawn pos [%d, %d, %d] while loading single npc %d in scene %s %d",
                    nX, nY, nZ, rPoint.nID, pSceneData->szConfigName, pSceneData->nSceneID);
                m_nErrorCount++;
            }

#ifdef _ROBOT
            m_NpcPosMap[rSingleNpc.nNpcTemplateID] = { pSceneData->nSceneID, rPoint.nX, rPoint.nY, rPoint.nZ };
#endif
        }

    }
    // Original code - no additional monsters
    else
    {
        INT32 nRetCode = 0;
        char szScript[256] = { 0 };
        char szBTTreeName[256] = { 0 };
        NPC_SINGLE_SPAWN_HEADER* pHeader = NULL;
        std::pair<CSceneStaticData::MAP_NAME_2_NPC::iterator, bool> InsRet;

        LOG_PROCESS_ERROR(pSceneStaticData);
        LOG_PROCESS_ERROR(pData);

        pHeader = (NPC_SINGLE_SPAWN_HEADER*)pData;

        pSceneStaticData->m_nSingleNpcCount = 0;

        LOG_PROCESS_ERROR(pHeader->nNpcCount >= 0 && pHeader->nNpcCount < MAX_SINGLE_NPC_COUNT);
        for (INT32 nIndex = 0; nIndex < pHeader->nNpcCount; nIndex++)
        {
            NPC_SINGLE_SPAWN& rSingleNpc = pHeader->NpcTable[nIndex];

            if (pSceneStaticData->m_nSingleNpcCount < rSingleNpc.nID)
                pSceneStaticData->m_nSingleNpcCount = rSingleNpc.nID;
        }
        pSceneStaticData->m_nSingleNpcCount++;

        pSceneStaticData->m_pSingleNpcTable = new NPC_SPAWN_POINT[pSceneStaticData->m_nSingleNpcCount];
        LOG_PROCESS_ERROR(pSceneStaticData->m_pSingleNpcTable);
        pSceneStaticData->m_dwMemorySize += sizeof(NPC_SPAWN_POINT) * pSceneStaticData->m_nSingleNpcCount;
        memset(pSceneStaticData->m_pSingleNpcTable, 0, sizeof(NPC_SPAWN_POINT) * pSceneStaticData->m_nSingleNpcCount);

        for (INT32 nIndex = 0; nIndex < pHeader->nNpcCount; nIndex++)
        {
            NPC_SINGLE_SPAWN& rSingleNpc = pHeader->NpcTable[nIndex];
            NPC_SPAWN_POINT& rPoint = pSceneStaticData->m_pSingleNpcTable[rSingleNpc.nID];

            // Some NPCs are blocked
            if (blockNpc != m_mapBlockNpc.end())
            {
                auto find_block_npc = blockNpc->second.find(rSingleNpc.nNpcTemplateID);
                if (find_block_npc != blockNpc->second.end())
                    continue;
            }

            rPoint.qwNameID = rSingleNpc.qwNameID;                   // npcID
            rPoint.nX = rSingleNpc.nX;                               // X coordinate
            rPoint.nY = rSingleNpc.nY;                               // Y coordinate
            rPoint.nZ = rSingleNpc.nZ;                               // Z coordinate
            rPoint.nRange = (INT32)rSingleNpc.wRange * POINT_PER_CM; // Range
            rPoint.nDirection = rSingleNpc.wDirection;               // Direction

            // NPC with invalid direction
            if (rPoint.nDirection < 0 || rPoint.nDirection >= MAX_DIRECTION_COUNT)
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("wrong face direction at point [%d, %d, %d]", nX, nY, nZ, pSceneData->szConfigName);
                m_nErrorCount++;
            }

            // NPC not found
            if (CDataManager<NPC_DATA>::GetInstance().GetData(rSingleNpc.nNpcTemplateID) == NULL)
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("invalid npc template id %d at point [%d, %d, %d] scene %s", rSingleNpc.nNpcTemplateID,
                    nX, nY, nZ, pSceneData->szConfigName);
                m_nErrorCount++;
            }

            rPoint.RandNpc.Add(rSingleNpc.nNpcTemplateID, 1);

            rPoint.nLevelOffset = rSingleNpc.nLevelOffset;                     // Level offset?
            rPoint.nAIID = rSingleNpc.nAIID;                                   // AIID
            rPoint.nQuality = rSingleNpc.byQuality;                            // Quality type
            rPoint.nMinRespawnTime = CONVERT_TIME(rSingleNpc.nMinRespawnTime); // Minimum respawn time
            rPoint.nMaxRespawnTime = CONVERT_TIME(rSingleNpc.nMaxRespawnTime); // Maximum respawn time
            rPoint.nSpawnTime = CONVERT_TIME(rSingleNpc.nSpawnTime);           // Spawn time
            rPoint.nReviveTime = CONVERT_TIME(rSingleNpc.nReviveTime);         // Revival time
            rPoint.nCurrentHp = rSingleNpc.nCurrentHp;                         // HP
            rPoint.nCurrentMp = rSingleNpc.nCurrentMp;                         // MP
            rPoint.bCanDropItem = rSingleNpc.bCanDropItem;                     // Whether items can drop

            rPoint.nPatrolPathID = rSingleNpc.nPatrolPathID;   // Patrol path ID
            rPoint.nPatrolStyle = rSingleNpc.nPatrolStyle;     // Patrol style
            rPoint.nFollowHostID = rSingleNpc.nFollowHostID;   // Follow ID?
            rPoint.nFollowOffsetX = rSingleNpc.nFollowOffsetX; // Follow X offset
            rPoint.nFollowOffsetY = rSingleNpc.nFollowOffsetY; // Follow Y offset

            rPoint.bAutoSpawn = rSingleNpc.IsAutoSpawn;        // Automatic spawn?
            rPoint.bAutoRespawn = rSingleNpc.IsAutoReSpawn;    // Automatic respawn?
            rPoint.nSmallGroupID = rPoint.nMediumGroupID = rPoint.nLargeGroupID = -1;  // Group IDs
            rPoint.nID = rSingleNpc.nID;                       // Index in array?

            // Load BT - load the scripted behavior tree
            rPoint.pDeadBTRoot = NULL;
            snprintf(szBTTreeName, sizeof(szBTTreeName), "%s_%d", g_BTEventName[evtSingleNpcDie], rPoint.nID);
            if (pSceneStaticData->m_pScript && pSceneStaticData->m_pScript->IsTableExist(szBTTreeName))
            {
                nRetCode = pSceneStaticData->m_pScript->CallFunction(LOAD_BEHAVIOR_TREE, "s:o", szBTTreeName, &rPoint.pDeadBTRoot);
                if (FALSE == nRetCode || NULL == rPoint.pDeadBTRoot)
                {
                    CRI("failed to load script %s of single npc %d scene %d", szBTTreeName, rPoint.nID, pSceneData->nSceneID);
                    m_nErrorCount++;
                }
            }

            // Add to pSceneStaticData->m_mapSingleNpc
            if (rPoint.qwNameID > 0)
            {
                InsRet = pSceneStaticData->m_mapSingleNpc.insert(std::make_pair(rPoint.qwNameID, &rPoint));
                if (!InsRet.second)
                {
                    char szName[256] = { 0 };
                    INT32 nX = rPoint.nX / POINT_PER_CM;
                    INT32 nY = rPoint.nY / POINT_PER_CM;
                    INT32 nZ = rPoint.nZ / POINT_PER_CM;

                    memcpy(szName, &rPoint.qwNameID, sizeof(rPoint.qwNameID));
                    ConvertCoordN2O(nX, nY, nZ);

                    WRN("duplicated npc name id %s at [%d, %d, %d] scene %s %d", szName,
                        nX, nY, nZ, pSceneData->szConfigName, pSceneData->nSceneID);
                    m_nErrorCount++;
                }
            }

            // s_mapNpcCount[rSingleNpc.nNpcTemplateID]++;

            // Check grid coordinates
            if (!pSceneStaticData->CheckGrid(rPoint.nX, rPoint.nY, rPoint.nZ + GRID_WIDTH))
            {
                INT32 nX = rPoint.nX / POINT_PER_CM;
                INT32 nY = rPoint.nY / POINT_PER_CM;
                INT32 nZ = rPoint.nZ / POINT_PER_CM;
                ConvertCoordN2O(nX, nY, nZ);
                WRN("detected wrong spawn pos [%d, %d, %d] while loading single npc %d in scene %s %d",
                    nX, nY, nZ, rPoint.nID, pSceneData->szConfigName, pSceneData->nSceneID);
                m_nErrorCount++;
            }

            // Locally added test code
            //auto it = setNpcID.find(rSingleNpc.nNpcTemplateID);
            //if (it != setNpcID.end())
            //{
            //    WRN("========================================");
            //    WRN("");
            //
            //    INT32 tmpnX = rPoint.nX / POINT_PER_CM;
            //    INT32 tmpnY = rPoint.nY / POINT_PER_CM;
            //    INT32 tmpnZ = rPoint.nZ / POINT_PER_CM;
            //    ConvertCoordN2O(tmpnX, tmpnY, tmpnZ);
            //    WRN("FindElite - MapID %d, nNpcTemplateID %d, xyz [%d, %d, %d], [%d, %d, %d], nRange %d, nDirection %d, nMinRespawnTime %d, nMaxRespawnTime %d, nSpawnTime %d, nReviveTime %d, Converted [nMinRespawnTime %d, nMaxRespawnTime %d, nSpawnTime %d, nReviveTime %d], bAutoSpawn %d, bAutoRespawn %d",
            //        pSceneData->nSceneID, rSingleNpc.nNpcTemplateID,
            //        rPoint.nX, rPoint.nY, rPoint.nZ, 
            //        rPoint.nRange, rPoint.nDirection, tmpnX, tmpnY, tmpnZ,
            //        rSingleNpc.nMinRespawnTime, rSingleNpc.nMaxRespawnTime, rSingleNpc.nSpawnTime, rSingleNpc.nReviveTime,
            //        rPoint.nMinRespawnTime, rPoint.nMaxRespawnTime, rPoint.nSpawnTime, rPoint.nReviveTime,
            //        rPoint.bAutoSpawn, rPoint.bAutoRespawn);
            //
            //    WRN("");
            //    WRN("========================================");
            //    WRN("");
            //}

#ifdef _ROBOT
            m_NpcPosMap[rSingleNpc.nNpcTemplateID] = { pSceneData->nSceneID, rPoint.nX, rPoint.nY, rPoint.nZ };
#endif
        }
    }

    // Original code
    //    INT32 nRetCode = 0;
    //    char szScript[256] = { 0 };
    //    char szBTTreeName[256] = { 0 };
    //    NPC_SINGLE_SPAWN_HEADER* pHeader = NULL;
    //    std::pair<CSceneStaticData::MAP_NAME_2_NPC::iterator, bool> InsRet;
    //
    //    LOG_PROCESS_ERROR(pSceneStaticData);
    //    LOG_PROCESS_ERROR(pData);
    //
    //    pHeader = (NPC_SINGLE_SPAWN_HEADER*)pData;
    //
    //    pSceneStaticData->m_nSingleNpcCount = 0;
    //
    //    LOG_PROCESS_ERROR(pHeader->nNpcCount >= 0 && pHeader->nNpcCount < MAX_SINGLE_NPC_COUNT);
    //    for (INT32 nIndex = 0; nIndex < pHeader->nNpcCount; nIndex++)
    //    {
    //        NPC_SINGLE_SPAWN& rSingleNpc = pHeader->NpcTable[nIndex];
    //
    //        if (pSceneStaticData->m_nSingleNpcCount < rSingleNpc.nID)
    //            pSceneStaticData->m_nSingleNpcCount = rSingleNpc.nID;
    //    }
    //    pSceneStaticData->m_nSingleNpcCount++;
    //
    //    pSceneStaticData->m_pSingleNpcTable = new NPC_SPAWN_POINT[pSceneStaticData->m_nSingleNpcCount];
    //    LOG_PROCESS_ERROR(pSceneStaticData->m_pSingleNpcTable);
    //    pSceneStaticData->m_dwMemorySize += sizeof(NPC_SPAWN_POINT) * pSceneStaticData->m_nSingleNpcCount;
    //    memset(pSceneStaticData->m_pSingleNpcTable, 0, sizeof(NPC_SPAWN_POINT) * pSceneStaticData->m_nSingleNpcCount);
    //
    //    for (INT32 nIndex = 0; nIndex < pHeader->nNpcCount; nIndex++)
    //    {
    //        NPC_SINGLE_SPAWN& rSingleNpc = pHeader->NpcTable[nIndex];
    //        NPC_SPAWN_POINT& rPoint = pSceneStaticData->m_pSingleNpcTable[rSingleNpc.nID];
    //
    //        rPoint.qwNameID = rSingleNpc.qwNameID;                   // npcID
    //        rPoint.nX = rSingleNpc.nX;                               // X coordinate
    //        rPoint.nY = rSingleNpc.nY;                               // Y coordinate
    //        rPoint.nZ = rSingleNpc.nZ;                               // Z coordinate
    //        rPoint.nRange = (INT32)rSingleNpc.wRange * POINT_PER_CM; // Range
    //        rPoint.nDirection = rSingleNpc.wDirection;               // Direction
    //
    //        // NPC with invalid direction
    //        if (rPoint.nDirection < 0 || rPoint.nDirection >= MAX_DIRECTION_COUNT)
    //        {
    //            INT32 nX = rPoint.nX / POINT_PER_CM;
    //            INT32 nY = rPoint.nY / POINT_PER_CM;
    //            INT32 nZ = rPoint.nZ / POINT_PER_CM;
    //            ConvertCoordN2O(nX, nY, nZ);
    //            WRN("wrong face direction at point [%d, %d, %d]", nX, nY, nZ, pSceneData->szConfigName);
    //            m_nErrorCount++;
    //        }
    //
    //        // NPC not found
    //        if (CDataManager<NPC_DATA>::GetInstance().GetData(rSingleNpc.nNpcTemplateID) == NULL)
    //        {
    //            INT32 nX = rPoint.nX / POINT_PER_CM;
    //            INT32 nY = rPoint.nY / POINT_PER_CM;
    //            INT32 nZ = rPoint.nZ / POINT_PER_CM;
    //            ConvertCoordN2O(nX, nY, nZ);
    //            WRN("invalid npc template id %d at point [%d, %d, %d] scene %s", rSingleNpc.nNpcTemplateID,
    //                nX, nY, nZ, pSceneData->szConfigName);
    //            m_nErrorCount++;
    //        }
    //
    //        rPoint.RandNpc.Add(rSingleNpc.nNpcTemplateID, 1);
    //
    //        rPoint.nLevelOffset = rSingleNpc.nLevelOffset;                     // Level offset?
    //        rPoint.nAIID = rSingleNpc.nAIID;                                   // AIID
    //        rPoint.nQuality = rSingleNpc.byQuality;                            // Quality type
    //        rPoint.nMinRespawnTime = CONVERT_TIME(rSingleNpc.nMinRespawnTime); // Minimum respawn time
    //        rPoint.nMaxRespawnTime = CONVERT_TIME(rSingleNpc.nMaxRespawnTime); // Maximum respawn time
    //        rPoint.nSpawnTime = CONVERT_TIME(rSingleNpc.nSpawnTime);           // Spawn time
    //        rPoint.nReviveTime = CONVERT_TIME(rSingleNpc.nReviveTime);         // Revival time
    //        rPoint.nCurrentHp = rSingleNpc.nCurrentHp;                         // HP
    //        rPoint.nCurrentMp = rSingleNpc.nCurrentMp;                         // MP
    //        rPoint.bCanDropItem = rSingleNpc.bCanDropItem;                     // Whether items can drop
    //
    //        rPoint.nPatrolPathID = rSingleNpc.nPatrolPathID;   // Patrol path ID
    //        rPoint.nPatrolStyle = rSingleNpc.nPatrolStyle;     // Patrol style
    //        rPoint.nFollowHostID = rSingleNpc.nFollowHostID;   // Follow ID?
    //        rPoint.nFollowOffsetX = rSingleNpc.nFollowOffsetX; // Follow X offset
    //        rPoint.nFollowOffsetY = rSingleNpc.nFollowOffsetY; // Follow Y offset
    //
    //        rPoint.bAutoSpawn = rSingleNpc.IsAutoSpawn;        // Automatic spawn?
    //        rPoint.bAutoRespawn = rSingleNpc.IsAutoReSpawn;    // Automatic respawn?
    //        rPoint.nSmallGroupID = rPoint.nMediumGroupID = rPoint.nLargeGroupID = -1;  // Group IDs
    //        rPoint.nID = rSingleNpc.nID;                       // Index in array?
    //
    //        // Load BT - load the scripted behavior tree
    //        rPoint.pDeadBTRoot = NULL;
    //        snprintf(szBTTreeName, sizeof(szBTTreeName), "%s_%d", g_BTEventName[evtSingleNpcDie], rPoint.nID);
    //        if (pSceneStaticData->m_pScript && pSceneStaticData->m_pScript->IsTableExist(szBTTreeName))
    //        {
    //            nRetCode = pSceneStaticData->m_pScript->CallFunction(LOAD_BEHAVIOR_TREE, "s:o", szBTTreeName, &rPoint.pDeadBTRoot);
    //            if (FALSE == nRetCode || NULL == rPoint.pDeadBTRoot)
    //            {
    //                CRI("failed to load script %s of single npc %d scene %d", szBTTreeName, rPoint.nID, pSceneData->nSceneID);
    //                m_nErrorCount++;
    //            }
    //        }
    //
    //        // Add to pSceneStaticData->m_mapSingleNpc
    //        if (rPoint.qwNameID > 0)
    //        {
    //            InsRet = pSceneStaticData->m_mapSingleNpc.insert(std::make_pair(rPoint.qwNameID, &rPoint));
    //            if (!InsRet.second)
    //            {
    //                char szName[256] = { 0 };
    //                INT32 nX = rPoint.nX / POINT_PER_CM;
    //                INT32 nY = rPoint.nY / POINT_PER_CM;
    //                INT32 nZ = rPoint.nZ / POINT_PER_CM;
    //
    //                memcpy(szName, &rPoint.qwNameID, sizeof(rPoint.qwNameID));
    //                ConvertCoordN2O(nX, nY, nZ);
    //
    //                WRN("duplicated npc name id %s at [%d, %d, %d] scene %s %d", szName,
    //                    nX, nY, nZ, pSceneData->szConfigName, pSceneData->nSceneID);
    //                m_nErrorCount++;
    //            }
    //        }
    //
    //        // s_mapNpcCount[rSingleNpc.nNpcTemplateID]++;
    //
    //        // Check grid coordinates
    //        if (!pSceneStaticData->CheckGrid(rPoint.nX, rPoint.nY, rPoint.nZ + GRID_WIDTH))
    //        {
    //            INT32 nX = rPoint.nX / POINT_PER_CM;
    //            INT32 nY = rPoint.nY / POINT_PER_CM;
    //            INT32 nZ = rPoint.nZ / POINT_PER_CM;
    //            ConvertCoordN2O(nX, nY, nZ);
    //            WRN("detected wrong spawn pos [%d, %d, %d] while loading single npc %d in scene %s %d",
    //                nX, nY, nZ, rPoint.nID, pSceneData->szConfigName, pSceneData->nSceneID);
    //            m_nErrorCount++;
    //        }
    //
    //#ifdef _ROBOT
    //        m_NpcPosMap[rSingleNpc.nNpcTemplateID] = { pSceneData->nSceneID, rPoint.nX, rPoint.nY, rPoint.nZ };
    //#endif
    //    }

Exit1:
    return TRUE;
Exit0:
    SAFE_DELETE_ARRAY(pSceneStaticData->m_pSingleNpcTable);

    return FALSE;
}

BOOL CSceneFileMgr::_LoadNpcSpawnData(const SCENE_DATA* pSceneData, CSceneStaticData* pSceneStaticData, char* pData, INT32 nSize)
{
    auto blockNpc = m_mapBlockNpc.find(pSceneData->nSceneID);

    INT32 nRetCode = 0;
    char szScript[256] = { 0 };
    char szBTTreeName[256] = { 0 };
    NPC_SPAWN_DATA_HEADER* pHeader = NULL;
    char* pOriginalData = pData;

    LOG_PROCESS_ERROR(pSceneStaticData);
    LOG_PROCESS_ERROR(pData);

    pHeader = (NPC_SPAWN_DATA_HEADER*)pData;
    pData += sizeof(NPC_SPAWN_DATA_HEADER);
    LOG_PROCESS_ERROR(pHeader->nLargeGroupCount >= 0 && pHeader->nLargeGroupCount < MAX_NPC_LARGE_GROUP_COUNT);

    pSceneStaticData->m_nNpcLargeGroupCount = 0;
    pSceneStaticData->m_nNpcMediumGroupCount = 0;

    // Calculate m_nNpcLargeGroupCount and m_nNpcMediumGroupCount
    for (INT32 nLargeGroupIndex = 0; nLargeGroupIndex < pHeader->nLargeGroupCount; nLargeGroupIndex++)
    {
        NPC_SPAWN_LARGE_GROUP* pLargeGroupData = (NPC_SPAWN_LARGE_GROUP*)pData;
        pData += sizeof(NPC_SPAWN_LARGE_GROUP);

        LOG_PROCESS_ERROR(pLargeGroupData->nID >= 0 && pLargeGroupData->nID < MAX_NPC_LARGE_GROUP_COUNT);

        if (pSceneStaticData->m_nNpcLargeGroupCount < pLargeGroupData->nID)
            pSceneStaticData->m_nNpcLargeGroupCount = pLargeGroupData->nID;

        LOG_PROCESS_ERROR(pLargeGroupData->nMediumGroupCount >= 0 && pLargeGroupData->nMediumGroupCount < MAX_NPC_MEDIUM_GROUP_PER_LARGE_GROUP);

        for (INT32 nMediumGroupIndex = 0; nMediumGroupIndex < pLargeGroupData->nMediumGroupCount; nMediumGroupIndex++)
        {
            NPC_SPAWN_MEDIUM_GROUP* pMediumGroupData = (NPC_SPAWN_MEDIUM_GROUP*)pData;
            pData += sizeof(NPC_SPAWN_MEDIUM_GROUP);

            LOG_PROCESS_ERROR(pMediumGroupData->nID >= 0 && pMediumGroupData->nID < MAX_NPC_MEDIUM_GROUP_COUNT);

            // Key assignment
            if (pSceneStaticData->m_nNpcMediumGroupCount < pMediumGroupData->nID)
                pSceneStaticData->m_nNpcMediumGroupCount = pMediumGroupData->nID;

            LOG_PROCESS_ERROR(pMediumGroupData->nSmallGroupCount >= 0 && pMediumGroupData->nSmallGroupCount < MAX_NPC_SMALL_GROUP_PER_MEDIUM_GROUP);

            for (INT32 nSmallGroupID = 0; nSmallGroupID < pMediumGroupData->nSmallGroupCount; nSmallGroupID++)
            {
                NPC_SPAWN_SMALL_GROUP* pSmallGroupData = (NPC_SPAWN_SMALL_GROUP*)pData;
                pData += sizeof(NPC_SPAWN_SMALL_GROUP);

                LOG_PROCESS_ERROR(pSmallGroupData->nPointCount >= 0 && pSmallGroupData->nPointCount < MAX_NPC_SPAWN_POINT_PER_SMALL_GROUP);

                for (INT32 nPointIndex = 0; nPointIndex < pSmallGroupData->nPointCount; nPointIndex++)
                {
                    NPC_SPAWN_POINT_DATA* pPointData = (NPC_SPAWN_POINT_DATA*)pData;
                    pData += sizeof(NPC_SPAWN_POINT_DATA);
                }
            }
        }
    }

    pSceneStaticData->m_nNpcLargeGroupCount++;
    pSceneStaticData->m_nNpcMediumGroupCount++;
    LOG_PROCESS_ERROR(pSceneStaticData->m_nNpcLargeGroupCount < MAX_NPC_LARGE_GROUP_COUNT);
    LOG_PROCESS_ERROR(pSceneStaticData->m_nNpcMediumGroupCount < MAX_NPC_MEDIUM_GROUP_COUNT);

    // Allocate large groups
    pSceneStaticData->m_pNpcLargeGroupTable = new NPC_LARGE_SPAWN_GROUP[pSceneStaticData->m_nNpcLargeGroupCount];
    LOG_PROCESS_ERROR(pSceneStaticData->m_pNpcLargeGroupTable);
    // Zero large groups
    pSceneStaticData->m_dwMemorySize += sizeof(NPC_LARGE_SPAWN_GROUP) * pSceneStaticData->m_nNpcLargeGroupCount;
    memset(pSceneStaticData->m_pNpcLargeGroupTable, 0, sizeof(NPC_LARGE_SPAWN_GROUP) * pSceneStaticData->m_nNpcLargeGroupCount);

    // Allocate medium groups
    pSceneStaticData->m_pNpcMediumGroupTable = new NPC_MEDIUM_SPAWN_GROUP[pSceneStaticData->m_nNpcMediumGroupCount];
    LOG_PROCESS_ERROR(pSceneStaticData->m_pNpcMediumGroupTable);
    // Zero medium groups
    pSceneStaticData->m_dwMemorySize += sizeof(NPC_MEDIUM_SPAWN_GROUP) * pSceneStaticData->m_nNpcMediumGroupCount;
    memset(pSceneStaticData->m_pNpcMediumGroupTable, 0, sizeof(NPC_MEDIUM_SPAWN_GROUP) * pSceneStaticData->m_nNpcMediumGroupCount);

    // Loop from the start again?
    pData = pOriginalData + sizeof(NPC_SPAWN_DATA_HEADER);
    for (INT32 nLargeGroupIndex = 0; nLargeGroupIndex < pHeader->nLargeGroupCount; nLargeGroupIndex++)
    {
        // Large group
        NPC_SPAWN_LARGE_GROUP* pLargeGroupData = (NPC_SPAWN_LARGE_GROUP*)pData;
        pData += sizeof(NPC_SPAWN_LARGE_GROUP);

        LOG_PROCESS_ERROR(pLargeGroupData->nID < pSceneStaticData->m_nNpcLargeGroupCount);
        NPC_LARGE_SPAWN_GROUP* pLargeGroup = pSceneStaticData->m_pNpcLargeGroupTable + pLargeGroupData->nID;
        LOG_PROCESS_ERROR(pLargeGroup);

        pLargeGroup->nEliteMin = pLargeGroupData->nEliteMin;
        pLargeGroup->nEliteMax = pLargeGroupData->nEliteMax;
        pLargeGroup->nEliteInterval = CONVERT_TIME(pLargeGroupData->nEliteInterval);
        pLargeGroup->nMaxDieCount = pLargeGroupData->nMaxDieCount;
        pLargeGroup->nID = pLargeGroupData->nID;

        // load BT
        pLargeGroup->pDeadBTRoot = NULL;
        snprintf(szBTTreeName, sizeof(szBTTreeName), "%s_%d", g_BTEventName[evtLargeGroupDie], pLargeGroup->nID);
        if (pSceneStaticData->m_pScript && pSceneStaticData->m_pScript->IsTableExist(szBTTreeName))
        {
            nRetCode = pSceneStaticData->m_pScript->CallFunction(LOAD_BEHAVIOR_TREE, "s:o", szBTTreeName, &pLargeGroup->pDeadBTRoot);
            if (FALSE == nRetCode || NULL == pLargeGroup->pDeadBTRoot)
            {
                WRN("failed to load behavior tree %s in scene %d large group %d", szBTTreeName, pSceneData->nSceneID, pLargeGroup->nID);
                m_nErrorCount++;
            }
        }

        pLargeGroup->nMediumGroupCount = 0;

        // Medium group
        LOG_PROCESS_ERROR(pLargeGroupData->nMediumGroupCount < MAX_NPC_MEDIUM_GROUP_PER_LARGE_GROUP);
        for (INT32 nMediumGroupIndex = 0; nMediumGroupIndex < pLargeGroupData->nMediumGroupCount; nMediumGroupIndex++)
        {
            NPC_SPAWN_MEDIUM_GROUP* pMediumGroupData = (NPC_SPAWN_MEDIUM_GROUP*)pData;
            pData += sizeof(NPC_SPAWN_MEDIUM_GROUP);

            LOG_PROCESS_ERROR(pMediumGroupData->nID < pSceneStaticData->m_nNpcMediumGroupCount);

            NPC_MEDIUM_SPAWN_GROUP* pMediumGroup = pSceneStaticData->m_pNpcMediumGroupTable + pMediumGroupData->nID;
            pLargeGroup->MediumGroupList[nMediumGroupIndex] = pMediumGroupData->nID;
            if (pMediumGroup->nID != 0)
            {
                WRN("same medium group %d refered in several large groups", pMediumGroupData->nID);
                m_nErrorCount++;
            }

            pMediumGroup->bAutoSpawn = pMediumGroupData->bAutoSpawn;
            pMediumGroup->szDeadScript[0] = 0;
            pMediumGroup->nID = pMediumGroupData->nID;

            // load BT
            pMediumGroup->pDeadBTRoot = NULL;
            snprintf(szBTTreeName, sizeof(szBTTreeName), "%s_%d", g_BTEventName[evtMediumGroupDie], pMediumGroup->nID);
            if (pSceneStaticData->m_pScript && pSceneStaticData->m_pScript->IsTableExist(szBTTreeName))
            {
                nRetCode = pSceneStaticData->m_pScript->CallFunction(LOAD_BEHAVIOR_TREE, "s:o", szBTTreeName, &pMediumGroup->pDeadBTRoot);
                if (FALSE == nRetCode || NULL == pMediumGroup->pDeadBTRoot)
                {
                    CRI("failed to load behavior tree %s in scene %d large group %d medium group %d", szBTTreeName,
                        pSceneData->nSceneID, pLargeGroup->nID, pMediumGroup->nID);
                    m_nErrorCount++;
                }
            }

            pMediumGroup->nSmallGroupCount = 0;

            // Small group
            LOG_PROCESS_ERROR(pMediumGroupData->nSmallGroupCount < MAX_NPC_SMALL_GROUP_PER_MEDIUM_GROUP);
            for (INT32 nSmallGroupID = 0; nSmallGroupID < pMediumGroupData->nSmallGroupCount; nSmallGroupID++)
            {
                NPC_SPAWN_SMALL_GROUP* pSmallGroupData = (NPC_SPAWN_SMALL_GROUP*)pData;
                pData += sizeof(NPC_SPAWN_SMALL_GROUP);

                NPC_SMALL_SPAWN_GROUP* pSmallGroup = (NPC_SMALL_SPAWN_GROUP*)malloc(
                    sizeof(NPC_SMALL_SPAWN_GROUP) + sizeof(NPC_SPAWN_POINT) * pSmallGroupData->nPointCount);
                LOG_PROCESS_ERROR(pSmallGroup);

                pSceneStaticData->m_dwMemorySize += sizeof(NPC_SMALL_SPAWN_GROUP) + sizeof(NPC_SPAWN_POINT) * pSmallGroupData->nPointCount;

                memset(pSmallGroup, 0, sizeof(NPC_SMALL_SPAWN_GROUP) + sizeof(NPC_SPAWN_POINT) * pSmallGroupData->nPointCount);
                pMediumGroup->SmallGroupList[nSmallGroupID] = pSmallGroup;

                nRetCode = pMediumGroup->RandSmallGroup.Add(nSmallGroupID, pSmallGroupData->nWeight);
                LOG_PROCESS_ERROR(nRetCode);

                pSmallGroup->nPointCount = 0;
                pSmallGroup->bDynamicRespawnTime = pSmallGroupData->bDynamicRespawnTime;

                // Iterate small groups
                for (INT32 nPointIndex = 0; nPointIndex < pSmallGroupData->nPointCount; nPointIndex++)
                {
                    NPC_SPAWN_POINT_DATA* pPointData = (NPC_SPAWN_POINT_DATA*)pData;
                    pData += sizeof(NPC_SPAWN_POINT_DATA);

                    NPC_SPAWN_POINT* pSpawnPoint = pSmallGroup->PointList + nPointIndex;
                    pSpawnPoint->nX = pPointData->nX;
                    pSpawnPoint->nY = pPointData->nY;
                    pSpawnPoint->nZ = pPointData->nZ;
                    pSpawnPoint->nRange = (INT32)pPointData->wRange * POINT_PER_CM;
                    pSpawnPoint->nDirection = pPointData->wDirection;

                    if (pSpawnPoint->nDirection < 0 || pSpawnPoint->nDirection >= MAX_DIRECTION_COUNT)
                    {
                        INT32 nX = pSpawnPoint->nX / POINT_PER_CM;
                        INT32 nY = pSpawnPoint->nY / POINT_PER_CM;
                        INT32 nZ = pSpawnPoint->nZ / POINT_PER_CM;
                        ConvertCoordN2O(nX, nY, nZ);
                        WRN("wrong face direction at point scene %d %s [%d, %d, %d]",
                            pSceneData->nSceneID, pSceneData->szConfigName, nX, nY, nZ);
                        m_nErrorCount++;
                    }

                    pSpawnPoint->nLevelOffset = pMediumGroupData->nLevelOffset;
                    pSpawnPoint->nAIID = pPointData->nAIID;
                    pSpawnPoint->nQuality = pPointData->byQuality;
                    pSpawnPoint->nMinRespawnTime = pMediumGroupData->nMinRespawnTime * GAME_FPS / 1000;
                    pSpawnPoint->nMaxRespawnTime = pMediumGroupData->nMaxRespawnTime * GAME_FPS / 1000;
                    pSpawnPoint->nSpawnTime = pPointData->nSpawnTime * GAME_FPS / 1000;
                    pSpawnPoint->nReviveTime = pPointData->nReviveTime * GAME_FPS / 1000;
                    pSpawnPoint->nCurrentHp = pPointData->nCurrentHp;
                    pSpawnPoint->nCurrentMp = pPointData->nCurrentMp;
                    pSpawnPoint->bCanDropItem = pPointData->bCanDropItem;
                    pSpawnPoint->nPatrolPathID = pPointData->nPatrolPathID;
                    pSpawnPoint->nPatrolStyle = pPointData->nPatrolStyle;
                    pSpawnPoint->nFollowHostID = pPointData->nFollowHostID;
                    pSpawnPoint->nFollowOffsetX = pPointData->nFollowOffsetX;
                    pSpawnPoint->nFollowOffsetY = pPointData->nFollowOffsetY;
                    pSpawnPoint->bAutoSpawn = pMediumGroupData->bAutoSpawn;
                    pSpawnPoint->bAutoRespawn = pMediumGroupData->bAutoRespawn;
                    pSpawnPoint->nSmallGroupID = nSmallGroupID;
                    pSpawnPoint->nMediumGroupID = pMediumGroupData->nID;
                    pSpawnPoint->nLargeGroupID = pLargeGroupData->nID;
                    pSpawnPoint->pDeadBTRoot = NULL;

                    for (INT32 nNpcIndex = 0; nNpcIndex < MAX_NPC_TEMPLATE_COUNT; nNpcIndex++)
                    {
                        // Some NPCs are blocked
                        if (blockNpc != m_mapBlockNpc.end())
                        {
                            auto find_block_npc = blockNpc->second.find(pPointData->NpcList[nNpcIndex].nTemplateID);
                            if (find_block_npc != blockNpc->second.end())
                                continue;
                        }

                        if (pPointData->NpcList[nNpcIndex].nTemplateID > 0 && pPointData->NpcList[nNpcIndex].nWeight > 0)
                        {
                            // Check whether the monster exists
                            if (CDataManager<NPC_DATA>::GetInstance().GetData(pPointData->NpcList[nNpcIndex].nTemplateID) == NULL)
                            {
                                INT32 nX = pSpawnPoint->nX / POINT_PER_CM;
                                INT32 nY = pSpawnPoint->nY / POINT_PER_CM;
                                INT32 nZ = pSpawnPoint->nZ / POINT_PER_CM;
                                ConvertCoordN2O(nX, nY, nZ);

                                WRN("invalid npc template id %d at [%d, %d, %d] medium group %d scene %d %s",
                                    pPointData->NpcList[nNpcIndex].nTemplateID, nX, nY, nZ,
                                    pMediumGroupData->nID, pSceneData->nSceneID, pSceneData->szConfigName);
                                m_nErrorCount++;
                            }

                            // Check whether insertion succeeded
                            nRetCode = pSpawnPoint->RandNpc.Add(pPointData->NpcList[nNpcIndex].nTemplateID, pPointData->NpcList[nNpcIndex].nWeight);
                            if (!nRetCode)
                            {
                                INT32 nX = pSpawnPoint->nX / POINT_PER_CM;
                                INT32 nY = pSpawnPoint->nY / POINT_PER_CM;
                                INT32 nZ = pSpawnPoint->nZ / POINT_PER_CM;
                                ConvertCoordN2O(nX, nY, nZ);

                                WRN("failed to add npc template, id %d at [%d, %d, %d] medium group %d scene %d %s",
                                    pPointData->NpcList[nNpcIndex].nTemplateID, nX, nY, nZ,
                                    pMediumGroupData->nID, pSceneData->nSceneID, pSceneData->szConfigName);
                                m_nErrorCount++;
                            }

                            // Locally added test code
                            //auto it = setNpcID.find(pPointData->NpcList[nNpcIndex].nTemplateID);
                            //if (it != setNpcID.end())
                            //{
                            //    WRN("========================================");
                            //    WRN("");
                            //
                            //    INT32 tmpnX = pSpawnPoint->nX / POINT_PER_CM;
                            //    INT32 tmpnY = pSpawnPoint->nY / POINT_PER_CM;
                            //    INT32 tmpnZ = pSpawnPoint->nZ / POINT_PER_CM;
                            //    ConvertCoordN2O(tmpnX, tmpnY, tmpnZ);
                            //
                            //    WRN("FindElite - MapID %d, nNpcTemplateID %d, nWeight %d, (nLargeGroupID %d, nMediumGroupID %d, nSmallGroupID %d) ,xyz [%d, %d, %d], [%d, %d, %d], nRange %d, nDirection %d, nMinRespawnTime %d, nMaxRespawnTime %d, nSpawnTime %d, nReviveTime %d, Converted [nMinRespawnTime %d, nMaxRespawnTime %d, nSpawnTime %d, nReviveTime %d], bAutoSpawn %d, bAutoRespawn %d",
                            //        pSceneData->nSceneID, pPointData->NpcList[nNpcIndex].nTemplateID, pPointData->NpcList[nNpcIndex].nWeight, 
                            //        pSpawnPoint->nLargeGroupID, pSpawnPoint->nMediumGroupID, pSpawnPoint->nSmallGroupID, 
                            //        pSpawnPoint->nX, pSpawnPoint->nY, pSpawnPoint->nZ, tmpnX, tmpnY, tmpnZ,
                            //        pSpawnPoint->nRange, pSpawnPoint->nDirection,
                            //        pMediumGroupData->nMinRespawnTime, pMediumGroupData->nMaxRespawnTime, pPointData->nSpawnTime, pPointData->nReviveTime,
                            //        pSpawnPoint->nMinRespawnTime, pSpawnPoint->nMaxRespawnTime, pSpawnPoint->nSpawnTime, pSpawnPoint->nReviveTime,
                            //        pSpawnPoint->bAutoSpawn, pSpawnPoint->bAutoRespawn);
                            //
                            //    WRN("");
                            //    WRN("========================================");
                            //    WRN("");
                            //}
                        }
                    }

                    pSmallGroup->nPointCount++;

                    if (pSpawnPoint->RandNpc.GetTotalWeight() <= 0)
                    {
                        INT32 nX = pSpawnPoint->nX / POINT_PER_CM;
                        INT32 nY = pSpawnPoint->nY / POINT_PER_CM;
                        INT32 nZ = pSpawnPoint->nZ / POINT_PER_CM;
                        ConvertCoordN2O(nX, nY, nZ);

                        WRN("no npc template data at [%d, %d, %d] medium group %d scene %d %s",
                            nX, nY, nZ, pMediumGroupData->nID, pSceneData->nSceneID, pSceneData->szConfigName);
                        m_nErrorCount++;
                    }

                    if (!pSceneStaticData->CheckGrid(pSpawnPoint->nX, pSpawnPoint->nY, pSpawnPoint->nZ + GRID_WIDTH))
                    {
                        INT32 nX = pSpawnPoint->nX / POINT_PER_CM;
                        INT32 nY = pSpawnPoint->nY / POINT_PER_CM;
                        INT32 nZ = pSpawnPoint->nZ / POINT_PER_CM;
                        ConvertCoordN2O(nX, nY, nZ);

                        WRN("detected wrong spawn pos [%d, %d, %d] while loading npc spawn point %d medium group %d large group %d in scene %d %s",
                            nX, nY, nZ, pSpawnPoint->nID, pMediumGroup->nID, pLargeGroup->nID, pSceneData->nSceneID, pSceneData->szConfigName);
                        m_nErrorCount++;
                    }

                }

                pMediumGroup->nSmallGroupCount++;
            }

            pLargeGroup->nMediumGroupCount++;
        }
    }

    return TRUE;
Exit0:
    if (pSceneStaticData)
    {
        pSceneStaticData->_ClearNpcSpawnData();
    }
    return FALSE;
}

BOOL CSceneFileMgr::_LoadDoodadSpawnData(const SCENE_DATA* pSceneData, CSceneStaticData* pSceneStaticData, char* pData, INT32 nSize)
{
    INT32 nRetCode = 0;
    DOODAD_SPAWN_DATA_HEADER* pHeader = NULL;

    LOG_PROCESS_ERROR(pSceneStaticData);
    LOG_PROCESS_ERROR(pData);

    pHeader = (DOODAD_SPAWN_DATA_HEADER*)pData;
    pData += sizeof(DOODAD_SPAWN_DATA_HEADER);

    LOG_PROCESS_ERROR(pHeader->nGroupCount < MAX_DOODAD_SPAWN_GROUP_COUNT);
    pSceneStaticData->m_nDoodadSpawnGroupCount = pHeader->nGroupCount;

    pSceneStaticData->m_pDoodadSpawnGroupTable = new DOODAD_SPAWN_GROUP[pHeader->nGroupCount];
    LOG_PROCESS_ERROR(pSceneStaticData->m_pDoodadSpawnGroupTable);
    pSceneStaticData->m_dwMemorySize += sizeof(DOODAD_SPAWN_GROUP) * pHeader->nGroupCount;

    for (INT32 nGroupID = 0; nGroupID < pHeader->nGroupCount; nGroupID++)
    {
        DOODAD_SPAWN_GROUP& rGroup = pSceneStaticData->m_pDoodadSpawnGroupTable[nGroupID];
        DOODAD_SPAWN_GROUP_DATA* pGroup = (DOODAD_SPAWN_GROUP_DATA*)pData;
        pData += sizeof(DOODAD_SPAWN_GROUP_DATA) + sizeof(DOODAD_SPAWN_POINT_DATA) * pGroup->nPointCount;

        rGroup.nGroupID = nGroupID;
        rGroup.nMaxConcurrentPoint = pGroup->nMaxConcurrentPoint;
        rGroup.IsAutoSpawn = pGroup->IsAutoSpawn;
        rGroup.IsAutoReSpawn = pGroup->IsAutoReSpawn;
        rGroup.nRespawnInterval = pGroup->nRespawnInterval * GAME_FPS / 1000;
        rGroup.nPointCount = pGroup->nPointCount;

        for (INT32 nIndex = 0; nIndex < MAX_DOODAD_COUNT_PER_GROUP; nIndex++)
        {
            if (pGroup->DoodadList[nIndex].nTemplateID != 0)
            {
                if (CDataManager<DOODAD_DATA>::GetInstance().GetData(pGroup->DoodadList[nIndex].nTemplateID) == NULL)
                {
                    WRN("invaild doodad template id %d in group %d scene %s", pGroup->DoodadList[nIndex].nTemplateID,
                        rGroup.nGroupID, pSceneData->szConfigName);
                    m_nErrorCount++;
                }

                nRetCode = rGroup.RandDoodad.Add(pGroup->DoodadList[nIndex].nTemplateID, pGroup->DoodadList[nIndex].nWeight);
                if (!nRetCode)
                {
                    WRN("total doodad rand weight exceeds maximum (1024)");
                    m_nErrorCount++;
                }
            }
        }

        if (rGroup.RandDoodad.GetTotalWeight() <= 0)
        {
            WRN("no doodad template data in group(point count %d) scene %d(%s)",
                pGroup->nPointCount, pSceneData->nSceneID, pSceneData->szConfigName);
            m_nErrorCount++;
        }

        LOG_PROCESS_ERROR(pGroup->nPointCount < MAX_DOODAD_SPAWN_POINT_COUNT);
        rGroup.pPointList = new DOODAD_SPAWN_POINT[pGroup->nPointCount];
        LOG_PROCESS_ERROR(rGroup.pPointList);
        pSceneStaticData->m_dwMemorySize += sizeof(DOODAD_SPAWN_POINT) * pGroup->nPointCount;

        for (INT32 nIndex = 0; nIndex < pGroup->nPointCount; nIndex++)
        {
            rGroup.pPointList[nIndex].nID = nIndex;
            rGroup.pPointList[nIndex].nX = pGroup->PointList[nIndex].nX;
            rGroup.pPointList[nIndex].nY = pGroup->PointList[nIndex].nY;
            rGroup.pPointList[nIndex].nZ = pGroup->PointList[nIndex].nZ;
            rGroup.pPointList[nIndex].nFaceDirection = pGroup->PointList[nIndex].shDirection;
            rGroup.pPointList[nIndex].nGroupID = rGroup.nGroupID;

            if (!pSceneStaticData->CheckGrid(rGroup.pPointList[nIndex].nX, rGroup.pPointList[nIndex].nY, rGroup.pPointList[nIndex].nZ + GRID_WIDTH))
            {
                INT32 nX = (INT32)(rGroup.pPointList[nIndex].nX / POINT_PER_CM);
                INT32 nY = (INT32)(rGroup.pPointList[nIndex].nY / POINT_PER_CM);
                INT32 nZ = (INT32)(rGroup.pPointList[nIndex].nZ / POINT_PER_CM);
                ConvertCoordN2O(nX, nY, nZ);

                WRN("detected wrong spawn pos [%d, %d, %d] while loading doodad spawn point %d group %d in scene %s",
                    nX, nY, nZ, nIndex, rGroup.nGroupID, pSceneData->szConfigName);
                m_nErrorCount++;
            }
        }
    }

    return TRUE;
Exit0:
    pSceneStaticData->_ClearDoodadSpawnData();

    return FALSE;
}

BOOL CSceneFileMgr::_LoadServerPoint(const SCENE_DATA* pSceneData, CSceneStaticData* pSceneStaticData, char* pData, INT32 nSize)
{
    std::pair<CSceneStaticData::MAP_NAME_2_POINT::iterator, bool> InsRet;
    SERVER_POINT_HEADER* pHeader = NULL;

    LOG_PROCESS_ERROR(pSceneStaticData);
    LOG_PROCESS_ERROR(pData);

    pHeader = (SERVER_POINT_HEADER*)pData;

    pSceneStaticData->m_nServerPointCount = 0;
    pSceneStaticData->m_nRevivePointCount = 0;
    for (INT32 nIndex = 0; nIndex < pHeader->nPointCount; ++nIndex)
    {
        if (pSceneStaticData->m_nServerPointCount < pHeader->PointTable[nIndex].nID)
            pSceneStaticData->m_nServerPointCount = pHeader->PointTable[nIndex].nID;
        if ((pHeader->PointTable[nIndex].qwNameID & 0xFFFF) == 'R#')
        {
            pSceneStaticData->m_nRevivePointCount++;
        }
    }
    pSceneStaticData->m_nServerPointCount++;

    if (pSceneStaticData->m_nServerPointCount > 0)
    {
        pSceneStaticData->m_pServerPointTable = new SERVER_POINT[pSceneStaticData->m_nServerPointCount];
        LOG_PROCESS_ERROR(pSceneStaticData->m_pServerPointTable);
        pSceneStaticData->m_dwMemorySize += sizeof(SERVER_POINT) * pSceneStaticData->m_nServerPointCount;
    }

    if (pSceneStaticData->m_nRevivePointCount > 0)
    {
        pSceneStaticData->m_pRevivePointTable = new REVIVE_POINT[pSceneStaticData->m_nServerPointCount];
        LOG_PROCESS_ERROR(pSceneStaticData->m_pRevivePointTable);
        pSceneStaticData->m_dwMemorySize += sizeof(REVIVE_POINT) * pSceneStaticData->m_nServerPointCount;
    }

    for (INT32 nIndex = 0; nIndex < pHeader->nPointCount; ++nIndex)
    {
        INT32 nID = pHeader->PointTable[nIndex].nID;
        SERVER_POINT* pServerPoint = pSceneStaticData->m_pServerPointTable + nID;
        pServerPoint->nID = nID;
        pServerPoint->qwNameID = pHeader->PointTable[nIndex].qwNameID;
        pServerPoint->nX = pHeader->PointTable[nIndex].nX;
        pServerPoint->nY = pHeader->PointTable[nIndex].nY;
        pServerPoint->nZ = pHeader->PointTable[nIndex].nZ;
        pServerPoint->nDirection = pHeader->PointTable[nIndex].nDirection;
        pServerPoint->nRange = pHeader->PointTable[nIndex].nRange;

        InsRet = pSceneStaticData->m_mapServerPoint.insert(std::make_pair(pServerPoint->qwNameID, pServerPoint));
        if (!InsRet.second)
        {
            INT32 nX = pServerPoint->nX / POINT_PER_CM;
            INT32 nY = pServerPoint->nY / POINT_PER_CM;
            INT32 nZ = pServerPoint->nZ / POINT_PER_CM;
            ConvertCoordN2O(nX, nY, nZ);
            WRN("duplicated server point name %s at [%d, %d, %d]", (char*)(&pServerPoint->qwNameID), nX, nY, nZ);
            m_nErrorCount++;
        }

        if ((pServerPoint->qwNameID & 0xFFFF) == 'R#')
        {
            char szName[9] = { 0 };
            memcpy(szName, &pServerPoint->qwNameID, sizeof(pServerPoint->qwNameID));
            szName[8] = 0;

            REVIVE_POINT* pRevivePoint = pSceneStaticData->m_pRevivePointTable + pServerPoint->nID;
            memcpy(pRevivePoint, pServerPoint, sizeof(*pServerPoint));
            pRevivePoint->nFaction = 0;

            for (INT32 nChar = 2; nChar < 8; nChar++)
            {
                if (szName[nChar] == 'F')
                {
                    pRevivePoint->nFaction = atoi(szName + nChar + 1);
                    break;
                }
            }
        }

        if ((pServerPoint->qwNameID & 0xFFFF) == 'P#')
        {
            char szName[9] = { 0 };
            memcpy(szName, &pServerPoint->qwNameID, sizeof(pServerPoint->qwNameID));
            szName[8] = 0;

            INT32 nPathID = -1;
            INT32 nPointIndex = -1;
            sscanf(szName, PATROL_TAG "%d" PATROL_SEPERATOR "%d", &nPathID, &nPointIndex);
            if (nPathID <= 0 || nPathID >= MAX_PATROL_PATH_ID ||
                nPointIndex <= 0 || nPointIndex >= MAX_POINT_COUNT_PER_PATH)
            {
                WRN("error patrol path id %d or node index %d", nPathID, nPointIndex);
                m_nErrorCount++;
                continue;
            }

            LOG_PROCESS_ERROR(pSceneStaticData->m_PathPointTable[nPathID].nPointCount + 1 < MAX_POINT_COUNT_PER_PATH);
            LOG_PROCESS_ERROR(pSceneStaticData->m_PathPointTable[nPathID].PointList[nPointIndex - 1].nID == 0);
            pSceneStaticData->m_PathPointTable[nPathID].PointList[nPointIndex - 1] = *pServerPoint;
            pSceneStaticData->m_PathPointTable[nPathID].nPointCount++;
        }

        if (!pSceneStaticData->CheckGrid(pServerPoint->nX, pServerPoint->nY, pServerPoint->nZ + GRID_WIDTH))
        {
            INT32 nX = pServerPoint->nX / POINT_PER_CM;
            INT32 nY = pServerPoint->nY / POINT_PER_CM;
            INT32 nZ = pServerPoint->nZ / POINT_PER_CM;
            ConvertCoordN2O(nX, nY, nZ);

            WRN("detected wrong pos [%d, %d, %d] while loading server point %d in scene %s",
                nX, nY, nZ, pServerPoint->nID, pSceneData->szConfigName);
            m_nErrorCount++;
        }
    }

    return TRUE;
Exit0:

    SAFE_DELETE_ARRAY(pSceneStaticData->m_pServerPointTable);
    SAFE_DELETE_ARRAY(pSceneStaticData->m_pRevivePointTable);

    return FALSE;
}

// Load teleports.bin
BOOL CSceneFileMgr::_LoadTeleport()
{
    INT32 nRetCode = 0;
    TCHAR szFileName[PATH_LEN];
    IFile* pFile = NULL;
    INT32 nFileSize = 0;
    char* pData = NULL;
    INT32* pnCount = NULL;
    SCENE_TELEPORTS* pTeleportList = NULL;
    MAP_ID_2_DATA::iterator scenefileItr;

    snprintf(szFileName, PATH_LEN, "%s/%s/%s", CGameWorld::GetInstance().GetConfigPath(), SCENE_FILE_FOLDER, TELEPORT_FILE_NAME);
    pFile = g_OpenFile(szFileName, "rb");
    LOG_PROCESS_ERROR(pFile);

    nFileSize = pFile->Size();
    LOG_PROCESS_ERROR(nFileSize);

    pData = new char[nFileSize];
    LOG_PROCESS_ERROR(pData);

    nRetCode = pFile->Read(pData, nFileSize);
    LOG_PROCESS_ERROR(nRetCode == nFileSize);

    pFile->Close();
    pFile = NULL;

    pnCount = (INT32*)pData;

    pTeleportList = (SCENE_TELEPORTS*)(pData + sizeof(*pnCount));
    for (INT32 i = 0; i < *pnCount; ++i)
    {
        INT32 nTelportCount = 0;
        CSceneStaticData* pSceneStaticData = NULL;
        SCENE_TELEPORTS* poTeleport;

        poTeleport = (SCENE_TELEPORTS*)(pTeleportList + i);
        LOG_PROCESS_ERROR(poTeleport);

        scenefileItr = m_mapScene.find(poTeleport->SrcSceneID);
        if (scenefileItr == m_mapScene.end())
        {
            WRN("cannot find refered scene %d in teleport point %d", poTeleport->SrcSceneID, poTeleport->ID);
            m_nErrorCount++;
            continue;
        }

        pSceneStaticData = scenefileItr->second;
        LOG_PROCESS_ERROR(pSceneStaticData);

        LOG_PROCESS_ERROR(poTeleport->ID > 0 && poTeleport->ID < MAX_TELEPORT_POINT_PER_SCENE);
        memcpy(&pSceneStaticData->m_TeleportPointList[poTeleport->ID], poTeleport, sizeof(SCENE_TELEPORTS));
    }
    SAFE_DELETE_ARRAY(pData);

    return TRUE;
Exit0:
    if (pFile)
        pFile->Close();
    SAFE_DELETE_ARRAY(pData);

    return FALSE;
}

// Load teleport_platforms.bin
BOOL CSceneFileMgr::_LoadTeleportPlatforms(void)
{
    INT32 nRetCode = 0;
    TCHAR szFileName[PATH_LEN];
    IFile* pFile = NULL;
    INT32 nFileSize = 0;
    char* pData = NULL;
    INT32* pnCount = NULL;
    TELEPORT_PLATFORM* pTeleportTownsList = NULL;
    const SCENE_DATA* pstSceneData = NULL;

    snprintf(szFileName, PATH_LEN, "%s/%s/%s", CGameWorld::GetInstance().GetConfigPath(), SCENE_FILE_FOLDER, TELEPORT_PLATFORM_FILE_NAME);
    pFile = g_OpenFile(szFileName, "rb");
    LOG_PROCESS_ERROR(pFile);

    nFileSize = pFile->Size();
    LOG_PROCESS_ERROR(nFileSize);

    pData = new char[nFileSize];
    LOG_PROCESS_ERROR(pData);

    nRetCode = pFile->Read(pData, nFileSize);
    LOG_PROCESS_ERROR(nRetCode == nFileSize);

    pFile->Close();
    pFile = NULL;

    pnCount = (INT32*)pData;

    pTeleportTownsList = (TELEPORT_PLATFORM*)(pData + sizeof(*pnCount));
    for (INT32 i = 0; i < *pnCount; ++i)
    {
        TELEPORT_PLATFORM* poTeleportform = (TELEPORT_PLATFORM*)(pTeleportTownsList + i);
        LOG_PROCESS_ERROR(poTeleportform);

        LOG_PROCESS_ERROR(poTeleportform->ID >= 0 && poTeleportform->ID < MAX_TELEPORT_PLATFORM_COUNT);

        pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(poTeleportform->nSceneID);
        LOG_PROCESS_ERROR(pstSceneData);
        LOG_PROCESS_ERROR(pstSceneData->nType == estStatic || pstSceneData->nType == estLine || pstSceneData->nType == estUnique);

        memcpy(&m_lstTeleportPlatforms[poTeleportform->ID], poTeleportform, sizeof(TELEPORT_PLATFORM));
    }
    SAFE_DELETE_ARRAY(pData);

    return TRUE;
Exit0:
    if (pFile)
        pFile->Close();
    SAFE_DELETE_ARRAY(pData);

    return FALSE;
}

// Zone phx
BOOL CSceneFileMgr::_LoadPhyxData(const SCENE_DATA* pSceneData, CSceneStaticData* pSceneStaticData, char* pData, INT32 nSize)
{
    INT32 nRegionWidth = 0;
    INT32 nRegionHeight = 0;
    UINT32 dwValid_GRID_DATA = 0;
    INT32 nRegionIndex = 0;
    INT32 nRetCode = 0;
    char szScript[256] = { 0 };
    char szBTTreeName[256] = { 0 };

    SCENE_PHYX_HEADER* pHeader = (SCENE_PHYX_HEADER*)pData;
    char* pPointer = pData + sizeof(SCENE_PHYX_HEADER);

    LOG_PROCESS_ERROR(pHeader->dwTag == SCENE_PHYX_FILE_TAG);
    LOG_PROCESS_ERROR(pHeader->dwVersion == SCENE_PHYX_FILE_VERSION);

    pSceneStaticData->m_pcSceneData = pSceneData;
    pSceneStaticData->m_nMaxZoneX = pHeader->nMaxZoneX;
    pSceneStaticData->m_nMaxZoneY = pHeader->nMaxZoneY;

    memset(pSceneStaticData->m_ZoneData, NULL, sizeof(pSceneStaticData->m_ZoneData));

    // load BT
    for (INT32 nIndex = 0; nIndex < MAX_VOLUME_COUNT; nIndex++)
    {
        pSceneStaticData->m_VolumeTreeTable[nIndex] = NULL;
        snprintf(szBTTreeName, sizeof(szBTTreeName), "%s_%d", g_BTEventName[evtTouchVolume], nIndex);
        if (pSceneStaticData->m_pScript && pSceneStaticData->m_pScript->IsTableExist(szBTTreeName))
        {
            nRetCode = pSceneStaticData->m_pScript->CallFunction(LOAD_BEHAVIOR_TREE, "s:o", szBTTreeName, &pSceneStaticData->m_VolumeTreeTable[nIndex]);
            if (FALSE == nRetCode || NULL == pSceneStaticData->m_VolumeTreeTable[nIndex])
            {
                CRI("failed to load behavior tree %s in scene %d volume %d", szBTTreeName, pSceneData->nSceneID, nIndex);
                m_nErrorCount++;
            }
        }
    }

    memcpy(pSceneStaticData->m_nMaxVolumeTriggerCount, pHeader->nMaxVolumeTriggerCount, sizeof(pSceneStaticData->m_nMaxVolumeTriggerCount));

    for (UINT32 nZoneIndex = 0; nZoneIndex < pHeader->dwZoneCount; nZoneIndex++)
    {
        ZONE_DATA* pZoneData = (ZONE_DATA*)pPointer;
        pPointer += sizeof(ZONE_DATA) + sizeof(GRID_DATA) * (pZoneData->nGridCount + 1);

        LOG_PROCESS_ERROR(pZoneData->usZoneX < MAX_ZONE_PER_SCENE);
        LOG_PROCESS_ERROR(pZoneData->usZoneY < MAX_ZONE_PER_SCENE);

        pSceneStaticData->m_ZoneData[pZoneData->usZoneX][pZoneData->usZoneY] = pZoneData;
        pSceneStaticData->m_nInstanceZoneTotalSize +=
            sizeof(ZONE_INSTANCE_DATA) + (pZoneData->nGridCount + 1) * sizeof(GRID_INSTANCE_DATA);

        for (INT32 nGridIndex = 1; nGridIndex <= pZoneData->nGridCount; nGridIndex++)
        {
            GRID_DATA* pGridData = pZoneData->GridList + nGridIndex;
            if (!pGridData->bOutOfPoly)
                pSceneStaticData->m_dwSceneWalkableArea++;
        }
        pSceneStaticData->m_dwSceneArea += pZoneData->nGridCount;
    }

    // AI analysis
    // Rows and columns of logical regions
    // A scene contains multiple zones, each ZONE_WIDTH units wide.
    // A region is a larger logical unit of size REGION_WIDTH, usually a multiple of ZONE_WIDTH.
    // REGION_WIDTH / ZONE_WIDTH is the number of zones along one region dimension.
    // Divide the scene zone column count m_nMaxZoneX by zones per region to obtain region columns (+1 handles the remainder).
    // nRegionHeight (region rows) is calculated similarly.
    nRegionWidth = pSceneStaticData->m_nMaxZoneX / (REGION_WIDTH / ZONE_WIDTH) + 1;
    LOG_PROCESS_ERROR(nRegionWidth < (1 << 16));
    nRegionHeight = pSceneStaticData->m_nMaxZoneY / (REGION_WIDTH / ZONE_WIDTH) + 1;
    LOG_PROCESS_ERROR(nRegionHeight < (1 << 16));

    // AI analysis
    // Count valid regions in the scene
    // A region is valid if it contains at least one walkable grid (bOutOfPoly == 0).
    // Use GetGroundGrid to check walkability; mark the region valid if a grid has bOutOfPoly == 0.
    // 1 - Loop bound: REGION_WIDTH / GRID_WIDTH
    // Each region has size REGION_WIDTH, and each grid has size GRID_WIDTH.
    // Therefore the number of grids per region along X or Y is REGION_WIDTH / GRID_WIDTH.
    // For 16 = 2097152 / 131072, each region has 16 grids along each axis, or 256 grids to scan.
    // 2 - The !bValidRegion condition exits early once a walkable grid marks the region valid.
    pSceneStaticData->m_dwValidRegion = 0;
    dwValid_GRID_DATA = 0;
    for (INT32 nRegionX = 0; nRegionX < nRegionWidth; nRegionX++)
    {
        for (INT32 nRegionY = 0; nRegionY < nRegionHeight; nRegionY++)
        {
            BOOL bValidRegion = FALSE;

            for (INT32 nGridX = 0; nGridX < (REGION_WIDTH / GRID_WIDTH) && !bValidRegion; nGridX++)
            {
                for (INT32 nGridY = 0; nGridY < (REGION_WIDTH / GRID_WIDTH) && !bValidRegion; nGridY++)
                {
                    const GRID_DATA* pcGridData = NULL;
                    INT32 nX = nRegionX * REGION_WIDTH + nGridX * GRID_WIDTH + GRID_WIDTH / 2;
                    INT32 nY = nRegionY * REGION_WIDTH + nGridY * GRID_WIDTH + GRID_WIDTH / 2;

                    pcGridData = pSceneStaticData->GetGroundGrid(nX, nY);
                    while (pcGridData)
                    {
                        if (!pcGridData->bOutOfPoly)
                        {
                            bValidRegion = TRUE;
                            break;
                        }

                        pcGridData = pSceneStaticData->GetNextGrid(nX, nY, pcGridData);
                    }
                }
            }

            if (bValidRegion)
                pSceneStaticData->m_dwValidRegion++;
        }
    }

    pSceneStaticData->m_pdwValidRegionList = new UINT32[pSceneStaticData->m_dwValidRegion + 1];

    // AI analysis
    // Populate m_pdwValidRegionList
    // Key assignment: pSceneStaticData->m_pdwValidRegionList[nRegionIndex++] = nRegionX << 16 | nRegionY;
    // Shift nRegionX left 16 bits for the high half; store nRegionY in the low 16 bits.
    // The result is a 32-bit integer containing both X and Y.
    // This provides efficient storage and retrieval.
    // INT32 decodedX = encodedValue >> 16;  // Decode X
    // INT32 decodedY = encodedValue & 0xFFFF; // Decode Y
    nRegionIndex = 0;
    for (INT32 nRegionX = 0; nRegionX < nRegionWidth; nRegionX++)
    {
        for (INT32 nRegionY = 0; nRegionY < nRegionHeight; nRegionY++)
        {
            BOOL bValidRegion = FALSE;

            for (INT32 nGridX = 0; nGridX < (REGION_WIDTH / GRID_WIDTH) && !bValidRegion; nGridX++)
            {
                for (INT32 nGridY = 0; nGridY < (REGION_WIDTH / GRID_WIDTH) && !bValidRegion; nGridY++)
                {
                    const GRID_DATA* pcGridData = NULL;
                    INT32 nX = nRegionX * REGION_WIDTH + nGridX * GRID_WIDTH + GRID_WIDTH / 2;
                    INT32 nY = nRegionY * REGION_WIDTH + nGridY * GRID_WIDTH + GRID_WIDTH / 2;

                    pcGridData = pSceneStaticData->GetGroundGrid(nX, nY);
                    while (pcGridData)
                    {
                        if (!pcGridData->bOutOfPoly)
                        {
                            bValidRegion = TRUE;
                            break;
                        }

                        pcGridData = pSceneStaticData->GetNextGrid(nX, nY, pcGridData);
                    }
                }
            }

            if (bValidRegion)
            {
                pSceneStaticData->m_pdwValidRegionList[nRegionIndex++] = nRegionX << 16 | nRegionY;
            }
        }
    }

    LOG_PROCESS_ERROR(nRegionIndex == pSceneStaticData->m_dwValidRegion);
    pSceneStaticData->m_pdwValidRegionList[nRegionIndex] = 0;

    return TRUE;
Exit0:

    return FALSE;
}

// Terrain nav
BOOL CSceneFileMgr::_LoadNavData(const SCENE_DATA* pSceneData, CSceneStaticData* pSceneStaticData, char* pData, INT32 nSize)
{
    INT32 nRetCode = 0;
    SCENE_NAV_HEADER* pHeader = (SCENE_NAV_HEADER*)pData;
    char* pPointer = pData + sizeof(SCENE_NAV_HEADER);

    LOG_PROCESS_ERROR(pHeader->dwTag == SCENE_NAV_FILE_TAG);
    LOG_PROCESS_ERROR(pHeader->dwVersion == SCENE_NAV_FILE_VERSION || pHeader->dwVersion == 2);

    // Vertices
    pSceneStaticData->m_NavVertexData = new NAV_VERTEX_DATA[pHeader->wVertexCount + 2];
    LOG_PROCESS_ERROR(pSceneStaticData->m_NavVertexData);
    pSceneStaticData->m_dwMemorySize += sizeof(NAV_VERTEX_DATA) * (pHeader->wVertexCount + 2);

    // Edges
    pSceneStaticData->m_NavEdgeData = new NAV_EDGE_DATA[pHeader->wEdgeCount];
    LOG_PROCESS_ERROR(pSceneStaticData->m_NavEdgeData);
    pSceneStaticData->m_dwMemorySize += sizeof(NAV_EDGE_DATA) * pHeader->wEdgeCount;

    pSceneStaticData->m_NavFixedVertexData = new NAV_VERTEX_DATA[pHeader->wVertexCount + 2];
    LOG_PROCESS_ERROR(pSceneStaticData->m_NavFixedVertexData);
    pSceneStaticData->m_dwMemorySize += sizeof(NAV_VERTEX_DATA) * (pHeader->wVertexCount + 2);

    memcpy(pSceneStaticData->m_NavVertexData, pHeader->VertexData, sizeof(NAV_VERTEX_DATA) * pHeader->wVertexCount);
    memcpy(pSceneStaticData->m_NavEdgeData, pHeader->VertexData + pHeader->wVertexCount, sizeof(NAV_EDGE_DATA) * pHeader->wEdgeCount);
    if (pHeader->dwVersion == SCENE_NAV_FILE_VERSION)
    {
        memcpy(pSceneStaticData->m_NavFixedVertexData, (NAV_EDGE_DATA*)(pHeader->VertexData + pHeader->wVertexCount) + pHeader->wEdgeCount,
            sizeof(NAV_VERTEX_DATA) * pHeader->wVertexCount);
    }
    else
    {
        memcpy(pSceneStaticData->m_NavFixedVertexData, pHeader->VertexData, sizeof(NAV_VERTEX_DATA) * pHeader->wVertexCount);
    }

    pSceneStaticData->m_nNavVertexCount = pHeader->wVertexCount;
    pSceneStaticData->m_nNavEdgeCount = pHeader->wEdgeCount;

    return TRUE;
Exit0:
    SAFE_DELETE_ARRAY(pSceneStaticData->m_NavVertexData);
    SAFE_DELETE_ARRAY(pSceneStaticData->m_NavFixedVertexData);
    SAFE_DELETE_ARRAY(pSceneStaticData->m_NavEdgeData);

    return FALSE;
}

// Load phx/nav/scn
BOOL CSceneFileMgr::CTraverseSceneData::operator()(const SCENE_DATA* pSceneData)
{
    BOOL bResult = FALSE;
    INT32 nRetCode = 0;
    IFile* pFile = NULL;
    TCHAR szFileName[PATH_LEN] = { 0 };
    char* pData = NULL;
    INT32 nFileSize = 0;
    SCENE_FILE_HEADER* pFileHeader = NULL;
    CSceneStaticData* pSceneStaticData = NULL;
    std::pair<MAP_ID_2_DATA::iterator, bool> InsRet;

    LOG_PROCESS_ERROR(pSceneData);

    pSceneStaticData = new CSceneStaticData;
    LOG_PROCESS_ERROR(pSceneStaticData);

    nRetCode = pSceneStaticData->Init();
    LOG_PROCESS_ERROR(nRetCode);

    // Load scene script
#ifndef	_ROBOT
    snprintf(szFileName, PATH_LEN, "%s/%s/%s/main.lua", CGameWorld::GetInstance().GetConfigPath(),
        SCENE_SCRIPT_FOLDER, pSceneData->szConfigName);

    if (g_IsFileExist(szFileName))
    {
        snprintf(szFileName, PATH_LEN, "%s/%s/main.lua", SCENE_SCRIPT_FOLDER, pSceneData->szConfigName);

        pSceneStaticData->m_pScript = CScriptMgr::GetInstance().FindScript(szFileName);
        if (pSceneStaticData->m_pScript == NULL)
            pSceneStaticData->m_pScript = CScriptMgr::GetInstance().NewScript(szFileName);

        if (pSceneStaticData->m_pScript)
        {
            for (INT32 nIndex = evtSceneBegin; nIndex < evtSceneEnd; nIndex++)
            {
                if (pSceneStaticData->m_pScript && pSceneStaticData->m_pScript->IsTableExist(g_BTEventName[nIndex]))
                {
                    nRetCode = pSceneStaticData->m_pScript->CallFunction(LOAD_BEHAVIOR_TREE, "s:o", g_BTEventName[nIndex],
                        pSceneStaticData->m_pBTTable + nIndex);
                    if (FALSE == nRetCode || NULL == pSceneStaticData->m_pBTTable[nIndex])
                    {
                        WRN("failed to load bt %s in scene %d %s", g_BTEventName[nIndex], pSceneData->nSceneID, pSceneData->szConfigName);
                        pThis->m_nErrorCount++;
                    }
                }
            }
        }
        else
        {
            WRN("failed to load scene script %d %s ", pSceneData->nSceneID, pSceneData->szConfigName);
        }
    }
#endif

    // Load phx
    snprintf(szFileName, PATH_LEN, "%s/%s/%s/%s.%s", CGameWorld::GetInstance().GetConfigPath(), SCENE_FILE_FOLDER,
        pSceneData->szConfigName, pSceneData->szConfigName, SCENE_PHYX_FILE_EXT_NAME);

    pFile = g_OpenFile(szFileName, "rb");
    if (pFile)
    {
        nFileSize = pFile->Size();
        LOG_PROCESS_ERROR(nFileSize > sizeof(SCENE_PHYX_HEADER));

        pSceneStaticData->m_pZoneRawData = new char[nFileSize];
        LOG_PROCESS_ERROR(pSceneStaticData->m_pZoneRawData);
        pSceneStaticData->m_dwMemorySize += nFileSize;

        nRetCode = pFile->Read(pSceneStaticData->m_pZoneRawData, nFileSize);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = pThis->_LoadPhyxData(pSceneData, pSceneStaticData, pSceneStaticData->m_pZoneRawData, nFileSize);
        LOG_PROCESS_ERROR(nRetCode);

        pFile->Close();
        pFile = NULL;
    }

    // Load nav
    snprintf(szFileName, PATH_LEN, "%s/%s/%s/%s.%s", CGameWorld::GetInstance().GetConfigPath(), SCENE_FILE_FOLDER,
        pSceneData->szConfigName, pSceneData->szConfigName, SCENE_NAV_FILE_EXT_NAME);

    pFile = g_OpenFile(szFileName, "rb");
    if (pFile)
    {
        nFileSize = pFile->Size();
        LOG_PROCESS_ERROR(nFileSize > sizeof(SCENE_NAV_HEADER));

        pData = new char[nFileSize];
        LOG_PROCESS_ERROR(pData);

        nRetCode = pFile->Read(pData, nFileSize);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = pThis->_LoadNavData(pSceneData, pSceneStaticData, pData, nFileSize);
        LOG_PROCESS_ERROR(nRetCode);

        pFile->Close();
        pFile = NULL;
        SAFE_DELETE_ARRAY(pData);
    }

    // Load scn
    snprintf(szFileName, PATH_LEN, "%s/%s/%s/%s.%s", CGameWorld::GetInstance().GetConfigPath(),
        SCENE_FILE_FOLDER, pSceneData->szConfigName, pSceneData->szConfigName, SCENE_FILE_EXT_NAME);

    pFile = g_OpenFile(szFileName, "rb");
    LOG_PROCESS_ERROR(pFile);
    if (pFile)
    {
        nFileSize = pFile->Size();
        LOG_PROCESS_ERROR(nFileSize > sizeof(SCENE_FILE_HEADER));

        pData = new char[nFileSize];
        LOG_PROCESS_ERROR(pData);

        nRetCode = pFile->Read(pData, nFileSize);
        LOG_PROCESS_ERROR(nRetCode);

        pFileHeader = (SCENE_FILE_HEADER*)pData;
        LOG_PROCESS_ERROR(pFileHeader->dwTag == SCENE_FILE_TAG);
        LOG_PROCESS_ERROR(pFileHeader->dwVersion == SCENE_FILE_VERSION);

        for (INT32 nBlockIndex = 0; nBlockIndex < pFileHeader->nBlockCount; nBlockIndex++)
        {
            SCENE_FILE_BLOCK_HEADER* pBlockHeader = pFileHeader->BlockTable + nBlockIndex;
            switch (pBlockHeader->eBlockType)
            {
            case sbtNpcSingleSpawn:
            {
                nRetCode = pThis->_LoadSingleNpc(
                    pSceneData, pSceneStaticData, pData + pBlockHeader->nBlockOffset, pBlockHeader->nBlockSize);
                LOG_PROCESS_ERROR(nRetCode);
            }
            break;
            case sbtNpcSpawnData:
            {
                nRetCode = pThis->_LoadNpcSpawnData(
                    pSceneData, pSceneStaticData, pData + pBlockHeader->nBlockOffset, pBlockHeader->nBlockSize);
                LOG_PROCESS_ERROR(nRetCode);
            }
            break;
            case sbtDoodadSpawnData:
            {
                nRetCode = pThis->_LoadDoodadSpawnData(
                    pSceneData, pSceneStaticData, pData + pBlockHeader->nBlockOffset, pBlockHeader->nBlockSize);
                LOG_PROCESS_ERROR(nRetCode);
            }
            break;
            case sbtServerPoint:
            {
                nRetCode = pThis->_LoadServerPoint(
                    pSceneData, pSceneStaticData, pData + pBlockHeader->nBlockOffset, pBlockHeader->nBlockSize);
                LOG_PROCESS_ERROR(nRetCode);
            }
            break;
            default:
                WRN("unknown scn file %d(%s) block type %d, size %d", pSceneData->nSceneID, pSceneData->szConfigName, pBlockHeader->eBlockType, pBlockHeader->nBlockSize);
                break;
            }
        }
    }

    pFile->Close();
    pFile = NULL;
    SAFE_DELETE_ARRAY(pData);

    pSceneStaticData->_CheckSpawnPos();

    InsRet = pThis->m_mapScene.insert(std::make_pair(pSceneData->nSceneID, pSceneStaticData));
    LOG_PROCESS_ERROR(InsRet.second);

    bResult = TRUE;

    INF("loaded scene data file %d %s (area %d/%d, named npc %d, region %d/%d) successfully (cost %.2fMB)",
        pSceneData->nSceneID, pSceneData->szConfigName,
        (INT32)(pSceneStaticData->m_dwSceneWalkableArea * 0.64f * 0.64f),
        (INT32)(pSceneStaticData->m_dwSceneArea * 0.64f * 0.64f),
        pSceneStaticData->m_mapSingleNpc.size(),
        pSceneStaticData->m_dwValidRegion,
        pSceneStaticData->m_nMaxZoneX * pSceneStaticData->m_nMaxZoneY / (REGION_WIDTH / ZONE_WIDTH) / (REGION_WIDTH / ZONE_WIDTH),
        pSceneStaticData->m_dwMemorySize / (1024 * 1024.0f));
    pThis->m_dwMemorySize += pSceneStaticData->m_dwMemorySize;


Exit0:
    if (pFile)
        pFile->Close();

    SAFE_DELETE_ARRAY(pData);
    if (bResult)
    {
        pThis->m_nLoadedCount++;
    }
    else
    {
        WRN("failed to load scene data file %d %s", pSceneData->nSceneID, pSceneData->szConfigName);
        SAFE_DELETE(pSceneStaticData);
    }

    return TRUE;
}

BOOL CSceneFileMgr::Init(void)
{
    INT32 nRetCode = 0;
    UINT64 qwStartTick = GetTickCountEx();
    CTraverseSceneData TraverseSceneData;
    CDataManager<SCENE_DATA>& rDataMgr = CDataManager<SCENE_DATA>::GetInstance();

    m_dwMemorySize = 0;

    TraverseSceneData.pThis = this;
    m_nLoadedCount = 0;

    // Add extra NPC spawns / blocks
    Init_AdditionalProcessNpcs();

    // Calls BOOL CSceneFileMgr::CTraverseSceneData::operator()(const SCENE_DATA* pSceneData)
    nRetCode = rDataMgr.Traverse(TraverseSceneData);
    LOG_CHECK_ERROR(nRetCode);

    INF("read %d/%d data in scene file mgr successfully (cost %d ms %.2fMB)",
        m_nLoadedCount, rDataMgr.GetDataCount(),
        (INT32)(GetTickCountEx() - qwStartTick), m_dwMemorySize / (1024 * 1024.0f));

    if (m_nErrorCount > 0)
    {
        g_AppendErrorMsg("场景数据文件中有---%d---个读取错误\n", m_nErrorCount);
    }

    if (m_nLoadedCount < rDataMgr.GetDataCount())
    {
        g_AppendErrorMsg("场景数据文件中有---%d---个场景读取失败\n", rDataMgr.GetDataCount() - m_nLoadedCount);
    }

    // Load teleports.bin
    nRetCode = _LoadTeleport();
    LOG_CHECK_ERROR(nRetCode);

    // Load teleport_platforms.bin
    memset(m_lstTeleportPlatforms, 0, sizeof(m_lstTeleportPlatforms));
    nRetCode = _LoadTeleportPlatforms();
    LOG_CHECK_ERROR(nRetCode);

    //for (std::map<INT32, INT32>::iterator it = s_mapNpcCount.begin();
    //    it != s_mapNpcCount.end(); ++it)
    //{
    //    const NPC_DATA* pcNpcData = CDataManager<NPC_DATA>::GetInstance().GetData(it->first);
    //    if (pcNpcData)
    //    {
    //        INF("%5d\t%5d\t%5d", it->first, pcNpcData->nAIID, it->second);
    //    }
    //    else
    //    {
    //        INF("%5d\tunknown npc\t%5d", it->first, it->second);
    //    }
    //}

    return TRUE;
}

BOOL CSceneFileMgr::UnInit(void)
{
    INT32 nRetCode = 0;

    for (MAP_ID_2_DATA::iterator it = m_mapScene.begin();
        it != m_mapScene.end(); ++it)
    {
        CSceneStaticData* pStaticData = it->second;

        nRetCode = pStaticData->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        SAFE_DELETE(pStaticData);
    }
    m_mapScene.clear();

    return TRUE;
}

CSceneFileMgr& CSceneFileMgr::GetInstance(void)
{
    return ms_Instance;
}

CSceneStaticData* CSceneFileMgr::GetStaticData(UINT32 dwSceneTempID)
{
    MAP_ID_2_DATA::iterator it;

    it = m_mapScene.find(dwSceneTempID);
    PROCESS_ERROR(it != m_mapScene.end());

    return it->second;
Exit0:
    return NULL;
}

const TELEPORT_PLATFORM* CSceneFileMgr::GetTeleportPlatform(UINT32 dwTeleportPlatformsID)
{
    LOG_PROCESS_ERROR(dwTeleportPlatformsID < MAX_TELEPORT_PLATFORM_COUNT);
    LOG_PROCESS_ERROR(m_lstTeleportPlatforms[dwTeleportPlatformsID].ID == dwTeleportPlatformsID);

    return &m_lstTeleportPlatforms[dwTeleportPlatformsID];
Exit0:
    return NULL;
}

const NPC_POSITION* CSceneFileMgr::GetNpcPosition(INT32 nTemplateID) const
{
    NPC_POS_MAP::const_iterator it = m_NpcPosMap.find(nTemplateID);
    if (it == m_NpcPosMap.end())
        return NULL;
    else
        return &it->second;
}

// Add extra NPC spawns / blocks
void CSceneFileMgr::Init_AdditionalProcessNpcs()
{
    // Add extra NPC spawns
    {
        m_vecAddSpawnNpcs.clear();

        // Example code
        {
            //INT32 nMapID_1 = 142;
            //ADD_NPC_SPAWN_POINT Npc_001;
            //Npc_001.nID = 100;
            //Npc_001.nTemplateID = 5301;
            //Npc_001.nX = 153944064;  // 153944064, 106627072, 4703939
            //Npc_001.nY = 106627072;
            //Npc_001.nZ = 4703939;
            //Npc_001.nDirection = 4015;
            //Npc_001.nRange = 3932160;
            //Npc_001.nLevelOffset = 5;
            //Npc_001.nQuality = 10;
            //Npc_001.nMinRespawnTime = 4800;
            //Npc_001.nMaxRespawnTime = 4800;
            //Npc_001.nCurrentHp = 0;
            //Npc_001.nCurrentMp = 0;
            //Npc_001.bCanDropItem = 1;
            //Npc_001.bAutoSpawn = 1;
            //Npc_001.bAutoRespawn = 1;
            //Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;  // Group IDs
            //
            //std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;
            //vecMap_1.push_back(Npc_001);
            //m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));
        }

        // 146 Woma Temple1 - 3150,3151
        {
            INT32 nMapID_1 = 146;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 3150;
            Npc_001.nX = 136773632;
            Npc_001.nY = 75825152;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3151;
            Npc_002.nX = 149880832;
            Npc_002.nY = 62586880;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 155 Woma Temple2 - 3150,3151
        {
            INT32 nMapID_1 = 155;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 3150;
            Npc_001.nX = 101777408;
            Npc_001.nY = 64290816;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3151;
            Npc_002.nX = 148963328;
            Npc_002.nY = 73465856;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 148 Death Canyon1 - 3220,3214
        {
            INT32 nMapID_1 = 148;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 3220;
            Npc_001.nX = 167575552;
            Npc_001.nY = 63635456;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3214;
            Npc_002.nX = 152240128;
            Npc_002.nY = 64946176;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 201 Death Canyon2 - 3220,3214
        {
            INT32 nMapID_1 = 201;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 3220;
            Npc_001.nX = 100597760;
            Npc_001.nY = 65077248;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3214;
            Npc_002.nX = 92209152;
            Npc_002.nY = 75694080;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 157 Death Canyon3 - 3217,3213
        {
            INT32 nMapID_1 = 157;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 3217;
            Npc_001.nX = 170852352;
            Npc_001.nY = 66387968;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3213;
            Npc_002.nX = 159973376;
            Npc_002.nY = 70713344;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 158 Death Canyon5 - 3217,3213
        {
            INT32 nMapID_1 = 158;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 3217;
            Npc_001.nX = 174915584;
            Npc_001.nY = 77266944;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3213;
            Npc_002.nX = 172556288;
            Npc_002.nY = 65339392;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 203 Death Canyon6 - 1020
        {
            INT32 nMapID_1 = 203;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1020;
            Npc_001.nX = 109641728;
            Npc_001.nY = 65077248;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1020;
            Npc_002.nX = 94961664;
            Npc_002.nY = 67436544;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 149 Stone Tomb1 - 1045,1046
        {
            INT32 nMapID_1 = 149;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1045;
            Npc_001.nX = 121700352;
            Npc_001.nY = 68091904;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1046;
            Npc_002.nX = 121438208;
            Npc_002.nY = 56688640;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 159 Stone Tomb2 - 1045,1046
        {
            INT32 nMapID_1 = 159;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1045;
            Npc_001.nX = 127729664;
            Npc_001.nY = 62062592;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1046;
            Npc_002.nX = 135856128;
            Npc_002.nY = 42795008;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 160 Stone Tomb3 - 1048,1049
        {
            INT32 nMapID_1 = 160;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1048;
            Npc_001.nX = 150405120;
            Npc_001.nY = 60882944;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1049;
            Npc_002.nX = 121962496;
            Npc_002.nY = 68091904;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 161 Stone Tomb4 - 1048,1049
        {
            INT32 nMapID_1 = 161;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1048;
            Npc_001.nX = 139264000;
            Npc_001.nY = 50659328;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1049;
            Npc_002.nX = 117506048;
            Npc_002.nY = 64552960;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 162 Stone Tomb5 - 1047,1050
        {
            INT32 nMapID_1 = 162;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1047;
            Npc_001.nX = 121438208;
            Npc_001.nY = 47382528;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1050;
            Npc_002.nX = 148439040;
            Npc_002.nY = 70057984;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 164 Hongmo Altar - 1047,1050
        {
            INT32 nMapID_1 = 164;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1047;
            Npc_001.nX = 145686528;
            Npc_001.nY = 36765696;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1050;
            Npc_002.nX = 133103616;
            Npc_002.nY = 48300032;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 180 Zuma Temple1 - 1086,1087
        {
            INT32 nMapID_1 = 180;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1086;
            Npc_001.nX = 147783680;
            Npc_001.nY = 29818880;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1087;
            Npc_002.nX = 122355712;
            Npc_002.nY = 42270720;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 167 Zuma Temple2 - 1086,1087
        {
            INT32 nMapID_1 = 167;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1086;
            Npc_001.nX = 147914752;
            Npc_001.nY = 30081024;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1087;
            Npc_002.nX = 122355712;
            Npc_002.nY = 42270720;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 168 Zuma Temple3 - 1090,3300
        {
            INT32 nMapID_1 = 168;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1090;
            Npc_001.nX = 134807552;
            Npc_001.nY = 53673984;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3300;
            Npc_002.nX = 125894656;
            Npc_002.nY = 30605312;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 169 Zuma Temple4 - 1090,3300
        {
            INT32 nMapID_1 = 169;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1090;
            Npc_001.nX = 133758976;
            Npc_001.nY = 53805056;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3300;
            Npc_002.nX = 125894656;
            Npc_002.nY = 30605312;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 170 Zuma Temple5 - 3301,3302
        {
            INT32 nMapID_1 = 170;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 3301;
            Npc_001.nX = 122880000;
            Npc_001.nY = 26279936;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 3302;
            Npc_002.nX = 132055040;
            Npc_002.nY = 42663936;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 172 Zuma Hall - 3300
        {
            INT32 nMapID_1 = 172;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 3300;
            Npc_001.nX = 144244736;
            Npc_001.nY = 23658496;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 188 Chiyue Canyon North Entrance - 1103,1104
        {
            INT32 nMapID_1 = 188;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1103;
            Npc_001.nX = 143851520;
            Npc_001.nY = 32571392;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1104;
            Npc_002.nX = 121700352;
            Npc_002.nY = 32440320;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 189 Chiyue Canyon South Entrance - 1103,1104
        {
            INT32 nMapID_1 = 189;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1103;
            Npc_001.nX = 132579328;
            Npc_001.nY = 25362432;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1104;
            Npc_002.nX = 144375808;
            Npc_002.nY = 25362432;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 190 Chiyue Canyon East Entrance - 1106,1107
        {
            INT32 nMapID_1 = 190;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1106;
            Npc_001.nX = 136642560;
            Npc_001.nY = 35061760;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1107;
            Npc_002.nX = 145031168;
            Npc_002.nY = 28377088;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 191 Chiyue Canyon Square - 1106,1107
        {
            INT32 nMapID_1 = 191;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1106;
            Npc_001.nX = 130613248;
            Npc_001.nY = 27328512;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1107;
            Npc_002.nX = 144375808;
            Npc_002.nY = 45416448;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 193 Left Corridor - 1105,1108
        {
            INT32 nMapID_1 = 193;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1105;
            Npc_001.nX = 134414336;
            Npc_001.nY = 19857408;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1108;
            Npc_002.nX = 118947840;
            Npc_002.nY = 42008576;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 194 Right Corridor - 1105,1108
        {
            INT32 nMapID_1 = 194;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1105;
            Npc_001.nX = 130613248;
            Npc_001.nY = 41615360;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1108;
            Npc_002.nX = 144113664;
            Npc_002.nY = 29949952;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 195 Land of Choice - 1097
        {
            INT32 nMapID_1 = 195;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1097;
            Npc_001.nX = 127467520;
            Npc_001.nY = 23527424;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1097;
            Npc_002.nX = 136511488;
            Npc_002.nY = 47513600;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 196 Valley Secret Passage1 - 1097
        {
            INT32 nMapID_1 = 196;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1097;
            Npc_001.nX = 134938624;
            Npc_001.nY = 24182784;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 197 Valley Secret Passage2 - 1097
        {
            INT32 nMapID_1 = 197;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1097;
            Npc_001.nX = 135593984;
            Npc_001.nY = 24182784;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 187 Jungle Maze - 1105,1108
        {
            INT32 nMapID_1 = 187;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 1105;
            Npc_001.nX = 125370368;
            Npc_001.nY = 99680256;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 1108;
            Npc_002.nX = 159318016;
            Npc_002.nY = 74907648;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 223 Nether Sea1 - 2175
        {
            INT32 nMapID_1 = 223;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 2175;
            Npc_001.nX = 135856128;
            Npc_001.nY = 49741824;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 2175;
            Npc_002.nX = 107937792;
            Npc_002.nY = 46465024;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 224 Nether Sea2 - 2106, 2115
        {
            INT32 nMapID_1 = 224;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 2106;
            Npc_001.nX = 139264000;
            Npc_001.nY = 69402624;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 2115;
            Npc_002.nX = 133103616;
            Npc_002.nY = 74121216;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 225 Nether Sea3 - 2117, 2118
        {
            INT32 nMapID_1 = 225;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 2117;
            Npc_001.nX = 124452864;
            Npc_001.nY = 54460416;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 2118;
            Npc_002.nX = 132186112;
            Npc_002.nY = 76087296;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }

        // 226 Nether Sea4 - 2117, 2118
        {
            INT32 nMapID_1 = 226;
            std::vector<ADD_NPC_SPAWN_POINT> vecMap_1;

            ADD_NPC_SPAWN_POINT Npc_001;
            Npc_001.nID = 100;
            Npc_001.nTemplateID = 2117;
            Npc_001.nX = 153681920;
            Npc_001.nY = 41222144;
            Npc_001.nZ = 4703939;
            Npc_001.nDirection = 4015;
            Npc_001.nRange = 3932160;
            Npc_001.nLevelOffset = 5;
            Npc_001.nQuality = 10;
            Npc_001.nMinRespawnTime = 57600;
            Npc_001.nMaxRespawnTime = 57600;
            Npc_001.nCurrentHp = 0;
            Npc_001.nCurrentMp = 0;
            Npc_001.bCanDropItem = 1;
            Npc_001.bAutoSpawn = 1;
            Npc_001.bAutoRespawn = 1;
            Npc_001.nSmallGroupID = Npc_001.nMediumGroupID = Npc_001.nLargeGroupID = -1;

            ADD_NPC_SPAWN_POINT Npc_002;
            Npc_002.nID = 101;
            Npc_002.nTemplateID = 2118;
            Npc_002.nX = 150536192;
            Npc_002.nY = 59572224;
            Npc_002.nZ = 4703939;
            Npc_002.nDirection = 4015;
            Npc_002.nRange = 3932160;
            Npc_002.nLevelOffset = 5;
            Npc_002.nQuality = 10;
            Npc_002.nMinRespawnTime = 57600;
            Npc_002.nMaxRespawnTime = 57600;
            Npc_002.nCurrentHp = 0;
            Npc_002.nCurrentMp = 0;
            Npc_002.bCanDropItem = 1;
            Npc_002.bAutoSpawn = 1;
            Npc_002.bAutoRespawn = 1;
            Npc_002.nSmallGroupID = Npc_002.nMediumGroupID = Npc_002.nLargeGroupID = -1;

            vecMap_1.push_back(Npc_001);
            vecMap_1.push_back(Npc_002);
            m_vecAddSpawnNpcs.insert(std::make_pair(nMapID_1, vecMap_1));

            //WRN("========================================");
            //WRN("");

            INT32 nX1 = Npc_001.nX / POINT_PER_CM;
            INT32 nY1 = Npc_001.nY / POINT_PER_CM;
            INT32 nZ1 = Npc_001.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX1, nY1, nZ1);

            INT32 nX2 = Npc_002.nX / POINT_PER_CM;
            INT32 nY2 = Npc_002.nY / POINT_PER_CM;
            INT32 nZ2 = Npc_002.nZ / POINT_PER_CM;
            ConvertCoordN2O(nX2, nY2, nZ2);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX1, nY1, nZ1);
            //WRN("DebugElite - MapID %d, Point [%d, %d, %d]", nMapID_1, nX2, nY2, nZ2);

            //WRN("");
            //WRN("========================================");
            //WRN("");
        }
    }

    // Block additional NPCs
    {
        m_mapBlockNpc[143] = { 6128, 6101 };  // 143-Bichon Province       => 6128-Wandering Swordsman, 6101-Royal Treasury Teleporter
        m_mapBlockNpc[147] = { 6128, 6101 };  // 147-Mengzhong Province       => 6128-Wandering Swordsman, 6101-Royal Treasury Teleporter
        m_mapBlockNpc[207] = { 3118 };  // 207-Corpse King Hall       => 3118-Lightning Ghost Corpse
        m_mapBlockNpc[156] = {  953 };  // 156-Woma Temple floor 3  => 953-Poison Needle Demon Warrior
        m_mapBlockNpc[203] = { 3227 };  // 203-Death Canyon floor 6  => 3227-Soul-Stealing Jumping Bee
        m_mapBlockNpc[164] = { 3248 };  //  35-Hongmo Altar     => 3248-White Boar Thug
        m_mapBlockNpc[172] = { 1078 };  // 172-Zuma Hall     => 1078-Evil Arrow Archer
        m_mapBlockNpc[173] = { 1078 };  // 173-Zuma Leader’s House => 1078-Evil Arrow Archer
        m_mapBlockNpc[197] = { 1102 };  // 197-Valley Secret Passage     => 1102-Demon-Hunting Spider01
        m_mapBlockNpc[244] = { 1142 };  // 244-Demon Dragon Blood Domain     => 1142-Demon Dragon Ghost Archer01
        m_mapBlockNpc[66]  = {  329 };  //  66-Bull Demon Temple Hall => 329-Dark Priest
        m_mapBlockNpc[55]  = {  306 };  //  55-Bone Demon Cave, floor 5   => 306-Skull-Crushing Archer
    }

}
