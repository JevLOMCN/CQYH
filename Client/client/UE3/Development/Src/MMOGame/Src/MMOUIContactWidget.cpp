#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FriendManager.h"
#include "MMOMsgButtonManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOUIContactWidget)

#define OFFLINE 3
//------------------------------------------------------------------------
//void UMMOUIContactWidget::RequestDelAttention(INT RoleId)
//{
//	CMessageHandler::GetInstance().RequestDelFriend(RoleId);
//}

//------------------------------------------------------------------------
//void UMMOUIContactWidget::RequestAddAttention( INT RoleId,const FString& TargetName )
//{
//	if( RoleId == Player->ServerID || TargetName == Player->ActorName )
//	{
//		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.CantAddSelf") ) ); 
//		return;
//	}
//	CMessageHandler::GetInstance().RequestAddFriend( TargetName , RoleId);
//}



//------------------------------------------------------------------------
void UMMOUIContactWidget::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_UpdateNearContact);
	evts.AddItem(GameEvent_PlayerBaseInfo);
	evts.AddItem(GameEvent_UpdateFriendGroup);	
	evts.AddItem(GameEvent_UpdateBlackList);
	evts.AddItem(GameEvent_UpdateEnemyContent);
	evts.AddItem(GameEvent_FriendDelGroup);
	evts.AddItem(GameEvent_UpdateKillInfo);
	EventAdapter = new TGameEventAdapter<UMMOUIContactWidget>(this, evts);


	MyFriendStatus = GetObject( TEXT("MyFriendStatus") );
	ContactListContent = GetObject( TEXT("ContactListContentWidget") );
	SearchPanel = GetObject( TEXT("searchPanel") );
	
	if( ContactListContent )
	{
		SearchText = ContactListContent->GetObject( TEXT("textInput_search") );

		//ContactListContent->SetInt( TEXT("OFFLINE") , EFOS_Offline );
		//ContactListContent->SetInt( TEXT("ONLINE") , EFOS_Online );
		//ContactListContent->SetInt( TEXT("AFK") , EFOS_Afk );
		//ContactListContent->SetInt( TEXT("BUSY") , EFOS_Busy );

		TipsWidget = ContactListContent->GetObject( TEXT("PlayerTips") );
	}

	if(GFXPlayer)
	{
		UGFxObject* enumValue = GFXPlayer->CreateObject(TEXT("Object"));
		if(enumValue)
		{
			enumValue->SetInt( TEXT("offline") , E_FS_OFFLINE );
			enumValue->SetInt( TEXT("online") , E_FS_ONLINE );
			enumValue->SetInt( TEXT("afk") , E_FS_AFK );
			enumValue->SetInt( TEXT("busy") , E_FS_BUSY );
			eventCallSetEnumValue(enumValue);
		}
	}
		
	FScriptDelegate fun( this , TEXT("ChangeGroupName") );
	MMOUI::GetUIManager()->SetChangeGroupNameDelegate( fun );
		



	/*const INT statsNum = 3;
	FString text[statsNum];
	text[0] = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Online") );
	text[1] = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Afk") );
	text[2] = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Busy") );
	FString frame[statsNum];
	frame[0] = TEXT( "online" );
	frame[1] = TEXT( "afk" );
	frame[2] = TEXT( "busy" );
	INT stats[statsNum];
	stats[0] = E_FS_ONLINE;
	stats[1] = E_FS_AFK;
	stats[2] = E_FS_BUSY;

	if( MyFriendStatus )
	{
	for ( INT i = 0 ; i < statsNum ; i++ )
	{
	FString szName = TEXT("status");
	szName += appItoa( i );
	UGFxObject* obj = MyFriendStatus->GetObject( szName );
	if( obj )
	{
	UGFxObject* tf = obj->GetObject( TEXT("textField") );
	if(tf)
	tf->SetText( text[i] );
	UGFxObject* mc = obj->GetObject( TEXT("status_mc") );
	if(mc)
	mc->GotoAndStop( frame[i] );
	obj->SetInt( "itemid" , stats[i]+1);
	}
	}

	}*/

	UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
	if( data )
	{
		data->SetString( TEXT("defaultSignTxt") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.DefaultSign") ) );
		data->SetString( TEXT("defaultSearchText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.DefaultSearchText") ) );
		data->SetString( TEXT("statusButtonText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.Status") ) );
		data->SetString( TEXT("tab0") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.tab0") ) );
		data->SetString( TEXT("tab1") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.tab1") ) );
		data->SetString( TEXT("tab2") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.tab2") ) );
		data->SetString( TEXT("tab3") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.tab3") ) );
		data->SetString( TEXT("tab4") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.tab4") ) );
		eventCallSetPanelText(data);
	}
	

	//Initialize search panel
	FriendManager::getSingleton().ClearSearchList();
	FillInSearchPanel();
	
	//Default groups: friends, following
	CreateNewGroup( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Friend") ), DefaultFriendGroupId );
	CreateNewGroup( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Attention") ), AttentionGroupId );
	
	//eventCallUpdateKillInfo( TEXT("test\ntest<br>test3"));
}

void UMMOUIContactWidget::UnInit()
{
	UMMOUIBase::UnInit();

	FScriptDelegate funNone(EC_EventParm);
	//MMOUI::GetUIManager()->SetUpdateBlackListDelegate(funNone);
	//MMOUI::GetUIManager()->SetUpdateEnemyListDelegate(funNone);
	//MMOUI::GetUIManager()->SetUpdateNearContactDelegate(funNone);

	MMOUI::GetUIManager()->SetChangeGroupNameDelegate( funNone );
	//MMOUI::GetUIManager()->SetDeleteGroupDelegate( funNone );
}

void UMMOUIContactWidget::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if( GFXPlayer == NULL )
	{
		return;
	}

	//
	UGFxObject* tab = GetObject( TEXT("tab0") );
	if( tab )
	{
		GFXPlayer->eventCallWidgetDispatchEvent( tab , TEXT("click") );
	}

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
	{		
		return;
	}

	
	if(v)
	{
		UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
		if( data )
		{
			data->SetString( TEXT("headIcon") , MMOFacade::getSingleton().GetPlayerIcon( player->GetClassId() , player->GetSex() ) );
			data->SetString( TEXT("playerName") , player->ActorName );
			eventCallUpdateBaseInfo(data);
		}
		UpdateDateList(DT_BlackList);
		UpdateDateList(DT_FriendNear);
		//UpdateDateList(DT_FriendList);
		UpdateDateList(DT_EnemyList);
		UpdateAllFriend();

		UpdateKillInfo();
		
	}	
}

void UMMOUIContactWidget::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(!evt)
	{
		return;
	}

	if (this->GetVisible()== FALSE)
	{
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_UpdateBlackList:
		/*if( BlackListContent )
		{
		BlackListContent->UpdateBlackList();
		}*/
		UpdateDateList(DT_BlackList);
		break;
	case GameEvent_UpdateEnemyContent:
		/*if(EnemyListContent)
		{
		EnemyListContent->UpdateEnemyList();
		}*/
		UpdateDateList(DT_EnemyList);
		break;
	case GameEvent_UpdateFriendGroup:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
			if( msg /*&& FriendListContent */)
			{
				//FriendListContent->UpdateFriendGroup( msg->IntData );
				UpdateDateList(DT_FriendList,msg->IntData);
			}

		}
		break;
	case GameEvent_UpdateNearContact:
		/*if( NearContactContent )
		{
			NearContactContent->UpdateNearContactList();
		}	*/
		UpdateDateList(DT_FriendNear);
		break;
	case GameEvent_FriendDelGroup:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
			if( msg )
			{
				DeleteGroup(msg->IntData);
			}
		}
		break;
	case GameEvent_UpdateKillInfo:
		{
			UpdateKillInfo();
		}
		break;
	}
	
}


///------------------------------------------------------------------------
void UMMOUIContactWidget::UpdateKillInfo()
{
	TArray<MMOKillInfo>& list = FriendManager::getSingleton().GetKillInfoList();
	FString showText;
	for ( INT i = list.Num()-1 ; i >= 0 ; i-- )
	{
		showText += list(i).ShowText + TEXT("<br>");
	}
	eventCallUpdateKillInfo(showText);
}

///------------------------------------------------------------------------
void UMMOUIContactWidget::SearchFocusChange( UBOOL in , FString text )
{
	if( in )
	{
		if( SearchPanel && !text.IsEmpty() )
		{			
			SearchPanelWillOpen = TRUE;
			NeedUpdateSearchPanel = TRUE;
			FriendChangeSearchText(text);
		}
	}
	else
	{
		if( SearchPanel )
		{			
			SearchPanelWillOpen = FALSE;
			NeedUpdateSearchPanel = TRUE;
		}
	}
}

void UMMOUIContactWidget::UpdateTime(FLOAT dt)
{
	
	if( NeedUpdateSearchPanel && SearchPanel ) 
	{
		UpdateSearchTime += dt;
		if( UpdateSearchTime >0.5 )
		{
			SearchPanel->SetVisible(SearchPanelWillOpen);
			NeedUpdateSearchPanel = FALSE;
			UpdateSearchTime = 0;
		}
		
	}
}

void UMMOUIContactWidget::FriendChangeSearchText(const FString& text)
{
	if( text.IsEmpty() )
	{
		if( SearchPanel )
		{
			SearchPanel->SetVisible(FALSE);

		}
		
	}
	else
	{
		if( SearchPanel )
		{
			SearchPanel->SetVisible(TRUE);
			INT groupNum = FriendManager::getSingleton().GetFriendGroupNum();

			TArray<MMOFriendMember> matchFriendList;
			
			for ( INT grpIdx = 0 ; grpIdx < groupNum ; grpIdx++ )
			{
				INT friendNum = FriendManager::getSingleton().GetFriendNumInGroup(grpIdx);
				for ( INT friendIdx = 0 ; friendIdx < friendNum ; friendIdx++ )
				{
					MMOFriendMember* friendMember = FriendManager::getSingleton().GetFriendGroupMember( grpIdx , friendIdx );
					if( friendMember && ( friendMember->AbbName.InStr( text ) >= 0 || friendMember->ActorName.InStr( text ) >= 0 ) )
					{
						if( matchFriendList.Num() > UCONST_MaxSearchPanelItem )
						{
							return;
						}

						matchFriendList.AddItem(*friendMember);
						
					}
				}
			}

			UpdateSearchList(matchFriendList);
		}
	}
}

//-------------------------------------------------------------------------------------
void UMMOUIContactWidget::FriendOverItem( INT groupId , INT index )
{
	MMOFriendMember* member = FriendManager::getSingleton().GetFriendGroupMember( groupId , index );
	if( member && TipsWidget )
	{
		UGFxObject* obj = TipsWidget->GetObject(TEXT("HeadIcon"));
		if(obj)
		{
			FString path = MMOFacade::getSingleton().GetPlayerIcon(member->ClassId,member->Sex);
			
			obj->SetString( TEXT("source") , path );
		}
	}
}

void UMMOUIContactWidget::UpdateSearchList( TArray<MMOFriendMember>& resList )
{
	INT resNum = resList.Num();
	UBOOL same = TRUE;
	MMOFriendMember* searchList = FriendManager::getSingleton().GetSearchList();

	if( resNum > UCONST_MaxSearchPanelItem )
	{
		//Exceeds
		resNum = UCONST_MaxSearchPanelItem;
		//return;
	}

	for( INT idx = 0 ; idx < resNum ; idx++ )
	{
		if( resList(idx).RoleId != searchList[idx].RoleId )
		{
			same = FALSE;
			break;
		}
	}

	if( same )
	{
		for( INT idx = resNum ; idx < UCONST_MaxSearchPanelItem ; idx++ )
		{
			if( searchList[idx].RoleId != 0 )
			{
				same = FALSE;
				break;
			}
		}
	}


	if( !same )
	{
		appMemzero( searchList , sizeof(MMOFriendMember)*UCONST_MaxSearchPanelItem );
		//appMemcpy( searchList , resList.GetData() , sizeof(MMOFriendMember)*resNum );

		for ( INT j = 0 ; j < resNum ; j++ )
		{
			searchList[j] = resList(j);
		}

		FillInSearchPanel();
	}
	//
}

void UMMOUIContactWidget::FillInSearchPanel()
{
	MMOFriendMember* searchList = FriendManager::getSingleton().GetSearchList();
	
	UGFxObject* searchData = GFXPlayer->CreateObject( TEXT("Object") );
	if(searchData)
	{
		INT has = 0;
		UGFxObject *data = GFXPlayer->CreateArray();
		for ( INT i = 0 ; i < UCONST_MaxSearchPanelItem ; i++ )
		{
			MMOFriendMember& member = searchList[i];
			if( member.RoleId == 0 )
			{
				break;
			}
			has++;

			UGFxObject* item = GFXPlayer->CreateObject( TEXT("Object") );
			item->SetString( TEXT("name") , member.ActorName );
			item->SetInt( TEXT("lv") , member.nIntimacy );
			item->SetString( TEXT("classTxt") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa(member.ClassId)  ) );
			item->SetString( TEXT("location") , member.Location );
			item->SetString( TEXT("sign") , member.Sign );
			item->SetInt( TEXT("roleId") , member.RoleId );
			item->SetInt( TEXT("stats") , member.Online );
			FString headIcon = MMOFacade::getSingleton().GetPlayerIcon( member.ClassId , member.Sex );
			if (member.Online  == OFFLINE)
			{
				item->SetString( TEXT("iconPath") ,  headIcon+"_g");
				
			}
			else
			{
				item->SetString( TEXT("iconPath") ,  headIcon);
			}
			
			data->SetElementObject( i , item );
		}
		searchData->SetObject( TEXT("searchArray") , data );
		searchData->SetString( TEXT("descText") , FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.DescText") ) , has )  );
	}

	eventCallSetSearchData( searchData );
}


void UMMOUIContactWidget::ClearUI()
{
	/*if( BlackListContent )
	{
		BlackListContent->ClearUI();
	}

	if( EnemyListContent )
	{
		EnemyListContent->ClearUI();
	}

	if( FriendListContent )
	{
		FriendListContent->ClearUI();
	}*/

	for ( INT i = 0 ; i < AttentionGroupId+1 ; i++ )
	{
		UpdateDateList(DT_FriendList,i);
	}
	for ( INT i = 1 ; i < AttentionGroupId ; i++ )
	{
		//Delete all groups
		eventCallDeleteGroup(i);
	}
		
}



//-------------------------------------------------------------------------------------------------------------------------
void UMMOUIContactWidget::DeleteGroup(INT groupID)
{
	/*if(FriendListContent)
	{
	FriendListContent->eventCallDeleteGroup(groupID);
	}*/
	eventCallDeleteGroup(groupID);
}
//-------------------------------------------------------------------------------------------------------------------------
void UMMOUIContactWidget::CreateNewGroup(const FString& groupName , INT groupID)
{
	MMOFriendGroup* fGroup = FriendManager::getSingleton().GetFriendGroup(groupID);
	if(!fGroup)
	{
		return;
	}

	fGroup->GroupName = groupName;

	if( groupID == DefaultFriendGroupId || groupID == AttentionGroupId ) //friend group and following group are defaults, operations differ
	{
		fGroup->IsDefault = TRUE;
	}
	else
	{
		fGroup->IsDefault = FALSE;
	}
	
	/*if(FriendListContent)
	{
	FriendListContent->eventCallCreateNewGroup( groupName , groupID );
	}*/
	eventCallCreateNewGroup( groupName , groupID );
}
//-------------------------------------------------------------------------------------------------------------------------
void UMMOUIContactWidget::ChangeGroupName(const FString& GroupName,INT GroupId)
{
	if(GroupName.IsEmpty())
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.GroupName") )  ,MsgInfoType_Table);
		return;
	}
	//Banned word filter
	FString name = GroupName;
	MMOUI::ReplaceCensoredWord(name);

	CreateNewGroup( name , GroupId );

	CMessageHandler::GetInstance().ChangeFriendGroupName(name  , GroupId );
}


