/********************************************************************
created:	7:29:2014   10:22
file:		role_gm_cmd.h
author:		Allan Tao

purpose:
*********************************************************************/

#ifndef _H_ROLE_GM_CMD_H_
#define _H_ROLE_GM_CMD_H_


#include "../qt_cpp23.h"
#include <QtWidgets/QDialog>
#include <string>
#include "ui_role_gm_cmd.h"
#include "string_def.h"
class QLabel;

class CRoleGMCmd : public QDialog
{
    Q_OBJECT

public:
    CRoleGMCmd(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CRoleGMCmd();

    const char* GetBuffer() const { return m_Buffer.c_str(); }
    const char* GetReason() const { return m_Reason.c_str(); }
    BOOL IsExeCmd()     { return m_bExe; }


public slots:
void OK();
void Exit();


private:

    std::string m_Buffer, m_Reason;
    bool m_bExe=false, m_confirming=false;
    QLabel* m_status=nullptr;

    Ui::roleGMDialog ui;
};




#endif //_H_ROLE_GM_CMD_H_

