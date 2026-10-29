#ifndef _ID_OBJECT_POOL_H_
#define _ID_OBJECT_POOL_H_

#include "process_error.h"
#include "linux_map.h"
#include <typeinfo>
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>
#include <psapi.h>

#ifdef WIN32
    #pragma warning(push)
    #pragma warning(disable : 4312)
#endif  // WIN32

#define FENCE_NUMBER    (0xABCDEF0123456789)
#define MAGIC_FREE_TAG  (0x87654321)
#define HASH_TABLE_SIZE	(1024 * 64)

template<class T, class N>
class CIDObjectPool
{
public:
    CIDObjectPool();
    ~CIDObjectPool();
    CIDObjectPool(const CIDObjectPool&) = delete;
    CIDObjectPool& operator=(const CIDObjectPool&) = delete;

    BOOL Init(INT32 nInitSize);
    BOOL UnInit(void);

    T* New(N dwID);
    BOOL Delete(T* pObject);
    T* Find(N dwID);

    template<class TFunc>
    inline BOOL Traverse(TFunc& rFunc);

    inline INT32 GetSize(void);
	inline INT32 GetFreeSize(void);
	inline INT32 GetUsedSize(void);
	inline INT32 GetObjSize(void);

    UINT32 GetObjID(T* pObject);

    void CheckFence(void);

private:
    INT32			m_nSize;
    std::array<LM_MAP_NODE<N>*, HASH_TABLE_SIZE> m_pDataMapRoot;
    LM_MAP_NODE<N>	m_FreeListHead;
    INT32			m_nFreeListSize;
	INT32			m_nObjSize;

    struct T_NODE
    {
        T           Data;
#ifdef _DEBUG_FENCE
        UINT64      qwFence;
#endif  // _DEBUG_FENCE
    };

    struct PayloadDeleter
    {
        std::size_t count = 0;
        void operator()(T_NODE* data) const noexcept
        {
            // The original ID pool destroyed payloads in ascending slot order.
            std::destroy_n(data, count);
            std::allocator<T_NODE>{}.deallocate(data, count);
        }
    };
    using PayloadOwner = std::unique_ptr<T_NODE[], PayloadDeleter>;

    static PayloadOwner _AllocatePayloads(std::size_t count)
    {
        std::allocator<T_NODE> allocator;
        auto* storage = allocator.allocate(count);
        try { std::uninitialized_default_construct_n(storage, count); }
        catch (...)
        {
            allocator.deallocate(storage, count);
            throw;
        }
        return PayloadOwner(storage, PayloadDeleter{count});
    }

    struct CHUNK_NODE
    {
        std::unique_ptr<LM_MAP_NODE<N>[]> pDataNode;
        PayloadOwner pTNode;
        INT32 nSize;
    };
    // Moving an owner never moves its indexed nodes or payload objects.
    std::vector<CHUNK_NODE> m_chunks;

    BOOL _CreateChunk(INT32 nSize);
    LM_MAP_NODE<N>* _GetDataNode(T* pData);
    T* _GetData(LM_MAP_NODE<N>* pDataNode);
};

template<class T, class N>
CIDObjectPool<T, N>::CIDObjectPool()
{
    m_nSize = 0;
    m_nFreeListSize = 0;
    m_pDataMapRoot.fill(nullptr);
    memset(&m_FreeListHead, 0, sizeof(m_FreeListHead));
	m_nObjSize = 0;
}

