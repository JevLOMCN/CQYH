#include "stdafx.h"
#include "guild_storage.h"

#include <time.h>

#include "time_ex.h"

#include "item_def.h"
#include "../gameworld/item.h"
#include "../gameworld/item_bag.h"
#include "guild_def.h"
#include "error_def.h"

#include "manageserver.h"
#include "role_data.h"
#include "ms_guild_mgr.h"

#include "message_handler.h"
#include "guild_config.h"
#include "server_config.h"
#include "db/ms_sql_stmt_list.h"
#include "sql_mgr.h"
#include "db/pre_save_list.h"
#include "item_data.h"

CMsGuildStorage::CMsGuildStorage()
{
    m_pOwnerGuild = NULL;
    m_nContainerCursor = 0;
    m_nItemCursor = 0;
    m_nCD = 0;
    memset(m_aaItemBuffer, 0, sizeof(m_aaItemBuffer));
    m_nHistoryNum = 0;
}

CMsGuildStorage::~CMsGuildStorage()
{
    m_pOwnerGuild = NULL;
    m_nContainerCursor = 0;
    m_nItemCursor = 0;
    m_nCD = 0;
    memset(m_aaItemBuffer, 0, sizeof(m_aaItemBuffer));
    m_nHistoryNum = 0;
}

