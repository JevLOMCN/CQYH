#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FSMIO.h"
#include "CommonGameEvents.h"
#include "MMOGuideManager.h"
#include "MMOBuffManager.h"
#include "FriendManager.h"
#include "TeamManager.h"
#include "MMOGuildData.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUITargetInfo)


void UMMOUITargetInfo::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_TargetChange);
	evts.AddItem(GameEvent_ActorDestroyed);
	evts.AddItem(GameEvent_ActorDestroyed);
	EventAdapter = new TGameEventAdapter<UMMOUITargetInfo>(this, evts);

	if( GFXPlayer ){
		GFXPlayer->AddPostAdvanceDelegate( FScriptDelegate(this, TEXT("PostAdvance")) );
	}
	MMOUI::GetUIManager()->SetAddTargetBuffPanelDelegate(FScriptDelegate(this, TEXT("AddTargetBuffPanel")));
	MMOUI::GetUIManager()->SetDeleteTargetBuffPanelDelegate(FScriptDelegate(this, TEXT("DeleteTargetBuffPanel")));

}

void UMMOUITargetInfo::UnInit()
{
	Super::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);
	MMOUI::GetUIManager()->SetAddTargetBuffPanelDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetDeleteTargetBuffPanelDelegate(nullDelgate);
	if( GFXPlayer ){
		GFXPlayer->RemovePostAdvanceDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUITargetInfo::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
}

void UMMOUITargetInfo::OnFocusLost()
{
	UMMOUIBase::OnFocusLost();

	//MMOUI::GetUIManager()->ShowPanel( MMOUI::GetUIManager()->Panel_RClickMenu , FALSE );
}

//------------------------------------------------------------------------
void UMMOUITargetInfo::NPCHpChange(class ANPC* npc,INT hp)
{

//	AMMOActor* TargetActor = MMOTargetManager::getSingleton().GetTargetMMOActor();
//	AMMOActor* SelectActor = MMOTargetManager::getSingleton().GetSelectMMOActor();

//	AMMOActor* TargetActor = FSMIO::getSingleton().Target.GetTargetMMOActor();
	AMMOActor* SelectActor = FSMIO::getSingleton().Target.GetSelectMMOActor();
	
	if( npc && SelectActor && (SelectActor==npc) )
	{
		UpdateNPCInfo( npc , true,hp );
	}
	
}
//------------------------------------------------------------------------
void UMMOUITargetInfo::OtherPlayerHpChange(class AMMOPlayer* pl,INT HP)
{
	//AMMOActor* TargetActor = MMOTargetManager::getSingleton().GetTargetMMOActor();
	//AMMOActor* SelectActor = MMOTargetManager::getSingleton().GetSelectMMOActor();

	AMMOActor* SelectActor = FSMIO::getSingleton().Target.GetSelectMMOActor();


	if( pl && SelectActor && (SelectActor == pl) )
	{
		UpdateNPCInfo( pl , true ,HP);
	}
}
//------------------------------------------------------------------------
void UMMOUITargetInfo::UpdateNPCInfo(class AMMOActor* Actor,UBOOL Direct,int hp)
{
		if( Actor == NULL)
		{
			return;
		}

		INT curHP = Actor->ActorAttributes->GetCurHp();
		INT maxHP = Actor->ActorAttributes->GetMaxHp();
		static INT nLastActorID = 0;
		if(hp == 0)
		{
			//Record ID of the last dead target monster
			nLastActorID = Actor->ServerID;
		}

		//Since the server did not sync npc attributes at the start, the initial value is 0,
		if(curHP==0 && (nLastActorID !=  Actor->ServerID)&& Actor->eventIsDead()==FALSE)
		{
			curHP = maxHP;
		}
		
		FLOAT HPPercent = 0.01;

		//debugf( TEXT("UMMOUITargetInfo::UpdateNPCInfo-----%s  Hp=%d MaxHp=%d  percent=%d "), *(Actor->ActorName),curHP,maxHP,Actor->ActorAttributes->HpPercent );

		if( LastHP == curHP && !Direct )  //no need to update constantly
		{
			return;
		}

		if( LastHP >= curHP )  //no animation when it increases
		{
			Direct = true;
		}


		if(curHP<0)
		{
			
			HPPercent = FLOAT(Actor->ActorAttributes->HpPercent)/100.0;
		}
		else if(maxHP == 0)
		{
			HPPercent = 0.001;
		}
		else
		{
			HPPercent = FLOAT(curHP)/maxHP;
		}

		if(HPPercent < 0.001)
		{
			HPPercent = 0.001;
		}
		if( HPPercent >1)
		{
			HPPercent = 1;
		}

		bool bShowPercent = true;
		ANPC* tNpc = Cast<ANPC>(Actor);
		if(tNpc)
		{
			bShowPercent = false;
		}
		else if(FriendManager::getSingleton().IsMyFriend(Actor->ServerID)
			|| MMOGuildData::getSingleton().IsGuildFullMember(Actor->ServerID) || TeamManager::getSingleton().GetMemberById(Actor->ServerID))
		{
			bShowPercent = false;
		}

		TArray<	FASValue> args;
		args.AddZeroed(5);
		args(0).Type = AS_Number;
		args(0).N = HPPercent;

		args(1).Type = AS_Boolean;
		args(1).B = !Direct;

		args(2).Type = AS_Number;
		args(2).N = curHP;

		args(3).Type = AS_Number;
		args(3).N = maxHP;

		args(4).Type = AS_Boolean;
		args(4).B = bShowPercent;

		Invoke( TEXT("HPTail"),args);
		LastHP = curHP;

		UGFxObject* LvTf =GetObject( TEXT("tf_level") );
		if( LvTf )
		{
			if(bShowPercent)
			{
				LvTf->SetText(TEXT(""));
			}
			else
			{
				ANPC* tNpc = Cast<ANPC>(Actor);
				if(tNpc)
				{
					LvTf->SetText( FString::Printf( TEXT("%d") , tNpc->Level ) );
				}
				else
				{
					LvTf->SetText( FString::Printf( TEXT("%d") , Actor->ActorAttributes->LevelAttributes.Level ) );
				}
			}
		}
}

