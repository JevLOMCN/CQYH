#include "stdafx.h"
#include "circle_list.h"

BOOL CCircleList::Init(UINT32 size)
{
    if (size == 0 || !m_data.empty())
    {
        LOG_CHECK_ERROR(size > 0 && m_data.empty());
        return FALSE;
    }
    m_data.resize(size);
    m_head.store(0, std::memory_order_relaxed);
    m_rear.store(0, std::memory_order_relaxed);
    return TRUE;
}

BOOL CCircleList::UnInit()
{
    std::vector<void*>{}.swap(m_data);
    m_head.store(0, std::memory_order_relaxed);
    m_rear.store(0, std::memory_order_relaxed);
    return TRUE;
}
