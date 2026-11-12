#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "RankListManager.h"
#include "MMOChatManager.h"
#include "MMOGuildManager.h"
#include "MMOArenaManager.h"
#include "MasterStudentManager.h"
#include "../net/message_handler_RankList.h"
#include "MMOAchievementData.h"


IMPLEMENT_CLASS(UMMOUIRankListPanel)

#define SHOW_TIME 0.3
#define MAX_ACTIVITY 86400
#define SHOW_RANK_TIME 180

//==================================================Total()====================================================== 
void UMMOUIRankListPanel::NativeInitLoad()
{
	UMMOUIBase::NativeInitLoad();
	
}

void UMMOUIRankListPanel::NativeInitOnce()
{
	UMMOUIBase::NativeInitOnce();
	check(NULL == EventAdapter);
	if (NULL == EventAdapter)
	{
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_RankList_UpdateCategoryButton);
		evts.AddItem(GameEvent_RankList);
		evts.AddItem(GameEvent_RankList_RefreshList);
		EventAdapter = new TGameEventAdapter<UMMOUIRankListPanel>(this, evts);
	}

	LoadPersonMenuData();
	LoadGuildMenuData();
	LoadTournamentMenuData();
	LoadPowerMenuData();
	LoadActivityMenuData();

	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}
	refresh = FALSE;

	SetCurrentActivity();


	FString strMaxLevel = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.RankList.MaxLevel")), RankListManager::getSingleton().GetMaxLevel());
	eventCallUCSetMaxLevel(strMaxLevel);
}

void UMMOUIRankListPanel::PostOnShow(UBOOL v)
{
	UMMOUIBase::PostOnShow(v);
	if (v)
	{
		if (RankListManager::getSingleton().IsShowRank())
		{
			eventCallUCShowRank(TRUE);
			eventCallUCOnPersonTabClick();
			UpdateTimeText();

			FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.ActiveMsg"));
			eventCallShowUCActiveMsg(msg);
		}
		else
		{
			eventCallUCShowRank(FALSE);
		}
		
	}
}

void UMMOUIRankListPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (NULL != evt)
	{
		switch(evt->GetEventType())
		{
		case GameEvent_RankList:
			{
				UpdateRankList();
			}
			break;
		case GameEvent_RankList_UpdateCategoryButton:
			{
				eventCallUpdateCategoryButton();
			}
			break;
		case GameEvent_RankList_RefreshList:
			{
				refresh = TRUE;
			}
			break;

		}
	}
}

void UMMOUIRankListPanel::UpdateRankList()
{
	INT myRank = -1;

	const RANK_LIST eRankList = RankListManager::getSingleton().GetType();
	switch(eRankList)
	{
	case	rlLevel:					// level ranking
		{
			UpdatePersonLevelRank();
			break;
		}
	case	rlPower:					// combat power ranking
	//case	rlArmerPower:				// equipment combat power ranking
	//case	rlSpiritPower:				// realm combat power ranking
	//case	rlItemPower:                // physique combat power
		{
			UpdatePersonPowerRank();
			break;
		}
	//case	rlMoney:					// wealth leaderboard
	//	{
	//		UpdatePersonRichRank();
	//		myRank = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_02);
	//		break;
	//	}
	case	rlAchievement:				// achievement points ranking
		{		
			UpdatePersonAchievementRank();
			myRank = MMOAchievementData::getSingleton().GetAchievementPoint();
			break;
		}
	case	rlMaster:					// famous master value ranking
		{
			UpdatePersonMasterRank();
			myRank = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_MASTER_EXP);
			break;
		}
			// family ranking
	case	rlGuild_Level:				// family level ranking
		{
			UpdateGuildLevelRank();
			break;
		}
	case	rlGuild_Money:				// family money ranking
		{
			UpdateGuildFoundRank();
			break;
		}
	case	rlGuild_Resource_Herb:		// family resource 01
		{
			UpdateGuildResourceRank();
			break;
		}

			// arena ranking
	//case	rlRoleArenaScore:				
	//	{
	//		UpdateTournamentLadderRank();
	//		break;
	//	}
	//case	rlArenaScore:			
	//	{
	//		UpdateTournamentCorpsRank();
	//		break;
	//	}

	case	rlPkValue:					// pk value ranking
		{
			UpdateTournamentPKRank();
			AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
			if( player && player->ActorAttributes )
			{
				myRank =  player->ActorAttributes->GetPkValue();
			}
			
			break;
		}
	case	rlTotalKillCount:			// total kills
		{
			UpdateTournamentKillRank();
			break;
		}
	case rlGuild_Pow:
		{
			updateGuildPowerRank();
		}
		break;
	case rlGuild_War:
		{
			UpdateGuildWarRank();
		}
	//case rlExp:
	//	{
	//		UpdatePersonExpRank();
	//		break;
	//	}
	//case rlActivity01:                  //Bichi Overlord
	//	{
	//		UpdateActivetyRank_BQBZ();
	//	}
	//	break;
	case rlActivity02:                   //Nine-Layer Demon Tower
		{
			UpdateActivityRank_JCYT();
		}
		break;
	case rlArena1v1:                   //arena
		{
			UpdateActivetyRank_Arena1v1();
		}
		break;
	default:
		{
			//ASSERT_CHECK_ERROR_RETURN(NULL && "UpdateRankList is error!");
			break;
		}
	}

	//FString Page = FString::Printf(TEXT("%d/%d"), RankListManager::getSingleton().GetPageIndex()+1, RankListManager::getSingleton().GetPageTotal());
	//eventCallUpdatePage(Page);
	UpdateMaxPage(RankListManager::getSingleton().GetPageIndex(),RankListManager::getSingleton().GetPageTotal());

	FString strRank = TEXT("");
	switch(eRankList)
	{
	case rlGuild_Level:
	case rlGuild_Money:
	case rlGuild_Resource_Herb:
	case rlGuild_Pow:
	case rlGuild_War:
		strRank = GetSelfRankStr(true);
		break;
	default:
		strRank = GetSelfRankStr(false);
		break;
	}
	
	/*if (0 == RankListManager::getSingleton().GetSelfRank())
	strRank = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.NotRank"));
	else
	strRank = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.RankIndex")), RankListManager::getSingleton().GetSelfRank());*/
	if (myRank == 0)
	{
		strRank = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.NotRank"));
		eventCallUpdateMyRank(strRank,0,0);
	}
	else
	{
		eventCallUpdateMyRank(strRank,RankListManager::getSingleton().GetSelfRank(),RankListManager::getSingleton().GetSelfLastRank());
	}
	
}

FString UMMOUIRankListPanel::GetSelfRankStr(bool isGuild)
{
	FString strRank = TEXT("");

	AMMOPlayer* self = MMOFacade::getSingleton().GetMainPlayer();

	if (0 == RankListManager::getSingleton().GetSelfRank())
	{
		if (isGuild)
		{
			if (self && self->GuildName.IsEmpty())
			{
				strRank = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Guild.NotRank"));
			}
			else
			{
				strRank = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Guild.NotRankIndex"));
				
			}
			
		}
		else
		{
			strRank = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.NotRank"));
		}
	}
	else
	{
		if (isGuild)
			strRank = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Guild.RankIndex")), RankListManager::getSingleton().GetSelfRank());
		else
			strRank = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.RankIndex")), RankListManager::getSingleton().GetSelfRank());
	}
	return strRank;
}

