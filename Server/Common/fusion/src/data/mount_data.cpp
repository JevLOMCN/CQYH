#include "stdafx.h"
#include "mount_data.h"

BOOL MOUNT_ENTRY::ReadLine(INT32& rnID, const char*& rpcszName, MOUNT_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

	READ_INT("ID", m_nID);

	READ_INT("LevelLimit", m_nLevelLimit);
	READ_INT("SpeedModificationRate", m_nSpeedModificationRate);
	READ_INT("HitUnmountRate", m_nHitUnmountRate);
	READ_INT("AuraID", m_nAuraID);
	// 1.05 mount book columns (empty cells read as 0)
	READ_INT_EXPR("MountPower", m_nMountPower);
	READ_INT_EXPR("SoulAuraID", m_nSoulSpellID);

	READ_INT("ItemID1", m_nItemID[mftFragA]);
	READ_INT("ItemID2", m_nItemID[mftFragB]);
	READ_INT("ItemID3", m_nItemID[mftFragC]);

	READ_INT("ItemCount1", m_nItemCount[mftFragA]);
	READ_INT("ItemCount2", m_nItemCount[mftFragB]);
	READ_INT("ItemCount3", m_nItemCount[mftFragC]);

	rnID = pData->m_nID;

	return TRUE;
Exit0:
	return FALSE;

}

BOOL MOUNT_POWER_ENTRY::ReadLine(INT32& rnID, const char*& rpcszName, MOUNT_POWER_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();
	char szName[32];

	READ_INT_EXPR("ID", m_nID);
	READ_INT_EXPR("Lap", m_nLap);
	READ_INT_EXPR("MaxSoulSlotCount", m_nMaxSoulSlotCount);
	READ_INT_EXPR("Power", m_nPower);

	for (INT32 i = 0; i < MOUNT_POWER_ATTR_COUNT; ++i)
	{
		// attribute categories this game does not know are read as 0 and not applied
		snprintf(szName, sizeof(szName), "AttrType%d", i + 1);
		nRetCode = pTabConfig->ReadData(szName, nRow, szExprValue, sizeof(szExprValue));
		LOG_PROCESS_ERROR(nRetCode);
		pData->m_nAttributeID[i] = 0;
		if (szExprValue[0] != '\0')
		{
			nRetCode = g_pReadDataScript->CallFunction("read_int_expr_safe", "s:d", szExprValue, &(pData->m_nAttributeID[i]));
			LOG_PROCESS_ERROR(nRetCode);
		}

		snprintf(szName, sizeof(szName), "AttrValue%d", i + 1);
		nRetCode = pTabConfig->ReadData(szName, nRow, szExprValue, sizeof(szExprValue));
		LOG_PROCESS_ERROR(nRetCode);
		pData->m_nAttributeValue[i] = (szExprValue[0] != '\0') ? atoi(szExprValue) : 0;
	}

	rnID = pData->m_nID;

	return TRUE;
Exit0:
	return FALSE;

}
