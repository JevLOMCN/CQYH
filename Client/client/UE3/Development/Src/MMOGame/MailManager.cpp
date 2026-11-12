#include "stdafx.h"
#include "MailManager.h"
#include "net/message_handler.h"
#include "MMOFacade.h"
#include <vector>
#include "mail_def.h"
#include "MMOQuestManager.h"
#include "MMOChatManager.h"
#include "UITextParser.h"
#include "MMOClientVariableManager.h"

using namespace std;
using namespace ExternalMessage;


MailManager::MailManager()
	: _NewMailCount(0)
{
	nCurMailType = -1;
	_needCheckMail = FALSE;
	_CurrentPage = 1;
	_CurrentPageNum = MAX_MAIL_PAGELINE;
	_CurrentMailID = 0;
}


MailManager::~MailManager()
{
	
}


void MailManager::Init()
{
	GConfig->GetString( TEXT("Mail"), TEXT("MailSystemIconPath") , mailSystemIconPath , GUIColorIni);
	GConfig->GetInt( TEXT("Mail"), TEXT("MailDelayTime") , sendMailDelayTime , GDefaultGameIni);
	
}


void MailManager::Shutdown()
{
	ClearAllMails();

	//save recent contacts of mail system
	SaveData();
}
void MailManager::SetMailType(INT nType)
{
	nCurMailType = nType;
}
void MailManager::AddOrUpdateMail( struct BriefMailInfo* mail, FString mailSubject )
{
	MailMap::iterator i = _MailMap.find(mail->dwMailID);
	if(i == _MailMap.end())
	{
		ClientBriefMailInfo* newMail = new ClientBriefMailInfo;
		appMemcpy(newMail, mail, sizeof(BriefMailInfo));
		newMail->IsSelected = false;
		newMail->mailSubject = mailSubject;
		if (newMail->byMailType != MAILTYPE_SYSTEM)
		{
			MMOUI::ReplaceCensoredWord(newMail->mailSubject,CensoredWord_Chat);
		}
		_MailMap.insert(std::make_pair(mail->dwMailID, newMail));
		NotifyUpdateMaxMailCount();
	}
}

const ClientBriefMailInfo* MailManager::FindMail( UINT32 mailID ) const
{
	MailMap::const_iterator i = _MailMap.find(mailID);
	if(i != _MailMap.end())
	{
		return i->second;
	}

	return 0;
}

void MailManager::MarkMailSelected(UINT32 mailID, bool sel)
{
	MailMap::const_iterator i = _MailMap.find(mailID);
	if(i != _MailMap.end())
	{
		i->second->IsSelected = sel;
		NotifyUpdateMailList();
	}
}

void MailManager::CheckAllSelectMail()
{
	INT index = 1;

	INT count = 0;
	
	if ( (_CurrentPage * MAX_MAIL_PAGELINE) > MAX_MAIL)
		return;

	MailMap::const_reverse_iterator i = _MailMap.rbegin();
	for (; i != _MailMap.rend(); ++i)
	{
		if(nCurMailType >= MAILTYPE_USER &&nCurMailType != i->second->byMailType)
			continue;

		if (IsCurrentPageMail(index,_CurrentPage))
		{
			if(i->second->IsSelected == TRUE)
			{
				count += 1;
			}
		}
		index++;
	}

	BOOL v = TRUE;
	if (count < _CurrentPageNum)
	{
		v = FALSE;
	}

	MMOGameEvent_Mail evt(updateSelectAll, 0);
	evt.selectValue = v;
	MMOUI::PostGameEvent(&evt);
	
}

void MailManager::SetCurrentPageNum(INT page, INT num)
{
	if (page == _CurrentPage)
	{
		_CurrentPageNum = num;
	}
}

void MailManager::MarkAllMailSelected( UBOOL sel )
{
	MailMap::iterator i = _MailMap.begin();
	for (; i != _MailMap.end(); ++i)
	{
		if(nCurMailType >= MAILTYPE_USER &&nCurMailType != i->second->byMailType)
			continue;
		i->second->IsSelected = sel;
	}

	NotifyUpdateMailList();
}

