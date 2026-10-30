#include "stdafx.h"
#include "bt_manager.h"

//#include "string_def.h"
#include "game_world.h"
#include "bt_action.h"
#include "chat_def.h"

CBTManager CBTManager::ms_Instance;
#define MAX_BT_LAYER	(64)

static constexpr const char* s_szOwnerNameList[botTotal] =
{
    "Invalid",
    "CCharacter",
    "CScene",
};

char const* const g_BTEventName[evtTotal] =
{
    "evtInvalid",

    "evtAIInit",
    "evtAIIdle",
    "evtAIFight",
    "evtAIDie",
    "evtAIRevive",
    "evtAIInteract",
    "",

    "evtSceneInit",
    "evtEnterScene",
    "evtLeaveScene",
    "evtTouchVolume",
    "evtSingleNpcDie",
    "evtMediumGroupDie",
    "evtLargeGroupDie",
    "evtRoleInteract",
    "evtRoleDie",
	"evtBossDie",
    "evtGuildWarBegin",
    "evtGuildWarEnd",

	"evtSabacBeforeStart",
	"evtSabacStart",
	"evtSabacOccupied",
	"evtSabacBeforeEnd",
	"evtSabacEnd",

    "evtGuildBuildingUpdate",
    "evtGuildBuildingUpgrade",
    "evtGuildBuildingDel",

};

CBTManager::CBTManager()
{
    m_nActionIDGenerator = 0;
    m_pCurrentCtx = NULL;
}

CBTManager::~CBTManager()
{
    UnInit();
}

BOOL CBTManager::Init(void)
{
    m_nActionIDGenerator = 0;
    m_pCurrentCtx = NULL;

    registerNodeFunc(_NodeFunc);
    m_nActionIDGenerator = batEnd;

    for (INT32 nIndex = 0; nIndex < btTotal; nIndex++)
    {
        if (ms_ActionFuncList[nIndex].szName[0])
        {
            std::pair<MAP_ACTION_NAME_2_FUNC::iterator, bool> InsRet;

            InsRet = m_mapActionName2Func.insert(std::make_pair(
                ms_ActionFuncList[nIndex].szName, ms_ActionFuncList + nIndex));
            LOG_PROCESS_ERROR(InsRet.second);
        }
        if (ms_ActionFuncList[nIndex].nActionID != btInvalid)
        {
            if (ms_ActionFuncList[nIndex].nActionID != nIndex)
            {
                CRI("error bt action id %d %d %s", nIndex, ms_ActionFuncList[nIndex].nActionID, ms_ActionFuncList[nIndex].szName);
                LOG_PROCESS_ERROR(FALSE);
            }
        }
    }

    LOG_PROCESS_ERROR(sizeof(g_BTEventName) / sizeof(g_BTEventName[0]) == evtTotal);
    return TRUE;
Exit0:

    return FALSE;
}

BOOL CBTManager::UnInit(void)
{
    for (MAP_ACTION_NAME_2_FUNC::iterator action_it = m_mapActionName2Func.begin(); action_it != m_mapActionName2Func.end();)
    {
        m_mapActionName2Func.erase( action_it++ );
    }

    return TRUE;
}

CBTManager& CBTManager::GetInstance(void)
{
    return ms_Instance;
}

BOOL CBTManager::Reload(void)
{

    return FALSE;
}

