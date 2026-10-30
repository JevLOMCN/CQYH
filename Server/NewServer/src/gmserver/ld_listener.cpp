#include "stdafx.h"
#include "ld_listener.h"

CLDListener CLDListener::m_Instance;

CLDListener & CLDListener::GetInstance()
{
    return m_Instance;
}

CLDListener::CLDListener()
{

}

CLDListener::~CLDListener()
{

}

BOOL CLDListener::Init()
{
    return TRUE;
}

BOOL CLDListener::UnInit()
{
    return TRUE;
}

BOOL CLDListener::_IsExistLD(UINT32 dwUserID)
{
    for (INT32 i = 0; i < m_nLDNum; ++i)
    {
        if (m_LDRegList[i].dwUserID == dwUserID)
            return TRUE;
    }

Exit0:
    return FALSE;
}

void CLDListener::ChangeGroupID(UINT32 dwUserID, INT32 nGroupID)
{
    INT32       nRetCode = 0;

    nRetCode = _IsExistLD(dwUserID);
    if (nRetCode == TRUE)
    {
        for (INT32 i = 0; i < m_nLDNum; ++i)
        {
            if (m_LDRegList[i].dwUserID == dwUserID)
            {
                m_LDRegList[i].nGroupID = nGroupID;
                return;
            }
        }
    }
    else
    {
        RegisterGroupID(dwUserID, nGroupID);
    }
    
    return;
}

void CLDListener::RegisterGroupID(UINT32 dwUserID, INT32 nGroupID)
{
    INT32 nRetCode = 0;

    nRetCode = _IsExistLD(dwUserID);
    PROCESS_ERROR(nRetCode == FALSE);

    m_LDRegList[m_nLDNum].dwUserID = dwUserID;
    m_LDRegList[m_nLDNum].nGroupID = nGroupID;
    ++m_nLDNum;

Exit0:
    return ;
}

void CLDListener::UnRegisterGroupID(UINT32 dwUserID)
{
    INT32       nRetCode = 0;

    nRetCode = _IsExistLD(dwUserID);
    PROCESS_ERROR(nRetCode == TRUE);

    for (INT32 i = 0; i < m_nLDNum; ++i)
    {
        if (m_LDRegList[i].dwUserID == dwUserID)
        {
            m_LDRegList[i] = m_LDRegList[m_nLDNum - 1];
            --m_nLDNum;
            break;
        }
    }

Exit0:
    return;
}