void MailManager::ResetAllMailSelected()
{
	MailMap::iterator i = _MailMap.begin();
	for (; i != _MailMap.end(); ++i)
	{
		if(nCurMailType >= MAILTYPE_USER &&nCurMailType != i->second->byMailType)
			continue;
		i->second->IsSelected = FALSE;
	}

	NotifyUpdateMailList();
}

void MailManager::MarkCurrentPageMailSelected(BOOL sel,INT page)
{
	INT index = 1;

	if ( (page * MAX_MAIL_PAGELINE) > MAX_MAIL)
		return;

	MailMap::const_reverse_iterator i = _MailMap.rbegin();
	for (; i != _MailMap.rend(); ++i)
	{
		if(nCurMailType >= MAILTYPE_USER &&nCurMailType != i->second->byMailType)
			continue;

		if (IsCurrentPageMail(index,page))
		{
			i->second->IsSelected = sel;
		}
		index++;
	}
	NotifyUpdateMailList();
	_CurrentPage = page;
}

BOOL MailManager::IsCurrentPageMail(INT index, INT page)
{
	INT currentPageMailNum = MAX_MAIL_PAGELINE * page;
	INT lastPageMailNum = MAX_MAIL_PAGELINE* (page - 1);

	if (lastPageMailNum < index &&  index <= currentPageMailNum)
		return TRUE;
	else
		return FALSE;

}

void MailManager::DeleteMail( UINT32 mailID )
{
	// cached detailed mail must also be deleted
	MailDetailMap::iterator it = _CachedMailMap.find(mailID);
	if(it != _CachedMailMap.end())
	{
		_CachedMailMap.erase(it);
	}


	MailMap::iterator i = _MailMap.find(mailID);
	if(i != _MailMap.end())
	{
		if (i->second->byNew)
		{
			_NewMailCount = Max(0, _NewMailCount - 1);
			DeleteNewMailBallHint();
			NotifyNewMailCount();
		}

		delete i->second;
		_MailMap.erase(i);

		NotifyDeleteMail(mailID);
		NotifyUpdateMailList();
		NotifyUpdateMaxMailCount();
	}
	else
	{
		warnf(NAME_Warning, TEXT("[MailManager::DeleteMail] mail not found:%d"), mailID);
	}
}

void MailManager::ReadMail(ClientMailDetail mail)
{
	_CachedMailMap[mail.dwMailID] = mail;

	if(_needOpenMailWnd)
		_ReadMail(mail);
	
	MailMap::const_iterator i = _MailMap.find(mail.dwMailID);
	if(i != _MailMap.end())
	{
		if(i->second->byNew)
		{
			_NewMailCount = Max(0, _NewMailCount - 1);
			DeleteNewMailBallHint();
			NotifyNewMailCount();
		}

		i->second->byNew = false;
		NotifyUpdateMailList();
	}
}
void MailManager::SetMailWndOpen(BOOL bNeed)
{
	_needOpenMailWnd = bNeed;
}
void MailManager::ClearAllMails()
{
	MailMap::iterator i = _MailMap.begin();
	for (; i != _MailMap.end(); ++i)
	{
		delete i->second;
	}

	_MailMap.clear();
}

const MailMap& MailManager::GetAllMails() const
{
	return _MailMap;
}

void MailManager::SetOpenReadMailDelegate( FScriptDelegate fun )
{
	_OpenReadMailDelegate = fun;
}

void MailManager::SetOpenMailPanelDelegate(FScriptDelegate fun)
{
	_OpenMailPanelDelegate = fun;
}

FScriptDelegate MailManager::GetOpenMailPanelDelegate()
{
	return _OpenMailPanelDelegate;
}

void MailManager::NotifyWriteSuccess()
{
	MMOGameEvent_Mail evt(WriteSuccess, 0);
	MMOUI::PostGameEvent(&evt);

	// Guide quest
	MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Mail);
}

void MailManager::NotifyUpdateMaxMailCount()
{
	MMOGameEvent_Mail evt(UpdateMaxMailCount, _MailMap.size());
	MMOUI::PostGameEvent(&evt);
}

void MailManager::NotifyDeleteMail(INT mailID)
{
	MMOFacade::getSingleton().PlayUISound(TEXT("Maildelete"));

	MMOGameEvent_Mail evt(MailType::DeleteMail, mailID);
	MMOUI::PostGameEvent(&evt);
}