void UMMOUIContactWidget::SetEnemyListNum( INT num )
{
	TArray<struct FASValue> Arguments;
	FASValue ASValue(EC_EventParm);

	ASValue.I = num;
	ASValue.Type = AS_Int;
	Arguments.AddItem(ASValue);

	if(ContactListContent)
	{
		ContactListContent->Invoke("SetEnemyListNum", Arguments );
	}
}

void UMMOUIContactWidget::SetEnemyItemValue( INT pos , struct MMOFriendMember* member )
{
	if(!member)
	{
		return;
	}

	TArray<struct FASValue> Arguments;

	Arguments.AddZeroed( 9 ); //argument count

	Arguments(0).I = pos;
	Arguments(0).Type = AS_Int;	 

	Arguments(1).S = member->ActorName ;
	Arguments(1).Type = AS_String;

	Arguments(2).I = member->nIntimacy ;
	Arguments(2).Type = AS_Int;

	Arguments(3).S = MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.School.SchoolName")) + appItoa(member->ClassId) ) ;
	Arguments(3).Type = AS_String;

	Arguments(4).S = member->Location;
	Arguments(4).Type = AS_String;

	Arguments(5).S = member->Sign ;
	Arguments(5).Type = AS_String;

	Arguments(6).I = member->RoleId;
	Arguments(6).Type = AS_Int;

	Arguments(7).I = member->Online  ;
	Arguments(7).Type = AS_Int;

	Arguments(8).S = MMOFacade::getSingleton().GetPlayerIcon( member->ClassId , member->Sex )  ;
	Arguments(8).Type = AS_String;

	if( ContactListContent )
	{
		ContactListContent->Invoke( "SetEnemyListItemValue" , Arguments );
	}
}

