#include "cmd_manager.h"
#include "gm_reports.h"
#include <algorithm>
#include <cstring>
#include <iterator>

namespace
{
template<std::size_t N> std::optional<std::string_view> text(const char (&value)[N])
{
    const auto* end = static_cast<const char*>(std::memchr(value, 0, N));
    if (!end) return {};
    return std::string_view(value, static_cast<std::size_t>(end - value));
}
BOOL issue(CCmdManager& manager, CommandPtr command)
{
    switch (command->Kind())
    {
    case CMD_QUERY_ROLE_DATA: return manager.QueryRoleData(command);
    case CMD_LOAD_ROLE_DATA: return manager.LoadRoleData(command);
    case CMD_EXE_ROLE_GM_CMD: return manager.ExeRoleGMCmd(command);
    case CMD_FORBIDDEN_CHAT: return manager.ForbidChat(command);
    case CMD_FREEZE_ROLE: return manager.FreezeRole(command);
    case CMD_KICK_ROLE: return manager.KickRole(command);
    case CMD_SEND_MAIL: return manager.SendMail(command);
    default: return FALSE;
    }
}
template<class T> bool assign_role(const CommandPtr& command, std::string_view name, UINT32 role)
{
    auto& value = *std::static_pointer_cast<T>(command);
    if constexpr (requires { value.roleList; })
    {
        if (value.roleList.empty() || text(value.roleList.front().szRoleName) != name) return false;
        value.roleList.front().dwRoleID = role;
    }
    else
    {
        if (text(value.szRoleName) != name) return false;
        value.dwRoleID = role;
    }
    return true;
}
}

std::expected<void, std::string> CCmdManager::ReceivePreQuery(UINT32 session, UINT32 role, std::string_view name)
{
    const auto active = m_pCmd;
    if (!m_initialized || !active || active->Kind() != CMD_PRE_QUERY || active->nType != CMD_PRE_QUERY ||
        active->nSvrNum != 1 || active->reply_peer != session)
        return std::unexpected("unexpected role-name reply");
    auto query = std::static_pointer_cast<PRE_QUERY>(active);
    if (text(query->szRoleName) != name || !query->pCmd || query->pCmd->Kind() != query->nCmdType ||
        query->pCmd->nType != query->nCmdType)
        return std::unexpected("role-name reply does not match its continuation");
    if (!role)
    {
        DelCmd(active);
        return std::unexpected("role not found");
    }
    const auto target = query->pCmd;
    Continuation continuation{active, [this, target] { return issue(*this, target); }};
    bool assigned = false;
    switch (target->Kind())
    {
    case CMD_QUERY_ROLE_DATA: assigned=assign_role<QUERY_ROLE_DATA>(target,name,role); break;
    case CMD_LOAD_ROLE_DATA: assigned=assign_role<LOAD_ROLE_DATA>(target,name,role); break;
    case CMD_EXE_ROLE_GM_CMD: assigned=assign_role<EXE_ROLE_CMD>(target,name,role); break;
    case CMD_FORBIDDEN_CHAT: assigned=assign_role<FORBIDDEN_CHAT>(target,name,role); break;
    case CMD_FREEZE_ROLE: assigned=assign_role<FREEZE_ROLE>(target,name,role); break;
    case CMD_KICK_ROLE: assigned=assign_role<KICK_ROLE>(target,name,role); break;
    case CMD_SEND_MAIL: assigned=assign_role<SEND_MAIL>(target,name,role); break;
    default: break;
    }
    if (!assigned) return std::unexpected("invalid or empty role continuation");
    target->reply_peer = session;
    target->reply_connection_lifetime = active->reply_connection_lifetime;
    active->nSvrNum = 0;
    m_continuation.emplace(std::move(continuation));
    return {};
}

std::expected<void, std::string> CCmdManager::ReceiveBulkReply(INT32 kind, UINT32 session, UINT32 role, INT32 result)
{
    const auto active = m_pCmd;
    if (!m_initialized || !active || active->Kind() != kind || active->nType != kind ||
        active->nSvrNum != 1 || active->reply_peer != session)
        return std::unexpected("unexpected bulk-role reply");
    const auto consume = [&]<class T>() -> std::expected<void, std::string>
    {
        auto command = std::static_pointer_cast<T>(active);
        if (command->roleList.empty() || command->roleList.front().dwRoleID != role)
            return std::unexpected("bulk-role reply does not match outstanding role");
        const auto path=command->report_path;
        if (path.native().find(std::filesystem::path::value_type{})!=std::filesystem::path::string_type::npos)
            return std::unexpected("invalid bulk-role report path");
        std::optional<Continuation> continuation;
        if (command->roleList.size() > 1)
        {
            const auto name = text(std::next(command->roleList.begin())->szRoleName);
            if (!name) return std::unexpected("invalid next role name");
            continuation.emplace(Continuation{active, [this, active, name=std::string(*name), kind]
            {
                return PreQuery(kind, name.c_str(), active);
            }});
        }
        auto row = command->roleList.front();
        row.nResult = result;
        command->roleResList.push_back(row);
        command->roleList.pop_front();
        active->nSvrNum = 0;
        if (continuation)
        {
            m_continuation = std::move(continuation);
            return {};
        }
        auto rows = std::move(command->roleResList);
        DelCmd(active);
        return gm::write_role_report(path, kind, rows);
    };
    switch (kind)
    {
    case CMD_FORBIDDEN_CHAT: return consume.operator()<FORBIDDEN_CHAT>();
    case CMD_FREEZE_ROLE: return consume.operator()<FREEZE_ROLE>();
    case CMD_KICK_ROLE: return consume.operator()<KICK_ROLE>();
    case CMD_SEND_MAIL: return consume.operator()<SEND_MAIL>();
    default: return std::unexpected("unsupported bulk-role reply type");
    }
}
