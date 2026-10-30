#include "message_handler.h"
#include "gm_packet.h"
#include "process_error.h"
#include "session_mgr.h"
#include "server_mgr.h"
#include "cmd_manager.h"
#include "gm_def.h"
#include "gm_version.h"
#include "common_message.h"
#include "gmtl_message.h"
#include "log.h"
#include <algorithm>
#include <cstring>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

using namespace InternalMessage;

namespace
{
template<class T> T message(UINT16 id)
{
    static_assert(std::is_trivially_copyable_v<T>);
    T value;
    std::memset(&value, 0, sizeof(value));
    value.wMsg = id;
    return value;
}
template<class T> std::span<const char> bytes_of(const T& value)
{
    return {reinterpret_cast<const char*>(&value), sizeof(value)};
}
template<std::size_t N> bool copy_text(char (&destination)[N], const char* source)
{
    if (!source) return false;
    const std::string_view text(source);
    if (text.size() >= N) return false;
    std::ranges::copy(text, destination);
    destination[text.size()] = '\0';
    return true;
}
template<class T> std::vector<char> text_message(const T& header, std::string_view text)
{
    if (text.size() >= MAX_MESSAGE_SIZE - sizeof(T)) return {};
    std::vector<char> bytes(sizeof(T) + text.size() + 1);
    std::memcpy(bytes.data(), &header, sizeof(T));
    std::ranges::copy(text, bytes.begin() + sizeof(T));
    return bytes;
}
bool group(INT32 value) { return value >= 0 && value < MAX_SERVER_GROUP_COUNT; }
bool role_chain(INT32 kind)
{
    switch (kind)
    {
    case CMD_PRE_QUERY: case CMD_QUERY_ROLE_DATA: case CMD_LOAD_ROLE_DATA: case CMD_EXE_ROLE_GM_CMD:
    case CMD_FORBIDDEN_CHAT: case CMD_FREEZE_ROLE: case CMD_KICK_ROLE: case CMD_SEND_MAIL:
    case CMD_QUERY_NAME_TO_ID: return true;
    default: return false;
    }
}

struct Issuing
{
    CommandPtr command;
    explicit Issuing(CommandPtr value) : command(std::move(value)) { if (command) ++command->in_flight; }
    ~Issuing() { if (command) --command->in_flight; }
};
}

BOOL CMessageHandler::_SendGM(UINT32 id, std::span<const char> bytes)
{
    if (!m_accepting || !m_pPacketAllocator) return FALSE;
    const auto generation = m_generation;
    const auto lifetime=m_lifetime;
    const auto command = CCmdManager::GetInstance().GetCurrentCmd();
    const auto binding=gm::connection_binding(id);
    if (!binding) return FALSE;
    auto packet = gm::copy_packet(m_pPacketAllocator, bytes,lifetime);
    if (!packet || !m_accepting || m_generation != generation || CCmdManager::GetInstance().GetCurrentCmd() != command) return FALSE;
    if (!binding->Current()) return FALSE;
    if (binding->socket->Send(packet.get(), static_cast<INT32>(bytes.size()), 0) != bytes.size()) return FALSE;
    packet.release(); // Successful native Send consumes the buffer.
    return TRUE;
}

