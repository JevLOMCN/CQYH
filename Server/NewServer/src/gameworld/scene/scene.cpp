#include "stdafx.h"
#include "scene.h"

#include "guild_config.h"

#include "game_world.h"
#include "region.h"
#include "grid_def.h"
#include "role.h"
#include "npc.h"
#include "hero.h"
#include "doodad.h"
#include "message_handler.h"
#include "scene_file_mgr.h"
#include "kismet/kismet_node.h"
#include "zone_def.h"
#include "script_mgr.h"
#include "score_def.h"
#include "statistic.h"
#include "spell_config.h"
#include "guild_mgr.h"
#include "statis_info.h"
#include "guild_building_data.h"
#include "npc_data.h"
#include "doodad_data.h"
#include "sabac_mgr.h"

#define MAX_NPC_COUNT_PER_SCENE     (1024 * 4)

//#define PERFORMANCE_OBSTACLED
#ifdef PERFORMANCE_OBSTACLED 
static UINT64   gs_qwTotalObstacleCount = 0;
static UINT32	gs_dwTempObstacleCount = 0;
static UINT64	gs_qwLastPrintObstacleTickCount = 0;
#endif

#define LOC_DIS	(GRID_WIDTH * 1)

const INT32 s_nLocationOffsetTable[][2] =
{
    { 0 * LOC_DIS, 0 * LOC_DIS },
    { 0 * LOC_DIS, 1 * LOC_DIS }, { 1 * LOC_DIS, 0 * LOC_DIS }, { 0 * LOC_DIS, -1 * LOC_DIS }, { -1 * LOC_DIS, 0 * LOC_DIS },
    { 1 * LOC_DIS, 1 * LOC_DIS }, { 1 * LOC_DIS, -1 * LOC_DIS }, { -1 * LOC_DIS, -1 * LOC_DIS }, { -1 * LOC_DIS, 1 * LOC_DIS },
    { 0 * LOC_DIS, 2 * LOC_DIS }, { 2 * LOC_DIS, 0 * LOC_DIS }, { 0 * LOC_DIS, -2 * LOC_DIS }, { -2 * LOC_DIS, 0 * LOC_DIS },
    { -1 * LOC_DIS, 2 * LOC_DIS }, { 2 * LOC_DIS, 1 * LOC_DIS }, { 1 * LOC_DIS, -2 * LOC_DIS }, { -2 * LOC_DIS, -1 * LOC_DIS },
    { 1 * LOC_DIS, 2 * LOC_DIS }, { 2 * LOC_DIS, -1 * LOC_DIS }, { -1 * LOC_DIS, -2 * LOC_DIS }, { -2 * LOC_DIS, 1 * LOC_DIS },
    { 2 * LOC_DIS, 2 * LOC_DIS }, { 2 * LOC_DIS, -2 * LOC_DIS }, { -2 * LOC_DIS, -2 * LOC_DIS }, { -2 * LOC_DIS, 2 * LOC_DIS },
    { 0 * LOC_DIS, 3 * LOC_DIS }, { 3 * LOC_DIS, 0 * LOC_DIS }, { 0 * LOC_DIS, -3 * LOC_DIS }, { -3 * LOC_DIS, 0 * LOC_DIS },
    { -1 * LOC_DIS, 3 * LOC_DIS }, { 3 * LOC_DIS, 1 * LOC_DIS }, { 1 * LOC_DIS, -3 * LOC_DIS }, { -3 * LOC_DIS, -1 * LOC_DIS },
    { 1 * LOC_DIS, 3 * LOC_DIS }, { 3 * LOC_DIS, -1 * LOC_DIS }, { -1 * LOC_DIS, -3 * LOC_DIS }, { -3 * LOC_DIS, 1 * LOC_DIS },
    { -2 * LOC_DIS, 3 * LOC_DIS }, { 3 * LOC_DIS, 2 * LOC_DIS }, { 2 * LOC_DIS, -3 * LOC_DIS }, { -3 * LOC_DIS, -2 * LOC_DIS },
    { 2 * LOC_DIS, 3 * LOC_DIS }, { 3 * LOC_DIS, -2 * LOC_DIS }, { -2 * LOC_DIS, -3 * LOC_DIS }, { -3 * LOC_DIS, 2 * LOC_DIS },
    { 3 * LOC_DIS, 3 * LOC_DIS }, { 3 * LOC_DIS, -3 * LOC_DIS }, { -3 * LOC_DIS, -3 * LOC_DIS }, { -3 * LOC_DIS, 3 * LOC_DIS },
    { 0 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, 0 * LOC_DIS }, { 0 * LOC_DIS, -4 * LOC_DIS }, { -4 * LOC_DIS, 0 * LOC_DIS },
    { -1 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, 1 * LOC_DIS }, { 1 * LOC_DIS, -4 * LOC_DIS }, { -4 * LOC_DIS, -1 * LOC_DIS },
    { 1 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, -1 * LOC_DIS }, { -1 * LOC_DIS, -4 * LOC_DIS }, { -4 * LOC_DIS, 1 * LOC_DIS },
    { -2 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, 2 * LOC_DIS }, { 2 * LOC_DIS, -4 * LOC_DIS }, { -4 * LOC_DIS, -2 * LOC_DIS },
    { 2 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, -2 * LOC_DIS }, { -2 * LOC_DIS, -4 * LOC_DIS }, { -4 * LOC_DIS, 2 * LOC_DIS },
    { -3 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, 3 * LOC_DIS }, { 3 * LOC_DIS, -4 * LOC_DIS }, { -4 * LOC_DIS, -3 * LOC_DIS },
    { 3 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, -3 * LOC_DIS }, { -3 * LOC_DIS, -4 * LOC_DIS }, { -4 * LOC_DIS, 3 * LOC_DIS },
    { -4 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, 4 * LOC_DIS }, { 4 * LOC_DIS, -4 * LOC_DIS }, { -4 * LOC_DIS, -4 * LOC_DIS },
};

CScene::CScene(void)
{
    m_nSceneTempID = m_nSceneID = 0;
    m_nWeather = swtInvalid;
    m_nWeatherEndFrame = 0;
    m_nWeatherRefreshFrame = 0;
    memset(m_poRegionList, NULL, sizeof(m_poRegionList));
    m_nRegionWidth = m_nRegionHeight = 0;
    m_pszZoneInstanceData = NULL;
    m_ppZoneInstanceData = NULL;
    m_nState = edstInvalid;
    m_nStartFrame = 0;
    m_lstRespawnNpc.Clear();
    m_lstRespawnDoodad.Clear();
    m_lstRole.Clear();
    m_nRoleCount = 0;
    m_nNpcCount = 0;
    m_pcSceneData = NULL;
    m_poStaticData = NULL;
    m_qwDoorTable = 0xFFFFFFFFFFFFFFFF;
    memset(m_nVarList, 0, sizeof(m_nVarList));
    memset(m_lstpoBattleTeam, 0, sizeof(m_lstpoBattleTeam));
    memset(m_SingleNpcObjID, 0, sizeof(m_SingleNpcObjID));
    m_CombatStatList.Clear();
    m_lstBTCtx.Clear();
    memset(&m_TraverseRoleOnCalculate, 0, sizeof(m_TraverseRoleOnCalculate));
    memset(&m_TraverseRoleOnSceneClose, 0, sizeof(m_TraverseRoleOnSceneClose));
    m_nServerGroupID = 0;
    m_nZoneWidth = m_nZoneHeight = 0;
    m_nTotalFrame = 0;
    m_nClosingFrame = 0;
    m_bReward = FALSE;
    m_bDLRecordRole = FALSE;
    m_bFirstBloodFlag = FALSE;
    memset(m_MediumGroupList, 0, sizeof(m_MediumGroupList));
    memset(m_DoodadGroupList, 0, sizeof(m_DoodadGroupList));
    m_nDLSceneID = 0;
    m_bSabacStarted = FALSE;
    m_bSabacEventTriggered = FALSE;
}

CScene::~CScene(void)
{
    UnInit();
}

