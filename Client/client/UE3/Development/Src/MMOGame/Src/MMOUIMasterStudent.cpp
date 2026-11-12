#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "MasterStudentManager.h"
#include "MMOMsgButtonManager.h"
#include "MMOChatManager.h"
#include "../net/message_handler_Quest.h"
#include "MMOGuideManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOUIMasterStudentPanel)

#define ASSERT_RETURN_VOID(p) {if(!(p))return;}

void UMMOUIMasterStudentPanel::Init()
{
	Super::Init();

	_ConfirmBox = NULL;

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MasterStudent);
	evts.AddItem(GameEvent_PlayerBaseInfo);
	EventAdapter = new TGameEventAdapter<UMMOUIMasterStudentPanel>(this, evts);
}
//
void UMMOUIMasterStudentPanel::UpdatePanelInfo()
{
	switch( _CurPanelType )
	{
	case EMSTPT_Outline:
		{
			UpdateMasterInfoList();
		}
		break;
	case EMSTPT_MasterPoster:
		{
			UpdateMasterPosterList();
		}
		break;
	case EMSTPT_MasterApply:
		{
			UpdateMasterApplyList();
		}
		break;
	case EMSTPT_StudentPoster:
		{
			UpdateStudentPosterList();
		}
		break;
	case EMSTPT_StudentApply:
		{
			UpdateStudentApplyList();
		}
		break;
	}
}
//Master's sect info
void UMMOUIMasterStudentPanel::UpdateMasterInfoList()
{
	INT idx = 0;
	//bool bShow = false;

	//Own current state
	INT myState = MasterStudentManager::getSingleton().GetMasterStudentState();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if (NULL != dataProvider)
	{
		const MasterDetailInfo& masterInfo = MasterStudentManager::getSingleton().GetMasterDetailInfo();
		if (masterInfo.nRoleID > 0)
		{
			//bShow = true;

			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Master.MasterInfoListItemData"));

			INT nNum = MasterStudentManager::getSingleton().GetSudentNum();
			INT nMaxStudent = 3;
			
			INT nMasterExp =  MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_MASTER_EXP);
			/*if(nMasterExp < 300)
			nMaxStudent = 3;
			else if(nMasterExp < 500)
			nMaxStudent = 4;
			else
			nMaxStudent = 5;*/
			nMaxStudent = MasterStudentManager::getSingleton().GetMasterStudentNum(nMasterExp);
			nMasterExp = MasterStudentManager::getSingleton().GetMasterExpByRatio(nMasterExp);

			eventCallUpdateMasterExp(nMasterExp );
			eventCallUpdateStudentNum(nNum,nMaxStudent);

			PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(masterInfo.nRoleID, TRUE);//data is small, request it again here

			itemObj->SetInt(TEXT("Id"), masterInfo.nRoleID );
			if(playerInfo)
			{
				itemObj->SetString(TEXT("Name"), playerInfo->_RoleName);
				itemObj->SetInt(TEXT("Level"), masterInfo.nLevel);
				FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"),playerInfo->_RoleClass));
				itemObj->SetString(TEXT("Profession"), sProfession );
			}
			else
			{
				++_IncompleteInfoCnt;
				itemObj->SetString(TEXT("Name"), masterInfo.masterName);
				itemObj->SetInt(TEXT("Level"), masterInfo.nLevel );
				FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), masterInfo.nClass));
				itemObj->SetString(TEXT("Profession"), sProfession );
			}
			
			INT conCount = MasterStudentManager::getSingleton().GetCongratulationCount(masterInfo.nCZLevel, masterInfo.nLevel);
			itemObj->SetInt(TEXT("congCount"),conCount );
			bool CanCongraluate = conCount > 0? true:false;
			itemObj->SetBool(TEXT("CanCongratulate"), CanCongraluate );
			itemObj->SetBool(TEXT("bMaster"), true );
			itemObj->SetInt(TEXT("exp"),masterInfo.reward.nExp);
			itemObj->SetInt(TEXT("gold"),masterInfo.reward.nMoney);
			itemObj->SetInt(TEXT("MasterValue"),MasterStudentManager::getSingleton().GetMasterExpByRatio(masterInfo.reward.nMasterExp));
			
			FString congratulateType = TEXT("");
			if(conCount > 0 )
				congratulateType = MMOUI::ParseLocalizedPropertyPath( TEXT( "MMOGame.Master.CongraluationLvUp") );
			itemObj->SetString(TEXT("CongratulateType"), congratulateType );
			//Do not show if self is the master
			AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
			if(pl&&pl->ServerID != masterInfo.nRoleID)
				dataProvider->SetElementObject(idx++, itemObj);
		}

		const StudentList& StudentDetails = MasterStudentManager::getSingleton().GetStudentList();
		StudentList::const_iterator itr = StudentDetails.begin();
		while (/*bShow &&*/ itr != StudentDetails.end())
		{
			const StudentListData stuData = *itr;
			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Master.MasterInfoListItemData"));
			if( itemObj != NULL /*&& stuData.nCZLevel < GRADUATE_LEVEL*/ )
			{
				itemObj->SetInt(TEXT("Id"), stuData.nRoleID );

				PlayerBaseInfo *playerInfo = NULL;

				if (stuData.nRoleID > 0)
				{
					playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(stuData.nRoleID);
				}
				

				if(playerInfo)
				{
					itemObj->SetString(TEXT("Name"), playerInfo->_RoleName);
					itemObj->SetInt(TEXT("Level"), stuData.nLevel );
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), playerInfo->_RoleClass));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				else
				{
					++_IncompleteInfoCnt;
					itemObj->SetString(TEXT("Name"), stuData.RoleName);
					itemObj->SetInt(TEXT("Level"), stuData.nLevel );
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), stuData.nClass));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				//If the last congratulation >= current level, congratulation is not possible
				
				bool CanCongraluate = true;
				INT conCount = MasterStudentManager::getSingleton().GetCongratulationCount(stuData.nCZLevel, stuData.nLevel);

				if (myState != E_MASTER_TEACHER)
				{
					CanCongraluate = false;
					conCount = 0;
				}
				//bool CanCongraluate = conCount > 0? true:false;
				itemObj->SetBool(TEXT("CanCongratulate"), CanCongraluate );
				itemObj->SetInt(TEXT("congCount"),conCount );

				//Last congratulation was for graduation
				FString congratulateType = TEXT("");
				if( CanCongraluate && conCount > 0 )
					congratulateType = MMOUI::ParseLocalizedPropertyPath( TEXT( "MMOGame.Master.CongraluationLvUp") );
				else
					congratulateType = MMOUI::ParseLocalizedPropertyPath( TEXT( "MMOGame.Master.CongraluationGraduated") );
				itemObj->SetString(TEXT("CongratulateType"), congratulateType );
				itemObj->SetBool(TEXT("bMaster"), false );
				itemObj->SetInt(TEXT("nState"), stuData.nState);

				//Whether to gray out
				itemObj->SetBool(TEXT("bLeaveMaster"),stuData.bLeaveMaster);
				itemObj->SetInt(TEXT("exp"),stuData.reward.nExp);
				itemObj->SetInt(TEXT("gold"),stuData.reward.nMoney);
				itemObj->SetInt(TEXT("MasterValue"),MasterStudentManager::getSingleton().GetMasterExpByRatio(stuData.reward.nMasterExp));

				//Whether it is the reward accumulated earlier with the master bSelfReward
				itemObj->SetBool(TEXT("bSelfReward"),stuData.reward.bSelfReward);

				dataProvider->SetElementObject(idx++, itemObj);
			}
			++itr;
		}

		//if (bShow && itr_Show != StudentDetails.end())
		//{
			eventCallUpdateMasterInfoListData(dataProvider);
		//}
		
	}
}
//Master application table
void UMMOUIMasterStudentPanel::UpdateMasterApplyList()
{
	const ApllicantsList& masterList = MasterStudentManager::getSingleton().GetMasterApllyList();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if (NULL != dataProvider)
	{
		ApllicantsList::const_iterator itr = masterList.begin();
		for (INT32 nIndex = 0; (itr != masterList.end())/*&& nIndex < MAX_ROW_MASTERLIST*/; ++itr, ++nIndex)
		{
			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Master.MasterData"));
			if (NULL != itemObj)
			{
				itemObj->SetInt(TEXT("Id"), (*itr).nRoleID);
				PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*itr).nRoleID);
				if(playerInfo)
				{
					itemObj->SetString(TEXT("Name"), playerInfo->_RoleName);
					itemObj->SetInt(TEXT("Level"), playerInfo->_Level);
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), playerInfo->_RoleClass ));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				else
				{
					++_IncompleteInfoCnt;
					itemObj->SetString(TEXT("Name"), (*itr).RoleName);
					itemObj->SetInt(TEXT("Level"), (*itr).nLevel );
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), (*itr).nClass ));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
			
				itemObj->SetInt(TEXT("Score"), MasterStudentManager::getSingleton().GetMasterExpByRatio((*itr).nMasterExp));

				dataProvider->SetElementObject(nIndex, itemObj);
			}
		}
	}
	eventCallUpdateMasterApplyListData(dataProvider);
}
//Recommended master table
void UMMOUIMasterStudentPanel::UpdateMasterPosterList()
{
	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_StudentApply);
	MMOUI::PostGameEvent(&evtGuide);

	INT nCurPage = MasterStudentManager::getSingleton().GetSystemPosterCurPage();
	INT nMaxPage = MasterStudentManager::getSingleton().GetSystemPosterMaxPage();
	const SystemPosterList& masterList = MasterStudentManager::getSingleton().GetMasterSystemList();
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if (NULL != dataProvider)
	{
		SystemPosterList::const_iterator itr = masterList.begin();
		//advance(itr,MAX_ROW_MASTERLIST*nCurPage);
		for (INT32 nIndex = 0; (itr != masterList.end())/*&& nIndex < MAX_ROW_MASTERLIST*/; ++itr, ++nIndex)
		{
			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Master.MasterData"));
			if (NULL != itemObj)
			{
				itemObj->SetInt(TEXT("Id"), (*itr).nRoleID);
				PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*itr).nRoleID);
				if(playerInfo)
				{
					itemObj->SetString(TEXT("Name"), playerInfo->_RoleName);
					itemObj->SetInt(TEXT("Level"), playerInfo->_Level );
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), playerInfo->_RoleClass));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				else
				{
					++_IncompleteInfoCnt;
					itemObj->SetString(TEXT("Name"), (*itr).RoleName);
					itemObj->SetInt(TEXT("Level"), (*itr).nLevel );
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), (*itr).nClass ));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				
				itemObj->SetInt(TEXT("Score"), MasterStudentManager::getSingleton().GetMasterExpByRatio((*itr).nMasterExp));
				int CanApply = (*itr).bCanApply?1:0;
				itemObj->SetBool(TEXT("bCanApply"), (*itr).bCanApply);
				FString sCanApply = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.master.CanApply") + FString::Printf(TEXT("%d"),CanApply  ));
				itemObj->SetString(TEXT("Status"), sCanApply );
				itemObj->SetBool(TEXT("IsSelected"), (*itr).bSelect);

				dataProvider->SetElementObject(nIndex, itemObj);
			}
		}
	}
	eventCallUpdateMasterPosterListData(dataProvider);
}
//Apprentice application table
void UMMOUIMasterStudentPanel::UpdateStudentApplyList()
{
	INT nIndex = 0;

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(NULL != dataProvider)
	{
		const ApllicantsList& studentList = MasterStudentManager::getSingleton().GetStudentApllyList();
		ApllicantsList::const_iterator itr = studentList.begin();
		for (; itr != studentList.end(); ++itr)
		{
			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Master.StudentData"));
			if(itemObj != NULL )
			{
				itemObj->SetInt(TEXT("Id"),(*itr).nRoleID);
				PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*itr).nRoleID);
				if(playerInfo)
				{
					itemObj->SetString(TEXT("Name"),playerInfo->_RoleName);
					itemObj->SetInt(TEXT("Level"), playerInfo->_Level);
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"),playerInfo->_RoleClass));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				else
				{
					++_IncompleteInfoCnt;
					itemObj->SetString(TEXT("Name"),(*itr).RoleName);
					itemObj->SetInt(TEXT("Level"), (*itr).nLevel);
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"),(*itr).nClass));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				
				//itemObj->SetBool(TEXT("IsSelected"), (*itr).);
				dataProvider->SetElementObject(nIndex++, itemObj);
			}
		}
		eventCallUpdateStudentApplyListData(dataProvider);
	}
}
//Apprentice recommendation table
void UMMOUIMasterStudentPanel::UpdateStudentPosterList()
{
	INT nIndex = 0;

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(NULL != dataProvider)
	{
		const SystemPosterList& studentList = MasterStudentManager::getSingleton().GetStudentSystemList();
		SystemPosterList::const_iterator itr = studentList.begin();
		for (; itr != studentList.end(); ++itr)
		{
			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Master.StudentData"));
			if(itemObj != NULL )
			{
				itemObj->SetInt(TEXT("Id"),(*itr).nRoleID);
				PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo((*itr).nRoleID);
				if(playerInfo)
				{
					itemObj->SetString(TEXT("Name"),playerInfo->_RoleName);
					itemObj->SetInt(TEXT("Level"), playerInfo->_Level);
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"),playerInfo->_RoleClass));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				else
				{
					++_IncompleteInfoCnt;
					itemObj->SetString(TEXT("Name"),(*itr).RoleName);
					itemObj->SetInt(TEXT("Level"), (*itr).nLevel);
					FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"),(*itr).nClass));
					itemObj->SetString(TEXT("Profession"), sProfession );
				}
				itemObj->SetBool(TEXT("bCanApply"), (*itr).bCanApply);
				dataProvider->SetElementObject(nIndex++, itemObj);
			}
		}
		eventCallUpdateStudentPosterListData(dataProvider);
	}
}

void UMMOUIMasterStudentPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (NULL == evt)
	{
		return;
	}
	switch(evt->GetEventType())
	{
	case GameEvent_MasterStudent:
		{
			MMOGameEvent_MasterStudent* pEvt_MasterStudent = (MMOGameEvent_MasterStudent*)evt;
			if(NULL == pEvt_MasterStudent)
			{
				break;
			}

			if (this->GetVisible() ==FALSE )
			{
				break;
			}
			switch(pEvt_MasterStudent->EventType)
			{
			case EMST_MasterInfo:
				{
					if( _CurPanelType == EMSTPT_Outline)
					{
						UpdateMasterInfoList();
					}
					break;
				}
			case EMST_MasterSystemPosterList:
				{
					if( _CurPanelType == EMSTPT_MasterPoster )
					{
						UpdateMasterPosterList();
					}
					break;
				}
			case EMST_StudentApplicantsList:
				{
					if( _CurPanelType == EMSTPT_StudentApply )
					{
						UpdateStudentApplyList();
					}
					break;
				}
			case  EMST_StudentSystemPosterList:
				{
					if( _CurPanelType == EMSTPT_StudentPoster )
					{
						UpdateStudentPosterList();
					}
					break;
				}
			case EMST_MasterApplicantsList:
				{
					if( _CurPanelType == EMSTPT_MasterApply )
					{
						UpdateMasterApplyList();
					}
					break;
				}
			case EMST_RecruitStatus:
				{
					INT nState = (E_MST_APPLY_RECRUIT_SUCC == MasterStudentManager::getSingleton().GetMasterRecruitStatus())?1:0;
					eventCallUCUpdateRecruitState(nState);
					break;
				}
			case EMST_ChangeValue:
				{
					INT nNum = MasterStudentManager::getSingleton().GetSudentNum();
					INT nMaxStudent = 3;

					INT nMasterExp =  MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_MASTER_EXP);
					/*if(nMasterExp < 300)
						nMaxStudent = 3;
					else if(nMasterExp < 500)
						nMaxStudent = 4;
					else
						nMaxStudent = 5;*/
					nMaxStudent = MasterStudentManager::getSingleton().GetMasterStudentNum(nMasterExp);
					nMasterExp = MasterStudentManager::getSingleton().GetMasterExpByRatio(nMasterExp);

					eventCallUpdateMasterExp(nMasterExp );
					eventCallUpdateStudentNum(nNum,nMaxStudent);
					break;
				}
			case EMST_MasterStateChange:
				{
					EMASTER_STATE state = MasterStudentManager::getSingleton().GetMasterStudentState();
					INT nSelfState = MasterStudentManager::getSingleton().GetSelfState();
					eventCallUpdateStateChange((INT)state,nSelfState);
					INT nState = (E_MST_APPLY_RECRUIT_SUCC == MasterStudentManager::getSingleton().GetMasterRecruitStatus())?1:0;
					eventCallUCUpdateRecruitState(nState);

					break;
				}
			case EMST_OnClickMasterStudent:
				{
					UpdateClickItem(pEvt_MasterStudent->ID);
					break;
				}
			case EMST_CDTime:
				{
					CheckCDTime();
				}
				break;
			case EMST_StudentGrad:
				{
					if (this->GetVisible())
					{
						if(GetCurPanelType() == EMSTPT_Outline) 
						{
							ASClickMasterTap(EMSTPT_Outline);
						}
					}
				}
				break;
			case EMST_MasterTwitterType:
				{
					ShowTwitterCheckBox();
				}
				break;
			case EMST_PlayerExpChange:
				{
					CMessageHandler::GetInstance().Send_QueryMasterExp();
				}
				break;
			default:
				break;
			}
		}
		break;
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* msg = (MMOGameEvent_PlayerBaseInfo*)evt;
			if( msg != NULL )
			{
				if( _IncompleteInfoCnt > 0 ){
					--_IncompleteInfoCnt;
					if( _IncompleteInfoCnt == 0 ){
						MasterStudentManager::getSingleton().UpdateMasterInfoForCompletion();
						MasterStudentManager::getSingleton().UpdateAllStudentInfoForCompletion();
						MasterStudentManager::getSingleton().UpdateAllApllicantsInfoForCompletion();
						MasterStudentManager::getSingleton().UpdateAllSystemPosterInfoForCompletion();

						UpdatePanelInfo();
					}
				}
			}
		}
		break;
	}
}

