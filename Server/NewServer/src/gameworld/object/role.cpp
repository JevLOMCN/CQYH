#include "stdafx.h"
#include <memory>
#include "role.h"

#include "zlib.h"
#include "utf8.h"

#include "math_ex.h"
#include "server_config.h"

#include "id_def.h"
#include "grid_def.h"
#include "scene_def.h"
#include "game_world.h"
#include "scene.h"
#include "region.h"
#include "zone_def.h"
#include "message_handler.h"
#include "robot_message_handler.h"
#include "spell_config.h"
#include "spell.h"
#include "rune_data.h"
#include "script_mgr.h"
#include "level_data.h"
#include "attr_conversion_data.h"
#include "attr_modifier.h"
#include "scene_file_mgr.h"
#include "kismet/kismet_node.h"
#include "drop_generator.h"
#include "modifier_tree.h"
#include "ai_processor.h"
#include "ai_manager.h"
#include "shop_data.h"
#include "npc_shop_mgr.h"
#include "doodad_data.h"
#include "role_faction_mgr.h"
#include "statistic.h"
#include "mount_data.h"
#include "hero_attr_data.h"
#include "guild_mgr.h"
#include "spirit_data.h"
#include "adventure.h"
#include "activity_reward_data.h"
#include "guild_config.h"
#include "sabac_mgr.h"
#include "checkin_data.h"
#include "anti_cheat.h"

#define MAX_MOVE_FRAME_OFFSET           GAME_FPS
#define PK_VALUE_REFRESH_INTERVAL       (GAME_FPS * 60 * 5)
#define MAX_HERO_DISTANCE               (1024 * POINT_PER_CM)

inline BOOL IsInSabacZone(CRole* poRole)
{
    PROCESS_ERROR(poRole);
    PROCESS_ERROR(poRole->GetScene());

    PROCESS_ERROR(poRole->GetScene()->GetSceneTempID() == g_GWConfig.Sabac.nSceneTempID);
    PROCESS_ERROR(poRole->GetScene()->IsSabacStarted());

    {
        GAME_WORLD_CONFIG::POS* lt = &g_GWConfig.Sabac.WarZone1LeftTop;
        GAME_WORLD_CONFIG::POS* rb = &g_GWConfig.Sabac.WarZone1RightBottom;

        BOOL bInZone = g_PointInCube(
            (INT32)(poRole->GetX() / POINT_PER_CM), (INT32)(poRole->GetY() / POINT_PER_CM), (INT32)(poRole->GetZ() / POINT_PER_CM),
            lt->x, lt->y, lt->z, rb->x, rb->y, rb->z);
        if (!bInZone)
        {
            lt = &g_GWConfig.Sabac.WarZone2LeftTop;
            rb = &g_GWConfig.Sabac.WarZone2RightBottom;

            bInZone = g_PointInCube(
                (INT32)(poRole->GetX() / POINT_PER_CM), (INT32)(poRole->GetY() / POINT_PER_CM), (INT32)(poRole->GetZ() / POINT_PER_CM),
                lt->x, lt->y, lt->z, rb->x, rb->y, rb->z);
        }

        return bInZone;
    }
Exit0:
    return FALSE;
}

CRole::CRole()
{
    m_eState = rsInvalid;
    m_eFcmState = fcmsFull;
    m_bFreeMode = FALSE;
    m_bInJump = FALSE;
    m_bInLogin = FALSE;
    m_bKicked = FALSE;
    m_bIsLoading = FALSE;
    m_dwSessionID = 0;
    m_szName[0] = 0;
    m_szUserName[0] = 0;
    m_nSceneTempID = m_nSceneID = 0;
    m_nLastCastSlotID = 0;
    m_nExp = 0;
    m_nExpToday = 0;
    m_dwPkValue = 0;
    m_ItemBag.SetOwner(this);
    m_Stall.SetOwner(this);
    m_Title.SetOwner(this);
    m_Trade.SetOwner(this);
    m_SceneListNode.pNext = m_SceneListNode.pPrev = NULL;
    m_SaveListNode.pNext = m_SaveListNode.pPrev = NULL;
    m_dwForbidChatTime = 0;
    m_nNormalChatTimer = 0;
    m_nGroupChatTimer = 0;
    m_nComboKill = 0;
    m_nGreyNameFrame = INT_MAX;
    m_poHero = NULL;
    memset(m_dwVIPTime, 0, sizeof(m_dwVIPTime));
    m_poCurrentSlot = NULL;
    m_dwCreateTime = 0;
    m_dwSaveTime = 0;
    memset(&m_EnterBattlePos, 0, sizeof(m_EnterBattlePos));
    memset(&m_StaticReturnPos, 0, sizeof(m_StaticReturnPos));
    memset(&m_RevivePos, 0, sizeof(m_RevivePos));
    m_dwJumpRandomNumber = 0;

    m_nPkMode = rpmInvalid;
    m_dwGuildID = INVALID_OBJECT_ID;
    m_dwInteractNpcID = INVALID_OBJECT_ID;
    m_nInteractOption = 0;
    m_dwLastAdjacentNoTeamRoleFrame = 0;
    m_dwLastAdjacentTeamFrame = 0;
    m_nLoginFrame = 0;
    m_nLevelUpFrame = 0;
    m_nLoadingSceneEndFrame = 0;
    m_dwBattleTeamID = 0;
    memset(&m_SpellCache, 0, sizeof(m_SpellCache));
    m_dwSpellCacheTimeoutFrm = 0;
    m_nLastSaveFrame = 0;
    m_nServerGroupID = 0;
    m_bHidden = FALSE;
    m_nGMLevel = 0;
    m_nNextFreeReviveFrm = 0;
    m_nKilledByPlayer = 0;
    m_nNextResetKilledTimesByPlayerFrm = 0;
    m_eLogoutType = rltInvalid;
    m_nLogoutFrame = 0;
    m_dwTeamID = 0;
    m_dwTeleportID = 0;
    m_nExtraExp = 0;
    m_nActivity = 0;
    m_nActivityReward = 0;
    m_dwClientUploadTarget = 0;

    m_nConsecutiveKillNpcCount = 0;
    m_nConsecutiveKillNpcFrame = 0;
    m_nConsecutiveKillNpcExp = 0;

    m_nConsecutiveKillPlayerFrame = 0;
    m_nConsecutiveKillPlayerCount = 0;

    m_dwJigsawTimeoutTime = 0;
    m_nJigsawRemainTime = 0;
    m_nAntiCheatLevel = DEFAULT_ROLE_ANTI_CHEAT_LEVEL;
    m_nAntiCheatPunishLevel = 0;
    m_nJigsawAnswerIndex = 0;
    m_dwJigsawSn = 0;

    memset(m_nMomentaryKillNpcCount, 0, sizeof(m_nMomentaryKillNpcCount));
    memset(m_nMomentaryKillNpcExp, 0, sizeof(m_nMomentaryKillNpcExp));

    m_llLastSuccessPickupFrame = 0;
    m_dwLastLineSceneTempID = 0;
    m_dwLastLineSceneID = 0;
    m_nPetState = psInvalid;
    m_nPetCount = 0;
    memset(m_Pet, 0, sizeof(m_Pet));

    memset(&m_LastCheckInInfo, 0, sizeof(m_LastCheckInInfo));

    m_dwTotalExerciseTime = 0;
    m_dwStartExerciseTime = 0;
    m_bDoingSubTime = FALSE;

#ifndef _ROBOT
    m_poGpkAuth = NULL;
#endif

#ifdef _ROBOT
    m_nServerFrame = 0;
    m_nFindState = 0;
    m_nFindTimer = 0;
    m_nFindTimes = 0;
    m_nPathSide = 0;
#endif  // _ROBOT

    m_bIpWhiteList = FALSE;
    m_bUserWhiteList = FALSE;
    m_dwIpAddr = 0;

    // Additional custom code
    m_dwGameTimeCard = 0;
    m_bDoTellGameTimeCard = FALSE;
    m_bHaveTime = TRUE;
    m_dwRuneID = 0;
    m_nRuneNormal_Warrior = 0;
    m_nRuneNormal_Mage = 0;
    m_nRuneNormal_Taoist = 0;
    m_nRuneAdvanced_Warrior = 0;
    m_nRuneAdvanced_Mage = 0;
    m_nRuneAdvanced_Taoist = 0;
    m_nLuckRootCount = 0;

}

CRole::~CRole()
{
    UnInit();
}

INT32 CRole::GetSpecies(void)
{
    return spcPlayer;
}

const TCHAR* CRole::GetName(void)
{
    return m_szName;
}

