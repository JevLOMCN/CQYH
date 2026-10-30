#include "stdafx.h"
#include "gm_role_fixture.h"
#include "role_info.h"
#include "gm_item_info.h"
#include "data_mgr.h"
#include "cmd_manager.h"
#include "gm_manager.h"
#include "message_handler.h"
#include "server_mgr.h"
#include "session_mgr.h"
#include "my_tree_widget_item.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QTextEdit>
#include <QtCore/QTimer>
#include <QtGui/QFontDatabase>
#include <QtCore/QTextCodec>
#include <iostream>

namespace
{
std::size_t checks, releases, modifications;
std::size_t attempts;
bool send_success = true;
UINT32 modified_role, modified_target;
std::string modified_reason;
std::vector<char> modified_bytes;
void check(bool condition, const char* message)
{
    ++checks;
    if (!condition) throw std::runtime_error(message);
}
class RoleView : public CRoleInfo
{
public:
    RoleView(UINT32 role, bool editable) : CRoleInfo(1, role, editable) {}
    using CRoleInfo::_Update;
};
template <class Operation>
void answer(QDialogButtonBox::StandardButton button, Operation operation)
{
    bool clicked = false;
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&] {
        for (auto* widget : QApplication::topLevelWidgets())
        {
            if (auto* box = qobject_cast<QDialog*>(widget); box && box->isVisible() && box->objectName() == "roleEditConfirmation")
            {
                if (auto* choice = box->findChild<QDialogButtonBox*>()->button(button))
                {
                    clicked = true;timer.stop();choice->click();return;
                }
            }
        }
    });
    timer.start(1);
    operation();
    timer.stop();
    check(clicked, "actual confirmation-dialog action");
}
gm::RoleDocument document()
{
    auto parsed = gm::parse_role_data(role_fixture::sample());
    check(parsed.has_value(), "parse widget fixture");
    auto other = (*parsed)[13];
    const UINT64 guid = 7;const UINT16 slot = 1;
    std::memcpy(other.payload.data() + offsetof(GM_ROLE_ITEM_SLOT, qwGuid), &guid, sizeof(guid));
    std::memcpy(other.payload.data() + offsetof(GM_ROLE_ITEM_SLOT, wItemSlot), &slot, sizeof(slot));
    parsed->push_back(std::move(other));
    return std::move(*parsed);
}
QTreeWidget* tree(RoleView& view, const char* name)
{
    auto* result = view.findChild<QTreeWidget*>(name);
    check(result != nullptr, "role tree exists");
    return result;
}
QTreeWidgetItem* field(QTreeWidget* tree, INT32 base)
{
    for (int row = 0; row < tree->topLevelItemCount(); ++row)
    {
        auto* item = static_cast<CMyTreeWidgetItem*>(tree->topLevelItem(row));
        if (item->GetBaseDataType() == base) return item;
    }
    throw std::runtime_error("missing base-data field");
}
}

