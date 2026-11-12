#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "TeamManager.h"
#include "MMOFullScreenEffectManager.h"
#include "MMOVipManager.h"
#include "MMOBuffManager.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIMyInfo)

/*
void UMMOUIPlayerInfo::PlayerHPChange(INT NewHP,INT MaxHP)
{
	float HPPercent;
	bool bTailHP;
	TArray<	FASValue> args;
	FASValue asval;
	UGFxObject* tObj;

	if( MaxHP <= 0 || NewHP<=0 )
	{
		HPPercent = 0.01;
	}
	else 
		HPPercent = float(NewHP)/MaxHP ;
	if( LastHP <=  NewHP ) //increased
		bTailHP = false;
	else
		bTailHP = true;
	HPPercent = Clamp(HPPercent,0.01f,1.0f);
	UGFxObject* hpTxt = GetObject( TEXT("HPBar_mc.Hptxt_mc"));
	if(hpTxt)
	{
		hpTxt->SetText(FString::Printf(TEXT("%d"), NewHP));
	}
	asval.Type = AS_Number;
	asval.N = HPPercent;
	args.AddItem( asval );
	asval.Type = AS_Boolean;
	asval.B = bTailHP;
	args.AddItem( asval );
	tObj = GetObject( "HPBar_mc");
	if( tObj != NULL )
		tObj->Invoke( "HPTail",args);
	LastHP = HPPercent;

}
//------------------------------------------------------------------------
void UMMOUIPlayerInfo::PlayerMPChange(INT NewMP,INT MaxMP)
{
	float MPPercent;
	bool bTailMP;
	TArray<	FASValue> args;
	FASValue asval;
	UGFxObject* tObj;

	if( MaxMP <= 0 || NewMP<=0 )
	{
		MPPercent = 0.01;
	}
	else 
		MPPercent = float(NewMP)/MaxMP ;
	if( LastMP <=  NewMP )
		bTailMP = false;
	else
		bTailMP = false;//trail has a bug, changed to off
	MPPercent = Clamp(MPPercent,0.01f,1.0f);
	UGFxObject* mpTxt = GetObject( TEXT("MpBar_mc.mptxt_mc"));
	if(mpTxt)
	{
		mpTxt->SetText(FString::Printf(TEXT("%d"), NewMP));
	}
	asval.Type = AS_Number;
	asval.N = MPPercent;
	args.AddItem( asval );
	asval.Type = AS_Boolean;
	asval.B = bTailMP;
	args.AddItem( asval );
	tObj = GetObject( "MpBar_mc");
	if( tObj != NULL )
		tObj->Invoke( "MPTail",args);
	LastMP = MPPercent;
}
*/


void UMMOUIMyInfo::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_MainPlayerDataChange:
		{
			MMOGameEvent_MainPlayerDataChange* e = (MMOGameEvent_MainPlayerDataChange*)evt;
			switch(e->DataChangeEventType)
			{
			case MMOGameEvent_MainPlayerDataChange::EMSET_HP:
				{
					PlayerHPChange(e->CurrentIntValue, e->MaxIntValue);
				}
				break;
			}
		}
		break;
	case GameEvent_TeamStateChaged:
		{
			UBOOL teamLeader = TeamManager::getSingleton().IsTeamLeader();
			UGFxObject* teamLeaderMc = GetObject( TEXT("teamLeader") );
			if(teamLeaderMc)
			{
				teamLeaderMc->SetVisible(teamLeader);
			}
		}
		break;
	case GameEvent_VipStateChanged:
		{
			MMOGameEvent_OneInt *vipStateEvt = (MMOGameEvent_OneInt*)evt;

			AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( vipStateEvt->IntData );
			if (actor == NULL){
				break;
			}

			//
			AMMOPlayer *pPlayer = Cast<AMMOPlayer>(actor);
			if( pPlayer != NULL && pPlayer->IsMainPlayer() ){
				eVipType curMyVip = MMOVipManager::getSingleton().GetMyCurrentVipType();
				eventCallAsSetVipLevel( curMyVip );
				pPlayer->SetVip(curMyVip);
			}

			MMOGameEvent_UpdatePlayerVipHint evt;
			MMOUI::PostGameEvent(&evt);
		}
		break;
	case GameEvent_UpdateVipLevel:
		{
			eVipType curMyVip = MMOVipManager::getSingleton().GetMyCurrentVipType();
			eventCallAsSetVipLevel( curMyVip );
		}
	}
}
//-----------------------------------------------------
void UMMOUIMyInfo::PlayerHPChange(int NewHP, int MaxHP)
{
	AMMOActor* MainPlayer = MMOFacade::getSingleton().GetMainActor();
	if(MainPlayer){
		TArray<	FASValue> args;
		FASValue asval;
		asval.Type = AS_Number;
		asval.N = MainPlayer->ActorAttributes->GetCurHp();
		args.AddItem( asval );
		asval.Type = AS_Number;
		asval.N = MainPlayer->ActorAttributes->GetMaxHp();
		args.AddItem( asval );
		this->Invoke(TEXT("SetHp"),args);

		if(NewHP>0 && NewHP*10<=MaxHP){
			MMOFullScreenEffectManager::getSingleton().StartEffect(FSET_LowHP);
		}
		else
		{
			MMOFullScreenEffectManager::getSingleton().StopEffect(FSET_LowHP);
		}
		////GFXPlayer->Invoke(TEXT("SetHp"),args);
		//UGFxObject* rootObj = GFXPlayer->GetVariableObject("root");
		//if(rootObj){
		//	rootObj->Invoke( "SetHp",args);
		//}
	}

	//eventSetHpFx(NewHP, MaxHP);
}

