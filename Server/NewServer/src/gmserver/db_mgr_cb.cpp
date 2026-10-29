#include "stdafx.h"
#include "sol_bindings.h"
#include "db_mgr.h"
#include "gm_def.h"
#include "role_db_def.h"
#include "role.h"
#include "zlib.h"
#include "quest_data.h"
#include "gm_role_mgr.h"
#include "kernel.h"
#include "new_role_config.h"
#include "script_mgr.h"
#include "gm_log_note.h"
#include "item_data.h"
#include "message_handler.h"
#include "spell_set.h"
#include "spell_config.h"
#include "../manageserver/mail/mail.h"
#include "gmserver.h"
#include "server_config.h"
#include "gm_log.h"
#include "game_world.h"

BOOL LoadRole(CMD_ROLE_DATA_BASE *pRoleCmd, INT32 &nLength, char *pData)
{
    INT32       nRetCode = 0;
    GM_ROLE_SLOT    *pRoleSlot = NULL;
    GM_ROLE_ITEM_SLOT   *pItemSlot = NULL;
    const TITLE         *pTitle = NULL;
    CSpellSet     *pSpellSet = NULL;
    const Item          *pItem = NULL;
    const ACCEPTED_QUEST*   pQuest = NULL;
    const QUEST_DATA*       pQuestData = NULL;
    const TITLE_TEMPLATE*   pTitleTemplate = NULL;
    CRole   *poRole = NULL;

    LOG_PROCESS_ERROR(pRoleCmd);
    LOG_PROCESS_ERROR(pRoleCmd->poRole);
    LOG_PROCESS_ERROR(pData);

    poRole = pRoleCmd->poRole;

    nLength = 0;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_ID;
    *((UINT32*)(pRoleSlot->szData)) = poRole->GetObjID();
    pRoleSlot->wLength = sizeof(UINT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_DELETETIME;
    *((UINT32*)(pRoleSlot->szData)) = pRoleCmd->dwActualDeleteTime;
    pRoleSlot->wLength = sizeof(UINT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_NAME;
    strxcpy(pRoleSlot->szData, poRole->GetName(), ROLE_NAME_LEN);
    pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + strlen(pRoleSlot->szData);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_GROUPID;
    *((INT32*)(pRoleSlot->szData)) = pRoleCmd->nGroupID;
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_CLASS;
    *((INT32*)(pRoleSlot->szData)) = poRole->GetClass();
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_SEX;
    *((INT32*)(pRoleSlot->szData)) = poRole->GetSex();
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_LEVEL;
    *((INT32*)(pRoleSlot->szData)) = poRole->GetLevel();
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_EXP;
    *((INT32*)(pRoleSlot->szData)) = poRole->GetExp();
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_PKVALUE;
    *((UINT32*)(pRoleSlot->szData)) = poRole->GetPkValue();
    pRoleSlot->wLength = sizeof(UINT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_GMLEVEL;
    *((INT32*)(pRoleSlot->szData)) = poRole->GetGMLevel();
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_MAP_ID;
    *((INT32*)(pRoleSlot->szData)) = poRole->GetSceneTempID();
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_MAP_SERIAL_ID;
    *((INT32*)(pRoleSlot->szData)) = poRole->GetSceneID();
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_MAP_SERIAL_ID;
    *((INT32*)(pRoleSlot->szData)) = poRole->GetSceneID();
    pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_X;
    *((INT64*)(pRoleSlot->szData)) = poRole->GetX();
    pRoleSlot->wLength = sizeof(INT64) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_Y;
    *((INT64*)(pRoleSlot->szData)) = poRole->GetY();
    pRoleSlot->wLength = sizeof(INT64) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_Z;
    *((INT64*)(pRoleSlot->szData)) = poRole->GetZ();
    pRoleSlot->wLength = sizeof(INT64) + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_LAST_LOGOUT_TIME;
    strxcpy(pRoleSlot->szData, pRoleCmd->szSaveTime, 21);
    pRoleSlot->wLength = 21 + sizeof(GM_ROLE_SLOT);
    nLength += pRoleSlot->wLength;

    //pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    //pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    //pRoleSlot->wBaseDataType = GM_RBD_MONEY_1;
    //*((INT32*)(pRoleSlot->szData)) = poRole->GetItemBag()->GetMoney(MONEYTYPE_01, TRUE);
    //pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
    //nLength += pRoleSlot->wLength;

    for (INT32 i = MONEYTYPE_01; i < MONEYTYPE_COUNT; ++i)
    {
        pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
        pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
        pRoleSlot->wBaseDataType = GM_RBD_MONEY_1 + i;
        *((INT32*)(pRoleSlot->szData)) = poRole->GetItemBag()->GetMoney(MONEYTYPE_01 + i, TRUE);
        pRoleSlot->wLength = sizeof(INT32) + sizeof(GM_ROLE_SLOT);
        nLength += pRoleSlot->wLength;
    }

    // title
    pTitle = poRole->GetTitle().First();
    while (pTitle)
    {
        GM_ROLE_TITLE_SLOT  *pTitleSlot = NULL;
        pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
        pRoleSlot->wRoleType = GM_ROLE_TITLE;

        pTitleSlot = (GM_ROLE_TITLE_SLOT *)pRoleSlot->szData;
        pTitleSlot->nTemplateID = pTitle->m_nTemplateID;
        pTitleSlot->dwExpireTime = pTitle->m_nExpireTime;

        pTitleTemplate = CDataManager<TITLE_TEMPLATE>::GetInstance().GetData(pTitle->m_nTemplateID);
        if (pTitleTemplate)
            strxcpy(pTitleSlot->szName, pTitleTemplate->m_szName, sizeof(pTitleSlot->szName));
        else
            strxcpy(pTitleSlot->szName, "NONE", sizeof(pTitleSlot->szName));

        pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + sizeof(GM_ROLE_TITLE_SLOT);
        nLength += pRoleSlot->wLength;

        pTitle = poRole->GetTitle().Next();
    }

    // spell
    poRole->GetSpellBook().BeginSpellSet();
    pSpellSet = poRole->GetSpellBook().NextSpellSet();
    while (pSpellSet)
    {
        GM_ROLE_SPELL_SLOT  *pSpellSlot = NULL;
        pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
        pRoleSlot->wRoleType = GM_ROLE_SPELL;

        pSpellSlot = (GM_ROLE_SPELL_SLOT *)pRoleSlot->szData;
        pSpellSlot->nSpellID = pSpellSet->GetID();
        pSpellSlot->nRuneID = pSpellSet->GetRuneSlot();
        pSpellSlot->nEquipedIndex = qssSlotInvalid;
        for (INT32 i = qssActiveBegin; i < qssActiveEnd; ++i)
        {
            CSpellSlot  *pSlot = poRole->GetSpellSlot(i);
            if (pSlot && pSlot->GetMainSpellID() == pSpellSlot->nSpellID)
            {
                pSpellSlot->nEquipedIndex = i;
                break;
            }
        }

        CSpell* poSpell = CSpellConfig::GetInstance().FindSpellTemplate(pSpellSet->GetID(), pSpellSet->GetLevel(), pSpellSet->GetRuneSlot());
        if (poSpell && poSpell->GetData())
            strxcpy(pSpellSlot->szName, poSpell->GetData()->m_szName, sizeof(pSpellSlot->szName));
        else
            strxcpy(pSpellSlot->szName, "NONE", sizeof(pSpellSlot->szName));

        pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + sizeof(GM_ROLE_SPELL_SLOT);
        nLength += pRoleSlot->wLength;

        pSpellSet = poRole->GetSpellBook().NextSpellSet();
    }

    // item
    poRole->GetItemBag()->Begin();
    pItem = poRole->GetItemBag()->Next();
    while (pItem)
    {
        GM_ROLE_ITEM_SLOT   *pItemSlot = NULL;
        pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);

        pRoleSlot->wRoleType = GM_ROLE_ITEM;
        pItemSlot = (GM_ROLE_ITEM_SLOT *)pRoleSlot->szData;

        pItemSlot->dwItemID = pItem->m_iteminfo.m_nTemplateID;
        pItemSlot->qwGuid = pItem->m_iteminfo.m_llGuid;
        pItemSlot->wConSlot = pItem->m_iteminfo.m_byConSlot;
        pItemSlot->wItemSlot = pItem->m_iteminfo.m_byItemSlot;
        pItemSlot->nNum = pItem->m_iteminfo.m_nCurDur;
        if (pItem->m_pTemplate)
        {
            if (pItem->m_pTemplate->m_byDurType != DURTYPE_STACK)
            {
                pItemSlot->nNum = 1;
            }

            pItemSlot->nType = pItem->m_pTemplate->m_byType;
            pItemSlot->nSubType = pItem->m_pTemplate->m_bySubType;
            pItemSlot->byBindType = pItem->m_iteminfo.m_byBindType;

            if (pItem->m_pTemplate->m_byType == ITEMTYPE_EQUIP)
            {
                pItemSlot->byIdentified = pItem->m_iteminfo.m_byIdentified;

                for (INT32 i = 0; i < 4; ++i)
                {
                    pItemSlot->wRandomAttrID[i] = pItem->m_iteminfo.m_wRandomAttrID[i];
                }
                pItemSlot->dwRoleID = pItem->m_iteminfo.m_dwRoleID;
                pItemSlot->dwCostMoney = pItem->m_iteminfo.m_dwCostMoney;

                if (pItemSlot->nSubType == ITEMSUBTYPE_EQUIP_WEAPON)
                {
                    pItemSlot->byLuck = pItem->m_iteminfo.m_byLuck;
                    pItemSlot->byLevelupTimes = pItem->m_iteminfo.m_byLevelupTimes;
                    pItemSlot->byLevelupPhyPoint = pItem->m_iteminfo.m_byLevelupPhyPoint;
                    pItemSlot->byLevelupMagPoint = pItem->m_iteminfo.m_byLevelupMagPoint;
                    pItemSlot->byLevelupTaoPoint = pItem->m_iteminfo.m_byLevelupTaoPoint;
                    pItemSlot->byLevelupTypeHidden = pItem->m_iteminfo.m_byLevelupTypeHidden;
                    pItemSlot->byLevelupPointHidden = pItem->m_iteminfo.m_byLevelupPointHidden;

                    for (INT32 i = 0; i < 6; ++i)
                    {
                        pItemSlot->wRune[i] = pItem->m_iteminfo.m_wRune[i];
                    }
                }
                else // Armor
                {
                    pItemSlot->dwSocketCombination = pItem->m_iteminfo.m_dwSocketCombination;
                    for (INT32 i = 0; i < 3; ++i)
                    {
                        pItemSlot->nSoulID[i] = pItem->m_iteminfo.m_nSoulID[i];
                    }
                }
            }
        
            strxcpy(pItemSlot->szData, pItem->m_pTemplate->m_szName, strlen(pItem->m_pTemplate->m_szName) + 1);
            pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + sizeof(GM_ROLE_ITEM_SLOT) + strlen(pItem->m_pTemplate->m_szName) + 1;
        }
        else
        {
            pItemSlot->nType = 0;
            strxcpy(pItemSlot->szData, "NONE", strlen("NONE"));
            pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + sizeof(GM_ROLE_ITEM_SLOT) + strlen("NONE") + 1;
        }
        nLength += pRoleSlot->wLength;

        pItem = poRole->GetItemBag()->Next();
    }

    pQuest = poRole->GetQuestList().Begin();
    while (pQuest)
    {
        pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
        pRoleSlot->wRoleType = GM_ROLE_QUEST;
        *(UINT32*)(pRoleSlot->szData) = pQuest->nQuestID;
        pQuestData = CDataManager<QUEST_DATA>::GetInstance().GetData(pQuest->nQuestID);
        if (pQuestData)
        {
            strxcpy(pRoleSlot->szData + sizeof(UINT32), pQuestData->szName, strlen(pQuestData->szName) + 1);
            pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + sizeof(UINT32) + strlen(pQuestData->szName) + 1;
        }
        else
        {
            strxcpy(pRoleSlot->szData + sizeof(UINT32), "NONE", strlen("NONE") + 1);
            pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + sizeof(UINT32) + strlen("NONE") + 1;
        }
        nLength += pRoleSlot->wLength;

        pQuest = poRole->GetQuestList().Next();
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL LoadGuild(CMD_ROLE_DATA_BASE *pRoleCmd, INT32 &nLength, char *pData)
{
    GM_ROLE_SLOT    *pRoleSlot = NULL;

    LOG_PROCESS_ERROR(pRoleCmd);
    LOG_PROCESS_ERROR(pData);

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_GUILD_ID;
    *((UINT32*)pRoleSlot->szData) = pRoleCmd->dwGuildID;
    pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + sizeof(UINT32);
    nLength += pRoleSlot->wLength;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL LoadGMForbid(CMD_ROLE_DATA_BASE *pRoleCmd, INT32 &nLength, char *pData)
{
    INT32   nRetCode = 0;
    GM_ROLE_SLOT    *pRoleSlot = NULL;

    LOG_PROCESS_ERROR(pData);
    LOG_PROCESS_ERROR(pRoleCmd);

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_FORBIDDEND_CHAT_TIME;
    strxcpy(pRoleSlot->szData, pRoleCmd->szForbidChat, 21);
    pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + 21;
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_FREEZE_ROLE_TIME;
    strxcpy(pRoleSlot->szData, pRoleCmd->szFreezeRole, 21);
    pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + 21;
    nLength += pRoleSlot->wLength;

    pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
    pRoleSlot->wRoleType = GM_ROLE_BASE_DATA;
    pRoleSlot->wBaseDataType = GM_RBD_FREEZE_CHEAT_ROLE;
    strxcpy(pRoleSlot->szData, pRoleCmd->szFreezeCheatRole, 21);
    pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + 21;
    nLength += pRoleSlot->wLength;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL LoadMail(CMD_ROLE_DATA_BASE *pRoleCmd, INT32 &nLength, char *pData)
{
    GM_ROLE_SLOT        *pRoleSlot = NULL;
    GM_ROLE_MAIL_SLOT   *pMailSlot = NULL;
    
    LOG_PROCESS_ERROR(pRoleCmd);
    LOG_PROCESS_ERROR(pData);

    for (CMD_ROLE_DATA_BASE::CMD_MAIL_LIST::iterator iter = pRoleCmd->mailList.begin(); iter != pRoleCmd->mailList.end(); ++iter)
    {
        pRoleSlot = (GM_ROLE_SLOT *)(pData + nLength);
        pRoleSlot->wRoleType = GM_ROLE_MAIL;
        pMailSlot = (GM_ROLE_MAIL_SLOT*)pRoleSlot->szData;

        pMailSlot->dwMailID = iter->dwMailID;
        pMailSlot->nMailType = iter->byMailType;
        pMailSlot->nAttachType = iter->byAttachType;
        pMailSlot->nState = iter->byIsNew;
        pMailSlot->nAttachLocked = iter->byAttachLocked;

        if (pMailSlot->nAttachType == ATTACHTYPE_SIMPLE)
        {
            MailAttach  ma;
            LOG_PROCESS_ERROR(iter->dwAttachBufferLen > 0);
            memcpy(ma.buffer, iter->AttachBuffer, iter->dwAttachBufferLen);
            pMailSlot->nItemID = ma.sa.nItemID;
            pMailSlot->nItemCount = ma.sa.nItemCount;
        }
        else if (pMailSlot->nAttachType == ATTACHTYPE_NONE)
        {
            pMailSlot->nItemID = 0;
            pMailSlot->nItemCount = 0;
        }
        else //if (pMailSlot->nAttachType == ATTACHTYPE_BUFFER)
        {
            pMailSlot->nItemID = 0;
            pMailSlot->nItemCount = 0;
            LOG_CHECK_ERROR(FALSE);
        }

        strxcpy(pMailSlot->szData, iter->szMailSubject, strlen(iter->szMailSubject) + 1);
        pRoleSlot->wLength = sizeof(GM_ROLE_SLOT) + sizeof(GM_ROLE_MAIL_SLOT) + strlen(iter->szMailSubject) + 1;
        nLength += pRoleSlot->wLength;
    }

    return TRUE;
Exit0:
    return FALSE;
}

INT32 ModifyRoleData(CMD_MODIFY_ROLE *pModifyCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    CRole       *poRole = NULL;
    CRole       *poTargetRole = NULL;
    GM_ROLE_SLOT    *pSlot = NULL;
    INT32           nLength = 0;
    char*       pLogContent = NULL;

    LOG_PROCESS_ERROR(pModifyCmd);

    poRole = pModifyCmd->poSelfRole;
    poTargetRole = pModifyCmd->poTargetRole;

    LOG_PROCESS_ERROR(poRole);
    if (pModifyCmd->dwTargetRoleID != 0)
        LOG_PROCESS_ERROR(poTargetRole);

    while (nLength < pModifyCmd->nLength)
    {
        pSlot = (GM_ROLE_SLOT *)(pModifyCmd->szData + nLength);

        switch (pSlot->wRoleType)
        {
        case GM_ROLE_BASE_DATA:
        {
            switch (pSlot->wBaseDataType)
            {
            case GM_RBD_ID:
            case GM_RBD_NAME:
            case GM_RBD_CLASS:
            case GM_RBD_SEX:
                break;
            case GM_RBD_LEVEL:
            {
                INT32 * pLevel = (INT32*)pSlot->szData;
                
                GM_LOG_BUFFER   buff;
                snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] Old Level[%d], New[%d]", GM_RBD_LEVEL, poRole->GetLevel(), *pLevel);
                pModifyCmd->logList.push_back(buff);

                poRole->SetLevel(*pLevel);
            }
                break;
            case GM_RBD_EXP:
            {
                UINT32 * pExp = (UINT32*)pSlot->szData;

                GM_LOG_BUFFER   buff;
                snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] Old Exp[%d], New[%d]", GM_RBD_EXP, poRole->GetExp(), *pExp);
                pModifyCmd->logList.push_back(buff);

                poRole->AddExp(*pExp - poRole->GetExp(), FALSE, FALSE);
            }
                break;
            case GM_RBD_PKVALUE:
            {
                UINT32 * pPKValue = (UINT32*)pSlot->szData;

                GM_LOG_BUFFER   buff;
                snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] Old PkValue[%u], New[%u]", GM_RBD_PKVALUE, poRole->GetPkValue(), *pPKValue);
                pModifyCmd->logList.push_back(buff);

                poRole->SetPkValue(*pPKValue);
            }
                break;
            case GM_RBD_GMLEVEL:
            {
                INT32 * pGMLevel = (INT32*)pSlot->szData;

                GM_LOG_BUFFER   buff;
                snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] Old GMLevel[%d], New[%d]", GM_RBD_GMLEVEL, poRole->GetGMLevel(), *pGMLevel);
                pModifyCmd->logList.push_back(buff);

                poRole->SetGMLevel(*pGMLevel);
            }
                break;
            case GM_RBD_MAP_ID:
            {
                GM_LOG_BUFFER   buff;
                snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] Return to Born Point", GM_RBD_MAP_ID);
                pModifyCmd->logList.push_back(buff);

                poRole->SetSceneTempID(g_NewRoleConfig.nSceneTempID);
                poRole->SetSceneID(0);
                poRole->SetX(g_NewRoleConfig.nX * POINT_PER_CM);
                poRole->SetY(g_NewRoleConfig.nY * POINT_PER_CM);
                poRole->SetZ(g_NewRoleConfig.nZ * POINT_PER_CM);
                poRole->SetFaceDirection(g_NewRoleConfig.wFaceDirection);
            }
                break;
            case GM_RBD_X:
            case GM_RBD_Y:
            case GM_RBD_Z:
                break;
                //case GM_RBD_MONEY_1:
            case GM_RBD_MONEY_2:
            case GM_RBD_MONEY_3:
            case GM_RBD_MONEY_4:
            case GM_RBD_MONEY_5:
            case GM_RBD_MONEY_6:
            case GM_RBD_MONEY_7:
            case GM_RBD_MONEY_8:
            case GM_RBD_MONEY_9:
            case GM_RBD_MONEY_10:
            case GM_RBD_MONEY_11:
            case GM_RBD_MONEY_12:
            case GM_RBD_MONEY_13:
            case GM_RBD_MONEY_14:
            {
                INT32 * pMoney = (INT32*)pSlot->szData;
                INT32   nDiff = GM_RBD_MONEY_1 - MONEYTYPE_01;
                INT32   nCount = 0;
                INT32   bLog = FALSE;

                GM_LOG_BUFFER   buff;
                snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] MoneyType[%d], Old Money[%d], New[%d]",
                    pSlot->wBaseDataType, pSlot->wBaseDataType - nDiff, poRole->GetItemBag()->GetMoney(pSlot->wBaseDataType - nDiff), *pMoney);
                pModifyCmd->logList.push_back(buff);

                if (pSlot->wBaseDataType == GM_RBD_MONEY_2)
                {
                    if (poRole->GetItemBag()->GetMoney(MONEYTYPE_02) > *pMoney)
                    {
                        pLogContent = new char[1024 * 8];
                        LOG_PROCESS_ERROR(pLogContent);

                        bLog = TRUE;
                        nCount = poRole->GetItemBag()->GetMoney(MONEYTYPE_02) - *pMoney;

                        snprintf(pLogContent, 1024 * 8, "%s\t%d\t%d\t%u\t%u\t%s\t%d\t%d\t%s\t%d\r\n",
                            get_time_str(time(NULL)), g_ServerConfig.Common.nAreaID, 
                            poRole->GetServerGroup(), poRole->GetObjID(),
                            0, "Sub", MONEYTYPE_02, nCount,
                            CGmServer::GetInstance().GetEnumString("EMoneyLog", MONEYLOG_GM),
                            0);
                    }
                    else if (poRole->GetItemBag()->GetMoney(MONEYTYPE_02) < *pMoney)
                    {
                        pLogContent = new char[1024 * 8];
                        LOG_PROCESS_ERROR(pLogContent);

                        bLog = TRUE;
                        nCount = *pMoney - poRole->GetItemBag()->GetMoney(MONEYTYPE_02);

                        snprintf(pLogContent, 1024 * 8, "%s\t%d\t%d\t%u\t%u\t%s\t%d\t%d\t%s\t%d\r\n",
                            get_time_str(time(NULL)), g_ServerConfig.Common.nAreaID, 
                            poRole->GetServerGroup(), 0,
                            poRole->GetObjID(), "Add", MONEYTYPE_02, nCount,
                            CGmServer::GetInstance().GetEnumString("EMoneyLog", MONEYLOG_GM),
                            0);
                    }
                }

                poRole->GetItemBag()->SetMoney(pSlot->wBaseDataType - nDiff, *pMoney);

                if (bLog)
                {
                    nRetCode = CGMLog::GetInstance().WriteLogData(glfMoneyLog, pLogContent);
                    LOG_PROCESS_ERROR(nRetCode);

                    SAFE_DELETE(pLogContent);
                }
            }
                break;
            default:
                break;
            }
        }
            break;
        case GM_ROLE_QUEST:
        {
            nRetCode = poRole->GetQuestList().Abandon(*((UINT32*)pSlot->szData));
            LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

            GM_LOG_BUFFER   buff;
            snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] Del Quest[%u]", GM_ROLE_QUEST, *((UINT32*)pSlot->szData));
            pModifyCmd->logList.push_back(buff);
        }
            break;
        case GM_ROLE_ITEM:
        {
            UINT32      dwValue = *((UINT32*)pSlot->szData);
            UINT16      wConSlot = dwValue >> 16;
            UINT16      wItemSlot = dwValue;
            INT32       nItemCount = 0;
            UINT64      qwGuid = 0;
            UINT32      dwItemID = 0;

            Item *pItem = poRole->GetItemBag()->GetItem(wConSlot, wItemSlot);
            LOG_PROCESS_ERROR_RET_CODE(pItem, errItemNotFound);
            LOG_PROCESS_ERROR_RET_CODE(pItem->m_pTemplate, errItemUnknown);

            dwItemID = pItem->m_pTemplate->m_nTemplateID;
            qwGuid = pItem->m_iteminfo.m_llGuid;

            if (pItem->m_pTemplate->m_byDurType != DURTYPE_STACK)
                nItemCount = 1;
            else
                nItemCount = pItem->m_iteminfo.m_nCurDur;

            if (pItem->m_pTemplate->m_byLog)
            {
                pLogContent = new char[1024 * 8];
                LOG_PROCESS_ERROR(pLogContent);
                snprintf(pLogContent, 1024 * 8, "%s\t%d\t%d\t%u\t%s\t%I64u\t%d\t%d\t%s\t%I64u\r\n",
                    get_time_str(time(NULL)), g_ServerConfig.Common.nAreaID, poRole->GetServerGroup(),
                    poRole->GetObjID(),
                    CGmServer::GetInstance().GetEnumString("EItemActionType", ITEMACTION_DEL),
                    pItem->m_iteminfo.m_llGuid, pItem->m_iteminfo.m_nTemplateID, nItemCount,
                    CGmServer::GetInstance().GetEnumString("EItemLog", ITEMLOG_GM),
                    0);
            }

            nRetCode = poRole->GetItemBag()->DeleteItemPos(wConSlot, wItemSlot, nItemCount, EItemLog::ITEMLOG_GM, 0);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

            if (pItem->m_pTemplate->m_byLog)
            {
                nRetCode = CGMLog::GetInstance().WriteLogData(glfItemLog, pLogContent);
                LOG_PROCESS_ERROR(nRetCode);

                SAFE_DELETE(pLogContent);
            }

            GM_LOG_BUFFER   buff;
            snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] Del Item[%llu][%u]", GM_ROLE_ITEM, qwGuid, dwItemID);
            pModifyCmd->logList.push_back(buff);
        }
            break;
        case GM_ROLE_TAKE_ITEM:
        {
            UINT32      dwValue = *((UINT32*)pSlot->szData);
            UINT16      wConSlot = dwValue >> 16;
            UINT16      wItemSlot = dwValue;
            INT64       llGuid = *((INT64*)(pSlot->szData + sizeof(UINT32)));
            INT32       nItemCount = 0;

            nRetCode = poTargetRole->GetItemBag()->GetFreeSlots(wConSlot);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode > 0, errItemBagFull);

            Item *pItem = poRole->GetItemBag()->GetItem(wConSlot, wItemSlot);
            LOG_PROCESS_ERROR_RET_CODE(pItem, errItemNotFound);
            LOG_PROCESS_ERROR_RET_CODE(pItem->m_pTemplate, errItemUnknown);
            LOG_PROCESS_ERROR_RET_CODE(pItem->m_iteminfo.m_llGuid == llGuid, errItemUnknown);

            if (pItem->m_pTemplate->m_byDurType != DURTYPE_STACK)
                nItemCount = 1;
            else
                nItemCount = pItem->m_iteminfo.m_nCurDur;

            if (pItem->m_pTemplate->m_byLog)
            {
                pLogContent = new char[1024 * 8];
                LOG_PROCESS_ERROR(pLogContent);
                snprintf(pLogContent, 1024 * 8, "%s\t%d\t%d\t%u\t%s\t%I64u\t%d\t%d\t%s\t%I64u\r\n",
                    get_time_str(time(NULL)), g_ServerConfig.Common.nAreaID, poRole->GetServerGroup(),
                    poRole->GetObjID(),
                    CGmServer::GetInstance().GetEnumString("EItemActionType", ITEMACTION_DEL),
                    pItem->m_iteminfo.m_llGuid, pItem->m_iteminfo.m_nTemplateID, nItemCount,
                    CGmServer::GetInstance().GetEnumString("EItemLog", ITEMLOG_GM),
                    poTargetRole->GetObjID());
            }

            nRetCode = poRole->GetItemBag()->RemoveItem(wConSlot, wItemSlot, EItemLog::ITEMLOG_GM);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);           // todo_tq 

            if (pItem->m_pTemplate->m_byLog)
            {
                nRetCode = CGMLog::GetInstance().WriteLogData(glfItemLog, pLogContent);
                LOG_PROCESS_ERROR(nRetCode);

                SAFE_DELETE(pLogContent);
            }

            GM_LOG_BUFFER   buff;
            snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] sender[%u][%s] item[%llu][%u]", 
                GM_ROLE_TAKE_ITEM, poRole->GetObjID(), poRole->GetName(), 
                pItem->m_iteminfo.m_llGuid, pItem->m_pTemplate->m_nTemplateID);
            pModifyCmd->logList.push_back(buff);

            if (pItem->m_pTemplate->m_byLog)
            {
                pLogContent = new char[1024 * 8];
                LOG_PROCESS_ERROR(pLogContent);
                snprintf(pLogContent, 1024 * 8, "%s\t%d\t%d\t%u\t%s\t%I64u\t%d\t%d\t%s\t%I64u\r\n",
                    get_time_str(time(NULL)), g_ServerConfig.Common.nAreaID, poTargetRole->GetServerGroup(),
                    poTargetRole->GetObjID(),
                    CGmServer::GetInstance().GetEnumString("EItemActionType", ITEMACTION_ADD),
                    pItem->m_iteminfo.m_llGuid, pItem->m_iteminfo.m_nTemplateID, nItemCount,
                    CGmServer::GetInstance().GetEnumString("EItemLog", ITEMLOG_GM),
                    poRole->GetObjID());
            }

            nRetCode = poTargetRole->GetItemBag()->AddItem(pItem, EItemLog::ITEMLOG_GM, 0);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

            if (pItem->m_pTemplate->m_byLog)
            {
                nRetCode = CGMLog::GetInstance().WriteLogData(glfItemLog, pLogContent);
                LOG_PROCESS_ERROR(nRetCode);

                SAFE_DELETE(pLogContent);
            }

            memset(buff.szLog, 0, sizeof(buff.szLog));
            snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] receiver[%u][%s] item[%llu][%u]", 
                GM_ROLE_TAKE_ITEM, poTargetRole->GetObjID(), poTargetRole->GetName(), 
                pItem->m_iteminfo.m_llGuid, pItem->m_pTemplate->m_nTemplateID);
            pModifyCmd->logList.push_back(buff);
        }
            break;
        case GM_ROLE_MAIL:
            ++pModifyCmd->nSlotCount;
            break;
        default:
            LOG_PROCESS_ERROR_RET_CODE(FALSE, errGMModifyRoleTypeError);
            break;
        }

        --pModifyCmd->nSlotCount;
        nLength += pSlot->wLength;
    }

    nRetCode = CDBMgr::GetInstance().SaveRoleData(poRole);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMSaveRoleDataFailed);

    if (poTargetRole)
    {
        nRetCode = CDBMgr::GetInstance().SaveRoleData(poTargetRole);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMSaveRoleDataFailed);
    }

    SAFE_DELETE(pLogContent);
    return errSuccess;
