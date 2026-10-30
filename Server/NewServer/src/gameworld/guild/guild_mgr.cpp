#include "stdafx.h"
#include "guild_mgr.h"

#include "guild_config.h"

#include "guild_welfare_data.h"
#include "guild_building_data.h"
#include "message_handler.h"
#include "drop_group_data.h"
#include "guild_dungeon_data.h"
#include "drop_generator.h"
#include "scene.h"
#include "npc.h"
#include "game_world.h"
#include "spell_config.h"
#include "global_config.h"
#include "scene_file_mgr.h"

CGuildMgr CGuildMgr::m_Instance_;

CGuildMgr& CGuildMgr::GetInstance()
{
    return m_Instance_;
}


CGuildMgr::CGuildMgr()
{

}

CGuildMgr::~CGuildMgr()
{

}

BOOL CGuildMgr::Init()
{
    INT32 nRetCode = 0;

    m_warList.clear();

    nRetCode = m_GuildPool.Init(g_GuildConfig.nInitGuildCount);
    LOG_PROCESS_ERROR(nRetCode);


    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGuildMgr::UnInit()
{
    INT32   nRetCode = 0;

    nRetCode = m_GuildPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    m_warList.clear();

    return TRUE;
}

CGSGuild *CGuildMgr::NewGuild(UINT32 dwGuildID)
{
    return m_GuildPool.New(dwGuildID);
}

BOOL CGuildMgr::DelGuild(CGSGuild *poGuild)
{
    return m_GuildPool.Delete(poGuild);
}

CGSGuild *CGuildMgr::FindGuild(UINT32 dwGuildID)
{
    return m_GuildPool.Find(dwGuildID);
}


BOOL CGuildMgr::AddWarRelation(UINT32 dwIntruderID, UINT32 dwDefenderID)
{
    GUILD_WAR_DATA  data;
    data.dwIntruderID = dwIntruderID;
    data.dwDefenderID = dwDefenderID;
    m_warList.push_back(data);

    return TRUE;
}

GUILD_WAR_DATA *CGuildMgr::FindWarRelation(UINT32 dwIntruderID, UINT32 dwDefenderID)
{
    GS_WAR_LIST::iterator   end_iter  = m_warList.end();
    for (GS_WAR_LIST::iterator iter = m_warList.begin(); iter != end_iter; ++iter)
    {
        if ((iter->dwIntruderID == dwIntruderID && iter->dwDefenderID == dwDefenderID) 
            || (iter->dwIntruderID == dwDefenderID && iter->dwDefenderID == dwIntruderID))
        {
            return &(*iter);
        }
    }
    return NULL;
}

GUILD_WAR_DATA *CGuildMgr::FindWarByAttackGuildID(UINT32 dwIntruderID)
{
    GS_WAR_LIST::iterator   end_iter  = m_warList.end();
    for (GS_WAR_LIST::iterator iter = m_warList.begin(); iter != end_iter; ++iter)
    {
        if ( iter->dwIntruderID == dwIntruderID )
        {
            return &(*iter);
        }
    }
    return NULL;
}

GUILD_WAR_DATA *CGuildMgr::FindWarByDefendGuildID(UINT32 dwDefenderID)
{
    GS_WAR_LIST::iterator   end_iter  = m_warList.end();
    for (GS_WAR_LIST::iterator iter = m_warList.begin(); iter != end_iter; ++iter)
    {
        if ( iter->dwDefenderID == dwDefenderID )
        {
            return &(*iter);
        }
    }
    return NULL;
}

BOOL CGuildMgr::DelWarRelation(UINT32 dwIntruderID, UINT32 dwDefenderID)
{
    GS_WAR_LIST::iterator   end_iter  = m_warList.end();
    for (GS_WAR_LIST::iterator iter = m_warList.begin(); iter != end_iter; ++iter)
    {
        if (iter->dwIntruderID == dwIntruderID && iter->dwDefenderID == dwDefenderID)
        {
            m_warList.erase(iter);
            break;
        }
    }

    return TRUE;
}

BOOL CGuildMgr::IsHostile(UINT32 dwGuildID, UINT32 dwTargetGuildID)
{
    INT32       nRetCode = 0;
    CGSGuild    *poGSGuild = NULL;
    GUILD_WAR_DATA *pWarData = NULL;

    poGSGuild = FindGuild(dwGuildID);
    LOG_PROCESS_ERROR(poGSGuild);

    nRetCode = poGSGuild->IsEnemy(dwTargetGuildID);
    PROCESS_SUCCESS(nRetCode);

    pWarData = FindWarRelation(dwGuildID, dwTargetGuildID);
    PROCESS_SUCCESS(pWarData);

Exit0:
    return FALSE;
Exit1:
    return TRUE;
}

BOOL CGuildMgr::IsFriendly(UINT32 dwGuildID, UINT32 dwTargetGuildID)
{
    INT32       nRetCode = 0;
    CGSGuild    *poGSGuild = NULL;

    poGSGuild = FindGuild(dwGuildID);
    LOG_PROCESS_ERROR(poGSGuild);

    nRetCode = poGSGuild->IsAlliance(dwTargetGuildID);
    PROCESS_SUCCESS(nRetCode);

Exit0:
    return FALSE;
Exit1:
    return TRUE;
}










BOOL CloseGuildDungeonCommission(UINT32 dwSceneTemplateID, UINT32 dwSceneID)
{
    INT32   nRetCode = 0;
    DROP_CHANCE_DATA        stDropChangeData[GuildDungeonDropGroupCount] ;
    UINT32  arrItemID[GuildDungeonDropGroupCount];
    INT32   arrItemCount[GuildDungeonDropGroupCount];
    INT32   nItemIDNum = 0;

    CGSGuild* poGuild = NULL;

    const GUILD_DUNGEON_DATA    *pGuildDungeon = GetGuildDungeonData(dwSceneTemplateID);
    LOG_PROCESS_ERROR(pGuildDungeon);

    poGuild = CGuildMgr::GetInstance().FindGuild(dwSceneID);
    LOG_PROCESS_ERROR(poGuild);

    memset(stDropChangeData, 0, sizeof(stDropChangeData));
    memset(arrItemID, 0, sizeof(arrItemID));
    memset(arrItemCount, 0, sizeof(arrItemCount));

    for (INT32 i = 0; i < GuildDungeonDropGroupCount; ++i)
    {
        stDropChangeData[i].nDropChance = pGuildDungeon->arrDropGroup[i].nChance;
        stDropChangeData[i].nDropGroupID = pGuildDungeon->arrDropGroup[i].dwGroupID;
    }

#ifndef _ROBOT
    nRetCode = CDropGenerator::GetInstance().GenerateGuildDungeonDrops(poGuild->GetServerGroup(), stDropChangeData, GuildDungeonDropGroupCount, arrItemID, arrItemCount, nItemIDNum);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoMSGuildCloseDungeonCommission(dwSceneTemplateID, dwSceneID, arrItemID, arrItemCount, nItemIDNum);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL GuildWarLockRole(CScene *poScene)
{
    COMBAT_STAT_NODE* pStatNode = NULL;
    CRole   *poRole = NULL;
    CAura* pAura = NULL;
    CAuraInstance* pAuraInstance = NULL;

    PROCESS_SUCCESS(poScene == NULL);

    pStatNode = (COMBAT_STAT_NODE*)poScene->GetCombatStatList().Head().pNext;
    while(pStatNode != &(poScene->GetCombatStatList().Rear()))
    {
        poRole = CGameWorld::GetInstance().FindRole(pStatNode->dwRoleID);
        if (poRole)
        {
            if (poRole->GetGuildID() == poScene->GetSceneID())
            {
                pAura = CSpellConfig::GetInstance().FindAuraTemplate(g_GuildConfig.nGuildWarDefendEndAuraID);
                if (pAura)
                {
                    pAuraInstance = poRole->AppendAura(pAura, poRole, poRole->GetObjID(), NULL, 0, 0, 0, 0);
                    LOG_CHECK_ERROR(pAuraInstance);
                }
            }
            else
            {
                pAura = CSpellConfig::GetInstance().FindAuraTemplate(g_GuildConfig.nGuildWarAttackEndAuraID);
                if (pAura)
                {
                    pAuraInstance = poRole->AppendAura(pAura, poRole, poRole->GetObjID(), NULL, 0, 0, 0, 0);
                    LOG_CHECK_ERROR(pAuraInstance);
                }
            }
        }

        pStatNode = (COMBAT_STAT_NODE*)pStatNode->pNext;
    }

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL SendMSToAddRes(UINT32 dwRoleID, INT32 nType, INT32 nCount)
{
#ifndef _ROBOT
	return CCliMessageHandler::GetInstance().DoMSGuildContribute(dwRoleID, nType, nCount);
#else
	return FALSE;
#endif
}







