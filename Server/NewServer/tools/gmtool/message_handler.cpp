/********************************************************************
	created:	6:8:2013   13:33
	file:		message_handler.cpp
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "message_handler.h"
#include "gm_session_events.h"
#include "gm_exchange_replies.h"
#include <string>
#include <QtCore/QPointer>
#include "session_mgr.h"
#include "tl_config.h"

#include "common_message.h"
#include "gmtl_message.h"
#include "string_ex.h"
#include "server_mgr.h"
#include "cmd_manager.h"
#include "base_role.h"
#include "gmtool.h"
#include "loader.h"
#include "gm_manager.h"
#include "data_mgr.h"
#include "gm_data_packets.h"
#include "gm_command_reply.h"
#include "gm_role_data.h"
#include "gm_receive_packets.h"
#include <vector>
#include "gm_log.h"
#include "gm_def.h"
#include "str_def.h"
#include <QtWidgets/QStatusBar>
#include "server_list.h"
#include "role_info.h"
#include "logic.h"
#include "gm_version.h"
#include "ld_widget.h"


using namespace InternalMessage;


void CMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
    LOG_PROCESS_ERROR(pSocketSession);
    LOG_PROCESS_ERROR(pSocketAcceptor);

Exit0:
    return ;
}

void gm::session_disconnected(const GM_SESSION& before,UINT64 generation)
{
    auto& sessions=CSessionMgr::GetInstance();
    if(sessions.Generation()!=generation) return;
    if(before.eState!=GM_SVR_STATE_DISCONNECT && before.eState!=GM_SVR_STATE_NONE)
    {
        CGMLog::GetInstance().Log("Server [{}] disconnected!",QString::fromUtf8(before.name.c_str()).toLocal8Bit().constData());
        if(sessions.Generation()!=generation) return;
        const auto* current=sessions.GetGMSession(before.dwSessionID);
        if(!current || current->pSocketSession || current->eState!=GM_SVR_STATE_DISCONNECT) return;
        gm::main_window().UpdateSvrNameColor(before.dwSessionID,GM_SVR_STATE_DISCONNECT);
    }
    if(sessions.Generation()==generation) CServerMgr::GetInstance().UpdateSelectServer();
}

namespace
{
template<class T> std::optional<T> fixed_reply(PACKET_BUFFER* packet, UINT32 size, UINT16 message)
{
    if (!packet || size < sizeof(T) || !gm::valid_receive_packet({packet, size})) return {};
    T result;
    std::memcpy(&result, packet, sizeof(result));
    if (result.wMsg != message) return {};
    return result;
}
bool current_session(const GM_SESSION& snapshot, ISocketSession* socket, UINT64 generation)
{
    auto& sessions = CSessionMgr::GetInstance();
    const auto* current = sessions.FromSocket(socket);
    return sessions.Generation() == generation && current && current->dwType == stGM &&
        current->nConnectState == SESSION_CONNECTED && current->dwSessionID == snapshot.dwSessionID &&
        current->nSvrID == snapshot.nSvrID && snapshot.connection_lifetime &&
        current->connection_lifetime == snapshot.connection_lifetime;
}
}

void CMessageHandler::OnGMRegisterAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_REGISTER_ACK>(packet, size, gm2tl_register_ack);
    auto& sessions = CSessionMgr::GetInstance();
    auto* session = sessions.FromSocket(socket);
    if (!reply || !session || session->dwType != stGM || session->nConnectState != SESSION_CONNECTED ||
        (session->eState != GM_SVR_STATE_NONE && session->eState != GM_SVR_STATE_DISCONNECT)) return;
    const auto before = *session;
    const auto generation = sessions.Generation();
    const QPointer<gmtool> window(gm::current_main_window());
    if (reply->nResult == errSuccess)
    {
        session->eState = GM_SVR_STATE_REGISTERED;
        if (window) window->UpdateSvrNameColor(before.dwSessionID, GM_SVR_STATE_REGISTERED);
    }
    else
    {
        CGMLog::GetInstance().Log("Failed to register with server [{}]", QString::fromUtf8(before.name.c_str()).toLocal8Bit().constData());
        if (current_session(before, socket, generation)) sessions.CloseSession(socket, false);
    }
}

void CMessageHandler::OnGMVerifyAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_VERIFY_ACK>(packet, size, gm2tl_verify_ack);
    const auto* session = CSessionMgr::GetInstance().FromSocket(socket);
    if (!reply || !session || session->dwType != stGM || session->nConnectState != SESSION_CONNECTED ||
        session->eState != GM_SVR_STATE_REGISTERED) return;
    if (auto* login = gm::current_login_dialog()) login->VerifyUser(reply->nResult, reply->bLD, socket);
}

void CMessageHandler::OnGMLoginAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_LOGIN_ACK>(packet, size, gm2tl_login_ack);
    auto& sessions = CSessionMgr::GetInstance();
    auto* session = sessions.FromSocket(socket);
    if (!reply || !session || session->dwType != stGM || session->nConnectState != SESSION_CONNECTED ||
        session->eState != GM_SVR_STATE_REGISTERED) return;
    const auto before = *session;
    const auto generation = sessions.Generation();
    const QPointer<gmtool> window(gm::current_main_window());
    if (reply->nResult != errSuccess)
    {
        CGMLog::GetInstance().Log("Failed to login to server [{}], errCode [{}]",
            QString::fromUtf8(before.name.c_str()).toLocal8Bit().constData(), reply->nResult);
        return;
    }
    session->eState = GM_SVR_STATE_LOGIN;
    if (window) window->UpdateSvrNameColor(before.dwSessionID, GM_SVR_STATE_LOGIN);
    if (!current_session(before, socket, generation)) return;
    CGMLog::GetInstance().Log("Successfully logged in to server [{}]", QString::fromUtf8(before.name.c_str()).toLocal8Bit().constData());
    if (!window || !current_session(before, socket, generation)) return;
    CServerMgr::GetInstance().UpdateSelectServer();
    if (window && current_session(before, socket, generation)) window->ServerLogin(before.nSvrID);
}

void CMessageHandler::OnGMItemListNtf(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    if (!packet || !socket)
    {
        LOG_CHECK_ERROR(FALSE);
        return;
    }
    const auto* session = CSessionMgr::GetInstance().FromSocket(socket);
    if (!session || session->nConnectState != SESSION_CONNECTED)
    {
        LOG_CHECK_ERROR(FALSE);
        return;
    }
    auto batch = gm::decode_item_batch({reinterpret_cast<const std::byte*>(packet), size});
    if (!batch)
    {
        CRI("GM item list: %s", batch.error().c_str());
        return;
    }
    const auto applied = CDataMgr::GetInstance().ApplyItems(session->dwSessionID, batch->clear, batch->items);
    LOG_CHECK_ERROR(applied);
}

void CMessageHandler::OnGMLogoutAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_LOGOUT_ACK>(packet, size, gm2tl_logout_ack);
    auto& sessions = CSessionMgr::GetInstance();
    auto* session = sessions.FromSocket(socket);
    if (!reply || !session || session->dwType != stGM || session->nConnectState != SESSION_CONNECTED ||
        session->eState != GM_SVR_STATE_LOGIN) return;
    const auto before = *session;
    const auto generation = sessions.Generation();
    const QPointer<gmtool> window(gm::current_main_window());
    session->eState = GM_SVR_STATE_REGISTERED;
    CDataMgr::GetInstance().ClearItemMap(before.dwSessionID);
    if (window) window->UpdateSvrNameColor(before.dwSessionID, GM_SVR_STATE_REGISTERED);
    if (window && current_session(before, socket, generation)) window->LogoutServer(before.dwSessionID);
    if (!current_session(before, socket, generation)) return;
    CGMLog::GetInstance().Log("Successfully logged out of server [{}]", QString::fromUtf8(before.name.c_str()).toLocal8Bit().constData());
    if (window && current_session(before, socket, generation)) CServerMgr::GetInstance().UpdateSelectServer();
}


void CMessageHandler::OnGMQueryRoleID(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    if (!packet) return;
    const auto rows = gm::decode_role_keys({reinterpret_cast<const std::byte*>(packet), size});
    if (!rows) return;
    auto delivery = gm::CommandReply::Begin(CMD_QUERY_ROLE_ID, socket);
    if (!delivery) return;
    const QPointer<gmtool> window(gm::current_main_window());
    if (!window) return;
    delivery->Complete();
    for (const auto& row : rows->rows)
    {
        if (!window || !delivery->Current()) return;
        if (!window->AddQueryRoleIDResult(rows->area, row.id, row.name.c_str(), row.account.c_str())) return;
    }
    if (rows->truncated && window && delivery->Current()) CGMLog::GetInstance().Log("Too much data, some data not displayed");
}

void CMessageHandler::OnGMQueryRoleDataAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    if (!packet) return;
    auto reply = gm::decode_query_role({reinterpret_cast<const std::byte*>(packet), size});
    if (!reply) return;
    auto delivery = gm::CommandReply::Begin(CMD_QUERY_ROLE_DATA, socket);
    if (!delivery || std::static_pointer_cast<QUERY_ROLE_DATA>(delivery->Owner())->dwRoleID != reply->role_id) return;
    const QPointer<gmtool> window(gm::current_main_window());
    if (!window) return;
    auto* role = CDataMgr::GetInstance().StoreRole(reply->role_id, std::move(reply->document));
    if (!role) return;
    delivery->Complete();
    window->AddQueryRoleDataResult(reply->area_id, reply->online, role);
}

void CMessageHandler::OnGMAnnouceAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_ANNOUNCE_ACK>(packet, size, gm2tl_announce_ack);
    if (!reply) return;
    auto delivery = gm::CommandReply::Begin(CMD_ANNOUNCE, socket);
    if (!delivery || std::static_pointer_cast<ANNOUNCE>(delivery->Owner())->nGroupID != reply->nGroupID) return;
    delivery->Complete();
    if (reply->nResult != errSuccess) CGMLog::GetInstance().Log("Failed to announce, error code: {}", reply->nResult);
}

void CMessageHandler::OnGMGetAnnounceAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    GM2TL_GET_ANNOUNCE_LIST_ACK header;
    if (!packet || size < sizeof(header) || !gm::valid_receive_packet({packet,size})) return;
    std::memcpy(&header,packet,sizeof(header));
    if (header.wMsg != gm2tl_get_announce_list_ack) return;
    auto delivery = gm::CommandReply::Begin(CMD_GET_ANNOUNCE_LIST, socket);
    if (!delivery || std::static_pointer_cast<GET_ANNOUNCE_LIST>(delivery->Owner())->nGroupID != header.nGroupID) return;
    const auto* session = CSessionMgr::GetInstance().FromSocket(socket);
    auto records = gm::decode_announcements({reinterpret_cast<const std::byte*>(packet), size}, session->nSvrID);
    if (!records) return;
    const QPointer<gmtool> window(gm::current_main_window());
    if (!window || !CDataMgr::GetInstance().ReplaceAnnounceList(*records)) return;
    delivery->Complete();
    window->UpdateAnnounceList();
    if (window && delivery->Current() && records->empty())
        window->statusBar()->showMessage(QStringLiteral("Current announcement list is empty"));
}

void CMessageHandler::OnGMDelAnnounce(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_DEL_ANNOUNCE_ACK>(packet, size, gm2tl_del_announce_ack);
    if (!reply) return;
    auto delivery = gm::CommandReply::Begin(CMD_DEL_ANNOUNCE, socket);
    if (!delivery) return;
    const auto command = std::static_pointer_cast<DEL_ANNOUNCE>(delivery->Owner());
    if (command->nGroupID != reply->nGroupID || command->dwAnnounceSerial != reply->dwAnnounceSerial) return;
    const QPointer<gmtool> window(gm::current_main_window());
    if (!window) return;
    delivery->Complete();
    window->DelAnnounce(reply->dwAnnounceSerial);
}

void CMessageHandler::OnGMExecuteGMStrAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    if (!fixed_reply<GM2TL_EXE_GM_STR_ACK>(packet, size, gm2tl_exe_gm_str_ack)) return;
    if (auto delivery = gm::CommandReply::Begin(CMD_EXECUTE_GM_STR, socket)) delivery->Complete();
}

void CMessageHandler::OnGMLoadRoleDataAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    if (!packet || g_pRoleInfo) return;
    auto reply = gm::decode_loaded_role({reinterpret_cast<const std::byte*>(packet), size});
    if (!reply) return;
    auto delivery = gm::CommandReply::Begin(CMD_LOAD_ROLE_DATA, socket);
    if (!delivery || std::static_pointer_cast<LOAD_ROLE_DATA>(delivery->Owner())->dwRoleID != reply->role_id) return;
    const QPointer<gmtool> window(gm::current_main_window());
    if (!window || !CDataMgr::GetInstance().StoreRole(reply->role_id, std::move(reply->document))) return;
    delivery->Complete();
    // The editor runs a nested Qt loop and may submit a separate modification.
    CCmdManager::GetInstance().DelCmd(delivery->Owner());
    window->LoadRoleData(delivery->Peer(), reply->role_id);
}

void CMessageHandler::OnGMModifyRoleDataAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_MODIFY_ROLE_DATA_ACK>(packet, size, gm2tl_modify_role_data_ack);
    if (!reply) return;
    auto delivery = gm::CommandReply::Begin(CMD_MODIFY_ROLE_DATA, socket);
    if (!delivery || std::static_pointer_cast<MODIFY_ROLE_DATA>(delivery->Owner())->dwRoleID != reply->dwRoleID) return;
    delivery->Complete(); // Legacy servers leave the unused nType/dwID1 fields unspecified.
}

void CMessageHandler::OnGMQueryNameToIDAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_QUERY_NAME_TO_ID_ACK>(packet, size, gm2tl_query_name_to_id_ack);
    if (!reply) return;
    auto delivery = gm::CommandReply::Begin(CMD_QUERY_NAME_TO_ID, socket);
    if (!delivery) return;
    const auto result = CCmdManager::GetInstance().ReceiveNameToID(delivery->Peer(), reply->nIndex, reply->dwRoleID, "../nameTOid.txt");
    if (result) delivery->Complete();
    else CRI("GM name query: %s", result.error().c_str());
}

void CMessageHandler::OnGMErrorStr(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    if (!packet || !CSessionMgr::GetInstance().FromSocket(socket) ||
        !gm::valid_receive_packet({packet, size})) return;
    constexpr auto prefix = offsetof(GM2TL_ERROR_STR, szErr);
    if (size <= prefix) return;
    const auto* end = static_cast<const char*>(std::memchr(packet + prefix, 0, size - prefix));
    if (!end) return;
    const std::string text(packet + prefix, static_cast<std::size_t>(end - (packet + prefix)));
    extern INT32 g_State;
    if (g_State == 2)
    {
        if (auto* window = gm::current_main_window()) window->OutputLog(text);
    }
    else if (g_State == 1)
    {
        if (auto* login = gm::current_login_dialog()) login->ServerError(socket, text);
    }
}

void CMessageHandler::OnGMRoleExeGMCmd(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto reply = fixed_reply<GM2TL_ROLE_EXE_GM_CMD>(packet, size, gm2tl_role_exe_gm_cmd);
    if (!reply) return;
    auto delivery = gm::CommandReply::Begin(CMD_EXE_ROLE_GM_CMD, socket);
    if (!delivery || std::static_pointer_cast<EXE_ROLE_CMD>(delivery->Owner())->dwRoleID != reply->dwRoleID) return;
    delivery->Complete();
}

CLDWidget* gm::exchange_reply_view()
{
    auto* window = gm::current_main_window();
    return window ? window->GetLDWidget() : nullptr;
}
void gm::exchange_reply_log(std::string_view text)
{
    const std::string owned(text);
    if (auto* window = gm::current_main_window()) window->OutputLog(owned.c_str());
}