Exit0:
    SAFE_DELETE(pLogContent);
    return nResult;
}

BOOL ExeLuaGMCmd(CMD_ROLE_EXE_GM_CMD *pCmd)
{
    INT32           nRetCode = 0;
    BOOL            bGMRet = FALSE;
    CLuaScript      *poScript = NULL;

    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pCmd->poRole);

    poScript = CScriptMgr::GetInstance().FindScript(GM_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    cqyh::lua::push_object(poScript->GetLuaState(), pCmd->poRole, "CRole");
    lua_setglobal(poScript->GetLuaState(), "TargetRole");

    nRetCode = poScript->CallFunction("OnNewGMCommand", "s:b", pCmd->szGM, &bGMRet);
    LOG_PROCESS_ERROR(nRetCode);
    //LOG_PROCESS_ERROR(bGMRet);

    nRetCode = CDBMgr::GetInstance().SaveRoleData(pCmd->poRole);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("role [%d][%s] executed gm command %s", pCmd->poRole->GetObjID(), pCmd->poRole->GetName(), pCmd->szGM);

    return TRUE;
Exit0:
    return FALSE;
}



BOOL CB_DBPreQueryRoleID(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    CMD_PRE_QUERY   *pQueryCmd = NULL;
    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (CMD_PRE_QUERY *)pCmd;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32      dwRoleID;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        pQueryCmd->dwRoleID = pLoadResult->dwRoleID;
        nRecords++;
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQueryRoleID(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    CMD_QUERY_ROLE_ID   *pQueryCmd = NULL;
    //UINT32      dwTLUserID = *((UINT32*)pUserData);
    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (CMD_QUERY_ROLE_ID *)pCmd;
    pQueryCmd->nCount = 0;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32      dwRoleID;
        char        szRoleName[ROLE_NAME_LEN];
        char        szUserName[USER_NAME_LEN];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        (it != rResult.end()) && (pQueryCmd->nCount < MAX_GMSVR_ROLE_KEY_NUM); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        pQueryCmd->RoleKey[pQueryCmd->nCount].dwRoleID = pLoadResult->dwRoleID;
        strxcpy(pQueryCmd->RoleKey[pQueryCmd->nCount].szRoleName, pLoadResult->szRoleName, ROLE_NAME_LEN);
        strxcpy(pQueryCmd->RoleKey[pQueryCmd->nCount].szUserName, pLoadResult->szUserName, USER_NAME_LEN);
        ++pQueryCmd->nCount;

        nRecords++;
    }
    ((CMD_QUERY_ROLE_ID*)pCmd)->eCmdState = GM_LS_QUERY_ROLE_NAME_ACK;
    ((CMD_QUERY_ROLE_ID*)pCmd)->nResult = errSuccess;

    INF("Query Role ID Finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}


BOOL CB_DBQueryGuildID(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    CMD_ROLE_DATA_BASE   *pRoleCmd = NULL;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pRoleCmd = (CMD_ROLE_DATA_BASE *)pCmd;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32	dwGuildID;
    };
#pragma pack(pop)

    pRoleCmd->dwGuildID = 0;
    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        pRoleCmd->dwGuildID = pLoadResult->dwGuildID;
    
        nRecords++;
        break;
    }

    pRoleCmd->eCmdState = GM_LS_QUERY_GUILD_ID_ACK;
    pRoleCmd->nResult = errSuccess;

    DBG("select guild id finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQueryGMForbid(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    CMD_ROLE_DATA_BASE   *pRoleCmd = NULL;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pRoleCmd = (CMD_ROLE_DATA_BASE *)pCmd;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT64	qwID;
        char    szTime[21];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        if ((INT32)pLoadResult->qwID == GM_RECORD_FORBID_CHAT)
        {
            strxcpy(pRoleCmd->szForbidChat, pLoadResult->szTime, 21);
        }
        else if ((INT32)pLoadResult->qwID == GM_RECORD_FREEZE_ROLE)
        {
            strxcpy(pRoleCmd->szFreezeRole, pLoadResult->szTime, 21);
        }
        else if ((INT32)pLoadResult->qwID == GM_RECORD_FREEZE_CHEAT_ROLE)
        {
            strxcpy(pRoleCmd->szFreezeCheatRole, pLoadResult->szTime, 21);
        }
        else
            LOG_CHECK_ERROR(FALSE);

        nRecords++;
    }

    pRoleCmd->eCmdState = GM_LS_QUERY_GM_FORBID_ACK;
    pRoleCmd->nResult = errSuccess;

    DBG("select guild id finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}



//BOOL CB_DBForbiddenChat(std::list<void*>& rResult, void* pUserData)
//{
//    INT32       nRetCode = 0;
//    INT32       nRecords = 0;
//    CRole       *poRole = NULL;
//    CMD_FORBIDDEN_CHAT     *pForbidChat = NULL;
//    static UINT8 s_szRoleData[65536];
//    UINT32      dwRoleDataSize = 65536;
//
//    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
//    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
//    LOG_PROCESS_ERROR(pCmd);
//    pForbidChat = (CMD_FORBIDDEN_CHAT *)pCmd;
//
//    poRole = CGMRoleMgr::GetInstance().FindRole(pForbidChat->dwRoleID);
//    LOG_PROCESS_ERROR(poRole);
//
//#pragma pack(push, 1)
//    struct LOAD_RESULT
//    {
//        UINT32	dwRoleID;
//        char	szRoleName[ROLE_NAME_LEN];
//        char	szUserName[USER_NAME_LEN];
//        UINT32	dwRoleDataSize;
//        char	szRoleData[8192];
//    };
//#pragma pack(pop)
//
//    for (std::list<void*>::iterator it = rResult.begin();
//        it != rResult.end(); ++it)
//    {
//        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
//
//        UINT32 dwDeleteTime = 0;
//        ROLE_DB_DATA* pRoleData = (ROLE_DB_DATA*)s_szRoleData;
//
//        nRetCode = uncompress(s_szRoleData, (uLongf*)&dwRoleDataSize, (const Bytef*)pLoadResult->szRoleData, pLoadResult->dwRoleDataSize);
//        LOG_PROCESS_ERROR(nRetCode == Z_OK);
//
//        nRetCode = poRole->Init(pLoadResult->dwRoleID, 0);
//        LOG_PROCESS_ERROR(nRetCode);
//
//        nRetCode = poRole->Load((const char*)s_szRoleData, dwRoleDataSize);
//        if (!nRetCode)
//        {
//            LOG_PROCESS_ERROR(pRoleData->BlockTable[0].btBlockType == rbtBase);
//
//            ROLE_DB_BASE_DATA* pBaseData = (ROLE_DB_BASE_DATA*)(pRoleData->RoleData + pRoleData->BlockTable[0].dwStartOffset);
//            dwDeleteTime = pBaseData->dwDeleteTime;
//            pBaseData->dwDeleteTime = 0;
//            if (pBaseData->byLevel == 0)
//            {
//                pRoleData->dwCRC = crc32(0, (const Bytef*)&pRoleData->dwVersion,
//                    pRoleData->dwLen - sizeof(pRoleData->dwLen) - sizeof(pRoleData->dwCRC));
//            }
//
//            nRetCode = poRole->Load((const char*)s_szRoleData, dwRoleDataSize);
//            LOG_PROCESS_ERROR(nRetCode);
//        }
//
//        nRecords++;
//        break;
//    }
//
//    //Role.SetForbidChatTime(pForbidChat->dwTime);
//
//    pForbidChat->eCmdState = 21;
//    pForbidChat->nResult = errSuccess;
//
//    INF("Forbidden Chat Finished. %u records ", nRecords);
//
//    delete (UINT32*)pUserData;
//    return TRUE;
//Exit0:
//    if (poRole)
//    {
//        CGMRoleMgr::GetInstance().DelRole(poRole);
//    }
//    delete (UINT32*)pUserData;
//    return FALSE;
//}
//

BOOL CB_DBSelectMailID(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    CMD_SEND_MAIL   *pSendMail = NULL;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pSendMail = (CMD_SEND_MAIL *)pCmd;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32	dwMailID;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        pSendMail->dwMailID = pLoadResult->dwMailID + 1;

        nRecords++;
        break;
    }

    INF("SELECT Mail ID Finished. %u records ", nRecords);

    return TRUE;
Exit0:
    //delete (UINT32*)pUserData;
    return FALSE;
}


BOOL CB_DBLoadRole(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    CMD_ROLE_DATA_BASE  *pRoleCmd = NULL;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pRoleCmd = (CMD_ROLE_DATA_BASE *)pCmd;

    LOG_PROCESS_ERROR(pRoleCmd->poRole);

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8   byGroupID;
        UINT32	dwRoleID;
        char	szRoleName[ROLE_NAME_LEN];
        char	szUserName[USER_NAME_LEN];
        char    szSaveTime[21];
        UINT32  dwDeleteTime;
        UINT32	dwRoleDataSize;
        char	szRoleData[8192];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        static UINT8 s_szRoleData[65536];
        UINT32 dwRoleDataSize = 65536;
        UINT32 dwDeleteTime = 0;
        ROLE_DB_DATA* pRoleData = (ROLE_DB_DATA*)s_szRoleData;
        INT32   dwSessionID = pLoadResult->byGroupID;

        pRoleCmd->nGroupID = pLoadResult->byGroupID;
        pRoleCmd->dwActualDeleteTime = pLoadResult->dwDeleteTime;
        strxcpy(pRoleCmd->szSaveTime, pLoadResult->szSaveTime, 21);

        nRetCode = uncompress(s_szRoleData, (uLongf*)&dwRoleDataSize, (const Bytef*)pLoadResult->szRoleData, pLoadResult->dwRoleDataSize);
        LOG_PROCESS_ERROR(nRetCode == Z_OK);

        nRetCode = pRoleCmd->poRole->Init(pLoadResult->dwRoleID, dwSessionID << 16);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = pRoleCmd->poRole->Load((const char*)s_szRoleData, dwRoleDataSize);
        if (!nRetCode)
        {
            LOG_PROCESS_ERROR(pRoleData->BlockTable[0].btBlockType == rbtBase);

            ROLE_DB_BASE_DATA* pBaseData = (ROLE_DB_BASE_DATA*)(pRoleData->RoleData + pRoleData->BlockTable[0].dwStartOffset);
            dwDeleteTime = pBaseData->dwDeleteTime;
            pBaseData->dwDeleteTime = 0;
            if (pBaseData->byLevel == 0)
            {
                pRoleData->dwCRC = crc32(0, (const Bytef*)&pRoleData->dwVersion,
                    pRoleData->dwLen - sizeof(pRoleData->dwLen) - sizeof(pRoleData->dwCRC));
            }

            nRetCode = pRoleCmd->poRole->Load((const char*)s_szRoleData, dwRoleDataSize);
            LOG_PROCESS_ERROR(nRetCode);
        }

        nRecords++;
        break;
    }
    LOG_PROCESS_ERROR(nRecords != 0);

    pRoleCmd->eCmdState = GM_LS_LOAD_ROLE_ACK;
    pRoleCmd->nResult = errSuccess;

    DBG("load role data finished. %u records ", nRecords);

    return TRUE;
Exit0:
    if (pCmd && pRoleCmd->poRole)
    {
        CGameWorld::GetInstance().DelRole(pRoleCmd->poRole);
        pRoleCmd->poRole = NULL;
    }
        
    return FALSE;
}


