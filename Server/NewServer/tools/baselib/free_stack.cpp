#include "stdafx.h"
#include "free_stack.h"

CFreeStack::~CFreeStack()
{
    UnInit();
}

BOOL CFreeStack::Init()
{
    bool empty;
    {
        std::scoped_lock lock(m_mutex);
        empty = m_head == nullptr;
    }
    // Log after unlocking: custom log sinks may themselves use a pool.
    LOG_CHECK_ERROR(empty);
    return empty;
}

BOOL CFreeStack::UnInit()
{
    bool empty;
    {
        std::scoped_lock lock(m_mutex);
        empty = m_head == nullptr;
        m_head = nullptr;
    }
    LOG_CHECK_ERROR(empty);
    return TRUE;
}
