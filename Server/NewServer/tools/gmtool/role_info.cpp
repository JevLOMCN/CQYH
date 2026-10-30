#include "stdafx.h"
#include <format>
#include <string>
#include "role_info.h"

//#include <QInputDialog>
//#include <QMainWindow>
#include <QtCore/QTextCodec>
#include <QtCore/QSignalBlocker>
#include "gm_ui.h"

#include "cmd_manager.h"
#include "gm_manager.h"
#include "my_tree_widget_item.h"
#include "message_handler.h"

#include "data_mgr.h"
#include "logic.h"
#include "server_mgr.h"
#include "gm_item_info.h"

CRoleInfo   *g_pRoleInfo;

namespace
{
QString legacy_text(const char* text)
{
    // Source narrow literals retain the protocol's GBK execution encoding;
    // convert UI labels explicitly instead of depending on the desktop locale.
    if (const auto* codec = QTextCodec::codecForName("GB18030"))
        return codec->toUnicode(text);
    return QString::fromLocal8Bit(text);
}

void erase_role_if_current(UINT32 id,const std::weak_ptr<const char>& reference)
{
    const auto lifetime=reference.lock();
    auto* role=CDataMgr::GetInstance().FindRole(id);
    if (lifetime && role && role->lifetime==lifetime) CDataMgr::GetInstance().DeleteRole(role);
}
}

CRoleInfo::CRoleInfo(UINT32 dwSessionID, UINT32 dwRoleID, BOOL bIsModify, QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
{
    ui.setupUi(this);
    m_editStatus = new QLabel(this);
    m_editStatus->setObjectName("roleEditStatus");
    m_editStatus->setWordWrap(true);
    m_editStatus->setStyleSheet("color: #b00020;");
    ui.gridLayout->addWidget(m_editStatus, ui.gridLayout->rowCount(), 0, 1, ui.gridLayout->columnCount());
    m_editStatus->hide();

    m_bIsModify = bIsModify;
    m_dwSessionID = dwSessionID;


    connect(ui.baseDataTreeWidget, &QTreeWidget::itemDoubleClicked, this, &CRoleInfo::BaseDataModify);
    connect(ui.baseDataTreeWidget, &QTreeWidget::itemChanged, this, &CRoleInfo::BaseDataChanged);
    
    connect(ui.itemWidget, &QTreeWidget::itemDoubleClicked, this, &CRoleInfo::ShowItemInfo);

    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(Exit()));
    connect(ui.OkButton, SIGNAL(clicked()), this, SLOT(EndModify())); 
    connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(ReturnPos()));
    connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(SetTargetRole()));

    connect(ui.takeItemButton, SIGNAL(clicked()), this, SLOT(TakeItem()));
    connect(ui.delQuestButton, SIGNAL(clicked()), this, SLOT(DelQuest()));
    connect(ui.delItemButton, SIGNAL(clicked()), this, SLOT(DelItem()));
    connect(ui.delMailButton, SIGNAL(clicked()), this, SLOT(DelMail()));

    m_dwRoleID = dwRoleID;
    m_dwTargetRoleID = 0;
    if (const auto* session=CSessionMgr::GetInstance().GetGMSession(dwSessionID))
        m_connectionLifetime=session->connection_lifetime;
    if (const auto* role=CDataMgr::GetInstance().FindRole(dwRoleID))
        m_roleLifetime=role->lifetime;

    m_edits.clear();

    m_ItemMap.clear();

    _Update();

Exit0:
    return;
}

CRoleInfo::~CRoleInfo()
{
    if (g_pRoleInfo == this)
        g_pRoleInfo = nullptr;
}



bool CRoleInfo::_AcceptEdit(const std::expected<void, std::string>& result)
{
    const QPointer<CRoleInfo> alive(this);
    const QPointer<QLabel> status(m_editStatus);
    if (result)
    {
        status->clear();
        if (alive && status) status->hide();
        return !alive.isNull();
    }
    status->setText(QString::fromUtf8(result.error().c_str()));
    if (alive && status) status->show();
    return false;
}

