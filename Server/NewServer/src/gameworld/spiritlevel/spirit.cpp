#include "stdafx.h"
#include "spirit_def.h"
#include "spirit.h"
#include "item_bag.h"
#include "spirit_data.h"
#include "game_world.h"
#include "message_handler.h"
#include "modifier_tree.h"
#include "char_modifier.h"
#include "chat_def.h"
#include "statistic.h"

CSpirit::CSpirit()
{
	m_nLevel = 1;
	m_nExp = 0;

	m_dwNextExp2PtsTime = 0;
	m_nTotalMoneyCost = 0;

	m_dwDailyUseMoneyChangeCount = 0;
	m_dwNextScanTimer = 0;			

	m_nBloodState = 0;

	m_nBagContainerSize = INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtBag];
	m_nStallContainerSize = INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtStall];
	m_nEquipContainerSize = INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtEquip];

	m_nProductAirLevel = 1;		

	memset(m_BagContainer, 0, sizeof(m_BagContainer));
	memset(m_StallContainer, 0, sizeof(m_StallContainer));
	memset(m_EquipContainer, 0, sizeof(m_EquipContainer));

	memset(m_nBloodSlot, 0, sizeof(m_nBloodSlot));

	m_pOwner = NULL;
}

CSpirit::~CSpirit()
{

}

INT32 CSpirit::Init(CRole* pOwner)
{
	LOG_PROCESS_ERROR(pOwner);

	m_nLevel = 1;
	m_nExp = 0;

	m_dwNextExp2PtsTime = 0;
	m_nTotalMoneyCost = 0;

	m_dwDailyUseMoneyChangeCount = 0;
	m_dwNextScanTimer = 0;			

	m_nBloodState = 0;

	m_nBagContainerSize = INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtBag];
	m_nStallContainerSize = INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtStall];
	m_nEquipContainerSize = INIT_SPIRIIT_AIR_SEA_BAG_SIZE[sasbtEquip];

	m_nProductAirLevel = 1;		

	memset(m_BagContainer, 0, sizeof(m_BagContainer));
	memset(m_StallContainer, 0, sizeof(m_StallContainer));
	memset(m_EquipContainer, 0, sizeof(m_EquipContainer));

	memset(m_nBloodSlot, 0, sizeof(m_nBloodSlot));

	m_pOwner = pOwner;

	return TRUE;
Exit0:
	return FALSE;
}
   
INT32 CSpirit::UnInit()
{
	m_pOwner = NULL;
	return TRUE;
}

INT32 CSpirit::MainLoop()
{
	INT32 nRetCode = 0;
	
	PROCESS_SUCCESS((m_dwNextScanTimer/86400) >= (CGameWorld::GetInstance().GetGameTime()/86400));

	m_dwNextScanTimer = CGameWorld::GetInstance().GetGameTime() + 86400;

	m_dwDailyUseMoneyChangeCount = 0;
	m_nTotalMoneyCost = 0;

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoSyncSpirit(m_pOwner);
	LOG_PROCESS_ERROR(nRetCode);
#endif

	return TRUE;

Exit0:
	return FALSE;

Exit1:
	return TRUE;
}

INT32 CSpirit::GetPow()
{
	const SPIRIT_DATA* pSpiritData = nullptr;
	LOG_PROCESS_ERROR(m_pOwner);
	PROCESS_SUCCESS(m_pOwner->GetLevel() < SPIRIT_OPEN_LEVEL);

	pSpiritData = CDataManager<SPIRIT_DATA>::GetInstance().GetData(m_pOwner->GetSpirit().GetLevel());
	LOG_PROCESS_ERROR(pSpiritData);

	

	return pSpiritData->nPower;

Exit1:
	return 0;

Exit0:
	return 0;
}

INT32 CSpirit::SetLevel(INT32 nLevel)
{
	INT32 nRetCode = 0;
	const SPIRIT_DATA* pSpiritData = NULL;

	pSpiritData = CDataManager<SPIRIT_DATA>::GetInstance().GetData(m_nLevel);
	LOG_PROCESS_ERROR(pSpiritData);

	m_nLevel = nLevel;
	m_nExp = 0;

#ifndef _ROBOT

	nRetCode = CCliMessageHandler::GetInstance().DoSpiritUp(m_pOwner, m_nLevel);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoSpiritExpChange(m_pOwner, m_nExp);
	LOG_PROCESS_ERROR(nRetCode);
#endif //_ROBOT

	return TRUE;
Exit0:
	return FALSE;
}

/****************** air sea *************************/

INT32 CSpirit::ChangeSoul2Pts(BOOL bUseMoney, BOOL bIsOnline)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	INT32 nDisCount = 100;
	UINT32 dwCurrentTime = 0;

	const SPIRIT_DATA* pSpiritData = NULL;
	const SPIRIT_CHANGE_DATA* pSpiritChangeData = NULL;

	LOG_PROCESS_ERROR(m_pOwner);

	pSpiritData = CDataManager<SPIRIT_DATA>::GetInstance().GetData(m_nLevel);
	LOG_PROCESS_ERROR(pSpiritData);

	dwCurrentTime = CGameWorld::GetInstance().GetGameTime();

