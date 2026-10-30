#include "stdafx.h"
#include "timermgr.h"

#include "manageserver.h"

CTimerMgr CTimerMgr::ms_Instance;

CTimerMgr::CTimerMgr()
{
	for(INT32 i = etlSecond; i < etlTotal; ++i)
		m_lstTimer[i].Clear();

	m_nTimerIDGenerator = 0;
	m_dwCurTime = 0;
}

CTimerMgr::~CTimerMgr()
{
	UnInit();
}

BOOL CTimerMgr::Init(UINT32 dwCurTime)
{
	INT32 nRetCode = 0;
	m_nTimerIDGenerator = 1;

	m_dwCurTime = dwCurTime;

	for(INT32 i = etlSecond; i < etlTotal; ++i)
	{
		m_lstTimer[i].Clear();
		m_lstNextTrigTime[i] = m_dwCurTime + lstTimeStandLevel[i];
	}
	nRetCode = m_TimerPool.Init(32);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimerMgr::UnInit(VOID)
{
	INT32 nRetCode = 0;
	BASE_LIST_NODE* pNode = NULL;
	BASE_LIST_NODE* pNextNode = NULL;

	for(INT32 i = etlSecond; i < etlTotal; ++i)
	{
		pNode = m_lstTimer[i].Head().pNext;
		while (pNode != &(m_lstTimer[i].Rear()))
		{
			STimer* pstTimer = (STimer*)pNode;
			pNextNode = pNode->pNext;

			nRetCode = StopTimer(pstTimer);
			LOG_CHECK_ERROR(nRetCode);

			nRetCode = DestroyTimer(pstTimer);
			LOG_CHECK_ERROR(nRetCode);

			pNode = pNextNode;
		}
		LOG_CHECK_ERROR(m_lstTimer[i].IsEmpty());
	}

	nRetCode = m_TimerPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

CTimerMgr& CTimerMgr::GetInstance(VOID)
{
	return ms_Instance;
}

STimer* CTimerMgr::CreateTimer()
{
	STimer* pResult = NULL;
	UINT32 dwTimerID = m_nTimerIDGenerator++;

	pResult = m_TimerPool.New(dwTimerID);
	LOG_PROCESS_ERROR(pResult);
	memset(pResult, 0, sizeof(STimer));

	for(INT32 i = etlSecond; i < etlTotal; ++i)
		pResult->pPrev = pResult->pNext = NULL;

	pResult->dwTimerID = dwTimerID;
	pResult->m_byTimerLevel = etlInvalid;

	return pResult;
Exit0:
	return NULL;
}

BOOL CTimerMgr::DestroyTimer(STimer* pstTimer)
{
	INT32 nRetcode = 0;
	LOG_PROCESS_ERROR(pstTimer);

	LOG_PROCESS_ERROR(pstTimer->m_byTimerLevel == etlInvalid);
	LOG_PROCESS_ERROR(pstTimer->pPrev == NULL && pstTimer->pNext == NULL);
	
	nRetcode = m_TimerPool.Delete(pstTimer);		
	LOG_CHECK_ERROR(nRetcode);

	return TRUE;
Exit0:
	return FALSE;
}

STimer* CTimerMgr::FindTimer(UINT32 dwTimerID)
{
	return m_TimerPool.Find(dwTimerID);
}

BOOL CTimerMgr::StartTimerAtTime(STimer* pstTimer, UINT32 dwStartTime)
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(dwStartTime >= m_dwCurTime);
	
	nRetCode = StartTimer(pstTimer, dwStartTime - m_dwCurTime);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimerMgr::StartTimer(STimer* pstTimer, UINT32 nDelay)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pstTimer);
	LOG_PROCESS_ERROR(pstTimer->m_nInterval > 0);
	LOG_PROCESS_ERROR(pstTimer->m_nRemainTrigCount > 0);

	pstTimer->m_dwNextTrigTime = m_dwCurTime + pstTimer->m_nInterval + nDelay;
	
	nRetCode = _UpdateTimer(pstTimer);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimerMgr::StopTimer(STimer* pstTimer)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pstTimer);
	PROCESS_SUCCESS(pstTimer->m_byTimerLevel == etlInvalid);
	PROCESS_SUCCESS(pstTimer->pPrev == NULL && pstTimer->pNext == NULL);
	
	nRetCode = m_lstTimer[pstTimer->m_byTimerLevel].Remove(pstTimer);
	LOG_PROCESS_ERROR(nRetCode);

	pstTimer->m_byTimerLevel = etlInvalid;

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimerMgr::_UpdateTimer(STimer* pstTimer)
{
	INT32 nRetCode = 0;
	BOOL bUpdate = FALSE;

	LOG_PROCESS_ERROR(pstTimer);

	if(pstTimer->m_nRemainTrigCount <= 0)
	{
		nRetCode = StopTimer(pstTimer);
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = DestroyTimer(pstTimer);
		LOG_CHECK_ERROR(nRetCode);

		PROCESS_SUCCESS(TRUE);
	}

	for(INT32 i = etlDay; i >= etlMinute; --i)
	{
		if(pstTimer->m_dwNextTrigTime > m_dwCurTime + lstTimeStandLevel[i])
		{
			PROCESS_SUCCESS(pstTimer->m_byTimerLevel == i);

			if(pstTimer->m_byTimerLevel != etlInvalid)
			{
				nRetCode = m_lstTimer[pstTimer->m_byTimerLevel].Remove(pstTimer);
				LOG_PROCESS_ERROR(nRetCode);
				pstTimer->m_byTimerLevel = etlInvalid;
			}

			nRetCode = m_lstTimer[i].PushRear(pstTimer);
			LOG_PROCESS_ERROR(nRetCode);

			pstTimer->m_byTimerLevel = i;

			bUpdate = TRUE;
			break;
		}
	}

	if(!bUpdate)
	{
		PROCESS_SUCCESS(pstTimer->m_byTimerLevel == etlSecond);
		
		if(pstTimer->m_byTimerLevel != etlInvalid)
		{
			nRetCode = m_lstTimer[pstTimer->m_byTimerLevel].Remove(pstTimer);
			LOG_PROCESS_ERROR(nRetCode);
			pstTimer->m_byTimerLevel = etlInvalid;
		}

		nRetCode = m_lstTimer[etlSecond].PushRear(pstTimer);
		LOG_PROCESS_ERROR(nRetCode);

		pstTimer->m_byTimerLevel = etlSecond;
	}

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}


