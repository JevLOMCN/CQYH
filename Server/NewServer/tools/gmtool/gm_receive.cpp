#include "message_handler.h"
#include "gm_packet.h"
#include "gm_receive_packets.h"
#include "common_message.h"
#include "gmtl_message.h"
#include "log.h"
#include <cstring>
#include <exception>

using namespace InternalMessage;

CMessageHandler CMessageHandler::ms_Instance;

CMessageHandler &CMessageHandler::GetInstance()
{
    return ms_Instance;
}

CMessageHandler::CMessageHandler() = default;

CMessageHandler::~CMessageHandler()
{
    UnInit();
}

BOOL CMessageHandler::Init(INet *pNet, IPacketAllocator *pPacketAllocator,std::shared_ptr<void> lifetime)
{
    if (!pNet || !pPacketAllocator || m_accepting) return FALSE;
    m_MessageFuncList.fill(nullptr);

    m_pNet = pNet;
    m_pPacketAllocator = pPacketAllocator;
    m_lifetime=std::move(lifetime);

    m_MessageFuncList[a2a_ping] = &CMessageHandler::OnAllPing;

    m_MessageFuncList[gm2tl_register_ack] = &CMessageHandler::OnGMRegisterAck;
    m_MessageFuncList[gm2tl_verify_ack] = &CMessageHandler::OnGMVerifyAck;
    m_MessageFuncList[gm2tl_login_ack] = &CMessageHandler::OnGMLoginAck;
    m_MessageFuncList[gm2tl_item_list_ntf] = &CMessageHandler::OnGMItemListNtf;
    m_MessageFuncList[gm2tl_logout_ack] = &CMessageHandler::OnGMLogoutAck;
    m_MessageFuncList[gm2tl_pre_query_ack] = &CMessageHandler::OnGMPreQueryAck;
    m_MessageFuncList[gm2tl_query_role_id_ack] = &CMessageHandler::OnGMQueryRoleID;
    m_MessageFuncList[gm2tl_query_role_data_ack] = &CMessageHandler::OnGMQueryRoleDataAck;

    m_MessageFuncList[gm2tl_forbid_chat_ack] = &CMessageHandler::OnGMForbidChatAck;
    m_MessageFuncList[gm2tl_freeze_role_ack] = &CMessageHandler::OnGMFreezeRoleAck;
    m_MessageFuncList[gm2tl_kick_role_ack] = &CMessageHandler::OnGMKickRoleAck;
    m_MessageFuncList[gm2tl_send_mail_ack] = &CMessageHandler::OnGMSendMailAck;
    m_MessageFuncList[gm2tl_announce_ack] = &CMessageHandler::OnGMAnnouceAck;
    m_MessageFuncList[gm2tl_get_announce_list_ack] = &CMessageHandler::OnGMGetAnnounceAck;
    m_MessageFuncList[gm2tl_del_announce_ack] = &CMessageHandler::OnGMDelAnnounce;
    m_MessageFuncList[gm2tl_log_ack] = &CMessageHandler::OnGMLogAck;
    m_MessageFuncList[gm2tl_exe_gm_str_ack] = &CMessageHandler::OnGMExecuteGMStrAck;
    m_MessageFuncList[gm2tl_load_role_data_ack] = &CMessageHandler::OnGMLoadRoleDataAck;
    m_MessageFuncList[gm2tl_modify_role_data_ack] = &CMessageHandler::OnGMModifyRoleDataAck;
    m_MessageFuncList[gm2tl_query_name_to_id_ack] = &CMessageHandler::OnGMQueryNameToIDAck;
    m_MessageFuncList[gm2tl_error_str] = &CMessageHandler::OnGMErrorStr;
    m_MessageFuncList[gm2tl_role_exe_gm_cmd] = &CMessageHandler::OnGMRoleExeGMCmd;

    m_MessageFuncList[gm2tl_select_group_ack] = &CMessageHandler::OnGMSelectGroupAck;
    m_MessageFuncList[gm2tl_query_exchange_log_ack] = &CMessageHandler::OnGMQueryExchangeLogAck;
    m_MessageFuncList[gm2tl_query_exchange_history_ack] = &CMessageHandler::OnGMQueryExchangeHistoryAck;
    m_MessageFuncList[gm2tl_exchange_price] = &CMessageHandler::OnGMExchangePrice;
    m_MessageFuncList[gm2tl_query_order_detail_ack] = &CMessageHandler::OnGMQueryOrderDetailAck;
    m_MessageFuncList[gm2tl_add_system_order_ack] = &CMessageHandler::OnGMAddSystemOrderAck;
    m_MessageFuncList[gm2tl_del_system_order_ack] = &CMessageHandler::OnGMDelSystemOrderAck;
    m_MessageFuncList[gm2tl_query_sys_exchange_info_ack] = &CMessageHandler::OnGMQuerySysExchangeInfoAck;
    m_MessageFuncList[gm2tl_get_exchange_limit_ack] = &CMessageHandler::OnGMGetExchangeLimitAck;
    m_MessageFuncList[gm2tl_set_exchange_limit_ack] = &CMessageHandler::OnGMSetExchangeLimitAck;
    ++m_generation;
    m_accepting = true;
    return TRUE;
}

BOOL CMessageHandler::UnInit()
{
    ++m_generation;
    m_accepting = false;
    m_MessageFuncList.fill(nullptr);
    m_pNet = nullptr;
    m_lifetime.reset();
    // Keep the allocator available for packets delivered during transport
    // teardown. The runtime must destroy the transport before the allocator.
    return TRUE;
}

void CMessageHandler::ReleaseAllocator(IPacketAllocator* allocator)
{
    if(!m_accepting && m_pPacketAllocator==allocator) m_pPacketAllocator=nullptr;
}
void CMessageHandler::OnRecv(ISocketSession* socket, PACKET_BUFFER* buffer, UINT32 size)
{
    gm::Packet packet(buffer, gm::PacketDeleter{m_pPacketAllocator,m_lifetime});
    if (!m_accepting || !socket || !buffer || size < sizeof(MESSAGE_HEADER) || size > MAX_MESSAGE_SIZE)
    {
        if (socket) socket->Close();
        return;
    }
    MESSAGE_HEADER header;
    std::memcpy(&header, buffer, sizeof(header));
    const auto callback = m_MessageFuncList[header.wMsg];
    if (!callback || !gm::valid_receive_packet({buffer, size}))
    {
        CRI("Invalid GM message id %u size %u", header.wMsg, size);
        socket->Close();
        return;
    }
    try
    {
        (this->*callback)(buffer, size, socket);
    }
    catch (const std::exception& error)
    {
        CRI("GM message %u failed: %s", header.wMsg, error.what());
        socket->Close();
    }
    catch (...)
    {
        CRI("GM message %u failed", header.wMsg);
        socket->Close();
    }
}
