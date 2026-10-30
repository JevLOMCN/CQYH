#include "stdafx.h"
#include "ms_guild.h"
#include "guild_db_def.h"
#include "zlib.h"

typedef BOOL(CMsGuild::*LOAD_GUILD_DATA_FUNC)(const char *pcszData);

BOOL CMsGuild::LoadGuild(const char *pcData)
{
    INT32       nRetCode = 0;
    const char *pcActualData = pcData;
    GUILD_DB_DATA       *pGuildData = NULL;

    static LOAD_GUILD_DATA_FUNC s_GuildLoadFunc[] = {
        NULL,
        &CMsGuild::LoadGuild_v1,
        &CMsGuild::LoadGuild_v2,
        &CMsGuild::LoadGuild_v3,
        &CMsGuild::LoadGuild_v4,
        &CMsGuild::LoadGuild_v5,
	};

    LOG_PROCESS_ERROR(pcData);
    pcActualData = pcData + sizeof(UINT32);
    pGuildData = (GUILD_DB_DATA *)pcActualData; // The extra four bytes are the length inserted at the beginning when saving the blob

    LOG_PROCESS_ERROR(pGuildData->dwLen >= 4 && pGuildData->dwLen < 60000);
    LOG_PROCESS_ERROR(pGuildData->dwCRC ==
        crc32(0, (const Bytef*)&pGuildData->dwVersion, pGuildData->dwLen - sizeof(pGuildData->dwLen) - sizeof(pGuildData->dwCRC)));
    LOG_PROCESS_ERROR(pGuildData->dwVersion > 0 && pGuildData->dwVersion < sizeof(s_GuildLoadFunc) / sizeof(s_GuildLoadFunc[0]));

    if (pGuildData->dwVersion < GUILD_DB_CURRENT_VERSION)
        INF("load old guild %d db data %d %d", pGuildData->dwGuildID, pGuildData->dwVersion, pGuildData->dwLen);

    nRetCode = (this->*s_GuildLoadFunc[pGuildData->dwVersion])(pcActualData);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
