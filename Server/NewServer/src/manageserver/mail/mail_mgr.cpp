#include "stdafx.h"
#include "mail_mgr.h"

#include <time.h>

#include "time_ex.h"

#include "role_data_mgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "role_data.h"
#include "server_config.h"
#include "error_def.h"
#include "mail_def.h"
#include "message_handler.h"
#include "manageserver.h"
#include "zlib.h"
#include "mail_template_data.h"
#include "db/ms_sql_stmt_list.h"
#include "role_db_def.h"
#include "gm_def.h"

CMailMgr CMailMgr::ms_Instance;

UINT32 CMailMgr::_GetMailID(INT32 nGroupID)
{
    //Mail ID generation:
    //First 8 bits: group number
    //Last 24 bits: sequence starting at 1
    UINT32 mailid = 0;

    LOG_PROCESS_ERROR(IsValidGroup(nGroupID));

    mailid = m_dwSeeds[nGroupID] | (nGroupID << 24);

    m_dwSeeds[nGroupID]++;
    if (m_dwSeeds[nGroupID] > 0x00FFFFFF)
        m_dwSeeds[nGroupID] = 1;

Exit0:
    return mailid;
}

BOOL CMailMgr::Init()
{
    BOOL bRet = FALSE;
    UINT64 qwStartTick;
    CheckMailAttach checkmail;

    bRet = m_MailPool.Init(g_ServerConfig.MS.nInitRoleCount * 32);
    LOG_PROCESS_ERROR(bRet);

    memset(seed_finder, 0, sizeof(seed_finder));

    qwStartTick = GetTickCountEx();

    INF("Deleting Expired Mails...");

    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlDeleteExpiredMail, NULL, NULL, "");
    LOG_PROCESS_ERROR(bRet);

    INF("Deleted /*TODO*/ Mail(s) in %d ms", (INT32)(GetTickCountEx() - qwStartTick));

    qwStartTick = GetTickCountEx();

    INF("Loading Mails...");

    m_nLoadedCount = 0;
    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectMail, _Load, NULL, "");
    LOG_PROCESS_ERROR(bRet);

    INF("Loaded %d Mail(s) in %d ms", m_nLoadedCount, (INT32)(GetTickCountEx() - qwStartTick));

    qwStartTick = GetTickCountEx();

    INF("Checking Mail Attachments...");

    m_nProcessedLockedAttachCount = 0;
    bRet = m_MailPool.Traverse(checkmail);
    LOG_PROCESS_ERROR(bRet);

    INF("Processed %d Locked Mail Attachments in %d ms", m_nProcessedLockedAttachCount, (INT32)(GetTickCountEx() - qwStartTick));

    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; i++)
    {
        if (IsValidGroup(i))
        {
            m_dwSeeds[i] = seed_finder[i].FindSeed();
            LOG_PROCESS_ERROR(m_dwSeeds[i]);
        }
        else
        {
            m_dwSeeds[i] = 0;
        }
    }

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CMailMgr::UnInit()
{
    BOOL bRet = 0;
    DeleteAllMail del;

    bRet = m_MailPool.Traverse(del);
    LOG_CHECK_ERROR(bRet);

    bRet = m_MailPool.UnInit();
    LOG_CHECK_ERROR(bRet);

    return TRUE;
}

INT32 CMailMgr::WriteMail(MS_ONLINE_ROLE_DATA* sender, const char* receiver, const char* subject, const char* content)
{
    BOOL bRet = FALSE;
    MS_ONLINE_ROLE_DATA* online_role = NULL;
    MS_ROLE_DATA* role = NULL;
    LOG_PROCESS_ERROR(sender);
    LOG_PROCESS_ERROR(receiver);
    LOG_PROCESS_ERROR(strcmp(sender->pRoleData->szRoleName, receiver));
    LOG_PROCESS_ERROR(subject);
    LOG_PROCESS_ERROR(strlen(subject) <= MAIL_SUBJECT_LEN);
    LOG_PROCESS_ERROR(content);
    LOG_PROCESS_ERROR(strlen(content) <= MAIL_CONTENT_LEN);

    role = CRoleDataMgr::GetInstance().FindRoleByName(receiver);
    if (role == NULL)
        return errMailReceiverNotFound;

    if (sender->pRoleData->byGroup != role->byGroup)
        return errMailReceiverNotFound;

    if (role->dwMailCounts >= MAX_MAIL_COUNT)
        return errMailReceiverFull;

    role->dwMailCounts++;
    bRet = CMessageHandler::GetInstance().DoGSMailCostCheck(sender->dwGSSessionID, sender->pRoleData->dwRoleID, role->dwRoleID, subject, content);
    LOG_PROCESS_ERROR(bRet);

    return errSuccess;

Exit0:
    return errMailUnknown;
}

