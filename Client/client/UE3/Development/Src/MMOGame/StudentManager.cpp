/********************************************************************
	Created:	2013/03/27
	Filename: 	StudentManager
	file ext:	cpp
	Author:		hemu@snda
	Purpose:	master/apprentice system Model StudentManager manages apprentices currently requesting apprenticeship
*********************************************************************/
#include "stdafx.h"
//#include "Facade.h"
#include "StudentManager.h"
using namespace MMO;

IMPLEMENT_COMPARE_POINTER( ClientBriefStudentInfo, __StudentName, { return strlen( A->szRoleName ) < strlen( B->szRoleName ) ? 1 : -1; } );
IMPLEMENT_COMPARE_POINTER( ClientBriefStudentInfo, __StudentClass, { return A->byClass < B->byClass ? 1 : -1; } );
IMPLEMENT_COMPARE_POINTER( ClientBriefStudentInfo, __StudentLevel, { return A->byLevel < B->byLevel ? 1 : -1; } );


void StudentManager::SortMembers(INT type)
{
    switch ( type )
    {
    case EMST_NAME:
        _StudentMap.ValueSort<COMPARE_POINTER_CLASS( ClientBriefStudentInfo, __StudentName )>();
        break;
    case EMST_CLASS:
        _StudentMap.ValueSort<COMPARE_POINTER_CLASS( ClientBriefStudentInfo, __StudentClass )>();
        break;
    case EMST_LEVEL:
        _StudentMap.ValueSort<COMPARE_POINTER_CLASS( ClientBriefStudentInfo, __StudentLevel )>();
        break;
    }
    //_UpdateGuildFullMemberListDelegate.Object->ProcessDelegate(MMOGAME_UpdateGuildFullMemberListDelegate, &_UpdateGuildFullMemberListDelegate, NULL);
}


StudentManager::StudentManager()
{

}

StudentManager::~StudentManager()
{

}

void StudentManager::Init()
{
	
}

void StudentManager::Shutdown()
{
	ClearAll();
}

void StudentManager::AddStudent( struct M2C_MASTER_ADD_APPLICANT* Student )
{
    ClientBriefStudentInfo* tStudent = FindStudent( Student->dwRoleID );
    if( tStudent )
    {
        tStudent->byLevel = Student->byLevel;
        tStudent->byClass = Student->byClass;
    }
    else
    {
        ClientBriefStudentInfo* newStudent = new ClientBriefStudentInfo;
        appMemcpy(newStudent, Student, sizeof( M2C_MASTER_ADD_APPLICANT ));
        newStudent->IsSelected = false;
        _StudentMap.Set( Student->dwRoleID, newStudent );
    }
}

StudentMapIterator StudentManager::BeginIterator()
{
    StudentMapIterator it( _StudentMap );
    return it;
}

ClientBriefStudentInfo* StudentManager::FindStudent( UINT32 studentID )
{
    ClientBriefStudentInfo** studentPP = _StudentMap.Find( studentID ); 
    if( studentPP && *studentPP )
        return *studentPP;
    else
        return NULL;
}

void StudentManager::MarkStudentSelected(UINT32 studentID, bool sel)
{
    ClientBriefStudentInfo* student = FindStudent( studentID );
    if( student )
    {
        student->IsSelected = sel;
        NotifyUpdateStudentList();
    }
}

void MMO::StudentManager::DeleteItem( UINT32 inID )
{

    ClientBriefStudentInfo* student = FindStudent( inID );
    if( student )
    {
        _StudentMap.Remove( inID );
        delete student;
        NotifyUpdateStudentList();
    }
    else
    {
        warnf(NAME_Warning, TEXT("[StudentManager::DeleteStudent] Student not found:%d"), inID);
    }
}

void StudentManager::ClearAll()
{
    StudentMapIterator i = BeginIterator();
    for (;  i ; ++i )
    {
        ClientBriefStudentInfo* student = i.Value();
        if( student )
        {
            delete student;
        }
    }

    _StudentMap.Empty();
}

const StudentMap& StudentManager::GetAllStudents() const
{
	return _StudentMap;
}

void StudentManager::SetUpdateStudentListDelegate( FScriptDelegate fun )
{
	_UpdateStudentListDelegate = fun;
}

void StudentManager::NotifyUpdateStudentList()
{
	//_UpdateStudentListDelegate.Object->ProcessDelegate(MMOGAME_UpdateStudentListDelegate,&_UpdateStudentListDelegate,NULL);
	//_UpdateStudentListDelegate.Object->ProcessDelegate( MMOGAME_UpdateStudentListDelegate,&_UpdateStudentListDelegate,NULL);
}

void StudentManager::ApplySelected()
{
    //accept apprenticeship
    StudentMapIterator i = BeginIterator();
    for ( ;  i ; )
    {
        ClientBriefStudentInfo* student = i.Value();
        if( student && student->IsSelected)
        {
            //ApplyStudent( student->nStudentID  );
            break;
        }
        else
        {
            ++i;
        }
    }
    NotifyUpdateStudentList();
}

