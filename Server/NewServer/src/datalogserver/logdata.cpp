#include "stdafx.h"
#include "logdata.h"

#include <time.h>

#include "mysql.h"
#include "mysqld_error.h"

#include "m2d_message.h"
#include "g2d_message.h"

#include "time_ex.h"
#include "dir.h"
#include "ifile.h"

CLogData CLogData::ms_Instance;

CLogData::CLogData()
{
	m_bStopThread = FALSE;
}

CLogData::~CLogData()
{
	UnInit();
}

BOOL CLogData::Init()
{
	INT32 nRetCode = 0;

	m_bStopThread = FALSE;

	for (INT32 nIndex = 0; nIndex < dlmtTotal; nIndex++)
	{
		nRetCode = ms_Instance.m_PushList[nIndex].Init(MAX_LOGDATA_PARAM_SIZE);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = ms_Instance.m_Thread[nIndex].Start(_WorkerThread, (void*)nIndex);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CLogData::UnInit()
{
	INT32 nRetCode = 0;

	m_bStopThread = TRUE;

	for (INT32 nIndex = 0; nIndex < dlmtTotal; nIndex++)
	{
		m_Thread[nIndex].Wait();

		while (auto* pending = static_cast<char*>(m_PushList[nIndex].Pop()))
            delete[] pending;

		nRetCode = m_PushList[nIndex].UnInit();
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
}

BOOL CLogData::AsyncWriteLog(INT32 nModuleIndex, const char* pMessage)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(nModuleIndex >= 0 && nModuleIndex < dlmtTotal);

	nRetCode = ms_Instance.m_PushList[nModuleIndex].Push((void*)pMessage);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

THREAD_DECLARE(CLogData::_WorkerThread)(void* pParam)
{
	INT32 nRetCode = 0;
	INT32 nModuleIndex = (INT32)pParam; 
	HANDLE hFile = INVALID_HANDLE_VALUE;
	UINT32 dwCreateFileTime = 0;
	UINT32 dwLastFlushTime = 0;
	INT32 nTimeZone = get_time_zone();
    BOOL bNeedFlush = FALSE;
	LARGE_INTEGER liFileSize = { 0 };
	DWORD dwWriteSize = 0;
	
	if (nModuleIndex < 0 || nModuleIndex >= dlmtTotal)
	{
		LOG_CHECK_ERROR(nModuleIndex >= 0 && nModuleIndex < dlmtTotal);
		return 0;
	}

    INF("module %d log thread started", nModuleIndex);
	while (!ms_Instance.m_bStopThread)
	{
		char* pMessage = NULL;
		UINT32 dwCurrentTime = time(NULL);

		if ((dwCurrentTime + nTimeZone) / 86400 != (dwCreateFileTime + nTimeZone) / 86400)
		{
			if (hFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}
		}

		if (hFile == INVALID_HANDLE_VALUE)
		{
			char szFileName[MAX_PATH];

			const tm_ex* pNow = localtime_ex();
			snprintf(szFileName, MAX_PATH, "logdata/%04d-%02d-%02d", pNow->tm_year, pNow->tm_mon, pNow->tm_mday);

			nRetCode = g_CreateDirectory("logdata", TRUE);
			LOG_CHECK_ERROR(nRetCode);

			nRetCode = g_CreateDirectory(szFileName, TRUE);
			LOG_CHECK_ERROR(nRetCode);
			
			snprintf(szFileName, MAX_PATH, "logdata/%04d-%02d-%02d/%s_%04d_%02d_%02d.txt",
				pNow->tm_year, pNow->tm_mon, pNow->tm_mday, s_pcszModuleName[nModuleIndex], 
				pNow->tm_year, pNow->tm_mon, pNow->tm_mday);

			hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			LOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

			nRetCode = GetFileSizeEx(hFile, &liFileSize);
			LOG_PROCESS_ERROR(nRetCode);

			nRetCode = SetFilePointer(hFile, liFileSize.LowPart, &liFileSize.HighPart, FILE_BEGIN);
			LOG_PROCESS_ERROR(nRetCode != INVALID_SET_FILE_POINTER);

			if (liFileSize.QuadPart == 0)
				ms_Instance._CreateHeader(hFile, nModuleIndex);

			dwCreateFileTime = dwCurrentTime;
		}

        if (bNeedFlush && dwCurrentTime > dwLastFlushTime)
		{
			nRetCode = FlushFileBuffers(hFile);
			LOG_CHECK_ERROR(nRetCode);

            bNeedFlush = FALSE;
			dwLastFlushTime = dwCurrentTime;
		}

		if (ms_Instance.m_PushList[nModuleIndex].Check())
		{
			INT32	nMessageSize = 0;

			pMessage = (char*)ms_Instance.m_PushList[nModuleIndex].Pop();
			LOG_PROCESS_ERROR(pMessage);

			nMessageSize = (INT32)strnlen(pMessage, MAX_DL_RECORD_MESSAGE_LEN);

			nRetCode = WriteFile(hFile, pMessage, nMessageSize, &dwWriteSize, NULL);
			LOG_PROCESS_ERROR(nRetCode && dwWriteSize == nMessageSize);

            bNeedFlush = TRUE;

			SAFE_DELETE_ARRAY(pMessage);
		}
		else
		{
			ms_Instance.m_Thread[nModuleIndex].WaitForStop(std::chrono::milliseconds(16));
		}

		continue;
	Exit0:
		SAFE_DELETE_ARRAY(pMessage);
	}

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return 0;
}

void CLogData::_CreateHeader(HANDLE hFile, INT32 nModuleIndex)
{
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	DWORD dwWriteSize = 0;

	static constexpr const char* s_szContent[dlmtTotal] =
	{
		// dlmtItem
		"Time\tAreaID\tGroupID\tRoleID\tActionType\tGuid\tTemplateID\tItemCount\tReason\tUserDefinedID\r\n",
		// dlmtChat
		"Time\tAreaID\tGroupID\tSourceID\tTargetID\tChatType\tMessage\r\n",
		// dlmtQuest
		"Time\tAreaID\tGroupID\tSourceID\tQuestID\tActionType\tQuestType\tQuestName\r\n",
		// dlmtAchievement
		"Time\tAreaID\tGroupID\tRoleID\tAchievementID\tAchievementName\r\n",
		// dlmtLevelUp
		"Time\tAreaID\tGroupID\tRoleID\tLevel\tLevelOnlineTime\tTotalOnlineTime\r\n",
		// dlmtDie
		"Time\tAreaID\tGroupID\tSceneTemplateID\tSceneID\tX\tY\tZ\tdwKillerID\tKillerType\tKillerName\tKillerLevel\tKillerClass\tVicID\tVicLevel\tVicClass\tVicName\r\n",
		// dlmtScene
		"Time\tAreaID\tGroupID\tSceneID\tTemplateID\tRoleID\tActionType\r\n",
		// dlmtConSlot
		"Time\tAreaID\tGroupID\tRoleID\tConType\tConSize\r\n",
		// dlmtRoleLogout_basic
		"Time\tAreaID\tGroupID\tRoleID\tSndaID\tSceneID\tSceneTemplateID\tName\tClass\tSex\tLevel\tExp\tMoney1\tMoney2\tX\tY\tZ\tGuildName\r\n",
		// dlmtRoleLogout_friend
		//"Time\tAreaID\tGroupID\tRoleID\tSndaID\tName\tFriend\r\n",
		// dlmtRoleLogout_master
		//"Time\tAreaID\tGroupID\tRoleID\tSndaID\tName\tStudent\r\n",
		// dlmtRoleLogout_itembag_default
		"Time\tAreaID\tGroupID\tRoleID\tSndaID\tName\t"\
			"DefaultGuid1\tDefaultTemplateID1\tDefaultColor1\tDefaultGuid2\tDefaultTemplateID2\tDefaultColor2\t"\
			"DefaultGuid3\tDefaultTemplateID3\tDefaultColor3\tDefaultGuid4\tDefaultTemplateID4\tDefaultColor4\t"\
			"DefaultGuid5\tDefaultTemplateID5\tDefaultColor5\tDefaultGuid6\tDefaultTemplateID6\tDefaultColor6\t"\
			"DefaultGuid7\tDefaultTemplateID7\tDefaultColor7\tDefaultGuid8\tDefaultTemplateID8\tDefaultColor8\t"\
			"DefaultGuid9\tDefaultTemplateID9\tDefaultColor9\tDefaultGuid10\tDefaultTemplateID10\tDefaultColor10\t"\
			"DefaultGuid11\tDefaultTemplateID11\tDefaultColor11\tDefaultGuid12\tDefaultTemplateID12\tDefaultColor12\t"\
			"DefaultGuid13\tDefaultTemplateID13\tDefaultColor13\tDefaultGuid14\tDefaultTemplateID14\tDefaultColor14\t"\
			"DefaultGuid15\tDefaultTemplateID15\tDefaultColor15\tDefaultGuid16\tDefaultTemplateID16\tDefaultColor16\t"\
			"DefaultGuid17\tDefaultTemplateID17\tDefaultColor17\tDefaultGuid18\tDefaultTemplateID18\tDefaultColor18\t"\
			"DefaultGuid19\tDefaultTemplateID19\tDefaultColor19\tDefaultGuid20\tDefaultTemplateID20\tDefaultColor20\t"\
			"DefaultGuid21\tDefaultTemplateID21\tDefaultColor21\tDefaultGuid22\tDefaultTemplateID22\tDefaultColor22\t"\
			"DefaultGuid23\tDefaultTemplateID23\tDefaultColor23\tDefaultGuid24\tDefaultTemplateID24\tDefaultColor24\t"\
			"DefaultGuid25\tDefaultTemplateID25\tDefaultColor25\tDefaultGuid26\tDefaultTemplateID26\tDefaultColor26\t"\
			"DefaultGuid27\tDefaultTemplateID27\tDefaultColor27\tDefaultGuid28\tDefaultTemplateID28\tDefaultColor28\t"\
			"DefaultGuid29\tDefaultTemplateID29\tDefaultColor29\tDefaultGuid30\tDefaultTemplateID30\tDefaultColor30\t"\
			"DefaultGuid31\tDefaultTemplateID31\tDefaultColor31\tDefaultGuid32\tDefaultTemplateID32\tDefaultColor32\t"\
			"DefaultGuid33\tDefaultTemplateID33\tDefaultColor33\tDefaultGuid34\tDefaultTemplateID34\tDefaultColor34\t"\
			"DefaultGuid35\tDefaultTemplateID35\tDefaultColor35\tDefaultGuid36\tDefaultTemplateID36\tDefaultColor36\t"\
			"DefaultGuid37\tDefaultTemplateID37\tDefaultColor37\tDefaultGuid38\tDefaultTemplateID38\tDefaultColor38\t"\
			"DefaultGuid39\tDefaultTemplateID39\tDefaultColor39\tDefaultGuid40\tDefaultTemplateID40\tDefaultColor40\t"\
			"DefaultGuid41\tDefaultTemplateID41\tDefaultColor41\tDefaultGuid42\tDefaultTemplateID42\tDefaultColor42\t"\
			"DefaultGuid43\tDefaultTemplateID43\tDefaultColor43\tDefaultGuid44\tDefaultTemplateID44\tDefaultColor44\t"\
			"DefaultGuid45\tDefaultTemplateID45\tDefaultColor45\tDefaultGuid46\tDefaultTemplateID46\tDefaultColor46\t"\
			"DefaultGuid47\tDefaultTemplateID47\tDefaultColor47\tDefaultGuid48\tDefaultTemplateID48\tDefaultColor48\t"\
			"DefaultGuid49\tDefaultTemplateID49\tDefaultColor49\tDefaultGuid50\tDefaultTemplateID50\tDefaultColor50\t"\
			"DefaultGuid51\tDefaultTemplateID51\tDefaultColor51\tDefaultGuid52\tDefaultTemplateID52\tDefaultColor52\t"\
			"DefaultGuid53\tDefaultTemplateID53\tDefaultColor53\tDefaultGuid54\tDefaultTemplateID54\tDefaultColor54\t"\
			"DefaultGuid55\tDefaultTemplateID55\tDefaultColor55\tDefaultGuid56\tDefaultTemplateID56\tDefaultColor56\t"\
			"DefaultGuid57\tDefaultTemplateID57\tDefaultColor57\tDefaultGuid58\tDefaultTemplateID58\tDefaultColor58\t"\
			"DefaultGuid59\tDefaultTemplateID59\tDefaultColor59\tDefaultGuid60\tDefaultTemplateID60\tDefaultColor60\t"\
			"DefaultGuid61\tDefaultTemplateID61\tDefaultColor61\tDefaultGuid62\tDefaultTemplateID62\tDefaultColor62\t"\
			"DefaultGuid63\tDefaultTemplateID63\tDefaultColor63\tDefaultGuid64\tDefaultTemplateID64\tDefaultColor64\r\n",
		// dlmtRoleLogout_itembag_equipped
		"Time\tAreaID\tGroupID\tRoleID\tSndaID\tName\tItemEquipedGuid1\t"\
			"ItemEquipedTemplateID1\tItemEquipedColor1\tItemEquipedGuid2\tItemEquipedTemplateID2\tItemEquipedColor2\t"\
			"ItemEquipedGuid3\tItemEquipedTemplateID3\tItemEquipedColor3\tItemEquipedGuid4\tItemEquipedTemplateID4\tItemEquipedColor4\t"\
			"ItemEquipedGuid5\tItemEquipedTemplateID5\tItemEquipedColor5\tItemEquipedGuid6\tItemEquipedTemplateID6\tItemEquipedColor6\t"\
			"ItemEquipedGuid7\tItemEquipedTemplateID7\tItemEquipedColor7\tItemEquipedGuid8\tItemEquipedTemplateID8\tItemEquipedColor8\t"\
			"ItemEquipedGuid9\tItemEquipedTemplateID9\tItemEquipedColor9\tItemEquipedGuid10\tItemEquipedTemplateID10\tItemEquipedColor10\t"\
			"ItemEquipedGuid11\tItemEquipedTemplateID11\tItemEquipedColor11\tItemEquipedGuid12\tItemEquipedTemplateID12\tItemEquipedColor12\t"\
			"ItemEquipedGuid13\tItemEquipedTemplateID13\tItemEquipedColor13\tItemEquipedGuid14\tItemEquipedTemplateID14\tItemEquipedColor14\t"\
			"ItemEquipedGuid15\tItemEquipedTemplateID15\tItemEquipedColor15\r\n",
		// dlmtLogin
		"Time\tAreaID\tGroupID\tSndaID\tActionType\tIP\r\n",
		// dlmtGmCommand
		//"Time\tAreaID\tGroupID\tRoleID\tRoleName\tResult\tCommand\r\n",
		// dlmtSubMoney
		"Time\tAreaID\tGroupID\tSrcRoleID\tTargetRoleID\tAction\tMoneyType\tMoneyCount\tReason\tParam\r\n",
		// dlmtTeam
		"Time\tAreaID\tGroupID\tTeamID\tRoleID\tLeaderID\tMemberCount\tActionType\r\n",
		// dlmtRolePosition
		"Time\tAreaID\tGroupID\tRoleID\tSceneTempID\tSceneID\tX\tY\tZ\r\n",
		// dlmtClientHardwareInfo
		"Time\tAreaID\tGroupID\tRoleID\t"\
			"stcpuinfo.AddressWidth\tstcpuinfo.Family\tstcpuinfo.L2CacheSize\tstcpuinfo.L3CacheSize\t" \
			"stcpuinfo.Level\tstcpuinfo.MaxClockSpeed\tstcpuinfo.Name\tstcpuinfo.NumberOfCores\t" \
			"stcpuinfo.NumberOfLogicalProcessors\tstcpuinfo.Revision\tstcpuinfo.Stepping\tstcpuinfo.Version\t" \
			"stdiskinfo.MediaType\tstdiskinfo.Model\tstdiskinfo.Size\tstmemoryinfo.Capacity\tstmemoryinfo.MemoryType\t" \
			"stmemoryinfo.Speed\tstnetworkadapterinfo.AdapterType\tpstnetworkadapterinfo.Name\tstnetworkadapterinfo.NetworkAdapterInfo\t" \
			"stvideoinfo.AdapterRAM\tstvideoinfo.CurrentBitsPerPixel\tstvideoinfo.CurrentHorizontalResolution\t" \
			"stvideoinfo.CurrentRefreshRate\tstvideoinfo.CurrentVerticalResolution\tstvideoinfo.Name\t" \
			"stvideoinfo.VideoMemoryType\tstvideoinfo.VideoProcessor\r\n",
		// dlmtClientFpsInfo
		"Time\tAreaID\tGroupID\tRoleID\tGameFps\tCpuFps\tGpuFps\tGateDelay\tGameDelay\r\n",
		// dlmtSpellEffect
		"Time\tAreaID\tGroupID\tRuneSlot\tSpellLevel\tSpellSerial\tCasterID\tSpellID\tTargetID\tParam1\tParam2\tFlag\r\n",
		// dlmtCastSpell
		"Time\tAreaID\tGroupID\tCasterLevel\tRuneSlot\tSpellLevel\t" \
			"SpellSerial\tCasterID\tSpellID\tTargetID\tCdRemain\tMp\tTargetX\t" \
			"TargetY\tTargetZ\tErrCode\tFromServer\tClientTime\tShiftPressed\r\n",

		// dlmtLearnSpell
		"Time\tAreaID\tGroupID\tRoleID\tRoleLevel\tRoleExp\tSpellID\tSpellLevel\r\n",
        // dlmtGuildEvent
        "Time\tAreaID\tGroupID\tGuildID\tEvent\tValue1\tValue2\tGMOperate\r\n",
        // dlmtGuildChest
        "Time\tAreaID\tGroupID\tGuildID\tMoney\tReason\r\n",
        // dlmtGuildNumStat
        "Time\tAreaID\tGroupID\tGuildNum\r\n",
        // dlmtGuildMoneyStat
        "Time\tAreaID\tGroupID\t" \
        "Guild1\tMoney1\tGuild2\tMoney2\tGuild3\tMoney3\tGuild4\tMoney4\tGuild5\tMoney5\t" \
        "Guild6\tMoney6\tGuild7\tMoney7\tGuild8\tMoney8\tGuild9\tMoney9\tGuild10\tMoney10" \
        "Guild11\tMoney11\tGuild12\tMoney12\tGuild13\tMoney13\tGuild14\tMoney14\tGuild15\tMoney15" \
        "Guild16\tMoney16\tGuild17\tMoney17\tGuild18\tMoney18\tGuild19\tMoney19\tGuild20\tMoney20\r\n",
        // dlmtGuildRiceStat 
        "Time\tAreaID\tGroupID\t" \
        "Guild1\tRice1\tGuild2\tRice2\tGuild3\tRice3\tGuild4\tRice4\tGuild5\tRice5\t" \
        "Guild6\tRice6\tGuild7\tRice7\tGuild8\tRice8\tGuild9\tRice9\tGuild10\tRice10" \
        "Guild11\tRice11\tGuild12\tRice12\tGuild13\tRice13\tGuild14\tRice14\tGuild15\tRice15" \
        "Guild16\tRice16\tGuild17\tRice17\tGuild18\tRice18\tGuild19\tRice19\tGuild20\tRice20\r\n",
        // dlmtGuildWoodStat
        "Time\tAreaID\tGroupID\t" \
        "Guild1\tWood1\tGuild2\tWood2\tGuild3\tWood3\tGuild4\tWood4\tGuild5\tWood5\t" \
        "Guild6\tWood6\tGuild7\tWood7\tGuild8\tWood8\tGuild9\tWood9\tGuild10\tWood10" \
        "Guild11\tWood11\tGuild12\tWood12\tGuild13\tWood13\tGuild14\tWood14\tGuild15\tWood15" \
        "Guild16\tWood16\tGuild17\tWood17\tGuild18\tWood18\tGuild19\tWood19\tGuild20\tWood20\r\n",
        // dlmtGuildStoneStat
        "Time\tAreaID\tGroupID\t" \
        "Guild1\tStone1\tGuild2\tStone2\tGuild3\tStone3\tGuild4\tStone4\tGuild5\tStone5\t" \
        "Guild6\tStone6\tGuild7\tStone7\tGuild8\tStone8\tGuild9\tStone9\tGuild10\tStone10" \
        "Guild11\tStone11\tGuild12\tStone12\tGuild13\tStone13\tGuild14\tStone14\tGuild15\tStone15" \
        "Guild16\tStone16\tGuild17\tStone17\tGuild18\tStone18\tGuild19\tStone19\tGuild20\tStone20\r\n",
        // dlmtGuildMetalStat
        "Time\tAreaID\tGroupID\t" \
        "Guild1\tMetal1\tGuild2\tMetal2\tGuild3\tMetal3\tGuild4\tMetal4\tGuild5\tMetal5\t" \
        "Guild6\tMetal6\tGuild7\tMetal7\tGuild8\tMetal8\tGuild9\tMetal9\tGuild10\tMetal10" \
        "Guild11\tMetal11\tGuild12\tMetal12\tGuild13\tMetal13\tGuild14\tMetal14\tGuild15\tMetal15" \
        "Guild16\tMetal16\tGuild17\tMetal17\tGuild18\tMetal18\tGuild19\tMetal19\tGuild20\tMetal20\r\n",
        // dlmtGuildBuildingStat
        "Time\tAreaID\tGroupID\tBuildingLevel\t" \
        "BuildingType1\tNum1\tBuildingType2\tNum2\tBuildingType3\tNum3\tBuildingType4\tNum4\tBuildingType5\tNum5\t" \
        "BuildingType6\tNum6\tBuildingType7\tNum7\tBuildingType8\tNum8\tBuildingType9\tNum9\tBuildingType10\tNum10" \
        "BuildingType11\tNum11\tBuildingType12\tNum12\tBuildingType13\tNum13\tBuildingType14\tNum14\tBuildingType15\tNum15" \
        "BuildingType16\tNum16\tBuildingType17\tNum17\tBuildingType18\tNum18\tBuildingType19\tNum19\tBuildingType20\tNum20\r\n",
        // dlmtBillingRequest
        "Time\tAreaID\tGroupID\tAction\tRpcID\tUserName\tUserName2\tUniqueID\tUniqueID2\tRoleID\tRoleID2\tOrderID\tOrderID2\tAmount\tParam\r\n",
        // dlmtBillingResult
        "Time\tAreaID\tGroupID\tAction\tRpcID\tUniqueID\tResult\r\n",
        // dlmtRuneItem
        "Time\tAreaID\tGroupID\tRoleID\tRoleLevel\tEquipTemplateID\tRuneItemID\tRuneID\r\n",
        // dlmtChangeItem
        "Time\tAreaID\tGroupID\tRoleID\tReason\tGuid\tTemplateID\tItemDetail\r\n",

        // dlmtStatRealMoney
        "Time\tAreaID\tGroupID\tRealMoneyToMoney02\tRealMoneyToItem\r\n",
        // dlmtStatQuestComplete
        "Time\tAreaID\tGroupID\tCompleteCount\r\n",
        // dlmtStatNpcDie
        "Time\tAreaID\tGroupID\tNpcID\tDieCount\r\n",
        // dlmtStatNpcMoney
        "Time\tAreaID\tGroupID\tNpcID\tMoneyCount\r\n",
        // dlmtStatMoney
        "Time\tAreaID\tGroupID\tProduceMoney01\tProduceMoney02\tDestroyMoney01\tDestroyMoney02\r\n",
        // dlmtStatMomentaryKillNpc
        "Time\tAreaID\tGroupID\tLevel5\tLevel6\tLevel7\tLevel8\tLevel9\tLevel10\tLevel11\tLevel12\tLevel13\tLevel14\tLevel15\tLevel16\tLevel17\tLevel18\r\n",
        // dlmtStatConsecutiveKillNpc
        "Time\tAreaID\tGroupID\tLevel5_9\tLevel10_14\tLevel15_19\tLevel20_24\tLevel25_29\tLevel30_34" \
        "\tLevel35_39\tLevel40_44\tLevel45_49\tLevel50_54\tLevel55_59\tLevel60_64\tLevel65_69" \
        "\tLevel70_74\tLevel75_79\tLevel80_84\tLevel85_89\tLevel90_94\tLevel95_99\r\n",
        // dlmtStatItem
        "Time\tAreaID\tGroupID\tTemplateID\tProduceCount\tDestroyCount\r\n",
		// dlmtAntiCheat
        "Time\tAreaID\tGroupID\tRoleID\tJigsawSn\tAction\tErrCode\tImageIndex\tRealAnswer\tBlockX\tBlockY\tRoleAnswer\r\n"

	};

	LOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);
	LOG_PROCESS_ERROR(nModuleIndex >= 0 && nModuleIndex < dlmtTotal);

	nSize = strlen(s_szContent[nModuleIndex]);
	nRetCode = WriteFile(hFile, s_szContent[nModuleIndex], nSize, &dwWriteSize, NULL);
	LOG_PROCESS_ERROR(nRetCode && dwWriteSize == nSize);

	nRetCode = FlushFileBuffers(hFile);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