INT32 CMailMgr::ListMailRecv(MS_ONLINE_ROLE_DATA* role)
{
    CMessageHandler::GetInstance().DoClientMailListRecv(role->dwCliSessionID, role->pRoleData->lstMails, MAX_MAIL_LIST_COUNT);
    return errSuccess;
}

INT32 CMailMgr::ReadMail(MS_ONLINE_ROLE_DATA* role, UINT32 dwMailID)
{
    MailInfo* mailinfo = m_MailPool.Find(dwMailID);
    if (mailinfo == NULL)
        return errMailNotFound;

    LOG_PROCESS_ERROR(role);
    if (mailinfo->dwReceiverRoleID != role->pRoleData->dwRoleID)
        return errMailNotFound;

    if (mailinfo->byNew == 1)
    {
        BOOL bRet = FALSE;

        mailinfo->byNew = 0;
        bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateMailRead, NULL, NULL, "ud", dwMailID);
        LOG_CHECK_ERROR(bRet);
    }

    CMessageHandler::GetInstance().DoClientMailRead(role->dwCliSessionID, mailinfo);

    return errSuccess;

Exit0:
    return errMailUnknown;
}

INT32 CMailMgr::DeleteMail(MS_ONLINE_ROLE_DATA* role, UINT32 dwMailID)
{
    MailInfo* mailinfo = m_MailPool.Find(dwMailID);
    if (mailinfo == NULL)
        return errMailNotFound;

    LOG_PROCESS_ERROR(role);
    if (mailinfo->dwReceiverRoleID != role->pRoleData->dwRoleID)
        return errMailNotFound;

    if (role->pRoleData->dwMailCounts == 0)
        return errMailUnknown;

    {
        BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteMail, NULL, NULL, "ud", dwMailID);
        LOG_CHECK_ERROR(bRet);
    }

    role->pRoleData->lstMails.Remove(mailinfo);
    role->pRoleData->dwMailCounts--;
    m_MailPool.Delete(mailinfo);

    CMessageHandler::GetInstance().DoClientMailDeleteSuccess(role->dwCliSessionID, dwMailID);

    return errSuccess;

Exit0:
    return errMailUnknown;
}

INT32 CMailMgr::DeleteMailByGM(MS_ROLE_DATA* role, UINT32 dwMailID)
{
    INT32       nResult = errMailUnknown;
    MS_ONLINE_ROLE_DATA *pOnlineRole = NULL;
    MailInfo*   mailinfo = NULL;


    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(role->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole == NULL, errGMRoleOnline);

    mailinfo = m_MailPool.Find(dwMailID);
    if (mailinfo == NULL)
        return errMailNotFound;

    LOG_PROCESS_ERROR(role);
    if (mailinfo->dwReceiverRoleID != role->dwRoleID)
        return errMailNotFound;

    if (role->dwMailCounts == 0)
        return errMailUnknown;

    {
        BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteMail, NULL, NULL, "ud", dwMailID);
        LOG_CHECK_ERROR(bRet);
    }

    role->lstMails.Remove(mailinfo);
    role->dwMailCounts--;
    m_MailPool.Delete(mailinfo);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMailMgr::GetMailAttach(MS_ONLINE_ROLE_DATA* role, UINT32 dwMailID)
{
    BOOL bRet = FALSE;
    MailInfo* mailinfo = m_MailPool.Find(dwMailID);
    if (mailinfo == NULL)
        return errMailNotFound;

    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR( role->pRoleData );

    if (mailinfo->dwReceiverRoleID != role->pRoleData->dwRoleID)
        return errMailNotFound;

    if (mailinfo->ma.byAttachType == ATTACHTYPE_NONE)
        return errMailAttachNotFound;

    if (mailinfo->ma.byAttachLocked)
        return errMailAttachNotFound;

    if (mailinfo->byNew == 1)
    {
        mailinfo->byNew = 0;
        bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateMailRead, NULL, NULL, "ud", dwMailID);
        LOG_CHECK_ERROR(bRet);
    }

    mailinfo->ma.byAttachLocked = 1;
    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmtOwned(mysqlUpdateMailAttachLocked,
        [mailID = dwMailID, receiverID = mailinfo->dwReceiverRoleID, mailTime = mailinfo->dwMailTime]
        (std::list<void*>& rows, void*) { return _OnMailAttachLocked(rows, mailID, receiverID, mailTime); },
        "cud", 1, dwMailID);
    LOG_PROCESS_ERROR(bRet);

    return errSuccess;

