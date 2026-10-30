#include "stdafx.h"
#include "ld_widget.h"
#include "gm_exchange_replies.h"
#include "gm_log_view.h"
#include "gm_receive_fixture.h"
#include "session_mgr.h"
#include "detail.h"
#include "gm_item_info.h"
#include "message_handler.h"
#include "server_mgr.h"
#include "tl_config.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QTextEdit>
#include <QtGui/QFontDatabase>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <format>
#include <functional>
#include <map>
#include <QtCore/QTimer>

TL_CONFIG g_TLConfig{};
namespace
{
std::size_t checks{},native_calls{};
bool native_success=true;
std::function<void()> native_callback;
std::array<INT32,5> native_values{};
UINT64 native_order{};
UINT32 selected_session=17;
INT32 selected_count=1;
std::vector<std::string> native_queries;
std::array<UINT32,4> native_times{};
QPointer<CLDWidget> reply_view;
std::map<ISocketSession*,GM_SESSION*> reply_sessions;
std::vector<std::string> reply_logs;
std::function<void()> reply_log_callback;
BOOL send_result()
{
    ++native_calls;
    const auto callback=native_callback;
    const auto result=native_success;
    if(callback) callback();
    return result;
}
void check(bool value,const char* text)
{
    ++checks;
    if (std::getenv("GM_EXCHANGE_TRACE")) std::cerr<<checks<<": "<<text<<'\n';
    if(!value)throw std::runtime_error(text);
}
void flush(){QCoreApplication::sendPostedEvents(nullptr,QEvent::DeferredDelete);QApplication::processEvents();}
struct ReplyAllocator final : IPacketAllocator
{
    std::map<char*,std::unique_ptr<char[]>> packets;
    std::size_t allocated{},freed{};
    PACKET_BUFFER* Alloc(INT32 size) override
    {
        auto storage=std::make_unique<char[]>(size+1);
        auto* bytes=storage.get()+1; // Exercise deliberately unaligned incoming packets.
        packets.emplace(bytes,std::move(storage));++allocated;return bytes;
    }
    void Free(PACKET_BUFFER* bytes) override {if(packets.erase(bytes)!=1)std::abort();++freed;}
    BOOL IsAllocated(PACKET_BUFFER* bytes) override{return packets.contains(bytes);}
    UINT32 PrintMemoryStat() override{return static_cast<UINT32>(packets.size());}
    void CheckError() override{}
};
struct ReplySocket final : ISocketSession
{
    GM_SESSION session{};
    std::size_t closes{};
    void* user=&session;
    ReplySocket(){session.connection_lifetime=std::make_shared<const char>(0);session.dwSessionID=selected_session;session.pSocketSession=this;session.eState=GM_SVR_STATE_LOGIN;session.nConnectState=SESSION_CONNECTED;reply_sessions.emplace(this,&session);}
    ~ReplySocket(){reply_sessions.erase(this);}
    INT32 Send(PACKET_BUFFER*,INT32,INT32) override{return 0;}
    INT32 SendNoPackageHead(PACKET_BUFFER*,INT32) override{return 0;}
    void* GetUserData() override{return user;}
    void SetUserData(void* value) override{user=value;}
    BOOL GetAddress(UINT32&,UINT16&) override{return FALSE;}
    BOOL GetRemoteAddress(UINT32&,UINT16&) override{return FALSE;}
    BOOL Close() override{++closes;return TRUE;}
};
struct ReplyNet final : INet
{
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override{return TRUE;}
    BOOL UnInit() override{return TRUE;}
    ISocketAcceptor* CreateAcceptor() override{return nullptr;}
    BOOL DestroyAcceptor(ISocketAcceptor*) override{return TRUE;}
    ISocketSession* Connect(UINT32,UINT16) override{return nullptr;}
    BOOL MainLoop() override{return TRUE;}
    INT64 PrintMemoryStat() override{return 0;}
};
template<class T> std::vector<char> reply(UINT16 id,UINT8 group=1)
{
    auto bytes=gm_test::receive_packet(id);
    gm_test::put(bytes,offsetof(T,byGroupID),group);return bytes;
}
void exchange_replies(const std::filesystem::path& screenshots)
{
    using namespace InternalMessage;
    ReplyAllocator allocator;ReplyNet net;ReplySocket socket;
    auto& handler=CMessageHandler::GetInstance();check(handler.Init(&net,&allocator),"actual receive dispatcher initialized");
    struct Stop{CMessageHandler& handler;~Stop(){handler.UnInit();}} stop{handler};
    const auto deliver=[&](const std::vector<char>& bytes)
    {
        auto* packet=allocator.Alloc(static_cast<INT32>(bytes.size()));std::memcpy(packet,bytes.data(),bytes.size());
        handler.OnRecv(&socket,packet,static_cast<UINT32>(bytes.size()));
        check(allocator.packets.empty() && allocator.allocated==allocator.freed,"actual receive releases its captured packet once");
    };
    CLDWidget view;view.ServerLogin(1);reply_view=&view;reply_logs.clear();
    auto select=reply<GM2TL_SELECT_GROUP_ACK>(gm2tl_select_group_ack);
    deliver(select);check(view.GetGroupID()==1,"actual group reply reaches view through OnRecv");
    auto other=reply<GM2TL_SELECT_GROUP_ACK>(gm2tl_select_group_ack,2);
    socket.session.dwSessionID=18;deliver(other);check(view.GetGroupID()==1,"reply from unselected peer ignored");socket.session.dwSessionID=selected_session;
    socket.session.eState=GM_SVR_STATE_REGISTERED;deliver(other);check(view.GetGroupID()==1,"reply from peer before login ignored");socket.session.eState=GM_SVR_STATE_LOGIN;
    socket.session.nConnectState=SESSION_DISCONNECTED;deliver(other);check(view.GetGroupID()==1,"reply from disconnected session ignored");socket.session.nConnectState=SESSION_CONNECTED;
    socket.session.pSocketSession=nullptr;deliver(other);check(view.GetGroupID()==1,"reply with replaced socket ignored");socket.session.pSocketSession=&socket;
    selected_count=2;CServerMgr::GetInstance().Init();deliver(other);check(view.GetGroupID()==1,"reply with multiple peers selected ignored");selected_count=1;CServerMgr::GetInstance().Init();
    auto price=reply<GM2TL_EXCHANGE_PRICE>(gm2tl_exchange_price);price.resize(price.size()+2*sizeof(GMTL_EXCHANGE_PRICE_INFO));
    gm_test::put(price,offsetof(GM2TL_EXCHANGE_PRICE,bySellPriceCount),UINT8{1});gm_test::put(price,offsetof(GM2TL_EXCHANGE_PRICE,byBuyPriceCount),UINT8{1});
    gm_test::put(price,sizeof(GM2TL_EXCHANGE_PRICE),GMTL_EXCHANGE_PRICE_INFO{INT32_MAX,3});
    gm_test::put(price,sizeof(GM2TL_EXCHANGE_PRICE)+sizeof(GMTL_EXCHANGE_PRICE_INFO),GMTL_EXCHANGE_PRICE_INFO{INT32_MIN,4});deliver(price);
    auto* sells=view.findChild<QTreeWidget*>("treeWidget");auto* buys=view.findChild<QTreeWidget*>("treeWidget_2");
    check(sells->topLevelItem(0)->text(1)=="21474836.47" && buys->topLevelItem(0)->text(1)=="-21474836.48","unaligned price reply renders exact owned rows");
    socket.user=reinterpret_cast<void*>(1);deliver(price);
    check(sells->topLevelItem(0)->text(1)=="21474836.47","owned socket mapping ignores invalid native userdata");socket.user=&socket.session;
    auto wrong_price=price;gm_test::put(wrong_price,offsetof(GM2TL_EXCHANGE_PRICE,byGroupID),UINT8{2});deliver(wrong_price);
    check(sells->topLevelItemCount()==1 && buys->topLevelItemCount()==1,"wrong-group price reply preserves active rows");
    auto log=reply<GM2TL_QUERY_EXCHANGE_LOG_ACK>(gm2tl_query_exchange_log_ack);log.resize(log.size()+sizeof(GM_EXCHANGE_PRICE_LOG));
    gm_test::put(log,offsetof(GM2TL_QUERY_EXCHANGE_LOG_ACK,nCount),INT32{1});gm_test::put(log,offsetof(GM2TL_QUERY_EXCHANGE_LOG_ACK,byIsClear),UINT8{1});
    GM_EXCHANGE_PRICE_LOG entry{};entry.byGroupID=1;entry.nPrice=29;entry.nCount=7;std::memcpy(entry.szExchangeTime,"2026-01-02 03:04:00",20);
    gm_test::put(log,sizeof(GM2TL_QUERY_EXCHANGE_LOG_ACK),entry);deliver(log);
    auto* logs=view.findChild<QTreeWidget*>("treeWidget_3");check(logs->topLevelItemCount()==1 && logs->topLevelItem(0)->text(1)=="0.29","actual log reply renders owned table rows");
    entry.byGroupID=2;gm_test::put(log,sizeof(GM2TL_QUERY_EXCHANGE_LOG_ACK),entry);deliver(log);
    check(logs->topLevelItemCount()==1,"inconsistent row group cannot replace current logs");
    auto history=reply<GM2TL_QUERY_EXCHANGE_HISTORY_ACK>(gm2tl_query_exchange_history_ack);history.resize(history.size()+sizeof(GM_EXCHANGE_PRICE_HISTORY));
    gm_test::put(history,offsetof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK,nCount),INT32{1});gm_test::put(history,offsetof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK,byIsClear),UINT8{1});
    GM_EXCHANGE_PRICE_HISTORY sample{};sample.byGroupID=1;sample.nOpen=100;sample.nHigh=200;sample.nLow=50;sample.nClose=150;sample.nVolume=20;
    std::memcpy(sample.szHistoryTime,"2026-01-02 03:04:00",20);gm_test::put(history,sizeof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK),sample);deliver(history);
    auto* chart=static_cast<CExchangeHistoryUI*>(view.findChild<QLabel*>("label_8"));const auto image=chart->grab().toImage();
    sample.byGroupID=2;sample.nClose=100000;gm_test::put(history,sizeof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK),sample);deliver(history);
    check(chart->grab().toImage()==image,"inconsistent history group preserves chart snapshot");
    auto detail=reply<GM2TL_QUERY_ORDER_DETAIL_ACK>(gm2tl_query_order_detail_ack);detail.resize(detail.size()+sizeof(GM_EXCHANGE_COUNT_INFO));
    gm_test::put(detail,offsetof(GM2TL_QUERY_ORDER_DETAIL_ACK,byOrderType),UINT8{2});gm_test::put(detail,offsetof(GM2TL_QUERY_ORDER_DETAIL_ACK,byCountCount),UINT8{1});
    GM_EXCHANGE_COUNT_INFO order{};order.qwOrderID=UINT64_MAX;order.nCount=3;std::memcpy(order.szRoleName,"role",5);
    gm_test::put(detail,sizeof(GM2TL_QUERY_ORDER_DETAIL_ACK),order);deliver(detail);
    QPointer<CDetailDialog> dialog=view.findChild<CDetailDialog*>();check(dialog && dialog->ContainsOrder(UINT64_MAX),"actual detail reply owns full-width order ID");
    auto invalid_detail=detail;gm_test::put(invalid_detail,offsetof(GM2TL_QUERY_ORDER_DETAIL_ACK,byOrderType),UINT8{7});deliver(invalid_detail);check(dialog,"invalid order type preserves detail dialog");
    auto deletion=reply<GM2TL_DEL_SYSTEM_ORDER_ACK>(gm2tl_del_system_order_ack);
    gm_test::put(deletion,offsetof(GM2TL_DEL_SYSTEM_ORDER_ACK,nResult),INT32{errUnknownError});gm_test::put(deletion,offsetof(GM2TL_DEL_SYSTEM_ORDER_ACK,qwOrderID),UINT64_MAX);deliver(deletion);flush();
    check(dialog && reply_logs.back().find("失败")!=std::string::npos,"failed deletion preserves retry dialog and logs failure");
    gm_test::put(deletion,offsetof(GM2TL_DEL_SYSTEM_ORDER_ACK,nResult),INT32{errSuccess});gm_test::put(deletion,offsetof(GM2TL_DEL_SYSTEM_ORDER_ACK,qwOrderID),UINT64{42});deliver(deletion);flush();
    check(dialog,"unrelated deletion cannot close current detail");
    gm_test::put(deletion,offsetof(GM2TL_DEL_SYSTEM_ORDER_ACK,qwOrderID),UINT64_MAX);deliver(deletion);flush();check(!dialog,"matching successful deletion retires current detail");
    auto addition=reply<GM2TL_ADD_SYSTEM_ORDER_ACK>(gm2tl_add_system_order_ack);gm_test::put(addition,offsetof(GM2TL_ADD_SYSTEM_ORDER_ACK,nPrice),INT32_MAX);
    gm_test::put(addition,offsetof(GM2TL_ADD_SYSTEM_ORDER_ACK,nResult),INT32{errSuccess});
    gm_test::put(addition,offsetof(GM2TL_ADD_SYSTEM_ORDER_ACK,nCount),INT32{7});deliver(addition);
    check(reply_logs.back().find("21474836.47")!=std::string::npos && reply_logs.back().find("成功")!=std::string::npos,"successful add reply logs complete price and outcome");
    const auto prior_logs=reply_logs.size();gm_test::put(addition,offsetof(GM2TL_ADD_SYSTEM_ORDER_ACK,byGroupID),UINT8{2});deliver(addition);check(reply_logs.size()==prior_logs,"wrong-group action reply cannot report success");
    auto limits=reply<GM2TL_GET_EXCHANGE_LIMIT_ACK>(gm2tl_get_exchange_limit_ack);gm_test::put(limits,offsetof(GM2TL_GET_EXCHANGE_LIMIT_ACK,nPriceHigh),INT32_MAX);deliver(limits);
    auto* limit=view.findChild<CExchangeLimit*>();check(limit && limit->findChild<QLabel*>("label_5")->text()=="21474836.47","actual get-limit reply renders exact price");
    auto changed=reply<GM2TL_SET_EXCHANGE_LIMIT_ACK>(gm2tl_set_exchange_limit_ack,2);gm_test::put(changed,offsetof(GM2TL_SET_EXCHANGE_LIMIT_ACK,nPriceHigh),INT32{29});deliver(changed);
    check(limit->findChild<QLabel*>("label_5")->text()=="21474836.47","wrong-group set-limit reply does not overwrite active limits");
    gm_test::put(changed,offsetof(GM2TL_SET_EXCHANGE_LIMIT_ACK,byGroupID),UINT8{1});deliver(changed);
    check(limit->findChild<QLabel*>("label_5")->text()=="0.29","matching set-limit reply updates current values");
    auto stats=reply<GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK>(gm2tl_query_sys_exchange_info_ack);gm_test::put(stats,offsetof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK,byFlag),UINT8{1});
    gm_test::put(stats,offsetof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK,llSysExchangeRMB),INT64{29});deliver(stats);
    check(view.findChild<QLabel*>("label")->text().contains("0.29"),"actual current-statistics reply reaches summary");
    gm_test::put(stats,offsetof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK,byFlag),UINT8{0});deliver(stats);
    check(view.findChild<CGMItemInfo*>(),"actual previous-statistics reply opens comparison");view.findChild<CGMItemInfo*>()->Exit();flush();
    {
        struct Restore{std::filesystem::path old=std::filesystem::current_path();~Restore(){std::error_code ignored;std::filesystem::current_path(old,ignored);}} restore;
        const auto root=std::filesystem::absolute(screenshots)/"exchange replies";std::filesystem::create_directories(root/"run");std::filesystem::current_path(root/"run");
        const auto first=static_cast<UINT32>(QDateTime(QDate(2026,1,2),QTime(0,0),Qt::UTC).toSecsSinceEpoch());
        gm_test::put(stats,offsetof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK,byFlag),UINT8{3});gm_test::put(stats,offsetof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK,dwBeginTime),first);
        gm_test::put(stats,offsetof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK,dwEndTime),first+86400);deliver(stats);
        check(reply_logs.back()=="导出数据成功" && std::filesystem::is_regular_file(root/"1-2_1-3.txt"),"export success follows actual staged report publication");
        const auto block=root/"1-4_1-5.txt";std::filesystem::create_directories(block);
        gm_test::put(stats,offsetof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK,dwBeginTime),first+2*86400);gm_test::put(stats,offsetof(GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK,dwEndTime),first+3*86400);deliver(stats);
        check(reply_logs.back()=="导出数据失败" && std::filesystem::is_directory(block),"export publication failure never logs success");
    }
    auto bad=price;bad.pop_back();const auto closes=socket.closes;deliver(bad);check(socket.closes==closes+1,"truncated frame rejected by actual dispatcher");
    auto* deleted=new CLDWidget;deleted->SetGroupID(1);reply_view=deleted;
    reply_log_callback=[&]{delete reply_view.data();};gm_test::put(addition,offsetof(GM2TL_ADD_SYSTEM_ORDER_ACK,byGroupID),UINT8{1});deliver(addition);reply_log_callback={};check(!reply_view,"widget destruction during reply log leaves no dangling view");
    reply_view=&view;reply_log_callback=[]{throw std::runtime_error("fixture logger failure");};const auto before=socket.closes;deliver(addition);reply_log_callback={};
    check(socket.closes==before+1,"throwing reply logger closes socket while packet remains scoped");
    handler.UnInit();const auto stopped=reply_logs.size();deliver(addition);check(reply_logs.size()==stopped,"late shutdown packet is freed without delivering UI reply");reply_view.clear();
}
void answer_exchange(bool accept,std::function<void()> during={})
{
    QTimer::singleShot(0,[accept,during]
    {
        QPointer<QDialog> dialog=qobject_cast<QDialog*>(QApplication::activeModalWidget());
        check(dialog && dialog->objectName()=="exchangeConfirmation","actual exchange confirmation opened");
        if(during) during();
        if(dialog) dialog->done(accept ? QDialog::Accepted : QDialog::Rejected);
    });
}
void logging_view(const std::filesystem::path& screenshots)
{
    QTextEdit tool;
    const auto now=std::chrono::system_clock::now();
    const std::string message=std::string(8192,'x')+" 日志 <b>100%</b> %n";
    check(gm::show_tool_log(tool,message,now).has_value(),"actual tool-log view accepts complete long message");
    check(tool.toPlainText().endsWith(QStringLiteral("日志 <b>100%</b> %n")) && tool.toPlainText().size()>8192,"legacy Chinese decoded explicitly and markup remains literal");
    check(gm::show_tool_log(tool,"second",now).has_value() && tool.toPlainText().endsWith("second"),"tool-log view appends plain rows");
    QTextEdit game;
    const auto root=std::filesystem::absolute(screenshots)/"logging view";std::filesystem::create_directories(root);
    const auto path=root/"output.txt";check(gm::clear_game_log(path).has_value(),"prepare GUI log output");
    auto utf8=QStringLiteral("角色 <b>100%</b>\nnext").toUtf8();utf8.push_back('\0');
    const std::span<const char> bytes(utf8.constData(),static_cast<std::size_t>(utf8.size()));
    check(gm::show_game_log(game,bytes,path).has_value() && game.toPlainText()==QStringLiteral("角色 <b>100%</b>\nnext"),"game logs retain UTF-8 and literal markup in real Qt editor");
    const auto snapshot=game.toPlainText();
    check(!gm::show_game_log(game,bytes.first(bytes.size()-1),path) && game.toPlainText()==snapshot,"unterminated log cannot alter view");
    check(!gm::show_game_log(game,bytes,root/"missing"/"file") && game.toPlainText()==snapshot,"file failure leaves displayed log unchanged");
    check(gm::show_game_log(game,bytes,path).has_value() && game.toPlainText()==snapshot+"\n"+snapshot,"successful retry appends complete plain text once");
    QPointer<QTextEdit> closing=new QTextEdit;
    QObject::connect(closing,&QTextEdit::textChanged,closing,&QObject::deleteLater);
    check(gm::show_tool_log(*closing,"close during update",now).has_value(),"queued view closure during text update");flush();check(!closing,"closing log editor releases Qt resources");
}
void exchange_queries()
{
    using namespace std::chrono;
    const auto tick=steady_clock::time_point{}+seconds(100);
    const auto wall=system_clock::time_point(seconds(2200000000));
    CLDWidget view;check(view.ServerLogin(1) && view.SetGroupID(1),"query fixture initialized");
    native_queries.clear();
    check(view.MainLoop(tick,wall) && native_queries==std::vector<std::string>{"log","history","statistics"},"actual first poll requests all views");
    check(native_values[0]==1 && native_values[1]==1 && native_times[0]==2200000000ULL/86400*86400 &&
        native_times[2]==2200000000ULL/3600*3600,"actual poll preserves full-width day/hour query fields");
    native_queries.clear();view.MainLoop(tick,wall+hours(24));
    check(native_queries.empty(),"wall-clock jump cannot repeat same monotonic poll");
    view.MainLoop(tick+seconds(1),wall-hours(24));check(native_queries==std::vector<std::string>{"log"},"wall-clock reversal does not suppress log polling");
    native_queries.clear();view.MainLoop(tick+seconds(3),wall);check(native_queries==std::vector<std::string>{"log","history"},"actual independent history cadence");
    native_queries.clear();view.MainLoop(tick+seconds(5),wall);check(native_queries==std::vector<std::string>{"log","statistics"},"actual independent statistics cadence");
    native_queries.clear();native_callback=[&]{view.MainLoop(tick+hours(2),wall);};
    view.MainLoop(tick+hours(1),wall);native_callback={};
    check(native_queries==std::vector<std::string>{"log","history","statistics"},"late and reentrant polling emits one batch");
    view.SetGroupID(2);view.SetGroupID(1);native_queries.clear();
    native_callback=[&]{view.SetGroupID(2);native_callback={};};
    check(!view.MainLoop(tick+hours(3),wall) && native_queries==std::vector<std::string>{"log"},"group change during callback stops stale remaining queries");
    native_queries.clear();view.MainLoop(tick+hours(3),wall);
    check(native_queries==std::vector<std::string>{"log","history","statistics"} && native_values[0]==2,"new group restarts polling immediately");
    view.SetGroupID(1);native_queries.clear();native_callback=[&]{view.UnInit();native_callback={};};
    check(!view.MainLoop(tick+hours(4),wall) && native_queries==std::vector<std::string>{"log"},"shutdown during callback stops remaining queries");
    view.ServerLogin(1);view.SetGroupID(1);native_queries.clear();
    native_callback=[&]{if(native_queries.back()=="statistics") {view.UnInit();native_callback={};}};
    check(!view.MainLoop(tick+hours(4),wall) && native_queries.size()==3,"closure in final statistics callback invalidates completion");
    view.ServerLogin(1);view.SetGroupID(1);native_queries.clear();
    native_callback=[&]{selected_session=18;native_callback={};};
    check(!view.MainLoop(tick+hours(4),wall) && native_queries==std::vector<std::string>{"log"},"peer change during callback stops stale queries");selected_session=17;
    QPointer<CLDWidget> deleted=new CLDWidget;deleted->SetGroupID(1);native_queries.clear();
    native_callback=[&]{delete deleted.data();native_callback={};};
    const auto survived=deleted->MainLoop(tick,wall);
    check(!survived && !deleted && native_queries==std::vector<std::string>{"log"},"widget deletion during poll callback releases scope safely");
    view.ServerLogin(1);view.SetGroupID(1);native_queries.clear();native_success=false;
    check(!view.MainLoop(tick,wall) && native_queries.size()==3,"independent queries continue after send rejection and report failure");native_success=true;
    view.SetGroupID(2);view.SetGroupID(1);native_queries.clear();
    check(!view.MainLoop(tick+seconds(1),system_clock::time_point(seconds(-1))) &&
        native_queries==std::vector<std::string>{"log","history"},"invalid wall time cannot wrap a statistics query");
    auto* begin=view.findChild<QDateTimeEdit*>("dateTimeEdit");auto* end=view.findChild<QDateTimeEdit*>("dateTimeEdit_2");
    check(begin && end,"export date controls");begin->setTimeSpec(Qt::UTC);end->setTimeSpec(Qt::UTC);
    const auto dates=[&](qint64 first,qint64 last)
    {begin->setDateTime(QDateTime::fromSecsSinceEpoch(first,Qt::UTC));end->setDateTime(QDateTime::fromSecsSinceEpoch(last,Qt::UTC));};
    native_queries.clear();dates(2200000000,2200003600);view.ExportTradeInfo();
    check(native_queries==std::vector<std::string>{"statistics"} && native_values[1]==3 &&
        native_times[0]==2200000000U && native_times[1]==2200003600U,"actual export dates beyond 2038 retain wire values");
    native_queries.clear();dates(-1,100);view.ExportTradeInfo();dates(200,100);view.ExportTradeInfo();
    dates(0,static_cast<qint64>(UINT32_MAX)+1);view.ExportTradeInfo();
    check(native_queries.empty(),"negative/reversed/overflow export dates send nothing");
    dates(0,UINT32_MAX);view.ExportTradeInfo();check(native_times[0]==0 && native_times[1]==UINT32_MAX,"exact unsigned timestamp endpoints accepted");
    native_queries.clear();
    for(const auto count:{0,2})
    {
        selected_count=count;CServerMgr::GetInstance().Init();
        view.ExportTradeInfo();view.MoreTradeInfo();view.SetExchangeLimit();view.MainLoop(tick+hours(5),wall);
        check(native_queries.empty(),"unselected/multiple-peer queries and polling send nothing");
    }
    selected_count=1;CServerMgr::GetInstance().Init();
    native_success=false;view.SetExchangeLimit();
    check(!view.findChild<QLabel*>("exchangeActionStatus")->text().isEmpty(),"query send failure reports retry");native_success=true;
    native_queries.clear();view.MoreTradeInfo();
    check(native_queries==std::vector<std::string>{"statistics"} && native_values[1]==0 &&
        native_times[1]-native_times[0]==86400 && native_times[2]==0 && native_times[3]==0,"previous-day query retains existing window convention");
    std::array<PRICE_DATA,2> prices{{{123,1},{456,2}}};view.UpdatePrice(1,1,1,prices.data());
    auto* sells=view.findChild<QTreeWidget*>("treeWidget");auto* buys=view.findChild<QTreeWidget*>("treeWidget_2");
    native_queries.clear();view.OrderDetail(sells->topLevelItem(0),0);
    check(native_values==std::array<INT32,5>{1,2,123,0,0},"sell detail query keeps type and exact price");
    view.OrderDetail_2(buys->topLevelItem(0),0);check(native_values==std::array<INT32,5>{1,1,456,0,0},"buy detail query keeps type and price");
    const auto queries=native_queries.size();view.OrderDetail(buys->topLevelItem(0),0);view.OrderDetail(nullptr,0);
    QTreeWidgetItem foreign;view.OrderDetail(&foreign,0);
    check(native_queries.size()==queries,"detail actions reject foreign/null items without casts");
    view.OrderSelected(sells->topLevelItem(0),0);view.OrderSelected_2(buys->topLevelItem(0),0);
    check(sells->topLevelItem(0)->isSelected() && buys->topLevelItem(0)->isSelected(),"both order tables support selection");
    auto* combo=view.findChild<QComboBox*>();combo->setCurrentIndex(2);native_success=false;view.ChangeGroup();
    check(sells->topLevelItemCount()==1 && sells->topLevelItem(0)->text(1)=="1.23","failed group request preserves active prices");native_success=true;
    native_callback=[&]{view.SetGroupID(2);PRICE_DATA fresh{200,3};view.UpdatePrice(2,1,0,&fresh);native_callback={};};view.ChangeGroup();
    check(sells->topLevelItemCount()==1 && sells->topLevelItem(0)->text(1)=="2.00","synchronous group reply and fresh prices are not erased after send");
    const auto groups=combo->count();check(!view.ServerLogin(999999) && combo->count()==groups && sells->topLevelItemCount()==1,"invalid server login preserves current view");
    auto& name=g_TLConfig.ServerList.arrGMList[0].groupList[MAX_SERVER_GROUP_COUNT-1].szGroupName;
    const auto saved=std::to_array(name);std::memset(name,'x',sizeof(name));
    check(!view.ServerLogin(1) && sells->topLevelItemCount()==1,"unterminated later group name cannot partially reset current view");std::memcpy(name,saved.data(),saved.size());
    check(view.ServerLogin(1) && sells->topLevelItemCount()==0 && buys->topLevelItemCount()==0,"successful server login resets prior selection and prices");
    view.SetGroupID(1);check(!view.UpdateExchangeHistory(2,0,TRUE,nullptr) && !view.UpdateExchangeHistory(1,-1,TRUE,nullptr) &&
        !view.UpdateExchangeHistory(1,0,2,nullptr) && view.UpdateExchangeHistory(1,0,FALSE,nullptr),"history group/flag/count validation reaches renderer result");
    view.UpdatePrice(1,1,1,prices.data());view.SetGroupID(2);
    check(!sells->topLevelItemCount() && !buys->topLevelItemCount(),"confirmed group switch clears prior price tables");
}
void exchange_actions(const std::filesystem::path& screenshots)
{
    const auto initial=native_calls;
    CExchangeLimit limit(1);
    auto* high=limit.findChild<QDoubleSpinBox*>("doubleSpinBox");
    auto* low=limit.findChild<QDoubleSpinBox*>("doubleSpinBox_2");
    auto* single=limit.findChild<QSpinBox*>("spinBox");
    auto* daily=limit.findChild<QSpinBox*>("spinBox_2");
    check(high && low && single && daily,"limit input controls");
    limit.ShowExchangeLimit(INT32_MAX,1,12,34);
    check(limit.findChild<QLabel*>("label_5")->text()=="21474836.47" &&
        limit.findChild<QLabel*>("label_6")->text()=="0.01","limit display preserves complete hundredths");
    limit.setLocale(QLocale(QLocale::German));
    high->setValue(1.15);low->setValue(0.29);single->setValue(12);daily->setValue(34);
    limit.show();flush();int finished=0;
    QObject::connect(&limit,&QDialog::finished,[&](int){++finished;});
    native_success=false;native_callback=[&]{limit.Modify();};limit.Modify();native_callback={};
    check(native_calls==initial+1 && limit.isVisible() && finished==0,"failed limit submission stays open and blocks reentrant submit");
    check(native_values==std::array<INT32,5>{1,115,29,12,34},"locale-independent rounded limit values");
    check(!limit.findChild<QLabel*>("exchangeActionStatus")->text().isEmpty(),"failed limit send shows retry message");
    limit.resize(500,360);if(auto* focus=QApplication::focusWidget())focus->clearFocus();flush();
    auto* status=limit.findChild<QLabel*>("exchangeActionStatus");
    check(status->width()>=status->fontMetrics().horizontalAdvance(status->text()),"retry message fits dialog width");
    check(limit.grab().save(QString::fromStdWString((screenshots/"gm-limit-retry-validation.png").wstring())),"limit retry screenshot");
    native_success=true;limit.Modify();check(finished==1 && !limit.isVisible(),"successful retry closes limit exactly once");
    high->setMaximum(1e10);high->setValue(21474836.48);limit.show();limit.Modify();
    check(native_calls==initial+2 && limit.isVisible(),"unrepresentable limit price never sent");limit.Exit();
    CExchangeLimit invalid_group(0);invalid_group.Modify();check(native_calls==initial+2,"invalid limit group rejected");
    QPointer<CExchangeLimit> deleted_limit=new CExchangeLimit(1);
    native_callback=[&]{delete deleted_limit.data();};deleted_limit->Modify();native_callback={};
    check(!deleted_limit && native_calls==initial+3,"limit can be destroyed during send callback");
    CLDWidget widget;widget.SetGroupID(1);
    auto* price=widget.findChild<QDoubleSpinBox*>("doubleSpinBox");
    auto* quantity=widget.findChild<QLineEdit*>("lineEdit");
    check(price && quantity,"add-order inputs");price->setValue(0.29);
    for(const auto* bad:{"","text","0","-1","2147483648"}) {quantity->setText(bad);widget.AddOrder();}
    check(native_calls==initial+3,"invalid order quantities send nothing");
    quantity->setText("7");answer_exchange(false,[&]
    {
        widget.AddOrder();
        check(QApplication::activeModalWidget()->grab().save(QString::fromStdWString((screenshots/"gm-exchange-confirmation.png").wstring())),"exchange confirmation screenshot");
    });widget.AddOrder();
    check(native_calls==initial+3,"cancel and reentrant confirmation send nothing");
    answer_exchange(true,[&]{widget.SetGroupID(2);});widget.AddOrder();
    check(native_calls==initial+3,"changed group invalidates pending confirmation");widget.SetGroupID(1);
    answer_exchange(true,[&]{selected_session=18;});widget.AddOrder();
    check(native_calls==initial+3,"changed peer invalidates same-group confirmation");selected_session=17;
    answer_exchange(true,[&]{quantity->setText("99");price->setValue(1.15);});widget.AddOrder();
    check(native_calls==initial+4 && native_values[0]==1 && native_values[1]==29 && native_values[2]==7,"confirmed order uses captured price/count");
    native_success=false;answer_exchange(true);widget.AddOrder();
    check(native_calls==initial+5 && !widget.findChild<QLabel*>("exchangeActionStatus")->text().isEmpty(),"failed add-order send reports retry");native_success=true;
    QPointer<CLDWidget> deleted_widget=new CLDWidget;deleted_widget->SetGroupID(1);
    deleted_widget->findChild<QLineEdit*>("lineEdit")->setText("1");
    answer_exchange(true,[&]{delete deleted_widget.data();});deleted_widget->AddOrder();
    check(!deleted_widget && native_calls==initial+5,"parent destruction during confirmation sends nothing");
    CDetailDialog detail(1);EXCHANGE_COUNT_DATA row{};row.qwOrderID=UINT64_MAX;
    detail.ShowData(QStringLiteral("orders"),std::span(&row,1));
    auto* tree=detail.findChild<QTreeWidget*>();detail.CancelSystemOrder();
    check(native_calls==initial+5,"cancel-order requires selection before confirmation");
    tree->setCurrentItem(tree->topLevelItem(0));answer_exchange(false);detail.CancelSystemOrder();
    check(native_calls==initial+5,"cancel-order confirmation can be rejected");
    answer_exchange(true,[&]{detail.CancelSystemOrder();row.qwOrderID=42;detail.ShowData(QStringLiteral("new"),std::span(&row,1));});
    detail.CancelSystemOrder();
    check(native_calls==initial+6 && native_order==UINT64_MAX,"cancel uses captured full-width order even when table changes");
    tree->setCurrentItem(tree->topLevelItem(0));native_success=false;answer_exchange(true);detail.CancelSystemOrder();
    check(native_calls==initial+7 && native_order==42 && !detail.findChild<QLabel*>("exchangeActionStatus")->text().isEmpty(),"failed cancel send reports retry");native_success=true;
    QPointer<CDetailDialog> deleted_detail=new CDetailDialog(1);
    deleted_detail->ShowData(QStringLiteral("orders"),std::span(&row,1));
    tree=deleted_detail->findChild<QTreeWidget*>();tree->setCurrentItem(tree->topLevelItem(0));
    answer_exchange(true,[&]{delete deleted_detail.data();});deleted_detail->CancelSystemOrder();
    check(!deleted_detail && native_calls==initial+7,"detail destruction during confirmation sends nothing");
    CDetailDialog closed_detail(1);closed_detail.ShowData(QStringLiteral("orders"),std::span(&row,1));
    tree=closed_detail.findChild<QTreeWidget*>();tree->setCurrentItem(tree->topLevelItem(0));
    answer_exchange(true,[&]{closed_detail.Exit();});closed_detail.CancelSystemOrder();
    check(native_calls==initial+7,"closed detail invalidates open confirmation even before destruction");
    CExchangeLimit closed_limit(1);int close_count=0;
    QObject::connect(&closed_limit,&QDialog::finished,[&](int){++close_count;});
    native_callback=[&]{closed_limit.Exit();};closed_limit.Modify();native_callback={};
    check(close_count==1 && native_calls==initial+8,"callback closure is not completed twice");
    const auto before_reconnect=native_calls;
    const auto reconnect=[] { CSessionMgr::GetInstance().GetGMSession(selected_session)->connection_lifetime=std::make_shared<const char>(0); };
    answer_exchange(true,reconnect);widget.AddOrder();
    check(native_calls==before_reconnect,"reused socket invalidates exchange confirmation");
    CExchangeLimit stale_limit(1);CDetailDialog stale_detail(1);
    stale_detail.ShowData(QStringLiteral("orders"),std::span(&row,1));
    auto* stale_tree=stale_detail.findChild<QTreeWidget*>();stale_tree->setCurrentItem(stale_tree->topLevelItem(0));
    reconnect();stale_limit.Modify();stale_detail.CancelSystemOrder();
    check(native_calls==before_reconnect,"cached exchange dialogs cannot send through replacement connection");
    native_queries.clear();native_callback=[&]{reconnect();native_callback={};};
    check(!widget.MainLoop(std::chrono::steady_clock::now()+std::chrono::hours(12),std::chrono::system_clock::now()) &&
        native_queries==std::vector<std::string>{"log"},"exchange polling stops after connection identity changes inside send");

}
void wheel(CExchangeHistoryUI& view,int direction,int count)
{
    for (int index=0;index<count;++index)
    {
        QWheelEvent event(QPointF(20,20),QPointF(20,20),QPoint(),QPoint(0,direction*120),
            Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);
        QApplication::sendEvent(&view,&event);
    }
}
void exchange_tables()
{
    CLDWidget view;
    check(view.SetGroupID(1),"table fixture group");
    auto* sells=view.findChild<QTreeWidget*>("treeWidget");
    auto* buys=view.findChild<QTreeWidget*>("treeWidget_2");
    auto* all=view.findChild<QTreeWidget*>("treeWidget_3");
    auto* system=view.findChild<QTreeWidget*>("treeWidget_4");
    auto* player=view.findChild<QTreeWidget*>("treeWidget_5");
    check(sells && buys && all && system && player,"named exchange tables exist");
    std::array<PRICE_DATA,3> prices{{{123,2},{INT32_MAX,3},{INT32_MIN,4}}};
    check(view.UpdatePrice(1,2,1,prices.data()),"replace price tables");
    check(sells->topLevelItemCount()==2 && buys->topLevelItemCount()==1,"sell and buy table sizes");
    check(sells->topLevelItem(0)->text(1)=="21474836.47" && sells->topLevelItem(1)->text(1)=="1.23" &&
        buys->topLevelItem(0)->text(1)=="-21474836.48","exact full-width prices and reverse sell ordering");
    check(sells->topLevelItem(0)->text(0).endsWith("2") && buys->topLevelItem(0)->text(0).endsWith("1"),"legacy order ranks preserved");
    check(!view.UpdatePrice(2,0,0,nullptr) && sells->topLevelItemCount()==2,"old-group reply preserves active prices");
    check(!view.UpdatePrice(1,-1,1,prices.data()) && !view.UpdatePrice(1,INT32_MAX,INT32_MAX,prices.data()) &&
        !view.UpdatePrice(1,40,1,prices.data()) && !view.UpdatePrice(1,1,0,nullptr),"price count and pointer bounds");
    check(sells->topLevelItem(0)->text(1)=="21474836.47","invalid price replies preserve current items");
    prices[1].nPrice=0;check(sells->topLevelItem(0)->text(1)=="21474836.47","price items own rendered values");
    std::array<PRICE_DATA,40> full{};check(view.UpdatePrice(1,20,20,full.data()) && sells->topLevelItemCount()==20 && buys->topLevelItemCount()==20,"maximum price reply");
    check(view.UpdatePrice(1,0,0,nullptr) && !sells->topLevelItemCount() && !buys->topLevelItemCount(),"empty price reply clears both sides");
    std::array<GM_EXCHANGE_PRICE_LOG,2> rows{};
    for (auto& row:rows) std::memcpy(row.szExchangeTime,"2026-01-02 03:04:00",20);
    rows[0].nPrice=INT32_MAX;rows[0].nCount=3;
    rows[1].nPrice=INT32_MIN;rows[1].nCount=4;rows[1].dwSellerID=42;
    std::vector<char> unaligned(1+sizeof(rows));std::memcpy(unaligned.data()+1,rows.data(),sizeof(rows));
    check(view.UpdateExchangeLog(1,2,TRUE,unaligned.data()+1),"unaligned log bytes copied into owned records");
    check(all->topLevelItemCount()==2 && system->topLevelItemCount()==1 && player->topLevelItemCount()==1,"logs partition into all/system/player tables");
    check(all->topLevelItem(0)->text(1)=="21474836.47" && player->topLevelItem(0)->text(1)=="-21474836.48" &&
        system->topLevelItem(0)->text(3)=="S" && player->topLevelItem(0)->text(3)=="P","exact log prices and seller labels");
    check(view.UpdateExchangeLog(1,2,FALSE,unaligned.data()+1) && all->topLevelItemCount()==4,"incremental log reply appends");
    std::memset(rows[1].szExchangeTime,'x',sizeof(rows[1].szExchangeTime));
    check(!view.UpdateExchangeLog(1,2,TRUE,reinterpret_cast<const char*>(rows.data())) && all->topLevelItemCount()==4,"invalid later timestamp preserves complete prior tables");
    check(!view.UpdateExchangeLog(2,0,TRUE,nullptr) && !view.UpdateExchangeLog(1,-1,TRUE,nullptr) &&
        !view.UpdateExchangeLog(1,MAX_EXCHANGE_LOG_NUM+1,TRUE,nullptr) && !view.UpdateExchangeLog(1,1,TRUE,nullptr) &&
        !view.UpdateExchangeLog(1,0,2,nullptr),"log group/count/flag/pointer validation");
    check(all->topLevelItemCount()==4 && system->topLevelItemCount()==2 && player->topLevelItemCount()==2,"invalid replies leave every table unchanged");
    check(view.UpdateExchangeLog(1,0,FALSE,nullptr) && all->topLevelItemCount()==4,"empty incremental reply retains log rows");
    check(view.UpdateExchangeLog(1,0,TRUE,nullptr) && !all->topLevelItemCount() && !system->topLevelItemCount() && !player->topLevelItemCount(),"empty replacement clears every log table");
}
void history_rendering(const std::filesystem::path& screenshot)
{
    std::vector<GM_EXCHANGE_PRICE_HISTORY> rows(96);
    for (std::size_t index=0;index<rows.size();++index)
    {
        auto& row=rows[index];row.nOpen=1000+static_cast<INT32>(index%30)*5;
        row.nClose=row.nOpen+(index%3==0 ? 20 : index%3==1 ? -15 : 0);
        row.nLow=std::min(row.nOpen,row.nClose)-10;row.nHigh=std::max(row.nOpen,row.nClose)+15;
        row.nVolume=static_cast<INT32>(index%9)*20+10;
        const auto time=std::format("2026-01-{} {:02}:00:00",2+index/24,index%24);
        std::memcpy(row.szHistoryTime,time.c_str(),time.size()+1);
    }
    CExchangeHistoryUI view;view.resize(800,480);
    check(view.SetData(static_cast<INT32>(rows.size()),reinterpret_cast<const char*>(rows.data())),"normal history staged");
    const auto original=view.grab().toImage();check(!original.isNull(),"normal candle/volume chart rendered");
    check(original.save(QString::fromStdWString(screenshot.wstring())),"history chart screenshot");
    QMouseEvent hover(QEvent::MouseMove,QPointF(5,150),Qt::NoButton,Qt::NoButton,Qt::NoModifier);
    QApplication::sendEvent(&view,&hover);check(view.grab().toImage()!=original,"hover maps to current visible record");
    QEvent leave(QEvent::Leave);QApplication::sendEvent(&view,&leave);
    check(view.grab().toImage()==original,"leaving chart clears hover without changing data");
    wheel(view,1,200);const auto maximum=view.grab().toImage();wheel(view,1,20);
    check(view.grab().toImage()==maximum,"zoom upper bound saturates");
    wheel(view,-1,200);const auto minimum=view.grab().toImage();wheel(view,-1,20);
    check(view.grab().toImage()==minimum,"zoom lower bound saturates");
    for (const QSize size : {QSize(1,1),QSize(20,20),QSize(100,234),QSize(220,235),QSize(800,480)})
    {
        view.resize(size);check(view.size()==size,"fixture uses actual requested viewport");
        check(view.grab().size()==size,"tiny and threshold viewport renders safely");
        QMouseEvent outside(QEvent::MouseMove,QPointF(-1,-1),Qt::NoButton,Qt::NoButton,Qt::NoModifier);
        QApplication::sendEvent(&view,&outside);view.grab();
    }
    auto extreme=rows;extreme.front().nVolume=INT32_MAX;extreme.front().nHigh=INT32_MAX;
    CExchangeHistoryUI previous;previous.resize(1600,480);
    check(previous.SetData(static_cast<INT32>(extreme.size()),reinterpret_cast<const char*>(extreme.data())),"history with old extreme row");
    previous.grab();previous.resize(800,480);wheel(previous,1,200);
    CExchangeHistoryUI fresh;fresh.resize(800,480);wheel(fresh,1,200);
    check(fresh.SetData(10,reinterpret_cast<const char*>(rows.data()+rows.size()-10)),"fresh visible tail");
    check(previous.grab().toImage()==fresh.grab().toImage(),"visible extrema recalculated after resize/zoom");
    for (auto& row : extreme)
    {row.nLow=INT32_MIN;row.nHigh=INT32_MAX;row.nOpen=INT32_MIN;row.nClose=INT32_MAX;row.nVolume=INT32_MAX;}
    check(view.SetData(static_cast<INT32>(extreme.size()),reinterpret_cast<const char*>(extreme.data())) && !view.grab().isNull(),"full signed price and maximum volume render without narrow overflow");
    for (auto& row : extreme) {row.nOpen=row.nClose=row.nHigh=row.nLow=0;row.nVolume=0;}
    check(view.SetData(static_cast<INT32>(extreme.size()),reinterpret_cast<const char*>(extreme.data())) && !view.grab().isNull(),"flat zero price and volume ranges");
    for (auto& row : extreme) {row.nOpen=row.nClose=row.nHigh=row.nLow=INT32_MIN;row.nVolume=INT32_MIN;}
    check(view.SetData(static_cast<INT32>(extreme.size()),reinterpret_cast<const char*>(extreme.data())) && !view.grab().isNull(),"negative extreme inputs remain bounded");
    check(view.UnInit() && !view.grab().isNull(),"reset clears owned records and rendering state");
}
}




