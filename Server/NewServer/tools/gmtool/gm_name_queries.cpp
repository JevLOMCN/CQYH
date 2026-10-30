#include "cmd_manager.h"
#include "gm_reports.h"
#include <iterator>

std::expected<void, std::string> CCmdManager::ReceiveNameToID(UINT32 session, INT32 index, UINT32 role,
    const std::filesystem::path& report)
{
    const auto active = m_pCmd;
    if (!m_initialized || !active || active->Kind() != CMD_QUERY_NAME_TO_ID || active->nType != CMD_QUERY_NAME_TO_ID)
        return std::unexpected("no matching name query");
    auto query = std::static_pointer_cast<QUERY_NAME_TO_ID>(active);
    if (query->pending_next || query->nSvrNum != 1 || query->session_id != session || query->current_index != index)
        return std::unexpected("unexpected name-query reply");
    const auto current = query->RoleNameMap.find(index);
    if (current == query->RoleNameMap.end()) return std::unexpected("unknown name-query index");
    current->second.dwRoleID = role;
    const auto next = std::next(current);
    query->nSvrNum = 0;
    if (next != query->RoleNameMap.end())
    {
        query->current_index = next->first;
        query->pending_next = true;
        // MainLoop sends only after all issuing frames have returned.
        return {};
    }
    auto names = std::move(query->RoleNameMap);
    DelCmd(active);
    return gm::write_name_report(report, names);
}