#ifndef _ROBOT
	LOG_PROCESS_ERROR_RET_CODE((m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_SOUL_PTS) >= pSpiritData->nSoulCost), errSpiritMoneyNotEnough); 
	LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetLevel() >= SPIRIT_OPEN_LEVEL, errSpiritNotOpen);

	if (bUseMoney)																														// Convert immediately
	{
		INT32 nMinuteTime = 0;
		INT32 nMoneyCost = 0;
		INT32 nDailyTimeAddtional = 0;

		LOG_PROCESS_ERROR_RET_CODE(m_dwDailyUseMoneyChangeCount < pSpiritData->nDailyExp2PtsMaxCount, errSpiritExp2PtsMaxCount);

		// Daily attempt bonus
		for (INT32 nIndex = 1; nIndex <= MAX_EXP2PTS_LEVEL; nIndex++)
		{
			pSpiritChangeData = CDataManager<SPIRIT_CHANGE_DATA>::GetInstance().GetData(nIndex);

			if(!pSpiritChangeData)
			{
				break;
			}
			else if (pSpiritChangeData->nDailyChangeTime <= m_dwDailyUseMoneyChangeCount)
			{
				nDailyTimeAddtional = pSpiritChangeData->nMoneyCostDiscount;
			}
		}

		if (m_dwNextExp2PtsTime > dwCurrentTime )
		{
			nMinuteTime = (m_dwNextExp2PtsTime - dwCurrentTime)%60 ? ((m_dwNextExp2PtsTime - dwCurrentTime)/60 + 1) : (m_dwNextExp2PtsTime - dwCurrentTime )/60;

			// Calculate cost based on spirit level and today’s attempt bonus
			if (nMinuteTime > 0)
			{
				nMoneyCost = (INT64)nMinuteTime * pSpiritData->nMoneyCostOneShot * (10000 + nDailyTimeAddtional) / 10000;
			}
		}

		LOG_PROCESS_ERROR_RET_CODE((m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_02) >= nMoneyCost), errSpiritExp2PtsMoneyOut);
		LOG_PROCESS_ERROR_RET_CODE((m_nTotalMoneyCost + pSpiritData->nMoneyCostOneShot) <= pSpiritData->nDailyExp2PtsMoneyCost, errSpiritDailyExp2PtsOut);

		nResult = m_pOwner->GetItemBag()->SubMoney(MONEYTYPE_02,  nMoneyCost, MONEYLOG_SPIRIT_EXP2PTS, 0);
		LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

		m_nTotalMoneyCost += nMoneyCost;
		m_dwDailyUseMoneyChangeCount++;

		if (m_pOwner->GetVIPLevel() > vlLevel0)
		{
			LOG_PROCESS_ERROR(m_pOwner->GetVIPLevel() < vlTotal);

			nDisCount = pSpiritData->nVipOnlineChangeDiscount[m_pOwner->GetVIPLevel() - vlLevel1] + 100;
		}
	}
	else if (!bIsOnline)																												// Offline conversion
	{	
		INT32 nDuration = 0;
		
		LOG_PROCESS_ERROR(m_pOwner->GetAchievementList().GetVariable(avdLastOfflineTime) > 0);
		LOG_PROCESS_ERROR(pSpiritData->dwDailyChangeCDTime > 0);
		LOG_PROCESS_ERROR(m_pOwner->GetVIPLevel() > vlLevel0 && m_pOwner->GetVIPLevel() < vlTotal);

		nDuration = (dwCurrentTime - (UINT32)m_pOwner->GetAchievementList().GetVariable(avdLastOfflineTime)) / pSpiritData->dwDailyChangeCDTime;
		LOG_PROCESS_ERROR_RET_CODE(nDuration > 0 , errSpiritChangeCDTime);

		nDisCount = pSpiritData->nOfflineChangeDiscount[m_pOwner->GetVIPLevel() - vlLevel1] * (nDuration + 1);
	}
	else																																// Online conversion
	{
		if (m_dwNextExp2PtsTime > dwCurrentTime)
		{
			CCliMessageHandler::GetInstance().DoSpiritNotifyExpChangeResult(m_pOwner, errSpiritChangeCDTime, (m_dwNextExp2PtsTime - CGameWorld::GetInstance().GetGameTime()));
			INF("Send Notify Exp Change Result Successful");
		}

		if (m_pOwner->GetVIPLevel() > vlLevel0)
		{
			LOG_PROCESS_ERROR(m_pOwner->GetVIPLevel() < vlTotal);

			nDisCount = pSpiritData->nVipOnlineChangeDiscount[m_pOwner->GetVIPLevel() - vlLevel1] + 100;
		}
	}

	nResult = m_pOwner->GetItemBag()->SubMoney(MONEYTYPE_SOUL_PTS,  pSpiritData->nSoulCost, MONEYLOG_SPIRIT_EXP2PTS, 0);
	LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

	m_nExp +=  pSpiritData->nAddExp * nDisCount / 100;

	nRetCode = CCliMessageHandler::GetInstance().DoSpiritExpChange(m_pOwner, m_nExp);
	LOG_PROCESS_ERROR(nRetCode);

	nResult = m_pOwner->GetItemBag()->AddMoney(MONEYTYPE_SPELL_PTS, pSpiritData->nAddSpiritPts * nDisCount / 100, MONEYLOG_SPIRIT_EXP2PTS, 0);
	LOG_PROCESS_ERROR(nResult == errSuccess);

	if (m_nLevel < MAX_SPIRIT_LEVEL && m_nExp >= pSpiritData->nNextLevelExp)
	{
		m_nExp -=  pSpiritData->nNextLevelExp;
		m_nLevel++;

		nRetCode = CCliMessageHandler::GetInstance().DoSpiritUp(m_pOwner, m_nLevel);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = CCliMessageHandler::GetInstance().DoSpiritExpChange(m_pOwner, m_nExp);
		LOG_PROCESS_ERROR(nRetCode);

		m_pOwner->GetAchievementList().AddVariable(avdSpiritLevelUP, 1);

		// Update equipped attributes after leveling up

		for (INT32 nIndex = 0; nIndex < MAX_SPIRIT_NODE_PER_LEVEL; nIndex++)
		{
			if (pSpiritData->nAttrTemplateNodeID[nIndex])
			{
				nRetCode = CModifierTree::GetInstance().RollBackExtraNode(pSpiritData->nAttrNodeID[nIndex], m_pOwner, NULL);
				LOG_PROCESS_ERROR(nRetCode);
			}
		}
		
		pSpiritData = CDataManager<SPIRIT_DATA>::GetInstance().GetData(m_nLevel);
		LOG_PROCESS_ERROR(pSpiritData);

		for (INT32 nIndex = 0; nIndex < MAX_SPIRIT_NODE_PER_LEVEL; nIndex++)		// Attributes corresponding to each spirit level
		{
			if (pSpiritData->nAttrTemplateNodeID[nIndex])
			{
				nRetCode = CModifierTree::GetInstance().ApplyExtraNode(pSpiritData->nAttrNodeID[nIndex], m_pOwner, NULL);
				LOG_PROCESS_ERROR(nRetCode);
			}
		}

		if (pSpiritData->nBroadcastStringID)						// Screen announcement required
		{
			const char* pcszMessage = NULL;
			char szMessage[256];

			if (pSpiritData->nBroadcastStringID < SERVER_STRING_START_ID)
			{
				snprintf(szMessage, 256, "<#P0:<#PN:%s>><#P1:%d><#T:%d>", m_pOwner->GetName(), pSpiritData->nLevel, pSpiritData->nBroadcastStringID);
				pcszMessage = szMessage;
			}
			else
			{
				pcszMessage = CGameWorld::GetInstance().GetString(pSpiritData->nBroadcastStringID);
				LOG_PROCESS_ERROR(pcszMessage);
			}

			nRetCode = m_pOwner->Say(cctGroup, cltTvShow, pcszMessage);
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

#endif

	m_dwNextExp2PtsTime = dwCurrentTime + pSpiritData->dwDailyChangeCDTime;

	return TRUE;
Exit0:

#ifndef _ROBOT
	m_pOwner->ErrorCodeNotify(nResult, 0, 0);
#endif	// _ROBOT

	return FALSE;
}

/******************* air sea ************************/

INT32 CSpirit::SetProductAirLevel(INT32 nLevel)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(nLevel > 0 && nLevel <= MAX_AIR_PROCEDURE);

	m_nProductAirLevel |= (1 << (nLevel - 1));

#ifndef _ROBOT
	
	nRetCode = CCliMessageHandler::GetInstance().DoSpiritAirLevelUP(m_pOwner, m_nProductAirLevel);
	LOG_PROCESS_ERROR(nRetCode);

#endif // _ROBOT

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CSpirit::ProductAir(INT32 nIndex)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	INT32 nRandNum = 0;
	INT32 nNum = 0;

	const AIR_LEVEL_DATA* pAirLevel = NULL;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(_GetProductAirLevel(nIndex));											// Whether this level is already active
	LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetLevel() >= SPIRIT_OPEN_LEVEL, errSpiritLevel);
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_AIR_PROCEDURE);

	pAirLevel = CDataManager<AIR_LEVEL_DATA>::GetInstance().GetData(MAKE_AIR_LEVEL_KEY((nIndex+1), m_pOwner->GetClass()));
	LOG_PROCESS_ERROR(pAirLevel);
	PROCESS_ERROR_RET_CODE(m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_SPELL_PTS) >= pAirLevel->nPtsCost,  errSpiritPtsNotEnough);

	nRandNum = g_Random(10000);

	if (nIndex < MAX_AIR_PROCEDURE)
	{
		for (INT32 nDropIndex = 0; nDropIndex < MAX_DROP_AIR_COUNT; nDropIndex++)			//　Qi drop
		{
			nNum += pAirLevel->nDropAirRate[nDropIndex];

			if (nRandNum <= nNum)
			{
				const AIR_DATA* pAirData = CDataManager<AIR_DATA>::GetInstance().GetData(pAirLevel->nDropAirID[nDropIndex]);
				LOG_PROCESS_ERROR(pAirData);

				nRetCode = AddAir(pAirData->nAirID, pAirData->nExp);
				PROCESS_ERROR_RET_CODE(nRetCode != INVALID_AIR_INDEX, errSpiritAirSeaBagFull);

				if (pAirData->nHandbookType > htInvalid)
					m_pOwner->GetAchievementList().ActivateHandbook(pAirData->nHandbookType, pAirData->nHandbookIndex);

				if (pAirData->nBroadcastStringID)
				{
					const char* pcszMessage = NULL;
					char szMessage[256];

					if (pAirData->nBroadcastStringID < SERVER_STRING_START_ID)
					{
						snprintf(szMessage, 256, "<#P0:<#PN:%s>><#P1:%s><#T:%d>", m_pOwner->GetName(), pAirData->szAirname, pAirData->nBroadcastStringID);
						pcszMessage = szMessage;
					}
					else
					{
						pcszMessage = CGameWorld::GetInstance().GetString(pAirData->nBroadcastStringID);
						LOG_PROCESS_ERROR(pcszMessage);
					}
#ifndef _ROBOT
					nRetCode = m_pOwner->Say(cctGroup, cltTvShow, pcszMessage);
					LOG_PROCESS_ERROR(nRetCode);
#endif
				}

				break;
			}
		}
		
		if (nIndex > 0)
		{
			_ClearProductAirLevel(nIndex);				// First dim the button for this level
		}

		if (nRandNum <= pAirLevel->nNextLevelRate && m_nLevel >= pAirLevel->nSpiritLevelLimit)	// Advance qi cultivation stage
		{
			SetProductAirLevel(nIndex+1);
		}
	}
	else
	{
		_ClearProductAirLevel(nIndex);				// First dim the button for this level
	}

	if (pAirLevel->nDropMoneyCount != 0)			// Arcane qi
	{
		nResult = m_pOwner->GetItemBag()->AddMoney(MONEYTYPE_AIR, pAirLevel->nDropMoneyCount, MONEYLOG_SPIRIT_ADDAIR, 0);
		PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);
	}

	nResult = m_pOwner->GetItemBag()->SubMoney(MONEYTYPE_SPELL_PTS, pAirLevel->nPtsCost, MONEYLOG_SPIRIT_ADDAIR, 0);
	LOG_PROCESS_ERROR(nResult == errSuccess);

	if (pAirLevel->nBroadcastStringID)
	{
		const char* pcszMessage = NULL;
		char szMessage[256];

		if (pAirLevel->nBroadcastStringID < SERVER_STRING_START_ID)
		{
			snprintf(szMessage, 256, "<#P0:<#PN:%s>><#P1:%d><#T:%d>", m_pOwner->GetName(), pAirLevel->nLevel, pAirLevel->nBroadcastStringID);
			pcszMessage = szMessage;
		}
		else
		{
			pcszMessage = CGameWorld::GetInstance().GetString(pAirLevel->nBroadcastStringID);
			LOG_PROCESS_ERROR(pcszMessage);
		}
#ifndef _ROBOT
		nRetCode = m_pOwner->Say(cctGroup, cltTvShow, pcszMessage);
		LOG_PROCESS_ERROR(nRetCode);
#endif
	}

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoSpiritAirLevelUP(m_pOwner, m_nProductAirLevel);
	LOG_PROCESS_ERROR(nRetCode);
