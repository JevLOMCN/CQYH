#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "ActivityManager.h"
#include "UITextParser.h"
#include "MMOMsgBoxManager.h"
#include "TeamManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Activity)


void UMMOGFXPlayer_Activity::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_Activity::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Activity);
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Activity,FALSE);
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_ActivityQuit,FALSE);
}

void UMMOGFXPlayer_Activity::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_Activity::ASSortActivity(INT type)
{
	if(_ActivityPanel)
		_ActivityPanel->SortActivity(type);
}
void UMMOGFXPlayer_Activity::ASShowActivityFilterType(INT nType)
{
	if(_ActivityPanel)
		_ActivityPanel->FilterActivityType(nType);
}
void UMMOGFXPlayer_Activity::ASShowActivityFilterRewardType(INT nType)
{
	if(_ActivityPanel)
		_ActivityPanel->FilterActivityRewardType(nType);
}

void UMMOGFXPlayer_Activity::ASShowTips_ActivityRewardItem(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_ItemSlot, mc);
}
 void UMMOGFXPlayer_Activity::ASHideTips_ActivityRewardItem()
 {
	MMOUI::GetUIManager()->delegateHideTips();
 }
 void UMMOGFXPlayer_Activity::ASShowTips_ActivityRewardItemBar(class UGFxObject* mc,INT nIndex)
 {
	 FString hint = UITextParser::HTMLBeginParagraph();

	 INT nState =ActivityManager::getSingleton().GetRewardState(nIndex);
	 FString indexStr = appItoa(nIndex + 1);
	 FString title = FString(TEXT("MMOGame.Activity.RewardTitle")) + indexStr;
	 hint += MMOUI::ParseLocalizedPropertyPath(title);
	 hint += '\n';
	 if(nState == 0)
	 {
		 FString content = FString(TEXT("MMOGame.Activity.RewardContentNotObtain")) + indexStr;
		 hint += FString::Printf(TEXT("<font color='#aa5500'>%s</font>"), *MMOUI::ParseLocalizedPropertyPath(content));
	 }
	 else if (nState == 1)
	 {
		 FString content = FString(TEXT("MMOGame.Activity.RewardContentObtainNow")) + indexStr;
		 hint += FString::Printf(TEXT("<font color='#14a800'>%s</font>"), *MMOUI::ParseLocalizedPropertyPath(content));
	 }
	 else if (nState == 2)
	 {
		 FString content = FString(TEXT("MMOGame.Activity.RewardContentAlreatyObtain")) + indexStr;
		 hint += FString::Printf(TEXT("<font color='#a8a8a8'>%s</font>"), *MMOUI::ParseLocalizedPropertyPath(content));
	 }
	 
	 hint += UITextParser::HTMLEndParagraph();

	 mc->SetString( TEXT("tipsHtml"), hint );
	 MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
 }
 void UMMOGFXPlayer_Activity::ASHideTips_ActivityRewardItemBar()
 {
	 MMOUI::GetUIManager()->delegateHideTips();
 }

 void UMMOGFXPlayer_Activity::ASClickActivityList(int nActivityID)
 {
	 if(_ActivityPanel)
	 {
		_ActivityPanel->SetActivityTeamList(nActivityID, 0);
		_ActivityPanel->SetActivityDetailInfo(nActivityID);

		if(!TeamManager::getSingleton().HasTeam())
		{
			TeamManager::getSingleton().SetTeamTargetType(etaatActivity);
			TeamManager::getSingleton().SetTeamTargetId(nActivityID);
		}
	 }
 }
void UMMOGFXPlayer_Activity::ASClickActivityFBList(INT nFBId)
{
	if(_ActivityPanel)
	{
		_ActivityPanel->SetActivityTeamList(nFBId, 0);
		_ActivityPanel->SetFBDetailInfo(nFBId);

		if(!TeamManager::getSingleton().HasTeam())
		{
			TeamManager::getSingleton().SetTeamTargetType(etaatDungeon);
			TeamManager::getSingleton().SetTeamTargetId(nFBId);
		}
	}
}
void UMMOGFXPlayer_Activity::ASClickQuitListHeadSort(INT type)
{
	if(_ActivityQuitPanel)
		_ActivityQuitPanel->SortActivity(type);
}
void UMMOGFXPlayer_Activity::ASClickOpenActivityPanel()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_ActivityQuit,FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Activity,TRUE, "");
}