bool CRoleInfo::_ModelCurrent() const
{
    const auto lifetime=m_roleLifetime.lock();
    const auto* role=CDataMgr::GetInstance().FindRole(m_dwRoleID);
    return lifetime && role && role->lifetime==lifetime;
}

bool CRoleInfo::_EditSessionCurrent(bool selected) const
{
    const auto lifetime=m_connectionLifetime.lock();
    const auto* session=CSessionMgr::GetInstance().GetGMSession(m_dwSessionID);
    if (!lifetime || !session || session->connection_lifetime!=lifetime || !session->pSocketSession ||
        session->nConnectState!=SESSION_CONNECTED || session->eState!=GM_SVR_STATE_LOGIN)
        return false;
    if (!selected) return true;
    auto& servers=CServerMgr::GetInstance();
    return servers.UpdateSelectServer() && servers.IsSingleSelected() && servers.GetSessionIDByIndex(0)==m_dwSessionID;
}

void CRoleInfo::_DeleteCurrentModel()
{
    erase_role_if_current(m_dwRoleID,m_roleLifetime);
}

void CRoleInfo::ReturnPos()
{
    if (!m_bIsModify || m_rendering || m_closed || m_submitting)
        return;
    gm::UiAction action(*this,m_submitting);
    const QPointer<CRoleInfo> alive(this);
    if (!gm::confirm_action(*this,QStringLiteral("Confirm Data Changes？"),"roleEditConfirmation") || !alive)
        return;
    if (!_ModelCurrent()) { _AcceptEdit(std::unexpected("The cached role changed. Reopen the editor."));return; }
    if (_AcceptEdit(m_edits.return_position()) && alive)
        ui.pushButton->setDisabled(true);
}

void CRoleInfo::EndModify()
{
    if (m_closed || m_rendering || m_submitting)
        return;
    gm::UiAction action(*this,m_submitting);
    const QPointer<CRoleInfo> alive(this);
    const auto role_id=m_dwRoleID;
    const auto model_lifetime=m_roleLifetime;
    if (m_bIsModify)
    {
        if (!_ModelCurrent() || !_EditSessionCurrent(!m_edits.bytes().empty()))
        { _AcceptEdit(std::unexpected("The role or connection changed. Reopen the editor."));return; }
        if (!m_edits.bytes().empty())
        {
            auto& manager=CCmdManager::GetInstance();
            if (manager.IsCmdExecuting()) { _AcceptEdit(std::unexpected("Wait for the current command to finish."));return; }
            const auto reason = ui.modifyRoleReasonEdit->toPlainText().toUtf8();
            if (reason.isEmpty() || reason.size() >= GM_REASON_LEN || reason.contains('\0'))
            {
                _AcceptEdit(std::unexpected("Enter a nonempty reason within the field limit, without NUL characters."));
                return;
            }
            const auto view = m_edits.bytes();
            const std::vector<char> payload(view.begin(), view.end());
            const auto role=m_dwRoleID,target=m_dwTargetRoleID;
            if (!gm::confirm_action(*this,QStringLiteral("Confirm Data Changes and Exit Editor?"),"roleEditConfirmation") || !alive)
                return;
            if (m_closed || manager.IsCmdExecuting() || !_ModelCurrent() || !_EditSessionCurrent(true))
            { _AcceptEdit(std::unexpected("The execution state changed. Reopen the editor or retry."));return; }
            BOOL sent=FALSE;
            try { sent=manager.ModifyRole(role,target,static_cast<INT32>(payload.size()),payload.data(),reason.constData()); }
            catch (...) { if (alive) _AcceptEdit(std::unexpected("The changes could not be sent. They remain available to retry."));return; }
            if (!alive) { if (sent) erase_role_if_current(role_id,model_lifetime);return; }
            if (!sent)
            {
                _AcceptEdit(std::unexpected("The changes could not be sent. They remain available to retry."));
                return;
            }
        }
        else
        {
            BOOL released=FALSE;
            try { released=CMessageHandler::GetInstance().DoReleaseRole(m_dwSessionID,m_dwRoleID); }
            catch (...) { if (alive) _AcceptEdit(std::unexpected("The role could not be released. Retry or cancel the editor."));return; }
            if (!alive) { if (released) erase_role_if_current(role_id,model_lifetime);return; }
            if (!released)
            { _AcceptEdit(std::unexpected("The role could not be released. Retry or cancel the editor."));return; }
        }
        _DeleteCurrentModel();
    }
    m_closed = true;
    if (g_pRoleInfo == this)
        g_pRoleInfo = nullptr;
    done(0);
}

