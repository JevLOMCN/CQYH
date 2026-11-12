#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "ActivityManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "TeamManager.h"
#include "MMOMapDataManager.h"
#include "ScaleformEngine.h"
#include "MMOGuideManager.h"
#include "client_option_def.h"

IMPLEMENT_CLASS(UMMOUIActivityViewPanel)


void UMMOUIActivityViewPanel::Init()
{
	Super::Init();
	eventInitActivityViewReward();
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_ActivityActive);
	evts.AddItem(GameEvent_ActivityRewardState);
	evts.AddItem(GameEvent_Activity);
	evts.AddItem(GameEvent_PlayerBaseInfo);
	EventAdapter = new TGameEventAdapter<UMMOUIActivityViewPanel>(this, evts);
}

void UMMOUIActivityViewPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if(v)
	{
		UpdateHuoYueValue();
		UpdateRewardBarInfo();
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.ActivityTitle")));
		if(nFilterReward != 0)
			FilterActivityRewardType(nFilterReward);
		else
			FilterActivityRewardType(7);

		eventCallShowRewardBar(CMessageHandler::GetInstance().GetClientOption() & CLIENT_OPTION_BIT_ACTIVITY_VALUE);
		
		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowActivityPanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
}

void UMMOUIActivityViewPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if (!IsVisible)
	{
		return;
	}

	if(gameEvent == NULL){
		return;
	}

	switch(gameEvent->GetEventType())
	{
	case GameEvent_ActivityActive:
		{
			UpdateHuoYueValue();
		}break;
	case GameEvent_ActivityRewardState:
		{
			UpdateRewardBarInfo();
		}break;
	case GameEvent_Activity:
		{
			MMOGameEvent_Activity *evt = (MMOGameEvent_Activity*)gameEvent;
			if(evt->EventType == MMOGameEvent_Activity::EActivity_UpdateActivityTeamList)
			{
				RefreshActivityTeamList();
				eventCallUpdateActivityTeamButtonState(TeamManager::getSingleton().HasTeam());
				eventCallUpdateActivityTeamPageInfo(99, ActivityManager::getSingleton().GetCurActivityTeamPage() + 1);
			}
			else if(evt->EventType == MMOGameEvent_Activity::EActivity_ForceRequireActivityTeam)
			{
				SetActivityTeamList(ActivityManager::getSingleton().GetCurActivityId(), ActivityManager::getSingleton().GetCurActivityTeamPage(), TRUE);
			}
		}break;
	case GameEvent_PlayerBaseInfo:
		{
			UpdateActivityTeamTips(ActivityManager::getSingleton().GetActivityTipsTeamId());
			RefreshActivityTeamList();
		}break;
	case GameEvent_EnterScene:
		{
			RefreshActivityList();
		}break;
	}
}
void UMMOUIActivityViewPanel::UpdateActivityState(INT nActivityID)
{

}
void UMMOUIActivityViewPanel::SortActivity(INT nType)
{
	if(nCurSortType == nType)
		nSortAdd = nSortAdd?0:1;
	else
	{
		nCurSortType = nType;
		nSortAdd = 0;
	}
	ActivityManager::getSingleton().SortActivityList(nCurSortType,nSortAdd);
	RefreshActivityList();
}

void UMMOUIActivityViewPanel::FilterActivityType(INT nType)
{
	nFilterType = nType;
	ActivityManager::getSingleton().FilterActivityType(nFilterType,nFilterReward);
	RefreshActivityList();

}

void UMMOUIActivityViewPanel::FilterActivityRewardType(INT nType)
{
	nFilterReward = nType;
	ActivityManager::getSingleton().FilterActivityType(nFilterType,nFilterReward);
	RefreshActivityList();
}

