#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "stdafx.h"
//#include "Facade.h"
#include "FriendManager.h"
#include "FriendChatManager.h"
#include "MMOChatManager.h"

template<> FriendChatManager* Singleton<FriendChatManager>::ms_Singleton = 0;

#define FriendChatLogFile TEXT("FriendChatLogFile")


FriendChatManager::FriendChatManager()
{
	HasLoad = FALSE;
	HasSave = FALSE;
}


FriendChatManager::~FriendChatManager()
{
	//SaveChatLogToFile();
}

void FriendChatManager::Init()
{
	MMOUI::RegisterEventHandler(this, GameEvent_ChangeRole);
}

void FriendChatManager::UnInit()
{
	
}

void FriendChatManager::ClearAll()
{
	//save friend chat history
	//SaveChatLogToFile();

	//clear all chat conversations
	for ( INT i = 0 ; i < ContactLogList.Num() ; i++ )
	{
		ContactLogList(i).LogList.Empty();
	}
	ContactLogList.Empty();

	for ( INT i = 0 ; i < ReadContactLogList.Num() ; i++ )
	{
		ReadContactLogList(i).LogList.Empty();
	}
	ReadContactLogList.Empty();

	for ( INT i = 0 ; i < UnReadContactLogList.Num() ; i++ )
	{
		UnReadContactLogList(i).LogList.Empty();
	}
	UnReadContactLogList.Empty();
}

void FriendChatManager::SaveChatLogToFile()
{
	//if( !( HasLoad && !HasSave ) )
	//{
	//	//not read
	//	return;
	//}

	FArchive* pArchive = NULL;
	
	if( MMOFacade::getSingleton().BeginExportAccountData( &pArchive , FriendChatLogFile ) )
	{
		INT size = ContactLogList.Num();
		pArchive->Serialize( (void*)(&size) , sizeof(INT) );
		for ( INT i = 0 ; i < size ; i++ )
		{
			pArchive->Serialize( (void*)( &ContactLogList(i).RoleId ) , sizeof(INT) );	

			INT nameLen = ContactLogList(i).PlayerName.Len()*sizeof(TCHAR);
			pArchive->Serialize( (void*)( &nameLen ) , sizeof(INT) );

			pArchive->Serialize( (void*)( ContactLogList(i).PlayerName.GetCharArray().GetData() ) , nameLen );

			INT num = ContactLogList(i).LogList.Num();
			pArchive->Serialize( (void*)( &num ) , sizeof(INT) );
			for ( INT j = 0 ; j < num ; j++ )
			{
				INT textLen = ContactLogList(i).LogList(j).Len()*sizeof(TCHAR);
				pArchive->Serialize( (void*)( &textLen ) , sizeof(INT) );
				pArchive->Serialize( (void*)( ContactLogList(i).LogList(j).GetCharArray().GetData() ) , textLen );
			}
		}

	}
	MMOFacade::getSingleton().EndExportAccountData( pArchive );

	//HasSave = TRUE;
	//HasLoad = FALSE;
}

void FriendChatManager::LoadChatLogFromFile()
{

	FArchive* pArchive      = NULL;
	size_t  lDataLength		= 0; 

	ContactLogList.Empty();

	//read binary
	if( MMOFacade::getSingleton().BeginLoadAccountBinary( &pArchive , FriendChatLogFile ) )
	{
		INT TotalSize = pArchive->TotalSize();
		INT	size = 0;

		pArchive->Serialize( &size, sizeof(UINT32) );

		for ( INT i = 0 ; i < size ; ++i )
		{
			ContactLogStruct contactLog(EC_EventParm);
			
			pArchive->Serialize( &contactLog.RoleId , sizeof(contactLog.RoleId) );
			INT nameLen = 0;
			pArchive->Serialize( &nameLen, sizeof(nameLen) );

			TCHAR playerName[1024] = {0};
			pArchive->Serialize( &playerName, nameLen );
			contactLog.PlayerName = playerName;
			contactLog.LogList.Empty();
			
			INT num = 0;//??? is this num wrong?
			pArchive->Serialize( &num, sizeof(num) );
				
			for ( INT j = 0 ; j < num ; j++ )
			{
				INT textLen = 0;
				pArchive->Serialize( &textLen, sizeof(textLen) );;
				TCHAR chatText[2048] = {0};
				pArchive->Serialize( (void*)( chatText ) , textLen );
				contactLog.LogList.AddItem( chatText );
			}

			if( FriendManager::getSingleton().IsMyFriend( contactLog.RoleId ) )
			{
				//only add for friends
				ContactLogList.AddItem(contactLog);
			}
			
		}
	}

	MMOFacade::getSingleton().EndLoadAccountBinary( pArchive );

	//HasLoad = TRUE;
	//HasSave = FALSE;
}
BOOL FriendChatManager::IsInRoleChatList(int nRoleID)
{
	for (int n = 0; n < curChatRoleIDList.Num(); ++n)
	{
		if(curChatRoleIDList(n) == nRoleID)
			return TRUE;
	}
	return FALSE;
}

INT FriendChatManager::GetRoleIndexByChatList(INT nRoleId)
{
	
	for (int n = 0; n < curChatRoleIDList.Num(); ++n)
	{
		if(curChatRoleIDList(n) == nRoleId)
			return n;
	}
	return -1;
}

