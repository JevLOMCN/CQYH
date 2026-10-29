#include "stdafx.h"
#include "scene_object.h"

#include "id_def.h"
#include "game_world.h"
#include "scene_file_mgr.h"
#include "message_handler.h"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

CSceneObject::CSceneObject()
{
	m_dwObjID = INVALID_OBJECT_ID;
    m_RegionListNode.pNext = m_RegionListNode.pPrev = NULL;
    m_bDeleteSelf = FALSE;
	m_nGameFrame = 0;
	m_poScene = NULL;
	m_poRegion = NULL;
    m_pcGrid = NULL;
	m_pcZone = NULL;
	m_pGridInstance = NULL;
	m_llX = m_llY = m_llZ = 0LL;
	m_nFaceDirection = 0;
	m_IsRotating = FALSE;
	m_nDestDirection = 0;
    m_nOmiga = 0;
	m_nBodyType = bodyInvalid;
}

CSceneObject::~CSceneObject()
{
    UnInit();
}

BOOL CSceneObject::Init(UINT32 dwObjID)
{
    m_bDeleteSelf = FALSE;
    m_nGameFrame = 0;
    m_RegionListNode.pPrev = m_RegionListNode.pNext = NULL;
    m_bDeleteSelf = FALSE;

    m_poScene = NULL;
    m_poRegion = NULL;
    m_pcGrid = NULL;
	m_pcZone = NULL;
	m_pGridInstance = NULL;
    m_llX = m_llY = m_llZ = 0;
    m_nFaceDirection = 0;
    m_dwObjID = dwObjID;
	m_IsRotating = FALSE;
	m_nDestDirection = 0;
	m_nBodyType = body0x0;

	return TRUE;
}

BOOL CSceneObject::UnInit(void)
{
    CBaseList::Remove(&m_RegionListNode);

	return TRUE;
}

BOOL CSceneObject::RotateTo(INT32 nDestDirection, INT32 nDurationFrm)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(nDestDirection >= 0 && nDestDirection < MAX_DIRECTION_COUNT);
	PROCESS_SUCCESS(nDestDirection == m_nFaceDirection);

	m_IsRotating = TRUE;

	if (nDurationFrm != 0)
	{
		m_nOmiga = (nDestDirection - m_nFaceDirection + MAX_DIRECTION_COUNT) % MAX_DIRECTION_COUNT;
		m_nOmiga = m_nOmiga < MAX_DIRECTION_COUNT / 2 ? m_nOmiga : -m_nOmiga;
		m_nOmiga /= nDurationFrm;
		m_IsRotating = TRUE;
		m_nDestDirection = nDestDirection;
	}
	else
	{
		SetFaceDirection(nDestDirection);
		m_IsRotating = FALSE;
	}

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoObjectRotate(this, nDestDirection, nDurationFrm * 1000 / GAME_FPS);
	LOG_PROCESS_ERROR(nRetCode);
#endif

Exit1:
    return TRUE;

Exit0:
	return FALSE;
}

BOOL CSceneObject::MainLoop(void)
{
    m_nGameFrame++;

	if(m_IsRotating)
	{
		INT32 nCurDirection = GetFaceDirection();
		nCurDirection += m_nOmiga;

		nCurDirection = (nCurDirection + MAX_DIRECTION_COUNT) % MAX_DIRECTION_COUNT;

		if(abs(nCurDirection - m_nDestDirection) < abs(m_nOmiga))
		{
			m_IsRotating = FALSE;
			nCurDirection = m_nDestDirection;
		}

		SetFaceDirection(nCurDirection);
	}

	return TRUE;
}

BOOL CSceneObject::CheckLoop(void)
{
	return m_nGameFrame < CGameWorld::GetInstance().GetGameFrame();
}

void CSceneObject::DeleteSelf(void)
{
    m_bDeleteSelf = TRUE;
}