void UMMOUIActivityViewPanel::RefreshActivityList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == 0)
		return;
	ActivityManager::getSingleton().SetActivityList(GFXPlayer,dataProvider);
	eventCallUpdateActivity(dataProvider);
	SetActivityDetailInfo(ActivityManager::getSingleton().GetCurActivityId());
}
void UMMOUIActivityViewPanel::SetActivityDetailInfo(INT nActivityID)
{
	UGFxObject* pkObject = GFXPlayer->CreateObject("UI_ActivityWidget.ActivityAS3.ActivityDetailInfo");
	if(pkObject == 0)
		return;
	ActivityManager::getSingleton().SetActivityDetailInfo(GFXPlayer,pkObject,nActivityID);
	UpdateRewardItemInfo(nActivityID);
	eventCallUpdateActivityDetailInfo(pkObject);
}
void UMMOUIActivityViewPanel::SetFBDetailInfo(INT nFBID)
{
	UGFxObject* pkObject = GFXPlayer->CreateObject("UI_ActivityWidget.ActivityAS3.ActivityDetailInfo");
	if(pkObject == 0)
		return;
	ActivityManager::getSingleton().SetFBDetailInfo(GFXPlayer,pkObject,nFBID);
	UpdateRewardItemInfo(nFBID);
	eventCallUpdateActivityDetailInfo(pkObject);
}
void UMMOUIActivityViewPanel::UpdateRewardBarInfo()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == 0)
		return;
	ActivityManager::getSingleton().SetRewardBarInfo(GFXPlayer,dataProvider);
	eventCallUpdateActivityRewardBarInfo(dataProvider);
}
void UMMOUIActivityViewPanel::UpdateHuoYueValue()
{
	INT nCurValue = ActivityManager::getSingleton().GetActiveValue();
	INT nMaxValue = ActivityManager::getSingleton().GetMaxActiveValue();
	if(nCurValue > nMaxValue)
		nMaxValue = nCurValue;
	eventCallUCUpdateHuoyueValue(nCurValue,nMaxValue);
}
void UMMOUIActivityViewPanel::UpdateRewardItemInfo(INT nActivityID)
{
	const ActivityData* pkActivity = ActivityManager::getSingleton().GetActivityData(nActivityID);
	if (!pkActivity)
	{
		for (INT i = 0; i < rewardList.Num(); ++ i)
		{
			if (rewardList(i))
			{
				GFXPlayer->SetItemSlotData(rewardList(i), NULL, -1, -1, 0);
			}
		}

		return;
	}

	for (INT i = 0; i < 8 && i < rewardList.Num(); ++ i)
	{
		if (rewardList(i))
		{
			INT itemTempId = pkActivity->nRewardInfo.nArrItemID[i];
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(itemTempId);
			if (itemTempId >= 0 && itemTemp)
			{
				GFXPlayer->SetItemSlotData(rewardList(i), itemTemp, -1, -1, -1);
			}
			else
			{
				GFXPlayer->SetItemSlotData(rewardList(i), NULL, -1, -1, 0);
			}
		}
	}
}

void UMMOUIActivityViewPanel::SetActivityTeamList(INT nActivityID, INT nPage, UBOOL bForce)
{
	//query
	UINT dwActivityType = 0;
	if(ActivityManager::getSingleton().GetActivityFilterType() == EActivityCopy)
	{
		dwActivityType = 2;
	}
	else
	{
		dwActivityType = 1;
	}


	if(ActivityManager::getSingleton().GetCurActivityId() != nActivityID || ActivityManager::getSingleton().GetCurActivityTeamPage() != nPage || bForce)
	{
		if(nActivityID > 0&&nPage >= 0)
		CMessageHandler::GetInstance().Send_QueryActivityTeamList(nActivityID, nPage, dwActivityType);
		else
		{
			nPage = 0;
			//clear UI
			ActivityManager::getSingleton().ClearActivityTeamList();
			MMOGameEvent_Activity evt(MMOGameEvent_Activity::EActivity_UpdateActivityTeamList);
			MMOUI::PostGameEvent(&evt);
		}

		ActivityManager::getSingleton().SetCurActivityId(nActivityID);
		ActivityManager::getSingleton().SetCurActivityTeamPage(nPage);
	}
	else
	{
		MMOGameEvent_Activity evt(MMOGameEvent_Activity::EActivity_UpdateActivityTeamList);
		MMOUI::PostGameEvent(&evt);
	}
}

