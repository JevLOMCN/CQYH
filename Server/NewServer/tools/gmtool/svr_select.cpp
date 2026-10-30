#include "stdafx.h"
#include "svr_select.h"

//#include <QInputDialog>
//#include <QMainWindow>
#include <QtWidgets/QMessageBox>
//#include "tl_config.h"
//#include "gm_manager.h"
//#include "cmd_manager.h"
//#include "svr_manager.h"
//
//
//CSvrSelect::CSvrSelect(QWidget *parent, Qt::WindowFlags flags)
//: QDialog(parent, flags)
//{
//    ui.setupUi(this);
//
//    //connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(Login()));
//    //connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
//
//
//    m_ppBox = new QCheckBox*[CSvrManager::GetInstance().GetMaxGMNum()];
//
//    INT32   x = 20, y = 20;
//
//    m_ppBox[0] = new QCheckBox(this);
//    m_ppBox[0]->setText(QString::fromLocal8Bit(cszAllSvr));
//    m_ppBox[0]->setGeometry(x, y, 75, 33);
//    m_ppBox[0]->setDisabled(FALSE);
//
//    x += 80;
//
//    for (INT32 nIndex = 1; nIndex < CSvrManager::GetInstance().GetMaxGMNum(); ++nIndex)
//    {
//        GM_DATA *pGMData = CSvrManager::GetInstance().GetGMData(nIndex);
//        if (pGMData)
//        {
//            m_ppBox[nIndex] = new QCheckBox(this);
//            m_ppBox[nIndex]->setText(QString::fromLocal8Bit(pGMData->szMSName));
//            m_ppBox[nIndex]->setGeometry(x, y, 75, 33);
//
//            m_ppBox[nIndex]->setDisabled(FALSE);
//
//            //switch(pSvrData->nState)        // todo_tq 12: state handling is also needed here. Allan Tao [17.12.2013]
//            //{
//            //case GM_SVR_STATE_DISCONNECT:
//            //    {
//            //        m_ppBox[nIndex]->setDisabled(TRUE);
//            //        //m_ppBox[nIndex].setBackgroundColor();
//            //    }
//            //    break;
//            //case GM_SVR_STATE_LOGIN_FAILED:
//            //    {
//            //        m_ppBox[nIndex]->setDisabled(TRUE);
//
//            //    }
//            //    break;
//            //case GM_SVR_STATE_LOGIN_SUCC:
//            //    {
//            //        m_ppBox[nIndex]->setDisabled(FALSE);
//            //    }
//            //    break;
//            //case GM_SVR_STATE_REGISTERED:
//            //    {
//            //        m_ppBox[nIndex]->setDisabled(TRUE);
//
//            //    }
//            //    break;
//            //case GM_SVR_STATE_CONNECTED:
//            //    {
//            //        m_ppBox[nIndex]->setDisabled(TRUE);
//
//            //    }
//            //    break;
//            //default:
//            //    break;
//            //}
//
//            x += 80;
//            if (x > ui.groupBox->width() - 3)
//            {
//                y += 40;
//                x = 20;
//            }
//        }
//    }
//
//
//    connect(ui.exeButton, SIGNAL(clicked()), this, SLOT(Execute()));
//    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
//    connect(m_ppBox[0], SIGNAL(clicked()), this, SLOT(SelectAll()));
//
//    show();
//
//}
//
//CSvrSelect::~CSvrSelect()
//{
//
//}
//
//
//void CSvrSelect::Exit()
//{
//    done(0);
//}
//
//void CSvrSelect::SelectAll()
//{
//    GM_DATA     *pGMData = NULL;
//    for (INT32 nIndex = 1; nIndex < CSvrManager::GetInstance().GetMaxGMNum(); ++nIndex)
//    {
//        pGMData = CSvrManager::GetInstance().GetGMData(nIndex);
//        if (pGMData)
//        {
//            //if (GM_SVR_STATE_LOGIN_SUCC == pGMData->nState)
//            {
//                m_ppBox[nIndex]->setChecked(TRUE);
//            }
//        }
//    }
//
//    return;
//Exit0:
//    return;
//}
//
//void CSvrSelect::Execute()
//{
//    // check
//    GM_DATA     *pGMData = NULL;
//    INT32       nRetCode = 0;
//
//    nRetCode = CCmdManager::GetInstance().CanExecuteNewCmd();
//    if (!nRetCode)
//    {
//        QMessageBox::critical(this, "critical", QString::fromLocal8Bit("A command is already running."), QMessageBox::Yes);
//        return ;
//    }
//
//    CCmdManager::GetInstance().ClearSvrList();
//    for (INT32 nIndex = 1; nIndex < CSvrManager::GetInstance().GetMaxGMNum(); ++nIndex)
//    {
//        pGMData = CSvrManager::GetInstance().GetGMData(nIndex);
//        if (pGMData)
//        {
//            if (m_ppBox[nIndex]->isChecked())
//            {
//                nRetCode = CCmdManager::GetInstance().AddSvr(pGMData->dwSessionID);
//                LOG_PROCESS_ERROR(nRetCode);
//            }
//        }
//    }
//
//    accept();
//
//    return;
//Exit0:
//    return;
//}
//
//
//void CSvrSelect::resizeEvent ( QResizeEvent * pEvent )
//{
//    INT32   x = 20, y = 20;
//
//    m_ppBox[0]->setGeometry(x, y, 75, 33);
//    x += 80;
//
//    for (INT32 nIndex = 1; nIndex < CSvrManager::GetInstance().GetMaxGMNum(); ++nIndex)
//    {
//        if (x + 76 > ui.groupBox->width())
//        {
//            y += 40;
//            x = 20;
//        }
//
//        m_ppBox[nIndex]->setGeometry(x, y, 75, 33);
//
//        x += 80;
//    }
//
//    show();
//
//}
