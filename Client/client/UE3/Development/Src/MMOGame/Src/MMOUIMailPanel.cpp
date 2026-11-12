#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOStats.h"
#include "MailManager.h"
#include "mail_def.h"
#include "UMMOUIMailPanel.h"
#include "ClientActionConditionCheck.h"
#include "MMOChatManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOUIMailPanel)

const static INT gs_perMailCostBindMoney = 1000;
#define CHECK_MAIL_TIME 30
 

//---------AS----------
void UMMOUIMailPanel::ASReadMail(INT mailID)
{
	MailManager::getSingleton().SetMailWndOpen(TRUE);
	// first check whether it is cached locally, if so no need to send the protocol
	if(!MailManager::getSingleton().TryReadCachedMail(mailID))
	{
		CMessageHandler::GetInstance().Send_ReadMail(mailID);
	}
}

void UMMOUIMailPanel::OnDeleteMail(UGFxObject *mc)
{
	if(mc == NULL)
		return;
	INT mailID = mc->GetInt(TEXT("iteminfoid"));
	CMessageHandler::GetInstance().Send_DeleteMail(mailID);
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Mail_MailRead, FALSE, "");
}


void UMMOUIMailPanel::ASSendMail(const FString& toPlayer,const FString& title,const FString& content)
{
	FClientActionConditionContex context;
	context.MailReceiver = toPlayer;
	context.MailSubject = title;
	context.MailContent = content;
	context.SelfPlayerName = MMOFacade::getSingleton().GetMainPlayer()->ActorName;

	FString mailTitle = title;

	ClientActionCondition_SendMail checker;
	if(!checker.CheckClientActionCondition(context))
	{
		return;
	}
	/*if( !MMOFacade::getSingleton().CheckEnoughMoney( E_MONEYTYPE_01 , gs_perMailCostBindMoney ) )
	return;*/

	INT money =	MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_02);
	if( money < gs_perMailCostBindMoney )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.MoneyNotEnough") ) , MsgInfoType_Table , TEXT("MMOGame.Mail.MoneyNotEnough") );
		return;
	}
	if(bSendMail)
	{
		bSendMail = FALSE;
	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.CannotSend") ) , MsgInfoType_Table , TEXT("MMOGame.Mail.CannotSend") );
		return;
	}

	MMOUI::ReplaceCensoredWord(context.MailContent,CensoredWord_Chat);
	MMOUI::ReplaceCensoredWord(context.MailSubject,CensoredWord_Chat);
	MMOUI::ReplaceCensoredWord(mailTitle,CensoredWord_Chat);

	CMessageHandler::GetInstance().Send_WriteMail(toPlayer, mailTitle, content);
	MailManager::getSingleton().AddRecentPlayer(toPlayer);
	MailManager::getSingleton().NotifyUpdateRecentPlayers();
}

void UMMOUIMailPanel::ASReadAll()
{
	INT nType = MailManager::getSingleton().GetCurMailType();
	MailManager::getSingleton().SetMailWndOpen(FALSE);
	const MailMap& mails = MailManager::getSingleton().GetAllMails();
	MailMap::const_iterator i = mails.begin();
	for (; i != mails.end(); ++i)
	{
		if(nType >= MAILTYPE_USER && i->second->byMailType != nType)
			continue;
		if (!i->second->IsSelected)
			continue;

		if(i->second->byAttachment)
		{
			CMessageHandler::GetInstance().Send_GetMailAttach(i->first);
		}
		else if (i->second->byNew)
		{
			ASReadMail(i->second->dwMailID);
		}
	}
}

void UMMOUIMailPanel::ASSelectAll()
{
	OnSelectAll();
}

void UMMOUIMailPanel::ASDeleteSelected()
{
	MailManager::getSingleton().DeleteSelected();
}

void UMMOUIMailPanel::ASSelectMail(INT mailID,UBOOL select)
{
	MailManager::getSingleton().MarkMailSelected(mailID, select == TRUE);
	MailManager::getSingleton().CheckAllSelectMail();
	
}

void UMMOUIMailPanel::ASMessageMailContentTooLong()
{
	FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_MailSend_CheckContentMaxLen"));
	MMOChatManager::getSingleton().AddSystemMessage(strTips); 
	MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_MailSend_CheckContentMaxLen")); 
}