BTNod* CBTManager::CreateBTNod(BTNod* pParent, INT32 nType, INT32 nTag, const char* pcszName, 
                               INT32 nParam0, INT32 nParam1, INT32 nParam2, INT32 nParam3, INT32 nLuaParam)
{
    BTNod* pResult = NULL;
    INT32 nRetCode = 0;
    const char* pcszActionName = NULL;

    pResult = CGameWorld::GetInstance().NewBTNod();
    LOG_PROCESS_ERROR(pResult);

    pResult->type = nType;
    pResult->nodeTag = nTag;

    //strxcpy(pResult->name, pcszName, BT_NODE_NAME_LEN);
    //if( atoi(pcszName) > 0 && atoi( pcszName ) < btTotal )
    //{
    //    pcszActionName = CBTManager::GetInstance().GetBTActionName( nType );
    //    strxcpy(pResult->name, pcszActionName, BT_NODE_NAME_LEN);
    //}

    pResult->params[0] = nParam0;
    pResult->params[1] = nParam1;
    pResult->params[2] = nParam2;
    pResult->params[3] = nParam3;
    pResult->luaParam = nLuaParam;
    pResult->childrenCount = 0;
    pResult->child = NULL;
    pResult->sibling = NULL;

    if (pParent)
    {
        nRetCode = BTNod_addChild(pParent, pResult);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return pResult;
Exit0:
    if (pResult)
        CGameWorld::GetInstance().DelBTNod(pResult);

    return NULL;
}

BOOL CBTManager::DestroyBT(BTNod* pRoot, INT32 nLayer)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pRoot);
	LOG_PROCESS_ERROR(nLayer < MAX_BT_LAYER);

    if ( pRoot->child )
    {
		nRetCode = DestroyBT(pRoot->child, nLayer + 1);
        LOG_CHECK_ERROR( nRetCode );
    }
    if ( pRoot->sibling )
    {
        nRetCode = DestroyBT(pRoot->sibling, nLayer + 1);
        LOG_CHECK_ERROR( nRetCode );
    }

    nRetCode = CGameWorld::GetInstance().DelBTNod(pRoot);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BTNod* CBTManager::CopyBT( BTNod* pRoot )
{
    BTNod* pResult = NULL;
    INT32 nRetCode = 0;

    PROCESS_ERROR(pRoot);

    pResult = CGameWorld::GetInstance().NewBTNod();
    LOG_PROCESS_ERROR( pResult );

    pResult->type = pRoot->type;
    //strxcpy(pResult->name, pRoot->name, sizeof(pResult->name) );
    memcpy( pResult->params, pRoot->params, sizeof( pResult->params ) );
    pResult->flag = pRoot->flag;
    pResult->childrenCount = pRoot->childrenCount;
    pResult->child = NULL;
    pResult->sibling = NULL;

    if ( pRoot->child )
    {
        pResult->child = CopyBT( pRoot->child );
        LOG_PROCESS_ERROR( pResult->child );
    }
    if ( pRoot->sibling )
    {
        pResult->sibling = CopyBT( pRoot->sibling );
        LOG_PROCESS_ERROR( pResult->sibling );
    }

    return pResult;
Exit0:
    if ( pResult )
    {
        DestroyBT( pResult );
    }
    return NULL;
}

void CBTManager::PrintBT(BTNod* pRoot, INT32 nLayer)
{
    char szTab[PATH_LEN] = {0};
    const char* pcszName = NULL;
    INT32 childIndex = 0;
    BTNod* nod = NULL;

    LOG_PROCESS_ERROR(pRoot);
    LOG_PROCESS_ERROR(nLayer < MAX_BT_LAYER);

    memset(szTab, ' ', nLayer * 4);

    pcszName = GetBTActionName( pRoot->type );
    INF("%s %s(%d,%d,%d,%d)", szTab, pcszName, 
        pRoot->params[0], pRoot->params[1], pRoot->params[2], pRoot->params[3]);

    if ( pRoot->child )
    {
        PrintBT( pRoot->child, nLayer + 1 );
    }
    if ( pRoot->sibling )
    {
        PrintBT( pRoot->sibling, nLayer );
    }

Exit0:
    return;
}

INT32 CBTManager::FindBTActionByName(const char* pcszName)
{
    MAP_ACTION_NAME_2_FUNC::iterator it;

    LOG_PROCESS_ERROR(pcszName);

    it  = m_mapActionName2Func.find(pcszName);
    PROCESS_ERROR(it != m_mapActionName2Func.end());
    PROCESS_ERROR(it->second);

    return it->second->nActionID;
Exit0:
    return btInvalid;
}