template<class T, class N>
CIDObjectPool<T, N>::~CIDObjectPool()
{
    UnInit();
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::_CreateChunk(INT32 nSize)
{
    if (nSize <= 0 || nSize > (std::numeric_limits<INT32>::max)() - m_nSize)
        return FALSE;
    // The intrusive tree stores color bits in the low two address bits.
    static_assert(sizeof(LM_MAP_NODE<N>) % 4 == 0);
    try
    {
        CHUNK_NODE chunk{
            std::make_unique_for_overwrite<LM_MAP_NODE<N>[]>(nSize),
            _AllocatePayloads(nSize), nSize};
        auto* nodes = chunk.pDataNode.get();
        // Constructed arrays roll back automatically if payload construction or
        // vector growth throws. Separate arrays honor T's alignment requirements.
#ifdef _DEBUG_FENCE
        for (INT32 index = 0; index < nSize; ++index)
            chunk.pTNode[index].qwFence = FENCE_NUMBER;
#endif
        m_chunks.push_back(std::move(chunk));
        for (INT32 index = 0; index < nSize; ++index)
        {
            nodes[index].pNext = index + 1 < nSize ? nodes + index + 1 : m_FreeListHead.pNext;
            nodes[index].pLeft = reinterpret_cast<LM_MAP_NODE<N>*>(MAGIC_FREE_TAG);
        }
        m_FreeListHead.pNext = nodes;
        m_nSize += nSize;
        m_nFreeListSize += nSize;
        return TRUE;
    }
    catch (...)
    {
        ERR("ID object pool chunk allocation or construction failed");
        return FALSE;
    }
}

template<class T, class N>
LM_MAP_NODE<N>* CIDObjectPool<T, N>::_GetDataNode(T* pData)
{
    if (!pData)
        return nullptr;
    const auto address = reinterpret_cast<std::uintptr_t>(pData);
    for (auto it = m_chunks.rbegin(); it != m_chunks.rend(); ++it)
    {
        const auto base = reinterpret_cast<std::uintptr_t>(&it->pTNode[0].Data);
        if (address >= base)
        {
            const auto offset = address - base;
            if (offset % sizeof(T_NODE) == 0 && offset / sizeof(T_NODE) < static_cast<std::size_t>(it->nSize))
                return it->pDataNode.get() + offset / sizeof(T_NODE);
        }
    }
    return nullptr;
}

template<class T, class N>
T* CIDObjectPool<T, N>::_GetData(LM_MAP_NODE<N>* pDataNode)
{
    if (!pDataNode)
        return nullptr;
    const auto address = reinterpret_cast<std::uintptr_t>(pDataNode);
    for (auto it = m_chunks.rbegin(); it != m_chunks.rend(); ++it)
    {
        const auto base = reinterpret_cast<std::uintptr_t>(it->pDataNode.get());
        if (address >= base)
        {
            const auto offset = address - base;
            if (offset % sizeof(LM_MAP_NODE<N>) == 0 && offset / sizeof(LM_MAP_NODE<N>) < static_cast<std::size_t>(it->nSize))
                return &it->pTNode[offset / sizeof(LM_MAP_NODE<N>)].Data;
        }
    }
    return nullptr;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::Init(INT32 nInitSize)
{
    if (!m_chunks.empty() || !_CreateChunk(nInitSize))
        return FALSE;
    m_nObjSize = sizeof(T);
    return TRUE;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::UnInit(void)
{
    LOG_CHECK_ERROR(m_nFreeListSize == m_nSize);
    CheckFence();
    // Match the former newest-chunk-first teardown order.
    while (!m_chunks.empty())
        m_chunks.pop_back();
    m_nSize = 0;
    m_nFreeListSize = 0;
    m_pDataMapRoot.fill(nullptr);
    m_FreeListHead.pNext = nullptr;
    return TRUE;
}

template<class T, class N>
T* CIDObjectPool<T, N>::New(N ID)
{
    INT32 nRetCode = 0;
    LM_MAP_NODE<N>* pDataNode = NULL;

    // take from free list
    if (m_FreeListHead.pNext == NULL)
    {
        nRetCode = _CreateChunk(m_nSize);
        LOG_PROCESS_ERROR(nRetCode);

        INF("expanded %s pool to %d in file %s", typeid(T).name(), m_nSize, __FILE__);
    }
    LOG_PROCESS_ERROR(m_FreeListHead.pNext);

    pDataNode = m_FreeListHead.pNext;
    LOG_PROCESS_ERROR(pDataNode->pLeft == (LM_MAP_NODE<N>*)MAGIC_FREE_TAG);
    m_FreeListHead.pNext = pDataNode->pNext;
    m_nFreeListSize--;

    // insert into map
    pDataNode->ID = ID;
    nRetCode = LM_Insert(pDataNode, m_pDataMapRoot.data() + (ID % HASH_TABLE_SIZE));
    LOG_PROCESS_ERROR(nRetCode);

    return _GetData(pDataNode);
Exit0:
    if (pDataNode)
    {
        // put back to free list
        pDataNode->pNext = m_FreeListHead.pNext;
        m_FreeListHead.pNext = pDataNode;
        m_nFreeListSize++;
    }

    return NULL;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::Delete(T* pObject)
{
    LM_MAP_NODE<N>* pDataNode = NULL;

    LOG_PROCESS_ERROR(pObject);

    pDataNode = _GetDataNode(pObject);
    LOG_PROCESS_ERROR(pDataNode);
	LOG_PROCESS_ERROR(pDataNode->pLeft != (LM_MAP_NODE<N>*)MAGIC_FREE_TAG);

    // remove from map
    LM_Erase(pDataNode, m_pDataMapRoot.data() + (pDataNode->ID % HASH_TABLE_SIZE));

    // put into free list
    pDataNode->pNext = m_FreeListHead.pNext;
    m_FreeListHead.pNext = pDataNode;
    pDataNode->pLeft = (LM_MAP_NODE<N>*)MAGIC_FREE_TAG;
    m_nFreeListSize++;

    return TRUE;
Exit0:

    return FALSE;
}

template<class T, class N>
T* CIDObjectPool<T, N>::Find(N ID)
{
    LM_MAP_NODE<N>* pDataNode = NULL;

    pDataNode = LM_Search(ID, m_pDataMapRoot[ID % HASH_TABLE_SIZE]);
    PROCESS_ERROR(pDataNode);

    return _GetData(pDataNode);
Exit0:

    return NULL;
}

template<class T, class N>
template<class TFunc>
inline BOOL CIDObjectPool<T, N>::Traverse(TFunc& rFunc)
{
    INT32 nRetCode = 0;

    for (INT32 nIndex = 0; nIndex < HASH_TABLE_SIZE; nIndex++)
    {
        LM_MAP_NODE<N>* pRootNode = m_pDataMapRoot[nIndex];
        if (pRootNode)
        {
            LM_MAP_NODE<N>* pDataNode = LM_First(pRootNode);
            while (pDataNode)
            {
                LM_MAP_NODE<N>* pNextNode = LM_Next(pDataNode);

                nRetCode = rFunc(pDataNode->ID, _GetData(pDataNode));
                PROCESS_ERROR(nRetCode);

                pDataNode = pNextNode;
            }
        }
    }

    return TRUE;
Exit0:

    return FALSE;
}

template<class T, class N>
inline INT32 CIDObjectPool<T, N>::GetSize(void)
{
    return m_nSize;
}

template<class T, class N>
inline INT32 CIDObjectPool<T, N>::GetFreeSize(void)
{
    return m_nFreeListSize;
}

template<class T, class N>
inline INT32 CIDObjectPool<T, N>::GetUsedSize(void)
{
    return m_nSize - m_nFreeListSize;
}

template<class T, class N>
inline INT32 CIDObjectPool<T, N>::GetObjSize(void)
{
	return m_nObjSize;
}

template<class T, class N>
UINT32 CIDObjectPool<T, N>::GetObjID(T* pObject)
{
    LM_MAP_NODE<N>* pDataNode = _GetDataNode(pObject);
    PROCESS_ERROR(pDataNode);

    return pDataNode->ID;
Exit0:
    return 0;
}

template<class T, class N>
void CIDObjectPool<T, N>::CheckFence(void)
{
#ifdef _DEBUG_FENCE
    for (const auto& chunk : m_chunks)
        for (INT32 index = 0; index < chunk.nSize; ++index)
            LOG_CHECK_ERROR_BREAK(chunk.pTNode[index].qwFence == FENCE_NUMBER);
#endif
    return;
}

#ifdef WIN32
    #pragma warning(pop)
#endif  // WIN32

#ifdef _DEBUG_FENCE
#undef _DEBUG_FENCE
#endif  // _DEBUG_FENCE

#endif	// _ID_OBJECT_POOL_H_
