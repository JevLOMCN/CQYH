#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MasterStudentManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOChatManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_MasterStudent)

void UMMOGFXPlayer_MasterStudent::PostAdvance(float DeltaTime)
{
	UGFxMoviePlayer::PostAdvance(DeltaTime);
	if( _MasterStudent->IsVisible )
	{
		_MasterStudent->PostAdvance( DeltaTime );
	}
}


void UMMOGFXPlayer_MasterStudent::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_MasterStudent::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_MasterStudent );
}
void UMMOGFXPlayer_MasterStudent::ASSendCongratulation(INT nType,INT nRoleID,const FString& strCon)
{
	//congratulations
	const AMMOPlayer* pkPlayer       =   MMOFacade::getSingleton().GetMainPlayer();
	INT nLevel = 0;

	if(pkPlayer)
	{
		const StudentListData* pkStudent = MasterStudentManager::getSingleton().GetStudentDataByID(nRoleID);
		const MasterDetailInfo&	master   = MasterStudentManager::getSingleton().GetMasterDetailInfo();
		
		if(MasterStudentManager::getSingleton().GetMasterStudentState() == E_MASTER_TEACHER)
		{
			//apprentice leveled up
			if(pkStudent)
				nLevel = pkStudent->nLevel - pkStudent->nCZLevel;
		}
		else if(nRoleID == master.nRoleID)
		{
			//master leveled up
			nLevel = master.nLevel - master.nCZLevel;
		}
		else
		{
			//fellow disciple leveled up
			if(pkStudent)
				nLevel = pkStudent->nLevel - pkStudent->nCZLevel;
		}

		const CongratulationGiftData* pkGift = MasterStudentManager::getSingleton().GetGiftConfigData(nLevel);
		if(pkGift != NULL)
		{
			INT nMoney = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_02);
			INT nNeedMoney = 0;
			if(nType == 1)
			{
				nNeedMoney = pkGift->nType0Consume;
			}
			else if (nType == 2)
			{
				nNeedMoney = pkGift->nType1Consume;
				
			}
			else if (nType == 3)
			{
				nNeedMoney = pkGift->nType2Consume;
			}
			if(nMoney < nNeedMoney)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOErrorInfo.ERROR_CODE_DEF.errItemNotEnoughMoneytype02")));
				MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOErrorInfo.ERROR_CODE_DEF.errItemNotEnoughMoneytype02")));
				return;
			}	
		}
		CMessageHandler::GetInstance().Send_Congratulation(pkPlayer->ServerID,nRoleID,nType,strCon,nLevel);
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,FALSE,"");
	}
}
void UMMOGFXPlayer_MasterStudent::ASClearMasterApplyList()
{
	//reject all
	const ApllicantsList& list = MasterStudentManager::getSingleton().GetMasterApllyList();
	for (ApllicantsList::const_iterator itr  = list.begin(); itr != list.end(); ++itr)
	{
		CMessageHandler::GetInstance().Send_RefuseMasterApply((*itr).nRoleID);
	}
}
void UMMOGFXPlayer_MasterStudent::ASAcceptMasterApplyList(INT nID)
{
	CMessageHandler::GetInstance().Send_AgreeMasterdApply(nID);
}
void UMMOGFXPlayer_MasterStudent::ASDenyMasterApplyList(INT nID)
{
	CMessageHandler::GetInstance().Send_RefuseMasterApply(nID);
}
void UMMOGFXPlayer_MasterStudent::ASMasterApplyListNextPage()
{
	INT nPage = MasterStudentManager::getSingleton().GetApplyCurPage();
	if(++nPage <= MasterStudentManager::getSingleton().GetApplyMaxPage())
		MasterStudentManager::getSingleton().SetApplyCurPage(nPage);
}
void UMMOGFXPlayer_MasterStudent::ASMasterApplyListPrevPage()
{
	INT nPage = MasterStudentManager::getSingleton().GetApplyCurPage();
	if(--nPage >= 0)
		MasterStudentManager::getSingleton().SetApplyCurPage(nPage);
}
void UMMOGFXPlayer_MasterStudent::ASSortMasterApplyList(INT nType)
{

}
void UMMOGFXPlayer_MasterStudent::ASSortMasterPosterList(INT nType)
{

}
void UMMOGFXPlayer_MasterStudent::ASSortStudentPosterList(INT nType)
{

}
void UMMOGFXPlayer_MasterStudent::ASSortStudentApplyList(INT nType)
{

}
void UMMOGFXPlayer_MasterStudent::ASMasterPosterListRefresh()
{
	CMessageHandler::GetInstance().Send_MasterListAck();
}
void UMMOGFXPlayer_MasterStudent::ASApplyMasterPosterList(INT nID)
{
	CMessageHandler::GetInstance().Send_SubmitApplyToMaster(nID);
}
void UMMOGFXPlayer_MasterStudent::ASCancelMasterPosterList(INT nID)
{
	CMessageHandler::GetInstance().Send_CancelStudentApplyBySelf(nID);
}
void UMMOGFXPlayer_MasterStudent::ASMasterPosterListNextPage()
{

}
void UMMOGFXPlayer_MasterStudent::ASMasterPosterListPrevPage()
{

}
void UMMOGFXPlayer_MasterStudent::ASClickMasterPosterList(INT nID)
{

}
void UMMOGFXPlayer_MasterStudent::ASLeaveMaster()
{
	HideMsgBox();

	FScriptDelegate LeaveMasterDelegate(EC_EventParm);
	LeaveMasterDelegate.FunctionName = TEXT("OnLeaveMaster");
	LeaveMasterDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnMasterCancel");
	Nofunction.Object = this;

	FString szText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MasterStudent.StudentLeaveMaster"));
	ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( szText , 0 , LeaveMasterDelegate , Nofunction );

	
}
void UMMOGFXPlayer_MasterStudent::OnLeaveMaster()
{
	CMessageHandler::GetInstance().Send_LeaveMaster();
	ConfirmMsgBox = NULL;
}

