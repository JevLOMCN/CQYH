#include "stdafx.h"
#include "message_handler.h"

#include "zlib.h"

#include "c2g_message.h"
#include "g2c_message.h"
#include "g2m_message.h"
#include "m2g_message.h"

#include "error_def.h"
#include "imessage_notifier.h"
#include "game_world.h"
#include "role.h"
#include "npc_data.h"
#include "doodad_data.h"
#include "scene_file_mgr.h"
#include "missile.h"
#include "team.h"
#include "guild_mgr.h"
#include "dungeon_data.h"

#ifndef _ROBOT

using namespace ExternalMessage;
using namespace InternalMessage;

void CCliMessageHandler::OnRoleStop(CRole* poRole, const char* pData)
{
    INT32 nRetCode = 0;
    C2G_ROLE_STOP* pMsg = (C2G_ROLE_STOP*)pData;

    nRetCode = poRole->Stand(FALSE);
    PROCESS_ERROR(nRetCode);

    return;
Exit0:
    poRole->ErrorCodeNotify( errMoveFail, 0, 0);

    return;
}

void CCliMessageHandler::OnRoleRotate(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_ROLE_ROTATE* pMsg = (C2G_ROLE_ROTATE*)pData;

	poRole->RotateToDir8(pMsg->wFaceDirection, CONVERT_TIME(pMsg->nDuration));

	return;
Exit0:
    KICK_ROLE(poRole, ktLogged);

	return;
}

void CCliMessageHandler::OnRoleWalk(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_ROLE_WALK* pMsg = (C2G_ROLE_WALK*)pData;

	nRetCode = poRole->WalkTo(ConvertCoordN2S(pMsg->wDestX), ConvertCoordN2S(pMsg->wDestY));
	PROCESS_ERROR(nRetCode);

	return;
Exit0:
	poRole->ErrorCodeNotify( errMoveFail, 0, 0);

	return;
}

void CCliMessageHandler::OnRoleRun(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_ROLE_RUN* pMsg = (C2G_ROLE_RUN*)pData;

	PROCESS_ERROR(!poRole->GetCCFlag().CheckFlag(cclSlowed));

	nRetCode = poRole->RunTo(ConvertCoordN2S(pMsg->wDestX), ConvertCoordN2S(pMsg->wDestY));
	PROCESS_ERROR(nRetCode);

	return;
Exit0:
	poRole->ErrorCodeNotify( errMoveFail, 0, 0);

	return;
}

void CCliMessageHandler::OnUpdatePos(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_UPDATE_POS* pMsg = (C2G_UPDATE_POS*)pData;
	INT64 llMaxDistance = 0;
    INT64 llDistanceDiff = 0;
	BOOL bNeedForceSyncPos = FALSE;
	CScene* pScene = NULL;
    INT32 nCurrentX = ConvertCoordN2S(pMsg->wCurrentX);
    INT32 nCurrentY = ConvertCoordN2S(pMsg->wCurrentY);
    INT32 nCurrentZ = ConvertCoordN2S(pMsg->wCurrentZ);

	if (poRole->GetMoveState() >=  CMS_ACTIVE_MOVE &&
		poRole->GetMoveState() < CMS_PASSIVE_MOVE)
	{
		llDistanceDiff = g_GetDistance2D(poRole->GetX(), poRole->GetY(), nCurrentX, nCurrentY);
		llMaxDistance = g_GWConfig.nForceSyncDistance * POINT_PER_CM;
		llMaxDistance *= llMaxDistance;

		if(llDistanceDiff > llMaxDistance)
		{
			INT32 nPosDirection = g_GetDirection(poRole->GetX(), poRole->GetY(), nCurrentX, nCurrentY);

			WRN("[%4d] role %s:%d update pos (%.2f, %.2f, %.2f), server current pos (%.2f, %.2f, %.2f), distance %d direction %d" , 
				CGameWorld::GetInstance().GetGameFrame(), poRole->GetName(), poRole->GetObjID(),
                nCurrentX * 1.0 / POINT_PER_CM, nCurrentY * 1.0 / POINT_PER_CM, nCurrentZ * 1.0 / POINT_PER_CM,
				poRole->GetX() * 1.0 / POINT_PER_CM, poRole->GetY() * 1.0 / POINT_PER_CM, poRole->GetZ() * 1.0 / POINT_PER_CM, 
				(INT32)(sqrt((float)llDistanceDiff) / POINT_PER_CM), nPosDirection * 360 / MAX_DIRECTION_COUNT);

            bNeedForceSyncPos = TRUE;
		}

		pScene = poRole->GetScene();
		LOG_PROCESS_ERROR(pScene);

		if (abs((INT32)(poRole->GetZ() - nCurrentZ)) > 8 * POINT_PER_METER)
		{
			WRN("[%4d] role %s:%d update pos (%.2f, %.2f, %.2f), server z (%.2f), different(%.2f)" , 
				CGameWorld::GetInstance().GetGameFrame(), poRole->GetName(), poRole->GetObjID(),
                nCurrentX * 1.0 / POINT_PER_CM, nCurrentY * 1.0 / POINT_PER_CM, nCurrentZ * 1.0 / POINT_PER_CM,
				poRole->GetZ() * 1.0 / POINT_PER_CM, (poRole->GetZ() - nCurrentZ) * 1.0f / POINT_PER_CM);
			bNeedForceSyncPos = TRUE;
		}
	}

	if (bNeedForceSyncPos)
		poRole->ForceSyncPos();

Exit1:
	return;
Exit0:

	return;
}

void CCliMessageHandler::OnPing(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
    C2G_PING* pMsg = (C2G_PING*)pData;

    nRetCode = DoPingAck(poRole, pMsg->dwPingTick);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CCliMessageHandler::OnQueryCharData(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_QUERY_CHAR_DATA* pQueryCharData = (C2G_QUERY_CHAR_DATA*)pData;
	INT32 eObjectType = 0;

	if (GetTypeFromID(pQueryCharData->dwCharID) == otRole)
	{
		CRole* poDataRole = CGameWorld::GetInstance().FindRole(pQueryCharData->dwCharID);
		PROCESS_SUCCESS(poDataRole == NULL);

		nRetCode = CCliMessageHandler::GetInstance().DoSyncRoleData(poRole, poDataRole, pQueryCharData->dwCRC);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = CCliMessageHandler::GetInstance().DoSyncCharAuraBrief(poRole, poDataRole);
		PROCESS_ERROR(nRetCode);
	}
	else if (GetTypeFromID(pQueryCharData->dwCharID) == otNpc)
	{
		CNpc* poDataNpc = CGameWorld::GetInstance().FindNpc(pQueryCharData->dwCharID);
		PROCESS_SUCCESS(poDataNpc == NULL);

        if (poDataNpc->GetHostID() == 0)
        {
            nRetCode = CCliMessageHandler::GetInstance().DoSyncNpcData(poRole, poDataNpc, pQueryCharData->dwCRC);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            nRetCode = CCliMessageHandler::GetInstance().DoSyncPetData(poRole, poDataNpc, pQueryCharData->dwCRC);
            LOG_CHECK_ERROR(nRetCode);
        }

		nRetCode = CCliMessageHandler::GetInstance().DoSyncCharAuraBrief(poRole, poDataNpc);
		PROCESS_ERROR(nRetCode);
	}
	else if (GetTypeFromID(pQueryCharData->dwCharID) == otHero)
	{
		CHero* poDataHero = CGameWorld::GetInstance().FindHero(pQueryCharData->dwCharID);
		PROCESS_SUCCESS(poDataHero == NULL);

		nRetCode = CCliMessageHandler::GetInstance().DoSyncHeroData(poRole, poDataHero, pQueryCharData->dwCRC);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = CCliMessageHandler::GetInstance().DoSyncCharAuraBrief(poRole, poDataHero);
		PROCESS_ERROR(nRetCode);
	}


Exit1:
	return;
Exit0:

	return;
}

void CCliMessageHandler::OnQueryRoleDetailedData(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	C2G_QUERY_ROLE_DETAIL_DATA* pMsg = (C2G_QUERY_ROLE_DETAIL_DATA*)pData;

	CRole* poTargetRole = CGameWorld::GetInstance().FindValidRole(pMsg->dwRoleID);
	LOG_PROCESS_ERROR_RET_CODE(poTargetRole, errRoleNotOnline);

	//PROCESS_ERROR(poRole->GetDistance3D(poTargetRole) <= REGION_WIDTH * REGION_WIDTH);

	nRetCode = CCliMessageHandler::GetInstance().DoSyncRoleAttrData(poRole, poTargetRole, pMsg->dwAttrCRC);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoSyncRoleEquipData(poRole, poTargetRole, pMsg->dwEquipCRC);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoSyncRoleSpiritData(poRole, poTargetRole, pMsg->dwSpiritCRC);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoSyncRoleSpellData(poRole, poTargetRole, pMsg->dwSpellCRC);
    LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (nResult != errUnknownError)
	{
		poRole->ErrorCodeNotify(nResult, 0, 0);
	}

	return;
}

void CCliMessageHandler::OnQueryRolePow(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_QUERY_ROLE_POW* pMsg = (C2G_QUERY_ROLE_POW*)pData;

	CRole* poTargetRole = CGameWorld::GetInstance().FindValidRole(pMsg->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	PROCESS_ERROR(poRole->GetDistance3D(poTargetRole) <= REGION_WIDTH * REGION_WIDTH);

	nRetCode = CCliMessageHandler::GetInstance().DoSyncRolePow(poRole, poTargetRole);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CCliMessageHandler::OnCancelCutScene(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_CANCEL_CUTSCENE* pMsg = (C2G_CANCEL_CUTSCENE*)pData;

	poRole->SetInCutScene(FALSE, 0);
Exit0:
	return;
}

void CCliMessageHandler::OnRoleTeleport(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_ROLE_TELEPORT* pMsg = (C2G_ROLE_TELEPORT*)pData;
	INT32 nDestX = pMsg->wDestX * 100;
	INT32 nDestY = pMsg->wDestY * 100;
	INT32 nDestZ = 0;

	LOG_PROCESS_ERROR(poRole->GetScene());
	LOG_PROCESS_ERROR(poRole->GetScene()->GetSceneData());
	LOG_PROCESS_ERROR(!poRole->GetScene()->GetSceneData()->bCannotUseTeleportRing);

	LOG_PROCESS_ERROR(poRole->GetCCFlag().CheckFlag(cclTransportationRing));
	PROCESS_ERROR_RET_CODE(poRole->GetCoolDown().CheckReady(MakeCoolDownID(COOLDOWN_SPECIAL_ITEM, cclTransportationRing)), errItemNotReady);

	ConvertCoordO2N(nDestX, nDestY, nDestZ);

	nRetCode = poRole->JumpTo(nDestX * POINT_PER_CM, nDestY * POINT_PER_CM, 100000 * POINT_PER_CM, poRole->GetFaceDirection());
	PROCESS_ERROR_RET_CODE(nRetCode, errJumpWrongPos);

	nRetCode = poRole->GetCoolDown().Add(MakeCoolDownID(COOLDOWN_SPECIAL_ITEM, cclTransportationRing),
		g_GWConfig.nTeleportRingCDTime * GAME_FPS / 1000);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);

	return;
}

void CCliMessageHandler::OnQueryRoleLocation(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_QUERY_ROLE_LOCATION* pMsg = (C2G_QUERY_ROLE_LOCATION*)pData;

	nRetCode = poRole->QueryRoleLocation(pMsg->szRoleName);
	PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);

	return;
}

void CCliMessageHandler::OnRecallTeamMember(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_RECALL_TEAM_MEMBER* pMsg = (C2G_RECALL_TEAM_MEMBER*)pData;

	nRetCode = poRole->RecallTeamMember();
	PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);

	return;
}