BOOL CMsGuildStorage::Init(CMsGuild *pMsGuild)
{
    INT32   nRetCode = 0;

    m_nCD = 0;
    LOG_PROCESS_ERROR(pMsGuild);
    memset(m_aaItemBuffer, 0, sizeof(m_aaItemBuffer));
    m_History.Clear();
    m_nHistoryNum = 0;
    m_pOwnerGuild = pMsGuild;

    m_nContainerCursor = 0;
    m_nItemCursor = 0;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildStorage::UnInit()
{
    memset(m_aaItemBuffer, 0, sizeof(m_aaItemBuffer));
    m_nCD = 0;
    m_History.Clear();
    m_nHistoryNum = 0;
    m_pOwnerGuild = NULL;

    m_nContainerCursor = 0;
    m_nItemCursor = 0;

    return TRUE;
}

INT32 CMsGuildStorage::PrePushItem(INT32 nConSlot, INT32 nItemSlot)
{
    INT32  nResult = errGuildFailed;

    LOG_PROCESS_ERROR(m_pOwnerGuild);
    LOG_PROCESS_ERROR_RET_CODE(nConSlot >= 0 && nConSlot < m_pOwnerGuild->GetLevel() && nConSlot < GUILD_STORAGE_MAX_TAG_NUM, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nItemSlot >= 0 && nItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG, errGuildStorageTargetSlotError);

    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byItemSize == 0, errGuildStorageSlotUsed);
    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byConSlot != 0xFF, errGuildStorageSlotUsed);

    m_aaItemBuffer[nConSlot][nItemSlot].byConSlot = 0xFF;
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSlot = 0xFF;
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSize = 0;

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuildStorage::UnPrePushItem(INT32 nConSlot, INT32 nItemSlot)
{
    INT32   nResult  = errUnknownError;

    LOG_PROCESS_ERROR(m_pOwnerGuild);
    LOG_PROCESS_ERROR_RET_CODE(nConSlot >= 0 && nConSlot < m_pOwnerGuild->GetLevel() && nConSlot < GUILD_STORAGE_MAX_TAG_NUM, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nItemSlot >= 0 && nItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG, errGuildStorageTargetSlotError);

    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byConSlot == 0xFF, errSuccess);

    m_aaItemBuffer[nConSlot][nItemSlot].byConSlot = 0;
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSlot = 0;

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuildStorage::PushItem(const MS_ROLE_DATA *pRoleData, INT32 nConSlot, INT32 nItemSlot, INT32 nBufferSize, const char *pBuffer)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pBuffer);
    LOG_PROCESS_ERROR(pRoleData);
    LOG_PROCESS_ERROR(m_pOwnerGuild);

    LOG_PROCESS_ERROR_RET_CODE(nConSlot >= 0 && nConSlot < m_pOwnerGuild->GetLevel() && nConSlot < GUILD_STORAGE_MAX_TAG_NUM, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nItemSlot >= 0 && nItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR(0 < nBufferSize && nBufferSize < sizeof(m_aaItemBuffer[nConSlot][nItemSlot].szItemBuffer));

    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byConSlot == 0xFF
        && m_aaItemBuffer[nConSlot][nItemSlot].byItemSlot == 0xFF
        && m_aaItemBuffer[nConSlot][nItemSlot].byItemSize == 0, errGuildStorageSlotUsed);

    memset(m_aaItemBuffer[nConSlot][nItemSlot].szItemBuffer, 0, sizeof(m_aaItemBuffer[nConSlot][nItemSlot].szItemBuffer));
    memcpy(m_aaItemBuffer[nConSlot][nItemSlot].szItemBuffer, pBuffer, nBufferSize);
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSize = nBufferSize;
    m_aaItemBuffer[nConSlot][nItemSlot].byConSlot = nConSlot;
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSlot = nItemSlot;

    nRetCode = _AddStorageEvent(STORAGE_HE_TYPE_PUSH, pRoleData->dwRoleID, nBufferSize, m_aaItemBuffer[nConSlot][nItemSlot].szItemBuffer);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);
    // update mysql
    nRetCode = SqlUpdateGuild(m_pOwnerGuild);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    {
        INT32       nItemCount = 0;
        ItemInfo    tempItemInfo;
        UINT32      tempItemSize = tempItemInfo.Load(pBuffer);
        LOG_CHECK_ERROR(tempItemSize == nBufferSize);

        if (tempItemSize == nBufferSize)
        {
            const ITEM_DATA* itemdata = NULL;
            LOG_PROCESS_ERROR(tempItemInfo.m_nTemplateID >= 99);
            itemdata = GetItemData(tempItemInfo.m_nTemplateID);
            LOG_PROCESS_ERROR(itemdata);

            nItemCount = itemdata->m_byDurType == DURTYPE_STACK ? tempItemInfo.m_nCurDur : 1;

            if (itemdata->m_byLog)
            {
                nRetCode = CMessageHandler::GetInstance().DoDLLogItem(
                    m_pOwnerGuild->GetID(),
                    pRoleData->byGroup,
                    tempItemInfo.m_llGuid,
                    tempItemInfo.m_nTemplateID,
                    nItemCount,
                    ITEMLOG_GUILD_STORAGE,
                    ITEMACTION_ADD,
                    pRoleData->dwRoleID);
                LOG_CHECK_ERROR(nRetCode);
            }

            if (itemdata->m_byStat)
            {
                nRetCode = CMessageHandler::GetInstance().DoDLStatItem(m_pOwnerGuild->GetSvrGroup(), tempItemInfo.m_nTemplateID, nItemCount, 1);
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuildStorage::PrePopItem(INT32 nConSlot, INT32 nItemSlot)
{
    INT32  nResult = errUnknownError;

    LOG_PROCESS_ERROR(m_pOwnerGuild);

    LOG_PROCESS_ERROR_RET_CODE(nConSlot >= 0 && nConSlot < m_pOwnerGuild->GetLevel() && nConSlot < GUILD_STORAGE_MAX_TAG_NUM, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nItemSlot >= 0 && nItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG, errGuildStorageTargetSlotError);

    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byItemSize != 0, errGuildStorageItemNotExist);
    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byConSlot != 0xFF, errGuildStorageSlotUsed);

    m_aaItemBuffer[nConSlot][nItemSlot].byConSlot = 0xFF;
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSlot = 0xFF;

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuildStorage::UnPrePopItem(INT32 nConSlot, INT32 nItemSlot)
{
    INT32   nResult = errUnknownError;

    LOG_PROCESS_ERROR(m_pOwnerGuild);
    LOG_PROCESS_ERROR_RET_CODE(nConSlot >= 0 && nConSlot < m_pOwnerGuild->GetLevel() && nConSlot < GUILD_STORAGE_MAX_TAG_NUM, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nItemSlot >= 0 && nItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG, errGuildStorageTargetSlotError);

    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byConSlot == 0xFF, errSuccess);

    m_aaItemBuffer[nConSlot][nItemSlot].byConSlot = (UINT8)nConSlot;
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSlot = (UINT8)nItemSlot;

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuildStorage::PopItem(const MS_ROLE_DATA *pRoleData, INT32 nConSlot, INT32 nItemSlot, INT32 &nBufferSize, char *pBuffer)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;

    LOG_PROCESS_ERROR(pBuffer);
    LOG_PROCESS_ERROR(pRoleData);
    LOG_PROCESS_ERROR(m_pOwnerGuild);

    LOG_PROCESS_ERROR_RET_CODE(nConSlot >= 0 && nConSlot < m_pOwnerGuild->GetLevel() && nConSlot < GUILD_STORAGE_MAX_TAG_NUM, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nItemSlot >= 0 && nItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR(m_aaItemBuffer[nConSlot][nItemSlot].byItemSize < nBufferSize);
    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byItemSize != 0, errGuildStorageItemNotExist);
    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nConSlot][nItemSlot].byConSlot == 0xFF, errGuildStorageSlotUsed);

    nBufferSize = m_aaItemBuffer[nConSlot][nItemSlot].byItemSize;
    memcpy(pBuffer, m_aaItemBuffer[nConSlot][nItemSlot].szItemBuffer, nBufferSize);

    m_aaItemBuffer[nConSlot][nItemSlot].byConSlot = 0;
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSlot = 0;
    m_aaItemBuffer[nConSlot][nItemSlot].byItemSize = 0;

    nRetCode = _AddStorageEvent(STORAGE_HE_TYPE_POP, pRoleData->dwRoleID, nBufferSize, m_aaItemBuffer[nConSlot][nItemSlot].szItemBuffer);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    nRetCode = SqlUpdateGuild(m_pOwnerGuild);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    {
        INT32       nItemCount = 0;
        ItemInfo    tempItemInfo;
        UINT32      tempItemSize = tempItemInfo.Load(pBuffer);
        LOG_CHECK_ERROR(tempItemSize == nBufferSize);

        if (tempItemSize == nBufferSize)
        {
            const ITEM_DATA* itemdata = NULL;
            LOG_PROCESS_ERROR(tempItemInfo.m_nTemplateID >= 99);
            itemdata = GetItemData(tempItemInfo.m_nTemplateID);
            LOG_PROCESS_ERROR(itemdata);

            nItemCount = itemdata->m_byDurType == DURTYPE_STACK ? tempItemInfo.m_nCurDur : 1;

            if (itemdata->m_byLog)
            {
                nRetCode = CMessageHandler::GetInstance().DoDLLogItem(
                    m_pOwnerGuild->GetID(),
                    pRoleData->byGroup,
                    tempItemInfo.m_llGuid,
                    tempItemInfo.m_nTemplateID,
                    nItemCount,
                    ITEMLOG_GUILD_STORAGE,
                    ITEMACTION_DEL,
                    pRoleData->dwRoleID);
                LOG_CHECK_ERROR(nRetCode);
            }

            if (itemdata->m_byStat)
            {
                nRetCode = CMessageHandler::GetInstance().DoDLStatItem(m_pOwnerGuild->GetSvrGroup(), tempItemInfo.m_nTemplateID, nItemCount, 0);
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuildStorage::MoveItem(INT32 nSourceConSlot, INT32 nSourceItemSlot, INT32 nTargetConSlot, INT32 nTargetItemSlot)
{
    GUILD_STORAGE_ITEM_DATA    stTempItemData;
    INT32       nResult = errUnknownError;

    LOG_PROCESS_ERROR(m_pOwnerGuild);

    LOG_PROCESS_ERROR_RET_CODE(nSourceConSlot >= 0 && nSourceConSlot < m_pOwnerGuild->GetLevel() && nSourceConSlot < GUILD_STORAGE_MAX_TAG_NUM, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nSourceItemSlot >= 0 && nSourceItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nTargetConSlot >= 0 && nTargetConSlot < m_pOwnerGuild->GetLevel() && nTargetConSlot < GUILD_STORAGE_MAX_TAG_NUM, errGuildStorageTargetSlotError);
    LOG_PROCESS_ERROR_RET_CODE(nTargetItemSlot >= 0 && nTargetItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG, errGuildStorageTargetSlotError);

    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byConSlot != 0xFF, errGuildStorageSlotUsing);
    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byConSlot != 0xFF, errGuildStorageSlotUsing);
    LOG_PROCESS_ERROR_RET_CODE(m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byItemSize != 0, errGuildStorageItemNotExist);


    if (m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byItemSize != 0)
    {
        memcpy(stTempItemData.szItemBuffer, m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].szItemBuffer, sizeof(m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].szItemBuffer));
        stTempItemData.byConSlot = m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byConSlot;
        stTempItemData.byItemSlot = m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byItemSlot;
        stTempItemData.byItemSize = m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byItemSize;

        memcpy(m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].szItemBuffer, m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].szItemBuffer, sizeof(m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].szItemBuffer));
        m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byConSlot = nTargetConSlot;
        m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byItemSlot = nTargetItemSlot;
        m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byItemSize = m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byItemSize;

        memcpy(&m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].szItemBuffer, stTempItemData.szItemBuffer, sizeof(m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].szItemBuffer));
        m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byConSlot = nSourceConSlot;
        m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byItemSlot = nSourceItemSlot;
        m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byItemSize = stTempItemData.byItemSize;
    }
    else
    {
        memcpy(m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].szItemBuffer, m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].szItemBuffer, sizeof(m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].szItemBuffer));
        m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byConSlot = nTargetConSlot;
        m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byItemSlot = nTargetItemSlot;
        m_aaItemBuffer[nTargetConSlot][nTargetItemSlot].byItemSize = m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byItemSize;

        m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byConSlot = 0;
        m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byItemSlot = 0;
        m_aaItemBuffer[nSourceConSlot][nSourceItemSlot].byItemSize = 0;
    }

    return errSuccess;
