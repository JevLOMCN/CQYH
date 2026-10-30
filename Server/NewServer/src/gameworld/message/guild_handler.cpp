#include "stdafx.h"
#include "message_handler.h"

#include "g2m_message.h"
#include "c2g_message.h"
#include "game_world.h"
#include "imessage_notifier.h"

#include "guild_config.h"

#include "data_manager.h"
#include "guild_welfare_data.h"
#include "guild_building_data.h"
#include "guild_mgr.h"
#include "scene_data.h"
#include "spell_config.h"
#include "item_bag.h"
#include "gs_guild.h"
#include "npc_data.h"
#include "scene_file_mgr.h"
#include "sabac_mgr.h"
#include "role.h"

using namespace ExternalMessage;

#ifndef _ROBOT

// Guild
void CCliMessageHandler::OnGuildCreate(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    C2G_GUILD_CREATE   *pMsg = (C2G_GUILD_CREATE*)pData;
    CNpc* pNpc = NULL;
    // check npc Allan Tao[8.5.2013] 
    //pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    //LOG_PROCESS_ERROR(pNpc);
    //nRetCode = poRole->CanInteract(pNpc);
    //LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    // pre check
    PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(MONEYTYPE_02) >= g_GuildConfig.nCanCreateGuildMoney, errItemNotEnoughMoneytype02);
    PROCESS_ERROR_RET_CODE(poRole->GetLevel() >= g_GuildConfig.nCanCreateGuildLevel, errGuildLowerLevel);
    PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetItemCount(g_GuildConfig.nCanCreateGuildItemTypeID) >= g_GuildConfig.nCanCreateGuildItemNum, errGuildNoItem);
    PROCESS_ERROR_RET_CODE(0 == poRole->GetGuildID(), errGuildAlreadyJoin);
    PROCESS_ERROR_RET_CODE(!CGameWorld::GetInstance().CheckCensoredWord(pMsg->szGuildName), errGuildCensoredWord);
    nRetCode = DoMSGuildCreate(poRole->GetObjID(), pMsg->szGuildName, pMsg->szAnnounce);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);

    return;
}

void CCliMessageHandler::OnGuildDrawSalary(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    C2G_GUILD_DRAW_SALARY   *pMsg = (C2G_GUILD_DRAW_SALARY*)pData;
    CNpc* pNpc = NULL;

    pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    LOG_PROCESS_ERROR(pNpc);

    nRetCode = poRole->CanInteract(pNpc);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNoGuild);

    // send to ms
    nRetCode = DoMSGuildDrawSalary(poRole->GetObjID());
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);

    return;
}

void CCliMessageHandler::OnGuildContribute(CRole *poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    INT32       nFinalMoney = 0;
    INT32       nGuildMoney = 0;
    CGSGuild    *poGuild = NULL;
    GS_GUILD_MEMBER *poMember = NULL;
    C2G_GUILD_CONTRIBUTE_FUND *pMsg = (C2G_GUILD_CONTRIBUTE_FUND*)pData;
    LOG_PROCESS_ERROR(pMsg);
    // decrement money
    LOG_PROCESS_ERROR(poRole);
    
    poGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    LOG_PROCESS_ERROR(poGuild);

    poMember = poGuild->GetMember(poRole->GetObjID());
    LOG_PROCESS_ERROR(poMember);

    PROCESS_ERROR_RET_CODE(pMsg->nMoney > 0 && pMsg->nMoney <= poRole->GetItemBag()->GetMoney(MONEYTYPE_02), errItemNotEnoughMoneytype02);

    nFinalMoney = poRole->GetRoleGuildData().CalculateFundLimit(poMember->nPosition, pMsg->nMoney);
    PROCESS_ERROR_RET_CODE(nFinalMoney != 0, errGuildDonateLimit);

    poRole->GetItemBag()->SubMoney(MONEYTYPE_02, nFinalMoney, MONEYLOG_GUILD_CONTRIBUTE, 0);
    nGuildMoney = nFinalMoney / 10;
    poRole->GetItemBag()->AddMoney(MONEYTYPE_GUILD, nGuildMoney, MONEYLOG_INVALID, 0);

    nRetCode = DoGuildContributeAck(poRole, nFinalMoney, nFinalMoney);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    // send to ms
    nRetCode = DoMSGuildContribute(poRole->GetObjID(), GUILD_ATTR_MONEY, nFinalMoney);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);

    return;
}

