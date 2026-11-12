#include "stdafx.h"
#include "message_handler.h"
#include "MailManager.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
#include "UITextParser.h"
#include "mail_def.h"

using namespace ExternalMessage;

//------------------------------------------------------------------------
void CMessageHandler::Send_WriteMail(const FString& toPalyer, const FString& inSubject, const FString& inContent)
{
#if M_DEBUG_MAIL
	M2C_MAIL_WRITE_SUCCESS *msg = (M2C_MAIL_WRITE_SUCCESS*)ms_MsgBuffer;
	msg->wMsg = m2c_mail_write_success;
	Parse_m2c_mail_write_success((const char*)msg, sizeof(M2C_MAIL_WRITE_SUCCESS));
#else

	FString subject = inSubject;
	FString content = inContent;

	MMOUI::ReplaceCensoredWord(subject,CensoredWord_Chat);
	MMOUI::ReplaceCensoredWord(content,CensoredWord_Chat);

	C2M_MAIL_WRITE* msg = (C2M_MAIL_WRITE*)ms_MsgBuffer;
	msg->wMsg = c2m_mail_write;
	strxcpy(msg->szRoleName, *toPalyer, ROLE_NAME_LEN);
	strxcpy(msg->szMailSubject, *subject, MAIL_SUBJECT_LEN);
	msg->dwMailContentLen = strxcpy(msg->szMailContent, *content, MAIL_CONTENT_LEN);
	msg->szMailContent[msg->dwMailContentLen] = '\0';
	msg->wSize = sizeof(C2M_MAIL_WRITE) + msg->dwMailContentLen + 1;

	_Send(msg, msg->wSize);
#endif
}
//------------------------------------------------------------------------
void CMessageHandler::Send_RecvMailList()
{
#if M_DEBUG_MAIL
	
	M2C_MAIL_LIST_RECV* msg = (M2C_MAIL_LIST_RECV*)ms_MsgBuffer;
	msg->wMsg = m2c_mail_list_recv;
	msg->dwMailCount = 30;
	msg->wSize = sizeof(M2C_MAIL_LIST_RECV);

	for (UINT32 i = 0; i < msg->dwMailCount; ++i)
	{
		BriefMailInfo* newMail = &msg->stBriefMailInfo[i];
		newMail->byMailType = i % 2;
		newMail->byNew = i % 2;
		newMail->dwMailID = i;
		newMail->dwMailTime = ::time(NULL);
		newMail->byAttachment = i % 2;
		strxcpy(newMail->szSenderRoleName, *FString::Printf(TEXT("God%d"), i), ROLE_NAME_LEN);
		strxcpy(newMail->szMailSubject, *FString::Printf(TEXT("Hello Paladin!(%d)"), i), MAIL_SUBJECT_LEN);		
		msg->wSize += sizeof(BriefMailInfo);
	}
	
	Parse_m2c_mail_list_recv((const char*)msg, msg->wSize);

	M2C_MAIL_NEW_COUNT newCnt;
	newCnt.wMsg = m2c_mail_new_count;
	newCnt.dwMailNewCount = appRand();
	Parse_m2c_mail_new_count((const char*)&newCnt, sizeof(M2C_MAIL_NEW_COUNT));

#else
	C2M_MAIL_LIST_RECV msg;
	msg.wMsg = c2m_mail_list_recv;
	_Send(&msg, sizeof(C2M_MAIL_LIST_RECV));
#endif
}
//------------------------------------------------------------------------
void CMessageHandler::Send_ReadMail(UINT32 mailID)
{
#if M_DEBUG_MAIL
	
	const ClientBriefMailInfo* mail = MailManager::getSingleton().FindMail(mailID);
	check(mail != NULL);

	M2C_MAIL_READ* msg = (M2C_MAIL_READ*)ms_MsgBuffer;
	msg->wMsg = m2c_mail_read;
	msg->dwMailID = mailID;
	msg->dwMailTime = mail->dwMailTime;
	
	FString textContent(TEXT("Welcome to World of Aage of Dawn!\nTake my bless!"));
	strxcpy(msg->szMailContent, *textContent, MAIL_CONTENT_LEN);
		
	strxcpy(msg->szMailSubject, mail->szMailSubject, MAIL_SUBJECT_LEN);

	strxcpy(msg->szSenderRoleName, mail->szSenderRoleName, ROLE_NAME_LEN);

	Parse_m2c_mail_read((const char*)msg, sizeof(M2C_MAIL_READ));
#else
	C2M_MAIL_READ msg;
	msg.wMsg = c2m_mail_read;
	msg.dwMailID = mailID;
	_Send(&msg, sizeof(C2M_MAIL_READ));
#endif
}
//------------------------------------------------------------------------
void CMessageHandler::Send_DeleteMail(UINT32 mailID)
{
#if M_DEBUG_MAIL
	M2C_MAIL_DELETE_SUCCESS *msg = (M2C_MAIL_DELETE_SUCCESS*)ms_MsgBuffer;
	msg->dwMailID = mailID;
	msg->wMsg = m2c_mail_delete_success;
	Parse_m2c_mail_delete_success((const char*)msg, sizeof(M2C_MAIL_DELETE_SUCCESS));
#else
	C2M_MAIL_DELETE msg;
	msg.wMsg = c2m_mail_delete;
	msg.dwMailID = mailID;
	_Send(&msg, sizeof(C2M_MAIL_DELETE));
#endif
}

