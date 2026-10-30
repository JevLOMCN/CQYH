#include "stdafx.h"
#include "init_reputation_data.h"
#include "script_mgr.h"
#include "string_def.h"
#include "money_def.h"

static constexpr const char* s_szClassName[clsPlayerEnd] =
{
	"Warrior",
	"Mage",
	"Rogue",
	"Archer",
	"Taoist",
};

BOOL INIT_REPUTATION_DATA::ReadLine(INT32& rnID, const char*& rpcszName, INIT_REPUTATION_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

    READ_INT("ForceID", nForceID);
    READ_INT("InitValue", nInitValue);
	READ_INT("MaxValue", nMaxValue);
	READ_INT("Active", bActive);
	READ_INT("CanAttackWithFreePkMode", bCanAttackWithFreePkMode);
	READ_INT("SabacMode", bSabacMode);

	for (INT32 nClass = clsPlayerBegin; nClass < clsPlayerEnd; nClass++)
	{
		pData->nMaxAwardItemCount[nClass] = 0;

		for (INT32 nIndex = 0; nIndex < MAX_REPUTATION_AWARD_COUNT; nIndex++)
		{
			char szField[COMMON_NAME_LEN];

			snprintf(szField, COMMON_NAME_LEN, "%sAwardItemID%d", s_szClassName[nClass], nIndex + 1);
			READ_INT(szField, nAwardItemID[nClass][nIndex]);

			snprintf(szField, COMMON_NAME_LEN, "%sAwardItemCount%d", s_szClassName[nClass], nIndex + 1);
			READ_INT(szField, nAwardItemCount[nClass][nIndex]);

			if (pData->nAwardItemID[nClass][nIndex] > MONEYTYPE_COUNT)
				pData->nMaxAwardItemCount[nClass]++;
		}
	}

    READ_END(nForceID);
}

