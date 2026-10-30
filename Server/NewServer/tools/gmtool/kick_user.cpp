#include "stdafx.h"
#include "kick_user.h"

//#include <QInputDialog>
//#include <QMainWindow>
#include <QtWidgets/QMessageBox>



CKickUser::CKickUser(QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    ui.setupUi(this);

    ui.comboBox->addItem("");
    ui.comboBox->addItem("1 hour");
    ui.comboBox->addItem("3 hour");
    ui.comboBox->addItem("6 hour");
    ui.comboBox->addItem("12 hour");
    ui.comboBox->addItem("1 day");
    ui.comboBox->addItem("3 day");
    ui.comboBox->addItem("7 day");
    ui.comboBox->addItem("forever");

    //connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(Login()));
    //connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
    connect(ui.exeButton, SIGNAL(clicked()), this, SLOT(Execute()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));

}

CKickUser::~CKickUser()
{

}

void CKickUser::Exit()
{
    done(0);
}

void CKickUser::Execute()
{
    accept();
}