void CCliMessageHandler::OnRoleMine(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_ROLE_MINE* pMsg = (C2G_ROLE_MINE*)pData;

	nRetCode = poRole->Mine(ConvertCoordN2S(pMsg->wDestX), ConvertCoordN2S(pMsg->wDestY));
	PROCESS_ERROR(nRetCode);

	return;
Exit0:
	poRole->ErrorCodeNotify( errMineFail, 0, 0);

	return;
}

BOOL CCliMessageHandler::DoSyncFrame(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_FRAME* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_FRAME*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_FRAME));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_frame;
	pMsg->nFrame = CGameWorld::GetInstance().GetGameFrame();

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_FRAME));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoPingAck(CRole* poRole, UINT32 dwPingTick)
{
    INT32 nRetCode = 0;
    G2C_PING_ACK* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_PING_ACK*)m_poPacketAllocator->Alloc(sizeof(G2C_PING_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_ping_ack;
    pMsg->dwPingTick = dwPingTick;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_PING_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoEnterScene(CRole* poRole, CScene* poScene)
{
	INT32 nRetCode = 0;
	G2C_ENTER_SCENE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poScene);

	pMsg = (G2C_ENTER_SCENE*)m_poPacketAllocator->Alloc(sizeof(G2C_ENTER_SCENE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_enter_scene;
	pMsg->nSceneTempID = poScene->GetSceneTempID();
	pMsg->nSceneID = poScene->GetSceneID();
	pMsg->byWeather = (UINT8)poScene->GetWeather();
	pMsg->wX = ConvertCoordS2N(poRole->GetX());
	pMsg->wY = ConvertCoordS2N(poRole->GetY());
	pMsg->wZ = ConvertCoordS2N(poRole->GetZ());

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ENTER_SCENE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoLeaveScene(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_LEAVE_SCENE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_LEAVE_SCENE*)m_poPacketAllocator->Alloc(sizeof(G2C_LEAVE_SCENE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_leave_scene;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_LEAVE_SCENE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncNextJumpPoint(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_NEXT_JUMP_POINT* pMsg = NULL;
	INT32 nX, nY, nZ;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_NEXT_JUMP_POINT*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_NEXT_JUMP_POINT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_next_jump_point;

	poRole->GetNextJumpPoint(nX, nY, nZ);
	pMsg->wJumpX = ConvertCoordS2N(nX);
	pMsg->wJumpY = ConvertCoordS2N(nY);
	pMsg->wJumpZ = ConvertCoordS2N(nZ);

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_NEXT_JUMP_POINT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncDoorState(CRole* poRole, CScene* poScene)
{
	INT32 nRetCode = 0;
	G2C_SYNC_DOOR_STATE* pMsg = NULL;

	pMsg = (G2C_SYNC_DOOR_STATE*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_DOOR_STATE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_door_state;
	pMsg->qwDoorState = poScene->GetDoorState();

	if (poRole)
	{
		nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_DOOR_STATE));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = _BroadcastScene(poScene, (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_DOOR_STATE));
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncRolePos(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_ROLE_POS* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_ROLE_POS*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_ROLE_POS));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_role_pos;
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->wCurrentX = ConvertCoordS2N(poRole->GetX());
	pMsg->wCurrentY = ConvertCoordS2N(poRole->GetY());
	pMsg->wCurrentZ = ConvertCoordS2N(poRole->GetZ());
    pMsg->wFaceDirection = (UINT16)poRole->GetFaceDirection();
    pMsg->byMoveState = (UINT8)poRole->GetMoveState();

	nRetCode = _Broadcast9Region(poRole->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_ROLE_POS));
	LOG_PROCESS_ERROR(nRetCode);

	//DBG("[%4d]role %d sync pos (%d, %d, %d)", CGameWorld::GetInstance().GetGameFrame(), poRole->GetObjID(), 
	//	(INT32)(poRole->GetX() / POINT_PER_CM), (INT32)(poRole->GetY() / POINT_PER_CM), (INT32)(poRole->GetZ() / POINT_PER_CM));

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharWalk(CCharacter* poChar)
{
	INT32 nRetCode = 0;
	G2C_CHAR_WALK* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_CHAR_WALK*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_WALK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_walk;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->wSpeed = poChar->GetAttr(catWalkSpeed);
	pMsg->wDestX = ConvertCoordS2N(poChar->GetX());
	pMsg->wDestY = ConvertCoordS2N(poChar->GetY());

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_WALK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharRun(CCharacter* poChar)
{
	INT32 nRetCode = 0;
	G2C_CHAR_RUN* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_CHAR_RUN*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_RUN));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_run;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->wSpeed = poChar->GetAttr(catRunSpeed);
	pMsg->wDestX = ConvertCoordS2N(poChar->GetX());
	pMsg->wDestY = ConvertCoordS2N(poChar->GetY());

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_RUN));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharStop(CCharacter* poChar, BOOL bForce)
{
    INT32 nRetCode = 0;
    G2C_CHAR_STOP* pMsg = NULL;

    LOG_PROCESS_ERROR(poChar);

    pMsg = (G2C_CHAR_STOP*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_STOP));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_char_stop;
    pMsg->dwCharID = poChar->GetObjID();
    pMsg->byForce = bForce;
    pMsg->wCurrentX = ConvertCoordS2N(poChar->GetX());
    pMsg->wCurrentY = ConvertCoordS2N(poChar->GetY());
    pMsg->wCurrentZ = ConvertCoordS2N(poChar->GetZ());

    nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_STOP));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoCharPassiveMove(CCharacter* poChar, INT32 nDuration)
{
	INT32 nRetCode = 0;
	G2C_CHAR_PASSIVE_MOVE* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_CHAR_PASSIVE_MOVE*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_PASSIVE_MOVE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_passive_move;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->byMoveState = poChar->GetMoveState();
	pMsg->wDestX = ConvertCoordS2N(poChar->GetX());
	pMsg->wDestY = ConvertCoordS2N(poChar->GetY());
    pMsg->wFaceDirection = (UINT16)poChar->GetFaceDirection();

	pMsg->wDuration = nDuration;

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_PASSIVE_MOVE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharFreeze(CCharacter* poCharacter, BOOL bFreeze)
{
	INT32 nRetCode = 0;
	G2C_CHAR_FREEZE* pMsg = NULL;

	LOG_PROCESS_ERROR(poCharacter);

	pMsg = (G2C_CHAR_FREEZE*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_FREEZE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_freeze;
	pMsg->dwCharID = poCharacter->GetObjID();
	pMsg->byFreezed = bFreeze;
	pMsg->wCurrentX = ConvertCoordS2N(poCharacter->GetX());
	pMsg->wCurrentY = ConvertCoordS2N(poCharacter->GetY());
	pMsg->wCurrentZ = ConvertCoordS2N(poCharacter->GetZ());

	nRetCode = _Broadcast9Region(poCharacter->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_FREEZE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharClientShowTime(CCharacter* poCharacter, BOOL bShowTime)
{
	INT32 nRetCode = 0;
	G2C_CHAR_CLIENT_SHOW_TIME* pMsg = NULL;

	LOG_PROCESS_ERROR(poCharacter);

	pMsg = (G2C_CHAR_CLIENT_SHOW_TIME*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_CLIENT_SHOW_TIME));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_client_show_time;
	pMsg->dwCharID = poCharacter->GetObjID();
	pMsg->byShowTime = bShowTime;
    pMsg->wCurrentX = ConvertCoordS2N(poCharacter->GetX());
    pMsg->wCurrentY = ConvertCoordS2N(poCharacter->GetY());
    pMsg->wCurrentZ = ConvertCoordS2N(poCharacter->GetZ());

	nRetCode = _Broadcast9Region(poCharacter->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_CLIENT_SHOW_TIME));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}


BOOL CCliMessageHandler::DoCharFear(CCharacter* poCharacter, BOOL bFreeze)
{

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoCharHard(CCharacter* poCharacter, INT32 nDuration)
{
	INT32 nRetCode = 0;
	G2C_CHAR_HARD* pMsg = NULL;

	LOG_PROCESS_ERROR(poCharacter);

	pMsg = (G2C_CHAR_HARD*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_HARD));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_hard;
	pMsg->dwCharID = poCharacter->GetObjID();
	pMsg->wDuration = nDuration;

	nRetCode = _Broadcast9Region(poCharacter->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_HARD));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncFlagPos(CCharacter* poCharacter)
{
	INT32 nRetCode = 0;
	G2C_SYNC_FLAG_POS* pMsg = NULL;
    CRole* poRole = NULL;

	LOG_PROCESS_ERROR(poCharacter);
    LOG_PROCESS_ERROR(poCharacter->GetObjType() == otRole);

    poRole = (CRole*)poCharacter;

	pMsg = (G2C_SYNC_FLAG_POS*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_FLAG_POS));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_flag_pos;
	pMsg->byFaction = poRole->GetFaction();
    pMsg->dwRoleID = poRole->GetObjID();
    pMsg->wCurrentX = ConvertCoordS2N(poRole->GetX());
    pMsg->wCurrentY = ConvertCoordS2N(poRole->GetY());
    pMsg->wCurrentZ = ConvertCoordS2N(poRole->GetZ());

	nRetCode = _BroadcastScene(poRole->GetScene(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_FLAG_POS));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoObjectRotate(CSceneObject* poObj, INT32 nDestDirection, INT32 nDuration)
{
	INT32 nRetCode = 0;
	G2C_OBJECT_ROTATE* pMsg = NULL;

	LOG_PROCESS_ERROR(poObj);

	pMsg = (G2C_OBJECT_ROTATE*)m_poPacketAllocator->Alloc(sizeof(G2C_OBJECT_ROTATE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_object_rotate;
	pMsg->dwCharID = poObj->GetObjID();
	pMsg->wDuration = nDuration;
	pMsg->wFaceDirection = nDestDirection;

	nRetCode = _Broadcast9Region(poObj->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_OBJECT_ROTATE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


BOOL CCliMessageHandler::DoCharAppear(CCharacter* poCharacter, CRegion* poDestRegion, INT32 nAppearType)
{
	INT32 nRetCode = 0;
	INT32 nMaxHp = 0;
	G2C_CHAR_APPEAR* pMsg= NULL;

	LOG_PROCESS_ERROR(poCharacter);

	pMsg = (G2C_CHAR_APPEAR*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_APPEAR));
	LOG_PROCESS_ERROR(pMsg);

	// send char appear message
	pMsg->wMsg = g2c_char_appear;
	pMsg->byAppearType = nAppearType;
	pMsg->stAppearData.dwCharID = poCharacter->GetObjID();
	pMsg->stAppearData.byMoveState = poCharacter->GetMoveState();
	pMsg->stAppearData.wCurrentX = ConvertCoordS2N(poCharacter->GetX());
	pMsg->stAppearData.wCurrentY = ConvertCoordS2N(poCharacter->GetY());
	pMsg->stAppearData.wCurrentZ = ConvertCoordS2N(poCharacter->GetZ());
	pMsg->stAppearData.wFaceDirection = (UINT16)poCharacter->GetFaceDirection();
	pMsg->stAppearData.byCombatPoseOn = poCharacter->GetFlag().CheckFlag(cfCombatPoseOn);

	nMaxHp = poCharacter->GetAttrList().GetAttr(catMaxHp);
	pMsg->stAppearData.byLifePercent = nMaxHp > 0 ? (UINT8)(poCharacter->GetHp() * 100 / nMaxHp) : 0;

	pMsg->stAppearData.byMountID = poCharacter->GetCurrentMountID();
	pMsg->stAppearData.wWingID = (UINT16)poCharacter->GetCurrentWingID();

    if (poCharacter->GetObjType() == otNpc)
        pMsg->stAppearData.bPet = (((CNpc*)poCharacter)->GetPetType() > 0 ? 1 : 0);
    else
        pMsg->stAppearData.bPet = 0;

	if (poCharacter->GetObjType() == otRole)
		pMsg->stAppearData.bGreyName = ((CRole*)poCharacter)->IsGreyName();

	if (poDestRegion == NULL)
	{
		nRetCode = _Broadcast9Region(poCharacter->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_APPEAR));
		LOG_PROCESS_ERROR(nRetCode);

		//        DBG("char %d appear @ center region(%I64d, %I64d)", poCharacter->GetObjID(), 
		//            poCharacter->GetX() / REGION_WIDTH, poCharacter->GetY() / REGION_WIDTH);
	}
	else
	{
		nRetCode = _BroadcastRegion(poDestRegion, (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_APPEAR));
		LOG_PROCESS_ERROR(nRetCode);

		//        DBG("char %d appear @ region(%d, %d)", poCharacter->GetObjID(), 
		//            poDestRegion->GetRegionX(), poDestRegion->GetRegionY());
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


struct CTraverseChar
{
	G2C_SYNC_CHAR_LIST* SyncCharList;
	INT32 nCount;

	BOOL operator()(CRole* poRole)
	{
		LOG_PROCESS_ERROR(poRole);
		LOG_PROCESS_ERROR(nCount < MAX_SYNC_CHAR_COUNT);

		if (!poRole->IsHidden())
		{
			CHAR_APPEAR_DATA& rAppearData = SyncCharList->astAppearData[nCount];

			rAppearData.dwCharID = poRole->GetObjID();
			rAppearData.byMoveState = poRole->GetMoveState();
			rAppearData.wCurrentX = ConvertCoordS2N(poRole->GetX());
			rAppearData.wCurrentY = ConvertCoordS2N(poRole->GetY());
			rAppearData.wCurrentZ = ConvertCoordS2N(poRole->GetZ());
			rAppearData.wFaceDirection = (UINT16)poRole->GetFaceDirection();
			rAppearData.byCombatPoseOn = poRole->GetFlag().CheckFlag(cfCombatPoseOn);

			INT32 nMaxHp = poRole->GetAttrList().GetAttr(catMaxHp);
			rAppearData.byLifePercent = nMaxHp > 0 ? (UINT8)(poRole->GetHp() * 100 / nMaxHp) : 0;

			rAppearData.byMountID = poRole->GetCurrentMountID();
			rAppearData.wWingID = (UINT16)poRole->GetCurrentWingID();
            rAppearData.bPet = 0;
			rAppearData.bGreyName = poRole->IsGreyName();

			nCount++;
			SyncCharList->wSize += sizeof(CHAR_APPEAR_DATA);
		}

		return TRUE;
Exit0:
		return FALSE;
	}

	BOOL operator()(CHero* poHero)
	{
		LOG_PROCESS_ERROR(poHero);
		LOG_PROCESS_ERROR(nCount < MAX_SYNC_CHAR_COUNT);

		{
			CHAR_APPEAR_DATA& rAppearData = SyncCharList->astAppearData[nCount];

			rAppearData.dwCharID = poHero->GetObjID();
			rAppearData.byMoveState = poHero->GetMoveState();
			rAppearData.wCurrentX = ConvertCoordS2N(poHero->GetX());
			rAppearData.wCurrentY = ConvertCoordS2N(poHero->GetY());
			rAppearData.wCurrentZ = ConvertCoordS2N(poHero->GetZ());
			rAppearData.wFaceDirection = (UINT16)poHero->GetFaceDirection();
			rAppearData.byCombatPoseOn = poHero->GetFlag().CheckFlag(cfCombatPoseOn);
            rAppearData.bPet = 0;
			rAppearData.bGreyName = 0;
			rAppearData.wWingID = 0;
			INT32 nMaxHp = poHero->GetAttrList().GetAttr(catMaxHp);
			rAppearData.byLifePercent = nMaxHp > 0 ? (UINT8)(poHero->GetHp() * 100 / nMaxHp) : 0;

			nCount++;
			SyncCharList->wSize += sizeof(CHAR_APPEAR_DATA);
		}

		return TRUE;
Exit0:
		return FALSE;
	}

	BOOL operator()(CNpc* poNpc)
	{
		LOG_PROCESS_ERROR(poNpc);
		LOG_PROCESS_ERROR(nCount < MAX_SYNC_CHAR_COUNT);

		{
			CHAR_APPEAR_DATA& rAppearData = SyncCharList->astAppearData[nCount];

			rAppearData.dwCharID = poNpc->GetObjID();
			rAppearData.byMoveState = poNpc->GetMoveState();
			rAppearData.wCurrentX = ConvertCoordS2N(poNpc->GetX());
			rAppearData.wCurrentY = ConvertCoordS2N(poNpc->GetY());
			rAppearData.wCurrentZ = ConvertCoordS2N(poNpc->GetZ());
			rAppearData.wFaceDirection = (UINT16)poNpc->GetFaceDirection();
			rAppearData.byCombatPoseOn = poNpc->GetFlag().CheckFlag(cfCombatPoseOn);
            rAppearData.bPet = (poNpc->GetPetType() > 0 ? 1 : 0);
			rAppearData.bGreyName = 0;
			rAppearData.wWingID = 0;

			INT32 nMaxHp = poNpc->GetAttrList().GetAttr(catMaxHp);
			rAppearData.byLifePercent = nMaxHp > 0 ? (UINT8)(poNpc->GetHp() * 100 / nMaxHp) : 0;

			nCount++;
		}

		return TRUE;
Exit0:
		return FALSE;
	}
};

BOOL CCliMessageHandler::DoSyncNewCharList(CRole* poRole, CRegion* poRegion)
{
	INT32 nRetCode = 0;
	G2C_SYNC_CHAR_LIST* pMsg = NULL;
	CTraverseChar TraverseChar;

	LOG_PROCESS_ERROR(poRegion);
	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_CHAR_LIST*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_char_list;
	TraverseChar.SyncCharList = pMsg;
	TraverseChar.nCount = 0;

	nRetCode = poRegion->TraverseRole(TraverseChar);
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = poRegion->TraverseHero(TraverseChar);
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = poRegion->TraverseNpc(TraverseChar);
	LOG_CHECK_ERROR(nRetCode);

	if (TraverseChar.nCount > 0)
	{
		pMsg->wSize = sizeof(G2C_SYNC_CHAR_LIST) + sizeof(CHAR_APPEAR_DATA) * TraverseChar.nCount;

		nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	}
	//    DBG("sync region (%d, %d) char list(%d) to role %d", poRegion->GetRegionX(), poRegion->GetRegionY(),
	//        TraverseChar.SyncCharList.byCharCount, poRole->GetObjID());

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoObjectDisappear(CSceneObject* poSceneObject, CRegion* poDestRegion, INT32 nDisappearType)
{
	INT32 nRetCode = 0;
	G2C_OBJECT_DISAPPEAR* pMsg = NULL;

	LOG_PROCESS_ERROR(poSceneObject);

	pMsg = (G2C_OBJECT_DISAPPEAR*)m_poPacketAllocator->Alloc(sizeof(G2C_OBJECT_DISAPPEAR));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_object_disappear;
	pMsg->dwObjID = poSceneObject->GetObjID();
    pMsg->byDisappearType = nDisappearType;

	if (poDestRegion == NULL)
	{
		nRetCode = _Broadcast9Region(poSceneObject->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_OBJECT_DISAPPEAR));
		LOG_PROCESS_ERROR(nRetCode);

		//        DBG("scene object [%d] disappear @ region (%I64d, %I64d)", poSceneObject->GetObjID(), 
		//            poSceneObject->GetX() / REGION_WIDTH, poSceneObject->GetY() / REGION_WIDTH);
	}
	else
	{
		nRetCode = _BroadcastRegion(poDestRegion, (PACKET_BUFFER*)pMsg, sizeof(G2C_OBJECT_DISAPPEAR));
		LOG_PROCESS_ERROR(nRetCode);

		//        DBG("scene object [%d] disappear @ region(%d, %d)", poSceneObject->GetObjID(), 
		//           poDestRegion->GetRegionX(), poDestRegion->GetRegionY());
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::CTraverseDisappearObj::operator()(CSceneObject* poSceneObject)
{
	if (poSceneObject->GetObjType() == otRole && ((CRole*)poSceneObject)->IsHidden())
		return TRUE;

	pMsg->dwObjIDList[nObjCount++] = poSceneObject->GetObjID();

	return TRUE;
}

BOOL CCliMessageHandler::DoObjectDisappearList(CRole* poRole, CRegion* poDestRegion)
{
	INT32 nRetCode = 0;
	G2C_OBJECT_DISAPPEAR_LIST* pMsg = NULL;
	CTraverseDisappearObj TraverseDisappearObj;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poDestRegion);

	pMsg = (G2C_OBJECT_DISAPPEAR_LIST*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_object_disappear_list;

	TraverseDisappearObj.pMsg = pMsg;
	TraverseDisappearObj.nObjCount = 0;

	poDestRegion->TraverseCharacter(TraverseDisappearObj);
	poDestRegion->TraverseDoodad(TraverseDisappearObj);
	poDestRegion->TraverseDrop(TraverseDisappearObj);

	pMsg->wSize = sizeof(G2C_OBJECT_DISAPPEAR_LIST) + sizeof(pMsg->dwObjIDList[0]) * TraverseDisappearObj.nObjCount;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncRoleData(CRole* poRole, CRole* poDataRole, UINT32 dwCRC)
{
	INT32 nRetCode = 0;
	UINT32 dwNewCRC = 0;
	G2C_SYNC_ROLE_DATA* pMsg = NULL;
    CGSGuild    *poGSGuild = NULL;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poDataRole);

	pMsg = (G2C_SYNC_ROLE_DATA*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_ROLE_DATA));
	LOG_PROCESS_ERROR(pMsg);

	memset(pMsg, 0, sizeof(G2C_SYNC_ROLE_DATA));

	pMsg->wMsg = g2c_sync_role_data;
	pMsg->dwRoleID = poDataRole->GetObjID();
	strxcpy(pMsg->szName, poDataRole->GetName(), ROLE_NAME_LEN);
	pMsg->byClass = poDataRole->GetClass();
	pMsg->bySex = poDataRole->GetSex();
    pMsg->byHairType = poDataRole->GetHairType();
    pMsg->byHairColor = poDataRole->GetHairColor();
    pMsg->byFaceType = poDataRole->GetFaceType();
    pMsg->bySkinColor = poDataRole->GetSkinColor();
	pMsg->byFaction = poDataRole->GetFaction();
	pMsg->dwPkValue = poDataRole->GetPkValue();
    pMsg->byVIPLevel = poDataRole->GetVIPLevel();

	pMsg->nMaxHp = poDataRole->GetAttr(catMaxHp);
	pMsg->nMaxMp = poDataRole->GetAttr(catMaxMp);
	pMsg->dwTeamID = poDataRole->GetTeamID();
	pMsg->byPkMode = poDataRole->GetPkMode();
	pMsg->byTitleID = poDataRole->GetTitle().GetEquipedTitleID();
	pMsg->dwCreateTime = poDataRole->GetCreateTime();
	
	pMsg->byStallState = poDataRole->GetStall()->GetStallState();
    pMsg->byStallLevel = poDataRole->GetStall()->GetStallLevelUpExpireTime() > CGameWorld::GetInstance().GetGameTime();
	strxcpy(pMsg->szStallName, poDataRole->GetStall()->GetStallName(), STALL_NAME_LEN);

    // gaga_5
    pMsg->byMasterState = poDataRole->GetMaster().GetState();

    if (INVALID_OBJECT_ID != poDataRole->GetGuildID())
        pMsg->dwGuildID = poDataRole->GetGuildID();
    else
        pMsg->dwGuildID = 0;

    pMsg->byRefineFx = poDataRole->GetItemBag()->GetRefineFx();
	poDataRole->GetItemBag()->GetAppearance(pMsg->anAvatar);

	dwNewCRC = crc32(0, (const Bytef*)pMsg, sizeof(G2C_SYNC_ROLE_DATA));
	if (dwNewCRC != dwCRC)
	{
		nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_ROLE_DATA));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncNpcData(CRole* poRole, CNpc* poDataNpc, UINT32 dwCRC)
{
	INT32 nRetCode = 0;
	G2C_SYNC_NPC_DATA* pMsg = NULL;
	UINT32 dwNewCRC = 0;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poDataNpc);

	pMsg = (G2C_SYNC_NPC_DATA*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_NPC_DATA));
	LOG_PROCESS_ERROR(pMsg);

	memset(pMsg, 0, sizeof(G2C_SYNC_NPC_DATA));

	pMsg->wMsg = g2c_sync_npc_data;
	pMsg->dwNpcID = poDataNpc->GetObjID();
	LOG_PROCESS_ERROR(poDataNpc->GetNpcData());
	pMsg->nTemplateID = poDataNpc->GetNpcData()->nTemplateID;
	pMsg->nMaxHp = poDataNpc->GetAttr(catMaxHp);
	pMsg->byQuality = poDataNpc->GetQuality();
	pMsg->byLevel = poDataNpc->GetLevel(); 

	dwNewCRC = crc32(0, (const Bytef*)pMsg, sizeof(G2C_SYNC_NPC_DATA));
	if (dwNewCRC != dwCRC)
	{
		nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_NPC_DATA));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::BroadcastNpcData(CNpc* poDataNpc)
{
    INT32 nRetCode = 0;
    G2C_SYNC_NPC_DATA* pMsg = NULL;

    LOG_PROCESS_ERROR(poDataNpc);

    pMsg = (G2C_SYNC_NPC_DATA*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_NPC_DATA));
    LOG_PROCESS_ERROR(pMsg);

    memset(pMsg, 0, sizeof(G2C_SYNC_NPC_DATA));

    pMsg->wMsg = g2c_sync_npc_data;
    pMsg->dwNpcID = poDataNpc->GetObjID();
    LOG_PROCESS_ERROR(poDataNpc->GetNpcData());
    pMsg->nTemplateID = poDataNpc->GetNpcData()->nTemplateID;
    pMsg->nMaxHp = poDataNpc->GetAttr(catMaxHp);
    pMsg->byQuality = poDataNpc->GetQuality();
    pMsg->byLevel = poDataNpc->GetLevel();

    nRetCode = _Broadcast9Region(poDataNpc->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_NPC_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncPetData(CRole* poRole, CNpc *poDataNpc, UINT32 dwCRC)
{
    INT32 nRetCode = 0;
    G2C_SYNC_NPC_EX_DATA* pMsg = NULL;
    CRole   *poHost = NULL;
    UINT32 dwNewCRC = 0;

    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(poDataNpc);

    pMsg = (G2C_SYNC_NPC_EX_DATA*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_NPC_EX_DATA));
    LOG_PROCESS_ERROR(pMsg);

    memset(pMsg, 0, sizeof(G2C_SYNC_NPC_EX_DATA));

    pMsg->wMsg = g2c_sync_npc_ex_data;

    pMsg->dwNpcID = poDataNpc->GetObjID();
    LOG_PROCESS_ERROR(poDataNpc->GetNpcData());
    pMsg->nTemplateID = poDataNpc->GetNpcData()->nTemplateID;
    pMsg->nMaxHp = poDataNpc->GetAttr(catMaxHp);
    pMsg->byQuality = poDataNpc->GetQuality();
    pMsg->byLevel = poDataNpc->GetLevel();

    pMsg->byType = npctPet;
    pMsg->byPetLevel = poDataNpc->GetPetLevel();
    pMsg->dwHostID = poDataNpc->GetHostID();
	pMsg->szHostName[0] = 0;

    poHost = CGameWorld::GetInstance().FindRole(pMsg->dwHostID);
    if (poHost)
    {
        strxcpy(pMsg->szHostName, poHost->GetName(), ROLE_NAME_LEN);
    }
    
    dwNewCRC = crc32(0, (const Bytef*)pMsg, sizeof(G2C_SYNC_NPC_EX_DATA));
    if (dwNewCRC != dwCRC)
    {
        nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_NPC_EX_DATA));
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncRoleAttrData(CRole* poRole, CRole* poTargetRole, UINT32 dwCRC)
{
	INT32 nRetCode = 0;
	INT32 nSize;
	G2C_SYNC_ROLE_ATTR_DATA* pMsg = NULL;
	UINT32 dwNewCRC = 0;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poTargetRole);

	nSize = sizeof(G2C_SYNC_ROLE_ATTR_DATA) + catTotal * sizeof(ATTR_DATA);
	pMsg = (G2C_SYNC_ROLE_ATTR_DATA*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_role_attr_data;
	pMsg->wSize = nSize;
	pMsg->dwRoleID = poTargetRole->GetObjID();
	pMsg->nFatigue = poTargetRole->GetItemBag()->GetMoney(MONEYTYPE_FATIGUE);

	for (INT32 i = catDefault; i < catTotal; i++)
	{
		pMsg->astAttrData[i].byAttrID = i;
		pMsg->astAttrData[i].nAttrValue = poTargetRole->GetAttrList().GetAttr(i);
	}

	dwNewCRC = crc32(0, (const Bytef*)pMsg, pMsg->wSize);

	if (dwNewCRC != dwCRC)
	{
		nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncCharAuraBrief(CRole* poRole, CCharacter* poTargetChar)
{
	INT32 nRetCode = 0;
	BOOL bNeedSend = FALSE;
	G2C_SYNC_AURA_BRIEF* pMsg = NULL;
	
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poTargetChar);

	pMsg = (G2C_SYNC_AURA_BRIEF*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_AURA_BRIEF));
	LOG_PROCESS_ERROR(pMsg);
	
	pMsg->wMsg = g2c_sync_aura_brief;
	pMsg->dwCharID = poTargetChar->GetObjID();

	for (INT32 i = adgBegin; i < adgTotal; i++)
	{
		pMsg->BriefData[i].wTemplateID = poTargetChar->GetAuraList().GetTopDisplayAuraID(i);
		pMsg->BriefData[i].dwInstanceID = poTargetChar->GetAuraList().GetTopDisplayAuraInstanceID(i);

		if (pMsg->BriefData[i].wTemplateID != 0)
			bNeedSend = TRUE;
	}

	PROCESS_ERROR(bNeedSend);

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_AURA_BRIEF));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncRolePow(CRole* poRole, CRole* poTargetRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_ROLE_POW* pMsg = NULL;
	
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poTargetRole);

	pMsg = (G2C_SYNC_ROLE_POW*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_ROLE_POW));
	LOG_PROCESS_ERROR(pMsg);
	
	pMsg->wMsg = g2c_sync_role_pow;
	pMsg->dwRoleID = poTargetRole->GetObjID();
	pMsg->nPow = poTargetRole->GetPow();

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_ROLE_POW));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoSyncOpenDoodadCount(CDoodad* poDoodad, CCharacter* poChar)
{
	INT32 nRetCode = 0;
	G2C_SYNC_DOODAD_OPEN_COUNT* pMsg = NULL;
	
	LOG_PROCESS_ERROR(poDoodad);
    LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_SYNC_DOODAD_OPEN_COUNT*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_DOODAD_OPEN_COUNT));
	LOG_PROCESS_ERROR(pMsg);
	
	pMsg->wMsg = g2c_sync_doodad_open_count;
	pMsg->dwDoodadID = poDoodad->GetObjID();
    pMsg->dwCharID = poChar->GetObjID();
	pMsg->byOpenCount = poDoodad->GetOpenCount();

	nRetCode = _Broadcast9Region(poDoodad->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_DOODAD_OPEN_COUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncTeleportPlatform(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_TELEPORT_PLATFORM* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_TELEPORT_PLATFORM*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_TELEPORT_PLATFORM));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_teleport_platform;
	memcpy(pMsg->nTeleportPlatform, poRole->GetTeleportform(), sizeof(pMsg->nTeleportPlatform));

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_TELEPORT_PLATFORM));
	LOG_PROCESS_ERROR(nRetCode);
	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoActiveTeleportPlatform(CRole* poRole, UINT8 byTeleportPlatformID)
{
	INT32 nRetCode = 0;
	G2C_ACTIVE_PLATFORM* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_ACTIVE_PLATFORM*)m_poPacketAllocator->Alloc(sizeof(G2C_ACTIVE_PLATFORM));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_active_platform;
	pMsg->byActivePlatformID = byTeleportPlatformID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ACTIVE_PLATFORM));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncHeroData(CRole* poRole, CHero* poHero, UINT32 dwCRC)
{
	INT32 nRetCode = 0;
	UINT32 dwNewCRC = 0;
	G2C_SYNC_HERO_DATA* pMsg = NULL;

	CRole* poHostRole = 0;

	LOG_PROCESS_ERROR(poHero);

	pMsg = (G2C_SYNC_HERO_DATA*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_HERO_DATA));
	LOG_PROCESS_ERROR(pMsg);

	poHostRole = poHero->GetHostRole();
	LOG_PROCESS_ERROR(poHostRole);

	memset(pMsg, 0, sizeof(G2C_SYNC_HERO_DATA));

	pMsg->wMsg = g2c_sync_hero_data;
	pMsg->dwHeroID = poHero->GetObjID();
	strxcpy(pMsg->szName, poHero->GetName(), ROLE_NAME_LEN);
	strxcpy(pMsg->szMasterName, poHostRole->GetName(), ROLE_NAME_LEN);
	pMsg->byClass = poHero->GetClass();
	pMsg->bySex = poHero->GetSex();
	pMsg->byLevel = poHero->GetLevel();

	pMsg->nMaxHp = poHero->GetAttr(catMaxHp);
	pMsg->nMaxMp = poHero->GetAttr(catMaxMp);

	poRole->GetItemBag()->GetHeroAppearance(pMsg->anAvatar);

	dwNewCRC = crc32(0, (const Bytef*)pMsg, sizeof(G2C_SYNC_HERO_DATA));
	if (dwNewCRC != dwCRC)
	{
		nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_HERO_DATA));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

