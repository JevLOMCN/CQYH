#include "stdafx.h"
#include <memory>
#include "npc.h"

#include "utf8.h"
#include <tchar.h>  // Add this header

#include "id_def.h"
#include "role_def.h"
#include "npc_data.h"
#include "npc_pattern.h"
#include "spell_config.h"

#include "game_world.h"
#include "scene.h"
#include "region.h"
#include "grid_def.h"
#include "scene_file_mgr.h"
#include "message_handler.h"
#include "drop_generator.h"
#include "script_mgr.h"
#include "ai_processor.h"
#include "kismet/kismet_node.h"
#include "npc_force_mgr.h"
#include "score_def.h"
#include "statistic.h"
#include "adventure.h"
#include "server_config.h"
#include "pet_data.h"
#include "attr_conversion_data.h"
#include "ai_data.h"

#define MAX_FIND_TIMER	(2 * REGION_WIDTH / GRID_WIDTH)

//CNpcSpellBook CNpc::ms_SpellBook;

CNpc::CNpc()
{
    m_nNpcState = npcInvalid;
    m_pNpcData = NULL;
    m_pNpcPattern = NULL;
    m_pSpawnPoint = NULL;
    m_dwHostID = 0;
    m_nQuality = nqInvalid;
    m_nSpawnFrame = 0;
    m_nNpcTimer = 0;
    m_nFindState = 0;
    m_nFindTimer = 0;
    m_nFindTimes = 0;
    m_nPathSide = 0;
    m_bAvoidGroundEffect = FALSE;
    m_bGenerateDrop = TRUE;
    m_bIsSummoned = FALSE;
    m_nPetType = 0;
    m_nPetInitLevel = 0;
    m_nPetMaxLevel = 0;
    m_nPetLevel = 0;
    m_nPetExp = 0;
    m_bAlwaysRun = FALSE;
    m_bNeverBeHit = FALSE;
    m_dwPetAliveFrm = 0;
    m_nLastRecoveryFrm = 0;
    m_nPetHpRestore = 0;
}

CNpc::~CNpc()
{
    UnInit();
}

const TCHAR* CNpc::GetName(void)
{
    if (m_pNpcData)
        return m_pNpcData->szDisplayName;
    else
        return NULL;
}

INT32 CNpc::GetSpecies(void)
{
    if (m_pNpcData)
        return m_pNpcData->nSpecies;
    else
        return spcInvalid;
}

