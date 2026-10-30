#include "stdafx.h"
#include "gm_item_info.h"


CGMItemInfo::CGMItemInfo(QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    ui.setupUi(this);

    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(Exit()));
}

CGMItemInfo::~CGMItemInfo()
{

}


void CGMItemInfo::Exit()
{
    done(0);
}

void CGMItemInfo::ShowInfo(const char *pcszBuffer, bool bBiggerFont)
{
    if (!pcszBuffer)
        return;

    ShowInfo(QString::fromLocal8Bit(pcszBuffer), bBiggerFont);
}

void CGMItemInfo::ShowInfo(const QString& text, bool bBiggerFont)
{
    ui.textEdit->setPlainText(text);

    if (bBiggerFont)
    {
        QFont   qft;
        qft.setBold(true);
        qft.setPointSize(12);
        ui.textEdit->setFont(qft);
    }

    return;
}

