#include "stdafx.h"
#include "ai_processor.h"

#include "id_def.h"
#include "error_def.h"

#include "game_world.h"
#include "region.h"
#include "zone_def.h"
#include "scene_file_mgr.h"
#include "character.h"
#include "script_mgr.h"
#include "btnod.h"
#include "npc_data.h"
#include "ai_data.h"
#include "ai_manager.h"
#include "sabac_mgr.h"
#include "guild_mgr.h"

#include "spell_data.h"
#include "spell_config.h"
#include "message_handler.h"
#include "robot_message_handler.h"

#include "profile.h"

#define FLEE_ANGLE          (MAX_DIRECTION_COUNT / 3)
#define FLEE_DISTANCE       (256 * POINT_PER_CM)

#define MAX_ADJUST_DISTANCE (128 * POINT_PER_CM)
#define AI_INTERACT_TIMEOUT (30 * GAME_FPS)

#define MELEE_CHASE_THRESHOLD		(384 * POINT_PER_CM)

enum SYNC_RUN_RESULT
{
	srrInvalid,

	srrNoSyncRun,
	srrSyncRunning,
	srrSyncRunFinished,

	srrTotal
};

CAIProcessor::CAIProcessor() 
{
	m_poOwner = NULL;
	memset(&m_ScanNearby, 0, sizeof(m_ScanNearby));

	m_nAIMoveState = amsInvalid;
	m_nAIFightState = afsInvalid;
	m_nMoveTimer = 0;

	m_nAIBossState = 0;
	m_nLatestHp = 0;

	m_nWaypointCount = 0;
	m_nWaypointIndex = 0;

	m_nAIID = 0;
	m_bIgnoreInvisible = FALSE;
	m_bCounterInvisible = FALSE;

	m_dwTargetID = 0;
	m_dwNearestEnemyID = 0;
	m_dwFarthestEnemyID = 0;
	m_dwCastingEnemyID = 0;
	m_dwNearestFriendID = 0;
	m_dwCorpseCharID = 0;
	m_dwAuraCharID = 0;
	m_dwNearestEnemyRoleID = 0;

	m_nSignalType = 0;
	m_dwSignalIssuerID = 0;
	m_nIssuedSignalNum = 0;

    m_nPatrolPathID = 0;
    m_nPatrolStyle = apsInvalid;
    m_nPatrolIndex = 0;

	m_nWanderPointCount = 0;

	m_llTargetX = 0LL;
	m_llTargetY = 0LL;
    m_nTargetDirection = 0;

	memset(&m_BTCtx, 0, sizeof(m_BTCtx));
	memset(&m_BTOnDieCtx, 0, sizeof(m_BTOnDieCtx));

#ifdef _ROBOT
	m_nRegisteredMsgID = 0;
#endif	// _ROBOT
}

CAIProcessor::~CAIProcessor()
{
	UnInit();
}

BOOL CAIProcessor::Init(CCharacter* poOwner, INT32 nAIID )
{
	INT32 nRetCode = 0;
	const NEW_AI_DATA* pAIData = nullptr;
    const AI_TEMPLATE* pAITemplate = nullptr;

	LOG_PROCESS_ERROR(poOwner);

	m_poOwner = poOwner;
	m_nAIID = nAIID;

	pAIData = CDataManager<NEW_AI_DATA>::GetInstance().GetData(nAIID);
	LOG_PROCESS_ERROR(pAIData);

    pAITemplate = CAIManager::GetInstance().GetTemplate(nAIID);
    LOG_PROCESS_ERROR(pAITemplate);

	memset(&m_ScanNearby, 0, sizeof(m_ScanNearby));
	m_ScanNearby.m_poAIProcessor = this;

	m_nAIMoveState = amsStop;
	m_nAIFightState = afsIdle;

	m_nMoveTimer = 0;

	m_nAIBossState = 0;
	m_nLatestHp = 0;

	m_bIgnoreInvisible = FALSE;
	m_bCounterInvisible = FALSE;

	m_dwTargetID = 0;
	m_dwNearestEnemyID = 0;
	m_dwFarthestEnemyID = 0;
	m_dwCastingEnemyID = 0;
	m_dwNearestFriendID = 0;
	m_dwCorpseCharID = 0;
	m_dwAuraCharID = 0;
	m_dwNearestEnemyRoleID = 0;

	m_nSignalType = 0;
	m_dwSignalIssuerID = 0;
	m_nIssuedSignalNum = 0;

    m_nPatrolPathID = 0;
    m_nPatrolStyle = apsInvalid;
    m_nPatrolIndex = 0;

	m_nWaypointCount = 0;
	m_nWanderPointCount = 0;

	memset(m_nVarList, 0, sizeof(m_nVarList));

	m_BTCtx.poChar = poOwner;
	m_BTCtx.eOwnerType = botAI;
	m_BTCtx.poScript = pAITemplate->pScript;
	m_BTCtx.callFrameCount = 0;
	m_BTCtx.eRunningType = brtInvalid;

	m_BTOnDieCtx.poChar = poOwner;
	m_BTOnDieCtx.eOwnerType = botAI;
	m_BTOnDieCtx.poScript = pAITemplate->pScript;
	m_BTOnDieCtx.callFrameCount = 0;
	m_BTOnDieCtx.eRunningType = brtInvalid;

	m_BTCtx.eEventType = evtInvalid;

	m_poOwner->GetAttrList().SetAttrA(m_poOwner, catWalkSpeed, pAIData->nWalkSpeed);
	m_poOwner->GetAttrList().SetAttrA(m_poOwner, catRunSpeed, pAIData->nRunSpeed);

	m_poOwner->SetCanRun(pAIData->bCanRun);
	
#ifdef _ROBOT
	CallInitBT();
	m_nRegisteredMsgID = 0;
#endif	// _ROBOT

	return TRUE;
Exit0:
	UnInit();

	return FALSE;
}