void MailManager::NotifyUpdateMailList()
{
	MMOGameEvent_Mail evt(UpdateMailList, 0);
	MMOUI::PostGameEvent(&evt);
}

void MailManager::NotifyUpdateRecentPlayers()
{
	MMOGameEvent_Mail evt(UpdateRecentPlayers, 0);
	MMOUI::PostGameEvent(&evt);
}

void MailManager::NotifyUpdateAcquireAttachment(INT mailID)
{
	MailDetailMap::iterator it =  _CachedMailMap.find(mailID);
	if(it != _CachedMailMap.end())
		it->second.nAttachmentItemID = -1;

	MailMap::const_iterator i = _MailMap.find(mailID);
	if(i != _MailMap.end())
	{
		if(i->second->byNew)
		{
			_NewMailCount = Max(0, _NewMailCount - 1);
			DeleteNewMailBallHint();
			NotifyNewMailCount();
		}
		i->second->byAttachment = 0;
		i->second->byNew = FALSE;
	}

	MMOGameEvent_Mail evt(GetAttachSuccess, 0);
	MMOUI::PostGameEvent(&evt);

	MMOGameEvent_Mail evt0(UpdateMailList, 0);
	MMOUI::PostGameEvent(&evt0);
}

void MailManager::NotifySendMailToReceiver(FString recever)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Mail, TRUE, "");

	MMOGameEvent_Mail evt(SendMailTo, 0);
	evt.Recevier = recever;
	MMOUI::PostGameEvent(&evt);
}

void MailManager::DeleteSelected()
{
	deleteMails.clear();
	BOOL bShowBox = FALSE;
	MailMap::iterator i = _MailMap.begin();
	for (; i != _MailMap.end(); ++i)
	{
		if(i->second->IsSelected)
		{
			deleteMails.push_back(i->first);
		}
		//bound gold ID == 0!!!!
		if (i->second->IsSelected && i->second->byAttachment != 0)
		{
			bShowBox = TRUE;
		}
	}

	if(bShowBox)
	{
		MMOGameEvent_Mail evt(DeleteMailBox, 0);
		MMOUI::PostGameEvent(&evt);
	}
	else
	{
		for (MailIDList::iterator it = deleteMails.begin(); it != deleteMails.end(); ++it)
		{
			 CMessageHandler::GetInstance().Send_DeleteMail(*it);
		}
	}
}

const MailIDList& MailManager::GetDeleteMails() const
{
	return deleteMails;
}


void MailManager::SetNewMailCount( UINT32 newMails )
{
	_NewMailCount = newMails;
	DeleteNewMailBallHint();
	NotifyNewMailCount();
	if (0 < _NewMailCount)
	{
		if(MMO::GetCurrentGameState() == EGameState_InGame)
		{
			FString strTips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mail.NewMail"));
			MMOChatManager::getSingleton().AddSystemMessage(strTips); 
			MMOUI::GetUIManager()->delegateAddScreenMessage(strTips,MsgInfoType_Table,TEXT("MMOGame.Mail.NewMail")); 
		}

		MMOGameEvent_Mail evt(NewMail_UpdatePanel, _NewMailCount);
		MMOUI::PostGameEvent(&evt);
		
		//for (INT n = 0; n < _NewMailCount; ++n)
		//{
			//MMOMsgButtonManager_Parms Parms(EC_EventParm);
			//MMOMsgButtonManager::getSingleton().AddNewButton(_OpenMailPanelDelegate, Parms, E_MsgBtnShow_Mail );
		//}
	}
}

void MailManager::NotifyNewMailCount()
{
	MMOGameEvent_Mail evt(NewMail, _NewMailCount);
	MMOUI::PostGameEvent(&evt);
}
void MailManager::DeleteNewMailBallHint()
{
	MMOMsgButtonManager::getSingleton().RemoveOneTypeButtons(E_MsgBtnShow_Mail);
}
bool MailManager::TryReadCachedMail( UINT32 mailID )
{
	MailDetailMap::const_iterator i = _CachedMailMap.find(mailID);
	if(i != _CachedMailMap.end())
	{
		_ReadMail(i->second);
		return true;
	}

	return false;
}