// Only the actual Qt role/item widgets and standard data/parser code run here.
// These native/command boundaries never contact a service or modify an account.
CCmdManager CCmdManager::m_Instance;
CCmdManager::CCmdManager() : m_nState(0), m_nResult(0), m_IsCmdComplete(FALSE), m_pCmd(nullptr) {}
CCmdManager::~CCmdManager() = default;
CCmdManager& CCmdManager::GetInstance() { return m_Instance; }
BOOL CCmdManager::IsCmdExecuting(BOOL) { return FALSE; }
BOOL CCmdManager::ModifyRole(UINT32 role, UINT32 target, INT32 length, const char* bytes, const char* reason)
{
    ++attempts;
    if (!send_success) return FALSE;
    ++modifications;modified_role=role;modified_target=target;modified_reason=reason;
    modified_bytes.assign(bytes, bytes + length);
    return TRUE;
}
CServerMgr CServerMgr::m_Instance;
CServerMgr& CServerMgr::GetInstance() { return m_Instance; }
BOOL CServerMgr::Init() { m_nSelectedSessionNum = 1;return TRUE; }
BOOL CServerMgr::UpdateSelectServer() { return TRUE; }
UINT32 CServerMgr::GetSessionIDByIndex(INT32 index) { return index==0 ? 1 : UINT32_MAX; }
namespace
{
struct FixtureSocket final:ISocketSession
{
    INT32 Send(PACKET_BUFFER*,INT32,INT32) override {return 0;}
    INT32 SendNoPackageHead(PACKET_BUFFER*,INT32) override {return 0;}
    void* GetUserData() override {return nullptr;}
    void SetUserData(void*) override {}
    BOOL GetAddress(UINT32&,UINT16&) override {return FALSE;}
    BOOL GetRemoteAddress(UINT32&,UINT16&) override {return FALSE;}
    BOOL Close() override {return TRUE;}
} fixture_socket;
}
CSessionMgr CSessionMgr::m_Instance;
CSessionMgr::CSessionMgr()
{
    auto& session=m_GMSession[1];session.dwSessionID=1;session.nSvrID=1;
    session.pSocketSession=&fixture_socket;session.nConnectState=SESSION_CONNECTED;
    session.eState=GM_SVR_STATE_LOGIN;session.connection_lifetime=std::make_shared<const char>(0);
}
CSessionMgr::~CSessionMgr()=default;
CSessionMgr& CSessionMgr::GetInstance() { return m_Instance; }
GM_SESSION* CSessionMgr::GetGMSession(UINT32 id) { return id==1 ? &m_GMSession[1] : nullptr; }
CGMManager CGMManager::m_Instance;
CGMManager::CGMManager() = default;
CGMManager::~CGMManager() = default;
CGMManager& CGMManager::GetInstance() { return m_Instance; }
BOOL CGMManager::MainLoop() { return TRUE; }
BOOL CGMManager::UnInit() { return TRUE; }
CMessageHandler CMessageHandler::ms_Instance;
CMessageHandler::CMessageHandler() : m_pNet(nullptr), m_pPacketAllocator(nullptr) {}
CMessageHandler::~CMessageHandler() = default;
CMessageHandler& CMessageHandler::GetInstance() { return ms_Instance; }
void CMessageHandler::OnAccepted(ISocketAcceptor*, ISocketSession*) {}
void CMessageHandler::OnConnected(ISocketSession*) {}
void CMessageHandler::OnRecv(ISocketSession*, PACKET_BUFFER*, UINT32) {}
void CMessageHandler::OnDisconnected(ISocketSession*, BOOL) {}
BOOL CMessageHandler::DoReleaseRole(UINT32, UINT32) { ++releases;return TRUE; }