void UMMOUIMasterStudentPanel::ShowTwitterCheckBox()
{
	UINT8 twitter = MasterStudentManager::getSingleton().GetMasterTwitter();
	EMASTER_STATE state = MasterStudentManager::getSingleton().GetMasterStudentState();
	BOOL show = state == E_MASTER_TEACHER? TRUE : FALSE;
	eventCallUCsetMasterInfoCheckBox(twitter,show);
}

void UMMOUIMasterStudentPanel::UnInit()
{
	UMMOUIBase::UnInit();
}

void UMMOUIMasterStudentPanel::PostOnShow(UBOOL v)
{
	UMMOUIBase::PostOnShow(v);
	if (v)
	{
		if( NULL == Player || NULL == Player->ActorAttributes)
			return;
		EMASTER_STATE state = MasterStudentManager::getSingleton().GetMasterStudentState();
		int PlayerLv = Player->ActorAttributes->LevelAttributes.Level;
		if( PlayerLv < STUD_LEVEL_LOWER_LIMIT  )
		{
			//If display conditions are not met, hide again
			FString LevNotEnough = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.LvNotEnough")), STUD_LEVEL_LOWER_LIMIT);
			MMOUI::GetUIManager()->delegateAddScreenMessage(LevNotEnough,MsgInfoType_Table,TEXT("MMOGame.Master.LvNotEnough"));
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterStudent,FALSE, "");
			return;
		}
		else if(PlayerLv >= GRADUATE_LEVEL)
		{
			const MasterDetailInfo& masterInfo = MasterStudentManager::getSingleton().GetMasterDetailInfo();
			if(state != E_MASTER_TEACHER)
			{
				if(masterInfo.nRoleID > 0)
				{
					FString strInfo = FString::Printf(TEXT("You have reached level %d and can graduate"),GRADUATE_LEVEL);
					MMOUI::GetUIManager()->delegateAddScreenMessage(strInfo);
					MMOChatManager::getSingleton().AddSystemMessage(strInfo);
				}
				/*else
				{
					MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Master.GetTobeMaster") ) ,MsgInfoType_Table,TEXT("MMOGame.Master.GetTobeMaster"));
					MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Master.GetTobeMaster")));
				}*/
			}
		}
		
		INT nSelfState = MasterStudentManager::getSingleton().GetSelfState();
		eventCallUpdateStateChange((INT)state,nSelfState);
		if(state == E_MASTER_TEACHER)
		{
			INT nState = (E_MST_APPLY_RECRUIT_SUCC == MasterStudentManager::getSingleton().GetMasterRecruitStatus())?1:0;
			eventCallUCUpdateRecruitState(nState);
		}

		ShowTwitterCheckBox();

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowMasterPanel);
		MMOUI::PostGameEvent(&evtGuide);

		CheckCDTime();
	}
	else
	{
		CloseAllPopUpPanel();

		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,FALSE,"");
	}
}