Exit0:
    return errMailUnknown;
}

MailInfo* CMailMgr::GetMailInfo(UINT32 dwMailID)
{
    MailInfo* mailinfo = m_MailPool.Find(dwMailID);
    PROCESS_ERROR(mailinfo);

    return mailinfo;
Exit0:
    return NULL;
}

void CMailMgr::OnMailCostCheck(INT32 nResult, UINT32 dwSenderRoleID, UINT32 dwReceiverRoleID, const char* subject, const char* content)
{
    MS_ROLE_DATA* sender = CRoleDataMgr::GetInstance().FindRole(dwSenderRoleID);
    MS_ROLE_DATA* receiver = CRoleDataMgr::GetInstance().FindRole(dwReceiverRoleID);
    MS_ONLINE_ROLE_DATA* online_sender = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwSenderRoleID);
    MS_ONLINE_ROLE_DATA* online_receiver = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwReceiverRoleID);

    LOG_PROCESS_ERROR(sender);
    LOG_PROCESS_ERROR(receiver);
    LOG_PROCESS_ERROR(receiver->dwMailCounts > 0);

    if (nResult == errSuccess)
    {
        BOOL bRet = FALSE;

        UINT32 mailid = _GetMailID(receiver->byGroup);
        LOG_PROCESS_ERROR(mailid);

        MailInfo* mail = m_MailPool.New(mailid);
        LOG_PROCESS_ERROR(mail);

        mail->dwMailID = m_MailPool.GetObjID(mail);
        mail->dwMailTime = CManageServer::GetInstance().GetGameTime();  //_time32(NULL); // for linux compatible
        mail->dwSenderRoleID = dwSenderRoleID;
        mail->dwReceiverRoleID = dwReceiverRoleID;
        mail->byMailType = MAILTYPE_USER;
        mail->byNew = 1;
        strxcpy(mail->szSenderRoleName, sender->szRoleName, ROLE_NAME_LEN);
        strxcpy(mail->szMailSubject, subject, MAIL_SUBJECT_LEN);
        strxcpy(mail->szMailContent, content, MAIL_CONTENT_LEN);
        mail->ma.byAttachType = ATTACHTYPE_NONE;
        mail->ma.byAttachLen = 0;

        ClearNode(mail);
        receiver->lstMails.PushRear(mail);

        if (online_sender)
            CMessageHandler::GetInstance().DoClientMailWriteSuccess(online_sender->dwCliSessionID);

        if (online_receiver)
            CMessageHandler::GetInstance().DoClientMailNewCount(online_receiver->dwCliSessionID, receiver->lstMails);

        bRet = _AddMailSql(receiver->byGroup, mail);
        LOG_CHECK_ERROR(bRet);
    }
    else
    {
        receiver->dwMailCounts--;
        if (online_sender)
            CMessageHandler::GetInstance().DoErrorNotify(online_sender->dwCliSessionID, nResult, 0, 0);
    }

    return;

Exit0:
    if (receiver && receiver->dwMailCounts > 0)
        receiver->dwMailCounts--;

    if (online_sender)
        CMessageHandler::GetInstance().DoErrorNotify(online_sender->dwCliSessionID, errMailUnknown, 0, 0);
}

