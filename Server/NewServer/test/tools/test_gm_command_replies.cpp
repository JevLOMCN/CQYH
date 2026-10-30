#include "gm_application_fixture.h"
#include "gm_role_fixture.h"
#include "gm_data_packets.h"
#include "gm_command_reply.h"
#include "gm_ui.h"
#include "server_mgr.h"
#include "role_info.h"
#include <QtWidgets/QStatusBar>
#include <limits>
#include <fstream>

namespace
{
void select(gmtool& window, bool multiple=false)
{
    auto* tree=window.findChild<QTreeWidget*>("treeWidget_SvrList");
    for(int index=0;index<2;++index)
    {
        CSessionMgr::GetInstance().GetGMSession(index)->eState=GM_SVR_STATE_LOGIN;
        tree->topLevelItem(index)->setCheckState(0,index==0 || multiple ? Qt::Checked : Qt::Unchecked);
    }
    CServerMgr::GetInstance().UpdateSelectServer();
}
void raw(Socket& socket,const std::vector<char>& bytes)
{
    auto* packet=socket.state->allocator->Alloc(static_cast<INT32>(bytes.size()));
    std::memcpy(packet,bytes.data(),bytes.size());
    CMessageHandler::GetInstance().OnRecv(&socket,packet,static_cast<UINT32>(bytes.size()));
}
std::vector<char> keys()
{
    GM2TL_QUERY_ROLE_ID_ACK header;
    std::memset(&header,0,sizeof(header));header.wMsg=gm2tl_query_role_id_ack;header.nCount=2;header.nAreaID=7;
    std::array<GM_ROLE_KEY_INFO,2> rows{};
    rows[0].dwRoleID=4294967295;rows[1].dwRoleID=42;
    std::memcpy(rows[0].szRoleName,"first",6);std::memcpy(rows[1].szRoleName,"second",7);
    for(auto& row:rows)std::memcpy(row.szUserName,"account",8);
    std::vector<char> bytes(sizeof(header)+sizeof(rows));
    std::memcpy(bytes.data(),&header,sizeof(header));std::memcpy(bytes.data()+sizeof(header),rows.data(),sizeof(rows));return bytes;
}
std::vector<char> announcements(INT32 group,bool empty=false)
{
    GM2TL_GET_ANNOUNCE_LIST_ACK header;
    std::memset(&header,0,sizeof(header));header.wMsg=gm2tl_get_announce_list_ack;header.nGroupID=group;header.nCount=empty ? 0 : 1;
    GMTL_ANNOUNCE_DATA row{};row.nGroupID=group;row.dwSerial=4294967295;row.dwIntervalTime=4294967295;row.dwRepeatNum=0;
    const auto text=QStringLiteral("公告 <b>fixture</b>").toUtf8();std::memcpy(row.szText,text.constData(),text.size()+1);
    std::vector<char> bytes(sizeof(header)+(empty ? 0 : sizeof(row)));std::memcpy(bytes.data(),&header,sizeof(header));
    if(!empty)std::memcpy(bytes.data()+sizeof(header),&row,sizeof(row));return bytes;
}
template<class T> std::vector<char> role(UINT16 message)
{
    T header{};header.wMsg=message;header.dwRoleID=4294967295;header.nAreaID=7;
    const auto document=role_fixture::sample();header.nLength=static_cast<INT32>(document.size());
    const auto prefix=offsetof(T,szData);std::vector<char> bytes(prefix+document.size());
    std::memcpy(bytes.data(),&header,prefix);std::memcpy(bytes.data()+prefix,document.data(),document.size());return bytes;
}
void simple(gmtool& window)
{
    auto& manager=CCmdManager::GetInstance();select(window,true);
    check(manager.SendGMStrCmd("fixture"),"broadcast string command starts");const auto command=manager.GetCurrentCmd();
    const auto ack=message<GM2TL_EXE_GM_STR_ACK>(gm2tl_exe_gm_str_ack);
    deliver(*current->net->sockets[0],ack);deliver(*current->net->sockets[0],ack);
    check(command->nSvrNum==1 && command->replies.at(0).done && !command->replies.at(1).done,"duplicate peer ACK cannot finish another peer");
    Socket orphan(current);orphan.data=reinterpret_cast<void*>(1);deliver(orphan,ack);
    check(command->nSvrNum==1,"unowned socket cannot acknowledge a command");
    deliver(*current->net->sockets[1],ack);check(command->nSvrNum==0 && command->in_flight==0,"both original peers complete exactly once");manager.MainLoop();
    check(!manager.GetCurrentCmd(),"completed command retires after callbacks return");
    current->send_hook=[&](Socket& socket,UINT16 id)
    {
        if(id!=tl2gm_exe_gm_str)return;
        if(&socket==current->net->sockets[0].get())deliver(*current->net->sockets[1],ack);
        deliver(socket,ack);
    };
    check(manager.SendGMStrCmd("fixture") && manager.GetCurrentCmd()->nSvrNum==0,"future peer ACK ignored until its own send begins");
    current->send_hook={};manager.MainLoop();
    current->send_hook=[](Socket& socket,UINT16 id)
    {if(id==tl2gm_exe_gm_str)current->accept=&socket!=current->net->sockets[0].get();};
    check(manager.SendGMStrCmd("partial") && manager.GetCurrentCmd()->nSvrNum==1,"failed peer excluded from partial broadcast reply count");
    current->send_hook={};deliver(*current->net->sockets[0],ack);
    check(manager.GetCurrentCmd()->nSvrNum==1,"rejected native send cannot consume an accepted peer reply");
    deliver(*current->net->sockets[1],ack);manager.MainLoop();select(window);
    check(manager.SendAnnounce(1,0,1,1,"fixture"),"announcement starts");auto announce=message<GM2TL_ANNOUNCE_ACK>(gm2tl_announce_ack);announce.nGroupID=2;
    deliver(*current->net->sockets[0],announce);check(manager.GetCurrentCmd()->nSvrNum==1,"wrong announcement group ignored");
    announce.nGroupID=1;announce.nResult=42;deliver(*current->net->sockets[0],announce);
    check(manager.GetCurrentCmd()->nSvrNum==0,"announcement error consumes only matching reply");manager.MainLoop();
    gm::RoleEdits edits;check(edits.set_signed(GM_RBD_LEVEL,1).has_value(),"owned modification payload staged");
    check(manager.ModifyRole(42,42,static_cast<INT32>(edits.bytes().size()),edits.bytes().data(),"fixture"),"modification starts");
    auto modified=message<GM2TL_MODIFY_ROLE_DATA_ACK>(gm2tl_modify_role_data_ack);modified.dwRoleID=43;
    deliver(*current->net->sockets[0],modified);check(manager.GetCurrentCmd()->nSvrNum==1,"wrong modified role ignored");
    modified.dwRoleID=42;modified.nType=std::numeric_limits<INT32>::min();modified.dwID1=4294967295;deliver(*current->net->sockets[0],modified);
    check(manager.GetCurrentCmd()->nSvrNum==0,"unused legacy modify fields do not affect matching ACK");manager.MainLoop();
    auto exe=std::static_pointer_cast<EXE_ROLE_CMD>(manager.NewCmd(CMD_EXE_ROLE_GM_CMD));exe->dwRoleID=42;
    std::memcpy(exe->szGM,"fixture",8);std::memcpy(exe->szReason,"fixture",8);
    check(manager.ExeRoleGMCmd(exe),"role command starts");auto executed=message<GM2TL_ROLE_EXE_GM_CMD>(gm2tl_role_exe_gm_cmd);executed.dwRoleID=41;
    deliver(*current->net->sockets[0],executed);check(exe->nSvrNum==1,"wrong role-command role ignored");
    executed.dwRoleID=42;deliver(*current->net->sockets[0],executed);deliver(*current->net->sockets[0],executed);
    check(exe->nSvrNum==0,"role-command ACK consumed once");manager.MainLoop();
}
}