void UMMOUIMailPanel::UpdateMailList()
{
	INT nType = MailManager::getSingleton().GetCurMailType();

	const MailMap& mails = MailManager::getSingleton().GetAllMails();
	MailMap::const_reverse_iterator i = mails.rbegin();
	//MailMap::const_iterator i = mails.begin();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	for (INT idx = 0; /*i != mails.end()*/i != mails.rend(); ++i)
	{
		const ClientBriefMailInfo* mail = i->second;
		if(mail == NULL)
			continue;

		if(nType >= MAILTYPE_USER && nType != mail->byMailType)
			continue;

		UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Mail.MailData"));
		if(itemObj == NULL)
			continue;

		itemObj->SetInt(TEXT("mail_id"), mail->dwMailID);

		INT mailIcon = mail->byNew ? EMailStatusIcon_New : EMailStatusIcon_Read;
		itemObj->SetInt(TEXT("mail_icon"), mailIcon);
		itemObj->SetBool(TEXT("mail_has_items_mc"), mail->byAttachment ? TRUE : FALSE);
		itemObj->SetBool(TEXT("mail_selected"), mail->IsSelected);

		const INT MAX_MAIL_DAYS = 30;
		INT days = appCeil((FLOAT)difftime((time_t)mail->dwMailTime, ::time(NULL)) / (60.0f * 60.0f * 24.0f)) + MAX_MAIL_DAYS;

		if (days <=0 )
		{
			days = 0;
		}

		itemObj->SetString(TEXT("mail_valid_days_txt"), FString::Printf(TEXT("%d%s"),  days,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.LeaveDays"))));

		FString RoleName = TEXT("");
		if (mail->byMailType == MAILTYPE_SYSTEM)
			//RoleName = TEXT("<IMG  SRC='img://MMO_UIIcon.ChatChannelIcon.ChatChannelSystem'/>");
			RoleName = MailManager::getSingleton().GetMailSystemIcon();
		else
			RoleName = UTF8_TO_TCHAR(mail->szSenderRoleName);

		if (RoleName == "Unknown Sender")
		{
			RoleName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowRole"));
		}
		itemObj->SetString(TEXT("mail_from_txt"), RoleName);
		itemObj->SetString(TEXT("mail_title_txt"), /*UTF8_TO_TCHAR(*/mail->mailSubject/*)*/);

		dataProvider->SetElementObject(idx, itemObj);

		++idx;
	}

	eventCallUpdateItemsData(dataProvider);
}

void UMMOUIMailPanel::ASSelectMailType(INT nType,INT page)
{
	MailManager::getSingleton().SetMailType(nType);

	//MMOGameEvent_Mail evt0(MailType::UpdateMailList, 0);
	//MMOUI::PostGameEvent(&evt0);
	MailManager::getSingleton().ResetAllMailSelected();
	MailManager::getSingleton().SetCurrentPage(page);
}

void UMMOUIMailPanel::NativeInitOnce()
{
	check(EventAdapter == NULL);
	if(NULL == EventAdapter)
	{
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_Mail);
		evts.AddItem(GameEvent_Mail_UpdateSelectAll);
		EventAdapter = new TGameEventAdapter<UMMOUIMailPanel>(this, evts);
	}

	UGFxObject* obj = NULL;

	obj = GetObject( TEXT("tab0_mc") );
	if( obj )
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.MailList") ) );
	obj = GetObject( TEXT("tab1_mc") );
	if( obj )
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.SendMail") ) );

	UGFxObject* listPageMc = GetObject( TEXT("mail_list_page_mc") );
	if( listPageMc )
	{
		obj = listPageMc->GetObject( TEXT("btn_read_all") );
		if(obj)
			obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_read_all") ));

		obj = listPageMc->GetObject( TEXT("btn_select_all") );
		if(obj)
			obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_select_all") ));

		obj = listPageMc->GetObject( TEXT("btn_delete") );
		if(obj)
			obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_delete") ));
	}

	UGFxObject* sendPageMc = GetObject( TEXT("send_mail_page_mc") );
	if(sendPageMc)
	{
		obj = sendPageMc->GetObject( TEXT("btn_send") );
		if(obj)
			obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_send") ) );
	}

	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}

	_currentTime = 0;
	_delayTime = 0;
	bSendMail = TRUE;
}

void UMMOUIMailPanel::BeforeViewUpdate(float DeltaTime)
{
	if(MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}

	if( IsVisible && MailManager::getSingleton().GetCheckMail())
	{
		if (MailManager::getSingleton().GetMailsCount() < MAX_MAIL)
		{
			if (_currentTime >=CHECK_MAIL_TIME)
			{
				CMessageHandler::GetInstance().Send_RecvMailList(); 
				_currentTime = 0;
			}
			_currentTime += DeltaTime;
		}
	}
	if( bSendMail == FALSE)
	{
		_delayTime += DeltaTime;
		if (_delayTime >= MailManager::getSingleton().GetSendMailDelayTime())
		{
			_delayTime = 0;
			bSendMail = TRUE;
		}
	}
}