void UMMOUIRankListPanel::LoadPersonMenuData()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if (dataProvider == NULL)
		return;

	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 0);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreePower")));
	dataProvider->SetElementObject(0, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 1);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeLevel")));
	dataProvider->SetElementObject(1, itemObj);

	/*itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 1);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.TeamTreeWeekKill")));
	dataProvider->SetElementObject(1, itemObj);*/

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 2);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.TeamTreePK")));
	dataProvider->SetElementObject(2, itemObj);

	//itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	//itemObj->SetInt(TEXT("index"), 3);
	//itemObj->SetInt(TEXT("type"), SUBNODE);
	//itemObj->SetInt(TEXT("deepth"), 0);
	//itemObj->SetBool(TEXT("visible"), true);
	//itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeMoney")));
	//dataProvider->SetElementObject(3, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 3);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeAchievement")));
	dataProvider->SetElementObject(3, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 4);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeMaster")));
	dataProvider->SetElementObject(4, itemObj);

	eventCallLoadPersonMenuData(dataProvider);

}

void UMMOUIRankListPanel::LoadPowerMenuData()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 0);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreePower")));
	dataProvider->SetElementObject(0, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 1);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeEquipPower")));
	dataProvider->SetElementObject(1, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 2);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeBuildPower")));
	dataProvider->SetElementObject(2, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 3);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeJingjiePower")));
	dataProvider->SetElementObject(3, itemObj);
	eventCallLoadPowerMenuData(dataProvider);
}

void UMMOUIRankListPanel::LoadGuildMenuData()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);

	if(NULL == dataProvider)
		return;

	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 0);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildWar")));
	dataProvider->SetElementObject(0, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 1);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildTreePower")));
	dataProvider->SetElementObject(1, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 2);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildTreeLevel")));
	dataProvider->SetElementObject(2, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 3);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildTreeMoney")));
	dataProvider->SetElementObject(3, itemObj);

	//itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	//itemObj->SetInt(TEXT("index"), 2);
	//itemObj->SetInt(TEXT("type"), SUBNODE);
	//itemObj->SetInt(TEXT("deepth"), 0);
	//itemObj->SetBool(TEXT("visible"), true);
	//itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildTreePower")));
	//dataProvider->SetElementObject(2, itemObj);
	eventCallLoadGuildMenuData(dataProvider);
}

void UMMOUIRankListPanel::LoadTournamentMenuData()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;
	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 0);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.TeamTreeTianTi")));
	dataProvider->SetElementObject(0, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 1);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.TeamTreeTeam")));
	dataProvider->SetElementObject(1, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 2);
	itemObj->SetInt(TEXT("type"), PARENTNODE_EX);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.TeamTreeFB")));
	dataProvider->SetElementObject(2, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 3);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 1);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.TeamTree_Dst_Dg06")));
	dataProvider->SetElementObject(3, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 4);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.TeamTreeWeekKill")));
	dataProvider->SetElementObject(4, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 5);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.TeamTreePK")));
	dataProvider->SetElementObject(5, itemObj);

	eventCallLoadTournamentMenuData(dataProvider);
}

void UMMOUIRankListPanel::LoadActivityMenuData()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;
	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 0);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.ActiveTG")));
	dataProvider->SetElementObject(0, itemObj);

	itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.TreeListData"));
	itemObj->SetInt(TEXT("index"), 1);
	itemObj->SetInt(TEXT("type"), SUBNODE);
	itemObj->SetInt(TEXT("deepth"), 0);
	itemObj->SetBool(TEXT("visible"), true);
	itemObj->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.ActiveLT")));
	dataProvider->SetElementObject(1, itemObj);

	eventCallLoadActiveMenuData(dataProvider);

}

void UMMOUIRankListPanel::OnPersonTreeItemClick(INT index)
{
	switch (index)
	{
		//Total combat power
	case 0:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlPower, 0);
			RankListManager::getSingleton().SetPageIndex(rlPower, 0);
			RankListManager::getSingleton().NotifyUpdateCategoryButton();
			break;
		}
		//level
	case 1:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlLevel, 0);
			RankListManager::getSingleton().SetPageIndex(rlLevel, 0);
			break;
		}
		//Weekly kills
		/*case 1:
		{
		CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlTotalKillCount, 0);
		RankListManager::getSingleton().SetPageIndex(rlTotalKillCount, 0);

		break;
		}*/
		//pk
	case 2:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlPkValue, 0);
			RankListManager::getSingleton().SetPageIndex(rlPkValue, 0);
			break;
		}
		//Wealth
	//case 3:
		//{
			//CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlMoney, 0);
			//RankListManager::getSingleton().SetPageIndex(rlMoney, 0);
			//break;
		//}
		//Achievement
	case 3:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlAchievement, 0);
			RankListManager::getSingleton().SetPageIndex(rlAchievement, 0);
		}
		break;
		//Famous master
	case 4:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlMaster, 0);
			RankListManager::getSingleton().SetPageIndex(rlMaster, 0);
		}
		break;
	default:
		//ASSERT_CHECK_ERROR_RETURN(false && TEXT("OnPersonTreeItemClick occor error!"));
		break;
	}
	//SetSelectRadioButton(0);
	//SetSelectCategoryButton(0);
	UpdateMaxPage(0,0);
	
}

void UMMOUIRankListPanel::OnGuildTreeItemClick(INT index)
{
	switch(index)
	{
	case 0:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlGuild_War, 0);
			RankListManager::getSingleton().SetPageIndex(rlGuild_War, 0);
		}
		break;
	case 1:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlGuild_Pow, 0);
			RankListManager::getSingleton().SetPageIndex(rlGuild_Pow, 0);
		}
		break;
	case 2:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlGuild_Level, 0);
			RankListManager::getSingleton().SetPageIndex(rlGuild_Level, 0);
			break;
		}
	case 3:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlGuild_Money, 0);
			RankListManager::getSingleton().SetPageIndex(rlGuild_Money, 0);
			break;
		}
	case 4:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlGuild_Resource_Herb, 0);
			RankListManager::getSingleton().SetPageIndex(rlGuild_Resource_Herb, 0);
			RankListManager::getSingleton().NotifyUpdateCategoryButton();
			break;
		}

	default:
		//ASSERT_CHECK_ERROR_RETURN(false && TEXT("OnGuildTreeItemClick occor error!"));
		break;
	}
	//SetSelectRadioButton(0);
	//SetSelectCategoryButton(0);
	UpdateMaxPage(0,0);
}

void UMMOUIRankListPanel::OnTournamentTreeItemClick(INT index)
{
	switch(index)
	{
	//case 0:
	//	{
	//		CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlRoleArenaScore, 0);
	//		RankListManager::getSingleton().SetPageIndex(rlRoleArenaScore, 0);
	//		break;
	//	}
	//case 1:
	//	{
	//		CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlArenaScore, 0);
	//		RankListManager::getSingleton().SetPageIndex(rlArenaScore, 0);
	//		break;
	//	}
	//case 3:
	//	{
	//		CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlDungeonNormalScore, 0);
	//		RankListManager::getSingleton().SetPageIndex(rlDungeonNormalScore, 0);
	//		break;
	//	}
	case 4:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlPkValue, 0);
			RankListManager::getSingleton().SetPageIndex(rlPkValue, 0);
			break;
		}
	case 5:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlTotalKillCount, 0);
			RankListManager::getSingleton().SetPageIndex(rlTotalKillCount, 0);
			break;
		}
	default:
		//ASSERT_CHECK_ERROR_RETURN(false && TEXT("OnTournamentTreeItemClick"));
		break;
	}
	//SetSelectRadioButton(0);
	//SetSelectCategoryButton(0);
}