//PostAdvance
void UMMOUIMasterStudentPanel::PostAdvance(float DeltaTime)
{
	if (this->GetVisible())
	{
		static float ftempTime = DeltaTime;
		if (0 < MasterStudentManager::getSingleton().GetCDTime()
			&& ftempTime < DeltaTime + 100)
		{
			ftempTime = DeltaTime;
			OnUpdateCDTime();
		}
	}
	

}

void UMMOUIMasterStudentPanel::CheckCDTime()
{
	BOOL show = TRUE;

	if (MasterStudentManager::getSingleton().GetCDTime() <= 0)
	{
		show = FALSE;
	}

	UGFxObject* pMasterListPanel = NULL;

	if(MasterStudentManager::getSingleton().GetMasterStudentState() == E_MASTER_TEACHER)
	{
		pMasterListPanel = GetObject("StudentPosterList_mc");
		
	}
	else
	{
		pMasterListPanel = GetObject("MasterPosterList_mc");
	}
	 
	if (NULL != pMasterListPanel)
	{
		UGFxObject* tF_tips = pMasterListPanel->GetObject("tF_tips");
		if (NULL != tF_tips)
		{
			tF_tips->SetBool(TEXT("visible"), show);
		}
	}
}

void UMMOUIMasterStudentPanel::OnUpdateCDTime()
{
	UGFxObject* pMasterListPanel = NULL;

	INT currentType = MasterStudentManager::getSingleton().GetMasterStudentState();

	if(currentType == E_MASTER_NONE)
	{
		pMasterListPanel = GetObject("MasterPosterList_mc");
	}
	else if(currentType == E_MASTER_TEACHER)
	{
		pMasterListPanel = GetObject("StudentPosterList_mc");
	}

	if (NULL != pMasterListPanel)
	{
		UGFxObject* tF_tips = pMasterListPanel->GetObject("tF_tips");
		if (NULL != tF_tips)
		{
			if(0 == MasterStudentManager::getSingleton().GetCDTime())
			{
				if(tF_tips->GetBool(TEXT("visible")))
					tF_tips->SetBool(TEXT("visible"), false);
			}
			else if (0 < MasterStudentManager::getSingleton().GetCDTime())
			{
				int curTime = time(NULL);
				int diffTime = MasterStudentManager::getSingleton().GetCDTime() - curTime;
				if (0 > diffTime)
				{
					if(tF_tips->GetBool(TEXT("visible")))
						tF_tips->SetBool(TEXT("visible"), false);
				}
				else
				{
					if(!tF_tips->GetBool(TEXT("visible")))
						tF_tips->SetBool(TEXT("visible"), true);

					FString strText;

					if (currentType == E_MASTER_NONE)
					{
						strText = MMOFacade::getSingleton().TimeDisplay((DWORD)diffTime) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CDTime"));
					}
					else if(currentType == E_MASTER_TEACHER)
					{
						strText = MMOFacade::getSingleton().TimeDisplay((DWORD)diffTime) + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Student.CDTime"));
					}
					 
					tF_tips->SetString(TEXT("text"),  strText);
				}
			}
		}
	}
}
//==========================================================================================
void UMMOUIMasterStudentPanel::CloseAllPopUpPanel()
{
	if( _ConfirmBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ConfirmBox );
		_ConfirmBox = NULL;
	}
}
//==========================================================================================
void UMMOUIMasterStudentPanel::PopUpMessageBoxToConfirmForceGraduate()
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT( "OnConfirmForceGraduate" );

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT( "OnCancelForceGraduate" );

	FString msgText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Master.ForceGraduateConfirmText") );
	_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msgText, 0, funYes, funNo );
}
//
void UMMOUIMasterStudentPanel::OnConfirmForceGraduate( UGFxObject *mc )
{
	_ConfirmBox = NULL;
	CMessageHandler::GetInstance().Send_ForceGrad();

	//Close congratulation UI
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,FALSE,"");
}
//
void UMMOUIMasterStudentPanel::OnCancelForceGraduate( UGFxObject *mc )
{
	_ConfirmBox = NULL;
}
//==========================================================================================
BYTE UMMOUIMasterStudentPanel::GetCurPanelType()
{
	return _CurPanelType;
}

