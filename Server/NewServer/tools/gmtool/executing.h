/********************************************************************
	created:	21:12:2013   22:22
	file:		executing.h
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#ifndef _H_EXECUTING_H_
#define _H_EXECUTING_H_




#include <QtWidgets/QMainWindow>
#include "ui_executing.h"

class CExecuting : public QDialog
{
    Q_OBJECT

public:
    CExecuting(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CExecuting();

private:
    Ui::ExecutingDialog ui;
};







#endif //_H_EXECUTING_H_