void CSceneObject::SetBodyType(INT32 nBodyType)
{
	INT32 nRetCode = 0;

	if (m_nBodyType != nBodyType)
	{
		if (m_poScene)
		{
			nRetCode = CheckGridCharRef(nBodyType);
			LOG_PROCESS_ERROR(nRetCode);

			nRetCode = ClearGridCharRef();
			LOG_PROCESS_ERROR(nRetCode);

			m_nBodyType = nBodyType;

			nRetCode = SetGridCharRef();
			LOG_PROCESS_ERROR(nRetCode);
		}
		else
		{
			m_nBodyType = nBodyType;
		}
	}

Exit0:

	return;
}

BOOL CSceneObject::CheckGridCharRef(INT32 nBodyType)
{
	const GRID_DATA* pcGridData = NULL;
	const ZONE_DATA* pcZoneData = NULL;
	GRID_INSTANCE_DATA* pGridInstance = NULL;

	LOG_PROCESS_ERROR(m_poScene);

	switch (nBodyType)
	{
	case body3x3:
		for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
		{
			m_poScene->GetLowerGrid(m_llX + GRID_WIDTH * g_nAdjacentX[nDirection],
				m_llY + GRID_WIDTH * g_nAdjacentY[nDirection], m_llZ + GRID_WIDTH, pcZoneData, pcGridData);
			pGridInstance = m_poScene->GetGridInstance(pcZoneData, pcGridData);
			PROCESS_ERROR(pcGridData && pGridInstance);
			PROCESS_ERROR(pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
			if (!pcGridData->bNotBlockChar)
				PROCESS_ERROR(pGridInstance->uCharRef == 0);
		}
	case body1x1:
		PROCESS_ERROR(m_pcGrid && m_pGridInstance);
		PROCESS_ERROR(m_pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
		if (!m_pcGrid->bNotBlockChar)
			PROCESS_ERROR(m_pGridInstance->uCharRef == 0);
		break;
	case body0x0:
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CSceneObject::CheckGridCharRef(CScene* poScene, INT64 llX, INT64 llY, INT64 llZ)
{
	const GRID_DATA* pcGridData = NULL;
	const ZONE_DATA* pcZoneData = NULL;
	GRID_INSTANCE_DATA* pGridInstance = NULL;

	LOG_PROCESS_ERROR(poScene);

	switch (m_nBodyType)
	{
	case body3x3:
		for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
		{
			poScene->GetLowerGrid(llX + GRID_WIDTH * g_nAdjacentX[nDirection],
				llY + GRID_WIDTH * g_nAdjacentY[nDirection], llZ + GRID_WIDTH, pcZoneData, pcGridData);
			pGridInstance = poScene->GetGridInstance(pcZoneData, pcGridData);
			PROCESS_ERROR(pcGridData && pGridInstance);
			PROCESS_ERROR(pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
			if (!pcGridData->bNotBlockChar)
				PROCESS_ERROR(pGridInstance->uCharRef == 0);
		}
	case body1x1:
		{
			poScene->GetLowerGrid(llX, llY, llZ + GRID_WIDTH, pcZoneData, pcGridData);
			pGridInstance = poScene->GetGridInstance(pcZoneData, pcGridData);
			PROCESS_ERROR(pcGridData && pGridInstance);
			PROCESS_ERROR(pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
			if (!pcGridData->bNotBlockChar)
				PROCESS_ERROR(pGridInstance->uCharRef == 0);
		}
		break;
	case body0x0:
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CSceneObject::CheckAdjGridCharRef(INT64 llX, INT64 llY, INT64 llZ)
{
	INT32 nDirection8 = 0;
	const GRID_DATA* pcGridData = NULL;
	const ZONE_DATA* pcZoneData = NULL;
	GRID_INSTANCE_DATA* pGridInstance = NULL;

	LOG_PROCESS_ERROR(m_poScene);

	nDirection8 = ConvertToDir8(g_GetDirection(m_llX, m_llY, llX, llY));
	LOG_PROCESS_ERROR(nDirection8 >= 0 && nDirection8 < DT_DIRECTION_8_COUNT);

	for (INT32 nIndex = 1; nIndex <= g_nAdjList[nDirection8][0]; nIndex++)
	{
		INT32 nDirection = g_nAdjList[nDirection8][nIndex];
		m_poScene->GetLowerGrid(llX + GRID_WIDTH * g_nAdjacentX[nDirection],
			llY + GRID_WIDTH * g_nAdjacentY[nDirection], llZ + GRID_WIDTH, pcZoneData, pcGridData);
		pGridInstance = m_poScene->GetGridInstance(pcZoneData, pcGridData);
		PROCESS_ERROR(pcGridData && pGridInstance);
		PROCESS_ERROR(pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
		if (!pcGridData->bNotBlockChar)
			PROCESS_ERROR(pGridInstance->uCharRef == 0);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSceneObject::SetGridCharRef(void)
{
	const GRID_DATA* pcGridData = NULL;
	const ZONE_DATA* pcZoneData = NULL;
	GRID_INSTANCE_DATA* pGridInstance = NULL;

	LOG_PROCESS_ERROR(m_poScene);

	switch (m_nBodyType)
	{
	case body3x3:
		for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
		{
			m_poScene->GetLowerGrid(m_llX + GRID_WIDTH * g_nAdjacentX[nDirection],
				m_llY + GRID_WIDTH * g_nAdjacentY[nDirection], m_llZ + GRID_WIDTH, pcZoneData, pcGridData);
			pGridInstance = m_poScene->GetGridInstance(pcZoneData, pcGridData);
			LOG_PROCESS_ERROR(pcGridData && pGridInstance);
			LOG_PROCESS_ERROR(pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
			pGridInstance->uCharRef++;
		}
	case body1x1:
		{
			LOG_PROCESS_ERROR(m_pGridInstance);
			LOG_PROCESS_ERROR(m_pGridInstance->uCharRef < MAX_CHAR_PER_GRID);
			m_pGridInstance->uCharRef++;
		}
		break;
	case body0x0:
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CSceneObject::ClearGridCharRef(void)
{
	const GRID_DATA* pcGridData = NULL;
	const ZONE_DATA* pcZoneData = NULL;
	GRID_INSTANCE_DATA* pGridInstance = NULL;

	LOG_PROCESS_ERROR(m_poScene);

	switch (m_nBodyType)
	{
	case body3x3:
		for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
		{
			m_poScene->GetLowerGrid(m_llX + GRID_WIDTH * g_nAdjacentX[nDirection],
				m_llY + GRID_WIDTH * g_nAdjacentY[nDirection], m_llZ + GRID_WIDTH, pcZoneData, pcGridData);
			pGridInstance = m_poScene->GetGridInstance(pcZoneData, pcGridData);
			LOG_PROCESS_ERROR(pcGridData && pGridInstance);
			LOG_PROCESS_ERROR(pGridInstance->uCharRef > 0);
			pGridInstance->uCharRef--;
		}
	case body1x1:
		{
			LOG_PROCESS_ERROR(m_pcGrid && m_pGridInstance);
			LOG_PROCESS_ERROR(m_pGridInstance->uCharRef > 0);
			m_pGridInstance->uCharRef--;
		}
		break;
	case body0x0:
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CSceneObject::CanTeleport(void)
{
	LOG_PROCESS_ERROR(m_pcGrid);

	return !m_pcGrid->bCannotTeleport;
Exit0:
	return FALSE;
}

BOOL CSceneObject::_ChangeRegion(CRegion* poDestRegion)
{
	return FALSE;
}

BOOL CSceneObject::_GridMove(INT64 llDestX, INT64 llDestY)
{
	return FALSE;
}

void GetOffsetedPos(INT64 llX, INT64 llY, INT32 nDirection, INT32 nHOffsetGrids, INT32 nVOffsetGrids, INT64& llOutX, INT64& llOutY)
{
	INT32 nDirection8 = ConvertToDir8(nDirection);
	INT32 nDirection8V = NormalizeDirection8(nDirection8 - 2);

	llOutX = llX;
	llOutY = llY;

	llOutX += nHOffsetGrids * g_nAdjacentX[nDirection8V] * GRID_WIDTH;
	llOutY += nHOffsetGrids * g_nAdjacentY[nDirection8V] * GRID_WIDTH;

	llOutX += nVOffsetGrids * g_nAdjacentX[nDirection8] * GRID_WIDTH;
	llOutY += nVOffsetGrids * g_nAdjacentY[nDirection8] * GRID_WIDTH;
}