void UMMOUIMasterStudentPanel::SetCurPanelType(BYTE type)
{
	_CurPanelType = type;
}

//----------------moved-----------------------
void UMMOUIMasterStudentPanel::ASClearMasterApplyList()
{
	//reject all
	const ApllicantsList& list = MasterStudentManager::getSingleton().GetMasterApllyList();
	for (ApllicantsList::const_iterator itr  = list.begin(); itr != list.end(); ++itr)
	{
		CMessageHandler::GetInstance().Send_RefuseMasterApply((*itr).nRoleID);
	}
}
void UMMOUIMasterStudentPanel::ASAcceptMasterApplyList(INT nID)
{
	CMessageHandler::GetInstance().Send_AgreeMasterdApply(nID);
}
void UMMOUIMasterStudentPanel::ASDenyMasterApplyList(INT nID)
{
	CMessageHandler::GetInstance().Send_RefuseMasterApply(nID);
}
void UMMOUIMasterStudentPanel::ASMasterApplyListNextPage()
{
	INT nPage = MasterStudentManager::getSingleton().GetApplyCurPage();
	if(++nPage <= MasterStudentManager::getSingleton().GetApplyMaxPage())
		MasterStudentManager::getSingleton().SetApplyCurPage(nPage);
}
void UMMOUIMasterStudentPanel::ASMasterApplyListPrevPage()
{
	INT nPage = MasterStudentManager::getSingleton().GetApplyCurPage();
	if(--nPage >= 0)
		MasterStudentManager::getSingleton().SetApplyCurPage(nPage);
}
void UMMOUIMasterStudentPanel::ASSortMasterApplyList(INT nType)
{

}
void UMMOUIMasterStudentPanel::ASSortMasterPosterList(INT nType)
{

}
void UMMOUIMasterStudentPanel::ASSortStudentPosterList(INT nType)
{

}
void UMMOUIMasterStudentPanel::ASSortStudentApplyList(INT nType)
{

}
void UMMOUIMasterStudentPanel::ASMasterPosterListRefresh()
{
	CMessageHandler::GetInstance().Send_MasterListAck();
}
void UMMOUIMasterStudentPanel::ASApplyMasterPosterList(INT nID)
{
	CMessageHandler::GetInstance().Send_SubmitApplyToMaster(nID);
}
void UMMOUIMasterStudentPanel::ASCancelMasterPosterList(INT nID)
{
	CMessageHandler::GetInstance().Send_CancelStudentApplyBySelf(nID);
}
void UMMOUIMasterStudentPanel::ASMasterPosterListNextPage()
{

}
void UMMOUIMasterStudentPanel::ASMasterPosterListPrevPage()
{

}
void UMMOUIMasterStudentPanel::ASClickMasterPosterList(INT nID)
{

}
void UMMOUIMasterStudentPanel::ASLeaveMaster()
{
	FScriptDelegate LeaveMasterDelegate(EC_EventParm);
	LeaveMasterDelegate.FunctionName = TEXT("OnLeaveMaster");
	LeaveMasterDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnMasterCancel");
	Nofunction.Object = this;

	FString szText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MasterStudent.StudentLeaveMaster"));
	_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( szText , 0 , LeaveMasterDelegate , Nofunction );


}
void UMMOUIMasterStudentPanel::OnLeaveMaster()
{
	CMessageHandler::GetInstance().Send_LeaveMaster();
	_ConfirmBox = NULL;
}