BOOL CNpc::Init(UINT32 dwObjID, const NPC_SPAWN_POINT* pSpawnPoint, UINT32 dwHostID)
{
    INT32 nRetCode = 0;

    nRetCode = CCharacter::Init(dwObjID);
    LOG_PROCESS_ERROR(nRetCode);

    SetClass(clsNpc);

    m_dwHostID = dwHostID;
    m_nNpcState = npcSpawn;
    m_nNpcTimer = 0;
    m_pSpawnPoint = pSpawnPoint;
    m_nFindState = 0;
    m_nFindTimer = 0;
    m_nFindTimes = 0;
    m_nPathSide = 0;
    m_bAvoidGroundEffect = FALSE;
    m_bGenerateDrop = TRUE;
    m_bIsSummoned = FALSE;
    m_pNpcPattern = NULL;
    m_nSpawnFrame = CGameWorld::GetInstance().GetGameFrame();

    m_nPetType = 0;
    m_nPetLevel = 0;
    m_nPetInitLevel = 0;
    m_nPetMaxLevel = 0;
    m_nPetExp = 0;
    m_bAlwaysRun = FALSE;
    m_bNeverBeHit = FALSE;
    m_dwPetAliveFrm = 0;
    m_nLastRecoveryFrm = CGameWorld::GetInstance().GetGameFrame();
    m_nPetHpRestore = 0;

    m_SpellBook.SetHighestSpellLevel(0);

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CNpc::SetTemplateData(INT32 nTemplateID)
{
    INT32 nRetCode = 0;
    CAIProcessor* pProcessor = NULL;

    // init npc data
    m_pNpcData = CDataManager<NPC_DATA>::GetInstance().GetData(nTemplateID);
    LOG_PROCESS_ERROR(m_pNpcData);

    m_pNpcPattern = CDataManager<NPC_PATTERN_DATA>::GetInstance().GetData(m_pNpcData->nPatternID);
    LOG_PROCESS_ERROR(m_pNpcPattern);

    SetLevel(m_pNpcData->nLevel);

    m_nQuality = m_pNpcData->nQuality;

    SetBodyType(m_pNpcData->nBodyType);

    nRetCode = ResetFlags();
    LOG_PROCESS_ERROR(nRetCode);

    for (INT32 i = catBeginExAttr; i < catEndExAttr; i++)
        m_AttrList.SetAttrA(this, i, m_pNpcPattern->nAttrListA[i]);

    m_nCurrentHp = m_AttrList.GetAttr(catMaxHp);
    m_nCurrentMp = m_AttrList.GetAttr(catMaxMp);

    m_AttrList.SetAttrBase(this, catWalkSpeed, 0);
    m_AttrList.SetAttrBase(this, catRunSpeed, 0);

    // init ai processor
#ifndef _ROBOT
    m_AttrList.SetAttrA(this, catWalkSpeed, CONVERT_SPEED(500));
    m_AttrList.SetAttrA(this, catRunSpeed, CONVERT_SPEED(750));

    nRetCode = SetAIProcessor(m_pNpcData->nAIID);
    LOG_PROCESS_ERROR(nRetCode);

    m_nOriginAiID = m_pNpcData->nAIID;

    m_bNeverBeHit = m_pNpcData->bNeverBeHit;

    if (m_poAIProcessor)
    {
        m_poAIProcessor->SetIgnoreInvisible(m_pNpcData->bIgnoreInvisible);
        m_poAIProcessor->SetCounterInvisible(m_pNpcData->bCounterInvisible);
    }

    if (m_pSpawnPoint)
    {
        if (m_pSpawnPoint->nCurrentHp > 0)
            m_nCurrentHp = m_pSpawnPoint->nCurrentHp;

        if (m_pSpawnPoint->nCurrentMp > 0)
            m_nCurrentMp = m_pSpawnPoint->nCurrentMp;

        if (m_pSpawnPoint->nAIID > 0)
        {
            nRetCode = SetAIProcessor(m_pSpawnPoint->nAIID);
            LOG_PROCESS_ERROR(nRetCode);

            m_nOriginAiID = m_pSpawnPoint->nAIID;
        }

        if (m_pSpawnPoint->nPatrolPathID > 0 && m_poAIProcessor)
        {
            m_poAIProcessor->SetPatrolParam(m_pSpawnPoint->nPatrolPathID, m_pSpawnPoint->nPatrolStyle);
        }
    }

    if (m_poScene)
    {
        nRetCode = CCliMessageHandler::GetInstance().BroadcastNpcData(this);
        LOG_CHECK_ERROR(nRetCode);
    }


#endif //_ROBOT

    return TRUE;
Exit0:
    m_pNpcData = NULL;

    return FALSE;
}

BOOL CNpc::UnInit(void)
{
    INT32 nRetCode = 0;

    if (m_dwHostID > 0)
        INF("pet debug log: CNpc::UnInit HostID[%d] OBJID[%d]", m_dwHostID, GetObjID());

    m_pNpcData = NULL;
    m_pSpawnPoint = NULL;
    m_nNpcState = npcInvalid;

    m_nPetType = 0;
    m_nPetLevel = 0;
    m_nPetInitLevel = 0;
    m_nPetMaxLevel = 0;
    m_nPetExp = 0;
    m_dwPetAliveFrm = 0;

    nRetCode = CCharacter::UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CNpc::SetAlwaysRun(BOOL bAlwaysRun)
{
    PROCESS_SUCCESS(m_bAlwaysRun == bAlwaysRun);

    if (m_poRegion)
    {
        if (bAlwaysRun)
            m_poRegion->IncrActiveCount(this);
        else
            m_poRegion->DecrActiveCount(this);
    }

    m_bAlwaysRun = bAlwaysRun;

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::MainLoop(void)
{
    INT32 nRetCode = 0;
    INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

    nRetCode = CSceneObject::CheckLoop();
    PROCESS_SUCCESS(!nRetCode);

    nRetCode = CCharacter::MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

    if ((nFrm % (GAME_FPS / 2)) == (m_dwObjID % (GAME_FPS / 2)))
        m_AuraList.Update(this);

    if ((nFrm % GAME_FPS) == (m_dwObjID % GAME_FPS))
        _On1SecMainLoop();

#ifndef _ROBOT
    if (m_nNpcState == npcAlive)
    {
        if ((nFrm % (GAME_FPS / 4)) == (m_dwObjID % (GAME_FPS / 4)))
        {
            if (m_bHpChanged)
            {
                CCliMessageHandler::GetInstance().DoSyncHp(this);
                m_bHpChanged = FALSE;
            }
        }
    }
#endif

Exit1:
    return TRUE;
Exit0:

    return FALSE;
}

INT32 CNpc::GetObjType(void)
{
    return otNpc;
}

BOOL CNpc::_ChangeRegion(CRegion* poDestRegion)
{
    INT32	nRetCode = 0;
    INT32	nTryDirection = 0;
    INT32	nOldDirection = 0;

    LOG_PROCESS_ERROR(poDestRegion);
    LOG_PROCESS_ERROR(m_poRegion);

    nTryDirection = g_GetDirection(m_poRegion->GetRegionX(), m_poRegion->GetRegionY(),
        poDestRegion->GetRegionX(), poDestRegion->GetRegionY()) * DT_DIRECTION_8_COUNT / MAX_DIRECTION_COUNT;

    for (INT32 nIndex = 1; nIndex <= g_nAdjList[nTryDirection][0]; nIndex++)
    {
        CRegion* poOldRegion;
        CRegion* poNewRegion;

        poNewRegion = poDestRegion->GetConnectedRegion(g_nAdjList[nTryDirection][nIndex]);

        nOldDirection = (g_nAdjList[nTryDirection][nIndex] + DT_DIRECTION_8_COUNT / 2) % DT_DIRECTION_8_COUNT;
        poOldRegion = m_poRegion->GetConnectedRegion(nOldDirection);

#ifndef _ROBOT
        if (poNewRegion)
        {
            nRetCode = CCliMessageHandler::GetInstance().DoCharAppear(this, poNewRegion, soatInRange);
            LOG_CHECK_ERROR(nRetCode);
        }

        if (poOldRegion)
        {
            nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(this, poOldRegion, sodtOutOfRange);
            LOG_CHECK_ERROR(nRetCode);
        }
#endif  // _ROBOT
    }

    nRetCode = m_poRegion->RemoveNpc(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = poDestRegion->AddNpc(this);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::_DistributeRegionBonus(CRole* poRole, CTeamInGS* poTeam, CRole* poKiller)
{
    INT64 llBaseExp = 0;
    INT64 llBaseSoul = 0;

    INT32 nNearMemberCount = 0;
    INT32 nTotalLevel = 0;
    BOOL bIsTeacher = FALSE;
    BOOL bIsStudent = FALSE;
    GAME_WORLD_CONFIG& rGWConfig = g_GWConfig;

    const static  INT32 s_MasterFactor = 10500;

    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(m_pNpcData);
    LOG_PROCESS_ERROR(m_pNpcData->nExpRate >= 0 && m_pNpcData->nExpRate <= 200000);
    LOG_PROCESS_ERROR(m_pNpcData->nSoulRate >= 0 && m_pNpcData->nSoulRate <= 200000);

    // Add experience
    if (m_pNpcData->bProfit)
    {
        // traverse team members
        nNearMemberCount = 1;
        nTotalLevel = poRole->GetLevel();
        if (poTeam)
        {
            for (INT32 nMemberIndex = 0; nMemberIndex < poTeam->GetMaxMemberCount(); nMemberIndex++)
            {
                if (poTeam->GetMemberTable(nMemberIndex).dwRoleID == INVALID_OBJECT_ID ||
                    poTeam->GetMemberTable(nMemberIndex).dwRoleID == poRole->GetObjID())
                    continue;

                CRole* poMemberRole = CGameWorld::GetInstance().FindValidRole(poTeam->GetMemberTable(nMemberIndex).dwRoleID);
                if (poMemberRole == NULL)
                    continue;

                INT64 llMaxDistance = g_GWConfig.nExpShareDistance * POINT_PER_CM * g_GWConfig.nExpShareDistance * POINT_PER_CM;
                if (poMemberRole->GetDistance3D(poRole) > llMaxDistance)
                    continue;

                nNearMemberCount++;
                nTotalLevel += poMemberRole->GetLevel();

                if (poRole->GetMaster().IsMyTeacher(poMemberRole->GetObjID()) && poRole->GetLevel() < GRADUATE_LEVEL)
                    bIsStudent = TRUE;
                if (poMemberRole->GetMaster().IsMyTeacher(poRole->GetObjID()) && poMemberRole->GetLevel() < GRADUATE_LEVEL)
                    bIsTeacher = TRUE;
            }
        }

        // base exp & soul
        llBaseExp = m_pNpcPattern->nExp;
        llBaseSoul = m_pNpcPattern->nSoul;

        llBaseExp = llBaseExp * m_pNpcData->nExpRate / RATE_MAX;
        llBaseSoul = llBaseSoul * m_pNpcData->nSoulRate / RATE_MAX;

        // global exp rate
        // master exp rate
        if (bIsTeacher)
            llBaseExp = llBaseExp * (g_GWConfig.nGlobalExpRate + g_GWConfig.nTeacherExpRate) / RATE_MAX;
        else if (bIsStudent)
            llBaseExp = llBaseExp * (g_GWConfig.nGlobalExpRate + g_GWConfig.nStudentExpRate) / RATE_MAX;
        else
            llBaseExp = llBaseExp * g_GWConfig.nGlobalExpRate / RATE_MAX;

        // level exp & soul
        INT32 nLevelDiff = m_nLevel - poKiller->GetLevel();
        if (nLevelDiff < -MAX_LEVEL_DIFF)
            nLevelDiff = -MAX_LEVEL_DIFF;
        else if (nLevelDiff > MAX_LEVEL_DIFF)
            nLevelDiff = MAX_LEVEL_DIFF;
        nLevelDiff = nLevelDiff + MAX_LEVEL_DIFF;
        LOG_PROCESS_ERROR(nLevelDiff >= 0 && nLevelDiff < sizeof(rGWConfig.nLevelExpRate) / sizeof(rGWConfig.nLevelExpRate[0]));
        llBaseExp = llBaseExp * rGWConfig.nLevelExpRate[nLevelDiff] / RATE_MAX;
        llBaseSoul = llBaseSoul * rGWConfig.nLevelExpRate[nLevelDiff] / RATE_MAX;

        // team exp & soul
        if (poTeam)
        {
            LOG_PROCESS_ERROR(nNearMemberCount < (sizeof(rGWConfig.nTeamExpRate) / sizeof(rGWConfig.nTeamExpRate[0])));
            llBaseExp = llBaseExp * rGWConfig.nTeamExpRate[nNearMemberCount] / RATE_MAX;
            llBaseSoul = llBaseSoul * rGWConfig.nTeamExpRate[nNearMemberCount] / RATE_MAX;
        }

        // total level
        llBaseExp = llBaseExp * poRole->GetLevel() / nTotalLevel + 1;
        llBaseSoul = llBaseSoul * poRole->GetLevel() / nTotalLevel;

        // add exp
        if (llBaseExp > 0)
        {
            Item* exp_bottle = poRole->GetItemBag()->GetItem(CONSLOT_DEPOSIT, DEPOSIT_EXP_SLOT);
            INT64 exp_bottle_exp = 0;
            if (exp_bottle && exp_bottle->m_iteminfo.m_nCurDur < exp_bottle->m_iteminfo.m_nMaxDur)
            {
                exp_bottle_exp = llBaseExp * g_GWConfig.nExpBottlePortion / RATE_MAX;
                llBaseExp -= exp_bottle_exp;
                exp_bottle->m_iteminfo.m_nCurDur += exp_bottle_exp * g_GWConfig.nExpBottleFactor / RATE_MAX;
                if (exp_bottle->m_iteminfo.m_nCurDur > exp_bottle->m_iteminfo.m_nMaxDur)
                    exp_bottle->m_iteminfo.m_nCurDur = exp_bottle->m_iteminfo.m_nMaxDur;

#ifndef _ROBOT
                CCliMessageHandler::GetInstance().DoSendEquipDurChange(poRole, CONSLOT_DEPOSIT, DEPOSIT_EXP_SLOT, exp_bottle->m_iteminfo.m_nCurDur);
#endif // _ROBOT
            }

            poRole->AddExp(llBaseExp, TRUE, m_pNpcData->bDailyExp);

            if (poRole->GetHero() != NULL && poRole->GetHero()->IsSummoned())
                poRole->GetHero()->AddExp(llBaseExp * g_GWConfig.nHeroExpGainRate / RATE_MAX);
        }

        // add soul
        if (llBaseSoul > 0)
        {
            Item* soul_bottle = poRole->GetItemBag()->GetItem(CONSLOT_DEPOSIT, DEPOSIT_SOUL_SLOT);
            if (soul_bottle && soul_bottle->m_iteminfo.m_nCurDur < soul_bottle->m_iteminfo.m_nMaxDur)
            {
                soul_bottle->m_iteminfo.m_nCurDur += llBaseSoul;
                if (soul_bottle->m_iteminfo.m_nCurDur > soul_bottle->m_iteminfo.m_nMaxDur)
                    soul_bottle->m_iteminfo.m_nCurDur = soul_bottle->m_iteminfo.m_nMaxDur;

#ifndef _ROBOT
                CCliMessageHandler::GetInstance().DoSendEquipDurChange(poRole, CONSLOT_DEPOSIT, DEPOSIT_SOUL_SLOT, soul_bottle->m_iteminfo.m_nCurDur);
#endif // _ROBOT
            }
            else
            {
                poRole->GetItemBag()->AddMoney(MONEYTYPE_SOUL_PTS, llBaseSoul, MONEYLOG_LOOT, 0);
            }
        }
    }

    // Reputation
    if (m_pNpcData->bReduceReputation)
    {
        poRole->GetReputation().ModifyValue(m_pNpcData->nForceID, g_GWConfig.SubReputation[m_nQuality]);
    }

    if (m_pNpcData->nRivalForceID != -1 &&
        poRole->GetReputation().GetValue(m_pNpcData->nRivalForceID) < g_GWConfig.AddReputation[m_nQuality].nMaxReputation)
    {
        poRole->GetReputation().ModifyValue(m_pNpcData->nRivalForceID, g_GWConfig.AddReputation[m_nQuality].nReputation);
    }

    CStatistic::GetInstance().OnKillNpc(poRole, this);
    poRole->GetQuestList().OnKillNpc(this);
    poRole->GetKillNpcQuest().OnKillNpc(this);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::_DistributeSceneBonus(CRole* poRole, CTeamInGS* poTeam)
{
    LOG_PROCESS_ERROR(poRole);

    // comba stat
    poRole->IncCombatStat(escsNpcKillCount, 1);

    if (m_nQuality == nqElite)
    {
        poRole->IncCombatStat(escsEliteNpcKillCount, 1);
        if (m_poScene->GetSceneData() &&
            (m_poScene->GetSceneData()->nType == estDungeon ||
                m_poScene->GetSceneData()->nType == estSoloDungeon))
        {
            m_poScene->CheckDungeonGoal(poRole);
        }
    }
    else if (m_nQuality == nqBoss)
    {
        poRole->IncCombatStat(escsBossKillCount, 1);
        if (m_poScene->GetSceneData() &&
            (m_poScene->GetSceneData()->nType == estDungeon ||
                m_poScene->GetSceneData()->nType == estSoloDungeon))
        {
            m_poScene->CheckDungeonGoal(poRole);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::Die(INT32 nParam)
{
    INT32 nRetCode = 0;
    INT32 nKillerType = otInvalid;
    CCharacter* poDirectKiller = NULL;
    CRole* poKiller = NULL;
    CRole* poHostRole = NULL;
    const DROP_CHANCE_DATA* pDropData = NULL;
    INT64 llShareDistance2 = g_GWConfig.nExpShareDistance * POINT_PER_CM * g_GWConfig.nExpShareDistance * POINT_PER_CM;

    LOG_PROCESS_ERROR(m_pNpcData);
    LOG_PROCESS_ERROR(m_poRegion);
    LOG_PROCESS_ERROR(m_poScene);
    LOG_PROCESS_ERROR(m_poScene->GetStaticData());

    LOG_PROCESS_ERROR(m_eMoveState != CMS_DIE);
    PROCESS_SUCCESS(m_oFlag.CheckFlag(cfBTDenyDie));

    // Death script.
#ifndef _ROBOT
    if (m_poAIProcessor && m_poAIProcessor->GetFightState() != afsDying)
    {
        nRetCode = m_poAIProcessor->CallDeadBT(m_dwDropOwnerID);
        LOG_CHECK_ERROR(nRetCode);

        PROCESS_SUCCESS(m_poAIProcessor->GetFightState() == afsDying);
    }
#endif

    PROCESS_ERROR(m_eMoveState != CMS_DIE);

    // Execute the base class death handler
    nRetCode = CCharacter::Die(nParam);
    LOG_PROCESS_ERROR(nRetCode);

    // Monster state
    m_nNpcState = npcCorpse;

    // Monster time
    m_nNpcTimer = CGameWorld::GetInstance().GetGameFrame() + m_pNpcData->nCorpseTime;

    // HP
    m_nPetHpRestore = 0;

    // Determine the immediate killer
    if (m_dwDropOwnerID)
        poDirectKiller = CGameWorld::GetInstance().FindValidCharacter(m_dwDropOwnerID);
    if (poDirectKiller == NULL && m_dwLastCasterID)
        poDirectKiller = CGameWorld::GetInstance().FindValidCharacter(m_dwLastCasterID);

    // Determine the final killer
    if (poDirectKiller)
    {
        if (poDirectKiller->GetObjType() == otNpc)
        {
            CNpc* killerNpc = (CNpc*)poDirectKiller;
            if (killerNpc->GetPetType())
                killerNpc->AddPetExp(GetLevel());
        }

        if (poDirectKiller->GetHostID())
        {
            CCharacter* poHostChar = CGameWorld::GetInstance().FindValidCharacter(poDirectKiller->GetHostID());
            if (poHostChar && poHostChar->GetScene() == poDirectKiller->GetScene())
                poDirectKiller = poHostChar;
        }

        if (poDirectKiller->GetObjType() == otRole)
            poKiller = (CRole*)poDirectKiller;
    }

    // Add experience and reputation, complete quests, and update map variables (e.g. spawn a boss after X kills)
    if (poKiller && m_bGenerateDrop)
    {
        if (poKiller->GetTeamID() != INVALID_OBJECT_ID)
        {
            CTeamInGS* pTeam = CGameWorld::GetInstance().FindTeam(poKiller->GetTeamID());
            LOG_PROCESS_ERROR(pTeam);

            for (INT32 nMemberIndex = 0; nMemberIndex < pTeam->GetMaxMemberCount(); nMemberIndex++)
            {
                CRole* poTeamMember = CGameWorld::GetInstance().FindValidRole(pTeam->GetMemberTable(nMemberIndex).dwRoleID);

                if (poTeamMember && poTeamMember->GetScene() == poKiller->GetScene())
                {
                    if (GetDistance3D(poTeamMember) <= llShareDistance2)
                    {
                        nRetCode = _DistributeRegionBonus(poTeamMember, pTeam, poKiller);
                        LOG_CHECK_ERROR(nRetCode);
                    }

                    nRetCode = _DistributeSceneBonus(poTeamMember, pTeam);
                    LOG_CHECK_ERROR(nRetCode);
                }
            }
        }
        else
        {
            // Grant region rewards
            nRetCode = _DistributeRegionBonus(poKiller, NULL, poKiller);
            LOG_CHECK_ERROR(nRetCode);

            // Distribute scene rewards
            nRetCode = _DistributeSceneBonus(poKiller, NULL);
            LOG_CHECK_ERROR(nRetCode);
        }

        poKiller->OnKillNpc(this, m_pNpcPattern->nExp);

        if (m_pNpcData->nHandbookType > htInvalid)
            poKiller->GetAchievementList().ActivateHandbook(m_pNpcData->nHandbookType, m_pNpcData->nHandbookIndex);

        CAdventure::GetInstance().OnKillNpc(poKiller, this);
    }

    // Announce a boss kill server-wide
    if (poKiller)
    {
        if (this->GetQuality() == nqBoss)
        {
            std::string strBossName = this->GetName();
            std::string strRoleName = poKiller->GetName();

            std::unique_ptr<char[]> strAnsiBossName(utf8_to_char(strBossName.c_str()));
            std::unique_ptr<char[]> strAnsiRoleName(utf8_to_char(strRoleName.c_str()));

            // Example: "[Woma Leader] was defeated by player [Ada]!"
            char strMsg[128] = { 0 };
            snprintf(strMsg, sizeof(strMsg), "[%s]被玩家[%s]击败!", strAnsiBossName ? strAnsiBossName.get() : "", strAnsiRoleName ? strAnsiRoleName.get() : "");

            std::unique_ptr<char[]> str_utf8(char_to_utf8(strMsg));
            Say(cctServer, cltTvShow, str_utf8 ? str_utf8.get() : "");

        }
    }

#ifndef _ROBOT
    // Execute the script after the monster dies
    if (m_pSpawnPoint)
    {
        CLuaScript* pScript = m_poScene->GetStaticData()->GetScript();

        // run BT
        if (m_pSpawnPoint->pDeadBTRoot)
        {
            nRetCode = m_poScene->RunBT(evtSingleNpcDie, 0, m_pSpawnPoint->pDeadBTRoot);
            //LOG_CHECK_ERROR(nRetCode);
            if (FALSE == nRetCode)
            {
                CRI("npc run BT evtSingleNpcDie error! ");
            }
        }

        // invoke Large Group and Medium Group
        if (m_pSpawnPoint->nLargeGroupID >= 0 && m_pSpawnPoint->nMediumGroupID >= 0)
        {
            m_poScene->IncreaseDieCount(this, m_pSpawnPoint->nLargeGroupID, m_pSpawnPoint->nMediumGroupID);
        }
    }

    //Generate drops
    if (m_pNpcData->bProfit &&
        m_bGenerateDrop &&
        m_poScene->GetState() == edstRuning &&
        (m_pSpawnPoint == NULL || m_pSpawnPoint->bCanDropItem))
    {
        const NPC_DROP_DATA* npc_drop_data = CDataManager<NPC_DROP_DATA>::GetInstance().GetData(m_pNpcData->nTemplateID);
        if (npc_drop_data)
        {
            BOOL big_drop = CDropManager::GetInstance().IsBigDrop(m_pNpcData->nTemplateID);
            nRetCode = CDropGenerator::GetInstance().GenerateDrops(m_poScene->GetServerGroup(),
                npc_drop_data->DropChanceData, npc_drop_data->nChanceCount,
                m_poScene, m_llX, m_llY, m_llZ,
                poKiller ? poKiller->GetObjID() : 0,
                GetObjID(), GetLevel(), GetQuality() >= nqElite, big_drop);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    // Check the monster’s final target ID (purpose currently unknown)
    if (m_poScene->CheckTargetID(m_pNpcData->nTemplateID))
    {
        //m_poScene->UpdateTarget(esttKillBoss);
        if (m_poScene->GetStaticData() && m_poScene->GetStaticData()->GetBT(evtBossDie))
        {
            nRetCode = m_poScene->RunBT(evtBossDie, GetObjID(), m_poScene->GetStaticData()->GetBT(evtBossDie));
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    // deal host (pet)
    if (GetPetType() != 0)
    {
        poHostRole = CGameWorld::GetInstance().FindRole(GetHostID());
        LOG_CHECK_ERROR(poHostRole);

        if (poHostRole)
        {
            nRetCode = poHostRole->DelPet(GetObjID());
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    // Save a log if the monster did not kill itself
    if (nParam != cdrSuicide)
    {
        nRetCode = CCliMessageHandler::GetInstance().DoDLNpcDie(m_poScene->GetServerGroup(),
            m_pNpcData->nTemplateID, m_pNpcData->bDieIDMonitor);
        LOG_CHECK_ERROR(nRetCode);
    }

#endif  // _ROBOT

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::_NpcLoop(void)
{
    INT32 nRetCode = 0;

    switch (m_nNpcState)
    {
    case npcSpawn:
    case npcRevive:
        if (m_nNpcTimer <= CGameWorld::GetInstance().GetGameFrame())
        {
            LOG_PROCESS_ERROR(m_pNpcData);
            if (!m_pNpcData->bInvincible)
                DecControlCount(cclInvincible);
            m_nNpcState = npcAlive;
        }
        break;
    case npcAlive:
        break;
    case npcCorpse:    // Corpse
        if (m_nNpcTimer < CGameWorld::GetInstance().GetGameFrame())
        {
            LOG_PROCESS_ERROR(m_pNpcData);
            if (m_pNpcData->nCorpseDoodadID != 0)
            {
                CDoodad* poDoodad = m_poScene->SpawnDoodad(m_pNpcData->nCorpseDoodadID, m_llX, m_llY, m_llZ, m_nFaceDirection);
                LOG_CHECK_ERROR(poDoodad);
            }

            if (m_pSpawnPoint && m_pSpawnPoint->bAutoRespawn)
            {
                INT32 nRespawnTime = INT_MAX;

                LOG_PROCESS_ERROR(m_poScene);
                LOG_PROCESS_ERROR(m_poScene->GetSceneData());
                LOG_PROCESS_ERROR(m_poScene->GetStaticData());

                // Respawn interval
                if (m_pSpawnPoint->nMinRespawnTime == 0 && m_pSpawnPoint->nMaxRespawnTime == 0)
                    nRespawnTime = g_Random(m_pNpcData->nMinRespawnTime, m_pNpcData->nMaxRespawnTime);
                else
                    nRespawnTime = g_Random(m_pSpawnPoint->nMinRespawnTime, m_pSpawnPoint->nMaxRespawnTime);

                if (m_nQuality <= nqNormal5)
                    nRespawnTime = (INT64)nRespawnTime * g_GWConfig.nRespawnTimeRate / RATE_MAX;

                if (m_pSpawnPoint->nMediumGroupID != -1)
                {
                    const NPC_MEDIUM_SPAWN_GROUP* pMediumGroup = m_poScene->GetStaticData()->GetNpcMediumGroup(m_pSpawnPoint->nMediumGroupID);
                    if (pMediumGroup)
                    {
                        LOG_PROCESS_ERROR(m_pSpawnPoint->nSmallGroupID >= 0 && m_pSpawnPoint->nSmallGroupID < pMediumGroup->nSmallGroupCount);
                        const NPC_SMALL_SPAWN_GROUP* pSmallGroup = pMediumGroup->SmallGroupList[m_pSpawnPoint->nSmallGroupID];
                        if (pSmallGroup->bDynamicRespawnTime)
                        {
                            INT32 nSpawnCount = m_poScene->GetMediumGroupSpawnCount(m_pSpawnPoint->nMediumGroupID);
                            if (nSpawnCount * RATE_MAX < pSmallGroup->nPointCount * g_GWConfig.nDynamicRespawnPercent)
                                nRespawnTime = 0;
                        }
                    }
                }

                // Recalculate respawn time
                if (m_nQuality <= nqNormal5)
                    m_nNpcTimer = m_nSpawnFrame + nRespawnTime;
                else
                    m_nNpcTimer = CGameWorld::GetInstance().GetGameFrame() + nRespawnTime;

                // Object state
                m_nNpcState = npcRespawn;

                nRetCode = m_poScene->AddNpcToRespawnList(this);
                LOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                DeleteSelf();
            }
        }
        break;
    case npcRespawn:
        if (m_nNpcTimer < CGameWorld::GetInstance().GetGameFrame())
        {
            LOG_PROCESS_ERROR(m_poScene);
            nRetCode = m_poScene->RespawnNpc(this);
            if (nRetCode)
            {
                //				m_oCCFlag.SetFlag(cclInvincible);
                m_nNpcState = npcRevive;
            }
            else
            {
                m_nNpcTimer = CGameWorld::GetInstance().GetGameFrame() + 5 * 60 * GAME_FPS;
            }
        }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    DeleteSelf();

    return FALSE;
}

ISpellBook& CNpc::GetSpellBook(void)
{
    return m_SpellBook;
}

void CNpc::OnHurt(CCharacter* poDealer, CBullet* poBullet, INT32 nEffectValue)
{
    CRole* poRole = NULL;
    INT32 nAttackType = 0;

    LOG_PROCESS_ERROR(poBullet);
    LOG_PROCESS_ERROR(poDealer);
    LOG_PROCESS_ERROR(m_poAIProcessor);

    CCharacter::OnHurt(poDealer, poBullet, nEffectValue);

    SPL("Npc OnHurt: %u", GetObjID());

    nAttackType = poBullet->GetSpellAttr(satAttackType);

    if (m_pNpcData->nQuality != nqBoss && m_nLevel < MAX_NPC_LEVEL)
    {
        if (poBullet->GetBulletType() == betDamage && (nAttackType == spatMag || nAttackType == spatTao) && m_nLevel < 50)
        {
            INT32 nChaseDelayTime = poBullet->GetSpellAttr(satMonsterChaseDelayTime) + g_Random(0, 1000);
            IncMoveDelay(CONVERT_TIME(nChaseDelayTime));
        }
    }

    if (nEffectValue >= m_nCurrentHp)
    {
        if (poDealer->GetObjType() == otRole)
        {
            poRole = (CRole*)poDealer;
        }
        else if (poDealer->GetObjType() == otNpc)
        {
            CNpc* poNpc = (CNpc*)poDealer;

            PROCESS_ERROR(poNpc->GetObjID() != GetObjID());

            PROCESS_ERROR(poNpc->GetHostID() != 0);

            poRole = CGameWorld::GetInstance().FindValidRole(poNpc->GetHostID());
            PROCESS_ERROR(poRole);
        }
        else if (poDealer->GetObjType() == otHero)
        {
            CHero* poHero = (CHero*)poDealer;
            poRole = poHero->GetHostRole();
        }
    }
    else
    {
        PROCESS_ERROR(poBullet != NULL);
        PROCESS_ERROR(poBullet->GetStiffFrm() > 0);

        if (m_nQuality != nqBoss && m_nQuality != nqElite)
        {
            if (poBullet->GetStiffRandomNum() > GetAttr(catHurtResist))
                m_oStiffFlag.SetFlag(cstfSilent);

            if (g_Random(0, RATE_MAX) < poBullet->GetSpellAttr(satKnockBackRate) && g_Random(0, RATE_MAX) > GetAttr(catRepelResist))
                m_oStiffFlag.SetFlag(cstfKnockBack);
        }
    }

Exit0:
    return;
}

BOOL CNpc::JumpTo(INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection)
{
    INT32 nRetCode = 0;
    INT64 llSrcX = 0, llSrcY = 0, llSrcZ = 0;
    INT32 nSrcFaceDirection = 0;
    CScene* poSrcScene = NULL;

    LOG_PROCESS_ERROR(m_poScene);

    llSrcX = m_llX;
    llSrcY = m_llY;
    llSrcZ = m_llZ;
    nSrcFaceDirection = GetFaceDirection();
    poSrcScene = m_poScene;

    nRetCode = m_poScene->RemoveNpc(this);
    LOG_PROCESS_ERROR(nRetCode);

    SetX(llX);
    SetY(llY);
    SetZ(llZ);
    SetFaceDirection(nFaceDirection);

    nRetCode = poSrcScene->AddNpc(this, soatInRange);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (m_poScene == NULL && poSrcScene != NULL)
    {
        SetX(llSrcX);
        SetY(llSrcY);
        SetZ(llSrcZ);
        SetFaceDirection(nSrcFaceDirection);

        nRetCode = poSrcScene->AddNpc(this, soatInRange);
        LOG_CHECK_ERROR(nRetCode);
    }

    return FALSE;
}

BOOL CNpc::Revive(INT32 nParam, INT32 nHpPercent)
{
    INT32 nRetCode = 0;
    LOG_PROCESS_ERROR(m_poScene);

#ifndef _ROBOT
    if (m_poAIProcessor)
    {
        m_poAIProcessor->CallReviveBT();
    }
#endif

    nRetCode = CCharacter::Revive(nParam, nHpPercent);
    LOG_PROCESS_ERROR(nRetCode);

    m_bGenerateDrop = FALSE;
    m_nNpcState = npcAlive;

    return TRUE;
Exit0:
    return FALSE;
}

void CNpc::_On1SecMainLoop(void)
{
    INT32 nRetCode = 0;

    nRetCode = _NpcLoop();
    LOG_CHECK_ERROR(nRetCode);

    return;
}

UINT32 CNpc::GetPetAliveFrm(void)
{
    if (m_dwPetAliveFrm > CGameWorld::GetInstance().GetGameFrame())
        return m_dwPetAliveFrm - CGameWorld::GetInstance().GetGameFrame();
    return 0;
}

BOOL GetPetSpeed(INT32 nBornType, INT32 nInitLevel, INT32& nAttackSpeed, INT32& nWalkSpeed)
{
    if (nBornType == petBornHoly)
    {
        nWalkSpeed = CONVERT_TIME(500 - nInitLevel * 30);
        nAttackSpeed = CONVERT_TIME((500 - nInitLevel * 50));
    }
    else if (nBornType == petBornSkeleton)
    {
        nWalkSpeed = CONVERT_TIME(1200 - nInitLevel * 250);
        nAttackSpeed = CONVERT_TIME((2000 - nInitLevel * 600));
    }
    else if (nBornType == petBornByMage)
    {
        nWalkSpeed = CONVERT_TIME((1500 - nInitLevel * 200));
        nAttackSpeed = CONVERT_TIME((1000 - nInitLevel * 200));
    }
    else
        LOG_PROCESS_ERROR(FALSE);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::TransformToPet(INT32 nPetType, INT32 nCurLevel, INT32 nSpellLevel, INT32 nMaxPetLevel, UINT32 dwAliveFrm, INT32 nPetExp)
{
    INT32   nRetCode = 0;
    const PET_DATA* pcPetData = NULL;
    CAIProcessor* poAi = NULL;
    CRole* poHost = NULL;
    INT32   nWalkSpeed = 0;
    INT32   nOriginWalkSpeed = 0;
    INT32   nAttackSpeed = 0;
    INT32   nOriginAttackSpeed = 0;
    const NEW_AI_DATA* pAIData = NULL;

    LOG_PROCESS_ERROR(m_poScene);

    poHost = CGameWorld::GetInstance().FindRole(m_dwHostID);
    LOG_PROCESS_ERROR(poHost);

    pcPetData = GetPetData(nPetType, nSpellLevel);
    LOG_PROCESS_ERROR(pcPetData);

    LOG_PROCESS_ERROR(m_pNpcData);
    pAIData = CDataManager<NEW_AI_DATA>::GetInstance().GetData(m_pNpcData->nAIID);
    LOG_PROCESS_ERROR(pAIData);

    m_nPetType = nPetType;
    m_nPetMaxLevel = nMaxPetLevel;
    m_nPetInitLevel = nSpellLevel;
    nRetCode = SetPetLevel(nCurLevel, FALSE);
    LOG_CHECK_ERROR(nRetCode);

    m_dwPetAliveFrm = dwAliveFrm + CGameWorld::GetInstance().GetGameFrame();
    m_nPetExp = nPetExp;
    m_bGenerateDrop = FALSE;

    nRetCode = GetCoolDown().Add(MakeCoolDownID(COOLDOWN_AI, nastLiveTimer), dwAliveFrm);
    LOG_PROCESS_ERROR(nRetCode);

    //	nRetCode = GetPetSpeed(pcPetData->nBornType, m_nPetInitLevel, nAttackSpeed, nWalkSpeed);
    //	LOG_PROCESS_ERROR(nRetCode);
    //
    //	if (pcPetData->nBornType == petBornByMage)
    //	{
    //		poAi = GetAIProcessor();
    //		if (poAi)
    //			nOriginAttackSpeed = poAi->GetGlobalCDDelayFrame();
    //		nOriginWalkSpeed = GetAttr(catWalkSpeed);
    //
    //		nWalkSpeed = nWalkSpeed < nOriginWalkSpeed ? nWalkSpeed : nOriginWalkSpeed;
    //		if (0 != nOriginAttackSpeed)
    //			nAttackSpeed = nAttackSpeed < nOriginAttackSpeed ? nAttackSpeed : nOriginAttackSpeed;
    //	}
    //
    //	//////////////////////////////////////////////////////////////////////////
    //	nRetCode = SetAIProcessor(pcPetData->nAIID);
    //	LOG_PROCESS_ERROR(nRetCode);
    //	LOG_PROCESS_ERROR(m_poAIProcessor);
    //
    //	m_AttrList.SetAttrA(this, catWalkSpeed, nWalkSpeed);
    //
    //	m_poAIProcessor->SetVariable(PET_AI_IS_EXTRA_WALK, 0);
    //	m_poAIProcessor->SetVariable(PET_AIV_MAGE_PET_SPELL_ID, pAIData->nSpellID[nastNormalSpell1]);
    //	m_poAIProcessor->SetVariable(PET_AI_MAGE_CHASE_THRESHOLD, CONVERT_LENGTH(pAIData->nChaseThreshold));
    //	m_poAIProcessor->SetVariable(PET_AI_MAGE_PREFER_DISTANCE, CONVERT_LENGTH(pAIData->nPreferDistance));
    SetHome(m_poScene->GetSceneTempID(), GetX(), GetY(), GetZ());
    SetNewAiID(pcPetData->nAIID);

    //	GetAIProcessor()->CallInitBT();
    //
    //	if (m_pNpcData)
    //	{
    //		m_poAIProcessor->SetIgnoreInvisible(m_pNpcData->bIgnoreInvisible);
    //		m_poAIProcessor->SetCounterInvisible(m_pNpcData->bCounterInvisible);
    //	}
    //
    //	//////////////////////////////////////////////////////////////////////////
    //
    //	nRetCode = ChangePetAI(poHost->GetPetState());
    //	LOG_PROCESS_ERROR(nRetCode);
    //
    //	poAi = GetAIProcessor();
    //	if (poAi)
    //		poAi->SetGlobalCDDelayFrame(nAttackSpeed);

    nRetCode = SetAlwaysRun(TRUE);
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncNpcTransformState(this, npctPet);
    LOG_PROCESS_ERROR(nRetCode);
#endif	// _ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::TransformToNpc(void)
{
    INT32       nRetCode = 0;
    CRole* poHost = NULL;

    INF("pet debug log: TransformToNpc HostID[%d] OBJID[%d]", m_dwHostID, GetObjID());
    LOG_PROCESS_ERROR(m_pNpcData);

    m_nPetType = 0;
    //m_nPetLevel = 0;      // redmine 22358
    m_nPetMaxLevel = 0;
    m_nPetInitLevel = 0;
    m_dwPetAliveFrm = 0;
    m_nPetExp = 0;
    m_bGenerateDrop = TRUE;

    poHost = CGameWorld::GetInstance().FindRole(m_dwHostID);
    LOG_PROCESS_ERROR(poHost);

    nRetCode = poHost->DelPet(GetObjID());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = SetAlwaysRun(FALSE);
    LOG_PROCESS_ERROR(nRetCode);

    GetThreatList().SetFadeTime(CONVERT_TIME(g_GWConfig.nThreatFadeTime), CONVERT_TIME(g_GWConfig.nThreatFadeTime));

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncNpcTransformState(this, npctNpc);
    LOG_PROCESS_ERROR(nRetCode);
#endif	// _ROBOT

    //	nRetCode = SetAIProcessor(GetNpcData()->nAIID);
    //	LOG_PROCESS_ERROR(nRetCode);
    SetNewAiID(m_pNpcData->nAIID);

    m_dwHostID = 0;

    return TRUE;
Exit0:
    m_dwHostID = 0;
    return FALSE;
}

BOOL CNpc::ChangePetAI(INT32 nPetState)
{
    INT32   nRetCode = 0;
    CRole* poHost = NULL;
    const NEW_AI_DATA* pAIData = NULL;
    const PET_DATA* pcPetData = NULL;

    LOG_PROCESS_ERROR(m_poAIProcessor);

    do
    {
        if (nPetState == psIdle)
        {
            //INF("char (name=  idle---------------------------");
            m_poAIProcessor->SetVariable(PET_AI_CONTROL, psIdle);

            nRetCode = Stand(FALSE);
        }
        else if (nPetState == psFight)
        {
            //INF("char (name=  fight---------------------------");

            m_poAIProcessor->SetVariable(PET_AI_CONTROL, psFight);
            m_poAIProcessor->SetVariable(PET_AI_IS_EXTRA_WALK, 1);
        }
        else
            LOG_PROCESS_ERROR(FALSE);
    } while (0);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::SetPetLevel(INT32 nPetLevel, BOOL bDecOldAttr)
{
    INT32       nRetCode = 0;
    CRole* poOwner = NULL;
    CAura* pAura = NULL;
    CAuraInstance* pAuraInstance = NULL;
    const INT32  HOLY_PET_HP_AURA_ID = 60461;
    const PET_DATA* pPetData = NULL;

    LOG_PROCESS_ERROR(GetPetType() != 0);
    LOG_PROCESS_ERROR(nPetLevel >= 0 && nPetLevel <= m_nPetMaxLevel && nPetLevel <= MAX_PET_LEVEL);

    pPetData = GetPetData(GetPetType(), nPetLevel);
    LOG_PROCESS_ERROR(pPetData);

    nRetCode = _ComputePetExtraAttribute(nPetLevel, bDecOldAttr);
    LOG_PROCESS_ERROR(nRetCode);

    m_nPetLevel = nPetLevel;

    poOwner = CGameWorld::GetInstance().FindRole(m_dwHostID);
    LOG_PROCESS_ERROR(poOwner);

    // add aura
    if (bDecOldAttr == TRUE && pPetData->nBornType == petBornHoly) // means pet upgrade level
    {
        pAura = CSpellConfig::GetInstance().FindAuraTemplate(HOLY_PET_HP_AURA_ID);
        LOG_PROCESS_ERROR(pAura);

        pAuraInstance = AppendAura(pAura, this, GetObjID(), NULL, 0, 0, 0, 0);
        LOG_PROCESS_ERROR(pAuraInstance);
    }


#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncPetLevel(this, GetObjID(), m_nPetLevel);
    LOG_CHECK_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

void CNpc::AddPetExp(INT32 nPetExp)
{
    // constant config ----------- begin
    static INT32 s_PetExtraExp[] = { 0, 0, 50, 100, 200, 300, 600 };
    const  INT32 PET_NPC_COEFFICIENT = 15;
    const  INT32 PET_CONSTANT_COEFFICIENT = 100;
    // constant config ----------- end

    INT32   nRetCode = 0;
    INT32   nUpgradeExp = 0;

    PROCESS_ERROR(nPetExp >= 0);
    PROCESS_ERROR(m_nPetLevel < MAX_PET_LEVEL && m_nPetLevel < m_nPetMaxLevel);
    LOG_PROCESS_ERROR(m_nPetLevel < (sizeof(s_PetExtraExp) / sizeof(INT32)));

    nUpgradeExp = m_nLevel * PET_NPC_COEFFICIENT + PET_CONSTANT_COEFFICIENT + s_PetExtraExp[m_nPetLevel];
    m_nPetExp += nPetExp;

    while (m_nPetExp >= nUpgradeExp)
    {
        nRetCode = SetPetLevel(m_nPetLevel + 1, TRUE);
        LOG_PROCESS_ERROR(nRetCode);

        // refresh
        SetHp(GetAttr(catMaxHp));

        m_nPetExp -= nUpgradeExp;

        if (m_nPetLevel == MAX_PET_LEVEL || m_nPetLevel == m_nPetMaxLevel)
        {
            m_nPetExp = 0;
            break;
        }

        LOG_PROCESS_ERROR(m_nPetLevel < (sizeof(s_PetExtraExp) / sizeof(INT32)));
        nUpgradeExp = m_nLevel * PET_NPC_COEFFICIENT + PET_CONSTANT_COEFFICIENT + s_PetExtraExp[m_nPetLevel];
    }

    return;
Exit0:
    return;
}

BOOL CNpc::_ComputeAttribute(INT32 nNewLevel)
{
    const ATTR_LEVEL* pNewAttr = NULL;
    const ATTR_LEVEL* pOldAttr = NULL;

    INT32 nMoveSpeed = 0;
    INT32 nMountMoveSpeed = 0;

    PROCESS_ERROR(nNewLevel > 0);

    pNewAttr = CDataManager<ATTR_LEVEL>::GetInstance().GetData(MakeAttrKey(nNewLevel, GetClass()));
    LOG_PROCESS_ERROR(pNewAttr);

    pOldAttr = CDataManager<ATTR_LEVEL>::GetInstance().GetData(MakeAttrKey(m_nLevel, GetClass()));
    LOG_PROCESS_ERROR(pOldAttr);

    m_AttrList.ResetExAttrBase();

    for (INT32 i = catBeginExAttr; i < catEndExAttr; i++)
    {
        m_AttrList.AddAttrBase(this, i, pNewAttr->m_nMainAttr[i] - pOldAttr->m_nMainAttr[i]);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::_ComputePetExtraAttribute(INT32 nNewPetLevel, BOOL bDecOldAttr)
{
    const PET_DATA* pOldPetData = NULL;
    const PET_DATA* pPetData = NULL;

    LOG_PROCESS_ERROR(GetPetType() != 0);

    pPetData = GetPetData(GetPetType(), nNewPetLevel);
    LOG_PROCESS_ERROR(pPetData);

    if (bDecOldAttr == TRUE)
    {
        pOldPetData = GetPetData(GetPetType(), GetPetLevel());
        LOG_PROCESS_ERROR(pOldPetData);

        if (m_AttrList.GetAttrBase(catMaxPhyAtk) >= pOldPetData->nBonusphyAtkMax)
            m_AttrList.AddAttrBase(this, catMaxPhyAtk, -pOldPetData->nBonusphyAtkMax);
        else
        {
            CRI("_ComputePetAttr error! catMaxPhyAtk < 0");
            m_AttrList.AddAttrBase(this, catMaxPhyAtk, m_AttrList.GetAttrBase(catMaxPhyAtk));
        }

        if (m_AttrList.GetAttrBase(catMagDef) >= pOldPetData->nMagDef)
            m_AttrList.AddAttrBase(this, catMagDef, -pOldPetData->nMagDef);
        else
        {
            CRI("_ComputePetAttr error! catMagDef < 0");
            m_AttrList.AddAttrBase(this, catMagDef, m_AttrList.GetAttrBase(catMagDef));
        }

        if (m_AttrList.GetAttrBase(catMaxMagDef) >= pOldPetData->nMagDefMax)
            m_AttrList.AddAttrBase(this, catMaxMagDef, -pOldPetData->nMagDefMax);
        else
        {
            CRI("_ComputePetAttr error! catMaxMagDef < 0");
            m_AttrList.AddAttrBase(this, catMaxMagDef, m_AttrList.GetAttrBase(catMaxMagDef));
        }

        if (m_AttrList.GetAttrBase(catMaxHp) >= pOldPetData->nBonusHPMaxA)
            m_AttrList.AddAttrBase(this, catMaxHp, -pOldPetData->nBonusHPMaxA);
        else
        {
            CRI("_ComputePetAttr error! catMaxHp < 0");
            m_AttrList.AddAttrBase(this, catMaxHp, m_AttrList.GetAttrBase(catMaxHp));
        }

        if (m_AttrList.GetAttrB(catMaxHp) >= pOldPetData->nBonusHPMaxB)
            m_AttrList.AddAttrB(this, catMaxHp, -pOldPetData->nBonusHPMaxB);
        else
        {
            CRI("_ComputePetAttr error! catMaxHp < 0");
            m_AttrList.AddAttrB(this, catMaxHp, m_AttrList.GetAttrB(catMaxHp));
        }
    }

    m_AttrList.SetAttrBase(this, catAccurate, pPetData->nAccurate);
    m_AttrList.AddAttrBase(this, catMaxPhyAtk, pPetData->nBonusphyAtkMax);
    m_AttrList.AddAttrBase(this, catMagDef, pPetData->nMagDef);
    m_AttrList.AddAttrBase(this, catMaxMagDef, pPetData->nMagDefMax);

    m_AttrList.AddAttrBase(this, catMaxHp, pPetData->nBonusHPMaxA);
    m_AttrList.AddAttrB(this, catMaxHp, pPetData->nBonusHPMaxB);

    if (m_nPetHpRestore <= 0)
    {
        m_nPetHpRestore = m_AttrList.GetAttr(catMaxHp);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CNpc::ResetFlags(void)
{
    LOG_PROCESS_ERROR(m_pNpcData);

    if (m_pNpcData->bInvincible)
        IncControlCount(cclInvincible);

    if (!m_pNpcData->bCanbeThrown)
        GetFlag().SetFlag(cfCannotBeThrown);

    if (m_pNpcData->bAuraInvalid)
        GetFlag().SetFlag(cfAuraInvalid);

    if (m_pNpcData->bNeverBeHit)
        GetFlag().SetFlag(cfNeverBeHit);

#ifndef _DEBUG
    if (m_pNpcData->bImmunePassiveMove)
        GetFlag().SetFlag(cfImmunePassiveMove);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

void CNpc::_Recovery(void)
{
    INT32 nHpRecovery = m_AttrList.GetAttr(catHpRecovery);
    INT32 nMpRecovery = m_AttrList.GetAttr(catMpRecovery);
    INT32 nIntervalFrm = CGameWorld::GetInstance().GetGameFrame() - m_nLastRecoveryFrm;
    INT32 nTap = m_AttrList.GetAttr(catHpRecoveryTap);

    nTap = nTap == 0 ? 1 : nTap;

    INT32 nRecoveryTimes = nIntervalFrm / GAME_FPS / nTap;

    // HP, MP Recovery:
    LOG_PROCESS_ERROR(g_GWConfig.nHpRecoveryParam >= 0);
    LOG_PROCESS_ERROR(g_GWConfig.nMpRecoveryParam >= 0);

    nHpRecovery = nHpRecovery == 0 ? g_GWConfig.nHpRecoveryParam : nHpRecovery;
    nMpRecovery = nMpRecovery == 0 ? g_GWConfig.nMpRecoveryParam : nMpRecovery;

    if (nIntervalFrm > GAME_FPS)
    {
        ChangeHp(nRecoveryTimes * (m_AttrList.GetAttr(catMaxHp) / nHpRecovery + 1), cdrRecovery);
    }

    else
        CCharacter::_Recovery();

    m_nLastRecoveryFrm = CGameWorld::GetInstance().GetGameFrame();

Exit0:
    return;
}

BOOL CNpc::_GridMove(INT64 llDestX, INT64 llDestY)
{
    INT32 nRetCode = 0;
    INT32 nDestRegionX;
    INT32 nDestRegionY;
    CRegion* poDestRegion = NULL;
    const ZONE_DATA* pcDestZone = NULL;
    const GRID_DATA* pcDestGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;

    GRID_INSTANCE_DATA* pOldGridInstance = NULL;
    INT64 llOldX = m_llX, llOldY = m_llY;

    LOG_PROCESS_ERROR(m_poScene);
    LOG_PROCESS_ERROR(m_eMoveState != CMS_DIE);

    nRetCode = m_poScene->GetLowerGrid(llDestX, llDestY, m_llZ + GRID_WIDTH, pcDestZone, pcDestGrid);
    PROCESS_ERROR(nRetCode);
    PROCESS_ERROR(!pcDestGrid->bOutOfPoly);

    if (pcDestGrid == m_pcGrid)
    {
        m_llX = llDestX;
        m_llY = llDestY;
    }
    else
    {
#ifndef _ROBOT
        if (pcDestGrid->uDoor > 0)
            PROCESS_ERROR(!m_poScene->IsDoorClosed(pcDestZone->byDoor[pcDestGrid->uDoor]));
#endif	// _ROBOT

        pGridInstance = m_poScene->GetGridInstance(pcDestZone, pcDestGrid);
        LOG_PROCESS_ERROR(pGridInstance);

        switch (m_nBodyType)
        {
        case body0x0:
            break;
        case body1x1:
            PROCESS_ERROR(pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
            if (!pcDestGrid->bNotBlockChar)
                PROCESS_ERROR(pGridInstance->uCharRef == 0);
            break;
        case body3x3:
            nRetCode = CheckAdjGridCharRef(llDestX, llDestY, m_llZ);
            PROCESS_ERROR(nRetCode);
            break;
        }

        PROCESS_ERROR(pGridInstance->uMissileTag1 == 0);
        if (m_bAvoidGroundEffect)
            PROCESS_ERROR(pGridInstance->uMissileTag2 == 0);

        nDestRegionX = (INT32)(llDestX / REGION_WIDTH);
        nDestRegionY = (INT32)(llDestY / REGION_WIDTH);

        poDestRegion = m_poScene->GetRegion(nDestRegionX, nDestRegionY);
        LOG_PROCESS_ERROR(poDestRegion);

        nRetCode = ClearGridCharRef();
        LOG_CHECK_ERROR(nRetCode);

        m_llX = llDestX;
        m_llY = llDestY;
        m_llZ = pcDestGrid->uHighLayer * GRID_LAYER_HEIGHT;

        m_pcGrid = pcDestGrid;
        m_pcZone = pcDestZone;

        pOldGridInstance = m_pGridInstance;
        m_pGridInstance = pGridInstance;

        nRetCode = SetGridCharRef();
        LOG_CHECK_ERROR(nRetCode);

        if (poDestRegion != m_poRegion)
        {
            nRetCode = _ChangeRegion(poDestRegion);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;

}

INT32 CNpc::ChangeHp(INT32 nValue, INT32 nReason)
{
    INT32 nRetCode = CCharacter::ChangeHp(nValue, nReason);

    if (nRetCode != 0)
    {
        m_nPetHpRestore += nValue;
        m_nPetHpRestore = m_nPetHpRestore < 0 ? 0 : m_nPetHpRestore;
    }

    return nRetCode;
}