BOOL CB_DBLoadMail(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    CMD_ROLE_DATA_BASE   *pRoleCmd = NULL;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pRoleCmd = (CMD_ROLE_DATA_BASE *)pCmd;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32      dwMailID;
        UINT32      dwMailTime;
        UINT8       byMailType;
        UINT8       byIsNew;
        UINT8       byAttachLocked;
        char        szMailSubject[MAIL_SUBJECT_LEN];
        UINT8       byAttachType;
        UINT32      dwAttachBufferLen;
        char        AttachBuffer[BUFFER_ATTACH_LEN];
    };
#pragma pack(pop)

    pRoleCmd->nMailCount = 0;
    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        CMD_MAIL_DATA   data;

        data.dwMailID = pLoadResult->dwMailID;
        data.dwMailTime = pLoadResult->dwMailTime;
        data.byMailType = pLoadResult->byMailType;
        data.byIsNew = pLoadResult->byIsNew;
        data.byAttachLocked = pLoadResult->byAttachLocked;
        memcpy(data.szMailSubject, pLoadResult->szMailSubject, MAIL_SUBJECT_LEN);
        data.byAttachType = pLoadResult->byAttachType;
        data.dwAttachBufferLen = pLoadResult->dwAttachBufferLen;
        memcpy(data.AttachBuffer, pLoadResult->AttachBuffer, BUFFER_ATTACH_LEN);

        ++pRoleCmd->nMailCount;
        pRoleCmd->mailList.push_back(data);

        nRecords++;
    }

    pRoleCmd->eCmdState = GM_LS_LOAD_MAIL_ACK;
    pRoleCmd->nResult = errSuccess;

    DBG("load mail finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}


BOOL CB_DBRoleExeCmd(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMDBError;
    INT32       nRecords = 0;
    CRole       Role;
    CMD_ROLE_EXE_GM_CMD *pRoleExeCmd = NULL;

    CLuaScript* poScript = NULL;
    BOOL        bGMRet = FALSE;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pRoleExeCmd = (CMD_ROLE_EXE_GM_CMD *)pCmd;

    nRetCode = CGMRoleMgr::GetInstance().CheckRoleLock(pRoleExeCmd->dwRoleID, pRoleExeCmd->dwTLUserID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == TRUE, errGMRoleBeOp);

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8   byGroupID;
        UINT32	dwRoleID;
        char	szRoleName[ROLE_NAME_LEN];
        char	szUserName[USER_NAME_LEN];
        char    szSaveTime[21];
        UINT32  dwDeleteTime;
        UINT32	dwRoleDataSize;
        char	szRoleData[8192];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        static UINT8 s_szRoleData[65536];
        UINT32 dwRoleDataSize = 65536;
        UINT32 dwDeleteTime = 0;
        ROLE_DB_DATA* pRoleData = (ROLE_DB_DATA*)s_szRoleData;
        INT32   dwSessionID = pLoadResult->byGroupID;

        nRetCode = uncompress(s_szRoleData, (uLongf*)&dwRoleDataSize, (const Bytef*)pLoadResult->szRoleData, pLoadResult->dwRoleDataSize);
        LOG_PROCESS_ERROR(nRetCode == Z_OK);

        nRetCode = Role.Init(pLoadResult->dwRoleID, dwSessionID << 16);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = Role.Load((const char*)s_szRoleData, dwRoleDataSize);
        if (!nRetCode)
        {
            LOG_PROCESS_ERROR(pRoleData->BlockTable[0].btBlockType == rbtBase);

            ROLE_DB_BASE_DATA* pBaseData = (ROLE_DB_BASE_DATA*)(pRoleData->RoleData + pRoleData->BlockTable[0].dwStartOffset);
            dwDeleteTime = pBaseData->dwDeleteTime;
            pBaseData->dwDeleteTime = 0;
            if (pBaseData->byLevel == 0)
            {
                pRoleData->dwCRC = crc32(0, (const Bytef*)&pRoleData->dwVersion,
                    pRoleData->dwLen - sizeof(pRoleData->dwLen) - sizeof(pRoleData->dwCRC));
            }

            nRetCode = Role.Load((const char*)s_szRoleData, dwRoleDataSize);
            LOG_PROCESS_ERROR(nRetCode);
        }



        nRecords++;
        break;
    }

    pRoleExeCmd->eCmdState = 3;
    pRoleExeCmd->nResult = errSuccess;
    INF("Role[%u] Exe GM Cmd Success. %u records ", pRoleExeCmd->dwRoleID);

    return TRUE;
Exit0:
    if (pRoleExeCmd)
    {
        pRoleExeCmd->eCmdState = 3;
        pRoleExeCmd->nResult = nRetCode;
    }

    return FALSE;
}

