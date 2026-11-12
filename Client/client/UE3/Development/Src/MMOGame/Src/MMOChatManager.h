
#ifndef _MMOChatManager_H_
#define _MMOChatManager_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#define ADD_MAP_USER_MARK_PROFIX ( TEXT("<A3596 ") )

#define GM_CHAT_COLOR (TEXT("#f64e1a"))
struct ChatMessage
{
	FString	ChatFilterChannelMsg; //channel message
	FString	ChatPlayer; //speaking player
	INT		ChatActorID;
	INT     ChatTypeId; //type
	INT		TargetId;
	INT		VipType;
	BOOL    isMe;
	unsigned bGM;
	ChatMessage()
	{
		ChatActorID = -1;
		ChatTypeId = -1;
		TargetId = -1;
		VipType = -1;
		bGM = 0;
		isMe = FALSE;
	}
};

//save chat channel messages
struct ChatFilterChannelMessage
{
	INT ChannelFilterType; // channel filter flag
	TArray<ChatMessage> ChatMessageList;
	UBOOL IsDefaultChannel;
	FString ChannelName;
};

enum ChatChnnelType
{
	CCPrivate,
	CCGuild,
	CCNear,
	CCTeam,
	CCDefault,
};

struct MMOGameEvent_Chat_AppendInput : public MMOGameEvent
{
public:

	MMOGameEvent_Chat_AppendInput(BYTE type, BYTE cid, INT sid, UBOOL def)
		: SlotType(type),
		ContainerID(cid),
		SlotID(sid),
		bDefault(def)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_Chat_AppendInput; }

	BYTE SlotType;
	BYTE ContainerID;
	INT SlotID;
	UBOOL bDefault;
};

struct MMOGameEvent_Chat_ShowPrivateTabEffect : public MMOGameEvent
{
public:

	MMOGameEvent_Chat_ShowPrivateTabEffect(INT idx)
		: tabId(idx)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_ShowPrivateTabEffect; }

	INT tabId;
};

struct MMOGameEvent_Chat_ShowChatErrorMsg : public MMOGameEvent
{
public:

	MMOGameEvent_Chat_ShowChatErrorMsg(FString err)
		: errMsg(err)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_Chat_ErrorMsg; }

	FString errMsg;
};

enum ESpecialCommandItem
{
	ESCI_TeleportRing = 99930023,
	ESCI_SearchNecklace = 99920023,
	ESCI_MemNecklace = 99920025,
	ESCI_MemRing = 99930031,
	ESCI_MemHand = 99940022,
};

class MMOChatManager : public SingletonMeyers<MMOChatManager>, public INativeGameEventHandler
{
public:

	MMOChatManager();
	virtual ~MMOChatManager();

	void Init();
	void Shutdown();

	void ClearAll();

	void AddDefaultFilterChannel();
	void SetFilterChannel(INT pos , const FString& ChannelName , INT FilterChannelType , UBOOL defaultChannel );
	void AddNewFilterChannelCopy( );
	void DelFilterChannelCopy( INT pos );

	void ReceiveMessage( INT location , INT ChannelId , INT TargetId , FString MsgContent , FString SenderName , INT SenderID , INT targeId = -1, unsigned bGM = 0 );

	void AddChatMessage( INT typeId , const FString& messageContent , const FString& senderName , INT senderID = -1 , UBOOL iSay = FALSE , INT targetId = -1, unsigned bGM = 0);
	void AddSystemMessage(const FString& MessageContent);
	void ImageFilter( FString& messageContent );

	void AddHintMessage(const FString& MessageContent);

	void ReCopyChatFilter();
	void ApplyCopyChatFilter();
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	TArray<ChatMessage>& GetChatBuffList(){ return ChatBuffList;}
	TArray<ChatMessage>& GetChatTrumpetList(){ return ChatTrumpetList;}
	TArray<ChatFilterChannelMessage>& GetChatFilterChannelList(){ return ChatFilterChannelList; }
	TArray<ChatFilterChannelMessage>& GetCopyChatFilterChannelList(){ return CopyChatFilterChannelList; } 

	UGFxObject* GreateChatObject(ChatMessage& CM ,UMMOGFXPlayer* GFXPlayer);

	//
	UBOOL ParseShareMapMark( const FString &chatContent, INT channelType, INT chatTarget, INT senderID, FString senderName );

	FString getVipIconPath(INT vipType);

	void AddScreenMessage();

	void SaveRecord();
	void LoadRecord();