void UMMOGFXPlayer_Activity::ASClickBackGame()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_ActivityQuit,FALSE, "");
}
void UMMOGFXPlayer_Activity::ASClickBackLogin()
{
	MMOUI::GetUIManager()->SendReturnLogin();
}
void UMMOGFXPlayer_Activity::ASClickQuitGame()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_ActivityQuit,FALSE, "");
	MMOUI::GetUIManager()->SendQuitGame();

    //UMMOUIActivityQuitPanel* UI = (UMMOUIActivityQuitPanel*)MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_ActivityQuit);
    //if (UI)
    //    UI->Show(FALSE);
}
void UMMOGFXPlayer_Activity::ASClickQuitGetReward(INT nIndex)
{
	INT nState = ActivityManager::getSingleton().GetRewardState(nIndex);
	if(nState == 1){
		const RewardBarItem* rewardData = ActivityManager::getSingleton().GetRewardBarCondition(nIndex);
		if( rewardData != NULL ){
			CMessageHandler::GetInstance().Send_GetActivityReward(rewardData->nID);
		}
	}
	else if (nState == 0)
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.rewardNoGetHint")),MsgInfoType_Table,TEXT("MMOGame.Activity.rewardNoGetHint"));
	else if (nState == 2)
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.rewardHaveGetHint")),MsgInfoType_Table,TEXT("MMOGame.Activity.rewardHaveGetHint"));
}
void UMMOGFXPlayer_Activity::ASClickGetReward()
{
	for (INT nIndex = 0; nIndex < 5; ++ nIndex)
	{
		INT nState = ActivityManager::getSingleton().GetRewardState(nIndex);
		if(nState == 1)
		{
			//claim only one at a time
			const RewardBarItem* rewardData = ActivityManager::getSingleton().GetRewardBarCondition(nIndex);
			if( rewardData != NULL ){
				CMessageHandler::GetInstance().Send_GetActivityReward(rewardData->nID);
			}
			return;
		}
	}
}

void UMMOGFXPlayer_Activity::ASShowActivityTeamView()
{
	/*if(_ActivityPanel)
	{
	_ActivityPanel->RefreshActivityTeamList();
	}*/
	MMOGameEvent_Activity evt(MMOGameEvent_Activity::EActivity_UpdateActivityTeamList);
	MMOUI::PostGameEvent(&evt);
}

void UMMOGFXPlayer_Activity::ASShowActivityFBView()
{
	if(_ActivityPanel)
	{
		if(ActivityManager::getSingleton().GetActivityFilterType() == EActivityCopy)
		{
			//Dungeon
			_ActivityPanel->SetFBDetailInfo(ActivityManager::getSingleton().GetCurActivityId());
		}
		else
		{
			//Activity
			_ActivityPanel->SetActivityDetailInfo(ActivityManager::getSingleton().GetCurActivityId());
		}
	}
}

void UMMOGFXPlayer_Activity::ASClickActivityTeamList(INT nTeamId)
{
	if(_ActivityPanel)
	{
		_ActivityPanel->ShowActivityTeamTips(FALSE);
	}
	ActivityManager::getSingleton().SetSelectedActivityTeam(nTeamId);
}

void UMMOGFXPlayer_Activity::ASRollOverActivityTeamList(INT nTeamId)
{
	if(_ActivityPanel)
	{
		_ActivityPanel->ShowActivityTeamTips(TRUE);
		_ActivityPanel->UpdateActivityTeamTips(nTeamId);
		_ActivityPanel->AutoSetTeamTipsPosition();
	}
}