int main(int argc,char** argv)
{
    try
    {
        WSADATA winsock{};if(WSAStartup(MAKEWORD(2,2),&winsock)!=0)throw std::runtime_error("Winsock initialization failed");
        struct WinsockCleanup{~WinsockCleanup(){WSACleanup();}} winsock_cleanup;
        CLog::GetInstance().SetLevelMask(0);
        QApplication application(argc,argv);application.setQuitOnLastWindowClosed(false);
        check(argc==2,"expected screenshot output path");
        check(QFontDatabase::addApplicationFont("C:/Windows/Fonts/msyh.ttc")>=0,"Chinese fixture font available");application.setFont(QFont(QStringLiteral("Microsoft YaHei"),9));
        const auto previous_directory=std::filesystem::current_path();
        const auto work_directory=std::filesystem::absolute(argv[1]).parent_path()/"gm-command-replies"/"working";
        std::filesystem::create_directories(work_directory);
        struct RestoreDirectory
        {std::filesystem::path previous;~RestoreDirectory(){std::error_code error;std::filesystem::current_path(previous,error);}} restore_directory{previous_directory};
        std::filesystem::current_path(work_directory);
        struct Stop{~Stop(){CGMManager::GetInstance().UnInit();}} stop_runtime;
        {
        gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);
        simple(window);
        auto& manager=CCmdManager::GetInstance();auto& handler=CMessageHandler::GetInstance();auto* socket=current->net->sockets[0].get();
        std::map<INT32,ROLE_NAME_DATA> names;
        ROLE_NAME_DATA first{},last{};std::memcpy(first.szRoleName,"one",4);std::memcpy(last.szRoleName,"two",4);
        names.emplace(1,first);names.emplace(std::numeric_limits<INT32>::max(),last);
        check(manager.QueryNameToID(names),"actual name-query callback starts");
        auto named=message<GM2TL_QUERY_NAME_TO_ID_ACK>(gm2tl_query_name_to_id_ack);named.nIndex=2;named.dwRoleID=42;deliver(*socket,named);
        check(manager.GetCurrentCmd()->nSvrNum==1,"wrong name-query index cannot consume matching peer");named.nIndex=1;deliver(*socket,named);deliver(*socket,named);
        check(manager.GetCurrentCmd()->nSvrNum==0 && manager.MainLoop() && manager.GetCurrentCmd()->nSvrNum==1,
            "owned name reply defers the sparse-index next request until delivery returns");
        named.nIndex=std::numeric_limits<INT32>::max();named.dwRoleID=4294967295;deliver(*socket,named);
        std::ifstream report("../nameTOid.txt",std::ios::binary);const std::string report_text{std::istreambuf_iterator<char>(report),{}};
        check(!manager.GetCurrentCmd() && report_text=="one  42\r\ntwo  4294967295\r\n","actual name-query callback retires owner and publishes exact report bytes");
        GM2TL_ITEM_LIST_NTF item_header{};item_header.wMsg=gm2tl_item_list_ntf;item_header.bCLS=TRUE;item_header.nCount=1;
        InternalMessage::ITEM_TEMPLATE_DATA item{};item.dwTempID=7;std::memcpy(item.szName,"fixture",8);
        std::vector<char> items(sizeof(item_header)+sizeof(item));std::memcpy(items.data(),&item_header,sizeof(item_header));std::memcpy(items.data()+sizeof(item_header),&item,sizeof(item));
        raw(*socket,items);check(CDataMgr::GetInstance().GetItems(0).size()==1,"actual item notification publishes owned validated records");
        item.dwTempID=8;std::memcpy(items.data()+sizeof(item_header),&item,sizeof(item));
        auto* item_session=CSessionMgr::GetInstance().GetGMSession(0);item_session->nConnectState=SESSION_DISCONNECTED;raw(*socket,items);item_session->nConnectState=SESSION_CONNECTED;
        check(CDataMgr::GetInstance().GetItems(0).front().dwTempID==7,"closing peer cannot replace item records");
        auto* tree=window.findChild<QTreeWidget*>("treeWidget_queryroleid");
        check(manager.QueryRoleID(0,"fixture"),"role-key query starts");const auto command=manager.GetCurrentCmd();auto bytes=keys();
        bool truncated_rejected=true;
        for(std::size_t size=0;size<bytes.size();++size)
            truncated_rejected=truncated_rejected && !gm::decode_role_keys({reinterpret_cast<const std::byte*>(bytes.data()),size});
        check(truncated_rejected,"every truncated role-key packet is rejected by the production decoder");
        auto unaligned=bytes;unaligned.insert(unaligned.begin(),0);
        check(gm::decode_role_keys({reinterpret_cast<const std::byte*>(unaligned.data()+1),bytes.size()}).has_value(),"unaligned role-key payload decoded without casts");
        auto invalid=bytes;const INT32 excessive=std::numeric_limits<INT32>::max();
        std::memcpy(invalid.data()+offsetof(GM2TL_QUERY_ROLE_ID_ACK,nCount),&excessive,sizeof(excessive));
        check(!gm::decode_role_keys({reinterpret_cast<const std::byte*>(invalid.data()),invalid.size()}),"excessive role-key count rejected before allocation");
        invalid=bytes;invalid[offsetof(GM2TL_QUERY_ROLE_ID_ACK,byIsEnd)]=2;
        check(!gm::decode_role_keys({reinterpret_cast<const std::byte*>(invalid.data()),invalid.size()}),"invalid truncation flag rejected");
        auto malformed=bytes;malformed[sizeof(GM2TL_QUERY_ROLE_ID_ACK)+offsetof(GM_ROLE_KEY_INFO,szRoleName)+ROLE_NAME_LEN-1]='x';
        std::fill_n(malformed.data()+sizeof(GM2TL_QUERY_ROLE_ID_ACK)+offsetof(GM_ROLE_KEY_INFO,szRoleName),ROLE_NAME_LEN,'x');
        check(!gm::decode_role_keys({reinterpret_cast<const std::byte*>(malformed.data()),malformed.size()}),"unterminated role name rejected");
        auto* incoming=current->allocator->Alloc(static_cast<INT32>(bytes.size()));
        std::memcpy(incoming,bytes.data(),bytes.size());
        auto connection=QObject::connect(tree->model(),&QAbstractItemModel::rowsInserted,&window,[&]
        {
            std::fill_n(incoming+sizeof(GM2TL_QUERY_ROLE_ID_ACK)+sizeof(GM_ROLE_KEY_INFO)+offsetof(GM_ROLE_KEY_INFO,szRoleName),ROLE_NAME_LEN,0);
            raw(*socket,bytes);
        });
        handler.OnRecv(socket,incoming,static_cast<UINT32>(bytes.size()));QObject::disconnect(connection);
        check(tree->topLevelItemCount()==2 && tree->topLevelItem(0)->text(1)==QStringLiteral("4294967295") && tree->topLevelItem(1)->text(2)==QStringLiteral("second"),
            "owned role rows survive packet mutation and reentrant duplicate delivery");
        check(command->nSvrNum==0 && command->reply_deliveries==0,"role-key reply releases delivery ownership");manager.MainLoop();
        check(manager.QueryAnnounceList(1),"announcement query starts");raw(*socket,announcements(2));
        check(manager.GetCurrentCmd()->nSvrNum==1,"wrong announcement-list group ignored");raw(*socket,announcements(1));
        auto* announcements_tree=window.findChild<QTreeWidget*>("announceWidget");
        check(CDataMgr::GetInstance().GetAnnounceList().size()==1 && announcements_tree->topLevelItemCount()==1 &&
            announcements_tree->topLevelItem(0)->data(0,Qt::UserRole).toUInt()==4294967295,"announcement records retain complete serial and server identity");manager.MainLoop();
        check(manager.DelAnnounce(1,4294967295u),"full-width deletion serial retained");auto deleted=message<GM2TL_DEL_ANNOUNCE_ACK>(gm2tl_del_announce_ack);deleted.nGroupID=1;deleted.dwAnnounceSerial=7;
        deliver(*socket,deleted);check(manager.GetCurrentCmd()->nSvrNum==1 && !CDataMgr::GetInstance().GetAnnounceList().empty(),"wrong deletion serial ignored");
        deleted.dwAnnounceSerial=4294967295;deliver(*socket,deleted);check(CDataMgr::GetInstance().GetAnnounceList().empty() && announcements_tree->topLevelItemCount()==0,"matching deletion updates owned list and actual Qt tree");manager.MainLoop();
        check(manager.QueryAnnounceList(1),"empty announcement query starts");raw(*socket,announcements(1,true));
        check(manager.GetCurrentCmd()->nSvrNum==0 && window.statusBar()->currentMessage()==QStringLiteral("当前没有公告"),"empty result completes inline without modal dialog");manager.MainLoop();
        auto query=std::static_pointer_cast<QUERY_ROLE_DATA>(manager.NewCmd(CMD_QUERY_ROLE_DATA));query->dwRoleID=4294967295;
        check(manager.QueryRoleData(query),"role-data query starts");const auto document=role<GM2TL_QUERY_ROLE_DATA_ACK>(gm2tl_query_role_data_ack);raw(*socket,document);
        check(query->nSvrNum==0 && CDataMgr::GetInstance().FindRole(4294967295),"matching owned role document publishes and completes");manager.MainLoop();
        auto* role_tree=window.findChild<QTreeWidget*>("treeWidget");
        check(role_tree->topLevelItemCount()==1 && role_tree->topLevelItem(0)->text(2)==QStringLiteral("4294967295") &&
            role_tree->topLevelItem(0)->text(5)==QStringLiteral("离线") && role_tree->topLevelItem(0)->text(7)==QStringLiteral("男"),
            "role display retains unsigned ID and explicit Chinese text encoding");
        window.ClearLog();window.resize(1200,760);window.show();window.findChild<QPushButton*>("loginButton")->setFocus();application.processEvents();
        check(window.grab().save(QString::fromUtf8(argv[1])),"actual result window screenshot written");window.hide();
        stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            auto& manager=CCmdManager::GetInstance();auto* tree=window.findChild<QTreeWidget*>("treeWidget_queryroleid");
            check(manager.QueryRoleID(0,"fixture"),"replacement query starts");const auto original=manager.GetCurrentCmd();bool replaced=false;
            auto connection=QObject::connect(tree->model(),&QAbstractItemModel::rowsInserted,&window,[&]
            {if(!replaced){replaced=true;manager.CmdEnd();manager.SendGMStrCmd("replacement");}});
            raw(*current->net->sockets[0],keys());QObject::disconnect(connection);
            check(replaced && tree->topLevelItemCount()==1 && manager.GetCurrentCmd()!=original &&
                manager.GetCurrentCmd()->Kind()==CMD_EXECUTE_GM_STR && manager.GetCurrentCmd()->nSvrNum==1,
                "UI callback replacement stops remaining rows without changing new reply count");
            check(original->in_flight==0 && original->reply_deliveries==0,"replaced owner released after UI callback");manager.CmdEnd();stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            auto& manager=CCmdManager::GetInstance();check(manager.SendGMStrCmd("fixture"),"socket-binding query starts");const auto command=manager.GetCurrentCmd();
            auto* session=CSessionMgr::GetInstance().GetGMSession(0);auto* original=session->pSocketSession;Socket replacement(current);
            session->pSocketSession=&replacement;
            const auto ack=message<GM2TL_EXE_GM_STR_ACK>(gm2tl_exe_gm_str_ack);deliver(*current->net->sockets[0],ack);deliver(replacement,ack);
            check(command->nSvrNum==1,"old or replacement socket cannot acknowledge a different binding");session->pSocketSession=original;
            deliver(*current->net->sockets[0],ack);check(command->nSvrNum==0,"original unchanged binding can complete");manager.MainLoop();stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            auto& manager=CCmdManager::GetInstance();auto& sessions=CSessionMgr::GetInstance();
            check(manager.SendGMStrCmd("fixture"),"generation-bound command starts");const auto command=manager.GetCurrentCmd();
            current->reuse_connections=true;
            check(sessions.UnInit() && sessions.Init(current->net) && sessions.MainLoop(),"native socket objects rebound under a new session generation");
            for(auto& socket:current->net->sockets)
            {
                CMessageHandler::GetInstance().OnConnected(socket.get());
                auto registration=message<GM2TL_REGISTER_ACK>(gm2tl_register_ack);registration.nResult=errSuccess;deliver(*socket,registration);
            }
            deliver(*current->net->sockets[0],message<GM2TL_EXE_GM_STR_ACK>(gm2tl_exe_gm_str_ack));
            check(command->nSvrNum==1,"same socket address under a new generation cannot acknowledge the old command");manager.CmdEnd();stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            auto& manager=CCmdManager::GetInstance();auto* tree=window.findChild<QTreeWidget*>("treeWidget_queryroleid");
            check(manager.QueryRoleID(0,"fixture"),"shutdown query starts");const auto original=manager.GetCurrentCmd();
            auto connection=QObject::connect(tree->model(),&QAbstractItemModel::rowsInserted,&window,[]{CGMManager::GetInstance().UnInit();});
            raw(*current->net->sockets[0],keys());QObject::disconnect(connection);
            check(!CGMManager::GetInstance().IsRunning() && tree->topLevelItemCount()==1 && original->in_flight==0,
                "UI callback shutdown stops publication and releases retained native resources");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            auto& manager=CCmdManager::GetInstance();auto load=std::static_pointer_cast<LOAD_ROLE_DATA>(manager.NewCmd(CMD_LOAD_ROLE_DATA));load->dwRoleID=4294967295;
            check(manager.LoadRoleData(load),"role editor request starts");bool editor_ready=false,replacement_started=false;
            QTimer::singleShot(0,&window,[&]
            {
                const QPointer<CRoleInfo> editor(g_pRoleInfo);
                editor_ready=editor && !manager.GetCurrentCmd() && load->in_flight>0;
                replacement_started=manager.SendGMStrCmd("replacement")!=FALSE;
                if(editor)editor->Exit();
            });
            raw(*current->net->sockets[0],role<GM2TL_LOAD_ROLE_DATA_ACK>(gm2tl_load_role_data_ack));
            check(editor_ready && replacement_started && !g_pRoleInfo && manager.GetCurrentCmd() &&
                manager.GetCurrentCmd()->Kind()==CMD_EXECUTE_GM_STR && manager.GetCurrentCmd()->nSvrNum==1,
                "modal editor retains old owner while a separate command can start");
            check(load->in_flight==0 && !CDataMgr::GetInstance().FindRole(4294967295),"editor exit releases cached role and old delivery frame");manager.CmdEnd();stop();
        }
        {
            QPointer<gmtool> window=new gmtool;loader login;gm::ApplicationViews views(*window,login);
            struct Delete{QPointer<gmtool> window;~Delete(){delete window.data();}} delete_window{window};
            start(*window);select(*window);auto& manager=CCmdManager::GetInstance();
            auto load=std::static_pointer_cast<LOAD_ROLE_DATA>(manager.NewCmd(CMD_LOAD_ROLE_DATA));load->dwRoleID=4294967295;
            check(manager.LoadRoleData(load),"parent-close role request starts");bool editor_ready=false;
            QTimer::singleShot(0,&login,[&]{editor_ready=g_pRoleInfo && !manager.GetCurrentCmd();window->deleteLater();});
            raw(*current->net->sockets[0],role<GM2TL_LOAD_ROLE_DATA_ACK>(gm2tl_load_role_data_ack));
            check(editor_ready && !window && !g_pRoleInfo && !CGMManager::GetInstance().IsRunning(),
                "parent deletion safely ends nested editor loop and native receive frame");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);
            auto tick=CSessionMgr::Clock::now();auto& sessions=CSessionMgr::GetInstance();
            check(sessions.SetClock([&]{return tick;}),"install reconnect clock");start(window);select(window);
            auto& manager=CCmdManager::GetInstance();
            check(manager.SendGMStrCmd("old binding"),"command starts before reconnect");
            const auto old=manager.GetCurrentCmd();const auto peers=gm::selected_peers(true);
            auto* socket=current->net->sockets[0].get();const auto token=sessions.GetGMSession(0)->connection_lifetime;
            current->reuse_connections=true;sessions.CloseSession(socket);tick+=std::chrono::milliseconds(RECONNECT_TIME);
            check(CGMManager::GetInstance().MainLoop(),"reconnect poll");
            CMessageHandler::GetInstance().OnConnected(socket);
            auto registered=message<GM2TL_REGISTER_ACK>(gm2tl_register_ack);registered.nResult=errSuccess;deliver(*socket,registered);select(window);
            check(sessions.GetGMSession(0)->pSocketSession==socket && sessions.GetGMSession(0)->connection_lifetime!=token,
                "real session manager reuses socket address with new identity");
            check(peers && !gm::selection_matches(*peers,true),"old UI confirmation cannot match reused socket");
            auto ack=message<GM2TL_EXE_GM_STR_ACK>(gm2tl_exe_gm_str_ack);deliver(*socket,ack);
            check(old->nSvrNum==1 && !gm::CommandReply::Begin(CMD_EXECUTE_GM_STR,socket),"old retained command rejects new binding reply");
            manager.CmdEnd();check(manager.SendGMStrCmd("new binding"),"fresh command accepts reconnected peer");deliver(*socket,ack);
            check(manager.GetCurrentCmd()->nSvrNum==0,"new binding reply completes new command");manager.MainLoop();stop();
            check(sessions.SetClock({}),"restore reconnect clock");
        }
        std::cout<<checks<<" GM command-reply checks passed\n";return 0;
    }
    catch(const std::exception& error){CGMManager::GetInstance().UnInit();std::cerr<<error.what()<<'\n';return 1;}
}