void  UMMOUIRankListPanel::OnPowerTreeItemClick(INT index)
{
	switch (index)
	{
	case 0:
		{
			CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlPower, 0);
			RankListManager::getSingleton().SetPageIndex(rlPower, 0);
		}
		break;
	case 1:
		{
			//CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlArmerPower, 0);
			//RankListManager::getSingleton().SetPageIndex(rlArmerPower, 0);
		}
		break;
	case 2:
		{
			//CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlItemPower,0);
			//RankListManager::getSingleton().SetPageIndex(rlItemPower,0);
		}
		break;
	case 3:
		{
			//CMessageHandler::GetInstance().Send_QueryRanklist((INT32)rlSpiritPower, 0);
			//RankListManager::getSingleton().SetPageIndex(rlSpiritPower, 0);
		}
		break;
	}
	//SetSelectRadioButton(0);
	//SetSelectCategoryButton(0);
}

void UMMOUIRankListPanel::OnRangeClick(INT index)
{
	RANK_LIST TotalType = RankListManager::getSingleton().GetTotalType();
	INT32 curTotal = RankListManager::getSingleton().GetPageTotal();
	switch(index)
	{
	case RP_Rdo0:
		{
			if(RP_Page0 < curTotal)
			{
				CMessageHandler::GetInstance().Send_QueryRanklist((INT)TotalType, RP_Page0);
				RankListManager::getSingleton().SetPageIndex(TotalType, RP_Page0);
			}
			break;
		}
	case RP_Rdo1:
		{
			if(RP_Page1 < curTotal)
			{
				CMessageHandler::GetInstance().Send_QueryRanklist((INT)TotalType, RP_Page1);
				RankListManager::getSingleton().SetPageIndex(TotalType, RP_Page1);
			}
			break;
		}
	case RP_Rdo2:
		{
			if(RP_Page2 < curTotal)
			{
				CMessageHandler::GetInstance().Send_QueryRanklist((INT)TotalType, RP_Page2);
				RankListManager::getSingleton().SetPageIndex(TotalType, RP_Page2);
			}
			break;
		}
	case RP_Rdo3:
		{
			if(RP_Page5 < curTotal)
			{
				CMessageHandler::GetInstance().Send_QueryRanklist((INT)TotalType, RP_Page5);
				RankListManager::getSingleton().SetPageIndex(TotalType, RP_Page5);
			}
			break;
		}
	case RP_Rdo4:
		{
			if(RP_Page10 < curTotal)
			{
				CMessageHandler::GetInstance().Send_QueryRanklist((INT)TotalType, RP_Page10);
				RankListManager::getSingleton().SetPageIndex(TotalType, RP_Page10);
			}
			break;
		}
	case RP_Rdo5:
		{
			if(RP_Page20 < curTotal)
			{
				CMessageHandler::GetInstance().Send_QueryRanklist((INT)TotalType, RP_Page20);
				RankListManager::getSingleton().SetPageIndex(TotalType, RP_Page20);
			}
			break;
		}
	case RP_Rdo6:
		{
			if(RP_Page30 < curTotal)
			{
				CMessageHandler::GetInstance().Send_QueryRanklist((INT)TotalType, RP_Page30);
				RankListManager::getSingleton().SetPageIndex(TotalType, RP_Page30);
			}
			break;
		}
	}
	RankListClear();
}


void UMMOUIRankListPanel::OnCategoryClick(INT index)
{
	INT32 Type = RankListManager::getSingleton().GetType();
	CMessageHandler::GetInstance().Send_QueryRanklist((INT32)Type + index, 0);
	RankListManager::getSingleton().SetPageIndex(RANK_LIST(Type + index), 0);
	//SetSelectRadioButton(0);

}


void UMMOUIRankListPanel::OnPrevPageClick()
{
	INT nCurPage = RankListManager::getSingleton().GetPageIndex();
	if (nCurPage-- > 0)
	{
		RANK_LIST totalType = RankListManager::getSingleton().GetTotalType();
		RankListManager::getSingleton().SetPageIndex(totalType, nCurPage);
		CMessageHandler::GetInstance().Send_QueryRanklist(totalType, nCurPage);
	}
}

void UMMOUIRankListPanel::OnNextPageClick()
{
	INT nCurPage = RankListManager::getSingleton().GetPageIndex();
	if (++nCurPage <= RankListManager::getSingleton().GetPageTotal() - 1)
	{
		RANK_LIST totalType = RankListManager::getSingleton().GetTotalType();
		RankListManager::getSingleton().SetPageIndex(totalType, nCurPage);
		CMessageHandler::GetInstance().Send_QueryRanklist(totalType, nCurPage);
	}
}

void UMMOUIRankListPanel::UpdateRangeBtn(int currentPage)
{
	INT nBtnIndex = RankListManager::getSingleton().GetRangeBtnIndex(currentPage);
	if(-1 != nBtnIndex)
	{
		eventCallUpdateRangeRdoSelect(nBtnIndex);
	}
	
}

void UMMOUIRankListPanel::UpdatePersonLevelRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlLevel);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		//if (NULL == pPlayerBaseInfo)
		//{
		//	if((*it)->Id > 0)
		//		MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		//}
		if ( pPlayerBaseInfo )
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.PersonLevelItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			if(pPlayerBaseInfo->_GuildName.Len() > 0)
				itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
			else
				itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
			itemObj->SetInt(TEXT("Level"), (*it)->Value);
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
			itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdatePersonListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateActivityRank_JCYT()
{

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	dataProvider->SetElementInt(0, (INT32)rlActivity02);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if ( pPlayerBaseInfo )
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.ActiveTGItemData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
			itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
			INT32 Layer = (*it)->Value >> 32;
			itemObj->SetString(TEXT("Layer"),appItoa(Layer));
			INT32 time =  (*it)->Value  & 0xffffffff ;
			time = MAX_ACTIVITY -  time;
			itemObj->SetInt(TEXT("TextTimer"),time);
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdateActivityListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateActivetyRank_BQBZ()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	dataProvider->SetElementInt(0, (INT32)rlActivity01);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if ( pPlayerBaseInfo )
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.ActiveBZItemData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);

			if(pPlayerBaseInfo->_GuildName.Len() > 0)
				itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
			else
				itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));

			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
			itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
			
			itemObj->SetInt(TEXT("num"),(*it)->Value);
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdateActivityListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateActivetyRank_Arena1v1()
{

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	dataProvider->SetElementInt(0, (INT32)rlArena1v1);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if ( pPlayerBaseInfo )
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.ActiveArenaItemData"));
			if (itemObj)
			{
				itemObj->SetInt(TEXT("Id"), (*it)->Id);
				INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
				itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
				itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
				FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
				itemObj->SetString(TEXT("Job"), Job);
				BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
				itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
				itemObj->SetInt(TEXT("Level"), pPlayerBaseInfo->_Level);
				itemObj->SetInt(TEXT("num"), (*it)->Value);
				if(pPlayerBaseInfo->_GuildName.Len() > 0)
					itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
				else
					itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
				dataProvider->SetElementObject(index, itemObj);
			}
			
			index++;
		}
	}
	eventCallUpdateActivityListData(dataProvider);
}
void UMMOUIRankListPanel::UpdatePersonPowerRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlPower);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	RankVec::const_iterator it = rankVec.begin();

	for (; it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if (pPlayerBaseInfo)
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.PersonPowerItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			if(pPlayerBaseInfo->_GuildName.Len() > 0)
				itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
			else
				itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
			itemObj->SetInt(TEXT("Level"), pPlayerBaseInfo->_Level);
			itemObj->SetString(TEXT("Power"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
			itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}

	/*UGFxObject* pPersonObject = GetObject(TEXT("power_mc"));
	ASSERT_CHECK_ERROR_RETURN(pPersonObject);
	UGFxObject* pPersonPagePowerObject = pPersonObject->GetObject(TEXT("PowerPagePower_mc"));
	ASSERT_CHECK_ERROR_RETURN(pPersonPagePowerObject);
	UGFxObject* pPowerButton = pPersonPagePowerObject->GetObject(TEXT("Power_btn_mc"));
	ASSERT_CHECK_ERROR_RETURN(pPowerButton);
	RANK_LIST type = RankListManager::getSingleton().GetType();
	if (rlPower == type)
	{
		pPowerButton->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreePower")));
	}
	else if(rlArmerPower == type)
	{
		pPowerButton->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeEquipPower")));
	}
	else if (rlSpiritPower == type)
	{
		pPowerButton->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeJingjiePower")));
	}
	else if (rlItemPower == type)
	{
		pPowerButton->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.PersonTreeBuildPower")));
	}*/

	//eventCallUpdatePersonListData(dataProvider);
	eventCallUpdatePersonListData(dataProvider);
}

void UMMOUIRankListPanel::UpdatePersonRichRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlMoney);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if (pPlayerBaseInfo)
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.PersonRichItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			if(pPlayerBaseInfo->_GuildName.Len() > 0)
				itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
			else
				itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
			itemObj->SetInt(TEXT("Level"), pPlayerBaseInfo->_Level);
			//itemObj->SetString(TEXT("Found"), FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.RankList.Money")) ,(*it)->Value/100));
			itemObj->SetInt(TEXT("Found"),(*it)->Value);
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
			itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdatePersonListData(dataProvider);
}