void CRoleInfo::Exit()
{
    if (m_closed || m_submitting)
        return;
    gm::UiAction action(*this,m_submitting);
    const QPointer<CRoleInfo> alive(this);
    const auto role_id=m_dwRoleID;
    const auto model_lifetime=m_roleLifetime;
    m_closed = true;
    if (g_pRoleInfo == this) g_pRoleInfo=nullptr;
    if (m_bIsModify)
    {
        if (_ModelCurrent() && _EditSessionCurrent(false))
        {
            try { const auto released=CMessageHandler::GetInstance().DoReleaseRole(m_dwSessionID,m_dwRoleID);LOG_CHECK_ERROR(released); }
            catch (...) { CRI("GM role release failed"); }
            if (!alive) { erase_role_if_current(role_id,model_lifetime);return; }
        }
        _DeleteCurrentModel();
    }
    if (g_pRoleInfo == this)
        g_pRoleInfo = nullptr;
    done(0);
}

void CRoleInfo::SetTargetRole()
{
    if (!m_bIsModify || m_rendering || m_closed || m_submitting)
        return;
    bool valid = false;
    const auto target = ui.lineEdit->text().toUInt(&valid);
    if (!valid)
    {
        _AcceptEdit(std::unexpected("Enter a role ID within the unsigned 32-bit range."));
        return;
    }
    m_dwTargetRoleID = target;
    ui.lineEdit->setDisabled(true);
}

void CRoleInfo::_RemoveSelected(QTreeWidget* tree, UINT16 type)
{
    if (!m_bIsModify || m_rendering || m_closed || m_submitting)
        return;
    auto* item = static_cast<CMyTreeWidgetItem*>(tree->currentItem());
    if (!item || item->isDisabled() || item->GetRoleType() != type)
        return;
    if (!_AcceptEdit(m_edits.remove(type, static_cast<UINT32>(item->GetID()))))
        return;
    item->setDisabled(true);
    item->setSelected(false);
}

void CRoleInfo::DelItem() { _RemoveSelected(ui.itemWidget, GM_ROLE_ITEM); }
void CRoleInfo::DelQuest() { _RemoveSelected(ui.questWidget, GM_ROLE_QUEST); }
void CRoleInfo::DelMail() { _RemoveSelected(ui.mailTreeWidget, GM_ROLE_MAIL); }

void CRoleInfo::TakeItem()
{
    if (!m_bIsModify || m_rendering || m_closed || m_submitting || !m_dwTargetRoleID)
        return;
    auto* item = static_cast<CMyTreeWidgetItem*>(ui.itemWidget->currentItem());
    if (!item || item->isDisabled() || item->GetRoleType() != GM_ROLE_ITEM)
        return;
    if (!_AcceptEdit(m_edits.take_item(static_cast<UINT32>(item->GetID()), static_cast<UINT64>(item->Get64ID()))))
        return;
    item->setDisabled(true);
    item->setSelected(false);
}

void CRoleInfo::reject()
{
    close();
}

void CRoleInfo::closeEvent(QCloseEvent *event)
{
    if (m_submitting) { event->ignore();return; }
    Exit();

    return ;
}

void CRoleInfo::BaseDataModify(QTreeWidgetItem* raw, int column)
{
    if (!raw || !m_bIsModify || m_rendering || m_closed || m_submitting)
        return;
    const auto* item = static_cast<CMyTreeWidgetItem*>(raw);
    const auto base = item->GetBaseDataType();
    const bool editable = item->GetRoleType() == GM_ROLE_BASE_DATA &&
        (base == GM_RBD_LEVEL || base == GM_RBD_GMLEVEL || base == GM_RBD_EXP || base == GM_RBD_PKVALUE ||
         (base >= GM_RBD_MONEY_2 && base <= GM_RBD_MONEY_14));
    auto flags = raw->flags();
    if (column == 1 && editable)
        flags |= Qt::ItemIsEditable;
    else
        flags &= ~Qt::ItemIsEditable;
    raw->setFlags(flags);
}

