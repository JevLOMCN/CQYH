#ifndef _MAIL_MGR_H_
#define _MAIL_MGR_H_

#include "mail.h"
#include "id_object_pool.h"
#include "server_def.h"

struct MS_ROLE_DATA;
struct MS_ONLINE_ROLE_DATA;
class CMailMgr
{
public:
    BOOL Init();
    BOOL UnInit();
    static CMailMgr& GetInstance() { return ms_Instance; }

    INT32 WriteMail(MS_ONLINE_ROLE_DATA* sender, const char* receiver, const char* subject, const char* content);
    INT32 ListMailRecv(MS_ONLINE_ROLE_DATA* role);
    INT32 ReadMail(MS_ONLINE_ROLE_DATA* role, UINT32 dwMailID);
    INT32 DeleteMail(MS_ONLINE_ROLE_DATA* role, UINT32 dwMailID);
    INT32 DeleteMailByGM(MS_ROLE_DATA* role, UINT32 dwMailID);
    INT32 GetMailAttach(MS_ONLINE_ROLE_DATA* role, UINT32 dwMailID);

    MailInfo*   GetMailInfo(UINT32 dwMailID);

    void OnMailCostCheck(INT32 nResult, UINT32 dwSenderRoleID, UINT32 dwReceiverRoleID, const char* subject, const char* content);
    void OnMailGetAttach(INT32 nResult, UINT32 dwMailID);

    BOOL SendSystemMail(MS_ROLE_DATA* receiver, INT32 nMailTemplateID);
    BOOL SendSystemMail(MS_ROLE_DATA* receiver, const char* subject, const char* content);
    BOOL SendSystemMailWithSimpleAttach(MS_ROLE_DATA* receiver, const char* subject, const char* content, INT32 itemid, INT32 itemcount);
    BOOL SendSystemMailWithBufferAttach(MS_ROLE_DATA* receiver, const char* subject, const char* content, const char* itembuffer, UINT8 bufferlen);

    INT64 PrintMemoryStat(void);

private:
    static BOOL _Load(std::list<void*>& rResult, void* pUserData);
    BOOL _LoadMail(UINT32 dwMailID, UINT32 dwMailTime, UINT32 dwSenderRoleID, UINT32 dwReceiverRoleID, UINT8 byMailType, UINT8 byNew, UINT8 byAttachLocked,
        char* szMailSubject, char* szMailContent, UINT8 byAttachType, UINT32 dwAttachBufferLen, char* AttachBuffer);
    BOOL _ProcessMail(MailInfo* pMailData);
    static BOOL _ProcessLockedAttach(std::list<void*>& rResult, void* pUserData);
    static BOOL _OnMailAttachLocked(std::list<void*>& rResult, UINT32 mailID, UINT32 receiverID, UINT32 mailTime);
    BOOL _AddMailSql(UINT8 byGroupID, MailInfo* pMailData);

    UINT32 _GetMailID(INT32 nGroupID);
    struct SEED_FINDER
    {
        UINT32 first_id;
        UINT32 last_id;
        UINT32 biggest_gap;
        UINT32 biggest_gap_start_id;

        BOOL CheckID(UINT32 _id);
        UINT32 FindSeed();
    };
    SEED_FINDER seed_finder[MAX_SERVER_GROUP_COUNT];

    static CMailMgr ms_Instance;
    UINT32 m_dwSeeds[MAX_SERVER_GROUP_COUNT];
    UINT32 m_nLoadedCount;
    UINT32 m_nProcessedLockedAttachCount;

    struct CheckMailAttach
    {
        bool operator()(UINT32 dwMailID, MailInfo* pMailData);
    };
    struct DeleteAllMail
    {
        bool operator()(UINT32 dwMailID, MailInfo* pMailData);
    };

    CIDObjectPool<MailInfo, UINT32> m_MailPool;
    std::multimap<UINT32, MailInfo*> m_ReceiverMap;
};


#endif // _MAIL_MGR_H_