BOOL CMessageHandler::_BroadcastGM(std::span<const char> bytes)
{
    if (!m_accepting || !m_pPacketAllocator || bytes.empty() || bytes.size() > MAX_MESSAGE_SIZE) return FALSE;
    const auto generation = m_generation;
    auto* const allocator = m_pPacketAllocator;
    const auto lifetime=m_lifetime;
    auto& manager = CCmdManager::GetInstance();
    const auto command = manager.GetCurrentCmd();
    if (command && command->reply_deliveries) return FALSE;
    Issuing issuing(command);
    struct Pending
    {
        UINT32 id;
        ISocketSession* socket;
        gm::Packet packet;
        gm::ConnectionBinding binding;
    };
    std::vector<Pending> pending;
    auto& selection = CServerMgr::GetInstance();
    const bool role_request = command && role_chain(command->Kind());
    const bool retained_peer = role_request && command->reply_peer != UINT32(-1);
    const auto count = retained_peer ? 1 : selection.GetSelectedNum();
    if (count <= 0 || count > MAX_GM_SESSION_COUNT) return FALSE;
    if (command && command->Kind() == CMD_QUERY_NAME_TO_ID && count != 1) return FALSE;
    if (command && command->Kind() == CMD_LOG && count != 1) return FALSE;
    if (command && (command->Kind() == CMD_GET_ANNOUNCE_LIST || command->Kind() == CMD_DEL_ANNOUNCE) && count != 1) return FALSE;
    if (role_request && count != 1) return FALSE;
    pending.reserve(count);
    // Snapshot and validate selection before allocations or native callbacks.
    for (INT32 index = 0; index < count; ++index)
    {
        const auto id = retained_peer ? command->reply_peer : selection.GetSessionIDByIndex(index);
        const auto* session = CSessionMgr::GetInstance().GetGMSession(id);
        if (!session || !session->pSocketSession || session->nConnectState != SESSION_CONNECTED ||
            std::ranges::any_of(pending, [id](const auto& value) { return value.id == id; })) return FALSE;
        const auto binding=gm::connection_binding(id);
        if (!binding || (retained_peer && command->reply_connection_lifetime!=binding->lifetime)) return FALSE;
        pending.push_back({id, session->pSocketSession, gm::Packet(nullptr, gm::PacketDeleter{allocator,lifetime}), *binding});
    }
    for (auto& value : pending)
    {
        value.packet = gm::copy_packet(allocator, bytes,lifetime);
        if (!value.packet) return FALSE;
    }
    if (!m_accepting || m_generation != generation || manager.GetCurrentCmd() != command) return FALSE;
    if (command)
    {
        std::map<UINT32, CMD_BASE::ReplyPeer> replies;
        for (const auto& peer : pending)
        {
            CMD_BASE::ReplyPeer reply{peer.socket,peer.binding.generation};
            reply.connection_lifetime=peer.binding.lifetime;
            replies.emplace(peer.id,std::move(reply));
        }
        command->replies.swap(replies);
        command->nSvrNum = count;
    }
    if (role_request)
    {
        command->reply_peer = pending.front().id;
        command->reply_connection_lifetime=pending.front().binding.lifetime;
    }
    if (command && command->Kind()==CMD_LOG)
    {
        command->reply_peer=pending.front().id;
        std::static_pointer_cast<LOG_DATA>(command)->reply_socket=pending.front().socket;
    }
    if (command && command->Kind() == CMD_QUERY_NAME_TO_ID)
        std::static_pointer_cast<QUERY_NAME_TO_ID>(command)->session_id = pending.front().id;
    std::size_t accepted = 0;
    std::size_t index = 0;
    const auto discard_peer = [&](UINT32 id)
    {
        if (!command) return;
        auto& reply = command->replies.at(id);
        if (!reply.done) { reply.done = true; command->nSvrNum = std::max(0, command->nSvrNum - 1); }
    };
    const auto discard_remaining = [&]
    {
        for (auto remaining = index; remaining < pending.size(); ++remaining) discard_peer(pending[remaining].id);
    };
    try
    {
        for (; index < pending.size(); ++index)
        {
            if (!m_accepting || m_generation != generation || manager.GetCurrentCmd() != command)
            {
                discard_remaining();
                break;
            }
            auto& value = pending[index];
            if (command) command->replies.at(value.id).issued = true;
            if (!value.binding.Current() ||
                value.socket->Send(value.packet.get(), static_cast<INT32>(bytes.size()), 0) != bytes.size())
            {
                discard_peer(value.id);
                WRN("GM send failed for session %u", value.id);
                continue;
            }
            value.packet.release();
            ++accepted;
        }
    }
    catch (...)
    {
        discard_remaining();
        throw;
    }
    // Partial broadcasts wait only for accepted sends. They are never retried
    // automatically: this protocol has no request IDs for deduplication.
    return accepted != 0;
}

BOOL CMessageHandler::DoAllPing()
{
    const auto lifetime=m_lifetime;
    if (!m_accepting) return FALSE;
    const auto generation = m_generation;
    bool sent = true;
    const auto value = message<A2A_PING>(a2a_ping);
    for (UINT32 id = 0; id < MAX_GM_SESSION_COUNT; ++id)
    {
        if (!m_accepting || m_generation != generation) return FALSE;
        const auto* session = CSessionMgr::GetInstance().GetGMSession(id);
        if (!session || !session->pSocketSession || session->nConnectState != SESSION_CONNECTED) continue;
        if (!_SendGM(id, bytes_of(value))) sent = false;
    }
    return sent;
}

BOOL CMessageHandler::DoRegisterToGM(UINT32 id)
{
    const auto lifetime=m_lifetime;
    if (!m_accepting) return FALSE;
    const auto generation = m_generation;
    const auto* session = CSessionMgr::GetInstance().GetGMSession(id);
    if (!session || !session->pSocketSession) return FALSE;
    auto value = message<TL2GM_REGISTER>(tl2gm_register);
    if (!session->pSocketSession->GetAddress(value.dwIP, value.wPort)) return FALSE;
    if (gethostname(value.szComputerName, sizeof(value.szComputerName)) != 0) return FALSE;
    value.szComputerName[sizeof(value.szComputerName) - 1] = '\0';
    if (m_generation != generation) return FALSE;
    return _SendGM(id, bytes_of(value));
}