void CCliMessageHandler::OnGuildViewStorage(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CNpc* pNpc = NULL;
    C2G_GUILD_VIEW_STORAGE *pMsg = (C2G_GUILD_VIEW_STORAGE*)pData;
    LOG_PROCESS_ERROR(pMsg);
    // decrement money
    LOG_PROCESS_ERROR(poRole);

    // check npc Allan Tao[8.5.2013] 
    //pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    //LOG_PROCESS_ERROR(pNpc);
    //nRetCode = poRole->CanInteract(pNpc);
    //LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNotFind);

    nRetCode = DoMSGuildViewStorage(poRole->GetObjID(), pMsg->byStorageTag);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildPushItem(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CNpc* pNpc = NULL;
    Item    *pItem = NULL;
    C2G_GUILD_PUSH_ITEM *pMsg = (C2G_GUILD_PUSH_ITEM*)pData;
    LOG_PROCESS_ERROR(pMsg);
    // decrement money
    LOG_PROCESS_ERROR(poRole);

    // check npc Allan Tao[8.5.2013] 
    //pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    //LOG_PROCESS_ERROR(pNpc);
    //nRetCode = poRole->CanInteract(pNpc);
    //LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    pItem = poRole->GetItemBag()->GetItem(pMsg->wItemConSlot, pMsg->wItemSlot);
    LOG_PROCESS_ERROR_RET_CODE(pItem, errItemNotFound);
    PROCESS_ERROR_RET_CODE(pItem->m_iteminfo.m_byBindType != BINDTYPE_BINDED, errItemBinded);

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNoGuild);
    LOG_PROCESS_ERROR_RET_CODE(CONSLOT_DEFAULT == pMsg->wItemConSlot, errItemInvalidPos);

    nRetCode = DoMSGuildPrePushItem(poRole->GetObjID(), pMsg->wItemConSlot, pMsg->wItemSlot, pMsg->byStorageConSlot, pMsg->byStorageSlot);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildPopItem(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CNpc* pNpc = NULL;
    C2G_GUILD_POP_ITEM *pMsg = (C2G_GUILD_POP_ITEM*)pData;
    LOG_PROCESS_ERROR(pMsg);
    // decrement money
    LOG_PROCESS_ERROR(poRole);

    // check npc Allan Tao[8.5.2013] 
    //pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    //LOG_PROCESS_ERROR(pNpc);
    //nRetCode = poRole->CanInteract(pNpc);
    //LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);


    LOG_PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNoGuild);
    LOG_PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(MONEYTYPE_02) >= g_GuildConfig.nGuildPopItemComsumeMoney, errGuildNotEnoughMoney);
    LOG_PROCESS_ERROR_RET_CODE(!poRole->GetItemBag()->GetItem(pMsg->wItemConSlot, pMsg->wItemSlot), errGuildStorageSlotUsed);

    nRetCode = DoMSGuildPrePopItem(poRole->GetObjID(), pMsg->byStorageConSlot, pMsg->byStorageSlot, pMsg->wItemConSlot, pMsg->wItemSlot);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildCreateBuilding(CRole *poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CNpc* pNpc = NULL;
    CGSGuild    *poGSGuild = NULL;
    C2G_GUILD_CREATE_BUILDING *pMsg = (C2G_GUILD_CREATE_BUILDING*)pData;
    GS_G_B_DATA *pGSGBData = NULL;
    const GUILD_BUILDING_DATA *pTBData = NULL;
    UINT64      qwNameID = 0;

    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poRole);

    // check npc Allan Tao[8.5.2013] 
    pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    LOG_PROCESS_ERROR(pNpc);
    LOG_PROCESS_ERROR(pNpc->GetScene());

    nRetCode = poRole->CanInteract(pNpc);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    LOG_PROCESS_ERROR_RET_CODE(pMsg->byBuildingType != GUILD_B_MAIN  && pMsg->byBuildingType != GUILD_B_DEITY && pMsg->byBuildingType != GUILD_B_WONDER, errGuildBuildingAlreadyExist);

    poGSGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild, errGuildNoGuild);
    LOG_PROCESS_ERROR_RET_CODE(pNpc->GetScene()->GetSceneID() == poRole->GetGuildID(), errGuildNotMember);

    LOG_PROCESS_ERROR(poRole->GetScene());

    pNpc = CGameWorld::GetInstance().FindNpc(pMsg->dwNpcID);
    LOG_PROCESS_ERROR_RET_CODE(pNpc, errNpcNotFound);
    LOG_PROCESS_ERROR_RET_CODE(pNpc->GetSpawnPoint(), errNpcNotFound);

    pGSGBData = poGSGuild->GetBuildingDataByNpcID(pNpc->GetSpawnPoint()->qwNameID);
    LOG_PROCESS_ERROR_RET_CODE(!pGSGBData, errGuildBuildingNpcIDError);

    //LOG_PROCESS_ERROR_RET_CODE(pGSGBData->nType == GUILD_B_NONE, errGuildBuildingAlreadyExist);

    pTBData = GetGuildBuildingData(pMsg->byBuildingType, 0);
    LOG_PROCESS_ERROR_RET_CODE(pTBData, errGuildBuildingIDError);

    // check num
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild->GetBuildingNumByType(pMsg->byBuildingType) < pTBData->nMaxNum, errGuildBMaxNum);
 
    // check npc type
    LOG_PROCESS_ERROR_RET_CODE(pNpc->GetNpcData(), errNpcDataNil);
    LOG_PROCESS_ERROR_RET_CODE(pTBData->dwCdtnBaseNpcID == pNpc->GetNpcData()->nTemplateID, errGuildErrorBuildingTypeForNpc);

    nRetCode = DoMSGuildCreateBuilding(poRole->GetObjID(), pNpc->GetSpawnPoint()->qwNameID, pMsg->byBuildingType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult != errUnknownError)
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;

}

