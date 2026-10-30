#include "stdafx.h"
#include "mail.h"

//#include <QInputDialog>
//#include <QMainWindow>
#include <QtWidgets/QMessageBox>

CMail::CMail(QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    ui.setupUi(this);

    //connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(Login()));
    //connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
    connect(ui.exeButton, SIGNAL(clicked()), this, SLOT(Execute()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
}

CMail::~CMail()
{

}

void CMail::Exit()
{
    done(0);
}

void CMail::Execute()
{
    accept();
}