void UMMOUIContactWidget::SetBlackListNum( INT num )
{
	TArray<struct FASValue> Arguments;
	FASValue ASValue(EC_EventParm);

	ASValue.I = num;
	ASValue.Type = AS_Int;
	Arguments.AddItem(ASValue);

	if(ContactListContent)
	{
		ContactListContent->Invoke("SetBlackListNum", Arguments );
	}
	
}

void UMMOUIContactWidget::SetBlackItemValue( INT pos , struct MMOFriendMember* member )
{
	if(!member)
	{
		return;
	}

	TArray<struct FASValue> Arguments;

	Arguments.AddZeroed( 9 ); //argument count

	Arguments(0).I = pos;
	Arguments(0).Type = AS_Int;	 

	Arguments(1).S = member->ActorName ;
	Arguments(1).Type = AS_String;

	Arguments(2).I = member->nIntimacy ;
	Arguments(2).Type = AS_Int;

	Arguments(3).S = MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.School.SchoolName")) + appItoa(member->ClassId) ) ;
	Arguments(3).Type = AS_String;

	Arguments(4).S = member->Location;
	Arguments(4).Type = AS_String;

	Arguments(5).S = member->Sign ;
	Arguments(5).Type = AS_String;

	Arguments(6).I = member->RoleId;
	Arguments(6).Type = AS_Int;

	Arguments(7).I = member->Online  ;
	Arguments(7).Type = AS_Int;

	Arguments(8).S = MMOFacade::getSingleton().GetPlayerIcon( member->ClassId , member->Sex )  ;
	Arguments(8).Type = AS_String;

	if( ContactListContent )
	{
		ContactListContent->Invoke( "SetBlackListItemValue" , Arguments );
	}
}