#endif

	return errSuccess;
Exit0:
	return nResult;
}

INT32 CSpirit::MergeAir(INT32 nSrcCon)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	const AIR_DATA* pSrcAir = NULL;

	LOG_PROCESS_ERROR(nSrcCon >= 0 && nSrcCon < MAX_AIRSEA_BAG_CONTAINER_SIZE);

	pSrcAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_BagContainer[nSrcCon].nTemplateID);
	LOG_PROCESS_ERROR(pSrcAir);

	for (INT32 nIndex = 0; nIndex < m_nBagContainerSize; nIndex++)
	{
		if (nIndex != nSrcCon && nIndex < MAX_AIRSEA_BAG_CONTAINER_SIZE)
		{
			const AIR_DATA* pTarAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_BagContainer[nIndex].nTemplateID);

			if (pTarAir && pTarAir->nAirID)
			{
				nRetCode = _MergeAir(nSrcCon, nIndex);
				LOG_PROCESS_ERROR_RET_CODE(nRetCode = errSuccess, nRetCode);
			}
		}

	}

	return errSuccess;

Exit0:
	return nResult;
}

INT32 CSpirit::MoveAir(INT32 nSrcType, INT32 nSrcIndex, INT32 nTarType, INT32 nTarIndex)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	INT32 nTargetAirID	= 0;
	INT32 nTargetAirExp	= 0;

	INT32 nSrcAirID	= 0;
	INT32 nSrcAirExp	= 0;

	const AIR_DATA* pAir = NULL;

	switch(nSrcType)
	{
	case sasbtStall:
		{
			LOG_PROCESS_ERROR(nSrcIndex >= 0 && nSrcIndex <= m_nStallContainerSize);
			LOG_PROCESS_ERROR_RET_CODE(m_StallContainer[nSrcIndex].nTemplateID, errSpiritAirSlotEmpty);

			pAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_StallContainer[nSrcIndex].nTemplateID);
			LOG_PROCESS_ERROR(pAir);
		}
		break;
	case sasbtBag:
		{
			LOG_PROCESS_ERROR(nSrcIndex >= 0 && nSrcIndex <= m_nBagContainerSize);
			LOG_PROCESS_ERROR_RET_CODE(m_BagContainer[nSrcIndex].nTemplateID, errSpiritAirSlotEmpty);

			pAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_BagContainer[nSrcIndex].nTemplateID);
			LOG_PROCESS_ERROR(pAir);
		}
		break;
	case sasbtEquip:
		{
			LOG_PROCESS_ERROR(nSrcIndex >= 0 && nSrcIndex <= m_nEquipContainerSize);
			LOG_PROCESS_ERROR_RET_CODE(m_EquipContainer[nSrcIndex].nTemplateID, errSpiritAirSlotEmpty);

			pAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_EquipContainer[nSrcIndex].nTemplateID);
			LOG_PROCESS_ERROR(pAir);
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	switch(nTarType)
	{
	case sasbtStall:
		{
			LOG_PROCESS_ERROR(nTarIndex >= 0 && nTarIndex < m_nStallContainerSize);
		}
		break;
	case sasbtBag:
		{
			LOG_PROCESS_ERROR(nTarIndex >= 0 && nTarIndex < m_nBagContainerSize);
		}
		break;
	case sasbtEquip:
		{
			LOG_PROCESS_ERROR(nTarIndex >= 0 && nTarIndex < m_nEquipContainerSize);

			for (INT32 nIndex = 0; nIndex < m_nEquipContainerSize; nIndex++)
			{
				if (m_EquipContainer[nIndex].nTemplateID != 0)
				{
					const AIR_DATA* pEquippedAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_EquipContainer[nIndex].nTemplateID);
					LOG_PROCESS_ERROR(pEquippedAir);
					PROCESS_ERROR_RET_CODE(pEquippedAir->nGroupID != pAir->nGroupID, errSpiritAirEquipSameGroup);
				}
			}
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	switch(nSrcType)
	{
	case sasbtStall:
		{
			nTargetAirID = m_StallContainer[nSrcIndex].nTemplateID;
			nTargetAirExp = m_StallContainer[nSrcIndex].nExp;

			m_StallContainer[nSrcIndex].nTemplateID = 0;
			m_StallContainer[nSrcIndex].nExp = 0;
		}
		break;
	case sasbtBag:
		{
			nTargetAirID = m_BagContainer[nSrcIndex].nTemplateID;
			nTargetAirExp = m_BagContainer[nSrcIndex].nExp;

			m_BagContainer[nSrcIndex].nTemplateID = 0;
			m_BagContainer[nSrcIndex].nExp = 0;
		}
		break;
	case sasbtEquip:
		{
			pAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_EquipContainer[nSrcIndex].nTemplateID);	// Remove equipment attributes
			LOG_PROCESS_ERROR(pAir);

			nTargetAirID = m_EquipContainer[nSrcIndex].nTemplateID;
			nTargetAirExp = m_EquipContainer[nSrcIndex].nExp;

			for (INT32 nIndex = 0; nIndex < MAX_ATTR_PER_NODE; nIndex++)
			{
				if (pAir->nNodeID[nIndex])
				{
					nRetCode = CModifierTree::GetInstance().RollBackExtraNode(pAir->nNodeID[nIndex], m_pOwner, NULL);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}

			m_EquipContainer[nSrcIndex].nTemplateID = 0;
			m_EquipContainer[nSrcIndex].nExp = 0;
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	switch(nTarType)
	{
	case sasbtStall:
		{
			nSrcAirID = m_StallContainer[nTarIndex].nTemplateID;
			nSrcAirExp = m_StallContainer[nTarIndex].nExp;

			m_StallContainer[nTarIndex].nTemplateID	= nTargetAirID;
			m_StallContainer[nTarIndex].nExp		= nTargetAirExp;
		}
		break;
	case sasbtBag:
		{
			nSrcAirID = m_BagContainer[nTarIndex].nTemplateID;
			nSrcAirExp = m_BagContainer[nTarIndex].nExp;

			m_BagContainer[nTarIndex].nTemplateID = nTargetAirID;
			m_BagContainer[nTarIndex].nExp		  =	nTargetAirExp;
		}
		break;
	case sasbtEquip:
		{
			if (m_EquipContainer[nTarIndex].nTemplateID)						// If qi is already equipped, first roll back its attributes
			{
				pAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_EquipContainer[nTarIndex].nTemplateID);
				LOG_PROCESS_ERROR(pAir);

				for (INT32 nIndex = 0; nIndex < MAX_ATTR_PER_NODE; nIndex++)		// Remove equipment attributes
				{
					if (pAir->nNodeID[nIndex])
					{
						nRetCode = CModifierTree::GetInstance().RollBackExtraNode(pAir->nNodeID[nIndex], m_pOwner, NULL);
						LOG_PROCESS_ERROR(nRetCode);
					}
				}
			}

			nSrcAirID = m_EquipContainer[nTarIndex].nTemplateID;
			nSrcAirExp = m_EquipContainer[nTarIndex].nExp;

			m_EquipContainer[nTarIndex].nTemplateID = nTargetAirID;
			m_EquipContainer[nTarIndex].nExp		= nTargetAirExp;

			pAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_EquipContainer[nTarIndex].nTemplateID);
			LOG_PROCESS_ERROR(pAir);

			for (INT32 nIndex = 0; nIndex < MAX_ATTR_PER_NODE; nIndex++)		// Apply equipment attributes
			{
				if (pAir->nNodeID[nIndex])
				{
					nRetCode = CModifierTree::GetInstance().ApplyExtraNode(pAir->nNodeID[nIndex], m_pOwner, NULL);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	if (nSrcAirID)
	{
		switch(nSrcType)
		{
		case sasbtStall:
			{
				m_StallContainer[nSrcIndex].nTemplateID	= nSrcAirID;
				m_StallContainer[nSrcIndex].nExp		= nSrcAirExp;
			}
			break;
		case sasbtBag:
			{
				m_BagContainer[nSrcIndex].nTemplateID = nSrcAirID;
				m_BagContainer[nSrcIndex].nExp		  =	nSrcAirExp;
			}
			break;
		case sasbtEquip:
			{
				m_EquipContainer[nSrcIndex].nTemplateID = nSrcAirID;
				m_EquipContainer[nSrcIndex].nExp		= nSrcAirExp;

				pAir = CDataManager<AIR_DATA>::GetInstance().GetData(nSrcAirID);
				LOG_PROCESS_ERROR(pAir);

				for (INT32 nIndex = 0; nIndex < MAX_ATTR_PER_NODE; nIndex++)		// Remove equipment attributes
				{
					if (pAir->nNodeID[nIndex])
					{
						nRetCode = CModifierTree::GetInstance().RollBackExtraNode(pAir->nNodeID[nIndex], m_pOwner, NULL);
						LOG_PROCESS_ERROR(nRetCode);
					}
				}
			}
			break;
		default:
			{
				LOG_PROCESS_ERROR(FALSE);
			}
			break;
		}
	}

#ifndef _ROBOT

	nRetCode = CCliMessageHandler::GetInstance().DoSpiritAirChange(m_pOwner, nTarType, nTarIndex, nTargetAirID, nTargetAirExp);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoSpiritAirChange(m_pOwner, nSrcType, nSrcIndex, nSrcAirID, nSrcAirExp);
	LOG_PROCESS_ERROR(nRetCode);

#endif
	return errSuccess;

Exit0:
	return nResult;
}

INT32 CSpirit::AddSlot(INT32 nType, INT32 nCount)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	INT32 nSlotIndex = 0;
	INT32 nMoneyTotalCost = 0;

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR(nCount > 0);

	switch(nType)
	{
	case sasbtBag:
		{
			LOG_PROCESS_ERROR(m_nBagContainerSize + nCount <= MAX_AIRSEA_BAG_CONTAINER_SIZE);

			nSlotIndex = m_nBagContainerSize - INIT_SPIRIIT_AIR_SEA_BAG_SIZE[nType];

			for (INT32 nIndex = 0; nIndex < nCount; nIndex++)
			{
				nMoneyTotalCost += (50 + 5*nSlotIndex);
				nSlotIndex++;
			}
		}
		break;
	case sasbtStall:
		{
			LOG_PROCESS_ERROR(m_nStallContainerSize + nCount <= MAX_AIRSEA_STALL_CONTAINER_SIZE);

			nSlotIndex = m_nStallContainerSize - INIT_SPIRIIT_AIR_SEA_BAG_SIZE[nType];

			for (INT32 nIndex = 0; nIndex < nCount; nIndex++)
			{
				nMoneyTotalCost += (50 + 5*nSlotIndex);
				nSlotIndex++;
			}

		}
		break;
	case sasbtEquip:
		{
			LOG_PROCESS_ERROR(m_nEquipContainerSize + 1 <= MAX_AIRSEA_EQUIP_CONTAINER_SIZE);

			const SPIRIT_SLOT_DATA* pSpiritSlotData = CDataManager<SPIRIT_SLOT_DATA>::GetInstance().GetData(m_nEquipContainerSize + 1);
			LOG_PROCESS_ERROR(pSpiritSlotData);
			LOG_PROCESS_ERROR(GetLevel() >= pSpiritSlotData->nLevelLimit);

			nMoneyTotalCost = pSpiritSlotData->nMoneyCost;
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_02) >= nMoneyTotalCost, errNotEnoughMoney);

	nResult = m_pOwner->GetItemBag()->SubMoney(MONEYTYPE_02, nMoneyTotalCost, MONEYLOG_SPIRIT_BUYSLOT, 0);
	LOG_PROCESS_ERROR(nResult == errSuccess);

	switch(nType)
	{
	case sasbtBag:
		{
			m_nBagContainerSize += nCount;
			nSize = m_nBagContainerSize;
		}
		break;
	case sasbtStall:
		{
			m_nStallContainerSize += nCount;
			nSize = m_nStallContainerSize;
		}
		break;
	case sasbtEquip:
		{
			m_nEquipContainerSize += 1;
			nSize = m_nEquipContainerSize;
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

#ifndef _ROBOT

	nRetCode = CCliMessageHandler::GetInstance().DoSpiritSlotSizeChange(m_pOwner, nType, nSize);
	LOG_PROCESS_ERROR(nRetCode);

#endif

	return errSuccess;

Exit0:
	return nResult;
}

INT32 CSpirit::SetAir(INT32 nBagType, INT32 nTemplateID, INT32 nExp, INT32 nIndex)
{
	INT32 nRetCode = 0;
	const AIR_DATA* pAirData = NULL;

	pAirData = CDataManager<AIR_DATA>::GetInstance().GetData(nTemplateID);
	LOG_PROCESS_ERROR(pAirData);

	switch(nBagType)
	{
	case sasbtStall:
		{
			LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_AIRSEA_STALL_CONTAINER_SIZE);

			m_StallContainer[nIndex].nTemplateID	= nTemplateID;
			m_StallContainer[nIndex].nExp		= nExp;
		}
		break;
	case sasbtBag:
		{
			LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_AIRSEA_BAG_CONTAINER_SIZE);

			m_BagContainer[nIndex].nTemplateID	= nTemplateID;
			m_BagContainer[nIndex].nExp		= nExp;
		}
		break;
	case sasbtEquip:
		{
			LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_AIRSEA_EQUIP_CONTAINER_SIZE);

			m_EquipContainer[nIndex].nTemplateID	= nTemplateID;
			m_EquipContainer[nIndex].nExp			= nExp;
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CSpirit::GetHigherAirLevelIndex()
{
	INT32 nRetCode = 0;
	INT32 nHigherLevel = 1;

	for (INT32 nIndex = MAX_AIR_PROCEDURE-1; nIndex >= 0; nIndex-- )
	{
		if (m_nProductAirLevel & (1 << nIndex))
		{
			nHigherLevel = nIndex;
			break;
		}
	}

	return nHigherLevel;
}

CAir* CSpirit::GetAir(INT32 nBagType, INT32 nBagIndex)
{
	switch(nBagType)
	{
	case sasbtBag:
		{
			LOG_PROCESS_ERROR(nBagIndex >= 0 && nBagIndex < m_nBagContainerSize);
			return &m_BagContainer[nBagIndex];
		}
		break;
	case sasbtStall:
		{
			LOG_PROCESS_ERROR(nBagIndex >= 0 && nBagIndex < m_nStallContainerSize);
			return &m_BagContainer[nBagIndex];
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
	}

Exit0:
	return NULL;
}

INT32 CSpirit::ExchangeAir(INT32 nAirID)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;

	const AIR_DATA* pAir = CDataManager<AIR_DATA>::GetInstance().GetData(nAirID);
	LOG_PROCESS_ERROR(pAir);
	LOG_PROCESS_ERROR(m_pOwner);

	LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_AIR) >= pAir->nExchangeCount, errSpiritExchangeMoney);

	nRetCode = AddAir(nAirID, pAir->nExp);	
	LOG_PROCESS_ERROR_RET_CODE(nRetCode != INVALID_AIR_INDEX, errSpiritAirSeaBagFull);

	if (pAir->nHandbookType > htInvalid)
		m_pOwner->GetAchievementList().ActivateHandbook(pAir->nHandbookType, pAir->nHandbookIndex);

	nResult = m_pOwner->GetItemBag()->SubMoney(MONEYTYPE_AIR, pAir->nExchangeCount, MONEYLOG_SPIRIT_BUYEXCHANGEAIR, 0);
	LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

	return errSuccess;

Exit0:
	return nResult;
}

/***************** blood sea ***************************/


INT32 CSpirit::SetBlood(INT32 nPage, INT32 nSlot, INT32 nNewNumber)
{
	INT32 nRetCode = 0;
	const BLOOD_DATA* pPreBlood = NULL;
	const BLOOD_DATA* pNextBlood = NULL;

	LOG_PROCESS_ERROR(nPage >= 0 && nPage < MAX_BLOODSEA_PAGE);
	LOG_PROCESS_ERROR(nSlot >= 0 && nSlot < MAX_BLOODSEA_SLOT_PER_PAGE);
	LOG_PROCESS_ERROR(nNewNumber > 0 && nNewNumber <= MAX_BLOODSEA_SLOT_NUMBER);

	// Remove

	pPreBlood = CDataManager<BLOOD_DATA>::GetInstance().GetData(MAKE_BLOOD_KEY(nPage+1, nSlot+1, m_nBloodSlot[nPage][nSlot]));
	pNextBlood = CDataManager<BLOOD_DATA>::GetInstance().GetData(MAKE_BLOOD_KEY(nPage+1, nSlot+1, nNewNumber));
	LOG_PROCESS_ERROR(pNextBlood);

	if (pPreBlood)
	{
		for (INT32 nIndex = 0; nIndex < MAX_BLOODSEA_ATTR_PER_SLOT; nIndex++)		// Remove equipment attributes
		{
			if (pPreBlood->nNodeID[nIndex])
			{
				nRetCode = CModifierTree::GetInstance().RollBackExtraNode(pPreBlood->nNodeID[nIndex], m_pOwner, NULL);
				LOG_PROCESS_ERROR(nRetCode);
			}
		}
	}

	m_nBloodSlot[nPage][nSlot] = pNextBlood->nNumber;

	// Equip

	for (INT32 nIndex = 0; nIndex < MAX_BLOODSEA_ATTR_PER_SLOT; nIndex++)		// Remove equipment attributes
	{
		if (pNextBlood->nNodeID[nIndex])
		{
			nRetCode = CModifierTree::GetInstance().ApplyExtraNode(pNextBlood->nNodeID[nIndex], m_pOwner, NULL);
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

#ifndef _ROBOT
	nRetCode = 	CCliMessageHandler::GetInstance().DoSpiritBrushSlot(m_pOwner, nPage+1, nSlot+1, m_nBloodSlot[nPage][nSlot]);
	LOG_PROCESS_ERROR(nRetCode);
#endif

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CSpirit::BrushBloodSlot(INT32 nPage, INT32 nSlot)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	INT32 nRandNumber = 0;
	INT32 nTotalRate = 0;

	const BLOOD_DATA* pNextBlood = NULL;

	LOG_PROCESS_ERROR(nPage >= 0 && nPage < MAX_BLOODSEA_PAGE);
	LOG_PROCESS_ERROR(nSlot >= 0 && nSlot < MAX_BLOODSEA_SLOT_PER_PAGE);

	LOG_PROCESS_ERROR(m_pOwner);
	LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetLevel() >= SPIRIT_OPEN_LEVEL, errSpiritRoleLevelLimit);
	LOG_PROCESS_ERROR_RET_CODE(m_nBloodSlot[nPage][nSlot] < MAX_BLOODSEA_SLOT_NUMBER, errSpiritBloodSlotNumber);
	
	nResult = _CanBrush(nPage, nSlot);
	LOG_PROCESS_ERROR(nResult == errSuccess);

	nRandNumber = g_Random(10000);

	for (INT32 nIndex = 1; nIndex <= MAX_BLOODSEA_SLOT_NUMBER; nIndex++)
	{
		pNextBlood = CDataManager<BLOOD_DATA>::GetInstance().GetData(MAKE_BLOOD_KEY(nPage+1, nSlot+1, nIndex));
		LOG_PROCESS_ERROR(pNextBlood);

		nTotalRate += pNextBlood->nRate;

		if (nTotalRate > nRandNumber)
		{
			break;
		}
	}

	LOG_PROCESS_ERROR_RET_CODE(m_nLevel >= pNextBlood->nLevelLimit, errSpiritBloodSlotLevel);														// Unlock level
	LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_SPELL_PTS) >= pNextBlood->nSpiritPtsCost, errSpiritSpellPts);				// Cultivation point amount

	nResult = m_pOwner->GetItemBag()->SubMoney(MONEYTYPE_SPELL_PTS, pNextBlood->nSpiritPtsCost, MONEYLOG_SPIRIT_BLOOD, 0);
	LOG_PROCESS_ERROR(nResult == errSuccess);

	nRetCode = SetBlood(nPage, nSlot, pNextBlood->nNumber);
	LOG_PROCESS_ERROR(nRetCode);

	return errSuccess;
Exit0:
	return nResult;
}

INT32 CSpirit::OpenBlood(INT32 nPage)
{
	INT32 nRetCode = 0;
	INT32 nResult = errUnknownError;
	const BLOOD_DATA* pBlood = NULL;

	LOG_PROCESS_ERROR(nPage >= 0 && nPage < MAX_BLOODSEA_PAGE);
	LOG_PROCESS_ERROR_RET_CODE(IsBloodOpen(nPage) == FALSE, errSpiritBloodAlreadyOpen);
	
	pBlood = CDataManager<BLOOD_DATA>::GetInstance().GetData(MAKE_BLOOD_KEY(nPage+1, 1, 1));
	LOG_PROCESS_ERROR(pBlood);
	LOG_PROCESS_ERROR(m_nLevel >= pBlood->nPageLevelLimit);

	nRetCode = SetBloodOpen(nPage);	
	LOG_PROCESS_ERROR(nRetCode);

	return errSuccess;
Exit0:
	return nResult;
}

/*************************************************************************************/

INT32 CSpirit::AddAir(INT32 nTemplateID, INT32 nExp)
{
	INT32 nRetCode = 0;
	INT32 nAirIndex = INVALID_AIR_INDEX;

	LOG_PROCESS_ERROR(m_pOwner);

	for (INT32 nIndex = 0; nIndex < m_nBagContainerSize; nIndex++)
	{
		if (m_BagContainer[nIndex].nTemplateID == 0)
		{
			m_BagContainer[nIndex].nTemplateID = nTemplateID;
			m_BagContainer[nIndex].nExp = nExp;

#ifndef _ROBOT
			nRetCode = CCliMessageHandler::GetInstance().DoSpiritAirChange(m_pOwner, sasbtBag, nIndex, nTemplateID, nExp);
			LOG_PROCESS_ERROR(nRetCode);
#endif	// _ROBOT

			nAirIndex = nIndex;
			break;
		}
	}

Exit0:
	return nAirIndex;
}

INT32 CSpirit::_MergeAir(INT32 nSrcCon, INT32 nTarCon)			// Source absorbs target
{
	INT32 nRetCode = 0;
	const AIR_DATA* pAir = NULL;

	LOG_PROCESS_ERROR(nSrcCon < m_nBagContainerSize);
	LOG_PROCESS_ERROR(nTarCon < m_nBagContainerSize);

	pAir = CDataManager<AIR_DATA>::GetInstance().GetData(m_BagContainer[nSrcCon].nTemplateID);
	LOG_PROCESS_ERROR(pAir);

	m_BagContainer[nSrcCon].nExp += m_BagContainer[nTarCon].nExp;

	if(m_BagContainer[nSrcCon].nExp < 0)
	{
		m_BagContainer[nSrcCon].nExp = INT_MAX;
	}

	m_BagContainer[nTarCon].nExp = 0;								// Absorbed
	m_BagContainer[nTarCon].nTemplateID = 0;

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoSpiritAirChange(m_pOwner, sasbtBag, nTarCon, m_BagContainer[nTarCon].nTemplateID, m_BagContainer[nTarCon].nExp);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoSpiritAirChange(m_pOwner, sasbtBag, nSrcCon, m_BagContainer[nSrcCon].nTemplateID, m_BagContainer[nSrcCon].nExp);
	LOG_PROCESS_ERROR(nRetCode);
		
#endif

	while ((pAir->nNextLevelExp != 0 && m_BagContainer[nSrcCon].nExp >= pAir->nNextLevelExp) && (pAir->nLevel < MAX_AIR_LEVEL))
	{
		const AIR_DATA* pNextAir = CDataManager<AIR_DATA>::GetInstance().GetData(pAir->nNextLevelID);
		LOG_PROCESS_ERROR(pNextAir);
		LOG_PROCESS_ERROR(pNextAir->nNextLevelExp > 0);

		m_BagContainer[nSrcCon].nTemplateID = pNextAir->nAirID;	// After leveling up, it becomes a new qi

#ifndef _ROBOT
		nRetCode = CCliMessageHandler::GetInstance().DoSpiritAirChange(m_pOwner, sasbtBag, nSrcCon, m_BagContainer[nSrcCon].nTemplateID, m_BagContainer[nSrcCon].nExp);
		LOG_PROCESS_ERROR(nRetCode);
#endif

		pAir = pNextAir;
	}

	return errSuccess;
Exit0:
	return errUnknownError;
}

INT32 CSpirit::_CanBrush(INT32 nPage, INT32 nSlot)
{
	INT32 nResult = errUnknownError;

	LOG_PROCESS_ERROR(nPage >= 0 && nPage < MAX_BLOODSEA_PAGE);
	LOG_PROCESS_ERROR(nSlot >= BLOODSEA_SLOT_INIT_SLOT	 && nSlot < MAX_BLOODSEA_SLOT_PER_PAGE);

	LOG_PROCESS_ERROR_RET_CODE(IsBloodOpen(nPage) == TRUE, errSpiritBloodNotOpen);

	if (nSlot != BLOODSEA_SLOT_INIT_SLOT)	// Ensure the previous level is unlocked
	{
		LOG_PROCESS_ERROR(m_nBloodSlot[nPage][nSlot-1] != 0);
	}

	return errSuccess;
Exit0:
	return nResult;
}

INT32 CSpirit::SetBloodOpen(INT32 nPage)
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(nPage >= 0 && nPage < MAX_BLOODSEA_PAGE);

	m_nBloodState |= (1 << nPage);

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoSpiritBloodOpen(m_pOwner, nPage+1);
	LOG_PROCESS_ERROR(nRetCode);
#endif	// _ROBOT

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CSpirit::IsBloodOpen(INT32 nPage)
{
	return (m_nBloodState& (1 << nPage)) ? TRUE : FALSE;
}