void CCliMessageHandler::OnGuildUpgradeBuilding(CRole *poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CNpc* pNpc = NULL;
    CGSGuild    *poGSGuild = NULL;
    C2G_GUILD_UPGRADE_BUILDING *pMsg = (C2G_GUILD_UPGRADE_BUILDING*)pData;
    GS_G_B_DATA *pGSGBData = NULL;
    const GUILD_BUILDING_DATA *pTBData = NULL;
    //const NPC_SPAWN_POINT*  pSvrPoint = NULL;
    UINT64      qwNameID = 0;

    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poRole);

    // check npc Allan Tao[8.5.2013] 
    pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    LOG_PROCESS_ERROR(pNpc);
    LOG_PROCESS_ERROR(pNpc->GetScene());

    nRetCode = poRole->CanInteract(pNpc);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    poGSGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild, errGuildNoGuild);
    LOG_PROCESS_ERROR_RET_CODE(pNpc->GetScene()->GetSceneID() == poRole->GetGuildID(), errGuildNotMember);
    LOG_PROCESS_ERROR(poRole->GetScene());

    //pNpc = poRole->GetScene()->FindNpc(pMsg->dwSingleNpcID);
    //LOG_PROCESS_ERROR_RET_CODE(pNpc, errNpcNotFound);

    if (pMsg->byUpgradeType == G_B_UPGRADE_BY_NPC)
    {
        pNpc = CGameWorld::GetInstance().FindNpc((UINT32)pMsg->qwNpcNameID);
        LOG_PROCESS_ERROR_RET_CODE(pNpc, errNpcNotFound);
        LOG_PROCESS_ERROR_RET_CODE(pNpc->GetSpawnPoint(), errNpcNotFound);
        qwNameID = pNpc->GetSpawnPoint()->qwNameID;
    }
    else if (pMsg->byUpgradeType == G_B_UPGRADE_BY_PANEL)
    {
        qwNameID = pMsg->qwNpcNameID;
    }
    else
        LOG_PROCESS_ERROR(FALSE);

    //pSvrPoint = poRole->GetScene()->GetStaticData()->GetSingleNpcByName(qwNameID);
    //LOG_PROCESS_ERROR_RET_CODE(pSvrPoint, errNpcNotFound);

    pGSGBData = poGSGuild->GetBuildingDataByNpcID(qwNameID);
    LOG_PROCESS_ERROR_RET_CODE(pGSGBData, errGuildBuildingNpcIDError);

    LOG_PROCESS_ERROR_RET_CODE(pGSGBData->nType != GUILD_B_NONE, errGuildBuildingNotExist);

    pTBData = GetGuildBuildingData(pGSGBData->nType, pGSGBData->nLevel);
    LOG_PROCESS_ERROR_RET_CODE(pTBData, errGuildBuildingIDError);

    // check level
    LOG_PROCESS_ERROR_RET_CODE(pGSGBData->nLevel < pTBData->nMaxLevel, errGuildBuildingMaxLevel);

    LOG_PROCESS_ERROR_RET_CODE(pNpc->GetNpcData(), errNpcDataNil);
    //LOG_PROCESS_ERROR_RET_CODE(pTBData->dwCdtnBaseNpcID == pNpc->GetNpcData()->nTemplateID, errGuildErrorBuildingTypeForNpc);
    LOG_PROCESS_ERROR_RET_CODE(pMsg->byBuildingType == pGSGBData->nType, errGuildBTypeNotMatch);

    nRetCode = DoMSGuildUpgradeBuilding(poRole->GetObjID(), qwNameID, pGSGBData->nType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult != errUnknownError)
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildDelBuilding(CRole *poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CNpc* pNpc = NULL;
    CGSGuild    *poGSGuild = NULL;
    C2G_GUILD_DEL_BUILDING *pMsg = (C2G_GUILD_DEL_BUILDING*)pData;
    GS_G_B_DATA *pGSGBData = NULL;
    UINT64      qwNameID = 0;

    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poRole);

    pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    LOG_PROCESS_ERROR(pNpc);
    LOG_PROCESS_ERROR(pNpc->GetScene());

    nRetCode = poRole->CanInteract(pNpc);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    poGSGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild, errGuildNoGuild);
    LOG_PROCESS_ERROR(poRole->GetScene());
    LOG_PROCESS_ERROR_RET_CODE(pNpc->GetScene()->GetSceneID() == poRole->GetGuildID(), errGuildNotMember);

    if (pMsg->byDelType == G_B_UPGRADE_BY_NPC)
    {
        pNpc = CGameWorld::GetInstance().FindNpc((UINT32)pMsg->qwNameID);
        LOG_PROCESS_ERROR_RET_CODE(pNpc, errNpcNotFound);
        LOG_PROCESS_ERROR_RET_CODE(pNpc->GetSpawnPoint(), errNpcNotFound);
        qwNameID = pNpc->GetSpawnPoint()->qwNameID;
    }
    else if (pMsg->byDelType == G_B_UPGRADE_BY_PANEL)
    {
        qwNameID = pMsg->qwNameID;
    }
    else
        LOG_PROCESS_ERROR(FALSE);

    pGSGBData = poGSGuild->GetBuildingDataByNpcID(qwNameID);
    LOG_PROCESS_ERROR_RET_CODE(pGSGBData, errGuildBuildingNpcIDError);

    LOG_PROCESS_ERROR_RET_CODE(pGSGBData->nType != GUILD_B_NONE, errGuildBuildingNotExist);

    //LOG_PROCESS_ERROR_RET_CODE(pNpc->GetNpcData(), errNpcDataNil);
    //LOG_PROCESS_ERROR_RET_CODE(pMsg->byBuildingType == pGSGBData->nType, errGuildBTypeNotMatch);

    nRetCode = DoMSGuildDelBuilding(poRole->GetObjID(), qwNameID, pGSGBData->nType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
        poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildBuyAura(CRole *poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CNpc* pNpc = NULL;
    CAura* pAura = NULL;
    CAuraInstance* pAuraInstance = NULL;
    CGSGuild    *poGSGuild = NULL;
    const GUILD_WELFARE_DATA  *pWelfareData = NULL;
    C2G_GUILD_BUY_AURA *pMsg = (C2G_GUILD_BUY_AURA*)pData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poRole);

    // check npc Allan Tao[8.5.2013] 
    pNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    LOG_PROCESS_ERROR(pNpc);
    LOG_PROCESS_ERROR(pNpc->GetScene());

    nRetCode = poRole->CanInteract(pNpc);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    poGSGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild, errGuildNotFind);
    LOG_PROCESS_ERROR_RET_CODE(pNpc->GetScene()->GetSceneID() == poRole->GetGuildID(), errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(0 < pMsg->byType && pMsg->byType < GUILD_MAX_WELFARE_AURA_COUNT, errGuildFailed);
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild->GetWelfareLevelByType(pMsg->byType) > 0, errGuildWelfareNeedUpgrade);

    pWelfareData = GetGuildWelfareData(pMsg->byType, poGSGuild->GetWelfareLevelByType(pMsg->byType));
    LOG_PROCESS_ERROR_RET_CODE(pWelfareData, errGuildWelfareAuraIDError);

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(MONEYTYPE_02) 
        >= pWelfareData->nPurchaseMoneyConsumption, errGuildNotEnoughMoney);

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(MONEYTYPE_GUILD) 
        >= pWelfareData->nPurchaseGuildCurrencyConsumption, errGuildNotEnoughContribution);

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetLevel() 
        >= pWelfareData->nPurchaseLevelRequirement, errGuildLowerLevel);

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNoGuild);

    LOG_PROCESS_ERROR_RET_CODE(!(poGSGuild->GetState() & GUILD_STATE_STOP_WELFARE), errGuildWelfareBeFrozen);

    //if (poRole->GetVIPLevel() > vlLevel0)
    //{
    LOG_PROCESS_ERROR(poRole->GetVIPLevel() < vlTotal);
        LOG_PROCESS_ERROR_RET_CODE(
            poRole->GetRoleGuildData().GetBuyAuraCount(pMsg->byType) < pWelfareData->arrVipPurchaseLimit[poRole->GetVIPLevel()], 
            errGuildAuraPurchaseLimit);
    //}
    //else
    //{
    //    LOG_PROCESS_ERROR_RET_CODE(
    //        poRole->GetRoleGuildData().GetBuyAuraCount(pMsg->byType) < pWelfareData->nPurchaseLimit, 
    //        errGuildAuraPurchaseLimit);
    //}
    
    nRetCode = poRole->GetItemBag()->SubMoney(MONEYTYPE_02, pWelfareData->nPurchaseMoneyConsumption, MONEYLOG_GUILD_BUY_AURA, 0);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);
	nRetCode = poRole->GetItemBag()->SubMoney(MONEYTYPE_GUILD, pWelfareData->nPurchaseGuildCurrencyConsumption, MONEYLOG_GUILD_BUY_AURA, 0);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    poRole->GetRoleGuildData().AddBuyAuraCount(pMsg->byType);

    pAura = CSpellConfig::GetInstance().FindAuraTemplate(pWelfareData->dwAuraID);
    LOG_PROCESS_ERROR_RET_CODE(pAura, errGuildConfigError);

    pAuraInstance = poRole->AppendAura(pAura, pNpc, pNpc->GetObjID(), NULL, 0, 0, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(pAuraInstance, errGuildBuyAuraFailed);
    
    nRetCode = DoGuildBuyAuraAck(poRole, pMsg->byType, 
        poRole->GetRoleGuildData().GetBuyAuraCount(pMsg->byType));
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
        poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildChangeWeather(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CNpc*       poNpc = NULL;
    CScene      *poScene = NULL;
    CGSGuild    *poGSGuild = NULL;

    C2G_GUILD_CHANGE_WEATHER *pMsg = (C2G_GUILD_CHANGE_WEATHER *)pData;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poRole);

    // check npc Allan Tao[8.5.2013] 
    poNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    LOG_PROCESS_ERROR(poNpc);
    LOG_PROCESS_ERROR(poNpc->GetScene());

    nRetCode = poRole->CanInteract(poNpc);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
    
    poScene = poRole->GetScene();
    LOG_PROCESS_ERROR(poScene);

    LOG_PROCESS_ERROR_RET_CODE(0 != poRole->GetGuildID(), errGuildNoGuild);
    poGSGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild, errGuildNotFind);
    LOG_PROCESS_ERROR_RET_CODE(poNpc->GetScene()->GetSceneID() == poRole->GetGuildID(), errGuildNotMember);

    // pre check 
    LOG_PROCESS_ERROR_RET_CODE(pMsg->byTimeType > GUILD_CW_TIME_NONE && pMsg->byTimeType < GUILD_CW_TIME_END, errGuildFailed);
    LOG_PROCESS_ERROR_RET_CODE(pMsg->byWeatherType > swtInvalid && pMsg->byWeatherType < swtTotal, errGuildFailed);
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild->GetLeaderID() == poRole->GetObjID(), errGuildNotLeader);
    LOG_PROCESS_ERROR_RET_CODE(poScene->GetSceneTempID() == g_GuildConfig.dwPalaceSceneTemplateID 
        || poScene->GetSceneTempID() == g_GuildConfig.dwSceneTemplateID, errGuildWrongMap);
    LOG_PROCESS_ERROR_RET_CODE(poRole->GetItemBag()->GetMoney(MONEYTYPE_02) >= g_GuildConfig.arrWeatherCondition[pMsg->byTimeType].nMoney, errItemNotEnoughMoneytype02);
    LOG_PROCESS_ERROR_RET_CODE(poScene->GetWeather() == swtNormal, errGuildWeatherAlreadyBeChanged);

	poRole->GetItemBag()->SubMoney(MONEYTYPE_02, g_GuildConfig.arrWeatherCondition[pMsg->byTimeType].nMoney, MONEYLOG_CHANGE_WEATHER, 0);

    poScene->SetWeather(pMsg->byWeatherType, g_GuildConfig.arrWeatherCondition[pMsg->byTimeType].nTime * GAME_FPS);

    return ;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
        poRole->ErrorCodeNotify(nResult, 0, 0);
    return ;
}

