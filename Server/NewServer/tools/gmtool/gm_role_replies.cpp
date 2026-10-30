#include "message_handler.h"
#include "cmd_manager.h"
#include "process_error.h"
#include "session_mgr.h"
#include "gmtl_message.h"
#include "gm_log.h"
#include "gm_command_reply.h"
#include "gm_receive_packets.h"
#include <cstring>

using namespace InternalMessage;
namespace
{
template<class Message, class Receive> void route(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket, INT32 kind, UINT16 message, Receive receive)
{
    if (!packet || size < sizeof(Message) || !socket) return;
    if (!gm::valid_receive_packet({packet,size})) return;
    auto delivery = gm::CommandReply::Begin(kind, socket);
    if (!delivery) return;
    const auto id = delivery->Peer();
    Message reply;
    std::memcpy(&reply, packet, sizeof(reply));
    if (reply.wMsg != message) return;
    const auto result = receive(id, reply);
    if (result) delivery->Complete();
    if (!result)
    {
        CRI("GM role reply: %s", result.error().c_str());
        CGMLog::GetInstance().Log("{}", result.error().c_str());
    }
}
}
void CMessageHandler::OnGMPreQueryAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    route<GM2TL_PRE_QUERY_ACK>(packet,size,socket,CMD_PRE_QUERY,gm2tl_pre_query_ack,[](UINT32 session, const auto& reply) -> std::expected<void,std::string>
    {
        const auto* end = static_cast<const char*>(std::memchr(reply.szRoleName,0,sizeof(reply.szRoleName)));
        if (!end) return std::unexpected("unterminated role-name reply");
        return CCmdManager::GetInstance().ReceivePreQuery(session,reply.dwRoleID,
            {reply.szRoleName,static_cast<std::size_t>(end-reply.szRoleName)});
    });
}
void CMessageHandler::OnGMForbidChatAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    route<GM2TL_FORBID_CHAT_ACK>(packet,size,socket,CMD_FORBIDDEN_CHAT,gm2tl_forbid_chat_ack,[](UINT32 session, const auto& reply)
    { return CCmdManager::GetInstance().ReceiveBulkReply(CMD_FORBIDDEN_CHAT,session,reply.dwRoleID,reply.nResult); });
}
void CMessageHandler::OnGMFreezeRoleAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    route<GM2TL_FREEZE_ROLE_ACK>(packet,size,socket,CMD_FREEZE_ROLE,gm2tl_freeze_role_ack,[](UINT32 session, const auto& reply)
    { return CCmdManager::GetInstance().ReceiveBulkReply(CMD_FREEZE_ROLE,session,reply.dwRoleID,reply.nResult); });
}
void CMessageHandler::OnGMKickRoleAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    route<GM2TL_KICK_ROLE_ACK>(packet,size,socket,CMD_KICK_ROLE,gm2tl_kick_role_ack,[](UINT32 session, const auto& reply)
    { return CCmdManager::GetInstance().ReceiveBulkReply(CMD_KICK_ROLE,session,reply.dwRoleID,reply.nResult); });
}
void CMessageHandler::OnGMSendMailAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    route<GM2TL_SEND_MAIL_ACK>(packet,size,socket,CMD_SEND_MAIL,gm2tl_send_mail_ack,[](UINT32 session, const auto& reply)
    { return CCmdManager::GetInstance().ReceiveBulkReply(CMD_SEND_MAIL,session,reply.dwRoleID,reply.nResult); });
}