void CMailMgr::OnMailGetAttach(INT32 nResult, UINT32 dwMailID)
{
    MailInfo* mail = m_MailPool.Find(dwMailID);
    LOG_PROCESS_ERROR(mail);
    LOG_PROCESS_ERROR(mail->ma.byAttachType == ATTACHTYPE_SIMPLE || mail->ma.byAttachType == ATTACHTYPE_BUFFER);
    LOG_PROCESS_ERROR(mail->ma.byAttachLocked);

    if (nResult == errSuccess)
    {
        MS_ONLINE_ROLE_DATA* online_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(mail->dwReceiverRoleID);
        BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateMailAttach, NULL, NULL, "ud", dwMailID);
        LOG_CHECK_ERROR(bRet);

        mail->ma.byAttachType = ATTACHTYPE_NONE;
        mail->ma.byAttachLocked = 0;

        if (online_role)
        {
            CMessageHandler::GetInstance().DoClientMailGetAttachAccess(online_role->dwCliSessionID, dwMailID);
        }
    }
    else
    {
        MS_ONLINE_ROLE_DATA* online_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(mail->dwReceiverRoleID);
        BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateMailAttachLocked, NULL, NULL, "cud", 0, dwMailID);
        LOG_CHECK_ERROR(bRet);

        mail->ma.byAttachLocked = 0;

        if (online_role)
        {
            CMessageHandler::GetInstance().DoErrorNotify(online_role->dwCliSessionID, nResult, 0, 0);
        }
    }

Exit0:
    return;
}

BOOL CMailMgr::SendSystemMail(MS_ROLE_DATA* receiver, INT32 nMailTemplateID)
{
    const MAIL_TEMPLATE_DATA* mail_data = NULL;
    
    LOG_PROCESS_ERROR(nMailTemplateID);
    
    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
    LOG_PROCESS_ERROR(mail_data);

    if (mail_data->m_nItemID && mail_data->m_nItemCount)
        return SendSystemMailWithSimpleAttach(receiver, mail_data->m_szMailSubject, mail_data->m_szMailContent, mail_data->m_nItemID, mail_data->m_nItemCount);
    else
        return SendSystemMail(receiver, mail_data->m_szMailSubject, mail_data->m_szMailContent);

Exit0:
    return FALSE;
}

