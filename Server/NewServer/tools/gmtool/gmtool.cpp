#include "stdafx.h"
#include "gmtool.h"
#include "loader.h"
#include "gm_log_view.h"
#include "gm_exchange_stats.h"
#include <QtCore/QPointer>
#include "gm_reports.h"
#include "gm_role_files.h"
#include "gm_ui.h"
#include <limits>
#include <QtWidgets//QApplication>
#include <QtCore/QTextCodec>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFileDialog>
#include "forbidden_chat.h"
#include "kick_user.h"
#include "mail.h"
#include "move_position.h"
#include "delete_item.h"
#include "view_guild.h"
#include "svr_select.h"
#include "gm_manager.h"
#include "message_handler.h"
#include "session_mgr.h"
#include "server_mgr.h"
#include "tl_config.h"
#include "my_tree_widget_item.h"
#include "svr_tree_widget_item.h"
#include "cmd_manager.h"
#include "base_role.h"
#include "executing.h"
#include "role_info.h"

#include "time_ex.h"
//#include "game_world.h"
#include "view_friend.h"
#include "gm_def.h"
#include "string_ex.h"
#include <time.h>
#include "gm_log.h"
#include "gmtool_def.h"
#include "data_mgr.h"
#include "logic.h"
#include "server_list.h"
#include "gm_version.h"
#include "role_gm_cmd.h"
#include "ld_widget.h"


#define TIME_ONE_HOUR_INDEX     (0)
#define TIME_ONE_DAY_INDEX     (1)
#define TIME_THREE_DAY_INDEX     (2)
#define TIME_SEVEN_DAY_INDEX     (3)
#define TIME_FOREVER_INDEX     (4)





gmtool::gmtool(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
    // initialize

    m_pLDWidget = NULL;
    memset(m_arrItemPoint, 0, sizeof(m_arrItemPoint));

    //ui.comboBox->insertItem(ROLE_SEARCH_NONE, " ");
    //ui.comboBox->insertItem(ROLE_SEARCH_BY_NAME, QString::fromLocal8Bit(cszUserSearchTypeName));
    //ui.comboBox->insertItem(ROLE_SEARCH_BY_ID, QString::fromLocal8Bit(cszUserSearchTypeID));
    //ui.comboBox->addItem(QString::fromLocal8Bit(cszUserSearchTypeAccount));

    ui.treeWidget->setColumnWidth(0, 55);
    ui.treeWidget->setColumnWidth(1, 55);
    ui.treeWidget->setColumnWidth(2, 60);
    ui.treeWidget->setColumnWidth(3, 100);
    ui.treeWidget->setColumnWidth(4, 80);
    ui.treeWidget->setColumnWidth(5, 80);
    ui.treeWidget->setColumnWidth(6, 130);
    ui.treeWidget->setColumnWidth(7, 40);
    ui.treeWidget->setColumnWidth(8, 60);
    ui.treeWidget->setColumnWidth(9, 40);

    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
    connect(ui.clearLogButton, SIGNAL(clicked()), this, SLOT(ClearLog()));
    connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(Login()));
    connect(ui.logoutButton, SIGNAL(clicked()), this, SLOT(Logout()));
    connect(ui.tabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(SecondInit(int)));
    //connect(ui.treeWidget_SvrList, SIGNAL(itemChanged(QTreeWidgetItem * item, int column)), this, SLOT(UpdateSelectedServer(QTreeWidgetItem * item, int column)));

    
    // tag 1
    connect(ui.radioButton_query_1, SIGNAL(clicked()), this, SLOT(SetFocusRoleName()));
    connect(ui.radioButton_query_2, SIGNAL(clicked()), this, SLOT(SetFocusUserName()));
    connect(ui.radioButton_queryByID, SIGNAL(clicked()), this, SLOT(SetFocusRoleIDByEdit()));
    connect(ui.radioButton_queryByFile, SIGNAL(clicked()), this, SLOT(SetFocusRoleIDByFile()));

    connect(ui.queryRoleID, SIGNAL(clicked()), this, SLOT(QueryRoleID()));
    connect(ui.query_role_data_button, SIGNAL(clicked()), this, SLOT(QueryRoleData()));
    connect(ui.viewButton, SIGNAL(clicked()), this, SLOT(ViewRoleData()));
    connect(ui.modifyButton, SIGNAL(clicked()), this, SLOT(ModifyRoleData()));
    connect(ui.toolButton, SIGNAL(clicked()), this, SLOT(OpenFile()));
    connect(ui.roleGMCmdhButton, SIGNAL(clicked()), this, SLOT(RoleGMCmd()));

    m_pQueryIDRadioGroup = new QButtonGroup(this);
    m_pQueryIDRadioGroup->addButton(ui.radioButton_query_1, GM_QUERY_BY_ROLE_NAME);
    m_pQueryIDRadioGroup->addButton(ui.radioButton_query_2, GM_QUERY_BY_USER_NAME);

    m_pQueryRoleRadioGroup = new QButtonGroup(this);
    m_pQueryRoleRadioGroup->addButton(ui.radioButton_queryByID, 1);
    m_pQueryRoleRadioGroup->addButton(ui.radioButton_queryByFile, 2);



    // tag 2
    connect(ui.radioButton_6, SIGNAL(clicked()), this, SLOT(SetFocusRoleIDByEdit2()));
    connect(ui.radioButton_5, SIGNAL(clicked()), this, SLOT(SetFocusRoleIDByFile2()));
    connect(ui.fc1_radioButton, SIGNAL(clicked()), this, SLOT(SetFocusForbidChat()));

    connect(ui.forbiddenChatButton, SIGNAL(clicked()), this, SLOT(ForBiddenChat()));
    connect(ui.unForbiddenChatButton, SIGNAL(clicked()), this, SLOT(UnforbidChat()));
    connect(ui.freezeRoleButton, SIGNAL(clicked()), this, SLOT(FreezeRole()));
    connect(ui.unfreezeRoleButton, SIGNAL(clicked()), this, SLOT(UnfreezeRole()));
    connect(ui.kickRoleButton, SIGNAL(clicked()), this, SLOT(KickRole()));
    connect(ui.mailButton, SIGNAL(clicked()), this, SLOT(Mail()));
    connect(ui.toolButton_2, SIGNAL(clicked()), this, SLOT(OpenFile_2()));
    m_pRadioGroup = new QButtonGroup(this);
    m_pRadioGroup->addButton(ui.radioButton_5, 1);
    m_pRadioGroup->addButton(ui.radioButton_6, 2);

    m_pFCRadioGroup = new QButtonGroup(this);
    m_pFCRadioGroup->addButton(ui.fc1_radioButton, 1);
    m_pFCRadioGroup->addButton(ui.fc2_radioButton, 2);

    ui.fc1_comboBox->clear();
    ui.fc1_comboBox->insertItem(TIME_ONE_HOUR_INDEX, QStringLiteral("1 Hour"));
    ui.fc1_comboBox->insertItem(TIME_ONE_DAY_INDEX, QStringLiteral("1 Day"));
    ui.fc1_comboBox->insertItem(TIME_THREE_DAY_INDEX, QStringLiteral("3 Days"));
    ui.fc1_comboBox->insertItem(TIME_SEVEN_DAY_INDEX, QStringLiteral("7 Days"));
    ui.fc1_comboBox->insertItem(TIME_FOREVER_INDEX, QStringLiteral("Permanent"));


    // tag 4
    connect(ui.comboBox, SIGNAL(clicked()), this, SLOT(SelectGroup()));
    connect(ui.annouceButton, SIGNAL(clicked()), this, SLOT(AnnounceFunc()));
    connect(ui.getAnnounceButton, SIGNAL(clicked()), this, SLOT(GetAnnounceList()));
    connect(ui.delAnnounceButton, SIGNAL(clicked()), this, SLOT(DelAnnounce()));

    QDateTime   qDataTime;
    qDataTime.setTime_t(time(NULL));
    ui.dateTimeEdit_announce->setDateTime(qDataTime);

    //connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(SelectSvrWithCheckBox()));
    //connect(ui.comboBox, SIGNAL(activated(const QString &)), this, SLOT(SearchRoleType(const QString &)));
    

    // tag 5

    ui.dateTimeEdit_log_1->setDateTime(qDataTime);
    ui.dateTimeEdit_log_2->setDateTime(qDataTime);

    ui.logComboBox->clear();
    ui.logComboBox->insertItem(GM_LOG_QUERY_ROLE_LOGIN, QStringLiteral("Account Login/Logout"));
    ui.logComboBox->insertItem(GM_LOG_QUERY_ROLE_LOGOUT, QStringLiteral("Character Logout"));
    ui.logComboBox->insertItem(GM_LOG_QUERY_ITEM, QStringLiteral("Item Transactions"));
    ui.logComboBox->insertItem(GM_LOG_QUERY_MONEY, QStringLiteral("Currency Transactions"));
    ui.logComboBox->insertItem(GM_LOG_QUERY_QUEST, QStringLiteral("Character Quests"));
    ui.logComboBox->insertItem(GM_LOG_QUERY_GM, QStringLiteral("GM Command Logs"));

    connect(ui.logComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeInputType(int)));
    connect(ui.searchButton_log, SIGNAL(clicked()), this, SLOT(LogSearch()));

    ui.lineEdit_log->setDisabled(true);
    _DisableTag5();

    ChangeInputType(ui.logComboBox->currentIndex());

    // tag 6
    connect(ui.queryfileButton, SIGNAL(clicked()), this, SLOT(QueryNameToID()));

    // tag 7
    connect(ui.pushButton_gmStrCmd, SIGNAL(clicked()), this, SLOT(SendGMStrCmd()));


    

    startTimer(125);

}