BOOL CB_DBCheckRoleFreeze(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    UINT32          dwTime = 0;
    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32  dwTime;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        
        dwTime = pLoadResult->dwTime;
        nRecords++;
        break;
    }

    if (dwTime > time(NULL))
        pCmd->nResult = errSuccess;
    else
        pCmd->nResult = errGMRoleNotFreeze;
    pCmd->eCmdState = GM_LS_MS_CHECK_ROLE_FREEZE_ACK;

    DBG("select check role freeze finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQueryNameToID(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    CMD_QUERY_NAME_TO_ID   *pQueryCmd = NULL;
    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (CMD_QUERY_NAME_TO_ID *)pCmd;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32      dwRoleID;
        char        szRoleName[ROLE_NAME_LEN];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        pQueryCmd->dwRoleID = pLoadResult->dwRoleID;

        break;
    }
    ((CMD_QUERY_NAME_TO_ID*)pCmd)->eCmdState = GM_LS_QUERY_NAME_LIST_ACK;
    ((CMD_QUERY_NAME_TO_ID*)pCmd)->nResult = errSuccess;

    DBG("Query Name To ID Finished. records ");

    return TRUE;
Exit0:
    return FALSE;
}



BOOL CB_DBQueryExchangeLog(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    INT32       nSendCount = 0;
    UINT8       byGroupID = 0;
    UINT32      dwSessionID = *((UINT32*)pUserData);
    GM_EXCHANGE_PRICE_LOG   *pELog = NULL;
    BOOL        bClear = FALSE;

    char        *pBuffer = new char[MAX_EXCHANGE_BUFFER_SIZE];
    LOG_PROCESS_ERROR(pBuffer);

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8       byGroupID;
        UINT32      dwExchangeTime;
        char        szExchangeTime[21];
        UINT32      dwSellerID;
        UINT32      dwSellerOrderID;
        UINT32      dwBuyerRoleID;
        UINT32      dwBuyerOrderID;
        INT32       nPrice;
        INT32       nCount;
    };