void UMMOUIMasterStudentPanel::ASFireStudent(INT nID)
{
	FScriptDelegate FireStudentDelegate(EC_EventParm);
	FireStudentDelegate.FunctionName = TEXT("OnFireStudent");
	FireStudentDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnMasterCancel");
	Nofunction.Object = this;

	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(nID);

	FString szText;

	if (playerInfo)
	{
		szText = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MasterStudent.IsFireStudent")) , *playerInfo->_RoleName);
	}
	else
	{
		FString name = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Guild.UnKnowRole"));
		szText = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MasterStudent.IsFireStudent")),*name);
	}

	_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( szText , 0 , FireStudentDelegate , Nofunction );

	if(_ConfirmBox)
		_ConfirmBox->SetInt( TEXT("dataid") , nID);
}


void UMMOUIMasterStudentPanel:: OnFireStudent(UGFxObject* mc)
{
	INT nID = mc->GetInt(TEXT("dataid"));
	CMessageHandler::GetInstance().Send_FireStudent(nID);
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,FALSE,"");
	_ConfirmBox = NULL;
}

void UMMOUIMasterStudentPanel:: OnMasterCancel()
{
	_ConfirmBox = NULL;
}

void UMMOUIMasterStudentPanel::ASClickMasterInfoList(INT nID)
{

}