void UMMOUIRankListPanel::UpdatePersonAchievementRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlAchievement);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if (pPlayerBaseInfo)
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.PersonAchievementItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			if(pPlayerBaseInfo->_GuildName.Len() > 0)
				itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
			else
				itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
			itemObj->SetInt(TEXT("Level"), pPlayerBaseInfo->_Level);
			itemObj->SetString(TEXT("Grades"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
			itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdatePersonListData(dataProvider);
}

void UMMOUIRankListPanel::UpdatePersonMasterRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlMaster);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if (pPlayerBaseInfo)
		{
			if ((*it)->Value <= 0)
				continue;

			INT exp = (*it)->Value;
			exp =  MasterStudentManager::getSingleton().GetMasterExpByRatio(exp);

			if (exp <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.PersonMasterItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			if(pPlayerBaseInfo->_GuildName.Len() > 0)
				itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
			else
				itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
			itemObj->SetInt(TEXT("Level"), pPlayerBaseInfo->_Level);
			
			itemObj->SetString(TEXT("Grades"), appItoa(exp));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
			itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdatePersonListData(dataProvider);
}

void UMMOUIRankListPanel::UpdatePersonExpRank()
{
	//UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	//dataProvider->SetElementInt(0, (INT32)rlExp);

	//INT index = 1;
	//const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	//for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	//{
	//	PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
	//	if (pPlayerBaseInfo)
	//	{
	//		UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.PersonPageExpItemListData"));
	//		itemObj->SetInt(TEXT("Id"), (*it)->Id);
	//		itemObj->SetString(TEXT("RankIndex"), appItoa(index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE));
	//		itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
	//		FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
	//		itemObj->SetString(TEXT("Job"), Job);
	//		if(pPlayerBaseInfo->_GuildName.Len() > 0)
	//			itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
	//		else
	//			itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
	//		itemObj->SetInt(TEXT("Level"), pPlayerBaseInfo->_Level);
	//		//Get current experience
	//		itemObj->SetString(TEXT("Exp"),appItoa(GetExp(pPlayerBaseInfo->_Level, GET_RANKLIST_EXP_FROM_KEY((*it)->Value))));

	//		dataProvider->SetElementObject(index, itemObj);
	//		index++;
	//	}
	//}
	//eventCallUpdatePersonListData(dataProvider);
}

INT64 UMMOUIRankListPanel::GetExp(INT level,INT64 exp)
{
	return MMOFacade::getSingleton().GetLevelExp(level) + exp;
}

void UMMOUIRankListPanel::UpdateGuildLevelRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;
	dataProvider->SetElementInt(0, (INT32)rlGuild_Level);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		GuildQueryInfo info;
		if (!MMOGuildData::getSingleton().GetGuildInfoById((*it)->Id, info))
		{
			if((*it)->Id > 0)
			{
				MMOGuildManager::getSingleton().QueryGuildInfo((*it)->Id);
			}
		}
		else
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.GuildLevelItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("GuildName"), info.Name);
			PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(info.LeaderId);
			if (NULL == pPlayerBaseInfo)
			{				
				itemObj->SetString(TEXT("GuilderName"), TEXT(" "));
			}
			else
				itemObj->SetString(TEXT("GuilderName"), pPlayerBaseInfo->_RoleName);
			itemObj->SetString(TEXT("Level"), appItoa((*it)->Value));
			GuildBuildingData Data;
			MMOGuildData::getSingleton().GetGuildBuildingData(GUILD_B_HOUSE, info.BuildingLevel, Data);
			itemObj->SetString(TEXT("Num"), FString::Printf(TEXT("%d/%d"), info.MemberCount, Data.Param1));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdateGuildListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateGuildFoundRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlGuild_Money);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		GuildQueryInfo info;
		if (!MMOGuildData::getSingleton().GetGuildInfoById((*it)->Id, info))
		{
			if((*it)->Id > 0)
			{
				MMOGuildManager::getSingleton().QueryGuildInfo((*it)->Id);
			}
		}
		else
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.GuildFoundItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("GuildName"), info.Name);
			PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(info.LeaderId);
			if (NULL == pPlayerBaseInfo)
			{				
				itemObj->SetString(TEXT("GuilderName"), TEXT(" "));
			}
			else
				itemObj->SetString(TEXT("GuilderName"), pPlayerBaseInfo->_RoleName);
			itemObj->SetString(TEXT("Level"), appItoa(info.Level));
			GuildBuildingData Data;
			MMOGuildData::getSingleton().GetGuildBuildingData(GUILD_B_HOUSE, info.BuildingLevel, Data);
			itemObj->SetString(TEXT("Num"), FString::Printf(TEXT("%d/%d"), info.MemberCount, Data.Param1));
			//itemObj->SetString(TEXT("Found"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Found"),(*it)->Value);
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdateGuildListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateGuildResourceRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlGuild_Resource_Herb);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		GuildQueryInfo info;
		if (!MMOGuildData::getSingleton().GetGuildInfoById((*it)->Id, info))
		{
			if((*it)->Id > 0)
			{
				MMOGuildManager::getSingleton().QueryGuildInfo((*it)->Id);
			}
		}
		else
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.GuildResourceItemListData"));
			if (!itemObj)
				continue;

			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("GuildName"), info.Name);
			PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(info.LeaderId);
			if (NULL == pPlayerBaseInfo)
			{				
				itemObj->SetString(TEXT("GuilderName"), TEXT(" "));
			}
			else
				itemObj->SetString(TEXT("GuilderName"), pPlayerBaseInfo->_RoleName);
			GuildBuildingData Data;
			MMOGuildData::getSingleton().GetGuildBuildingData(GUILD_B_HOUSE, info.BuildingLevel, Data);
			itemObj->SetString(TEXT("Num"), FString::Printf(TEXT("%d/%d"), info.MemberCount, Data.Param1));
			itemObj->SetString(TEXT("Level"), appItoa(info.Level));
			itemObj->SetString(TEXT("Food"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}

	/*UGFxObject* pGuildObject = GetObject(TEXT("guild_mc"));
	ASSERT_CHECK_ERROR_RETURN(pGuildObject);
	UGFxObject* pGuildPageResourceObject = pGuildObject->GetObject(TEXT("GuildPageResource_mc"));
	ASSERT_CHECK_ERROR_RETURN(pGuildPageResourceObject);
	UGFxObject* pResourceButton = pGuildPageResourceObject->GetObject(TEXT("foodstorage_btn_mc"));
	ASSERT_CHECK_ERROR_RETURN(pResourceButton);
	RANK_LIST type = RankListManager::getSingleton().GetTotalType();*/
	/*if (rlGuild_Resource_Wood == type)
	{
		pResourceButton->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildResWood")));
	}
	else if(rlGuild_Resource_Herb == type)
	{
		pResourceButton->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildResFood")));
	}
	else if (rlGuild_Resource_Crystal == type)
	{
		pResourceButton->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildResStone")));
	}
	else if (rlGuild_Resource_Silver == type)
	{
		pResourceButton->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.GuildResSliver")));
	}*/
	eventCallUpdateGuildListData(dataProvider);
}