void CCliMessageHandler::OnTeleport(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_TELEPORT* pMsg = (C2G_TELEPORT*)pData;
	const SCENE_TELEPORTS* pTeleport = NULL;
	CScene* poSrcScene = NULL;
	CSceneStaticData* pStaticData = NULL;
	const SCENE_DATA* pSceneData = NULL;
	INT32 nTgtSceneID = 0;

    PROCESS_ERROR_RET_CODE(poRole->GetState() == rsPlaying, errJumpWrongState);
    //PROCESS_ERROR_RET_CODE(!poRole->IsCasting(), errJumpCasting);
    PROCESS_ERROR_RET_CODE(poRole->GetStall()->GetStallState() == essNotOpen, errJumpStallOpen);
    PROCESS_ERROR_RET_CODE(!poRole->GetTrade()->IsTrading(), errJumpTrading);
    PROCESS_ERROR_RET_CODE(pMsg->dwTeleportID != 20, errJumpWrongPos);

	poSrcScene = poRole->GetScene();
	LOG_PROCESS_ERROR(poSrcScene);

	pStaticData = poSrcScene->GetStaticData();
	LOG_PROCESS_ERROR(pStaticData);

	pTeleport = pStaticData->FindTeleportPoint(pMsg->dwTeleportID);
	LOG_PROCESS_ERROR(pTeleport != NULL);
	LOG_PROCESS_ERROR(poSrcScene->GetSceneTempID() == pTeleport->SrcSceneID);

	{
		INT64 llDistance2D = g_GetDistance2D(poRole->GetX(), poRole->GetY(), 
			pTeleport->SrcSceneX, pTeleport->SrcSceneY);
		INT64 llTeleportRange = pTeleport->SrcRadius;
		llTeleportRange *= llTeleportRange;
		LOG_PROCESS_ERROR_RET_CODE(llDistance2D <= llTeleportRange, errSceneTooFarFromTeleport);
	}

	pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pTeleport->TargetSceneID);
	LOG_PROCESS_ERROR(pSceneData);

	if (pSceneData->nType == estDungeon && pTeleport->TargetSceneID != poSrcScene->GetSceneTempID())
	{
		if(poRole->GetTeamID() == 0)
		{
			nRetCode = CCliMessageHandler::GetInstance().DoQuerySceneDifficulty(poRole, pTeleport->TargetSceneID);
			LOG_PROCESS_ERROR(nRetCode);

			poRole->SetTeleportID(pMsg->dwTeleportID);
		}
		else
		{
			CTeamInGS* poTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
			LOG_PROCESS_ERROR(poTeam);
			LOG_PROCESS_ERROR_RET_CODE(poTeam->GetLeaderID() == poRole->GetObjID(), errSceneNotTeamLeader);
			LOG_PROCESS_ERROR_RET_CODE(poTeam->GetMemberCount() <= 5, errSceneNotTeamMember);

			nRetCode = CCliMessageHandler::GetInstance().DoQuerySceneDifficulty(poRole, pTeleport->TargetSceneID);
			LOG_PROCESS_ERROR(nRetCode);

			for (INT32 nIndex =0; nIndex < poTeam->GetMaxMemberCount(); nIndex++)
			{
				CRole* poMemeber = CGameWorld::GetInstance().FindRole(poTeam->GetMemberTable(nIndex).dwRoleID);
				if (poMemeber)
				{
					poMemeber->SetTeleportID(pMsg->dwTeleportID);
				}
			}
		}
	}
	else if (pSceneData->nType == estSoloDungeon && pTeleport->TargetSceneID != poSrcScene->GetSceneTempID())
	{
		nRetCode = CCliMessageHandler::GetInstance().DoQuerySceneDifficulty(poRole, pTeleport->TargetSceneID);
		LOG_PROCESS_ERROR(nRetCode);

		poRole->SetTeleportID(pMsg->dwTeleportID);
	}
	else if (pSceneData->nType == estStatic)
	{
		UINT32 dwCliSessionID = poRole->GetSessionID();
		INT32 nServerGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwCliSessionID);
		nTgtSceneID = (1 << nServerGroupID) | 1;

		nRetCode = CGameWorld::GetInstance().JumpTo(poRole, pTeleport->TargetSceneID, nTgtSceneID,  
			pTeleport->TargetSceneX, pTeleport->TargetSceneY, pTeleport->TargetSceneZ, pTeleport->TargetRotationYaw);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		if (pTeleport->TargetSceneID == poSrcScene->GetSceneTempID())
		{
			nTgtSceneID = poSrcScene->GetSceneID();
		}

		nRetCode = CGameWorld::GetInstance().JumpTo(poRole, pTeleport->TargetSceneID, nTgtSceneID,  
			pTeleport->TargetSceneX, pTeleport->TargetSceneY, pTeleport->TargetSceneZ, pTeleport->TargetRotationYaw);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return;
