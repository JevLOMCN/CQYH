#pragma once
#include "std_type.h"
#include "common_message.h"
#include "gmtl_message.h"
#include <cstring>
#include <vector>

namespace gm_test
{
template<class T> void put(std::vector<char>& bytes, std::size_t offset, T value)
{
    std::memcpy(bytes.data() + offset, &value, sizeof(value));
}
inline std::vector<char> receive_packet(UINT16 id)
{
    using namespace InternalMessage;
    std::size_t size = sizeof(MESSAGE_HEADER);
    switch (id)
    {
    case a2a_ping: size = sizeof(A2A_PING); break;
    case gm2tl_register_ack: size = sizeof(GM2TL_REGISTER_ACK); break;
    case gm2tl_verify_ack: size = sizeof(GM2TL_VERIFY_ACK); break;
    case gm2tl_login_ack: size = sizeof(GM2TL_LOGIN_ACK); break;
    case gm2tl_item_list_ntf: size = sizeof(GM2TL_ITEM_LIST_NTF); break;
    case gm2tl_logout_ack: size = sizeof(GM2TL_LOGOUT_ACK); break;
    case gm2tl_pre_query_ack: size = sizeof(GM2TL_PRE_QUERY_ACK); break;
    case gm2tl_query_role_id_ack: size = sizeof(GM2TL_QUERY_ROLE_ID_ACK); break;
    case gm2tl_query_role_data_ack: size = offsetof(GM2TL_QUERY_ROLE_DATA_ACK, szData); break;
    case gm2tl_forbid_chat_ack: size = sizeof(GM2TL_FORBID_CHAT_ACK); break;
    case gm2tl_freeze_role_ack: size = sizeof(GM2TL_FREEZE_ROLE_ACK); break;
    case gm2tl_kick_role_ack: size = sizeof(GM2TL_KICK_ROLE_ACK); break;
    case gm2tl_send_mail_ack: size = sizeof(GM2TL_SEND_MAIL_ACK); break;
    case gm2tl_announce_ack: size = sizeof(GM2TL_ANNOUNCE_ACK); break;
    case gm2tl_get_announce_list_ack: size = sizeof(GM2TL_GET_ANNOUNCE_LIST_ACK); break;
    case gm2tl_del_announce_ack: size = sizeof(GM2TL_DEL_ANNOUNCE_ACK); break;
    case gm2tl_log_ack: size = sizeof(GM2TL_LOG_ACK); break;
    case gm2tl_exe_gm_str_ack: size = sizeof(GM2TL_EXE_GM_STR_ACK); break;
    case gm2tl_load_role_data_ack: size = offsetof(GM2TL_LOAD_ROLE_DATA_ACK, szData); break;
    case gm2tl_modify_role_data_ack: size = sizeof(GM2TL_MODIFY_ROLE_DATA_ACK); break;
    case gm2tl_query_name_to_id_ack: size = sizeof(GM2TL_QUERY_NAME_TO_ID_ACK); break;
    case gm2tl_error_str: size = sizeof(GM2TL_ERROR_STR) + 1; break;
    case gm2tl_role_exe_gm_cmd: size = sizeof(GM2TL_ROLE_EXE_GM_CMD); break;
    case gm2tl_select_group_ack: size = sizeof(GM2TL_SELECT_GROUP_ACK); break;
    case gm2tl_query_exchange_log_ack: size = sizeof(GM2TL_QUERY_EXCHANGE_LOG_ACK); break;
    case gm2tl_query_exchange_history_ack: size = sizeof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK); break;
    case gm2tl_exchange_price: size = sizeof(GM2TL_EXCHANGE_PRICE); break;
    case gm2tl_query_order_detail_ack: size = sizeof(GM2TL_QUERY_ORDER_DETAIL_ACK); break;
    case gm2tl_add_system_order_ack: size = sizeof(GM2TL_ADD_SYSTEM_ORDER_ACK); break;
    case gm2tl_del_system_order_ack: size = sizeof(GM2TL_DEL_SYSTEM_ORDER_ACK); break;
    case gm2tl_query_sys_exchange_info_ack: size = sizeof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK); break;
    case gm2tl_get_exchange_limit_ack: size = sizeof(GM2TL_GET_EXCHANGE_LIMIT_ACK); break;
    case gm2tl_set_exchange_limit_ack: size = sizeof(GM2TL_SET_EXCHANGE_LIMIT_ACK); break;
    default: break;
    }
    std::vector<char> bytes(size);
    put(bytes, 0, id);
    if (id == gm2tl_log_ack) put(bytes, offsetof(GM2TL_LOG_ACK, nLength), INT32{1});
    return bytes;
}
}
