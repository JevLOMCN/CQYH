#include "stdafx.h"
#include "move_position.h"

//#include <QInputDialog>
//#include <QMainWindow>
#include <QtWidgets/QMessageBox>



CMovePosition::CMovePosition(QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    ui.setupUi(this);

    ui.comboBox->addItem("");
    ui.comboBox->addItem(QString::fromLocal8Bit("xsc - Beginner Village"));
    ui.comboBox->addItem(QString::fromLocal8Bit("hzc - Tiger Claw Manor"));
    ui.comboBox->addItem(QString::fromLocal8Bit("hhc - Black Tiger Village"));
    ui.comboBox->addItem(QString::fromLocal8Bit("lgz - Dragonbone Town"));
    ui.comboBox->addItem(QString::fromLocal8Bit("lgsm - Dragonbone Desert"));
    ui.comboBox->addItem(QString::fromLocal8Bit("hmyj - Desert Ruins"));


    //connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(Login()));
    //connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
    connect(ui.exeButton, SIGNAL(clicked()), this, SLOT(Execute()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
}

CMovePosition::~CMovePosition()
{

}

void CMovePosition::Execute()
{
    accept();
}

void CMovePosition::Exit()
{
    done(0);
}
