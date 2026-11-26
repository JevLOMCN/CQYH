#ifndef _FREE_OBJECT_POOL_H_
#define _FREE_OBJECT_POOL_H_

#include "free_stack.h"
#include "process_error.h"

template<class T>
class CFreeObjectPool
{
public:
    CFreeObjectPool();
    ~CFreeObjectPool();

    BOOL Init(INT32 nInitSize);
    BOOL UnInit(void);

    T* New(void);
    BOOL Delete(T* pObject);

    INT32 GetSize(void);

private:
    int	            m_nSize;

    struct DATA_NODE
    {
        STACK_NODE  StackNode;
        T			Data;
    };
    DATA_NODE*      m_pDataPool;
    CFreeStack      m_FreeStack;
};


template<class T>
CFreeObjectPool<T>::CFreeObjectPool()
{
    m_nSize = 0;
    m_pDataPool = NULL;
}

template<class T>
CFreeObjectPool<T>::~CFreeObjectPool()
{
    UnInit();
}

template<class T>
BOOL CFreeObjectPool<T>::Init(INT32 nInitSize)
{
    INT32 nRetCode = 0;
    if (nInitSize < 0 || m_pDataPool || (size_t)nInitSize > (size_t)-1 / sizeof(DATA_NODE))
        return FALSE;
    if (nInitSize == 0)
        return TRUE;

    m_pDataPool = new DATA_NODE[nInitSize]();
    LOG_PROCESS_ERROR(m_pDataPool);

    nRetCode = m_FreeStack.Init();
    LOG_PROCESS_ERROR(nRetCode);

    for (INT32 nIndex = 0; nIndex < nInitSize; nIndex++)
    {
        m_pDataPool[nIndex].StackNode.pData = &(m_pDataPool[nIndex].Data);
        m_FreeStack.Push((STACK_NODE*)(m_pDataPool + nIndex));
    }

    m_nSize = nInitSize;

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

template<class T>
BOOL CFreeObjectPool<T>::UnInit(void)
{
    INT32 nFreeSize = 0;

    while (m_FreeStack.Pop())
        nFreeSize++;
    LOG_CHECK_ERROR(m_nSize == nFreeSize);

    m_FreeStack.UnInit();
    SAFE_DELETE_ARRAY(m_pDataPool);
    m_nSize = 0;

    return TRUE;
}

template<class T>
T* CFreeObjectPool<T>::New(void)
{
    DATA_NODE* pDataNode = (DATA_NODE*)m_FreeStack.Pop();
    PROCESS_ERROR(pDataNode);

    return &(pDataNode->Data);
Exit0:
   return NULL;
}

template<class T>
BOOL CFreeObjectPool<T>::Delete(T* pObject)
{
    if (!pObject || !m_pDataPool || m_nSize <= 0)
        return FALSE;
    const size_t dataOffset = (BYTE*)&m_pDataPool[0].Data - (BYTE*)&m_pDataPool[0];
    const size_t relative = (size_t)pObject - (size_t)m_pDataPool;
    if (relative < dataOffset)
        return FALSE;
    const size_t nodeOffset = relative - dataOffset;
    if (nodeOffset % sizeof(DATA_NODE) != 0 || nodeOffset / sizeof(DATA_NODE) >= (size_t)m_nSize)
        return FALSE;
    DATA_NODE* node = &m_pDataPool[nodeOffset / sizeof(DATA_NODE)];
    m_FreeStack.Push(&node->StackNode);
    return TRUE;
}

template<class T>
INT32 CFreeObjectPool<T>::GetSize(void)
{
    return m_nSize;
}

#endif  // _FREE_OBJECT_POOL_H_