#pragma pack(pop)

    pELog = (GM_EXCHANGE_PRICE_LOG *)pBuffer;

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        
        byGroupID = pLoadResult->byGroupID;
        pELog[nSendCount].byGroupID = pLoadResult->byGroupID;
        pELog[nSendCount].dwExchangeTime = pLoadResult->dwExchangeTime;
        memcpy(pELog[nSendCount].szExchangeTime, pLoadResult->szExchangeTime, 21);
        pELog[nSendCount].dwSellerID = pLoadResult->dwSellerID;
        pELog[nSendCount].dwSellerOrderID = pLoadResult->dwSellerOrderID;
        pELog[nSendCount].dwBuyerRoleID = pLoadResult->dwBuyerRoleID;
        pELog[nSendCount].dwBuyerOrderID = pLoadResult->dwBuyerOrderID;
        pELog[nSendCount].nPrice = pLoadResult->nPrice;
        pELog[nSendCount].nCount = pLoadResult->nCount;
        ++nSendCount;
        ++nRecords;
        
        if (nSendCount == MAX_EXCHANGE_LOG_NUM)
        {
            if (nSendCount == nRecords)
                bClear = TRUE;

            nRetCode = CMessageHandler::GetInstance().DoTLQueryExchangeLogAck(dwSessionID, pLoadResult->byGroupID, bClear,
                MAX_EXCHANGE_LOG_NUM, MAX_EXCHANGE_LOG_NUM * sizeof(GM_EXCHANGE_PRICE_LOG), pBuffer);
            LOG_CHECK_ERROR(nRetCode);
            nSendCount = 0;
            bClear = FALSE;
            memset(pBuffer, 0, MAX_EXCHANGE_BUFFER_SIZE);
        }
    }

    if (nRecords < MAX_EXCHANGE_LOG_NUM)
        bClear = TRUE;
    
    if (nSendCount != 0 )
    {
        nRetCode = CMessageHandler::GetInstance().DoTLQueryExchangeLogAck(dwSessionID, byGroupID, 
            bClear, nSendCount, nSendCount * sizeof(GM_EXCHANGE_PRICE_LOG), pBuffer);
        LOG_CHECK_ERROR(nRetCode);
    }

    INF("Query Exchange price finished. records %d", nRecords);

    delete[] pBuffer;
    return TRUE;
