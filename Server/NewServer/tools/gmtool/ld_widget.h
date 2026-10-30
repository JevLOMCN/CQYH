/********************************************************************
	created:	21:9:2015   15:24
	file:		ld_widget.h
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#ifndef _H_LD_WIDGET_H_
#define _H_LD_WIDGET_H_

#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include "../qt_cpp23.h"
#include <QtWidgets/QWidget>
#include <QtCore/QPointer>
#include "gm_exchange_stats.h"
#include "ui_ld_widget.h"
#include "exchange_history_widget.h"
#include "exchange_limit.h"


struct PRICE_DATA 
{
    INT32       nPrice;
    INT32       nCount;
};

struct EXCHANGE_COUNT_DATA
{
    UINT32      dwRoleID;
    char        szRoleName[ROLE_NAME_LEN];
    UINT64      qwOrderID;
    INT32       nCount;
};

class CDetailDialog;

class CLDWidget : public QWidget
{
    Q_OBJECT

public:
    CLDWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CLDWidget();

    BOOL    Init();
    BOOL    UnInit();

    BOOL    ServerLogin(INT32 nSvrID);


    BOOL    MainLoop();
    INT32 GetGroupID() const { return m_nGroupID; }
    BOOL OrderDeleted(UINT64 order);
    BOOL MainLoop(std::chrono::steady_clock::time_point tick,std::chrono::system_clock::time_point wall);

    BOOL    SetGroupID(INT32 nGroupID);

    BOOL    UpdatePrice(INT32 nGroupID, INT32 nSellNum, INT32 nBuyNum, PRICE_DATA *pData);

    BOOL    UpdateExchangeLog(INT32 nGroupID, INT32 nCount, BOOL bClear, const char *pcData);

    BOOL    UpdateExchangeHistory(INT32 nGroupID, INT32 nCount, BOOL bClear, const char *pcData);

    BOOL    UpdateOrderDetail(INT32 nGroupID, INT32 nOrderType, INT32 nPrice, INT32 nCount, EXCHANGE_COUNT_DATA *pData);

    BOOL    ShowTodayStat(INT32 nGroupID, 
        INT64 llSysTotalMoney, INT64 llTotalMoney, 
        INT64 llSysTotalExchangeMoney, INT64 llTotalExchangeMoney,
        INT64 llSysExchangeRMB, INT64 llExchangeRMB,
        INT32 nAveragePriceToday, INT32 nAveragePriceLastHour);

    BOOL    ShowYesterdayStat(INT32 nGroupID, 
        INT64 llSysTotalMoney, INT64 llTotalMoney,
        INT64 llSysTotalExchangeMoney, INT64 llTotalExchangeMoney,
        INT64 llSysExchangeRMB, INT64 llExchangeRMB,
        INT32 nAveragePriceToday, INT32 nAveragePriceLastHour);

    BOOL    OutputStatToFile(UINT32 dwBeginTime, UINT32 dwEndTime, INT32 nGroupID,
        INT64 llSysTotalMoney, INT64 llTotalMoney,
        INT64 llSysTotalExchangeMoney, INT64 llTotalExchangeMoney,
        INT64 llSysExchangeRMB, INT64 llExchangeRMB,
        INT32 nAveragePriceToday, INT32 nAveragePriceLastHour);


    BOOL DelDetail();

    BOOL ShowExchangeLimit(INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit);
    void ClearExchangeLimit();

    //void paintEvent(QPaintEvent *event);

public slots:
    void ChangeGroup();
    void MoreTradeInfo();
    void ExportTradeInfo();
    void AddOrder();

    void OrderSelected(QTreeWidgetItem * item, int column);
    void OrderSelected_2(QTreeWidgetItem * item, int column);
    void OrderDetail(QTreeWidgetItem * item, int column);
    void OrderDetail_2(QTreeWidgetItem * item, int column);

    void SetExchangeLimit();

private:
    Ui::ld_widget ui;

    QPointer<CDetailDialog> m_pDetailDialog;
    QPointer<CExchangeLimit> m_pExchangeLimit;

    INT32       m_nGroupID;
    gm::ExchangePoll m_poll;
    bool m_polling=false;
    UINT64 m_view_generation=0;

    gm::ExchangeStatistics m_statistics;
    bool m_submitting=false;
    bool CheckExchangeSelection();
    void QueryOrderDetail(QTreeWidgetItem* item,QTreeWidget* tree,UINT8 type);
};



#endif //_H_LD_WIDGET_H_






