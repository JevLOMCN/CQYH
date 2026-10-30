#include "stdafx.h"
#include "guild_dungeon_data.h"
#include "math_ex.h"
#include "read_data_helper.h"

BOOL GUILD_DUNGEON_DATA::ReadLine(INT32& rnID, const char*& rpcszName, GUILD_DUNGEON_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
    READ_START();

    READ_INT("SceneTemplateID", dwSceneTemplateID);
    READ_INT("FatigueConsume", nFatigueComsume);
    READ_INT("ConditionLevel", nConditionLevel);
    READ_INT("ConditionBuildingID", dwConditionBuildingID);
    READ_INT("ConditionBuildingLevel", nConditionBuildingLevel);
    READ_INT("ConditionMoney", nConditionMoney);
    READ_INT("BestTeammateNum", nBestTeammateNum);
    READ_LENGTH("PosX", nPosX);
    READ_LENGTH("PosY", nPosY);
    READ_LENGTH("PosZ", nPosZ);
    READ_ANGLE("Direction", nDirection);

    for (INT32 i = 0; i < GuildDungeonDropGroupCount; ++i)
    {
        char szFieldName[256];
        snprintf(szFieldName, 256, "DropChance%d", i+1);
        READ_INT(szFieldName, arrDropGroup[i].nChance);

        snprintf(szFieldName, 256, "DropGroupID%d", i+1);
        READ_INT(szFieldName, arrDropGroup[i].dwGroupID);
    }

    for (INT32 i = 0; i < GuildDungeonAwardTableSize; ++i)
    {
        char szFieldName[256];
        snprintf(szFieldName, 256, "Award%d", i+1);
        READ_INT(szFieldName, arrAwardID[i]);
    }

    READ_END(dwSceneTemplateID);
}

const GUILD_DUNGEON_DATA * GetGuildDungeonData(UINT32 dwTemplateID)
{
    return CDataManager<GUILD_DUNGEON_DATA>::GetInstance().GetData(dwTemplateID);
}


