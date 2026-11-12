#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOStats.h"
#include "UMMOUIMailReadPanel.h"
#include "MailManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOChatManager.h"

IMPLEMENT_CLASS(UMMOUIMailReadPanel)

#define  MILLISECOND 3600000

//-----------AS----------------
void UMMOUIMailReadPanel::ASReplyMail(INT mailID)
{
	const ClientBriefMailInfo* mail = MailManager::getSingleton().FindMail(mailID);
	if(mail)
	{
		FString sender = UTF8_TO_TCHAR(mail->szSenderRoleName);
		FString subjReply = /*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.ReplySubj"))
			+ *//*UTF8_TO_TCHAR(*/mail->mailSubject/*)*/;
		UMMOUIMailPanel* mailListPanel = (UMMOUIMailPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Mail);
		if (mailListPanel)
		{
			mailListPanel->eventCallReplyMail(sender, subjReply);
		}
		
	}
	else
	{
		warnf(NAME_Error, TEXT("[UMMOGFXPlayer_Mail::ASReplyMail] failed to find mail:%d"), mailID);
	}
}

void UMMOUIMailReadPanel::ASDeleteMail(INT mailID)
{

	hideMsg();

	const MailMap& AllMailMap = MailManager::getSingleton().GetAllMails();
	MailMap::const_iterator it = AllMailMap.find(mailID);
	if(it == AllMailMap.end())
		return;
	if(it->second->byAttachment != 0)
	{
		/*FScriptDelegate DelMailDelegate(EC_EventParm);
		DelMailDelegate.FunctionName = TEXT("OnDeleteMail");
		DelMailDelegate.Object = this;*/

		FScriptDelegate Nofunction(EC_EventParm);
		Nofunction.FunctionName = TEXT("OnDeleteMail_Cancel");
		Nofunction.Object = this;

		FString szText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.CannotDelete"));
		msgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( szText , mailID , Nofunction , Nofunction );
	}
	else
	{
		CMessageHandler::GetInstance().Send_DeleteMail(mailID);
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Mail_MailRead, FALSE, "");
	}
}

void UMMOUIMailReadPanel::OnDeleteMail(UGFxObject* mc)
{
	if(!mc)
		return;

	INT mailID = mc->GetInt( TEXT("iteminfoid") );

	CMessageHandler::GetInstance().Send_DeleteMail(mailID);
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Mail_MailRead, FALSE, "");
}

void UMMOUIMailReadPanel::OnDeleteMail_Cancel(UGFxObject* mc)
{
	msgBox = NULL;
}