//------------------------------------------------------------------------
void UMMOUIContactWidget::SetFriendNum(INT GroupID,INT Num,INT OnlineNum)
{
	TArray<struct FASValue> Arguments;
	FASValue ASValue(EC_EventParm);
	ASValue.I = GroupID;
	ASValue.Type = AS_Int;
	Arguments.AddItem(ASValue);
	ASValue.I = Num;
	ASValue.Type = AS_Int;
	Arguments.AddItem(ASValue);
	ASValue.I = OnlineNum;
	ASValue.Type = AS_Int;
	Arguments.AddItem(ASValue);

	if(ContactListContent)
		ContactListContent->Invoke("SetFriendNum", Arguments );
}


///------------------------------------------------------------------------
void UMMOUIContactWidget::SendChangeMyInfo()
{
	CMessageHandler::GetInstance().RequestFriendChangeInfo( MySign , MyStatus );
}



void UMMOUIContactWidget::NativeInitOnce()
{

}

void UMMOUIContactWidget::ChangeMyInfo(const FString& Sign,INT status)
{
	if( MMOUI::HasCensoredWord( Sign ) )
	{
		if( ContactListContent )
		{
			UGFxObject* signTF = ContactListContent->GetObject(TEXT("textInput_signature"));
			if(signTF)
			{
				signTF->SetText( MySign );
			}
		}		
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.SignInvalid")) ,MsgInfoType_Table,TEXT("MMOGame.Friend.SignInvalid"));
		//Better to put focus in last
		eventCallSetFocusMc( TEXT("ContactListContentWidget") , TEXT("textInput_signature") , TEXT("") , TEXT("") );
		return;
	}
	
	MySign = Sign;
	MyStatus = status;
	SendChangeMyInfo();
}