void UMMOUIMasterStudentPanel::ASRightMenuMasterApplyList(INT nID)
{
	if (0 < nID)
	{
		const ApplicantsListData* pMaster = MasterStudentManager::getSingleton().GetApplicantsDataByID(nID);	
		if(pMaster == NULL)
			return;
		FIntPoint MousePos = MMOFacade::GetMouseCursorPosition();
		MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_MasterStudent ,
			MousePos.X , MousePos.Y, pMaster->nRoleID, pMaster->RoleName, 0 );
	}
}
void UMMOUIMasterStudentPanel::ASRightMenuStudentPosterList(INT nID)
{
	if (0 < nID)
	{
		const SystemPosterListData* pMaster = MasterStudentManager::getSingleton().GetSystemPosterDataByID(nID);
		if(pMaster == NULL)
			return;
		FIntPoint MousePos = MMOFacade::GetMouseCursorPosition();
		MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_MasterStudent ,
			MousePos.X , MousePos.Y, pMaster->nRoleID,pMaster->RoleName , 0 );
	}
}
void UMMOUIMasterStudentPanel::ASApplyStudentPosterList(INT nID)
{
	CMessageHandler::GetInstance().Send_SubmitApplyToStudent(nID);
}
void UMMOUIMasterStudentPanel::ASCancelStudentPosterList(INT nID)
{
	CMessageHandler::GetInstance().Send_CancelMasterApplyBySelf(nID);
	MasterStudentManager::getSingleton().ClearStudentTwitterInfo(nID);
}
void UMMOUIMasterStudentPanel::ASRefreshStudentPosterList()
{
	CMessageHandler::GetInstance().Send_SoloStudentListAck();
}
void UMMOUIMasterStudentPanel::ASClickStudentPosterList(INT nID)
{

}
void UMMOUIMasterStudentPanel::ASStudentPostListPage(INT nPage)
{

}
void UMMOUIMasterStudentPanel::ASAcceptStudentApplyList(INT nID)
{
	CMessageHandler::GetInstance().Send_AgreeStudentdApply(nID);
}
void UMMOUIMasterStudentPanel::ASDenyStudentApplyList(INT nID)
{
	CMessageHandler::GetInstance().Send_RefuseStudentApply(nID);
}
void UMMOUIMasterStudentPanel::ASClearStudentApplyList()
{
	const ApllicantsList& list = MasterStudentManager::getSingleton().GetStudentApllyList();
	for (ApllicantsList::const_iterator itr  = list.begin(); itr != list.end(); ++itr)
	{
		CMessageHandler::GetInstance().Send_RefuseStudentApply((*itr).nRoleID);
	}
}
void UMMOUIMasterStudentPanel::ASClickStudentApplyList(INT nID)
{

}
void UMMOUIMasterStudentPanel::ASRightMenuStudentApplyList(INT nID)
{
	if (0 < nID)
	{
		const ApplicantsListData* pMaster = MasterStudentManager::getSingleton().GetApplicantsDataByID(nID);	
		if(pMaster == NULL)
			return;
		FIntPoint MousePos = MMOFacade::GetMouseCursorPosition();
		MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_MasterStudent ,
			MousePos.X , MousePos.Y, pMaster->nRoleID, pMaster->RoleName, 0 );
	}
}
void UMMOUIMasterStudentPanel::ASClickStartRecruit()
{
	CMessageHandler::GetInstance().Send_ApplyRecruitStudent();
}
void UMMOUIMasterStudentPanel::ASClickStopRecruit()
{
	CMessageHandler::GetInstance().Send_CancelRecruitStudent();
}
void UMMOUIMasterStudentPanel::ASClickAcceptLeave(INT nID)
{
	CMessageHandler::GetInstance().Send_AgreeGrad(nID);
}
void UMMOUIMasterStudentPanel::ASClickCongratulation(INT nID)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,TRUE, "");
	UMMOUIMasterCongratulationMain* _MasterCongMain = (UMMOUIMasterCongratulationMain*)MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_MasterCongMain);
	if(_MasterCongMain)
		_MasterCongMain->SetCongratulationData(nID);
}
void UMMOUIMasterStudentPanel::ASClickMasterTap(INT nID)
{
	EMASTER_STATE state = MasterStudentManager::getSingleton().GetMasterStudentState();

	MasterPanelType panelType = (MasterPanelType)nID;
	SetCurPanelType( panelType );

	switch( panelType )
	{
	case EMSTPT_Outline:
		{
			if (state == E_MASTER_NONE)
			{
				CMessageHandler::GetInstance().Send_QueryMasterExp();
			}
			else
			{
				CMessageHandler::GetInstance().Send_GetMasterInfo();
			}
			
			
		}
		break;
	case EMSTPT_MasterPoster:
		{
			if(state != E_MASTER_TEACHER)
				CMessageHandler::GetInstance().Send_MasterListAck();
		}
		break;
	case EMSTPT_MasterApply:
		break;
	case EMSTPT_StudentPoster:
		{
			if(state == E_MASTER_TEACHER)
				CMessageHandler::GetInstance().Send_SoloStudentListAck();
		}
		break;
	case EMSTPT_StudentApply:
		break;
	}

	//
	UpdatePanelInfo();
}
void UMMOUIMasterStudentPanel::ASApplyGrad()
{
	CMessageHandler::GetInstance().Send_DesireGrad();
}
void UMMOUIMasterStudentPanel::ASForceGrad()
{
	CMessageHandler::GetInstance().Send_ForceGrad();
	//PopUpMessageBoxToConfirmForceGraduate();
}
void UMMOUIMasterStudentPanel::ASRightMenuStudentDetailList(INT nID)
{
	if (0 < nID)
	{
		INT roleId;
		FString roleName;

		const StudentListData* pMaster = MasterStudentManager::getSingleton().GetStudentDataByID(nID);	
		if(pMaster == NULL)
		{
			//whether is master
			const MasterDetailInfo& masterInfo = MasterStudentManager::getSingleton().GetMasterDetailInfo();
			if (masterInfo.nRoleID == nID)
			{
				roleId = masterInfo.nRoleID;
				roleName = masterInfo.masterName;
			}
			else
			{
				return;
			}
		}
		else
		{
			roleId = pMaster->nRoleID;
			roleName = pMaster->RoleName;

		}
		FIntPoint MousePos = MMOFacade::GetMouseCursorPosition();
		MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_MasterStudent ,
			MousePos.X , MousePos.Y, roleId, roleName, 0 );
	}
}
void UMMOUIMasterStudentPanel::ASClickCongraluate(INT nID)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,TRUE, "");
	UMMOUIMasterCongratulationMain* _MasterCongMain = (UMMOUIMasterCongratulationMain*)MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_MasterCongMain);
	if(_MasterCongMain)
		_MasterCongMain->SetCongratulationData(nID);
}
void UMMOUIMasterStudentPanel::ASRightMenuMasterList(INT nID)
{
	if (0 < nID)
	{
		const SystemPosterListData* pMaster = MasterStudentManager::getSingleton().GetSystemPosterDataByID(nID);
		if(pMaster == NULL)
			return;
		FIntPoint MousePos = MMOFacade::GetMouseCursorPosition();
		MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_MasterStudent ,
			MousePos.X , MousePos.Y, pMaster->nRoleID,pMaster->RoleName , 0 );
	}
}
void UMMOUIMasterStudentPanel::ASShowTips_MasterGift(class UGFxObject* mc,INT nType)
{
	if(mc == NULL)
		return;

	const MailTemplateData* pkMail = MasterStudentManager::getSingleton().GetMailTemplateData(nType);
	if(pkMail)
	{
		const GiftDetailData* pkGift = MasterStudentManager::getSingleton().GetGiftDeatilData(pkMail->nItemID);
		if(pkGift)
		{
			/*FString strTips = pkGift->strGiftName;
			for (INT n = 0; n < 3; ++n)
			{
				if(pkGift->nItemID[n] > 0)
				{
					UItem* item = MMOFacade::getSingleton().FindItemTemplate(pkGift->nItemID[n]);
					if(item)
					{
						strTips += "\n";
						if(pkGift->nItemCount[n] > 0)
							strTips += FString::Printf(TEXT("%s X %d"),*(item->GetDisplayName()),pkGift->nItemCount[n]);
						else
							strTips += item->GetDisplayName();
					}
				}
			}

			for (INT n = 0; n < 2; ++n)
			{
				if(pkGift->nMoneyType[n] < 0)
					continue;
				switch(pkGift->nMoneyType[n])
				{
				case E_MONEYTYPE_01:
					{
						strTips += "\n";
						strTips += FString::Printf(TEXT("%s X %d"),*MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_01")),pkGift->nMoneyNum[n]);
						break;
					}
				case E_MONEYTYPE_02:
					{
						strTips += "\n";
						strTips += FString::Printf(TEXT("%s X %d"),*MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_02")),pkGift->nMoneyNum[n]);
						break;
					}
				case E_MONEYTYPE_MORAL:
					{
						strTips += "\n";
						strTips += FString::Printf(TEXT("%s X %d"),*MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_MORAL")),pkGift->nMoneyNum[n]);
						break;
					}
					break;
				}
			}
			if(pkGift->nAddExp > 0)
			{
				strTips += "\n";
				strTips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintGasHeavenExpOne")),pkGift->nAddExp);
			}*/
			mc->SetString( TEXT("tipsHtml"), pkGift->strGiftTips );
			MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
		}
	}
}
void UMMOUIMasterStudentPanel::ASHideTips_MasterGift()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUIMasterStudentPanel::UpdateClickItem(INT id)
{
	switch( _CurPanelType )
	{
	case EMSTPT_MasterApply:
		{
			UpdateMasterApplyListSelected(id);
		}
		break;
	case EMSTPT_StudentApply:
		{
			UpdateStudentApplyListSelected(id);
		}
		break;
	}
}

