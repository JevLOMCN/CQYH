#include "gm_application_fixture.h"

int main(int argc,char** argv)
{
    try
    {
        WSADATA winsock{};
        if (WSAStartup(MAKEWORD(2,2), &winsock) != 0) throw std::runtime_error("Winsock initialization failed");
        struct WinsockCleanup { ~WinsockCleanup() { WSACleanup(); } } winsock_cleanup;
        CLog::GetInstance().SetLevelMask(0);
        QApplication application(argc,argv);application.setQuitOnLastWindowClosed(false);
        check(argc==2,"expected screenshot output path");
        check(QFontDatabase::addApplicationFont("C:/Windows/Fonts/msyh.ttc")>=0,"Chinese fixture font available");
        application.setFont(QFont(QStringLiteral("Microsoft YaHei"),9));
        struct Stop{~Stop(){CGMManager::GetInstance().UnInit();}} stop_runtime;
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);
            const auto initial=sent(tl2gm_verify);
            for(const auto& pair:std::vector<std::pair<QString,QString>>{
                {QString(),QStringLiteral("password")},{QStringLiteral("fixture"),QString()},
                {QString(33,'a'),QStringLiteral("password")},{QStringLiteral("fixture"),QString(33,'b')},
                {QString::fromUtf8("\xe7\x8c\xab").repeated(11),QStringLiteral("password")},
                {QStringLiteral("abc")+QChar(0)+QStringLiteral("def"),QStringLiteral("password")}})
            {credentials(login,pair.first,pair.second);login.Login();check(ready(login) && sent(tl2gm_verify)==initial,"invalid credentials rejected without truncation or send");}
            credentials(login,QString(32,'a'),QString(32,'b'));login.Login();
            check(!ready(login) && std::strlen(CGMManager::GetInstance().GetUserName())==32,"full wire-width credentials retained");
            const auto count=sent(tl2gm_verify);login.Login();check(sent(tl2gm_verify)==count,"duplicate login suppressed while pending");
            verification(*current->net->sockets[1]);check(!ready(login) && login.result()!=QDialog::Accepted,"wrong peer cannot verify pending login");
            verification(*current->net->sockets[0],42);check(ready(login) && login.result()!=QDialog::Accepted,"failed verification permits retry");
            check(login.findChild<QLabel*>("label_3")->text().contains(QStringLiteral("42")),"server failure reported inline");
            login.show();login.findChild<QPushButton*>("loginButton")->setFocus();application.processEvents();
            check(login.grab().save(QString::fromUtf8(argv[1])),"inline error screenshot written");login.hide();
            current->accept=false;credentials(login);login.Login();
            check(ready(login),"rejected native send restores retry controls");
            current->accept=true;
            current->send_hook=[](Socket&,UINT16 id){if(id==tl2gm_verify)throw std::runtime_error("fixture send failure");};
            login.Login();check(ready(login),"throwing send restores retry controls");current->send_hook={};
            pending(login);
            auto* timeout=login.findChild<QTimer*>("loginTimeout");
            check(timeout && timeout->isActive() && timeout->interval()==30000 && timeout->isSingleShot(),"pending request has a bounded Qt timeout");
            QMetaObject::invokeMethod(timeout,"timeout",Qt::DirectConnection);
            check(ready(login) && !timeout->isActive() && !CSessionMgr::GetInstance().GetGMSession(0)->pSocketSession,
                "timeout resets pending state and closes the expired connection");
            verification(*current->net->sockets[0]);check(login.result()!=QDialog::Accepted,"late reply from expired connection ignored");
            pending(login);verification(*current->net->sockets[1],42);check(ready(login),"retry can select another registered peer");
            stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);pending(login);
            server_error(*current->net->sockets[1],"wrong peer");
            check(!ready(login),"error text from another peer cannot cancel verification");
            server_error(*current->net->sockets[0],"<b>fixture error</b>");
            auto* label=login.findChild<QLabel*>("label_3");
            check(ready(login) && label->text()==QStringLiteral("<b>fixture error</b>") && label->textFormat()==Qt::PlainText,
                "owned server error completes inline as plain text");
            check(!CSessionMgr::GetInstance().GetGMSession(0)->pSocketSession,"failed verification connection retired before retry");
            stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);pending(login);
            auto* socket=current->net->sockets[0].get();CMessageHandler::GetInstance().OnDisconnected(socket,FALSE);
            QMetaObject::invokeMethod(login.findChild<QTimer*>("loginConnectionWatch"),"timeout",Qt::DirectConnection);
            check(ready(login),"disconnect watcher releases pending controls");verification(*socket);
            check(login.result()!=QDialog::Accepted,"disconnected peer cannot finish verification");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);pending(login);
            login.reject();check(ready(login) && login.result()==QDialog::Rejected,"dialog cancellation clears pending verification");
            verification(*current->net->sockets[0]);check(login.result()!=QDialog::Accepted,"cancelled verification cannot reopen the window");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);
            const auto name=QString::fromUtf8("\xe7\x8c\xab").repeated(10);
            credentials(login,name,QString(32,'p'));
            current->send_hook=[](Socket& socket,UINT16 id){if(id==tl2gm_verify)verification(socket);};
            login.Login();check(login.result()==QDialog::Accepted && window.isVisible(),"synchronous successful verification opens the actual main window");
            check(window.windowTitle().contains(name),"UTF-8 account title preserves complete text");
            const auto count=sent(tl2gm_verify);login.Login();verification(*current->net->sockets[0]);
            check(sent(tl2gm_verify)==count && !ready(login),"verified dialog cannot issue another request or accept duplicate replies");
            current->send_hook={};
            auto* tree=window.findChild<QTreeWidget*>("treeWidget_SvrList");tree->topLevelItem(0)->setCheckState(0,Qt::Checked);
            auto reply=message<GM2TL_LOGIN_ACK>(gm2tl_login_ack);reply.nResult=42;deliver(*current->net->sockets[0],reply);
            check(CSessionMgr::GetInstance().GetGMSession(0)->eState==GM_SVR_STATE_REGISTERED,"failed server login does not publish logged-in state");
            reply.nResult=errSuccess;deliver(*current->net->sockets[0],reply);
            check(CSessionMgr::GetInstance().GetGMSession(0)->eState==GM_SVR_STATE_LOGIN,"owned server login reply publishes state");
            deliver(*current->net->sockets[0],reply);
            auto logout=message<GM2TL_LOGOUT_ACK>(gm2tl_logout_ack);deliver(*current->net->sockets[0],logout);deliver(*current->net->sockets[0],logout);
            check(CSessionMgr::GetInstance().GetGMSession(0)->eState==GM_SVR_STATE_REGISTERED,"logout reply and duplicate retain registered state");
            current->send_hook=[&](Socket& socket,UINT16 id)
            {
                if(id==tl2gm_login){window.Login();auto ack=message<GM2TL_LOGIN_ACK>(gm2tl_login_ack);ack.nResult=errSuccess;deliver(socket,ack);}
                if(id==tl2gm_logout){window.Logout();auto ack=message<GM2TL_LOGOUT_ACK>(gm2tl_logout_ack);deliver(socket,ack);}
            };
            const auto login_count=sent(tl2gm_login);window.Login();
            check(sent(tl2gm_login)==login_count+1 && CSessionMgr::GetInstance().GetGMSession(0)->eState==GM_SVR_STATE_LOGIN,
                "actual login button accepts synchronous ACK and suppresses nested submission");
            ::ITEM_TEMPLATE_DATA item{};item.dwTempID=7;std::memcpy(item.szName,"fixture",8);
            check(CDataMgr::GetInstance().ApplyItems(0,true,{&item,1}),"logout item fixture staged");
            current->accept=false;current->send_hook={};window.Logout();
            check(CDataMgr::GetInstance().GetItems(0).size()==1,"failed logout send preserves item data");
            current->accept=true;
            current->send_hook=[&](Socket& socket,UINT16 id)
            {if(id==tl2gm_logout){window.Logout();auto ack=message<GM2TL_LOGOUT_ACK>(gm2tl_logout_ack);deliver(socket,ack);}};
            const auto logout_count=sent(tl2gm_logout);window.Logout();
            check(sent(tl2gm_logout)==logout_count+1 && CDataMgr::GetInstance().GetItems(0).empty() &&
                CSessionMgr::GetInstance().GetGMSession(0)->eState==GM_SVR_STATE_REGISTERED,
                "actual logout button clears items after ACK and suppresses nested submission");current->send_hook={};
            Socket orphan(current);orphan.data=reinterpret_cast<void*>(1);
            auto registration=message<GM2TL_REGISTER_ACK>(gm2tl_register_ack);registration.nResult=errSuccess;
            deliver(orphan,registration);verification(orphan);
            check(current->userdata_reads==0,"unowned socket metadata is never dereferenced");
            auto* peer=current->net->sockets[0].get();
            peer->data=reinterpret_cast<void*>(1);verification(*peer);peer->data=CSessionMgr::GetInstance().GetGMSession(0);
            check(current->userdata_reads==0,"owned socket lookup ignores invalid native userdata");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);
            auto* session=CSessionMgr::GetInstance().GetGMSession(0);session->eState=GM_SVR_STATE_NONE;
            auto failure=message<GM2TL_REGISTER_ACK>(gm2tl_register_ack);failure.nResult=42;deliver(*current->net->sockets[0],failure);
            check(session->nConnectState==SESSION_DEAD && !session->pSocketSession,"registration rejection closes the native binding and preserves dead state");stop();
        }
        {
            auto window=std::make_unique<gmtool>();loader login;gm::ApplicationViews views(*window,login);start(*window);
            credentials(login);
            QObject::connect(window.get(),&QWidget::windowTitleChanged,&login,[&]{window.reset();});
            current->send_hook=[](Socket& socket,UINT16 id){if(id==tl2gm_verify)verification(socket);};
            login.Login();check(!window && login.result()==QDialog::Rejected && !CGMManager::GetInstance().IsRunning(),
                "title-change destruction cancels verification instead of leaving a disabled dialog");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);credentials(login);
            QObject::connect(&window,&QWidget::windowTitleChanged,&login,[]{CGMManager::GetInstance().UnInit();});
            current->send_hook=[](Socket& socket,UINT16 id){if(id==tl2gm_verify)verification(socket);};
            login.Login();check(login.result()==QDialog::Rejected && !window.isVisible(),
                "title-change runtime shutdown cancels verification before showing main window");stop();
        }
        {
            auto window=std::make_unique<gmtool>();loader login;gm::ApplicationViews views(*window,login);start(*window);
            credentials(login);
            QObject::connect(&login,&QDialog::accepted,&login,[&]{window.reset();});
            current->send_hook=[](Socket& socket,UINT16 id){if(id==tl2gm_verify)verification(socket);};
            login.Login();check(!window && !CGMManager::GetInstance().IsRunning(),"accepted callback can destroy the main window and stop runtime");stop();
        }
        {
            auto window=std::make_unique<gmtool>();auto login=std::make_unique<loader>();gm::ApplicationViews views(*window,*login);start(*window);
            credentials(*login);
            current->send_hook=[&](Socket&,UINT16 id){if(id==tl2gm_verify){login.reset();window.reset();}};
            login->Login();check(!login && !window && !CGMManager::GetInstance().IsRunning(),"native send callback can destroy both views safely");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);credentials(login);
            current->send_hook=[](Socket&,UINT16 id){if(id==tl2gm_verify)CGMManager::GetInstance().UnInit();};
            login.Login();check(ready(login) && !CGMManager::GetInstance().IsRetiring(),"send-time shutdown clears pending state after native retirement");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);
            auto& sessions=CSessionMgr::GetInstance();auto tick=CSessionMgr::Clock::now();
            check(sessions.SetClock([&]{return tick;}),"install login reconnect clock");start(window);pending(login);
            auto* socket=current->net->sockets[0].get();current->reuse_connections=true;
            sessions.CloseSession(socket);tick+=std::chrono::milliseconds(RECONNECT_TIME);CGMManager::GetInstance().MainLoop();
            CMessageHandler::GetInstance().OnConnected(socket);auto registered=message<GM2TL_REGISTER_ACK>(gm2tl_register_ack);registered.nResult=errSuccess;deliver(*socket,registered);
            verification(*socket);check(login.result()!=QDialog::Accepted,"old login rejects verification on reused socket");
            const auto closes=current->closes;
            QMetaObject::invokeMethod(login.findChild<QTimer*>("loginTimeout"),"timeout",Qt::DirectConnection);
            check(ready(login) && current->closes==closes && sessions.GetGMSession(0)->pSocketSession==socket,
                "old login timeout cannot close replacement connection");
            pending(login);verification(*socket);check(login.result()==QDialog::Accepted,"fresh login verifies replacement binding");
            stop();check(sessions.SetClock({}),"restore login clock");
        }
        std::cout<<checks<<" GM authentication checks passed\n";return 0;
    }
    catch(const std::exception& error){CGMManager::GetInstance().UnInit();std::cerr<<error.what()<<'\n';return 1;}
}
