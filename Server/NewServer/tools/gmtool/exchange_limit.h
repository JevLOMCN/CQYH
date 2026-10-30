
#ifndef _H_EXCHANGE_LIMIT_H_
#define _H_EXCHANGE_LIMIT_H_


#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include "session_mgr.h"
#include "../qt_cpp23.h"
#include <QtWidgets/QMainWindow>
#include "ui_exchange_limit.h"

class CExchangeLimit : public QDialog
{
    Q_OBJECT

public:
    CExchangeLimit(INT32 nGroup, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CExchangeLimit();


    BOOL ShowExchangeLimit(INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit);


public slots:
    void Modify();
    void Exit();
    void done(int result) override;


private:
    INT32   m_nGroup;
    std::optional<gm::ConnectionBinding> m_connection;
    bool m_submitting=false;
    UINT64 m_generation=0;

    Ui::exchangeLimitDialog ui;
};




#endif //_H_EXCHANGE_LIMIT_H_




























