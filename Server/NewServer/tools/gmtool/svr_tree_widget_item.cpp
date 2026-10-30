#include "stdafx.h"
#include "svr_tree_widget_item.h"


CSvrTreeWidgetItem::CSvrTreeWidgetItem() :
QTreeWidgetItem()
{

}

CSvrTreeWidgetItem::~CSvrTreeWidgetItem()
{

}

INT32 CSvrTreeWidgetItem::GetSessionID()
{
    return m_dwSessionID;
}

void CSvrTreeWidgetItem::SetSessionID(UINT32 dwSessionID)
{
    m_dwSessionID = dwSessionID;
}
