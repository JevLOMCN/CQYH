#include "stdafx.h"
#include "view_guild.h"
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>

CViewGuild::CViewGuild(QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    ui.setupUi(this);

    //connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(Login()));
    //connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
    connect(ui.exeButton, SIGNAL(clicked()), this, SLOT(Execute()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
}

CViewGuild::~CViewGuild()
{

}

void CViewGuild::Execute()
{
    accept();
}

void CViewGuild::Exit()
{
    done(0);
}
