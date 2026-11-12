#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FriendManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Friend)

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Friend::ASFriendItemDClick(class UGFxObject* mc)
{
	if( mc )
	{
		INT index = mc->GetInt(TEXT("itemid"));
		INT groupId = mc->GetInt(TEXT("itemdataid"));
		
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if( player )
		{
			MMOFriendMember* member = FriendManager::getSingleton().GetFriendGroupMember( groupId , index );
			if( member )
			{
				MMOUI::GetUIManager()->delegateSelectFriendChat( member->RoleId , member->ActorName );
			}
		}
	}
}

void UMMOGFXPlayer_Friend::ASClickSearchItem( INT pos )
{
	if( Friend )
	{		
		MMOFriendMember* member = FriendManager::getSingleton().GetSearchFriend(pos);

		if(member)
		{
			MMOUI::GetUIManager()->delegateSelectFriendChat( member->RoleId , member->ActorName );
		}
	}
}

void UMMOGFXPlayer_Friend::PostAdvance(FLOAT dt)
{
	if(Friend)
	{
		Friend->UpdateTime(dt);
	}
}

void UMMOGFXPlayer_Friend::ASFriendOverItem(INT groupId,INT index)
{
	if(Friend)
	{
		Friend->FriendOverItem( groupId, index);
	}
}

void UMMOGFXPlayer_Friend::ASFriendAddFriend()
{
	MMOMsgBoxManager::getSingleton().AddModalInputMsgboxEvent( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.AddAttention") ) , 0 , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.AddAttention")) , GameEvent_FriendAdd );
}

void UMMOGFXPlayer_Friend::ASNearContactDClick(class UGFxObject* mc)
{
	if( mc )
	{
		INT index = mc->GetInt(TEXT("itemid"));
		INT groupId = mc->GetInt(TEXT("itemdataid"));

		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if( player )
		{
			MMOFriendMember* member = FriendManager::getSingleton().GetNearContact( index );
			if( member )
			{
				MMOUI::GetUIManager()->delegateSelectFriendChat( member->RoleId , member->ActorName );
			}
		}
	}
}

//
void UMMOGFXPlayer_Friend::ASFriendChangeSearchText(const FString& SearchText)
{
	if(Friend)
	{
		Friend->FriendChangeSearchText(SearchText);
	}
}

void UMMOGFXPlayer_Friend::ASFriendSearchFocusChange(UBOOL in,const FString& searchText)
{
	if(Friend)
	{
		Friend->SearchFocusChange( in , searchText );
	}
}

//---------------------------------click friend message reminder---------------------------------------------------------------------------------------
void UMMOGFXPlayer_Friend::ASFriendChatList(INT Index)
{
	if( ChatHint )
	{
		ChatHint->ClickFriendChatHint(Index);
	}
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Friend::ASFriendItemRightClick(INT Posx,INT Posy,class UGFxObject* Item,INT ItemId,const FString& NameText)
{
	INT GroupId;
		
	GroupId = Item->GetInt( "itemdataid");
	if( GroupId == AttentionGroupId )
	{
		MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_Attention , Posx , Posy , GroupId , NameText, ItemId );
	}
	else 
	{
		MMOFriendMember* mem = NULL;
		if( GroupId < AttentionGroupId )
		{
			mem = FriendManager::getSingleton().GetFriendGroupMember(GroupId,ItemId);
			if(mem)
			{
				MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_Friend , Posx , Posy , mem->RoleId , NameText, ItemId );
			}
		}
		else 
		{
			if( GroupId == BlackGroupId )
			{
				mem = FriendManager::getSingleton().GetBlacklistMember(ItemId);
			}
			else if( GroupId == EnemyGroupId  )
			{
				mem = FriendManager::getSingleton().GetEnemylistMember(ItemId);
			}
			else if( GroupId == NearGroupId )
			{
				mem = FriendManager::getSingleton().GetNearContact(ItemId);
			}

			//
			if( mem != NULL )
			{
				MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_Contact , Posx , Posy , mem->RoleId , NameText, ItemId );
			}
		}

		
	}
}


//------------------------------------------------------------------------------------------------------------------------
	/*
	Reorganize friend chat
	*/

// void UMMOGFXPlayer_Friend::ASSendFriendChat( INT WindowId , const FString& ChatMsg)
// {
//  	if( ContactLevel )
//  	{
//  		ContactLevel->SendFriendChatMessage( WindowId , ChatMsg );
//  	}
// }
// 
// //------------------------------------------------------------------------------------------------------------------------
// void UMMOGFXPlayer_Friend::ASContactWindowTab( UGFxObject* Window , class UGFxObject* Tab)
// {
// 	if( ContactLevel && Tab && Window)
// 	{
// 		INT TabId = Tab->GetInt(TEXT("itemdataid"));
// 		INT roelId = Tab->GetInt(TEXT("roleId"));
// 
// 		ContactLevel->ChangeToTab(roelId);
// 		ContactLevel->ContactWindowTab(Window,TabId);
// 	}
// }
// 
// //------------------------------------------------------------------------------------------------------------------------
// void UMMOGFXPlayer_Friend::ASContactWindowTabClose(class UGFxObject* Window,class UGFxObject* Tab)
// {
// 	if( ContactLevel && Window && Tab )
// 	{
// 		ContactLevel->WindowTabClose( Window , Tab );
// 	}
// }

void UMMOGFXPlayer_Friend::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_Friend::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Friend);
}

void UMMOGFXPlayer_Friend::ASFriendChangeStatus(INT Status)
{
	if(Friend)
	{
		Friend->ChangeMyInfo( Friend->MySign , Status );
	}
}

void UMMOGFXPlayer_Friend::ASFriendChangeSignature(const FString& Sign)
{
	if(Friend)
	{
		Friend->ChangeMyInfo( Sign , Friend->MyStatus );
	}	
}
void UMMOGFXPlayer_Friend::ASSelectRoleChatWnd(INT nRoleID)
{
	if(ContactLevel)
		ContactLevel->SetRoleChatContent(nRoleID);
}
void UMMOGFXPlayer_Friend::ASCloseRoleChatWnd(INT nRoleID)
{
	if(ContactLevel)
		ContactLevel->CloseRoleChat(nRoleID);
}
void UMMOGFXPlayer_Friend::ASSendFriendChatByRoleID(INT nRoleID,const FString& ChatMsg)
{
	if(ContactLevel)
		ContactLevel->SendFriendChat(nRoleID,ChatMsg);
}