INT32 CBTManager::RegisterBTAction(const char* pcszName)
{
    std::pair<MAP_ACTION_NAME_2_FUNC::iterator, bool> InsRet;

    LOG_PROCESS_ERROR(pcszName);

    ms_ActionFuncList[m_nActionIDGenerator].nActionID = m_nActionIDGenerator;
    strxcpy(ms_ActionFuncList[m_nActionIDGenerator].szName, pcszName, SCRIPT_NAME_LEN);
    ms_ActionFuncList[m_nActionIDGenerator].bLuaFunc = TRUE;
    ms_ActionFuncList[m_nActionIDGenerator].eOwnerMask = botAll;

    InsRet = m_mapActionName2Func.insert(std::make_pair(
        ms_ActionFuncList[m_nActionIDGenerator].szName, ms_ActionFuncList + m_nActionIDGenerator));
    LOG_PROCESS_ERROR(InsRet.second);

    return m_nActionIDGenerator++;
Exit0:
    return btInvalid;
}

const char* CBTManager::GetBTActionName( INT32 nActionID )
{
    ACTION_FUNC_ENTRY *pAction = NULL;
    LOG_PROCESS_ERROR( nActionID > 0 && nActionID < btTotal );

    pAction = &ms_ActionFuncList[ nActionID ];
    LOG_PROCESS_ERROR( pAction->nActionID > 0 );
    LOG_PROCESS_ERROR( pAction->szName[0] != '\0' );

    return pAction->szName;
Exit0:
    return NULL;
}

BOOL CBTManager::CallBTAction(INT32 nBTNodeType, INT32 nParam[])
{
    INT32 nResult = brvError;

    LOG_PROCESS_ERROR(m_pCurrentCtx);
    LOG_PROCESS_ERROR(nBTNodeType > btInvalid && nBTNodeType < btTotal);
    LOG_PROCESS_ERROR(ms_ActionFuncList[nBTNodeType].eOwnerMask & (1 << m_pCurrentCtx->eOwnerType));
    LOG_PROCESS_ERROR(!ms_ActionFuncList[nBTNodeType].bLuaFunc);
    LOG_PROCESS_ERROR(ms_ActionFuncList[nBTNodeType].pCFunc);

    nResult = ms_ActionFuncList[nBTNodeType].pCFunc(m_pCurrentCtx, nParam);

Exit0:
    return nResult;
}

INT32 CBTManager::TestBTAction(CRole* poRole, INT32 nBTNodeType, INT32 nParam[])
{
    INT32 nResult = brvError;
    INT32 nMask = 0;

    BTCustomCtx Ctx;
    BTNod* pRoot = NULL;
    CBTManager& rInstance = CBTManager::GetInstance();

    LOG_PROCESS_ERROR(poRole);

    pRoot = rInstance.CreateBTNod(NULL, nBTNodeType, 0, NULL, nParam[0], nParam[1], nParam[2], nParam[3], 0);
    LOG_PROCESS_ERROR(pRoot);

    memset(&Ctx, 0, sizeof(Ctx) );
    Ctx.poScript = NULL;
    Ctx.eRunningType = brtInvalid;

    nMask = rInstance.ms_ActionFuncList[nBTNodeType].eOwnerMask;
    if (nMask & botSceneBit)
    {
        Ctx.poScene = poRole->GetScene();
        Ctx.eOwnerType = botScene;
    }
    else if (nMask & botAIBit)
    {
        Ctx.poChar = poRole;
        Ctx.eOwnerType = botAI;;
    }
    LOG_PROCESS_ERROR(Ctx.poOwner);

    memset(Ctx.callFrame, 0, sizeof(Ctx.callFrame) );
    Ctx.callFrame[0].nod = pRoot;
    Ctx.callFrameCount = 1;

    nResult = CBTManager::_NodeFunc( &Ctx, pRoot );
    DBG( "bt action %d[%s]'s run result = %d", nBTNodeType, GetBTActionName( nBTNodeType ), nResult );

Exit0:
    if ( pRoot )
        rInstance.DestroyBT( pRoot );

    return nResult;
}

BOOL CBTManager::GetCtxRoleID()
{
    INT32 nRetCode = 0;
    LOG_PROCESS_ERROR(m_pCurrentCtx);
    return m_pCurrentCtx->dwRoleID;
Exit0:
    return 0;
}