Exit0:
    if (pBuffer)
        delete[] pBuffer;
    return FALSE;
}

BOOL CB_DBQueryExchangeHistory(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;
    INT32       nSendCount = 0;
    UINT8       byGroupID = 0;
    UINT32      dwSessionID = *((UINT32*)pUserData);
    GM_EXCHANGE_PRICE_HISTORY   *pEHistory = NULL;
    BOOL        bClear = FALSE;

    char        *pBuffer = new char[MAX_EXCHANGE_BUFFER_SIZE];
    LOG_PROCESS_ERROR(pBuffer);

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8       byGroupID;
        UINT32      dwHistoryTime;
        char        szHistoryTime[21];
        INT32       nOpen;
        INT32       nClose;
        INT32       nHigh;
        INT32       nLow;
        INT32       nVolume;
        INT64       llTotalPrice;
    };
#pragma pack(pop)

    pEHistory = (GM_EXCHANGE_PRICE_HISTORY *)pBuffer;

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        byGroupID = pLoadResult->byGroupID;
        pEHistory[nSendCount].byGroupID = pLoadResult->byGroupID;
        pEHistory[nSendCount].dwHistoryTime = pLoadResult->dwHistoryTime;
        memcpy(pEHistory[nSendCount].szHistoryTime, pLoadResult->szHistoryTime, 21);
        pEHistory[nSendCount].nOpen = pLoadResult->nOpen;
        pEHistory[nSendCount].nClose = pLoadResult->nClose;
        pEHistory[nSendCount].nHigh = pLoadResult->nHigh;
        pEHistory[nSendCount].nLow = pLoadResult->nLow;
        pEHistory[nSendCount].nVolume = pLoadResult->nVolume;
        pEHistory[nSendCount].llTotalPrice = pLoadResult->llTotalPrice;
        ++nSendCount;
        ++nRecords;

        if (nSendCount == MAX_EXCHANGE_HISTORY_NUM)
        {
            break;
            //if (nSendCount == nRecords)
            //    bClear = TRUE;
            //nRetCode = CMessageHandler::GetInstance().DoTLQueryExchangeHistoryAck(dwSessionID, 
            //    pLoadResult->byGroupID, bClear, MAX_EXCHANGE_HISTORY_NUM, MAX_EXCHANGE_HISTORY_NUM * sizeof(GM_EXCHANGE_PRICE_HISTORY), pBuffer);
            //LOG_CHECK_ERROR(nRetCode);
            //nSendCount = 0;
            //bClear = FALSE;
            //memset(pBuffer, 0, MAX_EXCHANGE_BUFFER_SIZE);
        }
    }

    //if (nRecords < MAX_EXCHANGE_HISTORY_NUM)
        bClear = TRUE;

    if (nSendCount != 0)
    {
        nRetCode = CMessageHandler::GetInstance().DoTLQueryExchangeHistoryAck(dwSessionID, byGroupID, 
            bClear, nSendCount, nSendCount * sizeof(GM_EXCHANGE_PRICE_HISTORY), pBuffer);
        LOG_CHECK_ERROR(nRetCode);
    }

    INF("Query Exchange history finished. records %d", nRecords);

    delete[] pBuffer;
    return TRUE;
Exit0:
    if (pBuffer)
        delete[] pBuffer;
    return FALSE;
}

