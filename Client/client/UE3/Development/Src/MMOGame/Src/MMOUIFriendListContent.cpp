#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FriendManager.h"

IMPLEMENT_CLASS(UMMOUIFriendListContent)

void UMMOUIFriendListContent::Init()
{
	//Super::Init();

	//check(EventAdapter == NULL);
	//TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_UpdateFriendGroup);
	//evts.AddItem(GameEvent_UpdateFriendContent);
	////evts.AddItem(GameEvent_PlayerBaseInfo);
	//EventAdapter = new TGameEventAdapter<UMMOUIFriendListContent>(this, evts);

}

void UMMOUIFriendListContent::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	//if(evt->GetEventType() == GameEvent_UpdateFriendGroup )
	//{
	//	MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
	//	if(msg)
	//	{
	//		UpdateFriendGroup( msg->IntData );
	//	}
	//	
	//}
	//else if( evt->GetEventType() == GameEvent_UpdateFriendContent )
	//{
	//	UpdateMyFriend();
	//}
}

void UMMOUIFriendListContent::ClearUI()
{
	for ( INT i = 0 ; i < AttentionGroupId+1 ; i++ )
	{
		UpdateFriendGroup(i);
	}
	for ( INT i = 1 ; i < AttentionGroupId ; i++ )
	{
		//Delete all groups
		eventCallDeleteGroup(i);
	}
}

//Update all friends
void UMMOUIFriendListContent::UpdateMyFriend()
{
	INT GroupNum = FriendManager::getSingleton().GetFriendGroupNum(); 
	for ( INT i = 0 ; i < GroupNum ; i++)
	{
		MMOFriendGroup* fGroup = FriendManager::getSingleton().GetFriendGroup(i);
		if(!fGroup)
		{
			continue;
		}
		if( fGroup->GroupName.IsEmpty() ) //no such group
		{
			eventCallDeleteGroup(i);		
		}
		else 
		{			
			eventCallCreateNewGroup( fGroup->GroupName , i );
			UpdateFriendGroup(i);
		}
	}
}

//Update one group of friends
void UMMOUIFriendListContent::UpdateFriendGroup( INT groupId )
{
	MMOFriendGroup* fGroup = FriendManager::getSingleton().GetFriendGroup(groupId);
	if(!fGroup)
	{
		return;
	}
	//TArray<MMOFriendMember>& nearContactList = FriendManager::getSingleton().NearContactList;
	if(!GFXPlayer)
	{
		return;
	}

	FriendManager::getSingleton().SortFriendGroup( groupId );

	INT friendNum = fGroup->FriendList.Num(); 
	//Note: this online actually means not offline
	INT onlineFriendNum = FriendManager::getSingleton().GetOnlineFriendNumInGroup(groupId); 

	UGFxObject *data = GFXPlayer->CreateObject(TEXT("Object"));
	if(data)
	{
		data->SetInt( TEXT("groupId") , groupId );
		data->SetInt( TEXT("memberNum") , friendNum );
		data->SetInt( TEXT("onlineNum") , onlineFriendNum );
		UGFxObject* memberList = GFXPlayer->CreateArray();
		if( memberList )
		{
			for ( INT i = 0 ; i < friendNum ; i++ )
			{
				UGFxObject* member = GFXPlayer->CreateObject(TEXT("Object"));
				MMOFriendMember* friendMember = FriendManager::getSingleton().GetFriendGroupMember( groupId , i );
				if( member && friendMember )
				{					
					member->SetString( TEXT("name") , friendMember->ActorName );
					member->SetInt( TEXT("lv") , friendMember->Lv );
					member->SetString( TEXT("classTxt") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( friendMember->ClassId)  ) );
					member->SetString( TEXT("location") , friendMember->Location );
					member->SetString( TEXT("sign") , friendMember->Sign );
					member->SetInt( TEXT("roleId") , friendMember->RoleId );
					member->SetInt( TEXT("stats") , friendMember->Online );
					member->SetString( TEXT("iconPath") , MMOFacade::getSingleton().GetPlayerIcon( friendMember->ClassId , friendMember->Sex ) );

					memberList->SetElementObject(i,member);
				}				
			}
			data->SetObject( TEXT("memberList") , memberList );
		}
	}

	eventCallSetFriendGroupListData( data );
}