//------------------------------------------------------------------------
FString UMMOUITargetInfo::GetAffixString(struct FAi_Affix affix )
{
	if( affix.AffixName.IsEmpty() )
	{
		return FString::Printf( TEXT("ErrorName%d ") , affix.ID );
	}
	else
	{
		return affix.AffixName + TEXT(" ");		
	}
}


//------------------------------------------------------------------------
void UMMOUITargetInfo::TargetActorChange(BYTE type,class AMMOActor* actor)
{

	if( actor == NULL )
	{
		return;
	}

	LastHP =-99;

	//UGFxObject* TypeTf =GetObject( TEXT("tf_type") );
	//UGFxObject* LvTf =GetObject( TEXT("tf_level") );
	//UGFxObject* AvatarButton =  GetObject( TEXT("AvatarButton_mc") );
	//UGFxObject* AvatarIcon = NULL;
	//if(AvatarButton)
	//	AvatarIcon = AvatarButton->GetObject( TEXT("RoleIcon") );
	//UGFxObject* TargetName = GetObject(TEXT("enemyname_mc"));

	ANPC* tNpc = NULL;
	AMMOPlayer* tPlayer = NULL;
	FString PreAffix;
	FString PostAffix;
	INT NpcTextColor = 0xffffff;

	if(AffixText)
	{
		AffixText->SetVisible(FALSE);
	}

	switch(type)
	{
		//Select another player
	case TargetActor_TYPE_OhterPlayer:
		tPlayer = Cast<AMMOPlayer>( actor );
		if( tPlayer && GFXPlayer )
		{
			GotoAndStop(TEXT("npc"));

			UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
			if(data)
			{
				data->SetString( TEXT("frameText") , TEXT("npc") );
				data->SetString( TEXT("lvText") , appItoa(tPlayer->GetActorLevel()) );
				data->SetString( TEXT("typeText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Player") ) );
				data->SetInt( TEXT("textColor") , -1 );
				FString path = FString( TEXT("img://MMOUIFaceIcon.Player.class") ) + appItoa(tPlayer->ActorAttributes->ClassType);
				if( tPlayer->GetSex() == EGT_Male )
				{
					path += TEXT("man");
				}
				else
				{
					path += TEXT("fe");
				}

				data->SetString( TEXT("targetName") , tPlayer->GetActorName() );// >RoleName );
				data->SetString( TEXT("avatarIconSource") , path );
				eventCallSetTargetInfo( data );
			}			
		}
		break;
	case TargetActor_TYPE_NPC:
		{
			GotoAndStop(TEXT("npc"));

			UNPCTemplate* npcTemp=NULL; 
			HintConfig* pHint =NULL;
			tNpc = Cast<ANPC>( actor );
			if( tNpc ){
				npcTemp = MMOFacade::getSingleton().FindNPCTemplate(tNpc->NPCID);

				pHint = MMOFacade::getSingleton().GetHintInfo(EActorClassType(tNpc->GetActorType()),ENPC_QUALITY(tNpc->Quality) );

				if( tNpc->Quality < MMOUI::GetUIManager()->MonsterQuality.Num() )
				{
					NpcTextColor = MMOUI::GetUIManager()->MonsterQuality( tNpc->Quality );
				}		
			}
			
			if( tNpc && npcTemp && GFXPlayer )
			{			
				UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
				if(data)
				{
					data->SetString( TEXT("frameText") , TEXT("npc") );
					data->SetString( TEXT("lvText") , appItoa( tNpc->GetActorLevel() ) );
					data->SetString( TEXT("typeText") , TEXT("") );
					data->SetInt( TEXT("textColor") , -1 );
					FString path = TEXT("img://");
					FString iconName = npcTemp->GetIconName();

					if( iconName.IsEmpty() )
					{							
						path += TEXT("MMOUIFaceIcon.npc.default");
					}
					else
					{
						path += iconName;
					}
					data->SetString( TEXT("targetName") , tNpc->DisplayName );
					data->SetString( TEXT("avatarIconSource") , path );
					eventCallSetTargetInfo( data );
				}
			}
		}
		break;
	case TargetActor_TYPE_MONSTER:
		{
			GotoAndStop(TEXT("monster"));

			tNpc = Cast<ANPC>( actor );

			//AffixText = GetObject("affixText");
			//if(AffixText)
			//{
			//	AffixText->SetVisible(TRUE);

			//	for ( INT k = 0 ; k < tNpc->AffixList.Num() ; k++ )
			//	{
			//		INT affixId = tNpc->AffixList(k);

			//		AffixClientInfo* affix = MMOFacade::getSingleton().FindAffix( affixId );
			//		if ( affix )
			//		{
			//			switch( affix->Type )
			//			{
			//			case atPrefix:								
			//				PreAffix += affix->Name ;								
			//				break;
			//			case atPostfix:
			//				PostAffix += affix->Name + TEXT(" ");
			//				break;
			//			}
			//		}
			//	}
			//	AffixText->SetText(PostAffix);
			//}
			

			if( tNpc && GFXPlayer )
			{			
				UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
				if(data)
				{

					FString monsterString = TEXT("MMOGame.GamePlay.Monster");
					monsterString += appItoa(tNpc->Quality);
					
					data->SetString( TEXT("frameText") , TEXT("monster") );
					data->SetString( TEXT("lvText") , appItoa( tNpc->GetActorLevel() ) );
					data->SetString( TEXT("typeText") , MMOUI::ParseLocalizedPropertyPath( monsterString ) );
										
					data->SetString( TEXT("targetName") , PreAffix + tNpc->DisplayName );
					data->SetInt( TEXT("textColor") , NpcTextColor );
					data->SetString( TEXT("avatarIconSource") , TEXT("") );
					eventCallSetTargetInfo( data );
				}
			}
			
		}
		break;
	case TargetActor_TYPE_BOSS:
		GotoAndStop(TEXT("boss"));
		tNpc = Cast<ANPC>( actor );		
		if( tNpc && GFXPlayer )
		{			
			UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
			if(data)
			{
				data->SetString( TEXT("frameText") , TEXT("boss") );
				data->SetString( TEXT("lvText") , appItoa( tNpc->GetActorLevel() ) );
				data->SetString( TEXT("typeText") , TEXT("") );
				data->SetInt( TEXT("textColor") , -1 );
				data->SetString( TEXT("targetName") , tNpc->DisplayName );
				data->SetString( TEXT("avatarIconSource") , TEXT("") );
				eventCallSetTargetInfo( data );
			}
		}
		
		break;
	}

	//if( TargetName )
	//{
	//	if(  Cast<ANPC>(actor) != NULL )
	//	{
	//		TargetName->SetInt( TEXT("textColor") , NpcTextColor );
	//		TargetName->SetText( PreAffix + Cast<ANPC>(actor)->DisplayName );
	//	}
	//	else
	//	{
	//		TargetName->SetText( actor->RoleName );
	//	}
	//}

	InfoType = type;


	UpdateNPCInfo( actor , true ,-1);
	MMOFacade::getSingleton().GetMMOBuffManager()->UpdateTargetBuff(actor->ServerID);
	//if( npc != none)
	//{
	//	GetVariableObject("root.enemyinfo_mc").GetObject("NpcInfoIcon_mc").GotoAndStopI( npc.NPCIcon +2);
	//}

	if (!IsVisible)
		Show(true);

	// add by zhangrui look up other characters' info here just in case (right-click menu operations may need this info)
	if (actor->IsA(AMMOPlayer::StaticClass()))
	{
		MMOFacade::getSingleton().RequestPlayerBaseInfo(actor->ServerID);
	}
	
}