BOOL CB_DBQuerySysExchangeOrderInfo(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;

    QUERY_SYSTEM_EXCHANGE_INFO   *pQueryCmd = NULL;
    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (QUERY_SYSTEM_EXCHANGE_INFO *)pCmd;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT32      dwRoleID;
        INT32       nCount;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        if (pLoadResult->dwRoleID == 0)
            pQueryCmd->llSysTotalMoney += pLoadResult->nCount;
        else
            pQueryCmd->llTotalMoney += pLoadResult->nCount;

        nRecords++;
    }

    pQueryCmd->llSysTotalMoney *= 10000;
    pQueryCmd->llTotalMoney *= 10000;

    ((QUERY_SYSTEM_EXCHANGE_INFO*)pCmd)->eCmdState = GM_LS_SYS_EXCHANGE_ORDER_ACK;
    ((QUERY_SYSTEM_EXCHANGE_INFO*)pCmd)->nResult = errSuccess;

    DBG("Query system exchange order info Finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQuerySysExchangeLogInfo(std::list<void*>& rResult, void* pUserData)
{
    INT32       nRetCode = 0;
    INT32       nRecords = 0;

    INT64       llHourCount = 0;
    INT64       llHourRMB = 0;

    QUERY_SYSTEM_EXCHANGE_INFO   *pQueryCmd = NULL;
    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);

    pQueryCmd = (QUERY_SYSTEM_EXCHANGE_INFO *)pCmd;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {

        UINT32      dwTime;
        UINT32      dwSellerRoleID;
        INT32       nPrice;
        INT32       nCount;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        if (pLoadResult->dwSellerRoleID == 0)
        {
            pQueryCmd->llSysExchangeRMB += (pLoadResult->nPrice * pLoadResult->nCount);
            pQueryCmd->llSysTotalExchangeMoney += pLoadResult->nCount;
        }
        else
        {
            pQueryCmd->llExchangeRMB += (pLoadResult->nPrice * pLoadResult->nCount);
            pQueryCmd->llTotalExchangeMoney += pLoadResult->nCount;
        }

        if (pLoadResult->dwSellerRoleID == 0)
        {
            if (pLoadResult->dwTime >= pQueryCmd->dwBeginHourTime && pLoadResult->dwTime <= pQueryCmd->dwEndHourTime)
            {
                llHourCount += pLoadResult->nCount;
                llHourRMB += (pLoadResult->nCount * pLoadResult->nPrice);
            }
        }

        nRecords++;
    }

    if (pQueryCmd->llSysTotalExchangeMoney == 0)
        pQueryCmd->nAveragePriceToday = 0;
    else
        pQueryCmd->nAveragePriceToday = pQueryCmd->llSysExchangeRMB / pQueryCmd->llSysTotalExchangeMoney;

    if (llHourCount == 0)
        pQueryCmd->nAveragePriceLastHour = 0;
    else
        pQueryCmd->nAveragePriceLastHour = llHourRMB / llHourCount;

    pQueryCmd->llSysTotalExchangeMoney *= 10000;
    pQueryCmd->llTotalExchangeMoney *= 10000;

    ((QUERY_SYSTEM_EXCHANGE_INFO*)pCmd)->eCmdState = GM_LS_SYS_EXCHANGE_LOG_ACK;
    ((QUERY_SYSTEM_EXCHANGE_INFO*)pCmd)->nResult = errSuccess;

    DBG("Query system exchange log info Finished. %u records ", nRecords);

    return TRUE;
Exit0:
    return FALSE;
}









// log
BOOL CB_DBQueryLogRoleLogin(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    INT32           nLength = 0;
    CMD_LOG         *pCmdLog = NULL;
    const char      *pAction = NULL;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pCmdLog = (CMD_LOG *)pCmd;

    memset(pCmdLog->szBuffer, 0, sizeof(pCmdLog->szBuffer));

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        //INT32       nLogTime;
        char        szTime[21];
        UINT8       byGroupID;
        INT16       shAreaID;
        char        szSndaID[65];
        char        szAction[11];
        char        szIP[19];
    };
#pragma pack(pop)

    nLength = sprintf(pCmdLog->szBuffer + nLength, "%s\n", "LogTime\tGroupID\tAreaID\tSndaID\tAction\tIP\n");
    LOG_PROCESS_ERROR(nLength != -1);

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        INT32       nReadLength = 0;

        GM_LOG_MAP::iterator iter = g_GMLogMap.find(pLoadResult->szAction);
        if (iter == g_GMLogMap.end())
            pAction = pLoadResult->szAction;
        else
            pAction = iter->second.c_str();

        nReadLength = sprintf(pCmdLog->szBuffer + nLength, "%s\t%d\t%hd\t%s\t%s\t%s\n", pLoadResult->szTime, (INT32)pLoadResult->byGroupID,
            pLoadResult->shAreaID, pLoadResult->szSndaID, pAction, pLoadResult->szIP);

        LOG_PROCESS_ERROR(nReadLength != -1);
        nLength += nReadLength;

        if (nLength > MAX_ITEM_BUFFER_SIZE - 1024)
        {
            pCmdLog->bTruncated = TRUE;
            break;
        }

        nRecords++;
    }

    INF("QUERY Log Finished. Is Trencated [%d]", (INT32)pCmdLog->bTruncated);

    pCmdLog->nLength = nLength;
    pCmdLog->eCmdState = GM_LS_QUERY_LOG_ACK;
    pCmdLog->nResult = errSuccess;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQueryLogRoleLogout(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    INT32           nLength = 0;
    CMD_LOG         *pCmdLog = NULL;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pCmdLog = (CMD_LOG *)pCmd;

    memset(pCmdLog->szBuffer, 0, sizeof(pCmdLog->szBuffer));

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        //INT32       nLogTime;
        char        szLogTime[21];
        INT16       shAreaID;
        UINT8       byGroupID;
        UINT32      dwRoleID;
        char        szSndaID[65];
        INT32       nSceneTemplateID;
        INT32       nSceneID;
        char        szName[22];
        char        szClass[17];
        char        szSex[17];
        UINT8       byLevel;
        INT32       nExp;
        INT32       nMoney1;
        INT32       nMoney2;
        INT32       nX;
        INT32       nY;
        INT32       nZ;
    };
#pragma pack(pop)

    nLength = sprintf(pCmdLog->szBuffer + nLength, "%s\n", "LogTime\tGroupID\tAreaID\tSndaID\tRoleID\tSceneTempID\tSceneID\tName\tClass\tSex\tLevel\tExp\tMoney1\tMoney2\tX\tY\tZ\n");
    LOG_PROCESS_ERROR(nLength != -1);

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        INT32       nReadLength = 0;
        nReadLength = sprintf(pCmdLog->szBuffer + nLength, "%s\t%d\t%hd\t%s\t%u\t%d\t%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", 
            pLoadResult->szLogTime, (INT32)pLoadResult->byGroupID, pLoadResult->shAreaID, pLoadResult->szSndaID, pLoadResult->dwRoleID, 
            pLoadResult->nSceneTemplateID, pLoadResult->nSceneID, pLoadResult->szName, pLoadResult->szClass, pLoadResult->szSex, 
            (INT32)pLoadResult->byLevel, pLoadResult->nExp, pLoadResult->nMoney1, pLoadResult->nMoney2, pLoadResult->nX, pLoadResult->nY, pLoadResult->nZ);

        LOG_PROCESS_ERROR(nReadLength != -1);
        nLength += nReadLength;

        if (nLength > MAX_ITEM_BUFFER_SIZE - 2048)
        {
            pCmdLog->bTruncated = TRUE;
            break;
        }

        nRecords++;
    }

    INF("QUERY Log Finished. Is Trencated [%d]", (INT32)pCmdLog->bTruncated);

    pCmdLog->nLength = nLength;
    pCmdLog->eCmdState = GM_LS_QUERY_LOG_ACK;
    pCmdLog->nResult = errSuccess;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQueryLogItem(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    INT32           nLength = 0;
    CMD_LOG         *pCmdLog = NULL;
    const char      *pActionType = NULL;
    const char      *pReason = NULL;
    const   ITEM_DATA  *pItemTemplate = NULL;
    const   EQUIP_DATA  *pEquipmentTemplate = NULL;

    UINT32      dwRoleID = 0;
    UINT32      dwItemID = 0;
    UINT64      qwGuid = 0;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pCmdLog = (CMD_LOG *)pCmd;

    memset(pCmdLog->szBuffer, 0, sizeof(pCmdLog->szBuffer));


    sscanf(pCmdLog->szQueryStr, "%u %u %llu", &dwRoleID, &dwItemID, &qwGuid);

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        //INT32       nLogTime;
        char        szLogTime[21];
        UINT8       byGroupID;
        INT16       shAreaID;
        UINT32      dwRoleID;
        char        szActionType[21];
        UINT64      qwGuid;
        UINT32      dwTemplateID;
        INT32       nItemCount;
        char        szReason[31];
        UINT64      qwUserDefinedID;
    };