void UMMOUIMyInfo::PlayerLevelChange(INT lastLv,INT newLv)
{
	if( GetObject(TEXT( "leveltext" )) )
	{
		GetObject(TEXT( "leveltext" ))->SetText( FString::Printf( TEXT( "%d") , newLv ) );
	}	
}

void UMMOUIMyInfo::PlayerPkModeChange( INT PkMode )
{
	CurrentPkMode = PkMode;
	if( GetObject(TEXT( "btnPkFlagOption" )) )
	{
		TArray<	FASValue> args;
		FASValue asval;
		asval.Type = AS_Number;
		asval.N = PkMode;
		args.AddItem( asval );

		this->Invoke(TEXT("SetPkMode"),args);
		//GetObject(TEXT( "PlayerInfo_mc" ))->Invoke(TEXT("SetPkMode"),args);//     ->GetObject(TEXT("iconPkType_mc"))->GotoAndStopI(PkMode+1);
	}	
}

//-----------------------------------------------------
void UMMOUIMyInfo::PlayerBaseInfoChange()
{
	UGFxObject* Obj = NULL;
	
	Obj = GetObject("PlayerName_mc");
	if( Obj && Player )
	{
		Obj->SetText( Player->GetActorName() );// >ActorName );
	}

	//PlayerLevelChange( player.ActorAttributes.LevelAttributes.Level );
	Obj = GetObject("playerProIcon");
	if( Obj && Player )
	{
		Obj->GotoAndStopI( Player->GetClassId()+1 );

		
		FString path = TEXT("img://MMOUIFaceIcon.Player.class");
		path += appItoa( Player->GetClassId() );
		if( Player->GetSex() == EGT_Male )
		{
			path += TEXT("man");
		}
		else
		{
			path += TEXT("fe");
		}

		UGFxObject* AvatarIcon = Obj->GetObject("roleIcon");
		if(AvatarIcon)
		{
			AvatarIcon->SetString( TEXT("source") , path );
		}
	}
	
	//Vip
	eVipType curMyVip = MMOVipManager::getSingleton().GetMyCurrentVipType();
	eventCallAsSetVipLevel( curMyVip );
}

void UMMOUIMyInfo::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MainPlayerDataChange);
	evts.AddItem(GameEvent_TeamStateChaged);
	evts.AddItem(GameEvent_VipStateChanged);
	evts.AddItem(GameEvent_UpdateVipLevel);
	EventAdapter = new TGameEventAdapter<UMMOUIMyInfo>(this, evts);

	MMOUI::GetUIManager()->SetASPkModeChangeDelegate(FScriptDelegate(this, TEXT("ASPkModeChange")));
	MMOUI::GetUIManager()->SetAddFunctionBuffPanelDelegate(FScriptDelegate(this, TEXT("AddFunctionBuffPanel")));
	MMOUI::GetUIManager()->SetDeleteFunctionBuffPanelDelegate(FScriptDelegate(this, TEXT("DeleteFunctionBuffPanel")));


}