gmtool::~gmtool()
{
    if (gm::current_main_window() == this)
    {
        gm::detach_main_window(this);
        CGMManager::GetInstance().UnInit();
    }
}

CLDWidget* gmtool::GetLDWidget() { return m_pLDWidget.data(); }

void gmtool::timerEvent(QTimerEvent*)
{
    if (gm::current_main_window() != this) return;
    const QPointer<gmtool> alive(this);
    auto& manager=CGMManager::GetInstance();
    if(!manager.MainLoop())
    {
        if(!alive)return;
        manager.UnInit();
        if (auto* login = gm::current_login_dialog()) login->reject();
        if (!alive) return;
        ui.statusBar->showMessage(QStringLiteral("Application error, will exit!"));close();return;
    }
    if(alive && manager.IsRunning() && m_pLDWidget) m_pLDWidget->MainLoop();
}

void gmtool::Ok()
{

}

void gmtool::Execute()
{

}

void gmtool::Exit()
{
    close();
}

void gmtool::ClearLog()
{
    ui.textBrowser->clear();
}


void gmtool::Login() { SessionAction(true); }
void gmtool::Logout() { SessionAction(false); }

void gmtool::SessionAction(bool login)
{
    if (m_sessionSubmitting || gm::current_main_window() != this) return;
    const QPointer<gmtool> alive(this);
    m_sessionSubmitting = true;
    struct Reset
    {
        QPointer<gmtool> owner;
        bool* active;
        ~Reset() { if (owner) *active = false; }
    } reset{this, &m_sessionSubmitting};
    for (UINT32 id = 0; id < MAX_GM_SESSION_COUNT; ++id)
    {
        if (!m_arrItemPoint[id] || m_arrItemPoint[id]->checkState(0) != Qt::Checked) continue;
        const auto* session = CSessionMgr::GetInstance().GetGMSession(id);
        if (!CGMManager::GetInstance().IsRunning() || !session || !session->pSocketSession ||
            session->nConnectState != SESSION_CONNECTED ||
            session->eState != (login ? GM_SVR_STATE_REGISTERED : GM_SVR_STATE_LOGIN))
        {
            ui.statusBar->showMessage(QStringLiteral("Server status is unavailable. Please try again later."));
            return;
        }
        const auto name = session->name;
        const std::string account(CGMManager::GetInstance().GetUserName());
        const std::string password(CGMManager::GetInstance().GetUserPwd());
        bool sent = false;
        try
        {
            sent = (login ? CMessageHandler::GetInstance().DoLoginToGM(id, account.c_str(), password.c_str()) :
                CMessageHandler::GetInstance().DoLogoutToGM(id)) != FALSE;
        }
        catch (...) { sent = false; }
        if (!alive) return;
        if (!sent) { ui.statusBar->showMessage(QStringLiteral("Failed to send. Please try again later.")); return; }
        CGMLog::GetInstance().Log("{}Server[{}]....", login ? "Login" : "Logout",
            QString::fromUtf8(name.c_str()).toLocal8Bit().constData());
        return; // The legacy UI operates on the first selected server.
    }
    ui.statusBar->showMessage(QStringLiteral("Please select a server first."));
}