void UMMOUIActivityViewPanel::RefreshActivityTeamList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == 0)
		return;
	ActivityManager::getSingleton().SetActivityTeamInfo(GFXPlayer,dataProvider, 0, 0);
	eventCallUpdateActivityTeamList(dataProvider);
}

void UMMOUIActivityViewPanel::ShowActivityTeamTips(UBOOL bShow)
{
	eventCallShowActivityTeamTips(bShow);
}

void UMMOUIActivityViewPanel::UpdateActivityTeamTips(INT nTeamId)
{
	UGFxObject* dataProvider = GFXPlayer->CreateObject("Object");
	ActivityManager::getSingleton().SetActivityTeamMemberInfo(GFXPlayer,dataProvider,nTeamId);
	eventCallUpdateActivityTeamTips(dataProvider);
}

void UMMOUIActivityViewPanel::AutoSetTeamTipsPosition()
{
	UGFxObject* tips_mc = GetObject(TEXT("activity_team_tips"));
	FIntPoint tipPos = MMOFacade::GetMouseCursorPosition();
	 
    INT ActualHeight = tips_mc->GetInt( TEXT("height") ) ;
	INT ActualWidth  = tips_mc->GetInt( TEXT("width") );

	INT px = GetInt(TEXT("x"));
	INT py = GetInt(TEXT("y"));

	INT ScreenSizeX = FGFxEngine::GetEngine()->GetRenderViewport()->GetSizeX() - 20;
	INT ScreenSizeY = FGFxEngine::GetEngine()->GetRenderViewport()->GetSizeY() - 20;

	if( tipPos.Y + ActualHeight > ScreenSizeY )
	{
		tipPos.Y = tipPos.Y - ActualHeight;
		if (tipPos.Y < 0)
		{
			tipPos.Y = ScreenSizeY - ActualHeight;
		}
	}
	else
	{
		tipPos.Y += 20;
	}

	if( tipPos.X + ActualWidth > ScreenSizeX )
	{
		tipPos.X = tipPos.X - ActualWidth - 20;
	}
	else
	{
		tipPos.X += 25;
	}

	tips_mc->SetPosition(tipPos.X - px, tipPos.Y - py);
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIActivityViewPanel::ASSortActivity(INT type)
{
	SortActivity(type);
}
void UMMOUIActivityViewPanel::ASShowActivityFilterType(INT nType)
{
	FilterActivityType(nType);
}
void UMMOUIActivityViewPanel::ASShowActivityFilterRewardType(INT nType)
{
	FilterActivityRewardType(nType);
}

void UMMOUIActivityViewPanel::ASShowTips_ActivityRewardItem(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_ItemSlot, mc);
}
void UMMOUIActivityViewPanel::ASHideTips_ActivityRewardItem()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOUIActivityViewPanel::ASShowTips_ActivityRewardItemBar(class UGFxObject* mc,INT nIndex)
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
void UMMOUIActivityViewPanel::ASHideTips_ActivityRewardItemBar()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUIActivityViewPanel::ASClickActivityList(int nActivityID)
{
	SetActivityTeamList(nActivityID, 0);
	SetActivityDetailInfo(nActivityID);

	if(!TeamManager::getSingleton().HasTeam())
	{
		TeamManager::getSingleton().SetTeamTargetType(etaatActivity);
		TeamManager::getSingleton().SetTeamTargetId(nActivityID);
	}
}
void UMMOUIActivityViewPanel::ASClickActivityFBList(INT nFBId)
{
	SetActivityTeamList(nFBId, 0);
	SetFBDetailInfo(nFBId);

	if(!TeamManager::getSingleton().HasTeam())
	{
		TeamManager::getSingleton().SetTeamTargetType(etaatDungeon);
		TeamManager::getSingleton().SetTeamTargetId(nFBId);
	}
}

