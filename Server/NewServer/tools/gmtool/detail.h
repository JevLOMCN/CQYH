/********************************************************************
	created:	5:12:2013   10:22
	file:		result.h
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#ifndef _H_DETAIL_H_
#define _H_DETAIL_H_


#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include "session_mgr.h"
#include "../qt_cpp23.h"
#include <QtWidgets/QMainWindow>
#include <span>
#include "ui_detail.h"

struct EXCHANGE_COUNT_DATA;

class CDetailDialog : public QDialog
{
    Q_OBJECT

public:
    CDetailDialog(INT32 nGroupID, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CDetailDialog();

    BOOL    ShowData(const QString& header, std::span<const EXCHANGE_COUNT_DATA> rows);
    bool ContainsOrder(UINT64 order) const;

public slots:
        void CancelSystemOrder();
        void Exit();
        void done(int result) override;


private:
    std::optional<gm::ConnectionBinding> m_connection;
    bool m_submitting=false;
    UINT64 m_generation=0;
    INT32   m_nGroupID;

    Ui::detailDialog ui;
};




#endif //_H_DETAIL_H_

