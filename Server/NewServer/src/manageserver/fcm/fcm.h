#ifndef _FCM_H
#define _FCM_H

#include "string_def.h"
#include "callback_queue.h"
#include "release_owner.h"

struct FCM_CALLBACK
{
    char szUserName[USER_NAME_LEN];
    INT32 nOnlineMinutes, nOfflineMinutes;
};

class CFCMSDKBase;
class CFcmMgr
{
public:
    static CFcmMgr& GetInstance()
    {
        static CFcmMgr ms_Instance;
        return ms_Instance;
    }

    ~CFcmMgr();
    BOOL Init();
    BOOL UnInit();
    BOOL MainLoop();

    void SetOnline(const char* szUserID, UINT32 dwIP);
    void SetOffline(const char* szUserID, UINT32 dwIP);
    void QueryFcmState(const char* szUserID, UINT32 dwIP);

    BOOL PushCallback(const char* szUserID, INT32 nOnlineMinutes, INT32 nOfflineMinutes);
    BOOL PopCallback(FCM_CALLBACK& cb);

private:
    cqyh::CallbackQueue<FCM_CALLBACK> m_callback_list;
    cqyh::ReleasePtr<CFCMSDKBase> m_poFCM;

    CFcmMgr();
};

#endif // _FCM_H
