
#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MasterStudentManager.h"
#include "MMOChatManager.h"

IMPLEMENT_CLASS(UMMOUIMasterCongratulationMain)

#define STUDENTMAXLEVEL 30

//==================================================Total()====================================================== 
void UMMOUIMasterCongratulationMain::NativeInitOnce()
{
	UMMOUIBase::NativeInitOnce();

	
}

void UMMOUIMasterCongratulationMain::OnNativeGameEvent(struct MMOGameEvent* evt)
{

}

void UMMOUIMasterCongratulationMain::UnInit()
{
	UMMOUIBase::UnInit();
	
}

void UMMOUIMasterCongratulationMain::PostOnShow(UBOOL v)
{
	UMMOUIBase::PostOnShow(v);
	if (v)
	{
		UMMOUIMasterStudentPanel* pkMaster = (UMMOUIMasterStudentPanel*)MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_MasterStudent);
		if(pkMaster&&pkMaster->IsVisible)
		{
			FLOAT xPos,yPos;
			pkMaster->GetPosition(xPos,yPos);
			xPos += pkMaster->eventGetWidth();
			yPos += (pkMaster->eventGetHeight() - eventGetHeight())*0.5f;
			SetPosition(xPos,yPos);
		}
	}
	else
	{
		currentRoleId = 0;
	}
}
void UMMOUIMasterCongratulationMain::PostAdvance(float DeltaTime)
{
}
void UMMOUIMasterCongratulationMain::SetCongratulationData(INT nRoleID)
{
	FString strRelationHint;
	FString strCongratulation;
	const StudentListData* pkStudent = MasterStudentManager::getSingleton().GetStudentDataByID(nRoleID);
	const MasterDetailInfo&	master   = MasterStudentManager::getSingleton().GetMasterDetailInfo();

	INT nLevel = 0;
	INT  nMailID1 = 0;
	INT  nMailID2 = 0;
	INT  nMailID3 = 0;

	currentRoleId = nRoleID;

	if(MasterStudentManager::getSingleton().GetMasterStudentState() == E_MASTER_TEACHER)
	{
		//apprentice leveled up
		if(pkStudent)
		{
			strRelationHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationLvUpToStudent")),*(pkStudent->RoleName));
			strCongratulation = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationLvUpStudent"));

			nLevel = MasterStudentManager::getSingleton().GetCongratulationLevel(pkStudent->nCZLevel,pkStudent->nLevel);//GetStudentCongratulationLevel(pkStudent->nLevel,pkStudent->nCZLevel);
			
			const CongratulationGiftData* pkGift = MasterStudentManager::getSingleton().GetGiftConfigData(nLevel);
			if(pkGift == NULL)
				return;
			nMailID1 = pkGift->nType0MailID1;
			nMailID2 = pkGift->nType0MailID2;
			nMailID3 = pkGift->nType0MailID3;

		}
	}
	else if(nRoleID == master.nRoleID)
	{
		//master leveled up
		strRelationHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationLvUpToMaster")),*(master.masterName));
		strCongratulation = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationLvUpMaster"));
		nLevel = master.nLevel - master.nCZLevel;
		const CongratulationGiftData* pkGift = MasterStudentManager::getSingleton().GetGiftConfigData(nLevel);
		if(pkGift == NULL)
			return;
		nMailID1 = pkGift->nType1MailID1;
		nMailID2 = pkGift->nType1MailID2;
		nMailID3 = pkGift->nType1MailID3;
	}
	else
	{
		//fellow disciple leveled up
		if(pkStudent)
		{
			strRelationHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationLvUpToTongMen")),*(pkStudent->RoleName));
			strCongratulation = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationLvUpTongMen"));

			//nLevel = GetStudentCongratulationLevel(pkStudent->nLevel,pkStudent->nCZLevel);
			nLevel = MasterStudentManager::getSingleton().GetCongratulationLevel(pkStudent->nCZLevel,pkStudent->nLevel);

			const CongratulationGiftData* pkGift = MasterStudentManager::getSingleton().GetGiftConfigData(nLevel);
			if(pkGift == NULL)
				return;
			nMailID1 = pkGift->nType2MailID1;
			nMailID2 = pkGift->nType2MailID2;
			nMailID3 = pkGift->nType2MailID3;
		}
	}
	eventCallUCSetTargetName(strRelationHint,strCongratulation);
	
	const CongratulationGiftData* pkGift = MasterStudentManager::getSingleton().GetGiftConfigData(nLevel);
	if(pkGift == NULL)
		return;
	INT nIndex = 0;
	FString strFree =  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationCostFree"));
	FString strConsum =  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationCostGold"));
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(NULL != dataProvider)
	{
		for (INT n = 0; n < 3; ++n)
		{
			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Master.MasterCongratulationData"));
			if(itemObj != NULL )
			{
				FString strTile;
				FString strCost;
				INT nMailID = 0;
				if(n == 0)
				{
					nMailID = nMailID1;
					strTile = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationTitle0"));
					strCost = (pkGift->nType0Consume == 0)?strFree:FString::Printf(*strConsum,pkGift->nType0Consume);
				}
				else if (n == 1)
				{
					nMailID = nMailID2;
					if(nRoleID == MasterStudentManager::getSingleton().GetMasterDetailInfo().nRoleID)
						strTile = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationTitle1"));
					else
						strTile = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationTitle2"));
					strCost = (pkGift->nType1Consume == 0)?strFree:FString::Printf(*strConsum,pkGift->nType1Consume);
				}
				else if (n == 2)
				{
					nMailID = nMailID3;
					if(nRoleID == MasterStudentManager::getSingleton().GetMasterDetailInfo().nRoleID)
						strTile = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationTitle4"));
					else
						strTile = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.CongraluationTitle3"));
					strCost = (pkGift->nType2Consume == 0)?strFree:FString::Printf(*strConsum,pkGift->nType2Consume);
				}
				itemObj->SetInt(TEXT("Id"),n);
				itemObj->SetInt(TEXT("nType"),n+1);
				itemObj->SetString(TEXT("Title"),strTile);
				itemObj->SetString(TEXT("Name"), strCost);
				itemObj->SetInt(TEXT("nMailID"),nMailID);
				dataProvider->SetElementObject(nIndex++, itemObj);
			}
		}
		eventCallSetWindowContent(dataProvider,nRoleID);
	}
}

void UMMOUIMasterCongratulationMain::ASSendCongratulation(INT nType,INT nRoleID,const FString& strCon)
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
				nLevel = MasterStudentManager::getSingleton().GetCongratulationLevel(pkStudent->nCZLevel , pkStudent->nLevel);
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
				nLevel = MasterStudentManager::getSingleton().GetCongratulationLevel(pkStudent->nCZLevel , pkStudent->nLevel);
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

INT UMMOUIMasterCongratulationMain::GetStudentCongratulationLevel(INT sLv, INT count)
{
	INT lv = 0;

	if (sLv >= STUDENTMAXLEVEL)
		lv = STUDENTMAXLEVEL - count;
	else
		lv = sLv - count;

	return lv;
}