void FriendChatManager::AddNewRoleIDChatList(int nRoleID)
{
	for (int n = 0; n < curChatRoleIDList.Num(); ++n)
	{
		if(curChatRoleIDList(n) == nRoleID)
			return;
	}
	curChatRoleIDList.AddItem(nRoleID);
}
void FriendChatManager::RemoveRoleIDChatList(int nRoleID)
{
	for (int n = 0; n < curChatRoleIDList.Num(); ++n)
	{
		if(curChatRoleIDList(n) == nRoleID)
		{
			curChatRoleIDList.Remove(n,1);
			break;
		}
	}
}

void FriendChatManager::ClearRoleIDChatList()
{
	curChatRoleIDList.Empty();
}

void FriendChatManager::RemoveUnReadChatContent(INT nRoleID)
{
	for(INT i = 0 ; i< UnReadContactLogList.Num() ;i++ ) 
	{
		if( UnReadContactLogList(i).RoleId == nRoleID )
		{
			UnReadContactLogList.Remove(i,1);
			return;
		}
	}
}
void FriendChatManager::AddNewChatReadLog(INT nRoleID,FString& strName, FString strContent,BOOL bfromSelf)
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
	{
		return;
	}
	ContactLogStruct CL(EC_EventParm);

	FString senderName;
	if(bfromSelf)
	{
		senderName = player->ActorName;
	}
	else
	{
		senderName = strName;
	}

	FString Msg = _GetShowText( senderName , strContent , bfromSelf );

	for(INT i = 0 ; i< ReadContactLogList.Num() ;i++ ) 
	{
		if( ReadContactLogList(i).RoleId == nRoleID )
		{
			ReadContactLogList(i).LogList.AddItem( Msg );
			return;
		}
	}
	CL.RoleId = nRoleID;
	CL.PlayerName = strName;
	CL.LogList.AddItem( Msg );
	ReadContactLogList.AddItem(CL);
}

void FriendChatManager::AddNewChatUnReadLog(INT nRoleID,FString& strName, FString strContent,BOOL addLog)
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
	{
		return;
	}

	ContactLogStruct CL(EC_EventParm);
	FString Msg = _GetShowText( strName , strContent , player->ServerID == nRoleID ); 

	for(INT i = 0 ; i< UnReadContactLogList.Num() ;i++ ) 
	{
		if( UnReadContactLogList(i).RoleId == nRoleID )
		{
			if(addLog)
			{
				UnReadContactLogList(i).LogList.AddItem( Msg );
			}
			return;
		}
	}
	CL.RoleId = nRoleID;
	CL.PlayerName = strName;
	if(addLog)
	{
		CL.LogList.AddItem( Msg );
	}
	UnReadContactLogList.AddItem(CL);
}

void FriendChatManager::AddNewChatLogList(INT nRoleID,FString& strName, FString strContent,BOOL bfromSelf)
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
	{
		return;
	}
	FriendManager::getSingleton().NotifyNewContact(nRoleID);

	ContactLogStruct CL(EC_EventParm);
	FString senderName;
	if(bfromSelf)
	{
		senderName = player->ActorName;
	}
	else
	{
		senderName = strName;
	}

	FString Msg = _GetShowText( senderName , strContent , bfromSelf );

	//if this person already has a message queue, add to the existing queue
	for(INT i = 0 ; i< ContactLogList.Num() ;i++ ) 
	{
		if( ContactLogList(i).RoleId == nRoleID )
		{
			ContactLogList(i).LogList.AddItem( Msg );
			if(ContactLogList(i).LogList.Num() > MaxFriendChatLogNum )
			{
				ContactLogList(i).LogList.Remove(0);
			}
			return;
		}
	}
	//otherwise create a new one
	CL.RoleId = nRoleID;
	CL.PlayerName = strName;
	CL.LogList.AddItem( Msg );
	ContactLogList.AddItem(CL);
}

FString FriendChatManager::_GetShowText( const FString& playerName , const FString& content , UBOOL myWord )
{
	_SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);
	FString sender = playerName + FString::Printf( TEXT(": %02d:%02d:%02d ") , SystemTime.wHour , SystemTime.wMinute , SystemTime.wSecond );
	FString color = myWord?(MMOChatManager::getSingleton().FriendChatMyContentColorString):(MMOChatManager::getSingleton().FriendChatOtherPlayerContentColorString);

	return FString::Printf( TEXT("<FONT Color='%s'>%s</FONT><br><FONT Color='%s'>%s</FONT>") , *MMOChatManager::getSingleton().FriendChatSenderColorString , *sender ,
		*color , *content);
}
TArray<ContactLogStruct>& FriendChatManager::GetLogList()
{
	return ContactLogList;
}
TArray<ContactLogStruct>& FriendChatManager::GetReadLog()
{
	return ReadContactLogList;
}
TArray<ContactLogStruct>& FriendChatManager::GetUnReadLog()
{
	return UnReadContactLogList;
}
INT  FriendChatManager::AddReadLog(ContactLogStruct& log)
{
	return ReadContactLogList.AddItem(log);
}

void FriendChatManager::Shutdown()
{
	SaveChatLogToFile();
}

void FriendChatManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_ChangeRole:
		{
			SaveChatLogToFile();
		}
		break;
	}
}