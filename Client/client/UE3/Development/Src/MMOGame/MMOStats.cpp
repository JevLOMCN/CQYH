#include "stdafx.h"
#include "MMOStats.h"

DECLARE_STATS_GROUP(TEXT("MMOLogic"),MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("Load: ProcessCharData"),STAT_Load_ProcessCharData,MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("Load: ProcessCharData_NPC"),STAT_Load_ProcessCharData_NPC,MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("Load: ProcessCharData_EventAppear"),STAT_Load_ProcessCharData_EventAppear,MMOSTATGROUP_Logic);

DECLARE_CYCLE_STAT(TEXT("PickActor Time"),MMOSTAT_PickActor,MMOSTATGROUP_Logic);


// Network
DECLARE_STATS_GROUP(TEXT("MMONetwork"),MMOSTATGROUP_Network);

DECLARE_DWORD_COUNTER_STAT(TEXT("Net delay"),MMOSTAT_Network_NetDelay,MMOSTATGROUP_Network);
DECLARE_DWORD_COUNTER_STAT(TEXT("Game delay"),MMOSTAT_Network_GameDelay,MMOSTATGROUP_Network);
DECLARE_FLOAT_COUNTER_STAT(TEXT("Upload speed"),MMOSTAT_Network_Upload,MMOSTATGROUP_Network);
DECLARE_FLOAT_COUNTER_STAT(TEXT("Download  speed"),MMOSTAT_Network_Download,MMOSTATGROUP_Network);
DECLARE_CYCLE_STAT(TEXT("Proc Msg Buf Cycle"),MMOSTAT_Network_ProcMsgBuf,MMOSTATGROUP_Network);


DECLARE_CYCLE_STAT(TEXT("DropItemSystem Time"), STAT_DropItemSystemTime, MMOSTATGROUP_Logic );
//only two levels for now
DECLARE_CYCLE_STAT(TEXT("DropItemManager Time"), STAT_DropItemManagerTime, MMOSTATGROUP_Logic );
DECLARE_CYCLE_STAT(TEXT("DropItemTips Time"), STAT_DropItemTipsTime, MMOSTATGROUP_Logic );

//DECLARE_FLOAT_COUNTER_STAT(TEXT("Elapsed Time"),MMOSTAT_Network_ElapsedTime,MMOSTATGROUP_Network);
//
//DECLARE_DWORD_COUNTER_STAT(TEXT("-Sent Bytes"),MMOSTAT_Network_SentBytes,MMOSTATGROUP_Network);
//DECLARE_DWORD_COUNTER_STAT(TEXT("-Sent Msg Count"),MMOSTAT_Network_SentMsgCount,MMOSTATGROUP_Network);
//
//DECLARE_FLOAT_COUNTER_STAT(TEXT("-Avg Sent Msg Size"),MMOSTAT_Network_AvgSentMsgSize,MMOSTATGROUP_Network);
//DECLARE_FLOAT_COUNTER_STAT(TEXT("-Avg Sent Bandwidth Cost(BPS)"),MMOSTAT_Network_AvgSentBandwidthCost,MMOSTATGROUP_Network);
//DECLARE_FLOAT_COUNTER_STAT(TEXT("-Avg Sent Bandwidth Cost Max(BPS)"),MMOSTAT_Network_AvgSentBandwidthCostMax,MMOSTATGROUP_Network);
//
//DECLARE_DWORD_COUNTER_STAT(TEXT("+Received Bytes"),MMOSTAT_Network_ReceivedBytes,MMOSTATGROUP_Network);
//DECLARE_DWORD_COUNTER_STAT(TEXT("+Received Msg Count"),MMOSTAT_Network_ReceivedMsgCount,MMOSTATGROUP_Network);
//
//DECLARE_FLOAT_COUNTER_STAT(TEXT("+Avg Rcv Msg Size"),MMOSTAT_Network_AvgRcvMsgSize,MMOSTATGROUP_Network);
//DECLARE_FLOAT_COUNTER_STAT(TEXT("+Avg Rcv Bandwidth Cost(BPS)"),MMOSTAT_Network_AvgRcvBandwidthCost,MMOSTATGROUP_Network);
//DECLARE_FLOAT_COUNTER_STAT(TEXT("+Avg Rcv Bandwidth Cost Max(BPS)"),MMOSTAT_Network_AvgRcvBandwidthCostMax,MMOSTATGROUP_Network);

