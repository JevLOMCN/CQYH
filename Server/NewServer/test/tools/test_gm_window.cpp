#include "gm_application_fixture.h"
#include "gm_ui.h"
#include "gm_role_fixture.h"
#include "role_gm_cmd.h"
#include "role_info.h"
#include "gm_reports.h"
#include "executing.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <fstream>

namespace
{
struct CompatibilityWindow:gmtool { using gmtool::_Tag2ReadRoleID; };
template<class T> T* widget(QWidget& owner,const char* name)
{
    auto* result=owner.findChild<T*>(name);
    if(!result)throw std::runtime_error(std::string("Missing fixture widget: ")+name);return result;
}
void select(gmtool& window,UINT32 id=0)
{
    auto* tree=widget<QTreeWidget>(window,"treeWidget_SvrList");
    for(UINT32 index=0;index<2;++index)
    {CSessionMgr::GetInstance().GetGMSession(index)->eState=GM_SVR_STATE_LOGIN;tree->topLevelItem(index)->setCheckState(0,index==id ? Qt::Checked : Qt::Unchecked);}
    CServerMgr::GetInstance().UpdateSelectServer();
}
template<class T> T request(UINT16 id)
{
    const auto found=std::find_if(current->sent.rbegin(),current->sent.rend(),[id](const auto& value){return value.first==id;});
    if(found==current->sent.rend() || found->second.size()<sizeof(T))throw std::runtime_error("Missing fixture packet");
    T result;std::memcpy(&result,found->second.data(),sizeof(result));return result;
}
bool confirm(gmtool& window,bool yes,std::function<void()> before={})
{
    auto* dialog=window.findChild<QDialog*>("gmActionConfirmation");
    if(!dialog)return false;
    const QPointer<QDialog> alive(dialog);
    if(before)before();
    if(alive){if(yes)alive->accept();else alive->reject();}
    return true;
}
void raw(Socket& socket,const std::vector<char>& bytes)
{
    auto* packet=socket.state->allocator->Alloc(static_cast<INT32>(bytes.size()));std::memcpy(packet,bytes.data(),bytes.size());
    CMessageHandler::GetInstance().OnRecv(&socket,packet,static_cast<UINT32>(bytes.size()));
}
std::vector<char> role_bytes(UINT32 id)
{
    auto bytes=role_fixture::sample();std::memcpy(bytes.data()+sizeof(GM_ROLE_SLOT),&id,sizeof(id));return bytes;
}
void role_ack(Socket& socket,UINT32 id)
{
    GM2TL_QUERY_ROLE_DATA_ACK header{};header.wMsg=gm2tl_query_role_data_ack;header.dwRoleID=id;header.nAreaID=7;
    const auto body=role_bytes(id);header.nLength=static_cast<INT32>(body.size());
    const auto prefix=offsetof(GM2TL_QUERY_ROLE_DATA_ACK,szData);std::vector<char> bytes(prefix+body.size());
    std::memcpy(bytes.data(),&header,prefix);std::memcpy(bytes.data()+prefix,body.data(),body.size());raw(socket,bytes);
}
void role_key(Socket& socket)
{
    GM2TL_QUERY_ROLE_ID_ACK header;std::memset(&header,0,sizeof(header));header.wMsg=gm2tl_query_role_id_ack;header.nCount=1;header.nAreaID=7;
    GM_ROLE_KEY_INFO row{};row.dwRoleID=42;std::memcpy(row.szRoleName,"new",4);std::memcpy(row.szUserName,"account",8);
    std::vector<char> bytes(sizeof(header)+sizeof(row));std::memcpy(bytes.data(),&header,sizeof(header));std::memcpy(bytes.data()+sizeof(header),&row,sizeof(row));raw(socket,bytes);
}
}

