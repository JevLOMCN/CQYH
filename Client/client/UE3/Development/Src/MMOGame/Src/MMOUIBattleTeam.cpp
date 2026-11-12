#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOPowerManager.h"
#include "BattleTeamManager.h"
#include "MMOMsgBoxManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIBattleTeam)

#define CreateTeamCost 1000
#define BattleTeamBtnNum 5
//number of text box controls
#define BattleTeamContentNum 6

//-------------------------------------------------------------------------------------
void UMMOUIBattleTeam::AcceptJoinBattleTeam( UGFxObject* mc )
{
	if( mc )
	{
		INT roleID = mc->GetInt( TEXT("iteminfoid") );
		CMessageHandler::GetInstance().Send_JoinBattleTeamAnswer( roleID );
	}

}

void UMMOUIBattleTeam::NativeInitOnce()
{
	//UGFxObject* obj = GetObject( TEXT("LeaveTeam") );
	//if(obj)
	//	obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.Leave") ) );

	//obj = GetObject( TEXT("ClosePanel") );
	//if(obj)
	//	obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.Cancel") ) );

}

//-------------------------------------------------------------------------------------
void UMMOUIBattleTeam::InviteEnterTeam(INT roleID,const FString& roleName)
{
	FScriptDelegate AcceptJoinBattleTeam(EC_EventParm);
	AcceptJoinBattleTeam.FunctionName = TEXT("AcceptJoinBattleTeam");
	AcceptJoinBattleTeam.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("");
	Nofunction.Object = this;

	FString szText = roleName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.InviteYou"));
	MMOMsgBoxManager::getSingleton().AddMessageBox( szText , roleID , AcceptJoinBattleTeam , Nofunction );
}

void UMMOUIBattleTeam::UpdateBattleTeamInfo()
{

	//const BattleTeamAchievement* achievementTable = BattleTeamManager::getSingleton().GetAchievementTable();
	//for ( INT j = 0 ; j < atptTotal ; j++ )
	{
/*		if( BattleTable[j] )
		{
			BattleTable[j]->GetObject( TEXT("ScoreTxt") )->SetText( appItoa(achievementTable[j].Score) );
			BattleTable[j]->GetObject( TEXT("RankTxt") )->SetText( appItoa(achievementTable[j].Rank) );
			BattleTable[j]->GetObject( TEXT("RunNumTxt") )->SetText( appItoa(achievementTable[j].Match) );
			BattleTable[j]->GetObject( TEXT("WinRateTxt") )->SetText( appItoa(achievementTable[j].WinRate) );
		}	*/	
	}

	//if(TeamNameTxt)
	//{
	//	TeamNameTxt->SetText( BattleTeamManager::getSingleton().GetTeamName() );
	//}
}

void UMMOUIBattleTeam::UpdateBattleMemberList()
{

	//const TArray<BattleTeamMember>& memberList = BattleTeamManager::getSingleton().GetMemberList();


	//for ( INT i = 0 ; i < TeamMemberNum ; i++ )
	//{
	//	UGFxObject* member = GetObject( FString::Printf( TEXT("RoleButton%d") , i ) );
	//	if( member )
	//	{
	//		member->SetVisible(FALSE);
	//	}
	//}

	//for ( INT i = 0 ; i < memberList.Num() ; i++ )
	//{
	//	UGFxObject* member = GetObject( FString::Printf( TEXT("RoleButton%d") , i ) );
	//	if( member )
	//	{
	//		member->SetVisible(TRUE);
	//		member->GetObject( TEXT("RoleNameTxt") )->SetText( memberList(i).PlayerName );
	//		member->GetObject( TEXT("LvTxt") )->SetText( FString::Printf( TEXT("lv%d") , memberList(i).Level ) );
	//		member->GetObject( TEXT("ClassTxt") )->SetText( 
	//			MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(memberList(i).ClassID)  ) );
	//		if( i == 0 )
	//		{
	//			member->GetObject( TEXT("CaptainTag") )->SetVisible(TRUE);
	//		}
	//		else
	//		{
	//			member->GetObject( TEXT("CaptainTag") )->SetVisible(FALSE);
	//		}
	//	}
	//}


	//this team message update may become a separate protocol; there is none currently, so put it here for now
	UpdateTeamInfo();
}