Exit0:
    return nResult;
}

STORAGE_EVENT_DATA *CMsGuildStorage::GetFirstStorageEventData()
{
    STORAGE_EVENT_DATA  *pData = NULL;
    BASE_LIST_NODE  *pNode = m_History.Head().pNext;
    PROCESS_ERROR(pNode != &m_History.Rear());

    pData = node_cast(STORAGE_EVENT_DATA*, node, pNode);

    return pData;
Exit0:
    return NULL;
}

STORAGE_EVENT_DATA *CMsGuildStorage::GetNextStorageEventData(STORAGE_EVENT_DATA *pData)
{
    STORAGE_EVENT_DATA  *pNextData = NULL;
    BASE_LIST_NODE  *pNode = NULL;
    LOG_PROCESS_ERROR(pData);

    pNode = pData->node.pNext;
    PROCESS_ERROR(pNode != &m_History.Rear());

    pNextData = node_cast(STORAGE_EVENT_DATA*, node, pNode);

    return pNextData;
Exit0:
    return NULL;
}

GUILD_STORAGE_ITEM_DATA * CMsGuildStorage::GetFirstConSlotItem(INT32 nConSlot)
{
    LOG_PROCESS_ERROR(m_pOwnerGuild);

    LOG_PROCESS_ERROR(nConSlot >= 0 && nConSlot < m_pOwnerGuild->GetLevel() && nConSlot < GUILD_STORAGE_MAX_TAG_NUM);
    m_nContainerCursor = nConSlot;
    m_nItemCursor = 0;
    while (m_nItemCursor < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG)
    {
        if (m_aaItemBuffer[m_nContainerCursor][m_nItemCursor].byItemSize != 0 && m_aaItemBuffer[m_nContainerCursor][m_nItemCursor].byConSlot != 0xFF)
        {
            return &m_aaItemBuffer[m_nContainerCursor][m_nItemCursor];
        }
        ++m_nItemCursor;
    }
    
Exit0:
    return NULL;
}