void UMMOGFXPlayer_Activity::ASRollOutActivityTeamList(INT nTeamId)
{
	if(_ActivityPanel)
	{
		_ActivityPanel->ShowActivityTeamTips(FALSE);
	}
}

void UMMOGFXPlayer_Activity::ASClickEnterTeamHome()
{

}

void UMMOGFXPlayer_Activity::ASClickTeamManager()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Team, TRUE, "");
}

void UMMOGFXPlayer_Activity::ASClickCreateTeam()
{
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Team, TRUE, "");

	INT nID = ActivityManager::getSingleton().GetCurActivityId();
	if(nID < 0)
		return;
	bool bCanTeam = false;
	if(ActivityManager::getSingleton().GetActivityFilterType() == EActivityCopy)
	{
		const FBActivityData*	pkActivity = ActivityManager::getSingleton().GetFBActivityData(nID);
		if(pkActivity&&pkActivity->nPersonNumMin > 1)
			bCanTeam = true;
	}
	else
	{
		const ActivityData*	pkActivity = ActivityManager::getSingleton().GetActivityData(nID);
		if(pkActivity&&pkActivity->nPersonNumMin > 1)
			bCanTeam = true;
	}
	if(!bCanTeam)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.TargetCannotCreatTeam")));
		return;
	}
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if(!pl)
	{
		return;
	}
	if( !TeamManager::getSingleton().HasTeam() )
	{
		CMessageHandler::GetInstance().Send_EnterTeam( pl->ServerID );
		CMessageHandler::GetInstance().Send_ChangeTeamActivity(TeamManager::getSingleton().GetTeamTargetId(), TeamManager::getSingleton().GetTeamTargetType());
	}
}

void UMMOGFXPlayer_Activity::ASClickEnterTeam()
{
	INT curTeamId = ActivityManager::getSingleton().GetSelectedActivityTeam();
	if(curTeamId > 0)
	{
		ActivityTeam* team = ActivityManager::getSingleton().GetActivityTeamById(curTeamId);
		if(team)
			CMessageHandler::GetInstance().Send_EnterTeam(team->LeaderId);
		else
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.NoTeamChoice")));
	}
}

void UMMOGFXPlayer_Activity::ASClickQuickEnterLimitted()
{

}

void UMMOGFXPlayer_Activity::ASClickQuickEnter()
{

}

void UMMOGFXPlayer_Activity::ASClickInvite()
{

}

void UMMOGFXPlayer_Activity::ASClickChangeTarget()
{

}

void UMMOGFXPlayer_Activity::ASClickLeaveTeam()
{

}
void UMMOGFXPlayer_Activity::ASClickFastJoinTeam(INT nID)
{
	const ActivityTeam* team = ActivityManager::getSingleton().GetRandTeamCurActivity();
	if(team)
		CMessageHandler::GetInstance().Send_EnterTeam(team->LeaderId);
	else
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.NoTeamChoice")));
}
void UMMOGFXPlayer_Activity::ASClickRandJoinTeam(INT nID)
{
	const ActivityTeam* team = ActivityManager::getSingleton().GetRandTeamCurActivity();
	if(team)
		CMessageHandler::GetInstance().Send_EnterTeam(team->LeaderId);
}
void UMMOGFXPlayer_Activity::ASPrevTeamPageClick()
{
	INT curPage = ActivityManager::getSingleton().GetCurActivityTeamPage();
	if(curPage > 0)
	{
		--curPage;
		if(_ActivityPanel)
		{
			_ActivityPanel->SetActivityTeamList(ActivityManager::getSingleton().GetCurActivityId(), curPage);
		}
	}
}

void UMMOGFXPlayer_Activity::ASNextTeamPageClick()
{
	INT curPage = ActivityManager::getSingleton().GetCurActivityTeamPage();
	if(curPage < 98)
	{
		++curPage;
		if(_ActivityPanel)
		{
			_ActivityPanel->SetActivityTeamList(ActivityManager::getSingleton().GetCurActivityId(), curPage);
		}
	}
}