	void SetLastChannelTab(BOOL v){_IsAddChannelTab = v;}
	BOOL GetLastChannelTab(){return _IsAddChannelTab;}

	void NotifyUpdateChannelTabClicked();

	INT GetChatFilterChannelListNum();


	BOOL CheckTrumpetItem();
	void RemoveTrumpet();
	BOOL GetShowTrumpet(){return isShowTrumpet;}
	void SetShowTrumpet(BOOL v){isShowTrumpet = v;}

	INT GetTrumpetTime(){return TrumpetTime;}
	INT GetTrumpetTimes(){return TrumpetTimes;}

	FString GetCurrentTrumpet();

	BOOL GetIsInTrumpetCDTimes(){return isInTrumpetCDTime;}
	void SetIsInTrumpetCDTimes(BOOL v){ isInTrumpetCDTime = v;}
	INT GetInTrumpetCDTimes(){return TrumpetCDTimes;}

	void ReCopyDefaultChatFilter();
	void SetDefaultChatFilter(const FString& ChannelName , INT FilterChannelType , UBOOL defaultChannel);
	void AddDefaultChatFilter();

	FString ParseNpcDialog(const FString& context);
	FString ParseNormalMsg(const FString& msg);

	BOOL IsChatFocus();
	void SetFocus(BOOL v){bFocus = v;}
	void NotifySetChannelByTab();

	BOOL ParsePrivateChatMessage(FString context);

	FString GetChatNearSign(){return NearSign;}
	FString GetChatGuildSign(){return GuildSign;}
	FString GetChatTeamSign(){return TeamSign;}

	void NoitfyShowPrivateTabEffect(ChatMessage CM);

	BOOL IsImgChat(FString content);

	void CleanChannelChat(INT id);

	INT GetChatNearCD(){return ChatNearCD;}

	// quest #18213
	UBOOL ParseSpecialCommand(const FString& cmd);

	void AddHintItem(ChatMessage CM);

	void AddChatErrorMsg(FString msg);

	void AddPrivateListMsg(ChatMessage CM);
	void AddPrivateMsgToBufList();

	void AddWorldTrumpetMessage();

	BOOL GetNewChat(){return bNewChat;}
	void SetNewChat(BOOL v){bNewChat = v;}

	void NotifyShowChatErrorMsg(FString errMsg);

public:
	//config

	FString TotalChannelFontColorString;
	FString NearChannelFontColorString;
	FString PrivateChannelFontColorString;
	FString TeamChannelFontColorString;
	FString SystemChannelFontColorString;
	FString GuildChannelFontColorString;
	FString TrumpetChannelFontColorString;
	//FString GMFontColorString;

	FString TvShowColor;
	FString MarqueeColor;
	//marquee count
	INT MarqueeTimes;
	//marquee time
	INT MarqueeSpeed; 

	FString WorldChannelIconPath;
	FString NearChannelIconPath;
	FString PrivateChannelIconPath;
	FString TeamChannelIconPath;
	FString SystemChannelIconPath;
	FString GuildChannelIconPath;
	FString TrumpetChannelIconPath;
	FString GmIconPath;
	FString LabaIconPath;

	FString VipLevel1IconPath;
	FString VipLevel2IconPath;
	FString VipLevel3IconPath;

	FString ChatMyNameColorString;
	FString ChatOtherPlayerColorString;
	FString ChatSkillColorString;
	FString ChatAchievementColorString;

	FString FriendChatSenderColorString;
	FString FriendChatMyContentColorString;
	FString FriendChatOtherPlayerContentColorString;

	INT TrumpetCDTimes;

	BOOL _IsAddChannelTab;

	INT TrumpetTime;
	INT TrumpetTimes;
	BOOL isShowTrumpet;
	BOOL isInTrumpetCDTime;
	INT TrumpetID;

	BOOL bFocus;

	FString TeamSign;
	FString GuildSign;
	FString NearSign;

	INT ChatNearCD;
	BOOL bNewChat;

protected:

	TArray<ChatMessage> ChatBuffList;
	TArray<ChatMessage> ChatTrumpetList;
	TArray<ChatMessage> ChatPrivateList;
	TArray<ChatFilterChannelMessage> ChatFilterChannelList;
	TArray<ChatFilterChannelMessage> CopyChatFilterChannelList;
	TArray<ChatFilterChannelMessage> DefaultChatFilterChannelList;

	//messages temporarily cached because there is no name...
	TArray<ChatMessage> tempBuffList;

	UBOOL CheckHasQuerryInfo( INT roleId );


};

#endif