int main(int argc, char** argv)
try
{
    QApplication app(argc, argv);
    check(QFontDatabase::addApplicationFont("C:/Windows/Fonts/arial.ttf") >= 0, "offscreen fixture font");
    check(QFontDatabase::addApplicationFont("C:/Windows/Fonts/msyh.ttc") >= 0, "offscreen Chinese fixture font");
    check(QTextCodec::codecForName("GB18030") != nullptr, "legacy source-text codec available");
    app.setFont(QFont("Microsoft YaHei", 9));
    check(argc == 2, "screenshot output argument");
    app.setQuitOnLastWindowClosed(false);
    CServerMgr::GetInstance().Init();
    auto& data = CDataMgr::GetInstance();data.Init();
    data.StoreRole(93, document());
    {
        RoleView view(93, true);
        auto* base = tree(view, "baseDataTreeWidget");
        auto* items = tree(view, "itemWidget");
        auto* quests = tree(view, "questWidget");
        auto* mail = tree(view, "mailTreeWidget");
        check(field(base, GM_RBD_NAME)->text(1) == QString(ROLE_NAME_LEN - 1, 'n'), "bounded nonterminated role-name display");
        check(field(base, GM_RBD_ID)->text(0) == QString(QChar(0x89d2)) + QChar(0x8272) + "ID", "legacy labels decode independently of desktop code page");
        check(field(base, GM_RBD_ID)->text(1) == "4294967295" && field(base, GM_RBD_EXP)->text(1) == "4294967295", "full-width base display");
        check(field(base, GM_RBD_MONEY_2)->text(1) == "-2147483648", "signed money display");
        check(items->topLevelItemCount() == 2 && items->topLevelItem(0)->text(1) == "18446744073709551615", "full-width item display");
        check(quests->columnWidth(0) > quests->fontMetrics().horizontalAdvance("4294967295") + quests->indentation(), "quest ID column fits full value");
        check(items->columnWidth(1) > items->fontMetrics().horizontalAdvance("18446744073709551615"), "item GUID column fits full value");
        check(quests->topLevelItemCount() == 1 && mail->topLevelItemCount() == 1 && tree(view, "spellTreeWidget")->topLevelItemCount() == 1 && tree(view, "titleTreeWidget")->topLevelItemCount() == 1, "all role sections");
        view._Update();
        check(tree(view, "spellTreeWidget")->topLevelItemCount() == 1 && tree(view, "titleTreeWidget")->topLevelItemCount() == 1, "refresh clears all previous rows");
        view.ShowItemInfo(items->topLevelItem(0), 0);
        auto* preview = view.findChild<CGMItemInfo*>();
        check(preview && preview->findChild<QTextEdit*>()->toPlainText().contains("18446744073709551615"), "owned item-detail preview");
        preview->close();QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        field(base, GM_RBD_LEVEL)->setText(1, "100");
        field(base, GM_RBD_LEVEL)->setText(1, "not a number");
        check(!view.findChild<QLabel*>("roleEditStatus")->text().isEmpty(), "inline validation feedback");
        check(field(base, GM_RBD_LEVEL)->text(1) == "100", "invalid input restores last accepted value");
        view.show();view.resize(1042, 832);QApplication::processEvents();
        check(view.grab().save(QString::fromLocal8Bit(argv[1])), "role validation screenshot");
        field(base, GM_RBD_EXP)->setText(1, "0");
        field(base, GM_RBD_EXP)->setText(1, "4294967295");
        answer(QDialogButtonBox::No, [&] { view.ReturnPos(); });
        answer(QDialogButtonBox::Yes, [&] { view.ReturnPos(); });
        auto* target = view.findChild<QLineEdit*>("lineEdit");
        target->setText("88");view.SetTargetRole();
        items->setCurrentItem(items->topLevelItem(0));view.TakeItem();
        items->setCurrentItem(items->topLevelItem(1));view.DelItem();
        quests->setCurrentItem(quests->topLevelItem(0));view.DelQuest();
        mail->setCurrentItem(mail->topLevelItem(0));view.DelMail();
        view.DelMail(); // A disabled selection must not append twice.
        auto* reason = view.findChild<QTextEdit*>("modifyRoleReasonEdit");
        check(reason != nullptr, "reason widget");
        view.EndModify();check(attempts == 0 && data.FindRole(93), "missing reason retains pending edits");
        reason->setPlainText(QString(GM_REASON_LEN, 'r'));
        view.EndModify();check(attempts == 0 && data.FindRole(93), "overlong reason retains pending edits");
        reason->setPlainText(QString("reason") + QChar(0));
        view.EndModify();check(attempts == 0 && data.FindRole(93), "NUL reason rejected");
        reason->setPlainText("reason");
        answer(QDialogButtonBox::No, [&] { view.EndModify(); });
        check(modifications == 0 && data.FindRole(93), "declined submission retains edits and role");
        send_success = false;
        answer(QDialogButtonBox::Yes, [&] { view.EndModify(); });
        check(attempts == 1 && modifications == 0 && data.FindRole(93), "failed command preserves editor and role");
        send_success = true;
        answer(QDialogButtonBox::Yes, [&] { view.EndModify(); });
        view.EndModify();check(attempts == 2, "completed editor cannot resubmit");
        check(modifications == 1 && modified_role == 93 && modified_target == 88 && modified_reason == "reason", "captured command boundary");
        std::vector<char> expected;
        role_fixture::append(expected, role_fixture::scalar(GM_RBD_LEVEL, INT32{100}));
        role_fixture::append(expected, role_fixture::scalar(GM_RBD_EXP, UINT32{0}));
        role_fixture::append(expected, role_fixture::scalar(GM_RBD_EXP, UINT32{4294967295}));
        role_fixture::append(expected, role_fixture::record(GM_ROLE_BASE_DATA, GM_RBD_MAP_ID, {}));
        gm::RoleEdits remaining;
        check(remaining.take_item(4294967295, (std::numeric_limits<UINT64>::max)()).has_value(), "expected transfer bytes");
        check(remaining.remove(GM_ROLE_ITEM, 0xffff0001).has_value(), "expected item removal");
        check(remaining.remove(GM_ROLE_QUEST, 4294967295).has_value(), "expected quest removal");
        check(remaining.remove(GM_ROLE_MAIL, 41).has_value(), "expected mail removal");
        expected.insert(expected.end(), remaining.bytes().begin(), remaining.bytes().end());
        check(modified_bytes == expected && modified_bytes.size() == 84, "actual UI produces ordered legacy-compatible edit bytes");
        check(!data.FindRole(93), "completed role closes owned model");
    }
    data.StoreRole(93, document());
    { RoleView view(93, true);view.EndModify();check(modifications == 1 && releases == 1, "population emits no edits"); }
    data.StoreRole(93, document());
    {
        RoleView view(93, false);
        auto* items = tree(view, "itemWidget");items->setCurrentItem(items->topLevelItem(0));view.DelItem();
        field(tree(view, "baseDataTreeWidget"), GM_RBD_LEVEL)->setText(1, "5");
        view.EndModify();check(modifications == 1 && releases == 1, "read-only actions send no edits");
    }
    data.StoreRole(93, document());
    { RoleView view(93, true);view.show();view.close();view.Exit();check(releases == 2 && !data.FindRole(93), "close/cancel releases once"); }
    data.UnInit();
    std::cout << checks << " GM role-widget checks passed\n";
    return 0;
}
catch (const std::exception& error)
{
    std::cerr << error.what() << '\n';
    return 1;
}
