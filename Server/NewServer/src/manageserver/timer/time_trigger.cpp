#include "stdafx.h"
#include "time_trigger.h"
#include "manageserver.h"

CTimeTrigger CTimeTrigger::m_Instance;

CTimeTrigger &CTimeTrigger::GetInstance()
{
    return m_Instance;
}

CTimeTrigger::CTimeTrigger()
{

}

CTimeTrigger::~CTimeTrigger()
{
	UnInit();
}

BOOL CTimeTrigger::Init()
{
	for (INT32 nIndex = 0; nIndex < tttTotal; nIndex++)
	    m_TimerList[nIndex].Clear();

	return TRUE;
}

BOOL CTimeTrigger::UnInit()
{
	for (INT32 nIndex = 0; nIndex < tttTotal; nIndex++)
	{
		LOG_CHECK_ERROR(m_TimerList[nIndex].IsEmpty());
	}

	return TRUE;
}

BOOL CTimeTrigger::MainLoop()
{
	INT32				nRetCode = 0;
	BASE_LIST_NODE      *pNode = NULL;
    BASE_TRIGGER_NODE   *pTriggerNode = NULL;
    UINT32				dwCurrentTime = CManageServer::GetInstance().GetGameTime();

	for (INT32 nIndex = tttInvalid; nIndex < tttTotal; nIndex++)
	{
		pNode = m_TimerList[nIndex].Head().pNext;
		while (pNode && pNode != &m_TimerList[nIndex].Rear())
		{
			pTriggerNode = (BASE_TRIGGER_NODE*)pNode;
			if (dwCurrentTime < pTriggerNode->dwWashoutTime)
				break;
			pNode = pTriggerNode->pNext;

			LOG_CHECK_ERROR(pTriggerNode->nTriggerType == nIndex);
			nRetCode = DeleteTrigger(pTriggerNode);
			LOG_CHECK_ERROR(nRetCode);

			LOG_CHECK_ERROR(pTriggerNode->fp);
			if (pTriggerNode->fp)
			{
				nRetCode = pTriggerNode->fp(pTriggerNode);
				if (TIME_TRIGGER_RET_CONTINUE == nRetCode)
				{
					nRetCode = InsertTrigger(pTriggerNode);
					LOG_CHECK_ERROR(nRetCode);
				}
			}
		}
	}

    return TRUE;
}

BOOL CTimeTrigger::InsertTrigger(BASE_TRIGGER_NODE *pTriggerNode)
{
    INT32 nRetCode = 0;
    BASE_TRIGGER_NODE* pInsertPos = NULL;

    LOG_PROCESS_ERROR(pTriggerNode);
    LOG_PROCESS_ERROR(pTriggerNode->fp);
	LOG_PROCESS_ERROR(pTriggerNode->nTriggerType >= tttInvalid && pTriggerNode->nTriggerType < tttTotal);

    ClearNode(pTriggerNode);

    pInsertPos = (BASE_TRIGGER_NODE*)m_TimerList[pTriggerNode->nTriggerType].Rear().pPrev;
	while (pInsertPos != &m_TimerList[pTriggerNode->nTriggerType].Head() &&
        pTriggerNode->dwWashoutTime < pInsertPos->dwWashoutTime)
    {
        pInsertPos = (BASE_TRIGGER_NODE*)pInsertPos->pPrev;
    }

    nRetCode = CBaseList::InsertAfter(pTriggerNode, pInsertPos);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CTimeTrigger::DeleteTrigger(BASE_TRIGGER_NODE *pTriggerNode)
{
	return CBaseList::Remove(pTriggerNode);
}
