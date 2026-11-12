#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FriendChatManager.h"
#include "FriendManager.h"

IMPLEMENT_CLASS(UMMOUIFriendChatHint)

	//---------------------------------------------------------------------------------------------------------------
	void UMMOUIFriendChatHint::UnReadFriendChatMsgChange()
{
	if( !GFXPlayer )
	{
		return;
	}

	const TArray<ContactLogStruct>& unReadLog =  FriendChatManager::getSingleton().GetUnReadLog();
	INT Num = unReadLog.Num();

	for ( INT i = 0 ; i < Num ; i++ )
	{
		BOOL HasCreate = FALSE;
		for( INT j = 0 ; j < FriendChatHintList.Num() ; j++ )
		{
			//Traversing everything may be redundant; in theory only one data entry changes. But a full traversal reduces some errors and the cost is not large.
			if( FriendChatHintList(j).RoleId == unReadLog(i).RoleId )
			{
				//Already exists, update it
				if( FriendChatHintList(j).ListItemGfxObject )
				{
					/*UGFxObject* TF_Num = FriendChatHintList(j).ListItemGfxObject->GetObject( TEXT("tF_msgNum"));
					if( TF_Num )
					{
						TF_Num->SetText( FString::Printf( TEXT("%d") , unReadLog(i).LogList.Num() ) );
					}		*/	
					eventCallUpdateItemList(unReadLog(i).RoleId,unReadLog(i).LogList.Num());
				}
				HasCreate = TRUE;
				break;
			}			
		}

		if( !HasCreate )
		{
			AddNewListItem( unReadLog(i).RoleId ,unReadLog(i).PlayerName , unReadLog(i).LogList.Num() );
		}

	}
	UpdateHintButtonInfo();

}

void UMMOUIFriendChatHint::ClearUI()
{

	UnReadFriendChatMsgChange();

}

//-------------------------------------------------------------------------------------
// void UMMOUIFriendChatHint::NativeInit()
// {
// 	FriendChatMgr = &FriendChatManager::getSingleton();
// }

//----------------------------------------------Reminder bar for unread messages from clicking a friend-----------------------------------------------------------------
void UMMOUIFriendChatHint::ClickFriendChatHint(INT Index)
{
	if( Index >= FriendChatHintList.Num() )
	{
		return;
	}

	MMOUI::GetUIManager()->delegateSelectFriendChat( FriendChatHintList(Index).RoleId , FriendChatHintList(Index).PlayerName );


	UpdateUnReadList();
	UpdateHintButtonInfo();
}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIFriendChatHint::MoveUnReadMsgToRead(INT RoleId,const FString& PlayerName)
{

	if( GFXPlayer )
	{
		MMOUI::GetUIManager()->delegateMoveUnReadMsgToRead( RoleId , PlayerName );
	}

}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIFriendChatHint::UpdateUnReadList()
{
	if( !GFXPlayer  )
	{
		return;
	}

	UBOOL bMatch = FALSE;

	const TArray<ContactLogStruct>& UnReadContactLogList =  FriendChatManager::getSingleton().GetUnReadLog();

	//Reduce the excess
	for ( INT i = FriendChatHintList.Num()-1 ; i >= 0  ; i-- )
	{
		bMatch = FALSE;

		for (INT j = 0 ; j < UnReadContactLogList.Num() ; j++ )
		{
			if( UnReadContactLogList(j).RoleId == FriendChatHintList(i).RoleId )
			{
				bMatch = TRUE;
				break;
			}
		}
		if( !bMatch )
		{
			RemoveListItem(i);
			FriendChatHintList.Remove( i );
		}
	}
	//Add the missing
	for ( INT i = 0 ; i < UnReadContactLogList.Num() ; i++ )
	{
		bMatch = FALSE;

		for (INT j = 0 ; j < FriendChatHintList.Num() ; j++ )
		{
			if( UnReadContactLogList(i).RoleId == FriendChatHintList(j).RoleId )
			{
				bMatch = TRUE;
				break;
			}
		}
		if( !bMatch )
		{
			AddNewListItem( UnReadContactLogList(i).RoleId , UnReadContactLogList(i).PlayerName , UnReadContactLogList(i).LogList.Num() );
		}
	}

}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIFriendChatHint::AddNewListItem(INT RoleId,const FString& RoleName,INT MsgNum)
{
	eventCallAddNewItem(RoleId,RoleName,MsgNum);
	FFriendChatHintStruct FriendChatHint(EC_EventParm);
	
	//UGFxObject* NewItem = eventCallAddNewItem();
	FriendChatHint.PlayerName = RoleName;//;
	FriendChatHint.RoleId = RoleId;//;

	/*if( NewItem )
	{
	UGFxObject* Tf_FriendName =  NewItem->GetObject( TEXT("tF_name"));
	if( Tf_FriendName )
	{
	Tf_FriendName->SetText(FriendChatHint.PlayerName);
	}
	UGFxObject* Tf_ChatNum =  NewItem->GetObject( TEXT("tF_msgNum"));
	if( Tf_ChatNum )
	{
	Tf_ChatNum->SetText( FString::Printf( TEXT("%d") ,  MsgNum ) );
	}
	}*/

	//FriendChatHint.ListItemGfxObject = NewItem;
	FriendChatHintList.AddItem(FriendChatHint);
}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIFriendChatHint::RemoveListItem( INT Index )
{
	if( !ListMc )
	{
		return;
	}

	/*TArray<struct FASValue> Arguments;
	FASValue Asvalue( EC_EventParm );
	Asvalue.I = Index;
	Asvalue.Type = AS_Int;
	Arguments.AddItem( Asvalue );

	ListMc->Invoke( TEXT("RemoveItem"),Arguments);*/
	eventCallRemoveItem(Index);
}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIFriendChatHint::UpdateHintButtonInfo()
{
	const TArray<ContactLogStruct>& UnReadContactLogList =  FriendChatManager::getSingleton().GetUnReadLog();
	if( HintButton && GFXPlayer  )
	{
		INT MsgNum = 0 ;
		INT ChatNum = UnReadContactLogList.Num() ;
		for( INT i = 0 ; i < ChatNum ; i++ )
		{
			MsgNum += UnReadContactLogList(i).LogList.Num();
		}
		UGFxObject* TF_MsgNum = HintButton->GetObject(TEXT("MsgNum"));
		if(TF_MsgNum)
		{
			TF_MsgNum->SetText( FString::Printf(TEXT("%d"),MsgNum));
		}
		
		UGFxObject* TF_ChatNum = HintButton->GetObject(TEXT("tF_num"));
		if(TF_ChatNum)
		{
			TF_ChatNum->SetText( FString::Printf(TEXT("%d"),ChatNum) );
		}

		if( ChatNum == 0 )
		{
			//
			HideChatHint();
			//this->SetVisible(FALSE); //Is it ok to only open the child row here?
		}
		else
		{
			ContactLogStruct friendTalk = UnReadContactLogList(ChatNum - 1);
			PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(friendTalk.RoleId);
			if (playerInfo && HintButton)
			{
				eventCallSetHead(playerInfo->_RoleClass,playerInfo->_Sex);
				if (HintButton->GetBool(TEXT("visible")) == FALSE)
				{
					HintButton->SetVisible(TRUE);
				}

			}

			//SetVisible(TRUE);
		}
	}

}

