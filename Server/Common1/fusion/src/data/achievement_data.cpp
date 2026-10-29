#include "stdafx.h"
#include "achievement_data.h"
#include "achievement_def.h"
#include "npc_data.h"
#include "item_data.h"
#include "role_def.h"
#include "data_manager.h"
#include "variable_def.h"

BOOL ACHIEVEMENT_DATA::ReadLine(INT32& rnID, const char*& rpcszName, ACHIEVEMENT_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

	READ_INT("ID", nAchievementID);
	READ_STR("Name", szName);
	READ_INT("PreAchievementID1", nPreAchievementID1);
	READ_INT("PreAchievementID2", nPreAchievementID2);	

	READ_INT("AchievementPts", nAchievementPts);

	// Achievement completion conditions
	for (int nIndex = 0; nIndex < MAX_CON_PER_ACHIEVEMENT; nIndex++)
	{
		char szAchievement[COMMON_NAME_LEN];

		snprintf(szAchievement, COMMON_NAME_LEN, "ConditionType%d",nIndex);
		READ_INT_EXPR(szAchievement, vecAchievementType[nIndex]);

		snprintf(szAchievement, COMMON_NAME_LEN, "ConditionValue%d1",nIndex);
		READ_INT_EXPR(szAchievement, vecAchievementValue1[nIndex]);

		snprintf(szAchievement, COMMON_NAME_LEN, "ConditionValue%d2",nIndex);
		READ_INT_EXPR(szAchievement, vecAchievementValue2[nIndex]);

		snprintf(szAchievement, COMMON_NAME_LEN, "ConditionValue%d3",nIndex);
		READ_INT_EXPR(szAchievement, vecAchievementValue3[nIndex]);

	}

	// Achievement completion rewards
	pData->nAwardSlotCount = 0;
	for (int nIndex = 0; nIndex < MAX_REWARD_PER_ACHIEVEMENT; nIndex++)
	{	
		char szAchievement[COMMON_NAME_LEN];

		snprintf(szAchievement, COMMON_NAME_LEN, "RewardID%d",nIndex);
		READ_INT_EXPR(szAchievement, nRewardID[nIndex]);

		snprintf(szAchievement, COMMON_NAME_LEN, "RewardType%d",nIndex);
		READ_INT_EXPR(szAchievement, nRewardType[nIndex]);

		if (pData->nRewardType[nIndex] == adtItem)
			pData->nAwardSlotCount++;
	}

	READ_END(nAchievementID);
}

BOOL HANDBOOK_DATA::ReadLine(INT32& rnID, const char*& rpcszName, HANDBOOK_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
	INT32 i = 0;
	char* p = pData->szValue;
	READ_START();

	memset(pData->szValue, 0, sizeof(pData->szValue));

	READ_INT("HandbookID", nHandbookID);
	READ_INT("HandbookIndex", nIndex);
	READ_INT_EXPR("HandbookType", nHandbookType);

	READ_STR("ID", szValue);

	if (p[strlen(pData->szValue)] != ',')
		p[strlen(pData->szValue)] = ',';

	while(1)
	{
		char* tok = SplitStr(p, ',');
		if(tok == NULL)
			break;

		pData->nID[i++] = atoi(tok);

		LOG_PROCESS_ERROR(i < MAX_HANDBOOKITEM_COUNT);
	}

//	READ_INT("ID", nID);

	READ_END(nHandbookID);
}

CAchievementIndexData CAchievementIndexData::ms_Instance;

CAchievementIndexData::CAchievementIndexData()
{

}

CAchievementIndexData::~CAchievementIndexData()
{
	UnInit();
}

BOOL CAchievementIndexData::Init(void)
{
	INT32 nDataCount = 0;
	INT32 nHandledCount = 0;
	const ACHIEVEMENT_DATA* pData = NULL;
	UINT16 wIdCount = 0;
	INT32 nCondType = 0;
	UINT16(*pawAchievementId)[ACHIEVEMENT_MAX] = NULL;

	memset(awDefAchievementId, 0, sizeof(awDefAchievementId));
	memset(awVarAchievementId, 0, sizeof(awVarAchievementId));
	memset(awRoleVarAchievementId, 0, sizeof(awRoleVarAchievementId));

	nDataCount = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetDataCount();
	LOG_PROCESS_ERROR(nDataCount > 0);

	pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetFirstData();
	LOG_PROCESS_ERROR(NULL != pData);

	for (nHandledCount = 0;; ++nHandledCount)
	{
		if (NULL == pData)
		{
			break;
		}

		for (INT32 nIndex = 0; nIndex < MAX_CON_PER_ACHIEVEMENT; nIndex++)
		{
			nCondType = pData->vecAchievementType[nIndex];

			LOG_PROCESS_ERROR(nCondType >= adInvalid && nCondType <= adTotal);

			//Filter invalid condition types
			if (adInvalid == nCondType){ continue; }
			if (adTotal == nCondType){ continue; }

			if (adVariable == nCondType)
			{
				LOG_PROCESS_ERROR(pData->vecAchievementValue1[nIndex] > avdInvalid);
				LOG_PROCESS_ERROR(pData->vecAchievementValue1[nIndex] < avdTotal);

				pawAchievementId = awVarAchievementId;
				nCondType = pData->vecAchievementValue1[nIndex];
			}
			else if (adRoleVariable == nCondType)
			{
				LOG_PROCESS_ERROR(pData->vecAchievementValue1[nIndex] > 0);
				LOG_PROCESS_ERROR(pData->vecAchievementValue1[nIndex] < MAX_ROLE_VARIABLE_COUNT);

				pawAchievementId = awRoleVarAchievementId;
				nCondType = pData->vecAchievementValue1[nIndex];
			}
			else
			{
				pawAchievementId = awDefAchievementId;
			}

			wIdCount = pawAchievementId[nCondType][0];
			pawAchievementId[nCondType][wIdCount + 1] = pData->nAchievementID;
			++pawAchievementId[nCondType][0];
			
		}

		pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetNextData();
	}

	LOG_PROCESS_ERROR(nHandledCount == nDataCount);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL CAchievementIndexData::UnInit(void)
{
	return TRUE;
}

CAchievementIndexData& CAchievementIndexData::GetInstance(void)
{
	return ms_Instance;
}

UINT16* CAchievementIndexData::GetAchievementIds(INT16 nCondType, INT16 nSubType)
{
	UINT16* pAchievementId = NULL;

	LOG_PROCESS_ERROR(nCondType > adInvalid && nCondType < adTotal);

	if (adVariable == nCondType)
	{
		LOG_PROCESS_ERROR(nSubType > avdInvalid && nSubType < avdTotal);

		pAchievementId = awVarAchievementId[nSubType];
	}
	else if (adRoleVariable == nCondType)
	{
		LOG_PROCESS_ERROR(nSubType > 0 && nSubType < MAX_ROLE_VARIABLE_COUNT);

		pAchievementId = awRoleVarAchievementId[nSubType];
	}
	else
	{
		pAchievementId = awDefAchievementId[nCondType];
	}

	return pAchievementId;

Exit0:

	return NULL;
}