void MailManager::_ReadMail( ClientMailDetail mail )
{
	_CurrentMailID = mail.dwMailID;

	MMOUIMailReadPanel_eventOpenReadMailDelegate_Parms Parms(EC_EventParm);
	Parms.mailID = mail.dwMailID;
	Parms.mailTime = mail.dwMailTime;
	Parms.mailType = mail.byMailType;
	Parms.mailAttachmentItemID = mail.nAttachmentItemID;
	Parms.mailAttachmentItemCount = mail.nAttachmentItemCount;
	Parms.subject = /*UTF8_TO_TCHAR(*/(mail.mailSubject);

	DialogContext tmpCtx;
	ParseContext parseCtx;
	UITextParser::getSingleton().Parse(parseCtx, tmpCtx, FString(UTF8_TO_TCHAR(mail.szMailContent)));
	Parms.content =  tmpCtx.TempText;

	if(mail.byMailType == MAILTYPE_SYSTEM)
	{
		Parms.sender = TEXT("<IMG  SRC='img://MMO_UIIcon.ChatChannelIcon.ChatChannelSystem'/>");
	}
	else
	{
		Parms.sender = UTF8_TO_TCHAR(mail.szSenderRoleName);
		MMOUI::ReplaceCensoredWord(Parms.subject,CensoredWord_Chat);
		MMOUI::ReplaceCensoredWord(Parms.content,CensoredWord_Chat);
	}

	//attachment unlock time
	UINT32 cdtime = 0;

	if (mail.nAttachCDTime > 0)
	{
		__time32_t today; 
		_time32(&today);
		_localtime32(&today);

		if (mail.dwMailTime + mail.nAttachCDTime > (UINT32)today)
		{
			cdtime = mail.dwMailTime + mail.nAttachCDTime - (UINT32)today;
		}
		
	}
	
	Parms.totalCD = mail.nAttachCDTime;
	Parms.cd = cdtime;


	_OpenReadMailDelegate.Object->ProcessDelegate(MMOGAME_OpenReadMailDelegate, &_OpenReadMailDelegate,&Parms);
}

void MailManager::AddRecentPlayer(const FString& strName)
{
	for (INT i = 0; i < _RecentPlayerArray.Num(); ++i)
	{
		if (_RecentPlayerArray(i) == strName)
		{
			_RecentPlayerArray.Remove(i);
		}
	}

	if (MAX_MAILRECENTPLAYER_NUM <= _RecentPlayerArray.Num())
	{
		_RecentPlayerArray.Remove(_RecentPlayerArray.Num() - 1);
	}
	_RecentPlayerArray.InsertItem(strName, 0);
}

const TArray<FString>& MailManager::GetRecentPlayersArray() const
{
	return _RecentPlayerArray;
}

void MailManager::LoadData()
{
	FArchive* archive = NULL;
	size_t dataLength = 0; 
	FString dataName = TEXT("Mail");
	TCHAR Buf[ROLE_NAME_LEN * MAX_MAILRECENTPLAYER_NUM + 64];
	ZeroMemory(Buf, sizeof(Buf));
	if(MMOFacade::getSingleton().BeginLoadAccountBinary(&archive, dataName))
	{
		archive->Serialize(Buf, archive->TotalSize());
		FString recentPlayersData(Buf);
		recentPlayersData.ParseIntoArray(&_RecentPlayerArray, TEXT("|"), TRUE);
	}
	MMOFacade::getSingleton().EndLoadAccountBinary(archive);
}

void MailManager::SaveData()
{
	FString data;
	FString dataName = TEXT("-Mail-");
	for (INT i = 0; i < _RecentPlayerArray.Num(); ++i)
	{
		data += _RecentPlayerArray(i) + TEXT("|");
	}
	TCHAR Buf[ROLE_NAME_LEN * MAX_MAILRECENTPLAYER_NUM + 64];
	ZeroMemory(Buf, sizeof(Buf));
	memcpy_s(Buf, sizeof(Buf), *data, sizeof(Buf));
	MMOFacade::getSingleton().ExportSimpleAccountData((void*)Buf, sizeof(Buf), dataName);
}

INT MailManager::GetMailsCount() const
{
	return _MailMap.size();
}

ClientMailDetail* MailManager::GetCurReadMail()
{
	MailDetailMap::iterator it = _CachedMailMap.find(_CurrentMailID);
	if(it != _CachedMailMap.end())
	{
		return &(it->second);
	}
	return NULL;
}
