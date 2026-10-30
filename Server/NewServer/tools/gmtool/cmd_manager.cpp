#include "cmd_manager.h"
#include "message_handler.h"
#include "gm_log.h"
#include "process_error.h"
#include <algorithm>
#include <cstring>
#include <limits>
#include <string_view>

class CRoleInfo;
extern CRoleInfo* g_pRoleInfo;

namespace
{
template <class T> std::shared_ptr<T> typed(CommandPtr command, INT32 kind)
{
    if (!command || command->Kind() != kind || command->nType != kind)
        return {};
    return std::static_pointer_cast<T>(std::move(command));
}
template <std::size_t Size> bool copy_field(char (&destination)[Size], const char* source)
{
    if (!source) return false;
    const std::string_view text(source);
    if (text.size() >= Size) return false;
    std::fill_n(destination, Size, '\0');
    std::copy(text.begin(), text.end(), destination);
    return true;
}
bool wire_time(INT64 value)
{
    return value >= 0 && static_cast<UINT64>(value) <= (std::numeric_limits<UINT32>::max)();
}
struct Issuing
{
    CommandPtr command;
    ~Issuing() { --command->in_flight; }
};
}

CCmdManager CCmdManager::m_Instance;
CCmdManager& CCmdManager::GetInstance() { return m_Instance; }
CCmdManager::CCmdManager() : m_nState(0), m_nResult(0), m_IsCmdComplete(TRUE) {}
CCmdManager::~CCmdManager() = default;

BOOL CCmdManager::Init()
{
    UnInit();
    m_initialized = true;
    return TRUE;
}
BOOL CCmdManager::UnInit()
{
    m_initialized = false;
    m_continuation.reset();
    m_pCmd.reset();
    ClearSvrList();
    m_IsCmdComplete = TRUE;
    m_now = Clock::now;
    return TRUE;
}
void CCmdManager::SetClock(std::function<Clock::time_point()> clock)
{
    m_now = clock ? std::move(clock) : std::function<Clock::time_point()>(Clock::now);
}
CCmdManager::Clock::time_point CCmdManager::DeadlineAfter(std::chrono::seconds timeout) const
{
    const auto now = m_now();
    const auto latest = Clock::time_point::max() - timeout;
    return now > latest ? Clock::time_point::max() : now + timeout;
}
BOOL CCmdManager::Submit(CommandPtr command, std::chrono::seconds timeout, std::function<BOOL()> send)
{
    if (!m_initialized || !command || command->Kind() != command->nType ||
        command->in_flight != 0)
        return FALSE;
    if (m_pCmd && m_pCmd != command)
    {
        const auto parent = typed<PRE_QUERY>(m_pCmd, CMD_PRE_QUERY);
        const auto wrapper = typed<PRE_QUERY>(command, CMD_PRE_QUERY);
        if ((!parent || parent->pCmd != command) && (!wrapper || wrapper->pCmd != m_pCmd)) return FALSE;
    }
    command->deadline = DeadlineAfter(timeout);
    m_continuation.reset();
    ++command->in_flight;
    m_pCmd = command;
    m_IsCmdComplete = FALSE;
    Issuing issuing{command};
    BOOL sent = FALSE;
    try { sent = send(); }
    catch (...)
    {
        DelCmd(command);
        throw;
    }
    if (!sent)
    {
        DelCmd(command);
        LOG_CHECK_ERROR(sent);
    }
    return sent && m_initialized;
}
BOOL CCmdManager::MainLoop()
{
    const auto command = m_pCmd;
    if (!command || command->in_flight != 0) return TRUE;
    const bool timeout = m_now() >= command->deadline;
    if (!timeout && m_continuation && m_continuation->owner == command)
    {
        auto continuation = std::move(*m_continuation);
        m_continuation.reset();
        try
        {
            const auto sent = continuation.run();
            if (!sent) DelCmd(command);
            return sent;
        }
        catch (...) { DelCmd(command); throw; }
    }
    if (!timeout && command->Kind() == CMD_QUERY_NAME_TO_ID && command->nType == CMD_QUERY_NAME_TO_ID)
    {
        auto query = std::static_pointer_cast<QUERY_NAME_TO_ID>(command);
        if (query->pending_next)
        {
            query->pending_next = false;
            return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [query]
            {
                return CMessageHandler::GetInstance().DoQueryNameToID(query->current_index,
                    query->RoleNameMap.at(query->current_index).szRoleName);
            });
        }
    }
    if (timeout || command->nSvrNum <= 0)
    {
        DelCmd(command);
        if (timeout) CGMLog::GetInstance().Log("Command timed out!");
    }
    return TRUE;
}

