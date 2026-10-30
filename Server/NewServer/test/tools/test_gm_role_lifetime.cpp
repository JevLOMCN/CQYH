#include "gm_application_fixture.h"
#include "gm_ui.h"
#include "gm_role_fixture.h"
#include "role_info.h"
#include "my_tree_widget_item.h"
#include <QtWidgets/QTextEdit>

namespace
{
template<class T> T* widget(QWidget& parent,const char* name)
{
    auto* value=parent.findChild<T*>(name);
    if (!value) throw std::runtime_error(std::string("Missing fixture widget: ")+name);
    return value;
}
void select(gmtool& window,UINT32 id=0)
{
    auto* tree=widget<QTreeWidget>(window,"treeWidget_SvrList");
    for (UINT32 index=0;index<2;++index)
    {
        CSessionMgr::GetInstance().GetGMSession(index)->eState=GM_SVR_STATE_LOGIN;
        tree->topLevelItem(index)->setCheckState(0,index==id ? Qt::Checked : Qt::Unchecked);
    }
    CServerMgr::GetInstance().UpdateSelectServer();
}
gm::RoleDocument document()
{
    auto parsed=gm::parse_role_data(role_fixture::sample());
    if (!parsed) throw std::runtime_error(parsed.error());return std::move(*parsed);
}
QTreeWidgetItem* level(CRoleInfo& editor)
{
    auto* tree=widget<QTreeWidget>(editor,"baseDataTreeWidget");
    for (int index=0;index<tree->topLevelItemCount();++index)
    {
        auto* item=static_cast<CMyTreeWidgetItem*>(tree->topLevelItem(index));
        if (item->GetBaseDataType()==GM_RBD_LEVEL) return item;
    }
    throw std::runtime_error("Missing level field");
}
CRoleInfo* create(gmtool& window,bool edits=true)
{
    check(CDataMgr::GetInstance().StoreRole(42,document())!=nullptr,"owned role document staged");
    auto* editor=new CRoleInfo(0,42,TRUE,&window);editor->show();
    if (edits) level(*editor)->setText(1,QStringLiteral("100"));
    widget<QTextEdit>(*editor,"modifyRoleReasonEdit")->setPlainText(QStringLiteral("original reason"));
    return editor;
}
void answer(CRoleInfo& editor,bool yes=true,std::function<void()> before={})
{
    QTimer::singleShot(0,&editor,[&editor,yes,before=std::move(before)]
    {
        const QPointer<QDialog> confirmation=editor.findChild<QDialog*>("roleEditConfirmation");
        check(confirmation && confirmation->isVisible(),"owned role confirmation is visible");
        check(confirmation->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::No)->isDefault(),"role confirmation defaults to No");
        if (before) before();
        if (confirmation) { if (yes) confirmation->accept();else confirmation->reject(); }
    });
}
const std::vector<char>& packet(UINT16 id)
{
    const auto found=std::find_if(current->sent.rbegin(),current->sent.rend(),[id](const auto& value){return value.first==id;});
    if (found==current->sent.rend()) throw std::runtime_error("Missing native packet");return found->second;
}
}

