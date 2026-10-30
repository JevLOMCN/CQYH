#include "stdafx.h"
#include "executing.h"
#include <QtCore/QTimer>

CExecuting::CExecuting(QWidget* parent,Qt::WindowFlags flags):QDialog(parent,flags)
{
    ui.setupUi(this);
    auto* timer=new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(500);
    connect(timer,&QTimer::timeout,this,&QDialog::accept);
    timer->start();
}

CExecuting::~CExecuting()=default;
