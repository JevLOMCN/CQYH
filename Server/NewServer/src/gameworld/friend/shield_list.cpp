#include "stdafx.h"
#include "shield_list.h"

CShieldList::CShieldList()
{
    m_byShieldNum = 0;
}

CShieldList::~CShieldList()
{
    m_byShieldNum = 0;
}

BOOL CShieldList::Init()
{
    m_byShieldNum = 0;
    memset(m_arrRoleID, 0, sizeof(m_arrRoleID));
    return TRUE;
}

BOOL CShieldList::UnInit()
{
    m_byShieldNum = 0;
    memset(m_arrRoleID, 0, sizeof(m_arrRoleID));
    return TRUE;
}

BOOL CShieldList::Save(char* pcData, UINT32& rdwSize)
{
    UINT8   *pCountPoint = NULL;
    UINT32  *pRolePoint = NULL;

    LOG_PROCESS_ERROR(pcData);

    pCountPoint = reinterpret_cast<UINT8*>(pcData);
    *pCountPoint = m_byShieldNum;
    ++pCountPoint;

    pRolePoint = reinterpret_cast<UINT32*>(pCountPoint);
    for (INT32 i = 0; i < m_byShieldNum; ++i)
    {
        pRolePoint[i] = m_arrRoleID[i];
    }
    rdwSize = sizeof(UINT8) + m_byShieldNum * sizeof(UINT32);

    return TRUE;
Exit0:
    return FALSE;
}

void CShieldList::ShieldRole(UINT32 dwRoleID)
{
    LOG_PROCESS_ERROR(m_byShieldNum < MAX_SHIELD_NUM);
    LOG_PROCESS_ERROR(!IsAlreadyShielded(dwRoleID));

    m_arrRoleID[m_byShieldNum++] = dwRoleID;

    return ;
Exit0:
    return ;
}

void CShieldList::UnShieldRole(UINT32 dwRoleID)
{
    for (UINT32 i = 0; i < m_byShieldNum; ++i)
    {
        if (m_arrRoleID[i] == dwRoleID)
        {
            m_arrRoleID[i] = m_arrRoleID[m_byShieldNum - 1];
            m_arrRoleID[m_byShieldNum - 1] = 0;
            --m_byShieldNum;
            break;
        }
    }

    return ;
}

BOOL CShieldList::IsShieldedListFull()
{
    LOG_PROCESS_ERROR(m_byShieldNum < MAX_SHIELD_NUM);

    return FALSE;
Exit0:
    return TRUE;
}

BOOL CShieldList::IsAlreadyShielded(UINT32 dwRoleID)
{
    for (UINT32 i = 0; i < m_byShieldNum; ++i)
    {
        if (m_arrRoleID[i] == dwRoleID)
        {
            return TRUE;
        }
    }

    return FALSE;
}

UINT32 CShieldList::FindShieldedRole(INT32 nIndex)
{ 
    LOG_PROCESS_ERROR(nIndex < MAX_SHIELD_NUM);

    return m_arrRoleID[nIndex];
Exit0:
    return 0;
}



