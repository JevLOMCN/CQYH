#include "stdafx.h"
#include "tl_user.h"

CTLUser::CTLUser() : 
m_dwUserID(0),
m_dwSessionID(0),
m_nState(TL_STATE_IDLE),
m_dwIP(0xFFFFFFFF),
m_wPort(0),
m_ePermission(GM_PERMISSION_GUEST),
m_bLD(FALSE),
m_pCmd(NULL)
{

}

CTLUser::~CTLUser()
{

}

BOOL CTLUser::Init()
{
    m_dwUserID = 0;
    m_dwSessionID = 0;
    m_nState = TL_STATE_IDLE;
    m_dwIP = 0;
    m_wPort = 0;
    memset(m_szComputerName, 0, sizeof(m_szComputerName));
    m_ePermission = GM_PERMISSION_GUEST;
    m_pCmd = NULL;
    m_bLD = FALSE;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CTLUser::UnInit()
{    
    m_dwUserID = 0;
    m_dwSessionID = 0;
    m_nState = TL_STATE_IDLE;
    m_dwIP = 0;
    m_wPort = 0;
    memset(m_szComputerName, 0, sizeof(m_szComputerName));
    m_ePermission = GM_PERMISSION_GUEST;
    m_pCmd = NULL;
    m_bLD = FALSE;

    return TRUE;
Exit0:
    return FALSE;
}