void UMMOUIActivityViewPanel::ASShowActivityFBView()
{
	if(ActivityManager::getSingleton().GetActivityFilterType() == EActivityCopy)
	{
		//Dungeon
		SetFBDetailInfo(ActivityManager::getSingleton().GetCurActivityId());
	}
	else
	{
		//Activity
		SetActivityDetailInfo(ActivityManager::getSingleton().GetCurActivityId());
	}
}

void UMMOUIActivityViewPanel::ASClickActivityTeamList(INT nTeamId)
{
	ShowActivityTeamTips(FALSE);
	ActivityManager::getSingleton().SetSelectedActivityTeam(nTeamId);
}

void UMMOUIActivityViewPanel::ASRollOverActivityTeamList(INT nTeamId)
{
	ShowActivityTeamTips(TRUE);
	UpdateActivityTeamTips(nTeamId);
	AutoSetTeamTipsPosition();
}

void UMMOUIActivityViewPanel::ASRollOutActivityTeamList(INT nTeamId)
{
	ShowActivityTeamTips(FALSE);
}

void UMMOUIActivityViewPanel::ASClickGetReward()
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

void UMMOUIActivityViewPanel::ASClickEnterTeamHome()
{

}

void UMMOUIActivityViewPanel::ASClickTeamManager()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Team, TRUE, "");
}

void UMMOUIActivityViewPanel::ASClickCreateTeam()
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

void UMMOUIActivityViewPanel::ASClickEnterTeam()
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

void UMMOUIActivityViewPanel::ASClickQuickEnterLimitted()
{

}

void UMMOUIActivityViewPanel::ASClickQuickEnter()
{

}

void UMMOUIActivityViewPanel::ASClickInvite()
{

}

void UMMOUIActivityViewPanel::ASClickChangeTarget()
{

}

void UMMOUIActivityViewPanel::ASClickLeaveTeam()
{

}

void UMMOUIActivityViewPanel::ASClickFastJoinTeam(INT nID)
{
	const ActivityTeam* team = ActivityManager::getSingleton().GetRandTeamCurActivity();
	if(team)
		CMessageHandler::GetInstance().Send_EnterTeam(team->LeaderId);
	else
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.NoTeamChoice")));
}

void UMMOUIActivityViewPanel::ASClickRandJoinTeam(INT nID)
{
	const ActivityTeam* team = ActivityManager::getSingleton().GetRandTeamCurActivity();
	if(team)
		CMessageHandler::GetInstance().Send_EnterTeam(team->LeaderId);
}

void UMMOUIActivityViewPanel::ASPrevTeamPageClick()
{
	INT curPage = ActivityManager::getSingleton().GetCurActivityTeamPage();
	if(curPage > 0)
	{
		--curPage;
		SetActivityTeamList(ActivityManager::getSingleton().GetCurActivityId(), curPage);
	}
}

void UMMOUIActivityViewPanel::ASNextTeamPageClick()
{
	INT curPage = ActivityManager::getSingleton().GetCurActivityTeamPage();
	if(curPage < 98)
	{
		++curPage;
		SetActivityTeamList(ActivityManager::getSingleton().GetCurActivityId(), curPage);
	}
}

void UMMOUIActivityViewPanel::ASShowActivityTeamView()
{
	/*if(_ActivityPanel)
	{
	_ActivityPanel->RefreshActivityTeamList();
	}*/
	MMOGameEvent_Activity evt(MMOGameEvent_Activity::EActivity_UpdateActivityTeamList);
	MMOUI::PostGameEvent(&evt);
}
