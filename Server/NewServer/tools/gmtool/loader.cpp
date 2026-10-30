#include "stdafx.h"
#include "loader.h"
#include "gm_manager.h"
#include "session_mgr.h"
#include "message_handler.h"
#include "gmtool.h"
#include "gm_version.h"
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QTextCodec>

loader::loader(QWidget* parent, Qt::WindowFlags flags) : QDialog(parent, flags)
{
    ui.setupUi(this);
    connect(ui.loginButton, &QPushButton::clicked, this, &loader::Login);
    connect(ui.exitButton, &QPushButton::clicked, this, &loader::Exit);
    ui.label_3->setTextFormat(Qt::PlainText);
    ui.label_3->setGeometry(10, 4, 280, 32);
    ui.label_3->setWordWrap(true);
    ui.label_3->hide();
    ui.userNameEdit->setFocus();
    setWindowTitle(QStringLiteral("GM工具:     版本号: %1---%2").arg(GM_MAJOR_VERSION).arg(GM_MINOR_VERSION));
    m_timeout = new QTimer(this);
    m_timeout->setObjectName(QStringLiteral("loginTimeout"));
    m_timeout->setSingleShot(true);
    m_timeout->setInterval(30000);
    connect(m_timeout, &QTimer::timeout, this, [this] {
        CancelPending(QStringLiteral("登录超时，请等待重新连接后重试"), true);
    });
    m_watch = new QTimer(this);
    m_watch->setObjectName(QStringLiteral("loginConnectionWatch"));
    m_watch->setInterval(125);
    connect(m_watch, &QTimer::timeout, this, [this] {
        if (m_pending && !Current(*m_pending))
            CancelPending(QStringLiteral("服务器已断开，请重新登录"), false);
    });
}

loader::~loader() { gm::detach_login_dialog(this); }

bool loader::Current(const Attempt& attempt) const
{
    auto& sessions = CSessionMgr::GetInstance();
    const auto* session = sessions.GetGMSession(attempt.session_id);
    return CGMManager::GetInstance().IsRunning() && sessions.Generation() == attempt.generation &&
        session && session->pSocketSession == attempt.socket && session->nConnectState == SESSION_CONNECTED &&
        session->eState == GM_SVR_STATE_REGISTERED && attempt.connection_lifetime &&
        session->connection_lifetime == attempt.connection_lifetime;
}

void loader::Status(const QString& text)
{
    const QPointer<QLabel> label(ui.label_3);
    label->setText(text);
    if (label) label->show();
}

void loader::ClearPending()
{
    const QPointer<loader> alive(this);
    m_pending.reset();
    m_timeout->stop();
    m_watch->stop();
    ui.loginButton->setEnabled(!m_verified);
    if (!alive) return;
    ui.userNameEdit->setEnabled(!m_verified);
    if (!alive) return;
    ui.userPwdEdit->setEnabled(!m_verified);
}

void loader::CancelPending(const QString& text, bool close_peer)
{
    if (!m_pending) return;
    const auto attempt = *m_pending;
    const QPointer<loader> alive(this);
    ClearPending();
    if (!alive) return;
    Status(text);
    if (!alive) return;
    // An expired request has no wire request ID. Retire its connection before a retry.
    if (close_peer && Current(attempt)) CSessionMgr::GetInstance().CloseSession(attempt.socket);
}

void loader::Login()
{
    if (gm::current_login_dialog() != this || m_pending || m_verified || m_submitting) return;
    const QPointer<loader> alive(this);
    m_submitting = true;
    struct Reset
    {
        QPointer<loader> owner;
        bool* active;
        ~Reset() { if (owner) *active = false; }
    } reset{this, &m_submitting};
    const auto name = ui.userNameEdit->text().toUtf8();
    const auto password = ui.userPwdEdit->text().toUtf8();
    if (name.isEmpty() || password.isEmpty() || name.size() >= USER_NAME_LEN ||
        password.size() >= GM_TOOL_PASSWORD_LEN || name.contains('\0') || password.contains('\0'))
    {
        Status(QStringLiteral("请输入有效账号和密码（UTF-8 各不超过32字节）"));
        return;
    }
    auto& sessions = CSessionMgr::GetInstance();
    std::optional<Attempt> attempt;
    if (CGMManager::GetInstance().IsRunning())
        for (UINT32 id = 0; id < MAX_GM_SESSION_COUNT; ++id)
        {
            const auto* session = sessions.GetGMSession(id);
            if (session && session->dwType == stGM && session->pSocketSession &&
                session->nConnectState == SESSION_CONNECTED && session->eState == GM_SVR_STATE_REGISTERED)
            {
                attempt = Attempt{sessions.Generation(), ++m_serial, id, session->pSocketSession, session->connection_lifetime};
                break;
            }
        }
    if (!attempt) { Status(QStringLiteral("没有已注册的服务器，请稍后重试")); return; }
    CGMManager::GetInstance().SetUserName(name.constData());
    CGMManager::GetInstance().SetUserPwd(password.constData());
    m_pending = *attempt;
    ui.loginButton->setEnabled(false);
    if (!alive) return;
    ui.userNameEdit->setEnabled(false);
    if (!alive) return;
    ui.userPwdEdit->setEnabled(false);
    if (!alive) return;
    Status(QStringLiteral("正在登录…"));
    if (!alive) return;
    if (!m_pending || m_pending->serial!=attempt->serial || !Current(*attempt))
    {
        CancelPending(QStringLiteral("服务器已断开，请重新登录"), false);
        return;
    }
    m_timeout->start();
    m_watch->start();
    bool sent = false;
    try { sent = CMessageHandler::GetInstance().DoVerify(attempt->session_id, name.constData(), password.constData()) != FALSE; }
    catch (...) { sent = false; }
    if (alive && m_pending && m_pending->serial == attempt->serial && (!sent || !Current(*attempt)))
        CancelPending(QStringLiteral("登录发送失败，请重试"), false);
}

void loader::done(int result)
{
    const QPointer<loader> alive(this);
    CancelPending(QStringLiteral("登录已取消"), true);
    if (alive) QDialog::done(result);
}

void loader::Exit() { reject(); }

void loader::ServerError(ISocketSession* socket, std::string_view text)
{
    if (!m_pending || m_pending->socket != socket || !Current(*m_pending) || text.size() > 65535) return;
    const auto* codec = QTextCodec::codecForName("GB18030");
    const auto message = codec ? codec->toUnicode(text.data(), static_cast<int>(text.size())) :
        QString::fromLocal8Bit(text.data(), static_cast<int>(text.size()));
    CancelPending(message, true);
}

BOOL loader::VerifyUser(INT32 result, BOOL exchange_mode, ISocketSession* socket)
{
    if (!m_pending || m_pending->socket != socket || !Current(*m_pending)) return FALSE;
    const auto attempt = *m_pending;
    const QPointer<loader> alive(this);
    const QPointer<gmtool> window(gm::current_main_window());
    if (result != errSuccess || !window)
    {
        ClearPending();
        if (alive) Status(QStringLiteral("登录失败，错误码：%1").arg(result));
        return TRUE;
    }
    m_verified = true;
    ClearPending();
    const auto can_finish = [alive, window, attempt]
    {
        if (!alive) return false;
        if (!window || !alive->Current(attempt)) { alive->reject(); return false; }
        return true;
    };
    if (!can_finish()) return TRUE;
    window->SetTitle();
    if (!can_finish()) return TRUE;
    if (exchange_mode) window->SetLDMode();
    if (!can_finish()) return TRUE;
    window->show();
    if (can_finish()) accept();
    return TRUE;
}