void UMMOUIMyInfo::UnInit()
{
	Super::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);
	MMOUI::GetUIManager()->SetASPkModeChangeDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetAddFunctionBuffPanelDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetDeleteFunctionBuffPanelDelegate(nullDelgate);
}

void UMMOUIMyInfo::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

	if(show)
	{
		MMOGameEvent_Any evt(GameEvent_RequestMainPlayerData);
		MMOUI::PostGameEvent(&evt);

		if(bFirstOpen)
		{
			bFirstOpen = FALSE;
			AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if(pMainPlayer&&pMainPlayer->ActorAttributes)
			{
				PlayerBaseInfoChange();
				PlayerLevelChange(0,pMainPlayer->ActorAttributes->LevelAttributes.Level);
				PlayerPkModeChange(pMainPlayer->ActorAttributes->PkMode);
				PlayerHPChange(pMainPlayer->ActorAttributes->GetCurHp(),pMainPlayer->ActorAttributes->GetCurMp());

			}
		}
		MMOFacade::getSingleton().GetMMOBuffManager()->RefreshSlotBuffEx(MMOFacade::getSingleton().GetMainPlayer()->ServerID);
	}
	else
	{
		MMOFacade::getSingleton().GetMMOBuffManager()->HideSlotBuffEx();
	}
}


void UMMOUIMyInfo::ASMyInfoRightClick( INT x , INT y )
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( player )
	{
		MMOUI::GetUIManager()->delegateOpenRClickMenu(
			ERCMType_TargetInfo , x , y, player->ServerID, player->GetActorName() , 0 );
	}
}


void UMMOUIMyInfo::ASSetPkMode(int PkMode)
{
	INT _PkModeLevel = 8;
	GConfig->GetInt( TEXT("PkMode") , TEXT("PkModeLevel") , _PkModeLevel , GDefaultGameIni );

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player&&player->GetActorLevel()>=_PkModeLevel){
		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		if (curMapInfo.OpenMapPkMode) //if (curMapInfo.HideRoleName)
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage7")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage7"));
		}
		else{
			CMessageHandler::GetInstance().Send_PkMode((UINT8)PkMode);
		}
	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage3")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage3"));
	}
}

void UMMOUIMyInfo::ASPkModeChange()
{
	int NewPkMode = CurrentPkMode+1;
	if(NewPkMode>4)NewPkMode=0;//at most five modes

	ASSetPkMode(NewPkMode);
}

UBOOL UMMOUIMyInfo::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	//if (strHoverControllerName != "btnPkFlagOption" &&
	//	strHoverControllerName != "hpFill_mc"		&&
	//	strHoverControllerName != "playerProIcon"	&&
	//	strHoverControllerName != "leveltext"		)

	if(	strHoverControllerName=="bk_mc" || 
		strHoverControllerName=="PlayerName_mc"
		)
	{
		return TRUE;
	}
	
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}

//========================================================================================================
void UMMOUIMyInfo::ASVIPRightClick()
{
	UMMOUIVip* vipPanel = (UMMOUIVip*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Vip);
	if( vipPanel != NULL ){
		vipPanel->Show( !vipPanel->IsVisible );
	}
}
//========================================================================================================
void UMMOUIMyInfo::ASVIPShowTips( UGFxObject *hintMc )
{
	if( hintMc == NULL ){
		return;
	}

	MMOVipManager &mmoVipMgr = MMOVipManager::getSingleton();

	eVipType curMyVip = mmoVipMgr.GetMyCurrentVipType();
	//
	FString vipHintStr;
	if( curMyVip != eVIPTyp_None ){
		FString vipName;
		UINT leftDay;
		UINT leftHour;
		mmoVipMgr.GetVipDescriptionString( curMyVip, vipName, leftDay, leftHour );

		vipHintStr = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.HeadVipTipsText") ), *vipName );

		if( leftDay > 0 ){
			vipHintStr += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.VipLeftDay") ), leftDay );
		}

		if( leftHour > 0 ){
			vipHintStr += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.VipLeftHour") ), leftHour );
		}
	}
	else{
		vipHintStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.HeadVipTipsTextNoVip") );
	}

	//
	hintMc->SetString( TEXT("tipsHtml"), vipHintStr );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_AutoSizeTips, hintMc );
}
//========================================================================================================
void UMMOUIMyInfo::ASVIPHideTips()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