BOOL CRole::Init(UINT32 dwObjID, UINT32 dwSessionID)
{
    INT32 nRetCode = 0;

    nRetCode = CCharacter::Init(dwObjID);
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
    m_nServerGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
    LOG_PROCESS_ERROR(IsValidGroup(m_nServerGroupID));
#endif //_ROBOT

    m_bIsLoading = FALSE;
    m_szName[0] = 0;
    m_szUserName[0] = 0;
    m_byHairType = 0;
    m_byHairColor = 0;
    m_byFaceType = 0;
    m_bySkinColor = 0;
    m_nSceneTempID = m_nSceneID = 0;
    m_dwTeamID = 0;
    m_dwBattleTeamID = 0;
    m_dwSessionID = dwSessionID;
    m_nExp = 0;
    m_nExtraExp = 0;
    m_nExpToday = 0;
    m_nLastCastSlotID = 0;
    m_dwInteractNpcID = 0;
    m_nInteractOption = 0;
    m_dwPkValue = 0;
    m_nComboKill = 0;
    m_nGreyNameFrame = INT_MAX;
    m_dwSpellCacheTimeoutFrm = 0;
    m_poHero = NULL;
    m_nFaction = 0;
    m_bHidden = FALSE;
    m_nGMLevel = 0;
    m_dwTeleportID = 0;
    m_dwJumpRandomNumber = g_Random(0x1000);

    m_eLogoutType = rltInvalid;
    m_nLogoutFrame = 0;
    memset(m_dwVIPTime, 0, sizeof(m_dwVIPTime));

    m_nConsecutiveKillNpcCount = 0;
    m_nConsecutiveKillNpcFrame = 0;
    m_nConsecutiveKillNpcExp = 0;

    m_nConsecutiveKillPlayerFrame = 0;
    m_nConsecutiveKillPlayerCount = 0;

    memset(m_nMomentaryKillNpcCount, 0, sizeof(m_nMomentaryKillNpcCount));
    memset(m_nMomentaryKillNpcExp, 0, sizeof(m_nMomentaryKillNpcExp));

    ClearSpellCache();
    m_ActivedMountList.clear();

    m_SpellBook.SetRole(this);

    SetBodyType(body1x1);

    for (INT32 i = 0; i < qssSlotCount; i++)
        m_SpellSlot[i].Lock(FALSE);

    for (INT32 i = 0; i < qssSlotCount; i++)
        m_SpellSlotForTransform[i].Lock(FALSE);

    m_poCurrentSlot = &m_SpellSlot[0];

    m_dwLastAdjacentTeamFrame = 0;
    m_dwLastAdjacentNoTeamRoleFrame = 0;

    m_eState = rsInvalid;
    m_eFcmState = fcmsFull;
    m_bFreeMode = FALSE;
    m_bInJump = FALSE;
    m_bInLogin = FALSE;
    m_bKicked = FALSE;
    m_nPkMode = rpmPeace;

    m_bIpWhiteList = FALSE;
    m_bUserWhiteList = FALSE;
    m_dwIpAddr = 0;

    memset(m_nVarList, 0, sizeof(m_nVarList));
    memset(m_nClientVarList, 0, sizeof(m_nClientVarList));
    memset(&m_EnterBattlePos, 0, sizeof(m_EnterBattlePos));
    memset(&m_StaticReturnPos, 0, sizeof(m_StaticReturnPos));
    memset(&m_RevivePos, 0, sizeof(m_RevivePos));

    nRetCode = m_QuestList.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_KillNpcQuest.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_Reputation.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_AchievementList.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_Title.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_Spirit.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_Master.Init(GetObjID());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_ShieldList.Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_Guild.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_Arena.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    m_dwForbidChatTime = 0;
    m_nNormalChatTimer = 0;
    m_nGroupChatTimer = 0;
    m_nLoginFrame = (INT32)CGameWorld::GetInstance().GetGameFrame();
    m_nLevelUpFrame = (INT32)CGameWorld::GetInstance().GetGameFrame();
    m_nLoadingSceneEndFrame = 0xFFFF;
    m_dwGuildID = 0;

    m_dwCreateTime = 0;
    m_dwSaveTime = 0;
    memset(m_lstDungeonRecord, 0, sizeof(m_lstDungeonRecord));
    memset(m_nTeleportPlatform, 0, sizeof(m_nTeleportPlatform));

    m_dwLastDailyRefreshTime = 0;
    m_dwLastWeeklyRefreshTime = 0;

    m_llLastSuccessPickupFrame = 0;

    m_nNextFreeReviveFrm = 0;
    m_nKilledByPlayer = 0;
    m_nNextResetKilledTimesByPlayerFrm = 0;

    m_SceneListNode.pNext = m_SceneListNode.pPrev = NULL;
    m_SaveListNode.pNext = m_SaveListNode.pPrev = NULL;

    m_nActivity = 0;
    m_nActivityReward = 0;

    m_dwLastLineSceneTempID = 0;
    m_dwLastLineSceneID = 0;
    m_nPetState = psFight;
    m_nPetCount = 0;
    memset(m_Pet, 0, sizeof(m_Pet));

    m_dwJigsawTimeoutTime = 0;
    m_nJigsawRemainTime = 0;
    m_nAntiCheatLevel = DEFAULT_ROLE_ANTI_CHEAT_LEVEL;
    m_nAntiCheatPunishLevel = 0;
    m_nJigsawAnswerIndex = 0;
    m_dwJigsawSn = 0;

    memset(&m_LastCheckInInfo, 0, sizeof(m_LastCheckInInfo));

    m_dwTotalExerciseTime = 0;
    m_dwStartExerciseTime = 0;
    m_bDoingSubTime = FALSE;

#ifndef _ROBOT
    m_poGpkAuth = CGameWorld::GetInstance().GetGPK()->AllocAuthObject();
    LOG_PROCESS_ERROR(m_poGpkAuth);
#endif

#ifdef _ROBOT
    m_nServerFrame = 0;
    m_bInBattleQueue = FALSE;
    m_dwReviveFrame = 0;
    m_nFindState = 0;
    m_nFindTimer = 0;
    m_nFindTimes = 0;
    m_nPathSide = 0;

    m_lstMails.clear();
    m_lstAttachments.clear();
    nRetCode = m_AsimovRole.Init(this);
    LOG_PROCESS_ERROR(nRetCode);

    memset(m_szCBFuncName, 0, sizeof(m_szCBFuncName));
    m_nWaitNpcTemplateID = 0;

#endif  // _ROBOT

    // Additional custom code
    m_dwGameTimeCard = 0;
    m_bDoTellGameTimeCard = FALSE;
    m_bHaveTime = TRUE;
    m_dwRuneID = 0;
    m_nRuneNormal_Warrior = 0;
    m_nRuneNormal_Mage = 0;
    m_nRuneNormal_Taoist = 0;
    m_nRuneAdvanced_Warrior = 0;
    m_nRuneAdvanced_Mage = 0;
    m_nRuneAdvanced_Taoist = 0;
    m_nLuckRootCount = 0;

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CRole::UnInit(void)
{
    INT32 nRetCode = 0;

    m_dwSessionID = 0;

#ifndef _ROBOT
    if (m_poGpkAuth != NULL)
    {
        m_poGpkAuth->Release();
        m_poGpkAuth = NULL;
    }
#endif

    nRetCode = m_QuestList.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_KillNpcQuest.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
        m_SpellSlot[i].Clear();

    for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
        m_SpellSlotForTransform[i].Clear();

    if (m_poHero != NULL)
    {
        RecallHero(FALSE);

        nRetCode = m_poHero->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelHero(m_poHero);
        LOG_CHECK_ERROR(nRetCode);

        m_poHero = NULL;
    }

    m_ItemBag.Clear();

    nRetCode = m_Reputation.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_AchievementList.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_Spirit.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_Arena.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    m_SpellBook.UnInit();

    LOG_CHECK_ERROR(m_SceneListNode.pPrev == NULL && m_SceneListNode.pNext == NULL);
    CBaseList::Remove(&m_SceneListNode);
    LOG_CHECK_ERROR(m_SaveListNode.pPrev == NULL && m_SaveListNode.pNext == NULL);
    CBaseList::Remove(&m_SaveListNode);

    nRetCode = CCharacter::UnInit();
    LOG_CHECK_ERROR(nRetCode);

    // Additional custom code
    m_dwGameTimeCard = 0;

    return TRUE;
}

BOOL CRole::JumpTo(INT64 llX, INT64 llY, INT64 llZ, INT32 nFaceDirection)
{
    INT32 nRetCode = 0;
    INT64 llSrcX = 0, llSrcY = 0, llSrcZ = 0;
    INT32 nSrcFaceDirection = 0;
    CScene* poSrcScene = NULL;

    LOG_PROCESS_ERROR(m_eState == rsPlaying);
    LOG_PROCESS_ERROR(m_poScene);

    llSrcX = m_llX;
    llSrcY = m_llY;
    llSrcZ = m_llZ;
    nSrcFaceDirection = GetFaceDirection();
    poSrcScene = m_poScene;

    nRetCode = m_poScene->RemoveRole(this);
    LOG_PROCESS_ERROR(nRetCode);

    SetSceneTempID(m_nSceneTempID);
    SetSceneID(m_nSceneID);
    SetX(llX);
    SetY(llY);
    SetZ(llZ);
    SetFaceDirection(nFaceDirection);

    nRetCode = poSrcScene->AddRole(this, soatInRange);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (m_poScene == NULL && poSrcScene != NULL)
    {
        SetX(llSrcX);
        SetY(llSrcY);
        SetZ(llSrcZ);
        SetFaceDirection(nSrcFaceDirection);

        nRetCode = poSrcScene->AddRole(this, soatInRange);
        if (!nRetCode)
        {
            KICK_ROLE(this, TRUE);
        }
    }

    return FALSE;
}

void CRole::GetNextJumpPoint(INT32& rnX, INT32& rnY, INT32& rnZ)
{
    INT32 nRetCode = 0;
    CLuaScript* poScript = NULL;

    poScript = CScriptMgr::GetInstance().FindScript(ITEM_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    nRetCode = poScript->CallFunction("_get_jump_point", "odd:ddd", this, "CRole", m_nSceneTempID, m_dwJumpRandomNumber, &rnX, &rnY, &rnZ);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:

    return;
}

UINT32 CRole::GetJumpRandomNumber(void)
{
    UINT32 dwTempSeed = g_dwRandSeed;
    UINT32 dwRandNumber = m_dwJumpRandomNumber;

    g_dwRandSeed = m_dwJumpRandomNumber;
    m_dwJumpRandomNumber = g_Random(0x1000);
#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoSyncNextJumpPoint(this);
#endif
    g_dwRandSeed = dwTempSeed;

    return dwRandNumber;
}

struct CTraverseTransferRole
{
    CRole* m_poSelfRole;
    INT64 m_llDestX;
    INT64 m_llDestY;
    INT64 m_llDestZ;
    INT64 m_llRange2;
    INT32 m_nCount;

    bool operator()(CRegion* poRegion)
    {
        LOG_PROCESS_ERROR(poRegion);

        poRegion->TraverseRole(*this);

    Exit0:
        return TRUE;
    }

    bool operator()(CRole* poRole)
    {
        INT32 nRetCode = 0;

        LOG_PROCESS_ERROR(poRole);
        PROCESS_ERROR(poRole != m_poSelfRole);
        PROCESS_ERROR(poRole->GetDistance3D(m_poSelfRole) < m_llRange2);
        PROCESS_ERROR(m_nCount > 0);

        nRetCode = poRole->JumpTo(m_llDestX, m_llDestY, m_llDestZ, poRole->GetFaceDirection());
        LOG_PROCESS_ERROR(nRetCode);

        m_nCount--;

    Exit0:
        return TRUE;
    }
};

BOOL CRole::TransferRole(INT64 llDestX, INT64 llDestY, INT64 llDestZ, INT32 nRange, INT32 nCount)
{
    CTraverseTransferRole TraverseTransferRole;

    LOG_PROCESS_ERROR(m_poRegion);

    TraverseTransferRole.m_poSelfRole = this;
    TraverseTransferRole.m_llDestX = llDestX;
    TraverseTransferRole.m_llDestY = llDestY;
    TraverseTransferRole.m_llDestZ = llDestZ;
    TraverseTransferRole.m_llRange2 = (INT64)nRange * nRange;
    TraverseTransferRole.m_nCount = nCount;

    m_poRegion->Traverse9Region(TraverseTransferRole);

    return TRUE;
Exit0:

    return FALSE;
}

ISpellBook& CRole::GetSpellBook(void)
{
    return m_SpellBook;
}

void CRole::ClearSpellForReloading(void)
{
    CCharacter::ClearSpellForReloading();

    for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
    {
        //		for(INT32 j = 0; j < MAX_RUNE_SLOT; j++)
        //			EquipRune(i, j);

        EquipSpell(i, 0);
    }
}

BOOL CRole::IsLoading(void)
{
    return m_bIsLoading;
}

void CRole::_CheckInteractBT(void)
{
    INT32 nRetCode = 0;

    if (m_InteractCtx.returnValue == brvRunning)
    {
        CNpc* poNpc = (CNpc*)m_InteractCtx.poChar;
        if (poNpc->GetAIProcessor())
        {
            nRetCode = poNpc->GetAIProcessor()->ProcessInteractSyncRun(m_InteractCtx);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
}


// ==================== Custom code begins

// Get the current time-card expiration time (seconds)
UINT32 CRole::GetGameTimeCard_UINT32()
{
    return m_dwGameTimeCard;
}

// Add time-card duration (seconds)
void CRole::AddGameTimeCard(UINT32 seconds)
{
    if (g_ServerConfig.CC.bCustomGameTimeCardEnable == false)
    {
        return;
    }

    UINT32 dwnowTime = CGameWorld::GetInstance().GetGameTime();
    
    // Expired
    if (dwnowTime > m_dwGameTimeCard)
    {
        m_dwGameTimeCard = dwnowTime + seconds;
    }
    // Not expired
    else
    {
        m_dwGameTimeCard += seconds;
    }

    SetFreeMode(FALSE);    // Update m_bFreeMode whenever a time card is consumed
    NotifyRole_GameTimeCard();
}

// Send a private message informing the player of remaining time
void CRole::NotifyRole_GameTimeCard()
{
    if (g_ServerConfig.CC.bCustomGameTimeCardEnable == false)
    {
        return;
    }

    char strMsg[128] = { 0 };
    const char* strTime = get_time_str(m_dwGameTimeCard);

    UINT32 dwnowTime = CGameWorld::GetInstance().GetGameTime();

    // Expired
    if (dwnowTime > m_dwGameTimeCard)
    {
        m_bHaveTime = TRUE;

        const char* strNoTime = "[Vip月卡]已过期!";

        snprintf(strMsg, sizeof(strMsg), "%s", strNoTime);
        std::unique_ptr<char[]> str_utf8(char_to_utf8(strMsg));
        Say(GetObjID(), cltNormal, str_utf8 ? str_utf8.get() : "");
    }
    // Not expired
    else
    {
        m_bHaveTime = FALSE;

        const char* strHaveTime = "[Vip月卡]时间: ";

        snprintf(strMsg, sizeof(strMsg), "%s%s", strHaveTime, strTime);
        std::unique_ptr<char[]> str_utf8(char_to_utf8(strMsg));
        Say(GetObjID(), cltNormal, str_utf8 ? str_utf8.get() : "");
    }
}

// Check whether the time card has expired
BOOL CRole::GetGameTimeCard_HaveTime()
{
    return m_bHaveTime;
}

// Get advanced inscription refinement result
INT32 CRole::GetRuneItemHighResult()
{
    return m_dwRuneID;
}

// Set advanced inscription refinement result
void CRole::SetRuneItemHighResult(INT32 RuneID)
{
    m_dwRuneID = RuneID;
}

// Get inscription node value
INT32 CRole::GetRuneRootCount(INT32 nType, INT32 cls)
{
    // Normal
    if (nType == rrrNormal)
    {
        switch (cls)
        {
        case clsWarrior:
            return m_nRuneNormal_Warrior;
        case clsMage:
            return m_nRuneNormal_Mage;
        case clsTaoist:
            return m_nRuneNormal_Taoist;
        }
    }
    // Advanced
    else if (nType == rrrAdvanced)
    {
        switch (cls)
        {
        case clsWarrior:
            return m_nRuneAdvanced_Warrior;
        case clsMage:
            return m_nRuneAdvanced_Mage;
        case clsTaoist:
            return m_nRuneAdvanced_Taoist;
        }
    }
    return 0;
}

// Increment inscription node value
void CRole::IncRuneRootCount(INT32 nType, INT32 cls)
{
    // Normal
    if (nType == rrrNormal)
    {
        switch (cls)
        {
        case clsWarrior:
        {
            if (m_nRuneNormal_Warrior >= RUNE_ROOT_NORMAL)
                m_nRuneNormal_Warrior = 0;
            else
                m_nRuneNormal_Warrior += 1;
            break;
        }
        case clsMage:
        {
            if (m_nRuneNormal_Mage >= RUNE_ROOT_NORMAL)
                m_nRuneNormal_Mage = 0;
            else
                m_nRuneNormal_Mage += 1;
            break;
        }
        case clsTaoist:
        {
            if (m_nRuneNormal_Taoist >= RUNE_ROOT_NORMAL)
                m_nRuneNormal_Taoist = 0;
            else
                m_nRuneNormal_Taoist += 1;
            break;
        }
        }
    }
    // Advanced
    else if (nType == rrrAdvanced)
    {
        switch (cls)
        {
        case clsWarrior:
        {
            if (m_nRuneAdvanced_Warrior >= RUNE_ROOT_ADVANCED)
                m_nRuneAdvanced_Warrior = 0;
            else
                m_nRuneAdvanced_Warrior += 1;
            break;
        }
        case clsMage:
        {
            if (m_nRuneAdvanced_Mage >= RUNE_ROOT_ADVANCED)
                m_nRuneAdvanced_Mage = 0;
            else
                m_nRuneAdvanced_Mage += 1;
            break;
        }
        case clsTaoist:
        {
            if (m_nRuneAdvanced_Taoist >= RUNE_ROOT_ADVANCED)
                m_nRuneAdvanced_Taoist = 0;
            else
                m_nRuneAdvanced_Taoist += 1;
            break;
        }
        }
    }
}

// Send a private message with the inscription node value
void CRole::NotifyRole_RuneRootCount(INT32 nType, INT32 cls)
{
    char strMsg[128] = { 0 };
    std::unique_ptr<char[]> str_utf8;

    // Normal
    if (nType == rrrNormal)
    {
        const char* str1 = "[普通铭文洗练]-";

        switch (cls)
        {
        case clsWarrior:
        {
            const char* str2 = "[战士] ";
            snprintf(strMsg, sizeof(strMsg), "%s%s%d次", str1, str2, m_nRuneNormal_Warrior);
            str_utf8.reset(char_to_utf8(strMsg));
            break;
        }
        case clsMage:
        {
            const char* str2 = "[法师] ";
            snprintf(strMsg, sizeof(strMsg), "%s%s%d次", str1, str2, m_nRuneNormal_Mage);
            str_utf8.reset(char_to_utf8(strMsg));
            break;
        }
        case clsTaoist:
        {
            const char* str2 = "[道士] ";
            snprintf(strMsg, sizeof(strMsg), "%s%s%d次", str1, str2, m_nRuneNormal_Taoist);
            str_utf8.reset(char_to_utf8(strMsg));
            break;
        }
        }
    }
    // Advanced
    else if (nType == rrrAdvanced)
    {
        const char* str1 = "[高级铭文洗练]-";

        switch (cls)
        {
        case clsWarrior:
        {
            const char* str2 = "[战士] ";
            snprintf(strMsg, sizeof(strMsg), "%s%s%d次", str1, str2, m_nRuneAdvanced_Warrior);
            str_utf8.reset(char_to_utf8(strMsg));
            break;
        }
        case clsMage:
        {
            const char* str2 = "[法师] ";
            snprintf(strMsg, sizeof(strMsg), "%s%s%d次", str1, str2, m_nRuneAdvanced_Mage);
            str_utf8.reset(char_to_utf8(strMsg));
            break;
        }
        case clsTaoist:
        {
            const char* str2 = "[道士] ";
            snprintf(strMsg, sizeof(strMsg), "%s%s%d次", str1, str2, m_nRuneAdvanced_Taoist);
            str_utf8.reset(char_to_utf8(strMsg));
            break;
        }
        }
    }

    Say(GetObjID(), cltDialog, str_utf8 ? str_utf8.get() : "");
}

void CRole::ResetLuckRootCount()
{
    m_nLuckRootCount = 0;
}

// Get blessing oil node value
INT32 CRole::GetLuckRootCount()
{
    return m_nLuckRootCount;
}

// Increment blessing oil node value
void CRole::IncLuckRootCount()
{
    m_nLuckRootCount += 1;
}

// Send a private message with the blessing oil node value
void CRole::NotifyRole_LuckRootCount()
{
    char strMsg[128] = { 0 };
    const char* str1 = "祝福油节点-";
    snprintf(strMsg, sizeof(strMsg), "%s[%d]", str1, m_nLuckRootCount);
    std::unique_ptr<char[]> str_utf8(char_to_utf8(strMsg));
    Say(GetObjID(), cltDialog, str_utf8 ? str_utf8.get() : "");
}

// ==================== Custom code ends

void CRole::_On1SecMainLoop(void)
{
    INT32 nRetCode = 0;
    INT64 llCurFrame = CGameWorld::GetInstance().GetGameFrame();
    UINT32 dwCurTime = CGameWorld::GetInstance().GetGameTime();

    LOG_PROCESS_ERROR(m_poScene);

#ifndef _ROBOT
    if (m_Trade.IsTrading())
    {
        if (GetMoveState() == CMS_DIE || GetFlag().CheckFlag(cfInCombat) || IsFreeMode())
        {
            m_Trade.CancelTrade();
        }
    }

    if (m_Stall.GetStallState() != essNotOpen)
    {
        if (GetMoveState() == CMS_DIE || GetFlag().CheckFlag(cfInCombat) || IsFreeMode())
        {
            m_Stall.RealClose();
        }
    }

    if (!IsDead())
        m_ItemBag.MainLoop();

    // Additional code - check time-card expiration
    if (g_ServerConfig.CC.bCustomGameTimeCardEnable == TRUE && // Custom time-card feature enabled
        dwCurTime > m_dwGameTimeCard &&                        // Time card expired
        IsFreeMode() == FALSE &&                               // m_bFreeMode flag == FALSE
        GetLevel() > g_ServerConfig.CC.nCustomFreeRoleLevel)   // Current level exceeds the free character level
    {
        // Temporarily disabled; an active time card currently doubles picked-up gold
        //SetFreeMode(TRUE);    // Only update m_bFreeMode when the time card expires

        // Tell the player the time card expired
        if (m_bDoTellGameTimeCard == FALSE)
        {
            NotifyRole_GameTimeCard();
            m_bDoTellGameTimeCard = TRUE;
            m_bHaveTime = TRUE;
        }
    }

    // Default code - check the original time-card system and change maps on expiry
    //if (IsFreeMode() && GetState() == rsPlaying && GetSceneTempID() != g_ServerConfig.MS.nFreeRoleSceneID && GetSceneTempID() != g_GWConfig.ArrestConfig.nSceneTempID)
    //    CGameWorld::GetInstance().JumpTo(this, g_ServerConfig.MS.nFreeRoleSceneID, 0,
    //    g_ServerConfig.MS.nFreeRoleScenePosX, g_ServerConfig.MS.nFreeRoleScenePosY,
    //    g_ServerConfig.MS.nFreeRoleScenePosZ, g_ServerConfig.MS.nFreeRoleSceneFaceDirection);

#endif // _ROBOT

    if (llCurFrame >= m_nNextResetKilledTimesByPlayerFrm)
        m_nKilledByPlayer = 0;

    if (!GetFlag().CheckFlag(cfDamageStatistics))
        ResetDamageStat();

    if (g_GWConfig.DebugOption.anticheat)
    {
        if (m_poScene->GetSceneData()->bEnableAntiCheat)
        {
            m_nJigsawRemainTime--;

            if (m_nJigsawRemainTime <= 0)
            {
                GenerateAntiCheatJigsaw();
            }

            //if (m_dwNextJigsawTime != 0 && m_dwNextJigsawTime <= dwCurTime)
            //{
            //	m_dwJigsawTimeoutTime = dwCurTime + g_GWConfig.nAntiCheatTimeout;
            //
            //	GenerateAntiCheatJigsaw();
            //}

#ifndef _ROBOT
            if (m_dwJigsawTimeoutTime != 0 && m_dwJigsawTimeoutTime <= dwCurTime)
            {
                INT32 nErrCode = FailAntiCheat(FALSE, TRUE);

                nRetCode = CCliMessageHandler::GetInstance().DoDLAntiCheatInfo(this, aclaAnswer, -1, nErrCode, NULL, NULL);
                LOG_CHECK_ERROR(nRetCode);
            }
#endif
        }
    }

    if (!IsDead())
    {
#ifndef _ROBOT
        _CheckInteractBT();

        if (m_poHero != NULL && m_poHero->IsSummoned() && !m_poHero->IsDead())
        {
            INT64 llDist = g_GetDistance2D(m_llX, m_llY, m_poHero->GetX(), m_poHero->GetY());

            if (llDist > MAX_HERO_DISTANCE * MAX_HERO_DISTANCE)
            {
                nRetCode = m_poHero->JumpTo(m_llX, m_llY, m_llZ, m_poHero->GetFaceDirection());
                LOG_CHECK_ERROR(nRetCode);
            }
        }

        if (m_dwFocusID != 0)
        {
            CCharacter* poTarget = CGameWorld::GetInstance().FindValidCharacter(m_dwFocusID);
            if (poTarget == NULL)
                SelectFocus(0);
        }

        nRetCode = _CaculateKillNpc();
        LOG_CHECK_ERROR(nRetCode);

        {
            //////////////////////////////////////////////////////////////////////////
            // optimized revive point code.
            const ZONE_DATA* pcZone = NULL;
            const GRID_DATA* pcGrid = NULL;
            const SCENE_DATA* pcSceneData = NULL;

            LOG_PROCESS_ERROR(m_poScene);

            CSceneStaticData* poSceneStaticData = m_poScene->GetStaticData();
            LOG_PROCESS_ERROR(poSceneStaticData);

            pcSceneData = m_poScene->GetSceneData();
            LOG_PROCESS_ERROR(pcSceneData);

            if (pcSceneData->nType == estStatic || pcSceneData->nType == estLine || pcSceneData->nType == estUnique)
            {
                nRetCode = poSceneStaticData->GetLowerGrid(m_llX, m_llY, m_llZ + GRID_WIDTH, pcZone, pcGrid);
                LOG_PROCESS_ERROR(nRetCode);
                LOG_PROCESS_ERROR(pcGrid);
                LOG_PROCESS_ERROR(pcZone);

                if (pcGrid->bInServerPointRange)
                {
                    INT32 nRevivePtID = pcZone->byServerPointIndex;

                    const SERVER_POINT* pt = poSceneStaticData->GetRevivePoint(nRevivePtID);
                    LOG_PROCESS_ERROR(pt);

                    if (m_RevivePos.nX != pt->nX || m_RevivePos.nY != pt->nY || m_RevivePos.nZ != pt->nZ)
                    {
                        m_RevivePos.nSceneID = m_poScene->GetSceneID();
                        m_RevivePos.nSceneTempID = m_poScene->GetSceneTempID();
                        m_RevivePos.nFaceDirection = pt->nDirection;
                        m_RevivePos.nX = pt->nX;
                        m_RevivePos.nY = pt->nY;
                        m_RevivePos.nZ = pt->nZ;
#ifndef _ROBOT
                        CCliMessageHandler::GetInstance().DoActivateRevivePt(this, pt->nX, pt->nY, pt->nZ);
#endif
                    }
                }

            }
            // end of optimized revive point code.
        }

#endif  // not _ROBOT
    }

Exit0:
    return;
}

void CRole::_On8SecMainLoop(void)
{
    INT32 nRetCode = 0;

#ifndef _ROBOT
    if (m_poScene && m_dwTeamID != INVALID_OBJECT_ID)
    {
        nRetCode = CCliMessageHandler::GetInstance().DoMSTeamSyncMember(this);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CCliMessageHandler::GetInstance().DoClientTeamSyncMember(this);
        LOG_CHECK_ERROR(nRetCode);
    }

    if (m_pGridInstance && m_pGridInstance->uCharRef > 1 &&
        m_nBodyType != body0x0 && m_pcGrid && !m_pcGrid->bNotBlockChar)
    {
        LOG_PROCESS_ERROR(m_poScene);

        for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
        {
            const ZONE_DATA* pcZoneData = NULL;
            const GRID_DATA* pcGridData = NULL;

            INT64 llX = m_llX + GRID_WIDTH * g_nAdjacentX[nDirection];
            INT64 llY = m_llY + GRID_WIDTH * g_nAdjacentY[nDirection];

            nRetCode = m_poScene->GetLowerGrid(llX, llY, m_llZ + GRID_WIDTH, pcZoneData, pcGridData);
            if (pcGridData && !pcGridData->bOutOfPoly && pcGridData->uVolume == 0)
            {
                GRID_INSTANCE_DATA* pGridInstance = m_poScene->GetGridInstance(pcZoneData, pcGridData);
                if (pGridInstance && pGridInstance->uCharRef == 0)
                {
                    nRetCode = _GridMove(llX, llY);
                    if (nRetCode)
                    {
                        ForceSyncPos();
                        break;
                    }
                }
            }
        }
    }
#endif  // not _ROBOT

Exit0:
    return;
}

void CRole::_On5MinMainLoop(void)
{
    INT32 nRetCode = 0;

    nRetCode = m_Spirit.MainLoop();
    LOG_CHECK_ERROR(nRetCode);

#ifndef _ROBOT
    m_Title.MainLoop();

    if (m_dwPkValue > 0)
    {
        m_dwPkValue -= g_GWConfig.VIPData[GetVIPLevel()].nDecreasePkValuePer5Min;
        if (m_dwPkValue <= 0)
            m_dwPkValue = 0;
        CCliMessageHandler::GetInstance().DoSyncPkValueChange(this);
    }

    if (!IsFreeMode())
    {
        const ROLE_LEVEL_DATA* data = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(GetLevel());
        LOG_CHECK_ERROR(data);

        if (data)
        {
            m_ItemBag.AddGoldPool(data->m_nGoldPoolPer5Min);
        }
    }

    CAdventure::GetInstance().OnMainLoop(this);

#ifdef _FINAL
    //	CCliMessageHandler::GetInstance().DoGpkAuthData(this);
#endif

    TryWeeklyRefresh();
    TryDailyRefresh();

#endif  // not _ROBOT

Exit0:
    return;
}

void CRole::_On1HourMainLoop(void)
{
    INT32 nRetCode = 0;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSUpdateRankListData(this);
    LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT
}

BOOL CRole::MainLoop(void)
{
    INT32 nRetCode = 0;

    INT64 llCurFrame = CGameWorld::GetInstance().GetGameFrame();
    INT32 eOriginalMoveState = m_eMoveState;

    PROCESS_SUCCESS(m_eState != rsPlaying && m_eState != rsLogout);

    nRetCode = CSceneObject::CheckLoop();
    PROCESS_SUCCESS(!nRetCode);

    nRetCode = CCharacter::MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

    if (eOriginalMoveState == CMS_WALK || eOriginalMoveState == CMS_RUN)
    {
        if (m_eMoveState == CMS_STAND && m_poCastingSpell == NULL)
        {
            nRetCode = CastCachedSpell();
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    m_AuraList.Update(this);

#ifdef _ROBOT
    if (IsRobot())
    {
        if (m_eMoveState == CMS_DIE && m_dwReviveFrame == 0)
        {
            m_dwReviveFrame = llCurFrame + 15 * GAME_FPS;
        }
        else if (m_eMoveState == CMS_DIE && m_dwReviveFrame < llCurFrame)
        {
            if (m_poScene)
            {
                UINT32 dwSceneTempID = m_poScene->GetSceneTempID();
                const SCENE_DATA* poSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(dwSceneTempID);
                if (poSceneData->nType != estArena)
                {
                    nRetCode = CRobotMessageHandler::GetInstance().DoRoleRevive(this);
                    LOG_CHECK_ERROR(nRetCode);
                }
            }
            m_dwReviveFrame = 0;
        }

        if (m_nCastingOverFrm > 0 && m_nCastingOverFrm < llCurFrame)
        {
            m_nCastingOverFrm = 0;

            CSpellInstance* s = GetCastingSpell();
            if (s)
            {
                CRI("role %s CastOverFrm timeout, SetCastingSpell %d null", GetName(), s->GetData()->m_nID);
                s->Release();
                SetCastingSpell(NULL);
            }
        }

        if (CGameWorld::GetInstance().GetGameFrame() % (4 * GAME_FPS) == m_dwObjID % (4 * GAME_FPS))
        {
            nRetCode = CRobotMessageHandler::GetInstance().DoUpdatePos(this);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
#endif  // _ROBOT

#ifndef _ROBOT

    if (m_dwSpellCacheTimeoutFrm > 0 && llCurFrame >= m_dwSpellCacheTimeoutFrm)
    {
        ClearSpellCache();
        m_dwSpellCacheTimeoutFrm = 0;
    }

    if (m_nGreyNameFrame <= CGameWorld::GetInstance().GetGameFrame())
    {
        nRetCode = CCliMessageHandler::GetInstance().DoSyncGreyName(this, FALSE);
        LOG_CHECK_ERROR(nRetCode);

        m_nGreyNameFrame = INT_MAX;
    }

    if ((llCurFrame % GAME_FPS) == (m_dwObjID % (GAME_FPS / 2)))
        _On1SecMainLoop();
    if ((llCurFrame % (8 * GAME_FPS)) == (m_dwObjID % (8 * GAME_FPS)))
        _On8SecMainLoop();
    if ((llCurFrame % (5 * 60 * GAME_FPS)) == (m_dwObjID % (5 * 60 * GAME_FPS)))
        _On5MinMainLoop();
    if ((llCurFrame % (3600 * GAME_FPS)) == (m_dwObjID % (3600 * GAME_FPS)))
        _On1HourMainLoop();

    if (m_bHpChanged)
    {
        CCliMessageHandler::GetInstance().DoSyncHp(this);
        m_bHpChanged = FALSE;
    }
    if (m_bMpChanged)
    {
        CCliMessageHandler::GetInstance().DoSyncMp(this);
        m_bMpChanged = FALSE;
    }

    if (m_poScene && m_poScene->GetDLRecordRole())
    {
        nRetCode = CCliMessageHandler::GetInstance().DoDLRolePosition(this);
        LOG_CHECK_ERROR(nRetCode);
    }

#endif  // _ROBOT

    //DBG("[%4d]role %s:%d @ (%d, %d, %d)", CGameWorld::GetInstance().GetGameFrame(),
    //    m_szName, m_dwObjID, (INT32)(m_llX / POINT_PER_CM), (INT32)(m_llY / POINT_PER_CM), (INT32)(m_llZ / POINT_PER_CM));

Exit1:

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::_GridMove(INT64 llDestX, INT64 llDestY)
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

        pOldGridInstance = m_pGridInstance;  // Save the current m_pGridInstance as the old object
        m_pGridInstance = pGridInstance;     // Assign the new pGridInstance to m_pGridInstance

        nRetCode = SetGridCharRef();
        LOG_CHECK_ERROR(nRetCode);

        if (poDestRegion != m_poRegion)
        {
            nRetCode = _ChangeRegion(poDestRegion);
            LOG_PROCESS_ERROR(nRetCode);
        }

        // Check volume
        nRetCode = CheckVolume();
        LOG_CHECK_ERROR(nRetCode);

        //////////////////////////////////////////////////////////////////////////
        // optimized revive point code.

        PROCESS_SUCCESS(m_RevivePos.nRevivePtID == m_pcZone->byServerPointIndex);

        {
            CSceneStaticData* poSceneStaticData = m_poScene->GetStaticData();
            LOG_PROCESS_ERROR(poSceneStaticData);

            if (m_pcGrid->bInServerPointRange)
            {
                INT32 nRevivePtID = m_pcZone->byServerPointIndex;

                const SERVER_POINT* pt = poSceneStaticData->GetRevivePoint(nRevivePtID);
                LOG_PROCESS_ERROR(pt);

                if (m_RevivePos.nX != pt->nX || m_RevivePos.nY != pt->nY || m_RevivePos.nZ != pt->nZ)
                {
                    m_RevivePos.nSceneID = m_poScene->GetSceneID();
                    m_RevivePos.nSceneTempID = m_poScene->GetSceneTempID();
                    m_RevivePos.nFaceDirection = pt->nDirection;
                    m_RevivePos.nX = pt->nX;
                    m_RevivePos.nY = pt->nY;
                    m_RevivePos.nZ = pt->nZ;
                    m_RevivePos.nRevivePtID = nRevivePtID;
#ifndef _ROBOT
                    CCliMessageHandler::GetInstance().DoActivateRevivePt(this, pt->nX, pt->nY, pt->nZ);
#endif
                }
            }
        }

        // end of optimized revive point code.
        //////////////////////////////////////////////////////////////////////////

    }

Exit1:
    return TRUE;
Exit0:
    return FALSE;

}

INT32 CRole::GetObjType(void)
{
    return otRole;
}

void CRole::SetVariable(INT32 nIndex, INT32 nValue)
{
    INT32 nRetCode = 0;
    LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_ROLE_VARIABLE_COUNT);

    m_nVarList[nIndex] = nValue;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoUpdateRoleVariable(this, nIndex);
    LOG_PROCESS_ERROR(nRetCode);
#endif  // _ROBOT

Exit0:
    return;
}

void CRole::AddVariable(INT32 nIndex, INT32 nCount)
{
    INT32 nRetCode = 0;
    LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_ROLE_VARIABLE_COUNT);

    if ((0x7FFFFFFF - m_nVarList[nIndex]) > nCount)
    {
        m_nVarList[nIndex] += nCount;
    }
    else
    {
        LOG_PROCESS_ERROR(FALSE);
    }

#ifndef _ROBOT

    nRetCode = CCliMessageHandler::GetInstance().DoUpdateRoleVariable(this, nIndex);
    LOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
    return;
}

BOOL CRole::SetClientVariable(INT32 nIndex, INT32 nValue)
{
    LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_CLIENT_VARIABLE_COUNT);

    m_nClientVarList[nIndex] = nValue;

    return TRUE;
Exit0:
    return FALSE;
}

void CRole::UpdateLastSaveFrame(void)
{
    m_nLastSaveFrame = (INT32)CGameWorld::GetInstance().GetGameFrame();
}

BOOL CRole::FinishScene(void)
{
    INT32 nRetCode = 0;
    CAuraInstance* poAura = NULL;

    if (IsDead())
    {
        nRetCode = Revive(crtAtCurrentPos, RATE_MAX);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        m_nCurrentHp = m_AttrList.GetAttr(catMaxHp);
    }

    for (INT32 i = auetNone; i < auetTotal; i++)
    {
        poAura = m_AuraList.BeginAura(i);

        while ((poAura = m_AuraList.NextAura(i, poAura)) != NULL)
        {
            if (poAura->GetAttr(aatCategory) == acNegative)
                RemoveAura(poAura, TRUE);
        }
    }


    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::ForceLeaveTeam(void)
{
    INT32 nRetCode = 0;
    PROCESS_SUCCESS(m_dwTeamID == 0);
    LOG_PROCESS_ERROR(m_poScene == NULL);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSLeaveTeam(this);
    LOG_PROCESS_ERROR(nRetCode);
#endif

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::ActivateTeleportPlatform(UINT8 byIndex)
{
    INT32 nIndex = byIndex / 8;
    INT32 nBit = byIndex % 8;

    LOG_PROCESS_ERROR(nIndex < MAX_TELEPORT_PLATFORM_COUNT / 8);

    m_nTeleportPlatform[nIndex] |= (1 << nBit);

    return TRUE;
Exit0:
    return FALSE;
}

void* CRole::GetTeleportform()
{
    return m_nTeleportPlatform;
}

BOOL CRole::CheckTeleportPlatform(UINT8 byIndex)
{
    INT32 nIndex = byIndex / 8;
    INT32 nBit = byIndex % 8;

    LOG_PROCESS_ERROR(nIndex < MAX_TELEPORT_PLATFORM_COUNT / 8);

    return m_nTeleportPlatform[nIndex] & (1 << nBit);
Exit0:
    return FALSE;
}

void CRole::SetLogoutFrame(UINT32 dwDelayFrame)
{
    m_nLogoutFrame = (INT32)(CGameWorld::GetInstance().GetGameFrame() + dwDelayFrame);
}

BOOL CRole::Logout(ERoleLogoutType eType, UINT32 dwDelayFrame)
{
    INT32 nRetCode = 0;
    INF("role %s SessionID %08X CRole::Logout, eType = %d, dwDelayFrame=%d",
        m_szName, m_dwSessionID, eType, dwDelayFrame);

#ifndef _ROBOT
    switch (eType)
    {
        case rltReturnLogin:
        case rltReturnSelectRole:
        {
            LOG_PROCESS_ERROR(m_eState == rsPlaying);
            SetLogoutFrame(dwDelayFrame);

            nRetCode = CCliMessageHandler::GetInstance().DoClientLogoutTimer(this, TRUE, dwDelayFrame);
            LOG_CHECK_ERROR(nRetCode);
        }
        break;
        case rltDisconnect:
        {
            if (m_eState == rsPlaying)
            {
                SetLogoutFrame(dwDelayFrame);
            }
            else
            {
                LOG_PROCESS_ERROR(m_eState == rsLogout);
                INF("CRole::Logout %s session<%x> won't set dwDelayFrame = %d because eState = %d ", GetName(), GetSessionID(), dwDelayFrame, m_eState);
            }
        }
        break;
        case rltKickout:
        {
            LOG_PROCESS_ERROR(m_eState == rsPlaying);
            SetLogoutFrame(dwDelayFrame);
        }
        break;
        default:
            LOG_PROCESS_ERROR(FALSE);
    }

#endif

    m_eLogoutType = eType;
    m_eState = rsLogout;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::Exit(void)
{
    INT32 nRetCode = 0;

    INF("Role #%s:%d# session <%08X> Exit", m_szName, m_dwObjID, m_dwSessionID);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSUpdateRankListData(this);
    LOG_CHECK_ERROR(nRetCode);
#endif	// _ROBOT

    GetAchievementList().AddVariable(avdOnlineTime,
        (CGameWorld::GetInstance().GetGameFrame() - m_nLoginFrame) / GAME_FPS);
    GetAchievementList().AddVariable(avdLevelOnlineTime,
        (CGameWorld::GetInstance().GetGameFrame() - m_nLevelUpFrame) / GAME_FPS);

    if (m_eState != rsJumpInGSWaitClientLoad)
    {
        LOG_PROCESS_ERROR(m_poScene);
        nRetCode = m_poScene->RemoveRole(this);
        LOG_PROCESS_ERROR(nRetCode);
    }

    // remove pet
    PackUpPet();

#ifndef _ROBOT
    if (m_dwJigsawTimeoutTime > 0)
    {
        INT32 nErrCode = FailAntiCheat(TRUE, FALSE);

        nRetCode = CCliMessageHandler::GetInstance().DoDLAntiCheatInfo(this, aclaAnswer, -1, nErrCode, NULL, NULL);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CCliMessageHandler::GetInstance().DoSaveAndUnlockRole(this);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    SetState(rsWaitUnlockRoleData);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::UpdateDungeonRecordMask(UINT32 dwSceneTempID)
{
    LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);

    m_lstDungeonRecord[dwSceneTempID].bEntered |= 1;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::UpdateDungeonEnterCount(UINT32 dwSceneTempID)
{
    LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
    LOG_PROCESS_ERROR(m_lstDungeonRecord[dwSceneTempID].uEnterCount < 0x7F);

    m_lstDungeonRecord[dwSceneTempID].uEnterCount++;

    return TRUE;
Exit0:
    return FALSE;
}

void CRole::ClearDungeonEnterCount(void)
{
    for (INT32 nIndex = 0; nIndex < MAX_SCENE_TEMPLATE_COUNT; nIndex++)
    {
        m_lstDungeonRecord[nIndex].uEnterCount = 0;
    }
}

BOOL CRole::IsFirstPassDungeon(UINT32 dwSceneTempID)
{
    LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);

    return m_lstDungeonRecord[dwSceneTempID].bEntered == 0;
Exit0:
    return FALSE;
}

UINT8 CRole::GetDungeonRecordMask(UINT32 dwSceneTempID)
{
    LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);

    return m_lstDungeonRecord[dwSceneTempID].bEntered;
Exit0:
    return 0;
}

void CRole::GetDungeonRecord(UINT8* pbyDungeonRecord, INT32 nSize)
{
    LOG_PROCESS_ERROR(pbyDungeonRecord);
    LOG_PROCESS_ERROR(nSize == sizeof(m_lstDungeonRecord));

    memcpy(pbyDungeonRecord, m_lstDungeonRecord, nSize);

Exit0:
    return;
}

UINT8 CRole::GetDungeonEnterCount(UINT32 dwSceneTempID)
{
    LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);

    return m_lstDungeonRecord[dwSceneTempID].uEnterCount;
Exit0:
    return 0xF;
}

BOOL CRole::CheckDungeonRecordMask(UINT32 dwSceneTempID)
{
    LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);

    return m_lstDungeonRecord[dwSceneTempID].bEntered == 1;
Exit0:
    return FALSE;
}

BOOL CRole::ForceSyncPos(void)
{
    INT32 nRetCode = 0;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncRolePos(this);
    LOG_PROCESS_ERROR(nRetCode);
#endif  // _ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::_ChangeRegion(CRegion* poDestRegion)
{
    INT32 nRetCode = 0;
    INT32 nDirection = 0;
    INT32 nOldDirection = 0;

    LOG_PROCESS_ERROR(poDestRegion);
    LOG_PROCESS_ERROR(m_poRegion);

    //DBG("change region from (%d, %d) to (%d, %d)", m_poRegion->GetRegionX(), m_poRegion->GetRegionY(),
    //    poDestRegion->GetRegionX(), poDestRegion->GetRegionY());

    nDirection = g_GetDirection(m_poRegion->GetRegionX(), m_poRegion->GetRegionY(),
        poDestRegion->GetRegionX(), poDestRegion->GetRegionY()) * DT_DIRECTION_8_COUNT / MAX_DIRECTION_COUNT;

    for (INT32 nIndex = 1; nIndex <= g_nAdjList[nDirection][0]; nIndex++)
    {
        CRegion* poOldRegion;
        CRegion* poNewRegion;

        poNewRegion = poDestRegion->GetConnectedRegion(g_nAdjList[nDirection][nIndex]);

        nOldDirection = (g_nAdjList[nDirection][nIndex] + DT_DIRECTION_8_COUNT / 2) % DT_DIRECTION_8_COUNT;
        poOldRegion = m_poRegion->GetConnectedRegion(nOldDirection);

#ifndef _ROBOT
        if (poNewRegion)
        {
            if (!m_bHidden)
            {
                nRetCode = CCliMessageHandler::GetInstance().DoCharAppear(this, poNewRegion, soatInRange);
                LOG_CHECK_ERROR(nRetCode);
            }

            nRetCode = CCliMessageHandler::GetInstance().DoSyncNewCharList(this, poNewRegion);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CCliMessageHandler::GetInstance().DoSyncDoodadList(this, poNewRegion);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CCliMessageHandler::GetInstance().DoSyncDropList(this, poNewRegion);
            LOG_CHECK_ERROR(nRetCode);
        }

        if (poOldRegion)
        {
            if (!m_bHidden)
            {
                nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(this, poOldRegion, sodtOutOfRange);
                LOG_CHECK_ERROR(nRetCode);
            }

            nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappearList(this, poOldRegion);
            LOG_CHECK_ERROR(nRetCode);
        }
#endif  // _ROBOT
    }

    nRetCode = m_poRegion->RemoveRole(this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = poDestRegion->AddRole(this);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::CheckDistance(CSceneObject* poSceneObject)
{
    INT64 llDistance3D;
    INT64 llMaxDistance3D;

    LOG_PROCESS_ERROR(poSceneObject);

    PROCESS_ERROR(poSceneObject->GetScene() == m_poScene);

    llDistance3D = GetDistance3D(poSceneObject);
    llMaxDistance3D = g_GWConfig.nNpcInteractDistance * POINT_PER_CM;
    llMaxDistance3D *= llMaxDistance3D;
    PROCESS_ERROR(llDistance3D < llMaxDistance3D);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CRole::CanInteract(CSceneObject* poSceneObject)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poSceneObject);

    if (poSceneObject->GetObjType() == otRole || poSceneObject->GetObjType() == otNpc)
    {
        nRetCode = CanAttack((CCharacter*)poSceneObject);
        PROCESS_ERROR_RET_CODE(!nRetCode, errNpcOpponentTarget);
    }

    if (poSceneObject->GetObjType() == otNpc)
    {
        CNpc* poNpc = (CNpc*)poSceneObject;
        LOG_PROCESS_ERROR(poNpc->GetNpcData());

        if (poNpc->GetNpcData()->bExclusive)
            PROCESS_ERROR_RET_CODE(poNpc->GetExclusiveOwnerID() == m_dwObjID, errNpcOpponentTarget);
        //if (poNpc->GetAIProcessor())
        //    PROCESS_ERROR_RET_CODE(poNpc->GetAIProcessor()->GetFightState() != afsIdle, errNpcInCombat);
    }

    nRetCode = CheckDistance(poSceneObject);
    PROCESS_ERROR_RET_CODE(nRetCode, errNpcTooFar);

    nResult = errSuccess;
Exit0:
    return nResult;
}

BOOL CRole::Interact(CNpc* poNpc)
{
    INT32 nRetCode = 0;
    BTNod* pInteractBT = NULL;
    const AI_TEMPLATE* pAITemplate = NULL;
    char szDialogContent[MAX_DIALOG_CONTENT_LEN];

    LOG_PROCESS_ERROR(poNpc);

    if (poNpc->GetNpcData()->nHandbookType >= htNpcBegin && poNpc->GetNpcData()->nHandbookType < htNpcEnd)
        GetAchievementList().ActivateHandbook(poNpc->GetNpcData()->nHandbookType, poNpc->GetNpcData()->nHandbookIndex);

    if (poNpc->GetAIProcessor())
    {
        if (poNpc->GetAIProcessor()->GetInteractBT())
        {
            pInteractBT = poNpc->GetAIProcessor()->GetInteractBT();
            PROCESS_ERROR(pInteractBT);

            pAITemplate = CAIManager::GetInstance().GetTemplate(poNpc->GetAIProcessor()->GetAIID());
            LOG_PROCESS_ERROR(pAITemplate);

            memset(&m_InteractCtx, 0, sizeof(m_InteractCtx));
            m_InteractCtx.callFrame[0].nod = pInteractBT;
            m_InteractCtx.callFrameCount = 1;
            m_InteractCtx.poScript = pAITemplate->pScript;

            m_InteractCtx.eEventType = evtAIInteract;
            m_InteractCtx.dwRoleID = m_dwObjID;
            m_InteractCtx.eOwnerType = botAI;
            m_InteractCtx.poChar = poNpc;

            BTCtx_run(m_InteractCtx);
            PROCESS_SUCCESS(m_InteractCtx.returnValue == brvSuccess || m_InteractCtx.returnValue == brvRunning);
        }
        poNpc->GetAIProcessor()->SetInteractState(TRUE);
    }

    poNpc->Stand(FALSE);

    strxcpy(szDialogContent, "<#Dft>", MAX_DIALOG_CONTENT_LEN);


#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoOpenDialog(this, poNpc->GetObjID(), szDialogContent);
    LOG_PROCESS_ERROR(nRetCode);
#endif  // _ROBOT

Exit1:
    m_dwInteractNpcID = poNpc->GetObjID();
    m_nInteractOption = 0;

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CRole::SelectOption(INT32 nOption)
{
    INT32 nResult = errUnknownError;

    PROCESS_ERROR_RET_CODE(m_InteractCtx.eRunningType == brtWaitSelect, errNpcDialogTimeout);

    m_nInteractOption = nOption;

    m_InteractCtx.returnValue = brvSuccess;
    m_InteractCtx.eRunningType = brtInvalid;

    BTCtx_run(m_InteractCtx);

    m_nInteractOption = 0;

    nResult = errSuccess;
Exit0:
    return nResult;
}

BOOL CRole::CheckOption(INT32 nOption)
{
    return (nOption == m_nInteractOption);
}

BOOL CRole::StopInteract(void)
{
    if (m_dwInteractNpcID != INVALID_OBJECT_ID)
    {
        CNpc* poNpc = CGameWorld::GetInstance().FindNpc(m_dwInteractNpcID);
        if (poNpc && poNpc->GetAIProcessor())
            poNpc->GetAIProcessor()->SetInteractState(FALSE);

        m_dwInteractNpcID = INVALID_OBJECT_ID;
        m_nInteractOption = 0;
    }

    return TRUE;
}

CSpellSet* CRole::CreateSpellSet(INT32 nSpellID)
{
    INT32 nRetCode = 0;
    CSpellSet* poSet = NULL;

    {
        CSpellSet* poExistSet = m_SpellBook.GetSpellSet(nSpellID);
        if (poExistSet)
            return poExistSet;
    }

    poSet = CGameWorld::GetInstance().NewSpellSet();
    LOG_PROCESS_ERROR(poSet);

    poSet->Init(nSpellID, -1, FALSE);

    nRetCode = m_SpellBook.AddSpellSet(poSet);
    LOG_PROCESS_ERROR(nRetCode);

    return poSet;

Exit0:
    if (poSet)
        CGameWorld::GetInstance().DelSpellSet(poSet);

    return NULL;
}


INT32 CRole::LearnSpell(INT32 nSpellID, INT32 nSpellLevel, BOOL bNeedSave)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    CSpellTemplate* e = NULL;
    CSpellSet* poSet = NULL;
    INT32 nHighestLevel = 0;

    if (nSpellID == 0)
        return errSuccess;

    nHighestLevel = CSpellConfig::GetInstance().GetHighestLevel(nSpellID);
    LOG_PROCESS_ERROR_RET_CODE(nSpellLevel <= nHighestLevel, errUnknownError);

    e = CSpellConfig::GetInstance().FindSpellTemplate(nSpellID, nSpellLevel, 0);
    LOG_PROCESS_ERROR_RET_CODE(e, errSpellNotExists);

#ifdef _FINAL
    LOG_PROCESS_ERROR_RET_CODE((e->GetAttr(satClass) & (1 << m_nClass)), errSpellLearningClassError);
#endif

    LOG_PROCESS_ERROR_RET_CODE(m_nLevel >= e->GetAttr(satRequireLevel), errSpellLearningLevelError);

    poSet = m_SpellBook.GetSpellSet(nSpellID);
    PROCESS_ERROR_RET_CODE(poSet == NULL || poSet->GetLevel() < 0, errSpellLearnt);

    poSet = CreateSpellSet(nSpellID);
    LOG_PROCESS_ERROR_RET_CODE(poSet, errSpellNotExists);

    poSet->Init(nSpellID, nSpellLevel, bNeedSave);

    LearnSpell(e->GetAttr(satNextSpell), nSpellLevel, FALSE);
Exit1:

#ifndef _ROBOT
    PushEquippingSpell(e);
    nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(e->GetAttr(satOnLearn), this, e->GetExParam());
    LOG_CHECK_ERROR(nRetCode >= 0);
    PopEquippingSpell();
#endif  // _ROBOT


    if (!IsLoading() && e->GetAttr(satCanEquip))
    {
#ifndef _ROBOT
        CCliMessageHandler::GetInstance().DoLearnSpell(this, nSpellID, nSpellLevel);
        CCliMessageHandler::GetInstance().DoDLLearnSpell(this, nSpellID, nSpellLevel);
#endif  // _ROBOT
    }


    return errSuccess;
Exit0:
    CRI("Learn spell Error: %d-%d", nSpellID, nSpellLevel);
    return nResult;
}

BOOL CRole::RemoveSpell(INT32 nSpellID)
{
    INT32 nRetCode = 0;
    CSpellTemplate* e = NULL;
    CSpellSet* poSet = NULL;

    PROCESS_SUCCESS(nSpellID == 0);

    //	DBG("remove spell: %d", nSpellID);

    poSet = m_SpellBook.GetSpellSet(nSpellID);
    LOG_PROCESS_ERROR(poSet);

    e = CSpellConfig::GetInstance().FindSpellTemplate(poSet->GetID(), poSet->GetActualLevel(), poSet->GetActualRuneSlot());
    LOG_PROCESS_ERROR(e);

    nRetCode = RemoveSpell(e->GetAttr(satNextSpell));
    LOG_CHECK_ERROR(nRetCode);

    PushEquippingSpell(e);
    nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(e->GetAttr(satOnLearn), this, e->GetExParam());
    LOG_CHECK_ERROR(nRetCode >= 0);
    nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(e->GetAttr(satOnForget), this, e->GetExParam());
    LOG_CHECK_ERROR(nRetCode >= 0);
    PopEquippingSpell();

    m_SpellBook.RemoveSpell(nSpellID);

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

INT64 CRole::AddExp(INT64 llExp, BOOL bByKillNpc, BOOL bDailyLimitExp)
{
    INT32 nRetCode = 0;
    const ROLE_LEVEL_DATA* poLevel = NULL;
    const ROLE_LEVEL_DATA* poNextLevel = NULL;
    BOOL bLevelUp = FALSE;
    INT32 nLevel = GetLevel();
    INT64 llExtraExp = 0;

    PROCESS_ERROR(!IsDead());
    PROCESS_ERROR(!IsFreeMode());
    PROCESS_ERROR(llExp > 0);
    PROCESS_ERROR(m_eFcmState != fcmsNone);

    if (m_eFcmState == fcmsHalf)
        llExp /= 2;

    poLevel = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(nLevel);
    LOG_PROCESS_ERROR(poLevel);

    poNextLevel = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(nLevel + 1);
    PROCESS_ERROR(poNextLevel);

    if (bByKillNpc)
    {
        if (m_nExtraExp > 0)
        {
            llExtraExp = llExp * g_GWConfig.nExtraExpRate / RATE_MAX;
            if (llExtraExp > m_nExtraExp)
                llExtraExp = m_nExtraExp;
            llExp += llExtraExp;
            m_nExtraExp -= llExtraExp;
        }
    }

    if (bDailyLimitExp)
    {
        if (m_nExpToday >= poLevel->m_nDailyExpLimit)
            llExp = 0;
        else if (m_nExpToday + llExp > poLevel->m_nDailyExpLimit)
            llExp = poLevel->m_nDailyExpLimit - m_nExpToday;
    }

    if (bByKillNpc && m_dwTeamID != INVALID_OBJECT_ID && (m_Master.IsStudent() || m_Master.IsTeacher()))
    {
        CTeamInGS* poTeam = CGameWorld::GetInstance().FindTeam(m_dwTeamID);
        if (poTeam)
        {
            for (INT32 nMemberIndex = 0; nMemberIndex < poTeam->GetMaxMemberCount(); nMemberIndex++)
            {
                if (poTeam->GetMemberTable(nMemberIndex).dwRoleID == INVALID_OBJECT_ID ||
                    poTeam->GetMemberTable(nMemberIndex).dwRoleID == m_dwObjID)
                    continue;

                CRole* poMemberRole = CGameWorld::GetInstance().FindValidRole(poTeam->GetMemberTable(nMemberIndex).dwRoleID);
                if (poMemberRole == NULL)
                    continue;

                INT64 llMaxDistance = g_GWConfig.nExpShareDistance * POINT_PER_CM * g_GWConfig.nExpShareDistance * POINT_PER_CM;
                if (poMemberRole->GetDistance3D(this) > llMaxDistance)
                    continue;

                if (m_Master.IsMyTeacher(poMemberRole->GetObjID()) && m_nLevel < GRADUATE_LEVEL)
                {
                    m_Master.AddStudentRewardExp((INT32)(llExp * g_GWConfig.nStudentExpRewardRate / RATE_MAX));
                    poMemberRole->GetItemBag()->AddMoney(MONEYTYPE_MORAL,
                        (INT32)(llExp * g_GWConfig.nMoralRate / RATE_MAX), MONEYLOG_EXP_MORAL, 0);
                    poMemberRole->GetMaster().AddMasterExp(m_dwObjID, (INT32)(llExp * g_GWConfig.nMasterExpRate / RATE_MAX));
                }
                if (poMemberRole->GetMaster().IsMyTeacher(m_dwObjID) && poMemberRole->GetLevel() < GRADUATE_LEVEL)
                    m_Master.AddTeacherRewardExp(poMemberRole->GetObjID(), (INT32)(llExp * g_GWConfig.nTeacherExpRewardRate / RATE_MAX));
            }
        }

    }

    m_nExp += (INT32)llExp;
    if (m_nExp < 0)
        m_nExp = INT_MAX;
    if (bDailyLimitExp)
        m_nExpToday += llExp;

    while (m_nExp >= poLevel->m_nMaxExp && nLevel < g_GWConfig.nLevelLimit)
    {
        poNextLevel = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(nLevel + 1);
        if (poNextLevel)
        {
            m_nExp -= poLevel->m_nMaxExp;
            nLevel++;
            poLevel = poNextLevel;
            bLevelUp = TRUE;

#ifndef _ROBOT
            nRetCode = CCliMessageHandler::GetInstance().DoDLLevelUp(this, GetObjID(), nLevel);
            LOG_CHECK_ERROR(nRetCode);

            GetAchievementList().SetVariable(avdLevelOnlineTime, 0);
            m_nLevelUpFrame = CGameWorld::GetInstance().GetGameFrame();
#endif  // _ROBOT
        }
        else
        {
            m_nExp = poLevel->m_nMaxExp;
            break;
        }
    }

    if (bLevelUp)
        SetLevel(poLevel->m_nLevel);

    if (m_eState == rsPlaying)
    {
#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoExpChange(this, llExp,
            poLevel->m_nMaxExp, m_nExpToday, poLevel->m_nDailyExpLimit, llExtraExp);
        LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT
    }

Exit0:
    return llExp;
}

void CRole::SubExp(INT32 nExp)
{
    LOG_PROCESS_ERROR(m_nExp >= nExp);

    m_nExp -= nExp;

    if (m_eState == rsPlaying)
    {
#ifndef _ROBOT
        INT32 nRetCode = 0;
        const ROLE_LEVEL_DATA* poLevel = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(GetLevel());
        LOG_PROCESS_ERROR(poLevel);
        nRetCode = CCliMessageHandler::GetInstance().DoExpChange(this, -(INT32)(nExp), poLevel->m_nMaxExp, m_nExpToday, poLevel->m_nDailyExpLimit, 0);
        LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT
    }

Exit0:
    return;
}

void CRole::AddExtraExp(INT32 nExp)
{
    INT32 nRetCode = 0;

    m_nExtraExp += nExp;

#ifndef _ROBOT
    if (!m_bIsLoading)
    {
        nRetCode = CCliMessageHandler::GetInstance().DoExtraExpChange(this);
        LOG_PROCESS_ERROR(nRetCode);
    }
#endif	// _ROBOT

Exit0:
    return;
}

void CRole::AddActivity(INT32 nActivity)
{
    INT32 nRetCode = 0;

    m_nActivity += nActivity;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoActivityChange(this);
    LOG_PROCESS_ERROR(nRetCode);
#endif  // _ROBOT

    return;
Exit0:
    return;
}

void CRole::SetScene(CScene* poScene)
{
    if (poScene)
    {
        m_nSceneTempID = poScene->GetSceneTempID();
        m_nSceneID = poScene->GetSceneID();
    }

    CCharacter::SetScene(poScene);
}

// Pick up item
INT32 CRole::PickUp(UINT32 dwDropID)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    BOOL pick_item = FALSE;

    CDrop* drop = CGameWorld::GetInstance().FindDrop(dwDropID);
    LOG_PROCESS_ERROR_RET_CODE(drop, errDropNotFound);

    LOG_PROCESS_ERROR_RET_CODE(!IsDead(), errItemRoleDead);
    LOG_PROCESS_ERROR_RET_CODE(!IsFreeMode(), errFreeMode);

    if (drop->m_content.m_type == DropContent::DROP_ITEM)
    {
        PROCESS_ERROR_RET_CODE(CGameWorld::GetInstance().GetGameFrame() >
            m_llLastSuccessPickupFrame + g_GWConfig.nLootTimeLimit * GAME_FPS / 1000, errDropFast);

        pick_item = TRUE;
    }

    PROCESS_ERROR_RET_CODE(drop->GetGridInstance() == GetGridInstance(), errDropFaraway);

    LOG_PROCESS_ERROR(drop->GetGridInstance());
    if (drop->GetGridInstance()->uCharRef > 1 ||
        (drop->GetGridInstance()->uCharRef == 1 && m_pGridInstance != drop->GetGridInstance()))
    {
        return errDropOtherStand;
    }

    nResult = drop->OnPickUp(this);

    if (nResult == errSuccess && pick_item)
    {
        m_llLastSuccessPickupFrame = CGameWorld::GetInstance().GetGameFrame();
    }

    if (drop->IsDeleteSelf())
    {
        LOG_PROCESS_ERROR(drop->GetScene());
        nRetCode = drop->GetScene()->RemoveDrop(drop);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = drop->UnInit();
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelDrop(drop);
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return nResult;
}

INT32 CRole::BuyItem(INT32 nShopID, INT32 nShopItemIndex, INT32 nItemCount)
{
    INT32 nResult = errItemUnknown;
    const SHOP_DATA* shop_item = NULL;

#ifndef _ROBOT

    LOG_PROCESS_ERROR(nShopID);
    LOG_PROCESS_ERROR(nItemCount > 0);

    LOG_PROCESS_ERROR_RET_CODE(!IsDead(), errItemRoleDead);

    if (nShopID == 1)
    {
        //vip shop
        LOG_PROCESS_ERROR_RET_CODE(GetVIPLevel() >= vlLevel1, errVipShopNotFound);
    }
    else
    {
        //normal npc shop
        CNpc* npc = CGameWorld::GetInstance().FindNpc(m_dwInteractNpcID);
        PROCESS_ERROR_RET_CODE(npc, errNpcNotFound);

        nResult = CanInteract(npc);
        PROCESS_ERROR(nResult == errSuccess);

        LOG_PROCESS_ERROR_RET_CODE(nShopID == npc->GetNpcData()->nShopID, errShopNotFound);
    }

    shop_item = CNpcShopMgr::GetInstance().FindShopData(nShopID, nShopItemIndex);
    LOG_PROCESS_ERROR_RET_CODE(shop_item, errShopItemNotFound);

    if ((shop_item->m_pItemTemplate->m_byDurType == DURTYPE_STACK && nItemCount * shop_item->m_nItemCount > shop_item->m_pItemTemplate->m_nDur) ||
        (shop_item->m_pItemTemplate->m_byDurType != DURTYPE_STACK && nItemCount > 1))
    {
        return errShopTooMany;
    }

    // check money
    if (GetItemBag()->GetMoney(shop_item->m_nCostItemID1) < shop_item->m_nCostValue1 * nItemCount)
    {
        if (shop_item->m_nCostItemID1 == MONEYTYPE_01)
            return errItemNotEnoughMoneytype01;
        else if (shop_item->m_nCostItemID1 == MONEYTYPE_01)
            return errItemNotEnoughMoneytype02;
        else
            return errNotEnoughMoney;
    }
    if (shop_item->m_nCostItemID2 != MONEYTYPE_INVALID)
    {
        if (GetItemBag()->GetMoney(shop_item->m_nCostItemID2) < shop_item->m_nCostValue2 * nItemCount)
        {
            if (shop_item->m_nCostItemID1 == MONEYTYPE_01)
                return errItemNotEnoughMoneytype01;
            else if (shop_item->m_nCostItemID1 == MONEYTYPE_01)
                return errItemNotEnoughMoneytype02;
            else
                return errNotEnoughMoney;
        }
    }

    // check reputation
    if (shop_item->m_nRequireForceID != INVALID_FORCE_ID)
    {
        LOG_PROCESS_ERROR_RET_CODE(GetReputation().GetValue(shop_item->m_nRequireForceID) >= shop_item->m_nReputation, errItemNotEnoughReputation);
    }

    // check guild building
    if (shop_item->m_nRequireGuildBuildingID)
    {
        CGSGuild* guild = NULL;
        LOG_PROCESS_ERROR_RET_CODE(GetGuildID() == GetSceneID(), errItemNotEnoughBuildingLevel);

        guild = CGuildMgr::GetInstance().FindGuild(GetGuildID());
        LOG_PROCESS_ERROR_RET_CODE(guild, errItemNotEnoughBuildingLevel);
        LOG_PROCESS_ERROR_RET_CODE(guild->GetBuildingMaxLevelByType(shop_item->m_nRequireGuildBuildingID) >= shop_item->m_nBuildingLevel, errItemNotEnoughBuildingLevel);
    }

    nResult = GetItemBag()->AddItem(shop_item->m_pItemTemplate->m_nTemplateID,
        shop_item->m_pItemTemplate->m_byDurType == DURTYPE_STACK ? nItemCount * shop_item->m_nItemCount : 1, ITEMLOG_SHOP, 0, FALSE);
    if (nResult != errSuccess)
        return nResult;

    // sub money
    GetItemBag()->SubMoney(shop_item->m_nCostItemID1, shop_item->m_nCostValue1 * nItemCount, MONEYLOG_BUY_ITEM, 0);
    if (shop_item->m_nCostItemID2 != MONEYTYPE_INVALID)
        GetItemBag()->SubMoney(shop_item->m_nCostItemID2, shop_item->m_nCostValue2 * nItemCount, MONEYLOG_BUY_ITEM, 0);


    return errSuccess;
#endif //_ROBOT
Exit0:
    return nResult;
}

INT32 CRole::SellItem(UINT8 byConSlot, UINT8 byItemSlot, INT16 shItemCount)
{
    INT32 nResult = errItemUnknown;
    INT32 nShopTypeSell = -1;

    if (GetVIPLevel() < vlLevel1)
    {
        CNpc* npc = CGameWorld::GetInstance().FindNpc(m_dwInteractNpcID);
        PROCESS_ERROR_RET_CODE(npc, errNpcNotFound);

        nResult = CanInteract(npc);
        PROCESS_ERROR(nResult == errSuccess);

        PROCESS_ERROR_RET_CODE(npc->GetNpcData()->nShopID || npc->GetNpcData()->nBlackMarketID, errNpcNoSuchFunction);

        if (npc->GetNpcData()->nBlackMarketID == 0)
        {
            const SHOP_DATA* data = CNpcShopMgr::GetInstance().FindShopData(npc->GetNpcData()->nShopID, 1);
            LOG_PROCESS_ERROR(data);
            nShopTypeSell = data->m_nShopTypeSell;
        }
    }

    return GetItemBag()->SellItem(byConSlot, byItemSlot, nShopTypeSell);

Exit0:
    return nResult;
}

INT32 CRole::ViewSoldItems()
{
    INT32 nResult = errItemUnknown;

    if (GetVIPLevel() < vlLevel1)
    {
        CNpc* npc = CGameWorld::GetInstance().FindNpc(m_dwInteractNpcID);
        PROCESS_ERROR_RET_CODE(npc, errNpcNotFound);

        nResult = CanInteract(npc);
        PROCESS_ERROR(nResult == errSuccess);

        PROCESS_ERROR_RET_CODE(npc->GetNpcData()->nShopID || npc->GetNpcData()->nBlackMarketID, errNpcNoSuchFunction);
    }

    return GetItemBag()->ViewSoldItems();

Exit0:
    return nResult;
}

INT32 CRole::BuyBackItem(UINT8 bySlot)
{
    INT32 nResult = errItemUnknown;

    if (GetVIPLevel() < vlLevel1)
    {
        CNpc* npc = CGameWorld::GetInstance().FindNpc(m_dwInteractNpcID);
        PROCESS_ERROR_RET_CODE(npc, errNpcNotFound);

        nResult = CanInteract(npc);
        PROCESS_ERROR(nResult == errSuccess);

        PROCESS_ERROR_RET_CODE(npc->GetNpcData()->nShopID || npc->GetNpcData()->nBlackMarketID, errNpcNoSuchFunction);
    }

    return GetItemBag()->BuyBackItem(bySlot);

Exit0:
    return nResult;
}

INT32 CRole::RepairItem(UINT8 byConSlot, UINT8 byItemSlot, BOOL bSpecial)
{
    INT32 nResult = errItemUnknown;

    if (bSpecial)
    {
        CNpc* npc = CGameWorld::GetInstance().FindNpc(m_dwInteractNpcID);
        PROCESS_ERROR_RET_CODE(npc, errNpcNotFound);

        nResult = CanInteract(npc);
        PROCESS_ERROR(nResult == errSuccess);

        LOG_PROCESS_ERROR_RET_CODE(npc->GetNpcData()->bInteractSpecialRepair, errNpcNoSuchFunction);
    }

    if (!bSpecial && GetVIPLevel() < vlLevel1)
    {
        CNpc* npc = CGameWorld::GetInstance().FindNpc(m_dwInteractNpcID);
        PROCESS_ERROR_RET_CODE(npc, errNpcNotFound);

        nResult = CanInteract(npc);
        PROCESS_ERROR(nResult == errSuccess);

        PROCESS_ERROR_RET_CODE(npc->GetNpcData()->nShopID || npc->GetNpcData()->nBlackMarketID, errNpcNoSuchFunction);
    }

    return GetItemBag()->RepairItem(byConSlot, byItemSlot, bSpecial);

Exit0:
    return nResult;
}

INT32 CRole::RepairAllEquippedItem()
{
    INT32 nResult = errItemUnknown;

    if (GetVIPLevel() < vlLevel1)
    {
        CNpc* npc = CGameWorld::GetInstance().FindNpc(m_dwInteractNpcID);
        PROCESS_ERROR_RET_CODE(npc, errNpcNotFound);

        nResult = CanInteract(npc);
        PROCESS_ERROR(nResult == errSuccess);

        PROCESS_ERROR_RET_CODE(npc->GetNpcData()->nShopID || npc->GetNpcData()->nBlackMarketID, errNpcNoSuchFunction);
    }

    return GetItemBag()->RepairAllEquippedItem();

Exit0:
    return nResult;
}

void CRole::OpenStall()
{
    LOG_CHECK_ERROR(GetMoveState() == CMS_STAND);
    m_eMoveState = CMS_STALL;
}

void CRole::CloseStall()
{
    LOG_CHECK_ERROR(GetMoveState() == CMS_STALL);
    m_eMoveState = CMS_STAND;
}

INT32 CRole::ExchangeAddOrder(INT32 nItemIDRequire, INT32 nItemCountRequire, UINT8 byOrderType, INT32 nPriceOrder, INT32 nCountOrder)
{
    return m_ItemBag.ExchangeAddOrder(nItemIDRequire, nItemCountRequire, byOrderType, nPriceOrder, nCountOrder);
}

BOOL CRole::_ComputeAttribute(INT32 nNewLevel)
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

BOOL CRole::DoSyncAttrChange(INT32 eAttrType)
{
    INT32 nRetCode = 0;

    PROCESS_ERROR(!IsLoading());

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoAttrChange(this, eAttrType);
    LOG_CHECK_ERROR(nRetCode);
#endif
    return TRUE;
Exit0:
    return FALSE;
}

void CRole::SaveReturnPos(void)
{
    const SCENE_DATA* pcSceneData = NULL;

    LOG_PROCESS_ERROR(m_poScene);

    pcSceneData = m_poScene->GetSceneData();
    LOG_PROCESS_ERROR(pcSceneData);

    if (pcSceneData->nType == estStatic || pcSceneData->nType == estLine ||
        pcSceneData->nType == estBirth || pcSceneData->nType == estUnique)
    {
        m_StaticReturnPos.nSceneTempID = m_poScene->GetSceneTempID();
        m_StaticReturnPos.nSceneID = m_poScene->GetSceneID();
        m_StaticReturnPos.nX = (INT32)m_llX;
        m_StaticReturnPos.nY = (INT32)m_llY;
        m_StaticReturnPos.nZ = (INT32)m_llZ;
        m_StaticReturnPos.nFaceDirection = m_nFaceDirection;
    }

Exit0:
    return;
}

const ABSOLUTE_SCENE_POS& CRole::GetStaticReturnPos(void)
{
    return m_StaticReturnPos;
}

const ABSOLUTE_SCENE_POS& CRole::GetRevivePos(void)
{
    return m_RevivePos;
}

BOOL CRole::JumpToReturnPos(void)
{
    INT32 nRetCode = 0;
    const SCENE_DATA* poSceneData = NULL;
    LOG_PROCESS_ERROR(m_poScene);

    poSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(m_poScene->GetSceneTempID());
    LOG_PROCESS_ERROR(poSceneData);
    PROCESS_ERROR(poSceneData->nType == estDungeon || poSceneData->nType == estGuildDungeon
        || poSceneData->nType == estArena || poSceneData->nType == estBattleField
        || poSceneData->nType == estGuild || poSceneData->nType == estSoloDungeon);

    nRetCode = CGameWorld::GetInstance().JumpTo(this, m_StaticReturnPos.nSceneTempID, m_StaticReturnPos.nSceneID,
        m_StaticReturnPos.nX, m_StaticReturnPos.nY, m_StaticReturnPos.nZ,
        m_StaticReturnPos.nFaceDirection);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::JumpToRevivePos(void)
{
    INT32 nRetCode = 0;
    const SCENE_DATA* poSceneData = NULL;
    LOG_PROCESS_ERROR(m_poScene);

    nRetCode = CGameWorld::GetInstance().JumpTo(this, m_RevivePos.nSceneTempID, m_RevivePos.nSceneID,
        m_RevivePos.nX, m_RevivePos.nY, m_RevivePos.nZ,
        m_RevivePos.nFaceDirection);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::AddCooldown(UINT32 dwCooldownID, INT32& nCooldownFrm)
{
    if (m_CoolDown.IsDisabled())
        return TRUE;

    if (!CCharacter::AddCooldown(dwCooldownID, nCooldownFrm))
        return FALSE;

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoAddCooldown(this, dwCooldownID, nCooldownFrm);
#endif
    return TRUE;
}

BOOL CRole::ChangeCooldown(UINT32 dwCooldownID, INT32& nCooldownFrm)
{
    if (m_CoolDown.IsDisabled())
        return TRUE;

    if (!CCharacter::ChangeCooldown(dwCooldownID, nCooldownFrm))
        return FALSE;

    INT32 nRemainFrm = m_CoolDown.GetRemainFrm(dwCooldownID);

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoAddCooldown(this, dwCooldownID, nRemainFrm + nCooldownFrm);
#endif
    return TRUE;
}

BOOL CRole::ResetCooldown(UINT32 dwCooldownID)
{
    if (m_CoolDown.IsDisabled())
        return TRUE;

    INT32 nRemainFrm = m_CoolDown.GetRemainFrm(dwCooldownID);

    if (!CCharacter::ResetCooldown(dwCooldownID))
        return FALSE;

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoAddCooldown(this, dwCooldownID, -nRemainFrm);
#endif
    return TRUE;

}

void CRole::ClearAllCooldown(void)
{
    m_CoolDown.Begin();

    INT32 nKey = 0;
    UINT32 dwTimeupFrm = 0;

    while (m_CoolDown.Next(nKey, dwTimeupFrm))
    {
#ifndef _ROBOT
        CCliMessageHandler::GetInstance().DoAddCooldown(this, nKey, 0);
#endif
    }

    m_CoolDown.ResetAll();
}

void CRole::SetFaction(INT32 nFaction)
{
    INT32 nRetCode = 0;

    m_nFaction = nFaction;

    if (m_poHero != NULL)
    {
        m_poHero->SetFaction(nFaction);
    }

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncFaction(this);
    LOG_CHECK_ERROR(nRetCode);
#endif
}

BOOL CRole::SetLevel(INT32 nNewLevel)
{
    CLuaScript* poScript = nullptr;
    INT32 nRetCode = 0;
    INT32 nOldLevel = m_nLevel;

    if (m_nLevel == nNewLevel)
        return TRUE;

    {
        const ROLE_LEVEL_DATA* pData = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(nNewLevel);
        LOG_PROCESS_ERROR(pData);
    }

    nRetCode = CCharacter::SetLevel(nNewLevel);  // Update attributes
    LOG_PROCESS_ERROR(nRetCode);

    poScript = CScriptMgr::GetInstance().FindScript(ROLE_LEVEL_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    SetHp(GetAttr(catMaxHp));
    ChangeMp(GetAttr(catMaxMp));

#ifndef _ROBOT
    nRetCode = poScript->CallFunction("main", "odd", this, "CRole", nOldLevel, nNewLevel);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    GetItemBag()->UpdateMoneyLimits();
    GetItemBag()->CheckWeight();

    if (m_eState == rsPlaying)
    {
#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoRoleLevelUp(this);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CCliMessageHandler::GetInstance().DoSyncMoney(this);
        LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT
    }

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSUpdateRoleInfo(this);
    LOG_CHECK_ERROR(nRetCode);

    if (m_nLevel <= g_GWConfig.nMasterFindTeacherLevelLimit && GetMaster().IsStudent() == FALSE
        && GetMaster().IsTeacher() == FALSE && GetMaster().GetTwitter() == TRUE)
    {
        nRetCode = CCliMessageHandler::GetInstance().DoMSTryToFindTeacher(GetObjID());
        LOG_CHECK_ERROR(nRetCode);
    }

#endif  // _ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::ChangePkMode(INT32 nMode)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(nMode > rpmInvalid && nMode < rpmTotal);
    PROCESS_SUCCESS(m_nPkMode == nMode);

#ifndef _ROBOT
    LOG_PROCESS_ERROR(m_nLevel > g_GWConfig.nPkProtectLevel);
#endif  // _ROBOT

    m_nPkMode = nMode;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoChangePkMode(this);
    LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::IsGreyName(void)
{
    return m_nGreyNameFrame != INT_MAX && m_nGreyNameFrame >= CGameWorld::GetInstance().GetGameFrame();
}

void CRole::SetGreyName(void)
{
    INT32 nRetCode = 0;

    if (m_nGreyNameFrame == INT_MAX)
    {
#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoSyncGreyName(this, TRUE);
        LOG_CHECK_ERROR(nRetCode);
#endif // _ROBOT
    }

    m_nGreyNameFrame = CGameWorld::GetInstance().GetGameFrame() + (g_GWConfig.nGreyNameDuration * GAME_FPS / 1000);
}

void CRole::OnHurt(CCharacter* poDealer, CBullet* poBullet, INT32 nEffectValue)
{
    CCharacter::OnHurt(poDealer, poBullet, nEffectValue);

    //	DBG("role hurt pos: (%d, %d)", m_llX / GRID_WIDTH, m_llY / GRID_WIDTH);

    m_nHpTap = 0;
    m_nMpTap = 0;

    if (poBullet)
    {
        INT32 nDealerDir = g_GetDirection(m_llX, m_llY, poDealer->GetX(), poDealer->GetY());

        if (poBullet->GetStiffFrm() > 0 && g_Cos(GetFaceDirection() - nDealerDir) >= 0)
        {
            if (g_Random(0, RATE_MAX) < poBullet->GetSpellAttr(satKnockBackRate) && g_Random(0, RATE_MAX) > GetAttr(catRepelResist))
                m_oStiffFlag.SetFlag(cstfKnockBack);
        }
    }

    if (nEffectValue > 0)
        m_ItemBag.LoseArmorDur(nEffectValue);

    CStatistic::GetInstance().OnHurt(this, poDealer, nEffectValue);
}

BOOL CRole::Die(INT32 nParam)
{
    INT32 nRetCode = 0;
    const SCENE_DATA* pcSceneData = NULL;
    CSceneStaticData* poSceneStaticData = NULL;
    BOOL bFreePK = FALSE;
    CCharacter* poKiller = NULL;

    LOG_PROCESS_ERROR(m_poScene);
    LOG_PROCESS_ERROR(m_eMoveState != CMS_DIE);

    pcSceneData = m_poScene->GetSceneData();
    LOG_PROCESS_ERROR(pcSceneData);

    poSceneStaticData = m_poScene->GetStaticData();
    LOG_PROCESS_ERROR(poSceneStaticData);

#ifndef _ROBOT
    if (m_eMoveState == CMS_STALL)
    {
        nRetCode = m_Stall.RealClose();
        LOG_CHECK_ERROR(nRetCode == errSuccess);
    }
#endif //_ROBOT

    nRetCode = CCharacter::Die(nParam);
    PROCESS_ERROR(nRetCode);


    ClearPet();

#ifndef _ROBOT
    // victim
    m_nComboKill = 0;

    if (m_poHero != NULL)
        RecallHero(TRUE);

    IncCombatStat(escsDeathCount, 1);

    m_ItemBag.DestroyItemOnDeath();
    if (m_pcGrid && m_pcGrid->uPkType == gptFree)
        bFreePK = TRUE;

    if (!pcSceneData->bCannotLoseDurability)
        m_ItemBag.LoseAllDur(5, ITEMLOG_DEATH);

    if (GetTypeFromID(m_dwLastCasterID) == otRole)
    {
        m_nNextResetKilledTimesByPlayerFrm = CGameWorld::GetInstance().GetGameFrame() + CONVERT_TIME(g_GWConfig.nReviveTimesResetTime);
        m_nNextFreeReviveFrm = CGameWorld::GetInstance().GetGameFrame() +
            CONVERT_TIME(g_Min(g_GWConfig.nFreeReviveParam1, g_GWConfig.nFreeReviveParam2 * m_nKilledByPlayer));
        m_nKilledByPlayer++;
    }

    nRetCode = CCliMessageHandler::GetInstance().DoReviveInfo(this);
    LOG_CHECK_ERROR(nRetCode);

    if (poSceneStaticData->GetBT(evtRoleDie))
    {
        nRetCode = m_poScene->RunBT(evtRoleDie, GetObjID(), poSceneStaticData->GetBT(evtRoleDie));
        LOG_CHECK_ERROR(nRetCode);
    }

    GetAchievementList().AddVariable(avdSelfDieCount, 1);

    // killer

    poKiller = CGameWorld::GetInstance().FindValidCharacter(m_dwLastCasterID);
    if (poKiller && poKiller->GetHostID() != 0)
        poKiller = CGameWorld::GetInstance().FindValidCharacter(poKiller->GetHostID());

    if (poKiller)
    {
        // Killed by another player
        if (poKiller->GetObjType() == otRole)
        {
            CRole* poKillerRole = (CRole*)poKiller;
            if (poKillerRole != this && !bFreePK)
            {
                if (IsRedName())
                {
                    CStatistic::GetInstance().OnKillWithNoGuilty(poKillerRole);

                    nRetCode = CCliMessageHandler::GetInstance().DoSyncPkResult(poKillerRole, this, 0, rprRedName);
                    LOG_CHECK_ERROR(nRetCode);
                }
                else if (IsGreyName())
                {
                    CStatistic::GetInstance().OnKillWithNoGuilty(poKillerRole);

                    nRetCode = CCliMessageHandler::GetInstance().DoSyncPkResult(poKillerRole, this, 0, rprCounterStrike);
                    LOG_CHECK_ERROR(nRetCode);
                }
                else if (GetGuildID() != INVALID_OBJECT_ID && poKillerRole->GetGuildID() != INVALID_OBJECT_ID &&
                    CGuildMgr::GetInstance().IsHostile(GetGuildID(), poKillerRole->GetGuildID()))
                {
                    CStatistic::GetInstance().OnKillWithNoGuilty(poKillerRole);

                    nRetCode = CCliMessageHandler::GetInstance().DoSyncPkResult(poKillerRole, this, 0, rprOpponentGuildMember);
                    LOG_CHECK_ERROR(nRetCode);
                }
                else if (poKillerRole->GetLevel() - m_nLevel >= 10 || g_Random(RATE_MAX) < g_GWConfig.nGainPkValueChance)
                {
                    INT32 nPkValue = poKillerRole->GetPkValue();

                    nPkValue += g_GWConfig.nGainPkValue;
                    if (nPkValue > MAX_PK_VALUE)
                        nPkValue = MAX_PK_VALUE;

                    nRetCode = CCliMessageHandler::GetInstance().DoSyncPkResult(poKillerRole, this, nPkValue - poKillerRole->GetPkValue(), rprNormal);
                    LOG_CHECK_ERROR(nRetCode);

                    poKillerRole->SetPkValue(nPkValue);

                    if (GetPkValue() < 100)
                    {
                        //poKillerRole->GetItemBag()->DecreaseWeaponLuck();  // Temporarily disabled; player kills no longer reduce weapon durability
                    }
                }
                else
                {
                    CStatistic::GetInstance().OnKillWithNoGuilty(poKillerRole);

                    nRetCode = CCliMessageHandler::GetInstance().DoSyncPkResult(poKillerRole, this, 0, rprLuckyKill);
                    LOG_CHECK_ERROR(nRetCode);
                }

                if (!GetScene()->GetFirstBloodFlag())
                {
                    GetScene()->SetFirstBloodFlag(TRUE);
                    poKillerRole->GetAchievementList().AddVariable(avdFirstBloodCount, 1);
                }
            }
            else
            {
                CStatistic::GetInstance().OnKillWithNoGuilty(poKillerRole);

                nRetCode = CCliMessageHandler::GetInstance().DoSyncPkResult(poKillerRole, this, 0, rprNoPunishment);
                LOG_CHECK_ERROR(nRetCode);
            }

            if (!m_pcGrid->bRoleCannotLoseItem)
                GetItemBag()->LoseItemOnDeath(TRUE);

            nRetCode = poKillerRole->OnKillPlayer(this);
            LOG_CHECK_ERROR(nRetCode);

            poKillerRole->IncComboKill();
            poKillerRole->IncCombatStat(escsRoleKillCount, 1);

            if ((pcSceneData->nType == estStatic) || (pcSceneData->nType == estLine))
            {
                nRetCode = CCliMessageHandler::GetInstance().DoSyncMsgPaoPao(this, MSG_PP_PERSONAL_ENEMY, poKillerRole->GetObjID());
                LOG_CHECK_ERROR(nRetCode);
            }

            nRetCode = CCliMessageHandler::GetInstance().DoDLDie(this,
                m_poScene->GetSceneTempID(), m_poScene->GetSceneID(), poKillerRole->GetObjID(), poKillerRole->GetName(),
                poKillerRole->GetLevel(), poKillerRole->GetClass(), GetX() / POINT_PER_CM, GetY() / POINT_PER_CM, GetZ() / POINT_PER_CM);
            LOG_CHECK_ERROR(nRetCode);
        }
        // Killed by a monster
        else if (poKiller->GetObjType() == otNpc)
        {
            CNpc* poKillerNpc = (CNpc*)poKiller;

            CStatistic::GetInstance().OnKilledByNpc(this, poKillerNpc);

            nRetCode = CCliMessageHandler::GetInstance().DoDLDie(this,
                m_poScene->GetSceneTempID(), m_poScene->GetSceneID(), poKillerNpc->GetObjID(), poKillerNpc->GetName(),
                poKillerNpc->GetLevel(), poKillerNpc->GetClass(), GetX() / POINT_PER_CM, GetY() / POINT_PER_CM, GetZ() / POINT_PER_CM);
            LOG_CHECK_ERROR(nRetCode);

            if (!m_pcGrid->bRoleCannotLoseItem)
                GetItemBag()->LoseItemOnDeath(FALSE);
        }
        else
        {
            CRI("error killer object type %d", poKiller->GetObjType());
        }
    }
#endif

    return TRUE;
Exit0:
    return FALSE;
}

CSpellSlot* CRole::GetSpellSlot(INT32 nSlotID)
{
    LOG_PROCESS_ERROR(m_poCurrentSlot);
    PROCESS_ERROR(nSlotID >= qssSlotBegin && nSlotID < qssSlotCount);

    return &m_poCurrentSlot[nSlotID];
Exit0:
    return NULL;
}

INT32 CRole::EquipSpell(INT32 nSlotID, INT32 nSpellID)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    INT32 nRuneSlot = 0;
    INT32 nSpellLevel = 0;
    CSpellSet* poSet = NULL;
    CSpell* poSpell = NULL;
    INT32 nPrevSpellID = 0;

    LOG_PROCESS_ERROR_RET_CODE(nSlotID >= qssSlotBegin && nSlotID < qssSlotCount, errUnknownError);
    LOG_PROCESS_ERROR_RET_CODE(!m_SpellSlot[nSlotID].IsLocked(), errSpellSlotLocked);
    PROCESS_SUCCESS(m_SpellSlot[nSlotID].GetMainSpellID() == nSpellID);

    if (nSpellID != 0)
    {
        INT32 nQteType = 0;

        //		nRetCode = m_CoolDown.CheckReady(MakeCoolDownID(COOLDOWN_SPELL, nSpellID));
        //		PROCESS_ERROR_RET_CODE(nRetCode, errSpellEquipNotReady);

        poSet = m_SpellBook.GetSpellSet(nSpellID);
        LOG_PROCESS_ERROR_RET_CODE(poSet != NULL, errSpellNotExists);

        nRuneSlot = poSet->GetRuneSlot();
        nSpellLevel = poSet->GetLevel();

        //		LOG_PROCESS_ERROR_RET_CODE(poSet->IsEnabled(), errSpellEquipDisabled);

        poSpell = poSet->GetSpell();
        LOG_PROCESS_ERROR_RET_CODE(poSpell != NULL, errUnknownError);
        LOG_PROCESS_ERROR_RET_CODE(poSpell->GetAttr(satCanEquip), errSpellCannotBeEquipped);

        LOG_PROCESS_ERROR_RET_CODE((1 << nSlotID) & poSpell->GetAttr(satBindSlot), errSpellEquipBindSlot);

        if (poSpell->GetAttr(satCastType) == spctPassive)
            PROCESS_ERROR_RET_CODE(nSlotID >= qssPassiveBegin && nSlotID < qssPassiveEnd, errSpellEquipPassiveSlotError);

        //		for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
        //		{
        //			nRetCode = m_SpellSlot[i].GetMainSpellID() != nSpellID;
        //			LOG_PROCESS_ERROR_RET_CODE(nRetCode, errSpellSlotNoDuplicate);
        //		}

        poSet->SetEquipSlot(nSlotID);
    }

    nPrevSpellID = m_SpellSlot[nSlotID].GetMainSpellID();

    if (nPrevSpellID != 0)
    {
        CSpell* poPrevSpell = NULL;
        CSpellSet* poPrevSet = m_SpellBook.GetSpellSet(nPrevSpellID);
        LOG_PROCESS_ERROR_RET_CODE(poPrevSet != NULL, errUnknownError);

        poPrevSpell = poPrevSet->GetSpell();
        LOG_PROCESS_ERROR_RET_CODE(poPrevSpell != NULL, errUnknownError);

        poPrevSet->SetEquipSlot(qssSlotInvalid);

        PushEquippingSpell(poPrevSpell);

        nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(poPrevSpell->GetAttr(satOnEquip), this, poPrevSpell->GetExParam());
        LOG_CHECK_ERROR(nRetCode >= 0);

        nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(poPrevSpell->GetAttr(satOnUnEquip), this, poPrevSpell->GetExParam());
        LOG_CHECK_ERROR(nRetCode >= 0);
        PopEquippingSpell();
    }

    m_SpellSlot[nSlotID].Equip(this, nSpellID);

    if (poSpell != NULL)
    {
        PushEquippingSpell(poSpell);

        nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(poSpell->GetAttr(satOnEquip), this, poSpell->GetExParam());
        LOG_CHECK_ERROR(nRetCode >= 0);
        PopEquippingSpell();
    }


    SPL("role[%s] equip spell[%d] to slot[%d]", GetName(), nSpellID, nSlotID);

#ifndef _ROBOT
    if (!IsLoading())
        CCliMessageHandler::GetInstance().DoEquipSpell(this, nSlotID, nSpellID, nRuneSlot, nSpellLevel);
#endif

Exit1:
    return errSuccess;
Exit0:

#ifndef _ROBOT
    this->ErrorCodeNotify(nResult, 0, 0);
#endif
    return nResult;
}

BOOL CRole::EquipRune(INT32 nSpellID, INT32 nRuneSlot)
{
    INT32 nRetCode = 0;
    CSpellSet* poSet = NULL;

    poSet = CreateSpellSet(nSpellID);
    LOG_PROCESS_ERROR(poSet);

    {
        INT32 nEquipSlot = poSet->GetEquipSlot();
        CSpellSlot* poSlot = GetSpellSlot(nEquipSlot);

        if (poSlot)
            EquipSpell(nEquipSlot, 0);

        poSet->SetRuneSlot(nRuneSlot);

        if (poSlot)
            EquipSpell(nEquipSlot, nSpellID);
    }

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoEquipRune(this, nSpellID, nRuneSlot, poSet->GetActualLevel());
#endif

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CRole::EquipRune(const RUNE_DATA* pRuneData, Item* poItem)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    INT32 nEquipSlot = 0;
    CSpellSet* poSet = NULL;
    CSpellTemplate* t = NULL;
    CSpellSlot* poSlot = NULL;

    LOG_PROCESS_ERROR_RET_CODE(pRuneData, errRuneNotExists);
    LOG_PROCESS_ERROR_RET_CODE(poItem, errUnknownError);

    t = CSpellConfig::GetInstance().FindSpellTemplate(pRuneData->nSpellID, 0, pRuneData->nRuneSlot);
    LOG_PROCESS_ERROR_RET_CODE(t, errSpellNotExists);

    PROCESS_ERROR_RET_CODE((t->GetAttr(satClass) & (1 << (m_nClass))) != 0, errRuneEquipClassError);

    poSet = CreateSpellSet(pRuneData->nSpellID);
    LOG_PROCESS_ERROR_RET_CODE(poSet, errSpellNotExists);

    nEquipSlot = poSet->GetEquipSlot();

    if (nEquipSlot != qssSlotInvalid)
        poSlot = GetSpellSlot(nEquipSlot);

    if (poSlot)
        EquipSpell(nEquipSlot, 0);

    nRetCode = poSet->AddItemSpellRuneNode(poItem->m_iteminfo.m_llGuid, pRuneData->nRuneID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errRuneEquipError);

    if (poSlot)
        EquipSpell(nEquipSlot, pRuneData->nSpellID);

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoEquipRune(this, pRuneData->nSpellID, pRuneData->nRuneSlot, poSet->GetLevel());
#endif

    return errSuccess;
Exit0:
    return nResult;
}

BOOL CRole::UnEquipRune(const RUNE_DATA* pRuneData, Item* poItem)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    INT32 nEquipSlot = 0;
    CSpellSet* poSet = NULL;
    CSpellSlot* poSlot = NULL;

    LOG_PROCESS_ERROR(pRuneData);

    poSet = m_SpellBook.GetSpellSet(pRuneData->nSpellID);
    LOG_PROCESS_ERROR(poSet);

    nEquipSlot = poSet->GetEquipSlot();
    poSlot = GetSpellSlot(nEquipSlot);

    if (poSlot)
        EquipSpell(nEquipSlot, 0);

    nRetCode = poSet->RemoveItemSpellRuneNode(poItem->m_iteminfo.m_llGuid);
    LOG_PROCESS_ERROR(nRetCode);


    if (poSlot)
        EquipSpell(nEquipSlot, pRuneData->nSpellID);

#ifndef _ROBOT
    if (!IsLoading())
        CCliMessageHandler::GetInstance().DoEquipRune(this, pRuneData->nSpellID, 0, poSet->GetLevel());
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::UnlockSpellSlot(INT32 nSlotID)
{
    INT32 nRetCode = 0;
    LOG_PROCESS_ERROR(nSlotID >= qssSlotBegin && nSlotID < qssSlotCount);

    m_SpellSlot[nSlotID].Lock(FALSE);

    if (!IsLoading())
    {
#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoUnlockSpellSlot(this, nSlotID);
        LOG_CHECK_ERROR(nRetCode);
#endif
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::AddSpellPts(INT32 nSpellID, INT32 nPts)
{
    INT32 nRetCode = 0;
    INT32 nLevel = 0;
    INT32 nRuneSlot = 0;

    SPL("add spell pts: %d, rolename: %s", nSpellID, m_szName);

    CSpellSet* poSet = GetSpellBook().GetSpellSet(nSpellID);
    LOG_PROCESS_ERROR(poSet);

    nLevel = poSet->GetLevel();
    nRuneSlot = poSet->GetRuneSlot();

    PROCESS_SUCCESS(nLevel >= MAX_SPELL_LEVEL);

    nPts = nPts * (RATE_MAX + GetAttr(catSpellPtsIncr)) / RATE_MAX;

    {
        const SPELL_PROGRESS_DATA* pData = CDataManager<SPELL_PROGRESS_DATA>::GetInstance().GetData(MakeSpellProgressKey(poSet->GetID(), nLevel));
        PROCESS_ERROR(pData);

        PROCESS_ERROR(m_nLevel >= pData->m_nRequireLevel);

        poSet->AddPts(nPts);

        if (poSet->GetPts() >= pData->m_nConsumePts)
        {
            CSpellTemplate* e1 = NULL;
            CSpellTemplate* e2 = NULL;

            PROCESS_ERROR(m_nLevel >= pData->m_nRequireLevel);

            e1 = CSpellConfig::GetInstance().FindSpellTemplate(poSet->GetID(), nLevel, nRuneSlot);
            LOG_PROCESS_ERROR(e1);

            e2 = CSpellConfig::GetInstance().FindSpellTemplate(poSet->GetID(), nLevel + 1, nRuneSlot);
            LOG_PROCESS_ERROR(e2);

            LOG_PROCESS_ERROR(e1->GetAttr(satCastType) == e2->GetAttr(satCastType));

            PushEquippingSpell(e1);
            nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(e1->GetAttr(satOnLearn), this, e1->GetExParam());
            LOG_CHECK_ERROR(nRetCode >= 0);
            nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(e1->GetAttr(satOnForget), this, e1->GetExParam());
            LOG_CHECK_ERROR(nRetCode >= 0);

            if (poSet->GetEquipSlot() > qssSlotInvalid && poSet->GetEquipSlot() < qssSlotCount)
            {
                nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(e1->GetAttr(satOnEquip), this, e1->GetExParam());
                LOG_CHECK_ERROR(nRetCode >= 0);
                nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(e1->GetAttr(satOnUnEquip), this, e1->GetExParam());
                LOG_CHECK_ERROR(nRetCode >= 0);
            }
            PopEquippingSpell();

            poSet->LevelUp();

            PushEquippingSpell(e2);
            nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(e2->GetAttr(satOnLearn), this, e2->GetExParam());
            LOG_CHECK_ERROR(nRetCode >= 0);

            if (poSet->GetEquipSlot() > qssSlotInvalid && poSet->GetEquipSlot() < qssSlotCount)
            {
                nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(e2->GetAttr(satOnEquip), this, e2->GetExParam());
                LOG_CHECK_ERROR(nRetCode >= 0);
            }
            PopEquippingSpell();

            poSet->AddPts(-pData->m_nConsumePts);

#ifndef _ROBOT
            CCliMessageHandler::GetInstance().DoSpellLevelUp(this, nSpellID, poSet->GetLevel());
            CCliMessageHandler::GetInstance().DoDLLearnSpell(this, nSpellID, poSet->GetLevel());
#endif
        }

    }

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoSyncSpellPts(this, nSpellID, poSet->GetPts());
#endif
Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

INT32 CRole::CastSpell(CSpell* poSpell, CCharacter* poTarget, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    INT32 nNextSpellID = 0;

    INT32 nItemCostID = 0;
    INT32 nItemCostCount = 0;

    INT32 nItemCostType = 0;
    INT32 nItemCostSubType = 0;
    Item* poItem = NULL;
    INT32 nWeaponTypeR = m_ItemBag.GetWeaponType();

    LOG_PROCESS_ERROR_RET_CODE(poSpell != NULL, errSpellNotExists);
    if (poSpell->GetAttr(satSwitchSpell) != 0)
    {
        CSpellSet* poSwitchSet = GetSpellBook().GetSpellSet(poSpell->GetAttr(satSwitchSpell));
        if (poSwitchSet != NULL)
        {
            poSpell = CSpellConfig::GetInstance().FindSpellTemplate(poSpell->GetID(), poSwitchSet->GetLevel(), poSwitchSet->GetRuneSlot());
        }
    }

    LOG_PROCESS_ERROR_RET_CODE(poSpell != NULL, errSpellNotExists);

    nItemCostID = poSpell->GetAttr(satItemCostID);
    nItemCostCount = poSpell->GetAttr(satItemCostCount);

    nItemCostType = poSpell->GetAttr(satItemCostType);
    nItemCostSubType = poSpell->GetAttr(satItemCostSubType);

    LOG_PROCESS_ERROR_RET_CODE((poSpell->GetAttr(satWeaponTypeR) & (1 << nWeaponTypeR)) != 0, errSpellWeaponTypeError);

#ifdef _FINAL
    LOG_PROCESS_ERROR_RET_CODE((poSpell->GetAttr(satClass) & (1 << (m_nClass))) != 0, errSpellErrorType);
#endif

    LOG_PROCESS_ERROR_RET_CODE(poSpell->GetAttr(satCastType) == spctActive || poSpell->GetAttr(satCastType) == spctDirectly, errSpellErrorType);

    if (nItemCostID != 0)
    {
        INT32 nItemCount = 0;

        if (nItemCostID < MONEYTYPE_COUNT)
            nItemCount = m_ItemBag.GetMoney(nItemCostID);
        else
            nItemCount = m_ItemBag.GetItemCount(nItemCostID);

        PROCESS_ERROR_RET_CODE(nItemCostCount <= nItemCount, errSpellLackOfItem);
    }
    else if (nItemCostType > ITEMTYPE_INVALID && nItemCostSubType > ITEMSUBTYPE_INVALID)
    {
        INT32 nSlot = m_ItemBag.SearchDefaultContainer(nItemCostType, nItemCostSubType, nItemCostCount);
        PROCESS_ERROR_RET_CODE(nSlot != INVALID_ITEM_SLOT, errSpellLackOfItem);

        poItem = m_ItemBag.GetItem(CONSLOT_DEFAULT, nSlot);
        LOG_PROCESS_ERROR_RET_CODE(poItem, errSpellLackOfItem);

        PROCESS_ERROR_RET_CODE(poItem->m_iteminfo.m_nCurDur >= nItemCostCount, errSpellLackOfItem);
    }

    ClearSpellCache();

    if (m_eMoveState == CMS_MINE)
        Stand(TRUE);

    if (poTarget)
    {
        //		DBG("cast spell from (%u, %u) to (%u, %u)", m_llX / GRID_WIDTH, m_llY / GRID_WIDTH, poTarget->GetX() / GRID_WIDTH, poTarget->GetY() / GRID_WIDTH);
    }

    nResult = CCharacter::CastSpell(poSpell, poTarget, nTargetX, nTargetY, nTargetZ);
    PROCESS_ERROR(nResult == errSuccess);

    SPL(">>> role request cast spell: %d, serial: %d, rolename: %s", poSpell->GetID(), m_nSpellSerial, m_szName);

    switch (m_poCastingSpell->GetAttr(satAttackType))
    {
        case spatPhy:
            m_nHpTap += g_GWConfig.nHpTapPhyAtk;
            m_nMpTap += g_GWConfig.nMpTapPhyAtk;
            break;
        case spatMag:
        case spatTao:
            m_nHpTap += g_GWConfig.nHpTapMagAtk;
            m_nMpTap += g_GWConfig.nMpTapMagAtk;
            break;
        default:
            break;
    }

    if (m_poCastingSpell != NULL)
        m_dwSpellCacheTimeoutFrm = CGameWorld::GetInstance().GetGameFrame() + m_poCastingSpell->GetCacheTimeFrm();

    if (nItemCostID != 0)
    {
        if (nItemCostID < MONEYTYPE_COUNT)
            nResult = m_ItemBag.SubMoney(nItemCostID, nItemCostCount, MONEYLOG_CAST_SPELL, 0);
        else
            nResult = m_ItemBag.DeleteItemId(nItemCostID, nItemCostCount, ITEMLOG_CAST_SPELL, FALSE, 0);

        LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, errUnknownError);
    }
    else if (nItemCostType > ITEMTYPE_INVALID && nItemCostSubType > ITEMSUBTYPE_INVALID)
    {
        LOG_PROCESS_ERROR_RET_CODE(poItem, errSpellUnknown);

        nRetCode = m_ItemBag.LoseItemDur(poItem->m_pTemplate->m_nTemplateID, nItemCostCount, ITEMLOG_SPELL);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errSpellUnknown);
    }

    //	DBG("role cast, serial: %d", m_nSpellSerial);
Exit0:
    {
        INT32 nRemainCooldownFrm = GetCoolDown().GetRemainFrm(MakeCoolDownID(COOLDOWN_SPELL, poSpell->GetID()));
#ifndef _ROBOT
        //CCliMessageHandler::GetInstance().DoDLCastSpell(this, poSpell->GetID(), poSpell->GetLevel(), poSpell->GetRuneSlot(), poTarget == NULL ? 0 : poTarget->GetObjID(), nTargetX, nTargetY, nTargetZ, m_nCurrentMp, nRemainCooldownFrm * 1000 / GAME_FPS, nResult, 0, TRUE, FALSE);
#endif
    }
    return nResult;
}

BOOL CRole::CastCachedSpell(void)
{
    INT32 nResult = errSpellUnknown;
    CSpellSet* poSet = NULL;
    CSpell* poSpell = NULL;
    INT32 nNextSpellID = 0;
    CCharacter* poTarget = NULL;

    PROCESS_SUCCESS(m_SpellCache.m_nSpellID == 0);

    poSet = m_SpellBook.GetSpellSet(m_SpellCache.m_nSpellID);
    LOG_PROCESS_ERROR(poSet != NULL);

    poSpell = poSet->GetSpell();
    LOG_PROCESS_ERROR(poSpell);

    poTarget = CGameWorld::GetInstance().FindValidCharacter(m_SpellCache.m_dwTargetID);

    nResult = CastSpell(poSpell, poTarget, m_SpellCache.m_llX, m_SpellCache.m_llY, m_SpellCache.m_llZ);
    PROCESS_ERROR(nResult == errSuccess);

    if (m_poCastingSpell != NULL)
        poSet->RecordNextSpell(this, m_poCastingSpell->GetAttr(satNextSpell), m_poCastingSpell->GetResetTimeFrm());

Exit1:
    return TRUE;
Exit0:
    //#ifndef _ROBOT
    //	this->ErrorCodeNotify(nResult, m_SpellCache.m_nSpellID, m_SpellCache.m_nSpellSerial);
    //#endif // !_ROBOT
    return FALSE;

}

BOOL CRole::_OnCastOver(void)
{
    INT32 nRetCode = 0;
    LOG_PROCESS_ERROR(m_poCastingSpell);

#ifndef _ROBOT	
    CCliMessageHandler::GetInstance().DoCastOver(this, m_poCastingSpell->GetID(), m_nSpellSerial);
#endif

    if (m_poCastingSpell->HasHitTarget())
    {
        AddSpellPts(m_poCastingSpell->GetID(), g_Random(0, 3) + 1);
    }

    SPL(">>> role cast over: %d, rolename: %s", m_poCastingSpell->GetID(), m_szName);

    nRetCode = CCharacter::_OnCastOver();
    LOG_PROCESS_ERROR(nRetCode);


#ifndef _ROBOT
    nRetCode = CastCachedSpell();
    LOG_CHECK_ERROR(nRetCode);
#endif
    m_dwSpellCacheTimeoutFrm = 0;

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CRole::GetWeaponType(void)
{
    return m_ItemBag.GetWeaponType();
}

INT32 CRole::ActivateMount(INT32 nMountID)
{
    INT32 nResult = errUnknownError;

    TActivatedMountList::iterator itr = m_ActivedMountList.begin();

    const MOUNT_ENTRY* pInfo = CDataManager<MOUNT_ENTRY>::GetInstance().GetData(nMountID);
    LOG_PROCESS_ERROR(pInfo);


    PROCESS_ERROR_RET_CODE(m_nLevel >= pInfo->m_nLevelLimit, errMountLevelLimited);

    itr = m_ActivedMountList.find(nMountID);
    PROCESS_ERROR_RET_CODE(itr == m_ActivedMountList.end(), errMountAlreadyExists);

    m_ActivedMountList.insert(nMountID);

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoActivateMount(this, nMountID);
#endif
    return errSuccess;
Exit0:
#ifndef _ROBOT
    this->ErrorCodeNotify(nResult, 0, 0);
#endif
    return nResult;
}

INT32 CRole::OnMount(INT32 nMountID)
{
    INT32 nResult = errUnknownError;
    TActivatedMountList::iterator itr;
    const MOUNT_ENTRY* pInfo = NULL;
    CAuraInstance* poAura = NULL;

    LOG_PROCESS_ERROR(m_nCurrentMountID == 0);
    LOG_PROCESS_ERROR(!m_oFlag.CheckFlag(cfOnMount));

    itr = m_ActivedMountList.find(nMountID);
    PROCESS_ERROR_RET_CODE(itr != m_ActivedMountList.end(), errMountNotExist);

    pInfo = CDataManager<MOUNT_ENTRY>::GetInstance().GetData(nMountID);
    LOG_PROCESS_ERROR_RET_CODE(pInfo, errMountNotExist);

    m_nCurrentMountID = nMountID;
    m_oFlag.SetFlag(cfOnMount);

    //	m_AttrList.AddAttrB(this, catMountMoveSpeed, pInfo->m_nSpeedModificationRate);

#ifndef _ROBOT
    if (!IsLoading())
        CCliMessageHandler::GetInstance().DoCharOnMount(this, nMountID);
#endif

    return errSuccess;
Exit0:
#ifndef _ROBOT
    this->ErrorCodeNotify(nResult, 0, 0);
#endif
    return nResult;
}

void CRole::OffMount(void)
{
    const MOUNT_ENTRY* pInfo = NULL;
    CAuraInstance* poAura = NULL;

    PROCESS_ERROR(m_oFlag.CheckFlag(cfOnMount));
    PROCESS_ERROR(m_nCurrentMountID != 0);

    m_oFlag.ClearFlag(cfOnMount);

    pInfo = CDataManager<MOUNT_ENTRY>::GetInstance().GetData(m_nCurrentMountID);
    LOG_PROCESS_ERROR(pInfo);

    m_nCurrentMountID = 0;
    //	m_AttrList.AddAttrB(this, catMountMoveSpeed, -pInfo->m_nSpeedModificationRate);

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoCharOffMount(this);
#endif

Exit0:
    return;
}

void CRole::SetPkValue(UINT32 dwPkValue)
{
    m_dwPkValue = dwPkValue;

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoSyncPkValueChange(this);
#endif
}

void CRole::IncComboKill(void)
{
    INT32 nRetCode = 0;

    m_nComboKill++;

    if ((m_nComboKill > 1) && (m_nComboKill <= 10))
    {
        GetAchievementList().AddVariable((avdFirstBloodCount + m_nComboKill - 1), 1);
    }

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncComboKill(this);
    LOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
    return;
}

BOOL CRole::GetVIPLevel(void)
{
    for (INT32 nIndex = vlLevel3; nIndex > vlLevel0; nIndex--)
    {
        if (m_dwVIPTime[nIndex] > CGameWorld::GetInstance().GetGameTime())
            return nIndex;
    }
    return vlLevel0;
}

INT32 CRole::SubscribeVIP(INT32 nVIPLevel, UINT32 dwMonth)
{
    UINT32 dwDuration = 0;
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    INT32 nMoney = 0;

    LOG_PROCESS_ERROR(nVIPLevel > vlLevel0 && nVIPLevel < vlTotal);
    LOG_PROCESS_ERROR(dwMonth < MAX_VIP_MONTH);

    dwDuration = dwMonth * 31 * 86400;
    if (m_dwVIPTime[nVIPLevel] > CGameWorld::GetInstance().GetGameTime())
    {
        m_dwVIPTime[nVIPLevel] += dwDuration;
    }
    else
    {
        m_dwVIPTime[nVIPLevel] = CGameWorld::GetInstance().GetGameTime() + dwDuration;
        m_ItemBag.UpdateMoneytype01DailyUsedLimit();
#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoSyncMoney(this);
        LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT
    }

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncVIPState(this);
    LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT

    GetAchievementList().AddVariable(avdSubscribeVipMonth, dwMonth);

    nResult = errSuccess;

Exit0:
    return nResult;
}

INT32 CRole::GetTotalOnlineTime(void)
{
    return m_AchievementList.GetVariable(avdOnlineTime) +
        (CGameWorld::GetInstance().GetGameFrame() - m_nLoginFrame) / GAME_FPS;
}

INT32 CRole::GetLevelOnlineTime(void)
{
    return m_AchievementList.GetVariable(avdLevelOnlineTime) +
        (CGameWorld::GetInstance().GetGameFrame() - m_nLevelUpFrame) / GAME_FPS;
}

/************************************************************************/
// Hero Begin

INT32 CRole::HireHero(INT32 nLevel, INT32 nClass, INT32 nSex, UINT32 dwHeroExp, INT32 eAiType, BOOL bFirstTime)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    CHero* poHero = NULL;

    PROCESS_ERROR_RET_CODE(m_poHero == NULL, errHeroAlreadyExist);

    nRetCode = m_CoolDown.CheckReady(MakeCoolDownID(COOLDOWN_MISC, COOLDOWN_MISC_HERO_REVIVE));
    PROCESS_ERROR_RET_CODE(nRetCode, errHeroNotReady);

    {
        const HERO_ATTR_DATA* pHeroData = CDataManager<HERO_ATTR_DATA>::GetInstance().GetData(MakeHeroAttrKey(nLevel, nClass));
        LOG_PROCESS_ERROR_RET_CODE(pHeroData, errHeroInvalid);

        poHero = CGameWorld::GetInstance().FindHero(MakeObjectID(otHero, GetObjID()));
        if (poHero == NULL)
        {
            poHero = CGameWorld::GetInstance().NewHero(MakeObjectID(otHero, GetObjID()));
            LOG_PROCESS_ERROR_RET_CODE(poHero, errHeroSummonFailed);
        }

        m_poHero = poHero;

        nRetCode = poHero->Init(this, CGameWorld::GetInstance().GetHeroID(poHero), dwHeroExp, nSex, pHeroData);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errHeroSummonFailed);
    }

    m_ItemBag.OpenHeroContainers();
    m_ItemBag.ApplyHeroEquipments();

    if (bFirstTime)
    {
        CLuaScript* poScript = CScriptMgr::GetInstance().FindScript(CREATE_HERO_SCRIPT_NAME);
        LOG_CHECK_ERROR(poScript);

        nRetCode = poScript->CallFunction("main", "oo", this, "CRole", poHero, "CHero");
        LOG_PROCESS_ERROR(nRetCode);
    }

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoHireHero(this);
    LOG_CHECK_ERROR(nRetCode);
#endif

    nRetCode = poHero->SetAiType(eAiType);
    LOG_CHECK_ERROR(nRetCode);

    return errSuccess;
Exit0:
    if (poHero != NULL)
    {
        poHero->UnInit();
        CGameWorld::GetInstance().DelHero(poHero);
    }

    m_poHero = NULL;

    return nResult;
}

INT32 CRole::FireHero(void)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;

    PROCESS_ERROR_RET_CODE(m_poHero, errHeroNotExist);

    nRetCode = m_ItemBag.GetUsedSlotCount(CONSLOT_HERO_EQUIPMENT);
    PROCESS_ERROR_RET_CODE(nRetCode == 0, errHeroContainerNotEmpty);

    nRetCode = m_ItemBag.GetUsedSlotCount(CONSLOT_HERO_EQUIPPED);
    PROCESS_ERROR_RET_CODE(nRetCode == 0, errHeroContainerNotEmpty);

    nResult = RecallHero(TRUE);
    LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

    m_ItemBag.CloseHeroContainers();

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoFireHero(this);
    LOG_CHECK_ERROR(nRetCode);
#endif

    nRetCode = CGameWorld::GetInstance().DelHero(m_poHero);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errHeroInvalid);

    m_poHero = NULL;

    return errSuccess;
Exit0:
    return nResult;

}

INT32 CRole::SummonHero(void)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    CAIProcessor* poHeroAi = NULL;

    PROCESS_ERROR_RET_CODE(m_poHero, errHeroNotExist);
    PROCESS_SUCCESS(m_poHero->IsSummoned());

    poHeroAi = m_poHero->GetAIProcessor();
    LOG_PROCESS_ERROR_RET_CODE(poHeroAi, errUnknownError);

    LOG_PROCESS_ERROR_RET_CODE(m_poScene, errUnknownError);

    nRetCode = m_CoolDown.CheckReady(MakeCoolDownID(COOLDOWN_MISC, COOLDOWN_MISC_HERO_REVIVE));
    PROCESS_ERROR_RET_CODE(nRetCode, errHeroNotReady);

    m_poHero->SetX(m_llX);
    m_poHero->SetY(m_llY);
    m_poHero->SetZ(m_llZ);

    nRetCode = m_poScene->AddHero(m_poHero, soatSpawn);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errUnknownError);

    m_poHero->SetSummoned(TRUE);

    nRetCode = poHeroAi->Reload();
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errUnknownError);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSummonHero(this);
    LOG_CHECK_ERROR(nRetCode);
#endif

Exit1:
    return errSuccess;
Exit0:
    return nResult;
}

INT32 CRole::RecallHero(BOOL bNotify)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    CScene* poScene = NULL;

    PROCESS_ERROR_RET_CODE(m_poHero, errHeroNotExist);
    PROCESS_SUCCESS(!m_poHero->IsSummoned());

    poScene = m_poHero->GetScene();
    PROCESS_ERROR_RET_CODE(poScene, errSuccess);

    nRetCode = poScene->RemoveHero(m_poHero);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errUnknownError);

    m_poHero->SetSummoned(FALSE);

    if (bNotify)
    {
#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoRecallHero(this);
        LOG_CHECK_ERROR(nRetCode);
#endif
    }

Exit1:
    return errSuccess;
Exit0:
    return nResult;
}

// Hero End 
/************************************************************************/

///////////////////////////// pet begin //////////////////////////////////
BOOL CRole::AddPet(UINT32 dwNpcID, INT32 nPetType)
{
    LOG_PROCESS_ERROR(m_nPetCount <= GetMaxPetCount());

    INF("pet debug log: CRole::AddPet HostID[%d] OBJID[%d]", GetObjID(), dwNpcID);
    for (INT32 i = 0; i < m_nPetCount; ++i)
        LOG_PROCESS_ERROR(dwNpcID != m_Pet[i].dwNpcID);

    m_Pet[m_nPetCount].dwNpcID = dwNpcID;
    m_Pet[m_nPetCount++].nPetType = nPetType;

#ifndef _ROBOT
    if (m_nPetCount == 1)
        this->ErrorCodeNotify(errPetChangeStateSuccess, GetPetState(), 0);
#endif //_ROBOT
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::DelPet(UINT32 dwNpcID)
{
    LOG_PROCESS_ERROR(m_nPetCount <= GetMaxPetCount());

    INF("pet debug log: CRole::DelPet HostID[%d] OBJID[%d]", GetObjID(), dwNpcID);
    for (INT32 i = 0; i < m_nPetCount; ++i)
    {
        if (dwNpcID == m_Pet[i].dwNpcID)
        {
            m_Pet[i].dwNpcID = m_Pet[m_nPetCount - 1].dwNpcID;
            m_Pet[i].nPetType = m_Pet[m_nPetCount - 1].nPetType;
            m_Pet[m_nPetCount - 1].dwNpcID = 0;
            m_Pet[m_nPetCount - 1].nPetType = 0;
            --m_nPetCount;
            break;
        }
    }

#ifndef _ROBOT
    if (m_nPetCount == 0)
        this->ErrorCodeNotify(errPetChangeStateSuccess, psInvalid, 0);
#endif //_ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::RecoverPet(void)
{
    INT32       nRetCode = 0;

    if (CGameWorld::GetInstance().GetGameTime() >= m_dwPetSaveTime)
    {
        memset(m_Pet, 0, sizeof(m_Pet));
        m_nPetCount = 0;
        return TRUE;
    }

    {
        if (GetPetCount() > 0)
            INF("pet debug log: CRole::RecoverPet HostID[%d] PetCount[%d]", GetObjID(), GetPetCount());

        for (INT32 i = 0; i < GetPetCount(); ++i)
        {
            CNpc* poNpc = NULL;
            LOG_PROCESS_ERROR(m_poScene);
            poNpc = m_poScene->SpawnNpc(m_Pet[i].dwNpcTemplateID, GetX(), GetY(),
                GetZ() + GRID_WIDTH, GetFaceDirection(), GetObjID(), NULL);
            LOG_PROCESS_ERROR(poNpc);

            m_Pet[i].dwNpcID = poNpc->GetObjID();
            nRetCode = poNpc->TransformToPet(m_Pet[i].nPetType, m_Pet[i].nLevel, m_Pet[i].nInitLevel, m_Pet[i].nMaxLevel, m_Pet[i].dwAliveTime, m_Pet[i].nPetExp);
            LOG_PROCESS_ERROR(nRetCode);

            poNpc->SetHp(m_Pet[i].nCurHp);
        }
    }

#ifndef _ROBOT 
    if (GetPetCount() > 0 && IsInLogin())
        this->ErrorCodeNotify(errPetChangeStateSuccess, GetPetState(), 0);
#endif

    return TRUE;
Exit0:
    ClearPet();
    return FALSE;
}

BOOL CRole::PackUpPet(void)
{
    INT32   nRetCode = 0;
    UINT32  dwNpcID = 0;
    CNpc* poNpc = NULL;

    if (GetPetCount() > 0)
        INF("pet debug log: CRole::PackUpPet HostID[%d] PetCount[%d]", GetObjID(), GetPetCount());

    for (INT32 i = 0; i < GetPetCount(); ++i)
    {
        dwNpcID = GetPetNpcID(i);
        LOG_CHECK_ERROR(dwNpcID != 0);

        poNpc = CGameWorld::GetInstance().FindNpc(dwNpcID);
        LOG_CHECK_ERROR(poNpc);
        if (poNpc == NULL)
            continue;

        LOG_CHECK_ERROR(poNpc->GetNpcData());
        if (poNpc->GetNpcData())
            m_Pet[i].dwNpcTemplateID = poNpc->GetNpcData()->nTemplateID;
        else
            m_Pet[i].dwNpcTemplateID = 0;
        m_Pet[i].nLevel = poNpc->GetPetLevel();
        m_Pet[i].nInitLevel = poNpc->GetPetInitLevel();
        m_Pet[i].nMaxLevel = poNpc->GetPetMaxLevel();
        m_Pet[i].dwAliveTime = poNpc->GetPetAliveFrm();
        m_Pet[i].nPetExp = poNpc->GetPetExp();
        m_Pet[i].nCurHp = poNpc->GetHp();

        LOG_CHECK_ERROR(poNpc->GetScene());
        if (poNpc->GetScene() != NULL)
        {
            INF("pet debug log: CRole::PackUpPet remove npc HostID[%d] PetCount[%d]", GetObjID(), GetPetCount());
            nRetCode = poNpc->GetScene()->RemoveNpc(poNpc);
            LOG_CHECK_ERROR(nRetCode);
        }

        nRetCode = poNpc->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelNpc(poNpc);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::ClearPet(void)
{
    INT32 nRetCode = 0;
    CNpc* poNpc = NULL;

    LOG_PROCESS_ERROR(GetPetCount() <= MAX_PET_PER_ROLE);

    for (INT32 i = 0; i < GetPetCount(); ++i)
    {
        if (GetPetNpcID(i) == 0)
            continue;
        poNpc = CGameWorld::GetInstance().FindNpc(GetPetNpcID(i));
        LOG_CHECK_ERROR(poNpc);
        if (poNpc == NULL)
            continue;

        LOG_CHECK_ERROR(poNpc->GetScene());
        if (poNpc->GetScene())
        {
            nRetCode = poNpc->GetScene()->RemoveNpc(poNpc);
            LOG_CHECK_ERROR(nRetCode);
        }

        nRetCode = poNpc->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelNpc(poNpc);
        LOG_CHECK_ERROR(nRetCode);
    }

    m_nPetCount = 0;
    memset(m_Pet, 0, sizeof(m_Pet));

#ifndef _ROBOT
    if (m_nPetCount == 0)
        this->ErrorCodeNotify(errPetChangeStateSuccess, psInvalid, 0);
#endif //_ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::ClearPet(INT32 nSpellEffectGroup)
{
    INT32 nRetCode = 0;
    CNpc* poNpc = NULL;

    LOG_PROCESS_ERROR(GetPetCount() <= MAX_PET_PER_ROLE);

    for (INT32 i = 0; i < GetPetCount(); )
    {
        poNpc = CGameWorld::GetInstance().FindNpc(GetPetNpcID(i));
        LOG_CHECK_ERROR(poNpc);
        if (poNpc == NULL)
        {
            ++i;
            continue;
        }

        if ((poNpc->GetNpcData()->nSpellEffectGroup & nSpellEffectGroup) == 0)
        {
            ++i;
            continue;
        }

        nRetCode = DelPet(poNpc->GetObjID());
        LOG_CHECK_ERROR(nRetCode);

        LOG_PROCESS_ERROR(poNpc->GetScene());
        nRetCode = poNpc->GetScene()->RemoveNpc(poNpc);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = poNpc->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelNpc(poNpc);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;

}


BOOL CRole::ClearRandomPet(void)
{
    INT32 nRetCode = 0;
    CNpc* poNpc = NULL;
    INT32 nIdx = 0;

    LOG_PROCESS_ERROR(m_nPetCount <= MAX_PET_PER_ROLE);

    nIdx = g_Random(m_nPetCount);

    poNpc = CGameWorld::GetInstance().FindNpc(GetPetNpcID(nIdx));
    LOG_PROCESS_ERROR(poNpc);

    nRetCode = DelPet(poNpc->GetObjID());
    LOG_CHECK_ERROR(nRetCode);

    LOG_PROCESS_ERROR(poNpc->GetScene());

    nRetCode = poNpc->GetScene()->RemoveNpc(poNpc);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = poNpc->UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGameWorld::GetInstance().DelNpc(poNpc);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;

}

BOOL CRole::CheckPet(INT32 nSpellEffectGroup)
{
    INT32 nRetCode = 0;
    CNpc* poNpc = NULL;

    LOG_PROCESS_ERROR(GetPetCount() <= MAX_PET_PER_ROLE);

    for (INT32 i = 0; i < GetPetCount(); i++)
    {
        poNpc = CGameWorld::GetInstance().FindNpc(GetPetNpcID(i));
        LOG_CHECK_ERROR(poNpc);

        if (poNpc == NULL)
            continue;

        if ((poNpc->GetNpcData()->nSpellEffectGroup & nSpellEffectGroup) != 0)
            return TRUE;
    }

Exit0:
    return FALSE;
}


///////////////////////////// pet end ////////////////////////////////////

BOOL CRole::_InitNewRole(void)
{
    INT32 nRetCode = 0;
    CLuaScript* poScript = NULL;

    m_ProMails.Clear();
    m_ItemBag.Clear();
    m_Stall.Clear();
    m_Friend.Clear();
    m_Title.clear();

    // Execute Lua script "script/create_role/main.lua"
    poScript = CScriptMgr::GetInstance().FindScript(CREATE_ROLE_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    nRetCode = poScript->CallFunction("main", "o", this, "CRole");
    LOG_PROCESS_ERROR(nRetCode);

    AddExp(1, FALSE, FALSE);

    nRetCode = _InitMp();
    LOG_PROCESS_ERROR(nRetCode);

    m_RevivePos.nSceneTempID = g_GWConfig.nDefaultReviveSceneTempID;
    m_RevivePos.nSceneID = 0;
    m_RevivePos.nX = CONVERT_LENGTH(g_GWConfig.DefaultRevivePos.x);
    m_RevivePos.nY = CONVERT_LENGTH(g_GWConfig.DefaultRevivePos.y);
    m_RevivePos.nZ = CONVERT_LENGTH(g_GWConfig.DefaultRevivePos.z);
    m_RevivePos.nFaceDirection = 0;

    StartAntiCheat();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::CheckVolume(void)
{
    INT32 nRetCode = 0;
    INT32 nVolumeID = -1;

    if (m_pcGrid && m_pcZone && m_pcGrid->uVolume > 0)
    {
        nVolumeID = m_pcZone->byVolume[m_pcGrid->uVolume];
    }

    if (nVolumeID != m_nVolumeID)
    {
        if (nVolumeID >= 0)
        {
            LOG_PROCESS_ERROR(m_poScene);
            nRetCode = m_poScene->TouchVolume(this, nVolumeID);
            LOG_PROCESS_ERROR(nRetCode);
        }

        m_nVolumeID = nVolumeID;
    }

    return TRUE;
Exit0:
    if (m_poScene)
        CRI("failed to trigger touch volume %d event in scene %s", nVolumeID, m_poScene->GetSceneData()->szConfigName);

    return FALSE;
}

void CRole::Hide(BOOL bHidden)
{
    INT32 nRetCode = 0;

#ifndef _ROBOT
    if (m_bHidden && !bHidden)
    {
        SetBodyType(body1x1);

        if (m_poScene)
        {
            nRetCode = CCliMessageHandler::GetInstance().DoCharAppear(this, NULL, soatLogin);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
    else if (!m_bHidden && bHidden)
    {
        SetBodyType(body0x0);

        if (m_poScene)
        {
            nRetCode = CCliMessageHandler::GetInstance().DoObjectDisappear(this, NULL, sodtLeaveScene);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
#endif	// _ROBOT

    m_bHidden = bHidden;
}

void CRole::Kick()
{
    KICK_ROLE(this, ktImmediately);
}

void CRole::Kick(INT32 eKickType, const char* pcszFileName, const char* pcszFuncName, INT32 nLineNum)
{
    INT32 nRetCode = 0;

    PROCESS_ERROR(!m_bKicked);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoDLKickRole(eKickType, m_szUserName, m_dwSessionID, m_szName, m_dwObjID, pcszFileName, nLineNum, pcszFuncName);
    LOG_CHECK_ERROR(nRetCode);

#if defined(_FINAL)
    if (eKickType == ktImmediately)
    {
        CRI("kick role #%s:%d# session <%08X> user %s immediately at line %d in function %s() file \"%s\"",
            m_szName, m_dwObjID, m_dwSessionID, m_szUserName, nLineNum, pcszFuncName, pcszFileName);

        nRetCode = CRoleKickMgr::GetInstance().AddToKickList(GetObjID());
        LOG_PROCESS_ERROR(nRetCode);
    }
    else if (eKickType == ktDelayed)
    {
        INT32 nDelayFrame = g_Random(GAME_FPS * 1200);
        CRI("kick role #%s:%d# session <%08X> user %s in %d frame at line %d in function %s() file \"%s\"",
            m_szName, m_dwObjID, m_dwSessionID, m_szUserName, m_nLogoutFrame, nLineNum, pcszFuncName, pcszFileName);

        nRetCode = Logout(rltKickout, nDelayFrame);
        LOG_CHECK_ERROR(nRetCode);
    }
    else if (eKickType == ktLogged)
    {
        CRI("kick role #%s:%d# session <%08X> user %s logged at line %d in function %s() file \"%s\"",
            m_szName, m_dwObjID, m_dwSessionID, m_szUserName, nLineNum, pcszFuncName, pcszFileName);
    }
    else
    {
        CRI("kick role #%s:%d# session <%08X> user %s (unknown kick type) at line %d in function %s() file \"%s\"",
            m_szName, m_dwObjID, m_dwSessionID, m_szUserName, nLineNum, pcszFuncName, pcszFileName);
    }
#else
    CRI("kick role #%s:%d# session <%08X> user %s immediately at line %d in function %s() file \"%s\"",
        m_szName, m_dwObjID, m_dwSessionID, m_szUserName, nLineNum, pcszFuncName, pcszFileName);

    nRetCode = CRoleKickMgr::GetInstance().AddToKickList(GetObjID());
    LOG_PROCESS_ERROR(nRetCode);
#endif  // _FINAL

#endif  // _ROBOT

Exit0:
    m_bKicked = TRUE;

    return;
}

BOOL CRole::ErrorCodeNotify(INT32 nErrCode, INT32 nParam1, INT32 nParam2)
{
#ifndef _ROBOT
    return CCliMessageHandler::GetInstance().DoErrorCodeNotify(this, nErrCode, nParam1, nParam2);
#else
    return TRUE;
#endif  // _ROBOT
}

INT32 CRole::GetPow(void)
{
    const ROLE_LEVEL_DATA* pData = NULL;
    const SPIRIT_DATA* pSpiritData = NULL;

    pData = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(m_nLevel);
    if (pData == nullptr)
    {
        LOG_CHECK_ERROR(pData);
        return 0;
    }

    INT32 nBasePow = pData->m_nPow;

    INT32 nSpellPow = m_SpellBook.GetPow();

    INT32 nItemPow = GetItemBag()->GetPow();

    INT32 nSpiritPow = GetSpirit().GetPow();

    INT32 nTitlePow = GetTitle().GetPow();

    return nBasePow + nItemPow + nSpiritPow + nTitlePow + nSpellPow;
}

void CRole::TryDailyRefresh()
{
    INT32 timezone = CGameWorld::GetInstance().GetTimeZone();
    UINT32 dwCurTime = CGameWorld::GetInstance().GetGameTime();
    INT32 nRetCode = 0;

    if ((m_dwLastDailyRefreshTime + timezone) / 86400 != (dwCurTime + timezone) / 86400)
    {
        m_nExpToday = 0;
        GetItemBag()->UpdateDaily();

        GetItemBag()->SetMoney(MONEYTYPE_FATIGUE, g_GWConfig.VIPData[GetVIPLevel()].nDailyFatigue);
        ClearDungeonEnterCount();

        if (m_eState == rsPlaying)
        {
#ifndef _ROBOT
            nRetCode = CCliMessageHandler::GetInstance().DoSyncMoney(this);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CCliMessageHandler::GetInstance().DoSyncCheckInInfo(this);
            LOG_CHECK_ERROR(nRetCode);
#endif
        }

        ClearActivity();

        nRetCode = m_QuestList.DailyRefresh();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_KillNpcQuest.ResetDailyQuest();
        LOG_CHECK_ERROR(nRetCode);

        m_dwLastDailyRefreshTime = dwCurTime;
    }
}

void CRole::TryWeeklyRefresh()
{
    INT32 timezone = CGameWorld::GetInstance().GetTimeZone();
    UINT32 dwCurTime = CGameWorld::GetInstance().GetGameTime();
    INT32 nRetCode = 0;

    if ((m_dwLastWeeklyRefreshTime + 86400 * 3 + timezone) / (86400 * 7) != (dwCurTime + 86400 * 3 + timezone) / (86400 * 7))
    {
        GetItemBag()->UpdateWeekly();
        if (m_eState == rsPlaying)
        {
#ifndef _ROBOT
            nRetCode = CCliMessageHandler::GetInstance().DoSyncMoney(this);
            LOG_CHECK_ERROR(nRetCode);
#endif
        }

        nRetCode = m_QuestList.WeeklyRefresh();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_KillNpcQuest.ResetWeeklyQuest();
        LOG_CHECK_ERROR(nRetCode);

        m_dwLastWeeklyRefreshTime = dwCurTime;
    }
}

CRole::_ReviveFunc CRole::m_ReviveFunc[] =
{
    NULL,
    &CRole::_OnAtCurrentPosRevive,
    &CRole::_OnToGraveyardRevive,
    &CRole::_OnToGraveyardRevive,
};

INT32 CRole::TryRevive(INT32 nType)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    INT32 nRequireItemID = 0;
    INT32 nRequireItemCount = 0;
    INT32 nItemCount = 0;
    INT64 llCurFrame = CGameWorld::GetInstance().GetGameFrame();
    INT32 eKillerObjType = GetTypeFromID(m_dwLastCasterID);

    LOG_PROCESS_ERROR(nType > crtInvalid && nType < crtTotal);

    LOG_PROCESS_ERROR(m_poScene);
    PROCESS_ERROR_RET_CODE(IsDead(), errReviveStillAlive);

    if (nType == crtInstant)
        PROCESS_ERROR_RET_CODE(eKillerObjType == otRole, errReviveForbidden);

    if (nType == crtFree && eKillerObjType == otRole)
        PROCESS_ERROR_RET_CODE(llCurFrame >= m_nNextFreeReviveFrm, errReviveNotReady);

    nRequireItemID = g_GWConfig.ReviveParam[nType].nRequireItemID;

    if (nRequireItemID != 0)
    {
        if (nRequireItemID < MONEYTYPE_COUNT && nRequireItemID > MONEYTYPE_INVALID)
            nItemCount = m_ItemBag.GetMoney(nRequireItemID);
        else
            nItemCount = m_ItemBag.GetItemCount(nRequireItemID);
    }

    nRequireItemCount = g_GWConfig.ReviveParam[nType].nRequireItemCount;

    //	if (nType == crtAtCurrentPos)
    //	{
    //		if (nNowFrm - m_nTryReviveFrm[crtAtCurrentPos] > CONVERT_TIME(g_GWConfig.nInstantReviveResetTime))
    //			m_nTryReviveCount[crtAtCurrentPos] = 0;
    //
    //		nRequireItemCount = g_GWConfig.ReviveParam[nType].nRequireItemCount * (m_nTryReviveCount[nType] + 1);
    //		nRequireItemCount = nRequireItemCount < g_GWConfig.ReviveParam[nType].nRequireItemMaxCount ? nRequireItemCount : g_GWConfig.ReviveParam[nType].nRequireItemMaxCount;
    //	}

    PROCESS_ERROR_RET_CODE(nItemCount >= nRequireItemCount, errReviveLackOfItem);

    if (nRequireItemID != 0)
    {
        if (nRequireItemID < MONEYTYPE_COUNT && nRequireItemID > MONEYTYPE_INVALID)
            nRetCode = m_ItemBag.SubMoney(nRequireItemID, nRequireItemCount, MONEYLOG_REVIVE, 0);
        else
            nRetCode = m_ItemBag.DeleteItemId(nRequireItemID, nRequireItemCount, ITEMLOG_REVIVE, FALSE, 0);

        LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
    }


    nRetCode = (this->*m_ReviveFunc[nType])();
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errReviveUnknown);

    //	m_eReviveType = nType;
    //	m_nTryReviveFrm[nType] = nNowFrm + CONVERT_TIME(g_GWConfig.ReviveParam[nType].nDelayTime);
    //	m_nTryReviveCount[nType]++;

    //#ifndef _ROBOT
    //	nRetCode = CCliMessageHandler::GetInstance().DoReviveResponse(this, g_GWConfig.ReviveParam[nType].nDelayTime);
    //	LOG_CHECK_ERROR(nRetCode);
    //#endif  // _ROBOT

    return errSuccess;
Exit0:
    return nResult;
}

//BOOL CRole::_OnInstantRevive(void)
//{
//	INT32 nRetCode = 0;
//
//	nRetCode = Revive(crtAtCurrentPos, g_GWConfig.ReviveParam[crtAtCurrentPos].nHpPercent);
//	LOG_PROCESS_ERROR(nRetCode);
//
//	return TRUE;
//Exit0:
//	return FALSE;
//}

BOOL CRole::_OnAtCurrentPosRevive(void)
{
    INT32 nRetCode = 0;

    nRetCode = Revive(crtAtCurrentPos, g_GWConfig.ReviveParam[crtInstant].nHpPercent);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::_OnToGraveyardRevive(void)
{
    INT32 nRetCode = 0;
    const SERVER_POINT* pt = NULL;
    INT32 eSceneType = estInvalid;

    LOG_PROCESS_ERROR(m_poScene);

    eSceneType = m_poScene->GetSceneData()->nType;

    nRetCode = Revive(crtFree, g_GWConfig.ReviveParam[crtFree].nHpPercent);
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
    if (IsInSabacZone(this))
    {
        INT32 nSceneTempID = g_GWConfig.Sabac.nOtherReviveSceneID;
        GAME_WORLD_CONFIG::POS& RevivePos = g_GWConfig.Sabac.OtherRevivePos;

        if (m_dwGuildID != 0)
        {
            CGSGuild* poGSGuild = CGuildMgr::GetInstance().FindGuild(m_dwGuildID);
            LOG_PROCESS_ERROR(poGSGuild);

            if (m_dwGuildID == CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID))
            {
                nSceneTempID = m_nSceneTempID;
                RevivePos = g_GWConfig.Sabac.DefendRevivePos;
            }
            else if (poGSGuild->IsJoinSabacToday())
            {
                nSceneTempID = g_GWConfig.Sabac.nAttackReviveSceneID;
                RevivePos = g_GWConfig.Sabac.AttackRevivePos;
            }
        }

        nRetCode = CGameWorld::GetInstance().JumpTo(this, nSceneTempID, 0,
            RevivePos.x * POINT_PER_CM, RevivePos.y * POINT_PER_CM, RevivePos.z * POINT_PER_CM, m_nFaceDirection);
        LOG_PROCESS_ERROR(nRetCode);

        PROCESS_SUCCESS(TRUE);
    }
#endif	// ROBOT

    if (IsRedName())
    {
        if (eSceneType != estBattleField && eSceneType != estArena)
        {
            nRetCode = CGameWorld::GetInstance().JumpTo(this, g_GWConfig.nRedNameSceneTempID, 0,
                CONVERT_LENGTH(g_GWConfig.RedNameReturnPos.x),
                CONVERT_LENGTH(g_GWConfig.RedNameReturnPos.y),
                CONVERT_LENGTH(g_GWConfig.RedNameReturnPos.z), m_nFaceDirection);

            LOG_PROCESS_ERROR(nRetCode);

            PROCESS_SUCCESS(TRUE);
        }
    }

    if (eSceneType == estStatic || eSceneType == estLine || eSceneType == estUnique)
    {
        nRetCode = JumpToRevivePos();
        LOG_PROCESS_ERROR(nRetCode);
    }
    else if (eSceneType == estDungeon || eSceneType == estGuild || eSceneType == estSoloDungeon)
    {
        CSceneStaticData* pStaticData = m_poScene->GetStaticData();
        LOG_PROCESS_ERROR(pStaticData);
        {
            const SERVER_POINT* pt = pStaticData->GetNearestRevivePoint(m_nFaction, m_llX, m_llY, m_llZ);
            if (pt)
            {
                nRetCode = JumpTo(pt->nX, pt->nY, pt->nZ, m_nFaction);
                LOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                nRetCode = JumpToRevivePos();
                LOG_PROCESS_ERROR(nRetCode);
            }
        }
    }
    else
    {
        nRetCode = JumpToReturnPos();
        LOG_PROCESS_ERROR(nRetCode);
    }
Exit1:

    return TRUE;
Exit0:
#ifndef _ROBOT
    this->ErrorCodeNotify(errReviveNoGraveyard, 0, 0);
#endif  // _ROBOT

    return FALSE;
}

BOOL CRole::_CaculateKillNpc(void)
{
    INT32 nRetCode = 0;

    // consecutive kill

    if (m_nConsecutiveKillNpcFrame != 0 && m_nConsecutiveKillNpcFrame < CGameWorld::GetInstance().GetGameFrame())
    {
        if (m_nConsecutiveKillNpcCount >= MIN_CONSECUTIVE_KILL_NPC_COUNT)
        {
            INT32 nExp = 0;

            INT32 nConsecutiveKillNpcExp = m_nConsecutiveKillNpcExp / 5;
            nConsecutiveKillNpcExp += GetLevel() * m_nConsecutiveKillNpcCount / 5;

            INT32 nExp_Compare = GetLevel() * GetLevel() * 5;
            nExp = (nConsecutiveKillNpcExp > nExp_Compare) ? nExp_Compare : nConsecutiveKillNpcExp;

            if (GetAchievementList().GetVariable(avdMaxConsecutiveKillNpcCount) < m_nConsecutiveKillNpcCount)
            {
                GetAchievementList().SetVariable(avdMaxConsecutiveKillNpcCount, m_nConsecutiveKillNpcCount);
            }

            AddExp(nExp, FALSE, TRUE);

#ifndef _ROBOT
            nRetCode = CCliMessageHandler::GetInstance().DoUpdateConsecutiveKillNpcEnd(this, nExp);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CCliMessageHandler::GetInstance().DoDLConsecutiveKillNpcCount(GetSessionID(),
                m_nConsecutiveKillNpcCount);
            LOG_CHECK_ERROR(nRetCode);
#endif
        }

        m_nConsecutiveKillNpcFrame = 0;
        m_nConsecutiveKillNpcExp = 0;
        m_nConsecutiveKillNpcCount = 0;
    }

    // momentary kill

    if (m_nMomentaryKillNpcCount[0])
    {
        INT32 nFormerKillCount = m_nMomentaryKillNpcCount[0] + m_nMomentaryKillNpcCount[1];
        INT32 nFormerKillExp = m_nMomentaryKillNpcExp[0] + m_nMomentaryKillNpcExp[1];

        INT32 nLatterKillCount = m_nMomentaryKillNpcCount[1] + m_nMomentaryKillNpcCount[2];
        INT32 nLatterKillExp = m_nMomentaryKillNpcExp[1] + m_nMomentaryKillNpcExp[2];

        INT32 nMaxKillCount = (nFormerKillCount >= nLatterKillCount) ? nFormerKillCount : nLatterKillCount;
        INT32 nTotalExp = (nFormerKillCount >= nLatterKillCount) ? nFormerKillExp : nLatterKillExp;

        if (nMaxKillCount >= MIN_MOMENTARY_KILL_NPC_COUNT)
        {
            INT32 nKillMomentaryExp = 0;

            INT32 nNpcExp = nTotalExp / 5;
            INT32 nRoleLimitExp = GetLevel() * GetLevel() * 5;

            nKillMomentaryExp = (nNpcExp > nRoleLimitExp) ? nRoleLimitExp : nNpcExp;

            if (GetAchievementList().GetVariable(avdMaxMomentaryKillNpcCount) < nMaxKillCount)
            {
                GetAchievementList().SetVariable(avdMaxMomentaryKillNpcCount, nMaxKillCount);
            }

            AddExp(nKillMomentaryExp, FALSE, TRUE);

#ifndef _ROBOT
            nRetCode = CCliMessageHandler::GetInstance().DoUpdateMomentaryKillNpcEnd(this, nMaxKillCount, nKillMomentaryExp);
            LOG_PROCESS_ERROR(nRetCode);

            nRetCode = CCliMessageHandler::GetInstance().DoDLMomentaryKillNpcCount(GetSessionID(), nMaxKillCount);
            LOG_CHECK_ERROR(nRetCode);
#endif
        }
    }

    m_nMomentaryKillNpcCount[0] = m_nMomentaryKillNpcCount[1];
    m_nMomentaryKillNpcCount[1] = m_nMomentaryKillNpcCount[2];
    m_nMomentaryKillNpcCount[2] = 0;

    m_nMomentaryKillNpcExp[0] = m_nMomentaryKillNpcExp[1];
    m_nMomentaryKillNpcExp[1] = m_nMomentaryKillNpcExp[2];
    m_nMomentaryKillNpcExp[2] = 0;

    return TRUE;
Exit0:
    return FALSE;

}

BOOL CRole::Transform(BOOL bTransform)
{
    if (bTransform)
    {
        m_poCurrentSlot = &m_SpellSlotForTransform[0];
    }
    else
    {
        m_poCurrentSlot = &m_SpellSlot[0];
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::Revive(INT32 nParam, INT32 nHpPercent)
{
    INT32 nRetCode = 0;
    LOG_PROCESS_ERROR(m_poScene && m_pGridInstance);
    LOG_PROCESS_ERROR(m_eMoveState == CMS_DIE);

    m_nGreyNameFrame = INT_MAX;
    //INF("role %s<%x> Revive scene(%d,%d) grid(%lld,%lld,%lld) uRoleRef++ = %d ", GetName(), GetSessionID(), 
    //    GetSceneTempID(), GetSceneID(), GetX()/GRID_WIDTH, GetY()/GRID_WIDTH, GetZ()/GRID_WIDTH, m_pGridInstance->uRoleRef );

    nRetCode = CCharacter::Revive(nParam, nHpPercent);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void CRole::CombatPoseSwitch(BOOL bOn, INT32 nFlag)
{
    if (bOn)
        GetFlag().SetFlag(cfCombatPoseOn);
    else
        GetFlag().ClearFlag(cfCombatPoseOn);

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoCombatPoseSwitch(this, nFlag);
#endif // _ROBOT
}

INT32 CRole::OnKillNpc(CNpc* pNpc, INT32 nExp)
{
    INT32 nRetCode = 0;
    INT32 nMomentaryKillNpcCount = 0;

    LOG_PROCESS_ERROR(pNpc);
    LOG_PROCESS_ERROR(pNpc->GetNpcData());

    //Consecutive kills
    m_nConsecutiveKillNpcCount++;
    m_nConsecutiveKillNpcExp += nExp;
    m_nConsecutiveKillNpcFrame = CGameWorld::GetInstance().GetGameFrame() + MAX_CONSECUTIVE_KILL_NPC_FRAME;

    // Momentary kills
    m_nMomentaryKillNpcCount[MOMENTARY_KILL_NPC_TIEM - 1]++;
    m_nMomentaryKillNpcExp[MOMENTARY_KILL_NPC_TIEM - 1] += nExp;

#ifndef _ROBOT
    if (m_nConsecutiveKillNpcCount >= MIN_CONSECUTIVE_KILL_NPC_COUNT)
    {
        nRetCode = CCliMessageHandler::GetInstance().DoUpdateConsecutiveKillNpcCount(this);
        LOG_PROCESS_ERROR(nRetCode);
    }
#endif

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CRole::OnKillPlayer(CRole* poVictim)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poVictim);

    CStatistic::GetInstance().OnKilledByPlayer(this, poVictim);

    // consecutive kill player

    if (m_nConsecutiveKillPlayerFrame >= CGameWorld::GetInstance().GetGameFrame())
        m_nConsecutiveKillPlayerCount++;
    else
        m_nConsecutiveKillPlayerCount = 1;
    m_nConsecutiveKillPlayerFrame = CGameWorld::GetInstance().GetGameFrame() + MAX_CONSECTIVE_KILL_PLAYER_FRAME;

    if (m_nConsecutiveKillPlayerCount >= MAX_CONSECUTIVE_KILL_PLAYER_COUNT)
    {
        if (GetAchievementList().GetVariable(avdMaxConsecutiveKillPlayerCount) < m_nConsecutiveKillPlayerCount)
        {
            GetAchievementList().SetVariable(avdMaxConsecutiveKillPlayerCount, m_nConsecutiveKillPlayerCount);
        }

#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoUpdateConsecutiveKillPlayerCount(this);
        LOG_PROCESS_ERROR(nRetCode);
#endif	

    }

    // quest
    GetQuestList().OnKillPlayer(poVictim->GetLevel(), &poVictim->GetAuraList());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::GMCreateGuild(char* pszGuildName, char* pszAnnounce)
{
#ifdef _DEBUG
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(pszGuildName);
    LOG_PROCESS_ERROR(pszAnnounce);

    nRetCode = GetItemBag()->AddMoney(MONEYTYPE_02, g_GuildConfig.nCanCreateGuildMoney, MONEYLOG_GM_COMMAND, 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetItemBag()->AddItem(g_GuildConfig.nCanCreateGuildItemTypeID, g_GuildConfig.nCanCreateGuildItemNum, ITEMLOG_GM_COMMAND, 0, FALSE);
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSGuildCreate(GetObjID(), pszGuildName, pszAnnounce);
    LOG_PROCESS_ERROR(nRetCode);
#endif	// _ROBOT

#endif // _DEBUG
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::HasMasterRelationshipInTeam(void)
{
    INT32 nRetCode = 0;
    CTeamInGS* pTeam = NULL;

    INT64 llShareDistance2 = g_GWConfig.nExpShareDistance * POINT_PER_CM * g_GWConfig.nExpShareDistance * POINT_PER_CM;

    PROCESS_ERROR(m_dwTeamID != INVALID_OBJECT_ID);

    pTeam = CGameWorld::GetInstance().FindTeam(m_dwTeamID);
    PROCESS_ERROR(pTeam);

    nRetCode = pTeam->HasMasterRelationShip(this, llShareDistance2);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::IsMasterRelationship(CRole* poRole)
{
    PROCESS_ERROR(poRole->GetObjID() != m_dwObjID);

    if (GetMaster().GetTeacherID() == poRole->GetObjID())
        return TRUE;
    if (poRole->GetMaster().GetTeacherID() == m_dwObjID)
        return TRUE;
Exit0:
    return FALSE;
}

CSpell* CRole::GetEquippingSpell(void)
{
    if (m_EquippingSpellStack.empty())
        return NULL;

    return m_EquippingSpellStack.top();
}

BOOL CRole::AddItemSpell(Item* poItem, INT32 nSpellID, INT32 nLevel)
{
    INT32 nRetCode = 0;
    CSpellSet* poSet = NULL;

    LOG_PROCESS_ERROR(poItem);

    poSet = CreateSpellSet(nSpellID);
    LOG_PROCESS_ERROR(poSet);

    nRetCode = poSet->AddItemSpellLevelNode(poItem->m_iteminfo.m_llGuid, nLevel);
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoLearnItemSpell(this, nSpellID, nLevel, m_nPendingItemSlot);
    LOG_CHECK_ERROR(nRetCode);
#endif
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::DelItemSpell(Item* poItem, INT32 nSpellID)
{
    INT32 nRetCode = 0;
    CSpellSet* poSet = NULL;

    LOG_PROCESS_ERROR(poItem);

    poSet = m_SpellBook.GetSpellSet(nSpellID);
    LOG_PROCESS_ERROR(poSet);

    if (poSet->GetRefCount() <= 1 && poSet->GetEquipSlot() != qssSlotInvalid)
    {
        EquipSpell(poSet->GetEquipSlot(), 0);
    }

    nRetCode = poSet->RemoveItemSpellLevelNode(poItem->m_iteminfo.m_llGuid);
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncSpellPts(this, nSpellID, poSet->GetPts());
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoRemoveItemSpell(this, nSpellID, m_nPendingItemSlot);
    LOG_CHECK_ERROR(nRetCode);

#endif // !_ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CRole::QueryRoleLocation(const char* pcszRoleName)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(GetCCFlag().CheckFlag(cclProbeNecklace));
    PROCESS_ERROR_RET_CODE(GetCoolDown().CheckReady(MakeCoolDownID(COOLDOWN_SPECIAL_ITEM, cclProbeNecklace)), errItemNotReady);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSQueryRoleLocation(this, pcszRoleName, TRUE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errItemRoleNotFound);
#endif //_ROBOT

    nResult = errSuccess;
Exit0:
    return nResult;
}

INT32 CRole::RecallTeamMember(void)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(GetTeamID() > 0);
    LOG_PROCESS_ERROR(GetCCFlag().CheckFlag(cclMemoSuit));
    LOG_PROCESS_ERROR(m_poScene && m_poScene->GetSceneData());
    PROCESS_ERROR_RET_CODE(GetCoolDown().CheckReady(MakeCoolDownID(COOLDOWN_SPECIAL_ITEM, cclMemoSuit)), errItemNotReady);
    PROCESS_ERROR_RET_CODE(!m_poScene->GetSceneData()->bCannotBeRecalled, errItemCannotUse);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSRecallTeamMember(this);
    LOG_PROCESS_ERROR(nRetCode);
#endif //_ROBOT

    nRetCode = GetCoolDown().Add(MakeCoolDownID(COOLDOWN_SPECIAL_ITEM, cclMemoSuit),
        g_GWConfig.nMemoSuitCDTime * GAME_FPS / 1000);
    LOG_PROCESS_ERROR(nRetCode);

    nResult = errSuccess;
Exit0:
    return nResult;
}

BOOL CRole::IsSabacGuildMember(void)
{
    if (m_dwGuildID == 0)
        return FALSE;
    else if (CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID) == 0)
        return FALSE;
    else if (m_dwGuildID == CSabacMgr::GetInstance().GetOccupyGuildID(m_nServerGroupID))
        return TRUE;

    return FALSE;
}

INT32 CRole::CheckIn(void)
{
    INT32 nResult = errUnknownError;

    INT32 nIndex = m_LastCheckInInfo.nIndex;
    UINT32 dwLastCheckInTime = m_LastCheckInInfo.dwLastCheckInTime;

    PROCESS_ERROR_RET_CODE(IsCheckInValid(), errCheckInClosed);
    PROCESS_ERROR_RET_CODE(CanCheckInToday(), errCheckInAlreadyDid);

    {
        const CHECKIN_DATA* pData = CDataManager<CHECKIN_DATA>::GetInstance().GetData(++nIndex);
        LOG_PROCESS_ERROR_RET_CODE(pData, errCheckInNotExist);

        PROCESS_ERROR_RET_CODE(m_nLevel >= pData->nLevelLimit, errCheckInLevelLimit);

        nResult = m_ItemBag.AddItem(pData->nAwardItemID, pData->nAwardItemCount, ITEMLOG_CHECKIN_AWARD, 0, TRUE);
        PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);
    }

    m_LastCheckInInfo.nIndex = nIndex;
    m_LastCheckInInfo.dwLastCheckInTime = CGameWorld::GetInstance().GetGameTime();

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoCheckIn(this, nIndex);
#endif // !x_Robot

    return errSuccess;
Exit0:
    return nResult;
}

BOOL CRole::CanCheckInToday(void)
{
    UINT32 dwServerStartTime = 0;
    LOG_PROCESS_ERROR(m_nServerGroupID < MAX_SERVER_GROUP_COUNT);

    PROCESS_SUCCESS(m_LastCheckInInfo.dwLastCheckInTime == 0);

    dwServerStartTime = g_ServerConfig.GroupConfig[m_nServerGroupID].dwStartDate;

    return (CGameWorld::GetInstance().GetGameTime() - dwServerStartTime) / 86400 > (m_LastCheckInInfo.dwLastCheckInTime - dwServerStartTime) / 86400;
Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::IsCheckInValid(void)
{
    UINT32 dwServerStartTime = 0;
    LOG_PROCESS_ERROR(m_nServerGroupID < MAX_SERVER_GROUP_COUNT);

    dwServerStartTime = g_ServerConfig.GroupConfig[m_nServerGroupID].dwStartDate;

    return (CGameWorld::GetInstance().GetGameTime() - dwServerStartTime) / 86400 < 28;
Exit0:
    return FALSE;
}

BOOL CRole::UpdateRankListNode(INT32 nRankListType, INT32 nValue1, INT32 nValue2)
{
    INT32 nRetCode = 0;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSUpdateRankListNode(
        m_nServerGroupID, nRankListType, m_dwObjID, ((INT64)nValue1 << 32) | nValue2);
    LOG_PROCESS_ERROR(nRetCode);
#endif //_ROBOT
    return TRUE;
Exit0:
    return FALSE;
}

UINT32 CRole::GetCurExerciseTime() const
{
    if (m_dwStartExerciseTime)
    {
        UINT32 cur_time = CGameWorld::GetInstance().GetGameTime();
        LOG_PROCESS_ERROR(cur_time > m_dwStartExerciseTime);

        UINT32 time_diff = cur_time - m_dwStartExerciseTime;
        return (time_diff > m_dwTotalExerciseTime) ? m_dwTotalExerciseTime : time_diff;
    }

Exit0:
    return 0;
}

void CRole::StartAntiCheat(void)
{
    INT32 nAntiCheatIndex = 0;

    LOG_PROCESS_ERROR(m_nAntiCheatLevel < MAX_ROLE_ANTI_CHEAT_LEVEL);
    LOG_PROCESS_ERROR(m_nAntiCheatPunishLevel >= 0 && m_nAntiCheatPunishLevel < MAX_ROLE_ANTI_CHEAT_PUNISH_LEVEL);

    nAntiCheatIndex = m_nAntiCheatLevel;

    if (m_nAntiCheatLevel < 0)
        nAntiCheatIndex = 0;

    //if (m_dwNextJigsawTime < CGameWorld::GetInstance().GetGameTime())
    //	m_dwNextJigsawTime = CGameWorld::GetInstance().GetGameTime() + g_Random(g_GWConfig.nAntiCheatInterval[nAntiCheatIndex] * (RATE_MAX - g_GWConfig.nAntiCheatIntervalFloatPct) / RATE_MAX, g_GWConfig.nAntiCheatInterval[nAntiCheatIndex] * (RATE_MAX + g_GWConfig.nAntiCheatIntervalFloatPct) / RATE_MAX);

    m_nJigsawRemainTime = g_Random(g_GWConfig.nAntiCheatInterval[nAntiCheatIndex] * (RATE_MAX - g_GWConfig.nAntiCheatIntervalFloatPct) / RATE_MAX, g_GWConfig.nAntiCheatInterval[nAntiCheatIndex] * (RATE_MAX + g_GWConfig.nAntiCheatIntervalFloatPct) / RATE_MAX);

    INF("anticheat:	start	<%s>	%d	%d	%d", m_szName, m_nAntiCheatLevel, m_nAntiCheatPunishLevel, m_nJigsawRemainTime);
    //	DBG("Next Jigsaw Time: %s, anti-cheat level: %d, punish level : %d", get_time_str(m_dwNextJigsawTime), nAntiCheatIndex, m_nAntiCheatPunishLevel);

    m_nJigsawAnswerIndex = -1;

Exit0:
    return;
}

void CRole::EndAntiCheat(void)
{
    //	m_dwNextJigsawTime = 0;
    m_nJigsawRemainTime = 0x7FFFFFFF;
}

void CRole::SuccessAntiCheat(void)
{
    INT32 nRetCode = 0;
    INT32 nRewardExp = 0;

    if (m_nAntiCheatLevel < MAX_ROLE_ANTI_CHEAT_LEVEL - 1)
        m_nAntiCheatLevel++;
    else
        m_nAntiCheatLevel = MAX_ROLE_ANTI_CHEAT_LEVEL - 1;

    LOG_PROCESS_ERROR(m_nAntiCheatLevel >= 0);

    if (m_nAntiCheatPunishLevel > 0)
        m_nAntiCheatPunishLevel--;

    m_dwJigsawTimeoutTime = 0;

    nRewardExp = m_nLevel * g_GWConfig.nAntiCheatInterval[m_nAntiCheatLevel] / 300 + 50;

    AddExp(nRewardExp, FALSE, FALSE);

    StartAntiCheat();

    INF("anticheat:	success	<%s>	%d	%d", m_szName, m_nAntiCheatLevel, m_nAntiCheatPunishLevel);

#ifndef _ROBOT
    nRetCode = this->ErrorCodeNotify(errAntiCheatSuccess, nRewardExp, 0);
    LOG_CHECK_ERROR(nRetCode);

    //	nRetCode = CCliMessageHandler::GetInstance().DoDLAntiCheatInfo(this, dactAnswer, 0, errSuccess, NULL, NULL);
    //	LOG_CHECK_ERROR(nRetCode);
#endif
Exit0:
    return;
}

INT32 CRole::FailAntiCheat(BOOL bExit, BOOL bIsTimeout)
{
    INT32 nRetCode = 0;
    INT32 nErrCode = 0;

    if (m_nAntiCheatLevel <= 0 && m_nAntiCheatPunishLevel < MAX_ROLE_ANTI_CHEAT_PUNISH_LEVEL - 1)
        m_nAntiCheatPunishLevel++;

    if (m_nAntiCheatLevel > 0)
        m_nAntiCheatLevel--;

    m_dwJigsawTimeoutTime = 0;

    if (!bExit)
    {
        if (m_nAntiCheatPunishLevel > 0)
        {
            if (!bIsTimeout)
            {
#ifndef _ROBOT
                nErrCode = errAntiCheatErrorKick;

                nRetCode = this->ErrorCodeNotify(nErrCode, g_GWConfig.nAntiCheatPunishTime[m_nAntiCheatPunishLevel], 0);
                LOG_CHECK_ERROR(nRetCode);

                if (g_GWConfig.nAntiCheatKickRole)
                {
                    nRetCode = JumpToRevivePos();
                    LOG_CHECK_ERROR(nRetCode);

                    INF("anticheat:	kick	<%s>	%d	%d", m_szName, m_nAntiCheatLevel, m_nAntiCheatPunishLevel);
                    //DBG("freeze login for [%d] minuties.", g_GWConfig.nAntiCheatPunishTime[m_nAntiCheatPunishLevel]);
                    //nRetCode = CCliMessageHandler::GetInstance().DoMSFreezeLogin(this, g_GWConfig.nAntiCheatPunishTime[m_nAntiCheatPunishLevel]);
                    //LOG_CHECK_ERROR(nRetCode);
                    //KICK_ROLE(this, ktImmediately);
                }
#endif
            }
            else
            {
#ifndef _ROBOT
                nErrCode = errAntiCheatErrorTimeoutReturn;

                nRetCode = this->ErrorCodeNotify(nErrCode, 0, 0);
                LOG_CHECK_ERROR(nRetCode);
#endif
                JumpToRevivePos();
            }
        }
        else
        {
#ifndef _ROBOT
            nErrCode = errAntiCheatError;

            nRetCode = this->ErrorCodeNotify(nErrCode, 0, 0);
            LOG_CHECK_ERROR(nRetCode);

            INF("anticheat:	failed	<%s>	%d	%d", m_szName, m_nAntiCheatLevel, m_nAntiCheatPunishLevel);

            //			nRetCode = CCliMessageHandler::GetInstance().DoDLAntiCheatInfo(this, 1, 0, errAntiCheatError, NULL, NULL);
            //			LOG_CHECK_ERROR(nRetCode);
#endif
        }

    }
    else
        nErrCode = errAntiCheatExit;

    StartAntiCheat();

    return nErrCode;
}

void CRole::GenerateAntiCheatJigsaw(void)
{
    INT32 nRetCode = 0;

    JigsawImage ImgData;
    JigsawAnswer AnsData;

    nRetCode = CAntiCheatMgr::GetInstance().GetInstance().GenerateJigsaw(&ImgData, &AnsData);
    LOG_PROCESS_ERROR(nRetCode);

    m_dwJigsawSn++;

    m_nJigsawAnswerIndex = AnsData.nAnswerIdx;

    m_dwJigsawTimeoutTime = CGameWorld::GetInstance().GetGameTime() + g_GWConfig.nAntiCheatTimeout;

    INF("anticheat:	generate	<%s>	%d	%d	%d	%d", m_szName, m_nAntiCheatLevel, m_nAntiCheatPunishLevel, ImgData.nImageIndex, AnsData.nAnswerIdx);

    m_nJigsawRemainTime = 0x7FFFFFFF;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoAntiCheatJigsaw(this, &ImgData, &AnsData, g_GWConfig.nAntiCheatTimeout);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoDLAntiCheatInfo(this, aclaGenerate, -1, errSuccess, &ImgData, &AnsData);
    LOG_CHECK_ERROR(nRetCode);

#endif

Exit0:
    return;
}

void CRole::SetInCutScene(BOOL bInCutScene, INT32 nDurationFrm)
{
    CCharacter::SetInCutScene(bInCutScene, nDurationFrm);

    for (INT32 i = 0; i < m_nPetCount; i++)
    {
        CNpc* poPet = CGameWorld::GetInstance().FindNpc(m_Pet[i].dwNpcID);
        if (poPet)
        {
            poPet->SetInCutScene(bInCutScene, nDurationFrm);
        }
    }
}