BOOL CAIProcessor::UnInit(void)
{
	BOOL nRetCode = FALSE;
	m_poOwner = NULL;
	m_nAIID = INVALID_OBJECT_ID;
	m_nAIMoveState = amsInvalid;
	m_nAIFightState = afsInvalid;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::MainLoop(void)
{
	INT32 nRetCode = 0;

	nRetCode = _ProcessMove();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = _ProcessSyncRun(m_BTCtx);
	PROCESS_SUCCESS(nRetCode == srrSyncRunning);

	nRetCode = _ProcessSyncRun(m_BTOnDieCtx);
	PROCESS_SUCCESS(nRetCode == srrSyncRunning);

	nRetCode = _ProcessFightState();
	LOG_CHECK_ERROR(nRetCode);

Exit1:

	return TRUE;
}

BOOL CAIProcessor::_ProcessFightState(void)
{
	INT32 nRetCode = 0;

	switch (m_nAIFightState)
	{
	case afsIdle:
		nRetCode = _OnIdle();
		LOG_PROCESS_ERROR(nRetCode);
		break;
	case afsFight:
		nRetCode = _OnFight();
		break;
	case afsGoHome:
//		nRetCode = _OnGoHome();
		break;
	case afsDying:
		nRetCode = _OnDying();
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return TRUE;
Exit0:
	WRN("failed to run ai processor %d", m_nAIID);
    if (g_GWConfig.DebugOption.debugai)
    {
        char szMessage[1024];
        sprintf(szMessage, "failed to run ai %d, fight state %d", m_nAIID,  m_nAIFightState);
        m_poOwner->Say(cctScreen, cltNormal, szMessage);

    }

	return FALSE;
}

BOOL CAIProcessor::EnterIdle(void)
{
	m_nAIFightState = afsIdle;

	m_dwTargetID = 0;
	m_poOwner->GetThreatList().Clear();

#ifndef _FINAL
	if (g_GWConfig.DebugOption.debugai)
		m_poOwner->Say(cctScreen, cltNormal, "enter idle");
#endif	// !_FINAL

	return TRUE;
}

BOOL CAIProcessor::EnterFight(void)
{
	m_nAIFightState = afsFight;

#ifndef _FINAL
	if (g_GWConfig.DebugOption.debugai)
		m_poOwner->Say(cctScreen, cltNormal, "enter fight");
#endif	// !_FINAL

	return TRUE;
}

BOOL CAIProcessor::GoHome(void)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_poOwner);
	LOG_PROCESS_ERROR(m_poOwner->GetScene());

	m_poOwner->GetThreatList().Clear();
	m_poOwner->SetHome(m_poOwner->GetScene()->GetSceneTempID(), m_poOwner->GetX(), m_poOwner->GetY(), m_poOwner->GetZ());

	nRetCode = EnterIdle();
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::Flee(INT32 nFleeTime)
{
	INT32 nRetCode = 0;
	CCharacter* poTarget = NULL;
	INT32 nAdjustDirection = 0;
	UINT32 dwObjID = 0;

	LOG_PROCESS_ERROR(m_poOwner);

	if (m_nAIMoveState != amsFlee)
	{
		m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + nFleeTime;
		m_llTargetX = m_llTargetY = 0;
//		m_poOwner->Stand(FALSE);
	}
	m_nAIMoveState = amsFlee;

	return TRUE;
Exit0:
	Stop();

	return FALSE;
}

BOOL CAIProcessor::Stop(void)
{
	INT32 nRetCode = 0;

    LOG_CHECK_ERROR(m_poOwner->GetMoveState() != CMS_DIE);
	if (m_poOwner->GetMoveState() != CMS_STAND)
	{
		nRetCode = m_poOwner->Stand(TRUE);
		PROCESS_ERROR(nRetCode);
	}

	m_nAIMoveState = amsStop;
    m_llTargetX = 0;
    m_llTargetY = 0;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::_MoveTo(INT64 llDestX, INT64 llDestY, INT64 llDestZ)
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	CSceneStaticData* pSceneStaticData = NULL;
	const GRID_DATA* pcDestGrid = NULL;
	const ZONE_DATA* pcDestZone = NULL;

	LOG_PROCESS_ERROR(m_poOwner);

	poScene = m_poOwner->GetScene();
	LOG_PROCESS_ERROR(poScene);

	pSceneStaticData = poScene->GetStaticData();
	LOG_PROCESS_ERROR(pSceneStaticData);

	if (llDestZ == 0)
		llDestZ = m_poOwner->GetZ();

	nRetCode = poScene->GetLowerGrid(llDestX, llDestY, llDestZ + GRID_WIDTH, pcDestZone, pcDestGrid);
	LOG_PROCESS_ERROR(nRetCode);

	PROCESS_ERROR(!pcDestGrid->bOutOfPoly);

	llDestZ = pcDestGrid->uHighLayer * GRID_LAYER_HEIGHT;

	m_nWaypointCount = MAX_WAYPOINT_COUNT;
	nRetCode = pSceneStaticData->FindPath((INT32)m_poOwner->GetX(), (INT32)m_poOwner->GetY(), (INT32)m_poOwner->GetZ(), 
		m_poOwner->GetZone()->wEdge[m_poOwner->GetGrid()->uEdge],
		(INT32)llDestX, (INT32)llDestY, (INT32)llDestZ, pcDestZone->wEdge[pcDestGrid->uEdge],
		m_nWaypointCount, (INT32*)&m_nWaypointList);
	PROCESS_ERROR(nRetCode && m_nWaypointCount >= 1);

	nRetCode = m_poOwner->MoveTo(m_nWaypointList[1][0], m_nWaypointList[1][1]);	
	PROCESS_ERROR(nRetCode);

	m_nWaypointIndex = 1;

	//DBG("[%ud] ai move from (%.2f, %.2f) to (%.2f, %.2f)", m_poOwner->GetObjID(),
	//    m_poOwner->GetX() * 1.0f / POINT_PER_CM, m_poOwner->GetY() * 1.0f / POINT_PER_CM,
	//    llDestX * 1.0f / POINT_PER_CM, llDestY * 1.0f / POINT_PER_CM);

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CAIProcessor::SyncMoveTo(INT64 llDestX, INT64 llDestY)
{
	INT32 nRetCode = 0;

	nRetCode = _MoveTo(llDestX, llDestY, 0);
	LOG_PROCESS_ERROR( nRetCode );

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::MoveTo(INT64 llDestX, INT64 llDestY)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_poOwner);

	if (m_poOwner->GetMoveState() == CMS_STAND || 
		m_poOwner->GetDestX() != llDestX && m_poOwner->GetDestY() != llDestY)
	{
		nRetCode = _MoveTo(llDestX, llDestY, 0);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::Wander(INT32 nWanderRange, INT32 nWanderInterval)
{
	INT32 nRetCode = 0;
	INT32 nDirection8 = 0;
	INT64 llDestX, llDestY;

	LOG_PROCESS_ERROR(nWanderRange > 0);
	LOG_PROCESS_ERROR(m_poOwner);

	PROCESS_SUCCESS(m_nMoveTimer > CGameWorld::GetInstance().GetGameFrame());

	if (m_poOwner->GetDistance3D(m_poOwner->GetHomeX(), m_poOwner->GetHomeY(), m_poOwner->GetHomeZ()) >= (INT64)nWanderRange * nWanderRange)
		nDirection8 = ConvertToDir8(g_GetDirection(m_poOwner->GetX(), m_poOwner->GetY(), m_poOwner->GetHomeX(), m_poOwner->GetHomeY()));
	else
		nDirection8 = g_Random(DT_DIRECTION_8_COUNT);
	llDestX = m_poOwner->GetX() + g_nAdjacentX[nDirection8] * GRID_WIDTH;
	llDestY = m_poOwner->GetY() + g_nAdjacentY[nDirection8] * GRID_WIDTH;

	//DBG("wander %d", nDirection8);

	nRetCode = m_poOwner->MoveTo(llDestX, llDestY);
	PROCESS_ERROR(nRetCode);

	m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + g_Random(m_poOwner->GetAttr(catWalkSpeed), nWanderInterval);
	m_nAIMoveState = amsWander;

Exit1:
	return TRUE;
Exit0:
    Stop();
	return FALSE;
}

BOOL CAIProcessor::NeedChase(CCharacter* poTarget, INT32 nChaseThreshold)
{
	INT32 nRetCode = 0;
	INT64 llDistance = 0;
	INT64 llMaxDistance = 0;
	INT64 llDistanceOffset = 0;

	LOG_PROCESS_ERROR(poTarget);

	PROCESS_ERROR(m_nAIMoveState != amsFlee);

	if (m_poOwner->GetMoveState() == CMS_STAND || nChaseThreshold >= MELEE_CHASE_THRESHOLD)
	{
		llDistance = g_GetDistance3D(m_poOwner->GetX(), m_poOwner->GetY(), m_poOwner->GetZ(),
			poTarget->GetX(), poTarget->GetY(), poTarget->GetZ()) / POINT_PER_CM / POINT_PER_CM;
		llMaxDistance = (m_poOwner->GetRadius() + poTarget->GetRadius() + nChaseThreshold) / POINT_PER_CM;

		llMaxDistance *= llMaxDistance;
		if (llDistance > llMaxDistance)
			return TRUE;
	}
	else
	{
		if (m_llTargetX != poTarget->GetX() || m_llTargetY != poTarget->GetY())
		{
			return TRUE;
		}
	}

Exit0:
	return FALSE;
}

BOOL CAIProcessor::NeedWithdraw(CCharacter* poTarget, INT32 nWithdrawThreshold)
{
	return FALSE;
}

BOOL CAIProcessor::Patrol(void)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_poOwner);

    PROCESS_ERROR(m_nPatrolPathID > 0);
	PROCESS_ERROR(m_poOwner->CanActiveMove());

    m_nAIMoveState = amsPatrol;

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CAIProcessor::Chase(INT32 nPreferDistance, INT32 nChaseThreshold, INT32 nDelayFrame)
{
	INT32 nRetCode = 0;
	CCharacter* poTarget = NULL;
	INT32 nDirection = -1;

	LOG_PROCESS_ERROR(m_poOwner);
	PROCESS_ERROR(m_poOwner->CanActiveMove());

	poTarget = CGameWorld::GetInstance().FindValidCharacter(m_dwTargetID);
	PROCESS_ERROR(poTarget);

	{
		if (m_llTargetX != poTarget->GetX() || m_llTargetY != poTarget->GetY())
		{
			nRetCode = _MoveToTarget(poTarget, nPreferDistance, nChaseThreshold);
			PROCESS_ERROR(nRetCode);

			m_llTargetX = poTarget->GetX();
			m_llTargetY = poTarget->GetY();
		}

		m_nAIMoveState = amsChase;

	}
	
	return TRUE;
Exit0:

	return FALSE;
}

BOOL CAIProcessor::Withdraw(INT32 nPreferDistance, INT32 nChaseThreshold)
{
	INT32 nRetCode = 0;
	BOOL bNeedChase = FALSE;
	CCharacter* poTarget = NULL;

	LOG_PROCESS_ERROR(m_poOwner);
	PROCESS_ERROR(m_poOwner->CanActiveMove());

	poTarget = CGameWorld::GetInstance().FindValidCharacter(m_dwTargetID);
	PROCESS_ERROR(poTarget);

	if (m_llTargetX != poTarget->GetX() || m_llTargetY != poTarget->GetY() || m_nAIMoveState != amsWithdraw)
	{
		nRetCode = _MoveToTarget(poTarget, nPreferDistance, nChaseThreshold);
		LOG_PROCESS_ERROR(nRetCode);

		m_llTargetX = poTarget->GetX();
		m_llTargetY = poTarget->GetY();
	}

    m_nAIMoveState = amsWithdraw;

	return TRUE;
Exit0:
    Stop();

	return FALSE;
}

BOOL CAIProcessor::StepAround(CCharacter* poTarget, INT32 nPreferDistance, INT32 nChaseThreshold)
{
	INT32 nRetCode = 0;

	INT64 llTargetX = 0LL;
	INT64 llTargetY = 0LL;
	INT32 nDirection = 0;
	INT32 nAdjustDirection = 0;
	INT32 nAdjustDistance = 0;

	return TRUE;

	LOG_PROCESS_ERROR(m_poOwner);
	LOG_PROCESS_ERROR(nPreferDistance >= 0);

	nAdjustDirection = g_Random(MAX_DIRECTION_COUNT / 6);
	if (nAdjustDirection < MAX_DIRECTION_COUNT / 12)
		nAdjustDirection -= MAX_DIRECTION_COUNT / 6;
	nDirection = g_GetDirection(poTarget->GetX(), poTarget->GetY(), m_poOwner->GetX(), m_poOwner->GetY());
	nDirection = (nDirection + nAdjustDirection + MAX_DIRECTION_COUNT) % MAX_DIRECTION_COUNT;

	if (m_poOwner->GetObjID() & 0x01)
		nAdjustDistance = 0;
	else
	{
		nAdjustDistance = nChaseThreshold - nPreferDistance - 16 * POINT_PER_CM;
		if (nAdjustDistance > MAX_ADJUST_DISTANCE)
			nAdjustDistance = MAX_ADJUST_DISTANCE;
	}
	nPreferDistance += nAdjustDistance;

	llTargetX = poTarget->GetX() + (m_poOwner->GetRadius() + poTarget->GetRadius() + nPreferDistance) * g_Cos(nDirection) / SIN_COS_NUMBER;
	llTargetY = poTarget->GetY() + (m_poOwner->GetRadius() + poTarget->GetRadius() + nPreferDistance) * g_Sin(nDirection) / SIN_COS_NUMBER;

	nRetCode = _MoveTo(llTargetX, llTargetY, 0);
	PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	Stop();

	return FALSE;
}

BOOL CAIProcessor::Follow(void)
{
	INT32 nRetCode = 0;
	CCharacter* poHost = NULL;
	INT32 nFollowSpeed = 0;
	INT64 llDestX, llDestY, llDistance2D;
	INT32 nDirectionY, nDirectionX;
    INT32 nOffsetX = m_nFollowOffsetX;
    INT32 nOffsetY = m_nFollowOffsetY;
    const NEW_AI_DATA   *pAIData = NULL;
    INT32 nAIID = 0;

	LOG_PROCESS_ERROR(m_poOwner);

	poHost = CGameWorld::GetInstance().FindValidCharacter(m_dwFollowHostID);
	PROCESS_ERROR(poHost);

    PROCESS_SUCCESS(poHost->GetScene() != NULL && poHost->GetScene() == m_poOwner->GetScene() 
        && poHost->GetX() == m_llTargetX && poHost->GetY() == m_llTargetY && m_nAIMoveState == amsFollow);

	nDirectionY = poHost->GetFaceDirection();
	nDirectionX = (nDirectionY + (MAX_DIRECTION_COUNT * 3 / 4)) % MAX_DIRECTION_COUNT;
	llDestX = poHost->GetX() + nOffsetX * g_Cos(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Cos(nDirectionY) / SIN_COS_NUMBER;
	llDestY = poHost->GetY() + nOffsetX * g_Sin(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Sin(nDirectionY) / SIN_COS_NUMBER;

    if (m_poOwner->GetScene() != NULL && poHost->GetScene() != NULL && poHost->GetScene() != m_poOwner->GetScene())
    {
        if (m_poOwner->GetObjType() == otRole)
        {
            nRetCode = CGameWorld::GetInstance().JumpTo((CRole*)m_poOwner, poHost->GetScene()->GetSceneTempID(),
                poHost->GetScene()->GetSceneID(), poHost->GetX(), poHost->GetY(), poHost->GetZ(), poHost->GetFaceDirection());
            LOG_PROCESS_ERROR(nRetCode);
        }
        else if (m_poOwner->GetObjType() == otNpc)
		{
			//Do not follow across scene transitions
			PROCESS_ERROR(FALSE);
            //nRetCode = CGameWorld::GetInstance().JumpTo((CNpc*)m_poOwner, poHost->GetScene()->GetSceneTempID(),
            //    poHost->GetScene()->GetSceneID(), poHost->GetX(), poHost->GetY(), poHost->GetZ(), poHost->GetFaceDirection());
            //LOG_PROCESS_ERROR(nRetCode);
        }
    }

	llDistance2D = g_GetDistance2D(m_poOwner->GetX(), m_poOwner->GetY(), llDestX, llDestY);
	if (m_bUseHostSpeed)
	{
		nFollowSpeed = poHost->GetAttrList().GetAttr(catWalkSpeed);
	}
	else
    {
        nFollowSpeed = m_poOwner->GetAttr(catWalkSpeed);
    }

#ifndef _ROBOT
	m_poOwner->GetAttrList().SetAttrA(m_poOwner, catWalkSpeed, nFollowSpeed);
#endif  // _ROBOT

	nRetCode = _MoveTo(llDestX, llDestY, m_poOwner->GetZ());
	PROCESS_ERROR(nRetCode);

	m_llTargetX = poHost->GetX();
	m_llTargetY = poHost->GetY();

	m_nAIMoveState = amsFollow;

Exit1:

	return TRUE;
Exit0:
	Stop();

	return FALSE;
}

BOOL CAIProcessor::PetFollow(void)
{
    INT32 nRetCode = 0;
    CRole*      poRole = NULL;
    CCharacter* poHost = NULL;
    INT32 nFollowSpeed = 0;
    INT64 llDestX, llDestY, llDistance2D;
    INT32 nDirectionY, nDirectionX;
    INT32 nOffsetX = m_nFollowOffsetX;
    INT32 nOffsetY = m_nFollowOffsetY;
    const NEW_AI_DATA   *pAIData = NULL;
    INT32 nAIID = 0;
    INT32 nIndex = 0;

    LOG_PROCESS_ERROR(m_poOwner);

    poHost = CGameWorld::GetInstance().FindValidCharacter(m_dwFollowHostID);
    PROCESS_ERROR(poHost);

    PROCESS_ERROR(poHost->GetObjType() == otRole);
    poRole = (CRole*)poHost;

    PROCESS_SUCCESS(poHost->GetScene() != NULL && poHost->GetScene() == m_poOwner->GetScene()
        && poHost->GetX() == m_llTargetX && poHost->GetY() == m_llTargetY && poHost->GetFaceDirection() == m_nTargetDirection && m_nAIMoveState == amsFollow);

    nDirectionY = poHost->GetFaceDirection();
    nDirectionX = (nDirectionY + (MAX_DIRECTION_COUNT * 3 / 4)) % MAX_DIRECTION_COUNT;

    nIndex = poRole->GetPetIndex(m_poOwner->GetObjID());
    if (nIndex < MAX_PET_PER_ROLE)
    {
        if (nIndex == 0)
            nOffsetX = nOffsetX + GRID_WIDTH;
        else if (nIndex == 2)
            nOffsetX = nOffsetX - GRID_WIDTH;
        else if (nIndex == 3)
        {
            nOffsetX = nOffsetX - GRID_WIDTH;
            nOffsetY = nOffsetY + GRID_WIDTH;
        }
        else if (nIndex == 4)
        {
            nOffsetY = nOffsetY + GRID_WIDTH * 2;
            nOffsetX = nOffsetX - GRID_WIDTH;
        }
    }

    llDestX = poHost->GetX() + nOffsetX * g_Cos(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Cos(nDirectionY) / SIN_COS_NUMBER;
    llDestY = poHost->GetY() + nOffsetX * g_Sin(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Sin(nDirectionY) / SIN_COS_NUMBER;

    if (m_poOwner->GetScene() != NULL && poHost->GetScene() != NULL && poHost->GetScene() != m_poOwner->GetScene())
    {
        if (m_poOwner->GetObjType() == otRole)
        {
            nRetCode = CGameWorld::GetInstance().JumpTo((CRole*)m_poOwner, poHost->GetScene()->GetSceneTempID(),
                poHost->GetScene()->GetSceneID(), poHost->GetX(), poHost->GetY(), poHost->GetZ(), poHost->GetFaceDirection());
            LOG_PROCESS_ERROR(nRetCode);
        }
        else if (m_poOwner->GetObjType() == otNpc)
        {
            nRetCode = CGameWorld::GetInstance().JumpTo((CNpc*)m_poOwner, poHost->GetScene()->GetSceneTempID(),
                poHost->GetScene()->GetSceneID(), poHost->GetX(), poHost->GetY(), poHost->GetZ(), poHost->GetFaceDirection());
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    llDistance2D = g_GetDistance2D(m_poOwner->GetX(), m_poOwner->GetY(), llDestX, llDestY);
    if (m_bUseHostSpeed)
    {
        nFollowSpeed = poHost->GetAttrList().GetAttr(catWalkSpeed);
    }
    else
    {
        nFollowSpeed = m_poOwner->GetAttr(catWalkSpeed);
        //nFollowSpeed = GetVariable(PET_AIV_WALK_SPEED);
    }

#ifndef _ROBOT
	m_poOwner->GetAttrList().SetAttrA(m_poOwner, catWalkSpeed, nFollowSpeed);
#endif  // _ROBOT

    nRetCode = _MoveTo(llDestX, llDestY, m_poOwner->GetZ());
    PROCESS_ERROR(nRetCode);

    m_llTargetX = poHost->GetX();
    m_llTargetY = poHost->GetY();
    m_nTargetDirection = poHost->GetFaceDirection();

    m_nAIMoveState = amsFollow;

Exit1:
    return TRUE;
Exit0:
    Stop();
    return FALSE;
}


BOOL CAIProcessor::JumpToHost(void)
{
    INT32 nRetCode = 0;
    CCharacter* poHost = NULL;
    INT32 nFollowSpeed = 0;
    INT64 llDestX, llDestY;
    INT32 nDirectionY, nDirectionX;
    INT32 nOffsetX = m_nFollowOffsetX;
    INT32 nOffsetY = m_nFollowOffsetY;
    const NEW_AI_DATA   *pAIData = NULL;
    INT32 nAIID = 0;

    LOG_PROCESS_ERROR(m_poOwner);

    poHost = CGameWorld::GetInstance().FindValidCharacter(m_dwFollowHostID);
    PROCESS_ERROR(poHost);

    //PROCESS_SUCCESS(poHost->GetScene() != NULL && poHost->GetScene() == m_poOwner->GetScene()
    //    && poHost->GetX() == m_llTargetX && poHost->GetY() == m_llTargetY && m_nAIMoveState == amsFollow);

    nDirectionY = poHost->GetFaceDirection();
    nDirectionX = (nDirectionY + (MAX_DIRECTION_COUNT * 3 / 4)) % MAX_DIRECTION_COUNT;
    llDestX = poHost->GetX() + nOffsetX * g_Cos(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Cos(nDirectionY) / SIN_COS_NUMBER;
    llDestY = poHost->GetY() + nOffsetX * g_Sin(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Sin(nDirectionY) / SIN_COS_NUMBER;

    PROCESS_ERROR(poHost->GetScene());
    if (m_poOwner->GetObjType() == otRole)
    {
        nRetCode = CGameWorld::GetInstance().JumpTo((CRole*)m_poOwner, poHost->GetScene()->GetSceneTempID(),
            poHost->GetScene()->GetSceneID(), llDestX, llDestY, poHost->GetZ(), poHost->GetFaceDirection());
        LOG_PROCESS_ERROR(nRetCode);
    }
    else if (m_poOwner->GetObjType() == otNpc)
    {
        nRetCode = CGameWorld::GetInstance().JumpTo((CNpc*)m_poOwner, poHost->GetScene()->GetSceneTempID(),
            poHost->GetScene()->GetSceneID(), llDestX, llDestY, poHost->GetZ(), poHost->GetFaceDirection());
        LOG_PROCESS_ERROR(nRetCode);
    }

    m_llTargetX = poHost->GetX();
    m_llTargetY = poHost->GetY();
    m_nTargetDirection = poHost->GetFaceDirection();

    m_nAIMoveState = amsFollow;

    return TRUE;
Exit0:
    return FALSE;
}



/************************************************************************/

BOOL CAIProcessor::_OnIdle(void)
{
	INT32 nRetCode = 0;

	nRetCode = _RunBT(evtAIIdle, 0, FALSE);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:

	CRI("AI %d idle tree brvError", m_nAIID );
	return FALSE;
}

BOOL CAIProcessor::_OnFight(void)
{
	INT32 nRetCode = 0;

	nRetCode = _RunBT(evtAIFight, 0, FALSE);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	GoHome();

	CRI("AI %d fight tree brvError", m_nAIID);
	return FALSE;
}

BOOL CAIProcessor::_OnDying(void)
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(m_poOwner);

	if (m_BTOnDieCtx.callFrameCount > 0)
	{
		DBG("AI %d dead tree", m_nAIID );
		BTCtx_run(m_BTOnDieCtx);
	}
	else
	{
		if (m_poOwner->GetFlag().CheckFlag(cfBTDenyDie))
		{
			m_poOwner->GetFlag().ClearFlag(cfBTDenyDie);
			EnterFight();
		}
		else
		{
			m_poOwner->Die(0);
		}
	}

	return TRUE;
Exit0:
	GoHome();

	CRI("AI %d dead tree brvError", m_nAIID);
	return FALSE;
}


BOOL CAIProcessor::_OnGoHome(void)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_poOwner);
	LOG_PROCESS_ERROR(m_poOwner->GetScene());

	//	GetFlag().ResetFlag();
	m_poOwner->GetFlag().ResetFlag();
	m_poOwner->SetHp( m_poOwner->GetAttr(catMaxHp) );

	if (m_poOwner->GetObjType() == otNpc)
	{
		((CNpc*)m_poOwner)->ResetFlags();
	}

	if (m_poOwner->GetRegion() && !m_poOwner->GetRegion()->IsInactiving())
	{
		if (m_nAIMoveState == amsStop || m_nMoveTimer <= CGameWorld::GetInstance().GetGameFrame())
		{
			if (m_poOwner->GetObjType() == otNpc)
			{
				CNpc* poNpc = (CNpc*)m_poOwner;
				if (poNpc->GetSpawnPoint())
					m_poOwner->RotateTo(poNpc->GetSpawnPoint()->nDirection, 0);
			}
			EnterIdle();
		}
	}
	else
	{
#ifndef _ROBOT
		nRetCode = m_poOwner->JumpTo(m_poOwner->GetHomeX(), m_poOwner->GetHomeY(), m_poOwner->GetHomeZ(), m_poOwner->GetFaceDirection());
		LOG_PROCESS_ERROR(nRetCode);

		if (m_poOwner->GetObjType() == otNpc)
		{
			CNpc* poNpc = (CNpc*)m_poOwner;
			if (poNpc->GetSpawnPoint())
				m_poOwner->RotateTo(poNpc->GetSpawnPoint()->nDirection, 0);
		}
#endif  // _ROBOT
		EnterIdle();
	}

	return TRUE;
Exit0:
	EnterIdle();

	return FALSE;
}

BOOL CAIProcessor::_MoveToTarget(CCharacter* poTarget, INT32 nPreferDistance, INT32 nChaseThreshold)
{
	INT32 nRetCode = 0;
	INT32 nDirection = 0;
	INT64 llDestX, llDestY;
	UINT32 dwObjID = 0;
	INT32 nDistance = 0;

	LOG_PROCESS_ERROR(poTarget);
	LOG_PROCESS_ERROR(m_poOwner);
	PROCESS_ERROR(m_poOwner->CanActiveMove());

	nDirection = g_GetDirection(m_poOwner->GetX(), m_poOwner->GetY(), poTarget->GetX(), poTarget->GetY());

	nDistance = m_poOwner->GetRadius() + poTarget->GetRadius() + nPreferDistance;
	llDestX = poTarget->GetX() - (nDistance - GRID_WIDTH) * g_Cos(nDirection) / SIN_COS_NUMBER;
	llDestY = poTarget->GetY() - (nDistance - GRID_WIDTH) * g_Sin(nDirection) / SIN_COS_NUMBER;

	llDestX = llDestX / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	llDestY = llDestY / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;

//	PROCESS_SUCCESS((llDestX - m_poOwner->GetX()) >= -nDistance && (llDestX - m_poOwner->GetX()) <= nDistance &&
//		(llDestY - m_poOwner->GetY()) >= -nDistance && (llDestY - m_poOwner->GetY()) <= nDistance);

	nRetCode = _MoveTo(llDestX, llDestY, poTarget->GetZ());
	if (!nRetCode)
	{
        INT32 nPos[1][3];
        nRetCode = m_poOwner->GetScene()->GetStaticData()->GetNearestAvailablePos(llDestX, llDestY, m_poOwner->GetZ(), 32, 1, nPos);
        if (nRetCode)
        {
            nRetCode = _MoveTo(nPos[0][0], nPos[0][1], nPos[0][2]);
            PROCESS_ERROR(nRetCode);
        }
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::_ProcessMove(void)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_poOwner);

	switch (m_nAIMoveState)
	{
	case amsStop:
		break;
	case amsWander:
		if (CGameWorld::GetInstance().GetGameFrame() > m_nMoveTimer)
			Stop();
		break;
	case amsChase:
	case amsWithdraw:
		if (CGameWorld::GetInstance().GetGameFrame() <= m_nMoveTimer + GAME_FPS / 4)
			break;
	case amsFollow:
	case amsGoHome:
		if (m_poOwner->GetMoveState() == CMS_STAND)
		{
			if (m_poOwner->GetX() / GRID_WIDTH == m_nWaypointList[m_nWaypointIndex][0] / GRID_WIDTH &&
				m_poOwner->GetY() / GRID_WIDTH == m_nWaypointList[m_nWaypointIndex][1] / GRID_WIDTH)
			{
				m_nWaypointIndex++;
                m_nWaypointList[m_nWaypointIndex][2] = 0;
			}
			if (m_nWaypointIndex < m_nWaypointCount && m_nWaypointList[m_nWaypointIndex][2] < 3)
			{
				nRetCode = m_poOwner->MoveTo(m_nWaypointList[m_nWaypointIndex][0], 
					m_nWaypointList[m_nWaypointIndex][1]);	
				PROCESS_ERROR(nRetCode);

                m_nWaypointList[m_nWaypointIndex][2]++;
			}
			else
			{
				Stop();
			}
		}
		break;
	case amsFlee:
		if (CGameWorld::GetInstance().GetGameFrame() < m_nMoveTimer)
		{
			CCharacter* poTarget = CGameWorld::GetInstance().FindValidCharacter(m_dwTargetID);
			if (poTarget && !poTarget->IsDead())
			{
				if (m_poOwner->GetMoveState() == CMS_STAND)
				{
					INT32 nDirection8 = ConvertToDir8(
						g_GetDirection(poTarget->GetX(), poTarget->GetY(), m_poOwner->GetX(), m_poOwner->GetY()));
					nDirection8 = nDirection8 + 1 - g_Random(2);

					INT64 llDestX = m_poOwner->GetX() + g_nAdjacentX[nDirection8] * GRID_WIDTH;
					INT64 llDestY = m_poOwner->GetY() + g_nAdjacentY[nDirection8] * GRID_WIDTH;
					//DBG("flee %d", nDirection8);
					nRetCode = _MoveTo(llDestX, llDestY, 0);
					if (!nRetCode)
					{
						INT32 nPos[1][3];
						nRetCode = m_poOwner->GetScene()->GetStaticData()->GetNearestAvailablePos(llDestX, llDestY, m_poOwner->GetZ(), 32, 1, nPos);
						if (nRetCode)
						{
							nRetCode = _MoveTo(nPos[0][0], nPos[0][1], nPos[0][2]);
							PROCESS_ERROR(nRetCode);
						}
					}
				}
			}
			else
			{
				Stop();
			}
		}
		else
		{
			Stop();
		}
		break;
    case amsPatrol:
        if (m_poOwner->GetMoveState() == CMS_STAND && m_nInteractTimer <= CGameWorld::GetInstance().GetGameFrame())
        {
            CScene* poScene = m_poOwner->GetScene();
            LOG_PROCESS_ERROR(poScene);

            CSceneStaticData* pSceneStaticData = poScene->GetStaticData();
            LOG_PROCESS_ERROR(pSceneStaticData);

            INT32 nPointCount = pSceneStaticData->GetPathPointCount(m_nPatrolPathID);

            INT32 nPatrolIndex = m_nPatrolIndex;
            if (m_nPatrolStyle == apsLineBack && m_nPatrolIndex >= nPointCount)
                nPatrolIndex = nPointCount * 2 - m_nPatrolIndex - 1;

            const SERVER_POINT* pcServerPoint = pSceneStaticData->GetPathPoint(m_nPatrolPathID, nPatrolIndex);
            LOG_PROCESS_ERROR(pcServerPoint);

			if ((m_poOwner->GetX() / GRID_WIDTH == pcServerPoint->nX / GRID_WIDTH) &&
				(m_poOwner->GetY() / GRID_WIDTH == pcServerPoint->nY / GRID_WIDTH))
            {
                m_nPatrolIndex++;

                if (m_nPatrolStyle == apsLineBack)
                {
                    if (m_nPatrolIndex >= nPointCount * 2)
                    {
                        nPatrolIndex = 0;
                        m_nPatrolIndex = 0;
                    }
                    else if (m_nPatrolIndex >= nPointCount)
                        nPatrolIndex = nPointCount * 2 - m_nPatrolIndex - 1;
                }
                else if (m_nPatrolStyle == apsCircle && m_nPatrolIndex >= nPointCount)
                {
                    nPatrolIndex = 0;
                    m_nPatrolIndex = 0;
                }
            }

            if (nPatrolIndex < nPointCount)
            {
                const SERVER_POINT* pcServerPoint = pSceneStaticData->GetPathPoint(m_nPatrolPathID, nPatrolIndex);
                LOG_PROCESS_ERROR(pcServerPoint);

                nRetCode = m_poOwner->MoveTo(pcServerPoint->nX, pcServerPoint->nY);
                PROCESS_ERROR(nRetCode);
            }
            else
            {
                Stop();
            }
        }
        break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return TRUE;
Exit0:
	Stop();

	return FALSE;
}

BOOL CAIProcessor::CScanSignal::operator()(stSignal* pSignal)
{
	CCharacter* pIssuer = NULL;
	LOG_PROCESS_ERROR(pSignal);

	PROCESS_ERROR( pSignal->nRemainTimes > 0);
	PROCESS_ERROR( pSignal->nExpiredFrame > CGameWorld::GetInstance().GetGameFrame() );
	PROCESS_ERROR( pSignal->nType == nFindSignalType );

	pIssuer = CGameWorld::GetInstance().FindCharacter( pSignal->dwIssuerID );
	if( NULL == pIssuer )
	{
		pSignal->nRemainTimes = 0;  // will remove in mainloop
		PROCESS_ERROR(FALSE);
	}

	PROCESS_ERROR(pIssuer != pSignalChecker);
	PROCESS_ERROR(!pSignalChecker->CanAttack(pIssuer));

	if( pSignalChecker->GetDistance3D( pIssuer ) <= llFindDistanceSquare )
	{
		pFoundSignal = pSignal;
		return FALSE;   // stop traverse
	}

Exit0:
	return TRUE;    // keep traverse
}

stSignal* CAIProcessor::PickSignal( INT32 nScanDistance, INT32 nSignalType )
{
	CRegion* poCenterRegion = NULL;
	INT32 RandStartPos = 0;

	CScanSignal oSignaltraverser;
	oSignaltraverser.pFoundSignal = NULL;
	oSignaltraverser.pSignalChecker = m_poOwner;
	oSignaltraverser.nFindSignalType = nSignalType;
	oSignaltraverser.llFindDistanceSquare = ((INT64)nScanDistance) * nScanDistance;

	LOG_PROCESS_ERROR( m_poOwner );

	poCenterRegion = m_poOwner->GetRegion();
	LOG_PROCESS_ERROR( poCenterRegion );

	poCenterRegion->TraverseSignal( oSignaltraverser );
	PROCESS_SUCCESS( oSignaltraverser.pFoundSignal );

	RandStartPos = rand() % DT_DIRECTION_8_COUNT;
	for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; ++nDirection)
	{
		CRegion* poConnectedRegion = poCenterRegion->GetConnectedRegion( (nDirection + RandStartPos) % DT_DIRECTION_8_COUNT );
		if (poConnectedRegion == NULL)
			continue;

		poConnectedRegion->TraverseSignal( oSignaltraverser );
		PROCESS_SUCCESS( oSignaltraverser.pFoundSignal );
	}

Exit0:
Exit1:
	if ( oSignaltraverser.pFoundSignal )
	{
		m_nSignalType = oSignaltraverser.pFoundSignal->nType;
		m_dwSignalIssuerID = oSignaltraverser.pFoundSignal->dwIssuerID;
		//m_nSignalParam = oSignaltraverser.pFoundSignal->nParam;
		//m_nSignalRemainTimes = oSignaltraverser.pFoundSignal->nRemainTimes;
		//m_nSignalExpiredFrame = oSignaltraverser.pFoundSignal->nExpiredFrame;

		--oSignaltraverser.pFoundSignal->nRemainTimes;

//		DBG( "character %d successfully ScanSignal %d, and reduce nRemainTimes", m_dwSignalIssuerID, m_nSignalType );
	}
	//else
	//{
	//    m_nSignalType = 0;
	//    m_nSignalParam = 0;
	//    m_nSignalRemainTimes = 0;
	//    m_nSignalExpiredFrame = 0;
	//    m_dwSignalIssuerID = 0;
	//}
	return oSignaltraverser.pFoundSignal;
}

BOOL CAIProcessor::IssueSignal( INT32 nSignalType, INT32 nSignalLivingFrame, INT32 nSignalProcessTimes, INT32 nSignalParam )
{
	BOOL nRetCode = TRUE;
	CRegion* pRegion = NULL;

	LOG_PROCESS_ERROR( m_poOwner );

	LOG_PROCESS_ERROR( m_nIssuedSignalNum <= SIGNAL_MAX_NUM_PER_CHARACTER );

	pRegion = m_poOwner->GetRegion();
	LOG_PROCESS_ERROR( pRegion );

	nRetCode = pRegion->InsertSignal( m_poOwner->GetObjID(), 
		nSignalType, nSignalParam, CGameWorld::GetInstance().GetGameFrame()+ nSignalLivingFrame, nSignalProcessTimes );
	PROCESS_ERROR( nRetCode );

	++ m_nIssuedSignalNum;

	return TRUE;
Exit0:
	return FALSE;
}

void CAIProcessor::DecreaseSignalNum(void)
{
	--m_nIssuedSignalNum;
	if ( m_nIssuedSignalNum < 0 )
	{
		m_nIssuedSignalNum = 0;
	}
}


BOOL CAIProcessor::SetScanEnemyParam(INT32 nScanEnemyRange)
{
	m_ScanNearby.m_llScanEnemyRange = (INT64)nScanEnemyRange * nScanEnemyRange;

	return TRUE;
}

BOOL CAIProcessor::SetScanFriendParam(INT32 nScanFriendRange, INT32 nFriendTempalteID)
{
	m_ScanNearby.m_llScanFriendRange = (INT64)nScanFriendRange * nScanFriendRange;
	m_ScanNearby.m_nFriendTempalteID = nFriendTempalteID;

	return TRUE;
}

BOOL CAIProcessor::SetScanCorpseParam(INT32 nCorpseTemplateID, BOOL bFriendCorpse)
{
	m_ScanNearby.m_nCorpseTemplateID = nCorpseTemplateID;
	m_ScanNearby.m_bFriendCorpse = bFriendCorpse;

	return TRUE;
}

BOOL CAIProcessor::SetScanAuraParam(INT32 nAuraID)
{
	m_ScanNearby.m_nAuraID = nAuraID; 

	return TRUE;
}

inline BOOL CAIProcessor::CScanNearby::operator()(CCharacter* poChar)
{
	INT64 llDistance;

	PROCESS_ERROR(poChar != m_poAIProcessor->m_poOwner);

	if (poChar->IsDead())
	{
		PROCESS_ERROR(m_poAIProcessor->m_dwCorpseCharID == INVALID_OBJECT_ID);

		if (m_nCorpseTemplateID != INVALID_OBJECT_ID && poChar->GetObjType() == otNpc)
		{
			CNpc* poNpc = (CNpc*)poChar;
			PROCESS_ERROR(poNpc->GetNpcData() && poNpc->GetNpcData()->nTemplateID == m_nCorpseTemplateID);
		}

		if (m_poAIProcessor->m_poOwner->CanAttack(poChar))
		{
			PROCESS_ERROR(!m_bFriendCorpse);

			llDistance = m_poAIProcessor->m_poOwner->GetDistance3D(poChar);
			PROCESS_ERROR(llDistance < m_llScanEnemyRange);
		}
		else
		{
			PROCESS_ERROR(m_bFriendCorpse);

			llDistance = m_poAIProcessor->m_poOwner->GetDistance3D(poChar);
			PROCESS_ERROR(llDistance < m_llScanFriendRange);
		}

		LOG_PROCESS_ERROR(poChar->GetScene());
		PROCESS_ERROR(!poChar->GetScene()->IsObstacled(m_poAIProcessor->GetOwner(), poChar, FALSE));

		m_poAIProcessor->m_dwCorpseCharID = poChar->GetObjID();
	}
	else
	{
		if (poChar->GetCCFlag().CheckFlag(cclInvisible) && !m_poAIProcessor->IgnoreInvisible())
		{
			PROCESS_ERROR(m_poAIProcessor->CounterInvisible());
			PROCESS_ERROR(m_poAIProcessor->m_poOwner->GetThreatList().CheckThreat(poChar->GetObjID()));
		}

		if (m_poAIProcessor->GetOwner()->CanAttack(poChar))
		{
			PROCESS_ERROR(!poChar->GetCCFlag().CheckFlag(cclInvincible));
            PROCESS_ERROR(poChar->GetFlag().CheckFlag(cfNeverBeHit) == FALSE);      // redmine 23610, Trap monster

			if (poChar->GetObjType() == otRole)
			{
				CRole* poRole = (CRole*)poChar;
				PROCESS_ERROR(!poRole->IsHidden());
			}

			llDistance = m_poAIProcessor->GetOwner()->GetDistance3D(poChar);
			PROCESS_ERROR(llDistance < m_llScanEnemyRange);

			LOG_PROCESS_ERROR(poChar->GetScene());
			PROCESS_ERROR(!poChar->GetScene()->IsObstacled(m_poAIProcessor->GetOwner(), poChar, FALSE));

			if (m_nAuraID != INVALID_OBJECT_ID && poChar->GetFirstAura(m_nAuraID))
			{
				m_poAIProcessor->m_dwAuraCharID = poChar->GetObjID();
			}

			if (poChar->IsCasting())
			{
				m_poAIProcessor->m_dwCastingEnemyID = poChar->GetObjID();
			}

			if (llDistance < m_llNearestEnemyDistance)
			{
				m_poAIProcessor->m_dwNearestEnemyID = poChar->GetObjID();
				m_llNearestEnemyDistance = llDistance;
			}

			if (poChar->GetObjType() == otRole && llDistance < m_llNearestEnemyRoleDistance)
			{
				m_poAIProcessor->m_dwNearestEnemyRoleID = poChar->GetObjID();
				m_llNearestEnemyRoleDistance = llDistance;
			}

			if (llDistance > m_llFarthestEnemyDistance)
			{
				m_poAIProcessor->m_dwFarthestEnemyID = poChar->GetObjID();
				m_llFarthestEnemyDistance = llDistance;
			}
		}
		else
		{
			llDistance = m_poAIProcessor->GetOwner()->GetDistance3D(poChar);
			PROCESS_ERROR(llDistance < m_llScanFriendRange);

			if (m_nFriendTempalteID != INVALID_OBJECT_ID && poChar->GetObjType() == otNpc)
			{
				CNpc* poNpc = (CNpc*)poChar;
				PROCESS_ERROR(poNpc->GetNpcData() && poNpc->GetNpcData()->nTemplateID == m_nFriendTempalteID);
			}

			LOG_PROCESS_ERROR(poChar->GetScene());
			PROCESS_ERROR(!poChar->GetScene()->IsObstacled(m_poAIProcessor->GetOwner(), poChar, FALSE));

			if (llDistance < m_llScanFriendRange)
			{
				m_poAIProcessor->m_dwNearestFriendID = poChar->GetObjID();
				m_llNearestFriendDistance = llDistance;
			}
		}
	}

Exit0:
	return TRUE;
}

BOOL CAIProcessor::CPetScanEnemy::operator()(CCharacter* poChar)
{
    CNpc    *poNpc = NULL;
    INT64 llDistance;

    PROCESS_ERROR(poChar != m_poAIProcessor->m_poOwner);
    PROCESS_ERROR(!poChar->IsDead());
    PROCESS_ERROR(poChar->GetObjType() == otNpc);
    PROCESS_ERROR(m_poAIProcessor->m_poOwner->CanAttack(poChar));
    PROCESS_ERROR(poChar->GetLevel() < m_nLimitLevel);
    PROCESS_ERROR(!poChar->GetCCFlag().CheckFlag(cclInvincible));
    PROCESS_ERROR(!poChar->GetFlag().CheckFlag(cfAuraInvalid));
    PROCESS_ERROR(!poChar->GetFlag().CheckFlag(cfNeverBeHit));
    poNpc = (CNpc *)poChar;
    PROCESS_ERROR((poNpc->GetNpcData()->nSpellEffectGroup & m_nspellEffectGroup) == TRUE);

    llDistance = m_poAIProcessor->m_poOwner->GetDistance3D(poChar);
    PROCESS_ERROR(llDistance < m_llScanEnemyRange);

    if (llDistance < m_llNearestEnemyDistance)
    {
        m_dwTargetID = poChar->GetObjID();
        m_llNearestEnemyDistance = llDistance;
    }

Exit0:
    return TRUE;
}

BOOL CAIProcessor::CScanEnemy::operator()(CCharacter* poChar)
{
	INT64 llDistance;

	PROCESS_ERROR(!poChar->IsDead());
	PROCESS_ERROR(!poChar->GetCCFlag().CheckFlag(cclInvincible));
    PROCESS_ERROR(poChar->GetFlag().CheckFlag(cfNeverBeHit) == FALSE);      // redmine 23610, Trap monster

	if (poChar->GetCCFlag().CheckFlag(cclInvisible) && !m_poAIProcessor->IgnoreInvisible())
	{
		PROCESS_ERROR(m_poAIProcessor->CounterInvisible());
		PROCESS_ERROR(m_poAIProcessor->m_poOwner->GetThreatList().CheckThreat(poChar->GetObjID()));
	}

	if (poChar->GetObjType() == otRole)
	{
		CRole* poRole = (CRole*)poChar;
		PROCESS_ERROR(!poRole->IsHidden());
	}

	llDistance = m_poAIProcessor->GetOwner()->GetDistance3D(poChar);
	PROCESS_ERROR(llDistance < m_llNearestEnemyDistance);

	PROCESS_ERROR(m_poAIProcessor->GetOwner()->CanAttack(poChar));

	LOG_PROCESS_ERROR(poChar->GetScene());
	PROCESS_ERROR(!poChar->GetScene()->IsObstacled(m_poAIProcessor->GetOwner(), poChar, FALSE));

	m_poAIProcessor->m_dwTargetID = poChar->GetObjID();
	m_llNearestEnemyDistance = llDistance;

Exit0:
	return TRUE;
}

BOOL CAIProcessor::CScanRedName::operator()(CCharacter* poChar)
{
	INT64 llDistance;

	if (poChar->GetObjType() == otRole)
	{
		CRole* poRole = (CRole*)poChar;
		PROCESS_ERROR(poRole->IsRedName());
		PROCESS_ERROR(!poRole->IsHidden());
	}
	else if (poChar->GetObjType() == otNpc)
	{
		CNpc* poNpc = (CNpc*)poChar;
		PROCESS_ERROR(!m_bOnlyRedName);
	}

	PROCESS_ERROR(!poChar->IsDead());
	PROCESS_ERROR(!poChar->GetCCFlag().CheckFlag(cclInvincible));
    PROCESS_ERROR(poChar->GetFlag().CheckFlag(cfNeverBeHit) == FALSE);      // redmine 23610, Trap monster

	llDistance = m_poAIProcessor->GetOwner()->GetDistance3D(poChar);
	PROCESS_ERROR(llDistance < m_llNearestEnemyDistance);

	PROCESS_ERROR(m_poAIProcessor->GetOwner()->CanAttack(poChar));

	LOG_PROCESS_ERROR(poChar->GetScene());
	PROCESS_ERROR(!poChar->GetScene()->IsObstacled(m_poAIProcessor->GetOwner(), poChar, FALSE));

	m_poAIProcessor->m_dwTargetID = poChar->GetObjID();
	m_llNearestEnemyDistance = llDistance;

Exit0:
	return TRUE;
}

BOOL CAIProcessor::CScanSabacWinner::operator()(CCharacter* poChar)
{
	INT64 llDistance;
	CRole* poRole = (CRole*)poChar;

	PROCESS_ERROR(poChar->GetObjType() == otRole);
	PROCESS_ERROR(!poRole->IsHidden());
	PROCESS_ERROR(!poChar->IsDead());

	llDistance = m_poAIProcessor->GetOwner()->GetDistance3D(poChar);
	PROCESS_ERROR(llDistance < m_llNearestFriendDistance);

	LOG_PROCESS_ERROR(poChar->GetScene());
	PROCESS_ERROR(!poChar->GetScene()->IsObstacled(m_poAIProcessor->GetOwner(), poChar, FALSE));

	m_poAIProcessor->m_dwTargetID = poChar->GetObjID();
	m_llNearestFriendDistance = llDistance;

	return FALSE;

Exit0:
	return TRUE;
}

BOOL CAIProcessor::CScanSabacEnemy::operator()(CCharacter* poChar)
{
	INT64 llDistance;
	CRole* poRole = NULL;

	PROCESS_ERROR(!poChar->IsDead());
	PROCESS_ERROR(!poChar->GetCCFlag().CheckFlag(cclInvincible));
    PROCESS_ERROR(poChar->GetFlag().CheckFlag(cfNeverBeHit) == FALSE);      // redmine 23610, Trap monster

	if (poChar->GetCCFlag().CheckFlag(cclInvisible) && !m_poAIProcessor->IgnoreInvisible())
	{
		PROCESS_ERROR(m_poAIProcessor->CounterInvisible());
		PROCESS_ERROR(m_poAIProcessor->m_poOwner->GetThreatList().CheckThreat(poChar->GetObjID()));
	}

	if (poChar->GetObjType() == otRole)
	{
		CRole* poRole = (CRole*)poChar;
		PROCESS_ERROR(!poRole->IsHidden());
	}

	llDistance = m_poAIProcessor->GetOwner()->GetDistance3D(poChar);
	PROCESS_ERROR(llDistance < m_llNearestEnemyDistance);

	PROCESS_ERROR(m_poAIProcessor->GetOwner()->CanAttack(poChar));

	LOG_PROCESS_ERROR(poChar->GetScene());
	PROCESS_ERROR(!poChar->GetScene()->IsObstacled(m_poAIProcessor->GetOwner(), poChar, FALSE));

	if (poChar->GetObjType() == otRole)
		poRole = (CRole*)poChar;
	else if (poChar->GetObjType() == otNpc || poChar->GetObjType() == otHero)
		poRole = CGameWorld::GetInstance().FindValidRole(poChar->GetHostID());

	if (poRole)
	{
		if (poChar->GetScene()->IsSabacStarted())
		{
			PROCESS_ERROR(poRole->GetGuildID() != CSabacMgr::GetInstance().GetOccupyGuildID(poRole->GetServerGroup()));
		}
		else
		{
			PROCESS_ERROR(poRole->GetGuildID() != 0 && CGuildMgr::GetInstance().IsHostile(
				poRole->GetGuildID(), CSabacMgr::GetInstance().GetOccupyGuildID(poRole->GetServerGroup())));
		}
	}

	m_poAIProcessor->m_dwTargetID = poChar->GetObjID();
	m_llNearestEnemyDistance = llDistance;

Exit0:
	return TRUE;
}

BOOL CAIProcessor::ScanEnemy(void)
{
	CScanEnemy _ScanEnemy;

	_ScanEnemy.m_poAIProcessor = this;
	_ScanEnemy.m_llNearestEnemyDistance = m_ScanNearby.m_llScanEnemyRange;

	_Scan9Region(m_poOwner->GetRegion(), _ScanEnemy);
	PROCESS_ERROR(m_dwTargetID != INVALID_OBJECT_ID);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::ScanNearby(void)
{
	m_dwNearestEnemyID = 0;
	m_dwFarthestEnemyID = 0;
	m_dwCastingEnemyID = 0;
	m_dwNearestFriendID = 0;
	m_dwCorpseCharID = 0;
	m_dwAuraCharID = 0;
	m_dwNearestEnemyRoleID = 0;

	m_ScanNearby.m_llNearestEnemyDistance = m_ScanNearby.m_llScanEnemyRange;
	m_ScanNearby.m_llNearestEnemyRoleDistance = m_ScanNearby.m_llScanEnemyRange;
	m_ScanNearby.m_llFarthestEnemyDistance = 0;
	m_ScanNearby.m_llNearestFriendDistance = m_ScanNearby.m_llScanFriendRange;

	_Scan9Region(m_poOwner->GetRegion(), m_ScanNearby);

	return TRUE;
}

BOOL CAIProcessor::ScanRedName(BOOL bOnlyRedName)
{
	CScanRedName _ScanRedName;

	_ScanRedName.m_poAIProcessor = this;
	_ScanRedName.m_llNearestEnemyDistance = m_ScanNearby.m_llScanEnemyRange;
	_ScanRedName.m_bOnlyRedName = bOnlyRedName;

	_Scan9Region(m_poOwner->GetRegion(), _ScanRedName);
	PROCESS_ERROR(m_dwTargetID != INVALID_OBJECT_ID);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::ScanSabacWinner(void)
{
	CScanSabacWinner _ScanSabacWinner;

	_ScanSabacWinner.m_poAIProcessor = this;
	_ScanSabacWinner.m_llNearestFriendDistance = m_ScanNearby.m_llNearestFriendDistance;

	_Scan9Region(m_poOwner->GetRegion(), _ScanSabacWinner);
	PROCESS_ERROR(m_dwTargetID != INVALID_OBJECT_ID);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::ScanSabacEnemy(void)
{
	CScanSabacEnemy _ScanSabacEnemy;

	_ScanSabacEnemy.m_poAIProcessor = this;
	_ScanSabacEnemy.m_llNearestEnemyDistance = m_ScanNearby.m_llScanEnemyRange;

	_Scan9Region(m_poOwner->GetRegion(), _ScanSabacEnemy);
	PROCESS_ERROR(m_dwTargetID != INVALID_OBJECT_ID);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _PetCheckValidChar(CCharacter *poChar, CCharacter *poOwner)
{
    PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poOwner);

    PROCESS_ERROR(poChar->IsDead() == FALSE);
    PROCESS_ERROR(poChar->GetFlag().CheckFlag(cfNeverBeHit) == FALSE);
    PROCESS_ERROR(poChar->GetScene() == poOwner->GetScene());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CAIProcessor::PetScanEnemy(INT32 nLevel, INT64 llDistance, INT32 nspellEffectGroup)
{
    INT32       nRetCode = 0;
    CRole*      poHost = NULL;
    INT64       llDistance3D = 0;
    UINT32      dwCurrentTime = CGameWorld::GetInstance().GetGameTime();
    BASE_LIST_NODE* pNode = NULL;
    UINT32      dwTargetID = 0;

    LOG_PROCESS_ERROR(m_poOwner);
    poHost = CGameWorld::GetInstance().FindRole(m_poOwner->GetHostID());
    LOG_PROCESS_ERROR(poHost);

    if (poHost->GetLastTargetTimer() + TARGET_WASHOUT_TIME >= dwCurrentTime && poHost->GetLastTargetID() != 0)
    {
        if (GetTypeFromID(poHost->GetLastTargetID()) == otRole)
        {
            CCharacter* poChar = CGameWorld::GetInstance().FindValidCharacter(poHost->GetLastTargetID());
            nRetCode = _PetCheckValidChar(poChar, m_poOwner);
            if (nRetCode == TRUE)
            {
                //DBG("pet scan enemy poHost->GetLastTargetID");
                dwTargetID = poHost->GetLastTargetID();
                PROCESS_SUCCESS(TRUE);
            }
        }
    }

    // first
    if (poHost->GetLastCasterTimer() + CASTER_WASHOUT_TIME >= dwCurrentTime && poHost->GetLastCasterID() != 0)
    {
        CCharacter* poChar = CGameWorld::GetInstance().FindValidCharacter(poHost->GetLastCasterID());
        nRetCode = _PetCheckValidChar(poChar, m_poOwner);
        if (nRetCode == TRUE)
        {
            //DBG("pet scan enemy poHost->GetLastCasterID");
            dwTargetID = poHost->GetLastCasterID();
            PROCESS_SUCCESS(TRUE);
        }
    }

    // second
    for (pNode = poHost->GetThreatList().GetThreatenList().Head().pNext; pNode != &(poHost->GetThreatList().GetThreatenList().Rear()); pNode = pNode->pNext)
    {
        CCharacter* poTargetChar = CGameWorld::GetInstance().FindValidCharacter(((THREAT_NODE*)pNode)->dwCharID);
        if (poTargetChar == NULL)
            continue;
        if (poTargetChar->GetObjID() != m_poOwner->GetObjID())      // Allow a pet to enter combat when its owner attacks it. The idle tree checks whether the target is the owner after this action
        {
            if (poTargetChar->GetObjType() != otNpc)
                continue;
            if (poTargetChar->GetHostID() == poHost->GetObjID())
                continue;
        }
        nRetCode = _PetCheckValidChar(poTargetChar, m_poOwner);
        if (nRetCode == FALSE)
            continue;

        //if (poTargetChar->GetFlag().CheckFlag(cfInvisible))
        //{
        //    if (!poChar->GetAIProcessor()->IgnoreInvisible() && !poChar->GetAIProcessor()->CounterInvisible())
        //        continue;
        //}
        //if (poChar->GetHostID() == poTargetChar->GetObjID())
        //    continue;

        //DBG("pet scan enemy poHost->GetThreatList().GetThreatenList");
        dwTargetID = poTargetChar->GetObjID();
        PROCESS_SUCCESS(TRUE);
    }

    // third
    if (poHost->GetLastTargetTimer() + TARGET_WASHOUT_TIME >= dwCurrentTime && poHost->GetLastTargetID() != 0)
    {
        CCharacter* poChar = CGameWorld::GetInstance().FindValidCharacter(poHost->GetLastTargetID());
        if (poChar && ( (poChar->GetObjType() == otNpc && poChar->GetHostID() != poHost->GetObjID()) 
                                || (poChar->GetObjType() != otNpc)) )
        {
            nRetCode = _PetCheckValidChar(poChar, m_poOwner);
            if (nRetCode == TRUE)
            {
                dwTargetID = poHost->GetLastTargetID();
                PROCESS_SUCCESS(TRUE);
            }
        }
    }

    // fourth
    if (m_poOwner->GetLastCasterTimer() + CASTER_WASHOUT_TIME >= dwCurrentTime && m_poOwner->GetLastCasterID() != 0)
    {
        CCharacter* poTargetChar = CGameWorld::GetInstance().FindValidCharacter(m_poOwner->GetLastCasterID());
        if (poTargetChar != NULL && poTargetChar->GetObjType() == otNpc)
        {
            nRetCode = _PetCheckValidChar(poTargetChar, m_poOwner);
            if (nRetCode == TRUE)
            {
                //DBG("pet scan enemy m_poOwner->GetLastCasterID");
                dwTargetID = m_poOwner->GetLastCasterID();
                PROCESS_SUCCESS(TRUE);
            }
        }
    }

    // fifth
    for (pNode = m_poOwner->GetThreatList().GetThreatenList().Head().pNext; pNode != &(m_poOwner->GetThreatList().GetThreatenList().Rear()); pNode = pNode->pNext)
    {
        CCharacter* poTargetChar = CGameWorld::GetInstance().FindValidCharacter(((THREAT_NODE*)pNode)->dwCharID);
        if (poTargetChar == NULL)
            continue;
        if (poTargetChar->GetObjType() != otNpc)
            continue;
        nRetCode = _PetCheckValidChar(poTargetChar, m_poOwner);
        if (nRetCode == FALSE)
            continue;

        if (((CNpc*)poTargetChar)->GetNpcData() && ((CNpc*)poTargetChar)->GetNpcData()->bCounterInvisible == FALSE)    // Ranged-monster tag described by design...
        {
            dwTargetID = poTargetChar->GetObjID();
            continue;
        }

        //DBG("pet scan enemy m_poOwner->GetThreatList().GetThreatenList");
        dwTargetID = poTargetChar->GetObjID();
        PROCESS_SUCCESS(TRUE);
    }

    // sixth
    if (poHost->GetPetCount() > 1)
    {
        for (INT32 i = 0; i < poHost->GetPetCount(); ++i)
        {
            UINT32  dwBrotherID = poHost->GetPetNpcID(i);
            if (dwBrotherID == m_poOwner->GetObjID())
                continue;

            CNpc  *poBrother = CGameWorld::GetInstance().FindNpc(dwBrotherID);
            if (poBrother && poBrother->GetAIProcessor() && poBrother->GetAIProcessor()->GetTargetID() > 0)
            {
                CCharacter* poTargetChar = CGameWorld::GetInstance().FindValidCharacter(poBrother->GetAIProcessor()->GetTargetID());
                if (poTargetChar && poTargetChar->GetHostID() != poHost->GetObjID())
                {
                    dwTargetID = poBrother->GetAIProcessor()->GetTargetID();
                    PROCESS_SUCCESS(TRUE);
                }

            }
        }
    }

    // seventh
    CPetScanEnemy _PetScanEnemy;
    llDistance3D = llDistance * llDistance;

    _PetScanEnemy.m_dwTargetID = 0;
    _PetScanEnemy.m_poAIProcessor = this;
    _PetScanEnemy.m_llNearestEnemyDistance = llDistance3D;
    _PetScanEnemy.m_llScanEnemyRange = llDistance3D;
    _PetScanEnemy.m_nLimitLevel = nLevel;
    _PetScanEnemy.m_nspellEffectGroup = nspellEffectGroup;

    _Scan9Region(m_poOwner->GetRegion(), _PetScanEnemy);
    PROCESS_ERROR(_PetScanEnemy.m_dwTargetID != INVALID_OBJECT_ID);
    dwTargetID = _PetScanEnemy.m_dwTargetID;
    PROCESS_SUCCESS(TRUE);

Exit0:
    return FALSE;
Exit1:
    m_poOwner->GetAIProcessor()->SetTargetID(dwTargetID);
    return TRUE;

}

BOOL CAIProcessor::PetWander(INT32 nWanderRange, INT32 nWanderInterval)
{
    INT32 nRetCode = 0;
    INT32 nDirection8 = 0;
    INT64 llDestX, llDestY;
    CCharacter  *poHost = NULL;

    LOG_PROCESS_ERROR(nWanderRange > 0);
    LOG_PROCESS_ERROR(m_poOwner);
    poHost = CGameWorld::GetInstance().FindRole(m_poOwner->GetHostID());
    LOG_PROCESS_ERROR(poHost);

    PROCESS_SUCCESS(m_nMoveTimer > CGameWorld::GetInstance().GetGameFrame());

    if (m_poOwner->GetDistance3D(poHost->GetX(), poHost->GetY(), poHost->GetZ()) >= (INT64)nWanderRange * nWanderRange)
        nDirection8 = ConvertToDir8(g_GetDirection(m_poOwner->GetX(), m_poOwner->GetY(), poHost->GetX(), poHost->GetY()));
    else
        nDirection8 = g_Random(DT_DIRECTION_8_COUNT);
    llDestX = m_poOwner->GetX() + g_nAdjacentX[nDirection8] * GRID_WIDTH;
    llDestY = m_poOwner->GetY() + g_nAdjacentY[nDirection8] * GRID_WIDTH;

    nRetCode = m_poOwner->MoveTo(llDestX, llDestY);
    LOG_PROCESS_ERROR(nRetCode);

    m_nMoveTimer = CGameWorld::GetInstance().GetGameFrame() + g_Random(m_poOwner->GetAttr(catWalkSpeed), nWanderInterval);
    m_nAIMoveState = amsWander;

Exit1:
    return TRUE;
Exit0:
    Stop();
    return FALSE;
}

BOOL CAIProcessor::Reload(void)
{
	INT32 nRetCode = FALSE;

	m_BTCtx.callFrameCount = 0;
	m_BTOnDieCtx.callFrameCount = 0;

	const NEW_AI_DATA* pAIData = CDataManager<NEW_AI_DATA>::GetInstance().GetData(m_nAIID);
	LOG_PROCESS_ERROR(pAIData);

    if (_GetExecBT(evtAIInit))
    {
        nRetCode = _RunBT(evtAIInit, 0, TRUE);
        LOG_PROCESS_ERROR(nRetCode);
    }

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAIProcessor::CallDeadBT(UINT32 dwKillerRoleID)
{
	INT32 nRetCode = 0;
	BTNod* pNod = _GetExecBT(evtAIDie);

	LOG_PROCESS_ERROR(m_nAIFightState != afsDying);

	if (pNod)
	{
		m_nAIFightState = afsDying;

		memset(m_BTOnDieCtx.callFrame, 0, sizeof(m_BTOnDieCtx.callFrame));
		m_BTOnDieCtx.callFrame[0].nod = pNod;
		LOG_PROCESS_ERROR(m_BTOnDieCtx.callFrame[0].nod);

		m_BTOnDieCtx.callFrameCount = 1;
		m_BTOnDieCtx.debugging = 0;

		m_BTOnDieCtx.dwRoleID = dwKillerRoleID;
		m_BTOnDieCtx.eEventType = evtAIDie;

		BTCtx_run(m_BTOnDieCtx);

		PROCESS_SUCCESS(m_BTOnDieCtx.returnValue == brvRunning);

		m_nAIFightState = afsFight;
	}
Exit1:
	return TRUE;
Exit0:
	CRI("AI %d dead tree brvError", m_nAIID);
	return FALSE;
}

void CAIProcessor::CallReviveBT()
{
	INT32 nRetCode = 0;

	if (_GetExecBT(evtAIRevive))
	{
		nRetCode = _RunBT(evtAIRevive, 0, TRUE);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return;
Exit0:

	DBG("AI %d revive tree brvError", m_nAIID );
	return;
}

void CAIProcessor::CallInitBT(void)
{
	INT32 nRetCode = 0;

	if (_GetExecBT(evtAIInit))
	{
		nRetCode = _RunBT(evtAIInit, 0, TRUE);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return;
Exit0:

	CRI("AI %d revive tree brvError", m_nAIID);
	return;
}

void CAIProcessor::SetInteractState(BOOL bEnabled)
{
    if (bEnabled)
        m_nInteractTimer = CGameWorld::GetInstance().GetGameFrame() + AI_INTERACT_TIMEOUT;
    else
        m_nInteractTimer = 0;
}

BTNod* CAIProcessor::GetInteractBT()
{
	return _GetExecBT(evtAIInteract);
}

BOOL CAIProcessor::ProcessInteractSyncRun(BTCustomCtx& rCtx)
{
	INT32 nRetCode = 0;

	nRetCode = _ProcessSyncRun(rCtx);
	PROCESS_SUCCESS(nRetCode == srrSyncRunning);

	if (nRetCode == srrSyncRunFinished)
	{
		BTCtx_run(rCtx);
	}

Exit1:
	return TRUE;
Exit0:
	rCtx.eRunningType = brtInvalid;
	return FALSE;
}

INT32 CAIProcessor::_ProcessSyncRun(BTCustomCtx& rCtx)
{
	INT32 nRetCode = srrNoSyncRun;

	LOG_PROCESS_ERROR( m_poOwner );

	switch( rCtx.eRunningType )
	{
	case brtInvalid:
		{
			nRetCode = srrNoSyncRun;
		}
		break;
	case brtWait:
		if (CGameWorld::GetInstance().GetGameFrame() < rCtx.nWaitFrame)
		{
			nRetCode = srrSyncRunning;
		}
		else
		{
			rCtx.eRunningType = brtInvalid;
			rCtx.returnValue = brvSuccess;   // timer out, action success
			nRetCode =  srrSyncRunFinished;
		}
		break;
	case brtSyncRotate:
		if (m_poOwner->IsRotating())
		{
			nRetCode = srrSyncRunning;
		}
		else
		{
			rCtx.eRunningType = brtInvalid;
			rCtx.returnValue = brvSuccess;   // movement finish, action success
			nRetCode = srrSyncRunFinished;
		}
		break;
	case brtSyncMove:
		{
			if( CMS_STAND == m_poOwner->GetMoveState() )
			{
				rCtx.eRunningType = brtInvalid;
				rCtx.returnValue = brvSuccess;   // movement finish, action success
				nRetCode = srrSyncRunFinished;
			}
			else
			{
				nRetCode = srrSyncRunning;
			}
		}
		break;
	case brtStepAround:
		{
			if ( //m_nAIMoveState == amsStop || 
				m_poOwner->GetMoveState() == CMS_STAND )
			{
				CCharacter* poTarget = CGameWorld::GetInstance().FindValidCharacter(m_dwTargetID);
				PROCESS_ERROR(poTarget);
				INT32 nNewDirection = g_GetDirection(m_poOwner->GetX(), m_poOwner->GetY(), poTarget->GetX(), poTarget->GetY());
				m_poOwner->RotateTo( NormalizeDirection(nNewDirection), 0 );
				//DBG("StepAround finished, then rotate to %d", nNewDirection );

				rCtx.eRunningType = brtInvalid;
				rCtx.returnValue = brvSuccess;   // movement finish, action success
				nRetCode = srrSyncRunFinished;
			}
			else
			{
				nRetCode = srrSyncRunning;
			}
		}
		break;
	case brtWaitSelect:
		if (CGameWorld::GetInstance().GetGameFrame() < rCtx.nWaitFrame)
		{
			nRetCode = srrSyncRunning;
		}
		else
		{
			rCtx.callFrameCount = 0;
			rCtx.eRunningType = brtInvalid;
			rCtx.returnValue = brvSuccess;
			nRetCode = srrSyncRunFinished;
		}
		break;
	case brtWaitBreaking:
		{
			INT32 i = 0;
		}
	case brtWaitCasting:
		if (m_poOwner->GetCastingSpell())
		{
			nRetCode = srrSyncRunning;
		}
		else
		{
			rCtx.eRunningType = brtInvalid;
			rCtx.returnValue = brvSuccess;
			nRetCode = srrSyncRunFinished;
		}
		break;
    case brtWaitMSGuildData:
    {
        if (CGameWorld::GetInstance().GetGameFrame() < rCtx.nWaitFrame)
        {
            nRetCode = srrSyncRunning;
        }
        else
        {
            rCtx.callFrameCount = 0;
            rCtx.eRunningType = brtInvalid;
            rCtx.returnValue = brvFail;
            nRetCode = srrSyncRunFinished;
        }
    }
        break;
	case brtWaitMessage:
#ifdef _ROBOT
		if (m_nRegisteredMsgID == 0)
		{
			rCtx.eRunningType = brtInvalid;
			rCtx.returnValue = brvSuccess;
			nRetCode = srrSyncRunFinished;
		}
		else if (CGameWorld::GetInstance().GetGameFrame() >= rCtx.nWaitFrame)
		{
			rCtx.eRunningType = brtInvalid;
			rCtx.returnValue = brvFail;
			nRetCode = srrSyncRunFinished;
		}
		else
		{
			nRetCode = srrSyncRunning;
		}
		break;
#endif	// _ROBOT
	default:
		CRI("unknown bt ctx running type: %d", rCtx.eRunningType);
		break;
	}

Exit0:
	return nRetCode;
}

void CAIProcessor::PrintAllBT()
{
	for ( INT32 nIndex = evtAIBegin; nIndex < evtAIEnd; ++nIndex )
	{
		if( !_GetExecBT( nIndex ) )
			continue;
		INF( "bt %s:", g_BTEventName[nIndex]);
		CBTManager::GetInstance().PrintBT( _GetExecBT( nIndex ), 0 );
	}
}

#ifdef _ROBOT
BOOL CAIProcessor::RegisterMsg(INT32 nMsgID)
{
	LOG_PROCESS_ERROR(m_nRegisteredMsgID == 0);

	m_nRegisteredMsgID = nMsgID;

	return TRUE;
Exit0:
	return FALSE;
}

void CAIProcessor::CheckMsgID(INT32 nMsgID)
{
	if (m_nRegisteredMsgID == nMsgID)
		m_nRegisteredMsgID = 0;
}

#endif	// _ROBOT

inline BOOL CAIProcessor::_RunBT(INT32 nEventType, UINT32 dwRoleID, BOOL bForce)
{
	INT32 nRetCode = 0;

	if (bForce || m_BTCtx.callFrameCount == 0)
	{
		memset(m_BTCtx.callFrame, 0, sizeof(m_BTCtx.callFrame) );
		m_BTCtx.callFrame[0].nod = _GetExecBT(nEventType);
		LOG_PROCESS_ERROR(m_BTCtx.callFrame[0].nod);

		m_BTCtx.callFrameCount = 1;
		m_BTCtx.debugging = 0;

        //if (m_nAIID == 9290 || m_nAIID == 9291 || m_nAIID == 9293 || m_nAIID == 9294)
        //{
        //    m_BTCtx.debugging = 1;
        //}

		m_BTCtx.dwRoleID = dwRoleID;
		m_BTCtx.eEventType = nEventType;
	}

	BTCtx_run(m_BTCtx);

	return TRUE;
Exit0:

	return FALSE;
}

BTNod* CAIProcessor::_GetExecBT(INT32 nEventType)
{
    const AI_TEMPLATE* pAITemplate = CAIManager::GetInstance().GetTemplate(m_nAIID);
    LOG_PROCESS_ERROR(pAITemplate);
    LOG_PROCESS_ERROR(nEventType >= evtAIBegin && nEventType < evtAIEnd);

    return pAITemplate->pTreeList[nEventType - evtAIBegin];
Exit0:
    return NULL;
}