void UMMOUIFriendChatHint::PostOnShow(UBOOL v)
{
	//Override, because movetop is not wanted
}

void UMMOUIFriendChatHint::ASFriendChatList(INT Index)
{
	ClickFriendChatHint(Index);
}

void UMMOUIFriendChatHint::Init()
{
	Super::Init();

	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_ShowFriendChatHint);
	evts.AddItem(GameEvent_HideFriendChatHint);
	evts.AddItem(GameEvent_SetFriendChatHint);
	evts.AddItem(GameEvent_PlayerBaseInfo);

	EventAdapter = new TGameEventAdapter<UMMOUIFriendChatHint>(this, evts);
}
void UMMOUIFriendChatHint::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_ShowFriendChatHint:
		{
			MMOGameEvent_FriendChat_ShowChatHint* e = (MMOGameEvent_FriendChat_ShowChatHint*)evt;
			if (e)
			{
				ShowChatHint(e->roldId);
			}
		}
		break;
	case GameEvent_HideFriendChatHint:
		{
			HideChatHint();
		}
		break;
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo *e = (MMOGameEvent_PlayerBaseInfo*)evt;
			if (e)
			{
				UpdateHeadImg(e->_RoldID);
			}

		}
		break;
	}
}

void UMMOUIFriendChatHint::UpdateHeadImg(UINT32 roleId)
{
	if( HintButton )
	{
		const TArray<ContactLogStruct>& UnReadContactLogList =  FriendChatManager::getSingleton().GetUnReadLog();
		if (UnReadContactLogList.Num() > 0)
		{
			ContactLogStruct friendTalk = UnReadContactLogList(UnReadContactLogList.Num() - 1);
			if (roleId == friendTalk.RoleId)
			{
				PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(friendTalk.RoleId);
				if (playerInfo)
				{
					eventCallSetHead(playerInfo->_RoleClass,playerInfo->_Sex);

					if (HintButton->GetBool(TEXT("visible")) == FALSE)
					{
						HintButton->SetVisible(TRUE);
					}

				}
			}

		}
	}
}
void UMMOUIFriendChatHint::ShowChatHint(INT curRoldId)
{
	INT unReadChatNum = 0;
	INT curChatNum = 0;
	INT roleClass = 0;
	INT roleSex = 0;

	const TArray<ContactLogStruct>& UnReadContactLogList =  FriendChatManager::getSingleton().GetUnReadLog();
	const TArray<int>& curChatList = FriendChatManager::getSingleton().GetChatList();

	unReadChatNum = UnReadContactLogList.Num();
	curChatNum = curChatList.Num();

	if ( unReadChatNum > 0 )
	{
		ContactLogStruct friendTalk = UnReadContactLogList(unReadChatNum - 1);
		PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(friendTalk.RoleId);
		if (playerInfo)
		{
			roleClass = playerInfo->_RoleClass;
			roleSex = playerInfo->_Sex;
		}

	}
	else if(curChatNum > 0)
	{
		MMOFriendMember member = FriendManager::getSingleton().GetFriendInfoByRoleId( curRoldId );
		roleClass = member.ClassId;
		roleSex = member.Sex;

	}

	if(HintButton)
	{
		eventCallsetHeadByMinimize(roleClass,roleSex);
		if (HintButton->GetBool(TEXT("visible")) == FALSE)
		{
			HintButton->SetVisible(TRUE);
		}

	}

}

void UMMOUIFriendChatHint::HideChatHint()
{
	if(ListMc)
	{
		ListMc->SetVisible(FALSE);
	}
	if(HintButton)
	{
		HintButton->SetVisible(FALSE);
	}
	if (list)
	{
		list->SetVisible(FALSE);
	}
}