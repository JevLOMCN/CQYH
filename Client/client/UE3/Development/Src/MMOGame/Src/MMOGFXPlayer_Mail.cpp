#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MailManager.h"
#include "mail_def.h"
#include "ClientActionConditionCheck.h"
#include "CommonGameEvents.h"
#include "UITextParser.h"
#include "MMOQuestManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOChatManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Mail)
//IMPLEMENT_CLASS(UMMOUIMailPanel)
//IMPLEMENT_CLASS(UMMOUIMailReadPanel)

//const static INT gs_perMailCostBindMoney = 99;


void UMMOGFXPlayer_Mail::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Mail::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Mail );
}

void UMMOGFXPlayer_Mail::UnInit()
{
	Super::UnInit();

}

void UMMOGFXPlayer_Mail::ASReadMail(INT mailID)
{
	MailManager::getSingleton().SetMailWndOpen(TRUE);
	// first check whether it is cached locally, if so no need to send the protocol
	if(!MailManager::getSingleton().TryReadCachedMail(mailID))
	{
		CMessageHandler::GetInstance().Send_ReadMail(mailID);
	}
}

void UMMOGFXPlayer_Mail::ASReplyMail(INT mailID)
{
	const ClientBriefMailInfo* mail = MailManager::getSingleton().FindMail(mailID);
	if(mail)
	{
		FString sender = UTF8_TO_TCHAR(mail->szSenderRoleName);
		FString subjReply = /*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.ReplySubj"))
			+ *//*UTF8_TO_TCHAR(*/mail->mailSubject/*)*/;
		_MailListPanel->eventCallReplyMail(sender, subjReply);
	}
	else
	{
		warnf(NAME_Error, TEXT("[UMMOGFXPlayer_Mail::ASReplyMail] failed to find mail:%d"), mailID);
	}
}

void UMMOGFXPlayer_Mail::ASDeleteMail(INT mailID)
{
	const MailMap& AllMailMap = MailManager::getSingleton().GetAllMails();
	MailMap::const_iterator it = AllMailMap.find(mailID);
	if(it == AllMailMap.end())
		return;
	if(it->second->byAttachment != 0)
	{
		FScriptDelegate DelMailDelegate(EC_EventParm);
		DelMailDelegate.FunctionName = TEXT("OnDeleteMail");
		DelMailDelegate.Object = this;

		FScriptDelegate Nofunction(EC_EventParm);
		Nofunction.FunctionName = TEXT("");
		Nofunction.Object = this;

		FString szText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.BoxDelText"));
		MMOMsgBoxManager::getSingleton().AddMessageBox( szText , mailID , DelMailDelegate , Nofunction );
	}
	else
	{
		CMessageHandler::GetInstance().Send_DeleteMail(mailID);
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Mail_MailRead, FALSE, "");
	}
}

void UMMOGFXPlayer_Mail::OnDeleteMail(UGFxObject *mc)
{
	if(mc == NULL)
		return;
	INT mailID = mc->GetInt(TEXT("iteminfoid"));
	CMessageHandler::GetInstance().Send_DeleteMail(mailID);
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Mail_MailRead, FALSE, "");
}

void UMMOGFXPlayer_Mail::ASAcquireMailItems(INT mailID)
{
	const ClientBriefMailInfo* mail = MailManager::getSingleton().FindMail(mailID);
	if(mail && mail->byAttachment)
	{
		CMessageHandler::GetInstance().Send_GetMailAttach(mailID);
	}
}

void UMMOGFXPlayer_Mail::ASSendMail(const FString& toPlayer,const FString& title,const FString& content)
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
	//if( !MMOFacade::getSingleton().CheckEnoughMoney( E_MONEYTYPE_01 , gs_perMailCostBindMoney ) )
		//return;

	MMOUI::ReplaceCensoredWord(context.MailContent);
	MMOUI::ReplaceCensoredWord(context.MailSubject);
	MMOUI::ReplaceCensoredWord(mailTitle);

	CMessageHandler::GetInstance().Send_WriteMail(toPlayer, mailTitle, content);
	MailManager::getSingleton().AddRecentPlayer(toPlayer);
	MailManager::getSingleton().NotifyUpdateRecentPlayers();
}