void gmtool::SecondInit(int nIndex)
{
    INT32       nRetCode = 0;
    // index 0 - n
    if (nIndex == 1)
    {
        UINT32 dwSessionID = 0;

        INF("second init begin");
        for (INT32 nIndex = 0; nIndex < MAX_GM_SESSION_COUNT; ++nIndex)
        {
            nRetCode = IsServerSelected(nIndex);
            if (nRetCode == TRUE)
            {
                dwSessionID = nIndex;
                break;
            }
        }
        if (MAX_GM_SESSION_COUNT == dwSessionID)
        {
            //QMessageBox::critical(this, "critical", QStringLiteral("请选择执行区服"), QMessageBox::Yes);
            return;
        }

        INF("second init begin item map");
        ui.mailItem_comboBox->clear();
        INT32 i = 1;
        for (const auto& item : CDataMgr::GetInstance().GetItems(dwSessionID))
        {
            QString     str;
            
            str.append(item.szName);

            str.append(" - ");
            
            char        szNum[64] = { 0 };
            gm_itoa(item.dwTempID, szNum);

            str.append(szNum);


            ui.mailItem_comboBox->insertItem(i, str);
            ++i;
        }
        INF("second init item map end i[%d]", i);
    }
    else if ( nIndex == 3 )
    {
        // tag 4
        QDateTime   qDataTime;
        qDataTime.setTime_t(time(NULL));
        ui.dateTimeEdit_announce->setDateTime(qDataTime);

        ui.announceWidget->clear();
    }
    else if (nIndex == 4)
    {
        QDateTime   qDataTime;
        qDataTime.setTime_t(time(NULL));
        ui.dateTimeEdit_log_1->setDateTime(qDataTime);
        ui.dateTimeEdit_log_2->setDateTime(qDataTime);

        INT32   nComboIndex = ui.logComboBox->currentIndex();
        ChangeInputType(nComboIndex);
    }
Exit0:
    return;
}

void gmtool::UpdateSelectedServer(QTreeWidgetItem * item, int column)
{
    INT32       nRetCode = 0;
    nRetCode = CServerMgr::GetInstance().UpdateSelectServer();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

//void gmtool::SearchRoleType(const QString & selectItemText)
//{
//    //if (selectItemText == QString::fromLocal8Bit(cszUserSearchTypeName))
//    //{
//    //    ui.label_2->setText(QString::fromLocal8Bit(cszUserSearchTypeName));
//    //    ui.label_2->show();
//    //    ui.lineEdit->show();
//    //}
//    //else if (selectItemText == QString::fromLocal8Bit(cszUserSearchTypeID))
//    //{
//    //    ui.label_2->setText(QString::fromLocal8Bit(cszUserSearchTypeID));
//    //    ui.label_2->show();
//    //    ui.lineEdit->show();
//    //}
//    //else if (selectItemText == QString::fromLocal8Bit(cszUserSearchTypeAccount))
//    //{
//    //    ui.label_2->setText(QString::fromLocal8Bit(cszUserSearchTypeAccount));
//    //    ui.label_2->show();
//    //    ui.lineEdit->show();
//    //}
//    //else
//    //{
//    //    DisableAllInputWidget();
//    //}
//    //ui.lineEdit->clear();
//    
//}

void gmtool::SetFocusRoleName()
{
    ui.lineEdit_query_1->setFocus();
}

void gmtool::SetFocusUserName()
{
    ui.lineEdit_query_2->setFocus();
}

void gmtool::SetFocusRoleIDByEdit()
{
    ui.lineEdit_queryByid->setFocus();
}

void gmtool::SetFocusRoleIDByFile()
{
    ui.lineEdit_queryByFile->setFocus();
}

void gmtool::QueryRoleID()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto peers=gm::selected_peers(false);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    const auto kind=m_pQueryIDRadioGroup->checkedId();
    if (kind!=GM_QUERY_BY_ROLE_NAME && kind!=GM_QUERY_BY_USER_NAME)
    { ui.statusBar->showMessage(QStringLiteral("Please select a query type"));return; }
    const auto name=gm::text_field(kind==GM_QUERY_BY_ROLE_NAME ? ui.lineEdit_query_1->text() : ui.lineEdit_query_2->text(),
        USER_NAME_LEN,QStringLiteral("Query Name"));
    if (!name) { ui.statusBar->showMessage(name.error());return; }
    ui.treeWidget_queryroleid->clear();
    if (!alive) return;
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,false))
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed. Please try again."));return; }
    const auto sent=manager.QueryRoleID(kind,name->c_str());
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command"));
}

void gmtool::QueryRoleData()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    if (!ui.radioButton_queryByID->isChecked()) { ui.statusBar->showMessage(QStringLiteral("Please select a single role name"));return; }
    const auto name=gm::text_field(ui.lineEdit_queryByid->text(),ROLE_NAME_LEN,QStringLiteral("Role Name"));
    if (!name) { ui.statusBar->showMessage(name.error());return; }
    auto command=std::static_pointer_cast<QUERY_ROLE_DATA>(manager.NewCmd(CMD_QUERY_ROLE_DATA));
    if (!gm::copy_field(command->szRoleName,*name)) return;
    ui.treeWidget->clear();
    if (!alive) return;
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true))
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed. Please try again."));return; }
    CDataMgr::GetInstance().ClearRole();
    const auto sent=manager.PreQuery(CMD_QUERY_ROLE_DATA,name->c_str(),command);
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command"));
}

void gmtool::ViewRoleData()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    if (CCmdManager::GetInstance().IsCmdExecuting())
    { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto* item=ui.treeWidget->currentItem();
    bool valid=false;
    const auto role=item ? item->data(2,Qt::UserRole).toUInt(&valid) : 0;
    if (!valid || !role || !CDataMgr::GetInstance().FindRole(role))
    { ui.statusBar->showMessage(QStringLiteral("Please select valid role data"));return; }
    const QPointer<CRoleInfo> dialog=new CRoleInfo(0,role,FALSE,this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void gmtool::ModifyRoleData()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    if (!ui.radioButton_queryByID->isChecked()) { ui.statusBar->showMessage(QStringLiteral("Please select a single role name"));return; }
    const auto name=gm::text_field(ui.lineEdit_queryByid->text(),ROLE_NAME_LEN,QStringLiteral("Role Name"));
    if (!name) { ui.statusBar->showMessage(name.error());return; }
    if (!gm::confirm_action(*this,QStringLiteral("Please confirm that the role “%1” is offline, otherwise it may cause data inconsistency. Continue modifying?")
        .arg(QString::fromUtf8(name->c_str()))) || !alive) return;
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true))
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed. Please try again."));return; }
    auto command=std::static_pointer_cast<LOAD_ROLE_DATA>(manager.NewCmd(CMD_LOAD_ROLE_DATA));
    if (!gm::copy_field(command->szRoleName,*name)) return;
    ui.treeWidget->clear();
    if (!alive) return;
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true))
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed. Please try again."));return; }
    const auto sent=manager.PreQuery(CMD_LOAD_ROLE_DATA,name->c_str(),command);
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command"));
}

void gmtool::OpenFile() { PickRoleFile(true); }
void gmtool::PickRoleFile(bool primary)
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    const QPointer<QLineEdit> destination(primary ? ui.lineEdit_queryByFile : ui.lineEdit);
    gm::DialogScope scope(new QFileDialog(this));
    const auto dialog=scope.Get();
    dialog->setObjectName(QStringLiteral("gmRoleFileDialog"));
    dialog->setOption(QFileDialog::DontUseNativeDialog);
    dialog->setWindowTitle(QStringLiteral("Select File"));dialog->setDirectory(QStringLiteral("."));
    dialog->setFileMode(QFileDialog::ExistingFile);
    if (dialog->exec()!=QDialog::Accepted || !alive || !dialog || !destination) return;
    const auto files=dialog->selectedFiles();
    if (!files.isEmpty()) destination->setText(files.front());
}

