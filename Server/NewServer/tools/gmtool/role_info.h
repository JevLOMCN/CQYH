/********************************************************************
	created:	25:12:2013   16:12
	file:		role_info.h
	author:		Allan Tao
	
	purpose:	
*********************************************************************/



#ifndef _H_ROLE_INFO_H_
#define _H_ROLE_INFO_H_


#include "../qt_cpp23.h"
#include "gm_role_data.h"
#include <map>
#include <memory>
#include <QtWidgets/QDialog>
#include <QCloseEvent>
#include <QtWidgets/QLabel>
#include "ui_role_info.h"
#include "gm_def.h"



class CItemTreeWidgetItem;

class CRoleInfo : public QDialog
{
    Q_OBJECT

public:
    CRoleInfo(UINT32 dwSessionID, UINT32 dwRoleID, BOOL bIsModify = FALSE, QWidget *parent = 0, Qt::WindowFlags flags = 0);

    ~CRoleInfo();

    void reject() override;





protected:
    void _Update();
    void _UpdateBaseData(const gm::RoleRecord& record);
    void _RemoveSelected(QTreeWidget* tree, UINT16 type);
    bool _AcceptEdit(const std::expected<void, std::string>& result);
    bool _ModelCurrent() const;
    bool _EditSessionCurrent(bool selected) const;
    void _DeleteCurrentModel();

public slots:
    //void AddItem();

    void SetTargetRole();
    
    void DelItem();
    void DelQuest();
    void DelMail();
    void TakeItem();

    void ReturnPos();
    void EndModify();
    void Exit();

    void closeEvent(QCloseEvent *event);

    void BaseDataModify(QTreeWidgetItem * item, int column);
    void BaseDataChanged(QTreeWidgetItem * item, int column);

    void ShowItemInfo(QTreeWidgetItem * item, int column);


private:
    UINT32 m_dwRoleID{};
    UINT32 m_dwTargetRoleID{};

    Ui::RoleInfoDialog ui;

    BOOL        m_bIsModify;
    UINT32      m_dwSessionID;

    gm::RoleEdits m_edits;
    bool m_rendering = false;
    bool m_closed = false;
    bool m_submitting = false;
    std::weak_ptr<const char> m_connectionLifetime,m_roleLifetime;
    QLabel* m_editStatus = nullptr;


    typedef std::map<UINT64, GM_ROLE_ITEM_SLOT> GM_ITEM_MAP;
    GM_ITEM_MAP     m_ItemMap;
};


extern CRoleInfo   *g_pRoleInfo;


#endif //_H_ROLE_INFO_H_