void UMMOGFXPlayer_Mail::ASReadAll()
{
	INT nType = MailManager::getSingleton().GetCurMailType();
	MailManager::getSingleton().SetMailWndOpen(FALSE);
	const MailMap& mails = MailManager::getSingleton().GetAllMails();
	MailMap::const_iterator i = mails.begin();
	for (; i != mails.end(); ++i)
	{
		if(nType >= MAILTYPE_USER && i->second->byMailType != nType)
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

void UMMOGFXPlayer_Mail::ASSelectAll()
{
	if(_MailListPanel == NULL)
		return;
	_MailListPanel->OnSelectAll();
}

void UMMOGFXPlayer_Mail::ASDeleteSelected()
{
	MailManager::getSingleton().DeleteSelected();
}

void UMMOGFXPlayer_Mail::ASSelectMail(INT mailID,UBOOL select)
{
	MailManager::getSingleton().MarkMailSelected(mailID, select == TRUE);
}

void UMMOGFXPlayer_Mail::ASMessageMailContentTooLong()
{
	FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_MailSend_CheckContentMaxLen"));
	MMOChatManager::getSingleton().AddSystemMessage(strTips); 
	MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_MailSend_CheckContentMaxLen")); 
}
void UMMOGFXPlayer_Mail::ASSelectMailType(INT nType)
{
	MailManager::getSingleton().SetMailType(nType);

	MMOGameEvent_Mail evt0(UpdateMailList, 0);
	MMOUI::PostGameEvent(&evt0);
}
////------------------------------------------------------------------------
//// UMMOUIMailReadPanel
////------------------------------------------------------------------------
// void UMMOUIMailReadPanel::NativeInitOnce()
// {
//	 MailManager::getSingleton().SetOpenReadMailDelegate(FScriptDelegate(this, TEXT("OpenReadMail")));
//
//
//	 check(EventAdapter == NULL);
//	 if(NULL == EventAdapter)
//	 {
//		 TArray<EGameEvent> evts;
//		 evts.AddItem(GameEvent_Mail);
//		 EventAdapter = new TGameEventAdapter<UMMOUIMailReadPanel>(this, evts);
//	 }
//
//	 UGFxObject* obj = NULL;
//	 obj = GetObject( TEXT("btn_reply") );
//	 if(obj)
//		 obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_reply") ));
//
//	 obj = GetObject( TEXT("btn_delete") );
//	 if(obj)
//		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_delete_mail") ));
//
//	 obj = GetObject( TEXT("btn_acquire") );
//	 if(obj)
//		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_acquire") ));
//
//	 obj = GetObject(TEXT("mail_item_0"));
//	 if(obj)
//		 obj->SetInt( TEXT("SlotType") , ESPType_ReadMail );
// }
//
// void UMMOUIMailReadPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
// {
//	 MMOGameEvent_Mail* pMailEvt = (MMOGameEvent_Mail*)evt;
//	 if(pMailEvt == NULL)
//		 return;
//	 switch(pMailEvt->eMailType)
//	 {
//	 case MailType::DeleteMail:
//		 {
//			 OnMailDelete(pMailEvt->_MailCount);
//			 break;
//		 }
//	 case MailType::GetAttachSuccess:
//		 {
//			 UpdateAcquireAttachment();
//			 break;
//		 }
//	 case MailType::DeleteMailBox:
//		 {
//			 DeleteSelected();
//			 break;
//		 }
//	 }
// }
//
// void UMMOUIMailReadPanel::DeleteSelected()
// {
//	 FScriptDelegate DelAllMailDelegate(EC_EventParm);
//	 DelAllMailDelegate.FunctionName = TEXT("OnDeleteSelected");
//	 DelAllMailDelegate.Object = this;
//
//	 FScriptDelegate Nofunction(EC_EventParm);
//	 Nofunction.FunctionName = TEXT("");
//	 Nofunction.Object = this;
//
//	 FString szText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.BoxDelAllText"));
//	 MMOMsgBoxManager::getSingleton().AddMessageBox( szText , 0 , DelAllMailDelegate , Nofunction );
// }
//
// void UMMOUIMailReadPanel::OnDeleteSelected()
// {
//	 const MailIDList& deleteMails = MailManager::getSingleton().GetDeleteMails();
//	 for (size_t j = 0; j < deleteMails.size(); ++j)
//	 {
//		 CMessageHandler::GetInstance().Send_DeleteMail(deleteMails[j]);
//	 }
// }
//
// //------------------------------------------------------------------------
// void UMMOUIMailReadPanel::OpenReadMail(INT mailID,INT mailTime,BYTE mailType, INT mailAttachmentItemID, int mailAttachmentItemCount,
//	 const FString& sender,const FString& subject,const FString& content)
// {
//	 _CurrentMailID = mailID; 
//	 //bound gold ID in the item table = 0
//	 if(mailAttachmentItemID >= 0)
//	 {
//		 UGFxObject* itemSlot = GetObject( TEXT("mail_item_0") );
//		 UGFxObject* pAmount = GetObject(TEXT("tl_itemcount"));
//		 if(itemSlot&&pAmount)
//		 {
//			 pAmount->SetText("");
//			 UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(mailAttachmentItemID);
//			 if(itemTemp)	 
//			 {
//				 GFXPlayer->SetItemSlotData( itemSlot , itemTemp, ESPType_ReadMail, E_CONSLOT_MAX, 0);
//
//				 if(itemTemp->ItemDuration == E_DURTYPE_STACK&&mailAttachmentItemCount > 0)
//				 {
//					 FString strCount = FString::Printf(TEXT("X %d"),mailAttachmentItemCount);
//					 pAmount->SetText(strCount);
//				 } 
//			 }
//			 else
//			 {
//				  FString strHint = FString::Printf(TEXT("This mail contains invalid item id:%d"),mailAttachmentItemID);
//				  MMOUI::GetUIManager()->delegateAddScreenMessage(strHint); 
//			 }
//		 }
//	 }
//
//	eventCallUpdateMailInfo(mailID, mailType, mailAttachmentItemID, sender, subject, content);
//	this->Show(TRUE);
//	UMMOUIMailPanel* pkMail = (UMMOUIMailPanel*)MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Mail);
//	if(pkMail)
//	{
//			FLOAT xPos,yPos;
//		pkMail->GetPosition(xPos,yPos);
//		/*xPos += pkMail->eventGetWidth();
//		yPos += (pkMail->eventGetHeight() - eventGetHeight())*0.5f;
//		SetPosition(xPos,yPos);*/
//		eventCallSetReadPanelPostion(xPos,yPos);
//	}
// }
//
// void UMMOUIMailReadPanel::UpdateAcquireAttachment()
// {
//	 eventCallUpdateAcquireAttachment();
// }
//
// void UMMOUIMailReadPanel::OnMailDelete(INT mailID)
// {
//	 if(IsVisible && (mailID == _CurrentMailID))
//	 {
//		 Show(FALSE);
//	 }
//	 FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.MailDeleteTips"));
//	 MMOChatManager::getSingleton().AddSystemMessage(strTips); 
//	 MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.Mail.MailDeleteTips")); 
// }
//
// //------------------------------------------------------------------------
// // UMMOUIMailPanel
// //------------------------------------------------------------------------
// void UMMOUIMailPanel::NativeInitOnce()
// {
//	 check(EventAdapter == NULL);
//	 if(NULL == EventAdapter)
//	 {
//		 TArray<EGameEvent> evts;
//		 evts.AddItem(GameEvent_Mail);
//		 EventAdapter = new TGameEventAdapter<UMMOUIMailPanel>(this, evts);
//	 }
//
//	 UGFxObject* obj = NULL;
//
//	 obj = GetObject( TEXT("tab0_mc") );
//	 if( obj )
//		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.MailList") ) );
//	 obj = GetObject( TEXT("tab1_mc") );
//	 if( obj )
//		 obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.SendMail") ) );
//
//	 UGFxObject* listPageMc = GetObject( TEXT("mail_list_page_mc") );
//	 if( listPageMc )
//	 {
//		 obj = listPageMc->GetObject( TEXT("btn_read_all") );
//		 if(obj)
//			 obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_read_all") ));
//
//		 obj = listPageMc->GetObject( TEXT("btn_select_all") );
//		 if(obj)
//			 obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_select_all") ));
//
//		 obj = listPageMc->GetObject( TEXT("btn_delete") );
//		 if(obj)
//			 obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_delete") ));
//	 }
//	
//	 UGFxObject* sendPageMc = GetObject( TEXT("send_mail_page_mc") );
//	 if(sendPageMc)
//	 {
//		 obj = sendPageMc->GetObject( TEXT("btn_send") );
//		 if(obj)
//			 obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_send") ) );
//	 }
//	
// }
//
// void UMMOUIMailPanel::OnSelectAll()
// {
//	 UGFxObject* pMailListPage= GetObject(TEXT("mail_list_page_mc"));
//	 if(pMailListPage == NULL)
//		 return;
//	 UGFxObject* pSelectAll = pMailListPage->GetObject(TEXT("btn_select_all"));
//	 if(pSelectAll == NULL)
//		 return;
//	 UBOOL bSelected = pSelectAll->GetBool(TEXT("selected"));
//	 MailManager::getSingleton().MarkAllMailSelected(bSelected);
// }
//
// void UMMOUIMailPanel::UpdateMailList()
// {
//	 INT nType = MailManager::getSingleton().GetCurMailType();
//
//	 const MailMap& mails = MailManager::getSingleton().GetAllMails();
//	 MailMap::const_reverse_iterator i = mails.rbegin();
//
//	 UGFxObject* dataProvider = GFXPlayer->CreateArray();
//
// 	 for (INT idx = 0; i != mails.rend(); ++i)
//	 {
//		 const ClientBriefMailInfo* mail = i->second;
//		 if(mail == NULL)
//			 continue;
//
//		 if(nType >= MAILTYPE_USER && nType != mail->byMailType)
//			 continue;
//
//		 UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Mail.MailData"));
//		 if(itemObj == NULL)
//			 continue;
//
//		 itemObj->SetInt(TEXT("mail_id"), mail->dwMailID);
//
//		 INT mailIcon = mail->byNew ? EMailStatusIcon_New : EMailStatusIcon_Read;
//		 itemObj->SetInt(TEXT("mail_icon"), mailIcon);
//		 itemObj->SetBool(TEXT("mail_has_items_mc"), mail->byAttachment ? TRUE : FALSE);
//		 itemObj->SetBool(TEXT("mail_selected"), mail->IsSelected);
//
//		 const INT MAX_MAIL_DAYS = 30;
//		 INT days = appCeil((FLOAT)difftime((time_t)mail->dwMailTime, ::time(NULL)) / (60.0f * 60.0f * 24.0f)) + MAX_MAIL_DAYS;
//		 
//		 itemObj->SetString(TEXT("mail_valid_days_txt"), FString::Printf(TEXT("%d%s"),  days,
//			 *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.LeaveDays"))));
//
//		 FString RoleName = TEXT("");
//		 if (mail->byMailType == MAILTYPE_SYSTEM)
//			 RoleName = TEXT("<IMG  SRC='img://MMO_UIIcon.ChatChannelIcon.ChatChannelSystem'/>");
//		 else
//			 RoleName = UTF8_TO_TCHAR(mail->szSenderRoleName);
//		 itemObj->SetString(TEXT("mail_from_txt"), RoleName);
//		 itemObj->SetString(TEXT("mail_title_txt"), /*UTF8_TO_TCHAR(*/mail->mailSubject/*)*/);
//
//		 dataProvider->SetElementObject(idx, itemObj);
//
//		  ++idx;
//	 }
//
//	 eventCallUpdateItemsData(dataProvider);
// }
//
// void UMMOUIMailPanel::UpdateRecentPlayers()
// {
//	 const TArray<FString>& recentPlayersArray = MailManager::getSingleton().GetRecentPlayersArray();
//	 UGFxObject* dataProvider = GFXPlayer->CreateArray();
//	 if(dataProvider == NULL)
//		 return;
//
//	 for (INT index = 0; index < recentPlayersArray.Num(); index++)
//	 {
//		 if(!recentPlayersArray(index).IsEmpty())
//			dataProvider->SetElementString(index, recentPlayersArray(index));
//	 }
//	 eventCallAddRecentPlayers(dataProvider);
// }
//
// void UMMOUIMailPanel::OnMailWriteSuccess()
// {
//	 FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.MailSendSuccess"));
//	 MMOChatManager::getSingleton().AddSystemMessage(strTips); 
//	 MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.Mail.MailSendSuccess")); 
//	 eventCallClearSendContent();
// }
//
//void UMMOUIMailPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
//{
//	MMOGameEvent_Mail* pMailEvt = (MMOGameEvent_Mail*)evt;
//	if(pMailEvt == NULL)
//		return;
//	switch(pMailEvt->eMailType)
//	{
//	case MailType::NewMail:
//		{
//			if(IsVisible)
//			{
//				//CMessageHandler::GetInstance().Send_RecvMailList(); 
//			}
//			break;
//		}
//	case MailType::UpdateMailList:
//		{
//			UpdateMailList();
//			break;
//		}
//	case MailType::UpdateRecentPlayers:
//		{
//			UpdateRecentPlayers();
//			break;
//		}
//	case MailType::WriteSuccess:
//		{
//			OnMailWriteSuccess();
//			break;
//		}
//	case MailType::SendMailTo:
//		{
//			if (!IsVisible)
//				Show(TRUE);
//			TurnSendMail(pMailEvt->Recevier);
//			break;
//		}
//	}
//}
//
//void UMMOUIMailPanel::TurnSendMail(const FString& playerName)
//{
//	//switch to send page
//	eventCallShowTabPage(1);
//
//	FString contentName = playerName;
//
//	int Pos = playerName.InStr( TEXT("("));
//	if( Pos > 0)
//	{
//		contentName = playerName.Left(Pos);
//	}
//
//	if(SendPage)
//	{
//		UGFxObject* obj = SendPage->GetObject( TEXT("recentPlayers_combox_mc") );
//		if(obj == NULL)
//			return;
//		UGFxObject* receiver = obj->GetObject(TEXT("txt_combox_mc"));
//		if(receiver == NULL)
//			return;
//		if(obj)
//		{
//			obj->SetText(contentName);
//		}
//	}
//}
//
// void UMMOUIMailPanel::PostOnShow(UBOOL show)
// {
//	 Super::PostOnShow(show);
//	 if(show)
//	 {
//		 eventCallShowTabPage(0);
//		 CMessageHandler::GetInstance().Send_RecvMailList(); // TODO: optimize
//		 UpdateRecentPlayers();
//
//		 // new player tutorial
//		 MMOGameEvent_Guide evtGuide(EGUET_ShowMailPanel);
//		 MMOUI::PostGameEvent(&evtGuide);
//	 }
//	 else
//	 {
//		 UMMOUIMailReadPanel* maiRead = (UMMOUIMailReadPanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Mail_MailRead);
//		 if(maiRead&&maiRead->IsVisible)
//			 maiRead->Show(FALSE);
//	 }
// }