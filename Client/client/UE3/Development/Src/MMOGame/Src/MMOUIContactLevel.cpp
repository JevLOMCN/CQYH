#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FriendChatManager.h"
#include "FriendManager.h"
#include "MMOChatManager.h"
#include "ScaleformEngine.h"

IMPLEMENT_CLASS(UMMOUIContactLevel)

//----------------------------
/*
//reorganize friend chat
*/

/*
DispatchChatContent

Three cases:
	1 the other party is in own chat list

	2 the other party is in own chat list and currently chatting

	3 the other party is not in own chat list
*/

void UMMOUIContactLevel::DispatchChatContent(INT nRoleID,FString& strContent)
{
	MMOFriendMember FriendInfo;

	if( Player && Player->ServerID == nRoleID) 
	{
		FriendInfo.RoleId = nRoleID;
		FriendInfo.ActorName = Player->ActorName;
	}
	else
	{
		FriendInfo = FriendManager::getSingleton().GetFriendInfoByRoleId(nRoleID);
	}

	FriendManager::getSingleton().UpdateFriendMemberInfo(FriendInfo,nRoleID);

	if( FriendInfo.RoleId != nRoleID ) 
	{
		return;
	}

	FriendChatManager::getSingleton().AddNewChatLogList(nRoleID,FriendInfo.ActorName,strContent,Player->ServerID == nRoleID);

	if(GetVisible()&&FriendChatManager::getSingleton().IsInRoleChatList(nRoleID))
	{
		FriendChatManager::getSingleton().AddNewChatReadLog(nRoleID,FriendInfo.ActorName,strContent,Player->ServerID == nRoleID);
		if(nRoleID == nCurRoleID)
		{
			SetRoleChatContent(nRoleID);
		}
		else
		{
			INT nIndex = FriendChatManager::getSingleton().GetRoleIndexByChatList(nRoleID);
			if (nIndex >= 0)
			{
				//tab flashing
				eventCallUCShowRoleItemEffect(nIndex);
			}
			
		}
	}
	else
	{
		FriendChatManager::getSingleton().AddNewChatUnReadLog(nRoleID,FriendInfo.ActorName,strContent,Player->ServerID != nRoleID);
		MMOFacade::getSingleton().NotifyUnReadFriendChatMsgChange( );
	}
}
void UMMOUIContactLevel::SendFriendChat(int nRoleID, FString strContent)
{
	if(nRoleID <= 0)
		return;

	MMOFriendMember FriendInfo = FriendManager::getSingleton().GetFriendInfoByRoleId( nRoleID );

	FriendManager::getSingleton().UpdateFriendMemberInfo(FriendInfo,nRoleID);
	
	if( FriendInfo.RoleId != nRoleID) 
	{
		return; // invalid
	}
	

	MMOUI::ReplaceCensoredWord(strContent,CensoredWord_Chat);

	CMessageHandler::GetInstance().SendFriendChat(nRoleID,strContent);

	FriendChatManager::getSingleton().AddNewChatLogList(nRoleID,FriendInfo.ActorName,strContent,true);
	FriendChatManager::getSingleton().AddNewChatReadLog(nRoleID,FriendInfo.ActorName,strContent,true);

	if(nRoleID == nCurRoleID)
		SetRoleChatContent(nRoleID);
}
void  UMMOUIContactLevel::SetRoleChatContent(INT nRoleID)
{
	nCurRoleID = nRoleID;

	UGFxObject* ShowArea = GetObject( TEXT("TextShowArea_mc") );
	if( !ShowArea)
		return;
	if(nRoleID == 0)
	{
		ShowArea->SetText(TEXT(""));
		return;
	}
	
	MMOFriendMember FriendInfo = FriendManager::getSingleton().GetFriendInfoByRoleId( nRoleID );

	//or from stall
	FriendManager::getSingleton().UpdateFriendMemberInfo(FriendInfo,nRoleID);

	FString Msg = TEXT("");
	const TArray<ContactLogStruct>& showList = FriendChatManager::getSingleton().GetLogList();

	for( INT i = 0 ; i < showList.Num() ; i++ )
	{		
		if( showList(i).PlayerName == FriendInfo.ActorName)
		{
			INT listNum = Min( FriendChatManager::ShowMaxLog , showList(i).LogList.Num() );
			for( INT j = 0 ; j < listNum ; j++ )
			{
				Msg += FString::Printf( TEXT("<FONT COLOR='%s'>%s</FONT><br>") , *MMOUI::GetUIManager()->TotalChannelFontColorString , *showList(i).LogList(j) );
			}
			break;
		}		
	}

	TArray<struct FASValue> Arguments;
	FASValue ASValue(EC_EventParm);
	ASValue.S =  Msg;
	ASValue.Type = AS_String;

	Arguments.AddItem(ASValue);
	ShowArea->Invoke( TEXT("NewAppendHtml") , Arguments );
}
void  UMMOUIContactLevel::CloseRoleChat(int nRoleID)
{
	FriendChatManager::getSingleton().RemoveRoleIDChatList(nRoleID);
	if(nRoleID == nCurRoleID)
	{
		nCurRoleID = 0;
		SetRoleChatContent(0);
	}
	UpdateRoleChatList();

	const TArray<int>& curChatList = FriendChatManager::getSingleton().GetChatList();
	if(curChatList.Num() == 0)
	{
		MMOGameEvent_Any evt( GameEvent_HideFriendChatHint );
		MMOUI::PostGameEvent(&evt);
	}
}
void UMMOUIContactLevel::UpdateRoleChatList()
{
	const TArray<int>& curChatList = FriendChatManager::getSingleton().GetChatList();
	if(curChatList.Num() == 0)
	{
		nCurRoleID = 0;
		Show(FALSE);
	}

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == NULL)
		return;
	for (int n = 0; n < curChatList.Num(); ++n)
	{
		UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Friend.FriendNameListItemData"));
		if( itemObj != NULL )
		{
			MMOFriendMember member = FriendManager::getSingleton().GetFriendInfoByRoleId( curChatList(n) );
			FriendManager::getSingleton().UpdateFriendMemberInfo(member,curChatList(n));
			if(member.RoleId != 0)
			{
				//gender
				itemObj->SetInt(TEXT("RoleID"), member.RoleId );
				//class
				itemObj->SetString(TEXT("RoleName"), member.ActorName);
				// character ID
				itemObj->SetInt(_T("OnLine"), member.Online);

				if(nCurRoleID > 0&& nCurRoleID == member.RoleId )
				{
					itemObj->SetBool(_T("Selected"), TRUE);
				}
				else if(nCurRoleID == 0&& n==0)
					itemObj->SetBool(_T("Selected"), TRUE);
			}
			dataProvider->SetElementObject( n, itemObj );
		}
	}
	eventCallUCDataFriendChatList(dataProvider);
}