#pragma pack(pop)

    nLength = sprintf(pCmdLog->szBuffer + nLength, "%s\n", "LogTime\tGroupID\tAreaID\tRoleID\tActionType\tGuid\tTemplateID\tItemCount\tReason\tUserDefineID\n");
    LOG_PROCESS_ERROR(nLength != -1);

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        INT32       nReadLength = 0;

        if ((qwGuid != 0) && (pLoadResult->qwGuid != qwGuid))
            continue;
        if ((dwItemID != 0) && (pLoadResult->dwTemplateID != dwItemID))
            continue;
        if ((dwRoleID != 0) && (pLoadResult->dwRoleID != dwRoleID))
            continue;

        GM_LOG_MAP::iterator iter = g_GMLogMap.find(pLoadResult->szActionType);
        if (iter == g_GMLogMap.end())
            pActionType = pLoadResult->szActionType;
        else
            pActionType = iter->second.c_str();

        iter = g_GMLogMap.find(pLoadResult->szReason);
        if (iter == g_GMLogMap.end())
            pReason = pLoadResult->szReason;
        else
            pReason = iter->second.c_str();

        pItemTemplate = CDataManager<ITEM_DATA>::GetInstance().GetData(pLoadResult->dwTemplateID);
        if (pItemTemplate)
        {
            nReadLength = sprintf(pCmdLog->szBuffer + nLength, "%s\t%d\t%hd\t%u\t%s\t%llu\t%s\t%d\t%s\t%llu\n", pLoadResult->szLogTime, (INT32)pLoadResult->byGroupID,
                pLoadResult->shAreaID, pLoadResult->dwRoleID, pActionType, pLoadResult->qwGuid, pItemTemplate->m_szName, pLoadResult->nItemCount,
                pReason, pLoadResult->qwUserDefinedID);
        }
        else
        {
            pEquipmentTemplate = CDataManager<EQUIP_DATA>::GetInstance().GetData(pLoadResult->dwTemplateID);
            if (pEquipmentTemplate)
            {
                nReadLength = sprintf(pCmdLog->szBuffer + nLength, "%s\t%d\t%hd\t%u\t%s\t%llu\t%s\t%d\t%s\t%llu\n", pLoadResult->szLogTime, (INT32)pLoadResult->byGroupID,
                    pLoadResult->shAreaID, pLoadResult->dwRoleID, pActionType, pLoadResult->qwGuid, pEquipmentTemplate->m_szName, pLoadResult->nItemCount,
                    pReason, pLoadResult->qwUserDefinedID);
            }
            else
            {
                nReadLength = sprintf(pCmdLog->szBuffer + nLength, "%s\t%d\t%hd\t%u\t%s\t%llu\t%u\t%d\t%s\t%llu\n", pLoadResult->szLogTime, (INT32)pLoadResult->byGroupID,
                    pLoadResult->shAreaID, pLoadResult->dwRoleID, pLoadResult->szActionType, pLoadResult->qwGuid, pLoadResult->dwTemplateID, pLoadResult->nItemCount,
                    pLoadResult->szReason, pLoadResult->qwUserDefinedID);
            }
        }

        LOG_PROCESS_ERROR(nReadLength != -1);
        nLength += nReadLength;

        if (nLength > MAX_ITEM_BUFFER_SIZE - 2048)
        {
            pCmdLog->bTruncated = TRUE;
            break;
        }

        nRecords++;
    }

    INF("QUERY Log Finished. Is Trencated [%d]", (INT32)pCmdLog->bTruncated);

    pCmdLog->nLength = nLength;
    pCmdLog->eCmdState = GM_LS_QUERY_LOG_ACK;
    pCmdLog->nResult = errSuccess;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQueryLogMoney(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    INT32           nLength = 0;
    CMD_LOG         *pCmdLog = NULL;
    const char      *pAction = NULL;
    const char      *pReason = NULL;

    UINT32          dwRoleID = 0;
    UINT32          dwTargetRoleID = 0;
    INT32           nMoneyType = 0;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pCmdLog = (CMD_LOG *)pCmd;

    memset(pCmdLog->szBuffer, 0, sizeof(pCmdLog->szBuffer));
    sscanf(pCmdLog->szQueryStr, "%u %u %d", &dwRoleID, &dwTargetRoleID, &nMoneyType);

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        //INT32       nLogTime;
        char        szLogTime[21];
        UINT8       byGroupID;
        INT16       shAreaID;
        UINT32      dwSourceID;
        UINT32      dwTargetID;
        char        szReason[33];
        INT8        cParam;
        char        szAction[5];
        INT8        cMoneyType;
        INT32       nMoneyCount;
    };
#pragma pack(pop)

    nLength = sprintf(pCmdLog->szBuffer + nLength, "%s\n", "LogTime\tGroupID\tAreaID\tSourceID\tTargetID\tReason\tParam\tAction\tMoneyType\tMoneyCount\n");
    LOG_PROCESS_ERROR(nLength != -1);

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        INT32       nReadLength = 0;

        if ((nMoneyType != 0) && (pLoadResult->cMoneyType != nMoneyType))
            continue;
        if ((dwTargetRoleID != 0) && (pLoadResult->dwTargetID != dwTargetRoleID))
            continue;
        if ((dwRoleID != 0) && (pLoadResult->dwSourceID != dwRoleID))
            continue;

        GM_LOG_MAP::iterator iter = g_GMLogMap.find(pLoadResult->szAction);
        if (iter == g_GMLogMap.end())
            pAction = pLoadResult->szAction;
        else
            pAction = iter->second.c_str();

        iter = g_GMLogMap.find(pLoadResult->szReason);
        if (iter == g_GMLogMap.end())
            pReason = pLoadResult->szReason;
        else
            pReason = iter->second.c_str();

        nReadLength = sprintf(pCmdLog->szBuffer + nLength, "%s\t%d\t%hd\t%u\t%u\t%s\t%d\t%s\t%d\t%d\n", pLoadResult->szLogTime, (INT32)pLoadResult->byGroupID,
            pLoadResult->shAreaID, pLoadResult->dwSourceID, pLoadResult->dwTargetID, pReason, (INT32)pLoadResult->cParam, pAction,
            (INT32)pLoadResult->cMoneyType, pLoadResult->nMoneyCount);

        LOG_PROCESS_ERROR(nReadLength != -1);
        nLength += nReadLength;

        if (nLength > MAX_ITEM_BUFFER_SIZE - 2048)
        {
            pCmdLog->bTruncated = TRUE;
            break;
        }

        nRecords++;
    }

    INF("QUERY Log Finished. Is Trencated [%d]", (INT32)pCmdLog->bTruncated);

    pCmdLog->nLength = nLength;
    pCmdLog->eCmdState = GM_LS_QUERY_LOG_ACK;
    pCmdLog->nResult = errSuccess;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CB_DBQueryLogQuest(std::list<void*>& rResult, void* pUserData)
{
    INT32           nRetCode = 0;
    INT32           nRecords = 0;
    INT32           nLength = 0;
    CMD_LOG         *pCmdLog = NULL;
    const char      *pAction = NULL;
    const char      *pReason = NULL;

    UINT32          dwRoleID = 0;
    INT32           nQuestID = 0;

    UINT32 dwCmdSerialID = *((UINT32*)pUserData);
    CMD_DATA_BASE *pCmd = CKernel::GetInstance().FindCmd(dwCmdSerialID);
    LOG_PROCESS_ERROR(pCmd);
    pCmdLog = (CMD_LOG *)pCmd;

    memset(pCmdLog->szBuffer, 0, sizeof(pCmdLog->szBuffer));
    sscanf(pCmdLog->szQueryStr, "%u %d", &dwRoleID, &nQuestID);

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        //INT32       nLogTime;
        char        szLogTime[21];
        UINT8       byGroupID;
        INT16       shAreaID;
        UINT32      dwRoleID;
        UINT32      dwQuestID;
        char        szAction[9];
        char        szType[7];
        char        szQuestName[21];
    };
#pragma pack(pop)

    nLength = sprintf(pCmdLog->szBuffer + nLength, "%s\n", "LogTime\tGroupID\tAreaID\tRoleID\tQuestID\tAction\tType\tQuestName\n");
    LOG_PROCESS_ERROR(nLength != -1);

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;
        INT32       nReadLength = 0;

        if ((nQuestID != 0) && (pLoadResult->dwQuestID != nQuestID))
            continue;
        if ((dwRoleID != 0) && (pLoadResult->dwRoleID != dwRoleID))
            continue;

        GM_LOG_MAP::iterator iter = g_GMLogMap.find(pLoadResult->szAction);
        if (iter == g_GMLogMap.end())
            pAction = pLoadResult->szAction;
        else
            pAction = iter->second.c_str();

        iter = g_GMLogMap.find(pLoadResult->szType);
        if (iter == g_GMLogMap.end())
            pReason = pLoadResult->szType;
        else
            pReason = iter->second.c_str();

        nReadLength = sprintf(pCmdLog->szBuffer + nLength, "%s\t%d\t%hd\t%u\t%u\t%s\t%s\t%s\n", pLoadResult->szLogTime, (INT32)pLoadResult->byGroupID,
            pLoadResult->shAreaID, pLoadResult->dwRoleID, pLoadResult->dwQuestID, pLoadResult->szAction, pLoadResult->szType, pLoadResult->szQuestName);

        LOG_PROCESS_ERROR(nReadLength != -1);
        nLength += nReadLength;

        if (nLength > MAX_ITEM_BUFFER_SIZE - 2048)
        {
            pCmdLog->bTruncated = TRUE;
            break;
        }

        nRecords++;
    }

    INF("QUERY Log Finished. Is Trencated [%d]", (INT32)pCmdLog->bTruncated);

    pCmdLog->nLength = nLength;
    pCmdLog->eCmdState = GM_LS_QUERY_LOG_ACK;
    pCmdLog->nResult = errSuccess;

    return TRUE;
Exit0:
    return FALSE;
}