BOOL CMessageHandler::DoVerify(UINT32 id, const char* name, const char* password)
{
    auto value = message<TL2GM_VERIFY>(tl2gm_verify);
    if (!copy_text(value.szName, name) || !copy_text(value.szPwd, password)) return FALSE;
    return _SendGM(id, bytes_of(value));
}

BOOL CMessageHandler::DoLoginToGM(UINT32 id, const char* name, const char* password)
{
    auto value = message<TL2GM_LOGIN>(tl2gm_login);
    value.nVersion = GM_MAJOR_VERSION;
    value.nDependVersion = GM_DEPEND_VERSION;
    if (!copy_text(value.szName, name) || !copy_text(value.szPwd, password)) return FALSE;
    return _SendGM(id, bytes_of(value));
}

BOOL CMessageHandler::DoLogoutToGM(UINT32 id)
{
    return _SendGM(id, bytes_of(message<TL2GM_LOGOUT>(tl2gm_logout)));
}

BOOL CMessageHandler::DoReleaseRole(UINT32 session, UINT32 role)
{
    auto value = message<TL2GM_RELEASE_ROLE>(tl2gm_release_role);
    value.dwRoleID = role;
    return _SendGM(session, bytes_of(value));
}

BOOL CMessageHandler::DoAnnounce(INT32 group_id, UINT32 begin, INT32 repeats, INT32 interval, const char* text)
{
    if (!text || !group(group_id)) return FALSE;
    auto value = message<TL2GM_ANNOUNCE>(tl2gm_announce);
    value.nGroupID = group_id;
    value.dwBeginTime = begin;
    value.nRepeatNum = repeats;
    value.dwIntervalTime = interval;
    const auto bytes = text_message(value, text);
    return _BroadcastGM(bytes);
}

BOOL CMessageHandler::DoLog(INT32 type, UINT32 begin, UINT32 end, const char* query)
{
    if (!query || type<GM_LOG_QUERY_ROLE_LOGIN || type>=GM_QUERY_NUM || end<begin) return FALSE;
    auto value = message<TL2GM_LOG>(tl2gm_log);
    value.nLogType = type;
    value.dwBeginTime = begin;
    value.dwEndTime = end;
    const auto bytes = text_message(value, query);
    return _BroadcastGM(bytes);
}

BOOL CMessageHandler::DoExecuteGMStrCmd(const char* text)
{
    if (!text) return FALSE;
    const auto bytes = text_message(message<TL2GM_EXE_GM_STR>(tl2gm_exe_gm_str), text);
    return _BroadcastGM(bytes);
}

BOOL CMessageHandler::DoModifyRoleData(UINT32 role, UINT32 target, INT32 length, const char* data, const char* reason)
{
    if (!role || !data || length <= 0 || length > MAX_GM_ROLE_DATA_STR_LENGTH) return FALSE;
    auto value = message<TL2GM_MODIFY_ROLE_DATA>(tl2gm_modify_role_data);
    if (!copy_text(value.szReason, reason)) return FALSE;
    value.dwRoleID = role;
    value.dwTargetRoleID = target;
    value.nLength = length;
    std::vector<char> bytes(sizeof(value) + length);
    std::memcpy(bytes.data(), &value, sizeof(value));
    std::memcpy(bytes.data() + sizeof(value), data, length);
    return _BroadcastGM(bytes);
}

