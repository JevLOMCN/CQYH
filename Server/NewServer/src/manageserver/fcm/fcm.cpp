#include "stdafx.h"
#include "fcm.h"
#include "FCMsdkBase.h"
#include "server_config.h"
#include "client/usermgr.h"
#include "role_data.h"
#include "onlinerole/online_role_data_mgr.h"
#include "message_handler.h"
#include <cstdio>
#include <memory>

int func_callbk(const char* szUserID, const char* szRoleID, int iOnlineMinutes, int iOfflineMinutes);
void func_switch(int iNeedFCM);
void func_Querycallback(const char* szUserID, const char* szRoleID, int istate, int iOnlineMinutes, int iOfflineMinutes);

CFcmMgr::CFcmMgr() = default;
CFcmMgr::~CFcmMgr() { UnInit(); }

BOOL CFcmMgr::Init()
{
    if (m_poFCM || m_callback_list.IsInitialized())
    {
        LOG_CHECK_ERROR(!m_poFCM && !m_callback_list.IsInitialized());
        return FALSE;
    }
    LOG_PROCESS_ERROR(g_ServerConfig.MS.bFcmEnable);
    LOG_PROCESS_ERROR(g_ServerConfig.MS.nFcmCallbackCount > 0);
    LOG_PROCESS_ERROR(m_callback_list.InitSlots(g_ServerConfig.MS.nFcmCallbackCount));
    INF("Starting FCM...");
    m_poFCM.reset(CreateSDKInstance());
    LOG_PROCESS_ERROR(m_poFCM);
    // The queue is ready before initFCMSDK can synchronously deliver a callback.
    LOG_PROCESS_ERROR(m_poFCM->initFCMSDK(FCM_APPID, g_ServerConfig.Common.nAreaID,
                                       0, func_callbk, func_switch, func_Querycallback));
    return TRUE;
Exit0:
    UnInit();
    return FALSE;
}

BOOL CFcmMgr::UnInit()
{
    m_callback_list.Close();
    m_poFCM.reset();
    m_callback_list.UnInit();
    return TRUE;
}

BOOL CFcmMgr::MainLoop()
{
    while (auto cb = m_callback_list.Pop())
    {
        MS_USER* user = CUserMgr::GetInstance().FindUserByUserName(cb->szUserName);
        if (user && !user->bAdult)
        {
            MS_ONLINE_ROLE_DATA* role = NULL;

            if (cb->nOnlineMinutes >= 300)
                user->byFcmState = 2;// fcmsNone;
            else if (cb->nOnlineMinutes >= 180)
                user->byFcmState = 1;// fcmsHalf;
            else
                user->byFcmState = 0;// fcmsFull;

            role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(user->dwCurRoleID);
            if (role)
            {

                CMessageHandler::GetInstance().DoGSFcmState(role->dwGSSessionID, role->pRoleData->dwRoleID, user->byFcmState);

                CMessageHandler::GetInstance().DoClientFcmState(role->dwCliSessionID, cb->nOnlineMinutes, cb->nOfflineMinutes);
            }
        }
    }

    return TRUE;
}

void CFcmMgr::SetOnline(const char* szUserID, UINT32 dwIP)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(g_ServerConfig.MS.bFcmEnable);

    nRetCode = m_poFCM->FCMIDOnline(szUserID, "a", inet_ntoa(*(in_addr*)(&dwIP)));
    LOG_PROCESS_ERROR(nRetCode == 1010);

Exit0:
    return;
}

void CFcmMgr::SetOffline(const char* szUserID, UINT32 dwIP)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(g_ServerConfig.MS.bFcmEnable);

    nRetCode = m_poFCM->FCMIDOffline(szUserID, "a", inet_ntoa(*(in_addr*)(&dwIP)));
    LOG_PROCESS_ERROR(nRetCode == 1010);

Exit0:
    return;
}

void CFcmMgr::QueryFcmState(const char* szUserID, UINT32 dwIP)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(g_ServerConfig.MS.bFcmEnable);

    nRetCode = m_poFCM->FCMQueryStateNoBlock(szUserID, "a", inet_ntoa(*(in_addr*)(&dwIP)));
    LOG_PROCESS_ERROR(nRetCode == 1010);

Exit0:
    return;
}

BOOL CFcmMgr::PushCallback(const char* user, INT32 online, INT32 offline) try
{
    if (!user)
        return FALSE;
    auto record = std::make_unique<FCM_CALLBACK>();
    strxcpy(record->szUserName, user, USER_NAME_LEN);
    record->nOnlineMinutes = online;
    record->nOfflineMinutes = offline;
    const auto accepted = m_callback_list.Push(std::move(record));
    LOG_CHECK_ERROR(accepted);
    return accepted;
}
catch (...)
{
    std::fputs("FCM callback allocation failed\n", stderr);
    return FALSE;
}

BOOL CFcmMgr::PopCallback(FCM_CALLBACK& callback)
{
    auto record = m_callback_list.Pop();
    if (!record)
        return FALSE;
    callback = *record;
    return TRUE;
}

int func_callbk(const char* szUserID, const char* szRoleID, int iOnlineMinutes, int iOfflineMinutes) try
{
    BOOL bRet = CFcmMgr::GetInstance().PushCallback(szUserID, iOnlineMinutes, iOfflineMinutes);
    LOG_PROCESS_ERROR(bRet);

Exit0:
    DBG("func_callbk: %s, %d, %d", szUserID, iOnlineMinutes, iOfflineMinutes);
    return 0;
}
catch (...)
{
    std::fputs("FCM state callback failed\n", stderr);
    return 0;
}


void func_switch(int iNeedFCM) try
{
    DBG("func_switch: %d", iNeedFCM);
}
catch (...)
{
    std::fputs("FCM switch callback failed\n", stderr);
}


void func_Querycallback(const char* szUserID, const char* szRoleID, int istate, int iOnlineMinutes, int iOfflineMinutes) try
{
    BOOL bRet = CFcmMgr::GetInstance().PushCallback(szUserID, iOnlineMinutes, iOfflineMinutes);
    LOG_PROCESS_ERROR(bRet);

Exit0:
    DBG("func_Querycallback: %s, %d, %d", szUserID, iOnlineMinutes, iOfflineMinutes);
}
catch (...)
{
    std::fputs("FCM query callback failed\n", stderr);
}