void UMMOUIContactLevel::SelectNewRoleChat(INT RoleId,const FString& PlayerName)
{
	/** remove friend restriction
	/*if( !FriendManager::getSingleton().IsMyFriend(RoleId))
		return;*/

	if(RoleId <= 0)
		return;
	FriendChatManager::getSingleton().RemoveUnReadChatContent(RoleId);

	if(RoleId == nCurRoleID)
	{
		if(!GetVisible())
			Show(TRUE);
		return;
	}
	nCurRoleID = RoleId;

	FriendChatManager::getSingleton().AddNewRoleIDChatList(RoleId);
	UpdateRoleChatList();
	if(!GetVisible())
		Show(TRUE);
}

void UMMOUIContactLevel::UnInit()
{
	Super::UnInit();
	FScriptDelegate nullDelgate(EC_EventParm);


	MMOUI::GetUIManager()->SetSelectFriendChatDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetMoveUnReadMsgToReadDelegate(nullDelgate);
}

void UMMOUIContactLevel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch( evt->GetEventType() )
	{
	case GameEvent_FriendInfoChange:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
			if(msg)
			{
				UpdateRoleChatList();
			}
		}
		break;

	case GameEvent_ChangeRole:
		{
			FriendChatManager::getSingleton().ClearRoleIDChatList();
		}
		break;
	}

}
void UMMOUIContactLevel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_FriendInfoChange);
	evts.AddItem(GameEvent_ChangeRole);
	EventAdapter = new TGameEventAdapter<UMMOUIContactLevel>(this, evts);
}
void UMMOUIContactLevel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	INT faceNum = 0;
	GConfig->GetInt( TEXT("Chat"), TEXT("FaceNum") , faceNum , GGameIni );
	UGFxObject* popFace = GetObject( TEXT("PopFacePanel") );
	if( popFace )
	{
		for ( INT i = 0 ; i < faceNum ; i++ )
		{
			UGFxObject* face = popFace->GetObject( FString( TEXT("FaceButton") ) + appItoa(i) );
			if(face)
			{
				face->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Chat.face") ) + appItoa(i) ) );
			}
		}

	}

	if(FriendChatManager::getSingleton().GetChatList().Num() == 0)
		nCurRoleID = 0;

	if(v)
	{
		UpdateRoleChatList();
	}
}
void UMMOUIContactLevel::OpenUnReadChatContent(INT RoleId,const FString& PlayerName)
{
	FriendChatManager::getSingleton().RemoveUnReadChatContent(RoleId);
	MMOFacade::getSingleton().NotifyUnReadFriendChatMsgChange( );

	if(!GetVisible())
		Show(TRUE);
	nCurRoleID = RoleId;

	FriendChatManager::getSingleton().AddNewRoleIDChatList(RoleId);
	UpdateRoleChatList();
}
void UMMOUIContactLevel::FriendChatMsg(INT RoleId,const FString& Content)
{

	FString strCC = Content;
	MMOUI::ReplaceCensoredWord(strCC, CensoredWord_ChatRecive);
	DispatchChatContent(RoleId,strCC);
}
// reorganize friend chat