void CCliMessageHandler::OnGuildEnterMap(CRole *poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CGSGuild    *poGSGuild = NULL;
    const SCENE_DATA      *pSceneData = NULL;
    GUILD_WAR_DATA  *pWarData = NULL;

    C2G_GUILD_ENTER_MAP *pMsg = (C2G_GUILD_ENTER_MAP *)pData;

    LOG_PROCESS_ERROR_RET_CODE(0 != poRole->GetGuildID(), errGuildNoGuild);

    pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(g_GuildConfig.dwSceneTemplateID);
    LOG_PROCESS_ERROR_RET_CODE(pSceneData, errGuildFailed);

    poGSGuild = CGuildMgr::GetInstance().FindGuild(pMsg->dwSceneID);
    LOG_PROCESS_ERROR_RET_CODE(poGSGuild, errGuildNotFind);

    if (pMsg->byMapType == GUILD_ENTER_OWNER)
    {
        LOG_PROCESS_ERROR(pMsg->dwSceneID == poRole->GetGuildID());

    }
    else if (pMsg->byMapType == GUILD_ENTER_ALLIANCE)
    {
        nRetCode = poGSGuild->IsAlliance(poRole->GetGuildID());
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNotAlliance);

        pWarData = CGuildMgr::GetInstance().FindWarByDefendGuildID(poGSGuild->GetID());
        PROCESS_ERROR_RET_CODE(pWarData, errGuildNotWarTime);
    }

    nRetCode = CGameWorld::GetInstance().JumpTo(poRole, g_GuildConfig.dwSceneTemplateID, pMsg->dwSceneID, 
        g_GuildConfig.nEnterMapPosX * POINT_PER_CM, g_GuildConfig.nEnterMapPosY * POINT_PER_CM, 
        g_GuildConfig.nEnterMapPosZ * POINT_PER_CM, g_GuildConfig.nEnterMapDirection);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildEnterSceneFailed);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildOpenDungeon(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CNpc        *poNpc = NULL;
    C2G_GUILD_OPEN_DUNGEON *pMsg = (C2G_GUILD_OPEN_DUNGEON *)pData;
    
    poNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    LOG_PROCESS_ERROR(poNpc);
    LOG_PROCESS_ERROR(poNpc->GetScene());

    nRetCode = poRole->CanInteract(poNpc);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNoGuild);
    LOG_PROCESS_ERROR_RET_CODE(poNpc->GetScene()->GetSceneID() == poRole->GetGuildID(), errGuildNotMember);

    nRetCode = DoMSGuildPreOpenDungeon(poRole->GetGuildID(), pMsg->dwTemplateID, poRole->GetObjID(), pMsg->dwLeaderID, poRole->GetItemBag()->GetMoney(MONEYTYPE_02));
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildOpenDungeonFailed);

    return ;
