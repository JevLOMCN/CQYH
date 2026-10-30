#include "stdafx.h"
#include "role_gm_cmd.h"
#include "gm_ui.h"

CRoleGMCmd::CRoleGMCmd(QWidget* parent, Qt::WindowFlags flags) : QDialog(parent,flags)
{
    ui.setupUi(this);
    m_status=new QLabel(this);
    m_status->setObjectName(QStringLiteral("roleCommandStatus"));
    m_status->setTextFormat(Qt::PlainText);m_status->setWordWrap(true);m_status->hide();
    ui.gridLayout->addWidget(m_status,ui.gridLayout->rowCount(),0,1,2);
    connect(ui.okButton,&QPushButton::clicked,this,&CRoleGMCmd::OK);
    connect(ui.exitButton,&QPushButton::clicked,this,&CRoleGMCmd::Exit);
}
CRoleGMCmd::~CRoleGMCmd()=default;

void CRoleGMCmd::OK()
{
    if (m_bExe || m_confirming) return;
    gm::UiAction action(*this,m_confirming);
    const QPointer<CRoleGMCmd> alive(this);
    auto text = gm::text_field(ui.textEdit->toPlainText(), MAX_GM_COMMAND_LEN, QStringLiteral("Command"), 2);
    auto reason = gm::text_field(ui.textEdit_2->toPlainText(), GM_REASON_LEN, QStringLiteral("Reason"), 2);
    if (!text || !reason)
    {
        m_status->setText(!text ? text.error() : reason.error());m_status->show();return;
    }
    if (!gm::confirm_action(*this,QStringLiteral("Confirm executing this GM command?")) || !alive) return;
    m_Buffer=std::move(*text);m_Reason=std::move(*reason);m_bExe=true;
    accept();
}
void CRoleGMCmd::Exit() { reject(); }
