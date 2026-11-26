#ifndef _MMO_KILLRECORDMANAGER_H_
#define _MMO_KILLRECORDMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOGameClasses.h"


struct MMOGameEvent_KillRecord : public MMOGameEvent
{
public:
	enum EKillRecordEventType
	{
		EKillRecordEvent_KillMonsterCount,
		EKillRecordEvent_KillMonsterReward,
		EKillRecordEvent_KillMonsterRewardMemotary,
		EKillRecordEvent_KillPLayerCount,
	};

	MMOGameEvent_KillRecord(EKillRecordEventType eventType)
		: EventType(eventType), nCount(0), nExp(0), bNew(false)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_KillRecord;
	}

	EKillRecordEventType	EventType;
	INT						nCount;
	INT						nExp;
	bool					bNew;
};


class MMOKillRecordManager: public SingletonMeyers<MMOKillRecordManager>
{
	enum CONST_ENUM_KILLMANAGER
	{
		CONST_KILLPALYERMIN = 3,
		CONST_KILL_TIMESPAN = 4,
		CONST_KILL_MINCOUNT = 5,
		CONST_KILL_MAXCOUNT = 999,
	};
public:
	MMOKillRecordManager()
		: nKillPlayerCount(0), nKillPlayerRecord(0), nKillMonsterCount(0),
		  nKillMonsterRecord(0), nKillMonsterMomentaryCount(0), nKillMonsterMomentaryRecord(0) {}

	void Init();
	void Shutdown();

	FString GetKillPlayerCountTitle(INT nCount);
	void NotifySynKillRecordData(void* pkData);
	void NotifyNewKillMonster(INT nCount);
	void NotifyNewKillPlayer(INT nCount);

	void NotifyKillPlayerRecord(INT nRecord);
	void NotifyKillMonsterRecord(INT nRecord);
	void NotifyKillMonsterMomentaryRecord(INT nRecord);

	void NotifyKillMonsterReward(INT nCount,INT nExp);
	void NotifyKillMonsterMomentaryReward(INT nCount,INT nExp);

private:
	INT nKillPlayerCount;
	INT nKillPlayerRecord;

	INT nKillMonsterCount;
	INT nKillMonsterRecord;

	INT nKillMonsterMomentaryCount;
	INT nKillMonsterMomentaryRecord;
};

#endif