#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <algorithm>

// type of message displayed
enum EScreenMessageType
{
	ESMT_Invalid,
	ESMT_SkillLearned,
	ESMT_WeaponSkillGained,
	ESMT_SkillSlotUnlock,
	ESMT_MainSkillLevelUp,
	ESMT_Achievement,
	ESMT_FBAchievement,
	ESMT_FBUnlock
};

//// type of message displayed
//enum EScreenMessageDisplayType
//{
//	ESMDT_Skill,
//	ESMDT_Achievement,
//	ESMDT_FBAchievement,
//	ESMDT_FBUnlock
//};

// which screen area it is displayed in; messages in the same area are queued
enum EScreenMsgChannel
{
	ESMC_TopCenter,
	ESMC_CenterCenter,
	ESMC_BottomCenter
};

class MessageData
{
public:
	MessageData()
		: DelayTime(0.0f)
		, MessageType(ESMT_Invalid)
		, MagicValue(-1)
	{
	}

	EScreenMessageType MessageType;
	FString MessageText;
	INT MagicValue;
	float DelayTime;

	virtual void Destroy()
	{
		delete this;
	}
};

class IMessageUI
{
public:

	virtual EScreenMessageType GetType() const = 0;

	// returns the time needed to display this message, generally determined by the UI effect
	virtual float DisplayMessage(MessageData* msgData) = 0;
};

class ScreenMessageQueue: public INativeGameEventHandler
{
public:

	ScreenMessageQueue()
		: _enable(false)
	{
	}

	static ScreenMessageQueue& getSingleton();

	void Init();
	void Shutdown();
	void OnNativeGameEvent( struct MMOGameEvent* evt );

	void AddMessage(EScreenMsgChannel channel, MessageData* msgData);

	void Update(float deltaTime);

	void Clear();
	void ClearCurrent( EScreenMsgChannel channel );

	void Enable(bool enab);


	IMessageUI* GetMessageUI(EScreenMessageType msgType);
	void NotifyEndPlayMsg();

	void RegisterMessageUI(IMessageUI* ui);

protected:

	typedef std::queue<MessageData*> MessageDataList;
	typedef std::map<EScreenMsgChannel, MessageDataList> MessageChannelMap;
	MessageChannelMap _messages;
	bool _enable;

	typedef std::map<EScreenMessageType, IMessageUI*> MessageUIMap;
	MessageUIMap _messageUIMap;
};

//skill learned
class MessageUISkillLearned : public IMessageUI
{
public:

	virtual EScreenMessageType GetType() const
	{
		return ESMT_SkillLearned;
	}

	virtual float DisplayMessage(MessageData* msgData);
};

//
class MessageUIWeaponSkillGained : public IMessageUI
{
public:

	virtual EScreenMessageType GetType() const
	{
		return ESMT_WeaponSkillGained;
	}

	virtual float DisplayMessage(MessageData* msgData);
};
//skill slot unlock
class MessageUISkillSlotUnlock : public IMessageUI
{
public:

	virtual EScreenMessageType GetType() const
	{
		return ESMT_SkillSlotUnlock;
	}

	virtual float DisplayMessage(MessageData* msgData);
};

//skill learned
class MessageUIMainSkillLevelUp : public IMessageUI
{
public:

	virtual EScreenMessageType GetType() const
	{
		return ESMT_MainSkillLevelUp;
	}

	virtual float DisplayMessage(MessageData* msgData);
};

//Achievement
class MessageUIAchievement : public IMessageUI
{
public:

	virtual EScreenMessageType GetType() const
	{
		return ESMT_Achievement;
	}

	virtual float DisplayMessage(MessageData* msgData);
};

//Achievement
class MessageUIFBAchievement : public IMessageUI
{
public:

	virtual EScreenMessageType GetType() const
	{
		return ESMT_FBAchievement;
	}

	virtual float DisplayMessage(MessageData* msgData);
};

//FBUnlock
class MessageUIFBUnlock : public IMessageUI
{
public:

	virtual EScreenMessageType GetType() const
	{
		return ESMT_FBUnlock;
	}

	virtual float DisplayMessage(MessageData* msgData);
};