void UMMOUIRankListPanel::updateGuildPowerRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlGuild_Pow);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		GuildQueryInfo info;
		if (!MMOGuildData::getSingleton().GetGuildInfoById((*it)->Id, info))
		{
			if((*it)->Id > 0)
			{
				MMOGuildManager::getSingleton().QueryGuildInfo((*it)->Id);
			}
		}
		else
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.GuildPowerItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("GuildName"), info.Name);
			PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(info.LeaderId);
			if (NULL == pPlayerBaseInfo)
			{				
				itemObj->SetString(TEXT("GuilderName"), TEXT(" "));
			}
			else
				itemObj->SetString(TEXT("GuilderName"), pPlayerBaseInfo->_RoleName);
			itemObj->SetString(TEXT("Level"), appItoa(info.Level));
			GuildBuildingData Data;
			MMOGuildData::getSingleton().GetGuildBuildingData(GUILD_B_HOUSE, info.BuildingLevel, Data);
			itemObj->SetString(TEXT("Num"), FString::Printf(TEXT("%d/%d"), info.MemberCount, Data.Param1));
			//itemObj->SetString(TEXT("Found"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Pow"),(*it)->Value);
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));

			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdateGuildListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateGuildWarRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlGuild_War);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		GuildQueryInfo info;
		if (!MMOGuildData::getSingleton().GetGuildInfoById((*it)->Id, info))
		{
			if((*it)->Id > 0)
			{
				MMOGuildManager::getSingleton().QueryGuildInfo((*it)->Id);
			}
		}
		else
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.GuildFoundItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("GuildName"), info.Name);
			PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(info.LeaderId);
			if (NULL == pPlayerBaseInfo)
			{				
				itemObj->SetString(TEXT("GuilderName"), TEXT(" "));
			}
			else
				itemObj->SetString(TEXT("GuilderName"), pPlayerBaseInfo->_RoleName);
			itemObj->SetString(TEXT("Level"), appItoa(info.Level));
			GuildBuildingData Data;
			MMOGuildData::getSingleton().GetGuildBuildingData(GUILD_B_HOUSE, info.BuildingLevel, Data);
			itemObj->SetString(TEXT("Num"), FString::Printf(TEXT("%d/%d"), info.MemberCount, Data.Param1));
			//itemObj->SetString(TEXT("Found"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Found"),(*it)->Value);
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));

			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdateGuildListData(dataProvider);
}
void UMMOUIRankListPanel::UpdateTournamentLadderRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	//dataProvider->SetElementInt(0, (INT32)rlRoleArenaScore);
	if(NULL == dataProvider)
		return;

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if (pPlayerBaseInfo)
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.TournamentLadderItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			itemObj->SetString(TEXT("Level"), appItoa(pPlayerBaseInfo->_Level));
			itemObj->SetString(TEXT("Grades"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdateTournamentListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateTournamentCorpsRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	//dataProvider->SetElementInt(0, (INT32)rlArenaScore);
	if(NULL == dataProvider)
		return;

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		//const ArenaTeamInfo* pArenaTeamInfo = MMOArenaManager::getSingleton().FindAreanTeam((*it)->Id);
		//if (NULL == pArenaTeamInfo)
		//{
		//	if((*it)->Id > 0)
		//		CMessageHandler::GetInstance().Send_RequestBattleTeamInfo((*it)->Id);
		//}
		//else
		//{
		//	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.TournamentCorpsItemListData"));
		//	itemObj->SetInt(TEXT("Id"), (*it)->Id);
		//	itemObj->SetString(TEXT("RankIndex"), appItoa(index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE));
		//	itemObj->SetString(TEXT("CorpsName"), pArenaTeamInfo->AreanTeamName);
		//	itemObj->SetString(TEXT("Captain"), pArenaTeamInfo->Captain);
		//	itemObj->SetString(TEXT("Grades"), appItoa((*it)->Value));
		//	dataProvider->SetElementObject(index, itemObj);
		//	index++;
		//}
	}
	eventCallUpdateTournamentListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateTournamentFB0Rank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
//	dataProvider->SetElementInt(0, (INT32)rlDungeonNormalScore);
	if(NULL == dataProvider)
		return;

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if (pPlayerBaseInfo)
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.FBSingleItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			itemObj->SetString(TEXT("Level"), appItoa(pPlayerBaseInfo->_Level));
			itemObj->SetString(TEXT("Grade"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	eventCallUpdateTournamentListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateTournamentKillRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlTotalKillCount);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if (pPlayerBaseInfo)
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.TournamentKillItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			itemObj->SetString(TEXT("Level"), appItoa(pPlayerBaseInfo->_Level));
			if(pPlayerBaseInfo->_GuildName.Len() > 0)
				itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
			else
				itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
			itemObj->SetString(TEXT("KillNum"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	//eventCallUpdateTournamentListData(dataProvider);
	eventCallUpdatePersonListData(dataProvider);
}

void UMMOUIRankListPanel::UpdateTournamentPKRank()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//ASSERT_CHECK_ERROR_RETURN(dataProvider);
	if(NULL == dataProvider)
		return;

	dataProvider->SetElementInt(0, (INT32)rlPkValue);

	INT index = 1;
	const RankVec& rankVec = RankListManager::getSingleton().GetRankVec();
	for (RankVec::const_iterator it = rankVec.begin(); it != rankVec.end(); ++it)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*it)->Id);
		if (pPlayerBaseInfo)
		{
			if ((*it)->Value <= 0)
				continue;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.RankList.TournamentPKItemListData"));
			itemObj->SetInt(TEXT("Id"), (*it)->Id);
			INT currentIndex = index + RankListManager::getSingleton().GetPageIndex()*MAX_RANK_NODE_NUM_PER_PAGE;
			itemObj->SetString(TEXT("RankIndex"), appItoa(currentIndex));
			itemObj->SetString(TEXT("RoleName"), pPlayerBaseInfo->_RoleName);
			FString Job = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass));
			itemObj->SetString(TEXT("Job"), Job);
			itemObj->SetString(TEXT("Level"), appItoa(pPlayerBaseInfo->_Level));
			if(pPlayerBaseInfo->_GuildName.Len() > 0)
				itemObj->SetString(TEXT("GuildName"), pPlayerBaseInfo->_GuildName);
			else
				itemObj->SetString(TEXT("GuildName"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Empty")));
			itemObj->SetString(TEXT("PKValue"), appItoa((*it)->Value));
			itemObj->SetInt(TEXT("Ranking"),GetRanking((*it)->LastRank,currentIndex));
			BOOL isPlayer = MMOFacade::getSingleton().GetMainPlayer()->ServerID == (*it)->Id?TRUE:FALSE;
			itemObj->SetBool(TEXT("IsPlayer"),isPlayer);
			dataProvider->SetElementObject(index, itemObj);
			index++;
		}
	}
	//eventCallUpdateTournamentListData(dataProvider);
	eventCallUpdatePersonListData(dataProvider);
}