BOOL CCmdManager::PreQuery(INT32 type, const char* name, CommandPtr target)
{
    if (!target || target->Kind() != type || target->nType != type) return FALSE;
    switch (type)
    {
    case CMD_QUERY_ROLE_DATA: case CMD_LOAD_ROLE_DATA: case CMD_EXE_ROLE_GM_CMD:
    case CMD_FORBIDDEN_CHAT: case CMD_FREEZE_ROLE: case CMD_KICK_ROLE: case CMD_SEND_MAIL: break;
    default: return FALSE;
    }
    auto command = std::static_pointer_cast<PRE_QUERY>(NewCmd(CMD_PRE_QUERY));
    if (!copy_field(command->szRoleName, name)) return FALSE;
    command->nCmdType = type;
    command->reply_peer = target->reply_peer;
    command->reply_connection_lifetime = target->reply_connection_lifetime;
    command->pCmd = std::move(target);
    return Submit(command, std::chrono::seconds(GM_SVR_TIMEOUT), [command] {
        return CMessageHandler::GetInstance().DoPreQuery(command->szRoleName);
    });
}
BOOL CCmdManager::QueryRoleID(INT32 type, const char* name)
{
    auto command = std::static_pointer_cast<QUERY_ROLE_ID>(NewCmd(CMD_QUERY_ROLE_ID));
    if (!copy_field(command->szName, name)) return FALSE;
    command->nQueryType = type;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [command] {
        return CMessageHandler::GetInstance().DoQueryRoleID(command->nQueryType, command->szName);
    });
}
BOOL CCmdManager::QueryRoleData(CommandPtr input)
{
    auto command = typed<QUERY_ROLE_DATA>(std::move(input), CMD_QUERY_ROLE_DATA);
    if (!command) return FALSE;
    return Submit(command, std::chrono::seconds(TL_CMD_LONG_TIMEOUT), [command] {
        return CMessageHandler::GetInstance().DoQueryRoleData(command->dwRoleID);
    });
}
BOOL CCmdManager::ForbidChat(CommandPtr input)
{
    auto command = typed<FORBIDDEN_CHAT>(std::move(input), CMD_FORBIDDEN_CHAT);
    if (!command || command->roleList.empty() || !wire_time(command->llTime)) return FALSE;
    const auto role = command->roleList.front().dwRoleID;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [command, role] {
        return CMessageHandler::GetInstance().DoForbidChat(static_cast<UINT32>(command->llTime), role);
    });
}
BOOL CCmdManager::FreezeRole(CommandPtr input)
{
    auto command = typed<FREEZE_ROLE>(std::move(input), CMD_FREEZE_ROLE);
    if (!command || command->roleList.empty() || !wire_time(command->llTime)) return FALSE;
    const auto role = command->roleList.front().dwRoleID;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [command, role] {
        return CMessageHandler::GetInstance().DoFreezeRole(static_cast<UINT32>(command->llTime), role);
    });
}
BOOL CCmdManager::KickRole(CommandPtr input)
{
    auto command = typed<KICK_ROLE>(std::move(input), CMD_KICK_ROLE);
    if (!command || command->roleList.empty()) return FALSE;
    const auto role = command->roleList.front().dwRoleID;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [role] {
        return CMessageHandler::GetInstance().DoKickRole(role);
    });
}
BOOL CCmdManager::SendMail(CommandPtr input)
{
    auto command = typed<SEND_MAIL>(std::move(input), CMD_SEND_MAIL);
    if (!command || command->roleList.empty()) return FALSE;
    const auto role = command->roleList.front().dwRoleID;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [command, role] {
        return CMessageHandler::GetInstance().DoSendMail(role, command->szTitle, command->szContent,
            command->szReason, command->dwItemID, command->nItemNum);
    });
}
BOOL CCmdManager::SendAnnounce(INT32 group, UINT32 begin, INT32 interval, INT32 repeats, const char* text)
{
    auto command = std::static_pointer_cast<ANNOUNCE>(NewCmd(CMD_ANNOUNCE));
    if (!copy_field(command->szAnnounce, text)) return FALSE;
    command->nGroupID=group;command->dwBeginTime=begin;command->nIntervalTime=interval;command->nRepeatNum=repeats;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [command] {
        return CMessageHandler::GetInstance().DoAnnounce(command->nGroupID, command->dwBeginTime,
            command->nRepeatNum, command->nIntervalTime, command->szAnnounce);
    });
}
BOOL CCmdManager::QueryAnnounceList(INT32 group)
{
    auto command = std::static_pointer_cast<GET_ANNOUNCE_LIST>(NewCmd(CMD_GET_ANNOUNCE_LIST));
    command->nGroupID=group;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [group] {
        return CMessageHandler::GetInstance().DoGetAnnounceList(group);
    });
}
BOOL CCmdManager::DelAnnounce(INT32 group, UINT32 serial)
{
    auto command = std::static_pointer_cast<DEL_ANNOUNCE>(NewCmd(CMD_DEL_ANNOUNCE));
    command->nGroupID=group;command->dwAnnounceSerial=static_cast<UINT32>(serial);
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [command] {
        return CMessageHandler::GetInstance().DoDelAnnounce(command->nGroupID, command->dwAnnounceSerial);
    });
}
BOOL CCmdManager::QueryNameToID(std::map<INT32, ROLE_NAME_DATA>& names)
{
    if (names.empty() || names.size() > (std::numeric_limits<INT32>::max)() || names.begin()->first <= 0) return FALSE;
    for (const auto& [index, name] : names)
        if (!std::memchr(name.szRoleName, 0, sizeof(name.szRoleName))) return FALSE;
    auto command = std::static_pointer_cast<QUERY_NAME_TO_ID>(NewCmd(CMD_QUERY_NAME_TO_ID));
    command->RoleNameMap=names;command->current_index=names.begin()->first;command->nSvrNum=1;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [command] {
        return CMessageHandler::GetInstance().DoQueryNameToID(command->current_index,
            command->RoleNameMap.at(command->current_index).szRoleName);
    });
}
BOOL CCmdManager::SendGMStrCmd(const char* text)
{
    auto command = std::static_pointer_cast<EXECUTE_GM_STR>(NewCmd(CMD_EXECUTE_GM_STR));
    if (!copy_field(command->szStr, text)) return FALSE;
    return Submit(command, std::chrono::seconds(TL_CMD_TIMEOUT_TIME), [command] {
        return CMessageHandler::GetInstance().DoExecuteGMStrCmd(command->szStr);
    });
}
BOOL CCmdManager::LoadRoleData(CommandPtr input)
{
    auto command = typed<LOAD_ROLE_DATA>(std::move(input), CMD_LOAD_ROLE_DATA);
    if (!command) return FALSE;
    return Submit(command, std::chrono::seconds(TL_CMD_LONG_TIMEOUT), [command] {
        return CMessageHandler::GetInstance().DoLoadRoleData(command->dwRoleID);
    });
}
BOOL CCmdManager::ModifyRole(UINT32 role, UINT32 target, INT32 length, const char* bytes, const char* reason)
{
    if (!role || !bytes || length <= 0 || length > MAX_GM_ROLE_DATA_STR_LENGTH) return FALSE;
    auto command = std::static_pointer_cast<MODIFY_ROLE_DATA>(NewCmd(CMD_MODIFY_ROLE_DATA));
    if (!copy_field(command->szReason, reason)) return FALSE;
    command->dwRoleID=role;command->dwTargetRoleID=target;
    command->data.assign(bytes, bytes + length);
    return Submit(command, std::chrono::seconds(TL_CMD_LONG_TIMEOUT), [command] {
        return CMessageHandler::GetInstance().DoModifyRoleData(command->dwRoleID, command->dwTargetRoleID,
            static_cast<INT32>(command->data.size()), command->data.data(), command->szReason);
    });
}
BOOL CCmdManager::ExeRoleGMCmd(CommandPtr input)
{
    auto command = typed<EXE_ROLE_CMD>(std::move(input), CMD_EXE_ROLE_GM_CMD);
    if (!command) return FALSE;
    return Submit(command, std::chrono::seconds(TL_CMD_LONG_TIMEOUT), [command] {
        return CMessageHandler::GetInstance().DoExeRoleGMCmd(command->dwRoleID, command->szGM, command->szReason);
    });
}
BOOL CCmdManager::QueryLog(INT32 type, UINT32 begin, UINT32 end, const char* query)
{
    if (type<GM_LOG_QUERY_ROLE_LOGIN || type>=GM_QUERY_NUM || end<begin) return FALSE;
    auto command = std::static_pointer_cast<LOG_DATA>(NewCmd(CMD_LOG));
    if (!copy_field(command->szQueryStr, query)) return FALSE;
    command->nLogType=type;command->dwBeginTime=begin;command->dwEndTime=end;
    return Submit(command, std::chrono::seconds(TL_CMD_LONG_TIMEOUT), [command] {
        return CMessageHandler::GetInstance().DoLog(command->nLogType, command->dwBeginTime, command->dwEndTime, command->szQueryStr);
    });
}
BOOL CCmdManager::SetExeServerNum(INT32 count)
{
    const auto command=m_pCmd;
    if (!command || count < 0) return FALSE;
    command->nSvrNum=count;
    return TRUE;
}
BOOL CCmdManager::IsCmdExecuting(BOOL modifying)
{
    return static_cast<bool>(m_pCmd) || (!modifying && g_pRoleInfo);
}
void CCmdManager::CmdEnd() { DelCmd(m_pCmd); }
BOOL CCmdManager::DelCmd(CommandPtr command)
{
    if (!command) return FALSE;
    if (command == m_pCmd)
    {
        m_continuation.reset();
        m_pCmd.reset();
        m_IsCmdComplete=TRUE;
    }
    return TRUE;
}

