
#include "stdafx.h"
#include "gm_exchange_stats.h"
#include "gm_exchange_ui.h"
#include "gm_def.h"
#include "exchange_limit.h"
#include "message_handler.h"
#include "server_mgr.h"
#include <QtCore/QPointer>

CExchangeLimit::CExchangeLimit(INT32 nGroup, QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    ui.setupUi(this);

    connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(Modify()));
    connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(Exit()));


    m_nGroup = nGroup;
    m_connection=gm::connection_binding(CServerMgr::GetInstance().GetSessionIDByIndex(0));


}

CExchangeLimit::~CExchangeLimit()
{

}

BOOL CExchangeLimit::ShowExchangeLimit(INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit)
{
    ui.label_5->setText(QString::fromStdString(gm::exchange_price(nPriceHigh)));
    ui.label_6->setText(QString::fromStdString(gm::exchange_price(nPriceLow)));
    ui.label_7->setText(QString::number(nSingleOrderLimit));
    ui.label_8->setText(QString::number(nDailyOrderLimit));

    return TRUE;
}


void CExchangeLimit::Exit()
{
    //g_gmtool.GetLDWidget()->ClearExchangeLimit();

Exit0:
    done(0);
}

void CExchangeLimit::Modify()
{
    if (m_submitting || !gm::exchange_connection_current(m_connection)) return;
    gm::ExchangeAction action(*this,m_submitting);
    const QPointer<CExchangeLimit> alive(this);
    const auto generation=m_generation;
    ui.doubleSpinBox->interpretText();ui.doubleSpinBox_2->interpretText();
    ui.spinBox->interpretText();ui.spinBox_2->interpretText();
    const auto high=gm::exchange_price_units(ui.doubleSpinBox->value());
    const auto low=gm::exchange_price_units(ui.doubleSpinBox_2->value());
    if (!high || !low || m_nGroup<1 || m_nGroup>=MAX_SERVER_GROUP_COUNT)
    {
        gm::exchange_status(*this,QStringLiteral("Invalid price or region group"));return;
    }
    if (!alive || m_generation!=generation || !gm::exchange_connection_current(m_connection)) return;
    const auto sent=CMessageHandler::GetInstance().DoSetExchangeLimit(m_nGroup,*high,*low,
        ui.spinBox->value(),ui.spinBox_2->value());
    if (!alive || m_generation!=generation) return;
    if (sent) done(QDialog::Accepted);
    else gm::exchange_status(*this,QStringLiteral("Failed to send request. Please try again."));
}
void CExchangeLimit::done(int result)
{
    ++m_generation;
    QDialog::done(result);
}