void CRoleInfo::BaseDataChanged(QTreeWidgetItem* raw, int column)
{
    if (!raw || column != 1 || !m_bIsModify || m_rendering || m_closed)
        return;
    if (m_submitting)
    {
        const QSignalBlocker blocker(ui.baseDataTreeWidget);
        raw->setText(1,raw->data(1,Qt::UserRole).toString());return;
    }
    gm::UiAction rendering(*this,m_rendering);
    const QPointer<CRoleInfo> alive(this);
    auto* item = static_cast<CMyTreeWidgetItem*>(raw);
    if (item->GetRoleType() != GM_ROLE_BASE_DATA)
        return;
    const auto base = static_cast<UINT16>(item->GetBaseDataType());
    bool valid = false;
    std::expected<void, std::string> result;
    if (base == GM_RBD_EXP || base == GM_RBD_PKVALUE)
    {
        const auto value = item->text(1).toUInt(&valid);
        if (valid) result = m_edits.set_unsigned(base, value);
    }
    else if (base == GM_RBD_LEVEL || base == GM_RBD_GMLEVEL || (base >= GM_RBD_MONEY_2 && base <= GM_RBD_MONEY_14))
    {
        const auto value = item->text(1).toInt(&valid);
        if (valid) result = m_edits.set_signed(base, value);
    }
    else
        return;
    if (!valid || !result)
    {
        item->setText(1, item->data(1, Qt::UserRole).toString());
        if (!valid)
            result = std::unexpected("Enter an integer within this field's range.");
        _AcceptEdit(result);
        return;
    }
    if (!_AcceptEdit(result) || !alive) return;
    item->setData(1, Qt::UserRole, item->text(1));
}

void CRoleInfo::ShowItemInfo(QTreeWidgetItem * item, int column)
{
    std::string text;
    GM_ITEM_MAP::iterator iter;
    CMyTreeWidgetItem   *pTreeItem = (CMyTreeWidgetItem*)item;
    LOG_PROCESS_ERROR(pTreeItem);

    iter = m_ItemMap.find(pTreeItem->Get64ID());
    LOG_PROCESS_ERROR(iter != m_ItemMap.end());

    text += std::format("{}\t{}\n", "Item Type ID", iter->second.dwItemID);
    text += std::format("{}\t{}\n", "Item GUID", iter->second.qwGuid);
    text += std::format("{}\t{}\n", "Container", iter->second.wConSlot);
    text += std::format("{}\t{}\n", "Container Position", iter->second.wItemSlot);
    text += std::format("{}\t{}\n", "Item Type", iter->second.nType);
    text += std::format("{}\t{}\n", "Item Subtype", iter->second.nSubType);
    text += std::format("{}\t{}\n", "Quantity", iter->second.nNum);
    text += std::format("{}\t{}\n", "Binding Type", iter->second.byBindType);

    text += std::format("{}\t{}\n", "Identification Type", iter->second.byIdentified);
    text += std::format("{}\t{}\n", "Random Attribute 1", iter->second.wRandomAttrID[0]);
    text += std::format("{}\t{}\n", "Random Attribute 2", iter->second.wRandomAttrID[1]);
    text += std::format("{}\t{}\n", "Random Attribute 3", iter->second.wRandomAttrID[2]);
    text += std::format("{}\t{}\n", "Random Attribute 4", iter->second.wRandomAttrID[3]);

    text += std::format("{}\t{}\n", "Signature", iter->second.dwRoleID);
    text += std::format("{}\t{}\n", "Gold Spent", iter->second.dwCostMoney);
    text += std::format("{}\t{}\n", "Luck", iter->second.byLuck);

    text += std::format("{}\t{}\n", "Weapon Upgrade Attempts", iter->second.byLevelupTimes);
    text += std::format("{}\t{}\n", "Weapon Upgrade Physical Points", iter->second.byLevelupPhyPoint);
    text += std::format("{}\t{}\n", "Weapon Upgrade Magic Points", iter->second.byLevelupMagPoint);
    text += std::format("{}\t{}\n", "Weapon Upgrade Taoism Points", iter->second.byLevelupTaoPoint);
    text += std::format("{}\t{}\n", "Weapon Hidden Type", iter->second.byLevelupTypeHidden);
    text += std::format("{}\t{}\n", "Weapon Hidden Points", iter->second.byLevelupPointHidden);

    text += std::format("{}\t{}\n", "Soul Forging 1", iter->second.wRune[0]);
    text += std::format("{}\t{}\n", "Soul Forging 2", iter->second.wRune[1]);
    text += std::format("{}\t{}\n", "Soul Forging 3", iter->second.wRune[2]);
    text += std::format("{}\t{}\n", "Soul Forging 4", iter->second.wRune[3]);
    text += std::format("{}\t{}\n", "Soul Forging 5", iter->second.wRune[4]);
    text += std::format("{}\t{}\n", "Soul Forging 6", iter->second.wRune[5]);

    text += std::format("{}\t{}\n", "Socket Configuration", iter->second.dwSocketCombination);
    text += std::format("{}\t{}\n", "Gem ID 1", iter->second.nSoulID[0]);
    text += std::format("{}\t{}\n", "Gem ID 2", iter->second.nSoulID[1]);
    text += std::format("{}\t{}\n", "Gem ID 3", iter->second.nSoulID[2]);


    {
    auto *pItemInfo = new CGMItemInfo(this);
    pItemInfo->setAttribute(Qt::WA_DeleteOnClose);

    LOG_PROCESS_ERROR(pItemInfo);

    pItemInfo->ShowInfo(legacy_text(text.c_str()), FALSE);

    pItemInfo->show();
    }

Exit0:
    return;
}