void UMMOUIRankListPanel::OnRightClick1Menu(INT Id)
{
	AMMOActor* pActor = MMOFacade::getSingleton().GetMainActor();
	//ASSERT_CHECK_ERROR_RETURN(pActor);
	if(NULL == pActor)
		return;
	if(pActor->ServerID != Id)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(Id);
		if (pPlayerBaseInfo)
		{
			FIntPoint MousePos = MMOFacade::GetMouseCursorPosition();
			MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_RankListPlayer ,
				MousePos.X , MousePos.Y, Id, pPlayerBaseInfo->_RoleName, 0 );
		}
	}
}

void UMMOUIRankListPanel::OnRightClick2Menu(INT Id)
{
	GuildQueryInfo info;
	if (!MMOGuildData::getSingleton().GetGuildInfoById(Id, info))
	{
		if(Id > 0)
		{
			MMOGuildManager::getSingleton().QueryGuildInfo(Id);
		}
	}
	else
	{
		AMMOActor* pActor = MMOFacade::getSingleton().GetMainActor();
		//ASSERT_CHECK_ERROR_RETURN(pActor);
		if(NULL == pActor)
			return;

		const GuildBaseInfo& guildBaseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		if(guildBaseInfo.Id != info.Id)
		{
			FIntPoint MousePos = MMOFacade::GetMouseCursorPosition();
			MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_RankListGuild ,
				MousePos.X , MousePos.Y, Id, info.Name, 0 );
		}
	}
}

void UMMOUIRankListPanel::OnPeronMasterClick(INT Id)
{
	AMMOActor* pActor = MMOFacade::getSingleton().GetMainActor();
	if (pActor == NULL)
		return;
	//ASSERT_CHECK_ERROR_RETURN(pActor);
	if(pActor->ServerID != Id)
	{
		PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(Id);
		if (pPlayerBaseInfo)
			CMessageHandler::GetInstance().Send_SubmitApplyToMaster(Id); 
	}
}

void UMMOUIRankListPanel::OnPersonTabClick()
{
	//CMessageHandler::GetInstance().Send_QueryRanklist(rlPower, 0);
	//UGFxObject* pPersonObject = GetObject(TEXT("person_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pPersonObject);
	//UGFxObject* pPersonPageLevelObject = pPersonObject->GetObject(TEXT("PersonPagePower_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pPersonPageLevelObject);
	//UGFxObject* pRadioButton = pPersonPageLevelObject->GetObject(TEXT("range0_radio_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pRadioButton);
	//pRadioButton->SetBool(TEXT("selected"), TRUE);
	OnPersonTreeItemClick(0);
	UpdateMaxPage(0,0);
}

void UMMOUIRankListPanel::OnGuildTabClick()
{
	//CMessageHandler::GetInstance().Send_QueryRanklist(rlGuild_Pow, 0);
	//UGFxObject* pGuildObject = GetObject(TEXT("guild_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pGuildObject);
	//UGFxObject* pGuildPageLevelObject = pGuildObject->GetObject(TEXT("GuildPagePower_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pGuildPageLevelObject);
	//UGFxObject* pRadioButton = pGuildPageLevelObject->GetObject(TEXT("range0_radio_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pRadioButton);
	//pRadioButton->SetBool(TEXT("selected"), TRUE);
	OnGuildTreeItemClick(0);
	UpdateMaxPage(0,0);
}

void UMMOUIRankListPanel::OnTournamentTabClick()
{
	//CMessageHandler::GetInstance().Send_QueryRanklist(rlRoleArenaScore, 0);
	UGFxObject* pTournamentObject = GetObject(TEXT("tournament_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pTournamentObject);
	if(NULL == pTournamentObject)
		return;

	UGFxObject* pTournamentPageLevelObject = pTournamentObject->GetObject(TEXT("TournamentPageLadder_mc"));
	if(NULL == pTournamentPageLevelObject)
		return;

	//ASSERT_CHECK_ERROR_RETURN(pTournamentPageLevelObject);
	UGFxObject* pRadioButton = pTournamentPageLevelObject->GetObject(TEXT("range0_radio_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pRadioButton);
	if(NULL == pRadioButton)
		return;

	pRadioButton->SetBool(TEXT("selected"), TRUE);
}

void UMMOUIRankListPanel::OnPowerTabClick()
{
	CMessageHandler::GetInstance().Send_QueryRanklist(rlPower, 0);
	UGFxObject* pPowerObject = GetObject(TEXT("power_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pPowerObject);
	if(NULL == pPowerObject)
		return;

	UGFxObject* pPowerPagePowerObject = pPowerObject->GetObject(TEXT("PowerPagePower_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pPowerPagePowerObject);
	if(NULL == pPowerPagePowerObject)
		return;

	UGFxObject* pRadioButton = pPowerPagePowerObject->GetObject(TEXT("range0_radio_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pRadioButton);
	if(NULL == pRadioButton)
		return;

	pRadioButton->SetBool(TEXT("selected"), TRUE);
}

//Activity
void UMMOUIRankListPanel::OnActivityTabClick()
{
	
	OnActivityTreeItemClick(0);
	UpdateMaxPage(0,0);
}

void UMMOUIRankListPanel::OnActivityTreeItemClick(INT index)
{
	switch(index)
	{
	case 0: // Nine-Layer Demon Tower
		{
			CMessageHandler::GetInstance().Send_QueryRanklist(_currentActivityType, 0);
			RankListManager::getSingleton().SetPageIndex(RANK_LIST(_currentActivityType), 0);

			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				eventCallUpdateActivityRdo(mainPlayer->GetClassId());
			}
		}
		break;

	case 1://1v1
		{
			CMessageHandler::GetInstance().Send_QueryRanklist(rlArena1v1, 0);
			RankListManager::getSingleton().SetPageIndex(rlArena1v1, 0);
		}
		break;
	}
}


INT UMMOUIRankListPanel::GetCurrentRangeIndex()
{
	RANK_LIST type = RankListManager::getSingleton().GetType();
	UGFxObject* pPersonObject = GetObject(TEXT("person_mc"));
	//ASSERT_CHECK_ERROR_0_RETURN(pPersonObject);
	UGFxObject* pGuildObject = GetObject(TEXT("guild_mc"));
	//ASSERT_CHECK_ERROR_0_RETURN(pGuildObject);
	UGFxObject* pTournamentObject = GetObject(TEXT("tournament_mc"));
	//ASSERT_CHECK_ERROR_0_RETURN(pTournamentObject);

	if (NULL == pPersonObject || NULL == pGuildObject || NULL == pTournamentObject)
		return 0;

	UGFxObject* pPage = NULL;
	int rangeIndex = 0;

	switch(type)
	{
	case rlLevel:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageLevel_mc"));
			break;
		}
	case rlPower:
	//case rlArmerPower:
	//case rlSpiritPower:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPagePower_mc"));
			break;
		}
	/*case rlMoney:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageRich_mc"));
			break;
		}*/
	case rlAchievement:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageAchievement_mc"));
			break;
		}
	case rlMaster:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageMaster_mc"));
			break;
		}
	case rlGuild_Level:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageLevel_mc"));
			break;
		}
	case rlGuild_Money:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageFound_mc"));
			break;
		}
	case	rlGuild_Resource_Herb:		// family resources
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageResource_mc"));
			break;
		}
	case	rlGuild_Pow:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPagePower_mc"));
			break;
		}
	case rlGuild_War:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageExp_mc"));
		}
		break;
	//case rlArenaScore:
	//	{
	//		pPage = pTournamentObject->GetObject(TEXT("TournamentPageCorps_mc"));
	//		break;
	//	}
	//case rlRoleArenaScore:
	//	{
	//		pPage = pTournamentObject->GetObject(TEXT("TournamentPageLadder_mc"));
	//		break;
	//	}
	case rlPkValue:
		{
			pPage = pTournamentObject->GetObject(TEXT("TournamentPagePK_mc"));
			break;
		}
	case rlTotalKillCount:
		{
			pPage = pTournamentObject->GetObject(TEXT("TournamentPageKill_mc"));
			break;
		}
	//case rlDungeonNormalScore:
	//	{
	//		pPage = pTournamentObject->GetObject(TEXT("TournamentPageFB_mc"));
	//		break;
	//	}
	default:
		{
			//ASSERT_CHECK_ERROR_0_RETURN(NULL && TEXT("GetCurrentRangeIndex occor error!"));
			break;
		}
	}
	//ASSERT_CHECK_ERROR_0_RETURN(pPage);
	if (NULL == pPage)
		return 0;

	UGFxObject* pRadioButton0 = pPage->GetObject(TEXT("range0_radio_mc"));
	UGFxObject* pRadioButton1 = pPage->GetObject(TEXT("range1_radio_mc"));
	UGFxObject* pRadioButton2 = pPage->GetObject(TEXT("range2_radio_mc"));
	UGFxObject* pRadioButton3 = pPage->GetObject(TEXT("range3_radio_mc"));
	UGFxObject* pRadioButton4 = pPage->GetObject(TEXT("range4_radio_mc"));
	UGFxObject* pRadioButton5 = pPage->GetObject(TEXT("range5_radio_mc"));
	UGFxObject* pRadioButton6 = pPage->GetObject(TEXT("range6_radio_mc"));
	if(pRadioButton0 && pRadioButton0->GetBool(TEXT("selected")))
		rangeIndex = 0;
	else if(pRadioButton1 && pRadioButton1->GetBool(TEXT("selected")))
		rangeIndex = 1;
	else if(pRadioButton2 && pRadioButton2->GetBool(TEXT("selected")))
		rangeIndex = 2;
	else if(pRadioButton3 && pRadioButton3->GetBool(TEXT("selected")))
		rangeIndex = 3;
	else if(pRadioButton4 && pRadioButton4->GetBool(TEXT("selected")))
		rangeIndex = 4;
	else if(pRadioButton5 && pRadioButton5->GetBool(TEXT("selected")))
		rangeIndex = 5;
	else if(pRadioButton6 && pRadioButton5->GetBool(TEXT("selected")))
		rangeIndex = 6;
	return rangeIndex;
}