BOOL CMessageHandler::DoPreQuery(const char *pcszRoleName)
{
    auto value = message<TL2GM_PRE_QUERY>(tl2gm_pre_query);
    if (!copy_text(value.szRoleName, pcszRoleName)) return FALSE;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoQueryRoleID(INT32 nQueryType, const char *pcszName)
{
    auto value = message<TL2GM_QUERY_ROLE_ID>(tl2gm_query_role_id);
    value.nQueryType = nQueryType;
    if (!copy_text(value.szName, pcszName)) return FALSE;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoQueryRoleData(UINT32 dwRoleID)
{
    auto value = message<TL2GM_QUERY_ROLE_DATA>(tl2gm_query_role_data);
    value.dwRoleID = dwRoleID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoForbidChat(UINT32 dwTime, UINT32 dwRoleID)
{
    auto value = message<TL2GM_FORBID_CHAT>(tl2gm_forbid_chat);
    value.dwTime = dwTime;
    value.dwRoleID = dwRoleID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoFreezeRole(UINT32 dwTime, UINT32 dwRoleID)
{
    auto value = message<TL2GM_FREEZE_ROLE>(tl2gm_freeze_role);
    value.dwTime = dwTime;
    value.dwRoleID = dwRoleID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoKickRole(UINT32 dwRoleID)
{
    auto value = message<TL2GM_KICK_ROLE>(tl2gm_kick_role);
    value.dwRoleID = dwRoleID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoSendMail(UINT32 dwRoleID, const char *pcszSubject, const char *pcszContent, const char *pcszReason, UINT32 dwItemID, INT32 nItemNum)
{
    auto value = message<TL2GM_SEND_MAIL>(tl2gm_send_mail);
    value.dwItemID = dwItemID;
    value.nItemNum = nItemNum;
    value.dwRoleID = dwRoleID;
    if (!copy_text(value.szSubject, pcszSubject)) return FALSE;
    if (!copy_text(value.szContent, pcszContent)) return FALSE;
    if (!copy_text(value.szReason, pcszReason)) return FALSE;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoGetAnnounceList(INT32 nGroupID)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_GET_ANNOUNCE_LIST>(tl2gm_get_announce_list);
    value.nGroupID = nGroupID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoDelAnnounce(INT32 nGroupID, UINT32 dwAnnounceSerial)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_DEL_ANNOUNCE>(tl2gm_del_announce);
    value.nGroupID = nGroupID;
    value.dwAnnounceSerial = dwAnnounceSerial;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoQueryNameToID(INT32 nIndex, const char *pcszRoleName)
{
    auto value = message<TL2GM_QUERY_NAME_TO_ID>(tl2gm_query_name_to_id);
    value.nIndex = nIndex;
    if (!copy_text(value.szRoleName, pcszRoleName)) return FALSE;
    // Every row uses the retained peer and refreshes the same reply ledger.
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoLoadRoleData(UINT32 dwRoleID)
{
    auto value = message<TL2GM_LOAD_ROLE_DATA>(tl2gm_load_role_data);
    value.dwRoleID = dwRoleID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoExeRoleGMCmd(UINT32 dwRoleID, const char *pcszGM, const char *pcszReason)
{
    auto value = message<TL2GM_ROLE_EXE_CMD>(tl2gm_role_exe_cmd);
    value.dwRoleID = dwRoleID;
    if (!copy_text(value.szReason, pcszReason)) return FALSE;
    if (!copy_text(value.szGM, pcszGM)) return FALSE;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoSelectGroup(INT32 nGroupID)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_SELECT_GROUP>(tl2gm_select_group);
    value.byGroupID = nGroupID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoQueryExchangeLog(INT32 nGroupID)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_EXCHANGE_PRICE>(tl2gm_exchange_price);
    value.byGroupID = nGroupID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoQueryExchangeHistory(INT32 nGroupID)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_EXCHANGE_HISTORY>(tl2gm_exchange_history);
    value.byGroupID = nGroupID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoQueryOrderDetail(INT32 nGroupID, UINT8 byOrderType, INT32 nPrice)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_ORDER_DETAIL>(tl2gm_order_detail);
    value.byGroupID = nGroupID;
    value.nPrice = nPrice;
    value.byOrderType = byOrderType;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoAddSystemOrder( INT32 nGroupID, INT32 nPrice, INT32 nCount)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_ADD_SYSTEM_ORDER>(tl2gm_add_system_order);
    value.byGroupID = nGroupID;
    value.nPrice = nPrice;
    value.nCount = nCount;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoDelSystemOrder(INT32 nGroupID, UINT64 qwOrderID)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_DEL_SYSTEM_ORDER>(tl2gm_del_system_order);
    value.byGroupID = nGroupID;
    value.qwOrderID = qwOrderID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoQuerySysExchangeInfo(INT32 nGroupID, INT32 nFlag, UINT32 dwBeginToday, UINT32 dwEndToday, UINT32 dwBeginHour, UINT32 dwEndHour)
{
    if (!group(nGroupID)) return FALSE;
    if (nFlag < 0 || nFlag > 255) return FALSE;
    auto value = message<TL2GM_SYS_EXCHANGE_INFO>(tl2gm_sys_exchange_info);
    value.byGroupID = nGroupID;
    value.byFlag = nFlag;
    value.dwBeginDayTime = dwBeginToday;
    value.dwEndDayTime = dwEndToday;
    value.dwBeginHourTime = dwBeginHour;
    value.dwEndHourTime = dwEndHour;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoQueryExchangeLimit(INT32 nGroupID)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_GET_EXCHANGE_LIMIT>(tl2gm_get_exchange_limit);
    value.byGroupID = (UINT8)nGroupID;
    return _BroadcastGM(bytes_of(value));
}

BOOL CMessageHandler::DoSetExchangeLimit(INT32 nGroupID, INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit)
{
    if (!group(nGroupID)) return FALSE;
    auto value = message<TL2GM_SET_EXCHANGE_LIMIT>(tl2gm_set_exchange_limit);
    value.byGroupID = nGroupID;
    value.nPriceHigh = nPriceHigh;
    value.nPriceLow = nPriceLow;
    value.nSingleOrderLimit = nSingleOrderLimit;
    value.nDailyOrderLimit = nDailyOrderLimit;
    return _BroadcastGM(bytes_of(value));
}
