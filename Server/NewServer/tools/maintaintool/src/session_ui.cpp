#include "stdafx.h"
#include "session_mgr.h"
#include "maintaintool.h"
#include "message_handler.h"

maintain::SessionHooks maintain::make_session_hooks()
{
    return {
        [](INT32 area) { return MaintainTool::GetInstance().CheckAreaExpanded(area) != FALSE; },
        [](SRemoteGuardSession& session, E_MT_CONNECT_STATE state) {
            return MaintainTool::GetInstance().UpdateGuardConnectInfo(&session, state, nullptr) != FALSE;
        },
        [](SRemoteGuardSession& session) { return CMTMessageHandler::GetInstance().DoPing(&session) != FALSE; }};
}
