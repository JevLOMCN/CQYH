#include "stdafx.h"
#include "application_mgr.h"
#include "manageserver.h"
#include <time.h>

#include "time_ex.h"

CApplicationMgr CApplicationMgr::m_Instance;

CApplicationMgr & CApplicationMgr::GetInstance()
{
    return m_Instance;
}

CApplicationMgr::CApplicationMgr()
{

}

CApplicationMgr::~CApplicationMgr()
{

}

BOOL CApplicationMgr::Init()
{
    for (INT32 i = 0; i < HASH_TABLE_BUCKET_NUM; ++i)
    {
        m_HashTable1[i].Clear();
        m_HashTable2[i].Clear();
    }
    m_TimeList.Clear();

    return TRUE;
}

BOOL CApplicationMgr::UnInit()
{
    for (INT32 i = 0; i < HASH_TABLE_BUCKET_NUM; ++i)
    {
        m_HashTable1[i].Clear();
        m_HashTable2[i].Clear();
    }
    m_TimeList.Clear();
    return FALSE;
}

BOOL CApplicationMgr::MainLoop()
{
    BASE_LIST_NODE  *pNode = NULL;
    APP_NODE        *pAppNode = NULL;
    INT32           nRetCode = 0;
    INT32           nCurrentTime = CManageServer::GetInstance().GetGameTime();
    INT32           nCount = 0;

    pNode = m_TimeList.Head().pNext;
    while (pNode && pNode != &m_TimeList.Rear())
    {
        ++nCount;
        pAppNode = node_cast(APP_NODE*, TimeNode, pNode);
        if (nCurrentTime < pAppNode->dwWashOutTime)
        {
            break;
        }
        if (nCount > 500)
        {
            WRN("CApplicationMgr MainLoop deal too many time washout data per loop");
            break;
        }

        pNode = pNode->pNext;

        nRetCode = RemoveApplication(pAppNode);
        if (nRetCode == FALSE)
        {
            LOG_CHECK_ERROR(nRetCode);
            continue;
        }

        LOG_CHECK_ERROR(pAppNode->fp);
        if (pAppNode->fp)
            pAppNode->fp(pAppNode);
    }

    return TRUE;
}