int main(int argc,char** argv)
{
    try
    {
        WSADATA winsock{};check(WSAStartup(MAKEWORD(2,2),&winsock)==0,"Winsock fixture starts");
        struct WinsockCleanup{~WinsockCleanup(){WSACleanup();}} winsock_cleanup;
        QApplication application(argc,argv);application.setQuitOnLastWindowClosed(false);CLog::GetInstance().SetLevelMask(0);
        struct Stop{~Stop(){CGMManager::GetInstance().UnInit();}} stop_runtime;
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);
            auto tick=CSessionMgr::Clock::now();check(CSessionMgr::GetInstance().SetClock([&]{return tick;}),"inject local connection clock");
            start(window);select(window);auto& manager=CCmdManager::GetInstance();
            {
                gm::DialogScope scope(create(window));const auto editor=scope.Get();
                const auto initial=sent(tl2gm_modify_role_data);answer(*editor,false);editor->EndModify();
                check(sent(tl2gm_modify_role_data)==initial && editor->isVisible() && CDataMgr::GetInstance().FindRole(42),"declined modification retains editor and cached role");
                current->accept=false;answer(*editor);editor->EndModify();
                check(editor->isVisible() && CDataMgr::GetInstance().FindRole(42) && !manager.GetCurrentCmd(),"failed modification leaves edits available for retry");current->accept=true;
                auto* target=widget<QLineEdit>(*editor,"lineEdit");target->setText(QStringLiteral("88"));editor->SetTargetRole();
                answer(*editor,true,[&]
                {
                    widget<QTextEdit>(*editor,"modifyRoleReasonEdit")->setPlainText(QStringLiteral("changed"));
                    target->setText(QStringLiteral("99"));editor->SetTargetRole();level(*editor)->setText(1,QStringLiteral("200"));
                    editor->EndModify();editor->ReturnPos();editor->Exit();editor->close();
                });
                editor->EndModify();
                const auto& bytes=packet(tl2gm_modify_role_data);TL2GM_MODIFY_ROLE_DATA header;std::memcpy(&header,bytes.data(),sizeof(header));
                check(header.dwRoleID==42 && header.dwTargetRoleID==88 && std::string(header.szReason)=="original reason","role, target and reason captured before confirmation");
                const auto expected=role_fixture::scalar(GM_RBD_LEVEL,INT32{100});
                check(bytes.size()==sizeof(header)+expected.size() && std::equal(expected.begin(),expected.end(),bytes.begin()+sizeof(header)),"confirmation mutation cannot append or change the submitted edit bytes");
                check(!editor->isVisible() && !CDataMgr::GetInstance().FindRole(42) && sent(tl2gm_modify_role_data)==initial+2,"successful editor retires once after failed attempt and ignores nested actions");manager.CmdEnd();
            }
            {
                gm::DialogScope scope(create(window));const auto editor=scope.Get();const auto initial=sent(tl2gm_modify_role_data);
                select(window,1);editor->EndModify();check(sent(tl2gm_modify_role_data)==initial,"editor cannot submit to another selected server");select(window);
                answer(*editor,true,[&]{select(window,1);});editor->EndModify();check(sent(tl2gm_modify_role_data)==initial && editor->isVisible(),"selection is checked again after confirmation");select(window);
                answer(*editor,true,[&]{check(manager.SendGMStrCmd("busy"),"independent command starts during nested confirmation");});editor->EndModify();
                check(sent(tl2gm_modify_role_data)==initial,"busy state is checked again after confirmation");manager.CmdEnd();
            }
            {
                gm::DialogScope scope(create(window));const auto editor=scope.Get();const auto initial=sent(tl2gm_modify_role_data);
                const auto old_lifetime=CDataMgr::GetInstance().FindRole(42)->lifetime;
                answer(*editor,true,[&]{CDataMgr::GetInstance().ClearRole();CDataMgr::GetInstance().StoreRole(42,document());});editor->EndModify();
                check(sent(tl2gm_modify_role_data)==initial && CDataMgr::GetInstance().FindRole(42)->lifetime!=old_lifetime,"replacement cache entry invalidates the old editor before submission");
                const auto releases=sent(tl2gm_release_role);editor->Exit();
                check(sent(tl2gm_release_role)==releases && CDataMgr::GetInstance().FindRole(42),"stale editor neither releases nor deletes the replacement role");
            }
            {
                gm::DialogScope scope(create(window));const auto editor=scope.Get();auto& sessions=CSessionMgr::GetInstance();
                auto* session=sessions.GetGMSession(0);auto* const socket=session->pSocketSession;const auto lifetime=session->connection_lifetime;
                current->reuse_connections=true;check(sessions.CloseSession(socket),"original editor connection closes");
                tick+=std::chrono::milliseconds(RECONNECT_TIME);CGMManager::GetInstance().MainLoop();
                CMessageHandler::GetInstance().OnConnected(socket);auto registered=message<GM2TL_REGISTER_ACK>(gm2tl_register_ack);registered.nResult=errSuccess;deliver(*static_cast<Socket*>(socket),registered);select(window);
                check(session->pSocketSession==socket && session->connection_lifetime!=lifetime,"same native socket address reconnects with a new owned lifetime");
                const auto initial=sent(tl2gm_modify_role_data);editor->EndModify();const auto releases=sent(tl2gm_release_role);editor->Exit();
                check(sent(tl2gm_modify_role_data)==initial && sent(tl2gm_release_role)==releases,"old editor cannot modify or release a reused native connection");
            }
            {
                gm::DialogScope scope(create(window));const auto editor=scope.Get();
                current->send_hook=[&](Socket&,UINT16 id){if (id==tl2gm_modify_role_data) throw std::runtime_error("fixture native send failure");};
                answer(*editor);editor->EndModify();current->send_hook={};
                check(editor && editor->isVisible() && CDataMgr::GetInstance().FindRole(42),"throwing native modification retains editor and model");manager.CmdEnd();
            }
            {
                gm::DialogScope scope(create(window));const auto editor=scope.Get();
                current->send_hook=[&](Socket&,UINT16 id){if (id==tl2gm_modify_role_data) delete editor.data();};
                answer(*editor);editor->EndModify();current->send_hook={};check(!editor && !CDataMgr::GetInstance().FindRole(42),"native modification callback may destroy its editor without retaining its retired model");manager.CmdEnd();
            }
            {
                gm::DialogScope scope(create(window));const auto editor=scope.Get();const auto lifetime=CDataMgr::GetInstance().FindRole(42)->lifetime;
                current->send_hook=[&](Socket&,UINT16 id){if (id==tl2gm_modify_role_data) CDataMgr::GetInstance().StoreRole(42,document());};
                answer(*editor);editor->EndModify();current->send_hook={};
                check(CDataMgr::GetInstance().FindRole(42) && CDataMgr::GetInstance().FindRole(42)->lifetime!=lifetime,"successful old editor does not delete a cache replacement from its send callback");manager.CmdEnd();
            }
            {
                gm::DialogScope scope(create(window,false));const auto editor=scope.Get();current->accept=false;editor->EndModify();current->accept=true;
                check(editor->isVisible() && CDataMgr::GetInstance().FindRole(42),"failed no-edit release remains retryable");
                const auto before=sent(tl2gm_release_role);current->send_hook=[&](Socket&,UINT16 id){if (id==tl2gm_release_role){editor->Exit();editor->EndModify();}};
                editor->EndModify();current->send_hook={};check(sent(tl2gm_release_role)==before+1 && !CDataMgr::GetInstance().FindRole(42),"no-edit release suppresses reentry and retires its owned model");
            }
            {
                gm::DialogScope scope(create(window,false));const auto editor=scope.Get();
                current->send_hook=[&](Socket&,UINT16 id){if (id==tl2gm_release_role) delete editor.data();};
                editor->Exit();current->send_hook={};check(!editor && !CDataMgr::GetInstance().FindRole(42),"release callback may destroy a cancelling editor without retaining its retired model");
            }
            stop();check(CSessionMgr::GetInstance().SetClock({}),"restore native session clock after fixture");
        }
        for (const auto return_position:{false,true})
        {
            QPointer<gmtool> window=new gmtool;loader login;gm::ApplicationViews views(*window,login);
            struct Delete{QPointer<gmtool> window;~Delete(){delete window.data();}} delete_window{window};
            start(*window);select(*window);const QPointer<CRoleInfo> editor=create(*window);
            answer(*editor,true,[&]{delete window.data();});
            if (return_position) editor->ReturnPos();else editor->EndModify();
            check(!editor && !window && !CGMManager::GetInstance().IsRunning(),"parent deletion during nested role confirmation safely cancels the editor");stop();
        }
        std::cout<<checks<<" GM role-lifetime checks passed\n";return 0;
    }
    catch(const std::exception& error){CGMManager::GetInstance().UnInit();std::cerr<<error.what()<<'\n';return 1;}
}