void CRoleInfo::_UpdateBaseData(const gm::RoleRecord& record)
{
    QString value;
    bool disabled = false;
    switch (record.base)
    {
    case GM_RBD_ID:
        value = QString::number(record.value<UINT32>());
        disabled = true;
        break;
    case GM_RBD_PKVALUE:
    case GM_RBD_EXP:
        value = QString::number(record.value<UINT32>());
        break;
    case GM_RBD_DELETETIME:
    {
        const auto deleted = record.value<UINT32>();
        QDateTime date;
        date.setTime_t(deleted);
        date.setTimeZone(QTimeZone(28800));
        value = deleted ? date.toString("yyyy-MM-dd hh:mm:ss") : "0";
        disabled = true;
        break;
    }
    case GM_RBD_NAME:
    case GM_RBD_LAST_LOGOUT_TIME:
    case GM_RBD_FORBIDDEND_CHAT_TIME:
    case GM_RBD_FREEZE_ROLE_TIME:
    case GM_RBD_FREEZE_CHEAT_ROLE:
        value = QString::fromUtf8(record.text().c_str());
        disabled = true;
        break;
    case GM_RBD_GROUPID:
    case GM_RBD_CLASS:
    case GM_RBD_SEX:
    case GM_RBD_LEVEL:
    case GM_RBD_MAP_ID:
    case GM_RBD_MAP_SERIAL_ID:
    case GM_RBD_GMLEVEL:
        value = QString::number(record.value<INT32>());
        break;
    default:
        if (record.base < GM_RBD_MONEY_1 || record.base > GM_RBD_MONEY_14)
            return;
        value = QString::number(record.value<INT32>());
        disabled = record.base == GM_RBD_MONEY_1;
        break;
    }
    auto item = std::make_unique<CMyTreeWidgetItem>();
    item->SetRoleType(record.type);
    item->SetBaseDataType(record.base);
    item->setText(0, legacy_text(GetBaseDataName(record.base)));
    item->setText(1, value);
    item->setData(1, Qt::UserRole, value);
    item->setDisabled(disabled);
    ui.baseDataTreeWidget->addTopLevelItem(item.release());
}


