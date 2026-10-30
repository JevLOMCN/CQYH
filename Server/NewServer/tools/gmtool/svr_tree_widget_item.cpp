/********************************************************************
	created:	13:2:2014   10:36
	file:		svr_tree_widget_item.cpp
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

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