//--------------moved-----------------------
void UMMOUIContactWidget::ASFriendChangeSearchText(const FString& SearchText)
{

	FriendChangeSearchText(SearchText);

}

void UMMOUIContactWidget::ASFriendSearchFocusChange(UBOOL in,const FString& searchText)
{
	SearchFocusChange( in , searchText );
}

void UMMOUIContactWidget::ASFriendChangeStatus(INT Status)
{
	ChangeMyInfo( MySign , Status );
}

void UMMOUIContactWidget::ASFriendChangeSignature(const FString& Sign)
{
	ChangeMyInfo( Sign , MyStatus );	
}
void UMMOUIContactWidget::ASFriendAddFriend()
{
	if( msgBox != NULL )
	{
		msgBox->SetVisible( FALSE );
		msgBox = NULL;
	}

	FScriptDelegate funOk(EC_EventParm);
	funOk.FunctionName = "AddNewFriend";
	funOk.Object = this;

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;

	FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.AddAttention") );

	msgBox = MMOMsgBoxManager::getSingleton().AddInputMessageBox(msg  , 0 , 
		msg , funOk,  funNo, eInputFormatAll );

	//MMOMsgBoxManager::getSingleton().AddModalInputMsgboxEvent( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.AddAttention") ) , 0 , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.AddAttention")) , GameEvent_FriendAdd );
}