BOOL CTimerMgr::_TrigTimerLevel(INT32 eTimeLevel)
{
	INT32 nRetCode = 0;
	BASE_LIST_NODE* pNode = NULL;
	BASE_LIST_NODE* pNextNode = NULL;
	LOG_PROCESS_ERROR(eTimeLevel >= etlSecond);
	LOG_PROCESS_ERROR(eTimeLevel <= etlDay);

	pNode = m_lstTimer[eTimeLevel].Head().pNext;
	while (pNode != &(m_lstTimer[eTimeLevel].Rear()))
	{
		STimer* pstTimer = (STimer*)pNode;
		pNextNode = pNode->pNext;
		
		while(pstTimer->m_dwNextTrigTime <= m_dwCurTime)
		{
			nRetCode = 0;
			if(pstTimer->m_szFunc[0] > 0)
			{
				//do script func
				nRetCode = 1;
			}
			
			if(!nRetCode)
			{
				nRetCode = pstTimer->m_pFunc(pstTimer->m_nParam[0], pstTimer->m_nParam[1], pstTimer->m_nParam[2], pstTimer->m_szParam);
				LOG_CHECK_ERROR(nRetCode);
			}
			pstTimer->m_dwNextTrigTime += pstTimer->m_nInterval;
			pstTimer->m_nRemainTrigCount -= 1;
		}

		nRetCode = _UpdateTimer(pstTimer);
		LOG_CHECK_ERROR(nRetCode);
		
		pNode = pNextNode;
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimerMgr::MainLoop()
{
	INT32 nRetCode = 0;
    m_dwCurTime = CManageServer::GetInstance().GetGameTime();
	
	PROCESS_SUCCESS(m_dwCurTime < m_lstNextTrigTime[etlSecond]);

	for(INT32 i = etlDay; i >= etlSecond; --i )
	{
		if(m_lstNextTrigTime[i] <= m_dwCurTime)
		{
			nRetCode = _TrigTimerLevel(i);
			LOG_CHECK_ERROR(nRetCode);

			m_lstNextTrigTime[i] = m_dwCurTime + lstTimeStandLevel[i];
		}
	}

Exit1:
	return TRUE;
}
