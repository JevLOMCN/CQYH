#ifndef _FREE_OBJECT_POOL_H_
#define _FREE_OBJECT_POOL_H_

#include <stddef.h>
#include <memory>
#include "free_stack.h"

#define HEAD_FENCE (0xABCDEF0123456789)
#define REAR_FENCE (0x9876543210FEDCBA)

template<class T>
class CFreeObjectPool
{
public:
    CFreeObjectPool();
    ~CFreeObjectPool();
    CFreeObjectPool(const CFreeObjectPool&) = delete;
    CFreeObjectPool& operator=(const CFreeObjectPool&) = delete;

    BOOL Init(INT32 nInitSize);
    BOOL UnInit(void);

    inline T* New(void);
	inline BOOL Delete(T* pObject);

    INT32 GetSize(void);
	void* GetDataPool(void);
	intptr_t GetDataOffset(void);
	UINT32 GetDataSize(void);

    BOOL CheckFence(void);

private:
    int	            m_nSize;

    struct DATA_NODE
    {
#ifdef _DEBUG_FENCE
        UINT64      qwHeadFence;
#endif  // _DEBUG_FENCE
        STACK_NODE  StackNode;
        T			Data;
#ifdef _DEBUG_FENCE
        UINT64      qwRearFence;
#endif  // _DEBUG_FENCE
    };
    std::unique_ptr<DATA_NODE[]> m_pDataPool;
    CFreeStack      m_FreeStack;
};


template<class T>
CFreeObjectPool<T>::CFreeObjectPool()
{
    m_nSize = 0;
    m_pDataPool.reset();
}

template<class T>
CFreeObjectPool<T>::~CFreeObjectPool()
{
    UnInit();
}

template<class T>
BOOL CFreeObjectPool<T>::Init(INT32 nInitSize)
{
    if (nInitSize < 0 || m_pDataPool)
        return FALSE;
    try
    {
        auto records = std::make_unique_for_overwrite<DATA_NODE[]>(nInitSize);
        if (!m_FreeStack.Init())
            return FALSE;
        m_pDataPool = std::move(records);
        m_nSize = nInitSize;
        for (INT32 index = 0; index < nInitSize; ++index)
        {
#ifdef _DEBUG_FENCE
            m_pDataPool[index].qwHeadFence = HEAD_FENCE;
            m_pDataPool[index].qwRearFence = REAR_FENCE;
#endif
            m_pDataPool[index].StackNode.pData = &m_pDataPool[index].Data;
            m_FreeStack.Push(&m_pDataPool[index].StackNode);
        }
        return TRUE;
    }
    catch (...)
    {
        ERR("Fixed object pool allocation or construction failed");
        UnInit();
        return FALSE;
    }
}

template<class T>
BOOL CFreeObjectPool<T>::UnInit(void)
{
    INT32 nFreeSize = 0;

    CheckFence();

    while (m_FreeStack.Pop())
        nFreeSize++;
    LOG_CHECK_ERROR(m_nSize == nFreeSize);

    m_FreeStack.UnInit();
    m_pDataPool.reset();
    m_nSize = 0;

    return TRUE;
}

template<class T>
inline T* CFreeObjectPool<T>::New(void)
{
    DATA_NODE* pDataNode;

    STACK_NODE* pStackNode = m_FreeStack.Pop();
    PROCESS_ERROR(pStackNode);
    
    pDataNode = (DATA_NODE*)((char*)pStackNode - (char*)(&((DATA_NODE*)0)->StackNode));

#ifdef _DEBUG_FENCE
    LOG_CHECK_ERROR_BREAK(pDataNode->qwHeadFence == HEAD_FENCE);
    LOG_CHECK_ERROR_BREAK(pDataNode->qwRearFence == REAR_FENCE);
#endif  // _DEBUG_FENCE

    return &(pDataNode->Data);
Exit0:
   return NULL;
}

template<class T>
inline BOOL CFreeObjectPool<T>::Delete(T* pObject)
{
    DATA_NODE* pDataNode = NULL;
    LOG_PROCESS_ERROR(pObject);
    pDataNode = (DATA_NODE*)((char*)pObject - (char*)(&((DATA_NODE*)0)->Data));

#ifdef _DEBUG_FENCE
    {
        LOG_CHECK_ERROR_BREAK(pDataNode->qwHeadFence == HEAD_FENCE);
        LOG_CHECK_ERROR_BREAK(pDataNode->qwRearFence == REAR_FENCE);
    }
#endif  // _DEBUG_FENCE

    m_FreeStack.Push(&pDataNode->StackNode);

    return TRUE;
Exit0:
    return FALSE;
}

template<class T>
INT32 CFreeObjectPool<T>::GetSize(void)
{
    return m_nSize;
}

template<class T>
void* CFreeObjectPool<T>::GetDataPool(void)
{
	return m_pDataPool.get();
}

template<class T>
intptr_t CFreeObjectPool<T>::GetDataOffset(void)
{
	return offsetof(DATA_NODE, Data); //&(((DATA_NODE*)0)->Data);
}

template<class T>
UINT32 CFreeObjectPool<T>::GetDataSize(void)
{
	return sizeof(DATA_NODE);
}

template<class T>
BOOL CFreeObjectPool<T>::CheckFence(void)
{
#ifdef _DEBUG_FENCE
    for (INT32 nIndex = 0; nIndex < m_nSize; nIndex++)
    {
        LOG_CHECK_ERROR(m_pDataPool[nIndex].qwHeadFence == HEAD_FENCE);
        LOG_CHECK_ERROR(m_pDataPool[nIndex].qwRearFence == REAR_FENCE);
    }
#endif  // _DEBUG_FENCE

    return TRUE;
}

#ifdef _DEBUG_FENCE
#undef _DEBUG_FENCE
#endif  // _DEBUG_FENCE

#endif  // _FREE_OBJECT_POOL_H_