void gmtool::RoleGMCmd()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    if (!ui.radioButton_queryByID->isChecked()) { ui.statusBar->showMessage(QStringLiteral("Please select a single role name"));return; }
    const auto name=gm::text_field(ui.lineEdit_queryByid->text(),ROLE_NAME_LEN,QStringLiteral("Role Name"));
    if (!name) { ui.statusBar->showMessage(name.error());return; }
    gm::DialogScope scope(new CRoleGMCmd(this));
    const auto dialog=scope.Get();
    if (dialog->exec()!=QDialog::Accepted || !alive || !dialog || !dialog->IsExeCmd()) return;
    const std::string text(dialog->GetBuffer()),reason(dialog->GetReason());
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true))
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed. Please try again."));return; }
    auto command=std::static_pointer_cast<EXE_ROLE_CMD>(manager.NewCmd(CMD_EXE_ROLE_GM_CMD));
    if (!gm::copy_field(command->szRoleName,*name) || !gm::copy_field(command->szGM,text) ||
        !gm::copy_field(command->szReason,reason)) return;
    const auto sent=manager.PreQuery(CMD_EXE_ROLE_GM_CMD,name->c_str(),command);
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command"));
}

// tag 2
void gmtool::SetFocusRoleIDByEdit2()
{
    ui.textEdit_RoleID->setFocus();
}

void gmtool::SetFocusRoleIDByFile2()
{
    ui.lineEdit->setFocus();
}

void gmtool::SetFocusForbidChat()
{
    ui.fc1_comboBox->showPopup();
}



void gmtool::ForBiddenChat() { BulkAction(CMD_FORBIDDEN_CHAT); }
void gmtool::UnforbidChat() { BulkAction(CMD_FORBIDDEN_CHAT,true); }
void gmtool::FreezeRole() { BulkAction(CMD_FREEZE_ROLE); }
void gmtool::UnfreezeRole() { BulkAction(CMD_FREEZE_ROLE,true); }
void gmtool::KickRole() { BulkAction(CMD_KICK_ROLE); }
void gmtool::Mail() { BulkAction(CMD_SEND_MAIL); }

void gmtool::BulkAction(INT32 kind,bool revoke)
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    std::list<ROLE_RESULT> roles;
    std::filesystem::path report_path;
    if (ui.radioButton_6->isChecked())
    {
        const auto name=gm::text_field(ui.textEdit_RoleID->toPlainText(),ROLE_NAME_LEN,QStringLiteral("Role Name"));
        if (!name) { ui.statusBar->showMessage(name.error());return; }
        ROLE_RESULT row{};row.nResult=errUnknownError;
        if (!gm::copy_field(row.szRoleName,*name)) return;
        roles.push_back(row);
    }
    else if (ui.radioButton_5->isChecked())
    {
        const auto filename=gm::text_field(ui.lineEdit->text(),std::numeric_limits<std::size_t>::max(),QStringLiteral("File Name"));
        if (!filename) { ui.statusBar->showMessage(filename.error());return; }
        report_path=gm::utf8_path(*filename);
        auto batch=gm::read_role_batch(report_path);
        if (!batch) { ui.statusBar->showMessage(QString::fromUtf8(batch.error().c_str()));return; }
        if (batch->empty()) { ui.statusBar->showMessage(QStringLiteral("Valid role name count is 0"));return; }
        roles=std::move(*batch);
    }
    else { ui.statusBar->showMessage(QStringLiteral("Please select role input method"));return; }
    UINT32 duration=0;
    if (!revoke && (kind==CMD_FORBIDDEN_CHAT || kind==CMD_FREEZE_ROLE))
    {
        if (ui.fc1_radioButton->isChecked())
        {
            using namespace std::chrono;
            constexpr std::array<seconds,5> presets{hours{1},days{1},days{3},days{7},seconds{FOREVER_TIME}};
            const auto index=ui.fc1_comboBox->currentIndex();
            if (index<0 || static_cast<std::size_t>(index)>=presets.size())
            { ui.statusBar->showMessage(QStringLiteral("Please select a valid time"));return; }
            duration=static_cast<UINT32>(presets[index].count());
        }
        else if (ui.fc2_radioButton->isChecked())
        {
            const auto minutes=gm::numeric_field<INT64>(ui.forbiddenChatTimeEdit->text(),QStringLiteral("Minutes"));
            if (!minutes) { ui.statusBar->showMessage(minutes.error());return; }
            const auto seconds=gm::minute_duration(*minutes,FOREVER_TIME);
            if (!seconds) { ui.statusBar->showMessage(QStringLiteral("Minutes must be a positive integer, use -1 for permanent"));return; }
            duration=*seconds;
        }
        else { ui.statusBar->showMessage(QStringLiteral("Please select a time"));return; }
    }
    const auto make=[&]<class T>()
    {
        auto command=std::static_pointer_cast<T>(manager.NewCmd(kind));
        command->report_path=std::move(report_path);command->roleList=std::move(roles);
        if constexpr (requires { command->llTime; }) command->llTime=duration;
        return command;
    };
    CommandPtr command;
    QString question;
    switch (kind)
    {
    case CMD_FORBIDDEN_CHAT:
        command=make.operator()<FORBIDDEN_CHAT>();question=revoke ? QStringLiteral("Confirm Unban Chat?") : QStringLiteral("Confirm Ban Chat?");break;
    case CMD_FREEZE_ROLE:
        command=make.operator()<FREEZE_ROLE>();question=revoke ? QStringLiteral("Confirm Unfreeze Role?") : QStringLiteral("Confirm Freeze Role?");break;
    case CMD_KICK_ROLE:
        command=make.operator()<KICK_ROLE>();question=QStringLiteral("Confirm Force Role Offline?");break;
    case CMD_SEND_MAIL:
    {
        const auto item=gm::numeric_field<UINT32>(ui.lineEdit_mail_2->text(),QStringLiteral("Item ID"),true);
        const auto count=gm::numeric_field<INT32>(ui.lineEdit_mail_3->text(),QStringLiteral("Item Count"),true);
        if (!item || !count) { ui.statusBar->showMessage(!item ? item.error() : count.error());return; }
        if (*count<0 || ((*item==0)!=(*count==0)))
        { ui.statusBar->showMessage(QStringLiteral("Item ID and Count must be either both 0 or both positive"));return; }
        // Empty subject/body are valid legacy wire values, including attachment-only mail.
        const auto subject=gm::text_field(ui.lineEdit_mail_1->text(),MAIL_SUBJECT_LEN,QStringLiteral("Subject"),0);
        const auto content=gm::text_field(ui.textEdit_mail->toPlainText(),MAIL_CONTENT_LEN,QStringLiteral("Content"),0);
        const auto reason=gm::text_field(ui.mailReasonEdit->toPlainText(),GM_REASON_LEN,QStringLiteral("Reason"),*item ? 1 : 0);
        if (!subject || !content || !reason)
        { ui.statusBar->showMessage(!subject ? subject.error() : !content ? content.error() : reason.error());return; }
        auto mail=make.operator()<SEND_MAIL>();mail->dwItemID=*item;mail->nItemNum=*count;
        if (!gm::copy_field(mail->szTitle,*subject) || !gm::copy_field(mail->szContent,*content) || !gm::copy_field(mail->szReason,*reason)) return;
        command=std::move(mail);question=QStringLiteral("Confirm Send Mail?");break;
    }
    default: return;
    }
    const auto submit=[&]<class T>()
    {
        const auto typed=std::static_pointer_cast<T>(command);
        const auto name=std::string(typed->roleList.front().szRoleName);
        question+=QStringLiteral("（%1 characters）").arg(static_cast<qulonglong>(typed->roleList.size()));
        if (!gm::confirm_action(*this,question) || !alive) return;
        if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true))
        { ui.statusBar->showMessage(QStringLiteral("Execution status has changed, please try again"));return; }
        const auto sent=manager.PreQuery(kind,name.c_str(),command);
        if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command"));
    };
    switch (kind)
    {
    case CMD_FORBIDDEN_CHAT: submit.operator()<FORBIDDEN_CHAT>();break;
    case CMD_FREEZE_ROLE: submit.operator()<FREEZE_ROLE>();break;
    case CMD_KICK_ROLE: submit.operator()<KICK_ROLE>();break;
    case CMD_SEND_MAIL: submit.operator()<SEND_MAIL>();break;
    }
}