void UMMOUIMailReadPanel::ASAcquireMailItems(INT mailID)
{
	const ClientBriefMailInfo* mail = MailManager::getSingleton().FindMail(mailID);
	if(mail && mail->byAttachment)
	{
		CMessageHandler::GetInstance().Send_GetMailAttach(mailID);
	}
}


 void UMMOUIMailReadPanel::NativeInitOnce()
 {
	 MailManager::getSingleton().SetOpenReadMailDelegate(FScriptDelegate(this, TEXT("OpenReadMail")));


	 check(EventAdapter == NULL);
	 if(NULL == EventAdapter)
	 {
		 TArray<EGameEvent> evts;
		 evts.AddItem(GameEvent_Mail);
		 EventAdapter = new TGameEventAdapter<UMMOUIMailReadPanel>(this, evts);
	 }

	 UGFxObject* obj = NULL;
	 obj = GetObject( TEXT("btn_reply") );
	 if(obj)
		 obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_reply") ));

	 obj = GetObject( TEXT("btn_delete") );
	 if(obj)
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_delete_mail") ));

	 obj = GetObject( TEXT("btn_acquire") );
	 if(obj)
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.btn_acquire") ));

	 obj = GetObject(TEXT("mail_item_0"));
	 if(obj)
		 obj->SetInt( TEXT("SlotType") , ESPType_ReadMail );
 }

 void UMMOUIMailReadPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
 {
	 MMOGameEvent_Mail* pMailEvt = (MMOGameEvent_Mail*)evt;
	 if(pMailEvt == NULL)
		 return;
	 switch(pMailEvt->eMailType)
	 {
	 case MailType::DeleteMail:
		 {
			 OnMailDelete(pMailEvt->_MailCount);
			 break;
		 }
	 case MailType::GetAttachSuccess:
		 {
			 UpdateAcquireAttachment();
			 break;
		 }
	 case MailType::DeleteMailBox:
		 {
			 DeleteSelected();
			 break;
		 }
	 }
 }

 void UMMOUIMailReadPanel::DeleteSelected()
 {
	 hideMsg();
	 FScriptDelegate DelAllMailDelegate(EC_EventParm);
	 DelAllMailDelegate.FunctionName = TEXT("OnDeleteSelected");
	 DelAllMailDelegate.Object = this;

	 FScriptDelegate Nofunction(EC_EventParm);
	 Nofunction.FunctionName = TEXT("OnDeleteMail_Cancel");
	 Nofunction.Object = this;

	 FString szText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mail.CannotDelete"));
	 msgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( szText , 0 , Nofunction , Nofunction );
 }

 void UMMOUIMailReadPanel::OnDeleteSelected()
 {
	 const MailIDList& deleteMails = MailManager::getSingleton().GetDeleteMails();
	 for (size_t j = 0; j < deleteMails.size(); ++j)
	 {
		 CMessageHandler::GetInstance().Send_DeleteMail(deleteMails[j]);
	 }
 }

 //------------------------------------------------------------------------
 void UMMOUIMailReadPanel::OpenReadMail(INT mailID,INT mailTime,BYTE mailType, INT mailAttachmentItemID, int mailAttachmentItemCount,
	 const FString& sender,const FString& subject,const FString& content,INT totalCD,INT cd)
 {
	 _CurrentMailID = mailID; 
	 //Bound gold ID in the item table = 0
	 if(mailAttachmentItemID >= 0)
	 {
		 UGFxObject* itemSlot = GetObject( TEXT("mail_item_0") );
		 UGFxObject* pAmount = GetObject(TEXT("tl_itemcount"));
		 if(itemSlot&&pAmount)
		 {
			 pAmount->SetText("");
			 UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(mailAttachmentItemID);
			 if(itemTemp)	 
			 {
				 GFXPlayer->SetItemSlotData( itemSlot , itemTemp, ESPType_ReadMail, E_CONSLOT_MAX, 0);

				 if(itemTemp->ItemDuration == E_DURTYPE_STACK&&mailAttachmentItemCount > 0)
				 {
					 FString strCount = FString::Printf(TEXT("X %d"),mailAttachmentItemCount);
					 pAmount->SetText(strCount);
				 } 
			 }
			 else
			 {
				  FString strHint = FString::Printf(TEXT("This mail contains an invalid item id: %d"),mailAttachmentItemID);
				  MMOUI::GetUIManager()->delegateAddScreenMessage(strHint); 
			 }
		 }
	 }

	FString msg = GetCDMsg(totalCD,cd);
	eventCallUpdateMailInfo(mailID, mailType, mailAttachmentItemID, sender, subject, content, totalCD, cd,msg);
	this->Show(TRUE);
	UMMOUIMailPanel* pkMail = (UMMOUIMailPanel*)MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Mail);
	if(pkMail)
	{
			FLOAT xPos,yPos;
		pkMail->GetPosition(xPos,yPos);
		/*xPos += pkMail->eventGetWidth();
		yPos += (pkMail->eventGetHeight() - eventGetHeight())*0.5f;
		SetPosition(xPos,yPos);*/
		eventCallSetReadPanelPostion(xPos,yPos);
	}
 }

 void UMMOUIMailReadPanel::UpdateAcquireAttachment()
 {
	 eventCallUpdateAcquireAttachment();
 }

 void UMMOUIMailReadPanel::OnMailDelete(INT mailID)
 {
	 if(IsVisible && (mailID == _CurrentMailID))
	 {
		 Show(FALSE);
	 }
	 FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.MailDeleteTips"));
	 MMOChatManager::getSingleton().AddSystemMessage(strTips); 
	 MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.Mail.MailDeleteTips")); 
 }

 void UMMOUIMailReadPanel::hideMsg()
 {
	 if( msgBox != NULL )
	 {
		 MMOMsgBoxManager::getSingleton().GetMessageAck( FALSE , msgBox);
		 //msgBox->SetVisible( FALSE );
		 msgBox = NULL;
	 }
 }


FString UMMOUIMailReadPanel::GetCDMsg(INT32 totalTime,INT32 cd)
{
	if (totalTime > 0)
	{
		totalTime = totalTime / MILLISECOND;
	}
	if(cd > 0)
	{
		cd = cd / MILLISECOND;
	}
	FString msg =  FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.CDMsg")) ,totalTime/*,cd*/);
	return msg;
}