CLDWidget* gm::exchange_reply_view(){return reply_view.data();}
void gm::exchange_reply_log(std::string_view text)
{
    reply_logs.emplace_back(text);
    const auto callback=reply_log_callback;if(callback)callback();
}
void CMessageHandler::OnAllPing(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMRegisterAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMVerifyAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMLoginAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMItemListNtf(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMLogoutAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMPreQueryAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryRoleID(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMForbidChatAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMFreezeRoleAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMKickRoleAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMSendMailAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMAnnouceAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMGetAnnounceAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMDelAnnounce(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMLogAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMExecuteGMStrAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMLoadRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMModifyRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryNameToIDAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMErrorStr(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMRoleExeGMCmd(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnAccepted(ISocketAcceptor*,ISocketSession*){}
void CMessageHandler::OnConnected(ISocketSession*){}

void CMessageHandler::OnDisconnected(ISocketSession*,BOOL){}
BOOL CMessageHandler::DoSelectGroup(INT32 group){native_queries.emplace_back("group");native_values[0]=group;return send_result();}
BOOL CMessageHandler::DoQueryExchangeLog(INT32 group){native_queries.emplace_back("log");native_values[0]=group;return send_result();}
BOOL CMessageHandler::DoQueryExchangeHistory(INT32 group){native_queries.emplace_back("history");native_values[0]=group;return send_result();}
BOOL CMessageHandler::DoQueryOrderDetail(INT32 group,UINT8 type,INT32 price){native_queries.emplace_back("detail");native_values={group,type,price,0,0};return send_result();}
BOOL CMessageHandler::DoAddSystemOrder(INT32 group,INT32 price,INT32 count){native_values={group,price,count,0,0};return send_result();}
BOOL CMessageHandler::DoDelSystemOrder(INT32 group,UINT64 order){native_values[0]=group;native_order=order;return send_result();}
BOOL CMessageHandler::DoQuerySysExchangeInfo(INT32 group,INT32 flag,UINT32 begin,UINT32 end,UINT32 hour_begin,UINT32 hour_end){native_queries.emplace_back("statistics");native_values={group,flag,0,0,0};native_times={begin,end,hour_begin,hour_end};return send_result();}
BOOL CMessageHandler::DoQueryExchangeLimit(INT32 group){native_queries.emplace_back("limit");native_values[0]=group;return send_result();}
BOOL CMessageHandler::DoSetExchangeLimit(INT32 group,INT32 high,INT32 low,INT32 single,INT32 daily){native_values={group,high,low,single,daily};return send_result();}
CSessionMgr CSessionMgr::m_Instance;
CSessionMgr::CSessionMgr()=default;
CSessionMgr::~CSessionMgr()=default;
CSessionMgr& CSessionMgr::GetInstance(){return m_Instance;}
GM_SESSION* CSessionMgr::GetGMSession(UINT32 id)
{
    for (const auto& [socket,session]:reply_sessions)
        if (session->dwSessionID==id) return session;
    static ReplySocket selected_socket;
    selected_socket.session.dwSessionID=selected_session;
    return id==selected_session ? &selected_socket.session : nullptr;
}
GM_SESSION* CSessionMgr::Find(ISocketSession* socket)
{
    const auto found=reply_sessions.find(socket);
    return found!=reply_sessions.end() && found->second->pSocketSession==socket ? found->second : nullptr;
}
CServerMgr CServerMgr::m_Instance;
CServerMgr& CServerMgr::GetInstance(){return m_Instance;}
BOOL CServerMgr::Init(){m_nSelectedSessionNum=selected_count;return TRUE;}
UINT32 CServerMgr::GetSessionIDByIndex(INT32 index){return index==0 ? selected_session : UINT32(-1);}

int main(int argc,char** argv)
{
    QApplication app(argc,argv);
    CLog::GetInstance().SetLevelMask(0);
    try
    {
        check(argc==2,"screenshot path argument");
        check(QFontDatabase::addApplicationFont("C:/Windows/Fonts/msyh.ttc")>=0,"Chinese fixture font available");
        app.setFont(QFont("Microsoft YaHei",9));
        CServerMgr::GetInstance().Init();g_TLConfig.ServerList.arrGMList[0].nSvrID=1;
        std::memcpy(g_TLConfig.ServerList.arrGMList[0].groupList[1].szGroupName,"Test group",11);
        CLDWidget widget;widget.resize(1200,900);
        check(widget.DelDetail(),"detail close safe before first detail exists");widget.ClearExchangeLimit();
        check(widget.ServerLogin(1) && widget.SetGroupID(1),"actual exchange widget initialized");
        check(!widget.SetGroupID(-1) && !widget.SetGroupID(MAX_SERVER_GROUP_COUNT),"group bounds");
        auto* summary=widget.findChild<QLabel*>("label");check(summary,"statistics label");
        check(widget.ShowTodayStat(1,123456,987654,55555,77777,12345,-6789,1234,5678),"today statistics");
        check(summary->text().contains(QString::fromUtf8("\xE7\xB3\xBB\xE7\xBB\x9F\xE6\x8A\x95\xE6\x94\xBE\xE9\x87\x8F")) && summary->text().contains("123.45"),"GBK labels decode correctly on desktop and amounts display");
        const auto original=summary->text();check(!widget.ShowTodayStat(2,1,2,3,4,5,6,7,8) && summary->text()==original,"wrong-group statistics preserve current view");
        for (auto* edit : widget.findChildren<QDateTimeEdit*>()) edit->setDateTime(QDateTime(QDate(2026,1,2),QTime(3,4)));
        widget.show();flush();if(auto* focus=app.focusWidget())focus->clearFocus();flush();
        check(summary->height()>=summary->heightForWidth(summary->width()),"complete statistics text fits the label");
        check(widget.grab().save(argv[1]),"actual exchange statistics screenshot");
        {
            const auto output=std::filesystem::absolute(argv[1]).parent_path()/"exchange ui";
            std::filesystem::create_directories(output/"run");
            struct Restore
            {
                std::filesystem::path path=std::filesystem::current_path();
                ~Restore(){std::error_code ignored;std::filesystem::current_path(path,ignored);}
            } restore;
            std::filesystem::current_path(output/"run");
            const auto begin=QDateTime(QDate(2026,1,2),QTime(3,4)).toTime_t();
            const auto end=QDateTime(QDate(2026,1,3),QTime(5,6)).toTime_t();
            const auto report=gm::exchange_export(begin,end,{123456,987654,55555,77777,12345,-6789,1234,5678});
            check(widget.OutputStatToFile(begin,end,1,123456,987654,55555,77777,12345,-6789,1234,5678),"actual widget export in disposable working directory");
            std::ifstream file(std::filesystem::path("..")/report->filename,std::ios::binary);
            const std::string bytes(std::istreambuf_iterator<char>(file),{});
            check(bytes==report->bytes,"widget export preserves argument mapping and complete report bytes");
            check(!widget.OutputStatToFile(begin,end,2,1,2,3,4,5,6,7,8),"wrong-group export rejected");
        }
        check(widget.ShowYesterdayStat(1,10000,20000,30000,40000,500,600,700,800),"comparison dialog");
        auto* info=widget.findChild<CGMItemInfo*>();check(info && info->testAttribute(Qt::WA_DeleteOnClose),"comparison dialog has scoped Qt ownership");
        QPointer<CGMItemInfo> info_guard=info;info->Exit();flush();check(!info_guard,"closed comparison dialog destroyed");
        EXCHANGE_COUNT_DATA row{};row.dwRoleID=42;row.qwOrderID=UINT64_MAX;row.nCount=3;std::memcpy(row.szRoleName,"role",5);
        check(widget.UpdateOrderDetail(1,2,1234,1,&row),"order detail uses owned row snapshot");
        QPointer<CDetailDialog> first=widget.findChild<CDetailDialog*>();check(first,"detail dialog created");
        auto* tree=first->findChild<QTreeWidget*>();check(tree && tree->topLevelItemCount()==1 && tree->topLevelItem(0)->text(0)=="18446744073709551615","full-width order ID displayed");
        check(tree->columnWidth(0)>=tree->fontMetrics().horizontalAdvance(tree->topLevelItem(0)->text(0)),"complete order ID fits its column");
        row.qwOrderID=0;std::memcpy(row.szRoleName,"edit",5);
        check(tree->topLevelItem(0)->text(1)=="role","caller mutation does not alter rendered detail snapshot");
        check(!widget.UpdateOrderDetail(2,2,1,1,&row) && !widget.UpdateOrderDetail(1,2,1,256,&row) && !widget.UpdateOrderDetail(1,2,1,1,nullptr),"invalid detail input rejected before UI replacement");
        check(first && tree->topLevelItemCount()==1,"invalid details preserve current dialog");
        check(widget.UpdateOrderDetail(1,1,5678,0,nullptr),"empty detail reply is valid");flush();check(!first,"replacement destroys prior detail dialog");
        check(widget.findChildren<CDetailDialog*>().size()==1,"single live detail after replacement");
        auto* second=widget.findChild<CDetailDialog*>();QPointer<CDetailDialog> second_guard=second;
        second->Exit();flush();check(!second_guard && widget.DelDetail(),"manual close clears guarded detail pointer");
        check(widget.ShowExchangeLimit(100,10,20,30),"limit dialog");QPointer<CExchangeLimit> limit=widget.findChild<CExchangeLimit*>();check(limit,"limit created");
        limit->Exit();flush();check(!limit,"closed limit dialog destroyed");
        check(widget.ShowExchangeLimit(200,20,40,60),"limit reopens safely");
        limit=widget.findChild<CExchangeLimit*>();check(widget.SetGroupID(2),"group switch");flush();check(!limit && summary->text().isEmpty(),"group switch retires old limit and resets statistics");
        check(widget.SetGroupID(1) && widget.ShowYesterdayStat(1,0,0,0,0,0,0,0,0),"comparison safe before today's statistics");
        auto* text=widget.findChild<CGMItemInfo*>()->findChild<QTextEdit*>();check(text->toPlainText().contains("0.00"),"reset statistics initialized");
        widget.findChild<CGMItemInfo*>()->Exit();flush();
        auto* history=static_cast<CExchangeHistoryUI*>(widget.findChild<QLabel*>("label_8"));check(history,"named history widget exists");
        check(!history->SetData(-1,nullptr) && !history->SetData(MAX_EXCHANGE_HISTORY_NUM+1,nullptr),"history count bounds");
        GM_EXCHANGE_PRICE_HISTORY sample{};sample.nLow=100;sample.nHigh=200;sample.nOpen=120;sample.nClose=180;sample.nVolume=20;
        std::memcpy(sample.szHistoryTime,"2026-01-02 03:04:00",20);
        check(history->SetData(1,reinterpret_cast<const char*>(&sample)),"owned history row");
        history->resize(500,400);history->grab();const auto chart=history->grab().toImage();
        sample.nHigh=999;sample.nClose=888;
        check(history->grab().toImage()==chart,"history rendering does not borrow caller records");
        std::memset(sample.szHistoryTime,'x',sizeof(sample.szHistoryTime));
        check(!history->SetData(1,reinterpret_cast<const char*>(&sample)) && history->grab().toImage()==chart,"invalid history preserves rendered snapshot");
        check(history->SetData(0,nullptr),"empty history accepted");history->resize(20,20);history->grab();
        history_rendering(std::filesystem::path(argv[1]).parent_path()/"gm-history-validation.png");
        exchange_tables();
        check(widget.UnInit() && native_calls==0,"display and lifecycle fixture performs no native GM operation");
        exchange_actions(std::filesystem::path(argv[1]).parent_path());
        exchange_queries();
        exchange_replies(std::filesystem::path(argv[1]).parent_path());
        logging_view(std::filesystem::path(argv[1]).parent_path());
        std::cout<<checks<<" GM exchange-widget checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
