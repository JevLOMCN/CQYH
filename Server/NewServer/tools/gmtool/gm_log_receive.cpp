#include "message_handler.h"
#include "cmd_manager.h"
#include "gm_log_data.h"
#include "gm_receive_packets.h"
#include "gm_command_reply.h"
#include "session_mgr.h"

void CMessageHandler::OnGMLogAck(PACKET_BUFFER* packet,UINT32 size,ISocketSession* socket)
{
    if (!packet || !socket) return;
    const auto* session=CSessionMgr::GetInstance().FromSocket(socket);
    if (!session || session->pSocketSession!=socket || session->nConnectState!=SESSION_CONNECTED) return;
    auto& manager=CCmdManager::GetInstance();
    const auto active=manager.GetCurrentCmd();
    if (!active || active->Kind()!=CMD_LOG || active->nType!=CMD_LOG || active->nSvrNum<=0) return;
    const auto command=std::static_pointer_cast<LOG_DATA>(active);
    if (command->reply_peer!=session->dwSessionID || command->reply_socket!=socket || command->reply_busy || command->reply_done) return;
    const auto reply=gm::decode_log_reply({packet,size});
    if (!reply || reply->type!=command->nLogType) return;
    auto delivery=gm::CommandReply::Begin(CMD_LOG,socket);
    if (!delivery) return;
    command->reply_busy=true;
    struct Reset { std::shared_ptr<LOG_DATA> command;~Reset(){command->reply_busy=false;} } reset{command};
    if (!gm::output_game_log(reply->type,reply->truncated,reply->text)) return;
    // Keep the original owner alive across callbacks; never decrement a replacement command.
    command->reply_done=true;delivery->Complete();
}