void UMMOUITargetInfo::PostAdvance ( FLOAT DeltaTime )
{
	//Super::PostAdvance(DeltaTime);

	if(IsVisible){
		TickTime(DeltaTime);
	}
}

void UMMOUITargetInfo::TickTime(FLOAT tickTime)
{
	if(DisappearTime>0.0f)
	{
		DisappearTime-=tickTime;
		if(DisappearTime<=0.0f)
		{
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HudInfo_TargetInfo, FALSE, "");
		}
		else if(DisappearTime<=1.0f)
		{
			eventCallSetAlpha( DisappearTime );
		}
	}
}

void ProcessCurrentTarget(class AMMOActor* actor, UMMOUITargetInfo* ui)
{
	if(ui==NULL)return;

	if(ui->LastMMOActor!=actor)
	{
		ui->LastMMOActor = actor;
		ui->eventDeleteAllBuff();
	}

	if( actor==NULL )
	{
		ui->DisappearTime = 1.0f;
		//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HudInfo_TargetInfo, FALSE, "");
		////ui->Show(FALSE);
	}
	else if( actor->IsA(ANPC::StaticClass() ) )
	{
		ui->DisappearTime = -1.0f;
		ui->eventCallSetAlpha(1.0f);

		ANPC* npc = Cast<ANPC>(actor);
		if( npc->IsMonster()  )
		{
			if( npc->ActorType == EACT_Boss)
			{
				//debugTargetf(TEXT("--------- TargetActorChange TargetActor_TYPE_BOSS"));
				ui->TargetActorChange( TargetActor_TYPE_BOSS, npc );
			}
			else
			{
				//debugTargetf(TEXT("--------- TargetActorChange TargetActor_TYPE_MONSTER"));
				ui->TargetActorChange( TargetActor_TYPE_MONSTER, npc );
			}
		}
		else
		{

			//debugTargetf(TEXT("--------- TargetActorChange TargetActor_TYPE_NPC"));
			ui->TargetActorChange( TargetActor_TYPE_NPC, npc );
		}
	}
	else if( actor->IsA(AMMOPlayer::StaticClass()))
	{
		AMMOPlayer* player = Cast<AMMOPlayer>( actor );
		if( !player->IsMainPlayer() )
		{
			ui->DisappearTime = -1.0f;
			ui->eventCallSetAlpha(1.0f);
			//debugTargetf(TEXT("--------- TargetActorChange TargetActor_TYPE_OhterPlayer"));
			if(ui)
			{
				ui->TargetActorChange( TargetActor_TYPE_OhterPlayer, actor );
			}
		}
	}
}

