#include "stdafx.h"
#include "character.h"

#include "grid_def.h"
#include "scene_def.h"
#include "npc_data.h"
#include "doodad_data.h"

#include "drop_generator.h"
#include "game_world.h"
#include "message_handler.h"
#include "robot_message_handler.h"
#include "npc_force_mgr.h"
#include "role_faction_mgr.h"
#include "scene_file_mgr.h"
#include "kismet/kismet_node.h"
#include "zone_def.h"
#include "ai_processor.h"
#include "bt_manager.h"
#include "guild_mgr.h"
#include "mail_template_data.h"
#include "init_reputation_data.h"
#include "item_mgr.h"

#define LEAP_DETECTION_HEIGHT   (768 * POINT_PER_CM)
#define MAX_FIND_TIMER			(2 * REGION_WIDTH / GRID_WIDTH)
#define MIN_HP_WHILE_RUNNING	(10)
#define DASH_DISTANCE			(16)
#define MINE_INTERVAL			(GAME_FPS * 5 / 4)

CCharacter::CCharacter()
{
    m_nClass = 0;
    m_nSex = 0;
    m_nLevel = 0;
    m_nFaction = rfrInvalid;
    m_bHpChanged = FALSE;
    m_bMpChanged = FALSE;

    m_nCurrentHp = 0;
    m_nCurrentMp = 0;
    m_nHpTap = 0;
    m_nMpTap = 0;

    m_nTotalDrugCureHp = 0;
    m_nTotalSpellCureHp = 0;
    m_nTotalDrugCureMp = 0;
    m_nCurrentHpLeech = 0;
    m_llNextCureFrm = 0;

    m_nExHp = 0;
    m_nLastRecoveryFrm = 0;
    m_eStance = csfStand;

    m_bInCutScene = FALSE;
    m_nCutSceneEndFrm = 0;

    m_llDestX = m_llDestY = 0;
    m_eMoveState = CMS_INVALID;
    m_nMoveSpeed = 0;
    m_nMoveTimer = 0;
    m_nWalkTimer = 0;
    m_nRunTimer = 0;
    m_nPassiveMoveTimer = 0;
    m_nFearDirection = 0;
    m_bCanRun = FALSE;
    m_nMineTimer = 0;
    m_nMineCDTimer = 0;
    m_nMineValueCount = 0;
    m_nLastMinePos = 0;

    m_nFindState = 0;
    m_nFindTimer = 0;
    m_nFindTimes = 0;
    m_nPathSide = 0;

    m_nVolumeID = -1;
    m_nOriginAiID = 0;
    m_poAIProcessor = NULL;
    m_pStatNode = NULL;

    m_eSpellState = cspsInvalid;
    m_dwFocusID = 0;
    m_dwHostID = 0;
    m_dwExclusiveOwnerID = 0;
    m_dwSeducerID = 0;
    m_dwDamageShareCharID = 0;
    m_dwOperateDoodadID = 0;
    m_poCastingSpell = NULL;
    m_nSubSpellDepth = 0;
#ifdef _ROBOT
    m_nCastingOverFrm = 0;
    m_nReferCount = 0;
#endif
    //m_poPreCastingSpell = NULL;
    m_nSpellSerial = 0;
    m_dwLastCasterID = 0;
    m_dwLastCasterTimer = 0;
    m_dwLastTargetID = 0;
    m_dwLastTargetTimer = 0;
    m_dwDropOwnerID = 0;
    m_eHitTag = sphtagCommon;
    m_dwAuraGenerator = 0;
    m_dwEventGenerator = 0;
    m_nRemainDamageAbsorb = 0;
    m_nDamagTypeBits = 0;
    m_nCounterStiffRate = 0;
    m_nStiffEndFrm = 0;
    m_nNextStiffDecayFrm = 0;

    m_nCounterControlRate = 0;
    m_nNextControlDecayFrm = 0;
    m_nTotalControlCount = 0;

    m_nDamageCount = 0;
    m_nTotalDamage = 0;

    m_nHomeSceneID = 0;
    m_llHomeX = 0;
    m_llHomeY = 0;
    m_llHomeZ = 0;
    m_poPendingItem = NULL;
    m_nPendingItemSlot = 0;
    m_nCurrentMountID = 0;
    m_nCurrentWingID = 0;

    m_nLastHurtFrm = 0;
    m_nNewAiID = 0;
    m_nGlobalCDDelayFrame = 0;

    m_BulletList.Clear();

    memset(m_btControlCount, 0, sizeof(m_btControlCount));
}

CCharacter::~CCharacter()
{
    UnInit();
}