Exit0:
    if (nResult == errUnknownError || nResult == errGuildNoGuild)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildEnterDungeon(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    CNpc        *poNpc = NULL;
    CTeamInGS     *poTeam = NULL;
    C2G_GUILD_ENTER_DUNGEON *pMsg = (C2G_GUILD_ENTER_DUNGEON*)pData;

    poNpc = CGameWorld::GetInstance().FindNpc(poRole->GetInteractNpc());
    LOG_PROCESS_ERROR(poNpc);
    LOG_PROCESS_ERROR(poNpc->GetScene());

    nRetCode = poRole->CanInteract(poNpc);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNoGuild);
    LOG_PROCESS_ERROR_RET_CODE(poRole->GetTeamID() != 0, errTeamNotInTeam);
    LOG_PROCESS_ERROR_RET_CODE(poNpc->GetScene()->GetSceneID() == poRole->GetGuildID(), errGuildNotMember);

    poTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
    LOG_PROCESS_ERROR_RET_CODE(poTeam, errTeamNotInTeam);
    //LOG_PROCESS_ERROR_RET_CODE(poTeam->GetLeaderID() == poRole->GetObjID(), errTeamNotLeader);

    nRetCode = DoMSGuildEnterDungeon(poRole->GetObjID(), pMsg->dwTemplateID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildEnterDungeonFailed);

    return ;