//
DECLARE_CYCLE_STAT(TEXT("MMOMinimap Tick Time"), STAT_MMOMinimap, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOMinimap_UpdateMapStaticMarks"), STAT_MMOMinimap_UpdateMapStaticMarks, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOMinimap_UpdateMapNearPlayer"), STAT_MMOMinimap_UpdateMapNearPlayer, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOMinimap_UpdateMapNearPlayer_SetMapMarkIcon"), STAT_MMOMinimap_UpdateMapNearPlayer_SetMapMarkIcon, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOMinimap_UpdateMapGlobalPlayer"), STAT_MMOMinimap_UpdateMapGlobalPlayer, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOMinimap_UpdateMapPath"), STAT_MMOMinimap_UpdateMapPath, MMOSTATGROUP_Logic);

//
DECLARE_CYCLE_STAT(TEXT("MMOWorldmap Tick Time"), STAT_MMOWorldmap, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdateMapStaticMarks"), STAT_MMOWorldmap_UpdateMapStaticMarks, MMOSTATGROUP_Logic);

DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdateMapStaticMarks_CreateMapMarkUIObject"), STAT_MMOWorldmap_UpdateMapStaticMarks_CreateMapMarkUIObject, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdateMapStaticMarks_eventInitMapMark"), STAT_MMOWorldmap_UpdateMapStaticMarks_eventInitMapMark, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdateMapStaticMarks_NavPoint"), STAT_MMOWorldmap_UpdateMapStaticMarks_NavPoint, MMOSTATGROUP_Logic);

DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdateMarksGlobalPlayer"), STAT_MMOWorldmap_UpdateMarksGlobalPlayer, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdateMarksNearPlayer"), STAT_MMOWorldmap_UpdateMarksNearPlayer, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdatePlayerMarksInMap"), STAT_MMOWorldmap_UpdatePlayerMarksInMap, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdateMapPath"), STAT_MMOWorldmap_UpdateMapPath, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("STAT_MMOWorldmap_UpdateMapMarksInfomation"), STAT_MMOWorldmap_UpdateMapMarksInfomation, MMOSTATGROUP_Logic);


DECLARE_CYCLE_STAT(TEXT("MMO AbilitySystem Time"), STAT_AbilitySystemTime, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("MMO Ability Particle Time"), STAT_AbilityParticleTime, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("MMO Ability ParticleSpawn Time"), STAT_AbilityParticleSpawnTime, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("MMO Ability ParticleActive Time"), STAT_AbilityParticleActiveTime, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("MMO AbilityEmitterTick Time"), STAT_AbilityEmitterTickTime, MMOSTATGROUP_Logic);

DECLARE_CYCLE_STAT(TEXT("MMO MoveDelta Time"), STAT_MoveDeltaTime, MMOSTATGROUP_Logic);

// add by zhangrui05@snda
DECLARE_CYCLE_STAT(TEXT("Achievement Time"), STAT_AchievementTime, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("Quest Time"), STAT_QuestTime, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("Guide Time"), STAT_GuideTime, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("Guild Time"), STAT_GuildTime, MMOSTATGROUP_Logic);

DECLARE_CYCLE_STAT(TEXT("  UpdateStatue"), STAT_GuildTime_UpdateStatue, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("  UpdateGuildForeignTime"), STAT_GuildTime_UpdateGuildForeignTime, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("  UpdateGuildAllyList"), STAT_GuildTime_EGET_UpdateGuildAllyList, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("  UpdateGuildEnemyList"), STAT_GuildTime_EGET_UpdateGuildEnemyList, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("  UpdateGuildApplyAllyList"), STAT_GuildTime_EGET_UpdateGuildApplyAllyList, MMOSTATGROUP_Logic);

DECLARE_CYCLE_STAT(TEXT("UI SceneHint"), STAT_UI_SceneHint, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("UI SceneHint Player"), STAT_UI_SceneHint_Player, MMOSTATGROUP_Logic);
DECLARE_CYCLE_STAT(TEXT("UI SceneHint NPC"), STAT_UI_SceneHint_NPC, MMOSTATGROUP_Logic);

