//  Allan Tao[5.8.2014] 
#include "stdafx.h"
#include "ms_post_process_data.h"

#include "master_gift.h"
#include "mail_template_data.h"
#include "game_world_config.h"
#include "ranklist/ranklist.h"
#include "achievement_data.h"
#include "guild_achievement.h"
#include "../guild/ms_guild_achi_mgr.h"
#include "guild_achievement_cdtn.h"

BOOL g_ProcessMasterGift(MS_MASTER_GIFT_TEMPLATE* pData, BOOL bInit)
{
    const MAIL_TEMPLATE_DATA    *pMailTemplate = NULL;
    if (bInit)
    {
        if (g_GWConfig.nMasterGradGiftMailIDForS > 0)
        {
            pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(g_GWConfig.nMasterGradGiftMailIDForS);
            LOG_PROCESS_ERROR(pMailTemplate);
        }
        if (g_GWConfig.nMasterGradGiftMailIDForT > 0)
        {
            pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(g_GWConfig.nMasterGradGiftMailIDForT);
            LOG_PROCESS_ERROR(pMailTemplate);
        }
        if (pData->m_dwLvlUpStudMailID > 0)
        {
            pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(pData->m_dwLvlUpStudMailID);
            LOG_PROCESS_ERROR(pMailTemplate);
        }
        if (pData->m_dwLvlUpTeacherMailID > 0)
        {
            pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(pData->m_dwLvlUpTeacherMailID);
            LOG_PROCESS_ERROR(pMailTemplate);
        }

        for (INT32 i = 0; i < MASTER_CONG_LVL_UP_TYPE_NUM; ++i)
        {
            if (pData->m_arrCZT2STeacherMailID[i] > 0)
            {
                pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(pData->m_arrCZT2STeacherMailID[i]);
                LOG_PROCESS_ERROR(pMailTemplate);
            }
            if (pData->m_arrCZT2SStudentMailID[i] > 0)
            {
                pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(pData->m_arrCZT2SStudentMailID[i]);
                LOG_PROCESS_ERROR(pMailTemplate);
            }

            if (pData->m_arrCZS2TTeacherMailID[i] > 0)
            {
                pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(pData->m_arrCZS2TTeacherMailID[i]);
                LOG_PROCESS_ERROR(pMailTemplate);
            }
            if (pData->m_arrCZS2TStudentMailID[i])
            {
                pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(pData->m_arrCZS2TStudentMailID[i]);
                LOG_PROCESS_ERROR(pMailTemplate);
            }

            if (pData->m_arrCZC2CSelfMailID[i] > 0)
            {
                pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(pData->m_arrCZC2CSelfMailID[i]);
                LOG_PROCESS_ERROR(pMailTemplate);
            }
            if (pData->m_arrCZC2CTargetMailID[i] > 0)
            {
                pMailTemplate = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(pData->m_arrCZC2CTargetMailID[i]);
                LOG_PROCESS_ERROR(pMailTemplate);
            }
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL g_ProcessAchievement(ACHIEVEMENT_DATA* pData, BOOL bInit)
{
	LOG_PROCESS_ERROR(pData);

	if (bInit)
	{
		if (pData->vecAchievementType[0] == adRanklist)
			CRankListMgr::GetInstance().AddRankAchievementID(pData->nAchievementID);
	}
	else
	{
		if (pData->vecAchievementType[0] == adRanklist)
			CRankListMgr::GetInstance().DelRankAchievementID(pData->nAchievementID);
	}

    return TRUE;
Exit0:
    return FALSE;
}

BOOL g_ProcessGuildAchievement(GUILD_ACHIEVEMENT *pData, BOOL bInit)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pData);

    if (bInit == FALSE)
    {
        if ( 0 == pData->nID)
        {
            nRetCode = CMSGuildAchiMgr::GetInstance().UnInit();
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    for (INT32 i = 0; i < GuildAchiCdtnTableSize; ++i)
    {
        if (pData->CdtnData[i].nCdtnType == 0)
            continue;
        LOG_PROCESS_ERROR(pData->CdtnData[i].nParam1 >= 0 && pData->CdtnData[i].nParam1 <= 32000);
    }

    for (INT32 i = 0; i < GuildAchiCdtnTableSize; ++i)
    {
        if (pData->CdtnData[i].nCdtnType == 0)
            continue;
        nRetCode = CMSGuildAchiMgr::GetInstance().AddAchiType(pData->CdtnData[i].nCdtnType, pData->CdtnData[i].nParam1, pData->nID);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL g_ProcessGuildAchiCdtn(GUILD_ACHI_CDTN *pData, BOOL bInit)
{
    LOG_PROCESS_ERROR(pData);

    LOG_PROCESS_ERROR(pData->nParam1 >= 0 && pData->nParam1 <= 32000);
    LOG_PROCESS_ERROR(pData->nPos >= 0 && pData->nPos <= 7);

    return TRUE;
Exit0:
    return FALSE;
}