void gmtool::OpenFile_2() { PickRoleFile(false); }

BOOL gmtool::_SelectGroup(INT32& group)
{
    if (!CServerMgr::GetInstance().IsSingleSelected()) return FALSE;
    const auto index=ui.comboBox->currentIndex();
    if (index<0 || index>=MAX_SERVER_GROUP_COUNT) return FALSE;
    group=index;return TRUE;
}

void gmtool::AnnounceFunc()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    INT32 group;
    if (!_SelectGroup(group)) { ui.statusBar->showMessage(QStringLiteral("Please select a valid group"));return; }
    const auto date=ui.dateTimeEdit_announce->dateTime();const auto epoch=date.toSecsSinceEpoch();
    if (!date.isValid() || epoch<0 || static_cast<UINT64>(epoch)>std::numeric_limits<UINT32>::max())
    { ui.statusBar->showMessage(QStringLiteral("Announcement time is out of protocol range"));return; }
    const auto begin=static_cast<UINT32>(std::chrono::floor<std::chrono::minutes>(std::chrono::seconds{epoch}).count()*60);
    const auto interval=gm::numeric_field<INT32>(ui.lineEdit_announce_1->text(),QStringLiteral("Interval (seconds)"));
    const auto repeats=gm::numeric_field<INT32>(ui.lineEdit_announce_2->text(),QStringLiteral("Repeat Count"));
    if (!interval || !repeats) { ui.statusBar->showMessage(!interval ? interval.error() : repeats.error());return; }
    if (*interval<=0 || *repeats<=0) { ui.statusBar->showMessage(QStringLiteral("Interval and repeat count must be positive"));return; }
    auto text=gm::text_field(ui.textEdit_announce->toPlainText(),GM_ANNOUNCE_LENGTH,QStringLiteral("Announcement"));
    if (!text) { ui.statusBar->showMessage(text.error());return; }
    gm::normalize_line_breaks(std::span<char>(*text));
    if (!gm::confirm_action(*this,QStringLiteral("Confirm Send Announcement?")) || !alive) return;
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true) || ui.comboBox->currentIndex()!=group)
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed, please try again"));return; }
    const auto sent=manager.SendAnnounce(group,begin,*interval,*repeats,text->c_str());
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command"));
}

void gmtool::GetAnnounceList()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    INT32 group;
    if (!_SelectGroup(group)) { ui.statusBar->showMessage(QStringLiteral("Please select a valid group"));return; }
    ui.announceWidget->clear();
    if (!alive) return;
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true) || ui.comboBox->currentIndex()!=group)
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed, please try again"));return; }
    CDataMgr::GetInstance().ClearAnnounceList();
    const auto sent=manager.QueryAnnounceList(group);
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command"));
}

void gmtool::DelAnnounce()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command executing..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    INT32 group;
    if (!_SelectGroup(group)) { ui.statusBar->showMessage(QStringLiteral("Please select a valid group"));return; }
    const auto* selected=ui.announceWidget->currentItem();
    bool valid=false;const auto serial=selected ? selected->data(0,Qt::UserRole).toUInt(&valid) : 0;
    bool server_valid=false;const auto server=selected ? selected->data(0,Qt::UserRole+1).toInt(&server_valid) : 0;
    const auto* session=CSessionMgr::GetInstance().GetGMSession(peers->front().id);
    if (!valid || !server_valid || !session || server!=session->nSvrID)
    { ui.statusBar->showMessage(QStringLiteral("Please select the announcement for the current server"));return; }
    if (!gm::confirm_action(*this,QStringLiteral("Confirm deletion of announcement %1?").arg(serial)) || !alive) return;
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true) || ui.comboBox->currentIndex()!=group)
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed, please try again"));return; }
    const auto sent=manager.DelAnnounce(group,serial);
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command"));
}

