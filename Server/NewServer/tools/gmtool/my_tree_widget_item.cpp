/********************************************************************
	created:	19:8:2013   15:43
	file:		my_tree_widget_item.cpp
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "my_tree_widget_item.h"


CMyTreeWidgetItem::CMyTreeWidgetItem() :
QTreeWidgetItem()
{
    m_nSvrID = 0;
    m_qwID = 0;
    m_dwID = 0;

    m_nRoleType = 0;
    m_nBaseType = 0;
}

CMyTreeWidgetItem::~CMyTreeWidgetItem()
{

}

INT32 CMyTreeWidgetItem::GetSvrID() const
{
    return m_nSvrID;
}

//CRole* CMyTreeWidgetItem::GetRoleDataPtr()
//{
//    return m_poRole;
//}

//void CMyTreeWidgetItem::SetSvrID(INT32 nSvrID)
//{
//    m_nSvrID = nSvrID;
//}

//void CMyTreeWidgetItem::SetCRolePtr(CRole *poRole)
//{
//    m_poRole = poRole;
//}

