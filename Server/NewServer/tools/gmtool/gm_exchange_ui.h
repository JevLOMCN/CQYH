#pragma once
#include "../qt_cpp23.h"
#include "session_mgr.h"
#include "server_mgr.h"
#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

namespace gm
{
inline bool exchange_connection_current(const std::optional<ConnectionBinding>& binding)
{
    auto& selection=CServerMgr::GetInstance();
    return binding && binding->Current() && selection.IsSingleSelected() &&
        selection.GetSessionIDByIndex(0)==binding->id;
}
class ExchangeAction
{
    QPointer<QWidget> m_owner;
    bool& m_active;
public:
    ExchangeAction(QWidget& owner,bool& active) : m_owner(&owner),m_active(active) { m_active=true; }
    ExchangeAction(const ExchangeAction&)=delete;
    ExchangeAction& operator=(const ExchangeAction&)=delete;
    ~ExchangeAction() { if (m_owner) m_active=false; }
};
inline void exchange_status(QWidget& owner,const QString& message)
{
    auto* label=owner.findChild<QLabel*>("exchangeActionStatus",Qt::FindDirectChildrenOnly);
    if (!label)
    {
        label=new QLabel(&owner);
        label->setObjectName("exchangeActionStatus");
        label->setWordWrap(true);
        if (auto* grid=qobject_cast<QGridLayout*>(owner.layout()))
            grid->addWidget(label,grid->rowCount(),0,1,grid->columnCount());
        else owner.layout()->addWidget(label);
    }
    label->setText(message);
    label->setVisible(!message.isEmpty());
}
inline bool confirm_exchange(QWidget& parent,const QString& message)
{
    // Heap ownership permits the parent to be destroyed during the nested event loop.
    QPointer<QDialog> dialog=new QDialog(&parent);
    dialog->setObjectName("exchangeConfirmation");
    dialog->setWindowTitle(QStringLiteral("确认"));
    auto* layout=new QVBoxLayout(dialog);
    auto* label=new QLabel(message,dialog);
    label->setWordWrap(true);
    auto* buttons=new QDialogButtonBox(QDialogButtonBox::Yes|QDialogButtonBox::No,dialog);
    layout->addWidget(label);layout->addWidget(buttons);
    QObject::connect(buttons->button(QDialogButtonBox::Yes),&QPushButton::clicked,dialog,&QDialog::accept);
    QObject::connect(buttons->button(QDialogButtonBox::No),&QPushButton::clicked,dialog,&QDialog::reject);
    buttons->button(QDialogButtonBox::No)->setDefault(true);
    dialog->setMinimumWidth(320);
    const auto result=dialog->exec();
    delete dialog.data();
    return result==QDialog::Accepted;
}
}