void UMMOUIBattleTeam::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	//if(evt->GetEventType() != GameEvent_BattleTeam )
	//{
	//	return;
	//}

	switch( evt->GetEventType() )
	{
	case GameEvent_BattleTeam:
		{
			MMOGameEvent_BattleTeam* battleTeamEvt = (MMOGameEvent_BattleTeam*)evt;
			if(battleTeamEvt == NULL)
			{
				return;
			}

			switch( battleTeamEvt->EvtType )
			{
			case MMOGameEvent_BattleTeam::EBET_TurnTab:
				{
					ClickOwnPanel( battleTeamEvt->tab );
				}
				break;
			case MMOGameEvent_BattleTeam::EBET_UpdateMemberList:
				if( false/* BattleTeamManager::getSingleton().HasTeam()*/ )
				{
					UpdateTeamInfo();
					UpdateMemberList();
				}
				else
				{
					//switches to the single-player page when leaving the team
					ClickOwnPanel(BattleTeamPage_1v1);
				}
				break;
			//case MMOGameEvent_BattleTeam::EBET_InviteEnterTeam:

			//	InviteEnterTeam( battleTeamEvt->roleID , battleTeamEvt->roleName );
			//	break;
			default:
				break;
			}
		}
		break;
	case GameEvent_BattleTeamCreate:
		{
			MMOGameEvent_MsgboxCallBack* msgboxEvt = (MMOGameEvent_MsgboxCallBack*)evt;
			if( msgboxEvt && msgboxEvt->Box )
			{
				if( msgboxEvt->Agree )
				{
					UGFxObject* inputClip = msgboxEvt->Box->GetObject( "textInput" );
					if( inputClip )
					{
						FString InputText = inputClip->GetString("text");
						CMessageHandler::GetInstance().Send_CreateBattleTeam( InputText , 0 );
					}
					
				}
			}
			
		}
		break;
	case GameEvent_BattleTeamAddMember:
		{
			MMOGameEvent_MsgboxCallBack* msgboxEvt = (MMOGameEvent_MsgboxCallBack*)evt;
			if( msgboxEvt && msgboxEvt->Box )
			{
				if( msgboxEvt->Agree )
				{
					UGFxObject* inputClip = msgboxEvt->Box->GetObject( "textInput" );
					if( inputClip )
					{
						FString InputText = inputClip->GetString("text");
						CMessageHandler::GetInstance().Send_JoinBattleTeam(InputText);
					}
				}

			}
		}
	}

	

}

void UMMOUIBattleTeam::ClickAddMember()
{
	//if( BattleTeamManager::getSingleton().IsTeamLeader() )
	//{
	//	MMOMsgBoxManager::getSingleton().AddInputMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.AddMember") ) , 0 , MMOUI::ParseLocalizedPropertyPath( 
	//		TEXT("MMOGame.Arena.InputName") ) , GameEvent_BattleTeamAddMember );
	//}
}

void UMMOUIBattleTeam::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_BattleTeam);
	evts.AddItem(GameEvent_BattleTeamCreate);
	evts.AddItem(GameEvent_BattleTeamAddMember);
	EventAdapter = new TGameEventAdapter<UMMOUIBattleTeam>(this, evts);

	if( GFXPlayer )
	{
		UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
		if(data)
		{
			data->SetString( TEXT("titleName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.Title") ) );
			data->SetString( TEXT("addButton") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.AddButton") ) );

			FString path;
			UGFxObject* btnSingleArray = GFXPlayer->CreateArray();
			if( btnSingleArray )
			{
				for ( INT i = 0 ; i < BattleTeamBtnNum ; i++ )
				{
					path = TEXT("MMOGame.BattleTeam.SingleBtn");
					path += appItoa(i);					
					btnSingleArray->SetElementString( i , MMOUI::ParseLocalizedPropertyPath( path ) );
				}	
				data->SetObject( TEXT("btnSingleArray") , btnSingleArray );
			}

			UGFxObject* btnTeamArray = GFXPlayer->CreateArray();
			if( btnTeamArray )
			{
				for ( INT i = 0 ; i < BattleTeamBtnNum ; i++ )
				{
					path = TEXT("MMOGame.BattleTeam.TeamBtn");
					path += appItoa(i);	
					btnTeamArray->SetElementString( i , MMOUI::ParseLocalizedPropertyPath( path ) );
				}	
				data->SetObject( TEXT("btnTeamArray") , btnTeamArray );
			}

			UGFxObject* contentSingleArray = GFXPlayer->CreateArray();
			if( contentSingleArray )
			{
				for ( INT i = 0 ; i < BattleTeamContentNum ; i++ )
				{
					path = TEXT("MMOGame.BattleTeam.SingleContent");
					path += appItoa(i);	
					contentSingleArray->SetElementString( i , MMOUI::ParseLocalizedPropertyPath( path ) );
				}	
				data->SetObject( TEXT("contentSingleArray") , contentSingleArray );
			}

			UGFxObject* contentTeamArray = GFXPlayer->CreateArray();
			if( contentTeamArray )
			{
				for ( INT i = 0 ; i < BattleTeamContentNum ; i++ )
				{
					path = TEXT("MMOGame.BattleTeam.TeamContent");
					path += appItoa(i);	
					contentTeamArray->SetElementString( i , MMOUI::ParseLocalizedPropertyPath( path ) );
				}	
				data->SetObject( TEXT("contentTeamArray") , contentTeamArray );
			}
		}
		eventCallSetPanelText(data);

	}
	
	ClickOwnPanel(BattleTeamPage_1v1);
	
}