void UMMOGFXPlayer_MasterStudent::ASFireStudent(INT nID)
{
	HideMsgBox();

	FScriptDelegate FireStudentDelegate(EC_EventParm);
	FireStudentDelegate.FunctionName = TEXT("OnFireStudent");
	FireStudentDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnMasterCancel");
	Nofunction.Object = this;

	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(nID);

	if (playerInfo)
	{
		FString szText = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MasterStudent.IsFireStudent")) , *playerInfo->_RoleName);
		ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( szText , 0 , FireStudentDelegate , Nofunction );

		if(ConfirmMsgBox)
			ConfirmMsgBox->SetInt( TEXT("dataid") , nID);
	}
}

void UMMOGFXPlayer_MasterStudent:: OnFireStudent(UGFxObject* mc)
{
	INT nID = mc->GetInt(TEXT("dataid"));
	CMessageHandler::GetInstance().Send_FireStudent(nID);
	ConfirmMsgBox = NULL;
}

void UMMOGFXPlayer_MasterStudent:: OnMasterCancel()
{
	ConfirmMsgBox = NULL;
}

void UMMOGFXPlayer_MasterStudent::ASClickMasterInfoList(INT nID)
{

}
void UMMOGFXPlayer_MasterStudent::ASRightMenuMasterApplyList(INT nID)
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
void UMMOGFXPlayer_MasterStudent::ASRightMenuStudentPosterList(INT nID)
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
void UMMOGFXPlayer_MasterStudent::ASApplyStudentPosterList(INT nID)
{
	CMessageHandler::GetInstance().Send_SubmitApplyToStudent(nID);
	FString hintText = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.ApplyStudent"));
	MMOUI::GetUIManager()->delegateAddScreenMessage( hintText , MsgInfoType_Table , hintText );
	MMOChatManager::getSingleton().AddSystemMessage( hintText );
}
void UMMOGFXPlayer_MasterStudent::ASCancelStudentPosterList(INT nID)
{
	CMessageHandler::GetInstance().Send_CancelMasterApplyBySelf(nID);
}
void UMMOGFXPlayer_MasterStudent::ASRefreshStudentPosterList()
{
	CMessageHandler::GetInstance().Send_SoloStudentListAck();
}
void UMMOGFXPlayer_MasterStudent::ASClickStudentPosterList(INT nID)
{

}
void UMMOGFXPlayer_MasterStudent::ASStudentPostListPage(INT nPage)
{

}
void UMMOGFXPlayer_MasterStudent::ASAcceptStudentApplyList(INT nID)
{
	CMessageHandler::GetInstance().Send_AgreeStudentdApply(nID);
}
void UMMOGFXPlayer_MasterStudent::ASDenyStudentApplyList(INT nID)
{
	CMessageHandler::GetInstance().Send_RefuseStudentApply(nID);
}
void UMMOGFXPlayer_MasterStudent::ASClearStudentApplyList()
{
	const ApllicantsList& list = MasterStudentManager::getSingleton().GetStudentApllyList();
	for (ApllicantsList::const_iterator itr  = list.begin(); itr != list.end(); ++itr)
	{
		CMessageHandler::GetInstance().Send_RefuseStudentApply((*itr).nRoleID);
	}
}
void UMMOGFXPlayer_MasterStudent::ASClickStudentApplyList(INT nID)
{

}
void UMMOGFXPlayer_MasterStudent::ASRightMenuStudentApplyList(INT nID)
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
void UMMOGFXPlayer_MasterStudent::ASClickStartRecruit()
{
	CMessageHandler::GetInstance().Send_ApplyRecruitStudent();
}
void UMMOGFXPlayer_MasterStudent::ASClickStopRecruit()
{
	CMessageHandler::GetInstance().Send_CancelRecruitStudent();
}
void UMMOGFXPlayer_MasterStudent::ASClickAcceptLeave(INT nID)
{
	CMessageHandler::GetInstance().Send_AgreeGrad(nID);
}
void UMMOGFXPlayer_MasterStudent::ASClickCongratulation(INT nID)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,TRUE, "");
	if(_MasterCongMain)
		_MasterCongMain->SetCongratulationData(nID);
}
void UMMOGFXPlayer_MasterStudent::ASClickMasterTap(INT nID)
{
	EMASTER_STATE state = MasterStudentManager::getSingleton().GetMasterStudentState();
	if( _MasterStudent == NULL ){
		return;
	}

	MasterPanelType panelType = (MasterPanelType)nID;
	_MasterStudent->SetCurPanelType( panelType );

	switch( panelType )
	{
	case EMSTPT_Outline:
		{
			CMessageHandler::GetInstance().Send_GetMasterInfo();
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
	_MasterStudent->UpdatePanelInfo();
}
void UMMOGFXPlayer_MasterStudent::ASApplyGrad()
{
	CMessageHandler::GetInstance().Send_DesireGrad();
}
void UMMOGFXPlayer_MasterStudent::ASForceGrad()
{
	if( _MasterStudent == NULL ){
		return;
	}
	CMessageHandler::GetInstance().Send_ForceGrad();
	_MasterStudent->PopUpMessageBoxToConfirmForceGraduate();
}
void UMMOGFXPlayer_MasterStudent::ASRightMenuStudentDetailList(INT nID)
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
void UMMOGFXPlayer_MasterStudent::ASClickCongraluate(INT nID)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterCongMain,TRUE, "");
	if(_MasterCongMain)
		_MasterCongMain->SetCongratulationData(nID);
}
void UMMOGFXPlayer_MasterStudent::ASRightMenuMasterList(INT nID)
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
void UMMOGFXPlayer_MasterStudent::ASShowTips_MasterGift(class UGFxObject* mc,INT nType)
{
	if(mc == NULL)
		return;

	const MailTemplateData* pkMail = MasterStudentManager::getSingleton().GetMailTemplateData(nType);
	if(pkMail)
	{
		const GiftDetailData* pkGift = MasterStudentManager::getSingleton().GetGiftDeatilData(pkMail->nItemID);
		if(pkGift)
		{
			FString strTips = pkGift->strGiftName;
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
			}
			mc->SetString( TEXT("tipsHtml"), strTips );
			MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
		}
	}
}
void UMMOGFXPlayer_MasterStudent::ASHideTips_MasterGift()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer_MasterStudent::HideMsgBox()
{
	if( ConfirmMsgBox != NULL )
	{
		MMOMsgBoxManager::getSingleton().GetMessageAck( FALSE , ConfirmMsgBox);
		//ConfirmMsgBox->SetVisible( FALSE );
		ConfirmMsgBox = NULL;
	}
}