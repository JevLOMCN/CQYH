#include "stdafx.h"
#include "gm_exchange_ui.h"
#include "detail.h"
#include "my_tree_widget_item.h"
#include "ld_widget.h"
#include "message_handler.h"
#include "server_mgr.h"
#include <memory>

CDetailDialog::CDetailDialog(INT32 nGroupID, QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    m_nGroupID = nGroupID;
    m_connection=gm::connection_binding(CServerMgr::GetInstance().GetSessionIDByIndex(0));
    ui.setupUi(this);

    connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(Exit()));
    connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(CancelSystemOrder()));
}

CDetailDialog::~CDetailDialog()
{

}

BOOL CDetailDialog::ShowData(const QString& header, std::span<const EXCHANGE_COUNT_DATA> rows)
{
    if (rows.size() > 255) return FALSE;
    for (const auto& row : rows)
        if (!std::memchr(row.szRoleName,0,sizeof(row.szRoleName))) return FALSE;
    ui.label->setText(header);
    ui.treeWidget->clear();
    for (const auto& row : rows)
    {
        auto item=std::make_unique<CMyTreeWidgetItem>();
        item->Set64ID(row.qwOrderID);
        item->setText(0,QString::number(row.qwOrderID));
        item->setText(1,QString::fromUtf8(row.szRoleName));
        item->setText(2,QString::number(row.nCount));
        if (!row.dwRoleID) item->setTextColor(1,QColor(255,0,0));
        ui.treeWidget->addTopLevelItem(item.get()); item.release();
    }
    for (INT32 column=0;column<3;++column) ui.treeWidget->resizeColumnToContents(column);
    return TRUE;
}

bool CDetailDialog::ContainsOrder(UINT64 order) const
{
    for (int index=0;index<ui.treeWidget->topLevelItemCount();++index)
    {
        bool valid=false;
        const auto value=ui.treeWidget->topLevelItem(index)->text(0).toULongLong(&valid);
        if (valid && value==order) return true;
    }
    return false;
}
void CDetailDialog::CancelSystemOrder()
{
    if (m_submitting || !gm::exchange_connection_current(m_connection)) return;
    auto* item=ui.treeWidget->currentItem();
    if (!item) { gm::exchange_status(*this,QStringLiteral("Please select an order."));return; }
    bool valid=false;
    const auto order=item->text(0).toULongLong(&valid);
    if (!valid) { gm::exchange_status(*this,QStringLiteral("Invalid order ID"));return; }
    gm::ExchangeAction action(*this,m_submitting);
    const QPointer<CDetailDialog> alive(this);
    const auto generation=m_generation;
    const auto confirmed=gm::confirm_exchange(*this,QStringLiteral("Confirm cancellation of order?"));
    if (!alive || !confirmed || m_generation!=generation || !gm::exchange_connection_current(m_connection)) return;
    // The item may have been replaced while the confirmation was open. Send the captured order only.
    const auto sent=CMessageHandler::GetInstance().DoDelSystemOrder(m_nGroupID,order);
    if (alive) gm::exchange_status(*this,sent ? QString() : QStringLiteral("Failed to send request. Please try again."));
}

void CDetailDialog::Exit()
{
    done(0);
}
void CDetailDialog::done(int result)
{
    ++m_generation;
    QDialog::done(result);
}
