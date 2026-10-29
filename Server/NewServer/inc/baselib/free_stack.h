#ifndef _INTERLOCK_SLIST_H_
#define _INTERLOCK_SLIST_H_

#include <mutex>

// Keep this layout: nodes also occur inside packet pools and mapped role data.
struct STACK_NODE
{
    STACK_NODE* pNext;
    void*       pData;
};

// Process-local synchronization over borrowed, stable nodes. Push/Pop allocate
// nothing and accept full-width pointers. Init/UnInit require stopped users.
class CFreeStack
{
public:
    CFreeStack() = default;
    ~CFreeStack();
    CFreeStack(const CFreeStack&) = delete;
    CFreeStack& operator=(const CFreeStack&) = delete;

    BOOL Init();
    BOOL UnInit();
    void Push(STACK_NODE* node);
    STACK_NODE* Pop();

private:
    std::mutex m_mutex;
    STACK_NODE* m_head = nullptr;
};

inline void CFreeStack::Push(STACK_NODE* node)
{
    if (!node)
    {
        LOG_CHECK_ERROR(node);
        return;
    }
    std::scoped_lock lock(m_mutex);
    node->pNext = m_head;
    m_head = node;
}

inline STACK_NODE* CFreeStack::Pop()
{
    std::scoped_lock lock(m_mutex);
    auto* node = m_head;
    if (node)
        m_head = node->pNext;
    return node;
}

#endif // _INTERLOCK_SLIST_H_