void UMMOUIContactWidget::AddNewFriend(class UGFxObject* mc)
{
	if( !mc )return;

	FString InputText = mc->GetObject( "textInput" )->GetString("text");

	if( !InputText.IsEmpty() )
	{
		FriendManager::getSingleton().RequestAddAttention( 0 , InputText );
	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.EmptyName") ) , MsgInfoType_Table ,TEXT("MMOGame.Friend.EmptyName"));
	}

	msgBox = NULL;
}

void UMMOUIContactWidget::AddNewFriend_No(class UGFxObject* mc)
{
	msgBox = NULL;
}

void UMMOUIContactWidget::ASClickSearchItem( INT pos )
{		
	MMOFriendMember* member = FriendManager::getSingleton().GetSearchFriend(pos);

	if(member)
	{
		MMOUI::GetUIManager()->delegateSelectFriendChat( member->RoleId , member->ActorName );
	}
}

void UMMOUIContactWidget::PostAdvance(FLOAT dt)
{
	UpdateTime(dt);
}

void UMMOUIContactWidget::ASFriendItemDClick(class UGFxObject* mc)
{
	if( mc )
	{
		INT index = mc->GetInt(TEXT("itemid"));
		INT groupId = mc->GetInt(TEXT("itemdataid"));

		if (groupId == AttentionGroupId)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errFriendNotMutual"));
			MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Warning,text);
		
			return;
		}
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