Exit0:
    if (nResult == errUnknownError || nResult == errGuildNoGuild)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnGuildRewardAchievement(CRole *poRole, const char *pData)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    C2G_GUILD_REWARD_ACHIEVEMENT *pMsg = (C2G_GUILD_REWARD_ACHIEVEMENT*)pData;

    nRetCode = poRole->GetRoleGuildData().CanRewardAchievement(pMsg->nID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAchiAlreadyReward);
    LOG_PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNoGuild);

    nRetCode = DoMSGuildAchievementReward(poRole->GetObjID(), pMsg->nID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
	poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnLeavelGuild(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR_RET_CODE(poRole->GetGuildID() != 0, errGuildNoGuild);

    nRetCode = DoMSGuildLeaveGuild(poRole->GetObjID());
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnQueryGuildWarRecord(CRole* poRole, const char* pData)
{

}

BOOL CCliMessageHandler::DoMSGuildCreate(UINT32 dwRoleID, const char *pcszGuildName, const char *pcszAnnounce)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_CREATE *pMsg = (InternalMessage::G2M_GUILD_CREATE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_CREATE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_create;
    pMsg->dwRoleID = dwRoleID;
    LOG_PROCESS_ERROR(strlen(pcszAnnounce) < GUILD_ANNOUNCE_LEN);
    strxcpy(pMsg->szAnnounce, pcszAnnounce, GUILD_ANNOUNCE_LEN);
    strxcpy(pMsg->szGuildName, pcszGuildName, GUILD_NAME_LEN);

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_CREATE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildDrawSalary(UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_DRAW_SALARY *pMsg = (InternalMessage::G2M_GUILD_DRAW_SALARY*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_DRAW_SALARY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_draw_salary;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_DRAW_SALARY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildContribute(UINT32 dwRoleID, INT32 nType, INT32 nMoney)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_CONTRIBUTE *pMsg = (InternalMessage::G2M_GUILD_CONTRIBUTE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_CONTRIBUTE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_contribute;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nType = nType;
    pMsg->nMoney = nMoney;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_CONTRIBUTE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildViewStorage(UINT32 dwRoleID, UINT8 byStorageTag)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_VIEW_STORAGE *pMsg = (InternalMessage::G2M_GUILD_VIEW_STORAGE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_VIEW_STORAGE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_view_storage;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byStorageTag = byStorageTag;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_VIEW_STORAGE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildPrePushItem(UINT32 dwRoleID, UINT16 wSourceConSlot, UINT16 wSourceSlot, UINT8 byTargetConSlot, UINT8 byTargetSlot)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_PRE_PUSH_ITEM *pMsg = (InternalMessage::G2M_GUILD_PRE_PUSH_ITEM*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_PRE_PUSH_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_pre_push_item;
    pMsg->dwRoleID = dwRoleID;
    pMsg->wSourceConSlot = wSourceConSlot;
    pMsg->wSourceSlot = wSourceSlot;
    pMsg->byTargetConSlot = byTargetConSlot;
    pMsg->byTargetSlot = byTargetSlot;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_PRE_PUSH_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildPushItem(INT32 nResult, UINT32 dwRoleID, UINT8 byConSlot, UINT8 bySlot, ItemInfo &iteminfo)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_PUSH_ITEM *pMsg = (InternalMessage::G2M_GUILD_PUSH_ITEM*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_PUSH_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_push_item;
    pMsg->nResult = nResult;
    pMsg->dwRoleID = dwRoleID;
    pMsg->bySlot = bySlot;
    pMsg->byConSlot = byConSlot;
    if (errSuccess == pMsg->nResult)
    {
        memset(pMsg->buff, 0, sizeof(pMsg->buff));
        pMsg->wItemSize = iteminfo.Save(pMsg->buff);
    }

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_PUSH_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildPrePopItem(UINT32 dwRoleID, UINT8 byConSlot, UINT8 bySlot, UINT16 wTargetConSlot, UINT16 wTargetSlot)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_PRE_POP_ITEM *pMsg = (InternalMessage::G2M_GUILD_PRE_POP_ITEM*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_POP_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_pre_pop_item;
    pMsg->dwRoleID = dwRoleID;
    pMsg->bySlot = bySlot;
    pMsg->byConSlot = byConSlot;
    pMsg->wTargetConSlot = wTargetConSlot;
    pMsg->wTargetSlot = wTargetSlot;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_PRE_POP_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildPopItem(INT32 nResult, UINT32 dwRoleID, UINT8 byConSlot, UINT8 bySlot, UINT16 wConSlot, UINT16 wSlot)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_POP_ITEM *pMsg = (InternalMessage::G2M_GUILD_POP_ITEM*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_POP_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_pop_item;
    pMsg->nResult = nResult;
    pMsg->dwRoleID = dwRoleID;
    pMsg->bySlot = bySlot;
    pMsg->byConSlot = byConSlot;
    pMsg->wConSlot = wConSlot;
    pMsg->wSlot = wSlot;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_POP_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildCreateBuilding(UINT32 dwRoleID, UINT64 qwNpcNameID, INT32 nType)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_CREATE_BUILDING *pMsg = (InternalMessage::G2M_GUILD_CREATE_BUILDING*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_CREATE_BUILDING));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_create_building;
    pMsg->dwRoleID = dwRoleID;
    pMsg->qwNpcNameID = qwNpcNameID;
    pMsg->nType = nType;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_CREATE_BUILDING));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildUpgradeBuilding(UINT32 dwRoleID, UINT64 qwNpcNameID, INT32 nType)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_UPGRADE_BUILDING *pMsg = (InternalMessage::G2M_GUILD_UPGRADE_BUILDING*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_UPGRADE_BUILDING));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_upgrade_building;
    pMsg->dwRoleID = dwRoleID;
    pMsg->qwNpcNameID = qwNpcNameID;
    pMsg->nType = nType;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_UPGRADE_BUILDING));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildDelBuilding(UINT32 dwRoleID, UINT64 qwNameID, INT32 nType)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_GUILD_DEL_BUILDING *pMsg = (InternalMessage::G2M_GUILD_DEL_BUILDING*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_DEL_BUILDING));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_del_building;
    pMsg->dwRoleID = dwRoleID;
    pMsg->qwNameID = qwNameID;
    pMsg->nType = nType;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_DEL_BUILDING));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildPreOpenDungeon(UINT32 dwGuildID, UINT32 dwTemplateID, UINT32 dwOwnerID, UINT32 dwLeaderID, INT64 llMoney)
{
    INT32   nRetCode = 0;
    InternalMessage::G2M_GUILD_PRE_OPEN_DUNGEON *pMsg = (InternalMessage::G2M_GUILD_PRE_OPEN_DUNGEON*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_PRE_OPEN_DUNGEON));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_pre_open_dungeon;
    pMsg->dwGuildID = dwGuildID;
    pMsg->dwTemplateID = dwTemplateID;
    pMsg->dwOwnerID = dwOwnerID;
    pMsg->dwLeaderID = dwLeaderID;
    pMsg->llMoney = llMoney;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER *)pMsg, sizeof(InternalMessage::G2M_GUILD_PRE_OPEN_DUNGEON));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildOpenDungeon(UINT32 dwOwnerID, UINT32 dwLeaderID, UINT32 dwTemplateID)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_GUILD_OPEN_DUNGEON *pMsg = (InternalMessage::G2M_GUILD_OPEN_DUNGEON*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_OPEN_DUNGEON));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_open_dungeon;
    pMsg->dwLeaderID = dwLeaderID;
    pMsg->dwOwnerID = dwOwnerID;
    pMsg->dwTemplateID = dwTemplateID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_OPEN_DUNGEON));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildEnterDungeon(UINT32 dwRoleID, UINT32 dwTemplateID)
{
    INT32       nRetCode = 0;

    InternalMessage::G2M_GUILD_ENTER_DUNGEON *pMsg = (InternalMessage::G2M_GUILD_ENTER_DUNGEON*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_ENTER_DUNGEON));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_enter_dungeon;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwTemplateID = dwTemplateID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_ENTER_DUNGEON));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildCloseDungeonCommission(UINT32 dwTemplateID, UINT32 dwSceneID, UINT32 arrItemID[], INT32 arrItemCount[], INT32 nItemIDNum)
{
    INT32       nRetCode = 0;

    InternalMessage::G2M_GUILD_CLOSE_DUNGEON_COMMISSION *pMsg = (InternalMessage::G2M_GUILD_CLOSE_DUNGEON_COMMISSION*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_CLOSE_DUNGEON_COMMISSION));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_close_dungeon_commission;
    pMsg->dwSceneID = dwSceneID;
    pMsg->dwTemplateID = dwTemplateID;

    for (pMsg->nItemNum = 0; pMsg->nItemNum < nItemIDNum; ++pMsg->nItemNum)
    {
        pMsg->arrItemID[pMsg->nItemNum] = arrItemID[pMsg->nItemNum];
        pMsg->arrItemCount[pMsg->nItemNum] = arrItemCount[pMsg->nItemNum];
    }

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_CLOSE_DUNGEON_COMMISSION));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSNpcChangeGuildRes(UINT32 dwNpcID, UINT32 dwGuildID, INT32 nChangeType, INT32 nResType, INT32 nCount, INT32 nReason, INT32 nCtxType)
{
    INT32       nRetCode = 0;

    InternalMessage::G2M_GUILD_CHANGE_RES   *pMsg = (InternalMessage::G2M_GUILD_CHANGE_RES*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_CHANGE_RES));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_change_res;
    pMsg->dwNpcID = dwNpcID;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nResourceType = nResType;
    pMsg->nChangeType = nChangeType;
    pMsg->nCount = nCount;
    pMsg->nReason = nReason;
    pMsg->byCtxType = (UINT8)nCtxType;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_CHANGE_RES));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildEndWar(UINT32 dwDefenderID, INT32 nWarResult)
{
    INT32       nRetCode = 0;

    InternalMessage::G2M_GUILD_END_WAR *pMsg = (InternalMessage::G2M_GUILD_END_WAR*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_END_WAR));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_end_war;
    pMsg->dwDefenderID = dwDefenderID;
    pMsg->nWarDefenderResult = nWarResult;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_END_WAR));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildChangeBuildingState(UINT32 dwGuildID, UINT64 qwNameID, UINT32 dwReviveTime)
{
    INT32       nRetCode = 0;

    InternalMessage::G2M_GUILD_CHANGE_BUILDING_STATE *pMsg = (InternalMessage::G2M_GUILD_CHANGE_BUILDING_STATE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_GUILD_CHANGE_BUILDING_STATE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_guild_change_building_state;
    pMsg->dwGuildID = dwGuildID;
    pMsg->qwNameID = qwNameID;
    pMsg->dwReviveTime = dwReviveTime;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_GUILD_CHANGE_BUILDING_STATE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildAchievementReward(UINT32 dwRoleID, INT32 nID)
{
    INT32       nRetCode = 0;
    using namespace InternalMessage;

    G2M_GUILD_ACHIEVEMENT_REWARD    *pMsg = 
        (G2M_GUILD_ACHIEVEMENT_REWARD*)m_poPacketAllocator->Alloc(sizeof(G2M_GUILD_ACHIEVEMENT_REWARD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2m_guild_achievement_reward;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nID = nID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(G2M_GUILD_ACHIEVEMENT_REWARD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSGuildLeaveGuild(UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    using namespace InternalMessage;

    G2M_GUILD_LEAVE_GUILD* pMsg = (G2M_GUILD_LEAVE_GUILD*)m_poPacketAllocator->Alloc(sizeof(G2M_GUILD_LEAVE_GUILD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2m_guild_leave_guild;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(G2M_GUILD_LEAVE_GUILD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncGuild(CRole *poRole)
{
    INT32       nRetCode = 0;

    G2C_SYNC_GUILD *pMsg = (G2C_SYNC_GUILD*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_GUILD));
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(poRole);

    pMsg->wMsg = g2c_sync_guild;
    pMsg->dwCD = poRole->GetRoleGuildData().GetCD();

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_GUILD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncGuildName(CRole* poRole, UINT32 dwGuilID)
{
    INT32       nRetCode = 0;

    G2C_SYNC_GUILD_NAME *pMsg = (G2C_SYNC_GUILD_NAME*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_GUILD_NAME));
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poRole);

    pMsg->wMsg = g2c_sync_guild_name;
    pMsg->dwRoleID = poRole->GetObjID();
    pMsg->dwGuildID = dwGuilID;

    nRetCode = _Broadcast9Region(poRole->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_GUILD_NAME));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoGuildBuyAuraAck(CRole *poRole, INT32 nType, UINT8 byCount)
{
    INT32       nRetCode = 0;
    G2C_GUILD_BUY_AURA_ACK *pMsg = (G2C_GUILD_BUY_AURA_ACK*)m_poPacketAllocator->Alloc(sizeof(G2C_GUILD_BUY_AURA_ACK));
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poRole);

    pMsg->wMsg = g2c_guild_buy_aura_ack;
    pMsg->byType = nType;
    pMsg->byCount = byCount;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_GUILD_BUY_AURA_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncPurchaseAuraRecord(CRole *poRole) 
{
    INT32       nRetCode = 0;
    UINT32      dwLength = 0;
    GUILD_BUY_AURA_RECORD   *pData = NULL;
    G2C_SYNC_PURCHASE_AURA_RECORD *pMsg = (G2C_SYNC_PURCHASE_AURA_RECORD*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_purchase_aura_record;

    pMsg->wCount = 0;
    pData = poRole->GetRoleGuildData().GetFirstRecord();
    while (pData)
    {
        pMsg->data[pMsg->wCount].nType = pData->nType;
        pMsg->data[pMsg->wCount].byCount = pData->byCount;
        ++pMsg->wCount;

        pData = poRole->GetRoleGuildData().GetNextRecord();
    }

    pMsg->wSize = sizeof(G2C_SYNC_PURCHASE_AURA_RECORD) + pMsg->wCount * sizeof(GUILD_BUY_AURA_RECORD_INFO);

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncGuildAchievement(CRole *poRole)
{
    INT32       nRetCode = 0;
    UINT32      dwLength = 0;
    G2C_SYNC_GUILD_ACHIEVEMENT *pMsg = NULL;
    
    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(poRole->GetRoleGuildData().GetAchievementList());

    pMsg = (G2C_SYNC_GUILD_ACHIEVEMENT*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_GUILD_ACHIEVEMENT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_guild_achievement;

    for (INT32 i = 0; i < GUILD_MAX_ACHI_ARRAY_LENGTH; ++i)
        pMsg->acAchievement[i] = poRole->GetRoleGuildData().GetAchievementList()[i];

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_GUILD_ACHIEVEMENT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoGuildDrawSalaryAck(CRole *poRole, INT32 nSalary)
{
    INT32       nRetCode = 0;
    G2C_GUILD_DRAW_SALARY_ACK *pMsg = (G2C_GUILD_DRAW_SALARY_ACK*)m_poPacketAllocator->Alloc(sizeof(G2C_GUILD_DRAW_SALARY_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_guild_draw_salary_ack;
    pMsg->nSalary = nSalary;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_GUILD_DRAW_SALARY_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoGuildContributeAck(CRole *poRole, INT32 nMoney, INT32 nContribution)
{
    INT32       nRetCode = 0;
    G2C_GUILD_CONTRIBUTE_ACK *pMsg = (G2C_GUILD_CONTRIBUTE_ACK*)m_poPacketAllocator->Alloc(sizeof(G2C_GUILD_CONTRIBUTE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_guild_contribute_ack;
    pMsg->nContribution = nContribution;
    pMsg->nMoney = nMoney;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_GUILD_CONTRIBUTE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncSabacEnrollList(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_SABAC_ENROLL_LIST *pMsg = NULL;
	INT32 nServerGroup = 0;
	INT32 nGuildCount = 0;
	UINT32 dwGuildID = 0;
	INT32 nGuildIndex = 0;
	const ENROLL_SABAC_DATA* pcEnrollSabacData = NULL;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poRole->GetScene());

	nServerGroup = poRole->GetScene()->GetServerGroup();
	nGuildCount = CSabacMgr::GetInstance().GetEnrolledGuildCount(nServerGroup);

	pMsg = (G2C_SYNC_SABAC_ENROLL_LIST*)m_poPacketAllocator->Alloc(
		sizeof(G2C_SYNC_SABAC_ENROLL_LIST) + nGuildCount * sizeof(G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_sabac_enroll_list;
	pMsg->wSize = sizeof(G2C_SYNC_SABAC_ENROLL_LIST) + nGuildCount * sizeof(G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA);
	pcEnrollSabacData = CSabacMgr::GetInstance().GetFirstGuild(nServerGroup);
	while (pcEnrollSabacData && nGuildIndex < nGuildCount)
	{
		pMsg->EnrollList[nGuildIndex].dwGuildID = pcEnrollSabacData->dwGuildID;
		pMsg->EnrollList[nGuildIndex].dwEnrollTime = pcEnrollSabacData->dwEnrollTime;
		nGuildIndex++;
		pcEnrollSabacData = CSabacMgr::GetInstance().GetNextGuild(nServerGroup);
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoOccupySabac(INT32 nServerGroup, UINT32 dwOldGuildID, UINT32 dwNewGuildID)
{
	INT32 nRetCode = 0;
	G2C_OCCUPY_SABAC *pMsg = (G2C_OCCUPY_SABAC*)m_poPacketAllocator->Alloc(sizeof(G2C_OCCUPY_SABAC));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_occupy_sabac;
	pMsg->dwOldGuildID = dwOldGuildID;
	pMsg->dwNewGuildID = dwNewGuildID;

	nRetCode = _BroadcaseGroupClient(nServerGroup, (PACKET_BUFFER*)pMsg, sizeof(G2C_OCCUPY_SABAC));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSabacStateChange(INT32 nServerGroup, BOOL bStart)
{
	INT32 nRetCode = 0;
	G2C_SABAC_STATE_CHANGE* pMsg = (G2C_SABAC_STATE_CHANGE*)m_poPacketAllocator->Alloc(sizeof(G2C_SABAC_STATE_CHANGE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sabac_state_change;
	pMsg->byStarted = bStart;

	nRetCode = _BroadcaseGroupClient(nServerGroup, (PACKET_BUFFER*)pMsg, sizeof(G2C_SABAC_STATE_CHANGE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSEnrollSabac(UINT32 dwGuildID, UINT32 dwEnrollTime)
{
	INT32 nRetCode = 0;
	InternalMessage::G2M_ENROLL_SABAC *pMsg = 
		(InternalMessage::G2M_ENROLL_SABAC*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ENROLL_SABAC));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_enroll_sabac;
	pMsg->dwGuildID = dwGuildID;
	pMsg->dwEnrollTime = dwEnrollTime;

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ENROLL_SABAC));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSOccupySabac(INT32 nServerGroup, UINT32 dwOldGuildID, UINT32 dwNewGuildID)
{
	INT32       nRetCode = 0;
	InternalMessage::G2M_OCCUPY_SABAC* pMsg =
		(InternalMessage::G2M_OCCUPY_SABAC*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_OCCUPY_SABAC));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_occupy_sabac;
	pMsg->byServerGroup = (UINT8)nServerGroup;
	pMsg->dwOldGuildID = dwOldGuildID;
	pMsg->dwNewGuildID = dwNewGuildID;

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_OCCUPY_SABAC));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSErectStatue(CRole* poRole)
{
	INT32       nRetCode = 0;
	InternalMessage::G2M_ERECT_STATUE* pMsg =
		(InternalMessage::G2M_ERECT_STATUE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ERECT_STATUE));
	LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = InternalMessage::g2m_erect_statue;
    pMsg->dwRoleID = poRole->GetObjID();
	pMsg->byHairType = poRole->GetHairType();
	pMsg->byHairColor = poRole->GetHairColor();
	pMsg->byFaceType = poRole->GetFaceType();
	pMsg->bySkinColor = poRole->GetSkinColor();
	pMsg->byTitleID = poRole->GetTitle().GetEquipedTitleID();
	pMsg->byRefineFx = poRole->GetItemBag()->GetRefineFx();
	poRole->GetItemBag()->GetAppearance(pMsg->anAvatar);

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ERECT_STATUE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

#endif //_ROBOT