BOOL CScene::Init(INT32 nServerGroupID, INT32 nSceneTempID, INT32 nSceneID)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    char* pszZoneInstanceData = NULL;
    UINT64 qwStartTick = GetTickCountEx();

    m_poStaticData = CSceneFileMgr::GetInstance().GetStaticData(nSceneTempID);
    LOG_PROCESS_ERROR(m_poStaticData);

    m_pcSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTempID);
    LOG_PROCESS_ERROR(m_pcSceneData != NULL);

    m_nServerGroupID = nServerGroupID;
    m_nSceneTempID = nSceneTempID;
    m_nSceneID = nSceneID;
    m_nState = edstRuning;
    m_nWeather = swtNormal;
    m_nWeatherEndFrame = INT_MAX;
    m_nWeatherRefreshFrame = 0;
    m_bSabacStarted = FALSE;
    m_bSabacEventTriggered = FALSE;

    m_nTotalFrame = 0;
    m_nClosingFrame = 0;
    memset(m_lstpoBattleTeam, 0, sizeof(m_lstpoBattleTeam));
    m_bFirstBloodFlag = 0;

    m_nDLSceneID = (nSceneTempID << 4) | nSceneID;

    memset(m_poRegionList, NULL, sizeof(m_poRegionList));

    m_nRegionWidth = m_poStaticData->GetMaxZoneX() / (REGION_WIDTH / ZONE_WIDTH) + 1;
    m_nRegionHeight = m_poStaticData->GetMaxZoneY() / (REGION_WIDTH / ZONE_WIDTH) + 1;

    for (INT32 nRegionX = 0; nRegionX < m_nRegionWidth; nRegionX++)
    {
        for (INT32 nRegionY = 0; nRegionY < m_nRegionHeight; nRegionY++)
        {
            m_poRegionList[nRegionX][nRegionY] = CGameWorld::GetInstance().NewRegion();
            LOG_PROCESS_ERROR(m_poRegionList[nRegionX][nRegionY]);
        }
    }

    for (INT32 nRegionX = 0; nRegionX < m_nRegionWidth; nRegionX++)
    {
        for (INT32 nRegionY = 0; nRegionY < m_nRegionHeight; nRegionY++)
        {
            nRetCode = m_poRegionList[nRegionX][nRegionY]->Init(this, nRegionX, nRegionY);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    m_nZoneWidth = m_poStaticData->GetMaxZoneX() + 1;
    m_nZoneHeight = m_poStaticData->GetMaxZoneY() + 1;

    LOG_PROCESS_ERROR(m_ppZoneInstanceData == NULL);
    m_ppZoneInstanceData = new ZONE_INSTANCE_DATA * [m_nZoneWidth * m_nZoneHeight];
    LOG_PROCESS_ERROR(m_ppZoneInstanceData);

    LOG_PROCESS_ERROR(m_pszZoneInstanceData == NULL);
    m_pszZoneInstanceData = new char[m_poStaticData->GetInstanceZoneTotalSize()];
    LOG_PROCESS_ERROR(m_pszZoneInstanceData);

    memset(m_pszZoneInstanceData, 0, m_poStaticData->GetInstanceZoneTotalSize());
    pszZoneInstanceData = m_pszZoneInstanceData;

    for (INT32 nZoneX = 0; nZoneX < m_nZoneWidth; nZoneX++)
    {
        for (INT32 nZoneY = 0; nZoneY < m_nZoneHeight; nZoneY++)
        {
            const ZONE_DATA* pcZoneData = m_poStaticData->GetZoneData(nZoneX, nZoneY);
            if (pcZoneData)
            {
                m_ppZoneInstanceData[nZoneX + nZoneY * m_nZoneWidth] = (ZONE_INSTANCE_DATA*)pszZoneInstanceData;
                pszZoneInstanceData += sizeof(ZONE_INSTANCE_DATA) + (pcZoneData->nGridCount + 1) * sizeof(GRID_INSTANCE_DATA);
            }
        }
    }

    if (m_pcSceneData->nType == estBattleField)
    {
        for (INT32 i = 0; i < MAX_FACTION_COUNT; ++i)
        {
            m_lstpoBattleTeam[i] = CGameWorld::GetInstance().NewTeam(0);
            LOG_PROCESS_ERROR(m_lstpoBattleTeam[i]);

            nRetCode = m_lstpoBattleTeam[i]->Init();
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    m_nStartFrame = CGameWorld::GetInstance().GetGameFrame();
    SetVariable(esvtSceneLostFaction, 0);

    m_qwDoorTable = 0xFFFFFFFFFFFFFFFF;
    memset(m_nVarList, 0, sizeof(m_nVarList));
    for (INT32 nIndex = 0; nIndex < MAX_NPC_LARGE_GROUP_COUNT; nIndex++)
    {
        m_LargeGroupList[nIndex].nDieCount = -1;
    }
    memset(m_MediumGroupList, 0, sizeof(m_MediumGroupList));
    memset(m_DoodadGroupList, 0, sizeof(m_DoodadGroupList));
    memset(m_byVolumeTriggerCount, 0, sizeof(m_byVolumeTriggerCount));

    memset(m_SingleNpcObjID, 0, sizeof(m_SingleNpcObjID));
    m_nNpcCount = 0;

#ifndef _ROBOT
    nRetCode = _LoadNpc();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _LoadDoodad();
    LOG_CHECK_ERROR(nRetCode);
#endif

    m_lstRespawnNpc.Clear();
    m_lstRespawnDoodad.Clear();
    m_lstRole.Clear();
    m_CombatStatList.Clear();
    m_nRoleCount = 0;
    m_lstBTCtx.Clear();

    if (m_poStaticData->GetBT(evtSceneInit))
    {
        nRetCode = RunBT(evtSceneInit, 0, m_poStaticData->GetBT(evtSceneInit));
        LOG_CHECK_ERROR(nRetCode);
    }

    INF("initialized scene %d-%X (%d npc(%.2fMB) %d ms %.2f MB)", nSceneTempID, nSceneID, m_nNpcCount,
        (sizeof(CNpc) + sizeof(CAIProcessor)) * m_nNpcCount / (1024.0f * 1024.0f),
        (INT32)(GetTickCountEx() - qwStartTick),
        m_poStaticData->GetInstanceZoneTotalSize() / (1024.0f * 1024.0f));

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

// Load NPCs
BOOL CScene::_LoadNpc(void)
{
    INT32 nRetCode = 0;
    INT32 nNpcTemplateID = INVALID_OBJECT_ID;
    CNpc* poNpc = NULL;

    LOG_PROCESS_ERROR(m_poStaticData);

    for (INT32 nSingleNpcID = 0; nSingleNpcID < m_poStaticData->GetSingleNpcCount(); nSingleNpcID++)
    {
        const NPC_SPAWN_POINT* pSpawnPoint = m_poStaticData->GetSingleNpc(nSingleNpcID);
        if (pSpawnPoint == NULL || !pSpawnPoint->bAutoSpawn)
            continue;

        poNpc = SpawnNpcPoint(pSpawnPoint);
        LOG_CHECK_ERROR(poNpc);

        if (poNpc)
            m_SingleNpcObjID[nSingleNpcID] = poNpc->GetObjID();
    }

    for (INT32 nLargeGroupID = 0; nLargeGroupID < m_poStaticData->GetNpcLargeGroupCount(); nLargeGroupID++)
    {
        if (m_poStaticData->GetNpcLargeGroup(nLargeGroupID))
        {
            nRetCode = SpawnNpcLargeGroup(nLargeGroupID);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CScene::_GetRandDoodadPoint(const DOODAD_SPAWN_GROUP* pcSpawnGroup)
{
    INT32 nPointID = 0;
    INT32 nRandPoint = 0;
    INT32 nRemainPointCount = 0;

    LOG_PROCESS_ERROR(pcSpawnGroup);
    LOG_PROCESS_ERROR(m_poStaticData);

    nRemainPointCount = pcSpawnGroup->nMaxConcurrentPoint - m_DoodadGroupList[pcSpawnGroup->nGroupID].nSpawnCount;
    LOG_PROCESS_ERROR(nRemainPointCount > 0);
    nRandPoint = g_Random(nRemainPointCount);

    for (; nPointID < pcSpawnGroup->nPointCount; nPointID++)
    {
        if (!_IsDoodadPointSpawned(pcSpawnGroup->nGroupID, nPointID))
        {
            if (nRandPoint == 0)
                break;
            nRandPoint--;
        }
    }
    LOG_PROCESS_ERROR(nPointID < pcSpawnGroup->nPointCount);

    return nPointID;
Exit0:

    return -1;
}

BOOL CScene::_IsDoodadPointSpawned(INT32 nGroupID, INT32 nPointID)
{
    LOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_DOODAD_SPAWN_GROUP_COUNT);
    LOG_PROCESS_ERROR(nPointID >= 0 && nPointID < MAX_DOODAD_SPAWN_POINT_COUNT);

    return (m_DoodadGroupList[nGroupID].bSpawnedList[nPointID / 32] & (1 << (nPointID % 32))) > 0;
Exit0:
    return TRUE;
}

void CScene::_SetDoodadPointSpawned(INT32 nGroupID, INT32 nPointID, BOOL bSpawned)
{
    LOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_DOODAD_SPAWN_GROUP_COUNT);
    LOG_PROCESS_ERROR(nPointID >= 0 && nPointID < MAX_DOODAD_SPAWN_POINT_COUNT);

    if (bSpawned)
    {
        m_DoodadGroupList[nGroupID].bSpawnedList[nPointID / 32] |= (1 << (nPointID % 32));
        m_DoodadGroupList[nGroupID].nSpawnCount++;
    }
    else
    {
        m_DoodadGroupList[nGroupID].bSpawnedList[nPointID / 32] &= ~(1 << (nPointID % 32));
        m_DoodadGroupList[nGroupID].nSpawnCount--;
    }
Exit0:
    return;
}

INT32 CScene::_SideConvertToFaction()
{
    //according to Scene to Convert;
    INT32 nResult = 0;
    switch (m_pcSceneData->nType)
    {
    case estStatic:
    case estDungeon:
    case estLine:
    case estGuild:
    case estBirth:
    case estGuildDungeon:
    case estUnique:
    case estSoloDungeon:
        nResult = 0;
        break;
    case estBattleField:
    case estArena:
    {
        INT32   nRandValue = rand() % 2;
        nResult = nRandValue + 1;
    }
    break;
    }

    return nResult;
}

BOOL CScene::_LoadDoodad(void)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(m_poStaticData);

    for (INT32 nGroupID = 0; nGroupID < m_poStaticData->GetDoodadSpawnGroupCount(); nGroupID++)
    {
        const DOODAD_SPAWN_GROUP* pGroup = m_poStaticData->GetDoodadSpawnGroup(nGroupID);
        LOG_PROCESS_ERROR(pGroup);

        if (pGroup->IsAutoSpawn)
        {
            nRetCode = SpawnDoodadGroup(nGroupID);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CScene::CalculateDungeonScore()
{
    INT32 nRetCode = 0;
    INT32 nDungeonID = 0;
    LOG_PROCESS_ERROR(m_nState == edstClosing);
    LOG_PROCESS_ERROR(m_pcSceneData->nType == estDungeon || m_pcSceneData->nType == estSoloDungeon);

#ifndef _ROBOT
    m_TraverseRoleOnCalculate.m_nTotalFrame = m_nTotalFrame;
    m_TraverseRoleOnCalculate.m_pDungeonData = CDataManager<DUNGEON_DATA>::GetInstance().GetData(m_nSceneTempID);
    LOG_PROCESS_ERROR(m_TraverseRoleOnCalculate.m_pDungeonData);

    nRetCode = TraverseRole(m_TraverseRoleOnCalculate);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoSyncDungeonScore(this);
    LOG_PROCESS_ERROR(nRetCode);

    m_bReward = TRUE;
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::IsCanReward()
{
    return m_nState == edstClosing && m_bReward;
}

BOOL CScene::CTraverseRoleOnSceneDestroy::operator ()(CRole* poRole)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poRole->GetScene());

    if (poRole->GetStaticReturnPos().nSceneTempID != poRole->GetScene()->GetSceneTempID())
        nRetCode = poRole->JumpToReturnPos();

    if (!nRetCode && poRole->GetRevivePos().nSceneTempID != poRole->GetScene()->GetSceneTempID())
        nRetCode = poRole->JumpToRevivePos();

    if (!nRetCode)
    {
        nRetCode = CGameWorld::GetInstance().JumpTo(poRole, g_GWConfig.nErrorReturnSceneTempID, 0,
            g_GWConfig.ErrorReturnPos.x, g_GWConfig.ErrorReturnPos.y, g_GWConfig.ErrorReturnPos.z,
            poRole->GetFaceDirection());
        LOG_CHECK_ERROR(nRetCode);
    }

Exit0:
    return TRUE;
}

BOOL CScene::CTraverseRoleSceneClose::operator ()(CRole* poRole)
{
    INT32 nRetCode = 0;
    CAura* pAura = NULL;
    PROCESS_ERROR(poRole);

    nRetCode = poRole->FinishScene();
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
    if (bRoleAura)
    {
        pAura = CSpellConfig::GetInstance().FindAuraTemplate(g_GWConfig.nSceneFinishBuffID);
        if (pAura)
        {
            CAuraInstance* pAuraInstance = poRole->AppendAura(pAura, poRole, poRole->GetObjID(), NULL, GAME_FPS * 300, 0, 0, 0);
            LOG_CHECK_ERROR(pAuraInstance);
        }
    }
#endif

Exit0:
    return TRUE;
}

BOOL CScene::CTraverseNpcSceneClose::operator()(CNpc* poNpc)
{
    if (poNpc->GetPetType() == 0)
    {
        poNpc->SetLastCasterID(0);
        poNpc->SetDropOwnerID(0);
        poNpc->SetGenerateDrop(FALSE);
        poNpc->Die(0);
    }

Exit0:
    return TRUE;
}

BOOL CScene::CTraverseRoleCalculate::operator()(CRole* poRole)
{
    CTeamInGS* pTeam = nullptr;
    INT32 nRetCode = 0;
    CScene* poScene = NULL;

    INT32 nTimeScore = edsC;
    INT32 nTotalAttackValue = 0;
    INT32 nAttackScore = edsC;
    INT32 nTotalGoalValue = 0;
    INT32 nStandardTotalGoalValue = 0;
    UINT32 dwGoalResult = 0;
    INT32 nGoalScore = edsC;
    INT32 nTotalValue = 0;
    INT32 nTotalScore = edsC;

    INT32 nNormalExp = 0;
    INT32 nTeamExp = 0;
    INT32 nGuildExp = 0;
    INT32 nMasterExp = 0;
    INT32 nVipExp = 0;
    INT32 nFirstPassExp = 0;
    INT32 nActivityExp = 0;
    INT32 nTotalExp = 0;

    const static INT32 s_ScoreFactor[edsTotal] =
    {
        10000,  //edsC,
        11000,  //edsB,
        12000,  //edsA,
        13000,  //edsS,
        14000,  //edsSS,
        15000,  //edsSSS
    };

    const static  INT32 s_MasterFactor = 500;

    LOG_PROCESS_ERROR(poRole);

    poScene = poRole->GetScene();
    LOG_PROCESS_ERROR(poScene);

#ifndef _ROBOT

    for (INT32 i = edsSSS; i >= edsC; --i)
    {
        if (m_nTotalFrame <= (INT64)m_pDungeonData->nStandardTime * g_GWConfig.DungeonScore.nTimeScoreRate[i] / RATE_MAX)
        {
            nTimeScore = i;
            break;
        }
    }

    for (INT32 i = edsSSS; i >= edsC; --i)
    {
        if (poRole->GetCombatStat(escsNpcKillCount) >=
            (INT64)m_pDungeonData->nStandardNpcValue * g_GWConfig.DungeonScore.nAttackScoreRate[i] / RATE_MAX)
        {
            nTotalAttackValue += i;
            break;
        }
    }

    for (INT32 i = edsSSS; i >= edsC; --i)
    {
        if (poRole->GetCombatStat(escsDoodadKillCount) >=
            (INT64)m_pDungeonData->nStandarddDoodadValue * g_GWConfig.DungeonScore.nAttackScoreRate[i] / RATE_MAX)
        {
            nTotalAttackValue += i;
            break;
        }
    }

    for (INT32 i = edsSSS; i >= edsC; --i)
    {
        if (nTotalAttackValue >= g_GWConfig.DungeonScore.nTotalAttackValue[i])
        {
            nAttackScore = i;
            break;
        }
    }

    if (poRole->GetCombatStat(escsBossKillCount) >= m_pDungeonData->nStandardBossValue)
    {
        nTotalGoalValue += g_GWConfig.DungeonScore.nKillBossScore;
        dwGoalResult = (dwGoalResult << 1) | 1;
    }
    else
    {
        dwGoalResult = (dwGoalResult << 1) | 0;
    }
    nStandardTotalGoalValue += g_GWConfig.DungeonScore.nKillBossScore;

    if (poRole->GetCombatStat(escsEliteNpcKillCount) >= m_pDungeonData->nStandardEliteNpcValue)
    {
        nTotalGoalValue += g_GWConfig.DungeonScore.nKillEliteNpcScore;
        dwGoalResult = (dwGoalResult << 1) | 1;
    }
    else
    {
        dwGoalResult = (dwGoalResult << 1) | 0;
    }
    nStandardTotalGoalValue += g_GWConfig.DungeonScore.nKillEliteNpcScore;

    if (m_nTotalFrame <= m_pDungeonData->nStandardSwiftTime)
    {
        nTotalGoalValue += g_GWConfig.DungeonScore.nSwiftTimeScore;
        dwGoalResult = (dwGoalResult << 1) | 1;
    }
    else
    {
        dwGoalResult = (dwGoalResult << 1) | 0;
    }
    nStandardTotalGoalValue += g_GWConfig.DungeonScore.nSwiftTimeScore;

    if (poRole->GetCombatStat(escsDeathCount) == 0)
    {
        nTotalGoalValue += g_GWConfig.DungeonScore.nNeverDieScore;
        dwGoalResult = (dwGoalResult << 1) | 1;
    }
    else
    {
        dwGoalResult = (dwGoalResult << 1) | 0;
    }
    nStandardTotalGoalValue += g_GWConfig.DungeonScore.nNeverDieScore;

    for (INT32 nIndex = 0; nIndex < MAX_CUSTOM_GOAL_PER_DUNGEON; nIndex++)
    {
        if (m_pDungeonData->nCustomGoalVarID[nIndex] != INVALID_VARIABLE_ID &&
            poScene->GetVariable(m_pDungeonData->nCustomGoalVarID[nIndex]) == m_pDungeonData->nCustomGoalVarValue[nIndex])
        {
            nTotalGoalValue += m_pDungeonData->nCustomGoalScore[nIndex];
            dwGoalResult = (dwGoalResult << 1) | 1;
            nStandardTotalGoalValue += m_pDungeonData->nCustomGoalScore[nIndex];
        }
        else
        {
            dwGoalResult = (dwGoalResult << 1) | 0;
        }
    }

    for (INT32 i = edsSSS; i >= edsC; --i)
    {
        if (nStandardTotalGoalValue - nTotalGoalValue <= g_GWConfig.DungeonScore.nGoalScoreDiff[i])
        {
            nGoalScore = i;
            break;
        }
    }

    nTotalValue = nTimeScore + nAttackScore + nGoalScore;
    for (INT32 i = edsSSS; i >= edsC; --i)
    {
        if (nTotalValue >= g_GWConfig.DungeonScore.nTotalScore[i])
        {
            nTotalScore = i;
            break;
        }
    }

    poRole->SetCombatStat(escsTotalScore, nTotalScore);

    nNormalExp = (4LL * s_ScoreFactor[nTotalScore] / (10 * RATE_MAX) + 1) * 10;

    pTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
    if (pTeam && pTeam->GetMemberCount() > 0)
    {
        for (INT32 nMemberIndex = 0; nMemberIndex < pTeam->GetMaxMemberCount(); nMemberIndex++)
        {
            if (pTeam->GetMemberTable(nMemberIndex).dwRoleID != INVALID_OBJECT_ID &&
                pTeam->GetMemberTable(nMemberIndex).dwRoleID != poRole->GetObjID())
            {
                CRole* poMemberRole = CGameWorld::GetInstance().FindValidRole(pTeam->GetMemberTable(nMemberIndex).dwRoleID);
                if (poMemberRole)
                {
                    if (poMemberRole->GetScene() == poScene)
                    {
                        nTeamExp = m_pDungeonData->nTeamExp;
                    }
                    if (poRole->GetGuildID() != INVALID_OBJECT_ID && poRole->GetGuildID() == poMemberRole->GetGuildID())
                    {
                        nGuildExp = m_pDungeonData->nGuildExp;
                    }
                    if (poRole->IsMasterRelationship(poMemberRole))
                    {
                        nMasterExp = nNormalExp * s_MasterFactor / RATE_MAX;
                    }
                }
            }
        }
    }

    nVipExp = m_pDungeonData->nVipExp[poRole->GetVIPLevel()];

    if (poRole->IsFirstPassDungeon(poScene->GetSceneTempID()))
        nFirstPassExp = m_pDungeonData->nFirstPassExp;

    nActivityExp = m_pDungeonData->nActivityExp;

    nTotalExp = nNormalExp + nTeamExp + nGuildExp + nMasterExp + nVipExp + nFirstPassExp + nActivityExp;

    poRole->AddExp(nTotalExp, FALSE, FALSE);
    poRole->UpdateDungeonRecordMask(poScene->GetSceneTempID());
    nRetCode = CCliMessageHandler::GetInstance().DoDungeonFinishData(poRole, m_nTotalFrame / GAME_FPS, nTimeScore,
        poRole->GetCombatStat(escsNpcKillCount), poRole->GetCombatStat(escsDoodadKillCount),
        nAttackScore, dwGoalResult, nGoalScore, nTotalScore, nNormalExp, nTeamExp,
        nGuildExp, nMasterExp, nVipExp, nFirstPassExp, nActivityExp, nTotalExp);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::UnInit(void)
{
    INT32 nRetCode = 0;
    BASE_LIST_NODE* pNode = NULL;
    BASE_LIST_NODE* pNextNode = NULL;

    if (m_nSceneTempID != 0)
        INF("unInitializing scene %d-%d-%d", m_nServerGroupID, m_nSceneTempID, m_nSceneID);

#ifdef _ROBOT
    for (BASE_LIST_NODE* pNode = m_lstRole.Head().pNext;
        pNode != &(m_lstRole.Rear()); pNode = pNextNode)
    {
        CRole* poRole = node_cast(CRole*, GetSceneListNode(), pNode);
        pNextNode = pNode->pNext;

        nRetCode = RemoveRole(poRole);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = poRole->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelRole(poRole);
        LOG_CHECK_ERROR(nRetCode);
    }
#endif  // _ROBOT

    for (INT32 i = 0; i < MAX_FACTION_COUNT; ++i)
    {
        if (m_lstpoBattleTeam[i])
        {
            LOG_CHECK_ERROR(m_lstpoBattleTeam[i]->GetMemberCount() == 0);

            nRetCode = m_lstpoBattleTeam[i]->UnInit();
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CGameWorld::GetInstance().DelTeam(m_lstpoBattleTeam[i]);
            LOG_CHECK_ERROR(nRetCode);

            m_lstpoBattleTeam[i] = NULL;
        }
    }

    for (INT32 nRegionX = 0; nRegionX < MAX_REGION_PER_SCENE; nRegionX++)
    {
        for (INT32 nRegionY = 0; nRegionY < MAX_REGION_PER_SCENE; nRegionY++)
        {
            if (m_poRegionList[nRegionX][nRegionY])
            {
                nRetCode = m_poRegionList[nRegionX][nRegionY]->UnInit();
                LOG_CHECK_ERROR(nRetCode);

                nRetCode = CGameWorld::GetInstance().DelRegion(m_poRegionList[nRegionX][nRegionY]);
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }
    memset(m_poRegionList, NULL, sizeof(m_poRegionList));

    SAFE_DELETE_ARRAY(m_pszZoneInstanceData);
    SAFE_DELETE_ARRAY(m_ppZoneInstanceData);
    m_ppZoneInstanceData = NULL;

    m_poStaticData = NULL;

    pNode = m_lstRespawnNpc.Head().pNext;
    while (pNode != &m_lstRespawnNpc.Rear())
    {
        CNpc* poNpc = node_cast(CNpc*, GetRegionListNode(), pNode);
        BASE_LIST_NODE* pNextNode = pNode->pNext;

        nRetCode = poNpc->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelNpc(poNpc);
        LOG_CHECK_ERROR(nRetCode);

        pNode = pNextNode;
    }
    m_lstRespawnNpc.Clear();

    pNode = m_lstRespawnDoodad.Head().pNext;
    while (pNode != &m_lstRespawnDoodad.Rear())
    {
        CDoodad* poDoodad = node_cast(CDoodad*, GetRegionListNode(), pNode);
        BASE_LIST_NODE* pNextNode = pNode->pNext;

        nRetCode = poDoodad->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelDoodad(poDoodad);
        LOG_CHECK_ERROR(nRetCode);

        pNode = pNextNode;
    }

    pNode = m_CombatStatList.Head().pNext;
    while (pNode != &(m_CombatStatList.Rear()))
    {
        BASE_LIST_NODE* pNextNode = pNode->pNext;
        CGameWorld::GetInstance().DelSceneCombatStatNode((COMBAT_STAT_NODE*)pNode);
        pNode = pNextNode;
    }
    m_CombatStatList.Clear();

    pNode = m_lstBTCtx.Head().pNext;
    while (pNode != &m_lstBTCtx.Rear())
    {
        BTCustomCtx* pBTCtx = node_cast(BTCustomCtx*, ListNode, pNode);
        BASE_LIST_NODE* pNextNode = pNode->pNext;

        nRetCode = CGameWorld::GetInstance().DelBTCtx(pBTCtx);
        LOG_CHECK_ERROR(nRetCode);

        pNode = pNextNode;
    }
    m_lstBTCtx.Clear();

    LOG_CHECK_ERROR(m_nRoleCount == 0);
    LOG_CHECK_ERROR(m_nNpcCount == 0);
    m_lstRole.Clear();
    m_nRoleCount = 0;
    m_nNpcCount = 0;
    m_nSceneTempID = m_nSceneID = 0;

    memset(m_SingleNpcObjID, 0, sizeof(m_SingleNpcObjID));

    return TRUE;
}

BOOL CScene::CheckTargetID(UINT32 dwNpcTempID)
{
    LOG_PROCESS_ERROR(m_pcSceneData);
    PROCESS_ERROR(m_pcSceneData->nFinishTarget == dwNpcTempID);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::MainLoop(void)
{
    INT32 nRetCode = 0;
    BASE_LIST_NODE* pNode = NULL;

#ifndef _ROBOT
    _CheckDLRecordRoleBegin(CGameWorld::GetInstance().GetGameFrame());
#endif	// _ROBOT

    // All regions in the current map
    UINT32* pdwValidRegionList = m_poStaticData->GetValidRegionList();
    while (*pdwValidRegionList)
    {
        INT32 nRegionX = *pdwValidRegionList >> 16;
        INT32 nRegionY = *pdwValidRegionList & 0xFFFF;
        CRegion* poRegion = m_poRegionList[nRegionX][nRegionY];
        if (poRegion && poRegion->IsActive(CGameWorld::GetInstance().GetGameTime()))
        {
            nRetCode = poRegion->MainLoop();  // Role/Npc/Doodad/Missile/Drop
            LOG_CHECK_ERROR(nRetCode);
        }
        pdwValidRegionList++;
    }

    //for (INT32 nRegionX = 0; nRegionX < m_nRegionWidth; nRegionX++)
    //{
    //	for (INT32 nRegionY = 0; nRegionY < m_nRegionHeight; nRegionY++)
    //	{
    //		CRegion* poRegion = m_poRegionList[nRegionX][nRegionY];
    //		if (poRegion && poRegion->IsActive())
    //		{
    //			nRetCode = poRegion->MainLoop();
    //			LOG_CHECK_ERROR(nRetCode);
    //		}
    //	}
    //}

#ifndef _ROBOT

    // NPC suicide: delete the NPC object
    {
        NPC_LIVE_TIMER_LIST::iterator it = m_LiveTimerList.begin();

        if (it != m_LiveTimerList.end() && it->dwSuicideTime < CGameWorld::GetInstance().GetGameTime())
        {
            CNpc* poNpc = CGameWorld::GetInstance().FindNpc(it->dwNpcID);
            if (poNpc)
            {
                nRetCode = RemoveNpc(poNpc);
                LOG_CHECK_ERROR(nRetCode);

                nRetCode = poNpc->UnInit();
                LOG_CHECK_ERROR(nRetCode);

                nRetCode = CGameWorld::GetInstance().DelNpc(poNpc);
                LOG_CHECK_ERROR(nRetCode);
            }

            m_LiveTimerList.erase(it);
            // INF("npc %d is erased from live timer list");
        }
    }

    // NPC respawn
    pNode = m_lstRespawnNpc.Head().pNext;
    while (pNode != &m_lstRespawnNpc.Rear())
    {
        CNpc* poNpc = node_cast(CNpc*, GetRegionListNode(), pNode);
        BASE_LIST_NODE* pNextNode = pNode->pNext;

        nRetCode = poNpc->MainLoop();
        LOG_CHECK_ERROR(nRetCode);

        if (poNpc->IsDeleteSelf())
        {
            nRetCode = poNpc->UnInit();
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CGameWorld::GetInstance().DelNpc(poNpc);
            LOG_CHECK_ERROR(nRetCode);
        }

        pNode = pNextNode;
    }

    // Doodad
    pNode = m_lstRespawnDoodad.Head().pNext;
    while (pNode != &m_lstRespawnDoodad.Rear())
    {
        CDoodad* poDoodad = node_cast(CDoodad*, GetRegionListNode(), pNode);
        BASE_LIST_NODE* pNextNode = pNode->pNext;

        nRetCode = poDoodad->MainLoop();
        LOG_CHECK_ERROR(nRetCode);

        if (poDoodad->IsDeleteSelf())
        {
            nRetCode = poDoodad->UnInit();
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CGameWorld::GetInstance().DelDoodad(poDoodad);
            LOG_CHECK_ERROR(nRetCode);
        }

        pNode = pNextNode;
    }

    // Behavior tree
    pNode = m_lstBTCtx.Head().pNext;
    while (pNode != &m_lstBTCtx.Rear())
    {
        BTCustomCtx* pBTCtx = node_cast(BTCustomCtx*, ListNode, pNode);
        BASE_LIST_NODE* pNextNode = pNode->pNext;

        nRetCode = _ProcessSyncRun(pBTCtx);
        LOG_CHECK_ERROR(nRetCode);

        pNode = pNextNode;
    }

    // Teleport players out when the map is destroyed
    if (m_nState == edstClosing && m_nClosingFrame < CGameWorld::GetInstance().GetGameFrame())
    {
        nRetCode = TraverseRole(m_TraverseRoleOnDestroy);
        LOG_CHECK_ERROR(nRetCode);

        m_nClosingFrame = INT_MAX;
    }

    if (m_nWeatherRefreshFrame < CGameWorld::GetInstance().GetGameFrame() && m_pcSceneData->RandWeather.GetTotalWeight() > 0)
    {
        INT32 nWeatherDataIndex = m_pcSceneData->RandWeather.Rand();
        if (nWeatherDataIndex >= 0 && nWeatherDataIndex < MAX_RANDOM_WEATHER_COUNT)
        {
            nRetCode = SetWeather(m_pcSceneData->WeatherData[nWeatherDataIndex].nWeather,
                g_Random(m_pcSceneData->WeatherData[nWeatherDataIndex].nMinDuration,
                    m_pcSceneData->WeatherData[nWeatherDataIndex].nMaxDuration));
            LOG_CHECK_ERROR(nRetCode);

            m_nWeatherRefreshFrame = CGameWorld::GetInstance().GetGameFrame() +
                g_Random(m_pcSceneData->nMinWeatherRefreshInterval, m_pcSceneData->nMaxWeatherRefreshInterval);
        }
        else
        {
            CRI("error weather index %d in scene %d", nWeatherDataIndex, m_pcSceneData->nSceneID);
            m_nWeatherRefreshFrame = INT_MAX;
        }
    }

    if (m_nWeatherEndFrame < CGameWorld::GetInstance().GetGameFrame())
    {
        nRetCode = SetWeather(swtNormal, INT_MAX);
        LOG_CHECK_ERROR(nRetCode);

        m_nWeatherEndFrame = INT_MAX;
    }

    _CheckDLRecordRoleEnd();

    // Sabac
    if ((m_nSceneTempID + m_nSceneID) % GAME_FPS == CGameWorld::GetInstance().GetGameFrame() % GAME_FPS &&
        m_nSceneTempID == g_GWConfig.Sabac.nSceneTempID)
        _OnSabacMainLoop();

#endif

    return TRUE;
}

BOOL CScene::Close(INT32 nDelayFrame)
{
    INT32 nRetCode = 0;
    LOG_PROCESS_ERROR(m_pcSceneData);

    m_nState = edstClosing;
    m_nTotalFrame = CGameWorld::GetInstance().GetGameFrame() - m_nStartFrame;

#ifndef _ROBOT

    nRetCode = CCliMessageHandler::GetInstance().DoSceneFinish(this);
    LOG_PROCESS_ERROR(nRetCode);

    if (m_pcSceneData->nType == estDungeon || m_pcSceneData->nType == estGuildDungeon || m_pcSceneData->nType == estSoloDungeon)
        m_TraverseRoleOnSceneClose.bRoleAura = FALSE;
    else
        m_TraverseRoleOnSceneClose.bRoleAura = TRUE;

    nRetCode = TraverseRole(m_TraverseRoleOnSceneClose);
    LOG_PROCESS_ERROR(nRetCode);

    for (INT32 nRegionX = 0; nRegionX < MAX_REGION_PER_SCENE; nRegionX++)
    {
        for (INT32 nRegionY = 0; nRegionY < MAX_REGION_PER_SCENE; nRegionY++)
        {
            if (m_poRegionList[nRegionX][nRegionY])
            {
                nRetCode = m_poRegionList[nRegionX][nRegionY]->TraverseNpc(m_TraverseNpcOnSceneClose);
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }

    nRetCode = CCliMessageHandler::GetInstance().DoMSCloseScene(m_nServerGroupID, m_nSceneTempID, m_nSceneID);
    LOG_PROCESS_ERROR(nRetCode);

    m_nClosingFrame = CGameWorld::GetInstance().GetGameFrame() + nDelayFrame;
#endif

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::AddRole(CRole* poRole, INT32 nAppearType)
{
    INT32 nRetCode = 0;
    INT32 nFaction = 0;

    CRegion* poRegion = NULL;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;
    ZONE_INSTANCE_DATA* pZoneInstance = NULL;

    LOG_PROCESS_ERROR(poRole);

    PROCESS_ERROR(m_nState == edstRuning);
    LOG_PROCESS_ERROR(poRole->GetScene() == NULL);
    LOG_PROCESS_ERROR(m_poStaticData);
    LOG_PROCESS_ERROR(m_pcSceneData);

    /************************************************************************/
    // check conditions

    nRetCode = FindValidPos(poRole, TRUE, pcZone, pcGrid, pGridInstance, poRegion);
    LOG_PROCESS_ERROR(nRetCode);

    pZoneInstance = GetZoneInstance(poRole->GetX(), poRole->GetY());
    LOG_PROCESS_ERROR(pZoneInstance);

    /************************************************************************/
    // add into scene

    nRetCode = poRegion->AddRole(poRole);
    LOG_PROCESS_ERROR(nRetCode);

    poRole->SetZ(pcGrid->uHighLayer * GRID_LAYER_HEIGHT);
    poRole->SetGrid(pcGrid);
    poRole->SetGridInstance(pGridInstance);
    poRole->SetZone(pcZone);

    poRole->SetScene(this);

    if (!poRole->IsDead())
    {
        nRetCode = poRole->SetGridCharRef();
        LOG_CHECK_ERROR(nRetCode);
    }

    poRole->SetState(rsPlaying);
    if (poRole->GetMoveState() != CMS_DIE)
        poRole->Stand(TRUE);

    nRetCode = m_lstRole.PushHead(&poRole->GetSceneListNode());
    LOG_CHECK_ERROR(nRetCode);
    m_nRoleCount++;

    if (m_pcSceneData->nHandbookType > htInvalid)
        poRole->GetAchievementList().ActivateHandbook(m_pcSceneData->nHandbookType, m_pcSceneData->nHandbookIndex);

    INF("<%08X> added role %s:%d into scene %d-%d (%d,%d,%d), total %d",
        poRole->GetSessionID(), poRole->GetName(), poRole->GetObjID(), m_nSceneTempID, m_nSceneID,
        (INT32)(poRole->GetX() / POINT_PER_CM), (INT32)(poRole->GetY() / POINT_PER_CM), (INT32)(poRole->GetZ() / POINT_PER_CM),
        CGameWorld::GetInstance().GetRoleCount());

    /************************************************************************/
    // post process, CANNOT FAIL

#ifndef _ROBOT
    if (m_pcSceneData->nType == estBattleField)          // todo_tq temp code Allan Tao[2.12.2013] 
    {
        nFaction = _SideConvertToFaction();
        poRole->SetFaction(nFaction);
    }

    nRetCode = CCliMessageHandler::GetInstance().DoEnterScene(poRole, this);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoSyncNextJumpPoint(poRole);
    LOG_CHECK_ERROR(nRetCode);
#endif

    {
        CHero* poHero = poRole->GetHero();
        if (poHero != NULL)
        {
            nRetCode = poRole->SummonHero();
            LOG_CHECK_ERROR(nRetCode == errSuccess);
        }
    }

#ifndef _ROBOT
    if (!poRole->IsHidden())
    {
        nRetCode = CCliMessageHandler::GetInstance().DoCharAppear(poRole, NULL, nAppearType);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CCliMessageHandler::GetInstance().DoSyncNewCharList(poRole, poRegion);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoSyncMissileList(poRole, poRegion);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoSyncDoodadList(poRole, poRegion);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoSyncDropList(poRole, poRegion);
    LOG_CHECK_ERROR(nRetCode);

    for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
    {
        CRegion* poConnectedRegion = poRegion->GetConnectedRegion(nDirection);
        if (poConnectedRegion == NULL)
            continue;

        nRetCode = CCliMessageHandler::GetInstance().DoSyncNewCharList(poRole, poConnectedRegion);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CCliMessageHandler::GetInstance().DoSyncDoodadList(poRole, poConnectedRegion);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CCliMessageHandler::GetInstance().DoSyncDropList(poRole, poConnectedRegion);
        LOG_CHECK_ERROR(nRetCode);
    }

    poRole->UpdateLastSaveFrame();

    nRetCode = CGameWorld::GetInstance().AddToSaveList(poRole);
    LOG_CHECK_ERROR(nRetCode);

    //update role_var
    if (poRole->GetAchievementList().GetVariable(avdSceneTempID) != m_nSceneTempID || poRole->GetAchievementList().GetVariable(avdSceneID) != m_nSceneID)
    {
        for (INT32 i = avdForSceneBegin; i <= avdForSceneEnd; ++i)
            poRole->SetVariable(i, 0);

        poRole->GetAchievementList().SetVariable(avdSceneTempID, m_nSceneTempID);
        poRole->GetAchievementList().SetVariable(avdSceneID, m_nSceneID);
    }

    if (m_pcSceneData->nType == estDungeon || m_pcSceneData->nType == estSoloDungeon)
    {
        poRole->GetAchievementList().AddVariable(avdEnterDungeonSceneCount, 1);
    }
    else if (m_pcSceneData->nType == estArena)
    {

    }
    else if (m_pcSceneData->nType == estBattleField)
    {
        INT32 nFaction = poRole->GetFaction();
        LOG_CHECK_ERROR(nFaction < MAX_FACTION_COUNT);
        LOG_CHECK_ERROR(m_lstpoBattleTeam[nFaction]);

        if (nFaction < MAX_FACTION_COUNT && m_lstpoBattleTeam[nFaction])
        {
            for (INT32 i = 0; i < MAX_TEAM_MEMBER_COUNT; ++i)
            {
                if (m_lstpoBattleTeam[nFaction]->GetMemberTable(i).dwRoleID == 0)
                {
                    m_lstpoBattleTeam[nFaction]->GetMemberTable(i).dwRoleID = poRole->GetObjID();
                    m_lstpoBattleTeam[nFaction]->GetMemberTable(i).nSex = poRole->GetSex();
                    m_lstpoBattleTeam[nFaction]->GetMemberTable(i).nLevel = poRole->GetLevel();
                    m_lstpoBattleTeam[nFaction]->GetMemberTable(i).nClass = poRole->GetClass();
                    strxcpy(m_lstpoBattleTeam[nFaction]->GetMemberTable(i).szRoleName, poRole->GetName(), ROLE_NAME_LEN);

                    m_lstpoBattleTeam[nFaction]->SetMemberCount(m_lstpoBattleTeam[nFaction]->GetMemberCount() + 1);

                    poRole->SetBattleTeamID(m_lstpoBattleTeam[nFaction]->GetTeamID());
                    break;
                }
            }
        }

        nRetCode = CCliMessageHandler::GetInstance().DoClientBattleTeamJoin(poRole, m_lstpoBattleTeam[nFaction]);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CCliMessageHandler::GetInstance().DoClientBattleTeamAdd(m_lstpoBattleTeam[nFaction], poRole);
        LOG_CHECK_ERROR(nRetCode);
    }

    _AttachCombatStatNode(poRole);

    if (m_poStaticData->GetBT(evtEnterScene))
    {
        nRetCode = RunBT(evtEnterScene, poRole->GetObjID(), m_poStaticData->GetBT(evtEnterScene));
        PROCESS_ERROR(nRetCode);
    }

    //	if (g_GWConfig.DebugOption.anticheat)
    //	{
    //		if (m_pcSceneData->bEnableAntiCheat)
    //			poRole->StartAntiCheat();
    //		else
    //			poRole->EndAntiCheat();
    //	}

#endif
    return TRUE;
Exit0:

    if (poRole)
    {
        const char* pcszName = "Unknown";
        INT32 nX = poRole->GetX() / POINT_PER_CM;
        INT32 nY = poRole->GetY() / POINT_PER_CM;
        INT32 nZ = poRole->GetZ() / POINT_PER_CM;
        ConvertCoordN2O(nX, nY, nZ);
        if (poRole->GetName())
            pcszName = poRole->GetName();

        CRI("failed to add role %s:%d into scene %d-%d [%d, %d, %d]", pcszName, poRole->GetObjID(),
            m_nSceneTempID, m_nSceneID, nX, nY, nZ);

        poRole->SetScene(NULL);
        if (poRegion)
            poRegion->RemoveRole(poRole);
    }

    return FALSE;
}

BOOL CScene::RemoveRole(CRole* poRole)
{
    INT32 nRetCode = 0;
    CRegion* poRegion = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;
    COMBAT_STAT_NODE* pStatNode = NULL;

    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(poRole->GetScene());

    if (m_poStaticData && m_poStaticData->GetBT(evtLeaveScene))
    {
        nRetCode = RunBT(evtLeaveScene, poRole->GetObjID(), m_poStaticData->GetBT(evtLeaveScene));
        LOG_CHECK_ERROR(nRetCode);
    }

    {
        CHero* poHero = poRole->GetHero();
        if (poHero != NULL)
        {
            nRetCode = poRole->RecallHero(TRUE);
            LOG_CHECK_ERROR(nRetCode == errSuccess);
        }
    }

    if (m_pcSceneData && m_pcSceneData->nType == estLine)
    {
        poRole->SetLastLineSceneTempID(m_nSceneTempID);
        poRole->SetLastLineSceneID(m_nSceneID);
    }

#ifndef _ROBOT
    if (poRole->GetStall()->GetStallState() != essNotOpen)
    {
        poRole->GetStall()->RealClose();
    }

    if (!poRole->IsHidden())
    {
        nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(poRole, NULL, sodtLeaveScene);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CCliMessageHandler::GetInstance().DoLeaveScene(poRole);
    LOG_CHECK_ERROR(nRetCode);
#endif

    nRetCode = CBaseList::Remove(&poRole->GetSceneListNode());
    LOG_CHECK_ERROR(nRetCode);
    m_nRoleCount--;

    if (poRole->GetStall()->GetStallState() != essNotOpen)
    {
#ifndef _ROBOT
        poRole->GetStall()->RealClose();
#endif
    }
    poRole->GetTrade()->Clear();

    pGridInstance = poRole->GetGridInstance();
    LOG_PROCESS_ERROR(pGridInstance);

#ifndef _ROBOT
    if (pGridInstance->uMissileTag2)
    {
        CMissile* poMissile = FindMissileByPlacedGrid(poRole->GetX() / GRID_WIDTH, poRole->GetY() / GRID_WIDTH);
        if (poMissile)
            poMissile->OnCharLeave(poRole);
    }
#endif


    if (!poRole->IsDead())
    {
        nRetCode = poRole->ClearGridCharRef();
        LOG_CHECK_ERROR(nRetCode);
    }

    poRole->SetGridInstance(NULL);
    poRole->SetGrid(NULL);
    poRole->SetZone(NULL);

    poRegion = poRole->GetRegion();
    LOG_CHECK_ERROR(poRegion);

    if (poRegion)
    {
        nRetCode = poRegion->RemoveRole(poRole);
        LOG_CHECK_ERROR(nRetCode);
    }

    poRole->SetScene(NULL);

#ifndef _ROBOT
    nRetCode = CBaseList::Remove(&poRole->GetSaveListNode());
    LOG_CHECK_ERROR(nRetCode);
#endif

    poRole->SetCombatStatNode(NULL);

    INF("<%08X> removed role %s:%d from scene %d-%d, total %d, scene left role count=%d ",
        poRole->GetSessionID(), poRole->GetName(), poRole->GetObjID(), m_nSceneTempID, m_nSceneID,
        CGameWorld::GetInstance().GetRoleCount(), GetRoleCount());

#ifndef _ROBOT

    if (m_pcSceneData && m_pcSceneData->nType == estBattleField)
    {
        BOOL bChangeLeader = FALSE;
        INT32 nFaction = poRole->GetFaction();
        LOG_PROCESS_ERROR(nFaction < MAX_FACTION_COUNT);
        LOG_PROCESS_ERROR(m_lstpoBattleTeam[nFaction]);

        nRetCode = CCliMessageHandler::GetInstance().DoClientBattleTeamRemove(m_lstpoBattleTeam[nFaction], poRole->GetObjID());
        LOG_PROCESS_ERROR(nRetCode);

        for (INT32 i = 0; i < MAX_TEAM_MEMBER_COUNT; ++i)
        {
            if (m_lstpoBattleTeam[nFaction]->GetMemberTable(i).dwRoleID == poRole->GetObjID())
            {
                m_lstpoBattleTeam[nFaction]->GetMemberTable(i).dwRoleID = 0;
                m_lstpoBattleTeam[nFaction]->SetMemberCount(m_lstpoBattleTeam[nFaction]->GetMemberCount() - 1);
                poRole->SetBattleTeamID(0);

                if (i == 0 && m_lstpoBattleTeam[nFaction]->GetMemberCount() > 0)
                    bChangeLeader = TRUE;

                break;
            }
        }

        if (bChangeLeader)
        {
            for (INT32 i = 1; i < MAX_TEAM_MEMBER_COUNT; ++i)
            {
                if (m_lstpoBattleTeam[nFaction]->GetMemberTable(i).dwRoleID > 0)
                {
                    memcpy(&m_lstpoBattleTeam[nFaction]->GetMemberTable(0),
                        &m_lstpoBattleTeam[nFaction]->GetMemberTable(i),
                        sizeof(TeamMember));

                    m_lstpoBattleTeam[nFaction]->GetMemberTable(i).dwRoleID = 0;
                    break;
                }
            }

            nRetCode = CCliMessageHandler::GetInstance().DoClientBattleTeamLeaderChange(m_lstpoBattleTeam[nFaction], m_lstpoBattleTeam[nFaction]->GetMemberTable(0).dwRoleID);
            LOG_PROCESS_ERROR(nRetCode);
        }

    }

#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::AddNpc(CNpc* poNpc, INT32 nAppearType)
{
    INT32 nRetCode = 0;

    CRegion* poRegion = NULL;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    LOG_PROCESS_ERROR(poNpc);
    LOG_PROCESS_ERROR(m_poStaticData);

    nRetCode = FindValidPos(poNpc, FALSE, pcZone, pcGrid, pGridInstance, poRegion);
    if (!nRetCode && poNpc->IsPet())
    {
        nRetCode = FindValidPos(poNpc, TRUE, pcZone, pcGrid, pGridInstance, poRegion);
    }
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = poRegion->AddNpc(poNpc);
    LOG_PROCESS_ERROR(nRetCode);

    poNpc->SetZ(pcGrid->uHighLayer * GRID_LAYER_HEIGHT);
    poNpc->SetGrid(pcGrid);
    poNpc->SetGridInstance(pGridInstance);
    poNpc->SetZone(pcZone);

    poNpc->SetScene(this);

    if (!poNpc->IsDead())
    {
        nRetCode = poNpc->Stand(TRUE);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = poNpc->SetGridCharRef();
        LOG_CHECK_ERROR(nRetCode);
    }

    m_nNpcCount++;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoCharAppear(poNpc, NULL, nAppearType);
    LOG_CHECK_ERROR(nRetCode);
#endif // _ROBOT

    return TRUE;
Exit0:
    if (poNpc)
    {
        INT32 nNpcTemplateID = 0;
        INT32 nX = poNpc->GetX() / POINT_PER_CM;
        INT32 nY = poNpc->GetY() / POINT_PER_CM;
        INT32 nZ = poNpc->GetZ() / POINT_PER_CM;
        if (poNpc->GetNpcData())
            nNpcTemplateID = poNpc->GetNpcData()->nTemplateID;
        ConvertCoordN2O(nX, nY, nZ);
        CRI("failed to add npc %d into scene %d-%d [%d, %d, %d]", nNpcTemplateID, m_nSceneTempID, m_nSceneID, nX, nY, nZ);

        poNpc->SetScene(NULL);
        if (poRegion)
            poRegion->RemoveNpc(poNpc);
    }

    return FALSE;
}

BOOL CScene::RemoveNpc(CNpc* poNpc)
{
    INT32 nRetCode = 0;
    CRegion* poRegion = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    LOG_PROCESS_ERROR(poNpc);

    m_nNpcCount--;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(poNpc, NULL, sodtLeaveScene);
    LOG_CHECK_ERROR(nRetCode);

    pGridInstance = poNpc->GetGridInstance();
    LOG_PROCESS_ERROR(pGridInstance);

    if (pGridInstance->uMissileTag2)
    {
        CMissile* poMissile = FindMissileByPlacedGrid(poNpc->GetX() / GRID_WIDTH, poNpc->GetY() / GRID_WIDTH);
        if (poMissile)
            poMissile->OnCharLeave(poNpc);
    }
#endif  // _ROBOT

    if (!poNpc->IsDead())
    {
        nRetCode = poNpc->ClearGridCharRef();
        LOG_CHECK_ERROR(nRetCode);
    }

    poNpc->SetGridInstance(NULL);
    poNpc->SetGrid(NULL);
    poNpc->SetZone(NULL);

    poRegion = poNpc->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    nRetCode = poRegion->RemoveNpc(poNpc);
    LOG_PROCESS_ERROR(nRetCode);

    poNpc->SetScene(NULL);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::AddHero(CHero* poHero, INT32 nAppearType)
{
    INT32 nRetCode = 0;
    CRegion* poRegion = NULL;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    LOG_PROCESS_ERROR(poHero);
    LOG_PROCESS_ERROR(m_poStaticData);

    nRetCode = FindValidPos(poHero, FALSE, pcZone, pcGrid, pGridInstance, poRegion);
    LOG_PROCESS_ERROR(nRetCode);

    poHero->SetZ(pcGrid->uHighLayer * GRID_LAYER_HEIGHT);
    poHero->SetGridInstance(pGridInstance);
    poHero->SetGrid(pcGrid);
    poHero->SetZone(pcZone);

    nRetCode = poRegion->AddHero(poHero);
    LOG_PROCESS_ERROR(nRetCode);

    poHero->SetScene(this);

    if (!poHero->IsDead())
    {
        nRetCode = poHero->Stand(TRUE);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = poHero->SetGridCharRef();
        LOG_CHECK_ERROR(nRetCode);
    }

    INF("added hero %s:%d from scene (%d, %d)", poHero->GetName(), poHero->GetObjID(), m_nSceneTempID, m_nSceneID);

    poHero->SetCombatStat(escsTotalHeal, 0);
    poHero->SetCombatStat(escsTotalDamage, 0);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoCharAppear(poHero, NULL, nAppearType);
    LOG_CHECK_ERROR(nRetCode);
#endif // _ROBOT

    return TRUE;
Exit0:
    if (poHero)
    {
        poHero->SetScene(NULL);
        if (poRegion)
            poRegion->RemoveHero(poHero);
    }

    return FALSE;
}

BOOL CScene::RemoveHero(CHero* poHero)
{
    INT32 nRetCode = 0;
    CRegion* poRegion = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    LOG_PROCESS_ERROR(poHero);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(poHero, NULL, sodtLeaveScene);
    LOG_CHECK_ERROR(nRetCode);

    pGridInstance = poHero->GetGridInstance();
    LOG_PROCESS_ERROR(pGridInstance);

    if (pGridInstance->uMissileTag2)
    {
        CMissile* poMissile = FindMissileByPlacedGrid(poHero->GetX() / GRID_WIDTH, poHero->GetY() / GRID_WIDTH);
        if (poMissile)
            poMissile->OnCharLeave(poHero);
    }
#endif  // _ROBOT

    if (!poHero->IsDead())
    {
        nRetCode = poHero->ClearGridCharRef();
        LOG_CHECK_ERROR(nRetCode);
    }

    poHero->SetGrid(NULL);
    poHero->SetGridInstance(NULL);
    poHero->SetZone(NULL);

    poRegion = poHero->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    nRetCode = poRegion->RemoveHero(poHero);
    LOG_PROCESS_ERROR(nRetCode);

    poHero->SetScene(NULL);

    INF("removed hero %s:%d from scene (%d, %d)", poHero->GetName(), poHero->GetObjID(), m_nSceneTempID, m_nSceneID);

    return TRUE;
Exit0:
    return FALSE;

}

BOOL CScene::AddDoodad(CDoodad* poDoodad)
{
    INT32 nRetCode = 0;
    INT32 nRegionX = 0;
    INT32 nRegionY = 0;
    CRegion* poRegion = NULL;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    LOG_PROCESS_ERROR(poDoodad);

    nRegionX = (INT32)poDoodad->GetX() / REGION_WIDTH;
    nRegionY = (INT32)poDoodad->GetY() / REGION_WIDTH;

    nRetCode = GetLowerGrid((INT32)poDoodad->GetX(), (INT32)poDoodad->GetY(), (INT32)poDoodad->GetZ() + GRID_WIDTH, pcZone, pcGrid);
    LOG_PROCESS_ERROR(nRetCode);

    pGridInstance = GetGridInstance(pcZone, pcGrid);
    LOG_PROCESS_ERROR(pGridInstance);

    nRetCode = poDoodad->CheckGridCharRef(this, poDoodad->GetX(), poDoodad->GetY(), poDoodad->GetZ());
    LOG_PROCESS_ERROR(nRetCode);

    poDoodad->SetGrid(pcGrid);
    poDoodad->SetGridInstance(pGridInstance);
    poDoodad->SetZone(pcZone);

    poRegion = GetRegion(nRegionX, nRegionY);
    LOG_PROCESS_ERROR(poRegion);

    nRetCode = poRegion->AddDoodad(poDoodad);
    LOG_PROCESS_ERROR(nRetCode);

    poDoodad->SetScene(this);

    nRetCode = poDoodad->SetGridCharRef();
    LOG_CHECK_ERROR(nRetCode);

    if (poDoodad->GetSpawnPoint())
    {
        _SetDoodadPointSpawned(poDoodad->GetSpawnPoint()->nGroupID, poDoodad->GetSpawnPoint()->nID, TRUE);
    }

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoDoodadAppear(poDoodad);
    LOG_CHECK_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    if (poDoodad)
    {
        INT32 nDoodadID = 0;
        INT32 nX = poDoodad->GetX() / POINT_PER_CM;
        INT32 nY = poDoodad->GetY() / POINT_PER_CM;
        INT32 nZ = poDoodad->GetZ() / POINT_PER_CM;
        if (poDoodad->GetDoodadData())
            nDoodadID = poDoodad->GetDoodadData()->nTemplateID;
        ConvertCoordN2O(nX, nY, nZ);
        CRI("failed to add doodad %d into scene %d-%d [%d, %d, %d]", nDoodadID, m_nSceneTempID, m_nSceneID, nX, nY, nZ);

        poDoodad->SetScene(NULL);
        if (poRegion)
            poRegion->RemoveDoodad(poDoodad);
    }

    return FALSE;
}

BOOL CScene::RemoveDoodad(CDoodad* poDoodad)
{
    INT32 nRetCode = 0;
    CRegion* poRegion = NULL;

    LOG_PROCESS_ERROR(poDoodad);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(poDoodad, NULL, sodtLeaveScene);
    LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT

    if (poDoodad->GetSpawnPoint())
    {
        _SetDoodadPointSpawned(poDoodad->GetSpawnPoint()->nGroupID, poDoodad->GetSpawnPoint()->nID, FALSE);
    }

    nRetCode = poDoodad->ClearGridCharRef();
    LOG_CHECK_ERROR(nRetCode);

    poDoodad->SetGrid(NULL);
    poDoodad->SetGridInstance(NULL);
    poDoodad->SetZone(NULL);

    poRegion = poDoodad->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    nRetCode = poRegion->RemoveDoodad(poDoodad);
    LOG_PROCESS_ERROR(nRetCode);

    poDoodad->SetScene(NULL);

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CScene::AddDrop(CDrop* poDrop, INT64 llX, INT64 llY, INT64 llZ, UINT32 dwSrcID)
{
    //Try to find a suitable position and spawn the drop
    //If none is found, delete the drop and return FALSE

    INT32 nRetCode = 0;
    INT32 nRegionX = 0;
    INT32 nRegionY = 0;
    CRegion* poRegion = NULL;

    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    INT32 nDropPosIndex = 0;
    INT64 llCenterX = llX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    INT64 llCenterY = llY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    INT64 llDropX, llDropY, llDropZ;
    BOOL bFound = FALSE;

    LOG_PROCESS_ERROR(poDrop);
    LOG_PROCESS_ERROR(poDrop->GetScene() == NULL);

    for (UINT8 looking = 0; looking < MAX_DROP_PER_GRID; looking++)
    {
        for (nDropPosIndex = 1; nDropPosIndex < sizeof(s_nLocationOffsetTable) / sizeof(s_nLocationOffsetTable[0]); nDropPosIndex++)
        {
            llDropX = llCenterX + s_nLocationOffsetTable[nDropPosIndex][0];
            llDropY = llCenterY + s_nLocationOffsetTable[nDropPosIndex][1];

            nRetCode = GetLowerGrid(llDropX, llDropY, llZ, pcZone, pcGrid);
            if (!nRetCode)
                continue;

            LOG_PROCESS_ERROR(pcZone);
            LOG_PROCESS_ERROR(pcGrid);

            pGridInstance = GetGridInstance(pcZone, pcGrid);
            if (pGridInstance == NULL)
                continue;

            llDropZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;
            if (!pcGrid->bOutOfPoly && !pcGrid->bCannotDropItem && pGridInstance->uCharRef == 0 && pGridInstance->uDropRef == looking)
            {
                bFound = TRUE;
                break;
            }
        }

        if (bFound)
            break;
    }

    if (!bFound)
    {
        if (poDrop->m_content.m_type == DropContent::DROP_MONEY)
        {
            CRI("No Place To Hold Drop: SceneID:%d, SceneTemplateID:%d, MoneyType:%d, MoneyCount:%d",
                GetSceneID(), GetSceneTempID(), poDrop->m_content.m_money.moneytype, poDrop->m_content.m_money.count);
        }
        else if (poDrop->m_content.m_type == DropContent::DROP_ITEM)
        {
            CRI("No Place To Hold Drop: SceneID:%d, SceneTemplateID:%d, ItemID:%d, ItemCount:%d",
                GetSceneID(), GetSceneTempID(), poDrop->m_content.m_item->m_iteminfo.m_nTemplateID,
                poDrop->m_content.m_item->m_pTemplate->m_byDurType == DURTYPE_STACK ? poDrop->m_content.m_item->m_iteminfo.m_nCurDur : 1);
        }
        else
        {
            CRI("No Place To Hold Drop: SceneID:%d, SceneTemplateID:%d, DropType: Unknown",
                GetSceneID(), GetSceneTempID());
        }
    }
    LOG_PROCESS_ERROR(bFound);

    poRegion = GetRegion((INT32)llDropX / REGION_WIDTH, (INT32)llDropY / REGION_WIDTH);
    LOG_PROCESS_ERROR(poRegion);

    nRetCode = poRegion->AddDrop(poDrop);
    LOG_PROCESS_ERROR(nRetCode);

    pGridInstance->uDropRef++;

    poDrop->SetX(llDropX);
    poDrop->SetY(llDropY);
    poDrop->SetZ(llDropZ);

    poDrop->SetGrid(pcGrid);
    poDrop->SetGridInstance(pGridInstance);
    poDrop->SetZone(pcZone);

    poDrop->SetScene(this);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoGenerateDropFromObject(poDrop, dwSrcID);
    LOG_CHECK_ERROR(nRetCode);

    if (poDrop->m_content.m_type == DropContent::DROP_ITEM &&
        GetTypeFromID(dwSrcID) == otNpc)
    {
        INT32 npc_template_id = 0;
        CNpc* npc = CGameWorld::GetInstance().FindNpc(dwSrcID);
        if (npc && npc->GetNpcData())
            npc_template_id = npc->GetNpcData()->nTemplateID;

        if (poDrop->m_content.m_item->m_pTemplate->m_byLog)
        {
            nRetCode = CCliMessageHandler::GetInstance().DoDLLogItem(
                0,
                GetServerGroup(),
                poDrop->m_content.m_item->m_iteminfo.m_llGuid,
                poDrop->m_content.m_item->m_pTemplate->m_nTemplateID,
                poDrop->m_content.m_item->m_pTemplate->m_byDurType == DURTYPE_STACK ? poDrop->m_content.m_item->m_iteminfo.m_nCurDur : 1,
                ITEMLOG_NPC_DROP,
                ITEMACTION_DROP,
                npc_template_id);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    if (poDrop->m_content.m_type == DropContent::DROP_MONEY &&
        poDrop->m_content.m_money.moneytype == MONEYTYPE_02 &&
        GetTypeFromID(dwSrcID) == otNpc)
    {
        INT32 npc_template_id = 0;
        CNpc* npc = CGameWorld::GetInstance().FindNpc(dwSrcID);
        if (npc && npc->GetNpcData())
            npc_template_id = npc->GetNpcData()->nTemplateID;
        {
            nRetCode = CCliMessageHandler::GetInstance().DoDLNpcMoney(
                GetServerGroup(),
                npc->GetNpcData()->nTemplateID,
                npc->GetNpcData()->bStatMoney,
                poDrop->m_content.m_money.count);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
#endif

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CScene::RemoveDrop(CDrop* poDrop)
{
    INT32 nRetCode = 0;
    CRegion* poRegion = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    LOG_PROCESS_ERROR(poDrop);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(poDrop, NULL, sodtLeaveScene);
    LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT

    pGridInstance = poDrop->GetGridInstance();
    LOG_PROCESS_ERROR(pGridInstance);
    LOG_CHECK_ERROR(pGridInstance->uDropRef > 0);
    if (pGridInstance->uDropRef > 0)
        pGridInstance->uDropRef--;

    poDrop->SetGrid(NULL);
    poDrop->SetGridInstance(NULL);
    poDrop->SetZone(NULL);

    poRegion = poDrop->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    nRetCode = poRegion->RemoveDrop(poDrop);
    LOG_PROCESS_ERROR(nRetCode);

    poDrop->SetScene(NULL);

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CScene::AddMissile(CMissile* poMissile)
{
    INT32 nRetCode = 0;
    INT32 nRegionX = 0;
    INT32 nRegionY = 0;
    CRegion* poRegion = NULL;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    LOG_PROCESS_ERROR(poMissile);

    nRegionX = (INT32)poMissile->GetX() / REGION_WIDTH;
    nRegionY = (INT32)poMissile->GetY() / REGION_WIDTH;

    nRetCode = GetLowerGrid((INT32)poMissile->GetX(), (INT32)poMissile->GetY(), (INT32)poMissile->GetZ() + GRID_WIDTH, pcZone, pcGrid);
    LOG_PROCESS_ERROR(nRetCode);

    pGridInstance = GetGridInstance(pcZone, pcGrid);
    LOG_PROCESS_ERROR(pGridInstance);

    poMissile->SetGrid(pcGrid);
    poMissile->SetGridInstance(pGridInstance);
    poMissile->SetZone(pcZone);

    poRegion = GetRegion(nRegionX, nRegionY);
    LOG_PROCESS_ERROR(poRegion);

    nRetCode = poRegion->AddMissile(poMissile);
    LOG_PROCESS_ERROR(nRetCode);

    poMissile->SetScene(this);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMissileAppear(poMissile, NULL);
    LOG_CHECK_ERROR(nRetCode);
#endif // _ROBOT

    return TRUE;
Exit0:
    if (poMissile)
    {
        poMissile->SetScene(NULL);
        if (poRegion)
            poRegion->RemoveMissile(poMissile);
    }
    return FALSE;
}

BOOL CScene::RemoveMissile(CMissile* poMissile)
{
    INT32 nRetCode = 0;
    CRegion* poRegion = NULL;

    LOG_PROCESS_ERROR(poMissile);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(poMissile, NULL, sodtLeaveScene);
    LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT

    poRegion = poMissile->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    nRetCode = poRegion->RemoveMissile(poMissile);
    LOG_PROCESS_ERROR(nRetCode);

    poMissile->SetScene(NULL);

    return TRUE;
Exit0:

    return FALSE;
}

CNpc* CScene::SpawnPet(INT32 nTemplateID, INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection,
    UINT32 dwHostID, const NPC_SPAWN_POINT* pcSpawnPoint, INT32 nAppearType)
{
    CNpc* poNpc = NULL;
    INT32 nRetCode = 0;
    const NPC_DATA* pNpcData = NULL;
    CAIProcessor* poAIProcessor = NULL;

    LOG_PROCESS_ERROR(nAppearType > soatInvalid && nAppearType < soatTotal);
    PROCESS_ERROR(m_nState == edstRuning);
    LOG_PROCESS_ERROR(m_nNpcCount < MAX_NPC_COUNT_PER_SCENE);

    pNpcData = CDataManager<NPC_DATA>::GetInstance().GetData(nTemplateID);
    LOG_PROCESS_ERROR(pNpcData);

    poNpc = CGameWorld::GetInstance().NewNpc(0);
    LOG_PROCESS_ERROR(poNpc);

    nRetCode = poNpc->Init(CGameWorld::GetInstance().GetNpcID(poNpc), pcSpawnPoint, dwHostID);
    LOG_PROCESS_ERROR(nRetCode);

    poNpc->SetX(llX);
    poNpc->SetY(llY);
    poNpc->SetZ(llZ);
    poNpc->SetFaceDirection(nFaceDirection);

    if (poNpc->GetSpawnPoint() && poNpc->GetSpawnPoint()->nSpawnTime > 0)
    {
        poNpc->SetNpcTimer(CGameWorld::GetInstance().GetGameFrame() + poNpc->GetSpawnPoint()->nSpawnTime);
        poNpc->IncControlCount(cclInvincible);
    }
    else if (pNpcData->nSpawnTime > 0)
    {
        poNpc->SetNpcTimer(CGameWorld::GetInstance().GetGameFrame() + pNpcData->nSpawnTime);
        poNpc->IncControlCount(cclInvincible);
    }

    nRetCode = poNpc->SetTemplateData(nTemplateID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = AddNpc(poNpc, nAppearType);
    LOG_PROCESS_ERROR(nRetCode);

    poNpc->SetHome(m_nSceneTempID, poNpc->GetX(), poNpc->GetY(), poNpc->GetZ());

    poAIProcessor = poNpc->GetAIProcessor();
    LOG_PROCESS_ERROR(poAIProcessor);

    poAIProcessor->CallInitBT();

    return poNpc;
Exit0:
    if (poNpc)
    {
        poNpc->UnInit();
        CGameWorld::GetInstance().DelNpc(poNpc);
    }
    CRI("Failed to spawn npc %d @ scene %d (%d, %d, %d)", nTemplateID, m_nSceneTempID,
        llX / POINT_PER_CM, llY / POINT_PER_CM, llZ / POINT_PER_CM);

    return NULL;
}

// Spawn NPCs
CNpc* CScene::SpawnNpc(INT32 nTemplateID, INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection, UINT32 dwHostID, const NPC_SPAWN_POINT* pcSpawnPoint)
{
    CNpc* poNpc = NULL;
    INT32 nRetCode = 0;
    const NPC_DATA* pNpcData = NULL;
    CAIProcessor* poAIProcessor = NULL;

    // New code: skip certain NPCs so they do not spawn; move this to configuration later
    if (this->m_nSceneTempID == 143 || this->m_nSceneTempID == 147)
    {
        switch (nTemplateID)
        {
        case 6171:	  // Open Beta Envoy 01
        case 6160:	  // Mafa Envoy
        case 6168:    // Homecoming Envoy
            return NULL;
        default:
            break;
        }
    }

    switch (nTemplateID)
    {
    case 3157:
        int a = 0;
        break;
    }


    PROCESS_ERROR(m_nState == edstRuning);
    LOG_PROCESS_ERROR(m_nNpcCount < MAX_NPC_COUNT_PER_SCENE);

    pNpcData = CDataManager<NPC_DATA>::GetInstance().GetData(nTemplateID);
    LOG_PROCESS_ERROR(pNpcData);

    poNpc = CGameWorld::GetInstance().NewNpc(0);
    LOG_PROCESS_ERROR(poNpc);

    nRetCode = poNpc->Init(CGameWorld::GetInstance().GetNpcID(poNpc), pcSpawnPoint, dwHostID);
    LOG_PROCESS_ERROR(nRetCode);

    poNpc->SetX(llX);
    poNpc->SetY(llY);
    poNpc->SetZ(llZ);
    poNpc->SetFaceDirection(nFaceDirection);

    if (poNpc->GetSpawnPoint() && poNpc->GetSpawnPoint()->nSpawnTime > 0)
    {
        poNpc->SetNpcTimer(CGameWorld::GetInstance().GetGameFrame() + poNpc->GetSpawnPoint()->nSpawnTime);
        poNpc->IncControlCount(cclInvincible);
    }
    else if (pNpcData->nSpawnTime > 0)
    {
        poNpc->SetNpcTimer(CGameWorld::GetInstance().GetGameFrame() + pNpcData->nSpawnTime);
        poNpc->IncControlCount(cclInvincible);
    }

    nRetCode = poNpc->SetTemplateData(nTemplateID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = AddNpc(poNpc, soatSpawn);
    LOG_PROCESS_ERROR(nRetCode);

    poNpc->SetHome(m_nSceneTempID, poNpc->GetX(), poNpc->GetY(), poNpc->GetZ());

    poAIProcessor = poNpc->GetAIProcessor();
    LOG_PROCESS_ERROR(poAIProcessor);

    poAIProcessor->CallInitBT();

    return poNpc;
Exit0:
    if (poNpc)
    {
        poNpc->UnInit();
        CGameWorld::GetInstance().DelNpc(poNpc);
    }
    CRI("Failed to spawn npc %d @ scene %d (%d, %d, %d)", nTemplateID, m_nSceneTempID,
        llX / POINT_PER_CM, llY / POINT_PER_CM, llZ / POINT_PER_CM);

    return NULL;
}

CNpc* CScene::SpawnNpcPoint(const NPC_SPAWN_POINT* pcSpawnPoint)
{
    CNpc* poNpc = NULL;
    INT32 nRetCode = 0;
    INT32 nTemplateID = 0;
    INT32 nFaceDirection = 0;
    INT32 nX, nY, nZ;
    NPC_LARGE_GROUP* pLargeGroup = NULL;
    NPC_MEDIUM_GROUP* pMediumGroup = NULL;

    LOG_PROCESS_ERROR(pcSpawnPoint);
    PROCESS_ERROR(m_nState == edstRuning);
    LOG_PROCESS_ERROR(m_nNpcCount < MAX_NPC_COUNT_PER_SCENE);

    //	poNpc = CGameWorld::GetInstance().NewNpc(0);
    //	LOG_PROCESS_ERROR(poNpc);

    if (pcSpawnPoint->nLargeGroupID >= 0 && pcSpawnPoint->nLargeGroupID < MAX_NPC_LARGE_GROUP_COUNT)
        pLargeGroup = m_LargeGroupList + pcSpawnPoint->nLargeGroupID;
    if (pcSpawnPoint->nMediumGroupID >= 0 && pcSpawnPoint->nMediumGroupID < MAX_NPC_MEDIUM_GROUP_PER_LARGE_GROUP)
        pMediumGroup = m_MediumGroupList + pcSpawnPoint->nMediumGroupID;

    // template id
    nTemplateID = pcSpawnPoint->RandNpc.Rand();
    LOG_PROCESS_ERROR(nTemplateID > 0);

    nFaceDirection = pcSpawnPoint->nDirection;
    if (pcSpawnPoint->nRange > 0)
        nFaceDirection = g_Random(8) * (MAX_DIRECTION_COUNT / 8);

    nX = pcSpawnPoint->nX + pcSpawnPoint->nRange - g_Random(pcSpawnPoint->nRange * 2);
    nY = pcSpawnPoint->nY + pcSpawnPoint->nRange - g_Random(pcSpawnPoint->nRange * 2);
    nZ = pcSpawnPoint->nZ;

    poNpc = SpawnNpc(nTemplateID, nX, nY, nZ, nFaceDirection, 0, pcSpawnPoint);
    if (poNpc == NULL)
    {
        CRI("retry to spawn at original point");

        poNpc = SpawnNpc(nTemplateID, pcSpawnPoint->nX, pcSpawnPoint->nY, pcSpawnPoint->nZ, nFaceDirection, 0, pcSpawnPoint);
        LOG_PROCESS_ERROR(poNpc);
    }

    if (pMediumGroup)
        pMediumGroup->nSpawnCount++;

    return poNpc;
Exit0:
    if (poNpc)
    {
        poNpc->UnInit();
        CGameWorld::GetInstance().DelNpc(poNpc);
    }
    if (pcSpawnPoint)
    {
        CRI("Failed to spawn npc point %d @ scene %d (%d, %d, %d) finally", nTemplateID, m_nSceneTempID,
            pcSpawnPoint->nX / POINT_PER_CM, pcSpawnPoint->nY / POINT_PER_CM, pcSpawnPoint->nZ / POINT_PER_CM);
    }
    else
    {
        CRI("Failed to spawn npc @ scene %d null point", m_nSceneTempID);
    }

    return NULL;
}

CNpc* CScene::SpawnSingleNpc(INT32 nSingleNpcID)
{
    CNpc* pResult = NULL;
    const NPC_SPAWN_POINT* pSpawnPoint = NULL;

    LOG_PROCESS_ERROR(m_poStaticData);

    pSpawnPoint = m_poStaticData->GetSingleNpc(nSingleNpcID);
    LOG_PROCESS_ERROR(pSpawnPoint);

    pResult = SpawnNpcPoint(pSpawnPoint);
    LOG_PROCESS_ERROR(pResult);

    return pResult;
Exit0:
    CRI("failed to spawn single npc %d scene %d", nSingleNpcID, m_nSceneTempID);

    return NULL;
}

INT32 CScene::GetMediumGroupSpawnCount(INT32 nMediumGroupID)
{
    NPC_MEDIUM_GROUP& rMediumGroup = m_MediumGroupList[nMediumGroupID];

    LOG_PROCESS_ERROR(nMediumGroupID >= 0 && nMediumGroupID < MAX_NPC_MEDIUM_GROUP_COUNT);

    return rMediumGroup.nSpawnCount;
Exit0:
    return 0;
}

BOOL CScene::SpawnNpcMediumGroup(INT32 nMediumGroupID)
{
    const NPC_MEDIUM_SPAWN_GROUP* pMediumGroup = NULL;
    INT32 nSmallGroupIndex = 0;
    NPC_MEDIUM_GROUP& rMediumGroup = m_MediumGroupList[nMediumGroupID];
    CNpc* poNpc = NULL;

    LOG_PROCESS_ERROR(nMediumGroupID >= 0 && nMediumGroupID < MAX_NPC_MEDIUM_GROUP_COUNT);
    LOG_PROCESS_ERROR(rMediumGroup.nSpawnCount == 0);
    LOG_PROCESS_ERROR(m_poStaticData);

    pMediumGroup = m_poStaticData->GetNpcMediumGroup(nMediumGroupID);
    LOG_PROCESS_ERROR(pMediumGroup);

    nSmallGroupIndex = g_Random(pMediumGroup->nSmallGroupCount);
    PROCESS_ERROR(nSmallGroupIndex < pMediumGroup->nSmallGroupCount);
    for (INT32 nPointIndex = 0; nPointIndex < pMediumGroup->SmallGroupList[nSmallGroupIndex]->nPointCount; nPointIndex++)
    {
        const NPC_SPAWN_POINT* pPoint = pMediumGroup->SmallGroupList[nSmallGroupIndex]->PointList + nPointIndex;

        poNpc = SpawnNpcPoint(pPoint);
        LOG_CHECK_ERROR(poNpc);
    }

    return TRUE;
Exit0:
    CRI("failed to spawn npc medium group %d scene %d", nMediumGroupID, m_nSceneTempID);

    return FALSE;
}

BOOL CScene::SpawnNpcLargeGroup(INT32 nLargeGroupID)
{
    INT32 nRetCode = 0;
    const NPC_LARGE_SPAWN_GROUP* pLargeGroup = NULL;
    NPC_LARGE_GROUP& rLargeGroup = m_LargeGroupList[nLargeGroupID];

    LOG_PROCESS_ERROR(nLargeGroupID >= 0 && nLargeGroupID < MAX_NPC_LARGE_GROUP_COUNT);
    LOG_PROCESS_ERROR(rLargeGroup.nDieCount == -1);
    LOG_PROCESS_ERROR(m_poStaticData);

    pLargeGroup = m_poStaticData->GetNpcLargeGroup(nLargeGroupID);
    LOG_PROCESS_ERROR(pLargeGroup);

    for (INT32 nMediumGroupIndex = 0; nMediumGroupIndex < pLargeGroup->nMediumGroupCount; nMediumGroupIndex++)
    {
        const NPC_MEDIUM_SPAWN_GROUP* pMediumGroup =
            m_poStaticData->GetNpcMediumGroup(pLargeGroup->MediumGroupList[nMediumGroupIndex]);

        if (pMediumGroup && pMediumGroup->bAutoSpawn)
        {
            nRetCode = SpawnNpcMediumGroup(pLargeGroup->MediumGroupList[nMediumGroupIndex]);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    CRI("failed to spawn npc large group %d scene %d", nLargeGroupID, m_nSceneTempID);

    return FALSE;
}

BOOL CScene::RespawnNpc(CNpc* poNpc)
{
    INT32 nRetCode = 0;
    INT32 nTemplateID = 0;
    const NPC_SPAWN_POINT* pSpawnPoint = NULL;
    NPC_LARGE_GROUP* pLargeGroup = NULL;
    NPC_MEDIUM_GROUP* pMediumGroup = NULL;
    const NPC_DATA* pNpcData = NULL;
    CAIProcessor* poAIProcessor = NULL;

    LOG_PROCESS_ERROR(poNpc);
    LOG_PROCESS_ERROR(poNpc->GetNpcData());

    //	poNpc->DisableBlock();

    CBaseList::Remove(&poNpc->GetRegionListNode());

    pSpawnPoint = poNpc->GetSpawnPoint();
    LOG_PROCESS_ERROR(pSpawnPoint);

    if (pSpawnPoint->nLargeGroupID >= 0 && pSpawnPoint->nLargeGroupID < MAX_NPC_LARGE_GROUP_COUNT)
        pLargeGroup = m_LargeGroupList + pSpawnPoint->nLargeGroupID;
    if (pSpawnPoint->nMediumGroupID >= 0 && pSpawnPoint->nMediumGroupID < MAX_NPC_MEDIUM_GROUP_PER_LARGE_GROUP)
        pMediumGroup = m_MediumGroupList + pSpawnPoint->nMediumGroupID;

    // template id
    nTemplateID = pSpawnPoint->RandNpc.Rand();
    LOG_PROCESS_ERROR(nTemplateID > 0);

    pNpcData = CDataManager<NPC_DATA>::GetInstance().GetData(nTemplateID);
    LOG_PROCESS_ERROR(pNpcData);

    nRetCode = poNpc->UnInit();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = poNpc->Init(CGameWorld::GetInstance().GetNpcID(poNpc), pSpawnPoint, 0);
    LOG_PROCESS_ERROR(nRetCode);

    poNpc->SetX(pSpawnPoint->nX + pSpawnPoint->nRange - g_Random(pSpawnPoint->nRange * 2));
    poNpc->SetY(pSpawnPoint->nY + pSpawnPoint->nRange - g_Random(pSpawnPoint->nRange * 2));
    poNpc->SetZ(pSpawnPoint->nZ);
    poNpc->SetFaceDirection(pSpawnPoint->nDirection);

    if (poNpc->GetSpawnPoint() && poNpc->GetSpawnPoint()->nReviveTime > 0)
    {
        poNpc->SetNpcTimer(CGameWorld::GetInstance().GetGameFrame() + poNpc->GetSpawnPoint()->nReviveTime);
        poNpc->IncControlCount(cclInvincible);
    }
    else if (pNpcData->nSpawnTime > 0)
    {
        poNpc->SetNpcTimer(CGameWorld::GetInstance().GetGameFrame() + pNpcData->nReviveTime);
        poNpc->IncControlCount(cclInvincible);
    }

    nRetCode = poNpc->SetTemplateData(nTemplateID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = AddNpc(poNpc, soatRevive);
    if (!nRetCode)
    {
        CRI("retry to respawn at original point");

        poNpc->SetX(pSpawnPoint->nX);
        poNpc->SetY(pSpawnPoint->nY);
        poNpc->SetZ(pSpawnPoint->nZ);

        nRetCode = AddNpc(poNpc, soatRevive);
        LOG_PROCESS_ERROR(nRetCode);
    }

    poNpc->SetHome(m_nSceneTempID, poNpc->GetX(), poNpc->GetY(), poNpc->GetZ());

    poAIProcessor = poNpc->GetAIProcessor();
    LOG_PROCESS_ERROR(poAIProcessor);

    poAIProcessor->CallInitBT();

    return TRUE;
Exit0:
    if (pSpawnPoint)
    {
        WRN("Failed to respawn npc %d @ scene %d (%d, %d, %d)", nTemplateID, m_nSceneTempID,
            pSpawnPoint->nX / POINT_PER_CM, pSpawnPoint->nY / POINT_PER_CM, pSpawnPoint->nZ / POINT_PER_CM);
    }
    else
    {
        WRN("Failed to respawn npc @ scene %d null point", m_nSceneTempID);
    }

    return FALSE;
}

BOOL CScene::AddNpcToRespawnList(CNpc* poNpc)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poNpc);

    nRetCode = RemoveNpc(poNpc);
    LOG_PROCESS_ERROR(nRetCode);

    m_lstRespawnNpc.PushHead(&poNpc->GetRegionListNode());
    poNpc->SetScene(this);

    return TRUE;
Exit0:
    return FALSE;
}

void CScene::IncreaseDieCount(CNpc* poNpc, INT32 nLargeGroupID, INT32 nMediumGroupID)
{
    INT32 nRetCode = 0;
    const NPC_LARGE_SPAWN_GROUP* pLargeGroup = NULL;
    const NPC_MEDIUM_SPAWN_GROUP* pMediumGroup = NULL;
    NPC_LARGE_GROUP& rLargeGroup = m_LargeGroupList[nLargeGroupID];
    NPC_MEDIUM_GROUP& rMediumGroup = m_MediumGroupList[nMediumGroupID];

    LOG_PROCESS_ERROR(poNpc);
    LOG_PROCESS_ERROR(nLargeGroupID >= 0 && nLargeGroupID < MAX_NPC_LARGE_GROUP_COUNT);
    LOG_PROCESS_ERROR(nMediumGroupID >= 0 && nMediumGroupID < MAX_NPC_MEDIUM_GROUP_PER_LARGE_GROUP);
    LOG_PROCESS_ERROR(m_poStaticData);

    pLargeGroup = m_poStaticData->GetNpcLargeGroup(nLargeGroupID);
    LOG_PROCESS_ERROR(pLargeGroup);

    pMediumGroup = m_poStaticData->GetNpcMediumGroup(nMediumGroupID);
    LOG_PROCESS_ERROR(pMediumGroup);

    rLargeGroup.nDieCount++;
    rMediumGroup.nDieCount++;

    if (m_poStaticData->GetScript())
    {
        if (rLargeGroup.nDieCount == pLargeGroup->nMaxDieCount)
        {
            if (pLargeGroup->pDeadBTRoot)
            {
                nRetCode = RunBT(evtLargeGroupDie, 0, pLargeGroup->pDeadBTRoot);
                LOG_CHECK_ERROR(nRetCode);
            }
        }

        if (rMediumGroup.nDieCount == rMediumGroup.nSpawnCount)
        {
            rMediumGroup.nSpawnCount++; // disable trigger again

            // run BT
            if (pMediumGroup->pDeadBTRoot)
            {
                nRetCode = RunBT(evtMediumGroupDie, 0, pMediumGroup->pDeadBTRoot);
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }

    return;
Exit0:

    return;
}

void CScene::AddNpcLiveTimer(UINT32 dwNpcID, UINT32 dwSuicideTime)
{
    NPC_LIVE_TIMER LiveTimer = { dwSuicideTime, dwNpcID };

    NPC_LIVE_TIMER_LIST::iterator itInsertPos = m_LiveTimerList.begin();
    for (; itInsertPos != m_LiveTimerList.end() && itInsertPos->dwSuicideTime < dwSuicideTime; ++itInsertPos);

    m_LiveTimerList.insert(itInsertPos, LiveTimer);
}

CDoodad* CScene::SpawnDoodad(INT32 nTemplateID, INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection, const DOODAD_SPAWN_POINT* pcSpawnPoint)
{
    CDoodad* poDoodad = NULL;
    INT32 nRetCode = 0;

    poDoodad = CGameWorld::GetInstance().NewDoodad();
    LOG_PROCESS_ERROR(poDoodad);

    nRetCode = poDoodad->Init(CGameWorld::GetInstance().GetDoodadID(poDoodad), nTemplateID);
    LOG_PROCESS_ERROR(nRetCode);

    poDoodad->SetX(llX);
    poDoodad->SetY(llY);
    poDoodad->SetZ(llZ);
    poDoodad->SetFaceDirection(nFaceDirection);
    poDoodad->SetSpawnPoint(pcSpawnPoint);

    nRetCode = AddDoodad(poDoodad);
    LOG_PROCESS_ERROR(nRetCode);

    return poDoodad;
Exit0:
    if (poDoodad)
    {
        poDoodad->UnInit();
        CGameWorld::GetInstance().DelDoodad(poDoodad);
    }

    return NULL;
}

CDoodad* CScene::SpawnDoodadPoint(const DOODAD_SPAWN_POINT* pcSpawnPoint)
{
    CDoodad* poDoodad = NULL;
    INT32 nRetCode = 0;
    const DOODAD_SPAWN_GROUP* pcSpawnGroup = NULL;
    INT32 nTemplateID;

    pcSpawnGroup = m_poStaticData->GetDoodadSpawnGroup(pcSpawnPoint->nGroupID);
    LOG_PROCESS_ERROR(pcSpawnGroup);

    nTemplateID = pcSpawnGroup->RandDoodad.Rand();

    poDoodad = SpawnDoodad(nTemplateID,
        pcSpawnPoint->nX, pcSpawnPoint->nY, pcSpawnPoint->nZ, pcSpawnPoint->nFaceDirection, pcSpawnPoint);
    LOG_PROCESS_ERROR(poDoodad);

    return poDoodad;
Exit0:
    if (poDoodad)
    {
        poDoodad->UnInit();
        CGameWorld::GetInstance().DelDoodad(poDoodad);
    }

    return NULL;
}

BOOL CScene::SpawnDoodadGroup(INT32 nGroupID)
{
    INT32 nRetCode = 0;
    const DOODAD_SPAWN_GROUP* pcSpawnGroup = NULL;
    CDoodad* poDoodad = NULL;

    LOG_PROCESS_ERROR(m_poStaticData);

    pcSpawnGroup = m_poStaticData->GetDoodadSpawnGroup(nGroupID);
    LOG_PROCESS_ERROR(pcSpawnGroup);

    for (INT32 nIndex = 0; nIndex < pcSpawnGroup->nMaxConcurrentPoint; nIndex++)
    {
        INT32 nPointID = _GetRandDoodadPoint(pcSpawnGroup);
        LOG_PROCESS_ERROR(nPointID >= 0);

        poDoodad = SpawnDoodadPoint(pcSpawnGroup->pPointList + nPointID);
        LOG_CHECK_ERROR(poDoodad);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::AddDoodadToRespawnList(CDoodad* poDoodad)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poDoodad);
    LOG_PROCESS_ERROR(m_poStaticData);

    nRetCode = RemoveDoodad(poDoodad);
    LOG_PROCESS_ERROR(nRetCode);

    m_lstRespawnDoodad.PushHead(&poDoodad->GetRegionListNode());

    poDoodad->SetScene(this);

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CScene::RespawnDoodad(CDoodad* poDoodad)
{
    INT32 nRetCode = 0;
    const DOODAD_SPAWN_GROUP* pcSpawnGroup = NULL;
    INT32 nPointID;

    LOG_PROCESS_ERROR(poDoodad);
    LOG_PROCESS_ERROR(poDoodad->GetSpawnPoint());
    LOG_PROCESS_ERROR(m_poStaticData);

    CBaseList::Remove(&poDoodad->GetRegionListNode());

    pcSpawnGroup = m_poStaticData->GetDoodadSpawnGroup(poDoodad->GetSpawnPoint()->nGroupID);
    LOG_PROCESS_ERROR(pcSpawnGroup);

    nPointID = _GetRandDoodadPoint(pcSpawnGroup);
    LOG_PROCESS_ERROR(nPointID >= 0);

    poDoodad->SetSpawnPoint(pcSpawnGroup->pPointList + nPointID);

    nRetCode = poDoodad->SetTemplateID(pcSpawnGroup->RandDoodad.Rand());
    LOG_PROCESS_ERROR(nRetCode);

    poDoodad->SetX(pcSpawnGroup->pPointList[nPointID].nX);
    poDoodad->SetY(pcSpawnGroup->pPointList[nPointID].nY);
    poDoodad->SetZ(pcSpawnGroup->pPointList[nPointID].nZ);
    poDoodad->SetFaceDirection(pcSpawnGroup->pPointList[nPointID].nFaceDirection);

    nRetCode = AddDoodad(poDoodad);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (poDoodad)
    {
        poDoodad->UnInit();
        nRetCode = CGameWorld::GetInstance().DelDoodad(poDoodad);
        LOG_CHECK_ERROR(nRetCode);
    }
    return FALSE;
}

BOOL CScene::GetLowerGrid(INT64 llX, INT64 llY, INT64 llZ, const ZONE_DATA*& rpcZoneData, const GRID_DATA*& rpcGridData)
{
    LOG_PROCESS_ERROR(m_poStaticData);

    return m_poStaticData->GetLowerGrid(llX, llY, llZ, rpcZoneData, rpcGridData);
Exit0:
    return FALSE;
}

INT32 CScene::GetLowerZ(INT64 llX, INT64 llY, INT64 llZ)
{
    LOG_PROCESS_ERROR(m_poStaticData);

    return m_poStaticData->GetLowerZ(llX, llY, llZ);
Exit0:
    return NULL;
}

INT32 CScene::GetNearestAvailablePos(INT32 nCenterX, INT32 nCenterY, INT32 nCenterZ, INT32 nMaxRange,
    INT32 nPosCount, INT32 nPos[][3])
{
    LOG_PROCESS_ERROR(m_poStaticData);

    return m_poStaticData->GetNearestAvailablePos(nCenterX, nCenterY, nCenterZ, nMaxRange, nPosCount, nPos);
Exit0:
    return 0;
}

void CScene::SetDoorState(INT32 nDoorID, BOOL bClosed)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(nDoorID >= 0 && nDoorID < MAX_DOOR_ID);

    if (bClosed)
        m_qwDoorTable |= (1LL << nDoorID);
    else
        m_qwDoorTable &= ~(1LL << nDoorID);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncDoorState(NULL, this);
    LOG_PROCESS_ERROR(nRetCode);
#endif  // _ROBOT

Exit0:
    return;
}

BOOL CScene::IsDoorClosed(INT32 nDoorID)
{
    LOG_PROCESS_ERROR(nDoorID >= 0 && nDoorID < MAX_DOOR_ID);

    return (m_qwDoorTable & (1LL << nDoorID)) > 0;
Exit0:
    return TRUE;
}

void CScene::SetVariable(INT32 nIndex, INT32 nValue)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_SCENE_VARIABLE_COUNT);

    m_nVarList[nIndex] = nValue;
#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoUpdateSceneVariable(this, nIndex);
    LOG_PROCESS_ERROR(nRetCode);
#endif  // _ROBOT

    DBG("set scene %d-%d variable %d to %d", m_nSceneTempID, m_nSceneID, nIndex, nValue);

Exit0:
    return;
}

BOOL CScene::IsObstacled(CSceneObject* poSrcObj, CSceneObject* poDestObj, BOOL bCheckBlockMove)
{
    LOG_PROCESS_ERROR(poSrcObj);
    LOG_PROCESS_ERROR(poDestObj);
    LOG_PROCESS_ERROR(poSrcObj->GetScene() == poDestObj->GetScene());

    return IsObstacled(poSrcObj->GetX(), poSrcObj->GetY(), poSrcObj->GetZ(), poDestObj->GetX(), poDestObj->GetY(), poDestObj->GetZ(), bCheckBlockMove);
Exit0:
    return TRUE;
}

// Check whether this is an obstacle point?
BOOL CScene::IsObstacled(INT64 llSrcX, INT64 llSrcY, INT64 llSrcZ, INT64 llDstX, INT64 llDstY, INT64 llDstZ, BOOL bCheckBlockMove)
{
    INT32 nRetCode = 0;
    const GRID_DATA* pSrcGrid = NULL;
    const ZONE_DATA* pSrcZone = NULL;
    const GRID_DATA* pDstGrid = NULL;
    const ZONE_DATA* pDstZone = NULL;
    const GRID_DATA* pCurGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    INT64 llStepX = 0, llStepY = 0, llStepZ = 0;
    INT32 nSrcZoneX = 0, nSrcZoneY = 0, nDstZoneX = 0, nDstZoneY = 0;
    INT32 nRevertDirection = g_GetDirection(llDstX, llDstY, llSrcX, llSrcY);

    llSrcX = llSrcX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    llSrcY = llSrcY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;

    llDstX = llDstX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    llDstY = llDstY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;

    if (llSrcX == llDstX && llSrcY == llDstY)
        return FALSE;

    llSrcZ += ROLE_HEIGHT * 2;
    llDstZ += ROLE_HEIGHT * 2;

    INT64 llDetX = llDstX - llSrcX;
    INT64 llDetY = llDstY - llSrcY;
    INT64 llDetZ = llDstZ - llSrcZ;

    if (llDetX == 0 && llDetY == 0)
        return FALSE;

    if (g_Abs(llDetX) >= g_Abs(llDetY))
    {
        llStepX = llDetX >= 0 ? GRID_WIDTH : -GRID_WIDTH;
        llStepY = llDetY * llStepX / llDetX;
    }
    else
    {
        llStepY = llDetY >= 0 ? GRID_WIDTH : -GRID_WIDTH;
        llStepX = llDetX * llStepY / llDetY;
    }

    while (llSrcX != llDstX && llSrcY != llDstY)
    {
        const ZONE_DATA* pZoneData = NULL;

        nRetCode = GetLowerGrid(llSrcX, llSrcY, llSrcZ, pZoneData, pCurGrid);
        LOG_PROCESS_ERROR(nRetCode);

        PROCESS_ERROR(!pCurGrid->bBlockBullet);

        pGridInstance = GetGridInstance(pZoneData, pCurGrid);
        LOG_PROCESS_ERROR(nRetCode);

        if (bCheckBlockMove)
        {
            PROCESS_ERROR(!pCurGrid->bOutOfPoly);
            if (pCurGrid->uDoor > 0)
                PROCESS_ERROR(!IsDoorClosed(pZoneData->byDoor[pCurGrid->uDoor]));
            PROCESS_ERROR(pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
        }

        do
        {
            PROCESS_ERROR(llSrcZ < pCurGrid->uLowLayer * GRID_LAYER_HEIGHT || llSrcZ > pCurGrid->uHighLayer * GRID_LAYER_HEIGHT);

            pCurGrid = &pZoneData->GridList[pCurGrid->uNextGrid];

        } while (pCurGrid->uNextGrid != 0);

        llSrcX += llStepX;
        llSrcY += llStepY;
    }

    return FALSE;

Exit0:
    return TRUE;


#ifdef PERFORMANCE_OBSTACLED
    gs_dwTempObstacleCount++;
    gs_qwTotalObstacleCount++;
    UINT64 qwCurTick = GetTickCountEx();

    if (gs_qwLastPrintObstacleTickCount < qwCurTick)
    {
        INT32 nValue = gs_dwTempObstacleCount / 60;
        CRI("IsObstacled total count %lld, total count: %d in a minute, %d per second",
            gs_qwTotalObstacleCount, gs_dwTempObstacleCount, nValue);

        gs_dwTempObstacleCount = 0;
        gs_qwLastPrintObstacleTickCount = qwCurTick + 1000 * 60;
    }
#endif

#ifndef _ROBOT
    g_ChangeStatisInfo(statIsObstacledCount, 1);
#endif

#if 0

    llSrcX = llSrcX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    llSrcY = llSrcY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;

    llDstX = llDstX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    llDstY = llDstY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;

    if (llSrcX == llDstX && llSrcY == llDstY)
        return FALSE;

    INT64 nGridSrcXInZone = (llSrcX / GRID_WIDTH) % GRID_PER_ZONE;
    INT64 nGridSrcYInZone = (llSrcY / GRID_WIDTH) % GRID_PER_ZONE;

    INT64 nGridDstXInZone = (llDstX / GRID_WIDTH) % GRID_PER_ZONE;
    INT64 nGridDstYInZone = (llDstY / GRID_WIDTH) % GRID_PER_ZONE;

    INT64 llDetX = llDstX - llSrcX;
    INT64 llDetY = llDstY - llSrcY;
    INT64 llDetZ = llDstZ - llSrcZ;

    if (llDetX == 0 && llDetY == 0)
        return FALSE;

    nSrcZoneX = (INT32)(llSrcX / ZONE_WIDTH);
    nSrcZoneY = (INT32)(llSrcY / ZONE_WIDTH);

    nDstZoneX = (INT32)(llDstX / ZONE_WIDTH);
    nDstZoneY = (INT32)(llDstY / ZONE_WIDTH);

    const ZONE_DATA* pZoneData = m_poStaticData->GetZoneData(nSrcZoneX, nSrcZoneY);
    LOG_PROCESS_ERROR(pZoneData != NULL);

    pSrcGrid = &pZoneData->GridList[pZoneData->GroundGrid[nGridSrcXInZone][nGridSrcYInZone]];

    pZoneData = m_poStaticData->GetZoneData(nDstZoneX, nDstZoneY);
    PROCESS_ERROR(pZoneData != NULL);

    pDstGrid = &pZoneData->GridList[pZoneData->GroundGrid[nGridDstXInZone][nGridDstYInZone]];

    if (g_Abs(llDetX) >= g_Abs(llDetY))
    {
        llStepX = llDetX >= 0 ? GRID_WIDTH : -GRID_WIDTH;
        llStepY = llDetY * llStepX / llDetX;
        //llStepZ = llDetZ * llStepX / llDetX;
    }
    else
    {
        llStepY = llDetY >= 0 ? GRID_WIDTH : -GRID_WIDTH;
        llStepX = llDetX * llStepY / llDetY;
        //llStepZ = llDetZ * llStepY / llDetY;
    }

    pCurGrid = pSrcGrid;

    while (nSrcZoneX != nDstZoneX || nSrcZoneY != nDstZoneY || nGridSrcXInZone != nGridDstXInZone || nGridSrcYInZone != nGridDstYInZone)
    {
        if (nSrcZoneX == nDstZoneX && nSrcZoneY == nDstZoneY)
        {
            if (nGridSrcXInZone == nGridDstXInZone)
                llStepX = 0;
            if (nGridSrcYInZone == nGridDstYInZone)
                llStepY = 0;
        }

        llSrcX += llStepX;
        llSrcY += llStepY;

        //llSrcZ += llStepZ;

        nSrcZoneX = (INT32)(llSrcX / ZONE_WIDTH);
        nSrcZoneY = (INT32)(llSrcY / ZONE_WIDTH);

        nGridSrcXInZone = (llSrcX / GRID_WIDTH) % GRID_PER_ZONE;
        nGridSrcYInZone = (llSrcY / GRID_WIDTH) % GRID_PER_ZONE;

        const ZONE_DATA* pZoneData = m_poStaticData->GetZoneData((INT32)(llSrcX / ZONE_WIDTH), (INT32)(llSrcY / ZONE_WIDTH));
        LOG_PROCESS_ERROR(pZoneData != NULL);

        //		DBG("Current Zone: X=%d, Y=%d", nSrcZoneX , nSrcZoneY);
        //		DBG("--Current Grid: X=%d, Y=%d", nGridSrcXInZone, nGridSrcYInZone);

        pCurGrid = &pZoneData->GridList[pZoneData->GroundGrid[nGridSrcXInZone][nGridSrcYInZone]];
        PROCESS_ERROR(!pCurGrid->bBlockBullet);

        pGridInstance = GetGridInstance(pZoneData, pCurGrid);
        LOG_PROCESS_ERROR(pGridInstance);

        if (bCheckBlockMove)
        {
            PROCESS_ERROR(!pCurGrid->bOutOfPoly);
            if (pCurGrid->uDoor > 0)
                PROCESS_ERROR(!IsDoorClosed(pZoneData->byDoor[pCurGrid->uDoor]));
            PROCESS_ERROR(pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
        }

        do
        {
            PROCESS_ERROR(llSrcZ < pCurGrid->uLowLayer * GRID_LAYER_HEIGHT || llSrcZ > pCurGrid->uHighLayer * GRID_LAYER_HEIGHT);

            pCurGrid = &pZoneData->GridList[pCurGrid->uNextGrid];

        } while (pCurGrid->uNextGrid != 0);
    }

    return FALSE;

Exit0:
    return TRUE;
#endif
}

BOOL CScene::TouchVolume(CRole* poRole, INT32 nVolumeID)
{
    INT32 nRetCode = 0;
    BTNod* pBTNod = NULL;

    LOG_PROCESS_ERROR(nVolumeID >= 0 && nVolumeID < MAX_VOLUME_COUNT);

    if (nVolumeID >= 0 &&
        (m_byVolumeTriggerCount[nVolumeID] < m_poStaticData->GetMaxVolumeTriggerCount(nVolumeID) ||
            m_poStaticData->GetMaxVolumeTriggerCount(nVolumeID) == 0))
    {
        m_byVolumeTriggerCount[nVolumeID]++;

        pBTNod = m_poStaticData->GetVolumeTree(nVolumeID);
        if (pBTNod)
        {
            nRetCode = RunBT(evtTouchVolume, poRole->GetObjID(), pBTNod);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::RunBT(INT32 eEventType, UINT32 dwRoleID, BTNod* pRoot)
{
    INT32 nRetCode = 0;

    BTCustomCtx* pBTCtx = CGameWorld::GetInstance().NewBTCtx();
    LOG_PROCESS_ERROR(pBTCtx);

    memset(pBTCtx->callFrame, 0, sizeof(pBTCtx->callFrame));
    pBTCtx->callFrame[0].nod = pRoot;
    pBTCtx->callFrame[0].child = NULL;
    pBTCtx->callFrameCount = 1;
    pBTCtx->debugging = 0;
    pBTCtx->poScript = m_poStaticData->GetScript();

    pBTCtx->eEventType = eEventType;
    pBTCtx->dwRoleID = dwRoleID;
    pBTCtx->eOwnerType = botScene;
    pBTCtx->poScene = this;

    BTCtx_run(*pBTCtx);

    if (pBTCtx->returnValue == brvRunning)
    {
        pBTCtx->ListNode.pPrev = pBTCtx->ListNode.pNext = NULL;

        _PushBTCtx(pBTCtx);
    }
    else
    {
        nRetCode = CGameWorld::GetInstance().DelBTCtx(pBTCtx);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    if (pBTCtx)
    {
        nRetCode = CGameWorld::GetInstance().DelBTCtx(pBTCtx);
        LOG_PROCESS_ERROR(nRetCode);
    }
    return FALSE;
}

void CScene::_PushBTCtx(BTCustomCtx* pBTCtx)
{
    m_lstBTCtx.PushRear(&pBTCtx->ListNode);
}

BOOL CScene::_ProcessSyncRun(BTCustomCtx* pBTCtx)
{
    INT32 nRetCode = 0;

    switch (pBTCtx->eRunningType)
    {
    case brtWait:
        if (pBTCtx->nWaitFrame <= CGameWorld::GetInstance().GetGameFrame())
        {
            pBTCtx->returnValue = brvSuccess;   // timer out, action success

            BTCtx_run(*pBTCtx);

            if (pBTCtx->returnValue != brvRunning)
            {
                CBaseList::Remove(&pBTCtx->ListNode);

                nRetCode = CGameWorld::GetInstance().DelBTCtx(pBTCtx);
                LOG_PROCESS_ERROR(nRetCode);
            }
        }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CBaseList::Remove(&pBTCtx->ListNode);

    nRetCode = CGameWorld::GetInstance().DelBTCtx(pBTCtx);
    LOG_CHECK_ERROR(nRetCode);

    return FALSE;
}

BOOL CScene::Say(UINT32 dwTargetID, UINT32 dwLocation, const char* pcszMessage)
{
    INT32  nResult = errUnknownError;
    INT32  nRetCode = 0;
    INT32  nMessageSize = 0;

#ifndef _ROBOT

    LOG_PROCESS_ERROR(pcszMessage);
    nMessageSize = (INT32)strnlen(pcszMessage, MAX_CHAT_MESSAGE_LEN) + 1;
    LOG_PROCESS_ERROR(pcszMessage[nMessageSize - 1] == '\0');
    LOG_PROCESS_ERROR(dwLocation > cltInvalid && dwLocation < cltTotal);

    switch (GetTypeFromID(dwTargetID))
    {
    case otChannel:
    {
        switch (dwTargetID)
        {
        case cctScene:
        {
            nRetCode = CCliMessageHandler::GetInstance().DoChatMessage(
                this, "", CHAT_SYSTEM_ID, dwTargetID, dwLocation, pcszMessage, nMessageSize);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
        break;
        case cctServer:
        {
            nRetCode = CCliMessageHandler::GetInstance().DoMSChatMessage(
                CHAT_SYSTEM_ID, "", dwTargetID, dwLocation, 0, FALSE, pcszMessage, nMessageSize);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
        break;
        case cctGroup:
        {
            nRetCode = CCliMessageHandler::GetInstance().DoMSChatMessage(
                CHAT_SYSTEM_ID, "", dwTargetID, dwLocation, FALSE, m_nServerGroupID, pcszMessage, nMessageSize);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
        break;
        case cctScreen:
        default:
            break;
        }
    }
    break;

    case otRole:
    case otGuild:
    case otTeam:
    {
        nRetCode = CCliMessageHandler::GetInstance().DoMSChatMessage(
            CHAT_SYSTEM_ID, "", dwTargetID, dwLocation, 0, FALSE, pcszMessage, nMessageSize);
        LOG_PROCESS_ERROR(nRetCode);
    }
    break;

    case otNpc:
    case otDoodad:
    case otMissile:
    case otDrop:
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return errSuccess;
#endif

Exit0:
    return errUnknownError;
}

void CScene::_AttachCombatStatNode(CRole* poRole)
{
    INT32 nRetCode = 0;
    COMBAT_STAT_NODE* pStatNode = NULL;

    PROCESS_ERROR(m_pcSceneData);

    if (m_pcSceneData->nType != estStatic && m_pcSceneData->nType != estLine &&
        m_pcSceneData->nType != estBirth && m_pcSceneData->nType != estUnique)
    {
        // try to find combat stat node
        pStatNode = (COMBAT_STAT_NODE*)m_CombatStatList.Head().pNext;
        while (pStatNode != &(m_CombatStatList.Rear()))
        {
            if (pStatNode->dwRoleID == poRole->GetObjID())
            {
                break;
            }

            pStatNode = (COMBAT_STAT_NODE*)pStatNode->pNext;
        }

        if (pStatNode == &(m_CombatStatList.Rear()))
        {
            pStatNode = CGameWorld::GetInstance().NewSceneCombatStatNode();
            LOG_PROCESS_ERROR(pStatNode);

            memset(pStatNode, 0, sizeof(COMBAT_STAT_NODE));
            pStatNode->dwRoleID = poRole->GetObjID();
            pStatNode->byFaction = poRole->GetFaction();

            nRetCode = m_CombatStatList.PushRear(pStatNode);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    poRole->SetCombatStatNode(pStatNode);

    return;
Exit0:
    if (pStatNode)
    {
        CBaseList::Remove(pStatNode);
        poRole->SetCombatStatNode(NULL);
        CGameWorld::GetInstance().DelSceneCombatStatNode(pStatNode);
    }

    return;
}



BOOL CScene::FindValidPos(CSceneObject* poSceneObjcet, BOOL bStacked, const ZONE_DATA*& rpcZone, const GRID_DATA*& rpcGrid,
    GRID_INSTANCE_DATA*& rpGridInstance, CRegion*& rpRegion)
{
    INT32 nRetCode = 0;

    INT64 llX, llY, llZ;
    INT32 nRegionX = 0;
    INT32 nRegionY = 0;

    INT32 nPosIndex = 0;
    while (nPosIndex < sizeof(s_nLocationOffsetTable) / sizeof(s_nLocationOffsetTable[0]))
    {
        llX = poSceneObjcet->GetX() + s_nLocationOffsetTable[nPosIndex][0] * 2;
        llY = poSceneObjcet->GetY() + s_nLocationOffsetTable[nPosIndex][1] * 2;
        llZ = poSceneObjcet->GetZ() + GRID_WIDTH;

        nRetCode = GetLowerGrid(llX, llY, llZ, rpcZone, rpcGrid);
        PROCESS_ERROR(nRetCode);
        PROCESS_ERROR(!rpcGrid->bOutOfPoly);

        rpGridInstance = GetGridInstance(rpcZone, rpcGrid);
        LOG_PROCESS_ERROR(rpGridInstance);
        PROCESS_ERROR(rpGridInstance->uCharRef < MAX_CHAR_PER_GRID);

        if (!bStacked)
        {
            nRetCode = poSceneObjcet->CheckGridCharRef(this, llX, llY, llZ);
            PROCESS_ERROR(nRetCode);
        }

        nRegionX = llX / REGION_WIDTH;
        nRegionY = llY / REGION_WIDTH;

        rpRegion = GetRegion(nRegionX, nRegionY);
        LOG_PROCESS_ERROR(rpRegion);

        poSceneObjcet->SetX(llX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2);
        poSceneObjcet->SetY(llY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2);
        poSceneObjcet->SetZ(llZ);

        return TRUE;
    Exit0:
        nPosIndex++;
        continue;
    }

    rpcZone = NULL;
    rpcGrid = NULL;
    rpGridInstance = NULL;
    rpRegion = NULL;

    INF("CScene::_FindValidPos for obj %d failed, scene tempID=%d, id=%d, x=%d, y=%d, z=%d", poSceneObjcet->GetObjID(), GetSceneTempID(),
        GetSceneID(), poSceneObjcet->GetX() / POINT_PER_CM, poSceneObjcet->GetY() / POINT_PER_CM, poSceneObjcet->GetZ() / POINT_PER_CM);

    return FALSE;
}

BOOL CScene::SetWeather(INT32 nWeather, INT32 nDurationFrame)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(nWeather > swtInvalid && nWeather < swtTotal);

    m_nWeather = nWeather;
    m_nWeatherEndFrame = CGameWorld::GetInstance().GetGameFrame() + nDurationFrame;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoUpdateSceneWeather(this);
    LOG_CHECK_ERROR(nRetCode);
#endif	// _ROBOT

    return TRUE;
Exit0:
    return FALSE;
}


CNpc* CScene::FindNpc(INT32 nSingleNpcID)
{
    PROCESS_ERROR(nSingleNpcID >= 0 && nSingleNpcID < MAX_SINGLE_NPC_COUNT);

    return CGameWorld::GetInstance().FindNpc(m_SingleNpcObjID[nSingleNpcID]);
Exit0:
    return NULL;
}

void CScene::CheckDungeonGoal(CRole* poRole)
{
#ifndef _ROBOT
    INT32 nRetCode = 0;
    const DUNGEON_DATA* pDungeonData = NULL;

    pDungeonData = CDataManager<DUNGEON_DATA>::GetInstance().GetData(m_nSceneTempID);
    LOG_PROCESS_ERROR(pDungeonData);

    if (poRole->GetCombatStat(escsBossKillCount) >= pDungeonData->nStandardBossValue ||
        poRole->GetCombatStat(escsEliteNpcKillCount) >= pDungeonData->nStandardEliteNpcValue)
    {
        nRetCode = CCliMessageHandler::GetInstance().DoSyncDungeonGoal(poRole);
        LOG_PROCESS_ERROR(nRetCode);
    }
Exit0:
    return;
#endif	// _ROBOT
}

BOOL CScene::_CheckGrid(INT64 llX, INT64 llY, INT64 llZ, GRID_INSTANCE_DATA* pSelfGridInstance)
{
    INT32 nRetCode = 0;
    const ZONE_DATA* pcZoneData = NULL;
    const GRID_DATA* pcGridData = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    nRetCode = GetLowerGrid(llX, llY, llZ + GRID_WIDTH, pcZoneData, pcGridData);
    PROCESS_ERROR(nRetCode);
    PROCESS_ERROR(!pcGridData->bOutOfPoly);

    pGridInstance = GetGridInstance(pcZoneData, pcGridData);
    LOG_PROCESS_ERROR(pGridInstance);
    if (!pcGridData->bNotBlockChar && pGridInstance != pSelfGridInstance)
        PROCESS_ERROR(pGridInstance->uCharRef == 0);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CScene::CheckAdjGrid(INT64 llX, INT64 llY, INT64 llZ, GRID_INSTANCE_DATA* pSelfGridInstance)
{
    INT32 nRetCode = 0;
    static const INT32 s_nDirectionTable[4][3][2] =
    {
        { { GRID_WIDTH, 0 }, { 0, GRID_WIDTH }, { 0,-GRID_WIDTH } },
        { { 0, GRID_WIDTH }, {-GRID_WIDTH, 0 }, { GRID_WIDTH, 0 } },
        { { 0,-GRID_WIDTH }, { GRID_WIDTH, 0 }, {-GRID_WIDTH, 0 } },
        { {-GRID_WIDTH, 0 }, { 0,-GRID_WIDTH }, { 0, GRID_WIDTH } },
    };
    INT32 nPointX = (INT32)(llX % GRID_WIDTH);
    INT32 nPointY = (INT32)(llY % GRID_WIDTH);
    INT32 nPart = ((nPointX + nPointY < GRID_WIDTH) * 2 + (nPointY > nPointX));

    nRetCode = _CheckGrid(llX + s_nDirectionTable[nPart][0][0], llY + s_nDirectionTable[nPart][0][1], llZ, pSelfGridInstance);
    if (!nRetCode)
    {
        nRetCode = _CheckGrid(llX + s_nDirectionTable[nPart][1][0], llY + s_nDirectionTable[nPart][1][1], llZ, pSelfGridInstance);
        PROCESS_ERROR(nRetCode);

        nRetCode = _CheckGrid(llX + s_nDirectionTable[nPart][2][0], llY + s_nDirectionTable[nPart][2][1], llZ, pSelfGridInstance);
        PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

#ifndef _ROBOT
BOOL CScene::CTraverseRoleForSabac::operator()(CRole* poRole)
{
    if (poRole->IsDead())
        return TRUE;

    if (poRole->GetGMLevel() > 0)
        return TRUE;

    BOOL bInZone = g_PointInCube(
        poRole->GetX() / POINT_PER_CM, poRole->GetY() / POINT_PER_CM, poRole->GetZ() / POINT_PER_CM,
        g_GWConfig.Sabac.OccupiedZoneLeftTop.x,
        g_GWConfig.Sabac.OccupiedZoneLeftTop.y,
        g_GWConfig.Sabac.OccupiedZoneLeftTop.z,
        g_GWConfig.Sabac.OccupiedZoneRightBottom.x,
        g_GWConfig.Sabac.OccupiedZoneRightBottom.y,
        g_GWConfig.Sabac.OccupiedZoneRightBottom.z);
    if (!bInZone)
        return TRUE;

    if (poRole->GetGuildID() == INVALID_OBJECT_ID)
        return FALSE;

    CGSGuild* poGSGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    if (poGSGuild == NULL || (!poGSGuild->IsJoinSabacToday() && !poGSGuild->IsOccupiedSabac()))
        return FALSE;

    if (dwGuildID == 0)
    {
        dwGuildID = poRole->GetGuildID();
        return TRUE;
    }

    if (poRole->GetGuildID() == dwGuildID)
        return TRUE;

    return FALSE;
}

void CScene::_DeliverSabacAward(UINT32 dwGuildID)
{
    UINT32 dwRoleID = INVALID_OBJECT_ID;
    INT32 nRetCode = 0;

    CGSGuild* poGSGuild = CGuildMgr::GetInstance().FindGuild(dwGuildID);
    LOG_PROCESS_ERROR(poGSGuild);

    dwRoleID = poGSGuild->GetFirstMember();
    while (dwRoleID != INVALID_OBJECT_ID)
    {
        BOOL bAddedItem = FALSE;
        INT32 nItemID = g_GWConfig.Sabac.nMemberAwardItemID;

        if (dwRoleID == poGSGuild->GetLeaderID())
            nItemID = g_GWConfig.Sabac.nLeaderAwardItemID;

        CRole* poRole = CGameWorld::GetInstance().FindValidRole(dwRoleID);
        if (poRole)
        {
            nRetCode = poRole->GetItemBag()->AddItem(nItemID, 1, ITEMLOG_SABAC_AWARD, 0, FALSE);
            if (nRetCode == errSuccess)
                bAddedItem = TRUE;
        }

        if (!bAddedItem)
        {
            char szMailTitle[COMMON_NAME_LEN];
            char szMailContent[COMMON_NAME_LEN];
            snprintf(szMailTitle, COMMON_NAME_LEN, "<#T:%d>", g_GWConfig.Sabac.nAwardMailTitleStringID);
            snprintf(szMailContent, COMMON_NAME_LEN, "<#T:%d>", g_GWConfig.Sabac.nAwardMailContentStringID);

            nRetCode = CCliMessageHandler::GetInstance().DoMSSendSystemMailWithSimpleAttach(
                dwRoleID, szMailTitle, szMailContent, nItemID, 1);
            if (!nRetCode)
                CRI("failed to deliver sabac award to role %d", dwRoleID);
        }

        dwRoleID = poGSGuild->GetNextMember();
    }

    return;
Exit0:
    CRI("failed to deliver sabac award to guild %d", dwGuildID);
    return;
}

void CScene::_OnSabacMainLoop(void)
{
    INT32 nRetCode = 0;
    char szMessage[MAX_CHAT_MESSAGE_LEN];

    const tm_ex t = CGameWorld::GetInstance().GetLocalTime();
    INT32 nCurrentTime = t.tm_hour * 100 + t.tm_min;

    if (m_bSabacStarted)
    {
        if (nCurrentTime < g_GWConfig.Sabac.nEndTime)
        {
            if (nCurrentTime >= g_GWConfig.Sabac.nStartOccupyTime)
            {
                BOOL bFound = FALSE;
                INT32 nStartRegionX = g_GWConfig.Sabac.OccupiedZoneLeftTop.x / (REGION_WIDTH / POINT_PER_CM);
                INT32 nStartRegionY = g_GWConfig.Sabac.OccupiedZoneLeftTop.y / (REGION_WIDTH / POINT_PER_CM);
                INT32 nEndRegionX = g_GWConfig.Sabac.OccupiedZoneRightBottom.x / (REGION_WIDTH / POINT_PER_CM) + 1;
                INT32 nEndRegionY = g_GWConfig.Sabac.OccupiedZoneRightBottom.y / (REGION_WIDTH / POINT_PER_CM) + 1;
                UINT32 dwSabacOccupyGuildID = 0;

                for (INT32 nRegionX = nStartRegionX; nRegionX <= nEndRegionX && !bFound; nRegionX++)
                {
                    for (INT32 nRegionY = nStartRegionY; nRegionY <= nEndRegionY && !bFound; nRegionY++)
                    {
                        CRegion* poRegion = GetRegion(nRegionX, nRegionY);
                        if (poRegion && poRegion->IsActive(CGameWorld::GetInstance().GetGameFrame()))
                        {
                            CTraverseRoleForSabac TraverseRoleForSabac;

                            TraverseRoleForSabac.dwGuildID = dwSabacOccupyGuildID;

                            nRetCode = poRegion->TraverseRole(TraverseRoleForSabac);
                            if (nRetCode)
                            {
                                if (dwSabacOccupyGuildID == 0)
                                {
                                    dwSabacOccupyGuildID = TraverseRoleForSabac.dwGuildID;
                                }
                            }
                            else
                            {
                                dwSabacOccupyGuildID = 0;
                                bFound = TRUE;
                            }
                        }
                    }
                }

                if (dwSabacOccupyGuildID != 0 &&
                    dwSabacOccupyGuildID != CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID))
                {
                    if (m_poStaticData->GetBT(evtSabacOccupied))
                    {
                        nRetCode = RunBT(evtSabacOccupied, 0, m_poStaticData->GetBT(evtSabacOccupied));
                        LOG_CHECK_ERROR(nRetCode);
                    }

                    nRetCode = CCliMessageHandler::GetInstance().DoMSOccupySabac(m_nServerGroupID,
                        CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID), dwSabacOccupyGuildID);
                    LOG_CHECK_ERROR(nRetCode);

                    CSabacMgr::GetInstance().SetOccupyGuildID(m_nServerGroupID, dwSabacOccupyGuildID);

                    //nRetCode = CCliMessageHandler::GetInstance().DoOccupySabac(
                    //	m_nServerGroupID, m_dwSabacOwnerGuildID, dwSabacOccupyGuildID);
                    //LOG_CHECK_ERROR(nRetCode);
                }

                if (!m_bSabacEventTriggered && m_poStaticData->GetBT(evtSabacBeforeEnd))
                {
                    UINT32 dwTime = CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone();
                    UINT32 dwEndTime = g_GWConfig.Sabac.nEndTime / 100 * 3600 + (g_GWConfig.Sabac.nEndTime % 100) * 60;
                    dwEndTime = dwTime - (dwTime % 86400) + dwEndTime;
                    if (dwEndTime <= dwTime)
                        dwEndTime += 86400;
                    INT32 nRemainTime = dwEndTime - dwTime;
                    if (nRemainTime <= 10 * 60)
                    {
                        nRetCode = RunBT(evtSabacBeforeEnd, 0, m_poStaticData->GetBT(evtSabacBeforeEnd));
                        LOG_CHECK_ERROR(nRetCode);

                        m_bSabacEventTriggered = TRUE;
                    }
                }
            }
        }
        else
        {
            if (CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID) > 0)
                _DeliverSabacAward(CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID));

            nRetCode = CCliMessageHandler::GetInstance().DoMSOccupySabac(m_nServerGroupID,
                CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID),
                CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID));
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CCliMessageHandler::GetInstance().DoSabacStateChange(m_nServerGroupID, FALSE);
            LOG_CHECK_ERROR(nRetCode);

            SetVariable(8, FALSE);
            m_bSabacStarted = FALSE;
            m_bSabacEventTriggered = FALSE;

            if (m_poStaticData->GetBT(evtSabacEnd))
            {
                nRetCode = RunBT(evtSabacEnd, 0, m_poStaticData->GetBT(evtSabacEnd));
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }
    else if (g_GWConfig.Sabac.bEnabled && CSabacMgr::GetInstance().HasEnrolledGuildToday(m_nServerGroupID))
    {
        if (!m_bSabacEventTriggered && m_poStaticData->GetBT(evtSabacBeforeStart))
        {
            UINT32 dwTime = CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone();
            UINT32 dwStartTime = g_GWConfig.Sabac.nStartTime / 100 * 3600 + (g_GWConfig.Sabac.nStartTime % 100) * 60;
            dwStartTime = dwTime - (dwTime % 86400) + dwStartTime;
            if (dwStartTime <= dwTime)
                dwStartTime += 86400;

            INT32 nRemainTime = dwStartTime - dwTime;
            if (nRemainTime < 10 * 60)
            {
                nRetCode = RunBT(evtSabacBeforeStart, 0, m_poStaticData->GetBT(evtSabacBeforeStart));
                LOG_CHECK_ERROR(nRetCode);

                m_bSabacEventTriggered = TRUE;
            }
        }

        if (nCurrentTime >= g_GWConfig.Sabac.nStartTime && nCurrentTime < g_GWConfig.Sabac.nEndTime)
        {
            nRetCode = CCliMessageHandler::GetInstance().DoSabacStateChange(m_nServerGroupID, TRUE);
            LOG_CHECK_ERROR(nRetCode);

            SetVariable(8, g_GWConfig.Sabac.nEndTime);
            m_bSabacStarted = TRUE;
            m_bSabacEventTriggered = FALSE;

            CGSGuild* poGuild = CGuildMgr::GetInstance().FindGuild(CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID));
            if (poGuild && !poGuild->IsJoinSabacToday())
            {
                UINT32 dwTime = CGameWorld::GetInstance().GetGameTime() - 86400 * 3;

                nRetCode = CCliMessageHandler::GetInstance().DoMSEnrollSabac(poGuild->GetID(), dwTime);
                LOG_CHECK_ERROR(nRetCode);
            }

            if (m_poStaticData->GetBT(evtSabacStart))
            {
                nRetCode = RunBT(evtSabacStart, 0, m_poStaticData->GetBT(evtSabacStart));
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }

Exit0:

    return;
}

BOOL CScene::AddMissilePlacedGrid(INT32 nGridX, INT32 nGridY, CMissile* poMissile)
{
    INT32 nRetCode = 0;

    UINT32 dwKey = (nGridX << 16) + nGridY;
    LOG_PROCESS_ERROR(poMissile);

    nRetCode = m_MissilePlaceMap.insert(std::make_pair(dwKey, poMissile)).second;
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

CMissile* CScene::FindMissileByPlacedGrid(INT32 nGridX, INT32 nGridY)
{
    UINT32 dwKey = (nGridX << 16) + nGridY;
    TMissilePlaceMap::iterator itr = m_MissilePlaceMap.find(dwKey);
    PROCESS_ERROR(itr != m_MissilePlaceMap.end());

    return itr->second;
Exit0:
    return NULL;
}

BOOL CScene::RemoveMissilePlacedGrid(INT32 nGridX, INT32 nGridY)
{
    UINT32 dwKey = (nGridX << 16) + nGridY;

    TMissilePlaceMap::iterator itr = m_MissilePlaceMap.find(dwKey);
    PROCESS_ERROR(itr != m_MissilePlaceMap.end());

    m_MissilePlaceMap.erase(itr);

    return TRUE;
Exit0:
    return FALSE;
}


#endif	// !_ROBOT