// tag 2
void gmtool::ChangeInputType(int nIndex)
{
    if (nIndex < GM_LOG_QUERY_NONE || nIndex > GM_QUERY_NUM)
        return;

    ui.label_log_0->setDisabled(true);
    ui.lineEdit_log_0->setDisabled(true);

    ui.label_log_1->setDisabled(true);
    ui.lineEdit_log_1->setDisabled(true);

    ui.label_log_2->setDisabled(true);
    ui.lineEdit_log_2->setDisabled(true);

    ui.label_log_3->setDisabled(true);
    ui.lineEdit_log_3->setDisabled(true);

    ui.label_log_4->setDisabled(true);
    ui.lineEdit_log_4->setDisabled(true);

    ui.label_log_5->setDisabled(true);
    ui.lineEdit_log_5->setDisabled(true);

    ui.label_log_6->setDisabled(true);
    ui.lineEdit_log_6->setDisabled(true);


    switch (nIndex)
    {
    case GM_LOG_QUERY_ROLE_LOGIN:
    {
        ui.label_log_0->setDisabled(false);
        ui.lineEdit_log_0->setDisabled(false);
    }
        break;
    case GM_LOG_QUERY_ROLE_LOGOUT:
    {
        ui.label_log_1->setDisabled(false);
        ui.lineEdit_log_1->setDisabled(false);
    }
        break;
    case GM_LOG_QUERY_ITEM:
    {
        ui.label_log_1->setDisabled(false);
        ui.lineEdit_log_1->setDisabled(false);

        ui.label_log_2->setDisabled(false);
        ui.lineEdit_log_2->setDisabled(false);

        ui.label_log_3->setDisabled(false);
        ui.lineEdit_log_3->setDisabled(false);
    }
        break;
    case GM_LOG_QUERY_MONEY:
    {
        ui.label_log_1->setDisabled(false);
        ui.lineEdit_log_1->setDisabled(false);

        ui.label_log_4->setDisabled(false);
        ui.lineEdit_log_4->setDisabled(false);

        ui.label_log_5->setDisabled(false);
        ui.lineEdit_log_5->setDisabled(false);
    }
        break;
    case GM_LOG_QUERY_QUEST:
    {
        ui.label_log_1->setDisabled(false);
        ui.lineEdit_log_1->setDisabled(false);

        ui.label_log_6->setDisabled(false);
        ui.lineEdit_log_6->setDisabled(false);
    }
        break;
    default:
        break;
    }


}

void gmtool::LogSearch()
{
    if (m_logSubmitting) return;
    m_logSubmitting=true;
    const QPointer<gmtool> alive(this);
    struct Reset { QPointer<gmtool> owner;~Reset(){if (owner) owner->m_logSubmitting=false;} } reset{alive};
    if (CCmdManager::GetInstance().IsCmdExecuting() || !CServerMgr::GetInstance().IsSingleSelected())
    { ui.statusBar->showMessage(QStringLiteral("Please select a single server and wait for the current command to complete"));return; }
    const auto type=ui.logComboBox->currentIndex();
    const auto query=_SerializeQueryLogStr(type);
    const auto begin=ui.dateTimeEdit_log_1->dateTime(),end=ui.dateTimeEdit_log_2->dateTime();
    const auto window=gm::exchange_window(begin.toSecsSinceEpoch(),end.toSecsSinceEpoch());
    if (!query) { ui.statusBar->showMessage(QString::fromUtf8(query.error().c_str()));return; }
    if (!begin.isValid() || !end.isValid() || !window)
    { ui.statusBar->showMessage(QStringLiteral("Please select a valid and ordered date range"));return; }
    if (!_PreDealGameLogFile()) { ui.statusBar->showMessage(QStringLiteral("Failed to create the specified log output file"));return; }
    // Clear before issuing: a synchronous reply must remain visible after Submit returns.
    ui.textEdit->clear();
    if (!alive) return;
    ui.statusBar->clearMessage();
    const auto sent=CCmdManager::GetInstance().QueryLog(type,window->begin,window->end,query->c_str());
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command, please try again"));
}

BOOL gmtool::PutsGameLog(INT32,BOOL truncated,INT32 length,const char* bytes)
{
    if (!bytes || length<1 || length>0xFFFF || (truncated!=FALSE && truncated!=TRUE)) return FALSE;
    const QPointer<gmtool> alive(this);
    const auto shown=gm::show_game_log(*ui.textEdit,{bytes,static_cast<std::size_t>(length)},std::filesystem::path("..")/"output.txt");
    if (!alive) return shown.has_value();
    if (!shown) ui.statusBar->showMessage(QString::fromUtf8(shown.error().c_str()));
    else if (truncated) ui.statusBar->showMessage(QStringLiteral("Query data is too long, please shorten the query range"));
    else ui.statusBar->clearMessage();
    return shown.has_value();
}

// tag 6
void gmtool::QueryNameToID()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command execution in progress..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    const auto path=gm::text_field(ui.queryfile_lineEdit->text(),std::numeric_limits<std::size_t>::max(),QStringLiteral("File Name"));
    if (!path) { ui.statusBar->showMessage(path.error());return; }
    auto names=gm::read_name_queries(gm::utf8_path(*path));
    if (!names) { ui.statusBar->showMessage(QString::fromUtf8(names.error().c_str()));return; }
    if (names->empty()) { ui.statusBar->showMessage(QStringLiteral("No valid character names in the file"));return; }
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true))
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed, please try again"));return; }
    const auto sent=manager.QueryNameToID(*names);
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command, please try again"));
}

void gmtool::SendGMStrCmd()
{
    if (m_actionSubmitting || gm::current_main_window()!=this) return;
    gm::UiAction action(*this,m_actionSubmitting);
    const QPointer<gmtool> alive(this);
    auto& manager=CCmdManager::GetInstance();
    if (manager.IsCmdExecuting()) { ui.statusBar->showMessage(QStringLiteral("Command execution in progress..."));return; }
    const auto peers=gm::selected_peers(true);
    if (!peers) { ui.statusBar->showMessage(peers.error());return; }
    const auto text=gm::text_field(ui.textEdit_gmStrCmd->toPlainText(),MAX_GM_COMMAND_LEN,QStringLiteral("GM Commands"));
    if (!text) { ui.statusBar->showMessage(text.error());return; }
    if (!gm::confirm_action(*this,QStringLiteral("Are you sure you want to execute this GM command?")) || !alive) return;
    if (manager.IsCmdExecuting() || !gm::selection_matches(*peers,true))
    { ui.statusBar->showMessage(QStringLiteral("Execution status has changed, please try again"));return; }
    const auto sent=manager.SendGMStrCmd(text->c_str());
    if (alive && !sent) ui.statusBar->showMessage(QStringLiteral("Failed to send command, please try again"));
}

BOOL gmtool::Init()
{
    std::vector<std::pair<UINT32, std::unique_ptr<CSvrTreeWidgetItem>>> prepared;
    for (UINT32 id = 0; id < MAX_GM_SESSION_COUNT; ++id)
    {
        const auto* session = CSessionMgr::GetInstance().GetGMSession(id);
        if (!session || session->dwType != stGM) continue;
        auto item = std::make_unique<CSvrTreeWidgetItem>();
        item->SetSessionID(id);
        item->setCheckState(0, Qt::Unchecked);
        item->setText(0, QString::fromUtf8(session->name.c_str()));
        item->setForeground(0, QBrush(QColor(255, 0, 0, 255), Qt::SolidPattern));
        prepared.emplace_back(id, std::move(item));
    }
    // Qt owns published tree items. Invalidate the borrowed cache before clearing them.
    std::fill(std::begin(m_arrItemPoint), std::end(m_arrItemPoint), nullptr);
    ui.treeWidget_SvrList->clear();
    for (auto& [id, item] : prepared)
    {
        auto* raw = item.get();
        ui.treeWidget_SvrList->addTopLevelItem(raw);
        item.release();
        m_arrItemPoint[id] = raw;
    }
    connect(ui.treeWidget_SvrList, &QTreeWidget::itemClicked, this,
        &gmtool::UpdateSelectedServer, Qt::UniqueConnection);
    return TRUE;
}