INT32 CBTManager::_NodeFunc(BTCtx* ctx, BTNod* nod)
{
    INT32 nResult = brvError;
    INT32 nRetCode = 0;
    INT32 nParam[MAX_BT_PARAM] = {0};
    BTCustomCtx* pCtx = (BTCustomCtx*)ctx;
    CBTManager& rInstance = CBTManager::GetInstance();

    LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(nod);
    LOG_PROCESS_ERROR(nod->type > btInvalid && nod->type < btTotal);
    LOG_PROCESS_ERROR(rInstance.ms_ActionFuncList[nod->type].eOwnerMask & (1 << pCtx->eOwnerType));

    // prepare params
    for (INT32 nParamIndex = 0; nParamIndex < MAX_BT_PARAM; nParamIndex++)
    {
        if (nod->luaParam & (1 << nParamIndex))
        {
            char szFuncName[COMMON_NAME_LEN];
            snprintf(szFuncName, COMMON_NAME_LEN, "ParamFunc%d", nod->params[nParamIndex]);

            LOG_PROCESS_ERROR( pCtx->poScript);
            rInstance.m_pCurrentCtx = pCtx;
            nRetCode = pCtx->poScript->CallFunction(szFuncName, "o:d", 
                pCtx->poOwner, s_szOwnerNameList[pCtx->eOwnerType], nParam + nParamIndex);
            rInstance.m_pCurrentCtx = NULL;
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            nParam[nParamIndex] = nod->params[nParamIndex];
        }
    }

    // call action function
    if (rInstance.ms_ActionFuncList[nod->type].bLuaFunc && 
        rInstance.ms_ActionFuncList[nod->type].szName[0])
    {
        LOG_PROCESS_ERROR( pCtx->poScript);
        rInstance.m_pCurrentCtx = pCtx;
        nRetCode = pCtx->poScript->CallFunction(rInstance.ms_ActionFuncList[nod->type].szName, "odddd:d", 
            pCtx->poOwner, s_szOwnerNameList[pCtx->eOwnerType], nParam[0], nParam[1], nParam[2], nParam[3], &nResult);
        rInstance.m_pCurrentCtx = NULL;
        LOG_PROCESS_ERROR(nRetCode);
    }
    else if (rInstance.ms_ActionFuncList[nod->type].pCFunc)
    {
        nResult = rInstance.ms_ActionFuncList[nod->type].pCFunc(pCtx, nParam);
    }

    if (ctx->debugging)
    {
        char szNodeInfo[256];
        for (INT32 i = 0; i < ctx->callFrameCount - 1; i++)
            szNodeInfo[i] = '\t';
        if (rInstance.GetBTActionName(nod->type))
        {
            snprintf(szNodeInfo + ctx->callFrameCount - 1, 200, "%s(%d, %d, %d, %d):%d", 
                rInstance.GetBTActionName(nod->type), nParam[0], nParam[1], nParam[2], nParam[3], nResult);
            INF(szNodeInfo);
        }
    }

Exit0:
    if (nResult == brvError)
    {
        if (nod && rInstance.GetBTActionName(nod->type) &&
            pCtx && pCtx->poChar && pCtx->poChar->GetAIProcessor())
        {
            WRN("%s(%d, %d, %d, %d) error, ai %d, event %d", 
                rInstance.GetBTActionName(nod->type), nParam[0], nParam[1], nParam[2], nParam[3],
                pCtx->poChar->GetAIProcessor()->GetAIID(), pCtx->eEventType);
            if (g_GWConfig.DebugOption.debugai)
            {
                char szMessage[1024];
                sprintf(szMessage, "%s(%d, %d, %d, %d) error, ai %d, event %d", 
                    rInstance.GetBTActionName(nod->type), nParam[0], nParam[1], nParam[2], nParam[3],
                    pCtx->poChar->GetAIProcessor()->GetAIID(), pCtx->eEventType);
                pCtx->poChar->Say(cctScreen, cltNormal, szMessage);
            }
        }
        else if (nod)
        {
            WRN("run unknown bt node %d(%d, %d, %d, %d) error", nod->type, nod->params[0], nod->params[1], nod->params[2], nod->params[3]);
        }
        else
            WRN("run unknown bt node error, node NULL");
    }
    return nResult;
}