void UMMOUITargetInfo::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt->GetEventType() == GameEvent_TargetChange)
	{
		MMOGameEvent_TargetEvent* targetEvt = (MMOGameEvent_TargetEvent*)evt;
		if(targetEvt == NULL)
		{
			return;
		}

		switch(targetEvt->EventType)
		{
		case MMOGameEvent_TargetEvent::ETCT_CurrentTarget:
			{
				//// Do nothing during cutscene playback, otherwise red border and target portrait UI will show
				//AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
				//if (mainPlayer && !mainPlayer->IsInMatinee)
				//{
				//	CurrentMMOActor = targetEvt->CurrentTarget;
				//	if(CurrentMMOActor)
				//		ProcessCurrentTarget(CurrentMMOActor, this);
				//	else
				//		ProcessCurrentTarget(DesiredMMOActor, this);
				//}
			}
			break;

		case MMOGameEvent_TargetEvent::ETCT_DesiredTarget:
			{
				DesiredMMOActor = targetEvt->DesiredTarget;
				//CurrentMMOActor = targetEvt->CurrentTarget;
				//if(CurrentMMOActor)
				//	ProcessCurrentTarget(CurrentMMOActor, this);
				//else
					ProcessCurrentTarget(DesiredMMOActor, this);
			}
			break;
		}
	}
	else if(evt->GetEventType() == GameEvent_ActorDestroyed)
	{
		MMOGameEvent_ActorDestroyed* destroyEvt = (MMOGameEvent_ActorDestroyed*)(evt);
		if(destroyEvt)
		{
			if(destroyEvt->Actor && destroyEvt->Actor == DesiredMMOActor )
			{
				DesiredMMOActor = NULL;
				//if(CurrentMMOActor==NULL)
				{
					ProcessCurrentTarget(DesiredMMOActor, this);
				}
			}
			//if(destroyEvt->Actor && destroyEvt->Actor == CurrentMMOActor )
			//{
			//	CurrentMMOActor = NULL;
			//	ProcessCurrentTarget(DesiredMMOActor, this);
			//}

		}
	}
	else if(evt->GetEventType() == GameEvent_OtherPlayerLevelUp)
	{
		//AMMOActor* targetActor = Cast<AMMOActor>(MMOTargetManager::getSingleton().GetFocusActor());
		AMMOActor* targetActor =  FSMIO::getSingleton().Target.GetSelectMMOActor();// Cast<AMMOActor>(MMOTargetManager::getSingleton().GetFocusActor());


		
		MMOGameEvent_OtherPlayerLevelUp* levelup = (MMOGameEvent_OtherPlayerLevelUp*)(evt);
		if( levelup && targetActor && levelup->roleID == targetActor->ServerID )
		{
			UpdateNPCInfo( targetActor , true ,-1);
		}
	}
}

void UMMOUITargetInfo::OnLeave()
{
	CurrentMMOActor =NULL;
	DesiredMMOActor =NULL;
}


void UMMOUITargetInfo::ASRightClickTargetInfo2(INT x,INT y,const FString& RoleName)
{
	AMMOActor* targetActor = FSMIO::getSingleton().Target.GetDesireMMOActor();
	//	AMMOActor* targetActor = Cast<AMMOActor>(MMOTargetManager::getSingleton().GetDesireActor());		//TargetManager::getSingleton().GetCurrentTarget();
	//MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_TargetInfo , x , y, 0, RoleName , 0 );
	if( targetActor )
	{

		MMOUI::GetUIManager()->delegateOpenRClickMenu(
			ERCMType_TargetInfo , x , y, targetActor->ServerID, RoleName, 0 );

	}
}



UBOOL UMMOUITargetInfo::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if(	strHoverControllerName=="bk_mc" )
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}