Exit0:
	if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);
	return;
}

void CCliMessageHandler::OnEnterDungeon(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_ENTER_DUNGEON* pMsg = (C2G_ENTER_DUNGEON*)pData;
	CScene* poSrcScene = NULL;
	CSceneStaticData* pStaticData = NULL;
	const SCENE_TELEPORTS* pTeleport = NULL;
	const SERVER_POINT* pServerPoint = NULL;
	const SCENE_DATA* pSceneData = NULL;
	const SCENE_DATA* pTargetSceneData = NULL;
    INT32 nDungeonID = 0;
    const DUNGEON_DATA* pDungeonData = NULL;
	BOOL bCheckCondition = TRUE;

	UINT32 nTargetSceneTempID = 0;
	UINT32 dwTargetX = 0;
	UINT32 dwTargetY = 0;
	UINT32 dwTargetZ = 0;
	UINT32 dwTargetRotationYaw = 0;
	CTeamInGS* poTeam = NULL;

    PROCESS_ERROR_RET_CODE(poRole->GetState() == rsPlaying, errJumpWrongState);
    PROCESS_ERROR_RET_CODE(!poRole->IsCasting(), errJumpCasting);
    PROCESS_ERROR_RET_CODE(poRole->GetStall()->GetStallState() == essNotOpen, errJumpStallOpen);
    PROCESS_ERROR_RET_CODE(!poRole->GetTrade()->IsTrading(), errJumpTrading);

    poSrcScene = poRole->GetScene();
	LOG_PROCESS_ERROR(poSrcScene);

	pStaticData = poSrcScene->GetStaticData();
	LOG_PROCESS_ERROR(pStaticData);

	if(poRole->GetTeleportID() == INVALID_TELEPORT_ID)
	{
		pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(poSrcScene->GetSceneTempID());
		LOG_PROCESS_ERROR(pSceneData);
		LOG_PROCESS_ERROR(pSceneData->nType == estDungeon || pSceneData->nType == estSoloDungeon);

		nTargetSceneTempID = poSrcScene->GetSceneTempID();

		pServerPoint = pStaticData->GetServerPoint(DUNGEON_REPLAY_START_POS_NAME);
		LOG_PROCESS_ERROR(pServerPoint);

		dwTargetX = pServerPoint->nX;
		dwTargetY = pServerPoint->nY;
		dwTargetZ = pServerPoint->nZ;
		dwTargetRotationYaw = 0;

		bCheckCondition = FALSE;
	}
	else
	{
		pTeleport = pStaticData->FindTeleportPoint(poRole->GetTeleportID());
		LOG_PROCESS_ERROR(pTeleport);
		LOG_PROCESS_ERROR(poSrcScene->GetSceneTempID() == pTeleport->SrcSceneID);

		nTargetSceneTempID = pTeleport->TargetSceneID;
		dwTargetX = pTeleport->TargetSceneX;
		dwTargetY = pTeleport->TargetSceneY;
		dwTargetZ = pTeleport->TargetSceneZ;
		dwTargetRotationYaw = pTeleport->TargetRotationYaw;

		bCheckCondition = TRUE;
	}
//	LOG_PROCESS_ERROR(poRole->CheckDungeonDifficultyMask(nTargetSceneTempID, pMsg->bySelectedDifficulty));

	pTargetSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nTargetSceneTempID);
	LOG_PROCESS_ERROR(pTargetSceneData);
	LOG_PROCESS_ERROR(pTargetSceneData->nType == estDungeon || pTargetSceneData->nType == estSoloDungeon);

	pDungeonData = CDataManager<DUNGEON_DATA>::GetInstance().GetData(nTargetSceneTempID);
    LOG_PROCESS_ERROR(pDungeonData);

	if (poRole->GetTeamID() == 0 || pTargetSceneData->nType == estSoloDungeon)
	{
		if(bCheckCondition && pTeleport)
		{
			INT64 llDistance3D = g_GetDistance3D(poRole->GetX(), poRole->GetY(), poRole->GetZ(),
				pTeleport->SrcSceneX, pTeleport->SrcSceneY, pTeleport->SrcSceneZ);
			INT64 llTeleportRange = pTeleport->SrcRadius;
			llTeleportRange *= llTeleportRange;
			LOG_PROCESS_ERROR_RET_CODE(llDistance3D <= llTeleportRange, errSceneTooFarFromTeleport);
		}

		PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(MONEYTYPE_FATIGUE) > 0, errSceneNoFatigue);
		PROCESS_ERROR_RET_CODE(poRole->GetLevel() >= pDungeonData->nMinLevel && poRole->GetLevel() <= pDungeonData->nMaxLevel, errSceneNoLevel);
        PROCESS_ERROR_RET_CODE(poRole->GetDungeonEnterCount(nTargetSceneTempID) < pDungeonData->nMaxEnterCount, errSceneEnterCountLimit);

		nRetCode = poRole->GetItemBag()->SubMoney(MONEYTYPE_FATIGUE, pDungeonData->nFatigueValue, MONEYLOG_INVALID, 0);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = CGameWorld::GetInstance().JumpTo(poRole, nTargetSceneTempID, 0,
			dwTargetX, dwTargetY, dwTargetZ, dwTargetRotationYaw);
		LOG_PROCESS_ERROR(nRetCode);

        nRetCode = poRole->UpdateDungeonEnterCount(nTargetSceneTempID);
        LOG_CHECK_ERROR(nRetCode);
	}
	else
	{
		poTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
		LOG_PROCESS_ERROR(poTeam);

		LOG_PROCESS_ERROR_RET_CODE(poTeam->GetLeaderID() == poRole->GetObjID(), errSceneNotTeamLeader);
		LOG_PROCESS_ERROR_RET_CODE(poTeam->GetMemberCount() <= pDungeonData->nRoleLimit, errSceneNotTeamMember);

		for (INT32 nIndex = 0; nIndex < poTeam->GetMaxMemberCount(); nIndex++)
		{
			if (poTeam->GetMemberTable(nIndex).dwRoleID == INVALID_OBJECT_ID)
				continue;

			CRole* poMemeber = CGameWorld::GetInstance().FindRole(poTeam->GetMemberTable(nIndex).dwRoleID);
			LOG_PROCESS_ERROR_RET_CODE(poMemeber, errSceneTooFarFromTeleport);

			if(bCheckCondition && pTeleport)
			{
				INT64 llDistance3D = g_GetDistance3D(poMemeber->GetX(), poMemeber->GetY(), poMemeber->GetZ(),
					pTeleport->SrcSceneX, pTeleport->SrcSceneY, pTeleport->SrcSceneZ);
				INT64 llTeleportRange = pTeleport->SrcRadius;

				llTeleportRange *= llTeleportRange;
				LOG_PROCESS_ERROR_RET_CODE(llDistance3D <= llTeleportRange, errSceneTooFarFromTeleport);
			}

			PROCESS_ERROR_RET_CODE(poMemeber->GetItemBag()->GetMoney(MONEYTYPE_FATIGUE) > 0, errSceneNoFatigue);
			PROCESS_ERROR_RET_CODE(poMemeber->GetLevel() >= pDungeonData->nMinLevel && poMemeber->GetLevel() <= pDungeonData->nMaxLevel, errSceneNoLevel);
			PROCESS_ERROR_RET_CODE(poMemeber->GetDungeonEnterCount(nTargetSceneTempID) < pDungeonData->nMaxEnterCount, errSceneEnterCountLimit);
		}

		for (INT32 nIndex = 0; nIndex < poTeam->GetMaxMemberCount(); nIndex++)
		{
			CRole* poMemeber = CGameWorld::GetInstance().FindRole(poTeam->GetMemberTable(nIndex).dwRoleID);
			if (poMemeber)
			{
				nRetCode = poMemeber->GetItemBag()->SubMoney(MONEYTYPE_FATIGUE, pDungeonData->nFatigueValue, MONEYLOG_INVALID, 0);
				LOG_PROCESS_ERROR(nRetCode);

				nRetCode = CGameWorld::GetInstance().JumpTo(poMemeber, nTargetSceneTempID, 0,
					dwTargetX, dwTargetY, dwTargetZ, dwTargetRotationYaw);
				LOG_CHECK_ERROR(nRetCode);

                nRetCode = poMemeber->UpdateDungeonEnterCount(nTargetSceneTempID);
                LOG_CHECK_ERROR(nRetCode);
            }
		}
	}

	return;