void UMMOUIBattleTeam::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	UpdateTeamInfo();
	
}

void UMMOUIBattleTeam::UpdateTeamInfo()
{
	UGFxObject* teamInfo = GFXPlayer->CreateObject( TEXT("Object") );
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !player )
	{
		return;
	}
	//this team's battle record cannot be used directly
	
	if( PageType == BattleTeamPage_1v1 )
	{
		if(teamInfo)
		{
			//no data here, so assign empty values for now
			teamInfo->SetString( TEXT("score") , appItoa(player->PersonalArenaScore) );
			teamInfo->SetString( TEXT("rank") , TEXT("")/*appItoa(0)*/ );
			teamInfo->SetString( TEXT("arenaCount") , TEXT("")/*appItoa(0)*/ );
			teamInfo->SetString( TEXT("win") , TEXT("")/*appItoa(0)*/ );
			teamInfo->SetString( TEXT("lose") , TEXT("")/*appItoa(0)*/ );
			teamInfo->SetString( TEXT("rate") , TEXT("")/*appItoa(0)*/ );
			teamInfo->SetString( TEXT("myName") , player->GetActorName() );
			teamInfo->SetString( TEXT("period") , TEXT("")/*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.Period2") )*/ );
			
			eventCallUCTurnOneToOne(teamInfo);
		}
	}
	else if( PageType == BattleTeamPage_5v5 )
	{
		if(teamInfo)
		{
			//const BattleTeamAchievement* arenaAchievement = BattleTeamManager::getSingleton().GetAchievementTable( atpt5V5 );
			//if( arenaAchievement )
			//{
			//	teamInfo->SetString( TEXT("score") , appItoa( arenaAchievement->Score ) );
			//	teamInfo->SetString( TEXT("rank") , appItoa( arenaAchievement->Rank ) );
			//	teamInfo->SetString( TEXT("teamCount") , appItoa( arenaAchievement->Match ) );
			//	teamInfo->SetString( TEXT("win") , appItoa( arenaAchievement->Win ) );
			//	teamInfo->SetString( TEXT("lose") , appItoa( arenaAchievement->Match - arenaAchievement->Win ) );
			//	teamInfo->SetString( TEXT("singleCount") , appItoa( arenaAchievement->Match ) );
			//	teamInfo->SetBool( TEXT("isLeader") , BattleTeamManager::getSingleton().IsTeamLeader() );
			//	teamInfo->SetString( TEXT("teamName") , BattleTeamManager::getSingleton().GetTeamName() );
			//	teamInfo->SetString( TEXT("period") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.Period2") ) );

			//	eventCallUCTurnFiveToFive(teamInfo);
			//}
			
		}
	}
		
}

void UMMOUIBattleTeam::ClickOwnPanel( INT tab )
{
	UGFxObject* tabButton = GetObject( FString( TEXT("BattleTable") ) + appItoa(tab) );
	if( tabButton )
	{
		GFXPlayer->eventCallWidgetDispatchEvent( tabButton , TEXT("click") );
	}
}

