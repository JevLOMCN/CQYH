#include "stdafx.h"
#include "forbidden_chat.h"

//#include <QInputDialog>
//#include <QMainWindow>
#include <QtWidgets/QMessageBox>



CForbiddenChat::CForbiddenChat(QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    ui.setupUi(this);

    ui.comboBox->addItem("");
    ui.comboBox->addItem("15 min");
    ui.comboBox->addItem("30 min");
    ui.comboBox->addItem("1 hour");
    ui.comboBox->addItem("3 hour");
    ui.comboBox->addItem("1 day");
    ui.comboBox->addItem("3 day");
    ui.comboBox->addItem("forever");


    //connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(Login()));
    //connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
    connect(ui.exeButton, SIGNAL(clicked()), this, SLOT(Execute()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));


}

CForbiddenChat::~CForbiddenChat()
{

}

void CForbiddenChat::Execute()
{
    accept();
}

void CForbiddenChat::Exit()
{
    done(0);
}
