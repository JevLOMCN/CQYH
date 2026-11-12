#ifndef _FRIENDCHATMANAGER_H_
#define _FRIENDCHATMANAGER_H_

#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"

#include "MMOSingleton.h"

struct ContactLogStruct
{
	TArray<FString> LogList;
	FString PlayerName;
	INT RoleId;

	ContactLogStruct() {}
	ContactLogStruct(EEventParm)
	{
		appMemzero(this, sizeof(ContactLogStruct));
	}
};

struct MMOGameEvent_FriendChat_ShowChatHint : public MMOGameEvent
{
public:

	MMOGameEvent_FriendChat_ShowChatHint(INT idx)
		: roldId(idx)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_ShowFriendChatHint; }

	INT roldId;
};

struct MMOGameEvent_FriendChat_SetChatHintImg : public MMOGameEvent
{
public:

	MMOGameEvent_FriendChat_SetChatHintImg(INT idx)
		: roldId(idx)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_SetFriendChatHint; }

	INT roldId;
};

class FriendChatManager : public SingletonMeyers<FriendChatManager>, public INativeGameEventHandler
{
public:

	void Init();
	void UnInit();
	void ClearAll();

	FriendChatManager();
	virtual ~FriendChatManager();

	void SaveChatLogToFile();
	void LoadChatLogFromFile();

	//chat list maintenance
	void AddNewRoleIDChatList(int nRoleID);
	void RemoveRoleIDChatList(int nRoleID);
	BOOL IsInRoleChatList(int nRoleID);
	INT GetRoleIndexByChatList(INT nRoleId);
	TArray<int>& GetChatList() {return curChatRoleIDList;};

	//chat record maintenance

	void RemoveUnReadChatContent(INT nRoleID);
	void AddNewChatReadLog(INT nRoleID,FString& strName, FString strContent,BOOL bfromSelf = FALSE);
	void AddNewChatUnReadLog(INT nRoleID,FString& strName, FString strContent,BOOL addLog = FALSE);
	void AddNewChatLogList(INT nRoleID,FString& strName, FString strContent,BOOL bfromSelf = FALSE);

	TArray<ContactLogStruct>& GetLogList() ;
	TArray<ContactLogStruct>& GetReadLog() ;
	TArray<ContactLogStruct>& GetUnReadLog() ;

	INT  AddReadLog(ContactLogStruct& log);
	void Shutdown();
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void ClearRoleIDChatList();

	const static  INT MaxFriendChatLogNum  = 100;
	const static  INT ShowMaxLog  = 100;

private:
	FString _GetShowText( const FString& playerName , const FString& content , UBOOL myWord );
private:
	//list of all messages
	TArray<ContactLogStruct> ContactLogList;
	//list of read messages
	TArray<ContactLogStruct> ReadContactLogList;
	//list of unread messages
	TArray<ContactLogStruct> UnReadContactLogList;

private:
	TArray<int>              curChatRoleIDList;

protected:

	UBOOL HasLoad;
	UBOOL HasSave;

};

#endif