void CRoleInfo::_Update()
{
    if (m_rendering || m_closed)
        return;
    const auto* role = CDataMgr::GetInstance().FindRole(m_dwRoleID);
    if (!role)
        return;
    const auto document = role->document;
    QScopedValueRollback<bool> rendering(m_rendering, true);
    QSignalBlocker changes(ui.baseDataTreeWidget);
    ui.baseDataTreeWidget->clear();
    ui.questWidget->clear();
    ui.itemWidget->clear();
    ui.mailTreeWidget->clear();
    ui.spellTreeWidget->clear();
    ui.titleTreeWidget->clear();
    m_ItemMap.clear();
    for (const auto& record : document)
    {
        if (record.type == GM_ROLE_BASE_DATA)
        {
            _UpdateBaseData(record);
            continue;
        }
        auto item = std::make_unique<CMyTreeWidgetItem>();
        item->SetRoleType(record.type);
        switch (record.type)
        {
        case GM_ROLE_SPELL:
        {
            const auto spell = record.value<GM_ROLE_SPELL_SLOT>();
            item->SetID(spell.nSpellID);
            item->Set64ID(spell.nRuneID);
            item->setText(0, QString::fromUtf8(spell.szName));
            if (spell.nEquipedIndex >= qssActiveBegin)
                item->setText(1, QString::number(static_cast<qlonglong>(spell.nEquipedIndex) - qssActiveBegin + 1));
            ui.spellTreeWidget->addTopLevelItem(item.release());
            break;
        }
        case GM_ROLE_TITLE:
        {
            const auto title = record.value<GM_ROLE_TITLE_SLOT>();
            item->SetID(title.nTemplateID);
            item->setText(0, QString::fromUtf8(title.szName));
            item->setText(1, QString::number(title.dwExpireTime));
            ui.titleTreeWidget->addTopLevelItem(item.release());
            break;
        }
        case GM_ROLE_ITEM:
        {
            const auto attributes = record.value<GM_ROLE_ITEM_SLOT>();
            item->Set64ID(attributes.qwGuid);
            item->SetID((static_cast<UINT32>(attributes.wConSlot) << 16) | attributes.wItemSlot);
            item->setText(0, QString::number(attributes.dwItemID));
            item->setText(1, QString::number(static_cast<qulonglong>(attributes.qwGuid)));
            item->setText(2, QString::fromUtf8(record.text(offsetof(GM_ROLE_ITEM_SLOT, szData)).c_str()));
            item->setText(3, legacy_text(GetItemContainerString(attributes.wConSlot)));
            item->setText(4, legacy_text(GetItemTypeString(attributes.nType)));
            item->setText(5, QString::number(attributes.nNum));
            m_ItemMap.try_emplace(attributes.qwGuid, attributes);
            ui.itemWidget->addTopLevelItem(item.release());
            break;
        }
        case GM_ROLE_QUEST:
            item->SetID(record.value<UINT32>());
            item->setText(0, QString::number(record.value<UINT32>()));
            item->setText(1, QString::fromUtf8(record.text(sizeof(UINT32)).c_str()));
            ui.questWidget->addTopLevelItem(item.release());
            break;
        case GM_ROLE_MAIL:
        {
            const auto mail = record.value<GM_ROLE_MAIL_SLOT>();
            item->SetID(mail.dwMailID);
            item->setText(0, QString::fromUtf8(record.text(offsetof(GM_ROLE_MAIL_SLOT, szData)).c_str()));
            item->setText(1, legacy_text(cszMailTypeName[mail.nMailType]));
            item->setText(2, legacy_text(cszMailIsNewName[mail.nState]));
            item->setText(3, legacy_text(GetMailAttachType(mail.nAttachType)));
            if (mail.nAttachType)
            {
                item->setText(4, QString::number(mail.nItemID));
                item->setText(5, QString::number(mail.nItemCount));
            }
            ui.mailTreeWidget->addTopLevelItem(item.release());
            break;
        }
        default:
            break;
        }
    }
    ui.baseDataTreeWidget->resizeColumnToContents(0);
    ui.questWidget->resizeColumnToContents(0);
    ui.itemWidget->resizeColumnToContents(0);
    ui.itemWidget->resizeColumnToContents(1);
    if (!m_bIsModify)
    {
        ui.delQuestButton->setDisabled(true);
        ui.delItemButton->setDisabled(true);
        ui.delMailButton->setDisabled(true);
        ui.takeItemButton->setDisabled(true);
        ui.pushButton->setDisabled(true);
    }
}