void UMMOUIMasterStudentPanel::UpdateMasterApplyListSelected(INT id)
{
	INT nIndex = 0;
	const ApllicantsList& studentList = MasterStudentManager::getSingleton().GetStudentApllyList();
	ApllicantsList::const_iterator itr = studentList.begin();
	for (; itr != studentList.end(); ++itr)
	{
		if ((*itr).nRoleID == id)
		{
			eventCallUCUpdateMasterApplyListDataOnSelected(nIndex,id);
			break;
		}
		nIndex++;

	}

}

void UMMOUIMasterStudentPanel::UpdateStudentApplyListSelected(INT id)
{
	INT nIndex = 0;
	const ApllicantsList& studentList = MasterStudentManager::getSingleton().GetStudentApllyList();
	ApllicantsList::const_iterator itr = studentList.begin();
	for (; itr != studentList.end(); ++itr)
	{
		if ((*itr).nRoleID == id)
		{
			eventCallUCUpdateStudentApplyListDataOnSelected(nIndex,id);
			break;
		}
		nIndex++;
	}
}

UBOOL UMMOUIMasterStudentPanel::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	//return TRUE;
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

void UMMOUIMasterStudentPanel::ASMasterInfoClick(UBOOL selected)
{
	UINT8 twitter = selected ? 1 : 0;
	CMessageHandler::GetInstance().Send_Twitter(twitter);
}

void UMMOUIMasterStudentPanel::ASClickMasterExp(INT id,UBOOL bSelfReward)
{
	if (bSelfReward)
	{
		AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if( mainPlayer == NULL ){
			return;
		}

		CMessageHandler::GetInstance().Send_ReciveMasterExp(mainPlayer->ServerID);
	}
	else
	{
		CMessageHandler::GetInstance().Send_ReciveMasterExp(id);
	}
	
}