BOOL CCharacter::Init(UINT32 dwObjID)
{
    INT32 nRetCode = 0;

    nRetCode = CSceneObject::Init(dwObjID);
    LOG_PROCESS_ERROR(nRetCode);

    m_AttrList.Init();
    m_ThreatList.Init(this);
    m_AuraList.Init();
    m_oFlag.ResetFlag();
    m_oCCFlag.ResetFlag();
    m_oStiffFlag.ResetFlag();
    m_oExFlag.ResetFlag();
    m_CoolDown.Init();

    m_eMoveState = CMS_STAND;
    m_llDestX = m_llDestY = 0;
    m_nMoveTimer = 0;
    m_nWalkTimer = 0;
    m_nRunTimer = 0;
    m_nPassiveMoveTimer = 0;
    m_nVolumeID = -1;
    m_nLevel = 0;
    m_nFaction = rfrDefault;
    m_bCanRun = FALSE;
    m_nMineTimer = 0;
    m_nMineCDTimer = 0;
    m_nMineValueCount = 0;
    m_nLastMinePos = 0;

    m_nFindState = 0;
    m_nFindTimer = 0;
    m_nFindTimes = 0;
    m_nPathSide = 0;

    m_bInCutScene = FALSE;
    m_nCutSceneEndFrm = 0;

    m_nCurrentHp = 0;
    m_nCurrentMp = 0;
    m_nHpTap = 0;
    m_nMpTap = 0;
    m_nExHp = 0;
    m_nTotalDrugCureHp = 0;
    m_nTotalSpellCureHp = 0;
    m_nTotalDrugCureMp = 0;
    m_nCurrentHpLeech = 0;
    m_llNextCureFrm = 0;

    m_eStance = csfStand;
    m_nCounterStiffRate = 0;
    m_nStiffEndFrm = 0;
    m_nNextStiffDecayFrm = 0;
    m_bHpChanged = FALSE;
    m_bMpChanged = FALSE;
    m_nCounterControlRate = 0;
    m_nNextControlDecayFrm = 0;
    m_nTotalControlCount = 0;

    m_eSpellState = cspsIdle;
    m_eHitTag = sphtagCommon;
    m_dwAuraGenerator = 0;
    m_dwEventGenerator = 0;
    m_nRemainDamageAbsorb = 0;
    m_nDamagTypeBits = 0;
    m_nLastRecoveryFrm = 0;
    m_poCastingSpell = NULL;
    m_nSubSpellDepth = 0;
#ifdef _ROBOT
    m_nCastingOverFrm = 0;
    m_nReferCount = 0;
#endif

    m_dwLastCasterID = 0;
    m_dwLastCasterTimer = 0;
    m_dwLastTargetID = 0;
    m_dwLastTargetTimer = 0;
    m_dwDropOwnerID = 0;
    m_nOriginAiID = 0;
    m_poAIProcessor = NULL;
    m_poPendingItem = NULL;
    m_nPendingItemSlot = 0;
    m_oFlag.SetFlag(cfNotInCombat);
    m_nCurrentMountID = 0;
    m_nCurrentWingID = 0;
    m_pStatNode = NULL;
    m_RemoveEventList.clear();
    m_PendingSpellStack.Clear();
    m_PendingAuraStack.Clear();
    m_PendingEventNodeStack.Clear();
    m_CoolDown.ResetAll();

    m_PendingAuraList.clear();

    m_nLastHurtFrm = 0;

    m_dwFocusID = 0;
    m_dwHostID = 0;
    m_dwExclusiveOwnerID = 0;
    m_dwSeducerID = 0;
    m_dwDamageShareCharID = 0;
    m_dwOperateDoodadID = 0;
    m_nNewAiID = 0;
    m_nGlobalCDDelayFrame = 0;

    memset(m_btControlCount, 0, sizeof(m_btControlCount));

    LOG_PROCESS_ERROR(m_BulletList.IsEmpty());

    //	m_BulletList.Clear();

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CCharacter::UnInit(void)
{
    INT32 nRetCode = 0;

    m_eMoveState = CMS_INVALID;

    if (m_poCastingSpell != NULL)
        CGameWorld::GetInstance().DelSpellInstance(m_poCastingSpell);

    m_poCastingSpell = NULL;
#ifdef _ROBOT
    m_nCastingOverFrm = 0;
    m_nReferCount = 0;
#endif
    m_pStatNode = NULL;

    SetAIProcessor(0);

    m_RegTimerMap.clear();
    m_RemoveEventList.clear();

    m_CoolDown.UnInit();

    BASE_LIST_NODE* pNextNode = NULL;
    for (BASE_LIST_NODE* pNode = m_BulletList.Head().pNext; pNode != &(m_BulletList.Rear()); pNode = pNextNode)
    {
        CBullet* poBullet = node_cast(CBullet*, GetListNode(), pNode);
        pNextNode = pNode->pNext;

        poBullet->UnInit();
    }
    m_BulletList.Clear();

    m_AuraList.UnInit();

    for (INT32 i = 0; i < meoTotal; i++)
    {
        m_RegEventMap[i].clear();
    }

    m_ThreatList.UnInit();

    nRetCode = CSceneObject::UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CCharacter::SetLevel(INT32 nNewLevel)
{
    INT32 nRetCode = 0;

    nRetCode = _ComputeAttribute(nNewLevel);
    LOG_PROCESS_ERROR(nRetCode);

    m_nLevel = nNewLevel;

    return TRUE;
Exit0:
    CRI("failed to set character %d %s level to %d", m_dwObjID, GetName(), nNewLevel);
    return FALSE;
}

INT32 CCharacter::ChangeHp(INT32 nValue, INT32 nReason)
{
    INT32 nMaxHp = 0;
    INT32 nOriginHp = m_nCurrentHp;

    PROCESS_ERROR(nValue != 0);

    nMaxHp = m_AttrList.GetAttr(catMaxHp);
    m_nCurrentHp += nValue;

    if (m_nCurrentHp >= nMaxHp)
    {
        m_nTotalDrugCureHp = 0;
        m_nTotalSpellCureHp = 0;
        m_nCurrentHp = nMaxHp;
    }

    if (nOriginHp != m_nCurrentHp)
        m_bHpChanged = TRUE;

#ifndef _ROBOT

    if (m_nCurrentHp < 0)
    {
        if (m_oFlag.CheckFlag(cfTempUndead))
            m_nExHp = m_nCurrentHp;

        m_nCurrentHp = 0;
    }

    if (m_nCurrentHp <= 0 && !IsDead())
        Die(nReason);
#endif
    return nOriginHp - m_nCurrentHp;
Exit0:
    return 0;
}


void CCharacter::_On1SecMainLoop(void)
{
    INT64 llFrm = CGameWorld::GetInstance().GetGameFrame();

#ifndef _ROBOT
    if (!IsDead())
    {
        m_ThreatList.MainLoop();

        m_nHpTap += m_AttrList.GetAttr(catTapIncrease);
        m_nMpTap += m_AttrList.GetAttr(catTapIncrease);

        _Recovery();
    }
#endif

Exit0:
    return;
}


BOOL CCharacter::MainLoop(void)
{
    INT32 nRetCode = 0;
    INT64 llFrm = CGameWorld::GetInstance().GetGameFrame();

    nRetCode = CSceneObject::MainLoop();
    PROCESS_ERROR(nRetCode);

    if (m_poAIProcessor)
    {
        if (m_nNewAiID != 0 && m_poAIProcessor->GetAIID() != m_nNewAiID)
        {
            SetAIProcessor(m_nNewAiID);
            m_nNewAiID = 0;
            m_poAIProcessor->CallInitBT();
        }
    }

#ifndef _ROBOT
    _UpdateBullet();

    if ((llFrm % GAME_FPS) == (m_dwObjID % GAME_FPS))
        _On1SecMainLoop();
#endif

    if (!IsDead())
    {
        if (m_poAIProcessor && (llFrm % (GAME_FPS / 4)) == (m_dwObjID % (GAME_FPS / 4)))
        {
            nRetCode = m_poAIProcessor->MainLoop();
            LOG_PROCESS_ERROR(nRetCode);
        }

#ifndef _ROBOT
        if (!m_oFlag.CheckFlag(cfTempUndead))
        {
            ChangeHp(m_nExHp, cdrTimeToDie);
            m_nExHp = 0;
        }

        if (m_bInCutScene && llFrm >= m_nCutSceneEndFrm)
            SetInCutScene(FALSE, 0);
#endif	// _ROBOT

        nRetCode = MoveLoop();
        LOG_CHECK_ERROR(nRetCode);

#ifndef _ROBOT
        if (m_nStiffEndFrm > 0 && m_nStiffEndFrm <= CGameWorld::GetInstance().GetGameFrame())
        {
            Stiff(FALSE, 0, 0);
            m_nStiffEndFrm = 0;
        }

        if (m_nNextStiffDecayFrm > 0 && m_nNextStiffDecayFrm <= CGameWorld::GetInstance().GetGameFrame())
        {
            if (m_nCounterStiffRate > 0)
            {
                m_nCounterStiffRate += g_GWConfig.nStiffParamB;
                m_nNextStiffDecayFrm += g_GWConfig.nStiffIntervalB;
            }

            if (m_nCounterStiffRate <= 0)
            {
                m_nCounterStiffRate = 0;
                m_nNextStiffDecayFrm = 0;
            }
        }

        if (m_nNextControlDecayFrm > 0 && m_nNextControlDecayFrm <= CGameWorld::GetInstance().GetGameFrame())
        {
            if (m_nCounterControlRate > 0)
            {
                //				SPL("Start Control Decay");
                m_nCounterControlRate += g_GWConfig.nControlParamB;
                m_nNextControlDecayFrm += g_GWConfig.nControlIntervalB;
            }
            else if (m_nCounterControlRate <= 0)
            {
                //				SPL("Control Decay Over");
                m_nCounterControlRate = 0;
                m_nNextControlDecayFrm = 0;
            }
        }

        _CheckCombat();

        _UpdateEvent();


        if (m_llNextCureFrm < llFrm)
        {
            INT32 nSpellCureHp = 0;
            INT32 nDrugCureHp = 0;
            INT32 nDrugCureMp = 0;

            //INT32 nStdDrugCureHp = m_nLevel / g_GWConfig.nCureParam1 + g_GWConfig.nCureParam2;
            INT32 nStdDrugCureHp = m_nLevel / g_GWConfig.nDrugCureParam1 + g_GWConfig.nDrugCureParam2;
            INT32 nStdDrugCureMp = nStdDrugCureHp;

            m_llNextCureFrm = llFrm + CONVERT_TIME(g_Max((600 - m_nLevel * 10), 250));

            nSpellCureHp = m_nTotalSpellCureHp > g_GWConfig.nHpSpellCure ? g_GWConfig.nHpSpellCure : m_nTotalSpellCureHp;
            ChangeHp(nSpellCureHp, cdrRecovery);
            m_nTotalSpellCureHp -= g_GWConfig.nHpSpellCure;
            m_nTotalSpellCureHp = m_nTotalSpellCureHp >= 0 ? m_nTotalSpellCureHp : 0;

            nDrugCureHp = m_nTotalDrugCureHp > nStdDrugCureHp ? nStdDrugCureHp : m_nTotalDrugCureHp;
            ChangeHp(nDrugCureHp, cdrRecovery);
            m_nTotalDrugCureHp -= nStdDrugCureHp;
            m_nTotalDrugCureHp = m_nTotalDrugCureHp >= 0 ? m_nTotalDrugCureHp : 0;

#ifdef _DEBUG
            if (nDrugCureHp > 0)
                DBG("drug hp cure: %d", nDrugCureHp);
#endif
            nDrugCureMp = m_nTotalDrugCureMp > nStdDrugCureMp ? nStdDrugCureMp : m_nTotalDrugCureMp;
            ChangeMp(nDrugCureMp);
            m_nTotalDrugCureMp -= nStdDrugCureMp;
            m_nTotalDrugCureMp = m_nTotalDrugCureMp >= 0 ? m_nTotalDrugCureMp : 0;
        }
#endif
    }

#ifndef _ROBOT
    _UpdateSpell();
    _UpdateBullet();
#endif

    return TRUE;
Exit0:
    return FALSE;
}

void CCharacter::ChangeTotalSpellHpCure(INT32 nValue)
{
    m_nTotalSpellCureHp += nValue;

    m_nTotalSpellCureHp = m_nTotalSpellCureHp <= g_GWConfig.nHpSpellCureMax ? m_nTotalSpellCureHp : g_GWConfig.nHpSpellCureMax;
}

BOOL CCharacter::MoveLoop(void)
{
    INT32 nRetCode = 0;

    switch (m_eMoveState)
    {
    case CMS_STAND:
        _OnStand();
        break;
    case CMS_SIT:
        _OnSit();
        break;
    case CMS_WALK:
        _OnWalk();
        break;
    case CMS_RUN:
        _OnRun();
        break;
    case CMS_MOVE:
        _OnMove();
        break;
    case CMS_MINE:  // Mining in progress
        _OnMine();
        break;
    case CMS_DASH:
    {
        BOOL bBroken = FALSE;
        _OnDash(bBroken);
    }
    break;
    case CMS_KNOCK_BACK:
        _OnKnockBack();
        break;
    case CMS_THROWN:
        _OnThrown();
        break;
    case CMS_CLIENT_SHOW_TIME:
    case CMS_FREEZE:
        _OnFreeze();
        break;
    case CMS_FEAR:
        _OnFear();
        break;
    case CMS_DIE:
        break;
    case CMS_STALL:
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    if (m_poCastingSpell != NULL && m_poCastingSpell->GetAttr(satMoveBreak) && m_eMoveState != CMS_STAND)
    {
        nRetCode = BreakCast(TRUE, FALSE);
        LOG_CHECK_ERROR(nRetCode);
    }

    if (m_eSpellState == cspsOpening && m_eMoveState != CMS_STAND)
    {
        nRetCode = _CancelOpening();
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CCharacter::Stand(BOOL bForce)
{
    INT32 nRetCode = 0;

    if (!bForce)
    {
        nRetCode = CanActiveMove();
        PROCESS_ERROR(nRetCode);
    }
    LOG_PROCESS_ERROR(m_eMoveState != CMS_DIE);

    m_llX = m_llX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llY = m_llY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llDestX = m_llDestY = 0;
    m_eMoveState = CMS_STAND;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoCharStop(this, bForce);
    LOG_CHECK_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

//BOOL CCharacter::Hard(INT32 nDuration)
//{
//	PROCESS_ERROR(!CheckFlag(cfImmuneStiff));
//
//	m_nHardFrame = CGameWorld::GetInstance().GetGameFrame() + nDuration;
//
//    Stand(FALSE);
//
//#ifndef _ROBOT
//	CCliMessageHandler::GetInstance().DoCharHard(this, nDuration * 1000 / GAME_FPS);
//#endif
//
//	return TRUE;
//Exit0:
//	return FALSE;
//}

BOOL CCharacter::Freeze(BOOL bFreeze)
{
    INT32 nRetCode = 0;

    PROCESS_ERROR(!GetCCFlag().CheckFlag(cclInvincible));
    PROCESS_ERROR(m_eMoveState != CMS_DIE);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoCharFreeze(this, bFreeze);
    LOG_CHECK_ERROR(nRetCode);
#endif

    m_llDestX = m_llDestY = 0;
    if (bFreeze)
        m_eMoveState = CMS_FREEZE;
    else
        m_eMoveState = CMS_STAND;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::ClientShowTime(BOOL bShowTime)
{
    INT32 nRetCode = 0;

    PROCESS_ERROR(m_eMoveState != CMS_DIE);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoCharClientShowTime(this, bShowTime);
    LOG_CHECK_ERROR(nRetCode);
#endif

    m_llDestX = m_llDestY = 0;
    if (bShowTime)
        m_eMoveState = CMS_CLIENT_SHOW_TIME;
    else
        m_eMoveState = CMS_STAND;

    return TRUE;
Exit0:
    return FALSE;

}

BOOL CCharacter::Fear(INT32 nDirection, INT32 nDuration)
{
    LOG_PROCESS_ERROR(_CanPassiveMove());
    LOG_PROCESS_ERROR(!GetCCFlag().CheckFlag(cclInvincible));

    m_nFearDirection = nDirection;
    m_nPassiveMoveTimer = CGameWorld::GetInstance().GetGameFrame() + nDuration;
    m_eMoveState = CMS_FEAR;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::WalkTo(INT64 llDestX, INT64 llDestY)
{
    PROCESS_ERROR(CanActiveMove());
    PROCESS_ERROR(m_nWalkTimer < CGameWorld::GetInstance().GetGameFrame());
    LOG_PROCESS_ERROR(m_llDestX == 0 && m_llDestY == 0);

    m_llDestX = llDestX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llDestY = llDestY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_nMoveSpeed = m_AttrList.GetAttr(catWalkSpeed);
    m_eMoveState = CMS_WALK;

    _OnWalk();

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CCharacter::RunTo(INT64 llDestX, INT64 llDestY)
{
    PROCESS_ERROR(CanActiveMove());
    PROCESS_ERROR(m_nCurrentHp >= MIN_HP_WHILE_RUNNING);  // Cannot run below 10 HP
    PROCESS_ERROR(!GetCCFlag().CheckFlag(cclSlowed));
    PROCESS_ERROR(m_nRunTimer < CGameWorld::GetInstance().GetGameFrame());
    LOG_PROCESS_ERROR(m_llDestX == 0 && m_llDestY == 0);

    m_llDestX = llDestX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llDestY = llDestY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_nMoveSpeed = m_AttrList.GetAttr(catRunSpeed);
    m_eMoveState = CMS_RUN;

    _OnRun();

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CCharacter::MoveTo(INT64 llDestX, INT64 llDestY)
{
    PROCESS_ERROR(CanActiveMove());

    m_llDestX = llDestX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llDestY = llDestY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_eMoveState = CMS_MOVE;

    m_nFindState = 0;
    m_nFindTimer = 0;
    m_nFindTimes = 0;

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CCharacter::Mine(INT64 llDestX, INT64 llDestY)
{
    INT32 nRetCode = 0;
    INT32 nMineInterval = 0;

    PROCESS_ERROR(CanActiveMove());

    LOG_PROCESS_ERROR(abs(llDestX - m_llX) <= GRID_WIDTH && abs(llDestY - m_llY) <= GRID_WIDTH);

    m_llDestX = llDestX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llDestY = llDestY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_eMoveState = CMS_MINE;

    // Calculate mining interval
    nMineInterval = g_GWConfig.nMineInterval - g_Min(813, g_Min(375, m_nLevel * 14) + 60 * m_AttrList.GetAttr(catAtkSpeed)) +
        m_AttrList.GetAttr(catExtraHardTime);

    m_nMineTimer = CGameWorld::GetInstance().GetGameFrame() + CONVERT_TIME(nMineInterval);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoCharMine(this, nMineInterval);
    LOG_CHECK_ERROR(nRetCode);
#endif	// _ROBOT

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CCharacter::Dash(INT32 nDuration, INT64 llDestX, INT64 llDestY)
{
    BOOL bBroken = FALSE;
    PROCESS_ERROR(_CanPassiveMove());
    PROCESS_ERROR(llDestX != m_llX || llDestY != m_llY);

    m_llDestX = llDestX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llDestY = llDestY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_nMoveSpeed = nDuration;
    m_eMoveState = CMS_DASH;

    _OnDash(bBroken);
    PROCESS_ERROR(!bBroken);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::KnockBack(INT32 nDuration, INT64 llDestX, INT64 llDestY)
{
    INT32 nRetCode = 0;
    INT32 nTotalStep = 0;
    INT32 nCurStep = 0;

    PROCESS_ERROR(_CanPassiveMove());
    PROCESS_ERROR(!GetFlag().CheckFlag(cfImmunePassiveMove));
    PROCESS_ERROR(llDestX != m_llX || llDestY != m_llY);

    m_llDestX = llDestX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llDestY = llDestY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_nMoveSpeed = nDuration;
    m_eMoveState = CMS_KNOCK_BACK;

    if (m_llDestX != 0 && m_llDestY != 0)
    {
        INT32 nDestDirection8 = ConvertToDir8(g_GetDirection(m_llX, m_llY, m_llDestX, m_llDestY));
        SetFaceDirection((nDestDirection8 + 4) * (MAX_DIRECTION_COUNT / DT_DIRECTION_8_COUNT) % MAX_DIRECTION_COUNT);

        nTotalStep = g_Max(g_Abs(m_llDestX - m_llX), g_Abs(m_llDestY - m_llY)) / GRID_WIDTH;

        for (INT32 nIndex = 0; nIndex < DASH_DISTANCE; nIndex++)
        {
            if (m_llX == m_llDestX && m_llY == m_llDestY)
                break;

            INT64 llDestX = m_llX + g_nAdjacentX[nDestDirection8] * GRID_WIDTH;
            INT64 llDestY = m_llY + g_nAdjacentY[nDestDirection8] * GRID_WIDTH;

            nRetCode = _GridMove(llDestX, llDestY);
            if (!nRetCode || m_eMoveState != CMS_KNOCK_BACK)
                break;

            nCurStep++;
        }

        nDuration = nTotalStep <= 0 ? 0 : m_nMoveSpeed * nCurStep / nTotalStep;
        nDuration *= nCurStep;

        m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + nDuration;
        m_llDestX = m_llDestY = 0;

#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoCharPassiveMove(this, nDuration);
        LOG_CHECK_ERROR(nRetCode);
#endif //_ROBOT
    }

#ifndef _ROBOT
    CheckMissileTag();
#endif	// _ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::Thrown(INT32 nDuration, INT64 llDestX, INT64 llDestY)
{
    INT32 nRetCode = 0;

    PROCESS_ERROR(_CanPassiveMove());
    PROCESS_ERROR(!GetFlag().CheckFlag(cfImmunePassiveMove));

    m_llDestX = llDestX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_llDestY = llDestY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
    m_nMoveSpeed = nDuration;
    m_eMoveState = CMS_THROWN;

    if (m_llDestX != 0 && m_llDestY != 0)
    {
        INT32 nDestDirection8 = ConvertToDir8(g_GetDirection(m_llX, m_llY, m_llDestX, m_llDestY));
        SetFaceDirection((nDestDirection8 + 4) * (MAX_DIRECTION_COUNT / DT_DIRECTION_8_COUNT) % MAX_DIRECTION_COUNT);

        for (INT32 nIndex = 0; nIndex < DASH_DISTANCE; nIndex++)
        {
            if (m_llX == m_llDestX && m_llY == m_llDestY)
                break;

            INT64 llDestX = m_llX + g_nAdjacentX[nDestDirection8] * GRID_WIDTH;
            INT64 llDestY = m_llY + g_nAdjacentY[nDestDirection8] * GRID_WIDTH;

            nRetCode = _GridMove(llDestX, llDestY);
            if (!nRetCode || m_eMoveState != CMS_THROWN)
                break;
        }

        m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + m_nMoveSpeed;
        m_llDestX = m_llDestY = 0;

#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoCharPassiveMove(this, m_nMoveSpeed);
        LOG_CHECK_ERROR(nRetCode);
#endif
    }

#ifndef _ROBOT
    CheckMissileTag();
#endif	// _ROBOT

    return TRUE;
Exit0:
    return FALSE;

}


BOOL CCharacter::Die(INT32 nParam)
{
    INT32 nRetCode = 0;
    INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

#ifndef _ROBOT
    if (!m_oFlag.CheckFlag(cfTempUndead))
        TrigEvent(meoOnDying, NULL);

    LOG_PROCESS_ERROR(m_eMoveState != CMS_DIE);
    PROCESS_ERROR(!m_oFlag.CheckFlag(cfTempUndead));

    BreakCast(TRUE, FALSE);

    m_nCurrentHp = 0;
    m_oStiffFlag.ResetFlag();
#endif

    ClearGridCharRef();
    m_ThreatList.Clear();    // Clear threat list
    m_eMoveState = CMS_DIE;

#ifndef _ROBOT
    _LeaveCombat();    // Leave combat

    if (m_poAIProcessor != NULL)
        m_poAIProcessor->SetTargetID(0);

    // Send death packet
    nRetCode = CCliMessageHandler::GetInstance().DoCharDie(this, nParam);
    LOG_CHECK_ERROR(nRetCode);

    _ClearAfterDie();
#endif  // _ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

void CCharacter::SetHome(INT32 nSceneID, INT64 llHomeX, INT64 llHomeY, INT64 llHomeZ)
{
    m_nHomeSceneID = nSceneID;
    m_llHomeX = llHomeX;
    m_llHomeY = llHomeY;
    m_llHomeZ = llHomeZ;
    if (m_poAIProcessor)
        m_poAIProcessor->ClearWanderPoint();

    //INF("char %s SetHome, m_poAIProcessor= %p", GetName(), m_poAIProcessor);
}

void CCharacter::SetLastCasterID(UINT32 dwID)
{
    m_dwLastCasterID = dwID;
    m_dwLastCasterTimer = CGameWorld::GetInstance().GetGameTime();
}

void CCharacter::SetLastTargetID(UINT32 dwObjID)
{
    m_dwLastTargetID = dwObjID;
    m_dwLastTargetTimer = CGameWorld::GetInstance().GetGameTime();
}

void CCharacter::IncMoveDelay(INT32 nDelayFrame)
{
    if (m_nMoveTimer < CGameWorld::GetInstance().GetGameFrame())
        m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + nDelayFrame;
    else
        m_nMoveTimer += nDelayFrame;
}

BOOL CCharacter::CanActiveMove(void)
{
    LOG_PROCESS_ERROR(m_pcGrid);
    PROCESS_ERROR(!m_bInCutScene);
    PROCESS_ERROR(m_eMoveState >= CMS_ACTIVE_MOVE && m_eMoveState < CMS_PASSIVE_MOVE);
    PROCESS_ERROR(m_oStiffFlag.GetFlag() == 0);
    PROCESS_ERROR(!m_oCCFlag.CheckFlag(cclStun));

    if (m_poCastingSpell != NULL)
    {
        if (m_eSpellState >= cspsCastingBegin && m_eSpellState < cspsCastingEnd)
            PROCESS_ERROR(m_poCastingSpell->GetAttr(satCanMoveInPrepare));
        //		else if (m_eSpellState == cspsHard)
        //			PROCESS_ERROR(m_poCastingSpell->GetAttr(satCanMoveInHardTime));
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::_CanPassiveMove(void)
{
    LOG_PROCESS_ERROR(m_pcGrid);

    PROCESS_ERROR(!m_bInCutScene);
    PROCESS_ERROR(m_eMoveState >= CMS_ACTIVE_MOVE && m_eMoveState < CMS_PASSIVE_MOVE);

    return TRUE;
Exit0:
    return FALSE;
}

void CCharacter::_OnWalk(void)
{
    INT32 nRetCode = 0;

    if (m_llDestX != 0 && m_llDestY != 0 && m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        INT32 nDestDirection8 = ConvertToDir8(g_GetDirection(m_llX, m_llY, m_llDestX, m_llDestY));
        SetFaceDirection(nDestDirection8 * (MAX_DIRECTION_COUNT / DT_DIRECTION_8_COUNT));

        // Calculate destination coordinates in the facing direction
        INT64 llDestX = m_llX + g_nAdjacentX[nDestDirection8] * GRID_WIDTH;
        INT64 llDestY = m_llY + g_nAdjacentY[nDestDirection8] * GRID_WIDTH;

        nRetCode = _GridMove(llDestX, llDestY);
        PROCESS_ERROR(nRetCode);

        m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + m_nMoveSpeed;
        m_llDestX = m_llDestY = 0;

        m_nHpTap += g_GWConfig.nHpTapWalk;
        m_nMpTap += g_GWConfig.nMpTapWalk;

#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoCharWalk(this);
        LOG_CHECK_ERROR(nRetCode);

        CheckMissileTag();
#endif	// _ROBOT
    }

    if (m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        m_llDestX = m_llDestY = 0;
        m_eMoveState = CMS_STAND;
    }

    return;
Exit0:
    Stand(TRUE);

    return;
}

void CCharacter::_OnRun(void)
{
    INT32 nRetCode = 0;

    if (m_llDestX != 0 && m_llDestY != 0 && m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        INT32 nDestDirection8 = ConvertToDir8(g_GetDirection(m_llX, m_llY, m_llDestX, m_llDestY));
        SetFaceDirection(nDestDirection8 * (MAX_DIRECTION_COUNT / DT_DIRECTION_8_COUNT));

        INT64 llDestX = m_llX + g_nAdjacentX[nDestDirection8] * GRID_WIDTH;
        INT64 llDestY = m_llY + g_nAdjacentY[nDestDirection8] * GRID_WIDTH;

        nRetCode = _GridMove(llDestX, llDestY);  // The function updates m_llX and m_llY
        PROCESS_ERROR(nRetCode);
        PROCESS_ERROR(m_eMoveState == CMS_RUN);

        llDestX = m_llX + g_nAdjacentX[nDestDirection8] * GRID_WIDTH;  // Take another step toward the target
        llDestY = m_llY + g_nAdjacentY[nDestDirection8] * GRID_WIDTH;

        _GridMove(llDestX, llDestY);

        m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + m_nMoveSpeed;
        m_llDestX = m_llDestY = 0;

        m_nHpTap += g_GWConfig.nHpTapRun;
        m_nMpTap += g_GWConfig.nMpTapRun;

#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoCharRun(this);
        LOG_CHECK_ERROR(nRetCode);

        CheckMissileTag();  // Check whether a trap was stepped on
#endif	// _ROBOT
    }

    if (m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        m_llDestX = m_llDestY = 0;
        m_eMoveState = CMS_STAND;
    }

    return;
Exit0:
    Stand(TRUE);

    return;
}

void CCharacter::_OnMove(void)
{
    INT32 nRetCode = 0;
    INT64 llDestX;
    INT64 llDestY;
    INT32 nDestDirection8, nTryDirection8;
    BOOL bRun = FALSE;

    if (m_nMoveTimer > CGameWorld::GetInstance().GetGameFrame())
        return;

    if (m_llX / GRID_WIDTH == m_llDestX / GRID_WIDTH && m_llY / GRID_WIDTH == m_llDestY / GRID_WIDTH)
    {
        m_nFindTimer = 0;
        m_nFindTimes = 0;
        m_nFindState = 0;
        m_llDestX = m_llDestY = 0;
        m_eMoveState = CMS_STAND;

        return;
    }
    else
    {
        nDestDirection8 = ConvertToDir8(g_GetDirection(m_llX, m_llY, m_llDestX, m_llDestY));
        nTryDirection8 = nDestDirection8;

        llDestX = m_llX + g_nAdjacentX[nTryDirection8] * GRID_WIDTH;
        llDestY = m_llY + g_nAdjacentY[nTryDirection8] * GRID_WIDTH;

        nRetCode = _GridMove(llDestX, llDestY);
        if (nRetCode && m_bCanRun && m_eMoveState == CMS_MOVE)
        {
            INT32 nNextDestDirection8 = ConvertToDir8(g_GetDirection(m_llX, m_llY, m_llDestX, m_llDestY));
            INT32 nDirectionDiff = (nNextDestDirection8 + DT_DIRECTION_8_COUNT - nDestDirection8) % DT_DIRECTION_8_COUNT;

            if (nDirectionDiff <= 1)
            {
                llDestX = m_llX + g_nAdjacentX[nTryDirection8] * GRID_WIDTH;
                llDestY = m_llY + g_nAdjacentY[nTryDirection8] * GRID_WIDTH;

                nRetCode = _GridMove(llDestX, llDestY);
                if (nRetCode)
                    bRun = TRUE;
            }

            goto Exit1;
        }
        else
        {
            PROCESS_SUCCESS(nRetCode);
        }

        if (llDestX == m_llDestX && llDestY == m_llDestY)
        {
            m_nFindTimer = 0;
            m_nFindTimes = 0;
            m_nFindState = 0;
            m_llDestX = m_llDestY = 0;
            m_eMoveState = CMS_STAND;

            return;
        }

        if (m_nFindState == 0)
        {
            m_nFindState = 1;
            m_nFindTimer = 0;
            m_nPathSide = 0;
            m_nFindTimes++;

            PROCESS_ERROR(m_nFindTimes <= 1);

            for (int nIndex = 1; nIndex < 4; nIndex++)
            {
                nTryDirection8 = (nDestDirection8 + nIndex) & 0x07;
                llDestX = m_llX + g_nAdjacentX[nTryDirection8] * GRID_WIDTH;
                llDestY = m_llY + g_nAdjacentY[nTryDirection8] * GRID_WIDTH;
                m_nPathSide = 1;

                nRetCode = _GridMove(llDestX, llDestY);
                PROCESS_SUCCESS(nRetCode);

                nTryDirection8 = (nDestDirection8 - nIndex) & 0x07;
                llDestX = m_llX + g_nAdjacentX[nTryDirection8] * GRID_WIDTH;
                llDestY = m_llY + g_nAdjacentY[nTryDirection8] * GRID_WIDTH;
                m_nPathSide = 0;

                nRetCode = _GridMove(llDestX, llDestY);
                PROCESS_SUCCESS(nRetCode);
            }
        }
        else
        {
            PROCESS_ERROR(m_nFindTimer <= MAX_FIND_TIMER);
            m_nFindTimer++;

            if (m_nPathSide == 1)
            {
                for (INT32 nIndex = 1; nIndex < 4; nIndex++)
                {
                    nTryDirection8 = (nDestDirection8 + nIndex) & 0x07;
                    llDestX = m_llX + g_nAdjacentX[nTryDirection8] * GRID_WIDTH;
                    llDestY = m_llY + g_nAdjacentY[nTryDirection8] * GRID_WIDTH;

                    nRetCode = _GridMove(llDestX, llDestY);
                    PROCESS_SUCCESS(nRetCode);
                }
            }
            else
            {
                for (INT32 nIndex = 0; nIndex < 4; nIndex++)
                {
                    INT32 nTryDirection8 = (nDestDirection8 - nIndex) & 0x07;
                    llDestX = m_llX + g_nAdjacentX[nTryDirection8] * GRID_WIDTH;
                    llDestY = m_llY + g_nAdjacentY[nTryDirection8] * GRID_WIDTH;

                    nRetCode = _GridMove(llDestX, llDestY);
                    PROCESS_SUCCESS(nRetCode);
                }
            }
        }
    }

Exit0:
    m_nFindTimer = 0;
    m_nFindTimes = 0;
    m_nFindState = 0;
    m_llDestX = m_llDestY = 0;
    m_nMoveTimer = 0;
    m_eMoveState = CMS_STAND;

    return;
Exit1:
    SetFaceDirection(nTryDirection8 * (MAX_DIRECTION_COUNT / DT_DIRECTION_8_COUNT));

    if (bRun)
    {
        m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + m_AttrList.GetAttr(catRunSpeed);

#ifdef _ROBOT
        if (GetObjType() == otRole && ((CRole*)this)->IsRobot())
            CRobotMessageHandler::GetInstance().DoRoleRun((CRole*)this);
#else
        CCliMessageHandler::GetInstance().DoCharRun(this);

        CheckMissileTag();
#endif	// _ROBOT
    }
    else
    {
        m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + m_AttrList.GetAttr(catWalkSpeed);

#ifdef _ROBOT
        if (GetObjType() == otRole && ((CRole*)this)->IsRobot())
            CRobotMessageHandler::GetInstance().DoRoleWalk((CRole*)this);
#else
        CCliMessageHandler::GetInstance().DoCharWalk(this);

        CheckMissileTag();
#endif	// _ROBOT
    }

    return;
}

void CCharacter::_OnMine(void)
{
    INT32 nRetCode = 0;
    CRole* poRole = (CRole*)this;
    const GRID_DATA* pcGridData = NULL;
    const ZONE_DATA* pcZoneData = NULL;
    const SCENE_DATA* pcSceneData = NULL;

    if (m_nMineTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        LOG_PROCESS_ERROR(GetObjType() == otRole);
        LOG_PROCESS_ERROR(m_poScene);

#ifndef _ROBOT

        pcSceneData = m_poScene->GetSceneData();
        LOG_PROCESS_ERROR(pcSceneData);

        // Deduct pickaxe durability
        if (pcSceneData->nMineItemID1 > 0)
        {
            // 99900007 - pickaxe
            nRetCode = poRole->GetItemBag()->LoseMineDur(pcSceneData->nMineItemID1);
            if (!nRetCode)
            {
                PROCESS_ERROR(pcSceneData->nMineItemID2 > 0);

                // 99900041 - dark iron pickaxe
                nRetCode = poRole->GetItemBag()->LoseMineDur(pcSceneData->nMineItemID2);
                PROCESS_ERROR(nRetCode);
            }
        }

        // Get data for the current coordinates
        nRetCode = m_poScene->GetLowerGrid(m_llDestX, m_llDestY, m_llZ, pcZoneData, pcGridData);
        LOG_PROCESS_ERROR(nRetCode);

        // Check whether the current coordinates are a mining point
        if (pcGridData && pcGridData->bMine)
        {
            // Check whether this gathering point matches the previous one
            INT32 nMinePos = ((m_llX / GRID_WIDTH) << 16) | (m_llY / GRID_WIDTH);
            if (nMinePos != m_nLastMinePos)
            {
                m_nMineCDTimer = 0;
                m_nMineValueCount = 0;
            }

            if (m_nMineCDTimer <= CGameWorld::GetInstance().GetGameFrame())
            {
                INT32 nRate = g_GWConfig.nGlobalDropRate;
                nRate = nRate * poRole->GetFcmRate() / RATE_MAX;  // Apply anti-addiction restriction, halving the chance
                nRate = nRate * poRole->GetItemBag()->GetChanceByGoldPool() / RATE_MAX;  // Calculate chance using fatigue

                for (INT32 nIndex = 0; nIndex < MAX_MINE_DROP_GROUP; nIndex++)
                {
                    // Roll to determine whether something was mined
                    DropContent content = { 0 };
                    nRetCode = CDropGenerator::GetInstance().GenerateDropContentFromDropChanceData(
                        //poRole->GetServerGroup(), pcSceneData->MineDrop + nIndex, content, nRate);
                        poRole->GetServerGroup(), pcSceneData->MineDrop[1] + nIndex, content, nRate);    // For now, always use index [1]
                    if (!nRetCode)
                        continue;

                    // Mine currency
                    if (content.m_type == DropContent::DROP_MONEY)
                    {
                        nRetCode = poRole->GetItemBag()->AddMoney(content.m_money.moneytype, content.m_money.count, MONEYLOG_MINE, 0);
                        if (nRetCode == errSuccess)
                        {
                            m_nMineValueCount += content.m_money.count;
                            if (content.m_money.moneytype == MONEYTYPE_02)
                                poRole->GetItemBag()->SubGoldPool(content.m_money.count);
                        }
                    }
                    // Mine an item
                    else if (content.m_type == DropContent::DROP_ITEM)
                    {
                        nRetCode = poRole->GetItemBag()->AddItem(content.m_item, ITEMLOG_MINE, 0);
                        if (nRetCode == errSuccess)
                        {
                            m_nMineValueCount += content.m_item->m_pTemplate->m_nSellPrice;
                            if (content.m_item->m_pTemplate->m_nSellPrice > 0)
                                poRole->GetItemBag()->SubGoldPool(content.m_item->m_pTemplate->m_nSellPrice);
                        }
                        else
                        {
                            BOOL bItemRet = CItemMgr::GetInstance().DestroyItem(content.m_item);
                            LOG_CHECK_ERROR(bItemRet);
                            poRole->ErrorCodeNotify(nRetCode, 0, 0);
                        }
                    }
                }

                if (m_nMineValueCount >= g_GWConfig.nMineMaxValue)
                {
                    m_nMineValueCount = 0;
                    m_nMineCDTimer = CGameWorld::GetInstance().GetGameFrame() + CONVERT_TIME(g_GWConfig.nMineCDTime);
                }

                m_nLastMinePos = nMinePos;
            }

            //if (g_Random(RATE_MAX) < pcSceneData->nMineDoodadChance)
            if (g_Random(RATE_MAX) < pcSceneData->nMineDoodadChance[1])    // For now, always use index [1]
            {
                //CDoodad* poDoodad = m_poScene->SpawnDoodad(pcSceneData->nMineDoodadID, m_llX, m_llY, m_llZ, m_nFaceDirection, NULL);
                CDoodad* poDoodad = m_poScene->SpawnDoodad(pcSceneData->nMineDoodadID[1], m_llX, m_llY, m_llZ, m_nFaceDirection, NULL);    // For now, always use index [1]
                LOG_CHECK_ERROR(poDoodad);
            }
        }
#endif	// _ROBOT

        m_llDestX = m_llDestY = 0;
        m_eMoveState = CMS_STAND;
    }

    return;
Exit0:
    Stand(FALSE);

    return;
}

void CCharacter::_OnDash(BOOL& bBroken)
{
    INT32 nRetCode = 0;
    INT32 nTotalStep = 0;
    INT32 nCurStep = 0;
    INT32 nDuration = 0;
    bBroken = FALSE;

    if (m_llDestX != 0 && m_llDestY != 0 && m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        INT32 nDestDirection8 = ConvertToDir8(g_GetDirection(m_llX, m_llY, m_llDestX, m_llDestY));

        if (m_llDestX != m_llX || m_llDestY != m_llY)
        {
            SetFaceDirection(nDestDirection8 * (MAX_DIRECTION_COUNT / DT_DIRECTION_8_COUNT));
        }

        nTotalStep = g_Max(g_Abs(m_llDestX - m_llX), g_Abs(m_llDestY - m_llY)) / GRID_WIDTH;

        for (INT32 nIndex = 0; nIndex < DASH_DISTANCE; nIndex++)
        {
            if (m_llX == m_llDestX && m_llY == m_llDestY)
                break;

            INT64 llDestX = m_llX + g_nAdjacentX[nDestDirection8] * GRID_WIDTH;
            INT64 llDestY = m_llY + g_nAdjacentY[nDestDirection8] * GRID_WIDTH;

            nRetCode = _GridMove(llDestX, llDestY);
            if (!nRetCode || m_eMoveState != CMS_DASH)
            {
                bBroken = TRUE;
                break;
            }

            nCurStep++;
        }

        nDuration = nTotalStep <= 0 ? 0 : m_nMoveSpeed * nCurStep / nTotalStep;
        nDuration *= nCurStep;

        m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + nDuration;
        m_llDestX = m_llDestY = 0;

#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoCharPassiveMove(this, nDuration);
        LOG_CHECK_ERROR(nRetCode);

        CheckMissileTag();
#endif	// _ROBOT
    }

    if (m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        m_llDestX = m_llDestY = 0;
        m_eMoveState = CMS_STAND;
    }

Exit0:
    return;
}

void CCharacter::_OnKnockBack(void)
{
    if (m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        m_llDestX = m_llDestY = 0;
        m_eMoveState = CMS_STAND;
    }

Exit0:
    return;
}

void CCharacter::_OnThrown(void)
{
    if (m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        m_llDestX = m_llDestY = 0;
        m_eMoveState = CMS_STAND;
    }

Exit0:
    return;
}

void CCharacter::_OnStand(void)
{
    //if (m_pcGrid)
    //	m_llZ = m_pcGrid->uHighLayer * GRID_LAYER_HEIGHT;

    return;
}

void CCharacter::_OnSit(void)
{
    return;
}

void CCharacter::_OnFreeze(void)
{
    return;
}

void CCharacter::_OnFear(void)
{
    INT32 nRetCode = 0;

    if (m_nPassiveMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
    {
        if (m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
        {
            INT32 nDirection = (m_nFearDirection + g_Random(3) + 7) % DT_DIRECTION_8_COUNT;
            INT32 nDestX = m_llX + g_nAdjacentX[nDirection] * GRID_WIDTH;
            INT32 nDestY = m_llY + g_nAdjacentY[nDirection] * GRID_WIDTH;

            nRetCode = _GridMove(nDestX, nDestY);
            if (nRetCode)
            {
                m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + m_nMoveSpeed;

#ifndef _ROBOT
                nRetCode = CCliMessageHandler::GetInstance().DoCharWalk(this);
                LOG_CHECK_ERROR(nRetCode);
#endif
            }
        }
    }
    else
    {
        nRetCode = Stand(TRUE);
    }

    return;
}

BOOL CCharacter::PlayAction(UINT32 dwActionID)
{
    INT32 nRetCode = 0;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoPlayAction(this, dwActionID);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

void CCharacter::SetInCutScene(BOOL bInCutScene, INT32 nDurationFrm)
{
    if (bInCutScene)
    {
        PROCESS_ERROR(!m_bInCutScene);
        IncControlCount(cclInvincible);
        m_nCutSceneEndFrm = CGameWorld::GetInstance().GetGameFrame() + nDurationFrm;
    }
    else
    {
        PROCESS_ERROR(m_bInCutScene);
        DecControlCount(cclInvincible);
    }

    m_bInCutScene = bInCutScene;

Exit0:
    return;
}



BOOL CCharacter::Revive(INT32 nParam, INT32 nHpPercent)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(m_eMoveState == CMS_DIE);

    SetHp((INT64)nHpPercent * GetAttr(catMaxHp) / RATE_MAX);

    _InitMp();

    if (!CheckGridCharRef(m_nBodyType))
    {
#ifndef _ROBOT
        nRetCode = JumpTo(m_llX, m_llY, m_llZ, m_nFaceDirection);
        LOG_PROCESS_ERROR(nRetCode);
#endif // _ROBOT
    }

    nRetCode = SetGridCharRef();
    LOG_CHECK_ERROR(nRetCode);

    m_llDestX = m_llDestY = 0;
    m_eMoveState = CMS_STAND;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoCharRevive(this, nParam);
    LOG_CHECK_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::SetAIProcessor(INT32 nAIID)
{
    INT32 nRetCode = FALSE;

    if (nAIID == 0)
    {
        if (m_poAIProcessor)
        {
            m_poAIProcessor->UnInit();
            CGameWorld::GetInstance().DelAIProcessor(m_poAIProcessor);
            m_poAIProcessor = NULL;
        }
    }
    else
    {
        if (m_poAIProcessor == NULL)
        {
            m_poAIProcessor = CGameWorld::GetInstance().NewAIProcessor();
            LOG_PROCESS_ERROR(m_poAIProcessor);
        }
        else
        {
            PROCESS_SUCCESS(m_poAIProcessor->GetAIID() == nAIID);

            nRetCode = m_poAIProcessor->UnInit();
            LOG_CHECK_ERROR(nRetCode);
        }

        nRetCode = m_poAIProcessor->Init(this, nAIID);
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    return TRUE;
Exit0:
    if (m_poAIProcessor)
    {
        m_poAIProcessor->UnInit();
        CGameWorld::GetInstance().DelAIProcessor(m_poAIProcessor);
        m_poAIProcessor = NULL;
    }
    return FALSE;
}

CCharacter::_MpRecoveryFunc CCharacter::m_MpRecoveryFunc[] =
{
    &CCharacter::_MpRecoveryWarrior,
    &CCharacter::_MpRecoveryMage,
    &CCharacter::_MpRecoveryAssassin,
    &CCharacter::_MpRecoveryArcher,
    NULL,
};

INT32 CCharacter::_MpRecoveryWarrior(INT32 nPeriod)
{
    return 0;
    //	return GetAttr(catMpRecovery);
}

INT32 CCharacter::_MpRecoveryMage(INT32 nPeriod)
{
    return 0;
    //	return GetAttr(catMpRecovery);
}

INT32 CCharacter::_MpRecoveryAssassin(INT32 nPeriod)
{
    //	INT32 nRecovery = 0;
    //	INT32 nMp = GetMp();
    //	INT32 nHalfMaxMp = GetAttr(catMaxMp)/2;
    //
    //	nRecovery = GetAttr(catMpRecovery);
    //
    //	if(nMp < nHalfMaxMp)
    //	{
    //		if(nMp + nRecovery > nHalfMaxMp)
    //			nRecovery = nHalfMaxMp - nMp;
    //
    //		return nRecovery;
    //	}
    //
    //	else if(nMp > nHalfMaxMp)
    //	{
    //		if(nMp - nRecovery < nHalfMaxMp)
    //			nRecovery = nMp - nHalfMaxMp;
    //
    //		return -nRecovery;
    //	}

    return 0;
}

INT32 CCharacter::_MpRecoveryArcher(INT32 nPeriod)
{
    return 0;
    //	return GetAttr(catMaxMp) * GetAttr(catMpRecoveryRate) / RATE_MAX;
}

BOOL CCharacter::CanAttack(CCharacter* poChar)
{
    INT32 nRetCode = 0;
    CCharacter* poAttackChar = this;
    CCharacter* poDefendChar = poChar;
    CCharacter* poFindChar = NULL;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(m_dwObjID != poChar->GetObjID());

    if (GetObjType() == otRole)
    {
        CRole* poRole = (CRole*)this;
        if (poRole->GetPkMode() == rpmSlayer && poChar->GetHostID() == m_dwObjID)
            return TRUE;
    }

    if (GetCCFlag().CheckFlag(cclSeduced))
    {
        if (m_dwSeducerID != INVALID_OBJECT_ID)
        {
            poFindChar = CGameWorld::GetInstance().FindValidCharacter(m_dwSeducerID);
            if (poFindChar)
                poAttackChar = poFindChar;
        }
    }

    if (GetCCFlag().CheckFlag(cclFury))
    {
        return TRUE;
    }

    if (m_dwHostID != INVALID_OBJECT_ID)
    {
        poFindChar = CGameWorld::GetInstance().FindCharacter(m_dwHostID);
        if (poFindChar)
            poAttackChar = poFindChar;
    }

    if (poChar->GetHostID() != INVALID_OBJECT_ID)
    {
        poFindChar = CGameWorld::GetInstance().FindCharacter(poChar->GetHostID());
        if (poFindChar)
            poDefendChar = poFindChar;
    }

    // self
    PROCESS_ERROR(poAttackChar != poDefendChar);

    if (poAttackChar->GetObjType() == otRole)
    {
        CRole* poAttackRole = (CRole*)poAttackChar;

        if (poDefendChar->GetObjType() == otRole)
        {
            CRole* poDefendRole = (CRole*)poDefendChar;
            BOOL bSameTeam = TRUE;
            BOOL bSameGuild = FALSE;
            INT32 nFactionType;

            // level
            if (poAttackChar->GetScene() && poAttackChar->GetScene()->GetSceneTempID() == 142)
                PROCESS_ERROR(poAttackRole->GetLevel() > g_GWConfig.nPkProtectLevel &&
                    poDefendRole->GetLevel() > g_GWConfig.nPkProtectLevel);

            // grid
            LOG_PROCESS_ERROR(GetGrid());
            PROCESS_ERROR(GetGrid()->uPkType != gptSafe);

            LOG_PROCESS_ERROR(poChar->GetGrid());
            PROCESS_ERROR(poChar->GetGrid()->uPkType != gptSafe);

            if (GetGrid()->bSlayerArea && poChar->GetGrid()->bSlayerArea)
                return TRUE;

            switch (poAttackRole->GetPkMode())
            {
            case rpmPeace:
                return FALSE;
            case rpmTeam:
                if (poAttackRole->GetTeamID() == 0 || poDefendRole->GetTeamID() == 0 ||
                    (poAttackRole->GetTeamID() != poDefendRole->GetTeamID()))
                    return TRUE;
                else
                    return FALSE;
            case rpmGuild:
                if (poAttackRole->GetGuildID() != 0 && poDefendRole->GetGuildID() != 0 &&
                    (poAttackRole->GetGuildID() == poDefendRole->GetGuildID() ||
                        CGuildMgr::GetInstance().IsFriendly(poAttackRole->GetGuildID(), poDefendRole->GetGuildID())))
                    return FALSE;
                else
                    return TRUE;
            case rpmSlayer:
                return TRUE;
            case rpmJustice:
                if (poDefendRole->IsRedName() || poDefendRole->IsGreyName())
                    return TRUE;
                else
                    return FALSE;
            case rpmMaster:
                if (poAttackRole->IsMasterRelationship(poDefendRole))
                    return FALSE;
                else
                    return TRUE;
            default:
                LOG_PROCESS_ERROR(FALSE);
            }
        }
        else if (poDefendChar->GetObjType() == otNpc)
        {
            INT32 nReputation = 0;
            CNpc* poDefendNpc = (CNpc*)poDefendChar;

            const NPC_DATA* pData = poDefendNpc->GetNpcData();
            LOG_PROCESS_ERROR(pData);

            if (pData->bExclusive)
                PROCESS_ERROR(poDefendNpc->GetExclusiveOwnerID() == m_dwObjID);

            const INIT_REPUTATION_DATA* pReputationData = CDataManager<INIT_REPUTATION_DATA>::GetInstance().GetData(pData->nForceID);
            LOG_PROCESS_ERROR(pReputationData);

            if (pReputationData->bCanAttackWithFreePkMode)
            {
                if (poAttackRole->GetPkMode() == rpmSlayer)
                    return TRUE;
                else
                    return FALSE;
            }
            else if (pReputationData->bSabacMode)
            {
                if (poAttackRole->IsSabacGuildMember())
                {
                    if (poAttackRole->GetPkMode() == rpmSlayer)
                        return TRUE;
                    else
                        return FALSE;
                }
            }

            nReputation = poAttackRole->GetReputation().GetValue(pData->nForceID);
            PROCESS_ERROR(nReputation < 0);

            return TRUE;
        }
        else
        {
            LOG_PROCESS_ERROR(FALSE);
        }
    }
    else if (poAttackChar->GetObjType() == otNpc)
    {
        CNpc* poAttackNpc = (CNpc*)poAttackChar;

        if (poDefendChar->GetObjType() == otRole)
        {
            INT32 nReputation = 0;
            CRole* poDefendRole = (CRole*)poDefendChar;

            const NPC_DATA* pData = poAttackNpc->GetNpcData();
            LOG_PROCESS_ERROR(pData);

            if (pData->bExclusive)
                PROCESS_ERROR(m_dwExclusiveOwnerID == poDefendRole->GetObjID());

            nReputation = poDefendRole->GetReputation().GetValue(pData->nForceID);
            PROCESS_ERROR(nReputation < 0);

            return TRUE;
        }
        else if (poDefendChar->GetObjType() == otNpc)
        {
            CNpc* poDefendNpc = (CNpc*)poDefendChar;

            LOG_PROCESS_ERROR(poAttackNpc->GetNpcData());
            LOG_PROCESS_ERROR(poDefendNpc->GetNpcData());

            nRetCode = CNpcForceMgr::GetInstance().GetRelation(poAttackNpc->GetNpcData()->nForceID, poDefendNpc->GetNpcData()->nForceID);
            PROCESS_ERROR(nRetCode < 0);

            return TRUE;
        }
        else
        {
            LOG_PROCESS_ERROR(FALSE);
        }
    }
    else
    {
        LOG_PROCESS_ERROR(FALSE);
    }

Exit0:
    return FALSE;
}

INT32 CCharacter::Say(UINT32 dwTargetID, UINT32 dwLocation, const char* pcszMessage)
{
    INT32  nResult = errUnknownError;
    INT32  nRetCode = 0;
    INT32  nMessageSize = 0;
    BOOL bGM = FALSE;

    LOG_PROCESS_ERROR(pcszMessage);
    nMessageSize = (INT32)strnlen(pcszMessage, MAX_CHAT_MESSAGE_LEN) + 1;
    LOG_PROCESS_ERROR(pcszMessage[nMessageSize - 1] == '\0');
    LOG_PROCESS_ERROR((dwLocation > cltInvalid) && (dwLocation < cltTotal));

    if (GetObjType() == otRole)
        bGM = ((CRole*)this)->GetGMLevel() > 0;

    switch (GetTypeFromID(dwTargetID))
    {
    case otChannel:
    {
#ifndef _ROBOT
        switch (dwTargetID)
        {
        case cctScene:
        {
            nRetCode = CCliMessageHandler::GetInstance().DoChatMessage(GetScene(), GetName(), GetObjID(),
                dwTargetID, dwLocation, pcszMessage, nMessageSize);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
        break;
        case cctScreen:
        {
            nRetCode = CCliMessageHandler::GetInstance().DoChatMessage(GetRegion(), GetName(), GetObjID(),
                dwTargetID, dwLocation, pcszMessage, nMessageSize);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
        break;
        case cctServer:
        case cctGroup:
        {
            nRetCode = CCliMessageHandler::GetInstance().DoMSChatMessage(
                GetObjID(), GetName(), dwTargetID, dwLocation, 0, bGM, pcszMessage, nMessageSize);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
        break;
        default:
            break;
        }
#else
        nRetCode = CRobotMessageHandler::GetInstance().DoSendChatToGS((CRole*)this, dwTargetID, pcszMessage);
        LOG_PROCESS_ERROR(nRetCode);
#endif
    }
    break;

    case otRole:
    case otGuild:
    case otTeam:
    {
#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoMSChatMessage(
            GetObjID(), GetName(), dwTargetID, dwLocation, 0, bGM, pcszMessage, nMessageSize);
        LOG_PROCESS_ERROR(nRetCode);
#else
        nRetCode = CRobotMessageHandler::GetInstance().DoSendChatToMS((CRole*)this, dwTargetID, pcszMessage);
        LOG_PROCESS_ERROR(nRetCode);
#endif
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
Exit0:
    return errUnknownError;
}

INT32 CCharacter::Say(UINT32 dwTargetID, UINT32 dwLocation, UINT32 dwStringID)
{
    INT32 nRetCode = 0;
    char cszWords[MAX_CHAT_MESSAGE_LEN];
    const char* pcszMessage = NULL;

    if (dwStringID > SERVER_STRING_START_ID) // server translate
    {
        pcszMessage = CGameWorld::GetInstance().GetString(dwStringID);
        LOG_PROCESS_ERROR(pcszMessage);
    }
    else // client need
    {
        snprintf(cszWords, PATH_LEN, "<S>%u</S>", dwStringID);
        pcszMessage = cszWords;
    }

    return Say(dwTargetID, dwLocation, pcszMessage);
Exit0:
    return errUnknownError;
}

INT32 CCharacter::Say(UINT32 dwTargetID, UINT32 dwLocation, UINT32 dwStringID, INT32 nParam)
{
    INT32 nRetCode = 0;
    char cszWords[MAX_CHAT_MESSAGE_LEN];
    const char* pcszMessage = NULL;

    if (dwStringID > SERVER_STRING_START_ID) // server translate
    {
        pcszMessage = CGameWorld::GetInstance().GetString(dwStringID);
        LOG_PROCESS_ERROR(pcszMessage);
    }
    else // client need
    {
        snprintf(cszWords, PATH_LEN, "<#P0:%d><#T:%d>", nParam, dwStringID);
        pcszMessage = cszWords;
    }

    return Say(dwTargetID, dwLocation, pcszMessage);
Exit0:
    return errUnknownError;

}

BOOL CCharacter::SendMail(UINT32 dwRoleID, INT32 nMailTemplateID)
{
    const MAIL_TEMPLATE_DATA* mail_data = NULL;

    LOG_PROCESS_ERROR(nMailTemplateID);

    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
    LOG_PROCESS_ERROR(mail_data);

#ifndef _ROBOT
    if (mail_data->m_nItemID && mail_data->m_nItemCount)
        return CCliMessageHandler::GetInstance().DoMSSendSystemMailWithSimpleAttach(
            dwRoleID, mail_data->m_szMailSubject, mail_data->m_szMailContent, mail_data->m_nItemID, mail_data->m_nItemCount);
    else
        return CCliMessageHandler::GetInstance().DoMSSendSystemMail(dwRoleID, mail_data->m_szMailSubject, mail_data->m_szMailContent);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::SendMail(UINT32 dwRoleID, INT32 nMailTemplateID, INT32 nItemID, INT32 nItemCount)
{
    const MAIL_TEMPLATE_DATA* mail_data = NULL;

    LOG_PROCESS_ERROR(nMailTemplateID);

    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
    LOG_PROCESS_ERROR(mail_data);

#ifndef _ROBOT
    return CCliMessageHandler::GetInstance().DoMSSendSystemMailWithSimpleAttach(
        dwRoleID, mail_data->m_szMailSubject, mail_data->m_szMailContent, nItemID, nItemCount);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::SendMail(UINT32 dwRoleID, INT32 nMailTemplateID, const ItemInfo& iteminfo)
{
    char attachbuffer[1024];
    UINT32 attachlen = 0;
    const MAIL_TEMPLATE_DATA* mail_data = NULL;

    LOG_PROCESS_ERROR(nMailTemplateID);

    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
    LOG_PROCESS_ERROR(mail_data);

    attachlen = iteminfo.Save(attachbuffer);

#ifndef _ROBOT
    return CCliMessageHandler::GetInstance().DoMSSendSystemMailWithBufferAttach(
        dwRoleID, mail_data->m_szMailSubject, mail_data->m_szMailContent, attachbuffer, attachlen);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

//BOOL CCharacter::SendMail(UINT32 dwRoleID, const char* szMailSubject, const char* szMailContent, INT32 nItemID, INT32 nItemCount)
//{
//#ifndef _ROBOT
//	return CCliMessageHandler::GetInstance().DoMSSendSystemMailWithSimpleAttach(dwRoleID, szMailSubject, szMailContent, nItemID, nItemCount);
//#endif
//}

INT32 CCharacter::OperateDoodad(UINT32 dwDoodadID)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    INT64 llMaxDistance = 0;

    CDoodad* pDoodad = CGameWorld::GetInstance().FindDoodad(dwDoodadID);
    PROCESS_ERROR_RET_CODE(pDoodad, errDoodadNotExist);
    PROCESS_ERROR_RET_CODE(pDoodad->GetRegion(), errDoodadNotExist);
    PROCESS_ERROR_RET_CODE(m_eSpellState == cspsIdle, errDoodadSpellCasting);

    PROCESS_ERROR_RET_CODE(pDoodad->GetScene() == GetScene(), errDoodadOutOfRange);
    llMaxDistance = g_GWConfig.nDoodadOperateDistance * POINT_PER_CM;
    PROCESS_ERROR_RET_CODE(GetDistance3D(pDoodad) < llMaxDistance * llMaxDistance, errDoodadOutOfRange);
    PROCESS_ERROR_RET_CODE(pDoodad->CanOpen(this), errDoodadCannotOperate);

    LOG_PROCESS_ERROR(pDoodad->GetDoodadData());
    if (pDoodad->GetDoodadData()->nOperateDuration <= 0)
    {
        nRetCode = OpenDoodad(pDoodad, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        m_eSpellState = cspsOpening;
        m_dwOperateDoodadID = dwDoodadID;
        INT32 nPrepareFrm = pDoodad->GetDoodadData()->nOperateDuration;
        m_PreparingTimer.Enable(nPrepareFrm, 1, CGameWorld::GetInstance().GetGameFrame(), nPrepareFrm);

#ifndef _ROBOT
        nRetCode = CCliMessageHandler::GetInstance().DoOperateDoodadBegin(this, dwDoodadID, nPrepareFrm);
        LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT
    }

    nResult = errSuccess;
Exit0:
    return nResult;
}

BOOL CCharacter::OpenDoodad(CDoodad* pDoodad, INT32 nDelayFrame)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pDoodad);

    if (pDoodad->CanOpen(this))
    {
        pDoodad->OnOpen(this, nDelayFrame);
    }

    return TRUE;
Exit0:
    if (pDoodad && pDoodad->GetScene())
    {
        nRetCode = pDoodad->GetScene()->RemoveDoodad(pDoodad);
        LOG_CHECK_ERROR(nRetCode);
    }

    if (pDoodad)
    {
        nRetCode = pDoodad->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGameWorld::GetInstance().DelDoodad(pDoodad);
        LOG_CHECK_ERROR(nRetCode);
    }

    return FALSE;
}

BOOL CCharacter::RotateTo(INT32 nFaceDirection, INT32 nDurationFrm)
{
    INT32 nRetCode = 0;

    PROCESS_ERROR(m_eMoveState >= CMS_ACTIVE_MOVE && m_eMoveState < CMS_PASSIVE_MOVE);
    PROCESS_ERROR(m_oStiffFlag.GetFlag() == 0);
    PROCESS_ERROR(!m_oCCFlag.CheckFlag(cclStun));

    nRetCode = CSceneObject::RotateTo(nFaceDirection, nDurationFrm);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::RotateToDir8(INT32 nDirection, INT32 nDurationFrm)
{
    INT32 nRetCode = 0;

    nDirection = ConvertToDir8(nDirection) * MAX_DIRECTION_COUNT / 8;

    nRetCode = RotateTo(nDirection, nDurationFrm);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCharacter::RotateToMirDir8(INT32 nMirDir8, INT32 nDurationFrm)
{
    INT32 nRetCode = 0;

    nRetCode = RotateTo(nMirDir8 * MAX_DIRECTION_COUNT / 8, nDurationFrm);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;

}

INT32 CCharacter::GetWeaponType(void)
{
    return WEAPONTYPE_HAND;
}

BOOL CCharacter::DoSyncAttrChange(INT32 eAttrType)
{
    return FALSE;
}

BOOL CCharacter::_InitMp(void)
{
    //	static INT32 nMaxMp[clsTotal] =
    //	{
    //		0,
    //		RATE_MAX,
    //		RATE_MAX,
    //		RATE_MAX,
    //		0,
    //	};

    //	m_nCurrentMp = nMaxMp[m_nClass] * m_AttrList.GetAttr(catMaxMp) / RATE_MAX;
    m_nCurrentMp = m_AttrList.GetAttr(catMaxMp);

    return TRUE;
}

INT32 CCharacter::GetPow(void)
{
    return 0;
}

BOOL CCharacter::Transform(BOOL bTransform)
{
    return FALSE;
}

CSpell* CCharacter::GetEquippingSpell(void)
{
    return NULL;
}

void CCharacter::_Recovery(void)
{
    INT32 nHpRecovery = m_AttrList.GetAttr(catHpRecovery);
    INT32 nMpRecovery = m_AttrList.GetAttr(catMpRecovery);

    // HP, MP Recovery:
    LOG_PROCESS_ERROR(g_GWConfig.nHpRecoveryParam >= 0);
    LOG_PROCESS_ERROR(g_GWConfig.nMpRecoveryParam >= 0);

    nHpRecovery = nHpRecovery == 0 ? g_GWConfig.nHpRecoveryParam : nHpRecovery;
    nMpRecovery = nMpRecovery == 0 ? g_GWConfig.nMpRecoveryParam : nMpRecovery;

    if (m_nHpTap >= m_AttrList.GetAttr(catHpRecoveryTap))
    {
        ChangeHp(m_AttrList.GetAttr(catMaxHp) / nHpRecovery + 1, cdrRecovery);
        m_nHpTap = 0;
    }
    if (m_nMpTap >= m_AttrList.GetAttr(catMpRecoveryTap))
    {
        ChangeMp(m_AttrList.GetAttr(catMaxMp) / nMpRecovery + 1);
        m_nMpTap = 0;
    }

    if (m_nHpTap < 0 && m_nHpTap < -m_AttrList.GetAttr(catHpRecoveryTap))
    {
        ChangeHp(-1, cdrRecovery);
        m_nHpTap += m_AttrList.GetAttr(catHpRecoveryTap);
    }
Exit0:
    return;
}

void CCharacter::MpToHp(INT32 nThisConvValue)
{
    INT32 nConvRemain = 0;
    INT32 nMpLimit = 0;

    PROCESS_ERROR(nThisConvValue > 0);

    nMpLimit = GetAttr(catMaxMp) - 1;

    if (nThisConvValue > nMpLimit)
    {
        nConvRemain = nThisConvValue - nMpLimit;
        nThisConvValue = nMpLimit;
    }
    else
    {
        nConvRemain = GetAttr(catMpToHpRemain);

        if (nConvRemain > 0)
        {
            if (nThisConvValue > nConvRemain)
            {
                nConvRemain = 0;
            }
            else
                nConvRemain = -nThisConvValue;
        }
    }

    m_AttrList.AddAttrA(this, catMpToHpRemain, nConvRemain);
    m_AttrList.AddAttrA(this, catMpToHp, nThisConvValue);
    m_AttrList.AddAttrA(this, catMaxHp, nThisConvValue);
    m_AttrList.AddAttrA(this, catMaxMp, -nThisConvValue);

Exit0:
    return;
}

void CCharacter::HpToMp(INT32 nThisConvValue)
{
    INT32 nConvRemain = 0;

    PROCESS_ERROR(nThisConvValue > 0);

    if (nThisConvValue >= GetAttr(catMpToHpRemain))
    {
        nConvRemain = GetAttr(catMpToHpRemain);
        nThisConvValue -= nConvRemain;
    }
    else
    {
        nConvRemain = nThisConvValue;
        nThisConvValue = 0;
    }

    if (nThisConvValue > GetAttr(catMpToHp))
        nThisConvValue = GetAttr(catMpToHp);

    nThisConvValue = -nThisConvValue;
    nConvRemain = -nConvRemain;

    m_AttrList.AddAttrA(this, catMpToHpRemain, nConvRemain);
    m_AttrList.AddAttrA(this, catMpToHp, nThisConvValue);
    m_AttrList.AddAttrA(this, catMaxHp, nThisConvValue);
    m_AttrList.AddAttrA(this, catMaxMp, -nThisConvValue);

Exit0:
    return;
}
