#include "stdafx.h"
#include "wings_data.h"

BOOL WINGS_ENTRY::ReadLine(INT32& rnID, const char*& rpcszName, WINGS_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

	READ_INT("ID", m_nID);
	READ_INT_EXPR("AttributeID", m_nAttributeID);
	READ_INT("AttributeValue", m_nAttributeValue);
	READ_INT("Power", m_nPower);

	rnID = pData->m_nID;

	return TRUE;
Exit0:
	return FALSE;

}

BOOL WINGS_LEVEL_ENTRY::ReadLine(INT32& rnID, const char*& rpcszName, WINGS_LEVEL_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

	READ_INT("Level", m_nLevel);
	READ_INT_EXPR("Power", m_nPower);
	// attribute categories that only exist in 1.05 (burning / bleeding resist, ignore defence ...) are read as 0 and not applied
	nRetCode = pTabConfig->ReadData("AttributeID1", nRow, szExprValue, sizeof(szExprValue));
	LOG_PROCESS_ERROR(nRetCode);
	pData->m_nAttributeID[0] = 0;
	if (szExprValue[0] != '\0')
	{
		nRetCode = g_pReadDataScript->CallFunction("read_int_expr_safe", "s:d", szExprValue, &(pData->m_nAttributeID[0]));
		LOG_PROCESS_ERROR(nRetCode);
	}
	READ_INT_EXPR("AttributeValue1", m_nAttributeValue[0]);
	nRetCode = pTabConfig->ReadData("AttributeID2", nRow, szExprValue, sizeof(szExprValue));
	LOG_PROCESS_ERROR(nRetCode);
	pData->m_nAttributeID[1] = 0;
	if (szExprValue[0] != '\0')
	{
		nRetCode = g_pReadDataScript->CallFunction("read_int_expr_safe", "s:d", szExprValue, &(pData->m_nAttributeID[1]));
		LOG_PROCESS_ERROR(nRetCode);
	}
	READ_INT_EXPR("AttributeValue2", m_nAttributeValue[1]);
	READ_INT_EXPR("ConsumeItemID1", m_nConsumeItemID[0]);
	READ_INT_EXPR("ConsumeItemCnt1", m_nConsumeItemCnt[0]);
	READ_INT_EXPR("ConsumeItemID2", m_nConsumeItemID[1]);
	READ_INT_EXPR("ConsumeItemCnt2", m_nConsumeItemCnt[1]);

	rnID = pData->m_nLevel;

	return TRUE;
Exit0:
	return FALSE;

}