Exit0:
	//if (nResult == errUnknownError)
	//{
	//	m_poNotifier->KickUser(poRole->GetSessionID());
	//}
	//else
	{
		if(poTeam && poTeam->GetLeaderID() == poRole->GetObjID())
		{
			for (INT32 nIndex = 0; nIndex < poTeam->GetMaxMemberCount(); nIndex++)
			{
				CRole* poMemeber = CGameWorld::GetInstance().FindRole(poTeam->GetMemberTable(nIndex).dwRoleID);
				if (poMemeber)
					poMemeber->ErrorCodeNotify(nResult, 0, 0);
			}
		}
		else
		{
			poRole->ErrorCodeNotify(nResult, 0, 0);
		}
	}

	return;
}

void CCliMessageHandler::OnSelectSceneDifficulty(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_SELECT_SCENE_DIFFICULTY* pMsg = (C2G_SELECT_SCENE_DIFFICULTY*)pData;

	if(poRole->GetTeamID() > 0)
	{
		CTeamInGS* poTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
		LOG_PROCESS_ERROR(poTeam);
		LOG_PROCESS_ERROR_RET_CODE(poTeam->GetLeaderID() == poRole->GetObjID(), errSceneNotTeamLeader);
	}

	nRetCode = CCliMessageHandler::GetInstance().DoSelectSceneDifficulty(poRole, pMsg->bySelectedDifficulty);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);
	return;
}