int main(int argc,char** argv)
{
    try
    {
        WSADATA winsock{};if(WSAStartup(MAKEWORD(2,2),&winsock)!=0)throw std::runtime_error("Winsock initialization failed");
        struct WinsockCleanup{~WinsockCleanup(){WSACleanup();}} winsock_cleanup;
        CLog::GetInstance().SetLevelMask(0);QApplication application(argc,argv);application.setQuitOnLastWindowClosed(false);
        check(argc==2,"expected screenshot output path");
        check(QFontDatabase::addApplicationFont("C:/Windows/Fonts/msyh.ttc")>=0,"Chinese fixture font available");application.setFont(QFont(QStringLiteral("Microsoft YaHei"),9));
        const auto previous=std::filesystem::current_path();
        const auto directory=std::filesystem::absolute(argv[1]).parent_path()/"gm-window";std::filesystem::create_directories(directory);
        struct Restore{std::filesystem::path previous;~Restore(){std::error_code error;std::filesystem::current_path(previous,error);}} restore{previous};
        std::filesystem::current_path(directory);
        struct Stop{~Stop(){CGMManager::GetInstance().UnInit();}} stop_runtime;
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            auto& manager=CCmdManager::GetInstance();
            auto* role_name=widget<QLineEdit>(window,"lineEdit_query_1");auto* account_name=widget<QLineEdit>(window,"lineEdit_query_2");
            auto* results=widget<QTreeWidget>(window,"treeWidget_queryroleid");
            window.AddQueryRoleIDResult(1,7,"old","account");
            const auto initial=sent(tl2gm_query_role_id);window.QueryRoleID();
            check(sent(tl2gm_query_role_id)==initial && results->topLevelItemCount()==1,"missing query type retains existing results");
            widget<QRadioButton>(window,"radioButton_query_1")->setChecked(true);
            for(const auto& invalid:std::vector<QString>{QString(),QString(USER_NAME_LEN,'x'),QString::fromUtf8("\xe7\x8c\xab").repeated(11),QStringLiteral("a")+QChar(0)+QStringLiteral("b")})
            {role_name->setText(invalid);widget<QPushButton>(window,"queryRoleID")->click();check(sent(tl2gm_query_role_id)==initial && results->topLevelItemCount()==1,"invalid name rejected without truncation or clearing results");}
            role_name->setText(QStringLiteral("fixture"));check(manager.SendGMStrCmd("fixture"),"busy command staged");window.QueryRoleID();
            check(sent(tl2gm_query_role_id)==initial && results->topLevelItemCount()==1,"busy query leaves current results intact");manager.CmdEnd();
            widget<QRadioButton>(window,"radioButton_query_2")->setChecked(true);account_name->setText(QString(32,'u'));
            current->send_hook=[&](Socket& socket,UINT16 id){if(id==tl2gm_query_role_id){account_name->setText(QStringLiteral("changed"));window.QueryRoleID();role_key(socket);}};
            window.QueryRoleID();current->send_hook={};const auto query=request<TL2GM_QUERY_ROLE_ID>(tl2gm_query_role_id);
            check(std::string(query.szName)==std::string(32,'u') && query.nQueryType==GM_QUERY_BY_USER_NAME,"complete owned account query survives UI mutation");
            check(sent(tl2gm_query_role_id)==initial+1 && results->topLevelItemCount()==1 && results->topLevelItem(0)->text(2)==QStringLiteral("new"),"synchronous reply retained and nested query suppressed");manager.MainLoop();
            widget<QRadioButton>(window,"radioButton_query_1")->setChecked(true);role_name->setText(QStringLiteral("fixture"));
            auto connection=QObject::connect(results->model(),&QAbstractItemModel::modelReset,&window,[&]{select(window,1);});
            window.QueryRoleID();QObject::disconnect(connection);
            check(sent(tl2gm_query_role_id)==initial+1 && !manager.GetCurrentCmd(),"selection change during result clearing cancels submission");select(window);
            auto document=gm::parse_role_data(role_bytes(42));check(document.has_value(),"owned cached role staged");
            auto* stored=CDataMgr::GetInstance().StoreRole(42,std::move(*document));window.AddQueryRoleDataResult(7,FALSE,stored);
            auto* role_results=widget<QTreeWidget>(window,"treeWidget");auto* name=widget<QLineEdit>(window,"lineEdit_queryByid");
            widget<QRadioButton>(window,"radioButton_queryByID")->setChecked(true);
            const auto prequery_count=sent(tl2gm_pre_query);
            for(const auto& invalid:std::vector<QString>{QString(),QString(ROLE_NAME_LEN,'x'),QStringLiteral("a")+QChar(0)})
            {name->setText(invalid);window.QueryRoleData();window.ModifyRoleData();check(sent(tl2gm_pre_query)==prequery_count && role_results->topLevelItemCount()==1 && CDataMgr::GetInstance().FindRole(42),"invalid role query/edit preserves tree and cached document");}
            name->setText(QStringLiteral("original"));bool saw=false;QTimer::singleShot(0,&window,[&]{saw=confirm(window,false);});window.ModifyRoleData();
            check(saw && sent(tl2gm_pre_query)==prequery_count && role_results->topLevelItemCount()==1,"declined role editing preserves results");
            QTimer::singleShot(0,&window,[&]{saw=confirm(window,true,[&]{select(window,1);});});window.ModifyRoleData();
            check(saw && sent(tl2gm_pre_query)==prequery_count,"changed selected peer invalidates edit confirmation");select(window);
            QTimer::singleShot(0,&window,[&]{saw=confirm(window,true,[&]{name->setText(QStringLiteral("changed"));window.ModifyRoleData();});});window.ModifyRoleData();
            check(saw && sent(tl2gm_pre_query)==prequery_count+1,"confirmed editor sends one captured role despite UI mutation/reentry");
            auto load=std::static_pointer_cast<PRE_QUERY>(manager.GetCurrentCmd());
            check(load->nCmdType==CMD_LOAD_ROLE_DATA && std::string(load->szRoleName)=="original","edit uses pre-confirmation role identity");manager.CmdEnd();
            name->setText(QStringLiteral("queried"));
            current->send_hook=[&](Socket& socket,UINT16 id)
            {
                if(id==tl2gm_pre_query){name->setText(QStringLiteral("changed again"));window.QueryRoleData();auto ack=message<GM2TL_PRE_QUERY_ACK>(gm2tl_pre_query_ack);ack.dwRoleID=7;std::memcpy(ack.szRoleName,"queried",8);deliver(socket,ack);}
                if(id==tl2gm_query_role_data)role_ack(socket,7);
            };
            window.QueryRoleData();check(manager.MainLoop() && manager.MainLoop(),"role query completes through deferred typed continuation");current->send_hook={};
            check(role_results->topLevelItemCount()==1 && role_results->topLevelItem(0)->text(2)==QStringLiteral("7") && CDataMgr::GetInstance().FindRole(7),"actual role query preserves synchronous owned reply");
            role_results->setCurrentItem(role_results->topLevelItem(0));window.ViewRoleData();const QPointer<CRoleInfo> view=window.findChild<CRoleInfo*>();
            check(view && view->isVisible(),"cached role opens a parent-owned read-only view");view->close();QCoreApplication::sendPostedEvents(nullptr,QEvent::DeferredDelete);check(!view,"read-only role view deletes on close");
            role_name->setText(QString(USER_NAME_LEN-1,'r'));widget<QRadioButton>(window,"radioButton_query_1")->setChecked(true);window.QueryRoleID();
            check(std::string(request<TL2GM_QUERY_ROLE_ID>(tl2gm_query_role_id).szName)==std::string(USER_NAME_LEN-1,'r'),
                "fuzzy role-name query retains the existing full wire-field capacity");manager.CmdEnd();role_name->setText(QStringLiteral("fixture"));
            account_name->setText(QString(USER_NAME_LEN,'x'));widget<QRadioButton>(window,"radioButton_query_2")->setChecked(true);window.QueryRoleID();
            window.resize(1200,760);window.show();widget<QPushButton>(window,"queryRoleID")->setFocus();application.processEvents();
            check(window.grab().save(QString::fromUtf8(argv[1])),"inline role-query validation screenshot written");window.hide();
            stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            auto& manager=CCmdManager::GetInstance();auto* command_text=widget<QTextEdit>(window,"textEdit_gmStrCmd");
            const auto original_count=sent(tl2gm_exe_gm_str);
            for(const auto& invalid:std::vector<QString>{QString(),QString(MAX_GM_COMMAND_LEN,'x'),QStringLiteral("ab")+QChar(0)})
            {command_text->setPlainText(invalid);window.SendGMStrCmd();check(sent(tl2gm_exe_gm_str)==original_count,"invalid raw command never opens confirmation or sends");}
            command_text->setPlainText(QStringLiteral("original command"));bool saw=false;
            QTimer::singleShot(0,&window,[&]{saw=confirm(window,false);});window.SendGMStrCmd();
            check(saw && sent(tl2gm_exe_gm_str)==original_count,"declined raw command sends nothing");
            QTimer::singleShot(0,&window,[&]{saw=confirm(window,true,[&]{command_text->setPlainText(QStringLiteral("changed"));window.SendGMStrCmd();});});window.SendGMStrCmd();
            const auto& packet=current->sent.back().second;
            check(saw && sent(tl2gm_exe_gm_str)==original_count+1 &&
                std::string(packet.data()+sizeof(TL2GM_EXE_GM_STR))=="original command","raw command uses captured text and rejects nested submission");manager.CmdEnd();
            command_text->setPlainText(QStringLiteral("fixture"));QTimer::singleShot(0,&window,[&]{saw=confirm(window,true,[&]{select(window,1);});});window.SendGMStrCmd();
            check(saw && sent(tl2gm_exe_gm_str)==original_count+1 && !manager.GetCurrentCmd(),"changed peer invalidates raw command confirmation");select(window);
            current->accept=false;QTimer::singleShot(0,&window,[&]{saw=confirm(window,true);});window.SendGMStrCmd();
            check(saw && !manager.GetCurrentCmd() && window.statusBar()->currentMessage()==QStringLiteral("发送命令失败"),"failed raw send leaves UI retryable");current->accept=true;
            const auto name_path=directory/L"窗口角色名单.txt";
            {std::ofstream output(name_path,std::ios::binary);output<<"one\r\ntwo\r\n";}
            auto* file=widget<QLineEdit>(window,"queryfile_lineEdit");
            file->setText(QStringLiteral("missing-file"));const auto named_count=sent(tl2gm_query_name_to_id);window.QueryNameToID();
            check(sent(tl2gm_query_name_to_id)==named_count && !manager.GetCurrentCmd(),"missing name file is reported without sending");
            file->setText(QString::fromStdWString(name_path.wstring()));window.QueryNameToID();
            const auto named=request<TL2GM_QUERY_NAME_TO_ID>(tl2gm_query_name_to_id);
            check(sent(tl2gm_query_name_to_id)==named_count+1 && named.nIndex==1 && std::string(named.szRoleName)=="one",
                "Unicode file path loads staged standard name records");manager.CmdEnd();
            auto* destination=widget<QLineEdit>(window,"lineEdit_queryByFile");destination->setText(QStringLiteral("unchanged"));
            QTimer::singleShot(0,&window,[&]{if(auto* dialog=window.findChild<QFileDialog*>("gmRoleFileDialog")){saw=true;dialog->reject();}});window.OpenFile();
            check(saw && destination->text()==QStringLiteral("unchanged"),"cancelled file picker preserves existing path");
            saw=false;QTimer::singleShot(0,&window,[&]
            {
                const QPointer<QFileDialog> dialog=window.findChild<QFileDialog*>("gmRoleFileDialog");
                if(dialog){saw=true;dialog->selectFile(QString::fromStdWString(name_path.wstring()));QTimer::singleShot(50,dialog,[dialog]{if(dialog)static_cast<QDialog*>(dialog.data())->accept();});}
            });window.OpenFile();
            check(saw && std::filesystem::equivalent(std::filesystem::path(destination->text().toStdWString()),name_path),"file picker publishes complete native Unicode path");
            saw=false;QTimer::singleShot(0,&window,[&]
            {
                const QPointer<QFileDialog> dialog=window.findChild<QFileDialog*>("gmRoleFileDialog");
                if(dialog){saw=true;dialog->selectFile(QString::fromStdWString(name_path.wstring()));QTimer::singleShot(50,dialog,[dialog]{if(dialog)static_cast<QDialog*>(dialog.data())->accept();});}
            });window.OpenFile_2();
            check(saw && std::filesystem::equivalent(std::filesystem::path(widget<QLineEdit>(window,"lineEdit")->text().toStdWString()),name_path),"second picker publishes to its captured destination");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            auto& manager=CCmdManager::GetInstance();auto* name=widget<QLineEdit>(window,"lineEdit_queryByid");
            widget<QRadioButton>(window,"radioButton_queryByID")->setChecked(true);name->setText(QStringLiteral("original role"));
            const auto original_count=sent(tl2gm_pre_query);bool invalid_rejected=false;
            QTimer::singleShot(0,&window,[&]
            {
                const QPointer<CRoleGMCmd> dialog=window.findChild<CRoleGMCmd*>();
                if(!dialog)return;
                auto* text=widget<QTextEdit>(*dialog,"textEdit");auto* reason=widget<QTextEdit>(*dialog,"textEdit_2");
                text->setPlainText(QString(MAX_GM_COMMAND_LEN,'x'));reason->setPlainText(QStringLiteral("reason"));dialog->OK();
                invalid_rejected=!dialog->IsExeCmd() && !widget<QLabel>(*dialog,"roleCommandStatus")->text().isEmpty();
                text->setPlainText(QStringLiteral("fixture"));reason->setPlainText(QString(GM_REASON_LEN,'r'));dialog->OK();
                invalid_rejected=invalid_rejected && !dialog->IsExeCmd();
                reason->setPlainText(QStringLiteral("ab")+QChar(0));dialog->OK();invalid_rejected=invalid_rejected && !dialog->IsExeCmd();dialog->reject();
            });window.RoleGMCmd();
            check(invalid_rejected && sent(tl2gm_pre_query)==original_count,"role command rejects excessive/NUL fields before confirmation");
            bool captured=false;QTimer::singleShot(0,&window,[&]
            {
                const QPointer<CRoleGMCmd> dialog=window.findChild<CRoleGMCmd*>();if(!dialog)return;
                widget<QTextEdit>(*dialog,"textEdit")->setPlainText(QString(MAX_GM_COMMAND_LEN-1,'x'));
                widget<QTextEdit>(*dialog,"textEdit_2")->setPlainText(QStringLiteral("original reason"));
                QTimer::singleShot(0,&window,[&window,dialog,&captured]
                {
                    captured=confirm(window,true,[&]
                    {
                        widget<QTextEdit>(*dialog,"textEdit")->setPlainText(QStringLiteral("changed"));
                        widget<QTextEdit>(*dialog,"textEdit_2")->setPlainText(QStringLiteral("changed reason"));
                        widget<QLineEdit>(window,"lineEdit_queryByid")->setText(QStringLiteral("changed role"));dialog->OK();window.RoleGMCmd();
                    });
                });dialog->OK();
            });window.RoleGMCmd();
            const auto pre=std::static_pointer_cast<PRE_QUERY>(manager.GetCurrentCmd());
            const auto command=pre ? std::static_pointer_cast<EXE_ROLE_CMD>(pre->pCmd) : nullptr;
            check(captured && command && sent(tl2gm_pre_query)==original_count+1 && !window.findChild<CRoleGMCmd*>(),"accepted role dialog publishes one owned command and is destroyed");
            check(std::string(command->szRoleName)=="original role" && std::string(command->szReason)=="original reason" &&
                std::string(command->szGM)==std::string(MAX_GM_COMMAND_LEN-1,'x'),"role/reason/maximum command remain complete and immutable after confirmation");manager.CmdEnd();
            name->setText(QStringLiteral("fixture"));bool switched=false;QTimer::singleShot(0,&window,[&]
            {
                const QPointer<CRoleGMCmd> dialog=window.findChild<CRoleGMCmd*>();if(!dialog)return;
                widget<QTextEdit>(*dialog,"textEdit")->setPlainText(QStringLiteral("fixture"));widget<QTextEdit>(*dialog,"textEdit_2")->setPlainText(QStringLiteral("reason"));
                QTimer::singleShot(0,&window,[&]{switched=confirm(window,true,[&]{select(window,1);});});dialog->OK();
            });window.RoleGMCmd();
            check(switched && sent(tl2gm_pre_query)==original_count+1 && !manager.GetCurrentCmd(),"role dialog cannot submit against a different selected peer");stop();
        }
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            widget<QRadioButton>(window,"radioButton_queryByID")->setChecked(true);widget<QLineEdit>(window,"lineEdit_queryByid")->setText(QStringLiteral("fixture"));
            bool saw=false;QTimer::singleShot(0,&window,[&]{saw=confirm(window,true,[]{CGMManager::GetInstance().UnInit();});});window.ModifyRoleData();
            check(saw && !CGMManager::GetInstance().IsRunning() && sent(tl2gm_pre_query)==0,"runtime shutdown invalidates edit confirmation without touching retired sockets");stop();
        }
        for(int kind=0;kind<3;++kind)
        {
            QPointer<gmtool> window=new gmtool;loader login;gm::ApplicationViews views(*window,login);
            struct Delete{QPointer<gmtool> window;~Delete(){delete window.data();}} delete_window{window};
            start(*window);select(*window);widget<QRadioButton>(*window,"radioButton_queryByID")->setChecked(true);
            widget<QLineEdit>(*window,"lineEdit_queryByid")->setText(QStringLiteral("fixture"));
            bool opened=false;QTimer::singleShot(0,&login,[&]
            {
                opened=kind==0 ? window->findChild<QDialog*>("gmActionConfirmation")!=nullptr :
                    kind==1 ? window->findChild<CRoleGMCmd*>()!=nullptr : window->findChild<QFileDialog*>("gmRoleFileDialog")!=nullptr;
                window->deleteLater();
            });
            if(kind==0)window->ModifyRoleData();else if(kind==1)window->RoleGMCmd();else window->OpenFile();
            check(opened && !window && !CGMManager::GetInstance().IsRunning(),"parent deletion safely cancels confirmation/role-command/file dialog");stop();
        }
        {
            QPointer<gmtool> window=new gmtool;loader login;gm::ApplicationViews views(*window,login);
            struct Delete{QPointer<gmtool> window;~Delete(){delete window.data();}} delete_window{window};
            start(*window);select(*window);widget<QRadioButton>(*window,"radioButton_queryByID")->setChecked(true);
            widget<QLineEdit>(*window,"lineEdit_queryByid")->setText(QStringLiteral("fixture"));
            current->send_hook=[&](Socket&,UINT16 id){if(id==tl2gm_pre_query)delete window.data();};window->QueryRoleData();
            check(!window && !CGMManager::GetInstance().IsRunning(),"native query send callback can destroy the main window safely");stop();
        }
        {
            CompatibilityWindow window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            std::list<UINT32> ids{9};auto* input=widget<QTextEdit>(window,"textEdit_RoleID");
            widget<QRadioButton>(window,"radioButton_6")->setChecked(true);
            for(const auto& invalid:std::vector<QString>{QString(),QStringLiteral("4294967296"),QStringLiteral("-1"),QStringLiteral("1x"),QStringLiteral("1")+QChar(0),QStringLiteral("1 ").repeated(11)})
            {input->setPlainText(invalid);check(!window._Tag2ReadRoleID(ids) && ids==std::list<UINT32>{9},"compatibility ID validation preserves prior results without a nested error dialog");}
            input->setPlainText(QStringLiteral("0\t4294967295\n1"));check(window._Tag2ReadRoleID(ids) && ids==std::list<UINT32>({0,UINT32_MAX,1}),"compatibility parser keeps full unsigned IDs and whitespace format");
            widget<QRadioButton>(window,"radioButton_5")->setChecked(true);check(!window._Tag2ReadRoleID(ids) && ids.size()==3,"unsupported compatibility file mode leaves prior values intact");
            check(!window.OutputMessageBox(nullptr),"information helper rejects null text");
            bool shown=false;
            QTimer::singleShot(0,&window,[&]
            {
                auto* dialog=window.findChild<QDialog*>("gmInformationMessage");check(dialog!=nullptr,"scoped information dialog exists");
                auto* label=dialog->findChild<QLabel*>();
                check(label && label->textFormat()==Qt::PlainText && label->text()==QStringLiteral("<b>literal</b> 中"),"information dialog treats markup literally and decodes legacy Chinese explicitly");
                shown=true;dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok)->click();
            });
            check(window.OutputMessageBox("<b>literal</b> \xd6\xd0") && shown && !window.findChild<QDialog*>("gmInformationMessage"),"accepted information dialog is cleaned up before returning");
            CExecuting executing(&window);const auto timer=executing.findChild<QTimer*>();
            check(timer && timer->interval()==500 && timer->isSingleShot(),"compatibility progress dialog owns its original single-shot 500ms timer");
            check(executing.exec()==QDialog::Accepted && !timer->isActive(),"only the configured progress timer accepts the dialog once");stop();
        }
        {
            QPointer<gmtool> window=new gmtool;loader login;gm::ApplicationViews views(*window,login);
            struct Delete{QPointer<gmtool> window;~Delete(){delete window.data();}} delete_window{window};start(*window);select(*window);
            QTimer::singleShot(0,window.data(),[&]{delete window.data();});
            check(!window->OutputMessageBox("fixture") && !window,"information dialog safely unwinds when its parent is deleted");stop();
        }
        std::cout<<checks<<" GM window-input checks passed\n";return 0;
    }
    catch(const std::exception& error){CGMManager::GetInstance().UnInit();std::cerr<<error.what()<<'\n';return 1;}
}
