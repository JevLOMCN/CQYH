#include "message_handler.h"
#include "session_mgr.h"
#include "gm_session_events.h"
#include "log.h"

namespace
{
void close_unknown(ISocketSession* socket)
{
    if(socket) try{socket->Close();}catch(...){CRI("GM orphan socket close failed");}
}
}
void CMessageHandler::OnConnected(ISocketSession* socket)
{
    if(!socket) return;
    auto& manager=CSessionMgr::GetInstance();const auto generation=manager.Generation();
    try
    {
        const auto* session=manager.Connected(socket);
        if(!session){if(!manager.Owns(socket))close_unknown(socket);return;}
        if(!DoRegisterToGM(session->dwSessionID) && manager.Generation()==generation && manager.Owns(socket)) manager.CloseSession(socket);
    }
    catch(...)
    {
        CRI("GM connection callback failed");
        if(manager.Generation()==generation && manager.Owns(socket))
            try{manager.CloseSession(socket);}catch(...){CRI("GM connection cleanup failed");}
    }
}
void CMessageHandler::OnDisconnected(ISocketSession* socket,BOOL)
{
    auto& manager=CSessionMgr::GetInstance();const auto generation=manager.Generation();
    try
    {
        const auto before=manager.Disconnected(socket);
        if(before && manager.Generation()==generation) gm::session_disconnected(*before,generation);
    }
    catch(...){CRI("GM disconnect callback failed");}
}
void CMessageHandler::OnAllPing(PACKET_BUFFER*,UINT32,ISocketSession* socket)
{
    auto& manager=CSessionMgr::GetInstance();
    if(!manager.Refresh(socket) && !manager.Owns(socket)) close_unknown(socket);
}
