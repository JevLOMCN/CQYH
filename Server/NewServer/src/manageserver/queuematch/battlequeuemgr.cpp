#include "stdafx.h"
#include "battlequeuemgr.h"

#include <time.h>

#include "time_ex.h"

#include "id_def.h"

#include "manageserver.h"
#include "role_data_mgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "scene_data.h"
#include "data_manager.h"
#include "scene/scenemgr.h"
#include "team/team.h"
#include "message_handler.h"
#include "ms_session_mgr.h"

CBattleQueueMgr CBattleQueueMgr::ms_Instance;

CBattleQueueMgr::CBattleQueueMgr()
{

}

CBattleQueueMgr::~CBattleQueueMgr()
{

}

BOOL CBattleQueueMgr::Init(VOID)
{
	INT32 nRetCode = 0;
	memset(m_lstBattleScene, 0, sizeof(m_lstBattleScene));
	for(INT32 i = 0; i < MAX_SCENE_TEMPLATE_COUNT; ++i)
	{
		m_lstBattleScene[i].dwSceneTempID = 0;
		memset(&(m_lstBattleScene[i].stStatisticsBattleData), 0, sizeof(SStatisticsDataForBattle));
		for(int j = 0; j < MAX_SIDE_COUNT; j++)
		{
			m_lstBattleScene[i].m_lstNewBattleQueue[j].Clear();
			m_lstBattleScene[i].m_lstNewBattleQueueRoleCount[j] = 0;
		}
	}

	for(INT32 i = 0; i < bttTotal; ++i)
		m_lstBattleQueue.Clear();
	
	m_lstBattleTempRole.Clear();

	memset(m_BattleMatchToSceneTempID, 0, sizeof(m_BattleMatchToSceneTempID));

	nRetCode = m_BattleQueueInfoPool.Init(32);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_BattleTempRolePool.Init(32);
	LOG_PROCESS_ERROR(nRetCode);

	CTraverseSceneDataForBattle stTraverseSceneDataForBattle;
	nRetCode = CDataManager<SCENE_DATA>::GetInstance().Traverse(stTraverseSceneDataForBattle);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::UnInit(VOID)
{
	INT32 nRetCode = 0;

	for(INT32 i = 0; i < MAX_SCENE_TEMPLATE_COUNT; ++i)
	{
		m_lstBattleScene[i].dwSceneTempID = 0;
		memset(&(m_lstBattleScene[i].stStatisticsBattleData), 0, sizeof(SStatisticsDataForBattle));
		for(int j = 0; j < MAX_SIDE_COUNT; j++)
			m_lstBattleScene[i].m_lstNewBattleQueue[j].Clear();
	}

	for(INT32 i = 0; i < bttTotal; ++i)
		m_lstBattleQueue.Clear();

	nRetCode = m_BattleQueueInfoPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_BattleTempRolePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

CBattleQueueMgr& CBattleQueueMgr::GetInstance(VOID)
{
	return ms_Instance;
}

SBattleQueueInfo* CBattleQueueMgr::FindBattleQueneInfo(UINT32 dwBattleQueneInfoID)
{
	return m_BattleQueueInfoPool.Find(dwBattleQueneInfoID);
}

SBattleQueueInfo* CBattleQueueMgr::CreateBattleQueneInfo(UINT32 dwBattleQueneInfoID)
{
	SBattleQueueInfo* pResult = NULL;

	pResult = m_BattleQueueInfoPool.New(dwBattleQueneInfoID);
	LOG_PROCESS_ERROR(pResult);
	
	pResult->dwBattleQueueID = dwBattleQueneInfoID;
	pResult->pPrev = pResult->pNext = NULL;
	pResult->dwBattleMatch = 0;

	pResult->bySide = 0xFF;
	return pResult;
Exit0:
	return NULL;
}

BOOL CBattleQueueMgr::DestroyBattleQueneInfo(SBattleQueueInfo* stBattleQueneInfo)
{	
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(stBattleQueneInfo);
	LOG_PROCESS_ERROR(stBattleQueneInfo->pNext == NULL);
	LOG_PROCESS_ERROR(stBattleQueneInfo->pPrev == NULL);

	nRetCode = m_BattleQueueInfoPool.Delete(stBattleQueneInfo);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::JoinBattle(UINT32 dwID, UINT32 dwRoleCount, UINT32 dwMatch)
{
	INT32 nRetCode = 0;
	SBattleQueueInfo* pstBattleQueue = NULL;
	CTeam* poTeam = NULL;
	MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;

	LOG_PROCESS_ERROR(dwID > 0);
	LOG_PROCESS_ERROR(dwRoleCount > 0);
	LOG_PROCESS_ERROR(dwMatch > 0);

	nRetCode = _UpdateBattleTempRole();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _AddtoCurBattle(dwID, dwRoleCount, dwMatch);
	PROCESS_SUCCESS(nRetCode);

	poTeam = CTeamMgr::GetInstance().Find(dwID);
	if(poTeam)
	{
		for(INT32 i = 0; i < poTeam->GetMemberSizeLimit(); ++i)
		{
			MS_ONLINE_ROLE_DATA* poTeamMember = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleIDInGroup(poTeam->GetMemberID(i), poTeam->GetGroupID());
			if(poTeamMember)
			{
				memset(&(poTeamMember->stBattleQueue), 0, sizeof(poTeamMember->stBattleQueue));
				poTeamMember->stBattleQueue.byQueue = qsInQueue;

				nRetCode = CMessageHandler::GetInstance().DoClientJoinBattleQueue(poTeamMember->dwCliSessionID);
				LOG_CHECK_ERROR(nRetCode);
			}
		}
	}
	else
	{
		pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwID);
		PROCESS_ERROR(pstOnlineRole);

		memset(&(pstOnlineRole->stBattleQueue), 0, sizeof(pstOnlineRole->stBattleQueue));

		pstOnlineRole->stBattleQueue.byQueue = qsInQueue;

		nRetCode = CMessageHandler::GetInstance().DoClientJoinBattleQueue(pstOnlineRole->dwCliSessionID);
		LOG_CHECK_ERROR(nRetCode);
	}

	pstBattleQueue = CBattleQueueMgr::GetInstance().CreateBattleQueneInfo(dwID);
	LOG_PROCESS_ERROR(pstBattleQueue);

	pstBattleQueue->dwRoleCount = dwRoleCount;
	pstBattleQueue->dwBattleMatch = dwMatch;

	nRetCode = StartQueue(pstBattleQueue);
	LOG_PROCESS_ERROR(nRetCode);

Exit1:
	return TRUE;
Exit0:
	if(pstBattleQueue)
	{
		nRetCode = StopQueue(pstBattleQueue);
		LOG_CHECK_ERROR(nRetCode);
	}
	return FALSE;
}

BOOL CBattleQueueMgr::CancelJoinBattle(UINT32 dwRoleID)
{
	INT32 nRetCode = 0;
	SBattleQueueInfo* pstBattleQueueInfo = NULL;
	MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;
	CTeam* poTeam = NULL;

	pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
	LOG_PROCESS_ERROR(pstOnlineRole);

	poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
	if(poTeam)
	{
		//LOG_PROCESS_ERROR(dwRoleID == poTeam->GetLeader());

		pstBattleQueueInfo = CBattleQueueMgr::GetInstance().FindBattleQueneInfo(poTeam->GetTeamID());
		LOG_PROCESS_ERROR(pstBattleQueueInfo);

		nRetCode = CBattleQueueMgr::GetInstance().StopQueue(pstBattleQueueInfo);
		LOG_PROCESS_ERROR(nRetCode);

		for(INT32 i = 0; i < poTeam->GetMemberSizeLimit(); ++i)
		{
			MS_ONLINE_ROLE_DATA* poTeamMember = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleIDInGroup(poTeam->GetMemberID(i), poTeam->GetGroupID());
			if(poTeamMember)
			{
				poTeamMember->stBattleQueue.byQueue = qsNotInQueue;

				nRetCode = CMessageHandler::GetInstance().DoClientCancelJoinBattleAck(poTeamMember->dwCliSessionID, dwRoleID);
				LOG_CHECK_ERROR(nRetCode);
			}
		}

		nRetCode = CBattleQueueMgr::GetInstance().DestroyBattleQueneInfo(pstBattleQueueInfo);
		LOG_CHECK_ERROR(nRetCode);
	}
	else
	{
		pstBattleQueueInfo = CBattleQueueMgr::GetInstance().FindBattleQueneInfo(dwRoleID);
		LOG_PROCESS_ERROR(pstBattleQueueInfo);

		nRetCode = CBattleQueueMgr::GetInstance().StopQueue(pstBattleQueueInfo);
		LOG_PROCESS_ERROR(nRetCode);

		pstOnlineRole->stBattleQueue.byQueue = qsNotInQueue;

		nRetCode = CMessageHandler::GetInstance().DoClientCancelJoinBattleAck(pstOnlineRole->dwCliSessionID, dwRoleID);
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = CBattleQueueMgr::GetInstance().DestroyBattleQueneInfo(pstBattleQueueInfo);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::StartQueue(SBattleQueueInfo* stBattleQueueInfo)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(stBattleQueueInfo);
	LOG_PROCESS_ERROR(stBattleQueueInfo->pNext == NULL);
	LOG_PROCESS_ERROR(stBattleQueueInfo->pPrev == NULL);
	LOG_PROCESS_ERROR(stBattleQueueInfo->dwBattleMatch > 0);
	LOG_PROCESS_ERROR(stBattleQueueInfo->dwRoleCount > 0);

	stBattleQueueInfo->dwStartTime = CManageServer::GetInstance().GetGameTime();

	nRetCode = _AddtoNewBattle(stBattleQueueInfo);
	PROCESS_SUCCESS(nRetCode);

	nRetCode = m_lstBattleQueue.PushRear(stBattleQueueInfo);
	LOG_PROCESS_ERROR(nRetCode);
		
Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::StopQueue(SBattleQueueInfo* stBattleQueueInfo)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(stBattleQueueInfo);
	LOG_PROCESS_ERROR(stBattleQueueInfo->pNext != NULL || stBattleQueueInfo->pPrev != NULL);
	LOG_PROCESS_ERROR(stBattleQueueInfo->dwBattleMatch > 0);

	if(stBattleQueueInfo->bySide == 0xFF)
	{		
		nRetCode = m_lstBattleQueue.Remove(stBattleQueueInfo);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		UINT32 dwSceneTempID = stBattleQueueInfo->dwBattleMatch;
		LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
		LOG_PROCESS_ERROR(m_lstBattleScene[dwSceneTempID].dwSceneTempID == dwSceneTempID);
		LOG_PROCESS_ERROR(stBattleQueueInfo->bySide < m_lstBattleScene[dwSceneTempID].bySideCount);

		nRetCode = m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueue[stBattleQueueInfo->bySide].Remove(stBattleQueueInfo);
		LOG_PROCESS_ERROR(nRetCode);
		m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[stBattleQueueInfo->bySide] -= stBattleQueueInfo->dwRoleCount;
	}

	stBattleQueueInfo->dwStopTime = CManageServer::GetInstance().GetGameTime();

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::AddBattleInfo(UINT32 dwSceneTempID, UINT32 byBattleType, UINT8 byMinRole, UINT8 byMaxRole, UINT8 bySide)
{
	LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);

	m_lstBattleScene[dwSceneTempID].dwSceneTempID = dwSceneTempID;
	m_lstBattleScene[dwSceneTempID].bySideCount = bySide;
	for(INT32 i = 0; i < bySide; ++i)
	{
		m_lstBattleScene[dwSceneTempID].byMinRoleCount[i] = byMinRole;
		m_lstBattleScene[dwSceneTempID].byMaxRoleCount[i] = byMaxRole;
	}

	for(INT32 i = 0; i < 32; ++i)
	{
		if(m_BattleMatchToSceneTempID[i] == 0)
		{
			m_BattleMatchToSceneTempID[i] = dwSceneTempID;
			break;
		}
		else if(dwSceneTempID < m_BattleMatchToSceneTempID[i])
		{
			for(INT32 j = 31; j > i; j-- )
				m_BattleMatchToSceneTempID[j] = m_BattleMatchToSceneTempID[j-1];
			m_BattleMatchToSceneTempID[i] = dwSceneTempID;
		}
	}
	return TRUE;
Exit0:
	return FALSE;
}

struct CTraverseSceneBySceneTempID
{
	UINT32 bFind;
	UINT32 dwRoleCount;
	SBattleScene* pstBattleScene;

	UINT32 nSceneID;
	UINT8  bySide;
	BOOL operator()(SCENE_INFO* pstSceneInfo)
	{
		INT32 nRetCode = 0;
		INT32 index = 0;

		PROCESS_SUCCESS(bFind);
		PROCESS_SUCCESS(!pstSceneInfo);
		PROCESS_SUCCESS(pstSceneInfo->eState != ESCENESTATE_NORMAL);
		PROCESS_SUCCESS(!pstBattleScene);

		for(INT32 i = 1; i < pstSceneInfo->nSideCount; ++i)
		{
			if(pstSceneInfo->nSideCurRoleCount[i] < pstSceneInfo->nSideCurRoleCount[index])
				index = i;
		}

		PROCESS_SUCCESS(pstSceneInfo->nSideCurRoleCount[index] + dwRoleCount > pstBattleScene->byMaxRoleCount[index]);

		for(INT32 i = 0; i < pstSceneInfo->nSideCount; ++i)
		{
			if(i == index)
				continue;

			PROCESS_SUCCESS(pstSceneInfo->nSideCurRoleCount[index] + (UINT8)dwRoleCount > pstSceneInfo->nSideCurRoleCount[i] + 1);
		}

		nSceneID = pstSceneInfo->nSceneID;
		bySide = index;
		bFind = TRUE;

Exit1:
		return TRUE;
	}
};

BOOL CBattleQueueMgr::_GetBattleScene(UINT32 dwSceneTempID, UINT32 dwRoleCount, UINT32& nSceneID, UINT8& bySide)
{
	INT32 nRetCode = 0;

	CTraverseSceneBySceneTempID stTraverseSceneBySceneTempID;
	stTraverseSceneBySceneTempID.bFind = FALSE;
	stTraverseSceneBySceneTempID.nSceneID = 0;
	stTraverseSceneBySceneTempID.bySide = 0;

	LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
	LOG_PROCESS_ERROR(m_lstBattleScene[dwSceneTempID].dwSceneTempID == dwSceneTempID);
	
	stTraverseSceneBySceneTempID.pstBattleScene = &m_lstBattleScene[dwSceneTempID];
	stTraverseSceneBySceneTempID.dwRoleCount = dwRoleCount;

	nRetCode = CSceneMgr::GetInstance().TraverseSceneBySceneTempID(dwSceneTempID, stTraverseSceneBySceneTempID);
	PROCESS_ERROR(nRetCode);
	PROCESS_ERROR(stTraverseSceneBySceneTempID.bFind);

	nSceneID = stTraverseSceneBySceneTempID.nSceneID;
	bySide = stTraverseSceneBySceneTempID.bySide;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::_AddtoCurBattle(UINT32 dwID, UINT32 dwRoleCount, UINT32 dwMatch)
{
	INT32 dwSceneTempID = 0;
	INT32 nRetCode = 0;

	UINT32 dwSceneID = 0;
	UINT8 bySide = 0;
	BOOL bFlag = FALSE;

	LOG_PROCESS_ERROR(dwID > 0);
	LOG_PROCESS_ERROR(dwRoleCount > 0);
	LOG_PROCESS_ERROR(dwMatch > 0);

	for(INT32 i = 0; i < 32; ++i)
	{
		SCENE_INFO* pstEnterScene = NULL;

		if(!(dwMatch & (1<<i)))
			continue;

		dwSceneTempID = m_BattleMatchToSceneTempID[i];
		if(dwSceneTempID == 0)
			continue;
		
		nRetCode = _GetBattleScene(dwSceneTempID, dwRoleCount, dwSceneID, bySide);
		if(!nRetCode)
			continue;
		
		pstEnterScene = CSceneMgr::GetInstance().FindScene(dwSceneTempID, dwSceneID);
		if(!pstEnterScene)
			continue;

		nRetCode = _EnterBattle(pstEnterScene, bySide, dwID);	
		LOG_PROCESS_ERROR(nRetCode);
		
		bFlag = TRUE;
		break;
	}

	PROCESS_ERROR(bFlag);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::_AddtoNewBattle(SBattleQueueInfo* stBattleQueueInfo)
{
	INT32 dwSceneTempID = 0;
	INT32 nRetCode = 0;
	BOOL bFind = FALSE;

	LOG_PROCESS_ERROR(stBattleQueueInfo);

	for(INT32 i = 0; i < 32; ++i)
	{
		if(!(stBattleQueueInfo->dwBattleMatch & (1<<i)))
			continue;

		dwSceneTempID = m_BattleMatchToSceneTempID[i];
		if(dwSceneTempID == 0)
			continue;

		LOG_PROCESS_ERROR(m_lstBattleScene[dwSceneTempID].dwSceneTempID == dwSceneTempID);
		
		UINT8 dwIndex = 0;
		for(INT32 j = 1; j < m_lstBattleScene[dwSceneTempID].bySideCount; ++j)
		{
			if(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[j] < m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[dwIndex])
				dwIndex = j;
		}

		if(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[dwIndex] + stBattleQueueInfo->dwRoleCount > m_lstBattleScene[dwSceneTempID].byMaxRoleCount[dwIndex])
			continue;

		stBattleQueueInfo->bySide = dwIndex;
		stBattleQueueInfo->dwBattleMatch = dwSceneTempID;
		m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueue[dwIndex].PushRear(stBattleQueueInfo);
		m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[dwIndex] += stBattleQueueInfo->dwRoleCount;
		bFind = TRUE;

		break;
	}

	PROCESS_ERROR(bFind);
	
	while(_CheckStartNewBattle(dwSceneTempID))
	{
		nRetCode = _StartNewBattle(dwSceneTempID);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = _UpdateNewBattle(dwSceneTempID);
		LOG_PROCESS_ERROR(nRetCode);
	}	

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::_CheckStartNewBattle(UINT32 dwSceneTempID)
{
	INT32 nRetCode = 0;
	BOOL bFlag = TRUE;

	LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
	LOG_PROCESS_ERROR(m_lstBattleScene[dwSceneTempID].dwSceneTempID == dwSceneTempID);

	for(INT32 i = 0; i < m_lstBattleScene[dwSceneTempID].bySideCount; ++i)
	{
		if(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[i] < m_lstBattleScene[dwSceneTempID].byMinRoleCount[i])
		{
			bFlag = FALSE;
			break;
		}
	}

	return bFlag;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::_StartNewBattle(UINT32 dwSceneTempID)
{
	INT32 nRetCode = 0;
	UINT32 dwSuitableGS = 0;
	SCENE_INFO* poSceneInfo = NULL;
	const SCENE_DATA* poSceneData = NULL;

	LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
	LOG_PROCESS_ERROR(m_lstBattleScene[dwSceneTempID].dwSceneTempID == dwSceneTempID);

	poSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(dwSceneTempID);
	LOG_PROCESS_ERROR(poSceneData);

	dwSuitableGS = CMSSessionMgr::GetInstance().GetSuitableGS(0);
	LOG_PROCESS_ERROR(dwSuitableGS > 0);

	poSceneInfo = CSceneMgr::GetInstance().CreateScene(dwSceneTempID, 0, 0);
	LOG_PROCESS_ERROR(poSceneInfo);

	poSceneInfo->dwGSSessionID = dwSuitableGS;
	poSceneInfo->eState = ESCENESTATE_CREATING;
	poSceneInfo->dwCreateTime = CManageServer::GetInstance().GetGameTime();
	poSceneInfo->WaitUserList.empty();

	nRetCode = CMessageHandler::GetInstance().DoGSCreateScene(dwSuitableGS, poSceneInfo->nServerGroupID,
		dwSceneTempID, poSceneInfo->nSceneID);
	LOG_PROCESS_ERROR(nRetCode);
	
	for(INT32 i = 0; i < m_lstBattleScene[dwSceneTempID].bySideCount; ++i)
	{
		BASE_LIST_NODE* pNextNode = NULL;
		BASE_LIST_NODE* pCurNode = m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueue[i].Head().pNext;
		poSceneInfo->nSideCurRoleCount[i] = 0;

		while(pCurNode != &(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueue[i].Rear()))
		{
			pNextNode = pCurNode->pNext;
			SBattleQueueInfo* stBattleQueueInfo = (SBattleQueueInfo*)pCurNode;

			nRetCode = _EnterBattle(poSceneInfo, i, stBattleQueueInfo->dwBattleQueueID);
			LOG_CHECK_ERROR(nRetCode);

			nRetCode = StopQueue(stBattleQueueInfo);
			LOG_CHECK_ERROR(nRetCode);

			nRetCode = DestroyBattleQueneInfo(stBattleQueueInfo);
			LOG_CHECK_ERROR(nRetCode);

			pCurNode = pNextNode;
		}

		LOG_CHECK_ERROR(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueue[i].IsEmpty());
		LOG_CHECK_ERROR(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[i] == 0);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::_UpdateNewBattle(UINT32 dwSceneTempID)
{
	INT32 nRetCode = 0;
	BASE_LIST_NODE* pCurNode = NULL;
	BASE_LIST_NODE* pNextNode = NULL;
	const SCENE_DATA* poSceneData = NULL;

	LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
	LOG_PROCESS_ERROR(m_lstBattleScene[dwSceneTempID].dwSceneTempID == dwSceneTempID);

	poSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(dwSceneTempID);
	LOG_PROCESS_ERROR(poSceneData);

	pCurNode = m_lstBattleQueue.Head().pNext;
	while(pCurNode != &(m_lstBattleQueue.Rear()))
	{
		pNextNode = pCurNode->pNext;

		SBattleQueueInfo* poQueueInfo = (SBattleQueueInfo*)pCurNode;
		if(!poQueueInfo)
		{
			pCurNode = pNextNode;
			continue;
		}
		
		BOOL bFlag = FALSE;
		for(INT32 i = 0; i < 32; ++i)
		{
			if((poQueueInfo->dwBattleMatch & (1<<i)) && (m_BattleMatchToSceneTempID[i] == dwSceneTempID))
			{
				bFlag = TRUE;
				break;
			}
		}

		if(!bFlag)
			continue;

		INT32 index = 0;
		for(INT32 i = 0; i < m_lstBattleScene[dwSceneTempID].bySideCount; ++i)
		{
			if(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[i] < m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[index])
				index = i;

			if(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[i] < m_lstBattleScene[dwSceneTempID].byMaxRoleCount[i])
				bFlag = FALSE;
		}
		PROCESS_SUCCESS(bFlag);

		if(m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[index] + poQueueInfo->dwRoleCount > m_lstBattleScene[dwSceneTempID].byMaxRoleCount[index])
		{
			pCurNode = pNextNode;
			continue;
		}

		nRetCode = m_lstBattleQueue.Remove(poQueueInfo);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueue[index].PushRear(poQueueInfo);
		LOG_PROCESS_ERROR(nRetCode);
		m_lstBattleScene[dwSceneTempID].m_lstNewBattleQueueRoleCount[index] += poQueueInfo->dwRoleCount;
		poQueueInfo->bySide = index;
		poQueueInfo->dwBattleMatch = dwSceneTempID;

		pCurNode = pNextNode;
	}
	
Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::_EnterBattle(SCENE_INFO* pstEnterScene, UINT8 bySide, UINT32 dwID)
{
	INT32 nRetCode = 0;
	INT32 nObjType = 0;
	const SCENE_DATA* pstSceneData = NULL;
	UINT32 dwEnterTime = CManageServer::GetInstance().GetGameTime();

	LOG_PROCESS_ERROR(pstEnterScene);

	pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pstEnterScene->nSceneTempID);
	LOG_PROCESS_ERROR(pstSceneData);

	nObjType = GetTypeFromID(dwID);
	if(nObjType == otTeam)
	{
		CTeam* poTeam = NULL;

		poTeam = CTeamMgr::GetInstance().Find(dwID);
		LOG_PROCESS_ERROR(poTeam);

		for(INT32 i = 0; i < poTeam->GetMemberSizeLimit(); ++i)
		{
			MS_ONLINE_ROLE_DATA* poMember = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleIDInGroup(poTeam->GetMemberID(i), poTeam->GetGroupID());
			SBattleTempRole* pstTempRole = NULL;
			if(!poMember)
				continue;

			if(poMember->pRoleData->byLevel < pstSceneData->nMinPlayerLevel || poMember->pRoleData->byLevel > pstSceneData->nMaxPlayerLevel)
			{
				KICK_USER(poMember->dwCliSessionID, ktLogged);
				continue;
			}

			nRetCode = CMessageHandler::GetInstance().DoClientJoinBattleConfirmReq(poMember->dwCliSessionID, pstEnterScene->nSceneTempID);
			LOG_CHECK_ERROR(nRetCode);

			pstTempRole = m_BattleTempRolePool.New(poMember->pRoleData->dwRoleID);
			LOG_PROCESS_ERROR(pstTempRole);

			pstTempRole->bySide = bySide;
			pstTempRole->dwRoleID = poMember->pRoleData->dwRoleID;
			pstTempRole->dwSceneTempID = pstEnterScene->nSceneTempID;
			pstTempRole->dwSceneID = pstEnterScene->nSceneID;
			pstTempRole->dwTimer = CManageServer::GetInstance().GetGameTime() + MAX_JOIN_CONFIRM_TIME;
			pstTempRole->pPrev = pstTempRole->pNext = NULL;

			nRetCode = m_lstBattleTempRole.PushRear(pstTempRole);
			LOG_CHECK_ERROR(nRetCode);

			poMember->stBattleQueue.byQueue = qsQueueConfirm;
			poMember->stBattleQueue.dwJoinConfirmTimeOut = dwEnterTime + MAX_JOIN_CONFIRM_TIME;
			poMember->stBattleQueue.dwJoinSceneTempID = pstEnterScene->nSceneTempID;
			poMember->stBattleQueue.dwJoinSceneID = pstEnterScene->nSceneID;
			poMember->stBattleQueue.dwJoinSide = bySide;

			pstEnterScene->nSideCurRoleCount[bySide]++;
		}
	}
	else if(nObjType == otRole)
	{
		MS_ONLINE_ROLE_DATA* pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwID);
		if(pOnlineRole)
		{
			SBattleTempRole* pstTempRole = NULL;

			if(pOnlineRole->pRoleData->byLevel < pstSceneData->nMinPlayerLevel || pOnlineRole->pRoleData->byLevel > pstSceneData->nMaxPlayerLevel)
			{
				KICK_USER(pOnlineRole->dwCliSessionID, ktLogged);
			}
			else
			{
				nRetCode = CMessageHandler::GetInstance().DoClientJoinBattleConfirmReq(pOnlineRole->dwCliSessionID, pstEnterScene->nSceneTempID);
				LOG_CHECK_ERROR(nRetCode);

				pstTempRole = m_BattleTempRolePool.New(pOnlineRole->pRoleData->dwRoleID);
				LOG_PROCESS_ERROR(pstTempRole);

				pstTempRole->bySide = bySide;
				pstTempRole->dwRoleID = pOnlineRole->pRoleData->dwRoleID;
				pstTempRole->dwSceneTempID = pstEnterScene->nSceneTempID;
				pstTempRole->dwSceneID = pstEnterScene->nSceneID;
				pstTempRole->dwTimer = CManageServer::GetInstance().GetGameTime() + MAX_JOIN_CONFIRM_TIME;
				pstTempRole->pPrev = pstTempRole->pNext = NULL;

				nRetCode = m_lstBattleTempRole.PushRear(pstTempRole);
				LOG_CHECK_ERROR(nRetCode);

				pOnlineRole->stBattleQueue.byQueue = qsQueueConfirm;
				pOnlineRole->stBattleQueue.dwJoinConfirmTimeOut = CManageServer::GetInstance().GetGameTime() + MAX_JOIN_CONFIRM_TIME;
				pOnlineRole->stBattleQueue.dwJoinSceneTempID = pstEnterScene->nSceneTempID;
				pOnlineRole->stBattleQueue.dwJoinSceneID = pstEnterScene->nSceneID;
				pOnlineRole->stBattleQueue.dwJoinSide = bySide;

				pstEnterScene->nSideCurRoleCount[bySide]++;
			}
		}
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:
	return FALSE;
}	

BOOL CBattleQueueMgr::_UpdateBattleTempRole()
{
	INT32 nRetCode = 0;
	BASE_LIST_NODE* pNextNode = NULL;
	BASE_LIST_NODE* pCurNode = m_lstBattleTempRole.Head().pNext;

	BASE_LIST_NODE* pBattleSceneNode = NULL;
	while(pCurNode != &(m_lstBattleTempRole.Rear()))
	{
		SCENE_INFO* pstScene = NULL;
		pNextNode = pCurNode->pNext;
		SBattleTempRole* pstTempRole = (SBattleTempRole*)pCurNode;
		
		PROCESS_SUCCESS(pstTempRole->dwTimer > CManageServer::GetInstance().GetGameTime());

		pstScene = CSceneMgr::GetInstance().FindScene(pstTempRole->dwSceneTempID, pstTempRole->dwSceneID);
		if(pstScene)
			pstScene->nSideCurRoleCount[pstTempRole->bySide]--;
		
		nRetCode = m_lstBattleTempRole.Remove(pstTempRole);
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = m_BattleTempRolePool.Delete(pstTempRole);
		LOG_CHECK_ERROR(nRetCode);
		
		pCurNode = pNextNode;
	}

Exit1:
	return TRUE;
}

BOOL CBattleQueueMgr::LeaveBattle(UINT32 dwRoleID, UINT32 dwSceneTempID, UINT32 dwSceneID, BOOL bNormalLeave, UINT8 bySide)
{
	INT32 nRetCode = 0;

	if (bNormalLeave)
    {
        SCENE_INFO* pstSceneInfo = CSceneMgr::GetInstance().FindScene(dwSceneTempID, dwSceneID);
        PROCESS_ERROR(pstSceneInfo);

        pstSceneInfo->nSideCurRoleCount[bySide]--;
    }
    else
	{
		SBattleTempRole* pstTempRole = NULL;

		pstTempRole = m_BattleTempRolePool.New(dwRoleID);
		LOG_PROCESS_ERROR(pstTempRole);

		pstTempRole->pPrev = pstTempRole->pNext = NULL;
		pstTempRole->bySide = bySide;
		pstTempRole->dwRoleID = dwRoleID;
		pstTempRole->dwSceneTempID = dwSceneTempID;
		pstTempRole->dwSceneID = dwSceneID;
		pstTempRole->dwTimer = CManageServer::GetInstance().GetGameTime() + MAX_ABNORMAL_LEAVE_TIME;

		m_lstBattleTempRole.PushRear(pstTempRole);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBattleQueueMgr::CheckBattleTempRole(UINT32 dwRoleID, UINT32 dwSceneTempID, UINT32 dwSceneID, UINT8 bySide)
{
	INT32 nRetCode = 0;

	SBattleTempRole* pstTempRole = m_BattleTempRolePool.Find(dwRoleID);
	PROCESS_ERROR(pstTempRole);

	PROCESS_ERROR(dwSceneTempID == pstTempRole->dwSceneTempID);
	PROCESS_ERROR(dwSceneID = pstTempRole->dwSceneID);
	LOG_PROCESS_ERROR(bySide == pstTempRole->bySide);
	PROCESS_ERROR(CManageServer::GetInstance().GetGameTime() <= pstTempRole->dwTimer);

	nRetCode = m_lstBattleTempRole.Remove(pstTempRole);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_BattleTempRolePool.Delete(pstTempRole);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if(pstTempRole)
	{
		if(pstTempRole->pPrev != NULL || pstTempRole->pNext != NULL)
		{
			nRetCode = m_lstBattleTempRole.Remove(pstTempRole);
			LOG_PROCESS_ERROR(nRetCode);
		}

		nRetCode = m_BattleTempRolePool.Delete(pstTempRole);
		LOG_PROCESS_ERROR(nRetCode);
	}
	return FALSE;
}

BOOL CBattleQueueMgr::CTraverseSceneDataForBattle::operator()(const SCENE_DATA* pSceneData)
{
	if(pSceneData->nType == estBattleField)
		CBattleQueueMgr::GetInstance().AddBattleInfo(pSceneData->nSceneID, pSceneData->nBattleType,
		pSceneData->nSideCount, pSceneData->nMinRoleLimitPerSide, pSceneData->nMaxRoleLimitPerSide);
	
	return TRUE;
}
