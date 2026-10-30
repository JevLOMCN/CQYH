#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_

#include <exception>
#include <limits>
#include <memory>
#include <vector>
#include <typeinfo>
#include <psapi.h>

#ifdef WIN32
    #pragma warning(push)
    #pragma warning(disable : 4312)
#endif  

#define MAGIC_USED_TAG         (0x87654321)

template<class T>
class CObjectPool
{
public:
	CObjectPool();
	~CObjectPool();
    CObjectPool(const CObjectPool&) = delete;
    CObjectPool& operator=(const CObjectPool&) = delete;

	BOOL Init(INT32 nInitSize);
	BOOL UnInit(void);

	T* New(void);
	BOOL Delete(T* pObject);

	INT32 GetSize(void);
	INT32 GetFreeSize(void);
    INT32 GetUsedSize(void);
	INT32 GetObjSize(void);

private:
	INT32 m_nSize;
	INT32 m_nFreeSize;
	INT32 m_nObjSize;

	struct DATA_NODE
	{
		T			Data;
		DATA_NODE*	pNext;
	}	m_FreeListHead;

    // Each array stays at its original address as the owner vector grows.
    std::vector<std::unique_ptr<DATA_NODE[]>> m_chunks;

	BOOL _CreateChunk(INT32 nSize);
};

template<class T>
CObjectPool<T>::CObjectPool()
{
	m_nSize = 0;
	m_nFreeSize = 0;
	m_nObjSize = 0;
	m_FreeListHead.pNext = NULL;

}

template<class T>
CObjectPool<T>::~CObjectPool()
{
	UnInit();
}

template<class T>
BOOL CObjectPool<T>::_CreateChunk(INT32 nSize)
{
    if (nSize <= 0 || nSize > (std::numeric_limits<INT32>::max)() - m_nSize)
        return FALSE;
    try
    {
        auto chunk = std::make_unique_for_overwrite<DATA_NODE[]>(nSize);
        auto* nodes = chunk.get();
        m_chunks.push_back(std::move(chunk));
        for (INT32 index = 0; index < nSize; ++index)
            nodes[index].pNext = index + 1 < nSize ? nodes + index + 1 : m_FreeListHead.pNext;
        m_FreeListHead.pNext = nodes;
        m_nSize += nSize;
        m_nFreeSize += nSize;
        return TRUE;
    }
    catch (...)
    {
        ERR("Object pool chunk allocation or construction failed");
        return FALSE;
    }
}

template<class T>
BOOL CObjectPool<T>::Init(INT32 nInitSize)
{
    if (!m_chunks.empty() || !_CreateChunk(nInitSize))
        return FALSE;
    m_nObjSize = sizeof(DATA_NODE);
    return TRUE;
}

template<class T>
BOOL CObjectPool<T>::UnInit(void)
{
    LOG_CHECK_ERROR(m_nFreeSize == m_nSize);
    // Arrays retain reverse-slot destruction; visit newest chunks first.
    while (!m_chunks.empty())
        m_chunks.pop_back();
    m_FreeListHead.pNext = nullptr;
    m_nFreeSize = 0;
    m_nSize = 0;
    return TRUE;
}

template<class T>
T* CObjectPool<T>::New(void)
{
	DATA_NODE* pResult = NULL;
	INT32 nRetCode = 0;

	if (m_FreeListHead.pNext == NULL)
	{
		nRetCode = _CreateChunk(m_nSize);
		LOG_PROCESS_ERROR(nRetCode);

        INF("expanded %s pool to %d in file %s", typeid(T).name(), m_nSize, __FILE__);
    }

	pResult = m_FreeListHead.pNext;
    LOG_PROCESS_ERROR(pResult);

	m_FreeListHead.pNext = pResult->pNext;
    pResult->pNext = ((DATA_NODE*)MAGIC_USED_TAG);
	m_nFreeSize--;

	return &(pResult->Data);
Exit0:
	return NULL;
}

template<class T>
BOOL CObjectPool<T>::Delete(T* pObject)
{
    DATA_NODE* pDataNode = (DATA_NODE*)pObject;

    LOG_PROCESS_ERROR(pObject);
    LOG_PROCESS_ERROR(pDataNode->pNext == (DATA_NODE*)MAGIC_USED_TAG);

    pDataNode->pNext = m_FreeListHead.pNext;
	m_FreeListHead.pNext = (DATA_NODE*)pObject;
	m_nFreeSize++;

	return TRUE;
Exit0:
	return FALSE;
}

template<class T>
inline INT32 CObjectPool<T>::GetSize(void)
{
	return m_nSize;
}

template<class T>
inline INT32 CObjectPool<T>::GetFreeSize(void)
{
	return m_nFreeSize;
}

template<class T>
inline INT32 CObjectPool<T>::GetUsedSize(void)
{
    return m_nSize - m_nFreeSize;
}

template<class T>
inline INT32 CObjectPool<T>::GetObjSize(void)
{
	return m_nObjSize;
}

#ifdef WIN32
    #pragma warning(pop)
#endif  // WIN32

#endif	// _OBJECT_POOL_H_
