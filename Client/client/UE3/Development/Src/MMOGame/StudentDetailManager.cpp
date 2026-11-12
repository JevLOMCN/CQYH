/********************************************************************
	Created:	2013/03/27
	Filename: 	StudentDetailManager
	file ext:	cpp
	Author:		hemu@snda
	Purpose:	master/apprentice system Model StudentDetailManager manages the current apprentice list
*********************************************************************/
#include "stdafx.h"
//#include "Facade.h"
#include "StudentDetailManager.h"

using namespace MMO;
using namespace ExternalMessage;


StudentDetailManager::StudentDetailManager()
{

}

StudentDetailManager::~StudentDetailManager()
{

}

void StudentDetailManager::Init()
{
	
}

void StudentDetailManager::Shutdown()
{
	ClearAll();
}

void StudentDetailManager::AddStudentDetail( struct M2C_MASTER_NTF_MST_INFO::MASTER_DATA* StudentDetail )
{
	StudentDetailMap::iterator i = _StudentDetailMap.find( StudentDetail->dwRoleID );
	if(i == _StudentDetailMap.end())
	{
		ClientBriefStudentDetailInfo* newStudentDetail = new ClientBriefStudentDetailInfo;
		appMemcpy(newStudentDetail, StudentDetail, sizeof(MASTER_DATA));
		newStudentDetail->IsSelected = false;
		//newStudentDetail->CanApply = true;
		_StudentDetailMap.insert(std::make_pair(StudentDetail->dwRoleID, newStudentDetail));
	}
	else
	{
		//refresh data directly
		//leave IsSelected unchanged
		i->second->byLevel = StudentDetail->byLevel;
		i->second->byClass = StudentDetail->byClass; //just in case
		i->second->byCZCount = StudentDetail->byCZCount;
		i->second->byMstState = StudentDetail->byMstState;
		//warnf(NAME_Error, TEXT("[StudentDetailManager::AddStudentDetail] StudentDetail already exist:%d"), StudentDetail->dwRoleID);
	}
}

ClientBriefStudentDetailInfo* StudentDetailManager::FindStudentDetail( UINT32 StudentDetailID ) const
{
	StudentDetailMap::const_iterator i = _StudentDetailMap.find(StudentDetailID);
	if(i != _StudentDetailMap.end())
	{
		return i->second;
	}

	return 0;
}

void StudentDetailManager::MarkStudentDetailSelected(UINT32 StudentDetailID, bool sel)
{
	StudentDetailMap::const_iterator i = _StudentDetailMap.find(StudentDetailID);
	if(i != _StudentDetailMap.end())
	{
		i->second->IsSelected = sel;
		NotifyUpdateStudentDetailList();
	}
}

void MMO::StudentDetailManager::DeleteItem( UINT32 inID )
{
	StudentDetailMap::iterator i = _StudentDetailMap.find(inID);
	if(i != _StudentDetailMap.end())
	{
		delete i->second;
		_StudentDetailMap.erase(i);
		NotifyUpdateStudentDetailList();
	}
	else
	{
		warnf(NAME_Warning, TEXT("[StudentDetailManager::DeleteStudentDetail] StudentDetail not found:%d"), inID);
	}
}

void StudentDetailManager::ClearAll()
{
	StudentDetailMap::iterator i = _StudentDetailMap.begin();
	for (; i != _StudentDetailMap.end(); ++i)
	{
		delete i->second;
	}

	_StudentDetailMap.clear();
}

const StudentDetailMap& StudentDetailManager::GetAllStudentDetails() const
{
	return _StudentDetailMap;
}

void StudentDetailManager::SetUpdateStudentDetailListDelegate( FScriptDelegate fun )
{
	_UpdateStudentDetailListDelegate = fun;
}

void StudentDetailManager::NotifyUpdateStudentDetailList()
{
	//_UpdateStudentDetailListDelegate.Object->ProcessDelegate( MMOGAME_UpdateStudentDetailListDelegate,&_UpdateStudentDetailListDelegate,NULL);
}

void StudentDetailManager::ApplySelected()
{
	StudentDetailMap::iterator i = _StudentDetailMap.begin();
	for (; i != _StudentDetailMap.end();)
	{
		if(i->second->IsSelected)
		{
			//become apprentice
			//CMessageHandler::GetInstance().Send_StudentDetailSubmitStudApp( i->second->dwRoleID ) ; 
		}
		else
		{
			++i;
		}
	}
	NotifyUpdateStudentDetailList();
}