void UMMOUIRankListPanel::RankListClear()
{
	RankListManager::getSingleton().Clear();
	RankListManager::getSingleton().SetSelfRank(0);
	UpdateRankList();
}

void UMMOUIRankListPanel::SetSelectRadioButton(INT index)
{
	RANK_LIST type = RankListManager::getSingleton().GetType();
	UGFxObject* pPersonObject = GetObject(TEXT("person_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pPersonObject);
	UGFxObject* pGuildObject = GetObject(TEXT("guild_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pGuildObject);
	UGFxObject* pTournamentObject = GetObject(TEXT("tournament_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pTournamentObject);
	UGFxObject* pPowerObject = GetObject(TEXT("power_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pPowerObject);

	if (NULL == pPersonObject || NULL == pGuildObject || NULL == pTournamentObject ||NULL == pPowerObject )
		return;

	UGFxObject* pPage = NULL;
	int rangeIndex = 0;

	switch(type)
	{
	case rlLevel:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageLevel_mc"));
			break;
		}
	case rlPower:
		{
			pPage = pPowerObject->GetObject(TEXT("PowerPagePower_mc"));
			break;
		}
	//case rlArmerPower:
	//	{
	//		pPage = pPowerObject->GetObject(TEXT("PowerPageEquip_mc"));
	//		break;
	//	}
	//case rlSpiritPower:
	//	{
	//		pPage = pPowerObject->GetObject(TEXT("PowerPageAmbit_mc"));
	//		break;
	//	}
	//case rlItemPower:
	//	{
	//		pPage = pPowerObject->GetObject(TEXT("PowerPageBuild_mc"));
	//		break;
	//	}
	/*case rlMoney:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageRich_mc"));
			break;
		}*/
	case rlAchievement:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageAchievement_mc"));
			break;
		}
	case rlMaster:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageMaster_mc"));
			break;
		}
	case rlGuild_Level:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageLevel_mc"));
			break;
		}
	case rlGuild_Money:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageFound_mc"));
			break;
		}
	case	rlGuild_Resource_Herb:		// family resources
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageResource_mc"));
			break;
		}
	case	rlGuild_Pow:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPagePower_mc"));
			break;
		}

	case	rlGuild_War:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageExp_mc"));
			break;
		}
	//case rlArenaScore:
	//	{
	//		pPage = pTournamentObject->GetObject(TEXT("TournamentPageCorps_mc"));
	//		break;
	//	}
	//case rlRoleArenaScore:
	//	{
	//		pPage = pTournamentObject->GetObject(TEXT("TournamentPageLadder_mc"));
	//		break;
	//	}
	case rlPkValue:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPagePK_mc"));
			break;
		}
	case rlTotalKillCount:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageKill_mc"));
			break;
		}
	//case rlDungeonNormalScore:
	//	{
	//		pPage = pTournamentObject->GetObject(TEXT("TournamentPageFB_mc"));
	//		break;
	//	}
	//case rlExp:
	//	{
	//		pPage = pPersonObject->GetObject(TEXT("PersonPageExp_mc"));
	//		break;
	//	}
	default:
		{
			//ASSERT_CHECK_ERROR_RETURN(NULL && TEXT("GetCurrentRangeIndex occor error!"));
			break;
		}
	}
	//ASSERT_CHECK_ERROR_RETURN(pPage);
	if (NULL == pPage)
	{
		return;
	}
	UGFxObject* pRadioButton0 = pPage->GetObject(TEXT("range0_radio_mc"));
	UGFxObject* pRadioButton1 = pPage->GetObject(TEXT("range1_radio_mc"));
	UGFxObject* pRadioButton2 = pPage->GetObject(TEXT("range2_radio_mc"));
	UGFxObject* pRadioButton3 = pPage->GetObject(TEXT("range3_radio_mc"));
	UGFxObject* pRadioButton4 = pPage->GetObject(TEXT("range4_radio_mc"));
	UGFxObject* pRadioButton5 = pPage->GetObject(TEXT("range5_radio_mc"));
	UGFxObject* pRadioButton6 = pPage->GetObject(TEXT("range5_radio_mc"));
	if (index == 0)
	{
		if(pRadioButton0)
			pRadioButton0->SetBool(TEXT("selected"), true);
	}
	else if (index == 1)
	{
		if (pRadioButton1)
			pRadioButton1->SetBool(TEXT("selected"), true);
	}
	else if (index ==2)
	{
		if (pRadioButton2)
			pRadioButton2->SetBool(TEXT("selected"), true);
	}
	else if (index == 3)
	{
		if (pRadioButton3)
			pRadioButton3->SetBool(TEXT("selected"), true);
	}
	else if (index == 4)
	{
		if (pRadioButton4)
			pRadioButton4->SetBool(TEXT("selected"), true);
	}
	else if (index == 5)
	{
		if (pRadioButton5)
			pRadioButton5->SetBool(TEXT("selected"), true);
	}
	else if (index == 6)
	{
		if (pRadioButton6)
			pRadioButton6->SetBool(TEXT("selected"), true);
	}
}