//-------------------------one of the 2 core functions of this class, messages others send you are dispatched through this; the other is clicking friend chat info, see SelectFriendChat--------------------------------------------------
// void UMMOUIContactLevel::FriendChatMsg(INT RoleId,const FString& Content)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	MMOFriendMember FriendInfo;
// 	
// 	if( Player && Player->ServerID == RoleId) 
// 	{
// 		//case of sending to oneself
// 		FriendInfo.RoleId = RoleId;
// 		FriendInfo.ActorName = Player->ActorName;
// 	}
// 	else
// 	{
// 		FriendInfo = FriendManager::getSingleton().GetFriendInfoByRoleId(RoleId);
// 	}
// 	
// 	if( FriendInfo.RoleId != RoleId ) 
// 	{
// 		return; // invalid
// 	}
// 
// 	FString FriendName = FriendInfo.ActorName;
// 	AddtoContactLog( RoleId , FriendName , Content ); //add to Log regardless of whether there is a window
// 
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 		{
// 			if( ContactWindowList(i).RoleId[j] == RoleId )
// 			{
// 				//there is a created window with the player who sent the message
// 
// 				AddtoReadContactLog( RoleId , FriendName , Content );
// 
// 				if( ContactWindowList(i).ActiveId == j )
// 				{				
// 					//the current window is the active one
// 					AddContactMessage( ContactWindowList(i).Widget , FriendName  );
// 				}
// 				else
// 				{
// 					//if not active, give an activation prompt
// 					SetChildTabHint( ContactWindowList(i).Widget , j , TRUE );
// 				}
// 
// 				return;
// 			}
// 		}
// 	}	
// 
// 	//no existing window, add to unread list
// 	AddtoUnReadContactLog( RoleId , FriendInfo.ActorName , Content );
// 
// 	MMOFacade::getSingleton().NotifyUnReadFriendChatMsgChange( );
// }
// 
// void UMMOUIContactLevel::UnInit()
// {
// 	Super::UnInit();
// 	FScriptDelegate nullDelgate(EC_EventParm);
// 
// 
// 	MMOUI::GetUIManager()->SetSelectFriendChatDelegate(nullDelgate);
// 	MMOUI::GetUIManager()->SetMoveUnReadMsgToReadDelegate(nullDelgate);
// }
// 
// void UMMOUIContactLevel::OnNativeGameEvent(struct MMOGameEvent* evt)
// {
// 	switch( evt->GetEventType() )
// 	{
// 	case GameEvent_FriendInfoChange:
// 		{
// 			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
// 			if(msg)
// 			{
// 				INT windowId = GetWindowIdByRoleId(msg->IntData);
// 				UpdateWindowTab( windowId );
// 			}
// 		}
// 		break;
// 	}
// 
// }
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::NativeInit()
// {
// 	FriendChatMgr = &FriendChatManager::getSingleton();
// 
// 	SetVisible(TRUE);
// }
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::ClearUI()
// {
// 	//reorganize friend chat
// 	return;
// 
// 	FriendChatMgr = &FriendChatManager::getSingleton();
// 	if(!FriendChatMgr)
// 	{
// 		return;
// 	}
// 	//clear all windows
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		FASValue AsVal(EC_EventParm);	
// 		AsVal.S = ContactWindowList(i).WindowName;
// 		AsVal.Type = AS_String;
// 
// 		TArray<struct FASValue> Arguments;
// 		Arguments.AddItem(AsVal);
// 
// 		Invoke( TEXT("RemoveWindow") , Arguments );
// 	}
// 	ContactWindowList.Empty();
// }
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::AddtoReadContactLog(INT RoleId,const FString& PlayerName,const FString& Content,UBOOL fromSelf)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	ContactLogStruct CL(EC_EventParm);
// 	
// 	FString senderName;
// 	if(fromSelf)
// 	{
// 		senderName = Player->ActorName;
// 	}
// 	else
// 	{
// 		senderName = PlayerName;
// 	}
// 		
// 	FString Msg = _GetShowText( senderName , Content , fromSelf );
// 
// 	for(INT i = 0 ; i< FriendChatMgr->ReadContactLogList.Num() ;i++ ) 
// 	{
// 		if( FriendChatMgr->ReadContactLogList(i).RoleId == RoleId )
// 		{
// 			FriendChatMgr->ReadContactLogList(i).LogList.AddItem( Msg );
// 			return;
// 		}
// 	}
// 	CL.RoleId = RoleId;
// 	CL.PlayerName = PlayerName;
// 	CL.LogList.AddItem( Msg );
// 	FriendChatMgr->ReadContactLogList.AddItem(CL);
// }
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::AddtoUnReadContactLog(INT RoleId,const FString& PlayerName,const FString& Content,UBOOL addLog )
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if(!FriendChatMgr)
// 	{
// 		return;
// 	}
// 	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
// 	if(!player)
// 	{
// 		return;
// 	}
// 
// 	ContactLogStruct CL(EC_EventParm);
// 		
// 	FString Msg = _GetShowText( PlayerName , Content , player->ServerID == RoleId ); 
// 
// 	for(INT i = 0 ; i< FriendChatMgr->UnReadContactLogList.Num() ;i++ ) 
// 	{
// 		if( FriendChatMgr->UnReadContactLogList(i).RoleId == RoleId )
// 		{
// 			if(addLog)
// 			{
// 				FriendChatMgr->UnReadContactLogList(i).LogList.AddItem( Msg );
// 			}
// 			return;
// 		}
// 	}
// 	CL.RoleId = RoleId;
// 	CL.PlayerName = PlayerName;
// 	if(addLog)
// 	{
// 		CL.LogList.AddItem( Msg );
// 	}
// 	FriendChatMgr->UnReadContactLogList.AddItem(CL);
// }

//------------------------------------add to dialog box-------------------------------------------------
// void UMMOUIContactLevel::AddContactMessage( UGFxObject* Window , const FString& PlayerName /*,const FString& Content*/)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if( !Window )
// 	{
// 		return;
// 	}
// 	UGFxObject* ShowArea = Window->GetObject( TEXT("TextShowArea_mc") );
// 	if( !ShowArea)
// 	{
// 		return;
// 	}
// 	
// 
// 	FString Msg;
// 
// 	//originally used ReadContactLogList, later changed to log
// 	TArray<ContactLogStruct>& showList = FriendChatMgr->ContactLogList;
// 	for( INT i = 0 ; i < showList.Num() ; i++ )
// 	{		
// 		if( showList(i).PlayerName == PlayerName)
// 		{
// 			INT listNum = Min( ShowMaxLog , showList(i).LogList.Num() );
// 			for( INT j = 0 ; j < listNum ; j++ )
// 			{
// 				Msg += FString::Printf( TEXT("<FONT COLOR='%s'> %s</FONT><br>") , *MMOUI::GetUIManager()->TotalChannelFontColorString , *showList(i).LogList(j) );
// 			}
// 
// 			break;
// 		}		
// 	}
// 
// 	TArray<struct FASValue> Arguments;
// 	FASValue ASValue(EC_EventParm);
// 
// 	ASValue.S =  Msg;
// 	ASValue.Type = AS_String;
// 
// 	Arguments.AddItem(ASValue);
// 	ShowArea->Invoke( TEXT("NewAppendHtml") , Arguments );
// 
// }
// 
// 
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::ContactWindowTab(UGFxObject* Window,INT Tab)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if( Tab >= WindowChatNum || !Window )
// 	{
// 		return;//invalid
// 	}
// 
// 	INT WindowId = Window->GetInt(TEXT("WindowId"));
// 	for (INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		if( ContactWindowList(i).WindowId == WindowId )
// 		{
// 			AddContactMessage( Window , ContactWindowList(i).PlayerName[Tab] );
// 			break;
// 		}
// 	}
// 	
// }
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::CreateNewWindow(const FString& PlayerName , INT RoleId )
// {
// 	//reorganize friend chat
// 	return;
// 
// 		FASValue AsVal(EC_EventParm);
// 		FString WindowName = FString::Printf( TEXT("ContactWidget%d") , CurWindowId );
// 		AsVal.S =  WindowName;
// 		AsVal.Type = AS_String;
// 		
// 		TArray<struct FASValue> Arguments;
// 		Arguments.AddItem(AsVal);
// 
// 		Invoke( TEXT("AddWindow") , Arguments );
// 
// 		UGFxObject* mc = GetObject( WindowName );
// 		
// 		if( !mc )
// 		{
// 			return;
// 		}
// 
// 		mc->SetInt( TEXT("WindowId"),CurWindowId );
// 
// 		FContactWindowStruct CW(EC_EventParm);
// 		CW.Widget = mc;
// 		CW.ActiveId = 0;
// 		CW.PlayerName[0] = PlayerName;
// 		CW.WindowName = WindowName;
// 		CW.WindowId = CurWindowId;
// 		CW.RoleId[0] = RoleId;
// 
// 		ContactWindowList.AddItem( CW);
// 
// 		//ActiveContactPlayer ( mc, 0 ,CW.WindowName );
// 
// 		UGFxObject* Tab = NULL;
// 		
// 		SetChildWindow(mc,1,FALSE,FALSE);
// 		SetChildWindow(mc,2,FALSE,FALSE);
// 		SetChildWindow(mc,3,FALSE,FALSE);
// 		SetChildWindow(mc,0,TRUE,FALSE,PlayerName,RoleId);
// 
// 		ActiveChildWindow( mc , 0 );
// 
// 		SetNewWindowText( mc );
// 
// 		CurWindowId++;
// }
// 
// //--------------------------
// void UMMOUIContactLevel::SetNewWindowText( UGFxObject* window )
// {
// 	//reorganize friend chat
// 	return;
// 
// 	INT faceNum = 0;
// 	GConfig->GetInt( TEXT("Chat"), TEXT("FaceNum") , faceNum , GGameIni );
// 	UGFxObject* popFace = window->GetObject( TEXT("PopFacePanel") );
// 	if( popFace )
// 	{
// 		for ( INT i = 0 ; i < faceNum ; i++ )
// 		{
// 			UGFxObject* face = popFace->GetObject( FString( TEXT("FaceButton") ) + appItoa(i) );
// 			if(face)
// 			{
// 				face->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Chat.face") ) + appItoa(i) ) );
// 			}
// 		}
// 
// 	}
// }
// 
// //-----------------------------------------move those in the not-shown list out------------------------
// void UMMOUIContactLevel::MoveUnReadMsgToRead(INT RoleId,const FString& PlayerName)
// {
// 
// 	//reorganize friend chat
// 	return;
// 
// 	if(!FriendChatMgr)
// 	{
// 		return;
// 	}
// 
// 	INT ReadIndex = -1;
// 	INT UnReadIndex = -1;
// 
// 	for ( INT i = 0 ; i < FriendChatMgr->ReadContactLogList.Num() ; i++ )
// 	{
// 		if( FriendChatMgr->ReadContactLogList(i).RoleId == RoleId )
// 		{
// 			ReadIndex = i;
// 		}
// 	}
// 	for ( INT j = 0 ; j < FriendChatMgr->UnReadContactLogList.Num() ; j++ )
// 	{
// 		if( FriendChatMgr->UnReadContactLogList(j).RoleId == RoleId )
// 		{
// 			UnReadIndex = j;
// 		}
// 	}
// 	if( UnReadIndex < 0 )
// 	{
// 		//no unread
// 		return;
// 	}
// 	if( ReadIndex < 0 ) //this person doesn't exist, create a new one
// 	{
// 		ContactLogStruct NewContackLog(EC_EventParm);
// 		NewContackLog.RoleId = RoleId;
// 		NewContackLog.PlayerName = PlayerName;
// 		ReadIndex = FriendChatMgr->ReadContactLogList.AddItem(NewContackLog);		
// 	}
// 	//copy data
// 	for ( INT k = 0 ; k < FriendChatMgr->UnReadContactLogList(UnReadIndex).LogList.Num() ; k++ )
// 	{
// 		FriendChatMgr->ReadContactLogList( ReadIndex ).LogList.AddItem( FriendChatMgr->UnReadContactLogList(UnReadIndex).LogList(k) );
// 	}
// 	//delete the original
// 	FriendChatMgr->UnReadContactLogList(UnReadIndex).LogList.Empty();
// 	FriendChatMgr->UnReadContactLogList.Remove(UnReadIndex);
// 	return;
// 
// }
// 
// void UMMOUIContactLevel::Init()
// {
// 	Super::Init();
// 
// 	check(EventAdapter == NULL);
// 	TArray<EGameEvent> evts;
// 	evts.AddItem(GameEvent_FriendInfoChange);
// 	EventAdapter = new TGameEventAdapter<UMMOUIContactLevel>(this, evts);
// }
// 
// INT UMMOUIContactLevel::GetWindowIdByRoleId( INT roleId )
// {
// 	//reorganize friend chat
// 	return;
// 
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 		{
// 			if( ContactWindowList(i).RoleId[j] == roleId ) 
// 			{
// 				
// 				return ContactWindowList(i).WindowId;
// 			}
// 		}		
// 	}
// 
// 	return -1;
// }
// 
// //-------------------------------------------click friend's chat, one of the 2 core functions of this class, the other is dispatching messages FriendChatMsg-------------------
// void UMMOUIContactLevel::SelectFriendChat(INT RoleId,const FString& RoleName)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if( !FriendManager::getSingleton().IsMyFriend(RoleId))
// 	{
// 		return;
// 	}
// 
// 	//if there are unread ones, move unread info to read
// 	MoveUnReadMsgToRead( RoleId , RoleName );
// 
// 	//first see whether this dialog window is open
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 		{
// 			if( ContactWindowList(i).RoleId[j] == RoleId ) 
// 			{
// 				//this window exists, activate it
// 				ContactWindowList(i).ActiveId = j ;
// 				ActiveChildWindow( ContactWindowList(i).Widget , j );
// 				return;
// 			}
// 		}		
// 	}
// 
// 	//no existing window, look for a free one among the big windows to open
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 		{
// 			if( ContactWindowList(i).RoleId[j] == 0 ) //if a window is free, show it, then activate
// 			{
// 				
// 				ContactWindowList(i).RoleId[j] = RoleId;
// 				ContactWindowList(i).PlayerName[j] = RoleName;
// 				UpdateWindowTab( ContactWindowList(i).WindowId );
// 
// 				ActiveChildWindow( ContactWindowList(i).Widget , j );
// 				return;
// 			}
// 		}		
// 	}
// 
// 	//none free, create a new big window
// 	CreateNewWindow( RoleName , RoleId );
// 
// 	
// 	
// }
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::ActiveChildWindow(class UGFxObject* Window,INT ChildId)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if( Window )
// 	{
// 		SetChildTabHint( Window , ChildId , FALSE );
// 		//todo highlight
// 		ContactWindowTab( Window , ChildId );
// 
// 		if(GFXPlayer)
// 			FGFxEngine::GetEngine()->SetFocusMovie(GFXPlayer->pMovie);
// 
// 		TArray<	FASValue> args;
// 		Window->Invoke( TEXT("setFocus"),args );
// 
// 		FASValue AsVal(EC_EventParm);	
// 		AsVal.I = ChildId;
// 		AsVal.Type = AS_Int;
// 
// 		TArray<struct FASValue> Arguments;
// 		Arguments.AddItem(AsVal);
// 
// 		Window->Invoke( TEXT("AutoClickTabIndex") , Arguments );
// 
// 
// 		AMMOPlayerController* Ctrl = MMOFacade::getSingleton().GetMainController();
// 		if(Ctrl)
// 			Ctrl->SetInTextInput(TRUE);
// 	}
// 	
// }
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::AddtoContactLog(INT RoleId,const FString& PlayerName,const FString& Content,UBOOL fromSelf)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if(!FriendChatMgr)
// 	{
// 		return;
// 	}
// 
// 	//note: both contacting the other party and the other party contacting you are added here; roleid and name are always the other party's
// 
// 	//tell the manager the latest contact
// 	FriendManager::getSingleton().NotifyNewContact(RoleId);
// 	
// 	ContactLogStruct CL(EC_EventParm);
// 
// 	
// 	FString senderName;
// 	if(fromSelf)
// 	{
// 		senderName = Player->ActorName;
// 	}
// 	else
// 	{
// 		senderName = PlayerName;
// 	}
// 
// 	FString Msg = _GetShowText( senderName , Content , fromSelf );
// 
// 	//already have a message queue for this person, add to the previous queue
// 	for(INT i = 0 ; i< FriendChatMgr->ContactLogList.Num() ;i++ ) 
// 	{
// 		if( FriendChatMgr->ContactLogList(i).RoleId == RoleId )
// 		{
// 			FriendChatMgr->ContactLogList(i).LogList.AddItem( Msg );
// 			if( FriendChatMgr->ContactLogList(i).LogList.Num() > MaxFriendChatLogNum )
// 			{
// 				FriendChatMgr->ContactLogList(i).LogList.Remove(0);
// 			}
// 			return;
// 		}
// 	}
// 	//otherwise create new
// 	CL.RoleId = RoleId;
// 	CL.PlayerName = PlayerName;
// 	CL.LogList.AddItem( Msg );
// 	FriendChatMgr->ContactLogList.AddItem(CL);
// }
// 
// 
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::SendFriendChatMessage(INT WindowId,const FString& MsgContent)
// {
// 	//reorganize friend chat
// 	return;
// 	
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		if( ContactWindowList(i).WindowId == WindowId )
// 		{
// 			INT TargetId = ContactWindowList(i).RoleId[ ContactWindowList(i).ActiveId ];
// 			CMessageHandler::GetInstance().SendFriendChat(TargetId,MsgContent);
// 
// 			//after speaking, add the words to own box
// 			MMOFriendMember FriendInfo = FriendManager::getSingleton().GetFriendInfoByRoleId( TargetId );
// 		
// 			if( FriendInfo.RoleId != TargetId ) 
// 			{
// 				return; // illegal
// 			}
// 			FString FriendName = FriendInfo.ActorName;
// 			AddtoContactLog( TargetId , FriendName , MsgContent , true );
// 			AddtoReadContactLog( TargetId , FriendName , MsgContent ,true);
// 			AddContactMessage( ContactWindowList(i).Widget , FriendName  );
// 
// 			return;
// 		}
// 	}
// }
// 
// //---------------------------Set window reminder---------------
// void UMMOUIContactLevel::SetChildTabHint( UGFxObject* Window , INT ChildId , UBOOL openHint )
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if( !Window )
// 	{
// 		return;
// 	}
// 
// 	UGFxObject* Tab = Window->GetObject( FString::Printf(TEXT("tab%d"),ChildId) );
// 	if( !Tab )
// 	{
// 		return;
// 	}
// 
// 	UGFxObject* hint = Tab->GetObject( TEXT("alertAnimation") );
// 	if(hint)
// 	{
// 		if( openHint )
// 		{
// 			hint->GotoAndPlayI(1);
// 		}
// 		else
// 		{
// 			hint->GotoAndStopI(1);
// 		}
// 	}
// }
// void UMMOUIContactLevel::ChangeToTab(INT nRoleID)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 		{
// 			if( ContactWindowList(i).RoleId[j] == nRoleID ) 
// 			{
// 				//this window exists, activate it
// 				ContactWindowList(i).ActiveId = j ;
// 				ActiveChildWindow( ContactWindowList(i).Widget , j );
// 				return;
// 			}
// 		}		
// 	}
// }
// //---------------------------Set child window----------------------------------------------------------
// void UMMOUIContactLevel::SetChildWindow(UGFxObject* Window , INT ChildId , UBOOL Visible , UBOOL CloseVisible , FString PlayerName , INT roleId )
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if( !Window )
// 	{
// 		return;
// 	}
// 	UGFxObject* Tab = Window->GetObject( FString::Printf(TEXT("tab%d"),ChildId) );
// 	UGFxObject* Close = Window->GetObject( FString::Printf(TEXT("tabclose%d"),ChildId) );
// 	if( Close )
// 	{
// 		Close->SetVisible(CloseVisible);
// 	}
// 	if( !Tab )
// 	{
// 		return;
// 	}
// 	if( !Visible )
// 	{
// 		Tab->SetVisible(FALSE);
// 	}
// 	else
// 	{
// 		Tab->SetVisible(TRUE);
// 		Tab->SetInt("tabId",ChildId);
// 		Tab->SetInt("roleId",roleId);
// 
// 		UGFxObject* TF_TabName = Tab->GetObject( TEXT("tF_playerName") );
// 		if( TF_TabName )
// 		{
// 			TF_TabName->SetText( PlayerName );
// 		}
// 
// 		UGFxObject* OnlineStatClip = Tab->GetObject( TEXT("ContactStatus") );
// 		if( OnlineStatClip )
// 		{
// 			INT stat = 0;
// 			MMOFriendMember member = FriendManager::getSingleton().GetFriendInfoByRoleId( roleId );
// 			if( member.RoleId != 0 )
// 			{
// 				stat = member.Online;				
// 			}
// 			else
// 			{
// 				stat = E_FS_OFFLINE;
// 			}
// 
// 			switch(stat)
// 			{
// 			case E_FS_OFFLINE:
// 				OnlineStatClip->GotoAndStop( TEXT("offline") );
// 				break;
// 			case E_FS_ONLINE:
// 				OnlineStatClip->GotoAndStop( TEXT("online") );
// 				break;
// 			case E_FS_BUSY:
// 				OnlineStatClip->GotoAndStop( TEXT("busy") );
// 				break;
// 			case E_FS_AFK:
// 				OnlineStatClip->GotoAndStop( TEXT("afk") );
// 				break;	
// 					
// 			}
// 
// 		}
// 	}
// 
// }
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::RemoveWindow(const FString& WindowName)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	FASValue AsVal(EC_EventParm);	
// 	AsVal.S =  WindowName;
// 	AsVal.Type = AS_String;
// 
// 	TArray<struct FASValue> Arguments;
// 	Arguments.AddItem(AsVal);
// 
// 	Invoke( TEXT("RemoveWindow") , Arguments );
// 
// 	for( INT i = 0 ; i< ContactWindowList.Num() ; i++ )
// 	{
// 		if( ContactWindowList(i).WindowName == WindowName )
// 		{
// 			//Delete ReadLog related
// 			for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 			{
// 				for ( INT k = FriendChatMgr->ReadContactLogList.Num()-1 ; k >= 0 ; k-- )
// 				{
// 					if( ContactWindowList(i).RoleId[j] != 0 && ContactWindowList(i).RoleId[j] == FriendChatMgr->ReadContactLogList(k).RoleId )
// 					{
// 						FriendChatMgr->ReadContactLogList(k).LogList.Empty();
// 						FriendChatMgr->ReadContactLogList.Remove(k);
// 					}
// 				}
// 			}
// 			//Delete window
// 			ContactWindowList.Remove(i,1);
// 		}
// 	}
// }
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::WindowTabClose(class UGFxObject* Window,class UGFxObject* Tab)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	if( !Window || !Tab )
// 	{
// 		return;
// 	}
// 
// 	INT WindowId = Window->GetInt(TEXT("WindowId"));
// 	INT TabId = Tab->GetInt(TEXT("itemid"));
// 
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		if( ContactWindowList(i).WindowId == WindowId ) // found the window
// 		{
// 			INT OpenChildWindowNum = GetWindowOpenChildNum(ContactWindowList(i).WindowId);
// 
// 			if( ContactWindowList(i).ActiveId == TabId ) //the closed one is the active displayed one
// 			{
// 				if( TabId == 0  )
// 				{
// 					if( OpenChildWindowNum > 1 )
// 					{
// 						ContactWindowTab( ContactWindowList(i).Widget , TabId+1 );
// 					}
// 				}
// 				else
// 				{
// 					ContactWindowTab( ContactWindowList(i).Widget , TabId-1 );
// 				}
// 			}
// 
// 			for ( INT j = TabId ; j < OpenChildWindowNum ; j++ )
// 			{
// 				if( j != OpenChildWindowNum-1 ) //move forward
// 				{
// 					ContactWindowList(i).RoleId[j] = ContactWindowList(i).RoleId[j+1];
// 					ContactWindowList(i).PlayerName[j] = ContactWindowList(i).PlayerName[j+1];
// 				}
// 				else//the last child window is ignored
// 				{
// 					ContactWindowList(i).RoleId[j] = 0 ;
// 					ContactWindowList(i).PlayerName[j] = TEXT("");
// 
// 				}					
// 			}
// 			if( ContactWindowList(i).ActiveId > TabId )
// 			{
// 				ContactWindowList(i).ActiveId --;
// 			}
// 			UpdateWindowTab(WindowId);
// 
// 		}
// 	}
// 
// }
// 
// //-------------------------------------------------------------------------------------
// INT UMMOUIContactLevel::GetWindowOpenChildNum(INT WindowId)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	INT Num = 0;
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		if( ContactWindowList(i).WindowId == WindowId )
// 		{
// 			for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 			{
// 				if( ContactWindowList(i).RoleId[j] != 0 )
// 				{
// 					Num++;
// 				}
// 			}
// 			break;
// 		}
// 
// 	}
// 	return Num;
// }
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::UpdateWindowTab(INT WindowId)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	for ( INT i = 0 ; i < ContactWindowList.Num() ; i++ )
// 	{
// 		if( ContactWindowList(i).WindowId == WindowId )
// 		{
// 			INT OpenNum = GetWindowOpenChildNum( WindowId );
// 
// 			for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 			{
// 				if( OpenNum == 1 && j == 0 )
// 				{ 
// 					//If there is only one, it is special and cannot be closed with X
// 					SetChildWindow( ContactWindowList(i).Widget , 0 , TRUE , FALSE , ContactWindowList(i).PlayerName[0] , ContactWindowList(i).RoleId[0] );
// 					continue;
// 				}
// 				UBOOL Visible = (ContactWindowList(i).RoleId[j] != 0) ;
// 				SetChildWindow( ContactWindowList(i).Widget , j , Visible , Visible , ContactWindowList(i).PlayerName[j] , ContactWindowList(i).RoleId[j] );				
// 			}
// 			
// 			break;
// 		}
// 	}
// }
// 
// //-------------------------------------------------------------------------------------
// void UMMOUIContactLevel::MiniMizeWindow(const FString& WindowName)
// {
// 	//reorganize friend chat
// 	return;
// 
// 	for( INT i = 0 ; i< ContactWindowList.Num() ; i++ )
// 	{	
// 
// 		if( ContactWindowList(i).WindowName == WindowName )
// 		{
// 			//Delete ReadLog related
// 			for ( INT j = 0 ; j < WindowChatNum ; j++ )
// 			{
// 				if( ContactWindowList(i).RoleId[j] != 0 )
// 				{
// 					AddtoUnReadContactLog( ContactWindowList(i).RoleId[j] , ContactWindowList(i).PlayerName[j] , TEXT("") , FALSE );
// 
// 				}
// 
// 				
// 			}
// 
// 		}
// 	}
// 	MMOFacade::getSingleton().NotifyUnReadFriendChatMsgChange( );
// 	
// 	RemoveWindow( WindowName );
// }
// 
// void UMMOUIContactLevel::PostOnShow(UBOOL v)
// {
// 	//Override, because movetop is not wanted
// }
// 
// FString UMMOUIContactLevel::_GetShowText( const FString& playerName , const FString& content , UBOOL myWord )
// {
// 	//appSystemTime( INT& Year, INT& Month, INT& DayOfWeek, INT& Day, INT& Hour, INT& Min, INT& Sec, INT& MSec );
// 	_SYSTEMTIME SystemTime;
// 	::GetLocalTime(&SystemTime);
// 	FString sender = playerName + FString::Printf( TEXT(": %02d:%02d:%02d ") , SystemTime.wHour , SystemTime.wMinute , SystemTime.wSecond );
// 	FString color = myWord?(MMOChatManager::getSingleton().FriendChatMyContentColorString):(MMOChatManager::getSingleton().FriendChatOtherPlayerContentColorString);
// 
// 	return FString::Printf( TEXT("<FONT Color='%s'>%s</FONT><br><FONT Color='%s'>%s</FONT>") , *MMOChatManager::getSingleton().FriendChatSenderColorString , *sender ,
// 		*color , *content);
// }


void UMMOUIContactLevel::ASSendFriendChatByRoleID(INT nRoleID,const FString& ChatMsg)
{
	SendFriendChat(nRoleID,ChatMsg);
}
void UMMOUIContactLevel::ASSelectRoleChatWnd(INT nRoleID)
{
	SetRoleChatContent(nRoleID);
	MMOGameEvent_FriendChat_SetChatHintImg evt( nRoleID );
	MMOUI::PostGameEvent(&evt);
}
void UMMOUIContactLevel::ASCloseRoleChatWnd(INT nRoleID)
{
	CloseRoleChat(nRoleID);
}

void UMMOUIContactLevel::ASContactWidgetMiniMize(const FString& WindowName)
{
	MMOGameEvent_FriendChat_ShowChatHint evt( nCurRoleID );
	MMOUI::PostGameEvent(&evt);
	this->SetVisible(FALSE);
}

void UMMOUIContactLevel:: ASShowFriendChatList()
{
	this->SetVisible(TRUE);
	MMOGameEvent_Any evt( GameEvent_HideFriendChatHint );
	MMOUI::PostGameEvent(&evt);
}

void UMMOUIContactLevel:: ASOnPanelClose( const FString& PanelName )
{
	
}
void UMMOUIContactLevel::ASContactWidgetClose(const FString& PanelName)
{
	MMOGameEvent_Any evt( GameEvent_HideFriendChatHint );
	MMOUI::PostGameEvent(&evt);
}