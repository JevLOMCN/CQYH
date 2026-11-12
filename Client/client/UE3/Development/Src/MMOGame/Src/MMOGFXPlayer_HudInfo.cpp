#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
//#include "TargetManager.h"
#include "FSMIO.h"
#include "TeamManager.h"
#include "MMOMapDataManager.h"
#include "MMOVipManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_HudInfo)


void UMMOGFXPlayer_HudInfo::Init()
{
	Super::Init();


	check(FALSE && "UMMOGFXPlayer_HudInfo::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_HudInfo);

	MMOUI::GetUIManager()->SetASPkModeChangeDelegate(FScriptDelegate(this, TEXT("ASPkModeChange")));

	MMOUI::GetUIManager()->SetAddFunctionBuffPanelDelegate(FScriptDelegate(this, TEXT("AddFunctionBuffPanel")));
	MMOUI::GetUIManager()->SetDeleteFunctionBuffPanelDelegate(FScriptDelegate(this, TEXT("DeleteFunctionBuffPanel")));

	MMOUI::GetUIManager()->SetAddTargetBuffPanelDelegate(FScriptDelegate(this, TEXT("AddTargetBuffPanel")));
	MMOUI::GetUIManager()->SetDeleteTargetBuffPanelDelegate(FScriptDelegate(this, TEXT("DeleteTargetBuffPanel")));

	MMOUI::GetUIManager()->SetHeroInfoUpdateDataDelegate(FScriptDelegate(this, TEXT("HeroUpdateData")));
}

void UMMOGFXPlayer_HudInfo::UnInit()
{
	Super::UnInit();

	MMOUI::GetUIManager()->SetASPkModeChangeDelegate(FScriptDelegate(EC_EventParm));

	MMOUI::GetUIManager()->SetAddFunctionBuffPanelDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetDeleteFunctionBuffPanelDelegate(FScriptDelegate(EC_EventParm));

	MMOUI::GetUIManager()->SetAddTargetBuffPanelDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetDeleteTargetBuffPanelDelegate(FScriptDelegate(EC_EventParm));

	MMOUI::GetUIManager()->SetHeroInfoUpdateDataDelegate(FScriptDelegate(EC_EventParm));
}

//--------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_HudInfo::ASRightClickTargetInfo(INT x,INT y,const FString& RoleName)
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

void UMMOGFXPlayer_HudInfo::ASMyInfoRightClick( INT x , INT y )
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( player )
	{
		MMOUI::GetUIManager()->delegateOpenRClickMenu(
			ERCMType_TargetInfo , x , y, player->ServerID, player->ActorName , 0 );
	}
}

void UMMOGFXPlayer_HudInfo::ASSetPkMode(int PkMode)
{
	INT _PkModeLevel = 8;
	GConfig->GetInt( TEXT("PkMode") , TEXT("PkModeLevel") , _PkModeLevel , GDefaultGameIni );

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player&&player->GetActorLevel()>=_PkModeLevel){
		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		if (curMapInfo.OpenMapPkMode)   //.HideRoleName)
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

void UMMOGFXPlayer_HudInfo::ASPkModeChange()
{
	int NewPkMode = PlayerInfo->CurrentPkMode+1;
	if(NewPkMode>4)NewPkMode=0;//at most five modes

	ASSetPkMode(NewPkMode);
}

void UMMOGFXPlayer_HudInfo::ASHeroUncall()
{
	CMessageHandler::GetInstance().Send_HeroUncall();
}
void UMMOGFXPlayer_HudInfo::ASHeroBag()
{

}
void UMMOGFXPlayer_HudInfo::FunMode(INT Mode)
{
	INT AiMode = hatStandstill;
	switch(Mode)
	{
	case 1:
		AiMode = hatActive;
		break;
	case 2:
		AiMode = hatPassive;
		break;
	case 3:
		AiMode = hatFollow;
		break;
	case 4:
		AiMode = hatStandstill;
		break;
	}
	CMessageHandler::GetInstance().Send_ChangeHeroAi(AiMode);
}

//========================================================================================================
void UMMOGFXPlayer_HudInfo::ASTeamMemberRightClick( INT x , INT y , INT index )
{	
	MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_TeamMate, x , y , 0 , "" , index );
}

//========================================================================================================
void UMMOGFXPlayer_HudInfo::ASShowTeammateHint( UGFxObject *mc, INT index )
{
	if( mc == NULL ){
		return;
	}

	//
	INT teammateIdx = TeamManager::getSingleton().GetMemberPosByUIPos( index );
	MMOTeamMember* teammate = TeamManager::getSingleton().GetTeamMember( teammateIdx );
	if( teammate == NULL ){
		return;
	}

	//
	FString hintString;
	//class
	hintString = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf( TEXT("%d"), teammate->ClassId ) );
	hintString += TEXT("<br>");

	//location
	if( teammate->Online ){
		FExportMap teammateMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( teammate->MapID );
		hintString += teammateMapInfo.MapDisplayName;
		if( teammateMapInfo.ServerMapType == E_est_Line ){
			FString stringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.SceneLineListString") );
			UINT sceneLineID = 0;
			UINT sceneServerID = 0;
			MMOMapDataManager::ParseServerMapLine( teammate->MapLineID, sceneServerID, sceneLineID );
			hintString += FString::Printf( *stringFormat, sceneLineID );
		}
	}
	else{
		hintString += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Offline") );
	}
	mc->SetString( TEXT("tipsHtml"), hintString );

	//
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

//========================================================================================================
void UMMOGFXPlayer_HudInfo::ASHideTeammateHint()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer_HudInfo::ASTeamHintLeave()
{
	//MMOMsgBoxManager::getSingleton().AddModalMessageBoxEvent( TEXT("Leave party?") , 0 , GameEvent_TeamLeaveAsk );
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{
		MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_LeaveTeam,player->ServerID);
		MMOUI::PostGameEvent(&et);
	}
}

void UMMOGFXPlayer_HudInfo::ASTeamExpand(UBOOL v)
{
	MMOGameEvent_TeamExpand msg(v);
	MMOUI::PostGameEvent( &msg );
}
//========================================================================================================
void UMMOGFXPlayer_HudInfo::ASVIPRightClick()
{
	UMMOUIVip* vipPanel = (UMMOUIVip*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Vip);
	if( vipPanel != NULL ){
		vipPanel->Show( !vipPanel->IsVisible );
	}
}
//========================================================================================================
void UMMOGFXPlayer_HudInfo::ASVIPShowTips( UGFxObject *hintMc )
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
void UMMOGFXPlayer_HudInfo::ASVIPHideTips()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_HudInfo::PostAdvance(FLOAT inDeltaTime )
{
	if(TargetInfo && TargetInfo->IsVisible)
	{
		TargetInfo->TickTime(inDeltaTime);
	}
}