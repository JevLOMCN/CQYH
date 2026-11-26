#ifndef _MAILMANAGER_H_
#define _MAILMANAGER_H_

#include "MMOSingleton.h"
#include "m2c_message.h"
#include "MMOUtil.h"
#include "MMOFacade.h"
#include "MMOMsgButtonManager.h"
#include <list>

#define M_DEBUG_MAIL									0
#define MAX_MAILRECENTPLAYER_NUM			10
#define MAX_MAIL_NUM									140

#define MAX_MAIL_PAGE 10
#define MAX_MAIL_PAGELINE 5
#define MAX_MAIL 50


struct ClientBriefMailInfo : ExternalMessage::BriefMailInfo
{
	ClientBriefMailInfo() : IsSelected(FALSE)
	{
		appMemzero(static_cast<ExternalMessage::BriefMailInfo*>(this), sizeof(ExternalMessage::BriefMailInfo));
	}
	UBOOL IsSelected;
	FString mailSubject;
};

struct ClientMailDetail : ExternalMessage::M2C_MAIL_READ
{
	ClientMailDetail()
	{
		appMemzero(static_cast<ExternalMessage::M2C_MAIL_READ*>(this), sizeof(ExternalMessage::M2C_MAIL_READ));
	}
	FString mailSubject;
};

enum MailType
{
	UpdateMaxMailCount,
	NewMail,
	WriteSuccess,
	UpdateMailList,
	DeleteMail,
	GetAttachSuccess,
	UpdateRecentPlayers,
	SendMailTo,
	DeleteMailBox,
	updateSelectAll,
	NewMail_UpdatePanel,
};

struct MMOGameEvent_Mail
	:public MMOGameEvent
{
	MMOGameEvent_Mail(MailType mailType, INT count)
		:eMailType(mailType)
		,_MailCount(count)
		,Recevier(TEXT(""))
		,selectValue(FALSE)
	{

	}
	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Mail;
	}

	MailType eMailType;
	INT _MailCount;
	FString Recevier;
	BOOL selectValue;
};

typedef std::map<UINT32, struct ClientBriefMailInfo*, M_UE3_STL_ALLOC_MAP(UINT32, struct ClientBriefMailInfo*) > MailMap;
typedef std::map<UINT32, struct ClientMailDetail, M_UE3_STL_ALLOC_MAP(UINT32, struct ClientMailDetail) > MailDetailMap;
typedef std::vector<UINT32, M_UE3_STL_ALLOC(UINT32) > MailIDList;

class MailManager : public SingletonMeyers<MailManager>
{
public:

	MailManager();
	virtual ~MailManager();

	void Init();
	void Shutdown();

	void AddOrUpdateMail(struct ExternalMessage::BriefMailInfo* mail, FString mailSubject = "");
	const ClientBriefMailInfo* FindMail(UINT32 mailID) const;
	void MarkMailSelected(UINT32 mailID, bool sel);
	void MarkAllMailSelected(UBOOL sel);
	void MarkCurrentPageMailSelected(BOOL sel,INT page);
	BOOL IsCurrentPageMail(INT index, INT page);
	void ResetAllMailSelected();

	void DeleteMail(UINT32 mailID);		
	void DeleteSelected();
	const MailIDList& GetDeleteMails() const;

	const MailMap& GetAllMails() const;

	bool TryReadCachedMail(UINT32 mailID);
	void ReadMail( ClientMailDetail mail);
	void SetMailType(INT nType);
	INT  GetCurMailType(){return nCurMailType;};

	void ClearAllMails();

	void SetOpenReadMailDelegate(FScriptDelegate fun);
	void SetOpenMailPanelDelegate(FScriptDelegate fun);
	FScriptDelegate GetOpenMailPanelDelegate();

	void SetNewMailCount(UINT32 newMails);

	void AddRecentPlayer(const FString& strName);
	const TArray<FString>& GetRecentPlayersArray() const;
	void	LoadData();
	void	SaveData();

	INT GetMailsCount() const;
	void SetMailWndOpen(BOOL bNeed); 
	void DeleteNewMailBallHint();

	void NotifyWriteSuccess();
	void NotifyUpdateMaxMailCount();
	void NotifyDeleteMail(INT mailID);
	void NotifyUpdateMailList();
	void NotifyNewMailCount();
	void NotifyUpdateRecentPlayers();
	void NotifyUpdateAcquireAttachment(INT mailID);
	void NotifySendMailToReceiver(FString recever);

	BOOL GetCheckMail(){return _needCheckMail;}
	void SetCheckMail(BOOL v) {_needCheckMail = v;}

	INT GetCurrentPage(){return _CurrentPage;}
	void SetCurrentPage(INT p ){_CurrentPage = p;}

	void CheckAllSelectMail();

	void SetCurrentPageNum(INT page, INT num);

	FString GetMailSystemIcon(){return mailSystemIconPath;}

	INT GetSendMailDelayTime(){return sendMailDelayTime;}

	ClientMailDetail* GetCurReadMail();

protected:
	MailMap _MailMap;
	INT _NewMailCount;
	MailDetailMap _CachedMailMap;
	TArray<FString> _RecentPlayerArray;

	FScriptDelegate _OpenReadMailDelegate;
	FScriptDelegate _OpenMailPanelDelegate;

	MailIDList deleteMails;
	BOOL			_needOpenMailWnd;
	INT			nCurMailType;
	void _ReadMail( ClientMailDetail mail );

	BOOL _needCheckMail;

	INT _CurrentPage;
	INT _CurrentPageNum;

	INT sendMailDelayTime;

	FString mailSystemIconPath;

	INT _CurrentMailID;
};

#endif
