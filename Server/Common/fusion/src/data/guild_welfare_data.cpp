#include "stdafx.h"
#include "guild_welfare_data.h"
#include "read_data_helper.h"
#include "role_def.h"

BOOL GUILD_WELFARE_DATA::ReadLine(INT32& rnID, const char*& rpcszName, GUILD_WELFARE_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
    READ_START();

    READ_INT("ID", dwID);
    READ_INT("Level", nLevel);
    READ_INT("AuraID", dwAuraID);
    READ_INT("Kind", nKind);
    READ_INT("MaxLevel", nMaxLevel);
    READ_INT("UpgradeTime", nUpgradeTime);
    READ_INT("ConditionBuildingID", dwConditionBuildingID);
    READ_INT("ConditionBuildingLvl", nConditionBuildingLvl);
    READ_INT("PurchaseLevelRequirement", nPurchaseLevelRequirement);
    READ_INT("PurchaseMoneyConsumption", nPurchaseMoneyConsumption);
    READ_INT("PurchaseGuildCurrencyConsumption", nPurchaseGuildCurrencyConsumption);
    //READ_INT("PurchaseLimit", nPurchaseLimit);
    READ_INT("VipPurchaseLimit0", arrVipPurchaseLimit[vlLevel0]);
    READ_INT("VipPurchaseLimit1", arrVipPurchaseLimit[vlLevel1]);
    READ_INT("VipPurchaseLimit2", arrVipPurchaseLimit[vlLevel2]);
    READ_INT("VipPurchaseLimit3", arrVipPurchaseLimit[vlLevel3]);
    READ_INT("UpgradeMoneyConsumption", nUpgradeMoneyConsumption);
    READ_INT("UpgradeHerbConsumption", nUpgradeHerbConsumption);
    READ_INT("UpgradeWoodConsumption", nUpgradeWoodConsumption);
    READ_INT("UpgradeCrystalConsumption", nUpgradeCrystalConsumption);
    READ_INT("UpgradeSliverConsumption", nUpgradeSilverConsumption);

    //READ_END(((pData->dwID<<16)+pData->nLevel));
    rnID = (pData->dwID<<16)+pData->nLevel;

    return TRUE;
Exit0:
    rnID = (pData->dwID<<16)+pData->nLevel;
    return FALSE;
}


const GUILD_WELFARE_DATA* GetGuildWelfareData(INT32 nType, INT32 nLevel)
{
    return CDataManager<GUILD_WELFARE_DATA>::GetInstance().GetData((nType << 16) + nLevel);
}