BOOL CApplicationMgr::InsertApplication(APP_NODE *pNode)
{
    INT32   nHashKey1;
    INT32   nHashKey2;
    BASE_LIST_NODE  *pPrev = NULL;

    LOG_PROCESS_ERROR(!Find(pNode->dwID_1, pNode->dwID_2));

    //_GetHashValue(pNode->dwID_1);
    nHashKey1 = _GetHashValue(pNode->dwID_1);
    nHashKey2 = _GetHashValue(pNode->dwID_2);

#ifdef _DEBUG
    if (nHashKey1 >= HASH_TABLE_BUCKET_NUM || nHashKey1 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }

    if (nHashKey2 >= HASH_TABLE_BUCKET_NUM || nHashKey2 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }
#else
    LOG_PROCESS_ERROR(nHashKey1 >= 0 && nHashKey1 < HASH_TABLE_BUCKET_NUM);
    LOG_PROCESS_ERROR(nHashKey2 >= 0 && nHashKey2 < HASH_TABLE_BUCKET_NUM);
#endif

    ClearNode(&pNode->HashTableNode1);
    ClearNode(&pNode->HashTableNode2);
    ClearNode(&pNode->TimeNode);

    m_HashTable1[nHashKey1].PushRear(&pNode->HashTableNode1);
    m_HashTable2[nHashKey2].PushRear(&pNode->HashTableNode2);

    // insert into time list
    pPrev = m_TimeList.Rear().pPrev;
    while (pPrev && pPrev != &m_TimeList.Head())
    {
        if (node_cast(APP_NODE*, TimeNode, pPrev)->dwWashOutTime > pNode->dwWashOutTime)
        {
            m_TimeList.InsertBefore(&pNode->TimeNode, pPrev);
            break;
        }
        pPrev = pPrev->pPrev;
    }
    if (pPrev == &m_TimeList.Head())
    {
        m_TimeList.InsertAfter(&pNode->TimeNode, pPrev);
    }
 
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CApplicationMgr::RemoveApplication(APP_NODE *pNode)
{
    INT32   nHashKey1;
    INT32   nHashKey2;

    //_GetHashValue(pNode->dwID_1);
    nHashKey1 = _GetHashValue(pNode->dwID_1);
    nHashKey2 = _GetHashValue(pNode->dwID_2);

#ifdef _DEBUG
    if (nHashKey1 >= HASH_TABLE_BUCKET_NUM || nHashKey1 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }

    if (nHashKey2 >= HASH_TABLE_BUCKET_NUM || nHashKey2 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }
#else
    LOG_PROCESS_ERROR(nHashKey1 >= 0 && nHashKey1 < HASH_TABLE_BUCKET_NUM);
    LOG_PROCESS_ERROR(nHashKey2 >= 0 && nHashKey2 < HASH_TABLE_BUCKET_NUM);
#endif

    LOG_CHECK_ERROR(m_HashTable1[nHashKey1].Remove(&pNode->HashTableNode1));
    LOG_CHECK_ERROR(m_HashTable2[nHashKey2].Remove(&pNode->HashTableNode2));
    LOG_PROCESS_ERROR(m_TimeList.Remove(&pNode->TimeNode));

    return TRUE;
Exit0:
    return FALSE;
}

APP_NODE *CApplicationMgr::Find(UINT32 dwID1, UINT32 dwID2)
{
    INT32 nHashKey1;
    APP_NODE  *pNode  = NULL;
    BASE_LIST_NODE  *pNext = NULL;

    nHashKey1 = _GetHashValue(dwID1);

#ifdef _DEBUG
    if (nHashKey1 >= HASH_TABLE_BUCKET_NUM || nHashKey1 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }
#else
    LOG_PROCESS_ERROR(nHashKey1 >= 0 && nHashKey1 < HASH_TABLE_BUCKET_NUM);
#endif

    pNext = m_HashTable1[nHashKey1].Head().pNext;
    while (pNext && pNext != &m_HashTable1[nHashKey1].Rear())
    {
        pNode = node_cast(APP_NODE*, HashTableNode1, pNext);
        if (pNode->dwID_1 == dwID1 && pNode->dwID_2 == dwID2)
        {
            PROCESS_SUCCESS(pNode);
        }
        pNext = pNext->pNext;
    }


    return NULL;
Exit1:
    return pNode;
Exit0:
    return NULL;
}

APP_NODE *CApplicationMgr::FirstAppNodeByID1(UINT32 dwID1)
{
    INT32 nHashKey1;
    APP_NODE  *pNode  = NULL;
    BASE_LIST_NODE  *pNext = NULL;

    nHashKey1 = _GetHashValue(dwID1);

#ifdef _DEBUG
    if (nHashKey1 >= HASH_TABLE_BUCKET_NUM || nHashKey1 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }
#else
    LOG_PROCESS_ERROR(nHashKey1 >= 0 && nHashKey1 < HASH_TABLE_BUCKET_NUM);
#endif

    pNext = m_HashTable1[nHashKey1].Head().pNext;
    while (pNext && pNext != &m_HashTable1[nHashKey1].Rear())
    {
        pNode = node_cast(APP_NODE*, HashTableNode1, pNext);
        if (pNode->dwID_1 == dwID1)
        {
            PROCESS_SUCCESS(pNode);
        }
        pNext = pNext->pNext;
    }

    return NULL;
Exit1:
    return pNode;
Exit0:
    return NULL;
}

APP_NODE *CApplicationMgr::NextAppNodeByID1(APP_NODE *pNode)
{
    INT32 nHashKey1;
    APP_NODE *pNextNode = NULL;
    BASE_LIST_NODE  *pNext = NULL;

    LOG_PROCESS_ERROR(pNode);

    nHashKey1 = _GetHashValue(pNode->dwID_1);
#ifdef _DEBUG
    if (nHashKey1 >= HASH_TABLE_BUCKET_NUM || nHashKey1 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }
#else
    LOG_PROCESS_ERROR(nHashKey1 >= 0 && nHashKey1 < HASH_TABLE_BUCKET_NUM);
#endif

    
    pNext = pNode->HashTableNode1.pNext;
    while (pNext && pNext != &m_HashTable1[nHashKey1].Rear())
    {
        pNextNode = node_cast(APP_NODE*, HashTableNode1, pNext);
        if (pNextNode->dwID_1 == pNode->dwID_1)
        {
            PROCESS_SUCCESS(pNextNode);
        }
        pNext = pNext->pNext;
    }

    return NULL;
Exit1:
    return pNextNode;
Exit0:
    return NULL;
}

APP_NODE *CApplicationMgr::FirstAppNodeByID2(UINT32 dwID2)
{
    INT32 nHashKey2;
    APP_NODE  *pNode  = NULL;
    BASE_LIST_NODE  *pNext = NULL;

    nHashKey2 = _GetHashValue(dwID2);

#ifdef _DEBUG
    if (nHashKey2 >= HASH_TABLE_BUCKET_NUM || nHashKey2 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }
#else
    LOG_PROCESS_ERROR(nHashKey2 >= 0 && nHashKey2 < HASH_TABLE_BUCKET_NUM);
#endif

    pNext = m_HashTable2[nHashKey2].Head().pNext;
    while (pNext && pNext != &m_HashTable2[nHashKey2].Rear())
    {
        pNode = node_cast(APP_NODE*, HashTableNode2, pNext);
        if (pNode->dwID_2 == dwID2)
        {
            PROCESS_SUCCESS(pNode);
        }
        pNext = pNext->pNext;
    }

    return NULL;
Exit1:
    return pNode;
Exit0:
    return NULL;
}

APP_NODE *CApplicationMgr::NextAppNodeByID2(APP_NODE *pNode)
{
    INT32 nHashKey2;
    APP_NODE *pNextNode = NULL;
    BASE_LIST_NODE  *pNext = NULL;

    LOG_PROCESS_ERROR(pNode);

    nHashKey2 = _GetHashValue(pNode->dwID_2);
#ifdef _DEBUG
    if (nHashKey2 >= HASH_TABLE_BUCKET_NUM || nHashKey2 < 0 )
    {
        ASSERT(FALSE && "hash key overflow!");
        LOG_PROCESS_ERROR(FALSE);
    }
#else
    LOG_PROCESS_ERROR(nHashKey2 >= 0 && nHashKey2 < HASH_TABLE_BUCKET_NUM);
#endif

    pNext = pNode->HashTableNode2.pNext;
    while (pNext && pNext != &m_HashTable2[nHashKey2].Rear())
    {
        pNextNode = node_cast(APP_NODE*, HashTableNode2, pNext);
        if (pNextNode->dwID_2 == pNode->dwID_2)
        {
            PROCESS_SUCCESS(pNextNode);
        }
        pNext = pNext->pNext;
    }

    return NULL;
Exit1:
    return pNextNode;
Exit0:
    return NULL;
}

INT32 CApplicationMgr::_GetHashValue(UINT32 dwID)
{
    // to be optimized Allan Tao[10.7.2013] 
    UINT16  v1;
    UINT16  v2;
    INT32   key;

    v1 = dwID;
    v2 = (dwID >> 16);

    key = v1 ^ v2;
    key &= 0x00000FFF;

    return key;

    //pNode->dwID_1 ^ (pNode->dwID_1 >> 16) & 0x00000FFF;

}