void CMessageHandler::Send_GetMailAttach(UINT32 mailID)
{
	C2M_MAIL_GET_ATTACH msg;
	msg.wMsg = c2m_mail_get_attach;
	msg.dwMailID = mailID;
	_Send(&msg, sizeof(C2M_MAIL_GET_ATTACH));
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_mail_list_recv(const char* buf, UINT32 size)
{
	M2C_MAIL_LIST_RECV *msg = (M2C_MAIL_LIST_RECV*)buf;

	DialogContext tmpCtx;
	for (UINT32 i = 0; i < msg->wMailCount; ++i)
	{
		ParseContext parseCtx;

		BriefMailInfo* mail = &msg->stBriefMailInfo[i];
		tmpCtx.Clear();
		UITextParser::getSingleton().Parse(parseCtx, tmpCtx, UTF8_TO_TCHAR(mail->szSenderRoleName));
		ZeroMemory(mail->szSenderRoleName, sizeof(mail->szSenderRoleName));
		strxcpy(mail->szSenderRoleName, *(tmpCtx.TempText), sizeof(mail->szSenderRoleName));
		tmpCtx.Clear();
		UITextParser::getSingleton().Parse(parseCtx, tmpCtx, UTF8_TO_TCHAR(mail->szMailSubject));
		ZeroMemory(mail->szMailSubject, sizeof(mail->szMailSubject));
		
		strxcpy(mail->szMailSubject, *(tmpCtx.TempText), sizeof(mail->szMailSubject));

		MailManager::getSingleton().AddOrUpdateMail(mail,tmpCtx.TempText);
	}

	MailManager::getSingleton().NotifyUpdateMailList();

	if (msg->wMailCount >= MAX_MAIL_LIST_COUNT)
	{
		MailManager::getSingleton().SetCheckMail(TRUE);
	}
	else
	{
		MailManager::getSingleton().SetCheckMail(FALSE);
	}
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_mail_write_success(const char* buf, UINT32 size)
{
	MailManager::getSingleton().NotifyWriteSuccess();
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_mail_read(const char* buf, UINT32 size)
{
	M2C_MAIL_READ *msg = (M2C_MAIL_READ*)buf;
	FString roleName = UTF8_TO_TCHAR(msg->szSenderRoleName);
	FString subject = UTF8_TO_TCHAR(msg->szMailSubject);
	//FString content = UTF8_TO_TCHAR(msg->szMailContent);

	DialogContext tmpCtx;
	ParseContext parseCtx;
	UITextParser::getSingleton().Parse(parseCtx, tmpCtx, UTF8_TO_TCHAR(msg->szSenderRoleName));
	ZeroMemory(msg->szSenderRoleName, sizeof(msg->szSenderRoleName));
	strxcpy(msg->szSenderRoleName, *(tmpCtx.TempText), sizeof(msg->szSenderRoleName));

	tmpCtx.Clear();
	UITextParser::getSingleton().Parse(parseCtx, tmpCtx, UTF8_TO_TCHAR(msg->szMailSubject));
	ZeroMemory(msg->szMailSubject, sizeof(msg->szMailSubject));
	strxcpy(msg->szMailSubject, *(tmpCtx.TempText), sizeof(msg->szMailSubject));

	ClientMailDetail newMail;
	appMemcpy(&newMail, msg, sizeof(M2C_MAIL_READ));
	newMail.mailSubject = tmpCtx.TempText;
	//<#T1008 For this kind of client-assembled mail the designer wants no length limit, so parse it only when it needs to be displayed
	//tmpCtx.Clear();
	//UITextParser::getSingleton().Parse(parseCtx, tmpCtx, UTF8_TO_TCHAR(msg->szMailContent));
	//ZeroMemory(msg->szMailContent, sizeof(msg->szMailContent));
	//strxcpy(msg->szMailContent, *(tmpCtx.TempText), sizeof(msg->szMailContent));

	MailManager::getSingleton().ReadMail(newMail);
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_mail_delete_success(const char* buf, UINT32 size)
{
	M2C_MAIL_DELETE_SUCCESS *msg = (M2C_MAIL_DELETE_SUCCESS*)buf;
	MailManager::getSingleton().DeleteMail(msg->dwMailID);
	MailManager::getSingleton().NotifyUpdateMailList();
	MailManager::getSingleton().CheckAllSelectMail();
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_mail_new_count(const char* buf, UINT32 size)
{
	M2C_MAIL_NEW_COUNT *msg = (M2C_MAIL_NEW_COUNT*)buf;
	
	MailManager::getSingleton().SetNewMailCount(msg->dwMailNewCount);
}

void CMessageHandler::Parse_m2c_mail_get_attach_success(const char* buf, UINT32 size)
{
	if (buf && size >= sizeof(M2C_MAIL_GET_ATTACH_SUCCESS))
	{
		M2C_MAIL_GET_ATTACH_SUCCESS* msg = (M2C_MAIL_GET_ATTACH_SUCCESS*)buf;
		MailManager::getSingleton().NotifyUpdateAcquireAttachment(msg->dwMailID);
	}
	
}