#pragma once
#include "../qt_cpp23.h"
#include "gm_manager.h"
#include "cmd_manager.h"
#include "session_mgr.h"
#include "server_mgr.h"
#include "gm_input.h"
#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <algorithm>
#include <expected>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace gm
{
class UiAction
{
    QPointer<QWidget> m_owner;
    bool* m_flag;
    bool m_previous;
public:
    UiAction(QWidget& owner, bool& flag) : m_owner(&owner), m_flag(&flag), m_previous(std::exchange(flag,true)) {}
    UiAction(const UiAction&)=delete;
    UiAction& operator=(const UiAction&)=delete;
    ~UiAction() { if (m_owner) *m_flag=m_previous; }
};

template<class T> class DialogScope
{
    QPointer<T> m_dialog;
public:
    explicit DialogScope(T* dialog) : m_dialog(dialog) {}
    DialogScope(const DialogScope&)=delete;
    DialogScope& operator=(const DialogScope&)=delete;
    ~DialogScope() { delete m_dialog.data(); }
    QPointer<T> Get() const { return m_dialog; }
};

inline bool message_dialog(QWidget& parent,const QString& text,QDialogButtonBox::StandardButtons choices,
    QDialogButtonBox::StandardButton default_choice,const char* object_name)
{
    DialogScope scope(new QDialog(&parent));
    const auto dialog=scope.Get();
    dialog->setObjectName(QString::fromLatin1(object_name));
    dialog->setWindowTitle(QStringLiteral("注意"));
    auto* layout=new QVBoxLayout(dialog);
    auto* label=new QLabel(text,dialog);
    label->setTextFormat(Qt::PlainText);label->setWordWrap(true);
    auto* buttons=new QDialogButtonBox(choices,dialog);
    layout->addWidget(label);layout->addWidget(buttons);
    QObject::connect(buttons,&QDialogButtonBox::accepted,dialog,&QDialog::accept);
    QObject::connect(buttons,&QDialogButtonBox::rejected,dialog,&QDialog::reject);
    if (auto* button=buttons->button(default_choice)) button->setDefault(true);
    dialog->setMinimumWidth(340);
    return dialog->exec()==QDialog::Accepted;
}

inline bool confirm_action(QWidget& parent,const QString& text,const char* object_name="gmActionConfirmation")
{
    return message_dialog(parent,text,QDialogButtonBox::Yes|QDialogButtonBox::No,QDialogButtonBox::No,object_name);
}

inline std::expected<std::string,QString> text_field(const QString& text, std::size_t capacity,
    const QString& label, std::size_t minimum=1)
{
    const auto bytes=text.toUtf8();
    if (static_cast<std::size_t>(bytes.size())<minimum || static_cast<std::size_t>(bytes.size())>=capacity || bytes.contains('\0'))
    {
        if (capacity==std::numeric_limits<std::size_t>::max())
            return std::unexpected(label+QStringLiteral("不能为空或含空字符"));
        return std::unexpected(label+QStringLiteral("需为 %1 至 %2 个 UTF-8 字节，且不能含空字符")
            .arg(static_cast<qulonglong>(minimum)).arg(static_cast<qulonglong>(capacity-1)));
    }
    return std::string(bytes.constData(),static_cast<std::size_t>(bytes.size()));
}
template<std::size_t N> bool copy_field(char (&field)[N],std::string_view text)
{
    if (text.size()>=N || text.find('\0')!=std::string_view::npos) return false;
    std::ranges::fill(field,'\0');std::ranges::copy(text,field);return true;
}
template<std::integral T> std::expected<T,QString> numeric_field(const QString& text,const QString& label,bool empty_zero=false)
{
    const auto bytes=text.toUtf8();
    const auto value=parse_decimal<T>({bytes.constData(),static_cast<std::size_t>(bytes.size())},empty_zero);
    if (!value) return std::unexpected(label+QStringLiteral("格式或范围无效"));
    return *value;
}

struct SelectedPeer
{
    UINT32 id;
    ISocketSession* socket;
    UINT64 generation;
    std::shared_ptr<const char> connection_lifetime;
    bool operator==(const SelectedPeer&) const=default;
};
inline std::expected<std::vector<SelectedPeer>,QString> selected_peers(bool single)
{
    auto& selection=CServerMgr::GetInstance();
    auto& sessions=CSessionMgr::GetInstance();
    if (!CGMManager::GetInstance().IsRunning() || !selection.UpdateSelectServer())
        return std::unexpected(QStringLiteral("服务器未连接，请稍后重试"));
    const auto count=selection.GetSelectedNum();
    if (count<=0 || count>MAX_GM_SESSION_COUNT || (single && count!=1))
        return std::unexpected(single ? QStringLiteral("请选择一个执行区服") : QStringLiteral("请选择执行区服"));
    std::vector<SelectedPeer> peers;peers.reserve(count);
    for (INT32 index=0;index<count;++index)
    {
        const auto id=selection.GetSessionIDByIndex(index);
        const auto* session=sessions.GetGMSession(id);
        if (!session || !session->pSocketSession || !session->connection_lifetime || session->nConnectState!=SESSION_CONNECTED || session->eState!=GM_SVR_STATE_LOGIN)
            return std::unexpected(QStringLiteral("服务器状态已变化，请重试"));
        peers.push_back({id,session->pSocketSession,sessions.Generation(),session->connection_lifetime});
    }
    return peers;
}
inline bool selection_matches(std::span<const SelectedPeer> peers,bool single)
{
    const auto current=selected_peers(single);
    return current && std::ranges::equal(*current,peers);
}
}