GUILD_STORAGE_ITEM_DATA *CMsGuildStorage::GetNextConSlotItem()
{
    LOG_PROCESS_ERROR(m_pOwnerGuild);
    LOG_PROCESS_ERROR(m_nContainerCursor >= 0 && m_nContainerCursor < m_pOwnerGuild->GetLevel() && m_nContainerCursor < GUILD_STORAGE_MAX_TAG_NUM);
    ++m_nItemCursor;

    while (m_nItemCursor < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG)
    {
        if (m_aaItemBuffer[m_nContainerCursor][m_nItemCursor].byItemSize != 0 && m_aaItemBuffer[m_nContainerCursor][m_nItemCursor].byConSlot != 0xFF)
        {
            return &m_aaItemBuffer[m_nContainerCursor][m_nItemCursor];
        }
        ++m_nItemCursor;
    }
    
Exit0:
    return NULL;
}

BOOL CMsGuildStorage::_AddStorageEvent(INT32 nType, UINT32 dwRoleID, INT32 nLength, const char *pcszItemBuffer)
{
    INT32   nRetCode = 0;
    BASE_LIST_NODE      *pNode = NULL;
    STORAGE_EVENT_DATA  *pEventData = NULL;
    INT32   nActuallyLength = 0;

    LOG_PROCESS_ERROR(m_pOwnerGuild);

    if (m_nHistoryNum == GUILD_MAX_STORAGE_EVENT_NUM)
    {
        pNode = m_History.PopHead();
        LOG_PROCESS_ERROR(pNode);
        --m_nHistoryNum;
        pEventData = (STORAGE_EVENT_DATA*)((char*)pNode - (char*)&(((EVENT_DATA*)0)->node));
    }
    else
    {
        pEventData = CMsGuildMgr::GetInstance().CreateStorageEventData();
    }
    LOG_PROCESS_ERROR(pEventData);

    ClearNode(&pEventData->node);
    pEventData->nType = nType;
    pEventData->dwRoleID = dwRoleID;
    pEventData->dwOccurTime = CManageServer::GetInstance().GetGameTime();
    nActuallyLength = nLength > sizeof(pEventData->szItemBuffer) ? sizeof(pEventData->szItemBuffer) : nLength;
    LOG_PROCESS_ERROR(nActuallyLength <= GUILD_MAX_ITEM_BUFFER_LENGTH);
    memcpy(pEventData->szItemBuffer, pcszItemBuffer, nActuallyLength);
    pEventData->nBufferSize = nActuallyLength;

    nRetCode = m_History.PushRear(&pEventData->node);
    LOG_PROCESS_ERROR(nRetCode);
    ++m_nHistoryNum;

    nRetCode = CPreSaveList::GetInstance().SaveToList(PRE_SAVE_GUILD_MAIN, m_pOwnerGuild->GetID());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientAddStorageEventNtf(m_pOwnerGuild, 
        nType, dwRoleID, pEventData->dwOccurTime, nActuallyLength, pcszItemBuffer);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    // release
    if (pEventData)
    {
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteStorageEventData(pEventData));
    }

    return FALSE;
}

BOOL CMsGuildStorage::CheckCD()
{
    if (m_nCD < CManageServer::GetInstance().GetGameTime())
    {
        m_nCD = CManageServer::GetInstance().GetGameTime() + 1;
        return TRUE;
    }
    return FALSE;
}