BOOL gmtool::SetTitle()
{
    setWindowTitle(QStringLiteral("Account: %1    Version: %2---%3")
        .arg(QString::fromUtf8(CGMManager::GetInstance().GetUserName()))
        .arg(GM_MAJOR_VERSION).arg(GM_MINOR_VERSION));
    return TRUE;
}

BOOL gmtool::IsServerSelected(UINT32 dwSessionID)
{
    {
    LOG_PROCESS_ERROR(dwSessionID < MAX_GM_SESSION_COUNT);

    if (!m_arrItemPoint[dwSessionID])
        return FALSE;

    Qt::CheckState state = m_arrItemPoint[dwSessionID]->checkState(0);
    if (state != Qt::Checked)
        return FALSE;

    return TRUE;
    }
Exit0:
    return FALSE;
}

BOOL gmtool::UpdateSvrNameColor(UINT32 dwSessionID, GM_SVR_STATE eState)
{
    QBrush brush(QColor(0, 0, 0, 255));
    brush.setStyle(Qt::SolidPattern);

    LOG_PROCESS_ERROR(dwSessionID < MAX_GM_SESSION_COUNT);
    LOG_PROCESS_ERROR(m_arrItemPoint[dwSessionID]);

    switch (eState)
    {
    case GM_SVR_STATE_NONE:
        {
            
        }
        break;
    case GM_SVR_STATE_REGISTERED:   // Yellow
        {
            brush.setColor(QColor(200,200,0,255));
        }
        break;
    case GM_SVR_STATE_LOGIN:        // Green
        {
            brush.setColor(QColor(0,255,0,255));
        }
        break;
    case GM_SVR_STATE_DISCONNECT:       // Red
        {
            brush.setColor(QColor(255,0,0,255));
        }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    m_arrItemPoint[dwSessionID]->setForeground(0, brush);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL gmtool::OutputLog(const char* text)
{
    return text ? OutputLog(std::string_view(text)) : FALSE;
}
BOOL gmtool::OutputLog(std::string_view text)
{
    return gm::show_tool_log(*ui.textBrowser,text,std::chrono::system_clock::now()).has_value();
}
BOOL gm::output_tool_log(std::string_view text)
{
    auto* window = gm::current_main_window();
    return window ? window->OutputLog(text) : FALSE;
}

BOOL gmtool::OutputMessageBox(const char *pcszStr)
{
    if (!pcszStr) return FALSE;
    const QPointer<gmtool> alive(this);
    const auto* codec=QTextCodec::codecForName("GB18030");
    const auto text=codec ? codec->toUnicode(pcszStr) : QString::fromLocal8Bit(pcszStr);
    gm::message_dialog(*this,text,QDialogButtonBox::Ok,QDialogButtonBox::Ok,"gmInformationMessage");
    return !alive.isNull();
}

BOOL gmtool::_ReadRoleFromFile(const char* filename, std::list<ROLE_RESULT>& roles)
{
    if (!filename) return FALSE;
    auto result = gm::read_role_batch(gm::utf8_path(filename));
    if (!result) { OutputLog(result.error().c_str()); return FALSE; }
    roles.swap(*result);
    return TRUE;
}

BOOL gmtool::UpdateRoleResult(const char* filename, INT32 kind, std::list<ROLE_RESULT>& roles)
{
    if (!filename) return FALSE;
    const auto result = gm::write_role_report(gm::utf8_path(filename), kind, roles);
    if (!result) OutputLog(result.error().c_str());
    return result.has_value();
}

BOOL gmtool::_ReadRoleIDFromFile(const char* filename, std::list<UINT32>& ids)
{
    if (!filename) return FALSE;
    auto result = gm::read_role_ids(gm::utf8_path(filename));
    if (!result) { OutputLog(result.error().c_str()); return FALSE; }
    ids.swap(*result);
    return TRUE;
}

BOOL gmtool::_ReadRoleNameFromFile(const char* filename, std::map<INT32, ROLE_NAME_DATA>& names)
{
    if (!filename) return FALSE;
    const auto path = gm::utf8_path(filename);
    auto result = gm::read_name_queries(path.is_absolute() ? path : std::filesystem::path("..") / path);
    if (!result) { OutputLog(result.error().c_str()); return FALSE; }
    names.swap(*result);
    return TRUE;
}

BOOL gmtool::_Tag2ReadRoleID(ROLEID_LIST& roleIDList)
{
    if (!ui.radioButton_6->isChecked())
    { ui.statusBar->showMessage(QStringLiteral("Please select a character ID input method"));return FALSE; }
    const auto bytes=ui.textEdit_RoleID->toPlainText().toUtf8();
    auto parsed=gm::parse_role_ids({bytes.constData(),static_cast<std::size_t>(bytes.size())});
    if (!parsed) { ui.statusBar->showMessage(QStringLiteral("Invalid character ID format or failed to retrieve character ID"));return FALSE; }
    if (parsed->size()>10) { ui.statusBar->showMessage(QStringLiteral("Too many character IDs"));return FALSE; }
    roleIDList.swap(*parsed);
    return TRUE;
}

void gmtool::_DisableTag5()
{
    ui.lineEdit_log_1->setDisabled(true);
    ui.lineEdit_log_2->setDisabled(true);
    ui.lineEdit_log_3->setDisabled(true);
}

BOOL gmtool::_PreDealGameLogFile()
{
    return gm::clear_game_log(std::filesystem::path("..")/"output.txt").has_value();
}

std::expected<std::string,std::string> gmtool::_SerializeQueryLogStr(INT32 type)
{
    const std::array fields{ui.lineEdit_log_0->text().toUtf8(),ui.lineEdit_log_1->text().toUtf8(),ui.lineEdit_log_2->text().toUtf8(),
        ui.lineEdit_log_3->text().toUtf8(),ui.lineEdit_log_4->text().toUtf8(),ui.lineEdit_log_5->text().toUtf8(),ui.lineEdit_log_6->text().toUtf8()};
    gm::LogQueryFields views;
    for (std::size_t index=0;index<fields.size();++index) views[index]={fields[index].constData(),static_cast<std::size_t>(fields[index].size())};
    return gm::serialize_log_query(type,views);
}

BOOL gmtool::UpdateAnnounceList()
{
    const auto records = CDataMgr::GetInstance().GetAnnounceList();
    std::vector<std::unique_ptr<CMyTreeWidgetItem>> items;
    items.reserve(records.size());
    for (const auto& record : records)
    {
        if (record.nGroupID < 0 || record.nGroupID >= MAX_SERVER_GROUP_COUNT ||
            !std::memchr(record.szText, 0, sizeof(record.szText))) return FALSE;
        auto item = std::make_unique<CMyTreeWidgetItem>();
        item->SetID(static_cast<INT32>(record.dwSerial));
        item->setData(0, Qt::UserRole, record.dwSerial);
        item->setData(0, Qt::UserRole+1, record.nSvrID);
        QString group = QStringLiteral("Group name not found");
        if (record.nGroupID == 0) group = QStringLiteral("All Regions");
        else if (const auto* server = GetServer(record.nSvrID))
        {
            const auto& name = server->groupList[record.nGroupID].szGroupName;
            const auto* end = static_cast<const char*>(std::memchr(name, 0, sizeof(name)));
            if (!end) return FALSE;
            group = QString::fromUtf8(name, static_cast<int>(end-name));
        }
        item->setText(0, group);
        item->setText(1, QString::number(record.dwIntervalTime));
        item->setText(2, QString::number(record.dwRepeatNum));
        item->setText(3, QString::fromUtf8(record.szText));
        items.push_back(std::move(item));
    }
    const QPointer<gmtool> alive(this);
    ui.announceWidget->clear();
    if (!alive) return FALSE;
    for (auto& item : items)
    {
        ui.announceWidget->addTopLevelItem(item.get());
        item.release();
        if (!alive) return FALSE;
    }
    return TRUE;
}

BOOL gmtool::DelAnnounce(UINT32 dwAnnounceSerial)
{
    INT32       nRetCode = 0;

    nRetCode = CDataMgr::GetInstance().DelAnnounceFromList(dwAnnounceSerial);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateAnnounceList();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL gmtool::AddQueryRoleIDResult(INT32 area, UINT32 role, const char* name, const char* account)
{
    if (!name || !account) return FALSE;
    auto item = std::make_unique<CMyTreeWidgetItem>();
    item->setText(0, QString::number(area));
    item->setText(1, QString::number(role));
    item->setText(2, QString::fromUtf8(name));
    item->setText(3, QString::fromUtf8(account));
    ui.treeWidget_queryroleid->addTopLevelItem(item.get());
    item.release();
    return TRUE;
}

BOOL gmtool::AddQueryRoleDataResult(INT32 area, BOOL online, GM_ROLE_DATA* role)
{
    if (!role)
        return FALSE;
    const auto role_id = role->dwRoleID;
    const auto document = role->document;
    std::string name, save_time;
    INT32 group = -1, sex = 0, role_class = 0, level = 0;
    UINT32 deleted = 0;
    for (const auto& record : document)
    {
        if (record.type != GM_ROLE_BASE_DATA)
            continue;
        switch (record.base)
        {
        case GM_RBD_NAME: name = record.text(); break;
        case GM_RBD_GROUPID: group = record.value<INT32>(); break;
        case GM_RBD_DELETETIME: deleted = record.value<UINT32>(); break;
        case GM_RBD_CLASS: role_class = record.value<INT32>(); break;
        case GM_RBD_SEX: sex = record.value<INT32>(); break;
        case GM_RBD_LEVEL: level = record.value<INT32>(); break;
        case GM_RBD_LAST_LOGOUT_TIME: save_time = record.text(); break;
        default: break;
        }
    }
    auto item = std::make_unique<CMyTreeWidgetItem>();
    if (!DisplayBaseInfoWidgetItem(item.get(), area, group, role_id, online, save_time.c_str(), deleted,
                                  name.c_str(), sex, role_class, level))
        return FALSE;
    const QPointer<gmtool> alive(this);
    ui.treeWidget->addTopLevelItem(item.release());
    if (!alive) return FALSE;
    ui.treeWidget->resizeColumnToContents(2);
    if (!alive) return FALSE;
    ui.treeWidget->resizeColumnToContents(6);
    return TRUE;
}

BOOL gmtool::LoadRoleData(UINT32 session, UINT32 role_id)
{
    if (g_pRoleInfo) return FALSE;
    QPointer<CRoleInfo> dialog = new CRoleInfo(session, role_id, TRUE, this);
    g_pRoleInfo = dialog;
    struct Clear
    {
        QPointer<CRoleInfo> dialog;
        ~Clear() { if (dialog && g_pRoleInfo == dialog.data()) g_pRoleInfo = nullptr; delete dialog.data(); }
    } clear{dialog};
    dialog->exec();
    return TRUE;
}

BOOL gmtool::SetLDMode()
{
    INT32 nTabPageCount = ui.tabWidget->count();

    for (INT32 i = 0; i < nTabPageCount; ++i)
    {
        ui.tabWidget->removeTab(0);
    }

    if (!m_pLDWidget)
        m_pLDWidget = new CLDWidget(ui.tabWidget);
    ui.tabWidget->addTab(m_pLDWidget, QString());

    return TRUE;
}

BOOL gmtool::ServerLogin(INT32 server_id)
{
    const auto& servers = g_TLConfig.ServerList.arrGMList;
    const auto found = std::ranges::find_if(servers, [server_id](const auto& server) {
        return server.nSvrID == server_id;
    });
    if (found == std::end(servers)) return FALSE;
    QStringList groups{QStringLiteral("All Regions")};
    for (INT32 index = 1; index < MAX_SERVER_GROUP_COUNT; ++index)
    {
        const auto& name = found->groupList[index].szGroupName;
        const auto* end = static_cast<const char*>(std::memchr(name, 0, sizeof(name)));
        if (!end) return FALSE;
        groups.push_back(QStringLiteral("%1 - ").arg(index) + QString::fromUtf8(name, static_cast<int>(end - name)));
    }
    const QPointer<gmtool> alive(this);
    ui.comboBox->clear();
    if (!alive) return FALSE;
    for (const auto& group : groups)
    {
        ui.comboBox->addItem(group);
        if (!alive) return FALSE;
    }
    const QPointer<CLDWidget> exchange(m_pLDWidget);
    if (exchange) exchange->ServerLogin(server_id);
    return TRUE;
}

BOOL gmtool::LogoutServer(UINT32 dwSessionID)
{
    if (m_pLDWidget)
    {
        m_pLDWidget->UnInit();
    }

    return TRUE;
}

BOOL ParseIDString(const char* buffer, UINT32 length, std::list<UINT32>& ids)
{
    if (!buffer) return FALSE;
    auto result = gm::parse_role_ids({buffer, length});
    if (!result) return FALSE;
    ids.swap(*result);
    return TRUE;
}

BOOL gm::output_game_log(INT32 type,BOOL truncated,std::string_view text)
{
    if (text.size()>65534) return FALSE;
    const std::string owned(text);
    auto* window = gm::current_main_window();
    return window ? window->PutsGameLog(type,truncated,static_cast<INT32>(owned.size()+1),owned.c_str()) : FALSE;
}
