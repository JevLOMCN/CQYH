#include "stdafx.h"
#include "circle_list.h"
#include "process_error.h"

CCircleList::CCircleList()
{
    m_dwSize = m_dwHead = m_dwRear = 0;
    m_ppData = NULL;
}

CCircleList::~CCircleList()
{
    UnInit();
}

BOOL CCircleList::Init(UINT32 dwSize)
{
    if (!dwSize || m_ppData || (size_t)dwSize > (size_t)-1 / sizeof(void*))
        return FALSE;

    m_ppData = new void*[dwSize]();
    LOG_PROCESS_ERROR(m_ppData);

    m_dwSize = dwSize;
    m_dwHead = m_dwRear = 0;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCircleList::UnInit(void)
{
    SAFE_DELETE_ARRAY(m_ppData);
    m_dwSize = m_dwHead = m_dwRear = 0;

    return TRUE;
}

BOOL CCircleList::Push(void* pData)
{
    PROCESS_ERROR(m_dwSize > 0 );
    PROCESS_ERROR((m_dwRear + 1) % m_dwSize != m_dwHead);

    PROCESS_ERROR(m_ppData);
    m_ppData[m_dwRear] = pData;
    m_dwRear = (m_dwRear + 1) % m_dwSize;
    
    return TRUE;
Exit0:
    return FALSE;
}

void* CCircleList::Pop(void)
{
    void* pResult = NULL;

    PROCESS_ERROR(m_dwSize > 0 && m_ppData && m_dwRear != m_dwHead);

    pResult = m_ppData[m_dwHead];
    m_dwHead = (m_dwHead + 1) % m_dwSize;

Exit0:
    return pResult;
}

void* CCircleList::Check(void)
{
    PROCESS_ERROR(m_dwSize > 0 && m_ppData && m_dwRear != m_dwHead);

    return m_ppData[m_dwHead];
Exit0:
    return NULL;
}

BOOL CCircleList::IsEmpty(void)
{
    return m_dwHead == m_dwRear;
}