void UMMOUIMailPanel::OnSelectAll()
{
	UGFxObject* pMailListPage= GetObject(TEXT("mail_list_page_mc"));
	if(pMailListPage == NULL)
		return;
	UGFxObject* pSelectAll = pMailListPage->GetObject(TEXT("btn_select_all"));
	if(pSelectAll == NULL)
		return;
	UBOOL bSelected = pSelectAll->GetBool(TEXT("selected"));
	MailManager::getSingleton().MarkAllMailSelected(bSelected);
}

void UMMOUIMailPanel::ASSelectCurrentPageMail(UBOOL selected,INT currentPage)
{
	MailManager::getSingleton().MarkCurrentPageMailSelected(selected,currentPage);
}


void UMMOUIMailPanel::UpdateRecentPlayers()
{
	const TArray<FString>& recentPlayersArray = MailManager::getSingleton().GetRecentPlayersArray();
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == NULL)
		return;

	for (INT index = 0; index < recentPlayersArray.Num(); index++)
	{
		if(!recentPlayersArray(index).IsEmpty())
			dataProvider->SetElementString(index, recentPlayersArray(index));
	}
	eventCallAddRecentPlayers(dataProvider);
}

void UMMOUIMailPanel::OnMailWriteSuccess()
{
	FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.MailSendSuccess"));
	MMOChatManager::getSingleton().AddSystemMessage(strTips); 
	MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.Mail.MailSendSuccess")); 
	eventCallClearSendContent();
}

void UMMOUIMailPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	MMOGameEvent_Mail* pMailEvt = (MMOGameEvent_Mail*)evt;
	if(pMailEvt == NULL)
		return;
	switch(pMailEvt->eMailType)
	{
	case MailType::NewMail:
		{
			//if(IsVisible)
			//{
				//CMessageHandler::GetInstance().Send_RecvMailList(); 
			//}
			break;
		}
	case MailType::NewMail_UpdatePanel:
		{
			if(IsVisible)
			{
				CMessageHandler::GetInstance().Send_RecvMailList(); 
			}
			else
			{
				for (INT n = 0; n < pMailEvt->_MailCount; ++n)
				{
					MMOMsgButtonManager_Parms Parms(EC_EventParm);
					MMOMsgButtonManager::getSingleton().AddNewButton(MailManager::getSingleton().GetOpenMailPanelDelegate(), Parms, E_MsgBtnShow_Mail );
				}
			}
		}
		break;
	case MailType::UpdateMailList:
		{
			UpdateMailList();
			break;
		}
	case MailType::UpdateRecentPlayers:
		{
			UpdateRecentPlayers();
			break;
		}
	case MailType::WriteSuccess:
		{
			OnMailWriteSuccess();
			break;
		}
	case MailType::SendMailTo:
		{
			if (!IsVisible)
				Show(TRUE);
			TurnSendMail(pMailEvt->Recevier);
			break;
		}
	case MailType::updateSelectAll:
		{
			eventCallUpdateSelectAllState(pMailEvt->selectValue);
		}
		break;

	}
}

void UMMOUIMailPanel::TurnSendMail(const FString& playerName)
{

	FString contentName = playerName;

	int Pos = playerName.InStr( TEXT("("));
	if( Pos > 0)
	{
		contentName = playerName.Left(Pos);
	}
	eventCallSetPlayerName(contentName);
	//Switch to the send page
	eventCallShowTabPage(1);
}

void UMMOUIMailPanel::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);
	if(show)
	{
		eventCallShowTabPage(0);
		CMessageHandler::GetInstance().Send_RecvMailList(); // TODO: optimize
		UpdateRecentPlayers();

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowMailPanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else
	{
		CloseMail();
		UMMOUIMailReadPanel* maiRead = (UMMOUIMailReadPanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Mail_MailRead);
		if(maiRead&&maiRead->IsVisible)
			maiRead->Show(FALSE);
	}
}

void UMMOUIMailPanel::ASMailPageChange(INT page)
{
	MailManager::getSingleton().ResetAllMailSelected();
	MailManager::getSingleton().SetCurrentPage(page);
}

void UMMOUIMailPanel::ASSetCurrentPageNum(INT page, int num)
{
	MailManager::getSingleton().SetCurrentPageNum(page,num);
};

void UMMOUIMailPanel::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUIMailPanel::CloseMail()
{
	eventCallUpdateSelectAllState(FALSE);
	MailManager::getSingleton().MarkCurrentPageMailSelected(FALSE,MailManager::getSingleton().GetCurrentPage());
}