void UMMOUIContactWidget::ASFriendItemRightClick(INT Posx,INT Posy,class UGFxObject* Item,INT ItemId,const FString& NameText)
{
	INT GroupId;
	INT RoleId;

	RoleId = Item->GetInt( "iteminfoid");
	GroupId = FriendManager::getSingleton().GetFriendGroupId(RoleId);//Item->GetInt( "itemdataid");
	
	if( GroupId == AttentionGroupId )
	{
		MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_Attention , Posx , Posy , GroupId , NameText, ItemId );
	}
	else 
	{
		MMOFriendMember* mem = NULL;
		if( GroupId < AttentionGroupId )
		{
			mem = FriendManager::getSingleton().GetFriendGroupMemberById(GroupId,RoleId);
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

void UMMOUIContactWidget::ASFriendOverItem(INT groupId,INT index)
{
	FriendOverItem( groupId, index);
}

void UMMOUIContactWidget::UpdateDateList(DataType type,int groupId)
{
	TArray<MMOFriendMember>* list = NULL;
	UGFxObject *data = NULL;
	MMOFriendGroup* fGroup = NULL;

	if(!GFXPlayer)
	{
		return;
	}

	switch(type)
	{
	case DT_FriendNear:
		{
			list = &(FriendManager::getSingleton().NearContactList);
			data = GFXPlayer->CreateArray();
		}
		break;
	case DT_FriendList:
		{
			fGroup = FriendManager::getSingleton().GetFriendGroup(groupId);
			if(!fGroup)
			{
				return;
			}
			data = GFXPlayer->CreateObject(TEXT("Object"));
		}
		break;
	case DT_BlackList:
		{
			list = &(FriendManager::getSingleton().GetBlackList());
			data = GFXPlayer->CreateArray();
		}
		break;
	case DT_EnemyList:
		{
			list = &(FriendManager::getSingleton().EnemyList);
			data = GFXPlayer->CreateArray();
		}
		break;
	}
	 
	if ( type == DT_FriendList)
	{
		FriendManager::getSingleton().SortFriendGroup( groupId );

		INT friendNum = fGroup->FriendList.Num(); 
		//Note: this online actually means not offline
		INT onlineFriendNum = FriendManager::getSingleton().GetOnlineFriendNumInGroup(groupId); 

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
						member->SetInt( TEXT("lv") , friendMember->nIntimacy );
						member->SetString( TEXT("classTxt") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( friendMember->ClassId)  ) );
						if(friendMember->Online != OFFLINE)
						{
							member->SetString( TEXT("location") , "-");//friendMember->Location );
						}
						else
						{
							member->SetString( TEXT("location") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Offline") ));
						}
						
						member->SetString( TEXT("sign") , friendMember->Sign );
						member->SetInt( TEXT("roleId") , friendMember->RoleId );
						member->SetInt( TEXT("stats") , friendMember->Online );
						FString headIcon = MMOFacade::getSingleton().GetPlayerIcon( friendMember->ClassId , friendMember->Sex );
						if (friendMember->Online == OFFLINE)
						{
							member->SetString( TEXT("iconPath") , headIcon+"_g" );
							
						}
						else
						{
							member->SetString( TEXT("iconPath") , headIcon );
						}

						memberList->SetElementObject(i,member);
					}				
				}
				data->SetObject( TEXT("memberList") , memberList );
			}
		}
	}
	else
	{
		for ( INT i = 0 ; i < (*list).Num() ; i++ )
		{
			MMOFriendMember blackContact = (*list)(i);
			if (blackContact.RoleId <= 0)
				continue;
			UGFxObject* item = GFXPlayer->CreateObject( TEXT("Object") );
			item->SetString( TEXT("name") , blackContact.ActorName );
			item->SetInt( TEXT("lv") , blackContact.nIntimacy );
			item->SetString( TEXT("classTxt") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa(blackContact.ClassId)  ) );
			item->SetString( TEXT("location")  , "-");// blackContact.Location );
			item->SetString( TEXT("sign") , blackContact.Sign );
			item->SetInt( TEXT("roleId") , blackContact.RoleId );
			item->SetInt( TEXT("stats") , blackContact.Online );
			item->SetString( TEXT("iconPath") , MMOFacade::getSingleton().GetPlayerIcon(blackContact.ClassId,blackContact.Sex) );

			data->SetElementObject( i , item );
		}
	}
	

	eventCallSetFriendListData(data , type);
}

void UMMOUIContactWidget::UpdateAllFriend()
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
			UpdateDateList(DT_FriendList,i);
		}
	}
}

void UMMOUIContactWidget::ASShowTips_State(class UGFxObject *mc)
{
	if( mc == NULL ){
		return;
	}
	mc->SetString( TEXT("tipsHtml"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.Status") ) );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

void UMMOUIContactWidget::ASHideTips_State(class UGFxObject *mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUIContactWidget::ASNearContactDClick(class UGFxObject* mc)
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

