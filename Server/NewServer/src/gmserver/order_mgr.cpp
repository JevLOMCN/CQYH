#include "stdafx.h"
#include "order_mgr.h"
#include "server_def.h"
#include "message_handler.h"
#include "ld_listener.h"

COrderMgr COrderMgr::m_Instance;

COrderMgr & COrderMgr::GetInstance()
{
    return m_Instance;
}

COrderMgr::COrderMgr()
{

}

COrderMgr::~COrderMgr()
{

}

BOOL COrderMgr::Init()
{
    m_nTickCount = 0;
    return TRUE;
}

BOOL COrderMgr::UnInit()
{
    return TRUE;
}

BOOL COrderMgr::MainLoop()
{
    INT32   nRetCode = 0;

    // 4 times / per sec
    //if (m_qwGameFrame % (GAME_FPS / 4) == 0)
    if ( m_nTickCount % (GAME_FPS * 2) == 0)
    {
        if (CLDListener::GetInstance().GetLDRegNum() > 0)
        {
            //for (INT32 i = 1; i < MAX_SERVER_GROUP_COUNT; ++i)
            {
                nRetCode = CMessageHandler::GetInstance().DoMSGetExchangePrice(1);
                LOG_CHECK_ERROR(nRetCode);
            }
        }
    }

    ++m_nTickCount;

    return TRUE;
}
