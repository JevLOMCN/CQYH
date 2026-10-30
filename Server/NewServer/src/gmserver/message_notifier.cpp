#include "stdafx.h"
#include "message_notifier.h"
//#include "gm_message_handler.h"

BOOL CMessageNotifier::SendToUser(UINT32 dwClientSessionID, PACKET_BUFFER* pData, UINT32 dwSize)
{
    INT32 nRetCode = 0;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageNotifier::SendToUsers(UINT32 dwGTSessionID, UINT32 dwClientSessionIDList[], INT32 nSessionCount, PACKET_BUFFER* pData, UINT32 dwSize)
{
    INT32 nRetCode = 0;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageNotifier::SendToMS(PACKET_BUFFER* pData, UINT32 dwSize)
{
    INT32 nRetCode = 0;

    //nRetCode = CGSMessageHandler::GetInstance().SendMS(pData, dwSize);
    //LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageNotifier::SendToDL(PACKET_BUFFER* pData, UINT32 dwSize)
{
    return TRUE;
}

BOOL CMessageNotifier::ReturnSelectRole(UINT32 dwClientSessionID)
{
    return FALSE;
}

BOOL CMessageNotifier::SaveRoleData(CRole* poRole, BOOL bSaveFile)
{
    return FALSE;
}

BOOL CMessageNotifier::UnlockRole(CRole* poRole)
{
    return FALSE;
}

BOOL CMessageNotifier::_KickUser(UINT32 dwClientSessionID)
{
    return FALSE;
}

BOOL CMessageNotifier::Logout(UINT32 dwClientSessionID)
{
    return FALSE;
}