void CCliMessageHandler::OnChangeLine(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_CHANGE_LINE* pMsg = (C2G_CHANGE_LINE*)pData;
	const SCENE_DATA* poSceneData = NULL;
	INT32 nNewSceneID = pMsg->byLineIndex;
	CScene* poScene = NULL;

    PROCESS_ERROR_RET_CODE(poRole->GetState() == rsPlaying, errJumpWrongState);
    PROCESS_ERROR_RET_CODE(!poRole->IsCasting(), errJumpCasting);
    PROCESS_ERROR_RET_CODE(poRole->GetStall()->GetStallState() == essNotOpen, errJumpStallOpen);
    PROCESS_ERROR_RET_CODE(!poRole->GetTrade()->IsTrading(), errJumpTrading);

	poSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(poRole->GetSceneTempID());
	LOG_PROCESS_ERROR(poSceneData);
	LOG_PROCESS_ERROR(poSceneData->nType == estLine);

	poScene = poRole->GetScene();
	LOG_PROCESS_ERROR(poScene);

	nNewSceneID |= poRole->GetSceneID() & 0xFFFF0000;

	nRetCode = CGameWorld::GetInstance().JumpTo(poRole, poRole->GetSceneTempID(), nNewSceneID, 
		poRole->GetX(), poRole->GetY(), poRole->GetZ(), poRole->GetFaceDirection());
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else

	return;
}