BOOL CMailMgr::SendSystemMail(MS_ROLE_DATA* receiver, const char* subject, const char* content)
{
    UINT32 mailid{};
    MailInfo* mail = NULL;
    MS_ONLINE_ROLE_DATA* online_role = NULL;
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(receiver);
    LOG_PROCESS_ERROR(subject);
    LOG_PROCESS_ERROR(strlen(subject) <= MAIL_SUBJECT_LEN);
    LOG_PROCESS_ERROR(content);
    LOG_PROCESS_ERROR(strlen(content) <= MAIL_CONTENT_LEN);

    mailid = _GetMailID(receiver->byGroup);
    LOG_PROCESS_ERROR(mailid);

    mail = m_MailPool.New(mailid);
    LOG_PROCESS_ERROR(mail);

    receiver->dwMailCounts++;

    mail->dwMailID = m_MailPool.GetObjID(mail);
    mail->dwMailTime = CManageServer::GetInstance().GetGameTime();  //_time32(NULL); // for linux compatible
    mail->dwSenderRoleID = 0;
    mail->dwReceiverRoleID = receiver->dwRoleID;
    mail->byMailType = MAILTYPE_SYSTEM;
    mail->byNew = 1;
    memset(mail->szSenderRoleName, 0, ROLE_NAME_LEN);
    strxcpy(mail->szMailSubject, subject, MAIL_SUBJECT_LEN);
    strxcpy(mail->szMailContent, content, MAIL_CONTENT_LEN);

    mail->ma.byAttachLocked = 0;
    mail->ma.byAttachType = ATTACHTYPE_NONE;
    mail->ma.byAttachLen = 0;

    ClearNode(mail);
    receiver->lstMails.PushRear(mail);

    online_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(receiver->dwRoleID);
    if (online_role)
        CMessageHandler::GetInstance().DoClientMailNewCount(online_role->dwCliSessionID, receiver->lstMails);

    bRet = _AddMailSql(receiver->byGroup, mail);
    LOG_CHECK_ERROR(bRet);

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CMailMgr::SendSystemMailWithSimpleAttach(MS_ROLE_DATA* receiver, const char* subject, const char* content, INT32 itemid, INT32 itemcount)
{
    UINT32 mailid{};
    MailInfo* mail = NULL;
    MS_ONLINE_ROLE_DATA* online_role = NULL;
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(receiver);
    LOG_PROCESS_ERROR(subject);
    LOG_PROCESS_ERROR(strlen(subject) <= MAIL_SUBJECT_LEN);
    LOG_PROCESS_ERROR(content);
    LOG_PROCESS_ERROR(strlen(content) <= MAIL_CONTENT_LEN);
    LOG_PROCESS_ERROR(itemid > 0 && itemcount > 0);

    mailid = _GetMailID(receiver->byGroup);
    LOG_PROCESS_ERROR(mailid);

    mail = m_MailPool.New(mailid);
    LOG_PROCESS_ERROR(mail);

    receiver->dwMailCounts++;

    mail->dwMailID = m_MailPool.GetObjID(mail);
    mail->dwMailTime = CManageServer::GetInstance().GetGameTime();
    mail->dwSenderRoleID = 0;
    mail->dwReceiverRoleID = receiver->dwRoleID;
    mail->byMailType = MAILTYPE_SYSTEM;
    mail->byNew = 1;
    memset(mail->szSenderRoleName, 0, ROLE_NAME_LEN);
    strxcpy(mail->szMailSubject, subject, MAIL_SUBJECT_LEN);
    strxcpy(mail->szMailContent, content, MAIL_CONTENT_LEN);

    mail->ma.byAttachLocked = 0;
    mail->ma.byAttachType = ATTACHTYPE_SIMPLE;
    mail->ma.byAttachLen = sizeof(SimpleAttach);
    mail->ma.sa.nItemID = itemid;
    mail->ma.sa.nItemCount = itemcount;

    ClearNode(mail);
    receiver->lstMails.PushRear(mail);

    online_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(receiver->dwRoleID);
    if (online_role)
        CMessageHandler::GetInstance().DoClientMailNewCount(online_role->dwCliSessionID, receiver->lstMails);

    bRet = _AddMailSql(receiver->byGroup, mail);
    LOG_CHECK_ERROR(bRet);

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CMailMgr::SendSystemMailWithBufferAttach(MS_ROLE_DATA* receiver, const char* subject, const char* content, const char* itembuffer, UINT8 bufferlen)
{
    UINT32 mailid{};
    MailInfo* mail = NULL;
    MS_ONLINE_ROLE_DATA* online_role = NULL;
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(receiver);
    LOG_PROCESS_ERROR(subject);
    LOG_PROCESS_ERROR(strlen(subject) <= MAIL_SUBJECT_LEN);
    LOG_PROCESS_ERROR(content);
    LOG_PROCESS_ERROR(strlen(content) <= MAIL_CONTENT_LEN);
    LOG_PROCESS_ERROR(bufferlen <= BUFFER_ATTACH_LEN);

    mailid = _GetMailID(receiver->byGroup);
    LOG_PROCESS_ERROR(mailid);

    mail = m_MailPool.New(mailid);
    LOG_PROCESS_ERROR(mail);

    receiver->dwMailCounts++;

    mail->dwMailID = m_MailPool.GetObjID(mail);
    mail->dwMailTime = CManageServer::GetInstance().GetGameTime();
    mail->dwSenderRoleID = 0;
    mail->dwReceiverRoleID = receiver->dwRoleID;
    mail->byMailType = MAILTYPE_SYSTEM;
    mail->byNew = 1;
    memset(mail->szSenderRoleName, 0, ROLE_NAME_LEN);
    strxcpy(mail->szMailSubject, subject, MAIL_SUBJECT_LEN);
    strxcpy(mail->szMailContent, content, MAIL_CONTENT_LEN);

    mail->ma.byAttachLocked = 0;
    mail->ma.byAttachType = ATTACHTYPE_BUFFER;
    mail->ma.byAttachLen = bufferlen;
    memcpy(mail->ma.buffer, itembuffer, bufferlen);

    ClearNode(mail);
    receiver->lstMails.PushRear(mail);

    online_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(receiver->dwRoleID);
    if (online_role)
        CMessageHandler::GetInstance().DoClientMailNewCount(online_role->dwCliSessionID, receiver->lstMails);

    bRet = _AddMailSql(receiver->byGroup, mail);
    LOG_CHECK_ERROR(bRet);

    return TRUE;

Exit0:
    return FALSE;
}

INT64 CMailMgr::PrintMemoryStat(void)
{
    INT64 llTotalSize = (INT64)m_MailPool.GetUsedSize() * sizeof(MailInfo);

    INF("all mails allocated %.2fMB", llTotalSize / (1024 * 1024.0f));

    return llTotalSize;
}

//Read each message and check whether AttachLocked is 1
//0 means normal; continue to the next message
//1 means attachment processing failed; read the player’s ProcessedMails from the role table
//If the mail was processed, set AttachLocked to 0 and delete the attachment.
//If the mail was not processed, set AttachLocked to 0 and retain the attachment.
BOOL CMailMgr::_Load(std::list<void*>& rResult, void* pUserData)
{
    BOOL bRet = FALSE;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8 byGroupID;
        UINT32 dwMailID;
        UINT32 dwMailTime;
        UINT32 dwSenderRoleID;
        UINT32 dwReceiverRoleID;
        UINT8 byMailType;
        UINT8 byNew;
        UINT8 byAttachLocked;
        char szMailSubject[MAIL_SUBJECT_LEN];
        char szMailContent[MAIL_CONTENT_LEN];
        UINT8 byAttachType;
        UINT32 dwAttachBufferLen;
        char AttachBuffer[BUFFER_ATTACH_LEN];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->byGroupID));

        bRet = CMailMgr::ms_Instance._LoadMail(
            pLoadResult->dwMailID,
            pLoadResult->dwMailTime,
            pLoadResult->dwSenderRoleID,
            pLoadResult->dwReceiverRoleID,
            pLoadResult->byMailType,
            pLoadResult->byNew,
            pLoadResult->byAttachLocked,
            pLoadResult->szMailSubject,
            pLoadResult->szMailContent,
            pLoadResult->byAttachType,
            pLoadResult->dwAttachBufferLen,
            pLoadResult->AttachBuffer
            );
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMailMgr::_LoadMail(UINT32 dwMailID, UINT32 dwMailTime, UINT32 dwSenderRoleID, UINT32 dwReceiverRoleID, UINT8 byMailType, UINT8 byNew, UINT8 byAttachLocked,
                         char* szMailSubject, char* szMailContent, UINT8 byAttachType, UINT32 dwAttachBufferLen, char* AttachBuffer)
{
    INT32 group_id{};
    BOOL bRet{};
    MailInfo* mail = NULL;

    LOG_PROCESS_ERROR(byAttachType < ATTACHTYPE_COUNT);
    LOG_PROCESS_ERROR(dwAttachBufferLen <= BUFFER_ATTACH_LEN);

    mail = m_MailPool.New(dwMailID);
    LOG_PROCESS_ERROR(mail);

    mail->dwMailID = dwMailID;
    mail->dwMailTime = dwMailTime;
    mail->dwSenderRoleID = dwSenderRoleID;
    mail->dwReceiverRoleID = dwReceiverRoleID;
    mail->byMailType = byMailType;
    mail->byNew = byNew;
    strxcpy(mail->szMailSubject, szMailSubject, MAIL_SUBJECT_LEN);
    strxcpy(mail->szMailContent, szMailContent, MAIL_CONTENT_LEN);

    mail->ma.byAttachLocked = byAttachLocked;
    mail->ma.byAttachType = byAttachType;
    mail->ma.byAttachLen = (UINT8)dwAttachBufferLen;
    if (byAttachType == ATTACHTYPE_SIMPLE || byAttachType == ATTACHTYPE_BUFFER)
    {
        LOG_PROCESS_ERROR(dwAttachBufferLen > 0);
        memcpy(mail->ma.buffer, AttachBuffer, dwAttachBufferLen);
    }

    group_id = dwMailID >> 24;
    LOG_PROCESS_ERROR(IsValidGroup(group_id));

    bRet = seed_finder[group_id].CheckID(dwMailID & 0x00FFFFFF);
    LOG_PROCESS_ERROR(bRet);

    m_nLoadedCount++;

    return TRUE;

Exit0:
    if (mail)
    {
        m_MailPool.Delete(mail);
    }
    return FALSE;
}

BOOL CMailMgr::_ProcessMail(MailInfo* pMailData)
{
    BOOL bRet;
    MS_ROLE_DATA* receiver = NULL;

    if (pMailData->ma.byAttachLocked)
    {
        bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectRoleData, _ProcessLockedAttach, pMailData, "ud", pMailData->dwReceiverRoleID);
        LOG_PROCESS_ERROR(bRet);

        m_nProcessedLockedAttachCount++;
    }

    if (pMailData->byMailType == MAILTYPE_USER)
    {
        MS_ROLE_DATA* sender = CRoleDataMgr::GetInstance().FindRole(pMailData->dwSenderRoleID);
        if (sender)
            strxcpy(pMailData->szSenderRoleName, sender->szRoleName, ROLE_NAME_LEN);
        else
            strxcpy(pMailData->szSenderRoleName, "Unknown Sender", ROLE_NAME_LEN);
    }

    receiver = CRoleDataMgr::GetInstance().FindRole(pMailData->dwReceiverRoleID);
    if (receiver)
    {
        ClearNode(pMailData);
        receiver->lstMails.PushRear(pMailData);
        receiver->dwMailCounts++;
    }
    else
        m_MailPool.Delete(pMailData);

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CMailMgr::_ProcessLockedAttach(std::list<void*>& rResult, void* pUserData)
{
    BOOL bRet;
    BOOL bProcessed = FALSE;
    MailInfo* mail = (MailInfo*)pUserData;

#pragma pack(push, 1)
    struct SELECT_RESULT
    {
        UINT32 dwRoleDataLen;
        char RoleData[32768];
    };
#pragma pack(pop)

    if (rResult.size() == 1)
    {
        SELECT_RESULT* select_result = (SELECT_RESULT*)rResult.front();
        unsigned char role_data_buf[65536];
        unsigned long buflen = 65536;

        INT32 nRet = uncompress(role_data_buf, &buflen, (unsigned char*)select_result->RoleData, select_result->dwRoleDataLen);
        LOG_PROCESS_ERROR(nRet == Z_OK);

        ROLE_DB_DATA* pRoleData = (ROLE_DB_DATA*)role_data_buf;
        LOG_PROCESS_ERROR(pRoleData->dwLen == buflen);

        for (UINT32 i = 0; i < rbtTotal; i++)
        {
            ROLE_DB_DATA_BLOCK* pBlock = &pRoleData->BlockTable[i];
            if (pBlock->btBlockType == rbtProcessedMails && pBlock->dwBlockSize > 0)
            {
                const ROLE_DB_PROCESSED_MAILS* pMailData = (const ROLE_DB_PROCESSED_MAILS*)&pRoleData->RoleData[pBlock->dwStartOffset];
                for (UINT8 j=0; j<pMailData->byMailCount; j++)
                {
                    if (mail->dwMailID == pMailData->dwMailID[j])
                    {
                        bProcessed = TRUE;
                        break;
                    }
                }
                break;
            }
        }
    }
    else
    {
        LOG_PROCESS_ERROR(FALSE);
    }

    if (bProcessed)
    {
        mail->ma.byAttachLocked = 0;
        mail->ma.byAttachType = ATTACHTYPE_NONE;
        mail->ma.byAttachLen = 0;
        bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateMailAttach, NULL, NULL, "ud", mail->dwMailID);
        LOG_PROCESS_ERROR(bRet);
    }
    else
    {
        mail->ma.byAttachLocked = 0;
        bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateMailAttachLocked, NULL, NULL, "cud", 0, mail->dwMailID);
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMailMgr::_OnMailAttachLocked(std::list<void*>& rResult, UINT32 mailID, UINT32 receiverID, UINT32 mailTime)
{
    MS_ONLINE_ROLE_DATA* role = NULL;
    MailInfo* mail = GetInstance().m_MailPool.Find(mailID);
    // The mail may have been deleted while this asynchronous update was queued.
    if (!mail || mail->dwReceiverRoleID != receiverID || mail->dwMailTime != mailTime)
        return TRUE;

    LOG_PROCESS_ERROR(mail);
    LOG_PROCESS_ERROR(mail->ma.byAttachType == ATTACHTYPE_SIMPLE || mail->ma.byAttachType == ATTACHTYPE_BUFFER);
    LOG_PROCESS_ERROR(mail->ma.byAttachLocked);

    role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(mail->dwReceiverRoleID);
    PROCESS_ERROR(role);

    if (mail->ma.byAttachType == ATTACHTYPE_SIMPLE)
    {
        BOOL bRet = CMessageHandler::GetInstance().DoGSGetSimpleAttach(role->dwGSSessionID, mail->dwReceiverRoleID, mail->dwMailID, mail->ma.sa.nItemID, mail->ma.sa.nItemCount);
        LOG_PROCESS_ERROR(bRet);
    }
    else if (mail->ma.byAttachType == ATTACHTYPE_BUFFER)
    {
        BOOL bRet = CMessageHandler::GetInstance().DoGSGetBufferAttach(role->dwGSSessionID, mail->dwReceiverRoleID, mail->dwMailID, mail->ma.buffer);
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;

Exit0:
    if (mail)
    {
        BOOL bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateMailAttachLocked, NULL, NULL, "cud", 0, mail->dwMailID);
        LOG_CHECK_ERROR(bRet);
    }

    return FALSE;
}

BOOL CMailMgr::_AddMailSql(UINT8 byGroupID, MailInfo* mail)
{
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(IsValidGroup(byGroupID));

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlInsertMail, NULL, NULL, "ucudududucssucb",
        byGroupID, mail->dwMailID, mail->dwSenderRoleID, mail->dwReceiverRoleID, mail->byMailType, mail->szMailSubject, mail->szMailContent,
        mail->ma.byAttachType, mail->ma.buffer, mail->ma.byAttachLen);
    LOG_PROCESS_ERROR(bRet);

Exit0:
    return bRet;
}