void UMMOUIBattleTeam::UpdateMemberList()
{
	if( PageType == BattleTeamPage_1v1 )
	{
		//label:"Summary", index:0, type:0, deepth:0, visible:true , MemberName:"1",MemberClass:"Warrior",MemberBattleCount:20,MemberGrade:"1-3",MemberRate:"56%
		UGFxObject* teamList = GFXPlayer->CreateArray();
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

		if( teamList && player )
		{
			UGFxObject* member = GFXPlayer->CreateObject( TEXT("Object") );
			if( member )
			{
				member->SetString( TEXT("label") , TEXT("") );
				member->SetInt( TEXT("index") , 0 );
				member->SetInt( TEXT("type") , 0 );
				member->SetInt( TEXT("deepth") , 0 );
				member->SetBool( TEXT("visible") , TRUE );
				member->SetString( TEXT("MemberName") , player->GetActorName() );
				member->SetString( TEXT("MemberLv") , appItoa( player->GetActorLevel() ) );
				member->SetString( TEXT("MemberClass") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( player->GetClassId() ) ) );
				member->SetString( TEXT("MemberBattleCount") , appItoa(player->GetActorLevel()) );//temporarily substituted; level
				member->SetString( TEXT("MemberGrade") , player->GuildName );//temporarily substituted; guild
				member->SetString( TEXT("MemberRate") , appItoa( MMOPowerManager::getSingleton().GetPower() ) );//temporarily substituted; combat power
				member->SetInt( TEXT("MemberRoleId") , player->ServerID );
				

				teamList->SetElementObject( 0 , member );
			}

			eventCallUpdateTeamList(teamList);
		}
	}
	else if( PageType == BattleTeamPage_5v5 )
	{
		UGFxObject* teamList = GFXPlayer->CreateArray();
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

		//const TArray<BattleTeamMember>& memberList = BattleTeamManager::getSingleton().GetMemberList();

		//if( teamList && player )
		//{
		//	for ( INT idx = 0 ; idx < memberList.Num() ; idx++ )
		//	{
		//		UGFxObject* member = GFXPlayer->CreateObject( TEXT("Object") );
		//		if( member )
		//		{
		//			member->SetString( TEXT("label") , TEXT("") );
		//			member->SetInt( TEXT("index") , 0 );
		//			member->SetInt( TEXT("type") , 0 );
		//			member->SetInt( TEXT("deepth") , 0 );
		//			member->SetBool( TEXT("visible") , TRUE );
		//			member->SetString( TEXT("MemberName") , memberList(idx).PlayerName );
		//			member->SetString( TEXT("MemberLv") , appItoa( memberList(idx).Level ) );
		//			member->SetString( TEXT("MemberClass") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( memberList(idx).ClassID ) ) );
		//			member->SetString( TEXT("MemberBattleCount") , appItoa(0) );		//missing
		//			member->SetString( TEXT("MemberGrade") , appItoa(0) );				//missing
		//			member->SetString( TEXT("MemberRate") , appItoa( memberList(idx).TeamScore ) );
		//			member->SetInt( TEXT("MemberRoleId") , memberList(idx).ServerID );
		//			member->SetBool( TEXT("IsLeader") , ( BattleTeamManager::getSingleton().GetTeamLeadID() == memberList(idx).ServerID ) );

		//			teamList->SetElementObject( idx , member );
		//		}
		//	}

		//	eventCallUpdateTeamList(teamList);
		//}
	}
}


void UMMOUIBattleTeam::BattleTeamOneToOne()
{
	PageType = BattleTeamPage_1v1;
	UpdateTeamInfo();
	UpdateMemberList();
	
	
}

void UMMOUIBattleTeam::BattleTeamFiveToFive()
{
	PageType = BattleTeamPage_5v5;
	UpdateTeamInfo();
	UpdateMemberList();

	
	
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIBattleTeam::ASBattleTeamCheckName(const FString& teamName)
{

}

void UMMOUIBattleTeam::ASBattleTeamLeave()
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{
		CMessageHandler::GetInstance().Send_LeaveBattleTeam( player->ServerID );
	}
}

void UMMOUIBattleTeam::ASBattleTeamAddMember()
{
	ClickAddMember();
}

void UMMOUIBattleTeam::ASBattleTeamRightClickMember( INT roleId , INT x , INT y )
{
	if( PageType == BattleTeamPage_5v5 )
	{
		MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_BattleMember , x , y , roleId , TEXT("") , 0 );
	}

}

void UMMOUIBattleTeam::ASBattleTeamTurnOneToOne()
{
	BattleTeamOneToOne();
}

void UMMOUIBattleTeam::ASBattleTeamTurnFiveToFive()
{
	//determine whether there is a team
	if( false/*BattleTeamManager::getSingleton().HasTeam()*/ )
	{
		BattleTeamFiveToFive();
	}
	else
	{
		ClickOwnPanel( BattleTeamPage_1v1 );

		FString content = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.Cost") ) + UITextParser::HTMLMoneyIconString( MONEYTYPE_02 , CreateTeamCost );
		MMOMsgBoxManager::getSingleton().AddModalInputMsgboxEvent( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.CreateTeamName") ) , 0 , content , GameEvent_BattleTeamCreate );
	}

}
//===============================================================================================================
//===============================================================================================================