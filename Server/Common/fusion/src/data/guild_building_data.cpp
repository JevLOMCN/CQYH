#include "stdafx.h"
#include "guild_building_data.h"
#include "read_data_helper.h"

BOOL GUILD_BUILDING_DATA::ReadLine(INT32& rnID, const char*& rpcszName, GUILD_BUILDING_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
    READ_START();

    READ_INT("ID", dwID);
    READ_INT("Level", nLevel);
    READ_INT("MaxLevel", nMaxLevel);
    READ_INT("MaxNum", nMaxNum);
    READ_INT("Prosperity", nProsperity);
    READ_INT("MoneyConsumption", nMoneyConsumption);
    READ_INT("TotalMoneyConsumption", nTotalMoneyConsumption);
    READ_INT("HerbConsumption", nHerbConsumption);
    READ_INT("TotalHerbConsumption", nTotalHerbConsumption);
    READ_INT("WoodConsumption", nWoodConsumption);
    READ_INT("TotalWoodConsumption", nTotalWoodConsumption);
    READ_INT("CrystalConsumption", nCrystalConsumption);
    READ_INT("TotalCrystalConsumption", nTotalCrystalConsumption);
    READ_INT("SilverConsumption", nSilverConsumption);
    READ_INT("TotalSilverConsumption", nTotalSilverConsumption);
    READ_INT("UpgradeTime", nUpgradeTime);
    READ_INT("RoutineConsumption", nRoutineConsumption);
    READ_INT("CdtnBaseNpcID", dwCdtnBaseNpcID);
    READ_INT("ConditionBuildingID", dwConditionBuildingID);
    READ_INT("ConditionBuildingLvl", nConditionBuildingLvl);
    READ_INT("UpgradingSpellID", dwUpgradingSpellID);
    READ_INT("NpcTemplateID", dwNpcTemplateID);
    READ_INT("DeadNpcTemplateID", dwDeadNpcTemplateID);
    READ_INT("ReviveTime", nReviveTime);
    READ_INT("Param1", nParam1);
    READ_INT("Param2", nParam2);
    READ_INT("Param3", nParam3);
    READ_INT("Param4", nParam4);
    READ_INT("Param5", nParam5);
    READ_INT("IntruderMsgID", dwIntruderMsgID);
    READ_INT("DefenderMsgID", dwDefenderMsgID);
    READ_INT("FewMembersReward", dwFewMembersReward);
    READ_INT("SomeMembersReward", dwSomeMembersReward);
    READ_INT("ManyMembersReward", dwManyMembersReward);

    //READ_END(((dwID<<16) + nLevel));
    rnID = (pData->dwID<<16)+pData->nLevel;

    return TRUE;
Exit0:
    rnID = (pData->dwID<<16)+pData->nLevel;
    return FALSE;
}



const GUILD_BUILDING_DATA* GetGuildBuildingData(UINT32 dwType, INT32 nLevel)
{
    return CDataManager<GUILD_BUILDING_DATA>::GetInstance().GetData((dwType << 16) + nLevel);
}








