/********************************************************************
	created:	21:9:2015   15:33
	file:		ld_widget.cpp
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "gm_exchange_ui.h"
#include "ld_widget.h"
#include "my_tree_widget_item.h"
#include "message_handler.h"
#include "server_mgr.h"
#include "gm_def.h"
#include "gm_item_info.h"
#include "detail.h"
#include "time_ex.h"
#include "tl_config.h"
#include "gm_exchange_stats.h"
#include "gm_reports.h"
#include <QtCore/QTextCodec>
#include <format>
#include <algorithm>
#include <array>
#include <cstring>
#include <iterator>
#include <memory>
#include <span>
#include <vector>

namespace
{
QString legacy_text(std::string_view text)
{
    if (const auto* codec = QTextCodec::codecForName("GB18030"))
        return codec->toUnicode(text.data(), static_cast<int>(text.size()));
    return QString::fromLocal8Bit(text.data(), static_cast<int>(text.size()));
}
}

CLDWidget::CLDWidget(QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent, flags)
{
    m_nGroupID = 0;

    ui.setupUi(this);
    ui.label->setWordWrap(true);

    ui.treeWidget->setColumnWidth(0, 46);
    ui.treeWidget->setColumnWidth(1, 55);
    ui.treeWidget->setColumnWidth(2, 50);

    ui.treeWidget_2->setColumnWidth(0, 46);
    ui.treeWidget_2->setColumnWidth(1, 55);
    ui.treeWidget_2->setColumnWidth(2, 50);


    ui.treeWidget_3->setColumnWidth(0, 136);
    ui.treeWidget_3->setColumnWidth(1, 50);
    ui.treeWidget_3->setColumnWidth(2, 40);
    ui.treeWidget_3->setColumnWidth(3, 20);
    ui.treeWidget_3->setFrameShape(QTreeWidget::NoFrame);
    ui.gridLayout_5->setMargin(0);

    ui.treeWidget_4->setColumnWidth(0, 136);
    ui.treeWidget_4->setColumnWidth(1, 50);
    ui.treeWidget_4->setColumnWidth(2, 40);
    ui.treeWidget_4->setColumnWidth(3, 20);
    ui.treeWidget_4->setFrameShape(QTreeWidget::NoFrame);
    ui.gridLayout_6->setMargin(0);

    ui.treeWidget_5->setColumnWidth(0, 136);
    ui.treeWidget_5->setColumnWidth(1, 50);
    ui.treeWidget_5->setColumnWidth(2, 40);
    ui.treeWidget_5->setColumnWidth(3, 20);
    ui.treeWidget_5->setFrameShape(QTreeWidget::NoFrame);
    ui.gridLayout_7->setMargin(0);

    

    connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(ChangeGroup()));
    connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(MoreTradeInfo()));
    connect(ui.pushButton_4, SIGNAL(clicked()), this, SLOT(ExportTradeInfo()));
    connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(AddOrder()));

    connect(ui.treeWidget, &QTreeWidget::itemClicked, this, &CLDWidget::OrderSelected);
    connect(ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &CLDWidget::OrderDetail);
    connect(ui.treeWidget_2, &QTreeWidget::itemClicked, this, &CLDWidget::OrderSelected_2);
    connect(ui.treeWidget_2, &QTreeWidget::itemDoubleClicked, this, &CLDWidget::OrderDetail_2);

    connect(ui.pushButton_5, SIGNAL(clicked()), this, SLOT(SetExchangeLimit()));

    const auto now=QDateTime::currentDateTime();
    ui.dateTimeEdit->setDateTime(now);
    ui.dateTimeEdit_2->setDateTime(now);

    show();

    m_pExchangeLimit = NULL;
}

CLDWidget::~CLDWidget()
{

}

BOOL CLDWidget::Init()
{
    UnInit();
    return TRUE;
}

BOOL CLDWidget::UnInit()
{
    ++m_view_generation;m_nGroupID=0;m_poll={};
    const QPointer<CLDWidget> alive(this);
    DelDetail();if (!alive) return FALSE;
    ClearExchangeLimit();if (!alive) return FALSE;
    m_statistics={};
    ui.label_6->clear();ui.comboBox->clear();
    for (auto* tree : {ui.treeWidget,ui.treeWidget_2,ui.treeWidget_3,ui.treeWidget_4,ui.treeWidget_5}) tree->clear();
    ui.label->clear();ui.doubleSpinBox->setValue(0);ui.lineEdit->clear();ui.label_8->UnInit();
    if (auto* status=findChild<QLabel*>("exchangeActionStatus",Qt::FindDirectChildrenOnly)) { status->clear();status->hide(); }
    return TRUE;
}

BOOL CLDWidget::ServerLogin(INT32 server_id)
{
    const auto& servers=g_TLConfig.ServerList.arrGMList;
    const auto server=std::ranges::find_if(servers,[&](const auto& value){return value.nSvrID==server_id;});
    if (server_id<=0 || server==std::end(servers)) return FALSE;
    QStringList groups{QString()};
    for (int index=1;index<MAX_SERVER_GROUP_COUNT;++index)
    {
        const auto& name=server->groupList[index].szGroupName;
        const auto* end=static_cast<const char*>(std::memchr(name,0,sizeof(name)));
        if (!end) return FALSE;
        groups.push_back(QString::number(index)+QStringLiteral(" - ")+QString::fromUtf8(name,static_cast<int>(end-name)));
    }
    const QPointer<CLDWidget> alive(this);
    if (!UnInit() || !alive) return FALSE;
    ui.comboBox->addItems(groups);
    return TRUE;
}

BOOL CLDWidget::MainLoop()
{
    return MainLoop(std::chrono::steady_clock::now(),std::chrono::system_clock::now());
}
BOOL CLDWidget::MainLoop(std::chrono::steady_clock::time_point tick,std::chrono::system_clock::time_point wall)
{
    if (m_polling || m_nGroupID<1 || !CServerMgr::GetInstance().IsSingleSelected()) return TRUE;
    gm::ExchangeAction action(*this,m_polling);
    const QPointer<CLDWidget> alive(this);
    const auto group=m_nGroupID;
    const auto generation=m_view_generation;
    const auto session=CServerMgr::GetInstance().GetSessionIDByIndex(0);
    const auto binding=gm::connection_binding(session);
    if (!binding) return FALSE;
    const auto current=[&]
    {
        return alive && binding->Current() && m_view_generation==generation && m_nGroupID==group &&
            CServerMgr::GetInstance().IsSingleSelected() && CServerMgr::GetInstance().GetSessionIDByIndex(0)==session;
    };
    const auto due=m_poll.take(tick);
    bool success=true;
    if (due.log) success=CMessageHandler::GetInstance().DoQueryExchangeLog(group)!=FALSE;
    if (!current()) return FALSE;
    if (due.history) success=(CMessageHandler::GetInstance().DoQueryExchangeHistory(group)!=FALSE) && success;
    if (!current()) return FALSE;
    if (due.statistics)
    {
        const auto seconds=std::chrono::duration_cast<std::chrono::seconds>(wall.time_since_epoch()).count();
        const auto day=gm::exchange_day(seconds),hour=gm::exchange_hour(seconds);
        if (!day || !hour) return FALSE;
        success=(CMessageHandler::GetInstance().DoQuerySysExchangeInfo(group,1,day->begin,day->end,hour->begin,hour->end)!=FALSE) && success;
    }
    return current() && success;
}

BOOL CLDWidget::SetGroupID(INT32 group)
{
    if (group<0 || group>=MAX_SERVER_GROUP_COUNT) return FALSE;
    if (m_nGroupID!=group)
    {
        ++m_view_generation;m_nGroupID=group;m_poll={};
        const QPointer<CLDWidget> alive(this);
        DelDetail();if (!alive) return FALSE;
        ClearExchangeLimit();if (!alive) return FALSE;
        m_statistics={};ui.label->clear();ui.label_8->UnInit();
        for (auto* tree : {ui.treeWidget,ui.treeWidget_2,ui.treeWidget_3,ui.treeWidget_4,ui.treeWidget_5}) tree->clear();
        if (auto* status=findChild<QLabel*>("exchangeActionStatus",Qt::FindDirectChildrenOnly)) { status->clear();status->hide(); }
    }
    ui.comboBox->setCurrentIndex(group);ui.label_6->setText(ui.comboBox->currentText());
    auto palette=ui.label_6->palette();palette.setColor(QPalette::WindowText,Qt::red);ui.label_6->setPalette(palette);
    return TRUE;
}

BOOL CLDWidget::UpdatePrice(INT32 group, INT32 sell_count, INT32 buy_count, PRICE_DATA* data)
{
    if (group != m_nGroupID || sell_count < 0 || buy_count < 0 ||
        sell_count > 40 || buy_count > 40 - sell_count || ((sell_count || buy_count) && !data)) return FALSE;
    const auto count=static_cast<std::size_t>(sell_count+buy_count);
    std::vector<PRICE_DATA> rows(count);
    if (count) std::memcpy(rows.data(),data,count*sizeof(PRICE_DATA));
    using Items=std::vector<std::unique_ptr<CMyTreeWidgetItem>>;
    std::array<Items,2> prepared;
    const auto prepare=[&](std::size_t index, int side, int position)
    {
        auto item=std::make_unique<CMyTreeWidgetItem>();
        item->SetID(rows[index].nPrice);
        item->setData(0,Qt::UserRole,rows[index].nPrice);
        item->setText(0,legacy_text(side ? "买" : "卖")+QString::number(position));
        item->setText(1,QString::fromStdString(gm::exchange_price(rows[index].nPrice)));
        item->setText(2,QString::number(rows[index].nCount));
        prepared[side].push_back(std::move(item));
    };
    for (int index=sell_count; index>0; --index) prepare(index-1,0,index);
    for (int index=0; index<buy_count; ++index) prepare(sell_count+index,1,index+1);
    const std::array trees{ui.treeWidget,ui.treeWidget_2};
    for (std::size_t side=0; side<trees.size(); ++side)
    {
        trees[side]->clear();
        for (auto& item : prepared[side]) trees[side]->addTopLevelItem(item.release());
    }
    return TRUE;
}

BOOL CLDWidget::UpdateExchangeLog(INT32 group, INT32 count, BOOL clear, const char* data)
{
    if (group != m_nGroupID || count<0 || count>MAX_EXCHANGE_LOG_NUM ||
        (clear!=FALSE && clear!=TRUE) || (count && !data)) return FALSE;
    std::vector<GM_EXCHANGE_PRICE_LOG> rows(count);
    if (count) std::memcpy(rows.data(),data,rows.size()*sizeof(GM_EXCHANGE_PRICE_LOG));
    for (const auto& row : rows)
        if (!std::memchr(row.szExchangeTime,0,sizeof(row.szExchangeTime))) return FALSE;
    using Items=std::vector<std::unique_ptr<CMyTreeWidgetItem>>;
    std::array<Items,3> prepared;
    for (const auto& row : rows)
    {
        const std::array text{QString::fromUtf8(row.szExchangeTime),
            QString::fromStdString(gm::exchange_price(row.nPrice)),QString::number(row.nCount),
            row.dwSellerID ? QStringLiteral("P") : QStringLiteral("S")};
        const auto prepare=[&](int table)
        {
            auto item=std::make_unique<CMyTreeWidgetItem>();
            for (int column=0;column<static_cast<int>(text.size());++column) item->setText(column,text[column]);
            prepared[table].push_back(std::move(item));
        };
        prepare(0);prepare(row.dwSellerID ? 2 : 1);
    }
    const std::array trees{ui.treeWidget_3,ui.treeWidget_4,ui.treeWidget_5};
    for (std::size_t table=0;table<trees.size();++table)
    {
        if (clear) trees[table]->clear();
        for (auto& item : prepared[table]) trees[table]->addTopLevelItem(item.release());
    }
    return TRUE;
}

BOOL CLDWidget::UpdateExchangeHistory(INT32 group,INT32 count,BOOL clear,const char* bytes)
{
    if (group!=m_nGroupID || (clear!=FALSE && clear!=TRUE)) return FALSE;
    // The server sends a complete history snapshot for both flag values.
    return ui.label_8->SetData(count,bytes);
}

BOOL CLDWidget::UpdateOrderDetail(INT32 group, INT32 type, INT32 price, INT32 count, EXCHANGE_COUNT_DATA* data)
{
    if (group != m_nGroupID || count < 0 || count > 255 || (count && !data)) return FALSE;
    std::vector<EXCHANGE_COUNT_DATA> rows;
    if (count) rows.assign(data, data + count);
    for (const auto& row : rows)
        if (!std::memchr(row.szRoleName,0,sizeof(row.szRoleName))) return FALSE;
    const auto header = legacy_text(std::format("区组\t{}\n{}\n价格\t{}\n",group,type==2 ? "卖单" : "买单",gm::exchange_price(price)));
    auto dialog = std::make_unique<CDetailDialog>(group,this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    if (!dialog->ShowData(header,rows)) return FALSE;
    DelDetail();
    m_pDetailDialog=dialog.get();
    connect(dialog.get(),&QDialog::finished,this,[this,guard=QPointer<CDetailDialog>(dialog.get())]
    { if (m_pDetailDialog.data()==guard.data()) m_pDetailDialog.clear(); });
    dialog->show();
    dialog.release();
    return TRUE;
}

BOOL CLDWidget::ShowTodayStat(INT32 group, INT64 system_money, INT64 player_money,
    INT64 system_traded, INT64 player_traded, INT64 system_gems, INT64 player_gems,
    INT32 average, INT32 previous_average)
{
    if (group != m_nGroupID) return FALSE;
    const gm::ExchangeStatistics candidate{system_money,player_money,system_traded,player_traded,system_gems,player_gems,average,previous_average};
    const auto text = legacy_text(gm::exchange_summary(candidate));
    m_statistics=candidate;
    ui.label->setText(text);
    ui.label->setMinimumHeight(ui.label->heightForWidth(ui.label->width()));
    return TRUE;
}

BOOL CLDWidget::ShowYesterdayStat(INT32 group, INT64 system_money, INT64 player_money,
    INT64 system_traded, INT64 player_traded, INT64 system_gems, INT64 player_gems,
    INT32 average, INT32 previous_average)
{
    if (group != m_nGroupID) return FALSE;
    const gm::ExchangeStatistics yesterday{system_money,player_money,system_traded,player_traded,system_gems,player_gems,average,previous_average};
    const auto text = legacy_text(gm::exchange_comparison(m_statistics,yesterday));
    auto dialog = std::make_unique<CGMItemInfo>(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->ShowInfo(text,TRUE);
    dialog->show();
    dialog.release();
    return TRUE;
}

BOOL CLDWidget::OutputStatToFile(UINT32 begin, UINT32 end, INT32 group,
    INT64 system_money, INT64 player_money, INT64 system_traded, INT64 player_traded,
    INT64 system_gems, INT64 player_gems, INT32 average, INT32 previous_average)
{
    if (group != m_nGroupID) return FALSE;
    const gm::ExchangeStatistics values{system_money,player_money,system_traded,player_traded,system_gems,player_gems,average,previous_average};
    const auto report=gm::exchange_export(begin,end,values);
    if (!report) { CRI("GM statistics: %s",report.error().c_str()); return FALSE; }
    const auto written=gm::write_text_report(std::filesystem::path("..")/report->filename,report->bytes);
    if (!written) CRI("GM statistics: %s",written.error().c_str());
    return written.has_value();
}

BOOL CLDWidget::DelDetail()
{
    const auto dialog=m_pDetailDialog;
    m_pDetailDialog.clear();
    if (dialog) dialog->Exit();
    return TRUE;
}

BOOL CLDWidget::OrderDeleted(UINT64 order)
{
    if (m_pDetailDialog && m_pDetailDialog->ContainsOrder(order)) return DelDetail();
    return FALSE;
}
BOOL CLDWidget::ShowExchangeLimit(INT32 high, INT32 low, INT32 single, INT32 daily)
{
    if (!m_pExchangeLimit)
    {
        auto* dialog = new CExchangeLimit(m_nGroupID,this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        m_pExchangeLimit=dialog;
        connect(dialog,&QDialog::finished,this,[this,guard=QPointer<CExchangeLimit>(dialog)]
        { if (m_pExchangeLimit.data()==guard.data()) m_pExchangeLimit.clear(); });
    }
    m_pExchangeLimit->ShowExchangeLimit(high,low,single,daily);
    m_pExchangeLimit->show();
    return TRUE;
}

void CLDWidget::ClearExchangeLimit()
{
    const auto dialog=m_pExchangeLimit;
    m_pExchangeLimit.clear();
    if (dialog) dialog->Exit();
}

bool CLDWidget::CheckExchangeSelection()
{
    if (m_nGroupID>0 && m_nGroupID<MAX_SERVER_GROUP_COUNT && CServerMgr::GetInstance().IsSingleSelected()) return true;
    gm::exchange_status(*this,QStringLiteral("请选择有效区组和单一区服"));return false;
}
void CLDWidget::ChangeGroup()
{
    if (m_submitting) return;
    gm::ExchangeAction action(*this,m_submitting);
    const auto group=ui.comboBox->currentIndex();
    if (group<0 || group>=MAX_SERVER_GROUP_COUNT || !CServerMgr::GetInstance().IsSingleSelected())
    { gm::exchange_status(*this,QStringLiteral("请选择有效区组和单一区服"));return; }
    const QPointer<CLDWidget> alive(this);
    const auto sent=CMessageHandler::GetInstance().DoSelectGroup(group);
    if (alive) gm::exchange_status(*this,sent ? QString() : QStringLiteral("发送失败，请重试"));
    // SetGroupID clears the view only after the server confirms the switch.
}
void CLDWidget::MoreTradeInfo()
{
    if (m_submitting) return;
    gm::ExchangeAction action(*this,m_submitting);
    if (!CheckExchangeSelection()) return;
    const auto now=std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    const auto window=gm::exchange_day(now,get_time_zone(),true);
    if (!window) { gm::exchange_status(*this,QStringLiteral("日期超出协议范围"));return; }
    const QPointer<CLDWidget> alive(this);
    const auto sent=CMessageHandler::GetInstance().DoQuerySysExchangeInfo(m_nGroupID,0,window->begin,window->end,0,0);
    if (alive) gm::exchange_status(*this,sent ? QString() : QStringLiteral("发送失败，请重试"));
}
void CLDWidget::ExportTradeInfo()
{
    if (m_submitting) return;
    gm::ExchangeAction action(*this,m_submitting);
    if (!CheckExchangeSelection()) return;
    const auto begin=ui.dateTimeEdit->dateTime(),end=ui.dateTimeEdit_2->dateTime();
    const auto window=gm::exchange_window(begin.toSecsSinceEpoch(),end.toSecsSinceEpoch());
    if (!begin.isValid() || !end.isValid() || !window)
    { gm::exchange_status(*this,QStringLiteral("请选择有效且有序的日期范围"));return; }
    const QPointer<CLDWidget> alive(this);
    const auto sent=CMessageHandler::GetInstance().DoQuerySysExchangeInfo(m_nGroupID,3,window->begin,window->end,0,0);
    if (alive) gm::exchange_status(*this,sent ? QString() : QStringLiteral("发送失败，请重试"));
}

void CLDWidget::AddOrder()
{
    if (m_submitting) return;
    gm::ExchangeAction action(*this,m_submitting);
    const QPointer<CLDWidget> alive(this);
    const auto group=m_nGroupID;
    const auto session=CServerMgr::GetInstance().GetSessionIDByIndex(0);
    const auto binding=gm::connection_binding(session);
    if (!binding) return;
    ui.doubleSpinBox->interpretText();
    const auto price=gm::exchange_price_units(ui.doubleSpinBox->value());
    bool valid=false;
    const auto count=ui.lineEdit->text().toInt(&valid);
    if (!price || !valid || count<=0 || group<1 || group>=MAX_SERVER_GROUP_COUNT ||
        !CServerMgr::GetInstance().IsSingleSelected())
    {
        gm::exchange_status(*this,QStringLiteral("请输入有效价格和数量，并选择单一区服"));return;
    }
    const auto confirmed=gm::confirm_exchange(*this,QStringLiteral("是否确定单价加单？"));
    if (!alive || !confirmed) return;
    if (!binding->Current() || m_nGroupID!=group || !CServerMgr::GetInstance().IsSingleSelected() ||
        CServerMgr::GetInstance().GetSessionIDByIndex(0)!=session)
    {
        gm::exchange_status(*this,QStringLiteral("区服已更改，请重新确认"));return;
    }
    const auto sent=CMessageHandler::GetInstance().DoAddSystemOrder(group,*price,count);
    if (alive) gm::exchange_status(*this,sent ? QString() : QStringLiteral("发送失败，请重试"));
}

void CLDWidget::OrderSelected(QTreeWidgetItem* item,int)
{
    if (item && item->treeWidget()==ui.treeWidget) item->setSelected(true);
}
void CLDWidget::OrderSelected_2(QTreeWidgetItem* item,int)
{
    if (item && item->treeWidget()==ui.treeWidget_2) item->setSelected(true);
}
void CLDWidget::QueryOrderDetail(QTreeWidgetItem* item,QTreeWidget* tree,UINT8 type)
{
    if (m_submitting) return;
    gm::ExchangeAction action(*this,m_submitting);
    if (!CheckExchangeSelection() || !item || item->treeWidget()!=tree) return;
    bool valid=false;const auto price=item->data(0,Qt::UserRole).toInt(&valid);
    if (!valid) { gm::exchange_status(*this,QStringLiteral("订单价格无效"));return; }
    const QPointer<CLDWidget> alive(this);
    const auto sent=CMessageHandler::GetInstance().DoQueryOrderDetail(m_nGroupID,type,price);
    if (alive) gm::exchange_status(*this,sent ? QString() : QStringLiteral("发送失败，请重试"));
}
void CLDWidget::OrderDetail(QTreeWidgetItem* item,int) { QueryOrderDetail(item,ui.treeWidget,2); }
void CLDWidget::OrderDetail_2(QTreeWidgetItem* item,int) { QueryOrderDetail(item,ui.treeWidget_2,1); }
void CLDWidget::SetExchangeLimit()
{
    if (m_submitting) return;
    gm::ExchangeAction action(*this,m_submitting);
    if (!CheckExchangeSelection()) return;
    const QPointer<CLDWidget> alive(this);
    const auto sent=CMessageHandler::GetInstance().DoQueryExchangeLimit(m_nGroupID);
    if (alive) gm::exchange_status(*this,sent ? QString() : QStringLiteral("发送失败，请重试"));
}