void CCliMessageHandler::OnLeaveScene(CRole* poRole, const char* pData)
{
	INT32 nResult = 0;
    INT32 nRetCode = 0;
    CScene* poScene = NULL;

    poScene = poRole->GetScene();
    LOG_PROCESS_ERROR(poScene);

    LOG_PROCESS_ERROR(poScene->GetSceneData());
	PROCESS_ERROR_RET_CODE(!poRole->IsDead(), errJumpDead);

    switch (poScene->GetSceneData()->nType)
    {
    case estBattleField:
    case estArena:
        {
            nRetCode = poRole->JumpToReturnPos();
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
    case estDungeon:
	case estSoloDungeon:
        {
            nRetCode = poRole->JumpToReturnPos();
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
	case estGuildDungeon:
		{
			nRetCode = poRole->JumpToReturnPos();
			LOG_PROCESS_ERROR(nRetCode);
		}
		break;
	case estGuild:
		{
			nRetCode = poRole->JumpToReturnPos();
			LOG_PROCESS_ERROR(nRetCode);
		}
		break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return;
Exit0:
    return;
}

void CCliMessageHandler::OnRevive(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
	C2G_REVIVE* pMsg = (C2G_REVIVE*)pData;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pMsg->byType > crtInvalid && pMsg->byType < crtTotal);

	nResult = poRole->TryRevive(pMsg->byType);
	PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else if(nResult != errSuccess)
		poRole->ErrorCodeNotify(nResult, 0, 0);

    return;
}

void CCliMessageHandler::OnCombatPoseSwitch(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
	C2G_COMBAT_POSE_SWITCH* pMsg = (C2G_COMBAT_POSE_SWITCH*)pData;

	LOG_PROCESS_ERROR(poRole);

	poRole->CombatPoseSwitch(pMsg->byCombatPoseOn, pMsg->byClientFlag);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else if(nResult != errSuccess)
		poRole->ErrorCodeNotify(nResult, 0, 0);

    return;
}

BOOL CCliMessageHandler::DoChangeSceneEnter(CRole* poRole, BOOL bIsInJump)
{
	INT32 nRetCode = 0;
	G2C_CHANGESCENE_ENTER* pMsg = NULL;
	CScene* pScene = NULL;
	UINT32 dwClientSessionID = 0;
	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_CHANGESCENE_ENTER*)m_poPacketAllocator->Alloc(sizeof(G2C_CHANGESCENE_ENTER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_changescene_enter;

	pMsg->nSceneTempID = poRole->GetSceneTempID();
	pMsg->nSceneID = poRole->GetSceneID();

	// poRole->GetScene() is NULL until client load complete and reply
	dwClientSessionID = poRole->GetSessionID();
	pScene = CGameWorld::GetInstance().FindScene(MakeSceneObjID(poRole->GetSceneTempID(), poRole->GetSceneID()));
	LOG_PROCESS_ERROR( pScene );

	pMsg->wDestX = ConvertCoordS2N(poRole->GetX());
	pMsg->wDestY = ConvertCoordS2N(poRole->GetY());
	pMsg->wDestZ = ConvertCoordS2N(poRole->GetZ());
	pMsg->wFaceDirection = (UINT16)poRole->GetFaceDirection();
	pMsg->byIsInJump = bIsInJump;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHANGESCENE_ENTER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoQueryScene(CRole* poRole, INT32 nSceneTempID, INT32 nSceneID, 
	INT32 nDestX, INT32 nDestY, INT32 nDestZ, INT32 nFaceDirection)
{
	INT32 nRetCode = 0;
	G2M_QUERY_SCENE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2M_QUERY_SCENE*)m_poPacketAllocator->Alloc(sizeof(G2M_QUERY_SCENE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2m_query_scene;
	pMsg->dwClientSessionID = poRole->GetSessionID();
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->nSceneTempID = nSceneTempID;
	pMsg->nSceneID = nSceneID;
	pMsg->nDestX = nDestX;
	pMsg->nDestY = nDestY;
	pMsg->nDestZ = nDestZ; 
	pMsg->nFaceDirection = nFaceDirection;

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(G2M_QUERY_SCENE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


BOOL CCliMessageHandler::DoQuerySceneDifficulty(CRole* poRole, UINT32 dwSceneTemplateID)
{
	INT32 nRetCode = 0;
	G2C_QUERY_SCENE_DIFFICULTY* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_QUERY_SCENE_DIFFICULTY*)m_poPacketAllocator->Alloc(sizeof(G2C_QUERY_SCENE_DIFFICULTY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_query_scene_difficulty;
	pMsg->dwSceneTemplateID = dwSceneTemplateID;

	if (poRole->GetTeamID() == 0)
	{
        pMsg->byDungeonDifficultyMask = poRole->GetDungeonRecordMask(dwSceneTemplateID);
        pMsg->byDungeonEnterCount = poRole->GetDungeonEnterCount(dwSceneTemplateID);

        nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_QUERY_SCENE_DIFFICULTY));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		CTeamInGS* poTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
		LOG_PROCESS_ERROR(poTeam);

        pMsg->byDungeonDifficultyMask = 0xFF;
        pMsg->byDungeonEnterCount = 0;

		for (INT32 nIndex = 0; nIndex < poTeam->GetMaxMemberCount(); nIndex++)
		{
			CRole* poRole = CGameWorld::GetInstance().FindValidRole(poTeam->GetMemberTable(nIndex).dwRoleID);
			if (poRole == NULL)
				continue;
			
        	pMsg->byDungeonDifficultyMask &= poRole->GetDungeonRecordMask(dwSceneTemplateID);
            if (poRole->GetDungeonEnterCount(dwSceneTemplateID) > pMsg->byDungeonEnterCount)
                pMsg->byDungeonEnterCount = poRole->GetDungeonEnterCount(dwSceneTemplateID);
		}

		nRetCode = _BroadcastTeam(poTeam, (PACKET_BUFFER*)pMsg, sizeof(G2C_QUERY_SCENE_DIFFICULTY));
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSelectSceneDifficulty(CRole* poRole, INT32 nSceneDifficulty)
{
	INT32 nRetCode = 0;
	G2C_SELECT_SCENE_DIFFICULTY* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SELECT_SCENE_DIFFICULTY*)m_poPacketAllocator->Alloc(sizeof(G2C_SELECT_SCENE_DIFFICULTY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_select_scene_difficulty;
	pMsg->bySelectedDifficulty = nSceneDifficulty;

	if (poRole->GetTeamID() == 0)
	{
		nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SELECT_SCENE_DIFFICULTY));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		CTeamInGS* poTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
		LOG_PROCESS_ERROR(poTeam);

		nRetCode = _BroadcastTeam(poTeam, (PACKET_BUFFER*)pMsg, sizeof(G2C_SELECT_SCENE_DIFFICULTY));
		LOG_PROCESS_ERROR(nRetCode);

		//nRetCode = _Broadcast9Region(poRole->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SELECT_SCENE_DIFFICULTY));
		//LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

void CCliMessageHandler::OnTeleportByPlatform(CRole* poRole, const char *pData)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	C2G_TELEPORT_BY_PLATFORM* pMsg = (C2G_TELEPORT_BY_PLATFORM*)pData;
	const TELEPORT_PLATFORM* pSrcTeleportForm = NULL;
	const TELEPORT_PLATFORM* pTgtTeleportForm = NULL;
	SCENE_DATA*		   pstSceneData = NULL;

    PROCESS_ERROR_RET_CODE(poRole->GetState() == rsPlaying, errJumpWrongState);
    PROCESS_ERROR_RET_CODE(!poRole->IsCasting(), errJumpCasting);
    PROCESS_ERROR_RET_CODE(poRole->GetStall()->GetStallState() == essNotOpen, errJumpStallOpen);
    PROCESS_ERROR_RET_CODE(!poRole->GetTrade()->IsTrading(), errJumpTrading);

	LOG_PROCESS_ERROR(pMsg->bySrcPlatform < MAX_TELEPORT_PLATFORM_COUNT);
	LOG_PROCESS_ERROR(pMsg->byTgtPlatform < MAX_TELEPORT_PLATFORM_COUNT);

	pSrcTeleportForm = CSceneFileMgr::GetInstance().GetTeleportPlatform(pMsg->bySrcPlatform);
	LOG_PROCESS_ERROR(pSrcTeleportForm != NULL);
	LOG_PROCESS_ERROR(poRole->GetSceneTempID() == pSrcTeleportForm->nSceneID);

	{		
		INT64 llDistance2D = g_GetDistance2D(poRole->GetX(), poRole->GetY(), 
			pSrcTeleportForm->nSceneX, pSrcTeleportForm->nSceneY);
		INT64 llTeleportRange = pSrcTeleportForm->nRadius;
		llTeleportRange *= llTeleportRange;
		LOG_PROCESS_ERROR_RET_CODE(llDistance2D <= llTeleportRange, errSceneTooFarFromTeleport);
	}

	nRetCode = poRole->ActivateTeleportPlatform(pMsg->bySrcPlatform);
	LOG_PROCESS_ERROR(nRetCode);

	PROCESS_SUCCESS(pMsg->bySrcPlatform == pMsg->byTgtPlatform);

	pTgtTeleportForm = CSceneFileMgr::GetInstance().GetTeleportPlatform(pMsg->byTgtPlatform);
	LOG_PROCESS_ERROR(pTgtTeleportForm != NULL);

	if(!pTgtTeleportForm->nInitEnable)
		LOG_PROCESS_ERROR(poRole->CheckTeleportPlatform(pMsg->byTgtPlatform));

	if(pTgtTeleportForm->nMoney > 0)
	{
		PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(pTgtTeleportForm->nMoneyType) >= pTgtTeleportForm->nMoney, errItemNotEnoughMoney);

		nRetCode = poRole->GetItemBag()->SubMoney(pTgtTeleportForm->nMoneyType, pTgtTeleportForm->nMoney, MONEYLOG_TELEPORT_PLATFORM, 0);
		LOG_PROCESS_ERROR(nRetCode);
	}

	nRetCode = CGameWorld::GetInstance().JumpTo(poRole, pTgtTeleportForm->nSceneID, 0,
		pTgtTeleportForm->nSceneX, pTgtTeleportForm->nSceneY, pTgtTeleportForm->nSceneZ, pTgtTeleportForm->nRotationYaw);
	LOG_PROCESS_ERROR(nRetCode);
	
	return;
Exit1:
	nRetCode = DoActiveTeleportPlatform(poRole,	pMsg->bySrcPlatform);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if(nResult == errUnknownError)
	{
        KICK_ROLE(poRole, ktLogged);
	}
	else
	{
		nRetCode = poRole->ErrorCodeNotify(nResult, 0, 0);
		LOG_CHECK_ERROR(nRetCode);
	}
	return;
}

void CCliMessageHandler::OnOnMount(CRole* poRole, const char* pData)
{
	INT32 nResult = 0;

	C2G_ON_MOUNT* pMsg = (C2G_ON_MOUNT*)pData;

	poRole->OffMount();

	nResult = poRole->OnMount(pMsg->nMountID);
	(void)nResult; // 1.05 backport: a rejected mount request is already reported via ErrorCodeNotify, do not kick

	return;
}

void CCliMessageHandler::OnOffMount(CRole* poRole, const char* pData)
{
	poRole->OffMount();
}

BOOL CCliMessageHandler::DoSyncMountList(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_MOUNT_LIST* pMsg = NULL;
	INT32 nMountID = 0;
	INT32 nCount = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_MOUNT_LIST*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_mount_list;
	pMsg->wSize = sizeof(G2C_SYNC_MOUNT_LIST);

	poRole->BeginMount();

	while((nMountID = poRole->NextMount()) != 0)
	{
		pMsg->byMountID[nCount++] = nMountID;
		pMsg->wSize += sizeof(UINT8);
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoActivateMount(CRole* poRole, INT32 nMountID)
{
	INT32 nRetCode = 0;
	G2C_ACTIVATE_MOUNT* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_ACTIVATE_MOUNT*)m_poPacketAllocator->Alloc(sizeof(G2C_ACTIVATE_MOUNT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_activate_mount;
	pMsg->nMountID = nMountID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ACTIVATE_MOUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharOnMount(CCharacter* poChar, INT32 nMountID)
{
	INT32 nRetCode = 0;
	G2C_CHAR_ON_MOUNT* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_CHAR_ON_MOUNT*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_ON_MOUNT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_on_mount;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->byMountID = nMountID;

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_ON_MOUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoCharOffMount(CCharacter* poChar)
{
	INT32 nRetCode = 0;
	G2C_CHAR_OFF_MOUNT* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_CHAR_OFF_MOUNT*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_OFF_MOUNT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_off_mount;
	pMsg->dwCharID = poChar->GetObjID();

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_OFF_MOUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

void CCliMessageHandler::OnWingOp(CRole* poRole, const char* pData)
{
	C2G_WING_OP* pMsg = (C2G_WING_OP*)pData;

	poRole->WingOperate(pMsg->nOp, pMsg->nParam);
}

BOOL CCliMessageHandler::DoWingInfo(CRole* poRole, BOOL bUpgraded)
{
	INT32 nRetCode = 0;
	G2C_WING_INFO* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_WING_INFO*)m_poPacketAllocator->Alloc(sizeof(G2C_WING_INFO));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_wing_info;
	pMsg->nWingLevel = poRole->GetWingLevel();
	pMsg->dwOwnedMask = poRole->GetWingMask();
	pMsg->wCurrentWing = (UINT16)poRole->GetCurrentWingID();
	pMsg->byUpgraded = bUpgraded ? 1 : 0;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_WING_INFO));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharWing(CCharacter* poChar, INT32 nWingID)
{
	INT32 nRetCode = 0;
	G2C_CHAR_WING* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_CHAR_WING*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_WING));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_wing;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->wWingID = (UINT16)nWingID;

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_WING));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}
BOOL CCliMessageHandler::DoCombatPoseSwitch(CRole* poRole, INT32 nFlag)
{
	INT32 nRetCode = 0;
	G2C_COMBAT_POSE_SWITCH* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_COMBAT_POSE_SWITCH*)m_poPacketAllocator->Alloc(sizeof(G2C_COMBAT_POSE_SWITCH));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_combat_pose_switch;
	pMsg->dwCharID = poRole->GetObjID();
	pMsg->byCombatPoseOn = poRole->GetFlag().CheckFlag(cfCombatPoseOn);
	pMsg->byClientFlag = nFlag;

	nRetCode = _Broadcast9Region(poRole->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_COMBAT_POSE_SWITCH));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharMine(CCharacter* poChar, INT32 nMineInterval)
{
	INT32 nRetCode = 0;
	G2C_CHAR_MINE* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_CHAR_MINE*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_MINE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_mine;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->wDestX = ConvertCoordS2N(poChar->GetDestX());
	pMsg->wDestY = ConvertCoordS2N(poChar->GetDestY());
	pMsg->wMineInterval = (UINT16)nMineInterval;

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_MINE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSRecallTeamMember(CRole* poRole)
{
	INT32   nRetCode = 0;
	InternalMessage::G2M_RECALL_TEAM_MEMBER* pMsg = NULL;
	CScene* poScene = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (InternalMessage::G2M_RECALL_TEAM_MEMBER*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_RECALL_TEAM_MEMBER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_recall_team_member;
	pMsg->dwLeaderID = poRole->GetObjID();
	pMsg->nSceneTempID = poRole->GetSceneTempID();
	pMsg->nSceneID = poRole->GetSceneID();
	pMsg->nX = (INT32)poRole->GetX();
	pMsg->nY = (INT32)poRole->GetY();
	pMsg->nZ = (INT32)poRole->GetZ();

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_RECALL_TEAM_MEMBER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSSetServerVariable(INT32 nServerGroup, UINT64 qwKey, BOOL bAbsoluteValue, INT32 nVarValue)
{
	INT32   nRetCode = 0;
	InternalMessage::G2M_SET_SERVER_VARIABLE* pMsg = NULL;

	pMsg = (InternalMessage::G2M_SET_SERVER_VARIABLE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_SET_SERVER_VARIABLE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_set_server_variable;
	pMsg->byServerGroup = (UINT8)nServerGroup;
	pMsg->qwKey = qwKey;
	pMsg->bAbsoluteValue = (UINT8)bAbsoluteValue;
	pMsg->nVarValue = nVarValue;

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_RECALL_TEAM_MEMBER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

#endif  // _ROBOT
