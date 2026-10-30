#include "message_handler.h"
#include "gm_exchange_replies.h"
#include "gm_receive_packets.h"
#include "gm_exchange_stats.h"
#include "gmtl_message.h"
#include "session_mgr.h"
#include "server_mgr.h"
#include "ld_widget.h"
#include "error_def.h"
#include <algorithm>
#include <cstring>
#include <format>
#include <vector>

using namespace InternalMessage;
namespace
{
template<class T,class Action>
void receive(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket,UINT16 id,Action action,bool selecting=false)
{
    if (!bytes || !socket || size<sizeof(T) || size>MAX_MESSAGE_SIZE ||
        !CServerMgr::GetInstance().IsSingleSelected()) return;
    const std::span<const char> packet(bytes,size);
    T header;
    std::memcpy(&header,bytes,sizeof(header));
    if (header.wMsg!=id || !gm::valid_receive_packet(packet) || header.byGroupID>=MAX_SERVER_GROUP_COUNT) return;
    const auto* session=CSessionMgr::GetInstance().FromSocket(socket);
    if (!session || session->pSocketSession!=socket || session->nConnectState!=SESSION_CONNECTED || session->eState!=GM_SVR_STATE_LOGIN ||
        session->dwSessionID!=CServerMgr::GetInstance().GetSessionIDByIndex(0)) return;
    const QPointer<CLDWidget> view=gm::exchange_reply_view();
    if (!view || (!selecting && (!header.byGroupID || header.byGroupID!=view->GetGroupID()))) return;
    action(view,header,packet.subspan(sizeof(T)));
}
template<class T>
std::vector<T> records(std::span<const char> bytes,std::size_t count)
{
    std::vector<T> result(count);
    if (count) std::memcpy(result.data(),bytes.data(),count*sizeof(T));
    return result;
}
}
void CMessageHandler::OnGMSelectGroupAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_SELECT_GROUP_ACK>(bytes,size,socket,gm2tl_select_group_ack,
        [](auto view,const auto& header,auto){view->SetGroupID(header.byGroupID);},true);
}
void CMessageHandler::OnGMQueryExchangeLogAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_QUERY_EXCHANGE_LOG_ACK>(bytes,size,socket,gm2tl_query_exchange_log_ack,
        [](auto view,const auto& header,auto body)
        {
            const auto rows=records<GM_EXCHANGE_PRICE_LOG>(body,header.nCount);
            if (std::ranges::any_of(rows,[&](const auto& row){return row.byGroupID!=header.byGroupID;})) return;
            view->UpdateExchangeLog(header.byGroupID,header.nCount,header.byIsClear,reinterpret_cast<const char*>(rows.data()));
        });
}
void CMessageHandler::OnGMQueryExchangeHistoryAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_QUERY_EXCHANGE_HISTORY_ACK>(bytes,size,socket,gm2tl_query_exchange_history_ack,
        [](auto view,const auto& header,auto body)
        {
            const auto rows=records<GM_EXCHANGE_PRICE_HISTORY>(body,header.nCount);
            if (std::ranges::any_of(rows,[&](const auto& row){return row.byGroupID!=header.byGroupID;})) return;
            view->UpdateExchangeHistory(header.byGroupID,header.nCount,header.byIsClear,reinterpret_cast<const char*>(rows.data()));
        });
}
void CMessageHandler::OnGMExchangePrice(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_EXCHANGE_PRICE>(bytes,size,socket,gm2tl_exchange_price,
        [](auto view,const auto& header,auto body)
        {
            const auto wire=records<GMTL_EXCHANGE_PRICE_INFO>(body,header.bySellPriceCount+header.byBuyPriceCount);
            std::vector<PRICE_DATA> rows;rows.reserve(wire.size());
            for (const auto& row:wire) rows.push_back({row.nPrice,row.nCount});
            view->UpdatePrice(header.byGroupID,header.bySellPriceCount,header.byBuyPriceCount,rows.data());
        });
}
void CMessageHandler::OnGMQueryOrderDetailAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_QUERY_ORDER_DETAIL_ACK>(bytes,size,socket,gm2tl_query_order_detail_ack,
        [](auto view,const auto& header,auto body)
        {
            if (header.byOrderType!=1 && header.byOrderType!=2) return;
            const auto wire=records<GM_EXCHANGE_COUNT_INFO>(body,header.byCountCount);
            std::vector<EXCHANGE_COUNT_DATA> rows;rows.reserve(wire.size());
            for (const auto& row:wire)
            {
                EXCHANGE_COUNT_DATA value{};
                value.dwRoleID=row.dwRoleID;value.qwOrderID=row.qwOrderID;value.nCount=row.nCount;
                std::memcpy(value.szRoleName,row.szRoleName,sizeof(value.szRoleName));rows.push_back(value);
            }
            view->UpdateOrderDetail(header.byGroupID,header.byOrderType,header.nPrice,static_cast<INT32>(rows.size()),rows.data());
        });
}
void CMessageHandler::OnGMAddSystemOrderAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_ADD_SYSTEM_ORDER_ACK>(bytes,size,socket,gm2tl_add_system_order_ack,
        [](auto,const auto& header,auto)
        {
            gm::exchange_reply_log(std::format("增加订单{}. 单价 {}, 数量 {}",header.nResult==errSuccess ? "成功" : "失败",
                gm::exchange_price(header.nPrice),header.nCount));
        });
}
void CMessageHandler::OnGMDelSystemOrderAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_DEL_SYSTEM_ORDER_ACK>(bytes,size,socket,gm2tl_del_system_order_ack,
        [](auto view,const auto& header,auto)
        {
            const auto message=std::format("取消订单{}. 单价 {}, 数量 {}",header.nResult==errSuccess ? "成功" : "失败",
                gm::exchange_price(header.nPrice),header.nCount);
            if (header.nResult==errSuccess) view->OrderDeleted(header.qwOrderID);
            gm::exchange_reply_log(message);
        });
}
void CMessageHandler::OnGMQuerySysExchangeInfoAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK>(bytes,size,socket,gm2tl_query_sys_exchange_info_ack,
        [](auto view,const auto& h,auto)
        {
            if (h.byFlag==1) view->ShowTodayStat(h.byGroupID,h.llSysTotalMoney,h.llTotalMoney,h.llSysTotalExchangeMoney,
                h.llTotalExchangeMoney,h.llSysExchangeRMB,h.llExchangeRMB,h.nAveragePriceToday,h.nAveragePriceLastHour);
            else if (h.byFlag==0) view->ShowYesterdayStat(h.byGroupID,h.llSysTotalMoney,h.llTotalMoney,h.llSysTotalExchangeMoney,
                h.llTotalExchangeMoney,h.llSysExchangeRMB,h.llExchangeRMB,h.nAveragePriceToday,h.nAveragePriceLastHour);
            else if (h.byFlag==3)
            {
                if (!gm::exchange_window(h.dwBeginTime,h.dwEndTime)) return;
                const auto written=view->OutputStatToFile(h.dwBeginTime,h.dwEndTime,h.byGroupID,h.llSysTotalMoney,h.llTotalMoney,
                    h.llSysTotalExchangeMoney,h.llTotalExchangeMoney,h.llSysExchangeRMB,h.llExchangeRMB,h.nAveragePriceToday,h.nAveragePriceLastHour);
                gm::exchange_reply_log(written ? "导出数据成功" : "导出数据失败");
            }
        });
}
void CMessageHandler::OnGMGetExchangeLimitAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_GET_EXCHANGE_LIMIT_ACK>(bytes,size,socket,gm2tl_get_exchange_limit_ack,
        [](auto view,const auto& h,auto){view->ShowExchangeLimit(h.nPriceHigh,h.nPriceLow,h.nSingleOrderLimit,h.nDailyOrderLimit);});
}
void CMessageHandler::OnGMSetExchangeLimitAck(PACKET_BUFFER* bytes,UINT32 size,ISocketSession* socket)
{
    receive<GM2TL_SET_EXCHANGE_LIMIT_ACK>(bytes,size,socket,gm2tl_set_exchange_limit_ack,
        [](auto view,const auto& h,auto)
        {
            view->ShowExchangeLimit(h.nPriceHigh,h.nPriceLow,h.nSingleOrderLimit,h.nDailyOrderLimit);
            gm::exchange_reply_log("修改订单限制成功");
        });
}