bool CMailMgr::CheckMailAttach::operator ()(UINT32 dwMailID, MailInfo* pMailData)
{
    return CMailMgr::GetInstance()._ProcessMail(pMailData);
}

bool CMailMgr::DeleteAllMail::operator()(UINT32 dwMailID, MailInfo* pMailData)
{
    BOOL bRet = FALSE;
    MS_ROLE_DATA* role_data = NULL;

    LOG_PROCESS_ERROR(pMailData);
    LOG_PROCESS_ERROR(dwMailID == pMailData->dwMailID);

    role_data = CRoleDataMgr::GetInstance().FindRole(pMailData->dwReceiverRoleID);
    LOG_PROCESS_ERROR(role_data);

    role_data->lstMails.Remove(pMailData);
    role_data->dwMailCounts--;

    bRet = ms_Instance.m_MailPool.Delete(pMailData);
    LOG_CHECK_ERROR(bRet);

Exit0:
    return TRUE;
}

BOOL CMailMgr::SEED_FINDER::CheckID(UINT32 _id)
{
    LOG_PROCESS_ERROR(_id);
    LOG_PROCESS_ERROR(_id > last_id);

    if (first_id == 0)
        first_id = _id;

    if (last_id != 0)
    {
        UINT32 gap = _id - last_id;
        if (biggest_gap < gap)
        {
            biggest_gap = gap;
            biggest_gap_start_id = last_id;
        }
    }

    last_id = _id;

    return TRUE;

Exit0:
    return FALSE;
}

UINT32 CMailMgr::SEED_FINDER::FindSeed()
{
    if (first_id == 0)
        return 1;

    if (biggest_gap == 0)
        return first_id + 1;

    UINT32 cycle_gap = (0x00FFFFFF - last_id) + first_id;

    if (cycle_gap > biggest_gap)
    {
        LOG_PROCESS_ERROR(cycle_gap > 500000);
        return last_id + 1;
    }
    else
    {
        LOG_PROCESS_ERROR(biggest_gap > 500000);
        return biggest_gap_start_id + 1;
    }

Exit0:
    return 0;
}