void UMMOUIRankListPanel::SetSelectCategoryButton(INT index)
{
	RANK_LIST type = RankListManager::getSingleton().GetType();
	UGFxObject* pPersonObject = GetObject(TEXT("person_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pPersonObject);
	UGFxObject* pGuildObject = GetObject(TEXT("guild_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pGuildObject);
	UGFxObject* pTournamentObject = GetObject(TEXT("tournament_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pTournamentObject);
	UGFxObject* pPowerObject = GetObject(TEXT("power_mc"));
	//ASSERT_CHECK_ERROR_RETURN(pPowerObject);

	if (NULL == pPersonObject || NULL == pGuildObject || NULL == pTournamentObject || NULL == pPowerObject)
		return;

	UGFxObject* pPage = NULL;
	int rangeIndex = 0;

	switch(type)
	{
	/*case rlLevel:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageLevel_mc"));
			break;
		}*/
	case rlPower:
		{
			pPage = pPowerObject->GetObject(TEXT("PowerPagePower_mc"));
		}
		break;
	//case rlArmerPower:
	//	{
	//		pPage = pPowerObject->GetObject(TEXT("PowerPageBuild_mc"));
	//	}
	//	break;
	//case rlSpiritPower:
	//	{
	//		pPage = pPowerObject->GetObject(TEXT("PowerPageAmbit_mc"));
	//		break;
	//	}
	/*case rlMoney:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageRich_mc"));
			break;
		}
	case rlAchievement:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageAchievement_mc"));
			break;
		}
	case rlMaster:
		{
			pPage = pPersonObject->GetObject(TEXT("PersonPageMaster_mc"));
			break;
		}
	case rlGuild_Level:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageLevel_mc"));
			break;
		}
	case rlGuild_Money:
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageFound_mc"));
			break;
		}*/
	case	rlGuild_Resource_Herb:		// family resources
		{
			pPage = pGuildObject->GetObject(TEXT("GuildPageResource_mc"));
			break;
		}
	/*case rlArenaScore:
		{
			pPage = pTournamentObject->GetObject(TEXT("TournamentPageCorps_mc"));
			break;
		}
	case rlRoleArenaScore:
		{
			pPage = pTournamentObject->GetObject(TEXT("TournamentPageLadder_mc"));
			break;
		}
	case rlPkValue:
		{
			pPage = pTournamentObject->GetObject(TEXT("TournamentPagePK_mc"));
			break;
		}
	case rlTotalKillCount:
		{
			pPage = pTournamentObject->GetObject(TEXT("TournamentPageKill_mc"));
			break;
		}*/
	//case rlDungeonNormalScore:
	//	{
	//		pPage = pTournamentObject->GetObject(TEXT("TournamentPageFB_mc"));
	//		break;
	//	}
	default:
		{
			//ASSERT_CHECK_ERROR_RETURN(NULL && TEXT("GetCurrentRangeIndex occor error!"));
			return;
			break;
		}
	}
	//ASSERT_CHECK_ERROR_RETURN(pPage);
	if (NULL == pPage)
	{
		return;
	}
	UGFxObject* pCategoryButton0 = pPage->GetObject(TEXT("category0_btn_mc"));
	UGFxObject* pCategoryButton1 = pPage->GetObject(TEXT("category1_btn_mc"));
	UGFxObject* pCategoryButton2 = pPage->GetObject(TEXT("category2_btn_mc"));
	UGFxObject* pCategoryButton3 = pPage->GetObject(TEXT("category3_btn_mc"));
	UGFxObject* pCategoryButton4 = pPage->GetObject(TEXT("category4_btn_mc"));
	if (index == 0)
	{
		if(pCategoryButton0)
			pCategoryButton0->SetBool(TEXT("selected"), true);
	}
	else if (index == 1)
	{
		if (pCategoryButton1)
			pCategoryButton1->SetBool(TEXT("selected"), true);
	}
	else if (index ==2)
	{
		if (pCategoryButton2)
			pCategoryButton2->SetBool(TEXT("selected"), true);
	}
	else if (index == 3)
	{
		if (pCategoryButton3)
			pCategoryButton3->SetBool(TEXT("selected"), true);
	}
	else if (index == 4)
	{
		if (pCategoryButton4)
			pCategoryButton4->SetBool(TEXT("selected"), true);
	}
}

void UMMOUIRankListPanel::UpdateMaxPage(  INT curPage, INT maxPage )
{
	UBOOL preEnable = TRUE;
	UBOOL nextEnable = TRUE;
	if( curPage == 0 )
	{
		preEnable = FALSE;
	}
	if( curPage + 1 >= maxPage )
	{
		nextEnable = FALSE;
	}

	if( GFXPlayer )
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
		if(data)
		{
			data->SetBool( TEXT("preEnable") , preEnable );
			data->SetBool( TEXT("nextEnable") , nextEnable );
			data->SetString( TEXT("PageNumTxt") , FString::Printf( TEXT("%d/%d") , curPage + 1 , maxPage ) );
			data->SetInt(TEXT("MaxPage"),maxPage);
			eventCallSetRankPageButton( data );
		}
		UpdateRangeBtn(curPage+1);
	}
}

void UMMOUIRankListPanel::UpdateTimeText()
{
	FString timeUpdate = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Rank.Time"));
	eventCallUpdateTime(timeUpdate);
}

INT UMMOUIRankListPanel::GetRanking(UINT8 lastRank, INT currentRank)
{
	if (lastRank == 0 || lastRank == currentRank)
	{
		return Ranking_Deafult;
	}
	else if(lastRank > currentRank)
	{
		return Ranking_Up;
	}
	else if (lastRank <currentRank )
	{
		return Ranking_Down;
	}

	return Ranking_Deafult;
}

UBOOL UMMOUIRankListPanel::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

void UMMOUIRankListPanel::BeforeViewUpdate(float DeltaTime)
{
	if ( this->GetVisible() && refresh )
	{
		if ( currentTime >= SHOW_TIME )
		{
			currentTime = 0;
			refresh = FALSE;
			UpdateRankList();

			//Bichi Overlord real-time update
			//if (RankListManager::getSingleton().GetType() == rlActivity01)
			//{
				//OnActivityTreeItemClick(1);
			//}
			
		}

		currentTime += DeltaTime;
	}

	CheckShowRank(DeltaTime);
}

void UMMOUIRankListPanel::CheckShowRank(float DeltaTime)
{
	if (RankListManager::getSingleton().IsShowRank() == FALSE)
	{
		if (showRankTime >= SHOW_RANK_TIME)
		{
			// Current time
			__time32_t today;
			_time32(&today);
			struct tm *todayTimeTemp = _localtime32(&today);
			struct tm todayTime = *todayTimeTemp;

			struct tm serverTime = RankListManager::getSingleton().GetShowRankTime();

			if (todayTime.tm_mday - serverTime.tm_mday > 0)
			{
				RankListManager::getSingleton().SetIsShowRank(TRUE);
				if (this->GetVisible())
				{
					eventCallUCShowRank(TRUE);
				}
			}

			showRankTime = 0;
		}
		showRankTime += DeltaTime;
	}
}

void UMMOUIRankListPanel::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUIRankListPanel::SetCurrentActivity()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		switch(mainPlayer->GetClassId())
		{
		case EPCT_PlayerClassA:
			case EPCT_PlayerClassG:
				_currentActivityType = rlActivity02;
			break;
		case EPCT_PlayerClassB:
			case EPCT_PlayerClassH:
				_currentActivityType = rlActivity03;
			break;
		case EPCT_PlayerClassE:
				_currentActivityType = rlActivity04;
			break;
		default:
				_currentActivityType = rlActivity02;
			break;
		}
	}
}