BOOL CCmdManager::Success(CommandPtr pCmd)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pCmd && pCmd->Kind() == pCmd->nType);

    switch (pCmd->nType)
    {
    case CMD_QUERY_ROLE_ID:
    {
        nRetCode = CGMLog::GetInstance().Log("Query character ID command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_QUERY_ROLE_DATA:
    {
        nRetCode = CGMLog::GetInstance().Log("Query character information command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_FORBIDDEN_CHAT:
    {
        nRetCode = CGMLog::GetInstance().Log("Forbidden character chat command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_FREEZE_ROLE:
    {
        nRetCode = CGMLog::GetInstance().Log("Freeze character command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_KICK_ROLE:
    {
        nRetCode = CGMLog::GetInstance().Log("Kick character command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_ANNOUNCE:
    {
        nRetCode = CGMLog::GetInstance().Log("Publish announcement command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_SEND_MAIL:
    {
        nRetCode = CGMLog::GetInstance().Log("Send email command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_LOG:
    {
        nRetCode = CGMLog::GetInstance().Log("Query log command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_EXECUTE_GM_STR:
    {
        nRetCode = CGMLog::GetInstance().Log("Execute gm lua command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_LOAD_ROLE_DATA:
    {
        nRetCode = CGMLog::GetInstance().Log("Load role data command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_MODIFY_ROLE_DATA:
    {
        nRetCode = CGMLog::GetInstance().Log("Modify role data command executed successfully");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    default:
        break;
    }

    CCmdManager::GetInstance().DelCmd(pCmd);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCmdManager::Washout(CommandPtr pCmd)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pCmd && pCmd->Kind() == pCmd->nType);

    switch (pCmd->nType)
    {
    case CMD_QUERY_ROLE_ID:
    {
        nRetCode = CGMLog::GetInstance().Log("Query role ID command timed out, some servers have not completed execution");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_QUERY_ROLE_DATA:
    {
        QUERY_ROLE_DATA *pQuery = static_cast<QUERY_ROLE_DATA*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Query role data command timed out");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_FORBIDDEN_CHAT:
    {
        FORBIDDEN_CHAT *pForbidChat = static_cast<FORBIDDEN_CHAT*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Forbidden character chat command timed out");
        LOG_CHECK_ERROR(nRetCode);

    }
        break;
    case CMD_FREEZE_ROLE:
    {
        FREEZE_ROLE *pFreezeRole = static_cast<FREEZE_ROLE*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Freeze role command timed out");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_KICK_ROLE:
    {
        KICK_ROLE *pKickRole = static_cast<KICK_ROLE*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Kick role command timed out");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_ANNOUNCE:
    {
        ANNOUNCE *pAnnounce = static_cast<ANNOUNCE*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Announcement command timed out. The command was not executed on [{}] remaining servers.", pAnnounce->nSvrNum);
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_SEND_MAIL:
    {
        SEND_MAIL   *pSendMail = static_cast<SEND_MAIL*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Send mail command timed out");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_LOG:
    {
        nRetCode = CGMLog::GetInstance().Log("Query log command timed out");
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_EXECUTE_GM_STR:
    {
        EXECUTE_GM_STR *pExeStrCmd = static_cast<EXECUTE_GM_STR*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Execute gm lua command timed out. The command was not executed on [{}] remaining servers.", pExeStrCmd->nSvrNum);
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_LOAD_ROLE_DATA:
    {
        LOAD_ROLE_DATA *pLoadRoleCmd = static_cast<LOAD_ROLE_DATA*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Load role [{}] data timed out", pLoadRoleCmd->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    case CMD_MODIFY_ROLE_DATA:
    {
        MODIFY_ROLE_DATA *pModifyRoleCmd = static_cast<MODIFY_ROLE_DATA*>(pCmd.get());
        nRetCode = CGMLog::GetInstance().Log("Modify role data type [{}] timed out", pModifyRoleCmd->nType);
        LOG_CHECK_ERROR(nRetCode);
    }
        break;
    default:
        break;
    }

    CCmdManager::GetInstance().DelCmd(pCmd);

    return TRUE;
Exit0:
    nRetCode = CGMLog::GetInstance().Log("Invalid command!");
    LOG_CHECK_ERROR(nRetCode);
    return FALSE;
}

CommandPtr CCmdManager::NewCmd(INT32 type)
{
    CommandPtr command;
    switch (type)
    {
    case CMD_PRE_QUERY: command=std::make_shared<PRE_QUERY>();break;
    case CMD_QUERY_ROLE_ID: command=std::make_shared<QUERY_ROLE_ID>();break;
    case CMD_QUERY_ROLE_DATA: command=std::make_shared<QUERY_ROLE_DATA>();break;
    case CMD_FORBIDDEN_CHAT: command=std::make_shared<FORBIDDEN_CHAT>();break;
    case CMD_FREEZE_ROLE: command=std::make_shared<FREEZE_ROLE>();break;
    case CMD_KICK_ROLE: command=std::make_shared<KICK_ROLE>();break;
    case CMD_ANNOUNCE: command=std::make_shared<ANNOUNCE>();break;
    case CMD_GET_ANNOUNCE_LIST: command=std::make_shared<GET_ANNOUNCE_LIST>();break;
    case CMD_DEL_ANNOUNCE: command=std::make_shared<DEL_ANNOUNCE>();break;
    case CMD_SEND_MAIL: command=std::make_shared<SEND_MAIL>();break;
    case CMD_LOG: command=std::make_shared<LOG_DATA>();break;
    case CMD_EXECUTE_GM_STR: command=std::make_shared<EXECUTE_GM_STR>();break;
    case CMD_LOAD_ROLE_DATA: command=std::make_shared<LOAD_ROLE_DATA>();break;
    case CMD_MODIFY_ROLE_DATA: command=std::make_shared<MODIFY_ROLE_DATA>();break;
    case CMD_QUERY_NAME_TO_ID: command=std::make_shared<QUERY_NAME_TO_ID>();break;
    case CMD_EXE_ROLE_GM_CMD: command=std::make_shared<EXE_ROLE_CMD>();break;
    default: return {};
    }
    command->nType=type;
    command->allocation_kind_=type;
    return command;
}

BOOL CCmdManager::AddSvr(UINT32 server)
{
    m_PreExecuteSvrList.push_back(server);
    return TRUE;
}
UINT32 CCmdManager::First()
{
    m_Cursor=0;
    return m_PreExecuteSvrList.empty() ? UINT32(-1) : m_PreExecuteSvrList.front();
}
UINT32 CCmdManager::Next()
{
    if (m_Cursor < m_PreExecuteSvrList.size()) ++m_Cursor;
    return m_Cursor < m_PreExecuteSvrList.size() ? m_PreExecuteSvrList[m_Cursor] : UINT32(-1);
}
BOOL CCmdManager::IsCompleteAll() { return m_IsCmdComplete; }
