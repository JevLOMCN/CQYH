#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOKillRecordManager.h"
#include "MMOFacade.h"

//=======================================================================================
void MMOKillRecordManager::Init()
{
	nKillMonsterRecord = 0;
	nKillMonsterMomentaryRecord = 0;
	nKillPlayerCount = 0;
	nKillMonsterCount = 0;
}
//=======================================================================================
void MMOKillRecordManager::Shutdown()
{

}
void MMOKillRecordManager::NotifySynKillRecordData(void* pkData)
{
	nKillMonsterCount = 0;
	nKillPlayerCount  = 0;
}
void MMOKillRecordManager::NotifyNewKillMonster(INT nCount)
{
	nKillMonsterCount = nCount;

	//trigger combo-kill state
	if(nKillMonsterCount >= CONST_KILL_MINCOUNT)
	{
		//max combo-kill limit
		if(nKillMonsterCount > CONST_KILL_MAXCOUNT)
			nKillMonsterCount = CONST_KILL_MAXCOUNT;

		MMOGameEvent_KillRecord evt( MMOGameEvent_KillRecord::EKillRecordEvent_KillMonsterCount);
		evt.nCount = nKillMonsterCount;
		MMOUI::PostGameEvent(&evt);
	}
}
void MMOKillRecordManager::NotifyNewKillPlayer(INT nCount)
{
	nKillPlayerCount = nCount;

	if(nKillPlayerCount >= CONST_KILLPALYERMIN)
	{
		//max combo-kill limit
		if(nKillPlayerCount > CONST_KILL_MAXCOUNT)
			nKillPlayerCount = CONST_KILL_MAXCOUNT;

		MMOGameEvent_KillRecord evt( MMOGameEvent_KillRecord::EKillRecordEvent_KillPLayerCount);

		evt.nCount = nKillPlayerCount;
		evt.bNew   = (nKillPlayerCount >= nKillPlayerRecord);
		MMOUI::PostGameEvent(&evt);

		if(nKillPlayerCount >= nKillPlayerRecord)
			nKillPlayerRecord = nKillPlayerCount;
	}
}
void MMOKillRecordManager::NotifyKillMonsterReward(INT nCount,INT nExp)
{
	MMOGameEvent_KillRecord evt( MMOGameEvent_KillRecord::EKillRecordEvent_KillMonsterReward);
	evt.nCount = nCount;
	evt.nExp = nExp;
	evt.bNew = (nKillMonsterRecord <= nKillMonsterCount);
	MMOUI::PostGameEvent(&evt);

	if(nKillMonsterCount >= nKillMonsterRecord)
		nKillMonsterRecord = nKillMonsterCount;
	nKillMonsterCount = 0;
}

void MMOKillRecordManager::NotifyKillMonsterMomentaryReward(INT nCount,INT nExp)
{
	nKillMonsterMomentaryCount = nCount;
	MMOGameEvent_KillRecord evt( MMOGameEvent_KillRecord::EKillRecordEvent_KillMonsterRewardMemotary);
	evt.nCount = nCount;
	evt.nExp = nExp;
	evt.bNew = (nKillMonsterMomentaryRecord <= nKillMonsterMomentaryCount);
	MMOUI::PostGameEvent(&evt);

	if(nKillMonsterMomentaryCount >= nKillMonsterMomentaryRecord)
		nKillMonsterMomentaryRecord = nKillMonsterMomentaryCount;
	nKillMonsterMomentaryCount  = 0;
}

FString MMOKillRecordManager::GetKillPlayerCountTitle(INT nCount)
{
	static FString strArr[7] = {TEXT("Killing Spree"),TEXT("Rampage"),TEXT("Unstoppable"),TEXT("Insane Slaughter"),
									TEXT("Monstrous"),TEXT("Godlike"),TEXT("Beyond Godlike")};
	if(nCount < 3)
		nCount = 3;
	if(nCount > 9)
		nCount = 9;
	return strArr[nCount - 3];
}
void MMOKillRecordManager::NotifyKillPlayerRecord(INT nRecord)
{
	nKillPlayerRecord = nRecord;
}
void MMOKillRecordManager::NotifyKillMonsterRecord(INT nRecord)
{
	nKillMonsterRecord = nRecord;
}
void MMOKillRecordManager::NotifyKillMonsterMomentaryRecord(INT nRecord)
{
	nKillMonsterMomentaryRecord = nRecord;
}