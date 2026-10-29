#include "stdafx.h"
#include "kill_npc_quest.h"

#include "error_def.h"
#include "kill_npc_quest_data.h"
#include "npc_data.h"
#include "role.h"
#include "npc.h"
#include "message_handler.h"

CKillNpcQuest::CKillNpcQuest()
{
	m_poOwner = NULL;
	memset(m_wKillNpcCount, 0, sizeof(m_wKillNpcCount));
	memset(m_byCompleteList, 0, sizeof(m_byCompleteList));
}

CKillNpcQuest::~CKillNpcQuest()
{
	UnInit();
}

BOOL CKillNpcQuest::Init(CRole* poOwner)
{
	LOG_PROCESS_ERROR(poOwner);

	memset(m_wKillNpcCount, 0, sizeof(m_wKillNpcCount));
	memset(m_byCompleteList, 0, sizeof(m_byCompleteList));
	m_poOwner = poOwner;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CKillNpcQuest::UnInit(void)
{

	return TRUE;
}

BOOL CKillNpcQuest::ResetDailyQuest(void)
{
	const KILL_NPC_QUEST_DATA* pcQuestData = NULL;

	pcQuestData = CDataManager<KILL_NPC_QUEST_DATA>::GetInstance().GetFirstData();
	while (pcQuestData)
	{
		if (pcQuestData->eResetType == qttDay)
		{
			m_wKillNpcCount[pcQuestData->nQuestID] = 0;
			m_byCompleteList[pcQuestData->nQuestID] = 0;

#ifndef _ROBOT
			CCliMessageHandler::GetInstance().DoResetKillNpcCount(m_poOwner, pcQuestData->nQuestID);
#endif //_ROBOT
		}

		pcQuestData = CDataManager<KILL_NPC_QUEST_DATA>::GetInstance().GetNextData();
	}

	return TRUE;
Exit0:

	return FALSE;
}

BOOL CKillNpcQuest::ResetWeeklyQuest(void)
{
	const KILL_NPC_QUEST_DATA* pcQuestData = NULL;

	pcQuestData = CDataManager<KILL_NPC_QUEST_DATA>::GetInstance().GetFirstData();
	while (pcQuestData)
	{
		if (pcQuestData->eResetType == qttWeek)
		{
			m_wKillNpcCount[pcQuestData->nQuestID] = 0;
			m_byCompleteList[pcQuestData->nQuestID] = 0;

#ifndef _ROBOT
			CCliMessageHandler::GetInstance().DoResetKillNpcCount(m_poOwner, pcQuestData->nQuestID);
#endif //_ROBOT
		}

		pcQuestData = CDataManager<KILL_NPC_QUEST_DATA>::GetInstance().GetNextData();
	}

	return TRUE;
Exit0:

	return FALSE;
}

INT32 CKillNpcQuest::CanComplete(INT32 nQuestID, INT32 nIndex)
{
	INT32 nResult = errUnknownError;
	const KILL_NPC_QUEST_DATA* pcQuestData = NULL;

	LOG_PROCESS_ERROR(nQuestID >= 0 && nQuestID < MAX_KILL_NPC_QUEST_COUNT);
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_KILL_COUNT_PER_NPC);
	LOG_PROCESS_ERROR((m_byCompleteList[nQuestID] & (1 << nIndex)) == 0);

	pcQuestData = CDataManager<KILL_NPC_QUEST_DATA>::GetInstance().GetData(nQuestID);
	LOG_PROCESS_ERROR(pcQuestData);

	PROCESS_ERROR_RET_CODE(m_wKillNpcCount[nQuestID] >= pcQuestData->nKillCount[nIndex], errQuestNotEnoughKill);

	nResult = errSuccess;
Exit0:
	return nResult;
}

INT32 CKillNpcQuest::Complete(INT32 nQuestID, INT32 nIndex)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	const KILL_NPC_QUEST_DATA* pcQuestData = NULL;

	LOG_PROCESS_ERROR(nQuestID >= 0 && nQuestID < MAX_KILL_NPC_QUEST_COUNT);
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_KILL_COUNT_PER_NPC);

	m_byCompleteList[nQuestID] |= (1 << nIndex);

	pcQuestData = CDataManager<KILL_NPC_QUEST_DATA>::GetInstance().GetData(nQuestID);
	LOG_PROCESS_ERROR(pcQuestData);

	// Keep dispatching both reward fields; their handlers validate zero amounts.
	m_poOwner->AddExp(pcQuestData->nRewardExp[nIndex], FALSE, TRUE);
	nRetCode = m_poOwner->GetItemBag()->AddMoney(MONEYTYPE_ACHIEVEMENTPTS,
		pcQuestData->nAchievementPts[nIndex], MONEYLOG_ACHIEVEMENT_REWARD, 0);
	LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoCompleteKillNpcQuest(m_poOwner, nQuestID, nIndex);
	LOG_CHECK_ERROR(nRetCode);
#endif //_ROBOT

	nResult = errSuccess;
Exit0:
	return nResult;
}

void CKillNpcQuest::OnKillNpc(CNpc* poNpc)
{
	INT32 nRetCode = 0;
	INT32 nQuestID;

	LOG_PROCESS_ERROR(poNpc);
	LOG_PROCESS_ERROR(poNpc->GetNpcData());

	nQuestID = poNpc->GetNpcData()->nKillNpcQuestID;
	PROCESS_ERROR(nQuestID >= 0 && nQuestID < MAX_KILL_NPC_QUEST_COUNT);
	PROCESS_ERROR(m_wKillNpcCount[nQuestID] < USHRT_MAX);

	m_wKillNpcCount[nQuestID]++;

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoSyncKillNpcCount(m_poOwner, nQuestID, m_wKillNpcCount[nQuestID]);
	LOG_CHECK_ERROR(nRetCode);
#endif //_ROBOT

Exit0:
	return;
}
